#include <glew.h> // GLEW must be included before GLFW.
#include <glfw3.h>

#include "Console.h"

#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>
#include <string>

namespace {

constexpr const char* WINDOW_TITLE = "OpenGL Practice 11";

std::size_t gridColumns = 20;
std::size_t gridRows = 20;

bool readGridSize(std::size_t& columns, std::size_t& rows) {

    constexpr long long MAX_DIMENSION = std::numeric_limits<int>::max() / 8;
    while (true) {
        std::cout << "Grid columns (x), rows (y): " << std::flush;
        std::string line;
        if (!std::getline(std::cin, line)) {
            std::cerr << "Grid input ended.\n";
            return false;
        }

        std::istringstream input(line);
        long long x = 0;
        long long y = 0;
        std::string extra;
        if (!(input >> x >> y) || (input >> extra)
            || x <= 0 || y <= 0 || x > MAX_DIMENSION || y > MAX_DIMENSION) {
            std::cout << "Enter two positive integers from 1 to "
                      << MAX_DIMENSION << ".\n";
            continue;
        }
        const std::size_t newColumns = static_cast<std::size_t>(x);
        const std::size_t newRows = static_cast<std::size_t>(y);
        if (newColumns > std::numeric_limits<std::size_t>::max() / newRows) {
            std::cout << "Grid cell count is too large. Try again.\n";
            continue;
        }
      
        columns = newColumns;
        rows = newRows;
        return true;
    }
}

std::unique_ptr<PracticeConsole> createConsole(GLFWwindow* window,
                                              std::size_t columns, std::size_t rows) {
    return std::make_unique<Practice11Console>(window, columns, rows);
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

void keyCallback(GLFWwindow* window, int key, int, int action, int mods) {
    // On the main keyboard, '+' is Shift+'='. Plain '=' has no command.
    if (key == GLFW_KEY_EQUAL && (mods & GLFW_MOD_SHIFT) == 0) return;
    Console* console = static_cast<Console*>(glfwGetWindowUserPointer(window));
    if (console != nullptr) {
        console->onKey(key, action);
    }
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {

    const double aspect = static_cast<double>(gridColumns) / gridRows;
    int boardWidth = width;
    int boardHeight = height;
    if (width > height * aspect) boardWidth = static_cast<int>(height * aspect);
    else boardHeight = static_cast<int>(width / aspect);
    glViewport((width - boardWidth) / 2, (height - boardHeight) / 2,
               boardWidth, boardHeight);
}

} // namespace

int main() {
   
    if (!readGridSize(gridColumns, gridRows)) return -1;

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
    framebufferSizeCallback(window, framebufferWidth, framebufferHeight);

    std::cout << "Practice 11: automatic zigzag, +/- speed, R reset, Q/Esc quit.\n";
    std::unique_ptr<PracticeConsole> console = createConsole(window, gridColumns, gridRows);
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
