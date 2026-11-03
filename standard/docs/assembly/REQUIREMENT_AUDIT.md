[후속 구현: 1~22 컨테이너·라이브 포트·핫로드](FEATURE_CONTAINERS.md) · [편집/디버깅 가이드 확장](EDIT_DEBUG_GUIDE.md)

# 실습 표현력과 반복 소스 검토

2026-11-03. 이 문서는 정적 소스 대조 기록이다. 컴파일·테스트·검증기·화면 실행 기록이 아니다. 오류 0건이나 모든 실습의 실행 재현 완료를 주장하지 않는다.

## 초기 원시 기능 평가: 컨테이너 도입 전

모드0은 원시 부품·계층·변환·면·입력·기존 물리 API를 조립할 수 있다. 여기에 guide가 안내하는 C++ Motion/Custom 함수를 추가하면 실습의 조건 분기와 상태 전이도 작성할 수 있다. **콘솔 기본 명령만으로 모든 실습의 모든 키·난수·전이까지 자동 복제하는 것은 현재 지원하지 않는다.** 사용자의 직접 코드 편집 선호에 따라 이를 별도 언어로 확장하지 않았다.

이하 표는 컨테이너 도입 전의 원시 기능 평가다. 후속 feature add/port는 standard1~22의 기존 상태/입력을 재사용한다. 원본2D 이식과 컨테이너 사이 충돌 병합은 별도 범위다.

명령 지원, C++ 확장 경로, 실제 실행 확인을 구분한다. C++ 확장 경로가 있다는 사실은 해당 실습 전용 기능을 이미 이식했다는 뜻이 아니다.

## 요구 → 공통 기능 → 남은 직접 편집

