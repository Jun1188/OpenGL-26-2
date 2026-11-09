# 응용 패턴은 의미 단위 API의 조립이다

## 선택 기준

모양이나 실습 번호마다 새 함수를 만들지 않는다. 생성·좌표 변환·시간 평가·상태 변경·연결·표시의 의미를 먼저 나눈다.
새 수학 계산, 여러 컴포넌트에 걸친 일관성, 재사용할 상태가 있으면 구현 API를 둔다.
그 API들의 단순 호출 순서와 상수만 다른 경우에는 이 문서의 조립법으로 표현한다.

## 주요 실습 패턴과 API 대응

| 실습·목적 | 의미 단위 조립 | 유지하는 이유 또는 문서 조립 경계 |
|---|---|---|
| 13 면 선택·표시 | addCuboid → setFaceVisible / SceneActions::faceVisible | 6면의 표시 정책은 데이터로 설정 |
| 14 이동·회전·형상 교체 | setTransform / Motion::rotation → setModel | 형상 교환 조합은 아래 문서 함수로 구현 |
| 15~16 면 펼침·닫힘·순차 동작 | splitFace → AxisLine → foldFace → 시간에 따른 각도 | 힌지 행렬은 foldFace, 면 순서·각도·시간은 응용 데이터 |
| 17 자전·공전·스케일·반사 | rotation(Local/World) + fixed(reflect...) + 레이어 연결 | 단일 자전·공전은 createSpin/createOrbit, 복합은 createMotion 단계 목록 |
| 18 천체 계층·경로 | addGroup/setParent → 궤도 회전 → 자식 위치 → 자전 | 표시 크기는 메시/자식에 두어 궤도 반지름과 분리 |
| 19 부품 계층·진동·교환 | addBox/addJoint → setParent → pendulum/linear/path | 포신·깃대 같은 이름별 API는 만들지 않음 |
| 20 컨베이어·격자·번호·거리 | GridLayout + DistancePath + snakeCells + conveyorCornerPose + addNumber | 거리·코너 접선·숫자 선분 생성의 계산은 유지 |
| 21 회전 방·반사·낙하 | addRoom → stepRoomBody → position → setPosition | 로컬→월드 이탈 및 속도·중력 상태를 공통 구현으로 유지 |
| 22 경로 진행·방향·다중 뷰 | DistancePath::sample → follow → threeViews/renderViews | 거리 접선·yaw와 viewport 구성 책임을 유지 |
| 23 보행·점프·벽 개방 | addWalkingRig + stepCharacter/jump + poseWalkingRig + cameraFacingWall/openRoomWall | 접지·장애물·보행 위상·벽 판정은 재사용 계산 |
| 사람형·관절·그네 | addHumanoid/poseHumanoid + createJointAnimation/poseHierarchy + addSwingWithRider | 팔꿈치·무릎·착석/좌석/진자 계층을 일관되게 구성 |
| 모든 육면체 모드 | CuboidModeConfig → bindCuboidMode → CuboidCommand/SceneActions | 초기 데이터·연결·입력을 편집하며 Scene/ECS를 그대로 사용 |

루트 framework 기준의 현재 구현 목록이다. 육면체 전용 모드는 새 모드의 생성 대상이며 기존 실습의 사각뿔·구·원뿔 API는 기존 호출부를 위해 유지한다.
이전 실습의 전용 Console 입력·상수까지 모두 모드 API로 재작성하지 않는다. 기초 API와 아래 조립법으로 같은 주요 패턴을 구성한다.
addWalkingRig는 머리·어깨·엉덩이의 단순 보행 리그, addHumanoid는 팔꿈치·무릎·상체·목을 편집하는 관절 리그다. 서로 다른 ID/자세 계약을 유지한다.

## 공전 + 자전 / 경로 + 자전

이전 createOrbitAndSpin/createPathAndSpin 전용 API는 제거했다. 저장소의 실행 C++ 호출부에는 사용처가 없었다.
같은 작업을 createMotion의 단계 목록으로 표현한다. 로컬/월드 지정이 코드에 드러난다.

```cpp
std::optional<graphics::MotionId> orbitAndSpin(graphics::MotionSystem& system,
        graphics::RotationMotion orbit, graphics::RotationMotion spin) {
    orbit.space = graphics::TransformSpace::World;
    spin.space = graphics::TransformSpace::Local;
    return system.createMotion({graphics::Motion::rotation(orbit), graphics::Motion::rotation(spin)});
}
std::optional<graphics::MotionId> pathAndSpin(graphics::MotionSystem& system,
        const graphics::PathMotion& path, graphics::RotationMotion spin) {
    spin.space = graphics::TransformSpace::Local;
    return system.createMotion({graphics::Motion::followPath(path), graphics::Motion::rotation(spin)});
}
```

createMotion 목록은 동시 평가되는 기여의 순서다. 시간상 순차 배치는 Motion::then에 유한 durationSeconds를 지정해 구성한다.
공간별 누적 행렬은 API 순서대로 적용된다. 최종 적용은 parent × rest × local이다.
World/Local 단계를 번갈아 등록한다고 하나의 단일 행렬에서 그 순서대로 교차 곱하는 것은 아니다.

## 면 펼침과 순차 동작

