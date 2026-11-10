#include "Console.h"
#include "SceneSetup.h"
#include <iostream>

bool Console::initScene() {
    scene_.clear();
    configureScene(scene_, app_.getRenderOptions());
    scene_.update(0);
    if (enableExamController_) controller_.initialize();
    return true;
}
void Console::onKey(int key, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) app_.requestClose();
    if (action == GLFW_PRESS && key == GLFW_KEY_SPACE) paused_ = !paused_;
    if (action == GLFW_PRESS && key == GLFW_KEY_HOME) { seconds_ = 0; if (enableExamController_) controller_.reset(); scene_.update(0); }
    if (enableExamController_) controller_.onKey(key, action, mods);
}
void Console::onFocus(bool focused) { if (enableExamController_) controller_.onFocus(focused); }
void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
    const auto hit = app_.pickAt(event.windowX, event.windowY);
    if (!hit) { std::cout << "선택: 없음\n"; return; }
    const auto name = scene_.name(hit->entityId);
    if (!name.empty()) std::cout << "선택: " << name << " | debugger 명령: show object " << name << '\n';
}
void Console::onUpdate(float dt) {
    const float step = paused_ ? 0.f : dt;
    seconds_ += step;
    if (enableExamController_) controller_.update(step);
    scene_.update(seconds_);
}
bool Console::onRender() {
    scene_.syncTools(); // Application has already advanced native Scene motions.
    if (enableExamController_) controller_.syncVisuals();
    return app_.renderScene();
}
