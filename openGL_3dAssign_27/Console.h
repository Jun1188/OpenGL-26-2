#pragma once
#include "framework/Framework.h"

class Console {
    graphics::Application& app_;
    graphics::EntityId lamp_ = 0;
    std::size_t color_ = 0;
    double angle_ = 0;
    int direction_ = 0;
    void apply();
public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action, int mods);
    void onUpdate(float dt);
};
