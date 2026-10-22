#pragma once
#include <algorithm>
#include <stdexcept>

namespace graphics {
inline bool Motion::validStep(const MotionStep& step) {
    return bool(step.transform) && std::isfinite(step.startSeconds) && step.startSeconds >= 0
        && (!step.durationSeconds || (std::isfinite(*step.durationSeconds)
            && *step.durationSeconds >= 0
            && std::isfinite(step.startSeconds + *step.durationSeconds)));
}
inline bool Motion::append(const MotionStep& step) { return insert(steps_.size(), step); }
inline bool Motion::insert(std::size_t index, const MotionStep& step) {
    if (index > steps_.size() || !validStep(step)) return false;
    steps_.insert(steps_.begin() + index, step);
    return true;
}
inline bool Motion::replace(std::size_t index, const MotionStep& step) {
    if (index >= steps_.size() || !validStep(step)) return false;
    steps_[index] = step;
    return true;
}
inline bool Motion::erase(std::size_t index) {
    if (index >= steps_.size()) return false;
    steps_.erase(steps_.begin() + index);
    return true;
}
inline bool Motion::then(MotionStep step) {
    double end = 0;
    for (const auto& current : steps_) {
        if (!current.durationSeconds) return false;
        end = std::max(end, current.startSeconds + *current.durationSeconds);
    }
    step.startSeconds += end;
    return append(step);
}
inline std::optional<MotionSample> Motion::sample(double seconds) const {
    if (!std::isfinite(seconds) || seconds < 0) return {};
    MotionSample result;
    for (const auto& step : steps_) {
        if (!step.enabled || seconds < step.startSeconds) continue;
        double time = seconds - step.startSeconds;
        if (step.durationSeconds) time = std::min(time, *step.durationSeconds);
        const auto matrix = step.transform(time);
        if (!TransformSystem::validAffine(matrix)) return {};
        auto& target = step.space == TransformSpace::Parent ? result.parent : result.local;
        target = matrix * target; // Within each space, API order is application order.
        if (!TransformSystem::validAffine(target)) return {};
    }
    return result;
}
inline MotionStep Motion::fixed(const glm::mat4& matrix, TransformSpace space) {
    MotionStep step;
    if (TransformSystem::validAffine(matrix)) step.transform = [matrix](double) { return matrix; };
    step.space = space;
    step.durationSeconds = 0;
    return step;
}
inline MotionStep Motion::rotation(const RotationMotion& config) {
    MotionStep step;
    if (!std::isfinite(config.degreesPerSecond) || !std::isfinite(config.initialDegrees)
        || !TransformSystem::rotation(config.axis, config.initialDegrees)) return step;
    step.space = config.space;
    step.transform = [config](double seconds) {
        const auto matrix = TransformSystem::rotation(config.axis,
            config.initialDegrees + config.degreesPerSecond * seconds);
        if (!matrix) throw std::domain_error("Rotation motion produced an invalid matrix.");
        return *matrix;
    };
    return step;
}
inline MotionStep Motion::followPath(const PathMotion& config) {
    MotionStep step;
    if (!config.path || !config.interpolation || !std::isfinite(config.durationSeconds)
        || config.durationSeconds <= 0 || !std::isfinite(config.rate) || config.rate == 0
        || !std::isfinite(config.phase) || config.phase < 0 || config.phase > 1) return step;
    step.space = config.space;
    if (config.playback == Playback::Once) {
        step.durationSeconds = (config.rate > 0 ? 1 - config.phase : config.phase)
                             * config.durationSeconds / std::abs(config.rate);
        if (!std::isfinite(*step.durationSeconds)) return MotionStep{};
    }
    step.transform = [config](double seconds) {
        double progress = config.phase + seconds / config.durationSeconds * config.rate;
        if (!std::isfinite(progress)) throw std::domain_error("Path time overflow.");
        if (config.playback == Playback::Once) progress = std::clamp(progress, 0.0, 1.0);
        else if (config.playback == Playback::Loop) progress -= std::floor(progress);
        else {
            progress -= std::floor(progress / 2) * 2;
            if (progress > 1) progress = 2 - progress;
        }
        const double u = config.interpolation(progress);
        if (!std::isfinite(u)) throw std::domain_error("Interpolation returned non-finite progress.");
        glm::vec3 offset = config.path(u);
        if (config.relativeToStart) {
            const double initial = config.interpolation(config.phase);
            if (!std::isfinite(initial)) throw std::domain_error("Invalid initial interpolation.");
            offset -= config.path(initial);
        }
        const auto matrix = TransformSystem::translation(offset);
        if (!matrix) throw std::domain_error("Path returned a non-finite position.");
        return *matrix;
    };
    return step;
}
inline std::optional<MotionId> MotionSystem::createMotion(const Motion& motion) {
    const auto sample = motion.sample(0);
    if (!sample || nextMotion_ == std::numeric_limits<MotionId>::max()) return {};
    const auto id = nextMotion_;
    MotionState state;
    state.motion = motion;
    state.sampled = *sample;
    motions_.emplace(id, std::move(state));
    ++nextMotion_;
    return id;
}
inline const MotionState* MotionSystem::getMotion(MotionId id) const {
    const auto found = motions_.find(id);
    return found == motions_.end() ? nullptr : &found->second;
}
inline bool MotionSystem::setMotion(MotionId id, const Motion& motion) {
    const auto found = motions_.find(id);
    if (found == motions_.end()) return false;
    const auto sample = motion.sample(found->second.timeSeconds);
    if (!sample) return false;
    found->second.motion = motion;
    found->second.sampled = *sample;
    return true;
}
inline bool MotionSystem::setMotionPlaying(MotionId id, bool playing) {
    const auto found = motions_.find(id);
    if (found == motions_.end()) return false;
    found->second.playing = playing;
    return true;
}
inline bool MotionSystem::setMotionEnabled(MotionId id, bool enabled) {
    const auto found = motions_.find(id);
    if (found == motions_.end()) return false;
    found->second.enabled = enabled;
    return true;
}
inline bool MotionSystem::seekMotion(MotionId id, double seconds) {
    const auto found = motions_.find(id);
    if (found == motions_.end()) return false;
    const auto sample = found->second.motion.sample(seconds);
    if (!sample) return false;
    found->second.timeSeconds = seconds;
    found->second.sampled = *sample;
    return true;
}
inline bool MotionSystem::removeMotion(MotionId id) { return motions_.erase(id) != 0; }
inline bool MotionSystem::update(double deltaSeconds) {
    if (!std::isfinite(deltaSeconds) || deltaSeconds < 0) return false;
    struct Pending { MotionId id; double time; MotionSample sample; };
    std::vector<Pending> pending;
    pending.reserve(motions_.size());
    for (const auto& entry : motions_) {
        const auto& state = entry.second;
        if (!state.playing) continue;
        const double time = state.timeSeconds + deltaSeconds;
        const auto sample = state.motion.sample(time);
        if (!sample) return false;
        pending.push_back({entry.first, time, *sample});
    }
    for (const auto& item : pending) {
        auto& state = motions_.at(item.id);
        state.timeSeconds = item.time;
        state.sampled = item.sample;
    }
    return true;
}
inline void MotionSystem::clear() { motions_.clear(); }
inline std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::Parent;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit,
                                                            RotationMotion spin) {
    orbit.space = TransformSpace::Parent;
    spin.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(orbit)) || !motion.append(Motion::rotation(spin))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createPathAndSpin(const PathMotion& path,
                                                           RotationMotion spin) {
    spin.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::followPath(path)) || !motion.append(Motion::rotation(spin))) return {};
    return createMotion(motion);
}
} // namespace graphics
