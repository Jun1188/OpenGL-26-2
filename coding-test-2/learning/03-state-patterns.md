# 상태·입력·시간에서 패턴 만들기

기믹마다 먼저 상태 표를 만든다. angle, direction, playing, selected, step, radius를 구분한다. 렌더 함수 안에서 각도를 증가시키면 렌더 패스 수에 따라 속도가 달라지므로 시간 갱신은 프레임당 한 번 수행한다.

## 1. 토글·유지 입력

```cpp
int toggleDirection(int current, int requested) {
    return current == requested ? 0 : requested;
}
// PRESS에서만 토글한다. REPEAT에서는 다시 뒤집지 않는다.
if (action == GLFW_PRESS && key == GLFW_KEY_Y)
    direction = toggleDirection(direction, (mods & GLFW_MOD_SHIFT) ? -1 : 1);
// 유지형 이동은 PRESS/REPEAT에 true, RELEASE에 false를 기록한다.
held = action != GLFW_RELEASE;
```

GLFW의 key 값은 대문자 키 코드다. y/Y의 구분은 Shift modifier로 한다. 포커스를 잃으면 held 상태를 해제해 키를 놓은 이벤트를 받지 못해도 계속 이동하지 않도록 한다. 토글 애니메이션과 유지 입력은 별개 계약이다.

## 2. 속도를 바꿀 때 위치가 튀지 않게 하기

```cpp
angle = std::remainder(angle + direction * degreesPerSecond * dt, 360.0);
float a = glm::radians(float(angle));
glm::vec3 position = center + glm::vec3(radius*std::cos(a), height, radius*std::sin(a));
```

angle 자체가 상태이므로 정지·반전해도 현재 위치에서 이어진다. 반대로 매번 angle=새속도*전체경과시간으로 계산하면 속도를 바꾼 순간 위치가 튈 수 있다. 절대 시간 함수는 고정된 모션을 재현할 때, dt 적분은 조작에 따라 속도가 달라질 때 유용하다. 도·라디안과 초 단위를 섞지 않는다.

왕복각은 연속 위상이 있을 때 amplitude*sin(phase)로 만들 수 있다. 일정 속도 왕복이 필요하면 PingPong 샘플을 사용하거나 구간을 반사한다. sin 왕복은 끝에서 느려지고 중심에서 빠르므로 두 운동은 동일하지 않다.

## 3. 단일 상태로 여러 출력을 만들기

radius, center, height로 광원 위치와 원 궤도, 표시 도형을 함께 계산한다. 광원에만 새 반경을 쓰고 궤도에는 옛 반경을 쓰면 두 출력이 분리된다. 부모가 있다면 이 값이 부모 지역 좌표인지 월드 좌표인지도 함께 적는다.

```cpp
auto point = [&](double radians) {
    return center + glm::vec3(radius*std::cos(radians), height, radius*std::sin(radians));
};
light.position = point(phase);
markerWorld = parentWorld * glm::translate(glm::mat4(1), light.position);
```

원 궤도의 모든 점도 같은 point 함수와 같은 parentWorld로 변환한다. 예제의 작은 광원 표시는 unlit 재질이다. 빛을 내는 위치의 표시와 장면을 실제 비추는 PointLight는 별개의 출력이다.

## 4. 복합 상태의 작성 순서

```text
입력 → 상태 변경 → 사용자 변환/레이어 → Scene 기본 모션
→ 최종 월드 행렬로 광원·카메라·표시 동기화 → render
```

Standard의 SceneAuthoring::syncTools와 ExamController::syncVisuals는 render 직전에 호출된다. native Scene 모션이 update 콜백 뒤에 진행되므로 이 시점이 필요하다. sync에서는 dt를 더하지 않는다. Space는 제공 작업공간의 컨트롤러/작성 시계를 정지한다. 사용자가 별도로 만든 MotionId는 setMotionPlaying 등으로 함께 제어해야 한다.

N04/N05와 N09/N10에서 토글·반경·시점 변화를 연습한다. 상태 하나를 여러 함수가 동시에 쓰게 하지 말고, 상태의 소유자와 출력이 덮어써지는 순서를 정한다.
