#include "DiagramState.h"

#include <cmath>

namespace {

constexpr float AREA_BOTTOM = -0.95f;
constexpr float AREA_TOP = 0.95f;
constexpr float RECT_HALF_WIDTH = 0.075f;
constexpr float RECT_HALF_HEIGHT = 0.085f;
constexpr float STACK_X = 0.75f;
constexpr float STACK_SPACING = 0.10f;
constexpr std::size_t STACK_CAPACITY = 16;
constexpr float ANIMATION_STEP_TIME = 0.04f;

Diagram makeRectangle(float left, float bottom, float right, float top,
                      GlColor color, bool filled) {
    // EBO fill: 0-1-2, 2-1-3. Outline: 0-1-3-2.
    return Diagram(GLShape::Rect,
        { {left, bottom}, {right, bottom}, {left, top}, {right, top} },
        color, {}, filled);
}

struct Bounds {
    float left;
    float right;
    float bottom;
    float top;
};

Bounds boundsOf(const Diagram& diagram) {
    const std::vector<Point> vertices = diagram.worldVertices();
    Bounds bounds{ vertices[0].x, vertices[0].x, vertices[0].y, vertices[0].y };
    for (Point vertex : vertices) {
        if (vertex.x < bounds.left) bounds.left = vertex.x;
        if (vertex.x > bounds.right) bounds.right = vertex.x;
        if (vertex.y < bounds.bottom) bounds.bottom = vertex.y;
        if (vertex.y > bounds.top) bounds.top = vertex.y;
    }
    return bounds;
}

bool overlaps(const Bounds& a, const Bounds& b) {
    return a.left <= b.right && a.right >= b.left
        && a.bottom <= b.top && a.top >= b.bottom;
}

} // namespace

DiagramStore::DiagramStore()
    : fixedDiagrams_{
        makeRectangle(-RECT_HALF_WIDTH, -RECT_HALF_HEIGHT,
                      RECT_HALF_WIDTH, RECT_HALF_HEIGHT, {}, true),
        makeRectangle(-RECT_HALF_WIDTH, -RECT_HALF_HEIGHT,
                      RECT_HALF_WIDTH, RECT_HALF_HEIGHT, {}, true),
        makeRectangle(-0.95f, AREA_BOTTOM, -0.35f, AREA_TOP,
                      {0.65f, 0.75f, 0.9f}, false),
        makeRectangle(-0.30f, AREA_BOTTOM, 0.2f, AREA_TOP,
                      {0.65f, 0.75f, 0.9f}, false),
        makeRectangle(hitX_Left, hitY_Min, hitX_Right, hitY_Max,
                      {0.9f, 0.9f, 0.9f}, false)
    } {
    stackedDiagrams_.reserve(STACK_CAPACITY);
    reset();
}

void DiagramStore::reset() {
    stackedDiagrams_.clear();
    rects_ = {};
    animationTime_ = 0.0f;
    spawnPair();
    ++revision_;
}

void DiagramStore::spawnPair() {
    if (stackedDiagrams_.size() + rects_.size() > STACK_CAPACITY) return;

    for (std::size_t i = 0; i < rects_.size(); ++i) {
        const Bounds area = boundsOf(fixedDiagrams_[i + 2]);
        Diagram& diagram = fixedDiagrams_[i];
        const float y = i == 0 ? randomFloat(-0.7f, -0.3f)
                               : randomFloat(0.3f, 0.7f);
        diagram.setPivot({ (area.left + area.right) * 0.5f, y });
        GlColor color;
        color.setRandom();
        diagram.setColor(color);

        RectAttribute& rect = rects_[i];
        rect = {};
        rect.rs = RectState::Moving;
        rect.Index_ = i;
        rect.direction = i == 0 ? 1.0f : -1.0f;
        rect.speed = i == 0 ? 0.65f : 0.87f;
    }
}

