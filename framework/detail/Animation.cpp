#include "../Framework.h"
#include "Internal.h"
#include <set>

namespace graphics {
namespace detail {
bool validJointRest(const Transform3D& rest) {
    return DiagramSystem::validTransform(rest) && rest.scale.x > 1e-6f &&
           std::abs(rest.scale.x - rest.scale.y) < 1e-6f && std::abs(rest.scale.x - rest.scale.z) < 1e-6f;
}
}

HumanoidPose HumanoidPose::seated() {
    HumanoidPose pose;

    for (std::size_t i = 0; i < 2; ++i) {
        pose.hips[i].x = -90;
        pose.knees[i].x = 90;
        pose.shoulders[i].x = -20;
        pose.elbows[i].x = -40;
    }

    return pose;
}

std::optional<JointAnimation> AnimationSystem::createJointAnimation(const Scene& scene, EntityId joint,
                                                                    const Motion& motion) {
    const auto* component = scene.getComponent(joint);
    const auto sample = motion.sample(0);

    if (!component || !sample || !detail::validJointRest(component->transform) || !detail::rigidRoom(sample->parent) ||
        !detail::rigidRoom(sample->local)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::createJointAnimation",
                            "Need a joint with positive uniform rest scale and a rigid motion sample.");

        return {};
    }

    return JointAnimation{joint, component->transform, motion};
}

bool AnimationSystem::poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds) {
    if (!std::isfinite(seconds) || seconds < 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy",
                                   "Need nonnegative finite seconds.");

    std::set<EntityId> seen;
    std::vector<std::pair<EntityId, Transform3D>> staged;

    staged.reserve(animation.size());

    for (const auto& track : animation) {
        const auto sample = track.motion.sample(seconds);
        const auto rest = DiagramSystem::modelMatrix(track.rest);

        if (!scene.getComponent(track.joint) || !seen.insert(track.joint).second || !sample ||
            !detail::validJointRest(track.rest) || !detail::rigidRoom(sample->parent) ||
            !detail::rigidRoom(sample->local))
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy",
                "Need unique existing joints, positive uniform rest scales and rigid motion samples.");
        // World contributions are in the joint parent's space, before its rest pose.
        const auto matrix = sample->parent * rest * sample->local;
        Transform3D transform;

        transform.basis = glm::vec3(matrix[3]);
        transform.scale = track.rest.scale;
        transform.rotation = glm::mat3(matrix) / track.rest.scale.x;

        if (!DiagramSystem::validTransform(transform))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy",
                                       "Joint composition overflow.");
        staged.emplace_back(track.joint, transform);
    }

    for (const auto& item : staged)
        if (!scene.setTransform(item.first, item.second))
            return false;

    return true;
}

