#include <glew.h>
#include <glfw3.h>

#include "Console.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>

namespace {

constexpr const char* WINDOW_TITLE = "OpenGL Practice 13";

struct Controls {
    enum class Target { Cuboid, Tetrahedron, Camera };
    Target target = Target::Cuboid;
    int sizeAxis = 0;
    float moveSpeed = 1.0f;
    float resizeSpeed = 0.6f;
    std::array<bool, GLFW_KEY_LAST + 1> pressed{};
};

struct WindowState {
    Console* console = nullptr;
    Controls* controls = nullptr;
};

// 이 두 함수는 다른 .cpp로 옮겨도 같은 콜백 API로 등록할 수 있다.
void onKey(Console&, int key, int action, void* userData) {
    Controls& controls = *static_cast<Controls*>(userData);
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            controls.pressed[key] = true;
        } else if (action == GLFW_RELEASE) {
            controls.pressed[key] = false;
        }
    }
    if (action != GLFW_PRESS) {
        return;
    }
    switch (key) {
    case GLFW_KEY_1: controls.target = Controls::Target::Cuboid; break;
    case GLFW_KEY_2: controls.target = Controls::Target::Tetrahedron; break;
    case GLFW_KEY_3: controls.target = Controls::Target::Camera; break;
    case GLFW_KEY_X: controls.sizeAxis = 0; break;
    case GLFW_KEY_Y: controls.sizeAxis = 1; break;
    case GLFW_KEY_Z: controls.sizeAxis = 2; break;
    default: break;
    }
}

void onUpdate(Console& console, float deltaTime, void* userData) {
    const Controls& controls = *static_cast<Controls*>(userData);
    const auto& key = controls.pressed;
    glm::vec3 direction{
        static_cast<float>(key[GLFW_KEY_RIGHT]) - static_cast<float>(key[GLFW_KEY_LEFT]),
        static_cast<float>(key[GLFW_KEY_UP]) - static_cast<float>(key[GLFW_KEY_DOWN]),
        static_cast<float>(key[GLFW_KEY_PAGE_UP]) - static_cast<float>(key[GLFW_KEY_PAGE_DOWN])
    };
    const bool moving = glm::dot(direction, direction) > 0.0f;
    const glm::vec3 movement = moving
        ? glm::normalize(direction) * controls.moveSpeed * deltaTime : glm::vec3(0.0f);

    if (controls.target == Controls::Target::Camera) {
        // 카메라만 이동하고 target은 유지해 입체를 다른 방향에서 볼 수 있다.
        if (moving) {
            graphics::Camera& camera = console.getCamera();
            graphics::Camera candidate = camera;
            candidate.position += movement;
            if (graphics::CameraSystem::validCamera(candidate)) {
                camera = candidate;
            }
        }
        return;
    }

    const auto id = controls.target == Controls::Target::Cuboid
        ? console.getCuboidId() : console.getTetrahedronId();
    if (!id) {
        return;
    }
    graphics::Scene& scene = console.getScene();
    const graphics::Component_Poly* component = scene.getComponent(*id);
    if (component == nullptr) {
        return;
    }
    if (moving) {
        scene.setPosition(*id, component->transform.basis + movement);
    }

    const float sizeDirection =
        static_cast<float>(key[GLFW_KEY_EQUAL] || key[GLFW_KEY_KP_ADD])
        - static_cast<float>(key[GLFW_KEY_MINUS] || key[GLFW_KEY_KP_SUBTRACT]);
    if (sizeDirection != 0.0f) {
        glm::vec3 lengths = component->sideLengths;
        const int axis = controls.sizeAxis;
        lengths[axis] = std::max(0.05f,
            lengths[axis] + sizeDirection * controls.resizeSpeed * deltaTime);
        if (lengths[axis] != component->sideLengths[axis]) {
            scene.setSideLengths(*id, lengths);
        }
    }
}

std::filesystem::path findShaderDirectory() {
    std::filesystem::path directory = std::filesystem::current_path();
    for (int depth = 0; depth < 6; ++depth) {
        if (std::filesystem::exists(directory / "VertexShader.glsl")
            && std::filesystem::exists(directory / "FragmentShader.glsl")) {
            return directory;
        }
        const auto project = directory / "openGL_3dAssign_13";
        if (std::filesystem::exists(project / "VertexShader.glsl")
            && std::filesystem::exists(project / "FragmentShader.glsl")) {
            return project;
        }
        if (directory == directory.parent_path()) {
            break;
        }
        directory = directory.parent_path();
    }
    return {};
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
    WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (state != nullptr && state->console != nullptr) {
        state->console->onKey(key, action);
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (state != nullptr && state->console != nullptr) {
        state->console->resizeFramebuffer(width, height);
    }
}

void focusCallback(GLFWwindow* window, int focused) {
    WindowState* state = static_cast<WindowState*>(glfwGetWindowUserPointer(window));
    if (state != nullptr && state->controls != nullptr && focused == GLFW_FALSE) {
        state->controls->pressed.fill(false);
    }
}

} // namespace

int main() {
    if (!glfwInit()) {
        std::cerr << "GLFW initialization failed.\n";
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    GLFWwindow* window = glfwCreateWindow(
        graphics::WINDOW_WIDTH, graphics::WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Window creation failed.\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cerr << "GLEW initialization failed.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    const auto shaderDirectory = findShaderDirectory();
    if (shaderDirectory.empty()) {
        std::cerr << "VertexShader.glsl and FragmentShader.glsl were not found.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    Console console(window);
    if (!console.initGraphics(shaderDirectory)) {
        std::cerr << "Scene or graphics initialization failed.\n";
        console.shutdownGraphics();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    Controls controls;
    console.setKeyHandler(onKey, &controls);
    console.setUpdateHandler(onUpdate, &controls);
    WindowState windowState{ &console, &controls };
    glfwSetWindowUserPointer(window, &windowState);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetWindowFocusCallback(window, focusCallback);
    std::cout << "1: cuboid, 2: right tetrahedron, 3: camera\n"
              << "Arrows: world X/Y, PageUp/PageDown: world Z\n"
              << "X/Y/Z: resize axis, +/-: resize, Esc/Q: quit\n";

    int exitCode = 0;
    double previousTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
            break;
        }
        if (!console.hasFramebuffer()) {
            glfwWaitEvents();
            previousTime = glfwGetTime();
            continue;
        }
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(
            std::clamp(currentTime - previousTime, 0.0, 0.05));
        previousTime = currentTime;
        console.update(deltaTime);
        if (!console.render()) {
            std::cerr << "Scene rendering failed.\n";
            exitCode = -1;
            break;
        }
        glfwSwapBuffers(window);
    }

    glfwSetWindowUserPointer(window, nullptr);
    console.setKeyHandler(nullptr);
    console.setUpdateHandler(nullptr);
    console.shutdownGraphics();
    glfwDestroyWindow(window);
    glfwTerminate();
    return exitCode;
}
