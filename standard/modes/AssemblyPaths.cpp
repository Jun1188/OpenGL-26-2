#include "Paths.h"
#include "PracticeCommon.h"


graphics::PathMotion standard_modes::path1() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
    return path;
}

graphics::PathMotion standard_modes::path2() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
    return path;
}

graphics::PathMotion standard_modes::path3() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polygon(
        {{1, -.5f, 0}, {2.4f, -.4f, 0}, {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
    return path;
}

graphics::PathMotion standard_modes::path4() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = [center](double u) {
        const double angle = u * 6.283185307179586;
        return center + glm::vec3(float(.7 * std::sin(2 * angle)), float(.6 * std::sin(angle)),
                                  float(.3 * std::cos(angle)));
    };
    path.interpolation = graphics::PathSystem::smoothStep;
    return path;
}

graphics::PathMotion standard_modes::path5() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    return path;
}

graphics::PathMotion standard_modes::path6() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
    path.playback = graphics::Playback::PingPong;
    return path;
}
