# 1단계: 주석 정리
실습 함수별 기능 설명을 남기고 본문 주석을 제거했다. 실행 코드는 변경하지 않았다. 빌드·테스트는 사용자에게 맡긴다.

## openGL_3dAssign_15/Console.cpp
### before
```cpp
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
    const auto inverseView = glm::transpose(glm::mat3(glm::rotate(
        glm::rotate(glm::mat4(1), glm::radians(30.0f), {1,0,0}),
        glm::radians(30.0f), {0,1,0})));
    camera.position = inverseView * glm::vec3(0,0,4);
    camera.up = inverseView * glm::vec3(0,1,0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetAll();
    std::cout << "Practice 15 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
              << "Cube only 모드: T == top Z rotation, F == front open/close, S == left/right face normal rotation\n"
              << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
              << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
              << "All face motions can overlap with object spin(중복 모션 가능).\n"
              << "T/S/1/2/4/5 repeat to pause/resume(한번 더 누르면 정지). F/B repeat to reverse. 2 swaps and returns.\n";
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
            std::cout << "선택면들 -Selected cube faces (1-based 면): " << motion.pair[0] + 1
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
        case Motion::Swap: {
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
        default: break;
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
                motion.running = false; // 이 모션만 완료한다. 다른 모션은 계속 실행한다.
            }
        } else {
            const float period = kind == Motion::Swap ? 180.0f : 360.0f;
            motion.progress = std::fmod(motion.progress + ROTATION_SPEED * deltaTime, period);
        }
    }
    applyFaceMotion();
}
```
### after
```cpp
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

/// 각 면의 정점 색을 지정한다.
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

/// 지정한 피벗 주위로 회전한다.
glm::mat4 rotateAround(const glm::vec3& pivot, const glm::vec3& axis, float degrees) {
    return glm::translate(glm::mat4(1.0f), pivot)
        * glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axis)
        * glm::translate(glm::mat4(1.0f), -pivot);
}

/// 지정한 피벗 주위로 크기를 변경한다.
glm::mat4 scaleAround(const glm::vec3& pivot, float scale) {
    return glm::translate(glm::mat4(1.0f), pivot)
        * glm::scale(glm::mat4(1.0f), glm::vec3(scale))
        * glm::translate(glm::mat4(1.0f), -pivot);
}

}

/// 장면 객체와 초기 상태를 구성한다.
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
    const auto inverseView = glm::transpose(glm::mat3(glm::rotate(
        glm::rotate(glm::mat4(1), glm::radians(30.0f), {1,0,0}),
        glm::radians(30.0f), {0,1,0})));
    camera.position = inverseView * glm::vec3(0,0,4);
    camera.up = inverseView * glm::vec3(0,1,0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetAll();
    std::cout << "Practice 15 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
              << "Cube only 모드: T == top Z rotation, F == front open/close, S == left/right face normal rotation\n"
              << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
              << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
              << "All face motions can overlap with object spin(중복 모션 가능).\n"
              << "T/S/1/2/4/5 repeat to pause/resume(한번 더 누르면 정지). F/B repeat to reverse. 2 swaps and returns.\n";
    return true;
}

/// 면별 애니메이션을 초기화한다.
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

/// 조작할 도형을 선택한다.
void Console::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;
    motions_ = {};
    resetFaces();
    applyObjectTransform();
}

/// 전체 조작 상태를 초기화한다.
void Console::resetAll() {
    spinning_ = false;
    spinDegrees_ = 0.0f;
    application_.getRenderOptions() = graphics::RenderOptions{};
    selectShape(false);
}

/// 물체 변환을 장면에 반영한다.
void Console::applyObjectTransform() {
    graphics::Transform3D transform;
    transform.rotation = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(spinDegrees_), Y_AXIS));
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

/// 선택한 면 애니메이션을 시작한다.
void Console::beginMotion(Motion kind) {
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
                std::array<std::size_t, 2>{ 0, 3 }, { 3, 1 }, { 1, 2 }, { 2, 0 }
            };
            const std::array<glm::vec3, 4> corners{
                glm::vec3{ HALF_SIZE, 0.0f, HALF_SIZE }, { HALF_SIZE, 0.0f, -HALF_SIZE },
                { -HALF_SIZE, 0.0f, -HALF_SIZE }, { -HALF_SIZE, 0.0f, HALF_SIZE }
            };
            const int index = std::uniform_int_distribution<int>(0, 3)(random_);
            motion.pair = pairs[index];
            motion.hinge = corners[index];
            std::cout << "선택면들 -Selected cube faces (1-based 면): " << motion.pair[0] + 1
                      << ", " << motion.pair[1] + 1 << '\n';
        }
    }
    applyFaceMotion();
}

/// 면별 변환을 장면에 반영한다.
void Console::applyFaceMotion() {
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
            compose(4, rotateAround({ 0.0f, HALF_SIZE, 0.0f }, Z_AXIS, progress));
            break;
        case Motion::Front:
            compose(0, rotateAround({ 0.0f, HALF_SIZE, HALF_SIZE }, X_AXIS, -90.0f * progress));
            break;
        case Motion::Side:

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
        case Motion::Swap: {

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
        default: break;
        }
    }
    const auto& back = motions_[static_cast<std::size_t>(Motion::Back)];
    for (std::size_t face = 0; face < matrices.size(); ++face) {
        scene.setFaceLocalMatrix(cuboidId_, face, matrices[face]);
        scene.setFaceVisible(cuboidId_, face,
            !pyramidSelected_ && !(face == 1 && back.initialized && back.progress == 1.0f));
    }
}

/// 키 입력에 따라 동작을 변경한다.
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

/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float deltaTime) {
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
```

## openGL_3dAssign_15/main.cpp
### before
```cpp
#include "Console.h"
#include <iostream>

int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 15";
    application.setInitHandler([&](graphics::Application&) {
        return practice.initScene();
    });
    application.setKeyHandler([&](graphics::Application&, int key, int action) {
        practice.onKey(key, action);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        practice.onUpdate(deltaTime);
    });
    return application.run(config);
}
```
### after
```cpp
#include "Console.h"
#include <iostream>

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 15";
    application.setInitHandler([&](graphics::Application&) {
        return practice.initScene();
    });
    application.setKeyHandler([&](graphics::Application&, int key, int action) {
        practice.onKey(key, action);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        practice.onUpdate(deltaTime);
    });
    return application.run(config);
}
```

## openGL_3dAssign_15/Console.h
### before
```cpp
#pragma once

#include "framework/include/Application.h"
#include <array>
#include <random>

// 실습 15: 각 면 모션의 상태를 독립적으로 관리하고 행렬을 합성한다.
class Console {
private:
    enum class Motion { None, Top, Front, Side, Back, Hinge, Swap, OrbitY, OrbitX, Count };
    struct FaceMotion {
        bool initialized = false;
        bool running = false;
        float progress = 0.0f; // 회전은 도, 열기·축소는 0~1이다.
        float direction = 1.0f;
        std::array<std::size_t, 2> pair{ 0, 3 };
        glm::vec3 hinge{ 0.4f, 0.0f, 0.4f };
    };

    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    bool spinning_ = false;
    float spinDegrees_ = 0.0f;
    std::array<FaceMotion, static_cast<std::size_t>(Motion::Count)> motions_{};
    std::mt19937 random_{ std::random_device{}() };

    void selectShape(bool pyramid);
    void resetFaces();
    void resetAll();
    void beginMotion(Motion kind);
    void applyObjectTransform();
    void applyFaceMotion();

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    void onKey(int key, int action);
    void onUpdate(float deltaTime);
};
```
### after
```cpp
#pragma once

#include "framework/include/Application.h"
#include <array>
#include <random>

class Console {
private:
    enum class Motion { None, Top, Front, Side, Back, Hinge, Swap, OrbitY, OrbitX, Count };
    struct FaceMotion {
        bool initialized = false;
        bool running = false;
        float progress = 0.0f;
        float direction = 1.0f;
        std::array<std::size_t, 2> pair{ 0, 3 };
        glm::vec3 hinge{ 0.4f, 0.0f, 0.4f };
    };

    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    bool spinning_ = false;
    float spinDegrees_ = 0.0f;
    std::array<FaceMotion, static_cast<std::size_t>(Motion::Count)> motions_{};
    std::mt19937 random_{ std::random_device{}() };

    /// 조작할 도형을 선택한다.
    void selectShape(bool pyramid);
    /// 면별 애니메이션을 초기화한다.
    void resetFaces();
    /// 전체 조작 상태를 초기화한다.
    void resetAll();
    /// 선택한 면 애니메이션을 시작한다.
    void beginMotion(Motion kind);
    /// 물체 변환을 장면에 반영한다.
    void applyObjectTransform();
    /// 면별 변환을 장면에 반영한다.
    void applyFaceMotion();

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& application) : application_(application) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float deltaTime);
};
```

## openGL_3dAssign_15/Graphics.h
### before
```cpp
#pragma once

// 이전 include 경로를 유지하는 호환 헤더.
#include "framework/include/Graphics.h"
```
### after
```cpp
#pragma once

#include "framework/include/Graphics.h"
```

## openGL_3dAssign_15/resource.h
### before
```cpp
//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by openGL_assign_789.rc

// 다음은 새 개체에 사용할 기본값입니다.
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
```
### after
```cpp


#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
```

## openGL_3dAssign_16/Console.cpp
### before
```cpp
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
```
### after
```cpp
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

/// 지정한 피벗 주위로 회전한다.
glm::mat4 rotateAround(const glm::vec3& pivot, const glm::vec3& axis, float degrees) {
    return glm::translate(glm::mat4(1.0f), pivot)
        * glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axis)
        * glm::translate(glm::mat4(1.0f), -pivot);
}

/// 지정한 피벗 주위로 크기를 변경한다.
glm::mat4 scaleAround(const glm::vec3& pivot, float scale) {
    return glm::translate(glm::mat4(1.0f), pivot)
        * glm::scale(glm::mat4(1.0f), glm::vec3(scale))
        * glm::translate(glm::mat4(1.0f), -pivot);
}

}

/// 장면 객체와 초기 상태를 구성한다.
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

/// 면별 애니메이션을 초기화한다.
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

/// 조작할 도형을 선택한다.
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

/// 전체 조작 상태를 초기화한다.
void Console::resetAll() {
    spinning_ = false;
    spinDegrees_ = 0.0f;
    application_.getRenderOptions() = graphics::RenderOptions{};
    selectShape(false);
}

/// 물체 변환을 장면에 반영한다.
void Console::applyObjectTransform() {
    graphics::Transform3D transform;
    transform.rotation = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(spinDegrees_), Y_AXIS));
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

/// 선택한 면 애니메이션을 시작한다.
void Console::beginMotion(Motion kind) {
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

/// 면별 변환을 장면에 반영한다.
void Console::applyFaceMotion() {
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
            compose(4, rotateAround({ 0.0f, HALF_SIZE, 0.0f }, Z_AXIS, progress));
            break;
        case Motion::Front:
            compose(0, rotateAround({ 0.0f, HALF_SIZE, HALF_SIZE }, X_AXIS, -90.0f * progress));
            break;
        case Motion::Side:

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

/// 키 입력에 따라 동작을 변경한다.
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

/// 프레임 시간으로 상태와 장면을 갱신한다.
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
                motion.running = false;
            }
        } else {
            const float period = kind == Motion::Swap ? 180.0f : 360.0f;
            motion.progress = std::fmod(motion.progress + ROTATION_SPEED * deltaTime, period);
        }
    }
    applyFaceMotion();
}

/// 피라미드 애니메이션을 시작한다.
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
/// 피라미드 애니메이션 시간을 갱신한다.
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
/// 피라미드 면 변환을 반영한다.
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
```

## openGL_3dAssign_16/main.cpp
### before
```cpp
#include "Console.h"
#include <iostream>

int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 16";
    application.setInitHandler([&](graphics::Application&) {
        return practice.initScene();
    });
    application.setKeyHandler([&](graphics::Application&, int key, int action) {
        practice.onKey(key, action);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        practice.onUpdate(deltaTime);
    });
    return application.run(config);
}
```
### after
```cpp
#include "Console.h"
#include <iostream>

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 16";
    application.setInitHandler([&](graphics::Application&) {
        return practice.initScene();
    });
    application.setKeyHandler([&](graphics::Application&, int key, int action) {
        practice.onKey(key, action);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        practice.onUpdate(deltaTime);
    });
    return application.run(config);
}
```

## openGL_3dAssign_16/Console.h
### before
```cpp
#pragma once

#include "framework/include/Application.h"
#include <array>
#include <random>

// 실습 16: 각 면 모션의 상태를 독립적으로 관리하고 행렬을 합성한다.
class Console {
private:
    enum class Motion { None, Top, Front, Side, Back, Hinge, Swap, OrbitY, OrbitX, Count };
    struct FaceMotion {
        bool initialized = false;
        bool running = false;
        float progress = 0.0f; // 회전은 도, 열기·축소는 0~1이다.
        float direction = 1.0f;
        std::array<std::size_t, 2> pair{ 0, 3 };
        glm::vec3 hinge{ 0.4f, 0.0f, 0.4f };
    };

    enum class PyramidMotion { None, All, Sequence, Swap, Apex };
    PyramidMotion pyramidMotion_ = PyramidMotion::None;
    bool pyramidRunning_ = false;
    float pyramidProgress_ = 0, pyramidDirection_ = 1;
    void beginPyramid(PyramidMotion motion);
    void updatePyramid(float dt);
    void applyPyramid();
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    bool spinning_ = false;
    float spinDegrees_ = 0.0f;
    std::array<FaceMotion, static_cast<std::size_t>(Motion::Count)> motions_{};
    std::mt19937 random_{ std::random_device{}() };

    void selectShape(bool pyramid);
    void resetFaces();
    void resetAll();
    void beginMotion(Motion kind);
    void applyObjectTransform();
    void applyFaceMotion();

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    void onKey(int key, int action);
    void onUpdate(float deltaTime);
};
```
### after
```cpp
#pragma once

#include "framework/include/Application.h"
#include <array>
#include <random>

class Console {
private:
    enum class Motion { None, Top, Front, Side, Back, Hinge, Swap, OrbitY, OrbitX, Count };
    struct FaceMotion {
        bool initialized = false;
        bool running = false;
        float progress = 0.0f;
        float direction = 1.0f;
        std::array<std::size_t, 2> pair{ 0, 3 };
        glm::vec3 hinge{ 0.4f, 0.0f, 0.4f };
    };

    enum class PyramidMotion { None, All, Sequence, Swap, Apex };
    PyramidMotion pyramidMotion_ = PyramidMotion::None;
    bool pyramidRunning_ = false;
    float pyramidProgress_ = 0, pyramidDirection_ = 1;
    /// 피라미드 애니메이션을 시작한다.
    void beginPyramid(PyramidMotion motion);
    /// 피라미드 애니메이션 시간을 갱신한다.
    void updatePyramid(float dt);
    /// 피라미드 면 변환을 반영한다.
    void applyPyramid();
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    bool spinning_ = false;
    float spinDegrees_ = 0.0f;
    std::array<FaceMotion, static_cast<std::size_t>(Motion::Count)> motions_{};
    std::mt19937 random_{ std::random_device{}() };

    /// 조작할 도형을 선택한다.
    void selectShape(bool pyramid);
    /// 면별 애니메이션을 초기화한다.
    void resetFaces();
    /// 전체 조작 상태를 초기화한다.
    void resetAll();
    /// 선택한 면 애니메이션을 시작한다.
    void beginMotion(Motion kind);
    /// 물체 변환을 장면에 반영한다.
    void applyObjectTransform();
    /// 면별 변환을 장면에 반영한다.
    void applyFaceMotion();

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& application) : application_(application) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float deltaTime);
};
```

## openGL_3dAssign_17/Console.cpp
### before
```cpp
#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
glm::mat3 turn(float degrees,glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1),glm::radians(degrees),axis));
}
}
graphics::Model Console::model(int shape) const {
    if(shape==0) {
        auto cone=graphics::DiagramSystem::makeCone(.5f,1.0f);
        for(auto& face:cone.faces)
            face.outline=graphics::OutlineMode::AllTriangleEdges;
        return cone;
    }
    if(shape==1)return graphics::DiagramSystem::makeSphere(.5f);
    return graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube);
}
glm::mat3 Console::orbitRotation(std::size_t i) const {
    return turn(objects_[i].orbitDegrees,{0,1,0});
}
glm::vec3 Console::worldPosition(std::size_t i) const {
    return orbitRotation(i)*objects_[i].position;
}
bool Console::initScene() {
    reset();
    std::cout << "Practice 17: 1 left, 2 right, 3 both; x/X,y/Y spin; r/R orbit\n"
              << "a/A local scale, b/B origin scale, d/D left/right, e/E up/down\n"
              << "t straight swap, u bypass swap, v scale/spin/orbit; c shape, s reset, q quit\n";
    return true;
}
void Console::reset() {
    auto& scene=app_.getScene();
    scene.clear();
    objects_={};
    movement_=glm::vec2(0);
    selected_=3;
    motion_=Motion::None;
    running_=false;
    elapsed_=0;
    for(std::size_t i=0;i<2;++i) {
        auto& o=objects_[i];
        o.position={i==0?-1.0f:1.0f,0,0};
        o.shape=i==0?0:2;
        o.orbit=scene.addGroup().value();
        o.body=scene.addModel(model(o.shape),graphics::Transform3D{},graphics::PivotMode::Preserve).value();
        if(!scene.setParent(o.body,o.orbit)||!scene.setTint(o.body,i==0?graphics::GlColor{1,.65f,.3f}:graphics::GlColor{.4f,.8f,1}))
            throw std::runtime_error("Cannot initialize object hierarchy.");
    }
    scene.addLine({-2,0,0},{2,0,0},{1,.2f,.2f});
    scene.addLine({0,-2,0},{0,2,0},{.2f,1,.2f});
    scene.addLine({0,0,-2},{0,0,2},{.2f,.4f,1});
    auto& camera=app_.getCamera();
    camera=graphics::Camera{};
    const auto inverseView=glm::transpose(turn(30,{1,0,0})*turn(30,{0,1,0}));
    camera.position=inverseView*glm::vec3(0,0,6);
    camera.up=inverseView*glm::vec3(0,1,0);
    camera.orthographic=true;
    camera.orthographicHalfExtent=2.4f;
    app_.getRenderOptions()=graphics::RenderOptions{};
    app_.getRenderOptions().wireframe=true;
    apply();
}
void Console::apply() {
    auto& scene=app_.getScene();
    for(std::size_t i=0;i<2;++i) {
        const auto& o=objects_[i];
        graphics::Transform3D orbit,body;

        orbit.rotation=orbitRotation(i);
        body.basis=o.position;
        body.rotation=o.rotation;
        if (o.shape == 0)
            body.rotation *= turn(-45.0f, { 0,0,1});
        body.scale=glm::vec3(o.scale);
        if(!scene.setTransform(o.orbit,orbit)||!scene.setTransform(o.body,body))
            throw std::runtime_error("Invalid model transform.");
    }
}
void Console::begin(Motion motion) {
    if(motion_==motion) {
        running_=!running_;
        return;
    }
    motion_=motion;
    running_=true;
    elapsed_=0;
    for(std::size_t i=0;i<2;++i) {
        starts_[i]=worldPosition(i);
        scales_[i]=objects_[i].scale;
    }
}
void Console::onKey(int key,int action,int mods) {
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    const float sign=upper?-1.0f:1.0f;
    if(key==GLFW_KEY_D||key==GLFW_KEY_E) {
        if(action==GLFW_PRESS||action==GLFW_RELEASE) {
            float& direction=key==GLFW_KEY_D?movement_.x:movement_.y;
            direction=action==GLFW_RELEASE?0:(key==GLFW_KEY_D?-sign:sign);
        }
        return;
    }
    if(action!=GLFW_PRESS)return;
    if(key==GLFW_KEY_Q||key==GLFW_KEY_ESCAPE){
        app_.requestClose();
        return;
    }
    if(key>=GLFW_KEY_1&&key<=GLFW_KEY_3){
        selected_=key-GLFW_KEY_0;
        return;
    }
    if(key==GLFW_KEY_S){
        reset();
        return;
    }
    if(key==GLFW_KEY_T){
        begin(Motion::Straight);
        return;
    }
    if(key==GLFW_KEY_U){
        begin(Motion::Bypass);
        return;
    }
    if(key==GLFW_KEY_V){
        begin(Motion::Combined);
        return;
    }
    if(key==GLFW_KEY_C) {
        for(auto& o:objects_) {
            o.shape=(o.shape+1)%3;
            if(!app_.getScene().setModel(o.body,model(o.shape)))throw std::runtime_error("Cannot replace model.");
        }
        apply();
        return;
    }
    for(std::size_t i=0;i<2;++i) {
        if(selected_!=3&&selected_!=int(i+1))continue;
        auto& o=objects_[i];
        switch(key) {
        case GLFW_KEY_X:
            o.spin.x=o.spin.x==sign?0:sign;
            break;
        case GLFW_KEY_Y:
            o.spin.y=o.spin.y==sign?0:sign;
            break;
        case GLFW_KEY_R:
            o.orbitDirection=o.orbitDirection==sign?0:sign;
            break;
        case GLFW_KEY_A:
            o.scale=std::clamp(o.scale*(upper?1/1.1f:1.1f),.15f,3.0f);
            break;
        case GLFW_KEY_B:
            {
            const float old=o.scale;
            o.scale=std::clamp(old*(upper?1/1.1f:1.1f),.15f,3.0f);
            o.position*=o.scale/old;
            break;
        }
        default:
            break;
        }
    }
    // A paused animation resumes from the edited pose rather than restoring stale captures.
    if(motion_!=Motion::None && !running_) {
        for(std::size_t i=0;i<2;++i){
            starts_[i]=worldPosition(i);
            scales_[i]=objects_[i].scale;
        }
        elapsed_=0;
    }
    apply();
}
void Console::onUpdate(float dt) {
    if(running_)elapsed_+=dt;
    for(std::size_t i=0;i<2;++i) {
        auto& o=objects_[i];
        if(selected_==3  ||  selected_==int(i+1))
            o.position+=glm::transpose(orbitRotation(i))*glm::vec3(movement_*.7f*dt,0);

        const bool combined=motion_==Motion::Combined;
        const auto spin=combined?(running_?glm::vec2(0,1):glm::vec2(0)):o.spin;

        const float orbit=combined?(running_?1.0f:0.0f):o.orbitDirection;
        o.rotation=o.rotation*turn(45*dt*spin.x,{1,0,0})*turn(45*dt*spin.y,{0,1,0});

        const auto x=glm::normalize(o.rotation[0]),z=glm::normalize(glm::cross(x,o.rotation[1]));

        o.rotation=glm::mat3(x,glm::cross(z,x),z);
        o.orbitDegrees=std::fmod(o.orbitDegrees+30*dt*orbit,360.0f);

        if(running_&&combined)o.scale=scales_[i]*(1+(i==0?.35f:-.35f)*std::sin(elapsed_));
        else if(running_&&(motion_==Motion::Straight||motion_==Motion::Bypass)) {
            const float t=std::min(elapsed_/2.0f,1.0f);
            auto point=glm::mix(starts_[i],starts_[1-i],t);
            if(motion_==Motion::Bypass)point.y+=(i==0?1.0f:-1.0f)*.9f*std::sin(3.14159265f*t);
            o.position=glm::transpose(orbitRotation(i))*point;
        }
    }
    if(running_&&(motion_==Motion::Straight||motion_==Motion::Bypass)&&elapsed_>=2) {
        motion_=Motion::None;
        running_=false;
    }
    apply();
}

void Console::onFocus(bool focused) {
    if(!focused)movement_=glm::vec2(0);
}
```
### after
```cpp
#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
/// 축과 각도로 회전 행렬을 만든다.
glm::mat3 turn(float degrees,glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1),glm::radians(degrees),axis));
}
}
/// 선택한 도형의 모델을 만든다.
graphics::Model Console::model(int shape) const {
    if(shape==0) {
        auto cone=graphics::DiagramSystem::makeCone(.5f,1.0f);
        for(auto& face:cone.faces)
            face.outline=graphics::OutlineMode::AllTriangleEdges;
        return cone;
    }
    if(shape==1)return graphics::DiagramSystem::makeSphere(.5f);
    return graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube);
}
/// 공전 회전 행렬을 계산한다.
glm::mat3 Console::orbitRotation(std::size_t i) const {
    return turn(objects_[i].orbitDegrees,{0,1,0});
}
/// 물체의 월드 위치를 계산한다.
glm::vec3 Console::worldPosition(std::size_t i) const {
    return orbitRotation(i)*objects_[i].position;
}
/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    reset();
    std::cout << "Practice 17: 1 left, 2 right, 3 both; x/X,y/Y spin; r/R orbit\n"
              << "a/A local scale, b/B origin scale, d/D left/right, e/E up/down\n"
              << "t straight swap, u bypass swap, v scale/spin/orbit; c shape, s reset, q quit\n";
    return true;
}
/// 배치와 조작 상태를 초기화한다.
void Console::reset() {
    auto& scene=app_.getScene();
    scene.clear();
    objects_={};
    movement_=glm::vec2(0);
    selected_=3;
    motion_=Motion::None;
    running_=false;
    elapsed_=0;
    for(std::size_t i=0;i<2;++i) {
        auto& o=objects_[i];
        o.position={i==0?-1.0f:1.0f,0,0};
        o.shape=i==0?0:2;
        o.orbit=scene.addGroup().value();
        o.body=scene.addModel(model(o.shape),graphics::Transform3D{},graphics::PivotMode::Preserve).value();
        if(!scene.setParent(o.body,o.orbit)||!scene.setTint(o.body,i==0?graphics::GlColor{1,.65f,.3f}:graphics::GlColor{.4f,.8f,1}))
            throw std::runtime_error("Cannot initialize object hierarchy.");
    }
    scene.addLine({-2,0,0},{2,0,0},{1,.2f,.2f});
    scene.addLine({0,-2,0},{0,2,0},{.2f,1,.2f});
    scene.addLine({0,0,-2},{0,0,2},{.2f,.4f,1});
    auto& camera=app_.getCamera();
    camera=graphics::Camera{};
    const auto inverseView=glm::transpose(turn(30,{1,0,0})*turn(30,{0,1,0}));
    camera.position=inverseView*glm::vec3(0,0,6);
    camera.up=inverseView*glm::vec3(0,1,0);
    camera.orthographic=true;
    camera.orthographicHalfExtent=2.4f;
    app_.getRenderOptions()=graphics::RenderOptions{};
    app_.getRenderOptions().wireframe=true;
    apply();
}
/// 계산한 변환을 장면에 반영한다.
void Console::apply() {
    auto& scene=app_.getScene();
    for(std::size_t i=0;i<2;++i) {
        const auto& o=objects_[i];
        graphics::Transform3D orbit,body;

        orbit.rotation=orbitRotation(i);
        body.basis=o.position;
        body.rotation=o.rotation;
        if (o.shape == 0)
            body.rotation *= turn(-45.0f, { 0,0,1});
        body.scale=glm::vec3(o.scale);
        if(!scene.setTransform(o.orbit,orbit)||!scene.setTransform(o.body,body))
            throw std::runtime_error("Invalid model transform.");
    }
}
/// 선택한 이동을 시작한다.
void Console::begin(Motion motion) {
    if(motion_==motion) {
        running_=!running_;
        return;
    }
    motion_=motion;
    running_=true;
    elapsed_=0;
    for(std::size_t i=0;i<2;++i) {
        starts_[i]=worldPosition(i);
        scales_[i]=objects_[i].scale;
    }
}
/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key,int action,int mods) {
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    const float sign=upper?-1.0f:1.0f;
    if(key==GLFW_KEY_D||key==GLFW_KEY_E) {
        if(action==GLFW_PRESS||action==GLFW_RELEASE) {
            float& direction=key==GLFW_KEY_D?movement_.x:movement_.y;
            direction=action==GLFW_RELEASE?0:(key==GLFW_KEY_D?-sign:sign);
        }
        return;
    }
    if(action!=GLFW_PRESS)return;
    if(key==GLFW_KEY_Q||key==GLFW_KEY_ESCAPE){
        app_.requestClose();
        return;
    }
    if(key>=GLFW_KEY_1&&key<=GLFW_KEY_3){
        selected_=key-GLFW_KEY_0;
        return;
    }
    if(key==GLFW_KEY_S){
        reset();
        return;
    }
    if(key==GLFW_KEY_T){
        begin(Motion::Straight);
        return;
    }
    if(key==GLFW_KEY_U){
        begin(Motion::Bypass);
        return;
    }
    if(key==GLFW_KEY_V){
        begin(Motion::Combined);
        return;
    }
    if(key==GLFW_KEY_C) {
        for(auto& o:objects_) {
            o.shape=(o.shape+1)%3;
            if(!app_.getScene().setModel(o.body,model(o.shape)))throw std::runtime_error("Cannot replace model.");
        }
        apply();
        return;
    }
    for(std::size_t i=0;i<2;++i) {
        if(selected_!=3&&selected_!=int(i+1))continue;
        auto& o=objects_[i];
        switch(key) {
        case GLFW_KEY_X:
            o.spin.x=o.spin.x==sign?0:sign;
            break;
        case GLFW_KEY_Y:
            o.spin.y=o.spin.y==sign?0:sign;
            break;
        case GLFW_KEY_R:
            o.orbitDirection=o.orbitDirection==sign?0:sign;
            break;
        case GLFW_KEY_A:
            o.scale=std::clamp(o.scale*(upper?1/1.1f:1.1f),.15f,3.0f);
            break;
        case GLFW_KEY_B:
            {
            const float old=o.scale;
            o.scale=std::clamp(old*(upper?1/1.1f:1.1f),.15f,3.0f);
            o.position*=o.scale/old;
            break;
        }
        default:
            break;
        }
    }

    if(motion_!=Motion::None && !running_) {
        for(std::size_t i=0;i<2;++i){
            starts_[i]=worldPosition(i);
            scales_[i]=objects_[i].scale;
        }
        elapsed_=0;
    }
    apply();
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    if(running_)elapsed_+=dt;
    for(std::size_t i=0;i<2;++i) {
        auto& o=objects_[i];
        if(selected_==3  ||  selected_==int(i+1))
            o.position+=glm::transpose(orbitRotation(i))*glm::vec3(movement_*.7f*dt,0);

        const bool combined=motion_==Motion::Combined;
        const auto spin=combined?(running_?glm::vec2(0,1):glm::vec2(0)):o.spin;

        const float orbit=combined?(running_?1.0f:0.0f):o.orbitDirection;
        o.rotation=o.rotation*turn(45*dt*spin.x,{1,0,0})*turn(45*dt*spin.y,{0,1,0});

        const auto x=glm::normalize(o.rotation[0]),z=glm::normalize(glm::cross(x,o.rotation[1]));

        o.rotation=glm::mat3(x,glm::cross(z,x),z);
        o.orbitDegrees=std::fmod(o.orbitDegrees+30*dt*orbit,360.0f);

        if(running_&&combined)o.scale=scales_[i]*(1+(i==0?.35f:-.35f)*std::sin(elapsed_));
        else if(running_&&(motion_==Motion::Straight||motion_==Motion::Bypass)) {
            const float t=std::min(elapsed_/2.0f,1.0f);
            auto point=glm::mix(starts_[i],starts_[1-i],t);
            if(motion_==Motion::Bypass)point.y+=(i==0?1.0f:-1.0f)*.9f*std::sin(3.14159265f*t);
            o.position=glm::transpose(orbitRotation(i))*point;
        }
    }
    if(running_&&(motion_==Motion::Straight||motion_==Motion::Bypass)&&elapsed_>=2) {
        motion_=Motion::None;
        running_=false;
    }
    apply();
}

/// 포커스가 없으면 지속 입력을 해제한다.
void Console::onFocus(bool focused) {
    if(!focused)movement_=glm::vec2(0);
}
```

