#include "Assembly.h"
#include <algorithm>

namespace assembly {
bool Assembly::objectCommand(const Tokens& a, std::ostream&) {
    const auto& command = a[0];
    if (command == "add") {
        require(a.size() == 6 || a.size() == 9, "add <종류> <이름> <X> <Y> <Z> [크기X 크기Y 크기Z]");
        add(a[1], a[2], vector(a, 3), a.size() == 9 ? vector(a, 6) : glm::vec3(1));
        state_.selected = a[2]; state_.selectedLayer.clear();
        return true;
    }
    if (command == "parent") {
        require(a.size() == 3, "parent <자식> <부모 또는 none>. 현재 월드 배치와 지역 레이어를 유지합니다.");
        auto& child = node(a[1]);
        require(!child.mount, "면 부착 관절은 unmount 후 부모를 바꾸세요.");
        auto* driver = positionDriver(a[1]);
        require(!driver || (driver->effect != Effect::Body && driver->effect != Effect::Walk), "물리 루트에는 부모를 추가할 수 없습니다.");
        for (const auto& behavior : state_.behaviors) {
            if (behavior.effect != Effect::Body && behavior.effect != Effect::Walk) continue;
            const auto& room = node(behavior.reference);
            require((!room.mesh || child.id != *room.mesh) && a[1] != behavior.target + ".mesh",
                    "물리 메시/방 walls의 부모를 바꿀 수 없습니다.");
        }
        const auto parent = a[2] == "none" ? std::optional<graphics::EntityId>{} : std::optional<graphics::EntityId>{node(a[2]).id};
        require(app_.getScene().setParentPreserveWorld(child.id, parent), "부모 연결 실패. 순환 여부를 확인하세요.");
        child.attachment = app_.getScene().getComponent(child.id)->attachment;
        return true;
    }
    if (command == "color" || command == "show") {
        require(command == "color" ? a.size() == 5 : a.size() == 3, "color <대상> <R> <G> <B> 또는 show <대상> <on/off>");
        const auto root = node(a[1]).id;
        const auto color = command == "color" ? vector(a, 2) : glm::vec3(1);
        require(glm::all(glm::greaterThanEqual(color, glm::vec3(0))) && glm::all(glm::lessThanEqual(color, glm::vec3(1))), "색은 0~1입니다.");
        const bool visible = command == "show" ? boolean(a[2]) : true;
        auto& scene = app_.getScene();
        for (auto id : scene.getIds()) {
            std::optional<graphics::EntityId> ancestor = id;
            while (ancestor && *ancestor != root)
                ancestor = scene.getComponent(*ancestor)->parent;
            if (ancestor) {
                require(command == "color" ? scene.setTint(id, {color.x, color.y, color.z}) : scene.setVisible(id, visible), "색/가시성 적용 실패.");
            }
        }
        return true;
    }
    if (command == "move") {
        require(a.size() == 5, "move <대상> <DX> <DY> <DZ>");
        move(a[1], vector(a, 2));
        return true;
    }
    if (command == "rotate") {
        require(a.size() == 4, "rotate <대상> <축> <도>");
        rotate(a[1], axis(a[2]), float(number(a[3])));
        return true;
    }
    if (command == "scale" || command == "pivot" || command == "position" || command == "rest" || command == "rest3") {
        require(a.size() >= 3, command + " <대상> <값>");
        auto rest = node(a[1]).rest;
        if (command == "scale") {
            require(a.size() == 3 || a.size() == 5, "scale <대상> <균일값 또는 SX SY SZ>");
            rest.scale = a.size() == 3 ? glm::vec3(float(number(a[2]))) : vector(a, 2);
        } else if (command == "rest" || command == "rest3") {
            require(command == "rest" ? a.size() == 18 : a.size() == 20, "rest <대상> <위치3> <회전행렬 열우선9> <균일배율> <피벗3>");
            rest.basis = vector(a, 2);
            for (int c = 0; c < 3; ++c)
                for (int r = 0; r < 3; ++r)
                    rest.rotation[c][r] = float(number(a[5 + c * 3 + r]));
            rest.scale = command == "rest3" ? vector(a, 14) : glm::vec3(float(number(a[14])));
            rest.localBasis = vector(a, command == "rest3" ? 17 : 15);
        } else {
            require(a.size() == 5, command + " <대상> <X> <Y> <Z>");
            if (command == "pivot") rest.localBasis = vector(a, 2);
            else rest.basis = vector(a, 2);
        }
        setRest(a[1], rest);
        return true;
    }
    return false;
}

bool Assembly::dispatch(const Tokens& a, std::ostream& out) {
    if (dynamicsCommand(a, out) || sceneToolCommand(a, out) || workspaceCommand(a, out)) return true;
    if (a[0] == "help") {
        require(a.size() <= 2, "help [기능]");
        if (a.size() == 2) guide(a[1], out);
        else help(out);
        return true;
    }
    if (a[0] == "guide") {
        require(a.size() == 2, "guide <기능 또는 객체 이름>");
        guide(a[1], out);
        return true;
    }
    if (a[0] == "list") {
        require(a.size() == 1, "list");
        for (const auto& item : state_.nodes) inspect(item.first, out);
        for (const auto& binding : state_.bindings)
            out << "binding key=" << binding.key << " mods=" << binding.mods << (binding.hold ? " hold " : " press ") << binding.command << '\n';
        return true;
    }
    if (a[0] == "diagnose") {
        require(a.size() == 2, "diagnose <객체 또는 부품>");
        diagnose(a[1], out);
        return true;
    }
    if (a[0] == "inspect" || a[0] == "select") {
        require(a.size() == 2, a[0] + " <객체/부품 이름>");
        inspect(a[1], out);
        if (a[0] == "select") { state_.selected = a[1]; state_.selectedLayer.clear(); }
        return true;
    }
    if (a[0] == "clear") {
        require(a.size() == 1, "clear");
        clear();
        return true;
    }
    if (a[0] == "hotload") {
        require(a.size() == 2, "hotload <파일>. Ctrl+F5 또는 reload로 다시 읽습니다.");
        state_.reloadPath = a[1];
        return true;
    }
    if (a[0] == "reload") {
        require(a.size() == 1 && !replaying_, "reload. 레시피 안에서 파일 명령을 중첩하지 않습니다.");
        const auto path = state_.reloadPath;
        return load(path, out);
    }
    if (a[0] == "save" || a[0] == "load") {
        require(a.size() == 2 && !replaying_, "save/load <파일>. 레시피 안에서 파일 명령을 중첩할 수 없습니다.");
        return a[0] == "save" ? save(a[1], out) : load(a[1], out);
    }
    if (surfaceCommand(a, out) || trackCommand(a, out) || hierarchyCommand(a, out) || objectCommand(a, out) || motionCommand(a, out) || inputCommand(a, out))
        return true;
    if (const auto handled = customCommand(*this, a, out))
        return *handled;
    return false;
}
} // namespace assembly
