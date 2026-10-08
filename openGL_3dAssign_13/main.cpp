#include <glew.h> // GLEW는 GLFW보다 먼저 포함한다.
#include <glfw3.h>

#include "Console.h"

#include <filesystem>
#include <iostream>

namespace {

constexpr const char* WINDOW_TITLE = "OpenGL Practice 13";

std::filesystem::path findShaderDirectory() {
    std::filesystem::path directory = std::filesystem::current_path();
    for (int depth = 0; depth < 6; ++depth) {
        if (std::filesystem::exists(directory / "VertexShader.glsl")
            && std::filesystem::exists(directory / "FragmentShader.glsl")) {
            return directory;
        }
        // 저장소 루트에서 실행할 때도 이 과제의 셰이더를 찾는다.
        const std::filesystem::path project = directory / "openGL_3dAssign_13";
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
    Console* console = static_cast<Console*>(glfwGetWindowUserPointer(window));
    if (console != nullptr) {
        console->onKey(key, action);
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Console* console = static_cast<Console*>(glfwGetWindowUserPointer(window));
    if (console != nullptr) {
        console->resizeFramebuffer(width, height);
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
        WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE, nullptr, nullptr);
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
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    const std::filesystem::path shaderDirectory = findShaderDirectory();
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

    glfwSetWindowUserPointer(window, &console);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    int exitCode = 0;
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        if (glfwWindowShouldClose(window)) {
            break;
        }
        if (!console.render()) {
            std::cerr << "Scene rendering failed.\n";
            exitCode = -1;
            break;
        }
        glfwSwapBuffers(window);
    }

    // GPU 자원은 GLFW 창과 OpenGL 컨텍스트보다 먼저 해제한다.
    glfwSetWindowUserPointer(window, nullptr);
    console.shutdownGraphics();
    glfwDestroyWindow(window);
    glfwTerminate();
    return exitCode;
}
