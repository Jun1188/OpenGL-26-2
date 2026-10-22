# 현재 실행 구조와 OBJ 변경

2026-10-22 작업 트리의 소스·graphify AST·함수 정의·콜백 등록 앵커를 대조한 정적 설명이다. 빌드·실행 결과가 아니다. 루트 framework와 샘플별 framework의 변경 대상 코드는 같고, 프로젝트 설명 문서는 기존 차이를 유지했다. 실습 20은 사용자 미추적 폴더라 로컬 반영만 했다.

## 독립 실행 파일

| 프로젝트 | main부터 종료까지 |
|---|---|
| standard | main → Console sample 구성 → init/key/mouse 등록 → Application::run. initScene에서 객체·Motion·레이어 구성, onKey에서 상태 변경, onMouse에서 피킹. 공통 run 루프와 shutdown 사용 |
| openGL_sample0 | main → GLFW/GLEW와 셰이더 생성 → loadMeshVertices → MeshParser::parse / MeshValidator::validate → VAO/VBO와 카메라·입력 루프 → glDeleteBuffers/VertexArrays/Program → glfwTerminate. 프레임워크 AssetLoader와 별도 구현 |
| 실습 15 | main → Console practice → init/key/update 등록 → run. initScene/resetAll → beginMotion → onUpdate → applyFaceMotion/setFaceLocalMatrix와 applyObjectTransform |
| 실습 16 | 15와 같은 공통 경로에 beginPyramid/updatePyramid/applyPyramid 추가. 피벗·진행률을 면 행렬로 전달 |
| 실습 17 | main의 init/modified-key/update/focus 등록 → run. reset이 그룹·모델 구성, onKey/onUpdate가 상태 계산, apply가 부모와 자식 변환 반영 |
| 실습 18 | init/modified-key/update/focus 등록 → run. initScene에서 천체·궤도 구성, onUpdate → apply에서 선·천체 변환 반영 |
| 실습 19 | init/modified-key/update/focus 등록 → run. reset/box가 부품 계층 생성, onKey/onUpdate → apply가 탱크 부품·카메라 상태 반영 |
| 실습 20 | 별도 main과 별도 Console. 현재 initScene/reset/box/apply/onKey/onUpdate/onFocus는 탱크 예제 구조를 사용하며, main의 창 제목은 Practice 20이다. 다른 실행 파일과 상태를 공유하지 않음 |
| examples/basic.cpp | 별도 main → Application과 초기 장면·입력 콜백 등록 → 공통 run/shutdown |
| examples/assets.cpp | 별도 main → 인자·텍스처 옵션 → init 콜백에서 loadObj, 경고·오류 출력, addModel → 마우스 피킹·Esc 콜백 → 공통 run/shutdown |
| examples/motion.cpp | 별도 main → Motion·레이어 초기화 콜백 → 공통 run과 Scene의 Motion 갱신 → shutdown |

현재 8개 vcxproj는 각각 독립 실행 프로젝트다. 예제 3개는 별도로 main 하나씩 등록한다. 그래프의 PROJECTS.json은 실제 컴파일 소스와 진입점 위치를 기록한다. 과거 기록의 워밍업·2D·실습 13/14는 현재 작업 트리에서 확인되지 않는다. 서로 다른 main의 호출 흐름을 합치지 않았다.

## 공통 프레임 루프

Framework.h가 Application.h와 include 계층 및 detail inline 구현을 포함한다. Application::run → initialize에서 경로 해석 → GLFW DLL → 창·OpenGL 컨텍스트 → Renderer::initialize → 셰이더·VAO/VBO/EBO → AssetLoader 생성 → GLFW 콜백 등록 → initHandler 순으로 준비한다. 콜백 등록은 본문 실행이 아니다.

PollEvents에서 입력 콜백이 실행된다. 유효한 프레임버퍼에서는 updateHandler → Scene::updateMotions → renderHandler 또는 renderScene → SwapBuffers로 진행한다. 최소화 상태는 WaitEvents를 사용한다. 이벤트 예외는 dispatch가 보관하고 run이 자원을 정리한 뒤 전달한다. 종료 시 shutdown이 AssetLoader, GPU 자원, 창, GLFW를 정리한다.

## OBJ와 CPU → GPU

입력 상대경로 → AssetLoader::resolve의 루트 경계 → 파일 바이트 → objText → parseObj 표준 해석 → ObjParseError일 때 허용된 호환 해석 → 독립 코너 인덱스와 누락 속성 처리 → 정규화된 작업 공간에서 다각형 분할 → 원래 좌표의 Model/Mesh → finishLoad의 검사·피벗 → ModelResult.

호출자는 성공을 확인하고 Scene::addModel에 모델과 pivotMode/localBasis를 전달한다. Scene 변경의 geometryRevision에 따라 Renderer::upload가 GpuVertex(position/color/UV/normal)를 VAO/VBO/EBO에 전달한다. loadObj 자체는 OpenGL 핸들을 생성하지 않는다. UV는 텍스처 표시 설정과 독립적으로 보존한다. texturePath가 있을 때에만 텍스처 기능과 면별 UV를 요구한다.

Renderer::draw는 faceWorldMatrix/worldMatrix 및 MotionSystem::layerMatrices를 사용해 모델 변환과 uMVP/uNormalMatrix/uTint를 설정한다. 로컬 정점 → 모델·부모·면 변환 → view/projection → clip/NDC → viewport → 래스터화 → 프래그먼트 셰이더·깊이 처리 → 화면으로 진행한다. pick/screenRay도 같은 면 월드 변환을 사용한다. 내장 셰이더는 detail/Shaders.h, 선택 외부 셰이더는 shaders/*.glsl에 있다.

## 정의와 호출 위치

| 역할 | 소스 위치 |
|---|---|
| OBJ API 선언 | `framework/include/ModelLoader.h:81` |
| OBJ API 본문 | `framework/detail/ObjLoader.inl:280` |
| OBJ 파싱 | `framework/detail/ObjLoader.inl:109` |
| 문자 인코딩 | `framework/detail/ObjLoader.inl:73` |
| 음수/전방 참조 | `framework/detail/ObjLoader.inl:50` |
| 기존 primitive 호출 | `framework/detail/PrimitiveLoader.inl:3` |
| 기존 model 호출 | `framework/include/ModelLoader.h:90` |
| 소비 예제 | `framework/examples/assets.cpp:21` |
| 실행 초기화 | `framework/detail/Application.inl:39` |
| Scene 등록 | `framework/detail/Scene.inl:568` |

## 그래프 증거와 한계

프레임워크 그래프는 22개 소스, 1,010개 노드·1,524개 간선이다. 전체 그래프는 49개 소스, 1,510개 노드·2,341개 간선이다. SDK·중복 프레임워크·빌드 산출물·임시 백업·배포 ZIP은 제외했다. framework-analysis/graphify-out와 graphify-out에 JSON·HTML·보고서·소스 해시를 갱신했다.

AST의 오버로드·외부 타입·std::function·GLFW 동적 심벌 해석은 완전하지 않다. 프레임워크에는 endpoint 미해결 간선 49개, 전체에는 101개가 있으며 GRAPH_HEALTH.md에 남겼다. 다른 프로그램의 같은 함수 이름으로 연결된 간선은 확정 호출로 보지 않는다. 위의 직접 호출·등록 위치는 소스와 대조했고, 드라이버·DLL 내부와 실제 호출 시점의 실행 결과는 확인하지 않았다.
