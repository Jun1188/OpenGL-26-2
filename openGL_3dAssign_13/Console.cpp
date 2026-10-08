#include <glew.h> // GLEW는 GLFW보다 먼저 포함한다.
#include <glfw3.h>

#include "Console.h"

#include <utility>

bool Console::initScene() {
    Entity scene;

    Transform3D cubeTransform;
    cubeTransform.basis = { -0.9f, 0.0f, 0.0f };
    cubeTransform.rotationDegrees = { 20.0f, -25.0f, 0.0f };
    if (!scene.push_entity(
        DiagramSystem::makePolyhedron(GLShape::Cube), cubeTransform)) {
        return false;
    }

    Transform3D tetraTransform;
    tetraTransform.basis = { 0.9f, 0.0f, 0.0f };
    tetraTransform.rotationDegrees = { 0.0f, 25.0f, 0.0f };
    if (!scene.push_entity(
        DiagramSystem::makePolyhedron(GLShape::Tetra), tetraTransform)) {
        return false;
    }
    entities_ = std::move(scene);
    return true;
}

bool Console::initGraphics(const std::filesystem::path& shaderDirectory) {
    if (window_ == nullptr || !initScene()) {
        return false;
    }
    if (!shader_.initShader(shaderDirectory / "VertexShader.glsl",
                            shaderDirectory / "FragmentShader.glsl")
        || !shader_.initBuffer()) {
        shader_.destroy();
        return false;
    }
    uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();

    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    resizeFramebuffer(width, height);
    return true;
}

void Console::shutdownGraphics() {
    shader_.destroy();
    uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
}

void Console::onKey(int key, int action) {
    if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}

void Console::resizeFramebuffer(int width, int height) {
    framebufferWidth_ = width;
    framebufferHeight_ = height;
    glViewport(0, 0, width, height);
}

bool Console::render() {
    if (framebufferWidth_ <= 0 || framebufferHeight_ <= 0) {
        return true; // 최소화 중에는 화면 비율 계산과 렌더링을 건너뛴다.
    }

    glClearColor(0.12f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 형상 등록·삭제 때만 버퍼 갱신. 변환 변경은 render의 uMVP로 전달한다.
    if (uploadedRevision_ != entities_.getGeometryRevision()) {
        if (!shader_.upload(entities_)) {
            return false;
        }
        uploadedRevision_ = entities_.getGeometryRevision();
    }
    const float aspect = static_cast<float>(framebufferWidth_)
        / static_cast<float>(framebufferHeight_);
    const glm::mat4 view = CameraSystem::viewMatrix(camera_);
    const glm::mat4 projection = CameraSystem::projectionMatrix(camera_, aspect);
    return shader_.render(entities_, view, projection);
}
