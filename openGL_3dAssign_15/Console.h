#pragma once

#include "framework/include/Application.h"
#include <array>
#include <random>

class Console {
private:
    enum class Motion { None, Top, Front, Side, Back, Hinge, Swap, OrbitY, OrbitX, Count };
    struct FaceMotion {
        bool initialized = false;
        bool running = false;
        float progress = 0.0f;
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
    std::array<FaceMotion, static_cast<std::size_t>(Motion::Count)> motions_{};
    std::mt19937 random_{ std::random_device{}() };

    /// 조작할 도형을 선택한다.
    void selectShape(bool pyramid);
    /// 면별 애니메이션을 초기화한다.
    void resetFaces();
    /// 전체 조작 상태를 초기화한다.
    void resetAll();
    /// 선택한 면 애니메이션을 시작한다.
    void beginMotion(Motion kind);
    /// 물체 변환을 장면에 반영한다.
    void applyObjectTransform();
    /// 면별 변환을 장면에 반영한다.
    void applyFaceMotion();

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& application) : application_(application) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float deltaTime);
};
