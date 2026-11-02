#include <glew.h>
#include <glfw3.h>

#include "Console.h"

namespace {

constexpr float MOVE_STEP = 0.04f;
constexpr float DIAGONAL_STEP = 0.02828427f;

} // namespace

PracticeConsole::PracticeConsole(GLFWwindow* window) : window_(window) {
}

bool PracticeConsole::initGraphics(const std::filesystem::path& shaderDirectory) {
    updateWindowSize();

    if (!store_.resetAtRandom()) {
        std::cerr << "Initial board generation failed.\n";

        return false;
    }

    if (!shader_.initShader(shaderDirectory / "VertexShader.glsl", shaderDirectory / "FragmentShader.glsl")) {
        return false;
    }

    if (!shader_.initBuffer()) {
        shader_.destroy();

        return false;
    }
    updateTitle();
    std::cout << "Drag pieces to the boards. R: reset, C: clear, Q/Esc: quit.\n"
              << "WASD/IJKL: move selected piece, 1-4: move all loose pieces.\n";

    return true;
}

void PracticeConsole::shutdownGraphics() {
    shader_.destroy();
}

void PracticeConsole::updateWindowSize() {
    int width = 0;
    int height = 0;

    glfwGetWindowSize(window_, &width, &height);
    store_.setWindowSize(width, height);
}

void PracticeConsole::updateTitle() {
    const std::string title = "OpenGL Practice 10 - " + std::to_string(store_.getCompletedBoardCount()) + "/" +
                              std::to_string(store_.getBoards().size()) + " boards complete";

    glfwSetWindowTitle(window_, title.c_str());
}

void PracticeConsole::cancelDrag() {
    dragging_ = false;
    dragOffset_ = {};
}

bool PracticeConsole::handleQuit(int key, int action) {
    if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q)) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);

        return true;
    }

    return false;
}

bool PracticeConsole::render() {
    updateWindowSize();

    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window_, &width, &height);

    if (width <= 0 || height <= 0) {
        return true; // 최소화된 창에는 그리지 않는다.
    }
    // 좌측 조각 영역 60%, 우측 모양판 영역 40%. 추가 정점은 필요 없다.
    glClearColor(0.12f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    const int splitPixel = static_cast<int>((REGION_SPLIT_X + 1.0f) * 0.5f * width);

    glEnable(GL_SCISSOR_TEST);
    glScissor(splitPixel, 0, width - splitPixel, height);
    glClearColor(0.18f, 0.18f, 0.22f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glScissor(splitPixel, 0, 1, height);
    glClearColor(0.5f, 0.5f, 0.55f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_SCISSOR_TEST);

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
        if (key == GLFW_KEY_R) {
            if (store_.resetAtRandom()) {
                cancelDrag();
                updateTitle();
            } else {
                std::cerr << "Reset failed; the current boards were preserved.\n";
            }

            return;
        }

        if (key == GLFW_KEY_C) {
            cancelDrag();
            store_.clear();
            updateTitle();

            return;
        }
    }

    switch (key) {
    case GLFW_KEY_W:
        store_.translateSelected({0.0f, MOVE_STEP});
        break;
    case GLFW_KEY_A:
        store_.translateSelected({-MOVE_STEP, 0.0f});
        break;
    case GLFW_KEY_S:
        store_.translateSelected({0.0f, -MOVE_STEP});
        break;
    case GLFW_KEY_D:
        store_.translateSelected({MOVE_STEP, 0.0f});
        break;
    case GLFW_KEY_I:
        store_.translateSelected({-DIAGONAL_STEP, DIAGONAL_STEP});
        break;
    case GLFW_KEY_J:
        store_.translateSelected({DIAGONAL_STEP, DIAGONAL_STEP});
        break;
    case GLFW_KEY_K:
        store_.translateSelected({-DIAGONAL_STEP, -DIAGONAL_STEP});
        break;
    case GLFW_KEY_L:
        store_.translateSelected({DIAGONAL_STEP, -DIAGONAL_STEP});
        break;
    case GLFW_KEY_1:
        store_.translateAll({-MOVE_STEP, 0.0f});
        break;
    case GLFW_KEY_2:
        store_.translateAll({MOVE_STEP, 0.0f});
        break;
    case GLFW_KEY_3:
        store_.translateAll({0.0f, MOVE_STEP});
        break;
    case GLFW_KEY_4:
        store_.translateAll({0.0f, -MOVE_STEP});
        break;
    default:
        break;
    }

    if (dragging_) {
        // 드래그 도중 키보드로 이동해도 다음 커서 입력에서 되돌아가지 않는다.
        const std::optional<Point> pivot = store_.getSelectedPivot();

        if (pivot) {
            dragOffset_ = {pivot->x - lastCursor_.x, pivot->y - lastCursor_.y};
        } else {
            cancelDrag();
        }
    }
}

void PracticeConsole::onMouseButton(int button, int action, Point point) {
    if (button != GLFW_MOUSE_BUTTON_LEFT) {
        return;
    }

    if (action == GLFW_PRESS) {
        cancelDrag();
        store_.selectAt(point);

        const std::optional<Point> pivot = store_.getSelectedPivot();

        if (pivot) {
            dragging_ = true;
            lastCursor_ = point;
            dragOffset_ = {pivot->x - point.x, pivot->y - point.y};
        }
    } else if (action == GLFW_RELEASE && dragging_) {
        onCursorMove(point); // 마지막 커서 위치도 먼저 반영한다.
        updateWindowSize();

        if (store_.mergeSelectedDiagram()) {
            updateTitle();
        }
        cancelDrag();
        // 결합 실패 시 위치와 선택은 유지한다. 키보드로도 이동할 수 있다.
    }
}

void PracticeConsole::onCursorMove(Point point) {
    lastCursor_ = point;

    if (dragging_) {
        store_.moveSelectedTo({point.x + dragOffset_.x, point.y + dragOffset_.y});
    }
}