std::optional<HumanoidRig> AnimationSystem::addHumanoid(Scene& scene, const HumanoidConfig& config,
                                                        std::optional<EntityId> parent) {
    if (!config.grid.valid() || !detail::rigidRoom(config.grid.frame) || !std::isfinite(config.upperArmFraction) ||
        config.upperArmFraction <= 0 || config.upperArmFraction >= 1 || !std::isfinite(config.upperLegFraction) ||
        config.upperLegFraction <= 0 || config.upperLegFraction >= 1 || !detail::validColor(config.torsoColor) ||
        !detail::validColor(config.headColor) || !detail::validColor(config.leftColor) ||
        !detail::validColor(config.rightColor) || (parent && !scene.getComponent(*parent))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addHumanoid",
                            "Invalid grid, limb fractions, colors or parent.");

        return {};
    }

    Scene staged = scene;
    HumanoidRig rig;
    const auto extent = config.grid.extent;
    Transform3D rootTransform;

    rootTransform.basis = glm::vec3(config.grid.frame[3]);
    rootTransform.rotation = glm::mat3(config.grid.frame);

    const auto root = staged.addGroup(rootTransform);

    if (!root || !staged.setParent(*root, parent))
        return {};
    rig.root = *root;

    const auto pelvis =
        PatternSystem::addJoint(staged, rig.root, config.grid.origin + extent * glm::vec3(.5f, .4f, .5f));

    if (!pelvis)
        return {};
    rig.pelvis = *pelvis;

    const auto torso = PatternSystem::addJoint(staged, rig.pelvis, {0, extent.y * .18f, 0});

    if (!torso)
        return {};
    rig.torso = *torso;

    const auto torsoMesh =
        PatternSystem::addBox(staged, extent * glm::vec3(.45f, .35f, .7f), {0, 0, 0}, config.torsoColor, rig.torso);
    const auto neck = PatternSystem::addJoint(staged, rig.torso, {0, extent.y * .22f, 0});

    if (!torsoMesh || !neck)
        return {};
    rig.torsoMesh = *torsoMesh;
    rig.neck = *neck;

    const auto head = PatternSystem::addBox(staged, extent * glm::vec3(.3f, .2f, .6f), {0, extent.y * .1f, 0},
                                            config.headColor, rig.neck);
    const auto nose = PatternSystem::addBox(staged, extent * glm::vec3(.1f, .05f, .2f),
                                            {0, extent.y * .1f, extent.z * .4f}, {1, .2f, .2f}, rig.neck);

    if (!head || !nose)
        return {};
    rig.head = *head;
    rig.nose = *nose;

    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto color = i == 0 ? config.leftColor : config.rightColor;
        const float upperArm = extent.y * .32f * config.upperArmFraction;
        const float forearm = extent.y * .32f - upperArm;
        const float thigh = extent.y * .4f * config.upperLegFraction;
        const float shin = extent.y * .4f - thigh;
        const auto shoulder = PatternSystem::addJoint(staged, rig.torso, {side * extent.x * .3f, extent.y * .14f, 0});
        const auto hip = PatternSystem::addJoint(staged, rig.pelvis, {side * extent.x * .15f, 0, 0});

        if (!shoulder || !hip)
            return {};
        rig.shoulders[i] = *shoulder;
        rig.hips[i] = *hip;

        const auto elbow = PatternSystem::addJoint(staged, *shoulder, {0, -upperArm, 0});
        const auto knee = PatternSystem::addJoint(staged, *hip, {0, -thigh, 0});

        if (!elbow || !knee)
            return {};
        rig.elbows[i] = *elbow;
        rig.knees[i] = *knee;

        const auto hand = PatternSystem::addJoint(staged, *elbow, {0, -forearm, 0});
        const auto foot = PatternSystem::addJoint(staged, *knee, {0, -shin, 0});
        const auto upper = PatternSystem::addBox(staged, {extent.x * .15f, upperArm, extent.z * .3f},
                                                 {0, -upperArm / 2, 0}, color, *shoulder);
        const auto lower = PatternSystem::addBox(staged, {extent.x * .15f, forearm, extent.z * .3f},
                                                 {0, -forearm / 2, 0}, color, *elbow);
        const auto thighMesh =
            PatternSystem::addBox(staged, {extent.x * .18f, thigh, extent.z * .4f}, {0, -thigh / 2, 0}, color, *hip);
        const auto shinMesh =
            PatternSystem::addBox(staged, {extent.x * .18f, shin, extent.z * .4f}, {0, -shin / 2, 0}, color, *knee);

        if (!hand || !foot || !upper || !lower || !thighMesh || !shinMesh)
            return {};
        rig.hands[i] = *hand;
        rig.feet[i] = *foot;
        rig.upperArms[i] = *upper;
        rig.forearms[i] = *lower;
        rig.thighs[i] = *thighMesh;
        rig.shins[i] = *shinMesh;
    }
    scene = std::move(staged);

    return rig;
}

bool AnimationSystem::poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose) {
    const std::array<EntityId, 11> joints{rig.pelvis,       rig.torso,     rig.neck,      rig.shoulders[0],
                                          rig.shoulders[1], rig.elbows[0], rig.elbows[1], rig.hips[0],
                                          rig.hips[1],      rig.knees[0],  rig.knees[1]};
    const std::array<glm::vec3, 11> angles{pose.pelvisDegrees, pose.torsoDegrees, pose.neckDegrees, pose.shoulders[0],
                                           pose.shoulders[1],  pose.elbows[0],    pose.elbows[1],   pose.hips[0],
                                           pose.hips[1],       pose.knees[0],     pose.knees[1]};
    std::set<EntityId> seen;
    std::array<Transform3D, 11> transforms;

    for (std::size_t i = 0; i < joints.size(); ++i) {
        const auto* component = scene.getComponent(joints[i]);

        if (!component || !seen.insert(joints[i]).second || !detail::finiteVector(angles[i]))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHumanoid",
                                       "Invalid joint or Euler angles.");

        glm::mat4 rotation(1);

        for (int axis = 0; axis < 3; ++axis) {
            glm::vec3 direction(0);

            direction[axis] = 1;

            const auto turn = TransformSystem::rotation({{0, 0, 0}, direction}, angles[i][axis]);

            if (!turn)
                return false;
            rotation = *turn * rotation;
        }
        transforms[i] = component->transform;
        transforms[i].rotation = glm::mat3(rotation);

        if (!DiagramSystem::validTransform(transforms[i]))
            return false;
    }

    for (std::size_t i = 0; i < joints.size(); ++i)
        if (!scene.setTransform(joints[i], transforms[i]))
            return false;

    return true;
}

