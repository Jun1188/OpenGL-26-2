# 최종 실행 구조

graphify 코드 AST: 3703 nodes / 7125 edges. [HTML](../../graphify-out/practice-24-26-27/graph.html), [JSON](../../graphify-out/practice-24-26-27/graph.json), [소스 범위](../../graphify-out/practice-24-26-27/SOURCE_MANIFEST.json). SDK·아카이브·중복 프레임워크 배포본은 추출에서 제외한다. 새 프로젝트의 실제 컴파일 입력 목록은 PROJECTS.json에 각 폴더의 로컬 framework와 함께 기록한다.

## 공통 파이프라인

각 독립 main은 Application과 해당 폴더의 Console을 생성하고 초기화·입력·갱신 콜백을 등록한다. 콜백 등록은 실제 프레임 실행이 아니다. Application::run이 초기화 후 이벤트 루프에서 입력과 갱신을 호출한다. 사용자 update 다음 Scene::updateMotions를 진행하고 렌더한다. 종료에서는 컨텍스트가 살아 있는 동안 GPU 자원을 해제한다.

24: initScene → 기존 L자 벨트/상자 + addTruck → resetState. 숫자 입력 → loadParcel → 부모/적재 슬롯 변경. onUpdate → 트럭 상태·경계 처리 → 미적재 물체 경로 갱신 → 후보 카메라 5개 갱신 → 선택 카메라 적용. 상자 번호와 EntityId를 분리한다.

26: initScene → 육면체/사각뿔/광원 표시/단위 궤도. 입력은 형상·조명·자전/공전 방향·반지름 상태를 바꾼다. onUpdate → 각도 → apply → 두 형상 자세·가시성·광원 위치·궤도 크기·관찰 거리. 같은 radius가 광원과 궤도에 사용된다.

27: initScene → 구 3개와 광원 표시. 입력은 광원 색 인덱스와 공전 방향을 바꾼다. onUpdate → 각도 → apply → 점광원 위치·색 및 표시 객체 변환·색.

Standard: main → Console → defaultRecipe → Assembly::execute → resolveCommand/dispatch → applyPoses. 일반 객체·지역 레이어 뒤 사용자/물리 제어기를 갱신하고 syncSceneTools에서 광원·부착 카메라를 얻는다. material 명령은 기존 재질을 복사해 지정 필드만 바꾼다. 새로운 실습 모드를 컴파일하지 않는다. MovingBeacon.h는 등록 방법을 설명한 C++ 참조다.

Renderer는 Scene의 CPU 정점/색/법선/UV를 VAO/VBO/EBO로 올린다. 각 draw에서 faceWorldMatrix, MVP, 역전치 법선 행렬, 재질 계수·패스 시점·광원을 전송한다. 정점 셰이더는 월드 위치/법선을 전달하고 프래그먼트 셰이더가 Phong 조명을 계산한다. unlit 표면과 선·윤곽선은 조명 계산을 분리한다.

## 그래프의 해석 경계

AST calls/include 관계는 정적 소스 추출이다. std::function 콜백의 실제 실행 순서, 사용자 조건과 런타임 바인딩은 해당 소스와 대조해야 한다. 독립 프로젝트의 Console/main 이름이 같아도 서로 호출하는 것이 아니다. generator는 프로젝트를 넘나드는 calls를 AMBIGUOUS로 표시한다. 실행·화면·컴파일 증거가 아니다.

## 저장소의 독립 진입점 목록

각 행은 서로 다른 실행 프로젝트다. 부재 파일은 이전 sparse/historical 기록이며 이번 작업에서 삭제한 것으로 보지 않는다.

- `openGL_2dAssign11/openGL_sample2.vcxproj`: `openGL_2dAssign11/main.cpp:109` main
- `openGL_2dAssign12/openGL_sample2.vcxproj`: `openGL_2dAssign12/main.cpp:59` main
- `openGL_2dAssign_10/openGL_sample2.vcxproj`: `openGL_2dAssign_10/main.cpp:86` main
- `openGL_2dAssign_789/openGL_sample2.vcxproj`: `openGL_2dAssign_789/main.cpp:103` main (조건부 분기 확인)
- `openGL_3dAssign_13/openGL_sample2.vcxproj`: `openGL_3dAssign_13/main.cpp:4` main
- `openGL_3dAssign_14/openGL_sample2.vcxproj`: `openGL_3dAssign_14/main.cpp:4` main
- `openGL_3dAssign_15/openGL_sample2.vcxproj`: `openGL_3dAssign_15/main.cpp:5` main
- `openGL_3dAssign_16/openGL_assign_16.vcxproj`: `openGL_3dAssign_16/main.cpp:5` main
- `openGL_3dAssign_17/openGL_assign_17.vcxproj`: `openGL_3dAssign_17/main.cpp:4` main
- `openGL_3dAssign_18/openGL_assign_18.vcxproj`: `openGL_3dAssign_18/main.cpp:4` main
- `openGL_3dAssign_19/openGL_assign_19.vcxproj`: `openGL_3dAssign_19/main.cpp:4` main
- `openGL_3dAssign_20/openGL_assign_20.vcxproj`: `openGL_3dAssign_20/main.cpp:5` main
- `openGL_3dAssign_21/openGL_assign_21.vcxproj`: `openGL_3dAssign_21/main.cpp:5` main
- `openGL_3dAssign_22/openGL_assign_22.vcxproj`: `openGL_3dAssign_22/main.cpp:5` main
- `openGL_3dAssign_23/openGL_assign_23.vcxproj`: `openGL_3dAssign_23/main.cpp:5` main
- `openGL_3dAssign_24/openGL_assign_24.vcxproj`: `openGL_3dAssign_24/main.cpp:5` main
- `openGL_3dAssign_26/openGL_assign_26.vcxproj`: `openGL_3dAssign_26/main.cpp:5` main
- `openGL_3dAssign_27/openGL_assign_27.vcxproj`: `openGL_3dAssign_27/main.cpp:5` main
- `openGL_assign123456/openGL_sample2.vcxproj`: `openGL_assign123456/main.cpp:79` main (조건부 분기 확인)
- `standard/standard.vcxproj`: `standard/main.cpp:6` main
- `framework/examples/basic.cpp`: `framework/examples/basic.cpp:?` main
- `framework/examples/assets.cpp`: `framework/examples/assets.cpp:?` main
- `framework/examples/motion.cpp`: `framework/examples/motion.cpp:?` main
