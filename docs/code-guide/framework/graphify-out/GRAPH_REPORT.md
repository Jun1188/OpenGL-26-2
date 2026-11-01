# Graph Report - framework  (2026-11-01)

## Corpus Check
- 26 files · ~21,189 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 1269 nodes · 1956 edges · 89 communities (67 shown, 10 thin omitted)
- Extraction: 99% EXTRACTED · 1% INFERRED · 0% AMBIGUOUS · INFERRED: 28 edges (avg confidence: 0.82)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `819f5545`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
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
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
-  코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- examples 코드 구조
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
-  코드 구조
- include 코드 구조
-  코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- examples 코드 구조
- examples 코드 구조
- examples 코드 구조

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 86 edges
2. `Scene` - 71 edges
3. `Application` - 55 edges
4. `Renderer` - 40 edges
5. `MotionSystem` - 35 edges
6. `PlaneFigure` - 34 edges
7. `GlfwApi` - 30 edges
8. `PatternSystem` - 25 edges
9. `Component_Poly` - 18 edges
10. `main()` - 17 edges

## Surprising Connections (you probably didn't know these)
- `main()` --references--> `Application`  [INFERRED]
  examples/assets.cpp → include/Application.h
- `main()` --calls--> `pickAtCursor`  [INFERRED]
  examples/assets.cpp → include/Application.h
- `main()` --references--> `Application`  [INFERRED]
  examples/basic.cpp → include/Application.h
- `main()` --calls--> `pickAt`  [INFERRED]
  examples/basic.cpp → include/Application.h
- `main()` --references--> `Application`  [INFERRED]
  examples/motion.cpp → include/Application.h

## Import Cycles
- None detected.

## Communities (89 total, 10 thin omitted)

### Community 0 - "detail 코드 구조"
Cohesion: 0.02
Nodes (82): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+74 more)

### Community 1 - "detail 코드 구조"
Cohesion: 0.03
Nodes (58): DiagramSystem::makeSphere, DiagramSystem::makeCone, Scene::setMeshVertices, DiagramSystem::modelMatrix, Scene::worldMatrix, DiagramSystem::validTransform, Scene::setSideLengths, Scene::addSphere (+50 more)

### Community 2 - "include 코드 구조"
Cohesion: 0.03
Nodes (57): LayerId, Scene, addBasis, addCone, addCuboid, addGroup, addLine, addModel (+49 more)

### Community 3 - "detail 코드 구조"
Cohesion: 0.08
Nodes (41): AssetLoader::loadObj(), ObjCorner, normal, position, uv, ObjFace, batch, corners (+33 more)

### Community 4 - "include 코드 구조"
Cohesion: 0.08
Nodes (32): checkGlAt(), GetError, wglGetProc, PipelineStage, T, resolve(), symbol(), Diagnostic (+24 more)

### Community 5 - "detail 코드 구조"
Cohesion: 0.08
Nodes (41): Motion::append(), Motion::erase(), Motion::fixed(), Motion::followPath(), Motion::insert(), Motion::replace(), Motion::rotation(), Motion::sample() (+33 more)

### Community 6 - "detail 코드 구조"
Cohesion: 0.10
Nodes (35): appendFace(), DiagramSystem::makeCone(), DiagramSystem::makePolyhedron(), DiagramSystem::modelMatrix(), DiagramSystem::validPolyhedron(), DiagramSystem::validTransform(), nextGeometryRevision(), positiveLengths() (+27 more)

### Community 7 - "include 코드 구조"
Cohesion: 0.06
Nodes (34): exception_ptr, Application, assets_, callbackError_, camera_, contextFailed_, contextThread_, cursorHandler_ (+26 more)

### Community 8 - "detail 코드 구조"
Cohesion: 0.06
Nodes (34): PatternSystem::snap, PatternSystem::follow, AnimationClock::advance, Bounds3::valid, DistancePath::sample, PatternSystem::swapModels, PatternSystem::foldFace, Bounds3::overlaps (+26 more)

### Community 9 - "detail 코드 구조"
Cohesion: 0.06
Nodes (33): MotionSystem::setMotionPlaying, Motion::followPath, MotionSystem::createPathAndSpin, MotionSystem::setMotion, MotionSystem::removeMotion, MotionSystem::bindMotion, MotionSystem::createOrbitAndSpin, Motion::append (+25 more)

### Community 10 - "detail 코드 구조"
Cohesion: 0.06
Nodes (31): GlfwApi, createWindow, DestroyWindow, GetCurrentContext, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize (+23 more)

### Community 11 - "include 코드 구조"
Cohesion: 0.06
Nodes (32): GLint, vector, Renderer, destroy, draw, drawRanges_, ebo_, hasTextureLocation_ (+24 more)

### Community 12 - "include 코드 구조"
Cohesion: 0.07
Nodes (30): LayerId, map, MotionSystem, bindMotion, clear, createMotion, createOrbit, createOrbitAndSpin (+22 more)

### Community 13 - "include 코드 구조"
Cohesion: 0.08
Nodes (29): CullMode, OutlineMode, string, uint32_t, vector, PlaneFigure, color, creaseAngleDegrees (+21 more)

