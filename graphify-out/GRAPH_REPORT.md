# Graph Report - OpenGL-26-2  (2026-10-22)

## Corpus Check
- Large corpus: 420 files · ~2,408,479 words. Semantic extraction will be expensive (many Claude tokens). Consider running on a subfolder.

## Summary
- 1510 nodes · 2341 edges · 98 communities (65 shown, 11 thin omitted)
- Extraction: 95% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 116 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `8933b861`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_16 코드 구조
- openGL_3dAssign_19 코드 구조
- framework 코드 구조
- openGL_3dAssign_17 코드 구조
- openGL_3dAssign_20 코드 구조
- openGL_3dAssign_15 코드 구조
- openGL_3dAssign_18 코드 구조
- framework 코드 구조
- standard 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
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
- openGL_sample0 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- openGL_sample0 코드 구조
- openGL_3dAssign_16 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- openGL_sample0 코드 구조
- openGL_3dAssign_17 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_15 코드 구조
- openGL_3dAssign_19 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_20 코드 구조
- standard 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_18 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 83 edges
2. `Scene` - 71 edges
3. `Application` - 62 edges
4. `Console` - 41 edges
5. `Renderer` - 37 edges
6. `Console` - 37 edges
7. `MotionSystem` - 35 edges
8. `Console` - 33 edges
9. `PlaneFigure` - 32 edges
10. `Console` - 29 edges

## Surprising Connections (you probably didn't know these)
- `main()` --calls--> `choosePath`  [AMBIGUOUS]
  framework/examples/motion.cpp → standard/Console.h
- `triangulate()` --calls--> `cross()`  [AMBIGUOUS]
  framework/include/Geometry.h → openGL_sample0/MeshValidator.cpp
- `choosePath` --calls--> `append`  [INFERRED]
  standard/Console.h → framework/include/Motion.h
- `circle()` --calls--> `point`  [INFERRED]
  openGL_3dAssign_18/Console.cpp → framework/include/Transform.h
- `main()` --calls--> `setMouseHandler`  [INFERRED]
  standard/main.cpp → framework/include/Application.h

## Import Cycles
- None detected.

## Communities (98 total, 11 thin omitted)

### Community 0 - "framework 코드 구조"
Cohesion: 0.05
Nodes (101): array, AxisLine, Camera, Component_Poly, EntityId, appendFace(), CameraSystem::projectionMatrix(), CameraSystem::validCamera() (+93 more)

### Community 1 - "framework 코드 구조"
Cohesion: 0.02
Nodes (80): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+72 more)

### Community 2 - "framework 코드 구조"
Cohesion: 0.03
Nodes (57): LayerId, Scene, addBasis, addCone, addCuboid, addGroup, addLine, addModel (+49 more)

### Community 3 - "framework 코드 구조"
Cohesion: 0.03
Nodes (57): Scene::getEntityLayer, Scene::faceWorldMatrix, Scene::setMeshVertices, Scene::setSideLengths, Scene::Scene, Scene::transformTexcoords, DiagramSystem::modelMatrix, Scene::setFaceTransform (+49 more)

### Community 4 - "framework 코드 구조"
Cohesion: 0.07
Nodes (44): AssetLoader::loadObj(), ObjCorner, normal, position, uv, ObjFace, batch, corners (+36 more)

### Community 5 - "openGL_3dAssign_16 코드 구조"
Cohesion: 0.08
Nodes (43): Console, applyFaceMotion, applyObjectTransform, applyPyramid, beginMotion, beginPyramid, cuboidId_, initScene (+35 more)

### Community 6 - "openGL_3dAssign_19 코드 구조"
Cohesion: 0.06
Nodes (43): Console, apply, box, cameraOrbit_, cameraX_, cameraYaw_, cameraZ_, central_ (+35 more)

### Community 7 - "framework 코드 구조"
Cohesion: 0.08
Nodes (41): Motion::append(), Motion::erase(), Motion::fixed(), Motion::followPath(), Motion::insert(), Motion::replace(), Motion::rotation(), Motion::sample() (+33 more)

