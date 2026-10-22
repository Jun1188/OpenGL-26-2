# Motion·변환·레이어 전체 코드 변경 전후

- 날짜: 2026-10-22. 빌드·테스트·실행·벤치마크는 수행하지 않았다.
- 코드 before 기준: 작업 시작 시 공통 코드가 깨끗했던 Git c3083408720cbfb4033b6440c249767cad7e6dd0. after는 최종 저장된 코드다.
- 단계 1: 81c1070 — 기본 변환·경로·Motion. 단계 2: 0494202 — 레이어 트리·Scene·렌더 연결. 단계 3: 예제·문서·AST 구조 갱신(이 문서와 같은 커밋).
- 기존 실습 Console/main/project 변경은 수정·스테이징하지 않았다. 두 루트 문서의 선행 미커밋 확장도 보존하고 이번 추가 절만 별도 스테이징한다.
- 아래는 모든 수정·추가 코드 파일을 각각 열거한 전체 변경 블록이다. 새로운 파일은 before 없음, after 전체 코드다. 기존 파일은 차이가 있는 모든 구간을 앞뒤 3줄과 함께 제시한다. 문서·그래프 데이터는 뒤의 범위와 링크를 참고한다.

## framework/include/Transform.h

축·반사 행렬과 경로 함수의 공개 계약을 분리한다.

before: 없음

after:

```cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {

struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

using PathFunction = std::function<glm::vec3(double)>;
using InterpolationFunction = std::function<double(double)>;

// These functions return no matrix/path when their parameters are invalid.
class TransformSystem {
public:
    static bool validAffine(const glm::mat4& matrix);
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    static std::optional<glm::mat4> scaling(const glm::vec3& factors,
                                           const glm::vec3& center = glm::vec3(0));
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point,
                                               const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU,
                                const glm::vec3& axisV);
    static PathFunction circle(const glm::vec3& center, float radius,
                               const glm::vec3& normal = glm::vec3(0, 1, 0));
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    static PathFunction polygon(std::vector<glm::vec3> points);
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU,
                                  const glm::vec3& halfV);
    static double linear(double progress);
    static double smoothStep(double progress);
};
} // namespace graphics
#include "../detail/Transform.inl"
```

## framework/detail/Transform.inl

점·직선·평면 대칭과 회전, 길이 기반 선분 경로 및 원·타원 계산을 구현한다.

before: 없음

after:

```cpp
#pragma once
#include <algorithm>

namespace graphics {
namespace detail {
inline std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction)) return {};
    const double length = glm::length(glm::dvec3(direction));
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return glm::vec3(glm::dvec3(direction) / length);
}
inline std::optional<glm::mat4> motionAround(const glm::vec3& center,
                                           const glm::mat4& matrix) {
    if (!finiteVector(center)) return {};
    const auto result = glm::translate(glm::mat4(1), center) * matrix
                      * glm::translate(glm::mat4(1), -center);
    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result)
                                                : std::nullopt;
}
inline bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);
    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

inline bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0
        && m[2][3] == 0 && m[3][3] == 1;
}
inline std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset)) return {};
    return glm::translate(glm::mat4(1), offset);
}
inline std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors,
                                                       const glm::vec3& center) {
    if (!detail::finiteVector(factors)) return {};
    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}
inline std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis,
                                                        double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);
    if (!unit || !std::isfinite(degrees)) return {};
    const float radians = static_cast<float>(std::remainder(degrees, 360.0)
                                            * 3.14159265358979323846 / 180.0);
    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}
inline std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}
inline std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);
    if (!unit) return {};
    glm::mat3 matrix(0);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);
    return detail::motionAround(line.point, glm::mat4(matrix));
}
inline std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point,
                                                            const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);
    if (!unit) return {};
    glm::mat3 matrix(1);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];
    return detail::motionAround(point, glm::mat4(matrix));
}
inline PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u,
                                       const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;
        return center + u * static_cast<float>(std::cos(angle))
                      + v * static_cast<float>(std::sin(angle));
    };
}
inline PathFunction PathSystem::circle(const glm::vec3& center, float radius,
                                      const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);
    if (!n || !std::isfinite(radius) || radius <= 0) return {};
    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));
    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}
inline PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2) return {};
    for (const auto& p : points) if (!detail::finiteVector(p)) return {};
    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());
        if (points.size() < 4) return {};
    }
    std::vector<double> distances{0};
    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));
        if (!std::isfinite(length) || length <= 1e-12) return {};
        distances.push_back(distances.back() + length);
    }
    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1
                                                   : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);
        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight)
                       + glm::dvec3(points[i]) * weight);
    };
}
inline PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}
inline PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u,
                                         const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);
    if (std::abs(glm::dot(*a, *b)) > 1e-5f) return {};
    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}
inline double PathSystem::linear(double progress) { return progress; }
inline double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics
```

## framework/include/Motion.h

동작·레이어 식별자, 설정, 시간·합성·트리 편집 API를 공개한다.

before: 없음

after:

```cpp
#pragma once
#include "Transform.h"
#include <map>

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};
struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
};

class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
public:
    bool append(const MotionStep& step);
    bool insert(std::size_t index, const MotionStep& step);
    bool replace(std::size_t index, const MotionStep& step);
    bool erase(std::size_t index);
    bool then(MotionStep step); // Requires all previous steps to have finite durations.
    const std::vector<MotionStep>& getSteps() const { return steps_; }
    std::optional<MotionSample> sample(double seconds) const;
    static MotionStep fixed(const glm::mat4& matrix,
                            TransformSpace space = TransformSpace::World);
    static MotionStep rotation(const RotationMotion& config);
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};
struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;
public:
    static constexpr LayerId rootLayer() { return 0; }
    std::optional<MotionId> createMotion(const Motion& motion);
    bool setMotion(MotionId id, const Motion& motion);
    const MotionState* getMotion(MotionId id) const;
    bool setMotionPlaying(MotionId id, bool playing);
    bool setMotionEnabled(MotionId id, bool enabled);
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id);
    bool update(double deltaSeconds);
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    bool reparentLayer(LayerId id, LayerId parent);
    bool setLayer(LayerId id, const LayerConfig& config);
    const LayerState* getLayer(LayerId id) const;
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    std::optional<MotionId> createSpin(RotationMotion config);
    std::optional<MotionId> createOrbit(RotationMotion config);
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
#include "../detail/Motion.inl"
```

## framework/detail/Motion.inl

원자적 시각 반영, 동작 합성, 공유 시계, 레이어 편집과 행렬 합성을 구현한다.

before: 없음

after:

```cpp
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
        auto& target = step.space == TransformSpace::World ? result.parent : result.local;
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
inline bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id)) return false;
    for (auto& layer : layers_)
        if (layer.second.config.motion == id) layer.second.config.motion.reset();
    return true;
}
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
inline void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};
    motions_.clear();
    layers_.swap(emptyLayers);
}
inline bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform)
        && (!config.motion || getMotion(*config.motion));
}
inline const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);
    return found == layers_.end() ? nullptr : &found->second;
}
inline std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
    auto staged = layers_;
    const auto id = nextLayer_;
    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;
    return id;
}
inline std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);
    if (!current || !current->parent || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
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
inline bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);
    if (!current || !current->parent) return false;
    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());
    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());
    for (auto child : current->children) staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);
    return true;
}
inline std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);
    if (!current || !current->parent || !current->children.empty()) return {};
    const auto parent = *current->parent;
    if (!removeLayer(top)) return {};
    return parent;
}
inline bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);
    if (!current || !current->parent || !getLayer(parent)) return false;
    if (current->parent == parent) return true;
    auto ancestor = std::optional<LayerId>(parent);
    while (ancestor) {
        if (*ancestor == id) return false;
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
inline bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);
    if (found == layers_.end() || !validLayer(config)) return false;
    found->second.config = config;
    return true;
}
inline bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);
    if (!current || (motion && !getMotion(*motion))) return false;
    auto config = current->config;
    config.motion = motion;
    return setLayer(layer, config);
}
inline std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);
    while (current) {
        const auto layer = getLayer(*current);
        if (!layer || chain.size() >= layers_.size()) return {};
        chain.push_back(*current);
        current = layer->parent;
    }
    MotionSample result;
    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;
        if (!config.enabled) continue;
        MotionSample own;
        if (config.space == TransformSpace::World) own.parent = config.transform;
        else own.local = config.transform;
        if (config.motion) {
            const auto state = getMotion(*config.motion);
            if (!state) return {};
            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }
        if (*i == rootLayer()) result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }
        if (!TransformSystem::validAffine(result.parent)
            || !TransformSystem::validAffine(result.local)) return {};
    }
    return result;
}
inline std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit,
                                                            RotationMotion spin) {
    orbit.space = TransformSpace::World;
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
```

## framework/include/Graphics.h

Scene이 MotionSystem과 바인딩을 소유하고 공개 API로 연결한다.

### 블록 1: before L1~6, after L1~7

before:

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

```

after:

```cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

```

### 블록 2: before L24~29, after L25~32

before:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
```

after:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;
    void rebuildFaceIds();

public:
```

### 블록 3: before L52~57, after L55~67

before:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

after:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
    MotionSystem& getMotions() { return motions_; }
    const MotionSystem& getMotions() const { return motions_; }
    bool bindEntity(EntityId id, LayerId layer);
    bool unbindEntity(EntityId id);
    std::optional<LayerId> getEntityLayer(EntityId id) const;
    bool updateMotions(double deltaSeconds) { return motions_.update(deltaSeconds); }
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

## framework/detail/Scene.inl

기존 객체 SRT와 단일 레이어 가지를 합성하여 면·선·피킹에 같은 결과를 제공한다.

### 블록 1: before L212~239, after L212~274

before:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    glm::mat4 result(1);
    std::optional<EntityId> current = id;
    std::size_t remaining = components_.size();
    while (current) {
        if (*current >= components_.size() || remaining-- == 0) return {};
        const auto& component = components_[*current];
        result = detail::transformMatrix(component.transform) * result;
        if (!detail::finiteMatrix(result)) return {};
        current = component.parent;
    }
    return result;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto& component = components_[id];
    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
    if (component.parent) {
        const auto parent = worldMatrix(*component.parent);
        if (!parent) return {};
        result = *parent * result;
    }
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

after:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;
    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size()) return {};
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());
    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;
    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);
        if (binding) { layer = *binding; anchor = i - 1; break; }
    }
    const auto motion = motions_.layerMatrices(layer);
    if (!motion) return {};
    glm::mat4 prefix(1), suffix(1);
    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
    }
    const auto result = motion->parent * prefix * motion->local * suffix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto world = worldMatrix(id);
    if (!world) return {};
    const auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 child(1);
    if (face.independence)
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * detail::transformMatrix(face.transform);
    const auto result = *world * child * face.localAnimationMatrix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size()) return {};
    if (lines_[id].owner) return worldMatrix(*lines_[id].owner);
    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());
    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}
inline bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer)) return false;
    entityLayers_[id] = layer;
    return true;
}
inline bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size()) return false;
    entityLayers_.erase(id);
    return true;
}
inline std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto found = entityLayers_.find(id);
    if (found == entityLayers_.end() || !motions_.getLayer(found->second)) return {};
    return found->second;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

### 블록 2: before L324~329, after L359~366

before:

```cpp
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

after:

```cpp
}

inline void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

## framework/detail/Renderer.inl

소유자 없는 선에도 루트 행렬을 적용하고 소유 선은 객체와 같은 변환을 사용한다.

### 블록 1: before L152~159, after L152~158

before:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

after:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

### 블록 2: before L184~190, after L183~190

before:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

after:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

## framework/detail/Application.inl

사용자 update 후 렌더 전에 Motion을 한 번 갱신한다. 실패 시 기존 자원 정리 경로로 종료한다.

### 블록 1: before L172~177, after L172~182

before:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

after:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

## framework/examples/motion.cpp

사용자 확인용 독립 예제로 경로·공유 동작·반사·레이어 편집·피킹을 연결한다.

before: 없음

after:

```cpp
#include "../Framework.h"
#include <iostream>

int main() {
    using namespace graphics;
    Application app;
    std::optional<MotionId> pathId, spinId, globalId;
    std::optional<LayerId> mirrorLayer, normalSpinLayer, insertedLayer;
    bool paused = false, spinEnabled = true, globalEnabled = false;
    int reflection = 1;

    const auto choosePath = [&](int kind) {
        PathMotion path;
        path.durationSeconds = 6;
        const glm::vec3 center(1.7f, 0, 0);
        if (kind == 1) path.path = PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 2) path.path = PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 3)
            path.path = PathSystem::polygon({{1, -.5f, 0}, {2.4f, -.4f, 0},
                {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
        else if (kind == 4) {
            path.path = [center](double u) {
                const double angle = u * 6.283185307179586;
                return center + glm::vec3(float(.7 * std::sin(2 * angle)),
                    float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
            };
            path.interpolation = PathSystem::smoothStep;
        } else if (kind == 5) path.path = PathSystem::circle(center, .65f, {0, 0, 1});
        else {
            path.path = PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
            path.playback = Playback::PingPong;
        }
        Motion motion;
        auto& motions = app.getScene().getMotions();
        return pathId && motion.append(Motion::followPath(path))
            && motions.setMotion(*pathId, motion) && motions.seekMotion(*pathId, 0);
    };

    app.setInitHandler([&](Application& current) {
        auto& scene = current.getScene();
        auto& motions = scene.getMotions();
        const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        if (!first || !second) return false;
        if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) return false;
        pathId = motions.createMotion(Motion{});
        RotationMotion spin;
        spin.degreesPerSecond = 90;
        spinId = motions.createSpin(spin);
        RotationMotion global;
        global.degreesPerSecond = 15;
        globalId = motions.createOrbit(global);
        if (!pathId || !spinId || !globalId || !choosePath(1)) return false;
        if (!motions.setMotionEnabled(*globalId, false)
            || !motions.bindMotion(MotionSystem::rootLayer(), *globalId)) return false;

        const auto normalPath = motions.pushLayer(MotionSystem::rootLayer());
        const auto reflectionMatrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
        if (!normalPath || !reflectionMatrix) return false;
        LayerConfig mirror;
        mirror.transform = *reflectionMatrix;
        mirrorLayer = motions.pushLayer(MotionSystem::rootLayer(), mirror);
        if (!mirrorLayer) return false;
        const auto mirroredPath = motions.pushLayer(*mirrorLayer);
        normalSpinLayer = motions.pushLayer(*normalPath);
        if (!mirroredPath || !normalSpinLayer) return false;
        const auto mirroredSpin = motions.pushLayer(*mirroredPath);
        if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId)
            || !motions.bindMotion(*mirroredPath, *pathId)
            || !motions.bindMotion(*normalSpinLayer, *spinId)
            || !motions.bindMotion(*mirroredSpin, *spinId)
            || !scene.bindEntity(*first, *normalSpinLayer)
            || !scene.bindEntity(*second, *mirroredSpin)) return false;
        if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
            || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
            || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) return false;
        current.getCamera().position = {5, 3, 8};
        std::cout << "1 타원, 2 사각형, 3 다각형, 4 사용자 경로+보간, 5 원, 6 왕복 꺾은선\n"
                  << "Space 시간 정지/재개, S 자전 표시, G 전체 공전, M 반사 변경, I 레이어 삽입/삭제\n"
                  << "M: 없음 -> YZ평면 반사 -> 점대칭 -> Y직선 대칭 순환. 클릭: 객체 ID. Esc 종료\n";
        return true;
    });
    app.setKeyHandler([&](Application& current, int key, int action) {
        if (action != GLFW_PRESS) return;
        auto& motions = current.getScene().getMotions();
        bool ok = true;
        if (key == GLFW_KEY_ESCAPE) current.requestClose();
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) ok = choosePath(key - GLFW_KEY_1 + 1);
        else if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            ok = motions.setMotionPlaying(*pathId, !paused)
                && motions.setMotionPlaying(*spinId, !paused)
                && motions.setMotionPlaying(*globalId, !paused);
        } else if (key == GLFW_KEY_S) {
            spinEnabled = !spinEnabled;
            ok = motions.setMotionEnabled(*spinId, spinEnabled);
        } else if (key == GLFW_KEY_G) {
            globalEnabled = !globalEnabled;
            ok = motions.setMotionEnabled(*globalId, globalEnabled);
        } else if (key == GLFW_KEY_M) {
            reflection = (reflection + 1) % 4;
            std::optional<glm::mat4> matrix = glm::mat4(1);
            if (reflection == 1) matrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
            if (reflection == 2) matrix = TransformSystem::reflectPoint({0, 0, 0});
            if (reflection == 3) matrix = TransformSystem::reflectLine(AxisLine{});
            LayerConfig config;
            if (!matrix) ok = false;
            else { config.transform = *matrix; ok = motions.setLayer(*mirrorLayer, config); }
        } else if (key == GLFW_KEY_I) {
            if (insertedLayer) {
                ok = motions.removeLayer(*insertedLayer);
                if (ok) insertedLayer.reset();
            } else {
                LayerConfig config;
                config.space = TransformSpace::Local;
                const auto scale = TransformSystem::scaling(glm::vec3(1.5f));
                if (!scale) ok = false;
                else {
                    config.transform = *scale;
                    insertedLayer = motions.insertLayer(*normalSpinLayer, config);
                    ok = insertedLayer.has_value();
                }
            }
        }
        if (!ok) current.requestClose();
    });
    app.setMouseHandler([](Application& current, const MouseEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
        const auto hit = current.pickAt(event.windowX, event.windowY);
        if (hit) std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    });
    ApplicationConfig config;
    config.title = "Motion and layer tree";
    return app.run(config);
}
```

## openGL_3dAssign_13/framework/include/Transform.h

축·반사 행렬과 경로 함수의 공개 계약을 분리한다.

before: 없음

after:

```cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {

struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

using PathFunction = std::function<glm::vec3(double)>;
using InterpolationFunction = std::function<double(double)>;

// These functions return no matrix/path when their parameters are invalid.
class TransformSystem {
public:
    static bool validAffine(const glm::mat4& matrix);
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    static std::optional<glm::mat4> scaling(const glm::vec3& factors,
                                           const glm::vec3& center = glm::vec3(0));
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point,
                                               const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU,
                                const glm::vec3& axisV);
    static PathFunction circle(const glm::vec3& center, float radius,
                               const glm::vec3& normal = glm::vec3(0, 1, 0));
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    static PathFunction polygon(std::vector<glm::vec3> points);
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU,
                                  const glm::vec3& halfV);
    static double linear(double progress);
    static double smoothStep(double progress);
};
} // namespace graphics
#include "../detail/Transform.inl"
```

## openGL_3dAssign_13/framework/detail/Transform.inl

점·직선·평면 대칭과 회전, 길이 기반 선분 경로 및 원·타원 계산을 구현한다.

before: 없음

after:

```cpp
#pragma once
#include <algorithm>

namespace graphics {
namespace detail {
inline std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction)) return {};
    const double length = glm::length(glm::dvec3(direction));
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return glm::vec3(glm::dvec3(direction) / length);
}
inline std::optional<glm::mat4> motionAround(const glm::vec3& center,
                                           const glm::mat4& matrix) {
    if (!finiteVector(center)) return {};
    const auto result = glm::translate(glm::mat4(1), center) * matrix
                      * glm::translate(glm::mat4(1), -center);
    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result)
                                                : std::nullopt;
}
inline bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);
    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

inline bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0
        && m[2][3] == 0 && m[3][3] == 1;
}
inline std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset)) return {};
    return glm::translate(glm::mat4(1), offset);
}
inline std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors,
                                                       const glm::vec3& center) {
    if (!detail::finiteVector(factors)) return {};
    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}
inline std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis,
                                                        double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);
    if (!unit || !std::isfinite(degrees)) return {};
    const float radians = static_cast<float>(std::remainder(degrees, 360.0)
                                            * 3.14159265358979323846 / 180.0);
    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}
inline std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}
inline std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);
    if (!unit) return {};
    glm::mat3 matrix(0);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);
    return detail::motionAround(line.point, glm::mat4(matrix));
}
inline std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point,
                                                            const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);
    if (!unit) return {};
    glm::mat3 matrix(1);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];
    return detail::motionAround(point, glm::mat4(matrix));
}
inline PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u,
                                       const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;
        return center + u * static_cast<float>(std::cos(angle))
                      + v * static_cast<float>(std::sin(angle));
    };
}
inline PathFunction PathSystem::circle(const glm::vec3& center, float radius,
                                      const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);
    if (!n || !std::isfinite(radius) || radius <= 0) return {};
    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));
    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}
inline PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2) return {};
    for (const auto& p : points) if (!detail::finiteVector(p)) return {};
    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());
        if (points.size() < 4) return {};
    }
    std::vector<double> distances{0};
    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));
        if (!std::isfinite(length) || length <= 1e-12) return {};
        distances.push_back(distances.back() + length);
    }
    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1
                                                   : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);
        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight)
                       + glm::dvec3(points[i]) * weight);
    };
}
inline PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}
inline PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u,
                                         const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);
    if (std::abs(glm::dot(*a, *b)) > 1e-5f) return {};
    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}
