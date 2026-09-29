#pragma once

#include "Common.h"

#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

class DiagramStore {
public:
    bool push(const Diagram& diagram);
    bool resetAtRandom();//작성해야 함
    bool replaceAt(std::size_t index, const Diagram& diagram);
    void clear();
    void selectAt(Point point);
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
    std::vector<Diagram> diagrams_;
    std::optional<std::size_t> selectedIndex_;
    std::uint64_t revision_ = 0;
};
