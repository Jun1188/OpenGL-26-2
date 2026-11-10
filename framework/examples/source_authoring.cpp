#include "../Framework.h"
#include <iostream>
#include <exception>

// Compile as an independent entrypoint with this folder's Framework.props.
int main() try {
    graphics::Application app;
    graphics::SceneAuthoring scene(app);
    double seconds = 0;
    app.setInitHandler([&](graphics::Application&) {
        scene.clear();
        graphics::ObjectSetup object;
        object.position = {0.f, 0.f, 0.f};
        object.scale = {2.f, 1.f, 0.5f};
        object.material.color = {0.3f, 0.6f, 0.9f};
        object.material.specularStrength = 0.7f;
        object.material.shininess = 64;
        scene.addObject("sample", object);
        graphics::LayerSetup spin;
        spin.spinDegreesPerSecond = 30;
        scene.addLayer("sample", "spin", spin);
        graphics::PointLight key;
        key.position = {3.f, 4.f, 5.f};
        key.intensity = 2;
        scene.addLight("key", key);
        graphics::Camera camera;
        scene.setCamera(camera);
        auto& render = app.getRenderOptions();
        render.lighting = true;
        render.directionalIntensity = 0;
        render.ambientColor = {0.15f, 0.15f, 0.15f};
        scene.update(0);
        return true;
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) { seconds += dt; scene.update(seconds); });
    graphics::ApplicationConfig config;
    config.title = "Native source authoring example";
    return app.run(config);
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n'; return 1;
}
