# 처음 보는 사물과 기믹을 작성하는 방법

**공통 프레임워크는 연산을 제공하고, 사용자 코드는 그 연산을 선택하는 상태와 규칙을 제공한다.** 사물 목록을 늘리는 방식으로 확장하지 않는다. 실습 23까지의 내용에서 저울대 위를 이동하는 저울 같은 새로운 문제가 나왔다는 것은 전이 사례다. 저울 자체를 고정 대비 대상으로 삼지 않는다.

[응용 N제 12문제](exercises/README.md) · [연산 템플릿](TRANSFORM_TEMPLATES.md) · [실습 24·26·27 소스](PRACTICE_24_PLUS.md)

## 요구를 코드 책임으로 나누기

| 요구의 종류 | 사용자 코드가 정할 값/규칙 | 기존 연결 지점 |
|---|---|---|
| 새로운 사물 | 정점·인덱스·부품·재질 | Mesh/Model, Scene::addModel/setMesh, Assembly::model |
| 공통 이동과 내부 이동 | 부모·부착·지역 행렬 순서 | setParentPreserveWorld/setWorldPlacement/setLocalLayers |
| 시간에 정해진 움직임 | 매개변수·기간·활성 조건 | Motion/DistancePath 또는 Behavior::custom |
| 처음 보는 입력/기믹 | 상태·상태 전이·조건·목표값 | customCommand, Behavior::key/mouse/cursor/focus |
| 부착된 시점/광원 | 기준 프레임·지역 시선·색·반사율 | CameraSystem::attached, PointLight, Material |
| 힘에 따른 움직임 | 질량·힘 법칙·기준 프레임·해제 규칙 | ForceFunction, ReferenceFrame, DynamicsSystem |
| 새 제약/강체 운동 | 자유도·토크·관성·접촉/제약 해법 | 사용자 모델 또는 반복 사용할 최소 공통 연산을 별도 추가 |

새 기믹은 먼저 입력 → 상태 → 출력으로 적는다. 예를 들어 레일상의 이동 거리, 끝점 대기, 다음 입력의 방향은 새로운 제어 규칙이다. 이를 단순한 무한 자전이나 자동 왕복으로 바꾸면 요구가 달라진다. 힘 때문에 기울어지는 물체를 정해진 사인 각도로 대신하는 것도 같은 문제다.

## 실제 C++ 연결 예: 회전 프레임 위의 이동 탐침

[MovingBeacon.h 원문](../examples/extensions/MovingBeacon.h)은 기존 실습의 객체가 아닌 회전 검사 장치다. 프레임이 회전하는 동안 탐침이 그 위를 왕복하고, 탐침에 붙은 카메라와 광원이 함께 움직인다. 입력을 처리하는 새 런타임 모드나 공통 Effect enum은 추가하지 않았다.

`modes/AssemblyExtensions.cpp`에서 다음 include와 분기를 `customCommand`에 추가하면 `beacon rig`로 생성할 수 있다. 이 문서는 참조이며 해당 분기를 현재 런타임에 자동 등록하지 않는다.

```cpp
#include "../examples/extensions/MovingBeacon.h"

// customCommand(Assembly& scene, const Tokens& args, std::ostream&) 내부
if (args[0] == "beacon") {
    require(args.size() == 2, "beacon <새 이름>");
    addMovingBeacon(scene, args[1]);
    return true;
}
```

등록·사용자 재빌드 후 `beacon rig`, `camera use rig.view` 순서로 입력한다. 생성 함수는 C++ API만 사용하고 내부 execute로 명령을 중복 기록하지 않는다. 바깥 생성 명령 하나와 이후 카메라 선택 명령으로 레시피를 재구성한다. 화면 속 카메라 표시 도형은 해당 카메라를 사용할 때 숨겨진다.

작성된 예제의 핵심은 다음과 같다. 카메라·광원의 상속은 일반 객체 부착을 사용한다. 탐침의 이동 함수만 사용자 코드다.

