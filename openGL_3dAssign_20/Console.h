#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::AnimationClock clock_;
    std::array<graphics::EntityId, 5> boxes_{};
    std::vector<graphics::EntityId> stripes_;
    bool leftHeld_ = false;
    bool rightHeld_ = false;
    /// 이동 거리로 상자와 벨트 표시를 배치한다.
    void placeMovingObjects();
    /// 이동 시간과 카메라 상태를 초기화한다.
    void resetState();
    /// 카메라 배율을 변경한다.
    void zoom(float factor);

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app) : app_(app) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods);
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
};
