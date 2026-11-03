#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
class Practice18 final : public PracticeMode {
    graphics::Application& app_;
    graphics::EntityId root_ = 0, sun_ = 0;
    std::array<graphics::EntityId, 3> tracks_{}, planets_{}, moonFrames_{}, moonTracks_{}, moons_{};
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0};
    std::array<float, 3> planetAngle_{0, 120, 240}, moonAngle_{0, 120, 240};
    float radius_ = 1, targetRadius_ = 1, sunTime_ = 0, moonTilt_ = 0, moonSpin_ = 0;
    bool pulse_ = false, rotateMoons_ = false;
    /// 계산한 변환을 장면에 반영한다.
    void apply();

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Practice18(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene() override;
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods) override;
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt) override;
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused) override;
};

using practice_common::turn;

namespace {
/// 축과 각도로 회전 행렬을 만든다.


constexpr std::array<float, 3> radii{2.0f, 2.8f, 3.6f};
constexpr std::array<float, 3> inclinations{0, 45, -45};

/// 부모에 연결한 원 궤도를 그린다.
void circle(graphics::Scene& scene, graphics::EntityId owner, graphics::GlColor color, bool xz) {
    constexpr int segments = 96;

    for (int i = 0; i < segments; ++i) {
        const float a = 6.283185307f * i / segments, b = 6.283185307f * (i + 1) / segments;
        const auto point = [&](float angle) {
            return xz ? glm::vec3(std::cos(angle), 0, std::sin(angle)) : glm::vec3(std::cos(angle), std::sin(angle), 0);
        };

        if (!scene.addLine(point(a), point(b), color, owner))
            throw std::runtime_error("Cannot add orbit line.");
    }
}
}

/// 장면 객체와 초기 상태를 구성한다.

bool Practice18::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    root_ = scene.addGroup().value();
    sun_ = scene.addSphere(.55f, {0, 0, 0}).value();

    if (!scene.setParent(sun_, root_))
        return false;

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {0, 5, 8};
    camera.target = {0, 0, 0};
    camera.orthographic = true;
    camera.orthographicHalfExtent = 5.2f;

    const std::array<graphics::GlColor, 3> colors{graphics::GlColor{1, .25f, .2f}, {.2f, .65f, 1}, {.3f, 1, .4f}};
    const std::array<graphics::GlColor, 3> moonColors{graphics::GlColor{.8f, .3f, 1}, {1, .5f, .15f}, {.4f, 1, .85f}};

    for (std::size_t i = 0; i < 3; ++i) {
        tracks_[i] = scene.addGroup().value();
        planets_[i] = scene.addGroup().value();
        moonFrames_[i] = scene.addGroup().value();
        moonTracks_[i] = scene.addGroup().value();

        const auto planet = scene.addSphere(.24f + .06f * float(i), {0, 0, 0}).value();
        const float moonRadius = .09f + .015f * float(i);

        moons_[i] = scene.addSphere(moonRadius, {0, 0, 0}).value();

        if (!scene.setParent(tracks_[i], root_) || !scene.setParent(planets_[i], root_) ||
            !scene.setParent(planet, planets_[i]) || !scene.setParent(moonFrames_[i], planets_[i]) ||
            !scene.setParent(moonTracks_[i], moonFrames_[i]) || !scene.setParent(moons_[i], moonFrames_[i]) ||
            !scene.setTint(planet, colors[i]) || !scene.setTint(moons_[i], moonColors[i]))
            return false;
        circle(scene, tracks_[i], colors[i], true);
        circle(scene, moonTracks_[i], moonColors[i], true);

        if (!scene.addLine({moonRadius, 0, 0}, {2 * moonRadius, 0, 0}, {1, 1, .2f}, moons_[i]))
            return false;
    }
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().lighting = true;
    apply();
    std::cout << "Practice 18: p orthographic / P perspective; m solid / M wire\n"
              << "WASD move XY, +/- move Z; 1 orbit size; 2 sun pulse/color; 3 moon orbit rotation/spin; q quit\n";

    return true;
}

