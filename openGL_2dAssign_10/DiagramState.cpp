#include "DiagramState.h"

namespace {

constexpr float EPSILON = 0.00001f;
constexpr float SNAP_DISTANCE_PIXELS = 24.0f;
constexpr float SOURCE_MARGIN = 0.02f;

// 실제 경계는 모든 월드 정점에서 계산한다.
VertexBounds boundsAt(const Diagram& diagram) {
    VertexBounds bounds;

    for (Point vertex : diagram.worldVertices()) {
        bounds.left = std::min(bounds.left, vertex.x);
        bounds.right = std::max(bounds.right, vertex.x);
        bounds.bottom = std::min(bounds.bottom, vertex.y);
        bounds.top = std::max(bounds.top, vertex.y);
    }

    return bounds;
}

bool insideRegion(const Diagram& diagram, float left, float right, float bottom, float top) {
    if (!diagram.fitsNdc()) {
        return false;
    }

    const VertexBounds bounds = boundsAt(diagram);

    return bounds.left >= left && bounds.right <= right && bounds.bottom >= bottom && bounds.top <= top;
}

std::vector<Point> perimeter(const Diagram& diagram) {
    const std::vector<Point> vertices = diagram.worldVertices();

    if (vertices.size() == 4) {
        return {vertices[0], vertices[1], vertices[3], vertices[2]};
    }

    return vertices;
}

float cross(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool onSegment(Point p, Point a, Point b) {
    return std::abs(cross(a, b, p)) <= EPSILON && p.x >= std::min(a.x, b.x) - EPSILON &&
           p.x <= std::max(a.x, b.x) + EPSILON && p.y >= std::min(a.y, b.y) - EPSILON &&
           p.y <= std::max(a.y, b.y) + EPSILON;
}

bool segmentsIntersect(Point a, Point b, Point c, Point d) {
    const float abC = cross(a, b, c);
    const float abD = cross(a, b, d);
    const float cdA = cross(c, d, a);
    const float cdB = cross(c, d, b);

    if (((abC > EPSILON && abD < -EPSILON) || (abC < -EPSILON && abD > EPSILON)) &&
        ((cdA > EPSILON && cdB < -EPSILON) || (cdA < -EPSILON && cdB > EPSILON))) {
        return true;
    }

    return onSegment(c, a, b) || onSegment(d, a, b) || onSegment(a, c, d) || onSegment(b, c, d);
}

bool diagramsOverlap(const Diagram& a, const Diagram& b) {
    const VertexBounds ab = boundsAt(a);
    const VertexBounds bb = boundsAt(b);

    if (ab.right < bb.left || bb.right < ab.left || ab.top < bb.bottom || bb.top < ab.bottom) {
        return false;
    }

    const std::vector<Point> ap = perimeter(a);
    const std::vector<Point> bp = perimeter(b);

    if (ap.empty() || bp.empty()) {
        return false;
    }

    for (std::size_t i = 0; i < ap.size(); ++i) {
        for (std::size_t j = 0; j < bp.size(); ++j) {
            if (segmentsIntersect(ap[i], ap[(i + 1) % ap.size()], bp[j], bp[(j + 1) % bp.size()])) {
                return true;
            }
        }
    }

    return a.contains(bp[0]) || b.contains(ap[0]);
}

bool overlapsItems(const Diagram& candidate, const std::vector<Diagram>& items) {
    for (const Diagram& existing : items) {
        // 초기 배치 검사 대상은 좌측의 이동 조각이다.
        if (existing.isMovable() && diagramsOverlap(candidate, existing)) {
            return true;
        }
    }

    return false;
}

std::vector<Point> squareVertices(float side) {
    const float half = side / 2.0f;

    return {{-half, -half}, {half, -half}, {-half, half}, {half, half}};
}

Point centroid(const std::vector<Point>& vertices) {
    Point result{};

    for (Point p : vertices) {
        result.x += p.x;
        result.y += p.y;
    }

    if (!vertices.empty()) {
        result.x /= static_cast<float>(vertices.size());
        result.y /= static_cast<float>(vertices.size());
    }

    return result;
}

// 템플릿 좌표 -> Fit. 같은 로컬 데이터를 복사해 Unfit을 만든다.
Diagram makeTarget(GLShape shape, TriangleCase triangleCase, const std::vector<Point>& vertices, Point pivot,
                   std::size_t boardId, std::size_t pairId) {
    GlColor color;

    color.setRandom();

    const Point localPivot = centroid(vertices);
    Diagram target = shape == GLShape::Triangle
                         ? Diagram(triangleCase, vertices, color, pivot, localPivot, BoundsCase::Fit)
                         : Diagram(shape, vertices, color, pivot, localPivot, BoundsCase::Fit);

    target.setIdentity(pairId, boardId);

    return target;
}

struct SourceCell {
    float left;
    float right;
    float bottom;
    float top;
};

std::vector<SourceCell> shuffledSourceCells() {
    constexpr int COLUMNS = 4;
    constexpr int ROWS = 5;
    constexpr float LEFT = -0.96f;
    constexpr float RIGHT = REGION_SPLIT_X - 0.04f;
    constexpr float BOTTOM = -0.96f;
    constexpr float TOP = 0.96f;
    const float width = (RIGHT - LEFT) / COLUMNS;
    const float height = (TOP - BOTTOM) / ROWS;
    std::vector<SourceCell> cells;

    cells.reserve(COLUMNS * ROWS);

    for (int row = 0; row < ROWS; ++row) {
        for (int column = 0; column < COLUMNS; ++column) {
            const float left = LEFT + column * width;
            const float bottom = BOTTOM + row * height;

            cells.push_back({left, left + width, bottom, bottom + height});
        }
    }
    // Fisher-Yates: 배열의 각 칸을 한 번씩 섞는다.
    for (std::size_t i = cells.size(); i > 1; --i) {
        const std::size_t j = static_cast<std::size_t>(randomInt(0, static_cast<int>(i - 1)));

        std::swap(cells[i - 1], cells[j]);
    }

    return cells;
}

bool appendPair(const Diagram& target, const SourceCell& cell, std::vector<Diagram>& items) {
    if (items.size() + 2 > MAX_DIAGRAMS || !insideRegion(target, REGION_SPLIT_X + 0.02f, 0.98f, -0.98f, 0.98f)) {
        return false;
    }
    // 중심을 원점으로 옮겨 변환 후 도형의 실제 크기·비대칭 범위를 구한다.
    Diagram source = target.makeUnfitCopy();
    const VertexBounds relative = boundsAt(source);
    const float minimumX = cell.left + SOURCE_MARGIN - relative.left;
    const float maximumX = cell.right - SOURCE_MARGIN - relative.right;
    const float minimumY = cell.bottom + SOURCE_MARGIN - relative.bottom;
    const float maximumY = cell.top - SOURCE_MARGIN - relative.top;

    if (minimumX > maximumX || minimumY > maximumY) {
        return false;
    }
    // 재시도는 제한한다. 실패 시 호출자가 기존 Store를 유지한다.
    for (int attempt = 0; attempt < 16; ++attempt) {
        const Point pivot{randomFloat(minimumX, maximumX), randomFloat(minimumY, maximumY)};

        if (source.setPose(pivot, target.getRotation()) &&
            insideRegion(source, cell.left + SOURCE_MARGIN, cell.right - SOURCE_MARGIN, cell.bottom + SOURCE_MARGIN,
                         cell.top - SOURCE_MARGIN) &&
            !overlapsItems(source, items)) {
            items.push_back(target);
            items.push_back(source);

            return true;
        }
    }

    return false;
}

} // namespace

std::vector<Point> setRandomTriPointByCase(TriangleCase case_, float width, float height) {
    std::vector<Point> vertices;

    vertices.reserve(3);

    if (width <= 0.0f || height <= 0.0f) {
        return vertices;
    }

    if (case_ == TriangleCase::Equilateral) {
        const float h = std::sqrt(3.0f) * width / 2.0f;

        vertices = {{-width / 2.0f, 0.0f}, {width / 2.0f, 0.0f}, {0.0f, h}};
    } else if (case_ == TriangleCase::RightAngled) {
        vertices = {{0.0f, 0.0f}, {width, 0.0f}, {0.0f, height}};
    }

    return vertices;
}

float DiagramStore::getSafeLength(float maximum) const {
    return randomFloat(maximum * 0.85f, maximum);
}

bool DiagramStore::overlap(const Diagram& diagram) const {
    return overlapsItems(diagram, diagrams_);
}

bool DiagramStore::resetAtRandom() {
    const DiagCaseTable nextTable = makeDCT();
    const std::vector<SourceCell> cells = shuffledSourceCells();
    std::vector<Diagram> targets;

    targets.reserve(MAX_DIAGRAMS / 2);

    std::vector<ShapeBoard> nextBoards{{"Square grid"}, {"Windmill"}, {"Right triangles"}, {"House"}, {"Arrow"}};
    const Point centers[BOARD_COUNT] = {{0.4f, 0.65f}, {0.8f, 0.65f}, {0.4f, 0.0f}, {0.8f, 0.0f}, {0.4f, -0.65f}};
    std::size_t nextPairId = 1;

    // 1. 사각형 묶음: 행·열 수가 달라지므로 전체 조각 개수도 달라진다.
    const float gridSide = getSafeLength(0.12f);

    for (int row = 0; row < nextTable.rectRows; ++row) {
        for (int column = 0; column < nextTable.rectColumns; ++column) {
            const Point pivot{centers[0].x + (column - (nextTable.rectColumns - 1) / 2.0f) * gridSide * LOCAL_TO_NDC_X,
                              centers[0].y + (row - (nextTable.rectRows - 1) / 2.0f) * gridSide};

            targets.push_back(
                makeTarget(GLShape::Rect, TriangleCase::None, squareVertices(gridSide), pivot, 0, nextPairId++));
        }
    }

    // 2. 바람개비: 네 정삼각형의 꼭짓점이 가운데를 향한다.
    const float windSide = getSafeLength(0.16f);
    const float windHeight = std::sqrt(3.0f) * windSide / 2.0f;
    const float radius = 2.0f * windHeight / 3.0f + 0.008f;
    const Point windPivots[] = {{centers[1].x, centers[1].y - radius},
                                {centers[1].x + radius * LOCAL_TO_NDC_X, centers[1].y},
                                {centers[1].x, centers[1].y + radius},
                                {centers[1].x - radius * LOCAL_TO_NDC_X, centers[1].y}};
    const std::vector<Point> windVertices = setRandomTriPointByCase(TriangleCase::Equilateral, windSide, windHeight);

    for (int i = 0; i < 4; ++i) {
        Diagram target =
            makeTarget(GLShape::Triangle, TriangleCase::Equilateral, windVertices, windPivots[i], 1, nextPairId++);

        if (!target.setRotation(i * 90.0f)) {
            return false;
        }
        targets.push_back(target);
    }

    // 3. 두 직각삼각형으로 직사각형을 만든다.
    const float rightWidth = getSafeLength(0.20f);
    const float rightHeight = rightWidth * 1.5f;
    const std::vector<Point> rightVertices =
        setRandomTriPointByCase(TriangleCase::RightAngled, rightWidth, rightHeight);

    for (int i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        const Point pivot{centers[2].x + sign * rightWidth / 6.0f * LOCAL_TO_NDC_X,
                          centers[2].y + sign * rightHeight / 6.0f};
        Diagram target =
            makeTarget(GLShape::Triangle, TriangleCase::RightAngled, rightVertices, pivot, 2, nextPairId++);

        if (!target.setRotation(i * 180.0f)) {
            return false;
        }
        targets.push_back(target);
    }

    // 4. 집: 사각형 몸체와 정삼각형 지붕.
    const float houseSide = getSafeLength(0.18f);
    const float roofHeight = std::sqrt(3.0f) * houseSide / 2.0f;
    const Point houseBody{centers[3].x, centers[3].y - roofHeight / 2.0f};

    targets.push_back(
        makeTarget(GLShape::Rect, TriangleCase::None, squareVertices(houseSide), houseBody, 3, nextPairId++));

    const Point houseRoof{houseBody.x, houseBody.y + houseSide / 2.0f + roofHeight / 3.0f};

    targets.push_back(makeTarget(GLShape::Triangle, TriangleCase::Equilateral,
                                 setRandomTriPointByCase(TriangleCase::Equilateral, houseSide, roofHeight), houseRoof,
                                 3, nextPairId++));

    // 5. 화살표: 사각형 몸체와 오른쪽을 향한 정삼각형 머리.
    const float arrowSide = getSafeLength(0.18f);
    const float arrowHeight = std::sqrt(3.0f) * arrowSide / 2.0f;
    const Point arrowBody{centers[4].x - arrowHeight / 2.0f * LOCAL_TO_NDC_X, centers[4].y};

    targets.push_back(
        makeTarget(GLShape::Rect, TriangleCase::None, squareVertices(arrowSide), arrowBody, 4, nextPairId++));

    const Point arrowHead{arrowBody.x + (arrowSide / 2.0f + arrowHeight / 3.0f) * LOCAL_TO_NDC_X, arrowBody.y};
    Diagram head = makeTarget(GLShape::Triangle, TriangleCase::Equilateral,
                              setRandomTriPointByCase(TriangleCase::Equilateral, arrowSide, arrowHeight), arrowHead, 4,
                              nextPairId++);

    if (!head.setRotation(270.0f)) {
        return false;
    }
    targets.push_back(head);

    const std::size_t expectedPairs = nextTable.NumOfEqual + nextTable.NumOfRight + nextTable.NumOfRect;

    if (targets.size() != expectedPairs || targets.size() > cells.size()) {
        return false;
    }

    std::vector<Diagram> nextItems;

    nextItems.reserve(targets.size() * 2);

    for (std::size_t i = 0; i < targets.size(); ++i) {
        if (!appendPair(targets[i], cells[i], nextItems)) {
            return false;
        }
        ++nextBoards[targets[i].getBoardId()].totalSlots;
    }

    // 모든 생성과 검증이 성공한 뒤에만 기존 상태를 교체한다.
    diagrams_ = std::move(nextItems);
    boards_ = std::move(nextBoards);
    dct_ = nextTable;
    selectedIndex_.reset();
    ++revision_;

    return true;
}

void DiagramStore::clear() {
    if (!diagrams_.empty() || !boards_.empty() || selectedIndex_) {
        diagrams_.clear();
        boards_.clear();
        dct_ = {};
        selectedIndex_.reset();
        ++revision_;
    }
}

void DiagramStore::clearSelection() {
    if (selectedIndex_) {
        selectedIndex_.reset();
        ++revision_;
    }
}

void DiagramStore::selectAt(Point point) {
    std::optional<std::size_t> found;

    for (std::size_t i = diagrams_.size(); i > 0; --i) {
        if (diagrams_[i - 1].isMovable() && diagrams_[i - 1].contains(point)) {
            found = i - 1;
            break;
        }
    }

    if (selectedIndex_ != found) {
        selectedIndex_ = found;
        ++revision_;
    }
}

std::optional<Point> DiagramStore::getSelectedPivot() const {
    if (!selectedIndex_ || *selectedIndex_ >= diagrams_.size() || !diagrams_[*selectedIndex_].isMovable()) {
        return std::nullopt;
    }

    return diagrams_[*selectedIndex_].getPivot();
}

bool DiagramStore::moveSelectedTo(Point pivot) {
    const std::optional<Point> oldPivot = getSelectedPivot();

    if (!oldPivot || (oldPivot->x == pivot.x && oldPivot->y == pivot.y)) {
        return false;
    }

    Diagram& diagram = diagrams_[*selectedIndex_];

    if (!diagram.setPose(pivot, diagram.getRotation())) {
        return false;
    }
    ++revision_;

    return true;
}

void DiagramStore::setWindowSize(int width, int height) {
    if (width > 0 && height > 0) {
        windowWidth_ = width;
        windowHeight_ = height;
    }
}

int DiagramStore::findClosestSameShape_IndexAtCloseAndFit(const Diagram& diagram) const {
    if (!diagram.isMovable()) {
        return -1;
    }

    float bestSquared = SNAP_DISTANCE_PIXELS * SNAP_DISTANCE_PIXELS;
    int bestIndex = -1;

    for (std::size_t i = 0; i < diagrams_.size(); ++i) {
        const Diagram& target = diagrams_[i];

        if (target.getBoundCase() != BoundsCase::Fit || target.isMatched() || !diagram.hasSameShape(target)) {
            continue;
        }

        const float dx = (diagram.getPivot().x - target.getPivot().x) * windowWidth_ / 2.0f;
        const float dy = (diagram.getPivot().y - target.getPivot().y) * windowHeight_ / 2.0f;
        const float squared = dx * dx + dy * dy;

        if (squared <= bestSquared && (bestIndex < 0 || squared < bestSquared)) {
            bestSquared = squared;
            bestIndex = static_cast<int>(i);
        }
    }

    return bestIndex;
}

bool DiagramStore::mergeSelectedDiagram() {
    if (!getSelectedPivot()) {
        return false;
    }

    const std::size_t sourceIndex = *selectedIndex_;
    const int found = findClosestSameShape_IndexAtCloseAndFit(diagrams_[sourceIndex]);

    if (found < 0) {
        return false;
    }

    const std::size_t targetIndex = static_cast<std::size_t>(found);
    const std::size_t boardId = diagrams_[targetIndex].getBoardId();

    if (boardId >= boards_.size()) {
        return false;
    }
    // 삭제 전에 목표와 모양판을 갱신한다. 삭제 후 참조·인덱스를 재사용하지 않는다.
    diagrams_[targetIndex].markMatched(diagrams_[sourceIndex].getColor());

    ShapeBoard& board = boards_[boardId];

    ++board.matchedSlots;
    board.completed = board.matchedSlots == board.totalSlots;
    diagrams_.erase(diagrams_.begin() + static_cast<std::ptrdiff_t>(sourceIndex));
    selectedIndex_.reset();
    ++revision_;

    return true;
}

std::size_t DiagramStore::getCompletedBoardCount() const {
    std::size_t count = 0;

    for (const ShapeBoard& board : boards_) {
        if (board.completed) {
            ++count;
        }
    }

    return count;
}

bool DiagramStore::translateSelected(Point direction) {
    if (!getSelectedPivot() || !diagrams_[*selectedIndex_].translate(direction)) {
        return false;
    }
    ++revision_;

    return true;
}

bool DiagramStore::translateAll(Point direction) {
    bool hasMovable = false;

    for (const Diagram& diagram : diagrams_) {
        if (diagram.isMovable()) {
            hasMovable = true;

            if (!diagram.canTranslate(direction)) {
                return false;
            }
        }
    }

    if (!hasMovable) {
        return false;
    }

    for (Diagram& diagram : diagrams_) {
        if (diagram.isMovable()) {
            diagram.translate(direction);
        }
    }
    ++revision_;

    return true;
}

bool DiagramStore::setSelectedScale(Point scale) {
    if (!getSelectedPivot() || !diagrams_[*selectedIndex_].setScale(scale)) {
        return false;
    }
    ++revision_;

    return true;
}

bool DiagramStore::setSelectedRotation(float degrees) {
    if (!getSelectedPivot() || !diagrams_[*selectedIndex_].setRotation(degrees)) {
        return false;
    }
    ++revision_;

    return true;
}

bool DiagramStore::setSelectedDirection(Point direction, float speed) {
    if (!getSelectedPivot()) {
        return false;
    }
    diagrams_[*selectedIndex_].setDirection(direction, speed);
    ++revision_;

    return true;
}

bool DiagramStore::advance(float deltaTime) {
    bool changed = false;

    for (Diagram& diagram : diagrams_) {
        if (diagram.isMovable()) {
            changed = diagram.advance(deltaTime) || changed;
        }
    }

    if (changed) {
        ++revision_;
    }

    return changed;
}
