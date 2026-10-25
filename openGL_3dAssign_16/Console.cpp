#include "Console.h"
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {

constexpr float HALF_SIZE = 0.4f;
constexpr float ROTATION_SPEED = 45.0f;
const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };



glm::mat4 rotateAround(const glm::vec3& pivot, const glm::vec3& axis, float degrees) {
    return glm::translate(glm::mat4(1.0f), pivot)
        * glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axis)
        * glm::translate(glm::mat4(1.0f), -pivot);
}

glm::mat4 scaleAround(const glm::vec3& pivot, float scale) {
    return glm::translate(glm::mat4(1.0f), pivot)
        * glm::scale(glm::mat4(1.0f), glm::vec3(scale))
        * glm::translate(glm::mat4(1.0f), -pivot);
}

} // namespace

bool Console::initScene() {
    auto& scene = application_.getScene();
    scene.clear();
    auto cube = graphics::DiagramSystem::makePolyhedron(
        graphics::GLShape::Cube, glm::vec3(HALF_SIZE * 2.0f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(
        graphics::GLShape::Pyramid, glm::vec3(HALF_SIZE * 2.0f));
    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);
    if (!cubeId || !pyramidId) {
        return false;
    }

    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;
    if (!scene.addLine({ -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.2f, 0.2f })
        || !scene.addLine({ 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.2f, 1.0f, 0.2f })
        || !scene.addLine({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.2f, 0.4f, 1.0f })) {
        return false;
    }
    auto& camera = application_.getCamera();
    const auto inverseView = glm::transpose(glm::mat3(glm::rotate(
        glm::rotate(glm::mat4(1), glm::radians(30.0f), {1,0,0}),
        glm::radians(30.0f), {0,1,0})));
    camera.position = inverseView * glm::vec3(0,0,4);
    camera.up = inverseView * glm::vec3(0,1,0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.5f;
    resetAll();
    std::cout << "Practice 16 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
              << "Cube only 모드: T == top Z rotation, F == front open/close, S == left/right face normal rotation\n"
              << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
              << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
              << "All face motions can overlap with object spin(중복 모션 가능).\n"
              << "T/S/1/2/4/5 repeat to pause/resume(한번 더 누르면 정지). F/B repeat to reverse. 2 swaps and returns.\n";
    std::cout << "Pyramid 6: meet below, 7: sequential unfolding, 8: opposite swap, 9: apex opening\n";
    return true;
}

void Console::resetFaces() {
    auto& scene = application_.getScene();
    for (auto id : scene.getIds()) {
        const bool visible = id == (pyramidSelected_ ? pyramidId_ : cuboidId_);
        for (std::size_t i = 0; i < scene.getComponent(id)->polyhedron.faces.size(); ++i) {
            scene.setFaceLocalMatrix(id, i, glm::mat4(1.0f));
            scene.setFaceVisible(id, i, visible);
        }
    }
}

void Console::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;
    motions_ = {};
    pyramidMotion_=PyramidMotion::None;
    pyramidRunning_=false;
    pyramidProgress_=0;
    pyramidDirection_=1;
    resetFaces();
    applyObjectTransform();
}

void Console::resetAll() {
    spinning_ = false;
    spinDegrees_ = 0.0f;
    application_.getRenderOptions() = graphics::RenderOptions{};
    selectShape(false);
}

void Console::applyObjectTransform() {
    graphics::Transform3D transform;
    transform.rotation = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(spinDegrees_), Y_AXIS));
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void Console::beginMotion(Motion kind) {
    if (pyramidSelected_) {
        return; // 사각뿔에서는 육면체 면 명령을 무시한다.
    }
    auto& motion = motions_[static_cast<std::size_t>(kind)];
    if (motion.initialized) {
        if (kind == Motion::Front || kind == Motion::Back) {
            motion.direction = -motion.direction;
            motion.running = true;
        } else {
            motion.running = !motion.running;
        }
    } else {
        motion.initialized = true;
        motion.running = true;
        if (kind == Motion::Hinge || kind == Motion::Swap) {
            const std::array<std::array<std::size_t, 2>, 4> pairs{
                std::array<std::size_t, 2>{ 0, 3 }, { 3, 1 }, { 1, 2 }, { 2, 0 }
            };
            const std::array<glm::vec3, 4> corners{
                glm::vec3{ HALF_SIZE, 0.0f, HALF_SIZE }, { HALF_SIZE, 0.0f, -HALF_SIZE },
                { -HALF_SIZE, 0.0f, -HALF_SIZE }, { -HALF_SIZE, 0.0f, HALF_SIZE }
            };
            const int index = std::uniform_int_distribution<int>(0, 3)(random_);
            motion.pair = pairs[index];
            motion.hinge = corners[index];
            std::cout << "선택면들 : " << motion.pair[0] + 1
                      << ", " << motion.pair[1] + 1 << '\n';
        }
    }
    applyFaceMotion();
}

