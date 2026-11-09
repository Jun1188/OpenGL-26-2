# 변환 조합 레퍼런스

SRT와 중심 회전의 뜻을 알고 있다는 전제로, **요구 문장 → 저장할 값 → 적용할 공간·순서 → 실제 API**를 연결한다. 특정 실습·형상을 정답으로 고정하지 않는다.

| 요구 문장 | 저장할 값 | 연산·공간 | Standard / C++ |
|---|---|---|---|
| 이 점에 물체를 둔다 | 월드 기준 배치 D | inverse(부모W)×D를 부착에 저장 | t / Scene::setWorldPlacement |
| 부모 이동을 함께 따른다 | 부모 ID와 부착 행렬 | 부모 최종W×자식 부착 | parent / setParentPreserveWorld |
| 물체 내부 동작만 바꾼다 | 그 객체의 지역 레이어 목록 | 앞 레이어 결과를 뒤에서 상속 | layer / setLocalLayers |
| 회전한 축으로 이동한다 | 각도, 지역 이동량 | R×T | 회전 레이어 뒤 이동 레이어 |
| 이동 후 월드 축으로 돌린다 | 월드 피벗 C와 축 | T(C)×R×T(-C)를 왼쪽에 곱함 | r / TransformSystem::rotation |
| 한쪽 끝을 유지하며 늘린다 | 모델의 고정점, 축별 배율 | 피벗 유지 보정 후 비균등 S | pivot keep + s |
| 점 주위로 공전한다 | 축 직선, 시간별 각도 | T(C)×R(θ(t))×T(-C) | insert orbit / RotationMotion |
| 임의 축으로 회전한다 | 0 아닌 방향 벡터, 축 위 점 | 축 정규화 후 중심 회전 | insert spin-axis/orbit-axis / AxisLine |
| 자전과 공전을 따로 제어한다 | 서로 다른 레이어·각속도 | 공전 프레임 아래 자전 | insert orbit → insert spin |
| 주기적으로 왕복한다 | 진폭, 주기, 위상 | θ(t)=offset+A sin(2πt/P+phase) | insert pendulum / PendulumMotion |
| 시간에 따라 위치를 바꾼다 | from/to, 기간, 보간 함수 | p(u), u(t), 재생 방식 | insert linear / PathMotion |
| 일정 시간 뒤 동작을 추가한다 | 시작 시각, 지속 시간 | 활성 구간에서 행렬 평가 | MotionStep::startSeconds/durationSeconds |
| 누른 시간만큼 바꾼다 | 속도와 키 누름 상태 | 변화량=속도×dt | bind hold / update 콜백 |
| 조명·시점을 이동시킨다 | 광원 위치·카메라 시선 | 같은 객체 상속 후 렌더 파라미터로 전달 | light/camera / PointLight·Camera |
| 중력·원심력 등이 작용한다 | 질량, 기준 프레임 운동, 힘 | 가속도 계산 → 적분 → 위치 변환 | force / DynamicsSystem |

## 행렬을 직접 끼우는 공통 API

다음 조각은 이미 생성한 `scene`과 `id`를 사용한다. 행렬 값은 실제 문제에서 계산해 넣는다. 클래스·문서·명령 파서를 복사할 필요가 없다.

```cpp
auto T = graphics::TransformSystem::translation({2,0,0});
auto R = graphics::TransformSystem::rotation({{0,0,0},{0,1,0}}, 30);
auto S = graphics::TransformSystem::scaling({2,1,0.5f});
if (T && R && S) {
    // Each matrix is a dependent local layer. Geometry receives S, R, T in that order.
    bool ok = scene.setLocalLayers(id, {*T, *R, *S});
}
```

부모와의 관계는 객체의 부착 행렬, 내부 조합은 자기 레이어에 저장한다. 월드 기준 배치를 설정할 때 자기 레이어를 다시 초기화하지 않는다. 비균등 배율과 회전의 합성으로 생긴 전단은 행렬 상태로 유지한다.

## 시간·조건을 문제에 맞게 작성

```cpp
graphics::Motion motion;
graphics::LinearMotion move;
move.to = {3,0,0}; move.durationSeconds = 2;
move.space = graphics::TransformSpace::Local;
bool moving = motion.append(graphics::Motion::linear(move));
graphics::RotationMotion turn;
turn.degreesPerSecond = 45;
turn.space = graphics::TransformSpace::Local;
auto rotate = graphics::Motion::rotation(turn);
rotate.startSeconds = 2;
bool turning = motion.append(rotate);
auto sample = moving && turning ? motion.sample(time) : std::nullopt;
if (sample) {
    bool ok = scene.setLocalLayers(id, {sample->parent, sample->local});
}
```

위 이동은 Once 재생이므로 끝 위치를 유지하고, 회전은 2초부터 시작한다. `durationSeconds`로 단계 자체를 끝내면 그 단계의 기여도 사라진다. `then`은 앞 단계들의 종료 시각이 모두 유한할 때만 사용할 수 있다. 조건에 따라 기준점·각속도·레이어를 바꾸려면 업데이트 콜백에서 조건을 판단하고 해당 값만 갱신한다. sample은 행렬 평가이고 의미 action 실행은 별도 계약이다.

## 여러 물체의 기준점·질량·힘

형상 크기와 부모 배치만으로 물리 질량·관성·제약이 자동 결정되지는 않는다. 기준 프레임 운동, 힘 법칙, 적분 상태, 시각적 형상을 별도 값으로 둔다. [사용자 힘 연결](APPLICATION_RECIPES.md)에서 콜백을 바꾸면 스프링·외력·감쇠 등을 조합할 수 있다. 저울·회전 구조물·매달린 추 등 문제 형상은 이 값들을 사용하는 응용 코드다.

## 빠른 소스 위치

- `modes/AssemblyWorkspace.cpp`: 선택 대상 해석, 피벗, 축별 배율, 객체 내부 레이어, 순서 변경, 템플릿 삽입.
- `framework/include/Transform.h`, `detail/Transform.cpp`: 이동·배율·임의 축 회전·반사 행렬.
- `framework/include/Motion.h`, `detail/Motion.cpp`: 시간 함수와 단계 삽입·활성 구간.
- `framework/detail/Scene.cpp`: 부착·상속·레이어·렌더/피킹의 공통 월드 합성.
- `framework/include/Dynamics.h`, `detail/Dynamics.cpp`: 기준 프레임 가속도·힘·적분.
- `modes/AssemblySceneTools.cpp`: 같은 변환을 광원·카메라에 연결.

[연산별 실행 템플릿](TRANSFORM_TEMPLATES.md) · [전체 명령과 인수](QUICK_COMMANDS.md) · [실습 소스 참조](REFERENCE_PRACTICES.md)
