#include "ExamController.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
constexpr double pi = 3.14159265358979323846;
void require(bool result, const char* message) { if (!result) throw std::runtime_error(message); }
int toggleDirection(int current, int desired) { return current == desired ? 0 : desired; }
}
glm::vec3 ExamController::orbitPoint(double degrees) const {
    const double radians = degrees * pi / 180;
    return state_.orbitCenter + glm::vec3(state_.radius * float(std::cos(radians)), state_.height,
                                         state_.radius * float(std::sin(radians)));
}
void ExamController::initialize() {
    // @debug hook initialize
    state_ = State{};
    state_.light = scene_.light("key");
    lightParent_ = scene_.lightParent("key");
    state_.camera = app_.getCamera();
    state_.lighting = app_.getRenderOptions().lighting;
    state_.radius = glm::length(glm::vec2(state_.light.position.x, state_.light.position.z));
    state_.height = state_.light.position.y;
    state_.lightDegrees = std::atan2(state_.light.position.z, state_.light.position.x) * 180 / pi;
    initial_ = state_;
    auto sphere = graphics::DiagramSystem::makeSphere(.55f, 24, 16);
    for (auto& mesh : sphere.meshes()) {
        mesh.material.color = {.35f, .7f, .9f};
        mesh.material.specularStrength = .8f;
        mesh.material.shininess = 64;
    }
    graphics::Transform3D pose;
    pose.basis = {2.5f, 1.f, 0.f}; pose.scale = {1.5f, .8f, 1.f};
    scene_.addModel("studySphere", sphere, pose);
    scene_.addLayer("studySphere", "studySpin", graphics::LayerSetup{});
    auto marker = graphics::DiagramSystem::makeSphere(.10f, 12, 8);
    for (auto& mesh : marker.meshes()) { mesh.material.unlit = true; mesh.material.color = {1, 1, 1}; }
    marker_ = scene_.addModel("lightMarker", marker);
    ring_.clear(); ringRadius_ = -1; move_.fill(false);
    update(0);
    scene_.update(0);
    syncVisuals();
    help();
}
void ExamController::reset() { state_ = initial_; move_.fill(false); update(0); }
void ExamController::help() const {
    std::cout << "코딩 작업공간: m 조명 | c 광원 색 3종 | y/Y 광원 +/- 공전 토글 | z/Z 반경 감소/증가\n"
                 "w/a/s/d 시점 월드 XZ 이동 | r/R 카메라 +/- 공전 토글 | p 구의 지역 자전 토글\n"
                 "F2 셰이더 출력(합성/법선/확산/정반사) | Home 초기화 | Space 시간 정지 | q/Esc 종료\n"
                 "수정 위치: SceneSetup.cpp / ExamController.h,cpp / framework/shaders/*.glsl\n";
}
void ExamController::onKey(int key, int action, int mods) {
    // @debug hook input
    const int moveKeys[] = {GLFW_KEY_W, GLFW_KEY_A, GLFW_KEY_S, GLFW_KEY_D};
    for (std::size_t i = 0; i < move_.size(); ++i) if (key == moveKeys[i]) move_[i] = action != GLFW_RELEASE;
    if (action != GLFW_PRESS) return;
    const int direction = mods & GLFW_MOD_SHIFT ? -1 : 1;
    if (key == GLFW_KEY_M) state_.lighting = !state_.lighting;
    if (key == GLFW_KEY_C) {
        const graphics::GlColor colors[] = {{1, 1, 1}, {1, .3f, .2f}, {.2f, .5f, 1}};
        state_.color = (state_.color + 1) % 3;
        state_.light.color = colors[state_.color];
    }
    if (key == GLFW_KEY_Y) state_.lightDirection = toggleDirection(state_.lightDirection, direction);
    if (key == GLFW_KEY_R) state_.cameraDirection = toggleDirection(state_.cameraDirection, direction);
    if (key == GLFW_KEY_P) state_.spinDirection = state_.spinDirection ? 0 : 1;
    if (key == GLFW_KEY_Z) state_.radius = std::max(.25f, state_.radius + (mods & GLFW_MOD_SHIFT ? .25f : -.25f));
    if (key == GLFW_KEY_F2) state_.shaderView = (state_.shaderView + 1) % 4;
    if (key == GLFW_KEY_F1) help();
    if (key == GLFW_KEY_Q) app_.requestClose();
}
void ExamController::onFocus(bool focused) { if (!focused) move_.fill(false); }
void ExamController::update(float dt) {
    // @debug hook update
    require(std::isfinite(dt) && dt >= 0, "ExamController: invalid frame delta.");
    state_.lightDegrees = std::remainder(state_.lightDegrees + state_.lightDirection * 45.0 * dt, 360.0);
    state_.spinDegrees = std::remainder(state_.spinDegrees + state_.spinDirection * 35.0 * dt, 360.0);
    auto movement = glm::vec3(int(move_[3]) - int(move_[1]), 0, int(move_[2]) - int(move_[0]));
    if (glm::length(movement) > 0) state_.camera.position += glm::normalize(movement) * state_.moveSpeed * dt;
    const auto orbit = glm::rotate(glm::mat4(1), glm::radians(float(state_.cameraDirection) * 35.f * dt), glm::vec3(0, 1, 0));
    state_.camera.position = state_.camera.target + glm::vec3(orbit * glm::vec4(state_.camera.position - state_.camera.target, 0));
    graphics::LayerSetup spin;
    spin.rotationDegrees.y = float(state_.spinDegrees);
    scene_.setLayer("studySphere", "studySpin", spin);
    state_.light.position = orbitPoint(state_.lightDegrees);
    scene_.setLight("key", state_.light, lightParent_);
    // Camera motion here is explicitly world-authored; another controller must not write it simultaneously.
    scene_.setCamera(state_.camera);
    app_.getRenderOptions().lighting = state_.lighting;
}
void ExamController::syncVisuals() {
    // @debug hook final-sync
    auto& scene = app_.getScene();
    glm::mat4 frame(1);
    if (!lightParent_.empty()) {
        const auto world = scene.worldMatrix(scene_.object(lightParent_));
        require(world.has_value(), "ExamController: light parent frame unavailable."); frame = *world;
    }
    const auto worldPoint = [&](double angle) { return glm::vec3(frame * glm::vec4(orbitPoint(angle), 1)); };
    require(scene.setWorldPlacement(marker_, glm::translate(glm::mat4(1), worldPoint(state_.lightDegrees))), "Light marker placement failed.");
    require(scene.setTint(marker_, state_.light.color), "Light marker color failed.");
    const bool guideDirty = ring_.empty() || ringFrame_ != frame || ringCenter_ != state_.orbitCenter ||
        ringRadius_ != state_.radius || ringHeight_ != state_.height;
    if (!guideDirty) return;
    constexpr std::size_t segments = 64;
    if (ring_.empty() && state_.radius > 0) {
        for (std::size_t i = 0; i < segments; ++i) {
            const auto line = scene.addLine(worldPoint(360.0 * i / segments), worldPoint(360.0 * (i + 1) / segments), {.35f, .4f, .45f});
            require(line.has_value(), "Orbit guide creation failed."); ring_.push_back(*line);
        }
    }
    for (std::size_t i = 0; i < ring_.size(); ++i) {
        const auto a = worldPoint(360.0 * i / segments), b = worldPoint(360.0 * (i + 1) / segments);
        require(scene.setVector(ring_[i], a, b - a), "Orbit guide update failed.");
    }
    ringFrame_ = frame; ringCenter_ = state_.orbitCenter; ringRadius_ = state_.radius; ringHeight_ = state_.height;
}
bool ExamController::bindUniforms(const graphics::DrawUniformContext& context) const {
    // @debug hook uniform
    return context.setUniform("uStudyView", state_.shaderView);
}
