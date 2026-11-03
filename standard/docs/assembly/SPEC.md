# 0번 기능 조립 API 명세

2026-11-03. 구현 소스를 기준으로 작성했다. 빌드·테스트·실제 입력 결과를 검증한 명세는 아니다.

## 목적과 편집 원칙

`Assembly`는 기존 Scene·Motion·Animation·Collision·Pattern API를 연결한다. 콘솔은 간단한 생성·배치·동작 실험과 레시피 기록을 제공한다. 별도 스크립트 언어·GUI 편집기·일반 게임 엔진을 만들지 않는다. 세부 치수·축·모션·추가 기능은 `guide`가 찾은 C++ 함수를 고친다.

`main.cpp`의 `initialMode = 0`이면 0번으로 고정 시작한다. 기본 nullopt는 기존 1번으로 시작하며 0 키로 조립 모드에 들어간다. 추가 실습의 고유 숫자 키와 충돌하면 Alt+0을 사용한다. 전체 모드 범위는 0~22다.

## 상태와 실행 책임

- 객체와 부품은 이름으로 식별한다. `part box robot torso ...`는 `robot.torso`와 메시 노드 `robot.torso.mesh`를 만든다.
- 부모 관계는 Scene에 저장한다. 각 기능은 Scene 전체를 지우지 않는다. `clear`, 기본 레시피 재구성, 모드 전환만 전체 초기화한다.
- 입력과 명령은 기준 SRT 또는 물리 상태를 바꾼다. 프레임은 기준 SRT·모션 → 컨베이어/면/벽 → 방 물리 → 보행 → C++ Custom → 면 관절의 깊이 순서로 계산한다. 한 기능의 시간은 프레임당 한 번 진행한다. 렌더 패스는 시간을 진행하지 않는다.
- 같은 대상의 위치를 경로·공전·선형 이동·컨베이어·물리가 동시에 소유하면 거부한다. 필요하면 부모 공전 그룹과 자식 이동 그룹으로 계층을 나눈다. 자전과 관절 동작은 위치 동작과 합성할 수 있다.
- 같은 실제 메시의 같은 면을 두 동작이 소유하면 거부한다. 서로 다른 이름을 통해 같은 면에 접근해도 같다.
- disabled 상태에서도 제어 소유권은 유지한다. 다른 제어 기능으로 바꾸려면 remove를 사용한다.

## 계층 생성과 SRT

```text
group 이름 X Y Z
part 종류 부모 부품이름 X Y Z [크기X 크기Y 크기Z]
joint 부모 관절이름 X Y Z
add 종류 이름 X Y Z [크기X 크기Y 크기Z]
parent 자식 부모|none
srt 대상 위치X 위치Y 위치Z 회전X 회전Y 회전Z 균일배율
position 대상 X Y Z
move 대상 DX DY DZ
rotate 대상 x|y|z 도
scale 대상 균일배율
pivot 대상 X Y Z [keep|raw]
pivot 대상 center [keep|raw]
center 대상 [keep|raw]
```

크기는 메시 생성 치수다. 균일 SRT 배율과 구분한다. `part`의 위치는 부모 로컬 좌표이며 반환 이름은 부모.부품이다. `parent`도 기존 로컬 SRT를 유지한다. 월드 자세 보존 reparent가 아니다.

SRT는 `T(basis) * R * S * T(-pivot)`이다. srt의 회전은 `Rz * Ry * Rx`이므로 열벡터에는 X→Y→Z 순서로 적용한다. 위치는 부모 좌표의 피벗 위치이며 srt는 기존 pivot을 유지한다. rotate는 기준 자세의 로컬 증분 회전이다.

pivot의 기본 keep는 `basis += R*S*(newPivot-oldPivot)`로 기준 배치를 유지한다. raw는 basis를 보정하지 않는다. center는 모든 하위 메시의 현재 형상을 대상 모델 좌표로 옮긴 AABB의 중심이다. 질량중심이 아니며 숨긴 면도 포함한다. 선만 있는 그룹에는 중심을 계산할 수 없다. 피벗 변경 중 활성 모션은 새 중심에서 다시 평가하므로, 중심 설정은 동작 부착 전 또는 seek 0 상태에서 한다.

