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
constexpr float PI = 3.14159265358979323846f;
constexpr float INNER_BOUND = 0.999f;

struct VertexBounds {
    float left = 1.0f;
    float right = -1.0f;
    float bottom = 1.0f;
    float top = -1.0f;
};

VertexBounds boundsAt(const Diagram& diagram, Point pivot, float degrees) {
    VertexBounds bounds;

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
    } else if (bounds.right > INNER_BOUND) {
        pivot.x += INNER_BOUND - bounds.right;
    }

    if (bounds.bottom < -INNER_BOUND) {
        pivot.y += -INNER_BOUND - bounds.bottom;
    } else if (bounds.top > INNER_BOUND) {
        pivot.y += INNER_BOUND - bounds.top;
    }

    return pivot;
}

float headingDegrees(Point velocity) {
    // The local triangle tip points upward before rotation.
    return std::atan2(-velocity.x, velocity.y) * 180.0f / PI;
}

int practice8Quadrant(Point point) {
    if (point.x > 0.0f && point.y > 0.0f)
        return 0;

    if (point.x < 0.0f && point.y > 0.0f)
        return 1;

    if (point.x < 0.0f && point.y < 0.0f)
        return 2;

    if (point.x > 0.0f && point.y < 0.0f)
        return 3;

    return -1;
}

float practice8MaxHalf(Point center) {
    const float horizontalMargin = std::min(std::abs(center.x), 1.0f - std::abs(center.x));
    const float verticalMargin = std::min(std::abs(center.y), 1.0f - std::abs(center.y));

    return std::min(0.20f, 0.8f * std::min(horizontalMargin, verticalMargin));
}

bool practice8FitsBounds(const Diagram& triangle, int quadrant) {
    if (!triangle.fitsNdc()) {
        return false;
    }

    for (Point vertex : triangle.worldVertices()) {
        if (practice8Quadrant(vertex) != quadrant) {
            return false;
        }
    }

    return true;
}

Point randomPractice8Center(int quadrant, std::optional<Point> previous) {
    const float x = randomFloat(0.22f, 0.78f);
    const float y = randomFloat(0.22f, 0.78f);
    Point center;

    switch (quadrant) {
    case 0:
        center = {x, y};
        break;
    case 1:
        center = {-x, y};
        break;
    case 2:
        center = {-x, -y};
        break;
    default:
        center = {x, -y};
        break;
    }

    if (previous && std::abs(center.x - previous->x) + std::abs(center.y - previous->y) < 0.05f) {
        const float newX = std::abs(previous->x) < 0.5f ? 0.78f : 0.22f;

        center.x = quadrant == 0 || quadrant == 3 ? newX : -newX;
    }

    return center;
}

std::optional<Diagram> makePractice8Triangle(Point center) {
    const int quadrant = practice8Quadrant(center);
    const float maximum = practice8MaxHalf(center);

    if (quadrant < 0 || maximum <= 0.0f) {
        return std::nullopt;
    }

    GlColor color;

    color.setRandom();

    Diagram triangle(GLShape::Triangle, std::vector<Point>{{-1.0f, -1.0f}, {1.0f, -1.0f}, {0.0f, 1.0f}}, color, center,
                     Point{0.0f, 0.0f});
    const float minimum = std::min(0.04f, maximum * 0.5f);
    const float half = randomFloat(minimum, maximum);

    if (!triangle.setScale({half, half}) || !practice8FitsBounds(triangle, quadrant)) {
        return std::nullopt;
    }

    return triangle;
}

std::optional<Diagram> makePractice9Triangle(Point center) {
    const float margin = std::min(1.0f - std::abs(center.x), 1.0f - std::abs(center.y));
    const float maximum = std::min(0.15f, 0.8f * margin);

    if (maximum <= 0.0f) {
        return std::nullopt;
    }

    GlColor color;

    color.setRandom();

    Diagram triangle(GLShape::Triangle, std::vector<Point>{{-1.0f, -1.0f}, {1.0f, -1.0f}, {0.0f, 1.0f}}, color, center,
                     Point{0.0f, 0.0f});
    const float minimum = std::min(0.04f, maximum * 0.5f);
    const float half = randomFloat(minimum, maximum);

    if (!triangle.setScale({half, half})) {
        return std::nullopt;
    }

    return triangle;
}

