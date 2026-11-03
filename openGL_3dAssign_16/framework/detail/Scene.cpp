#include "../Framework.h"
#include "Internal.h"
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace graphics {
namespace detail {

bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f && value.y > 0.0f && value.z > 0.0f;
}

std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;

    return ++revision;
}

void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color) {
    PlaneFigure face;

    face.shape = shape;
    face.localVertices = vertices;
    face.color = color;
    face.normal = glm::normalize(
        glm::cross(face.localVertices[1] - face.localVertices[0], face.localVertices[2] - face.localVertices[0]));
    polyhedron.faces.push_back(face);
}

} // namespace detail

using detail::finiteVector;
using detail::finiteMatrix;
using detail::validColor;
using detail::positiveLengths;
using detail::nextGeometryRevision;
using detail::appendFace;

Scene::Scene() {
    geometryRevision_ = nextGeometryRevision();
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
    pyramidFaceId_.reserve(MAX_ENTITIES * 5);
    lines_.reserve(MAX_LINES);
}

std::optional<EntityId> Scene::addGroup(const Transform3D& transform) {
    if (!DiagramSystem::validTransform(transform))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Scene::addGroup",
                                "Rejected condition: !DiagramSystem::validTransform(transform)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const auto id = components_.size();
    Component_Poly group;

    group.polyhedron.shape = GLShape::None;
    group.transform = transform;
    group.sideLengths = glm::vec3(0);
    group.transformOnly = true;
    components_.push_back(group);
    polyId_.push_back(id);
    geometryRevision_ = nextGeometryRevision();

    return id;
}

bool Scene::setTint(EntityId id, GlColor color) {
    if (id >= components_.size() || !validColor(color))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTint",
            "Rejected condition: id>=components_.size() || !validColor(color)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    components_[id].tint = color;

    return true;
}

std::optional<EntityId> Scene::addSphere(float radius, const glm::vec3& position, unsigned slices, unsigned stacks) {
    Transform3D transform;

    transform.basis = position;

    return push_entity(DiagramSystem::makeSphere(radius, slices, stacks), transform);
}

std::optional<EntityId> Scene::addCone(float radius, float height, const glm::vec3& position, unsigned slices) {
    Transform3D transform;

    transform.basis = position;

    return push_entity(DiagramSystem::makeCone(radius, height, slices), transform);
}

std::optional<EntityId> Scene::push_entity(const Polyhedron& polyhedron, const Transform3D& transform) {
    if (!DiagramSystem::validPolyhedron(polyhedron) || !DiagramSystem::validTransform(transform)) {
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::push_entity",
                "Rejected condition: !DiagramSystem::validPolyhedron(polyhedron) || !DiagramSystem::validTransform(transform)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);
    }

    glm::vec3 minimum = polyhedron.faces[0].localVertices[0];
    glm::vec3 maximum = minimum;

    for (const PlaneFigure& face : polyhedron.faces) {
        for (const glm::vec3& vertex : face.localVertices) {
            minimum = glm::min(minimum, vertex);
            maximum = glm::max(maximum, vertex);
        }
    }

    if (!finiteVector(maximum - minimum))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Scene::push_entity",
                                "Rejected condition: !finiteVector(maximum - minimum)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const EntityId id = components_.size();

    components_.push_back({polyhedron, transform, maximum - minimum});
    polyId_.push_back(id);

    std::vector<FaceId>* faceIds = nullptr;

    if (polyhedron.shape == GLShape::Cube)
        faceIds = &cubeFaceId_;

    if (polyhedron.shape == GLShape::Tetra)
        faceIds = &tetraFaceId_;

    if (polyhedron.shape == GLShape::Pyramid)
        faceIds = &pyramidFaceId_;

    if (faceIds)
        for (std::size_t i = 0; i < polyhedron.faces.size(); ++i)
            faceIds->push_back({id, i});
    geometryRevision_ = nextGeometryRevision();

    return id;
}

std::optional<EntityId> Scene::addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition) {
    Transform3D transform;

    transform.basis = worldPosition;

    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths), transform);
}

