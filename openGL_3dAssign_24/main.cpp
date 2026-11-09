#include "Console.h"
#include <iostream>
#include <Windows.h>

int main() {
    SetConsoleOutputCP(CP_UTF8);
    try {
        graphics::Application app;
        Console practice(app);
        app.setInitHandler([&](graphics::Application&) { return practice.initScene(); });
        app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
            practice.onKey(key, action, mods);
        });
        app.setFocusHandler([&](graphics::Application&, bool focused) { practice.onFocus(focused); });
        app.setUpdateHandler([&](graphics::Application&, float dt) { practice.onUpdate(dt); });
        graphics::ApplicationConfig config;
        config.title = "실습 24 - 물류 센터와 트럭";
        config.width = 1100;
        config.height = 800;
        return app.run(config);
    } catch (const std::exception& error) {
        std::cerr << "실습 24 실패: " << error.what() << '\n';
        return 1;
    }
}
