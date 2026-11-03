# Graph Report - framework  (2026-11-03)

## Corpus Check
- 33 files · ~32,156 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 1573 nodes · 2749 edges · 111 communities (85 shown, 10 thin omitted)
- Extraction: 93% EXTRACTED · 7% INFERRED · 0% AMBIGUOUS · INFERRED: 182 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `621b20ce`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
-  코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
-  코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
-  코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
-  코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- examples 코드 구조
- detail 코드 구조
- examples 코드 구조
- examples 코드 구조

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 90 edges
2. `Scene` - 69 edges
3. `Application` - 60 edges
4. `Renderer` - 42 edges
5. `PlaneFigure` - 35 edges
6. `MotionSystem` - 33 edges
7. `CuboidMode` - 31 edges
8. `GlfwApi` - 30 edges
9. `Camera` - 27 edges
10. `Motion` - 26 edges

## Surprising Connections (you probably didn't know these)
- `MotionSystem::popLayer()` --calls--> `removeLayer`  [INFERRED]
  detail/Motion.cpp → include/Motion.h
- `MotionSystem::bindMotion()` --calls--> `setLayer`  [INFERRED]
  detail/Motion.cpp → include/Motion.h
- `PatternSystem::openRoomWall()` --calls--> `openFace`  [INFERRED]
  detail/Patterns.cpp → include/Patterns.h
- `PatternSystem::addWalkingRig()` --calls--> `point`  [INFERRED]
  detail/Patterns.cpp → include/Transform.h
- `PatternSystem::poseWalkingRig()` --calls--> `rotateJoint`  [INFERRED]
  detail/Patterns.cpp → include/Patterns.h

## Import Cycles
- None detected.

## Communities (111 total, 10 thin omitted)

### Community 0 - "detail 코드 구조"
Cohesion: 0.06
Nodes (99): AnimationSystem::poseHumanoid(), appendFace(), CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix(), array, CullMode, EntityId (+91 more)

### Community 1 - "detail 코드 구조"
Cohesion: 0.02
Nodes (86): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+78 more)

### Community 2 - "detail 코드 구조"
Cohesion: 0.03
Nodes (57): Scene::setFaceLocalMatrix, CameraSystem::viewMatrix, Scene::worldMatrix, Scene::getComponent, Scene::generateTexcoords, Scene::lineWorldMatrix, Scene::transformTexcoords, Scene::bindEntity (+49 more)

### Community 3 - "include 코드 구조"
Cohesion: 0.04
Nodes (52): LayerId, map, Scene, addBasis, addCone, addCuboid, addGroup, addLine (+44 more)

### Community 4 - "detail 코드 구조"
Cohesion: 0.08
Nodes (46): AssetLoader::loadObj(), Model, size_t, string, vector, ObjCorner, normal, position (+38 more)

### Community 5 - "detail 코드 구조"
Cohesion: 0.10
Nodes (38): mat4, optional, PathFunction, vec3, vector, motionAround(), motionPlane(), motionUnitVector() (+30 more)

### Community 6 - "detail 코드 구조"
Cohesion: 0.05
Nodes (37): MotionSystem::reparentLayer, MotionSystem::popLayer, Motion::insertAction, MotionSystem::createMotion, MotionSystem::update, Motion::linear, MotionSystem::layerMatrices, MotionSystem::getLayer (+29 more)

### Community 7 - "detail 코드 구조"
Cohesion: 0.06
Nodes (34): DistancePath::sample, DistancePath::setPoints, CollisionSystem::stepRoomBody, CollisionSystem::stepCharacter, PatternSystem::threeViews, PatternSystem::addBox, PatternSystem::snap, PatternSystem::orbitCamera (+26 more)

### Community 8 - "include 코드 구조"
Cohesion: 0.06
Nodes (31): exception_ptr, Application, assets_, callbackError_, camera_, configureRenderer, contextFailed_, contextThread_ (+23 more)

### Community 9 - "include 코드 구조"
Cohesion: 0.09
Nodes (30): CameraSystem, projectionMatrix, viewMatrix, EntityId, optional, Scene, size_t, vec2 (+22 more)

