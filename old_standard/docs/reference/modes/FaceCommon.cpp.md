# 과거 구현 참조: FaceCommon.cpp

- 원래 위치: `standard/modes/FaceCommon.cpp`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#include "FaceCommon.h"

using namespace face_common;
using practice_common::assignVertexColors;
using practice_common::rotateAround;
using practice_common::scaleAround;


/// 장면 객체와 초기 상태를 구성한다.

bool FaceMode::initScene() {
    auto& scene = application_.getScene();

    scene.clear();

    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(HALF_SIZE * 2.0f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(HALF_SIZE * 2.0f));

    assignVertexColors(cube);
    assignVertexColors(pyramid);

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
    camera.orthographicHalfExtent = 1.0f;
    resetAll();
    std::cout
        << "Practice 15 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
        << "Cube only 모드: T == top Z rotation, F == front open/close, S == left/right face normal rotation\n"
        << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
        << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
        << "All face motions can overlap with object spin(중복 모션 가능).\n"
        << "T/S/1/2/4/5 repeat to pause/resume(한번 더 누르면 정지). F/B repeat to reverse. 2 swaps and returns.\n";

    return true;
}

void FaceMode::resetFaces() {
    auto& scene = application_.getScene();

    for (auto id : scene.getIds()) {
        const bool visible = id == (pyramidSelected_ ? pyramidId_ : cuboidId_);

        for (std::size_t i = 0; i < scene.getComponent(id)->polyhedron.faces.size(); ++i) {
            scene.setFaceLocalMatrix(id, i, glm::mat4(1.0f));
            scene.setFaceVisible(id, i, visible);
        }
    }
}

void FaceMode::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;
    motions_ = {};
    resetFaces();
    applyObjectTransform();
}

void FaceMode::resetAll() {
    spinning_ = false;
    spinDegrees_ = 0.0f;
    application_.getRenderOptions() = graphics::RenderOptions{};
    selectShape(false);
}

void FaceMode::applyObjectTransform() {
    graphics::Transform3D transform;

    transform.rotation = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(spinDegrees_), Y_AXIS));
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void FaceMode::beginMotion(Motion kind) {
    if (pyramidSelected_) {
        return;
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
                std::array<std::size_t, 2>{0, 3}, {3, 1}, {1, 2}, {2, 0}};
            const std::array<glm::vec3, 4> corners{glm::vec3{HALF_SIZE, 0.0f, HALF_SIZE},
                                                   {HALF_SIZE, 0.0f, -HALF_SIZE},
                                                   {-HALF_SIZE, 0.0f, -HALF_SIZE},
                                                   {-HALF_SIZE, 0.0f, HALF_SIZE}};
            const int index = std::uniform_int_distribution<int>(0, 3)(random_);

            motion.pair = pairs[index];
            motion.hinge = corners[index];
            std::cout << "선택면들 -Selected cube faces (1-based 면): " << motion.pair[0] + 1 << ", "
                      << motion.pair[1] + 1 << '\n';
        }
    }
    applyFaceMotion();
}

void FaceMode::applyFaceMotion() {
    auto& scene = application_.getScene();
    std::array<glm::mat4, 6> matrices;

    matrices.fill(glm::mat4(1.0f));

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
            compose(4, rotateAround({0.0f, HALF_SIZE, 0.0f}, Z_AXIS, progress));
            break;
        case Motion::Front:
            compose(0, rotateAround({0.0f, HALF_SIZE, HALF_SIZE}, X_AXIS, -90.0f * progress));
            break;
        case Motion::Side:

            compose(2, rotateAround({-HALF_SIZE, 0.0f, 0.0f}, X_AXIS, progress));
            compose(3, rotateAround({HALF_SIZE, 0.0f, 0.0f}, X_AXIS, progress));
            break;
        case Motion::Back:
            compose(1, scaleAround({0.0f, 0.0f, -HALF_SIZE}, 1.0f - progress));
            break;
        case Motion::Hinge:
            for (auto face : motion.pair) {
                compose(face, rotateAround(motion.hinge, Y_AXIS, progress));
            }
            break;
        case Motion::Swap: {

            const float angle = progress <= 90.0f ? progress : 180.0f - progress;

            compose(motion.pair[0], rotateAround(glm::vec3(0.0f), Y_AXIS, angle));
            compose(motion.pair[1], rotateAround(glm::vec3(0.0f), Y_AXIS, -angle));
            break;
        }
        case Motion::OrbitY:
            for (std::size_t face : {0u, 1u, 2u, 3u}) {
                compose(face, rotateAround(glm::vec3(0.0f), Y_AXIS, progress));
            }
            break;
        case Motion::OrbitX:
            for (std::size_t face : {4u, 5u, 0u, 1u}) {
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

void FaceMode::onKey(int key, int action, int) {
    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        application_.requestClose();
        break;
    case GLFW_KEY_H: {
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
    default:
        break;
    }
}

void FaceMode::onUpdate(float deltaTime) {
    if (spinning_) {
        spinDegrees_ = std::fmod(spinDegrees_ + ROTATION_SPEED * deltaTime, 360.0f);
        applyObjectTransform();
    }

    if (pyramidSelected_) {
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
                motion.running = false;
            }
        } else {
            const float period = kind == Motion::Swap ? 180.0f : 360.0f;

            motion.progress = std::fmod(motion.progress + ROTATION_SPEED * deltaTime, period);
        }
    }
    applyFaceMotion();
}
````
