#pragma once
#include "../framework/Framework.h"

// Native C++ references. Call one function from configureScene after scene.clear().
namespace source_examples {
inline void rotatingDisplay(graphics::SceneAuthoring& scene, graphics::RenderOptions& options) {
    graphics::ObjectSetup support;
    support.size = {0.4f, 3.f, 0.4f};
    support.position = {1.f, 0.f, 0.f};
    support.material.color = {0.2f, 0.4f, 0.7f};
    scene.addObject("support", support);

    graphics::ObjectSetup display;
    display.size = {2.f, 1.f, 0.2f};
    display.position = {2.f, 1.f, 0.f}; // World-authored, even with a parent.
    display.scale = {1.5f, 1.f, 0.5f};
    display.material.color = {0.9f, 0.6f, 0.2f};
    scene.addObject("display", display, "support");

    graphics::LayerSetup parentTurn;
    parentTurn.spinDegreesPerSecond = 20;
    scene.addLayer("support", "turn", parentTurn);
    graphics::LayerSetup hinge;
    hinge.pivot = {-1.f, 0.f, 0.f};
    hinge.spinAxis = {0.f, 0.f, 1.f};
    hinge.spinDegreesPerSecond = 45;
    scene.addLayer("display", "hinge", hinge);
    graphics::LayerSetup shift;
    shift.translation = {0.5f, 0.f, 0.f};
    scene.addLayer("display", "shift", shift);

    options.lighting = true;
    options.ambientColor = {0.2f, 0.2f, 0.2f};
    options.directionalIntensity = 0;
    graphics::PointLight key;
    key.position = {3.f, 5.f, 6.f};
    key.intensity = 2;
    scene.addLight("key", key);
    graphics::Camera camera;
    camera.position = {8.f, 5.f, 10.f};
    camera.target = {1.f, 0.f, 0.f};
    scene.setCamera(camera);
}

inline void attachedLightAndCamera(graphics::SceneAuthoring& scene, graphics::RenderOptions& options) {
    graphics::ObjectSetup carrier;
    carrier.position = {0.f, 0.f, 0.f};
    carrier.size = {1.f, 0.2f, 1.f};
    scene.addObject("carrier", carrier);
    graphics::LayerSetup turn;
    turn.spinDegreesPerSecond = 25;
    scene.addLayer("carrier", "orbit", turn);

    graphics::ObjectSetup sculpture;
    sculpture.shape = graphics::GLShape::Pyramid;
    sculpture.position = {0.f, 0.8f, 0.f};
    sculpture.material.color = {0.5f, 0.7f, 0.9f};
    sculpture.material.specularStrength = 0.8f;
    sculpture.material.shininess = 128;
    scene.addObject("sculpture", sculpture);

    options.lighting = true;
    options.ambientColor = {0.1f, 0.1f, 0.1f};
    options.directionalIntensity = 0;
    graphics::PointLight lamp;
    lamp.position = {3.f, 3.f, 0.f}; // Carrier-local position.
    lamp.color = {1.f, 0.8f, 0.5f};
    lamp.intensity = 2;
    scene.addLight("lamp", lamp, "carrier");
    graphics::Camera camera;
    camera.position = {6.f, 4.f, 0.f}; // Position, target and up are carrier-local.
    camera.target = {0.f, 0.8f, 0.f};
    scene.setCamera(camera, "carrier");
}
} // namespace source_examples
