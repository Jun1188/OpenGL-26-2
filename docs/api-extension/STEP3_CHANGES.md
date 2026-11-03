# 3단계: 패턴 압축·문서와 구조 갱신

세 전용 조합을 공통 createMotion과 문서 조립으로 압축했다. 정점 설정 공개 helper와 육면체 면 색 설정을 보완하고 API 문서 생성기를 맞췄다.

전체 쌍은 [전체 코드 변경 전후](ALL_CODE_BEFORE_AFTER.md)를 읽는다. 아래는 2단계 커밋 대비 변경이다.

```diff
diff --git a/framework-analysis/update_graph.py b/framework-analysis/update_graph.py
index 34e05b8..af2603f 100644
--- a/framework-analysis/update_graph.py
+++ b/framework-analysis/update_graph.py
@@ -43,8 +43,11 @@ def project_inventory():
                 line=text[:m.start()].count('\n')+1
                 item={'file':relative,'line':line,'name':m.group(1)}
                 if 'computer_graphics_warmUp1/' in relative:
-                    problem=1 if line<550 else 2 if line<765 else 3 if line<876 else 4
-                    item.update(condition=f'PROB == {problem}',active_by_source_default=problem==2)
+                    branches=list(re.finditer(r'^\s*#(?:if|elif)\s+PROB\s*==\s*(\d+)',text[:m.start()],re.M))
+                    default=re.search(r'^\s*#define\s+PROB\s+(\d+)',text,re.M)
+                    problem=int(branches[-1].group(1)) if branches else None
+                    item.update(condition=f'PROB == {problem}',
+                                active_by_source_default=bool(default and problem==int(default.group(1))))
                 entries.append(item)
         projects.append({'project':p.relative_to(REPOSITORY).as_posix(),'sources':sources,'entrypoints':entries})
     for name in ['basic','assets','motion']:
diff --git a/framework/detail/CuboidMode.cpp b/framework/detail/CuboidMode.cpp
index 00e12b8..4aedc86 100644
--- a/framework/detail/CuboidMode.cpp
+++ b/framework/detail/CuboidMode.cpp
@@ -8,7 +8,7 @@ bool validCuboid(const CuboidConfig& c) {
         !detail::validColor(c.color) || !DiagramSystem::validTransform(c.transform))
         return false;
     for (const auto& face : c.faces)
-        if (!DiagramSystem::validTransform(face.transform) ||
+        if (!DiagramSystem::validTransform(face.transform) || !detail::validColor(face.color) ||
             (face.cull != CullMode::Inherit && face.cull != CullMode::None &&
              face.cull != CullMode::Back && face.cull != CullMode::Front))
             return false;
@@ -75,7 +75,10 @@ bool CuboidMode::apply(Application& app) {
     std::map<std::string, MotionId> motions;
     for (const auto& c : config_.cuboids) {
         if (!validCuboid(c)) return false;
-        const auto id = staged.addCuboid(c.size, c.transform.basis);
+        auto model = DiagramSystem::makePolyhedron(GLShape::Cube, c.size);
+        for (std::size_t face = 0; face < c.faces.size(); ++face)
+            model.faces[face].color = c.faces[face].color;
+        const auto id = staged.addModel(model, c.transform, PivotMode::Preserve);
         if (!id || !staged.setTransform(*id, c.transform) || !staged.setTint(*id, c.color) ||
             !staged.setVisible(*id, c.visible)) return false;
         for (std::size_t face = 0; face < c.faces.size(); ++face)
diff --git a/framework/detail/Motion.cpp b/framework/detail/Motion.cpp
index c7ed50a..e7bced3 100644
--- a/framework/detail/Motion.cpp
+++ b/framework/detail/Motion.cpp
@@ -674,66 +674,20 @@ std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
     return result;
 }
 
-std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
-    config.space = TransformSpace::Local;
-
+std::optional<MotionId> MotionSystem::createMotion(std::initializer_list<MotionStep> steps) {
     Motion motion;
-
-    if (!motion.append(Motion::rotation(config)))
-        return (
-            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createSpin",
-                                "Rejected condition: !motion.append(Motion::rotation(config))",
-                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
-            std::nullopt);
-
+    for (const auto& step : steps)
+        if (!motion.append(step)) return std::nullopt;
     return createMotion(motion);
 }
 
-std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
-    config.space = TransformSpace::World;
-
-    Motion motion;
-
-    if (!motion.append(Motion::rotation(config)))
-        return (
-            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createOrbit",
-                                "Rejected condition: !motion.append(Motion::rotation(config))",
-                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
-            std::nullopt);
-
-    return createMotion(motion);
-}
-
-std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit, RotationMotion spin) {
-    orbit.space = TransformSpace::World;
-    spin.space = TransformSpace::Local;
-
-    Motion motion;
-
-    if (!motion.append(Motion::rotation(orbit)) || !motion.append(Motion::rotation(spin)))
-        return (
-            Diagnostics::failAt(
-                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createOrbitAndSpin",
-                "Rejected condition: !motion.append(Motion::rotation(orbit)) || !motion.append(Motion::rotation(spin))",
-                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
-            std::nullopt);
-
-    return createMotion(motion);
+std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
+    config.space = TransformSpace::Local;
+    return createMotion({Motion::rotation(config)});
 }
 
-std::optional<MotionId> MotionSystem::createPathAndSpin(const PathMotion& path, RotationMotion spin) {
-    spin.space = TransformSpace::Local;
-
-    Motion motion;
-
-    if (!motion.append(Motion::followPath(path)) || !motion.append(Motion::rotation(spin)))
-        return (
-            Diagnostics::failAt(
-                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createPathAndSpin",
-                "Rejected condition: !motion.append(Motion::followPath(path)) || !motion.append(Motion::rotation(spin))",
-                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
-            std::nullopt);
-
-    return createMotion(motion);
+std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
+    config.space = TransformSpace::World;
+    return createMotion({Motion::rotation(config)});
 }
 } // namespace graphics
diff --git a/framework/detail/Native.h b/framework/detail/Native.h
index 7c1538a..3584a08 100644
--- a/framework/detail/Native.h
+++ b/framework/detail/Native.h
@@ -226,6 +226,7 @@ struct GlApi {
     PFNGLUNIFORMMATRIX3FVPROC UniformMatrix3fv = nullptr;
     PFNGLUNIFORM3FPROC Uniform3f = nullptr;
     PFNGLUNIFORM1IPROC Uniform1i = nullptr;
+    PFNGLUNIFORM1FPROC Uniform1f = nullptr;
     PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;
 
     template <class T> T resolve(const char* name) {
@@ -298,6 +299,7 @@ struct GlApi {
         UniformMatrix3fv = resolve<decltype(UniformMatrix3fv)>("glUniformMatrix3fv");
         Uniform3f = resolve<decltype(Uniform3f)>("glUniform3f");
         Uniform1i = resolve<decltype(Uniform1i)>("glUniform1i");
+        Uniform1f = resolve<decltype(Uniform1f)>("glUniform1f");
         ActiveTexture = resolve<decltype(ActiveTexture)>("glActiveTexture");
     }
 };
diff --git a/framework/detail/Patterns.cpp b/framework/detail/Patterns.cpp
index dc2fc81..4315637 100644
--- a/framework/detail/Patterns.cpp
+++ b/framework/detail/Patterns.cpp
@@ -672,28 +672,6 @@ bool PatternSystem::openRoomWall(Scene& scene, EntityId room, std::size_t face,
     return openFace(scene, room, face, progress, glm::vec3(0), pivot, true);
 }
 
-bool PatternSystem::swapModels(Scene& scene, EntityId first, EntityId second) {
-    const auto* a = scene.getComponent(first);
-    const auto* b = scene.getComponent(second);
-
-    if (!a || !b || a->transformOnly || b->transformOnly)
-        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "PatternSystem::swapModels",
-                                   "Both entities must contain a model.");
-
-    const auto firstModel = a->polyhedron, secondModel = b->polyhedron;
-
-    if (!scene.setModel(first, secondModel))
-        return false;
-
-    if (!scene.setModel(second, firstModel)) {
-        scene.setModel(first, firstModel);
-
-        return false;
-    }
-
-    return true;
-}
-
 std::vector<glm::ivec3> PatternSystem::snakeCells(const glm::ivec3& cells) {
     std::vector<glm::ivec3> result;
 
diff --git a/framework/detail/Renderer.cpp b/framework/detail/Renderer.cpp
index 10611f3..3597fdf 100644
--- a/framework/detail/Renderer.cpp
+++ b/framework/detail/Renderer.cpp
@@ -199,8 +199,8 @@ bool Renderer::initShader(const std::filesystem::path& vertexPath, const std::fi
     outlineColorLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.outlineColor.c_str());
     tintLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.tint.c_str());
 
-    return mvpLocation_ >= 0 || Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glGetUniformLocation",
-                                                    "Required uMVP uniform is missing or optimized away.");
+    return mvpLocation_ >= 0 || (config_.uniforms.mvp.empty() && bool(config_.bindUniforms)) || Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glGetUniformLocation",
+                                                    "Required MVP uniform is missing or optimized away: " + config_.uniforms.mvp);
 }
 
 bool Renderer::initialize(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath,
diff --git a/framework/include/Application.h b/framework/include/Application.h
index a7b4fc2..b97086d 100644
--- a/framework/include/Application.h
+++ b/framework/include/Application.h
@@ -19,10 +19,10 @@ struct ApplicationConfig {
     std::filesystem::path projectRoot = ".";              // Relative to the executable.
     std::filesystem::path runtimeDirectory = "framework"; // Relative to projectRoot.
 
-    RenderConfig renderConfig;
     FeatureOptions features;
     bool verticalSync = true;
     DiagnosticOptions diagnostics;
+    RenderConfig renderConfig;
 };
 
 struct MouseEvent {
diff --git a/framework/include/CuboidMode.h b/framework/include/CuboidMode.h
index 63cac86..42f5ec8 100644
--- a/framework/include/CuboidMode.h
+++ b/framework/include/CuboidMode.h
@@ -4,6 +4,7 @@
 namespace graphics {
 struct CuboidFaceConfig {
     Transform3D transform;
+    GlColor color;
     bool visible = true;
     CullMode cull = CullMode::Inherit;
 };
diff --git a/framework/include/Motion.h b/framework/include/Motion.h
index 4631243..0050873 100644
--- a/framework/include/Motion.h
+++ b/framework/include/Motion.h
@@ -1,6 +1,7 @@
 ﻿#pragma once
 #include "Transform.h"
 #include <map>
+#include <initializer_list>
 
 namespace graphics {
 using MotionId = std::uint64_t;
@@ -84,7 +85,7 @@ public:
         return steps_;
     }
 
-    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성하여 MotionSample을 반환한다.
+    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다. 의미 동작은 실행하지 않는다.
     std::optional<MotionSample> sample(double seconds) const;
     /// [API] 변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.
     std::optional<MotionSample> execute(double seconds) const;
@@ -139,6 +140,8 @@ public:
 
     /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
     std::optional<MotionId> createMotion(const Motion& motion);
+    /// [API] 의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.
+    std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);
     /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
     bool setMotion(MotionId id, const Motion& motion);
     /// [API] 모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
@@ -177,9 +180,6 @@ public:
     std::optional<MotionId> createSpin(RotationMotion config);
     /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
     std::optional<MotionId> createOrbit(RotationMotion config);
-    /// [API] 월드 공전과 로컬 자전을 하나의 모션에 등록한다.
-    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
-    /// [API] 경로 이동과 로컬 자전을 하나의 모션에 등록한다.
-    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
+
 };
 } // namespace graphics
diff --git a/framework/include/Patterns.h b/framework/include/Patterns.h
index 7a7a9e1..eddddea 100644
--- a/framework/include/Patterns.h
+++ b/framework/include/Patterns.h
@@ -128,8 +128,6 @@ public:
     static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);
     /// [API] 선택 옆벽의 정점에서 상단 피벗을 계산하고 openFace로 높이를 축소/복원한다.
     static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
-    /// [API] 두 ID의 위치는 유지하고 모델을 교환한다.
-    static bool swapModels(Scene& scene, EntityId first, EntityId second);
     /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
     static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
     /// [API] 두 값의 차이가 허용 오차 이하인지 검사한다.
diff --git a/framework/include/RenderConfig.h b/framework/include/RenderConfig.h
index 3220eed..e19819d 100644
--- a/framework/include/RenderConfig.h
+++ b/framework/include/RenderConfig.h
@@ -44,6 +44,30 @@ struct DrawUniformContext {
     std::size_t face;
     glm::mat4 model, view, projection;
     bool depthOnly = false;
+    /// [API] 현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.
+    bool setUniform(const std::string& name, float value) const {
+        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
+        if (location < 0 || !std::isfinite(value)) return false;
+        detail::gl().Uniform1f(location, value); return true;
+    }
+    /// [API] 현재 program의 추가 int/bool/sampler uniform을 설정한다.
+    bool setUniform(const std::string& name, int value) const {
+        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
+        if (location < 0) return false;
+        detail::gl().Uniform1i(location, value); return true;
+    }
+    /// [API] 현재 program의 추가 vec3 uniform을 설정한다.
+    bool setUniform(const std::string& name, const glm::vec3& value) const {
+        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
+        if (location < 0 || !detail::finiteVector(value)) return false;
+        detail::gl().Uniform3f(location, value.x, value.y, value.z); return true;
+    }
+    /// [API] 현재 program의 추가 mat4 uniform을 설정한다.
+    bool setUniform(const std::string& name, const glm::mat4& value) const {
+        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
+        if (location < 0 || !detail::finiteMatrix(value)) return false;
+        detail::gl().UniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]); return true;
+    }
 };
 
 struct RenderConfig {
diff --git a/tools/Write-ApiKo.py b/tools/Write-ApiKo.py
index cb10904..8b0280f 100644
--- a/tools/Write-ApiKo.py
+++ b/tools/Write-ApiKo.py
@@ -120,8 +120,6 @@ DESCRIPTIONS={
 'getLayer':'레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.',
 'createSpin':'로컬 공간 회전 모션을 등록하여 자전에 사용한다.',
 'createOrbit':'월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.',
-'createOrbitAndSpin':'월드 공전과 로컬 자전을 하나의 모션에 등록한다.',
-'createPathAndSpin':'경로 이동과 로컬 자전을 하나의 모션에 등록한다.',
 'contains':'점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.',
 'overlaps':'두 축 정렬 경계 상자가 겹치는지 검사한다.',
 'setPoints':'연속 점들의 누적 거리 경로를 구성한다. closed면 마지막에서 처음으로 연결한다.',
@@ -158,7 +156,7 @@ SPECIAL={
 ('Diagnostics','clear'):'마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.',
 ('Scene','clear'):'Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.',
 ('MotionSystem','clear'):'모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.',
-('Motion','sample'):'절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성하여 MotionSample을 반환한다.',
+('Motion','sample'):'절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성한다. 의미 동작은 실행하지 않는다.',
 ('DistancePath','sample'):'누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.',
 ('Motion','rotation'):'축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.',
 ('TransformSystem','rotation'):'직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.',
@@ -202,10 +200,38 @@ def declarations(path):
     walk(PARSER.parse(b).root_node);return out
 def description(r):
     owner,name=r['owner'],r['name']
+    source_lines=(ROOT/r['file']).read_text(encoding='utf-8-sig').splitlines()
+    preceding=r['line']-2
+    if preceding>=0 and '/// [API]' in source_lines[preceding]:
+        return source_lines[preceding].split('/// [API]',1)[1].strip()
     if '= delete' in r['signature']:return '이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.'
     if name==owner:return {'Application':'초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.','Scene':'빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.','Renderer':'GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.','AssetLoader':'프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.'}[owner]
     if name.startswith('~'):return '실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.'
     if name=='operator bool':return '모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.'
+    if owner=='SceneActions':return '지정 값과 대상 ID를 기존 API에 전달하는 '+name+' 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.'
+    if owner=='DrawUniformContext':return '현재 바인딩된 program의 추가 uniform을 설정한다. 이름·GLSL 자료형과 값의 유한성을 맞춘다.'
+    if owner=='CuboidMode':return {
+        'setConfig':'초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.',
+        'getConfig':'초기 설정의 읽기 전용 참조를 반환한다.',
+        'setCuboid':'이름별 육면체 초기 설정을 교체하거나 추가한다.',
+        'bindKey':'같은 key/action/mods 바인딩을 교체하거나 추가한다.',
+        'unbindKey':'같은 key/action/mods 바인딩을 해제한다.',
+        'getBindings':'키 바인딩 목록의 읽기 전용 참조를 반환한다.',
+        'setSetupHandler':'새 장면 구성 후 실행할 설정 핸들러를 교체한다.',
+        'setUpdateHandler':'프레임별 응용 갱신 핸들러를 교체한다.',
+        'apply':'임시 CPU Scene에 육면체를 구성하고 성공하면 장면 전체를 교체한다.',
+        'update':'등록된 모드 프레임 핸들러를 호출한다.',
+        'dispatch':'일치하는 키 바인딩을 실행한다. 없는 키는 정상 처리한다.',
+        'entity':'이름에 대응하는 마지막 생성 엔티티 ID를 조회한다.',
+        'layer':'이름에 대응하는 초기 생성 레이어 ID를 조회한다.',
+        'motion':'이름에 대응하는 초기 생성 모션 ID를 조회한다.',
+        'action':'실행 시 육면체 이름을 해석하는 의미 명령 핸들러를 만든다.'}[name]
+    if name=='bindCuboidMode':return '육면체 모드의 init/update/수정키 핸들러를 설치한다. 기존 해당 핸들러를 교체한다.'
+    if name=='configureRenderer':return '현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.'
+    if owner=='Motion' and name in ('action','insertAction','execute'):
+        return {'action':'다른 단계와 조립할 의미 동작 단계를 만든다.',
+                'insertAction':'주어진 인덱스에 람다·함수 포인터·의미 동작을 삽입한다.',
+                'execute':'변환과 동작을 순서대로 평가한다. 외부 부수 효과는 복구하지 않는다.'}[name]
     if (owner,name) in SPECIAL:return SPECIAL[owner,name]
     if name in DESCRIPTIONS:return DESCRIPTIONS[name]
     if name in guide.PURPOSE:return guide.PURPOSE[name]
diff --git a/tools/Write-CodeGuide.py b/tools/Write-CodeGuide.py
index 4b29464..8320db3 100644
--- a/tools/Write-CodeGuide.py
+++ b/tools/Write-CodeGuide.py
@@ -117,7 +117,6 @@ PURPOSE={
 'foldFace':'지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.',
 'screenOnPlane':'화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.',
 'snap':'좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.',
-'swapModels':'두 ID의 위치는 유지하고 모델을 교환한다.',
 'reset':'응용 시간·ID·계층·카메라·입력과 모드를 초기 상태로 복구한다.',
 'placeMovingObjects':'상자와 줄무늬가 공유하는 거리 시계에 offset을 더해 각 최종 자세를 계산한다.',
 'syncScene':'물리/논리 상태에서 결정한 월드 위치·회전을 표시 Scene에 반영한다.',
diff --git a/tools/author_framework_guides.py b/tools/author_framework_guides.py
index af203fe..40e4b2f 100644
--- a/tools/author_framework_guides.py
+++ b/tools/author_framework_guides.py
@@ -292,9 +292,11 @@ bool connectMotion(graphics::Scene& scene,graphics::EntityId entity) {
     if(!system.popLayer(*layer)||!system.removeMotion(*id)) return false;
     auto spin=system.createSpin(rotation);
     auto orbit=system.createOrbit(rotation);
-    auto combined=system.createOrbitAndSpin(rotation,rotation);
+    auto orbitConfig=rotation; orbitConfig.space=graphics::TransformSpace::World;
+    auto spinConfig=rotation; spinConfig.space=graphics::TransformSpace::Local;
+    auto combined=system.createMotion({graphics::Motion::rotation(orbitConfig), graphics::Motion::rotation(spinConfig)});
     graphics::PathMotion path; path.path=graphics::PathSystem::circle({0,0,0},1);
-    auto moving=system.createPathAndSpin(path,rotation);
+    auto moving=system.createMotion({graphics::Motion::followPath(path), graphics::Motion::rotation(spinConfig)});
     if(!spin||!orbit||!combined||!moving) return false;
     system.clear(); // 기존 레이어 연결을 사용하지 않는 별도 예제 종료 지점
     return true;
@@ -498,7 +500,11 @@ bool editParts(graphics::Scene& scene,graphics::EntityId body,graphics::EntityId
     auto selected=graphics::PatternSystem::cameraFacingWall(scene,room,camera);
     if(selected&&!graphics::PatternSystem::openRoomWall(scene,room,*selected,.5f)) return false;
     auto other=scene.addCuboid({1,1,1},{2,0,0}); if(!other) return false;
-    if(!graphics::PatternSystem::swapModels(scene,body,*other)) return false;
+    auto staged=scene;
+    const auto firstModel=scene.getComponent(body)->polyhedron;
+    const auto secondModel=scene.getComponent(*other)->polyhedron;
+    if(!staged.setModel(body,secondModel)||!staged.setModel(*other,firstModel)) return false;
+    scene=std::move(staged);
     if(!graphics::PatternSystem::moveCamera(camera,{.2f,0,0})
        ||!graphics::PatternSystem::yawCamera(camera,5)
        ||!graphics::PatternSystem::orbitCamera(camera,{{0,0,0},{0,1,0}},5)
@@ -566,9 +572,39 @@ bool rotateSelected(graphics::Scene& scene,std::vector<graphics::JointAnimation>
 // 실제 키 선택·배율 제한·복원은 아래 standard::Console 소스를 따른다.
 ''','시계를 정지해도 seconds는 유지되므로 같은 시각에서 편집 결과를 즉시 볼 수 있다. 입력 후 재생을 재개해도 rest가 남는다. standard의 handleJointKey는 실패 시 rest를 복원하고, R은 선택 관절의 최초 rest를 복원한다.',('standard/Console.h','standard/Console.cpp'))
 
+group('editable','의미 동작·육면체 모드·셰이더 연결 편집',
+ '설정과 작은 핸들러를 편집하여 기존 Scene/ECS에 동작을 연결한다. 세부 계약과 내부 코드 수정 순서는 MODE_ACTIONS.md, RENDER_CONFIGURATION.md, PATTERN_COMPOSITION.md를 읽는다.',
+ 'mode는 설정과 이름 바인딩을 보관한다. Scene은 컴포넌트 데이터, MotionSystem은 시간, Renderer는 GPU 자원을 소유한다.',r'''
+bool configureEditable(graphics::Application& app) {
+    auto mode=std::make_shared<graphics::CuboidMode>();
+    graphics::CuboidConfig box; box.name="box"; box.size={1,2,1};
+    if(!mode->setCuboid(box)) return false;
+    graphics::CuboidCommand command;
+    command.operation=graphics::CuboidOperation::Translate; command.value={.1f,0,0};
+    if(!mode->bindKey({GLFW_KEY_RIGHT,GLFW_PRESS,0,graphics::CuboidMode::action("box",command)})) return false;
+    mode->setSetupHandler([](graphics::Application& app,graphics::CuboidMode& mode) {
+        const auto id=mode.entity("box"); if(!id) return false;
+        graphics::Motion motion;
+        graphics::RotationMotion spin;
+        if(!motion.append(graphics::Motion::rotation(spin))) return false;
+        if(!motion.insertAction(1,graphics::SceneActions::visible(app.getScene(),*id,true))) return false;
+        const auto track=app.getScene().getMotions().createMotion(motion); if(!track) return false;
+        graphics::LayerConfig layer; layer.motion=*track;
+        const auto binding=app.getScene().getMotions().pushLayer(graphics::MotionSystem::rootLayer(),layer);
+        return binding && app.getScene().bindEntity(*id,*binding);
+    });
+    return graphics::bindCuboidMode(app,mode);
+}
+// Run before app.run(config), not inside an existing init callback.
+// config.renderConfig.bindUniforms = [](const graphics::DrawUniformContext& context) {
+//     return context.setUniform("uAmplitude", .1f);
+// };
+''','sample/seek는 의미 동작을 실행하지 않는다. 모션 갱신은 활성 동작을 실행한다. mode.apply는 Scene 전체를 교체한다. 사용자 셰이더 입력은 실제 바이트와 맞춰야 한다.',
+ ('framework/include/RenderConfig.h','framework/include/Actions.h','framework/include/CuboidMode.h','framework/detail/CuboidMode.cpp'))
+
 def choose(a):
     o,n=a['owner'],a['name']
-    if o=='Application': return 'assets' if n=='getAssets' else 'app'
+    if o=='Application': return 'editable' if n=='configureRenderer' else ('assets' if n=='getAssets' else 'app')
     if o=='Scene':
         if n in 'setFaceVisible setFaceCullMode setFaceLocalMatrix setPosition setTransform setParent worldMatrix faceWorldMatrix setFaceTransform setLocalBasis splitFace setFaceSRT addGroup'.split():return 'transform'
         if n in 'setMesh getMesh setMeshVertices setVertexPosition setTriangleIndices generateTexcoords transformTexcoords setTexcoords setVertexNormals setMaterial setOutline setFaceVertexColors'.split():return 'mesh'
@@ -579,6 +615,7 @@ def choose(a):
     if o in ('CameraSystem','Renderer'):return 'render'
     if o in ('AssetLoader','ModelResult','TextureResult','TextureImage'):return 'assets'
     if o in ('PlaneFigure','Polyhedron'):return 'mesh'
+    if o in ('SceneActions','CuboidMode','DrawUniformContext') or n in ('bindCuboidMode','configureRenderer'):return 'editable'
     if o=='Motion':return 'motion'
     if o=='MotionSystem':return 'layers'
     if o=='Diagnostics':return 'diagnostics'
@@ -659,7 +696,7 @@ def runtime():
 
 PATTERNS=[
  ('bootstrap','콜백으로 응용 상태 연결','app',['상태와 ID 보관','init에서 Scene 생성','입력으로 상태 변경','update로 변환 반영','렌더·종료는 Application에 위임']),
- ('spin-orbit','자전과 공전을 분리','layers',['공전축·자전축 지정','createOrbitAndSpin으로 모션 등록','레이어에 모션 연결','객체 바인딩 → 자동 시간 진행']),
+ ('spin-orbit','자전과 공전을 분리','layers',['공전축·자전축 지정','createMotion 단계 목록으로 모션 등록','레이어에 모션 연결','객체 바인딩 → 자동 시간 진행']),
  ('path','경로 전환·왕복','math',['PathSystem으로 경로 생성','PathMotion의 기간·재생 정책 지정','Motion.followPath 구성','MotionSystem.setMotion → seekMotion(0)']),
  ('sequence','순차 동작과 끝 자세 유지','motion',['유한 duration의 단계 생성','append로 첫 단계 등록','then으로 다음 시작 시각 배치','sample이 종료 자세를 유지']),
  ('shared','모션 공유와 대칭 분기','layers',['공통 MotionId 생성','정상·반사 레이어 분기','동일 MotionId 연결','같은 시각을 서로 다른 변환으로 표시']),
@@ -682,9 +719,11 @@ PATTERNS=[
  ('swing','그네(진자) → 좌석 → 사람','animation',['addSwingWithRider 설정','고정 frame → hinge → seat → person.root','앉은 자세와 그네·상체 트랙 구성','update: 누적 시간으로 poseHierarchy','부모 변환이 사람의 전체 관절에 전달']),
  ('views','여러 카메라로 같은 Scene 표시','parts',['update에서 Scene 상태 한 번 갱신','threeViews로 카메라·픽셀 영역 구성','Application.renderViews 호출','패스마다 같은 시간의 장면 표시'])]
 
+PATTERNS.append(('editable','의미 동작·육면체 모드·정점 입력 편집','editable',['초기 설정과 의미 동작 구성','이름 기반 키 바인딩 등록','프레임당 모션 갱신과 동작 실행','RenderConfig로 정점 입력·uniform 연결']))
+
 def main():
     entries=apis()
-    intro=para('함수 이름을 외우는 대신 실제 상황에서 무엇을 저장하고 무엇을 바꾸는지 읽는다. 공개 함수·오버로드·생성자·소멸자·삭제된 복사 연산을 현재 framework/include 헤더에서 수록했다. graphics::detail 및 private 구현은 공개 호출 대상으로 나열하지 않고 공통 구현 설명에서 다룬다.')
+    intro='<p><a href="MODE_ACTIONS.md">의미 동작·육면체 모드</a> · <a href="RENDER_CONFIGURATION.md">정점 입력·내부 수정 순서</a> · <a href="PATTERN_COMPOSITION.md">패턴 조립·API 압축</a></p>'+para('함수 이름을 외우는 대신 실제 상황에서 무엇을 저장하고 무엇을 바꾸는지 읽는다. 공개 함수·오버로드·생성자·소멸자·삭제된 복사 연산을 현재 framework/include 헤더에서 수록했다. graphics::detail 및 private 구현은 공개 호출 대상으로 나열하지 않고 공통 구현 설명에서 다룬다.')
     intro+='<p class="note">모든 코드는 현재 선언·구현에 근거한 문서다. 새 사용 사례는 함께 쓸 API를 묶은 함수 단편이며, 독립 main인 예제는 별도로 표시한다. 실행 성공을 확인한 코드라는 뜻은 아니다. 외부 자산 경로는 사용자가 준비한다.</p>'
     body=intro+runtime()+'<section id="cases"><h2>함께 사용하는 API의 실제 사례 코드</h2>'+para('기본 포함은 framework/Framework.h다. 출력 사례는 iostream, 예외 처리는 stdexcept를 추가한다. 아래 독립 함수들은 graphics 이름을 완전히 적었고, 입력 Scene/ID/컨텍스트의 전제는 각 사례에 명시한다.')+'</section>'
     body+=''.join(case(k) for k in GROUPS)

```