inline double PathSystem::linear(double progress) { return progress; }
inline double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics
```

## openGL_3dAssign_13/framework/include/Motion.h

동작·레이어 식별자, 설정, 시간·합성·트리 편집 API를 공개한다.

before: 없음

after:

```cpp
#pragma once
#include "Transform.h"
#include <map>

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};
struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
};

class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
public:
    bool append(const MotionStep& step);
    bool insert(std::size_t index, const MotionStep& step);
    bool replace(std::size_t index, const MotionStep& step);
    bool erase(std::size_t index);
    bool then(MotionStep step); // Requires all previous steps to have finite durations.
    const std::vector<MotionStep>& getSteps() const { return steps_; }
    std::optional<MotionSample> sample(double seconds) const;
    static MotionStep fixed(const glm::mat4& matrix,
                            TransformSpace space = TransformSpace::World);
    static MotionStep rotation(const RotationMotion& config);
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};
struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;
public:
    static constexpr LayerId rootLayer() { return 0; }
    std::optional<MotionId> createMotion(const Motion& motion);
    bool setMotion(MotionId id, const Motion& motion);
    const MotionState* getMotion(MotionId id) const;
    bool setMotionPlaying(MotionId id, bool playing);
    bool setMotionEnabled(MotionId id, bool enabled);
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id);
    bool update(double deltaSeconds);
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    bool reparentLayer(LayerId id, LayerId parent);
    bool setLayer(LayerId id, const LayerConfig& config);
    const LayerState* getLayer(LayerId id) const;
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    std::optional<MotionId> createSpin(RotationMotion config);
    std::optional<MotionId> createOrbit(RotationMotion config);
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
#include "../detail/Motion.inl"
```

## openGL_3dAssign_13/framework/detail/Motion.inl

원자적 시각 반영, 동작 합성, 공유 시계, 레이어 편집과 행렬 합성을 구현한다.

before: 없음

after:

```cpp
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
        auto& target = step.space == TransformSpace::World ? result.parent : result.local;
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
inline bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id)) return false;
    for (auto& layer : layers_)
        if (layer.second.config.motion == id) layer.second.config.motion.reset();
    return true;
}
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
inline void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};
    motions_.clear();
    layers_.swap(emptyLayers);
}
inline bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform)
        && (!config.motion || getMotion(*config.motion));
}
inline const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);
    return found == layers_.end() ? nullptr : &found->second;
}
inline std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
    auto staged = layers_;
    const auto id = nextLayer_;
    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;
    return id;
}
inline std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);
    if (!current || !current->parent || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
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
inline bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);
    if (!current || !current->parent) return false;
    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());
    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());
    for (auto child : current->children) staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);
    return true;
}
inline std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);
    if (!current || !current->parent || !current->children.empty()) return {};
    const auto parent = *current->parent;
    if (!removeLayer(top)) return {};
    return parent;
}
inline bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);
    if (!current || !current->parent || !getLayer(parent)) return false;
    if (current->parent == parent) return true;
    auto ancestor = std::optional<LayerId>(parent);
    while (ancestor) {
        if (*ancestor == id) return false;
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
inline bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);
    if (found == layers_.end() || !validLayer(config)) return false;
    found->second.config = config;
    return true;
}
inline bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);
    if (!current || (motion && !getMotion(*motion))) return false;
    auto config = current->config;
    config.motion = motion;
    return setLayer(layer, config);
}
inline std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);
    while (current) {
        const auto layer = getLayer(*current);
        if (!layer || chain.size() >= layers_.size()) return {};
        chain.push_back(*current);
        current = layer->parent;
    }
    MotionSample result;
    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;
        if (!config.enabled) continue;
        MotionSample own;
        if (config.space == TransformSpace::World) own.parent = config.transform;
        else own.local = config.transform;
        if (config.motion) {
            const auto state = getMotion(*config.motion);
            if (!state) return {};
            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }
        if (*i == rootLayer()) result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }
        if (!TransformSystem::validAffine(result.parent)
            || !TransformSystem::validAffine(result.local)) return {};
    }
    return result;
}
inline std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit,
                                                            RotationMotion spin) {
    orbit.space = TransformSpace::World;
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
```

## openGL_3dAssign_13/framework/include/Graphics.h

Scene이 MotionSystem과 바인딩을 소유하고 공개 API로 연결한다.

### 블록 1: before L1~6, after L1~7

before:

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

```

after:

```cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

```

### 블록 2: before L24~29, after L25~32

before:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
```

after:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;
    void rebuildFaceIds();

public:
```

### 블록 3: before L52~57, after L55~67

before:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

after:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
    MotionSystem& getMotions() { return motions_; }
    const MotionSystem& getMotions() const { return motions_; }
    bool bindEntity(EntityId id, LayerId layer);
    bool unbindEntity(EntityId id);
    std::optional<LayerId> getEntityLayer(EntityId id) const;
    bool updateMotions(double deltaSeconds) { return motions_.update(deltaSeconds); }
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

## openGL_3dAssign_13/framework/detail/Scene.inl

기존 객체 SRT와 단일 레이어 가지를 합성하여 면·선·피킹에 같은 결과를 제공한다.

### 블록 1: before L212~239, after L212~274

before:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    glm::mat4 result(1);
    std::optional<EntityId> current = id;
    std::size_t remaining = components_.size();
    while (current) {
        if (*current >= components_.size() || remaining-- == 0) return {};
        const auto& component = components_[*current];
        result = detail::transformMatrix(component.transform) * result;
        if (!detail::finiteMatrix(result)) return {};
        current = component.parent;
    }
    return result;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto& component = components_[id];
    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
    if (component.parent) {
        const auto parent = worldMatrix(*component.parent);
        if (!parent) return {};
        result = *parent * result;
    }
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

after:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;
    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size()) return {};
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());
    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;
    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);
        if (binding) { layer = *binding; anchor = i - 1; break; }
    }
    const auto motion = motions_.layerMatrices(layer);
    if (!motion) return {};
    glm::mat4 prefix(1), suffix(1);
    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
    }
    const auto result = motion->parent * prefix * motion->local * suffix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto world = worldMatrix(id);
    if (!world) return {};
    const auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 child(1);
    if (face.independence)
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * detail::transformMatrix(face.transform);
    const auto result = *world * child * face.localAnimationMatrix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size()) return {};
    if (lines_[id].owner) return worldMatrix(*lines_[id].owner);
    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());
    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}
inline bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer)) return false;
    entityLayers_[id] = layer;
    return true;
}
inline bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size()) return false;
    entityLayers_.erase(id);
    return true;
}
inline std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto found = entityLayers_.find(id);
    if (found == entityLayers_.end() || !motions_.getLayer(found->second)) return {};
    return found->second;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

### 블록 2: before L324~329, after L359~366

before:

```cpp
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

after:

```cpp
}

inline void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

## openGL_3dAssign_13/framework/detail/Renderer.inl

소유자 없는 선에도 루트 행렬을 적용하고 소유 선은 객체와 같은 변환을 사용한다.

### 블록 1: before L152~159, after L152~158

before:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

after:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

### 블록 2: before L184~190, after L183~190

before:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

after:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

## openGL_3dAssign_13/framework/detail/Application.inl

사용자 update 후 렌더 전에 Motion을 한 번 갱신한다. 실패 시 기존 자원 정리 경로로 종료한다.

### 블록 1: before L172~177, after L172~182

before:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

after:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

## openGL_3dAssign_13/framework/examples/motion.cpp

사용자 확인용 독립 예제로 경로·공유 동작·반사·레이어 편집·피킹을 연결한다.

before: 없음

after:

```cpp
#include "../Framework.h"
#include <iostream>

int main() {
    using namespace graphics;
    Application app;
    std::optional<MotionId> pathId, spinId, globalId;
    std::optional<LayerId> mirrorLayer, normalSpinLayer, insertedLayer;
    bool paused = false, spinEnabled = true, globalEnabled = false;
    int reflection = 1;

    const auto choosePath = [&](int kind) {
        PathMotion path;
        path.durationSeconds = 6;
        const glm::vec3 center(1.7f, 0, 0);
        if (kind == 1) path.path = PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 2) path.path = PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 3)
            path.path = PathSystem::polygon({{1, -.5f, 0}, {2.4f, -.4f, 0},
                {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
        else if (kind == 4) {
            path.path = [center](double u) {
                const double angle = u * 6.283185307179586;
                return center + glm::vec3(float(.7 * std::sin(2 * angle)),
                    float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
            };
            path.interpolation = PathSystem::smoothStep;
        } else if (kind == 5) path.path = PathSystem::circle(center, .65f, {0, 0, 1});
        else {
            path.path = PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
            path.playback = Playback::PingPong;
        }
        Motion motion;
        auto& motions = app.getScene().getMotions();
        return pathId && motion.append(Motion::followPath(path))
            && motions.setMotion(*pathId, motion) && motions.seekMotion(*pathId, 0);
    };

    app.setInitHandler([&](Application& current) {
        auto& scene = current.getScene();
        auto& motions = scene.getMotions();
        const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        if (!first || !second) return false;
        if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) return false;
        pathId = motions.createMotion(Motion{});
        RotationMotion spin;
        spin.degreesPerSecond = 90;
        spinId = motions.createSpin(spin);
        RotationMotion global;
        global.degreesPerSecond = 15;
        globalId = motions.createOrbit(global);
        if (!pathId || !spinId || !globalId || !choosePath(1)) return false;
        if (!motions.setMotionEnabled(*globalId, false)
            || !motions.bindMotion(MotionSystem::rootLayer(), *globalId)) return false;

        const auto normalPath = motions.pushLayer(MotionSystem::rootLayer());
        const auto reflectionMatrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
        if (!normalPath || !reflectionMatrix) return false;
        LayerConfig mirror;
        mirror.transform = *reflectionMatrix;
        mirrorLayer = motions.pushLayer(MotionSystem::rootLayer(), mirror);
        if (!mirrorLayer) return false;
        const auto mirroredPath = motions.pushLayer(*mirrorLayer);
        normalSpinLayer = motions.pushLayer(*normalPath);
        if (!mirroredPath || !normalSpinLayer) return false;
        const auto mirroredSpin = motions.pushLayer(*mirroredPath);
        if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId)
            || !motions.bindMotion(*mirroredPath, *pathId)
            || !motions.bindMotion(*normalSpinLayer, *spinId)
            || !motions.bindMotion(*mirroredSpin, *spinId)
            || !scene.bindEntity(*first, *normalSpinLayer)
            || !scene.bindEntity(*second, *mirroredSpin)) return false;
        if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
            || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
            || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) return false;
        current.getCamera().position = {5, 3, 8};
        std::cout << "1 타원, 2 사각형, 3 다각형, 4 사용자 경로+보간, 5 원, 6 왕복 꺾은선\n"
                  << "Space 시간 정지/재개, S 자전 표시, G 전체 공전, M 반사 변경, I 레이어 삽입/삭제\n"
                  << "M: 없음 -> YZ평면 반사 -> 점대칭 -> Y직선 대칭 순환. 클릭: 객체 ID. Esc 종료\n";
        return true;
    });
    app.setKeyHandler([&](Application& current, int key, int action) {
        if (action != GLFW_PRESS) return;
        auto& motions = current.getScene().getMotions();
        bool ok = true;
        if (key == GLFW_KEY_ESCAPE) current.requestClose();
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) ok = choosePath(key - GLFW_KEY_1 + 1);
        else if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            ok = motions.setMotionPlaying(*pathId, !paused)
                && motions.setMotionPlaying(*spinId, !paused)
                && motions.setMotionPlaying(*globalId, !paused);
        } else if (key == GLFW_KEY_S) {
            spinEnabled = !spinEnabled;
            ok = motions.setMotionEnabled(*spinId, spinEnabled);
        } else if (key == GLFW_KEY_G) {
            globalEnabled = !globalEnabled;
            ok = motions.setMotionEnabled(*globalId, globalEnabled);
        } else if (key == GLFW_KEY_M) {
            reflection = (reflection + 1) % 4;
            std::optional<glm::mat4> matrix = glm::mat4(1);
            if (reflection == 1) matrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
            if (reflection == 2) matrix = TransformSystem::reflectPoint({0, 0, 0});
            if (reflection == 3) matrix = TransformSystem::reflectLine(AxisLine{});
            LayerConfig config;
            if (!matrix) ok = false;
            else { config.transform = *matrix; ok = motions.setLayer(*mirrorLayer, config); }
        } else if (key == GLFW_KEY_I) {
            if (insertedLayer) {
                ok = motions.removeLayer(*insertedLayer);
                if (ok) insertedLayer.reset();
            } else {
                LayerConfig config;
                config.space = TransformSpace::Local;
                const auto scale = TransformSystem::scaling(glm::vec3(1.5f));
                if (!scale) ok = false;
                else {
                    config.transform = *scale;
                    insertedLayer = motions.insertLayer(*normalSpinLayer, config);
                    ok = insertedLayer.has_value();
                }
            }
        }
        if (!ok) current.requestClose();
    });
    app.setMouseHandler([](Application& current, const MouseEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
        const auto hit = current.pickAt(event.windowX, event.windowY);
        if (hit) std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    });
    ApplicationConfig config;
    config.title = "Motion and layer tree";
    return app.run(config);
}
```

## openGL_3dAssign_14/framework/include/Transform.h

축·반사 행렬과 경로 함수의 공개 계약을 분리한다.

before: 없음

after:

```cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {

struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

using PathFunction = std::function<glm::vec3(double)>;
using InterpolationFunction = std::function<double(double)>;

// These functions return no matrix/path when their parameters are invalid.
class TransformSystem {
public:
    static bool validAffine(const glm::mat4& matrix);
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    static std::optional<glm::mat4> scaling(const glm::vec3& factors,
                                           const glm::vec3& center = glm::vec3(0));
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point,
                                               const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU,
                                const glm::vec3& axisV);
    static PathFunction circle(const glm::vec3& center, float radius,
                               const glm::vec3& normal = glm::vec3(0, 1, 0));
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    static PathFunction polygon(std::vector<glm::vec3> points);
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU,
                                  const glm::vec3& halfV);
    static double linear(double progress);
    static double smoothStep(double progress);
};
} // namespace graphics
#include "../detail/Transform.inl"
```

## openGL_3dAssign_14/framework/detail/Transform.inl

점·직선·평면 대칭과 회전, 길이 기반 선분 경로 및 원·타원 계산을 구현한다.

before: 없음

after:

```cpp
#pragma once
#include <algorithm>

namespace graphics {
namespace detail {
inline std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction)) return {};
    const double length = glm::length(glm::dvec3(direction));
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return glm::vec3(glm::dvec3(direction) / length);
}
inline std::optional<glm::mat4> motionAround(const glm::vec3& center,
                                           const glm::mat4& matrix) {
    if (!finiteVector(center)) return {};
    const auto result = glm::translate(glm::mat4(1), center) * matrix
                      * glm::translate(glm::mat4(1), -center);
    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result)
                                                : std::nullopt;
}
inline bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);
    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

inline bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0
        && m[2][3] == 0 && m[3][3] == 1;
}
inline std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset)) return {};
    return glm::translate(glm::mat4(1), offset);
}
inline std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors,
                                                       const glm::vec3& center) {
    if (!detail::finiteVector(factors)) return {};
    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}
inline std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis,
                                                        double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);
    if (!unit || !std::isfinite(degrees)) return {};
    const float radians = static_cast<float>(std::remainder(degrees, 360.0)
                                            * 3.14159265358979323846 / 180.0);
    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}
inline std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}
inline std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);
    if (!unit) return {};
    glm::mat3 matrix(0);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);
    return detail::motionAround(line.point, glm::mat4(matrix));
}
inline std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point,
                                                            const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);
    if (!unit) return {};
    glm::mat3 matrix(1);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];
    return detail::motionAround(point, glm::mat4(matrix));
}
inline PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u,
                                       const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;
        return center + u * static_cast<float>(std::cos(angle))
                      + v * static_cast<float>(std::sin(angle));
    };
}
inline PathFunction PathSystem::circle(const glm::vec3& center, float radius,
                                      const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);
    if (!n || !std::isfinite(radius) || radius <= 0) return {};
    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));
    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}
inline PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2) return {};
    for (const auto& p : points) if (!detail::finiteVector(p)) return {};
    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());
        if (points.size() < 4) return {};
    }
    std::vector<double> distances{0};
    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));
        if (!std::isfinite(length) || length <= 1e-12) return {};
        distances.push_back(distances.back() + length);
    }
    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1
                                                   : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);
        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight)
                       + glm::dvec3(points[i]) * weight);
    };
}
inline PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}
inline PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u,
                                         const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);
    if (std::abs(glm::dot(*a, *b)) > 1e-5f) return {};
    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}
inline double PathSystem::linear(double progress) { return progress; }
inline double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics
```

## openGL_3dAssign_14/framework/include/Motion.h

동작·레이어 식별자, 설정, 시간·합성·트리 편집 API를 공개한다.

before: 없음

after:

```cpp
#pragma once
#include "Transform.h"
#include <map>

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};
struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
};

class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
public:
    bool append(const MotionStep& step);
    bool insert(std::size_t index, const MotionStep& step);
    bool replace(std::size_t index, const MotionStep& step);
    bool erase(std::size_t index);
    bool then(MotionStep step); // Requires all previous steps to have finite durations.
    const std::vector<MotionStep>& getSteps() const { return steps_; }
    std::optional<MotionSample> sample(double seconds) const;
    static MotionStep fixed(const glm::mat4& matrix,
                            TransformSpace space = TransformSpace::World);
    static MotionStep rotation(const RotationMotion& config);
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};
struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;
public:
    static constexpr LayerId rootLayer() { return 0; }
    std::optional<MotionId> createMotion(const Motion& motion);
    bool setMotion(MotionId id, const Motion& motion);
    const MotionState* getMotion(MotionId id) const;
    bool setMotionPlaying(MotionId id, bool playing);
    bool setMotionEnabled(MotionId id, bool enabled);
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id);
    bool update(double deltaSeconds);
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    bool reparentLayer(LayerId id, LayerId parent);
    bool setLayer(LayerId id, const LayerConfig& config);
    const LayerState* getLayer(LayerId id) const;
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    std::optional<MotionId> createSpin(RotationMotion config);
    std::optional<MotionId> createOrbit(RotationMotion config);
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
#include "../detail/Motion.inl"
```

## openGL_3dAssign_14/framework/detail/Motion.inl

원자적 시각 반영, 동작 합성, 공유 시계, 레이어 편집과 행렬 합성을 구현한다.

before: 없음

after:

```cpp
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
        auto& target = step.space == TransformSpace::World ? result.parent : result.local;
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
inline bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id)) return false;
    for (auto& layer : layers_)
        if (layer.second.config.motion == id) layer.second.config.motion.reset();
    return true;
}
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
inline void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};
    motions_.clear();
    layers_.swap(emptyLayers);
}
inline bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform)
        && (!config.motion || getMotion(*config.motion));
}
inline const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);
    return found == layers_.end() ? nullptr : &found->second;
}
inline std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
    auto staged = layers_;
    const auto id = nextLayer_;
    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;
    return id;
}
inline std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);
    if (!current || !current->parent || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
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
inline bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);
    if (!current || !current->parent) return false;
    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());
    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());
    for (auto child : current->children) staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);
    return true;
}
inline std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);
    if (!current || !current->parent || !current->children.empty()) return {};
    const auto parent = *current->parent;
    if (!removeLayer(top)) return {};
    return parent;
}
inline bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);
    if (!current || !current->parent || !getLayer(parent)) return false;
    if (current->parent == parent) return true;
    auto ancestor = std::optional<LayerId>(parent);
    while (ancestor) {
        if (*ancestor == id) return false;
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
inline bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);
    if (found == layers_.end() || !validLayer(config)) return false;
    found->second.config = config;
    return true;
}
inline bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);
    if (!current || (motion && !getMotion(*motion))) return false;
    auto config = current->config;
    config.motion = motion;
    return setLayer(layer, config);
}
inline std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);
    while (current) {
        const auto layer = getLayer(*current);
        if (!layer || chain.size() >= layers_.size()) return {};
        chain.push_back(*current);
        current = layer->parent;
    }
    MotionSample result;
    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;
        if (!config.enabled) continue;
        MotionSample own;
        if (config.space == TransformSpace::World) own.parent = config.transform;
        else own.local = config.transform;
        if (config.motion) {
            const auto state = getMotion(*config.motion);
            if (!state) return {};
            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }
        if (*i == rootLayer()) result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }
        if (!TransformSystem::validAffine(result.parent)
            || !TransformSystem::validAffine(result.local)) return {};
    }
    return result;
}
inline std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit,
                                                            RotationMotion spin) {
    orbit.space = TransformSpace::World;
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
```

## openGL_3dAssign_14/framework/include/Graphics.h

Scene이 MotionSystem과 바인딩을 소유하고 공개 API로 연결한다.

