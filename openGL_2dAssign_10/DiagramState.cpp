#include "DiagramState.h"



constexpr float INNER_BOUND = 0.999f;


VertexBounds boundsAt(const Diagram& diagram, Point pivot, float degrees) {
    VertexBounds bounds;

    switch (diagram.getBoundCase()) {//작성해야 함
    case BoundsCase::Fit:
        bounds.setFit();
        break;
    case BoundsCase::Unfit:
        bounds.setUnfit();
        break;
    default:
    }


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
    }
    else if (bounds.right > INNER_BOUND) {
        pivot.x += INNER_BOUND - bounds.right;
    }
    if (bounds.bottom < -INNER_BOUND) {
        pivot.y += -INNER_BOUND - bounds.bottom;
    }
    else if (bounds.top > INNER_BOUND) {
        pivot.y += INNER_BOUND - bounds.top;
    }
    return pivot;
}


bool DiagramStore::overlap(const Diagram& diagram) const {//작성해야 함

}

size_t DiagramStore::getSafeLenghth() const {//작성해야 함

}


bool DiagramStore::push(const Diagram& diagram) {
    if (diagrams_.size() >= MAX_DIAGRAMS or not diagram.fitsNdc()) {
        return false;
    }
    diagrams_.push_back(diagram);
    ++revision_;
    return true;
}


void DiagramStore::resetTable() {
    fitRest = dct;
    unfitRest = dct;
}

Shape DiagramStore::getFitTableMember() const{
    if (0 < fitRest.NumOfEqual) return Shape{ GLShape::Triangle, TriangleCase::Equilateral };
    if(0 < fitRest.NumOfRight) return Shape{ GLShape::Triangle, TriangleCase::RightAngled };
    if (0 < fitRest.NumOfSpot) return Shape{ GLShape::Spot, TriangleCase::None };
    if(0 < fitRest.NumOfLine) return Shape{ GLShape::Line, TriangleCase::None };
    if (0 < fitRest.NumOfRect) return Shape{ GLShape::Rect, TriangleCase::None };
    return Shape{ GLShape::None, TriangleCase::None };
} 

Shape DiagramStore::getUnfitTableMember() const {
    if (0 < unfitRest.NumOfEqual) return Shape{ GLShape::Triangle, TriangleCase::Equilateral };
    if (0 < unfitRest.NumOfRight) return Shape{ GLShape::Triangle, TriangleCase::RightAngled };
    if (0 < unfitRest.NumOfSpot) return Shape{ GLShape::Spot, TriangleCase::None };
    if (0 < unfitRest.NumOfLine) return Shape{ GLShape::Line, TriangleCase::None };
    if (0 < unfitRest.NumOfRect) return Shape{ GLShape::Rect, TriangleCase::None };
    return Shape{ GLShape::None, TriangleCase::None };

}


std::vector<Point> setRandomTriPointByCase(const TriangleCase& case_) {//작성해야 함
    /*
    h = getSafeLength();
    s = getSafeLength();

    * 크기를 한 번 무작위로 정하고, 종류에 맞는 세 점을 반환합니다.
- 정삼각형: 한 변 길이가 s, 높이가 h = sqrt(3) * s / 2이면
  (-s/2, 0), (s/2, 0), (0, h)
- 직각삼각형: 두 직각변 길이가 w, h이면
  (0, 0), (w, 0), (0, h)
세 점을 각각 무작위로 만들면 해당 삼각형 조건을 보장할 수 없습니다. 길이를 무작위로 정하고 점은 계산하세요.
    */
    switch (case_) {
    case TriangleCase::Equilateral:
        //

    }
}

