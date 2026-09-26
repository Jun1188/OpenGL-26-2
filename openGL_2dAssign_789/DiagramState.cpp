#include "DiagramState.h"

bool DiagramStore::push(const Diagram& diagram) {
    if (diagrams_.size() >= MAX_DIAGRAMS || !diagram.fitsNdc()) {
        return false;
    }
    diagrams_.push_back(diagram);
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
