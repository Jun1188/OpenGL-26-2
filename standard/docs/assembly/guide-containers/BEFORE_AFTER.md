# 가이드·컨테이너 작업 전체 변경 전후

2026-11-03. 기준 커밋 `d35729b`의 실제 파일과 최종 저장 파일을 비교한다. 이전 조립 API 작업은 상위 BEFORE_AFTER.md에 별도로 보존한다. 빌드·테스트·앱 실행은 하지 않았다.

각 파일 전체를 before/after로 제시한다. 따라서 변경한 블록과 그 문맥을 생략하지 않는다. 새 파일은 before 없음이다. 무관한 사용자 API HTML/Framework.props 수정, 기존 부재 프로젝트는 이번 코드 변경 목록에서 제외한다. 예제 파일은 콘솔 조립 코드이므로 전체를 포함한다.

## 단계와 커밋

```text
9277399 Assembly step 11: compose modes 1 to 22 with isolated feature ports and hot reload
833a8d3 Assembly step 12: guide exact source sites edits and debugging procedures
89e7b46 Assembly step 13: map composed picks to native controls and copy guide assets
3d13196 Assembly step 14: reject recursive workspace reloads inside feature controllers
e4ecaa9 Assembly step 15: allow all 22 mode containers in one workspace
775debe Assembly step 16: distinguish editable configuration from callback debug sites
```

마지막 문서·그래프·패키징 단계의 커밋은 이 보고서를 포함한 최종 Git 이력에서 확인한다. source graph 기준은 architecture/SOURCE_MANIFEST.json, ZIP 영수증 기준은 배포 ZIP 옆 manifest다.

## 입력 → 상태/변환 → 출력

feature add/port와 포커스 입력 → 독립 native CPU Scene 또는 공유 program 시계 → native face frame과 작성자 외곽/면 SRT 합성 → 공통 Renderer/피킹/면 관절. guide 명령 → 메타데이터와 현재 디스크 소스 → 파일/줄/변경 예/디버깅 순서. Ctrl+F5 → 레시피 파싱/실패 복원 → 새 구성. C++ 함수 변경은 사용자 재빌드 대상이다.

## 사용자 확인

[사용자 빌드·회귀·핫로드 확인](../USER_CHECKS.md), [기능 컨테이너와 한계](../FEATURE_CONTAINERS.md), [구조 그래프](../architecture/GRAPH_REPORT.md)를 따른다. 독립 물리 월드는 자동 병합하지 않는다. private 원본 순간 상태/외부 IO는 완전한 역실행 대상이 아니다.

## framework/detail/Scene.cpp

유효한 affine 면 프레임만 저장하고 기존 faceWorldMatrix 합성 경로를 재사용한다.

### before

````cpp
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
````

### after

````cpp
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

bool Scene::setFaceFrame(EntityId id, std::size_t faceIndex, const glm::mat4& matrix) {
    if (!getMesh(id, faceIndex) || !TransformSystem::validAffine(matrix))
        return false;
    components_[id].polyhedron.faces[faceIndex].transformFrame = matrix;
    return true;
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
````

## framework/include/Graphics.h

원본 최종 면 자세와 작성자 면 자세를 별도로 합성하는 setFaceFrame API를 공개한다.

### before

````cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include "RenderConfig.h"
#include <map>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;


    void rebuildFaceIds();


public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();


    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});


    /// [API] 반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32,
                                      unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);


    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);


    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});


    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);


    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);


    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);


    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);


    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);


    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);


    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);


    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);


    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.
    bool setParent(EntityId id, std::optional<EntityId> parent);


    /// [API] 부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;


    /// [API] 엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;


    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    MotionSystem& getMotions() {
        return motions_;
    }


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const MotionSystem& getMotions() const {
        return motions_;
    }


    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);


    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);


    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;


    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
    bool updateMotions(double deltaSeconds) {
        return motions_.update(deltaSeconds);
    }


    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);


    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);


    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);


    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);


    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;


    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);


    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);


    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);


    /// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});


    /// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                            float rotationDegrees = 0);


    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);


    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);


    /// [API] 메시의 색상·텍스처 등 재질 데이터를 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);


    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});


    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);


    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);


    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);


    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);


    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
    std::optional<std::array<LineId, 3>> addBasis(const glm::vec3& origin,
                                                  const glm::mat3& basisVectors = glm::mat3(1.0f),
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);


    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.


    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
    const std::vector<EntityId>& getIds() const {
        return polyId_;
    }


    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;


    /// [API] 직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getCubeFaceIds() const {
        return cubeFaceId_;
    }


    /// [API] 사면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getTetraFaceIds() const {
        return tetraFaceId_;
    }


    /// [API] 피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getPyramidFaceIds() const {
        return pyramidFaceId_;
    }


    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const {
        return lines_;
    }


    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
    std::uint64_t getGeometryRevision() const {
        return geometryRevision_;
    }
};

class DiagramSystem {
public:
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));


    /// [API] 반지름·분할 수로 CPU 구 메시 모델을 생성한다.
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.
    static Model makeCone(float radius, float height, unsigned slices = 32);


    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);


    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);


    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);


    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{2.8f, 2.0f, 5.0f};
    glm::vec3 target{0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

class CameraSystem {
public:
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);


    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);


    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);


struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
    bool textures = false; // Explicit opt-in; no texture GPU work when false.
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
    CullMode cull = CullMode::None; // Append to preserve existing aggregate initializers.
};

struct Viewport {
    int x = 0, y = 0, width = 0, height = 0;
}; // Bottom-left framebuffer pixels.

struct RenderPass {
    Camera camera;
    Viewport viewport;
    RenderOptions options;
    bool clearColor = true, clearDepth = true;
};

class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };

    RenderConfig config_;
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1, tintLocation_ = -1;
    std::vector<DrawRange> drawRanges_;

    using TextureMap =
        std::map<std::shared_ptr<const TextureImage>, GLuint, std::owner_less<std::shared_ptr<const TextureImage>>>;

    TextureMap textures_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;


    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);


    bool upload(const Scene& scene, bool textures);


    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;


public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer(const Renderer&) = delete;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer& operator=(const Renderer&) = delete;


    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{},
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});


    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});


    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);


    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);


    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy(); // Call while the owning context is current, before it is destroyed.


    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.
    void abandon() {
        shaderProgramID_ = vao_ = vbo_ = ebo_ = 0;
        textures_.clear();
        drawRanges_.clear();
        uploadedScene_ = nullptr;
        uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    }
};
} // namespace graphics
````

### after

````cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include "RenderConfig.h"
#include <map>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;


    void rebuildFaceIds();


public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();


    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});


    /// [API] 반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32,
                                      unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);


    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);


    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});


    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);


    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);

    /// [API] 형상 재업로드 없이 면 앞쪽의 부모 프레임을 설정한다. 렌더·피킹·법선 변환에 함께 반영한다.
    bool setFaceFrame(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);


    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);


    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);


    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);


    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);


    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);


    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);


    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.
    bool setParent(EntityId id, std::optional<EntityId> parent);


    /// [API] 부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;


    /// [API] 엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;


    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    MotionSystem& getMotions() {
        return motions_;
    }


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const MotionSystem& getMotions() const {
        return motions_;
    }


    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);


    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);


    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;


    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
    bool updateMotions(double deltaSeconds) {
        return motions_.update(deltaSeconds);
    }


    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);


    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);


    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);


    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);


    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;


    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);


    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);


    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);


    /// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});


    /// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                            float rotationDegrees = 0);


    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);


    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);


    /// [API] 메시의 색상·텍스처 등 재질 데이터를 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);


    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});


    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);


    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);


    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);


    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);


    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
    std::optional<std::array<LineId, 3>> addBasis(const glm::vec3& origin,
                                                  const glm::mat3& basisVectors = glm::mat3(1.0f),
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);


    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.


    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
    const std::vector<EntityId>& getIds() const {
        return polyId_;
    }


    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;


    /// [API] 직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getCubeFaceIds() const {
        return cubeFaceId_;
    }


    /// [API] 사면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getTetraFaceIds() const {
        return tetraFaceId_;
    }


    /// [API] 피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getPyramidFaceIds() const {
        return pyramidFaceId_;
    }


    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const {
        return lines_;
    }


    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
    std::uint64_t getGeometryRevision() const {
        return geometryRevision_;
    }
};

class DiagramSystem {
public:
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));


    /// [API] 반지름·분할 수로 CPU 구 메시 모델을 생성한다.
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.
    static Model makeCone(float radius, float height, unsigned slices = 32);


    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);


    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);


    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);


    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{2.8f, 2.0f, 5.0f};
    glm::vec3 target{0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

class CameraSystem {
public:
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);


    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);


    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);


struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
    bool textures = false; // Explicit opt-in; no texture GPU work when false.
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
    CullMode cull = CullMode::None; // Append to preserve existing aggregate initializers.
};

struct Viewport {
    int x = 0, y = 0, width = 0, height = 0;
}; // Bottom-left framebuffer pixels.

struct RenderPass {
    Camera camera;
    Viewport viewport;
    RenderOptions options;
    bool clearColor = true, clearDepth = true;
};

class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };

    RenderConfig config_;
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1, tintLocation_ = -1;
    std::vector<DrawRange> drawRanges_;

    using TextureMap =
        std::map<std::shared_ptr<const TextureImage>, GLuint, std::owner_less<std::shared_ptr<const TextureImage>>>;

    TextureMap textures_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;


    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);


    bool upload(const Scene& scene, bool textures);


    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;


public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer(const Renderer&) = delete;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer& operator=(const Renderer&) = delete;


    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{},
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});


    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});


    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);


    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);


    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy(); // Call while the owning context is current, before it is destroyed.


    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.
    void abandon() {
        shaderProgramID_ = vao_ = vbo_ = ebo_ = 0;
        textures_.clear();
        drawRanges_.clear();
        uploadedScene_ = nullptr;
        uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    }
};
} // namespace graphics
````

## standard/Console.cpp

Ctrl+F5 진입과 컨테이너 포커스의 숫자 입력 소유권을 연결한다.

### before

````cpp
#include "Console.h"
#include <iostream>

bool Console::initScene() {
    printHelp();
    return activateMode(initialMode_.value_or(1));
}

bool Console::activateMode(int mode) {
    if (mode < 0 || mode > lastStandardMode)
        return false;
    if (mode >= 1 && mode <= 6 && demo_ == Demo::Motion && pathId_) {
        activeMode_ = mode;
        return choosePath(mode);
    }
    if (practiceMode_)
        practiceMode_->onFocus(false);
    practiceMode_.reset();
    activeMode_ = mode;
    animation_.clear();
    jointControls_.clear();
    selectedJoint_ = 0;
    selectedAxis_ = 0;
    roomControl_.reset();
    bodyControl_.reset();
    pathId_.reset();
    spinId_.reset();
    globalId_.reset();
    mirrorLayer_.reset();
    normalSpinLayer_.reset();
    insertedLayer_.reset();
    clock_.reset();
    app_.getCamera() = graphics::Camera{};
    app_.getRenderOptions() = graphics::RenderOptions{};
    if (mode == 0 || mode >= 10) {
        practiceMode_ = makePracticeMode(mode, app_);
        return practiceMode_ && practiceMode_->initScene();
    }
    const bool ok = mode <= 6 ? initMotion(mode) : mode == 7 ? initGrid() : mode == 8 ? initRoom() : initAnimation();
    if (!ok || !applyControls())
        return false;
    printSelectedJoint();
    return true;
}

/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key, int action, int mods) {
    if (!initialMode_ && action == GLFW_PRESS) {
        int next = -1;
        if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F11)
            next = 10 + key - GLFW_KEY_F1;
        else if (key == GLFW_KEY_LEFT_BRACKET)
            next = activeMode_ == 0 ? lastStandardMode : activeMode_ - 1;
        else if (key == GLFW_KEY_RIGHT_BRACKET)
            next = activeMode_ == lastStandardMode ? 0 : activeMode_ + 1;
        else if ((activeMode_ <= 9 || (mods & GLFW_MOD_ALT)) && key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
            next = key - GLFW_KEY_0;
        if (next >= 0) {
            if (!activateMode(next))
                app_.requestClose();
            return;
        }
    }
    if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
        printHelp();
        return;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app_.requestClose();
        return;
    }
    if (practiceMode_) {
        practiceMode_->onKey(key, action, mods);
        return;
    }
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
        pathId_.reset();
        if (!activateMode(activeMode_))
            app_.requestClose();
        return;
    }
    if (action == GLFW_REPEAT) {
        handleJointKey(key);
        return;
    }
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_V) {
        threeViews_ = !threeViews_;

        return;
    }

    if (key == GLFW_KEY_D) {
        trace_ = !trace_;
        graphics::Diagnostics::configure({true, trace_});

        return;
    }

    if (handleJointKey(key))
        return;

    if (key == GLFW_KEY_F && demo_ == Demo::Room) {
        floorOpen_ = !floorOpen_;
        if (!updateRoom(0))
            app_.requestClose();

        return;
    }

    if (demo_ != Demo::Motion) {
        if (key == GLFW_KEY_SPACE)
            clock_.playing = !clock_.playing;
        else if (key == GLFW_KEY_ESCAPE)
            app_.requestClose();
        else if (key == GLFW_KEY_H)
            printHelp();

        return;
    }

    auto& motions = app_.getScene().getMotions();
    bool ok = true;

    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;

        ok = pathId_ && spinId_ && globalId_ && motions.setMotionPlaying(*pathId_, playing) &&
             motions.setMotionPlaying(*spinId_, playing) && motions.setMotionPlaying(*globalId_, playing);

        if (ok) {
            paused_ = !paused_;
            std::cout << (paused_ ? "시간 정지\n" : "시간 재개\n");
        }
    } else if (key == GLFW_KEY_S) {
        ok = spinId_ && motions.setMotionEnabled(*spinId_, !spinEnabled_);

        if (ok) {
            spinEnabled_ = !spinEnabled_;
            std::cout << "자전 기여: " << spinEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_G) {
        ok = globalId_ && motions.setMotionEnabled(*globalId_, !globalEnabled_);

        if (ok) {
            globalEnabled_ = !globalEnabled_;
            std::cout << "전체 공전 기여: " << globalEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_M) {
        ok = changeReflection();
    } else if (key == GLFW_KEY_I) {
        ok = toggleInsertedLayer();
    } else if (key == GLFW_KEY_H) {
        printHelp();
    }

    if (!ok) {
        std::cerr << "샘플 명령 적용 실패. 장면을 종료합니다.\n";
        app_.requestClose();
    }
}

/// 마우스 버튼 입력을 처리한다.
void Console::onMouse(const graphics::MouseEvent& event) {
    if (practiceMode_) {
        practiceMode_->onMouse(event);
        return;
    }
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }

    std::optional<graphics::PickResult> hit;

    if (threeViews_) {
        const auto size = app_.getFramebufferSize();

        if (size[0] < 3 || size[1] < 2)
            return;

        const auto passes =
            graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

        for (const auto& pass : passes) {
            const auto& v = pass.viewport;
            const double top = size[1] - v.y - v.height;

            if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top &&
                event.framebufferY < top + v.height)
                hit = graphics::pick(app_.getScene(), pass.camera, event.framebufferX, event.framebufferY,
                                     {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
        }
    } else
        hit = app_.pickAt(event.windowX, event.windowY);

    selectPicked(hit);
}

/// 조작 방법을 출력한다.
void Console::printHelp() const {
    std::cout
        << "standard: initialMode=nullopt이면 모드 전환, 정수이면 해당 모드 고정\n"
        << "[ / ] 전체 0~22 모드 순환, Alt+0~9 데모 선택, F12 공통 도움말\n"
        << "F1~F11: 모드 10~20 = 실습 13~23 (각 실습의 기존 키 사용)\n"
        << "모드 21: WASD 카메라 충돌 / 모드 22: 내장 3D 메시 입력 처리\n"
        << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
        << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
        << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
        << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
        << "왼쪽 클릭: 사람·그네·상자 전체 선택 / 빈 공간: 선택 해제\n"
        << "0 기능 조립 콘솔: help / guide / save / load / 7 Grid / 8 방·물리\n"
        << "9 선형보간·진자·사람형·그네→사람 계층 애니메이션\n"
        << "1~9: Home 전체 복원 / Tab/Backspace 객체·관절 선택, X/Y/Z 축, 좌우 회전, 상하 이동, PageUp/Down 피벗, +/- 균일 배율, R 복원\n"
        << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

bool Console::applyControls() {
    if (!graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds))
        return false;
    return demo_ == Demo::Grid ? updateGrid() : demo_ == Demo::Room ? updateRoom(0) : true;
}

void Console::onUpdate(float dt) {
    if (practiceMode_) {
        practiceMode_->onUpdate(dt);
        return;
    }
    if (demo_ != Demo::Motion && !clock_.advance(dt)) {
        app_.requestClose();
        return;
    }
    bool ok = demo_ == Demo::Animation ? updateAnimation()
        : graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds);
    if (demo_ == Demo::Grid)
        ok = updateGrid() && ok;
    else if (demo_ == Demo::Room)
        ok = updateRoom(clock_.playing ? dt : 0) && ok;
    if (!ok)
        app_.requestClose();
}

/// 현재 장면을 지정한 뷰로 렌더링한다.
void Console::onCursor(double x, double y) {
    if (practiceMode_)
        practiceMode_->onCursor(x, y);
}

void Console::onFocus(bool focused) {
    if (practiceMode_)
        practiceMode_->onFocus(focused);
}

bool Console::onRender() {
    if (practiceMode_)
        return practiceMode_->onRender();
    if (!threeViews_)
        return app_.renderScene();

    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    return app_.renderViews(
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
````

### after

````cpp
#include "Console.h"
#include <iostream>

bool Console::initScene() {
    printHelp();
    return activateMode(initialMode_.value_or(1));
}

bool Console::activateMode(int mode) {
    if (mode < 0 || mode > lastStandardMode)
        return false;
    if (mode >= 1 && mode <= 6 && demo_ == Demo::Motion && pathId_) {
        activeMode_ = mode;
        return choosePath(mode);
    }
    if (practiceMode_)
        practiceMode_->onFocus(false);
    practiceMode_.reset();
    activeMode_ = mode;
    animation_.clear();
    jointControls_.clear();
    selectedJoint_ = 0;
    selectedAxis_ = 0;
    roomControl_.reset();
    bodyControl_.reset();
    pathId_.reset();
    spinId_.reset();
    globalId_.reset();
    mirrorLayer_.reset();
    normalSpinLayer_.reset();
    insertedLayer_.reset();
    clock_.reset();
    app_.getCamera() = graphics::Camera{};
    app_.getRenderOptions() = graphics::RenderOptions{};
    if (mode == 0 || mode >= 10) {
        practiceMode_ = makePracticeMode(mode, app_);
        return practiceMode_ && practiceMode_->initScene();
    }
    const bool ok = mode <= 6 ? initMotion(mode) : mode == 7 ? initGrid() : mode == 8 ? initRoom() : initAnimation();
    if (!ok || !applyControls())
        return false;
    printSelectedJoint();
    return true;
}

/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key, int action, int mods) {
    if (key == GLFW_KEY_F5 && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
        if (activeMode_ != 0 && !activateMode(0)) {
            std::cerr << "조립 작업공간 초기화 실패.\n";
            return;
        }
        if (practiceMode_) practiceMode_->onKey(key, action, mods);
        return;
    }
    if (!initialMode_ && action == GLFW_PRESS) {
        int next = -1;
        if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F11)
            next = 10 + key - GLFW_KEY_F1;
        else if (key == GLFW_KEY_LEFT_BRACKET)
            next = activeMode_ == 0 ? lastStandardMode : activeMode_ - 1;
        else if (key == GLFW_KEY_RIGHT_BRACKET)
            next = activeMode_ == lastStandardMode ? 0 : activeMode_ + 1;
        else if (((activeMode_ <= 9 && !(activeMode_ == 0 && practiceMode_ && practiceMode_->capturesNumericKeys())) || (mods & GLFW_MOD_ALT)) && key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
            next = key - GLFW_KEY_0;
        if (next >= 0) {
            if (!activateMode(next))
                app_.requestClose();
            return;
        }
    }
    if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
        printHelp();
        return;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app_.requestClose();
        return;
    }
    if (practiceMode_) {
        practiceMode_->onKey(key, action, mods);
        return;
    }
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
        pathId_.reset();
        if (!activateMode(activeMode_))
            app_.requestClose();
        return;
    }
    if (action == GLFW_REPEAT) {
        handleJointKey(key);
        return;
    }
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_V) {
        threeViews_ = !threeViews_;

        return;
    }

    if (key == GLFW_KEY_D) {
        trace_ = !trace_;
        graphics::Diagnostics::configure({true, trace_});

        return;
    }

    if (handleJointKey(key))
        return;

    if (key == GLFW_KEY_F && demo_ == Demo::Room) {
        floorOpen_ = !floorOpen_;
        if (!updateRoom(0))
            app_.requestClose();

        return;
    }

    if (demo_ != Demo::Motion) {
        if (key == GLFW_KEY_SPACE)
            clock_.playing = !clock_.playing;
        else if (key == GLFW_KEY_ESCAPE)
            app_.requestClose();
        else if (key == GLFW_KEY_H)
            printHelp();

        return;
    }

    auto& motions = app_.getScene().getMotions();
    bool ok = true;

    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;

        ok = pathId_ && spinId_ && globalId_ && motions.setMotionPlaying(*pathId_, playing) &&
             motions.setMotionPlaying(*spinId_, playing) && motions.setMotionPlaying(*globalId_, playing);

        if (ok) {
            paused_ = !paused_;
            std::cout << (paused_ ? "시간 정지\n" : "시간 재개\n");
        }
    } else if (key == GLFW_KEY_S) {
        ok = spinId_ && motions.setMotionEnabled(*spinId_, !spinEnabled_);

        if (ok) {
            spinEnabled_ = !spinEnabled_;
            std::cout << "자전 기여: " << spinEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_G) {
        ok = globalId_ && motions.setMotionEnabled(*globalId_, !globalEnabled_);

        if (ok) {
            globalEnabled_ = !globalEnabled_;
            std::cout << "전체 공전 기여: " << globalEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_M) {
        ok = changeReflection();
    } else if (key == GLFW_KEY_I) {
        ok = toggleInsertedLayer();
    } else if (key == GLFW_KEY_H) {
        printHelp();
    }

    if (!ok) {
        std::cerr << "샘플 명령 적용 실패. 장면을 종료합니다.\n";
        app_.requestClose();
    }
}

/// 마우스 버튼 입력을 처리한다.
void Console::onMouse(const graphics::MouseEvent& event) {
    if (practiceMode_) {
        practiceMode_->onMouse(event);
        return;
    }
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }

    std::optional<graphics::PickResult> hit;

    if (threeViews_) {
        const auto size = app_.getFramebufferSize();

        if (size[0] < 3 || size[1] < 2)
            return;

        const auto passes =
            graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

        for (const auto& pass : passes) {
            const auto& v = pass.viewport;
            const double top = size[1] - v.y - v.height;

            if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top &&
                event.framebufferY < top + v.height)
                hit = graphics::pick(app_.getScene(), pass.camera, event.framebufferX, event.framebufferY,
                                     {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
        }
    } else
        hit = app_.pickAt(event.windowX, event.windowY);

    selectPicked(hit);
}

/// 조작 방법을 출력한다.
void Console::printHelp() const {
    std::cout
        << "standard: initialMode=nullopt이면 모드 전환, 정수이면 해당 모드 고정\n"
        << "[ / ] 전체 0~22 모드 순환, Alt+0~9 데모 선택, F12 공통 도움말\n"
        << "Ctrl+F5: 조립 작업공간 진입/외부 레시피 재로드 (고정 모드에서도 사용)\n"
        << "F1~F11: 모드 10~20 = 실습 13~23 (각 실습의 기존 키 사용)\n"
        << "모드 21: WASD 카메라 충돌 / 모드 22: 내장 3D 메시 입력 처리\n"
        << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
        << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
        << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
        << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
        << "왼쪽 클릭: 사람·그네·상자 전체 선택 / 빈 공간: 선택 해제\n"
        << "0 기능 조립 콘솔: help / guide / save / load / 7 Grid / 8 방·물리\n"
        << "9 선형보간·진자·사람형·그네→사람 계층 애니메이션\n"
        << "1~9: Home 전체 복원 / Tab/Backspace 객체·관절 선택, X/Y/Z 축, 좌우 회전, 상하 이동, PageUp/Down 피벗, +/- 균일 배율, R 복원\n"
        << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

bool Console::applyControls() {
    if (!graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds))
        return false;
    return demo_ == Demo::Grid ? updateGrid() : demo_ == Demo::Room ? updateRoom(0) : true;
}

void Console::onUpdate(float dt) {
    if (practiceMode_) {
        practiceMode_->onUpdate(dt);
        return;
    }
    if (demo_ != Demo::Motion && !clock_.advance(dt)) {
        app_.requestClose();
        return;
    }
    bool ok = demo_ == Demo::Animation ? updateAnimation()
        : graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds);
    if (demo_ == Demo::Grid)
        ok = updateGrid() && ok;
    else if (demo_ == Demo::Room)
        ok = updateRoom(clock_.playing ? dt : 0) && ok;
    if (!ok)
        app_.requestClose();
}

/// 현재 장면을 지정한 뷰로 렌더링한다.
void Console::onCursor(double x, double y) {
    if (practiceMode_)
        practiceMode_->onCursor(x, y);
}

void Console::onFocus(bool focused) {
    if (practiceMode_)
        practiceMode_->onFocus(focused);
}

bool Console::onRender() {
    if (practiceMode_)
        return practiceMode_->onRender();
    if (!threeViews_)
        return app_.renderScene();

    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    return app_.renderViews(
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
````

## standard/Console.h

조립 Scene의 피킹 결과를 native 의미상 선택으로 전달한다.

### before

````cpp
#pragma once
#include "framework/Framework.h"
#include "modes/PracticeMode.h"

class Console {
    graphics::Application& app_;
    std::optional<int> initialMode_;
    int activeMode_ = 1;
    std::unique_ptr<PracticeMode> practiceMode_;
    bool activateMode(int mode);
    bool initMotion(int kind);
    bool updateGrid();
    bool updateRoom(float dt);
    bool updateAnimation();
    std::optional<graphics::MotionId> pathId_;
    std::optional<graphics::MotionId> spinId_;
    std::optional<graphics::MotionId> globalId_;
    std::optional<graphics::LayerId> mirrorLayer_;
    std::optional<graphics::LayerId> normalSpinLayer_;
    std::optional<graphics::LayerId> insertedLayer_;
    bool paused_ = false;
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;

    enum class Demo { Motion, Grid, Room, Animation };

    Demo demo_ = Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_ = 0, roomCube_ = 0;
    std::vector<graphics::JointAnimation> animation_;

    enum class ControlKind { Object, Joint, RoomFrame, RoomBody };

    struct JointControl {
        std::size_t track = 0;
        std::string name;
        graphics::Transform3D initialRest;
        ControlKind kind = ControlKind::Object;
    };

    std::vector<JointControl> jointControls_;
    std::optional<std::size_t> roomControl_, bodyControl_;
    graphics::EntityId roomFrame_ = 0, bodyFrame_ = 0;
    glm::mat3 releasedRotation_{1};
    bool addControl(graphics::EntityId id, const std::string& name, ControlKind kind = ControlKind::Object);
    bool applyControls();
    void selectPicked(std::optional<graphics::PickResult> hit);
    std::size_t selectedJoint_ = 0;
    int selectedAxis_ = 0;


    /// 선택한 관절의 기준 SRT와 피벗을 키 입력으로 편집한다.
    bool handleJointKey(int key);


    /// 현재 선택 관절과 조작 축을 콘솔에 출력한다.
    void printSelectedJoint() const;


    bool threeViews_ = false, trace_ = false, floorOpen_ = false;


    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();


    /// 회전하는 방 예제를 구성한다.
    bool initRoom();


    /// 선형보간·그네·사람 관절의 계층 애니메이션 예제를 구성한다.
    bool initAnimation();


    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);


    /// 반사 변환을 순서대로 변경한다.
    bool changeReflection();


    /// 크기 레이어를 삽입하거나 제거한다.
    bool toggleInsertedLayer();


    /// 조작 방법을 출력한다.
    void printHelp() const;


public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app, std::optional<int> initialMode = std::nullopt)
        : app_(app), initialMode_(initialMode) {
    }


    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();


    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods = 0);
    void onCursor(double x, double y);
    void onFocus(bool focused);


    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);


    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);


    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
};
````

