#pragma once

#include "Common.h"

#include <cstdint>


std::vector<Point> setRandomTriPointByCase(const TriangleCase& case_);

class DiagramStore {
public:
    void resetTable();
    DiagramStore(DiagCaseTable dct = {makeDCT()}) : dct{dct} {}

    Diagram makeDiagram(GLShape shape, TriangleCase case_);
    bool push(const Diagram& diagram);

    Shape getFitTableMember() const;
    Shape getUnfitTableMember() const;
    bool overlap(const Diagram&) const;//작성해야 함, 존재하는 도형과 겹치는지 판별- 겹치면 true, or not false -> insideTriangle, nearline, cross을 이용하면 될듯
    size_t getSafeLenghth() const;// 작성해야 함, 도형과 겹치지 않을 정도의 적당한 길이를 랜덤으로 반환
    
    int findClosestSameShape_IndexAtCloseAndFit(const Diagram& diagram) const;//Drag를 마치는 함수에서 호출된다, Bounds가 Fit이고 일정 거리 이하에서 가장 가까운, 같은 유형의 도형의 인덱스를 반환한다, 없으면 -1을 반환한다.
    void mergeSelectedDiagram(); //find로 fit 도형을 찾으면 현재 드래그 중인 unfit 도형을 삭제하고, 동일한 fit 도형의 모드를 line loop에서 삼각형으로 바꾼다.
    bool resetAtRandom();//작성해야 함
    bool replaceAt(std::size_t index, const Diagram& diagram);
    void clear();
    void selectAt(Point point);//unfit
    bool translateSelected(Point direction);
    bool translateAll(Point direction);
    bool setSelectedScale(Point scale);
    bool setSelectedRotation(float degrees);
    bool setSelectedDirection(Point direction, float speed);
    bool advance(float deltaTime);

    

    const std::vector<Diagram>& getItems() const { return diagrams_; }
    std::optional<std::size_t> getSelectedIndex() const { return selectedIndex_; }
    std::uint64_t getRevision() const { return revision_; }

private:
    std::vector<Diagram> diagrams_; //diagram 자체는 전역, 
    DiagCaseTable dct{};// max num table of all daigram case
    DiagCaseTable unfitRest{}, fitRest{};
    std::optional<std::size_t> selectedIndex_;
    std::uint64_t revision_ = 0;
};
