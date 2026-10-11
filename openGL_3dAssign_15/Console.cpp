#include "Console.h"
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {

constexpr float HALF_SIZE = 0.4f;
constexpr float ROTATION_SPEED = 45.0f;
const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };



// 부호 조합은 육면체 8개 꼭짓점을 구분한다. 사각뿔은 바닥 4개와 꼭짓점 색이 다르다.
void assignVertexColors(graphics::Polyhedron& shape) {
    const std::array<graphics::GlColor, 8> palette{
        graphics::GlColor{ 1.0f, 0.2f, 0.2f }, { 0.2f, 1.0f, 0.2f },
        { 0.2f, 0.4f, 1.0f }, { 1.0f, 1.0f, 0.2f },
        { 1.0f, 0.2f, 1.0f }, { 0.2f, 1.0f, 1.0f },
        { 1.0f, 0.6f, 0.2f }, { 0.7f, 0.3f, 1.0f }
    };
    for (auto& face : shape.faces) {
        for (const auto& vertex : face.localVertices) {
            const int index = (vertex.x > 0.0f ? 1 : 0)
                | (vertex.y > 0.0f ? 2 : 0) | (vertex.z > 0.0f ? 4 : 0);
            face.vertexColors.push_back(palette[index]);
        }
    }
}

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
    assignVertexColors(cube);
    assignVertexColors(pyramid);
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
    camera.position = { 1.8f, 1.2f, 3.0f };
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetAll();
    std::cout << "Practice 15 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
              << "Cube only 모드: T == top Z rotation, F == front open/close, S == right-face normal rotation\n"
              << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
              << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
              << "Only one face motion 활성가능. Object 호출.\n"
              << "T/S/4/5 repeat to pause/resume. F/B repeat to reverse.\n";
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
    motion_ = FaceMotion{};
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
    transform.rotationDegrees = { 30.0f, 30.0f, 0.0f };
    // 초기 기울기 다음에 로컬 Y축 자전을 적용한다.
    transform.orientation = glm::angleAxis(glm::radians(spinDegrees_), Y_AXIS);
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void Console::beginMotion(Motion kind) {
    if (pyramidSelected_) {
        return; // 사각뿔에서는 육면체 면 명령을 무시한다.
    }
    if (motion_.kind == kind) {
        if (kind == Motion::Front || kind == Motion::Back) {
            motion_.direction = -motion_.direction;
            motion_.running = true;
            return;
        }
        if (kind != Motion::Hinge && kind != Motion::Swap) {
            motion_.running = !motion_.running;
            return;
        }
    }
    // 새 동작은 이전 동작의 면 행렬·숨김을 모두 초기화한다.
    resetFaces();
    motion_ = FaceMotion{};
    motion_.kind = kind;
    motion_.running = true;
    if (kind == Motion::Hinge || kind == Motion::Swap) {
        const std::array<std::array<std::size_t, 2>, 4> pairs{
            std::array<std::size_t, 2>{ 0, 3 }, { 3, 1 }, { 1, 2 }, { 2, 0 }
        };
        const std::array<glm::vec3, 4> corners{
            glm::vec3{ HALF_SIZE, 0.0f, HALF_SIZE }, { HALF_SIZE, 0.0f, -HALF_SIZE },
            { -HALF_SIZE, 0.0f, -HALF_SIZE }, { -HALF_SIZE, 0.0f, HALF_SIZE }
        };
        const int index = std::uniform_int_distribution<int>(0, 3)(random_);
        motion_.pair = pairs[index];
        motion_.hinge = corners[index];
        std::cout << "Selected cube faces (1-based): " << motion_.pair[0] + 1
                  << ", " << motion_.pair[1] + 1 << '\n';
    }
    applyFaceMotion();
}

