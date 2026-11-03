#include "../Console.h"

bool Console::initRoom() {
    auto& scene = app_.getScene();
    scene.clear();
    demo_ = Demo::Room;
    floorOpen_ = false;
    const auto frame = scene.addGroup(), bodyFrame = scene.addGroup();
    const auto room = graphics::PatternSystem::addRoom(scene, {{-2, -1, -2}, {2, 2, 2}});
    if (!frame || !bodyFrame || !room || !scene.setParent(*room, *frame))
        return false;
    const auto cube = graphics::PatternSystem::addBox(scene, {.5f, .5f, .5f}, {0, 0, 0}, {1, .7f, .2f}, *bodyFrame);
    if (!cube)
        return false;
    room_ = *room;
    roomCube_ = *cube;
    roomFrame_ = *frame;
    bodyFrame_ = *bodyFrame;
    roomControl_ = jointControls_.size();
    if (!addControl(roomFrame_, "room", ControlKind::RoomFrame))
        return false;
    bodyControl_ = jointControls_.size();
    if (!addControl(roomCube_, "physical box", ControlKind::RoomBody))
        return false;
    roomBody_ = graphics::RoomBody{};
    roomBody_.localPosition = {0, -.75f, 0};
    releasedRotation_ = glm::mat3(1);
    app_.getCamera().position = {5, 3, 8};
    app_.getCamera().target = {0, 0, 0};
    return true;
}

bool Console::updateRoom(float dt) {
    auto& scene = app_.getScene();
    if (!roomControl_ || !bodyControl_)
        return false;
    const auto& manual = animation_[jointControls_[*roomControl_].track].rest;
    const auto& box = animation_[jointControls_[*bodyControl_].track].rest;
    const float angle = .4f * float(std::sin(clock_.seconds));
    const auto automatic = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));
    const auto matrix = graphics::DiagramSystem::modelMatrix(manual) * automatic;
    graphics::Transform3D roomTransform;
    roomTransform.rotation = glm::mat3(automatic);
    roomTransform.basis = glm::vec3(automatic * glm::vec4(0, .5f, 0, 1));
    const glm::vec3 radius(.25f * box.scale.x);
    roomBody_.halfExtent = glm::abs(box.rotation[0]) * radius.x + glm::abs(box.rotation[1]) * radius.y +
                          glm::abs(box.rotation[2]) * radius.z;
    if (!roomBody_.released) {
        const auto low = glm::vec3(-2, -1, -2) + roomBody_.halfExtent;
        const auto high = glm::vec3(2, 2, 2) - roomBody_.halfExtent;
        roomBody_.localPosition.x = std::clamp(roomBody_.localPosition.x, low.x, high.x);
        roomBody_.localPosition.z = std::clamp(roomBody_.localPosition.z, low.z, high.z);
        roomBody_.localPosition.y = floorOpen_ ? std::min(roomBody_.localPosition.y, high.y)
                                             : std::clamp(roomBody_.localPosition.y, low.y, high.y);
        releasedRotation_ = glm::mat3(matrix);
    }
    const auto angularVelocity = manual.rotation * glm::vec3(0, 0, clock_.playing ? .4f * float(std::cos(clock_.seconds)) : 0);
    if (!scene.setTransform(room_, roomTransform) || !scene.setFaceVisible(room_, 5, !floorOpen_) ||
        !graphics::CollisionSystem::stepRoomBody(roomBody_, {{-2, -1, -2}, {2, 2, 2}}, matrix, dt, floorOpen_,
                                                  false, {0, -9.8f, 0}, angularVelocity))
        return false;
    graphics::Transform3D bodyTransform;
    bodyTransform.basis = roomBody_.position(matrix);
    bodyTransform.rotation = releasedRotation_;
    return scene.setTransform(bodyFrame_, bodyTransform);
}
