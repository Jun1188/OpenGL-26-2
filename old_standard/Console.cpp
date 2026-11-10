#include "Console.h"
#include <iostream>

bool Console::initScene() {
    if (!assembly::defaultRecipe(workspace_, std::cout)) return false;
    workspace_.help(std::cout);
    input_.prompt();
    return true;
}
void Console::onKey(int key, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_F12) {
        workspace_.help(std::cout);
        return;
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_F5 && (mods & GLFW_MOD_CONTROL)) {
        workspace_.execute("reload", std::cout);
        return;
    }
    workspace_.onKey(key, action, mods, std::cout);
}
void Console::onMouse(const graphics::MouseEvent& event) { workspace_.onMouse(event, std::cout); }
void Console::onCursor(double x, double y) { workspace_.onCursor(x, y, std::cout); }
void Console::onFocus(bool focused) { workspace_.onFocus(focused); }
void Console::onUpdate(float dt) {
    for (unsigned i = 0; i < 8; ++i) {
        const auto line = input_.poll();
        if (!line) break;
        if (*line == "quit") { app_.requestClose(); return; }
        workspace_.execute(*line, std::cout);
        input_.prompt();
    }
    workspace_.update(dt, std::cout);
}
bool Console::onRender() { return workspace_.render(); }