std::optional<EntityId> Scene::addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition) {
    Transform3D transform;

    transform.basis = worldPosition;

    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths), transform);
}

std::optional<EntityId> Scene::addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition) {
    Transform3D transform;

    transform.basis = worldPosition;

    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths), transform);
}

bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVisible",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;

    return true;
}

bool Scene::setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() ||
        (mode != CullMode::Inherit && mode != CullMode::None && mode != CullMode::Back && mode != CullMode::Front))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Raster, "Scene::setFaceCullMode",
                                   "Invalid entity, face index or culling mode.",
                                   "Check the model's face list and CullMode enum.", id);
    components_[id].polyhedron.faces[faceIndex].cull = mode;

    return true;
}

bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix, FaceAnimationSpace space) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix) ||
        (space != FaceAnimationSpace::Local && space != FaceAnimationSpace::Model)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceLocalMatrix",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    components_[id].polyhedron.faces[faceIndex].animationSpace = space;

    return true;
}

bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVertexColors",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];

    if (!colors.empty() && colors.size() != face.localVertices.size()) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVertexColors",
            "Rejected condition: !colors.empty() && colors.size() != face.localVertices.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    for (GlColor color : colors) {
        if (!validColor(color)) {
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVertexColors",
                "Rejected condition: !validColor(color)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
        }
    }
    face.vertexColors = colors;
    geometryRevision_ = nextGeometryRevision();

    return true;
}

const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    // For a parented entity this position is in parent coordinates.
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setPosition",
            "Rejected condition: id >= components_.size() || !finiteVector(worldPosition)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    auto candidate = components_[id].transform;

    candidate.basis = worldPosition;

    return setTransform(id, candidate);
}

bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if (id >= components_.size() || !positiveLengths(sideLengths))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths",
            "Rejected condition: id>=components_.size()||!positiveLengths(sideLengths)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto& c = components_[id];

    if (c.transformOnly)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths", "Rejected condition: c.transformOnly",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = c.polyhedron;
    const auto box = detail::bounds(replacement);
    const auto oldSize = box.second - box.first;

    if (!positiveLengths(oldSize))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths",
            "Rejected condition: !positiveLengths(oldSize)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto ratio = sideLengths / oldSize;

    if (!finiteVector(ratio))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths",
            "Rejected condition: !finiteVector(ratio)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    // Preserve topology/materials. Resize mesh coordinates around the model pivot.
    for (auto& face : replacement.faces) {
        for (auto& vertex : face.localVertices)
            vertex = c.transform.localBasis + (vertex - c.transform.localBasis) * ratio;
        face.transform.localBasis =
            c.transform.localBasis + (face.transform.localBasis - c.transform.localBasis) * ratio;
        recalculateNormals(face);
    }

    return setModel(id, replacement);
}

bool Scene::setParent(EntityId id, std::optional<EntityId> parent) {
    if (id >= components_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setParent",
            "Rejected condition: id >= components_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto ancestor = parent;
    std::size_t remaining = components_.size();

    while (ancestor) {
        if (*ancestor >= components_.size() || *ancestor == id || remaining-- == 0)
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::setParent",
                "Rejected condition: *ancestor >= components_.size() || *ancestor == id || remaining-- == 0",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
        ancestor = components_[*ancestor].parent;
    }
    components_[id].parent = parent;

    return true;
}

std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size())
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::worldMatrix",
                    "Rejected condition: id >= components_.size()",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;

    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size())
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Scene, "Scene::worldMatrix",
                        "Rejected condition: *current >= components_.size() || chain.size() >= components_.size()",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                    std::nullopt);
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());

    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;

    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);

        if (binding) {
            layer = *binding;
            anchor = i - 1;
            break;
        }
    }

    const auto motion = motions_.layerMatrices(layer);

    if (!motion)
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::worldMatrix", "Rejected condition: !motion",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    glm::mat4 prefix(1), suffix(1);

    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;

        matrix *= detail::transformMatrix(components_[chain[i]].transform);
    }

    const auto result = motion->parent * prefix * motion->local * suffix;

    if (!detail::finiteMatrix(result)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Scene::worldMatrix",
                            "World transform overflow after parent and motion composition.",
                            "Reduce coordinate/scale magnitudes and inspect the parent/layer chain.", id);

        return {};
    }

    return result;
}

