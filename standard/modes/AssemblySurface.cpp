#include "Assembly.h"
#include <algorithm>
#include <sstream>

namespace assembly {
namespace {
glm::mat3 frame(glm::vec3 forward, glm::vec3 up) {
    require(graphics::detail::finiteVector(forward) && graphics::detail::finiteVector(up) &&
            glm::length(forward) > .0001f && glm::length(up) > .0001f, "0이 아닌 유한한 방향이 필요합니다.");
    const auto z = glm::normalize(forward), cross = glm::cross(up, z);
    require(glm::length(cross) > .0001f, "normal과 up은 평행할 수 없습니다.");
    const auto x = glm::normalize(cross);
    return glm::mat3(x, glm::cross(z, x), z);
}
}

graphics::EntityId Assembly::meshEntity(const std::string& name) const {
    const auto& object = node(name);
    require(object.mesh.has_value(), "메시가 있는 객체/부품을 지정하세요.");
    return *object.mesh;
}

void Assembly::requireFeatureEditable(graphics::EntityId mesh) const {
    for (const auto& f : state_.features)
        for (const auto& part : f.entities)
            require(part.second.mesh != mesh, "라이브 기능의 형상/면 속성은 native 컨트롤러 소유입니다. feature snapshot으로 독립 복사 후 편집하세요.");
}

void Assembly::requireGeometryEditable(graphics::EntityId mesh) const {
    requireFeatureEditable(mesh);
    for (const auto& b : state_.behaviors) {
        if (b.effect != Effect::Body && b.effect != Effect::Walk) continue;
        require(node(b.reference).mesh != mesh && (b.effect != Effect::Body || node(b.target).mesh != mesh),
                "물리에 사용 중인 형상은 편집할 수 없습니다. 물리를 remove한 뒤 재구성하세요.");
    }
}

void Assembly::requireFaceEditable(graphics::EntityId mesh, std::size_t face) const {
    requireGeometryEditable(mesh);
    for (const auto& b : state_.behaviors)
        require(!faceEffect(b.effect) || node(b.target).mesh != mesh || b.faceIndex != face,
                "해당 면의 동작을 remove한 뒤 기준 형상을 편집하세요.");
}

void Assembly::model(const std::string& name, const graphics::Model& geometry, glm::vec3 position) {
    require(graphics::DiagramSystem::validPolyhedron(geometry), "삼각형·색·노멀·인덱스를 갖춘 유효한 모델이 필요합니다.");
    group(name, position);
    const auto mesh = app_.getScene().push_entity(geometry);
    require(mesh.has_value(), "사용자 모델 생성 실패.");
    require(app_.getScene().setParent(*mesh, node(name).id), "사용자 모델 연결 실패.");
    node(name).mesh = *mesh;
    node(name).guideTopic = "geometry";
    registerNode(name + ".mesh", *mesh, false, "mesh", *mesh);
}

std::string Assembly::mount(const std::string& parent, const std::string& localName, FaceMount attachment) {
    attachment.mesh = meshEntity(parent);
    const auto* mesh = app_.getScene().getMesh(attachment.mesh, attachment.face);
    require(mesh != nullptr, "부착할 면 번호를 확인하세요. surface <객체> <면>");
    const auto indices = graphics::detail::triangles(*mesh);
    require(attachment.triangle < indices.size() / 3, "부착 삼각형 번호가 범위를 벗어났습니다.");
    if (attachment.barycentric)
        require(attachment.coordinate.x >= 0 && attachment.coordinate.y >= 0 &&
                attachment.coordinate.x + attachment.coordinate.y <= 1, "삼각형 좌표는 U,V>=0, U+V<=1입니다.");
    require(graphics::detail::finiteVector(attachment.coordinate), "유한한 부착 좌표가 필요합니다.");
    const auto name = part("joint", parent, localName, {0, 0, 0});
    require(app_.getScene().setParent(node(name).id, attachment.mesh), "면 부착 부모 연결 실패.");
    node(name).mount = attachment;
    return name;
}

void Assembly::applyMounts() {
    auto& scene = app_.getScene();
    std::vector<std::pair<std::size_t, std::string>> ordered;
    for (const auto& item : state_.nodes) {
        if (!item.second.mount) continue;
        std::size_t depth = 0;
        auto id = scene.getComponent(item.second.id)->parent;
        while (id) { ++depth; id = scene.getComponent(*id)->parent; }
        ordered.emplace_back(depth, item.first);
    }
    std::sort(ordered.begin(), ordered.end());
    for (const auto& item : ordered) {
        const auto& object = node(item.second);
        const auto& a = *object.mount;
        const auto* mesh = scene.getMesh(a.mesh, a.face);
        require(mesh != nullptr, "부착한 면이 없습니다.");
        const auto indices = graphics::detail::triangles(*mesh);
        require(a.triangle < indices.size() / 3, "부착 삼각형이 변경됐습니다.");
        const auto p0 = mesh->localVertices[indices[a.triangle * 3]],
                   p1 = mesh->localVertices[indices[a.triangle * 3 + 1]],
                   p2 = mesh->localVertices[indices[a.triangle * 3 + 2]];
        const auto world = scene.worldMatrix(a.mesh), faceWorld = scene.faceWorldMatrix(a.mesh, a.face);
        require(world && faceWorld, "부착할 면의 행렬이 없습니다.");
        const auto inverse = glm::inverse(*world);
        require(graphics::TransformSystem::validAffine(inverse), "특이한 부모에는 관절을 부착할 수 없습니다.");
        const auto local = inverse * *faceWorld;
        const auto edge = glm::normalize(glm::vec3(local * glm::vec4(p1 - p0, 0)));
        const auto normal = glm::cross(edge, glm::normalize(glm::vec3(local * glm::vec4(p2 - p0, 0))));
        const auto rotation = frame(normal, edge);
        const auto point = a.barycentric ? p0 + (p1 - p0) * a.coordinate.x + (p2 - p0) * a.coordinate.y : a.coordinate;
        const auto anchor = glm::vec3(local * glm::vec4(point, 1)) + (a.barycentric ? rotation[2] * a.coordinate.z : glm::vec3(0));
        const auto pose = glm::translate(glm::mat4(1), anchor) * glm::mat4(rotation) *
            graphics::DiagramSystem::modelMatrix(scene.getComponent(object.id)->transform);
        auto result = object.rest;
        result.localBasis = {0, 0, 0};
        result.basis = glm::vec3(pose[3]);
        result.rotation = glm::mat3(pose) / result.scale.x;
        require(scene.setTransform(object.id, result), "면 관절 자세 합성 실패.");
    }
}

bool Assembly::surfaceCommand(const Tokens& a, std::ostream& out) {
    const auto& command = a[0];
    if (command == "warp") {
        require(a.size() == 5, "warp <group> <SX> <SY> <SZ>. 자식 group/joint에 동작을 붙이세요.");
        auto& object = node(a[1]);
        require(object.kind == "group" && !object.mount, "warp는 일반 group에 적용합니다.");
        for (const auto& b : state_.behaviors) require(b.target != a[1], "동작이 없는 외곽 group을 warp로 사용하세요.");
        object.affineGroup = true;
        auto rest = object.rest; rest.scale = vector(a, 2); setRest(a[1], rest);
        return true;
    }
    if (command == "orient") {
        require(a.size() == 8, "orient <대상> <+Z 방향3> <up 방향3>");
        auto rest = node(a[1]).rest; rest.rotation = frame(vector(a, 2), vector(a, 5)); setRest(a[1], rest);
        return true;
    }
    if (command == "mount" || command == "anchor") {
        require(command == "mount" ? a.size() == 8 : a.size() == 7,
                "mount <객체> <관절이름> <면> <삼각형> <U> <V> <법선거리> 또는 anchor <객체> <관절이름> <면> <모델좌표3>");
        FaceMount location;
        location.face = indexNumber(a[3]); location.barycentric = command == "mount";
        location.triangle = command == "mount" ? indexNumber(a[4]) : 0;
        location.coordinate = vector(a, command == "mount" ? 5 : 4);
        mount(a[1], a[2], location);
        return true;
    }
    if (command == "unmount") {
        require(a.size() == 2 && node(a[1]).mount.has_value(), "unmount <면 관절>. 부모 로컬 기준 자세로 돌아갑니다.");
        node(a[1]).mount.reset();
        return true;
    }
    if (command == "triangle" || command == "triangle-add") {
        require(a.size() == 11, "triangle 또는 triangle-add <이름> <A3> <B3> <C3>");
        graphics::Mesh mesh;
        mesh.shape = graphics::GLFace::Triangle;
        mesh.localVertices = {vector(a, 2), vector(a, 5), vector(a, 8)};
        graphics::recalculateNormals(mesh);
        graphics::Model geometry;
        if (command == "triangle-add") {
            const auto id = meshEntity(a[1]); requireGeometryEditable(id);
            geometry = app_.getScene().getComponent(id)->polyhedron;
            geometry.faces.push_back(mesh);
            require(app_.getScene().setModel(id, geometry), "삼각형 추가 실패.");
        } else { geometry.faces.push_back(mesh); model(a[1], geometry); }
        return true;
    }
    if (command == "line" || command == "digit") {
        require(command == "line" ? a.size() == 11 : a.size() == 10,
                "line <부모> <A3> <B3> <RGB> 또는 digit <부모> <0~9> <위치3> <높이> <RGB>");
        const auto color = vector(a, command == "line" ? 8 : 7);
        require(glm::all(glm::greaterThanEqual(color, glm::vec3(0))) && glm::all(glm::lessThanEqual(color, glm::vec3(1))), "색은 0~1입니다.");
        if (command == "line") require(app_.getScene().addLine(vector(a, 2), vector(a, 5),
            {color.x, color.y, color.z}, node(a[1]).id).has_value(), "선 생성 실패.");
        else {
            const auto digit = indexNumber(a[2]); require(digit <= 9, "숫자는 0~9입니다.");
            require(graphics::PatternSystem::addNumber(app_.getScene(), unsigned(digit), vector(a, 3), float(number(a[6])),
                {color.x, color.y, color.z}, node(a[1]).id), "숫자 생성 실패.");
        }
        return true;
    }
    const std::vector<std::string> supported{"surface", "normal", "winding", "vertex", "face-srt", "face-mode", "facecolor", "outline", "cull", "visible"};
    if (std::find(supported.begin(), supported.end(), command) == supported.end()) return false;
    require(a.size() >= 3, command + " <객체> <면/all> ...");
    auto& scene = app_.getScene();
    std::vector<std::pair<graphics::EntityId, std::size_t>> faces;
    if (a[2] == "all") {
        const auto root = node(a[1]).id;
        for (auto mesh : scene.getIds()) {
            std::optional<graphics::EntityId> ancestor = mesh;
            while (ancestor && *ancestor != root) ancestor = scene.getComponent(*ancestor)->parent;
            if (!ancestor) continue;
            for (std::size_t i = 0; i < scene.getComponent(mesh)->polyhedron.faces.size(); ++i) faces.emplace_back(mesh, i);
        }
    } else {
        const auto id = meshEntity(a[1]), face = indexNumber(a[2]);
        require(scene.getMesh(id, face) != nullptr, "면 번호 범위를 확인하세요."); faces.emplace_back(id, face);
    }
    require(!faces.empty(), "편집할 면이 없습니다.");
    for (const auto& location : faces) {
        const auto id = location.first, f = location.second;
        auto mesh = *scene.getMesh(id, f);
        if (command == "surface") {
            require(a.size() == 3, "surface <객체> <면/all>");
            out << a[1] << " mesh=" << id << " face=" << f << " triangles=" << graphics::detail::triangles(mesh).size() / 3 << '\n';
            for (std::size_t i = 0; i < mesh.localVertices.size(); ++i) {
                const auto p = mesh.localVertices[i]; out << "  v" << i << '=' << p.x << ',' << p.y << ',' << p.z << '\n';
            }
            const auto triangles = graphics::detail::triangles(mesh);
            for (std::size_t i = 0; i < triangles.size(); i += 3)
                out << "  triangle " << i / 3 << ": " << triangles[i] << ',' << triangles[i + 1] << ',' << triangles[i + 2] << '\n';
            if (!mesh.vertexNormals.empty()) {
                const auto n = mesh.vertexNormals.front(); out << "  first shading normal=" << n.x << ',' << n.y << ',' << n.z << '\n';
            }
            continue;
        }
        requireFeatureEditable(id);
        if (command == "cull" || command == "visible") {
            require(a.size() == 4, "cull/visible <객체> <면/all> <값>");
            if (command == "visible") {
                for (const auto& b : state_.behaviors)
                    require(!faceEffect(b.effect) || node(b.target).mesh != id || b.faceIndex != f,
                            "면 동작을 remove한 뒤 가시성을 변경하세요.");
                require(scene.setFaceVisible(id, f, boolean(a[3])), "면 가시성 변경 실패.");
            } else {
                require(a[3] == "inherit" || a[3] == "none" || a[3] == "back" || a[3] == "front", "컬링은 inherit/none/back/front입니다.");
                require(scene.setFaceCullMode(id, f, a[3] == "inherit" ? graphics::CullMode::Inherit : a[3] == "none" ? graphics::CullMode::None : a[3] == "back" ? graphics::CullMode::Back : graphics::CullMode::Front), "면 컬링 변경 실패.");
            }
        } else if (command == "normal") {
            require(a.size() == 6 || (a.size() == 4 && (a[3] == "auto" || a[3] == "flip")),
                    "normal <객체> <면/all> <NX NY NZ 또는 auto/flip>");
            if (a.size() == 6) {
                const auto n = vector(a, 3); require(glm::length(n) > .0001f, "0 노멀은 허용하지 않습니다.");
                mesh.normal = glm::normalize(n); mesh.vertexNormals.assign(mesh.localVertices.size(), mesh.normal);
            } else {
                if (a[3] == "auto" || mesh.vertexNormals.empty()) graphics::recalculateNormals(mesh);
                if (a[3] == "flip") { for (auto& n : mesh.vertexNormals) n = -n; mesh.normal = -mesh.normal; }
            }
            require(scene.setMesh(id, f, mesh), "노멀 변경 실패.");
        } else if (command == "facecolor") {
            require(a.size() == 6, "facecolor <객체> <면/all> <R G B>");
            const auto color = vector(a, 3);
            require(glm::all(glm::greaterThanEqual(color, glm::vec3(0))) && glm::all(glm::lessThanEqual(color, glm::vec3(1))), "색은 0~1입니다.");
            require(scene.setFaceVertexColors(id, f, std::vector<graphics::GlColor>(mesh.localVertices.size(), {color.x, color.y, color.z})), "면 색 변경 실패.");
        } else if (command == "outline") {
            require(a.size() == 4 && (a[3] == "none" || a[3] == "features" || a[3] == "triangles"), "outline <객체> <면/all> <none/features/triangles>");
            require(scene.setOutline(id, f, a[3] == "none" ? graphics::OutlineMode::None : a[3] == "features" ? graphics::OutlineMode::FeatureEdges : graphics::OutlineMode::AllTriangleEdges), "윤곽 변경 실패.");
        } else {
            requireFaceEditable(id, f);
            if (command == "winding") {
                require(a.size() == 4 && a[3] == "flip", "winding <객체> <면/all> flip. 조명 노멀은 별도입니다.");
                mesh.triangleIndices = graphics::detail::triangles(mesh);
                for (std::size_t i = 0; i < mesh.triangleIndices.size(); i += 3) std::swap(mesh.triangleIndices[i + 1], mesh.triangleIndices[i + 2]);
                require(scene.setMesh(id, f, mesh), "winding 변경 실패.");
            } else if (command == "vertex") {
                require(a.size() == 7 && faces.size() == 1, "vertex <객체> <면> <정점> <XYZ>");
                require(scene.setVertexPosition(id, f, indexNumber(a[3]), vector(a, 4)), "정점 변경 실패.");
            } else if (command == "face-mode") {
                require(a.size() == 4 && (a[3] == "hierarchy" || a[3] == "independent"), "face-mode <객체> <면/all> <hierarchy/independent>");
                require(scene.setFaceTransformMode(id, f, a[3] == "hierarchy" ? graphics::FaceTransformMode::Hierarchy : graphics::FaceTransformMode::Independent, true), "면 종속 변경 실패.");
            } else {
                require(a.size() == 15, "face-srt <객체> <면/all> <이동3> <회전도3> <배율3> <중심3>");
                graphics::Transform3D t;
                t.basis = vector(a, 3); t.scale = vector(a, 9); t.localBasis = vector(a, 12);
                const auto r = glm::radians(vector(a, 6));
                t.rotation = glm::mat3(glm::rotate(glm::mat4(1), r.z, glm::vec3(0, 0, 1)) * glm::rotate(glm::mat4(1), r.y, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1), r.x, glm::vec3(1, 0, 0)));
                require(glm::all(glm::greaterThan(glm::abs(t.scale), glm::vec3(.0001f))) && glm::all(glm::lessThanEqual(glm::abs(t.scale), glm::vec3(100))), "면 배율은 0이 아니며 절댓값 .0001~100입니다.");
                require(scene.setFaceSRT(id, f, t), "면 SRT 변경 실패.");
            }
        }
    }
    return true;
}
} // namespace assembly
