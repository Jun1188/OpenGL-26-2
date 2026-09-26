#define PROB 6

#include <glew.h> // GLEW must be included before GLFW.
#include <glfw3.h>

#include "Console.h"

#include <iostream>
#include <memory>

#if PROB != 2 && PROB != 3 && PROB != 4 && PROB !=5 && PROB != 6
#error PROB must be 2 ~ 6.
#endif

namespace {

#if PROB == 2
constexpr const char* WINDOW_TITLE = "OpenGL Practice 2";
#elif PROB == 3
constexpr const char* WINDOW_TITLE = "OpenGL Practice 3";
#elif PROB == 4
constexpr const char* WINDOW_TITLE = "OpenGL Practice 4";
#elif PROB == 5
constexpr const char* WINDOW_TITLE = "OpenGL Practice 5";
#elif PROB == 6
constexpr const char* WINDOW_TITLE = "OpenGL Practice 6";
#endif

std::unique_ptr<Console> createConsole(GLFWwindow* window) {
#if PROB == 2
    return std::make_unique<Practice2Console>(window);
#elif PROB == 3
    return std::make_unique<Practice3Console>(window);
#elif PROB == 4
    return std::make_unique<Practice4Console>(window);
#elif PROB == 5
    return std::make_unique<Practice5Console>(window);
#elif PROB == 6
    return std::make_unique<Practice6Console>(window);
#endif
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
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    GLFWwindow* window = glfwCreateWindow(
        WINDOW_WIDTH,
        WINDOW_HEIGHT,
        WINDOW_TITLE,
        nullptr,
        nullptr
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

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    std::unique_ptr<Console> console = createConsole(window);
    glfwSetWindowUserPointer(window, console.get());
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, cursorPositionCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    double previousTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        const double currentTime = glfwGetTime();
        float deltaTime = static_cast<float>(currentTime - previousTime);
        previousTime = currentTime;

        if (deltaTime > 0.05f) {
            deltaTime = 0.05f;
        }
        console->update(deltaTime);
        console->render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    console.reset();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
