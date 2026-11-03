# 시작·조립·직접 편집

명령으로 배치를 확인한 뒤 C++를 고치는 흐름을 기본으로 한다. 상세 문법은 [명세](SPEC.md), 객체 조합은 [패턴](PATTERN_COMBINATIONS.md)을 읽는다.

## 0번 시작

main.cpp의 `constexpr std::optional<int> initialMode = 0;`으로 고정 시작한다. 기존 nullopt를 유지하면 창에서 0 또는 Alt+0으로 들어간다. Visual Studio에서 사용자가 빌드·실행한 뒤 콘솔의 asm>에 명령을 입력한다.

```text
help
guide patterns
build truck myTruck
guide myTruck
```

기본 레시피에 이미 객체가 있다. 새 조립만 보려면 먼저 clear를 입력한다. build는 실제 C++ 예제 함수다. guide truck/tank/person은 해당 함수의 현재 파일·줄 번호와 수정할 항목을 출력한다.

## 육면체부터 계층 구성

```text
clear
group robot 0 0 0
part box robot torso 0 1 0 0.6 0.8 0.4
joint robot shoulder -0.4 1.3 0
part box robot.shoulder arm 0 -0.3 0 0.2 0.6 0.2
pendulum armSwing robot.shoulder z 25 3
pivot robot 0 1 0 keep
spin bodySpin robot y 20
```

robot.shoulder는 보이는 메시가 없는 연결점이다. arm은 그 연결점 아래의 큐브다. torso와 arm은 robot의 자식이므로 최상위 robot을 바꾸면 함께 변환된다.

```text
srt robot 2 1 0 0 30 0 1.2
select robot.shoulder
rotate robot.shoulder z 10
guide hierarchy
guide pivot
```

srt의 위치는 피벗의 부모 좌표다. keep로 중심을 바꾼 뒤에는 basis 값이 새 피벗 위치를 나타낸다. 위치가 바뀐 것처럼 보여도 기준 메시 배치를 보존한 상태다.

## 자전과 공전 분리

```text
group orbitFrame 0 0 0
parent robot orbitFrame
orbit orbitMotor orbitFrame y 12 0 0 0
```

robot에는 자전, 부모 orbitFrame에는 공전을 둔다. 공전 반지름은 robot의 부모 로컬 위치에서 생긴다. 움직임이 없으면 robot이 공전 중심에 있는지 확인한다. `guide motion`, `inspect robot`, `inspect orbitFrame`을 사용한다.

## 저장과 재구성

```text
save "my-scene.assembly"
clear
load "my-scene.assembly"
```

저장 위치는 콘솔에 절대 경로로 출력한다. 움직임의 현재 프레임은 저장하지 않는다. 명령이 틀리면 이전 조립 상태를 유지하고 오류와 guide 항목을 출력한다. 다른 위치의 파일도 따옴표로 지정할 수 있다.

## C++ 직접 편집

1. guide truck으로 AssemblyExamples.cpp의 buildTruckExample 위치를 찾는다.
2. part의 위치·크기 또는 door 관절 연결점을 고친다.
3. 모션 세부값은 같은 파일의 pendulum 설정을 고친다.
4. 사용자가 다시 빌드·실행한다.
5. build truck myTruck으로 새 구성을 확인한다.

명령 문법을 새로 늘리는 작업부터 시작할 필요는 없다. C++ 예제 함수에서 group/part/attach를 직접 조합한다. 더 일반적인 확장이 필요할 때만 guide custom으로 customCommand 분기를 추가한다. 기본 시작 구성은 guide recipe에서 고친다.

한글 이름과 경로는 UTF-8로 처리한다. 명령과 기능 이름은 소문자이며 객체 이름은 대소문자를 구분한다. 창을 최소화하면 명령 처리도 창 복원까지 기다린다.

## 면 관절·노멀·효과 실험

```text
load "examples/assembly/surface-lab.assembly"
guide mount
surface base 4
guide normal
guide face-track
```

mount는 객체/면/삼각형/U/V/거리를 지정한다. anchor는 원본 메시 좌표를 지정한다. surface는 사용할 정점과 삼각형 번호를 출력한다. normal은 조명, winding은 앞면 방향, orient는 객체 회전 방향이다. 서로 다른 입력이다. 자세한 계약은 SPEC.md를 읽는다.

`guide practice13`부터 `guide practice23`까지 기존 실습 함수로 이동할 수 있다. 기본 명령으로 표현하지 않은 조건·난수·상태 전이는 해당 코드를 Motion 또는 Custom으로 옮긴다. [표현력 평가](REQUIREMENT_AUDIT.md)는 기본 명령의 범위를 구분한다.

사용자 빌드 시 examples/assembly와 docs/assembly가 실행 파일 폴더에 복사된다. load의 상대 파일은 현재 작업 폴더를 먼저 찾고, 없으면 실행 파일 폴더에서 찾는다. save는 현재 작업 폴더에 저장하며 실제 절대 경로를 출력한다.