Diagram makePractice7Diagram(GLShape shape) {
    const Point pivot{randomFloat(-0.75f, 0.75f), randomFloat(-0.75f, 0.75f)};
    GlColor color;

    color.setRandom();

    const float half = randomFloat(0.08f, 0.17f);

    switch (shape) {
    case GLShape::Spot:
        return Diagram(shape, std::vector<Point>{{-0.02f, -0.02f}, {0.02f, -0.02f}, {-0.02f, 0.02f}, {0.02f, 0.02f}},
                       color, pivot);
    case GLShape::Line:
        return Diagram(shape, std::vector<Point>{{-half, 0.0f}, {half, 0.0f}}, color, pivot);
    case GLShape::Triangle:
        return Diagram(shape, std::vector<Point>{{-half, -half}, {half, -half}, {0.0f, half}}, color, pivot);
    case GLShape::Rect:
        return Diagram(shape, std::vector<Point>{{-half, -half}, {half, -half}, {-half, half}, {half, half}}, color,
                       pivot);
    }

    return Diagram(GLShape::Triangle, std::vector<Point>{{-half, -half}, {half, -half}, {0.0f, half}}, color, pivot);
}

} // namespace

bool PracticeConsole::initGraphics(const std::filesystem::path& shaderDirectory) {
    if (!shader_.initShader(shaderDirectory / "VertexShader.glsl", shaderDirectory / "FragmentShader.glsl")) {
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
        case GLFW_KEY_P:
            store_.push(makePractice7Diagram(GLShape::Spot));

            return;
        case GLFW_KEY_E:
            store_.push(makePractice7Diagram(GLShape::Line));

            return;
        case GLFW_KEY_T:
            store_.push(makePractice7Diagram(GLShape::Triangle));

            return;
        case GLFW_KEY_R:
            store_.push(makePractice7Diagram(GLShape::Rect));

            return;
        case GLFW_KEY_C:
            store_.clear();

            return;
        default:
            break;
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
}

void Practice7Console::onMouseButton(int button, int action, Point point) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        store_.selectAt(point);
    }
}

Practice8Console::Practice8Console(GLFWwindow* window) : PracticeConsole(window) {
    reset();
}

Practice8Console::Command Practice8Console::parseKey(int key, int action) const {
    if (action != GLFW_PRESS) {
        return Command::None;
    }

    switch (key) {
    case GLFW_KEY_A:
        return Command::Fill;
    case GLFW_KEY_B:
        return Command::Outline;
    case GLFW_KEY_EQUAL:
    case GLFW_KEY_KP_ADD:
        return Command::Grow;
    case GLFW_KEY_MINUS:
    case GLFW_KEY_KP_SUBTRACT:
        return Command::Shrink;
    case GLFW_KEY_C:
        return Command::Reset;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        return Command::Quit;
    default:
        return Command::None;
    }
}

void Practice8Console::input(Command command) {
    switch (command) {
    case Command::Fill:
        outline_ = false;
        break;
    case Command::Outline:
        outline_ = true;
        break;
    case Command::Grow:
        resizeStep_ = 0.02f;
        break;
    case Command::Shrink:
        resizeStep_ = -0.02f;
        break;
    case Command::Reset:
        reset();
        break;
    case Command::Quit:
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
        break;
    default:
        break;
    }
}

void Practice8Console::onKey(int key, int action) {
    input(parseKey(key, action));
}

bool Practice8Console::replaceItems(const std::vector<Diagram>& items) {
    if (items.size() != 6 || items[0].getShape() != GLShape::Line || items[1].getShape() != GLShape::Line ||
        !items[0].fitsNdc() || !items[1].fitsNdc()) {
        return false;
    }

    for (int quadrant = 0; quadrant < 4; ++quadrant) {
        const Diagram& triangle = items[2 + quadrant];

        if (triangle.getShape() != GLShape::Triangle || !practice8FitsBounds(triangle, quadrant)) {
            return false;
        }
    }

    store_.clear();

    for (const Diagram& item : items) {
        store_.push(item);
    }

    return true;
}

