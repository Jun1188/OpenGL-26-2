#pragma once
#include "modes/Assembly.h"
#include "modes/AssemblyConsole.h"

// Standard owns the editor; reference practices do not participate in its runtime.
class Console {
    graphics::Application& app_;
    assembly::Assembly workspace_;
    assembly::ConsoleInput input_;
public:
    explicit Console(graphics::Application& app) : app_(app), workspace_(app) {}
    bool initScene();
    void onKey(int key, int action, int mods = 0);
    void onMouse(const graphics::MouseEvent& event);
    void onCursor(double x, double y);
    void onFocus(bool focused);
    void onUpdate(float dt);
    bool onRender();
};