| 요구/모드 | 현재 조합 | 정확한 실습 동작을 더 맞출 위치 |
|---|---|---|
| 객체·형상 지정 | add/part box·pyramid·sphere·cone·tetra, triangle/triangle-add, Assembly::model | Model/AssetLoader로 임의 메시/UV/재질 구성. guide geometry |
| 객체-면-좌표 관절 | mount의 면·삼각형·barycentric 좌표, anchor의 원본 메시 좌표 | 표면별 축/roll 규칙은 applyMounts. guide mount |
| 노멀·방향·컬링 | normal auto/flip/XYZ, orient +Z/up, winding flip, cull inherit/none/back/front | per-vertex 노멀·UV·셰이더는 Scene API. guide normal/geometry |
| 계층·변환 중심 | parent/group/part/joint, srt/pivot/center, warp | 정적 비균일/반사 그룹과 동적 강체 관절을 분리. guide hierarchy/warp |
| 기능별 키·마우스·커서 | Effect::Input의 key/mouse/cursor/focus, 기본 bind/drag | C++ 입력 훅이 입력을 소비하며 별도 기능으로 조립. guide input |
| 2D7/8 도형 생성·크기·윤곽 | triangle/triangle-add/line, srt/face-srt, outline, 직각 카메라 | 클릭 생성·정점 조합 정책을 Input 훅에 옮김. 원본 Practice7Console::onMouseButton, Practice8Console::input 참고 |
| 2D9 반사·지그재그·나선 | path/linear/spin, 임의 MotionStep과 Custom | 경계 충돌/모드 상태는 openGL_2dAssign_789의 advanceLinear/advanceSpiral 참고 |
| 2D10 보드 조각 드래그·결합 | 평면 메시·계층·Input 마우스/커서, screenOnPlane | 겹침/정렬/합류 조건은 원본 PracticeConsole의 드래그/키 처리에서 C++로 옮김. 기본 drag 명령은 회전용 |
| 2D11 격자·충돌 효과 | line/메시 격자, Custom 시간 동작, Input 키 | 원본 Grid::moveByStep/stepEffect/advanceEffects의 셀·효과 상태 이식 필요 |
| 2D12 적중 상태 | 그룹·면·Custom, Input의 적중 키 | 원본 DiagramStore::advance/checkHit의 Moving/HitAnimated/Fixed 전이 이식 필요 |
| 1~6 경로·반사 | path 1~6, spin/orbit, warp 반사 그룹 | 임의 경로/보간/시간 효과는 MotionStep. guide motion |
| 7 격자·번호·꺾은선 | add grid, line/digit, path/linear | DistancePath·snap·snakeCells의 세부 배치/방향은 Custom. guide objects/custom |
| 8 방·슬라이딩·이탈 | add room, box physics, body gravity/velocity/bounce, release, visible floor | RoomBody는 강체 방과 축 정렬 반크기 모델. 정밀 강체 물리 아님 |
| 9 그네·사람·보간 | add swing/humanoid, build person, joint/pendulum/linear | 수작업 사람과 충돌 walker의 어댑터는 CODE_GUIDE |
| 실습13 면 표시·난수 선택 | visible <객체> all off 뒤 원하는 면 on, facecolor | 동일 난수 분포·키 전이: guide practice13의 상태/선택 함수를 customCommand로 이동 |
| 실습14 형상·자전·깊이 | add box/pyramid + show, spin/hold bind, render depth/wire | 같은 SRT를 유지한 setModel 교체: guide practice14 |
| 실습15 복합 면 동작 | face/face-track, face-srt, 객체 spin, enable/play/seek | 같은 면의 여러 행렬은 **하나의 FaceTrack.motion**에 조립. guide practice15 |
| 실습16 순차 펼침·교환 | 임의 축·중심 face-track turn + 지연, pingpong | 순환 순서·사각뿔 각도 atan2·입력별 역재생 상태: guide practice16 |
| 실습17 교환·우회·크기 | 그룹별 linear/경로/자전·공전, warp | 서로의 현재 위치로 교환, 사인 배율, 충돌 회피 조건: guide practice17 → Custom |
| 실습18 태양·행성·달 | add planets 또는 그룹/구/공전 계층, 피벗·기울인 그룹 | 기본 planets는 간단한 예. 정확한 궤도/달 평면/반지름 상태: guide practice18 |
| 실습19 탱크 | add tank 기존 공유 부품 또는 build tank 원시 부품, bind/관절 | 키 재입력 정지·포탑 상호 제약·추적 카메라: guide practice19 |
| 실습20 컨베이어 | add conveyor, belt, line/digit, 카메라 | 실제 거리 간격·코너 반경·택배 자세: guide practice20 및 conveyorCornerPose |
| 실습21 회전 방·공 | sphere physics, body bounce, release, drag | 다중 공 생성 제한·정확한 입력 상태는 guide practice21. 공-공 충돌은 별도 구현 |
| 실습22 탱크3뷰 | 기존 탱크 조립 + view3 | 동일 상태를 3회 읽는다. guide practice22 |
| 실습23 로봇·점프·벽 | walker/walk/jump/obstacle/wall/visible | 난수 장애물 배치·카메라 선택 벽·열림 전이: guide practice23 |
| 부록21 카메라 충돌 | camera 명령 + C++ Custom에서 Application/Collision API 접근 | Mode21CameraCollision.cpp의 onUpdate와 충돌 함수. 기본 camera move에는 충돌을 자동 부착하지 않음 |
| 부록22 내장 메시 | triangle/triangle-add/vertex + Assembly::model | UV별 코너/과제 범위 제약은 Mode22EmbeddedMesh.cpp의 validateEmbedded를 참고 |

2D 원본 함수는 저장소의 해당 실습 폴더에서 확인한 참고 위치다. standard의 빌드/실행 의존성으로 연결하지 않았다. 도형 표현과 C++ 훅은 제공하지만 2D 실습 전용 전이가 이미 복제된 것은 아니다.

## 반복 검토와 수정 근거

### 1차: 표현력과 입력 경로

기존 구현을 Scene·Motion·Pattern API와 실습13~23의 입력/갱신 함수에 대조했다. 면에 붙이는 관절, 노멀 지정, 비균일/반사 그룹, 임의 축/중심의 면 이동·축소·지연이 빠져 있었다. AssemblySurface.cpp와 AssemblyTracks.cpp에 공통 구현을 추가했다. Mode0Assembly의 cursor 전달도 연결하여 오른쪽 드래그로 객체를 돌릴 수 있게 했다.

### 2차: 충돌·소유권·복원 경로

단일 메시의 별칭 두 개가 같은 면을 제어하는 경우, 물리 메시/walls의 별도 변형, 부모 순환, 실패한 load의 부분 장면을 대조했다. faceEffect/mesh ID 기반 검사를 공유하고 command의 Scene/State/카메라/렌더 설정 복원을 유지했다. 보행 리그 자신의 부품과 방 walls를 장애물로 등록하는 것도 거부한다. 초기 보행 방향은 루트 Y회전에서 읽는다.

