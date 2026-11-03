#include "Assembly.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace assembly {
namespace {
std::pair<int, int> keyCode(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return char(std::toupper(c)); });
    int mods = 0;
    for (;;) {
        const auto plus = text.find('+');
        if (plus == std::string::npos) break;
        const auto modifier = text.substr(0, plus);
        if (modifier == "CTRL") mods |= GLFW_MOD_CONTROL;
        else if (modifier == "SHIFT") mods |= GLFW_MOD_SHIFT;
        else if (modifier == "ALT") mods |= GLFW_MOD_ALT;
        else require(false, "수정키는 CTRL/SHIFT/ALT입니다.");
        text.erase(0, plus + 1);
    }
    const std::map<std::string, int> keys{{"LEFT", GLFW_KEY_LEFT}, {"RIGHT", GLFW_KEY_RIGHT},
        {"UP", GLFW_KEY_UP}, {"DOWN", GLFW_KEY_DOWN}, {"SPACE", GLFW_KEY_SPACE}, {"TAB", GLFW_KEY_TAB},
        {"BACKSPACE", GLFW_KEY_BACKSPACE}, {"ENTER", GLFW_KEY_ENTER}, {"PLUS", GLFW_KEY_EQUAL},
        {"MINUS", GLFW_KEY_MINUS}, {"PAGEUP", GLFW_KEY_PAGE_UP}, {"PAGEDOWN", GLFW_KEY_PAGE_DOWN}};
    int key = text.size() == 1 && text[0] >= 'A' && text[0] <= 'Z' ? int(text[0]) : 0;
    if (const auto found = keys.find(text); found != keys.end()) key = found->second;
    require(key != 0, "키는 영문자·방향키·SPACE·TAB·BACKSPACE·ENTER·PLUS·MINUS·PAGEUP·PAGEDOWN입니다.");
    return {key, mods};
}
}

void Assembly::bind(Binding binding) {
    require(binding.key > 0 && !binding.command.empty(), "키와 실행할 명령이 필요합니다.");
    for (const auto& current : state_.bindings)
        require(current.key != binding.key || current.mods != binding.mods, "중복 키입니다. unbind 후 다시 연결하세요.");
    binding.tokens = tokenize(binding.command);
    require(!binding.tokens.empty(), "빈 바인딩 명령입니다.");
    if (binding.hold) {
        require((binding.tokens[0] == "move" && binding.tokens.size() == 5) ||
                (binding.tokens[0] == "rotate" && binding.tokens.size() == 4), "hold는 move/rotate 속도 명령만 지원합니다.");
        node(binding.tokens[1]);
        if (binding.tokens[0] == "move") vector(binding.tokens, 2);
        else { axis(binding.tokens[2]); number(binding.tokens[3]); }
    }
    state_.bindings.push_back(std::move(binding));
}

bool Assembly::inputCommand(const Tokens& a, std::ostream&) {
    if (a[0] == "bind") {
        require(a.size() == 4 && (a[2] == "press" || a[2] == "hold"), "bind <키> <press/hold> \"명령\"");
        const auto key = keyCode(a[1]);
        Binding binding;
        binding.key = key.first; binding.mods = key.second; binding.hold = a[2] == "hold"; binding.command = a[3];
        bind(std::move(binding));
        return true;
    }
    if (a[0] == "unbind") {
        require(a.size() == 2, "unbind <키>");
        const auto key = keyCode(a[1]);
        const auto it = std::find_if(state_.bindings.begin(), state_.bindings.end(), [&](const Binding& b) {
            return b.key == key.first && b.mods == key.second;
        });
        require(it != state_.bindings.end(), "등록되지 않은 키입니다.");
        state_.bindings.erase(it);
        return true;
    }
    if (a[0] == "pause" || a[0] == "view") {
        require(a.size() == 2, "pause <on/off> 또는 view <1/3>");
        if (a[0] == "pause") state_.paused = boolean(a[1]);
        else {
            require(a[1] == "1" || a[1] == "3", "뷰는 1 또는 3입니다.");
            state_.threeViews = a[1] == "3";
        }
        return true;
    }
    if (a[0] == "render") {
        require(a.size() == 3, "render <wire/lighting/depth> <on/off>");
        const bool enabled = boolean(a[2]);
        auto& options = app_.getRenderOptions();
        if (a[1] == "wire") options.wireframe = enabled;
        else if (a[1] == "lighting") options.lighting = enabled;
        else if (a[1] == "depth") options.depthTest = enabled;
        else require(false, "렌더 설정은 wire/lighting/depth입니다.");
        return true;
    }
    if (a[0] != "camera") return false;
    require(a.size() >= 3, "camera <pose/move/yaw/orbit/zoom/projection/extent/fov/clip> ...");
    auto camera = app_.getCamera();
    if (a[1] == "pose") {
        require(a.size() == 11, "camera pose <위치3> <target3> <up3>");
        camera.position = vector(a, 2); camera.target = vector(a, 5); camera.up = vector(a, 8);
    } else if (a[1] == "move") {
        require(a.size() == 5, "camera move <DX> <DY> <DZ>");
        require(graphics::PatternSystem::moveCamera(camera, vector(a, 2)), "카메라 이동 실패.");
    } else if (a[1] == "yaw" || a[1] == "zoom") {
        require(a.size() == 3, "camera yaw <도> 또는 camera zoom <배율>");
        require(a[1] == "yaw" ? graphics::PatternSystem::yawCamera(camera, float(number(a[2]))) :
            graphics::PatternSystem::zoomCamera(camera, float(number(a[2]))), "카메라 회전/줌 실패.");
    } else if (a[1] == "orbit") {
        require(a.size() == 7, "camera orbit <축> <도> <CX> <CY> <CZ>");
        require(graphics::PatternSystem::orbitCamera(camera, {vector(a, 4), axis(a[2])}, float(number(a[3]))), "카메라 공전 실패.");
    } else if (a[1] == "projection") {
        require(a.size() == 3 && (a[2] == "ortho" || a[2] == "perspective"), "camera projection <ortho/perspective>");
        camera.orthographic = a[2] == "ortho";
    } else if (a[1] == "extent" || a[1] == "fov") {
        require(a.size() == 3, "camera extent <값> 또는 camera fov <도>");
        if (a[1] == "extent") camera.orthographicHalfExtent = float(number(a[2]));
        else camera.fieldOfViewDegrees = float(number(a[2]));
    } else if (a[1] == "clip") {
        require(a.size() == 4, "camera clip <near> <far>");
        camera.nearPlane = float(number(a[2])); camera.farPlane = float(number(a[3]));
    } else require(false, "지원하지 않는 카메라 명령입니다. guide camera");
    require(graphics::CameraSystem::validCamera(camera), "카메라의 위치·target·up·투영 범위를 확인하세요.");
    app_.getCamera() = camera;
    return true;
}