최상위 group에 적용한 이동·회전·균일배율은 모든 자식에 전달한다. 개별 부품은 이름으로 select하여 편집한다. 마우스는 수작업 계층의 최상위 group을 선택한다. 별도 group에 속하지 않은 프리셋은 가장 가까운 의미상 객체를 선택한다.

## 객체 종류

`box`, `pyramid`, `sphere`, `cone`, `tetra`, `joint`, `humanoid`, `swing`, `tank`, `walker`, `room`, `grid`, `conveyor`, `planets`를 제공한다. sphere는 같은 지름 3개, cone은 같은 X/Z 지름을 요구한다. 복합 객체는 크기 인자 대신 scale을 쓴다.

기본 room의 로컬 경계는 (-2,0,-2)~(2,3,2), 앞면 0은 표시상 열린 상태다. 벽·바닥과 충돌 경계의 의미는 별개다. 기본 grid는 6×1×4다. planets는 태양·행성 3개·달의 간단한 공전 조립 예이며 실습18 전체 요구를 대체하지 않는다. conveyor도 기본 경로와 택배 예제를 제공한다. 세부 구성이 필요하면 guide objects/room/grid 또는 해당 C++ 함수를 편집한다.

## 동작 부착

```text
spin ID 대상 축 도/초 [local|world]
orbit ID 대상 축 도/초 중심X 중심Y 중심Z
path ID 대상 경로번호1~6 주기초
linear ID 대상 DX DY DZ 시간초 [once|loop|pingpong]
pendulum ID 대상 축 진폭도 주기초
belt ID 대상 속도 높이보정 [위상0~1]
face ID 대상 면 spin|orbit 축 도/초
face ID 대상 면 hinge 모서리 도/초
wall ID 방 면0~3 개방시간초
play ID on|off
enable ID on|off
seek ID 초
remove ID
```

spin은 지정한 피벗의 자전이다. local은 기준 모델 축, world는 프레임워크의 World 공간, 즉 대상 부모 프레임의 축이다. 공간 부모가 있으면 절대 월드 축과 같다고 가정하지 않는다. orbit의 중심도 부모 프레임 좌표다. 최상위 공전 그룹을 월드 원점에 두면 중심 숫자가 월드 좌표가 된다.

path는 기존 1~6 경로 정의를 공유하며 시작점으로부터의 변위를 부모 공간에 적용한다. linear는 모델 로컬 공간의 변위다. local 기여는 pivot을 기준으로 합성한다. 부모 공전 그룹과 자식 자전 그룹을 분리하면 중심과 축을 독립 지정할 수 있다. 같은 공간의 모션은 등록 순서의 적용 순서를 따른다.

play off는 현재 시간의 자세를 유지한다. enable off는 일반 모션·컨베이어의 기여를 제거하고 면/벽은 부착 전 상태를 복원한다. 물리는 시간 적분만 멈추며 미이탈 객체는 움직이는 방을 계속 따른다. remove는 동작을 제거하고 기준 자세로 돌아간다. 면은 부착 전 행렬·공간·가시성을 복원한다. 물리에는 seek를 적용하지 않는다.

hinge의 축은 선택한 메시 모서리에서 계산한다. spin/orbit의 축과 구분한다. 기본 wall은 닫힘부터 개방까지의 일회 진행이다. 숨긴 앞면 0을 열기 대상으로 쓰려면 먼저 가시성을 확인한다.

## 물리와 보행

```text
physics ID 상자 방
walk ID walker 방 속도
obstacle 보행ID 메시객체
jump 보행ID 속도
speed 보행ID 속도
direction 보행ID X Z
visible 방 5 off
```