### after

````cpp
#pragma once
#include "framework/Framework.h"
#include "modes/PracticeMode.h"

class Console {
    graphics::Application& app_;
    std::optional<int> initialMode_;
    int activeMode_ = 1;
    std::unique_ptr<PracticeMode> practiceMode_;
    bool activateMode(int mode);
    bool initMotion(int kind);
    bool updateGrid();
    bool updateRoom(float dt);
    bool updateAnimation();
    std::optional<graphics::MotionId> pathId_;
    std::optional<graphics::MotionId> spinId_;
    std::optional<graphics::MotionId> globalId_;
    std::optional<graphics::LayerId> mirrorLayer_;
    std::optional<graphics::LayerId> normalSpinLayer_;
    std::optional<graphics::LayerId> insertedLayer_;
    bool paused_ = false;
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;

    enum class Demo { Motion, Grid, Room, Animation };

    Demo demo_ = Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_ = 0, roomCube_ = 0;
    std::vector<graphics::JointAnimation> animation_;

    enum class ControlKind { Object, Joint, RoomFrame, RoomBody };

    struct JointControl {
        std::size_t track = 0;
        std::string name;
        graphics::Transform3D initialRest;
        ControlKind kind = ControlKind::Object;
    };

    std::vector<JointControl> jointControls_;
    std::optional<std::size_t> roomControl_, bodyControl_;
    graphics::EntityId roomFrame_ = 0, bodyFrame_ = 0;
    glm::mat3 releasedRotation_{1};
    bool addControl(graphics::EntityId id, const std::string& name, ControlKind kind = ControlKind::Object);
    bool applyControls();
    void selectPicked(std::optional<graphics::PickResult> hit);
    std::size_t selectedJoint_ = 0;
    int selectedAxis_ = 0;


    /// 선택한 관절의 기준 SRT와 피벗을 키 입력으로 편집한다.
    bool handleJointKey(int key);


    /// 현재 선택 관절과 조작 축을 콘솔에 출력한다.
    void printSelectedJoint() const;


    bool threeViews_ = false, trace_ = false, floorOpen_ = false;


    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();


    /// 회전하는 방 예제를 구성한다.
    bool initRoom();


    /// 선형보간·그네·사람 관절의 계층 애니메이션 예제를 구성한다.
    bool initAnimation();


    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);


    /// 반사 변환을 순서대로 변경한다.
    bool changeReflection();


    /// 크기 레이어를 삽입하거나 제거한다.
    bool toggleInsertedLayer();


    /// 조작 방법을 출력한다.
    void printHelp() const;


public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app, std::optional<int> initialMode = std::nullopt)
        : app_(app), initialMode_(initialMode) {
    }


    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    void selectFromContainer(std::optional<graphics::PickResult> hit) { if (!practiceMode_) selectPicked(hit); }


    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods = 0);
    void onCursor(double x, double y);
    void onFocus(bool focused);


    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);


    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);


    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
};
````

## standard/framework/detail/Scene.cpp

유효한 affine 면 프레임만 저장하고 기존 faceWorldMatrix 합성 경로를 재사용한다.

### before

````cpp
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
````

### after

````cpp
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

bool Scene::setFaceFrame(EntityId id, std::size_t faceIndex, const glm::mat4& matrix) {
    if (!getMesh(id, faceIndex) || !TransformSystem::validAffine(matrix))
        return false;
    components_[id].polyhedron.faces[faceIndex].transformFrame = matrix;
    return true;
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
````

## standard/framework/include/Graphics.h

원본 최종 면 자세와 작성자 면 자세를 별도로 합성하는 setFaceFrame API를 공개한다.

### before

````cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include "RenderConfig.h"
#include <map>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;


    void rebuildFaceIds();


public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();


    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});


    /// [API] 반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32,
                                      unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);


    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);


    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});


    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);


    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);


    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);


    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);


    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);


    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);


    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);


    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);


    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.
    bool setParent(EntityId id, std::optional<EntityId> parent);


    /// [API] 부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;


    /// [API] 엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;


    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    MotionSystem& getMotions() {
        return motions_;
    }


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const MotionSystem& getMotions() const {
        return motions_;
    }


    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);


    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);


    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;


    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
    bool updateMotions(double deltaSeconds) {
        return motions_.update(deltaSeconds);
    }


    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);


    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);


    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);


    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);


    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;


    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);


    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);


    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);


    /// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});


    /// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                            float rotationDegrees = 0);


    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);


    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);


    /// [API] 메시의 색상·텍스처 등 재질 데이터를 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);


    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});


    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);


    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);


    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);


    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);


    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
    std::optional<std::array<LineId, 3>> addBasis(const glm::vec3& origin,
                                                  const glm::mat3& basisVectors = glm::mat3(1.0f),
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);


    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.


    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
    const std::vector<EntityId>& getIds() const {
        return polyId_;
    }


    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;


    /// [API] 직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getCubeFaceIds() const {
        return cubeFaceId_;
    }


    /// [API] 사면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getTetraFaceIds() const {
        return tetraFaceId_;
    }


    /// [API] 피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getPyramidFaceIds() const {
        return pyramidFaceId_;
    }


    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const {
        return lines_;
    }


    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
    std::uint64_t getGeometryRevision() const {
        return geometryRevision_;
    }
};

class DiagramSystem {
public:
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));


    /// [API] 반지름·분할 수로 CPU 구 메시 모델을 생성한다.
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.
    static Model makeCone(float radius, float height, unsigned slices = 32);


    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);


    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);


    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);


    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{2.8f, 2.0f, 5.0f};
    glm::vec3 target{0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

class CameraSystem {
public:
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);


    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);


    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);


struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
    bool textures = false; // Explicit opt-in; no texture GPU work when false.
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
    CullMode cull = CullMode::None; // Append to preserve existing aggregate initializers.
};

struct Viewport {
    int x = 0, y = 0, width = 0, height = 0;
}; // Bottom-left framebuffer pixels.

struct RenderPass {
    Camera camera;
    Viewport viewport;
    RenderOptions options;
    bool clearColor = true, clearDepth = true;
};

class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };

    RenderConfig config_;
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1, tintLocation_ = -1;
    std::vector<DrawRange> drawRanges_;

    using TextureMap =
        std::map<std::shared_ptr<const TextureImage>, GLuint, std::owner_less<std::shared_ptr<const TextureImage>>>;

    TextureMap textures_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;


    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);


    bool upload(const Scene& scene, bool textures);


    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;


public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer(const Renderer&) = delete;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer& operator=(const Renderer&) = delete;


    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{},
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});


    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});


    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);


    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);


    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy(); // Call while the owning context is current, before it is destroyed.


    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.
    void abandon() {
        shaderProgramID_ = vao_ = vbo_ = ebo_ = 0;
        textures_.clear();
        drawRanges_.clear();
        uploadedScene_ = nullptr;
        uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    }
};
} // namespace graphics
````

### after

````cpp
#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include "RenderConfig.h"
#include <map>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;


    void rebuildFaceIds();


public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();


    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});


    /// [API] 반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32,
                                      unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);


    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);


    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});


    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);


    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);

    /// [API] 형상 재업로드 없이 면 앞쪽의 부모 프레임을 설정한다. 렌더·피킹·법선 변환에 함께 반영한다.
    bool setFaceFrame(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);


    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);


    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);


    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);


    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);


    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);


    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);


    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.
    bool setParent(EntityId id, std::optional<EntityId> parent);


    /// [API] 부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;


    /// [API] 엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;


    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    MotionSystem& getMotions() {
        return motions_;
    }


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const MotionSystem& getMotions() const {
        return motions_;
    }


    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);


    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);


    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;


    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
    bool updateMotions(double deltaSeconds) {
        return motions_.update(deltaSeconds);
    }


    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);


    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);


    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);


    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);


    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;


    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);


    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);


    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);


    /// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});


    /// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                            float rotationDegrees = 0);


    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);


    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);


    /// [API] 메시의 색상·텍스처 등 재질 데이터를 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);


    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});


    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);


    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);


    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);


    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);


    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
    std::optional<std::array<LineId, 3>> addBasis(const glm::vec3& origin,
                                                  const glm::mat3& basisVectors = glm::mat3(1.0f),
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);


    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.


    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
    const std::vector<EntityId>& getIds() const {
        return polyId_;
    }


    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;


    /// [API] 직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getCubeFaceIds() const {
        return cubeFaceId_;
    }


    /// [API] 사면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getTetraFaceIds() const {
        return tetraFaceId_;
    }


    /// [API] 피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getPyramidFaceIds() const {
        return pyramidFaceId_;
    }


    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const {
        return lines_;
    }


    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
    std::uint64_t getGeometryRevision() const {
        return geometryRevision_;
    }
};

class DiagramSystem {
public:
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));


    /// [API] 반지름·분할 수로 CPU 구 메시 모델을 생성한다.
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.
    static Model makeCone(float radius, float height, unsigned slices = 32);


    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);


    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);


    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);


    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{2.8f, 2.0f, 5.0f};
    glm::vec3 target{0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

class CameraSystem {
public:
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);


    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);


    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);


struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
    bool textures = false; // Explicit opt-in; no texture GPU work when false.
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
    CullMode cull = CullMode::None; // Append to preserve existing aggregate initializers.
};

struct Viewport {
    int x = 0, y = 0, width = 0, height = 0;
}; // Bottom-left framebuffer pixels.

struct RenderPass {
    Camera camera;
    Viewport viewport;
    RenderOptions options;
    bool clearColor = true, clearDepth = true;
};

class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };

    RenderConfig config_;
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1, tintLocation_ = -1;
    std::vector<DrawRange> drawRanges_;

    using TextureMap =
        std::map<std::shared_ptr<const TextureImage>, GLuint, std::owner_less<std::shared_ptr<const TextureImage>>>;

    TextureMap textures_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;


    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);


    bool upload(const Scene& scene, bool textures);


    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;


public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer(const Renderer&) = delete;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer& operator=(const Renderer&) = delete;


    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{},
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});


    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});


    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);


    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);


    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy(); // Call while the owning context is current, before it is destroyed.


    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.
    void abandon() {
        shaderProgramID_ = vao_ = vbo_ = ebo_ = 0;
        textures_.clear();
        drawRanges_.clear();
        uploadedScene_ = nullptr;
        uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    }
};
} // namespace graphics
````

## standard/modes/Assembly.cpp

clear에서 컨테이너 상태를 정리하고 공통 변환/가시성 소유권을 보존한다.

### before

````cpp
#include "Assembly.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace assembly {
void require(bool success, const std::string& message) {
    if (!success)
        throw std::runtime_error(message);
}

Tokens tokenize(const std::string& line) {
    Tokens result;
    std::string token;
    bool quoted = false, started = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (c == '#' && !quoted)
            break;
        if (c == '"') {
            quoted = !quoted;
            started = true;
        } else if (c == '\\' && quoted && i + 1 < line.size() && (line[i + 1] == '"' || line[i + 1] == '\\')) {
            token += line[++i];
        } else if (!quoted && std::isspace(static_cast<unsigned char>(c))) {
            if (started) {
                result.push_back(token);
                token.clear();
                started = false;
            }
        } else {
            token += c;
            started = true;
        }
    }
    require(!quoted, "닫히지 않은 따옴표입니다.");
    if (started)
        result.push_back(token);
    return result;
}

std::string quote(const std::string& text) {
    std::ostringstream out;
    out << std::quoted(text);
    return out.str();
}

double number(const std::string& text) {
    std::size_t consumed = 0;
    const double value = std::stod(text, &consumed);
    require(consumed == text.size() && std::isfinite(value) && std::abs(value) <= 1000000,
            "유한한 수(-1000000~1000000)가 필요합니다: " + text);
    return value;
}

std::size_t indexNumber(const std::string& text) {
    const auto value = number(text);
    require(value >= 0 && value == std::floor(value), "0 이상 정수 인덱스가 필요합니다.");
    return std::size_t(value);
}

glm::vec3 vector(const Tokens& args, std::size_t offset) {
    require(offset + 3 <= args.size(), "XYZ 값 3개가 필요합니다.");
    return {float(number(args[offset])), float(number(args[offset + 1])), float(number(args[offset + 2]))};
}

glm::vec3 axis(const std::string& text) {
    require(text == "x" || text == "y" || text == "z", "축은 x/y/z 중 하나입니다.");
    return text == "x" ? glm::vec3(1, 0, 0) : text == "y" ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, 1);
}

bool boolean(const std::string& text) {
    require(text == "on" || text == "off", "on 또는 off가 필요합니다.");
    return text == "on";
}

bool rigid(const glm::mat4& matrix) {
    const auto r = glm::mat3(matrix), gram = glm::transpose(r) * r;
    return graphics::TransformSystem::validAffine(matrix) && std::abs(glm::determinant(r) - 1) < .001f &&
        glm::length(gram[0] - glm::vec3(1, 0, 0)) < .001f &&
        glm::length(gram[1] - glm::vec3(0, 1, 0)) < .001f &&
        glm::length(gram[2] - glm::vec3(0, 0, 1)) < .001f;
}

Node& Assembly::node(const std::string& name) {
    const auto found = state_.nodes.find(name);
    require(found != state_.nodes.end(), "없는 객체/부품입니다: " + name + ". list 또는 guide objects를 사용하세요.");
    return found->second;
}

const Node& Assembly::node(const std::string& name) const {
    const auto found = state_.nodes.find(name);
    require(found != state_.nodes.end(), "없는 객체/부품입니다: " + name);
    return found->second;
}

void Assembly::clear() {
    ++topologyRevision_;
    app_.getScene().clear();
    state_ = State{};
    faulted_ = false;
}

void Assembly::registerNode(const std::string& name, graphics::EntityId id, bool whole,
                            const std::string& kind, std::optional<graphics::EntityId> mesh) {
    require(state_.nodes.size() < 1000, "객체/부품 이름 1000개 한도입니다.");
    require(!name.empty() && !state_.nodes.count(name), "중복 이름입니다: " + name);
    const auto* component = app_.getScene().getComponent(id);
    require(component != nullptr, "등록할 엔티티가 없습니다.");
    for (const auto& item : state_.nodes)
        require(item.second.id != id, "같은 엔티티를 두 이름으로 등록할 수 없습니다.");
    ++topologyRevision_;
    state_.nodes.emplace(name, Node{id, mesh, component->transform, component->transform, kind, whole, {}, {}});
    if (state_.selected.empty() && whole)
        state_.selected = name;
}

Behavior* Assembly::positionDriver(const std::string& target) {
    for (auto& behavior : state_.behaviors)
        if (behavior.target == target && behavior.positionDriver)
            return &behavior;
    return nullptr;
}

void Assembly::attach(Behavior behavior) {
    require(!behavior.name.empty(), "동작 이름이 필요합니다.");
    const auto& targetNode = node(behavior.target);
    require(!targetNode.affineGroup || behavior.effect == Effect::Custom || behavior.effect == Effect::Input,
            "비균일/반사 그룹은 정적 계층입니다. 자식 group/joint에 모션을 붙이세요.");
    if (behavior.effect == Effect::Input)
        require(bool(behavior.key) || bool(behavior.mouse) || bool(behavior.cursor) || bool(behavior.focus), "Input에는 입력 콜백이 필요합니다.");
    if (behavior.effect == Effect::Custom) {
        require(bool(behavior.custom), "Custom 동작 함수가 필요합니다.");
        behavior.positionDriver = true;
    }
    if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Conveyor)
        behavior.positionDriver = true;
    const auto checkGeometryOwner = [&](const Behavior& physics, const Behavior& other) {
        if (other.effect == Effect::Input || (physics.effect != Effect::Body && physics.effect != Effect::Walk)) return;
        const auto& object = node(other.target);
        const auto& room = node(physics.reference);
        const bool face = faceEffect(other.effect);
        if (physics.effect == Effect::Body && face)
            require(object.mesh != node(physics.target).mesh, "물리 상자 면은 별도로 변형할 수 없습니다.");
        if (!face)
            require(!room.mesh || object.id != *room.mesh, "물리 방 walls에는 별도 이동/회전을 붙일 수 없습니다. 방 루트를 사용하세요.");
    };
    for (const auto& current : state_.behaviors) {
        checkGeometryOwner(current, behavior);
        checkGeometryOwner(behavior, current);
        require(current.name != behavior.name, "중복 동작 이름입니다: " + behavior.name);
        require(behavior.effect == Effect::Input || current.effect == Effect::Input ||
                (!(current.effect == Effect::Body && behavior.target == current.target + ".mesh") &&
                 !(behavior.effect == Effect::Body && current.target == behavior.target + ".mesh")),
                "물리 메시에는 별도 동작을 붙일 수 없습니다. 상자 루트를 사용하세요.");
        const bool currentFace = faceEffect(current.effect);
        const bool newFace = faceEffect(behavior.effect);
        if (currentFace && newFace)
            require(node(current.target).mesh != node(behavior.target).mesh || current.faceIndex != behavior.faceIndex,
                    "같은 메시의 같은 면에 동작이 이미 있습니다.");
        if (current.target != behavior.target)
            continue;
        require(!(current.positionDriver && behavior.positionDriver), "위치 제어가 충돌합니다. 기존 동작을 remove 하거나 자식 부품에 붙이세요.");
        const bool currentPhysics = current.effect == Effect::Body || current.effect == Effect::Walk || current.effect == Effect::Custom;
        const bool newPhysics = behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Custom;
        require(!((currentPhysics && behavior.effect == Effect::Motion) || (newPhysics && current.effect == Effect::Motion)),
                "물리 루트와 Motion은 같은 SRT를 소유할 수 없습니다. 자식 부품에 동작을 붙이세요.");
    }
    if (behavior.effect == Effect::Motion || behavior.effect == Effect::FaceTrack) {
        for (const auto& step : behavior.motion.getSteps())
            require(!step.action, "변환 모션에 action을 넣을 수 없습니다. bind 명령 또는 customCommand를 사용하세요.");
        const auto sample = behavior.motion.sample(0);
        require(sample.has_value(), "유효한 모션 샘플이 필요합니다.");
        if (behavior.effect == Effect::Motion)
            require(rigid(sample->parent) && rigid(sample->local), "객체 Motion은 유효한 강체 변환이어야 합니다.");
    }
    if (faceEffect(behavior.effect)) {
        const auto& target = node(behavior.target);
        require(target.mesh.has_value(), "면 동작에는 메시가 필요합니다.");
        const auto* component = app_.getScene().getComponent(*target.mesh);
        require(component && behavior.faceIndex < component->polyhedron.faces.size(), "면 번호 범위를 확인하세요.");
        const auto& face = component->polyhedron.faces[behavior.faceIndex];
        behavior.initialFaceMatrix = face.localAnimationMatrix;
        behavior.initialFaceSpace = face.animationSpace;
        behavior.initialFaceVisible = face.visible;
        const auto srt = graphics::detail::faceSrtMatrix(face);
        behavior.faceTrackBase = face.animationSpace == graphics::FaceAnimationSpace::Model
            ? face.localAnimationMatrix : srt * face.localAnimationMatrix * glm::inverse(srt);
        require(graphics::TransformSystem::validAffine(behavior.faceTrackBase), "면의 기준 행렬이 특이합니다.");
    }
    state_.behaviors.push_back(std::move(behavior));
    ++topologyRevision_;
}

void Assembly::setRest(const std::string& target, const graphics::Transform3D& rest) {
    auto& value = node(target);
    bool immutable = false;
    for (const auto& behavior : state_.behaviors) {
        immutable = immutable || (behavior.effect == Effect::Body && target == behavior.target + ".mesh");
        if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk) {
            const auto& room = node(behavior.reference);
            immutable = immutable || (room.mesh && value.id == *room.mesh);
        }
    }
    if (immutable) {
        require(rest.basis == value.rest.basis && rest.localBasis == value.rest.localBasis && rest.scale == value.rest.scale &&
            rest.rotation[0] == value.rest.rotation[0] && rest.rotation[1] == value.rest.rotation[1] &&
            rest.rotation[2] == value.rest.rotation[2], "물리 메시/walls 대신 객체 루트를 편집하세요.");
        return;
    }
    require(glm::all(glm::lessThanEqual(glm::abs(rest.basis), glm::vec3(1000000))) &&
            glm::all(glm::lessThanEqual(glm::abs(rest.localBasis), glm::vec3(1000000))), "위치/피벗 범위는 ±1000000입니다.");
    const bool validScale = value.affineGroup
        ? glm::all(glm::greaterThan(glm::abs(rest.scale), glm::vec3(.0001f))) &&
          glm::all(glm::lessThanEqual(glm::abs(rest.scale), glm::vec3(100)))
        : rest.scale.x > .0001f && rest.scale.x <= 100 && std::abs(rest.scale.x - rest.scale.y) < .0001f &&
          std::abs(rest.scale.x - rest.scale.z) < .0001f;
    require(graphics::DiagramSystem::validTransform(rest) && validScale,
            "유효한 SRT가 필요합니다. 기본은 양수 균일 배율이며 warp 그룹은 0이 아닌 축별 배율(.0001~100)을 씁니다.");
    for (const auto& behavior : state_.behaviors) {
        if ((behavior.effect == Effect::Body || behavior.effect == Effect::Walk) && behavior.reference == target)
            require(rigid(graphics::DiagramSystem::modelMatrix(rest)), "물리 공간은 강체 변환을 유지해야 합니다.");
        if (behavior.effect == Effect::Walk && behavior.reference == target)
            require(glm::length(rest.rotation[0] - glm::vec3(1, 0, 0)) < .001f &&
                    glm::length(rest.rotation[1] - glm::vec3(0, 1, 0)) < .001f,
                    "보행 충돌 공간은 월드 축 정렬을 유지해야 합니다.");
    }
    if (auto* driver = positionDriver(target)) {
        if (driver->effect == Effect::Body) {
            require(glm::length(rest.localBasis) < .0001f, "물리 상자의 피벗은 중심을 유지합니다.");
            const auto matrix = app_.getScene().worldMatrix(node(driver->reference).id);
            require(matrix && rigid(*matrix), "방의 강체 행렬이 필요합니다.");
            const auto delta = rest.basis - value.rest.basis;
            if (driver->body.released)
                driver->body.worldPosition += delta;
            else
                driver->body.localPosition += glm::transpose(glm::mat3(*matrix)) * delta;
        } else if (driver->effect == Effect::Walk) {
            require(rest.scale == glm::vec3(1) && glm::length(rest.localBasis) < .0001f,
                    "보행 루트는 배율 1과 중심 피벗을 유지합니다.");
            require(std::abs(rest.rotation[1].y - 1) < .001f, "보행 루트는 Y축 회전만 지원합니다.");
            driver->character.feet += rest.basis - value.rest.basis;
            driver->character.direction = rest.rotation * glm::vec3(0, 0, 1);
        }
    }
    value.rest = rest;
}

void Assembly::move(const std::string& target, glm::vec3 delta) {
    auto rest = node(target).rest;
    rest.basis += delta;
    setRest(target, rest);
}

void Assembly::rotate(const std::string& target, glm::vec3 direction, float degrees) {
    auto rest = node(target).rest;
    rest.rotation *= glm::mat3(glm::rotate(glm::mat4(1), glm::radians(degrees), direction));
    const auto x = glm::normalize(rest.rotation[0]);
    const auto z = glm::normalize(glm::cross(x, rest.rotation[1]));
    rest.rotation = glm::mat3(x, glm::cross(z, x), z);
    setRest(target, rest);
}

void Assembly::inspect(const std::string& target, std::ostream& out) const {
    const auto& value = node(target);
    const auto& t = value.rest;
    out << target << " kind=" << value.kind << " id=" << value.id << " position="
        << t.basis.x << ',' << t.basis.y << ',' << t.basis.z << " scale=" << t.scale.x << '\n';
    for (const auto& behavior : state_.behaviors)
        if (behavior.target == target)
            out << "  " << behavior.name << " time=" << behavior.seconds << " playing=" << behavior.playing
                << " enabled=" << behavior.enabled << '\n';
}

bool Assembly::execute(const std::string& line, std::ostream& out) {
    Tokens args;
    try {
        require(line.size() <= 4096, "명령은 4096바이트 이하입니다.");
        args = tokenize(line);
    } catch (const std::exception& error) {
        out << "명령 거절: " << error.what() << '\n';
        return false;
    }
    if (args.empty())
        return true;
    auto savedScene = app_.getScene();
    const auto savedState = state_;
    const auto savedCamera = app_.getCamera();
    const auto savedOptions = app_.getRenderOptions();
    const bool savedFault = faulted_;
    const bool readOnly = args[0] == "save" || args[0] == "guide" || args[0] == "help" || args[0] == "list" || args[0] == "inspect" || args[0] == "surface";
    try {
        require(dispatch(args, out), "지원하지 않는 명령입니다. help 또는 guide custom을 사용하세요.");
        if (!readOnly)
            require(applyPoses(0), "조립 결과의 상태/변환이 유효하지 않습니다.");
        if (!replaying_ && !readOnly && args[0] != "load" && args[0] != "select" && args[0] != "jump" && args[0] != "release")
            state_.recipe.push_back(line);
        if (!readOnly) faulted_ = false;
        return true;
    } catch (const std::exception& error) {
        app_.getScene() = std::move(savedScene);
        state_ = savedState;
        faulted_ = savedFault;
        app_.getCamera() = savedCamera;
        app_.getRenderOptions() = savedOptions;
        out << "명령 거절: " << error.what() << "\n이전 조립 상태를 유지했습니다. guide " << args[0] << '\n';
        return false;
    }
}
} // namespace assembly
````

### after

````cpp
#include "Assembly.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace assembly {
void require(bool success, const std::string& message) {
    if (!success)
        throw std::runtime_error(message);
}

Tokens tokenize(const std::string& line) {
    Tokens result;
    std::string token;
    bool quoted = false, started = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (c == '#' && !quoted)
            break;
        if (c == '"') {
            quoted = !quoted;
            started = true;
        } else if (c == '\\' && quoted && i + 1 < line.size() && (line[i + 1] == '"' || line[i + 1] == '\\')) {
            token += line[++i];
        } else if (!quoted && std::isspace(static_cast<unsigned char>(c))) {
            if (started) {
                result.push_back(token);
                token.clear();
                started = false;
            }
        } else {
            token += c;
            started = true;
        }
    }
    require(!quoted, "닫히지 않은 따옴표입니다.");
    if (started)
        result.push_back(token);
    return result;
}

std::string quote(const std::string& text) {
    std::ostringstream out;
    out << std::quoted(text);
    return out.str();
}

double number(const std::string& text) {
    std::size_t consumed = 0;
    const double value = std::stod(text, &consumed);
    require(consumed == text.size() && std::isfinite(value) && std::abs(value) <= 1000000,
            "유한한 수(-1000000~1000000)가 필요합니다: " + text);
    return value;
}

