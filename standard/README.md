# standard: 모드별 입력·피킹·실습 패턴

2026-11-03. Windows x64, C++17, OpenGL 3.3 Core. 빌드·테스트·앱 실행은 사용자가 수행한다.

[0번 기능 조립·직접 편집](docs/assembly/README.md) · [전체 문서](DOCUMENT_INDEX.md) · [입력과 반응 평가](docs/MODE_INPUT_AUDIT.md) · [사용자 확인](USER_CHECKS.md) · [전체 변경 전후](docs/input-modes/BEFORE_AFTER.md)

## main에서 시작 모드 지정

`main.cpp`의 다음 선언 한 곳을 바꾼다.

```cpp
constexpr std::optional<int> initialMode = std::nullopt;
```

`std::nullopt`는 기존 Motion 장면(1번)으로 시작하고 키로 모드를 전환한다. `0`, `1` 또는 `9` 같은 정수는 해당 모드로 바로 시작하고 실행 중 모드 전환을 막는다. 추가 모드도 `10`~`22`로 지정할 수 있다. 범위 밖 상수는 static_assert에서 거부한다.

nullopt에서 0~9 모드 사이에는 숫자 키를 사용한다. 추가 실습에서는 숫자 키가 면·객체 선택 기능이므로 실습에 전달한다. 어느 모드에서든 Alt+0~9는 해당 데모로 전환한다. F1~F11은 모드 10~20(실습 13~23), `[`·`]`는 전체 0~22를 순환한다. F12는 공통 도움말이며 Esc는 종료다. 고정 모드에서는 일반 모드 전환 키가 비활성화된다. Ctrl+F5는 고정 모드에서도 조립 작업공간에 들어가는 전용 편집 키다.

## 모드와 파일

| 번호 | 내용 | 파일 |
|---|---|---|
| 0 | 기능 조립 콘솔·C++ 편집 안내 | [소스](modes/Mode0Assembly.cpp) · [문서](docs/assembly/README.md) |
| 1 | 타원 | [소스](modes/Mode1Ellipse.cpp) |
| 2 | 사각형 | [소스](modes/Mode2Rectangle.cpp) |
| 3 | 다각형 | [소스](modes/Mode3Polygon.cpp) |
| 4 | 사용자 3D 경로·보간 | [소스](modes/Mode4Spatial.cpp) |
| 5 | 원 | [소스](modes/Mode5Circle.cpp) |
| 6 | 왕복 꺾은선 | [소스](modes/Mode6Polyline.cpp) |
| 7 | 격자·거리 경로 | [소스](modes/Mode7Grid.cpp) |
| 8 | 회전 방·미끄럼·낙하 | [소스](modes/Mode8Room.cpp) |
| 9 | 그네·사람형·선형보간 | [소스](modes/Mode9Animation.cpp) |
| 10 | 실습 13 | [소스](modes/Mode10Practice13.cpp) |
| 11 | 실습 14 | [소스](modes/Mode11Practice14.cpp) |
| 12 | 실습 15 | [소스](modes/Mode12Practice15.cpp) |
| 13 | 실습 16 | [소스](modes/Mode13Practice16.cpp) |
| 14 | 실습 17 | [소스](modes/Mode14Practice17.cpp) |
| 15 | 실습 18 | [소스](modes/Mode15Practice18.cpp) |
| 16 | 실습 19 | [소스](modes/Mode16Practice19.cpp) |
| 17 | 실습 20 | [소스](modes/Mode17Practice20.cpp) |
| 18 | 실습 21 | [소스](modes/Mode18Practice21.cpp) |
| 19 | 실습 22 | [소스](modes/Mode19Practice22.cpp) |
| 20 | 실습 23 | [소스](modes/Mode20Practice23.cpp) |
| 21 | 카메라 이동·구/AABB 충돌 | [소스](modes/Mode21CameraCollision.cpp) |
| 22 | 3D 정점·UV·삼각형 입력 처리 | [소스](modes/Mode22EmbeddedMesh.cpp) |

## 1~9 공통 조작

| 입력 | 반응 |
|---|---|
| 왼쪽 클릭 | 클릭 메시의 의미상 객체 선택. 사람의 몸통·팔·다리는 같은 사람 루트, 그네의 줄·좌석·지지대는 그네 프레임을 선택한다. 탑승자는 별도 사람 루트를 선택한다. |
| 빈 공간·편집 대상이 아닌 바닥 클릭 | 선택 해제. 이전 객체를 계속 편집하지 않는다. |
| Tab / Backspace | 다음 / 이전 객체·관절 선택. 9에서는 개별 관절 편집도 가능하다. |
| X / Y / Z | 편집 축 선택 |
| ← / → | 기준 자세의 로컬 회전 -5° / +5° |
| ↑ / ↓ | 부모 좌표에서 +.05 / -.05 이동 |
| PageUp / PageDown | 모델 좌표의 피벗 +.05 / -.05 |
| + 또는 = / - | 양수 균일 배율 확대 / 축소 (.1~3) |
| R / Home | 선택 대상의 기준값 / 현재 장면 전체 복원. 고정 모드에서도 Home 사용 가능. |
| Space | 시간 정지·재개. 정지 중 선택·편집은 가능. |
| V / D | 3뷰 / 단계 추적 로그 |
| S / G / M / I (1~6) | 자전 기여 / 전체 공전 기여 / 반사 종류 / 크기 레이어 삽입·삭제 |
| F (8) | 바닥 개방·복원. 정지 중에도 표시를 즉시 변경. 이미 이탈한 상자는 계속 월드 낙하. |

