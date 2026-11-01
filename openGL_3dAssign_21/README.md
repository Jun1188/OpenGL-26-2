# 실습 21: 3차원 공 튕기기

`openGL_assign_21.sln` 또는 `openGL_assign_21.slnx`를 Visual Studio에서 연다. 프로젝트와 출력 파일 이름은 `openGL_assign_21`이다. `framework/` 전체를 함께 보존한다. 별도 SDK 설치나 소스의 절대 경로를 요구하지 않는다.

기준 원문: `../lecture_pdf/homework/지엘3 좌표계 변환 (실습 2122).pdf`, 2쪽 실습 21. 공통 프레임워크 기준 커밋은 `fd79f92`다. 실습 23은 포함하지 않는다.

## 조작

| 입력 | 동작 |
|---|---|
| `z` / `Z` | 카메라와 주목점을 월드 Z 양/음 방향으로 0.35씩 이동 |
| `y` / `Y` | 카메라의 현재 화면 로컬 Y축으로 시계/반시계 자전, 한 번에 4도 |
| `B` | 새 공 생성. 초기 1개, 전체 생성 수 최대 5개 |
| 왼쪽 버튼 + 좌우 드래그 | 공간을 Z축으로 회전. 왼쪽 드래그는 앞에서 보아 반시계, 오른쪽은 시계 |
| `e` / `F` | 바닥 열기/닫기. 개방 즉시 모든 내부 공·상자를 중력 낙하로 전환. F는 호환 별칭 |
| `v` | 방의 Y축 자전 시작/정지. 속도 30도/초 |
| `R` | 공 1개, 상자 3개, Z 기울기와 Y 자전 각도·상태, 바닥, 카메라를 모두 초기화 |
| `H` | 콘솔 도움말 |
| `Q` / `Esc` | 종료 |

대소문자는 `Shift XOR Caps Lock`으로 구분한다. `z/y`는 GLFW 키 반복을 허용한다. `B/e/F/v/R/H/Q/Esc`는 `GLFW_PRESS`에만 반응한다. `B/e/F/v`는 대소문자 모두 같은 명령이다. 포커스를 잃으면 드래그를 해제하고 물리 업데이트를 멈춘다. 포커스를 되찾아도 새 왼쪽 클릭 전에는 드래그하지 않는다.

## 요구사항과 코드

| 요구사항 | 구현 위치 및 처리 |
|---|---|
| 원근/은면 제거/컬링 | `Console::initScene`: 원근 Camera, depthTest=true, 기본 Back 컬링 |
| 앞면 열린 공간, 서로 다른 5면 색, 내부 표시 | `PatternSystem::addRoom`의 앞면 숨김, 각 면 Front 컬링·안쪽 법선, `setFaceVertexColors` |
| sphere 이동과 벽 반사 | `addBall`로 sphere 생성, `onUpdate`에서 elastic=true인 `stepRoomBody` |
| 최대 5개 공 | `ballCount_`로 제한. 이미 낙하한 공도 포함. R로 생성 수 초기화 |
| 바닥 Z축 일렬 상자 3개 | `initScene`의 0.65/0.9/1.2 크기와 Z=-2.3/0/2.3 배치 |
| 공간 Z회전과 바닥 미끄럼 | `onCursor`의 각도 변경 → 월드 중력을 방 로컬로 변환하는 elastic=false 계산 |
| 바닥 개방과 객체 낙하 | `e` 또는 `F` → face 5 숨김 → 모든 미이탈 객체의 현재 월드 위치·속도와 회전을 보존하고 즉시 released=true → 월드 중력 낙하 |
| 방 Y축 자전 | `v`로 roomSpinning_ 전환 → onUpdate에서 Y각도 시간 적분 → Yspin * Ztilt 합성 |
| 카메라 이동과 자전 | `onKey`의 `PatternSystem::moveCamera`와 `yawCamera` |

공의 표면은 `CullMode::None`으로 유지하고, 컬링 요구는 공간 면의 Front 컬링과 상자의 기본 Back 컬링으로 적용한다. 바닥을 닫은 동안 공은 안쪽에서 중력 없이 일정 속력으로 반사한다. 바닥 개방 명령은 위치나 이동 방향에 관계없이 모든 내부 공·상자를 즉시 중력 낙하 상태로 바꾼다. 현재 월드 위치와 회전을 보존하고, 로컬 속도를 방 회전으로 월드 속도로 변환한다. 바닥 개방 중 새로 생성한 공도 낙하 상태로 시작한다. 바닥을 다시 닫아도 이미 낙하한 객체는 복귀하지 않는다.