### Community 8 - "openGL_3dAssign_17 코드 구조"
Cohesion: 0.09
Nodes (39): Console, apply, begin, elapsed_, initScene, model, motion_, movement_ (+31 more)

### Community 9 - "openGL_3dAssign_20 코드 구조"
Cohesion: 0.06
Nodes (39): Console, apply, box, boxs, cameraOrbit_, cameraX_, cameraYaw_, cameraZ_ (+31 more)

### Community 10 - "openGL_3dAssign_15 코드 구조"
Cohesion: 0.10
Nodes (35): assignVertexColors(), Console, applyFaceMotion, applyObjectTransform, beginMotion, cuboidId_, initScene, motions_ (+27 more)

### Community 11 - "openGL_3dAssign_18 코드 구조"
Cohesion: 0.07
Nodes (33): point, circle(), Console, apply, initScene, moonAngle_, moonFrames_, moons_ (+25 more)

### Community 12 - "framework 코드 구조"
Cohesion: 0.06
Nodes (33): MotionSystem::createSpin, MotionSystem::setLayer, Motion::rotation, MotionSystem::bindMotion, MotionSystem::update, MotionSystem::createPathAndSpin, MotionSystem::removeLayer, MotionSystem::reparentLayer (+25 more)

### Community 13 - "standard 코드 구조"
Cohesion: 0.09
Nodes (29): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY (+21 more)

### Community 14 - "framework 코드 구조"
Cohesion: 0.09
Nodes (31): CameraSystem, projectionMatrix, validCamera, viewMatrix, Camera, EntityId, optional, size_t (+23 more)

### Community 15 - "framework 코드 구조"
Cohesion: 0.06
Nodes (31): GLuint, vector, Renderer, destroy, draw, drawRanges_, ebo_, hasTextureLocation_ (+23 more)

### Community 16 - "framework 코드 구조"
Cohesion: 0.07
Nodes (30): LayerId, map, MotionSystem, bindMotion, clear, createMotion, createOrbit, createOrbitAndSpin (+22 more)

### Community 17 - "openGL_sample0 코드 구조"
Cohesion: 0.13
Nodes (26): GLenum, Bounds, maximum, minimum, Camera, forward, position, radius (+18 more)

### Community 18 - "framework 코드 구조"
Cohesion: 0.08
Nodes (26): exception_ptr, Application, assets_, callbackError_, camera_, cursorHandler_, focusHandler_, glfwInitialized_ (+18 more)

### Community 19 - "framework 코드 구조"
Cohesion: 0.08
Nodes (26): GlfwApi, createWindow, DestroyWindow, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize, GetWindowUserPointer (+18 more)

### Community 20 - "framework 코드 구조"
Cohesion: 0.14
Nodes (17): AssetLoader, features_, imageCache_, loadModel, loadObj, loadPrimitive, projectRoot_, FeatureOptions (+9 more)

### Community 21 - "framework 코드 구조"
Cohesion: 0.10
Nodes (22): finishLoad(), Model, optional, OutlineMode, PivotMode, vec3, vector, LoadOptions (+14 more)

### Community 22 - "framework 코드 구조"
Cohesion: 0.13
Nodes (20): main(), AxisLine, direction, vec3, PathSystem, circle, ellipse, linear (+12 more)

### Community 23 - "framework 코드 구조"
Cohesion: 0.15
Nodes (15): ApplicationConfig, Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler() (+7 more)

### Community 24 - "framework 코드 구조"
Cohesion: 0.10
Nodes (20): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+12 more)

### Community 25 - "framework 코드 구조"
Cohesion: 0.10
Nodes (20): OutlineMode, PlaneFigure, color, creaseAngleDegrees, independence, localAnimationMatrix, localVertices, material (+12 more)

### Community 26 - "framework 코드 구조"
Cohesion: 0.10
Nodes (19): Application::run, Application::requestClose, Application::setKeyHandler, Application::setFocusHandler, Application::shutdown, Application::pickAt, detail::glfw, Application::setUpdateHandler (+11 more)

### Community 27 - "framework 코드 구조"
Cohesion: 0.22
Nodes (17): appendMesh(), generateTexcoords(), Model, uint32_t, vec2, vec3, vector, mergeCoplanar() (+9 more)