### Community 10 - "include 코드 구조"
Cohesion: 0.07
Nodes (29): GLint, GLuint, vector, Renderer, config_, drawRanges_, ebo_, hasTextureLocation_ (+21 more)

### Community 11 - "detail 코드 구조"
Cohesion: 0.07
Nodes (28): GlfwApi, createWindow, DestroyWindow, GetCurrentContext, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize (+20 more)

### Community 12 - "include 코드 구조"
Cohesion: 0.08
Nodes (25): MotionSystem::clear(), LayerId, map, MotionSystem, bindMotion, clear, createOrbit, createSpin (+17 more)

### Community 13 - "detail 코드 구조"
Cohesion: 0.08
Nodes (24): Application::getFramebufferSize, Application::pickAtCursor, detail::glfw, Application::setKeyHandler, detail::glfw, Application::setInitHandler, Application::setCursorHandler, Application::setRenderHandler (+16 more)

### Community 14 - "include 코드 구조"
Cohesion: 0.23
Nodes (12): CuboidHandler, CuboidMode::action(), EntityId, LayerId, mat3, MotionAction, MotionId, optional (+4 more)

### Community 15 - "include 코드 구조"
Cohesion: 0.10
Nodes (20): CuboidMode, action, apply, bindings_, bindKey, config_, dispatch, entities_ (+12 more)

### Community 16 - "include 코드 구조"
Cohesion: 0.09
Nodes (22): CullMode, GLFace, OutlineMode, PlaneFigure, color, creaseAngleDegrees, cull, independence (+14 more)

### Community 17 - "include 코드 구조"
Cohesion: 0.09
Nodes (22): PatternSystem, addNumber, addPolygon, addRoom, addWalkingRig, aligned, cameraFacingWall, conveyorCornerPose (+14 more)

### Community 18 - "detail 코드 구조"
Cohesion: 0.16
Nodes (18): AnimationSystem::addHumanoid(), AnimationSystem::addSwingWithRider(), AnimationSystem::createJointAnimation(), AnimationSystem::poseHierarchy(), EntityId, optional, Scene, vector (+10 more)

### Community 19 - "include 코드 구조"
Cohesion: 0.12
Nodes (21): MotionAction, size_t, Motion::action(), Motion::append(), Motion::erase(), Motion::evaluate(), Motion::insert(), Motion::insertAction() (+13 more)

### Community 20 - "include 코드 구조"
Cohesion: 0.11
Nodes (20): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+12 more)

### Community 21 - "include 코드 구조"
Cohesion: 0.12
Nodes (19): FeatureOptions, textures, finishLoad(), Model, optional, PivotMode, shared_ptr, string (+11 more)

