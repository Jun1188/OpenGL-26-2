#include "RectState.h"

#include <algorithm>

#include <cmath>
//#include <limits>
#include <print>
#include <utility>

namespace {

constexpr std::size_t MAX_RECTS_PER_SECTION = 5;
constexpr std::size_t MAX_CREATED_RECTS4 = 10;

constexpr std::size_t MAX_TOTAL_RECTS = 20;
constexpr float MIN_INITIAL_HALF_SIZE = 0.08f;
constexpr float MAX_INITIAL_HALF_SIZE = 0.20f;
constexpr float MIN_SPLIT_EDGE = 0.05f;
constexpr std::size_t MAX_PRACTICE4_RECTS = 5;
constexpr float PRACTICE4_HALF_WIDTH = 0.10f;
constexpr float PRACTICE4_HALF_HEIGHT = 0.10f;
constexpr float DIAGONAL_SPEED = 0.35f;
constexpr float ZIGZAG_HORIZONTAL_SPEED = 0.45f;
constexpr float ORBIT_APPROACH_SPEED = 0.55f;
constexpr float ORBIT_EDGE_SPEED = 0.40f;
constexpr float RESIZE_SPEED = 0.08f;
constexpr float MIN_HALF_SIZE = 0.05f;
constexpr float MAX_HALF_SIZE = 0.22f;
constexpr float RECOLOR_INTERVAL = 0.25f;
constexpr std::size_t MIN_PRACTICE5_RECTS = 20;
constexpr std::size_t MAX_PRACTICE5_RECTS = 40;
constexpr std::size_t MAX_PRACTICE5_CREATED_RECTS = 10;
constexpr float PRACTICE5_RECT_HALF_SIZE = 0.05f;
constexpr float PRACTICE5_ERASER_INITIAL_HALF_SIZE = 0.10f;
constexpr float PRACTICE5_ERASER_RESIZE_STEP = 0.05f;
constexpr float PRACTICE5_ERASER_MIN_HALF_SIZE = PRACTICE5_RECT_HALF_SIZE;
constexpr std::size_t MIN_PRACTICE6_RECTS = 5;
constexpr std::size_t MAX_PRACTICE6_RECTS = 10;
constexpr float PRACTICE6_MIN_INITIAL_HALF_SIZE = 0.12f;
constexpr float PRACTICE6_MAX_INITIAL_HALF_SIZE = 0.22f;
constexpr float PRACTICE6_MOVE_SPEED = 0.30f;
constexpr float PRACTICE6_SHRINK_SPEED = 0.025f;
constexpr float PRACTICE6_MIN_HALF_SIZE = 0.008f;
constexpr float PRACTICE6_COLOR_FADE_SPEED = 0.60f;
constexpr float PRACTICE6_EMISSION_INTERVAL = 0.08f;
const Bounds WINDOW_BOUNDS{};

Point getSectionCenter(Section section) {
    switch (section) {
    case Section::Section1:
        return {0.5f, 0.5f};
    case Section::Section2:
        return {-0.5f, 0.5f};
    case Section::Section3:
        return {-0.5f, -0.5f};
    case Section::Section4:
        return {0.5f, -0.5f};
    default:
        return {};
    }
}

Section getSection(const Rect& rect) {
    const Point center = rect.getCenter();

    if (center.x > 0.0f && center.y > 0.0f) {
        return Section::Section1;
    }

    if (center.x < 0.0f && center.y > 0.0f) {
        return Section::Section2;
    }

    if (center.x < 0.0f && center.y < 0.0f) {
        return Section::Section3;
    }

    if (center.x > 0.0f && center.y < 0.0f) {
        return Section::Section4;
    }

    return Section::Error;
}

Bounds getSectionBounds(Section section) {
    switch (section) {
    case Section::Section1:
        return {0.0f, 1.0f, 0.0f, 1.0f};
    case Section::Section2:
        return {-1.0f, 0.0f, 0.0f, 1.0f};
    case Section::Section3:
        return {-1.0f, 0.0f, -1.0f, 0.0f};
    case Section::Section4:
        return {0.0f, 1.0f, -1.0f, 0.0f};
    default:
        return {};
    }
}

} // namespace

const std::vector<AnimatedRect>& RectStore::getItems() const {
    return rects_;
}

Rect* RectStore::getSelected() {
    if (!selectedIndex_.has_value()) {
        return nullptr;
    }

    return &rects_[*selectedIndex_].rect;
}

const Rect* RectStore::getSelected() const {
    if (!selectedIndex_.has_value()) {
        return nullptr;
    }

    return &rects_[*selectedIndex_].rect;
}

std::optional<RectStore::Index> RectStore::getSelectedIndex() const {
    return selectedIndex_;
}

std::optional<RectStore::Index> RectStore::findTopmostAt(const Point& point, std::optional<Index> excludedIndex) const {
    for (std::size_t i = rects_.size(); i > 0; --i) {
        const std::size_t index = i - 1;
        const bool isExcluded = excludedIndex.has_value() && index == *excludedIndex;

        if (!isExcluded && rects_[index].rect.contains(point)) {
            return index;
        }
    }

    return std::nullopt;
}

void RectStore::select(std::optional<Index> index) {
    if (index.has_value() && *index < rects_.size()) {
        selectedIndex_ = index;

        return;
    }
    selectedIndex_.reset();
}

