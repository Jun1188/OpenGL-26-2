# 실행 프로젝트와 CPU → GPU 흐름

소스와 graphify AST·정의·콜백 등록 앵커를 대조한 정적 구조 요약이다. 실행 결과를 관찰한 보고가 아니다. SDK와 과제별 프레임워크 사본은 그래프 입력에서 제외하고 루트 원본을 분석한다. 실제 프로젝트는 동일한 내부 배포본을 사용한다.

## 독립 진입점

- computer_graphics_warmUp1/FileName.cpp: PROB=2가 소스 기본값이다. PROB=1/2/3/4의 main은 조건부 대안이다. 입력 → 문제별 컨테이너/메뉴 → 종료 흐름이다.
- Computer-Graphics_WarmmingUp2/FileName.cpp의 wmain: 콘솔 생성 → UTF-8 인자 변환 → warmup::Application::run → 명령·보드·메시 처리 → 예외 출력/반환. tests/Tests.cpp는 별도 테스트 프로젝트의 main이며 실행하지 않았다.
- openGL_sample0/main.cpp: GLFW/GLEW 초기화 → 셰이더·VAO/VBO → 메시 읽기·검증 → 입력·카메라·프레임 루프 → 자원·창 해제.
- openGL_assign123456/main.cpp와 네 2D 실습의 각 main: 기존 GLFW/GLEW 초기화 → Console/RectState 또는 DiagramState → 입력·업데이트·렌더 → 해제. 각각 별도 프로그램이며 새 3D API로 이식하지 않았다.
- openGL_3dAssign_13~19/main.cpp: 각각 Console 생성 → Application 콜백 등록 → run. main의 람다는 등록 시점에 실행되지 않는다.
- framework/examples/basic.cpp와 assets.cpp: 각각 독립 예제 main이다. 프레임워크 자체에는 main이 없다. 두 예제를 한 실행 프로젝트에 동시에 등록하지 않는다.

PROJECTS.json은 각 프로젝트의 실제 컴파일 소스와 main/wmain 위치를 기록한다. 워밍업은 PROB 조건과 소스 기본 활성 여부를 구분한다. 다른 매크로 설정을 사용하면 활성 분기는 달라질 수 있다.

## 공통 3D 실행 순서

Framework.h → Application.h → Graphics.h/ModelLoader.h/Picking.h → detail inline 구현을 포함한다. src 호환용 cpp는 제거했다.

Application::run → initialize → 실행 파일 기준 상대 projectRoot/runtimeDirectory 해석 → 번들 GLFW DLL → 창·OpenGL 3.3 core 컨텍스트 → Renderer::initialize → 내장/선택 셰이더 컴파일·링크 → VAO/VBO/EBO → AssetLoader → GLFW C 콜백 등록 → initHandler.

PollEvents가 키·포커스·마우스 콜백을 실행한다. dispatch는 C 콜백의 예외를 보관한다. run은 프레임버퍼가 양수일 때 dt를 0~0.05초로 제한하여 updateHandler → renderHandler 또는 renderScene → SwapBuffers를 호출한다. 최소화 상태에서는 WaitEvents를 사용한다. 콜백 등록 순서와 본문 실행 조건을 혼동하지 않는다.

정상 종료/예외 → shutdown → AssetLoader 해제 → 컨텍스트 current → Renderer::destroy(텍스처·버퍼·프로그램) → 창 해제 → GLFW 종료. 사용자 예외는 정리 후 다시 전달한다.

## 실습별 콜백 내부

- 13: initScene이 두 도형·축을 생성하고 onKey/selectFaces가 표시할 면을 정한다.
- 14: initScene의 항등 회전·카메라 시점 → onKey의 대상·방향 → onUpdate의 로컬 회전 오른쪽 곱·월드 이동 → applyTransform → setTransform. onFocus는 이동 키를 해제한다.
- 15: initScene/resetAll → beginMotion의 모션별 실행·진행률 → onUpdate → applyFaceMotion의 T/F/S/B/1/2/4/5 행렬 합성 → setFaceLocalMatrix. applyObjectTransform은 별도 Y 자전만 적용한다.
- 16: 육면체 흐름은 15를 유지한다. beginPyramid/updatePyramid/applyPyramid가 사각뿔 모드 하나의 진행률을 하단 모서리·위 꼭짓점의 피벗 행렬로 바꾼다.
- 17: reset이 공전 그룹·모델을 생성 → onKey가 대상·방향·교환 모드를 설정 → onUpdate가 월드 위치·모션 계산 → apply가 공전 부모와 시각 모델의 로컬 변환을 설정한다. c는 두 모델을 모두 교체하며 변환을 유지한다.
- 18: initScene이 전체 루트·중심 구·행성 위치·궤도 선·달 방향을 생성 → onUpdate의 각도·반지름·입력 → apply가 같은 평면·반지름으로 궤도 선과 천체 위치를 설정한다.
- 19: reset/box가 부품 계층 생성 → onKey의 모션 상태 → onUpdate의 이동 한계·카메라 포즈 → apply의 접지 높이·중앙 회전·상부 교환·포신/깃대 피벗. 카메라 Y축은 현재 시선과 오른쪽 벡터로 계산한다.

