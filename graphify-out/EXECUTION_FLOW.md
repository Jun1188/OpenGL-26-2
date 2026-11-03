# 실행 프로젝트와 CPU → GPU 흐름 (2026-11-03)

현재 소스·graphify AST의 정적 분석이다. 실행 프로젝트들은 별도 프로그램이며 한 호출 흐름으로 합치지 않는다.
SDK와 독립 프로젝트의 framework 중복 사본은 그래프에서 제외한다. 공통 framework 33개 파일과 저장소 실행 소스를 분석했다.

## 독립 프로젝트·진입점

- Computer-Graphics_WarmmingUp2/Computer-Graphics_WarmmingUp2.vcxproj: `Computer-Graphics_WarmmingUp2/FileName.cpp:54`
- Computer-Graphics_WarmmingUp2/tests/WarmupTests.vcxproj: `Computer-Graphics_WarmmingUp2/tests/Tests.cpp:262`
- computer_graphics_warmUp1/computer_graphics_warmUp1.vcxproj: `computer_graphics_warmUp1/FileName.cpp:553` (PROB == 1, 소스 기본 활성=False), `computer_graphics_warmUp1/FileName.cpp:903` (PROB == 2, 소스 기본 활성=True), `computer_graphics_warmUp1/FileName.cpp:1123` (PROB == 3, 소스 기본 활성=False), `computer_graphics_warmUp1/FileName.cpp:1319` (PROB == 4, 소스 기본 활성=False)
- openGL_2dAssign11/openGL_sample2.vcxproj: `openGL_2dAssign11/main.cpp:109`
- openGL_2dAssign12/openGL_sample2.vcxproj: `openGL_2dAssign12/main.cpp:59`
- openGL_2dAssign_10/openGL_sample2.vcxproj: `openGL_2dAssign_10/main.cpp:86`
- openGL_2dAssign_789/openGL_sample2.vcxproj: `openGL_2dAssign_789/main.cpp:103`
- openGL_3dAssign_13/openGL_sample2.vcxproj: `openGL_3dAssign_13/main.cpp:4`
- openGL_3dAssign_14/openGL_sample2.vcxproj: `openGL_3dAssign_14/main.cpp:4`
- openGL_3dAssign_15/openGL_sample2.vcxproj: `openGL_3dAssign_15/main.cpp:5`
- openGL_3dAssign_16/openGL_assign_16.vcxproj: `openGL_3dAssign_16/main.cpp:5`
- openGL_3dAssign_17/openGL_assign_17.vcxproj: `openGL_3dAssign_17/main.cpp:4`
- openGL_3dAssign_18/openGL_assign_18.vcxproj: `openGL_3dAssign_18/main.cpp:4`
- openGL_3dAssign_19/openGL_assign_19.vcxproj: `openGL_3dAssign_19/main.cpp:4`
- openGL_3dAssign_20/openGL_assign_20.vcxproj: `openGL_3dAssign_20/main.cpp:5`
- openGL_3dAssign_21/openGL_assign_21.vcxproj: `openGL_3dAssign_21/main.cpp:5`
- openGL_3dAssign_22/openGL_assign_22.vcxproj: `openGL_3dAssign_22/main.cpp:5`
- openGL_3dAssign_23/openGL_assign_23.vcxproj: `openGL_3dAssign_23/main.cpp:5`
- openGL_assign123456/openGL_sample2.vcxproj: `openGL_assign123456/main.cpp:79`
- openGL_sample0/openGL_sample0.vcxproj: `openGL_sample0/main.cpp:308`
- standard/standard.vcxproj: `standard/main.cpp:4`
- 독립 예제: `framework/examples/basic.cpp:main 검색`
- 독립 예제: `framework/examples/assets.cpp:main 검색`
- 독립 예제: `framework/examples/motion.cpp:main 검색`

WarmUp1의 PROB 조건은 main 앞의 실제 #if/#elif에서 읽었다. 현재 소스 기본값은 2이며 컴파일러 정의가 바뀌면 다른 진입점이 선택된다.
WarmupTests는 별도 테스트 프로젝트다. 목록 수록만 했으며 실행하지 않았다.

## 공통 3D 프로그램: 실습 13~23과 standard