### 블록 1: before L1~6, after L1~7

before:

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

```

after:

```cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

```

### 블록 2: before L24~29, after L25~32

before:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
```

after:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;
    void rebuildFaceIds();

public:
```

### 블록 3: before L52~57, after L55~67

before:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

after:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
    MotionSystem& getMotions() { return motions_; }
    const MotionSystem& getMotions() const { return motions_; }
    bool bindEntity(EntityId id, LayerId layer);
    bool unbindEntity(EntityId id);
    std::optional<LayerId> getEntityLayer(EntityId id) const;
    bool updateMotions(double deltaSeconds) { return motions_.update(deltaSeconds); }
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

## openGL_3dAssign_14/framework/detail/Scene.inl

기존 객체 SRT와 단일 레이어 가지를 합성하여 면·선·피킹에 같은 결과를 제공한다.

### 블록 1: before L212~239, after L212~274

before:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    glm::mat4 result(1);
    std::optional<EntityId> current = id;
    std::size_t remaining = components_.size();
    while (current) {
        if (*current >= components_.size() || remaining-- == 0) return {};
        const auto& component = components_[*current];
        result = detail::transformMatrix(component.transform) * result;
        if (!detail::finiteMatrix(result)) return {};
        current = component.parent;
    }
    return result;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto& component = components_[id];
    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
    if (component.parent) {
        const auto parent = worldMatrix(*component.parent);
        if (!parent) return {};
        result = *parent * result;
    }
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

after:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;
    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size()) return {};
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());
    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;
    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);
        if (binding) { layer = *binding; anchor = i - 1; break; }
    }
    const auto motion = motions_.layerMatrices(layer);
    if (!motion) return {};
    glm::mat4 prefix(1), suffix(1);
    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
    }
    const auto result = motion->parent * prefix * motion->local * suffix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto world = worldMatrix(id);
    if (!world) return {};
    const auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 child(1);
    if (face.independence)
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * detail::transformMatrix(face.transform);
    const auto result = *world * child * face.localAnimationMatrix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size()) return {};
    if (lines_[id].owner) return worldMatrix(*lines_[id].owner);
    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());
    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}
inline bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer)) return false;
    entityLayers_[id] = layer;
    return true;
}
inline bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size()) return false;
    entityLayers_.erase(id);
    return true;
}
inline std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto found = entityLayers_.find(id);
    if (found == entityLayers_.end() || !motions_.getLayer(found->second)) return {};
    return found->second;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

### 블록 2: before L324~329, after L359~366

before:

```cpp
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

after:

```cpp
}

inline void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

## openGL_3dAssign_14/framework/detail/Renderer.inl

소유자 없는 선에도 루트 행렬을 적용하고 소유 선은 객체와 같은 변환을 사용한다.

### 블록 1: before L152~159, after L152~158

before:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

after:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

### 블록 2: before L184~190, after L183~190

before:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

after:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

## openGL_3dAssign_14/framework/detail/Application.inl

사용자 update 후 렌더 전에 Motion을 한 번 갱신한다. 실패 시 기존 자원 정리 경로로 종료한다.

### 블록 1: before L172~177, after L172~182

before:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

after:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

## openGL_3dAssign_14/framework/examples/motion.cpp

사용자 확인용 독립 예제로 경로·공유 동작·반사·레이어 편집·피킹을 연결한다.

before: 없음

after:

```cpp
#include "../Framework.h"
#include <iostream>

int main() {
    using namespace graphics;
    Application app;
    std::optional<MotionId> pathId, spinId, globalId;
    std::optional<LayerId> mirrorLayer, normalSpinLayer, insertedLayer;
    bool paused = false, spinEnabled = true, globalEnabled = false;
    int reflection = 1;

    const auto choosePath = [&](int kind) {
        PathMotion path;
        path.durationSeconds = 6;
        const glm::vec3 center(1.7f, 0, 0);
        if (kind == 1) path.path = PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 2) path.path = PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 3)
            path.path = PathSystem::polygon({{1, -.5f, 0}, {2.4f, -.4f, 0},
                {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
        else if (kind == 4) {
            path.path = [center](double u) {
                const double angle = u * 6.283185307179586;
                return center + glm::vec3(float(.7 * std::sin(2 * angle)),
                    float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
            };
            path.interpolation = PathSystem::smoothStep;
        } else if (kind == 5) path.path = PathSystem::circle(center, .65f, {0, 0, 1});
        else {
            path.path = PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
            path.playback = Playback::PingPong;
        }
        Motion motion;
        auto& motions = app.getScene().getMotions();
        return pathId && motion.append(Motion::followPath(path))
            && motions.setMotion(*pathId, motion) && motions.seekMotion(*pathId, 0);
    };

    app.setInitHandler([&](Application& current) {
        auto& scene = current.getScene();
        auto& motions = scene.getMotions();
        const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        if (!first || !second) return false;
        if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) return false;
        pathId = motions.createMotion(Motion{});
        RotationMotion spin;
        spin.degreesPerSecond = 90;
        spinId = motions.createSpin(spin);
        RotationMotion global;
        global.degreesPerSecond = 15;
        globalId = motions.createOrbit(global);
        if (!pathId || !spinId || !globalId || !choosePath(1)) return false;
        if (!motions.setMotionEnabled(*globalId, false)
            || !motions.bindMotion(MotionSystem::rootLayer(), *globalId)) return false;

        const auto normalPath = motions.pushLayer(MotionSystem::rootLayer());
        const auto reflectionMatrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
        if (!normalPath || !reflectionMatrix) return false;
        LayerConfig mirror;
        mirror.transform = *reflectionMatrix;
        mirrorLayer = motions.pushLayer(MotionSystem::rootLayer(), mirror);
        if (!mirrorLayer) return false;
        const auto mirroredPath = motions.pushLayer(*mirrorLayer);
        normalSpinLayer = motions.pushLayer(*normalPath);
        if (!mirroredPath || !normalSpinLayer) return false;
        const auto mirroredSpin = motions.pushLayer(*mirroredPath);
        if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId)
            || !motions.bindMotion(*mirroredPath, *pathId)
            || !motions.bindMotion(*normalSpinLayer, *spinId)
            || !motions.bindMotion(*mirroredSpin, *spinId)
            || !scene.bindEntity(*first, *normalSpinLayer)
            || !scene.bindEntity(*second, *mirroredSpin)) return false;
        if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
            || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
            || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) return false;
        current.getCamera().position = {5, 3, 8};
        std::cout << "1 타원, 2 사각형, 3 다각형, 4 사용자 경로+보간, 5 원, 6 왕복 꺾은선\n"
                  << "Space 시간 정지/재개, S 자전 표시, G 전체 공전, M 반사 변경, I 레이어 삽입/삭제\n"
                  << "M: 없음 -> YZ평면 반사 -> 점대칭 -> Y직선 대칭 순환. 클릭: 객체 ID. Esc 종료\n";
        return true;
    });
    app.setKeyHandler([&](Application& current, int key, int action) {
        if (action != GLFW_PRESS) return;
        auto& motions = current.getScene().getMotions();
        bool ok = true;
        if (key == GLFW_KEY_ESCAPE) current.requestClose();
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) ok = choosePath(key - GLFW_KEY_1 + 1);
        else if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            ok = motions.setMotionPlaying(*pathId, !paused)
                && motions.setMotionPlaying(*spinId, !paused)
                && motions.setMotionPlaying(*globalId, !paused);
        } else if (key == GLFW_KEY_S) {
            spinEnabled = !spinEnabled;
            ok = motions.setMotionEnabled(*spinId, spinEnabled);
        } else if (key == GLFW_KEY_G) {
            globalEnabled = !globalEnabled;
            ok = motions.setMotionEnabled(*globalId, globalEnabled);
        } else if (key == GLFW_KEY_M) {
            reflection = (reflection + 1) % 4;
            std::optional<glm::mat4> matrix = glm::mat4(1);
            if (reflection == 1) matrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
            if (reflection == 2) matrix = TransformSystem::reflectPoint({0, 0, 0});
            if (reflection == 3) matrix = TransformSystem::reflectLine(AxisLine{});
            LayerConfig config;
            if (!matrix) ok = false;
            else { config.transform = *matrix; ok = motions.setLayer(*mirrorLayer, config); }
        } else if (key == GLFW_KEY_I) {
            if (insertedLayer) {
                ok = motions.removeLayer(*insertedLayer);
                if (ok) insertedLayer.reset();
            } else {
                LayerConfig config;
                config.space = TransformSpace::Local;
                const auto scale = TransformSystem::scaling(glm::vec3(1.5f));
                if (!scale) ok = false;
                else {
                    config.transform = *scale;
                    insertedLayer = motions.insertLayer(*normalSpinLayer, config);
                    ok = insertedLayer.has_value();
                }
            }
        }
        if (!ok) current.requestClose();
    });
    app.setMouseHandler([](Application& current, const MouseEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
        const auto hit = current.pickAt(event.windowX, event.windowY);
        if (hit) std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    });
    ApplicationConfig config;
    config.title = "Motion and layer tree";
    return app.run(config);
}
```

## openGL_3dAssign_15/framework/include/Transform.h

축·반사 행렬과 경로 함수의 공개 계약을 분리한다.

before: 없음

after:

```cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {

struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

using PathFunction = std::function<glm::vec3(double)>;
using InterpolationFunction = std::function<double(double)>;

// These functions return no matrix/path when their parameters are invalid.
class TransformSystem {
public:
    static bool validAffine(const glm::mat4& matrix);
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    static std::optional<glm::mat4> scaling(const glm::vec3& factors,
                                           const glm::vec3& center = glm::vec3(0));
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point,
                                               const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU,
                                const glm::vec3& axisV);
    static PathFunction circle(const glm::vec3& center, float radius,
                               const glm::vec3& normal = glm::vec3(0, 1, 0));
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    static PathFunction polygon(std::vector<glm::vec3> points);
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU,
                                  const glm::vec3& halfV);
    static double linear(double progress);
    static double smoothStep(double progress);
};
} // namespace graphics
#include "../detail/Transform.inl"
```

## openGL_3dAssign_15/framework/detail/Transform.inl

점·직선·평면 대칭과 회전, 길이 기반 선분 경로 및 원·타원 계산을 구현한다.

before: 없음

after:

```cpp
#pragma once
#include <algorithm>

namespace graphics {
namespace detail {
inline std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction)) return {};
    const double length = glm::length(glm::dvec3(direction));
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return glm::vec3(glm::dvec3(direction) / length);
}
inline std::optional<glm::mat4> motionAround(const glm::vec3& center,
                                           const glm::mat4& matrix) {
    if (!finiteVector(center)) return {};
    const auto result = glm::translate(glm::mat4(1), center) * matrix
                      * glm::translate(glm::mat4(1), -center);
    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result)
                                                : std::nullopt;
}
inline bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);
    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

inline bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0
        && m[2][3] == 0 && m[3][3] == 1;
}
inline std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset)) return {};
    return glm::translate(glm::mat4(1), offset);
}
inline std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors,
                                                       const glm::vec3& center) {
    if (!detail::finiteVector(factors)) return {};
    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}
inline std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis,
                                                        double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);
    if (!unit || !std::isfinite(degrees)) return {};
    const float radians = static_cast<float>(std::remainder(degrees, 360.0)
                                            * 3.14159265358979323846 / 180.0);
    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}
inline std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}
inline std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);
    if (!unit) return {};
    glm::mat3 matrix(0);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);
    return detail::motionAround(line.point, glm::mat4(matrix));
}
inline std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point,
                                                            const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);
    if (!unit) return {};
    glm::mat3 matrix(1);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];
    return detail::motionAround(point, glm::mat4(matrix));
}
inline PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u,
                                       const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;
        return center + u * static_cast<float>(std::cos(angle))
                      + v * static_cast<float>(std::sin(angle));
    };
}
inline PathFunction PathSystem::circle(const glm::vec3& center, float radius,
                                      const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);
    if (!n || !std::isfinite(radius) || radius <= 0) return {};
    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));
    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}
inline PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2) return {};
    for (const auto& p : points) if (!detail::finiteVector(p)) return {};
    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());
        if (points.size() < 4) return {};
    }
    std::vector<double> distances{0};
    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));
        if (!std::isfinite(length) || length <= 1e-12) return {};
        distances.push_back(distances.back() + length);
    }
    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1
                                                   : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);
        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight)
                       + glm::dvec3(points[i]) * weight);
    };
}
inline PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}
inline PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u,
                                         const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);
    if (std::abs(glm::dot(*a, *b)) > 1e-5f) return {};
    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}
inline double PathSystem::linear(double progress) { return progress; }
inline double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics
```

## openGL_3dAssign_15/framework/include/Motion.h

동작·레이어 식별자, 설정, 시간·합성·트리 편집 API를 공개한다.

before: 없음

after:

```cpp
#pragma once
#include "Transform.h"
#include <map>

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};
struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
};

class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
public:
    bool append(const MotionStep& step);
    bool insert(std::size_t index, const MotionStep& step);
    bool replace(std::size_t index, const MotionStep& step);
    bool erase(std::size_t index);
    bool then(MotionStep step); // Requires all previous steps to have finite durations.
    const std::vector<MotionStep>& getSteps() const { return steps_; }
    std::optional<MotionSample> sample(double seconds) const;
    static MotionStep fixed(const glm::mat4& matrix,
                            TransformSpace space = TransformSpace::World);
    static MotionStep rotation(const RotationMotion& config);
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};
struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;
public:
    static constexpr LayerId rootLayer() { return 0; }
    std::optional<MotionId> createMotion(const Motion& motion);
    bool setMotion(MotionId id, const Motion& motion);
    const MotionState* getMotion(MotionId id) const;
    bool setMotionPlaying(MotionId id, bool playing);
    bool setMotionEnabled(MotionId id, bool enabled);
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id);
    bool update(double deltaSeconds);
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    bool reparentLayer(LayerId id, LayerId parent);
    bool setLayer(LayerId id, const LayerConfig& config);
    const LayerState* getLayer(LayerId id) const;
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    std::optional<MotionId> createSpin(RotationMotion config);
    std::optional<MotionId> createOrbit(RotationMotion config);
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
#include "../detail/Motion.inl"
```

## openGL_3dAssign_15/framework/detail/Motion.inl

원자적 시각 반영, 동작 합성, 공유 시계, 레이어 편집과 행렬 합성을 구현한다.

before: 없음

after:

```cpp
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
        auto& target = step.space == TransformSpace::World ? result.parent : result.local;
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
inline bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id)) return false;
    for (auto& layer : layers_)
        if (layer.second.config.motion == id) layer.second.config.motion.reset();
    return true;
}
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
inline void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};
    motions_.clear();
    layers_.swap(emptyLayers);
}
inline bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform)
        && (!config.motion || getMotion(*config.motion));
}
inline const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);
    return found == layers_.end() ? nullptr : &found->second;
}
inline std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
    auto staged = layers_;
    const auto id = nextLayer_;
    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;
    return id;
}
inline std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);
    if (!current || !current->parent || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
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
inline bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);
    if (!current || !current->parent) return false;
    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());
    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());
    for (auto child : current->children) staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);
    return true;
}
inline std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);
    if (!current || !current->parent || !current->children.empty()) return {};
    const auto parent = *current->parent;
    if (!removeLayer(top)) return {};
    return parent;
}
inline bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);
    if (!current || !current->parent || !getLayer(parent)) return false;
    if (current->parent == parent) return true;
    auto ancestor = std::optional<LayerId>(parent);
    while (ancestor) {
        if (*ancestor == id) return false;
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
inline bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);
    if (found == layers_.end() || !validLayer(config)) return false;
    found->second.config = config;
    return true;
}
inline bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);
    if (!current || (motion && !getMotion(*motion))) return false;
    auto config = current->config;
    config.motion = motion;
    return setLayer(layer, config);
}
inline std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);
    while (current) {
        const auto layer = getLayer(*current);
        if (!layer || chain.size() >= layers_.size()) return {};
        chain.push_back(*current);
        current = layer->parent;
    }
    MotionSample result;
    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;
        if (!config.enabled) continue;
        MotionSample own;
        if (config.space == TransformSpace::World) own.parent = config.transform;
        else own.local = config.transform;
        if (config.motion) {
            const auto state = getMotion(*config.motion);
            if (!state) return {};
            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }
        if (*i == rootLayer()) result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }
        if (!TransformSystem::validAffine(result.parent)
            || !TransformSystem::validAffine(result.local)) return {};
    }
    return result;
}
inline std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit,
                                                            RotationMotion spin) {
    orbit.space = TransformSpace::World;
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
```

## openGL_3dAssign_15/framework/include/Graphics.h

Scene이 MotionSystem과 바인딩을 소유하고 공개 API로 연결한다.

### 블록 1: before L1~6, after L1~7

before:

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

```

after:

```cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

```

### 블록 2: before L24~29, after L25~32

before:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
```

after:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;
    void rebuildFaceIds();

public:
```

### 블록 3: before L52~57, after L55~67

before:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

after:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
    MotionSystem& getMotions() { return motions_; }
    const MotionSystem& getMotions() const { return motions_; }
    bool bindEntity(EntityId id, LayerId layer);
    bool unbindEntity(EntityId id);
    std::optional<LayerId> getEntityLayer(EntityId id) const;
    bool updateMotions(double deltaSeconds) { return motions_.update(deltaSeconds); }
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

## openGL_3dAssign_15/framework/detail/Scene.inl

기존 객체 SRT와 단일 레이어 가지를 합성하여 면·선·피킹에 같은 결과를 제공한다.

### 블록 1: before L212~239, after L212~274

before:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    glm::mat4 result(1);
    std::optional<EntityId> current = id;
    std::size_t remaining = components_.size();
    while (current) {
        if (*current >= components_.size() || remaining-- == 0) return {};
        const auto& component = components_[*current];
        result = detail::transformMatrix(component.transform) * result;
        if (!detail::finiteMatrix(result)) return {};
        current = component.parent;
    }
    return result;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto& component = components_[id];
    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
    if (component.parent) {
        const auto parent = worldMatrix(*component.parent);
        if (!parent) return {};
        result = *parent * result;
    }
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

after:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;
    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size()) return {};
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());
    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;
    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);
        if (binding) { layer = *binding; anchor = i - 1; break; }
    }
    const auto motion = motions_.layerMatrices(layer);
    if (!motion) return {};
    glm::mat4 prefix(1), suffix(1);
    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
    }
    const auto result = motion->parent * prefix * motion->local * suffix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto world = worldMatrix(id);
    if (!world) return {};
    const auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 child(1);
    if (face.independence)
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * detail::transformMatrix(face.transform);
    const auto result = *world * child * face.localAnimationMatrix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size()) return {};
    if (lines_[id].owner) return worldMatrix(*lines_[id].owner);
    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());
    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}
inline bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer)) return false;
    entityLayers_[id] = layer;
    return true;
}
inline bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size()) return false;
    entityLayers_.erase(id);
    return true;
}
inline std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto found = entityLayers_.find(id);
    if (found == entityLayers_.end() || !motions_.getLayer(found->second)) return {};
    return found->second;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

### 블록 2: before L324~329, after L359~366

before:

```cpp
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

after:

```cpp
}

inline void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

## openGL_3dAssign_15/framework/detail/Renderer.inl

소유자 없는 선에도 루트 행렬을 적용하고 소유 선은 객체와 같은 변환을 사용한다.

### 블록 1: before L152~159, after L152~158

before:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

after:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

### 블록 2: before L184~190, after L183~190

before:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

after:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

## openGL_3dAssign_15/framework/detail/Application.inl

사용자 update 후 렌더 전에 Motion을 한 번 갱신한다. 실패 시 기존 자원 정리 경로로 종료한다.

### 블록 1: before L172~177, after L172~182

before:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

after:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

## openGL_3dAssign_15/framework/examples/motion.cpp

사용자 확인용 독립 예제로 경로·공유 동작·반사·레이어 편집·피킹을 연결한다.

before: 없음

after:

