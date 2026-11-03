#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
class Practice17 final : public PracticeMode {
    struct Object {
        graphics::EntityId orbit = 0, body = 0;
        glm::vec3 position{0};
        glm::mat3 rotation{1};
        glm::vec2 spin{0};
        float orbitDegrees = 0, orbitDirection = 0, scale = .65f;
        int shape = 0;
    };
    enum class Motion { None, Straight, Bypass, Combined };

    graphics::Application& app_;
    std::array<Object, 2> objects_{};
    int selected_ = 3;
    glm::vec2 movement_{0};
    Motion motion_ = Motion::None;
    bool running_ = false;
    float elapsed_ = 0;
    std::array<glm::vec3, 2> starts_{};
    std::array<float, 2> scales_{};
    std::array<glm::vec3, 2> motionOffsets_{glm::vec3(0), glm::vec3(0)};
    /// 배치와 조작 상태를 초기화한다.
    void reset();
    /// 계산한 변환을 장면에 반영한다.
    void apply();
    /// 선택한 이동을 시작한다.
    void begin(Motion motion);
    /// 선택한 도형의 모델을 만든다.
    graphics::Model model(int shape) const;
    /// 공전 회전 행렬을 계산한다.
    glm::mat3 orbitRotation(std::size_t i) const;
    /// 물체의 월드 위치를 계산한다.
    glm::vec3 worldPosition(std::size_t i) const;

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Practice17(graphics::Application& app) : PracticeMode(app), app_(app) {
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

}

/// 선택한 도형의 모델을 만든다.

graphics::Model Practice17::model(int shape) const {
    if (shape == 0) {
        auto cone = graphics::DiagramSystem::makeCone(.5f, 1.0f);

        for (auto& face : cone.faces)
            face.outline = graphics::OutlineMode::AllTriangleEdges;

        return cone;
    }

    if (shape == 1)
        return graphics::DiagramSystem::makeSphere(.5f);

    return graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube);
}

glm::mat3 Practice17::orbitRotation(std::size_t i) const {
    return turn(objects_[i].orbitDegrees, {0, 1, 0});
}

glm::vec3 Practice17::worldPosition(std::size_t i) const {
    return orbitRotation(i) * objects_[i].position;
}

bool Practice17::initScene() {
    reset();
    std::cout << "Practice 17: 1 left, 2 right, 3 both; x/X,y/Y spin; r/R orbit\n"
              << "a/A local scale, b/B origin scale, d/D left/right, e/E up/down\n"
              << "t straight swap, u bypass swap, v scale/spin/orbit; c shape, s reset, q quit\n";

    return true;
}

void Practice17::reset() {
    auto& scene = app_.getScene();

    scene.clear();
    objects_ = {};
    movement_ = glm::vec2(0);
    motionOffsets_.fill(glm::vec3(0));
    selected_ = 3;
    motion_ = Motion::None;
    running_ = false;
    elapsed_ = 0;

    for (std::size_t i = 0; i < 2; ++i) {
        auto& o = objects_[i];

        o.position = {i == 0 ? -1.0f : 1.0f, 0, 0};
        o.shape = i == 0 ? 0 : 2;
        o.orbit = scene.addGroup().value();
        o.body = scene.addModel(model(o.shape), graphics::Transform3D{}, graphics::PivotMode::Preserve).value();

        if (!scene.setParent(o.body, o.orbit) ||
            !scene.setTint(o.body, i == 0 ? graphics::GlColor{1, .65f, .3f} : graphics::GlColor{.4f, .8f, 1}))
            throw std::runtime_error("Cannot initialize object hierarchy.");
    }
    scene.addLine({-2, 0, 0}, {2, 0, 0}, {1, .2f, .2f});
    scene.addLine({0, -2, 0}, {0, 2, 0}, {.2f, 1, .2f});
    scene.addLine({0, 0, -2}, {0, 0, 2}, {.2f, .4f, 1});

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};

    const auto inverseView = glm::transpose(turn(30, {1, 0, 0}) * turn(30, {0, 1, 0}));

    camera.position = inverseView * glm::vec3(0, 0, 6);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 2.4f;
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().wireframe = true;
    apply();
}

void Practice17::apply() {
    auto& scene = app_.getScene();

    for (std::size_t i = 0; i < 2; ++i) {
        const auto& o = objects_[i];
        graphics::Transform3D orbit, body;

        orbit.rotation = orbitRotation(i);
        body.basis = o.position;
        body.rotation = o.rotation;

        if (o.shape == 0)
            body.rotation *= turn(-45.0f, {0, 0, 1});
        body.scale = glm::vec3(o.scale);

        if (!scene.setTransform(o.orbit, orbit) || !scene.setTransform(o.body, body))
            throw std::runtime_error("Invalid model transform.");
    }
}

void Practice17::begin(Motion motion) {
    if (motion_ == motion) {
        running_ = !running_;

        return;
    }
    motion_ = motion;
    motionOffsets_.fill(glm::vec3(0));
    running_ = true;
    elapsed_ = 0;

    for (std::size_t i = 0; i < 2; ++i) {
        starts_[i] = worldPosition(i);
        scales_[i] = objects_[i].scale;
    }
}

