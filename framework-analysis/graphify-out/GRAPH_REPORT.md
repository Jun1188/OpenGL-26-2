# Graph Report - framework  (2026-11-03)

## Corpus Check
- 33 files · ~32,800 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 1602 nodes · 2817 edges · 113 communities (89 shown, 8 thin omitted)
- Extraction: 93% EXTRACTED · 7% INFERRED · 0% AMBIGUOUS · INFERRED: 184 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `ea1ebb7d`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
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
- detail 코드 구조
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
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
-  코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
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
2. `Scene` - 70 edges
3. `Application` - 60 edges
4. `Renderer` - 42 edges
5. `PlaneFigure` - 40 edges
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
- `PatternSystem::poseWalkingRig()` --calls--> `rotateJoint`  [INFERRED]
  detail/Patterns.cpp → include/Patterns.h
- `Scene::setPosition()` --calls--> `setTransform`  [INFERRED]
  detail/Scene.cpp → include/Graphics.h
- `Scene::worldMatrix()` --calls--> `getEntityLayer`  [INFERRED]
  detail/Scene.cpp → include/Graphics.h

## Import Cycles
- None detected.

## Communities (113 total, 8 thin omitted)

### Community 0 - "detail 코드 구조"
Cohesion: 0.05
Nodes (103): appendFace(), CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix(), array, CullMode, EntityId, FaceAnimationSpace (+95 more)

### Community 1 - "detail 코드 구조"
Cohesion: 0.02
Nodes (86): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+78 more)

### Community 2 - "detail 코드 구조"
Cohesion: 0.03
Nodes (58): Scene::setFaceLocalMatrix, Scene::setMeshVertices, Scene::setVisible, Scene::lineWorldMatrix, Scene::setPosition, Scene::setVertexPosition, Scene::clear, DiagramSystem::makePolyhedron (+50 more)

### Community 3 - "include 코드 구조"
Cohesion: 0.04
Nodes (54): LayerId, map, uint64_t, Scene, addBasis, addCone, addCuboid, addGroup (+46 more)

### Community 4 - "include 코드 구조"
Cohesion: 0.09
Nodes (36): CuboidHandler, EntityId, LayerId, map, MotionId, optional, string, CuboidMode::action() (+28 more)

### Community 5 - "detail 코드 구조"
Cohesion: 0.08
Nodes (47): AssetLoader::loadObj(), Model, size_t, string, vector, ObjCorner, normal, position (+39 more)

### Community 6 - "detail 코드 구조"
Cohesion: 0.05
Nodes (37): MotionSystem::reparentLayer, MotionSystem::popLayer, Motion::insertAction, MotionSystem::createMotion, MotionSystem::update, Motion::linear, MotionSystem::layerMatrices, MotionSystem::getLayer (+29 more)

### Community 7 - "detail 코드 구조"
Cohesion: 0.06
Nodes (35): DistancePath::sample, DistancePath::setPoints, CollisionSystem::stepRoomBody, CollisionSystem::stepCharacter, PatternSystem::aligned, PatternSystem::addBox, PatternSystem::threeViews, Bounds3::overlaps (+27 more)

### Community 8 - "include 코드 구조"
Cohesion: 0.06
Nodes (31): exception_ptr, Application, assets_, callbackError_, camera_, configureRenderer, contextFailed_, contextThread_ (+23 more)

### Community 9 - "include 코드 구조"
Cohesion: 0.08
Nodes (31): Application::pickAt(), Application::pickAtCursor(), optional, pickAt, EntityId, optional, Scene, size_t (+23 more)

### Community 10 - "include 코드 구조"
Cohesion: 0.06
Nodes (30): GLint, GLuint, vector, Renderer, config_, drawRanges_, ebo_, hasTextureLocation_ (+22 more)

