#include "Paths.h"

graphics::PathMotion standard_modes::path6() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
    path.playback = graphics::Playback::PingPong;
    return path;
}
