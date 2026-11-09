#include "Assembly.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace assembly {
void require(bool success, const std::string& message) {
    if (!success)
        throw std::runtime_error(message);
}

Tokens tokenize(const std::string& line) {
    Tokens result;
    std::string token;
    bool quoted = false, started = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (c == '#' && !quoted)
            break;
        if (c == '"') {
            quoted = !quoted;
            started = true;
        } else if (c == '\\' && quoted && i + 1 < line.size() && (line[i + 1] == '"' || line[i + 1] == '\\')) {
            token += line[++i];
        } else if (!quoted && std::isspace(static_cast<unsigned char>(c))) {
            if (started) {
                result.push_back(token);
                token.clear();
                started = false;
            }
        } else {
            token += c;
            started = true;
        }
    }
    require(!quoted, "닫히지 않은 따옴표입니다.");
    if (started)
        result.push_back(token);
    return result;
}

std::string quote(const std::string& text) {
    std::ostringstream out;
    out << std::quoted(text);
    return out.str();
}

double number(const std::string& text) {
    std::size_t consumed = 0;
    const double value = std::stod(text, &consumed);
    require(consumed == text.size() && std::isfinite(value) && std::abs(value) <= 1000000,
            "유한한 수(-1000000~1000000)가 필요합니다: " + text);
    return value;
}

std::size_t indexNumber(const std::string& text) {
    const auto value = number(text);
    require(value >= 0 && value == std::floor(value), "0 이상 정수 인덱스가 필요합니다.");
    return std::size_t(value);
}

glm::vec3 vector(const Tokens& args, std::size_t offset) {
    require(offset + 3 <= args.size(), "XYZ 값 3개가 필요합니다.");
    return {float(number(args[offset])), float(number(args[offset + 1])), float(number(args[offset + 2]))};
}

glm::vec3 axis(const std::string& text) {
    require(text == "x" || text == "y" || text == "z", "축은 x/y/z 중 하나입니다.");
    return text == "x" ? glm::vec3(1, 0, 0) : text == "y" ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, 1);
}

bool boolean(const std::string& text) {
    require(text == "on" || text == "off", "on 또는 off가 필요합니다.");
    return text == "on";
}

bool rigid(const glm::mat4& matrix) {
    const auto r = glm::mat3(matrix), gram = glm::transpose(r) * r;
    return graphics::TransformSystem::validAffine(matrix) && std::abs(glm::determinant(r) - 1) < .001f &&
        glm::length(gram[0] - glm::vec3(1, 0, 0)) < .001f &&
        glm::length(gram[1] - glm::vec3(0, 1, 0)) < .001f &&
        glm::length(gram[2] - glm::vec3(0, 0, 1)) < .001f;
}

Node& Assembly::node(const std::string& name) {
    const auto found = state_.nodes.find(name);
    require(found != state_.nodes.end(), "없는 객체/부품입니다: " + name + ". list 또는 guide objects를 사용하세요.");
    return found->second;
}

const Node& Assembly::node(const std::string& name) const {
    const auto found = state_.nodes.find(name);
    require(found != state_.nodes.end(), "없는 객체/부품입니다: " + name);
    return found->second;
}

void Assembly::clear() {
    editStart_.reset(); editOperation_=0; editCursor_.reset();
    ++topologyRevision_;
    app_.getScene().clear();
    app_.getRenderOptions().pointLights.clear();
    state_ = State{};
    faulted_ = false;
}

void Assembly::registerNode(const std::string& name, graphics::EntityId id, bool whole,
                            const std::string& kind, std::optional<graphics::EntityId> mesh) {
    require(state_.nodes.size() < 1000, "객체/부품 이름 1000개 한도입니다.");
    require(!name.empty() && !state_.nodes.count(name), "중복 이름입니다: " + name);
    const auto* component = app_.getScene().getComponent(id);
    require(component != nullptr, "등록할 엔티티가 없습니다.");
    for (const auto& item : state_.nodes)
        require(item.second.id != id, "같은 엔티티를 두 이름으로 등록할 수 없습니다.");
    ++topologyRevision_;
    state_.nodes.emplace(name, Node{id, mesh, component->transform, component->transform, kind, whole, {}, {}});
    if (state_.selected.empty() && whole)
        state_.selected = name;
}

