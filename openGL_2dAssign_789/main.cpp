#define PROB 7

#include <glew.h> // GLEW must be included before GLFW.
#include <glfw3.h>

#include "Console.h"

#include <filesystem>
#include <iostream>
#include <memory>

#if PROB != 7 && PROB != 8 && PROB != 9
#error PROB must be 7, 8, or 9.
#endif

namespace {

#if PROB == 7
constexpr const char* WINDOW_TITLE = "OpenGL Practice 7";
#elif PROB == 8
constexpr const char* WINDOW_TITLE = "OpenGL Practice 8";
#else
constexpr const char* WINDOW_TITLE = "OpenGL Practice 9";
#endif

std::unique_ptr<PracticeConsole> createConsole(GLFWwindow* window) {
#if PROB == 7
    return std::make_unique<Practice7Console>(window);
#elif PROB == 8
    return std::make_unique<Practice8Console>(window);
#else
    return std::make_unique<Practice9Console>(window);
#endif
}

std::filesystem::path findShaderDirectory() {
    std::filesystem::path directory = std::filesystem::current_path();
    for (int depth = 0; depth < 6; ++depth) {
        if (std::filesystem::exists(directory / "VertexShader.glsl")
            && std::filesystem::exists(directory / "FragmentShader.glsl")) {
            return directory;
        }
        if (directory == directory.parent_path()) {
            break;
        }
        directory = directory.parent_path();
    }
    return {};
}

Point screenToOpenGl(GLFWwindow* window, double mouseX, double mouseY) {
    int width = 0;
    int height = 0;
    glfwGetWindowSize(window, &width, &height);
    if (width <= 0 || height <= 0) {
        return {};
    }
    return {
        static_cast<float>(2.0 * mouseX / width - 1.0),
        static_cast<float>(1.0 - 2.0 * mouseY / height)
    };
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
    Console* console = static_cast<Console*>(glfwGetWindowUserPointer(window));
    if (console != nullptr) {
        console->onKey(key, action);
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int) {
    Console* console = static_cast<Console*>(glfwGetWindowUserPointer(window));
    if (console == nullptr) {
        return;
    }
    double mouseX = 0.0;
    double mouseY = 0.0;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    console->onMouseButton(button, action, screenToOpenGl(window, mouseX, mouseY));
}

void cursorPositionCallback(GLFWwindow* window, double mouseX, double mouseY) {
    Console* console = static_cast<Console*>(glfwGetWindowUserPointer(window));
    if (console != nullptr) {
        console->onCursorMove(screenToOpenGl(window, mouseX, mouseY));
    }
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
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

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr
    );
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

    const std::filesystem::path shaderDirectory = findShaderDirectory();
    if (shaderDirectory.empty()) {
        std::cerr << "VertexShader.glsl and FragmentShader.glsl were not found.\n";
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    std::unique_ptr<PracticeConsole> console = createConsole(window);
    if (!console->initGraphics(shaderDirectory)) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glfwSetWindowUserPointer(window, console.get());
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    int exitCode = 0;
    double previousTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        const double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;
        if (deltaTime > 0.05f) {
            deltaTime = 0.05f;
        }

        console->update(deltaTime);
        if (!console->render()) {
            exitCode = -1;
            break;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    console->shutdownGraphics();
    console.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
    return exitCode;
}
