# Graph Report - framework  (2026-10-12)

## Corpus Check
- Corpus is ~5,091 words - fits in a single context window. You may not need a graph.

## Summary
- 643 nodes · 1013 edges · 26 communities
- Extraction: 97% EXTRACTED · 3% INFERRED · 0% AMBIGUOUS · INFERRED: 31 edges (avg confidence: 0.81)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- 네이티브 함수 연결
- 장면과 계층 변환
- Application.h 구조
- ModelLoader.h 구조
- 장면 공개 API
- 장면 공개 API
- 네이티브 함수 연결
- 앱 실행과 입력
- UV와 기하 계산
- 모델과 재질 데이터
- 모델과 재질 데이터
- GPU 렌더링
- 일반 모델 로딩
- 이미지 디코딩
- 교점과 UV 피킹
- 장면 공개 API
- 교점과 UV 피킹
- 모델과 재질 데이터
- 장면 공개 API
- 모델과 재질 데이터
- Application.h 구조
- 모델과 재질 데이터
- 모델과 재질 데이터
- 모델과 재질 데이터
- 장면 공개 API
-  구조

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 82 edges
2. `Scene` - 59 edges
3. `Application` - 49 edges
4. `Renderer` - 36 edges
5. `PlaneFigure` - 33 edges
6. `GlfwApi` - 28 edges
7. `Transform3D` - 18 edges
8. `Camera` - 17 edges
9. `Component_Poly` - 17 edges
10. `AssetLoader` - 17 edges

## Surprising Connections (you probably didn't know these)
- `Application::initialize()` --references--> `ApplicationConfig`  [EXTRACTED]
  detail/Application.inl → include/Application.h
- `Application::run()` --references--> `ApplicationConfig`  [EXTRACTED]
  detail/Application.inl → include/Application.h
- `Application::pickAt()` --references--> `PickResult`  [EXTRACTED]
  detail/Application.inl → include/Picking.h
- `Application::pickAtCursor()` --references--> `PickResult`  [EXTRACTED]
  detail/Application.inl → include/Picking.h
- `AssetLoader::loadModel()` --references--> `LoadOptions`  [EXTRACTED]
  detail/AssimpLoader.inl → include/ModelLoader.h

## Import Cycles
- None detected.

## Communities (26 total, 0 thin omitted)

### Community 0 - "네이티브 함수 연결"
Cohesion: 0.03
Nodes (79): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+71 more)

### Community 1 - "장면과 계층 변환"
Cohesion: 0.07
Nodes (70): appendFace(), CameraSystem::projectionMatrix(), CameraSystem::viewMatrix(), DiagramSystem::faceModelMatrix(), DiagramSystem::makePolyhedron(), DiagramSystem::modelMatrix(), EntityId, GLFace (+62 more)

### Community 2 - "Application.h 구조"
Cohesion: 0.06
Nodes (43): array, main(), main(), main(), exception_ptr, Handler, Application, assets_ (+35 more)

### Community 3 - "ModelLoader.h 구조"
Cohesion: 0.05
Nodes (45): AssetLoader::loadPrimitive(), path, size_t, string, objIndex(), uint8_t, TextureImage, height (+37 more)

### Community 4 - "장면 공개 API"
Cohesion: 0.04
Nodes (47): uint64_t, Scene, addBasis, addCuboid, addLine, addModel, addPyramid, addRightTetrahedron (+39 more)

### Community 5 - "장면 공개 API"
Cohesion: 0.05
Nodes (42): GLint, DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount (+34 more)

### Community 6 - "네이티브 함수 연결"
Cohesion: 0.06
Nodes (37): executableDirectory(), frameworkDirectory(), wglGetProc, GlfwApi, CreateWindow, DestroyWindow, GetCursorPos, GetFramebufferSize (+29 more)

### Community 7 - "앱 실행과 입력"
Cohesion: 0.08
Nodes (26): Application::findShaderDirectory(), Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler() (+18 more)

### Community 8 - "UV와 기하 계산"
Cohesion: 0.13
Nodes (23): optional, appendMesh(), generateTexcoords(), Mesh, Model, uint32_t, vec2, vec3 (+15 more)

### Community 9 - "모델과 재질 데이터"
Cohesion: 0.10
Nodes (22): GLFace, OutlineMode, uint32_t, vector, PlaneFigure, color, creaseAngleDegrees, independence (+14 more)