```cpp
behavior.effect = assembly::Effect::Custom;
behavior.positionDriver = true;
behavior.custom = [frame, probe](graphics::Scene& scene,
        const graphics::Transform3D&, double seconds, float) {
    const auto parent = scene.worldMatrix(frame);
    if (!parent) return false;
    const float x = float(-3 * std::cos(seconds * 2 * std::acos(-1.0) / 4));
    return scene.setWorldPlacement(probe,
        *parent * glm::translate(glm::mat4(1), glm::vec3(x, .4f, 0)));
};
```

이 함수는 주어진 시각의 위치를 계산하는 운동학 예제다. 물리 법칙을 적분한 예제가 아니다. 끝점 정지·입력 재출발이 필요하면 거리와 대기 상태를 따로 보관한다. 힘을 적용하려면 같은 출력 자리에 Dynamics의 계산 결과를 연결한다.

## 동작 소유권과 갱신 순서

1. 하나의 위치 상태는 한 제어기가 쓴다. Custom/적분 물리와 일반 Motion이 같은 루트 SRT를 덮어쓰지 않게 한다. 자전·표시 배율을 추가할 때는 별도 자식에 둔다.
2. 현재 Assembly는 일반 객체/지역 레이어를 합성한 뒤 상태 동작을 적용하고, 마지막에 광원·카메라를 동기화한다. 여러 사용자 제어기 사이에 의존성이 있으면 한 컨트롤러 안에서 순서를 정하거나 등록 순서를 명시한다. 임의의 사용자 콜백을 자동 위상 정렬하는 기능은 없다.
3. 시간 함수는 seconds를 사용하고 적분은 dt를 사용한다. 렌더 콜백에서 시간을 중복 진행하지 않는다. pause 또는 dt=0에서 적분 상태를 바꾸지 않는다.
4. 씬 ID는 clear 뒤에 재사용된다. 장면을 다시 만들면 캡처한 ID도 새로 등록한다. std::function이 공유 포인터/외부 참조를 캡처하면 Assembly의 상태 복사가 외부 부작용까지 되돌리지 않는다. 실패 전에 검증하거나 독립 상태를 값으로 소유한다.
5. C++ 확장 소스 변경 후 재빌드는 사용자가 수행한다. `.assembly` load/hotload는 명령을 다시 읽는 기능이며 C++ 컴파일러가 아니다. 저장 레시피는 생성 명령을 다시 실행하고 애니메이션 시간을 처음부터 시작한다. 임의 C++ 상태의 현재 프레임 저장은 별도로 작성한다.

## 힘과 새 제약

`DynamicsSystem`은 질점·힘 콜백·회전 기준 프레임을 제공한다. 새로운 사물에서 필요한 토크, 관성, 레일 구속, 강체 접촉이나 관절 제약이 저절로 생기지는 않는다. 먼저 문제의 자유도와 물리 가정을 정하고 필요한 상태 방정식을 사용자 컨트롤러에 작성한다. 여러 기믹에서 같은 연산이 반복될 때 그 연산만 공통 API로 올린다.

예를 들어 위치를 지정한 레일 이동과 힘 때문에 레일을 미끄러지는 운동은 다르다. 후자는 힘의 레일 접선 성분, 질량, 속도, 끝점 제약을 정해야 한다. 재사용 대상은 특정 사물 클래스가 아니라 이 계산과 좌표 변환이다. [E27-04](exercises/P27.md#e27-04---회전-캡슐에서-분리되는-발광-탐사-입자-c)는 해제 전후 상태 공간이 바뀌는 예다.

## 새 실습이 나올 때

PDF 요구사항을 전부 대응표에 등록 → 독립 실습 구현 → 재사용할 연산/확장 지점 정리 → 다른 사물·기믹의 N제 추가 → 최종 소스 참조와 목차 갱신 순서로 진행한다. 문제 ID와 이전 문제를 유지한다. 문서·문제의 생성과 실제 실행 확인은 구분한다. 이 절차는 저장소 AGENTS.md에도 기록했다.
