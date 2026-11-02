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
    std::optional<MotionSample> sample(double seconds) const; // seconds는 프레임 간격이 아닌 절대 시간(초).
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
    bool setMotionEnabled(MotionId id, bool enabled); // playing과 별개로 변환 기여 여부를 설정한다.
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id); // 연결된 레이어의 모션 연결도 해제한다.
    bool update(double deltaSeconds); // 모든 샘플 계산이 성공하면 상태를 반영한다.
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{}); // root 위에는 삽입할 수 없다.
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
