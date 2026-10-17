#include "Console.h"
#include <iostream>

int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 16";
    application.setInitHandler([&](graphics::Application&) {
        return practice.initScene();
    });
    application.setKeyHandler([&](graphics::Application&, int key, int action) {
        practice.onKey(key, action);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        practice.onUpdate(deltaTime);
    });
    return application.run(config);
}
