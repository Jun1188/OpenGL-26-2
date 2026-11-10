# 과거 구현 참조: Mode20Practice23.cpp

- 원래 위치: `standard/modes/Mode20Practice23.cpp`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace {
class Practice23 final : public PracticeMode {
    graphics::Application& app_;
    graphics::CharacterBody body_;
    graphics::JointRig rig_;
    graphics::EntityId stage_ = 0;
    std::vector<graphics::Bounds3> obstacles_;
    graphics::Bounds3 bounds_{{-5, 0, -5}, {5, 4, 5}};
    std::array<float, 4> doors_{};
    std::array<bool, 4> open_{};
    std::optional<std::size_t> activeWall_;

    unsigned randomSeed_ = std::random_device{}();

    std::mt19937 random_{randomSeed_};
    bool walking_ = true;

public:
    explicit Practice23(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float dt) override;
};

using practice_common::require;

namespace {

}

bool Practice23::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    obstacles_.clear();
    body_ = graphics::CharacterBody{};
    body_.feet = {-3, 0, -3};
    body_.speed = 1;
    body_.height = 1.8f;
    body_.halfWidth = {.5f, .5f};
    doors_.fill(0);
    open_.fill(false);
    activeWall_.reset();
    walking_ = true;

    const auto stage = graphics::PatternSystem::addRoom(scene, bounds_, false);

    if (!stage)
        return false;
    stage_ = *stage;

    for (std::size_t face = 0; face < 4; ++face) {
        if (!graphics::PatternSystem::openRoomWall(scene, stage_, face, 0))
            return false;
    }

    graphics::GridLayout grid;

    grid.origin = {-.5f, 0, -.3f};
    grid.extent = {1, 1.8f, .6f};

    const auto rig = graphics::PatternSystem::addWalkingRig(scene, grid);

    if (!rig)
        return false;
    rig_ = *rig;
    std::cout << "실습 23 난수 시드=" << randomSeed_ << "; 초기화 시 기존 난수 엔진의 다음 수열을 사용합니다.\n";

    std::vector<graphics::Bounds3> occupied{{{-3.8f, 0, -3.8f}, {-2.2f, 2, -2.2f}}};

    for (unsigned i = 0; i < 3; ++i) {
        const glm::vec3 size{.9f + .2f * i, .6f + .25f * i, .9f + .15f * i};
        const auto box =
            graphics::CollisionSystem::randomFreeBox({{-4, 0, -4}, {4, size.y, 4}}, size, occupied, random_);

        if (!box)
            return false;

        if (!graphics::PatternSystem::addBox(scene, size, (box->minimum + box->maximum) * .5f,
                                             {.3f + .2f * i, .5f, .8f - .2f * i}))
            return false;
        occupied.push_back(*box);
        obstacles_.push_back(*box);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 11};
    app_.getCamera().target = {0, 1, 0};
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().depthTest = true;
    app_.getRenderOptions().lighting = true;

    return graphics::PatternSystem::poseWalkingRig(scene, rig_, body_.feet, 0, 0, body_.speed);
}

void Practice23::onKey(int key, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    auto& camera = app_.getCamera();

    if (key == GLFW_KEY_X || key == GLFW_KEY_Z) {
        glm::vec3 delta(0);

        delta[key == GLFW_KEY_X ? 0 : 2] = upper ? -.35f : .35f;
        require(graphics::PatternSystem::moveCamera(camera, delta), "실습 23 카메라 이동 실패.");

        return;
    }

    if (key == GLFW_KEY_Y) {
        require(graphics::PatternSystem::orbitCamera(camera, {camera.target, {0, 1, 0}}, upper ? 4.0f : -4.0f),
                "실습 23 카메라 공전 실패.");

        return;
    }

    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
        body_.speed = std::min(3.0f, body_.speed + .1f);

    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
        body_.speed = std::max(0.0f, body_.speed - .1f);

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        body_.direction = {0, 0, 1};
        walking_ = true;
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        body_.direction = {0, 0, -1};
        walking_ = true;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        body_.direction = {-1, 0, 0};
        walking_ = true;
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        body_.direction = {1, 0, 0};
        walking_ = true;
    }

    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_O) {
        if (activeWall_)
            open_[*activeWall_] = !open_[*activeWall_];
        else {
            activeWall_ = graphics::PatternSystem::cameraFacingWall(app_.getScene(), stage_, camera);

            if (activeWall_)
                open_[*activeWall_] = true;
        }

        if (activeWall_)
            std::cout << "선택한 벽=" << *activeWall_ << (open_[*activeWall_] ? " 열기\n" : " 원상복귀\n");
    }

    if (key == GLFW_KEY_SPACE)
        walking_ = !walking_;

    if (key == GLFW_KEY_J)
        graphics::CollisionSystem::jump(body_, 5.5f);

    if (key == GLFW_KEY_I)
        require(initScene(), "실습 23 초기화 실패: 앞선 API 오류를 확인하세요.");

    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
        app_.requestClose();
}

void Practice23::onUpdate(float dt) {
    const float requested = body_.speed;

    if (!walking_)
        body_.speed = 0;

    const auto result = graphics::CollisionSystem::stepCharacter(body_, bounds_, obstacles_, dt);

    body_.speed = requested;
    require(result.has_value(), "실습 23 캐릭터 충돌 계산 실패.");

    const float yaw = glm::degrees(std::atan2(body_.direction.x, body_.direction.z));

    require(graphics::PatternSystem::poseWalkingRig(app_.getScene(), rig_, body_.feet, yaw, body_.walkDistance,
                                                    body_.speed),
            "실습 23 로봇 관절 자세 적용 실패.");

    for (std::size_t face = 0; face < doors_.size(); ++face) {
        doors_[face] = std::clamp(doors_[face] + (open_[face] ? 1.0f : -1.0f) * dt, 0.0f, 1.0f);
        require(graphics::PatternSystem::openRoomWall(app_.getScene(), stage_, face, doors_[face]),
                "실습 23 벽 개방·복원 실패.");
    }

    if (activeWall_ && !open_[*activeWall_] && doors_[*activeWall_] == 0)
        activeWall_.reset();
}
}

std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app) {
    return std::make_unique<Practice23>(app);
}
````