### Community 11 - "include 코드 구조"
Cohesion: 0.07
Nodes (29): CullMode, FaceAnimationSpace, FaceTransformMode, GLFace, OutlineMode, uint32_t, vector, PlaneFigure (+21 more)

### Community 12 - "include 코드 구조"
Cohesion: 0.11
Nodes (28): AnimationSystem::addHumanoid(), AnimationSystem::addSwingWithRider(), AnimationSystem::createJointAnimation(), AnimationSystem::poseHierarchy(), AnimationSystem::poseHumanoid(), EntityId, optional, Scene (+20 more)

### Community 13 - "detail 코드 구조"
Cohesion: 0.07
Nodes (28): GlfwApi, createWindow, DestroyWindow, GetCurrentContext, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize (+20 more)

### Community 14 - "include 코드 구조"
Cohesion: 0.08
Nodes (25): MotionSystem::clear(), LayerId, map, MotionSystem, bindMotion, clear, createOrbit, createSpin (+17 more)

### Community 15 - "detail 코드 구조"
Cohesion: 0.08
Nodes (24): Application::getFramebufferSize, Application::pickAtCursor, detail::glfw, Application::setKeyHandler, detail::glfw, Application::setInitHandler, Application::setCursorHandler, Application::setRenderHandler (+16 more)

### Community 16 - "detail 코드 구조"
Cohesion: 0.25
Nodes (20): mat4, optional, PathFunction, vec3, vector, motionAround(), motionPlane(), motionUnitVector() (+12 more)

### Community 17 - "include 코드 구조"
Cohesion: 0.10
Nodes (21): bindCuboidMode(), shared_ptr, CuboidMode, action, apply, bindings_, bindKey, config_ (+13 more)

### Community 18 - "include 코드 구조"
Cohesion: 0.09
Nodes (22): PatternSystem, addNumber, addPolygon, addRoom, addWalkingRig, aligned, cameraFacingWall, conveyorCornerPose (+14 more)

### Community 19 - "include 코드 구조"
Cohesion: 0.12
Nodes (21): MotionAction, size_t, Motion::action(), Motion::append(), Motion::erase(), Motion::evaluate(), Motion::insert(), Motion::insertAction() (+13 more)

### Community 20 - "include 코드 구조"
Cohesion: 0.11
Nodes (20): mat4, Scene, vector, Renderer::draw(), Renderer::render(), Renderer::renderViews(), CullMode, vec3 (+12 more)

### Community 21 - "detail 코드 구조"
Cohesion: 0.20
Nodes (17): initializer_list, MotionId, Motion::then(), MotionSystem::createMotion(), MotionSystem::createOrbit(), MotionSystem::createSpin(), MotionSystem::getMotion(), MotionSystem::removeMotion() (+9 more)

### Community 22 - "include 코드 구조"
Cohesion: 0.11
Nodes (18): HumanoidRig, elbows, feet, forearms, hands, head, hips, knees (+10 more)

### Community 23 - "detail 코드 구조"
Cohesion: 0.23
Nodes (14): AnimationClock::advance(), Bounds3::contains(), ivec3, vector, DistancePath::setPoints(), PatternSystem::aligned(), PatternSystem::moveCamera(), PatternSystem::orbitCamera() (+6 more)

### Community 24 - "detail 코드 구조"
Cohesion: 0.28
Nodes (17): EntityId, optional, Scene, size_t, PatternSystem::addBox(), PatternSystem::addJoint(), PatternSystem::addNumber(), PatternSystem::addPolygon() (+9 more)

### Community 25 - "include 코드 구조"
Cohesion: 0.12
Nodes (17): vec3, HumanoidConfig, grid, headColor, leftColor, rightColor, torsoColor, upperArmFraction (+9 more)

### Community 26 - "include 코드 구조"
Cohesion: 0.12
Nodes (15): DistancePath, distances_, points_, sample, setPoints, mat4, vec3, vector (+7 more)

