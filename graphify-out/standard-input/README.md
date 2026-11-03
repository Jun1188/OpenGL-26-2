# 입력·모드 구조 그래프

2026-11-03. graphify AST로 현재 존재하는 C++ 148개 파일을 추출했다. 3,395개 노드와 6,358개 관계다. 입력 소스 커밋은 `cb244b55d5d16c32eb2dc983f931301f40defc94`이다.

[그래프](graph.html) · [기계 판독 데이터](graph.json) · [프로젝트별 진입점](PROJECTS.json) · [입력 파일·해시·한계](SOURCE_MANIFEST.json) · [모드 입력 평가](../../standard/docs/MODE_INPUT_AUDIT.md)

## 확인 방법과 범위

실제 graphify 추출기를 사용했다. 외부 의미 분석 API, 빌드, 앱 실행, 테스트, 벤치마크, 추출 진단 검증기를 실행하지 않았다. 함수 포인터·콜백·가상 함수의 런타임 연결은 AST만으로 확정하지 않는다. 서로 다른 실행 프로그램의 같은 Console 이름으로 생긴 연결은 확정 호출이 아니다. 루트 framework와 standard에서 변경한 동봉 Picking/Application만 포함하며 SDK와 나머지 중복 배포본은 제외했다.

기존 전역 graphify-out의 그래프를 덮어쓰지 않았다. 이 폴더는 입력·모드 작업의 별도 스냅샷이다. 기존 부재 파일은 복원·삭제하지 않았다. 과거 진입점만 남은 워밍업·sample0은 PROJECTS.json의 available=false로 구분한다.

## 프로그램별 main

- `Computer-Graphics_WarmmingUp2/Computer-Graphics_WarmmingUp2.vcxproj`: `Computer-Graphics_WarmmingUp2/FileName.cpp:54`. 작업 전부터 부재. 위 진입점은 과거 색인 정보.
- `Computer-Graphics_WarmmingUp2/tests/WarmupTests.vcxproj`: `Computer-Graphics_WarmmingUp2/tests/Tests.cpp:262`. 작업 전부터 부재. 위 진입점은 과거 색인 정보.
- `computer_graphics_warmUp1/computer_graphics_warmUp1.vcxproj`: `computer_graphics_warmUp1/FileName.cpp:553`, `computer_graphics_warmUp1/FileName.cpp:903`, `computer_graphics_warmUp1/FileName.cpp:1123`, `computer_graphics_warmUp1/FileName.cpp:1319`. 작업 전부터 부재. 위 진입점은 과거 색인 정보.
- `openGL_2dAssign11/openGL_sample2.vcxproj`: `openGL_2dAssign11/main.cpp:109`. 현재 소스 존재.
- `openGL_2dAssign12/openGL_sample2.vcxproj`: `openGL_2dAssign12/main.cpp:59`. 현재 소스 존재.
- `openGL_2dAssign_10/openGL_sample2.vcxproj`: `openGL_2dAssign_10/main.cpp:86`. 현재 소스 존재.
- `openGL_2dAssign_789/openGL_sample2.vcxproj`: `openGL_2dAssign_789/main.cpp:103`. 현재 소스 존재.
- `openGL_3dAssign_13/openGL_sample2.vcxproj`: `openGL_3dAssign_13/main.cpp:4`. 현재 소스 존재.
- `openGL_3dAssign_14/openGL_sample2.vcxproj`: `openGL_3dAssign_14/main.cpp:4`. 현재 소스 존재.
- `openGL_3dAssign_15/openGL_sample2.vcxproj`: `openGL_3dAssign_15/main.cpp:5`. 현재 소스 존재.
- `openGL_3dAssign_16/openGL_assign_16.vcxproj`: `openGL_3dAssign_16/main.cpp:5`. 현재 소스 존재.
- `openGL_3dAssign_17/openGL_assign_17.vcxproj`: `openGL_3dAssign_17/main.cpp:4`. 현재 소스 존재.
- `openGL_3dAssign_18/openGL_assign_18.vcxproj`: `openGL_3dAssign_18/main.cpp:4`. 현재 소스 존재.
- `openGL_3dAssign_19/openGL_assign_19.vcxproj`: `openGL_3dAssign_19/main.cpp:4`. 현재 소스 존재.
- `openGL_3dAssign_20/openGL_assign_20.vcxproj`: `openGL_3dAssign_20/main.cpp:5`. 현재 소스 존재.
- `openGL_3dAssign_21/openGL_assign_21.vcxproj`: `openGL_3dAssign_21/main.cpp:5`. 현재 소스 존재.
- `openGL_3dAssign_22/openGL_assign_22.vcxproj`: `openGL_3dAssign_22/main.cpp:5`. 현재 소스 존재.
- `openGL_3dAssign_23/openGL_assign_23.vcxproj`: `openGL_3dAssign_23/main.cpp:5`. 현재 소스 존재.
- `openGL_assign123456/openGL_sample2.vcxproj`: `openGL_assign123456/main.cpp:79`. 현재 소스 존재.
- `openGL_sample0/openGL_sample0.vcxproj`: `openGL_sample0/main.cpp:308`. 작업 전부터 부재. 위 진입점은 과거 색인 정보.
- `standard/standard.vcxproj`: `standard/main.cpp:6`. 현재 소스 존재.
- 독립 예제 `framework/examples/basic.cpp`의 main. 다른 실행 파일과 합치지 않는다.
- 독립 예제 `framework/examples/assets.cpp`의 main. 다른 실행 파일과 합치지 않는다.
- 독립 예제 `framework/examples/motion.cpp`의 main. 다른 실행 파일과 합치지 않는다.