## openGL_3dAssign_17/main.cpp
### before
```cpp
#include "Console.h"
int main() {
    graphics::Application app;
    Console practice(app);

    app.setInitHandler([&](graphics::Application&){
        return practice.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
        practice.onKey(key,action,mods);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt){
        practice.onUpdate(dt);
    });
    app.setFocusHandler([&](graphics::Application&,bool focused){
        practice.onFocus(focused);
    });

    graphics::ApplicationConfig config;
    config.title="OpenGL Practice 17";
    return app.run(config);
}
```
### after
```cpp
#include "Console.h"
/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    graphics::Application app;
    Console practice(app);

    app.setInitHandler([&](graphics::Application&){
        return practice.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
        practice.onKey(key,action,mods);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt){
        practice.onUpdate(dt);
    });
    app.setFocusHandler([&](graphics::Application&,bool focused){
        practice.onFocus(focused);
    });

    graphics::ApplicationConfig config;
    config.title="OpenGL Practice 17";
    return app.run(config);
}
```

## openGL_3dAssign_17/Console.h
### before
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>

class Console {
    struct Object {
        graphics::EntityId orbit=0,body=0;
        glm::vec3 position{0};
        glm::mat3 rotation{1};
        glm::vec2 spin{0};
        float orbitDegrees=0,orbitDirection=0,scale=.65f;
        int shape=0;
    };
    enum class Motion { None, Straight, Bypass, Combined };
    graphics::Application& app_;
    std::array<Object,2> objects_{};
    int selected_=3;
    glm::vec2 movement_{0};
    Motion motion_=Motion::None;
    bool running_=false;
    float elapsed_=0;
    std::array<glm::vec3,2> starts_{};
    std::array<float,2> scales_{};
    void reset();
    void apply();
    void begin(Motion motion);
    graphics::Model model(int shape) const;
    glm::mat3 orbitRotation(std::size_t i) const;
    glm::vec3 worldPosition(std::size_t i) const;
public:
    explicit Console(graphics::Application& app):app_(app){}
    bool initScene();
    void onKey(int key,int action,int mods);
    void onUpdate(float dt);
    void onFocus(bool focused);
};
```
### after
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>

class Console {
    struct Object {
        graphics::EntityId orbit=0,body=0;
        glm::vec3 position{0};
        glm::mat3 rotation{1};
        glm::vec2 spin{0};
        float orbitDegrees=0,orbitDirection=0,scale=.65f;
        int shape=0;
    };
    enum class Motion { None, Straight, Bypass, Combined };
    graphics::Application& app_;
    std::array<Object,2> objects_{};
    int selected_=3;
    glm::vec2 movement_{0};
    Motion motion_=Motion::None;
    bool running_=false;
    float elapsed_=0;
    std::array<glm::vec3,2> starts_{};
    std::array<float,2> scales_{};
    /// 배치와 조작 상태를 초기화한다.
    void reset();
    /// 계산한 변환을 장면에 반영한다.
    void apply();
    /// 선택한 이동을 시작한다.
    void begin(Motion motion);
    /// 선택한 도형의 모델을 만든다.
    graphics::Model model(int shape) const;
    /// 공전 회전 행렬을 계산한다.
    glm::mat3 orbitRotation(std::size_t i) const;
    /// 물체의 월드 위치를 계산한다.
    glm::vec3 worldPosition(std::size_t i) const;
public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app):app_(app){}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key,int action,int mods);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused);
};
```

## openGL_3dAssign_18/Console.cpp
### before
```cpp
#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
glm::mat3 turn(float degrees,glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1),glm::radians(degrees),axis));
}
constexpr std::array<float,3> radii{2.0f,2.8f,3.6f};
constexpr std::array<float,3> inclinations{0,45,-45};
void circle(graphics::Scene& scene,graphics::EntityId owner,graphics::GlColor color,bool xz) {
    constexpr int segments=96;
    for(int i=0;i<segments;++i) {
        const float a=6.283185307f*i/segments,b=6.283185307f*(i+1)/segments;
        const auto point=[&](float angle){
            return xz?glm::vec3(std::cos(angle),0,std::sin(angle))
                                                         :glm::vec3(std::cos(angle),std::sin(angle),0);
        };
        if(!scene.addLine(point(a),point(b),color,owner))throw std::runtime_error("Cannot add orbit line.");
    }
}
}
bool Console::initScene() {
    auto& scene=app_.getScene();
    scene.clear();
    root_=scene.addGroup().value();
    sun_=scene.addSphere(.55f,{0,0,0}).value();
    if(!scene.setParent(sun_,root_))return false;
    auto& camera=app_.getCamera();
    camera=graphics::Camera{};
    camera.position={0,5,8};
    camera.target={0,0,0};
    camera.orthographic=true;
    camera.orthographicHalfExtent=5.2f;
    const std::array<graphics::GlColor,3> colors{graphics::GlColor{1,.25f,.2f},{.2f,.65f,1},{.3f,1,.4f}};
    const std::array<graphics::GlColor,3> moonColors{graphics::GlColor{.8f,.3f,1},{1,.5f,.15f},{.4f,1,.85f}};
    for(std::size_t i=0;i<3;++i) {
        tracks_[i]=scene.addGroup().value();
        planets_[i]=scene.addGroup().value();
        moonFrames_[i]=scene.addGroup().value();
        moonTracks_[i]=scene.addGroup().value();
        const auto planet=scene.addSphere(.24f+.06f*float(i),{0,0,0}).value();
        const float moonRadius=.09f+.015f*float(i);
        moons_[i]=scene.addSphere(moonRadius,{0,0,0}).value();
        if(!scene.setParent(tracks_[i],root_)||!scene.setParent(planets_[i],root_)
            ||!scene.setParent(planet,planets_[i])||!scene.setParent(moonFrames_[i],planets_[i])
            ||!scene.setParent(moonTracks_[i],moonFrames_[i])||!scene.setParent(moons_[i],moonFrames_[i])
            ||!scene.setTint(planet,colors[i])||!scene.setTint(moons_[i],moonColors[i]))return false;
        circle(scene,tracks_[i],colors[i],true);
        circle(scene,moonTracks_[i],moonColors[i],true);
        if(!scene.addLine({moonRadius,0,0},{2*moonRadius,0,0},{1,1,.2f},moons_[i]))return false;
    }
    app_.getRenderOptions()=graphics::RenderOptions{};
    app_.getRenderOptions().lighting=true;
    apply();
    std::cout << "Practice 18: p orthographic / P perspective; m solid / M wire\n"
              << "WASD move XY, +/- move Z; 1 orbit size; 2 sun pulse/color; 3 moon orbit rotation/spin; q quit\n";
    return true;
}
void Console::apply() {
    auto& scene=app_.getScene();
    if(!scene.setPosition(root_,position_))throw std::runtime_error("Invalid system position.");

    graphics::Transform3D sun;
    sun.scale=glm::vec3(1+.18f*std::sin(sunTime_));

    if(!scene.setTransform(sun_,sun)||!scene.setTint(sun_,{1,.65f+.2f*std::sin(sunTime_),.15f+.1f*std::cos(sunTime_)}))
        throw std::runtime_error("Invalid sun transform.");

    for(std::size_t i=0;i<3;++i) {
        const auto tilt=turn(inclinations[i],{0,0,1});
        graphics::Transform3D track;
        track.rotation=tilt;
        track.scale=glm::vec3(radii[i]*radius_);
        const auto planet=tilt*turn(planetAngle_[i],{0,1,0})*glm::vec3(radii[i]*radius_,0,0);
        graphics::Transform3D frame;
        frame.rotation=turn(moonTilt_,{0,0,1});
        const float r=(.55f+.15f*float(i))*radius_,a=glm::radians(moonAngle_[i]);
        graphics::Transform3D moonTrack;
        moonTrack.scale=glm::vec3(r);
        graphics::Transform3D moon;
        moon.basis={r*std::cos(a),0,r*std::sin(a)};
        moon.rotation=turn(moonSpin_,{0,1,0});
        if(!scene.setTransform(tracks_[i],track)||!scene.setPosition(planets_[i],planet)
            ||!scene.setTransform(moonFrames_[i],frame)||!scene.setTransform(moonTracks_[i],moonTrack)
            ||!scene.setTransform(moons_[i],moon))
            throw std::runtime_error("Invalid orbit transform.");
    }
}
void Console::onKey(int key,int action,int mods) {
    if(key>=0&&key<=GLFW_KEY_LAST) {
        if(action==GLFW_PRESS)pressed_[key]=true;
        else if(action==GLFW_RELEASE)pressed_[key]=false;
    }
    if(action!=GLFW_PRESS)return;
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    switch(key) {
    case GLFW_KEY_P:
        app_.getCamera().orthographic=!upper;
        break;
    case GLFW_KEY_M:
        app_.getRenderOptions().wireframe=upper;
        break;
    case GLFW_KEY_1:
        targetRadius_=targetRadius_==1?1.3f:1.0f;
        break;
    case GLFW_KEY_2:
        pulse_=!pulse_;
        break;
    case GLFW_KEY_3:
        rotateMoons_=!rotateMoons_;
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}
void Console::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_D])-float(pressed_[GLFW_KEY_A]),
        float(pressed_[GLFW_KEY_W])-float(pressed_[GLFW_KEY_S]),
        float(pressed_[GLFW_KEY_EQUAL]||pressed_[GLFW_KEY_KP_ADD])-float(pressed_[GLFW_KEY_MINUS]||pressed_[GLFW_KEY_KP_SUBTRACT])};
    if(glm::length(movement)>0)position_+=glm::normalize(movement)*dt*1.5f;
    radius_+=std::clamp(targetRadius_-radius_,-.3f*dt,.3f*dt);
    if(pulse_)sunTime_=std::fmod(sunTime_+dt,6.283185307f);
    if(rotateMoons_) {
        moonTilt_=std::fmod(moonTilt_+25*dt,360.0f);
        moonSpin_=std::fmod(moonSpin_+90*dt,360.0f);
    }
    for(std::size_t i=0;i<3;++i) {
        planetAngle_[i]=std::fmod(planetAngle_[i]+(20+10*float(i))*dt,360.0f);
        moonAngle_[i]=std::fmod(moonAngle_[i]+(70+30*float(i))*dt,360.0f);
    }
    apply();
}
void Console::onFocus(bool focused) {
    if(!focused)pressed_.fill(false);
}
```
### after
```cpp
#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
/// 축과 각도로 회전 행렬을 만든다.
glm::mat3 turn(float degrees,glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1),glm::radians(degrees),axis));
}
constexpr std::array<float,3> radii{2.0f,2.8f,3.6f};
constexpr std::array<float,3> inclinations{0,45,-45};
/// 부모에 연결한 원 궤도를 그린다.
void circle(graphics::Scene& scene,graphics::EntityId owner,graphics::GlColor color,bool xz) {
    constexpr int segments=96;
    for(int i=0;i<segments;++i) {
        const float a=6.283185307f*i/segments,b=6.283185307f*(i+1)/segments;
        const auto point=[&](float angle){
            return xz?glm::vec3(std::cos(angle),0,std::sin(angle))
                                                         :glm::vec3(std::cos(angle),std::sin(angle),0);
        };
        if(!scene.addLine(point(a),point(b),color,owner))throw std::runtime_error("Cannot add orbit line.");
    }
}
}
/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    auto& scene=app_.getScene();
    scene.clear();
    root_=scene.addGroup().value();
    sun_=scene.addSphere(.55f,{0,0,0}).value();
    if(!scene.setParent(sun_,root_))return false;
    auto& camera=app_.getCamera();
    camera=graphics::Camera{};
    camera.position={0,5,8};
    camera.target={0,0,0};
    camera.orthographic=true;
    camera.orthographicHalfExtent=5.2f;
    const std::array<graphics::GlColor,3> colors{graphics::GlColor{1,.25f,.2f},{.2f,.65f,1},{.3f,1,.4f}};
    const std::array<graphics::GlColor,3> moonColors{graphics::GlColor{.8f,.3f,1},{1,.5f,.15f},{.4f,1,.85f}};
    for(std::size_t i=0;i<3;++i) {
        tracks_[i]=scene.addGroup().value();
        planets_[i]=scene.addGroup().value();
        moonFrames_[i]=scene.addGroup().value();
        moonTracks_[i]=scene.addGroup().value();
        const auto planet=scene.addSphere(.24f+.06f*float(i),{0,0,0}).value();
        const float moonRadius=.09f+.015f*float(i);
        moons_[i]=scene.addSphere(moonRadius,{0,0,0}).value();
        if(!scene.setParent(tracks_[i],root_)||!scene.setParent(planets_[i],root_)
            ||!scene.setParent(planet,planets_[i])||!scene.setParent(moonFrames_[i],planets_[i])
            ||!scene.setParent(moonTracks_[i],moonFrames_[i])||!scene.setParent(moons_[i],moonFrames_[i])
            ||!scene.setTint(planet,colors[i])||!scene.setTint(moons_[i],moonColors[i]))return false;
        circle(scene,tracks_[i],colors[i],true);
        circle(scene,moonTracks_[i],moonColors[i],true);
        if(!scene.addLine({moonRadius,0,0},{2*moonRadius,0,0},{1,1,.2f},moons_[i]))return false;
    }
    app_.getRenderOptions()=graphics::RenderOptions{};
    app_.getRenderOptions().lighting=true;
    apply();
    std::cout << "Practice 18: p orthographic / P perspective; m solid / M wire\n"
              << "WASD move XY, +/- move Z; 1 orbit size; 2 sun pulse/color; 3 moon orbit rotation/spin; q quit\n";
    return true;
}
/// 계산한 변환을 장면에 반영한다.
void Console::apply() {
    auto& scene=app_.getScene();
    if(!scene.setPosition(root_,position_))throw std::runtime_error("Invalid system position.");

    graphics::Transform3D sun;
    sun.scale=glm::vec3(1+.18f*std::sin(sunTime_));

    if(!scene.setTransform(sun_,sun)||!scene.setTint(sun_,{1,.65f+.2f*std::sin(sunTime_),.15f+.1f*std::cos(sunTime_)}))
        throw std::runtime_error("Invalid sun transform.");

    for(std::size_t i=0;i<3;++i) {
        const auto tilt=turn(inclinations[i],{0,0,1});
        graphics::Transform3D track;
        track.rotation=tilt;
        track.scale=glm::vec3(radii[i]*radius_);
        const auto planet=tilt*turn(planetAngle_[i],{0,1,0})*glm::vec3(radii[i]*radius_,0,0);
        graphics::Transform3D frame;
        frame.rotation=turn(moonTilt_,{0,0,1});
        const float r=(.55f+.15f*float(i))*radius_,a=glm::radians(moonAngle_[i]);
        graphics::Transform3D moonTrack;
        moonTrack.scale=glm::vec3(r);
        graphics::Transform3D moon;
        moon.basis={r*std::cos(a),0,r*std::sin(a)};
        moon.rotation=turn(moonSpin_,{0,1,0});
        if(!scene.setTransform(tracks_[i],track)||!scene.setPosition(planets_[i],planet)
            ||!scene.setTransform(moonFrames_[i],frame)||!scene.setTransform(moonTracks_[i],moonTrack)
            ||!scene.setTransform(moons_[i],moon))
            throw std::runtime_error("Invalid orbit transform.");
    }
}
/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key,int action,int mods) {
    if(key>=0&&key<=GLFW_KEY_LAST) {
        if(action==GLFW_PRESS)pressed_[key]=true;
        else if(action==GLFW_RELEASE)pressed_[key]=false;
    }
    if(action!=GLFW_PRESS)return;
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    switch(key) {
    case GLFW_KEY_P:
        app_.getCamera().orthographic=!upper;
        break;
    case GLFW_KEY_M:
        app_.getRenderOptions().wireframe=upper;
        break;
    case GLFW_KEY_1:
        targetRadius_=targetRadius_==1?1.3f:1.0f;
        break;
    case GLFW_KEY_2:
        pulse_=!pulse_;
        break;
    case GLFW_KEY_3:
        rotateMoons_=!rotateMoons_;
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_D])-float(pressed_[GLFW_KEY_A]),
        float(pressed_[GLFW_KEY_W])-float(pressed_[GLFW_KEY_S]),
        float(pressed_[GLFW_KEY_EQUAL]||pressed_[GLFW_KEY_KP_ADD])-float(pressed_[GLFW_KEY_MINUS]||pressed_[GLFW_KEY_KP_SUBTRACT])};
    if(glm::length(movement)>0)position_+=glm::normalize(movement)*dt*1.5f;
    radius_+=std::clamp(targetRadius_-radius_,-.3f*dt,.3f*dt);
    if(pulse_)sunTime_=std::fmod(sunTime_+dt,6.283185307f);
    if(rotateMoons_) {
        moonTilt_=std::fmod(moonTilt_+25*dt,360.0f);
        moonSpin_=std::fmod(moonSpin_+90*dt,360.0f);
    }
    for(std::size_t i=0;i<3;++i) {
        planetAngle_[i]=std::fmod(planetAngle_[i]+(20+10*float(i))*dt,360.0f);
        moonAngle_[i]=std::fmod(moonAngle_[i]+(70+30*float(i))*dt,360.0f);
    }
    apply();
}
/// 포커스가 없으면 지속 입력을 해제한다.
void Console::onFocus(bool focused) {
    if(!focused)pressed_.fill(false);
}
```

## openGL_3dAssign_18/main.cpp
### before
```cpp
#include "Console.h"
int main() {
    graphics::Application app;
    Console practice(app);
    app.setInitHandler([&](graphics::Application&){
        return practice.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
        practice.onKey(key,action,mods);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt){
        practice.onUpdate(dt);
    });
    app.setFocusHandler([&](graphics::Application&,bool focused){
        practice.onFocus(focused);
    });
    graphics::ApplicationConfig config;
    config.title="OpenGL Practice 18";
    return app.run(config);
}
```
### after
```cpp
#include "Console.h"
/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    graphics::Application app;
    Console practice(app);
    app.setInitHandler([&](graphics::Application&){
        return practice.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
        practice.onKey(key,action,mods);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt){
        practice.onUpdate(dt);
    });
    app.setFocusHandler([&](graphics::Application&,bool focused){
        practice.onFocus(focused);
    });
    graphics::ApplicationConfig config;
    config.title="OpenGL Practice 18";
    return app.run(config);
}
```

## openGL_3dAssign_18/Console.h
### before
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_=0,sun_=0;
    std::array<graphics::EntityId,3> tracks_{},planets_{},moonFrames_{},moonTracks_{},moons_{};
    std::array<bool,GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 position_{0};
    std::array<float,3> planetAngle_{0,120,240},moonAngle_{0,120,240};
    float radius_=1,targetRadius_=1,sunTime_=0,moonTilt_=0,moonSpin_=0;
    bool pulse_=false,rotateMoons_=false;
    void apply();
public:
    explicit Console(graphics::Application& app):app_(app){}
    bool initScene();
    void onKey(int key,int action,int mods);
    void onUpdate(float dt);
    void onFocus(bool focused);
};
```
### after
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_=0,sun_=0;
    std::array<graphics::EntityId,3> tracks_{},planets_{},moonFrames_{},moonTracks_{},moons_{};
    std::array<bool,GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 position_{0};
    std::array<float,3> planetAngle_{0,120,240},moonAngle_{0,120,240};
    float radius_=1,targetRadius_=1,sunTime_=0,moonTilt_=0,moonSpin_=0;
    bool pulse_=false,rotateMoons_=false;
    /// 계산한 변환을 장면에 반영한다.
    void apply();
public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app):app_(app){}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key,int action,int mods);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused);
};
```

