#pragma once
#include "framework/Framework.h"

class Console {
    graphics::Application& app_;
    graphics::SceneAuthoring scene_;
    double seconds_ = 0;
    bool paused_ = false;
public:
    explicit Console(graphics::Application& app) : app_(app), scene_(app) {}
    bool initScene();
    void onKey(int key, int action, int mods = 0);
    void onMouse(const graphics::MouseEvent& event);
    void onUpdate(float dt);
    bool onRender();
};