void Console::applyFaceMotion() {
    auto& scene = application_.getScene();
    std::array<glm::mat4, 6> matrices;
    matrices.fill(glm::mat4(1.0f));
    // 열벡터 기준으로 다음 모션을 왼쪽에 곱한다. 매 프레임 기준 형상에서 계산한다.
    // 적용 순서: T -> F -> S -> B -> 1 -> 2 -> 4 -> 5 -> 전체 객체 변환.
    const auto compose = [&](std::size_t face, const glm::mat4& matrix) {
        matrices[face] = matrix * matrices[face];
    };
    for (std::size_t index = 1; index < motions_.size(); ++index) {
        const auto kind = static_cast<Motion>(index);
        const auto& motion = motions_[index];
        if (!motion.initialized) {
            continue;
        }
        const float progress = motion.progress;
        switch (kind) {
        case Motion::Top:
            compose(4, rotateAround({ 0.0f, HALF_SIZE, 0.0f }, Z_AXIS, progress));
            break;
        case Motion::Front:
            compose(0, rotateAround({ 0.0f, HALF_SIZE, HALF_SIZE }, X_AXIS, -90.0f * progress));
            break;
        case Motion::Side:
            // 마주 보는 왼쪽과 오른쪽 면을 각각 자기 중점의 X축으로 회전한다.
            compose(2, rotateAround({ -HALF_SIZE, 0.0f, 0.0f }, X_AXIS, progress));
            compose(3, rotateAround({ HALF_SIZE, 0.0f, 0.0f }, X_AXIS, progress));
            break;
        case Motion::Back:
            compose(1, scaleAround({ 0.0f, 0.0f, -HALF_SIZE }, 1.0f - progress));
            break;
        case Motion::Hinge:
            for (auto face : motion.pair) {
                compose(face, rotateAround(motion.hinge, Y_AXIS, progress));
            }
            break;
        case Motion::Swap:
            {
            // 0 -> 90 -> 0 왕복: 두 면이 교체된 뒤 복귀하며 같은 동작을 반복한다.
            const float angle = progress <= 90.0f ? progress : 180.0f - progress;
            compose(motion.pair[0], rotateAround(glm::vec3(0.0f), Y_AXIS, angle));
            compose(motion.pair[1], rotateAround(glm::vec3(0.0f), Y_AXIS, -angle));
            break;
        }
        case Motion::OrbitY:
            for (std::size_t face : { 0u, 1u, 2u, 3u }) {
                compose(face, rotateAround(glm::vec3(0.0f), Y_AXIS, progress));
            }
            break;
        case Motion::OrbitX:
            for (std::size_t face : { 4u, 5u, 0u, 1u }) {
                compose(face, rotateAround(glm::vec3(0.0f), X_AXIS, progress));
            }
            break;
        default:
            break;
        }
    }
    const auto& back = motions_[static_cast<std::size_t>(Motion::Back)];
    for (std::size_t face = 0; face < matrices.size(); ++face) {
        scene.setFaceLocalMatrix(cuboidId_, face, matrices[face]);
        scene.setFaceVisible(cuboidId_, face,
            !pyramidSelected_ && !(face == 1 && back.initialized && back.progress == 1.0f));
    }
}

void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }
    switch (key) {
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        application_.requestClose();
        break;
    case GLFW_KEY_H:
        {
        auto& options = application_.getRenderOptions();
        options.depthTest = !options.depthTest;
        break;
    }
    case GLFW_KEY_P:
        selectShape(!pyramidSelected_);
        break;
    case GLFW_KEY_Y:
        spinning_ = !spinning_;
        break;
    case GLFW_KEY_R:
        resetAll();
        break;
    case GLFW_KEY_T:
        beginMotion(Motion::Top);
        break;
    case GLFW_KEY_F:
        beginMotion(Motion::Front);
        break;
    case GLFW_KEY_S:
        beginMotion(Motion::Side);
        break;
    case GLFW_KEY_B:
        beginMotion(Motion::Back);
        break;
    case GLFW_KEY_1:
        beginMotion(Motion::Hinge);
        break;
    case GLFW_KEY_2:
        beginMotion(Motion::Swap);
        break;
    case GLFW_KEY_4:
        beginMotion(Motion::OrbitY);
        break;
    case GLFW_KEY_5:
        beginMotion(Motion::OrbitX);
        break;
    case GLFW_KEY_6:
        beginPyramid(PyramidMotion::All);
        break;
    case GLFW_KEY_7:
        beginPyramid(PyramidMotion::Sequence);
        break;
    case GLFW_KEY_8:
        beginPyramid(PyramidMotion::Swap);
        break;
    case GLFW_KEY_9:
        beginPyramid(PyramidMotion::Apex);
        break;
    default:
        break;
    }
}

