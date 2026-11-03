# guide로 편집하고 디버깅하기

2026-11-03. 빌드·테스트·앱 실행은 사용자가 수행한다. guide는 소스를 읽는 편집 도우미다. 임의의 C++를 실행 중 재컴파일하거나 알고리즘 오류를 자동 증명하는 기능은 아니다.

## 출력에서 확인할 것

`guide tank`, `guide mount`, `guide feature`, `guide practice17`, `guide mode21`을 입력한다. 등록된 가이드55개와 별칭을 지원한다. 객체 이름을 주면 그 객체의 생성 기능/원본 실습으로 연결한다. 이름 충돌은 `guide feature:tank`로 구분한다.

1. 현재 소스의 파일 경로와 표식 일치 줄을 출력한다. 앞뒤 코드와 줄 번호도 함께 표시한다.
2. 여러 줄이 일치하면 최대4곳과 총 일치 수를 표시한다. 함수/부모 문맥을 비교한다.
3. 작성 기준의 before/after 변경 예와 이유·단위·제약을 출력한다.
4. 관찰할 변수, 중단점 위치, 디버깅-수정 순서, 예상 반응을 출력한다.
5. 이 문서 및 기능별 문서를 연결한다.

before/after 예시는 현재 파일을 자동 복사한 패치가 아니다. 사용자가 값을 고쳤다면 위쪽의 **현재 소스 출력**이 우선한다. 표식이 사라지면 줄 번호를 추정하지 않는다. 파일이 없는 배포본도 확인 불가로 표시한다. 새 기능을 만들면 GuideRecipes.cpp에 실제 source marker와 변경 예를 추가한다. GuideRecipes.h의 스키마는 목적·변경 예·제약·디버깅·예상 결과와 편집 위치를 분리한다.

cpp-effect/input 가이드는 AssemblyExtensions.cpp의 customCommand에서 USER_COMMAND_BEGIN 아래에 넣을 완전한 분기와 필요한 include를 안내한다. 실행 중단점 파일은 편집 지점과 구분한다. humanoid/swing/walker/grid는 생성 호출부의 실제 설정 줄을 표시한다.

## 탱크 포신 길이를 바꾸는 예

```text
guide tank
```

출력된 AssemblyExamples.cpp의 barrel/muzzle 줄을 찾는다. 길이2→3이면 중심1→1.5, 총구2→3을 함께 바꾼다. 길이만 바꾸면 관절 시작점이 뒤로 밀린다. 포탑 연결 위치까지 바꾸려면 turret/gun 관절 위치를 별도로 편집한다.

C++ 수정은 사용자가 다시 빌드한 뒤 `build tank 새이름`으로 재구성한다. .assembly 파일에서 `part box` 숫자를 고친 경우에는 Ctrl+F5로 다시 읽는다. 같은 이름으로 build를 반복하면 기존 객체를 덮어쓰지 않고 중복 이름을 거절한다.

## 입력-상태-출력에서 첫 차이 찾기

```text
pause on
inspect hero
diagnose hero
surface hero.pelvis.torso.body 0
guide pivot
```

- 입력: 키/버튼의 PRESS·REPEAT·RELEASE, 수정키, 포커스, window/framebuffer 좌표가 맞는지 확인한다.
- 기준값: rest.basis/rotation/scale/localBasis가 바뀌는지 확인한다.
- 소유권: 같은 target의 위치 소유자가 둘인지, 원본 실습과 조립 외곽 SRT 중 어느 쪽을 고치는지 확인한다.
- 합성: parent→rest→local, native face frame→추가 면 모션의 곱 순서를 확인한다.
- 출력: world/faceWorld determinant, 면 visible/cull, normal, 카메라·뷰포트·DPI를 확인한다.

Visual Studio 디버거에서는 guide가 출력한 입력/갱신 줄에 중단점을 둔다. Call Stack에서 실제 호출 경로를 보고 Locals/Watch에 위 변수와 행렬을 등록한다. 첫 프레임과 다음 프레임을 비교한다. 다음 프레임에서 편집값이 사라지면 누적 갱신 함수가 기준값을 덮어쓰는지 조사한다. 한 번에 한 값만 바꾸고 같은 입력/시각에서 결과를 비교한다.

## 증상별 수정 방향

| 증상 | 볼 값 | 수정 방향 |
|---|---|---|
| 부위만 선택됨 | PickResult.entityId→부모 체인 | 의미상 root/joint 등록과 부모 연결을 고친다. 기능 포트는 실제 조립 화면의 피킹 ID를 원본 ID로 역매핑한다. |
| 회전 중심이 밀림 | pivot, basis, 자식 메시 오프셋 | keep 보정과 T(C)·R·T(-C)를 구분한다. 관절과 메시 중심을 분리한다. |
| 이동이 다음 프레임에 복원됨 | rest와 applied transform | 기준값에 자동 모션을 합성한다. 두 위치 제어는 서로 다른 부모/자식에 둔다. |
| 노멀을 뒤집었는데 컬링이 그대로 | vertexNormals, triangleIndices, cull | 노멀은 조명이다. winding/cull을 따로 바꾼다. |
| 창 밖에서도 계속 이동 | held 상태, focus(false) | RELEASE/focus에서 자체 상태도 해제한다. |
| 컨테이너 카메라 키가 안 보임 | feature camera 소유자 | `feature camera 이름`으로 카메라 포트를 선택한다. |
| Ctrl+F5 뒤 코드가 그대로 | 바꾼 파일 종류 | 레시피/데이터만 재로드한다. C++는 사용자 재빌드 후 재시작한다. |
| guide 줄을 못 찾음 | 현재 marker와 함수명 | 소스 검색으로 새 위치를 찾고 GuideRecipes.cpp의 사이트를 갱신한다. |

## 원자성과 한계

명령/레시피 실패는 조립 Scene/State/카메라/렌더 설정을 복원한다. native 컨테이너 입력은 다음 update에서 처리하는 이벤트다. 이미 진행한 사용자 콜백의 외부 IO/shared 상태 및 원본 실습 내부 상태를 임의 시점까지 역실행하지 않는다. 프레임 오류는 pause 후 `feature reset 이름`, remove 또는 reload로 복구한다.

C++26의 표준 리플렉션은 컴파일 타임 메타데이터다. 여기서는 C++17에서 사용할 수 있는 명시적 가이드 메타데이터와 diagnose를 제공한다. 자세한 판단은 [리플렉션과 핫로드](REFLECTION_AND_HOTLOAD.md)를 읽는다.