std::size_t indexNumber(const std::string& text) {
    const auto value = number(text);
    require(value >= 0 && value == std::floor(value), "0 이상 정수 인덱스가 필요합니다.");
    return std::size_t(value);
}

glm::vec3 vector(const Tokens& args, std::size_t offset) {
    require(offset + 3 <= args.size(), "XYZ 값 3개가 필요합니다.");
    return {float(number(args[offset])), float(number(args[offset + 1])), float(number(args[offset + 2]))};
}

glm::vec3 axis(const std::string& text) {
    require(text == "x" || text == "y" || text == "z", "축은 x/y/z 중 하나입니다.");
    return text == "x" ? glm::vec3(1, 0, 0) : text == "y" ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, 1);
}

bool boolean(const std::string& text) {
    require(text == "on" || text == "off", "on 또는 off가 필요합니다.");
    return text == "on";
}

bool rigid(const glm::mat4& matrix) {
    const auto r = glm::mat3(matrix), gram = glm::transpose(r) * r;
    return graphics::TransformSystem::validAffine(matrix) && std::abs(glm::determinant(r) - 1) < .001f &&
        glm::length(gram[0] - glm::vec3(1, 0, 0)) < .001f &&
        glm::length(gram[1] - glm::vec3(0, 1, 0)) < .001f &&
        glm::length(gram[2] - glm::vec3(0, 0, 1)) < .001f;
}

Node& Assembly::node(const std::string& name) {
    const auto found = state_.nodes.find(name);
    require(found != state_.nodes.end(), "없는 객체/부품입니다: " + name + ". list 또는 guide objects를 사용하세요.");
    return found->second;
}

const Node& Assembly::node(const std::string& name) const {
    const auto found = state_.nodes.find(name);
    require(found != state_.nodes.end(), "없는 객체/부품입니다: " + name);
    return found->second;
}

void Assembly::clear() {
    ++topologyRevision_;
    app_.getScene().clear();
    state_ = State{};
    faulted_ = false;
}

void Assembly::registerNode(const std::string& name, graphics::EntityId id, bool whole,
                            const std::string& kind, std::optional<graphics::EntityId> mesh) {
    require(state_.nodes.size() < 1000, "객체/부품 이름 1000개 한도입니다.");
    require(!name.empty() && !state_.nodes.count(name), "중복 이름입니다: " + name);
    const auto* component = app_.getScene().getComponent(id);
    require(component != nullptr, "등록할 엔티티가 없습니다.");
    for (const auto& item : state_.nodes)
        require(item.second.id != id, "같은 엔티티를 두 이름으로 등록할 수 없습니다.");
    ++topologyRevision_;
    state_.nodes.emplace(name, Node{id, mesh, component->transform, component->transform, kind, whole, {}, {}});
    if (state_.selected.empty() && whole)
        state_.selected = name;
}

Behavior* Assembly::positionDriver(const std::string& target) {
    for (auto& behavior : state_.behaviors)
        if (behavior.target == target && behavior.positionDriver)
            return &behavior;
    return nullptr;
}

void Assembly::attach(Behavior behavior) {
    require(!behavior.name.empty(), "동작 이름이 필요합니다.");
    const auto& targetNode = node(behavior.target);
    require(!targetNode.affineGroup || behavior.effect == Effect::Custom || behavior.effect == Effect::Input,
            "비균일/반사 그룹은 정적 계층입니다. 자식 group/joint에 모션을 붙이세요.");
    if (behavior.effect == Effect::Input)
        require(bool(behavior.key) || bool(behavior.mouse) || bool(behavior.cursor) || bool(behavior.focus), "Input에는 입력 콜백이 필요합니다.");
    if (behavior.effect == Effect::Custom) {
        require(bool(behavior.custom), "Custom 동작 함수가 필요합니다.");
        behavior.positionDriver = true;
    }
    if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Conveyor)
        behavior.positionDriver = true;
    const auto checkGeometryOwner = [&](const Behavior& physics, const Behavior& other) {
        if (other.effect == Effect::Input || (physics.effect != Effect::Body && physics.effect != Effect::Walk)) return;
        const auto& object = node(other.target);
        const auto& room = node(physics.reference);
        const bool face = faceEffect(other.effect);
        if (physics.effect == Effect::Body && face)
            require(object.mesh != node(physics.target).mesh, "물리 상자 면은 별도로 변형할 수 없습니다.");
        if (!face)
            require(!room.mesh || object.id != *room.mesh, "물리 방 walls에는 별도 이동/회전을 붙일 수 없습니다. 방 루트를 사용하세요.");
    };
    for (const auto& current : state_.behaviors) {
        checkGeometryOwner(current, behavior);
        checkGeometryOwner(behavior, current);
        require(current.name != behavior.name, "중복 동작 이름입니다: " + behavior.name);
        require(behavior.effect == Effect::Input || current.effect == Effect::Input ||
                (!(current.effect == Effect::Body && behavior.target == current.target + ".mesh") &&
                 !(behavior.effect == Effect::Body && current.target == behavior.target + ".mesh")),
                "물리 메시에는 별도 동작을 붙일 수 없습니다. 상자 루트를 사용하세요.");
        const bool currentFace = faceEffect(current.effect);
        const bool newFace = faceEffect(behavior.effect);
        if (currentFace && newFace)
            require(node(current.target).mesh != node(behavior.target).mesh || current.faceIndex != behavior.faceIndex,
                    "같은 메시의 같은 면에 동작이 이미 있습니다.");
        if (current.target != behavior.target)
            continue;
        require(!(current.positionDriver && behavior.positionDriver), "위치 제어가 충돌합니다. 기존 동작을 remove 하거나 자식 부품에 붙이세요.");
        const bool currentPhysics = current.effect == Effect::Body || current.effect == Effect::Walk || current.effect == Effect::Custom;
        const bool newPhysics = behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Custom;
        require(!((currentPhysics && behavior.effect == Effect::Motion) || (newPhysics && current.effect == Effect::Motion)),
                "물리 루트와 Motion은 같은 SRT를 소유할 수 없습니다. 자식 부품에 동작을 붙이세요.");
    }
    if (behavior.effect == Effect::Motion || behavior.effect == Effect::FaceTrack) {
        for (const auto& step : behavior.motion.getSteps())
            require(!step.action, "변환 모션에 action을 넣을 수 없습니다. bind 명령 또는 customCommand를 사용하세요.");
        const auto sample = behavior.motion.sample(0);
        require(sample.has_value(), "유효한 모션 샘플이 필요합니다.");
        if (behavior.effect == Effect::Motion)
            require(rigid(sample->parent) && rigid(sample->local), "객체 Motion은 유효한 강체 변환이어야 합니다.");
    }
    if (faceEffect(behavior.effect)) {
        const auto& target = node(behavior.target);
        require(target.mesh.has_value(), "면 동작에는 메시가 필요합니다.");
        const auto* component = app_.getScene().getComponent(*target.mesh);
        require(component && behavior.faceIndex < component->polyhedron.faces.size(), "면 번호 범위를 확인하세요.");
        const auto& face = component->polyhedron.faces[behavior.faceIndex];
        behavior.initialFaceMatrix = face.localAnimationMatrix;
        behavior.initialFaceSpace = face.animationSpace;
        behavior.initialFaceVisible = face.visible;
        const auto srt = graphics::detail::faceSrtMatrix(face);
        behavior.faceTrackBase = face.animationSpace == graphics::FaceAnimationSpace::Model
            ? face.localAnimationMatrix : srt * face.localAnimationMatrix * glm::inverse(srt);
        require(graphics::TransformSystem::validAffine(behavior.faceTrackBase), "면의 기준 행렬이 특이합니다.");
    }
    state_.behaviors.push_back(std::move(behavior));
    ++topologyRevision_;
}

void Assembly::setRest(const std::string& target, const graphics::Transform3D& rest) {
    auto& value = node(target);
    bool immutable = false;
    for (const auto& behavior : state_.behaviors) {
        immutable = immutable || (behavior.effect == Effect::Body && target == behavior.target + ".mesh");
        if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk) {
            const auto& room = node(behavior.reference);
            immutable = immutable || (room.mesh && value.id == *room.mesh);
        }
    }
    if (immutable) {
        require(rest.basis == value.rest.basis && rest.localBasis == value.rest.localBasis && rest.scale == value.rest.scale &&
            rest.rotation[0] == value.rest.rotation[0] && rest.rotation[1] == value.rest.rotation[1] &&
            rest.rotation[2] == value.rest.rotation[2], "물리 메시/walls 대신 객체 루트를 편집하세요.");
        return;
    }
    require(glm::all(glm::lessThanEqual(glm::abs(rest.basis), glm::vec3(1000000))) &&
            glm::all(glm::lessThanEqual(glm::abs(rest.localBasis), glm::vec3(1000000))), "위치/피벗 범위는 ±1000000입니다.");
    const bool validScale = value.affineGroup
        ? glm::all(glm::greaterThan(glm::abs(rest.scale), glm::vec3(.0001f))) &&
          glm::all(glm::lessThanEqual(glm::abs(rest.scale), glm::vec3(100)))
        : rest.scale.x > .0001f && rest.scale.x <= 100 && std::abs(rest.scale.x - rest.scale.y) < .0001f &&
          std::abs(rest.scale.x - rest.scale.z) < .0001f;
    require(graphics::DiagramSystem::validTransform(rest) && validScale,
            "유효한 SRT가 필요합니다. 기본은 양수 균일 배율이며 warp 그룹은 0이 아닌 축별 배율(.0001~100)을 씁니다.");
    for (const auto& behavior : state_.behaviors) {
        if ((behavior.effect == Effect::Body || behavior.effect == Effect::Walk) && behavior.reference == target)
            require(rigid(graphics::DiagramSystem::modelMatrix(rest)), "물리 공간은 강체 변환을 유지해야 합니다.");
        if (behavior.effect == Effect::Walk && behavior.reference == target)
            require(glm::length(rest.rotation[0] - glm::vec3(1, 0, 0)) < .001f &&
                    glm::length(rest.rotation[1] - glm::vec3(0, 1, 0)) < .001f,
                    "보행 충돌 공간은 월드 축 정렬을 유지해야 합니다.");
    }
    if (auto* driver = positionDriver(target)) {
        if (driver->effect == Effect::Body) {
            require(glm::length(rest.localBasis) < .0001f, "물리 상자의 피벗은 중심을 유지합니다.");
            const auto matrix = app_.getScene().worldMatrix(node(driver->reference).id);
            require(matrix && rigid(*matrix), "방의 강체 행렬이 필요합니다.");
            const auto delta = rest.basis - value.rest.basis;
            if (driver->body.released)
                driver->body.worldPosition += delta;
            else
                driver->body.localPosition += glm::transpose(glm::mat3(*matrix)) * delta;
        } else if (driver->effect == Effect::Walk) {
            require(rest.scale == glm::vec3(1) && glm::length(rest.localBasis) < .0001f,
                    "보행 루트는 배율 1과 중심 피벗을 유지합니다.");
            require(std::abs(rest.rotation[1].y - 1) < .001f, "보행 루트는 Y축 회전만 지원합니다.");
            driver->character.feet += rest.basis - value.rest.basis;
            driver->character.direction = rest.rotation * glm::vec3(0, 0, 1);
        }
    }
    value.rest = rest;
}

void Assembly::move(const std::string& target, glm::vec3 delta) {
    auto rest = node(target).rest;
    rest.basis += delta;
    setRest(target, rest);
}

void Assembly::rotate(const std::string& target, glm::vec3 direction, float degrees) {
    auto rest = node(target).rest;
    rest.rotation *= glm::mat3(glm::rotate(glm::mat4(1), glm::radians(degrees), direction));
    const auto x = glm::normalize(rest.rotation[0]);
    const auto z = glm::normalize(glm::cross(x, rest.rotation[1]));
    rest.rotation = glm::mat3(x, glm::cross(z, x), z);
    setRest(target, rest);
}

void Assembly::inspect(const std::string& target, std::ostream& out) const {
    const auto& value = node(target);
    const auto& t = value.rest;
    out << target << " kind=" << value.kind << " id=" << value.id << " position="
        << t.basis.x << ',' << t.basis.y << ',' << t.basis.z << " scale=" << t.scale.x << '\n';
    for (const auto& behavior : state_.behaviors)
        if (behavior.target == target)
            out << "  " << behavior.name << " time=" << behavior.seconds << " playing=" << behavior.playing
                << " enabled=" << behavior.enabled << '\n';
}

bool Assembly::execute(const std::string& line, std::ostream& out) {
    Tokens args;
    try {
        require(line.size() <= 4096, "명령은 4096바이트 이하입니다.");
        args = tokenize(line);
    } catch (const std::exception& error) {
        out << "명령 거절: " << error.what() << '\n';
        return false;
    }
    if (args.empty())
        return true;
    auto savedScene = app_.getScene();
    const auto savedState = state_;
    const auto savedCamera = app_.getCamera();
    const auto savedOptions = app_.getRenderOptions();
    const bool savedFault = faulted_;
    const bool readOnly = args[0] == "save" || args[0] == "guide" || args[0] == "help" || args[0] == "list" || args[0] == "inspect" || args[0] == "surface" || args[0] == "diagnose" ||
        (args[0] == "feature" && args.size() > 1 && (args[1] == "list" || args[1] == "parts"));
    try {
        require(dispatch(args, out), "지원하지 않는 명령입니다. help 또는 guide custom을 사용하세요.");
        if (!readOnly)
            require(applyPoses(0), "조립 결과의 상태/변환이 유효하지 않습니다.");
        if (!replaying_ && !readOnly && args[0] != "load" && args[0] != "reload" && args[0] != "select" && args[0] != "jump" && args[0] != "release" && !(args[0] == "feature" && args.size() > 1 && args[1] == "key"))
            state_.recipe.push_back(line);
        if (!readOnly) faulted_ = false;
        return true;
    } catch (const std::exception& error) {
        app_.getScene() = std::move(savedScene);
        state_ = savedState;
        faulted_ = savedFault;
        app_.getCamera() = savedCamera;
        app_.getRenderOptions() = savedOptions;
        out << "명령 거절: " << error.what() << "\n이전 조립 상태를 유지했습니다. guide " << args[0] << '\n';
        return false;
    }
}
} // namespace assembly
````

## standard/modes/Assembly.h

컨테이너·진단·입력 라우팅·핫로드 상태와 API를 선언한다.

### before

````cpp
#pragma once
#include "PracticeMode.h"
#include <functional>
#include <map>
#include <ostream>

namespace assembly {
using Tokens = std::vector<std::string>;
class Assembly;

struct FaceMount {
    graphics::EntityId mesh = 0;
    std::size_t face = 0, triangle = 0;
    glm::vec3 coordinate{0};
    bool barycentric = true;
};

struct Node {
    graphics::EntityId id = 0;
    std::optional<graphics::EntityId> mesh;
    graphics::Transform3D rest, initial;
    std::string kind;
    bool whole = false;
    std::optional<graphics::Bounds3> room;
    std::optional<graphics::JointRig> walker;
    std::string guideTopic;
    std::optional<FaceMount> mount;
    bool affineGroup = false;
};

enum class Effect { Motion, Face, FaceTrack, Wall, Body, Walk, Conveyor, Custom, Input };
inline bool faceEffect(Effect effect) { return effect == Effect::Face || effect == Effect::FaceTrack || effect == Effect::Wall; }
struct Behavior {
    std::string name, target, reference;
    Effect effect = Effect::Motion;
    graphics::Motion motion;
    double seconds = 0;
    bool playing = true, enabled = true, released = false;
    bool positionDriver = false, parentSpin = false;
    graphics::FaceRotationConfig face;
    std::size_t faceIndex = 0;
    glm::mat4 initialFaceMatrix{1};
    graphics::FaceAnimationSpace initialFaceSpace = graphics::FaceAnimationSpace::Local;
    bool initialFaceVisible = true;
    glm::mat4 faceTrackBase{1};
    float rate = 1, phase = 0, height = .455f;
    graphics::RoomBody body;
    glm::vec3 gravity{0, -9.8f, 0}, angularVelocity{0};
    bool elastic = false, releaseRequested = false;
    glm::mat3 releasedRotation{1};
    graphics::CharacterBody character;
    std::vector<std::string> obstacles;
    glm::mat4 previousRoom{1};
    std::function<bool(graphics::Scene&, const graphics::Transform3D&, double, float)> custom;
    std::function<bool(Assembly&, int, int, int)> key;
    std::function<bool(Assembly&, const graphics::MouseEvent&)> mouse;
    std::function<bool(Assembly&, double, double)> cursor;
    std::function<void(Assembly&, bool)> focus;
};

struct Binding {
    int key = 0, mods = 0;
    bool hold = false, down = false;
    std::string command;
    Tokens tokens;
};

class Assembly {
    struct State {
        std::map<std::string, Node> nodes;
        std::vector<Behavior> behaviors;
        std::vector<Binding> bindings;
        std::vector<std::string> recipe;
        std::string selected;
        int axis = 0;
        std::string dragTarget;
        glm::vec3 dragAxis{0, 1, 0};
        float dragSensitivity = .35f;
        bool dragging = false;
        std::optional<double> dragX;
        bool paused = false, threeViews = false;
    } state_;
    graphics::Application& app_;
    bool replaying_ = false, faulted_ = false;
    std::uint64_t topologyRevision_ = 0;

    bool dispatch(const Tokens& args, std::ostream& out);
    bool motionCommand(const Tokens& args, std::ostream& out);
    bool objectCommand(const Tokens& args, std::ostream& out);
    bool hierarchyCommand(const Tokens& args, std::ostream& out);
    bool surfaceCommand(const Tokens& args, std::ostream& out);
    bool trackCommand(const Tokens& args, std::ostream& out);
    bool inputCommand(const Tokens& args, std::ostream& out);
    bool applyPoses(float dt);
    bool dispatchInput(const std::function<bool(const Behavior&)>& callback, std::ostream& out);
    void applyMounts();
    void registerNode(const std::string& name, graphics::EntityId id, bool whole,
                      const std::string& kind = "part", std::optional<graphics::EntityId> mesh = std::nullopt);
    void registerPerson(const std::string& prefix, const graphics::HumanoidRig& rig);
    Behavior* positionDriver(const std::string& target);
    bool save(const std::string& path, std::ostream& out) const;
    bool load(const std::string& path, std::ostream& out);
public:
    explicit Assembly(graphics::Application& app) : app_(app) {}
    graphics::Application& application() { return app_; }
    const std::map<std::string, Node>& nodes() const { return state_.nodes; }
    const std::vector<Behavior>& behaviors() const { return state_.behaviors; }
    Node& node(const std::string& name);
    const Node& node(const std::string& name) const;
    void clear();
    void add(const std::string& kind, const std::string& name, glm::vec3 position,
             glm::vec3 size = glm::vec3(1));
    void group(const std::string& name, glm::vec3 position);
    std::string part(const std::string& kind, const std::string& parent, const std::string& localName,
                     glm::vec3 position, glm::vec3 size = glm::vec3(1));
    void srt(const std::string& target, glm::vec3 position, glm::vec3 degrees, float scale);
    void pivot(const std::string& target, glm::vec3 point, bool keep = true);
    glm::vec3 center(const std::string& target) const;
    void attach(Behavior behavior);
    void move(const std::string& target, glm::vec3 delta);
    void rotate(const std::string& target, glm::vec3 axis, float degrees);
    void setRest(const std::string& target, const graphics::Transform3D& rest);
    graphics::EntityId meshEntity(const std::string& target) const;
    void requireGeometryEditable(graphics::EntityId mesh) const;
    void requireFaceEditable(graphics::EntityId mesh, std::size_t face) const;
    void model(const std::string& name, const graphics::Model& model, glm::vec3 position = glm::vec3(0));
    std::string mount(const std::string& parent, const std::string& localName, FaceMount attachment);
    void bind(Binding binding);
    bool execute(const std::string& line, std::ostream& out);
    bool update(float dt, std::ostream& out);
    bool render();
    void onKey(int key, int action, int mods, std::ostream& out);
    void onMouse(const graphics::MouseEvent& event, std::ostream& out);
    void onFocus(bool focused);
    void onCursor(double x, double y, std::ostream& out);
    void guide(const std::string& topic, std::ostream& out) const;
    void help(std::ostream& out) const;
    void inspect(const std::string& target, std::ostream& out) const;
};

Tokens tokenize(const std::string& line);
std::string quote(const std::string& text);
double number(const std::string& text);
std::size_t indexNumber(const std::string& text);
glm::vec3 vector(const Tokens& args, std::size_t offset);
glm::vec3 axis(const std::string& text);
bool boolean(const std::string& text);
void require(bool success, const std::string& message);
bool rigid(const glm::mat4& matrix);
std::optional<bool> customCommand(Assembly& scene, const Tokens& args, std::ostream& out);
bool defaultRecipe(Assembly& scene, std::ostream& out);
} // namespace assembly
````

### after

````cpp
#pragma once
#include "PracticeMode.h"
#include "FeatureContainer.h"
#include <functional>
#include <map>
#include <ostream>

namespace assembly {
using Tokens = std::vector<std::string>;
class Assembly;

struct FaceMount {
    graphics::EntityId mesh = 0;
    std::size_t face = 0, triangle = 0;
    glm::vec3 coordinate{0};
    bool barycentric = true;
};

struct Node {
    graphics::EntityId id = 0;
    std::optional<graphics::EntityId> mesh;
    graphics::Transform3D rest, initial;
    std::string kind;
    bool whole = false;
    std::optional<graphics::Bounds3> room;
    std::optional<graphics::JointRig> walker;
    std::string guideTopic;
    std::optional<FaceMount> mount;
    bool affineGroup = false;
};

enum class Effect { Motion, Face, FaceTrack, Wall, Body, Walk, Conveyor, Custom, Input };
inline bool faceEffect(Effect effect) { return effect == Effect::Face || effect == Effect::FaceTrack || effect == Effect::Wall; }
struct Behavior {
    std::string name, target, reference;
    Effect effect = Effect::Motion;
    graphics::Motion motion;
    double seconds = 0;
    bool playing = true, enabled = true, released = false;
    bool positionDriver = false, parentSpin = false;
    graphics::FaceRotationConfig face;
    std::size_t faceIndex = 0;
    glm::mat4 initialFaceMatrix{1};
    graphics::FaceAnimationSpace initialFaceSpace = graphics::FaceAnimationSpace::Local;
    bool initialFaceVisible = true;
    glm::mat4 faceTrackBase{1};
    float rate = 1, phase = 0, height = .455f;
    graphics::RoomBody body;
    glm::vec3 gravity{0, -9.8f, 0}, angularVelocity{0};
    bool elastic = false, releaseRequested = false;
    glm::mat3 releasedRotation{1};
    graphics::CharacterBody character;
    std::vector<std::string> obstacles;
    glm::mat4 previousRoom{1};
    std::function<bool(graphics::Scene&, const graphics::Transform3D&, double, float)> custom;
    std::function<bool(Assembly&, int, int, int)> key;
    std::function<bool(Assembly&, const graphics::MouseEvent&)> mouse;
    std::function<bool(Assembly&, double, double)> cursor;
    std::function<void(Assembly&, bool)> focus;
};

struct Binding {
    int key = 0, mods = 0;
    bool hold = false, down = false;
    std::string command;
    Tokens tokens;
};

class Assembly {
    struct State {
        std::map<std::string, Node> nodes;
        std::vector<Behavior> behaviors;
        std::vector<Binding> bindings;
        std::vector<FeatureInstance> features;
        std::string featureFocus, featureCamera;
        std::string reloadPath = "examples/assembly/workspace.assembly";
        std::vector<std::string> recipe;
        std::string selected;
        int axis = 0;
        std::string dragTarget;
        glm::vec3 dragAxis{0, 1, 0};
        float dragSensitivity = .35f;
        bool dragging = false;
        std::optional<double> dragX;
        bool paused = false, threeViews = false;
    } state_;
    graphics::Application& app_;
    bool replaying_ = false, faulted_ = false;
    std::uint64_t topologyRevision_ = 0;

    bool dispatch(const Tokens& args, std::ostream& out);
    bool motionCommand(const Tokens& args, std::ostream& out);
    bool objectCommand(const Tokens& args, std::ostream& out);
    bool hierarchyCommand(const Tokens& args, std::ostream& out);
    bool surfaceCommand(const Tokens& args, std::ostream& out);
    bool trackCommand(const Tokens& args, std::ostream& out);
    bool featureCommand(const Tokens& args, std::ostream& out);
    void advanceFeatures(float dt);
    void syncFeatures();
    void featureAppearance(graphics::EntityId id, std::optional<graphics::GlColor> color, std::optional<bool> visible);
    bool routeFeatureKey(int key, int action, int mods, std::ostream& out);
    bool routeFeatureMouse(const graphics::MouseEvent& event, std::ostream& out);
    bool routeFeatureCursor(double x, double y);
    void queueFeatureFocus(bool value);
    void queueFeatureEvent(FeatureInstance& feature, FeatureEvent event);
    bool inputCommand(const Tokens& args, std::ostream& out);
    bool applyPoses(float dt);
    bool dispatchInput(const std::function<bool(const Behavior&)>& callback, std::ostream& out);
    void applyMounts();
    void registerNode(const std::string& name, graphics::EntityId id, bool whole,
                      const std::string& kind = "part", std::optional<graphics::EntityId> mesh = std::nullopt);
    void registerPerson(const std::string& prefix, const graphics::HumanoidRig& rig);
    Behavior* positionDriver(const std::string& target);
    bool save(const std::string& path, std::ostream& out) const;
    bool load(const std::string& path, std::ostream& out);
public:
    explicit Assembly(graphics::Application& app) : app_(app) {}
    graphics::Application& application() { return app_; }
    const std::map<std::string, Node>& nodes() const { return state_.nodes; }
    const std::vector<Behavior>& behaviors() const { return state_.behaviors; }
    Node& node(const std::string& name);
    const Node& node(const std::string& name) const;
    void clear();
    void addFeature(const std::string& name, int mode);
    void portFeature(const std::string& source, graphics::EntityId native, const std::string& name);
    void add(const std::string& kind, const std::string& name, glm::vec3 position,
             glm::vec3 size = glm::vec3(1));
    void group(const std::string& name, glm::vec3 position);
    std::string part(const std::string& kind, const std::string& parent, const std::string& localName,
                     glm::vec3 position, glm::vec3 size = glm::vec3(1));
    void srt(const std::string& target, glm::vec3 position, glm::vec3 degrees, float scale);
    void pivot(const std::string& target, glm::vec3 point, bool keep = true);
    glm::vec3 center(const std::string& target) const;
    void attach(Behavior behavior);
    void move(const std::string& target, glm::vec3 delta);
    void rotate(const std::string& target, glm::vec3 axis, float degrees);
    void setRest(const std::string& target, const graphics::Transform3D& rest);
    graphics::EntityId meshEntity(const std::string& target) const;
    void requireGeometryEditable(graphics::EntityId mesh) const;
    void requireFeatureEditable(graphics::EntityId mesh) const;
    void requireFaceEditable(graphics::EntityId mesh, std::size_t face) const;
    void model(const std::string& name, const graphics::Model& model, glm::vec3 position = glm::vec3(0));
    std::string mount(const std::string& parent, const std::string& localName, FaceMount attachment);
    void bind(Binding binding);
    bool execute(const std::string& line, std::ostream& out);
    bool update(float dt, std::ostream& out);
    bool render();
    void onKey(int key, int action, int mods, std::ostream& out);
    void onMouse(const graphics::MouseEvent& event, std::ostream& out);
    void onFocus(bool focused);
    void onCursor(double x, double y, std::ostream& out);
    bool capturesNumericKeys() const { return !state_.featureFocus.empty(); }
    void guide(const std::string& topic, std::ostream& out) const;
    void help(std::ostream& out) const;
    void inspect(const std::string& target, std::ostream& out) const;
    void diagnose(const std::string& target, std::ostream& out) const;
};

Tokens tokenize(const std::string& line);
std::string quote(const std::string& text);
double number(const std::string& text);
std::size_t indexNumber(const std::string& text);
std::pair<int, int> keyCode(std::string text, bool modeKeys = false);
glm::vec3 vector(const Tokens& args, std::size_t offset);
glm::vec3 axis(const std::string& text);
bool boolean(const std::string& text);
void require(bool success, const std::string& message);
bool rigid(const glm::mat4& matrix);
std::optional<bool> customCommand(Assembly& scene, const Tokens& args, std::ostream& out);
bool defaultRecipe(Assembly& scene, std::ostream& out);
} // namespace assembly
````