Behavior* Assembly::positionDriver(const std::string& target) {
    for (auto& behavior : state_.behaviors)
        if (behavior.target == target && behavior.positionDriver)
            return &behavior;
    return nullptr;
}

void Assembly::attach(Behavior behavior) {
    require(!behavior.name.empty(), "동작 이름이 필요합니다.");
    const auto& targetNode = node(behavior.target);
    require(!targetNode.affineGroup || behavior.effect == Effect::Custom || behavior.effect == Effect::Input,
            "비균일/반사 그룹은 정적 계층입니다. 자식 group/joint에 모션을 붙이세요.");
    if (behavior.effect == Effect::Input)
        require(bool(behavior.key) || bool(behavior.mouse) || bool(behavior.cursor) || bool(behavior.focus), "Input에는 입력 콜백이 필요합니다.");
    if (behavior.effect == Effect::Custom) {
        require(bool(behavior.custom), "Custom 동작 함수가 필요합니다.");
        behavior.positionDriver = true;
    }
    if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Conveyor)
        behavior.positionDriver = true;
    const auto checkGeometryOwner = [&](const Behavior& physics, const Behavior& other) {
        if (other.effect == Effect::Input || (physics.effect != Effect::Body && physics.effect != Effect::Walk)) return;
        const auto& object = node(other.target);
        const auto& room = node(physics.reference);
        const bool face = faceEffect(other.effect);
        if (physics.effect == Effect::Body && face)
            require(object.mesh != node(physics.target).mesh, "물리 상자 면은 별도로 변형할 수 없습니다.");
        if (!face)
            require(!room.mesh || object.id != *room.mesh, "물리 방 walls에는 별도 이동/회전을 붙일 수 없습니다. 방 루트를 사용하세요.");
    };
    for (const auto& current : state_.behaviors) {
        checkGeometryOwner(current, behavior);
        checkGeometryOwner(behavior, current);
        require(current.name != behavior.name, "중복 동작 이름입니다: " + behavior.name);
        require(behavior.effect == Effect::Input || current.effect == Effect::Input ||
                (!(current.effect == Effect::Body && behavior.target == current.target + ".mesh") &&
                 !(behavior.effect == Effect::Body && current.target == behavior.target + ".mesh")),
                "물리 메시에는 별도 동작을 붙일 수 없습니다. 상자 루트를 사용하세요.");
        const bool currentFace = faceEffect(current.effect);
        const bool newFace = faceEffect(behavior.effect);
        if (currentFace && newFace)
            require(node(current.target).mesh != node(behavior.target).mesh || current.faceIndex != behavior.faceIndex,
                    "같은 메시의 같은 면에 동작이 이미 있습니다.");
        if (current.target != behavior.target)
            continue;
        require(!(current.positionDriver && behavior.positionDriver), "위치 제어가 충돌합니다. 기존 동작을 remove 하거나 자식 부품에 붙이세요.");
        const bool currentPhysics = current.effect == Effect::Body || current.effect == Effect::Walk || current.effect == Effect::Custom;
        const bool newPhysics = behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Custom;
        require(!((currentPhysics && behavior.effect == Effect::Motion) || (newPhysics && current.effect == Effect::Motion)),
                "물리 루트와 Motion은 같은 SRT를 소유할 수 없습니다. 자식 부품에 동작을 붙이세요.");
    }
    if (behavior.effect == Effect::Motion || behavior.effect == Effect::FaceTrack) {
        for (const auto& step : behavior.motion.getSteps())
            require(!step.action, "변환 모션에 action을 넣을 수 없습니다. bind 명령 또는 customCommand를 사용하세요.");
        const auto sample = behavior.motion.sample(0);
        require(sample.has_value(), "유효한 모션 샘플이 필요합니다.");
        if (behavior.effect == Effect::Motion)
            require(rigid(sample->parent) && rigid(sample->local), "객체 Motion은 유효한 강체 변환이어야 합니다.");
    }
    if (faceEffect(behavior.effect)) {
        const auto& target = node(behavior.target);
        require(target.mesh.has_value(), "면 동작에는 메시가 필요합니다.");
        const auto* component = app_.getScene().getComponent(*target.mesh);
        require(component && behavior.faceIndex < component->polyhedron.faces.size(), "면 번호 범위를 확인하세요.");
        const auto& face = component->polyhedron.faces[behavior.faceIndex];
        behavior.initialFaceMatrix = face.localAnimationMatrix;
        behavior.initialFaceSpace = face.animationSpace;
        behavior.initialFaceVisible = face.visible;
        const auto srt = graphics::detail::faceSrtMatrix(face);
        behavior.faceTrackBase = face.animationSpace == graphics::FaceAnimationSpace::Model
            ? face.localAnimationMatrix : srt * face.localAnimationMatrix * glm::inverse(srt);
        require(graphics::TransformSystem::validAffine(behavior.faceTrackBase), "면의 기준 행렬이 특이합니다.");
    }
    state_.behaviors.push_back(std::move(behavior));
    ++topologyRevision_;
}

