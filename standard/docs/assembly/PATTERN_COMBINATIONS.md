# 기능 조합 패턴

2026-11-03. 아래는 소스에서 구성한 조합과 예상 동작이다. 빌드·실행 결과가 아니다. 상세 모델은 C++로 직접 편집하고, 콘솔은 배치와 동작을 실험하는 데 사용한다.

## 먼저 선택할 작업 경로

| 목적 | 시작 명령 | 직접 편집 |
|---|---|---|
| 탱크 | `build tank rover` → `guide rover` | AssemblyExamples.cpp의 buildTankExample |
| 트럭·컨테이너 | `build truck carrier` → `guide carrier` | 같은 파일의 buildTruckExample |
| 사람 | `build person hero` → `guide hero` | 같은 파일의 buildPersonExample |
| 원시 부품부터 명령으로 관찰 | 아래 파일을 load | .assembly의 치수·관절·모션 수치 |
| 면에 관절 부착 | `guide mount` | AssemblySurface.cpp의 mount/applyMounts |
| 복잡한 조건·교환·효과 | `guide cpp-effect`, `guide practice17` | Custom 동작 또는 하나의 복합 Motion |

기본 레시피에 hero/rover가 있으므로 중복 이름을 피하거나 clear 후 시작한다. guide의 기능 이름과 객체 이름이 충돌하면 `guide feature:tank`, `guide feature:normal`을 사용한다.

## 탱크: 차체 → 포탑 → 포신

[전체 수작업 레시피](../../examples/assembly/tank.assembly):

```text
load "examples/assembly/tank.assembly"
guide tank-pattern
```

계층은 tank 아래 body/leftTrack/rightTrack/turret, turret 아래 body/gun, gun 아래 barrel/muzzle이다. body는 치수를 가진 큐브이며 turret/gun은 회전 연결점이다. 메시와 관절을 분리하면 포탑의 자전이 차체에 역으로 전달되지 않는다.

핵심 조합은 다음과 같다. 전체 레시피의 해당 부분을 편집하거나 clear 후 차례로 구성한다.

```text
group tank 0 0 0
part box tank body 0 0.45 0 2.2 0.5 2.6
joint tank turret 0 0.8 0
part box tank.turret body 0 0.25 0 1.4 0.5 1.5
joint tank.turret gun 0 0.3 0.65
part box tank.turret.gun barrel 0 0 1 0.2 0.2 2
pivot tank 0 0.6 0 keep
spin turretMotor tank.turret y 25
pendulum gunMotor tank.turret.gun x 10 4
```

루트 tank를 SRT로 옮기면 차체·포탑·포신 전체에 전달한다. 포탑과 포신의 시간은 각 동작 ID에서 제어한다. 정지한 후 `select tank.turret.gun`으로 포신만 수동 조정할 수 있다. 차량 지면 물리나 궤도 마찰은 이 모델에 포함되지 않는다.

## 트럭: 차체와 컨테이너 문 힌지

[전체 수작업 레시피](../../examples/assembly/truck.assembly):

```text
load "examples/assembly/truck.assembly"
guide truck-pattern
```

truck 아래 chassis/cabin/wheel1~4/container가 있다. container 아래 floor/roof/leftWall/rightWall/frontWall과 leftDoor/rightDoor가 있다. 각 문 panel은 관절에서 ±0.425만큼 떨어져 있어, 문 중심이 아닌 양쪽 가장자리를 기준으로 돈다. 바퀴도 큐브로 만들어 원시 부품만으로 조립하는 과정을 유지한다.

```text
joint truck.container leftDoor -0.85 0 -0.95
part box truck.container.leftDoor panel 0.425 0.625 0 0.85 1.2 0.1
pendulum doorMotor truck.container.leftDoor y 40 6
```

전체 .assembly는 양쪽 문을 ±40°로 반대 위상 왕복한다. C++ buildTruckExample은 offset/phase까지 지정하여 한쪽 0~80°, 다른 쪽 -80~0°로 열린다. 실제 문 개방 범위는 C++ 예제의 pendulum helper를 편집한다. 컨테이너만 이동하려면 truck.container를, 트럭 전체를 이동하려면 truck을 지정한다.

## 사람: 골반·몸통·팔다리 관절

[전체 수작업 레시피](../../examples/assembly/person.assembly):

```text
load "examples/assembly/person.assembly"
guide person-pattern
```

person → pelvis → torso → neck/head/nose, torso → 양쪽 shoulder → upperArm/elbow/forearm, pelvis → 양쪽 hip → thigh/knee/shin/foot 순서다. 어깨와 엉덩이는 진자의 중심이다. 반대쪽 팔다리에 반 주기 위상을 주어 반대 방향으로 움직인다.

```text
pendulum leftArm person.pelvis.torso.leftShoulder x 25 2
pendulum rightArm person.pelvis.torso.rightShoulder x 25 2
seek rightArm 1
linear stroll person 3 0 0 5 pingpong
```

