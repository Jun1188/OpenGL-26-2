#include <glew.h> // GLEW must be included before GLFW.
#include <glfw3.h>

#include "Console.h"

namespace {

constexpr float SCALE_STEP = 0.05f;

enum class OutlineMode {
    SelectedOnly,
    EveryRectangle
};

template <typename State>
void renderRects(const State& state, OutlineMode outlineMode) {
    const std::vector<AnimatedRect>& rects = state.getItems();
    const std::vector<std::size_t> renderOrder = state.getRenderOrder();

    for (std::size_t index : renderOrder) {
        renderFilledRect(rects[index].rect);
        if (outlineMode == OutlineMode::EveryRectangle) {
            renderRectOutline(rects[index].rect, { 0.15f, 0.15f, 0.15f }, 1.0f);
        }
    }
}

} // namespace

void BaseColorState::setRandomSections() {
    section1Color.setRandom();
    section2Color.setRandom();
    section3Color.setRandom();
    section4Color.setRandom();
}

Practice2Console::Practice2Console(GLFWwindow* window)
    : window_(window) {
    baseColorState_.setRandomSections();
}

Practice2Console::Command Practice2Console::parseKey(int key, int action) const {
    if (action != GLFW_PRESS) {
        return Command::None;
    }

    switch (key) {
    case GLFW_KEY_1:
        return Command::AddSection1;
    case GLFW_KEY_2:
        return Command::AddSection2;
    case GLFW_KEY_3:
        return Command::AddSection3;
    case GLFW_KEY_4:
        return Command::AddSection4;
    case GLFW_KEY_EQUAL:
    case GLFW_KEY_KP_ADD:
        return Command::ScaleUp;
    case GLFW_KEY_MINUS:
    case GLFW_KEY_KP_SUBTRACT:
        return Command::ScaleDown;
    case GLFW_KEY_C:
        return Command::Recolor;
    case GLFW_KEY_R:
        return Command::Reset;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        return Command::Quit;
    default:
        return Command::None;
    }
}

void Practice2Console::input(Command command) {
    switch (command) {
    case Command::AddSection1:
        rectState_.push(Section::Section1);
        break;
    case Command::AddSection2:
        rectState_.push(Section::Section2);
        break;
    case Command::AddSection3:
        rectState_.push(Section::Section3);
        break;
    case Command::AddSection4:
        rectState_.push(Section::Section4);
        break;
    case Command::ScaleUp:
        rectState_.scaleSelected(SCALE_STEP);
        break;
    case Command::ScaleDown:
        rectState_.scaleSelected(-SCALE_STEP);
        break;
    case Command::Recolor:
        rectState_.recolorSelected();
        break;
    case Command::Reset:
        rectState_.removeAll();
        baseColorState_.setRandomSections();
        break;
    case Command::Quit:
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
        break;
    default:
        break;
    }
}

void Practice2Console::onKey(int key, int action) {
    input(parseKey(key, action));
}

void Practice2Console::onMouseButton(int button, int action, const Point& point) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        rectState_.select(point);
    }
}

void Practice2Console::onCursorMove(const Point&) {
}

void Practice2Console::render() const {
    const GlColor& background = baseColorState_.backgroundColor;
    glClearColor(background.red, background.green, background.blue, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderBase();
    renderRects(rectState_, OutlineMode::SelectedOnly);

    const Rect* selected = rectState_.getSelected();
    if (selected != nullptr) {
        renderRectOutline(*selected, { 1.0f, 1.0f, 1.0f }, 4.0f);
    }
}

void Practice2Console::renderBase() const {
    setOpenGlColor(baseColorState_.section1Color);
    glRectf(0.0f, 0.0f, 1.0f, 1.0f);

    setOpenGlColor(baseColorState_.section2Color);
    glRectf(-1.0f, 0.0f, 0.0f, 1.0f);

    setOpenGlColor(baseColorState_.section3Color);
    glRectf(-1.0f, -1.0f, 0.0f, 0.0f);

    setOpenGlColor(baseColorState_.section4Color);
    glRectf(0.0f, -1.0f, 1.0f, 0.0f);

    glLineWidth(1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
    glVertex2f(-1.0f, 0.0f);
    glVertex2f(1.0f, 0.0f);
    glVertex2f(0.0f, -1.0f);
    glVertex2f(0.0f, 1.0f);
    glEnd();
}

Practice3Console::Practice3Console(GLFWwindow* window)
    : window_(window) {
}

Practice3Console::Command Practice3Console::parseKey(int key, int action) const {
    if (action != GLFW_PRESS) {
        return Command::None;
    }

    switch (key) {
    case GLFW_KEY_A:
        return Command::AddRandomRect;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        return Command::Quit;
    default:
        return Command::None;
    }
}

void Practice3Console::input(Command command) {
    switch (command) {
    case Command::AddRandomRect:
        rectState_.pushRandom();
        break;
    case Command::Quit:
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
        break;
    default:
        break;
    }
}

void Practice3Console::onKey(int key, int action) {
    input(parseKey(key, action));
}

void Practice3Console::onMouseButton(int button, int action, const Point& point) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            rectState_.beginDrag(point);
        }
        else if (action == GLFW_RELEASE) {
            rectState_.endDrag();
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        rectState_.splitAt(point);
    }
}

