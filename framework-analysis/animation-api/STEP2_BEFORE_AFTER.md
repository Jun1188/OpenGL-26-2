# 2단계: 시간 모션·사람형·그네 계층 API
새 API와 표준 예제를 추가했다. 기존 보행 API는 유지한다. 전체 변경 블록의 전후 코드를 아래에 싣는다. 빌드·테스트는 사용자에게 맡긴다.

## framework/include/Motion.h
### before (L27)
```
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
```
### after (L27)
```
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct LinearMotion {
    glm::vec3 from{0}, to{1, 0, 0};
    double durationSeconds = 1;
    Playback playback = Playback::Once;
    TransformSpace space = TransformSpace::Local;
};
struct PendulumMotion {
    AxisLine axis{{0, 0, 0}, {1, 0, 0}};
    double amplitudeDegrees = 25;
    double periodSeconds = 3;
    double phaseDegrees = 0;
    double offsetDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
```
### before (L62)
```
    static MotionStep rotation(const RotationMotion& config);
    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
```
### after (L76)
```
    static MotionStep rotation(const RotationMotion& config);
    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);
    /// [API] 시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.
    static MotionStep linear(const LinearMotion& config);
    /// [API] 지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.
    static MotionStep pendulum(const PendulumMotion& config);
};

struct MotionState {
```

## framework/detail/Motion.inl
### before (L103)
```
    };
    return step;
}
inline std::optional<MotionId> MotionSystem::createMotion(const Motion& motion) {
    const auto sample = motion.sample(0);
    if (!sample || nextMotion_ == std::numeric_limits<MotionId>::max()) return (Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Motion, "MotionSystem::createMotion", "Rejected condition: !sample || nextMotion_ == std::numeric_limits<MotionId>::max()", "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."), std::nullopt);
```
### after (L103)
```
    };
    return step;
}
inline MotionStep Motion::linear(const LinearMotion& config) {
    if (!detail::finiteVector(config.from) || !detail::finiteVector(config.to)
        || !std::isfinite(config.durationSeconds) || config.durationSeconds <= 0
        || (config.playback != Playback::Once && config.playback != Playback::Loop
            && config.playback != Playback::PingPong)
        || (config.space != TransformSpace::World && config.space != TransformSpace::Local)) return {};
    PathMotion path;
    path.path = [from = glm::dvec3(config.from), to = glm::dvec3(config.to)](double u) {
        return glm::vec3((1 - u) * from + u * to);
    };
    path.durationSeconds = config.durationSeconds;
    path.playback = config.playback;
    path.space = config.space;
    return followPath(path);
}
inline MotionStep Motion::pendulum(const PendulumMotion& config) {
    MotionStep step;
    if (!std::isfinite(config.amplitudeDegrees) || config.amplitudeDegrees < 0
        || !std::isfinite(config.periodSeconds) || config.periodSeconds <= 0
        || !std::isfinite(config.phaseDegrees) || !std::isfinite(config.offsetDegrees)
        || !std::isfinite(std::abs(config.offsetDegrees) + config.amplitudeDegrees)
        || (config.space != TransformSpace::World && config.space != TransformSpace::Local)
        || !TransformSystem::rotation(config.axis, config.offsetDegrees)) return step;
    step.space = config.space;
    step.transform = [config](double seconds) {
        constexpr double pi = 3.14159265358979323846;
        const double phase = std::remainder(seconds, config.periodSeconds) / config.periodSeconds * 2 * pi
                           + std::remainder(config.phaseDegrees, 360.0) * pi / 180;
        const auto matrix = TransformSystem::rotation(config.axis,
            config.offsetDegrees + config.amplitudeDegrees * std::sin(phase));
        if (!matrix) throw std::domain_error("Pendulum motion produced an invalid matrix.");
        return *matrix;
    };
    return step;
}
inline std::optional<MotionId> MotionSystem::createMotion(const Motion& motion) {
    const auto sample = motion.sample(0);
    if (!sample || nextMotion_ == std::numeric_limits<MotionId>::max()) return (Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Motion, "MotionSystem::createMotion", "Rejected condition: !sample || nextMotion_ == std::numeric_limits<MotionId>::max()", "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."), std::nullopt);
```

## framework/Framework.h
### before (L2)
```
// Copy this complete folder, then include this entry point. C++17, Windows x64.
#include "include/Application.h"
#include "include/Patterns.h"
```
### after (L2)
```
// Copy this complete folder, then include this entry point. C++17, Windows x64.
#include "include/Application.h"
#include "include/Patterns.h"
#include "include/Animation.h"
```

## framework/include/Animation.h
### before
없음
### after
```
#pragma once
#include "Patterns.h"

namespace graphics {
struct JointAnimation {
    EntityId joint = 0;
    Transform3D rest;
    Motion motion;
};
struct HumanoidConfig {
    GridLayout grid{glm::vec3(-.5f, 0, -.3f), glm::vec3(1, 1.8f, .6f)};
    float upperArmFraction = .5f, upperLegFraction = .5f;
    GlColor torsoColor{.65f, .65f, .7f}, headColor{1, .8f, .5f};
    GlColor leftColor{.2f, .6f, 1}, rightColor{1, .4f, .2f};
};
struct HumanoidRig {
    EntityId root = 0, pelvis = 0, torso = 0, neck = 0;
    EntityId torsoMesh = 0, head = 0, nose = 0;
    std::array<EntityId, 2> shoulders{}, elbows{}, hands{}, hips{}, knees{}, feet{};
    std::array<EntityId, 2> upperArms{}, forearms{}, thighs{}, shins{};
};
struct HumanoidPose {
    glm::vec3 pelvisDegrees{0}, torsoDegrees{0}, neckDegrees{0};
    std::array<glm::vec3, 2> shoulders{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> elbows{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> hips{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> knees{glm::vec3(0), glm::vec3(0)};
    /// [API] 허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.
    static HumanoidPose seated();
};
struct SwingConfig {
    glm::vec3 pivot{0, 3, 0}, seatSize{1.4f, .12f, .7f};
    float length = 2, ropeThickness = .035f;
    PendulumMotion pendulum;
    HumanoidConfig person;
    double riderSwayDegrees = 8;
};
struct SwingRig {
    EntityId frame = 0, hinge = 0, seat = 0;
    std::array<EntityId, 2> ropes{};
    HumanoidRig person;
    std::vector<JointAnimation> animation;
};
class AnimationSystem {
public:
    /// [API] 관절의 현재 강체 자세와 모션을 보관한다. 자세를 편집한 뒤 다시 만들면 새 기준 자세를 사용한다.
    static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);
    /// [API] 초 단위 절대 시간으로 관절들을 샘플링한 뒤 모두 유효할 때 반영한다. Scene 부모 계층을 유지한다.
    static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);
    /// [API] 실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.
    static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{},
                                                 std::optional<EntityId> parent = std::nullopt);
    /// [API] 관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.
    static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);
    /// [API] 고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.
    static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{},
                                                   std::optional<EntityId> parent = std::nullopt);
};
} // namespace graphics
#include "../detail/Animation.inl"
```

