#include <glew.h> // GLEW must precede GLFW.
#include <glfw3.h>

#include "Console.h"

#include <algorithm>
#include <cmath>
#include <optional>
#include <vector>

namespace {

constexpr float MOVE_STEP = 0.04f;
constexpr float DIAGONAL_STEP = 0.02828427f; // MOVE_STEP / sqrt(2)
constexpr float INNER_BOUND = 0.999f;

VertexBounds boundsAt(const Diagram& diagram, Point pivot, float degrees) {
    VertexBounds bounds;

    switch (diagram.getBoundCase()) {//작성해야 함
    case BoundsCase::Fit:
        bounds.setFit();
        break;
    case BoundsCase::Unfit:
        bounds.setUnfit();
        break;
    default:
    }
    

    for (Point vertex : diagram.worldVerticesAt(pivot, degrees)) {
        bounds.left = std::min(bounds.left, vertex.x);
        bounds.right = std::max(bounds.right, vertex.x);
        bounds.bottom = std::min(bounds.bottom, vertex.y);
        bounds.top = std::max(bounds.top, vertex.y);
    }
    return bounds;
}

Point clampPivot(const Diagram& diagram, Point pivot, float degrees) {
    const VertexBounds bounds = boundsAt(diagram, pivot, degrees);
    if (bounds.left < -INNER_BOUND) {
        pivot.x += -INNER_BOUND - bounds.left;
    }
    else if (bounds.right > INNER_BOUND) {
        pivot.x += INNER_BOUND - bounds.right;
    }
    if (bounds.bottom < -INNER_BOUND) {
        pivot.y += -INNER_BOUND - bounds.bottom;
    }
    else if (bounds.top > INNER_BOUND) {
        pivot.y += INNER_BOUND - bounds.top;
    }
    return pivot;
}



Diagram makeDiagram(GLShape shape, TriangleCase case_) {
    const Point pivot{//...? 원점이어야 하지 않을까
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
        return Diagram(shape, setRandomTriPointByCase(case_), color, pivot);
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



explicit PracticeConsole::PracticeConsole(GLFWwindow* window) : window_(window) {
    //store_.push(GLShape shape, const std::vector<Point>&localVertices,
    //    GlColor color, Point pivot, Point localPivot);
}

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

void PracticeConsole::onKey(int key, int action) {
    if (handleQuit(key, action)) {
        return;
    }
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }

    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_R: store_.resetAtRandom(); return;
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

void PracticeConsole::onMouseButton(int button, int action, Point point) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        store_.selectAt(point);
    }
}