CRLF로 편집한 레시피는 줄 끝 CR을 제거한 뒤 자동 저장 블록 표식을 비교한다. 갱신 오류에서는 hold와 drag를 모두 해제한다.

warp의 XYZ 배율을 기존 균일 rest 저장 형식으로 저장하면 축 정보가 사라지는 경로가 있었다. rest3 저장/불러오기를 추가했다. 면 관절은 Scene 깊이 순서로 갱신하여 부모 관절을 먼저 계산한다. normal, winding, 부착 법선을 서로 다른 의미로 취급했다.

### 3차: 수치·시간·실습 대조

face-track의 지연/한번/반복/왕복, remove/enable 복원, keep 피벗, 부모축 자전+공전, physics 이탈 후 자세를 소스 식으로 추적했다. face-track 시간 하한과 양수 비특이 배율을 적용했다. mount의 변 방향은 정규화한 뒤 외적하여 크기에 따른 불필요한 거절을 줄였다. 구의 물리 반크기는 회전한 정점 AABB 대신 원래 반지름을 사용한다.

경로·방 물리·보행의 시간은 update에서 한 번만 진행한다. Custom은 기준 SRT·현재 시각·유효 dt를 받는다. 렌더에는 적분을 넣지 않는다. graphify AST는 구조 산출에만 사용했으며 컴파일 오류나 런타임 오류를 검증하는 도구로 사용하지 않았다.

### 4차: 입력 조립과 콜백 수명

2D7~12의 클릭 생성·드래그·적중·격자 효과도 대조했다. 고정된 키 문자열 바인딩만으로는 입력 의미를 붙이기 어려워 Effect::Input을 추가했다. key/mouse/cursor/focus를 기능 생성 함수에서 붙이고, 이벤트 목록 복사와 토폴로지 번호로 callback의 clear/load/remove 이후 이전 목록 실행을 중단한다. RELEASE/focus 해제는 먼저 처리한다. 사용자 shared 상태의 원자적 복원은 지원 범위 밖으로 명시했다.

## 실행 전 남은 조건

- [사용자 확인 절차](USER_CHECKS.md)를 실제로 수행해야 한다. 특히 조립 레시피 로드, 면 관절 추적, 반사 피킹, 저장 재구성, 기존7/16/19 회귀를 확인한다.
- 임의 C++ 콜백의 올바름을 API가 증명하지 않는다. callback은 유효한 Scene 상태를 반환해야 한다. 외부 IO/공유 상태 변경은 명령 복원 범위 밖이다.
- 물리는 교육용 강체 방·AABB/반지름 모델이며 면의 그래픽 개방이 일반 메시 충돌을 자동 생성하지 않는다. side wall 시각 효과와 물리 경계는 별도다. floor5의 표시만 기본 이탈 조건에 연결돼 있다.
- 임의 셰이더 효과, 투명 픽셀, 텍스처 로딩, 정밀 차량·구-구 충돌은 직접 C++/셰이더 편집 범위다. 기존 프레임워크 API는 이용할 수 있지만 명령만으로 이미 완성된 기능으로 표시하지 않는다.

## 후속 구현 결과: 원본 동작 보존 컨테이너

2026-11-03 후속 요청에서 standard1~22 전체를 대상으로 확정했다. feature add는 각 원본 Console/PracticeMode를 독립 CPU 상태로 보관하여 위 표의 난수/토글/교환/역재생/보행 동작을 다시 구현하지 않고 재사용한다. feature port는 원본 엔티티 하위 구조를 라이브 출력 포트로 꺼내며, snapshot은 독립 형상이다. 이 추가는 기본 primitive 명령만으로 모든 알고리즘을 복제했다는 뜻이 아니다.

가이드는 현재 코드 줄/주변 문맥/변경 예/단위/디버깅 순서/예상 반응을 제공한다. diagnose는 실제 등록 필드/행렬/기능 소유자를 출력한다. C++26 표준 리플렉션은 컴파일 타임 기능으로 확인하여 C++17 메타데이터 모듈을 채택했다. 빌드/테스트/앱 실행으로 오류0건을 입증한 것은 아니다.
