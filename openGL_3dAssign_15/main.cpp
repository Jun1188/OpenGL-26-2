#include "Console.h"
#include <iostream>

int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 15";
    config.shaderDirectory = graphics::Application::findShaderDirectory();
    if (config.shaderDirectory.empty()) {
        std::cerr << "Shaders were not found beside the executable or in framework/shaders.\n";
        return -1;
    }
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
