# Graph Report - framework  (2026-10-17)

## Corpus Check
- Corpus is ~5,853 words - fits in a single context window. You may not need a graph.

## Summary
- 753 nodes · 1097 edges · 48 communities (34 shown, 6 thin omitted)
- Extraction: 98% EXTRACTED · 2% INFERRED · 0% AMBIGUOUS · INFERRED: 18 edges (avg confidence: 0.81)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `ac6110db`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- detail 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
-  코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- include 코드 구조
- include 코드 구조
- include 코드 구조
-  코드 구조
- include 코드 구조
- include 코드 구조
- detail 코드 구조
- include 코드 구조
- detail 코드 구조
- detail 코드 구조
- examples 코드 구조
- detail 코드 구조
- examples 코드 구조

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 83 edges
2. `Scene` - 60 edges
3. `Application` - 47 edges
4. `Renderer` - 37 edges
5. `PlaneFigure` - 32 edges
6. `GlfwApi` - 28 edges
7. `Component_Poly` - 18 edges
8. `AssetLoader` - 17 edges
9. `PickResult` - 15 edges
10. `Camera` - 13 edges

## Surprising Connections (you probably didn't know these)
- `main()` --calls--> `pickAtCursor`  [INFERRED]
  examples/assets.cpp → include/Application.h
- `main()` --calls--> `pickAt`  [INFERRED]
  examples/basic.cpp → include/Application.h
- `main()` --references--> `Application`  [INFERRED]
  examples/assets.cpp → include/Application.h
- `main()` --calls--> `requestClose`  [INFERRED]
  examples/assets.cpp → include/Application.h
- `main()` --calls--> `run`  [INFERRED]
  examples/assets.cpp → include/Application.h

## Import Cycles
- None detected.

## Communities (48 total, 6 thin omitted)

### Community 0 - "detail 코드 구조"
Cohesion: 0.02
Nodes (80): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+72 more)

### Community 1 - "detail 코드 구조"
Cohesion: 0.07
Nodes (74): Component_Poly, appendFace(), DiagramSystem::faceModelMatrix(), DiagramSystem::makeCone(), DiagramSystem::makePolyhedron(), DiagramSystem::makeSphere(), DiagramSystem::modelMatrix(), DiagramSystem::validPolyhedron() (+66 more)

### Community 2 - "detail 코드 구조"
Cohesion: 0.04
Nodes (53): CameraSystem::projectionMatrix, Scene::setLocalBasis, Scene::setFaceVertexColors, Scene::addSphere, Scene::addModel, Scene::addBasis, Scene::clear, Scene::setVertexNormals (+45 more)

### Community 3 - "include 코드 구조"
Cohesion: 0.05
Nodes (47): main(), main(), exception_ptr, Handler, Application, assets_, callbackError_, camera_ (+39 more)

### Community 4 - "include 코드 구조"
Cohesion: 0.04
Nodes (50): Scene, addBasis, addCone, addCuboid, addGroup, addLine, addModel, addPyramid (+42 more)

### Community 5 - "detail 코드 구조"
Cohesion: 0.07
Nodes (40): aiMatrix4x4, Camera, AssetLoader::loadModel(), AssimpApi, getColor, getError, getTexture, importFile (+32 more)

### Community 6 - "include 코드 구조"
Cohesion: 0.05
Nodes (42): GLint, GLsizei, DrawRange, count, entityId, faceIndex, firstIndex, lineId (+34 more)

### Community 7 - "include 코드 구조"
Cohesion: 0.06
Nodes (40): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+32 more)

### Community 8 - "detail 코드 구조"
Cohesion: 0.08
Nodes (26): GlfwApi, createWindow, DestroyWindow, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize, GetWindowUserPointer (+18 more)

### Community 9 - "include 코드 구조"
Cohesion: 0.16
Nodes (21): appendMesh(), generateTexcoords(), Mesh, Model, uint32_t, vec2, vec3, mergeCoplanar() (+13 more)

### Community 10 - "include 코드 구조"
Cohesion: 0.10
Nodes (21): OutlineMode, uint32_t, vector, PlaneFigure, color, creaseAngleDegrees, independence, localAnimationMatrix (+13 more)

### Community 11 - "detail 코드 구조"
Cohesion: 0.15
Nodes (15): ApplicationConfig, Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler() (+7 more)