## framework/detail/Animation.inl
### before
없음
### after
```
#pragma once
#include <set>

namespace graphics {
inline HumanoidPose HumanoidPose::seated() {
    HumanoidPose pose;
    for (std::size_t i = 0; i < 2; ++i) {
        pose.hips[i].x = -90;
        pose.knees[i].x = 90;
        pose.shoulders[i].x = -20;
        pose.elbows[i].x = -40;
    }
    return pose;
}
inline std::optional<JointAnimation> AnimationSystem::createJointAnimation(
        const Scene& scene, EntityId joint, const Motion& motion) {
    const auto* component = scene.getComponent(joint);
    const auto sample = motion.sample(0);
    if (!component || !sample || !detail::rigidRoom(DiagramSystem::modelMatrix(component->transform))
        || !detail::rigidRoom(sample->parent) || !detail::rigidRoom(sample->local)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::createJointAnimation",
            "Need an existing rigid joint and a rigid motion sample. Scale the child mesh instead of the joint.");
        return {};
    }
    return JointAnimation{joint, component->transform, motion};
}
inline bool AnimationSystem::poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds) {
    if (!std::isfinite(seconds) || seconds < 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy", "Need nonnegative finite seconds.");
    std::set<EntityId> seen;
    std::vector<std::pair<EntityId, Transform3D>> staged;
    staged.reserve(animation.size());
    for (const auto& track : animation) {
        const auto sample = track.motion.sample(seconds);
        const auto rest = DiagramSystem::modelMatrix(track.rest);
        if (!scene.getComponent(track.joint) || !seen.insert(track.joint).second || !sample
            || !detail::rigidRoom(rest) || !detail::rigidRoom(sample->parent) || !detail::rigidRoom(sample->local))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy",
                "Need unique existing joints, rigid rest poses and rigid motion samples.");
        // World contributions are in the joint parent's space, before its rest pose.
        const auto matrix = sample->parent * rest * sample->local;
        Transform3D transform;
        transform.basis = glm::vec3(matrix[3]);
        transform.rotation = glm::mat3(matrix);
        if (!DiagramSystem::validTransform(transform))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy", "Joint composition overflow.");
        staged.emplace_back(track.joint, transform);
    }
    for (const auto& item : staged) if (!scene.setTransform(item.first, item.second)) return false;
    return true;
}
inline std::optional<HumanoidRig> AnimationSystem::addHumanoid(Scene& scene, const HumanoidConfig& config,
                                                               std::optional<EntityId> parent) {
    if (!config.grid.valid() || !detail::rigidRoom(config.grid.frame)
        || !std::isfinite(config.upperArmFraction) || config.upperArmFraction <= 0 || config.upperArmFraction >= 1
        || !std::isfinite(config.upperLegFraction) || config.upperLegFraction <= 0 || config.upperLegFraction >= 1
        || !detail::validColor(config.torsoColor) || !detail::validColor(config.headColor)
        || !detail::validColor(config.leftColor) || !detail::validColor(config.rightColor)
        || (parent && !scene.getComponent(*parent))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addHumanoid", "Invalid grid, limb fractions, colors or parent.");
        return {};
    }
    Scene staged = scene;
    HumanoidRig rig;
    const auto extent = config.grid.extent;
    Transform3D rootTransform;
    rootTransform.basis = glm::vec3(config.grid.frame[3]);
    rootTransform.rotation = glm::mat3(config.grid.frame);
    const auto root = staged.addGroup(rootTransform);
    if (!root || !staged.setParent(*root, parent)) return {};
    rig.root = *root;
    const auto pelvis = PatternSystem::addJoint(staged, rig.root, config.grid.origin + extent * glm::vec3(.5f, .4f, .5f));
    if (!pelvis) return {};
    rig.pelvis = *pelvis;
    const auto torso = PatternSystem::addJoint(staged, rig.pelvis, {0, extent.y * .18f, 0});
    if (!torso) return {};
    rig.torso = *torso;
    const auto torsoMesh = PatternSystem::addBox(staged, extent * glm::vec3(.45f, .35f, .7f), {0, 0, 0}, config.torsoColor, rig.torso);
    const auto neck = PatternSystem::addJoint(staged, rig.torso, {0, extent.y * .22f, 0});
    if (!torsoMesh || !neck) return {};
    rig.torsoMesh = *torsoMesh; rig.neck = *neck;
    const auto head = PatternSystem::addBox(staged, extent * glm::vec3(.3f, .2f, .6f), {0, extent.y * .1f, 0}, config.headColor, rig.neck);
    const auto nose = PatternSystem::addBox(staged, extent * glm::vec3(.1f, .05f, .2f), {0, extent.y * .1f, extent.z * .4f}, {1, .2f, .2f}, rig.neck);
    if (!head || !nose) return {};
    rig.head = *head; rig.nose = *nose;
    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto color = i == 0 ? config.leftColor : config.rightColor;
        const float upperArm = extent.y * .32f * config.upperArmFraction;
        const float forearm = extent.y * .32f - upperArm;
        const float thigh = extent.y * .4f * config.upperLegFraction;
        const float shin = extent.y * .4f - thigh;
        const auto shoulder = PatternSystem::addJoint(staged, rig.torso, {side * extent.x * .3f, extent.y * .14f, 0});
        const auto hip = PatternSystem::addJoint(staged, rig.pelvis, {side * extent.x * .15f, 0, 0});
        if (!shoulder || !hip) return {};
        rig.shoulders[i] = *shoulder; rig.hips[i] = *hip;
        const auto elbow = PatternSystem::addJoint(staged, *shoulder, {0, -upperArm, 0});
        const auto knee = PatternSystem::addJoint(staged, *hip, {0, -thigh, 0});
        if (!elbow || !knee) return {};
        rig.elbows[i] = *elbow; rig.knees[i] = *knee;
        const auto hand = PatternSystem::addJoint(staged, *elbow, {0, -forearm, 0});
        const auto foot = PatternSystem::addJoint(staged, *knee, {0, -shin, 0});
        const auto upper = PatternSystem::addBox(staged, {extent.x * .15f, upperArm, extent.z * .3f}, {0, -upperArm / 2, 0}, color, *shoulder);
        const auto lower = PatternSystem::addBox(staged, {extent.x * .15f, forearm, extent.z * .3f}, {0, -forearm / 2, 0}, color, *elbow);
        const auto thighMesh = PatternSystem::addBox(staged, {extent.x * .18f, thigh, extent.z * .4f}, {0, -thigh / 2, 0}, color, *hip);
        const auto shinMesh = PatternSystem::addBox(staged, {extent.x * .18f, shin, extent.z * .4f}, {0, -shin / 2, 0}, color, *knee);
        if (!hand || !foot || !upper || !lower || !thighMesh || !shinMesh) return {};
        rig.hands[i] = *hand; rig.feet[i] = *foot;
        rig.upperArms[i] = *upper; rig.forearms[i] = *lower;
        rig.thighs[i] = *thighMesh; rig.shins[i] = *shinMesh;
    }
    scene = std::move(staged);
    return rig;
}
inline bool AnimationSystem::poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose) {
    const std::array<EntityId, 11> joints{rig.pelvis, rig.torso, rig.neck,
        rig.shoulders[0], rig.shoulders[1], rig.elbows[0], rig.elbows[1],
        rig.hips[0], rig.hips[1], rig.knees[0], rig.knees[1]};
    const std::array<glm::vec3, 11> angles{pose.pelvisDegrees, pose.torsoDegrees, pose.neckDegrees,
        pose.shoulders[0], pose.shoulders[1], pose.elbows[0], pose.elbows[1],
        pose.hips[0], pose.hips[1], pose.knees[0], pose.knees[1]};
    std::set<EntityId> seen;
    std::array<Transform3D, 11> transforms;
    for (std::size_t i = 0; i < joints.size(); ++i) {
        const auto* component = scene.getComponent(joints[i]);
        if (!component || !seen.insert(joints[i]).second || !detail::finiteVector(angles[i]))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHumanoid", "Invalid joint or Euler angles.");
        glm::mat4 rotation(1);
        for (int axis = 0; axis < 3; ++axis) {
            glm::vec3 direction(0); direction[axis] = 1;
            const auto turn = TransformSystem::rotation({{0, 0, 0}, direction}, angles[i][axis]);
            if (!turn) return false;
            rotation = *turn * rotation;
        }
        transforms[i] = component->transform;
        transforms[i].rotation = glm::mat3(rotation);
        if (!DiagramSystem::validTransform(transforms[i])) return false;
    }
    for (std::size_t i = 0; i < joints.size(); ++i) if (!scene.setTransform(joints[i], transforms[i])) return false;
    return true;
}
inline std::optional<SwingRig> AnimationSystem::addSwingWithRider(Scene& scene, const SwingConfig& config,
                                                                 std::optional<EntityId> parent) {
    Motion swingMotion;
    auto pendulum = config.pendulum;
    pendulum.axis.point = {0, 0, 0}; pendulum.space = TransformSpace::Local;
    if (!detail::finiteVector(config.pivot) || !detail::positiveLengths(config.seatSize)
        || !std::isfinite(config.length) || config.length <= 0
        || !std::isfinite(config.ropeThickness) || config.ropeThickness <= 0
        || !std::isfinite(config.riderSwayDegrees) || config.riderSwayDegrees < 0
        || (parent && !scene.getComponent(*parent)) || !swingMotion.append(Motion::pendulum(pendulum))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addSwingWithRider", "Invalid swing configuration or parent.");
        return {};
    }
    Scene staged = scene;
    SwingRig rig;
    Transform3D transform; transform.basis = config.pivot;
    const auto frame = staged.addGroup(transform);
    if (!frame || !staged.setParent(*frame, parent)) return {};
    rig.frame = *frame;
    const auto hinge = PatternSystem::addJoint(staged, rig.frame, {0, 0, 0});
    if (!hinge) return {};
    rig.hinge = *hinge;
    const auto seat = PatternSystem::addBox(staged, config.seatSize, {0, -config.length, 0}, {.55f, .3f, .1f}, rig.hinge);
    if (!seat) return {};
    rig.seat = *seat;
    if (!PatternSystem::addBox(staged, {config.seatSize.x + .6f, .12f, .12f}, {0, 0, 0}, {.3f, .35f, .4f}, rig.frame)) return {};
    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto rope = PatternSystem::addBox(staged, {config.ropeThickness, config.length, config.ropeThickness},
            {side * config.seatSize.x * .43f, -config.length / 2, 0}, {.75f, .75f, .75f}, rig.hinge);
        if (!rope || !PatternSystem::addBox(staged, {.12f, config.length + 1, .12f},
            {side * (config.seatSize.x / 2 + .2f), -(config.length + 1) / 2, 0}, {.3f, .35f, .4f}, rig.frame)) return {};
        rig.ropes[i] = *rope;
    }
    const auto person = addHumanoid(staged, config.person, rig.seat);
    if (!person || !poseHumanoid(staged, *person, HumanoidPose::seated())) return {};
    rig.person = *person;
    const auto* root = staged.getComponent(rig.person.root);
    auto personTransform = root->transform;
    const auto hipCenter = config.person.grid.origin + config.person.grid.extent * glm::vec3(.5f, .4f, .5f);
    personTransform.basis = glm::vec3(0, config.seatSize.y / 2, 0) - personTransform.rotation * hipCenter;
    if (!staged.setTransform(rig.person.root, personTransform)) return {};
    const auto swingTrack = createJointAnimation(staged, rig.hinge, swingMotion);
    PendulumMotion rider;
    rider.periodSeconds = pendulum.periodSeconds; rider.amplitudeDegrees = config.riderSwayDegrees;
    rider.phaseDegrees = std::remainder(pendulum.phaseDegrees, 360.0) + 180;
    Motion riderMotion;
    if (!riderMotion.append(Motion::pendulum(rider))) return {};
    const auto riderTrack = createJointAnimation(staged, rig.person.torso, riderMotion);
    if (!swingTrack || !riderTrack) return {};
    rig.animation = {*swingTrack, *riderTrack};
    if (!poseHierarchy(staged, rig.animation, 0)) return {};
    scene = std::move(staged);
    return rig;
}
} // namespace graphics
```