```cpp
#include "../Framework.h"
#include <iostream>

int main() {
    using namespace graphics;
    Application app;
    std::optional<MotionId> pathId, spinId, globalId;
    std::optional<LayerId> mirrorLayer, normalSpinLayer, insertedLayer;
    bool paused = false, spinEnabled = true, globalEnabled = false;
    int reflection = 1;

    const auto choosePath = [&](int kind) {
        PathMotion path;
        path.durationSeconds = 6;
        const glm::vec3 center(1.7f, 0, 0);
        if (kind == 1) path.path = PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 2) path.path = PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 3)
            path.path = PathSystem::polygon({{1, -.5f, 0}, {2.4f, -.4f, 0},
                {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
        else if (kind == 4) {
            path.path = [center](double u) {
                const double angle = u * 6.283185307179586;
                return center + glm::vec3(float(.7 * std::sin(2 * angle)),
                    float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
            };
            path.interpolation = PathSystem::smoothStep;
        } else if (kind == 5) path.path = PathSystem::circle(center, .65f, {0, 0, 1});
        else {
            path.path = PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
            path.playback = Playback::PingPong;
        }
        Motion motion;
        auto& motions = app.getScene().getMotions();
        return pathId && motion.append(Motion::followPath(path))
            && motions.setMotion(*pathId, motion) && motions.seekMotion(*pathId, 0);
    };

    app.setInitHandler([&](Application& current) {
        auto& scene = current.getScene();
        auto& motions = scene.getMotions();
        const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        if (!first || !second) return false;
        if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) return false;
        pathId = motions.createMotion(Motion{});
        RotationMotion spin;
        spin.degreesPerSecond = 90;
        spinId = motions.createSpin(spin);
        RotationMotion global;
        global.degreesPerSecond = 15;
        globalId = motions.createOrbit(global);
        if (!pathId || !spinId || !globalId || !choosePath(1)) return false;
        if (!motions.setMotionEnabled(*globalId, false)
            || !motions.bindMotion(MotionSystem::rootLayer(), *globalId)) return false;

        const auto normalPath = motions.pushLayer(MotionSystem::rootLayer());
        const auto reflectionMatrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
        if (!normalPath || !reflectionMatrix) return false;
        LayerConfig mirror;
        mirror.transform = *reflectionMatrix;
        mirrorLayer = motions.pushLayer(MotionSystem::rootLayer(), mirror);
        if (!mirrorLayer) return false;
        const auto mirroredPath = motions.pushLayer(*mirrorLayer);
        normalSpinLayer = motions.pushLayer(*normalPath);
        if (!mirroredPath || !normalSpinLayer) return false;
        const auto mirroredSpin = motions.pushLayer(*mirroredPath);
        if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId)
            || !motions.bindMotion(*mirroredPath, *pathId)
            || !motions.bindMotion(*normalSpinLayer, *spinId)
            || !motions.bindMotion(*mirroredSpin, *spinId)
            || !scene.bindEntity(*first, *normalSpinLayer)
            || !scene.bindEntity(*second, *mirroredSpin)) return false;
        if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
            || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
            || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) return false;
        current.getCamera().position = {5, 3, 8};
        std::cout << "1 타원, 2 사각형, 3 다각형, 4 사용자 경로+보간, 5 원, 6 왕복 꺾은선\n"
                  << "Space 시간 정지/재개, S 자전 표시, G 전체 공전, M 반사 변경, I 레이어 삽입/삭제\n"
                  << "M: 없음 -> YZ평면 반사 -> 점대칭 -> Y직선 대칭 순환. 클릭: 객체 ID. Esc 종료\n";
        return true;
    });
    app.setKeyHandler([&](Application& current, int key, int action) {
        if (action != GLFW_PRESS) return;
        auto& motions = current.getScene().getMotions();
        bool ok = true;
        if (key == GLFW_KEY_ESCAPE) current.requestClose();
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) ok = choosePath(key - GLFW_KEY_1 + 1);
        else if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            ok = motions.setMotionPlaying(*pathId, !paused)
                && motions.setMotionPlaying(*spinId, !paused)
                && motions.setMotionPlaying(*globalId, !paused);
        } else if (key == GLFW_KEY_S) {
            spinEnabled = !spinEnabled;
            ok = motions.setMotionEnabled(*spinId, spinEnabled);
        } else if (key == GLFW_KEY_G) {
            globalEnabled = !globalEnabled;
            ok = motions.setMotionEnabled(*globalId, globalEnabled);
        } else if (key == GLFW_KEY_M) {
            reflection = (reflection + 1) % 4;
            std::optional<glm::mat4> matrix = glm::mat4(1);
            if (reflection == 1) matrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
            if (reflection == 2) matrix = TransformSystem::reflectPoint({0, 0, 0});
            if (reflection == 3) matrix = TransformSystem::reflectLine(AxisLine{});
            LayerConfig config;
            if (!matrix) ok = false;
            else { config.transform = *matrix; ok = motions.setLayer(*mirrorLayer, config); }
        } else if (key == GLFW_KEY_I) {
            if (insertedLayer) {
                ok = motions.removeLayer(*insertedLayer);
                if (ok) insertedLayer.reset();
            } else {
                LayerConfig config;
                config.space = TransformSpace::Local;
                const auto scale = TransformSystem::scaling(glm::vec3(1.5f));
                if (!scale) ok = false;
                else {
                    config.transform = *scale;
                    insertedLayer = motions.insertLayer(*normalSpinLayer, config);
                    ok = insertedLayer.has_value();
                }
            }
        }
        if (!ok) current.requestClose();
    });
    app.setMouseHandler([](Application& current, const MouseEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
        const auto hit = current.pickAt(event.windowX, event.windowY);
        if (hit) std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    });
    ApplicationConfig config;
    config.title = "Motion and layer tree";
    return app.run(config);
}
```

## openGL_3dAssign_16/framework/include/Transform.h

축·반사 행렬과 경로 함수의 공개 계약을 분리한다.

before: 없음

after:

```cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {

struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

using PathFunction = std::function<glm::vec3(double)>;
using InterpolationFunction = std::function<double(double)>;

// These functions return no matrix/path when their parameters are invalid.
class TransformSystem {
public:
    static bool validAffine(const glm::mat4& matrix);
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    static std::optional<glm::mat4> scaling(const glm::vec3& factors,
                                           const glm::vec3& center = glm::vec3(0));
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point,
                                               const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU,
                                const glm::vec3& axisV);
    static PathFunction circle(const glm::vec3& center, float radius,
                               const glm::vec3& normal = glm::vec3(0, 1, 0));
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    static PathFunction polygon(std::vector<glm::vec3> points);
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU,
                                  const glm::vec3& halfV);
    static double linear(double progress);
    static double smoothStep(double progress);
};
} // namespace graphics
#include "../detail/Transform.inl"
```

## openGL_3dAssign_16/framework/detail/Transform.inl

점·직선·평면 대칭과 회전, 길이 기반 선분 경로 및 원·타원 계산을 구현한다.

before: 없음

after:

```cpp
#pragma once
#include <algorithm>

namespace graphics {
namespace detail {
inline std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction)) return {};
    const double length = glm::length(glm::dvec3(direction));
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return glm::vec3(glm::dvec3(direction) / length);
}
inline std::optional<glm::mat4> motionAround(const glm::vec3& center,
                                           const glm::mat4& matrix) {
    if (!finiteVector(center)) return {};
    const auto result = glm::translate(glm::mat4(1), center) * matrix
                      * glm::translate(glm::mat4(1), -center);
    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result)
                                                : std::nullopt;
}
inline bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);
    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

inline bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0
        && m[2][3] == 0 && m[3][3] == 1;
}
inline std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset)) return {};
    return glm::translate(glm::mat4(1), offset);
}
inline std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors,
                                                       const glm::vec3& center) {
    if (!detail::finiteVector(factors)) return {};
    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}
inline std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis,
                                                        double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);
    if (!unit || !std::isfinite(degrees)) return {};
    const float radians = static_cast<float>(std::remainder(degrees, 360.0)
                                            * 3.14159265358979323846 / 180.0);
    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}
inline std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}
inline std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);
    if (!unit) return {};
    glm::mat3 matrix(0);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);
    return detail::motionAround(line.point, glm::mat4(matrix));
}
inline std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point,
                                                            const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);
    if (!unit) return {};
    glm::mat3 matrix(1);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];
    return detail::motionAround(point, glm::mat4(matrix));
}
inline PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u,
                                       const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;
        return center + u * static_cast<float>(std::cos(angle))
                      + v * static_cast<float>(std::sin(angle));
    };
}
inline PathFunction PathSystem::circle(const glm::vec3& center, float radius,
                                      const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);
    if (!n || !std::isfinite(radius) || radius <= 0) return {};
    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));
    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}
inline PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2) return {};
    for (const auto& p : points) if (!detail::finiteVector(p)) return {};
    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());
        if (points.size() < 4) return {};
    }
    std::vector<double> distances{0};
    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));
        if (!std::isfinite(length) || length <= 1e-12) return {};
        distances.push_back(distances.back() + length);
    }
    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1
                                                   : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);
        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight)
                       + glm::dvec3(points[i]) * weight);
    };
}
inline PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}
inline PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u,
                                         const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);
    if (std::abs(glm::dot(*a, *b)) > 1e-5f) return {};
    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}
inline double PathSystem::linear(double progress) { return progress; }
inline double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics
```

## openGL_3dAssign_16/framework/include/Motion.h

동작·레이어 식별자, 설정, 시간·합성·트리 편집 API를 공개한다.

before: 없음

after:

```cpp
#pragma once
#include "Transform.h"
#include <map>

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};
struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
};

class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
public:
    bool append(const MotionStep& step);
    bool insert(std::size_t index, const MotionStep& step);
    bool replace(std::size_t index, const MotionStep& step);
    bool erase(std::size_t index);
    bool then(MotionStep step); // Requires all previous steps to have finite durations.
    const std::vector<MotionStep>& getSteps() const { return steps_; }
    std::optional<MotionSample> sample(double seconds) const;
    static MotionStep fixed(const glm::mat4& matrix,
                            TransformSpace space = TransformSpace::World);
    static MotionStep rotation(const RotationMotion& config);
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};
struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;
public:
    static constexpr LayerId rootLayer() { return 0; }
    std::optional<MotionId> createMotion(const Motion& motion);
    bool setMotion(MotionId id, const Motion& motion);
    const MotionState* getMotion(MotionId id) const;
    bool setMotionPlaying(MotionId id, bool playing);
    bool setMotionEnabled(MotionId id, bool enabled);
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id);
    bool update(double deltaSeconds);
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    bool reparentLayer(LayerId id, LayerId parent);
    bool setLayer(LayerId id, const LayerConfig& config);
    const LayerState* getLayer(LayerId id) const;
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    std::optional<MotionId> createSpin(RotationMotion config);
    std::optional<MotionId> createOrbit(RotationMotion config);
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
#include "../detail/Motion.inl"
```

## openGL_3dAssign_16/framework/detail/Motion.inl

원자적 시각 반영, 동작 합성, 공유 시계, 레이어 편집과 행렬 합성을 구현한다.

before: 없음

after:

```cpp
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
        auto& target = step.space == TransformSpace::World ? result.parent : result.local;
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
inline bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id)) return false;
    for (auto& layer : layers_)
        if (layer.second.config.motion == id) layer.second.config.motion.reset();
    return true;
}
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
inline void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};
    motions_.clear();
    layers_.swap(emptyLayers);
}
inline bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform)
        && (!config.motion || getMotion(*config.motion));
}
inline const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);
    return found == layers_.end() ? nullptr : &found->second;
}
inline std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
    auto staged = layers_;
    const auto id = nextLayer_;
    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;
    return id;
}
inline std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);
    if (!current || !current->parent || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
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
inline bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);
    if (!current || !current->parent) return false;
    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());
    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());
    for (auto child : current->children) staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);
    return true;
}
inline std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);
    if (!current || !current->parent || !current->children.empty()) return {};
    const auto parent = *current->parent;
    if (!removeLayer(top)) return {};
    return parent;
}
inline bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);
    if (!current || !current->parent || !getLayer(parent)) return false;
    if (current->parent == parent) return true;
    auto ancestor = std::optional<LayerId>(parent);
    while (ancestor) {
        if (*ancestor == id) return false;
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
inline bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);
    if (found == layers_.end() || !validLayer(config)) return false;
    found->second.config = config;
    return true;
}
inline bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);
    if (!current || (motion && !getMotion(*motion))) return false;
    auto config = current->config;
    config.motion = motion;
    return setLayer(layer, config);
}
inline std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);
    while (current) {
        const auto layer = getLayer(*current);
        if (!layer || chain.size() >= layers_.size()) return {};
        chain.push_back(*current);
        current = layer->parent;
    }
    MotionSample result;
    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;
        if (!config.enabled) continue;
        MotionSample own;
        if (config.space == TransformSpace::World) own.parent = config.transform;
        else own.local = config.transform;
        if (config.motion) {
            const auto state = getMotion(*config.motion);
            if (!state) return {};
            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }
        if (*i == rootLayer()) result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }
        if (!TransformSystem::validAffine(result.parent)
            || !TransformSystem::validAffine(result.local)) return {};
    }
    return result;
}
inline std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit,
                                                            RotationMotion spin) {
    orbit.space = TransformSpace::World;
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
```

## openGL_3dAssign_16/framework/include/Graphics.h

Scene이 MotionSystem과 바인딩을 소유하고 공개 API로 연결한다.

### 블록 1: before L1~6, after L1~7

before:

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

```

after:

```cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

```

### 블록 2: before L24~29, after L25~32

before:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
```

after:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;
    void rebuildFaceIds();

public:
```

### 블록 3: before L52~57, after L55~67

before:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

after:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
    MotionSystem& getMotions() { return motions_; }
    const MotionSystem& getMotions() const { return motions_; }
    bool bindEntity(EntityId id, LayerId layer);
    bool unbindEntity(EntityId id);
    std::optional<LayerId> getEntityLayer(EntityId id) const;
    bool updateMotions(double deltaSeconds) { return motions_.update(deltaSeconds); }
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

## openGL_3dAssign_16/framework/detail/Scene.inl

기존 객체 SRT와 단일 레이어 가지를 합성하여 면·선·피킹에 같은 결과를 제공한다.

### 블록 1: before L212~239, after L212~274

before:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    glm::mat4 result(1);
    std::optional<EntityId> current = id;
    std::size_t remaining = components_.size();
    while (current) {
        if (*current >= components_.size() || remaining-- == 0) return {};
        const auto& component = components_[*current];
        result = detail::transformMatrix(component.transform) * result;
        if (!detail::finiteMatrix(result)) return {};
        current = component.parent;
    }
    return result;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto& component = components_[id];
    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
    if (component.parent) {
        const auto parent = worldMatrix(*component.parent);
        if (!parent) return {};
        result = *parent * result;
    }
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

after:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;
    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size()) return {};
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());
    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;
    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);
        if (binding) { layer = *binding; anchor = i - 1; break; }
    }
    const auto motion = motions_.layerMatrices(layer);
    if (!motion) return {};
    glm::mat4 prefix(1), suffix(1);
    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
    }
    const auto result = motion->parent * prefix * motion->local * suffix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto world = worldMatrix(id);
    if (!world) return {};
    const auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 child(1);
    if (face.independence)
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * detail::transformMatrix(face.transform);
    const auto result = *world * child * face.localAnimationMatrix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size()) return {};
    if (lines_[id].owner) return worldMatrix(*lines_[id].owner);
    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());
    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}
inline bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer)) return false;
    entityLayers_[id] = layer;
    return true;
}
inline bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size()) return false;
    entityLayers_.erase(id);
    return true;
}
inline std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto found = entityLayers_.find(id);
    if (found == entityLayers_.end() || !motions_.getLayer(found->second)) return {};
    return found->second;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

### 블록 2: before L324~329, after L359~366

before:

```cpp
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

after:

```cpp
}

inline void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

## openGL_3dAssign_16/framework/detail/Renderer.inl

소유자 없는 선에도 루트 행렬을 적용하고 소유 선은 객체와 같은 변환을 사용한다.

### 블록 1: before L152~159, after L152~158

before:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

after:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

### 블록 2: before L184~190, after L183~190

before:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

after:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

## openGL_3dAssign_16/framework/detail/Application.inl

사용자 update 후 렌더 전에 Motion을 한 번 갱신한다. 실패 시 기존 자원 정리 경로로 종료한다.

### 블록 1: before L172~177, after L172~182

before:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

after:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

## openGL_3dAssign_16/framework/examples/motion.cpp

사용자 확인용 독립 예제로 경로·공유 동작·반사·레이어 편집·피킹을 연결한다.

before: 없음

after:

```cpp
#include "../Framework.h"
#include <iostream>

int main() {
    using namespace graphics;
    Application app;
    std::optional<MotionId> pathId, spinId, globalId;
    std::optional<LayerId> mirrorLayer, normalSpinLayer, insertedLayer;
    bool paused = false, spinEnabled = true, globalEnabled = false;
    int reflection = 1;

    const auto choosePath = [&](int kind) {
        PathMotion path;
        path.durationSeconds = 6;
        const glm::vec3 center(1.7f, 0, 0);
        if (kind == 1) path.path = PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 2) path.path = PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 3)
            path.path = PathSystem::polygon({{1, -.5f, 0}, {2.4f, -.4f, 0},
                {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
        else if (kind == 4) {
            path.path = [center](double u) {
                const double angle = u * 6.283185307179586;
                return center + glm::vec3(float(.7 * std::sin(2 * angle)),
                    float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
            };
            path.interpolation = PathSystem::smoothStep;
        } else if (kind == 5) path.path = PathSystem::circle(center, .65f, {0, 0, 1});
        else {
            path.path = PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
            path.playback = Playback::PingPong;
        }
        Motion motion;
        auto& motions = app.getScene().getMotions();
        return pathId && motion.append(Motion::followPath(path))
            && motions.setMotion(*pathId, motion) && motions.seekMotion(*pathId, 0);
    };

    app.setInitHandler([&](Application& current) {
        auto& scene = current.getScene();
        auto& motions = scene.getMotions();
        const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        if (!first || !second) return false;
        if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) return false;
        pathId = motions.createMotion(Motion{});
        RotationMotion spin;
        spin.degreesPerSecond = 90;
        spinId = motions.createSpin(spin);
        RotationMotion global;
        global.degreesPerSecond = 15;
        globalId = motions.createOrbit(global);
        if (!pathId || !spinId || !globalId || !choosePath(1)) return false;
        if (!motions.setMotionEnabled(*globalId, false)
            || !motions.bindMotion(MotionSystem::rootLayer(), *globalId)) return false;

        const auto normalPath = motions.pushLayer(MotionSystem::rootLayer());
        const auto reflectionMatrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
        if (!normalPath || !reflectionMatrix) return false;
        LayerConfig mirror;
        mirror.transform = *reflectionMatrix;
        mirrorLayer = motions.pushLayer(MotionSystem::rootLayer(), mirror);
        if (!mirrorLayer) return false;
        const auto mirroredPath = motions.pushLayer(*mirrorLayer);
        normalSpinLayer = motions.pushLayer(*normalPath);
        if (!mirroredPath || !normalSpinLayer) return false;
        const auto mirroredSpin = motions.pushLayer(*mirroredPath);
        if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId)
            || !motions.bindMotion(*mirroredPath, *pathId)
            || !motions.bindMotion(*normalSpinLayer, *spinId)
            || !motions.bindMotion(*mirroredSpin, *spinId)
            || !scene.bindEntity(*first, *normalSpinLayer)
            || !scene.bindEntity(*second, *mirroredSpin)) return false;
        if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
            || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
            || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) return false;
        current.getCamera().position = {5, 3, 8};
        std::cout << "1 타원, 2 사각형, 3 다각형, 4 사용자 경로+보간, 5 원, 6 왕복 꺾은선\n"
                  << "Space 시간 정지/재개, S 자전 표시, G 전체 공전, M 반사 변경, I 레이어 삽입/삭제\n"
                  << "M: 없음 -> YZ평면 반사 -> 점대칭 -> Y직선 대칭 순환. 클릭: 객체 ID. Esc 종료\n";
        return true;
    });
    app.setKeyHandler([&](Application& current, int key, int action) {
        if (action != GLFW_PRESS) return;
        auto& motions = current.getScene().getMotions();
        bool ok = true;
        if (key == GLFW_KEY_ESCAPE) current.requestClose();
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) ok = choosePath(key - GLFW_KEY_1 + 1);
        else if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            ok = motions.setMotionPlaying(*pathId, !paused)
                && motions.setMotionPlaying(*spinId, !paused)
                && motions.setMotionPlaying(*globalId, !paused);
        } else if (key == GLFW_KEY_S) {
            spinEnabled = !spinEnabled;
            ok = motions.setMotionEnabled(*spinId, spinEnabled);
        } else if (key == GLFW_KEY_G) {
            globalEnabled = !globalEnabled;
            ok = motions.setMotionEnabled(*globalId, globalEnabled);
        } else if (key == GLFW_KEY_M) {
            reflection = (reflection + 1) % 4;
            std::optional<glm::mat4> matrix = glm::mat4(1);
            if (reflection == 1) matrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
            if (reflection == 2) matrix = TransformSystem::reflectPoint({0, 0, 0});
            if (reflection == 3) matrix = TransformSystem::reflectLine(AxisLine{});
            LayerConfig config;
            if (!matrix) ok = false;
            else { config.transform = *matrix; ok = motions.setLayer(*mirrorLayer, config); }
        } else if (key == GLFW_KEY_I) {
            if (insertedLayer) {
                ok = motions.removeLayer(*insertedLayer);
                if (ok) insertedLayer.reset();
            } else {
                LayerConfig config;
                config.space = TransformSpace::Local;
                const auto scale = TransformSystem::scaling(glm::vec3(1.5f));
                if (!scale) ok = false;
                else {
                    config.transform = *scale;
                    insertedLayer = motions.insertLayer(*normalSpinLayer, config);
                    ok = insertedLayer.has_value();
                }
            }
        }
        if (!ok) current.requestClose();
    });
    app.setMouseHandler([](Application& current, const MouseEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
        const auto hit = current.pickAt(event.windowX, event.windowY);
        if (hit) std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    });
    ApplicationConfig config;
    config.title = "Motion and layer tree";
    return app.run(config);
}
```

## openGL_3dAssign_17/framework/include/Transform.h

축·반사 행렬과 경로 함수의 공개 계약을 분리한다.

before: 없음

after:

```cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {

struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

using PathFunction = std::function<glm::vec3(double)>;
using InterpolationFunction = std::function<double(double)>;

// These functions return no matrix/path when their parameters are invalid.
class TransformSystem {
public:
    static bool validAffine(const glm::mat4& matrix);
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    static std::optional<glm::mat4> scaling(const glm::vec3& factors,
                                           const glm::vec3& center = glm::vec3(0));
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point,
                                               const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU,
                                const glm::vec3& axisV);
    static PathFunction circle(const glm::vec3& center, float radius,
                               const glm::vec3& normal = glm::vec3(0, 1, 0));
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    static PathFunction polygon(std::vector<glm::vec3> points);
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU,
                                  const glm::vec3& halfV);
    static double linear(double progress);
    static double smoothStep(double progress);
};
} // namespace graphics
#include "../detail/Transform.inl"
```

## openGL_3dAssign_17/framework/detail/Transform.inl

점·직선·평면 대칭과 회전, 길이 기반 선분 경로 및 원·타원 계산을 구현한다.

before: 없음

after:

```cpp
#pragma once
#include <algorithm>

