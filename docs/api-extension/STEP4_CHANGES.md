# 4단계: 면 변환 모드와 회전 분류

기존 independence는 면 SRT 활성화였다. 변환 상속을 별도 enum으로 분리하고 현재 자세를 유지하는 기준 행렬을 보관한다.

입력: 모드/회전 종류/모서리/축/절대 각도 → 상태: Mesh의 frame/SRT/추가 행렬 → 출력: 렌더러와 피킹의 faceWorldMatrix.

공전은 A×S로 적용해 면이 이동해도 원본 모델 중심을 돈다. 초기 모션 연결 뒤 독립 기준을 고정한다. 기존 API의 기본 S×A는 보존한다.

[전체 before/after](ALL_CODE_BEFORE_AFTER.md) · [사용법·사용자 확인](../../framework/docs/FACE_TRANSFORMS.md)

```diff
diff --git a/framework/detail/CuboidMode.cpp b/framework/detail/CuboidMode.cpp
index 4aedc86..97cebf6 100644
--- a/framework/detail/CuboidMode.cpp
+++ b/framework/detail/CuboidMode.cpp
@@ -3,12 +3,20 @@
 
 namespace graphics {
 namespace {
+bool validFaceRotation(const std::optional<FaceRotationConfig>& rotation) {
+    if (!rotation) return true;
+    if (!std::isfinite(rotation->degrees)) return false;
+    if (rotation->kind == FaceRotationKind::EdgeHinge) return rotation->edge < 4;
+    return (rotation->kind == FaceRotationKind::SelfSpin || rotation->kind == FaceRotationKind::ModelOrbit) &&
+           detail::finiteVector(rotation->axis) && glm::length(glm::dvec3(rotation->axis)) > 1e-12;
+}
 bool validCuboid(const CuboidConfig& c) {
     if (c.name.empty() || !detail::finiteVector(c.size) || c.size.x <= 0 || c.size.y <= 0 || c.size.z <= 0 ||
         !detail::validColor(c.color) || !DiagramSystem::validTransform(c.transform))
         return false;
     for (const auto& face : c.faces)
-        if (!DiagramSystem::validTransform(face.transform) || !detail::validColor(face.color) ||
+        if (!DiagramSystem::validTransform(face.transform) || !detail::validColor(face.color) || !validFaceRotation(face.rotation) ||
+            (face.mode != FaceTransformMode::Hierarchy && face.mode != FaceTransformMode::Independent) ||
             (face.cull != CullMode::Inherit && face.cull != CullMode::None &&
              face.cull != CullMode::Back && face.cull != CullMode::Front))
             return false;
@@ -82,9 +90,10 @@ bool CuboidMode::apply(Application& app) {
         if (!id || !staged.setTransform(*id, c.transform) || !staged.setTint(*id, c.color) ||
             !staged.setVisible(*id, c.visible)) return false;
         for (std::size_t face = 0; face < c.faces.size(); ++face)
-            if (!staged.setFaceTransform(*id, face, c.faces[face].transform, false) ||
+            if (!staged.setFaceTransform(*id, face, c.faces[face].transform, true) ||
                 !staged.setFaceVisible(*id, face, c.faces[face].visible) ||
-                !staged.setFaceCullMode(*id, face, c.faces[face].cull)) return false;
+                !staged.setFaceCullMode(*id, face, c.faces[face].cull) ||
+                (c.faces[face].rotation && !PatternSystem::rotateFace(staged, *id, face, *c.faces[face].rotation))) return false;
         entities.emplace(c.name, *id);
         if (c.motion) {
             const auto motion = staged.getMotions().createMotion(*c.motion);
@@ -96,6 +105,9 @@ bool CuboidMode::apply(Application& app) {
             layers.emplace(c.name, *idLayer);
             motions.emplace(c.name, *motion);
         }
+        // Freeze independent faces only after the initial motion/layer binding exists.
+        for (std::size_t face = 0; face < c.faces.size(); ++face)
+            if (!staged.setFaceTransformMode(*id, face, c.faces[face].mode)) return false;
     }
     app.getScene() = std::move(staged);
     app.getCamera() = config_.camera;
diff --git a/framework/detail/Patterns.cpp b/framework/detail/Patterns.cpp
index 4315637..3a42ae8 100644
--- a/framework/detail/Patterns.cpp
+++ b/framework/detail/Patterns.cpp
@@ -592,6 +592,38 @@ bool PatternSystem::addNumber(Scene& scene, unsigned digit, const glm::vec3& pos
     return true;
 }
 
+bool PatternSystem::rotateFace(Scene& scene, EntityId entity, std::size_t face, const FaceRotationConfig& config) {
+    const auto* component = scene.getComponent(entity);
+    const auto* mesh = scene.getMesh(entity, face);
+    if (!component || !mesh || mesh->localVertices.size() < 3 || !std::isfinite(config.degrees))
+        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "rotateFace",
+                                   "Need a valid polygon face and finite angle.");
+    const auto srt = detail::faceSrtMatrix(*mesh);
+    auto point = [&](const glm::vec3& vertex) { return glm::vec3(srt * glm::vec4(vertex, 1)); };
+    AxisLine axis;
+    switch (config.kind) {
+    case FaceRotationKind::EdgeHinge:
+        if (config.edge >= mesh->localVertices.size()) return false;
+        axis.point = point(mesh->localVertices[config.edge]);
+        axis.direction = point(mesh->localVertices[(config.edge + 1) % mesh->localVertices.size()]) - axis.point;
+        break;
+    case FaceRotationKind::SelfSpin:
+        for (const auto& vertex : mesh->localVertices) axis.point += point(vertex);
+        axis.point /= float(mesh->localVertices.size());
+        axis.direction = config.axis;
+        break;
+    case FaceRotationKind::ModelOrbit: {
+        const auto bounds = detail::bounds(component->polyhedron);
+        axis.point = bounds.first*.5f + bounds.second*.5f;
+        axis.direction = config.axis;
+        break;
+    }
+    default: return false;
+    }
+    const auto matrix = TransformSystem::rotation(axis, config.degrees);
+    return matrix && scene.setFaceLocalMatrix(entity, face, *matrix, FaceAnimationSpace::Model);
+}
+
 bool PatternSystem::foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees) {
     const auto matrix = TransformSystem::rotation(hinge, degrees);
 
diff --git a/framework/detail/Scene.cpp b/framework/detail/Scene.cpp
index 631b05e..85c89d3 100644
--- a/framework/detail/Scene.cpp
+++ b/framework/detail/Scene.cpp
@@ -198,14 +198,16 @@ bool Scene::setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode) {
     return true;
 }
 
-bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix) {
-    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix)) {
+bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix, FaceAnimationSpace space) {
+    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix) ||
+        (space != FaceAnimationSpace::Local && space != FaceAnimationSpace::Model)) {
         return Diagnostics::failAt(
             __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceLocalMatrix",
             "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix)",
             "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
     }
     components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
+    components_[id].polyhedron.faces[faceIndex].animationSpace = space;
 
     return true;
 }
@@ -401,7 +403,9 @@ std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceInd
                 "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
             std::nullopt);
 
-    const auto world = worldMatrix(id);
+    const auto& face = components_[id].polyhedron.faces[faceIndex];
+    const auto world = face.transformMode == FaceTransformMode::Hierarchy
+                           ? worldMatrix(id) : std::optional<glm::mat4>(glm::mat4(1));
 
     if (!world)
         return (Diagnostics::failAt(
@@ -409,13 +413,7 @@ std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceInd
                     "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                 std::nullopt);
 
-    const auto& face = components_[id].polyhedron.faces[faceIndex];
-    glm::mat4 child(1);
-
-    if (face.independence)
-        child = glm::translate(glm::mat4(1), face.transform.localBasis) * detail::transformMatrix(face.transform);
-
-    const auto result = *world * child * face.localAnimationMatrix;
+    const auto result = *world * face.transformFrame * detail::faceLocalMatrix(face);
 
     if (!detail::finiteMatrix(result)) {
         Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Scene::faceWorldMatrix",
@@ -490,6 +488,32 @@ bool Scene::setTransform(EntityId id, const Transform3D& transform) {
     return true;
 }
 
+bool Scene::setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld) {
+    if (!getMesh(id, faceIndex) || (mode != FaceTransformMode::Hierarchy && mode != FaceTransformMode::Independent))
+        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
+                                   "Invalid entity, face index or transform mode.");
+    auto& face = components_[id].polyhedron.faces[faceIndex];
+    glm::mat4 frame(1);
+    if (preserveWorld) {
+        if (mode == face.transformMode) return true;
+        const auto world = worldMatrix(id);
+        if (!world) return false;
+        const auto oldParent = face.transformMode == FaceTransformMode::Hierarchy ? *world : glm::mat4(1);
+        frame = oldParent * face.transformFrame;
+        if (mode == FaceTransformMode::Hierarchy) {
+            const float determinant = glm::determinant(*world);
+            if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
+                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
+                                           "Cannot preserve world pose under a singular parent transform.");
+            frame = glm::inverse(*world) * frame;
+        }
+    }
+    if (!TransformSystem::validAffine(frame)) return false;
+    face.transformMode = mode;
+    face.transformFrame = frame;
+    return true;
+}
+
 bool Scene::setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence) {
     if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() ||
         !DiagramSystem::validTransform(transform)) {
diff --git a/framework/include/Actions.h b/framework/include/Actions.h
index fa36201..6b1f512 100644
--- a/framework/include/Actions.h
+++ b/framework/include/Actions.h
@@ -1,5 +1,5 @@
 ﻿#pragma once
-#include "Application.h"
+#include "Patterns.h"
 
 namespace graphics {
 // Value-configured operations. Scene and camera references are non-owning.
@@ -43,6 +43,17 @@ public:
     static MotionAction faceVisible(Scene& scene, EntityId id, std::size_t face, bool value) {
         return [&scene, id, face, value](double, const MotionSample&) { return scene.setFaceVisible(id, face, value); };
     }
+    static MotionAction faceRotation(Scene& scene, EntityId id, std::size_t face, FaceRotationConfig config) {
+        return [&scene, id, face, config](double, const MotionSample&) {
+            return PatternSystem::rotateFace(scene, id, face, config);
+        };
+    }
+    static MotionAction faceMode(Scene& scene, EntityId id, std::size_t face, FaceTransformMode mode,
+                                 bool preserveWorld = true) {
+        return [&scene, id, face, mode, preserveWorld](double, const MotionSample&) {
+            return scene.setFaceTransformMode(id, face, mode, preserveWorld);
+        };
+    }
     static MotionAction layer(Scene& scene, EntityId id, std::optional<LayerId> value) {
         return [&scene, id, value](double, const MotionSample&) {
             return value ? scene.bindEntity(id, *value) : scene.unbindEntity(id);
diff --git a/framework/include/CuboidMode.h b/framework/include/CuboidMode.h
index 42f5ec8..9b6d723 100644
--- a/framework/include/CuboidMode.h
+++ b/framework/include/CuboidMode.h
@@ -7,6 +7,8 @@ struct CuboidFaceConfig {
     GlColor color;
     bool visible = true;
     CullMode cull = CullMode::Inherit;
+    FaceTransformMode mode = FaceTransformMode::Hierarchy;
+    std::optional<FaceRotationConfig> rotation;
 };
 struct CuboidConfig {
     std::string name;
diff --git a/framework/include/Graphics.h b/framework/include/Graphics.h
index e416853..cef46e0 100644
--- a/framework/include/Graphics.h
+++ b/framework/include/Graphics.h
@@ -56,7 +56,8 @@ public:
     /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
     bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);
     /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
-    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);
+    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
+                            FaceAnimationSpace space = FaceAnimationSpace::Local);
     /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
     bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);
     /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
@@ -98,6 +99,8 @@ public:
 
     /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
     bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);
+    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
+    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);
 
     /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
     std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
diff --git a/framework/include/Model.h b/framework/include/Model.h
index 976ed8a..65c91d1 100644
--- a/framework/include/Model.h
+++ b/framework/include/Model.h
@@ -34,6 +34,8 @@ enum class GLFace { None, Rect, Triangle, Mesh };
 enum class PivotMode { Preserve, BottomCenter, Custom };
 enum class OutlineMode { None, FeatureEdges, AllTriangleEdges, Explicit };
 enum class CullMode { Inherit, None, Back, Front };
+enum class FaceTransformMode { Hierarchy, Independent };
+enum class FaceAnimationSpace { Local, Model };
 
 struct Transform3D {
     glm::vec3 basis{0.0f};
@@ -87,6 +89,9 @@ struct PlaneFigure {
     float creaseAngleDegrees = 1.0f;
     std::string name;
     CullMode cull = CullMode::Inherit;
+    FaceTransformMode transformMode = FaceTransformMode::Hierarchy;
+    glm::mat4 transformFrame{1.0f}; // Parent-local correction, or a frozen world frame.
+    FaceAnimationSpace animationSpace = FaceAnimationSpace::Local;
 
     /// [API] 정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.
     static PlaneFigure fromVertices(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& triangles) {
@@ -183,6 +188,11 @@ inline bool validMesh(const Mesh& m) {
         m.cull != CullMode::Front)
         return false;
 
+    if ((m.transformMode != FaceTransformMode::Hierarchy && m.transformMode != FaceTransformMode::Independent) ||
+        !finiteMatrix(m.transformFrame) ||
+        (m.animationSpace != FaceAnimationSpace::Local && m.animationSpace != FaceAnimationSpace::Model))
+        return false;
+
     if (m.localVertices.size() < 3 || !validColor(m.color) || !validColor(m.material.color) ||
         !finiteVector(m.normal) || !finiteMatrix(m.localAnimationMatrix) || !std::isfinite(m.creaseAngleDegrees) ||
         m.creaseAngleDegrees < 0 || m.creaseAngleDegrees > 180)
@@ -268,15 +278,22 @@ inline glm::mat4 transformMatrix(const Transform3D& t) {
     return glm::translate(glm::scale(m, t.scale), -t.localBasis);
 }
 
-inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
-    glm::mat4 child(1);
+inline glm::mat4 faceSrtMatrix(const Mesh& face) {
+    return face.independence
+        ? glm::translate(glm::mat4(1), face.transform.localBasis) * transformMatrix(face.transform)
+        : glm::mat4(1);
+}
 
-    if (face.independence) {
-        // Rotate/scale around a face pivot without moving an identity child.
-        child = glm::translate(glm::mat4(1), face.transform.localBasis) * transformMatrix(face.transform);
-    }
+inline glm::mat4 faceLocalMatrix(const Mesh& face) {
+    const auto child = faceSrtMatrix(face);
+    return face.animationSpace == FaceAnimationSpace::Model
+        ? face.localAnimationMatrix * child : child * face.localAnimationMatrix;
+}
 
-    return transformMatrix(c.transform) * child * face.localAnimationMatrix;
+inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
+    const auto parent = face.transformMode == FaceTransformMode::Hierarchy ? transformMatrix(c.transform) : glm::mat4(1);
+    return parent * face.transformFrame * faceLocalMatrix(face);
 }
+
 } // namespace detail
 } // namespace graphics
diff --git a/framework/include/Patterns.h b/framework/include/Patterns.h
index eddddea..564b0f6 100644
--- a/framework/include/Patterns.h
+++ b/framework/include/Patterns.h
@@ -96,6 +96,14 @@ struct JointRig {
     std::array<EntityId, 2> shoulders{}, hips{};
 };
 
+enum class FaceRotationKind { EdgeHinge, SelfSpin, ModelOrbit };
+struct FaceRotationConfig {
+    FaceRotationKind kind = FaceRotationKind::EdgeHinge;
+    std::size_t edge = 0; // Cyclic boundary edge of the original polygon.
+    glm::vec3 axis{0, 1, 0}; // Model-local direction for self spin / model orbit.
+    double degrees = 0; // Absolute angle, not a frame increment.
+};
+
 class PatternSystem {
 public:
     /// [API] 직선 → 안쪽 코너 중심 원호 → 직선의 거리에서 위치·접선·yaw를 계산한다. 끝에서 처음으로 순환한다.
@@ -121,6 +129,8 @@ public:
                           std::optional<EntityId> parent = std::nullopt);
     /// [API] 지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.
     static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);
+    /// [API] 원래 모서리 힌지·면 중심 자전·원본 모델 중심 공전을 구분하여 면 회전 행렬을 설정한다.
+    static bool rotateFace(Scene& scene, EntityId entity, std::size_t face, const FaceRotationConfig& config);
     /// [API] 진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.
     static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement,
                          const glm::vec3& topPivot, bool scaleOpen);
diff --git a/tools/Write-ApiKo.py b/tools/Write-ApiKo.py
index 8b0280f..7f830f9 100644
--- a/tools/Write-ApiKo.py
+++ b/tools/Write-ApiKo.py
@@ -208,6 +208,17 @@ def description(r):
     if name==owner:return {'Application':'초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.','Scene':'빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.','Renderer':'GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.','AssetLoader':'프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.'}[owner]
     if name.startswith('~'):return '실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.'
     if name=='operator bool':return '모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.'
+    additions={
+        ('HumanoidPose','seated'):'허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다.',
+        ('AnimationSystem','createJointAnimation'):'관절의 기준 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율이다.',
+        ('AnimationSystem','poseHierarchy'):'기준 SRT와 모션을 합성하고 모든 관절이 유효하면 자세를 반영한다.',
+        ('AnimationSystem','addHumanoid'):'편집 가능한 사람형 관절·메시 계층을 만든다. 구성 실패는 기존 Scene을 유지한다.',
+        ('AnimationSystem','poseHumanoid'):'각 관절 위치를 유지하며 로컬 Euler 자세를 적용한다.',
+        ('AnimationSystem','addSwingWithRider'):'고정 프레임·진자 관절·좌석·앉은 사람과 모션을 구성한다.',
+        ('Motion','pendulum'):'축·진폭·주기·위상의 사인 진자 회전 단계를 만든다.',
+        ('Scene','setFaceTransformMode'):'면의 계층 상속·월드 독립을 전환한다. 기본값은 현재 자세 보존이다.',
+        ('PatternSystem','rotateFace'):'현재 면의 모서리 힌지·중심 자전·원본 모델 중심 공전을 구분해 회전을 설정한다.'}
+    if (owner,name) in additions:return additions[owner,name]
     if owner=='SceneActions':return '지정 값과 대상 ID를 기존 API에 전달하는 '+name+' 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.'
     if owner=='DrawUniformContext':return '현재 바인딩된 program의 추가 uniform을 설정한다. 이름·GLSL 자료형과 값의 유한성을 맞춘다.'
     if owner=='CuboidMode':return {
diff --git a/tools/author_framework_guides.py b/tools/author_framework_guides.py
index 40e4b2f..38cbc4d 100644
--- a/tools/author_framework_guides.py
+++ b/tools/author_framework_guides.py
@@ -27,37 +27,20 @@ def source(p):
     return '<details><summary>실제 저장소 소스 · '+esc(p)+'</summary>'+code(read(p))+'</details>'
 
 def apis():
+    # Reuse the public-declaration extractor so inline APIs without generated comments are included.
+    import importlib.util
+    spec = importlib.util.spec_from_file_location('framework_api_catalog', ROOT/'tools/Write-ApiKo.py')
+    catalog = importlib.util.module_from_spec(spec)
+    spec.loader.exec_module(catalog)
     result = []
-    for p in sorted((ROOT/'framework/include').glob('*.h')):
-        s = p.read_text(encoding='utf-8-sig')
-        mask = re.sub(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"', lambda m: ''.join('\n' if c=='\n' else ' ' for c in m.group()), s)
-        spans = []
-        for m in re.finditer(r'\b(?:class|struct)\s+(\w+)\s*\{', mask):
-            start=m.end()-1; depth=1; end=start+1
-            while end<len(mask) and depth:
-                depth += (mask[end]=='{')-(mask[end]=='}'); end+=1
-            spans.append((start,end,m.group(1)))
-        for m in re.finditer(r'^[ \t]*///\s*\[API\]\s*(.+)\n',s,re.M):
-            start=m.end(); i=start; parens=0
-            while i<len(s):
-                c=s[i]
-                if c=='(': parens+=1
-                elif c==')': parens-=1
-                elif c in ';{' and parens==0: break
-                i+=1
-            signature=' '.join(s[start:i].strip().split())
-            if '(' not in signature: continue
-            prefix=signature.split('(',1)[0]
-            nm=re.search(r'(operator\s+bool|operator\s*=|~?\w+)\s*$',prefix)
-            if not nm: continue
-            name=nm.group(1)
-            if re.search(r'\w+::'+re.escape(name)+r'\s*$',prefix):continue
-            enclosing=[v for v in spans if v[0]<start<v[1]]
-            owner=min(enclosing,key=lambda v:v[1]-v[0])[2] if enclosing else ''
-            result.append(dict(owner=owner,name=name,signature=signature,file=p.relative_to(ROOT).as_posix(),line=s[:start].count('\n')+1,description=m.group(1)))
-            if name=='readTextFile': result[-1]['description']='파일을 문자열로 읽고 UTF-8 BOM을 제거한다. 파일을 열지 못하면 빈 문자열을 반환한다.'
+    for path in sorted((ROOT/'framework/include').glob('*.h')):
+        for record in catalog.declarations(path):
+            record['description'] = catalog.description(record)
+            record['result'] = catalog.result_contract(record)
+            result.append(record)
     return result
 
+
 GROUPS={}
 def group(key,title,story,roles,code_text,after,files=()):
     GROUPS[key]=dict(title=title,story=story,roles=roles,code=code_text,after=after,files=files)
@@ -602,8 +585,31 @@ bool configureEditable(graphics::Application& app) {
 ''','sample/seek는 의미 동작을 실행하지 않는다. 모션 갱신은 활성 동작을 실행한다. mode.apply는 Scene 전체를 교체한다. 사용자 셰이더 입력은 실제 바이트와 맞춰야 한다.',
  ('framework/include/RenderConfig.h','framework/include/Actions.h','framework/include/CuboidMode.h','framework/detail/CuboidMode.cpp'))
 
+group('face-modes','면의 독립·계층과 힌지·자전·공전',
+ '육면체의 변환 상속과 면 자체의 회전 중심을 별도 설정으로 조합한다.',
+ 'FaceTransformMode는 상속, FaceRotationKind는 축의 위치/방향, FaceAnimationSpace는 면 SRT와 추가 행렬의 적용 순서다.',r'''
+bool faceMotion(graphics::Scene& scene,graphics::EntityId box,std::size_t face) {
+    graphics::Transform3D pose; pose.basis={0,.2f,0};
+    if(!scene.setFaceSRT(box,face,pose)) return false;
+    if(!scene.setFaceTransformMode(box,face,graphics::FaceTransformMode::Independent)) return false;
+    graphics::FaceRotationConfig rotation;
+    rotation.kind=graphics::FaceRotationKind::EdgeHinge;
+    rotation.edge=0; rotation.degrees=90;
+    if(!graphics::PatternSystem::rotateFace(scene,box,face,rotation)) return false;
+    rotation.kind=graphics::FaceRotationKind::SelfSpin; rotation.axis={0,1,0};
+    if(!graphics::PatternSystem::rotateFace(scene,box,face,rotation)) return false;
+    rotation.kind=graphics::FaceRotationKind::ModelOrbit;
+    if(!graphics::PatternSystem::rotateFace(scene,box,face,rotation)) return false;
+    graphics::Motion actions;
+    if(!actions.append(graphics::Motion::action(graphics::SceneActions::faceRotation(scene,box,face,rotation)))) return false;
+    return actions.insertAction(1,graphics::SceneActions::faceMode(scene,box,face,graphics::FaceTransformMode::Hierarchy));
+}
+''','독립 면은 전환 순간의 기준을 고정하며 기본 계층 모드는 본체 변환을 따른다. 위 회전 호출은 각 종류의 시연이며 마지막 행렬이 최종 값이다. 자세 보존과 적용 순서는 FACE_TRANSFORMS.md를 읽는다.',
+ ('framework/include/Model.h','framework/detail/Scene.cpp','framework/include/Patterns.h','framework/detail/Patterns.cpp'))
+
 def choose(a):
     o,n=a['owner'],a['name']
+    if (o=='Scene' and n=='setFaceTransformMode') or (o=='PatternSystem' and n=='rotateFace') or (o=='SceneActions' and n in ('faceMode','faceRotation')):return 'face-modes'
     if o=='Application': return 'editable' if n=='configureRenderer' else ('assets' if n=='getAssets' else 'app')
     if o=='Scene':
         if n in 'setFaceVisible setFaceCullMode setFaceLocalMatrix setPosition setTransform setParent worldMatrix faceWorldMatrix setFaceTransform setLocalBasis splitFace setFaceSRT addGroup'.split():return 'transform'
@@ -721,6 +727,8 @@ PATTERNS=[
 
 PATTERNS.append(('editable','의미 동작·육면체 모드·정점 입력 편집','editable',['초기 설정과 의미 동작 구성','이름 기반 키 바인딩 등록','프레임당 모션 갱신과 동작 실행','RenderConfig로 정점 입력·uniform 연결']))
 
+PATTERNS.append(('face-modes','면의 독립·계층과 힌지·자전·공전','face-modes',['변환 상속 모드 선택','면 SRT 설정','회전 종류와 축 설정','본체 변환과 면 변환을 합성']))
+
 def main():
     entries=apis()
     intro='<p><a href="MODE_ACTIONS.md">의미 동작·육면체 모드</a> · <a href="RENDER_CONFIGURATION.md">정점 입력·내부 수정 순서</a> · <a href="PATTERN_COMPOSITION.md">패턴 조립·API 압축</a></p>'+para('함수 이름을 외우는 대신 실제 상황에서 무엇을 저장하고 무엇을 바꾸는지 읽는다. 공개 함수·오버로드·생성자·소멸자·삭제된 복사 연산을 현재 framework/include 헤더에서 수록했다. graphics::detail 및 private 구현은 공개 호출 대상으로 나열하지 않고 공통 구현 설명에서 다룬다.')

```
