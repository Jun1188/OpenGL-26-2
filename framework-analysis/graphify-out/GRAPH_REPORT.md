# Graph Report - framework  (2026-10-11)

## Corpus Check
- Corpus is 7,966 words; framework-only AST analysis requested. Vendor SDKs and generated reports excluded.

## Summary
- 613 nodes · 964 edges · 23 communities
- Extraction: 97% EXTRACTED · 3% INFERRED · 0% AMBIGUOUS · INFERRED: 26 edges (avg confidence: 0.82)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `a194f470`
- Source freshness: compare framework code hashes in SOURCE_MANIFEST.json. Documentation-only commits do not change these inputs.
- Rebuild with scan_root=framework and output/cache_root=framework-analysis. See ../README.md. Do not use the entire repository or analysis folder as the source corpus.

## Community Hubs (Navigation)
- OpenGL 함수 연결
- 모델 로딩과 기하 계산
- 도형 생성과 좌표 변환
- 카메라와 피킹·렌더 설정
- 공개 API와 사용 예제
- 앱 실행과 입력 처리
- GPU 드로우 범위
- 장면 편집과 갱신
- 윈도우와 DLL 로딩
- 면 메시와 외곽선
- GPU 버퍼와 셰이더
- 텍스처 이미지 디코딩
- 모델 행렬과 검증
- 색상과 보조 선
- 마우스 좌표 데이터
- 이미지와 정점 변환
- 모델과 면 목록
- SRT와 로컬 기준점
- 객체 표시와 기준점 모드
- 면 식별과 검증
- 재질과 텍스처
- 일반 정점 데이터
- 하단 중점 계산

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 82 edges
2. `Scene` - 54 edges
3. `Application` - 48 edges
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

### Community 1 - "모델 로딩과 기하 계산"
Cohesion: 0.05
Nodes (58): aiMatrix4x4, appendMesh(), Mesh, Model, uint32_t, vec3, vector, mergeCoplanar() (+50 more)

### Community 2 - "도형 생성과 좌표 변환"
Cohesion: 0.08
Nodes (63): appendFace(), CameraSystem::projectionMatrix(), CameraSystem::viewMatrix(), DiagramSystem::faceModelMatrix(), DiagramSystem::makePolyhedron(), DiagramSystem::modelMatrix(), EntityId, GLFace (+55 more)

### Community 3 - "카메라와 피킹·렌더 설정"
Cohesion: 0.06
Nodes (45): array, mat4, Renderer::draw(), Renderer::render(), CameraSystem::validCamera(), optional, Camera, farPlane (+37 more)

### Community 4 - "공개 API와 사용 예제"
Cohesion: 0.06
Nodes (41): main(), main(), exception_ptr, Handler, Application, assets_, callbackError_, camera_ (+33 more)

### Community 5 - "앱 실행과 입력 처리"
Cohesion: 0.06
Nodes (39): Application::findShaderDirectory(), Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler() (+31 more)

### Community 6 - "GPU 드로우 범위"
Cohesion: 0.05
Nodes (42): GLint, DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount (+34 more)

### Community 7 - "장면 편집과 갱신"
Cohesion: 0.05
Nodes (42): uint64_t, Scene, addBasis, addCuboid, addLine, addModel, addPyramid, addRightTetrahedron (+34 more)

### Community 8 - "윈도우와 DLL 로딩"
Cohesion: 0.06
Nodes (37): executableDirectory(), frameworkDirectory(), wglGetProc, GlfwApi, CreateWindow, DestroyWindow, GetCursorPos, GetFramebufferSize (+29 more)

### Community 9 - "면 메시와 외곽선"
Cohesion: 0.10
Nodes (20): GLFace, OutlineMode, PlaneFigure, color, creaseAngleDegrees, independence, localAnimationMatrix, localVertices (+12 more)

### Community 10 - "GPU 버퍼와 셰이더"
Cohesion: 0.18
Nodes (16): compileShader(), GpuVertex, color, normal, position, uv, GLuint, path (+8 more)

