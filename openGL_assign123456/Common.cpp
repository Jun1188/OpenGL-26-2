#include <glew.h> // GLEW must be included before GLFW.
#include <glfw3.h>

#include "Common.h"

#include <algorithm>
#include <random>

namespace {

std::random_device randomDevice;
std::default_random_engine randomEngine(randomDevice());

} // namespace

float randomFloat(float minimum, float maximum) {
    std::uniform_real_distribution<float> distribution(minimum, maximum);

    return distribution(randomEngine);
}

size_t randomIndex(size_t minSize, size_t maxSize) {
    std::uniform_int_distribution<size_t> distribution(minSize, maxSize);

    return distribution(randomEngine);
}

bool randomBool() {
    std::uniform_int_distribution<int> distribution(0, 1);

    return distribution(randomEngine) == 1;
}

void GlColor::set(float r, float g, float b) {
    red = r;
    green = g;
    blue = b;
}

void GlColor::setRandom() {
    set(randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f), randomFloat(0.0f, 1.0f));
}

Rect::Rect(const Point& first, const Point& second, const GlColor& color)
    : minimum_{std::min(first.x, second.x), std::min(first.y, second.y)},
      maximum_{std::max(first.x, second.x), std::max(first.y, second.y)}, color_(color) {
}

void Rect::resetAssignedColorAt(const GlColor gl) {
    color_ = gl;
}

Rect Rect::fromCenter(const Point& center, float halfWidth, float halfHeight, const GlColor& color) {
    return Rect({center.x - halfWidth, center.y - halfHeight}, {center.x + halfWidth, center.y + halfHeight}, color);
}

const Point& Rect::getMinimum() const {
    return minimum_;
}

const Point& Rect::getMaximum() const {
    return maximum_;
}

const GlColor& Rect::getColor() const {
    return color_;
}

float Rect::getLeft() const {
    return minimum_.x;
}

float Rect::getRight() const {
    return maximum_.x;
}

float Rect::getBottom() const {
    return minimum_.y;
}

float Rect::getTop() const {
    return maximum_.y;
}

float Rect::getWidth() const {
    return maximum_.x - minimum_.x;
}

float Rect::getHeight() const {
    return maximum_.y - minimum_.y;
}

Point Rect::getCenter() const {
    return {(minimum_.x + maximum_.x) * 0.5f, (minimum_.y + maximum_.y) * 0.5f};
}

bool Rect::contains(const Point& point) const {
    return point.x >= minimum_.x && point.x <= maximum_.x && point.y >= minimum_.y && point.y <= maximum_.y;
}

bool Rect::overlaps(const Rect& other) const {
    const bool overlapsOnX = minimum_.x < other.maximum_.x && maximum_.x > other.minimum_.x;
    const bool overlapsOnY = minimum_.y < other.maximum_.y && maximum_.y > other.minimum_.y;

    return overlapsOnX && overlapsOnY;
}

bool Rect::scaleWithin(float amount, const Bounds& bounds) {
    const Point candidateMinimum{minimum_.x - amount, minimum_.y - amount};
    const Point candidateMaximum{maximum_.x + amount, maximum_.y + amount};

    const bool positiveSize = candidateMinimum.x < candidateMaximum.x && candidateMinimum.y < candidateMaximum.y;
    const bool insideBounds = candidateMinimum.x >= bounds.left && candidateMaximum.x <= bounds.right &&
                              candidateMinimum.y >= bounds.bottom && candidateMaximum.y <= bounds.top;

    if (!positiveSize || !insideBounds) {
        return false;
    }

    minimum_ = candidateMinimum;
    maximum_ = candidateMaximum;

    return true;
}

void Rect::moveCenterWithin(const Point& desiredCenter, const Bounds& bounds) {
    const float halfWidth = getWidth() * 0.5f;
    const float halfHeight = getHeight() * 0.5f;

    const float centerX = std::clamp(desiredCenter.x, bounds.left + halfWidth, bounds.right - halfWidth);
    const float centerY = std::clamp(desiredCenter.y, bounds.bottom + halfHeight, bounds.top - halfHeight);

    minimum_ = {centerX - halfWidth, centerY - halfHeight};
    maximum_ = {centerX + halfWidth, centerY + halfHeight};
}

void Rect::setRandomColor() {
    color_.setRandom();
}

void setOpenGlColor(const GlColor& color) {
    glColor3f(color.red, color.green, color.blue);
}

void renderFilledRect(const Rect& rect) {
    setOpenGlColor(rect.getColor());
    glRectf(rect.getLeft(), rect.getBottom(), rect.getRight(), rect.getTop());
}

void renderFilledRect(const AnimatedRect& aniRect) {
    setOpenGlColor(aniRect.rect.getColor());
    glRectf(aniRect.rect.getLeft(), aniRect.rect.getBottom(), aniRect.rect.getRight(), aniRect.rect.getTop());
}

void renderRectOutline(const Rect& rect, const GlColor& color, float lineWidth) {
    glLineWidth(lineWidth);
    setOpenGlColor(color);
    glBegin(GL_LINE_LOOP);
    glVertex2f(rect.getLeft(), rect.getBottom());
    glVertex2f(rect.getRight(), rect.getBottom());
    glVertex2f(rect.getRight(), rect.getTop());
    glVertex2f(rect.getLeft(), rect.getTop());
    glEnd();
    glLineWidth(1.0f);
}

Point screenToOpenGl(GLFWwindow* window, double mouseX, double mouseY) {
    int windowWidth = 0;
    int windowHeight = 0;

    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    if (windowWidth <= 0 || windowHeight <= 0) {
        return {};
    }

    return {static_cast<float>(2.0 * mouseX / windowWidth - 1.0),
            static_cast<float>(1.0 - 2.0 * mouseY / windowHeight)};
}
