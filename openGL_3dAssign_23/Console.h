#pragma once
#include "framework/Framework.h"
class Console {
    graphics::Application& app_;
    graphics::CharacterBody body_;
    graphics::JointRig rig_;
    graphics::EntityId stage_=0;
    std::vector<graphics::Bounds3> obstacles_;
    graphics::Bounds3 bounds_{{-5,0,-5},{5,4,5}};
    float door_=0;
    bool open_=false,walking_=true;
public:
    explicit Console(graphics::Application& app):app_(app) {}
    bool initScene();
    void onKey(int key,int action,int mods);
    void onUpdate(float dt);
};
