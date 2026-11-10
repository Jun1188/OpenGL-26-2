# Standard를 가져갔을 때의 실전 작성

## 파일마다 맡길 책임

- SceneSetup.cpp: 객체·부모·초기 재질·초기 광원·시점. @debug 구획은 소스 위치 표시다.
- ExamController.h: 변화하는 상태와 기본값. 현재 값과 기준값을 구분한다.
- ExamController.cpp initialize: 구·광원 표시와 필요한 선/레이어를 한 번 만든다.
- onKey/onFocus: PRESS 토글, 유지 입력, 포커스 해제만 처리한다.
- update(dt): 상태를 갱신하고 setLayer/setLight/setCamera로 전달한다.
- Console::onRender: native Scene 모션이 끝난 뒤 syncTools→syncVisuals→renderScene.
- main.cpp: 콜백 수명, 외부 셰이더 선택, 추가 uniform 바인딩.

같은 광원·카메라를 여러 컨트롤러가 쓰지 않는다. 기본 컨트롤러는 key 광원 값을 읽어 제어하고 처음 등록한 light parent를 보존한다. 카메라는 초기 월드 카메라를 읽어 세계 좌표로 제어한다. 부모에 붙은 다른 카메라를 학습하려면 이 제어권을 분리한다.

## 일반 모델을 직접 추가하기

```cpp
auto model = graphics::DiagramSystem::makeSphere(.5f, 24, 16);
for (auto& mesh : model.meshes()) {
    mesh.material.color = {.4f,.7f,.9f};
    mesh.material.specularStrength = .8f;
    mesh.material.shininess = 64;
}
graphics::Transform3D worldPose;
worldPose.basis = {2,1,0};
worldPose.scale = {2,1,.5f};
scene.addModel("sample", model, worldPose);
```

모델 데이터 안의 재질을 보존한다. addObject는 상자/사면체/피라미드 편의 생성이고 addModel은 준비한 임의 모델을 등록한다. 모델마다 새로운 공통 enum이나 실습 모드를 늘리지 않는다.

## 실제 소스 편집 보조

```text
project "자신의 standard 폴더"
where state
where input
where update
where shader
where uniform
show object panel
object panel scale 2 1 0.5
find "setLight"
```

where는 현재 파일의 @debug hook과 주변 코드를 출력한다. 외부 GLSL과 내장 대체 소스가 함께 나올 수 있으므로 main의 경로 설정을 먼저 본다. debugger는 C++ 의미를 실행하거나 자동 저장하지 않는다. before/after 제안을 보고 직접 편집·저장한다. 외부 파일을 바꾼 뒤 사용자 Rebuild/실행으로 확인한다. 도구는 standard/debugger 안에 있고 제출 시 제외한다. Standard와 framework는 debugger를 참조하지 않는다.

## 수정 한 건의 완결 조건

예를 들어 '광원을 이동하는 대상의 지역 좌표에서 공전'시킨다면: 광원 parent를 정하고, center/radius/phase를 그 지역 좌표로 정의하고, setLight에 parent를 전달하고, 최종 parentWorld로 표시 도형·궤도도 변환한다. 키 입력에서 반경만 바꾸고 표시는 별도 상수로 놓는 것으로 끝내지 않는다.

Scene::setWorldPlacement는 자신의 지역 레이어 이전 배치를 다룬다. setLayer는 해당 객체에 미리 등록한 레이어를 다음 update에서 평가한다. setLight는 등록된 이름만 수정하며 없는 이름을 임의 생성하지 않는다. syncTools는 시간을 진행시키지 않는다. 실패 시 값을 무시하고 계속 진행하는 대신 원인을 소스에서 확인한다.

## 제출 경계

제출 패키지에서는 debugger, .vs, 프로젝트 x64, 튜토리얼/학습 HTML·문서를 제외한다. C++ 소스·프로젝트/props·셰이더·필수 SDK 헤더/라이브러리/DLL/라이선스는 남긴다. dependencies 안의 x64 라이브러리 폴더는 빌드 산출물이 아니므로 보존한다. 소스 입력만 기준으로 30,000,000바이트 이하를 관리한다. 실제 제출 규칙이 cpp/셰이더만 요구하면 그 규칙을 사용자와 다시 구분하며, 이 패키지를 자동 제출하지 않는다.

기존 C++ 참조 함수로 장면 전체를 교체할 때는 main.cpp에서 `Console sample(app, false);`로 기본 시험 컨트롤러를 끈다. 그 예제가 등록한 광원 이름·부착 카메라의 소유권을 유지하기 위한 선택이다. 기본 작업공간은 true다.
