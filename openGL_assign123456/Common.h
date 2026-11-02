#pragma once

struct GLFWwindow;

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

struct Point {
    float x = 0.0f;
    float y = 0.0f;
};

struct Bounds {
    float left = -1.0f;
    float right = 1.0f;
    float bottom = -1.0f;
    float top = 1.0f;
};

struct GlColor {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;

    void set(float r, float g, float b);
    void setRandom();
};

class Rect {
public:
    Rect(const Point& first, const Point& second, const GlColor& color);

    static Rect fromCenter(const Point& center, float halfWidth, float halfHeight, const GlColor& color);

    const Point& getMinimum() const;
    const Point& getMaximum() const;
    const GlColor& getColor() const;

    float getLeft() const;
    float getRight() const;
    float getBottom() const;
    float getTop() const;
    float getWidth() const;
    float getHeight() const;
    Point getCenter() const;

    bool contains(const Point& point) const;
    bool overlaps(const Rect& other) const;

    bool scaleWithin(float amount, const Bounds& bounds);

    void moveCenterWithin(const Point& desiredCenter, const Bounds& bounds);

    void resetAssignedColorAt(const GlColor gl);
    void setRandomColor();

private:
    Point minimum_{};
    Point maximum_{};
    GlColor color_{};
};

enum class OrbitEdge { Top, Right, Bottom, Left };

struct AnimatedRect {
    explicit AnimatedRect(const Rect& source) : rect(source) {
    }

    Rect rect;
    Point diagonalVelocity{};
    Point zigzagVelocity{};
    OrbitEdge orbitEdge = OrbitEdge::Top;
    bool orbitReachedBorder = false;
    bool growing = true;
    float recolorElapsed = 0.0f;

    bool isZigzagMove = true;
    float zigzagStep = 0.0f;
};

float randomFloat(float minimum, float maximum);

size_t randomIndex(size_t minSize, size_t maxSize);

bool randomBool();

void setOpenGlColor(const GlColor& color);
void renderFilledRect(const Rect& rect);
void renderFilledRect(const AnimatedRect& aniRect);
void renderRectOutline(const Rect& rect, const GlColor& color, float lineWidth);

Point screenToOpenGl(GLFWwindow* window, double mouseX, double mouseY);
