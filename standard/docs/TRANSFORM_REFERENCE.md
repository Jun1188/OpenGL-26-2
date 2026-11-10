# 객체 월드 배치와 지역 레이어

`ObjectSetup`은 `size`로 모델을 만든 뒤 다음 월드 배치를 지정한다.

```text
D = T(position) × Rz × Ry × Rx × S(scale) × T(-pivot)
attachment = inverse(parentWorld) × D
W = parentWorld × attachment × L1 × L2 × ...
```

기본 SRT는 단위행렬이며 attachment에 배치가 들어간다. 부모는 자식 등록 전에 존재해야 한다. 연결 순간에는 지정한 월드 배치를 유지하고 이후 부모 움직임을 상속한다. `size`는 모델 형상 크기, `scale`은 변환 배율이므로 지역 레이어의 이동에도 scale의 영향이 남는다.

각 `LayerSetup`의 행렬은 다음과 같다.

```text
L = T(translation + pivot) × Rz × Ry × Rx × Rspin(seconds) × S(scale) × T(-pivot)
```

translation과 pivot은 선행 레이어의 지역 좌표다. spin은 초당 도 단위이며 `spinAxis`를 정규화한다. 회전 순서를 명시하고 절대 시간에서 계산하므로 프레임별 각도 누적을 사용하지 않는다. 객체 pivot은 `T(-pivot)`으로 모델 원점을 재지정한다. 지역 레이어 pivot은 `T(pivot)...T(-pivot)`으로 해당 점 주위 변환을 수행한다. 피벗 숫자만 수정할 때 기존 화면 배치를 자동 보존하지 않는다.

비균등 배율과 회전을 계층으로 합성한 전단을 행렬 그대로 유지한다. 0 배율, 유한하지 않은 값, 특이 변환은 거절한다. `addLayer` 등록 순서가 의존 순서다. 특정 사물·시험 문제에 고정된 그룹이 아니다.

부모가 있는 점광원의 position과 카메라 position/target/up은 부모 지역 좌표다. 점광원은 점을 월드로 변환한다. 카메라는 `CameraSystem::attached`로 시선/up을 직교화한다. 광원 색·강도·거리 감쇠는 형상 배율과 별개다.