void Assembly::setRest(const std::string& target, const graphics::Transform3D& rest) {
    auto& value = node(target);
    bool immutable = false;
    for (const auto& behavior : state_.behaviors) {
        immutable = immutable || (behavior.effect == Effect::Body && target == behavior.target + ".mesh");
        if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk) {
            const auto& room = node(behavior.reference);
            immutable = immutable || (room.mesh && value.id == *room.mesh);
        }
    }
    if (immutable) {
        require(rest.basis == value.rest.basis && rest.localBasis == value.rest.localBasis && rest.scale == value.rest.scale &&
            rest.rotation[0] == value.rest.rotation[0] && rest.rotation[1] == value.rest.rotation[1] &&
            rest.rotation[2] == value.rest.rotation[2], "물리 메시/walls 대신 객체 루트를 편집하세요.");
        return;
    }
    require(glm::all(glm::lessThanEqual(glm::abs(rest.basis), glm::vec3(1000000))) &&
            glm::all(glm::lessThanEqual(glm::abs(rest.localBasis), glm::vec3(1000000))), "위치/피벗 범위는 ±1000000입니다.");
    const bool validScale = glm::all(glm::greaterThan(glm::abs(rest.scale), glm::vec3(.0001f))) &&
        glm::all(glm::lessThanEqual(glm::abs(rest.scale), glm::vec3(100)));
    require(graphics::DiagramSystem::validTransform(rest) && validScale,
            "배율은 각 축 절댓값 .0001~100이며, 유효한 SRT가 필요합니다.");
    for (const auto& behavior : state_.behaviors) {
        if ((behavior.effect == Effect::Body || behavior.effect == Effect::Walk) && behavior.reference == target)
            require(rigid(graphics::DiagramSystem::modelMatrix(rest)), "물리 공간은 강체 변환을 유지해야 합니다.");
        if (behavior.effect == Effect::Walk && behavior.reference == target)
            require(glm::length(rest.rotation[0] - glm::vec3(1, 0, 0)) < .001f &&
                    glm::length(rest.rotation[1] - glm::vec3(0, 1, 0)) < .001f,
                    "보행 충돌 공간은 월드 축 정렬을 유지해야 합니다.");
    }
    if (auto* driver = positionDriver(target)) {
        if (driver->effect == Effect::Body) {
            require(glm::length(rest.localBasis) < .0001f, "물리 상자의 피벗은 중심을 유지합니다.");
            const auto matrix = app_.getScene().worldMatrix(node(driver->reference).id);
            require(matrix && rigid(*matrix), "방의 강체 행렬이 필요합니다.");
            const auto delta = rest.basis - value.rest.basis;
            if (driver->body.released)
                driver->body.worldPosition += delta;
            else
                driver->body.localPosition += glm::transpose(glm::mat3(*matrix)) * delta;
        } else if (driver->effect == Effect::Walk) {
            require(rest.scale == glm::vec3(1) && glm::length(rest.localBasis) < .0001f,
                    "보행 루트는 배율 1과 중심 피벗을 유지합니다.");
            require(std::abs(rest.rotation[1].y - 1) < .001f, "보행 루트는 Y축 회전만 지원합니다.");
            driver->character.feet += rest.basis - value.rest.basis;
            driver->character.direction = rest.rotation * glm::vec3(0, 0, 1);
        }
    }
    value.rest = rest;
}

