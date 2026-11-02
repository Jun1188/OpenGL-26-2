#include <glew.h> // GLEW must precede GLFW.
#include <glfw3.h>

#include "Console.h"

bool PracticeConsole::initGraphics(const std::filesystem::path& shaderDirectory) {
    if (!shader_.initShader(shaderDirectory / "VertexShader.glsl", shaderDirectory / "FragmentShader.glsl"))
        return false;

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
        if (!shader_.upload(store_.getItems()))
            return false;
        uploadedRevision_ = store_.getRevision();
    }
    shader_.render();

    return true;
}

void PracticeConsole::onKey(int key, int action) {
    if (handleQuit(key, action) || action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
        store_.checkHit();
    } else if (key == GLFW_KEY_R) {
        store_.reset();
    }
}

void PracticeConsole::update(float deltaTime) {
    store_.advance(deltaTime);
}