void Practice8Console::reset() {
    GlColor axisColor;

    axisColor.set(0.85f, 0.85f, 0.85f);

    std::vector<Diagram> items;

    items.emplace_back(GLShape::Line, std::vector<Point>{{-1.0f, 0.0f}, {1.0f, 0.0f}}, axisColor, Point{});
    items.emplace_back(GLShape::Line, std::vector<Point>{{0.0f, -1.0f}, {0.0f, 1.0f}}, axisColor, Point{});

    for (int quadrant = 0; quadrant < 4; ++quadrant) {
        std::optional<Point> previous;

        if (store_.getItems().size() == 6) {
            previous = store_.getItems()[2 + quadrant].getPivot();
        }

        const std::optional<Diagram> triangle = makePractice8Triangle(randomPractice8Center(quadrant, previous));

        if (!triangle) {
            return;
        }
        items.push_back(*triangle);
    }
    replaceItems(items);
}

void Practice8Console::onMouseButton(int button, int action, Point point) {
    if (action != GLFW_PRESS) {
        return;
    }

    const int quadrant = practice8Quadrant(point);

    if (quadrant < 0) {
        return;
    }

    std::vector<Diagram> items = store_.getItems();
    const std::size_t index = 2 + static_cast<std::size_t>(quadrant);

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        const std::optional<Diagram> replacement = makePractice8Triangle(point);

        if (!replacement) {
            return;
        }
        items[index] = *replacement;
        replaceItems(items);
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        Diagram& triangle = items[index];
        const float maximum = practice8MaxHalf(triangle.getPivot());
        const float minimum = std::min(0.04f, maximum * 0.25f);
        const std::vector<Point> vertices = triangle.worldVertices();
        const float current = (vertices[1].x - vertices[0].x) * 0.5f;
        const float next = std::clamp(current + resizeStep_, minimum, maximum);

        if (next != current && triangle.setScale({next, next}) && practice8FitsBounds(triangle, quadrant)) {
            replaceItems(items);
        }
    }
}

bool Practice8Console::render() {
    glPolygonMode(GL_FRONT_AND_BACK, outline_ ? GL_LINE : GL_FILL);

    const bool rendered = PracticeConsole::render();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    return rendered;
}

Practice9Console::Practice9Console(GLFWwindow* window) : PracticeConsole(window) {
    reset();
}

void Practice9Console::reset() {
    store_.clear();
    motions_.clear();

    for (int i = 0; i < 4; ++i) {
        const Point center{randomFloat(-0.70f, 0.70f), randomFloat(-0.70f, 0.70f)};
        const std::optional<Diagram> triangle = makePractice9Triangle(center);

        if (triangle && store_.push(*triangle)) {
            motions_.push_back({});
            resetMotion(motions_.size() - 1);
        }
    }
}

void Practice9Console::resetMotion(std::size_t triangleIndex) {
    const std::vector<Diagram>& items = store_.getItems();

    if (triangleIndex >= motions_.size() || triangleIndex >= items.size()) {
        return;
    }

    Motion& motion = motions_[triangleIndex];

    motion.anchor = items[triangleIndex].getPivot();
    motion.speed = 0.26f + 0.02f * static_cast<float>(triangleIndex);

    motion.angle = mode_ == Mode::HorizontalZigzag ? 0.0f : static_cast<float>(triangleIndex) * PI * 0.5f;

    motion.zigzagHorMov = true;
    motion.zigSignY = 1.0f;
    motion.stepYcnt = 0;
    motion.angularSpeed = 1.5f + 0.04f * static_cast<float>(triangleIndex);
    motion.radius = 0.0f;
    motion.radialSpeed = 0.12f + 0.0005f * static_cast<float>(triangleIndex);
    motion.radialDirection = 1.0f;

    const std::size_t directionIndex = triangleIndex % 4;
    const float xSign = directionIndex == 0 || directionIndex == 3 ? 1.0f : -1.0f;
    const float ySign = directionIndex < 2 ? 1.0f : -1.0f;

    if (mode_ == Mode::HorizontalZigzag) {
        motion.velocity = {xSign * motion.speed, 0};
    } else if (mode_ == Mode::VerticalZigzag) {
        motion.velocity = {xSign * 0.25f * motion.speed, ySign * 0.9682458f * motion.speed};
    } else {
        motion.velocity = {xSign * 0.7071068f * motion.speed, ySign * 0.7071068f * motion.speed};
    }
}

void Practice9Console::selectMode(Mode mode) {
    mode_ = mode;

    for (std::size_t i = 0; i < motions_.size(); ++i) {
        resetMotion(i); // Capture each triangle's current pivot for this mode.
    }
}

