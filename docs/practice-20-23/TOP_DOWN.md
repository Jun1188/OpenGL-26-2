# 최신 구조: 재개 수정

소스 ed32c20 기준: 프레임워크 26파일/1263노드/1941간선, 저장소 62파일/1907노드/3010간선. 실습23 main은 openGL_3dAssign_23/main.cpp이며 init/key/update → addWalkingRig/stepCharacter/poseWalkingRig/openFace → Application 렌더링 경로다. 새 컨텍스트 보호·진단·20/21 흐름은 [RESUME.md](RESUME.md)를 따른다. 아래는 최초 구현 당시의 상세 흐름으로 보존한다.

# 현재 소스의 탑다운 구조

분석 기준 소스 커밋은 `cf2567d`다. graphify의 코드 AST·소스 정의/콜백 등록 앵커를 사용하고 실제 main/Application/Renderer/Scene/Patterns와 대조했다. 프레임워크 26파일·1,246노드·1,913간선, 현재 저장소 코드 59파일·1,864노드·2,932간선이다. 실행·컴파일·GPU 추적 결과가 아니다.

## 독립 진입점

| 실행 프로젝트/예제 | main 위치 | 이후 흐름 |
|---|---|---|
| 실습15 | openGL_3dAssign_15/main.cpp:4 | Console의 initScene/onKey/onUpdate 등록 → Application::run |
| 실습16 | openGL_3dAssign_16/main.cpp:4 | Console의 initScene/onKey/onUpdate 등록 → Application::run |
| 실습17 | openGL_3dAssign_17/main.cpp:2 | init/key/mods/update/focus 콜백 → Application::run |
| 실습18 | openGL_3dAssign_18/main.cpp:2 | init/key/mods/update/focus 콜백 → Application::run |
| 실습19 | openGL_3dAssign_19/main.cpp:2 | init/key/mods/update/focus 콜백 → Application::run |
| 실습20 | openGL_3dAssign_20/main.cpp:5 | Conveyor Console, init/key/mods/focus/update → Application::run |
| 실습21 | openGL_3dAssign_21/main.cpp:5 | Room Console, init/key/mods/mouse/cursor/focus/update → Application::run |
| 실습22 | openGL_3dAssign_22/main.cpp:5 | Tank Console, init/key/mods/update/focus/render → Application::run |
| standard | standard/main.cpp:3 | Motion/Grid/Room Console, init/key/mouse/update/render → Application::run |
| openGL_sample0 | openGL_sample0/main.cpp:258 | 자체 GLFW/GLEW 초기화 → TXT 모델·버퍼·셰이더 → 자체 루프. Application 경로와 별개 |
| framework/examples/basic.cpp | main | init에서 cuboid·transform/outline, mouse에서 pick·면 SRT → Application::run |
| framework/examples/assets.cpp | main(argc,argv) | 모드/상대경로 인자 → init에서 AssetLoader::loadObj → addModel → Application::run |
| framework/examples/motion.cpp | main | Motion·반사·레이어 콜백 → Application::run |

위 10개 vcxproj와 3개 독립 예제는 서로 다른 실행 프로그램이다. examples는 main 소스만 제공하며 별도 vcxproj가 아니다. 현재 디스크의 main에는 과거 PROB 분기 프로젝트가 없으므로 한 흐름으로 합치지 않았다. sparse-checkout으로 없는 옛 실습은 이 현재 디스크 분석에 포함하지 않았다. 전체 파일·진입점 목록은 [PROJECTS.json](../../graphify-out/PROJECTS.json)에 있다.

## Application이 콜백을 실행하는 시점

