#include "FaceCommon.h"
#include <iostream>

using namespace face_common;
using practice_common::rotateAround;

namespace {
class Practice16 final : public FaceMode {
    enum class PyramidMotion { None, All, Sequence, Swap, Apex };
    PyramidMotion pyramidMotion_ = PyramidMotion::None;
    bool pyramidRunning_ = false;
    float pyramidProgress_ = 0, pyramidDirection_ = 1;
    void beginPyramid(PyramidMotion motion);
    void updatePyramid(float dt);
    void applyPyramid();
    void selectShape(bool pyramid) override {
        FaceMode::selectShape(pyramid);
        pyramidMotion_ = PyramidMotion::None;
        pyramidRunning_ = false;
        pyramidProgress_ = 0;
        pyramidDirection_ = 1;
    }
public:
    explicit Practice16(graphics::Application& app) : FaceMode(app) {}
    bool initScene() override;
    void onKey(int key, int action, int mods) override {
        if (action == GLFW_PRESS && key >= GLFW_KEY_6 && key <= GLFW_KEY_9) {
            const PyramidMotion modes[] = {PyramidMotion::All, PyramidMotion::Sequence,
                                           PyramidMotion::Swap, PyramidMotion::Apex};
            beginPyramid(modes[key - GLFW_KEY_6]);
            return;
        }
        FaceMode::onKey(key, action, mods);
    }
    void onUpdate(float dt) override {
        if (!pyramidSelected_) {
            FaceMode::onUpdate(dt);
            return;
        }
        if (spinning_) {
            spinDegrees_ = std::fmod(spinDegrees_ + ROTATION_SPEED * dt, 360.0f);
            applyObjectTransform();
        }
        updatePyramid(dt);
    }
};

bool Practice16::initScene() {
    auto& scene = application_.getScene();

    scene.clear();

    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(HALF_SIZE * 2.0f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(HALF_SIZE * 2.0f));
    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);

    if (!cubeId || !pyramidId) {
        return false;
    }

    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;

    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();
    const auto inverseView = glm::transpose(glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0})));

    camera.position = inverseView * glm::vec3(0, 0, 4);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.5f;
    resetAll();
    std::cout
        << "Practice 16 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
        << "Cube only 모드: T == top Z rotation, F == front open/close, S == left/right face normal rotation\n"
        << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
        << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
        << "All face motions can overlap with object spin(중복 모션 가능).\n"
        << "T/S/1/2/4/5 repeat to pause/resume(한번 더 누르면 정지). F/B repeat to reverse. 2 swaps and returns.\n";
    std::cout << "Pyramid 6: meet below, 7: sequential unfolding, 8: opposite swap, 9: apex opening\n";

    return true;
}


void Practice16::beginPyramid(PyramidMotion motion) {
    if (!pyramidSelected_)
        selectShape(true);

    if (pyramidMotion_ != motion) {
        pyramidMotion_ = motion;
        pyramidProgress_ = 0;
        pyramidDirection_ = 1;
        pyramidRunning_ = true;

    } else if (motion == PyramidMotion::All || motion == PyramidMotion::Apex) {
        pyramidDirection_ = -pyramidDirection_;
        pyramidRunning_ = true;
    } else if (motion == PyramidMotion::Swap && !pyramidRunning_ && (pyramidProgress_ == 0 || pyramidProgress_ == 1)) {
        pyramidDirection_ = -pyramidDirection_;
        pyramidRunning_ = true;
    } else
        pyramidRunning_ = !pyramidRunning_;

    applyPyramid();
}


void Practice16::updatePyramid(float dt) {
    if (pyramidRunning_) {
        if (pyramidMotion_ == PyramidMotion::Sequence)
            pyramidProgress_ = std::fmod(pyramidProgress_ + dt, 8.0f);
        else {
            pyramidProgress_ = std::clamp(pyramidProgress_ + pyramidDirection_ * dt * .5f, 0.0f, 1.0f);

            if (pyramidProgress_ == 0 || pyramidProgress_ == 1)
                pyramidRunning_ = false;
        }
    }
    applyPyramid();
}


void Practice16::applyPyramid() {
    auto& scene = application_.getScene();
    const float b = HALF_SIZE, h = 2 * HALF_SIZE;

    const float inward = glm::degrees(std::atan2(h, b));

    const std::array<glm::vec3, 4> hinges{glm::vec3{0, -b, b}, {b, -b, 0}, {0, -b, -b}, {-b, -b, 0}};
    const std::array<glm::vec3, 4> axes{X_AXIS, -Z_AXIS, -X_AXIS, Z_AXIS};

    const auto active = std::size_t(pyramidProgress_ / 2.0f) % 4;
    const float phase = std::fmod(pyramidProgress_, 2.0f);

    for (std::size_t i = 0; i < 4; ++i) {
        glm::mat4 matrix(1);

        if (pyramidMotion_ == PyramidMotion::All)
            matrix = rotateAround(hinges[i], axes[i], (360.0f - 2 * inward) * pyramidProgress_);

        else if (pyramidMotion_ == PyramidMotion::Sequence && i == active)
            matrix = rotateAround(hinges[i], axes[i], (180 - inward) * (phase <= 1 ? phase : 2 - phase));

        else if (pyramidMotion_ == PyramidMotion::Swap && (i == 0 || i == 2))
            matrix = rotateAround({0, b, 0}, Y_AXIS, (i == 0 ? 180.0f : -180.0f) * pyramidProgress_);

        else if (pyramidMotion_ == PyramidMotion::Apex)
            matrix = rotateAround({0, b, 0}, axes[i], -45.0f * pyramidProgress_);

        scene.setFaceLocalMatrix(pyramidId_, i, matrix);
    }
    scene.setFaceLocalMatrix(pyramidId_, 4, glm::mat4(1));
}

}

std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app) {
    return std::make_unique<Practice16>(app);
}
