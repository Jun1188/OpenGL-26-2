#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app) {
    switch (mode) {
    case 10: return makePractice13(app);
    case 11: return makePractice14(app);
    case 12: return makePractice15(app);
    case 13: return makePractice16(app);
    case 14: return makePractice17(app);
    case 15: return makePractice18(app);
    case 16: return makePractice19(app);
    case 17: return makePractice20(app);
    case 18: return makePractice21(app);
    case 19: return makePractice22(app);
    case 20: return makePractice23(app);
    case 21: return makeCameraCollision(app);
    case 22: return makeEmbeddedMesh(app);
    default: return {};
    }
}
