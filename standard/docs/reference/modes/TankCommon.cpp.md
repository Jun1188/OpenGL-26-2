# 과거 구현 참조: TankCommon.cpp

- 원래 위치: `standard/modes/TankCommon.cpp`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#include "TankCommon.h"
#include "AssemblyParts.h"
#include "PracticeCommon.h"
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
class TankMode final : public PracticeMode {
    graphics::Application& app_;
    bool threeViews_;
    graphics::EntityId root_ = 0, central_ = 0;
    std::array<graphics::EntityId, 2> lower_{}, upper_{}, guns_{}, poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_ = 0, centralAngle_ = 0, gunAngle_ = 0, poleTime_ = 0;
    float swapProgress_ = 0, swapDirection_ = 1;
    float cameraX_ = 0, cameraZ_ = 0, cameraYaw_ = 0, cameraOrbit_ = 0;
    bool lowerRunning_ = false, centralRunning_ = false, gunRunning_ = false, poleRunning_ = false;
    bool swapRunning_ = false, swapStarted_ = false;

    unsigned colorVersion_ = 0;

    graphics::EntityId box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent);

    void reset();
    void apply();
    void recolor();

public:
    TankMode(graphics::Application& app, bool threeViews) : PracticeMode(app), app_(app), threeViews_(threeViews) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float dt) override;
    void onFocus(bool focused) override;

    bool onRender() override;
};

using practice_common::requireEntity;

namespace {

}

graphics::EntityId TankMode::box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent) {
    const auto id = requireEntity(graphics::PatternSystem::addBox(app_.getScene(), size, position, {1, 1, 1}, parent),
                                  "탱크 상자 생성 실패: 크기·위치·부모 ID를 확인하세요.");

    colored_.push_back(id);

    return id;
}

bool TankMode::initScene() {
    reset();
    std::cout << (threeViews_ ? "실습 22: 원근 / XZ 평면도 / XY 정면도\n" : "실습 19: 탱크와 부품·카메라 조작\n")
              << "방향키 이동; d 하부 높이; t 중앙 자전; l 상부 교환\n"
              << "g 포신 반대 회전; p 깃대 반대 기울기; x/X,z/Z 카메라 이동\n"
              << "y/Y 카메라 시선 회전; r/R 공전(재입력 정지); O/o 색 변경; c 초기화; q 종료\n";

    return true;
}

void TankMode::reset() {
    auto& scene = app_.getScene();

    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_ = glm::vec3(0);
    lowerTime_ = centralAngle_ = gunAngle_ = poleTime_ = swapProgress_ = 0;
    swapDirection_ = 1;
    cameraX_ = cameraZ_ = cameraYaw_ = cameraOrbit_ = 0;
    lowerRunning_ = centralRunning_ = gunRunning_ = poleRunning_ = swapRunning_ = swapStarted_ = false;
    colorVersion_ = 0;
    const auto ground = requireEntity(scene.addGroup(), "바닥 그룹 생성 실패.");
    box({12, .1f, 12}, {0, -.05f, 0}, ground);
    const auto rig = assembly::addTankParts(scene);
    root_ = rig.root; central_ = rig.central;
    lower_ = rig.lower; upper_ = rig.upper; guns_ = rig.guns; poles_ = rig.poles;
    colored_.insert(colored_.end(), rig.colored.begin(), rig.colored.end());

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {-5, 4.5f, 7};
    camera.target = {0, .7f, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 6.5f;
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().lighting = true;
    recolor();
    apply();
}

void TankMode::recolor() {
    const std::array<graphics::GlColor, 10> palette{graphics::GlColor{.3f, .35f, .4f},
                                                    {.25f, .6f, .3f},
                                                    {.55f, .35f, .2f},
                                                    {.2f, .55f, .75f},
                                                    {.7f, .75f, .2f},
                                                    {.9f, .4f, .15f},
                                                    {1, .8f, .2f},
                                                    {.4f, .7f, .8f},
                                                    {.7f, .25f, .6f},
                                                    {.6f, .9f, .4f}};

    for (std::size_t i = 0; i < colored_.size(); ++i)
        if (!app_.getScene().setTint(colored_[i], palette[(i + colorVersion_) % palette.size()]))
            throw std::runtime_error("탱크 부품 색상 적용 실패.");
}

void TankMode::apply() {
    auto& scene = app_.getScene();

    if (!scene.setPosition(root_, position_))
        throw std::runtime_error("탱크 위치 적용 실패.");

    const float factor = 1 + .25f * std::sin(lowerTime_), height = .6f * factor;

    if (!scene.setPosition(central_, {0, height, 0}) ||
        !graphics::PatternSystem::rotateJoint(scene, central_, {0, 1, 0}, centralAngle_))
        throw std::runtime_error("중앙 관절 변환 적용 실패.");

    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        graphics::Transform3D low;

        low.basis = {sign * .8f, height * .5f, 0};
        low.scale = {1, factor, 1};

        const float a = 3.14159265f * swapProgress_;
        const glm::vec3 upperPosition{sign * .65f * std::cos(a), .25f, sign * .65f * std::sin(a)};

        if (!scene.setTransform(lower_[i], low) || !scene.setPosition(upper_[i], upperPosition) ||
            !graphics::PatternSystem::rotateJoint(scene, guns_[i], {0, 1, 0}, sign * gunAngle_) ||
            !graphics::PatternSystem::rotateJoint(scene, poles_[i], {1, 0, 0}, sign * 60 * std::sin(poleTime_)))
            throw std::runtime_error("탱크 부품 변환 적용 실패.");
    }
}