void RectStore::clearSelection() {
    selectedIndex_.reset();
}

void RectStore::clear() {
    rects_.clear();
    capture_.clear();
    clearSelection();
}

void RectStore::push(const Rect& rect) {
    rects_.emplace_back(rect);
}

void RectStore::push(const AnimatedRect& rect) {
    rects_.push_back(rect);
}

bool RectStore::erase(Index index) {
    if (index >= rects_.size()) {
        return false;
    }

    if (selectedIndex_.has_value() && *selectedIndex_ == index) {
        selectedIndex_.reset();
    } else if (selectedIndex_.has_value() && *selectedIndex_ > index) {
        --(*selectedIndex_);
    }

    rects_.erase(rects_.begin() + static_cast<std::ptrdiff_t>(index));

    return true;
}

std::size_t RectStore::size() const {
    return rects_.size();
}

AnimatedRect& RectStore::get(Index index) {
    return rects_.at(index);
}

const AnimatedRect& RectStore::get(Index index) const {
    return rects_.at(index);
}

void RectStore::captureCurrentState() {
    capture_ = rects_;
}

void RectStore::updateCapture(const Rect& r) {
    capture_.emplace_back(r);
}

bool RectStore::restoreCapturedState() {
    if (capture_.empty()) {
        return false;
    }

    rects_ = capture_;
    capture_.clear();
    clearSelection();

    return true;
}

bool Practice2RectState::push(Section section) {
    if (section == Section::Error) {
        return false;
    }

    if (countInSection(section) >= MAX_RECTS_PER_SECTION) {
        removeOldestInSection(section);
    }

    GlColor color;

    color.setRandom();

    const float halfSize = randomFloat(0.10f, 0.35f);

    store_.push(Rect::fromCenter(getSectionCenter(section), halfSize, halfSize, color));

    return true;
}

Rect* Practice2RectState::select(const Point& point) {
    store_.clearSelection();

    const std::vector<std::size_t> renderOrder = getRenderOrder();
    const std::vector<AnimatedRect>& rects = store_.getItems();

    for (auto it = renderOrder.rbegin(); it != renderOrder.rend(); ++it) {
        const std::size_t index = *it;

        if (rects[index].rect.contains(point)) {
            store_.select(index);

            return store_.getSelected();
        }
    }

    return nullptr;
}

bool Practice2RectState::scaleSelected(float amount) {
    Rect* selected = store_.getSelected();

    if (selected == nullptr) {
        return false;
    }

    const Section section = getSection(*selected);

    if (section == Section::Error) {
        return false;
    }

    return selected->scaleWithin(amount, getSectionBounds(section));
}

bool Practice2RectState::recolorSelected() {
    Rect* selected = store_.getSelected();

    if (selected == nullptr) {
        return false;
    }

    selected->setRandomColor();

    return true;
}

void Practice2RectState::removeAll() {
    store_.clear();
}

const std::vector<AnimatedRect>& Practice2RectState::getItems() const {
    return store_.getItems();
}

Rect* Practice2RectState::getSelected() {
    return store_.getSelected();
}

const Rect* Practice2RectState::getSelected() const {
    return store_.getSelected();
}

std::vector<std::size_t> Practice2RectState::getRenderOrder() const {
    const std::vector<AnimatedRect>& rects = store_.getItems();
    std::vector<std::size_t> order;

    order.reserve(rects.size());

    // Equal sizes keep the old rule: the oldest rectangle is rendered last.
    for (std::size_t i = rects.size(); i > 0; --i) {
        order.push_back(i - 1);
    }

    std::stable_sort(order.begin(), order.end(), [&rects](std::size_t left, std::size_t right) {
        return rects[left].rect.getWidth() > rects[right].rect.getWidth();
    });

    return order;
}

std::size_t Practice2RectState::countInSection(Section section) const {
    std::size_t count = 0;

    for (const AnimatedRect& item : store_.getItems()) {
        if (getSection(item.rect) == section) {
            ++count;
        }
    }

    return count;
}

void Practice2RectState::removeOldestInSection(Section section) {
    const std::vector<AnimatedRect>& rects = store_.getItems();

    for (std::size_t i = 0; i < rects.size(); ++i) {
        if (getSection(rects[i].rect) != section) {
            continue;
        }

        store_.erase(i);

        return;
    }
}

bool Practice3RectState::pushRandom() {
    if (store_.size() >= MAX_CREATED_RECTS4) {
        return false;
    }

    const float halfWidth = randomFloat(MIN_INITIAL_HALF_SIZE, MAX_INITIAL_HALF_SIZE);
    const float halfHeight = randomFloat(MIN_INITIAL_HALF_SIZE, MAX_INITIAL_HALF_SIZE);
    const Point center{randomFloat(WINDOW_BOUNDS.left + halfWidth, WINDOW_BOUNDS.right - halfWidth),
                       randomFloat(WINDOW_BOUNDS.bottom + halfHeight, WINDOW_BOUNDS.top - halfHeight)};

    GlColor color;

    color.setRandom();
    store_.push(Rect::fromCenter(center, halfWidth, halfHeight, color));

    return true;
}

Rect* Practice3RectState::select(const Point& point) {
    store_.select(store_.findTopmostAt(point));

    return store_.getSelected();
}