physics는 중심 기준 box/sphere를 방에 연결한다. sphere는 원래 반지름, box는 회전/크기를 반영한 반크기를 사용한다. 기본 탄성은 sphere=on, box=off다. 방은 강체 변환을 유지한다. 물리 루트에는 공간 부모나 별도 Motion을 붙이지 않는다. 상자 루트 이동·회전·크기는 물리 상태/AABB에 반영하지만 mesh 부품만 따로 변형할 수 없다. room.walls도 별도로 옮기지 않고 room 루트를 조작한다.

walk는 add walker 또는 C++로 JointRig를 등록한 루트에 연결한다. 보행 루트는 배율1, 중심 피벗, Y회전을 유지한다. 보행 공간은 월드 축 정렬이다. 장애물은 현재 월드 AABB로 읽는다. 바닥을 열면 상자와 보행 리그가 이탈·낙하하며 다시 닫아도 이탈한 객체를 방으로 되돌리지 않는다. Home 또는 load로 재구성한다.

정밀 메시 충돌·마찰·상호 강체 충돌·차량 물리는 제공하지 않는다. 수작업 사람 예제의 관절 보행 모션과 충돌을 포함한 walker는 구분한다.

## 입력·표시·카메라

```text
bind RIGHT hold "move hero 1 0 0"
bind J press "jump walking 5.5"
bind SHIFT+T press "rotate tank.turret y -5"
unbind RIGHT
color 대상 R G B
show 대상 on|off
visible 대상 면 on|off
cull 대상 면 none|back|front
select 대상
inspect 대상
list
pause on|off
view 1|3
render wire|lighting|depth on|off
camera pose 위치3 target3 up3
camera move DX DY DZ
camera yaw 도
camera orbit 축 도 중심3
camera zoom 배율
camera projection ortho|perspective
camera extent 값
camera fov 도
camera clip near far
```

press는 최초 누름만 처리한다. hold는 move/rotate 값에 dt를 곱한 속도 입력이다. 포커스를 잃으면 hold를 해제한다. CTRL/SHIFT/ALT를 조합하고 Caps/Num Lock은 바인딩 비교에서 제외한다. 중복 키는 거부하므로 unbind 후 변경한다. 숫자·모드 전환 F키·대괄호·Esc·Home은 바인딩 키 목록에 넣지 않는다.

기본 창 입력은 클릭 선택, Tab/Backspace 부품 순환, XYZ 축 선택, 좌우 회전, 상하 이동, Space 전체 정지, Home 기본 레시피다. 사용자가 등록한 키가 기본 편집보다 먼저 처리된다. Esc는 종료다. 콘솔 quit도 종료한다.

## 레시피와 직접 C++ API

save/load의 파일은 UTF-8 BOM 텍스트 명령 목록이다. 저장에는 작성 명령, 기준 SRT와 카메라·표시·정지 상태를 포함한다. hold 편집도 최종 기준 SRT로 남긴다. 순간 jump/release 이벤트는 자동 기록하지 않는다. 진행 중인 프레임·물리 속도·정확한 동작 시각의 스냅샷은 아니다.

load는 한 줄이라도 실패하면 불러오기 전 Scene·기준 상태·카메라·렌더 설정을 복원한다. 파일 명령을 중첩하지 않는다. save는 같은 디렉터리의 임시 파일에 쓴 뒤 교체하며 실패하면 기존 파일을 보존한다. 저장은 현재 작업 디렉터리 기준으로 절대화한다. load는 현재 작업 디렉터리의 상대 파일을 먼저 찾고 없으면 실행 파일 폴더에서 찾는다. 실제 사용한 절대 경로를 출력한다. 사용자 빌드 시 standard.vcxproj가 examples/assembly와 docs/assembly를 출력 폴더로 복사한다. `# @state-begin` 이후는 자동 저장 기준값 블록이다.

명령 길이는 4096바이트, add/part에 명시하는 전체 이름은 64바이트(자동 하위 별칭은 접미사가 추가됨), 이름 수는 1000개, 레시피는 2MiB·10000줄 미만이다. 값은 유한하고 절댓값1000000 이하, 균일 배율은 .0001 초과100 이하이며 보행/점프 속도는0~10이다.

