# 애니메이션 지정 API

`framework/Framework.h`를 포함한다. `standard`에서도 같은 API를 사용한다.
새 사람형·계층 패턴은 `graphics::AnimationSystem`, 기본 시간 모션은 `graphics::Motion`에 있다.
기존 `PatternSystem::addWalkingRig`, `poseWalkingRig`, `JointRig`는 유지한다.

[렌더링 흐름·그네/사람 계층 흐름도](../../framework-analysis/animation-api/flowcharts.html)에서 호출 순서와 부모 관계를 볼 수 있다.

## 선형보간

```cpp
graphics::LinearMotion move;
move.from = {0, 0, 0};
move.to = {2, 0, 0};
move.durationSeconds = 4;
move.playback = graphics::Playback::PingPong;
graphics::Motion motion;
if (!motion.append(graphics::Motion::linear(move))) return false;
```

from/to는 기준 자세에 더할 이동 변환이다. 기본 공간은 Local이다. Once는 끝 위치를 유지하고,
Loop는 처음으로 돌아가며, PingPong은 양 끝 사이를 왕복한다. durationSeconds는 편도 시간이다.
기존 MotionSystem에 등록·레이어 연결해서 쓸 수도 있고, 아래 관절 트랙에 넣을 수도 있다.
`PathSystem::linear`는 진행률 함수이며 두 위치 사이 이동을 만드는 이 API와 역할이 다르다.
유한한 위치, 양수 시간, 유효한 재생 정책과 공간이 필요하다. 잘못된 설정은 빈 MotionStep으로
반환하며 append가 거부한다.

## 진자 운동

```cpp
graphics::PendulumMotion sway;
sway.axis = {{0, 0, 0}, {1, 0, 0}};
sway.amplitudeDegrees = 25;
sway.periodSeconds = 3;
sway.phaseDegrees = 0;
sway.offsetDegrees = 0;
graphics::Motion motion;
if (!motion.append(graphics::Motion::pendulum(sway))) return false;
```

각도 = offsetDegrees + amplitudeDegrees * sin(2π*t/periodSeconds + phaseDegrees*π/180).
기본 공간은 Local, 기본 회전축은 X이다. 진폭은 0 이상, 주기는 양수, 축은 0이 아닌 유한 벡터다.
주기와 진폭을 직접 정하는 운동학적 애니메이션이다. 중력·줄 길이에서 주기를 구하는 물리 진자는 아니다.

## 사람형과 자세

```cpp
graphics::HumanoidConfig config;
config.grid.extent = {1, 1.8f, .6f};
config.upperArmFraction = .5f;
config.upperLegFraction = .5f;
const auto person = graphics::AnimationSystem::addHumanoid(scene, config);
if (!person) return false;
auto pose = graphics::HumanoidPose::seated();
pose.elbows[0] = {-60, 0, 0};
if (!graphics::AnimationSystem::poseHumanoid(scene, *person, pose)) return false;
```

실습 23의 몸통·머리·팔·다리 비율을 유지하고 팔꿈치·무릎을 추가했다.
루트→골반→몸통→목→머리, 몸통→어깨→팔꿈치→손, 골반→엉덩이→무릎→발로 연결한다.
배열 인덱스 0은 모델의 -X 쪽, 1은 +X 쪽이다. 사람은 +Z 방향을 본다.
HumanoidPose는 골반·몸통·목·양쪽 어깨·팔꿈치·엉덩이·무릎의 도 단위 XYZ 로컬 회전이다.
X→Y→Z 순서로 적용하므로 열벡터 기준 최종 회전은 Rz*Ry*Rx이다.
기본 자세는 모두 0이다. seated()는 엉덩이 -90°, 무릎 +90°, 어깨 -20°, 팔꿈치 -40°다.

HumanoidRig에는 관절 ID와 torsoMesh/head/nose/upperArms/forearms/thighs/shins 메시 ID가 있다.
Scene::setPosition, setSideLengths, setTint, PatternSystem::rotateJoint로 각 부위를 편집할 수 있다.
관절을 돌리면 자식 부위가 따라간다. torso는 관절이고 torsoMesh는 표시 메시다.
크기는 grid.extent, 팔다리 위쪽 비율은 upperArmFraction/upperLegFraction으로 정한다.
부모를 지정하면 grid.frame은 그 부모의 로컬 공간에서 해석한다.

## 관절 계층 애니메이션

```cpp
const auto track = graphics::AnimationSystem::createJointAnimation(scene, person->elbows[0], motion);
if (!track) return false;
std::vector<graphics::JointAnimation> animation{*track};
// 매 프레임 누적 절대 시간(초)을 전달한다.
if (!graphics::AnimationSystem::poseHierarchy(scene, animation, elapsedSeconds)) return false;
```