bool Practice3RectState::beginDrag(const Point& point) {
    Rect* selected = select(point);

    dragging_ = selected != nullptr;

    if (!dragging_) {
        return false;
    }

    const Point center = selected->getCenter();

    dragOffset_ = {point.x - center.x, point.y - center.y};

    return true;
}

void Practice3RectState::dragTo(const Point& point) {
    Rect* selected = store_.getSelected();

    if (selected == nullptr || !dragging_) {
        return;
    }

    const Point desiredCenter{point.x - dragOffset_.x, point.y - dragOffset_.y};

    selected->moveCenterWithin(desiredCenter, WINDOW_BOUNDS);
}

bool Practice3RectState::endDrag() {
    const std::optional<RectStore::Index> draggedIndex = store_.getSelectedIndex();

    if (!draggedIndex.has_value() || !dragging_) {
        dragging_ = false;

        return false;
    }

    dragging_ = false;

    const std::optional<RectStore::Index> targetIndex = findTopmostOverlap(*draggedIndex);

    if (!targetIndex.has_value()) {
        return false;
    }

    return merge(*draggedIndex, *targetIndex);
}

bool Practice3RectState::splitAt(const Point& point) {
    if (store_.size() >= MAX_TOTAL_RECTS) {
        return false;
    }

    const std::optional<RectStore::Index> index = store_.findTopmostAt(point);

    if (!index.has_value()) {
        return false;
    }

    const Rect source = store_.get(*index).rect;
    const bool canSplitVertically = source.getWidth() > MIN_SPLIT_EDGE * 2.0f;
    const bool canSplitHorizontally = source.getHeight() > MIN_SPLIT_EDGE * 2.0f;

    if (!canSplitVertically && !canSplitHorizontally) {
        return false;
    }

    bool splitVertically = canSplitVertically;

    if (canSplitVertically && canSplitHorizontally) {
        splitVertically = randomBool();
    }

    GlColor firstColor;
    GlColor secondColor;

    firstColor.setRandom();
    secondColor.setRandom();

    Rect first = source;
    Rect second = source;

    if (splitVertically) {
        const float splitX = randomFloat(source.getLeft() + MIN_SPLIT_EDGE, source.getRight() - MIN_SPLIT_EDGE);

        first = Rect(source.getMinimum(), {splitX, source.getTop()}, firstColor);
        second = Rect({splitX, source.getBottom()}, source.getMaximum(), secondColor);
    } else {
        const float splitY = randomFloat(source.getBottom() + MIN_SPLIT_EDGE, source.getTop() - MIN_SPLIT_EDGE);

        first = Rect(source.getMinimum(), {source.getRight(), splitY}, firstColor);
        second = Rect({source.getLeft(), splitY}, source.getMaximum(), secondColor);
    }

    store_.erase(*index);
    store_.push(first);
    store_.push(second);
    store_.clearSelection();
    dragging_ = false;

    return true;
}

bool Practice3RectState::isDragging() const {
    return dragging_;
}

const std::vector<AnimatedRect>& Practice3RectState::getItems() const {
    return store_.getItems();
}

Rect* Practice3RectState::getSelected() {
    return store_.getSelected();
}

const Rect* Practice3RectState::getSelected() const {
    return store_.getSelected();
}

std::vector<std::size_t> Practice3RectState::getRenderOrder() const {
    const std::vector<AnimatedRect>& rects = store_.getItems();
    std::vector<std::size_t> order;

    order.reserve(rects.size());

    for (std::size_t i = 0; i < rects.size(); ++i) {
        order.push_back(i);
    }

    return order;
}

std::optional<RectStore::Index> Practice3RectState::findTopmostOverlap(RectStore::Index draggedIndex) const {
    const std::vector<AnimatedRect>& rects = store_.getItems();

    if (draggedIndex >= rects.size()) {
        return std::nullopt;
    }

    const Rect& dragged = rects[draggedIndex].rect;

    // Search in reverse insertion order so the visible topmost overlap wins.
    for (std::size_t i = rects.size(); i > 0; --i) {
        const std::size_t index = i - 1;

        if (index != draggedIndex && dragged.overlaps(rects[index].rect)) {
            return index;
        }
    }

    return std::nullopt;
}

bool Practice3RectState::merge(RectStore::Index firstIndex, RectStore::Index secondIndex) {
    const std::vector<AnimatedRect>& rects = store_.getItems();

    if (firstIndex >= rects.size() || secondIndex >= rects.size() || firstIndex == secondIndex) {
        return false;
    }

    const Rect first = rects[firstIndex].rect;
    const Rect second = rects[secondIndex].rect;
    const Point minimum{std::min(first.getLeft(), second.getLeft()), std::min(first.getBottom(), second.getBottom())};
    const Point maximum{std::max(first.getRight(), second.getRight()), std::max(first.getTop(), second.getTop())};

    GlColor mergedColor;

    mergedColor.setRandom();

    const Rect merged(minimum, maximum, mergedColor);

    const RectStore::Index largerIndex = std::max(firstIndex, secondIndex);
    const RectStore::Index smallerIndex = std::min(firstIndex, secondIndex);

    store_.erase(largerIndex);
    store_.erase(smallerIndex);
    store_.push(merged);

    store_.select(store_.size() - 1);

    return true;
}