namespace graphics {
namespace detail {
inline std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction)) return {};
    const double length = glm::length(glm::dvec3(direction));
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return glm::vec3(glm::dvec3(direction) / length);
}
inline std::optional<glm::mat4> motionAround(const glm::vec3& center,
                                           const glm::mat4& matrix) {
    if (!finiteVector(center)) return {};
    const auto result = glm::translate(glm::mat4(1), center) * matrix
                      * glm::translate(glm::mat4(1), -center);
    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result)
                                                : std::nullopt;
}
inline bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);
    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

inline bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0
        && m[2][3] == 0 && m[3][3] == 1;
}
inline std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset)) return {};
    return glm::translate(glm::mat4(1), offset);
}
inline std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors,
                                                       const glm::vec3& center) {
    if (!detail::finiteVector(factors)) return {};
    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}
inline std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis,
                                                        double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);
    if (!unit || !std::isfinite(degrees)) return {};
    const float radians = static_cast<float>(std::remainder(degrees, 360.0)
                                            * 3.14159265358979323846 / 180.0);
    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}
inline std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}
inline std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);
    if (!unit) return {};
    glm::mat3 matrix(0);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);
    return detail::motionAround(line.point, glm::mat4(matrix));
}
inline std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point,
                                                            const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);
    if (!unit) return {};
    glm::mat3 matrix(1);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];
    return detail::motionAround(point, glm::mat4(matrix));
}
inline PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u,
                                       const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;
        return center + u * static_cast<float>(std::cos(angle))
                      + v * static_cast<float>(std::sin(angle));
    };
}
inline PathFunction PathSystem::circle(const glm::vec3& center, float radius,
                                      const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);
    if (!n || !std::isfinite(radius) || radius <= 0) return {};
    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));
    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}
inline PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2) return {};
    for (const auto& p : points) if (!detail::finiteVector(p)) return {};
    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());
        if (points.size() < 4) return {};
    }
    std::vector<double> distances{0};
    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));
        if (!std::isfinite(length) || length <= 1e-12) return {};
        distances.push_back(distances.back() + length);
    }
    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1
                                                   : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);
        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight)
                       + glm::dvec3(points[i]) * weight);
    };
}
inline PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}
inline PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u,
                                         const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);
    if (std::abs(glm::dot(*a, *b)) > 1e-5f) return {};
    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}
inline double PathSystem::linear(double progress) { return progress; }
inline double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics
```

## openGL_3dAssign_17/framework/include/Motion.h

동작·레이어 식별자, 설정, 시간·합성·트리 편집 API를 공개한다.

before: 없음

after:

```cpp
#pragma once
#include "Transform.h"
#include <map>

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};
struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
};

class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
public:
    bool append(const MotionStep& step);
    bool insert(std::size_t index, const MotionStep& step);
    bool replace(std::size_t index, const MotionStep& step);
    bool erase(std::size_t index);
    bool then(MotionStep step); // Requires all previous steps to have finite durations.
    const std::vector<MotionStep>& getSteps() const { return steps_; }
    std::optional<MotionSample> sample(double seconds) const;
    static MotionStep fixed(const glm::mat4& matrix,
                            TransformSpace space = TransformSpace::World);
    static MotionStep rotation(const RotationMotion& config);
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};
struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;
public:
    static constexpr LayerId rootLayer() { return 0; }
    std::optional<MotionId> createMotion(const Motion& motion);
    bool setMotion(MotionId id, const Motion& motion);
    const MotionState* getMotion(MotionId id) const;
    bool setMotionPlaying(MotionId id, bool playing);
    bool setMotionEnabled(MotionId id, bool enabled);
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id);
    bool update(double deltaSeconds);
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    bool reparentLayer(LayerId id, LayerId parent);
    bool setLayer(LayerId id, const LayerConfig& config);
    const LayerState* getLayer(LayerId id) const;
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    std::optional<MotionId> createSpin(RotationMotion config);
    std::optional<MotionId> createOrbit(RotationMotion config);
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
#include "../detail/Motion.inl"
```

## openGL_3dAssign_17/framework/detail/Motion.inl

원자적 시각 반영, 동작 합성, 공유 시계, 레이어 편집과 행렬 합성을 구현한다.

before: 없음

after:

```cpp
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
        auto& target = step.space == TransformSpace::World ? result.parent : result.local;
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
inline bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id)) return false;
    for (auto& layer : layers_)
        if (layer.second.config.motion == id) layer.second.config.motion.reset();
    return true;
}
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
inline void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};
    motions_.clear();
    layers_.swap(emptyLayers);
}
inline bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform)
        && (!config.motion || getMotion(*config.motion));
}
inline const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);
    return found == layers_.end() ? nullptr : &found->second;
}
inline std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
    auto staged = layers_;
    const auto id = nextLayer_;
    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;
    return id;
}
inline std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);
    if (!current || !current->parent || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
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
inline bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);
    if (!current || !current->parent) return false;
    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());
    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());
    for (auto child : current->children) staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);
    return true;
}
inline std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);
    if (!current || !current->parent || !current->children.empty()) return {};
    const auto parent = *current->parent;
    if (!removeLayer(top)) return {};
    return parent;
}
inline bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);
    if (!current || !current->parent || !getLayer(parent)) return false;
    if (current->parent == parent) return true;
    auto ancestor = std::optional<LayerId>(parent);
    while (ancestor) {
        if (*ancestor == id) return false;
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
inline bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);
    if (found == layers_.end() || !validLayer(config)) return false;
    found->second.config = config;
    return true;
}
inline bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);
    if (!current || (motion && !getMotion(*motion))) return false;
    auto config = current->config;
    config.motion = motion;
    return setLayer(layer, config);
}
inline std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);
    while (current) {
        const auto layer = getLayer(*current);
        if (!layer || chain.size() >= layers_.size()) return {};
        chain.push_back(*current);
        current = layer->parent;
    }
    MotionSample result;
    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;
        if (!config.enabled) continue;
        MotionSample own;
        if (config.space == TransformSpace::World) own.parent = config.transform;
        else own.local = config.transform;
        if (config.motion) {
            const auto state = getMotion(*config.motion);
            if (!state) return {};
            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }
        if (*i == rootLayer()) result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }
        if (!TransformSystem::validAffine(result.parent)
            || !TransformSystem::validAffine(result.local)) return {};
    }
    return result;
}
inline std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit,
                                                            RotationMotion spin) {
    orbit.space = TransformSpace::World;
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
```

## openGL_3dAssign_17/framework/include/Graphics.h

Scene이 MotionSystem과 바인딩을 소유하고 공개 API로 연결한다.

### 블록 1: before L1~6, after L1~7

before:

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

```

after:

```cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

```

### 블록 2: before L24~29, after L25~32

before:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
```

after:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;
    void rebuildFaceIds();

public:
```

### 블록 3: before L52~57, after L55~67

before:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

after:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
    MotionSystem& getMotions() { return motions_; }
    const MotionSystem& getMotions() const { return motions_; }
    bool bindEntity(EntityId id, LayerId layer);
    bool unbindEntity(EntityId id);
    std::optional<LayerId> getEntityLayer(EntityId id) const;
    bool updateMotions(double deltaSeconds) { return motions_.update(deltaSeconds); }
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

## openGL_3dAssign_17/framework/detail/Scene.inl

기존 객체 SRT와 단일 레이어 가지를 합성하여 면·선·피킹에 같은 결과를 제공한다.

### 블록 1: before L212~239, after L212~274

before:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    glm::mat4 result(1);
    std::optional<EntityId> current = id;
    std::size_t remaining = components_.size();
    while (current) {
        if (*current >= components_.size() || remaining-- == 0) return {};
        const auto& component = components_[*current];
        result = detail::transformMatrix(component.transform) * result;
        if (!detail::finiteMatrix(result)) return {};
        current = component.parent;
    }
    return result;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto& component = components_[id];
    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
    if (component.parent) {
        const auto parent = worldMatrix(*component.parent);
        if (!parent) return {};
        result = *parent * result;
    }
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

after:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;
    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size()) return {};
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());
    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;
    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);
        if (binding) { layer = *binding; anchor = i - 1; break; }
    }
    const auto motion = motions_.layerMatrices(layer);
    if (!motion) return {};
    glm::mat4 prefix(1), suffix(1);
    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
    }
    const auto result = motion->parent * prefix * motion->local * suffix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto world = worldMatrix(id);
    if (!world) return {};
    const auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 child(1);
    if (face.independence)
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * detail::transformMatrix(face.transform);
    const auto result = *world * child * face.localAnimationMatrix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size()) return {};
    if (lines_[id].owner) return worldMatrix(*lines_[id].owner);
    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());
    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}
inline bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer)) return false;
    entityLayers_[id] = layer;
    return true;
}
inline bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size()) return false;
    entityLayers_.erase(id);
    return true;
}
inline std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto found = entityLayers_.find(id);
    if (found == entityLayers_.end() || !motions_.getLayer(found->second)) return {};
    return found->second;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

### 블록 2: before L324~329, after L359~366

before:

```cpp
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

after:

```cpp
}

inline void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

## openGL_3dAssign_17/framework/detail/Renderer.inl

소유자 없는 선에도 루트 행렬을 적용하고 소유 선은 객체와 같은 변환을 사용한다.

### 블록 1: before L152~159, after L152~158

before:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

after:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

### 블록 2: before L184~190, after L183~190

before:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

after:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

## openGL_3dAssign_17/framework/detail/Application.inl

사용자 update 후 렌더 전에 Motion을 한 번 갱신한다. 실패 시 기존 자원 정리 경로로 종료한다.

### 블록 1: before L172~177, after L172~182

before:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

after:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

## openGL_3dAssign_17/framework/examples/motion.cpp

사용자 확인용 독립 예제로 경로·공유 동작·반사·레이어 편집·피킹을 연결한다.

before: 없음

after:

```cpp
#include "../Framework.h"
#include <iostream>

int main() {
    using namespace graphics;
    Application app;
    std::optional<MotionId> pathId, spinId, globalId;
    std::optional<LayerId> mirrorLayer, normalSpinLayer, insertedLayer;
    bool paused = false, spinEnabled = true, globalEnabled = false;
    int reflection = 1;

    const auto choosePath = [&](int kind) {
        PathMotion path;
        path.durationSeconds = 6;
        const glm::vec3 center(1.7f, 0, 0);
        if (kind == 1) path.path = PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 2) path.path = PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 3)
            path.path = PathSystem::polygon({{1, -.5f, 0}, {2.4f, -.4f, 0},
                {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
        else if (kind == 4) {
            path.path = [center](double u) {
                const double angle = u * 6.283185307179586;
                return center + glm::vec3(float(.7 * std::sin(2 * angle)),
                    float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
            };
            path.interpolation = PathSystem::smoothStep;
        } else if (kind == 5) path.path = PathSystem::circle(center, .65f, {0, 0, 1});
        else {
            path.path = PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
            path.playback = Playback::PingPong;
        }
        Motion motion;
        auto& motions = app.getScene().getMotions();
        return pathId && motion.append(Motion::followPath(path))
            && motions.setMotion(*pathId, motion) && motions.seekMotion(*pathId, 0);
    };

    app.setInitHandler([&](Application& current) {
        auto& scene = current.getScene();
        auto& motions = scene.getMotions();
        const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        if (!first || !second) return false;
        if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) return false;
        pathId = motions.createMotion(Motion{});
        RotationMotion spin;
        spin.degreesPerSecond = 90;
        spinId = motions.createSpin(spin);
        RotationMotion global;
        global.degreesPerSecond = 15;
        globalId = motions.createOrbit(global);
        if (!pathId || !spinId || !globalId || !choosePath(1)) return false;
        if (!motions.setMotionEnabled(*globalId, false)
            || !motions.bindMotion(MotionSystem::rootLayer(), *globalId)) return false;

        const auto normalPath = motions.pushLayer(MotionSystem::rootLayer());
        const auto reflectionMatrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
        if (!normalPath || !reflectionMatrix) return false;
        LayerConfig mirror;
        mirror.transform = *reflectionMatrix;
        mirrorLayer = motions.pushLayer(MotionSystem::rootLayer(), mirror);
        if (!mirrorLayer) return false;
        const auto mirroredPath = motions.pushLayer(*mirrorLayer);
        normalSpinLayer = motions.pushLayer(*normalPath);
        if (!mirroredPath || !normalSpinLayer) return false;
        const auto mirroredSpin = motions.pushLayer(*mirroredPath);
        if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId)
            || !motions.bindMotion(*mirroredPath, *pathId)
            || !motions.bindMotion(*normalSpinLayer, *spinId)
            || !motions.bindMotion(*mirroredSpin, *spinId)
            || !scene.bindEntity(*first, *normalSpinLayer)
            || !scene.bindEntity(*second, *mirroredSpin)) return false;
        if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
            || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
            || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) return false;
        current.getCamera().position = {5, 3, 8};
        std::cout << "1 타원, 2 사각형, 3 다각형, 4 사용자 경로+보간, 5 원, 6 왕복 꺾은선\n"
                  << "Space 시간 정지/재개, S 자전 표시, G 전체 공전, M 반사 변경, I 레이어 삽입/삭제\n"
                  << "M: 없음 -> YZ평면 반사 -> 점대칭 -> Y직선 대칭 순환. 클릭: 객체 ID. Esc 종료\n";
        return true;
    });
    app.setKeyHandler([&](Application& current, int key, int action) {
        if (action != GLFW_PRESS) return;
        auto& motions = current.getScene().getMotions();
        bool ok = true;
        if (key == GLFW_KEY_ESCAPE) current.requestClose();
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) ok = choosePath(key - GLFW_KEY_1 + 1);
        else if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            ok = motions.setMotionPlaying(*pathId, !paused)
                && motions.setMotionPlaying(*spinId, !paused)
                && motions.setMotionPlaying(*globalId, !paused);
        } else if (key == GLFW_KEY_S) {
            spinEnabled = !spinEnabled;
            ok = motions.setMotionEnabled(*spinId, spinEnabled);
        } else if (key == GLFW_KEY_G) {
            globalEnabled = !globalEnabled;
            ok = motions.setMotionEnabled(*globalId, globalEnabled);
        } else if (key == GLFW_KEY_M) {
            reflection = (reflection + 1) % 4;
            std::optional<glm::mat4> matrix = glm::mat4(1);
            if (reflection == 1) matrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
            if (reflection == 2) matrix = TransformSystem::reflectPoint({0, 0, 0});
            if (reflection == 3) matrix = TransformSystem::reflectLine(AxisLine{});
            LayerConfig config;
            if (!matrix) ok = false;
            else { config.transform = *matrix; ok = motions.setLayer(*mirrorLayer, config); }
        } else if (key == GLFW_KEY_I) {
            if (insertedLayer) {
                ok = motions.removeLayer(*insertedLayer);
                if (ok) insertedLayer.reset();
            } else {
                LayerConfig config;
                config.space = TransformSpace::Local;
                const auto scale = TransformSystem::scaling(glm::vec3(1.5f));
                if (!scale) ok = false;
                else {
                    config.transform = *scale;
                    insertedLayer = motions.insertLayer(*normalSpinLayer, config);
                    ok = insertedLayer.has_value();
                }
            }
        }
        if (!ok) current.requestClose();
    });
    app.setMouseHandler([](Application& current, const MouseEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
        const auto hit = current.pickAt(event.windowX, event.windowY);
        if (hit) std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    });
    ApplicationConfig config;
    config.title = "Motion and layer tree";
    return app.run(config);
}
```

## openGL_3dAssign_18/framework/include/Transform.h

축·반사 행렬과 경로 함수의 공개 계약을 분리한다.

before: 없음

after:

```cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {

struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

using PathFunction = std::function<glm::vec3(double)>;
using InterpolationFunction = std::function<double(double)>;

// These functions return no matrix/path when their parameters are invalid.
class TransformSystem {
public:
    static bool validAffine(const glm::mat4& matrix);
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    static std::optional<glm::mat4> scaling(const glm::vec3& factors,
                                           const glm::vec3& center = glm::vec3(0));
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point,
                                               const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU,
                                const glm::vec3& axisV);
    static PathFunction circle(const glm::vec3& center, float radius,
                               const glm::vec3& normal = glm::vec3(0, 1, 0));
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    static PathFunction polygon(std::vector<glm::vec3> points);
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU,
                                  const glm::vec3& halfV);
    static double linear(double progress);
    static double smoothStep(double progress);
};
} // namespace graphics
#include "../detail/Transform.inl"
```

## openGL_3dAssign_18/framework/detail/Transform.inl

점·직선·평면 대칭과 회전, 길이 기반 선분 경로 및 원·타원 계산을 구현한다.

before: 없음

after:

```cpp
#pragma once
#include <algorithm>

namespace graphics {
namespace detail {
inline std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction)) return {};
    const double length = glm::length(glm::dvec3(direction));
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return glm::vec3(glm::dvec3(direction) / length);
}
inline std::optional<glm::mat4> motionAround(const glm::vec3& center,
                                           const glm::mat4& matrix) {
    if (!finiteVector(center)) return {};
    const auto result = glm::translate(glm::mat4(1), center) * matrix
                      * glm::translate(glm::mat4(1), -center);
    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result)
                                                : std::nullopt;
}
inline bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);
    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

inline bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0
        && m[2][3] == 0 && m[3][3] == 1;
}
inline std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset)) return {};
    return glm::translate(glm::mat4(1), offset);
}
inline std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors,
                                                       const glm::vec3& center) {
    if (!detail::finiteVector(factors)) return {};
    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}
inline std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis,
                                                        double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);
    if (!unit || !std::isfinite(degrees)) return {};
    const float radians = static_cast<float>(std::remainder(degrees, 360.0)
                                            * 3.14159265358979323846 / 180.0);
    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}
inline std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}
inline std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);
    if (!unit) return {};
    glm::mat3 matrix(0);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);
    return detail::motionAround(line.point, glm::mat4(matrix));
}
inline std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point,
                                                            const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);
    if (!unit) return {};
    glm::mat3 matrix(1);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];
    return detail::motionAround(point, glm::mat4(matrix));
}
inline PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u,
                                       const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;
        return center + u * static_cast<float>(std::cos(angle))
                      + v * static_cast<float>(std::sin(angle));
    };
}
inline PathFunction PathSystem::circle(const glm::vec3& center, float radius,
                                      const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);
    if (!n || !std::isfinite(radius) || radius <= 0) return {};
    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));
    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}
inline PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2) return {};
    for (const auto& p : points) if (!detail::finiteVector(p)) return {};
    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());
        if (points.size() < 4) return {};
    }
    std::vector<double> distances{0};
    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));
        if (!std::isfinite(length) || length <= 1e-12) return {};
        distances.push_back(distances.back() + length);
    }
    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1
                                                   : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);
        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight)
                       + glm::dvec3(points[i]) * weight);
    };
}
inline PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}
inline PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u,
                                         const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);
    if (std::abs(glm::dot(*a, *b)) > 1e-5f) return {};
    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}
inline double PathSystem::linear(double progress) { return progress; }
inline double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics
```

## openGL_3dAssign_18/framework/include/Motion.h

동작·레이어 식별자, 설정, 시간·합성·트리 편집 API를 공개한다.

before: 없음

after:

```cpp
#pragma once
#include "Transform.h"
#include <map>

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};
struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
};

class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
public:
    bool append(const MotionStep& step);
    bool insert(std::size_t index, const MotionStep& step);
    bool replace(std::size_t index, const MotionStep& step);
    bool erase(std::size_t index);
    bool then(MotionStep step); // Requires all previous steps to have finite durations.
    const std::vector<MotionStep>& getSteps() const { return steps_; }
    std::optional<MotionSample> sample(double seconds) const;
    static MotionStep fixed(const glm::mat4& matrix,
                            TransformSpace space = TransformSpace::World);
    static MotionStep rotation(const RotationMotion& config);
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};
struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;
public:
    static constexpr LayerId rootLayer() { return 0; }
    std::optional<MotionId> createMotion(const Motion& motion);
    bool setMotion(MotionId id, const Motion& motion);
    const MotionState* getMotion(MotionId id) const;
    bool setMotionPlaying(MotionId id, bool playing);
    bool setMotionEnabled(MotionId id, bool enabled);
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id);
    bool update(double deltaSeconds);
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    bool reparentLayer(LayerId id, LayerId parent);
    bool setLayer(LayerId id, const LayerConfig& config);
    const LayerState* getLayer(LayerId id) const;
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    std::optional<MotionId> createSpin(RotationMotion config);
    std::optional<MotionId> createOrbit(RotationMotion config);
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
#include "../detail/Motion.inl"
```

## openGL_3dAssign_18/framework/detail/Motion.inl

원자적 시각 반영, 동작 합성, 공유 시계, 레이어 편집과 행렬 합성을 구현한다.

before: 없음

after:

```cpp
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
        auto& target = step.space == TransformSpace::World ? result.parent : result.local;
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
inline bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id)) return false;
    for (auto& layer : layers_)
        if (layer.second.config.motion == id) layer.second.config.motion.reset();
    return true;
}
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
inline void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};
    motions_.clear();
    layers_.swap(emptyLayers);
}
inline bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform)
        && (!config.motion || getMotion(*config.motion));
}
inline const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);
    return found == layers_.end() ? nullptr : &found->second;
}
inline std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
    auto staged = layers_;
    const auto id = nextLayer_;
    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;
    return id;
}
inline std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);
    if (!current || !current->parent || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
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
inline bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);
    if (!current || !current->parent) return false;
    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());
    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());
    for (auto child : current->children) staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);
    return true;
}
inline std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);
    if (!current || !current->parent || !current->children.empty()) return {};
    const auto parent = *current->parent;
    if (!removeLayer(top)) return {};
    return parent;
}
inline bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);
    if (!current || !current->parent || !getLayer(parent)) return false;
    if (current->parent == parent) return true;
    auto ancestor = std::optional<LayerId>(parent);
    while (ancestor) {
        if (*ancestor == id) return false;
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
inline bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);
    if (found == layers_.end() || !validLayer(config)) return false;
    found->second.config = config;
    return true;
}
inline bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);
    if (!current || (motion && !getMotion(*motion))) return false;
    auto config = current->config;
    config.motion = motion;
    return setLayer(layer, config);
}
inline std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);
    while (current) {
        const auto layer = getLayer(*current);
        if (!layer || chain.size() >= layers_.size()) return {};
        chain.push_back(*current);
        current = layer->parent;
    }
    MotionSample result;
    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;
        if (!config.enabled) continue;
        MotionSample own;
        if (config.space == TransformSpace::World) own.parent = config.transform;
        else own.local = config.transform;
        if (config.motion) {
            const auto state = getMotion(*config.motion);
            if (!state) return {};
            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }
        if (*i == rootLayer()) result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }
        if (!TransformSystem::validAffine(result.parent)
            || !TransformSystem::validAffine(result.local)) return {};
    }
    return result;
}
inline std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit,
                                                            RotationMotion spin) {
    orbit.space = TransformSpace::World;
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
```

## openGL_3dAssign_18/framework/include/Graphics.h

Scene이 MotionSystem과 바인딩을 소유하고 공개 API로 연결한다.

### 블록 1: before L1~6, after L1~7

before:

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

```

after:

```cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

```

### 블록 2: before L24~29, after L25~32

before:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
```

after:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;
    void rebuildFaceIds();

public:
```

### 블록 3: before L52~57, after L55~67

before:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

after:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
    MotionSystem& getMotions() { return motions_; }
    const MotionSystem& getMotions() const { return motions_; }
    bool bindEntity(EntityId id, LayerId layer);
    bool unbindEntity(EntityId id);
    std::optional<LayerId> getEntityLayer(EntityId id) const;
    bool updateMotions(double deltaSeconds) { return motions_.update(deltaSeconds); }
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

## openGL_3dAssign_18/framework/detail/Scene.inl

기존 객체 SRT와 단일 레이어 가지를 합성하여 면·선·피킹에 같은 결과를 제공한다.

### 블록 1: before L212~239, after L212~274

before:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    glm::mat4 result(1);
    std::optional<EntityId> current = id;
    std::size_t remaining = components_.size();
    while (current) {
        if (*current >= components_.size() || remaining-- == 0) return {};
        const auto& component = components_[*current];
        result = detail::transformMatrix(component.transform) * result;
        if (!detail::finiteMatrix(result)) return {};
        current = component.parent;
    }
    return result;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto& component = components_[id];
    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
    if (component.parent) {
        const auto parent = worldMatrix(*component.parent);
        if (!parent) return {};
        result = *parent * result;
    }
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

after:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;
    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size()) return {};
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());
    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;
    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);
        if (binding) { layer = *binding; anchor = i - 1; break; }
    }
    const auto motion = motions_.layerMatrices(layer);
    if (!motion) return {};
    glm::mat4 prefix(1), suffix(1);
    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
    }
    const auto result = motion->parent * prefix * motion->local * suffix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto world = worldMatrix(id);
    if (!world) return {};
    const auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 child(1);
    if (face.independence)
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * detail::transformMatrix(face.transform);
    const auto result = *world * child * face.localAnimationMatrix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size()) return {};
    if (lines_[id].owner) return worldMatrix(*lines_[id].owner);
    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());
    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}
inline bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer)) return false;
    entityLayers_[id] = layer;
    return true;
}
inline bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size()) return false;
    entityLayers_.erase(id);
    return true;
}
inline std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto found = entityLayers_.find(id);
    if (found == entityLayers_.end() || !motions_.getLayer(found->second)) return {};
    return found->second;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

### 블록 2: before L324~329, after L359~366

before:

```cpp
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

after:

```cpp
}

inline void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

## openGL_3dAssign_18/framework/detail/Renderer.inl

소유자 없는 선에도 루트 행렬을 적용하고 소유 선은 객체와 같은 변환을 사용한다.

### 블록 1: before L152~159, after L152~158

before:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

after:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

### 블록 2: before L184~190, after L183~190

before:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

after:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

## openGL_3dAssign_18/framework/detail/Application.inl

사용자 update 후 렌더 전에 Motion을 한 번 갱신한다. 실패 시 기존 자원 정리 경로로 종료한다.

### 블록 1: before L172~177, after L172~182

before:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

after:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

## openGL_3dAssign_18/framework/examples/motion.cpp

사용자 확인용 독립 예제로 경로·공유 동작·반사·레이어 편집·피킹을 연결한다.

before: 없음

after:

```cpp
#include "../Framework.h"
#include <iostream>

int main() {
    using namespace graphics;
    Application app;
    std::optional<MotionId> pathId, spinId, globalId;
    std::optional<LayerId> mirrorLayer, normalSpinLayer, insertedLayer;
    bool paused = false, spinEnabled = true, globalEnabled = false;
    int reflection = 1;

    const auto choosePath = [&](int kind) {
        PathMotion path;
        path.durationSeconds = 6;
        const glm::vec3 center(1.7f, 0, 0);
        if (kind == 1) path.path = PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 2) path.path = PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 3)
            path.path = PathSystem::polygon({{1, -.5f, 0}, {2.4f, -.4f, 0},
                {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
        else if (kind == 4) {
            path.path = [center](double u) {
                const double angle = u * 6.283185307179586;
                return center + glm::vec3(float(.7 * std::sin(2 * angle)),
                    float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
            };
            path.interpolation = PathSystem::smoothStep;
        } else if (kind == 5) path.path = PathSystem::circle(center, .65f, {0, 0, 1});
        else {
            path.path = PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
            path.playback = Playback::PingPong;
        }
        Motion motion;
        auto& motions = app.getScene().getMotions();
        return pathId && motion.append(Motion::followPath(path))
            && motions.setMotion(*pathId, motion) && motions.seekMotion(*pathId, 0);
    };

    app.setInitHandler([&](Application& current) {
        auto& scene = current.getScene();
        auto& motions = scene.getMotions();
        const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        if (!first || !second) return false;
        if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) return false;
        pathId = motions.createMotion(Motion{});
        RotationMotion spin;
        spin.degreesPerSecond = 90;
        spinId = motions.createSpin(spin);
        RotationMotion global;
        global.degreesPerSecond = 15;
        globalId = motions.createOrbit(global);
        if (!pathId || !spinId || !globalId || !choosePath(1)) return false;
        if (!motions.setMotionEnabled(*globalId, false)
            || !motions.bindMotion(MotionSystem::rootLayer(), *globalId)) return false;

        const auto normalPath = motions.pushLayer(MotionSystem::rootLayer());
        const auto reflectionMatrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
        if (!normalPath || !reflectionMatrix) return false;
        LayerConfig mirror;
        mirror.transform = *reflectionMatrix;
        mirrorLayer = motions.pushLayer(MotionSystem::rootLayer(), mirror);
        if (!mirrorLayer) return false;
        const auto mirroredPath = motions.pushLayer(*mirrorLayer);
        normalSpinLayer = motions.pushLayer(*normalPath);
        if (!mirroredPath || !normalSpinLayer) return false;
        const auto mirroredSpin = motions.pushLayer(*mirroredPath);
        if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId)
            || !motions.bindMotion(*mirroredPath, *pathId)
            || !motions.bindMotion(*normalSpinLayer, *spinId)
            || !motions.bindMotion(*mirroredSpin, *spinId)
            || !scene.bindEntity(*first, *normalSpinLayer)
            || !scene.bindEntity(*second, *mirroredSpin)) return false;
        if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
            || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
            || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) return false;
        current.getCamera().position = {5, 3, 8};
        std::cout << "1 타원, 2 사각형, 3 다각형, 4 사용자 경로+보간, 5 원, 6 왕복 꺾은선\n"
                  << "Space 시간 정지/재개, S 자전 표시, G 전체 공전, M 반사 변경, I 레이어 삽입/삭제\n"
                  << "M: 없음 -> YZ평면 반사 -> 점대칭 -> Y직선 대칭 순환. 클릭: 객체 ID. Esc 종료\n";
        return true;
    });
    app.setKeyHandler([&](Application& current, int key, int action) {
        if (action != GLFW_PRESS) return;
        auto& motions = current.getScene().getMotions();
        bool ok = true;
        if (key == GLFW_KEY_ESCAPE) current.requestClose();
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) ok = choosePath(key - GLFW_KEY_1 + 1);
        else if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            ok = motions.setMotionPlaying(*pathId, !paused)
                && motions.setMotionPlaying(*spinId, !paused)
                && motions.setMotionPlaying(*globalId, !paused);
        } else if (key == GLFW_KEY_S) {
            spinEnabled = !spinEnabled;
            ok = motions.setMotionEnabled(*spinId, spinEnabled);
        } else if (key == GLFW_KEY_G) {
            globalEnabled = !globalEnabled;
            ok = motions.setMotionEnabled(*globalId, globalEnabled);
        } else if (key == GLFW_KEY_M) {
            reflection = (reflection + 1) % 4;
            std::optional<glm::mat4> matrix = glm::mat4(1);
            if (reflection == 1) matrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
            if (reflection == 2) matrix = TransformSystem::reflectPoint({0, 0, 0});
            if (reflection == 3) matrix = TransformSystem::reflectLine(AxisLine{});
            LayerConfig config;
            if (!matrix) ok = false;
            else { config.transform = *matrix; ok = motions.setLayer(*mirrorLayer, config); }
        } else if (key == GLFW_KEY_I) {
            if (insertedLayer) {
                ok = motions.removeLayer(*insertedLayer);
                if (ok) insertedLayer.reset();
            } else {
                LayerConfig config;
                config.space = TransformSpace::Local;
                const auto scale = TransformSystem::scaling(glm::vec3(1.5f));
                if (!scale) ok = false;
                else {
                    config.transform = *scale;
                    insertedLayer = motions.insertLayer(*normalSpinLayer, config);
                    ok = insertedLayer.has_value();
                }
            }
        }
        if (!ok) current.requestClose();
    });
    app.setMouseHandler([](Application& current, const MouseEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
        const auto hit = current.pickAt(event.windowX, event.windowY);
        if (hit) std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    });
    ApplicationConfig config;
    config.title = "Motion and layer tree";
    return app.run(config);
}
```

## openGL_3dAssign_19/framework/include/Transform.h

축·반사 행렬과 경로 함수의 공개 계약을 분리한다.

before: 없음

after:

```cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {

struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

using PathFunction = std::function<glm::vec3(double)>;
using InterpolationFunction = std::function<double(double)>;

// These functions return no matrix/path when their parameters are invalid.
class TransformSystem {
public:
    static bool validAffine(const glm::mat4& matrix);
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    static std::optional<glm::mat4> scaling(const glm::vec3& factors,
                                           const glm::vec3& center = glm::vec3(0));
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point,
                                               const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU,
                                const glm::vec3& axisV);
    static PathFunction circle(const glm::vec3& center, float radius,
                               const glm::vec3& normal = glm::vec3(0, 1, 0));
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    static PathFunction polygon(std::vector<glm::vec3> points);
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU,
                                  const glm::vec3& halfV);
    static double linear(double progress);
    static double smoothStep(double progress);
};
} // namespace graphics
#include "../detail/Transform.inl"
```

## openGL_3dAssign_19/framework/detail/Transform.inl

점·직선·평면 대칭과 회전, 길이 기반 선분 경로 및 원·타원 계산을 구현한다.

before: 없음

after:

```cpp
#pragma once
#include <algorithm>

namespace graphics {
namespace detail {
inline std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction)) return {};
    const double length = glm::length(glm::dvec3(direction));
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return glm::vec3(glm::dvec3(direction) / length);
}
inline std::optional<glm::mat4> motionAround(const glm::vec3& center,
                                           const glm::mat4& matrix) {
    if (!finiteVector(center)) return {};
    const auto result = glm::translate(glm::mat4(1), center) * matrix
                      * glm::translate(glm::mat4(1), -center);
    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result)
                                                : std::nullopt;
}
inline bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);
    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

inline bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0
        && m[2][3] == 0 && m[3][3] == 1;
}
inline std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset)) return {};
    return glm::translate(glm::mat4(1), offset);
}
inline std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors,
                                                       const glm::vec3& center) {
    if (!detail::finiteVector(factors)) return {};
    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}
inline std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis,
                                                        double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);
    if (!unit || !std::isfinite(degrees)) return {};
    const float radians = static_cast<float>(std::remainder(degrees, 360.0)
                                            * 3.14159265358979323846 / 180.0);
    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}
inline std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}
inline std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);
    if (!unit) return {};
    glm::mat3 matrix(0);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);
    return detail::motionAround(line.point, glm::mat4(matrix));
}
inline std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point,
                                                            const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);
    if (!unit) return {};
    glm::mat3 matrix(1);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];
    return detail::motionAround(point, glm::mat4(matrix));
}
inline PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u,
                                       const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;
        return center + u * static_cast<float>(std::cos(angle))
                      + v * static_cast<float>(std::sin(angle));
    };
}
inline PathFunction PathSystem::circle(const glm::vec3& center, float radius,
                                      const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);
    if (!n || !std::isfinite(radius) || radius <= 0) return {};
    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));
    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}
inline PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2) return {};
    for (const auto& p : points) if (!detail::finiteVector(p)) return {};
    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());
        if (points.size() < 4) return {};
    }
    std::vector<double> distances{0};
    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));
        if (!std::isfinite(length) || length <= 1e-12) return {};
        distances.push_back(distances.back() + length);
    }
    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1
                                                   : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);
        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight)
                       + glm::dvec3(points[i]) * weight);
    };
}
inline PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}
inline PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u,
                                         const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);
    if (std::abs(glm::dot(*a, *b)) > 1e-5f) return {};
    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}
inline double PathSystem::linear(double progress) { return progress; }
inline double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics
```

## openGL_3dAssign_19/framework/include/Motion.h

동작·레이어 식별자, 설정, 시간·합성·트리 편집 API를 공개한다.

before: 없음

after:

```cpp
#pragma once
#include "Transform.h"
#include <map>

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};
struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
};

class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
public:
    bool append(const MotionStep& step);
    bool insert(std::size_t index, const MotionStep& step);
    bool replace(std::size_t index, const MotionStep& step);
    bool erase(std::size_t index);
    bool then(MotionStep step); // Requires all previous steps to have finite durations.
    const std::vector<MotionStep>& getSteps() const { return steps_; }
    std::optional<MotionSample> sample(double seconds) const;
    static MotionStep fixed(const glm::mat4& matrix,
                            TransformSpace space = TransformSpace::World);
    static MotionStep rotation(const RotationMotion& config);
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};
struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;
public:
    static constexpr LayerId rootLayer() { return 0; }
    std::optional<MotionId> createMotion(const Motion& motion);
    bool setMotion(MotionId id, const Motion& motion);
    const MotionState* getMotion(MotionId id) const;
    bool setMotionPlaying(MotionId id, bool playing);
    bool setMotionEnabled(MotionId id, bool enabled);
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id);
    bool update(double deltaSeconds);
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    bool reparentLayer(LayerId id, LayerId parent);
    bool setLayer(LayerId id, const LayerConfig& config);
    const LayerState* getLayer(LayerId id) const;
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    std::optional<MotionId> createSpin(RotationMotion config);
    std::optional<MotionId> createOrbit(RotationMotion config);
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
#include "../detail/Motion.inl"
```

## openGL_3dAssign_19/framework/detail/Motion.inl

원자적 시각 반영, 동작 합성, 공유 시계, 레이어 편집과 행렬 합성을 구현한다.

before: 없음

after:

```cpp
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
        auto& target = step.space == TransformSpace::World ? result.parent : result.local;
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
inline bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id)) return false;
    for (auto& layer : layers_)
        if (layer.second.config.motion == id) layer.second.config.motion.reset();
    return true;
}
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
inline void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};
    motions_.clear();
    layers_.swap(emptyLayers);
}
inline bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform)
        && (!config.motion || getMotion(*config.motion));
}
inline const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);
    return found == layers_.end() ? nullptr : &found->second;
}
inline std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
    auto staged = layers_;
    const auto id = nextLayer_;
    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;
    return id;
}
inline std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);
    if (!current || !current->parent || !validLayer(config)
        || nextLayer_ == std::numeric_limits<LayerId>::max()) return {};
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
inline bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);
    if (!current || !current->parent) return false;
    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());
    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());
    for (auto child : current->children) staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);
    return true;
}
inline std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);
    if (!current || !current->parent || !current->children.empty()) return {};
    const auto parent = *current->parent;
    if (!removeLayer(top)) return {};
    return parent;
}
inline bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);
    if (!current || !current->parent || !getLayer(parent)) return false;
    if (current->parent == parent) return true;
    auto ancestor = std::optional<LayerId>(parent);
    while (ancestor) {
        if (*ancestor == id) return false;
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
inline bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);
    if (found == layers_.end() || !validLayer(config)) return false;
    found->second.config = config;
    return true;
}
inline bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);
    if (!current || (motion && !getMotion(*motion))) return false;
    auto config = current->config;
    config.motion = motion;
    return setLayer(layer, config);
}
inline std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);
    while (current) {
        const auto layer = getLayer(*current);
        if (!layer || chain.size() >= layers_.size()) return {};
        chain.push_back(*current);
        current = layer->parent;
    }
    MotionSample result;
    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;
        if (!config.enabled) continue;
        MotionSample own;
        if (config.space == TransformSpace::World) own.parent = config.transform;
        else own.local = config.transform;
        if (config.motion) {
            const auto state = getMotion(*config.motion);
            if (!state) return {};
            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }
        if (*i == rootLayer()) result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }
        if (!TransformSystem::validAffine(result.parent)
            || !TransformSystem::validAffine(result.local)) return {};
    }
    return result;
}
inline std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    Motion motion;
    if (!motion.append(Motion::rotation(config))) return {};
    return createMotion(motion);
}
inline std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit,
                                                            RotationMotion spin) {
    orbit.space = TransformSpace::World;
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
```

## openGL_3dAssign_19/framework/include/Graphics.h

Scene이 MotionSystem과 바인딩을 소유하고 공개 API로 연결한다.

### 블록 1: before L1~6, after L1~7

before:

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

```

after:

```cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

```

### 블록 2: before L24~29, after L25~32

before:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
```

after:

```cpp
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;
    void rebuildFaceIds();

public:
```

### 블록 3: before L52~57, after L55~67

before:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

after:

```cpp
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
    MotionSystem& getMotions() { return motions_; }
    const MotionSystem& getMotions() const { return motions_; }
    bool bindEntity(EntityId id, LayerId layer);
    bool unbindEntity(EntityId id);
    std::optional<LayerId> getEntityLayer(EntityId id) const;
    bool updateMotions(double deltaSeconds) { return motions_.update(deltaSeconds); }
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

```

## openGL_3dAssign_19/framework/detail/Scene.inl

기존 객체 SRT와 단일 레이어 가지를 합성하여 면·선·피킹에 같은 결과를 제공한다.

### 블록 1: before L212~239, after L212~274

before:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    glm::mat4 result(1);
    std::optional<EntityId> current = id;
    std::size_t remaining = components_.size();
    while (current) {
        if (*current >= components_.size() || remaining-- == 0) return {};
        const auto& component = components_[*current];
        result = detail::transformMatrix(component.transform) * result;
        if (!detail::finiteMatrix(result)) return {};
        current = component.parent;
    }
    return result;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto& component = components_[id];
    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
    if (component.parent) {
        const auto parent = worldMatrix(*component.parent);
        if (!parent) return {};
        result = *parent * result;
    }
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

after:

```cpp
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;
    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size()) return {};
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());
    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;
    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);
        if (binding) { layer = *binding; anchor = i - 1; break; }
    }
    const auto motion = motions_.layerMatrices(layer);
    if (!motion) return {};
    glm::mat4 prefix(1), suffix(1);
    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
    }
    const auto result = motion->parent * prefix * motion->local * suffix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto world = worldMatrix(id);
    if (!world) return {};
    const auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 child(1);
    if (face.independence)
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * detail::transformMatrix(face.transform);
    const auto result = *world * child * face.localAnimationMatrix;
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}
inline std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size()) return {};
    if (lines_[id].owner) return worldMatrix(*lines_[id].owner);
    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());
    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}