void Practice9Console::onKey(int key, int action) {
    if (handleQuit(key, action) || action != GLFW_PRESS) {
        return;
    }

    switch (key) {
    case GLFW_KEY_1:
        selectMode(Mode::Bounce);
        break;
    case GLFW_KEY_2:
        selectMode(Mode::HorizontalZigzag);
        break;
    case GLFW_KEY_3:
        selectMode(Mode::VerticalZigzag);
        break;
    case GLFW_KEY_4:
        selectMode(Mode::Spiral);
        break;
    case GLFW_KEY_A:
        outline_ = false;
        break;
    case GLFW_KEY_B:
        outline_ = true;
        break;
    case GLFW_KEY_EQUAL:
    case GLFW_KEY_KP_ADD:
        resizeStep_ = 0.02f;
        break;
    case GLFW_KEY_MINUS:
    case GLFW_KEY_KP_SUBTRACT:
        resizeStep_ = -0.02f;
        break;
    case GLFW_KEY_C:
        reset();
        break;
    default:
        break;
    }
}

void Practice9Console::onMouseButton(int button, int action, Point point) {
    if (action != GLFW_PRESS) {
        return;
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        const std::optional<Diagram> triangle = makePractice9Triangle(point);

        if (triangle && store_.push(*triangle)) {
            motions_.push_back({});
            resetMotion(motions_.size() - 1);
        }
    } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        const std::vector<Diagram>& items = store_.getItems();

        for (std::size_t i = items.size(); i > 0; --i) {
            if (!items[i - 1].contains(point)) {
                continue;
            }

            Diagram resized = items[i - 1];
            const float current = resized.getScale().x;
            const float next = std::clamp(current + resizeStep_, 0.02f, 0.20f);

            if (next != current && resized.setScale({next, next}) && store_.replaceAt(i - 1, resized)) {
                resetMotion(i - 1);
            }
            break;
        }
    }
}

bool Practice9Console::render() {
    glPolygonMode(GL_FRONT_AND_BACK, outline_ ? GL_LINE : GL_FILL);

    const bool rendered = PracticeConsole::render();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    return rendered;
}

constexpr float HORIZON_STEP_DURATION = 10.0f;
constexpr float HORIZON_STEP = 5.0f;

