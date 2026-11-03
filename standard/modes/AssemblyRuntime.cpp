#include "Assembly.h"
#include <algorithm>
#include <limits>

namespace assembly {
namespace {
graphics::Bounds3 bounds(const graphics::Scene& scene, graphics::EntityId mesh) {
    const auto* component = scene.getComponent(mesh);
    const auto matrix = scene.worldMatrix(mesh);
    require(component && matrix, "경계 계산에 필요한 메시/행렬이 없습니다.");
    graphics::Bounds3 box{glm::vec3(std::numeric_limits<float>::max()), glm::vec3(std::numeric_limits<float>::lowest())};
    for (const auto& face : component->polyhedron.faces)
        for (const auto& vertex : face.localVertices) {
            const auto p = glm::vec3(*matrix * glm::vec4(vertex, 1));
            box.minimum = glm::min(box.minimum, p);
            box.maximum = glm::max(box.maximum, p);
        }
    require(box.valid(), "양수 크기의 메시 경계가 필요합니다.");
    return box;
}
}

bool Assembly::applyPoses(float dt) {
    auto& scene = app_.getScene();
    const auto motionFor = [&](const std::string& name) {
        graphics::Motion motion;
        glm::mat3 parentRotation(1);
        for (const auto& b : state_.behaviors) {
            if (!b.enabled || b.target != name || b.effect != Effect::Motion || b.parentSpin) continue;
            const auto sample = b.motion.sample(b.seconds);
            require(sample.has_value(), "부모 공간 모션 샘플 실패: " + b.name);
            parentRotation = glm::mat3(sample->parent) * parentRotation;
        }
        for (const auto& b : state_.behaviors) {
            if (!b.enabled || b.target != name || b.effect != Effect::Motion)
                continue;
            const auto sample = b.motion.sample(b.seconds);
            require(sample.has_value(), "Motion 샘플 실패: " + b.name);
            const auto& rest = node(name).rest;
            const auto pivot = glm::translate(glm::mat4(1), rest.localBasis);
            const auto inversePivot = glm::translate(glm::mat4(1), -rest.localBasis);
            auto parent = sample->parent, local = sample->local;
            if (b.parentSpin) {
                const auto rotation = glm::mat4(parentRotation * rest.rotation);
                local = glm::transpose(rotation) * parent * rotation;
                parent = glm::mat4(1);
            }
            require(motion.append(graphics::Motion::fixed(parent, graphics::TransformSpace::World)) &&
                    motion.append(graphics::Motion::fixed(pivot * local * inversePivot, graphics::TransformSpace::Local)),
                    "Motion 합성 실패: " + b.name);
        }
        return motion;
    };
    std::vector<graphics::JointAnimation> tracks;
    for (const auto& item : state_.nodes) {
        auto* driver = positionDriver(item.first);
        if (driver && (driver->effect == Effect::Body || driver->effect == Effect::Walk || driver->effect == Effect::Custom))
            continue;
        if (item.second.affineGroup) {
            require(scene.setTransform(item.second.id, item.second.rest), "정적 warp 그룹 적용 실패.");
            continue;
        }
        tracks.push_back({item.second.id, item.second.rest, motionFor(item.first)});
    }
    require(graphics::AnimationSystem::poseHierarchy(scene, tracks, 0), "기준 자세와 동작 합성 실패.");
    for (int phase = 0; phase < 4; ++phase) {
    for (auto& b : state_.behaviors) {
        const int order = b.effect == Effect::Body ? 1 : b.effect == Effect::Walk ? 2 : b.effect == Effect::Custom ? 3 : 0;
        if (phase != order)
            continue;
        auto& target = node(b.target);
        if (b.effect == Effect::Conveyor) {
            if (!b.enabled) continue;
            const double length = 6.2 + 1.5707963267948966 * .8 + 4.2;
            const auto pose = graphics::PatternSystem::conveyorCornerPose(b.seconds * b.rate + b.phase * length,
                {-4, 1, -2}, {2.2f, 1, -1.2f}, .8f, 3);
            require(pose.has_value(), "컨베이어 경로 계산 실패.");
            auto rest = target.rest;
            const auto turn = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(pose->yawDegrees), glm::vec3(0, 1, 0)));
            rest.basis = pose->position + glm::vec3(0, b.height, 0) + turn * rest.basis;
            rest.rotation = turn * rest.rotation;
            require(graphics::AnimationSystem::poseHierarchy(scene, {{target.id, rest, motionFor(b.target)}}, 0), "컨베이어 자세 적용 실패.");
        } else if (faceEffect(b.effect)) {
            require(target.mesh.has_value(), "면 동작에는 메시가 필요합니다.");
            if (!b.enabled) {
                require(scene.setFaceLocalMatrix(*target.mesh, b.faceIndex, b.initialFaceMatrix, b.initialFaceSpace) &&
                        scene.setFaceVisible(*target.mesh, b.faceIndex, b.initialFaceVisible), "면 기준값 복원 실패.");
                continue;
            }
            if (b.effect == Effect::FaceTrack) {
                const auto sample = b.motion.sample(b.seconds);
                require(sample.has_value(), "면 트랙 샘플 실패.");
                const auto matrix = sample->parent * b.faceTrackBase * sample->local;
                require(std::abs(glm::determinant(glm::mat3(matrix))) > 1e-18f &&
                        scene.setFaceLocalMatrix(*target.mesh, b.faceIndex, matrix, graphics::FaceAnimationSpace::Model),
                        "면 트랙이 특이하거나 유효하지 않습니다.");
            } else if (b.effect == Effect::Face) {
                auto config = b.face;
                config.degrees = b.enabled ? b.seconds * b.rate : 0;
                require(graphics::PatternSystem::rotateFace(scene, *target.mesh, b.faceIndex, config), "면 회전 실패.");
            } else {
                const float u = b.enabled ? float(std::clamp(b.seconds / b.rate, 0.0, 1.0)) : 0;
                require(graphics::PatternSystem::openRoomWall(scene, *target.mesh, b.faceIndex, u), "벽 개방 실패.");
            }
        } else if (b.effect == Effect::Custom) {
            const float stepTime = b.enabled && b.playing && !state_.paused ? dt : 0;
            if (b.enabled) require(b.custom && b.custom(scene, target.rest, b.seconds, stepTime), "C++ 사용자 동작 실패: " + b.name);
        } else if (b.effect == Effect::Body) {
            const auto& room = node(b.reference);
            const auto matrix = scene.worldMatrix(room.id);
            require(room.room && matrix && rigid(*matrix), "물리 방은 강체여야 합니다.");
            const auto* walls = room.mesh ? scene.getComponent(*room.mesh) : nullptr;
            require(walls && walls->polyhedron.faces.size() > 5, "방 바닥이 없습니다.");
            const bool open = !walls->polyhedron.faces[5].visible;
            const auto* mesh = target.mesh ? scene.getComponent(*target.mesh) : nullptr;
            require(mesh != nullptr, "물리 상자 메시가 없습니다.");
            glm::vec3 half(0);
            for (const auto& face : mesh->polyhedron.faces)
                for (const auto& vertex : face.localVertices)
                    half = glm::max(half, glm::abs(target.rest.rotation * (vertex * target.rest.scale)));
            if (target.kind == "sphere") {
                float radius = 0;
                for (const auto& face : mesh->polyhedron.faces)
                    for (const auto& vertex : face.localVertices) radius = std::max(radius, glm::length(vertex));
                half = glm::vec3(radius * target.rest.scale.x);
            }
            b.body.halfExtent = half;
            if (!b.body.released) {
                b.releasedRotation = glm::mat3(*matrix);
                const auto low = room.room->minimum + half, high = room.room->maximum - half;
                require(glm::all(glm::lessThan(low, high)), "상자가 방보다 큽니다.");
                b.body.localPosition.x = std::clamp(b.body.localPosition.x, low.x, high.x);
                b.body.localPosition.z = std::clamp(b.body.localPosition.z, low.z, high.z);
                b.body.localPosition.y = open ? std::min(b.body.localPosition.y, high.y)
                                              : std::clamp(b.body.localPosition.y, low.y, high.y);
            }
            glm::vec3 omega(0);
            if (dt > 0) {
                const auto previous = glm::mat3(b.previousRoom), current = glm::mat3(*matrix);
                omega = (glm::cross(previous[0], current[0]) + glm::cross(previous[1], current[1]) +
                         glm::cross(previous[2], current[2])) / (2 * dt);
            }
            if (dt > 0) b.angularVelocity = omega;
            if (b.releaseRequested && !b.body.released) {
                b.body.worldPosition = b.body.position(*matrix);
                b.body.worldVelocity = glm::mat3(*matrix) * b.body.localVelocity +
                    glm::cross(b.angularVelocity, b.body.worldPosition - glm::vec3((*matrix)[3]));
                b.body.released = true;
            }
            b.releaseRequested = false;
            require(graphics::CollisionSystem::stepRoomBody(b.body, *room.room, *matrix,
                b.enabled && b.playing && !state_.paused ? dt : 0, open, b.elastic, b.gravity, b.angularVelocity), "방 충돌 계산 실패.");
            b.previousRoom = *matrix;
            auto rest = target.rest;
            rest.basis = b.body.position(*matrix);
            rest.rotation = b.releasedRotation * rest.rotation;
            require(scene.setTransform(target.id, rest), "물리 상자 자세 적용 실패.");
        } else if (b.effect == Effect::Walk) {
            const auto& room = node(b.reference);
            const auto matrix = scene.worldMatrix(room.id);
            require(room.room && target.walker && matrix && rigid(*matrix), "보행 리그와 방이 필요합니다.");
            const auto r = glm::mat3(*matrix);
            require(glm::length(r[0] - glm::vec3(1, 0, 0)) < .001f && glm::length(r[1] - glm::vec3(0, 1, 0)) < .001f,
                    "보행 공간은 월드 축 정렬을 유지해야 합니다.");
            const auto offset = glm::vec3((*matrix)[3]);
            const graphics::Bounds3 stage{room.room->minimum + offset, room.room->maximum + offset};
            std::vector<graphics::Bounds3> obstacles;
            for (const auto& obstacle : b.obstacles) {
                const auto& object = node(obstacle);
                require(object.mesh.has_value(), "장애물 메시가 없습니다.");
                obstacles.push_back(bounds(scene, *object.mesh));
            }
            const auto* walls = room.mesh ? scene.getComponent(*room.mesh) : nullptr;
            require(walls && walls->polyhedron.faces.size() > 5, "보행 공간의 바닥이 없습니다.");
            b.released = b.released || !walls->polyhedron.faces[5].visible;
            const float stepTime = b.enabled && b.playing && !state_.paused ? dt : 0;
            if (b.released) {
                const auto delta = b.character.direction * b.character.speed * stepTime;
                b.character.verticalSpeed -= 9.8f * stepTime;
                b.character.feet += delta + glm::vec3(0, b.character.verticalSpeed * stepTime, 0);
                b.character.walkDistance += glm::length(glm::vec2(delta.x, delta.z));
                b.character.grounded = false;
            } else {
                const auto step = graphics::CollisionSystem::stepCharacter(b.character, stage, obstacles, stepTime);
                require(step.has_value(), "보행 충돌 계산 실패.");
            }
            const float yaw = glm::degrees(std::atan2(b.character.direction.x, b.character.direction.z));
            require(graphics::PatternSystem::poseWalkingRig(scene, *target.walker, b.character.feet, yaw,
                b.character.walkDistance, b.character.speed), "보행 자세 적용 실패.");
            for (const auto& part : state_.nodes) {
                const auto id = part.second.id;
                const auto& rig = *target.walker;
                if (id != rig.head && id != rig.shoulders[0] && id != rig.shoulders[1] && id != rig.hips[0] && id != rig.hips[1])
                    continue;
                auto rest = scene.getComponent(id)->transform;
                const auto matrix = graphics::DiagramSystem::modelMatrix(rest) *
                    glm::inverse(graphics::DiagramSystem::modelMatrix(part.second.initial)) *
                    graphics::DiagramSystem::modelMatrix(part.second.rest);
                rest.scale = part.second.rest.scale;
                rest.localBasis = part.second.rest.localBasis;
                rest.rotation = glm::mat3(matrix) / rest.scale.x;
                rest.basis = glm::vec3(matrix[3]) + rest.rotation * (rest.scale * rest.localBasis);
                const std::vector<graphics::JointAnimation> edited{{part.second.id, rest, motionFor(part.first)}};
                require(graphics::AnimationSystem::poseHierarchy(scene, edited, 0), "보행과 부품 동작 합성 실패.");
            }
        }
    }
    }
    applyMounts();
    return true;
}