std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size())
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::faceWorldMatrix",
                "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
            std::nullopt);

    const auto& face = components_[id].polyhedron.faces[faceIndex];
    const auto world = face.transformMode == FaceTransformMode::Hierarchy
                           ? worldMatrix(id) : std::optional<glm::mat4>(glm::mat4(1));

    if (!world)
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::faceWorldMatrix", "Rejected condition: !world",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    const auto result = *world * face.transformFrame * detail::faceLocalMatrix(face);

    if (!detail::finiteMatrix(result)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Scene::faceWorldMatrix",
                            "Face world transform overflow.", "Inspect local face animation and entity transforms.",
                            id);

        return {};
    }

    return result;
}

std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size())
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::lineWorldMatrix",
                    "Rejected condition: id >= lines_.size()",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    if (lines_[id].owner)
        return worldMatrix(*lines_[id].owner);

    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());

    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}

bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::bindEntity",
            "Rejected condition: id >= components_.size() || !motions_.getLayer(layer)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    entityLayers_[id] = layer;

    return true;
}

bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::unbindEntity",
            "Rejected condition: id >= components_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    entityLayers_.erase(id);

    return true;
}

std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size())
        return {};

    const auto found = entityLayers_.find(id);

    if (found == entityLayers_.end() || !motions_.getLayer(found->second))
        return {};

    return found->second;
}

bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTransform",
            "Rejected condition: id >= components_.size() || !DiagramSystem::validTransform(transform)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    components_[id].transform = transform;

    return true;
}

bool Scene::setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld) {
    if (!getMesh(id, faceIndex) || (mode != FaceTransformMode::Hierarchy && mode != FaceTransformMode::Independent))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
                                   "Invalid entity, face index or transform mode.");
    auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 frame(1);
    if (preserveWorld) {
        if (mode == face.transformMode) return true;
        const auto world = worldMatrix(id);
        if (!world) return false;
        const auto oldParent = face.transformMode == FaceTransformMode::Hierarchy ? *world : glm::mat4(1);
        frame = oldParent * face.transformFrame;
        if (mode == FaceTransformMode::Hierarchy) {
            const float determinant = glm::determinant(*world);
            if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
                                           "Cannot preserve world pose under a singular parent transform.");
            frame = glm::inverse(*world) * frame;
        }
    }
    if (!TransformSystem::validAffine(frame)) return false;
    face.transformMode = mode;
    face.transformFrame = frame;
    return true;
}

bool Scene::setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() ||
        !DiagramSystem::validTransform(transform)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceTransform",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !DiagramSystem::validTransform(transform)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];

    face.transform = transform;
    face.independence = independence;

    return true;
}

std::optional<LineId> Scene::addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                     std::optional<EntityId> owner) {
    if (!finiteVector(start) || !finiteVector(end) || !validColor(color) ||
        (owner && getComponent(*owner) == nullptr)) {
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::addLine",
                "Rejected condition: !finiteVector(start) || !finiteVector(end) || !validColor(color) || (owner && getComponent(*owner) == nullptr)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);
    }

    const LineId id = lines_.size();

    lines_.push_back({start, end, color, owner});
    geometryRevision_ = nextGeometryRevision();

    return id;
}

std::optional<LineId> Scene::addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                       std::optional<EntityId> owner) {
    return addLine(origin, origin + vector, color, owner);
}

