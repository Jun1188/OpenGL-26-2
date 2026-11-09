# 육면체와 면의 변환: 독립·계층과 회전 종류

## 서로 다른 두 설정

FaceTransformMode는 육면체/조상/모션 변환을 따를지를 정한다.
FaceRotationKind는 어느 점·선을 중심으로 면을 회전시킬지를 정한다. 두 설정은 독립적으로 조합한다.
기존 Mesh.independence는 면 자체 SRT의 활성 플래그이며 월드 독립 모드가 아니다. 기존 실습은 Hierarchy가 기본이다.

| 변환 모드 | 육면체가 이동·회전·크기 변환할 때 | 용도 |
|---|---|---|
| Hierarchy | 면도 따라 움직이며 면 자체 동작을 추가 | 육면체를 돌리면서 펼치기 |
| Independent | 전환 시 고정한 기준에서 면 자체 동작만 수행 | 본체가 움직여도 분리된 면은 남기기 |

```cpp
if (!scene.setFaceTransformMode(box, face, graphics::FaceTransformMode::Independent)) return false;
// 이 시점의 면 자세를 유지한다. 이후 box와 조상/모션의 변환을 따르지 않는다.
if (!scene.setPosition(box, {2, 0, 0})) return false;
// 본체가 움직여도 독립 면은 원래 월드 기준에 남는다.
if (!scene.setFaceTransformMode(box, face, graphics::FaceTransformMode::Hierarchy)) return false;
// 다시 연결하는 순간의 자세는 유지하며 이후 본체 움직임을 따른다.
```

preserveWorld=true가 기본이다. 현재 기준을 보정 행렬로 보관하므로 모드 전환 순간에 면이 뛰지 않는다.
preserveWorld=false는 보정 기준을 항등으로 초기화한다. Independent에서는 모델 좌표가 곧 월드 좌표가 되므로 위치가 바뀔 수 있다.
재연결할 본체 월드 행렬이 특이하면 자세 보존을 거부하고 기존 면 상태를 유지한다.
독립은 변환 상속만 분리한다. 면 형상 편집·모델 교체·부모 엔티티 표시/삭제까지 분리된 별도 엔티티가 되는 것은 아니다.
DiagramSystem::faceModelMatrix는 단일 컴포넌트만 합성한다. 조상과 MotionSystem을 포함한 실제 월드 결과는 Scene::faceWorldMatrix로 조회한다.
렌더러와 피킹은 같은 faceWorldMatrix를 사용한다.

## 회전 세 종류

| FaceRotationKind | 축 위치 | 방향 입력 | 결과 |
|---|---|---|---|
| EdgeHinge | 원래 면의 edge번 모서리 선 | 현재 면 SRT를 적용한 두 끝점으로 계산 | 모서리를 고정하고 펼치거나 접음 |
| SelfSpin | 현재 면 SRT를 적용한 면 정점 중심 | axis: 모델 기준 X/Y/Z 또는 사용자 방향 | 면 중심 위치를 유지하며 자전 |
| ModelOrbit | 면 변환 전 원본 모델 로컬 bounds의 중심 | axis: 모델 기준 X/Y/Z 또는 사용자 방향 | 원본 모델 중심 주위로 면의 위치·방향이 함께 회전 |

현재 육면체의 각 면은 네 정점의 순환 경계다. edge=0..3은 정점 i → (i+1)%4 선이다. 다른 Mesh에는 경계 순서가 있는 다각형에만 적용한다.
axis는 방향이며 축 위치가 아니다. 0 벡터와 비유한 각도·축, 잘못된 인덱스는 거부한다.
degrees는 절대 각도다. 매 프레임 dt 각도를 누적해서 API에 전달한다. API 자체는 이전 회전에 계속 곱하지 않는다.

```cpp
graphics::FaceRotationConfig rotation;
rotation.kind = graphics::FaceRotationKind::EdgeHinge;
rotation.edge = 0;
rotation.degrees = 90;
if (!graphics::PatternSystem::rotateFace(scene, box, face, rotation)) return false;

rotation.kind = graphics::FaceRotationKind::SelfSpin;
rotation.axis = {0, 1, 0}; // 면 중심을 지나며 모델 Y축과 평행한 축.
rotation.degrees = 45;
if (!graphics::PatternSystem::rotateFace(scene, box, face, rotation)) return false;

rotation.kind = graphics::FaceRotationKind::ModelOrbit;
rotation.axis = {0, 1, 0};
rotation.degrees = 180;
if (!graphics::PatternSystem::rotateFace(scene, box, face, rotation)) return false;
```