std::optional<SwingRig> AnimationSystem::addSwingWithRider(Scene& scene, const SwingConfig& config,
                                                           std::optional<EntityId> parent) {
    Motion swingMotion;
    auto pendulum = config.pendulum;

    pendulum.axis.point = {0, 0, 0};
    pendulum.space = TransformSpace::Local;

    if (!detail::finiteVector(config.pivot) || !detail::positiveLengths(config.seatSize) ||
        !std::isfinite(config.length) || config.length <= 0 || !std::isfinite(config.ropeThickness) ||
        config.ropeThickness <= 0 || !std::isfinite(config.riderSwayDegrees) || config.riderSwayDegrees < 0 ||
        (parent && !scene.getComponent(*parent)) || !swingMotion.append(Motion::pendulum(pendulum))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addSwingWithRider",
                            "Invalid swing configuration or parent.");

        return {};
    }

    Scene staged = scene;
    SwingRig rig;
    Transform3D transform;

    transform.basis = config.pivot;

    const auto frame = staged.addGroup(transform);

    if (!frame || !staged.setParent(*frame, parent))
        return {};
    rig.frame = *frame;

    const auto hinge = PatternSystem::addJoint(staged, rig.frame, {0, 0, 0});

    if (!hinge)
        return {};
    rig.hinge = *hinge;

    const auto seat =
        PatternSystem::addBox(staged, config.seatSize, {0, -config.length, 0}, {.55f, .3f, .1f}, rig.hinge);

    if (!seat)
        return {};
    rig.seat = *seat;

    if (!PatternSystem::addBox(staged, {config.seatSize.x + .6f, .12f, .12f}, {0, 0, 0}, {.3f, .35f, .4f}, rig.frame))
        return {};

    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto rope = PatternSystem::addBox(staged, {config.ropeThickness, config.length, config.ropeThickness},
                                                {side * config.seatSize.x * .43f, -config.length / 2, 0},
                                                {.75f, .75f, .75f}, rig.hinge);

        if (!rope || !PatternSystem::addBox(staged, {.12f, config.length + 1, .12f},
                                            {side * (config.seatSize.x / 2 + .2f), -(config.length + 1) / 2, 0},
                                            {.3f, .35f, .4f}, rig.frame))
            return {};
        rig.ropes[i] = *rope;
    }

    const auto person = addHumanoid(staged, config.person, rig.seat);

    if (!person || !poseHumanoid(staged, *person, HumanoidPose::seated()))
        return {};
    rig.person = *person;

    const auto* root = staged.getComponent(rig.person.root);
    auto personTransform = root->transform;
    const auto hipCenter = config.person.grid.origin + config.person.grid.extent * glm::vec3(.5f, .4f, .5f);
    const float thighHalfThickness = config.person.grid.extent.z * .2f;

    personTransform.basis =
        glm::vec3(0, config.seatSize.y / 2 + thighHalfThickness, 0) - personTransform.rotation * hipCenter;

    if (!staged.setTransform(rig.person.root, personTransform))
        return {};

    const auto swingTrack = createJointAnimation(staged, rig.hinge, swingMotion);
    PendulumMotion rider;

    rider.periodSeconds = pendulum.periodSeconds;
    rider.amplitudeDegrees = config.riderSwayDegrees;
    rider.phaseDegrees = std::remainder(pendulum.phaseDegrees, 360.0) + 180;

    Motion riderMotion;

    if (!riderMotion.append(Motion::pendulum(rider)))
        return {};

    const auto riderTrack = createJointAnimation(staged, rig.person.torso, riderMotion);

    if (!swingTrack || !riderTrack)
        return {};
    rig.animation = {*swingTrack, *riderTrack};

    if (!poseHierarchy(staged, rig.animation, 0))
        return {};
    scene = std::move(staged);

    return rig;
}
} // namespace graphics