// Practice 4
bool Practice4RectState::canCreateRectAt(const Point& center) const {
    if (store_.size() >= MAX_PRACTICE4_RECTS) {
        return false;
    }

    return center.x >= WINDOW_BOUNDS.left + PRACTICE4_HALF_WIDTH &&
           center.x <= WINDOW_BOUNDS.right - PRACTICE4_HALF_WIDTH &&
           center.y >= WINDOW_BOUNDS.bottom + PRACTICE4_HALF_HEIGHT &&
           center.y <= WINDOW_BOUNDS.top - PRACTICE4_HALF_HEIGHT;
}

bool Practice4RectState::tryCreateRectAt(const Point& point) {
    if (!canCreateRectAt(point)) {
        return false;
    }

    GlColor color;

    color.setRandom();

    AnimatedRect item(Rect::fromCenter(point, PRACTICE4_HALF_WIDTH, PRACTICE4_HALF_HEIGHT, color));

    item.diagonalVelocity = {randomBool() ? DIAGONAL_SPEED : -DIAGONAL_SPEED,
                             randomBool() ? DIAGONAL_SPEED : -DIAGONAL_SPEED};
    item.zigzagVelocity = {randomBool() ? ZIGZAG_HORIZONTAL_SPEED : -ZIGZAG_HORIZONTAL_SPEED, 0.0f};
    store_.push(item);

    if (hasActiveAnimation()) {
        captureIfNeeded();
    }

    return true;
}

void Practice4RectState::update(float deltaTime) {
    if (deltaTime <= 0.0f || store_.size() == 0) {
        return;
    }

    const float safeDeltaTime = std::min(deltaTime, 0.08f);

    for (std::size_t i = 0; i < store_.size(); ++i) {
        AnimatedRect& item = store_.get(i);

        if (isResize_) {
            updateResize(item, safeDeltaTime);
        }

        Point movement{};

        if (isDiagonal_) {
            const Point diagonal = calculateDiagonalMovement(item, safeDeltaTime);

            movement.x += diagonal.x;
            movement.y += diagonal.y;
        }

        if (isHorizontalZigzag_) {
            const Point zigzag = calculateZigzagMovement(item, safeDeltaTime);

            movement.x += zigzag.x;
            movement.y += zigzag.y;
        }

        if (isOrbit_) {
            movement = calculateOrbitMovement(item, safeDeltaTime, movement);
        }
        moveWithinWindow(item, movement);

        if (isRecolor_) {
            updateRecolor(item, safeDeltaTime);
        }
    }
}

bool Practice4RectState::rollbackAtCapture() {
    if (!hasCapture_ || !store_.restoreCapturedState()) {
        return false;
    }

    stopAll();
    hasCapture_ = false;

    return true;
}

void Practice4RectState::toggleDiagonal() {
    captureIfNeeded();
    isDiagonal_ = !isDiagonal_;
}

void Practice4RectState::toggleHorizontalZigzag() {
    captureIfNeeded();
    isHorizontalZigzag_ = !isHorizontalZigzag_;
}

void Practice4RectState::toggleOrbit() {
    captureIfNeeded();
    isOrbit_ = !isOrbit_;

    if (isOrbit_) {
        initializeOrbitState();
    }
}

void Practice4RectState::toggleResize() {
    captureIfNeeded();
    isResize_ = !isResize_;
}

void Practice4RectState::toggleRecolor() {
    captureIfNeeded();
    isRecolor_ = !isRecolor_;
}

void Practice4RectState::reset() {
    stopAll();
    store_.clear();
    hasCapture_ = false;
}

bool Practice4RectState::hasActiveAnimation() const {
    return isDiagonal_ || isHorizontalZigzag_ || isOrbit_ || isResize_ || isRecolor_;
}

void Practice4RectState::captureIfNeeded() {
    if (hasCapture_ || store_.size() == 0) {
        return;
    }

    store_.captureCurrentState();
    hasCapture_ = true;
}

void Practice4RectState::initializeOrbitState() {
    for (std::size_t i = 0; i < store_.size(); ++i) {
        AnimatedRect& item = store_.get(i);
        const Point center = item.rect.getCenter();
        const float left = WINDOW_BOUNDS.left + item.rect.getWidth() * 0.5f;
        const float right = WINDOW_BOUNDS.right - item.rect.getWidth() * 0.5f;
        const float bottom = WINDOW_BOUNDS.bottom + item.rect.getHeight() * 0.5f;
        const float top = WINDOW_BOUNDS.top - item.rect.getHeight() * 0.5f;

        float nearestDistance = std::abs(top - center.y);

        item.orbitEdge = OrbitEdge::Top;

        const float rightDistance = std::abs(right - center.x);

        if (rightDistance < nearestDistance) {
            nearestDistance = rightDistance;
            item.orbitEdge = OrbitEdge::Right;
        }

        const float bottomDistance = std::abs(bottom - center.y);

        if (bottomDistance < nearestDistance) {
            nearestDistance = bottomDistance;
            item.orbitEdge = OrbitEdge::Bottom;
        }

        const float leftDistance = std::abs(left - center.x);

        if (leftDistance < nearestDistance) {
            item.orbitEdge = OrbitEdge::Left;
        }

        item.orbitReachedBorder = false;
    }
}

Point Practice4RectState::calculateDiagonalMovement(const AnimatedRect& item, float deltaTime) const {
    return {item.diagonalVelocity.x * deltaTime, item.diagonalVelocity.y * deltaTime};
}

