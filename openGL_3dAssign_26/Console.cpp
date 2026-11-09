#include "Console.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {
void require(bool success, const char* message) {
    if (!success) throw std::runtime_error(message);
}
}

bool Console::initScene() {
    auto& scene = app_.getScene();
    scene.clear();
    const auto box = scene.addCuboid({2, 2, 2}, {0, 0, 0});
    const auto pyramid = scene.addPyramid({2, 2, 2}, {0, 0, 0});
    const auto lamp = scene.addCuboid({.25f, .25f, .25f}, {0, 0, 3});
    const auto ring = scene.addGroup();
    require(box && pyramid && lamp && ring, "Lighting scene creation failed.");
    objects_ = {*box, *pyramid};
    lamp_ = *lamp;
    ring_ = *ring;
    for (auto id : {objects_[0], objects_[1], lamp_}) {
        const auto count = scene.getComponent(id)->polyhedron.faces.size();
        for (std::size_t face = 0; face < count; ++face) {
            auto mesh = *scene.getMesh(id, face);
            mesh.color = {1, 1, 1};
            mesh.vertexColors.clear();
            mesh.material.color = id == lamp_ ? graphics::GlColor{1, 1, 1} : graphics::GlColor{.12f, .85f, .25f};
            mesh.material.unlit = id == lamp_;
            mesh.material.ambientStrength = .12f;
            mesh.material.specularStrength = .6f;
            mesh.material.shininess = 64;
            require(scene.setMesh(id, face, mesh), "Material assignment failed.");
        }
    }
    constexpr unsigned segments = 96;
    const double tau = 2 * std::acos(-1.0);
    for (unsigned i = 0; i < segments; ++i) {
        const double a = tau * i / segments, b = tau * (i + 1) / segments;
        require(scene.addLine({float(std::sin(a)), 0, float(std::cos(a))},
                              {float(std::sin(b)), 0, float(std::cos(b))}, {.2f, .5f, .25f}, ring_).has_value(),
                "Orbit ring creation failed.");
    }
    auto& options = app_.getRenderOptions();
    options = graphics::RenderOptions{};
    options.lighting = true;
    options.directionalIntensity = 0;
    options.ambientColor = {1, 1, 1};
    options.background = {0, 0, 0, 1};
    options.pointLights.push_back(graphics::PointLight{});
    options.pointLights[0].attenuation = .025f;
    auto& camera = app_.getCamera();
    camera = graphics::Camera{};
    camera.position = {5, 4, 8};
    shape_ = 0;
    objectAngle_ = lightAngle_ = 0;
    radius_ = 3;
    spinning_ = false;
    orbitDirection_ = 0;
    apply();
    std::cout << "실습 26: n 육면체/사각뿔 | m 조명 켜기/끄기 | y 자전 시작/정지\n"
              << "r/R 광원 양/음 공전 (같은 방향 재입력: 정지) | z/Z 가까이/멀리 | q 종료\n";
    return true;
}

void Console::apply() {
    auto& scene = app_.getScene();
    graphics::Transform3D object;
    object.rotation = glm::mat3(glm::rotate(glm::mat4(1), float(glm::radians(objectAngle_)), glm::vec3(0, 1, 0)));
    for (std::size_t i = 0; i < objects_.size(); ++i) {
        require(scene.setTransform(objects_[i], object), "Object spin failed.");
        require(scene.setVisible(objects_[i], i == shape_), "Shape switch failed.");
    }
    const float angle = float(glm::radians(lightAngle_));
    const glm::vec3 position{radius_ * std::sin(angle), 0, radius_ * std::cos(angle)};
    graphics::Transform3D lamp, ring;
    lamp.basis = position;
    ring.scale = {radius_, 1, radius_};
    require(scene.setTransform(lamp_, lamp) && scene.setTransform(ring_, ring), "Light/ring placement failed.");
    app_.getRenderOptions().pointLights[0].position = position;
    app_.getCamera().position = glm::normalize(glm::vec3(5, 4, 8)) * std::max(10.5f, 3.5f * radius_);
}

void Console::onKey(int key, int action, int mods) {
    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    if (key == GLFW_KEY_Z && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        radius_ = std::clamp(radius_ + (upper ? .2f : -.2f), 1.8f, 8.0f);
        apply();
    }
    if (action != GLFW_PRESS) return;
    switch (key) {
    case GLFW_KEY_N: shape_ = 1 - shape_; apply(); break;
    case GLFW_KEY_M: app_.getRenderOptions().lighting = !app_.getRenderOptions().lighting; break;
    case GLFW_KEY_Y: spinning_ = !spinning_; break;
    case GLFW_KEY_R: {
        const int direction = upper ? -1 : 1;
        orbitDirection_ = orbitDirection_ == direction ? 0 : direction;
        break;
    }
    case GLFW_KEY_Q: app_.requestClose(); break;
    default: break;
    }
}

void Console::onUpdate(float dt) {
    if (spinning_) objectAngle_ = std::fmod(objectAngle_ + 45 * dt, 360.0);
    lightAngle_ = std::fmod(lightAngle_ + orbitDirection_ * 60 * dt, 360.0);
    apply();
}