void Practice18::apply() {
    auto& scene = app_.getScene();

    if (!scene.setPosition(root_, position_))
        throw std::runtime_error("Invalid system position.");

    graphics::Transform3D sun;

    sun.scale = glm::vec3(1 + .18f * std::sin(sunTime_));

    if (!scene.setTransform(sun_, sun) ||
        !scene.setTint(sun_, {1, .65f + .2f * std::sin(sunTime_), .15f + .1f * std::cos(sunTime_)}))
        throw std::runtime_error("Invalid sun transform.");

    for (std::size_t i = 0; i < 3; ++i) {
        const auto tilt = turn(inclinations[i], {0, 0, 1});
        graphics::Transform3D track;

        track.rotation = tilt;
        track.scale = glm::vec3(radii[i] * radius_);

        const auto planet = tilt * turn(planetAngle_[i], {0, 1, 0}) * glm::vec3(radii[i] * radius_, 0, 0);
        graphics::Transform3D frame;

        const auto& camera = app_.getCamera();
        const auto forward = glm::normalize(camera.target - camera.position);
        const auto right = glm::normalize(glm::cross(forward, camera.up));
        const auto screenUp = glm::cross(right, forward);
        frame.rotation = turn(moonTilt_, {0, 0, 1}) * glm::mat3(right, forward, screenUp);

        const float r = (.55f + .15f * float(i)) * radius_, a = glm::radians(moonAngle_[i]);
        graphics::Transform3D moonTrack;

        moonTrack.scale = glm::vec3(r);

        graphics::Transform3D moon;

        moon.basis = {r * std::cos(a), 0, r * std::sin(a)};
        moon.rotation = turn(moonSpin_, {0, 1, 0});

        if (!scene.setTransform(tracks_[i], track) || !scene.setPosition(planets_[i], planet) ||
            !scene.setTransform(moonFrames_[i], frame) || !scene.setTransform(moonTracks_[i], moonTrack) ||
            !scene.setTransform(moons_[i], moon))
            throw std::runtime_error("Invalid orbit transform.");
    }
}

void Practice18::onKey(int key, int action, int mods) {
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS)
            pressed_[key] = true;
        else if (action == GLFW_RELEASE)
            pressed_[key] = false;
    }

    if (action != GLFW_PRESS)
        return;

    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    switch (key) {
    case GLFW_KEY_P:
        app_.getCamera().orthographic = !upper;
        break;
    case GLFW_KEY_M:
        app_.getRenderOptions().wireframe = upper;
        break;
    case GLFW_KEY_1:
        targetRadius_ = targetRadius_ == 1 ? 1.3f : 1.0f;
        break;
    case GLFW_KEY_2:
        pulse_ = !pulse_;
        break;
    case GLFW_KEY_3:
        rotateMoons_ = !rotateMoons_;
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void Practice18::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_D]) - float(pressed_[GLFW_KEY_A]),
                       float(pressed_[GLFW_KEY_W]) - float(pressed_[GLFW_KEY_S]),
                       float(pressed_[GLFW_KEY_EQUAL] || pressed_[GLFW_KEY_KP_ADD]) -
                           float(pressed_[GLFW_KEY_MINUS] || pressed_[GLFW_KEY_KP_SUBTRACT])};

    if (glm::length(movement) > 0)
        position_ += glm::normalize(movement) * dt * 1.5f;
    radius_ += std::clamp(targetRadius_ - radius_, -.3f * dt, .3f * dt);

    if (pulse_)
        sunTime_ = std::fmod(sunTime_ + dt, 6.283185307f);

    if (rotateMoons_) {
        moonTilt_ = std::fmod(moonTilt_ + 25 * dt, 360.0f);
        moonSpin_ = std::fmod(moonSpin_ + 90 * dt, 360.0f);
    }

    for (std::size_t i = 0; i < 3; ++i) {
        planetAngle_[i] = std::fmod(planetAngle_[i] + (20 + 10 * float(i)) * dt, 360.0f);
        moonAngle_[i] = std::fmod(moonAngle_[i] + (70 + 30 * float(i)) * dt, 360.0f);
    }
    apply();
}

void Practice18::onFocus(bool focused) {
    if (!focused)
        pressed_.fill(false);
}
}

std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app) {
    return std::make_unique<Practice18>(app);
}