constexpr float ZIGZAG_VERTICAL_STEP = 1.0f;
constexpr float ZIGZAG_VERTICAL_DURATION = 5.0f; // (int / int)면 캐스팅되지 않고 int가 된다 (1/6 == 0),

//실수 결과 얻으려면 안전하게 두쪽 다 float로 해줘야 함

Point Practice4RectState::calculateZigzagMovement(AnimatedRect& item, float deltaTime) {
    if (item.isZigzagMove)
        return {item.zigzagVelocity.x * deltaTime, 0};
    else {
        if (item.zigzagStep < 1.0f) {
            item.zigzagStep += ZIGZAG_VERTICAL_STEP / ZIGZAG_VERTICAL_DURATION;

            return {0, item.zigzagVelocity.y * deltaTime};
        } else {
            item.isZigzagMove = true;

            return {item.zigzagVelocity.x * deltaTime, 0};
        }
    }
}

Point Practice4RectState::calculateOrbitMovement(AnimatedRect& item, float deltaTime,
                                                 const Point& additionalMovement) const {
    const Point center = item.rect.getCenter();
    const float left = WINDOW_BOUNDS.left + item.rect.getWidth() * 0.5f;
    const float right = WINDOW_BOUNDS.right - item.rect.getWidth() * 0.5f;
    const float bottom = WINDOW_BOUNDS.bottom + item.rect.getHeight() * 0.5f;
    const float top = WINDOW_BOUNDS.top - item.rect.getHeight() * 0.5f;

    if (!item.orbitReachedBorder) {
        Point target = center;

        switch (item.orbitEdge) {
        case OrbitEdge::Top:
            target = {std::clamp(center.x, left, right), top};
            break;
        case OrbitEdge::Right:
            target = {right, std::clamp(center.y, bottom, top)};
            break;
        case OrbitEdge::Bottom:
            target = {std::clamp(center.x, left, right), bottom};
            break;
        case OrbitEdge::Left:
            target = {left, std::clamp(center.y, bottom, top)};
            break;
        }

        const Point direction = {target.x - center.x, target.y - center.y};
        const float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        const float step = ORBIT_APPROACH_SPEED * deltaTime;

        Point movement{};

        if (distance <= step) {
            item.orbitReachedBorder = true;
            movement = direction;
        } else {
            movement = {direction.x / distance * step, direction.y / distance * step};
        }

        // Keep other position animations active along the selected edge while
        // orbit moves toward it. Orbit alone owns the perpendicular axis.
        if (item.orbitEdge == OrbitEdge::Top || item.orbitEdge == OrbitEdge::Bottom) {
            movement.x += additionalMovement.x;
        } else {
            movement.y += additionalMovement.y;
        }

        return movement;
    }

    float additionalClockwiseStep = 0.0f;

    switch (item.orbitEdge) {
    case OrbitEdge::Top:
        additionalClockwiseStep = additionalMovement.x;
        break;
    case OrbitEdge::Right:
        additionalClockwiseStep = -additionalMovement.y;
        break;
    case OrbitEdge::Bottom:
        additionalClockwiseStep = -additionalMovement.x;
        break;
    case OrbitEdge::Left:
        additionalClockwiseStep = additionalMovement.y;
        break;
    }

    // Position animations are combined as edge speed. They may accelerate or
    // slow orbit, but they cannot pull the rectangle away from the perimeter
    // or reverse its clockwise direction.
    const float step = std::max(0.0f, ORBIT_EDGE_SPEED * deltaTime + additionalClockwiseStep);
    Point movement = {0.0f, 0.0f};

    switch (item.orbitEdge) {
    case OrbitEdge::Top: {
        movement.y = top - center.y;

        const float distanceToCorner = std::max(0.0f, right - center.x);

        if (step <= distanceToCorner) {
            movement.x = step;
        } else {
            movement.x = distanceToCorner;
            movement.y -= step - distanceToCorner;
            item.orbitEdge = OrbitEdge::Right;
        }
        break;
    }
    case OrbitEdge::Right: {
        movement.x = right - center.x;

        const float distanceToCorner = std::max(0.0f, center.y - bottom);

        if (step <= distanceToCorner) {
            movement.y = -step;
        } else {
            movement.y = -distanceToCorner;
            movement.x -= step - distanceToCorner;
            item.orbitEdge = OrbitEdge::Bottom;
        }
        break;
    }
    case OrbitEdge::Bottom: {
        movement.y = bottom - center.y;

        const float distanceToCorner = std::max(0.0f, center.x - left);

        if (step <= distanceToCorner) {
            movement.x = -step;
        } else {
            movement.x = -distanceToCorner;
            movement.y += step - distanceToCorner;
            item.orbitEdge = OrbitEdge::Left;
        }
        break;
    }
    case OrbitEdge::Left: {
        movement.x = left - center.x;

        const float distanceToCorner = std::max(0.0f, top - center.y);

        if (step <= distanceToCorner) {
            movement.y = step;
        } else {
            movement.y = distanceToCorner;
            movement.x += step - distanceToCorner;
            item.orbitEdge = OrbitEdge::Top;
        }
        break;
    }
    }

    return movement;
}

constexpr float zigzagNormalSpeed = 0.3f;