### Community 11 - "텍스처 이미지 디코딩"
Cohesion: 0.19
Nodes (11): HRESULT, checkCom(), ComApartment, result, ComOwner, value, decodeImage(), path (+3 more)

### Community 12 - "모델 행렬과 검증"
Cohesion: 0.35
Nodes (10): faceMatrix(), finiteMatrix(), finiteVector(), mat4, Mesh, scale, transformMatrix(), triangles() (+2 more)

### Community 13 - "색상과 보조 선"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, optional, Line3D, color, end (+2 more)

### Community 14 - "마우스 좌표 데이터"
Cohesion: 0.25
Nodes (8): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY

### Community 15 - "이미지와 정점 변환"
Cohesion: 0.29
Nodes (7): uint32_t, uint8_t, vector, TextureImage, height, rgba, width

### Community 16 - "모델과 면 목록"
Cohesion: 0.29
Nodes (7): DiagramSystem::validPolyhedron(), GLShape, string, Polyhedron, faces, name, shape

### Community 17 - "SRT와 로컬 기준점"
Cohesion: 0.29
Nodes (7): DiagramSystem::validTransform(), Transform3D, basis, localBasis, orientation, rotationDegrees, quat

### Community 18 - "객체 표시와 기준점 모드"
Cohesion: 0.29
Nodes (7): Component_Poly, pivotMode, polyhedron, sideLengths, transform, visible, PivotMode

### Community 19 - "면 식별과 검증"
Cohesion: 0.33
Nodes (5): FaceId, entityId, faceIndex, EntityId, size_t

### Community 20 - "재질과 텍스처"
Cohesion: 0.33
Nodes (6): shared_ptr, Material, color, repeatTexture, texture, useTexture

### Community 21 - "일반 정점 데이터"
Cohesion: 0.33
Nodes (6): vec2, Vertex, color, normal, position, uv

### Community 22 - "하단 중점 계산"
Cohesion: 0.60
Nodes (5): bottomCenter(), bounds(), Model, vec3, pair

## Knowledge Gaps
- **295 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `Terminate` (+290 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 378 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **클릭한 면의 피킹과 렌더링은 같은 부모·면 변환을 어디에서 공유하는가?**
  _Picking.h와 Renderer.inl에서 faceModelMatrix 호출을 대조한다._
- **Scene 메시 수정은 어떤 경로로 다음 프레임의 GPU 버퍼 갱신에 반영되는가?**
  _Scene setter, geometryRevision, Renderer::render/upload를 추적한다._
- **모델과 텍스처 모드가 꺼져 있으면 로더와 DLL 연결은 어디에서 중단되는가?**
  _FeatureOptions, AssetLoader 분기와 AssimpApi::load를 추적한다._
## 분석 범위와 무결성

소스 루트는 framework이고 출력은 별도 framework-analysis/graphify-out이다. Framework.h, include, detail(.inl 포함), src, examples의 C++ 17개 파일만 입력이다. 다른 실행 프로젝트와 각 프로젝트의 framework 사본은 포함하지 않았다. SDK, docs/변경 전후 스냅샷, 자산 데이터와 이전 그래프는 제외했다. GLSL 파일은 detail/Shaders.h에 포함된 문자열의 별도 사본이다. 이 그래프는 문자열 내부 GLSL의 의미를 별도 AST로 분석하지 않는다.

613개 노드와 964개 방향 관계를 생성했다. 원시 관계에는 대상 심볼을 해석하지 못한 관계 48개가 있고, 같은 끝점의 방향 관계 77개가 병합될 수 있다. 세부 값은 GRAPH_HEALTH.md와 EXTRACTION.json에 보존한다. 타입 참조, 등록된 콜백과 DLL 함수 포인터를 실제 호출 순서로 단정하지 않는다. FRAMEWORK_FLOW.md는 해당 부분을 소스로 대조한 설명이다.

코드의 AST 분석만 수행했다. 별도 의미 분석 API 입력/출력 토큰은 0이다. graphify의 구조 검색 크기 벤치마크는 추정 토큰 수이며 실제 모델 비용·성능 측정이나 framework 동작 테스트가 아니다. 빌드·테스트는 수행하지 않았다.
