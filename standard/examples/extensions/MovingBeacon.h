#pragma once
#include "../../modes/Assembly.h"
#include <cmath>
#include <sstream>

// Reference adapter: call from customCommand; not a Standard mode or an automatic startup scene.
inline void addMovingBeacon(assembly::Assembly& workspace, const std::string& name) {
    std::ostringstream output;
    const auto command = [&](const std::string& line) {
        assembly::require(workspace.execute(line, output), output.str());
        output.str("");
        output.clear();
    };
    workspace.add("joint", name, {0, 1, 0});
    workspace.add("box", name + ".rail", {0, 1, 0}, {8, .12f, .12f});
    command("parent " + name + ".rail " + name);
    workspace.add("sphere", name + ".probe", {-3, 1.4f, 0}, {.5f, .5f, .5f});
    command("parent " + name + ".probe " + name);
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
    command("select " + name);
    command("insert spin y 25");
    command("light point " + name + ".lamp at -3 2 0");
    command("parent " + name + ".lamp " + name + ".probe");
    command("light color 0.3 0.7 1");
    command("light intensity 2");
    command("camera add " + name + ".view at -3 2 4");
    command("camera look -3 1.4 0");
    command("parent " + name + ".view " + name + ".probe");
    command("camera use");
}
