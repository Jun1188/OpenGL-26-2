[현재 줄·변경 전후 예시·디버깅 순서](EDIT_DEBUG_GUIDE.md) · [실습 컨테이너](FEATURE_CONTAINERS.md)

# 기능별 C++ 직접 편집 안내

콘솔은 배치·동작을 확인하는 보조 수단이다. 기본 확장 경로는 이 문서와 `guide <기능>`을 통해 C++를 직접 편집하는 것이다. 현재 줄 번호는 실행 시 소스 표식을 검색해 계산한다. 가이드가 디스크의 현재 줄과 주변 코드를 읽고, GuideRecipes의 변경 예·단위·제약·디버깅 절차를 함께 출력한다. 예시의 작성 기준값과 현재 파일 값은 구분한다.

| guide | 실제 파일·함수 | 바꿀 항목 |
|---|---|---|
| tank | modes/AssemblyExamples.cpp · buildTankExample | 차체·궤도·포탑·포신 치수와 연결점, 회전 모터 |
| truck | 같은 파일 · buildTruckExample | 바퀴 배치, 컨테이너 패널, 문 힌지와 범위 |
| person | 같은 파일 · buildPersonExample | 몸체·관절 계층, 팔다리 길이, 반대 위상 |
| tank-preset | modes/AssemblyParts.cpp · addTankParts | 실습19·22와 공유하는 기존 탱크 |
| mount / anchor | modes/AssemblySurface.cpp · mount/applyMounts | 객체·면·삼각형 좌표 연결과 깊이 순서 갱신 |
| normal / geometry | 같은 파일 · surfaceCommand/model | 노멀·winding·면 SRT·원시 삼각형·정점 |
| warp | 같은 파일 · surfaceCommand | 정적 비균일 배율·반사 그룹 |
| face-track | modes/AssemblyTracks.cpp · trackCommand | 임의 축/중심·지연·축소·반복 |
| cpp-effect | modes/AssemblyRuntime.cpp · Effect::Custom 분기 | 사용자 프레임 동작과 기준 SRT |
| practice13~23 | 각 실습의 실제 initScene/onUpdate/면 합성 함수 | 명령으로 일반화하지 않은 난수·조건·상태 전이 |
| hierarchy | modes/AssemblyHierarchy.cpp · group/part | 부모 연결, 부품 이름, 조립 단위 |
| srt | 같은 파일 · srt | 최상위 위치·Euler 회전·균일배율 |
| pivot / center | 같은 파일 · pivot/center | 중심 보존 보정과 하위 메시 경계 계산 |
| motion | modes/AssemblyMotion.cpp · motionCommand | 기존 Motion 설정·매개변수 |
| runtime | modes/AssemblyRuntime.cpp · applyPoses | 기준 SRT와 기능의 합성 순서 |
| face | modes/AssemblyMotion.cpp | 힌지/자전/공전 설정 |
| physics | modes/AssemblyRuntime.cpp | 중력, 방 AABB, 이탈·보행 계산 |
| humanoid / swing | framework/detail/Animation.cpp | 기본 리그 비율·연결점·그네 구조 |
| walker | framework/detail/Patterns.cpp · addWalkingRig | 기본 보행 리그의 형상과 관절 |
| input / camera | modes/AssemblyInput.cpp | 입력·카메라 제어 |
| recipe | modes/Mode0Recipe.cpp · USER_RECIPE_BEGIN | 초기 명령 목록 |
| custom | modes/AssemblyExtensions.cpp · USER_COMMAND_BEGIN | 사용자 정의 C++ 기능 진입점 |

## 작은 C++ 조립 함수

다음은 AssemblyExtensions.cpp의 customCommand 안에 추가할 수 있는 분기다. outer execute가 CPU 조립 상태를 보존한다. 사용자 이름과 동작 ID가 이미 있으면 예외가 발생하고 이전 구성을 유지한다.

```cpp
if (args[0] == "my-arm") {
    require(args.size() == 2, "my-arm <이름>");
    const auto& name = args[1];
    scene.group(name, {0, 0, 0});
    const auto shoulder = scene.part("joint", name, "shoulder", {0, 1, 0});
    scene.part("box", shoulder, "arm", {0, -.3f, 0}, {.2f, .6f, .2f});
    scene.pivot(name, scene.center(name));
    graphics::PendulumMotion sway;
    sway.axis.direction = {0, 0, 1};
    sway.amplitudeDegrees = 30;
    sway.periodSeconds = 2;
    Behavior motor;
    motor.name = name + ".motor";
    motor.target = shoulder;
    require(motor.motion.append(graphics::Motion::pendulum(sway)), "arm motor");
    scene.attach(std::move(motor));
    return true;
}
```