void TankMode::onKey(int key, int action, int mods) {
    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    if (key == GLFW_KEY_X || key == GLFW_KEY_Z) {
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            float& direction = key == GLFW_KEY_X ? cameraX_ : cameraZ_;

            direction = action == GLFW_RELEASE ? 0 : (upper ? -1.0f : 1.0f);
        }

        return;
    }

    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS)
            pressed_[key] = true;
        else if (action == GLFW_RELEASE)
            pressed_[key] = false;
    }

    if (action != GLFW_PRESS)
        return;

    const float direction = upper ? 1.0f : -1.0f;

    switch (key) {
    case GLFW_KEY_D:
        lowerRunning_ = !lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_ = !centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_ = !gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_ = !poleRunning_;
        break;
    case GLFW_KEY_L:
        if (!swapStarted_) {
            swapStarted_ = true;
            swapRunning_ = true;
        } else if (!swapRunning_ && (swapProgress_ == 0 || swapProgress_ == 1)) {
            swapDirection_ = -swapDirection_;
            swapRunning_ = true;
        } else
            swapRunning_ = !swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_ = cameraYaw_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_ = cameraOrbit_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_O:
        colorVersion_ = (colorVersion_ + 1) % 10;
        recolor();
        break;
    case GLFW_KEY_C:
        reset();
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void TankMode::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT]) - float(pressed_[GLFW_KEY_LEFT]), 0,
                       float(pressed_[GLFW_KEY_DOWN]) - float(pressed_[GLFW_KEY_UP])};

    if (glm::length(movement) > 0)
        position_ += glm::normalize(movement) * dt * 1.3f;

    position_.x = std::clamp(position_.x, -3.5f, 3.5f);
    position_.z = std::clamp(position_.z, -3.5f, 3.5f);

    if (lowerRunning_)
        lowerTime_ = std::fmod(lowerTime_ + dt, 6.283185307f);

    if (centralRunning_)
        centralAngle_ = std::fmod(centralAngle_ + 45 * dt, 360.0f);

    if (gunRunning_)
        gunAngle_ = std::fmod(gunAngle_ + 60 * dt, -360.0f);

    if (poleRunning_)
        poleTime_ = std::fmod(poleTime_ + dt, 6.283185307f);

    if (swapRunning_) {
        swapProgress_ = std::clamp(swapProgress_ + swapDirection_ * dt * .5f, 0.0f, 1.0f);

        if (swapProgress_ == 0 || swapProgress_ == 1)
            swapRunning_ = false;
    }

    auto camera = app_.getCamera();
    const glm::vec3 delta{cameraX_ * 2 * dt, 0, cameraZ_ * 2 * dt};

    if (!graphics::PatternSystem::moveCamera(camera, delta) ||
        (cameraYaw_ != 0 && !graphics::PatternSystem::yawCamera(camera, cameraYaw_ * 35 * dt)) ||
        (cameraOrbit_ != 0 &&
         !graphics::PatternSystem::orbitCamera(camera, {{0, 0, 0}, {0, 1, 0}}, cameraOrbit_ * 25 * dt)))
        throw std::runtime_error("카메라 갱신 실패: 위치·시선·회전축을 확인하세요.");
    app_.getCamera() = camera;
    apply();
}

void TankMode::onFocus(bool focused) {
    if (!focused) {
        pressed_.fill(false);
        cameraX_ = cameraZ_ = 0;
    }
}

bool TankMode::onRender() {
    if (!threeViews_)
        return app_.renderScene();
    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    const auto passes =
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

    if (passes.size() != 3)
        throw std::runtime_error("세 카메라 뷰 생성 실패: 프레임버퍼 크기와 카메라를 확인하세요.");

    return app_.renderViews(passes);
}
}

std::unique_ptr<PracticeMode> makeTankMode(graphics::Application& app, bool threeViews) {
    return std::make_unique<TankMode>(app, threeViews);
}
````
