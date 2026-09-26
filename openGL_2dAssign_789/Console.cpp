#include <glew.h> // GLEW must precede GLFW.
#include <glfw3.h>

#include "Console.h"

#include <vector>

namespace {

constexpr float MOVE_STEP = 0.04f;
constexpr float DIAGONAL_STEP = 0.02828427f; // MOVE_STEP / sqrt(2)

Diagram makePractice7Diagram(GLShape shape) {
    const Point pivot{
        randomFloat(-0.75f, 0.75f),
        randomFloat(-0.75f, 0.75f)
    };
    GlColor color;
    color.setRandom();
    const float half = randomFloat(0.08f, 0.17f);

    switch (shape) {
    case GLShape::Spot:
        return Diagram(shape, std::vector<Point>{
            { -0.02f, -0.02f }, { 0.02f, -0.02f },
            { -0.02f, 0.02f }, { 0.02f, 0.02f }
        }, color, pivot);
    case GLShape::Line:
        return Diagram(shape, std::vector<Point>{
            { -half, 0.0f }, { half, 0.0f }
        }, color, pivot);
    case GLShape::Triangle:
        return Diagram(shape, std::vector<Point>{
            { -half, -half }, { half, -half }, { 0.0f, half }
        }, color, pivot);
    case GLShape::Rect:
        return Diagram(shape, std::vector<Point>{
            { -half, -half }, { half, -half },
            { -half, half }, { half, half }
        }, color, pivot);
    }
    return Diagram(GLShape::Triangle, std::vector<Point>{
        { -half, -half }, { half, -half }, { 0.0f, half }
    }, color, pivot);
}

} // namespace

bool PracticeConsole::initGraphics(const std::filesystem::path& shaderDirectory) {
    if (!shader_.initShader(shaderDirectory / "VertexShader.glsl",
                            shaderDirectory / "FragmentShader.glsl")) {
        return false;
    }
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
    glClearColor(0.12f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    if (uploadedRevision_ != store_.getRevision()) {
        if (!shader_.upload(store_.getItems(), store_.getSelectedIndex())) {
            return false;
        }
        uploadedRevision_ = store_.getRevision();
    }
    shader_.render();
    return true;
}

void Practice7Console::onKey(int key, int action) {
    if (handleQuit(key, action)) {
        return;
    }
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_P: store_.push(makePractice7Diagram(GLShape::Spot)); return;
        case GLFW_KEY_E: store_.push(makePractice7Diagram(GLShape::Line)); return;
        case GLFW_KEY_T: store_.push(makePractice7Diagram(GLShape::Triangle)); return;
        case GLFW_KEY_R: store_.push(makePractice7Diagram(GLShape::Rect)); return;
        case GLFW_KEY_C: store_.clear(); return;
        default: break;
        }
    }

    switch (key) {
    case GLFW_KEY_W: store_.translateSelected({ 0.0f, MOVE_STEP }); break;
    case GLFW_KEY_A: store_.translateSelected({ -MOVE_STEP, 0.0f }); break;
    case GLFW_KEY_S: store_.translateSelected({ 0.0f, -MOVE_STEP }); break;
    case GLFW_KEY_D: store_.translateSelected({ MOVE_STEP, 0.0f }); break;
    case GLFW_KEY_I: store_.translateSelected({ -DIAGONAL_STEP, DIAGONAL_STEP }); break;
    case GLFW_KEY_J: store_.translateSelected({ DIAGONAL_STEP, DIAGONAL_STEP }); break;
    case GLFW_KEY_K: store_.translateSelected({ -DIAGONAL_STEP, -DIAGONAL_STEP }); break;
    case GLFW_KEY_L: store_.translateSelected({ DIAGONAL_STEP, -DIAGONAL_STEP }); break;
    case GLFW_KEY_1: store_.translateAll({ -MOVE_STEP, 0.0f }); break;
    case GLFW_KEY_2: store_.translateAll({ MOVE_STEP, 0.0f }); break;
    case GLFW_KEY_3: store_.translateAll({ 0.0f, MOVE_STEP }); break;
    case GLFW_KEY_4: store_.translateAll({ 0.0f, -MOVE_STEP }); break;
    default: break;
    }
}

void Practice7Console::onMouseButton(int button, int action, Point point) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        store_.selectAt(point);
    }
}

void Practice8Console::onKey(int key, int action) {
    handleQuit(key, action);
}

void Practice9Console::onKey(int key, int action) {
    handleQuit(key, action);
}
