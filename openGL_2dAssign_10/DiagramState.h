#pragma once

#include "Common.h"

#include <cstdint>

// 길이를 입력하고 점은 계산한다. 한 쌍에서 한 번만 호출한다.
std::vector<Point> setRandomTriPointByCase(TriangleCase case_, float width, float height);

struct ShapeBoard {
    std::string name;
    std::size_t totalSlots = 0;
    std::size_t matchedSlots = 0;
    bool completed = false;
};

class DiagramStore {
public:
    bool resetAtRandom();
    void clear();
    void selectAt(Point point);
    void clearSelection();
    bool moveSelectedTo(Point pivot);
    bool mergeSelectedDiagram();
    int findClosestSameShape_IndexAtCloseAndFit(const Diagram& diagram) const;
    bool overlap(const Diagram& diagram) const;
    float getSafeLength(float maximum) const;

    bool translateSelected(Point direction);
    bool translateAll(Point direction);
    bool setSelectedScale(Point scale);
    bool setSelectedRotation(float degrees);
    bool setSelectedDirection(Point direction, float speed);
    bool advance(float deltaTime);
    void setWindowSize(int width, int height);

    const std::vector<Diagram>& getItems() const { return diagrams_; }
    const std::vector<ShapeBoard>& getBoards() const { return boards_; }
    const DiagCaseTable& getCaseTable() const { return dct_; }
    std::optional<std::size_t> getSelectedIndex() const { return selectedIndex_; }
    std::optional<Point> getSelectedPivot() const;
    std::uint64_t getRevision() const { return revision_; }
    std::size_t getCompletedBoardCount() const;

private:
    std::vector<Diagram> diagrams_;
    std::vector<ShapeBoard> boards_;
    DiagCaseTable dct_{};
    std::optional<std::size_t> selectedIndex_;
    std::uint64_t revision_ = 0;
    int windowWidth_ = WINDOW_WIDTH;
    int windowHeight_ = WINDOW_HEIGHT;
};