각 main은 Application과 Console을 생성하고 해당 Console의 initScene/onKey/onUpdate/onMouse/onCursor/onRender/onFocus 중 필요한 콜백을 등록한다.
등록은 저장이며 즉시 실행이 아니다. 각 프로젝트의 main에 실제로 등록한 콜백만 실행한다.
Application::run → initialize → GLFW 창/현재 컨텍스트 → Renderer::initialize → initHandler → 이벤트 루프 순서다.
Renderer는 RenderConfig로 GL 프로그램·VAO/VBO/EBO를 구성한다. initHandler의 Console::initScene은 Scene의 ID·컴포넌트·모션·카메라를 준비한다.
PollEvents가 입력 콜백을 호출한다. 유효한 프레임에서 updateHandler → Scene::updateMotions → renderHandler 또는 renderScene → SwapBuffers를 처리한다.
MotionSystem은 계산 성공 뒤 시간/샘플을 확정하고 활성 모션의 삽입 동작을 실행한다. 여러 뷰의 렌더 패스는 상태를 중복 갱신하지 않는다.

## 새 육면체 모드

bindCuboidMode는 init/update/modified-key 핸들러를 설치한다.
init → CuboidMode::apply → 임시 Scene에 육면체/선택적 모션/레이어 구성 → 장면 교체 → setup 핸들러 순서다.
키 입력 → dispatch → 이름으로 현재 ID 조회 → CuboidCommand → SceneActions/기존 Scene·MotionSystem API 순서다.
mode.update는 응용 업데이트만 처리한다. 실제 자동 모션 시간 진행은 Application에서 한 번 수행한다.
ECS 데이터 저장은 Scene에 남고 GPU 소유권은 Renderer에 남는다.

## CPU → GPU

Scene/Mesh 데이터 → Renderer::upload의 RenderVertex 배열 → 기본 복사 또는 packVertices → VBO 바이트 배열 순서다.
VertexLayout의 location/components/type/normalized/stride/offset/integer가 VAO 입력 해석을 정한다. 정수 입력은 VertexAttribIPointer로 연결한다.
Scene::worldMatrix/faceWorldMatrix → Renderer::draw → 기본 uniform 이름 매핑 → 추가 bindUniforms → 정점 셰이더 → 클리핑/래스터화 → 프래그먼트 셰이더 → 깊이/컬링/프레임버퍼 순서다.
GLSL의 실제 계산은 셰이더 소스를 확인한다. 렌더러가 자료형을 자동 추론하거나 셰이더 의미를 변환하지 않는다.

## 다른 독립 프로그램

WarmUp2의 wmain은 warmup::Console과 warmup::Application을 만들고 run(arguments)을 호출한다. 공통 graphics::Application과 별개다.
이전 2D/기초 실습의 main은 각자의 GLFW 초기화·객체 생성·콜백·루프·종료 코드를 가진다. 해당 소스와 프로젝트 항목을 따로 읽는다.
서로 같은 main/Console 이름만으로 프로젝트 사이 호출 관계를 확정하지 않는다.

## 종료·한계

공통 Application은 정상 종료와 예외에서 shutdown을 호출하며 유효 컨텍스트에서 GPU·창·GLFW를 정리한다.
컨텍스트 실패 경로는 Renderer::abandon으로 삭제 호출을 피한다.
AST의 동적 콜백/같은 이름 함수 연결은 후보 관계다. GRAPH_HEALTH.md의 dangling endpoint와 관계 병합 수치를 함께 읽는다.
빌드·테스트·앱·브라우저 실행은 하지 않았다. 사용자 확인은 framework/docs/USER_CHECKS.md를 따른다.

## 면의 변환과 회전 의미

Scene::setFaceTransformMode는 Hierarchy/Independent와 전환 시 자세 보존을 설정한다.
Scene::faceWorldMatrix는 상속/고정 기준 F와 면 SRT·추가 애니메이션을 합성한다. 기본 계층 모드는 기존 결과를 유지한다.
PatternSystem::rotateFace는 EdgeHinge/SelfSpin/ModelOrbit의 축을 계산하고 Model 순서의 애니메이션 행렬을 설정한다.
CuboidMode는 초기 SRT·모션/레이어 연결 뒤 독립 면의 기준을 고정한다. 렌더러와 피킹은 같은 faceWorldMatrix를 소비한다.
수학 계약과 실제 사용자 확인 절차는 framework/docs/FACE_TRANSFORMS.md를 읽는다.