bool Assembly::update(float dt, std::ostream& out) {
    if (faulted_)
        return false;
    try {
        require(std::isfinite(dt) && dt >= 0 && dt <= .25f, "dt는 0~.25초여야 합니다.");
        for (const auto& binding : state_.bindings) {
            if (!binding.hold || !binding.down)
                continue;
            if (binding.tokens[0] == "move")
                move(binding.tokens[1], vector(binding.tokens, 2) * dt);
            else
                rotate(binding.tokens[1], axis(binding.tokens[2]), float(number(binding.tokens[3])) * dt);
        }
        if (!state_.paused)
            for (auto& behavior : state_.behaviors)
                if (behavior.playing && behavior.enabled)
                    behavior.seconds += dt;
        return applyPoses(dt);
    } catch (const std::exception& error) {
        faulted_ = true;
        state_.paused = true;
        state_.dragging = false; state_.dragX.reset();
        for (auto& binding : state_.bindings)
            binding.down = false;
        out << "업데이트 정지: " << error.what() << "\nremove <동작> 또는 clear로 조립을 수정하세요. guide physics\n";
        return false;
    }
}

bool Assembly::render() {
    if (!state_.threeViews)
        return app_.renderScene();
    const auto size = app_.getFramebufferSize();
    return size[0] < 3 || size[1] < 2 || app_.renderViews(graphics::PatternSystem::threeViews(
        app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
} // namespace assembly
