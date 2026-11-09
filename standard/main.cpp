#include "Console.h"
#include <exception>
#include <iostream>

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() try {
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
    app.setRenderHandler([&](graphics::Application&) {
        return sample.onRender();
    });

    graphics::ApplicationConfig config;

    config.title = "Standard - Scene workspace (mode 0)";

    return app.run(config);
} catch (const std::exception& error) {
    std::cerr << "standard 실행 실패: " << error.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "standard 실행 실패: 앞선 프레임워크 진단을 확인하세요.\n";
    return 1;
}
