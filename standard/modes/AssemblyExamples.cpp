#include "AssemblyExamples.h"

namespace assembly {
namespace {
void pendulum(Assembly& scene, const std::string& id, const std::string& target, glm::vec3 axis,
              double amplitude, double period, double offset = 0, double phase = 0) {
    graphics::PendulumMotion config;
    config.axis.direction = axis; config.amplitudeDegrees = amplitude;
    config.periodSeconds = period; config.offsetDegrees = offset; config.phaseDegrees = phase;
    Behavior behavior;
    behavior.name = id; behavior.target = target;
    require(behavior.motion.append(graphics::Motion::pendulum(config)), "예제 진자 생성 실패.");
    scene.attach(std::move(behavior));
}
}

void buildTankExample(Assembly& scene, const std::string& name) {
    scene.group(name, {0, 0, 0});
    scene.node(name).guideTopic = "tank";
    scene.part("box", name, "body", {0, .45f, 0}, {2.2f, .5f, 2.6f});
    for (int i = 0; i < 2; ++i)
        scene.part("box", name, i == 0 ? "leftTrack" : "rightTrack",
            {i == 0 ? -1.05f : 1.05f, .3f, 0}, {.45f, .6f, 3});
    const auto turret = scene.part("joint", name, "turret", {0, .8f, 0});
    scene.part("box", turret, "body", {0, .25f, 0}, {1.4f, .5f, 1.5f});
    const auto gun = scene.part("joint", turret, "gun", {0, .3f, .65f});
    scene.part("box", gun, "barrel", {0, 0, 1}, {.2f, .2f, 2});
    scene.part("box", gun, "muzzle", {0, 0, 2}, {.35f, .35f, .3f});
    scene.pivot(name, {0, .6f, 0});
    graphics::RotationMotion rotation;
    rotation.degreesPerSecond = 25;
    Behavior motor;
    motor.name = name + ".turretMotor"; motor.target = turret;
    require(motor.motion.append(graphics::Motion::rotation(rotation)), "포탑 회전 생성 실패.");
    scene.attach(std::move(motor));
    pendulum(scene, name + ".gunMotor", gun, {1, 0, 0}, 10, 4);
}

void buildTruckExample(Assembly& scene, const std::string& name) {
    scene.group(name, {0, 0, 0});
    scene.node(name).guideTopic = "truck";
    scene.part("box", name, "chassis", {0, .6f, 0}, {1.8f, .3f, 3.2f});
    scene.part("box", name, "cabin", {0, 1.2f, 1.1f}, {1.8f, 1.2f, 1.2f});
    for (int i = 0; i < 4; ++i)
        scene.part("box", name, "wheel" + std::to_string(i + 1),
            {i % 2 == 0 ? -.95f : .95f, .3f, i < 2 ? -1.05f : 1.05f}, {.4f, .6f, .6f});
    const auto cargo = scene.part("joint", name, "container", {0, .85f, -.65f});
    scene.part("box", cargo, "floor", {0, 0, 0}, {1.8f, .15f, 1.9f});
    scene.part("box", cargo, "roof", {0, 1.25f, 0}, {1.8f, .15f, 1.9f});
    scene.part("box", cargo, "leftWall", {-.85f, .625f, 0}, {.1f, 1.2f, 1.9f});
    scene.part("box", cargo, "rightWall", {.85f, .625f, 0}, {.1f, 1.2f, 1.9f});
    scene.part("box", cargo, "frontWall", {0, .625f, .9f}, {1.8f, 1.2f, .1f});
    const auto left = scene.part("joint", cargo, "leftDoor", {-.85f, 0, -.95f});
    const auto right = scene.part("joint", cargo, "rightDoor", {.85f, 0, -.95f});
    scene.part("box", left, "panel", {.425f, .625f, 0}, {.85f, 1.2f, .1f});
    scene.part("box", right, "panel", {-.425f, .625f, 0}, {.85f, 1.2f, .1f});
    scene.pivot(name, {0, .8f, 0});
    pendulum(scene, name + ".leftDoorMotor", left, {0, 1, 0}, 40, 6, 40, -90);
    pendulum(scene, name + ".rightDoorMotor", right, {0, 1, 0}, 40, 6, -40, 90);
}

void buildPersonExample(Assembly& scene, const std::string& name) {
    scene.group(name, {0, 0, 0});
    scene.node(name).guideTopic = "person";
    const auto pelvis = scene.part("joint", name, "pelvis", {0, .8f, 0});
    scene.part("box", pelvis, "body", {0, 0, 0}, {.4f, .25f, .3f});
    const auto torso = scene.part("joint", pelvis, "torso", {0, .35f, 0});
    scene.part("box", torso, "body", {0, 0, 0}, {.55f, .6f, .3f});
    const auto neck = scene.part("joint", torso, "neck", {0, .35f, 0});
    scene.part("box", neck, "head", {0, .18f, 0}, {.35f, .35f, .35f});
    scene.part("box", neck, "nose", {0, .18f, .21f}, {.1f, .08f, .12f});
    for (int i = 0; i < 2; ++i) {
        const std::string side = i == 0 ? "left" : "right";
        const float sign = i == 0 ? -1.0f : 1.0f;
        const auto shoulder = scene.part("joint", torso, side + "Shoulder", {sign * .38f, .17f, 0});
        scene.part("box", shoulder, "upperArm", {0, -.2f, 0}, {.14f, .4f, .16f});
        const auto elbow = scene.part("joint", shoulder, "elbow", {0, -.4f, 0});
        scene.part("box", elbow, "forearm", {0, -.18f, 0}, {.13f, .36f, .15f});
        const auto hip = scene.part("joint", pelvis, side + "Hip", {sign * .13f, 0, 0});
        scene.part("box", hip, "thigh", {0, -.2f, 0}, {.17f, .4f, .2f});
        const auto knee = scene.part("joint", hip, "knee", {0, -.4f, 0});
        scene.part("box", knee, "shin", {0, -.2f, 0}, {.17f, .4f, .2f});
        scene.part("box", knee, "foot", {0, -.4f, .08f}, {.2f, .1f, .35f});
        pendulum(scene, name + "." + side + "ArmMotor", shoulder, {1, 0, 0}, 25, 2, 0, i * 180);
        pendulum(scene, name + "." + side + "LegMotor", hip, {1, 0, 0}, 25, 2, 0, (1 - i) * 180);
    }
    scene.pivot(name, {0, .9f, 0});
}
}
