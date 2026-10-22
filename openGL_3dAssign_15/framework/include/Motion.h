#pragma once
#include "Transform.h"
#include <map>

namespace graphics {
using MotionId = std::uint64_t;
enum class TransformSpace { Parent, Local };
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
    TransformSpace space = TransformSpace::Parent;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::Parent;
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
                            TransformSpace space = TransformSpace::Parent);
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

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
public:
    std::optional<MotionId> createMotion(const Motion& motion);
    bool setMotion(MotionId id, const Motion& motion);
    const MotionState* getMotion(MotionId id) const;
    bool setMotionPlaying(MotionId id, bool playing);
    bool setMotionEnabled(MotionId id, bool enabled);
    bool seekMotion(MotionId id, double seconds);
    bool removeMotion(MotionId id);
    bool update(double deltaSeconds);
    void clear(); // Identifiers are never reused by this system, including after clear.
    std::optional<MotionId> createSpin(RotationMotion config);
    std::optional<MotionId> createOrbit(RotationMotion config);
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
#include "../detail/Motion.inl"