inline bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer)) return false;
    entityLayers_[id] = layer;
    return true;
}
inline bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size()) return false;
    entityLayers_.erase(id);
    return true;
}
inline std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto found = entityLayers_.find(id);
    if (found == entityLayers_.end() || !motions_.getLayer(found->second)) return {};
    return found->second;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
```

### 블록 2: before L324~329, after L359~366

before:

```cpp
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

after:

```cpp
}

inline void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();
    if (components_.empty() && lines_.empty()) {
        return;
    }
```

## openGL_3dAssign_19/framework/detail/Renderer.inl

소유자 없는 선에도 루트 행렬을 적용하고 소유 선은 객체와 같은 변환을 사용한다.

### 블록 1: before L152~159, after L152~158

before:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

after:

```cpp
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
```

### 블록 2: before L184~190, after L183~190

before:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

after:

```cpp
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
```

## openGL_3dAssign_19/framework/detail/Application.inl

사용자 update 후 렌더 전에 Motion을 한 번 갱신한다. 실패 시 기존 자원 정리 경로로 종료한다.

### 블록 1: before L172~177, after L172~182

before:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

after:

```cpp
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
```

## openGL_3dAssign_19/framework/examples/motion.cpp

사용자 확인용 독립 예제로 경로·공유 동작·반사·레이어 편집·피킹을 연결한다.

before: 없음

after:

```cpp
#include "../Framework.h"
#include <iostream>

int main() {
    using namespace graphics;
    Application app;
    std::optional<MotionId> pathId, spinId, globalId;
    std::optional<LayerId> mirrorLayer, normalSpinLayer, insertedLayer;
    bool paused = false, spinEnabled = true, globalEnabled = false;
    int reflection = 1;

    const auto choosePath = [&](int kind) {
        PathMotion path;
        path.durationSeconds = 6;
        const glm::vec3 center(1.7f, 0, 0);
        if (kind == 1) path.path = PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 2) path.path = PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 3)
            path.path = PathSystem::polygon({{1, -.5f, 0}, {2.4f, -.4f, 0},
                {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
        else if (kind == 4) {
            path.path = [center](double u) {
                const double angle = u * 6.283185307179586;
                return center + glm::vec3(float(.7 * std::sin(2 * angle)),
                    float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
            };
            path.interpolation = PathSystem::smoothStep;
        } else if (kind == 5) path.path = PathSystem::circle(center, .65f, {0, 0, 1});
        else {
            path.path = PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
            path.playback = Playback::PingPong;
        }
        Motion motion;
        auto& motions = app.getScene().getMotions();
        return pathId && motion.append(Motion::followPath(path))
            && motions.setMotion(*pathId, motion) && motions.seekMotion(*pathId, 0);
    };

    app.setInitHandler([&](Application& current) {
        auto& scene = current.getScene();
        auto& motions = scene.getMotions();
        const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        if (!first || !second) return false;
        if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) return false;
        pathId = motions.createMotion(Motion{});
        RotationMotion spin;
        spin.degreesPerSecond = 90;
        spinId = motions.createSpin(spin);
        RotationMotion global;
        global.degreesPerSecond = 15;
        globalId = motions.createOrbit(global);
        if (!pathId || !spinId || !globalId || !choosePath(1)) return false;
        if (!motions.setMotionEnabled(*globalId, false)
            || !motions.bindMotion(MotionSystem::rootLayer(), *globalId)) return false;

        const auto normalPath = motions.pushLayer(MotionSystem::rootLayer());
        const auto reflectionMatrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
        if (!normalPath || !reflectionMatrix) return false;
        LayerConfig mirror;
        mirror.transform = *reflectionMatrix;
        mirrorLayer = motions.pushLayer(MotionSystem::rootLayer(), mirror);
        if (!mirrorLayer) return false;
        const auto mirroredPath = motions.pushLayer(*mirrorLayer);
        normalSpinLayer = motions.pushLayer(*normalPath);
        if (!mirroredPath || !normalSpinLayer) return false;
        const auto mirroredSpin = motions.pushLayer(*mirroredPath);
        if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId)
            || !motions.bindMotion(*mirroredPath, *pathId)
            || !motions.bindMotion(*normalSpinLayer, *spinId)
            || !motions.bindMotion(*mirroredSpin, *spinId)
            || !scene.bindEntity(*first, *normalSpinLayer)
            || !scene.bindEntity(*second, *mirroredSpin)) return false;
        if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
            || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
            || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) return false;
        current.getCamera().position = {5, 3, 8};
        std::cout << "1 타원, 2 사각형, 3 다각형, 4 사용자 경로+보간, 5 원, 6 왕복 꺾은선\n"
                  << "Space 시간 정지/재개, S 자전 표시, G 전체 공전, M 반사 변경, I 레이어 삽입/삭제\n"
                  << "M: 없음 -> YZ평면 반사 -> 점대칭 -> Y직선 대칭 순환. 클릭: 객체 ID. Esc 종료\n";
        return true;
    });
    app.setKeyHandler([&](Application& current, int key, int action) {
        if (action != GLFW_PRESS) return;
        auto& motions = current.getScene().getMotions();
        bool ok = true;
        if (key == GLFW_KEY_ESCAPE) current.requestClose();
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) ok = choosePath(key - GLFW_KEY_1 + 1);
        else if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            ok = motions.setMotionPlaying(*pathId, !paused)
                && motions.setMotionPlaying(*spinId, !paused)
                && motions.setMotionPlaying(*globalId, !paused);
        } else if (key == GLFW_KEY_S) {
            spinEnabled = !spinEnabled;
            ok = motions.setMotionEnabled(*spinId, spinEnabled);
        } else if (key == GLFW_KEY_G) {
            globalEnabled = !globalEnabled;
            ok = motions.setMotionEnabled(*globalId, globalEnabled);
        } else if (key == GLFW_KEY_M) {
            reflection = (reflection + 1) % 4;
            std::optional<glm::mat4> matrix = glm::mat4(1);
            if (reflection == 1) matrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
            if (reflection == 2) matrix = TransformSystem::reflectPoint({0, 0, 0});
            if (reflection == 3) matrix = TransformSystem::reflectLine(AxisLine{});
            LayerConfig config;
            if (!matrix) ok = false;
            else { config.transform = *matrix; ok = motions.setLayer(*mirrorLayer, config); }
        } else if (key == GLFW_KEY_I) {
            if (insertedLayer) {
                ok = motions.removeLayer(*insertedLayer);
                if (ok) insertedLayer.reset();
            } else {
                LayerConfig config;
                config.space = TransformSpace::Local;
                const auto scale = TransformSystem::scaling(glm::vec3(1.5f));
                if (!scale) ok = false;
                else {
                    config.transform = *scale;
                    insertedLayer = motions.insertLayer(*normalSpinLayer, config);
                    ok = insertedLayer.has_value();
                }
            }
        }
        if (!ok) current.requestClose();
    });
    app.setMouseHandler([](Application& current, const MouseEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
        const auto hit = current.pickAt(event.windowX, event.windowY);
        if (hit) std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    });
    ApplicationConfig config;
    config.title = "Motion and layer tree";
    return app.run(config);
}
```

## framework-analysis/update_graph.py

독립 motion 예제를 프로젝트 진입점 색인에 포함한다.

### 블록 1: before L47~53, after L47~53

before:

```python
                    item.update(condition=f'PROB == {problem}',active_by_source_default=problem==2)
                entries.append(item)
        projects.append({'project':p.relative_to(REPOSITORY).as_posix(),'sources':sources,'entrypoints':entries})
    for name in ['basic','assets']:
        projects.append({'project':None,'standalone_example':True,'sources':[f'framework/examples/{name}.cpp'],
                         'entrypoints':[{'file':f'framework/examples/{name}.cpp','name':'main'}]})
    return projects
```

after:

```python
                    item.update(condition=f'PROB == {problem}',active_by_source_default=problem==2)
                entries.append(item)
        projects.append({'project':p.relative_to(REPOSITORY).as_posix(),'sources':sources,'entrypoints':entries})
    for name in ['basic','assets','motion']:
        projects.append({'project':None,'standalone_example':True,'sources':[f'framework/examples/{name}.cpp'],
                         'entrypoints':[{'file':f'framework/examples/{name}.cpp','name':'main'}]})
    return projects
```

## 배포 사본과 문서 범위

루트와 실습 13~19의 위 코드 파일은 같은 소스를 배포한다. 실습 코드에는 새 API 이식이나 키 정책 변경을 하지 않았다. README/APPLICATION_PATTERNS는 각 사본의 기존 설명을 보존하고 같은 새 절을 추가했다. 선행 확장 문서가 있는 루트와 사본의 기존 본문은 같다고 주장하지 않는다. USER_CHECKS에도 동일한 사용자 확인 항목을 추가했다.

- [공개 API와 사용법](../framework/README.md)
- [공유 동작 조립 패턴](../framework/docs/APPLICATION_PATTERNS.md)
- [사용자가 실행할 확인 절차](../framework/docs/USER_CHECKS.md)
- [프레임워크 호출 흐름](graphify-out/FRAMEWORK_FLOW.md)
- [독립 프로젝트 실행 흐름](../graphify-out/EXECUTION_FLOW.md)

각 문서의 2026-10-22 추가 절에는 함수 계약·계산·입력/상태/출력·실패 조건·사용자 관찰 지점을 담았다. 새 코드 예시는 위 motion.cpp 전체와 아래 문서 추가 절에 모두 포함한다. 원래 문서의 선행 미커밋 변경은 이 보고의 변경 코드로 취급하지 않는다.

### framework/README.md 및 실습 13~19 대응 문서의 추가 내용

before: 이 추가 절 없음. 기존 본문 보존.

after:

`````markdown
## 13. 변환·Motion·레이어 트리 (2026-10-22)

이 절은 공통 프레임워크에 추가한 API를 설명한다. 기존 실습의 Console 애니메이션은 유지한다. 새 API를 사용하지 않으면 Motion과 레이어는 항등변환이다. 카메라는 Scene 객체와 독립적이며 루트 레이어로 이동하지 않는다. 이 절과 예제는 빌드·실행하지 않았다.

### 13.1 구성과 프레임 순서

- `include/Transform.h`, `detail/Transform.inl`: 반사·회전·이동·크기 행렬 및 경로 함수.
- `include/Motion.h`, `detail/Motion.inl`: 시간에 따른 행렬 합성, 동작 식별자, 레이어 트리.
- `Scene::getMotions()`: 해당 장면의 MotionSystem 참조. `Framework.h` 또는 `Graphics.h`로 공개된다.
- `examples/motion.cpp`: 경로 선택, 공유 동작, 반사, 레이어 삽입·삭제, 클릭 피킹 예제. 다른 예제와 main이 중복되므로 하나만 프로젝트에 등록한다.

현재 프레임 순서는 `PollEvents -> 사용자 update 콜백 -> 종료 확인 -> Scene::updateMotions(dt) -> render 콜백/기본 렌더 -> SwapBuffers`다. Application::run 안에서는 updateMotions를 다시 호출하지 않는다. 콜백에서 만든 동작은 그 프레임의 dt만큼 진행된 뒤 렌더된다. run 밖에서 Scene과 Renderer를 직접 사용하면 호출자가 한 프레임에 한 번 updateMotions를 호출한다. 기존 dt의 0~0.05초 제한과 최소화 대기 처리는 유지한다. 실제 벽시계가 아니라 전달받은 dt의 누적 시간이다.

### 13.2 기본 행렬과 반사

```cpp
const auto rotate = graphics::TransformSystem::rotation(
    graphics::AxisLine{{1, 0, 0}, {0, 1, 0}}, 30.0);
const auto mirror = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
const auto pointSymmetry = graphics::TransformSystem::reflectPoint({1, 0, 0});
const auto lineSymmetry = graphics::TransformSystem::reflectLine(
    graphics::AxisLine{{0, 0, 0}, {0, 1, 0}});