std::optional<std::array<LineId, 3>> Scene::addBasis(const glm::vec3& origin, const glm::mat3& basisVectors,
                                                     const glm::vec3& axisLengths, std::optional<EntityId> owner) {
    if (!finiteVector(origin) || !positiveLengths(axisLengths) || (owner && getComponent(*owner) == nullptr)) {
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::addBasis",
                "Rejected condition: !finiteVector(origin) || !positiveLengths(axisLengths) || (owner && getComponent(*owner) == nullptr)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);
    }

    for (int i = 0; i < 3; ++i) {
        if (!finiteVector(basisVectors[i]) || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f ||
            !finiteVector(origin + basisVectors[i] * axisLengths[i])) {
            return (
                Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::addBasis",
                    "Rejected condition: !finiteVector(basisVectors[i]) || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f || !finiteVector(origin + basisVectors[i] * axisLengths[i])",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                std::nullopt);
        }
    }
    // 사전 검증 후 세 선을 함께 등록한다. 실패 입력은 장면을 변경하지 않는다.
    const std::array<GlColor, 3> colors{GlColor{1.0f, 0.2f, 0.2f}, GlColor{0.2f, 1.0f, 0.2f},
                                        GlColor{0.2f, 0.4f, 1.0f}};
    std::array<LineId, 3> ids{};

    for (int i = 0; i < 3; ++i) {
        ids[i] = lines_.size();
        lines_.push_back({origin, origin + basisVectors[i] * axisLengths[i], colors[i], owner});
    }
    geometryRevision_ = nextGeometryRevision();

    return ids;
}

bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin) || !finiteVector(origin + vector)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVector",
            "Rejected condition: id >= lines_.size() || !finiteVector(origin) || !finiteVector(origin + vector)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();

    return true;
}

void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();

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

bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis) && finiteVector(transform.scale) &&
           finiteVector(transform.rotation[0]) && finiteVector(transform.rotation[1]) &&
           finiteVector(transform.rotation[2]) && std::abs(glm::determinant(transform.rotation) - 1.0f) < 0.01f &&
           glm::length(glm::transpose(transform.rotation) * transform.rotation[0] - glm::vec3(1, 0, 0)) < 0.01f &&
           glm::length(glm::transpose(transform.rotation) * transform.rotation[1] - glm::vec3(0, 1, 0)) < 0.01f &&
           glm::length(glm::transpose(transform.rotation) * transform.rotation[2] - glm::vec3(0, 0, 1)) < 0.01f &&
           finiteMatrix(detail::transformMatrix(transform));
}

bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    if (polyhedron.faces.empty())
        return false;

    for (const auto& face : polyhedron.faces)
        if (!detail::validMesh(face) || !validTransform(face.transform))
            return false;

    return true;
}

Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
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
        appendFace(polyhedron, GLFace::Rect, {{-x, -y, z}, {x, -y, z}, {x, y, z}, {-x, y, z}}, {.9f, .3f, .3f});
        appendFace(polyhedron, GLFace::Rect, {{x, -y, -z}, {-x, -y, -z}, {-x, y, -z}, {x, y, -z}}, {.3f, .8f, .4f});
        appendFace(polyhedron, GLFace::Rect, {{-x, -y, -z}, {-x, -y, z}, {-x, y, z}, {-x, y, -z}}, {.3f, .5f, .9f});
        appendFace(polyhedron, GLFace::Rect, {{x, -y, z}, {x, -y, -z}, {x, y, -z}, {x, y, z}}, {.9f, .8f, .3f});
        appendFace(polyhedron, GLFace::Rect, {{-x, y, z}, {x, y, z}, {x, y, -z}, {-x, y, -z}}, {.3f, .8f, .9f});
        appendFace(polyhedron, GLFace::Rect, {{-x, -y, -z}, {x, -y, -z}, {x, -y, z}, {-x, -y, z}}, {.8f, .3f, .9f});
    } else if (shape == GLShape::Pyramid) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        const glm::vec3 frontLeft{-x, -y, z};
        const glm::vec3 frontRight{x, -y, z};
        const glm::vec3 backRight{x, -y, -z};
        const glm::vec3 backLeft{-x, -y, -z};
        const glm::vec3 apex{0.0f, y, 0.0f};

        polyhedron.faces.reserve(5);
        // 앞, 오른쪽, 뒤, 왼쪽, 바닥. 삼각형과 사각형은 바깥쪽 winding이다.
        appendFace(polyhedron, GLFace::Triangle, {frontLeft, frontRight, apex}, {.9f, .3f, .3f});
        appendFace(polyhedron, GLFace::Triangle, {frontRight, backRight, apex}, {.3f, .5f, .9f});
        appendFace(polyhedron, GLFace::Triangle, {backRight, backLeft, apex}, {.3f, .8f, .4f});
        appendFace(polyhedron, GLFace::Triangle, {backLeft, frontLeft, apex}, {.9f, .8f, .3f});
        appendFace(polyhedron, GLFace::Rect, {backLeft, backRight, frontRight, frontLeft}, {.8f, .3f, .9f});
    } else if (shape == GLShape::Tetra) {
        // 원점에서 직교하는 세 모서리를 만들고, 무게중심을 로컬 원점으로 옮긴다.
        const glm::vec3 center = sideLengths * 0.25f;
        const glm::vec3 origin = -center;
        const glm::vec3 x = glm::vec3(sideLengths.x, 0.0f, 0.0f) - center;
        const glm::vec3 y = glm::vec3(0.0f, sideLengths.y, 0.0f) - center;
        const glm::vec3 z = glm::vec3(0.0f, 0.0f, sideLengths.z) - center;

        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle, {origin, y, x}, {.9f, .4f, .3f});
        appendFace(polyhedron, GLFace::Triangle, {origin, x, z}, {.3f, .8f, .4f});
        appendFace(polyhedron, GLFace::Triangle, {origin, z, y}, {.3f, .5f, .9f});
        appendFace(polyhedron, GLFace::Triangle, {x, y, z}, {.9f, .8f, .3f});
    }

    return polyhedron;
}