void Practice4RectState::moveWithinWindow(AnimatedRect& item, const Point& movement) {
    Rect& rect = item.rect;
    const Point center = rect.getCenter();
    const Point desiredCenter{center.x + movement.x, center.y + movement.y};

    const float halfWidth = rect.getWidth() * 0.5f;
    const float halfHeight = rect.getHeight() * 0.5f;

    const bool hitLeft = not isOrbit_ && desiredCenter.x - halfWidth < WINDOW_BOUNDS.left;
    const bool hitRight = not isOrbit_ && desiredCenter.x + halfWidth > WINDOW_BOUNDS.right;

    if (hitLeft) {
        if (isDiagonal_ && item.diagonalVelocity.x < 0.0f) {
            item.diagonalVelocity.x = -item.diagonalVelocity.x;
        }

        if (isHorizontalZigzag_ && item.isZigzagMove && item.zigzagVelocity.x < 0.0f) {
            item.zigzagVelocity.x = -item.zigzagVelocity.x;
            item.isZigzagMove = false;
            item.zigzagStep = 0.0f;
            item.zigzagVelocity.y =
                item.zigzagVelocity.y == 0.0f
                    ? zigzagNormalSpeed
                    : ((item.zigzagVelocity.y == zigzagNormalSpeed or item.zigzagVelocity.y == -zigzagNormalSpeed)
                           ? item.zigzagVelocity.y
                           : item.zigzagVelocity.y / 4);
        }
    } else if (hitRight) {
        if (isDiagonal_ && item.diagonalVelocity.x > 0.0f) {
            item.diagonalVelocity.x = -item.diagonalVelocity.x;
        }

        if (isHorizontalZigzag_ && item.isZigzagMove && item.zigzagVelocity.x > 0.0f) {
            item.zigzagVelocity.x = -item.zigzagVelocity.x;
            item.isZigzagMove = false;
            item.zigzagStep = 0.0f;
            item.zigzagVelocity.y =
                item.zigzagVelocity.y == 0.0f
                    ? zigzagNormalSpeed
                    : ((item.zigzagVelocity.y == zigzagNormalSpeed or item.zigzagVelocity.y == -zigzagNormalSpeed)
                           ? item.zigzagVelocity.y
                           : item.zigzagVelocity.y / 4);
        }
    }

    const bool hitBottom = not isOrbit_ && desiredCenter.y - halfHeight < WINDOW_BOUNDS.bottom;
    const bool hitTop = not isOrbit_ && desiredCenter.y + halfHeight > WINDOW_BOUNDS.top;

    if (hitBottom) {
        if (isDiagonal_ && item.diagonalVelocity.y < 0.0f) {
            item.diagonalVelocity.y = -item.diagonalVelocity.y;
        }

        if (isHorizontalZigzag_ and not item.isZigzagMove and item.zigzagVelocity.y < 0) {
            item.zigzagVelocity.y = -item.zigzagVelocity.y * 4;
        }
    } else if (hitTop) {
        if (isDiagonal_ && item.diagonalVelocity.y > 0.0f) {
            item.diagonalVelocity.y = -item.diagonalVelocity.y;
        }

        if (isHorizontalZigzag_ and not item.isZigzagMove and item.zigzagVelocity.y > 0) {
            item.zigzagVelocity.y = -item.zigzagVelocity.y * 4;
        }
    }

    rect.moveCenterWithin(desiredCenter, WINDOW_BOUNDS);
}

void Practice4RectState::updateResize(AnimatedRect& item, float deltaTime) {
    const float halfWidth = item.rect.getWidth() * 0.5f;
    const float halfHeight = item.rect.getHeight() * 0.5f;

    if (item.growing && (halfWidth >= MAX_HALF_SIZE || halfHeight >= MAX_HALF_SIZE)) {
        item.growing = false;
    } else if (!item.growing && (halfWidth <= MIN_HALF_SIZE || halfHeight <= MIN_HALF_SIZE)) {
        item.growing = true;
    }

    const float direction = item.growing ? 1.0f : -1.0f;
    const float amount = RESIZE_SPEED * deltaTime * direction;
    const float resizedHalfWidth = std::clamp(halfWidth + amount, MIN_HALF_SIZE, MAX_HALF_SIZE);
    const float resizedHalfHeight = std::clamp(halfHeight + amount, MIN_HALF_SIZE, MAX_HALF_SIZE);
    const Point center = item.rect.getCenter();
    Rect resized = Rect::fromCenter(center, resizedHalfWidth, resizedHalfHeight, item.rect.getColor());

    resized.moveCenterWithin(center, WINDOW_BOUNDS);
    item.rect = resized;

    if (item.growing && (resizedHalfWidth >= MAX_HALF_SIZE || resizedHalfHeight >= MAX_HALF_SIZE)) {
        item.growing = false;
    } else if (!item.growing && (resizedHalfWidth <= MIN_HALF_SIZE || resizedHalfHeight <= MIN_HALF_SIZE)) {
        item.growing = true;
    }
}

void Practice4RectState::updateRecolor(AnimatedRect& item, float deltaTime) {
    item.recolorElapsed += deltaTime;

    if (item.recolorElapsed < RECOLOR_INTERVAL) {
        return;
    }

    item.rect.setRandomColor();
    item.recolorElapsed = 0.0f;
}

void Practice4RectState::stopAll() {
    isDiagonal_ = false;
    isHorizontalZigzag_ = false;
    isOrbit_ = false;
    isResize_ = false;
    isRecolor_ = false;
}

