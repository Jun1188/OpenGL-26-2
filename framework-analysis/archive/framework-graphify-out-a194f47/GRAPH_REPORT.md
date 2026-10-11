# Graph Report - framework  (2026-10-11)

## Corpus Check
- Corpus is ~4,767 words - fits in a single context window. You may not need a graph.

## Summary
- 609 nodes · 946 edges · 23 communities
- Extraction: 99% EXTRACTED · 1% INFERRED · 0% AMBIGUOUS · INFERRED: 12 edges (avg confidence: 0.85)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `a22a2006`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- OpenGL 함수 연결
- 기하와 카메라 변환
- 옵트인 자산 로딩
- Graphics.h
- 앱 실행 루프
- 드로우 범위 데이터
- 장면 편집 API
- 윈도우와 DLL 로딩
- 앱 상태와 콜백
- 면 메시 데이터
- GPU 버퍼와 셰이더
- 텍스처 이미지 디코딩
- 삼각화와 경계 추출
- Line3D
- 모델 행렬과 검증
- 재질과 텍스처
- 공개 진입점과 콜백
- 마우스 좌표 데이터
- 모델의 메시 목록
- SRT와 기준점
- 모델 상태
- 일반 정점 데이터
- 하단 중점 계산

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 82 edges
2. `Scene` - 54 edges
3. `Application` - 46 edges
4. `Renderer` - 36 edges
5. `PlaneFigure` - 33 edges
6. `GlfwApi` - 28 edges
7. `Transform3D` - 18 edges
8. `Camera` - 17 edges
9. `AssetLoader` - 17 edges
10. `PickResult` - 17 edges

## Surprising Connections (you probably didn't know these)
- `Application::initialize()` --references--> `ApplicationConfig`  [EXTRACTED]
  detail/Application.inl → include/Application.h
- `Application::run()` --references--> `ApplicationConfig`  [EXTRACTED]
  detail/Application.inl → include/Application.h
- `uploadTexture()` --references--> `TextureImage`  [EXTRACTED]
  detail/Renderer.inl → include/Model.h
- `Renderer::upload()` --calls--> `validPolyhedron`  [EXTRACTED]
  detail/Renderer.inl → include/Graphics.h
- `Renderer::upload()` --references--> `Scene`  [EXTRACTED]
  detail/Renderer.inl → include/Graphics.h

## Import Cycles
- None detected.

## Communities (23 total, 0 thin omitted)

### Community 0 - "OpenGL 함수 연결"
Cohesion: 0.03
Nodes (79): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+71 more)

### Community 1 - "기하와 카메라 변환"
Cohesion: 0.08
Nodes (62): appendFace(), CameraSystem::projectionMatrix(), CameraSystem::viewMatrix(), DiagramSystem::faceModelMatrix(), DiagramSystem::makePolyhedron(), DiagramSystem::modelMatrix(), EntityId, GLFace (+54 more)

### Community 2 - "옵트인 자산 로딩"
Cohesion: 0.06
Nodes (49): aiMatrix4x4, uint8_t, TextureImage, height, rgba, width, AssetLoader, features_ (+41 more)

### Community 3 - "Graphics.h"
Cohesion: 0.06
Nodes (46): array, mat4, Renderer::draw(), Renderer::render(), CameraSystem::validCamera(), optional, Camera, farPlane (+38 more)

### Community 4 - "앱 실행 루프"
Cohesion: 0.05
Nodes (42): Application::findShaderDirectory(), Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler() (+34 more)

### Community 5 - "드로우 범위 데이터"
Cohesion: 0.05
Nodes (42): GLint, DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount (+34 more)

### Community 6 - "장면 편집 API"
Cohesion: 0.05
Nodes (42): uint64_t, Scene, addBasis, addCuboid, addLine, addModel, addPyramid, addRightTetrahedron (+34 more)