## openGL_3dAssign_19/Console.cpp
### before
```cpp
#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
glm::mat3 turn(float degrees,glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1),glm::radians(degrees),axis));
}
}
graphics::EntityId Console::box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent) {
    auto model=graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube,size);
    for(auto& face:model.faces)face.color={1,1,1};
    graphics::Transform3D transform;
    transform.basis=position;
    auto& scene=app_.getScene();
    const auto id=scene.addModel(model,transform,graphics::PivotMode::Preserve).value();
    if(!scene.setParent(id,parent))throw std::runtime_error("Cannot connect tank part.");
    colored_.push_back(id);
    return id;
}
bool Console::initScene() {
    reset();
    std::cout << "Practice 19: arrows move tank; d lower height; t central spin; l upper swap\n"
              << "g guns opposite yaw; p poles opposite pitch; x/X,z/Z camera translation\n"
              << "y/Y camera yaw, r/R camera orbit (repeat pauses); o colors; c reset; q quit\n";
    return true;
}
void Console::reset() {
    auto& scene=app_.getScene();
    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_=glm::vec3(0);
    lowerTime_=centralAngle_=gunAngle_=poleTime_=swapProgress_=0;
    swapDirection_=1;
    cameraX_=cameraZ_=cameraYaw_=cameraOrbit_=0;
    lowerRunning_=centralRunning_=gunRunning_=poleRunning_=swapRunning_=swapStarted_=false;
    colorVersion_=0;
    root_=scene.addGroup().value();
    central_=scene.addGroup().value();
    if(!scene.setParent(central_,root_))throw std::runtime_error("Cannot create tank root.");
    const auto ground=scene.addGroup().value();
    box({12,.1f,12},{0,-.05f,0},ground);
    for(std::size_t i=0;i<2;++i)lower_[i]=box({.65f,.6f,1.8f},{i==0?-.8f:.8f,.3f,0},root_);
    box({2.3f,.25f,1.4f},{0,.125f,0},central_);
    for(std::size_t i=0;i<2;++i) {
        upper_[i]=scene.addGroup().value();
        guns_[i]=scene.addGroup().value();
        poles_[i]=scene.addGroup().value();
        if(!scene.setParent(upper_[i],central_)||!scene.setParent(guns_[i],upper_[i])||!scene.setParent(poles_[i],upper_[i]))
            throw std::runtime_error("Cannot create upper tank hierarchy.");
        box({.65f,.6f,.8f},{0,.3f,0},upper_[i]);
        box({.14f,.14f,1.1f},{0,0,.55f},guns_[i]);
        box({.08f,.85f,.08f},{0,.425f,0},poles_[i]);
    }
    auto& camera=app_.getCamera();
    camera=graphics::Camera{};
    camera.position={-5,4.5f,7};
    camera.target={0,.7f,0};
    camera.orthographic=false;
    app_.getRenderOptions()=graphics::RenderOptions{};
    app_.getRenderOptions().lighting=true;
    recolor();
    apply();
}
void Console::recolor() {
    const std::array<graphics::GlColor,10> palette{
        graphics::GlColor{.3f,.35f,.4f},{.25f,.6f,.3f},{.55f,.35f,.2f},{.2f,.55f,.75f},
        {.7f,.75f,.2f},{.9f,.4f,.15f},{1,.8f,.2f},{.4f,.7f,.8f},{.7f,.25f,.6f},{.6f,.9f,.4f}};
    for(std::size_t i=0;i<colored_.size();++i)
        if(!app_.getScene().setTint(colored_[i],palette[(i+colorVersion_)%palette.size()]))
            throw std::runtime_error("Cannot color tank part.");
}
void Console::apply() {
    auto& scene=app_.getScene();
    if(!scene.setPosition(root_,position_))throw std::runtime_error("Invalid tank position.");
    const float factor=1+.25f*std::sin(lowerTime_),height=.6f*factor;
    graphics::Transform3D center;
    center.basis={0,height,0};
    center.rotation=turn(centralAngle_,{0,1,0});
    if(!scene.setTransform(central_,center))throw std::runtime_error("Invalid central transform.");
    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        graphics::Transform3D low;
        low.basis = { sign * .8f,height * .5f,0 };
        low.scale = { 1,factor,1 };
        graphics::Transform3D upper;
        const float a = 3.14159265f * swapProgress_;
        upper.basis = { sign * .65f * std::cos(a),.25f,sign * .65f * std::sin(a) };
        const glm::vec3 bodyCenter{0, .3f, 0};
        const glm::vec3 gunOffset{0, .25f, .4f};
        graphics::Transform3D gun;
        gun.rotation = turn(sign * gunAngle_, {0, 1, 0});
        gun.basis = bodyCenter
                  + gun.rotation * (gunOffset - bodyCenter);
        graphics::Transform3D pole;
        pole.basis={0,.6f,0};
        pole.rotation=turn(sign*60*std::sin(poleTime_),{1,0,0});
        if(!scene.setTransform(lower_[i],low)||!scene.setTransform(upper_[i],upper)
            ||!scene.setTransform(guns_[i],gun)||!scene.setTransform(poles_[i],pole))
            throw std::runtime_error("Invalid tank part transform.");
    }
}
void Console::onKey(int key,int action,int mods) {
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    if(key==GLFW_KEY_X||key==GLFW_KEY_Z) {
        if(action==GLFW_PRESS||action==GLFW_RELEASE) {
            float& direction=key==GLFW_KEY_X?cameraX_:cameraZ_;
            direction=action==GLFW_RELEASE?0:(upper?-1.0f:1.0f);
        }
        return;
    }
    if(key>=0&&key<=GLFW_KEY_LAST) {
        if(action==GLFW_PRESS)pressed_[key]=true;
        else if(action==GLFW_RELEASE)pressed_[key]=false;
    }
    if(action!=GLFW_PRESS)return;
    const float direction=upper?1.0f:-1.0f;
    switch(key) {
    case GLFW_KEY_D:
        lowerRunning_=!lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_=!centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_=!gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_=!poleRunning_;
        break;
    case GLFW_KEY_L:
        if(!swapStarted_){
            swapStarted_=true;
            swapRunning_=true;
        }
        else if(!swapRunning_&&(swapProgress_==0||swapProgress_==1)){
            swapDirection_=-swapDirection_;
            swapRunning_=true;
        }
        else swapRunning_=!swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_=cameraYaw_==direction?0:direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_=cameraOrbit_==direction?0:direction;
        break;
    case GLFW_KEY_O:
        colorVersion_=(colorVersion_+1)%10;
        recolor();
        break;
    case GLFW_KEY_C:
        reset();
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}
void Console::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT])-float(pressed_[GLFW_KEY_LEFT]),0,
        float(pressed_[GLFW_KEY_DOWN])-float(pressed_[GLFW_KEY_UP])};
    if(glm::length(movement)>0)position_+=glm::normalize(movement)*dt*1.3f;
    // 12x12 floor minus a 2.5-unit radius covers every animated part's sweep.
    position_.x=std::clamp(position_.x,-3.5f,3.5f);
    position_.z=std::clamp(position_.z,-3.5f,3.5f);
    if(lowerRunning_)lowerTime_=std::fmod(lowerTime_+dt,6.283185307f);
    if(centralRunning_)centralAngle_=std::fmod(centralAngle_+45*dt,360.0f);
    if(gunRunning_)gunAngle_= std::fmod(gunAngle_+60*dt,-360.0f);
    if(poleRunning_)poleTime_=std::fmod(poleTime_+dt,6.283185307f);
    if(swapRunning_) {
        swapProgress_=std::clamp(swapProgress_+swapDirection_*dt*.5f,0.0f,1.0f);
        if(swapProgress_==0||swapProgress_==1)swapRunning_=false;
    }
    auto camera=app_.getCamera();
    const glm::vec3 delta{cameraX_*2*dt,0,cameraZ_*2*dt};
    camera.position+=delta;
    camera.target+=delta;
    if(cameraYaw_!=0) {
        const auto forward=glm::normalize(camera.target-camera.position);
        const auto right=glm::normalize(glm::cross(forward,camera.up));
        const auto localUp=glm::normalize(glm::cross(right,forward));
        const auto rotation=turn(cameraYaw_*35*dt,localUp);
        camera.target=camera.position+rotation*(camera.target-camera.position);
        camera.up=rotation*localUp;
    }
    if(cameraOrbit_!=0) {
        const auto rotation=turn(cameraOrbit_*25*dt,{0,1,0});
        camera.position=rotation*camera.position;
        camera.target=rotation*camera.target;
        camera.up=rotation*camera.up;
    }
    if(graphics::CameraSystem::validCamera(camera))app_.getCamera()=camera;
    apply();
}
void Console::onFocus(bool focused) {
    if(!focused){
        pressed_.fill(false);
        cameraX_=cameraZ_=0;
    }
}
```
### after
```cpp
#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
/// 축과 각도로 회전 행렬을 만든다.
glm::mat3 turn(float degrees,glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1),glm::radians(degrees),axis));
}
}
/// 부모에 연결한 상자를 만든다.
graphics::EntityId Console::box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent) {
    auto model=graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube,size);
    for(auto& face:model.faces)face.color={1,1,1};
    graphics::Transform3D transform;
    transform.basis=position;
    auto& scene=app_.getScene();
    const auto id=scene.addModel(model,transform,graphics::PivotMode::Preserve).value();
    if(!scene.setParent(id,parent))throw std::runtime_error("Cannot connect tank part.");
    colored_.push_back(id);
    return id;
}
/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    reset();
    std::cout << "Practice 19: arrows move tank; d lower height; t central spin; l upper swap\n"
              << "g guns opposite yaw; p poles opposite pitch; x/X,z/Z camera translation\n"
              << "y/Y camera yaw, r/R camera orbit (repeat pauses); o colors; c reset; q quit\n";
    return true;
}
/// 배치와 조작 상태를 초기화한다.
void Console::reset() {
    auto& scene=app_.getScene();
    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_=glm::vec3(0);
    lowerTime_=centralAngle_=gunAngle_=poleTime_=swapProgress_=0;
    swapDirection_=1;
    cameraX_=cameraZ_=cameraYaw_=cameraOrbit_=0;
    lowerRunning_=centralRunning_=gunRunning_=poleRunning_=swapRunning_=swapStarted_=false;
    colorVersion_=0;
    root_=scene.addGroup().value();
    central_=scene.addGroup().value();
    if(!scene.setParent(central_,root_))throw std::runtime_error("Cannot create tank root.");
    const auto ground=scene.addGroup().value();
    box({12,.1f,12},{0,-.05f,0},ground);
    for(std::size_t i=0;i<2;++i)lower_[i]=box({.65f,.6f,1.8f},{i==0?-.8f:.8f,.3f,0},root_);
    box({2.3f,.25f,1.4f},{0,.125f,0},central_);
    for(std::size_t i=0;i<2;++i) {
        upper_[i]=scene.addGroup().value();
        guns_[i]=scene.addGroup().value();
        poles_[i]=scene.addGroup().value();
        if(!scene.setParent(upper_[i],central_)||!scene.setParent(guns_[i],upper_[i])||!scene.setParent(poles_[i],upper_[i]))
            throw std::runtime_error("Cannot create upper tank hierarchy.");
        box({.65f,.6f,.8f},{0,.3f,0},upper_[i]);
        box({.14f,.14f,1.1f},{0,0,.55f},guns_[i]);
        box({.08f,.85f,.08f},{0,.425f,0},poles_[i]);
    }
    auto& camera=app_.getCamera();
    camera=graphics::Camera{};
    camera.position={-5,4.5f,7};
    camera.target={0,.7f,0};
    camera.orthographic=false;
    app_.getRenderOptions()=graphics::RenderOptions{};
    app_.getRenderOptions().lighting=true;
    recolor();
    apply();
}
/// 물체 색을 다시 지정한다.
void Console::recolor() {
    const std::array<graphics::GlColor,10> palette{
        graphics::GlColor{.3f,.35f,.4f},{.25f,.6f,.3f},{.55f,.35f,.2f},{.2f,.55f,.75f},
        {.7f,.75f,.2f},{.9f,.4f,.15f},{1,.8f,.2f},{.4f,.7f,.8f},{.7f,.25f,.6f},{.6f,.9f,.4f}};
    for(std::size_t i=0;i<colored_.size();++i)
        if(!app_.getScene().setTint(colored_[i],palette[(i+colorVersion_)%palette.size()]))
            throw std::runtime_error("Cannot color tank part.");
}
/// 계산한 변환을 장면에 반영한다.
void Console::apply() {
    auto& scene=app_.getScene();
    if(!scene.setPosition(root_,position_))throw std::runtime_error("Invalid tank position.");
    const float factor=1+.25f*std::sin(lowerTime_),height=.6f*factor;
    graphics::Transform3D center;
    center.basis={0,height,0};
    center.rotation=turn(centralAngle_,{0,1,0});
    if(!scene.setTransform(central_,center))throw std::runtime_error("Invalid central transform.");
    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        graphics::Transform3D low;
        low.basis = { sign * .8f,height * .5f,0 };
        low.scale = { 1,factor,1 };
        graphics::Transform3D upper;
        const float a = 3.14159265f * swapProgress_;
        upper.basis = { sign * .65f * std::cos(a),.25f,sign * .65f * std::sin(a) };
        const glm::vec3 bodyCenter{0, .3f, 0};
        const glm::vec3 gunOffset{0, .25f, .4f};
        graphics::Transform3D gun;
        gun.rotation = turn(sign * gunAngle_, {0, 1, 0});
        gun.basis = bodyCenter
                  + gun.rotation * (gunOffset - bodyCenter);
        graphics::Transform3D pole;
        pole.basis={0,.6f,0};
        pole.rotation=turn(sign*60*std::sin(poleTime_),{1,0,0});
        if(!scene.setTransform(lower_[i],low)||!scene.setTransform(upper_[i],upper)
            ||!scene.setTransform(guns_[i],gun)||!scene.setTransform(poles_[i],pole))
            throw std::runtime_error("Invalid tank part transform.");
    }
}
/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key,int action,int mods) {
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    if(key==GLFW_KEY_X||key==GLFW_KEY_Z) {
        if(action==GLFW_PRESS||action==GLFW_RELEASE) {
            float& direction=key==GLFW_KEY_X?cameraX_:cameraZ_;
            direction=action==GLFW_RELEASE?0:(upper?-1.0f:1.0f);
        }
        return;
    }
    if(key>=0&&key<=GLFW_KEY_LAST) {
        if(action==GLFW_PRESS)pressed_[key]=true;
        else if(action==GLFW_RELEASE)pressed_[key]=false;
    }
    if(action!=GLFW_PRESS)return;
    const float direction=upper?1.0f:-1.0f;
    switch(key) {
    case GLFW_KEY_D:
        lowerRunning_=!lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_=!centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_=!gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_=!poleRunning_;
        break;
    case GLFW_KEY_L:
        if(!swapStarted_){
            swapStarted_=true;
            swapRunning_=true;
        }
        else if(!swapRunning_&&(swapProgress_==0||swapProgress_==1)){
            swapDirection_=-swapDirection_;
            swapRunning_=true;
        }
        else swapRunning_=!swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_=cameraYaw_==direction?0:direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_=cameraOrbit_==direction?0:direction;
        break;
    case GLFW_KEY_O:
        colorVersion_=(colorVersion_+1)%10;
        recolor();
        break;
    case GLFW_KEY_C:
        reset();
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT])-float(pressed_[GLFW_KEY_LEFT]),0,
        float(pressed_[GLFW_KEY_DOWN])-float(pressed_[GLFW_KEY_UP])};
    if(glm::length(movement)>0)position_+=glm::normalize(movement)*dt*1.3f;

    position_.x=std::clamp(position_.x,-3.5f,3.5f);
    position_.z=std::clamp(position_.z,-3.5f,3.5f);
    if(lowerRunning_)lowerTime_=std::fmod(lowerTime_+dt,6.283185307f);
    if(centralRunning_)centralAngle_=std::fmod(centralAngle_+45*dt,360.0f);
    if(gunRunning_)gunAngle_= std::fmod(gunAngle_+60*dt,-360.0f);
    if(poleRunning_)poleTime_=std::fmod(poleTime_+dt,6.283185307f);
    if(swapRunning_) {
        swapProgress_=std::clamp(swapProgress_+swapDirection_*dt*.5f,0.0f,1.0f);
        if(swapProgress_==0||swapProgress_==1)swapRunning_=false;
    }
    auto camera=app_.getCamera();
    const glm::vec3 delta{cameraX_*2*dt,0,cameraZ_*2*dt};
    camera.position+=delta;
    camera.target+=delta;
    if(cameraYaw_!=0) {
        const auto forward=glm::normalize(camera.target-camera.position);
        const auto right=glm::normalize(glm::cross(forward,camera.up));
        const auto localUp=glm::normalize(glm::cross(right,forward));
        const auto rotation=turn(cameraYaw_*35*dt,localUp);
        camera.target=camera.position+rotation*(camera.target-camera.position);
        camera.up=rotation*localUp;
    }
    if(cameraOrbit_!=0) {
        const auto rotation=turn(cameraOrbit_*25*dt,{0,1,0});
        camera.position=rotation*camera.position;
        camera.target=rotation*camera.target;
        camera.up=rotation*camera.up;
    }
    if(graphics::CameraSystem::validCamera(camera))app_.getCamera()=camera;
    apply();
}
/// 포커스가 없으면 지속 입력을 해제한다.
void Console::onFocus(bool focused) {
    if(!focused){
        pressed_.fill(false);
        cameraX_=cameraZ_=0;
    }
}
```

## openGL_3dAssign_19/main.cpp
### before
```cpp
#include "Console.h"
int main() {
    graphics::Application app;
    Console practice(app);
    app.setInitHandler([&](graphics::Application&){
        return practice.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
        practice.onKey(key,action,mods);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt){
        practice.onUpdate(dt);
    });
    app.setFocusHandler([&](graphics::Application&,bool focused){
        practice.onFocus(focused);
    });
    graphics::ApplicationConfig config;
    config.title="OpenGL Practice 19";
    return app.run(config);
}
```
### after
```cpp
#include "Console.h"
/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    graphics::Application app;
    Console practice(app);
    app.setInitHandler([&](graphics::Application&){
        return practice.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
        practice.onKey(key,action,mods);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt){
        practice.onUpdate(dt);
    });
    app.setFocusHandler([&](graphics::Application&,bool focused){
        practice.onFocus(focused);
    });
    graphics::ApplicationConfig config;
    config.title="OpenGL Practice 19";
    return app.run(config);
}
```

## openGL_3dAssign_19/Console.h
### before
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_=0,central_=0;
    std::array<graphics::EntityId,2> lower_{},upper_{},guns_{},poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool,GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_=0,centralAngle_=0,gunAngle_=0,poleTime_=0;
    float swapProgress_=0,swapDirection_=1;
    float cameraX_=0,cameraZ_=0,cameraYaw_=0,cameraOrbit_=0;
    bool lowerRunning_=false,centralRunning_=false,gunRunning_=false,poleRunning_=false;
    bool swapRunning_=false,swapStarted_=false;
    unsigned colorVersion_=0;
    graphics::EntityId box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent);
    void reset();
    void apply();
    void recolor();
public:
    explicit Console(graphics::Application& app):app_(app){}
    bool initScene();
    void onKey(int key,int action,int mods);
    void onUpdate(float dt);
    void onFocus(bool focused);
};
```
### after
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_=0,central_=0;
    std::array<graphics::EntityId,2> lower_{},upper_{},guns_{},poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool,GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_=0,centralAngle_=0,gunAngle_=0,poleTime_=0;
    float swapProgress_=0,swapDirection_=1;
    float cameraX_=0,cameraZ_=0,cameraYaw_=0,cameraOrbit_=0;
    bool lowerRunning_=false,centralRunning_=false,gunRunning_=false,poleRunning_=false;
    bool swapRunning_=false,swapStarted_=false;
    unsigned colorVersion_=0;
    /// 부모에 연결한 상자를 만든다.
    graphics::EntityId box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent);
    /// 배치와 조작 상태를 초기화한다.
    void reset();
    /// 계산한 변환을 장면에 반영한다.
    void apply();
    /// 물체 색을 다시 지정한다.
    void recolor();
public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app):app_(app){}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key,int action,int mods);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused);
};
```

## openGL_3dAssign_20/Console.cpp
### before
```cpp
#include "Console.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {
constexpr float beltHeight = 1.0f;
constexpr float boxSize = .85f;
constexpr double beltSpeed = 1.25;
constexpr std::size_t stripeCount = 30;
constexpr float cornerRadius = .8f;
constexpr double halfPi = 1.57079632679489661923;
const glm::vec3 conveyorStart{-4, beltHeight, -2};
const glm::vec3 cornerPivot{2.2f, beltHeight, -1.2f};
constexpr float conveyorEndZ = 3.0f;
const double conveyorLength = (double(cornerPivot.x) - conveyorStart.x)
    + halfPi * double(cornerRadius) + (double(conveyorEndZ) - cornerPivot.z);

void require(bool success, const char* operation) {
    if (!success) throw std::runtime_error(operation);
}
}

bool Console::initScene() {
    auto& scene = app_.getScene();
    scene.clear();
    stripes_.clear();

    require(graphics::PatternSystem::addPolygon(scene,
        {{-6, 0, -4}, {-6, 0, 5}, {6, 0, 5}, {6, 0, -4}},
        {.25f, .30f, .34f}).has_value(), "Floor creation failed.");
    // Two convex planes meet at an edge: an L is not a single convex polygon.
    require(graphics::PatternSystem::addPolygon(scene,
        {{-4.8f, beltHeight, -2.8f}, {-4.8f, beltHeight, -1.2f},
         {3.8f, beltHeight, -1.2f}, {3.8f, beltHeight, -2.8f}},
        {.09f, .12f, .15f}).has_value(), "Horizontal conveyor plane creation failed.");
    require(graphics::PatternSystem::addPolygon(scene,
        {{2.2f, beltHeight, -1.2f}, {2.2f, beltHeight, 3.8f},
         {3.8f, beltHeight, 3.8f}, {3.8f, beltHeight, -1.2f}},
        {.09f, .12f, .15f}).has_value(), "Vertical conveyor plane creation failed.");

    for (std::size_t i = 0; i < stripeCount; ++i) {
        const auto stripe = graphics::PatternSystem::addBox(scene, {1.4f, .03f, .10f},
            {0, 0, 0}, {.72f, .77f, .80f});
        require(stripe.has_value(), "Conveyor stripe creation failed.");
        stripes_.push_back(*stripe);
    }

    const std::array<graphics::GlColor, 5> colors{{
        {.95f, .30f, .23f}, {.23f, .72f, .96f}, {.98f, .75f, .20f},
        {.38f, .86f, .43f}, {.78f, .44f, .96f}}};
    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const auto box = graphics::PatternSystem::addBox(scene,
            {boxSize, boxSize, boxSize}, {0, 0, 0}, colors[i]);
        require(box.has_value(), "Parcel creation failed.");
        boxes_[i] = *box;
        // Each outward face has its own label frame, so camera orbit keeps digits readable.
        for (int face = 0; face < 4; ++face) {
            const auto label = graphics::PatternSystem::addJoint(scene, *box, {0, 0, 0});
            require(label.has_value(), "Parcel label frame creation failed.");
            require(graphics::PatternSystem::rotateJoint(scene, *label, {0, 1, 0},
                float(face) * 90.0f), "Parcel label frame rotation failed.");
            require(graphics::PatternSystem::addNumber(scene, unsigned(i + 1),
                {-.12f, -.24f, boxSize * .5f + .008f}, .48f,
                {.04f, .04f, .06f}, *label), "Parcel number creation failed.");
        }
    }

    auto& options = app_.getRenderOptions();
    options = graphics::RenderOptions{};
    options.depthTest = true;
    options.lighting = true;
    options.background = {.08f, .10f, .14f, 1};
    resetState();
    std::cout << "실습 20: 택배 물류 센터\n"
        << "b 이동 시작/정지 | +/- 확대/축소 | p 원근 / P 직각\n"
        << "좌/우 중심 Y축 공전 | r 전체 리셋 | q 종료\n";
    return true;
}

void Console::placeMovingObjects() {
    auto& scene = app_.getScene();
    const double travelled = clock_.seconds * beltSpeed;
    const auto place = [&](graphics::EntityId entity, double distance, float height) {
        const auto pose = graphics::PatternSystem::conveyorCornerPose(distance,
            conveyorStart, cornerPivot, cornerRadius, conveyorEndZ);
        require(pose.has_value(), "Conveyor corner pose sampling failed.");
        const auto* component = scene.getComponent(entity);
        require(component != nullptr, "Conveyor moving entity is missing.");
        auto transform = component->transform;
        transform.basis = pose->position + glm::vec3(0, height, 0);
        transform.rotation = glm::mat3(glm::rotate(glm::mat4(1),
            glm::radians(pose->yawDegrees), glm::vec3(0, 1, 0)));
        require(scene.setTransform(entity, transform), "Conveyor moving entity transform failed.");
    };
    for (std::size_t i = 0; i < stripes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(stripes_.size());
        place(stripes_[i], travelled + offset, .015f);
    }
    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(boxes_.size());
        place(boxes_[i], travelled + offset, .03f + boxSize * .5f);
    }
}

void Console::resetState() {
    clock_.reset();
    clock_.playing = false;
    leftHeld_ = false;
    rightHeld_ = false;
    auto& camera = app_.getCamera();
    camera = graphics::Camera{};
    camera.position = {11, 10, 13};
    camera.target = {0, .7f, .3f};
    camera.up = {0, 1, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 7.5f;
    camera.nearPlane = .1f;
    camera.farPlane = 120;
    require(graphics::CameraSystem::validCamera(camera), "Reset camera is invalid.");
    placeMovingObjects();
}

void Console::zoom(float factor) {
    auto& camera = app_.getCamera();
    const float current = camera.orthographic ? camera.orthographicHalfExtent
        : glm::length(camera.position - camera.target);
    // Keep useful scene visibility and stay within the far clipping distance.
    const float next = std::clamp(current * factor, 2.0f, camera.orthographic ? 35.0f : 60.0f);
    require(graphics::PatternSystem::zoomCamera(camera, next / current), "Camera zoom failed.");
}

void Console::onKey(int key, int action, int mods) {
    if (key == GLFW_KEY_LEFT) leftHeld_ = action != GLFW_RELEASE;
    if (key == GLFW_KEY_RIGHT) rightHeld_ = action != GLFW_RELEASE;
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) zoom(.9f);
    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) zoom(1.0f / .9f);
    if (action != GLFW_PRESS) return;
    switch (key) {
    case GLFW_KEY_B:
        clock_.playing = !clock_.playing;
        break;
    case GLFW_KEY_P:
        // Shift and Caps Lock together produce lowercase p.
        app_.getCamera().orthographic = ((mods & GLFW_MOD_SHIFT) != 0)
            != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
        break;
    case GLFW_KEY_R:
        resetState();
        break;
    case GLFW_KEY_Q:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void Console::onFocus(bool focused) {
    if (!focused) {
        leftHeld_ = false;
        rightHeld_ = false;
    }
}

void Console::onUpdate(float dt) {
    require(clock_.advance(dt), "Conveyor clock update failed.");
    placeMovingObjects();
    const int direction = int(rightHeld_) - int(leftHeld_);
    if (direction != 0) {
        const graphics::AxisLine axis{app_.getCamera().target, {0, 1, 0}};
        require(graphics::PatternSystem::orbitCamera(app_.getCamera(), axis,
            float(direction) * 65.0f * dt), "Camera orbit failed.");
    }
    // Application::run advances Scene motions; do not update them a second time here.
}
```
### after
```cpp
#include "Console.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {
constexpr float beltHeight = 1.0f;
constexpr float boxSize = .85f;
constexpr double beltSpeed = 1.25;
constexpr std::size_t stripeCount = 30;
constexpr float cornerRadius = .8f;
constexpr double halfPi = 1.57079632679489661923;
const glm::vec3 conveyorStart{-4, beltHeight, -2};
const glm::vec3 cornerPivot{2.2f, beltHeight, -1.2f};
constexpr float conveyorEndZ = 3.0f;
const double conveyorLength = (double(cornerPivot.x) - conveyorStart.x)
    + halfPi * double(cornerRadius) + (double(conveyorEndZ) - cornerPivot.z);

/// 실패한 작업을 예외로 전달한다.
void require(bool success, const char* operation) {
    if (!success) throw std::runtime_error(operation);
}
}

/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    auto& scene = app_.getScene();
    scene.clear();
    stripes_.clear();

    require(graphics::PatternSystem::addPolygon(scene,
        {{-6, 0, -4}, {-6, 0, 5}, {6, 0, 5}, {6, 0, -4}},
        {.25f, .30f, .34f}).has_value(), "Floor creation failed.");

    require(graphics::PatternSystem::addPolygon(scene,
        {{-4.8f, beltHeight, -2.8f}, {-4.8f, beltHeight, -1.2f},
         {3.8f, beltHeight, -1.2f}, {3.8f, beltHeight, -2.8f}},
        {.09f, .12f, .15f}).has_value(), "Horizontal conveyor plane creation failed.");
    require(graphics::PatternSystem::addPolygon(scene,
        {{2.2f, beltHeight, -1.2f}, {2.2f, beltHeight, 3.8f},
         {3.8f, beltHeight, 3.8f}, {3.8f, beltHeight, -1.2f}},
        {.09f, .12f, .15f}).has_value(), "Vertical conveyor plane creation failed.");

    for (std::size_t i = 0; i < stripeCount; ++i) {
        const auto stripe = graphics::PatternSystem::addBox(scene, {1.4f, .03f, .10f},
            {0, 0, 0}, {.72f, .77f, .80f});
        require(stripe.has_value(), "Conveyor stripe creation failed.");
        stripes_.push_back(*stripe);
    }

    const std::array<graphics::GlColor, 5> colors{{
        {.95f, .30f, .23f}, {.23f, .72f, .96f}, {.98f, .75f, .20f},
        {.38f, .86f, .43f}, {.78f, .44f, .96f}}};
    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const auto box = graphics::PatternSystem::addBox(scene,
            {boxSize, boxSize, boxSize}, {0, 0, 0}, colors[i]);
        require(box.has_value(), "Parcel creation failed.");
        boxes_[i] = *box;

        for (int face = 0; face < 4; ++face) {
            const auto label = graphics::PatternSystem::addJoint(scene, *box, {0, 0, 0});
            require(label.has_value(), "Parcel label frame creation failed.");
            require(graphics::PatternSystem::rotateJoint(scene, *label, {0, 1, 0},
                float(face) * 90.0f), "Parcel label frame rotation failed.");
            require(graphics::PatternSystem::addNumber(scene, unsigned(i + 1),
                {-.12f, -.24f, boxSize * .5f + .008f}, .48f,
                {.04f, .04f, .06f}, *label), "Parcel number creation failed.");
        }
    }

    auto& options = app_.getRenderOptions();
    options = graphics::RenderOptions{};
    options.depthTest = true;
    options.lighting = true;
    options.background = {.08f, .10f, .14f, 1};
    resetState();
    std::cout << "실습 20: 택배 물류 센터\n"
        << "b 이동 시작/정지 | +/- 확대/축소 | p 원근 / P 직각\n"
        << "좌/우 중심 Y축 공전 | r 전체 리셋 | q 종료\n";
    return true;
}

/// 이동 거리로 상자와 벨트 표시를 배치한다.
void Console::placeMovingObjects() {
    auto& scene = app_.getScene();
    const double travelled = clock_.seconds * beltSpeed;
    const auto place = [&](graphics::EntityId entity, double distance, float height) {
        const auto pose = graphics::PatternSystem::conveyorCornerPose(distance,
            conveyorStart, cornerPivot, cornerRadius, conveyorEndZ);
        require(pose.has_value(), "Conveyor corner pose sampling failed.");
        const auto* component = scene.getComponent(entity);
        require(component != nullptr, "Conveyor moving entity is missing.");
        auto transform = component->transform;
        transform.basis = pose->position + glm::vec3(0, height, 0);
        transform.rotation = glm::mat3(glm::rotate(glm::mat4(1),
            glm::radians(pose->yawDegrees), glm::vec3(0, 1, 0)));
        require(scene.setTransform(entity, transform), "Conveyor moving entity transform failed.");
    };
    for (std::size_t i = 0; i < stripes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(stripes_.size());
        place(stripes_[i], travelled + offset, .015f);
    }
    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(boxes_.size());
        place(boxes_[i], travelled + offset, .03f + boxSize * .5f);
    }
}

/// 이동 시간과 카메라 상태를 초기화한다.
void Console::resetState() {
    clock_.reset();
    clock_.playing = false;
    leftHeld_ = false;
    rightHeld_ = false;
    auto& camera = app_.getCamera();
    camera = graphics::Camera{};
    camera.position = {11, 10, 13};
    camera.target = {0, .7f, .3f};
    camera.up = {0, 1, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 7.5f;
    camera.nearPlane = .1f;
    camera.farPlane = 120;
    require(graphics::CameraSystem::validCamera(camera), "Reset camera is invalid.");
    placeMovingObjects();
}

/// 카메라 배율을 변경한다.
void Console::zoom(float factor) {
    auto& camera = app_.getCamera();
    const float current = camera.orthographic ? camera.orthographicHalfExtent
        : glm::length(camera.position - camera.target);

    const float next = std::clamp(current * factor, 2.0f, camera.orthographic ? 35.0f : 60.0f);
    require(graphics::PatternSystem::zoomCamera(camera, next / current), "Camera zoom failed.");
}

/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key, int action, int mods) {
    if (key == GLFW_KEY_LEFT) leftHeld_ = action != GLFW_RELEASE;
    if (key == GLFW_KEY_RIGHT) rightHeld_ = action != GLFW_RELEASE;
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) zoom(.9f);
    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) zoom(1.0f / .9f);
    if (action != GLFW_PRESS) return;
    switch (key) {
    case GLFW_KEY_B:
        clock_.playing = !clock_.playing;
        break;
    case GLFW_KEY_P:

        app_.getCamera().orthographic = ((mods & GLFW_MOD_SHIFT) != 0)
            != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
        break;
    case GLFW_KEY_R:
        resetState();
        break;
    case GLFW_KEY_Q:
        app_.requestClose();
        break;
    default:
        break;
    }
}

/// 포커스가 없으면 지속 입력을 해제한다.
void Console::onFocus(bool focused) {
    if (!focused) {
        leftHeld_ = false;
        rightHeld_ = false;
    }
}

/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    require(clock_.advance(dt), "Conveyor clock update failed.");
    placeMovingObjects();
    const int direction = int(rightHeld_) - int(leftHeld_);
    if (direction != 0) {
        const graphics::AxisLine axis{app_.getCamera().target, {0, 1, 0}};
        require(graphics::PatternSystem::orbitCamera(app_.getCamera(), axis,
            float(direction) * 65.0f * dt), "Camera orbit failed.");
    }

}
```

