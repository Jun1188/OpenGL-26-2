#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_=0,central_=0;
    std::array<graphics::EntityId,2> lower_{},upper_{},guns_{},poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool,GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_=0,centralAngle_=0,gunAngle_=0,poleTime_=0;
    float swapProgress_=0,swapDirection_=1;
    float cameraX_=0,cameraZ_=0,cameraYaw_=0,cameraOrbit_=0;
    bool lowerRunning_=false,centralRunning_=false,gunRunning_=false,poleRunning_=false;
    bool swapRunning_=false,swapStarted_=false;
    unsigned colorVersion_=0;
    /// 부모에 연결한 상자를 만든다.
    graphics::EntityId box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent);
    /// 배치와 조작 상태를 초기화한다.
    void reset();
    /// 계산한 변환을 장면에 반영한다.
    void apply();
    /// 물체 색을 다시 지정한다.
    void recolor();
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
    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
};