## standard/modes/AssemblyCommands.cpp

feature/diagnose/hotload/reload 명령과 레시피 재귀 제한을 연결한다.

### before

````cpp
#include "Assembly.h"
#include <algorithm>

namespace assembly {
bool Assembly::objectCommand(const Tokens& a, std::ostream&) {
    const auto& command = a[0];
    if (command == "add") {
        require(a.size() == 6 || a.size() == 9, "add <종류> <이름> <X> <Y> <Z> [크기X 크기Y 크기Z]");
        add(a[1], a[2], vector(a, 3), a.size() == 9 ? vector(a, 6) : glm::vec3(1));
        return true;
    }
    if (command == "parent") {
        require(a.size() == 3, "parent <자식> <부모 또는 none>. 좌표는 부모 로컬 값으로 유지합니다.");
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
        require(app_.getScene().setParent(child.id, parent), "부모 연결 실패. 순환 여부를 확인하세요.");
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
            if (ancestor)
                require(command == "color" ? scene.setTint(id, {color.x, color.y, color.z}) : scene.setVisible(id, visible), "색/가시성 적용 실패.");
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
            require(a.size() == 3, "scale <대상> <양수 균일 배율>");
            rest.scale = glm::vec3(float(number(a[2])));
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
    if (a[0] == "inspect" || a[0] == "select") {
        require(a.size() == 2, a[0] + " <객체/부품 이름>");
        inspect(a[1], out);
        if (a[0] == "select") state_.selected = a[1];
        return true;
    }
    if (a[0] == "clear") {
        require(a.size() == 1, "clear");
        clear();
        return true;
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
````

### after

````cpp
#include "Assembly.h"
#include <algorithm>

namespace assembly {
bool Assembly::objectCommand(const Tokens& a, std::ostream&) {
    const auto& command = a[0];
    if (command == "add") {
        require(a.size() == 6 || a.size() == 9, "add <종류> <이름> <X> <Y> <Z> [크기X 크기Y 크기Z]");
        add(a[1], a[2], vector(a, 3), a.size() == 9 ? vector(a, 6) : glm::vec3(1));
        return true;
    }
    if (command == "parent") {
        require(a.size() == 3, "parent <자식> <부모 또는 none>. 좌표는 부모 로컬 값으로 유지합니다.");
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
        require(app_.getScene().setParent(child.id, parent), "부모 연결 실패. 순환 여부를 확인하세요.");
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
                if (command == "color") featureAppearance(id, graphics::GlColor{color.x, color.y, color.z}, {});
                else featureAppearance(id, {}, visible);
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
            require(a.size() == 3, "scale <대상> <양수 균일 배율>");
            rest.scale = glm::vec3(float(number(a[2])));
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
        if (a[0] == "select") state_.selected = a[1];
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
    if (featureCommand(a, out) || surfaceCommand(a, out) || trackCommand(a, out) || hierarchyCommand(a, out) || objectCommand(a, out) || motionCommand(a, out) || inputCommand(a, out))
        return true;
    if (const auto handled = customCommand(*this, a, out))
        return *handled;
    return false;
}
} // namespace assembly
````

## standard/modes/AssemblyDiagnostics.cpp

기준 SRT·부모·면·행렬 determinant·기능/카메라 상태를 출력한다.

### before

없음.

### after

````cpp
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
    for (const auto& feature : state_.features) {
        bool member = feature.name == target;
        for (const auto& part : feature.entities) member = member || part.second.group == object.id || part.second.mesh == object.id;
        if (member) out << "feature owner=" << feature.name << " source mode=" << feature.program->mode()
            << " playing=" << feature.playing << " enabled=" << feature.enabled << " focus=" << (state_.featureFocus == feature.name)
            << " geometry-owner=native; authoring=extra SRT/face motion\n";
    }
    const auto size = app_.getFramebufferSize();
    out << "framebuffer=" << size[0] << 'x' << size[1] << " views=" << (state_.threeViews ? 3 : 1)
        << " paused=" << state_.paused << " faulted=" << faulted_ << '\n';
    out << "1) pause on / feature play <id> off 2) surface <mesh> <face> 3) guide <feature>\n"
        << "4) inspect rest/pivot/parent/determinant; compare private controller vs extra authoring pose\n"
        << "5) recipe edit: reload / Ctrl+F5; C++ edit: rebuild by user, then recreate.\n";
}
} // namespace assembly
````

## standard/modes/AssemblyFeatureInput.cpp

원본 입력을 큐에 넣고 화면 피킹 ID를 native ID로 역매핑한다.

### before

없음.

### after

````cpp
#include "Assembly.h"
#include <algorithm>

namespace assembly {
void Assembly::queueFeatureEvent(FeatureInstance& feature, FeatureEvent event) {
    if (feature.events.size() >= 256) {
        feature.events.clear();
        FeatureEvent release; release.kind = FeatureEvent::Kind::Focus; release.focused = false;
        feature.events.push_back(release);
        return;
    }
    feature.events.push_back(std::move(event));
}
void Assembly::queueFeatureFocus(bool value) {
    for (auto& feature : state_.features) if (feature.name == state_.featureFocus) {
        FeatureEvent event; event.kind = FeatureEvent::Kind::Focus; event.focused = value;
        queueFeatureEvent(feature, event);
    }
}
bool Assembly::routeFeatureKey(int key, int action, int mods, std::ostream&) {
    for (auto& f : state_.features) if (f.name == state_.featureFocus && f.enabled) {
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) return true;
        if (key == GLFW_KEY_V && f.program->mode() <= 9) {
            if (action == GLFW_PRESS) state_.threeViews = !state_.threeViews;
            return true;
        }
        FeatureEvent event; event.kind = FeatureEvent::Kind::Key;
        event.key = key; event.action = action; event.mods = mods;
        queueFeatureEvent(f, event);
        return true;
    }
    return false;
}
bool Assembly::routeFeatureMouse(const graphics::MouseEvent& original, std::ostream& out) {
    for (auto& f : state_.features) if (f.name == state_.featureFocus && f.enabled) {
        try {
            if (f.program->mode() <= 9 && original.button == GLFW_MOUSE_BUTTON_LEFT && original.action == GLFW_PRESS) {
                std::optional<graphics::PickResult> hit;
                const auto size = app_.getFramebufferSize();
                if (state_.threeViews && size[0] >= 3 && size[1] >= 2) {
                    for (const auto& pass : graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions())) {
                        const auto& v = pass.viewport;
                        const double top = size[1] - v.y - v.height;
                        if (original.framebufferX >= v.x && original.framebufferX < v.x + v.width && original.framebufferY >= top && original.framebufferY < top + v.height)
                            hit = graphics::pick(app_.getScene(), pass.camera, original.framebufferX, original.framebufferY,
                                {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
                    }
                } else hit = app_.pickAt(original.windowX, original.windowY);
                FeatureEvent pending; pending.kind = FeatureEvent::Kind::Pick;
                if (hit) for (const auto& instance : state_.features) if (instance.program == f.program && instance.enabled)
                    for (const auto& entity : instance.entities) if (entity.second.mesh == hit->entityId) {
                        pending.hit = *hit; pending.hit->entityId = entity.first;
                    }
                queueFeatureEvent(f, pending);
                return true;
            }
            FeatureEvent pending; pending.kind = FeatureEvent::Kind::Mouse;
            pending.mouse = original; pending.view = f.program->camera();
            queueFeatureEvent(f, pending);
        } catch (const std::exception& error) { out << "기능 입력 거절: " << error.what() << '\n'; }
        return true;
    }
    return false;
}
bool Assembly::routeFeatureCursor(double x, double y) {
    for (auto& f : state_.features) if (f.name == state_.featureFocus && f.enabled) {
        FeatureEvent event; event.kind = FeatureEvent::Kind::Cursor; event.x = x; event.y = y;
        queueFeatureEvent(f, event); return true;
    }
    return false;
}
} // namespace assembly
````

## standard/modes/AssemblyFeatures.cpp

독립 실습/공유 포트/정지 복사·시간·원본 출력과 추가 편집을 조립한다.

### before

없음.

### after

````cpp
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
    require(state_.features.size() < 64 && mode >= 1 && mode <= lastStandardMode, "기능은 모드1~22, 인스턴스64개 이하여야 합니다.");
    group(name, {0, 0, 0});
    node(name).guideTopic = "mode" + std::to_string(mode);
    FeatureInstance feature;
    feature.name = name; feature.program = std::make_shared<FeatureContainer>(app_, mode);
    state_.features.push_back(std::move(feature));
}

void Assembly::portFeature(const std::string& source, graphics::EntityId native, const std::string& name) {
    require(state_.features.size() < 64, "기능 인스턴스64개 한도입니다.");
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
        require(a.size() == 4 && state_.features.size() < 64, "feature add <이름> <모드1~22>. 컨테이너 한도64개.");
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
        require(a.size() == 5 && state_.features.size() < 64, "feature port <기존이름> <native엔티티ID> <새이름>");
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
````

## standard/modes/AssemblyGuide.cpp

현재 소스 줄·변경 전후 예·이유·제약·디버깅 단계·예상 결과를 출력한다.

### before

````cpp
#include "Assembly.h"
#include <fstream>

namespace assembly {
namespace {
struct Location { const char* file; const char* marker; const char* edit; };

std::filesystem::path projectRoot() {
    std::wstring binary(32768, L'\0');
    const DWORD count = GetModuleFileNameW(nullptr, binary.data(), DWORD(binary.size()));
    if (count > 0 && count < binary.size()) {
        binary.resize(count);
        auto path = std::filesystem::path(binary).parent_path();
        for (int i = 0; i < 6 && !path.empty(); ++i, path = path.parent_path())
            if (std::filesystem::is_regular_file(path / "modes/Assembly.h")) return path;
    }
    const auto source = std::filesystem::u8path(__FILE__).parent_path().parent_path();
    if (std::filesystem::is_regular_file(source / "modes/Assembly.h")) return source;
    return {};
}

void show(const Location& location, std::ostream& out) {
    const auto root = projectRoot();
    const auto path = root.empty() ? std::filesystem::u8path(location.file) : root / location.file;
    out << "문서: " << (root.empty() ? std::filesystem::path("docs/assembly/CODE_GUIDE.md") : root / "docs/assembly/CODE_GUIDE.md").u8string() << '\n';
    std::ifstream source(path, std::ios::binary);
    std::string line;
    std::size_t number = 0;
    bool found = false;
    while (std::getline(source, line)) {
        ++number;
        if (line.find(location.marker) != std::string::npos) {
            out << path.u8string() << ':' << number << "\n  " << line << '\n';
            found = true;
            break;
        }
    }
    if (!found) out << path.u8string() << " : 소스/표식을 찾지 못해 현재 줄 번호를 확인할 수 없습니다.\n";
    out << "응용 조합: docs/assembly/PATTERN_COMBINATIONS.md\n";
    out << "수정 항목: " << location.edit << "\n레시피는 다시 load하고, C++ 수정 후 빌드·실행은 사용자가 수행하세요.\n";
}
}

void Assembly::help(std::ostream& out) const {
    out << "0번 조립 콘솔. 명세: docs/assembly/SPEC.md, 사용법: docs/assembly/USER_GUIDE.md\n"
        << "build tank/truck/person <이름> : C++ 예제 생성. 확장은 guide tank/truck/person으로 직접 편집\n"
        << "surface <객체> <면/all> | mount <객체> <관절> 면 삼각형 U V 거리 | anchor <객체> <관절> 면 XYZ\n"
        << "normal <객체> <면/all> XYZ 또는 auto/flip | winding <객체> <면/all> flip | warp <group> SX SY SZ\n"
        << "face-track <id> <객체> 면 시간 지연 once/loop/pingpong move/scale/turn ... | guide face-track\n"
        << "group <이름> X Y Z | part box <부모> <부품> 위치3 크기3 | joint <부모> <관절> 위치3\n"
        << "srt <그룹> 위치3 회전도3 균일배율 | pivot <그룹> center 또는 위치3 [keep/raw]\n"
        << "add <종류> <이름> X Y Z [크기3] | list | inspect <이름> | select <이름>\n"
        << "move/position <이름> X Y Z | rotate <이름> x/y/z 도 | scale <이름> 값 | parent <자식> <부모/none>\n"
        << "spin <id> <대상> 축 도/초 | path <id> <대상> 1~6 초 | pendulum <id> <대상> 축 진폭 초\n"
        << "linear <id> <대상> DX DY DZ 초 [once/loop/pingpong] | orbit <id> <대상> 축 도/초 중심3\n"
        << "face <id> <대상> 면 spin/orbit 축 도/초 또는 face <id> <대상> 면 hinge 모서리 도/초 | wall <id> <방> 면0~3 초\n"
        << "physics <id> <상자> <방> | walk <id> <walker> <방> 속도 | jump <보행id> 속도\n"
        << "bind RIGHT hold \"move hero 1 0 0\" | bind J press \"jump walking 5.5\" | unbind RIGHT\n"
        << "play/enable <id> on/off | seek <id> 초 | remove <id> | pause on/off | view 1/3\n"
        << "camera ... | render wire/lighting/depth on/off | save/load \"파일.assembly\" | clear\n"
        << "guide mount/normal/warp/face-track/cpp-effect/practice13~23 또는 guide <객체이름>, guide feature:<기능>\n"
        << "창: 클릭 선택, Tab/Backspace 부품, XYZ 축, 방향키 편집, Space 정지, Home 기본 레시피 복원\n";
}

void Assembly::guide(const std::string& requested, std::ostream& out) const {
    const bool feature = requested.compare(0, 8, "feature:") == 0;
    std::string topic = feature ? requested.substr(8) : requested;
    if (const auto found = state_.nodes.find(topic); !feature && found != state_.nodes.end()) {
        inspect(topic, out);
        std::optional<graphics::EntityId> id = found->second.id;
        topic = "objects";
        while (id) {
            bool matched = false;
            for (const auto& item : state_.nodes) {
                if (item.second.id == *id && item.second.whole) {
                    const auto& kind = item.second.kind;
                    if (!item.second.guideTopic.empty()) { topic = item.second.guideTopic; matched = true; break; }
                    if (kind == "humanoid" || kind == "swing" || kind == "tank" || kind == "walker" || kind == "grid" || kind == "room" || kind == "group" || kind == "joint")
                        topic = kind;
                    matched = true;
                    break;
                }
            }
            if (matched) break;
            const auto* component = app_.getScene().getComponent(*id);
            id = component ? component->parent : std::nullopt;
        }
    }
    const std::map<std::string, Location> locations{
        {"mount", {"modes/AssemblySurface.cpp", "std::string Assembly::mount(", "객체·면·삼각형 좌표 관절 부착과 프레임 추적"}},
        {"normal", {"modes/AssemblySurface.cpp", "if (command == \"normal\")", "조명 노멀·winding·면별 정점 데이터"}},
        {"geometry", {"modes/AssemblySurface.cpp", "void Assembly::model(", "사용자 Model 등록, 삼각형·정점·면 변환"}},
        {"warp", {"modes/AssemblySurface.cpp", "if (command == \"warp\")", "외곽 group의 비균일 배율과 반사"}},
        {"face-track", {"modes/AssemblyTracks.cpp", "bool Assembly::trackCommand(", "지연·반복·왕복 면 이동/축소/임의 축 회전"}},
        {"cpp-effect", {"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Custom)", "사용자 C++ 프레임 동작과 기준 SRT 합성"}},
        {"objects", {"modes/AssemblyObjects.cpp", "void Assembly::add(", "객체 종류 분기와 크기·색·부품 등록"}},
        {"humanoid", {"framework/detail/Animation.cpp", "std::optional<HumanoidRig> AnimationSystem::addHumanoid", "사람 비율·관절 연결점·머리/팔/다리 생성"}},
        {"swing", {"framework/detail/Animation.cpp", "std::optional<SwingRig> AnimationSystem::addSwingWithRider", "그네 지지대·줄·좌석·탑승자 연결"}},
        {"tank", {"modes/AssemblyExamples.cpp", "void buildTankExample(", "육면체 기반 차체·궤도·포탑·포신의 계층과 동작"}},
        {"tank-preset", {"modes/AssemblyParts.cpp", "TankParts addTankParts(", "기존 실습19·22와 공유하는 탱크 부품"}},
        {"walker", {"framework/detail/Patterns.cpp", "std::optional<JointRig> PatternSystem::addWalkingRig", "보행 리그의 몸체 비율·관절 연결점"}},
        {"grid", {"modes/AssemblyParts.cpp", "std::vector<graphics::EntityId> addGridParts(", "셀 배치·크기 비율·교대 색"}},
        {"room", {"modes/AssemblyObjects.cpp", "} else if (kind == \"room\")", "방 경계와 벽 메시 생성"}},
        {"hierarchy", {"modes/AssemblyHierarchy.cpp", "void Assembly::group(", "group/part/joint 조립 API와 최상위 변환"}},
        {"srt", {"modes/AssemblyHierarchy.cpp", "void Assembly::srt(", "최상위 위치·회전·균일배율을 한 번에 설정"}},
        {"pivot", {"modes/AssemblyHierarchy.cpp", "void Assembly::pivot(", "중심점 변경과 기준 배치 보존"}},
        {"center", {"modes/AssemblyHierarchy.cpp", "glm::vec3 Assembly::center(", "하위 메시 전체의 로컬 AABB 중심 계산"}},
        {"patterns", {"docs/assembly/PATTERN_COMBINATIONS.md", "# 기능 조합 패턴", "탱크·트럭·사람의 계층 조립과 C++ 수정 위치"}},
        {"truck", {"modes/AssemblyExamples.cpp", "void buildTruckExample(", "차체·바퀴·컨테이너·문 관절 조합"}},
        {"person", {"modes/AssemblyExamples.cpp", "void buildPersonExample(", "육면체·관절로 몸체·팔·다리 조합"}},
        {"motion", {"modes/AssemblyMotion.cpp", "bool Assembly::motionCommand(", "경로·회전·진자·면 동작 설정"}},
        {"face", {"modes/AssemblyMotion.cpp", "} else if (command == \"face\"", "힌지·자전·공전 종류, 면/모서리, 속도"}},
        {"physics", {"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Body)", "중력·충돌 경계·이탈/보행 계산"}},
        {"input", {"modes/AssemblyInput.cpp", "bool Assembly::inputCommand(", "키 이름·수정키·press/hold 문법과 동작"}},
        {"camera", {"modes/AssemblyInput.cpp", "if (a[1] == \"pose\")", "카메라 이동·투영·시야각과 줌"}},
        {"recipe", {"modes/Mode0Recipe.cpp", "USER_RECIPE_BEGIN", "기본 조립 명령 문자열을 추가·수정"}},
        {"custom", {"modes/AssemblyExtensions.cpp", "USER_COMMAND_BEGIN", "새 명령 분기와 typed Assembly API 호출"}},
        {"runtime", {"modes/AssemblyRuntime.cpp", "bool Assembly::applyPoses(", "프레임별 합성 순서와 기능 업데이트"}},
        {"save", {"modes/AssemblyPersistence.cpp", "bool Assembly::save(", "레시피 기록과 기준 SRT 저장 형식"}}};
    if (topic == "anchor" || topic == "unmount") topic = "mount";
    if (topic == "winding" || topic == "orient") topic = "normal";
    if (topic == "triangle" || topic == "triangle-add" || topic == "surface" || topic == "vertex" || topic == "face-srt" || topic == "face-mode" || topic == "facecolor" || topic == "outline" || topic == "line" || topic == "digit") topic = "geometry";
    if (topic == "effects" || topic == "render") topic = "input";
    if (topic.size() == 10 && topic.compare(0, 8, "practice") == 0) {
        const std::map<std::string, Location> practices{
            {"practice13", {"modes/Mode10Practice13.cpp", "bool Practice13::initScene()", "면 선택·무작위 선택 상태를 customCommand/Custom 동작으로 옮기기"}},
            {"practice14", {"modes/Mode11Practice14.cpp", "void Practice14::onUpdate(", "입력 상태·형상 교체·로컬 회전"}},
            {"practice15", {"modes/FaceCommon.cpp", "void FaceMode::applyFaceMotion()", "한 면의 복합 행렬을 하나의 FaceTrack Motion에 조립"}},
            {"practice16", {"modes/Mode13Practice16.cpp", "void Practice16::applyPyramid(", "순차 펼침과 다중 면 동기화"}},
            {"practice17", {"modes/Mode14Practice17.cpp", "void Practice17::onUpdate(", "교환·우회·사인 배율·조건 전이"}},
            {"practice18", {"modes/Mode15Practice18.cpp", "void Practice18::onUpdate(", "기울어진 궤도·달 중심·축과 반지름"}},
            {"practice19", {"modes/TankCommon.cpp", "void TankMode::onUpdate(", "탱크 부품·이동 상태·카메라 추적"}},
            {"practice20", {"modes/Mode17Practice20.cpp", "void Practice20::onUpdate(", "거리 기반 컨베이어·택배 간격·접선 자세"}},
            {"practice21", {"modes/Mode18Practice21.cpp", "void Practice21::onUpdate(", "드래그 방 회전·다중 상자/공 이탈 상태"}},
            {"practice22", {"modes/TankCommon.cpp", "bool TankMode::onRender()", "동일 상태의 3뷰 렌더와 카메라"}},
            {"practice23", {"modes/Mode20Practice23.cpp", "void Practice23::onUpdate(", "보행 충돌·장애물 난수 배치·벽 개방 상태"}}};
        if (const auto found = practices.find(topic); found != practices.end()) { show(found->second, out); return; }
    }
    if (topic == "truck-pattern") topic = "truck";
    if (topic == "tank-pattern") topic = "tank";
    if (topic == "person-pattern") topic = "person";
    if (topic == "group" || topic == "joint" || topic == "part") topic = "hierarchy";
    if (topic == "add" || topic == "parent" || topic == "color" || topic == "show") topic = "objects";
    if (topic == "spin" || topic == "orbit" || topic == "path" || topic == "linear" || topic == "pendulum" || topic == "remove") topic = "motion";
    if (topic == "wall") topic = "face";
    if (topic == "walk" || topic == "jump" || topic == "obstacle" || topic == "body" || topic == "release") topic = "physics";
    if (topic == "bind" || topic == "unbind" || topic == "drag") topic = "input";
    if (topic == "load") topic = "save";
    const auto found = locations.find(topic);
    if (found == locations.end()) {
        out << "지원 항목:";
        for (const auto& item : locations) out << ' ' << item.first;
        out << " practice13~23\n";
        return;
    }
    show(found->second, out);
}
} // namespace assembly
````

### after

````cpp
#include "Assembly.h"
#include "GuideRecipes.h"
#include <algorithm>
#include <fstream>

namespace assembly {
namespace {
std::filesystem::path projectRoot() {
    std::wstring binary(32768, L'\0');
    const DWORD count = GetModuleFileNameW(nullptr, binary.data(), DWORD(binary.size()));
    if (count > 0 && count < binary.size()) {
        binary.resize(count);
        auto path = std::filesystem::path(binary).parent_path();
        for (int i = 0; i < 6 && !path.empty(); ++i, path = path.parent_path())
            if (std::filesystem::is_regular_file(path / "modes/Assembly.h")) return path;
    }
    const auto source = std::filesystem::u8path(__FILE__).parent_path().parent_path();
    if (std::filesystem::is_regular_file(source / "modes/Assembly.h")) return source;
    return {};
}

void showSite(const GuideSite& site, std::ostream& out) {
    const auto root = projectRoot();
    const auto path = root.empty() ? std::filesystem::u8path(site.file) : root / site.file;
    std::ifstream input(path, std::ios::binary);
    if (!input) { out << path.u8string() << " : 소스가 없어 현재 줄 확인 불가. 표식=" << site.marker << '\n'; return; }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (lines.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
        lines.push_back(line);
    }
    out << "편집 위치: " << site.purpose << '\n';
    std::size_t matches = 0;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find(site.marker) == std::string::npos) continue;
        ++matches;
        if (matches > 4) continue;
        out << path.u8string() << ':' << i + 1 << '\n';
        const auto begin = i > 1 ? i - 2 : 0;
        const auto end = std::min(lines.size(), i + 4);
        for (auto n = begin; n < end; ++n) out << (n == i ? " > " : "   ") << n + 1 << " | " << lines[n] << '\n';
    }
    if (!matches) out << path.u8string() << " : 표식이 바뀌었습니다. 현재 줄을 추정하지 않습니다. 검색어=" << site.marker << '\n';
    if (matches > 1) out << "동일 표식 " << matches << "곳. 함수/부모 문맥을 비교하세요(최대4곳 표시).\n";
}
void showRecipe(const GuideRecipe& recipe, std::ostream& out) {
    out << "guide " << recipe.topic << ": " << recipe.goal << '\n';
    for (const auto& site : recipe.sites) showSite(site, out);
    out << "\n변경 예시 before (작성 기준값; 현재 코드는 위 출력 우선):\n" << recipe.before
        << "\nafter 예시:\n" << recipe.after << "\n변경 이유/단위/제약:\n" << recipe.reason
        << "\n디버깅-수정 순서:\n" << recipe.debug << "\n예상 관찰:\n" << recipe.expected << '\n';
    const auto root = projectRoot();
    out << "참고: " << (root.empty() ? std::filesystem::u8path(recipe.document) : root / recipe.document).u8string() << '\n'
        << "예시는 자동 패치가 아닙니다. 명령 예시는 콘솔/.assembly에서 실행합니다. C++ 예시는 표시한 함수/삽입 문맥에 맞춰 편집합니다.\nC++ 수정 후 빌드·실행 확인은 사용자가 수행합니다.\n";
}
}

void Assembly::help(std::ostream& out) const {
    out << "0번 조립 콘솔. 명세: docs/assembly/SPEC.md, 사용법: docs/assembly/USER_GUIDE.md\n"
        << "build tank/truck/person <이름> : C++ 예제 생성. 확장은 guide tank/truck/person으로 직접 편집\n"
        << "surface <객체> <면/all> | mount <객체> <관절> 면 삼각형 U V 거리 | anchor <객체> <관절> 면 XYZ\n"
        << "normal <객체> <면/all> XYZ 또는 auto/flip | winding <객체> <면/all> flip | warp <group> SX SY SZ\n"
        << "face-track <id> <객체> 면 시간 지연 once/loop/pingpong move/scale/turn ... | guide face-track\n"
        << "feature add <이름> 1~22 | feature parts <이름> | feature port <원본> <nativeID> <이름> | feature snapshot ...\n"
        << "feature focus/camera <이름/none> | feature play/enable <이름> on/off | Ctrl+F5 / hotload <파일> / reload\n"
        << "diagnose <객체> | guide <기능> : 현재 줄·변경 전후 예·이유·디버깅 단계\n"
        << "group <이름> X Y Z | part box <부모> <부품> 위치3 크기3 | joint <부모> <관절> 위치3\n"
        << "srt <그룹> 위치3 회전도3 균일배율 | pivot <그룹> center 또는 위치3 [keep/raw]\n"
        << "add <종류> <이름> X Y Z [크기3] | list | inspect <이름> | select <이름>\n"
        << "move/position <이름> X Y Z | rotate <이름> x/y/z 도 | scale <이름> 값 | parent <자식> <부모/none>\n"
        << "spin <id> <대상> 축 도/초 | path <id> <대상> 1~6 초 | pendulum <id> <대상> 축 진폭 초\n"
        << "linear <id> <대상> DX DY DZ 초 [once/loop/pingpong] | orbit <id> <대상> 축 도/초 중심3\n"
        << "face <id> <대상> 면 spin/orbit 축 도/초 또는 face <id> <대상> 면 hinge 모서리 도/초 | wall <id> <방> 면0~3 초\n"
        << "physics <id> <상자> <방> | walk <id> <walker> <방> 속도 | jump <보행id> 속도\n"
        << "bind RIGHT hold \"move hero 1 0 0\" | bind J press \"jump walking 5.5\" | unbind RIGHT\n"
        << "play/enable <id> on/off | seek <id> 초 | remove <id> | pause on/off | view 1/3\n"
        << "camera ... | render wire/lighting/depth on/off | save/load \"파일.assembly\" | clear\n"
        << "guide mount/normal/warp/face-track/cpp-effect/practice13~23 또는 guide <객체이름>, guide feature:<기능>\n"
        << "창: 클릭 선택, Tab/Backspace 부품, XYZ 축, 방향키 편집, Space 정지, Home 기본 레시피 복원\n";
}


void Assembly::guide(const std::string& requested, std::ostream& out) const {
    const bool feature = requested.compare(0, 8, "feature:") == 0;
    std::string topic = feature ? requested.substr(8) : requested;
    if (const auto found = state_.nodes.find(topic); !feature && found != state_.nodes.end()) {
        inspect(topic, out);
        std::optional<graphics::EntityId> id = found->second.id;
        topic = "objects";
        while (id) {
            bool matched = false;
            for (const auto& item : state_.nodes) if (item.second.id == *id && item.second.whole) {
                topic = !item.second.guideTopic.empty() ? item.second.guideTopic :
                    findGuideRecipe(item.second.kind) ? item.second.kind : "objects";
                matched = true; break;
            }
            if (matched) break;
            const auto* component = app_.getScene().getComponent(*id);
            id = component ? component->parent : std::nullopt;
        }
    }
    if (const auto* recipe = findGuideRecipe(topic)) { showRecipe(*recipe, out); return; }
    out << "지원 항목:";
    for (const auto& recipe : guideRecipes()) out << ' ' << recipe.topic;
    out << "\n객체 이름과 충돌하면 guide feature:<기능>을 사용하세요.\n";
}
} // namespace assembly
````

## standard/modes/AssemblyInput.cpp

공통 명령 키 해석을 재사용하고 컨테이너 입력 소유권을 연결한다.

### before

````cpp
#include "Assembly.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

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
        require(a.size() >= 3, "render <wire/lighting/depth/cull/light-direction/light-color/background> ...");
        auto& options = app_.getRenderOptions();
        if (a[1] == "cull") {
            require(a.size() == 3 && (a[2] == "none" || a[2] == "back" || a[2] == "front"), "render cull none/back/front");
            options.cull = a[2] == "none" ? graphics::CullMode::None : a[2] == "back" ? graphics::CullMode::Back : graphics::CullMode::Front;
            return true;
        }
        if (a[1] == "light-direction" || a[1] == "light-color" || a[1] == "background") {
            require(a.size() == 5, "render light-direction/light-color/background <값3>");
            const auto value = vector(a, 2);
            if (a[1] == "light-direction") {
                require(glm::length(value) > .0001f, "광원 방향은 0일 수 없습니다.");
                options.lightDirection = glm::normalize(value);
            } else {
                require(glm::all(glm::greaterThanEqual(value, glm::vec3(0))) && glm::all(glm::lessThanEqual(value, glm::vec3(1))), "색은 0~1입니다.");
                if (a[1] == "background") options.background = glm::vec4(value, 1);
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
    if (!focused) {
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
            if (item.second.id == *entity && item.second.whole &&
                (state_.selected.empty() || item.second.kind == "group"))
                state_.selected = item.first;
        const auto* component = app_.getScene().getComponent(*entity);
        entity = component ? component->parent : std::nullopt;
    }
    if (!state_.selected.empty()) {
        inspect(state_.selected, out);
        return;
    }
    out << "선택 해제\n";
}
void Assembly::onCursor(double x, double y, std::ostream& out) {
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
````

### after

````cpp
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
        require(a.size() >= 3, "render <wire/lighting/depth/cull/light-direction/light-color/background> ...");
        auto& options = app_.getRenderOptions();
        if (a[1] == "cull") {
            require(a.size() == 3 && (a[2] == "none" || a[2] == "back" || a[2] == "front"), "render cull none/back/front");
            options.cull = a[2] == "none" ? graphics::CullMode::None : a[2] == "back" ? graphics::CullMode::Back : graphics::CullMode::Front;
            return true;
        }
        if (a[1] == "light-direction" || a[1] == "light-color" || a[1] == "background") {
            require(a.size() == 5, "render light-direction/light-color/background <값3>");
            const auto value = vector(a, 2);
            if (a[1] == "light-direction") {
                require(glm::length(value) > .0001f, "광원 방향은 0일 수 없습니다.");
                options.lightDirection = glm::normalize(value);
            } else {
                require(glm::all(glm::greaterThanEqual(value, glm::vec3(0))) && glm::all(glm::lessThanEqual(value, glm::vec3(1))), "색은 0~1입니다.");
                if (a[1] == "background") options.background = glm::vec4(value, 1);
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
    if (routeFeatureKey(key, action, mods, out)) return;
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
    queueFeatureFocus(focused);
    if (!focused) {
        for (auto& binding : state_.bindings) binding.down = false;
        state_.dragging = false; state_.dragX.reset();
    }
    dispatchInput([&](const Behavior& b) { if (b.focus) b.focus(*this, focused); return false; }, std::cout);
}

void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {
    if (routeFeatureMouse(event, out)) return;
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
            if (item.second.id == *entity && item.second.whole &&
                (state_.selected.empty() || item.second.kind == "group"))
                state_.selected = item.first;
        const auto* component = app_.getScene().getComponent(*entity);
        entity = component ? component->parent : std::nullopt;
    }
    if (!state_.selected.empty()) {
        inspect(state_.selected, out);
        return;
    }
    out << "선택 해제\n";
}
void Assembly::onCursor(double x, double y, std::ostream& out) {
    if (routeFeatureCursor(x, y)) return;
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
````

## standard/modes/AssemblyPersistence.cpp

성공한 로드 경로를 다음 핫로드 대상으로 기록한다.

### before

````cpp
#include "Assembly.h"
#include <fstream>
#include <iomanip>
#include <sstream>

namespace assembly {
bool Assembly::save(const std::string& path, std::ostream& out) const {
    const auto file = std::filesystem::absolute(std::filesystem::u8path(path));
    auto temporary = file;
    temporary += L".saving-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(GetTickCount64());
    try {
        std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);
        require(bool(stream), "레시피 저장 파일을 열 수 없습니다.");
        stream << "\xEF\xBB\xBF# standard assembly recipe v1\nclear\n" << std::setprecision(9);
        for (const auto& line : state_.recipe)
            stream << line << '\n';
        stream << "# @state-begin\n";
        for (const auto& item : state_.nodes) {
            const auto& t = item.second.rest;
            stream << (item.second.affineGroup ? "rest3 " : "rest ") << quote(item.first) << ' ' << t.basis.x << ' ' << t.basis.y << ' ' << t.basis.z;
            for (int c = 0; c < 3; ++c)
                for (int r = 0; r < 3; ++r)
                    stream << ' ' << t.rotation[c][r];
            stream << ' ' << t.scale.x;
            if (item.second.affineGroup) stream << ' ' << t.scale.y << ' ' << t.scale.z;
            stream << ' ' << t.localBasis.x << ' ' << t.localBasis.y << ' ' << t.localBasis.z << '\n';
        }
        const auto& c = app_.getCamera();
        const auto& r = app_.getRenderOptions();
        stream << "camera pose " << c.position.x << ' ' << c.position.y << ' ' << c.position.z << ' '
               << c.target.x << ' ' << c.target.y << ' ' << c.target.z << ' ' << c.up.x << ' ' << c.up.y << ' ' << c.up.z << '\n'
               << "camera projection " << (c.orthographic ? "ortho" : "perspective") << '\n'
               << "camera extent " << c.orthographicHalfExtent << "\ncamera fov " << c.fieldOfViewDegrees
               << "\ncamera clip " << c.nearPlane << ' ' << c.farPlane << '\n'
               << "render wire " << (r.wireframe ? "on" : "off") << "\nrender lighting " << (r.lighting ? "on" : "off")
               << "\nrender depth " << (r.depthTest ? "on" : "off") << "\nview " << (state_.threeViews ? 3 : 1)
               << "\npause " << (state_.paused ? "on" : "off") << '\n'
               << "render cull " << (r.cull == graphics::CullMode::Front ? "front" : r.cull == graphics::CullMode::Back ? "back" : "none") << '\n'
               << "render light-direction " << r.lightDirection.x << ' ' << r.lightDirection.y << ' ' << r.lightDirection.z << '\n'
               << "render light-color " << r.lightColor.red << ' ' << r.lightColor.green << ' ' << r.lightColor.blue << '\n'
               << "render background " << r.background.r << ' ' << r.background.g << ' ' << r.background.b << '\n';
        stream.flush();
        require(bool(stream), "레시피 쓰기에 실패했습니다.");
        stream.close();
        require(bool(stream) && std::filesystem::file_size(temporary) <= 2 * 1024 * 1024, "저장 결과가 2MiB 한도를 넘거나 쓰기를 완료하지 못했습니다.");
        require(MoveFileExW(temporary.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0,
                "저장 파일 교체 실패. 기존 파일을 보존했습니다.");
        out << "조립 레시피 저장: " << file.u8string() << " (물리/애니메이션 현재 프레임의 스냅샷은 아님)\n";
        return true;
    } catch (...) {
        std::error_code ignored;
        std::filesystem::remove(temporary, ignored);
        throw;
    }
}

bool Assembly::load(const std::string& path, std::ostream& out) {
    const auto requested = std::filesystem::u8path(path);
    auto file = std::filesystem::absolute(requested);
    if (!requested.is_absolute() && !std::filesystem::exists(file)) {
        std::wstring binary(32768, L'\0');
        const DWORD count = GetModuleFileNameW(nullptr, binary.data(), DWORD(binary.size()));
        if (count > 0 && count < binary.size()) {
            binary.resize(count);
            const auto deployed = std::filesystem::path(binary).parent_path() / requested;
            if (std::filesystem::is_regular_file(deployed)) file = deployed;
        }
    }
    require(std::filesystem::file_size(file) <= 2 * 1024 * 1024, "레시피는 2MiB 이하입니다.");
    std::ifstream stream(file, std::ios::binary);
    require(bool(stream), "레시피 파일을 열 수 없습니다.");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (lines.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);
        require(line.size() <= 4096 && lines.size() < 10000, "명령은 4096바이트 이하, 레시피는 10000줄 미만입니다.");
        lines.push_back(line);
    }
    require(!stream.bad(), "레시피 읽기 실패.");
    clear();
    replaying_ = true;
    std::vector<std::string> authored;
    bool snapshot = false;
    try {
        for (std::size_t i = 0; i < lines.size(); ++i) {
            if (lines[i] == "# @state-begin") snapshot = true;
            if (!snapshot) authored.push_back(lines[i]);
            const auto tokens = tokenize(lines[i]);
            require(tokens.empty() || (tokens[0] != "save" && tokens[0] != "load"), "레시피 안의 save/load는 허용하지 않습니다.");
            require(execute(lines[i], out), "레시피 " + std::to_string(i + 1) + "줄 적용 실패.");
        }
        state_.recipe = std::move(authored);
        replaying_ = false;
        out << "조립 레시피 불러오기: " << file.u8string() << '\n';
        return true;
    } catch (...) {
        replaying_ = false;
        throw;
    }
}
} // namespace assembly
````

### after

````cpp
#include "Assembly.h"
#include <fstream>
#include <iomanip>
#include <sstream>

namespace assembly {
bool Assembly::save(const std::string& path, std::ostream& out) const {
    const auto file = std::filesystem::absolute(std::filesystem::u8path(path));
    auto temporary = file;
    temporary += L".saving-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(GetTickCount64());
    try {
        std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);
        require(bool(stream), "레시피 저장 파일을 열 수 없습니다.");
        stream << "\xEF\xBB\xBF# standard assembly recipe v1\nclear\n" << std::setprecision(9);
        for (const auto& line : state_.recipe)
            stream << line << '\n';
        stream << "# @state-begin\n";
        for (const auto& item : state_.nodes) {
            const auto& t = item.second.rest;
            stream << (item.second.affineGroup ? "rest3 " : "rest ") << quote(item.first) << ' ' << t.basis.x << ' ' << t.basis.y << ' ' << t.basis.z;
            for (int c = 0; c < 3; ++c)
                for (int r = 0; r < 3; ++r)
                    stream << ' ' << t.rotation[c][r];
            stream << ' ' << t.scale.x;
            if (item.second.affineGroup) stream << ' ' << t.scale.y << ' ' << t.scale.z;
            stream << ' ' << t.localBasis.x << ' ' << t.localBasis.y << ' ' << t.localBasis.z << '\n';
        }
        const auto& c = app_.getCamera();
        const auto& r = app_.getRenderOptions();
        stream << "camera pose " << c.position.x << ' ' << c.position.y << ' ' << c.position.z << ' '
               << c.target.x << ' ' << c.target.y << ' ' << c.target.z << ' ' << c.up.x << ' ' << c.up.y << ' ' << c.up.z << '\n'
               << "camera projection " << (c.orthographic ? "ortho" : "perspective") << '\n'
               << "camera extent " << c.orthographicHalfExtent << "\ncamera fov " << c.fieldOfViewDegrees
               << "\ncamera clip " << c.nearPlane << ' ' << c.farPlane << '\n'
               << "render wire " << (r.wireframe ? "on" : "off") << "\nrender lighting " << (r.lighting ? "on" : "off")
               << "\nrender depth " << (r.depthTest ? "on" : "off") << "\nview " << (state_.threeViews ? 3 : 1)
               << "\npause " << (state_.paused ? "on" : "off") << '\n'
               << "render cull " << (r.cull == graphics::CullMode::Front ? "front" : r.cull == graphics::CullMode::Back ? "back" : "none") << '\n'
               << "render light-direction " << r.lightDirection.x << ' ' << r.lightDirection.y << ' ' << r.lightDirection.z << '\n'
               << "render light-color " << r.lightColor.red << ' ' << r.lightColor.green << ' ' << r.lightColor.blue << '\n'
               << "render background " << r.background.r << ' ' << r.background.g << ' ' << r.background.b << '\n';
        stream.flush();
        require(bool(stream), "레시피 쓰기에 실패했습니다.");
        stream.close();
        require(bool(stream) && std::filesystem::file_size(temporary) <= 2 * 1024 * 1024, "저장 결과가 2MiB 한도를 넘거나 쓰기를 완료하지 못했습니다.");
        require(MoveFileExW(temporary.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0,
                "저장 파일 교체 실패. 기존 파일을 보존했습니다.");
        out << "조립 레시피 저장: " << file.u8string() << " (물리/애니메이션 현재 프레임의 스냅샷은 아님)\n";
        return true;
    } catch (...) {
        std::error_code ignored;
        std::filesystem::remove(temporary, ignored);
        throw;
    }
}

bool Assembly::load(const std::string& path, std::ostream& out) {
    const auto requested = std::filesystem::u8path(path);
    auto file = std::filesystem::absolute(requested);
    if (!requested.is_absolute() && !std::filesystem::exists(file)) {
        std::wstring binary(32768, L'\0');
        const DWORD count = GetModuleFileNameW(nullptr, binary.data(), DWORD(binary.size()));
        if (count > 0 && count < binary.size()) {
            binary.resize(count);
            const auto deployed = std::filesystem::path(binary).parent_path() / requested;
            if (std::filesystem::is_regular_file(deployed)) file = deployed;
        }
    }
    require(std::filesystem::file_size(file) <= 2 * 1024 * 1024, "레시피는 2MiB 이하입니다.");
    std::ifstream stream(file, std::ios::binary);
    require(bool(stream), "레시피 파일을 열 수 없습니다.");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (lines.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);
        require(line.size() <= 4096 && lines.size() < 10000, "명령은 4096바이트 이하, 레시피는 10000줄 미만입니다.");
        lines.push_back(line);
    }
    require(!stream.bad(), "레시피 읽기 실패.");
    clear();
    replaying_ = true;
    std::vector<std::string> authored;
    bool snapshot = false;
    try {
        for (std::size_t i = 0; i < lines.size(); ++i) {
            if (lines[i] == "# @state-begin") snapshot = true;
            if (!snapshot) authored.push_back(lines[i]);
            const auto tokens = tokenize(lines[i]);
            require(tokens.empty() || (tokens[0] != "save" && tokens[0] != "load" && tokens[0] != "reload"), "레시피 안의 save/load는 허용하지 않습니다.");
            require(execute(lines[i], out), "레시피 " + std::to_string(i + 1) + "줄 적용 실패.");
        }
        state_.recipe = std::move(authored);
        state_.reloadPath = file.u8string();
        replaying_ = false;
        out << "조립 레시피 불러오기: " << file.u8string() << '\n';
        return true;
    } catch (...) {
        replaying_ = false;
        throw;
    }
}
} // namespace assembly
````

## standard/modes/AssemblyRuntime.cpp

공유 native 프로그램은 한 번 갱신하고 표면 관절 전에 출력을 합성한다.

### before

````cpp
#include "Assembly.h"
#include <algorithm>
#include <limits>

namespace assembly {
namespace {
graphics::Bounds3 bounds(const graphics::Scene& scene, graphics::EntityId mesh) {
    const auto* component = scene.getComponent(mesh);
    const auto matrix = scene.worldMatrix(mesh);
    require(component && matrix, "경계 계산에 필요한 메시/행렬이 없습니다.");
    graphics::Bounds3 box{glm::vec3(std::numeric_limits<float>::max()), glm::vec3(std::numeric_limits<float>::lowest())};
    for (const auto& face : component->polyhedron.faces)
        for (const auto& vertex : face.localVertices) {
            const auto p = glm::vec3(*matrix * glm::vec4(vertex, 1));
            box.minimum = glm::min(box.minimum, p);
            box.maximum = glm::max(box.maximum, p);
        }
    require(box.valid(), "양수 크기의 메시 경계가 필요합니다.");
    return box;
}
}

bool Assembly::applyPoses(float dt) {
    auto& scene = app_.getScene();
    const auto motionFor = [&](const std::string& name) {
        graphics::Motion motion;
        glm::mat3 parentRotation(1);
        for (const auto& b : state_.behaviors) {
            if (!b.enabled || b.target != name || b.effect != Effect::Motion || b.parentSpin) continue;
            const auto sample = b.motion.sample(b.seconds);
            require(sample.has_value(), "부모 공간 모션 샘플 실패: " + b.name);
            parentRotation = glm::mat3(sample->parent) * parentRotation;
        }
        for (const auto& b : state_.behaviors) {
            if (!b.enabled || b.target != name || b.effect != Effect::Motion)
                continue;
            const auto sample = b.motion.sample(b.seconds);
            require(sample.has_value(), "Motion 샘플 실패: " + b.name);
            const auto& rest = node(name).rest;
            const auto pivot = glm::translate(glm::mat4(1), rest.localBasis);
            const auto inversePivot = glm::translate(glm::mat4(1), -rest.localBasis);
            auto parent = sample->parent, local = sample->local;
            if (b.parentSpin) {
                const auto rotation = glm::mat4(parentRotation * rest.rotation);
                local = glm::transpose(rotation) * parent * rotation;
                parent = glm::mat4(1);
            }
            require(motion.append(graphics::Motion::fixed(parent, graphics::TransformSpace::World)) &&
                    motion.append(graphics::Motion::fixed(pivot * local * inversePivot, graphics::TransformSpace::Local)),
                    "Motion 합성 실패: " + b.name);
        }
        return motion;
    };
    std::vector<graphics::JointAnimation> tracks;
    for (const auto& item : state_.nodes) {
        auto* driver = positionDriver(item.first);
        if (driver && (driver->effect == Effect::Body || driver->effect == Effect::Walk || driver->effect == Effect::Custom))
            continue;
        if (item.second.affineGroup) {
            require(scene.setTransform(item.second.id, item.second.rest), "정적 warp 그룹 적용 실패.");
            continue;
        }
        tracks.push_back({item.second.id, item.second.rest, motionFor(item.first)});
    }
    require(graphics::AnimationSystem::poseHierarchy(scene, tracks, 0), "기준 자세와 동작 합성 실패.");
    for (int phase = 0; phase < 4; ++phase) {
    for (auto& b : state_.behaviors) {
        const int order = b.effect == Effect::Body ? 1 : b.effect == Effect::Walk ? 2 : b.effect == Effect::Custom ? 3 : 0;
        if (phase != order)
            continue;
        auto& target = node(b.target);
        if (b.effect == Effect::Conveyor) {
            if (!b.enabled) continue;
            const double length = 6.2 + 1.5707963267948966 * .8 + 4.2;
            const auto pose = graphics::PatternSystem::conveyorCornerPose(b.seconds * b.rate + b.phase * length,
                {-4, 1, -2}, {2.2f, 1, -1.2f}, .8f, 3);
            require(pose.has_value(), "컨베이어 경로 계산 실패.");
            auto rest = target.rest;
            const auto turn = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(pose->yawDegrees), glm::vec3(0, 1, 0)));
            rest.basis = pose->position + glm::vec3(0, b.height, 0) + turn * rest.basis;
            rest.rotation = turn * rest.rotation;
            require(graphics::AnimationSystem::poseHierarchy(scene, {{target.id, rest, motionFor(b.target)}}, 0), "컨베이어 자세 적용 실패.");
        } else if (faceEffect(b.effect)) {
            require(target.mesh.has_value(), "면 동작에는 메시가 필요합니다.");
            if (!b.enabled) {
                require(scene.setFaceLocalMatrix(*target.mesh, b.faceIndex, b.initialFaceMatrix, b.initialFaceSpace) &&
                        scene.setFaceVisible(*target.mesh, b.faceIndex, b.initialFaceVisible), "면 기준값 복원 실패.");
                continue;
            }
            if (b.effect == Effect::FaceTrack) {
                const auto sample = b.motion.sample(b.seconds);
                require(sample.has_value(), "면 트랙 샘플 실패.");
                const auto matrix = sample->parent * b.faceTrackBase * sample->local;
                require(std::abs(glm::determinant(glm::mat3(matrix))) > 1e-18f &&
                        scene.setFaceLocalMatrix(*target.mesh, b.faceIndex, matrix, graphics::FaceAnimationSpace::Model),
                        "면 트랙이 특이하거나 유효하지 않습니다.");
            } else if (b.effect == Effect::Face) {
                auto config = b.face;
                config.degrees = b.enabled ? b.seconds * b.rate : 0;
                require(graphics::PatternSystem::rotateFace(scene, *target.mesh, b.faceIndex, config), "면 회전 실패.");
            } else {
                const float u = b.enabled ? float(std::clamp(b.seconds / b.rate, 0.0, 1.0)) : 0;
                require(graphics::PatternSystem::openRoomWall(scene, *target.mesh, b.faceIndex, u), "벽 개방 실패.");
            }
        } else if (b.effect == Effect::Custom) {
            const float stepTime = b.enabled && b.playing && !state_.paused ? dt : 0;
            if (b.enabled) require(b.custom && b.custom(scene, target.rest, b.seconds, stepTime), "C++ 사용자 동작 실패: " + b.name);
        } else if (b.effect == Effect::Body) {
            const auto& room = node(b.reference);
            const auto matrix = scene.worldMatrix(room.id);
            require(room.room && matrix && rigid(*matrix), "물리 방은 강체여야 합니다.");
            const auto* walls = room.mesh ? scene.getComponent(*room.mesh) : nullptr;
            require(walls && walls->polyhedron.faces.size() > 5, "방 바닥이 없습니다.");
            const bool open = !walls->polyhedron.faces[5].visible;
            const auto* mesh = target.mesh ? scene.getComponent(*target.mesh) : nullptr;
            require(mesh != nullptr, "물리 상자 메시가 없습니다.");
            glm::vec3 half(0);
            for (const auto& face : mesh->polyhedron.faces)
                for (const auto& vertex : face.localVertices)
                    half = glm::max(half, glm::abs(target.rest.rotation * (vertex * target.rest.scale)));
            if (target.kind == "sphere") {
                float radius = 0;
                for (const auto& face : mesh->polyhedron.faces)
                    for (const auto& vertex : face.localVertices) radius = std::max(radius, glm::length(vertex));
                half = glm::vec3(radius * target.rest.scale.x);
            }
            b.body.halfExtent = half;
            if (!b.body.released) {
                b.releasedRotation = glm::mat3(*matrix);
                const auto low = room.room->minimum + half, high = room.room->maximum - half;
                require(glm::all(glm::lessThan(low, high)), "상자가 방보다 큽니다.");
                b.body.localPosition.x = std::clamp(b.body.localPosition.x, low.x, high.x);
                b.body.localPosition.z = std::clamp(b.body.localPosition.z, low.z, high.z);
                b.body.localPosition.y = open ? std::min(b.body.localPosition.y, high.y)
                                              : std::clamp(b.body.localPosition.y, low.y, high.y);
            }
            glm::vec3 omega(0);
            if (dt > 0) {
                const auto previous = glm::mat3(b.previousRoom), current = glm::mat3(*matrix);
                omega = (glm::cross(previous[0], current[0]) + glm::cross(previous[1], current[1]) +
                         glm::cross(previous[2], current[2])) / (2 * dt);
            }
            if (dt > 0) b.angularVelocity = omega;
            if (b.releaseRequested && !b.body.released) {
                b.body.worldPosition = b.body.position(*matrix);
                b.body.worldVelocity = glm::mat3(*matrix) * b.body.localVelocity +
                    glm::cross(b.angularVelocity, b.body.worldPosition - glm::vec3((*matrix)[3]));
                b.body.released = true;
            }
            b.releaseRequested = false;
            require(graphics::CollisionSystem::stepRoomBody(b.body, *room.room, *matrix,
                b.enabled && b.playing && !state_.paused ? dt : 0, open, b.elastic, b.gravity, b.angularVelocity), "방 충돌 계산 실패.");
            b.previousRoom = *matrix;
            auto rest = target.rest;
            rest.basis = b.body.position(*matrix);
            rest.rotation = b.releasedRotation * rest.rotation;
            require(scene.setTransform(target.id, rest), "물리 상자 자세 적용 실패.");
        } else if (b.effect == Effect::Walk) {
            const auto& room = node(b.reference);
            const auto matrix = scene.worldMatrix(room.id);
            require(room.room && target.walker && matrix && rigid(*matrix), "보행 리그와 방이 필요합니다.");
            const auto r = glm::mat3(*matrix);
            require(glm::length(r[0] - glm::vec3(1, 0, 0)) < .001f && glm::length(r[1] - glm::vec3(0, 1, 0)) < .001f,
                    "보행 공간은 월드 축 정렬을 유지해야 합니다.");
            const auto offset = glm::vec3((*matrix)[3]);
            const graphics::Bounds3 stage{room.room->minimum + offset, room.room->maximum + offset};
            std::vector<graphics::Bounds3> obstacles;
            for (const auto& obstacle : b.obstacles) {
                const auto& object = node(obstacle);
                require(object.mesh.has_value(), "장애물 메시가 없습니다.");
                obstacles.push_back(bounds(scene, *object.mesh));
            }
            const auto* walls = room.mesh ? scene.getComponent(*room.mesh) : nullptr;
            require(walls && walls->polyhedron.faces.size() > 5, "보행 공간의 바닥이 없습니다.");
            b.released = b.released || !walls->polyhedron.faces[5].visible;
            const float stepTime = b.enabled && b.playing && !state_.paused ? dt : 0;
            if (b.released) {
                const auto delta = b.character.direction * b.character.speed * stepTime;
                b.character.verticalSpeed -= 9.8f * stepTime;
                b.character.feet += delta + glm::vec3(0, b.character.verticalSpeed * stepTime, 0);
                b.character.walkDistance += glm::length(glm::vec2(delta.x, delta.z));
                b.character.grounded = false;
            } else {
                const auto step = graphics::CollisionSystem::stepCharacter(b.character, stage, obstacles, stepTime);
                require(step.has_value(), "보행 충돌 계산 실패.");
            }
            const float yaw = glm::degrees(std::atan2(b.character.direction.x, b.character.direction.z));
            require(graphics::PatternSystem::poseWalkingRig(scene, *target.walker, b.character.feet, yaw,
                b.character.walkDistance, b.character.speed), "보행 자세 적용 실패.");
            for (const auto& part : state_.nodes) {
                const auto id = part.second.id;
                const auto& rig = *target.walker;
                if (id != rig.head && id != rig.shoulders[0] && id != rig.shoulders[1] && id != rig.hips[0] && id != rig.hips[1])
                    continue;
                auto rest = scene.getComponent(id)->transform;
                const auto matrix = graphics::DiagramSystem::modelMatrix(rest) *
                    glm::inverse(graphics::DiagramSystem::modelMatrix(part.second.initial)) *
                    graphics::DiagramSystem::modelMatrix(part.second.rest);
                rest.scale = part.second.rest.scale;
                rest.localBasis = part.second.rest.localBasis;
                rest.rotation = glm::mat3(matrix) / rest.scale.x;
                rest.basis = glm::vec3(matrix[3]) + rest.rotation * (rest.scale * rest.localBasis);
                const std::vector<graphics::JointAnimation> edited{{part.second.id, rest, motionFor(part.first)}};
                require(graphics::AnimationSystem::poseHierarchy(scene, edited, 0), "보행과 부품 동작 합성 실패.");
            }
        }
    }
    }
    applyMounts();
    return true;
}

bool Assembly::update(float dt, std::ostream& out) {
    if (faulted_)
        return false;
    try {
        require(std::isfinite(dt) && dt >= 0 && dt <= .25f, "dt는 0~.25초여야 합니다.");
        for (const auto& binding : state_.bindings) {
            if (!binding.hold || !binding.down)
                continue;
            if (binding.tokens[0] == "move")
                move(binding.tokens[1], vector(binding.tokens, 2) * dt);
            else
                rotate(binding.tokens[1], axis(binding.tokens[2]), float(number(binding.tokens[3])) * dt);
        }
        if (!state_.paused)
            for (auto& behavior : state_.behaviors)
                if (behavior.playing && behavior.enabled)
                    behavior.seconds += dt;
        return applyPoses(dt);
    } catch (const std::exception& error) {
        faulted_ = true;
        state_.paused = true;
        state_.dragging = false; state_.dragX.reset();
        for (auto& binding : state_.bindings)
            binding.down = false;
        out << "업데이트 정지: " << error.what() << "\nremove <동작> 또는 clear로 조립을 수정하세요. guide physics\n";
        return false;
    }
}

bool Assembly::render() {
    if (!state_.threeViews)
        return app_.renderScene();
    const auto size = app_.getFramebufferSize();
    return size[0] < 3 || size[1] < 2 || app_.renderViews(graphics::PatternSystem::threeViews(
        app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
} // namespace assembly
````

### after

````cpp
#include "Assembly.h"
#include <algorithm>
#include <limits>

namespace assembly {
namespace {
graphics::Bounds3 bounds(const graphics::Scene& scene, graphics::EntityId mesh) {
    const auto* component = scene.getComponent(mesh);
    const auto matrix = scene.worldMatrix(mesh);
    require(component && matrix, "경계 계산에 필요한 메시/행렬이 없습니다.");
    graphics::Bounds3 box{glm::vec3(std::numeric_limits<float>::max()), glm::vec3(std::numeric_limits<float>::lowest())};
    for (const auto& face : component->polyhedron.faces)
        for (const auto& vertex : face.localVertices) {
            const auto p = glm::vec3(*matrix * glm::vec4(vertex, 1));
            box.minimum = glm::min(box.minimum, p);
            box.maximum = glm::max(box.maximum, p);
        }
    require(box.valid(), "양수 크기의 메시 경계가 필요합니다.");
    return box;
}
}

bool Assembly::applyPoses(float dt) {
    auto& scene = app_.getScene();
    const auto motionFor = [&](const std::string& name) {
        graphics::Motion motion;
        glm::mat3 parentRotation(1);
        for (const auto& b : state_.behaviors) {
            if (!b.enabled || b.target != name || b.effect != Effect::Motion || b.parentSpin) continue;
            const auto sample = b.motion.sample(b.seconds);
            require(sample.has_value(), "부모 공간 모션 샘플 실패: " + b.name);
            parentRotation = glm::mat3(sample->parent) * parentRotation;
        }
        for (const auto& b : state_.behaviors) {
            if (!b.enabled || b.target != name || b.effect != Effect::Motion)
                continue;
            const auto sample = b.motion.sample(b.seconds);
            require(sample.has_value(), "Motion 샘플 실패: " + b.name);
            const auto& rest = node(name).rest;
            const auto pivot = glm::translate(glm::mat4(1), rest.localBasis);
            const auto inversePivot = glm::translate(glm::mat4(1), -rest.localBasis);
            auto parent = sample->parent, local = sample->local;
            if (b.parentSpin) {
                const auto rotation = glm::mat4(parentRotation * rest.rotation);
                local = glm::transpose(rotation) * parent * rotation;
                parent = glm::mat4(1);
            }
            require(motion.append(graphics::Motion::fixed(parent, graphics::TransformSpace::World)) &&
                    motion.append(graphics::Motion::fixed(pivot * local * inversePivot, graphics::TransformSpace::Local)),
                    "Motion 합성 실패: " + b.name);
        }
        return motion;
    };
    std::vector<graphics::JointAnimation> tracks;
    for (const auto& item : state_.nodes) {
        auto* driver = positionDriver(item.first);
        if (driver && (driver->effect == Effect::Body || driver->effect == Effect::Walk || driver->effect == Effect::Custom))
            continue;
        if (item.second.affineGroup) {
            require(scene.setTransform(item.second.id, item.second.rest), "정적 warp 그룹 적용 실패.");
            continue;
        }
        tracks.push_back({item.second.id, item.second.rest, motionFor(item.first)});
    }
    require(graphics::AnimationSystem::poseHierarchy(scene, tracks, 0), "기준 자세와 동작 합성 실패.");
    for (int phase = 0; phase < 4; ++phase) {
    for (auto& b : state_.behaviors) {
        const int order = b.effect == Effect::Body ? 1 : b.effect == Effect::Walk ? 2 : b.effect == Effect::Custom ? 3 : 0;
        if (phase != order)
            continue;
        auto& target = node(b.target);
        if (b.effect == Effect::Conveyor) {
            if (!b.enabled) continue;
            const double length = 6.2 + 1.5707963267948966 * .8 + 4.2;
            const auto pose = graphics::PatternSystem::conveyorCornerPose(b.seconds * b.rate + b.phase * length,
                {-4, 1, -2}, {2.2f, 1, -1.2f}, .8f, 3);
            require(pose.has_value(), "컨베이어 경로 계산 실패.");
            auto rest = target.rest;
            const auto turn = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(pose->yawDegrees), glm::vec3(0, 1, 0)));
            rest.basis = pose->position + glm::vec3(0, b.height, 0) + turn * rest.basis;
            rest.rotation = turn * rest.rotation;
            require(graphics::AnimationSystem::poseHierarchy(scene, {{target.id, rest, motionFor(b.target)}}, 0), "컨베이어 자세 적용 실패.");
        } else if (faceEffect(b.effect)) {
            require(target.mesh.has_value(), "면 동작에는 메시가 필요합니다.");
            if (!b.enabled) {
                require(scene.setFaceLocalMatrix(*target.mesh, b.faceIndex, b.initialFaceMatrix, b.initialFaceSpace) &&
                        scene.setFaceVisible(*target.mesh, b.faceIndex, b.initialFaceVisible), "면 기준값 복원 실패.");
                continue;
            }
            if (b.effect == Effect::FaceTrack) {
                const auto sample = b.motion.sample(b.seconds);
                require(sample.has_value(), "면 트랙 샘플 실패.");
                const auto matrix = sample->parent * b.faceTrackBase * sample->local;
                require(std::abs(glm::determinant(glm::mat3(matrix))) > 1e-18f &&
                        scene.setFaceLocalMatrix(*target.mesh, b.faceIndex, matrix, graphics::FaceAnimationSpace::Model),
                        "면 트랙이 특이하거나 유효하지 않습니다.");
            } else if (b.effect == Effect::Face) {
                auto config = b.face;
                config.degrees = b.enabled ? b.seconds * b.rate : 0;
                require(graphics::PatternSystem::rotateFace(scene, *target.mesh, b.faceIndex, config), "면 회전 실패.");
            } else {
                const float u = b.enabled ? float(std::clamp(b.seconds / b.rate, 0.0, 1.0)) : 0;
                require(graphics::PatternSystem::openRoomWall(scene, *target.mesh, b.faceIndex, u), "벽 개방 실패.");
            }
        } else if (b.effect == Effect::Custom) {
            const float stepTime = b.enabled && b.playing && !state_.paused ? dt : 0;
            if (b.enabled) require(b.custom && b.custom(scene, target.rest, b.seconds, stepTime), "C++ 사용자 동작 실패: " + b.name);
        } else if (b.effect == Effect::Body) {
            const auto& room = node(b.reference);
            const auto matrix = scene.worldMatrix(room.id);
            require(room.room && matrix && rigid(*matrix), "물리 방은 강체여야 합니다.");
            const auto* walls = room.mesh ? scene.getComponent(*room.mesh) : nullptr;
            require(walls && walls->polyhedron.faces.size() > 5, "방 바닥이 없습니다.");
            const bool open = !walls->polyhedron.faces[5].visible;
            const auto* mesh = target.mesh ? scene.getComponent(*target.mesh) : nullptr;
            require(mesh != nullptr, "물리 상자 메시가 없습니다.");
            glm::vec3 half(0);
            for (const auto& face : mesh->polyhedron.faces)
                for (const auto& vertex : face.localVertices)
                    half = glm::max(half, glm::abs(target.rest.rotation * (vertex * target.rest.scale)));
            if (target.kind == "sphere") {
                float radius = 0;
                for (const auto& face : mesh->polyhedron.faces)
                    for (const auto& vertex : face.localVertices) radius = std::max(radius, glm::length(vertex));
                half = glm::vec3(radius * target.rest.scale.x);
            }
            b.body.halfExtent = half;
            if (!b.body.released) {
                b.releasedRotation = glm::mat3(*matrix);
                const auto low = room.room->minimum + half, high = room.room->maximum - half;
                require(glm::all(glm::lessThan(low, high)), "상자가 방보다 큽니다.");
                b.body.localPosition.x = std::clamp(b.body.localPosition.x, low.x, high.x);
                b.body.localPosition.z = std::clamp(b.body.localPosition.z, low.z, high.z);
                b.body.localPosition.y = open ? std::min(b.body.localPosition.y, high.y)
                                              : std::clamp(b.body.localPosition.y, low.y, high.y);
            }
            glm::vec3 omega(0);
            if (dt > 0) {
                const auto previous = glm::mat3(b.previousRoom), current = glm::mat3(*matrix);
                omega = (glm::cross(previous[0], current[0]) + glm::cross(previous[1], current[1]) +
                         glm::cross(previous[2], current[2])) / (2 * dt);
            }
            if (dt > 0) b.angularVelocity = omega;
            if (b.releaseRequested && !b.body.released) {
                b.body.worldPosition = b.body.position(*matrix);
                b.body.worldVelocity = glm::mat3(*matrix) * b.body.localVelocity +
                    glm::cross(b.angularVelocity, b.body.worldPosition - glm::vec3((*matrix)[3]));
                b.body.released = true;
            }
            b.releaseRequested = false;
            require(graphics::CollisionSystem::stepRoomBody(b.body, *room.room, *matrix,
                b.enabled && b.playing && !state_.paused ? dt : 0, open, b.elastic, b.gravity, b.angularVelocity), "방 충돌 계산 실패.");
            b.previousRoom = *matrix;
            auto rest = target.rest;
            rest.basis = b.body.position(*matrix);
            rest.rotation = b.releasedRotation * rest.rotation;
            require(scene.setTransform(target.id, rest), "물리 상자 자세 적용 실패.");
        } else if (b.effect == Effect::Walk) {
            const auto& room = node(b.reference);
            const auto matrix = scene.worldMatrix(room.id);
            require(room.room && target.walker && matrix && rigid(*matrix), "보행 리그와 방이 필요합니다.");
            const auto r = glm::mat3(*matrix);
            require(glm::length(r[0] - glm::vec3(1, 0, 0)) < .001f && glm::length(r[1] - glm::vec3(0, 1, 0)) < .001f,
                    "보행 공간은 월드 축 정렬을 유지해야 합니다.");
            const auto offset = glm::vec3((*matrix)[3]);
            const graphics::Bounds3 stage{room.room->minimum + offset, room.room->maximum + offset};
            std::vector<graphics::Bounds3> obstacles;
            for (const auto& obstacle : b.obstacles) {
                const auto& object = node(obstacle);
                require(object.mesh.has_value(), "장애물 메시가 없습니다.");
                obstacles.push_back(bounds(scene, *object.mesh));
            }
            const auto* walls = room.mesh ? scene.getComponent(*room.mesh) : nullptr;
            require(walls && walls->polyhedron.faces.size() > 5, "보행 공간의 바닥이 없습니다.");
            b.released = b.released || !walls->polyhedron.faces[5].visible;
            const float stepTime = b.enabled && b.playing && !state_.paused ? dt : 0;
            if (b.released) {
                const auto delta = b.character.direction * b.character.speed * stepTime;
                b.character.verticalSpeed -= 9.8f * stepTime;
                b.character.feet += delta + glm::vec3(0, b.character.verticalSpeed * stepTime, 0);
                b.character.walkDistance += glm::length(glm::vec2(delta.x, delta.z));
                b.character.grounded = false;
            } else {
                const auto step = graphics::CollisionSystem::stepCharacter(b.character, stage, obstacles, stepTime);
                require(step.has_value(), "보행 충돌 계산 실패.");
            }
            const float yaw = glm::degrees(std::atan2(b.character.direction.x, b.character.direction.z));
            require(graphics::PatternSystem::poseWalkingRig(scene, *target.walker, b.character.feet, yaw,
                b.character.walkDistance, b.character.speed), "보행 자세 적용 실패.");
            for (const auto& part : state_.nodes) {
                const auto id = part.second.id;
                const auto& rig = *target.walker;
                if (id != rig.head && id != rig.shoulders[0] && id != rig.shoulders[1] && id != rig.hips[0] && id != rig.hips[1])
                    continue;
                auto rest = scene.getComponent(id)->transform;
                const auto matrix = graphics::DiagramSystem::modelMatrix(rest) *
                    glm::inverse(graphics::DiagramSystem::modelMatrix(part.second.initial)) *
                    graphics::DiagramSystem::modelMatrix(part.second.rest);
                rest.scale = part.second.rest.scale;
                rest.localBasis = part.second.rest.localBasis;
                rest.rotation = glm::mat3(matrix) / rest.scale.x;
                rest.basis = glm::vec3(matrix[3]) + rest.rotation * (rest.scale * rest.localBasis);
                const std::vector<graphics::JointAnimation> edited{{part.second.id, rest, motionFor(part.first)}};
                require(graphics::AnimationSystem::poseHierarchy(scene, edited, 0), "보행과 부품 동작 합성 실패.");
            }
        }
    }
    }
    syncFeatures();
    applyMounts();
    return true;
}

bool Assembly::update(float dt, std::ostream& out) {
    if (faulted_)
        return false;
    try {
        require(std::isfinite(dt) && dt >= 0 && dt <= .25f, "dt는 0~.25초여야 합니다.");
        for (const auto& binding : state_.bindings) {
            if (!binding.hold || !binding.down)
                continue;
            if (binding.tokens[0] == "move")
                move(binding.tokens[1], vector(binding.tokens, 2) * dt);
            else
                rotate(binding.tokens[1], axis(binding.tokens[2]), float(number(binding.tokens[3])) * dt);
        }
        if (!state_.paused)
            for (auto& behavior : state_.behaviors)
                if (behavior.playing && behavior.enabled)
                    behavior.seconds += dt;
        advanceFeatures(dt);
        return applyPoses(dt);
    } catch (const std::exception& error) {
        faulted_ = true;
        state_.paused = true;
        state_.dragging = false; state_.dragX.reset();
        for (auto& binding : state_.bindings)
            binding.down = false;
        out << "업데이트 정지: " << error.what() << "\nremove <동작> 또는 clear로 조립을 수정하세요. guide physics\n";
        return false;
    }
}

bool Assembly::render() {
    if (!state_.threeViews)
        return app_.renderScene();
    const auto size = app_.getFramebufferSize();
    return size[0] < 3 || size[1] < 2 || app_.renderViews(graphics::PatternSystem::threeViews(
        app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
} // namespace assembly
````

## standard/modes/AssemblySurface.cpp

native 소유 형상과 snapshot 편집 경계를 유지한다.

### before

````cpp
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

void Assembly::requireGeometryEditable(graphics::EntityId mesh) const {
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
````

### after

````cpp
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
````

## standard/modes/FeatureContainer.cpp

하나의 Application/GL 컨텍스트에서 CPU Scene과 native 제어기를 격리한다.

### before

없음.

### after

````cpp
#include "FeatureContainer.h"
#include "Assembly.h"
#include "../Console.h"

namespace assembly {
struct FeatureContainer::Impl {
    graphics::Application& host;
    int mode;
    graphics::Scene scene;
    graphics::Camera camera;
    graphics::RenderOptions options;
    Console controller;

    struct Context {
        Impl& owner;
        graphics::Scene saved;
        graphics::Camera camera;
        graphics::RenderOptions options;
        bool closing = false;
        explicit Context(Impl& value) : owner(value), saved(std::move(value.host.getScene())),
            camera(value.host.getCamera()), options(value.host.getRenderOptions()) {
            closing = owner.host.getWindow() && graphics::detail::glfw().WindowShouldClose(owner.host.getWindow());
            owner.host.getScene() = std::move(owner.scene);
            owner.host.getCamera() = owner.camera;
            owner.host.getRenderOptions() = owner.options;
        }
        void check() const {
            require(closing || !owner.host.getWindow() || !graphics::detail::glfw().WindowShouldClose(owner.host.getWindow()),
                "실습 컨테이너가 실패로 종료를 요청했습니다. guide runtime 또는 feature reset을 사용하세요.");
        }
        ~Context() {
            if (owner.host.getWindow() && !closing) graphics::detail::glfw().SetWindowShouldClose(owner.host.getWindow(), GLFW_FALSE);
            owner.scene = std::move(owner.host.getScene());
            owner.camera = owner.host.getCamera();
            owner.options = owner.host.getRenderOptions();
            owner.host.getScene() = std::move(saved);
            owner.host.getCamera() = camera;
            owner.host.getRenderOptions() = options;
        }
    };
    Impl(graphics::Application& app, int number) : host(app), mode(number), controller(app, number) {
        require(mode >= 1 && mode <= lastStandardMode, "feature 모드는 1~22입니다. 0번을 재귀 포함할 수 없습니다.");
        Context context(*this);
        require(controller.initScene(), "실습 컨테이너 초기화 실패: " + std::to_string(mode));
        context.check();
    }
};
FeatureContainer::FeatureContainer(graphics::Application& host, int mode) : impl_(std::make_unique<Impl>(host, mode)) {}
FeatureContainer::~FeatureContainer() = default;
int FeatureContainer::mode() const { return impl_->mode; }
const graphics::Scene& FeatureContainer::scene() const { return impl_->scene; }
const graphics::Camera& FeatureContainer::camera() const { return impl_->camera; }
const graphics::RenderOptions& FeatureContainer::options() const { return impl_->options; }
void FeatureContainer::step(float dt) {
    Impl::Context context(*impl_);
    impl_->controller.onUpdate(dt);
    require(impl_->host.getScene().updateMotions(dt), "실습 컨테이너 Motion 갱신 실패.");
    context.check();
}
void FeatureContainer::key(int key, int action, int mods) {
    require(!(key == GLFW_KEY_F5 && (mods & GLFW_MOD_CONTROL)), "컨테이너 내부에서는 작업공간을 재귀 생성하지 않습니다. 상위 Ctrl+F5를 사용하세요.");
    require(key != GLFW_KEY_Q && key != GLFW_KEY_ESCAPE, "Q/Esc는 컨테이너에 전달하지 않습니다. feature enable 또는 focus none을 사용하세요.");
    Impl::Context context(*impl_);
    impl_->controller.onKey(key, action, mods);
    context.check();
}
void FeatureContainer::mouse(const graphics::MouseEvent& event, const graphics::Camera& view) {
    Impl::Context context(*impl_);
    const auto saved = impl_->host.getCamera();
    impl_->host.getCamera() = view;
    try { impl_->controller.onMouse(event); }
    catch (...) { impl_->host.getCamera() = saved; throw; }
    impl_->host.getCamera() = saved;
    context.check();
}
void FeatureContainer::cursor(double x, double y) {
    Impl::Context context(*impl_); impl_->controller.onCursor(x, y); context.check();
}
void FeatureContainer::pick(std::optional<graphics::PickResult> hit) {
    Impl::Context context(*impl_); impl_->controller.selectFromContainer(hit); context.check();
}
void FeatureContainer::focus(bool value) {
    Impl::Context context(*impl_); impl_->controller.onFocus(value); context.check();
}
} // namespace assembly
````

## standard/modes/FeatureContainer.h

컨테이너·이벤트·엔티티/선 출력 포트 타입을 정의한다.

### before

없음.

### after

````cpp
#pragma once
#include "PracticeMode.h"
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace assembly {
// CPU-only state. The host remains the only Application and OpenGL context.
class FeatureContainer {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    FeatureContainer(graphics::Application& host, int mode);
    ~FeatureContainer();
    FeatureContainer(const FeatureContainer&) = delete;
    FeatureContainer& operator=(const FeatureContainer&) = delete;
    int mode() const;
    const graphics::Scene& scene() const;
    const graphics::Camera& camera() const;
    const graphics::RenderOptions& options() const;
    void step(float dt);
    void key(int key, int action, int mods);
    void mouse(const graphics::MouseEvent& event, const graphics::Camera& view);
    void cursor(double x, double y);
    void focus(bool value);
    void pick(std::optional<graphics::PickResult> hit);
};

struct FeatureEvent {
    enum class Kind { Key, Mouse, Cursor, Focus, Pick } kind = Kind::Focus;
    int key = 0, action = 0, mods = 0;
    double x = 0, y = 0;
    bool focused = false;
    graphics::MouseEvent mouse;
    graphics::Camera view;
    std::optional<graphics::PickResult> hit;
};
struct FeatureEntity {
    graphics::EntityId group = 0;
    std::optional<graphics::EntityId> mesh, nativeParent;
    bool parentKnown = false, visible = true;
    graphics::GlColor tint;
};
struct FeatureLine {
    graphics::LineId line = 0;
    graphics::EntityId frame = 0;
    glm::vec3 start{0}, end{0};
    bool visible = true;
};
struct FeatureInstance {
    std::string name;
    std::shared_ptr<FeatureContainer> program;
    std::optional<graphics::EntityId> branch;
    std::map<graphics::EntityId, FeatureEntity> entities;
    std::map<std::size_t, FeatureLine> lines;
    std::uint64_t revision = 0;
    bool enabled = true, playing = true;
    std::vector<FeatureEvent> events;
};
} // namespace assembly
````

## standard/modes/GuideRecipes.cpp

55개 기능/실습의 실제 표식·변경 예·디버깅 지식을 등록한다.

### before

없음.

### after

````cpp
#include "GuideRecipes.h"
#include <string>
#include <utility>

namespace assembly {
const std::vector<GuideRecipe>& guideRecipes() {
    static const std::vector<GuideRecipe> recipes{
        {"tank", "포신 길이와 중심을 함께 변경", "scene.part(\"box\", gun, \"barrel\", {0,0,1}, {.2f,.2f,2});\nscene.part(\"box\", gun, \"muzzle\", {0,0,2}, {.35f,.35f,.3f});", "scene.part(\"box\", gun, \"barrel\", {0,0,1.5f}, {.2f,.2f,3});\nscene.part(\"box\", gun, \"muzzle\", {0,0,3}, {.35f,.35f,.3f});", "길이는 모델 단위다. 길이 L이면 중심은 L/2, 총구는 L이다. 관절 위치는 유지한다.", "1. gun 기준점과 barrel/muzzle 위치, turretMotor.seconds를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "포탑 회전 중 포신 시작점은 고정되고 끝만 1 늘어난다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExamples.cpp", "scene.part(\"box\", gun, \"barrel\"", "포신 길이와 중심을 함께 변경"}, {"modes/AssemblyExamples.cpp", "rotation.degreesPerSecond =", "포탑 각속도: 도/초"}}},
        {"truck", "문 회전축과 열림 범위 수정", "pendulum(scene, name + \".leftDoorMotor\", left, {0,1,0}, 40,6,40,-90);", "pendulum(scene, name + \".leftDoorMotor\", left, {0,1,0}, 30,6,30,-90);", "offset±amplitude가 범위다. 예시는 0~80도를 0~60도로 바꾼다. 문 메시 중심을 힌지에서 떨어뜨려 둔다.", "1. leftDoor pivot와 panel 로컬 X=+.425, phase=-90을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "처음 닫힘을 유지하며 최대 열림각이 60도가 된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExamples.cpp", "const auto left = scene.part", "문 회전축과 열림 범위 수정"}, {"modes/AssemblyExamples.cpp", "void buildTruckExample(", "트럭/컨테이너 생성"}}},
        {"person", "사람 어깨·팔 비율 수정", "{sign * .38f, .17f, 0}", "{sign * .45f, .17f, 0}", "어깨 위치는 torso 로컬 좌표다. upperArm 길이를 바꾸면 elbow의 Y 오프셋도 같은 길이로 맞춘다.", "1. shoulder→elbow→forearm 부모 체인과 양팔 phase 차이180도를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "몸통 폭을 바꾸지 않고 어깨 간격이 넓어진다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExamples.cpp", "const auto shoulder = scene.part", "사람 어깨·팔 비율 수정"}, {"modes/AssemblyExamples.cpp", "void buildPersonExample(", "사람 전체 생성"}}},
        {"objects", "기본 형상 크기 지정", "scene.add(\"box\", \"crate\", {0,1,0}, {1,1,1});", "scene.add(\"box\", \"crate\", {0,1,0}, {2,1,.5f});", "생성 치수는 정점 데이터다. 전체 비율 변경은 scale/SRT와 구분한다. 크기는 양수다.", "1. Node.mesh와 실제 mesh.localVertices, 생성 실패 require 문구를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "폭2·높이1·깊이.5 상자를 생성한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "void Assembly::add(", "기본 형상 크기 지정"}}},
        {"geometry", "사용자 정점·삼각형 추가", "triangle plate 0 0 0 1 0 0 0 1 0", "triangle plate 0 0 0 2 0 0 0 1 0", "명령 예시다. 중복/일직선 점은 삼각형 면적0이므로 거절한다. C++에서는 Model→Assembly::model을 사용한다.", "1. triangleIndices 범위, cross(B-A,C-A), geometryRevision을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "가로 길이가2인 삼각형을 만든다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblySurface.cpp", "void Assembly::model(", "사용자 정점·삼각형 추가"}}},
        {"hierarchy", "관절을 부모에 붙이는 위치 수정", "joint robot elbow 0 -0.4 0", "joint robot elbow 0 -0.6 0", "부모 로컬 좌표다. parent는 현재 로컬 SRT를 유지하며 월드 자세를 자동 보존하지 않는다.", "1. 부모 ID, localBasis, worldMatrix의 부모 곱 순서를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "팔꿈치 연결점이 부모 아래로 .2 이동한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyHierarchy.cpp", "std::string Assembly::part(", "관절을 부모에 붙이는 위치 수정"}}},
        {"srt", "루트 위치·회전·균일 배율 변경", "srt hero 0 0 0 0 0 0 1", "srt hero 2 0 0 0 90 0 1.5", "각도는 도, 회전은 Rz*Ry*Rx다. 위치는 부모 공간 피벗 좌표다. 양수 균일 배율 .0001 초과100 이하.", "1. rest.basis/rotation/scale와 실제 Scene transform을 비교한다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "전체 계층이 X=2, Y회전90도, 1.5배가 된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyHierarchy.cpp", "void Assembly::srt(", "루트 위치·회전·균일 배율 변경"}}},
        {"pivot", "중심을 바꾸고 기준 배치 보존", "pivot hero 0 0 0 raw", "pivot hero 0 0.9 0 keep", "keep는 basis += R*S*(새중심-이전중심)이다. 동작 부착 전 또는 seek0에서 수정한다.", "1. rest.localBasis와 basis 보정, 중심을 둔 회전의 T(C)*R*T(-C)를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "기준 외형 위치를 유지하며 이후 자전 중심이 높이.9가 된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyHierarchy.cpp", "void Assembly::pivot(", "중심을 바꾸고 기준 배치 보존"}}},
        {"center", "하위 형상 AABB로 중심 계산", "center hero raw", "center hero keep", "현재 하위 메시의 로컬 AABB 중심이며 질량중심이 아니다. 숨긴 면도 포함한다.", "1. inverse(rootWorld)*faceWorld*vertex의 최소/최대, 활성 모션 시간을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "현재 형상 배치가 유지되며 자동 계산 중심을 사용한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyHierarchy.cpp", "glm::vec3 Assembly::center(", "하위 형상 AABB로 중심 계산"}}},
        {"mount", "면·삼각형 좌표에 관절 부착", "mount base hinge 4 0 0 0.5 0", "mount base hinge 4 0 0.25 0.25 0.1", "P=(1-U-V)A+UB+VC. U,V>=0,U+V<=1. 마지막 값은 메시 로컬 기하법선 거리다.", "1. surface의 삼각형 인덱스, U/V, 기하법선과 shading normal 차이를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "관절이 지정 표면 좌표에서 법선 방향 .1 떨어져 면 동작을 따른다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblySurface.cpp", "std::string Assembly::mount(", "면·삼각형 좌표에 관절 부착"}, {"modes/AssemblySurface.cpp", "void Assembly::applyMounts(", "프레임마다 부착점과 방향 추적"}}},
        {"normal", "조명 노멀과 winding 분리", "normal crate 0 0 0 1", "normal crate 0 0 0 -1", "노멀은 조명용이다. 컬링을 바꾸려면 cull/winding을 따로 바꾼다. 0 벡터 금지, 입력은 정규화한다.", "1. vertexNormals 크기, inverse-transpose(world)의 방향, render lighting on을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "빛 반응은 뒤집히며 삼각형 앞면과 mount 방향은 유지된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblySurface.cpp", "} else if (command == \"normal\")", "조명 노멀과 winding 분리"}}},
        {"warp", "정적 외곽 그룹의 비균일·반사", "warp frame 1 1 1", "warp frame -1 1.5 1", "0 배율 금지. 음수 X는 반사다. 일반 모션은 자식 그룹에 붙인다. 물리 방은 강체 조건을 유지한다.", "1. world determinant 음수와 피킹/렌더 컬링 일치를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "자식 전체를 X반사·Y1.5배로 표시한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblySurface.cpp", "if (command == \"warp\")", "정적 외곽 그룹의 비균일·반사"}}},
        {"face-track", "지연·왕복 면 회전", "face-track fold box 4 2 0 once turn 0 0 1 -1 1 0 90", "face-track fold box 4 3 1 pingpong turn 0 0 1 -1 1 0 60", "시간/지연은 초, 각도는 도다. 시간 .0001초 이상. 같은 면 소유자는 하나다.", "1. seconds/지연/진행률u/faceTrackBase를 seek로 비교한다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "1초 대기 후3초 동안60도 열리고3초 동안 닫힌다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyTracks.cpp", "bool Assembly::trackCommand(", "지연·왕복 면 회전"}}},
        {"cpp-effect", "절대 시각으로 효과 계산", "// USER_COMMAND_BEGIN 아래에 pulse 명령 분기 없음", "if (args[0] == \"pulse\") {\n    require(args.size() == 3, \"pulse <동작이름> <대상>\");\n    Behavior b;\n    b.name = args[1]; b.target = args[2];\n    b.effect = Effect::Custom; b.positionDriver = true;\n    const auto id = scene.node(args[2]).id;\n    b.custom = [id](graphics::Scene& output, const graphics::Transform3D& rest, double seconds, float) {\n        auto pose = rest;\n        pose.scale *= float(1 + .25 * std::sin(seconds * 2));\n        return output.setTransform(id, pose);\n    };\n    scene.attach(std::move(b));\n    return true;\n}", "AssemblyExtensions.cpp의 customCommand 안에 삽입하고 상단에 <cmath>/<utility>를 include한다. 사용자 재빌드 후 pulse breathing hero를 입력한다. 이전 배율 누적 대신 기준 rest로 매 프레임 재계산한다. 함수 지역 pose는 콜백 내부에서 선언한다.", "1. seconds와 dt, pause 때 dt=0인지, 대상 SRT 소유자를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "배율이 기준의 .75~1.25 범위에서만 움직인다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExtensions.cpp", "// USER_COMMAND_BEGIN", "customCommand 내부에 새 분기 추가. <cmath>와 <utility>도 include"}, {"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Custom)", "편집 위치가 아닌 콜백 실행 중단점"}}},
        {"humanoid", "리그 전체 크기·비율 변경", "config.grid.frame = glm::translate(glm::mat4(1), position);", "config.grid.frame = glm::translate(glm::mat4(1), position);\nconfig.grid.extent = {1, 2.1f, .6f};", "GridLayout extent는 모델 치수이며 각 관절 연결도 함께 재계산한다.", "1. shoulders/elbows/hips/knees 부모와 grid.frame을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "리그 전체 키가 커지며 관절 계층을 유지한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "config.grid.frame =", "add humanoid 생성 설정: 이 줄 아래에 extent 추가"}}},
        {"swing", "그네 진폭·주기 변경", "config.pivot += position;", "config.pivot += position;\nconfig.pendulum.amplitudeDegrees = 40;\nconfig.pendulum.periodSeconds = 4;", "도와 초를 사용한다. 지지대는 frame, 흔들리는 좌석/사람은 hinge 아래에 둔다.", "1. frame/hinge/seat/rider 부모 및 PendulumMotion phase를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "지지대는 고정되고 좌석과 탑승자가 더 크게, 느리게 흔들린다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "config.pivot += position;", "add swing 생성 설정: 이 줄 아래에 진폭/주기 추가"}}},
        {"tank-preset", "기존 실습19/22 공유 탱크 치수", "{.65f,.6f,1.8f}", "{.8f,.6f,2.0f}", "공유 생성기를 고치면 실습19/22 모두 바뀐다. 0번만 바꾸려면 buildTankExample을 편집한다.", "1. lower/central/upper/guns/poles ID와 형상 부모를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "양쪽 궤도의 폭·길이가 함께 커진다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyParts.cpp", "TankParts addTankParts(", "기존 실습19/22 공유 탱크 치수"}}},
        {"walker", "충돌 보행 리그의 비율 맞추기", "grid.frame = glm::translate(glm::mat4(1), position);\n// AssemblyMotion.cpp\nb.character.height = 1.8f;", "grid.frame = glm::translate(glm::mat4(1), position);\ngrid.extent.y = 2.0f;\n// AssemblyMotion.cpp\nb.character.height = 2.0f;", "충돌 높이와 생성 grid.extent.y를 함께 맞춘다. 임의 사람 그룹은 JointRig 어댑터가 필요하다.", "1. feet/grounded/height/halfWidth와 어깨·엉덩이 ID를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "그림의 머리 높이와 천장 충돌 높이가 일치한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "grid.frame =", "walker 그림 높이: 이 줄 아래에 extent.y 추가"}, {"modes/AssemblyMotion.cpp", "b.character.height =", "충돌 높이: 생성 높이와 함께 변경"}}},
        {"grid", "격자 수와 셀 치수 변경", "grid.cells = {6, 1, 4};", "grid.cells = {8, 1, 4};", "셀 개수는 정수이며 origin/extent와 별개다. 인덱스/번호·경로도 재계산한다.", "1. cells와 cellCenter, 상대 크기, 이동 상자의 기본 선택을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 면적을 X방향8칸으로 나눈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "grid.cells =", "조립 add grid 설정값"}, {"modes/Mode7Grid.cpp", "grid.cells =", "독립/컨테이너 모드7 설정값"}}},
        {"room", "방의 표시·충돌 경계 함께 변경", "const graphics::Bounds3 bounds{{-2,0,-2},{2,3,2}};", "const graphics::Bounds3 bounds{{-3,0,-3},{3,4,3}};", "room mesh와 Node.room에 같은 bounds를 전달한다. 방은 강체, walker 방은 월드 축 정렬이다.", "1. room.room bounds, walls face5 visible, RoomBody.localPosition을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "넓이6·높이4 방에서 그림과 충돌 경계가 일치한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "} else if (kind == \"room\")", "방의 표시·충돌 경계 함께 변경"}}},
        {"patterns", "조합할 계층과 기능을 고르기", "build tank rover", "build truck carrier", "복합 객체를 큐브/관절/모션으로 분리한다. 예제 함수 변경은 재빌드가 필요하다.", "1. parts의 이름, 최상위 루트와 직접 편집할 예제 함수를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "별도 파일의 탱크·트럭·사람 예제로 이동한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"docs/assembly/PATTERN_COMBINATIONS.md", "# 기능 조합 패턴", "조합할 계층과 기능을 고르기"}}},
        {"motion", "이동과 자전의 소유권 분리", "path walk hero 5 8", "path walk hero 5 12", "초 단위 주기가 늘면 속도가 줄어든다. 경로+공전 등 위치 소유자는 같은 노드에 중복 부착하지 않는다.", "1. Behavior.target/positionDriver/seconds, rest와 Motion.sample을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를12초에 한 번 이동한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyMotion.cpp", "bool Assembly::motionCommand(", "이동과 자전의 소유권 분리"}}},
        {"face", "면 힌지 모서리·속도 변경", "face lid box 4 hinge 0 30", "face lid box 4 hinge 1 -15", "면/모서리는0부터, 속도는 도/초다. hinge축은 실제 모서리에서 구한다.", "1. surface 정점 순서와 모서리 양끝, 면 소유 ID를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "다른 모서리에서 반대 방향·절반 속도로 회전한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyMotion.cpp", "} else if (command == \"face\"", "면 힌지 모서리·속도 변경"}}},
        {"physics", "중력·탄성·이탈 진단", "body fall gravity 0 -9.8 0", "body fall gravity 0 -4.9 0", "중력은 월드 가속도다. 몸체 속도는 이탈 전 방 로컬, 이후 월드다. 큰 시간 간격을 피한다.", "1. released/localPosition/worldPosition/worldVelocity/previousRoom/halfExtent를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "낙하 가속도가 절반이 되며 바닥 이탈 후 월드 중력을 따른다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Body)", "중력·탄성·이탈 진단"}}},
        {"input", "기능별 입력과 해제 처리", "// USER_COMMAND_BEGIN 아래에 turn-key 명령 분기 없음", "if (args[0] == \"turn-key\") {\n    require(args.size() == 3, \"turn-key <동작이름> <대상>\");\n    Behavior input;\n    input.name = args[1]; input.target = args[2];\n    input.effect = Effect::Input;\n    input.key = [name = args[2]](Assembly& a, int key, int action, int) {\n        if (key != GLFW_KEY_H || action != GLFW_PRESS) return false;\n        a.rotate(name, {0,1,0}, 5);\n        return true;\n    };\n    scene.attach(std::move(input));\n    return true;\n}", "AssemblyExtensions.cpp의 customCommand 안에 삽입하고 <utility>를 include한다. 사용자 재빌드 후 turn-key steering hero를 입력한다. bool은 입력 소비 여부다. focus(false)에서 사용자 누름 상태도 해제한다.", "1. action PRESS/REPEAT/RELEASE, physicalMods, topologyRevision과 callback 수명을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "H 최초 누름만5도 회전하며 기본 입력으로 중복 전달하지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExtensions.cpp", "// USER_COMMAND_BEGIN", "customCommand 내부에 새 분기 추가. <utility>도 include"}, {"modes/AssemblyInput.cpp", "bool Assembly::dispatchInput(", "편집 위치가 아닌 입력 전달 중단점"}}},
        {"camera", "시점·주목점·투영 수정", "camera pose 4 3 6 0 1 0 0 1 0", "camera pose 8 5 10 0 1 0 0 1 0", "position과 target이 같으면 안 되고 up은 시선과 평행할 수 없다. feature camera 포트가 활성화되면 그 포트가 시점을 소유한다.", "1. position/target/up, near/far, viewport 및 DPI를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 주목점을 더 멀리서 본다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyInput.cpp", "if (a[1] == \"pose\")", "시점·주목점·투영 수정"}}},
        {"recipe", "기본 조립 목록 수정", "\"build tank rover\",", "\"build truck rover\",", "객체 이름을 재사용할 때 초기 clear 이후 순서를 지킨다. 이 C++ 배열 변경은 재빌드, 외부 .assembly 변경은 reload다.", "1. defaultRecipe의 실패 줄, 중복 이름, 입력 바인딩 대상 존재 여부를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "시작 구성에서 rover가 트럭으로 생성된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode0Recipe.cpp", "USER_RECIPE_BEGIN", "기본 조립 목록 수정"}}},
        {"custom", "사용자 조립 함수 등록", "scene.move(args[1], axis(args[2]) * float(number(args[3])));", "scene.move(args[1], axis(args[2]) * float(number(args[3])) * 2.0f);", "nudge의 거리 배율을 두 배로 하는 예다. 새 명령은 인자 수·수치·대상 존재 검사를 먼저 둔다.", "1. args token 수와 require 실패 위치, 명령 전후 rest.basis를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 nudge 거리 입력이 두 배 이동한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExtensions.cpp", "USER_COMMAND_BEGIN", "사용자 조립 함수 등록"}}},
        {"runtime", "합성 순서와 덮어쓰기 진단", "rest.rotation = rotation;", "rest.rotation = baseRotation * rotation;", "새로운 절대 자세가 사용자 기준값을 지우지 않게 합성한다. 도입한 행렬의 좌표 공간을 명시한다.", "1. 부모/로컬 MotionSample과 rest, 한 프레임 dt 호출 수를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "수동 회전을 유지하면서 자동 모션이 중첩된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyRuntime.cpp", "bool Assembly::applyPoses(", "합성 순서와 덮어쓰기 진단"}}},
        {"save", "파일 재로드·실패 복원", "load \"examples/assembly/tank.assembly\"", "hotload \"examples/assembly/workspace.assembly\"\nreload", "레시피는 현재 프레임 스냅샷이 아니다. 잘못된 파일은 outer execute가 CPU Scene/State를 복원한다.", "1. 실제 절대 경로, 실패 줄, CRLF 제거와 # @state-begin 경계를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "Ctrl+F5가 지정 파일을 다시 읽고 실패 시 기존 조립을 유지한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyPersistence.cpp", "bool Assembly::load(", "파일 재로드·실패 복원"}}},
        {"hotload", "조립 파일 핫로드와 코드 변경 구분", "hotload \"examples/assembly/workspace.assembly\"", "hotload \"my-scene.assembly\"", "Ctrl+F5는 외부 레시피 재로드다. C++ 소스 변경은 컴파일/링크 없이는 실행 코드가 바뀌지 않는다.", "1. reloadPath, load 실패 줄과 savedState, focus release를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "지정 파일의 구성으로 교체하고 실패 시 이전 구성을 보존한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode0Assembly.cpp", "key == GLFW_KEY_F5", "조립 파일 핫로드와 코드 변경 구분"}, {"modes/AssemblyCommands.cpp", "if (a[0] == \"reload\")", "재로드 진입"}}},
        {"feature", "기존 실습을 독립 컨테이너·라이브 포트로 조립", "scene.addFeature(\"a\", 9);", "scene.addFeature(\"a\", 9);\nscene.addFeature(\"b\", 16);", "1~22를 선택한다. add는 독립 시계, port는 원본과 공유 시계다. 라이브 형상은 원본 소유, 독립 편집은 snapshot이다.", "1. FeatureInstance.program/branch/events, private Scene과 exported face frame을 비교한다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "서로의 Scene.clear와 상태를 덮어쓰지 않고 두 실습을 함께 갱신한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyFeatures.cpp", "void Assembly::addFeature(", "기존 실습을 독립 컨테이너·라이브 포트로 조립"}, {"modes/FeatureContainer.cpp", "struct Context", "CPU 상태 교환/복원"}, {"modes/AssemblyFeatures.cpp", "void Assembly::syncFeatures(", "출력 포트와 추가 SRT 합성"}}},
        {"debug", "잘못된 입력 반응을 파이프라인별로 좁히기", "inspect hero", "diagnose hero", "diagnose는 등록된 실제 런타임 필드와 행렬을 출력한다. 임의 C++ 지역변수는 디버거에서 관찰한다.", "1. rest→parent→world→face frame→camera/viewport 순서로 첫 차이를 찾는다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "덮어쓰는 소유자, 잘못된 피벗/반사/뷰포트의 근거를 확인한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyDiagnostics.cpp", "void Assembly::diagnose(", "잘못된 입력 반응을 파이프라인별로 좁히기"}}},
        {"mode1", "모드1 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode1Ellipse.cpp", "path.durationSeconds =", "모드1 경로 주기 수정"}}},
        {"mode2", "모드2 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode2Rectangle.cpp", "path.durationSeconds =", "모드2 경로 주기 수정"}}},
        {"mode3", "모드3 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode3Polygon.cpp", "path.durationSeconds =", "모드3 경로 주기 수정"}}},
        {"mode4", "모드4 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode4Spatial.cpp", "path.durationSeconds =", "모드4 경로 주기 수정"}}},
        {"mode5", "모드5 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode5Circle.cpp", "path.durationSeconds =", "모드5 경로 주기 수정"}}},
        {"mode6", "모드6 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode6Polyline.cpp", "path.durationSeconds =", "모드6 경로 주기 수정"}}},
        {"mode7", "격자 셀 수 수정", "grid.cells = {6,1,4};", "grid.cells = {8,1,4};", "단위는 소스의 모델 좌표/도/초를 따른다. 실습 전용 값과 조립 외곽 SRT를 구분한다.", "1. 원본 Console 상태와 기능 포트의 추가 SRT를 각각 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "격자 셀 수이 변경되며 다른 실습 인스턴스에는 직접 전달되지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode7Grid.cpp", "grid.cells =", "격자 셀 수 수정"}}},
        {"mode8", "물리 상자 시작 높이 수정", "roomBody_.localPosition = {0,-.75f,0};", "roomBody_.localPosition = {0,.5f,0};", "단위는 소스의 모델 좌표/도/초를 따른다. 실습 전용 값과 조립 외곽 SRT를 구분한다.", "1. 원본 Console 상태와 기능 포트의 추가 SRT를 각각 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "물리 상자 시작 높이이 변경되며 다른 실습 인스턴스에는 직접 전달되지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode8Room.cpp", "roomBody_.localPosition =", "물리 상자 시작 높이 수정"}}},
        {"mode9", "그네 진폭 수정", "swingConfig.pendulum.amplitudeDegrees = 25;", "swingConfig.pendulum.amplitudeDegrees = 40;", "단위는 소스의 모델 좌표/도/초를 따른다. 실습 전용 값과 조립 외곽 SRT를 구분한다.", "1. 원본 Console 상태와 기능 포트의 추가 SRT를 각각 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "그네 진폭이 변경되며 다른 실습 인스턴스에는 직접 전달되지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode9Animation.cpp", "swingConfig.pendulum.amplitudeDegrees =", "그네 진폭 수정"}}},
        {"practice13", "실습13 표시 면 조합", "selectFaces(cuboidId_, {0});", "selectFaces(cuboidId_, {0,4});", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. 면 인덱스·visible와 난수 첫/두 번째 선택 중복\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "표시 면 조합 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode10Practice13.cpp", "void Practice13::selectFaces", "실습13 표시 면 조합"}}},
        {"practice14", "실습14 자전 속도", "const float angle = glm::radians(45.0f * deltaTime);", "const float angle = glm::radians(90.0f * deltaTime);", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. angularDirection/rotation, 입력 release와 dt\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "자전 속도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode11Practice14.cpp", "void Practice14::onUpdate", "실습14 자전 속도"}}},
        {"practice15", "실습15 면 축소 특이행렬 방지", "compose(1, scaleAround({0.0f, 0.0f, -HALF_SIZE}, 1.0f - progress));", "compose(1, scaleAround({0.0f, 0.0f, -HALF_SIZE}, std::max(.01f, 1.0f - progress)));", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. motions_.progress/direction/running와 한 면에 합성한 행렬 순서\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "면 축소 특이행렬 방지 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/FaceCommon.cpp", "void FaceMode::applyFaceMotion", "실습15 면 축소 특이행렬 방지"}}},
        {"practice16", "실습16 순차 펼침 시간 분할", "const auto active = std::size_t(pyramidProgress_ / 2.0f) % 4;\nconst float phase = std::fmod(pyramidProgress_, 2.0f);", "const auto active = std::size_t(pyramidProgress_ / 3.0f) % 4;\nconst float phase = std::fmod(pyramidProgress_, 3.0f) * (2.0f/3.0f);", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. active와 phase를 같은 주기3으로 함께 변경; atan2(h,b) 각도\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "순차 펼침 시간 분할 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode13Practice16.cpp", "void Practice16::applyPyramid", "실습16 순차 펼침 시간 분할"}}},
        {"practice17", "실습17 교환 우회 높이", "point.y += (i == 0 ? 1.0f : -1.0f) * .9f * std::sin(3.14159265f * t);", "point.y += (i == 0 ? 1.0f : -1.0f) * 1.4f * std::sin(3.14159265f * t);", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. starts_/motionOffsets_/elapsed와 수동 이동 보존\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "교환 우회 높이 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode14Practice17.cpp", "void Practice17::onUpdate", "실습17 교환 우회 높이"}}},
        {"practice18", "실습18 행성 궤도 반지름", "constexpr std::array<float, 3> radii{2.0f, 2.8f, 3.6f};", "constexpr std::array<float, 3> radii{2.4f, 3.2f, 4.0f};", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. 행성/달 부모 프레임, orbit angle, 궤도선 평면을 같은 값으로 맞춤\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "행성 궤도 반지름 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode15Practice18.cpp", "constexpr std::array<float, 3> radii", "실습18 행성 궤도 반지름"}}},
        {"practice19", "실습19 탱크 이동 속도", "position_ += glm::normalize(movement) * dt * 1.3f;", "position_ += glm::normalize(movement) * dt * 1.8f;", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. moving direction 재입력 토글, 전역 이동과 부품 SRT 분리\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "탱크 이동 속도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/TankCommon.cpp", "position_ += glm::normalize(movement)", "실습19 탱크 이동 속도"}}},
        {"practice20", "실습20 컨베이어 거리 속도", "constexpr double beltSpeed = 1.25;", "constexpr double beltSpeed = 2.0;", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. distance 누적과 PathPose.tangent/yaw; 경로 길이/상자 위상\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "컨베이어 거리 속도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode17Practice20.cpp", "constexpr double beltSpeed", "실습20 컨베이어 거리 속도"}}},
        {"practice21", "실습21 마우스 회전 감도", "(previousMouseX_ - x) * .3", "(previousMouseX_ - x) * .15", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. pendingRotation_/roomAngle_/previousMouseX, 각속도와 이탈\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "마우스 회전 감도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode18Practice21.cpp", "void Practice21::onCursor", "실습21 마우스 회전 감도"}}},
        {"practice22", "실습22 공유 탱크의3뷰 출력", "view 1", "view 3", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. 렌더 패스에서 onUpdate가 재호출되지 않는지; workspace의 view 설정\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "공유 탱크의3뷰 출력 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/TankCommon.cpp", "bool TankMode::onRender", "실습22 공유 탱크의3뷰 출력"}}},
        {"practice23", "실습23 보행 점프 속도", "graphics::CollisionSystem::jump(body_, 5.5f);", "graphics::CollisionSystem::jump(body_, 4.5f);", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. feet/grounded/verticalSpeed, body.height와 그림 리그 높이\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "보행 점프 속도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode20Practice23.cpp", "graphics::CollisionSystem::jump(body_,", "실습23 보행 점프 속도"}}},
        {"mode21", "카메라 충돌 반지름 변경", "glm::dot(difference,difference) <= .15f*.15f", "glm::dot(difference,difference) <= .2f*.2f", "충돌 반지름과 이동 substep 간격을 같이 바꾼다. 모드21 카메라 포트를 선택해야 카메라 동작이 보인다.", "1. closest/difference/movement/steps와 카메라 포트 소유자를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "상자에서 더 먼 지점에 카메라가 멈춘다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode21CameraCollision.cpp", "bool blocked(", "카메라 충돌 반지름 변경"}}},
        {"mode22", "내장 정점·UV·삼각형 오류 수정", "{0,.5f,0}", "{0,.8f,0}", "위치는 과제 범위 [-1,1], UV는 [0,1]을 유지한다. 예시는 정점 배열에서 꼭짓점 Y만 .5에서 .8로 변경한다. 오류가 나면 입력 인덱스를 고치고 검사 조건을 완화하지 않는다.", "1. errors의 최초 항목, 중복 코너, cross 면적과 UV 혼합 여부를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "피라미드 꼭짓점이 .3 높아진다. 입력 오류는 errors의 최초 항목에서 고치고 범위 검사 자체를 약화하지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode22EmbeddedMesh.cpp", "const std::vector<glm::vec3> positions", "내장 정점·UV·삼각형 오류 수정"}}},
    };
    return recipes;
}

const GuideRecipe* findGuideRecipe(std::string_view requested) {
    std::string topic(requested);
    const std::pair<const char*, const char*> aliases[]{
        {"group","hierarchy"},{"part","hierarchy"},{"joint","hierarchy"},{"anchor","mount"},{"unmount","mount"},
        {"add","objects"},{"parent","hierarchy"},{"color","objects"},{"show","objects"},{"winding","normal"},{"orient","normal"},
        {"triangle","geometry"},{"triangle-add","geometry"},{"surface","geometry"},{"vertex","geometry"},{"face-srt","geometry"},{"face-mode","geometry"},{"facecolor","geometry"},{"outline","geometry"},{"line","geometry"},{"digit","geometry"},
        {"spin","motion"},{"orbit","motion"},{"path","motion"},{"linear","motion"},{"pendulum","motion"},{"remove","motion"},{"wall","face"},
        {"walk","physics"},{"jump","physics"},{"obstacle","physics"},{"body","physics"},{"release","physics"},{"bind","input"},{"unbind","input"},{"drag","input"},{"effects","input"},{"render","input"},
        {"load","save"},{"reload","hotload"},{"diagnose","debug"},{"tank-pattern","tank"},{"truck-pattern","truck"},{"person-pattern","person"},{"container","feature"}};
    for (const auto& alias : aliases) if (topic == alias.first) { topic = alias.second; break; }
    for (int mode = 10; mode <= 20; ++mode) if (topic == "mode" + std::to_string(mode)) topic = "practice" + std::to_string(mode + 3);
    for (const auto& recipe : guideRecipes()) if (topic == recipe.topic) return &recipe;
    return nullptr;
}
} // namespace assembly
````

