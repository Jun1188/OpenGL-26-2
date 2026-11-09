# 의미 단위 동작과 육면체 모드

## 변환 사이에 주요 API 동작 삽입

```cpp
graphics::Motion motion;
graphics::RotationMotion spin;
if (!motion.append(graphics::Motion::rotation(spin))) return false;
if (!motion.insertAction(1, graphics::SceneActions::visible(scene, box, true))) return false;
if (!motion.append(graphics::Motion::linear({}))) return false;
```

SceneActions는 transform/position/rotation/scale/pivot/tint/visible/faceVisible/faceRotation/faceMode/layer/motion/parent/camera를 의미 단위로 제공한다.
값 설정은 절대값이다. 반복 실행에도 같은 값을 설정한다. 계층이 있는 position은 부모 좌표다.
layer/motion에 nullopt를 주면 연결을 해제한다. 연결 자체의 유효성 검사는 기존 Scene/MotionSystem API를 따른다.
반환값은 MotionAction이며 append(Motion::action(...)), insertAction, replace, then에 사용할 수 있다.

```cpp
bool inspect(double seconds, const graphics::MotionSample& partial) {
    // partial.parent / partial.local: 이 위치 앞 단계까지의 누적 행렬.
    return seconds >= 0 && graphics::TransformSystem::validAffine(partial.parent);
}
// 함수 포인터와 캡처 람다 모두 같은 형식이다.
if (!motion.insertAction(1, inspect)) return false;
if (!motion.insertAction(2, [&scene, box](double t, const graphics::MotionSample&) {
    return scene.setPosition(box, {float(t), 0, 0});
})) return false;
```

sample는 동작을 실행하지 않는다. createMotion/setMotion/seekMotion도 동작을 실행하지 않는다.
execute는 변환과 동작을 배열 순서로 평가한다. then은 모든 앞 단계의 유한 종료 시각 뒤에 배치한다.
MotionSystem::update는 모든 모션 샘플이 성공한 뒤 시간·샘플을 확정하고 playing && enabled인 모션의 동작을 실행한다.
Application은 updateHandler 뒤에 updateMotions를 한 번 호출한다. 여기서 execute를 추가 호출하면 동작이 중복 실행된다.
poseHierarchy는 sample를 사용하므로 동작이 필요하면 별도 시점에 execute를 명시적으로 사용한다.

동작은 enabled이고 startSeconds에 도달한 매 평가마다 실행된다. durationSeconds는 전달 시간을 끝 값에 고정하며 실행 횟수를 제한하지 않는다.
일회 이벤트가 아니다. paused 모션은 자동 갱신 시 실행되지 않는다. enabled=false 모션은 시간이 진행돼도 동작을 실행하지 않는다.
순간 토글·카운터 증가를 반복 동작으로 등록하면 매 프레임 누적된다. 한 번의 토글은 키 바인딩에 등록한다.

변환 함수는 같은 시간에 같은 행렬을 반환하는 계산 함수여야 한다. 동작을 가진 모션은 사전 샘플 후 실행 평가를 하므로 변환 함수가 두 번 호출될 수 있다.
현재 실행 목록은 복사한다. 원본 편집은 다음 평가부터 반영된다. 동작 안에서 같은 시스템의 update/execute를 재귀 호출하거나 Scene 전체를 교체하지 않는다.
동작 실패와 예외는 앞서 발생한 외부 부수 효과를 되돌리지 않는다. MotionSystem의 시간·샘플은 이미 확정된 상태다.
transform과 action을 모두 채우거나 둘 다 비운 단계, 빈 함수 포인터, 범위 밖 index는 거부한다.

## 육면체 전용 모드 바인딩