void Practice3Console::onCursorMove(const Point& point) {
    rectState_.dragTo(point);
}

void Practice3Console::render() const {
    glClearColor(0.94f, 0.94f, 0.94f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    renderRects(rectState_, OutlineMode::EveryRectangle);

    if (rectState_.isDragging()) {
        const Rect* selected = rectState_.getSelected();
        if (selected != nullptr) {
            renderRectOutline(*selected, { 1.0f, 1.0f, 1.0f }, 4.0f);
        }
    }
}


Practice4Console::Command Practice4Console::parseKey(int key, int action) const {
    if (action != GLFW_PRESS) {
        return Command::None;
    }

    switch (key) {
    case GLFW_KEY_1:
        return Command::Diagonal;
    case GLFW_KEY_2:
        return Command::HorizontalZigzag;
    case GLFW_KEY_3:
        return Command::Orbit;
    case GLFW_KEY_4:
        return Command::Resize;
    case GLFW_KEY_5:
        return Command::Recolor;
    case GLFW_KEY_S:
        return Command::Stop;
    case GLFW_KEY_M:
        return Command::Rollback;
    case GLFW_KEY_R:
        return Command::Reset;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        return Command::Quit;
    case GLFW_KEY_P:
        return Command::Print;
    default:
        return Command::None;
    }
}
void Practice4Console::input(Practice4Console::Command command) {
    switch (command) {
    case Command::Diagonal:
        rectState_.toggleDiagonal();
        break;
    case Command::HorizontalZigzag:
        rectState_.toggleHorizontalZigzag();
        break;
    case Command::Orbit:
        rectState_.toggleOrbit();
        break;
    case Command::Resize:
        rectState_.toggleResize();
        break;
    case Command::Recolor:
        rectState_.toggleRecolor();
        break;
    case Command::Stop:
        rectState_.stopAll();
        break;
    case Command::Rollback:
        rectState_.rollbackAtCapture();
        break;
    case Command::Reset:
        rectState_.reset();
        break;
    case Command::Quit:
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
        break;
    case Command::Print:
        rectState_.printFlag();
        break;
    case Command::None:
    default:
        break;
    }
}

Practice4Console::Practice4Console(GLFWwindow* window)
    : window_(window) {
}

void Practice4Console::update(float deltaTime) {
    rectState_.update(deltaTime);
}

void Practice4Console::onKey(int key, int action) {
    input(parseKey(key, action));
}

void Practice4Console::onCursorMove(const Point&) {
}

void Practice4Console::onMouseButton(int button, int action, const Point& point) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        rectState_.tryCreateRectAt(point);
    }
}

void Practice4Console::render() const {
    glClearColor(0.18f, 0.18f, 0.18f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const AnimatedRect& item : rectState_.getItems()) {
        renderFilledRect(item.rect);
    }
}




//start practice 5

Practice5Console::Practice5Console(GLFWwindow* window)
    : window_(window) {
}

Practice5Console::Command Practice5Console::parseKey(int key, int action) const {
    if (action != GLFW_PRESS) {
        return Command::None;
    }

    switch (key) {
    case GLFW_KEY_R:
        return Command::Reset;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        return Command::Quit;
    default:
        return Command::None;
    }
}

void Practice5Console::input(Command command) {
    switch (command) {
    case Command::Reset:
        rectState_.reset();
        break;
    case Command::Quit:
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
        break;
    default:
        break;
    }
}

void Practice5Console::onKey(int key, int action) {
    input(parseKey(key, action));
}

void Practice5Console::onMouseButton(int button, int action, const Point& point) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            rectState_.beginDrag(point);
        }
        else if (action == GLFW_RELEASE) {
            rectState_.endDrag();
        }
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            rectState_.tryCreateRectAt(point);
        }
    }
    
}

void Practice5Console::onCursorMove(const Point& point) {
    rectState_.dragTo(point);
}

void Practice5Console::render() const {
    glClearColor(0.94f, 0.94f, 0.94f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (const AnimatedRect& item : rectState_.getItems())
        renderFilledRect(item.rect);
    if(rectState_.getEraser() != std::nullopt)
        renderFilledRect(rectState_.getEraser().value());
    
}


