# Graph Report - framework  (2026-10-22)

## Corpus Check
- Corpus is ~18,069 words - fits in a single context window. You may not need a graph.

## Summary
- 1010 nodes · 1524 edges · 66 communities (48 shown, 9 thin omitted)
- Extraction: 98% EXTRACTED · 2% INFERRED · 0% AMBIGUOUS · INFERRED: 23 edges (avg confidence: 0.81)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `8933b861`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
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
- include 코드 구조
- include 코드 구조
- examples 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
-  코드 구조
-  코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- examples 코드 구조
- examples 코드 구조
- examples 코드 구조

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 83 edges
2. `Scene` - 70 edges
3. `Application` - 48 edges
4. `Renderer` - 37 edges
5. `MotionSystem` - 35 edges
6. `PlaneFigure` - 32 edges
7. `GlfwApi` - 28 edges
8. `Component_Poly` - 18 edges
9. `main()` - 17 edges
10. `AssetLoader` - 17 edges

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

## Communities (66 total, 9 thin omitted)

### Community 0 - "detail 코드 구조"
Cohesion: 0.06
Nodes (97): AxisLine, Camera, Component_Poly, Motion::erase(), appendFace(), CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix() (+89 more)

### Community 1 - "detail 코드 구조"
Cohesion: 0.02
Nodes (80): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+72 more)

### Community 2 - "include 코드 구조"
Cohesion: 0.03
Nodes (57): LayerId, Scene, addBasis, addCone, addCuboid, addGroup, addLine, addModel (+49 more)

### Community 3 - "detail 코드 구조"
Cohesion: 0.03
Nodes (57): Scene::transformTexcoords, Scene::unbindEntity, DiagramSystem::makePolyhedron, Scene::setFaceVertexColors, Scene::addSphere, Scene::getMesh, CameraSystem::projectionMatrix, Scene::setFaceLocalMatrix (+49 more)

### Community 4 - "detail 코드 구조"
Cohesion: 0.07
Nodes (46): AssetLoader::loadObj(), ObjCorner, normal, position, uv, ObjFace, batch, corners (+38 more)

### Community 5 - "detail 코드 구조"
Cohesion: 0.08
Nodes (42): Motion::append(), Motion::fixed(), Motion::followPath(), Motion::insert(), Motion::replace(), Motion::rotation(), Motion::sample(), Motion::then() (+34 more)

### Community 6 - "include 코드 구조"
Cohesion: 0.05
Nodes (42): GLint, GLsizei, DrawRange, count, entityId, faceIndex, firstIndex, lineId (+34 more)

### Community 7 - "detail 코드 구조"
Cohesion: 0.06
Nodes (33): MotionSystem::setMotionPlaying, Motion::followPath, MotionSystem::createPathAndSpin, MotionSystem::setMotion, MotionSystem::removeMotion, MotionSystem::bindMotion, MotionSystem::createOrbitAndSpin, Motion::append (+25 more)

### Community 8 - "include 코드 구조"
Cohesion: 0.07
Nodes (30): LayerId, map, MotionSystem, bindMotion, clear, createMotion, createOrbit, createOrbitAndSpin (+22 more)

### Community 9 - "include 코드 구조"
Cohesion: 0.07
Nodes (28): exception_ptr, Application, assets_, callbackError_, camera_, cursorHandler_, focusHandler_, glfwInitialized_ (+20 more)

### Community 10 - "detail 코드 구조"
Cohesion: 0.08
Nodes (26): GlfwApi, createWindow, DestroyWindow, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize, GetWindowUserPointer (+18 more)

### Community 11 - "include 코드 구조"
Cohesion: 0.13
Nodes (21): main(), AxisLine, direction, point, vec3, PathSystem, circle, ellipse (+13 more)

### Community 12 - "include 코드 구조"
Cohesion: 0.16
Nodes (21): appendMesh(), generateTexcoords(), Mesh, Model, uint32_t, vec2, vec3, vector (+13 more)

### Community 13 - "include 코드 구조"
Cohesion: 0.10
Nodes (22): finishLoad(), Model, optional, OutlineMode, PivotMode, vec3, vector, LoadOptions (+14 more)

### Community 14 - "detail 코드 구조"
Cohesion: 0.15
Nodes (15): ApplicationConfig, Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler() (+7 more)

### Community 15 - "include 코드 구조"
Cohesion: 0.10
Nodes (20): GLFace, OutlineMode, PlaneFigure, color, creaseAngleDegrees, independence, localAnimationMatrix, localVertices (+12 more)

### Community 16 - "detail 코드 구조"
Cohesion: 0.10
Nodes (19): Application::shutdown, detail::glfw, Application::setModifiedKeyHandler, Application::getAssets, Application::setInitHandler, Application::setRenderHandler, Application::setMouseHandler, Application::setFocusHandler (+11 more)

### Community 17 - "detail 코드 구조"
Cohesion: 0.17
Nodes (12): executableDirectory(), frameworkDirectory(), wglGetProc, T, Module, handle_, relativePathInside(), resolve() (+4 more)

