#pragma once

#include "framework/include/Application.h"
#include <glfw3.h>
#include <array>

// 실습 14: 도형 선택, 연속 입력, 위치와 로컬 회전행렬을 관리한다.
class Console {
private:
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{ 0.0f };
    glm::mat3 rotation_{ 1.0f };
    glm::vec2 angularDirection_{ 0.0f };

    void selectShape(bool pyramid);
    void resetMotion();
    void applyTransform();

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    void onKey(int key, int action, int mods);
    void onUpdate(float deltaTime);
    void onFocus(bool focused);
};