### Community 7 - "윈도우와 DLL 로딩"
Cohesion: 0.06
Nodes (37): executableDirectory(), frameworkDirectory(), wglGetProc, GlfwApi, CreateWindow, DestroyWindow, GetCursorPos, GetFramebufferSize (+29 more)

### Community 8 - "앱 상태와 콜백"
Cohesion: 0.06
Nodes (35): exception_ptr, Application, assets_, callbackError_, camera_, cursorHandler_, findShaderDirectory, focusHandler_ (+27 more)

### Community 9 - "면 메시 데이터"
Cohesion: 0.10
Nodes (22): GLFace, OutlineMode, uint32_t, vector, PlaneFigure, color, creaseAngleDegrees, independence (+14 more)

### Community 10 - "GPU 버퍼와 셰이더"
Cohesion: 0.18
Nodes (16): compileShader(), GpuVertex, color, normal, position, uv, GLuint, path (+8 more)

### Community 11 - "텍스처 이미지 디코딩"
Cohesion: 0.19
Nodes (11): HRESULT, checkCom(), ComApartment, result, ComOwner, value, decodeImage(), path (+3 more)

### Community 12 - "삼각화와 경계 추출"
Cohesion: 0.32
Nodes (11): appendMesh(), Mesh, Model, uint32_t, vec3, vector, mergeCoplanar(), meshOutline() (+3 more)

### Community 13 - "Line3D"
Cohesion: 0.17
Nodes (11): FaceId, entityId, faceIndex, EntityId, optional, size_t, Line3D, color (+3 more)

### Community 14 - "모델 행렬과 검증"
Cohesion: 0.35
Nodes (10): faceMatrix(), finiteMatrix(), finiteVector(), mat4, Mesh, scale, transformMatrix(), triangles() (+2 more)

### Community 15 - "재질과 텍스처"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, shared_ptr, Material, color, repeatTexture (+2 more)

### Community 16 - "공개 진입점과 콜백"
Cohesion: 0.25
Nodes (4): Handler, requestClose, dispatch(), GLFWwindow

### Community 17 - "마우스 좌표 데이터"
Cohesion: 0.25
Nodes (8): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY

### Community 18 - "모델의 메시 목록"
Cohesion: 0.29
Nodes (7): DiagramSystem::validPolyhedron(), GLShape, string, Polyhedron, faces, name, shape

### Community 19 - "SRT와 기준점"
Cohesion: 0.29
Nodes (7): DiagramSystem::validTransform(), Transform3D, basis, localBasis, orientation, rotationDegrees, quat

### Community 20 - "모델 상태"
Cohesion: 0.29
Nodes (7): Component_Poly, pivotMode, polyhedron, sideLengths, transform, visible, PivotMode

### Community 21 - "일반 정점 데이터"
Cohesion: 0.33
Nodes (6): vec2, Vertex, color, normal, position, uv

### Community 22 - "하단 중점 계산"
Cohesion: 0.60
Nodes (5): bottomCenter(), bounds(), Model, vec3, pair

## Knowledge Gaps
- **301 isolated node(s):** `width`, `height`, `title`, `shaderDirectory`, `projectRoot` (+296 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 385 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
## 분석 범위와 한계

framework의 공개 헤더, detail 헤더/inl, 호환용 src 파일만 AST로 분석했다. dependencies, 예제와 문서는 구조 그래프에서 제외했다. .inl은 이 실행에서만 C++ 추출기로 등록했다. 설치 패키지는 변경하지 않았다. 의미 분석 API를 호출하지 않았고 추가 API 비용은 0이다.

엣지는 소스에서 추출한 정적 관계다. 타입 참조와 함수 포인터 연결을 실제 실행 호출로 단정하지 않는다. 해석되지 않은 심볼, 중복/병합 관계는 GRAPH_HEALTH.md와 원시 EXTRACTION.json에 보존한다. Windows/OpenGL 실행, 링크, 화면 결과는 확인하지 않았다. 기존 과제의 프로젝트 연결은 EXECUTION_FLOW.md에서 별도로 구분한다.
