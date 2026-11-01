# Graph Report - OpenGL-26-2  (2026-11-01)

## Corpus Check
- Large corpus: 475 files · ~3,270,080 words. Semantic extraction will be expensive (many Claude tokens). Consider running on a subfolder.

## Summary
- 1314 nodes · 2017 edges · 90 communities (70 shown, 6 thin omitted)
- Extraction: 98% EXTRACTED · 2% INFERRED · 0% AMBIGUOUS · INFERRED: 32 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `f49ee637`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_21 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_21 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
-  코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
-  코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
-  코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 86 edges
2. `Scene` - 71 edges
3. `Application` - 54 edges
4. `Renderer` - 40 edges
5. `MotionSystem` - 35 edges
6. `PlaneFigure` - 34 edges
7. `Console` - 31 edges
8. `GlfwApi` - 30 edges
9. `PatternSystem` - 25 edges
10. `Component_Poly` - 18 edges

## Surprising Connections (you probably didn't know these)
- `addBall` --calls--> `overlaps`  [INFERRED]
  openGL_3dAssign_21/Console.h → framework/include/Patterns.h
- `main()` --calls--> `run`  [INFERRED]
  openGL_3dAssign_21/main.cpp → framework/include/Application.h
- `main()` --calls--> `setCursorHandler`  [INFERRED]
  openGL_3dAssign_21/main.cpp → framework/include/Application.h
- `main()` --calls--> `setFocusHandler`  [INFERRED]
  openGL_3dAssign_21/main.cpp → framework/include/Application.h
- `main()` --calls--> `setInitHandler`  [INFERRED]
  openGL_3dAssign_21/main.cpp → framework/include/Application.h

## Import Cycles
- None detected.

## Communities (90 total, 6 thin omitted)

### Community 0 - "framework 코드 구조"
Cohesion: 0.02
Nodes (82): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+74 more)

### Community 1 - "framework 코드 구조"
Cohesion: 0.03
Nodes (58): Scene::worldMatrix, Scene::setFaceVertexColors, Scene::Scene, Scene::bindEntity, CameraSystem::validCamera, Scene::setVisible, Scene::setFaceTransform, Scene::addCuboid (+50 more)

### Community 2 - "framework 코드 구조"
Cohesion: 0.03
Nodes (57): LayerId, Scene, addBasis, addCone, addCuboid, addGroup, addLine, addModel (+49 more)

### Community 3 - "framework 코드 구조"
Cohesion: 0.07
Nodes (46): Application::renderViews(), AssetLoader::loadObj(), ObjCorner, normal, position, uv, ObjFace, batch (+38 more)

### Community 4 - "openGL_3dAssign_21 코드 구조"
Cohesion: 0.06
Nodes (48): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY (+40 more)

### Community 5 - "framework 코드 구조"
Cohesion: 0.05
Nodes (45): exception_ptr, Application, assets_, callbackError_, camera_, contextFailed_, contextThread_, cursorHandler_ (+37 more)

### Community 6 - "framework 코드 구조"
Cohesion: 0.07
Nodes (41): AxisLine, Bounds3, Camera, CharacterBody, CharacterStep, DistancePath, Bounds3::contains(), Bounds3::overlaps() (+33 more)

### Community 7 - "framework 코드 구조"
Cohesion: 0.09
Nodes (40): Motion::append(), Motion::followPath(), Motion::insert(), Motion::replace(), Motion::rotation(), Motion::sample(), Motion::then(), Motion::validStep() (+32 more)

### Community 8 - "framework 코드 구조"
Cohesion: 0.10
Nodes (34): appendFace(), DiagramSystem::makeCone(), DiagramSystem::makePolyhedron(), DiagramSystem::modelMatrix(), DiagramSystem::validPolyhedron(), DiagramSystem::validTransform(), nextGeometryRevision(), positiveLengths() (+26 more)

