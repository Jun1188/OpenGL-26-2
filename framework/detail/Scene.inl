#pragma once
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace graphics {
namespace detail {

inline bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f
        && value.y > 0.0f && value.z > 0.0f;
}

inline std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;
    return ++revision;
}

inline void appendFace(Polyhedron& polyhedron, GLFace shape,
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

} // namespace detail
using detail::finiteVector;
using detail::finiteMatrix;
using detail::validColor;
using detail::positiveLengths;
using detail::nextGeometryRevision;
using detail::appendFace;

inline Scene::Scene() {
    geometryRevision_ = nextGeometryRevision();
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
    pyramidFaceId_.reserve(MAX_ENTITIES * 5);
    lines_.reserve(MAX_LINES);
}

inline std::optional<EntityId> Scene::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (!DiagramSystem::validPolyhedron(polyhedron)
        || !DiagramSystem::validTransform(transform)) {
        return std::nullopt;
    }
    glm::vec3 minimum = polyhedron.faces[0].localVertices[0];
    glm::vec3 maximum = minimum;
    for (const PlaneFigure& face : polyhedron.faces) {
        for (const glm::vec3& vertex : face.localVertices) {
            minimum = glm::min(minimum, vertex);
            maximum = glm::max(maximum, vertex);
        }
    }
    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform, maximum - minimum });
    polyId_.push_back(id);
    std::vector<FaceId>* faceIds = nullptr;
    if (polyhedron.shape == GLShape::Cube) faceIds = &cubeFaceId_;
    if (polyhedron.shape == GLShape::Tetra) faceIds = &tetraFaceId_;
    if (polyhedron.shape == GLShape::Pyramid) faceIds = &pyramidFaceId_;
    if (faceIds) for (std::size_t i = 0; i < polyhedron.faces.size(); ++i)
        faceIds->push_back({id, i});
    geometryRevision_ = nextGeometryRevision();
    return id;
}

inline std::optional<EntityId> Scene::addCuboid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths),
                       transform);
}

inline std::optional<EntityId> Scene::addRightTetrahedron(
    const glm::vec3& edgeLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths),
                       transform);
}

inline std::optional<EntityId> Scene::addPyramid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths),
                       transform);
}

inline bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;
    return true;
}

inline bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex,
                               const glm::mat4& matrix) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !finiteMatrix(matrix)) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    return true;
}

inline bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex,
                                const std::vector<GlColor>& colors) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    if (!colors.empty() && colors.size() != face.localVertices.size()) {
        return false;
    }
    for (GlColor color : colors) {
        if (!validColor(color)) {
            return false;
        }
    }
    face.vertexColors = colors;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

inline const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

inline bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return false;
    }
    components_[id].transform.basis = worldPosition;
    return true;
}

inline bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if (id >= components_.size() || !positiveLengths(sideLengths)) {
        return false;
    }
    Component_Poly& component = components_[id];
    Polyhedron replacement = DiagramSystem::makePolyhedron(
        component.polyhedron.shape, sideLengths);
    if (!DiagramSystem::validPolyhedron(replacement)) {
        return false;
    }
    for (std::size_t i = 0; i < replacement.faces.size(); ++i) {
        replacement.faces[i].color = component.polyhedron.faces[i].color;
        replacement.faces[i].independence = component.polyhedron.faces[i].independence;
        replacement.faces[i].transform = component.polyhedron.faces[i].transform;
        replacement.faces[i].visible = component.polyhedron.faces[i].visible;
        replacement.faces[i].localAnimationMatrix =
            component.polyhedron.faces[i].localAnimationMatrix;
        replacement.faces[i].vertexColors = component.polyhedron.faces[i].vertexColors;
    }
    component.polyhedron = std::move(replacement);
    component.sideLengths = sideLengths;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true;
}

inline bool Scene::setFaceTransform(
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

inline std::optional<LineId> Scene::addLine(
    const glm::vec3& start, const glm::vec3& end,
    GlColor color, std::optional<EntityId> owner
) {
    if (!finiteVector(start) || !finiteVector(end)
        || !validColor(color) || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    const LineId id = lines_.size();
    lines_.push_back({ start, end, color, owner });
    geometryRevision_ = nextGeometryRevision();
    return id;
}

inline std::optional<LineId> Scene::addVector(
    const glm::vec3& origin, const glm::vec3& vector,
    GlColor color, std::optional<EntityId> owner
) {
    return addLine(origin, origin + vector, color, owner);
}

inline std::optional<std::array<LineId, 3>> Scene::addBasis(
    const glm::vec3& origin, const glm::mat3& basisVectors,
    const glm::vec3& axisLengths, std::optional<EntityId> owner
) {
    if (!finiteVector(origin)
        || !positiveLengths(axisLengths)
        || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    for (int i = 0; i < 3; ++i) {
        if (!finiteVector(basisVectors[i])
            || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f
            || !finiteVector(origin + basisVectors[i] * axisLengths[i])) {
            return std::nullopt;
        }
    }
    // 사전 검증 후 세 선을 함께 등록한다. 실패 입력은 장면을 변경하지 않는다.
    const std::array<GlColor, 3> colors{
        GlColor{ 1.0f, 0.2f, 0.2f },
        GlColor{ 0.2f, 1.0f, 0.2f },
        GlColor{ 0.2f, 0.4f, 1.0f }
    };
    std::array<LineId, 3> ids{};
    for (int i = 0; i < 3; ++i) {
        ids[i] = lines_.size();
        lines_.push_back({
            origin, origin + basisVectors[i] * axisLengths[i], colors[i], owner });
    }
    geometryRevision_ = nextGeometryRevision();
    return ids;
}

inline bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin)
        || !finiteVector(origin + vector)) {
        return false;
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    cubeFaceId_.clear();
    tetraFaceId_.clear();
    pyramidFaceId_.clear();
    lines_.clear();
    geometryRevision_ = nextGeometryRevision();
}

inline bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale)
        && std::isfinite(transform.orientation.w)
        && std::isfinite(transform.orientation.x)
        && std::isfinite(transform.orientation.y)
        && std::isfinite(transform.orientation.z)
        && std::isfinite(glm::dot(transform.orientation, transform.orientation))
        && glm::dot(transform.orientation, transform.orientation) > 0.000001f;
}

