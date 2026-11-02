#pragma once
#include "framework/Framework.h"
class Console {
    graphics::Application& app_;
    graphics::CharacterBody body_;
    graphics::JointRig rig_;
    graphics::EntityId stage_=0;
    std::vector<graphics::Bounds3> obstacles_;
    graphics::Bounds3 bounds_{{-5,0,-5},{5,4,5}};
    std::array<float,4> doors_{};
    std::array<bool,4> open_{};
    std::optional<std::size_t> activeWall_;
    unsigned randomSeed_=std::random_device{}();
    std::mt19937 random_{randomSeed_};
    bool walking_=true;
public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app):app_(app) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key,int action,int mods);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
};
