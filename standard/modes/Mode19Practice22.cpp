#include "TankCommon.h"

std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app) {
    return makeTankMode(app, true);
}
