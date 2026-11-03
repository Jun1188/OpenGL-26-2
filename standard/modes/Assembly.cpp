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
    app_.getScene().clear();
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
    node(behavior.target);
    if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Conveyor)
        behavior.positionDriver = true;
    for (const auto& current : state_.behaviors) {
        require(current.name != behavior.name, "중복 동작 이름입니다: " + behavior.name);
        require(!(current.effect == Effect::Body && behavior.target == current.target + ".mesh") &&
                !(behavior.effect == Effect::Body && current.target == behavior.target + ".mesh"),
                "물리 메시에는 별도 동작을 붙일 수 없습니다. 상자 루트를 사용하세요.");
        const bool currentFace = current.effect == Effect::Face || current.effect == Effect::Wall;
        const bool newFace = behavior.effect == Effect::Face || behavior.effect == Effect::Wall;
        if (currentFace && newFace)
            require(node(current.target).mesh != node(behavior.target).mesh || current.faceIndex != behavior.faceIndex,
                    "같은 메시의 같은 면에 동작이 이미 있습니다.");
        if (current.target != behavior.target)
            continue;
        require(!(current.positionDriver && behavior.positionDriver), "위치 제어가 충돌합니다. 기존 동작을 remove 하거나 자식 부품에 붙이세요.");
        const bool currentPhysics = current.effect == Effect::Body || current.effect == Effect::Walk;
        const bool newPhysics = behavior.effect == Effect::Body || behavior.effect == Effect::Walk;
        require(!((currentPhysics && behavior.effect == Effect::Motion) || (newPhysics && current.effect == Effect::Motion)),
                "물리 루트와 Motion은 같은 SRT를 소유할 수 없습니다. 자식 부품에 동작을 붙이세요.");
        require(!((current.effect == Effect::Face || current.effect == Effect::Wall) &&
                  (behavior.effect == Effect::Face || behavior.effect == Effect::Wall) &&
                  current.faceIndex == behavior.faceIndex), "같은 면에 동작이 이미 있습니다.");
    }
    if (behavior.effect == Effect::Motion) {
        for (const auto& step : behavior.motion.getSteps())
            require(!step.action, "변환 모션에 action을 넣을 수 없습니다. bind 명령 또는 customCommand를 사용하세요.");
        const auto sample = behavior.motion.sample(0);
        require(sample && rigid(sample->parent) && rigid(sample->local), "Motion은 유효한 강체 변환이어야 합니다.");
    }
    state_.behaviors.push_back(std::move(behavior));
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
    require(graphics::DiagramSystem::validTransform(rest) && rest.scale.x > .0001f &&
        rest.scale.x <= 100 && std::abs(rest.scale.x - rest.scale.y) < .0001f &&
        std::abs(rest.scale.x - rest.scale.z) < .0001f, "양수 균일 배율(.0001~100)의 유효한 SRT가 필요합니다.");
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
        << t.basis.x << ',' << t.basis.y << ',' << t.basis.z << " scale=" << t.scale.x << '\n';
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
    const bool readOnly = args[0] == "save" || args[0] == "guide" || args[0] == "help" || args[0] == "list" || args[0] == "inspect";
    try {
        require(dispatch(args, out), "지원하지 않는 명령입니다. help 또는 guide custom을 사용하세요.");
        if (args[0] != "save" && args[0] != "guide" && args[0] != "help" && args[0] != "list" && args[0] != "inspect")
            require(applyPoses(0), "조립 결과의 상태/변환이 유효하지 않습니다.");
        if (!replaying_ && args[0] != "save" && args[0] != "load" && args[0] != "guide" && args[0] != "help" &&
            args[0] != "list" && args[0] != "inspect" && args[0] != "select" && args[0] != "jump")
            state_.recipe.push_back(line);
        if (!readOnly) faulted_ = false;
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