Model DiagramSystem::makeSphere(float radius, unsigned slices, unsigned stacks) {
    Model model;

    if (!std::isfinite(radius) || radius <= 0 || slices < 3 || slices > 512 || stacks < 2 || stacks > 256)
        return model;

    Mesh mesh;

    mesh.localVertices.push_back({0, radius, 0});

    constexpr double pi = 3.14159265358979323846;

    for (unsigned row = 1; row < stacks; ++row) {
        const double latitude = pi * row / stacks;

        for (unsigned col = 0; col < slices; ++col) {
            const double longitude = 2 * pi * col / slices;

            mesh.localVertices.push_back(radius * glm::vec3(std::sin(latitude) * std::cos(longitude),
                                                            std::cos(latitude),
                                                            std::sin(latitude) * std::sin(longitude)));
        }
    }

    const auto bottom = std::uint32_t(mesh.localVertices.size());

    mesh.localVertices.push_back({0, -radius, 0});

    for (unsigned col = 0; col < slices; ++col) {
        const auto next = (col + 1) % slices;

        mesh.triangleIndices.insert(mesh.triangleIndices.end(), {0, 1 + next, 1 + col});

        for (unsigned row = 0; row + 2 < stacks; ++row) {
            const auto a = 1 + row * slices + col, b = 1 + row * slices + next, c = a + slices, d = b + slices;

            mesh.triangleIndices.insert(mesh.triangleIndices.end(), {a, b, c, b, d, c});
        }

        const auto a = 1 + (stacks - 2) * slices + col, b = 1 + (stacks - 2) * slices + next;

        mesh.triangleIndices.insert(mesh.triangleIndices.end(), {a, b, bottom});
    }

    for (const auto& p : mesh.localVertices)
        mesh.vertexNormals.push_back(glm::normalize(p));
    model.faces.push_back(std::move(mesh));

    return validPolyhedron(model) ? model : Model{};
}

