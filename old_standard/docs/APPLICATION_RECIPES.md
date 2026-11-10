# 응용 장면을 짧게 작성하기

[명령어](QUICK_COMMANDS.md) · [좌표와 레이어](TRANSFORM_WORKSPACE.md) · [확인 절차](../USER_CHECKS.md)

## 부모 이동과 자식의 지역 회전

```text
clear
add joint base at 0 0 0
insert linear 3 0 0 4
add box arm at 2 0 0
parent base
insert spin z 30
```

base의 이동은 arm에 상속된다. arm의 자전은 자기 지역 레이어다. 부모의 위치 제어와 자식의 회전 코드를 한 업데이트 함수에 섞지 않는다.

## 이동 가능한 조명

```text
light point lamp at 3 4 2
light color 1 0.8 0.5
light intensity 2
insert orbit y 30 center 0 0 0
```

점광원은 표시용 메시가 있어 클릭할 수 있다. t/r/s·부모·지역 레이어로 위치를 바꾼다. 기본 셰이더는 점광원 최대 8개다. `light sun 0`은 기존 방향광을 끈다. 점광원 강도 0은 해당 광원을 끈다. 거리 감쇠는 `1/(1+attenuation*distance²)`다.

## 카메라 추가 후 이동

```text
camera add observer at 6 4 8
camera look 0 0 0
camera use
t 5 3 7
camera fov 50
camera free
```

카메라도 같은 객체 상속과 레이어를 사용한다. 시선은 월드 목표점으로 입력한다. 상속된 배율·전단은 시선/up을 정규직교화해 카메라 자세에서 제거한다. near/far/FOV를 형상 배율로 자동 변경하지 않는다. 활성 카메라의 표시 메시는 자기 시야를 가리지 않도록 숨긴다. `camera free` 후에는 일반 시점의 camera move/yaw/orbit/zoom 명령을 사용할 수 있다.

## 이동·회전하는 스프링 저울

```text
load examples/assembly/scenarios/s13-moving-scale.assembly
force read load
pause on
```

이 예제는 **영자연길이의 3차원 선형 스프링과 점질량** 모델이다. 실제 시험의 저울 형상을 재현한 것은 아니다. 강체 carrier의 공전 레이어 → 기준 좌표계 가속도 → 중력·관성항·스프링 힘 → 추 위치 → 렌더 순서다. 선 길이와 기울기가 변하며 스프링 힘을 N으로 출력한다. 충돌·마찰·양팔 저울의 강체 토크는 자동 추가하지 않는다.

지역 유효 가속도는 `Rᵀ(g-a_origin) - ω×(ω×r) - 2ω×v - α×r`다. ω와 α는 지역 좌표로 바꾸며 단위는 rad/s와 rad/s²다. 콘솔 회전 입력은 도/초다. Standard는 프레임 이동·회전의 유한차분으로 가속도를 근사한다. C++에서는 ReferenceFrame에 해석적으로 계산한 값을 직접 넣을 수 있다.

`force read`의 스프링 힘은 실제 현재 스프링/감쇠 힘의 크기다. 같이 표시하는 접촉 반력은 **그 위치에 정지하도록 구속한다고 가정한** +Y 압축력이다. 움직이는 추의 실제 접촉력과 같다고 해석하지 않는다.

## C++에서 사용자 힘 추가

```cpp
graphics::ReferenceFrame frame;
graphics::ParticleState particle;
particle.mass = 1.0f;
auto spring = graphics::DynamicsSystem::spring({0, 0, 0}, 40.0f, 3.0f);
graphics::ForceFunction force = [spring](const graphics::ParticleState& p, double t) {
    return spring(p, t) + glm::vec3(float(2 * std::sin(t)), 0, 0);
};
bool ok = graphics::DynamicsSystem::step(particle, frame, dt, {0, -9.8f, 0}, force, time);
```

콜백은 **지역 뉴턴값**을 반환하는 순수 함수로 작성한다. 질량으로 나누는 처리는 step이 맡는다. 외부 파일·공유 상태를 변경하는 부수 효과는 복구하지 않는다. 기본 적분은 최대 1/240초 간격의 semi-implicit Euler이며 강성·질량·프레임 속도에 따른 수치 오차가 있다. 힘 기준 프레임은 강체여야 한다. 비균등 배율은 시각적 메시 자식에 적용한다.
