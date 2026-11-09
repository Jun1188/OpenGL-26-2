#pragma once
#include "framework/Framework.h"
#include <array>

class Console {
    graphics::Application& app_;
    std::array<graphics::EntityId, 2> objects_{};
    graphics::EntityId lamp_ = 0, ring_ = 0;
    std::size_t shape_ = 0;
    double objectAngle_ = 0, lightAngle_ = 0;
    float radius_ = 3;
    bool spinning_ = false;
    int orbitDirection_ = 0;
    void apply();
public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action, int mods);
    void onUpdate(float dt);
};
