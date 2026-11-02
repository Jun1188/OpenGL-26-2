#include "Board.h"
#include <stdexcept>

namespace warmup {
namespace {
bool isUnitStep(Delta delta) {
    return delta.x >= -1 && delta.x <= 1 && delta.y >= -1 && delta.y <= 1;
}

// Only movement normalizes the logical interval. Scaling retains its origin.
int movedPosition(int position, int length, int delta, int boardSize) {
    if (delta == 0)
        return position;

    const int moved = position + delta;

    if (moved + length <= 0)
        return moved + boardSize;

    if (moved + length > boardSize)
        return moved - boardSize;

    return moved;
}

bool growthCrossesBoundary(int origin, int oldLength, int newLength, int boardSize) {
    // Movement may leave a negative logical origin while the renderer wraps it.
    // Such an axis is already touching both board edges and cannot grow farther.
    return newLength > oldLength && (origin == 0 || origin + newLength == boardSize); // 순환 시에 resize 판정
    //-> 보수적으로 하면 origin < 0 || .. > ..
}
}

Rectangle::Rectangle(Point position, Size size) : position_(position), size_(size) {
    const int limit = board_rules::maximumSize;

    if (size.width < 1 || size.height < 1 || size.width > limit || size.height > limit)
        throw std::invalid_argument("Rectangle dimensions must be between 1 and 40.");

    if (position.x < -limit || position.y < -limit || position.x >= limit || position.y >= limit)
        throw std::invalid_argument("Rectangle logical position is outside the supported board range.");
}

bool Rectangle::fitsInside(int boardSize) const {
    return position_.x >= 0 && position_.y >= 0 && position_.x + size_.width <= boardSize &&
           position_.y + size_.height <= boardSize;
}

std::string describe(ChangeError error) {
    switch (error) {
    case ChangeError::none:
        return {};
    case ChangeError::invalidRectangle:
        return "Invalid rectangle number.";
    case ChangeError::invalidStep:
        return "The command requires a one-cell change.";
    case ChangeError::sizeLimit:
        return "Size limit reached; the entire command was rejected.";
    case ChangeError::boundary:
        return "Board boundary exceeded; the entire command was rejected.";
    }

    return "Unknown change error.";
}

ChangeError Board::reset(const Rectangles& rectangles) {
    for (const Rectangle& rectangle : rectangles) {
        if (!rectangle.fitsInside(board_rules::initialSize))
            return ChangeError::boundary;
    }
    rectangles_ = rectangles;
    size_ = board_rules::initialSize;

    return ChangeError::none;
}

ChangeError Board::moveRectangle(std::size_t id, Delta delta) {
    if (id >= rectangles_.size())
        return ChangeError::invalidRectangle;

    if (!isUnitStep(delta))
        return ChangeError::invalidStep;

    if (delta.x == 0 && delta.y == 0)
        return ChangeError::invalidStep;

    if (delta.x != 0 && delta.y != 0)
        return ChangeError::invalidStep;

    const Rectangle& current = rectangles_[id];
    const Point origin = current.position();
    const Size dimensions = current.size();
    const Point next{movedPosition(origin.x, dimensions.width, delta.x, size_),
                     movedPosition(origin.y, dimensions.height, delta.y, size_)};

    rectangles_[id] = Rectangle(next, dimensions);

    return ChangeError::none;
}

ChangeError Board::resizeRectangle(std::size_t id, Delta delta) {
    if (id >= rectangles_.size())
        return ChangeError::invalidRectangle;

    if (!isUnitStep(delta) || (delta.x == 0 && delta.y == 0))
        return ChangeError::invalidStep;

    const Rectangle& current = rectangles_[id];
    const Point origin = current.position();
    const Size old = current.size();
    const Size next{old.width + delta.x, old.height + delta.y};

    if (next.width < 1 || next.height < 1 || next.width > size_ || next.height > size_)
        return ChangeError::sizeLimit;

    if (growthCrossesBoundary(origin.x, old.width, next.width, size_) ||
        growthCrossesBoundary(origin.y, old.height, next.height, size_))
        return ChangeError::boundary;
    // Commit once, after BOTH dimensions pass. Failure never partially changes a shape.
    rectangles_[id] = Rectangle(origin, next);

    return ChangeError::none;
}

ChangeError Board::resizeBoard(int delta) {
    if (delta != -1 && delta != 1)
        return ChangeError::invalidStep;

    const int next = size_ + delta;

    if (next < board_rules::minimumSize || next > board_rules::maximumSize)
        return ChangeError::sizeLimit;

    if (delta < 0) {
        for (const Rectangle& rectangle : rectangles_) {
            if (!rectangle.fitsInside(next))
                return ChangeError::boundary;
        }
    }
    size_ = next;

    return ChangeError::none;
}
}