## standard/framework/include/Motion.h
### before (L27)
```
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
```
### after (L27)
```
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};
struct LinearMotion {
    glm::vec3 from{0}, to{1, 0, 0};
    double durationSeconds = 1;
    Playback playback = Playback::Once;
    TransformSpace space = TransformSpace::Local;
};
struct PendulumMotion {
    AxisLine axis{{0, 0, 0}, {1, 0, 0}};
    double amplitudeDegrees = 25;
    double periodSeconds = 3;
    double phaseDegrees = 0;
    double offsetDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
```
### before (L62)
```
    static MotionStep rotation(const RotationMotion& config);
    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);
};

struct MotionState {
```
### after (L76)
```
    static MotionStep rotation(const RotationMotion& config);
    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);
    /// [API] 시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.
    static MotionStep linear(const LinearMotion& config);
    /// [API] 지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.
    static MotionStep pendulum(const PendulumMotion& config);
};

struct MotionState {
```

## standard/framework/detail/Motion.inl
### before (L103)
```
    };
    return step;
}
inline std::optional<MotionId> MotionSystem::createMotion(const Motion& motion) {
    const auto sample = motion.sample(0);
    if (!sample || nextMotion_ == std::numeric_limits<MotionId>::max()) return (Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Motion, "MotionSystem::createMotion", "Rejected condition: !sample || nextMotion_ == std::numeric_limits<MotionId>::max()", "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."), std::nullopt);
```
### after (L103)
```
    };
    return step;
}
inline MotionStep Motion::linear(const LinearMotion& config) {
    if (!detail::finiteVector(config.from) || !detail::finiteVector(config.to)
        || !std::isfinite(config.durationSeconds) || config.durationSeconds <= 0
        || (config.playback != Playback::Once && config.playback != Playback::Loop
            && config.playback != Playback::PingPong)
        || (config.space != TransformSpace::World && config.space != TransformSpace::Local)) return {};
    PathMotion path;
    path.path = [from = glm::dvec3(config.from), to = glm::dvec3(config.to)](double u) {
        return glm::vec3((1 - u) * from + u * to);
    };
    path.durationSeconds = config.durationSeconds;
    path.playback = config.playback;
    path.space = config.space;
    return followPath(path);
}
inline MotionStep Motion::pendulum(const PendulumMotion& config) {
    MotionStep step;
    if (!std::isfinite(config.amplitudeDegrees) || config.amplitudeDegrees < 0
        || !std::isfinite(config.periodSeconds) || config.periodSeconds <= 0
        || !std::isfinite(config.phaseDegrees) || !std::isfinite(config.offsetDegrees)
        || !std::isfinite(std::abs(config.offsetDegrees) + config.amplitudeDegrees)
        || (config.space != TransformSpace::World && config.space != TransformSpace::Local)
        || !TransformSystem::rotation(config.axis, config.offsetDegrees)) return step;
    step.space = config.space;
    step.transform = [config](double seconds) {
        constexpr double pi = 3.14159265358979323846;
        const double phase = std::remainder(seconds, config.periodSeconds) / config.periodSeconds * 2 * pi
                           + std::remainder(config.phaseDegrees, 360.0) * pi / 180;
        const auto matrix = TransformSystem::rotation(config.axis,
            config.offsetDegrees + config.amplitudeDegrees * std::sin(phase));
        if (!matrix) throw std::domain_error("Pendulum motion produced an invalid matrix.");
        return *matrix;
    };
    return step;
}
inline std::optional<MotionId> MotionSystem::createMotion(const Motion& motion) {
    const auto sample = motion.sample(0);
    if (!sample || nextMotion_ == std::numeric_limits<MotionId>::max()) return (Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Motion, "MotionSystem::createMotion", "Rejected condition: !sample || nextMotion_ == std::numeric_limits<MotionId>::max()", "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."), std::nullopt);
```

## standard/framework/Framework.h
### before (L2)
```
// Copy this complete folder, then include this entry point. C++17, Windows x64.
#include "include/Application.h"
#include "include/Patterns.h"
```
### after (L2)
```
// Copy this complete folder, then include this entry point. C++17, Windows x64.
#include "include/Application.h"
#include "include/Patterns.h"
#include "include/Animation.h"
```

