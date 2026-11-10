# C++ 변환·조명·카메라 예제

원문은 `examples/SourceExamples.h`다. 외부 assembly 파일이나 명령 문자열을 실행하지 않는 C++ 함수 두 개를 제공한다. 현재 Standard의 기본 장면 대신 확인하려면 `SceneSetup.cpp`에서 이 헤더를 include하고 configureScene 본문을 아래 호출 중 하나로 직접 바꾼다. main.cpp도 `Console sample(app, false);`로 바꾸어 기본 시험 컨트롤러를 끈 뒤 사용자가 재빌드한다. 부착 카메라·광원 이름은 예제 함수가 소유한다.

```cpp
#include "examples/SourceExamples.h"
// configureScene 본문에서 하나를 선택한다.
source_examples::rotatingDisplay(scene, options);
// 또는 source_examples::attachedLightAndCamera(scene, options);
```

`rotatingDisplay`는 회전 지지대와 표시판이다. 객체 부모 상속, 비균등 배율, 왼쪽 끝 피벗 회전, 후행 지역 이동을 조합한다. `attachedLightAndCamera`는 회전하는 운반대에 광원과 카메라를 붙여 고정 조형물을 관찰한다. 원래 실습의 사물 이름만 바꾼 재출제 예측이 아니라 연산·시간·기준 프레임을 조합하는 예다.

둘 다 시간0 등록 후 `SceneAuthoring::update(seconds)`로 갱신한다. 부모의 등록된 지역 레이어는 0초에서 먼저 반영되어 나중에 등록하는 자식이 지정 월드 배치에서 시작한다. 등록 함수는 장면 초기화 단계에서 사용한다. 런타임에서 레이어를 추가하면 기존 지역 레이어가 우선 0초로 초기화되므로 다음 update까지의 중간 상태를 별도 동작으로 사용하지 않는다.

이 헤더는 참조 함수이며 독립 main을 추가하지 않는다. 구획 주석을 쓰는 기본 SceneSetup.cpp는 구조화 debugger 명령 대상이다. 예제 함수로 교체한 뒤에는 find/code/replace/insert를 사용하거나 편집할 코드에 유일한 @debug 구획을 직접 추가한다. 예제의 실행 결과는 아직 확인하지 않았다.

프레임워크 자체의 별도 main 예제는 `framework/examples/source_authoring.cpp`다. 이를 별도 프로젝트의 진입점으로 선택하고 그 폴더의 Framework.props를 import한다. Standard의 main과 동시에 컴파일하지 않는다.

## 전체 예제 코드

```cpp
#pragma once
#include "../framework/Framework.h"

// Native C++ references. Call one function from configureScene after scene.clear().
namespace source_examples {
inline void rotatingDisplay(graphics::SceneAuthoring& scene, graphics::RenderOptions& options) {
    graphics::ObjectSetup support;
    support.size = {0.4f, 3.f, 0.4f};
    support.position = {1.f, 0.f, 0.f};
    support.material.color = {0.2f, 0.4f, 0.7f};
    scene.addObject("support", support);

    graphics::ObjectSetup display;
    display.size = {2.f, 1.f, 0.2f};
    display.position = {2.f, 1.f, 0.f}; // World-authored, even with a parent.
    display.scale = {1.5f, 1.f, 0.5f};
    display.material.color = {0.9f, 0.6f, 0.2f};
    scene.addObject("display", display, "support");

    graphics::LayerSetup parentTurn;
    parentTurn.spinDegreesPerSecond = 20;
    scene.addLayer("support", "turn", parentTurn);
    graphics::LayerSetup hinge;
    hinge.pivot = {-1.f, 0.f, 0.f};
    hinge.spinAxis = {0.f, 0.f, 1.f};
    hinge.spinDegreesPerSecond = 45;
    scene.addLayer("display", "hinge", hinge);
    graphics::LayerSetup shift;
    shift.translation = {0.5f, 0.f, 0.f};
    scene.addLayer("display", "shift", shift);

    options.lighting = true;
    options.ambientColor = {0.2f, 0.2f, 0.2f};
    options.directionalIntensity = 0;
    graphics::PointLight key;
    key.position = {3.f, 5.f, 6.f};
    key.intensity = 2;
    scene.addLight("key", key);
    graphics::Camera camera;
    camera.position = {8.f, 5.f, 10.f};
    camera.target = {1.f, 0.f, 0.f};
    scene.setCamera(camera);
}

inline void attachedLightAndCamera(graphics::SceneAuthoring& scene, graphics::RenderOptions& options) {
    graphics::ObjectSetup carrier;
    carrier.position = {0.f, 0.f, 0.f};
    carrier.size = {1.f, 0.2f, 1.f};
    scene.addObject("carrier", carrier);
    graphics::LayerSetup turn;
    turn.spinDegreesPerSecond = 25;
    scene.addLayer("carrier", "orbit", turn);

    graphics::ObjectSetup sculpture;
    sculpture.shape = graphics::GLShape::Pyramid;
    sculpture.position = {0.f, 0.8f, 0.f};
    sculpture.material.color = {0.5f, 0.7f, 0.9f};
    sculpture.material.specularStrength = 0.8f;
    sculpture.material.shininess = 128;
    scene.addObject("sculpture", sculpture);

    options.lighting = true;
    options.ambientColor = {0.1f, 0.1f, 0.1f};
    options.directionalIntensity = 0;
    graphics::PointLight lamp;
    lamp.position = {3.f, 3.f, 0.f}; // Carrier-local position.
    lamp.color = {1.f, 0.8f, 0.5f};
    lamp.intensity = 2;
    scene.addLight("lamp", lamp, "carrier");
    graphics::Camera camera;
    camera.position = {6.f, 4.f, 0.f}; // Position, target and up are carrier-local.
    camera.target = {0.f, 0.8f, 0.f};
    scene.setCamera(camera, "carrier");
}
} // namespace source_examples
```
