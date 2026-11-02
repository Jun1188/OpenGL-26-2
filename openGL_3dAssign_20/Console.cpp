#include "Console.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {
constexpr float beltHeight = 1.0f;
constexpr float boxSize = .85f;
constexpr double beltSpeed = 1.25;
constexpr std::size_t stripeCount = 30;
constexpr float cornerRadius = .8f;
constexpr double halfPi = 1.57079632679489661923;
const glm::vec3 conveyorStart{-4, beltHeight, -2};
const glm::vec3 cornerPivot{2.2f, beltHeight, -1.2f};
constexpr float conveyorEndZ = 3.0f;
const double conveyorLength =
    (double(cornerPivot.x) - conveyorStart.x) + halfPi * double(cornerRadius) + (double(conveyorEndZ) - cornerPivot.z);

void require(bool success, const char* operation) {
    if (!success)
        throw std::runtime_error(operation);
}
}

bool Console::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    stripes_.clear();

    require(
        graphics::PatternSystem::addPolygon(scene, {{-6, 0, -4}, {-6, 0, 5}, {6, 0, 5}, {6, 0, -4}}, {.25f, .30f, .34f})
            .has_value(),
        "Floor creation failed.");

    require(graphics::PatternSystem::addPolygon(scene,
                                                {{-4.8f, beltHeight, -2.8f},
                                                 {-4.8f, beltHeight, -1.2f},
                                                 {3.8f, beltHeight, -1.2f},
                                                 {3.8f, beltHeight, -2.8f}},
                                                {.09f, .12f, .15f})
                .has_value(),
            "Horizontal conveyor plane creation failed.");
    require(
        graphics::PatternSystem::addPolygon(
            scene,
            {{2.2f, beltHeight, -1.2f}, {2.2f, beltHeight, 3.8f}, {3.8f, beltHeight, 3.8f}, {3.8f, beltHeight, -1.2f}},
            {.09f, .12f, .15f})
            .has_value(),
        "Vertical conveyor plane creation failed.");

    for (std::size_t i = 0; i < stripeCount; ++i) {
        const auto stripe = graphics::PatternSystem::addBox(scene, {1.4f, .03f, .10f}, {0, 0, 0}, {.72f, .77f, .80f});

        require(stripe.has_value(), "Conveyor stripe creation failed.");
        stripes_.push_back(*stripe);
    }

    const std::array<graphics::GlColor, 5> colors{
        {{.95f, .30f, .23f}, {.23f, .72f, .96f}, {.98f, .75f, .20f}, {.38f, .86f, .43f}, {.78f, .44f, .96f}}};

    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const auto box = graphics::PatternSystem::addBox(scene, {boxSize, boxSize, boxSize}, {0, 0, 0}, colors[i]);

        require(box.has_value(), "Parcel creation failed.");
        boxes_[i] = *box;

        for (int face = 0; face < 4; ++face) {
            const auto label = graphics::PatternSystem::addJoint(scene, *box, {0, 0, 0});

            require(label.has_value(), "Parcel label frame creation failed.");
            require(graphics::PatternSystem::rotateJoint(scene, *label, {0, 1, 0}, float(face) * 90.0f),
                    "Parcel label frame rotation failed.");

            const int count = int(i + 1);
            const float z = boxSize * .5f + .008f;

            for (int mark = 0; mark < count; ++mark) {
                const float x = (float(mark) - float(count - 1) * .5f) * .14f;

                require(graphics::PatternSystem::addPolygon(
                            scene,
                            {{x - .05f, -.05f, z}, {x + .05f, -.05f, z}, {x + .05f, .05f, z}, {x - .05f, .05f, z}},
                            {0, 0, 0}, *label)
                            .has_value(),
                        "Parcel square mark creation failed.");
            }
        }
    }

    auto& options = app_.getRenderOptions();

    options = graphics::RenderOptions{};
    options.depthTest = true;
    options.lighting = true;
    options.background = {.08f, .10f, .14f, 1};
    resetState();
    std::cout << "실습 20: 택배 물류 센터\n"
              << "b 이동 시작/정지 | +/- 확대/축소 | p 원근 / P 직각\n"
              << "좌/우 중심 Y축 공전 | r 전체 리셋 | q 종료\n";

    return true;
}

void Console::placeMovingObjects() {
    auto& scene = app_.getScene();
    const double travelled = clock_.seconds * beltSpeed;
    const auto place = [&](graphics::EntityId entity, double distance, float height) {
        const auto pose = graphics::PatternSystem::conveyorCornerPose(distance, conveyorStart, cornerPivot,
                                                                      cornerRadius, conveyorEndZ);

        require(pose.has_value(), "Conveyor corner pose sampling failed.");

        const auto* component = scene.getComponent(entity);

        require(component != nullptr, "Conveyor moving entity is missing.");

        auto transform = component->transform;

        transform.basis = pose->position + glm::vec3(0, height, 0);
        transform.rotation = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(pose->yawDegrees), glm::vec3(0, 1, 0)));
        require(scene.setTransform(entity, transform), "Conveyor moving entity transform failed.");
    };

    for (std::size_t i = 0; i < stripes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(stripes_.size());

        place(stripes_[i], travelled + offset, .015f);
    }

    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(boxes_.size());

        place(boxes_[i], travelled + offset, .03f + boxSize * .5f);
    }
}

void Console::resetState() {
    clock_.reset();
    clock_.playing = false;
    leftHeld_ = false;
    rightHeld_ = false;

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {11, 10, 13};
    camera.target = {0, .7f, .3f};
    camera.up = {0, 1, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 7.5f;
    camera.nearPlane = .1f;
    camera.farPlane = 120;
    require(graphics::CameraSystem::validCamera(camera), "Reset camera is invalid.");
    placeMovingObjects();
}

void Console::zoom(float factor) {
    auto& camera = app_.getCamera();
    const float current =
        camera.orthographic ? camera.orthographicHalfExtent : glm::length(camera.position - camera.target);

    const float next = std::clamp(current * factor, 2.0f, camera.orthographic ? 35.0f : 60.0f);

    require(graphics::PatternSystem::zoomCamera(camera, next / current), "Camera zoom failed.");
}

void Console::onKey(int key, int action, int mods) {
    if (key == GLFW_KEY_LEFT)
        leftHeld_ = action != GLFW_RELEASE;

    if (key == GLFW_KEY_RIGHT)
        rightHeld_ = action != GLFW_RELEASE;

    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
        zoom(.9f);

    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
        zoom(1.0f / .9f);

    if (action != GLFW_PRESS)
        return;

    switch (key) {
    case GLFW_KEY_B:
        clock_.playing = !clock_.playing;
        break;
    case GLFW_KEY_P:

        app_.getCamera().orthographic = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
        break;
    case GLFW_KEY_R:
        resetState();
        break;
    case GLFW_KEY_Q:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void Console::onFocus(bool focused) {
    if (!focused) {
        leftHeld_ = false;
        rightHeld_ = false;
    }
}

void Console::onUpdate(float dt) {
    require(clock_.advance(dt), "Conveyor clock update failed.");
    placeMovingObjects();

    const int direction = int(rightHeld_) - int(leftHeld_);

    if (direction != 0) {
        const graphics::AxisLine axis{app_.getCamera().target, {0, 1, 0}};

        require(graphics::PatternSystem::orbitCamera(app_.getCamera(), axis, float(direction) * 65.0f * dt),
                "Camera orbit failed.");
    }
}