void Practice4RectState::printFlag() {
    std::print("diag : {0}, horiz: {1}, orbit: {2}, resize: {3}, recol: {4}\n", isDiagonal_, isHorizontalZigzag_,
               isOrbit_, isResize_, isRecolor_);
}

const std::vector<AnimatedRect>& Practice4RectState::getItems() const {
    return store_.getItems();
}

//start practice5

Practice5RectState::Practice5RectState() {
    reset();
}

void Practice5RectState::reset() {
    store_.clear();
    createdRectCount_ = 0;
    createInitialRects();
    store_.captureCurrentState();
}

const std::vector<AnimatedRect>& Practice5RectState::getItems() const {
    return store_.getItems();
}

const std::optional<Rect>& Practice5RectState::getEraser() const {
    return eraser_;
}

void Practice5RectState::createInitialRects() {
    const std::size_t rectCount = randomIndex(MIN_PRACTICE5_RECTS, MAX_PRACTICE5_RECTS);

    for (std::size_t i = 0; i < rectCount; ++i) {
        pushRandomRect();
    }
}

void Practice5RectState::pushRandomRect() {
    const Point center{
        randomFloat(WINDOW_BOUNDS.left + PRACTICE5_RECT_HALF_SIZE, WINDOW_BOUNDS.right - PRACTICE5_RECT_HALF_SIZE),
        randomFloat(WINDOW_BOUNDS.bottom + PRACTICE5_RECT_HALF_SIZE, WINDOW_BOUNDS.top - PRACTICE5_RECT_HALF_SIZE)};

    GlColor color;

    color.setRandom();
    store_.push(Rect::fromCenter(center, PRACTICE5_RECT_HALF_SIZE, PRACTICE5_RECT_HALF_SIZE, color));
}

void Practice5RectState::pushRectAt(const Point& point) {
    GlColor color;

    color.setRandom();

    Rect rect = Rect::fromCenter(point, PRACTICE5_RECT_HALF_SIZE, PRACTICE5_RECT_HALF_SIZE, color);

    rect.moveCenterWithin(point, WINDOW_BOUNDS);
    store_.push(rect);
    store_.updateCapture(rect);
}

std::vector<std::size_t> Practice5RectState::findOverlaps() const {
    const std::vector<AnimatedRect>& rects = store_.getItems();

    if (!eraser_.has_value()) {
        return {};
    }

    std::vector<std::size_t> overlappingIndices;
    // Search from the back so erasing does not invalidate earlier indices.
    for (std::size_t i = rects.size(); i > 0; --i) {
        const std::size_t index = i - 1;

        if (eraser_->overlaps(rects[index].rect)) {
            overlappingIndices.push_back(index);
        }
    }

    return overlappingIndices;
}

bool Practice5RectState::tryCreateEraserAt(const Point& point) {
    if (eraser_.has_value()) {
        return false;
    }

    const GlColor black{0.0f, 0.0f, 0.0f};

    eraser_ = Rect::fromCenter(point, PRACTICE5_ERASER_INITIAL_HALF_SIZE, PRACTICE5_ERASER_INITIAL_HALF_SIZE, black);
    eraser_->moveCenterWithin(point, WINDOW_BOUNDS);

    return true;
}

bool Practice5RectState::tryCreateRectAt(const Point& point) {
    if (createdRectCount_ >= MAX_PRACTICE5_CREATED_RECTS) {
        return false;
    }

    pushRectAt(point);
    ++createdRectCount_;

    if (eraser_.has_value()) {
        resizeEraser(-PRACTICE5_ERASER_RESIZE_STEP);
    }

    return true;
}

bool Practice5RectState::beginDrag(const Point& point) {
    if (!tryCreateEraserAt(point)) {
        return false;
    }

    dragTo(point);

    return true;
}

void Practice5RectState::dragTo(const Point& point) {
    if (!eraser_.has_value()) {
        return;
    }

    eraser_->moveCenterWithin(point, WINDOW_BOUNDS);

    const std::vector<std::size_t> erasedIndices = findOverlaps();

    for (std::size_t index : erasedIndices) {
        const GlColor erasedColor = store_.get(index).rect.getColor();

        eraser_->resetAssignedColorAt(erasedColor);
        resizeEraser(PRACTICE5_ERASER_RESIZE_STEP);
        store_.erase(index);
    }
}

bool Practice5RectState::endDrag() {
    if (!eraser_.has_value()) {
        return false;
    }

    store_.restoreCapturedState();
    eraser_.reset();
    store_.captureCurrentState();

    return true;
}

bool Practice5RectState::resizeEraser(float halfSizeDelta) {
    if (!eraser_.has_value()) {
        return false;
    }

    const float maximumHalfWidth = (WINDOW_BOUNDS.right - WINDOW_BOUNDS.left) * 0.5f;
    const float maximumHalfHeight = (WINDOW_BOUNDS.top - WINDOW_BOUNDS.bottom) * 0.5f;

    const float halfWidth =
        std::clamp(eraser_->getWidth() * 0.5f + halfSizeDelta * 0.3f, PRACTICE5_ERASER_MIN_HALF_SIZE, maximumHalfWidth);
    const float halfHeight = std::clamp(eraser_->getHeight() * 0.5f + halfSizeDelta * 0.3f,
                                        PRACTICE5_ERASER_MIN_HALF_SIZE, maximumHalfHeight);

    const Point center = eraser_->getCenter();
    Rect resized = Rect::fromCenter(center, halfWidth, halfHeight, eraser_->getColor());

    resized.moveCenterWithin(center, WINDOW_BOUNDS);
    eraser_ = resized;

    return true;
}