### Community 12 - "detail 코드 구조"
Cohesion: 0.10
Nodes (19): Application::setMouseHandler, Application::shutdown, detail::glfw, Application::getAssets, Application::setModifiedKeyHandler, Application::setInitHandler, Application::setRenderHandler, Application::pickAtCursor (+11 more)

### Community 13 - "include 코드 구조"
Cohesion: 0.19
Nodes (11): HRESULT, checkCom(), ComApartment, result, ComOwner, value, decodeImage(), path (+3 more)

### Community 14 - "include 코드 구조"
Cohesion: 0.15
Nodes (13): ApplicationConfig, features, height, projectRoot, runtimeDirectory, shaderDirectory, title, verticalSync (+5 more)

### Community 15 - "include 코드 구조"
Cohesion: 0.22
Nodes (9): AssetLoader, features_, imageCache_, loadModel, loadPrimitive, projectRoot_, runtimeRoot_, path (+1 more)

### Community 16 - "include 코드 구조"
Cohesion: 0.17
Nodes (11): FaceId, entityId, faceIndex, EntityId, optional, size_t, Line3D, color (+3 more)

### Community 17 - "include 코드 구조"
Cohesion: 0.22
Nodes (10): finishLoad(), Model, optional, PivotMode, vec3, ModelResult, error, localBasis (+2 more)

### Community 18 - " 코드 구조"
Cohesion: 0.27
Nodes (8): array, wglGetProc, T, resolve(), symbol(), optional, string, vector

### Community 19 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): RenderOptions, background, depthTest, lightColor, lightDirection, lighting, outlineColor, textures (+2 more)

### Community 20 - "include 코드 구조"
Cohesion: 0.40
Nodes (9): faceMatrix(), finiteMatrix(), finiteVector(), Mesh, transformMatrix(), triangles(), validColor(), validMesh() (+1 more)

### Community 21 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+2 more)

### Community 22 - "include 코드 구조"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, shared_ptr, Material, color, repeatTexture (+2 more)

### Community 23 - "include 코드 구조"
Cohesion: 0.25
Nodes (8): DiagramSystem, faceModelMatrix, makeCone, makePolyhedron, makeSphere, modelMatrix, validPolyhedron, validTransform

### Community 24 - "include 코드 구조"
Cohesion: 0.25
Nodes (8): OutlineMode, LoadOptions, customPivot, flipTextureRows, mergeCoplanarFaces, outline, pivot, texturePath

### Community 25 - "detail 코드 구조"
Cohesion: 0.29
Nodes (6): Renderer::destroy, Renderer::upload, Renderer::draw, Renderer::initShader, Renderer::initialize, Renderer::render

### Community 26 - "detail 코드 구조"
Cohesion: 0.40
Nodes (4): executableDirectory(), frameworkDirectory(), path, relativePathInside()

### Community 27 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): GLShape, string, Polyhedron, faces, name, shape

### Community 28 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): vec2, Vertex, color, normal, position, uv

### Community 29 - "include 코드 구조"
Cohesion: 0.33
Nodes (6): Transform3D, basis, localBasis, rotation, scale, mat3

### Community 30 - " 코드 구조"
Cohesion: 0.60
Nodes (5): bottomCenter(), bounds(), Model, vec3, pair

### Community 31 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): uint8_t, TextureImage, height, rgba, width

### Community 32 - "include 코드 구조"
Cohesion: 0.40
Nodes (5): shared_ptr, string, TextureResult, error, image

### Community 33 - "detail 코드 구조"
Cohesion: 0.67
Nodes (3): Module, handle_, HMODULE

## Knowledge Gaps
- **404 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `Terminate` (+399 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 492 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **6 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Application` connect `include 코드 구조` to `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `detail 코드 구조`, `include 코드 구조`, ` 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.003) - this node is a cross-community bridge._
- **Why does `Scene` connect `include 코드 구조` to `include 코드 구조`, `include 코드 구조`, `include 코드 구조`, `include 코드 구조`?**
  _High betweenness centrality (0.002) - this node is a cross-community bridge._
- **Why does `GlApi` connect `detail 코드 구조` to `detail 코드 구조`, ` 코드 구조`?**
  _High betweenness centrality (0.001) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _404 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.024691358024691357 - nodes in this community are weakly interconnected._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.07333333333333333 - nodes in this community are weakly interconnected._
- **Should `detail 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.037037037037037035 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.
