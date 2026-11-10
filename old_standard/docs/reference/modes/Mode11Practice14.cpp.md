# 과거 구현 참조: Mode11Practice14.cpp

- 원래 위치: `standard/modes/Mode11Practice14.cpp`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <iostream>

namespace {
// 실습 14: 도형 선택, 연속 입력, 위치와 로컬 회전행렬을 관리한다.
class Practice14 final : public PracticeMode {
private:
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0.0f};
    glm::mat3 rotation_{1.0f};
    glm::vec2 angularDirection_{0.0f};

    void selectShape(bool pyramid);
    void resetMotion();
    void applyTransform();

public:
    explicit Practice14(graphics::Application& application) : PracticeMode(application), application_(application) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float deltaTime) override;
    void onFocus(bool focused) override;
};

using practice_common::assignVertexColors;

namespace {

// 부호 조합은 육면체 8개 꼭짓점을 구분한다. 사각뿔은 바닥 4개와 꼭짓점 색이 다르다.


} // namespace

bool Practice14::initScene() {
    auto& scene = application_.getScene();

    scene.clear();

    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(0.8f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(0.8f));

    assignVertexColors(cube);

    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);

    if (!cubeId || !pyramidId) {
        return false;
    }
    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;

    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();
    const auto inverseView = glm::transpose(glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0})));

    camera.position = inverseView * glm::vec3(0, 0, 4);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetMotion();
    selectShape(false);
    std::cout << "실습 14 명령어: C== cube, P== pyramid, H== depth test\n"
              << "w== wireframe / W== solid (Shift or Caps Lock)\n"
              << "x/X: local X +/-, y/Y: local Y +/-\n"
              << "이동: world X/Y translation, S: reset motion, Esc: quit\n";

    return true;
}

void Practice14::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;

    auto& scene = application_.getScene();

    for (auto id : scene.getIds()) {
        const bool visible = id == (pyramidSelected_ ? pyramidId_ : cuboidId_);

        for (std::size_t i = 0; i < scene.getComponent(id)->polyhedron.faces.size(); ++i) {
            scene.setFaceVisible(id, i, visible);
        }
    }
    applyTransform();
}

void Practice14::resetMotion() {
    position_ = glm::vec3(0.0f);

    rotation_ = glm::mat3(1);
    angularDirection_ = glm::vec2(0.0f);
    pressed_.fill(false);
    applyTransform();
}

void Practice14::applyTransform() {
    graphics::Transform3D transform;

    transform.basis = position_;
    transform.rotation = rotation_;
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void Practice14::onKey(int key, int action, int mods) {
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            pressed_[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressed_[key] = false;
        }
    }

    if (action != GLFW_PRESS) {
        return;
    }

    const bool uppercase = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    auto& options = application_.getRenderOptions();

    switch (key) {
    case GLFW_KEY_ESCAPE:
        application_.requestClose();
        break;
    case GLFW_KEY_C:
        selectShape(false);
        break;
    case GLFW_KEY_P:
        selectShape(true);
        break;
    case GLFW_KEY_H:
        options.depthTest = !options.depthTest;
        break;
    case GLFW_KEY_W:
        options.wireframe = !uppercase;
        break;
    case GLFW_KEY_X:
        angularDirection_.x = uppercase ? -1.0f : 1.0f;
        break;
    case GLFW_KEY_Y:
        angularDirection_.y = uppercase ? -1.0f : 1.0f;
        break;
    case GLFW_KEY_S:
        resetMotion();
        break;
    default:
        break;
    }
}

void Practice14::onUpdate(float deltaTime) {
    const glm::vec2 movement{static_cast<float>(pressed_[GLFW_KEY_RIGHT]) - static_cast<float>(pressed_[GLFW_KEY_LEFT]),
                             static_cast<float>(pressed_[GLFW_KEY_UP]) - static_cast<float>(pressed_[GLFW_KEY_DOWN])};

    if (glm::dot(movement, movement) > 0.0f) {
        position_ += glm::vec3(glm::normalize(movement) * (0.5f * deltaTime), 0.0f);
    }
    // 오른쪽 곱은 현재 객체의 로컬 축 회전이다. 위치는 별도로 보존한다.
    const float angle = glm::radians(45.0f * deltaTime);

    rotation_ = glm::mat3(glm::rotate(glm::rotate(glm::mat4(rotation_), angle * angularDirection_.x, {1, 0, 0}),
                                      angle * angularDirection_.y, {0, 1, 0}));

    const auto x = glm::normalize(rotation_[0]);
    const auto z = glm::normalize(glm::cross(x, rotation_[1]));

    rotation_ = glm::mat3(x, glm::cross(z, x), z);
    applyTransform();
}

void Practice14::onFocus(bool focused) {
    if (!focused) {
        pressed_.fill(false); // 포커스 밖에서 놓친 release가 이동을 계속시키지 않는다.
    }
}
}

std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app) {
    return std::make_unique<Practice14>(app);
}
````