## 입력 → 상태/변환 → 화면

`main`은 Application과 Console을 생성하고 초기화·수정키·마우스·커서·포커스·업데이트 콜백을 등록한다. `Application::run`이 초기화 후 이벤트를 전달하고 매 프레임 `onUpdate` 다음 기본 렌더를 호출한다. 예외는 프레임워크 자원 정리 후 main에서 원인을 출력한다.

`Console::initScene`은 Scene을 비우고 ID를 다시 받는다. 공과 상자는 `BodyObject`에 EntityId와 RoomBody를 함께 보관한다. 생성할 공은 반지름을 포함한 Bounds3로 기존 미이탈 공·상자와 겹치지 않는 위치를 제한 횟수만 검색한다. 빈 위치가 없으면 상태를 추가하지 않고 콘솔에 알린다.

드래그 dx가 Z 기울기 `roomAngle_`을 바꾼다. `v`가 자전 상태를 전환하고 `onUpdate`가 Y 자전 각도 `roomSpinAngle_`을 초당 30도로 적분한다. `roomWorld()`는 `Yspin * Ztilt` 순서의 순수 회전 행렬이다. 방의 로컬 Z 기울기를 적용한 뒤 월드 Y 자전을 적용한다. 월드 각속도는 `Y축 자전 속도 + Yspin * Z축 드래그 속도`로 합성한다. `stepRoomBody`는 이 행렬로 월드 중력을 로컬로 변환해 상자를 바닥으로 미끄러뜨린다. `syncScene`은 `body.position(roomWorld)`를 root 객체의 월드 위치에 적용한다. 객체에 공간 부모를 추가하지 않아 회전을 중복 적용하지 않는다. 상자의 미이탈 회전은 공간과 같고 이탈 시 회전을 보존한다.

공간 메시는 원래 Bounds3 중심을 기준으로 생성된다. 표시 위치는 `roomWorld * roomCenter`, 표시 회전은 `mat3(roomWorld)`이다. 물리 공간과 보이는 공간의 회전 중심을 같게 유지한다. 렌더러는 Scene의 정점·면 색·법선을 VAO/VBO/EBO로 전달하고, 모델·뷰·투영 변환 후 깊이와 컬링 상태를 적용한다.

## 재사용 API와 설계 한계

- 공통 계약: [APPLICATION_API.md](framework/docs/APPLICATION_API.md)
- 선언: [Patterns.h](framework/include/Patterns.h), [Application.h](framework/include/Application.h), [Graphics.h](framework/include/Graphics.h)
- 구현: [Patterns.inl](framework/detail/Patterns.inl), [Scene.inl](framework/detail/Scene.inl), [Application.inl](framework/detail/Application.inl)
- 실습 코드: [Console.h](Console.h), [Console.cpp](Console.cpp), [main.cpp](main.cpp)

공통 충돌은 방 축에 정렬된 반크기 경계를 사용하는 교육용 모델이다. 바닥이 닫힌 동안 공·상자는 방 벽과만 충돌한다. 바닥 개방 명령 뒤에는 방 벽 충돌을 중단하고 월드 중력 낙하를 계산한다. 객체끼리 충돌하지 않으므로 생성 뒤 경로에서 서로 통과할 수 있다. 구의 모서리 정밀 접촉, 마찰, 강체 토크, 코리올리/원심 가속도, 이탈 뒤 지면 착지는 제공하지 않는다. 공간 앞면은 표시상 열려 있지만 공의 +Z 물리 경계는 유지한다. 급격한 드래그는 정밀 회전 강체 충돌을 재현하지 않는다. 개방 명령의 즉시 낙하 전환은 모든 객체가 떨어지는 조작을 우선하며, 방 각속도에 의한 접선 속도는 추가하지 않는다. Y 자전 중에도 방의 각 면 Front 컬링과 안쪽 법선을 유지한다. 외부에서 바라보는 벽이 사라지는 것은 내부 벽 컬링 정책이다.

프로젝트는 x64 Debug/Release, C++17, /MTd·/MT를 사용한다. Toolchain.props가 설치된 도구 집합을 선택하므로 v145를 고정하지 않는다. Framework.props가 상대 경로의 포함 파일과 실행용 DLL 배치를 담당한다.

## 실행 상태

빌드·실행·테스트·검증기는 실행하지 않았다. 소스와 API 계약만 읽었다. 실제 화면·구동·다른 PC 호환성은 미확인이다. [USER_CHECKS.md](USER_CHECKS.md)의 절차와 예상 결과를 사용자가 직접 확인해야 한다.
