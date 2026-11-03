#include "../Console.h"

/// 회전하는 방 예제를 구성한다.
bool Console::initRoom() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Room;
    clock_.reset();
    floorOpen_ = false;

    const auto room = graphics::PatternSystem::addRoom(scene, {{-2, -1, -2}, {2, 2, 2}});
    const auto cube = graphics::PatternSystem::addBox(scene, {.5f, .5f, .5f}, {0, -.75f, 0}, {1, .7f, .2f});

    if (!room || !cube)
        return false;
    room_ = *room;
    roomCube_ = *cube;
    roomBody_ = graphics::RoomBody{};
    roomBody_.localPosition = {0, -.75f, 0};
    roomBody_.halfExtent = {.25f, .25f, .25f};
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {5, 3, 8};
    app_.getCamera().target = {0, 0, 0};

    return true;
}

bool Console::updateRoom(float dt) {
    auto& scene = app_.getScene();
    bool ok = true;
        const float angle = .4f * float(std::sin(clock_.seconds));
        const auto matrix = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));
        graphics::Transform3D roomTransform;

        roomTransform.rotation = glm::mat3(matrix);
        roomTransform.basis = glm::vec3(matrix * glm::vec4(0, .5f, 0, 1));
        ok =
            scene.setTransform(room_, roomTransform) && scene.setFaceVisible(room_, 5, !floorOpen_) &&
            graphics::CollisionSystem::stepRoomBody(roomBody_, {{-2, -1, -2}, {2, 2, 2}}, matrix, dt, floorOpen_, false,
                                                    {0, -9.8f, 0}, {0, 0, .4f * float(std::cos(clock_.seconds))});

        graphics::Transform3D cubeTransform;

        cubeTransform.basis = roomBody_.position(matrix);

        if (!roomBody_.released)
            cubeTransform.rotation = glm::mat3(matrix);
        ok = scene.setTransform(roomCube_, cubeTransform) && ok;
    return ok;
}