```text
각 main
  Application/Console 생성 (아직 OpenGL 초기화 안 함)
  set...Handler: std::function에 람다 저장 (등록 시 실행하지 않음)
  Application::run(config)
    Diagnostics 설정, 한 Application 실행 제한, 콘솔 UTF-8
    initialize
      exe 기준 projectRoot/runtimeDirectory 확인
      GLFW + 인접 VCRUNTIME140 DLL 로딩, GLFW 오류 콜백
      glfwInit → 3.3 core window → current context
      Renderer::initialize → GL 함수 주소 → 셰이더 compile/link → VAO/VBO/EBO
      AssetLoader 생성, GLFW 입력 콜백 설치
    initHandler → Console::initScene (CPU Scene 구성)
    반복
      PollEvents → native callback → dispatch → 사용자 key/mouse/cursor/focus
      창 최소화: WaitEvents, 다음 dt 기준 시각 재설정
      dt = clamp(실제 경과 시간, 0, .05)
      updateHandler → 응용 상태·충돌·최종 변환
      Scene::updateMotions(dt) → 레이어용 모션 샘플 (한 번)
      renderHandler가 있으면 실행, 없으면 renderScene
      SwapBuffers
    shutdown: AssetLoader 해제 → current context 유지하며 GPU 자원 삭제
      → window 파괴 → glfwTerminate
```

정상 종료와 예외 모두 컨텍스트를 없애기 전에 Renderer 자원을 해제한다. native 입력 콜백 예외는 dispatch가 저장하고 close를 요청하며 루프에서 다시 던진다. 표준 예외는 API/단계 진단 후 재전파한다. 20~22 main은 예외를 콘솔에 출력하고 실패 코드로 종료한다.

## 실습별 입력 → 상태 → 출력

### 20

Console::initScene → DistancePath::setPoints로 열린 ㄱ자 12거리 → 두 볼록 평면 벨트/바닥 → 줄무늬30·상자5·번호 선분. 초기에는 정지한다. b는 AnimationClock::playing, +/-는 zoomCamera, p/P는 projection flag, 좌/우는 orbitCamera, r은 카메라·시간·입력·배치를 복원한다.

onUpdate → clock.advance 한 번 → seconds*1.25에 각 객체의 거리 offset 추가 → PatternSystem::follow → DistancePath::sample 위치·접선/Y회전 → Scene::setTransform. 번호는 상자 자식 joint/line으로 함께 변환된다. 끝→시작은 순간 순환이고 코너 회전은 즉시 90도다. 카메라 공전은 정지한 벨트와 독립이다.

### 21

initScene → addRoom(앞면 시각 개방, Front cull) → 크기 다른 상자3 → addBall(초기1, 최대5). 물리 bounds와 표시 room 중심이 다르지 않도록 roomTransform.basis=R*roomCenter로 둔다. 공/상자는 룸의 표시 자식으로 중복 회전시키지 않고 각자 최종 월드 변환을 받는다.

마우스 dx → Z각도·누적 각속도 → onUpdate의 stepRoomBody. 방 안에서는 localPosition/localVelocity, 열린 바닥에서 이탈하면 worldPosition/worldVelocity 상태로 전환한다. 상자는 월드 중력을 로컬로 변환해 바닥을 따라 미끄러지고 공은 반크기 경계에서 탄성 반사한다. syncScene은 body.position(roomWorld), 상자의 방 회전/이탈 회전을 Scene에 반영한다. 시각적으로 열린 +Z 앞면의 물리 반사벽은 유지한다. F는 PDF에 키가 없어 선택한 바닥 개방 키다.

### 22

reset → 실습19의 루트/중앙/상부/포신/깃대 계층을 addBox/addJoint로 구성. onUpdate는 방향키 XZ 이동과 부품 모드·카메라를 한 번 갱신한다. apply는 중앙 높이/자전, 하부 높이, 상부 교환, 포신 몸체 중심 피벗과 깃대 밑점 피벗을 반영한다. 색상 O/o도 처리한다.

onRender → 현재 framebuffer 크기 → threeViews → 같은 Scene을 renderViews. 원근은 왼쪽, XZ 직각은 오른쪽 위(up=-Z), XY 직각은 오른쪽 아래다. 탱크 이동을 세 번 갱신하지 않는다. 프레임버퍼가 3x2 미만이면 렌더만 건너뛴다.

### standard

