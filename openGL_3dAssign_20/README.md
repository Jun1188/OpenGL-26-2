# 실습 20: 택배 물류 센터

원문: `../lecture_pdf/homework/지엘3 좌표계 변환 2 (실습 20).pdf`, 1~2쪽.
공통 프레임워크 기준 커밋: `fd79f92`. 기존 탱크 코드는 복원하지 않고 새 응용을 작성했다.

## 실행 프로젝트와 키

`openGL_assign_20.sln` 또는 `openGL_assign_20.slnx`를 연다. Debug/Release x64를 사용한다. 빌드·실행·테스트는 사용자가 수행한다. 에이전트는 이를 실행하지 않았다.

| 키 | 결과 |
|---|---|
| b | 벨트와 상자 이동을 함께 시작/정지. 초기 상태는 정지 |
| + / - | 확대/축소. 일반 + 위치(= 키), 숫자패드 +/−도 지원 |
| p / P | 소문자 p는 원근, 대문자 P는 직각 투영 |
| ← / → | 누르는 동안 주목점의 월드 Y축 기준 시계/반시계 공전. 위에서 내려다본 기준 |
| r | 이동시간 0, 정지, 모든 상자·줄무늬 초기 배치, 초기 원근 쿼터뷰, 확대율, 누름 상태 복원 |
| q | 종료 |

P 입력은 Shift와 Caps Lock의 XOR로 대소문자를 구분한다. 둘을 함께 켜면 소문자 p로 처리한다. 포커스를 잃으면 좌우 누름 상태를 해제한다. 카메라는 자동으로 움직이지 않는 고정 3인칭 쿼터뷰이며 사용자의 공전·줌 명령만 반영한다. 확대/축소의 범위는 원근 거리 2~60, 직각 반높이 2~35다.

## 요구사항과 구현 위치

| PDF 요구 | 구현 |
|---|---|
| XZ 바닥, 바닥 위 일정 높이의 ㄱ자 평면 벨트 | `Console::initScene`: y=0 바닥, y=1의 두 볼록 평면을 모서리에서 연결 |
| 육면체 여러 개로 줄무늬 | 폭 1.4, 높이 .03, 길이 .10의 육면체 30개 |
| 다른 색 상자 5개와 번호 | 서로 다른 색의 .85 정육면체 5개, 네 옆면에 1~5 번호 |
| 벨트와 상자가 함께 이동 | `placeMovingObjects`: 모든 객체에 동일한 `clock_.seconds * beltSpeed` 적용 |
| 끝에서 시작으로 순환 | 열린 `DistancePath`의 Loop 샘플. 끝→시작은 순간 이동이며 복귀 선분 없음 |
| 코너에서 상자 방향 회전 | `PatternSystem::follow`가 경로 접선의 Y 회전각 적용 |
| 고정 쿼터뷰, 키 명령 | `resetState`, `onKey`, `onUpdate`, `onFocus` |

## 입력 → 상태/변환 → 화면

`main`은 Application과 Console의 수명을 소유하고 초기화·수정키·포커스·업데이트 콜백을 등록한다. Application이 창과 GPU 자원을 생성한 뒤 `initScene`을 호출한다. 기본 렌더 콜백이 Scene을 그린다. 실패 bool/optional은 검사하고 예외를 main에서 출력한 뒤 비정상 종료 코드로 반환한다. 프레임워크는 그 전에 오류 단계와 API를 출력한다.

1. b 입력이 `clock_.playing`을 바꾼다.
2. 프레임마다 `onUpdate`가 `clock_.advance(dt)`를 한 번 호출한다.
3. `travelled = seconds × 1.25`에 각 객체의 고정 거리 오프셋을 더한다.
4. `follow`가 `DistancePath::sample`의 위치·접선으로 Scene 변환을 바꾼다.
5. Scene의 부모 변환은 상자와 네 측면 번호에 함께 적용된다. Renderer가 정점 입력과 모델·뷰·투영 행렬을 셰이더로 전달한다.

경로 중심은 `(-4,1,-2) → (3,1,-2) → (3,1,3)`이고 길이는 12다. 상자의 경로상 간격은 12/5=2.4, 줄무늬는 12/30=.4다. 같은 경로거리로 배치하므로 코너 양쪽 상자의 **직선거리**는 2.4보다 짧을 수 있다. 번호 순서는 경로상 순환 순서이며 역방향으로 번호가 바뀌지 않는다. 시작과 끝은 별개 위치이며 Loop는 끝에서 시작으로 순간 이동한다.

경로 접선이 모서리에서 즉시 바뀌므로 상자도 90도 즉시 회전한다. 둥근 코너 보간은 추가하지 않았다. 벨트 평면은 오목 ㄱ자를 단일 fan으로 만들지 않고 겹치지 않는 두 볼록 사각형으로 만든다. 줄무늬 아래면은 벨트 y=1, 위면은 y=1.03이다. 상자 바닥은 y=1.03이며 번호는 옆면보다 .008 바깥에 있다. 이 높이 분리로 겹치는 표시 면의 z-fighting을 피한다.

카메라는 초기 `(11,10,13)`에서 `(0,.7,.3)`을 본다. 공전축은 주목점을 지나는 월드 Y축이다. 벨트가 정지해도 카메라 조작은 가능하다. Application이 Scene 모션을 자동 갱신하므로 응용에서 `updateMotions`를 중복 호출하지 않는다.

## 공통 API와 배포

- `framework/docs/APPLICATION_API.md`: 응용 API 계약과 실습 대응.
- `framework/include/Patterns.h`, `framework/detail/Patterns.inl`: DistancePath, AnimationClock, addPolygon/addBox/addNumber/addJoint/rotateJoint/follow/zoomCamera/orbitCamera.
- `framework/include/Graphics.h`: Camera, Scene, RenderOptions.
- `framework/include/Application.h`, `framework/detail/Application.inl`: 콜백, Caps Lock 수정키, 포커스, 프레임 루프, 오류 전달.
- `framework/Toolchain.props`, `framework/Framework.props`: 설치된 도구 집합 선택과 런타임 배포.

프로젝트는 C++17, x64, Debug /MTd·Release /MT다. 고정 v145나 외부 SDK 절대경로를 추가하지 않았다. `projectRoot="."`, `runtimeDirectory="framework"`는 실행 파일 기준이며 빌드 시 Framework.props가 DLL과 셰이더를 출력 폴더로 복사한다. 배포할 때 실행 파일만 복사하지 말고 출력 폴더의 `framework`도 함께 복사한다.

## 확인 상태와 학습 질문

소스와 PDF를 읽고 구현했다. 빌드·실행·테스트·검증기는 실행하지 않았다. 실제 번호 가독성, 화면 배치, DLL 로딩, 다른 PC 호환성은 미확인이다. 사용자 확인 절차와 예상 결과는 `USER_CHECKS.md`에 있다.

확인 질문: `clock_.seconds` 대신 상자마다 별도 프레임 카운터를 사용하면 벨트와 상자의 상대 위치가 어떤 조건에서 달라질 수 있는가?
