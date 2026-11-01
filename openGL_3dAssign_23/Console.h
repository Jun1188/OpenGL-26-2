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
    unsigned randomSeed_=std::random_device{}();
    std::mt19937 random_{randomSeed_};
    bool walking_=true;
public:
    explicit Console(graphics::Application& app):app_(app) {}
    bool initScene();
    void onKey(int key,int action,int mods);
    void onUpdate(float dt);
};