## standard/framework/include/Animation.h
### before
없음
### after
```
#pragma once
#include "Patterns.h"

namespace graphics {
struct JointAnimation {
    EntityId joint = 0;
    Transform3D rest;
    Motion motion;
};
struct HumanoidConfig {
    GridLayout grid{glm::vec3(-.5f, 0, -.3f), glm::vec3(1, 1.8f, .6f)};
    float upperArmFraction = .5f, upperLegFraction = .5f;
    GlColor torsoColor{.65f, .65f, .7f}, headColor{1, .8f, .5f};
    GlColor leftColor{.2f, .6f, 1}, rightColor{1, .4f, .2f};
};
struct HumanoidRig {
    EntityId root = 0, pelvis = 0, torso = 0, neck = 0;
    EntityId torsoMesh = 0, head = 0, nose = 0;
    std::array<EntityId, 2> shoulders{}, elbows{}, hands{}, hips{}, knees{}, feet{};
    std::array<EntityId, 2> upperArms{}, forearms{}, thighs{}, shins{};
};
struct HumanoidPose {
    glm::vec3 pelvisDegrees{0}, torsoDegrees{0}, neckDegrees{0};
    std::array<glm::vec3, 2> shoulders{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> elbows{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> hips{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> knees{glm::vec3(0), glm::vec3(0)};
    /// [API] 허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.
    static HumanoidPose seated();
};
struct SwingConfig {
    glm::vec3 pivot{0, 3, 0}, seatSize{1.4f, .12f, .7f};
    float length = 2, ropeThickness = .035f;
    PendulumMotion pendulum;
    HumanoidConfig person;
    double riderSwayDegrees = 8;
};
struct SwingRig {
    EntityId frame = 0, hinge = 0, seat = 0;
    std::array<EntityId, 2> ropes{};
    HumanoidRig person;
    std::vector<JointAnimation> animation;
};
class AnimationSystem {
public:
    /// [API] 관절의 현재 강체 자세와 모션을 보관한다. 자세를 편집한 뒤 다시 만들면 새 기준 자세를 사용한다.
    static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);
    /// [API] 초 단위 절대 시간으로 관절들을 샘플링한 뒤 모두 유효할 때 반영한다. Scene 부모 계층을 유지한다.
    static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);
    /// [API] 실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.
    static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{},
                                                 std::optional<EntityId> parent = std::nullopt);
    /// [API] 관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.
    static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);
    /// [API] 고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.
    static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{},
                                                   std::optional<EntityId> parent = std::nullopt);
};
} // namespace graphics
#include "../detail/Animation.inl"
```

## standard/framework/detail/Animation.inl
### before
없음
### after
```
#pragma once
#include <set>

namespace graphics {
inline HumanoidPose HumanoidPose::seated() {
    HumanoidPose pose;
    for (std::size_t i = 0; i < 2; ++i) {
        pose.hips[i].x = -90;
        pose.knees[i].x = 90;
        pose.shoulders[i].x = -20;
        pose.elbows[i].x = -40;
    }
    return pose;
}
inline std::optional<JointAnimation> AnimationSystem::createJointAnimation(
        const Scene& scene, EntityId joint, const Motion& motion) {
    const auto* component = scene.getComponent(joint);
    const auto sample = motion.sample(0);
    if (!component || !sample || !detail::rigidRoom(DiagramSystem::modelMatrix(component->transform))
        || !detail::rigidRoom(sample->parent) || !detail::rigidRoom(sample->local)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::createJointAnimation",
            "Need an existing rigid joint and a rigid motion sample. Scale the child mesh instead of the joint.");
        return {};
    }
    return JointAnimation{joint, component->transform, motion};
}
inline bool AnimationSystem::poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds) {
    if (!std::isfinite(seconds) || seconds < 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy", "Need nonnegative finite seconds.");
    std::set<EntityId> seen;
    std::vector<std::pair<EntityId, Transform3D>> staged;
    staged.reserve(animation.size());
    for (const auto& track : animation) {
        const auto sample = track.motion.sample(seconds);
        const auto rest = DiagramSystem::modelMatrix(track.rest);
        if (!scene.getComponent(track.joint) || !seen.insert(track.joint).second || !sample
            || !detail::rigidRoom(rest) || !detail::rigidRoom(sample->parent) || !detail::rigidRoom(sample->local))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy",
                "Need unique existing joints, rigid rest poses and rigid motion samples.");
        // World contributions are in the joint parent's space, before its rest pose.
        const auto matrix = sample->parent * rest * sample->local;
        Transform3D transform;
        transform.basis = glm::vec3(matrix[3]);
        transform.rotation = glm::mat3(matrix);
        if (!DiagramSystem::validTransform(transform))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy", "Joint composition overflow.");
        staged.emplace_back(track.joint, transform);
    }
    for (const auto& item : staged) if (!scene.setTransform(item.first, item.second)) return false;
    return true;
}
inline std::optional<HumanoidRig> AnimationSystem::addHumanoid(Scene& scene, const HumanoidConfig& config,
                                                               std::optional<EntityId> parent) {
    if (!config.grid.valid() || !detail::rigidRoom(config.grid.frame)
        || !std::isfinite(config.upperArmFraction) || config.upperArmFraction <= 0 || config.upperArmFraction >= 1
        || !std::isfinite(config.upperLegFraction) || config.upperLegFraction <= 0 || config.upperLegFraction >= 1
        || !detail::validColor(config.torsoColor) || !detail::validColor(config.headColor)
        || !detail::validColor(config.leftColor) || !detail::validColor(config.rightColor)
        || (parent && !scene.getComponent(*parent))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addHumanoid", "Invalid grid, limb fractions, colors or parent.");
        return {};
    }
    Scene staged = scene;
    HumanoidRig rig;
    const auto extent = config.grid.extent;
    Transform3D rootTransform;
    rootTransform.basis = glm::vec3(config.grid.frame[3]);
    rootTransform.rotation = glm::mat3(config.grid.frame);
    const auto root = staged.addGroup(rootTransform);
    if (!root || !staged.setParent(*root, parent)) return {};
    rig.root = *root;
    const auto pelvis = PatternSystem::addJoint(staged, rig.root, config.grid.origin + extent * glm::vec3(.5f, .4f, .5f));
    if (!pelvis) return {};
    rig.pelvis = *pelvis;
    const auto torso = PatternSystem::addJoint(staged, rig.pelvis, {0, extent.y * .18f, 0});
    if (!torso) return {};
    rig.torso = *torso;
    const auto torsoMesh = PatternSystem::addBox(staged, extent * glm::vec3(.45f, .35f, .7f), {0, 0, 0}, config.torsoColor, rig.torso);
    const auto neck = PatternSystem::addJoint(staged, rig.torso, {0, extent.y * .22f, 0});
    if (!torsoMesh || !neck) return {};
    rig.torsoMesh = *torsoMesh; rig.neck = *neck;
    const auto head = PatternSystem::addBox(staged, extent * glm::vec3(.3f, .2f, .6f), {0, extent.y * .1f, 0}, config.headColor, rig.neck);
    const auto nose = PatternSystem::addBox(staged, extent * glm::vec3(.1f, .05f, .2f), {0, extent.y * .1f, extent.z * .4f}, {1, .2f, .2f}, rig.neck);
    if (!head || !nose) return {};
    rig.head = *head; rig.nose = *nose;
    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto color = i == 0 ? config.leftColor : config.rightColor;
        const float upperArm = extent.y * .32f * config.upperArmFraction;
        const float forearm = extent.y * .32f - upperArm;
        const float thigh = extent.y * .4f * config.upperLegFraction;
        const float shin = extent.y * .4f - thigh;
        const auto shoulder = PatternSystem::addJoint(staged, rig.torso, {side * extent.x * .3f, extent.y * .14f, 0});
        const auto hip = PatternSystem::addJoint(staged, rig.pelvis, {side * extent.x * .15f, 0, 0});
        if (!shoulder || !hip) return {};
        rig.shoulders[i] = *shoulder; rig.hips[i] = *hip;
        const auto elbow = PatternSystem::addJoint(staged, *shoulder, {0, -upperArm, 0});
        const auto knee = PatternSystem::addJoint(staged, *hip, {0, -thigh, 0});
        if (!elbow || !knee) return {};
        rig.elbows[i] = *elbow; rig.knees[i] = *knee;
        const auto hand = PatternSystem::addJoint(staged, *elbow, {0, -forearm, 0});
        const auto foot = PatternSystem::addJoint(staged, *knee, {0, -shin, 0});
        const auto upper = PatternSystem::addBox(staged, {extent.x * .15f, upperArm, extent.z * .3f}, {0, -upperArm / 2, 0}, color, *shoulder);
        const auto lower = PatternSystem::addBox(staged, {extent.x * .15f, forearm, extent.z * .3f}, {0, -forearm / 2, 0}, color, *elbow);
        const auto thighMesh = PatternSystem::addBox(staged, {extent.x * .18f, thigh, extent.z * .4f}, {0, -thigh / 2, 0}, color, *hip);
        const auto shinMesh = PatternSystem::addBox(staged, {extent.x * .18f, shin, extent.z * .4f}, {0, -shin / 2, 0}, color, *knee);
        if (!hand || !foot || !upper || !lower || !thighMesh || !shinMesh) return {};
        rig.hands[i] = *hand; rig.feet[i] = *foot;
        rig.upperArms[i] = *upper; rig.forearms[i] = *lower;
        rig.thighs[i] = *thighMesh; rig.shins[i] = *shinMesh;
    }
    scene = std::move(staged);
    return rig;
}
inline bool AnimationSystem::poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose) {
    const std::array<EntityId, 11> joints{rig.pelvis, rig.torso, rig.neck,
        rig.shoulders[0], rig.shoulders[1], rig.elbows[0], rig.elbows[1],
        rig.hips[0], rig.hips[1], rig.knees[0], rig.knees[1]};
    const std::array<glm::vec3, 11> angles{pose.pelvisDegrees, pose.torsoDegrees, pose.neckDegrees,
        pose.shoulders[0], pose.shoulders[1], pose.elbows[0], pose.elbows[1],
        pose.hips[0], pose.hips[1], pose.knees[0], pose.knees[1]};
    std::set<EntityId> seen;
    std::array<Transform3D, 11> transforms;
    for (std::size_t i = 0; i < joints.size(); ++i) {
        const auto* component = scene.getComponent(joints[i]);
        if (!component || !seen.insert(joints[i]).second || !detail::finiteVector(angles[i]))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHumanoid", "Invalid joint or Euler angles.");
        glm::mat4 rotation(1);
        for (int axis = 0; axis < 3; ++axis) {
            glm::vec3 direction(0); direction[axis] = 1;
            const auto turn = TransformSystem::rotation({{0, 0, 0}, direction}, angles[i][axis]);
            if (!turn) return false;
            rotation = *turn * rotation;
        }
        transforms[i] = component->transform;
        transforms[i].rotation = glm::mat3(rotation);
        if (!DiagramSystem::validTransform(transforms[i])) return false;
    }
    for (std::size_t i = 0; i < joints.size(); ++i) if (!scene.setTransform(joints[i], transforms[i])) return false;
    return true;
}
inline std::optional<SwingRig> AnimationSystem::addSwingWithRider(Scene& scene, const SwingConfig& config,
                                                                 std::optional<EntityId> parent) {
    Motion swingMotion;
    auto pendulum = config.pendulum;
    pendulum.axis.point = {0, 0, 0}; pendulum.space = TransformSpace::Local;
    if (!detail::finiteVector(config.pivot) || !detail::positiveLengths(config.seatSize)
        || !std::isfinite(config.length) || config.length <= 0
        || !std::isfinite(config.ropeThickness) || config.ropeThickness <= 0
        || !std::isfinite(config.riderSwayDegrees) || config.riderSwayDegrees < 0
        || (parent && !scene.getComponent(*parent)) || !swingMotion.append(Motion::pendulum(pendulum))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addSwingWithRider", "Invalid swing configuration or parent.");
        return {};
    }
    Scene staged = scene;
    SwingRig rig;
    Transform3D transform; transform.basis = config.pivot;
    const auto frame = staged.addGroup(transform);
    if (!frame || !staged.setParent(*frame, parent)) return {};
    rig.frame = *frame;
    const auto hinge = PatternSystem::addJoint(staged, rig.frame, {0, 0, 0});
    if (!hinge) return {};
    rig.hinge = *hinge;
    const auto seat = PatternSystem::addBox(staged, config.seatSize, {0, -config.length, 0}, {.55f, .3f, .1f}, rig.hinge);
    if (!seat) return {};
    rig.seat = *seat;
    if (!PatternSystem::addBox(staged, {config.seatSize.x + .6f, .12f, .12f}, {0, 0, 0}, {.3f, .35f, .4f}, rig.frame)) return {};
    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto rope = PatternSystem::addBox(staged, {config.ropeThickness, config.length, config.ropeThickness},
            {side * config.seatSize.x * .43f, -config.length / 2, 0}, {.75f, .75f, .75f}, rig.hinge);
        if (!rope || !PatternSystem::addBox(staged, {.12f, config.length + 1, .12f},
            {side * (config.seatSize.x / 2 + .2f), -(config.length + 1) / 2, 0}, {.3f, .35f, .4f}, rig.frame)) return {};
        rig.ropes[i] = *rope;
    }
    const auto person = addHumanoid(staged, config.person, rig.seat);
    if (!person || !poseHumanoid(staged, *person, HumanoidPose::seated())) return {};
    rig.person = *person;
    const auto* root = staged.getComponent(rig.person.root);
    auto personTransform = root->transform;
    const auto hipCenter = config.person.grid.origin + config.person.grid.extent * glm::vec3(.5f, .4f, .5f);
    personTransform.basis = glm::vec3(0, config.seatSize.y / 2, 0) - personTransform.rotation * hipCenter;
    if (!staged.setTransform(rig.person.root, personTransform)) return {};
    const auto swingTrack = createJointAnimation(staged, rig.hinge, swingMotion);
    PendulumMotion rider;
    rider.periodSeconds = pendulum.periodSeconds; rider.amplitudeDegrees = config.riderSwayDegrees;
    rider.phaseDegrees = std::remainder(pendulum.phaseDegrees, 360.0) + 180;
    Motion riderMotion;
    if (!riderMotion.append(Motion::pendulum(rider))) return {};
    const auto riderTrack = createJointAnimation(staged, rig.person.torso, riderMotion);
    if (!swingTrack || !riderTrack) return {};
    rig.animation = {*swingTrack, *riderTrack};
    if (!poseHierarchy(staged, rig.animation, 0)) return {};
    scene = std::move(staged);
    return rig;
}
} // namespace graphics
```