### Community 10 - "모델과 재질 데이터"
Cohesion: 0.11
Nodes (19): Component_Poly, parent, pivotMode, polyhedron, sideLengths, transform, visible, FaceId (+11 more)

### Community 11 - "GPU 렌더링"
Cohesion: 0.18
Nodes (16): compileShader(), GpuVertex, color, normal, position, uv, GLuint, path (+8 more)

### Community 12 - "일반 모델 로딩"
Cohesion: 0.21
Nodes (13): aiMatrix4x4, AssetLoader::loadModel(), AssimpApi, getColor, getError, getTexture, importFile, module (+5 more)

### Community 13 - "이미지 디코딩"
Cohesion: 0.19
Nodes (11): HRESULT, checkCom(), ComApartment, result, ComOwner, value, decodeImage(), path (+3 more)

### Community 14 - "교점과 UV 피킹"
Cohesion: 0.23
Nodes (13): optional, vec3, pick(), PickViewport, height, width, x, y (+5 more)

### Community 15 - "장면 공개 API"
Cohesion: 0.15
Nodes (13): mat4, Renderer::draw(), vec3, RenderOptions, background, depthTest, lightColor, lightDirection (+5 more)

### Community 16 - "교점과 UV 피킹"
Cohesion: 0.15
Nodes (13): EntityId, size_t, vec2, PickResult, barycentric, distance, entityId, faceIndex (+5 more)

### Community 17 - "모델과 재질 데이터"
Cohesion: 0.35
Nodes (10): faceMatrix(), finiteMatrix(), finiteVector(), mat4, Mesh, scale, transformMatrix(), triangles() (+2 more)

### Community 18 - "장면 공개 API"
Cohesion: 0.20
Nodes (10): CameraSystem::validCamera(), Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position (+2 more)

### Community 19 - "모델과 재질 데이터"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, shared_ptr, Material, color, repeatTexture (+2 more)

### Community 20 - "Application.h 구조"
Cohesion: 0.25
Nodes (8): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY

### Community 21 - "모델과 재질 데이터"
Cohesion: 0.29
Nodes (7): DiagramSystem::validPolyhedron(), GLShape, string, Polyhedron, faces, name, shape

### Community 22 - "모델과 재질 데이터"
Cohesion: 0.29
Nodes (7): DiagramSystem::validTransform(), Transform3D, basis, localBasis, orientation, rotationDegrees, quat

### Community 23 - "모델과 재질 데이터"
Cohesion: 0.33
Nodes (6): vec2, Vertex, color, normal, position, uv

### Community 24 - "장면 공개 API"
Cohesion: 0.60
Nodes (5): Renderer::render(), CameraSystem, projectionMatrix, validCamera, viewMatrix

### Community 25 - " 구조"
Cohesion: 0.60
Nodes (5): bottomCenter(), bounds(), Model, vec3, pair

## Knowledge Gaps
- **308 isolated node(s):** `module`, `ready`, `releaseImport`, `handle_`, `module` (+303 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 393 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `Scene` connect `장면 공개 API` to `모델과 재질 데이터`, `장면 공개 API`?**
  _High betweenness centrality (0.005) - this node is a cross-community bridge._
- **Why does `Application` connect `Application.h 구조` to `ModelLoader.h 구조`, `장면 공개 API`, `장면 공개 API`, `UV와 기하 계산`, `장면 공개 API`, `장면 공개 API`?**
  _High betweenness centrality (0.005) - this node is a cross-community bridge._
- **Why does `PlaneFigure` connect `모델과 재질 데이터` to `모델과 재질 데이터`, `모델과 재질 데이터`, `모델과 재질 데이터`, `모델과 재질 데이터`, `모델과 재질 데이터`, ` 구조`?**
  _High betweenness centrality (0.003) - this node is a cross-community bridge._
- **What connects `module`, `ready`, `releaseImport` to the rest of the system?**
  _308 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `네이티브 함수 연결` be split into smaller, more focused modules?**
  _Cohesion score 0.025 - nodes in this community are weakly interconnected._
- **Should `장면과 계층 변환` be split into smaller, more focused modules?**
  _Cohesion score 0.07394366197183098 - nodes in this community are weakly interconnected._
- **Should `Application.h 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.0596078431372549 - nodes in this community are weakly interconnected._