### Community 9 - "framework 코드 구조"
Cohesion: 0.06
Nodes (34): PatternSystem::addNumber, PatternSystem::addWalkingRig, Bounds3::valid, PatternSystem::orbitCamera, PatternSystem::cameraFacingWall, PatternSystem::addPolygon, AnimationClock::advance, Bounds3::contains (+26 more)

### Community 10 - "framework 코드 구조"
Cohesion: 0.06
Nodes (33): MotionSystem::createSpin, MotionSystem::setLayer, Motion::rotation, MotionSystem::bindMotion, MotionSystem::update, MotionSystem::createPathAndSpin, MotionSystem::removeLayer, MotionSystem::reparentLayer (+25 more)

### Community 11 - "framework 코드 구조"
Cohesion: 0.06
Nodes (31): GlfwApi, createWindow, DestroyWindow, GetCurrentContext, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize (+23 more)

### Community 12 - "framework 코드 구조"
Cohesion: 0.06
Nodes (32): vector, Renderer, destroy, draw, drawRanges_, ebo_, hasTextureLocation_, initialize (+24 more)

### Community 13 - "framework 코드 구조"
Cohesion: 0.07
Nodes (30): LayerId, map, MotionSystem, bindMotion, clear, createMotion, createOrbit, createOrbitAndSpin (+22 more)

### Community 14 - "framework 코드 구조"
Cohesion: 0.08
Nodes (28): CullMode, OutlineMode, string, uint32_t, vector, PlaneFigure, color, creaseAngleDegrees (+20 more)

### Community 15 - "framework 코드 구조"
Cohesion: 0.09
Nodes (26): CullMode, Motion::erase(), DiagramSystem::makeSphere(), Scene::generateTexcoords(), Scene::getMesh(), Scene::setFaceCullMode(), Scene::setFaceLocalMatrix(), Scene::setFaceVisible() (+18 more)

### Community 16 - "framework 코드 구조"
Cohesion: 0.20
Nodes (22): CollisionSystem::stepRoomBody(), rigidRoom(), Scene::setPosition(), motionAround(), motionPlane(), motionUnitVector(), PathSystem::circle(), PathSystem::ellipse() (+14 more)

### Community 17 - "framework 코드 구조"
Cohesion: 0.08
Nodes (25): PatternSystem, addBox, addJoint, addNumber, addPolygon, addRoom, addWalkingRig, aligned (+17 more)

### Community 18 - "framework 코드 구조"
Cohesion: 0.08
Nodes (23): Application::setUpdateHandler, detail::glfw, detail::glfw, Application::setMouseHandler, Application::renderScene, Application::shutdown, Application::renderViews, Application::run (+15 more)

### Community 19 - "framework 코드 구조"
Cohesion: 0.13
Nodes (16): ApplicationConfig, Application::getFramebufferSize(), Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler(), Application::setFocusHandler() (+8 more)

### Community 20 - "framework 코드 구조"
Cohesion: 0.20
Nodes (21): EntityId, PatternSystem::addBox(), PatternSystem::addJoint(), PatternSystem::addNumber(), PatternSystem::addPolygon(), PatternSystem::addRoom(), Scene::addCone(), Scene::addCuboid() (+13 more)

### Community 21 - "framework 코드 구조"
Cohesion: 0.17
Nodes (20): appendMesh(), generateTexcoords(), Mesh, Model, uint32_t, vec2, vec3, mergeCoplanar() (+12 more)

### Community 22 - "framework 코드 구조"
Cohesion: 0.18
Nodes (17): CameraSystem, projectionMatrix, validCamera, viewMatrix, optional, vec3, pick(), PickViewport (+9 more)

### Community 23 - "framework 코드 구조"
Cohesion: 0.28
Nodes (10): GridLayout, cells, extent, frame, origin, mat4, optional, vec3 (+2 more)