## standard/Console.h
### before (L13)
```
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;
    enum class Demo { Motion, Grid, Room };
    Demo demo_=Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_=0, roomCube_=0;
    bool threeViews_=false, trace_=false, floorOpen_=false;
    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();
    /// 회전하는 방 예제를 구성한다.
    bool initRoom();

    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);
```
### after (L13)
```
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;
    enum class Demo { Motion, Grid, Room, Animation };
    Demo demo_=Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_=0, roomCube_=0;
    std::vector<graphics::JointAnimation> animation_;
    bool threeViews_=false, trace_=false, floorOpen_=false;
    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();
    /// 회전하는 방 예제를 구성한다.
    bool initRoom();
    /// 선형보간·그네·사람 관절의 계층 애니메이션 예제를 구성한다.
    bool initAnimation();

    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);
```

## standard/Console.cpp
### before (L186)
```
    if (action != GLFW_PRESS) {
        return;
    }
    if(key==GLFW_KEY_7 || key==GLFW_KEY_8 || key==GLFW_KEY_0) {
        const bool ok=key==GLFW_KEY_7?initGrid():key==GLFW_KEY_8?initRoom():initScene();
        if(!ok) app_.requestClose();return;
    }
    if(key==GLFW_KEY_V) {threeViews_=!threeViews_;return;}
```
### after (L186)
```
    if (action != GLFW_PRESS) {
        return;
    }
    if(key==GLFW_KEY_7 || key==GLFW_KEY_8 || key==GLFW_KEY_9 || key==GLFW_KEY_0) {
        const bool ok=key==GLFW_KEY_7?initGrid():key==GLFW_KEY_8?initRoom():key==GLFW_KEY_9?initAnimation():initScene();
        if(!ok) app_.requestClose();return;
    }
    if(key==GLFW_KEY_V) {threeViews_=!threeViews_;return;}
```
### before (L270)
```
              << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
              << "왼쪽 클릭: 물체의 entity/face ID 출력\n"
              << "7 Grid/거리 기반 배치 / 8 실내 미끄럼·낙하 / 0 기존 Motion 예제\n"
              << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

```
### after (L270)
```
              << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
              << "왼쪽 클릭: 물체의 entity/face ID 출력\n"
              << "7 Grid/거리 기반 배치 / 8 실내 미끄럼·낙하 / 0 기존 Motion 예제\n"
              << "9 선형보간·진자·사람형·그네→사람 계층 애니메이션\n"
              << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

```
### before (L300)
```
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={5,3,8};app_.getCamera().target={0,0,0};
    return true;
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    if(demo_==Demo::Motion || !clock_.playing) return;
    if(!clock_.advance(dt)) {app_.requestClose();return;}
    auto& scene=app_.getScene();bool ok=true;
    if(demo_==Demo::Grid) {
        for(std::size_t i=0;i<gridBoxes_.size();++i)
            ok=graphics::PatternSystem::follow(scene,gridBoxes_[i],distancePath_,clock_.seconds+distancePath_.length()*i/5)&&ok;
    } else {
```
### after (L301)
```
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={5,3,8};app_.getCamera().target={0,0,0};
    return true;
}
/// 선형보간과 그네·사람의 부모·자식 애니메이션을 구성한다.
bool Console::initAnimation() {
    auto& scene = app_.getScene();
    scene.clear(); demo_ = Demo::Animation; clock_.reset(); animation_.clear();
    if (!graphics::PatternSystem::addBox(scene, {8, .15f, 6}, {0, -.15f, 0}, {.25f, .3f, .35f})) return false;
    graphics::SwingConfig swingConfig;
    swingConfig.pivot = {1, 3, 0};
    swingConfig.pendulum.amplitudeDegrees = 25;
    swingConfig.pendulum.periodSeconds = 3;
    const auto swing = graphics::AnimationSystem::addSwingWithRider(scene, swingConfig);
    if (!swing) return false;
    animation_ = swing->animation;

    graphics::HumanoidConfig personConfig;
    personConfig.grid.frame = glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0));
    const auto person = graphics::AnimationSystem::addHumanoid(scene, personConfig);
    if (!person) return false;
    auto pose = graphics::HumanoidPose::seated();
    pose.hips = {glm::vec3(0), glm::vec3(0)};
    pose.knees = {glm::vec3(0), glm::vec3(0)};
    if (!graphics::AnimationSystem::poseHumanoid(scene, *person, pose)) return false;
    for (std::size_t i = 0; i < 2; ++i) {
        graphics::PendulumMotion sway;
        sway.amplitudeDegrees = 30; sway.periodSeconds = 2;
        sway.phaseDegrees = i == 0 ? 0 : 180;
        graphics::Motion motion;
        if (!motion.append(graphics::Motion::pendulum(sway))) return false;
        const auto track = graphics::AnimationSystem::createJointAnimation(scene, person->shoulders[i], motion);
        if (!track) return false;
        animation_.push_back(*track);
    }
    const auto marker = scene.addGroup();
    if (!marker || !graphics::PatternSystem::addBox(scene, {.4f, .4f, .4f}, {0, 0, 0}, {1, .85f, .15f}, *marker)) return false;
    graphics::LinearMotion move;
    move.from = {-2, .3f, 2}; move.to = {2, .3f, 2};
    move.durationSeconds = 4; move.playback = graphics::Playback::PingPong;
    graphics::Motion motion;
    if (!motion.append(graphics::Motion::linear(move))) return false;
    const auto track = graphics::AnimationSystem::createJointAnimation(scene, *marker, motion);
    if (!track) return false;
    animation_.push_back(*track);
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 10}; app_.getCamera().target = {0, 1.4f, 0};
    app_.getRenderOptions().lighting = true;
    std::cout << "9: 노란 상자는 4초 편도 선형보간, 그네는 3초 주기 진자, 좌석의 사람은 계층 변환을 따릅니다.\n";
    return graphics::AnimationSystem::poseHierarchy(scene, animation_, 0);
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    if(demo_==Demo::Motion || !clock_.playing) return;
    if(!clock_.advance(dt)) {app_.requestClose();return;}
    auto& scene=app_.getScene();bool ok=true;
    if (demo_ == Demo::Animation) {
        ok = graphics::AnimationSystem::poseHierarchy(scene, animation_, clock_.seconds);
    } else if(demo_==Demo::Grid) {
        for(std::size_t i=0;i<gridBoxes_.size();++i)
            ok=graphics::PatternSystem::follow(scene,gridBoxes_[i],distancePath_,clock_.seconds+distancePath_.length()*i/5)&&ok;
    } else {
```

