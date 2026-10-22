## 공통 3D 실행 순서

Framework.h → Application.h → Graphics.h/ModelLoader.h/Picking.h → detail inline 구현을 포함한다. src 호환용 cpp는 제거했다.

Application::run → initialize → 실행 파일 기준 상대 projectRoot/runtimeDirectory 해석 → 번들 GLFW DLL → 창·OpenGL 3.3 core 컨텍스트 → Renderer::initialize → 내장/선택 셰이더 컴파일·링크 → VAO/VBO/EBO → AssetLoader → GLFW C 콜백 등록 → initHandler.

PollEvents가 키·포커스·마우스 콜백을 실행한다. dispatch는 C 콜백의 예외를 보관한다. run은 프레임버퍼가 양수일 때 dt를 0~0.05초로 제한하여 updateHandler → renderHandler 또는 renderScene → SwapBuffers를 호출한다. 최소화 상태에서는 WaitEvents를 사용한다. 콜백 등록 순서와 본문 실행 조건을 혼동하지 않는다.

정상 종료/예외 → shutdown → AssetLoader 해제 → 컨텍스트 current → Renderer::destroy(텍스처·버퍼·프로그램) → 창 해제 → GLFW 종료. 사용자 예외는 정리 후 다시 전달한다.

## 실습별 콜백 내부

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

loadObj는 내부 상대경로 → OBJ 텍스트 읽기 → 표준 parseObj → 파싱 실패 시 호환 parseObj → 삼각분할·공면 병합 → Model/Mesh다. loadPrimitive와 loadModel도 이 경로를 호출한다. Assimp는 제거했다. loadTexture는 Windows WIC → RGBA8 → Material → 선택적 GPU 텍스처다. DLL/COM/GPU 드라이버 내부는 분석 범위 밖이다.

## 한계

같은 이름 메서드·오버로드·간접 호출은 AST가 완전히 해석하지 못한다. 콜백 본문은 소스 앵커로 보강했다. GRAPH_HEALTH.md의 미해결 관계를 숨기지 않는다. 구조 관계 수는 실행 호출 횟수가 아니다. 빌드·화면·입력·종료 테스트와 벤치마크는 사용자 확인이 필요하다.

## Motion·레이어 경로 추가 (2026-10-22)

이 절은 새 소스와 AST 산출물을 대조한 정적 설명이다. 빌드·실행 결과가 아니다.

1. main은 Application과 상태를 만든 뒤 setInitHandler/setKeyHandler/setMouseHandler 등으로 콜백을 등록한다. 등록 시 본문을 실행하지 않는다.
2. Application::run 초기화 후 init 콜백에서 Scene::getMotions -> createMotion/createSpin/createOrbit, pushLayer/bindMotion, Scene::bindEntity를 호출한다. 사용자 함수는 createMotion/setMotion/seekMotion의 샘플 평가에서도 실행될 수 있다.
3. 프레임은 PollEvents -> 입력 콜백 -> 사용자 update -> 종료 확인 -> Scene::updateMotions -> MotionSystem::update -> Motion::sample -> 각 MotionStep.transform으로 진행한다.
4. 회전 Step은 TransformSystem::rotation을, 경로 Step은 시간의 clamp/순환/왕복 -> interpolation -> path -> TransformSystem::translation을 사용한다. 콜백 이름의 연결은 런타임 std::function을 통해 이루어지며 일반 AST 간선만으로 특정 람다를 확정하지 않는다.
5. Renderer::draw와 pick은 Scene::faceWorldMatrix -> Scene::worldMatrix -> MotionSystem::layerMatrices를 공유한다. worldMatrix는 가까운 유효 객체 바인딩을 찾아 P*A*L*D를 합성한다. root의 변환은 바깥쪽에서 한 번만 적용된다.
6. 선은 Renderer::draw -> Scene::lineWorldMatrix를 사용한다. owner가 있으면 worldMatrix, 없으면 root의 전역 행렬만 받는다.
7. CPU의 최종 model은 기존 VAO/VBO/EBO의 메시와 함께 uModel, projection*view*model의 uMVP, 역전치 uNormalMatrix로 전달된다. 반사는 메시 정점을 다시 업로드하지 않는다. Renderer의 컬링 해제와 pick의 양면 검사를 유지한다.
8. run 종료/예외는 기존 shutdown 순서로 자산·GPU·창·GLFW를 정리한다. Scene은 CPU Motion 및 레이어 데이터를 소유한다.

새 예제의 main은 기존 실습 13~19의 main과 별도다. examples/motion.cpp 하나만 프로젝트에 등록한다. 새 API를 사용하는 실습 이식은 하지 않았다. Scene::worldMatrix는 객체 깊이와 레이어 깊이만큼 순회하고 임시 벡터를 만든다. Motion 콜백은 시간 갱신에서 한 번 평가하며 행렬 조회에서 재호출하지 않는다. 성능·화면·입력은 사용자가 확인한다.

## OBJ 변경 범위와 현재 진입점

현재 실행 프로젝트와 OBJ 로드의 정확한 정의·호출 위치는 [OBJ 구조 요약](../../docs/obj-loader/ARCHITECTURE.md)을 따른다. 과거 기록의 실습 13·14와 워밍업 프로젝트는 현재 작업 트리에서 확인되지 않는다.
