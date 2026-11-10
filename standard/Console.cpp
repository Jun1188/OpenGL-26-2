#include "Console.h"
#include "SceneSetup.h"
#include <iostream>

bool Console::initScene() {
    scene_.clear();
    configureScene(scene_, app_.getRenderOptions());
    scene_.update(0);
    std::cout << "Standard: SceneSetup.cpp를 직접 편집하세요.\n"
                 "별도 debugger 프로젝트가 현재 소스의 경로/행/수정 전후 코드를 안내합니다.\n"
                 "클릭: 객체 이름 확인 | Space: 시간 정지 | Home: 시간 초기화 | Esc: 종료\n";
    return true;
}
void Console::onKey(int key, int action, int) {
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) app_.requestClose();
    if (key == GLFW_KEY_SPACE) paused_ = !paused_;
    if (key == GLFW_KEY_HOME) { seconds_ = 0; scene_.update(0); }
}
void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
    const auto hit = app_.pickAt(event.windowX, event.windowY);
    if (!hit) { std::cout << "선택: 없음\n"; return; }
    const auto name = scene_.name(hit->entityId);
    if (!name.empty()) std::cout << "선택: " << name << " | debugger 명령: show object " << name << '\n';
}
void Console::onUpdate(float dt) {
    if (!paused_) seconds_ += dt;
    scene_.update(seconds_);
}
bool Console::onRender() { return app_.renderScene(); }
