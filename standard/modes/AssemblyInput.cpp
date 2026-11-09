#include "Assembly.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

namespace assembly {
std::pair<int, int> keyCode(std::string text, bool modeKeys) {
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
    if (modeKeys && text.size() == 1 && text[0] >= '0' && text[0] <= '9') key = int(text[0]);
    if (const auto found = keys.find(text); found != keys.end()) key = found->second;
    require(key != 0, "키는 영문자·방향키·SPACE·TAB·BACKSPACE·ENTER·PLUS·MINUS·PAGEUP·PAGEDOWN입니다.");
    return {key, mods};
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
    if (a[0] == "drag") {
        require((a.size() == 2 && a[1] == "off") || a.size() == 4, "drag <객체> <축> <도/픽셀> 또는 drag off");
        state_.dragging = false; state_.dragX.reset();
        if (a.size() == 2) state_.dragTarget.clear();
        else {
            node(a[1]); state_.dragTarget = a[1]; state_.dragAxis = axis(a[2]); state_.dragSensitivity = float(number(a[3]));
        }
        return true;
    }
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
        require(a.size() >= 3, "render <wire/lighting/depth/cull/light-direction/light-color/ambient/background> ...");
        auto& options = app_.getRenderOptions();
        if (a[1] == "cull") {
            require(a.size() == 3 && (a[2] == "none" || a[2] == "back" || a[2] == "front"), "render cull none/back/front");
            options.cull = a[2] == "none" ? graphics::CullMode::None : a[2] == "back" ? graphics::CullMode::Back : graphics::CullMode::Front;
            return true;
        }
        if (a[1] == "light-direction" || a[1] == "light-color" || a[1] == "ambient" || a[1] == "background") {
            require(a.size() == 5, "render light-direction/light-color/ambient/background <값3>");
            const auto value = vector(a, 2);
            if (a[1] == "light-direction") {
                require(glm::length(value) > .0001f, "광원 방향은 0일 수 없습니다.");
                options.lightDirection = glm::normalize(value);
            } else {
                require(glm::all(glm::greaterThanEqual(value, glm::vec3(0))) && glm::all(glm::lessThanEqual(value, glm::vec3(1))), "색은 0~1입니다.");
                if (a[1] == "background") options.background = glm::vec4(value, 1);
                else if (a[1] == "ambient") options.ambientColor = {value.x, value.y, value.z};
                else options.lightColor = {value.x, value.y, value.z};
            }
            return true;
        }
        require(a.size() == 3, "render <wire/lighting/depth> <on/off>");
        const bool enabled = boolean(a[2]);
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

bool Assembly::dispatchInput(const std::function<bool(const Behavior&)>& callback, std::ostream& out) {
    try {
        // A callback may execute clear/load or remove itself.
        const auto snapshot = state_.behaviors;
        const auto revision = topologyRevision_;
        for (const auto& behavior : snapshot) {
            if (behavior.enabled && callback(behavior)) return true;
            if (topologyRevision_ != revision) return true;
        }
    } catch (const std::exception& error) {
        faulted_ = true; state_.paused = true;
        state_.dragging = false; state_.dragX.reset();
        for (auto& binding : state_.bindings) binding.down = false;
        out << "사용자 입력 정지: " << error.what() << "\nguide input 또는 remove <입력ID>로 수정하세요.\n";
        return true;
    }
    return false;
}

void Assembly::onKey(int key, int action, int mods, std::ostream& out) {
    if (action == GLFW_RELEASE) {
        for (auto& binding : state_.bindings)
            if (binding.key == key) binding.down = false;
    }
    if (dispatchInput([&](const Behavior& b) { return b.key && b.key(*this, key, action, mods); }, out)) return;
    if (action == GLFW_RELEASE) return;
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
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (editStart_) finishEdit(false,out);
        else { state_.editMode = "object"; state_.faceSelection.reset(); state_.selectedLayer.clear(); }
        return;
    }
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) { finishEdit(true,out); return; }
    if ((key == GLFW_KEY_G || key == GLFW_KEY_R || key == GLFW_KEY_S) && action == GLFW_PRESS && !state_.selected.empty()) {
        finishEdit(true,out);
        editStart_ = state_; editScene_=app_.getScene(); state_.paused = true;
        editOperation_ = key == GLFW_KEY_G ? 't' : key == GLFW_KEY_R ? 'r' : 's'; editCursor_.reset();
        out << "G/R/S [변환 미리보기] - X/Y/Z: 축, 마우스/방향키: 양, Shift: 미세, Enter: 확정, Esc: 취소\n";
    } else if (key == GLFW_KEY_X || key == GLFW_KEY_Y || key == GLFW_KEY_Z) {
        state_.axis = key == GLFW_KEY_X ? 0 : key == GLFW_KEY_Y ? 1 : 2;
        out << "축 [" << "XYZ"[state_.axis] << "] - " << (state_.selectedLayer.empty() ? "월드" : "지역 레이어") << " 좌표계\n";
    } else if (editOperation_ && (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT || key == GLFW_KEY_UP || key == GLFW_KEY_DOWN)) {
        const float sign = key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN ? -1.0f : 1.0f;
        editStep(sign * ((mods & GLFW_MOD_SHIFT) ? .1f : 1.0f),out);
    }

}