inline bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    if (polyhedron.faces.empty()) return false;
    for (const auto& face : polyhedron.faces)
        if (!detail::validMesh(face) || !validTransform(face.transform)) return false;
    return true;
}

inline Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
    Polyhedron polyhedron;
    if (!positiveLengths(sideLengths)) {
        return polyhedron;
    }
    polyhedron.shape = shape;
    if (shape == GLShape::Cube) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        polyhedron.faces.reserve(6);
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, z }, { x, -y, z }, { x, y, z }, { -x, y, z } },
            { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, -z }, { -x, -y, -z }, { -x, y, -z }, { x, y, -z } },
            { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { -x, -y, z }, { -x, y, z }, { -x, y, -z } },
            { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, z }, { x, -y, -z }, { x, y, -z }, { x, y, z } },
            { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, y, z }, { x, y, z }, { x, y, -z }, { -x, y, -z } },
            { .3f, .8f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { x, -y, -z }, { x, -y, z }, { -x, -y, z } },
            { .8f, .3f, .9f });
    } else if (shape == GLShape::Pyramid) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        const glm::vec3 frontLeft{ -x, -y, z };
        const glm::vec3 frontRight{ x, -y, z };
        const glm::vec3 backRight{ x, -y, -z };
        const glm::vec3 backLeft{ -x, -y, -z };
        const glm::vec3 apex{ 0.0f, y, 0.0f };
        polyhedron.faces.reserve(5);
        // 앞, 오른쪽, 뒤, 왼쪽, 바닥. 삼각형과 사각형은 바깥쪽 winding이다.
        appendFace(polyhedron, GLFace::Triangle,
            { frontLeft, frontRight, apex }, { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { frontRight, backRight, apex }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { backRight, backLeft, apex }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { backLeft, frontLeft, apex }, { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { backLeft, backRight, frontRight, frontLeft }, { .8f, .3f, .9f });
    } else if (shape == GLShape::Tetra) {
        // 원점에서 직교하는 세 모서리를 만들고, 무게중심을 로컬 원점으로 옮긴다.
        const glm::vec3 center = sideLengths * 0.25f;
        const glm::vec3 origin = -center;
        const glm::vec3 x = glm::vec3(sideLengths.x, 0.0f, 0.0f) - center;
        const glm::vec3 y = glm::vec3(0.0f, sideLengths.y, 0.0f) - center;
        const glm::vec3 z = glm::vec3(0.0f, 0.0f, sideLengths.z) - center;
        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle,
            { origin, y, x }, { .9f, .4f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, x, z }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, z, y }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { x, y, z }, { .9f, .8f, .3f });
    }
    return polyhedron;
}

inline glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    return detail::transformMatrix(transform);
}

inline glm::mat4 DiagramSystem::faceModelMatrix(const Component_Poly& component,
                                         const PlaneFigure& face) {
    return detail::faceMatrix(component, face);
}

inline bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);
    return finiteVector(camera.position) && finiteVector(camera.target)
        && finiteVector(camera.up) && finiteVector(direction) && finiteVector(right)
        && glm::dot(direction, direction) > 0.000001f
        && glm::dot(right, right) > 0.000001f
        && std::isfinite(camera.fieldOfViewDegrees)
        && camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f
        && std::isfinite(camera.nearPlane) && std::isfinite(camera.farPlane)
        && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane
        && std::isfinite(camera.orthographicHalfExtent)
        && camera.orthographicHalfExtent > 0.0f;
}

inline glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

inline glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                          camera.nearPlane, camera.farPlane);
    }
    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect,
                            camera.nearPlane, camera.farPlane);
}