## standard/modes/GuideRecipes.h

가이드의 목적/코드 전후/제약/진단/소스 위치 스키마를 분리한다.

### before

없음.

### after

````cpp
#pragma once
#include <string_view>
#include <vector>

namespace assembly {
struct GuideSite {
    const char* file;
    const char* marker;
    const char* purpose;
};

struct GuideRecipe {
    const char* topic;
    const char* goal;
    const char* before;
    const char* after;
    const char* reason;
    const char* debug;
    const char* expected;
    const char* document;
    std::vector<GuideSite> sites;
};

const std::vector<GuideRecipe>& guideRecipes();
const GuideRecipe* findGuideRecipe(std::string_view topic);
} // namespace assembly
````

## standard/modes/Mode0Assembly.cpp

Ctrl+F5를 조립 레시피 reload에 연결한다.

### before

````cpp
#include "Assembly.h"
#include "AssemblyConsole.h"
#include <iostream>

namespace {
class AssemblyMode final : public PracticeMode {
    graphics::Application& app_;
    assembly::Assembly scene_;
    assembly::ConsoleInput input_;
public:
    explicit AssemblyMode(graphics::Application& app) : PracticeMode(app), app_(app), scene_(app) {}
    bool initScene() override {
        if (!assembly::defaultRecipe(scene_, std::cout))
            std::cout << "기본 레시피가 중단됐습니다. 콘솔에서 수정하거나 guide recipe를 사용하세요.\n";
        scene_.help(std::cout);
        input_.prompt();
        return true;
    }
    void onKey(int key, int action, int mods) override {
        if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
            assembly::defaultRecipe(scene_, std::cout);
            return;
        }
        scene_.onKey(key, action, mods, std::cout);
    }
    void onMouse(const graphics::MouseEvent& event) override { scene_.onMouse(event, std::cout); }
    void onCursor(double x, double y) override { scene_.onCursor(x, y, std::cout); }
    void onFocus(bool focused) override { scene_.onFocus(focused); }
    void onUpdate(float dt) override {
        for (unsigned i = 0; i < 8; ++i) {
            const auto line = input_.poll();
            if (!line) break;
            if (*line == "quit") {
                app_.requestClose();
                return;
            }
            scene_.execute(*line, std::cout);
            input_.prompt();
        }
        scene_.update(dt, std::cout);
    }
    bool onRender() override { return scene_.render(); }
};
}

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app) {
    return std::make_unique<AssemblyMode>(app);
}
````

