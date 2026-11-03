#include "Paths.h"

graphics::PathMotion standard_modes::path5() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    return path;
}