## openGL_3dAssign_20/main.cpp
### before
```cpp
#include "Console.h"
#include <exception>
#include <iostream>

int main() {
    try {
        graphics::Application app;
        Console conveyor(app);
        app.setInitHandler([&](graphics::Application&) { return conveyor.initScene(); });
        app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
            conveyor.onKey(key, action, mods);
        });
        app.setFocusHandler([&](graphics::Application&, bool focused) {
            conveyor.onFocus(focused);
        });
        app.setUpdateHandler([&](graphics::Application&, float dt) { conveyor.onUpdate(dt); });
        graphics::ApplicationConfig config;
        config.title = "Practice 20 - Parcel conveyor";
        config.width = 1100;
        config.height = 800;
        config.projectRoot = ".";
        config.runtimeDirectory = "framework";
        const int result = app.run(config);
        if (result != 0) std::cerr << "Practice 20 failed. Read the preceding framework diagnostic.\n";
        return result;
    } catch (const std::exception& error) {
        std::cerr << "Practice 20: " << error.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Practice 20: unknown exception.\n";
        return 1;
    }
}
```
### after
```cpp
#include "Console.h"
#include <exception>
#include <iostream>

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    try {
        graphics::Application app;
        Console conveyor(app);
        app.setInitHandler([&](graphics::Application&) { return conveyor.initScene(); });
        app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
            conveyor.onKey(key, action, mods);
        });
        app.setFocusHandler([&](graphics::Application&, bool focused) {
            conveyor.onFocus(focused);
        });
        app.setUpdateHandler([&](graphics::Application&, float dt) { conveyor.onUpdate(dt); });
        graphics::ApplicationConfig config;
        config.title = "Practice 20 - Parcel conveyor";
        config.width = 1100;
        config.height = 800;
        config.projectRoot = ".";
        config.runtimeDirectory = "framework";
        const int result = app.run(config);
        if (result != 0) std::cerr << "Practice 20 failed. Read the preceding framework diagnostic.\n";
        return result;
    } catch (const std::exception& error) {
        std::cerr << "Practice 20: " << error.what() << '\n';
        return 1;
    } catch (...) {
        std::cerr << "Practice 20: unknown exception.\n";
        return 1;
    }
}
```

## openGL_3dAssign_20/Console.h
### before
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::AnimationClock clock_;
    std::array<graphics::EntityId, 5> boxes_{};
    std::vector<graphics::EntityId> stripes_;
    bool leftHeld_ = false;
    bool rightHeld_ = false;
    void placeMovingObjects();
    void resetState();
    void zoom(float factor);

public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action, int mods);
    void onFocus(bool focused);
    void onUpdate(float dt);
};
```
### after
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::AnimationClock clock_;
    std::array<graphics::EntityId, 5> boxes_{};
    std::vector<graphics::EntityId> stripes_;
    bool leftHeld_ = false;
    bool rightHeld_ = false;
    /// 이동 거리로 상자와 벨트 표시를 배치한다.
    void placeMovingObjects();
    /// 이동 시간과 카메라 상태를 초기화한다.
    void resetState();
    /// 카메라 배율을 변경한다.
    void zoom(float factor);

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app) : app_(app) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods);
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
};
```

## openGL_3dAssign_21/Console.cpp
### before
```cpp
#include "Console.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

void Console::require(bool succeeded, const char* operation) const {
    if (!succeeded) throw std::runtime_error(operation);
}

glm::mat4 Console::roomWorld() const {
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));
    return glm::rotate(spin, roomAngle_, glm::vec3(0, 0, 1));
}

void Console::releaseBody(BodyObject& object) const {
    const auto matrix = roomWorld();
    object.body.worldPosition = object.body.position(matrix);
    object.body.worldVelocity = glm::mat3(matrix) * object.body.localVelocity;
    object.releasedRotation = glm::mat3(matrix);
    object.body.released = true;
}

bool Console::initScene() {
    auto& scene = app_.getScene();
    scene.clear();
    objects_.clear();
    ballCount_ = 0;
    roomAngle_ = roomSpinAngle_ = pendingRotation_ = 0;
    floorOpen_ = dragging_ = roomSpinning_ = false;
    const auto room = graphics::PatternSystem::addRoom(scene, roomBounds_, true);
    if (!room) return false;
    room_ = *room;
    const graphics::GlColor colors[] = {
        {.6f, .6f, .6f}, {.35f, .5f, .8f}, {.85f, .4f, .4f},
        {.35f, .75f, .5f}, {.75f, .5f, .8f}, {.8f, .7f, .4f}
    };
    for (std::size_t face = 0; face < 6; ++face) {
        if (!scene.setFaceVertexColors(room_, face,
                std::vector<graphics::GlColor>(4, colors[face]))) return false;
    }

    // Different sizes share local X=0; local Z determines their initial row.
    const glm::vec3 sizes[] = {{.65f, .65f, .65f}, {.9f, .9f, .9f}, {1.2f, 1.2f, 1.2f}};
    const graphics::GlColor boxColors[] = {{1, .45f, .15f}, {.2f, .85f, .95f}, {.85f, .95f, .3f}};
    for (unsigned i = 0; i < 3; ++i) {
        BodyObject box;
        box.body.halfExtent = sizes[i] * .5f;
        box.body.localPosition = {0, roomBounds_.minimum.y + box.body.halfExtent.y,
                                 -2.3f + 2.3f * i};
        const auto entity = graphics::PatternSystem::addBox(
            scene, sizes[i], box.body.localPosition, boxColors[i]);
        if (!entity) return false;
        box.entity = *entity;
        objects_.push_back(box);
    }
    if (!addBall()) return false;

    auto& camera = app_.getCamera();
    camera = graphics::Camera{};
    camera.position = {0, .8f, 13};
    camera.target = {0, .25f, 0};
    camera.farPlane = 200;
    camera.orthographic = false;
    auto& render = app_.getRenderOptions();
    render.depthTest = true;
    render.cull = graphics::CullMode::Back;
    render.lighting = true;
    render.lightDirection = {.5f, 1, 2};
    printHelp();
    return syncScene();
}

bool Console::addBall() {
    if (ballCount_ >= 5) {
        std::cout << "공은 최대 5개입니다. R로 초기화할 수 있습니다.\n";
        return true;
    }
    const float radius = .24f;
    std::optional<glm::vec3> freePosition;
    // A finite search checks the complete sphere extent against every live body.
    for (int y = 0; y < 4 && !freePosition; ++y) {
        for (int z = 0; z < 5 && !freePosition; ++z) {
            for (int x = 0; x < 5 && !freePosition; ++x) {
                const glm::vec3 p(-2.2f + x * 1.1f, .1f + y * .5f, -2.6f + z * 1.3f);
                const graphics::Bounds3 candidate{p - glm::vec3(radius), p + glm::vec3(radius)};
                if (!roomBounds_.contains(p, glm::vec3(radius))) continue;
                bool occupied = false;
                for (const auto& object : objects_) {
                    if (object.body.released) continue;
                    const graphics::Bounds3 bodyBounds{
                        object.body.localPosition - object.body.halfExtent,
                        object.body.localPosition + object.body.halfExtent};
                    if (candidate.overlaps(bodyBounds)) { occupied = true; break; }
                }
                if (!occupied) freePosition = p;
            }
        }
    }
    if (!freePosition) {
        std::cout << "공을 생성할 빈 위치가 없습니다. 잠시 후 B를 누르세요.\n";
        return true;
    }
    auto& scene = app_.getScene();
    const auto sphere = scene.addSphere(radius, *freePosition);
    if (!sphere || !scene.setTint(*sphere, {1, .85f - .12f * ballCount_, .15f + .15f * ballCount_})
        || !scene.setFaceCullMode(*sphere, 0, graphics::CullMode::None)) return false;
    BodyObject ball;
    ball.entity = *sphere;
    ball.elastic = true;
    ball.body.localPosition = *freePosition;
    ball.body.halfExtent = glm::vec3(radius);
    ball.body.localVelocity = {1.5f + .2f * ballCount_, -1.4f - .12f * ballCount_,
                              ballCount_ % 2 ? -1.8f : 1.8f};
    if (floorOpen_) releaseBody(ball);
    objects_.push_back(ball);
    ++ballCount_;
    std::cout << "공 생성: " << ballCount_ << "/5\n";
    return true;
}

bool Console::syncScene() {
    auto& scene = app_.getScene();
    const auto matrix = roomWorld();
    graphics::Transform3D roomTransform;
    roomTransform.rotation = glm::mat3(matrix);
    // addRoom's geometry is centered; its center must rotate about the same origin as physics.
    roomTransform.basis = glm::vec3(matrix * glm::vec4(
        (roomBounds_.minimum + roomBounds_.maximum) * .5f, 1));
    if (!scene.setTransform(room_, roomTransform)
        || !scene.setFaceVisible(room_, 5, !floorOpen_)) return false;
    for (const auto& object : objects_) {
        graphics::Transform3D transform;
        transform.basis = object.body.position(matrix);
        if (!object.elastic)
            transform.rotation = object.body.released ? object.releasedRotation : glm::mat3(matrix);
        if (!scene.setTransform(object.entity, transform)) return false;
    }
    return true;
}

void Console::onKey(int key, int action, int mods) {
    if (!focused_ || (action != GLFW_PRESS && action != GLFW_REPEAT)) return;
    const bool uppercase = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    if (key == GLFW_KEY_Z) {
        require(graphics::PatternSystem::moveCamera(app_.getCamera(),
            {0, 0, uppercase ? -.35f : .35f}), "카메라 Z 이동 실패");
    } else if (key == GLFW_KEY_Y) {
        require(graphics::PatternSystem::yawCamera(app_.getCamera(), uppercase ? 4.0f : -4.0f),
                "카메라 로컬 Y 자전 실패");
    } else if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_B) {
            require(addBall() && syncScene(), "공 추가 또는 표시변환 실패");
        } else if (key == GLFW_KEY_E || key == GLFW_KEY_F) {
            floorOpen_ = !floorOpen_;
            if (floorOpen_) {
                for (auto& object : objects_)
                    if (!object.body.released) releaseBody(object);
            }
            require(syncScene(), "바닥 가시성 변경 실패");
            std::cout << (floorOpen_ ? "바닥 열림: 모든 내부 공과 상자가 즉시 중력 낙하\n"
                                     : "바닥 닫힘: 이미 이탈한 객체는 계속 낙하\n");
        } else if (key == GLFW_KEY_V) {
            roomSpinning_ = !roomSpinning_;
            std::cout << (roomSpinning_ ? "방 Y축 자전 시작\n" : "방 Y축 자전 정지\n");
        } else if (key == GLFW_KEY_R) {
            require(initScene(), "장면 초기화 실패");
        } else if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
            app_.requestClose();
        } else if (key == GLFW_KEY_H) {
            printHelp();
        }
    }
}

void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (event.action == GLFW_RELEASE) { dragging_ = false; return; }
    if (event.action == GLFW_PRESS && focused_) {
        dragging_ = true;
        previousMouseX_ = event.windowX;
    }
}

void Console::onCursor(double x, double) {
    if (!dragging_ || !focused_) return;
    const float delta = glm::radians(float((previousMouseX_ - x) * .3));
    previousMouseX_ = x;
    if (!std::isfinite(delta)) return;
    // Positive Z rotation is counterclockwise when looking through the front opening.
    roomAngle_ = std::remainder(roomAngle_ + delta, glm::radians(360.0f));
    pendingRotation_ += delta;
}

void Console::onFocus(bool focused) {
    focused_ = focused;
    dragging_ = false;
    pendingRotation_ = 0;
}

void Console::onUpdate(float dt) {
    if (!focused_) return;
    const float spinSpeed = roomSpinning_ ? glm::radians(30.0f) : 0;
    roomSpinAngle_ = std::remainder(roomSpinAngle_ + spinSpeed * dt, glm::radians(360.0f));
    const auto matrix = roomWorld();
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));
    const glm::vec3 angularVelocity = glm::vec3(0, spinSpeed, 0)
        + glm::mat3(spin) * glm::vec3(0, 0, dt > 0 ? pendingRotation_ / dt : 0);
    if (dt > 0) pendingRotation_ = 0;
    for (auto& object : objects_) {
        const bool wasReleased = object.body.released;
        require(graphics::CollisionSystem::stepRoomBody(object.body, roomBounds_, matrix,
            dt, floorOpen_, object.elastic, {0, -9.8f, 0}, angularVelocity),
            "실내 반사/미끄럼/낙하 계산 실패");
        if (!wasReleased && object.body.released) object.releasedRotation = glm::mat3(matrix);
    }
    require(syncScene(), "장면 표시변환 실패");
}

void Console::printHelp() const {
    std::cout << "실습 21: 실내 공 반사, 기울어진 바닥의 상자, 바닥 개방\n"
              << "z/Z: 카메라 월드 Z +/-, y/Y: 화면 로컬 Y 시계/반시계 자전\n"
              << "Shift XOR Caps Lock으로 소문자/대문자 구분. 카메라 키는 반복 허용.\n"
              << "B: 공 추가(초기 1개, 최대 5개), e/F: 바닥 열기/닫기\n"
              << "바닥 개방 즉시 모든 공/상자 중력 낙하. 개방 중 새 공도 낙하.\n"
              << "v: 방 Y축 30도/초 자전 시작/정지, 내부 벽 Front 컬링 유지\n"
              << "왼쪽 버튼 좌우 드래그: 공간 Z축 회전, 기울기에 따라 상자 미끄럼\n"
              << "R: 전체 초기화, H: 도움말, Q 또는 Esc: 종료\n"
              << "B/e/F/v/R/H/Q는 누른 순간 한 번만 적용. 포커스 상실 시 드래그 해제.\n";
}
```
### after
```cpp
#include "Console.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

/// 실패한 작업을 예외로 전달한다.
void Console::require(bool succeeded, const char* operation) const {
    if (!succeeded) throw std::runtime_error(operation);
}

/// 방의 월드 회전 행렬을 계산한다.
glm::mat4 Console::roomWorld() const {
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));
    return glm::rotate(spin, roomAngle_, glm::vec3(0, 0, 1));
}

/// 방 안의 물체를 월드 낙하 상태로 전환한다.
void Console::releaseBody(BodyObject& object) const {
    const auto matrix = roomWorld();
    object.body.worldPosition = object.body.position(matrix);
    object.body.worldVelocity = glm::mat3(matrix) * object.body.localVelocity;
    object.releasedRotation = glm::mat3(matrix);
    object.body.released = true;
}

/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    auto& scene = app_.getScene();
    scene.clear();
    objects_.clear();
    ballCount_ = 0;
    roomAngle_ = roomSpinAngle_ = pendingRotation_ = 0;
    floorOpen_ = dragging_ = roomSpinning_ = false;
    const auto room = graphics::PatternSystem::addRoom(scene, roomBounds_, true);
    if (!room) return false;
    room_ = *room;
    const graphics::GlColor colors[] = {
        {.6f, .6f, .6f}, {.35f, .5f, .8f}, {.85f, .4f, .4f},
        {.35f, .75f, .5f}, {.75f, .5f, .8f}, {.8f, .7f, .4f}
    };
    for (std::size_t face = 0; face < 6; ++face) {
        if (!scene.setFaceVertexColors(room_, face,
                std::vector<graphics::GlColor>(4, colors[face]))) return false;
    }

    const glm::vec3 sizes[] = {{.65f, .65f, .65f}, {.9f, .9f, .9f}, {1.2f, 1.2f, 1.2f}};
    const graphics::GlColor boxColors[] = {{1, .45f, .15f}, {.2f, .85f, .95f}, {.85f, .95f, .3f}};
    for (unsigned i = 0; i < 3; ++i) {
        BodyObject box;
        box.body.halfExtent = sizes[i] * .5f;
        box.body.localPosition = {0, roomBounds_.minimum.y + box.body.halfExtent.y,
                                 -2.3f + 2.3f * i};
        const auto entity = graphics::PatternSystem::addBox(
            scene, sizes[i], box.body.localPosition, boxColors[i]);
        if (!entity) return false;
        box.entity = *entity;
        objects_.push_back(box);
    }
    if (!addBall()) return false;

    auto& camera = app_.getCamera();
    camera = graphics::Camera{};
    camera.position = {0, .8f, 13};
    camera.target = {0, .25f, 0};
    camera.farPlane = 200;
    camera.orthographic = false;
    auto& render = app_.getRenderOptions();
    render.depthTest = true;
    render.cull = graphics::CullMode::Back;
    render.lighting = true;
    render.lightDirection = {.5f, 1, 2};
    printHelp();
    return syncScene();
}

/// 허용 범위에 공을 추가한다.
bool Console::addBall() {
    if (ballCount_ >= 5) {
        std::cout << "공은 최대 5개입니다. R로 초기화할 수 있습니다.\n";
        return true;
    }
    const float radius = .24f;
    std::optional<glm::vec3> freePosition;

    for (int y = 0; y < 4 && !freePosition; ++y) {
        for (int z = 0; z < 5 && !freePosition; ++z) {
            for (int x = 0; x < 5 && !freePosition; ++x) {
                const glm::vec3 p(-2.2f + x * 1.1f, .1f + y * .5f, -2.6f + z * 1.3f);
                const graphics::Bounds3 candidate{p - glm::vec3(radius), p + glm::vec3(radius)};
                if (!roomBounds_.contains(p, glm::vec3(radius))) continue;
                bool occupied = false;
                for (const auto& object : objects_) {
                    if (object.body.released) continue;
                    const graphics::Bounds3 bodyBounds{
                        object.body.localPosition - object.body.halfExtent,
                        object.body.localPosition + object.body.halfExtent};
                    if (candidate.overlaps(bodyBounds)) { occupied = true; break; }
                }
                if (!occupied) freePosition = p;
            }
        }
    }
    if (!freePosition) {
        std::cout << "공을 생성할 빈 위치가 없습니다. 잠시 후 B를 누르세요.\n";
        return true;
    }
    auto& scene = app_.getScene();
    const auto sphere = scene.addSphere(radius, *freePosition);
    if (!sphere || !scene.setTint(*sphere, {1, .85f - .12f * ballCount_, .15f + .15f * ballCount_})
        || !scene.setFaceCullMode(*sphere, 0, graphics::CullMode::None)) return false;
    BodyObject ball;
    ball.entity = *sphere;
    ball.elastic = true;
    ball.body.localPosition = *freePosition;
    ball.body.halfExtent = glm::vec3(radius);
    ball.body.localVelocity = {1.5f + .2f * ballCount_, -1.4f - .12f * ballCount_,
                              ballCount_ % 2 ? -1.8f : 1.8f};
    if (floorOpen_) releaseBody(ball);
    objects_.push_back(ball);
    ++ballCount_;
    std::cout << "공 생성: " << ballCount_ << "/5\n";
    return true;
}

/// 물리 상태를 표시 객체에 반영한다.
bool Console::syncScene() {
    auto& scene = app_.getScene();
    const auto matrix = roomWorld();
    graphics::Transform3D roomTransform;
    roomTransform.rotation = glm::mat3(matrix);

    roomTransform.basis = glm::vec3(matrix * glm::vec4(
        (roomBounds_.minimum + roomBounds_.maximum) * .5f, 1));
    if (!scene.setTransform(room_, roomTransform)
        || !scene.setFaceVisible(room_, 5, !floorOpen_)) return false;
    for (const auto& object : objects_) {
        graphics::Transform3D transform;
        transform.basis = object.body.position(matrix);
        if (!object.elastic)
            transform.rotation = object.body.released ? object.releasedRotation : glm::mat3(matrix);
        if (!scene.setTransform(object.entity, transform)) return false;
    }
    return true;
}

/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key, int action, int mods) {
    if (!focused_ || (action != GLFW_PRESS && action != GLFW_REPEAT)) return;
    const bool uppercase = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    if (key == GLFW_KEY_Z) {
        require(graphics::PatternSystem::moveCamera(app_.getCamera(),
            {0, 0, uppercase ? -.35f : .35f}), "카메라 Z 이동 실패");
    } else if (key == GLFW_KEY_Y) {
        require(graphics::PatternSystem::yawCamera(app_.getCamera(), uppercase ? 4.0f : -4.0f),
                "카메라 로컬 Y 자전 실패");
    } else if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_B) {
            require(addBall() && syncScene(), "공 추가 또는 표시변환 실패");
        } else if (key == GLFW_KEY_E || key == GLFW_KEY_F) {
            floorOpen_ = !floorOpen_;
            if (floorOpen_) {
                for (auto& object : objects_)
                    if (!object.body.released) releaseBody(object);
            }
            require(syncScene(), "바닥 가시성 변경 실패");
            std::cout << (floorOpen_ ? "바닥 열림: 모든 내부 공과 상자가 즉시 중력 낙하\n"
                                     : "바닥 닫힘: 이미 이탈한 객체는 계속 낙하\n");
        } else if (key == GLFW_KEY_V) {
            roomSpinning_ = !roomSpinning_;
            std::cout << (roomSpinning_ ? "방 Y축 자전 시작\n" : "방 Y축 자전 정지\n");
        } else if (key == GLFW_KEY_R) {
            require(initScene(), "장면 초기화 실패");
        } else if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
            app_.requestClose();
        } else if (key == GLFW_KEY_H) {
            printHelp();
        }
    }
}

/// 마우스 버튼 입력을 처리한다.
void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT) return;
    if (event.action == GLFW_RELEASE) { dragging_ = false; return; }
    if (event.action == GLFW_PRESS && focused_) {
        dragging_ = true;
        previousMouseX_ = event.windowX;
    }
}

/// 커서 이동으로 조작 상태를 갱신한다.
void Console::onCursor(double x, double) {
    if (!dragging_ || !focused_) return;
    const float delta = glm::radians(float((previousMouseX_ - x) * .3));
    previousMouseX_ = x;
    if (!std::isfinite(delta)) return;

    roomAngle_ = std::remainder(roomAngle_ + delta, glm::radians(360.0f));
    pendingRotation_ += delta;
}

/// 포커스가 없으면 지속 입력을 해제한다.
void Console::onFocus(bool focused) {
    focused_ = focused;
    dragging_ = false;
    pendingRotation_ = 0;
}

/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    if (!focused_) return;
    const float spinSpeed = roomSpinning_ ? glm::radians(30.0f) : 0;
    roomSpinAngle_ = std::remainder(roomSpinAngle_ + spinSpeed * dt, glm::radians(360.0f));
    const auto matrix = roomWorld();
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));
    const glm::vec3 angularVelocity = glm::vec3(0, spinSpeed, 0)
        + glm::mat3(spin) * glm::vec3(0, 0, dt > 0 ? pendingRotation_ / dt : 0);
    if (dt > 0) pendingRotation_ = 0;
    for (auto& object : objects_) {
        const bool wasReleased = object.body.released;
        require(graphics::CollisionSystem::stepRoomBody(object.body, roomBounds_, matrix,
            dt, floorOpen_, object.elastic, {0, -9.8f, 0}, angularVelocity),
            "실내 반사/미끄럼/낙하 계산 실패");
        if (!wasReleased && object.body.released) object.releasedRotation = glm::mat3(matrix);
    }
    require(syncScene(), "장면 표시변환 실패");
}

/// 조작 방법을 출력한다.
void Console::printHelp() const {
    std::cout << "실습 21: 실내 공 반사, 기울어진 바닥의 상자, 바닥 개방\n"
              << "z/Z: 카메라 월드 Z +/-, y/Y: 화면 로컬 Y 시계/반시계 자전\n"
              << "Shift XOR Caps Lock으로 소문자/대문자 구분. 카메라 키는 반복 허용.\n"
              << "B: 공 추가(초기 1개, 최대 5개), e/F: 바닥 열기/닫기\n"
              << "바닥 개방 즉시 모든 공/상자 중력 낙하. 개방 중 새 공도 낙하.\n"
              << "v: 방 Y축 30도/초 자전 시작/정지, 내부 벽 Front 컬링 유지\n"
              << "왼쪽 버튼 좌우 드래그: 공간 Z축 회전, 기울기에 따라 상자 미끄럼\n"
              << "R: 전체 초기화, H: 도움말, Q 또는 Esc: 종료\n"
              << "B/e/F/v/R/H/Q는 누른 순간 한 번만 적용. 포커스 상실 시 드래그 해제.\n";
}
```

## openGL_3dAssign_21/main.cpp
### before
```cpp
#include "Console.h"
#include <exception>
#include <iostream>

int main() {
    try {
        graphics::Application app;
        Console sample(app);
        app.setInitHandler([&](graphics::Application&) { return sample.initScene(); });
        app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
            sample.onKey(key, action, mods);
        });
        app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
            sample.onMouse(event);
        });
        app.setCursorHandler([&](graphics::Application&, double x, double y) { sample.onCursor(x, y); });
        app.setFocusHandler([&](graphics::Application&, bool focused) { sample.onFocus(focused); });
        app.setUpdateHandler([&](graphics::Application&, float dt) { sample.onUpdate(dt); });
        graphics::ApplicationConfig config;
        config.title = "Practice 21 - Bouncing balls and rotating room";
        config.width = 1100;
        config.height = 800;
        config.diagnostics = {true, false};
        return app.run(config);
    } catch (const std::exception& error) {
        std::cerr << "실습 21 종료 원인: " << error.what() << '\n';
    } catch (...) {
        std::cerr << "실습 21 종료 원인: 표준 예외가 아닌 예외. 앞선 프레임워크 진단을 확인하세요.\n";
    }
    return 1;
}
```
### after
```cpp
#include "Console.h"
#include <exception>
#include <iostream>

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    try {
        graphics::Application app;
        Console sample(app);
        app.setInitHandler([&](graphics::Application&) { return sample.initScene(); });
        app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
            sample.onKey(key, action, mods);
        });
        app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
            sample.onMouse(event);
        });
        app.setCursorHandler([&](graphics::Application&, double x, double y) { sample.onCursor(x, y); });
        app.setFocusHandler([&](graphics::Application&, bool focused) { sample.onFocus(focused); });
        app.setUpdateHandler([&](graphics::Application&, float dt) { sample.onUpdate(dt); });
        graphics::ApplicationConfig config;
        config.title = "Practice 21 - Bouncing balls and rotating room";
        config.width = 1100;
        config.height = 800;
        config.diagnostics = {true, false};
        return app.run(config);
    } catch (const std::exception& error) {
        std::cerr << "실습 21 종료 원인: " << error.what() << '\n';
    } catch (...) {
        std::cerr << "실습 21 종료 원인: 표준 예외가 아닌 예외. 앞선 프레임워크 진단을 확인하세요.\n";
    }
    return 1;
}
```

## openGL_3dAssign_21/Console.h
### before
```cpp
#pragma once
#include "framework/Framework.h"

class Console {
    struct BodyObject {
        graphics::EntityId entity = 0;
        graphics::RoomBody body;
        bool elastic = false;
        glm::mat3 releasedRotation{1};
    };
    graphics::Application& app_;
    const graphics::Bounds3 roomBounds_{{-3, -2, -3.5f}, {3, 2.5f, 3.5f}};
    graphics::EntityId room_ = 0;
    std::vector<BodyObject> objects_;
    unsigned ballCount_ = 0;
    float roomAngle_ = 0;
    float roomSpinAngle_ = 0;
    float pendingRotation_ = 0;
    bool roomSpinning_ = false;
    bool floorOpen_ = false;
    bool dragging_ = false;
    bool focused_ = true;
    double previousMouseX_ = 0;

    glm::mat4 roomWorld() const;
    bool addBall();
    void releaseBody(BodyObject& object) const;
    bool syncScene();
    void require(bool succeeded, const char* operation) const;
    void printHelp() const;

public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action, int mods);
    void onMouse(const graphics::MouseEvent& event);
    void onCursor(double x, double y);
    void onFocus(bool focused);
    void onUpdate(float dt);
};
```
### after
```cpp
#pragma once
#include "framework/Framework.h"

class Console {
    struct BodyObject {
        graphics::EntityId entity = 0;
        graphics::RoomBody body;
        bool elastic = false;
        glm::mat3 releasedRotation{1};
    };
    graphics::Application& app_;
    const graphics::Bounds3 roomBounds_{{-3, -2, -3.5f}, {3, 2.5f, 3.5f}};
    graphics::EntityId room_ = 0;
    std::vector<BodyObject> objects_;
    unsigned ballCount_ = 0;
    float roomAngle_ = 0;
    float roomSpinAngle_ = 0;
    float pendingRotation_ = 0;
    bool roomSpinning_ = false;
    bool floorOpen_ = false;
    bool dragging_ = false;
    bool focused_ = true;
    double previousMouseX_ = 0;

    /// 방의 월드 회전 행렬을 계산한다.
    glm::mat4 roomWorld() const;
    /// 허용 범위에 공을 추가한다.
    bool addBall();
    /// 방 안의 물체를 월드 낙하 상태로 전환한다.
    void releaseBody(BodyObject& object) const;
    /// 물리 상태를 표시 객체에 반영한다.
    bool syncScene();
    /// 실패한 작업을 예외로 전달한다.
    void require(bool succeeded, const char* operation) const;
    /// 조작 방법을 출력한다.
    void printHelp() const;

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app) : app_(app) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods);
    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);
    /// 커서 이동으로 조작 상태를 갱신한다.
    void onCursor(double x, double y);
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
};
```