### Community 27 - "include 코드 구조"
Cohesion: 0.15
Nodes (14): Motion::execute(), Motion::sample(), map, mat4, evaluate, MotionSample, local, parent (+6 more)

### Community 28 - "detail 코드 구조"
Cohesion: 0.23
Nodes (13): compileShader(), GLenum, GLuint, string, readTextFile(), Renderer::initialize(), Renderer::initShader(), Renderer::upload() (+5 more)

### Community 29 - "include 코드 구조"
Cohesion: 0.13
Nodes (16): Renderer::renderPass(), CameraSystem, projectionMatrix, viewMatrix, draw, upload, RenderPass, camera (+8 more)

### Community 30 - "include 코드 구조"
Cohesion: 0.18
Nodes (13): main(), PathSystem, circle, ellipse, linear, polygon, polyline, rectangle (+5 more)

### Community 31 - "include 코드 구조"
Cohesion: 0.13
Nodes (15): DrawUniformContext, depthOnly, entity, face, line, model, program, projection (+7 more)

### Community 32 - "detail 코드 구조"
Cohesion: 0.20
Nodes (11): Application::getFramebufferSize(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler(), Application::setKeyHandler(), Application::setModifiedKeyHandler(), Application::setMouseHandler(), Application::setRenderHandler() (+3 more)

### Community 33 - "include 코드 구조"
Cohesion: 0.13
Nodes (15): Motion::pendulum(), Motion::rotation(), TransformSpace, PendulumMotion, amplitudeDegrees, axis, offsetDegrees, periodSeconds (+7 more)

### Community 34 - "include 코드 구조"
Cohesion: 0.21
Nodes (11): AssetLoader::loadPrimitive(), AssetLoader, features_, imageCache_, loadModel, loadObj, loadPrimitive, projectRoot_ (+3 more)

### Community 35 - "include 코드 구조"
Cohesion: 0.30
Nodes (9): GridLayout, cells, extent, frame, origin, ivec3, mat4, optional (+1 more)

### Community 36 - "include 코드 구조"
Cohesion: 0.15
Nodes (14): FeatureOptions, textures, finishLoad(), Model, optional, vec3, vector, ModelResult (+6 more)

### Community 37 - "detail 코드 구조"
Cohesion: 0.13
Nodes (14): PathSystem::rectangle, PathSystem::polygon, TransformSystem::translation, PathSystem::ellipse, PathSystem::smoothStep, TransformSystem::rotation, TransformSystem::reflectPlane, PathSystem::polyline (+6 more)

### Community 38 - "include 코드 구조"
Cohesion: 0.15
Nodes (14): array, EntityId, vector, JointAnimation, joint, motion, rest, SwingRig (+6 more)

### Community 39 - "include 코드 구조"
Cohesion: 0.14
Nodes (14): ApplicationConfig, diagnostics, features, height, projectRoot, renderConfig, runtimeDirectory, shaderDirectory (+6 more)

### Community 40 - "detail 코드 구조"
Cohesion: 0.32
Nodes (13): LayerId, optional, MotionSystem::bindMotion(), MotionSystem::insertLayer(), MotionSystem::layerMatrices(), MotionSystem::popLayer(), MotionSystem::pushLayer(), MotionSystem::removeLayer() (+5 more)

### Community 41 - "include 코드 구조"
Cohesion: 0.17
Nodes (13): MotionSystem::getLayer(), MotionId, optional, vector, LayerConfig, enabled, motion, space (+5 more)

### Community 42 - "include 코드 구조"
Cohesion: 0.21
Nodes (10): HRESULT, checkCom(), ComApartment, result, ComOwner, value, decodeImage(), T (+2 more)

### Community 43 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): CuboidOperation, CuboidCommand, degrees, enabled, layer, motion, operation, transform (+4 more)

### Community 44 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): CuboidMode::setCuboid(), CuboidConfig, color, faces, motion, name, size, transform (+4 more)

### Community 45 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): Motion::followPath(), PathFunction, PathMotion, durationSeconds, interpolation, phase, playback, rate (+4 more)

### Community 46 - "include 코드 구조"
Cohesion: 0.18
Nodes (12): Scene::setTint(), GlColor, blue, green, red, shared_ptr, Material, color (+4 more)

### Community 47 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): Diagnostic, action, api, cause, file, line, object, operation (+4 more)

### Community 48 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount, outlineFirstIndex (+4 more)

### Community 49 - "include 코드 구조"
Cohesion: 0.17
Nodes (11): FaceId, entityId, faceIndex, EntityId, optional, size_t, Line3D, color (+3 more)

### Community 50 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): UniformBindings, hasTexture, lightColor, lightDirection, lighting, model, mvp, normal (+4 more)

### Community 51 - "detail 코드 구조"
Cohesion: 0.17
Nodes (11): CuboidMode::dispatch, CuboidMode::setCuboid, CuboidMode::action, CuboidMode::motion, CuboidMode::bindKey, CuboidMode::unbindKey, CuboidMode::apply, CuboidMode::update (+3 more)

### Community 52 - "include 코드 구조"
Cohesion: 0.25
Nodes (9): Application::configureRenderer(), main(), main(), pickAtCursor, requestClose, run, setInitHandler, setKeyHandler (+1 more)

### Community 53 - "detail 코드 구조"
Cohesion: 0.22
Nodes (9): checkGlAt(), GetError, wglGetProc, PipelineStage, T, relativePathInside(), resolve(), symbol() (+1 more)

### Community 54 - "include 코드 구조"
Cohesion: 0.25
Nodes (11): Bounds3::overlaps(), CollisionSystem::randomFreeBox(), CollisionSystem::stepCharacter(), horizontalOverlap(), Bounds3, contains, maximum, minimum (+3 more)

### Community 55 - "include 코드 구조"
Cohesion: 0.18
Nodes (11): CollisionSystem::jump(), CharacterBody, direction, feet, grounded, halfWidth, height, speed (+3 more)

### Community 56 - "include 코드 구조"
Cohesion: 0.44
Nodes (10): faceLocalMatrix(), faceMatrix(), faceSrtMatrix(), finiteMatrix(), mat4, Mesh, scale, transformMatrix() (+2 more)

### Community 57 - "include 코드 구조"
Cohesion: 0.22
Nodes (11): bottomCenter(), bounds(), Model, vec2, vec3, Vertex, color, normal (+3 more)

### Community 58 - "include 코드 구조"
Cohesion: 0.18
Nodes (11): Motion, action, erase, execute, fixed, insertAction, linear, pendulum (+3 more)

### Community 59 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): Application::ensureContext(), Application::initialize(), Diagnostics, PipelineStage, options

### Community 60 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): Motion::linear(), Playback, vec3, LinearMotion, durationSeconds, from, playback, space (+2 more)

### Community 61 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): PatternSystem::screenOnPlane(), Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position (+2 more)

### Community 62 - "include 코드 구조"
Cohesion: 0.29
Nodes (4): CharacterStep, blocked, landed, turned

### Community 63 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): CuboidFaceConfig, color, cull, mode, rotation, transform, visible, CullMode (+2 more)

### Community 64 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+2 more)

### Community 65 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): uint8_t, TextureImage, height, rgba, width, shared_ptr, string, TextureResult (+2 more)

### Community 66 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): OutlineMode, PivotMode, LoadOptions, allowObjFallback, customPivot, flipTextureRows, mergeCoplanarFaces, outline (+2 more)

### Community 67 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): GLenum, GLint, GLuint, VertexAttribute, components, integer, location, normalized (+2 more)

### Community 68 - "detail 코드 구조"
Cohesion: 0.22
Nodes (8): Renderer::destroy, Renderer::render, Renderer::renderPass, Renderer::renderViews, Renderer::upload, Renderer::initShader, Renderer::draw, Renderer::initialize

### Community 69 - "include 코드 구조"
Cohesion: 0.25
Nodes (7): Application::~Application(), Application::run(), initHandler_, renderHandler_, renderScene, shutdown, updateHandler_

### Community 70 - "include 코드 구조"
Cohesion: 0.25
Nodes (8): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY

### Community 71 - "include 코드 구조"
Cohesion: 0.29
Nodes (6): CuboidMode::bindKey(), CuboidKeyBinding, action, key, mods, CuboidHandler

### Community 72 - "detail 코드 구조"
Cohesion: 0.52
Nodes (7): CollisionSystem::stepRoomBody(), mat4, vec3, PatternSystem::addWalkingRig(), PatternSystem::follow(), PatternSystem::poseWalkingRig(), rigidRoom()

### Community 73 - "include 코드 구조"
Cohesion: 0.29
Nodes (7): Playback, DistancePath::sample(), PatternSystem::conveyorCornerPose(), PathPose, position, tangent, yawDegrees

### Community 74 - "include 코드 구조"
Cohesion: 0.29
Nodes (7): array, EntityId, JointRig, head, hips, root, shoulders

### Community 75 - "include 코드 구조"
Cohesion: 0.29
Nodes (6): RenderVertex, color, normal, position, uv, Scene

### Community 76 - "detail 코드 구조"
Cohesion: 0.29
Nodes (6): AnimationSystem::addHumanoid, AnimationSystem::poseHierarchy, AnimationSystem::addSwingWithRider, HumanoidPose::seated, AnimationSystem::createJointAnimation, AnimationSystem::poseHumanoid

### Community 77 - "detail 코드 구조"
Cohesion: 0.33
Nodes (6): Application::renderScene(), Application::renderViews(), Application::shutdown(), vector, ensureContext, getFramebufferSize

### Community 78 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): GLShape, string, Polyhedron, faces, name, shape

### Community 79 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): function, RenderConfig, bindUniforms, packVertices, uniforms, vertexLayout

### Community 80 - "detail 코드 구조"
Cohesion: 0.40
Nodes (5): CuboidMode::dispatch(), CuboidMode::update(), Handler, dispatch(), handler

### Community 81 - " 코드 구조"
Cohesion: 0.40
Nodes (5): mat4, TransformSpace, Motion::fixed(), MotionSystem::validLayer(), validAffine

### Community 82 - "detail 코드 구조"
Cohesion: 0.50
Nodes (3): Module, handle_, HMODULE

### Community 83 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): CuboidModeConfig, camera, cuboids, renderOptions, vector

### Community 84 - "include 코드 구조"
Cohesion: 0.40
Nodes (4): AnimationClock, advance, playing, seconds

### Community 85 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): CollisionSystem, jump, randomFreeBox, stepCharacter, stepRoomBody

### Community 86 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): size_t, vector, VertexLayout, attributes, stride

### Community 87 - "detail 코드 구조"
Cohesion: 0.50
Nodes (3): executableDirectory(), frameworkDirectory(), DWORD

### Community 88 - "include 코드 구조"
Cohesion: 0.50
Nodes (4): AxisLine, direction, point, vec3

## Knowledge Gaps
- **806 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `SetErrorCallback` (+801 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 979 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **8 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Application` connect `include 코드 구조` to `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `detail 코드 구조`, `include 코드 구조`, `include 코드 구조`, `detail 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.004) - this node is a cross-community bridge._
- **Why does `Scene` connect `include 코드 구조` to `detail 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.002) - this node is a cross-community bridge._
- **Why does `Renderer` connect `include 코드 구조` to `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `detail 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.002) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _806 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.05457875457875458 - nodes in this community are weakly interconnected._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.022988505747126436 - nodes in this community are weakly interconnected._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.03389830508474576 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.
