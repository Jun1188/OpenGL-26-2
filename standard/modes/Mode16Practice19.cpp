#include "TankCommon.h"

std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app) {
    return makeTankMode(app, false);
}