void Practice9Console::advanceLinear(std::size_t triangleIndex, float deltaTime) {
    const std::size_t storeIndex = triangleIndex;
    const Diagram current = store_.getItems()[storeIndex];
    Motion& motion = motions_[triangleIndex];
    const Point start = current.getPivot();

    if (mode_ == Mode::HorizontalZigzag) {
        Point nextPivot = start;
        float rotation = headingDegrees({motion.velocity.x, 0.0f});

        if (motion.zigzagHorMov) {
            nextPivot.x += motion.velocity.x * deltaTime;

            const VertexBounds proposed = boundsAt(current, nextPivot, rotation);

            if (proposed.left < -INNER_BOUND || proposed.right > INNER_BOUND) {
                motion.velocity.x = -motion.velocity.x;
                motion.zigzagHorMov = false;
                motion.stepYcnt = 0;
                nextPivot.x = start.x;
            }
        }

        if (not motion.zigzagHorMov) {
            motion.velocity.y = motion.zigSignY * HORIZON_STEP / HORIZON_STEP_DURATION;
            nextPivot.y += motion.velocity.y * deltaTime;
            rotation = headingDegrees({0.0f, motion.velocity.y});

            const VertexBounds stepped = boundsAt(current, nextPivot, rotation);

            if (stepped.top > INNER_BOUND || stepped.bottom < -INNER_BOUND) {
                motion.zigSignY = -motion.zigSignY;
                motion.velocity.y = motion.zigSignY * HORIZON_STEP / HORIZON_STEP_DURATION;
                nextPivot.y = start.y + motion.velocity.y * 2 * deltaTime;
                rotation = headingDegrees({0.0f, motion.velocity.y});
            }

            ++motion.stepYcnt;

            if (motion.stepYcnt == HORIZON_STEP_DURATION) {
                motion.zigzagHorMov = true;
                motion.stepYcnt = 0;
                motion.velocity.y = 0.0f;
                rotation = headingDegrees({motion.velocity.x, 0.0f});
            }
        }

        nextPivot = clampPivot(current, nextPivot, rotation);

        Diagram updated = current;

        if (updated.setPose(nextPivot, rotation)) {
            store_.replaceAt(storeIndex, updated);
        }

        return;
    }

    Point nextPivot{start.x + motion.velocity.x * deltaTime, start.y + motion.velocity.y * deltaTime};
    float rotation = headingDegrees(motion.velocity);
    const VertexBounds proposed = boundsAt(current, nextPivot, rotation);
    const bool hitX = proposed.left < -1.0f || proposed.right > 1.0f;
    const bool hitY = proposed.bottom < -1.0f || proposed.top > 1.0f;

    if (mode_ == Mode::VerticalZigzag && (hitX || hitY)) {
        if (hitY) {
            const float inwardY = proposed.bottom < -1.0f ? 1.0f : -1.0f;
            const float side = motion.velocity.x < 0.0f ? -1.0f : 1.0f;
            const float angle = 15.0f * PI / 180.0f; //before: randomFloat(0.0f, 20.0f)

            motion.velocity = {side * std::sin(angle) * motion.speed, inwardY * std::cos(angle) * motion.speed};
        } else {

            const float inwardX = proposed.left < -1.0f ? 1.0f : -1.0f;
            const float up = motion.velocity.y < 0.0f ? -1.0f : 1.0f;

            const float angle = randomFloat(65.0f, 80.0f) * PI / 180.0f;

            motion.velocity = {inwardX * std::cos(angle) * motion.speed, up * std::sin(angle) * motion.speed};
        }
    } else {

        if (hitX) {

            motion.velocity.x = -motion.velocity.x;
            motion.velocity.y = motion.velocity.y;
        }

        if (hitY) {
            motion.velocity.y = -motion.velocity.y;
        }
    }

    if (hitX || hitY) {
        nextPivot = {start.x + motion.velocity.x * deltaTime, start.y + motion.velocity.y * deltaTime};
    }

    rotation = headingDegrees(motion.velocity);

    nextPivot = clampPivot(current, nextPivot, rotation);

    Diagram updated = current;

    if (updated.setPose(nextPivot, rotation)) {
        store_.replaceAt(storeIndex, updated);
    }
}

void Practice9Console::advanceSpiral(std::size_t triangleIndex, float deltaTime) {
    const std::size_t storeIndex = triangleIndex;
    const Diagram current = store_.getItems()[storeIndex];
    Motion& motion = motions_[triangleIndex];
    const float previousRadius = motion.radius;

    motion.angle = std::fmod(motion.angle + motion.angularSpeed * deltaTime * 1.25f, 2.0f * PI);
    motion.radius += motion.radialDirection * motion.radialSpeed * deltaTime * 0.5f;

    if (motion.radius <= 0.0f) {
        motion.radius = 0.0f;
        motion.radialDirection = 1.0f;
    }

    const float rotation = motion.angle * 180.0f / PI;
    Point nextPivot{motion.anchor.x + motion.radius * std::sin(motion.angle),
                    motion.anchor.y + motion.radius * std::cos(motion.angle)};
    VertexBounds proposed = boundsAt(current, nextPivot, rotation);

    if (proposed.left < -1.0f || proposed.right > 1.0f || proposed.bottom < -1.0f || proposed.top > 1.0f) {
        motion.radialDirection = -1.0f;
        motion.radius = std::max(0.0f, previousRadius - motion.radialSpeed * deltaTime * 0.5f);
        nextPivot = {motion.anchor.x + motion.radius * std::sin(motion.angle),
                     motion.anchor.y + motion.radius * std::cos(motion.angle)};
    }
    nextPivot = clampPivot(current, nextPivot, rotation);

    Diagram updated = current;

    if (updated.setPose(nextPivot, rotation)) {
        store_.replaceAt(storeIndex, updated);
    }
}

void Practice9Console::update(float deltaTime) {
    if (deltaTime <= 0.0f || store_.getItems().size() != motions_.size()) {
        return;
    }

    for (std::size_t i = 0; i < motions_.size(); ++i) {
        if (mode_ == Mode::Spiral) {
            advanceSpiral(i, deltaTime);
        } else {
            advanceLinear(i, deltaTime);
        }
    }
}