```cpp
auto mode = std::make_shared<graphics::CuboidMode>();
graphics::CuboidConfig box;
box.name = "box";
box.size = {1, 2, 1};
box.transform.basis = {0, 1, 0};
if (!mode->setCuboid(box)) return -1;
graphics::CuboidCommand move;
move.operation = graphics::CuboidOperation::Translate;
move.value = {.1f, 0, 0};
if (!mode->bindKey({GLFW_KEY_RIGHT, GLFW_PRESS, 0,
                   graphics::CuboidMode::action("box", move)})) return -1;
if (!mode->bindKey({GLFW_KEY_ESCAPE, GLFW_PRESS, 0,
                   [](graphics::Application& app, graphics::CuboidMode&) {
                       app.requestClose(); return true;
                   }})) return -1;
if (!graphics::bindCuboidMode(app, mode)) return -1;
return app.run();
```

setConfig는 전체 초기 데이터, setCuboid는 이름별 초기 데이터를 교체한다. getConfig는 읽기 전용이다.
현재 장면에는 즉시 반영하지 않는다. mode.apply(app)를 명시적으로 호출하면 장면 전체를 육면체들로 재구성한다.
설정에는 크기·SRT·색·표시·6개 면 SRT/표시/cull·선택적 Motion·카메라·렌더 옵션이 있다.
서로 다른 이름을 사용한다. 기존 Scene의 모델·선·계층·모션은 apply 성공 시 교체된다. ECS 컴포넌트와 시스템은 그대로 사용한다.
CPU 구성 실패는 기존 Scene을 유지한다. 성공 뒤 setup 핸들러에서 발생한 외부 변경·실패는 되돌리지 않는다.

setSetupHandler는 새 ID를 받은 뒤 계층·SceneActions를 등록할 때 사용한다. setUpdateHandler는 프레임별 응용 동작을 추가한다.
entity/layer/motion(name)은 마지막 apply에서 생성한 ID를 반환한다. layer/motion은 초기 리소스 ID이며 이후 연결 편집 결과는 Scene에서 조회한다.
apply 후 과거 ID·포인터는 다시 받는다. CuboidMode::action은 실행 시 이름으로 ID를 찾아 재적용에 대응한다.
자동 모션 안의 SceneActions는 setup 시 새 ID로 다시 구성한다. 오래된 ID를 캡처한 config.motion을 그대로 재사용하지 않는다.

bindKey는 key/action/mods가 같은 항목을 교체하며 unbindKey는 해당 항목을 해제한다. mods는 정확히 일치시킨다.
getBindings로 현재 바인딩을 조회한다. Shift/Ctrl/Alt 및 Caps/Num lock 상태가 있으면 그 값을 포함한 항목을 등록한다.
CuboidCommand는 위치 설정·상대 이동·로컬 회전·절대 크기·SRT·피벗·표시·토글·레이어·모션·재생 상태를 제공한다.
모션/재생 명령은 현재 엔티티에 연결된 레이어를 사용한다. Motion 명령의 nullopt는 연결 해제다.

bindCuboidMode는 Application의 init/update/modified-key 핸들러를 교체한다. 기존 사용자 핸들러와 병용하려면 apply/update/dispatch를 기존 콜백에서 직접 호출한다.
mode 핸들러는 shared_ptr로 수명을 보관한다. 그 외 캡처한 참조의 수명은 호출자가 보장한다.
GL 자원 소유권은 Renderer, 장면 데이터는 Scene, 동작 실행은 Motion/MotionSystem, 모드 조립은 CuboidMode에 둔다.
상속 계층이나 엔티티별 가상 update를 추가하지 않는다.

## 사용자 확인

사용자가 빌드 후 회전 → 동작 → 이동 순서, partial 행렬, sample/seek에서 동작 횟수 0, update에서 1회를 확인한다.
같은 키 재등록·해제, 설정 변경 후 apply, 잘못된 이름·크기·빈 콜백의 거절을 확인한다.
육면체 모드에서도 기존 Scene과 MotionSystem API로 계층과 모션을 편집할 수 있어야 한다.
빌드·실행·테스트는 수행하지 않았다.

## 면의 독립·계층과 회전 의미

[면 변환과 회전 분류](FACE_TRANSFORMS.md): 가장자리 힌지·면 자전·원본 모델 중심 공전을 독립/계층 모드와 조합한다. 기존 independence 플래그는 면 SRT 활성화이며 월드 독립과 구분한다.