### Community 24 - "framework 코드 구조"
Cohesion: 0.20
Nodes (11): relativePathInside(), AssetLoader, features_, imageCache_, loadModel, loadObj, loadPrimitive, projectRoot_ (+3 more)

### Community 25 - "framework 코드 구조"
Cohesion: 0.15
Nodes (14): FeatureOptions, textures, finishLoad(), Model, optional, PivotMode, vector, ModelResult (+6 more)

### Community 26 - "openGL_3dAssign_21 코드 구조"
Cohesion: 0.18
Nodes (13): main, Console::onFocus, Console::onMouse, Console::onCursor, Console::onUpdate, Console::printHelp, Console::initScene, Console::addBall (+5 more)

### Community 27 - "framework 코드 구조"
Cohesion: 0.13
Nodes (14): PathSystem::circle, TransformSystem::reflectLine, PathSystem::rectangle, TransformSystem::reflectPlane, PathSystem::polygon, TransformSystem::reflectPoint, TransformSystem::scaling, PathSystem::linear (+6 more)

### Community 28 - "framework 코드 구조"
Cohesion: 0.14
Nodes (12): AxisLine, direction, point, vec3, PathSystem, circle, ellipse, linear (+4 more)

### Community 29 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): map, function, mat4, MotionSample, local, parent, MotionStep, durationSeconds (+4 more)

### Community 30 - "framework 코드 구조"
Cohesion: 0.15
Nodes (13): EntityId, size_t, vec2, PickResult, barycentric, distance, entityId, faceIndex (+5 more)

### Community 31 - "framework 코드 구조"
Cohesion: 0.21
Nodes (10): checkCom(), ComApartment, result, ComOwner, value, decodeImage(), T, uint8_t (+2 more)

### Community 32 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount, outlineFirstIndex (+4 more)

### Community 33 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): CullMode, RenderOptions, background, cull, depthTest, lightColor, lightDirection, lighting (+4 more)

### Community 34 - "framework 코드 구조"
Cohesion: 0.17
Nodes (11): FaceId, entityId, faceIndex, EntityId, optional, size_t, Line3D, color (+3 more)

### Community 35 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): Motion, append, erase, fixed, followPath, insert, replace, rotation (+4 more)

### Community 36 - "framework 코드 구조"
Cohesion: 0.18
Nodes (11): Diagnostic, action, api, cause, file, line, object, operation (+3 more)

### Community 37 - "framework 코드 구조"
Cohesion: 0.36
Nodes (4): Diagnostics, PipelineStage, string, options

### Community 38 - "framework 코드 구조"
Cohesion: 0.22
Nodes (11): bottomCenter(), bounds(), Model, vec2, vec3, Vertex, color, normal (+3 more)

### Community 39 - "framework 코드 구조"
Cohesion: 0.20
Nodes (11): optional, vector, LayerConfig, enabled, motion, space, transform, LayerState (+3 more)

### Community 40 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): ApplicationConfig, diagnostics, features, height, projectRoot, runtimeDirectory, shaderDirectory, title (+2 more)

### Community 41 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+2 more)

### Community 42 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): RenderPass, camera, clearColor, clearDepth, viewport, Viewport, height, width (+2 more)

### Community 43 - "framework 코드 구조"
Cohesion: 0.40
Nodes (9): faceMatrix(), finiteMatrix(), finiteVector(), mat4, Mesh, transformMatrix(), triangles(), validColor() (+1 more)

### Community 44 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+2 more)

### Community 45 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, shared_ptr, Material, color, repeatTexture (+2 more)

### Community 46 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): uint8_t, TextureImage, height, rgba, width, shared_ptr, string, TextureResult (+2 more)

### Community 47 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): OutlineMode, vec3, LoadOptions, allowObjFallback, customPivot, flipTextureRows, mergeCoplanarFaces, outline (+2 more)

### Community 48 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): CharacterBody, direction, feet, grounded, halfWidth, height, speed, verticalSpeed (+2 more)

