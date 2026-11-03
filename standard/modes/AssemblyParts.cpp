#include "AssemblyParts.h"
#include "PracticeCommon.h"

namespace assembly {
TankParts addTankParts(graphics::Scene& scene) {
    TankParts rig;
    const auto entity = [](std::optional<graphics::EntityId> id) {
        return practice_common::requireEntity(id, "Tank part creation failed.");
    };
    const auto box = [&](glm::vec3 size, glm::vec3 position, graphics::EntityId parent) {
        const auto id = entity(graphics::PatternSystem::addBox(scene, size, position, {1, 1, 1}, parent));
        rig.colored.push_back(id);
        return id;
    };
    rig.root = entity(scene.addGroup());
    rig.central = entity(graphics::PatternSystem::addJoint(scene, rig.root, {0, .6f, 0}));
    for (std::size_t i = 0; i < 2; ++i)
        rig.lower[i] = box({.65f, .6f, 1.8f}, {i == 0 ? -.8f : .8f, .3f, 0}, rig.root);
    box({2.3f, .25f, 1.4f}, {0, .125f, 0}, rig.central);
    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        rig.upper[i] = entity(graphics::PatternSystem::addJoint(scene, rig.central, {sign * .65f, .25f, 0}));
        rig.guns[i] = entity(graphics::PatternSystem::addJoint(scene, rig.upper[i], {0, .3f, 0}));
        rig.poles[i] = entity(graphics::PatternSystem::addJoint(scene, rig.upper[i], {0, .6f, 0}));
        box({.65f, .6f, .8f}, {0, .3f, 0}, rig.upper[i]);
        box({.14f, .14f, 1.1f}, {0, -.05f, .95f}, rig.guns[i]);
        box({.08f, .85f, .08f}, {0, .425f, 0}, rig.poles[i]);
    }
    return rig;
}

std::vector<graphics::EntityId> addGridParts(graphics::Scene& scene, const graphics::GridLayout& grid) {
    std::vector<graphics::EntityId> cells;
    for (int z = 0; z < grid.cells.z; ++z)
        for (int x = 0; x < grid.cells.x; ++x) {
            const auto center = grid.cellCenter({x, 0, z});
            const auto size = grid.relativeSize({.15f, 1, .22f});
            practice_common::require(center && size, "Invalid grid cell.");
            cells.push_back(practice_common::requireEntity(graphics::PatternSystem::addBox(scene, *size, *center,
                (x + z) % 2 ? graphics::GlColor{.25f, .3f, .4f} : graphics::GlColor{.4f, .45f, .55f}),
                "Grid cell creation failed."));
        }
    return cells;
}
} // namespace assembly
