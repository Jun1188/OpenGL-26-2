#include "Paths.h"

graphics::PathMotion standard_modes::path3() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polygon(
        {{1, -.5f, 0}, {2.4f, -.4f, 0}, {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
    return path;
}
