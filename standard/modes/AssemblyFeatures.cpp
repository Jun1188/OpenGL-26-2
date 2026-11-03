#include "Assembly.h"
#include <algorithm>
#include <limits>
#include <set>

namespace assembly {
namespace {
bool contains(const FeatureInstance& feature, graphics::EntityId id) {
    if (!feature.branch) return true;
    std::optional<graphics::EntityId> current = id;
    while (current) {
        if (*current == *feature.branch) return true;
        const auto* component = feature.program->scene().getComponent(*current);
        current = component ? component->parent : std::nullopt;
    }
    return false;
}
std::string entityName(const FeatureInstance& f, graphics::EntityId id) {
    return f.name + ".e" + std::to_string(id);
}
graphics::Model exportedModel(const graphics::Model& source) {
    auto result = source;
    for (auto& face : result.faces) {
        face.independence = false;
        face.transform = graphics::Transform3D{};
        face.transformMode = graphics::FaceTransformMode::Hierarchy;
        face.transformFrame = glm::mat4(1);
        face.localAnimationMatrix = glm::mat4(1);
        face.animationSpace = graphics::FaceAnimationSpace::Model;
    }
    return result;
}
std::optional<graphics::Transform3D> srtMatrix(const glm::mat4& matrix) {
    graphics::Transform3D t;
    t.basis = glm::vec3(matrix[3]);
    t.scale = {glm::length(glm::vec3(matrix[0])), glm::length(glm::vec3(matrix[1])), glm::length(glm::vec3(matrix[2]))};
    if (glm::any(glm::lessThanEqual(t.scale, glm::vec3(1e-8f)))) return {};
    if (glm::determinant(glm::mat3(matrix)) < 0) t.scale.x = -t.scale.x;
    for (int i = 0; i < 3; ++i) t.rotation[i] = glm::vec3(matrix[i]) / t.scale[i];
    return graphics::DiagramSystem::validTransform(t) ? std::optional<graphics::Transform3D>{t} : std::nullopt;
}
}

void Assembly::addFeature(const std::string& name, int mode) {
    require(state_.features.size() < 16 && mode >= 1 && mode <= lastStandardMode, "기능은 모드1~22, 인스턴스16개 이하여야 합니다.");
    group(name, {0, 0, 0});
    node(name).guideTopic = "mode" + std::to_string(mode);
    FeatureInstance feature;
    feature.name = name; feature.program = std::make_shared<FeatureContainer>(app_, mode);
    state_.features.push_back(std::move(feature));
}

void Assembly::portFeature(const std::string& source, graphics::EntityId native, const std::string& name) {
    require(state_.features.size() < 16, "기능 인스턴스16개 한도입니다.");
    const auto found = std::find_if(state_.features.begin(), state_.features.end(), [&](const auto& f) { return f.name == source; });
    require(found != state_.features.end() && found->program->scene().getComponent(native), "원본 기능과 native 엔티티 ID를 확인하세요.");
    FeatureInstance port;
    port.name = name; port.program = found->program; port.branch = native; port.playing = found->playing;
    group(name, {0, 0, 0}); node(name).guideTopic = "mode" + std::to_string(port.program->mode());
    state_.features.push_back(std::move(port));
}

bool Assembly::featureCommand(const Tokens& a, std::ostream& out) {
    if (a[0] != "feature") return false;
    require(a.size() >= 2, "feature add/port/list/parts/focus/camera/play/enable/reset/key ...");
    const auto find = [&](const std::string& name) -> FeatureInstance& {
        const auto it = std::find_if(state_.features.begin(), state_.features.end(), [&](const auto& f) { return f.name == name; });
        require(it != state_.features.end(), "없는 기능 컨테이너입니다: " + name);
        return *it;
    };
    if (a[1] == "add") {
        require(a.size() == 4 && state_.features.size() < 16, "feature add <이름> <모드1~22>. 컨테이너 한도16개.");
        const auto mode = indexNumber(a[3]);
        require(mode >= 1 && mode <= lastStandardMode, "feature 모드는 1~22입니다.");
        addFeature(a[2], int(mode));
        return true;
    }
    if (a[1] == "snapshot") {
        require(a.size() == 5, "feature snapshot <컨테이너> <nativeID> <새이름>");
        const auto& f = find(a[2]); const auto id = indexNumber(a[3]);
        const auto* c = f.program->scene().getComponent(id);
        require(c && !c->polyhedron.faces.empty(), "메시가 있는 native 엔티티 ID가 필요합니다.");
        auto geometry = exportedModel(c->polyhedron);
        for (std::size_t i = 0; i < geometry.faces.size(); ++i) {
            const auto matrix = f.program->scene().faceWorldMatrix(id, i);
            require(matrix.has_value(), "스냅샷 면 행렬이 없습니다.");
            const auto determinant = glm::determinant(glm::mat3(*matrix));
            require(std::abs(determinant) > 1e-18f, "특이한 면은 스냅샷으로 만들 수 없습니다.");
            auto& face = geometry.faces[i];
            if (face.vertexNormals.empty()) graphics::recalculateNormals(face);
            const auto normal = glm::transpose(glm::inverse(glm::mat3(*matrix)));
            for (auto& p : face.localVertices) p = glm::vec3(*matrix * glm::vec4(p, 1));
            for (auto& n : face.vertexNormals) if (glm::length(n) > 0) n = glm::normalize(normal * n);
            face.normal = glm::length(face.normal) > 0 ? glm::normalize(normal * face.normal) : glm::vec3(0);
            if (determinant < 0) {
                face.triangleIndices = graphics::detail::triangles(face);
                for (std::size_t t = 0; t < face.triangleIndices.size(); t += 3) std::swap(face.triangleIndices[t + 1], face.triangleIndices[t + 2]);
            }
        }
        model(a[4], geometry);
        require(app_.getScene().setTint(*node(a[4]).mesh, c->tint), "스냅샷 색 적용 실패.");
        require(app_.getScene().setParent(node(a[4]).id, node(f.name).id), "스냅샷 배치 부모 연결 실패.");
        return true;
    }
    if (a[1] == "port") {
        require(a.size() == 5 && state_.features.size() < 16, "feature port <기존이름> <native엔티티ID> <새이름>");
        portFeature(a[2], indexNumber(a[3]), a[4]);
        out << "라이브 포트는 원본의 상태/시계를 공유합니다. 독립 인스턴스는 feature add를 사용하세요.\n";
        return true;
    }
    if (a[1] == "list") {
        require(a.size() == 2, "feature list");
        for (const auto& f : state_.features)
            out << f.name << " mode=" << f.program->mode() << " enabled=" << f.enabled << " playing=" << f.playing
                << (f.branch ? " live-port" : " independent") << '\n';
        return true;
    }
    require(a.size() >= 3, "feature <동작> <이름 또는 none> ...");
    if (a[1] == "focus" || a[1] == "camera") {
        require(a.size() == 3, "feature focus/camera <이름 또는 none>");
        if (a[2] != "none") require(find(a[2]).enabled, "enabled 컨테이너를 선택하세요.");
        if (a[1] == "focus") {
            queueFeatureFocus(false);
            state_.featureFocus = a[2] == "none" ? "" : a[2];
            queueFeatureFocus(true);
        } else state_.featureCamera = a[2] == "none" ? "" : a[2];
        return true;
    }
    auto& f = find(a[2]);
    if (a[1] == "parts") {
        require(a.size() == 3, "feature parts <이름>");
        const auto& scene = f.program->scene();
        for (auto id : scene.getIds()) if (contains(f, id)) {
            const auto* c = scene.getComponent(id);
            out << entityName(f, id) << " native=" << id << " faces=" << c->polyhedron.faces.size();
            if (c->parent) out << " nativeParent=" << *c->parent;
            out << '\n';
        }
        return true;
    }
    if (a[1] == "play" || a[1] == "enable") {
        require(a.size() == 4, "feature play/enable <이름> on/off");
        const bool value = boolean(a[3]);
        if (a[1] == "play") {
            for (auto& item : state_.features) if (item.program == f.program) item.playing = value;
        } else {
            f.enabled = value;
            if (!value && state_.featureFocus == f.name) { queueFeatureFocus(false); state_.featureFocus.clear(); }
            if (!value && state_.featureCamera == f.name) state_.featureCamera.clear();
        }
        return true;
    }
    if (a[1] == "reset") {
        require(a.size() == 3, "feature reset <이름>. 모든 공유 라이브 포트가 함께 초기화됩니다.");
        const auto previous = f.program;
        const auto replacement = std::make_shared<FeatureContainer>(app_, previous->mode());
        for (auto& item : state_.features) if (item.program == previous) {
            item.program = replacement; item.events.clear(); item.revision = 0;
        }
        return true;
    }
    if (a[1] == "key") {
        require(a.size() == 5 && (a[4] == "press" || a[4] == "release"), "feature key <이름> <키/수정키+키> press/release");
        const auto key = keyCode(a[3], true);
        require(key.first != GLFW_KEY_Q && key.first != GLFW_KEY_ESCAPE, "Q/Esc 대신 feature enable을 사용하세요.");
        FeatureEvent event; event.kind = FeatureEvent::Kind::Key;
        event.key = key.first; event.mods = key.second; event.action = a[4] == "press" ? GLFW_PRESS : GLFW_RELEASE;
        queueFeatureEvent(f, event);
        return true;
    }
    require(false, "지원하는 feature 동작: add port list parts focus camera play enable reset key");
    return false;
}

void Assembly::advanceFeatures(float dt) {
    std::set<FeatureContainer*> advanced;
    for (auto& instance : state_.features) {
        if (!advanced.insert(instance.program.get()).second) continue;
        bool playing = false;
        for (auto& f : state_.features) if (f.program == instance.program) {
            playing = playing || (f.enabled && f.playing);
            for (const auto& event : f.events) {
                switch (event.kind) {
                case FeatureEvent::Kind::Pick: f.program->pick(event.hit); break;
                case FeatureEvent::Kind::Key: f.program->key(event.key, event.action, event.mods); break;
                case FeatureEvent::Kind::Mouse: f.program->mouse(event.mouse, event.view); break;
                case FeatureEvent::Kind::Cursor: f.program->cursor(event.x, event.y); break;
                case FeatureEvent::Kind::Focus: f.program->focus(event.focused); break;
                }
            }
            f.events.clear();
        }
        instance.program->step(!state_.paused && playing ? dt : 0);
    }
}

void Assembly::featureAppearance(graphics::EntityId id, std::optional<graphics::GlColor> color, std::optional<bool> visible) {
    for (auto& f : state_.features) {
        for (auto& part : f.entities) if (part.second.mesh == id) {
            if (color) part.second.tint = *color;
            if (visible) part.second.visible = *visible;
        }
        for (auto& line : f.lines) if (line.second.frame == id && visible) line.second.visible = *visible;
    }
}

void Assembly::syncFeatures() {
    auto& scene = app_.getScene();
    for (auto& feature : state_.features) {
        const auto& source = feature.program->scene();
        const auto revision = source.getGeometryRevision();
        const auto root = node(feature.name).id;
        std::set<graphics::EntityId> active;
        for (auto native : source.getIds()) {
            if (!contains(feature, native)) continue;
            active.insert(native);
            const auto* c = source.getComponent(native);
            auto it = feature.entities.find(native);
            if (it == feature.entities.end()) {
                const auto group = scene.addGroup(); require(group.has_value(), "기능 엔티티 포트 생성 실패.");
                FeatureEntity entity; entity.group = *group;
                registerNode(entityName(feature, native), *group, false, "feature-part");
                node(entityName(feature, native)).guideTopic = node(feature.name).guideTopic;
                it = feature.entities.emplace(native, entity).first;
            }
            auto& entity = it->second;
            if (c->polyhedron.faces.empty()) {
                if (entity.mesh) require(scene.setVisible(*entity.mesh, false), "기능의 이전 형상 숨기기 실패.");
                continue;
            }
            if (!entity.mesh) {
                const auto mesh = scene.push_entity(exportedModel(c->polyhedron)); require(mesh.has_value(), "기능 메시 포트 생성 실패.");
                require(scene.setParent(*mesh, entity.group), "기능 메시 부모 연결 실패.");
                entity.mesh = mesh; node(entityName(feature, native)).mesh = mesh;
            } else if (feature.revision != revision || scene.getComponent(*entity.mesh)->polyhedron.faces.size() != c->polyhedron.faces.size()) {
                auto geometry = exportedModel(c->polyhedron);
                const auto old = scene.getComponent(*entity.mesh)->polyhedron;
                for (std::size_t face = 0; face < std::min(old.faces.size(), geometry.faces.size()); ++face) {
                    geometry.faces[face].localAnimationMatrix = old.faces[face].localAnimationMatrix;
                    geometry.faces[face].animationSpace = old.faces[face].animationSpace;
                }
                require(scene.setModel(*entity.mesh, geometry), "기능 형상 갱신 실패.");
            }
            require(scene.setTint(*entity.mesh, {c->tint.red * entity.tint.red, c->tint.green * entity.tint.green, c->tint.blue * entity.tint.blue}) && scene.setVisible(*entity.mesh, feature.enabled && entity.visible && c->visible), "기능 표시 갱신 실패.");
            for (std::size_t face = 0; face < c->polyhedron.faces.size(); ++face) {
                const auto matrix = source.faceWorldMatrix(native, face);
                require(matrix && scene.setFaceFrame(*entity.mesh, face, *matrix), "기능 면 자세 갱신 실패.");
                const auto& original = c->polyhedron.faces[face];
                const auto cull = original.cull == graphics::CullMode::Inherit ? feature.program->options().cull : original.cull;
                require(scene.setFaceVisible(*entity.mesh, face, original.visible) && scene.setFaceCullMode(*entity.mesh, face, cull), "기능 면 표시 갱신 실패.");
            }
        }
        for (auto& item : feature.entities) {
            auto& entity = item.second;
            const auto* native = source.getComponent(item.first);
            if (!active.count(item.first) || !native) {
                if (entity.mesh) require(scene.setVisible(*entity.mesh, false), "이전 기능 메시 숨기기 실패.");
                continue;
            }
            if (!entity.parentKnown || entity.nativeParent != native->parent) {
                const auto found = native->parent ? feature.entities.find(*native->parent) : feature.entities.end();
                require(scene.setParent(entity.group, found == feature.entities.end() ? root : found->second.group), "기능 계층 연결 실패.");
                entity.nativeParent = native->parent; entity.parentKnown = true;
            }
        }
        const auto& lines = source.getLines();
        for (std::size_t i = 0; i < lines.size(); ++i) {
            const auto& line = lines[i];
            if (feature.branch && (!line.owner || !contains(feature, *line.owner))) continue;
            auto it = feature.lines.find(i);
            if (it == feature.lines.end()) {
                const auto frame = scene.addGroup(); require(frame.has_value(), "기능 선 프레임 생성 실패.");
                require(scene.setParent(*frame, root), "기능 선 부모 실패.");
                const auto id = scene.addLine(line.start, line.end, line.color, *frame); require(id.has_value(), "기능 선 생성 실패.");
                it = feature.lines.emplace(i, FeatureLine{*id, *frame, line.start, line.end}).first;
            }
            auto& output = it->second;
            const auto matrix = source.lineWorldMatrix(i); require(matrix.has_value(), "기능 선 행렬 실패.");
            auto transform = srtMatrix(*matrix);
            if (transform) {
                if (output.start != line.start || output.end != line.end) {
                    require(scene.setVector(output.line, line.start, line.end - line.start), "선 끝점 갱신 실패.");
                    output.start = line.start; output.end = line.end;
                }
            } else {
                transform = graphics::Transform3D{};
                const auto start = glm::vec3(*matrix * glm::vec4(line.start, 1)), end = glm::vec3(*matrix * glm::vec4(line.end, 1));
                if (output.start != start || output.end != end) {
                    require(scene.setVector(output.line, start, end - start), "아핀 선 갱신 실패.");
                    output.start = start; output.end = end;
                }
            }
            if (!feature.enabled || !output.visible) transform->scale = glm::vec3(0);
            require(scene.setTransform(output.frame, *transform), "기능 선 자세 실패.");
        }
        for (auto& item : feature.lines) if (item.first >= lines.size()) {
            auto hidden = graphics::Transform3D{}; hidden.scale = glm::vec3(0);
            require(scene.setTransform(item.second.frame, hidden), "이전 선 숨기기 실패.");
        }
        feature.revision = revision;
    }
    if (!state_.featureCamera.empty()) {
        const auto it = std::find_if(state_.features.begin(), state_.features.end(), [&](const auto& f) { return f.name == state_.featureCamera; });
        require(it != state_.features.end(), "카메라 소유 컨테이너가 없습니다.");
        const auto matrix = scene.worldMatrix(node(it->name).id); require(matrix.has_value(), "카메라 컨테이너 행렬 실패.");
        const auto scale = glm::length(glm::vec3((*matrix)[0]));
        require(scale > .0001f, "카메라 포트 배율이 너무 작습니다.");
        const auto frame = *matrix * glm::scale(glm::mat4(1), glm::vec3(1 / scale));
        require(rigid(frame), "카메라 포트의 부모 계층은 양수 균일 SRT여야 합니다.");
        auto camera = it->program->camera();
        camera.position = glm::vec3(*matrix * glm::vec4(camera.position, 1));
        camera.target = glm::vec3(*matrix * glm::vec4(camera.target, 1));
        camera.up = glm::normalize(glm::mat3(*matrix) * camera.up);
        camera.nearPlane *= scale; camera.farPlane *= scale; camera.orthographicHalfExtent *= scale;
        require(graphics::CameraSystem::validCamera(camera), "컨테이너 카메라가 유효하지 않습니다.");
        app_.getCamera() = camera; app_.getRenderOptions() = it->program->options();
    }
}
} // namespace assembly