트랙은 joint ID, 생성 시점의 rest 자세, Motion을 값으로 보관한다.
매번 rest에서 계산하므로 앞 프레임의 회전을 반복 누적하지 않는다.
관절 로컬 행렬 = sample.parent * rest * sample.local.
여기서 Motion의 World 기여는 **관절 부모 좌표**에서 rest 앞에 적용한다.
Local 기여는 rest 뒤에 적용한다. Scene의 부모 행렬은 나중에 바깥에서 합성한다.
모션 샘플·기준 자세는 이동+회전의 강체 변환만 허용한다. 크기는 자식 메시에서 바꾼다.
같은 관절 ID를 한 트랙 목록에 두 번 넣지 않는다. 여러 움직임은 하나의 Motion에 append한다.
트랙 생성 후 기준 자세를 편집했다면 createJointAnimation을 다시 호출해 새 rest를 저장한다.
Application은 MotionSystem만 자동 갱신한다. 이 목록은 update 콜백에서 poseHierarchy로 갱신한다.
같은 관절을 다른 update 코드나 모션 레이어가 동시에 조작하지 않도록 책임을 정한다.

## 그네(진자) → 사람 응용 패턴

```cpp
graphics::SwingConfig config;
config.pivot = {0, 3, 0};
config.length = 2;
config.pendulum.amplitudeDegrees = 25;
config.pendulum.periodSeconds = 3;
config.riderSwayDegrees = 8;
const auto swing = graphics::AnimationSystem::addSwingWithRider(scene, config);
if (!swing) return false;
// 매 프레임: 그네와 사람 상체를 함께 갱신한다.
if (!graphics::AnimationSystem::poseHierarchy(scene, swing->animation, elapsedSeconds)) return false;
```

고정 frame→진자 hinge→줄/좌석→사람 root→골반→몸통/다리 구조다.
고정 지지대는 frame의 자식이므로 hinge가 회전해도 함께 흔들리지 않는다.
좌석과 줄은 hinge의 자식이며, 사람은 좌석의 자식이다.
사람의 골반을 좌석 윗면에 맞추고 seated() 자세를 적용한다.
config.person.grid.frame의 회전은 유지하지만 위치는 착석 위치에 맞춰 다시 정한다.
config.pendulum.axis.point는 hinge 원점, space는 Local로 고정한다. pivot은 부모 공간 위치다.
축 방향·진폭·주기·위상·중심 각도는 config.pendulum으로 정한다.
상체는 같은 주기·180° 반대 위상으로 riderSwayDegrees만큼 별도로 흔들린다. 0이면 상체 추가 운동이 없다.
SwingRig::person의 모든 관절과 SwingRig::animation은 편집할 수 있다.
팔 트랙을 추가하려면 앉은 자세를 적용한 뒤 해당 어깨/팔꿈치에서 기준 자세를 캡처한다.

Scene::clear 이후에는 저장한 rig와 트랙을 다시 만든다. Scene ID는 재사용될 수 있다.
addHumanoid/addSwingWithRider는 임시 Scene에서 생성하므로 실패 시 기존 Scene을 보존한다.
성공 시 Scene 대입으로 기존 컴포넌트의 비소유 포인터가 무효화될 수 있다. 필요하면 ID로 다시 조회한다.

## 사용자가 수행할 확인

빌드·테스트·앱 실행은 수행하지 않았다. 아래는 예상 결과이며 실행 관찰이 아니다.

1. VS에서 standard를 x64로 빌드하고 실행한다. 9를 누르면 그네와 두 사람, 노란 상자가 표시되어야 한다.
2. 노란 상자는 t=0에서 x=-2, t=2에서 x=0, t=4에서 x=2, t=8에서 x=-2이어야 한다.
3. 진자 위상 0에서는 t=0 각도 0°, t=.75에서 +25°, t=1.5에서 0°, t=2.25에서 -25°이어야 한다.
4. 그네 좌석의 사람 골반이 좌석 윗면을 따라가고, 지지대는 고정되어야 한다.
5. 서 있는 사람은 양팔을 반대 위상으로 움직여야 한다. 팔꿈치는 어깨를 따라가야 한다.
6. Space로 전체 예제 시간을 멈추고 재개한다. V의 세 뷰에서도 같은 자세가 보여야 한다.
7. 0/7/8/9로 전환한 후 다시 9를 누른다. 오래된 ID 없이 초기 자세로 시작해야 한다.
8. durationSeconds=0, periodSeconds=0, 0축, 중복 관절 ID를 각각 지정하면 거부되어야 한다.
   poseHierarchy의 잘못된 마지막 트랙 때문에 앞 관절만 먼저 바뀌어서는 안 된다.
9. 기본 구성의 기존 Motion/격자/방 예제와 실습 23의 기존 보행 기능은 별도로 확인한다.