기존 Motion/반사/레이어 예제는 0으로 돌아간다. 7은 GridLayout.cellCenter/relativeSize로 바닥을 구성하고 DistancePath/follow로 번호 상자5를 이동한다. 8은 addRoom/RoomBody를 재사용한다. V는 renderScene/renderViews 전환, D는 진단 추적, F는 실내 바닥이다. 분할 화면의 mouse picking은 해당 pass camera와 위쪽 원점 viewport를 사용한다.

## Scene·변환·GPU 데이터

`Model.h`의 Transform3D는 basis/localBasis/rotation/scale을 보관한다. `detail::transformMatrix`는 `T(basis)*R*S*T(-localBasis)`다. Scene::worldMatrix는 부모 체인을 읽고 유효한 모션 바인딩 위치에서 prefix/suffix를 나누어 `motion.parent*prefix*motion.local*suffix`를 만든다. faceWorldMatrix는 여기에 독립 face SRT 및 localAnimationMatrix를 합성한다. ID나 affine/finite 조건이 실패하면 해당 API 진단을 출력한다.

Renderer::renderPass → Scene 형상 revision/texture mode가 바뀌면 upload → CPU Mesh의 정점 위치/색/UV/법선을 GpuVertex로 배열화 → VBO, 삼각형/외곽선 인덱스를 EBO로 업로드한다. VAO attribute 0/1/2/3은 위치/색/UV/법선이며 stride는 sizeof(GpuVertex), offset은 offsetof를 사용한다. 그룹 노드는 형상을 업로드하지 않는다. 이동·회전은 revision을 바꾸지 않고 draw에서 행렬로 적용한다.

Renderer::draw는 faceWorldMatrix로 model을 얻고 `MVP=projection*view*model`, normalMatrix는 가능한 경우 model의 mat3 역전치를 uniform으로 전달한다. DrawElements의 EBO offset은 firstIndex*sizeof(GLuint)다. 정점 셰이더는 `uMVP*vec4(vPos,1)`, 프래그먼트 셰이더는 색*tint, 선택 텍스처, 선택 방향광을 계산한다. 기본 shader는 detail/Shaders.h의 GLSL 330 core 내장 문자열이다. 외부 shaderDirectory를 지정한 경우 상대경로 두 GLSL을 읽는다.

실제 GPU 순서는 정점 입력 → 정점 셰이더 → 프리미티브 조립/클리핑/원근 나눗셈 → 뷰포트/래스터화 → 프래그먼트 셰이더 → 깊이/색 버퍼 → swap이다. CPU 코드가 GPU 내부의 모든 결과를 직접 검사하는 것은 아니다.

renderViews는 프레임 전체를 한 번 지운 후 패스들을 순서대로 실행한다. 패스별 viewport/scissor/clear와 aspect를 독립 적용한다. draw는 Mesh.cull 우선, RenderOptions.cull 기본값을 사용한다. 음수 determinant의 front winding을 보정한다. 내부 벽은 Front cull+안쪽 법선, 일반 물체는 None/Back이다.

## 그래프 해석 제한

EXTRACTED는 AST/정의/등록 소스에서 얻은 관계다. `registered_callback_body_calls`는 main이 등록한 람다 본문의 호출이며 등록 즉시 실행된다는 뜻이 아니다. 여러 독립 프로젝트의 Console 동명 메서드가 연결되는 간선은 AMBIGUOUS 처리한다. 정적 분석은 동적 std::function, GL 함수 포인터와 사용자 입력별 정확한 호출 순서를 전부 복원하지 못한다. 데이터 타입 외부 노드 등 dangling 관계와 동일 노드쌍 간선 축약도 있으므로 GRAPH_HEALTH/EXTRACTION 원본을 함께 본다. 실행 결과와 성능을 검증한 그래프로 표현하지 않는다.

사용자 확인: 각 프로젝트 Debug/Release x64 빌드·F5, 리셋/포커스/DPI/최소화, 20 번호와 경로, 21 벽 반사·낙하, 22 3뷰 동기화, 다른 PC의 DLL·도구 집합을 확인한다. 실습23은 구현하지 않았다.
