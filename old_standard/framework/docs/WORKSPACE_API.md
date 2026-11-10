# 객체 부착·지역 레이어·힘 API

공통 API는 문서·Standard 명령 파서 없이 사용할 수 있다. 공개 진입점은 Framework.h다.

| API | 입력 → 결과·실패 계약 |
|---|---|
| Scene::placementMatrix(id) | 부모 최종 변환 × 객체 부착 × 기준 SRT. 자기 지역 레이어 이전의 월드 배치. 잘못된 ID/행렬은 빈 optional |
| Scene::setWorldPlacement(id,D) | D를 부모 기준 아핀 부착으로 변환. 자기 레이어 유지. 특이행렬/없는 ID는 false |
| Scene::setParentPreserveWorld(id,parent) | 순환 검사 후 배치를 유지해 연결. 실패 시 기존 부모·부착 유지 |
| Scene::setAttachment(id,A) | 부모 기준 행렬 직접 설정. 유한하고 역행렬 있는 아핀 행렬 필요 |
| Scene::setLocalLayers(id,matrices) | 순서대로 지역 합성. 최대128, 실패 시 기존 목록 유지 |
| RenderOptions::pointLights | 월드 점광원 목록. 기본 셰이더 최대8, 색·강도·거리 감쇠 |
| DynamicsSystem::effectiveAcceleration | 강체 기준, 지역 위치·속도, 월드 중력 → 지역 가속도 |
| DynamicsSystem::step | 점질량·힘 콜백·dt≤0.25초 → 상태 적분. 비유한 결과는 미반영 |
| DynamicsSystem::holdingForce | 그 위치를 기준 프레임에 고정할 때 필요한 지역 힘 |
| DynamicsSystem::normalLoad | 정지 구속 힘을 지정 법선에 투영한 압축력. 실제 접촉 솔버가 아님 |
| DynamicsSystem::spring | 지역 앵커·양수 강성·0이상 감쇠 → 힘 콜백. 잘못된 값은 빈 함수 |

기존 MotionSystem의 공유 모션 레이어와 Component_Poly의 객체 내부 localLayers는 별개다. 새 객체 작성은 parent + attachment + localLayers 계약을 사용한다. 기존 AnimationSystem::poseHierarchy는 여전히 양수 균일 기준 자세와 강체 모션을 전제하는 호환 API다. 비균등·전단 결과를 이 API로 분해하지 않는다.

```cpp
auto parent = scene.addGroup();
auto child = scene.addCuboid({1,1,1}, {2,0,0});
if (parent && child) {
    bool attached = scene.setParentPreserveWorld(*child, *parent);
    auto turn = graphics::TransformSystem::rotation({{0,0,0},{0,1,0}}, 30);
    auto scale = graphics::TransformSystem::scaling({2,1,0.5f});
    if (attached && turn && scale) scene.setLocalLayers(*child, {*turn, *scale});
}
```

모든 행렬은 4×4 아핀 행렬이다. setLocalLayers는 결과를 SRT로 재분해하지 않는다. 렌더와 피킹은 Scene::worldMatrix를 공유한다.
