#pragma once
#include <array>
#include <cstddef>
#include <string>

namespace warmup {
namespace board_rules {
const int initialSize = 30;
const int minimumSize = 10;
const int maximumSize = 40;
const std::size_t rectangleCount = 2;
}

struct Point {
    int x = 0;
    int y = 0;
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

struct Size {
    int width = 1;
    int height = 1;
    bool operator==(const Size& other) const {
        return width == other.width && height == other.height;
    }
};

struct Delta {
    int x = 0;
    int y = 0;
};

// Value object: callers cannot modify the position or dimensions in place.
class Rectangle {
public:
    Rectangle() = default;
    Rectangle(Point position, Size size);
    Point position() const { return position_; }
    Size size() const { return size_; }
    int area() const { return size_.width * size_.height; }
    bool fitsInside(int boardSize) const;
    bool operator==(const Rectangle& other) const {
        return position_ == other.position_ && size_ == other.size_;
    }
private:
    Point position_{};
    Size size_{};
};

enum class ChangeError { none, invalidRectangle, invalidStep, sizeLimit, boundary };
std::string describe(ChangeError error);

// Owns the state and boundary policy. No input, rendering or Windows dependency.
class Board {
public:
    using Rectangles = std::array<Rectangle, board_rules::rectangleCount>;
    int size() const { return size_; }
    const Rectangles& rectangles() const { return rectangles_; }
    ChangeError reset(const Rectangles& rectangles);
    ChangeError moveRectangle(std::size_t id, Delta delta);
    ChangeError resizeRectangle(std::size_t id, Delta delta);
    ChangeError resizeBoard(int delta);
private:
    int size_ = board_rules::initialSize;
    Rectangles rectangles_{};
};
}