1~6 사이 경로 전환은 객체 편집과 재생 상태를 유지하고 경로 시간을 0으로 되돌린다. 7~9 및 추가 모드 전환은 각 장면의 상태를 새로 구성한다. 9 재입력은 nullopt에서 전체 재생성이다.

8번 방은 강체이므로 배율 1을 유지한다. 물리 상자의 피벗은 중심을 유지한다. 거부한 입력은 콘솔에 이유를 출력한다. 상자 회전·크기는 방 좌표의 AABB 반크기에 반영한다. 이는 메시 간 정밀 충돌이 아니다.

## 공통 구현과 실행 순서

`Console`은 모드 선택과 콜백 전달을 맡는다. `modes/Editor.cpp`는 객체 루트 선택과 기준 SRT 편집, `MotionCommon.cpp`는 1~6의 공유 Motion 구성, `PracticeCommon.h`는 수학·색·실패 처리, `FaceCommon.*`는 실습 15·16의 육면체 동작, `TankCommon.*`는 실습 19·22의 전체 탱크 동작을 공유한다.

main → 콜백 등록 → Application::run → initScene → activateMode. 프레임은 PollEvents → onUpdate → Scene.updateMotions → onRender → SwapBuffers다. 렌더 패스에서 시간을 다시 진행하지 않는다. 키·수정키·마우스·커서·포커스·갱신·렌더 콜백은 main에서 한 번만 등록한다.

경로·물리·애니메이션 상태와 편집 기준값을 합성한다. 피킹은 렌더 뷰의 카메라·프레임버퍼·컬링 정책을 사용한다. 반사 모델은 렌더러와 같은 winding 보정을 적용한다. 피킹은 삼각형 표면 기준이며 선분·텍스처 투명도·임의 셰이더 변형의 픽셀 일치를 보장하지 않는다.

## 문서와 배포

[API](docs/API_REFERENCE.html), [응용 패턴](docs/IMPLEMENTATION_PATTERNS.html), [시험 구현 순서](docs/EXAM_IMPLEMENTATION_ORDER.html), [독립 배포](PORTABLE.md)를 함께 읽는다. 실습 패턴 원문 코드는 [코드 모음](docs/practice-cookbook/README.md)에 보존한다. 현재 실행 모드의 차이는 MODE_INPUT_AUDIT.md가 우선한다.

standard 전체를 복사한다. 다른 실습 폴더·외부 모델·추가 라이브러리를 요구하지 않는다. 변경한 소스·설정·문서는 UTF-8 BOM이다. 기존 Framework.props와 API 문서의 사용자 수정은 보존했다.

## 0번 조립 API

0번은 group/part/joint로 큐브부터 계층을 쌓고, 최상위 SRT·피벗·자전·공전을 조합한다. mount/anchor는 객체의 면과 좌표에 관절을 붙인다. normal/winding/cull, warp 비균일·반사 그룹, face-track 이동·축소·임의 축 회전, 물리·보행·입력 바인딩을 지원한다. C++ Input 훅으로 키·마우스·커서·포커스 반응도 기능별로 붙일 수 있다.

콘솔은 실험을 돕는다. 확장은 `guide <기능>`으로 현재 C++ 파일·줄을 찾아 직접 편집한다. [명세](docs/assembly/SPEC.md), [직접 편집](docs/assembly/CODE_GUIDE.md), [탱크·트럭·사람 패턴](docs/assembly/PATTERN_COMBINATIONS.md), [실습별 표현력 평가](docs/assembly/REQUIREMENT_AUDIT.md)를 분리했다. 기본 명령만으로 모든 실습의 세부 상태 전이를 복제한 것은 아니다. 사용자 빌드·확인은 [0번 확인 절차](docs/assembly/USER_CHECKS.md)를 따른다.

## 실습 컨테이너와 코드 편집 가이드

Ctrl+F5는 examples/assembly/workspace.assembly를 읽어 조립 작업공간을 연다. `feature add 이름 1~22`로 모든 기존 모드를 독립 상태로 넣고, parts/port/snapshot으로 내부 구조를 꺼내 조립한다. 입력과 카메라는 focus/camera 포트로 선택한다. [기능 컨테이너](docs/assembly/FEATURE_CONTAINERS.md)를 따른다.

guide는 현재 파일의 실제 줄과 주변 코드, 변경 전후 예시, 단위/제약, 디버깅 순서와 예상 결과를 출력한다. [편집·디버깅](docs/assembly/EDIT_DEBUG_GUIDE.md), [C++26 리플렉션과 핫로드 구분](docs/assembly/REFLECTION_AND_HOTLOAD.md)을 읽는다. C++ 함수 구현 변경은 사용자 재빌드가 필요하다.

[30MB 이하 소스 패키지 정책](docs/assembly/PORTABLE_PACKAGE.md). .vs/x64 캐시를 제외하고 코드·SDK·문서·예제·핵심 그래프를 포함한다. 기존 작업 폴더를 삭제/정리하지 않는다.
