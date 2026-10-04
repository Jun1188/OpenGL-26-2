#include <glew.h> // GLEW must precede GLFW.
#include <glfw3.h>

#include "Console.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <stdexcept>

namespace {

std::size_t checkedDimension(std::size_t value) {
    if (value == 0 || value > static_cast<std::size_t>(std::numeric_limits<int>::max() / 8)) {
        throw std::invalid_argument("Grid dimensions must be positive and fit the renderer.");
    }
    return value;
}

constexpr float EFFECT_INTERVAL = 0.06f; // Five steps last about 0.3 seconds.

} // namespace

Grid::Grid(std::size_t columns, std::size_t rows)
    : gridX(checkedDimension(columns)), gridY(checkedDimension(rows)),
      squareX(2.0f / static_cast<float>(gridX)),
      squareY(2.0f / static_cast<float>(gridY)) {
    if (gridX > std::numeric_limits<std::size_t>::max() / gridY) {
        throw std::invalid_argument("Grid cell count overflow.");
    }
    resetBoard();
}

Point Grid::cellCenter(Cell cell) const {
    // Grid (0, 0) is top-left. OpenGL Y increases upward.
    return { boardLeft + (static_cast<float>(cell.x) + 0.5f) * squareX,
             boardTop - (static_cast<float>(cell.y) + 0.5f) * squareY };
}

void Grid::resetBoard() {
    store_.clear();
    cells_.clear();
    boardLines_.clear();
    effects_.clear();
    effectSteps_.clear();
    selectedIndex_ = 0;
    movingRight_ = true;

    const GlColor gridColor{ 0.3f, 0.3f, 0.35f };
    for (std::size_t x = 0; x <= gridX; ++x) {
        const float px = boardLeft + static_cast<float>(x) * squareX;
        boardLines_.emplace_back(GLShape::Line,
            std::vector<Point>{ { px, boardBottom }, { px, boardTop } }, gridColor, Point{});
    }
    for (std::size_t y = 0; y <= gridY; ++y) {
        const float py = boardTop - static_cast<float>(y) * squareY;
        boardLines_.emplace_back(GLShape::Line,
            std::vector<Point>{ { boardLeft, py }, { boardRight, py } }, gridColor, Point{});
    }

    Diagram hero(GLShape::Rect,
        std::vector<Point>{ { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 } },
        GlColor{ 1.0f, 1.0f, 1.0f }, cellCenter({ 0, 0 }));
    hero.setScale({ squareX * 0.36f, squareY * 0.36f });
    hero.setFilled(false); // Original pseudo code: start with an outline rectangle.
    if (store_.push(hero)) cells_.push_back({ 0, 0 });

    // Pick unique cells without a retry loop. Exclude the hero's starting cell.
    const std::size_t obstacleCount = std::min<std::size_t>(80, gridX * gridY - 1);
    for (std::size_t i = 0; i < obstacleCount; ++i) {
        const std::size_t cellCount = gridX * gridY;
        std::size_t choice = std::min(cellCount - 1,
            static_cast<std::size_t>(randomFloat(1.0f, static_cast<float>(cellCount))));
        Cell cell{ choice % gridX, choice / gridX };
        bool occupied = true;
        while (occupied) {
            occupied = false;
            for (const Cell& existing : cells_) {
                if (existing.x == cell.x && existing.y == cell.y) {
                    occupied = true;
                    break;
                }
            }
            if (occupied) {
                choice = (choice + 1) % cellCount;
                cell = { choice % gridX, choice / gridX };
            }
        }
        const int shape = static_cast<int>(i % 3); // All three shapes occur.
        std::vector<Point> vertices;
        if (shape == 0) vertices = { { -1, -1 }, { 1, -1 }, { 0, 1 } };
        else if (shape == 1) vertices = { { -1, -1 }, { 1, -1 }, { -1, 1 }, { 1, 1 } };
        else vertices = { { -1, 1 }, { 1, 1 }, { 0, -1 } };
        GlColor color;
        color.setRandom();
        Diagram obstacle(shape == 1 ? GLShape::Rect : GLShape::Triangle,
                         vertices, color, cellCenter(cell));
        // Independently vary X and Y, but always stay inside one cell.
        obstacle.setScale({ squareX * randomFloat(0.22f, 0.42f),
                            squareY * randomFloat(0.22f, 0.42f) });
        if (store_.push(obstacle)) cells_.push_back(cell);
    }
    ++revision_;
}

std::vector<Diagram> Grid::getRenderItems() const {
    std::vector<Diagram> items = boardLines_;
    const std::vector<Diagram>& diagrams = store_.getItems();
    if (diagrams.empty()) return items;
    const Cell hero = cells_[selectedIndex_];
    for (std::size_t i = 0; i < diagrams.size(); ++i) {
        if (i == selectedIndex_) continue;
        // During a swap, draw the new hero alone in the occupied cell.
        // The old hero becomes visible there once the new hero leaves.
        if (cells_[i].x == hero.x && cells_[i].y == hero.y) continue;
        items.push_back(diagrams[i]);
    }
    items.insert(items.end(), effects_.begin(), effects_.end());
    items.push_back(diagrams[selectedIndex_]); // Last item is the active hero.
    return items;
}

