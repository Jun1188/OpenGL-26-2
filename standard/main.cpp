#include "Console.h"
#include <exception>
#include <iostream>

int main() try {
    SetConsoleOutputCP(CP_UTF8);
    graphics::Application app;
    Console sample(app);
    app.setInitHandler([&](graphics::Application&) { return sample.initScene(); });
    app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) { sample.onKey(key, action, mods); });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) { sample.onMouse(event); });
    app.setUpdateHandler([&](graphics::Application&, float dt) { sample.onUpdate(dt); });
    app.setRenderHandler([&](graphics::Application&) { return sample.onRender(); });
    graphics::ApplicationConfig config;
    config.title = "Standard - Direct C++ source";
    return app.run(config);
} catch (const std::exception& error) {
    std::cerr << "Standard 실행 실패: " << error.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "Standard 실행 실패: 프레임워크 진단을 확인하세요.\n";
    return 1;
}