inline void Scene::rebuildFaceIds() {
    cubeFaceId_.clear(); tetraFaceId_.clear(); pyramidFaceId_.clear();
    for (EntityId id : polyId_) {
        const auto& model = components_[id].polyhedron;
        auto* ids = model.shape == GLShape::Cube ? &cubeFaceId_
            : model.shape == GLShape::Tetra ? &tetraFaceId_
            : model.shape == GLShape::Pyramid ? &pyramidFaceId_ : nullptr;
        if(ids) for(std::size_t f=0; f<model.faces.size(); ++f) ids->push_back({id,f});
    }
}
inline std::optional<EntityId> Scene::addModel(const Model& model, const Transform3D& transform,
                                             PivotMode pivot, const glm::vec3& customPivot) {
    if(!DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot)) return {};
    auto initial=transform;
    if(pivot==PivotMode::BottomCenter) initial.localBasis=detail::bottomCenter(model);
    else if(pivot==PivotMode::Custom) initial.localBasis=customPivot;
    auto id=push_entity(model,initial);
    if(id) components_[*id].pivotMode=pivot;
    return id;
}
inline bool Scene::setModel(EntityId id, const Model& model) {
    if(id>=components_.size() || !DiagramSystem::validPolyhedron(model)) return false;
    const auto box=detail::bounds(model);
    if(!detail::finiteVector(box.second-box.first)) return false;
    auto replacement=components_[id];
    replacement.polyhedron=model;replacement.sideLengths=box.second-box.first;
    if(replacement.pivotMode==PivotMode::BottomCenter)
        replacement.transform.localBasis=detail::bottomCenter(model);
    components_[id]=std::move(replacement);
    rebuildFaceIds();geometryRevision_=nextGeometryRevision();return true;
}
inline const Mesh* Scene::getMesh(EntityId id, std::size_t meshIndex) const {
    const auto* c=getComponent(id);
    return c && meshIndex<c->polyhedron.faces.size()?&c->polyhedron.faces[meshIndex]:nullptr;
}
inline bool Scene::setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh) {
    if(!getMesh(id,meshIndex)) return false;
    Model replacement=components_[id].polyhedron;
    replacement.faces[meshIndex]=mesh;
    return setModel(id,replacement);
}
inline bool Scene::setMeshVertices(EntityId id,std::size_t meshIndex,
                                   const std::vector<Vertex>& vertices,
                                   const std::vector<std::uint32_t>& triangles) {
    const auto* original=getMesh(id,meshIndex);if(!original)return false;
    auto replacement=*original;
    const auto geometry=Mesh::fromVertices(vertices,triangles);
    replacement.localVertices=geometry.localVertices;replacement.vertexNormals=geometry.vertexNormals;
    replacement.texcoords=geometry.texcoords;replacement.vertexColors=geometry.vertexColors;
    replacement.triangleIndices=triangles;replacement.shape=GLFace::Mesh;
    replacement.outlineIndices.clear();
    if(replacement.outline==OutlineMode::Explicit) replacement.outline=OutlineMode::None;
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setVertexPosition(EntityId id,std::size_t meshIndex,std::size_t vertexIndex,
                                     const glm::vec3& position) {
    const auto* mesh=getMesh(id,meshIndex);
    if(!mesh||vertexIndex>=mesh->localVertices.size()||!detail::finiteVector(position))return false;
    auto replacement=*mesh;replacement.localVertices[vertexIndex]=position;
    recalculateNormals(replacement);
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setTriangleIndices(EntityId id,std::size_t meshIndex,
                                      const std::vector<std::uint32_t>& indices) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.shape=GLFace::Mesh;replacement.triangleIndices=indices;
    if(!detail::validMesh(replacement))return false;
    recalculateNormals(replacement);return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setTexcoords(EntityId id,std::size_t meshIndex,const std::vector<glm::vec2>& uvs) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.texcoords=uvs;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setVertexNormals(EntityId id,std::size_t meshIndex,const std::vector<glm::vec3>& normals) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.vertexNormals=normals;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setMaterial(EntityId id,std::size_t meshIndex,const Material& material) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.material=material;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setOutline(EntityId id,std::size_t meshIndex,OutlineMode mode,
                              const std::vector<std::uint32_t>& explicitEdges) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.outline=mode;replacement.outlineIndices=explicitEdges;
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setLocalBasis(EntityId id,PivotMode mode,const glm::vec3& customPivot) {
    if(id>=components_.size()||!detail::finiteVector(customPivot))return false;
    auto& c=components_[id];
    if(mode==PivotMode::BottomCenter)c.transform.localBasis=detail::bottomCenter(c.polyhedron);
    else if(mode==PivotMode::Custom)c.transform.localBasis=customPivot;
    c.pivotMode=mode;return true;
}
inline bool Scene::setVisible(EntityId id,bool visible) {
    if(id>=components_.size())return false;
    components_[id].visible=visible;return true;
}
inline bool Scene::splitFace(EntityId id,std::size_t faceIndex) {
    if(!getMesh(id,faceIndex))return false;
    // The same face remains under its parent; vertices and indices are already face-local storage.
    auto& face=components_[id].polyhedron.faces[faceIndex];
    if(!face.independence) {face.transform=Transform3D{};face.independence=true;}
    return true;
}
inline bool Scene::setFaceSRT(EntityId id,std::size_t faceIndex,const Transform3D& transform) {
    return setFaceTransform(id,faceIndex,transform,true);
}
} // namespace graphics
