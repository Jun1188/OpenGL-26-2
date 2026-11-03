#include "Assembly.h"
#include "Paths.h"
#include <algorithm>

namespace assembly {
bool Assembly::motionCommand(const Tokens& a, std::ostream&) {
    const auto& command = a[0];
    if (command == "remove") {
        require(a.size() == 2, "remove <동작 이름>");
        const auto it = std::find_if(state_.behaviors.begin(), state_.behaviors.end(), [&](const Behavior& b) { return b.name == a[1]; });
        require(it != state_.behaviors.end(), "없는 동작입니다: " + a[1]);
        if ((it->effect == Effect::Face || it->effect == Effect::Wall) && node(it->target).mesh) {
            const auto mesh = *node(it->target).mesh;
            require(app_.getScene().setFaceLocalMatrix(mesh, it->faceIndex, it->initialFaceMatrix, it->initialFaceSpace) &&
                    app_.getScene().setFaceVisible(mesh, it->faceIndex, it->initialFaceVisible), "면 동작 복원 실패.");
        }
        state_.behaviors.erase(it);
        return true;
    }
    if (command == "play" || command == "enable" || command == "seek") {
        require(a.size() == 3, command + " <동작 이름> <on/off 또는 초>");
        const auto it = std::find_if(state_.behaviors.begin(), state_.behaviors.end(), [&](const Behavior& b) { return b.name == a[1]; });
        require(it != state_.behaviors.end(), "없는 동작입니다: " + a[1]);
        if (command == "play") it->playing = boolean(a[2]);
        else if (command == "enable") it->enabled = boolean(a[2]);
        else {
            require(it->effect != Effect::Body && it->effect != Effect::Walk, "적분 물리는 seek 대신 재조립을 사용하세요.");
            it->seconds = number(a[2]);
            require(it->seconds >= 0, "시간은 0 이상입니다.");
        }
        return true;
    }
    if (command == "jump" || command == "speed" || command == "direction" || command == "obstacle") {
        require(a.size() >= 3, command + " <보행 동작 이름> <값/객체>");
        const auto it = std::find_if(state_.behaviors.begin(), state_.behaviors.end(), [&](const Behavior& b) {
            return b.name == a[1] && b.effect == Effect::Walk;
        });
        require(it != state_.behaviors.end(), "보행 동작 이름이 필요합니다.");
        if (command == "obstacle") {
            require(a.size() == 3 && node(a[2]).mesh.has_value(), "obstacle <보행 동작> <메시 객체>");
            require(a[2] != it->target && a[2] != it->reference, "자신이나 방을 장애물로 등록할 수 없습니다.");
            require(std::find(it->obstacles.begin(), it->obstacles.end(), a[2]) == it->obstacles.end(), "이미 등록한 장애물입니다.");
            it->obstacles.push_back(a[2]);
        } else if (command == "direction") {
            require(a.size() == 4, "direction <보행 동작> <X> <Z>");
            const glm::vec3 direction(float(number(a[2])), 0, float(number(a[3])));
            require(glm::length(direction) > .0001f, "0이 아닌 보행 방향이 필요합니다.");
            it->character.direction = glm::normalize(direction);
        } else {
            require(a.size() == 3, command + " <보행 동작> <속도>");
            const float speed = float(number(a[2]));
            require(speed >= 0 && speed <= 10, "속도는 0~10입니다.");
            if (command == "speed") it->character.speed = speed;
            else require(graphics::CollisionSystem::jump(it->character, speed), "접지 중에만 점프할 수 있습니다.");
        }
        return true;
    }
    const std::vector<std::string> supported{"spin", "orbit", "path", "linear", "pendulum", "face", "wall", "physics", "walk", "belt"};
    if (std::find(supported.begin(), supported.end(), command) == supported.end())
        return false;
    require(a.size() >= 3, command + " <동작 이름> <대상> ...");
    Behavior b;
    b.name = a[1]; b.target = a[2];
    auto& object = node(b.target);
    if (command == "spin" || command == "orbit") {
        require(command == "spin" ? (a.size() == 5 || a.size() == 6) : a.size() == 8,
                "spin <id> <대상> <축> <도/초> [local/world] 또는 orbit <id> <대상> <축> <도/초> <CX> <CY> <CZ>");
        graphics::RotationMotion config;
        config.axis.direction = axis(a[3]);
        config.degreesPerSecond = number(a[4]);
        if (command == "orbit") {
            config.axis.point = vector(a, 5);
            config.space = graphics::TransformSpace::World;
            b.positionDriver = true;
        } else if (a.size() == 6) {
            require(a[5] == "local" || a[5] == "world", "공간은 local/world입니다.");
            config.space = a[5] == "local" ? graphics::TransformSpace::Local : graphics::TransformSpace::World;
            b.parentSpin = config.space == graphics::TransformSpace::World;
        }
        require(b.motion.append(graphics::Motion::rotation(config)), "회전 동작 생성 실패.");
    } else if (command == "path") {
        require(a.size() == 5, "path <id> <대상> <1~6> <주기 초>");
        const double index = number(a[3]);
        require(index >= 1 && index <= 6 && index == std::floor(index), "경로 번호는 1~6입니다.");
        using Factory = graphics::PathMotion (*)();
        const Factory factories[]{standard_modes::path1, standard_modes::path2, standard_modes::path3,
            standard_modes::path4, standard_modes::path5, standard_modes::path6};
        auto config = factories[int(index) - 1]();
        config.durationSeconds = number(a[4]);
        require(config.durationSeconds > 0, "주기는 양수입니다.");
        config.relativeToStart = true;
        b.positionDriver = true;
        require(b.motion.append(graphics::Motion::followPath(config)), "경로 동작 생성 실패.");
    } else if (command == "linear") {
        require(a.size() == 7 || a.size() == 8, "linear <id> <대상> <DX> <DY> <DZ> <초> [once/loop/pingpong]");
        graphics::LinearMotion config;
        config.to = vector(a, 3); config.durationSeconds = number(a[6]);
        require(config.durationSeconds > 0, "이동 시간은 양수입니다.");
        if (a.size() == 8) {
            require(a[7] == "once" || a[7] == "loop" || a[7] == "pingpong", "재생은 once/loop/pingpong입니다.");
            config.playback = a[7] == "once" ? graphics::Playback::Once : a[7] == "loop" ? graphics::Playback::Loop : graphics::Playback::PingPong;
        }
        b.positionDriver = true;
        require(b.motion.append(graphics::Motion::linear(config)), "선형 이동 생성 실패.");
    } else if (command == "pendulum") {
        require(a.size() == 6, "pendulum <id> <대상> <축> <진폭 도> <주기 초>");
        graphics::PendulumMotion config;
        config.axis.direction = axis(a[3]); config.amplitudeDegrees = number(a[4]); config.periodSeconds = number(a[5]);
        require(config.periodSeconds > 0 && config.amplitudeDegrees >= 0, "양수 주기와 0 이상 진폭이 필요합니다.");
        require(b.motion.append(graphics::Motion::pendulum(config)), "진자 생성 실패.");
    } else if (command == "face" || command == "wall") {
        require(object.mesh.has_value(), "면을 가진 대상이 필요합니다.");
        require(command == "face" ? a.size() == 7 : a.size() == 5,
                "face <id> <대상> <면> <spin/orbit> <축> <도/초> 또는 face <id> <대상> <면> hinge <모서리> <도/초> 또는 wall <id> <방> <면0~3> <초>");
        const double face = number(a[3]);
        const auto* mesh = app_.getScene().getComponent(*object.mesh);
        require(face >= 0 && face == std::floor(face) && face < mesh->polyhedron.faces.size(), "면 번호 범위를 확인하세요.");
        b.faceIndex = std::size_t(face);
        if (command == "wall") {
            require(object.room && face < 4, "방 옆벽 0~3이 필요합니다.");
            b.effect = Effect::Wall; b.rate = float(number(a[4]));
            require(b.rate > 0, "개방 시간은 양수입니다.");
        } else {
            b.effect = Effect::Face;
            require(a[4] == "hinge" || a[4] == "spin" || a[4] == "orbit", "면 동작은 hinge/spin/orbit입니다.");
            b.face.kind = a[4] == "hinge" ? graphics::FaceRotationKind::EdgeHinge :
                a[4] == "spin" ? graphics::FaceRotationKind::SelfSpin : graphics::FaceRotationKind::ModelOrbit;
            b.rate = float(number(a[6]));
            if (a[4] == "hinge") {
                const double edge = number(a[5]);
                require(edge >= 0 && edge == std::floor(edge) && edge < mesh->polyhedron.faces[b.faceIndex].localVertices.size(), "모서리 범위를 확인하세요.");
                b.face.edge = std::size_t(edge);
            } else b.face.axis = axis(a[5]);
        }
    } else if (command == "physics" || command == "walk") {
        require(command == "physics" ? a.size() == 4 : a.size() == 5,
                "physics <id> <상자> <방> 또는 walk <id> <walker> <방> <속도>");
        b.reference = a[3];
        const auto& room = node(b.reference);
        require(room.room.has_value(), "add room으로 만든 방이 필요합니다.");
        require(room.mesh.has_value(), "방 메시가 필요합니다.");
        const auto& wall = node(b.reference + ".walls");
        const auto originalWalls = graphics::DiagramSystem::modelMatrix(wall.initial);
        const auto currentWalls = graphics::DiagramSystem::modelMatrix(wall.rest);
        for (int c = 0; c < 4; ++c)
            require(glm::length(originalWalls[c] - currentWalls[c]) < .000001f,
                    "방 walls의 초기 변환을 유지한 뒤 물리를 붙이세요. 방 루트를 조작하세요.");
        const auto matrix = app_.getScene().worldMatrix(room.id);
        const auto world = app_.getScene().worldMatrix(object.id);
        require(matrix && world && rigid(*matrix) && !app_.getScene().getComponent(object.id)->parent,
                "강체 방과 부모 없는 물리 루트가 필요합니다.");
        b.previousRoom = *matrix; b.positionDriver = true;
        if (command == "physics") {
            require(object.kind == "box", "physics는 중심 기준 box를 지원합니다.");
            const auto* mesh = object.mesh ? app_.getScene().getComponent(*object.mesh) : nullptr;
            require(mesh && glm::length(object.rest.localBasis) < .0001f &&
                    glm::length(mesh->transform.basis) < .0001f && glm::length(mesh->transform.localBasis) < .0001f &&
                    glm::length(mesh->transform.scale - glm::vec3(1)) < .0001f &&
                    glm::length(mesh->transform.rotation[0] - glm::vec3(1, 0, 0)) < .0001f &&
                    glm::length(mesh->transform.rotation[1] - glm::vec3(0, 1, 0)) < .0001f,
                    "상자의 중심 피벗과 초기 mesh 변환을 유지한 뒤 physics를 붙이세요.");
            b.effect = Effect::Body;
            b.body.localPosition = glm::vec3(glm::inverse(*matrix) * (*world)[3]);
        } else {
            require(object.walker.has_value(), "add walker로 만든 리그가 필요합니다.");
            require(object.rest.scale == glm::vec3(1) && glm::length(object.rest.localBasis) < .0001f && rigid(*world),
                    "보행 리그는 배율 1과 중심 피벗을 유지해야 합니다.");
            b.effect = Effect::Walk;
            b.character.feet = glm::vec3((*world)[3]); b.character.height = 1.8f;
            b.character.speed = float(number(a[4]));
            require(b.character.speed >= 0 && b.character.speed <= 10, "보행 속도는 0~10입니다.");
        }
    } else {
        require(a.size() == 5 || a.size() == 6, "belt <id> <대상> <속도> <높이 보정> [위상0~1]");
        b.effect = Effect::Conveyor; b.positionDriver = true;
        b.rate = float(number(a[3])); b.height = float(number(a[4]));
        b.phase = a.size() == 6 ? float(number(a[5])) : 0;
        require(b.phase >= 0 && b.phase <= 1, "위상은 0~1입니다.");
    }
    attach(std::move(b));
    return true;
}
} // namespace assembly