직접 편집용 API는 Assembly.h의 group/part/srt/pivot/center/add/model/mount/attach/move/rotate/setRest/bind와 node 조회다. 콘솔은 같은 API를 호출한다. C++ API는 실패 시 예외를 사용한다. customCommand 안에서 호출하면 명령의 CPU 상태 복원 범위에 포함된다. 직접 초기화 함수에서 호출하면 호출자가 예외/부분 구성을 처리한다. 임의 파일 IO 같은 외부 부수효과는 Scene 복원으로 되돌리지 않는다.

Motion::action을 변환 트랙에 넣으면 거부한다. 순간 동작은 bind/customCommand에 둔다. 확장 지점은 [직접 편집 안내](CODE_GUIDE.md)를 따른다.

## 콘솔 처리의 한계

Windows 콘솔 이벤트와 사용 가능한 파이프 바이트만 읽는다. getline 대기나 배경 입력 스레드를 추가하지 않는다. 콘솔 Enter를 기다리는 동안 렌더 프레임은 진행한다. 현재 Application은 최소화 중 WaitEvents를 사용하므로 그동안 콘솔 명령은 창 복원 후 처리한다. 큰 load는 주 스레드에서 순차 적용하므로 완전 비동기 편집기가 아니다. 콘솔 모드 변경은 0번 종료 시 복원한다. 소스가 없거나 표식을 못 찾으면 guide는 줄 번호를 만들어내지 않는다.

## 면·노멀·원시 형상 확장

```text
surface 대상 면|all
mount 객체 관절이름 면 삼각형 U V 법선거리
anchor 객체 관절이름 면 X Y Z
unmount 면관절이름
normal 대상 면|all NX NY NZ
normal 대상 면|all auto|flip
orient 대상 +Z방향3 up방향3
winding 대상 면|all flip
vertex 대상 면 정점 X Y Z
triangle 이름 A3 B3 C3
triangle-add 대상 A3 B3 C3
face-srt 대상 면|all 이동3 회전도3 배율3 중심3
face-mode 대상 면|all hierarchy|independent
facecolor 대상 면|all R G B
outline 대상 면|all none|features|triangles
cull 대상 면|all inherit|none|back|front
visible 대상 면|all on|off
line 부모 A3 B3 R G B
digit 부모 숫자0~9 위치3 높이 R G B
warp group SX SY SZ
```

면·삼각형·정점 번호는 0부터 시작한다. all은 대상 하위의 실제 메시를 순회한다. normal은 조명 노멀만 변경하며 auto는 삼각형 winding에서 재계산한다. winding은 인덱스 방향만 바꾸며 조명 노멀은 바꾸지 않는다. orient는 객체 +Z/up 방향을 정하는 별도 SRT 회전이다.

mount는 P=(1-U-V)A+UB+VC이며 U,V>=0, U+V<=1이다. 마지막 값은 면의 기하 법선 방향 거리다. anchor는 지정한 원본 메시 좌표를 사용하고 첫 삼각형으로 방향을 정한다. 면 내부 투영 검사는 하지 않는다. 부착점은 면의 최종 변환을 따르고 +Z는 기하 법선, +Y는 첫 변이다. 면의 비균일 배율을 관절의 자체 배율로 복사하지 않는다. 부모 메시의 SRT는 계속 따른다. 부착 관절은 unmount 전 부모를 변경하지 않는다.

face-srt의 이동은 면 원본 좌표에서의 변위다. 프레임워크의 면 행렬은 T(pivot)·T(offset)·R·S·T(-pivot)이며 객체 srt의 basis와 구분한다. 배율은 0이 아니며 절댓값 .0001 초과100 이하다. face-mode는 현재 월드 자세를 보존하며 계층/독립 정책을 전환한다. 면 동작이 있으면 먼저 remove한다.