## framework/docs/ANIMATION_API.md
### before
없음
### after
```
# 애니메이션 지정 API

`framework/Framework.h`를 포함한다. `standard`에서도 같은 API를 사용한다.
새 사람형·계층 패턴은 `graphics::AnimationSystem`, 기본 시간 모션은 `graphics::Motion`에 있다.
기존 `PatternSystem::addWalkingRig`, `poseWalkingRig`, `JointRig`는 유지한다.

## 선형보간

```cpp
graphics::LinearMotion move;
move.from = {0, 0, 0};
move.to = {2, 0, 0};
move.durationSeconds = 4;
move.playback = graphics::Playback::PingPong;
graphics::Motion motion;
if (!motion.append(graphics::Motion::linear(move))) return false;
```

from/to는 기준 자세에 더할 이동 변환이다. 기본 공간은 Local이다. Once는 끝 위치를 유지하고,
Loop는 처음으로 돌아가며, PingPong은 양 끝 사이를 왕복한다. durationSeconds는 편도 시간이다.
기존 MotionSystem에 등록·레이어 연결해서 쓸 수도 있고, 아래 관절 트랙에 넣을 수도 있다.
`PathSystem::linear`는 진행률 함수이며 두 위치 사이 이동을 만드는 이 API와 역할이 다르다.
유한한 위치, 양수 시간, 유효한 재생 정책과 공간이 필요하다. 잘못된 설정은 빈 MotionStep으로
반환하며 append가 거부한다.

## 진자 운동

```cpp
graphics::PendulumMotion sway;
sway.axis = {{0, 0, 0}, {1, 0, 0}};
sway.amplitudeDegrees = 25;
sway.periodSeconds = 3;
sway.phaseDegrees = 0;
sway.offsetDegrees = 0;
graphics::Motion motion;
if (!motion.append(graphics::Motion::pendulum(sway))) return false;
```

각도 = offsetDegrees + amplitudeDegrees * sin(2π*t/periodSeconds + phaseDegrees*π/180).
기본 공간은 Local, 기본 회전축은 X이다. 진폭은 0 이상, 주기는 양수, 축은 0이 아닌 유한 벡터다.
주기와 진폭을 직접 정하는 운동학적 애니메이션이다. 중력·줄 길이에서 주기를 구하는 물리 진자는 아니다.

## 사람형과 자세

```cpp
graphics::HumanoidConfig config;
config.grid.extent = {1, 1.8f, .6f};
config.upperArmFraction = .5f;
config.upperLegFraction = .5f;
const auto person = graphics::AnimationSystem::addHumanoid(scene, config);
if (!person) return false;
auto pose = graphics::HumanoidPose::seated();
pose.elbows[0] = {-60, 0, 0};
if (!graphics::AnimationSystem::poseHumanoid(scene, *person, pose)) return false;
```

실습 23의 몸통·머리·팔·다리 비율을 유지하고 팔꿈치·무릎을 추가했다.
루트→골반→몸통→목→머리, 몸통→어깨→팔꿈치→손, 골반→엉덩이→무릎→발로 연결한다.
배열 인덱스 0은 모델의 -X 쪽, 1은 +X 쪽이다. 사람은 +Z 방향을 본다.
HumanoidPose는 골반·몸통·목·양쪽 어깨·팔꿈치·엉덩이·무릎의 도 단위 XYZ 로컬 회전이다.
X→Y→Z 순서로 적용하므로 열벡터 기준 최종 회전은 Rz*Ry*Rx이다.
기본 자세는 모두 0이다. seated()는 엉덩이 -90°, 무릎 +90°, 어깨 -20°, 팔꿈치 -40°다.

HumanoidRig에는 관절 ID와 torsoMesh/head/nose/upperArms/forearms/thighs/shins 메시 ID가 있다.
Scene::setPosition, setSideLengths, setTint, PatternSystem::rotateJoint로 각 부위를 편집할 수 있다.
관절을 돌리면 자식 부위가 따라간다. torso는 관절이고 torsoMesh는 표시 메시다.
크기는 grid.extent, 팔다리 위쪽 비율은 upperArmFraction/upperLegFraction으로 정한다.
부모를 지정하면 grid.frame은 그 부모의 로컬 공간에서 해석한다.

## 관절 계층 애니메이션

```cpp
const auto track = graphics::AnimationSystem::createJointAnimation(scene, person->elbows[0], motion);
if (!track) return false;
std::vector<graphics::JointAnimation> animation{*track};
// 매 프레임 누적 절대 시간(초)을 전달한다.
if (!graphics::AnimationSystem::poseHierarchy(scene, animation, elapsedSeconds)) return false;
```

트랙은 joint ID, 생성 시점의 rest 자세, Motion을 값으로 보관한다.
매번 rest에서 계산하므로 앞 프레임의 회전을 반복 누적하지 않는다.
관절 로컬 행렬 = sample.parent * rest * sample.local.
여기서 Motion의 World 기여는 **관절 부모 좌표**에서 rest 앞에 적용한다.
Local 기여는 rest 뒤에 적용한다. Scene의 부모 행렬은 나중에 바깥에서 합성한다.
모션 샘플·기준 자세는 이동+회전의 강체 변환만 허용한다. 크기는 자식 메시에서 바꾼다.
같은 관절 ID를 한 트랙 목록에 두 번 넣지 않는다. 여러 움직임은 하나의 Motion에 append한다.
트랙 생성 후 기준 자세를 편집했다면 createJointAnimation을 다시 호출해 새 rest를 저장한다.
Application은 MotionSystem만 자동 갱신한다. 이 목록은 update 콜백에서 poseHierarchy로 갱신한다.
같은 관절을 다른 update 코드나 모션 레이어가 동시에 조작하지 않도록 책임을 정한다.

## 그네(진자) → 사람 응용 패턴

```cpp
graphics::SwingConfig config;
config.pivot = {0, 3, 0};
config.length = 2;
config.pendulum.amplitudeDegrees = 25;
config.pendulum.periodSeconds = 3;
config.riderSwayDegrees = 8;
const auto swing = graphics::AnimationSystem::addSwingWithRider(scene, config);
if (!swing) return false;
// 매 프레임: 그네와 사람 상체를 함께 갱신한다.
if (!graphics::AnimationSystem::poseHierarchy(scene, swing->animation, elapsedSeconds)) return false;
```

고정 frame→진자 hinge→줄/좌석→사람 root→골반→몸통/다리 구조다.
고정 지지대는 frame의 자식이므로 hinge가 회전해도 함께 흔들리지 않는다.
좌석과 줄은 hinge의 자식이며, 사람은 좌석의 자식이다.
사람의 골반을 좌석 윗면에 맞추고 seated() 자세를 적용한다.
config.person.grid.frame의 회전은 유지하지만 위치는 착석 위치에 맞춰 다시 정한다.
config.pendulum.axis.point는 hinge 원점, space는 Local로 고정한다. pivot은 부모 공간 위치다.
축 방향·진폭·주기·위상·중심 각도는 config.pendulum으로 정한다.
상체는 같은 주기·180° 반대 위상으로 riderSwayDegrees만큼 별도로 흔들린다. 0이면 상체 추가 운동이 없다.
SwingRig::person의 모든 관절과 SwingRig::animation은 편집할 수 있다.
팔 트랙을 추가하려면 앉은 자세를 적용한 뒤 해당 어깨/팔꿈치에서 기준 자세를 캡처한다.

Scene::clear 이후에는 저장한 rig와 트랙을 다시 만든다. Scene ID는 재사용될 수 있다.
addHumanoid/addSwingWithRider는 임시 Scene에서 생성하므로 실패 시 기존 Scene을 보존한다.
성공 시 Scene 대입으로 기존 컴포넌트의 비소유 포인터가 무효화될 수 있다. 필요하면 ID로 다시 조회한다.

## 사용자가 수행할 확인

빌드·테스트·앱 실행은 수행하지 않았다. 아래는 예상 결과이며 실행 관찰이 아니다.

1. VS에서 standard를 x64로 빌드하고 실행한다. 9를 누르면 그네와 두 사람, 노란 상자가 표시되어야 한다.
2. 노란 상자는 t=0에서 x=-2, t=2에서 x=0, t=4에서 x=2, t=8에서 x=-2이어야 한다.
3. 진자 위상 0에서는 t=0 각도 0°, t=.75에서 +25°, t=1.5에서 0°, t=2.25에서 -25°이어야 한다.
4. 그네 좌석의 사람 골반이 좌석 윗면을 따라가고, 지지대는 고정되어야 한다.
5. 서 있는 사람은 양팔을 반대 위상으로 움직여야 한다. 팔꿈치는 어깨를 따라가야 한다.
6. Space로 전체 예제 시간을 멈추고 재개한다. V의 세 뷰에서도 같은 자세가 보여야 한다.
7. 0/7/8/9로 전환한 후 다시 9를 누른다. 오래된 ID 없이 초기 자세로 시작해야 한다.
8. durationSeconds=0, periodSeconds=0, 0축, 중복 관절 ID를 각각 지정하면 거부되어야 한다.
   poseHierarchy의 잘못된 마지막 트랙 때문에 앞 관절만 먼저 바뀌어서는 안 된다.
9. 기본 구성의 기존 Motion/격자/방 예제와 실습 23의 기존 보행 기능은 별도로 확인한다.
```

