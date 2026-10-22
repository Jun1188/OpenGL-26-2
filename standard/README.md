# standard: 독립 OpenGL 프레임워크 샘플

이 프로젝트는 `standard.slnx`로 여는 Windows x64용 독립 샘플이다. 실습 18~19와 같은 프로젝트 구성을 사용하고, 프레임워크의 Motion·경로·반사·레이어 API를 보여준다. 실습 1~19의 소스나 설정은 수정하지 않았다.

2026-10-22 기준 소스·설정으로 작성했다. 빌드·실행·다른 PC 배포 테스트는 수행하지 않았다. [사용자 확인 절차](USER_CHECKS.md)를 따라 직접 확인한다.

## 1. 시작

1. `standard` 폴더 전체를 보존한 상태에서 `standard.slnx`를 Visual Studio로 연다.
2. `standard`를 시작 프로젝트로 선택한다. `Debug | x64` 또는 `Release | x64`를 선택한다.
3. 사용자가 빌드하고 F5로 실행한다. 예상 출력은 `x64/Debug/standard.exe` 또는 `x64/Release/standard.exe`다.
4. 창과 한글 콘솔 안내를 확인한다. 키는 렌더링 창에 포커스가 있을 때 입력한다.

프로젝트는 C++17, v145, Windows SDK 10.0, x64 구성이다. 해당 도구 집합·SDK와 OpenGL 3.3을 지원하는 드라이버는 실행할 PC의 조건이다. 도구 집합이 없으면 설치 또는 사용자가 선택한 환경으로 재대상 지정이 필요하다. 프로젝트 생성만으로 이 환경이 설치되지는 않는다.

새 코드·프로젝트 설정·문서는 UTF-8 BOM이다. `framework/Framework.props`의 `/utf-8`과 `Application::run()`의 UTF-8 콘솔 출력 설정을 재사용한다. 제3자 의존성은 원본 바이트와 라이선스를 보존했다.

## 2. 폴더와 실행 흐름

| 파일 | 책임 |
|---|---|
| [main.cpp](main.cpp) | Application과 Console 생성, 콜백 등록, run 호출 |
| [Console.h](Console.h) | 샘플 상태·식별자·입력 인터페이스 |
| [Console.cpp](Console.cpp) | 장면 생성과 경로·반사·레이어 명령 |
| standard.vcxproj / .filters / .slnx | 독립 프로젝트, x64 구성, 소스 등록 |
| [framework/Framework.h](framework/Framework.h) | 샘플이 포함하는 공개 헤더 |
| framework/dependencies | 내부 의존성·DLL·라이선스 |
| [USER_CHECKS.md](USER_CHECKS.md) | 사용자가 실행할 빌드·조작·공유 확인 |
| [BEFORE_AFTER.md](BEFORE_AFTER.md) | 전체 신규 코드와 설정, 복사 범위, 분석 변경 |

```text
main -> 콜백 등록 -> Application::run
  초기화 -> Console::initScene
  반복:
    PollEvents -> onKey / onMouse (이벤트가 있을 때)
    Scene::updateMotions(dt) -> 렌더링 -> SwapBuffers
  shutdown -> 자산·GPU·창·GLFW 정리
```

콜백 등록 시 Console의 함수 본문을 즉시 실행하는 것은 아니다. 캡처한 Console은 run이 종료될 때까지 유지된다. 시간 갱신은 공통 Application이 담당하므로 샘플에 별도의 update 콜백이나 중복 Motion 갱신을 추가하지 않았다. 키를 누르고 있는 상태도 보관하지 않으므로 포커스 이탈 때 해제할 지속 입력 상태는 없다.

## 3. 초기 장면과 조작

주황·파랑 직육면체가 같은 경로와 자전 시각을 공유한다. 파랑 쪽 가지에는 YZ평면 반사가 먼저 적용된다. 각 물체의 기저 선은 물체에 부착되어 함께 움직인다. 가운데 회색 선은 소유자가 없으며 루트 레이어에만 영향을 받는다. 객체의 회전은 항등 상태에서 시작하고 초기 보기 각도는 카메라에 둔다.

| 입력 | 동작 |
|---|---|
| 1 | 타원 경로 |
| 2 | 사각형 경로 |
| 3 | 다각형 경로 |
| 4 | 사용자 3D 경로 + smoothStep 시간 보간 |
| 5 | 원 경로 |
| 6 | 열린 꺾은선의 왕복 이동 |
| Space | 경로·자전·전체 공전 시계를 현재 자세에서 정지/재개 |
| S | 자전 변환의 기여 제거/복구 |
| G | 루트 Y공전의 기여 제거/복구 |
| M | 평면 반사 -> 점대칭 -> 직선대칭 -> 반사 없음 순환 |
| I | 첫 객체에 Local 1.5배 레이어 삽입/삭제 |
| 왼쪽 클릭 | 보이는 물체의 entity/face ID 출력 |
| H / Esc | 도움말 / 종료 |