### Community 22 - "detail 코드 구조"
Cohesion: 0.15
Nodes (17): Application::~Application(), Application::getFramebufferSize(), Application::pickAt(), Application::pickAtCursor(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler(), Application::setKeyHandler() (+9 more)

### Community 23 - "detail 코드 구조"
Cohesion: 0.20
Nodes (17): initializer_list, MotionId, Motion::then(), MotionSystem::createMotion(), MotionSystem::createOrbit(), MotionSystem::createSpin(), MotionSystem::getMotion(), MotionSystem::removeMotion() (+9 more)

### Community 24 - "include 코드 구조"
Cohesion: 0.11
Nodes (18): HumanoidRig, elbows, feet, forearms, hands, head, hips, knees (+10 more)

### Community 25 - "detail 코드 구조"
Cohesion: 0.15
Nodes (15): bindCuboidMode(), shared_ptr, CuboidMode::apply(), CuboidMode::bindKey(), CuboidMode::dispatch(), CuboidMode::setConfig(), CuboidMode::update(), validCuboid() (+7 more)

### Community 26 - "detail 코드 구조"
Cohesion: 0.25
Nodes (17): Bounds3::contains(), EntityId, optional, Playback, Scene, size_t, vec3, DistancePath::sample() (+9 more)

### Community 27 - "include 코드 구조"
Cohesion: 0.15
Nodes (17): PatternSystem::cameraFacingWall(), PatternSystem::foldFace(), PatternSystem::moveCamera(), PatternSystem::orbitCamera(), PatternSystem::yawCamera(), PatternSystem::zoomCamera(), Camera, farPlane (+9 more)

### Community 28 - "include 코드 구조"
Cohesion: 0.23
Nodes (10): Application::ensureContext(), Application::initialize(), Application::run(), initHandler_, renderHandler_, renderScene, updateHandler_, Diagnostics (+2 more)

### Community 29 - "include 코드 구조"
Cohesion: 0.15
Nodes (14): Motion::execute(), Motion::sample(), map, mat4, evaluate, MotionSample, local, parent (+6 more)

### Community 30 - "detail 코드 구조"
Cohesion: 0.23
Nodes (13): compileShader(), GLenum, GLuint, string, readTextFile(), Renderer::initialize(), Renderer::initShader(), Renderer::upload() (+5 more)

### Community 31 - "include 코드 구조"
Cohesion: 0.17
Nodes (14): faceMatrix(), finiteMatrix(), mat4, Mesh, size_t, uint32_t, uint8_t, vector (+6 more)

### Community 32 - "include 코드 구조"
Cohesion: 0.13
Nodes (15): DrawUniformContext, depthOnly, entity, face, line, model, program, projection (+7 more)

### Community 33 - "include 코드 구조"
Cohesion: 0.13
Nodes (15): Motion::pendulum(), Motion::rotation(), TransformSpace, PendulumMotion, amplitudeDegrees, axis, offsetDegrees, periodSeconds (+7 more)

### Community 34 - "detail 코드 구조"
Cohesion: 0.16
Nodes (12): checkGlAt(), executableDirectory(), frameworkDirectory(), GetError, wglGetProc, PipelineStage, T, relativePathInside() (+4 more)

### Community 35 - "include 코드 구조"
Cohesion: 0.14
Nodes (14): PatternSystem::conveyorCornerPose(), mat4, vec3, PathPose, position, tangent, yawDegrees, RoomBody (+6 more)

### Community 36 - "include 코드 구조"
Cohesion: 0.21
Nodes (11): AssetLoader::loadPrimitive(), AssetLoader, features_, imageCache_, loadModel, loadObj, loadPrimitive, projectRoot_ (+3 more)

### Community 37 - "include 코드 구조"
Cohesion: 0.17
Nodes (9): CharacterStep, blocked, landed, turned, CollisionSystem, jump, randomFreeBox, stepCharacter (+1 more)

### Community 38 - "include 코드 구조"
Cohesion: 0.30
Nodes (9): GridLayout, cells, extent, frame, origin, ivec3, mat4, optional (+1 more)

### Community 39 - "detail 코드 구조"
Cohesion: 0.13
Nodes (14): PathSystem::rectangle, PathSystem::polygon, TransformSystem::translation, PathSystem::ellipse, PathSystem::smoothStep, TransformSystem::rotation, TransformSystem::reflectPlane, PathSystem::polyline (+6 more)

### Community 40 - "include 코드 구조"
Cohesion: 0.15
Nodes (14): array, EntityId, vector, JointAnimation, joint, motion, rest, SwingRig (+6 more)

### Community 41 - "include 코드 구조"
Cohesion: 0.14
Nodes (14): ApplicationConfig, diagnostics, features, height, projectRoot, renderConfig, runtimeDirectory, shaderDirectory (+6 more)

### Community 42 - "include 코드 구조"
Cohesion: 0.15
Nodes (13): CuboidMode::setCuboid(), CuboidConfig, color, faces, motion, name, size, transform (+5 more)

### Community 43 - "detail 코드 구조"
Cohesion: 0.32
Nodes (13): LayerId, optional, MotionSystem::bindMotion(), MotionSystem::insertLayer(), MotionSystem::layerMatrices(), MotionSystem::popLayer(), MotionSystem::pushLayer(), MotionSystem::removeLayer() (+5 more)

### Community 44 - "include 코드 구조"
Cohesion: 0.17
Nodes (13): MotionSystem::getLayer(), MotionId, optional, vector, LayerConfig, enabled, motion, space (+5 more)

### Community 45 - "include 코드 구조"
Cohesion: 0.15
Nodes (13): Renderer::renderPass(), draw, upload, RenderPass, camera, clearColor, clearDepth, viewport (+5 more)

### Community 46 - "include 코드 구조"
Cohesion: 0.21
Nodes (10): HRESULT, checkCom(), ComApartment, result, ComOwner, value, decodeImage(), T (+2 more)

### Community 47 - "include 코드 구조"
Cohesion: 0.15
Nodes (13): CullMode, vec3, RenderOptions, background, cull, depthTest, lightColor, lightDirection (+5 more)

### Community 48 - "include 코드 구조"
Cohesion: 0.23
Nodes (12): bottomCenter(), bounds(), mat3, Model, vec3, Transform3D, basis, localBasis (+4 more)

### Community 49 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): CuboidOperation, CuboidCommand, degrees, enabled, layer, motion, operation, transform (+4 more)

### Community 50 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): Motion::followPath(), PathFunction, PathMotion, durationSeconds, interpolation, phase, playback, rate (+4 more)

### Community 51 - "include 코드 구조"
Cohesion: 0.23
Nodes (12): Bounds3::overlaps(), CollisionSystem::randomFreeBox(), CollisionSystem::stepCharacter(), horizontalOverlap(), PatternSystem::addRoom(), Bounds3, contains, maximum (+4 more)

### Community 52 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): Diagnostic, action, api, cause, file, line, object, operation (+4 more)

### Community 53 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount, outlineFirstIndex (+4 more)

### Community 54 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): UniformBindings, hasTexture, lightColor, lightDirection, lighting, model, mvp, normal (+4 more)

### Community 55 - "detail 코드 구조"
Cohesion: 0.17
Nodes (11): CuboidMode::layer, CuboidMode::update, CuboidMode::bindKey, CuboidMode::motion, CuboidMode::setConfig, CuboidMode::dispatch, CuboidMode::unbindKey, CuboidMode::action (+3 more)

### Community 56 - "include 코드 구조"
Cohesion: 0.25
Nodes (9): Application::configureRenderer(), main(), main(), pickAtCursor, requestClose, run, setInitHandler, setKeyHandler (+1 more)

### Community 57 - " 코드 구조"
Cohesion: 0.29
Nodes (11): EntityId, LayerId, map, MotionId, optional, string, CuboidMode::entity(), CuboidMode::layer() (+3 more)

### Community 58 - "include 코드 구조"
Cohesion: 0.18
Nodes (11): CollisionSystem::jump(), CharacterBody, direction, feet, grounded, halfWidth, height, speed (+3 more)

### Community 59 - "include 코드 구조"
Cohesion: 0.20
Nodes (11): CollisionSystem::stepRoomBody(), mat4, PatternSystem::poseWalkingRig(), rigidRoom(), array, EntityId, JointRig, head (+3 more)

### Community 60 - "include 코드 구조"
Cohesion: 0.18
Nodes (11): Motion, action, erase, execute, fixed, insertAction, linear, pendulum (+3 more)

### Community 61 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): Motion::linear(), Playback, vec3, LinearMotion, durationSeconds, from, playback, space (+2 more)

### Community 62 - "detail 코드 구조"
Cohesion: 0.27
Nodes (8): AnimationClock::advance(), ivec3, vector, DistancePath::setPoints(), PatternSystem::addPolygon(), PatternSystem::aligned(), PatternSystem::snakeCells(), PatternSystem::threeViews()

### Community 63 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): CuboidFaceConfig, color, cull, transform, visible, CullMode, GlColor, blue (+2 more)

### Community 64 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): OutlineMode, vec3, LoadOptions, allowObjFallback, customPivot, flipTextureRows, mergeCoplanarFaces, outline (+2 more)

### Community 65 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): GLenum, GLint, GLuint, VertexAttribute, components, integer, location, normalized (+2 more)

### Community 66 - "include 코드 구조"
Cohesion: 0.22
Nodes (9): vec3, SwingConfig, length, pendulum, person, pivot, riderSwayDegrees, ropeThickness (+1 more)

### Community 67 - "include 코드 구조"
Cohesion: 0.22
Nodes (9): HumanoidPose, elbows, hips, knees, neckDegrees, pelvisDegrees, seated, shoulders (+1 more)

### Community 68 - "detail 코드 구조"
Cohesion: 0.22
Nodes (8): Renderer::destroy, Renderer::render, Renderer::renderPass, Renderer::renderViews, Renderer::upload, Renderer::initShader, Renderer::draw, Renderer::initialize

### Community 69 - "include 코드 구조"
Cohesion: 0.25
Nodes (7): PatternSystem::follow(), DistancePath, distances_, points_, sample, setPoints, vector

### Community 70 - "include 코드 구조"
Cohesion: 0.25
Nodes (8): HumanoidConfig, grid, headColor, leftColor, rightColor, torsoColor, upperArmFraction, upperLegFraction

### Community 71 - "include 코드 구조"
Cohesion: 0.25
Nodes (8): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY

### Community 72 - " 코드 구조"
Cohesion: 0.33
Nodes (7): mat4, Scene, vector, Renderer::draw(), Renderer::render(), Renderer::renderViews(), renderPass

### Community 73 - "include 코드 구조"
Cohesion: 0.29
Nodes (6): RenderVertex, color, normal, position, uv, Scene

### Community 74 - "detail 코드 구조"
Cohesion: 0.29
Nodes (6): AnimationSystem::addHumanoid, AnimationSystem::poseHierarchy, AnimationSystem::addSwingWithRider, HumanoidPose::seated, AnimationSystem::createJointAnimation, AnimationSystem::poseHumanoid

### Community 75 - "detail 코드 구조"
Cohesion: 0.33
Nodes (6): Application::renderScene(), Application::renderViews(), Application::shutdown(), vector, ensureContext, getFramebufferSize

### Community 76 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): GLShape, string, Polyhedron, faces, name, shape

### Community 77 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): shared_ptr, Material, color, repeatTexture, texture, useTexture

### Community 78 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): vec2, Vertex, color, normal, position, uv

### Community 79 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): function, RenderConfig, bindUniforms, packVertices, uniforms, vertexLayout

### Community 80 - " 코드 구조"
Cohesion: 0.40
Nodes (5): mat4, TransformSpace, Motion::fixed(), MotionSystem::validLayer(), validAffine

### Community 81 - "detail 코드 구조"
Cohesion: 0.50
Nodes (3): Module, handle_, HMODULE

### Community 82 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): CuboidModeConfig, camera, cuboids, renderOptions, vector

### Community 83 - "include 코드 구조"
Cohesion: 0.40
Nodes (4): AnimationClock, advance, playing, seconds

### Community 84 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): size_t, vector, VertexLayout, attributes, stride

## Knowledge Gaps
- **793 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `SetErrorCallback` (+788 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 959 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **10 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Application` connect `include 코드 구조` to `detail 코드 구조`, `include 코드 구조`, `include 코드 구조`, `detail 코드 구조`, `include 코드 구조`, `detail 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.006) - this node is a cross-community bridge._
- **Why does `Renderer` connect `include 코드 구조` to `include 코드 구조`, ` 코드 구조`, `include 코드 구조`, `include 코드 구조`, ` 코드 구조`, `include 코드 구조`, `include 코드 구조`, `detail 코드 구조`?**
  _High betweenness centrality (0.004) - this node is a cross-community bridge._
- **Why does `Scene` connect `include 코드 구조` to `detail 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, ` 코드 구조`, `include 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.003) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _793 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.06309454474859251 - nodes in this community are weakly interconnected._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.022988505747126436 - nodes in this community are weakly interconnected._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.034482758620689655 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.
