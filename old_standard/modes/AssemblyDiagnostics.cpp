#include "Assembly.h"
#include <iomanip>

namespace assembly {
void Assembly::diagnose(const std::string& target, std::ostream& out) const {
    const auto& object = node(target);
    const auto& scene = app_.getScene();
    inspect(target, out);
    const auto vec = [&](const char* label, const glm::vec3& p) { out << label << '=' << p.x << ',' << p.y << ',' << p.z << '\n'; };
    vec("rest.position (parent pivot position)", object.rest.basis);
    vec("rest.pivot (model)", object.rest.localBasis);
    vec("rest.scale", object.rest.scale);
    out << "rest.rotation determinant=" << glm::determinant(object.rest.rotation) << '\n';
    const auto matrix = scene.worldMatrix(object.id);
    if (matrix) {
        out << "world matrix: rows\n";
        for (int r = 0; r < 4; ++r) out << (*matrix)[0][r] << ' ' << (*matrix)[1][r] << ' ' << (*matrix)[2][r] << ' ' << (*matrix)[3][r] << '\n';
        out << "world determinant=" << glm::determinant(glm::mat3(*matrix)) << " (negative: reflected winding)\n";
    }
    out << "parent chain: ";
    std::optional<graphics::EntityId> ancestor = object.id;
    std::size_t remaining = scene.getIds().size() + 1;
    while (ancestor && remaining--) {
        out << *ancestor << ' ';
        const auto* c = scene.getComponent(*ancestor); ancestor = c ? c->parent : std::nullopt;
    }
    out << '\n';
    if (object.mesh) {
        const auto* c = scene.getComponent(*object.mesh);
        out << "mesh=" << *object.mesh << " visible=" << c->visible << " faces=" << c->polyhedron.faces.size() << '\n';
        for (std::size_t i = 0; i < c->polyhedron.faces.size(); ++i) {
            const auto& f = c->polyhedron.faces[i];
            const auto world = scene.faceWorldMatrix(*object.mesh, i);
            out << "face=" << i << " visible=" << f.visible << " cull=" << int(f.cull) << " vertices=" << f.localVertices.size()
                << " normals=" << f.vertexNormals.size() << " space=" << int(f.animationSpace)
                << " determinant=" << (world ? glm::determinant(glm::mat3(*world)) : 0) << '\n';
        }
    }
    const auto size = app_.getFramebufferSize();
    out << "framebuffer=" << size[0] << 'x' << size[1] << " views=" << (state_.threeViews ? 3 : 1)
        << " paused=" << state_.paused << " faulted=" << faulted_ << '\n';
    out << "pause on [시간 정지] - on/off: 시간 진행 여부\n"
        << "diagnose <이름> [현재 행렬과 상태] - 이름: 객체 또는 부품\n";
}
} // namespace assembly
