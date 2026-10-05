#include <glew.h> // GLEW must precede GLFW.
#include <glfw3.h>

#include "Console.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>




bool PracticeConsole::initGraphics(const std::filesystem::path& shaderDirectory) {
    if (!shader_.initShader(shaderDirectory / "VertexShader.glsl",
                            shaderDirectory / "FragmentShader.glsl")) return false;
    if (!shader_.initBuffer()) {
        shader_.destroy();
        return false;
    }
    return true;
}

void PracticeConsole::shutdownGraphics() {
    shader_.destroy();
}

bool PracticeConsole::handleQuit(int key, int action) {
    if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
        return true;
    }
    return false;
}

bool PracticeConsole::render() {
    glClearColor(0.08f, 0.08f, 0.12f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    if (uploadedRevision_ != store_.getRevision()) {
        const std::vector<Diagram> items = store_.getItems();
        // Grid always puts the active hero last, after the board and effects.
        const std::optional<std::size_t> selected = items.empty()
            ? std::nullopt : std::optional<std::size_t>(items.size() - 1);
        if (!shader_.upload(items, selected)) return false;
        uploadedRevision_ = store_.getRevision();
    }
    shader_.render();
    return true;
}

Practice11Console::Practice11Console(GLFWwindow* window)
    : PracticeConsole(window) {
}

void Practice11Console::onKey(int key, int action) {
    if (handleQuit(key, action) || (action != GLFW_PRESS && action != GLFW_REPEAT)) return;
    
}

void Practice11Console::update(float deltaTime) {
    if (!std::isfinite(deltaTime) || deltaTime <= 0.0f) return;

    effectTime_ += deltaTime;

    moveTime_ += deltaTime;
   
}