좌표·회전·스케일은 모델→부모→월드→뷰→투영 순서로 읽는다. part의 size는 정점 치수이며 rest.scale은 부모에게 전달되는 균일 배율이다. 관절을 회전시키면 자식 오프셋까지 따라 움직인다. 메시를 관절 위치에 직접 놓는 것과 자식 메시를 관절에서 떨어뜨려 놓는 것을 구분한다.

## 중심·자전·공전 수정

pivot keep는 기준 메시 배치를 보존한다. local 모션은 T(pivot)·local·T(-pivot)으로 합성한다. 자전 중심이 어긋나면 AssemblyRuntime.cpp의 motionFor와 AssemblyHierarchy.cpp의 pivot을 함께 읽는다. 공전은 별도 부모 그룹의 World 기여로 만들면 자전과 위치 제어를 분리할 수 있다.

parent 프레임의 축과 절대 월드 축을 혼동하지 않는다. 일반 동작 노드는 양수 균일 rest 계약을 사용한다. 비균일/음수 배율은 바깥 warp 그룹에 두며, 이 그룹은 poseHierarchy를 거치지 않는 정적 SRT다. 회전과 비균일 배율 그룹을 여러 계층으로 쌓으면 합성 결과에는 shear도 생길 수 있다. 물리 공간은 여전히 강체 조건을 지켜야 한다.

## 수작업 리그를 물리 보행에 연결할 때

관절 흔들림 예제와 CharacterBody 충돌 보행은 별개다. 직접 조립한 사람에 walk를 붙이려면 C++에서 root/head/shoulders/hips ID로 JointRig를 구성해 해당 루트 node.walker에 등록한다. 연결점과 몸체의 높이·반폭을 함께 맞춘다. 현재 보행 루트는 부모 없는 월드 축 정렬 stage, 배율1, 중심 피벗을 요구한다. 이 어댑터를 등록하지 않은 임의 그룹에는 walk를 붙이지 않는다.

## 재빌드와 범위

레시피 숫자 변경은 load로 재구성한다. C++ 변경은 사용자가 빌드한 뒤 실행 파일에 반영된다. 소스 파일이 없는 배포본에서는 guide가 현재 줄 번호를 확인할 수 없다고 출력한다. 잘못된 줄을 추정하지 않는다. framework/detail의 기본 리그를 고치면 해당 동봉 framework를 사용하는 다른 모드에도 영향을 줄 수 있다. 0번만 바꿀 목적이면 AssemblyExamples.cpp의 독립 예제부터 고친다.

## 사용자 프레임 효과 예

다음 분기를 customCommand 안에 추가하면 `pulse hero`로 전체 그룹의 크기를 시간에 따라 바꾸는 실험을 할 수 있다. 실행 시점에 C++ 소스가 자동 수정되는 기능은 없다. 사용자가 편집·빌드한 함수가 호출된다.

```cpp
if (args[0] == "pulse") {
    require(args.size() == 2, "pulse <대상>");
    const auto id = scene.node(args[1]).id;
    Behavior effect;
    effect.name = args[1] + ".pulse";
    effect.target = args[1];
    effect.effect = Effect::Custom;
    effect.custom = [id](graphics::Scene& target,
                        const graphics::Transform3D& rest,
                        double seconds, float /*dt*/) {
        auto pose = rest;
        pose.scale *= float(1 + .25 * std::sin(seconds * 2));
        return target.setTransform(id, pose);
    };
    scene.attach(std::move(effect));
    return true;
}
```

rest는 사용자가 편집한 기준 자세다. 이전 프레임의 scale에 매번 배율을 곱하면 지수적으로 커지므로 기준값과 절대 시각에서 다시 계산한다. 한 프레임마다 사용자 입력값을 덮어쓰는 원래 문제도 이 방법으로 피한다. 동일 target에 path/physics가 있으면 소유권 충돌로 거절한다. 필요하면 효과용 바깥 그룹과 이동용 자식 그룹을 나눈다.