## openGL_3dAssign_22/Console.cpp
### before
```cpp
#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
graphics::EntityId requireEntity(std::optional<graphics::EntityId> id,const char* reason) {
    if(!id)throw std::runtime_error(reason);
    return *id;
}
}
graphics::EntityId Console::box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent) {
    const auto id=requireEntity(graphics::PatternSystem::addBox(
        app_.getScene(),size,position,{1,1,1},parent),"탱크 상자 생성 실패: 크기·위치·부모 ID를 확인하세요.");
    colored_.push_back(id);
    return id;
}
bool Console::initScene() {
    reset();
    std::cout << "실습 22: 같은 장면을 원근 / XZ 평면도 / XY 정면도로 표시\n"
              << "방향키 이동; d 하부 높이; t 중앙 자전; l 상부 교환\n"
              << "g 포신 반대 회전; p 깃대 반대 기울기; x/X,z/Z 카메라 이동\n"
              << "y/Y 카메라 시선 회전; r/R 공전(재입력 정지); O/o 색 변경; c 초기화; q 종료\n";
    return true;
}
void Console::reset() {
    auto& scene=app_.getScene();
    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_=glm::vec3(0);
    lowerTime_=centralAngle_=gunAngle_=poleTime_=swapProgress_=0;
    swapDirection_=1;
    cameraX_=cameraZ_=cameraYaw_=cameraOrbit_=0;
    lowerRunning_=centralRunning_=gunRunning_=poleRunning_=swapRunning_=swapStarted_=false;
    colorVersion_=0;
    root_=requireEntity(scene.addGroup(),"탱크 루트 생성 실패.");
    central_=requireEntity(graphics::PatternSystem::addJoint(scene,root_,{0,.6f,0}),
        "중앙 관절 생성 실패.");
    const auto ground=requireEntity(scene.addGroup(),"바닥 그룹 생성 실패.");
    box({12,.1f,12},{0,-.05f,0},ground);
    for(std::size_t i=0;i<2;++i)lower_[i]=box({.65f,.6f,1.8f},{i==0?-.8f:.8f,.3f,0},root_);
    box({2.3f,.25f,1.4f},{0,.125f,0},central_);
    for(std::size_t i=0;i<2;++i) {
        const float sign=i==0?-1.0f:1.0f;
        upper_[i]=requireEntity(graphics::PatternSystem::addJoint(scene,central_,{sign*.65f,.25f,0}),
            "상부 관절 생성 실패.");
        guns_[i]=requireEntity(graphics::PatternSystem::addJoint(scene,upper_[i],{0,.3f,0}),
            "포신 중심 피벗 생성 실패.");
        poles_[i]=requireEntity(graphics::PatternSystem::addJoint(scene,upper_[i],{0,.6f,0}),
            "깃대 밑점 피벗 생성 실패.");
        box({.65f,.6f,.8f},{0,.3f,0},upper_[i]);
        // Gun offsets stay relative to the original body-center pivot (0,.3,0).
        box({.14f,.14f,1.1f},{0,-.05f,.95f},guns_[i]);
        box({.08f,.85f,.08f},{0,.425f,0},poles_[i]);
    }
    auto& camera=app_.getCamera();
    camera=graphics::Camera{};
    camera.position={-5,4.5f,7};
    camera.target={0,.7f,0};
    camera.orthographic=false;
    camera.orthographicHalfExtent=6.5f;
    app_.getRenderOptions()=graphics::RenderOptions{};
    app_.getRenderOptions().lighting=true;
    recolor();
    apply();
}
void Console::recolor() {
    const std::array<graphics::GlColor,10> palette{
        graphics::GlColor{.3f,.35f,.4f},{.25f,.6f,.3f},{.55f,.35f,.2f},{.2f,.55f,.75f},
        {.7f,.75f,.2f},{.9f,.4f,.15f},{1,.8f,.2f},{.4f,.7f,.8f},{.7f,.25f,.6f},{.6f,.9f,.4f}};
    for(std::size_t i=0;i<colored_.size();++i)
        if(!app_.getScene().setTint(colored_[i],palette[(i+colorVersion_)%palette.size()]))
            throw std::runtime_error("탱크 부품 색상 적용 실패.");
}
void Console::apply() {
    auto& scene=app_.getScene();
    if(!scene.setPosition(root_,position_))throw std::runtime_error("탱크 위치 적용 실패.");
    const float factor=1+.25f*std::sin(lowerTime_),height=.6f*factor;
    if(!scene.setPosition(central_,{0,height,0})
        ||!graphics::PatternSystem::rotateJoint(scene,central_,{0,1,0},centralAngle_))
        throw std::runtime_error("중앙 관절 변환 적용 실패.");
    for(std::size_t i=0;i<2;++i) {
        const float sign=i==0?-1.0f:1.0f;
        graphics::Transform3D low;
        low.basis={sign*.8f,height*.5f,0};
        low.scale={1,factor,1};
        const float a=3.14159265f*swapProgress_;
        const glm::vec3 upperPosition{sign*.65f*std::cos(a),.25f,sign*.65f*std::sin(a)};
        if(!scene.setTransform(lower_[i],low)||!scene.setPosition(upper_[i],upperPosition)
            ||!graphics::PatternSystem::rotateJoint(scene,guns_[i],{0,1,0},sign*gunAngle_)
            ||!graphics::PatternSystem::rotateJoint(scene,poles_[i],{1,0,0},sign*60*std::sin(poleTime_)))
            throw std::runtime_error("탱크 부품 변환 적용 실패.");
    }
}
void Console::onKey(int key,int action,int mods) {
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    if(key==GLFW_KEY_X||key==GLFW_KEY_Z) {
        if(action==GLFW_PRESS||action==GLFW_RELEASE) {
            float& direction=key==GLFW_KEY_X?cameraX_:cameraZ_;
            direction=action==GLFW_RELEASE?0:(upper?-1.0f:1.0f);
        }
        return;
    }
    if(key>=0&&key<=GLFW_KEY_LAST) {
        if(action==GLFW_PRESS)pressed_[key]=true;
        else if(action==GLFW_RELEASE)pressed_[key]=false;
    }
    if(action!=GLFW_PRESS)return;
    const float direction=upper?1.0f:-1.0f;
    switch(key) {
    case GLFW_KEY_D:
        lowerRunning_=!lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_=!centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_=!gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_=!poleRunning_;
        break;
    case GLFW_KEY_L:
        if(!swapStarted_){
            swapStarted_=true;
            swapRunning_=true;
        }
        else if(!swapRunning_&&(swapProgress_==0||swapProgress_==1)){
            swapDirection_=-swapDirection_;
            swapRunning_=true;
        }
        else swapRunning_=!swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_=cameraYaw_==direction?0:direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_=cameraOrbit_==direction?0:direction;
        break;
    case GLFW_KEY_O:
        colorVersion_=(colorVersion_+1)%10;
        recolor();
        break;
    case GLFW_KEY_C:
        reset();
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}
void Console::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT])-float(pressed_[GLFW_KEY_LEFT]),0,
        float(pressed_[GLFW_KEY_DOWN])-float(pressed_[GLFW_KEY_UP])};
    if(glm::length(movement)>0)position_+=glm::normalize(movement)*dt*1.3f;
    // 12x12 floor minus a 2.5-unit radius covers every animated part's sweep.
    position_.x=std::clamp(position_.x,-3.5f,3.5f);
    position_.z=std::clamp(position_.z,-3.5f,3.5f);
    if(lowerRunning_)lowerTime_=std::fmod(lowerTime_+dt,6.283185307f);
    if(centralRunning_)centralAngle_=std::fmod(centralAngle_+45*dt,360.0f);
    if(gunRunning_)gunAngle_= std::fmod(gunAngle_+60*dt,-360.0f);
    if(poleRunning_)poleTime_=std::fmod(poleTime_+dt,6.283185307f);
    if(swapRunning_) {
        swapProgress_=std::clamp(swapProgress_+swapDirection_*dt*.5f,0.0f,1.0f);
        if(swapProgress_==0||swapProgress_==1)swapRunning_=false;
    }
    auto camera=app_.getCamera();
    const glm::vec3 delta{cameraX_*2*dt,0,cameraZ_*2*dt};
    if(!graphics::PatternSystem::moveCamera(camera,delta)
        ||(cameraYaw_!=0&&!graphics::PatternSystem::yawCamera(camera,cameraYaw_*35*dt))
        ||(cameraOrbit_!=0&&!graphics::PatternSystem::orbitCamera(camera,{{0,0,0},{0,1,0}},cameraOrbit_*25*dt)))
        throw std::runtime_error("카메라 갱신 실패: 위치·시선·회전축을 확인하세요.");
    app_.getCamera()=camera;
    apply();
}
void Console::onFocus(bool focused) {
    if(!focused){
        pressed_.fill(false);
        cameraX_=cameraZ_=0;
    }
}

bool Console::onRender() {
    const auto size=app_.getFramebufferSize();
    if(size[0]<3||size[1]<2)return true;
    const auto passes=graphics::PatternSystem::threeViews(
        app_.getCamera(),size[0],size[1],app_.getRenderOptions());
    if(passes.size()!=3)throw std::runtime_error("세 카메라 뷰 생성 실패: 프레임버퍼 크기와 카메라를 확인하세요.");
    return app_.renderViews(passes);
}
```
### after
```cpp
#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
/// 생성 결과에서 유효한 객체 ID를 얻는다.
graphics::EntityId requireEntity(std::optional<graphics::EntityId> id,const char* reason) {
    if(!id)throw std::runtime_error(reason);
    return *id;
}
}
/// 부모에 연결한 상자를 만든다.
graphics::EntityId Console::box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent) {
    const auto id=requireEntity(graphics::PatternSystem::addBox(
        app_.getScene(),size,position,{1,1,1},parent),"탱크 상자 생성 실패: 크기·위치·부모 ID를 확인하세요.");
    colored_.push_back(id);
    return id;
}
/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    reset();
    std::cout << "실습 22: 같은 장면을 원근 / XZ 평면도 / XY 정면도로 표시\n"
              << "방향키 이동; d 하부 높이; t 중앙 자전; l 상부 교환\n"
              << "g 포신 반대 회전; p 깃대 반대 기울기; x/X,z/Z 카메라 이동\n"
              << "y/Y 카메라 시선 회전; r/R 공전(재입력 정지); O/o 색 변경; c 초기화; q 종료\n";
    return true;
}
/// 배치와 조작 상태를 초기화한다.
void Console::reset() {
    auto& scene=app_.getScene();
    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_=glm::vec3(0);
    lowerTime_=centralAngle_=gunAngle_=poleTime_=swapProgress_=0;
    swapDirection_=1;
    cameraX_=cameraZ_=cameraYaw_=cameraOrbit_=0;
    lowerRunning_=centralRunning_=gunRunning_=poleRunning_=swapRunning_=swapStarted_=false;
    colorVersion_=0;
    root_=requireEntity(scene.addGroup(),"탱크 루트 생성 실패.");
    central_=requireEntity(graphics::PatternSystem::addJoint(scene,root_,{0,.6f,0}),
        "중앙 관절 생성 실패.");
    const auto ground=requireEntity(scene.addGroup(),"바닥 그룹 생성 실패.");
    box({12,.1f,12},{0,-.05f,0},ground);
    for(std::size_t i=0;i<2;++i)lower_[i]=box({.65f,.6f,1.8f},{i==0?-.8f:.8f,.3f,0},root_);
    box({2.3f,.25f,1.4f},{0,.125f,0},central_);
    for(std::size_t i=0;i<2;++i) {
        const float sign=i==0?-1.0f:1.0f;
        upper_[i]=requireEntity(graphics::PatternSystem::addJoint(scene,central_,{sign*.65f,.25f,0}),
            "상부 관절 생성 실패.");
        guns_[i]=requireEntity(graphics::PatternSystem::addJoint(scene,upper_[i],{0,.3f,0}),
            "포신 중심 피벗 생성 실패.");
        poles_[i]=requireEntity(graphics::PatternSystem::addJoint(scene,upper_[i],{0,.6f,0}),
            "깃대 밑점 피벗 생성 실패.");
        box({.65f,.6f,.8f},{0,.3f,0},upper_[i]);

        box({.14f,.14f,1.1f},{0,-.05f,.95f},guns_[i]);
        box({.08f,.85f,.08f},{0,.425f,0},poles_[i]);
    }
    auto& camera=app_.getCamera();
    camera=graphics::Camera{};
    camera.position={-5,4.5f,7};
    camera.target={0,.7f,0};
    camera.orthographic=false;
    camera.orthographicHalfExtent=6.5f;
    app_.getRenderOptions()=graphics::RenderOptions{};
    app_.getRenderOptions().lighting=true;
    recolor();
    apply();
}
/// 물체 색을 다시 지정한다.
void Console::recolor() {
    const std::array<graphics::GlColor,10> palette{
        graphics::GlColor{.3f,.35f,.4f},{.25f,.6f,.3f},{.55f,.35f,.2f},{.2f,.55f,.75f},
        {.7f,.75f,.2f},{.9f,.4f,.15f},{1,.8f,.2f},{.4f,.7f,.8f},{.7f,.25f,.6f},{.6f,.9f,.4f}};
    for(std::size_t i=0;i<colored_.size();++i)
        if(!app_.getScene().setTint(colored_[i],palette[(i+colorVersion_)%palette.size()]))
            throw std::runtime_error("탱크 부품 색상 적용 실패.");
}
/// 계산한 변환을 장면에 반영한다.
void Console::apply() {
    auto& scene=app_.getScene();
    if(!scene.setPosition(root_,position_))throw std::runtime_error("탱크 위치 적용 실패.");
    const float factor=1+.25f*std::sin(lowerTime_),height=.6f*factor;
    if(!scene.setPosition(central_,{0,height,0})
        ||!graphics::PatternSystem::rotateJoint(scene,central_,{0,1,0},centralAngle_))
        throw std::runtime_error("중앙 관절 변환 적용 실패.");
    for(std::size_t i=0;i<2;++i) {
        const float sign=i==0?-1.0f:1.0f;
        graphics::Transform3D low;
        low.basis={sign*.8f,height*.5f,0};
        low.scale={1,factor,1};
        const float a=3.14159265f*swapProgress_;
        const glm::vec3 upperPosition{sign*.65f*std::cos(a),.25f,sign*.65f*std::sin(a)};
        if(!scene.setTransform(lower_[i],low)||!scene.setPosition(upper_[i],upperPosition)
            ||!graphics::PatternSystem::rotateJoint(scene,guns_[i],{0,1,0},sign*gunAngle_)
            ||!graphics::PatternSystem::rotateJoint(scene,poles_[i],{1,0,0},sign*60*std::sin(poleTime_)))
            throw std::runtime_error("탱크 부품 변환 적용 실패.");
    }
}
/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key,int action,int mods) {
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    if(key==GLFW_KEY_X||key==GLFW_KEY_Z) {
        if(action==GLFW_PRESS||action==GLFW_RELEASE) {
            float& direction=key==GLFW_KEY_X?cameraX_:cameraZ_;
            direction=action==GLFW_RELEASE?0:(upper?-1.0f:1.0f);
        }
        return;
    }
    if(key>=0&&key<=GLFW_KEY_LAST) {
        if(action==GLFW_PRESS)pressed_[key]=true;
        else if(action==GLFW_RELEASE)pressed_[key]=false;
    }
    if(action!=GLFW_PRESS)return;
    const float direction=upper?1.0f:-1.0f;
    switch(key) {
    case GLFW_KEY_D:
        lowerRunning_=!lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_=!centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_=!gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_=!poleRunning_;
        break;
    case GLFW_KEY_L:
        if(!swapStarted_){
            swapStarted_=true;
            swapRunning_=true;
        }
        else if(!swapRunning_&&(swapProgress_==0||swapProgress_==1)){
            swapDirection_=-swapDirection_;
            swapRunning_=true;
        }
        else swapRunning_=!swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_=cameraYaw_==direction?0:direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_=cameraOrbit_==direction?0:direction;
        break;
    case GLFW_KEY_O:
        colorVersion_=(colorVersion_+1)%10;
        recolor();
        break;
    case GLFW_KEY_C:
        reset();
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT])-float(pressed_[GLFW_KEY_LEFT]),0,
        float(pressed_[GLFW_KEY_DOWN])-float(pressed_[GLFW_KEY_UP])};
    if(glm::length(movement)>0)position_+=glm::normalize(movement)*dt*1.3f;

    position_.x=std::clamp(position_.x,-3.5f,3.5f);
    position_.z=std::clamp(position_.z,-3.5f,3.5f);
    if(lowerRunning_)lowerTime_=std::fmod(lowerTime_+dt,6.283185307f);
    if(centralRunning_)centralAngle_=std::fmod(centralAngle_+45*dt,360.0f);
    if(gunRunning_)gunAngle_= std::fmod(gunAngle_+60*dt,-360.0f);
    if(poleRunning_)poleTime_=std::fmod(poleTime_+dt,6.283185307f);
    if(swapRunning_) {
        swapProgress_=std::clamp(swapProgress_+swapDirection_*dt*.5f,0.0f,1.0f);
        if(swapProgress_==0||swapProgress_==1)swapRunning_=false;
    }
    auto camera=app_.getCamera();
    const glm::vec3 delta{cameraX_*2*dt,0,cameraZ_*2*dt};
    if(!graphics::PatternSystem::moveCamera(camera,delta)
        ||(cameraYaw_!=0&&!graphics::PatternSystem::yawCamera(camera,cameraYaw_*35*dt))
        ||(cameraOrbit_!=0&&!graphics::PatternSystem::orbitCamera(camera,{{0,0,0},{0,1,0}},cameraOrbit_*25*dt)))
        throw std::runtime_error("카메라 갱신 실패: 위치·시선·회전축을 확인하세요.");
    app_.getCamera()=camera;
    apply();
}
/// 포커스가 없으면 지속 입력을 해제한다.
void Console::onFocus(bool focused) {
    if(!focused){
        pressed_.fill(false);
        cameraX_=cameraZ_=0;
    }
}

/// 현재 장면을 지정한 뷰로 렌더링한다.
bool Console::onRender() {
    const auto size=app_.getFramebufferSize();
    if(size[0]<3||size[1]<2)return true;
    const auto passes=graphics::PatternSystem::threeViews(
        app_.getCamera(),size[0],size[1],app_.getRenderOptions());
    if(passes.size()!=3)throw std::runtime_error("세 카메라 뷰 생성 실패: 프레임버퍼 크기와 카메라를 확인하세요.");
    return app_.renderViews(passes);
}
```

## openGL_3dAssign_22/main.cpp
### before
```cpp
#include "Console.h"
#include <exception>
#include <iostream>

int main() {
    try {
        graphics::Application app;
        Console practice(app);
        app.setInitHandler([&](graphics::Application&){return practice.initScene();});
        app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
            practice.onKey(key,action,mods);
        });
        app.setUpdateHandler([&](graphics::Application&,float dt){practice.onUpdate(dt);});
        app.setFocusHandler([&](graphics::Application&,bool focused){practice.onFocus(focused);});
        app.setRenderHandler([&](graphics::Application&){return practice.onRender();});
        graphics::ApplicationConfig config;
        config.title="실습 22 - 원근 / XZ / XY";
        config.width=1200;
        config.height=800;
        return app.run(config);
    } catch(const std::exception& error) {
        std::cerr<<"실습 22 실패: "<<error.what()<<'\n';
    } catch(...) {
        std::cerr<<"실습 22 실패: 알 수 없는 예외.\n";
    }
    return 1;
}
```
### after
```cpp
#include "Console.h"
#include <exception>
#include <iostream>

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    try {
        graphics::Application app;
        Console practice(app);
        app.setInitHandler([&](graphics::Application&){return practice.initScene();});
        app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
            practice.onKey(key,action,mods);
        });
        app.setUpdateHandler([&](graphics::Application&,float dt){practice.onUpdate(dt);});
        app.setFocusHandler([&](graphics::Application&,bool focused){practice.onFocus(focused);});
        app.setRenderHandler([&](graphics::Application&){return practice.onRender();});
        graphics::ApplicationConfig config;
        config.title="실습 22 - 원근 / XZ / XY";
        config.width=1200;
        config.height=800;
        return app.run(config);
    } catch(const std::exception& error) {
        std::cerr<<"실습 22 실패: "<<error.what()<<'\n';
    } catch(...) {
        std::cerr<<"실습 22 실패: 알 수 없는 예외.\n";
    }
    return 1;
}
```

## openGL_3dAssign_22/Console.h
### before
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_=0,central_=0;
    std::array<graphics::EntityId,2> lower_{},upper_{},guns_{},poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool,GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_=0,centralAngle_=0,gunAngle_=0,poleTime_=0;
    float swapProgress_=0,swapDirection_=1;
    float cameraX_=0,cameraZ_=0,cameraYaw_=0,cameraOrbit_=0;
    bool lowerRunning_=false,centralRunning_=false,gunRunning_=false,poleRunning_=false;
    bool swapRunning_=false,swapStarted_=false;
    unsigned colorVersion_=0;
    graphics::EntityId box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent);
    void reset();
    void apply();
    void recolor();
public:
    explicit Console(graphics::Application& app):app_(app){}
    bool initScene();
    void onKey(int key,int action,int mods);
    void onUpdate(float dt);
    void onFocus(bool focused);
    bool onRender();
};
```
### after
```cpp
#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_=0,central_=0;
    std::array<graphics::EntityId,2> lower_{},upper_{},guns_{},poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool,GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_=0,centralAngle_=0,gunAngle_=0,poleTime_=0;
    float swapProgress_=0,swapDirection_=1;
    float cameraX_=0,cameraZ_=0,cameraYaw_=0,cameraOrbit_=0;
    bool lowerRunning_=false,centralRunning_=false,gunRunning_=false,poleRunning_=false;
    bool swapRunning_=false,swapStarted_=false;
    unsigned colorVersion_=0;
    /// 부모에 연결한 상자를 만든다.
    graphics::EntityId box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent);
    /// 배치와 조작 상태를 초기화한다.
    void reset();
    /// 계산한 변환을 장면에 반영한다.
    void apply();
    /// 물체 색을 다시 지정한다.
    void recolor();