1. splitFace 또는 setFaceSRT로 독립 면 변환을 준비한다.
2. 면 로컬 좌표에서 힌지의 점과 방향을 AxisLine으로 정한다.
3. progress를 [0,1]로 제한하고 목표 각도*progress를 foldFace에 전달한다.
4. 여러 면의 순서는 면 인덱스와 시작 시각의 배열로 정한다. 사각뿔 각도는 해당 기하에서 계산한다.
5. 원래 면의 정점과 피벗은 유지하고 매 프레임 기준 자세에서 계산한다.

```cpp
bool foldAt(graphics::Scene& scene, graphics::EntityId entity, std::size_t face,
            const graphics::AxisLine& hinge, double seconds, double start, double duration, float target) {
    if (!std::isfinite(seconds) || !std::isfinite(start) || !std::isfinite(duration) || duration <= 0) return false;
    const auto progress = std::clamp((seconds-start)/duration, 0.0, 1.0);
    return graphics::PatternSystem::foldFace(scene, entity, face, hinge, float(progress)*target);
}
```

순차 면 번호·목표 각도·열기/닫기는 응용 설정이다. 이를 위한 실습 번호별 함수를 새로 만들지 않는다.

## 모델 교환

이전 PatternSystem::swapModels는 제거했다. 두 setModel 조립으로 충분하며 실행 C++ 사용처가 없었다.
부분 실패에서 기존 Scene을 보존하려면 임시 Scene을 사용한다.

```cpp
bool swapModels(graphics::Scene& scene, graphics::EntityId first, graphics::EntityId second) {
    const auto* a = scene.getComponent(first);
    const auto* b = scene.getComponent(second);
    if (!a || !b || a->transformOnly || b->transformOnly) return false;
    const auto firstModel = a->polyhedron, secondModel = b->polyhedron;
    auto staged = scene;
    if (!staged.setModel(first, secondModel) || !staged.setModel(second, firstModel)) return false;
    scene = std::move(staged);
    return true;
}
```

setModel은 메시 형상을 바꾸며 엔티티의 변환·계층 ID는 유지한다. 반환된 포인터를 교체 후 계속 사용하지 않는다.

## 그네와 보행을 조립하는 방법

그네는 고정 frame → 회전 hinge → seat → person.root 순서다. addSwingWithRider가 이 주요 계층을 구현한다.
진자 함수는 Motion::pendulum, 관절 기준 자세는 JointAnimation.rest, 매 프레임 계산은 poseHierarchy가 맡는다.
상체 진동이나 팔 움직임은 기존 리그에 별도 JointAnimation을 추가한다. 그네 속도별·팔별 전용 생성 API는 추가하지 않는다.

보행은 CharacterBody 상태 → stepCharacter → 실제 이동 거리 → poseWalkingRig 순서다.
jump는 grounded일 때 수직 속도를 설정한다. 임의 애니메이션 시간 대신 walkDistance로 위상을 계산한다.
화면을 여러 뷰로 그려도 상태 갱신은 프레임당 한 번만 한다.

## SOLID와 ECS의 책임

| 책임 | 데이터/시스템 | 확장 지점 |
|---|---|---|
| 엔티티 상태 | Scene의 ID·Component_Poly·Transform3D·Mesh | 기존 set API와 새 데이터 기본값 |
| 변환 평가 | MotionStep·MotionSample·MotionSystem | 변환 함수와 의미 동작 |
| 초기 장면·입력 연결 | CuboidModeConfig·CuboidKeyBinding·CuboidMode | 설정 교체와 작은 핸들러 |
| 정점 입력·uniform 연결 | RenderConfig·VertexLayout·UniformBindings | packVertices / bindUniforms |
| GPU 소유·업로드·드로우 | Renderer | 렌더 설정을 소비하며 GL 수명 관리 |
| 응용 패턴 | PatternSystem·AnimationSystem | 공통 계산을 조립하는 응용 코드 |

SRP는 상태 저장·계산·GPU 소유의 책임을 나누는 데 적용한다. OCP는 설정과 함수 등록으로 확장한다.
ISP는 정점 구성·uniform·입력 핸들러를 각각 필요한 인자만 받게 한다. DIP는 응용 설정/함수 계약을 렌더러와 모드가 소비하게 한다.
LSP를 위해 새 상속 계층은 필요하지 않다. 기존 시스템의 bool/optional 실패 계약과 좌표·수명 조건을 유지한다.
완전한 archetype ECS로 전환하지 않으며, 현재 ID·컴포넌트 컨테이너·시스템 처리 방식을 유지한다.

## 관련 문서와 사용자 확인

- [의미 동작·육면체 모드](MODE_ACTIONS.md)
- [정점 입력·셰이더 설정 및 내부 수정 순서](RENDER_CONFIGURATION.md)
- [기존 응용 패턴](APPLICATION_PATTERNS.md)
- [사용자 확인 항목](USER_CHECKS.md)

사용자가 기본 실습과 새 모드의 빌드·실행을 확인한다. 세 전용 함수 제거는 저장소 밖 코드에는 호출부 수정이 필요한 공개 API 변경이다.
정적 소스 검토는 화면 동작·빌드 성공을 보장하지 않는다.

## 면의 독립·계층과 회전 의미

[면 변환과 회전 분류](FACE_TRANSFORMS.md): 가장자리 힌지·면 자전·원본 모델 중심 공전을 독립/계층 모드와 조합한다. 기존 independence 플래그는 면 SRT 활성화이며 월드 독립과 구분한다.
