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