public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app):app_(app){}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key,int action,int mods);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused);
    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
};
```

## openGL_3dAssign_23/Console.cpp
### before
```cpp
#include "Console.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>
namespace {
void require(bool ok,const char* message) {if(!ok) throw std::runtime_error(message);}
}
bool Console::initScene() {
    auto& scene=app_.getScene();scene.clear();obstacles_.clear();
    body_=graphics::CharacterBody{};body_.feet={-3,0,-3};body_.speed=1;
    body_.height=1.8f;body_.halfWidth={.5f,.5f};
    doors_.fill(0);open_.fill(false);activeWall_.reset();walking_=true;
    const auto stage=graphics::PatternSystem::addRoom(scene,bounds_,false);
    if(!stage) return false;stage_=*stage;
    for(std::size_t face=0;face<4;++face) {
        if(!graphics::PatternSystem::openRoomWall(scene,stage_,face,0)) return false;
    }
    graphics::GridLayout grid;grid.origin={-.5f,0,-.3f};grid.extent={1,1.8f,.6f};
    const auto rig=graphics::PatternSystem::addWalkingRig(scene,grid);if(!rig) return false;rig_=*rig;
    std::cout << "실습 23 난수 시드=" << randomSeed_ << "; 초기화 시 기존 난수 엔진의 다음 수열을 사용합니다.\n";
    std::vector<graphics::Bounds3> occupied{{{-3.8f,0,-3.8f},{-2.2f,2,-2.2f}}};
    for(unsigned i=0;i<3;++i) {
        const glm::vec3 size{.9f+.2f*i,.6f+.25f*i,.9f+.15f*i};
        const auto box=graphics::CollisionSystem::randomFreeBox({{-4,0,-4},{4,size.y,4}},size,occupied,random_);
        if(!box) return false;
        if(!graphics::PatternSystem::addBox(scene,size,(box->minimum+box->maximum)*.5f,{.3f+.2f*i,.5f,.8f-.2f*i})) return false;
        occupied.push_back(*box);obstacles_.push_back(*box);
    }
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={8,5,11};app_.getCamera().target={0,1,0};
    app_.getRenderOptions()=graphics::RenderOptions{};app_.getRenderOptions().depthTest=true;
    app_.getRenderOptions().lighting=true;
    return graphics::PatternSystem::poseWalkingRig(scene,rig_,body_.feet,0,0,body_.speed);
}
void Console::onKey(int key,int action,int mods) {
    if(action!=GLFW_PRESS && action!=GLFW_REPEAT) return;
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    auto& camera=app_.getCamera();
    if(key==GLFW_KEY_X || key==GLFW_KEY_Z) {
        glm::vec3 delta(0);delta[key==GLFW_KEY_X?0:2]=upper?-.35f:.35f;
        require(graphics::PatternSystem::moveCamera(camera,delta),"실습 23 카메라 이동 실패.");return;
    }
    if(key==GLFW_KEY_Y) {require(graphics::PatternSystem::orbitCamera(camera,{camera.target,{0,1,0}},upper?4.0f:-4.0f),"실습 23 카메라 공전 실패.");return;}
    if(key==GLFW_KEY_EQUAL || key==GLFW_KEY_KP_ADD) body_.speed=std::min(3.0f,body_.speed+.1f);
    if(key==GLFW_KEY_MINUS || key==GLFW_KEY_KP_SUBTRACT) body_.speed=std::max(0.0f,body_.speed-.1f);
    if(key==GLFW_KEY_W) {body_.direction={0,0,1};walking_=true;}
    if(key==GLFW_KEY_S) {body_.direction={0,0,-1};walking_=true;}
    if(key==GLFW_KEY_A) {body_.direction={-1,0,0};walking_=true;}
    if(key==GLFW_KEY_D) {body_.direction={1,0,0};walking_=true;}
    if(action!=GLFW_PRESS) return;
    if(key==GLFW_KEY_O) {
        if(activeWall_) open_[*activeWall_]=!open_[*activeWall_];
        else {
            activeWall_=graphics::PatternSystem::cameraFacingWall(app_.getScene(),stage_,camera);
            if(activeWall_) open_[*activeWall_]=true;
        }
        if(activeWall_) std::cout<<"선택한 벽="<<*activeWall_<<(open_[*activeWall_]?" 열기\n":" 원상복귀\n");
    }
    if(key==GLFW_KEY_SPACE) walking_=!walking_;
    if(key==GLFW_KEY_J) graphics::CollisionSystem::jump(body_,5.5f);
    if(key==GLFW_KEY_I) require(initScene(),"실습 23 초기화 실패: 앞선 API 오류를 확인하세요.");
    if(key==GLFW_KEY_Q || key==GLFW_KEY_ESCAPE) app_.requestClose();
}
void Console::onUpdate(float dt) {
    const float requested=body_.speed;
    if(!walking_) body_.speed=0;
    const auto result=graphics::CollisionSystem::stepCharacter(body_,bounds_,obstacles_,dt);
    body_.speed=requested;
    require(result.has_value(),"실습 23 캐릭터 충돌 계산 실패.");
    const float yaw=glm::degrees(std::atan2(body_.direction.x,body_.direction.z));
    require(graphics::PatternSystem::poseWalkingRig(app_.getScene(),rig_,body_.feet,yaw,body_.walkDistance,body_.speed),"실습 23 로봇 관절 자세 적용 실패.");
    for(std::size_t face=0;face<doors_.size();++face) {
        doors_[face]=std::clamp(doors_[face]+(open_[face]?1.0f:-1.0f)*dt,0.0f,1.0f);
        require(graphics::PatternSystem::openRoomWall(app_.getScene(),stage_,face,doors_[face]),"실습 23 벽 개방·복원 실패.");
    }
    if(activeWall_ && !open_[*activeWall_] && doors_[*activeWall_]==0) activeWall_.reset();
}
```
### after
```cpp
#include "Console.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>
namespace {
/// 실패한 작업을 예외로 전달한다.
void require(bool ok,const char* message) {if(!ok) throw std::runtime_error(message);}
}
/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    auto& scene=app_.getScene();scene.clear();obstacles_.clear();
    body_=graphics::CharacterBody{};body_.feet={-3,0,-3};body_.speed=1;
    body_.height=1.8f;body_.halfWidth={.5f,.5f};
    doors_.fill(0);open_.fill(false);activeWall_.reset();walking_=true;
    const auto stage=graphics::PatternSystem::addRoom(scene,bounds_,false);
    if(!stage) return false;stage_=*stage;
    for(std::size_t face=0;face<4;++face) {
        if(!graphics::PatternSystem::openRoomWall(scene,stage_,face,0)) return false;
    }
    graphics::GridLayout grid;grid.origin={-.5f,0,-.3f};grid.extent={1,1.8f,.6f};
    const auto rig=graphics::PatternSystem::addWalkingRig(scene,grid);if(!rig) return false;rig_=*rig;
    std::cout << "실습 23 난수 시드=" << randomSeed_ << "; 초기화 시 기존 난수 엔진의 다음 수열을 사용합니다.\n";
    std::vector<graphics::Bounds3> occupied{{{-3.8f,0,-3.8f},{-2.2f,2,-2.2f}}};
    for(unsigned i=0;i<3;++i) {
        const glm::vec3 size{.9f+.2f*i,.6f+.25f*i,.9f+.15f*i};
        const auto box=graphics::CollisionSystem::randomFreeBox({{-4,0,-4},{4,size.y,4}},size,occupied,random_);
        if(!box) return false;
        if(!graphics::PatternSystem::addBox(scene,size,(box->minimum+box->maximum)*.5f,{.3f+.2f*i,.5f,.8f-.2f*i})) return false;
        occupied.push_back(*box);obstacles_.push_back(*box);
    }
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={8,5,11};app_.getCamera().target={0,1,0};
    app_.getRenderOptions()=graphics::RenderOptions{};app_.getRenderOptions().depthTest=true;
    app_.getRenderOptions().lighting=true;
    return graphics::PatternSystem::poseWalkingRig(scene,rig_,body_.feet,0,0,body_.speed);
}
/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key,int action,int mods) {
    if(action!=GLFW_PRESS && action!=GLFW_REPEAT) return;
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    auto& camera=app_.getCamera();
    if(key==GLFW_KEY_X || key==GLFW_KEY_Z) {
        glm::vec3 delta(0);delta[key==GLFW_KEY_X?0:2]=upper?-.35f:.35f;
        require(graphics::PatternSystem::moveCamera(camera,delta),"실습 23 카메라 이동 실패.");return;
    }
    if(key==GLFW_KEY_Y) {require(graphics::PatternSystem::orbitCamera(camera,{camera.target,{0,1,0}},upper?4.0f:-4.0f),"실습 23 카메라 공전 실패.");return;}
    if(key==GLFW_KEY_EQUAL || key==GLFW_KEY_KP_ADD) body_.speed=std::min(3.0f,body_.speed+.1f);
    if(key==GLFW_KEY_MINUS || key==GLFW_KEY_KP_SUBTRACT) body_.speed=std::max(0.0f,body_.speed-.1f);
    if(key==GLFW_KEY_W) {body_.direction={0,0,1};walking_=true;}
    if(key==GLFW_KEY_S) {body_.direction={0,0,-1};walking_=true;}
    if(key==GLFW_KEY_A) {body_.direction={-1,0,0};walking_=true;}
    if(key==GLFW_KEY_D) {body_.direction={1,0,0};walking_=true;}
    if(action!=GLFW_PRESS) return;
    if(key==GLFW_KEY_O) {
        if(activeWall_) open_[*activeWall_]=!open_[*activeWall_];
        else {
            activeWall_=graphics::PatternSystem::cameraFacingWall(app_.getScene(),stage_,camera);
            if(activeWall_) open_[*activeWall_]=true;
        }
        if(activeWall_) std::cout<<"선택한 벽="<<*activeWall_<<(open_[*activeWall_]?" 열기\n":" 원상복귀\n");
    }
    if(key==GLFW_KEY_SPACE) walking_=!walking_;
    if(key==GLFW_KEY_J) graphics::CollisionSystem::jump(body_,5.5f);
    if(key==GLFW_KEY_I) require(initScene(),"실습 23 초기화 실패: 앞선 API 오류를 확인하세요.");
    if(key==GLFW_KEY_Q || key==GLFW_KEY_ESCAPE) app_.requestClose();
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    const float requested=body_.speed;
    if(!walking_) body_.speed=0;
    const auto result=graphics::CollisionSystem::stepCharacter(body_,bounds_,obstacles_,dt);
    body_.speed=requested;
    require(result.has_value(),"실습 23 캐릭터 충돌 계산 실패.");
    const float yaw=glm::degrees(std::atan2(body_.direction.x,body_.direction.z));
    require(graphics::PatternSystem::poseWalkingRig(app_.getScene(),rig_,body_.feet,yaw,body_.walkDistance,body_.speed),"실습 23 로봇 관절 자세 적용 실패.");
    for(std::size_t face=0;face<doors_.size();++face) {
        doors_[face]=std::clamp(doors_[face]+(open_[face]?1.0f:-1.0f)*dt,0.0f,1.0f);
        require(graphics::PatternSystem::openRoomWall(app_.getScene(),stage_,face,doors_[face]),"실습 23 벽 개방·복원 실패.");
    }
    if(activeWall_ && !open_[*activeWall_] && doors_[*activeWall_]==0) activeWall_.reset();
}
```

## openGL_3dAssign_23/main.cpp
### before
```cpp
#include "Console.h"
#include <iostream>
int main() {
    try {
        graphics::Application app;Console practice(app);
        app.setInitHandler([&](graphics::Application&){return practice.initScene();});
        app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){practice.onKey(key,action,mods);});
        app.setUpdateHandler([&](graphics::Application&,float dt){practice.onUpdate(dt);});
        graphics::ApplicationConfig config;config.title="실습 23 - 로봇";config.width=1100;config.height=800;
        std::cout << "실습 23: WASD 방향, +/- 속도, J 점프, Space 보행 정지/재개, O 벽 열기/같은 벽 복원, I 초기화, x/X z/Z 카메라 이동, y/Y 공전, Q 종료\n";
        return app.run(config);
    } catch(const std::exception& error) {std::cerr<<"실습 23 실패: "<<error.what()<<'\n';return 1;}
}
```
### after
```cpp
#include "Console.h"
#include <iostream>
/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    try {
        graphics::Application app;Console practice(app);
        app.setInitHandler([&](graphics::Application&){return practice.initScene();});
        app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){practice.onKey(key,action,mods);});
        app.setUpdateHandler([&](graphics::Application&,float dt){practice.onUpdate(dt);});
        graphics::ApplicationConfig config;config.title="실습 23 - 로봇";config.width=1100;config.height=800;
        std::cout << "실습 23: WASD 방향, +/- 속도, J 점프, Space 보행 정지/재개, O 벽 열기/같은 벽 복원, I 초기화, x/X z/Z 카메라 이동, y/Y 공전, Q 종료\n";
        return app.run(config);
    } catch(const std::exception& error) {std::cerr<<"실습 23 실패: "<<error.what()<<'\n';return 1;}
}
```

## openGL_3dAssign_23/Console.h
### before
```cpp
#pragma once
#include "framework/Framework.h"
class Console {
    graphics::Application& app_;
    graphics::CharacterBody body_;
    graphics::JointRig rig_;
    graphics::EntityId stage_=0;
    std::vector<graphics::Bounds3> obstacles_;
    graphics::Bounds3 bounds_{{-5,0,-5},{5,4,5}};
    std::array<float,4> doors_{};
    std::array<bool,4> open_{};
    std::optional<std::size_t> activeWall_;
    unsigned randomSeed_=std::random_device{}();
    std::mt19937 random_{randomSeed_};
    bool walking_=true;
public:
    explicit Console(graphics::Application& app):app_(app) {}
    bool initScene();
    void onKey(int key,int action,int mods);
    void onUpdate(float dt);
};
```
### after
```cpp
#pragma once
#include "framework/Framework.h"
class Console {
    graphics::Application& app_;
    graphics::CharacterBody body_;
    graphics::JointRig rig_;
    graphics::EntityId stage_=0;
    std::vector<graphics::Bounds3> obstacles_;
    graphics::Bounds3 bounds_{{-5,0,-5},{5,4,5}};
    std::array<float,4> doors_{};
    std::array<bool,4> open_{};
    std::optional<std::size_t> activeWall_;
    unsigned randomSeed_=std::random_device{}();
    std::mt19937 random_{randomSeed_};
    bool walking_=true;
public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app):app_(app) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key,int action,int mods);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
};
```

## openGL_sample0/main.cpp
### before
```cpp
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <glew.h> // GLEW must be included before GLFW.
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"

#include <cstddef>
#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>


namespace {

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
constexpr char kVertexShaderPath[] = "shaders/basic.vert";
constexpr char kFragmentShaderPath[] = "shaders/basic.frag";
constexpr char kMeshPath[] = "data/cube.txt";

struct RenderVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct Bounds {
    glm::vec3 minimum{std::numeric_limits<float>::max()};
    glm::vec3 maximum{std::numeric_limits<float>::lowest()};
};

struct Camera {
    glm::vec3 position{0.0f, 0.0f, 2.5f};
    glm::vec3 forward{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float speed = 2.0f;
    float radius = 0.15f;
};

Bounds transformBounds(const Bounds& local, const glm::mat4& model) {
    Bounds world;
    // Transform all eight corners to enclose the rotated object in world space.
    for (int corner = 0; corner < 8; ++corner) {
        const glm::vec3 point(
            corner & 1 ? local.maximum.x : local.minimum.x,
            corner & 2 ? local.maximum.y : local.minimum.y,
            corner & 4 ? local.maximum.z : local.minimum.z);
        const glm::vec3 transformed = glm::vec3(model * glm::vec4(point, 1.0f));
        world.minimum = glm::min(world.minimum, transformed);
        world.maximum = glm::max(world.maximum, transformed);
    }
    return world;
}

bool intersectsObject(const glm::vec3& position, float radius, const Bounds& bounds) {
    const glm::vec3 closest = glm::clamp(position, bounds.minimum, bounds.maximum);
    const glm::vec3 difference = position - closest;
    return glm::dot(difference, difference) <= radius * radius;
}

void updateCamera(GLFWwindow* window, Camera& camera, const Bounds& bounds, float deltaTime) {
    if (glfwGetWindowAttrib(window, GLFW_FOCUSED) != GLFW_TRUE) {
        return;
    }
    const glm::vec3 right = glm::normalize(glm::cross(camera.forward, camera.up));
    glm::vec3 direction(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) direction += camera.forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) direction -= camera.forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) direction -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) direction += right;
    if (glm::dot(direction, direction) == 0.0f) {
        return;
    }
    const glm::vec3 movement = glm::normalize(direction) * camera.speed * deltaTime;
    // Small steps prevent a fast camera from jumping across a thin bounding box.
    const int steps = std::max(1, static_cast<int>(std::ceil(
        glm::length(movement) / (camera.radius * 0.5f))));
    const glm::vec3 step = movement / static_cast<float>(steps);
    for (int i = 0; i < steps; ++i) {
        const glm::vec3 candidate = camera.position + step;
        if (intersectsObject(candidate, camera.radius, bounds)) {
            break;
        }
        camera.position = candidate;
    }
}

void glfwErrorCallback(int errorCode, const char* description) {
    std::cerr << "[GLFW 오류 " << errorCode << "] " << description << '\n';
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

bool readTextFile(const char* path, std::string& contents) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "[파일 오류] 셰이더 파일을 열 수 없습니다: " << path << '\n';
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    contents = buffer.str();
    // Files use UTF-8 BOM; GLSL source must start with #version after loading.
    if (contents.size() >= 3 &&
        static_cast<unsigned char>(contents[0]) == 0xEF &&
        static_cast<unsigned char>(contents[1]) == 0xBB &&
        static_cast<unsigned char>(contents[2]) == 0xBF) {
        contents.erase(0, 3);
    }
    return true;
}

GLuint compileShader(GLenum shaderType, const std::string& source, const char* path) {
    const GLuint shader = glCreateShader(shaderType);
    const char* sourcePointer = source.c_str();
    glShaderSource(shader, 1, &sourcePointer, nullptr);
    glCompileShader(shader);

    GLint compileSucceeded = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSucceeded);
    if (compileSucceeded == GL_TRUE) {
        return shader;
    }

    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<std::size_t>(logLength > 0 ? logLength : 1), '\0');
    glGetShaderInfoLog(shader, logLength, nullptr, log.data());

    std::cerr << "[셰이더 컴파일 오류] " << path << '\n' << log << '\n';
    glDeleteShader(shader);
    return 0;
}

GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexSource;
    std::string fragmentSource;
    if (!readTextFile(vertexPath, vertexSource) ||
        !readTextFile(fragmentPath, fragmentSource)) {
        return 0;
    }

    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
    if (vertexShader == 0) {
        return 0;
    }

    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint linkSucceeded = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSucceeded);
    if (linkSucceeded == GL_TRUE) {
        return program;
    }

    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<std::size_t>(logLength > 0 ? logLength : 1), '\0');
    glGetProgramInfoLog(program, logLength, nullptr, log.data());

    std::cerr << "[셰이더 링크 오류]\n" << log << '\n';
    glDeleteProgram(program);
    return 0;
}

bool loadMeshVertices(const char* path, std::vector<RenderVertex>& vertices, Bounds& bounds) {
    std::ifstream input(path);
    if (!input) {
        std::cerr << "[파일 오류] 메시 파일을 열 수 없습니다: " << path << '\n';
        return false;
    }

    const warmup::ParseResult parsed = warmup::MeshParser().parse(input);
    const warmup::ValidationResult validated = warmup::MeshValidator().validate(parsed.mesh);

    for (const warmup::Diagnostic& diagnostic : parsed.diagnostics) {
        std::cerr << "[메시 파싱 오류] 줄 " << diagnostic.line
                  << ": " << diagnostic.message << '\n';
    }
    for (const warmup::Diagnostic& diagnostic : validated.diagnostics) {
        std::cerr << "[메시 검증 오류] 줄 " << diagnostic.line;
        if (diagnostic.face != 0) {
            std::cerr << ", 면 " << diagnostic.face;
        }
        std::cerr << ": " << diagnostic.message << '\n';
    }

    if (!warmup::succeeded(parsed, validated)) {
        return false;
    }

    constexpr std::size_t verticesPerFace = 3;
    vertices.clear();
    vertices.reserve(validated.faces.size() * verticesPerFace);
    bounds = Bounds{};

    for (const warmup::ResolvedFace& face : validated.faces) {
        glm::dvec3 positions[3];
        for (int corner = 0; corner < 3; ++corner) {
            const warmup::Vertex& p = face.vertices[corner];
            positions[corner] = glm::dvec3(p.x, p.y, p.z);
        }
        // TXT has no normals: derive one normal per triangle from its winding.
        glm::dvec3 edgeA = positions[1] - positions[0];
        glm::dvec3 edgeB = positions[2] - positions[0];
        // Match the validator's support for very small valid triangles:
        // scale edges before the cross product to avoid double underflow.
        const glm::dvec3 absoluteA = glm::abs(edgeA);
        const glm::dvec3 absoluteB = glm::abs(edgeB);
        edgeA /= std::max({absoluteA.x, absoluteA.y, absoluteA.z});
        edgeB /= std::max({absoluteB.x, absoluteB.y, absoluteB.z});
        const glm::dvec3 faceNormal = glm::normalize(glm::cross(edgeA, edgeB));
        for (int corner = 0; corner < warmup::mesh_rules::triangleCorners; ++corner) {
            const glm::vec3 position(positions[corner]);
            vertices.push_back({position, glm::vec3(faceNormal)});
            bounds.minimum = glm::min(bounds.minimum, position);
            bounds.maximum = glm::max(bounds.maximum, position);
        }
    }

    if (vertices.empty()) {
        std::cerr << "[메시 오류] 렌더링할 삼각형이 없습니다.\n";
        return false;
    }

    std::cout << "메시 로드 완료: " << validated.faces.size()
              << "개 삼각형, " << vertices.size()
              << "개 렌더링 정점\n";
    return true;
}

} // namespace

int main() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "[초기화 오류] GLFW 초기화에 실패했습니다.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        kWindowWidth,
        kWindowHeight,
        "OpenGL Model / View / Projection - Grayscale Lighting",
        nullptr,
        nullptr);
    if (window == nullptr) {
        std::cerr << "[초기화 오류] GLFW 창을 생성하지 못했습니다.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    const GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cerr << "[초기화 오류] GLEW 초기화 실패: "
                  << glewGetErrorString(glewError) << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // GLEW can generate GL_INVALID_ENUM while loading a core profile context.
    glGetError();

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    const GLuint shaderProgram = createShaderProgram(kVertexShaderPath, kFragmentShaderPath);
    if (shaderProgram == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    // Validated TXT triangles become model-space position/normal data.
    std::vector<RenderVertex> vertices;
    Bounds localBounds;
    if (!loadMeshVertices(kMeshPath, vertices, localBounds)) {
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    GLuint vertexArray = 0;
    GLuint vertexBuffer = 0;
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(RenderVertex)),
        vertices.data(),
        GL_STATIC_DRAW);

    constexpr GLsizei stride = sizeof(RenderVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
        reinterpret_cast<const void*>(offsetof(RenderVertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        reinterpret_cast<const void*>(offsetof(RenderVertex, normal)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const GLint modelLocation = glGetUniformLocation(shaderProgram, "uModel");
    const GLint viewLocation = glGetUniformLocation(shaderProgram, "uView");
    const GLint projectionLocation = glGetUniformLocation(shaderProgram, "uProjection");
    const GLint lightLocation = glGetUniformLocation(shaderProgram, "uLightDirection");
    if (modelLocation == -1 || viewLocation == -1 ||
        projectionLocation == -1 || lightLocation == -1) {
        std::cerr << "[셰이더 오류] 변환 또는 조명 uniform을 찾을 수 없습니다.\n";
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n'
              << "W/S: 전진/후진, A/D: 좌우 이동 (시선 방향 고정)\n"
              << "카메라 구와 오브젝트 AABB 충돌 시 이동 또는 회전을 멈춥니다.\n"
              << "Esc 키를 누르거나 창을 닫으면 종료됩니다.\n";

    Camera camera;
    double previousTime = glfwGetTime();
    float modelAngle = 0.0f;
    glm::mat4 model(1.0f);
    Bounds worldBounds = transformBounds(localBounds, model);
    const glm::vec3 lightDirection = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f));

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(std::clamp(
            currentTime - previousTime, 0.0, 0.05));
        previousTime = currentTime;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (glfwWindowShouldClose(window) == GLFW_TRUE) break;

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        if (framebufferWidth <= 0 || framebufferHeight <= 0) {
            glfwWaitEventsTimeout(0.05);
            previousTime = glfwGetTime();
            continue;
        }
        const float aspect = static_cast<float>(framebufferWidth) / framebufferHeight;

        updateCamera(window, camera, worldBounds, deltaTime);
        const float candidateAngle = std::fmod(
            modelAngle + deltaTime * glm::radians(35.0f), glm::radians(360.0f));
        const glm::mat4 candidateModel = glm::rotate(
            glm::mat4(1.0f),
            candidateAngle,
            glm::normalize(glm::vec3(0.6f, 1.0f, 0.2f)));
        const Bounds candidateBounds = transformBounds(localBounds, candidateModel);
        // Keep the rendered transform and collision bounds synchronized.
        if (!intersectsObject(camera.position, camera.radius, candidateBounds)) {
            modelAngle = candidateAngle;
            model = candidateModel;
            worldBounds = candidateBounds;
        }
        const glm::mat4 view = glm::lookAt(
            camera.position, camera.position + camera.forward, camera.up);
        const glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), aspect, 0.1f, 100.0f);

        glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(lightLocation, 1, glm::value_ptr(lightDirection));
        glBindVertexArray(vertexArray);
        // GLSL outputs clip coordinates. OpenGL clips, divides by w,
        // maps NDC to the viewport, and rasterizes triangles into fragments.
        glDrawArrays(
            GL_TRIANGLES,
            0,
            static_cast<GLsizei>(vertices.size()));

        glfwSwapBuffers(window);
    }

    glBindVertexArray(0);
    glUseProgram(0);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```
### after
```cpp
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"

#include <cstddef>
#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
constexpr char kVertexShaderPath[] = "shaders/basic.vert";
constexpr char kFragmentShaderPath[] = "shaders/basic.frag";
constexpr char kMeshPath[] = "data/cube.txt";

struct RenderVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct Bounds {
    glm::vec3 minimum{std::numeric_limits<float>::max()};
    glm::vec3 maximum{std::numeric_limits<float>::lowest()};
};

struct Camera {
    glm::vec3 position{0.0f, 0.0f, 2.5f};
    glm::vec3 forward{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float speed = 2.0f;
    float radius = 0.15f;
};

/// 모델 경계를 월드 경계로 변환한다.
Bounds transformBounds(const Bounds& local, const glm::mat4& model) {
    Bounds world;

    for (int corner = 0; corner < 8; ++corner) {
        const glm::vec3 point(
            corner & 1 ? local.maximum.x : local.minimum.x,
            corner & 2 ? local.maximum.y : local.minimum.y,
            corner & 4 ? local.maximum.z : local.minimum.z);
        const glm::vec3 transformed = glm::vec3(model * glm::vec4(point, 1.0f));
        world.minimum = glm::min(world.minimum, transformed);
        world.maximum = glm::max(world.maximum, transformed);
    }
    return world;
}

/// 카메라와 물체 경계의 충돌을 검사한다.
bool intersectsObject(const glm::vec3& position, float radius, const Bounds& bounds) {
    const glm::vec3 closest = glm::clamp(position, bounds.minimum, bounds.maximum);
    const glm::vec3 difference = position - closest;
    return glm::dot(difference, difference) <= radius * radius;
}

/// 입력과 충돌에 따라 카메라를 이동한다.
void updateCamera(GLFWwindow* window, Camera& camera, const Bounds& bounds, float deltaTime) {
    if (glfwGetWindowAttrib(window, GLFW_FOCUSED) != GLFW_TRUE) {
        return;
    }
    const glm::vec3 right = glm::normalize(glm::cross(camera.forward, camera.up));
    glm::vec3 direction(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) direction += camera.forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) direction -= camera.forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) direction -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) direction += right;
    if (glm::dot(direction, direction) == 0.0f) {
        return;
    }
    const glm::vec3 movement = glm::normalize(direction) * camera.speed * deltaTime;

    const int steps = std::max(1, static_cast<int>(std::ceil(
        glm::length(movement) / (camera.radius * 0.5f))));
    const glm::vec3 step = movement / static_cast<float>(steps);
    for (int i = 0; i < steps; ++i) {
        const glm::vec3 candidate = camera.position + step;
        if (intersectsObject(candidate, camera.radius, bounds)) {
            break;
        }
        camera.position = candidate;
    }
}

/// GLFW 오류를 출력한다.
void glfwErrorCallback(int errorCode, const char* description) {
    std::cerr << "[GLFW 오류 " << errorCode << "] " << description << '\n';
}

/// 프레임버퍼 크기로 뷰포트를 갱신한다.
void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

/// 텍스트 파일에서 셰이더를 읽는다.
bool readTextFile(const char* path, std::string& contents) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "[파일 오류] 셰이더 파일을 열 수 없습니다: " << path << '\n';
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    contents = buffer.str();

    if (contents.size() >= 3 &&
        static_cast<unsigned char>(contents[0]) == 0xEF &&
        static_cast<unsigned char>(contents[1]) == 0xBB &&
        static_cast<unsigned char>(contents[2]) == 0xBF) {
        contents.erase(0, 3);
    }
    return true;
}

/// 셰이더 소스를 컴파일한다.
GLuint compileShader(GLenum shaderType, const std::string& source, const char* path) {
    const GLuint shader = glCreateShader(shaderType);
    const char* sourcePointer = source.c_str();
    glShaderSource(shader, 1, &sourcePointer, nullptr);
    glCompileShader(shader);

    GLint compileSucceeded = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSucceeded);
    if (compileSucceeded == GL_TRUE) {
        return shader;
    }

    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<std::size_t>(logLength > 0 ? logLength : 1), '\0');
    glGetShaderInfoLog(shader, logLength, nullptr, log.data());

    std::cerr << "[셰이더 컴파일 오류] " << path << '\n' << log << '\n';
    glDeleteShader(shader);
    return 0;
}

/// 셰이더를 연결해 프로그램을 만든다.
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexSource;
    std::string fragmentSource;
    if (!readTextFile(vertexPath, vertexSource) ||
        !readTextFile(fragmentPath, fragmentSource)) {
        return 0;
    }

    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
    if (vertexShader == 0) {
        return 0;
    }

    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint linkSucceeded = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSucceeded);
    if (linkSucceeded == GL_TRUE) {
        return program;
    }

    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<std::size_t>(logLength > 0 ? logLength : 1), '\0');
    glGetProgramInfoLog(program, logLength, nullptr, log.data());

    std::cerr << "[셰이더 링크 오류]\n" << log << '\n';
    glDeleteProgram(program);
    return 0;
}

/// 메시를 읽어 렌더링 정점과 경계를 만든다.
bool loadMeshVertices(const char* path, std::vector<RenderVertex>& vertices, Bounds& bounds) {
    std::ifstream input(path);
    if (!input) {
        std::cerr << "[파일 오류] 메시 파일을 열 수 없습니다: " << path << '\n';
        return false;
    }

    const warmup::ParseResult parsed = warmup::MeshParser().parse(input);
    const warmup::ValidationResult validated = warmup::MeshValidator().validate(parsed.mesh);

    for (const warmup::Diagnostic& diagnostic : parsed.diagnostics) {
        std::cerr << "[메시 파싱 오류] 줄 " << diagnostic.line
                  << ": " << diagnostic.message << '\n';
    }
    for (const warmup::Diagnostic& diagnostic : validated.diagnostics) {
        std::cerr << "[메시 검증 오류] 줄 " << diagnostic.line;
        if (diagnostic.face != 0) {
            std::cerr << ", 면 " << diagnostic.face;
        }
        std::cerr << ": " << diagnostic.message << '\n';
    }

    if (!warmup::succeeded(parsed, validated)) {
        return false;
    }

    constexpr std::size_t verticesPerFace = 3;
    vertices.clear();
    vertices.reserve(validated.faces.size() * verticesPerFace);
    bounds = Bounds{};

    for (const warmup::ResolvedFace& face : validated.faces) {
        glm::dvec3 positions[3];
        for (int corner = 0; corner < 3; ++corner) {
            const warmup::Vertex& p = face.vertices[corner];
            positions[corner] = glm::dvec3(p.x, p.y, p.z);
        }

        glm::dvec3 edgeA = positions[1] - positions[0];
        glm::dvec3 edgeB = positions[2] - positions[0];

        const glm::dvec3 absoluteA = glm::abs(edgeA);
        const glm::dvec3 absoluteB = glm::abs(edgeB);
        edgeA /= std::max({absoluteA.x, absoluteA.y, absoluteA.z});
        edgeB /= std::max({absoluteB.x, absoluteB.y, absoluteB.z});
        const glm::dvec3 faceNormal = glm::normalize(glm::cross(edgeA, edgeB));
        for (int corner = 0; corner < warmup::mesh_rules::triangleCorners; ++corner) {
            const glm::vec3 position(positions[corner]);
            vertices.push_back({position, glm::vec3(faceNormal)});
            bounds.minimum = glm::min(bounds.minimum, position);
            bounds.maximum = glm::max(bounds.maximum, position);
        }
    }

    if (vertices.empty()) {
        std::cerr << "[메시 오류] 렌더링할 삼각형이 없습니다.\n";
        return false;
    }

    std::cout << "메시 로드 완료: " << validated.faces.size()
              << "개 삼각형, " << vertices.size()
              << "개 렌더링 정점\n";
    return true;
}

}

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "[초기화 오류] GLFW 초기화에 실패했습니다.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        kWindowWidth,
        kWindowHeight,
        "OpenGL Model / View / Projection - Grayscale Lighting",
        nullptr,
        nullptr);
    if (window == nullptr) {
        std::cerr << "[초기화 오류] GLFW 창을 생성하지 못했습니다.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    const GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cerr << "[초기화 오류] GLEW 초기화 실패: "
                  << glewGetErrorString(glewError) << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glGetError();

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    const GLuint shaderProgram = createShaderProgram(kVertexShaderPath, kFragmentShaderPath);
    if (shaderProgram == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::vector<RenderVertex> vertices;
    Bounds localBounds;
    if (!loadMeshVertices(kMeshPath, vertices, localBounds)) {
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    GLuint vertexArray = 0;
    GLuint vertexBuffer = 0;
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(RenderVertex)),
        vertices.data(),
        GL_STATIC_DRAW);

    constexpr GLsizei stride = sizeof(RenderVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
        reinterpret_cast<const void*>(offsetof(RenderVertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        reinterpret_cast<const void*>(offsetof(RenderVertex, normal)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const GLint modelLocation = glGetUniformLocation(shaderProgram, "uModel");
    const GLint viewLocation = glGetUniformLocation(shaderProgram, "uView");
    const GLint projectionLocation = glGetUniformLocation(shaderProgram, "uProjection");
    const GLint lightLocation = glGetUniformLocation(shaderProgram, "uLightDirection");
    if (modelLocation == -1 || viewLocation == -1 ||
        projectionLocation == -1 || lightLocation == -1) {
        std::cerr << "[셰이더 오류] 변환 또는 조명 uniform을 찾을 수 없습니다.\n";
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n'
              << "W/S: 전진/후진, A/D: 좌우 이동 (시선 방향 고정)\n"
              << "카메라 구와 오브젝트 AABB 충돌 시 이동 또는 회전을 멈춥니다.\n"
              << "Esc 키를 누르거나 창을 닫으면 종료됩니다.\n";

    Camera camera;
    double previousTime = glfwGetTime();
    float modelAngle = 0.0f;
    glm::mat4 model(1.0f);
    Bounds worldBounds = transformBounds(localBounds, model);
    const glm::vec3 lightDirection = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f));

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(std::clamp(
            currentTime - previousTime, 0.0, 0.05));
        previousTime = currentTime;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (glfwWindowShouldClose(window) == GLFW_TRUE) break;

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        if (framebufferWidth <= 0 || framebufferHeight <= 0) {
            glfwWaitEventsTimeout(0.05);
            previousTime = glfwGetTime();
            continue;
        }
        const float aspect = static_cast<float>(framebufferWidth) / framebufferHeight;

        updateCamera(window, camera, worldBounds, deltaTime);
        const float candidateAngle = std::fmod(
            modelAngle + deltaTime * glm::radians(35.0f), glm::radians(360.0f));
        const glm::mat4 candidateModel = glm::rotate(
            glm::mat4(1.0f),
            candidateAngle,
            glm::normalize(glm::vec3(0.6f, 1.0f, 0.2f)));
        const Bounds candidateBounds = transformBounds(localBounds, candidateModel);

        if (!intersectsObject(camera.position, camera.radius, candidateBounds)) {
            modelAngle = candidateAngle;
            model = candidateModel;
            worldBounds = candidateBounds;
        }
        const glm::mat4 view = glm::lookAt(
            camera.position, camera.position + camera.forward, camera.up);
        const glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), aspect, 0.1f, 100.0f);

        glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(lightLocation, 1, glm::value_ptr(lightDirection));
        glBindVertexArray(vertexArray);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            static_cast<GLsizei>(vertices.size()));

        glfwSwapBuffers(window);
    }

    glBindVertexArray(0);
    glUseProgram(0);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
