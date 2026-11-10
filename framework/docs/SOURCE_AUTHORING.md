# SceneAuthoring: C++ 직접 작성 API

원본은 `include/SceneAuthoring.h`, 구현은 `detail/SceneAuthoring.cpp`다. Standard에 동일한 배포 소스를 포함한다.

| API | 계약 |
|---|---|
| ObjectSetup / addObject | box/tetra/pyramid, 모델 크기, 월드 배치·비균등 배율·피벗·재질. 부모를 먼저 등록 |
| addModel(name, model, worldPose, parent) | 구 등 준비한 Model의 기하·재질을 보존하고 월드 배치로 등록 |
| setLayer(object, name, setup) | 이미 등록한 지역 레이어 갱신. 시간 초기화 없이 다음 update에서 평가 |
| light(name) / lightParent(name) / setLight | 등록된 광원의 지역 값·프레임 조회 및 교체. 없는 이름은 예외 |
| syncTools | 모든 Scene 모션 뒤 부착 광원·카메라 최종 동기화. 시간을 진행시키지 않음 |
| LayerSetup / addLayer | 객체 소유 지역 레이어, T(p+pivot) Rz Ry Rx Rspin S T(-pivot), 등록 순서 상속 |
| PointLight / addLight | 최대8 점광원. 선택 부모가 있으면 지역 위치에서 월드로 변환 |
| Camera / setCamera | 하나의 활성 카메라. 선택 부모에 부착, 시선/up 직교화 |
| update(seconds) | 절대 시간으로 레이어·광원·카메라 평가. 프레임 dt를 직접 전달하지 않음 |
| object(name) / name(id) | 이름↔Scene 엔티티 ID 연결. 없는 이름은 예외, 없는 ID는 빈 이름 |
| clear | Scene과 작성 핸들을 함께 초기화. 이전 ID를 재사용하지 않음 |

직접 C++로 설정하고 사용자 콜백에서 갱신한다. 소스 편집용 주석은 debugger만 읽으며 런타임 의존성이 아니다. 새로운 사물의 상태·법칙은 독립 컨트롤러에서 기존 변환·Motion·Dynamics API로 연결한다. 재질과 셰이더의 기존 광원 계산을 재사용하며 이번 작업은 새로운 그림자·강체 솔버를 추가하지 않는다.

자세한 작성 순서와 확인 절차는 Standard의 DIRECT_CPP_WORKFLOW.md, SOURCE_DEBUGGER.md, LIGHTING_AND_MATERIALS.md를 참조한다. 소스·문서 반영과 컴파일·실행 성공은 구분한다. 빌드·테스트는 사용자가 수행한다.

기본 컨트롤러의 구·표시·광원 공전은 Standard의 독립 모듈이며 프레임워크에 시험 사물 enum을 추가하지 않는다. 외부/내장 GLSL에 합성·법선·확산·정반사 출력(uStudyView)을 제공한다. 예제 컨트롤러는 공통 API를 사용하는 선택 코드다.