void Assembly::onFocus(bool focused) {
    if (!focused) {
        finishEdit(false,std::cout);
        for (auto& binding : state_.bindings) binding.down = false;
        state_.dragging = false; state_.dragX.reset();
    }
    dispatchInput([&](const Behavior& b) { if (b.focus) b.focus(*this, focused); return false; }, std::cout);
}

void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {
    if (event.button == GLFW_MOUSE_BUTTON_RIGHT && event.action == GLFW_RELEASE) {
        state_.dragging = false; state_.dragX.reset();
    }
    if (dispatchInput([&](const Behavior& b) { return b.mouse && b.mouse(*this, event); }, out)) return;
    if (event.button == GLFW_MOUSE_BUTTON_RIGHT && !state_.dragTarget.empty()) {
        state_.dragging = event.action == GLFW_PRESS;
        state_.dragX.reset();
        return;
    }
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
    if (editStart_) { finishEdit(true,out); return; }
    std::optional<graphics::PickResult> hit;
    const auto setPlaneCursor = [&](const graphics::Camera& camera, const graphics::PickViewport& viewport) {
        const auto point = graphics::PatternSystem::screenOnPlane(camera,event.framebufferX,event.framebufferY,
            viewport,state_.planePoint,state_.planeNormal);
        if (point) state_.cursor = *point;
    };
    if (state_.threeViews) {
        const auto size = app_.getFramebufferSize();
        if (size[0] < 3 || size[1] < 2) return;
        for (const auto& pass : graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions())) {
            const auto& v = pass.viewport;
            const double top = size[1] - v.y - v.height;
            if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top && event.framebufferY < top + v.height)
            {
                setPlaneCursor(pass.camera,{double(v.x),top,double(v.width),double(v.height)});
                hit = graphics::pick(app_.getScene(), pass.camera, event.framebufferX, event.framebufferY,
                    {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
            }
        }
    } else {
        const auto size=app_.getFramebufferSize();
        setPlaneCursor(app_.getCamera(),{0,0,double(size[0]),double(size[1])});
        hit=app_.pickAt(event.windowX,event.windowY);
    }
    if (hit) state_.cursor = hit->worldPoint;
    if (state_.snapSpacing > 0) state_.cursor = graphics::PatternSystem::snap(state_.cursor,state_.snapSpacing);
    state_.selectedLayer.clear();
    state_.faceSelection = state_.editMode == "face" ? hit : std::nullopt;
    state_.selected.clear();
    std::optional<graphics::EntityId> entity = hit ? std::optional<graphics::EntityId>{hit->entityId} : std::nullopt;
    while (entity) {
        for (const auto& item : state_.nodes) {
            const bool selectable = state_.editMode == "object" ? item.second.whole : true;
            if (item.second.id == *entity && selectable) { state_.selected = item.first; break; }
        }
        if (!state_.selected.empty()) break;
        const auto* component = app_.getScene().getComponent(*entity);
        entity = component ? component->parent : std::nullopt;
    }
    if ((event.mods & GLFW_MOD_CONTROL) && !state_.selected.empty() && state_.editMode=="object") {
        auto parent=app_.getScene().getComponent(node(state_.selected).id)->parent;
        bool found=false;
        while (parent && !found) {
            for (const auto& item:state_.nodes)
                if(item.second.id==*parent && item.second.whole) {state_.selected=item.first;found=true;break;}
            if(!found) parent=app_.getScene().getComponent(*parent)->parent;
        }
    }
    out << "cursor [생성 기준점] - 월드 XYZ=" << state_.cursor.x << ',' << state_.cursor.y << ',' << state_.cursor.z << '\n';
    if (state_.faceSelection) out << "face [세부 면 선택] - entity=" << state_.faceSelection->entityId << ", face=" << state_.faceSelection->faceIndex << '\n';
    if (!state_.selected.empty()) {
        inspect(state_.selected, out);
        return;
    }
    out << "선택 해제\n";
}
void Assembly::onCursor(double x, double y, std::ostream& out) {
    if (editStart_ && std::isfinite(x)) {
        if (editCursor_) editStep(float(x-*editCursor_)*.1f,out);
        editCursor_=x;
        return;
    }
    if (dispatchInput([&](const Behavior& b) { return b.cursor && b.cursor(*this, x, y); }, out)) return;
    if (!state_.dragging || !std::isfinite(x)) return;
    try {
        if (state_.dragX) rotate(state_.dragTarget, state_.dragAxis, float(x - *state_.dragX) * state_.dragSensitivity);
        state_.dragX = x;
    } catch (const std::exception& error) {
        state_.dragging = false; state_.dragX.reset();
        out << "드래그 거절: " << error.what() << '\n';
    }
}
} // namespace assembly