초기 경로는 타원, 자전은 90도/초다. 루트 공전은 15도/초로 설정하되 초기 기여는 꺼 둔다. 경로의 durationSeconds는 6초다. 경로를 변경하면 경로 시각만 0으로 돌아가고 자전 시각·정지 상태는 유지한다.

Space는 마지막 변환을 유지하면서 시간을 멈춘다. S/G는 기여를 항등으로 바꾸며, 재생 중이면 숨겨진 시계도 계속 진행한다. 따라서 S/G로 다시 켤 때는 현재 시각의 자세가 나타난다. 반사 없음에서는 두 객체와 경로가 겹치므로 보이지 않는 오류로 단정하지 않는다.

## 4. API 사용 패턴

```text
root (전체 Y공전 Motion)
├─ 경로(pathId) -> 자전(spinId) -> 주황 객체
└─ 반사 행렬 -> 경로(같은 pathId) -> 자전(같은 spinId) -> 파랑 객체
```

하나의 pathId와 spinId를 두 가지에서 공유하므로 시계가 중복 진행하지 않는다. 반사를 경로 바깥에 두어 이동 궤적과 형상을 함께 반사한다. I키는 기존 자전 말단 위에 레이어를 넣고 제거하며, 말단 ID·객체 바인딩은 유지한다. 공간·중심·동작 합성의 계약은 동봉한 [프레임워크 사용설명서 13절](framework/README.md#13-변환motion레이어-트리-2026-10-22)에 있다.

입력은 choosePath/changeReflection/toggleInsertedLayer를 통해 상태와 연결을 바꾼다. 시간에 따른 행렬은 프레임워크가 계산하고 Renderer와 피킹이 같은 최종 행렬을 사용한다. 명령 API 실패는 콘솔에 알리고 종료를 요청한다. 사용자 함수는 유한한 좌표를 반환하고 평가 중 Scene·MotionSystem을 수정하지 않는 계산 함수로 작성한다.

선분 경로는 길이에 따라 매개변수를 나눈다. 타원·사용자 곡선은 매개변수가 일정하게 변해도 실제 이동 속도가 일정하지 않을 수 있다. 경로 함수는 이동 위치를 제공하고 객체 방향을 접선에 자동 정렬하지 않는다. 회전은 독립 Motion으로 합성한다.

샘플을 새 프로그램의 시작점으로 쓰려면 Console::initScene의 장면과 Console::onKey의 입력을 바꾼다. 기본 Application 수명·콜백 연결과 framework 내부 상대경로는 유지한다. 프레임워크 예제 폴더의 다른 main을 동시에 프로젝트에 추가하지 않는다.

## 5. 다른 폴더·PC로 공유

소스 공유는 `standard` 폴더 전체를 ZIP에 넣는다. 특히 내부 framework와 dependencies·라이선스를 포함한다. `.vs`, `x64`, `*.vcxproj.user` 같은 로컬 상태·빌드 결과는 소스 공유에 필요하지 않다. 다른 실습 폴더나 저장소 루트 framework는 실행 의존성이 아니다.

Framework.props는 자기 위치인 `$(MSBuildThisFileDirectory)` 기준으로 헤더와 DLL을 찾는다. 빌드 후 DLL·라이선스는 실행 파일 옆의 `framework/dependencies`, 셰이더는 `framework/shaders`로 배치된다. 이 샘플은 기본 도형·내장 셰이더를 사용하므로 외부 모델·텍스처를 요구하지 않는다. GLEW를 직접 초기화하거나 별도 절대경로 LIB를 추가하지 않고 현재 Native 로더 방식을 사용한다.

실행 파일만 공유할 때는 해당 출력 폴더의 EXE와 옆의 framework를 함께 보낸다. 기본 설정의 projectRoot는 실행 파일 기준 `.`이다. 다른 폴더를 작업 디렉터리로 사용해도 실행 파일 옆의 배포 폴더를 기준으로 찾는 구조다. 디버거 작업 폴더를 아직 평가되지 않은 TargetDir로 강제하지 않는다. 이 프로젝트는 `.vcxproj.user`를 생성하거나 개인 PC 경로를 저장하지 않는다.

동봉 프레임워크의 일부 과거 실습 설명 링크는 상위 저장소를 참조하므로 폴더 단독 공유 시 열리지 않을 수 있다. 샘플의 실행·조작·확인은 이 README와 USER_CHECKS만으로 따라갈 수 있다. 다른 PC에서의 실제 빌드·DLL 로딩·창 표시는 사용자 확인 항목이다.
