#pragma once

#include "framework/include/Application.h"
#include <array>
#include <random>

// 실습 15: 전체 자전과 하나의 면 동작을 분리한다.
class Console {
private:
    enum class Motion { None, Top, Front, Side, Back, Hinge, Swap, OrbitY, OrbitX };
    struct FaceMotion {
        Motion kind = Motion::None;
        bool running = false;
        float progress = 0.0f; // 회전은 도, 열기·축소는 0~1이다.
        float direction = 1.0f;
        std::array<std::size_t, 2> pair{ 0, 3 };
        glm::vec3 hinge{ 0.4f, 0.0f, 0.4f };
    };

    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    bool spinning_ = false;
    float spinDegrees_ = 0.0f;
    FaceMotion motion_;
    std::mt19937 random_{ std::random_device{}() };

    void selectShape(bool pyramid);
    void resetFaces();
    void resetAll();
    void beginMotion(Motion kind);
    void applyObjectTransform();
    void applyFaceMotion();

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    void onKey(int key, int action);
    void onUpdate(float deltaTime);
};
