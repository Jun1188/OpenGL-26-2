#include "FaceCommon.h"

std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app) {
    return std::make_unique<FaceMode>(app);
}
