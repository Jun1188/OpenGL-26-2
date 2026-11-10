# C++ 직접 작성과 실행 흐름

작업의 원본은 `SceneSetup.cpp`이다. 명령·레시피가 C++ 원본을 대신하지 않는다.

## 입력 → 상태/변환 → 출력

1. 사용자가 `ObjectSetup`, `LayerSetup`, `PointLight`, `Camera`, `RenderOptions` 값을 직접 편집한다.
2. `main`은 Application 콜백을 한 번 등록한다. `run` 초기화 후 `Console::initScene`이 `configureScene`을 호출한다.
3. `SceneAuthoring::addObject`는 월드 배치를 부모 기준 attachment에 저장한다. 비균등 배율 결과를 SRT로 다시 분해하지 않는다.
4. `Console::onUpdate`가 절대 시간을 누적하고 `SceneAuthoring::update`가 지역 레이어와 부모를 따르는 광원·카메라를 평가한다. Application의 기존 Scene 모션 갱신은 그 뒤 한 번 실행된다.
5. `Console::onRender` → `Application::renderScene` → Renderer가 모델·정규행렬·광원·재질을 GPU에 전달한다. Scene의 월드/면 행렬은 렌더와 피킹이 함께 사용한다.
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

Standard는 자체 framework·셰이더·헤더·라이브러리·실행 DLL을 가진다. debugger는 자체 소스와 Toolchain.props만 사용한다. 합본 솔루션의 프로젝트 경로는 편의용이며 각 단독 솔루션의 빌드/실행 의존성은 아니다. 실제 소스 위치는 debugger 실행 인수나 project 명령으로 선택한다. 폴더를 다른 위치로 복사한 뒤 빌드·실행하는 확인은 사용자가 수행한다.
