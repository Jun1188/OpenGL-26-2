#include "Console.h"

int main() {
    graphics::Application app;
    Console sample(app);

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setKeyHandler([&](graphics::Application&, int key, int action) {
        sample.onKey(key, action);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt) {sample.onUpdate(dt);});
    app.setRenderHandler([&](graphics::Application&) {return sample.onRender();});

    graphics::ApplicationConfig config;
    config.title = "standard - Motion, Grid, diagnostics and render passes";
    return app.run(config);
}
