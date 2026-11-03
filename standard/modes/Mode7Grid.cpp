#include "../Console.h"

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

    for (int z = 0; z < 4; ++z)
        for (int x = 0; x < 6; ++x) {
            const auto center = grid.cellCenter({x, 0, z}), size = grid.relativeSize({.15f, 1, .22f});

            if (!center || !size ||
                !graphics::PatternSystem::addBox(scene, *size, *center,
                                                 (x + z) % 2 ? graphics::GlColor{.25f, .3f, .4f}
                                                             : graphics::GlColor{.4f, .45f, .55f}))
                return false;
        }

    if (!distancePath_.setPoints({{-2.5f, .35f, -1.5f}, {2.5f, .35f, -1.5f}, {2.5f, .35f, 1.5f}}))
        return false;

    for (unsigned i = 0; i < 5; ++i) {
        const auto box =
            graphics::PatternSystem::addBox(scene, {.4f, .5f, .4f}, {0, 0, 0}, {.25f + .14f * i, .7f - .1f * i, .3f});

        if (!box || !graphics::PatternSystem::addNumber(scene, i + 1, {-.07f, -.1f, .205f}, .2f, {1, 1, 1}, *box))
            return false;
        gridBoxes_.push_back(*box);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {7, 6, 8};
    app_.getCamera().target = {0, 0, 0};
    app_.getCamera().orthographicHalfExtent = 4;

    return true;
}

bool Console::updateGrid() {
    auto& scene = app_.getScene();
    bool ok = true;
        for (std::size_t i = 0; i < gridBoxes_.size(); ++i)
            ok = graphics::PatternSystem::follow(scene, gridBoxes_[i], distancePath_,
                                                 clock_.seconds + distancePath_.length() * i / 5) &&
                 ok;
    return ok;
}
