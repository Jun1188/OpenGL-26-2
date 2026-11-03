#include "../Console.h"
#include "AssemblyParts.h"

/// 거리 기반 격자 예제를 구성한다.
bool Console::initGrid() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Grid;
    clock_.reset();
    gridBoxes_.clear();

    graphics::GridLayout grid;

    grid.origin = {-3, -.1f, -2};
    grid.extent = {6, .2f, 4};
    grid.cells = {6, 1, 4};

    const auto cells = assembly::addGridParts(scene, grid);
    for (std::size_t i = 0; i < cells.size(); ++i)
        if (!addControl(cells[i], "grid tile " + std::to_string(i + 1)))
            return false;

    if (!distancePath_.setPoints({{-2.5f, .35f, -1.5f}, {2.5f, .35f, -1.5f}, {2.5f, .35f, 1.5f}}))
        return false;

    for (unsigned i = 0; i < 5; ++i) {
        const auto frame = scene.addGroup();
        if (!frame)
            return false;
        const auto box = graphics::PatternSystem::addBox(scene, {.4f, .5f, .4f}, {0, 0, 0},
            {.25f + .14f * i, .7f - .1f * i, .3f}, *frame);

        if (!box || !graphics::PatternSystem::addNumber(scene, i + 1, {-.07f, -.1f, .205f}, .2f, {1, 1, 1}, *box))
            return false;
        if (!addControl(*box, "moving box " + std::to_string(i + 1)))
            return false;
        gridBoxes_.push_back(*frame);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {7, 6, 8};
    app_.getCamera().target = {0, 0, 0};
    app_.getCamera().orthographicHalfExtent = 4;

    selectedJoint_ = 24; // 처음에는 첫 이동 상자를 조작한다.
    return true;
}

bool Console::updateGrid() {
    auto& scene = app_.getScene();
    bool ok = true;
    for (std::size_t i = 0; i < gridBoxes_.size(); ++i)
        ok = graphics::PatternSystem::follow(scene, gridBoxes_[i], distancePath_,
            clock_.seconds + distancePath_.length() * i / gridBoxes_.size()) && ok;
    return ok;
}