### Community 14 - "detail 코드 구조"
Cohesion: 0.10
Nodes (24): AxisLine, Bounds3, CharacterBody, CharacterStep, Bounds3::contains(), Bounds3::overlaps(), CollisionSystem::jump(), CollisionSystem::randomFreeBox() (+16 more)

### Community 15 - "detail 코드 구조"
Cohesion: 0.17
Nodes (26): PatternSystem::addBox(), PatternSystem::addJoint(), PatternSystem::addNumber(), PatternSystem::addPolygon(), PatternSystem::addRoom(), PatternSystem::cameraFacingWall(), PatternSystem::openFace(), PatternSystem::openRoomWall() (+18 more)

### Community 16 - "include 코드 구조"
Cohesion: 0.08
Nodes (25): PatternSystem, addBox, addJoint, addNumber, addPolygon, addRoom, addWalkingRig, aligned (+17 more)

### Community 17 - "detail 코드 구조"
Cohesion: 0.21
Nodes (21): PatternSystem::follow(), Scene::setPosition(), motionAround(), motionPlane(), motionUnitVector(), PathSystem::circle(), PathSystem::ellipse(), PathSystem::polygon() (+13 more)

### Community 18 - "detail 코드 구조"
Cohesion: 0.08
Nodes (23): Application::setMouseHandler, Application::renderScene, Application::setUpdateHandler, Application::setCursorHandler, Application::setInitHandler, Application::ensureContext, Application::getFramebufferSize, detail::glfw (+15 more)

### Community 19 - "detail 코드 구조"
Cohesion: 0.13
Nodes (16): ApplicationConfig, Application::getFramebufferSize(), Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler(), Application::setFocusHandler() (+8 more)

### Community 20 - "include 코드 구조"
Cohesion: 0.11
Nodes (20): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+12 more)

### Community 21 - "include 코드 구조"
Cohesion: 0.18
Nodes (17): CameraSystem, projectionMatrix, validCamera, viewMatrix, optional, vec3, pick(), PickViewport (+9 more)

### Community 22 - "detail 코드 구조"
Cohesion: 0.13
Nodes (17): CullMode, Scene::generateTexcoords(), Scene::getMesh(), Scene::setFaceCullMode(), Scene::setFaceLocalMatrix(), Scene::setFaceVisible(), Scene::setMaterial(), Scene::setMesh() (+9 more)

### Community 23 - "include 코드 구조"
Cohesion: 0.18
Nodes (16): main(), PathSystem, circle, ellipse, linear, polygon, polyline, rectangle (+8 more)

### Community 24 - "detail 코드 구조"
Cohesion: 0.16
Nodes (15): Application::renderViews(), DistancePath::setPoints(), PatternSystem::snakeCells(), Renderer::renderPass(), Renderer::renderViews(), DiagramSystem::makeSphere(), Scene::setMeshVertices(), Scene::setOutline() (+7 more)

### Community 25 - "include 코드 구조"
Cohesion: 0.20
Nodes (11): relativePathInside(), AssetLoader, features_, imageCache_, loadModel, loadObj, loadPrimitive, projectRoot_ (+3 more)

### Community 26 - "detail 코드 구조"
Cohesion: 0.13
Nodes (14): TransformSystem::validAffine, PathSystem::polyline, TransformSystem::reflectLine, TransformSystem::rotation, TransformSystem::translation, PathSystem::smoothStep, TransformSystem::reflectPoint, PathSystem::ellipse (+6 more)

### Community 27 - "include 코드 구조"
Cohesion: 0.30
Nodes (13): appendMesh(), generateTexcoords(), Mesh, Model, uint32_t, vec2, vec3, mergeCoplanar() (+5 more)

### Community 28 - "include 코드 구조"
Cohesion: 0.16
Nodes (14): finiteVector(), validMesh(), finishLoad(), Model, optional, PivotMode, vector, ModelResult (+6 more)

### Community 29 - "detail 코드 구조"
Cohesion: 0.17
Nodes (13): Camera, PatternSystem::moveCamera(), PatternSystem::screenOnPlane(), PatternSystem::threeViews(), PatternSystem::yawCamera(), PatternSystem::zoomCamera(), Renderer::draw(), Renderer::render() (+5 more)

### Community 30 - "include 코드 구조"
Cohesion: 0.15
Nodes (13): ApplicationConfig, diagnostics, features, height, projectRoot, runtimeDirectory, shaderDirectory, title (+5 more)

### Community 31 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): map, function, mat4, MotionSample, local, parent, MotionStep, durationSeconds (+4 more)

### Community 32 - "include 코드 구조"
Cohesion: 0.15
Nodes (13): EntityId, size_t, vec2, PickResult, barycentric, distance, entityId, faceIndex (+5 more)

### Community 33 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): GLsizei, DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount (+4 more)

### Community 34 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): CullMode, RenderOptions, background, cull, depthTest, lightColor, lightDirection, lighting (+4 more)

### Community 35 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): Motion, append, erase, fixed, followPath, insert, replace, rotation (+4 more)