### Community 49 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): mat4, vec3, RoomBody, halfExtent, localPosition, localVelocity, released, worldPosition (+1 more)

### Community 50 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): PathMotion, durationSeconds, interpolation, phase, playback, rate, relativeToStart, space (+1 more)

### Community 51 - "framework 코드 구조"
Cohesion: 0.22
Nodes (8): Renderer::initShader, Renderer::destroy, Renderer::render, Renderer::initialize, Renderer::upload, Renderer::renderViews, Renderer::renderPass, Renderer::draw

### Community 52 - "framework 코드 구조"
Cohesion: 0.25
Nodes (8): DiagramSystem, faceModelMatrix, makeCone, makePolyhedron, makeSphere, modelMatrix, validPolyhedron, validTransform

### Community 53 - " 코드 구조"
Cohesion: 0.48
Nodes (4): array, optional, string, vector

### Community 54 - "framework 코드 구조"
Cohesion: 0.29
Nodes (7): Motion::fixed(), RotationMotion, axis, degreesPerSecond, initialDegrees, space, TransformSpace

### Community 55 - "framework 코드 구조"
Cohesion: 0.33
Nodes (4): CharacterStep, blocked, landed, turned

### Community 56 - "framework 코드 구조"
Cohesion: 0.29
Nodes (6): DistancePath, distances_, points_, sample, setPoints, vector

### Community 57 - "framework 코드 구조"
Cohesion: 0.29
Nodes (7): TransformSystem, reflectLine, reflectPlane, reflectPoint, rotation, scaling, translation

### Community 58 - "framework 코드 구조"
Cohesion: 0.33
Nodes (6): mat3, Transform3D, basis, localBasis, rotation, scale

### Community 59 - "framework 코드 구조"
Cohesion: 0.33
Nodes (6): MotionState, enabled, motion, playing, sampled, timeSeconds

### Community 60 - "framework 코드 구조"
Cohesion: 0.33
Nodes (6): EntityId, JointRig, head, hips, root, shoulders

### Community 61 - "framework 코드 구조"
Cohesion: 0.40
Nodes (4): AnimationClock, advance, playing, seconds

### Community 62 - "framework 코드 구조"
Cohesion: 0.40
Nodes (5): CollisionSystem, jump, randomFreeBox, stepCharacter, stepRoomBody

### Community 63 - " 코드 구조"
Cohesion: 0.50
Nodes (4): Component_Poly, DiagramSystem::faceModelMatrix(), Scene::getComponent(), PlaneFigure

### Community 64 - "framework 코드 구조"
Cohesion: 0.50
Nodes (3): DWORD, executableDirectory(), frameworkDirectory()

### Community 65 - "framework 코드 구조"
Cohesion: 0.50
Nodes (4): wglGetProc, T, resolve(), symbol()

### Community 66 - "framework 코드 구조"
Cohesion: 0.50
Nodes (4): DistancePath::sample(), PatternSystem::conveyorCornerPose(), PathPose, Playback

### Community 67 - "framework 코드 구조"
Cohesion: 0.50
Nodes (4): PathPose, position, tangent, yawDegrees

### Community 68 - "framework 코드 구조"
Cohesion: 0.67
Nodes (3): checkGlAt(), GetError, PipelineStage

### Community 69 - "framework 코드 구조"
Cohesion: 0.67
Nodes (3): DiagnosticOptions, errors, trace

## Knowledge Gaps
- **712 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `SetErrorCallback` (+707 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 846 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **6 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Application` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, ` 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.004) - this node is a cross-community bridge._
- **Why does `Scene` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, ` 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.002) - this node is a cross-community bridge._
- **Why does `MotionSystem` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.001) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _712 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `framework 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.024096385542168676 - nodes in this community are weakly interconnected._
- **Should `framework 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.03389830508474576 - nodes in this community are weakly interconnected._
- **Should `framework 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.034482758620689655 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.