새 rotateFace는 현재 면 SRT를 고려하여 축을 계산하고 모델 기준 회전 행렬을 설정한다.
면 SRT의 위치가 바뀌어도 ModelOrbit의 회전 중심은 원본 모델 중심이다.
힌지·자전은 SRT 변경 뒤 같은 설정으로 rotateFace를 다시 호출해 새 축 위치를 계산한다.
독립 모드의 모델 기준 축은 고정된 transformFrame을 기준으로 해석한다. 본체의 현재 움직임을 다시 읽어 따라가지 않는다.
원본 모델 중심은 현재 polyhedron의 원래 정점 bounds에서 계산하며 화면 투영 중심이나 면 중심이 아니다.
setModel/setSideLengths로 원본 형상을 편집하면 새 형상을 기준으로 회전을 다시 설정한다.

## 이동·SRT·애니메이션 합성 순서

W는 엔티티·조상·모션의 월드 행렬, F는 모드 전환 보정/고정 기준, S는 면 SRT, A는 추가 애니메이션 행렬이다.

- Hierarchy: W × F × 면 로컬 결과
- Independent: F × 면 로컬 결과
- FaceAnimationSpace::Local: S × A. 기존 setFaceLocalMatrix/foldFace/openFace의 기본 동작이다.
- FaceAnimationSpace::Model: A × S. 새 rotateFace가 이 순서를 사용한다.

즉 S로 면을 이동시킨 뒤 모델 중심 공전 A를 적용하면, 이동한 면 전체가 원본 모델 중심을 돈다.
힌지와 자전은 이동된 면 정점에서 축 위치를 구하므로 해당 선/중심을 유지한다.
setFaceSRT로 면의 이동·회전·크기·피벗을 설정할 수 있다. 이 함수는 면 SRT를 활성화하며 변환 모드는 바꾸지 않는다.
일반 변환 행렬은 setFaceLocalMatrix의 space 인자로 합성 위치를 지정한다. 단일 애니메이션 행렬을 교체하므로 여러 동작 합성은 직접 행렬을 조립한다.

```cpp
graphics::Transform3D facePose;
facePose.basis = {0, .2f, 0};
if (!scene.setFaceSRT(box, face, facePose)) return false;
rotation.kind = graphics::FaceRotationKind::ModelOrbit;
if (!graphics::PatternSystem::rotateFace(scene, box, face, rotation)) return false;
```

## 모드 초기 설정과 의미 동작 등록

```cpp
graphics::CuboidConfig config;
config.name = "box";
config.faces[0].mode = graphics::FaceTransformMode::Independent;
config.faces[0].transform.basis = {0, .2f, 0};
config.faces[0].rotation = rotation;
if (!mode->setCuboid(config)) return false; // 반영은 mode.apply(app).

if (!motion.insertAction(1, graphics::SceneActions::faceRotation(scene, box, face, rotation))) return false;
if (!motion.insertAction(2, graphics::SceneActions::faceMode(scene, box, face,
        graphics::FaceTransformMode::Independent))) return false;
```

faceMode는 기본 자세 보존으로 같은 모드에 반복 적용하면 현재 고정 기준을 다시 바꾸지 않는다.
참조 수명·ID 재발급·sample/execute의 구분은 [의미 동작 문서](MODE_ACTIONS.md)를 따른다.

## 사용자 확인 절차

1. 본체와 면이 보이는 육면체 두 개를 만든다. 한 면씩 같은 45도 힌지 회전을 적용한다.
2. 첫 육면체 면은 Hierarchy, 두 번째 면은 Independent로 설정한다. 본체 이동·회전·크기만 바꾼다.
3. 계층 면은 본체를 따르고 독립 면은 전환 시 기준을 유지해야 한다. 두 경우 모두 면의 추가 이동/회전은 동작해야 한다.
4. 힌지 회전은 두 끝점이 고정, 자전은 면 중심이 고정, 공전은 면 중심이 원본 모델 중심 주위로 이동하는지 확인한다.
5. 면을 먼저 옆으로 이동한 뒤 ModelOrbit을 적용한다. 이동된 면이 원본 모델 중심을 돌아야 한다.
6. 모드 전환 시 자세 보존, 잘못된 edge·0축 거부, 피킹과 화면 일치를 확인한다.

빌드·테스트·실행은 하지 않았다. 기존 실습의 기본 계층 동작 확인도 사용자에게 맡긴다.

모드 전환 보정 F가 있으면 회전 축과 원본 모델 중심도 그 보정된 기준에서 해석한다.
현재 본체의 모델 좌표로 기준을 초기화하려면 setFaceTransformMode(..., Hierarchy, false)를 호출한다. 이때 자세가 달라질 수 있다.
육면체 모드 초기 구성은 모션/레이어의 0초 자세를 연결한 뒤 독립 면 기준을 고정한다.