void Console::applyFaceMotion() {
    auto& scene = application_.getScene();
    const auto setMatrix = [&](std::size_t face, const glm::mat4& matrix) {
        scene.setFaceLocalMatrix(cuboidId_, face, matrix);
    };
    const float progress = motion_.progress;
    switch (motion_.kind) {
    case Motion::Top:
        setMatrix(4, rotateAround({ 0.0f, HALF_SIZE, 0.0f }, Z_AXIS, progress));
        break;
    case Motion::Front:
        // 음의 X 회전으로 앞면이 바깥쪽으로 열린다. 위쪽 모서리는 고정된다.
        setMatrix(0, rotateAround({ 0.0f, HALF_SIZE, HALF_SIZE }, X_AXIS, -90.0f * progress));
        break;
    case Motion::Side:
        setMatrix(3, rotateAround({ HALF_SIZE, 0.0f, 0.0f }, X_AXIS, progress));
        break;
    case Motion::Back:
        setMatrix(1, scaleAround({ 0.0f, 0.0f, -HALF_SIZE }, 1.0f - progress));
        scene.setFaceVisible(cuboidId_, 1, progress < 1.0f);
        break;
    case Motion::Hinge: {
        const auto matrix = progress == 360.0f ? glm::mat4(1.0f)
            : rotateAround(motion_.hinge, Y_AXIS, progress);
        setMatrix(motion_.pair[0], matrix);
        setMatrix(motion_.pair[1], matrix);
        break;
    }
    case Motion::Swap:
        // 순서는 앞→오른쪽→뒤→왼쪽이다. +90/-90이면 두 중심과 면이 서로 교체된다.
        setMatrix(motion_.pair[0], rotateAround(glm::vec3(0.0f), Y_AXIS, progress));
        setMatrix(motion_.pair[1], rotateAround(glm::vec3(0.0f), Y_AXIS, -progress));
        break;
    case Motion::OrbitY:
        for (std::size_t face : { 0u, 1u, 2u, 3u }) {
            setMatrix(face, rotateAround(glm::vec3(0.0f), Y_AXIS, progress));
        }
        break;
    case Motion::OrbitX:
        for (std::size_t face : { 4u, 5u, 0u, 1u }) {
            setMatrix(face, rotateAround(glm::vec3(0.0f), X_AXIS, progress));
        }
        break;
    default: break;
    }
}

void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }
    switch (key) {
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE: application_.requestClose(); break;
    case GLFW_KEY_H: {
        auto& options = application_.getRenderOptions();
        options.depthTest = !options.depthTest;
        break;
    }
    case GLFW_KEY_P: selectShape(!pyramidSelected_); break;
    case GLFW_KEY_Y: spinning_ = !spinning_; break;
    case GLFW_KEY_R: resetAll(); break;
    case GLFW_KEY_T: beginMotion(Motion::Top); break;
    case GLFW_KEY_F: beginMotion(Motion::Front); break;
    case GLFW_KEY_S: beginMotion(Motion::Side); break;
    case GLFW_KEY_B: beginMotion(Motion::Back); break;
    case GLFW_KEY_1: beginMotion(Motion::Hinge); break;
    case GLFW_KEY_2: beginMotion(Motion::Swap); break;
    case GLFW_KEY_4: beginMotion(Motion::OrbitY); break;
    case GLFW_KEY_5: beginMotion(Motion::OrbitX); break;
    default: break;
    }
}

void Console::onUpdate(float deltaTime) {
    if (spinning_) {
        spinDegrees_ = std::fmod(spinDegrees_ + ROTATION_SPEED * deltaTime, 360.0f);
        applyObjectTransform();
    }
    if (!motion_.running || pyramidSelected_) {
        return;
    }
    if (motion_.kind == Motion::Front || motion_.kind == Motion::Back) {
        motion_.progress = std::clamp(motion_.progress + motion_.direction * deltaTime, 0.0f, 1.0f);
        if (motion_.progress == 0.0f || motion_.progress == 1.0f) {
            motion_.running = false;
        }
    } else if (motion_.kind == Motion::Hinge || motion_.kind == Motion::Swap) {
        const float end = motion_.kind == Motion::Hinge ? 360.0f : 90.0f;
        motion_.progress = std::min(end, motion_.progress + ROTATION_SPEED * deltaTime);
        if (motion_.progress == end) {
            motion_.running = false;
        }
    } else {
        motion_.progress = std::fmod(motion_.progress + ROTATION_SPEED * deltaTime, 360.0f);
    }
    applyFaceMotion();
}