### Community 18 - "detail 코드 구조"
Cohesion: 0.13
Nodes (14): TransformSystem::validAffine, PathSystem::polyline, TransformSystem::reflectLine, TransformSystem::rotation, TransformSystem::translation, PathSystem::smoothStep, TransformSystem::reflectPoint, PathSystem::ellipse (+6 more)

### Community 19 - "include 코드 구조"
Cohesion: 0.21
Nodes (10): HRESULT, checkCom(), ComApartment, result, ComOwner, value, decodeImage(), T (+2 more)

### Community 20 - "include 코드 구조"
Cohesion: 0.26
Nodes (12): CameraSystem, projectionMatrix, validCamera, viewMatrix, optional, vec3, pick(), Ray (+4 more)

### Community 21 - "include 코드 구조"
Cohesion: 0.19
Nodes (11): AssetLoader, features_, imageCache_, loadModel, loadObj, loadPrimitive, projectRoot_, FeatureOptions (+3 more)

### Community 22 - "include 코드 구조"
Cohesion: 0.15
Nodes (13): EntityId, size_t, vec2, PickResult, barycentric, distance, entityId, faceIndex (+5 more)

### Community 23 - "include 코드 구조"
Cohesion: 0.17
Nodes (11): FaceId, entityId, faceIndex, EntityId, optional, size_t, Line3D, color (+3 more)

### Community 24 - "include 코드 구조"
Cohesion: 0.17
Nodes (12): Motion, append, erase, fixed, followPath, insert, replace, rotation (+4 more)

### Community 25 - "include 코드 구조"
Cohesion: 0.25
Nodes (11): main(), main(), Handler, pickAt, pickAtCursor, requestClose, run, setInitHandler (+3 more)

### Community 26 - "include 코드 구조"
Cohesion: 0.20
Nodes (11): optional, vector, LayerConfig, enabled, motion, space, transform, LayerState (+3 more)

### Community 27 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+2 more)

### Community 28 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): RenderOptions, background, depthTest, lightColor, lightDirection, lighting, outlineColor, textures (+2 more)

### Community 29 - "include 코드 구조"
Cohesion: 0.40
Nodes (9): faceMatrix(), finiteMatrix(), finiteVector(), mat4, Mesh, transformMatrix(), triangles(), validColor() (+1 more)

### Community 30 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+2 more)

### Community 31 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, shared_ptr, Material, color, repeatTexture (+2 more)

### Community 32 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): mat4, MotionSample, local, parent, MotionState, enabled, motion, playing (+2 more)

### Community 33 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): PathMotion, durationSeconds, interpolation, phase, playback, rate, relativeToStart, space (+2 more)

### Community 34 - "include 코드 구조"
Cohesion: 0.22
Nodes (9): ApplicationConfig, features, height, projectRoot, runtimeDirectory, shaderDirectory, title, verticalSync (+1 more)

### Community 35 - "include 코드 구조"
Cohesion: 0.25
Nodes (8): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY

### Community 36 - "include 코드 구조"
Cohesion: 0.25
Nodes (8): DiagramSystem, faceModelMatrix, makeCone, makePolyhedron, makeSphere, modelMatrix, validPolyhedron, validTransform

### Community 37 - "include 코드 구조"
Cohesion: 0.29
Nodes (7): uint32_t, uint8_t, vector, TextureImage, height, rgba, width

### Community 38 - "include 코드 구조"
Cohesion: 0.29
Nodes (5): shared_ptr, string, TextureResult, error, image

### Community 40 - "include 코드 구조"
Cohesion: 0.29
Nodes (7): function, MotionStep, durationSeconds, enabled, space, startSeconds, transform

### Community 41 - "detail 코드 구조"
Cohesion: 0.29
Nodes (6): Renderer::destroy, Renderer::upload, Renderer::draw, Renderer::initShader, Renderer::initialize, Renderer::render

### Community 42 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): GLShape, string, Polyhedron, faces, name, shape

### Community 43 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): vec2, Vertex, color, normal, position, uv

### Community 44 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): Transform3D, basis, localBasis, rotation, scale, mat3

### Community 45 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): RotationMotion, axis, degreesPerSecond, initialDegrees, space, TransformSpace

### Community 46 - " 코드 구조"
Cohesion: 0.50
Nodes (3): array, optional, map

### Community 47 - " 코드 구조"
Cohesion: 0.60
Nodes (5): bottomCenter(), bounds(), Model, vec3, pair

### Community 48 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): PickViewport, height, width, x, y

## Knowledge Gaps
- **544 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `Terminate` (+539 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 648 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **9 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Scene` connect `include 코드 구조` to `include 코드 구조`, `include 코드 구조`, ` 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.004) - this node is a cross-community bridge._
- **Why does `Application` connect `include 코드 구조` to `include 코드 구조`, `include 코드 구조`, `examples 코드 구조`, ` 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.003) - this node is a cross-community bridge._
- **Why does `MotionSystem` connect `include 코드 구조` to `include 코드 구조`, `include 코드 구조`, `detail 코드 구조`?**
  _High betweenness centrality (0.001) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _544 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.055523199378761406 - nodes in this community are weakly interconnected._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.024691358024691357 - nodes in this community are weakly interconnected._
- **Should `include 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.034482758620689655 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.