## standard: 등록과 실행을 구분한 흐름

`standard/main.cpp`는 Application과 Console을 생성한다. init·modified-key·mouse·cursor·focus·update·render 핸들러를 저장한다. 저장 시점에는 장면이나 키 처리를 실행하지 않는다.

Application::run → initialize → GLFW 창/현재 컨텍스트·Renderer 초기화 → initHandler → Console::initScene → activateMode 순서다. initialMode가 있으면 해당 모드를 시작하고 전환 입력을 막는다. 없으면 1번으로 시작한다.

1~6: 각 ModeN 파일의 PathMotion 정의 → MotionCommon의 공유 경로·자전·반사 레이어 → 상자 2개. 7: 격자 + 경로 프레임 + 편집 가능한 상자 자식. 8: 강체 방 프레임 + 같은 공간에서 계산한 RoomBody + 상자 편집 자식. 9: 그네 프레임→hinge→seat→rider와 별도 standing·linear marker다.

10~22: PracticeRegistry의 팩토리 → 모드 객체 생성 → initScene. PracticeMode의 가상 호출은 Console의 단일 콜백에서 전달한다. 12/13은 FaceCommon, 16/19는 TankCommon을 공유한다. 파생 클래스/파일을 함수처럼 호출하지 않는다.

매 프레임 PollEvents가 입력 콜백을 호출한다. 1~9의 공통 입력은 선택 인덱스·축·track.rest 또는 물리 상태를 바꾼다. 마우스는 실제 뷰의 ray → 최근접 보이는 삼각형 → 부모 체인 → 의미상 객체 루트 순서다. 추가 실습은 각 모드의 원래 키·마우스·커서·포커스 핸들러에 전달한다.

updateHandler는 모드별 상태를 한 번 진행한다. 기준 자세/모션 합성·경로/물리 반영 뒤 Application이 Scene::updateMotions를 한 번 진행한다. renderHandler는 같은 Scene을 한 뷰 또는 세 뷰에서 그린다. 렌더 패스에서 시간을 진행하지 않는다.

## 다른 실행 프로젝트

실습13~23의 각 main은 자신의 Console과 Application을 생성하고 그 파일에 실제 등록한 콜백만 사용한다. 이들은 standard에서 만든 PracticeMode와 별도 실행 파일이다. 공통 Application 루프의 초기화·업데이트·렌더·종료 계약은 같다.

기초2~6은 PROB에 따라 createConsole 분기가 바뀌고, 7~9도 PROB 분기를 가진다. main 자체는 각 프로젝트에서 하나다. 10·11·12는 별도 main이며, 11은 시작 시 격자 크기를 읽는다. 이 2D 프로젝트들은 GLFW/GLEW를 직접 초기화하고 자신의 Console/PracticeConsole을 만든다. 각 소스의 키·마우스·커서·프레임버퍼 콜백 등록 뒤 update → render → SwapBuffers → PollEvents 순서다. 3D Application의 PollEvents 선행 순서와 합쳐 설명하지 않는다. Core 프로젝트는 셰이더 경로와 initGraphics가 성공한 뒤 루프에 들어가고 shutdownGraphics → Console 해제 → 창 해제 → glfwTerminate로 끝난다. 기초 compatibility 프로젝트는 별도 render 경로를 유지한다.

부재 워밍업1의 여러 PROB main과 워밍업2 wmain은 과거 PROJECTS 정보로만 남는다. 현재 실행 흐름을 재확인했다고 보고하지 않는다. 테스트 프로젝트도 목록에만 남고 실행하지 않았다.

## CPU → GPU와 종료

Scene의 모델·정점·색·UV·법선 → Renderer::upload의 RenderVertex → VBO/EBO, VertexLayout → VAO 입력 해석이다. Scene::worldMatrix/faceWorldMatrix → Renderer::draw → model/view/projection과 조명·색·텍스처 uniform → 정점 셰이더 → 클리핑·래스터화 → 프래그먼트 셰이더 → 깊이·컬링·프레임버퍼 → SwapBuffers로 이어진다.

기본 standard는 `standard/framework/detail/Shaders.h`의 내장 GLSL 330 셰이더를 사용한다. VertexShader.glsl/FragmentShader.glsl은 외부 셰이더 경로를 구성할 때 구분한다. 피킹과 렌더는 같은 faceWorldMatrix를 쓰며 컬링·반사 winding을 맞춘다. GPU 변형이나 투명 픽셀의 완전 일치는 별도 문제다.

정상 종료·콜백 예외에서 Application은 현재 컨텍스트의 GPU 자원과 창·GLFW를 정리한다. 예외는 main에서 출력하고 실패 코드로 반환한다. 컨텍스트를 잃은 경로는 Renderer::abandon을 사용한다. 실제 종료·렌더·입력 성공은 [사용자 확인](../../standard/USER_CHECKS.md)에 맡긴다.
