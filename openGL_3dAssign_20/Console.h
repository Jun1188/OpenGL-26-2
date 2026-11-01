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
    void placeMovingObjects();
    void resetState();
    void zoom(float factor);

public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action, int mods);
    void onFocus(bool focused);
    void onUpdate(float dt);
};