### Community 36 - "include 코드 구조"
Cohesion: 0.25
Nodes (11): main(), main(), Handler, pickAt, pickAtCursor, requestClose, run, setInitHandler (+3 more)

### Community 37 - "include 코드 구조"
Cohesion: 0.22
Nodes (11): bottomCenter(), bounds(), Model, vec2, vec3, Vertex, color, normal (+3 more)

### Community 38 - "include 코드 구조"
Cohesion: 0.22
Nodes (11): faceMatrix(), finiteMatrix(), mat4, Mesh, Transform3D, basis, localBasis, rotation (+3 more)

### Community 39 - "include 코드 구조"
Cohesion: 0.18
Nodes (11): GlColor, blue, green, red, shared_ptr, Material, color, repeatTexture (+3 more)

### Community 40 - "include 코드 구조"
Cohesion: 0.20
Nodes (11): optional, vector, LayerConfig, enabled, motion, space, transform, LayerState (+3 more)

### Community 41 - "include 코드 구조"
Cohesion: 0.27
Nodes (7): HRESULT, checkCom(), ComApartment, result, ComOwner, value, T

### Community 42 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+2 more)

### Community 43 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): RenderPass, camera, clearColor, clearDepth, viewport, Viewport, height, width (+2 more)

### Community 44 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): OutlineMode, vec3, LoadOptions, allowObjFallback, customPivot, flipTextureRows, mergeCoplanarFaces, outline (+2 more)

### Community 45 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): PathMotion, durationSeconds, interpolation, phase, playback, rate, relativeToStart, space (+2 more)

### Community 46 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): CharacterBody, direction, feet, grounded, halfWidth, height, speed, verticalSpeed (+2 more)

### Community 47 - "include 코드 구조"
Cohesion: 0.22
Nodes (9): mat4, vec3, RoomBody, halfExtent, localPosition, localVelocity, released, worldPosition (+1 more)

### Community 48 - "detail 코드 구조"
Cohesion: 0.22
Nodes (8): Renderer::initShader, Renderer::destroy, Renderer::render, Renderer::draw, Renderer::upload, Renderer::renderPass, Renderer::renderViews, Renderer::initialize

### Community 49 - " 코드 구조"
Cohesion: 0.54
Nodes (4): array, optional, string, vector

### Community 50 - "include 코드 구조"
Cohesion: 0.25
Nodes (8): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY

### Community 51 - "include 코드 구조"
Cohesion: 0.25
Nodes (8): DiagramSystem, faceModelMatrix, makeCone, makePolyhedron, makeSphere, modelMatrix, validPolyhedron, validTransform

### Community 52 - "include 코드 구조"
Cohesion: 0.25
Nodes (7): FeatureOptions, textures, shared_ptr, string, TextureResult, error, image

### Community 53 - "detail 코드 구조"
Cohesion: 0.29
Nodes (6): executableDirectory(), frameworkDirectory(), DWORD, decodeImage(), uint8_t, vector

### Community 55 - "include 코드 구조"
Cohesion: 0.29
Nodes (7): UVOptions, axis, mapping, offset, rotationDegrees, scale, UVMapping

### Community 56 - "include 코드 구조"
Cohesion: 0.29
Nodes (6): size_t, uint8_t, TextureImage, height, rgba, width

### Community 57 - "include 코드 구조"
Cohesion: 0.29
Nodes (6): DistancePath, distances_, points_, sample, setPoints, vector

### Community 58 - "include 코드 구조"
Cohesion: 0.33
Nodes (4): CharacterStep, blocked, landed, turned

### Community 59 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): MotionState, enabled, motion, playing, sampled, timeSeconds

### Community 60 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): RotationMotion, axis, degreesPerSecond, initialDegrees, space, TransformSpace

### Community 61 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): Bounds3, contains, maximum, minimum, overlaps, valid

### Community 62 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): EntityId, JointRig, head, hips, root, shoulders

### Community 63 - "include 코드 구조"
Cohesion: 0.40
Nodes (4): AnimationClock, advance, playing, seconds

### Community 64 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): CollisionSystem, jump, randomFreeBox, stepCharacter, stepRoomBody

### Community 65 - "include 코드 구조"
Cohesion: 0.40
Nodes (4): AxisLine, direction, point, vec3

### Community 66 - " 코드 구조"
Cohesion: 0.50
Nodes (4): Component_Poly, DiagramSystem::faceModelMatrix(), Scene::getComponent(), PlaneFigure

### Community 67 - "include 코드 구조"
Cohesion: 0.50
Nodes (4): PathPose, position, tangent, yawDegrees

## Knowledge Gaps
- **685 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `SetErrorCallback` (+680 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 815 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **10 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Application` connect `include 코드 구조` to `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, ` 코드 구조`, `detail 코드 구조`, `examples 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.001) - this node is a cross-community bridge._
- **Why does `Scene` connect `include 코드 구조` to `include 코드 구조`, ` 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.001) - this node is a cross-community bridge._
- **Why does `GlApi` connect `detail 코드 구조` to `detail 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.001) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _685 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.024096385542168676 - nodes in this community are weakly interconnected._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.03389830508474576 - nodes in this community are weakly interconnected._
- **Should `include 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.034482758620689655 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.
