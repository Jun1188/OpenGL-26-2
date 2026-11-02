#pragma once
#include "framework/Framework.h"
#include <array>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_=0,sun_=0;
    std::array<graphics::EntityId,3> tracks_{},planets_{},moonFrames_{},moonTracks_{},moons_{};
    std::array<bool,GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 position_{0};
    std::array<float,3> planetAngle_{0,120,240},moonAngle_{0,120,240};
    float radius_=1,targetRadius_=1,sunTime_=0,moonTilt_=0,moonSpin_=0;
    bool pulse_=false,rotateMoons_=false;
    /// 계산한 변환을 장면에 반영한다.
    void apply();
public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app):app_(app){}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key,int action,int mods);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused);
};
