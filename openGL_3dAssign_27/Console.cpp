#include "Console.h"
#include <array>
#include <iostream>
#include <stdexcept>

namespace {
const std::array<graphics::GlColor, 3> colors{{{1, 1, 1}, {1, .25f, .2f}, {.25f, .45f, 1}}};
void require(bool success, const char* message) {
    if (!success) throw std::runtime_error(message);
}
}

bool Console::initScene() {
    auto& scene = app_.getScene();
    scene.clear();
    const std::array<glm::vec3, 3> positions{{{0, 0, 0}, {-3, 0, 0}, {3, 0, 0}}};
    const std::array<float, 3> radii{{.85f, .55f, 1.2f}};
    const std::array<graphics::GlColor, 3> materials{{{.1f, .8f, .2f}, {.15f, .25f, .95f}, {.9f, .15f, .12f}}};
    for (std::size_t i = 0; i < positions.size(); ++i) {
        const auto sphere = scene.addSphere(radii[i], positions[i], 48, 24);
        require(sphere.has_value(), "Sphere creation failed.");
        const auto count = scene.getComponent(*sphere)->polyhedron.faces.size();
        for (std::size_t face = 0; face < count; ++face) {
            auto mesh = *scene.getMesh(*sphere, face);
            mesh.color = {1, 1, 1};
            mesh.vertexColors.clear();
            mesh.material.color = materials[i];
            mesh.material.ambientStrength = .1f;
            mesh.material.specularStrength = .8f;
            mesh.material.shininess = 96;
            require(scene.setMesh(*sphere, face, mesh), "Sphere material failed.");
        }
    }
    const auto marker = graphics::PatternSystem::addBox(scene, {.2f, .2f, .2f}, {0, 2, 5}, {1, 1, 1});
    require(marker.has_value(), "Light marker creation failed.");
    lamp_ = *marker;
    const auto count = scene.getComponent(lamp_)->polyhedron.faces.size();
    for (std::size_t face = 0; face < count; ++face) {
        auto material = scene.getMesh(lamp_, face)->material;
        material.unlit = true;
        require(scene.setMaterial(lamp_, face, material), "Unlit marker failed.");
    }
    auto& options = app_.getRenderOptions();
    options = graphics::RenderOptions{};
    options.lighting = true;
    options.directionalIntensity = 0;
    options.ambientColor = {1, 1, 1};
    options.background = {0, 0, 0, 1};
    options.pointLights.push_back(graphics::PointLight{});
    options.pointLights[0].attenuation = .015f;
    options.pointLights[0].intensity = 1.5f;
    auto& camera = app_.getCamera();
    camera = graphics::Camera{};
    camera.position = {0, 2, 12};
    color_ = 0;
    angle_ = 0;
    direction_ = 0;
    apply();
    std::cout << "실습 27: c 광원 색 (백색/적색/청색) | r/R 양/음 방향 공전\n"
              << "같은 공전 방향 재입력: 정지 | q 종료\n";
    return true;
}

void Console::apply() {
    const float angle = float(glm::radians(angle_));
    const glm::vec3 position{5 * std::sin(angle), 2, 5 * std::cos(angle)};
    auto& light = app_.getRenderOptions().pointLights[0];
    light.position = position;
    light.color = colors[color_];
    graphics::Transform3D pose;
    pose.basis = position;
    require(app_.getScene().setTransform(lamp_, pose), "Light orbit failed.");
    require(app_.getScene().setTint(lamp_, colors[color_]), "Light marker color failed.");
}

void Console::onKey(int key, int action, int mods) {
    if (action != GLFW_PRESS) return;
    switch (key) {
    case GLFW_KEY_C: color_ = (color_ + 1) % colors.size(); apply(); break;
    case GLFW_KEY_R: {
        const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
        const int next = upper ? -1 : 1;
        direction_ = direction_ == next ? 0 : next;
        break;
    }
    case GLFW_KEY_Q: app_.requestClose(); break;
    default: break;
    }
}

void Console::onUpdate(float dt) {
    angle_ = std::fmod(angle_ + direction_ * 60 * dt, 360.0);
    apply();
}