```

## openGL_sample0/MeshParser.cpp
### before
```cpp
#include "Mesh.h"
#include <cmath>
#include <istream>
#include <sstream>

namespace warmup {
namespace {
// Read the whole token, not just a numeric prefix such as the 3 in "3abc".
bool readNumber(const std::string& token, double& value, std::string& error) {
    // The assignment accepts ordinary decimal/scientific notation, not 0x hexadecimal notation.
    if (token.find('x') != std::string::npos || token.find('X') != std::string::npos) {
        error = "Invalid number: " + token;
        return false;
    }
    std::istringstream input(token);
    double number = 0;
    char extra;
    if (!(input >> number) || input >> extra || !std::isfinite(number)) {
        error = "Invalid number or numeric range exceeded: " + token;
        return false;
    }
    value = number;
    return true;
}

bool readIndex(const std::string& token, std::size_t& index, std::string& error) {
    if (token.empty()) {
        error = "Missing index.";
        return false;
    }
    for (char ch : token) {
        if (ch < '0' || ch > '9') {
            error = "Invalid positive index: " + token;
            return false;
        }
    }
    std::istringstream input(token);
    std::size_t number = 0;
    if (!(input >> number) || number == 0) {
        error = "Index is outside the supported positive integer range: " + token;
        return false;
    }
    index = number - 1;
    return true;
}

bool readVertex(const std::vector<std::string>& tokens, Vertex& vertex, std::string& error) {
    if (tokens.size() != 3) {
        error = "A vertex requires three coordinate components.";
        return false;
    }
    return readNumber(tokens[0], vertex.x, error)
        && readNumber(tokens[1], vertex.y, error)
        && readNumber(tokens[2], vertex.z, error);
}

bool readTexture(const std::vector<std::string>& tokens, Texture& texture, std::string& error) {
    if (tokens.size() != 2) {
        error = "A texture requires two coordinate components.";
        return false;
    }
    return readNumber(tokens[0], texture.s, error)
        && readNumber(tokens[1], texture.t, error);
}

bool readCorner(const std::string& token, CornerIndex& corner, std::string& error) {
    CornerIndex candidate;
    std::size_t slash = token.find('/');
    if (slash != std::string::npos && token.find('/', slash + 1) != std::string::npos) {
        error = "Expected vertex or vertex/texture: " + token;
        return false;
    }
    if (!readIndex(token.substr(0, slash), candidate.vertex, error)) return false;
    if (slash != std::string::npos) {
        if (!readIndex(token.substr(slash + 1), candidate.texture, error)) return false;
        candidate.hasTexture = true;
    }
    corner = candidate;
    return true;
}

bool readFace(const std::vector<std::string>& tokens, TriangleIndices& face, std::string& error) {
    if (tokens.size() != mesh_rules::triangleCorners) {
        error = "A face requires exactly three corners.";
        return false;
    }
    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        if (!readCorner(tokens[i], face.corners[i], error)) {
            error = "Corner " + std::to_string(i + 1) + ": " + error;
            return false;
        }
        if (face.corners[i].hasTexture != face.corners[0].hasTexture) {
            error = "Mixed face formats: use three vertex indices or three vertex/texture pairs.";
            return false;
        }
    }
    return true;
}
}

ParseResult MeshParser::parse(std::istream& input) const {
    ParseResult result;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;
        // UTF-8 text files may start with these three BOM bytes.
        if (lineNumber == 1 && line.compare(0, 3, "\xef\xbb\xbf") == 0) line.erase(0, 3);
        std::size_t comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        std::istringstream row(line);
        std::string kind, token;
        if (!(row >> kind)) continue;
        std::vector<std::string> tokens;
        while (row >> token) tokens.push_back(token);

        std::string error;
        std::size_t faceNumber = 0;
        if (kind == "v") {
            VertexRecord record;
            record.line = lineNumber;
            record.valid = readVertex(tokens, record.value, error);
            result.mesh.vertices.push_back(record);
        } else if (kind == "vt") {
            TextureRecord record;
            record.line = lineNumber;
            record.valid = readTexture(tokens, record.value, error);
            result.mesh.textures.push_back(record);
        } else if (kind == "f") {
            FaceRecord record;
            record.line = lineNumber;
            record.valid = readFace(tokens, record.value, error);
            result.mesh.faces.push_back(record);
            faceNumber = result.mesh.faces.size();
        } else {
            error = "Unsupported record: " + kind;
        }
        if (!error.empty()) result.diagnostics.push_back({lineNumber, faceNumber, error});
    }
    if (input.bad() || (input.fail() && !input.eof())) {
        result.diagnostics.push_back({lineNumber, 0, "Input stream read failed."});
    }
    return result;
}
}
```
### after
```cpp
#include "Mesh.h"
#include <cmath>
#include <istream>
#include <sstream>

namespace warmup {
namespace {

/// 숫자 토큰 전체를 해석한다.
bool readNumber(const std::string& token, double& value, std::string& error) {

    if (token.find('x') != std::string::npos || token.find('X') != std::string::npos) {
        error = "Invalid number: " + token;
        return false;
    }
    std::istringstream input(token);
    double number = 0;
    char extra;
    if (!(input >> number) || input >> extra || !std::isfinite(number)) {
        error = "Invalid number or numeric range exceeded: " + token;
        return false;
    }
    value = number;
    return true;
}

/// 외부 인덱스를 내부 인덱스로 변환한다.
bool readIndex(const std::string& token, std::size_t& index, std::string& error) {
    if (token.empty()) {
        error = "Missing index.";
        return false;
    }
    for (char ch : token) {
        if (ch < '0' || ch > '9') {
            error = "Invalid positive index: " + token;
            return false;
        }
    }
    std::istringstream input(token);
    std::size_t number = 0;
    if (!(input >> number) || number == 0) {
        error = "Index is outside the supported positive integer range: " + token;
        return false;
    }
    index = number - 1;
    return true;
}

/// 정점 좌표를 해석한다.
bool readVertex(const std::vector<std::string>& tokens, Vertex& vertex, std::string& error) {
    if (tokens.size() != 3) {
        error = "A vertex requires three coordinate components.";
        return false;
    }
    return readNumber(tokens[0], vertex.x, error)
        && readNumber(tokens[1], vertex.y, error)
        && readNumber(tokens[2], vertex.z, error);
}

/// 텍스처 좌표를 해석한다.
bool readTexture(const std::vector<std::string>& tokens, Texture& texture, std::string& error) {
    if (tokens.size() != 2) {
        error = "A texture requires two coordinate components.";
        return false;
    }
    return readNumber(tokens[0], texture.s, error)
        && readNumber(tokens[1], texture.t, error);
}

/// 면의 정점·텍스처 인덱스를 해석한다.
bool readCorner(const std::string& token, CornerIndex& corner, std::string& error) {
    CornerIndex candidate;
    std::size_t slash = token.find('/');
    if (slash != std::string::npos && token.find('/', slash + 1) != std::string::npos) {
        error = "Expected vertex or vertex/texture: " + token;
        return false;
    }
    if (!readIndex(token.substr(0, slash), candidate.vertex, error)) return false;
    if (slash != std::string::npos) {
        if (!readIndex(token.substr(slash + 1), candidate.texture, error)) return false;
        candidate.hasTexture = true;
    }
    corner = candidate;
    return true;
}

/// 삼각형 면을 해석한다.
bool readFace(const std::vector<std::string>& tokens, TriangleIndices& face, std::string& error) {
    if (tokens.size() != mesh_rules::triangleCorners) {
        error = "A face requires exactly three corners.";
        return false;
    }
    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        if (!readCorner(tokens[i], face.corners[i], error)) {
            error = "Corner " + std::to_string(i + 1) + ": " + error;
            return false;
        }
        if (face.corners[i].hasTexture != face.corners[0].hasTexture) {
            error = "Mixed face formats: use three vertex indices or three vertex/texture pairs.";
            return false;
        }
    }
    return true;
}
}

/// 입력 메시와 문법 오류를 읽는다.
ParseResult MeshParser::parse(std::istream& input) const {
    ParseResult result;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;

        if (lineNumber == 1 && line.compare(0, 3, "\xef\xbb\xbf") == 0) line.erase(0, 3);
        std::size_t comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        std::istringstream row(line);
        std::string kind, token;
        if (!(row >> kind)) continue;
        std::vector<std::string> tokens;
        while (row >> token) tokens.push_back(token);

        std::string error;
        std::size_t faceNumber = 0;
        if (kind == "v") {
            VertexRecord record;
            record.line = lineNumber;
            record.valid = readVertex(tokens, record.value, error);
            result.mesh.vertices.push_back(record);
        } else if (kind == "vt") {
            TextureRecord record;
            record.line = lineNumber;
            record.valid = readTexture(tokens, record.value, error);
            result.mesh.textures.push_back(record);
        } else if (kind == "f") {
            FaceRecord record;
            record.line = lineNumber;
            record.valid = readFace(tokens, record.value, error);
            result.mesh.faces.push_back(record);
            faceNumber = result.mesh.faces.size();
        } else {
            error = "Unsupported record: " + kind;
        }
        if (!error.empty()) result.diagnostics.push_back({lineNumber, faceNumber, error});
    }
    if (input.bad() || (input.fail() && !input.eof())) {
        result.diagnostics.push_back({lineNumber, 0, "Input stream read failed."});
    }
    return result;
}
}
```

## openGL_sample0/MeshValidator.cpp
### before
```cpp
#include "Mesh.h"
#include <cmath>

namespace warmup {
namespace {
bool inRange(double value, double minimum, double maximum) {
    return std::isfinite(value) && value >= minimum && value <= maximum;
}
bool vertexInRange(const Vertex& vertex) {
    return inRange(vertex.x, mesh_rules::vertexMinimum, mesh_rules::vertexMaximum)
        && inRange(vertex.y, mesh_rules::vertexMinimum, mesh_rules::vertexMaximum)
        && inRange(vertex.z, mesh_rules::vertexMinimum, mesh_rules::vertexMaximum);
}
bool textureInRange(const Texture& texture) {
    return inRange(texture.s, mesh_rules::textureMinimum, mesh_rules::textureMaximum)
        && inRange(texture.t, mesh_rules::textureMinimum, mesh_rules::textureMaximum);
}
bool sameVertex(const Vertex& a, const Vertex& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
bool sameTexture(const Texture& a, const Texture& b) {
    return a.s == b.s && a.t == b.t;
}
Vertex subtract(const Vertex& a, const Vertex& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
Vertex cross(const Vertex& a, const Vertex& b) {
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
double squaredLength(const Vertex& vertex) {
    return vertex.x*vertex.x + vertex.y*vertex.y + vertex.z*vertex.z;
}
bool normalizeScale(Vertex& vertex) {
    double scale = std::abs(vertex.x);
    if (std::abs(vertex.y) > scale) scale = std::abs(vertex.y);
    if (std::abs(vertex.z) > scale) scale = std::abs(vertex.z);
    if (scale == 0) return false;
    vertex.x /= scale;
    vertex.y /= scale;
    vertex.z /= scale;
    return true;
}
bool isCollinear(const Vertex& a, const Vertex& b, const Vertex& c) {
    Vertex edgeA = subtract(b, a);
    Vertex edgeB = subtract(c, a);
    // Preserve the previous fix for very small, valid triangles.
    if (!normalizeScale(edgeA) || !normalizeScale(edgeB)) return true;
    return squaredLength(cross(edgeA, edgeB))
        <= mesh_rules::collinearSinSquared * squaredLength(edgeA) * squaredLength(edgeB);
}

bool resolveFace(const Mesh& mesh, const FaceRecord& record,
                 ResolvedFace& face, std::string& error) {
    const TriangleIndices& triangle = record.value;
    face.hasTextures = triangle.corners[0].hasTexture;
    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        const CornerIndex& corner = triangle.corners[i];
        if (corner.vertex >= mesh.vertices.size()) {
            error = "Vertex index out of range at corner " + std::to_string(i + 1) + '.';
            return false;
        }
        const VertexRecord& vertex = mesh.vertices[corner.vertex];
        if (!vertex.valid || !vertexInRange(vertex.value)) {
            error = "Face references an invalid vertex declaration.";
            return false;
        }
        face.vertexIndices[i] = corner.vertex;
        face.vertices[i] = vertex.value;
        if (corner.hasTexture != face.hasTextures) {
            error = "Mixed face formats.";
            return false;
        }
        if (corner.hasTexture) {
            if (corner.texture >= mesh.textures.size()) {
                error = "Texture index out of range at corner " + std::to_string(i + 1) + '.';
                return false;
            }
            const TextureRecord& texture = mesh.textures[corner.texture];
            if (!texture.valid || !textureInRange(texture.value)) {
                error = "Face references an invalid texture declaration.";
                return false;
            }
            face.textures[i] = texture.value;
        }
    }
    // Compare each pair of corners within this face only.
    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        for (int j = i + 1; j < mesh_rules::triangleCorners; ++j) {
            if (face.vertexIndices[i] == face.vertexIndices[j]) {
                error = "Duplicate vertex index within this face.";
                return false;
            }
            if (sameVertex(face.vertices[i], face.vertices[j])) {
                error = "Duplicate vertex coordinates within this face.";
                return false;
            }
            if (face.hasTextures && sameTexture(face.textures[i], face.textures[j])) {
                error = "Duplicate texture coordinates within this face.";
                return false;
            }
        }
    }
    if (isCollinear(face.vertices[0], face.vertices[1], face.vertices[2])) {
        error = "Collinear vertices do not form a triangle.";
        return false;
    }
    return true;
}
}

ValidationResult MeshValidator::validate(const Mesh& mesh) const {
    ValidationResult result;
    for (std::size_t i = 0; i < mesh.vertices.size(); ++i) {
        const VertexRecord& record = mesh.vertices[i];
        if (record.valid && !vertexInRange(record.value)) {
            result.diagnostics.push_back({record.line, 0,
                "Vertex " + std::to_string(i + 1) + " coordinate outside [-1, 1]."});
        }
    }
    for (std::size_t i = 0; i < mesh.textures.size(); ++i) {
        const TextureRecord& record = mesh.textures[i];
        if (record.valid && !textureInRange(record.value)) {
            result.diagnostics.push_back({record.line, 0,
                "Texture " + std::to_string(i + 1) + " coordinate outside [0, 1]."});
        }
    }
    if (mesh.faces.empty()) result.diagnostics.push_back({0, 0, "No faces found."});
    for (std::size_t i = 0; i < mesh.faces.size(); ++i) {
        const FaceRecord& record = mesh.faces[i];
        if (!record.valid) continue; // The parser already reported this syntax error.
        ResolvedFace face;
        face.number = i + 1;
        std::string error;
        if (resolveFace(mesh, record, face, error)) result.faces.push_back(face);
        else result.diagnostics.push_back({record.line, i + 1, error});
    }
    return result;
}

bool succeeded(const ParseResult& parsed, const ValidationResult& validated) {
    return parsed.diagnostics.empty() && validated.diagnostics.empty()
        && !parsed.mesh.faces.empty() && validated.faces.size() == parsed.mesh.faces.size();
}
}
```
### after
```cpp
#include "Mesh.h"
#include <cmath>

namespace warmup {
namespace {
/// 값이 지정 범위 안에 있는지 반환한다.
bool inRange(double value, double minimum, double maximum) {
    return std::isfinite(value) && value >= minimum && value <= maximum;
}
/// 정점 좌표 범위를 검사한다.
bool vertexInRange(const Vertex& vertex) {
    return inRange(vertex.x, mesh_rules::vertexMinimum, mesh_rules::vertexMaximum)
        && inRange(vertex.y, mesh_rules::vertexMinimum, mesh_rules::vertexMaximum)
        && inRange(vertex.z, mesh_rules::vertexMinimum, mesh_rules::vertexMaximum);
}
/// 텍스처 좌표 범위를 검사한다.
bool textureInRange(const Texture& texture) {
    return inRange(texture.s, mesh_rules::textureMinimum, mesh_rules::textureMaximum)
        && inRange(texture.t, mesh_rules::textureMinimum, mesh_rules::textureMaximum);
}
/// 두 정점 좌표가 같은지 반환한다.
bool sameVertex(const Vertex& a, const Vertex& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}
/// 두 텍스처 좌표가 같은지 반환한다.
bool sameTexture(const Texture& a, const Texture& b) {
    return a.s == b.s && a.t == b.t;
}
/// 두 정점 벡터의 차를 구한다.
Vertex subtract(const Vertex& a, const Vertex& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}
/// 두 벡터의 외적을 구한다.
Vertex cross(const Vertex& a, const Vertex& b) {
    return {a.y*b.z - a.z*b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x};
}
/// 벡터 길이의 제곱을 구한다.
double squaredLength(const Vertex& vertex) {
    return vertex.x*vertex.x + vertex.y*vertex.y + vertex.z*vertex.z;
}
/// 벡터 계산의 수치 범위를 조절한다.
bool normalizeScale(Vertex& vertex) {
    double scale = std::abs(vertex.x);
    if (std::abs(vertex.y) > scale) scale = std::abs(vertex.y);
    if (std::abs(vertex.z) > scale) scale = std::abs(vertex.z);
    if (scale == 0) return false;
    vertex.x /= scale;
    vertex.y /= scale;
    vertex.z /= scale;
    return true;
}
/// 세 정점이 일직선에 있는지 검사한다.
bool isCollinear(const Vertex& a, const Vertex& b, const Vertex& c) {
    Vertex edgeA = subtract(b, a);
    Vertex edgeB = subtract(c, a);

    if (!normalizeScale(edgeA) || !normalizeScale(edgeB)) return true;
    return squaredLength(cross(edgeA, edgeB))
        <= mesh_rules::collinearSinSquared * squaredLength(edgeA) * squaredLength(edgeB);
}

bool resolveFace(const Mesh& mesh, const FaceRecord& record,
                 ResolvedFace& face, std::string& error) {
    const TriangleIndices& triangle = record.value;
    face.hasTextures = triangle.corners[0].hasTexture;
    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        const CornerIndex& corner = triangle.corners[i];
        if (corner.vertex >= mesh.vertices.size()) {
            error = "Vertex index out of range at corner " + std::to_string(i + 1) + '.';
            return false;
        }
        const VertexRecord& vertex = mesh.vertices[corner.vertex];
        if (!vertex.valid || !vertexInRange(vertex.value)) {
            error = "Face references an invalid vertex declaration.";
            return false;
        }
        face.vertexIndices[i] = corner.vertex;
        face.vertices[i] = vertex.value;
        if (corner.hasTexture != face.hasTextures) {
            error = "Mixed face formats.";
            return false;
        }
        if (corner.hasTexture) {
            if (corner.texture >= mesh.textures.size()) {
                error = "Texture index out of range at corner " + std::to_string(i + 1) + '.';
                return false;
            }
            const TextureRecord& texture = mesh.textures[corner.texture];
            if (!texture.valid || !textureInRange(texture.value)) {
                error = "Face references an invalid texture declaration.";
                return false;
            }
            face.textures[i] = texture.value;
        }
    }

    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        for (int j = i + 1; j < mesh_rules::triangleCorners; ++j) {
            if (face.vertexIndices[i] == face.vertexIndices[j]) {
                error = "Duplicate vertex index within this face.";
                return false;
            }
            if (sameVertex(face.vertices[i], face.vertices[j])) {
                error = "Duplicate vertex coordinates within this face.";
                return false;
            }
            if (face.hasTextures && sameTexture(face.textures[i], face.textures[j])) {
                error = "Duplicate texture coordinates within this face.";
                return false;
            }
        }
    }
    if (isCollinear(face.vertices[0], face.vertices[1], face.vertices[2])) {
        error = "Collinear vertices do not form a triangle.";
        return false;
    }
    return true;
}
}

/// 메시의 좌표와 면을 검사한다.
ValidationResult MeshValidator::validate(const Mesh& mesh) const {
    ValidationResult result;
    for (std::size_t i = 0; i < mesh.vertices.size(); ++i) {
        const VertexRecord& record = mesh.vertices[i];
        if (record.valid && !vertexInRange(record.value)) {
            result.diagnostics.push_back({record.line, 0,
                "Vertex " + std::to_string(i + 1) + " coordinate outside [-1, 1]."});
        }
    }
    for (std::size_t i = 0; i < mesh.textures.size(); ++i) {
        const TextureRecord& record = mesh.textures[i];
        if (record.valid && !textureInRange(record.value)) {
            result.diagnostics.push_back({record.line, 0,
                "Texture " + std::to_string(i + 1) + " coordinate outside [0, 1]."});
        }
    }
    if (mesh.faces.empty()) result.diagnostics.push_back({0, 0, "No faces found."});
    for (std::size_t i = 0; i < mesh.faces.size(); ++i) {
        const FaceRecord& record = mesh.faces[i];
        if (!record.valid) continue;
        ResolvedFace face;
        face.number = i + 1;
        std::string error;
        if (resolveFace(mesh, record, face, error)) result.faces.push_back(face);
        else result.diagnostics.push_back({record.line, i + 1, error});
    }
    return result;
}

/// 파싱과 검사 결과가 성공했는지 반환한다.
bool succeeded(const ParseResult& parsed, const ValidationResult& validated) {
    return parsed.diagnostics.empty() && validated.diagnostics.empty()
        && !parsed.mesh.faces.empty() && validated.faces.size() == parsed.mesh.faces.size();
}
}
```

## openGL_sample0/Mesh.h
### before
```cpp
#pragma once
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace warmup {
namespace mesh_rules {
const double vertexMinimum = -1.0;
const double vertexMaximum = 1.0;
const double textureMinimum = 0.0;
const double textureMaximum = 1.0;
const double collinearSinSquared = 1e-20;
const int outputPrecision = 9;
const int triangleCorners = 3;
}

struct Vertex {
    double x = 0, y = 0, z = 0;
};
struct Texture {
    double s = 0, t = 0;
};
struct CornerIndex {
    std::size_t vertex = 0;  // Internal indices start at zero.
    std::size_t texture = 0;
    bool hasTexture = false;
};
struct TriangleIndices {
    CornerIndex corners[mesh_rules::triangleCorners];
};

// Keep an invalid declaration's slot so later indices do not shift.
struct VertexRecord {
    std::size_t line = 0;
    bool valid = false;     // Whether its syntax was parsed successfully.
    Vertex value;
};
struct TextureRecord {
    std::size_t line = 0;
    bool valid = false;
    Texture value;
};
struct FaceRecord {
    std::size_t line = 0;
    bool valid = false;
    TriangleIndices value;
};
struct Mesh {
    std::vector<VertexRecord> vertices;
    std::vector<TextureRecord> textures;
    std::vector<FaceRecord> faces;
};
struct Diagnostic {
    std::size_t line = 0;
    std::size_t face = 0;   // Zero means this error does not belong to a face.
    std::string message;
};
struct ParseResult {
    Mesh mesh;
    std::vector<Diagnostic> diagnostics;
};
struct ResolvedFace {
    std::size_t number = 0;
    std::size_t vertexIndices[mesh_rules::triangleCorners] = {};
    Vertex vertices[mesh_rules::triangleCorners];
    Texture textures[mesh_rules::triangleCorners];
    bool hasTextures = false;
};
struct ValidationResult {
    std::vector<ResolvedFace> faces;
    std::vector<Diagnostic> diagnostics;
};

class MeshParser {
public:
    ParseResult parse(std::istream& input) const;
};
class MeshValidator {
public:
    ValidationResult validate(const Mesh& mesh) const;
};
class MeshReporter {
public:
    void print(const ParseResult& parsed, const ValidationResult& validated) const;
    void print(const ParseResult& parsed, const ValidationResult& validated, std::ostream& output) const;
};
bool succeeded(const ParseResult& parsed, const ValidationResult& validated);
}
```
### after
```cpp
#pragma once
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace warmup {
namespace mesh_rules {
const double vertexMinimum = -1.0;
const double vertexMaximum = 1.0;
const double textureMinimum = 0.0;
const double textureMaximum = 1.0;
const double collinearSinSquared = 1e-20;
const int outputPrecision = 9;
const int triangleCorners = 3;
}

struct Vertex {
    double x = 0, y = 0, z = 0;
};
struct Texture {
    double s = 0, t = 0;
};
struct CornerIndex {
    std::size_t vertex = 0;
    std::size_t texture = 0;
    bool hasTexture = false;
};
struct TriangleIndices {
    CornerIndex corners[mesh_rules::triangleCorners];
};

struct VertexRecord {
    std::size_t line = 0;
    bool valid = false;
    Vertex value;
};
struct TextureRecord {
    std::size_t line = 0;
    bool valid = false;
    Texture value;
};
struct FaceRecord {
    std::size_t line = 0;
    bool valid = false;
    TriangleIndices value;
};
struct Mesh {
    std::vector<VertexRecord> vertices;
    std::vector<TextureRecord> textures;
    std::vector<FaceRecord> faces;
};
struct Diagnostic {
    std::size_t line = 0;
    std::size_t face = 0;
    std::string message;
};
struct ParseResult {
    Mesh mesh;
    std::vector<Diagnostic> diagnostics;
};
struct ResolvedFace {
    std::size_t number = 0;
    std::size_t vertexIndices[mesh_rules::triangleCorners] = {};
    Vertex vertices[mesh_rules::triangleCorners];
    Texture textures[mesh_rules::triangleCorners];
    bool hasTextures = false;
};
struct ValidationResult {
    std::vector<ResolvedFace> faces;
    std::vector<Diagnostic> diagnostics;
};

class MeshParser {
public:
    /// 입력 메시와 문법 오류를 읽는다.
    ParseResult parse(std::istream& input) const;
};
class MeshValidator {
public:
    /// 메시의 좌표와 면을 검사한다.
    ValidationResult validate(const Mesh& mesh) const;
};
class MeshReporter {
public:
    void print(const ParseResult& parsed, const ValidationResult& validated) const;
    void print(const ParseResult& parsed, const ValidationResult& validated, std::ostream& output) const;
};
/// 파싱과 검사 결과가 성공했는지 반환한다.
bool succeeded(const ParseResult& parsed, const ValidationResult& validated);
}
```

## standard/Console.h
### before
```cpp
#pragma once
#include "framework/Framework.h"

class Console {
    graphics::Application& app_;
    std::optional<graphics::MotionId> pathId_;
    std::optional<graphics::MotionId> spinId_;
    std::optional<graphics::MotionId> globalId_;
    std::optional<graphics::LayerId> mirrorLayer_;
    std::optional<graphics::LayerId> normalSpinLayer_;
    std::optional<graphics::LayerId> insertedLayer_;
    bool paused_ = false;
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;
    enum class Demo { Motion, Grid, Room };
    Demo demo_=Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_=0, roomCube_=0;
    bool threeViews_=false, trace_=false, floorOpen_=false;
    bool initGrid();
    bool initRoom();