### Community 28 - "openGL_sample0 코드 구조"
Cohesion: 0.13
Nodes (16): CornerIndex, hasTexture, texture, vertex, FaceRecord, line, valid, value (+8 more)

### Community 29 - "framework 코드 구조"
Cohesion: 0.15
Nodes (10): main(), main(), pickAt, pickAtCursor, requestClose, setMouseHandler, dispatch(), GLFWwindow (+2 more)

### Community 30 - "framework 코드 구조"
Cohesion: 0.16
Nodes (11): executableDirectory(), frameworkDirectory(), wglGetProc, T, Module, handle_, relativePathInside(), resolve() (+3 more)

### Community 31 - "openGL_sample0 코드 구조"
Cohesion: 0.14
Nodes (15): size_t, ResolvedFace, hasTextures, number, textures, vertexIndices, vertices, Vertex (+7 more)

### Community 32 - "openGL_sample0 코드 구조"
Cohesion: 0.32
Nodes (14): string, Vertex, cross(), inRange(), isCollinear(), MeshValidator::validate(), normalizeScale(), resolveFace() (+6 more)

### Community 33 - "openGL_3dAssign_16 코드 구조"
Cohesion: 0.15
Nodes (13): Console::initScene, Console::onUpdate, Console::updatePyramid, Console::selectShape, main, Console::onKey, Console::beginMotion, Console::resetFaces (+5 more)

### Community 34 - "framework 코드 구조"
Cohesion: 0.13
Nodes (14): PathSystem::circle, TransformSystem::reflectLine, PathSystem::rectangle, TransformSystem::reflectPlane, PathSystem::polygon, TransformSystem::reflectPoint, TransformSystem::scaling, PathSystem::linear (+6 more)

### Community 35 - "framework 코드 구조"
Cohesion: 0.16
Nodes (12): optional, map, mat4, MotionSample, local, parent, MotionState, enabled (+4 more)

### Community 36 - "framework 코드 구조"
Cohesion: 0.40
Nodes (13): run, setFocusHandler, setInitHandler, setKeyHandler, setModifiedKeyHandler, setUpdateHandler, main(), main() (+5 more)

### Community 37 - "framework 코드 구조"
Cohesion: 0.21
Nodes (10): checkCom(), ComApartment, result, ComOwner, value, decodeImage(), T, uint8_t (+2 more)

### Community 38 - "openGL_sample0 코드 구조"
Cohesion: 0.35
Nodes (12): istream, size_t, string, vector, Vertex, MeshParser::parse(), readCorner(), readFace() (+4 more)

### Community 39 - "openGL_sample0 코드 구조"
Cohesion: 0.18
Nodes (13): Diagnostic, face, line, message, string, vector, ParseResult, diagnostics (+5 more)

### Community 40 - "openGL_3dAssign_17 코드 구조"
Cohesion: 0.19
Nodes (11): Console::apply, Console::model, Console::initScene, main, Console::worldPosition, Console::onFocus, Console::reset, Console::begin (+3 more)

### Community 41 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount, outlineFirstIndex (+4 more)

### Community 42 - "framework 코드 구조"
Cohesion: 0.17
Nodes (11): FaceId, entityId, faceIndex, EntityId, optional, size_t, Line3D, color (+3 more)

### Community 43 - "framework 코드 구조"
Cohesion: 0.18
Nodes (12): MotionId, optional, vector, LayerConfig, enabled, motion, space, transform (+4 more)

### Community 44 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): Motion, append, erase, fixed, followPath, insert, replace, rotation (+4 more)

### Community 45 - "openGL_3dAssign_15 코드 구조"
Cohesion: 0.20
Nodes (10): Console::resetAll, main, Console::selectShape, Console::resetFaces, Console::onUpdate, Console::applyFaceMotion, Console::beginMotion, Console::onKey (+2 more)

### Community 46 - "openGL_3dAssign_19 코드 구조"
Cohesion: 0.24
Nodes (9): Console::recolor, Console::onKey, Console::onFocus, Console::box, Console::apply, Console::initScene, Console::reset, main (+1 more)

