#pragma once
#include "../../modes/Assembly.h"
#include <cmath>

// Reference adapter: call from customCommand; not a Standard mode or an automatic startup scene.
inline void addMovingBeacon(assembly::Assembly& workspace, const std::string& name) {
    auto& scene = workspace.application().getScene();
    const auto parent = [&](const std::string& childName, const std::string& parentName) {
        auto& child = workspace.node(childName);
        assembly::require(scene.setParentPreserveWorld(child.id, workspace.node(parentName).id), "Beacon parent failed.");
        child.attachment = scene.getComponent(child.id)->attachment;
    };
    workspace.add("joint", name, {0, 1, 0});
    workspace.add("box", name + ".rail", {0, 1, 0}, {8, .12f, .12f});
    parent(name + ".rail", name);
    workspace.add("sphere", name + ".probe", {-3, 1.4f, 0}, {.5f, .5f, .5f});
    parent(name + ".probe", name);
    const auto frame = workspace.node(name).id;
    const auto probe = workspace.node(name + ".probe").id;

    assembly::Behavior behavior;
    behavior.name = name + ".scan";
    behavior.target = name + ".probe";
    behavior.effect = assembly::Effect::Custom;
    behavior.positionDriver = true;
    behavior.custom = [frame, probe](graphics::Scene& scene, const graphics::Transform3D&, double seconds, float) {
        const auto parent = scene.worldMatrix(frame);
        if (!parent) return false;
        const float x = float(-3 * std::cos(seconds * 2 * std::acos(-1.0) / 4));
        const auto local = glm::translate(glm::mat4(1), glm::vec3(x, .4f, 0));
        return scene.setWorldPlacement(probe, *parent * local);
    };
    workspace.attach(std::move(behavior));
    assembly::Behavior spin;
    spin.name = name + ".turn";
    spin.target = name;
    graphics::RotationMotion rotation;
    rotation.axis.direction = {0, 1, 0};
    rotation.degreesPerSecond = 25;
    rotation.space = graphics::TransformSpace::Local;
    assembly::require(spin.motion.append(graphics::Motion::rotation(rotation)), "Beacon rotation failed.");
    workspace.attach(std::move(spin));
    workspace.add("box", name + ".lamp", {-3, 2, 0}, {.2f, .2f, .2f});
    parent(name + ".lamp", name + ".probe");
    auto& lamp = workspace.node(name + ".lamp");
    lamp.kind = "light";
    lamp.light = graphics::PointLight{};
    lamp.light->color = {.3f, .7f, 1};
    lamp.light->intensity = 2;
    assembly::require(scene.setTint(*lamp.mesh, lamp.light->color), "Beacon lamp tint failed.");
    workspace.add("box", name + ".view", {-3, 2, 4}, {.2f, .2f, .2f});
    parent(name + ".view", name + ".probe");
    auto& view = workspace.node(name + ".view");
    view.kind = "camera";
    view.camera = graphics::Camera{};
    view.camera->position = {0, 0, 0};
    view.camera->target = {0, -.6f, -4};
    for (auto id : {*lamp.mesh, *view.mesh}) {
        const auto count = scene.getComponent(id)->polyhedron.faces.size();
        for (std::size_t face = 0; face < count; ++face) {
            auto material = scene.getMesh(id, face)->material;
            material.unlit = true;
            assembly::require(scene.setMaterial(id, face, material), "Beacon marker material failed.");
        }
    }
    workspace.application().getRenderOptions().lighting = true;
}
