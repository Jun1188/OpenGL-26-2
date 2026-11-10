# C++ 직접 작성과 실행 흐름

작업의 원본은 `SceneSetup.cpp`이다. 명령·레시피가 C++ 원본을 대신하지 않는다.

## 입력 → 상태/변환 → 출력

1. 사용자가 `ObjectSetup`, `LayerSetup`, `PointLight`, `Camera`, `RenderOptions` 값을 직접 편집한다.
2. `main`은 Application 콜백을 한 번 등록한다. `run` 초기화 후 `Console::initScene`이 `configureScene`을 호출한다.
3. `SceneAuthoring::addObject`는 월드 배치를 부모 기준 attachment에 저장한다. 비균등 배율 결과를 SRT로 다시 분해하지 않는다.
4. `Console::onUpdate`가 ExamController에 프레임 dt(정지 중 0)를 전달하고 절대 시간을 누적한다. SceneAuthoring::update(seconds)가 지역 레이어를 평가한다. Application의 기존 Scene 모션 갱신은 그 뒤 한 번 실행된다.
5. `Console::onRender`가 최종 syncTools → controller.syncVisuals를 수행한 뒤 Application::renderScene → Renderer가 모델·정규행렬·광원·재질을 GPU에 전달한다. Scene의 월드/면 행렬은 렌더와 피킹이 함께 사용한다.
6. 종료 시 Application이 GPU·창·GLFW 수명을 정리한다. debugger는 이 실행 흐름에 참여하지 않는다.

## 편집 위치

| 바꿀 내용 | 작성 위치 / API |
|---|---|
| 객체 생성·위치·회전·비균등 배율·모델 피벗·재질 | `SceneSetup.cpp` object 구획 / `ObjectSetup`, `addObject` |
| 부모 연결 | `scene.addObject("자식", 설정, "부모")`, 부모를 먼저 등록 |
| 지역 레이어와 지속 회전 | layer 구획 / `LayerSetup`, `addLayer` |
| 독립 주변광·방향광·배경 | render 구획 / `RenderOptions` |
| 점광원·부모 부착 | light 구획 / `PointLight`, `addLight` |
| 투영·시점·부모 부착 | camera 구획 / `Camera`, `setCamera` |
| 특정 기믹의 상태·힘·입력 법칙 | 독립 컨트롤러 또는 `Console.cpp` 콜백에서 기존 공통 API 호출 |

새 사물마다 프레임워크 enum이나 명령 파서를 늘리지 않는다. 기본 형상 도우미는 box/tetra/pyramid를 지원한다. 구·원뿔·모델·면·경로·힘 등은 기존 Scene/PatternSystem/Motion/Dynamics API로 직접 연결할 수 있다. 이들 모두를 디버거의 구조화 명령으로 변환한다고 주장하지 않는다. 그런 소스는 `find/code/replace/insert`로 위치와 코드를 안내받는다.

`SceneAuthoring::update`가 자신에게 등록한 객체의 지역 레이어, 점광원 목록, 등록 카메라를 관리한다. 같은 상태에 별도 컨트롤러를 동시에 쓰면 덮어쓸 수 있다. 네이티브 모션은 별도 객체에 연결하거나 해당 상태의 소유자를 명확히 정한다. `clear` 후 이전 EntityId는 재사용하지 않는다.

각 프레임의 잘못된 입력은 예외로 Application 종료 경로에 전달한다. `addObject` 생성 실패는 생성 전 Scene으로 복원한다. 다른 등록 함수는 값을 확인한 뒤 목록에 추가한다. `update`는 여러 객체에 대한 트랜잭션이 아니며 중간 실패 시 계속 진행하지 않는다.

## 배포 경계

Standard는 자체 framework·셰이더·헤더·라이브러리·실행 DLL을 가진다. 선택 도구는 `standard/debugger`에 있으며 자체 소스와 Toolchain.props만 사용한다. Standard의 vcxproj와 framework의 헤더/props/런타임은 debugger를 참조하지 않는다. Standard를 직접 빌드·실행할 때 debugger가 필요하지 않다. debugger도 Standard나 framework를 링크하지 않고 실행 인수로 선택한 디스크 소스를 읽는다.

standard.sln은 Standard만 참조한다. 저장소 루트의 편의 솔루션은 두 프로젝트를 표시하지만 ProjectReference/ProjectDependencies를 추가하지 않으며 debugger의 Build.0 기본 선택도 제외한다. 도구를 빌드하려면 그 프로젝트를 선택하거나 내부 단독 솔루션을 사용한다. standard 폴더 전체를 복사하면 내부 두 프로젝트와 framework 사본이 함께 이동하며 상위 저장소에 실행/빌드 의존성을 두지 않는다. debugger 폴더를 뺀 복사본도 standard.sln/standard.vcxproj와 framework 소스를 그대로 사용한다. debugger 폴더 자체도 별도로 복사할 수 있다. 폴더 분리 후 실제 빌드·실행 확인은 사용자가 수행한다.


## 2차 코딩테스트 작업공간

2026-11-24 화요일 시험 준비의 전체 학습 HTML·순정 OpenGL·N제·시나리오는 저장소의 coding-test-2 폴더에 별도로 둔다. Standard 제출 입력은 해당 학습 폴더를 참조하지 않는다. 같은 저장소에서는 [오프라인 학습](../../coding-test-2/index.html)으로 이동할 수 있다. Standard만 복사한 배포본에서는 이 링크가 학습 참고 링크이며 빌드/실행 입력이 아니다.

ExamController는 key 광원과 월드 카메라, studySphere/studySpin, lightMarker를 소유한다. 기존 초기 광원을 읽고 부모 이름을 보존한다. 다른 소스가 같은 상태를 동시에 쓰지 않는다. 전체 장면을 기존 examples 함수로 교체할 때는 main.cpp의 Console sample(app, false)로 기본 컨트롤러를 끈다. 기본 실행은 true다.

외부 GLSL은 main의 shaderDirectory=framework/shaders로 선택한다. 소스 GLSL 수정 후 사용자가 Rebuild하여 출력 폴더의 사본도 갱신한다. 내장 detail/Shaders.h만 바꿔도 이 외부 경로가 바뀌는 것은 아니다. uStudyView=0/1/2/3은 합성/법선/확산/정반사이며 기존 내장 셰이더 사용자에게 기본 0을 유지한다.

제출은 debugger, 프로젝트 x64/.vs, docs, 튜토리얼·학습 HTML·예제를 뺀 소스 기준 30,000,000바이트 이하다. SDK의 x64 라이브러리·DLL·헤더·라이선스는 남긴다. Package-Standard.py는 실제 파일 삭제 없이 이 범위로 ZIP을 만든다. 표준 개발 환경(OS·MSVC·Windows SDK·호환 드라이버)은 필요하다. 상위/형제 폴더·개인 절대 경로의 실행/빌드 입력을 두지 않는다.

빌드·테스트·브라우저/앱 실행·화면 결과는 사용자 확인 대상이다. 소스·문서 생성과 실행 성공을 구분한다.