### after

````cpp
#include "Assembly.h"
#include "AssemblyConsole.h"
#include <iostream>

namespace {
class AssemblyMode final : public PracticeMode {
    graphics::Application& app_;
    assembly::Assembly scene_;
    assembly::ConsoleInput input_;
public:
    explicit AssemblyMode(graphics::Application& app) : PracticeMode(app), app_(app), scene_(app) {}
    bool initScene() override {
        if (!assembly::defaultRecipe(scene_, std::cout))
            std::cout << "기본 레시피가 중단됐습니다. 콘솔에서 수정하거나 guide recipe를 사용하세요.\n";
        scene_.help(std::cout);
        input_.prompt();
        return true;
    }
    bool capturesNumericKeys() const override { return scene_.capturesNumericKeys(); }
    void onKey(int key, int action, int mods) override {
        if (key == GLFW_KEY_F5 && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
            scene_.execute("reload", std::cout);
            return;
        }
        if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
            assembly::defaultRecipe(scene_, std::cout);
            return;
        }
        scene_.onKey(key, action, mods, std::cout);
    }
    void onMouse(const graphics::MouseEvent& event) override { scene_.onMouse(event, std::cout); }
    void onCursor(double x, double y) override { scene_.onCursor(x, y, std::cout); }
    void onFocus(bool focused) override { scene_.onFocus(focused); }
    void onUpdate(float dt) override {
        for (unsigned i = 0; i < 8; ++i) {
            const auto line = input_.poll();
            if (!line) break;
            if (*line == "quit") {
                app_.requestClose();
                return;
            }
            scene_.execute(*line, std::cout);
            input_.prompt();
        }
        scene_.update(dt, std::cout);
    }
    bool onRender() override { return scene_.render(); }
};
}

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app) {
    return std::make_unique<AssemblyMode>(app);
}
````