void Assembly::onKey(int key, int action, int mods, std::ostream& out) {
    if (action == GLFW_RELEASE) {
        for (auto& binding : state_.bindings)
            if (binding.key == key) binding.down = false;
        return;
    }
    const int physicalMods = mods & (GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT);
    for (auto& binding : state_.bindings) {
        if (binding.key != key || binding.mods != physicalMods) continue;
        if (binding.hold) binding.down = true;
        else if (action == GLFW_PRESS) {
            const auto command = binding.command;
            execute(command, out);
        }
        return;
    }
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        execute(state_.paused ? "pause off" : "pause on", out);
        return;
    }
    if ((key == GLFW_KEY_TAB || key == GLFW_KEY_BACKSPACE) && action == GLFW_PRESS && !state_.nodes.empty()) {
        auto it = state_.nodes.find(state_.selected);
        if (key == GLFW_KEY_TAB) {
            if (it == state_.nodes.end() || ++it == state_.nodes.end()) it = state_.nodes.begin();
        } else {
            if (it == state_.nodes.begin() || it == state_.nodes.end()) it = state_.nodes.end();
            --it;
        }
        state_.selected = it->first;
        inspect(it->first, out);
    } else if (key == GLFW_KEY_X || key == GLFW_KEY_Y || key == GLFW_KEY_Z) {
        state_.axis = key == GLFW_KEY_X ? 0 : key == GLFW_KEY_Y ? 1 : 2;
    } else if (!state_.selected.empty()) {
        std::ostringstream command;
        const auto target = quote(state_.selected);
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
            command << "rotate " << target << ' ' << "xyz"[state_.axis] << ' ' << (key == GLFW_KEY_LEFT ? -5 : 5);
        else if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
            glm::vec3 delta(0); delta[state_.axis] = key == GLFW_KEY_UP ? .05f : -.05f;
            command << "move " << target << ' ' << delta.x << ' ' << delta.y << ' ' << delta.z;
        }
        if (!command.str().empty()) execute(command.str(), out);
    }
}

void Assembly::onFocus(bool focused) {
    if (!focused)
        for (auto& binding : state_.bindings) binding.down = false;
}

void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
    std::optional<graphics::PickResult> hit;
    if (state_.threeViews) {
        const auto size = app_.getFramebufferSize();
        if (size[0] < 3 || size[1] < 2) return;
        for (const auto& pass : graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions())) {
            const auto& v = pass.viewport;
            const double top = size[1] - v.y - v.height;
            if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top && event.framebufferY < top + v.height)
                hit = graphics::pick(app_.getScene(), pass.camera, event.framebufferX, event.framebufferY,
                    {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
        }
    } else hit = app_.pickAt(event.windowX, event.windowY);
    state_.selected.clear();
    std::optional<graphics::EntityId> entity = hit ? std::optional<graphics::EntityId>{hit->entityId} : std::nullopt;
    while (entity) {
        for (const auto& item : state_.nodes)
            if (item.second.id == *entity && item.second.whole) {
                state_.selected = item.first;
                inspect(item.first, out);
                return;
            }
        const auto* component = app_.getScene().getComponent(*entity);
        entity = component ? component->parent : std::nullopt;
    }
    out << "선택 해제\n";
}
} // namespace assembly