// Practice 6: each clicked rectangle owns its source template and all of its fragments.
Practice6RectState::Practice6RectState() {
    createInitialRects();
}

void Practice6RectState::createInitialRects() {
    const std::size_t count = randomIndex(MIN_PRACTICE6_RECTS, MAX_PRACTICE6_RECTS);

    for (std::size_t i = 0; i < count; ++i) {
        const float halfWidth = randomFloat(PRACTICE6_MIN_INITIAL_HALF_SIZE, PRACTICE6_MAX_INITIAL_HALF_SIZE);
        const float halfHeight = randomFloat(PRACTICE6_MIN_INITIAL_HALF_SIZE, PRACTICE6_MAX_INITIAL_HALF_SIZE);
        const Point center{randomFloat(WINDOW_BOUNDS.left + halfWidth, WINDOW_BOUNDS.right - halfWidth),
                           randomFloat(WINDOW_BOUNDS.bottom + halfHeight, WINDOW_BOUNDS.top - halfHeight)};
        GlColor color;

        color.setRandom();
        store_.push(Rect::fromCenter(center, halfWidth, halfHeight, color));
    }
}

bool Practice6RectState::tryCreateAnimationAt(const Point& point) {
    const std::optional<RectStore::Index> index = store_.findTopmostAt(point);

    if (!index.has_value()) {
        return false;
    }

    splitRectToAnimationAt(*index);

    return true;
}

void Practice6RectState::splitRectToAnimationAt(RectStore::Index index) {
    Animation animation{store_.get(index).rect};

    createSplitRects(animation);
    animations_.push_back(std::move(animation));
    store_.erase(index);
}

void Practice6RectState::createSplitRects(Animation& animation) {
    const Rect& source = animation.source;
    const float cellWidth = source.getWidth() / 3.0f;
    const float cellHeight = source.getHeight() / 3.0f;

    for (int row = 0; row < 3; ++row) {
        for (int column = 0; column < 3; ++column) {
            if (row == 1 && column == 1) {
                continue;
            }

            const Point minimum{source.getLeft() + column * cellWidth, source.getBottom() + row * cellHeight};
            const Point maximum{minimum.x + cellWidth, minimum.y + cellHeight};
            Point direction{static_cast<float>(column - 1), static_cast<float>(row - 1)};
            const float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);

            direction.x /= length;
            direction.y /= length;

            animation.splitRects.push_back({Rect(minimum, maximum, source.getColor()), direction});
        }
    }
}

void Practice6RectState::updateAnimation(Animation& animation, float deltaTime) {
    for (std::size_t i = animation.splitRects.size(); i > 0; --i) {
        SplitRect& fragment = animation.splitRects[i - 1];
        const Rect& rect = fragment.rect;
        const float halfWidth = rect.getWidth() * 0.5f - PRACTICE6_SHRINK_SPEED * deltaTime;
        const float halfHeight = rect.getHeight() * 0.5f - PRACTICE6_SHRINK_SPEED * deltaTime;

        if (halfWidth <= PRACTICE6_MIN_HALF_SIZE || halfHeight <= PRACTICE6_MIN_HALF_SIZE) {
            animation.splitRects.erase(animation.splitRects.begin() + static_cast<std::ptrdiff_t>(i - 1));
            animation.emitting = false;
            continue;
        }

        const Point center = rect.getCenter();
        const Point nextCenter{center.x + fragment.direction.x * PRACTICE6_MOVE_SPEED * deltaTime,
                               center.y + fragment.direction.y * PRACTICE6_MOVE_SPEED * deltaTime};
        const GlColor color = rect.getColor();
        const GlColor nextColor{std::max(0.0f, color.red - PRACTICE6_COLOR_FADE_SPEED * deltaTime),
                                std::max(0.0f, color.green - PRACTICE6_COLOR_FADE_SPEED * deltaTime),
                                std::max(0.0f, color.blue - PRACTICE6_COLOR_FADE_SPEED * deltaTime)};

        fragment.rect = Rect::fromCenter(nextCenter, halfWidth, halfHeight, nextColor);
    }

    if (animation.emitting) {
        animation.emissionElapsed += deltaTime;

        while (animation.emissionElapsed >= PRACTICE6_EMISSION_INTERVAL) {
            createSplitRects(animation);
            animation.emissionElapsed -= PRACTICE6_EMISSION_INTERVAL;
        }
    }
}

void Practice6RectState::update(float deltaTime) {
    if (deltaTime <= 0.0f) {
        return;
    }

    for (std::size_t i = animations_.size(); i > 0; --i) {
        Animation& animation = animations_[i - 1];

        updateAnimation(animation, deltaTime);

        if (animation.splitRects.empty()) {
            animations_.erase(animations_.begin() + static_cast<std::ptrdiff_t>(i - 1));
        }
    }
}

const std::vector<AnimatedRect>& Practice6RectState::getItems() const {
    return store_.getItems();
}

const std::vector<Practice6RectState::Animation>& Practice6RectState::getAnimations() const {
    return animations_;
}