Model DiagramSystem::makeCone(float radius, float height, unsigned slices) {
    Model model;

    if (!std::isfinite(radius) || !std::isfinite(height) || radius <= 0 || height <= 0 || slices < 3 || slices > 512)
        return model;

    constexpr double pi = 3.14159265358979323846;
    // Separate sides/base keep the bottom rim sharp for normals and outlines.
    Mesh sides, base;

    sides.localVertices.push_back({0, height * .5f, 0});
    base.localVertices.push_back({0, -height * .5f, 0});

    for (unsigned i = 0; i < slices; ++i) {
        const double a = 2 * pi * i / slices;
        const glm::vec3 p{radius * float(std::cos(a)), -height * .5f, radius * float(std::sin(a))};

        sides.localVertices.push_back(p);
        base.localVertices.push_back(p);
    }

    for (unsigned i = 0; i < slices; ++i) {
        const auto a = 1 + i, b = 1 + (i + 1) % slices;

        sides.triangleIndices.insert(sides.triangleIndices.end(), {0, b, a});
        base.triangleIndices.insert(base.triangleIndices.end(), {0, a, b});
    }
    recalculateNormals(sides);
    recalculateNormals(base);
    model.faces = {std::move(sides), std::move(base)};

    return validPolyhedron(model) ? model : Model{};
}

glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    return detail::transformMatrix(transform);
}

glm::mat4 DiagramSystem::faceModelMatrix(const Component_Poly& component, const PlaneFigure& face) {
    return detail::faceMatrix(component, face);
}

bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);

    return finiteVector(camera.position) && finiteVector(camera.target) && finiteVector(camera.up) &&
           finiteVector(direction) && finiteVector(right) && std::isfinite(glm::dot(direction, direction)) &&
           std::isfinite(glm::dot(right, right)) && glm::dot(direction, direction) > 0.000001f &&
           glm::dot(right, right) > 0.000001f && std::isfinite(camera.fieldOfViewDegrees) &&
           camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f && std::isfinite(camera.nearPlane) &&
           std::isfinite(camera.farPlane) && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane &&
           std::isfinite(camera.orthographicHalfExtent) && camera.orthographicHalfExtent > 0.0f;
}

glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);

        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, camera.nearPlane, camera.farPlane);
    }

    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect, camera.nearPlane, camera.farPlane);
}

void Scene::rebuildFaceIds() {
    cubeFaceId_.clear();
    tetraFaceId_.clear();
    pyramidFaceId_.clear();

    for (EntityId id : polyId_) {
        const auto& model = components_[id].polyhedron;
        auto* ids = model.shape == GLShape::Cube      ? &cubeFaceId_
                    : model.shape == GLShape::Tetra   ? &tetraFaceId_
                    : model.shape == GLShape::Pyramid ? &pyramidFaceId_
                                                      : nullptr;

        if (ids)
            for (std::size_t f = 0; f < model.faces.size(); ++f)
                ids->push_back({id, f});
    }
}

std::optional<EntityId> Scene::addModel(const Model& model, const Transform3D& transform, PivotMode pivot,
                                        const glm::vec3& customPivot) {
    if (!DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot))
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::addModel",
                    "Rejected condition: !DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot)",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                std::nullopt);

    auto initial = transform;

    if (pivot == PivotMode::BottomCenter)
        initial.localBasis = detail::bottomCenter(model);
    else if (pivot == PivotMode::Custom)
        initial.localBasis = customPivot;

    auto id = push_entity(model, initial);

    if (id)
        components_[*id].pivotMode = pivot;

    return id;
}

bool Scene::setModel(EntityId id, const Model& model) {
    if (id >= components_.size() || !DiagramSystem::validPolyhedron(model))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setModel",
            "Rejected condition: id>=components_.size() || !DiagramSystem::validPolyhedron(model)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto box = detail::bounds(model);

    if (!detail::finiteVector(box.second - box.first))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setModel",
            "Rejected condition: !detail::finiteVector(box.second-box.first)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = components_[id];

    replacement.polyhedron = model;
    replacement.sideLengths = box.second - box.first;
    replacement.transformOnly = false;

    if (replacement.pivotMode == PivotMode::BottomCenter)
        replacement.transform.localBasis = detail::bottomCenter(model);
    components_[id] = std::move(replacement);
    rebuildFaceIds();
    geometryRevision_ = nextGeometryRevision();

    return true;
}

