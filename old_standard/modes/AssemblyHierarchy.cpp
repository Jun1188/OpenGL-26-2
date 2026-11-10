#include "Assembly.h"
#include <limits>

namespace assembly {
void Assembly::group(const std::string& name, glm::vec3 position) {
    add("joint", name, position);
    node(name).kind = "group";
}

std::string Assembly::part(const std::string& kind, const std::string& parentName, const std::string& localName,
                           glm::vec3 position, glm::vec3 size) {
    require(!localName.empty() && localName.find('.') == std::string::npos, "부품 이름은 점 없는 한 구간입니다.");
    const auto parent = node(parentName).id;
    const auto name = parentName + "." + localName;
    add(kind, name, position, size);
    require(app_.getScene().setParent(node(name).id, parent), "부품 부모 연결 실패.");
    node(name).whole = false;
    return name;
}

void Assembly::srt(const std::string& name, glm::vec3 position, glm::vec3 degrees, float scale) {
    auto rest = node(name).rest;
    const auto angles = glm::radians(degrees);
    rest.basis = position;
    rest.rotation = glm::mat3(glm::rotate(glm::mat4(1), angles.z, glm::vec3(0, 0, 1)) *
        glm::rotate(glm::mat4(1), angles.y, glm::vec3(0, 1, 0)) *
        glm::rotate(glm::mat4(1), angles.x, glm::vec3(1, 0, 0)));
    rest.scale = glm::vec3(scale);
    setRest(name, rest);
}

void Assembly::pivot(const std::string& name, glm::vec3 point, bool keep) {
    auto rest = node(name).rest;
    if (keep) rest.basis += rest.rotation * (rest.scale * (point - rest.localBasis));
    rest.localBasis = point;
    setRest(name, rest);
}

glm::vec3 Assembly::center(const std::string& name) const {
    const auto& target = node(name);
    auto& scene = app_.getScene();
    const auto world = scene.worldMatrix(target.id);
    require(world.has_value(), "그룹 행렬이 없습니다.");
    const auto inverse = glm::inverse(*world);
    glm::vec3 low(std::numeric_limits<float>::max()), high(std::numeric_limits<float>::lowest());
    bool geometry = false;
    for (auto id : scene.getIds()) {
        std::optional<graphics::EntityId> parent = id;
        while (parent && *parent != target.id) parent = scene.getComponent(*parent)->parent;
        if (!parent) continue;
        const auto* component = scene.getComponent(id);
        for (std::size_t f = 0; f < component->polyhedron.faces.size(); ++f) {
            const auto matrix = scene.faceWorldMatrix(id, f);
            require(matrix.has_value(), "부품 행렬이 없습니다.");
            for (const auto& vertex : component->polyhedron.faces[f].localVertices) {
                const auto point = glm::vec3(inverse * *matrix * glm::vec4(vertex, 1));
                low = glm::min(low, point); high = glm::max(high, point); geometry = true;
            }
        }
    }
    require(geometry, "중심을 계산할 메시 부품이 없습니다.");
    return (low + high) * .5f;
}

bool Assembly::hierarchyCommand(const Tokens& a, std::ostream&) {
    if (a[0] == "group") {
        require(a.size() == 5, "group <이름> <X> <Y> <Z>");
        group(a[1], vector(a, 2));
        return true;
    }
    if (a[0] == "part" || a[0] == "joint") {
        const bool joint = a[0] == "joint";
        require(joint ? a.size() == 6 : (a.size() == 7 || a.size() == 10),
                "part <종류> <부모> <부품이름> <위치3> [크기3] 또는 joint <부모> <관절이름> <위치3>");
        const std::size_t parentIndex = joint ? 1 : 2;
        part(joint ? "joint" : a[1], a[parentIndex], a[parentIndex + 1], vector(a, parentIndex + 2),
             !joint && a.size() == 10 ? vector(a, 7) : glm::vec3(1));
        return true;
    }
    if (a[0] == "srt") {
        require(a.size() == 9 || a.size() == 11, "srt <대상> <월드 위치3> <회전 도3> <균일배율 또는 SX SY SZ>");
        graphics::Transform3D desired;
        desired.basis = vector(a,2);
        const auto angles=glm::radians(vector(a,5));
        desired.rotation=glm::mat3(glm::rotate(glm::mat4(1),angles.z,glm::vec3(0,0,1)) *
            glm::rotate(glm::mat4(1),angles.y,glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1),angles.x,glm::vec3(1,0,0)));
        desired.scale=a.size()==9 ? glm::vec3(float(number(a[8]))) : vector(a,8);
        setWorld(a[1],graphics::DiagramSystem::modelMatrix(desired));
        return true;
    }
    if (a[0] != "pivot" && a[0] != "center") return false;
    const bool automatic = a[0] == "center" || (a.size() >= 3 && a[2] == "center");
    require(automatic ? (a[0] == "center" ? (a.size() == 2 || a.size() == 3) : (a.size() == 3 || a.size() == 4)) :
        (a.size() == 5 || a.size() == 6), "pivot <대상> <중심3 또는 center> [keep/raw] 또는 center <대상> [keep/raw]");
    const std::size_t option = a[0] == "center" ? 2 : automatic ? 3 : 5;
    const bool keep = a.size() <= option || a[option] == "keep";
    require(a.size() <= option || a[option] == "keep" || a[option] == "raw", "피벗 옵션은 keep 또는 raw입니다.");
    pivot(a[1], automatic ? center(a[1]) : vector(a, 2), keep);
    return true;
}
} // namespace assembly
