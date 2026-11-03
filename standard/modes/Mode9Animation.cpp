#include "../Console.h"
#include <iostream>

/// 선형보간과 그네·사람의 부모·자식 애니메이션을 구성한다.
bool Console::initAnimation() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Animation;
    clock_.reset();
    animation_.clear();
    jointControls_.clear();
    selectedJoint_ = 0;
    selectedAxis_ = 0;

    if (!graphics::PatternSystem::addBox(scene, {8, .15f, 6}, {0, -.15f, 0}, {.25f, .3f, .35f}))
        return false;

    graphics::SwingConfig swingConfig;

    swingConfig.pivot = {1, 3, 0};
    swingConfig.pendulum.amplitudeDegrees = 25;
    swingConfig.pendulum.periodSeconds = 3;

    const auto swing = graphics::AnimationSystem::addSwingWithRider(scene, swingConfig);

    if (!swing)
        return false;
    animation_ = swing->animation;

    graphics::HumanoidConfig personConfig;

    personConfig.grid.frame = glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0));

    const auto person = graphics::AnimationSystem::addHumanoid(scene, personConfig);

    if (!person)
        return false;

    auto pose = graphics::HumanoidPose::seated();

    pose.hips = {glm::vec3(0), glm::vec3(0)};
    pose.knees = {glm::vec3(0), glm::vec3(0)};

    if (!graphics::AnimationSystem::poseHumanoid(scene, *person, pose))
        return false;

    for (std::size_t i = 0; i < 2; ++i) {
        graphics::PendulumMotion sway;

        sway.amplitudeDegrees = 30;
        sway.periodSeconds = 2;
        sway.phaseDegrees = i == 0 ? 0 : 180;

        graphics::Motion motion;

        if (!motion.append(graphics::Motion::pendulum(sway)))
            return false;

        const auto track = graphics::AnimationSystem::createJointAnimation(scene, person->shoulders[i], motion);

        if (!track)
            return false;
        animation_.push_back(*track);
    }

    const auto marker = scene.addGroup();

    if (!marker || !graphics::PatternSystem::addBox(scene, {.4f, .4f, .4f}, {0, 0, 0}, {1, .85f, .15f}, *marker))
        return false;

    graphics::LinearMotion move;

    move.from = {-2, .3f, 2};
    move.to = {2, .3f, 2};
    move.durationSeconds = 4;
    move.playback = graphics::Playback::PingPong;

    graphics::Motion motion;

    if (!motion.append(graphics::Motion::linear(move)))
        return false;

    const auto track = graphics::AnimationSystem::createJointAnimation(scene, *marker, motion);

    if (!track)
        return false;
    animation_.push_back(*track);

    const auto control = [&](graphics::EntityId id, const std::string& name) {
        std::size_t index = 0;

        while (index < animation_.size() && animation_[index].joint != id)
            ++index;

        if (index == animation_.size()) {
            auto fixed = graphics::AnimationSystem::createJointAnimation(scene, id, graphics::Motion{});

            if (!fixed)
                return false;
            animation_.push_back(*fixed);
        }
        jointControls_.push_back({index, name, animation_[index].rest});

        return true;
    };

    const auto personControls = [&](const graphics::HumanoidRig& rig, const std::string& prefix) {
        if (!control(rig.root, prefix + " root") || !control(rig.pelvis, prefix + " pelvis") ||
            !control(rig.torso, prefix + " torso") || !control(rig.neck, prefix + " neck"))
            return false;

        for (std::size_t i = 0; i < 2; ++i) {
            const auto side = prefix + (i == 0 ? " left " : " right ");

            if (!control(rig.shoulders[i], side + "shoulder") || !control(rig.elbows[i], side + "elbow") ||
                !control(rig.hands[i], side + "hand") || !control(rig.hips[i], side + "hip") ||
                !control(rig.knees[i], side + "knee") || !control(rig.feet[i], side + "foot"))
                return false;
        }

        return true;
    };

    if (!control(swing->frame, "swing frame") || !control(swing->hinge, "swing hinge") ||
        !control(swing->seat, "swing seat") || !personControls(swing->person, "rider") ||
        !personControls(*person, "standing"))
        return false;
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 10};
    app_.getCamera().target = {0, 1.4f, 0};
    std::cout << "9: 노란 상자는 4초 편도 선형보간, 그네는 3초 주기 진자, 좌석의 사람은 계층 변환을 따릅니다.\n";
    printSelectedJoint();

    return graphics::AnimationSystem::poseHierarchy(scene, animation_, 0);
}

bool Console::updateAnimation() {
    return graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds);
}
