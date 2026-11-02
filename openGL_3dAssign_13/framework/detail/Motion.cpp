#include "../Framework.h"
#include "Internal.h"
#include <algorithm>
#include <stdexcept>

namespace graphics {
bool Motion::validStep(const MotionStep& step) {
    return bool(step.transform) && std::isfinite(step.startSeconds) && step.startSeconds >= 0 &&
           (!step.durationSeconds || (std::isfinite(*step.durationSeconds) && *step.durationSeconds >= 0 &&
                                      std::isfinite(step.startSeconds + *step.durationSeconds)));
}

bool Motion::append(const MotionStep& step) {
    return insert(steps_.size(), step);
}

bool Motion::insert(std::size_t index, const MotionStep& step) {
    if (index > steps_.size() || !validStep(step))
        return false;
    steps_.insert(steps_.begin() + index, step);

    return true;
}

bool Motion::replace(std::size_t index, const MotionStep& step) {
    if (index >= steps_.size() || !validStep(step))
        return false;
    steps_[index] = step;

    return true;
}

bool Motion::erase(std::size_t index) {
    if (index >= steps_.size())
        return false;
    steps_.erase(steps_.begin() + index);

    return true;
}

bool Motion::then(MotionStep step) {
    double end = 0;

    for (const auto& current : steps_) {
        if (!current.durationSeconds)
            return false;
        end = std::max(end, current.startSeconds + *current.durationSeconds);
    }
    step.startSeconds += end;

    return append(step);
}

std::optional<MotionSample> Motion::sample(double seconds) const {
    if (!std::isfinite(seconds) || seconds < 0)
        return {};

    MotionSample result;

    for (const auto& step : steps_) {
        if (!step.enabled || seconds < step.startSeconds)
            continue;

        double time = seconds - step.startSeconds;

        if (step.durationSeconds)
            time = std::min(time, *step.durationSeconds);

        const auto matrix = step.transform(time);

        if (!TransformSystem::validAffine(matrix))
            return {};

        auto& target = step.space == TransformSpace::World ? result.parent : result.local;

        target = matrix * target; // Within each space, API order is application order.

        if (!TransformSystem::validAffine(target))
            return {};
    }

    return result;
}

MotionStep Motion::fixed(const glm::mat4& matrix, TransformSpace space) {
    MotionStep step;

    if (TransformSystem::validAffine(matrix))
        step.transform = [matrix](double) {
            return matrix;
        };
    step.space = space;
    step.durationSeconds = 0;

    return step;
}

MotionStep Motion::rotation(const RotationMotion& config) {
    MotionStep step;

    if (!std::isfinite(config.degreesPerSecond) || !std::isfinite(config.initialDegrees) ||
        !TransformSystem::rotation(config.axis, config.initialDegrees))
        return step;
    step.space = config.space;
    step.transform = [config](double seconds) {
        const auto matrix =
            TransformSystem::rotation(config.axis, config.initialDegrees + config.degreesPerSecond * seconds);

        if (!matrix)
            throw std::domain_error("Rotation motion produced an invalid matrix.");

        return *matrix;
    };

    return step;
}

MotionStep Motion::followPath(const PathMotion& config) {
    MotionStep step;

    if (!config.path || !config.interpolation || !std::isfinite(config.durationSeconds) ||
        config.durationSeconds <= 0 || !std::isfinite(config.rate) || config.rate == 0 ||
        !std::isfinite(config.phase) || config.phase < 0 || config.phase > 1)
        return step;
    step.space = config.space;

    if (config.playback == Playback::Once) {
        step.durationSeconds =
            (config.rate > 0 ? 1 - config.phase : config.phase) * config.durationSeconds / std::abs(config.rate);

        if (!std::isfinite(*step.durationSeconds))
            return MotionStep{};
    }
    step.transform = [config](double seconds) {
        double progress = config.phase + seconds / config.durationSeconds * config.rate;

        if (!std::isfinite(progress))
            throw std::domain_error("Path time overflow.");

        if (config.playback == Playback::Once)
            progress = std::clamp(progress, 0.0, 1.0);
        else if (config.playback == Playback::Loop)
            progress -= std::floor(progress);
        else {
            progress -= std::floor(progress / 2) * 2;

            if (progress > 1)
                progress = 2 - progress;
        }

        const double u = config.interpolation(progress);

        if (!std::isfinite(u))
            throw std::domain_error("Interpolation returned non-finite progress.");

        glm::vec3 offset = config.path(u);

        if (config.relativeToStart) {
            const double initial = config.interpolation(config.phase);

            if (!std::isfinite(initial))
                throw std::domain_error("Invalid initial interpolation.");
            offset -= config.path(initial);
        }

        const auto matrix = TransformSystem::translation(offset);

        if (!matrix)
            throw std::domain_error("Path returned a non-finite position.");

        return *matrix;
    };

    return step;
}

std::optional<MotionId> MotionSystem::createMotion(const Motion& motion) {
    const auto sample = motion.sample(0);

    if (!sample || nextMotion_ == std::numeric_limits<MotionId>::max())
        return {};

    const auto id = nextMotion_;
    MotionState state;

    state.motion = motion;
    state.sampled = *sample;
    motions_.emplace(id, std::move(state));
    ++nextMotion_;

    return id;
}

const MotionState* MotionSystem::getMotion(MotionId id) const {
    const auto found = motions_.find(id);

    return found == motions_.end() ? nullptr : &found->second;
}

bool MotionSystem::setMotion(MotionId id, const Motion& motion) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return false;