```

반사는 회전 API와 분리한다. rotation은 축 위의 한 점과 축 방향을 받으며 양의 각도는 오른손 방향이다. 축 방향을 반대로 지정하면 같은 양의 각도도 반대 방향이다. 각도 단위는 도, 속도는 도/초다.

- `translation(offset)`: 이동 행렬.
- `scaling(factors, center)`: 지정 중심의 크기 변경. 음수와 0 배율도 허용한다.
- `rotation(axis, degrees)`: 지정 직선 주위의 회전.
- `reflectPlane(point, normal)`: 평면 양쪽의 거리를 뒤집는 평면 반사. determinant는 -1이다.
- `reflectPoint(center)`: 중심을 기준으로 세 좌표를 반전하는 점대칭. 3D에서는 determinant가 -1이다.
- `reflectLine(axis)`: 직선 방향 성분은 보존하고 수직 성분 둘을 뒤집는다. 3D에서 같은 직선 주위의 180도 회전과 수학적으로 같지만 별도 대칭 API로 제공한다.

기본식은 `T(center) * A * T(-center)`다. 평면 반사에서 A는 I-2nnᵀ, 직선 대칭에서는 2nnᵀ-I다. 방향 벡터는 내부에서 정규화한다. 영벡터·NaN·무한대나 계산 결과가 유효하지 않으면 optional이 비어 있다. 행렬은 유한한 아핀 행렬이어야 하며 투영 행렬은 허용하지 않는다.

기존 Transform3D.rotation은 순수 회전 조건을 유지한다. 반사 행렬을 이 필드에 억지로 넣지 않고 LayerConfig.transform 또는 Motion::fixed에 넣는다. Renderer는 면 컬링을 끄고 역전치 법선을 사용하며, 피킹은 양면 교차 검사와 같은 faceWorldMatrix를 사용한다. 0 배율로 붕괴한 면과 특이 행렬의 조명은 일반적인 입체 표면 결과를 보장하지 않는다.

### 13.3 경로 함수와 시간 보간

```cpp
graphics::PathMotion path;
path.path = graphics::PathSystem::ellipse({0, 0, 0}, {2, 0, 0}, {0, 0, 1});
path.interpolation = graphics::PathSystem::smoothStep;
path.durationSeconds = 4;
path.playback = graphics::Playback::Loop;
graphics::Motion motion;
if (!motion.append(graphics::Motion::followPath(path))) return false;
const auto id = scene.getMotions().createMotion(motion);
```

`PathFunction`은 `glm::vec3(double u)`, `InterpolationFunction`은 `double(double progress)` 형태의 std::function이다. 내부 흐름은 다음과 같다.

```text
q = phase + 경과초 / durationSeconds * rate
progress = Once의 clamp / Loop의 [0,1) 순환 / PingPong의 0↔1 왕복
u = interpolation(progress)
offset = path(u)
relativeToStart이면 offset -= path(interpolation(phase))
결과 = T(offset)
```

path의 반환값은 객체의 기존 위치를 직접 대입하는 절대 월드 좌표가 아니라 추가 이동량이다. 기존 배치가 원점이면 경로 좌표와 결과 위치가 일치한다. relativeToStart=true는 시작 위치를 보존하는 상대 이동에 사용한다. World/Local 공간에 따라 이 이동을 배치 앞/뒤에 적용한다. 객체가 경로 접선 방향을 자동으로 바라보지는 않는다. 방향 변화가 필요하면 별도 회전 Step 또는 사용자 행렬 함수를 합성한다.

| 설정 | 계약 |
|---|---|
| durationSeconds | 양의 유한값. rate=1일 때 경로 한 번의 진행 시간 |
| rate | 0이 아닌 유한값. 부호로 진행 방향 지정 |
| phase | 0~1의 시작 진행률. Once를 역방향으로 시작하려면 phase=1, rate<0 |
| playback | Once는 끝에서 유지, Loop는 순환, PingPong은 왕복 |
| relativeToStart | 시작 경로 위치를 차감할지 여부. 기본 false |
| space | World 또는 Local. 기본 World |

순환에서 열린 경로는 끝과 시작 사이에 점프한다. 보간 함수는 원래 시간 진행률 0~1을 받는다. 반환 u는 제한하지 않으므로 사용자 경로는 필요한 구간 밖 값도 처리해야 한다. 내장 polyline은 u를 0~1로 제한하고, ellipse는 주기 함수로 평가한다. 함수는 유한한 결과를 반환해야 한다.

PathSystem::ellipse(center, axisU, axisV)의 두 벡터는 반지름과 평면을 함께 정한다. 서로 평행하거나 길이가 0인 벡터는 거부한다. circle(center, radius, normal)은 법선에 수직인 원을 만든다. rectangle의 halfU/halfV는 수직인 반변 벡터다. polygon은 입력 순서대로 닫힌 경계를, polyline(points, closed=false)은 입력 순서대로 선분을 만든다. 연속 중복점·길이 0 선분은 거부한다. 닫힌 polygon은 마지막 점을 다시 적어도 된다.

polyline/polygon/rectangle은 선분 길이 누적으로 매개변수를 배분하므로 linear 보간에서 실제 이동 속도가 일정하다. 꼭짓점에서는 방향이 즉시 바뀐다. ellipse와 임의 사용자 경로는 매개변수 속도가 일정해도 실제 속도가 일정하지 않을 수 있다. smoothStep을 적용하면 선분 경로도 가감속한다. 일반 영역의 경계·내부 궤적은 사용자의 path 함수가 결정하며 영역만으로 경로를 자동 추론하지 않는다.

### 13.4 Motion 합성과 편의 API

Motion은 MotionStep의 목록이다. append/insert(index, step)/replace(index, step)/erase(index)로 편집하고 getSteps로 읽는다. 변경한 Motion을 setMotion(id, motion)에 다시 전달한다. 인덱스는 삽입·삭제 후 달라진다.

각 Step은 transform(elapsedSeconds), space, startSeconds, 선택적 durationSeconds, enabled를 가진다. 시작 전에는 기여하지 않고, 종료 시각 이후에는 마지막 행렬을 유지한다. 사용자 transform 콜백은 아핀 행렬을 반환한다. 같은 공간 안에서는 API로 추가한 순서대로 적용한다. 예를 들어 A 다음 B는 B*A다. World와 Local은 별도 목록으로 합성하므로 서로 다른 공간의 추가 순서로 두 공간을 교차 배치하지 않는다. 공간을 넘는 계층은 레이어로 표현한다.

```cpp
graphics::Motion combo;
graphics::RotationMotion orbit;
orbit.degreesPerSecond = -20;
orbit.axis = {{0, 0, 0}, {0, 1, 0}};
orbit.space = graphics::TransformSpace::World;
if (!combo.append(graphics::Motion::rotation(orbit))) return false;
const auto reflect = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
if (!reflect || !combo.append(graphics::Motion::fixed(*reflect))) return false;
```

append로 넣은 Step들은 각자의 startSeconds 기준으로 동시에 평가한다. then(step)은 앞선 모든 Step의 종료 시각 뒤에 배치한다. 앞선 Step 중 무기한 회전·반복 경로가 있으면 false다. 시간 순서 실행은 이전 변환을 삭제하거나 대체하지 않고 종료 행렬을 유지하며 다음 변환을 합성한다.

```cpp
graphics::Motion sequential;
graphics::MotionStep x;
x.durationSeconds = 1;
x.transform = [](double t) { return glm::translate(glm::mat4(1), glm::vec3(float(t), 0, 0)); };
graphics::MotionStep y;
y.durationSeconds = 1;
y.transform = [](double t) { return glm::translate(glm::mat4(1), glm::vec3(0, float(t), 0)); };
if (!sequential.append(x) || !sequential.then(y)) return false;
// t=0.5: (0.5,0,0), t=1.5: (1,0.5,0), t>=2: (1,1,0)의 추가 이동.
```

MotionSystem의 편의 생성 API는 기본 Step 합성을 재사용한다.

| API | 생성 결과 |
|---|---|
| createMotion(motion) | 사용자 조합 |
| createSpin(rotation) | space를 Local로 지정한 자전 |
| createOrbit(rotation) | space를 World로 지정한 공전 |
| createOrbitAndSpin(orbit, spin) | World 공전과 Local 자전의 독립 축·각속도 |
| createPathAndSpin(path, spin) | 사용자 경로의 이동과 Local 자전 |

createSpin의 기본 축은 모델 원점을 지나는 Y축이다. 모델의 localBasis를 중심으로 자전하려면 axis.point에도 같은 모델 좌표를 지정한다. createOrbit은 기존 모델 배치 전체를 지정 월드 직선 주위로 회전시킨다.

### 13.5 식별자와 재생 상태

- createMotion은 optional<MotionId>를 반환한다. ID는 MotionSystem 인스턴스에 속한다.
- getMotion은 const MotionState*를 반환한다. motion, timeSeconds, playing, enabled, sampled를 조회한다.
- setMotion은 설정을 교체하되 현재 시각·재생·활성 상태를 유지한다. 처음부터 시작하려면 seekMotion(id, 0)을 호출한다.
- setMotionPlaying(false)는 시계를 멈추고 마지막 변환을 유지한다. true는 그 시각부터 재개한다.
- setMotionEnabled(false)는 변환의 기여만 항등행렬로 바꾼다. playing=true이면 보이지 않는 동안에도 시간은 흐른다. 다시 활성화하면 현재 시각의 변환이 나타난다.
- seekMotion은 0 이상의 유한한 시각으로 이동하고 즉시 샘플을 바꾼다.
- removeMotion은 관련 레이어에서 그 동작 바인딩도 해제한다. 레이어의 고정 transform은 유지한다.
- update는 재생 중인 각 Motion을 한 번 평가한다. 같은 ID를 여러 레이어가 공유해도 시계가 여러 번 진행하지 않는다.
- Motion·Layer ID는 삭제나 clear 후에도 재사용하지 않는다. rootLayer()의 0만 장면 전체 루트로 유지한다. EntityId는 기존처럼 Scene::clear 후 재사용되므로 새로 받는다.

생성·설정·seek는 해당 시각의 콜백을 실행할 수 있다. bool/optional 실패는 식별자·매개변수 또는 행렬이 유효하지 않다는 뜻이다. 사용자 콜백 예외와 내장 경로의 비유한 결과 예외는 호출자에게 전파된다. Application::run은 기존 예외 정리 경로로 GPU·창 자원을 정리하고 재전파한다. update는 모든 샘플을 준비한 뒤 시계와 캐시를 반영하므로 중간 실패 시 일부 Motion만 시간이 진행하지 않는다. 콜백의 외부 부작용까지 되돌리지는 않는다.

콜백은 결정적 계산 함수로 사용하고, 평가 중 Scene·MotionSystem을 수정하거나 재진입하지 않는다. 캡처한 객체의 수명은 동작이 삭제될 때까지 유지한다. getMotion/getLayer/getComponent의 포인터를 수정 API 호출 전후로 장기 보관하지 않는다. API는 기존 Scene처럼 단일 렌더링 스레드에서 사용한다.

### 13.6 레이어 트리와 객체 바인딩

```cpp
auto& motions = scene.getMotions();
graphics::RotationMotion spin;
const auto spinId = motions.createSpin(spin);
const auto groupLayer = motions.pushLayer(graphics::MotionSystem::rootLayer());
if (!spinId || !groupLayer || !motions.bindMotion(*groupLayer, *spinId)) return false;
if (!scene.bindEntity(firstEntity, *groupLayer) || !scene.bindEntity(secondEntity, *groupLayer)) return false;
```

이 코드는 같은 자전 시각·파라미터를 공유한다. 각 객체에 직접 바인딩하면 각 객체의 모델 좌표에서 자전한다. 객체들의 공통 중심을 기준으로 함께 돌리려면 World 공전을 사용하거나, Scene::addGroup/setParent로 조립한 부모 객체에 Local 자전 레이어를 바인딩한다.

레이어는 root -> branch -> leaf 형태의 트리다. 각 루트-말단 경로가 하나의 변환 스택이며 형제 가지는 서로 영향을 주지 않는다. root의 고정 변환과 Motion은 미바인딩 객체·소유자 없는 선을 포함한 전체 Scene에 한 번 적용한다. LayerConfig는 transform, space, 선택적 motion, enabled를 가진다. 고정 transform과 Motion이 함께 있으면 같은 공간에서 고정 변환 다음 Motion을 적용한다. enabled=false는 그 레이어의 기여만 생략하며 자식과 공유 Motion 시계는 유지한다.

| API | 효과와 제한 |
|---|---|
| pushLayer(parent, config) | 부모의 마지막 자식으로 추가. 반환 ID를 다음 push의 부모로 쓰면 stack push |
| insertLayer(child, config) | 기존 child와 부모 사이에 삽입. child ID·객체 바인딩 보존 |
| popLayer(top) | 자식 없는 말단만 제거하고 부모 ID 반환. 루트·분기는 거부 |
| removeLayer(id) | 해당 노드만 제거. 자식은 기존 형제 위치에 부모로 승격. 루트 삭제 거부 |
| reparentLayer(id, parent) | 가지를 새 부모의 마지막 자식으로 이동. 자기 자신·자손 밑 연결 거부 |
| setLayer(id, config) | 고정 변환·공간·동작·활성 상태 수정. 트리 연결 보존 |
| getLayer(id) | const LayerState*로 설정·부모·자식 조회 |
| bindMotion(layer, id/nullopt) | Motion 연결/해제. 레이어당 하나이며 여러 Step은 Motion 안에서 합성 |
| Scene::bindEntity(entity, layer) | 객체당 말단 하나 연결. 기존 바인딩 교체 |
| Scene::unbindEntity(entity) | 직접 바인딩 해제. 부모의 바인딩 또는 루트로 복귀 |
| Scene::getEntityLayer(entity) | 직접 바인딩된 유효 ID만 반환. 상속된 ID는 반환하지 않음 |

같은 Scene 객체의 서로 다른 자식들을 독립시키려면 각자 다른 말단에 연결한다. 객체에 직접 연결이 없으면 가장 가까운 객체 조상의 바인딩을 사용한다. 직접 연결이 있으면 상속된 가지를 대체한다. 부모·자식에서 바인딩을 중복 누적하지 않는다. 부모와 자식의 효과를 모두 원하면 자식의 레이어 가지가 원하는 상위 레이어를 포함하도록 구성한다. 객체 부모 트리와 레이어 트리 사이에 포인터나 같은 모양을 강제하지 않는다.

삭제된 레이어의 직접 바인딩은 조회 시 무효 처리한다. 해당 객체는 가까운 객체 조상 바인딩 또는 루트로 복귀한다. 승격된 자식 레이어의 바인딩은 보존한다. reparent/remove는 월드 자세를 보존하는 역행렬 보정을 하지 않으므로 새 합성 결과로 바뀐다.

최종 행렬은 다음과 같다.

```text
P = 루트의 전역 변환 * 가지의 World 변환들
L = 가지의 Local 변환들
A = 객체 부모 계층의 기존 SRT 중 바인딩된 객체까지
D = 바인딩된 객체 아래 자손의 기존 SRT
M_entity = P * A * L * D
M_face = M_entity * 면의 기존 추가 SRT * localAnimationMatrix
```

직접 바인딩된 객체는 D=I다. 상속이면 Local 변환은 바인딩한 조상 위치에 들어가므로 그 그룹 전체가 함께 움직인다. 가지 내부는 루트에서 말단 순서로 행렬을 오른쪽에 쌓는다. 루트의 Local 기여도 장면 전체 변환으로 처리한다. World는 객체의 Scene 부모 좌표가 아니라 기존 객체 배치 전체의 바깥쪽, 즉 월드 측 공간이다.

소유자가 있는 선은 owner의 M_entity를, 소유자가 없는 선은 루트의 전역 행렬만 사용한다. 색·표시 여부·카메라·메시 정점은 이 레이어에서 수정하지 않는다. 월드 행렬을 조회할 때 객체·레이어의 깊이에 따라 CPU 순회와 임시 저장이 필요하다. 성능 측정은 하지 않았다.

실행 확인 절차는 [USER_CHECKS.md](docs/USER_CHECKS.md)의 Motion 절, 조립 예시는 [APPLICATION_PATTERNS.md](docs/APPLICATION_PATTERNS.md)의 추가 절을 참고한다.
`````

### framework/docs/APPLICATION_PATTERNS.md 및 실습 13~19 대응 문서의 추가 내용

before: 이 추가 절 없음. 기존 본문 보존.

after:

`````markdown
## 15. Motion과 레이어로 공유 동작 조립 (2026-10-22)

**목적.** 실습별 각도·시간 계산을 대체할 수 있는 공통 동작 API를 제공한다. 기존 실습의 Console을 새 API로 자동 이식하지는 않는다. 기본 계약은 [사용설명서 13절](../README.md#13-변환motion레이어-트리-2026-10-22)에 있다.

**입력 -> 상태 -> 출력.** 축·중심·속도 또는 path/interpolation -> MotionStep 목록과 MotionId별 누적 시각 -> World/Local 행렬 -> 레이어 가지 -> Scene::worldMatrix/faceWorldMatrix -> Renderer의 uModel/uMVP/uNormalMatrix 및 같은 행렬을 사용하는 피킹.

**패턴 A: 공전과 자전 분리.** createOrbitAndSpin은 서로 다른 축·초당 각속도를 받는다. 공전은 World, 자전은 Local에 합성한다. 원형 공전이면 기존 배치의 중심을 축 주위로 돌리고, 타원·사각형·다각형 궤적이면 PathMotion과 createPathAndSpin을 사용한다. 경로 이동만 원하면 Motion::followPath 하나만 추가한다.

**패턴 B: 부모 그룹과 개별 자전.** addGroup/setParent는 기존 조립 위치를 관리한다. 그룹에 연결된 Local Motion은 자식 모두에 적용된다. 자식이 직접 다른 레이어에 연결되면 그 가지가 그룹에서 상속한 가지를 대체한다. 그룹 공전과 자식 자전을 함께 적용하려면 레이어를 `root -> 공전 -> 자전`으로 구성하여 자식을 말단에 연결한다. 같은 변환을 두 트리에서 중복 평가하지 않는다.

**패턴 C: 같은 경로를 공유하고 한쪽을 반사.** [motion.cpp](../examples/motion.cpp)는 다음 구조를 만든다.

```text
root (전체 Y 공전; 초기 기여 비활성)
├─ 일반 경로 (pathId) -> 개별 자전 (spinId) -> 첫 객체 바인딩
└─ 반사 행렬 -> 반사 쪽 경로 (같은 pathId) -> 개별 자전 (같은 spinId) -> 둘째 객체 바인딩
```

두 가지는 같은 pathId·spinId를 참조하므로 시계가 동일하다. 반사가 경로보다 바깥에 있어 경로 이동량과 형상을 함께 반사한다. 반사를 경로 아래에 놓으면 이동량은 그대로이고 그 안쪽 좌표만 반사된다. 일반 객체의 위치가 (2,1,0)이면 YZ평면 반사 객체는 (-2,1,0), 점대칭 객체는 (-2,-1,0), Y축 직선대칭 객체는 (-2,1,0)이다. 평면과 직선 대칭을 구분하려면 z가 0이 아닌 사용자 경로(4번)를 관찰한다.

**패턴 D: 스택 편집.** 예제의 I키는 자전 말단 위에 Local 1.5배 레이어를 삽입한다. 말단 ID와 객체 바인딩은 유지되며 크기만 바뀐다. 다시 I키를 누르면 removeLayer로 삽입 노드를 제거하고 자전 레이어를 원래 부모로 승격한다. pushLayer가 반환한 말단을 계속 부모로 사용하면 push, popLayer로 그 말단을 제거하면 pop이다. 분기 노드에 popLayer를 호출하면 실패하므로 removeLayer 또는 연결 변경을 명시적으로 사용한다.

**패턴 E: 시간 순서 실행.** append는 동시 평가, then은 유한한 이전 종료 시각 뒤에 배치한다. 이전 결과는 유지된다. 예를 들어 X 1단위 이동 후 Y 1단위 이동은 최종적으로 (1,1,0)의 추가 이동이다. 무기한 자전 뒤 then은 거부하므로 먼저 Step.durationSeconds를 지정하거나 서로 다른 레이어에서 동시에 실행한다.

**주의.** PathMotion의 결과는 기존 위치에 대한 추가 변환이다. 기존 모델 위치가 0이 아닐 때 경로의 중심을 그대로 넣으면 두 이동이 합성된다. 모델 원점·localBasis·축 기준점을 맞추고 World/Local을 선택한다. 타원 경로는 각도 매개변수 속도와 실제 이동 속도가 다르다. 모든 사용자 함수는 평가 중 Scene이나 MotionSystem을 수정하지 않는 계산 함수로 작성한다.

**사용자 확인.** 경로·반사·자전을 각각 켜고 끈 뒤 합성한다. Space는 마지막 자세에서 시간을 정지·재개하고, S는 자전 기여 자체를 제거·복구한다. 두 기능의 결과가 다름을 확인한다. 새 예제는 독립 main이며 기존 실습 13~19의 키 기능을 바꾸지 않는다.
`````

### framework/docs/USER_CHECKS.md 및 실습 13~19 대응 문서의 추가 내용

before: 이 추가 절 없음. 기존 본문 보존.

after:

`````markdown
## Motion·반사·레이어 트리 사용자 확인 (2026-10-22)

아래 확인은 에이전트가 실행하지 않았다. 사용자가 Visual Studio C++17·x64에서 Debug/Release로 빌드하고 실행한다. framework/examples/motion.cpp를 main 하나로 등록하고 해당 framework/Framework.props를 적용한다. 기존 실습 main과 동시에 등록하지 않는다. 프로젝트 폴더의 상대경로 배포 설정은 기존 예제와 같다.

| 조작·입력 | 예상 결과와 관찰 지점 |
|---|---|
| 초기 실행 | 두 직육면체가 타원 경로와 Y자전을 공유. 파란 객체와 부착 기저는 YZ평면 반사 위치에 표시 |
| 1 / 2 / 3 / 5 | 타원 / 사각형 / 다각형 / 원 경로로 변경. 새 경로 시작 시각은 0으로 재설정 |
| 4 | 사용자 3D 경로와 smoothStep 보간. 구간 시작·끝에서 속도가 작아짐 |
| 6 | 열린 꺾은선을 끝과 시작에서 끊어 이동하지 않고 왕복 |
| Space | 모든 동작의 마지막 자세를 유지. 다시 누르면 같은 위상에서 계속 |
| S | 자전 기여만 항등으로 전환·복구. 경로는 계속되며 복구 시 현재 자전 시각으로 이동 |
| G | 루트 Y공전 기여 전환. 두 객체와 소유자 없는 세로 기준선 모두 영향받음 |
| M 반복 | 현재 평면 반사 -> 점대칭 -> Y축 직선대칭 -> 반사 없음 -> 평면 반사. 4번 3D 경로로 z 반전 차이 관찰 |
| I 반복 | 첫 객체에 Local 1.5배 레이어 삽입/삭제. 경로와 말단 바인딩은 유지. 둘째 객체 크기는 그대로 |
| 두 객체 클릭 | 화면에 보이는 위치에서 entity/face ID 출력. 반사·자전·경로·전체 공전 중에도 선택 위치 일치 |
| 최소화·복구, 창 크기 변경, Esc | 최소화 시간의 큰 점프 없이 재개, 화면 비율·선택 확인, 정상 종료 |

API 확인용으로 별도 Scene을 만들고 다음 조건도 사용자가 확인한다. 실행형 테스트 파일은 추가하지 않았다.

1. 회전 axis={{1,0,0},{0,1,0}}, angle=90에서 축 위 점 (1,2,0)은 유지되고 (2,0,0)은 (1,0,-1)로 이동한다. 음수 각도와 반대 방향 축의 차이를 비교한다.
2. reflectPlane의 normal=(1,0,0)은 (2,3,4)를 (-2,3,4)로, reflectPoint(0)는 (-2,-3,-4)로, reflectLine(Y)는 (-2,3,-4)로 바꾼다. 반사 두 번은 원래 위치로 복귀한다.
3. 길이 1과 3의 두 선분 polyline({(0,0,0),(1,0,0),(1,3,0)})에 linear를 쓰면 u=.25가 중간 꼭짓점 (1,0,0)이다. ellipse의 같은 u 간격이 같은 거리인지는 별개다.
4. README의 순차 X/Y 예제를 sample(.5), sample(1.5), sample(2)로 확인한다. 무기한 Step 다음 then, 영축 회전, 비직교 rectangle, 연속 중복 polyline, NaN 행렬, 없는 ID는 거부되어야 한다.
5. Layer root의 1단위 X이동은 미바인딩 객체와 소유자 없는 선에도 한 번 적용된다. 부모·자식에 같은 레이어를 연결해도 두 번 적용되지 않는다.
6. addGroup의 자식들이 부모 바인딩을 상속하면 부모의 Local 회전 중심으로 함께 움직인다. 자식의 직접 바인딩은 상속 가지를 대체하고, unbindEntity는 다시 상속시킨다.
7. root 아래 A 아래 B에서 reparentLayer(A,B)는 false이고 원래 연결을 유지한다. insertLayer(B)는 B의 바인딩을 보존한다. removeLayer(삽입 ID)는 B를 승격한다. popLayer(A)는 B가 있으면 실패한다.
8. Motion 삭제는 레이어의 동작 연결만 해제하고 고정 행렬은 유지한다. 레이어 삭제의 직접 바인딩은 가까운 객체 조상/루트로 복귀한다. clear 후 이전 Motion/Layer ID는 무효이며 새 ID는 재사용되지 않는다.
9. 둘째 Motion의 사용자 콜백이 NaN 행렬을 반환하면 update는 false이고 모든 Motion 시각은 이전 상태다. 콜백이 예외를 던지면 시각은 유지되고 예외가 전파된다. 콜백 외부의 부작용은 되돌리지 않는다.
10. 새 API를 사용하지 않는 기존 실습 13~19도 직접 빌드·입력·화면 확인한다. 정적 문서·행렬 설명·Git 커밋은 이 결과를 보증하지 않는다.
`````

## 파일 대조와 미실행 사항

| 루트 파일 | SHA-256 | 실습 13~19와 바이트 일치 |
|---|---|---|
| framework/include/Transform.h | e54d0f3eefd00199ababfd2f655cfb7ad07bf02d4da334efab5acf7d4792f8b0 | True |
| framework/detail/Transform.inl | 37dd31736c43d0d709e464b2e3df5db46cf070ab426e0a801d8789c090c38980 | True |
| framework/include/Motion.h | 67e7a6270c08d56b8fc765b230f7f626952ad87a05d346b4a1d87c4da6bbffdf | True |
| framework/detail/Motion.inl | 4d5a06b0d676c691a58d192891d467600e6e9d99d89c7483a457941f0a7a3131 | True |
| framework/include/Graphics.h | 8f4da33f8d634c6b366acdb6ff7cdc0690f8a49b770398b12a00b31aaddc06b1 | True |
| framework/detail/Scene.inl | 241c5341d78382c33b3765f17a6a93394fd00726a5d0c10ff13c80c8ce428cd8 | True |
| framework/detail/Renderer.inl | c4f09bbd2efb9a92c6443ddbf7be5e9b76d9a8a01be0af2f89b08c4ac833ccc4 | True |
| framework/detail/Application.inl | 11af563d7b62ce897ca083fb2435d0ba1286210582c040c51497074e855f06ef | True |
| framework/examples/motion.cpp | ac1884af0d3bef4800e1c9a0972c357b102e9451fc3dec4e6aba25618694c62c | True |

총 코드 파일 73개, 변경 블록 105개를 기록했다. 파일·본문·인코딩·배포 사본의 읽기 전용 대조만 수행했다. 빌드·테스트·화면·피킹·수학 예제 실행·성능 측정·사용자 이해 확인은 수행하지 않았다.

graphify의 code-only AST와 소스 정의 앵커를 사용했다. framework-analysis/graphify-out은 997노드·1486간선, 저장소 graphify-out은 3329노드·5295간선이다. 각 GRAPH_HEALTH에는 미해결 끝점 등 추출 한계가 포함된다. 그래프 간선은 동적 콜백 실행의 완전한 증거가 아니다. API 키·외부 의미 분석·추가 의존성 설치는 사용하지 않았다.