## 좌표와 GPU 데이터

로컬 정점 → T(-localBasis) → S → rotation(mat3) → T(basis) → 조상 행렬 → 면 추가 변환 → view → projection → clip/NDC → viewport → 래스터화 → 셰이더·깊이 → 화면.

worldMatrix와 faceWorldMatrix가 부모·면 변환을 계산한다. 그룹은 메시 없는 전용 노드이며 upload는 그룹 자체를 건너뛴다. 부모 visibility는 자식에게 상속하지 않는다.

형상/revision/textures 변경 → Renderer::upload → GpuVertex(position/color/UV/normal) → VBO/EBO/VAO. draw는 faceWorldMatrix 또는 선 owner의 worldMatrix → uMVP/uNormalMatrix/uTint → DrawElements를 수행한다. 변환·부모 연결·색상 배율만 바뀌면 정점을 재업로드하지 않는다. FeatureEdges는 공면 내부 대각선을 제외한다. 와이어+깊이 테스트는 전체 삼각형의 깊이 전처리 후 물체 색상의 선을 그린다.

screenRay는 카메라 역변환으로 광선을 만들고 pick은 같은 faceWorldMatrix로 가장 가까운 표시 삼각형의 교점을 계산한다. pickAt은 창 좌표를 프레임버퍼 픽셀로 변환한다. 선과 투명 텍스처 픽셀은 별도 피킹하지 않는다.

loadPrimitive는 내부 상대경로 → OBJ 부분집합·삼각분할·공면 병합이다. loadModel은 기능 확인 → 번들 Assimp → 노드 변환을 정점에 반영 → Model/Mesh다. loadTexture는 Windows WIC → RGBA8 → Material → 선택적 GPU 텍스처다. DLL/COM/GPU 드라이버 내부는 분석 범위 밖이다.

## 한계

같은 이름 메서드·오버로드·간접 호출은 AST가 완전히 해석하지 못한다. 콜백 본문은 소스 앵커로 보강했다. GRAPH_HEALTH.md의 미해결 관계를 숨기지 않는다. 구조 관계 수는 실행 호출 횟수가 아니다. 빌드·화면·입력·종료 테스트와 벤치마크는 사용자 확인이 필요하다.

## 독립 Motion 예제 추가 (2026-10-22)

framework/examples/motion.cpp의 main은 기본·자산 예제 및 각 실습의 main과 별도 진입점이다. 프로젝트에 자동 등록하지 않는다. PROJECTS.json의 standalone_example 항목으로 구분한다.

main -> Application 생성 -> 콜백 등록 -> run -> init 콜백에서 Scene 객체·공유 path/spin/global Motion과 두 레이어 가지 생성 -> PollEvents에서 키/클릭 콜백 -> 사용자 update(있을 때) -> Scene::updateMotions -> 렌더 -> 종료와 shutdown. 1~6키는 경로 설정 교체·seek, Space는 시계 정지, S/G는 기여 전환, M은 독립 반사 행렬, I는 레이어 삽입·삭제다. 클릭은 같은 faceWorldMatrix를 사용한 pickAt으로 연결된다.

공통 Application::run에 새 Motion 갱신이 추가되었지만 기존 실습들의 Console 상태 계산과 입력 정책은 유지한다. 구체적 CPU-GPU 흐름과 World/Local 합성은 ../framework-analysis/graphify-out/FRAMEWORK_FLOW.md의 추가 절을 참고한다. AST는 구조 분석이며 실제 콜백 실행·화면을 검증하지 않는다.
