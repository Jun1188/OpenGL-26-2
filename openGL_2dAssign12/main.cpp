#include <glew.h> // GLEW must be included before GLFW.
#include <glfw3.h>

#include "Console.h"

#include <filesystem>
#include <iostream>
#include <memory>

namespace {

constexpr const char* WINDOW_TITLE = "OpenGL Practice 12";

std::unique_ptr<PracticeConsole> createConsole(GLFWwindow* window) {
    return std::make_unique<PracticeConsole>(window);
}

std::filesystem::path findShaderDirectory() {
    std::filesystem::path directory = std::filesystem::current_path();

    for (int depth = 0; depth < 6; ++depth) {
        if (std::filesystem::exists(directory / "VertexShader.glsl") &&
            std::filesystem::exists(directory / "FragmentShader.glsl")) {
            return directory;
        }

        if (directory == directory.parent_path()) {
            break;
        }
        directory = directory.parent_path();
    }

    return {};
}

void keyCallback(GLFWwindow* window, int key, int, int action, int) {
    Console* console = static_cast<Console*>(glfwGetWindowUserPointer(window));

    if (console != nullptr) {
        console->onKey(key, action);
    }
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    // Preserve the initial layout aspect ratio when the window is resized.
    const double aspect = static_cast<double>(WINDOW_WIDTH) / WINDOW_HEIGHT;
    int boardWidth = width;
    int boardHeight = height;

    if (width > height * aspect)
        boardWidth = static_cast<int>(height * aspect);
    else
        boardHeight = static_cast<int>(width / aspect);
    glViewport((width - boardWidth) / 2, (height - boardHeight) / 2, boardWidth, boardHeight);
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

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);

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
    framebufferSizeCallback(window, framebufferWidth, framebufferHeight);

    std::cout << "Practice 12: Enter stack, R reset, Q/Esc quit.\n";

    std::unique_ptr<PracticeConsole> console = createConsole(window);

    if (!console->initGraphics(shaderDirectory)) {
        glfwDestroyWindow(window);
        glfwTerminate();

        return -1;
    }

    glfwSetWindowUserPointer(window, console.get());
    glfwSetKeyCallback(window, keyCallback);
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
