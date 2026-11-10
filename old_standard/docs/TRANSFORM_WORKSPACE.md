# 월드 배치·객체 상속·지역 레이어

## 합성 계약

열벡터 기준으로 다음과 같이 합성한다. 레이어 목록에서 앞에 있는 항목은 뒤 항목의 좌표계를 제공한다. 정점에는 오른쪽 행렬부터 작용한다.

```text
W_child = W_parent × A_child × B_child × L1 × L2 × ... × Ln
B_child = T(basis) × R × S × T(-localBasis)
```

객체 상속은 Scene의 parent 관계다. `A`는 객체가 소유한 부모 기준 아핀 부착 행렬, `B`는 기존 SRT, `L`은 그 객체가 소유한 지역 레이어다. 별도 전역 그룹을 만들어 레이어를 여러 물체에 암묵적으로 적용하지 않는다. 공통 움직임이 필요하면 부모 객체에 레이어를 넣고 자식 객체를 연결한다.

## 전역 좌표로 부착 위치 지정

사용자가 입력한 월드 기준 배치 D를 다음과 같이 저장한다.

```text
A_child = inverse(W_parent) × D × inverse(B_child)
```

자기 레이어 L은 이 계산에서 바꾸지 않는다. 부모 W에는 부모 자신의 레이어까지 포함한다. `parent base`는 현재 배치와 자기 레이어를 보존해 연결하며 이후 부모의 움직임을 상속한다. `parent none`은 같은 방식으로 연결을 해제한다. `t X Y Z`는 자기 레이어 이전 기준 배치의 월드 원점이다. `r`과 `s`는 최종 피벗을 중심으로 월드 축에 상대 변환을 적용한다.

예: 부모가 월드 X=10, 자식이 X=12일 때 연결하면 부모 기준 간격은 2다. 이후 부모가 X=15로 이동하면 자식은 X=17이 된다. 자식 내부 `layer t 1 0 0`은 별도의 지역 변위다.

## 비균등 배율과 행렬 보존

`s 2 1 0.5`, `layer s 2 1 0.5`, 축별 SRT를 지원한다. `s`는 배율을 곱한다. 절대 SRT는 `srt 이름 TX TY TZ RX RY RZ SX SY SZ`이며 R은 도 단위, Rz×Ry×Rx 순서다. 반사 배율도 사용할 수 있다. 0배율과 역행렬이 없는 부착·레이어 행렬은 거부한다.

비균등 배율을 가진 부모와 회전한 자식을 합성하면 전단이 생길 수 있다. 부착과 지역 레이어는 4×4 행렬로 보존하며 균일 배율이나 SRT로 강제 환원하지 않는다. 렌더·피킹·소유 선은 Scene의 같은 월드 합성을 사용한다. 법선은 렌더러의 역전치 행렬, 반사 앞면은 determinant 기반 winding 보정을 따른다.

## 지역 레이어 작성과 의존 순서

```text
layer add offset
layer t 2 0 0
layer add tilt
layer r z 30
layer add shape
layer s 2 1 0.5
layer select shape
layer before tilt
layer list
```

각 객체 안의 목록이 곧 단일 의존 체인이다. 분기하는 레이어가 필요하면 별도 자식 객체로 만든다. 레이어 이름은 객체 안에서 고유하며 `none`은 예약어다. `layer reset`은 정적 행렬만 초기화하고 모션은 유지한다. `layer play off`는 시간을 멈추고, `layer enable off`는 그 레이어 기여를 제거한다. 레이어 순서 변경은 뒤에 있는 모든 지역 축에 영향을 준다.

`insert spin`, `orbit`, `linear`, `pendulum`은 새 지역 모션 레이어를 만든다. orbit의 월드 중심은 삽입 시점의 객체 프레임으로 변환해 저장한다. 나중에 부모를 움직이면 공전 중심도 그 객체와 함께 이동한다. 지정 중심을 계속 월드에 고정하는 제약 조건은 아니다.

## 소스

- `framework/include/Model.h`: Component_Poly의 attachment/localLayers.
- `framework/detail/Scene.cpp`: placementMatrix, setWorldPlacement, setParentPreserveWorld, setLocalLayers, worldMatrix.
- `standard/modes/AssemblyWorkspace.cpp`: 짧은 명령 해석, 레이어 작성, 변환 미리보기.
- `standard/modes/AssemblyRuntime.cpp`: 모션 샘플과 레이어를 행렬로 전달.
- `standard/modes/AssemblyPersistence.cpp`: 대상·레이어를 명시한 레시피와 부착 행렬 저장.