void Assembly::move(const std::string& target, glm::vec3 delta) {
    auto rest = node(target).rest;
    rest.basis += delta;
    setRest(target, rest);
}

void Assembly::rotate(const std::string& target, glm::vec3 direction, float degrees) {
    auto rest = node(target).rest;
    rest.rotation *= glm::mat3(glm::rotate(glm::mat4(1), glm::radians(degrees), direction));
    const auto x = glm::normalize(rest.rotation[0]);
    const auto z = glm::normalize(glm::cross(x, rest.rotation[1]));
    rest.rotation = glm::mat3(x, glm::cross(z, x), z);
    setRest(target, rest);
}

void Assembly::inspect(const std::string& target, std::ostream& out) const {
    const auto& value = node(target);
    const auto& t = value.rest;
    out << target << " kind=" << value.kind << " id=" << value.id << " position="
        << t.basis.x << ',' << t.basis.y << ',' << t.basis.z << " scale=" << t.scale.x << ',' << t.scale.y << ',' << t.scale.z << '\n';
    for (const auto& behavior : state_.behaviors)
        if (behavior.target == target)
            out << "  " << behavior.name << " time=" << behavior.seconds << " playing=" << behavior.playing
                << " enabled=" << behavior.enabled << '\n';
}

bool Assembly::execute(const std::string& line, std::ostream& out) {
    Tokens args;
    try {
        require(line.size() <= 4096, "명령은 4096바이트 이하입니다.");
        args = tokenize(line);
    } catch (const std::exception& error) {
        out << "명령 거절: " << error.what() << '\n';
        return false;
    }
    if (args.empty())
        return true;
    auto savedScene = app_.getScene();
    const auto savedState = state_;
    const auto savedCamera = app_.getCamera();
    const auto savedOptions = app_.getRenderOptions();
    const bool savedFault = faulted_;
    const bool readOnly = args[0] == "save" || args[0] == "guide" || args[0] == "help" || args[0] == "list" || args[0] == "inspect" || args[0] == "surface" || args[0] == "diagnose";
    try {
        args = resolveCommand(args);
        require(dispatch(args, out), "지원하지 않는 명령입니다. help 또는 guide custom을 사용하세요.");
        if (!readOnly)
            require(applyPoses(0), "조립 결과의 상태/변환이 유효하지 않습니다.");
        if (!replaying_ && !readOnly && args[0] != "load" && args[0] != "reload" && args[0] != "select" && args[0] != "jump" && args[0] != "release")
        {
            std::string canonical;
            for (const auto& arg : args) { if (!canonical.empty()) canonical += ' '; canonical += quote(arg); }
            state_.recipe.push_back(canonical);
        }
        if (!readOnly) faulted_ = false;
        if (!replaying_ && !readOnly) {
            out << line << " [적용 완료] - 대상: " << (state_.selected.empty() ? "작업공간" : state_.selected);
            if (!state_.selectedLayer.empty()) out << ", 지역 레이어: " << state_.selectedLayer;
            out << '\n';
            if (!state_.selected.empty() && state_.nodes.count(state_.selected)) {
                const auto world = app_.getScene().placementMatrix(node(state_.selected).id);
                if (world) out << "월드 원점=" << (*world)[3].x << ',' << (*world)[3].y << ',' << (*world)[3].z << '\n';
            }
        }
        return true;
    } catch (const std::exception& error) {
        app_.getScene() = std::move(savedScene);
        state_ = savedState;
        faulted_ = savedFault;
        app_.getCamera() = savedCamera;
        app_.getRenderOptions() = savedOptions;
        out << "명령 거절: " << error.what() << "\n이전 조립 상태를 유지했습니다. guide " << args[0] << '\n';
        return false;
    }
}
} // namespace assembly
