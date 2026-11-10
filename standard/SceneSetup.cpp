#include "SceneSetup.h"

void configureScene(graphics::SceneAuthoring& scene, graphics::RenderOptions& options) {
    // @debug begin render main
    auto& cfg_render = options;
    cfg_render.lighting = true;
    cfg_render.textures = false;
    cfg_render.ambientColor = {0.15f, 0.15f, 0.15f};
    cfg_render.lightDirection = {1.f, 1.f, 1.f};
    cfg_render.lightColor = {1.f, 1.f, 1.f};
    cfg_render.directionalIntensity = 0.f;
    cfg_render.background = {0.08f, 0.10f, 0.14f, 1.f};
    // @debug end render main

    // @debug begin object base
    graphics::ObjectSetup cfg_base;
    cfg_base.shape = graphics::GLShape::Cube;
    cfg_base.size = {2.f, 0.5f, 2.f};
    cfg_base.position = {0.f, 0.f, 0.f};
    cfg_base.rotationDegrees = {0.f, 0.f, 0.f};
    cfg_base.scale = {1.f, 1.f, 1.f};
    cfg_base.pivot = {0.f, 0.f, 0.f};
    cfg_base.material.color = {0.3f, 0.5f, 0.8f};
    cfg_base.material.ambientStrength = 0.25f;
    cfg_base.material.diffuseStrength = 1.f;
    cfg_base.material.specularStrength = 0.4f;
    cfg_base.material.shininess = 32.f;
    cfg_base.material.unlit = false;
    scene.addObject("base", cfg_base);
    // @debug end object base

    // @debug begin object panel
    graphics::ObjectSetup cfg_panel;
    cfg_panel.shape = graphics::GLShape::Cube;
    cfg_panel.size = {2.f, 1.f, 0.2f};
    cfg_panel.position = {0.f, 1.f, 0.f};
    cfg_panel.rotationDegrees = {0.f, 0.f, 0.f};
    cfg_panel.scale = {1.f, 1.f, 1.f};
    cfg_panel.pivot = {0.f, 0.f, 0.f};
    cfg_panel.material.color = {0.9f, 0.5f, 0.2f};
    cfg_panel.material.ambientStrength = 0.25f;
    cfg_panel.material.diffuseStrength = 1.f;
    cfg_panel.material.specularStrength = 0.6f;
    cfg_panel.material.shininess = 64.f;
    cfg_panel.material.unlit = false;
    scene.addObject("panel", cfg_panel, "base");
    // @debug end object panel

    // @debug begin layer hinge
    graphics::LayerSetup cfg_hinge;
    cfg_hinge.translation = {0.f, 0.f, 0.f};
    cfg_hinge.rotationDegrees = {0.f, 0.f, 0.f};
    cfg_hinge.scale = {1.f, 1.f, 1.f};
    cfg_hinge.pivot = {-1.f, 0.f, 0.f};
    cfg_hinge.spinAxis = {0.f, 1.f, 0.f};
    cfg_hinge.spinDegreesPerSecond = 0;
    scene.addLayer("panel", "hinge", cfg_hinge);
    // @debug end layer hinge

    // @debug begin light key
    graphics::PointLight cfg_key;
    cfg_key.position = {2.f, 4.f, 3.f};
    cfg_key.color = {1.f, 0.95f, 0.85f};
    cfg_key.intensity = 1.5f;
    cfg_key.attenuation = 0.02f;
    scene.addLight("key", cfg_key);
    // @debug end light key

    // @debug begin camera main
    graphics::Camera cfg_main;
    cfg_main.position = {6.f, 4.f, 8.f};
    cfg_main.target = {0.f, 0.5f, 0.f};
    cfg_main.up = {0.f, 1.f, 0.f};
    cfg_main.fieldOfViewDegrees = 45.f;
    cfg_main.nearPlane = 0.1f;
    cfg_main.farPlane = 100.f;
    cfg_main.orthographic = false;
    cfg_main.orthographicHalfExtent = 4.f;
    scene.setCamera(cfg_main);
    // @debug end camera main

    // @debug insert
}
