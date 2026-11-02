#include "Console.h"
#include <exception>
#include <iostream>

int main() {
    try {
        graphics::Application app;
        Console sample(app);

        app.setInitHandler([&](graphics::Application&) {
            return sample.initScene();
        });
        app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
            sample.onKey(key, action, mods);
        });
        app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
            sample.onMouse(event);
        });
        app.setCursorHandler([&](graphics::Application&, double x, double y) {
            sample.onCursor(x, y);
        });
        app.setFocusHandler([&](graphics::Application&, bool focused) {
            sample.onFocus(focused);
        });
        app.setUpdateHandler([&](graphics::Application&, float dt) {
            sample.onUpdate(dt);
        });

        graphics::ApplicationConfig config;

        config.title = "Practice 21 - Bouncing balls and rotating room";
        config.width = 1100;
        config.height = 800;
        config.diagnostics = {true, false};

        return app.run(config);
    } catch (const std::exception& error) {
        std::cerr << "실습 21 종료 원인: " << error.what() << '\n';
    } catch (...) {
        std::cerr << "실습 21 종료 원인: 표준 예외가 아닌 예외. 앞선 프레임워크 진단을 확인하세요.\n";
    }

    return 1;
}
