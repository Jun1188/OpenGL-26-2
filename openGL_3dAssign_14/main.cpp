#include "Console.h"
#include <iostream>


int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 14";
    application.setInitHandler([&](graphics::Application&) {
        return practice.initScene();
    });
    application.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
        practice.onKey(key, action, mods);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        practice.onUpdate(deltaTime);
    });
    application.setFocusHandler([&](graphics::Application&, bool focused) {
        practice.onFocus(focused);
    });
    return application.run(config);
}