## standard/modes/PracticeMode.h

실습 입력 포커스가 숫자를 소비하는지 상위 Console에 전달한다.

### before

````cpp
#pragma once
#include "../framework/Framework.h"
#include <memory>

// Each practice owns its state; Console alone owns callback registration and switching.
class PracticeMode {
    graphics::Application& host_;
public:
    explicit PracticeMode(graphics::Application& app) : host_(app) {}
    virtual ~PracticeMode() = default;
    virtual bool initScene() = 0;
    virtual void onKey(int, int, int) = 0;
    virtual void onUpdate(float) {}
    virtual void onMouse(const graphics::MouseEvent&) {}
    virtual void onCursor(double, double) {}
    virtual void onFocus(bool) {}
    virtual bool onRender() { return host_.renderScene(); }
};

constexpr int lastStandardMode = 22;
std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice20(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice21(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app);
std::unique_ptr<PracticeMode> makeCameraCollision(graphics::Application& app);
std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app);

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app);
````

### after

````cpp
#pragma once
#include "../framework/Framework.h"
#include <memory>

// Each practice owns its state; Console alone owns callback registration and switching.
class PracticeMode {
    graphics::Application& host_;
public:
    explicit PracticeMode(graphics::Application& app) : host_(app) {}
    virtual ~PracticeMode() = default;
    virtual bool initScene() = 0;
    virtual void onKey(int, int, int) = 0;
    virtual void onUpdate(float) {}
    virtual void onMouse(const graphics::MouseEvent&) {}
    virtual void onCursor(double, double) {}
    virtual void onFocus(bool) {}
    virtual bool capturesNumericKeys() const { return false; }
    virtual bool onRender() { return host_.renderScene(); }
};

