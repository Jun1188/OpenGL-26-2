#pragma once
#include "framework/Framework.h"
#include <array>

class Console {
    struct Object {
        graphics::EntityId orbit=0,body=0;
        glm::vec3 position{0};
        glm::mat3 rotation{1};
        glm::vec2 spin{0};
        float orbitDegrees=0,orbitDirection=0,scale=.65f;
        int shape=0;
    };
    enum class Motion { None, Straight, Bypass, Combined };
    graphics::Application& app_;
    std::array<Object,2> objects_{};
    int selected_=3;
    glm::vec2 movement_{0};
    Motion motion_=Motion::None;
    bool running_=false;
    float elapsed_=0;
    std::array<glm::vec3,2> starts_{};
    std::array<float,2> scales_{};
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