void Practice17::onKey(int key, int action, int mods) {
    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    const float sign = upper ? -1.0f : 1.0f;

    if (key == GLFW_KEY_D || key == GLFW_KEY_E) {
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            float& direction = key == GLFW_KEY_D ? movement_.x : movement_.y;

            direction = action == GLFW_RELEASE ? 0 : (key == GLFW_KEY_D ? -sign : sign);
        }

        return;
    }

    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
        app_.requestClose();

        return;
    }

    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3) {
        selected_ = key - GLFW_KEY_0;

        return;
    }

    if (key == GLFW_KEY_S) {
        reset();

        return;
    }

    if (key == GLFW_KEY_T) {
        begin(Motion::Straight);

        return;
    }

    if (key == GLFW_KEY_U) {
        begin(Motion::Bypass);

        return;
    }

    if (key == GLFW_KEY_V) {
        begin(Motion::Combined);

        return;
    }

    if (key == GLFW_KEY_C) {
        for (auto& o : objects_) {
            o.shape = (o.shape + 1) % 3;

            if (!app_.getScene().setModel(o.body, model(o.shape)))
                throw std::runtime_error("Cannot replace model.");
        }
        apply();

        return;
    }

    for (std::size_t i = 0; i < 2; ++i) {
        if (selected_ != 3 && selected_ != int(i + 1))
            continue;

        auto& o = objects_[i];
        const auto previousPosition = worldPosition(i);
        const auto previousScale = o.scale;

        switch (key) {
        case GLFW_KEY_X:
            o.spin.x = o.spin.x == sign ? 0 : sign;
            break;
        case GLFW_KEY_Y:
            o.spin.y = o.spin.y == sign ? 0 : sign;
            break;
        case GLFW_KEY_R:
            o.orbitDirection = o.orbitDirection == sign ? 0 : sign;
            break;
        case GLFW_KEY_A:
            o.scale = std::clamp(o.scale * (upper ? 1 / 1.1f : 1.1f), .15f, 3.0f);
            break;
        case GLFW_KEY_B: {
            const float old = o.scale;

            o.scale = std::clamp(old * (upper ? 1 / 1.1f : 1.1f), .15f, 3.0f);
            o.position *= o.scale / old;
            break;
        }
        default:
            break;
        }
        if (motion_ == Motion::Straight || motion_ == Motion::Bypass)
            motionOffsets_[i] += worldPosition(i) - previousPosition;
        if (motion_ == Motion::Combined)
            scales_[i] *= o.scale / previousScale;
    }

    apply();
}

void Practice17::onUpdate(float dt) {
    if (running_)
        elapsed_ += dt;

    for (std::size_t i = 0; i < 2; ++i) {
        auto& o = objects_[i];

        if (selected_ == 3 || selected_ == int(i + 1)) {
            const auto delta = glm::vec3(movement_ * .7f * dt, 0);
            o.position += glm::transpose(orbitRotation(i)) * delta;
            if (motion_ == Motion::Straight || motion_ == Motion::Bypass)
                motionOffsets_[i] += delta;
        }

        const bool combined = motion_ == Motion::Combined;
        const auto spin = o.spin + (combined && running_ ? glm::vec2(0, 1) : glm::vec2(0));

        const float orbit = o.orbitDirection + (combined && running_ ? 1.0f : 0.0f);

        o.rotation = o.rotation * turn(45 * dt * spin.x, {1, 0, 0}) * turn(45 * dt * spin.y, {0, 1, 0});

        const auto x = glm::normalize(o.rotation[0]), z = glm::normalize(glm::cross(x, o.rotation[1]));

        o.rotation = glm::mat3(x, glm::cross(z, x), z);
        o.orbitDegrees = std::fmod(o.orbitDegrees + 30 * dt * orbit, 360.0f);

        if (running_ && combined)
            o.scale = scales_[i] * (1 + (i == 0 ? .35f : -.35f) * std::sin(elapsed_));
        else if (running_ && (motion_ == Motion::Straight || motion_ == Motion::Bypass)) {
            const float t = std::min(elapsed_ / 2.0f, 1.0f);
            auto point = glm::mix(starts_[i], starts_[1 - i], t) + motionOffsets_[i];

            if (motion_ == Motion::Bypass)
                point.y += (i == 0 ? 1.0f : -1.0f) * .9f * std::sin(3.14159265f * t);
            o.position = glm::transpose(orbitRotation(i)) * point;
        }
    }

    if (running_ && (motion_ == Motion::Straight || motion_ == Motion::Bypass) && elapsed_ >= 2) {
        motion_ = Motion::None;
        running_ = false;
    }
    apply();
}

void Practice17::onFocus(bool focused) {
    if (!focused)
        movement_ = glm::vec2(0);
}
}

std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app) {
    return std::make_unique<Practice17>(app);
}
