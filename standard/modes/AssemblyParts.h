#pragma once
#include "PracticeMode.h"

namespace assembly {
struct TankParts {
    graphics::EntityId root = 0, central = 0;
    std::array<graphics::EntityId, 2> lower{}, upper{}, guns{}, poles{};
    std::vector<graphics::EntityId> colored;
};
TankParts addTankParts(graphics::Scene& scene);
std::vector<graphics::EntityId> addGridParts(graphics::Scene& scene, const graphics::GridLayout& grid);
} // namespace assembly