void Console::onUpdate(float deltaTime) {
    if (spinning_) {
        spinDegrees_ = std::fmod(spinDegrees_ + ROTATION_SPEED * deltaTime, 360.0f);
        applyObjectTransform();
    }
    if (pyramidSelected_) {
        updatePyramid(deltaTime);
        return;
    }
    for (std::size_t index = 1; index < motions_.size(); ++index) {
        auto& motion = motions_[index];
        if (!motion.running) {
            continue;
        }
        const auto kind = static_cast<Motion>(index);
        if (kind == Motion::Front || kind == Motion::Back) {
            motion.progress = std::clamp(motion.progress + motion.direction * deltaTime, 0.0f, 1.0f);
            if (motion.progress == 0.0f || motion.progress == 1.0f) {
                motion.running = false; // 이 모션만 완료한다. 다른 모션은 계속 실행한다.
            }
        } else {
            const float period = kind == Motion::Swap ? 180.0f : 360.0f;
            motion.progress = std::fmod(motion.progress + ROTATION_SPEED * deltaTime, period);
        }
    }
    applyFaceMotion();
}

void Console::beginPyramid(PyramidMotion motion) {
    if(!pyramidSelected_)selectShape(true);
    if(pyramidMotion_!=motion) {
        pyramidMotion_=motion;
        pyramidProgress_=0;
        pyramidDirection_=1;
        pyramidRunning_=true;

    } else if(motion==PyramidMotion::All||motion==PyramidMotion::Apex) {
        pyramidDirection_=-pyramidDirection_;
        pyramidRunning_=true;
    } else if(motion==PyramidMotion::Swap && !pyramidRunning_
              && (pyramidProgress_==0 || pyramidProgress_==1)) {
        pyramidDirection_=-pyramidDirection_;
        pyramidRunning_=true;
    } else pyramidRunning_=!pyramidRunning_;

    applyPyramid();
}
void Console::updatePyramid(float dt) {
    if(pyramidRunning_) {
        if(pyramidMotion_==PyramidMotion::Sequence)pyramidProgress_=std::fmod(pyramidProgress_+dt,8.0f);
        else {
            pyramidProgress_=std::clamp(pyramidProgress_+pyramidDirection_*dt*.5f,0.0f,1.0f);
            if(pyramidProgress_==0||pyramidProgress_==1)pyramidRunning_=false;
        }
    }
    applyPyramid();
}
void Console::applyPyramid() {
    auto& scene=application_.getScene();
    const float b=HALF_SIZE,h=2*HALF_SIZE;

    const float inward=glm::degrees(std::atan2(h,b));

    const std::array<glm::vec3,4> hinges{glm::vec3{0,-b,b},{b,-b,0},{0,-b,-b},{-b,-b,0}};
    const std::array<glm::vec3,4> axes{X_AXIS,-Z_AXIS,-X_AXIS,Z_AXIS};

    const auto active=std::size_t(pyramidProgress_/2.0f)%4;
    const float phase=std::fmod(pyramidProgress_,2.0f);

    for(std::size_t i=0;i<4;++i) {
        glm::mat4 matrix(1);
        if(pyramidMotion_==PyramidMotion::All)
            matrix=rotateAround(hinges[i],axes[i],(360.0f-2*inward)*pyramidProgress_);

        else if(pyramidMotion_==PyramidMotion::Sequence && i==active)
            matrix=rotateAround(hinges[i],axes[i],(180-inward)*(phase<=1?phase:2-phase));

        else if(pyramidMotion_==PyramidMotion::Swap && (i==0||i==2))
            matrix=rotateAround({0,b,0},Y_AXIS,(i==0?180.0f:-180.0f)*pyramidProgress_);

        else if(pyramidMotion_==PyramidMotion::Apex)
            matrix=rotateAround({0,b,0},axes[i],-45.0f*pyramidProgress_);

        scene.setFaceLocalMatrix(pyramidId_,i,matrix);
    }
    scene.setFaceLocalMatrix(pyramidId_,4,glm::mat4(1));
}
