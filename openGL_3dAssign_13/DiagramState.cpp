#include "DiagramState.h"

#include <glm/gtc/matrix_transform.hpp>

#include <cmath>
#include <initializer_list>

namespace {

bool finiteVector(const glm::vec3& value) {
    return std::isfinite(value.x) && std::isfinite(value.y)
        && std::isfinite(value.z);
}

void appendFace(Polyhedron& polyhedron, GLFace shape,
                std::initializer_list<glm::vec3> vertices, GlColor color) {
    PlaneFigure face;
    face.shape = shape;
    face.localVertices = vertices;
    face.color = color;
    face.normal = glm::normalize(glm::cross(
        face.localVertices[1] - face.localVertices[0],
        face.localVertices[2] - face.localVertices[0]));
    polyhedron.faces.push_back(face);
}

} // namespace

Entity::Entity() {
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
}

std::optional<EntityId> Entity::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (components_.size() >= MAX_ENTITIES
        || !DiagramSystem::validPolyhedron(polyhedron)
        || !DiagramSystem::validTransform(transform)) {
        return std::nullopt;
    }

    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform });
    polyId_.push_back(id);
    std::vector<FaceId>& faceIds = polyhedron.shape == GLShape::Cube
        ? cubeFaceId_ : tetraFaceId_;
    for (std::size_t i = 0; i < polyhedron.faces.size(); ++i) {
        faceIds.push_back({ id, i });
    }
    ++geometryRevision_;
    return id;
}

const Component_Poly* Entity::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

bool Entity::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true; // 변환만 바뀌므로 로컬 정점 버퍼를 다시 올리지 않는다.
}

bool Entity::setFaceTransform(
    EntityId id, std::size_t faceIndex,
    const Transform3D& transform, bool independence
) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    face.transform = transform;
    face.independence = independence;
    return true;
}

void Entity::clear() {
    if (components_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    cubeFaceId_.clear();
    tetraFaceId_.clear();
    ++geometryRevision_;
}

bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale);
}

bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    const bool cube = polyhedron.shape == GLShape::Cube;
    if (!cube && polyhedron.shape != GLShape::Tetra) {
        return false;
    }
    if (polyhedron.faces.size() != (cube ? 6u : 4u)) {
        return false;
    }
    for (const PlaneFigure& face : polyhedron.faces) {
        if (face.shape != (cube ? GLFace::Rect : GLFace::Triangle)
            || face.localVertices.size() != (cube ? 4u : 3u)
            || !finiteVector(face.normal)
            || !std::isfinite(face.color.red)
            || !std::isfinite(face.color.green)
            || !std::isfinite(face.color.blue)
            || !validTransform(face.transform)) {
            return false;
        }
        for (const glm::vec3& vertex : face.localVertices) {
            if (!finiteVector(vertex)) {
                return false;
            }
        }
    }
    return true;
}

Polyhedron DiagramSystem::makePolyhedron(GLShape shape) {
    Polyhedron polyhedron;
    polyhedron.shape = shape;

    if (shape == GLShape::Cube) {
        polyhedron.faces.reserve(6);
        // 각 면의 네 정점은 바깥에서 볼 때 반시계 방향이다.
        appendFace(polyhedron, GLFace::Rect,
            { { -.5f, -.5f, .5f }, { .5f, -.5f, .5f },
              { .5f, .5f, .5f }, { -.5f, .5f, .5f } }, { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { .5f, -.5f, -.5f }, { -.5f, -.5f, -.5f },
              { -.5f, .5f, -.5f }, { .5f, .5f, -.5f } }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Rect,
            { { -.5f, -.5f, -.5f }, { -.5f, -.5f, .5f },
              { -.5f, .5f, .5f }, { -.5f, .5f, -.5f } }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { .5f, -.5f, .5f }, { .5f, -.5f, -.5f },
              { .5f, .5f, -.5f }, { .5f, .5f, .5f } }, { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { -.5f, .5f, .5f }, { .5f, .5f, .5f },
              { .5f, .5f, -.5f }, { -.5f, .5f, -.5f } }, { .3f, .8f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { -.5f, -.5f, -.5f }, { .5f, -.5f, -.5f },
              { .5f, -.5f, .5f }, { -.5f, -.5f, .5f } }, { .8f, .3f, .9f });
    } else if (shape == GLShape::Tetra) {
        const glm::vec3 top{ 0.0f, .65f, 0.0f };
        const glm::vec3 left{ -.6f, -.35f, .45f };
        const glm::vec3 right{ .6f, -.35f, .45f };
        const glm::vec3 back{ 0.0f, -.35f, -.6f };
        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle,
            { top, left, right }, { .9f, .4f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { top, right, back }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { top, back, left }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { left, back, right }, { .9f, .8f, .3f });
    }
    return polyhedron;
}

glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    // 열벡터 기준: 기준점 이동 -> S -> Rz -> Ry -> Rx -> 월드 위치.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.basis);
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, transform.scale);
    return glm::translate(model, -transform.localBasis);
}

glm::mat4 DiagramSystem::faceModelMatrix(
    const Component_Poly& component, const PlaneFigure& face
) {
    // 독립 면의 transform은 부모에 상대적인 값이 아닌 월드 변환이다.
    return modelMatrix(face.independence ? face.transform : component.transform);
}