const Mesh* Scene::getMesh(EntityId id, std::size_t meshIndex) const {
    const auto* c = getComponent(id);

    return c && meshIndex < c->polyhedron.faces.size() ? &c->polyhedron.faces[meshIndex] : nullptr;
}

bool Scene::setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh) {
    if (!getMesh(id, meshIndex))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setMesh", "Rejected condition: !getMesh(id,meshIndex)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    Model replacement = components_[id].polyhedron;

    replacement.faces[meshIndex] = mesh;

    return setModel(id, replacement);
}

bool Scene::setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                            const std::vector<std::uint32_t>& triangles) {
    const auto* original = getMesh(id, meshIndex);

    if (!original)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setMeshVertices", "Rejected condition: !original",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *original;
    const auto geometry = Mesh::fromVertices(vertices, triangles);

    replacement.localVertices = geometry.localVertices;
    replacement.vertexNormals = geometry.vertexNormals;
    replacement.texcoords = geometry.texcoords;
    replacement.vertexColors = geometry.vertexColors;
    replacement.triangleIndices = triangles;
    replacement.shape = GLFace::Mesh;
    replacement.outlineIndices.clear();

    if (replacement.outline == OutlineMode::Explicit)
        replacement.outline = OutlineMode::None;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh || vertexIndex >= mesh->localVertices.size() || !detail::finiteVector(position))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVertexPosition",
            "Rejected condition: !mesh||vertexIndex>=mesh->localVertices.size()||!detail::finiteVector(position)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.localVertices[vertexIndex] = position;
    recalculateNormals(replacement);

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTriangleIndices", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.shape = GLFace::Mesh;
    replacement.triangleIndices = indices;

    if (!detail::validMesh(replacement))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTriangleIndices",
            "Rejected condition: !detail::validMesh(replacement)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    recalculateNormals(replacement);

    return setMesh(id, meshIndex, replacement);
}

bool Scene::generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::generateTexcoords", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    return graphics::generateTexcoords(replacement, options) && setMesh(id, meshIndex, replacement);
}

bool Scene::transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                               float rotationDegrees) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::transformTexcoords", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    return graphics::transformTexcoords(replacement, scale, offset, rotationDegrees) &&
           setMesh(id, meshIndex, replacement);
}

bool Scene::setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTexcoords", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.texcoords = uvs;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVertexNormals", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.vertexNormals = normals;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setMaterial(EntityId id, std::size_t meshIndex, const Material& material) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setMaterial", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.material = material;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                       const std::vector<std::uint32_t>& explicitEdges) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setOutline", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.outline = mode;
    replacement.outlineIndices = explicitEdges;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot) {
    if (id >= components_.size() || !detail::finiteVector(customPivot))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setLocalBasis",
            "Rejected condition: id>=components_.size()||!detail::finiteVector(customPivot)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto& c = components_[id];

    if (c.transformOnly && mode == PivotMode::BottomCenter)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setLocalBasis",
            "Rejected condition: c.transformOnly && mode==PivotMode::BottomCenter",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    if (mode == PivotMode::BottomCenter)
        c.transform.localBasis = detail::bottomCenter(c.polyhedron);
    else if (mode == PivotMode::Custom)
        c.transform.localBasis = customPivot;
    c.pivotMode = mode;

    return true;
}

bool Scene::setVisible(EntityId id, bool visible) {
    if (id >= components_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVisible", "Rejected condition: id>=components_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    components_[id].visible = visible;

    return true;
}

bool Scene::splitFace(EntityId id, std::size_t faceIndex) {
    if (!getMesh(id, faceIndex))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::splitFace", "Rejected condition: !getMesh(id,faceIndex)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    // The same face remains under its parent; vertices and indices are already face-local storage.
    auto& face = components_[id].polyhedron.faces[faceIndex];

    if (!face.independence) {
        face.transform = Transform3D{};
        face.independence = true;
    }

    return true;
}

bool Scene::setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform) {
    return setFaceTransform(id, faceIndex, transform, true);
}
} // namespace graphics