Custom은 Scene과 기준 SRT를 받고, 필요하면 생성 함수에서 안전한 수명의 Application 참조를 캡처해 카메라를 제어할 수 있다. 외부 IO, shared_ptr 내부 상태, 다른 앱 변경은 execute의 Scene 복원으로 되돌릴 수 없다. 재현 가능한 효과는 절대 seconds에서 계산한다. 적분 효과는 dt=0일 때 상태를 진행하지 않는다.

## 한 면의 복합 행렬

Effect::FaceTrack을 사용하면 하나의 면 소유자 안에서 MotionStep을 여러 개 append할 수 있다. parent 기여는 원본 메시 모델 공간에서 기준 면 행렬 앞에, local 기여는 그 뒤에 합성한다. 기준 면 SRT까지 포함한 전체 결과는 기존 Scene::faceWorldMatrix가 렌더·피킹·면 관절에 전달한다.

실습15의 applyFaceMotion과 실습16의 applyPyramid는 여러 효과와 atan2 각도 계산의 실제 예다. guide practice15/practice16으로 열고 필요한 행렬을 하나의 FaceTrack.motion에 구성한다. 새로운 Effect 종류를 계속 추가할 필요는 없다. 지연·재생·enable·remove는 공통 컨트롤을 사용한다.

## 노멀과 형상 직접 편집

조명은 Mesh::vertexNormals, 컬링은 triangleIndices와 CullMode, 부착 방향은 실제 기하 삼각형의 외적을 사용한다. 이 셋을 같은 상태로 취급하지 않는다. Scene::setVertexNormals/setMesh/setModel을 사용하면 geometry revision이 갱신되어 Renderer가 정점을 다시 업로드한다. 비소유 getMesh 포인터는 setModel이나 새 객체 생성 뒤에 다시 구한다.

임의 메시·UV·재질은 graphics::Model을 구성해 Assembly::model로 등록한다. 객체 이름과 .mesh 별칭은 공통 등록 규칙을 사용한다. add/attach를 customCommand에서 호출하면 실패한 명령의 CPU 상태가 복원된다. 별도 초기화 함수에서 직접 호출하면 예외와 부분 구성을 호출자가 처리한다.

## 기능 단위 입력 콜백

Behavior의 Effect::Input은 SRT 소유권을 잡지 않고 key/mouse/cursor/focus 함수를 연결한다. guide input으로 연 파일의 dispatchInput/onKey/onMouse/onCursor/onFocus를 함께 읽는다. 예를 들어 AssemblyExtensions.cpp에 `<iostream>`을 포함하고 다음 분기를 customCommand에 추가할 수 있다.

```cpp
if (args[0] == "install-turn-key") {
    require(args.size() == 2, "install-turn-key <대상>");
    const std::string name = args[1];
    scene.node(name);
    Behavior input;
    input.name = name + ".input";
    input.target = name;
    input.effect = Effect::Input;
    input.key = [name](Assembly& target, int key, int action, int /*mods*/) {
        if (key != GLFW_KEY_H || action != GLFW_PRESS) return false;
        target.execute("rotate " + quote(name) + " y 5", std::cout);
        return true; // 입력을 소비했다는 뜻. execute의 성공 여부와 다르다.
    };
    scene.attach(std::move(input));
    return true;
}
```

활성 콜백은 등록 순서로 기본 바인딩/피킹/편집보다 먼저 호출한다. true를 반환하면 입력을 소비한다. enable off 또는 remove로 해제한다. RELEASE와 포커스 손실 시 기본 hold/drag 해제는 콜백보다 먼저 수행한다. Esc·Home·전역 모드 키는 상위 Console/모드가 처리한다.

callback이 clear/load/remove로 구성을 바꾸면 토폴로지 번호 변경을 확인해 이전 구성의 나머지 콜백 실행을 중단한다. 콜백 목록은 복사하므로 무효화된 vector 반복자를 사용하지 않는다. 지속되는 입력 상태가 필요하면 생성 함수가 만든 shared_ptr 상태를 함수들에 공유하고 focus(false)에서 누름/드래그를 해제한다. 값으로 캡처한 mutable 상태는 목록 복사 때문에 지속 저장소로 쓰지 않는다. shared 상태와 외부 부수효과는 명령 복원/정확한 프레임 저장 범위 밖이다.

2D 클릭 생성·드래그 결합·적중 키처럼 별도 입력 의미가 필요한 경우에도 공통 라우터를 매번 고치지 않고 이 훅에서 구현한다. 커서의 window 좌표와 framebuffer 좌표를 혼용하지 않는다. 뷰포트/DPI에 맞는 screenOnPlane/pickAt을 이용한다.