    const auto sample = motion.sample(found->second.timeSeconds);

    if (!sample)
        return false;
    found->second.motion = motion;
    found->second.sampled = *sample;

    return true;
}

bool MotionSystem::setMotionPlaying(MotionId id, bool playing) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return false;
    found->second.playing = playing;

    return true;
}

bool MotionSystem::setMotionEnabled(MotionId id, bool enabled) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return false;
    found->second.enabled = enabled;

    return true;
}

bool MotionSystem::seekMotion(MotionId id, double seconds) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return false;

    const auto sample = found->second.motion.sample(seconds);

    if (!sample)
        return false;
    found->second.timeSeconds = seconds;
    found->second.sampled = *sample;

    return true;
}

bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id))
        return false;

    for (auto& layer : layers_)
        if (layer.second.config.motion == id)
            layer.second.config.motion.reset();

    return true;
}

bool MotionSystem::update(double deltaSeconds) {
    if (!std::isfinite(deltaSeconds) || deltaSeconds < 0)
        return false;

    struct Pending {
        MotionId id;
        double time;
        MotionSample sample;
    };

    std::vector<Pending> pending;

    pending.reserve(motions_.size());

    for (const auto& entry : motions_) {
        const auto& state = entry.second;

        if (!state.playing)
            continue;

        const double time = state.timeSeconds + deltaSeconds;
        const auto sample = state.motion.sample(time);

        if (!sample)
            return false;
        pending.push_back({entry.first, time, *sample});
    }

    for (const auto& item : pending) {
        auto& state = motions_.at(item.id);

        state.timeSeconds = item.time;
        state.sampled = item.sample;
    }

    return true;
}

void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};

    motions_.clear();
    layers_.swap(emptyLayers);
}

bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform) && (!config.motion || getMotion(*config.motion));
}

const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);

    return found == layers_.end() ? nullptr : &found->second;
}

std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max())
        return {};

    auto staged = layers_;
    const auto id = nextLayer_;

    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;

    return id;
}

std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);

    if (!current || !current->parent || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max())
        return {};

    const auto parent = *current->parent;
    const auto id = nextLayer_;
    auto staged = layers_;

    staged.emplace(id, LayerState{config, parent, {child}});

    auto& siblings = staged.at(parent).children;

    std::replace(siblings.begin(), siblings.end(), child, id);
    staged.at(child).parent = id;
    layers_.swap(staged);
    ++nextLayer_;

    return id;
}

bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);

    if (!current || !current->parent)
        return false;

    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());

    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());

    for (auto child : current->children)
        staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);

    return true;
}

std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);

    if (!current || !current->parent || !current->children.empty())
        return {};

    const auto parent = *current->parent;

    if (!removeLayer(top))
        return {};

    return parent;
}

bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);

    if (!current || !current->parent || !getLayer(parent))
        return false;

    if (current->parent == parent)
        return true;

    auto ancestor = std::optional<LayerId>(parent);

    while (ancestor) {
        if (*ancestor == id)
            return false;
        ancestor = layers_.at(*ancestor).parent;
    }

    auto staged = layers_;
    auto& old = staged.at(*current->parent).children;

    old.erase(std::find(old.begin(), old.end(), id));
    staged.at(parent).children.push_back(id);
    staged.at(id).parent = parent;
    layers_.swap(staged);

    return true;
}

bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);

    if (found == layers_.end() || !validLayer(config))
        return false;
    found->second.config = config;

    return true;
}

bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);

    if (!current || (motion && !getMotion(*motion)))
        return false;

    auto config = current->config;

    config.motion = motion;

    return setLayer(layer, config);
}

std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);

    while (current) {
        const auto layer = getLayer(*current);

        if (!layer || chain.size() >= layers_.size())
            return {};
        chain.push_back(*current);
        current = layer->parent;
    }

    MotionSample result;

    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;

        if (!config.enabled)
            continue;

        MotionSample own;

        if (config.space == TransformSpace::World)
            own.parent = config.transform;
        else
            own.local = config.transform;

        if (config.motion) {
            const auto state = getMotion(*config.motion);

            if (!state)
                return {};

            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }

        if (*i == rootLayer())
            result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }

        if (!TransformSystem::validAffine(result.parent) || !TransformSystem::validAffine(result.local))
            return {};
    }

    return result;
}

std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;

    Motion motion;

    if (!motion.append(Motion::rotation(config)))
        return {};

    return createMotion(motion);
}

std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;

    Motion motion;

    if (!motion.append(Motion::rotation(config)))
        return {};

    return createMotion(motion);
}

std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit, RotationMotion spin) {
    orbit.space = TransformSpace::World;
    spin.space = TransformSpace::Local;

    Motion motion;

    if (!motion.append(Motion::rotation(orbit)) || !motion.append(Motion::rotation(spin)))
        return {};

    return createMotion(motion);
}

std::optional<MotionId> MotionSystem::createPathAndSpin(const PathMotion& path, RotationMotion spin) {
    spin.space = TransformSpace::Local;

    Motion motion;

    if (!motion.append(Motion::followPath(path)) || !motion.append(Motion::rotation(spin)))
        return {};

    return createMotion(motion);
}
} // namespace graphics