constexpr int lastStandardMode = 22;
std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice20(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice21(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app);
std::unique_ptr<PracticeMode> makeCameraCollision(graphics::Application& app);
std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app);

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app);
````

## standard/standard.vcxproj

사용자 빌드 시 레시피와 하위 가이드 문서/그래프를 재귀 배포한다.

### before

````xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{AD50DC3F-7731-4954-923F-907BB2A8277C}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <ProjectName>standard</ProjectName>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <Import Project="framework\Toolchain.props" />
  <PropertyGroup Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries Condition="'$(Configuration)'=='Debug'">true</UseDebugLibraries>
    <UseDebugLibraries Condition="'$(Configuration)'=='Release'">false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <OutDir>$(ProjectDir)x64\$(Configuration)\</OutDir>
    <IntDir>$(ProjectDir)x64\$(Configuration)\obj\</IntDir>
    <TargetName>standard</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <WarningLevel>Level4</WarningLevel>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebug</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreaded</RuntimeLibrary>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="modes\*.cpp" />
    <ClInclude Include="modes\*.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Animation.h" />
    <ClInclude Include="framework\include\Transform.h" />
    <ClInclude Include="framework\include\Patterns.h" />
    <ClInclude Include="framework\include\GridLayout.h" />
    <ClInclude Include="framework\include\Diagnostics.h" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ItemGroup>
    <AssemblyRecipe Include="examples\assembly\**\*.assembly" />
    <AssemblyDocument Include="docs\assembly\**\*.md" />
  </ItemGroup>
  <Target Name="CopyAssemblyLearningFiles" AfterTargets="Build">
    <Copy SourceFiles="@(AssemblyRecipe)" DestinationFiles="@(AssemblyRecipe->'$(TargetDir)examples\assembly\%(RecursiveDir)%(Filename)%(Extension)')" SkipUnchangedFiles="true" />
    <Copy SourceFiles="@(AssemblyDocument)" DestinationFiles="@(AssemblyDocument->'$(TargetDir)docs\assembly\%(RecursiveDir)%(Filename)%(Extension)')" SkipUnchangedFiles="true" />
  </Target>
</Project>
````

### after

````xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{AD50DC3F-7731-4954-923F-907BB2A8277C}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <ProjectName>standard</ProjectName>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <Import Project="framework\Toolchain.props" />
  <PropertyGroup Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries Condition="'$(Configuration)'=='Debug'">true</UseDebugLibraries>
    <UseDebugLibraries Condition="'$(Configuration)'=='Release'">false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <OutDir>$(ProjectDir)x64\$(Configuration)\</OutDir>
    <IntDir>$(ProjectDir)x64\$(Configuration)\obj\</IntDir>
    <TargetName>standard</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <WarningLevel>Level4</WarningLevel>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebug</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreaded</RuntimeLibrary>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="modes\*.cpp" />
    <ClInclude Include="modes\*.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Animation.h" />
    <ClInclude Include="framework\include\Transform.h" />
    <ClInclude Include="framework\include\Patterns.h" />
    <ClInclude Include="framework\include\GridLayout.h" />
    <ClInclude Include="framework\include\Diagnostics.h" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ItemGroup>
    <AssemblyRecipe Include="examples\assembly\**\*.assembly" />
    <AssemblyDocument Include="docs\assembly\**\*.md;docs\assembly\**\*.html;docs\assembly\**\*.json" />
  </ItemGroup>
  <Target Name="CopyAssemblyLearningFiles" AfterTargets="Build">
    <Copy SourceFiles="@(AssemblyRecipe)" DestinationFiles="@(AssemblyRecipe->'$(TargetDir)examples\assembly\%(RecursiveDir)%(Filename)%(Extension)')" SkipUnchangedFiles="true" />
    <Copy SourceFiles="@(AssemblyDocument)" DestinationFiles="@(AssemblyDocument->'$(TargetDir)docs\assembly\%(RecursiveDir)%(Filename)%(Extension)')" SkipUnchangedFiles="true" />
  </Target>
</Project>
````

## tools/Update-StandardModesGraph.py

기존 추출 절차에 실제 소유 standard API 구현 두 개를 추가한다.

### before

````python
"""Generate a code-only structure graph. Does not build, test, or run diagnostics."""
from pathlib import Path
import argparse
import datetime
import hashlib
import json
import re
import subprocess
import xml.etree.ElementTree as ET

from graphify.extract import extract
from graphify.build import build_from_json
from graphify.cluster import cluster
from graphify.export import to_json, to_html

ROOT = Path(__file__).resolve().parent.parent
_parser = argparse.ArgumentParser(description=__doc__)
_parser.add_argument('--output', default='graphify-out/standard-input')
_args = _parser.parse_args()
OUT = (ROOT / _args.output).resolve()
OUT.relative_to(ROOT)


def write(name, value):
    (OUT / name).write_text(value.rstrip() + '\n', encoding='utf-8-sig')


def dump(name, value):
    write(name, json.dumps(value, ensure_ascii=False, indent=2))


def inventory():
    projects = []
    ns = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}
    tracked = subprocess.check_output(['git', 'ls-files', '*.vcxproj'], cwd=ROOT, text=True, encoding='utf-8')
    historical = json.loads((ROOT / 'graphify-out/PROJECTS.json').read_text(encoding='utf-8-sig'))
    for relative in tracked.splitlines():
        path = ROOT / relative
        if not path.is_file():
            prior = next((p for p in historical if p.get('project') == relative), {})
            projects.append({**prior, 'project': relative, 'available': False,
                             'note': 'Absent before this task. Entry information is historical; no file was restored or deleted.'})
            continue
        sources = set()
        for element in ET.parse(path).findall('.//m:ClCompile', ns):
            include = element.get('Include')
            if include and '$(' not in include:
                sources.update(p.resolve() for p in path.parent.glob(include.replace('\\', '/')) if p.is_file())
        bundled = path.parent / 'framework/detail'
        if bundled.is_dir():
            sources.update(p.resolve() for p in bundled.glob('*.cpp'))
        entries = []
        for source in sorted(sources):
            text = source.read_text(encoding='utf-8-sig')
            for match in re.finditer(r'\b(?:int|void)\s+(main|wmain)\s*\(', text):
                entries.append({'file': source.relative_to(ROOT).as_posix(), 'name': match[1],
                                'line': text[:match.start()].count('\n') + 1,
                                'conditional_compilation': 'Review preprocessor branches in this source.' if '#if' in text else None})
        projects.append({'project': relative, 'available': True,
                         'sources': [p.relative_to(ROOT).as_posix() for p in sorted(sources)], 'entrypoints': entries})
    for name in ('basic', 'assets', 'motion'):
        projects.append({'standalone_example': True, 'file': f'framework/examples/{name}.cpp', 'entrypoint': 'main'})
    return projects


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    excluded = {'.git', '.vs', '.agents', '.codex', 'dependencies', 'packages', 'archive',
                'x64', 'Debug', 'Release', '.obj-work', 'dist', 'tests'}
    files = []
    for path in ROOT.rglob('*'):
        rel = path.relative_to(ROOT)
        if path.suffix.lower() not in {'.cpp', '.h', '.hpp'} or any(p in excluded for p in rel.parts):
            continue
        if 'framework' in rel.parts and rel.parts[0] != 'framework':
            continue
        files.append(path)
    # Include the two owning standard copies changed in this task, rather than every SDK distribution.
    files += [ROOT / 'standard/framework/include/Picking.h', ROOT / 'standard/framework/detail/Application.cpp']
    files = sorted(set(files))
    extraction = extract(files, cache_root=OUT, root=ROOT)
    node_sources = {n['id']: n.get('source_file', '').replace('\\', '/') for n in extraction['nodes']}
    for edge in extraction['edges']:
        a = node_sources.get(edge.get('source'), '').split('/')[0]
        b = node_sources.get(edge.get('target'), '').split('/')[0]
        if a and b and a != b and b != 'framework' and edge.get('relation') == 'calls':
            edge['confidence'] = 'AMBIGUOUS'
            edge['audit_note'] = 'Independent executables can have identically named classes. Confirm against owning sources.'
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    communities = cluster(graph)
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    to_json(graph, communities, str(OUT / 'graph.json'), force=True, built_at_commit=commit)
    to_html(graph, communities, str(OUT / 'graph.html'))
    for name in ('graph.json', 'graph.html'):
        write(name, (OUT / name).read_text(encoding='utf-8-sig'))
    dump('EXTRACTION.json', extraction)
    dump('PROJECTS.json', inventory())
    dump('SOURCE_MANIFEST.json', {
        'date': datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).date().isoformat(), 'source_commit': commit,
        'method': 'graphify AST; no semantic API, build, tests, app run, or extraction diagnostics',
        'nodes': graph.number_of_nodes(), 'edges': graph.number_of_edges(),
        'files': {p.relative_to(ROOT).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest() for p in files},
        'limits': ['Dynamic callback and virtual-dispatch order requires source review.',
                   'SDKs, tests, archives and duplicate framework distributions are excluded.',
                   'Missing pre-existing projects remain historical entries.']})
    print(f'graphify AST: {len(files)} files, {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges. No tests or diagnostics run.')


if __name__ == '__main__':
    main()
````

### after

````python
"""Generate a code-only structure graph. Does not build, test, or run diagnostics."""
from pathlib import Path
import argparse
import datetime
import hashlib
import json
import re
import subprocess
import xml.etree.ElementTree as ET

from graphify.extract import extract
from graphify.build import build_from_json
from graphify.cluster import cluster
from graphify.export import to_json, to_html

ROOT = Path(__file__).resolve().parent.parent
_parser = argparse.ArgumentParser(description=__doc__)
_parser.add_argument('--output', default='graphify-out/standard-input')
_args = _parser.parse_args()
OUT = (ROOT / _args.output).resolve()
OUT.relative_to(ROOT)


def write(name, value):
    (OUT / name).write_text(value.rstrip() + '\n', encoding='utf-8-sig')


def dump(name, value):
    write(name, json.dumps(value, ensure_ascii=False, indent=2))


def inventory():
    projects = []
    ns = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}
    tracked = subprocess.check_output(['git', 'ls-files', '*.vcxproj'], cwd=ROOT, text=True, encoding='utf-8')
    historical = json.loads((ROOT / 'graphify-out/PROJECTS.json').read_text(encoding='utf-8-sig'))
    for relative in tracked.splitlines():
        path = ROOT / relative
        if not path.is_file():
            prior = next((p for p in historical if p.get('project') == relative), {})
            projects.append({**prior, 'project': relative, 'available': False,
                             'note': 'Absent before this task. Entry information is historical; no file was restored or deleted.'})
            continue
        sources = set()
        for element in ET.parse(path).findall('.//m:ClCompile', ns):
            include = element.get('Include')
            if include and '$(' not in include:
                sources.update(p.resolve() for p in path.parent.glob(include.replace('\\', '/')) if p.is_file())
        bundled = path.parent / 'framework/detail'
        if bundled.is_dir():
            sources.update(p.resolve() for p in bundled.glob('*.cpp'))
        entries = []
        for source in sorted(sources):
            text = source.read_text(encoding='utf-8-sig')
            for match in re.finditer(r'\b(?:int|void)\s+(main|wmain)\s*\(', text):
                entries.append({'file': source.relative_to(ROOT).as_posix(), 'name': match[1],
                                'line': text[:match.start()].count('\n') + 1,
                                'conditional_compilation': 'Review preprocessor branches in this source.' if '#if' in text else None})
        projects.append({'project': relative, 'available': True,
                         'sources': [p.relative_to(ROOT).as_posix() for p in sorted(sources)], 'entrypoints': entries})
    for name in ('basic', 'assets', 'motion'):
        projects.append({'standalone_example': True, 'file': f'framework/examples/{name}.cpp', 'entrypoint': 'main'})
    return projects


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    excluded = {'.git', '.vs', '.agents', '.codex', 'dependencies', 'packages', 'archive',
                'x64', 'Debug', 'Release', '.obj-work', 'dist', 'tests'}
    files = []
    for path in ROOT.rglob('*'):
        rel = path.relative_to(ROOT)
        if path.suffix.lower() not in {'.cpp', '.h', '.hpp'} or any(p in excluded for p in rel.parts):
            continue
        if 'framework' in rel.parts and rel.parts[0] != 'framework':
            continue
        files.append(path)
    # Include the owning standard API copies changed in this task, rather than every SDK distribution.
    files += [ROOT / 'standard/framework/include/Picking.h', ROOT / 'standard/framework/detail/Application.cpp',
              ROOT / 'standard/framework/include/Graphics.h', ROOT / 'standard/framework/detail/Scene.cpp']
    files = sorted(set(files))
    extraction = extract(files, cache_root=OUT, root=ROOT)
    node_sources = {n['id']: n.get('source_file', '').replace('\\', '/') for n in extraction['nodes']}
    for edge in extraction['edges']:
        a = node_sources.get(edge.get('source'), '').split('/')[0]
        b = node_sources.get(edge.get('target'), '').split('/')[0]
        if a and b and a != b and b != 'framework' and edge.get('relation') == 'calls':
            edge['confidence'] = 'AMBIGUOUS'
            edge['audit_note'] = 'Independent executables can have identically named classes. Confirm against owning sources.'
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    communities = cluster(graph)
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    to_json(graph, communities, str(OUT / 'graph.json'), force=True, built_at_commit=commit)
    to_html(graph, communities, str(OUT / 'graph.html'))
    for name in ('graph.json', 'graph.html'):
        write(name, (OUT / name).read_text(encoding='utf-8-sig'))
    dump('EXTRACTION.json', extraction)
    dump('PROJECTS.json', inventory())
    dump('SOURCE_MANIFEST.json', {
        'date': datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).date().isoformat(), 'source_commit': commit,
        'method': 'graphify AST; no semantic API, build, tests, app run, or extraction diagnostics',
        'nodes': graph.number_of_nodes(), 'edges': graph.number_of_edges(),
        'files': {p.relative_to(ROOT).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest() for p in files},
        'limits': ['Dynamic callback and virtual-dispatch order requires source review.',
                   'SDKs, tests, archives and duplicate framework distributions are excluded.',
                   'Missing pre-existing projects remain historical entries.']})
    print(f'graphify AST: {len(files)} files, {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges. No tests or diagnostics run.')


if __name__ == '__main__':
    main()
````

## tools/Package-Standard.py

원본 삭제 없이 소스/SDK/문서 ZIP과 포함/제외·크기·해시 영수증을 생성한다.

### before

없음.

### after

````python
"""Create the source-only standard ZIP; no build, app run, tests or validators."""
from pathlib import Path
import argparse
import datetime
import hashlib
import json
import subprocess
import zipfile

ROOT = Path(__file__).resolve().parent.parent
SOURCE = ROOT / 'standard'
LIMIT = 30_000_000


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', default='dist/standard-composition-20261103.zip')
    args = parser.parse_args()
    output = (ROOT / args.output).resolve()
    output.relative_to(ROOT)
    if output.is_relative_to(SOURCE.resolve()):
        raise SystemExit('Output must be outside the source tree.')
    if output.exists():
        raise SystemExit('Output already exists. Choose a new --output path; existing artifacts are preserved.')
    included, excluded = [], []
    for path in sorted(SOURCE.rglob('*')):
        if not path.is_file():
            continue
        relative = path.relative_to(SOURCE)
        reason = None
        if relative.parts[0] in {'.vs', 'x64', '.git', '__pycache__'}:
            reason = 'IDE cache/build output; source, bundled SDK and licences remain included'
        elif path.name.endswith('.vcxproj.user') or path.suffix in {'.pyc', '.tmp'}:
            reason = 'Device-local or temporary file'
        elif not path.resolve().is_relative_to(SOURCE.resolve()):
            reason = 'Outside source tree link'
        entry = {'path': relative.as_posix(), 'bytes': path.stat().st_size}
        if reason:
            excluded.append({**entry, 'reason': reason})
        else:
            entry['sha256'] = hashlib.sha256(path.read_bytes()).hexdigest()
            included.append(entry)
    raw = sum(p['bytes'] for p in included)
    if raw >= LIMIT:
        raise SystemExit(f'Source exceeds 30MB before compression: {raw}. Review packaging scope; no files were deleted.')
    output.parent.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(output, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for entry in included:
            archive.write(SOURCE / entry['path'], 'standard/' + entry['path'])
    size = output.stat().st_size
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    result = {
        'date': datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).isoformat(),
        'source_commit': commit, 'source': str(SOURCE), 'archive': str(output),
        'raw_bytes': raw, 'zip_bytes': size, 'limit_bytes': LIMIT,
        'archive_sha256': hashlib.sha256(output.read_bytes()).hexdigest(),
        'included': included, 'excluded': excluded,
        'notes': ['Current working source includes preserved user changes.',
                  'Bundled SDK x64 libraries are included; only standard/x64 build output is excluded.',
                  'No source, cache, output, backup or user file was deleted.',
                  'No build, app run, tests, archive testzip, or validators were run.']}
    output.with_suffix('.manifest.json').write_text(json.dumps(result, ensure_ascii=False, indent=2)+'\n', encoding='utf-8-sig')
    report = ['# standard source package size', '', f'Source commit: `{commit}`. Current working source, including preserved user changes.', '',
              f'- Included files: {len(included)}', f'- Uncompressed: {raw:,} bytes ({raw / 1_000_000:.2f} MB)',
              f'- ZIP: {size:,} bytes ({size / 1_000_000:.2f} MB)', f'- Limit: {LIMIT:,} bytes',
              f'- Excluded cache/build bytes: {sum(e["bytes"] for e in excluded):,}',
              '- Deleted bytes: 0', '- Build/runtime/tests: delegated to user', '',
              'The adjacent manifest lists every included/excluded path, size and included-file SHA-256. The ZIP is a C++ source project; the old executable is not included.']
    output.with_suffix('.SIZE.md').write_text('\n'.join(report)+'\n',encoding='utf-8-sig')
    print(json.dumps({k:result[k] for k in ('archive','raw_bytes','zip_bytes','limit_bytes','archive_sha256')}, ensure_ascii=False))
    if size >= LIMIT:
        raise SystemExit('ZIP exceeds 30MB. Artifact retained for review; do not report delivery success.')


if __name__ == '__main__':
    main()
````

## standard/examples/assembly/modes/mode01.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드1 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 1
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode02.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드2 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 2
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode03.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드3 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 3
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode04.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드4 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 4
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode05.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드5 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 5
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode06.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드6 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 6
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode07.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드7 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 7
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode08.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드8 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 8
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode09.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드9 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 9
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode10.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드10 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 10
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode11.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드11 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 11
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode12.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드12 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 12
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode13.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드13 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 13
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode14.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드14 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 14
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode15.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드15 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 15
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode16.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드16 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 16
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode17.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드17 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 17
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode18.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드18 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 18
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode19.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드19 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 19
feature focus sample
feature camera sample
view 3
````

## standard/examples/assembly/modes/mode20.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드20 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 20
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode21.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드21 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 21
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/modes/mode22.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모드22 원본 상태를 기능 컨테이너로 조립
clear
feature add sample 22
feature focus sample
feature camera sample
view 1
````

## standard/examples/assembly/workspace.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# Ctrl+F5 기본 작업공간. 이 파일을 편집하고 다시 Ctrl+F5.
clear
feature add swingLab 9
move swingLab -5 0 0
feature add tankLab 16
move tankLab 5 0 0
camera pose 18 11 24 0 1 0 0 1 0
feature focus none
feature camera none
view 1
````

## standard/examples/assembly/all-modes.assembly

기존 실습의 기능/입력/카메라를 연결하는 조립 레시피 예제를 제공한다.

### before

없음.

### after

````text
# 모든1~22 모드를 한 장면에 배치. 성능/시각 확인은 사용자 수행.
clear
feature add m01 1
move m01 0 0 0
feature add m02 2
move m02 10 0 0
feature add m03 3
move m03 20 0 0
feature add m04 4
move m04 30 0 0
feature add m05 5
move m05 40 0 0
feature add m06 6
move m06 0 0 10
feature add m07 7
move m07 10 0 10
feature add m08 8
move m08 20 0 10
feature add m09 9
move m09 30 0 10
feature add m10 10
move m10 40 0 10
feature add m11 11
move m11 0 0 20
feature add m12 12
move m12 10 0 20
feature add m13 13
move m13 20 0 20
feature add m14 14
move m14 30 0 20
feature add m15 15
move m15 40 0 20
feature add m16 16
move m16 0 0 30
feature add m17 17
move m17 10 0 30
feature add m18 18
move m18 20 0 30
feature add m19 19
move m19 30 0 30
feature add m20 20
move m20 40 0 30
feature add m21 21
move m21 0 0 40
feature add m22 22
move m22 10 0 40
feature focus none
feature camera none
camera pose 45 42 75 20 0 20 0 1 0
camera clip 0.1 200
view 1
````