## standard/docs/ANIMATION_API.md
### before
없음
### after
```
# 애니메이션 지정 API

`framework/Framework.h`를 포함한다. `standard`에서도 같은 API를 사용한다.
새 사람형·계층 패턴은 `graphics::AnimationSystem`, 기본 시간 모션은 `graphics::Motion`에 있다.
기존 `PatternSystem::addWalkingRig`, `poseWalkingRig`, `JointRig`는 유지한다.

## 선형보간

```cpp
graphics::LinearMotion move;
move.from = {0, 0, 0};
move.to = {2, 0, 0};
move.durationSeconds = 4;
move.playback = graphics::Playback::PingPong;
graphics::Motion motion;
if (!motion.append(graphics::Motion::linear(move))) return false;
```

from/to는 기준 자세에 더할 이동 변환이다. 기본 공간은 Local이다. Once는 끝 위치를 유지하고,
Loop는 처음으로 돌아가며, PingPong은 양 끝 사이를 왕복한다. durationSeconds는 편도 시간이다.
기존 MotionSystem에 등록·레이어 연결해서 쓸 수도 있고, 아래 관절 트랙에 넣을 수도 있다.
`PathSystem::linear`는 진행률 함수이며 두 위치 사이 이동을 만드는 이 API와 역할이 다르다.
유한한 위치, 양수 시간, 유효한 재생 정책과 공간이 필요하다. 잘못된 설정은 빈 MotionStep으로
반환하며 append가 거부한다.

## 진자 운동

```cpp
graphics::PendulumMotion sway;
sway.axis = {{0, 0, 0}, {1, 0, 0}};
sway.amplitudeDegrees = 25;
sway.periodSeconds = 3;
sway.phaseDegrees = 0;
sway.offsetDegrees = 0;
graphics::Motion motion;
if (!motion.append(graphics::Motion::pendulum(sway))) return false;
```

각도 = offsetDegrees + amplitudeDegrees * sin(2π*t/periodSeconds + phaseDegrees*π/180).
기본 공간은 Local, 기본 회전축은 X이다. 진폭은 0 이상, 주기는 양수, 축은 0이 아닌 유한 벡터다.
주기와 진폭을 직접 정하는 운동학적 애니메이션이다. 중력·줄 길이에서 주기를 구하는 물리 진자는 아니다.

## 사람형과 자세

```cpp
graphics::HumanoidConfig config;
config.grid.extent = {1, 1.8f, .6f};
config.upperArmFraction = .5f;
config.upperLegFraction = .5f;
const auto person = graphics::AnimationSystem::addHumanoid(scene, config);
if (!person) return false;
auto pose = graphics::HumanoidPose::seated();
pose.elbows[0] = {-60, 0, 0};
if (!graphics::AnimationSystem::poseHumanoid(scene, *person, pose)) return false;
```

실습 23의 몸통·머리·팔·다리 비율을 유지하고 팔꿈치·무릎을 추가했다.
루트→골반→몸통→목→머리, 몸통→어깨→팔꿈치→손, 골반→엉덩이→무릎→발로 연결한다.
배열 인덱스 0은 모델의 -X 쪽, 1은 +X 쪽이다. 사람은 +Z 방향을 본다.
HumanoidPose는 골반·몸통·목·양쪽 어깨·팔꿈치·엉덩이·무릎의 도 단위 XYZ 로컬 회전이다.
X→Y→Z 순서로 적용하므로 열벡터 기준 최종 회전은 Rz*Ry*Rx이다.
기본 자세는 모두 0이다. seated()는 엉덩이 -90°, 무릎 +90°, 어깨 -20°, 팔꿈치 -40°다.

HumanoidRig에는 관절 ID와 torsoMesh/head/nose/upperArms/forearms/thighs/shins 메시 ID가 있다.
Scene::setPosition, setSideLengths, setTint, PatternSystem::rotateJoint로 각 부위를 편집할 수 있다.
관절을 돌리면 자식 부위가 따라간다. torso는 관절이고 torsoMesh는 표시 메시다.
크기는 grid.extent, 팔다리 위쪽 비율은 upperArmFraction/upperLegFraction으로 정한다.
부모를 지정하면 grid.frame은 그 부모의 로컬 공간에서 해석한다.

## 관절 계층 애니메이션

```cpp
const auto track = graphics::AnimationSystem::createJointAnimation(scene, person->elbows[0], motion);
if (!track) return false;
std::vector<graphics::JointAnimation> animation{*track};
// 매 프레임 누적 절대 시간(초)을 전달한다.
if (!graphics::AnimationSystem::poseHierarchy(scene, animation, elapsedSeconds)) return false;
```

트랙은 joint ID, 생성 시점의 rest 자세, Motion을 값으로 보관한다.
매번 rest에서 계산하므로 앞 프레임의 회전을 반복 누적하지 않는다.
관절 로컬 행렬 = sample.parent * rest * sample.local.
여기서 Motion의 World 기여는 **관절 부모 좌표**에서 rest 앞에 적용한다.
Local 기여는 rest 뒤에 적용한다. Scene의 부모 행렬은 나중에 바깥에서 합성한다.
모션 샘플·기준 자세는 이동+회전의 강체 변환만 허용한다. 크기는 자식 메시에서 바꾼다.
같은 관절 ID를 한 트랙 목록에 두 번 넣지 않는다. 여러 움직임은 하나의 Motion에 append한다.
트랙 생성 후 기준 자세를 편집했다면 createJointAnimation을 다시 호출해 새 rest를 저장한다.
Application은 MotionSystem만 자동 갱신한다. 이 목록은 update 콜백에서 poseHierarchy로 갱신한다.
같은 관절을 다른 update 코드나 모션 레이어가 동시에 조작하지 않도록 책임을 정한다.

## 그네(진자) → 사람 응용 패턴

```cpp
graphics::SwingConfig config;
config.pivot = {0, 3, 0};
config.length = 2;
config.pendulum.amplitudeDegrees = 25;
config.pendulum.periodSeconds = 3;
config.riderSwayDegrees = 8;
const auto swing = graphics::AnimationSystem::addSwingWithRider(scene, config);
if (!swing) return false;
// 매 프레임: 그네와 사람 상체를 함께 갱신한다.
if (!graphics::AnimationSystem::poseHierarchy(scene, swing->animation, elapsedSeconds)) return false;
```

고정 frame→진자 hinge→줄/좌석→사람 root→골반→몸통/다리 구조다.
고정 지지대는 frame의 자식이므로 hinge가 회전해도 함께 흔들리지 않는다.
좌석과 줄은 hinge의 자식이며, 사람은 좌석의 자식이다.
사람의 골반을 좌석 윗면에 맞추고 seated() 자세를 적용한다.
config.person.grid.frame의 회전은 유지하지만 위치는 착석 위치에 맞춰 다시 정한다.
config.pendulum.axis.point는 hinge 원점, space는 Local로 고정한다. pivot은 부모 공간 위치다.
축 방향·진폭·주기·위상·중심 각도는 config.pendulum으로 정한다.
상체는 같은 주기·180° 반대 위상으로 riderSwayDegrees만큼 별도로 흔들린다. 0이면 상체 추가 운동이 없다.
SwingRig::person의 모든 관절과 SwingRig::animation은 편집할 수 있다.
팔 트랙을 추가하려면 앉은 자세를 적용한 뒤 해당 어깨/팔꿈치에서 기준 자세를 캡처한다.

Scene::clear 이후에는 저장한 rig와 트랙을 다시 만든다. Scene ID는 재사용될 수 있다.
addHumanoid/addSwingWithRider는 임시 Scene에서 생성하므로 실패 시 기존 Scene을 보존한다.
성공 시 Scene 대입으로 기존 컴포넌트의 비소유 포인터가 무효화될 수 있다. 필요하면 ID로 다시 조회한다.

## 사용자가 수행할 확인

빌드·테스트·앱 실행은 수행하지 않았다. 아래는 예상 결과이며 실행 관찰이 아니다.

1. VS에서 standard를 x64로 빌드하고 실행한다. 9를 누르면 그네와 두 사람, 노란 상자가 표시되어야 한다.
2. 노란 상자는 t=0에서 x=-2, t=2에서 x=0, t=4에서 x=2, t=8에서 x=-2이어야 한다.
3. 진자 위상 0에서는 t=0 각도 0°, t=.75에서 +25°, t=1.5에서 0°, t=2.25에서 -25°이어야 한다.
4. 그네 좌석의 사람 골반이 좌석 윗면을 따라가고, 지지대는 고정되어야 한다.
5. 서 있는 사람은 양팔을 반대 위상으로 움직여야 한다. 팔꿈치는 어깨를 따라가야 한다.
6. Space로 전체 예제 시간을 멈추고 재개한다. V의 세 뷰에서도 같은 자세가 보여야 한다.
7. 0/7/8/9로 전환한 후 다시 9를 누른다. 오래된 ID 없이 초기 자세로 시작해야 한다.
8. durationSeconds=0, periodSeconds=0, 0축, 중복 관절 ID를 각각 지정하면 거부되어야 한다.
   poseHierarchy의 잘못된 마지막 트랙 때문에 앞 관절만 먼저 바뀌어서는 안 된다.
9. 기본 구성의 기존 Motion/격자/방 예제와 실습 23의 기존 보행 기능은 별도로 확인한다.
```

