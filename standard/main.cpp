#include "Console.h"

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    graphics::Application app;
    // nullopt: 숫자 키로 전환. 1~9: 지정한 모드에 고정해서 시작.
    constexpr std::optional<int> initialMode = std::nullopt;
    static_assert(!initialMode || (*initialMode >= 1 && *initialMode <= 9), "initialMode must be 1..9");
    Console sample(app, initialMode);

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setKeyHandler([&](graphics::Application&, int key, int action) {
        sample.onKey(key, action);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) {
        sample.onUpdate(dt);
    });
    app.setRenderHandler([&](graphics::Application&) {
        return sample.onRender();
    });

    graphics::ApplicationConfig config;

    config.title = "standard - Motion, Grid, diagnostics and render passes";

    return app.run(config);
}