수작업 사람은 관절 모션 예제다. 장애물·점프를 포함한 충돌 보행은 `add walker`, `walk`, `obstacle`, `jump`를 사용하거나 C++에서 JointRig를 등록한다. 정지 때 팔다리 진폭을 0으로 만드는 조건도 Custom 동작에서 실제 이동 거리에 연결한다.

## 최상위 중심·자전·공전·반사

```text
clear
build tank rover
srt rover 3 0.6 0 0 30 0 1.2
spin selfSpin rover y 20
group orbitFrame 0 0 0
parent rover orbitFrame
orbit around orbitFrame y 12 0 0 0
```

자식 rover의 피벗과 부모 orbitFrame의 공전 중심을 분리한다. orbit는 부모 프레임 좌표의 중심이며, 여기서는 orbitFrame에 공간 부모가 없으므로 월드 원점이다. center/피벗은 동작 부착 전 또는 seek 0 상태에서 정한다.

반사·비균일 배율은 바깥 그룹에 둔다. 기존 rover의 부모를 바꾸면 이전 공전 연결도 바뀌므로 다음은 별도 장면 예다.

```text
clear
group mirror 0 0 0
warp mirror -1 1.5 1
build person hero
parent hero mirror
rotate mirror y 30
```

warp 그룹에는 일반 Motion을 붙이지 않는다. 움직이는 자식 group/joint와 정적 비균일 그룹을 번갈아 쌓으면 서로 다른 중심의 배율·회전·반사를 조합할 수 있다. 평면 x=2 반사는 바깥 그룹의 pivot을 (2,0,0) keep로 지정하고 X 배율을 -1로 둔다. 임의 평면은 그룹을 회전해 축을 맞춘 뒤 반사한다. 법선은 inverse-transpose, 컬링은 변환 determinant를 통해 기존 렌더러/피킹 경로에서 처리한다.

## 객체·면·좌표에 관절 부착

[면 관절·노멀·복합 면 동작 레시피](../../examples/assembly/surface-lab.assembly)를 사용한다.

```text
clear
add box base 0 0 0 2 2 2
surface base 4
mount base connector 4 0 0 0.5 0
part box base.connector pole 0 0 0.5 0.15 0.15 1
pendulum poleMotor base.connector y 25 3
```

mount의 면4·삼각형0에 대해 P=(1-U-V)A+U·B+V·C다. U=0,V=.5는 A와 C의 중점이며 큐브 사각형 면의 중심에 해당한다. 마지막 값은 메시 로컬 법선 방향 거리다. 관절 +Z는 실제 삼각형 winding의 법선이며 +Y는 첫 변 방향이다. 조명용 normal을 뒤집어도 부착 방향과 컬링은 바뀌지 않는다.

`anchor base connector 4 X Y Z`는 삼각형0 방향과 지정한 원본 메시 좌표를 사용한다. 면 내부에 투영하거나 범위를 잘라 주지는 않는다. 면 이동/회전/축소 뒤에도 부착 좌표와 방향을 재계산한다. 부착 관절은 면 배율을 자체 배율로 복사하지 않으며, 부모 메시의 변환은 그대로 따른다. unmount는 부모 로컬 기준 자세로 돌아간다.

## 면 펼침·축소·노멀·컬링

```text
face-track fold base 4 2 0 pingpong turn 0 0 1 -1 1 0 90
face-track shrink base 1 2 1 pingpong scale 0.2 0.2 0.2 0 0 -1
normal base all auto
normal base 1 0 0 -1
cull base all back
outline base all features
render lighting on
render light-direction 1 2 1
```

fold는 임의 축·중심의 왕복 회전, shrink는 1초 지연 후 왕복 축소다. 같은 면은 한 동작이 소유한다. 한 면에 자전+공전+축소가 필요하면 C++의 하나의 FaceTrack.motion에 행렬 단계를 append한다. 다른 면은 각각의 트랙으로 동기화한다. guide practice15/practice16으로 실제 합성 행렬과 각도 계산을 찾는다. 노멀 편집은 조명 입력이고, winding 편집은 삼각형 앞면 및 mount 방향 입력이다.

## 실습 세부 동작의 C++ 적용

[실습별 표현력 평가](REQUIREMENT_AUDIT.md)에서 필요한 기능을 찾는다. guide practice13~23은 실제 기존 모드의 함수 위치를 출력한다. 해당 함수의 행렬/조건 블록을 AssemblyExtensions.cpp에서 생성하는 Motion 또는 Custom 동작으로 옮긴다. initScene 전체나 Console 여러 개를 실행해 Scene을 지우는 방식으로 합치지 않는다.

레시피 편집은 load로 반영한다. C++ 함수 변경은 사용자가 재빌드 후 build 명령 또는 초기 레시피로 확인한다. 현재 프레임의 물리 속도·Custom 내부 상태를 정확히 저장하는 기능은 제공하지 않는다.