    bool choosePath(int kind);
    bool changeReflection();
    bool toggleInsertedLayer();
    void printHelp() const;

public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action);
    void onMouse(const graphics::MouseEvent& event);
    void onUpdate(float dt);
    bool onRender();
};
```
### after
```cpp
#pragma once
#include "framework/Framework.h"

class Console {
    graphics::Application& app_;
    std::optional<graphics::MotionId> pathId_;
    std::optional<graphics::MotionId> spinId_;
    std::optional<graphics::MotionId> globalId_;
    std::optional<graphics::LayerId> mirrorLayer_;
    std::optional<graphics::LayerId> normalSpinLayer_;
    std::optional<graphics::LayerId> insertedLayer_;
    bool paused_ = false;
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;
    enum class Demo { Motion, Grid, Room };
    Demo demo_=Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_=0, roomCube_=0;
    bool threeViews_=false, trace_=false, floorOpen_=false;
    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();
    /// 회전하는 방 예제를 구성한다.
    bool initRoom();

    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);
    /// 반사 변환을 순서대로 변경한다.
    bool changeReflection();
    /// 크기 레이어를 삽입하거나 제거한다.
    bool toggleInsertedLayer();
    /// 조작 방법을 출력한다.
    void printHelp() const;

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app) : app_(app) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action);
    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
};
```

## standard/Console.cpp
### before
```cpp
#include "Console.h"
#include <iostream>

bool Console::choosePath(int kind) {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);

    if (kind == 1) {
        path.path = graphics::PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 2) {
        path.path = graphics::PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 3) {
        path.path = graphics::PathSystem::polygon({
            {1, -.5f, 0},
            {2.4f, -.4f, 0},
            {2.1f, .6f, 0},
            {1.4f, .8f, 0},
            {.9f, .2f, 0}
        });
    } else if (kind == 4) {
        path.path = [center](double u) {
            const double angle = u * 6.283185307179586;
            return center + glm::vec3(
                float(.7 * std::sin(2 * angle)),
                float(.6 * std::sin(angle)),
                float(.3 * std::cos(angle)));
        };
        path.interpolation = graphics::PathSystem::smoothStep;
    } else if (kind == 5) {
        path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    } else if (kind == 6) {
        path.path = graphics::PathSystem::polyline({
            {1, -.5f, 0},
            {2.4f, -.5f, 0},
            {2, .6f, 0}
        });
        path.playback = graphics::Playback::PingPong;
    } else {
        return false;
    }

    graphics::Motion motion;
    auto& motions = app_.getScene().getMotions();
    if (!pathId_ || !motion.append(graphics::Motion::followPath(path))) {
        return false;
    }
    if (!motions.setMotion(*pathId_, motion) || !motions.seekMotion(*pathId_, 0)) {
        return false;
    }
    std::cout << "경로 " << kind << ": 시작 시각 0초로 변경\n";
    return true;
}

bool Console::initScene() {
    demo_=Demo::Motion;
    auto& scene = app_.getScene();
    scene.clear();
    paused_ = false;
    spinEnabled_ = true;
    globalEnabled_ = false;
    reflection_ = 1;
    insertedLayer_.reset();

    const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
    const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
    if (!first || !second) {
        return false;
    }
    if (!scene.setTint(*first, {1, .35f, .2f})
        || !scene.setTint(*second, {.2f, .7f, 1})) {
        return false;
    }

    auto& motions = scene.getMotions();
    pathId_ = motions.createMotion(graphics::Motion{});
    graphics::RotationMotion spin;
    spin.degreesPerSecond = 90;
    spinId_ = motions.createSpin(spin);
    graphics::RotationMotion global;
    global.degreesPerSecond = 15;
    globalId_ = motions.createOrbit(global);
    if (!pathId_ || !spinId_ || !globalId_ || !choosePath(1)) {
        return false;
    }
    if (!motions.setMotionEnabled(*globalId_, false)
        || !motions.bindMotion(graphics::MotionSystem::rootLayer(), *globalId_)) {
        return false;
    }

    const auto normalPath = motions.pushLayer(graphics::MotionSystem::rootLayer());
    const auto reflection = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
    if (!normalPath || !reflection) {
        return false;
    }
    graphics::LayerConfig mirror;
    mirror.transform = *reflection;
    mirrorLayer_ = motions.pushLayer(graphics::MotionSystem::rootLayer(), mirror);
    if (!mirrorLayer_) {
        return false;
    }
    const auto mirroredPath = motions.pushLayer(*mirrorLayer_);
    normalSpinLayer_ = motions.pushLayer(*normalPath);
    if (!mirroredPath || !normalSpinLayer_) {
        return false;
    }
    const auto mirroredSpin = motions.pushLayer(*mirroredPath);
    if (!mirroredSpin
        || !motions.bindMotion(*normalPath, *pathId_)
        || !motions.bindMotion(*mirroredPath, *pathId_)
        || !motions.bindMotion(*normalSpinLayer_, *spinId_)
        || !motions.bindMotion(*mirroredSpin, *spinId_)
        || !scene.bindEntity(*first, *normalSpinLayer_)
        || !scene.bindEntity(*second, *mirroredSpin)) {
        return false;
    }
    if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
        || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
        || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) {
        return false;
    }

    auto& camera = app_.getCamera();
    camera.position = {5, 3, 8};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    printHelp();
    return true;
}

bool Console::changeReflection() {
    const int next = (reflection_ + 1) % 4;
    std::optional<glm::mat4> matrix = glm::mat4(1);
    if (next == 1) {
        matrix = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
    } else if (next == 2) {
        matrix = graphics::TransformSystem::reflectPoint({0, 0, 0});
    } else if (next == 3) {
        matrix = graphics::TransformSystem::reflectLine(graphics::AxisLine{});
    }
    if (!mirrorLayer_ || !matrix) {
        return false;
    }
    graphics::LayerConfig config;
    config.transform = *matrix;
    if (!app_.getScene().getMotions().setLayer(*mirrorLayer_, config)) {
        return false;
    }
    reflection_ = next;
    const char* names[] = {"없음", "YZ평면 반사", "원점 점대칭", "Y축 직선대칭"};
    std::cout << "반사: " << names[reflection_] << '\n';
    return true;
}

bool Console::toggleInsertedLayer() {
    auto& motions = app_.getScene().getMotions();
    if (insertedLayer_) {
        if (!motions.removeLayer(*insertedLayer_)) {
            return false;
        }
        insertedLayer_.reset();
        std::cout << "추가 크기 레이어 삭제\n";
        return true;
    }
    const auto scale = graphics::TransformSystem::scaling(glm::vec3(1.5f));
    if (!normalSpinLayer_ || !scale) {
        return false;
    }
    graphics::LayerConfig config;
    config.space = graphics::TransformSpace::Local;
    config.transform = *scale;
    insertedLayer_ = motions.insertLayer(*normalSpinLayer_, config);
    if (!insertedLayer_) {
        return false;
    }
    std::cout << "첫 객체에 1.5배 Local 크기 레이어 삽입\n";
    return true;
}

void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }
    if(key==GLFW_KEY_7 || key==GLFW_KEY_8 || key==GLFW_KEY_0) {
        const bool ok=key==GLFW_KEY_7?initGrid():key==GLFW_KEY_8?initRoom():initScene();
        if(!ok) app_.requestClose();return;
    }
    if(key==GLFW_KEY_V) {threeViews_=!threeViews_;return;}
    if(key==GLFW_KEY_D) {trace_=!trace_;graphics::Diagnostics::configure({true,trace_});return;}
    if(key==GLFW_KEY_F && demo_==Demo::Room) {floorOpen_=!floorOpen_;return;}
    if(demo_!=Demo::Motion) {
        if(key==GLFW_KEY_SPACE) clock_.playing=!clock_.playing;
        else if(key==GLFW_KEY_ESCAPE) app_.requestClose();
        else if(key==GLFW_KEY_H) printHelp();
        return;
    }
    auto& motions = app_.getScene().getMotions();
    bool ok = true;
    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        ok = choosePath(key - GLFW_KEY_1 + 1);
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;
        ok = pathId_ && spinId_ && globalId_
            && motions.setMotionPlaying(*pathId_, playing)
            && motions.setMotionPlaying(*spinId_, playing)
            && motions.setMotionPlaying(*globalId_, playing);
        if (ok) {
            paused_ = !paused_;
            std::cout << (paused_ ? "시간 정지\n" : "시간 재개\n");
        }
    } else if (key == GLFW_KEY_S) {
        ok = spinId_ && motions.setMotionEnabled(*spinId_, !spinEnabled_);
        if (ok) {
            spinEnabled_ = !spinEnabled_;
            std::cout << "자전 기여: " << spinEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_G) {
        ok = globalId_ && motions.setMotionEnabled(*globalId_, !globalEnabled_);
        if (ok) {
            globalEnabled_ = !globalEnabled_;
            std::cout << "전체 공전 기여: " << globalEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_M) {
        ok = changeReflection();
    } else if (key == GLFW_KEY_I) {
        ok = toggleInsertedLayer();
    } else if (key == GLFW_KEY_H) {
        printHelp();
    }
    if (!ok) {
        std::cerr << "샘플 명령 적용 실패. 장면을 종료합니다.\n";
        app_.requestClose();
    }
}

void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }
    std::optional<graphics::PickResult> hit;
    if(threeViews_) {
        const auto size=app_.getFramebufferSize();
        if(size[0]<3 || size[1]<2) return;
        const auto passes=graphics::PatternSystem::threeViews(app_.getCamera(),size[0],size[1],app_.getRenderOptions());
        for(const auto& pass:passes) {
            const auto& v=pass.viewport;const double top=size[1]-v.y-v.height;
            if(event.framebufferX>=v.x && event.framebufferX<v.x+v.width && event.framebufferY>=top && event.framebufferY<top+v.height)
                hit=graphics::pick(app_.getScene(),pass.camera,event.framebufferX,event.framebufferY,{double(v.x),top,double(v.width),double(v.height)});
        }
    } else hit=app_.pickAt(event.windowX,event.windowY);
    if (hit) {
        std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    }
}

void Console::printHelp() const {
    std::cout << "standard: 프레임워크 Motion·반사·레이어 샘플\n"
              << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
              << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
              << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
              << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
              << "왼쪽 클릭: 물체의 entity/face ID 출력\n"
              << "7 Grid/거리 기반 배치 / 8 실내 미끄럼·낙하 / 0 기존 Motion 예제\n"
              << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

bool Console::initGrid() {
    auto& scene=app_.getScene();scene.clear();demo_=Demo::Grid;clock_.reset();gridBoxes_.clear();
    graphics::GridLayout grid;grid.origin={-3,-.1f,-2};grid.extent={6,.2f,4};grid.cells={6,1,4};
    for(int z=0;z<4;++z) for(int x=0;x<6;++x) {
        const auto center=grid.cellCenter({x,0,z}),size=grid.relativeSize({.15f,1,.22f});
        if(!center || !size || !graphics::PatternSystem::addBox(scene,*size,*center,(x+z)%2?graphics::GlColor{.25f,.3f,.4f}:graphics::GlColor{.4f,.45f,.55f})) return false;
    }
    if(!distancePath_.setPoints({{-2.5f,.35f,-1.5f},{2.5f,.35f,-1.5f},{2.5f,.35f,1.5f}})) return false;
    for(unsigned i=0;i<5;++i) {
        const auto box=graphics::PatternSystem::addBox(scene,{.4f,.5f,.4f},{0,0,0},{.25f+.14f*i,.7f-.1f*i,.3f});
        if(!box || !graphics::PatternSystem::addNumber(scene,i+1,{-.07f,-.1f,.205f},.2f,{1,1,1},*box)) return false;
        gridBoxes_.push_back(*box);
    }
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={7,6,8};app_.getCamera().target={0,0,0};
    app_.getCamera().orthographicHalfExtent=4;return true;
}
bool Console::initRoom() {
    auto& scene=app_.getScene();scene.clear();demo_=Demo::Room;clock_.reset();floorOpen_=false;
    const auto room=graphics::PatternSystem::addRoom(scene,{{-2,-1,-2},{2,2,2}});
    const auto cube=graphics::PatternSystem::addBox(scene,{.5f,.5f,.5f},{0,-.75f,0},{1,.7f,.2f});
    if(!room || !cube) return false;room_=*room;roomCube_=*cube;
    roomBody_=graphics::RoomBody{};roomBody_.localPosition={0,-.75f,0};roomBody_.halfExtent={.25f,.25f,.25f};
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={5,3,8};app_.getCamera().target={0,0,0};
    return true;
}
void Console::onUpdate(float dt) {
    if(demo_==Demo::Motion || !clock_.playing) return;
    if(!clock_.advance(dt)) {app_.requestClose();return;}
    auto& scene=app_.getScene();bool ok=true;
    if(demo_==Demo::Grid) {
        for(std::size_t i=0;i<gridBoxes_.size();++i)
            ok=graphics::PatternSystem::follow(scene,gridBoxes_[i],distancePath_,clock_.seconds+distancePath_.length()*i/5)&&ok;
    } else {
        const float angle=.4f*float(std::sin(clock_.seconds));
        const auto matrix=glm::rotate(glm::mat4(1),angle,glm::vec3(0,0,1));
        graphics::Transform3D roomTransform;roomTransform.rotation=glm::mat3(matrix);roomTransform.basis=glm::vec3(matrix*glm::vec4(0,.5f,0,1));
        ok=scene.setTransform(room_,roomTransform) && scene.setFaceVisible(room_,5,!floorOpen_)
            && graphics::CollisionSystem::stepRoomBody(roomBody_,{{-2,-1,-2},{2,2,2}},matrix,dt,floorOpen_,false,{0,-9.8f,0},{0,0,.4f*float(std::cos(clock_.seconds))});
        graphics::Transform3D cubeTransform;cubeTransform.basis=roomBody_.position(matrix);
        if(!roomBody_.released) cubeTransform.rotation=glm::mat3(matrix);
        ok=scene.setTransform(roomCube_,cubeTransform)&&ok;
    }
    if(!ok) app_.requestClose();
}
bool Console::onRender() {
    if(!threeViews_) return app_.renderScene();
    const auto size=app_.getFramebufferSize();if(size[0]<3 || size[1]<2) return true;
    return app_.renderViews(graphics::PatternSystem::threeViews(app_.getCamera(),size[0],size[1],app_.getRenderOptions()));
}
```
### after
```cpp
#include "Console.h"
#include <iostream>

/// 선택한 경로로 이동 모션을 바꾼다.
bool Console::choosePath(int kind) {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);

    if (kind == 1) {
        path.path = graphics::PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 2) {
        path.path = graphics::PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 3) {
        path.path = graphics::PathSystem::polygon({
            {1, -.5f, 0},
            {2.4f, -.4f, 0},
            {2.1f, .6f, 0},
            {1.4f, .8f, 0},
            {.9f, .2f, 0}
        });
    } else if (kind == 4) {
        path.path = [center](double u) {
            const double angle = u * 6.283185307179586;
            return center + glm::vec3(
                float(.7 * std::sin(2 * angle)),
                float(.6 * std::sin(angle)),
                float(.3 * std::cos(angle)));
        };
        path.interpolation = graphics::PathSystem::smoothStep;
    } else if (kind == 5) {
        path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    } else if (kind == 6) {
        path.path = graphics::PathSystem::polyline({
            {1, -.5f, 0},
            {2.4f, -.5f, 0},
            {2, .6f, 0}
        });
        path.playback = graphics::Playback::PingPong;
    } else {
        return false;
    }

    graphics::Motion motion;
    auto& motions = app_.getScene().getMotions();
    if (!pathId_ || !motion.append(graphics::Motion::followPath(path))) {
        return false;
    }
    if (!motions.setMotion(*pathId_, motion) || !motions.seekMotion(*pathId_, 0)) {
        return false;
    }
    std::cout << "경로 " << kind << ": 시작 시각 0초로 변경\n";
    return true;
}

/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    demo_=Demo::Motion;
    auto& scene = app_.getScene();
    scene.clear();
    paused_ = false;
    spinEnabled_ = true;
    globalEnabled_ = false;
    reflection_ = 1;
    insertedLayer_.reset();

    const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
    const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
    if (!first || !second) {
        return false;
    }
    if (!scene.setTint(*first, {1, .35f, .2f})
        || !scene.setTint(*second, {.2f, .7f, 1})) {
        return false;
    }

    auto& motions = scene.getMotions();
    pathId_ = motions.createMotion(graphics::Motion{});
    graphics::RotationMotion spin;
    spin.degreesPerSecond = 90;
    spinId_ = motions.createSpin(spin);
    graphics::RotationMotion global;
    global.degreesPerSecond = 15;
    globalId_ = motions.createOrbit(global);
    if (!pathId_ || !spinId_ || !globalId_ || !choosePath(1)) {
        return false;
    }
    if (!motions.setMotionEnabled(*globalId_, false)
        || !motions.bindMotion(graphics::MotionSystem::rootLayer(), *globalId_)) {
        return false;
    }

    const auto normalPath = motions.pushLayer(graphics::MotionSystem::rootLayer());
    const auto reflection = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
    if (!normalPath || !reflection) {
        return false;
    }
    graphics::LayerConfig mirror;
    mirror.transform = *reflection;
    mirrorLayer_ = motions.pushLayer(graphics::MotionSystem::rootLayer(), mirror);
    if (!mirrorLayer_) {
        return false;
    }
    const auto mirroredPath = motions.pushLayer(*mirrorLayer_);
    normalSpinLayer_ = motions.pushLayer(*normalPath);
    if (!mirroredPath || !normalSpinLayer_) {
        return false;
    }
    const auto mirroredSpin = motions.pushLayer(*mirroredPath);
    if (!mirroredSpin
        || !motions.bindMotion(*normalPath, *pathId_)
        || !motions.bindMotion(*mirroredPath, *pathId_)
        || !motions.bindMotion(*normalSpinLayer_, *spinId_)
        || !motions.bindMotion(*mirroredSpin, *spinId_)
        || !scene.bindEntity(*first, *normalSpinLayer_)
        || !scene.bindEntity(*second, *mirroredSpin)) {
        return false;
    }
    if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
        || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
        || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) {
        return false;
    }

    auto& camera = app_.getCamera();
    camera.position = {5, 3, 8};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    printHelp();
    return true;
}

/// 반사 변환을 순서대로 변경한다.
bool Console::changeReflection() {
    const int next = (reflection_ + 1) % 4;
    std::optional<glm::mat4> matrix = glm::mat4(1);
    if (next == 1) {
        matrix = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
    } else if (next == 2) {
        matrix = graphics::TransformSystem::reflectPoint({0, 0, 0});
    } else if (next == 3) {
        matrix = graphics::TransformSystem::reflectLine(graphics::AxisLine{});
    }
    if (!mirrorLayer_ || !matrix) {
        return false;
    }
    graphics::LayerConfig config;
    config.transform = *matrix;
    if (!app_.getScene().getMotions().setLayer(*mirrorLayer_, config)) {
        return false;
    }
    reflection_ = next;
    const char* names[] = {"없음", "YZ평면 반사", "원점 점대칭", "Y축 직선대칭"};
    std::cout << "반사: " << names[reflection_] << '\n';
    return true;
}

/// 크기 레이어를 삽입하거나 제거한다.
bool Console::toggleInsertedLayer() {
    auto& motions = app_.getScene().getMotions();
    if (insertedLayer_) {
        if (!motions.removeLayer(*insertedLayer_)) {
            return false;
        }
        insertedLayer_.reset();
        std::cout << "추가 크기 레이어 삭제\n";
        return true;
    }
    const auto scale = graphics::TransformSystem::scaling(glm::vec3(1.5f));
    if (!normalSpinLayer_ || !scale) {
        return false;
    }
    graphics::LayerConfig config;
    config.space = graphics::TransformSpace::Local;
    config.transform = *scale;
    insertedLayer_ = motions.insertLayer(*normalSpinLayer_, config);
    if (!insertedLayer_) {
        return false;
    }
    std::cout << "첫 객체에 1.5배 Local 크기 레이어 삽입\n";
    return true;
}

/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }
    if(key==GLFW_KEY_7 || key==GLFW_KEY_8 || key==GLFW_KEY_0) {
        const bool ok=key==GLFW_KEY_7?initGrid():key==GLFW_KEY_8?initRoom():initScene();
        if(!ok) app_.requestClose();return;
    }
    if(key==GLFW_KEY_V) {threeViews_=!threeViews_;return;}
    if(key==GLFW_KEY_D) {trace_=!trace_;graphics::Diagnostics::configure({true,trace_});return;}
    if(key==GLFW_KEY_F && demo_==Demo::Room) {floorOpen_=!floorOpen_;return;}
    if(demo_!=Demo::Motion) {
        if(key==GLFW_KEY_SPACE) clock_.playing=!clock_.playing;
        else if(key==GLFW_KEY_ESCAPE) app_.requestClose();
        else if(key==GLFW_KEY_H) printHelp();
        return;
    }
    auto& motions = app_.getScene().getMotions();
    bool ok = true;
    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        ok = choosePath(key - GLFW_KEY_1 + 1);
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;
        ok = pathId_ && spinId_ && globalId_
            && motions.setMotionPlaying(*pathId_, playing)
            && motions.setMotionPlaying(*spinId_, playing)
            && motions.setMotionPlaying(*globalId_, playing);
        if (ok) {
            paused_ = !paused_;
            std::cout << (paused_ ? "시간 정지\n" : "시간 재개\n");
        }
    } else if (key == GLFW_KEY_S) {
        ok = spinId_ && motions.setMotionEnabled(*spinId_, !spinEnabled_);
        if (ok) {
            spinEnabled_ = !spinEnabled_;
            std::cout << "자전 기여: " << spinEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_G) {
        ok = globalId_ && motions.setMotionEnabled(*globalId_, !globalEnabled_);
        if (ok) {
            globalEnabled_ = !globalEnabled_;
            std::cout << "전체 공전 기여: " << globalEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_M) {
        ok = changeReflection();
    } else if (key == GLFW_KEY_I) {
        ok = toggleInsertedLayer();
    } else if (key == GLFW_KEY_H) {
        printHelp();
    }
    if (!ok) {
        std::cerr << "샘플 명령 적용 실패. 장면을 종료합니다.\n";
        app_.requestClose();
    }
}

/// 마우스 버튼 입력을 처리한다.
void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }
    std::optional<graphics::PickResult> hit;
    if(threeViews_) {
        const auto size=app_.getFramebufferSize();
        if(size[0]<3 || size[1]<2) return;
        const auto passes=graphics::PatternSystem::threeViews(app_.getCamera(),size[0],size[1],app_.getRenderOptions());
        for(const auto& pass:passes) {
            const auto& v=pass.viewport;const double top=size[1]-v.y-v.height;
            if(event.framebufferX>=v.x && event.framebufferX<v.x+v.width && event.framebufferY>=top && event.framebufferY<top+v.height)
                hit=graphics::pick(app_.getScene(),pass.camera,event.framebufferX,event.framebufferY,{double(v.x),top,double(v.width),double(v.height)});
        }
    } else hit=app_.pickAt(event.windowX,event.windowY);
    if (hit) {
        std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    }
}

/// 조작 방법을 출력한다.
void Console::printHelp() const {
    std::cout << "standard: 프레임워크 Motion·반사·레이어 샘플\n"
              << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
              << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
              << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
              << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
              << "왼쪽 클릭: 물체의 entity/face ID 출력\n"
              << "7 Grid/거리 기반 배치 / 8 실내 미끄럼·낙하 / 0 기존 Motion 예제\n"
              << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

/// 거리 기반 격자 예제를 구성한다.
bool Console::initGrid() {
    auto& scene=app_.getScene();scene.clear();demo_=Demo::Grid;clock_.reset();gridBoxes_.clear();
    graphics::GridLayout grid;grid.origin={-3,-.1f,-2};grid.extent={6,.2f,4};grid.cells={6,1,4};
    for(int z=0;z<4;++z) for(int x=0;x<6;++x) {
        const auto center=grid.cellCenter({x,0,z}),size=grid.relativeSize({.15f,1,.22f});
        if(!center || !size || !graphics::PatternSystem::addBox(scene,*size,*center,(x+z)%2?graphics::GlColor{.25f,.3f,.4f}:graphics::GlColor{.4f,.45f,.55f})) return false;
    }
    if(!distancePath_.setPoints({{-2.5f,.35f,-1.5f},{2.5f,.35f,-1.5f},{2.5f,.35f,1.5f}})) return false;
    for(unsigned i=0;i<5;++i) {
        const auto box=graphics::PatternSystem::addBox(scene,{.4f,.5f,.4f},{0,0,0},{.25f+.14f*i,.7f-.1f*i,.3f});
        if(!box || !graphics::PatternSystem::addNumber(scene,i+1,{-.07f,-.1f,.205f},.2f,{1,1,1},*box)) return false;
        gridBoxes_.push_back(*box);
    }
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={7,6,8};app_.getCamera().target={0,0,0};
    app_.getCamera().orthographicHalfExtent=4;return true;
}
/// 회전하는 방 예제를 구성한다.
bool Console::initRoom() {
    auto& scene=app_.getScene();scene.clear();demo_=Demo::Room;clock_.reset();floorOpen_=false;
    const auto room=graphics::PatternSystem::addRoom(scene,{{-2,-1,-2},{2,2,2}});
    const auto cube=graphics::PatternSystem::addBox(scene,{.5f,.5f,.5f},{0,-.75f,0},{1,.7f,.2f});
    if(!room || !cube) return false;room_=*room;roomCube_=*cube;
    roomBody_=graphics::RoomBody{};roomBody_.localPosition={0,-.75f,0};roomBody_.halfExtent={.25f,.25f,.25f};
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={5,3,8};app_.getCamera().target={0,0,0};
    return true;
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    if(demo_==Demo::Motion || !clock_.playing) return;
    if(!clock_.advance(dt)) {app_.requestClose();return;}
    auto& scene=app_.getScene();bool ok=true;
    if(demo_==Demo::Grid) {
        for(std::size_t i=0;i<gridBoxes_.size();++i)
            ok=graphics::PatternSystem::follow(scene,gridBoxes_[i],distancePath_,clock_.seconds+distancePath_.length()*i/5)&&ok;
    } else {
        const float angle=.4f*float(std::sin(clock_.seconds));
        const auto matrix=glm::rotate(glm::mat4(1),angle,glm::vec3(0,0,1));
        graphics::Transform3D roomTransform;roomTransform.rotation=glm::mat3(matrix);roomTransform.basis=glm::vec3(matrix*glm::vec4(0,.5f,0,1));
        ok=scene.setTransform(room_,roomTransform) && scene.setFaceVisible(room_,5,!floorOpen_)
            && graphics::CollisionSystem::stepRoomBody(roomBody_,{{-2,-1,-2},{2,2,2}},matrix,dt,floorOpen_,false,{0,-9.8f,0},{0,0,.4f*float(std::cos(clock_.seconds))});
        graphics::Transform3D cubeTransform;cubeTransform.basis=roomBody_.position(matrix);
        if(!roomBody_.released) cubeTransform.rotation=glm::mat3(matrix);
        ok=scene.setTransform(roomCube_,cubeTransform)&&ok;
    }
    if(!ok) app_.requestClose();
}
/// 현재 장면을 지정한 뷰로 렌더링한다.
bool Console::onRender() {
    if(!threeViews_) return app_.renderScene();
    const auto size=app_.getFramebufferSize();if(size[0]<3 || size[1]<2) return true;
    return app_.renderViews(graphics::PatternSystem::threeViews(app_.getCamera(),size[0],size[1],app_.getRenderOptions()));
}
```

## standard/main.cpp
### before
```cpp
#include "Console.h"

int main() {
    graphics::Application app;
    Console sample(app);

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setKeyHandler([&](graphics::Application&, int key, int action) {
        sample.onKey(key, action);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt) {sample.onUpdate(dt);});
    app.setRenderHandler([&](graphics::Application&) {return sample.onRender();});

    graphics::ApplicationConfig config;
    config.title = "standard - Motion, Grid, diagnostics and render passes";
    return app.run(config);
}
```
### after
```cpp
#include "Console.h"

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    graphics::Application app;
    Console sample(app);

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setKeyHandler([&](graphics::Application&, int key, int action) {
        sample.onKey(key, action);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt) {sample.onUpdate(dt);});
    app.setRenderHandler([&](graphics::Application&) {return sample.onRender();});

    graphics::ApplicationConfig config;
    config.title = "standard - Motion, Grid, diagnostics and render passes";
    return app.run(config);
}
```