bool Grid::getIsEndPos() const {
    if (cells_.empty()) return true;
    const Cell cell = cells_[selectedIndex_];
    const std::size_t lastX = gridY % 2 == 0 ? 0 : gridX - 1;
    return cell.y == gridY - 1 && cell.x == lastX;
}

bool Grid::moveTo(Cell cell, std::size_t idx) {
    if (idx >= cells_.size() || cell.x >= gridX || cell.y >= gridY) return false;
    Diagram moved = store_.getItems()[idx];
    if (!moved.setPose(cellCenter(cell), 0.0f) || !store_.replaceAt(idx, moved)) return false;
    cells_[idx] = cell;
    ++revision_;
    return true;
}

bool Grid::moveUp(std::size_t step, std::size_t idx) {
    if (idx >= cells_.size() || step > cells_[idx].y) return false;
    return moveTo({ cells_[idx].x, cells_[idx].y - step }, idx);
}

bool Grid::moveDown(std::size_t step, std::size_t idx) {
    if (idx >= cells_.size() || step > gridY - 1 - cells_[idx].y) return false;
    return moveTo({ cells_[idx].x, cells_[idx].y + step }, idx);
}

bool Grid::moveLeft(std::size_t step, std::size_t idx) {
    if (idx >= cells_.size() || step > cells_[idx].x) return false;
    return moveTo({ cells_[idx].x - step, cells_[idx].y }, idx);
}

bool Grid::moveRight(std::size_t step, std::size_t idx) {
    if (idx >= cells_.size() || step > gridX - 1 - cells_[idx].x) return false;
    return moveTo({ cells_[idx].x + step, cells_[idx].y }, idx);
}

void Grid::moveByStep() {
    if (getIsEndPos()) return;
    const Cell cell = cells_[selectedIndex_];
    bool moved = false;
    if (movingRight_ && cell.x < gridX - 1) {
        moved = moveRight(1, selectedIndex_);
    } else if (!movingRight_ && cell.x > 0) {
        moved = moveLeft(1, selectedIndex_);
    } else {
        moved = moveDown(1, selectedIndex_);
        if (moved) movingRight_ = !movingRight_;
    }
    if (moved) moveSelectedTo();
}

float Grid::getStepDuration() const {
    if (cells_.empty()) return 1.0f;
    const Cell cell = cells_[selectedIndex_];
    const bool nextIsVertical = movingRight_ ? cell.x == gridX - 1 : cell.x == 0;
    const float speed = nextIsVertical ? speedVertical : speedHorizontal;
    return 1.0f / (speed * speedMultiplier_);
}

void Grid::changeSpeed(float factor) {
    if (!std::isfinite(factor) || factor <= 0.0f) return;
    speedMultiplier_ = std::clamp(speedMultiplier_ * factor, 0.25f, 4.0f);
}

Diagram Grid::effectBy(const Diagram& changed) const {
    Diagram effect = changed;
    effect.setFilled(false);
    return effect;
}

void Grid::moveSelectedTo() {
    const Cell hero = cells_[selectedIndex_];
    const std::vector<Diagram>& diagrams = store_.getItems();
    for (std::size_t i = 0; i < cells_.size(); ++i) {
        if (i == selectedIndex_ || cells_[i].x != hero.x || cells_[i].y != hero.y) continue;
        // Keep the old hero here as an obstacle. Choose the struck diagram as the
        // new hero. Shape, color, size, and outline state stay with each Diagram.
        effects_.push_back(effectBy(diagrams[selectedIndex_]));
        effectSteps_.push_back(0);
        effects_.push_back(effectBy(diagrams[i]));
        effectSteps_.push_back(0);
        selectedIndex_ = i;
        ++revision_;
        return; // At most one obstacle can occupy a cell before this collision.
    }
}

bool Grid::stepEffect(std::size_t idx) {
    if (idx >= effects_.size() || idx >= effectSteps_.size()) return false;
    const std::size_t previousStep = effectSteps_[idx];
    if (previousStep >= EFFECT_STEP) return false;
    const std::size_t nextStep = ++effectSteps_[idx];
    if (nextStep >= EFFECT_STEP) return false;

    // sqrt(t): fast expansion first, then progressively smaller increments.
    const float previousFactor = 1.0f + 1.2f * std::sqrt(
        static_cast<float>(previousStep) / static_cast<float>(EFFECT_STEP - 1));
    const float nextFactor = 1.0f + 1.2f * std::sqrt(
        static_cast<float>(nextStep) / static_cast<float>(EFFECT_STEP - 1));
    const float ratio = nextFactor / previousFactor;
    const Point scale = effects_[idx].getScale();
    effects_[idx].setEffectScale({ scale.x * ratio, scale.y * ratio });
    GlColor color = effects_[idx].getColor();
    color.set(color.red * 0.78f, color.green * 0.78f, color.blue * 0.78f);
    effects_[idx].setColor(color);
    return true;
}

void Grid::advanceEffects() {
    if (effects_.empty()) return;
    // Erase backward to keep the two vectors' shared indices valid.
    for (std::size_t i = effects_.size(); i > 0; --i) {
        if (!stepEffect(i - 1)) {
            effects_.erase(effects_.begin() + (i - 1));
            effectSteps_.erase(effectSteps_.begin() + (i - 1));
        }
    }
    ++revision_;
}
