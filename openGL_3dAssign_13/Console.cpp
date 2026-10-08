#include <glew.h>
#include <glfw3.h>

#include "Console.h"

#include <cmath>
#include <utility>

bool Console::initScene() {
    // 기본 입력 값. 길이·월드 위치·회전을 각각 변경할 수 있다.
    const glm::vec3 cuboidSideLengths{ 1.0f, 1.0f, 1.0f };
    const glm::vec3 cuboidWorldPosition{ -0.9f, 0.0f, 0.0f };
    const glm::vec3 cuboidRotationDegrees{ 20.0f, -25.0f, 0.0f };
    const glm::vec3 tetrahedronEdgeLengths{ 1.0f, 1.0f, 1.0f };
    const glm::vec3 tetrahedronWorldPosition{ 0.9f, 0.0f, 0.0f };
    const glm::vec3 tetrahedronRotationDegrees{ 0.0f, 55.0f, 0.0f };

    graphics::Scene scene;
    const auto cuboid = scene.addCuboid(cuboidSideLengths, cuboidWorldPosition);
    const auto tetrahedron = scene.addRightTetrahedron(
        tetrahedronEdgeLengths, tetrahedronWorldPosition);
    if (!cuboid || !tetrahedron) {
        return false;
    }

    graphics::Transform3D cuboidTransform;
    cuboidTransform.basis = cuboidWorldPosition;
    cuboidTransform.rotationDegrees = cuboidRotationDegrees;
    graphics::Transform3D tetrahedronTransform;
    tetrahedronTransform.basis = tetrahedronWorldPosition;
    tetrahedronTransform.rotationDegrees = tetrahedronRotationDegrees;
    if (!scene.setTransform(*cuboid, cuboidTransform)
        || !scene.setTransform(*tetrahedron, tetrahedronTransform)) {
        return false;
    }

    const glm::vec3 basisOrigin{ 0.0f };
    const glm::mat3 basisVectors{ 1.0f };
    const glm::vec3 worldAxisLengths{ 1.4f };
    const glm::vec3 localAxisLengths{ 0.9f };
    const glm::vec3 vectorOrigin{ 0.0f, -0.9f, 0.0f };
    const glm::vec3 vectorValue{ 0.8f, 0.5f, 0.6f };
    const graphics::GlColor vectorColor{ 1.0f, 0.85f, 0.2f };
    if (!scene.addBasis(basisOrigin, basisVectors, worldAxisLengths)
        || !scene.addBasis(basisOrigin, basisVectors, localAxisLengths, *cuboid)
        || !scene.addBasis(basisOrigin, basisVectors, localAxisLengths, *tetrahedron)
        || !scene.addVector(vectorOrigin, vectorValue, vectorColor)) {
        return false;
    }
    scene_ = std::move(scene);
    cuboidId_ = cuboid;
    tetrahedronId_ = tetrahedron;
    return true;
}

bool Console::initGraphics(const std::filesystem::path& shaderDirectory) {
    if (window_ == nullptr || !initScene()) {
        return false;
    }
    if (!renderer_.initialize(shaderDirectory / "VertexShader.glsl",
                              shaderDirectory / "FragmentShader.glsl")) {
        return false;
    }
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    resizeFramebuffer(width, height);
    return true;
}

void Console::shutdownGraphics() {
    renderer_.destroy();
}

void Console::setKeyHandler(KeyHandler handler, void* userData) {
    keyHandler_ = handler;
    keyUserData_ = userData;
}

void Console::setUpdateHandler(UpdateHandler handler, void* userData) {
    updateHandler_ = handler;
    updateUserData_ = userData;
}

void Console::onKey(int key, int action) {
    if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
    if (keyHandler_ != nullptr) {
        keyHandler_(*this, key, action, keyUserData_);
    }
}

void Console::update(float deltaTime) {
    if (updateHandler_ != nullptr && std::isfinite(deltaTime) && deltaTime > 0.0f) {
        updateHandler_(*this, deltaTime, updateUserData_);
    }
}

void Console::resizeFramebuffer(int width, int height) {
    framebufferWidth_ = width;
    framebufferHeight_ = height;
}

bool Console::hasFramebuffer() const {
    return framebufferWidth_ > 0 && framebufferHeight_ > 0;
}

bool Console::render() {
    return renderer_.render(scene_, camera_, framebufferWidth_, framebufferHeight_);
}