warp는 동작이 없는 일반 group을 정적 비균일/반사 그룹으로 바꾼다. 같은 범위의 0이 아닌 음수 배율도 허용한다. 원점/피벗/회전은 기존 명령을 사용한다. 그 아래 group/joint에 움직임을 붙여 합성한다. 일반 객체 트랙과 보행은 양수 균일 배율 계약을 유지한다. 저장의 rest3는 warp의 축별 배율을 보존하는 내부 형식이다.

## 임의 중심 면 트랙·입력·물리 확장

```text
face-track ID 대상 면 시간 지연 once|loop|pingpong move DX DY DZ
face-track ID 대상 면 시간 지연 once|loop|pingpong scale SX SY SZ CX CY CZ
face-track ID 대상 면 시간 지연 once|loop|pingpong turn AX AY AZ CX CY CZ 도
drag 대상 x|y|z 도/픽셀
drag off
body 물리ID velocity VX VY VZ
body 물리ID gravity GX GY GZ
body 물리ID bounce on|off
release 물리ID
render cull none|back|front
render light-direction X Y Z
render light-color R G B
render background R G B
```

면 트랙은 원본 메시 모델 좌표의 추가 행렬이며 부착 전 기준 행렬을 보존한다. 시간은 .0001초 이상, 지연은0 이상이다. scale은 양수 .0001 초과100 이하로 보간하여 0 통과를 피한다. 완전히 없애려면 동작을 제거한 뒤 visible off를 사용한다. 한 면에 여러 효과가 필요하면 하나의 C++ FaceTrack.motion에 복합 행렬을 넣는다.

drag는 오른쪽 버튼의 수평 커서 변위를 지정한 로컬 축 회전으로 바꾼다. 키 hold와 함께 포커스 손실 시 해제한다. body velocity는 이탈 전 방 로컬, 이탈 후 월드 속도다. gravity는 월드 중력이다. release는 바닥 상태를 바꾸지 않고 현재 방 속도/각속도 기여를 사용해 이탈시킨다.

## C++ Custom 동작과 표현 범위

Behavior의 Effect::Custom과 custom(Scene&, const Transform3D& rest, double seconds, float dt)는 임의 조건/수치/프레임 동작의 확장점이다. 기본 동작을 모두 적용한 뒤 면 부착 관절을 계산하기 전에 호출한다. dt는 정지/비재생 중0이며 seconds는 재생 중에만 증가한다. 해당 target의 위치 제어 소유권을 가지며 동일 target의 일반 Motion/물리와 충돌하면 거부한다. 자식의 별도 동작은 유지한다.

callback은 받은 기준 SRT를 합성하고 Scene을 유효하게 유지해야 한다. enabled off는 마지막 사용자 자세를 유지하며 remove하면 기준 자세로 복귀한다. 임의 외부 상태/IO는 자동 복원·저장되지 않는다. guide cpp-effect와 guide practice13~23을 통해 기존 조건 블록을 직접 옮긴다. 명령만으로 모든 실습 키·난수·상태 전이를 복제한 것은 아니며 [표현력 평가](REQUIREMENT_AUDIT.md)가 그 경계를 명시한다.

## 기능 단위 C++ 입력

Effect::Input과 Behavior.key/mouse/cursor/focus는 입력 전용 확장점이다. key/mouse/cursor는 처리했으면 true, 전달할 때 false를 반환한다. focus는 모든 활성 훅에 알린다. 입력 전용 동작은 자체 위치 제어 소유권을 잡지 않는다. disabled는 이벤트를 받지 않고 play는 입력 비활성화가 아니다.

기본 release/focus 해제 → C++ 활성 입력 훅 → 일반 바인딩/편집/피킹 순서다. 구성 변경은 토폴로지 번호를 증가시켜 이전 목록의 후속 이벤트 실행을 중단한다. 예외는 갱신을 정지하고 기본 hold/drag를 해제한다. 콜백 내부의 외부 부수효과까지 되돌리지는 않는다. 구현 예와 수명 조건은 CODE_GUIDE.md를 따른다.