bool DiagramStore::checkHit() {
    const Bounds hit = boundsOf(fixedDiagrams_[4]);
    for (const RectAttribute& rect : rects_) {
        if (rect.rs != RectState::Moving || !rect.Index_
            || !overlaps(boundsOf(fixedDiagrams_[*rect.Index_]), hit)) {
            return false;
        }
    }

    // Reserve both stack positions together, before either animation finishes.
    for (std::size_t i = 0; i < rects_.size(); ++i) {
        RectAttribute& rect = rects_[i];
        rect.rs = RectState::HitAnimated;
        rect.step = 0;
        rect.start = fixedDiagrams_[*rect.Index_].getPivot();
        rect.target = { STACK_X, AREA_BOTTOM + RECT_HALF_HEIGHT
            + static_cast<float>(stackedDiagrams_.size() + i) * STACK_SPACING };
    }
    animationTime_ = 0.0f;
    ++revision_;
    return true;
}

bool DiagramStore::hitAnimation() {
    bool changed = false;
    for (RectAttribute& rect : rects_) {
        if (rect.rs != RectState::HitAnimated || !rect.Index_) continue;

        ++rect.step;
        const float t = static_cast<float>(rect.step) / static_cast<float>(max_step);
        Diagram& diagram = fixedDiagrams_[*rect.Index_];
        diagram.setPivot({ rect.start.x + (rect.target.x - rect.start.x) * t,
                           rect.start.y + (rect.target.y - rect.start.y) * t });
        changed = true;
        if (rect.step == max_step) {
            stackedDiagrams_.push_back(diagram);
            rect.rs = RectState::Fixed;
            rect.Index_.reset();
        }
    }
    if (changed) {
        ++revision_;
        if (rects_[0].rs == RectState::Fixed && rects_[1].rs == RectState::Fixed) {
            animationTime_ = 0.0f;
            spawnPair();
        }
    }
    return changed;
}

bool DiagramStore::advance(float deltaTime) {
    if (!std::isfinite(deltaTime) || deltaTime <= 0.0f) return false;

    if (rects_[0].rs == RectState::HitAnimated) {
        animationTime_ += deltaTime;
        bool changed = false;
        while (animationTime_ >= ANIMATION_STEP_TIME
               && rects_[0].rs == RectState::HitAnimated) {
            animationTime_ -= ANIMATION_STEP_TIME;
            changed = hitAnimation() || changed;
        }
        return changed;
    }

    bool changed = false;
    for (std::size_t i = 0; i < rects_.size(); ++i) {
        RectAttribute& rect = rects_[i];
        if (rect.rs != RectState::Moving || !rect.Index_) continue;

        const Bounds area = boundsOf(fixedDiagrams_[i + 2]);
        const float lower = area.bottom + RECT_HALF_HEIGHT;
        const float upper = area.top - RECT_HALF_HEIGHT;
        Diagram& diagram = fixedDiagrams_[*rect.Index_];
        Point pivot = diagram.getPivot();
        pivot.y += rect.direction * rect.speed * deltaTime;
        // Reflect the excess distance so a boundary does not discard movement.
        while (pivot.y > upper || pivot.y < lower) {
            if (pivot.y > upper) {
                pivot.y = 2.0f * upper - pivot.y;
                rect.direction = -1.0f;
            } else {
                pivot.y = 2.0f * lower - pivot.y;
                rect.direction = 1.0f;
            }
        }
        changed = diagram.setPivot(pivot) || changed;
    }
    if (changed) ++revision_;
    return changed;
}

std::vector<Diagram> DiagramStore::getItems() const {
    std::vector<Diagram> items;
    items.reserve(3 + stackedDiagrams_.size() + rects_.size());
    for (std::size_t i = 2; i < fixedDiagrams_.size(); ++i) {
        items.push_back(fixedDiagrams_[i]);
    }
    for (const Diagram& diagram : stackedDiagrams_) items.push_back(diagram);
    for (const RectAttribute& rect : rects_) {
        if (rect.Index_) items.push_back(fixedDiagrams_[*rect.Index_]);
    }
    return items;
}