## standard/USER_CHECKS.md
### before (L37)
```
## 4. 변경 경계

이 샘플을 만든 작업은 실습 1~19와 루트 프레임워크의 기존 파일을 변경하지 않았다. 향후 공용 프레임워크 수정은 루트에 먼저 반영하고, standard에 필요한 버전을 명시적으로 배포한다. tools/Sync-Framework.ps1은 실습 사본을 덮어쓰므로 이 샘플 생성에는 사용하지 않았다.
```
### after (L37)
```
## 4. 변경 경계

이 샘플을 만든 작업은 실습 1~19와 루트 프레임워크의 기존 파일을 변경하지 않았다. 향후 공용 프레임워크 수정은 루트에 먼저 반영하고, standard에 필요한 버전을 명시적으로 배포한다. tools/Sync-Framework.ps1은 실습 사본을 덮어쓰므로 이 샘플 생성에는 사용하지 않았다.

## 9번 애니메이션 예제

선형보간·진자·사람형·그네→사람 계층의 확인 절차는 [ANIMATION_API.md](docs/ANIMATION_API.md)의 사용자 확인 절을 따른다. 새 API·예제는 빌드·실행하지 않았다.
```

## standard/standard.vcxproj
### before (L48)
```
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Transform.h" />
    <ClInclude Include="framework\include\Patterns.h" />
    <ClInclude Include="framework\include\GridLayout.h" />
```
### after (L48)
```
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Animation.h" />
    <ClInclude Include="framework\detail\Animation.inl" />
    <ClInclude Include="framework\include\Transform.h" />
    <ClInclude Include="framework\include\Patterns.h" />
    <ClInclude Include="framework\include\GridLayout.h" />
```