### Community 47 - "framework 코드 구조"
Cohesion: 0.36
Nodes (9): bottomCenter(), bounds(), finiteMatrix(), finiteVector(), Model, vec3, validColor(), validMesh() (+1 more)

### Community 48 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+2 more)

### Community 49 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, shared_ptr, Material, color, repeatTexture (+2 more)

### Community 50 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): PathMotion, durationSeconds, interpolation, phase, playback, rate, relativeToStart, space (+2 more)

### Community 51 - "openGL_3dAssign_20 코드 구조"
Cohesion: 0.27
Nodes (8): Console::onUpdate, Console::onFocus, Console::initScene, Console::apply, Console::box, Console::reset, Console::onKey, main

### Community 52 - "standard 코드 구조"
Cohesion: 0.24
Nodes (8): Console::changeReflection, Console::onKey, main, Console::printHelp, Console::initScene, Console::toggleInsertedLayer, Console::choosePath, Console::onMouse

### Community 53 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): ApplicationConfig, features, height, projectRoot, runtimeDirectory, shaderDirectory, title, verticalSync (+1 more)

### Community 54 - "framework 코드 구조"
Cohesion: 0.28
Nodes (9): faceMatrix(), mat3, mat4, Transform3D, basis, localBasis, rotation, scale (+1 more)

### Community 55 - "framework 코드 구조"
Cohesion: 0.28
Nodes (8): uint32_t, uint8_t, vector, TextureImage, height, rgba, width, triangles()

### Community 56 - "framework 코드 구조"
Cohesion: 0.25
Nodes (8): DiagramSystem, faceModelMatrix, makeCone, makePolyhedron, makeSphere, modelMatrix, validPolyhedron, validTransform

### Community 57 - "openGL_3dAssign_18 코드 구조"
Cohesion: 0.36
Nodes (6): Console::onFocus, main, Console::apply, Console::initScene, Console::onUpdate, Console::onKey

### Community 58 - "framework 코드 구조"
Cohesion: 0.29
Nodes (7): UVOptions, axis, mapping, offset, rotationDegrees, scale, UVMapping

### Community 59 - "framework 코드 구조"
Cohesion: 0.29
Nodes (7): function, MotionStep, durationSeconds, enabled, space, startSeconds, transform

### Community 60 - "openGL_sample0 코드 구조"
Cohesion: 0.29
Nodes (7): Texture, s, t, TextureRecord, line, valid, value

### Community 61 - "framework 코드 구조"
Cohesion: 0.29
Nodes (6): Renderer::draw, Renderer::upload, Renderer::render, Renderer::initialize, Renderer::initShader, Renderer::destroy

### Community 62 - "framework 코드 구조"
Cohesion: 0.33
Nodes (6): string, Polyhedron, faces, name, shape, GLShape

### Community 63 - "framework 코드 구조"
Cohesion: 0.33
Nodes (6): vec2, Vertex, color, normal, position, uv

### Community 64 - "framework 코드 구조"
Cohesion: 0.33
Nodes (6): RotationMotion, axis, degreesPerSecond, initialDegrees, space, TransformSpace

## Ambiguous Edges - Review These
- `main()` → `choosePath`  [AMBIGUOUS]
  framework/examples/motion.cpp · relation: calls
- `triangulate()` → `cross()`  [AMBIGUOUS]
  framework/include/Geometry.h · relation: calls

## Knowledge Gaps
- **744 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `Terminate` (+739 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 915 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **11 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `main()` and `choosePath`?**
  _Edge tagged AMBIGUOUS (relation: calls) - confidence is low._
- **What is the exact relationship between `triangulate()` and `cross()`?**
  _Edge tagged AMBIGUOUS (relation: calls) - confidence is low._
- **Why does `Application` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.007) - this node is a cross-community bridge._
- **Why does `Scene` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.004) - this node is a cross-community bridge._
- **Why does `Console` connect `openGL_3dAssign_18 코드 구조` to `framework 코드 구조`?**
  _High betweenness centrality (0.003) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _744 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `framework 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.05390835579514825 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.