Diagram DiagramStore::makeDiagram(GLShape shape, TriangleCase case_) {// 수정해야 함, 항상 pivot을 2번 구한다: fit과 unfit으로 구한 뒤, push도 2번 할 예정.
    const Point pivot{//getTable로 
        randomFloat(-0.75f, 0.75f),
        randomFloat(-0.75f, 0.75f)
    };
    GlColor color;

    color.setRandom();
    const float half = randomFloat(0.08f, 0.17f);

    std::vector<Point> triPoint;
    triPoint.reserve(3);
    triPoint = setRandomTriPointByCase(case_);
    

    switch (shape) {
    case GLShape::Spot:
        return Diagram(shape, std::vector<Point>{
            { -0.02f, -0.02f }, { 0.02f, -0.02f },
            { -0.02f, 0.02f }, { 0.02f, 0.02f }
        }, color, pivot, {}, BoundsCase::Unfit);
    case GLShape::Line:
        return Diagram(shape, std::vector<Point>{
            { -half, 0.0f }, { half, 0.0f }
        }, color, pivot, {}, BoundsCase::Unfit);
    case GLShape::Triangle:
        Point triPivot{};
        for (Point p : triPoint) {
            triPivot.x += p.x;
            triPivot.y += p.y;
        }
        triPivot.x /= 3.0f;
        triPivot.y /= 3.0f;
        return Diagram(case_, triPoint, color, pivot, triPivot, BoundsCase::Unfit);

    case GLShape::Rect:
        return Diagram(shape, std::vector<Point>{
            { -half, -half }, { half, -half },
            { -half, half }, { half, half }
        }, color, pivot, {}, BoundsCase::Unfit);
    }
    return Diagram(GLShape::Triangle, std::vector<Point>{
        { -half, -half }, { half, -half }, { 0.0f, half }
    }, color, pivot, {}, BoundsCase::Unfit);
}
 // namespace





bool DiagramStore::replaceAt(std::size_t index, const Diagram& diagram) {
    if (index >= diagrams_.size() || !diagram.fitsNdc()) {
        return false;
    }
    diagrams_[index] = diagram;
    ++revision_;
    return true;
}

void DiagramStore::clear() {
    if (!diagrams_.empty() || selectedIndex_) {
        diagrams_.clear();
        selectedIndex_.reset();
        ++revision_;
    }
}


bool DiagramStore::resetAtRandom() {//작성해야 함
    clear();
    dct = makeDCT();
    resetTable();
    //table 만큼씩 push(makeDiagram())해야 함

}



void DiagramStore::selectAt(Point point) {
    std::optional<std::size_t> found;
    for (std::size_t i = diagrams_.size(); i > 0; --i) {
        if (diagrams_[i - 1].contains(point)) {
            found = i - 1; // Last drawn diagram is selected first.
            break;
        }
    }
    if (selectedIndex_ != found) {
        selectedIndex_ = found;
        ++revision_; // Selection also changes the one-object EBO.
    }
}

bool DiagramStore::translateSelected(Point direction) {
    if (!selectedIndex_) {
        return false;
    }
    if (!diagrams_[*selectedIndex_].translate(direction)) {
        return false;
    }
    ++revision_;
    return true;
}

bool DiagramStore::translateAll(Point direction) {
    if (diagrams_.empty()) {
        return false;
    }
    for (const Diagram& diagram : diagrams_) {
        if (!diagram.canTranslate(direction)) {
            return false;
        }
    }
    for (Diagram& diagram : diagrams_) {
        diagram.translate(direction);
    }
    ++revision_;
    return true;
}

bool DiagramStore::setSelectedScale(Point scale) {
    if (!selectedIndex_) {
        return false;
    }
    if (!diagrams_[*selectedIndex_].setScale(scale)) {
        return false;
    }
    ++revision_;
    return true;
}

bool DiagramStore::setSelectedRotation(float degrees) {
    if (!selectedIndex_) {
        return false;
    }
    if (!diagrams_[*selectedIndex_].setRotation(degrees)) {
        return false;
    }
    ++revision_;
    return true;
}

bool DiagramStore::setSelectedDirection(Point direction, float speed) {
    if (!selectedIndex_) {
        return false;
    }
    diagrams_[*selectedIndex_].setDirection(direction, speed);
    ++revision_;
    return true;
}

bool DiagramStore::advance(float deltaTime) {
    bool changed = false;
    for (Diagram& diagram : diagrams_) {
        changed = diagram.advance(deltaTime) || changed;
    }
    if (changed) {
        ++revision_;
    }
    return changed;
}
