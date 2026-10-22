# 구현된 응용 패턴 설명서

이 문서는 실습 13~19의 현재 로컬 구현에서 재사용할 수 있는 구조를 설명한다. 사용법과 API 계약은 [프레임워크 사용설명서](../README.md)를 먼저 읽는다.

기준일은 2026-10-20이다. 미커밋 상태를 포함한 소스를 읽어 정리했다. 빌드·실행·PDF 요구사항 대조를 수행한 결과는 아니다. 각 실습은 자체 main과 framework 사본을 가진 독립 프로그램이다. 아래 소스 링크는 저장소 루트 framework/docs를 기준으로 하므로 framework만 따로 복사하면 실습 링크는 제공되지 않는다.

## 1. 패턴과 구현 위치

| 실습 | 응용 패턴 | 먼저 읽을 함수 |
|---|---|---|
| [13](../../openGL_3dAssign_13/Console.cpp) | 면 표시 선택, 중복 없는 무작위 선택 | initScene, selectFaces, onKey |
| [14](../../openGL_3dAssign_14/Console.cpp) | 독립 카메라, 로컬 자전, 누르는 동안 이동 | initScene, onKey, onUpdate, applyTransform, onFocus |
| [15](../../openGL_3dAssign_15/Console.cpp) | 힌지·축소·면 공전의 중첩 | rotateAround, scaleAround, beginMotion, applyFaceMotion |
| [16](../../openGL_3dAssign_16/Console.cpp) | 각뿔 전개, 순차 실행, 꼭짓점 중심 동작 | beginPyramid, updatePyramid, applyPyramid |
| [17](../../openGL_3dAssign_17/Console.cpp) | 두 객체의 자전·공전·교환·형상 변경 | begin, worldPosition, apply, onUpdate |
| [18](../../openGL_3dAssign_18/Console.cpp) | 태양계, 궤도선과 천체의 계층 분리 | initScene, circle, apply, onUpdate |
| [19](../../openGL_3dAssign_19/Console.cpp) | 탱크 부품 조립, 접지 유지, 카메라 제어 | box, reset, apply, onUpdate |
| [basic 예제](../examples/basic.cpp) | 클릭한 면에 추가 SRT 적용 | main의 마우스 콜백 |
| [assets 예제](../examples/assets.cpp) | 선택적 모델·텍스처 로드 | main의 초기화 콜백 |

각 절은 목적 → 상태 → 계산 → API·코드 → 주의점 순으로 읽는다. 계산식은 실제 코드의 핵심을 요약한 것이며, C++ 단편은 별도 프로그램 전체가 아니다.

## 2. 공통 골격: 입력·상태·장면 반영을 분리

**목적.** 프레임워크의 이벤트 루프는 유지하면서 실습별 정책을 Console에 둔다.

```text
main: Application과 Console 생성 → 콜백 등록 → Application::run
입력: Console::onKey → 누름 상태·방향·실행 플래그 변경
갱신: Console::onUpdate(dt) → 위치·각도·진행률 계산
반영: apply / applyTransform / applyFaceMotion → Scene setter
출력: Application → Renderer → 셰이더 → 화면
```

main의 콜백은 Console을 호출하는 연결이다. 등록 순서대로 매 프레임 모두 실행되는 목록이 아니다. 입력은 이벤트 처리 중, 업데이트는 프레임 갱신 때 호출한다. 실습 13은 시간 애니메이션이 없어 update 콜백 없이도 동작한다.

**상태.** Console은 Application 참조, EntityId, 각도·위치, 키 상태와 실행 플래그를 가진다. Scene이 관리하는 포인터를 장기간 보관하지 않는다.

**API·코드.** setInitHandler, setKeyHandler 또는 setModifiedKeyHandler, setUpdateHandler, setFocusHandler를 사용한다. 실습 17~19의 main에서 수정키·업데이트·포커스 연결을 볼 수 있다.

**주의.** Console을 캡처한 람다가 살아 있는 동안 Console도 살아 있어야 한다. 실습의 main처럼 Console을 만든 뒤 run을 동기 호출하면 이 수명을 유지할 수 있다. 프레임워크에 실습 번호 전용 함수나 태양계·탱크 전용 API를 추가할 필요가 없다.

## 3. 면 선택과 무작위 선택: 실습 13

**목적.** 형상을 다시 만들지 않고 표시할 면만 바꾼다.

**상태.** cuboidId_, pyramidId_와 난수 엔진 random_을 유지한다. 면 번호는 로컬 형상의 고정 번호다. 키 1~6과 면 0~5는 1만큼 차이가 난다.

**계산.** selectFaces는 모든 엔티티의 모든 면을 숨긴 다음, 선택한 엔티티의 지정 면만 표시한다. C는 육면체에서 서로 다른 두 면, T는 각뿔의 바닥 4와 임의 측면 0~3을 고른다.

```text
first  = 0~5 중 무작위
second = 0~4 중 무작위
second >= first 이면 second를 1 증가
```

이 방법은 첫 면을 제외한 나머지 5개를 정확히 대응시킨다. 서로 다른 번호가 나올 때까지 계속 추첨하는 반복이 필요 없다.

**API·코드.** setFaceVisible, getIds, getComponent. [selectFaces와 onKey](../../openGL_3dAssign_13/Console.cpp)를 읽는다.

**주의.** GLFW_PRESS만 처리한다. GLFW_REPEAT를 같은 명령으로 처리하면 키를 길게 누르는 동안 무작위 결과가 계속 달라진다. 현재 initScene은 모델에 X/Y 각 30도 초기 회전을 넣는다. 이를 실습 14의 카메라만으로 만든 표시 기울기와 구분한다.

**직접 확인.** C로 선택된 두 면이 항상 다르고, 도형을 보는 방향이 달라도 면 번호 자체는 유지되는지 확인한다.

## 4. 초기 표시와 로컬 자전: 실습 14

**목적.** 처음부터 입체적으로 보이게 하면서, 사용자가 조작하는 객체의 회전 상태는 항등행렬에서 시작한다.

**상태.** position_, rotation_, angularDirection_, pressed_를 분리한다. 초기 보기 각도는 camera.position/up에 둔다.

**계산.** initScene에서 X/Y 회전을 조합한 3×3 행렬의 전치를 구한다. 순수 회전의 전치는 역행렬이므로, 이를 초기 시점과 up에 적용한다. 모델은 rotation_=I로 시작한다.

```text
movement = (Right-Left, Up-Down)
position += normalize(movement) × 0.5 × dt   (movement≠0)
R_next = R_current × Rx(45°×dt×dirX) × Ry(45°×dt×dirY)
```

추가 회전을 오른쪽에 곱하므로 현재 객체의 로컬 축으로 회전한다. position은 별도 상태라 자전만으로 원점을 중심으로 공전하지 않는다. 대각선 이동은 정규화하여 축 방향보다 빨라지지 않게 한다.

onUpdate는 회전의 첫 기저 x를 정규화하고 z=normalize(cross(x,y)), y=cross(z,x)로 다시 구성한다. 반복 행렬 곱의 부동소수점 오차가 기저를 틀어지게 하는 것을 줄인다.

**API·코드.** Camera, setTransform. [initScene, onUpdate, applyTransform](../../openGL_3dAssign_14/Console.cpp).

**주의.** 행렬 곱 순서는 교환할 수 없다. Rx를 먼저 코드에 썼다고 모델 정점에 항상 먼저 적용되는 것은 아니다. 열벡터의 실제 적용 순서는 오른쪽부터다.

**직접 확인.** 회전 중 이동해도 위치가 별도로 유지되는지, X 회전 후 Y 회전을 했을 때 현재 로컬 축을 따르는지 본다.

## 5. 연속 이동·대소문자·포커스 처리

**목적.** 운영체제의 키 반복 속도와 무관한 이동, 일회성 토글, 대소문자 방향 조작을 함께 구현한다.

**상태.** 실습 14·18·19는 pressed_ 배열을, 실습 17은 movement_ 방향을 보관한다.

**계산.** PRESS에서 누름을 기록하고 RELEASE에서 해제한다. onUpdate에서 속도×dt를 적용한다. 모드 변경과 실행 토글은 PRESS에서 한 번만 수행한다.

```cpp
const bool upper = ((mods & GLFW_MOD_SHIFT) != 0)
                != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
```

Shift와 Caps Lock 중 하나만 켜진 경우를 대문자로 취급하는 응용 정책이다. 물리 키 코드가 대문자·소문자별로 따로 오는 것으로 가정하지 않는다. Application은 GLFW_LOCK_KEY_MODS를 활성화한다.

**API·코드.** setModifiedKeyHandler, setFocusHandler. [14](../../openGL_3dAssign_14/Console.cpp), [17](../../openGL_3dAssign_17/Console.cpp), [18](../../openGL_3dAssign_18/Console.cpp), [19](../../openGL_3dAssign_19/Console.cpp)의 onKey/onFocus.

**주의.** 포커스를 잃은 동안 release 이벤트를 못 받을 수 있다. onFocus(false)에서 pressed_ 또는 이동 방향을 해제한다. 자동 회전 같은 토글 상태까지 모두 정지하는 정책은 아니다. Application이 응용의 키 배열을 자동으로 지워주지도 않는다.

**직접 확인.** 이동 키를 누른 상태에서 다른 창으로 전환한 뒤 돌아왔을 때 이동이 남아 있지 않은지 본다.

## 6. 피벗 회전·면 축소·중첩: 실습 15

**목적.** 본체의 자전을 유지하면서 일부 면만 접거나 돌리고, 여러 동작을 겹친다.

**상태.** 본체는 spinning_, spinDegrees_, 각 면 동작은 motions_의 initialized/running/progress/direction으로 관리한다. progress의 단위는 모드에 따라 0~1 비율 또는 도(degree)다.

### 6.1 고정 피벗으로 회전·확대축소

```text
회전 H(p, axis, θ) = T(p) × R(axis, θ) × T(-p)
확대축소 K(p, s)  = T(p) × S(s) × T(-p)
```

오른쪽의 T(-p)로 피벗을 원점에 옮긴 뒤 변환하고 T(p)로 복원한다. 피벗 위의 점은 제자리에 남는다. [rotateAround, scaleAround](../../openGL_3dAssign_15/Console.cpp)가 이 수식을 구현한다.

| 모드 | 계산·대상 | 같은 키 재입력 |
|---|---|---|
| T | 위면 4를 중심 (0,b,0)의 Z축으로 회전 | 정지·재개 |
| F | 앞면 0을 위쪽 모서리 (0,b,b)의 X축으로 -90×progress도 회전 | 방향 반전 |
| S | 좌·우면 2/3을 각각 자기 중심의 X축으로 회전 | 정지·재개 |
| B | 뒷면 1의 중심에서 1-progress배로 축소 | 방향 반전 |
| 1 | 공유 모서리의 두 면을 같은 힌지 Y축으로 회전 | 정지·재개 |
| 2 | 두 면을 로컬 원점 Y축으로 반대 회전, 0→90→0도 왕복 | 정지·재개 |
| 4 | 네 측면을 로컬 원점 Y축 주위로 공전 | 정지·재개 |
| 5 | 위·아래·앞·뒤면을 로컬 원점 X축 주위로 공전 | 정지·재개 |

b=0.4다. B는 진행률 1에서 면을 숨긴다. 실제 정점 배열을 매 프레임 줄이는 방식이 아니다.

### 6.2 중첩 순서와 전체 자전

applyFaceMotion은 매 프레임 각 면의 행렬을 I로 시작한다. 활성 모션을 순서대로 왼쪽에 곱한다.

```text
matrices[face] = nextMotion × matrices[face]
정점 적용 순서: T → F → S → B → 1 → 2 → 4 → 5 → 본체 변환
M_final = M_object × M_faceAnimation
```

여기서 T/F 등은 키로 선택한 모션이며 T는 이동행렬 기호가 아니다. 면 공전축은 객체의 로컬 원점에 속하므로 본체 회전도 따라간다. 월드 원점 고정 공전과는 행렬 위치가 다르다.

**API·코드.** setFaceLocalMatrix, setFaceVisible, setTransform. [beginMotion, applyFaceMotion, onUpdate](../../openGL_3dAssign_15/Console.cpp).

**주의.** 완료된 모션은 running만 false로 바꾸고 initialized는 남겨 마지막 자세를 유지한다. 정지는 초기화와 다르다. 이전 프레임의 변형 정점을 다시 입력으로 사용하지 않기 때문에, 진행률로 같은 자세를 재구성할 수 있다.

**직접 확인.** Y와 F를 함께 실행하고, F만 반전했을 때 전체 자전은 계속되는지 본다. 모션 적용 순서를 바꾸면 겹친 면의 결과가 왜 달라질지 설명해 본다.

## 7. 사각뿔의 전개 경로와 순차 실행: 실습 16

**목적.** 측면을 늘이지 않고 모서리·꼭짓점 중심 회전으로 변형한다.

**상태.** pyramidMotion_, pyramidRunning_, pyramidProgress_, pyramidDirection_. 모드를 바꾸면 진행률 0부터 시작한다. All/Apex는 재입력 시 방향 반전, Sequence는 정지·재개, Swap은 끝점에서 방향을 뒤집어 복귀한다.

**기하.** b=HALF_SIZE=0.4, h=2b=0.8, inward=degrees(atan2(h,b))≈63.435도다. 네 밑변의 중점을 hinges, 대응 회전축을 axes로 저장한다.

| 면 | 힌지 | 축 |
|---|---|---|
| 앞 0 | (0,-b,b) | +X |
| 오른쪽 1 | (b,-b,0) | -Z |
| 뒤 2 | (0,-b,-b) | -X |
| 왼쪽 3 | (-b,-b,0) | +Z |

| 키·모드 | 현재 코드의 계산 | 진행 |
|---|---|---|
| 6 All | 힌지에서 (360-2×inward)×p≈233.130×p도 회전 | p:0~1, 약 2초 |
| 7 Sequence | 활성 한 면을 (180-inward)×삼각파≈116.565×삼각파도 회전 | 면당 2초, 네 면 8초 주기 |
| 8 Swap | 꼭짓점 (0,b,0)의 Y축으로 앞·뒤면을 ±180×p도 회전 | 약 2초 후 끝점 정지 |
| 9 Apex | 꼭짓점 (0,b,0)에서 각 axes축으로 -45×p도 회전 | 약 2초 후 끝점 정지 |

6번의 +233.130도와 -126.870도는 끝 자세는 같지만 이동 경로가 다르다. 현재 코드는 바깥쪽으로 도는 +233.130도 경로를 선택한다. 이전 설명의 -126.870도를 현재 구현 각도로 재사용하면 중간 동작이 달라진다. 이 수치는 현재 코드와 b/h의 관계이며 PDF의 표현을 검증한 판정은 아니다.

### 순차 모션의 시간 분할

```text
progress = fmod(progress + dt, 8)
active   = floor(progress / 2) % 4
phase    = fmod(progress, 2)
amount   = phase <= 1 ? phase : 2 - phase
```

한 면이 1초 열리고 1초 닫힌 뒤 다음 면으로 넘어간다. 활성 면 외의 측면은 I로 설정하므로 이전 면의 열린 자세가 남지 않는다. 바닥 4도 항등행렬로 유지한다.

**API·코드.** setFaceLocalMatrix. [beginPyramid, updatePyramid, applyPyramid](../../openGL_3dAssign_16/Console.cpp).

**주의.** 같은 최종 자세에 도달하는 여러 회전 경로 중 어느 것을 선택할지는 응용 코드의 책임이다. setFaceLocalMatrix가 경로를 자동 선택하지 않는다.

**직접 확인.** 6번은 끝 자세뿐 아니라 처음부터 바깥 방향으로 움직이는지 본다. 7번은 동시에 여러 면이 열리지 않는지, 8번은 완료 후 재입력으로 돌아오는지 본다.

## 8. 두 객체의 자전·공전·크기·교환: 실습 17

**목적.** 공전과 자전을 나누고, 교환을 시작할 때의 현재 위치에서 경로를 만든다.

**상태.** 객체마다 orbit 그룹, body 모델, position, rotation, scale, orbitDegrees를 둔다. 전역 motion_은 None/Straight/Bypass/Combined 중 하나이며 running_, elapsed_, starts_, scales_를 함께 보관한다.

```text
orbit_i (rotation = RY(orbitDegrees))
└─ body_i (basis = position, rotation = 자전 상태, scale = 객체 크기)
worldPosition_i = orbitRotation_i × position_i
```

### 자전과 공전

body.rotation을 오른쪽으로 갱신하면 자전한다. 부모 orbit.rotation을 바꾸면 body.basis도 함께 회전해 공전한다. 수동 월드 XY 이동은 R_orbit의 전치로 역회전한 뒤 body의 로컬 position에 더한다.

현재 apply는 원뿔의 표시 회전에 Z축 -45도를 추가로 곱한다. 모양별 표시 보정이며 공전 그룹의 회전 상태와는 별개다.

### 제자리 확대와 원점 기준 확대

- a/A: scale만 1.1배 또는 그 역수로 변경한다. 위치는 유지한다.
- b/B: 새 scale을 0.15~3으로 제한한 뒤 position도 `newScale / oldScale`배 한다. 제한에 도달했을 때 위치만 계속 변하지 않게 실제 적용 비율을 쓴다.

### 직선·우회 교환

begin은 현재 월드 위치 2개와 크기를 캡처한다. 다른 모드로 바꾸면 캡처를 갱신하고 elapsed=0으로 시작한다. 같은 모드는 정지·재개한다.

```text
t = min(elapsed / 2, 1)
point_i = mix(start_i, start_other, t)
우회라면 point_i.y += sign_i × 0.9 × sin(πt)
body.position_i = transpose(R_orbit_i) × point_i
```

양쪽이 반대 부호의 높이를 사용하므로 중간에 위·아래로 갈라진다. 월드 경로를 부모 로컬 좌표로 변환해, 활성 공전 그룹 아래에서도 원하는 경로를 반영한다.

### 복합 모드와 우선순위

Combined는 반대 위상의 ±35% 크기 진동, 자전, 공전을 묶는다. 이 모드에서는 독립 o.spin/o.orbitDirection 대신 Combined 전용 회전 속도를 사용한다. 정지한 Combined는 그 회전도 정지한다. Straight/Bypass의 정지 중에는 별도로 켜둔 독립 자전·공전이 계속될 수 있다.

onUpdate에서 수동 이동 후 교환 위치를 계산하므로, 교환이 실행 중이면 교환 경로가 그 프레임의 위치를 결정한다. 정지 중 수동 편집 뒤에는 starts_/scales_/elapsed를 새 자세로 갱신하여 재개 시 낡은 캡처로 돌아가지 않게 한다.

**형상 교체.** C는 모든 객체의 원뿔→구→육면체를 순환한다. setModel로 body의 ID와 부모를 유지하고 apply로 위치·회전·크기를 다시 반영한다. PivotMode::Preserve를 사용해 모델 교체 때 바닥 피벗으로 재배치하지 않는다.

**API·코드.** addGroup, addModel, setParent, setModel, setTransform. [model, begin, apply, onKey, onUpdate](../../openGL_3dAssign_17/Console.cpp).

**주의.** 한 변수에 수동 입력·공전·교환을 모두 직접 더하면 좌표계와 우선순위를 잃기 쉽다. 월드 경로와 로컬 상태를 구분하고 최종 반영 위치를 한곳으로 모은다.

**직접 확인.** 공전 중 t/u를 실행해도 두 객체가 현재 월드 위치에서 교환을 시작하는지 본다. v를 정지했을 때 회전·크기 변화가 함께 멈추는지 본다.

## 9. 태양계: 위치 프레임·궤도선·표시 크기 분리: 실습 18

**목적.** 전체 이동, 행성 공전, 달 공전, 달 궤도 회전과 자전을 독립 제어한다. 태양의 시각적 크기 변화가 행성 궤도를 늘리지 않게 한다.

**실제 계층.** 아래의 planetBody는 코드에서 지역변수 planet으로 생성하는 구다. planets_는 구가 아닌 위치 그룹이다.

```text
root_ (전체 이동)
├─ sun_ (표시용 구, 크기·tint 진동)
├─ tracks_[i] (행성 궤도선의 기울기·반지름)
└─ planets_[i] (행성 중심 위치 그룹)
   ├─ planetBody (표시용 구)
   └─ moonFrames_[i] (달 궤도 평면 회전)
      ├─ moonTracks_[i] (달 궤도선의 반지름)
      └─ moons_[i] (달의 공전 위치·자전)
         └─ owner가 달인 노란 방향선
```

**상태.** planetAngle_, moonAngle_, radius_/targetRadius_, moonTilt_, moonSpin_, sunTime_을 분리한다.

### 행성 위치와 선의 대응

```text
r_i = (2.0, 2.8, 3.6 중 하나) × radius
planetPosition = RZ(inclination_i) × RY(angle_i) × (r_i,0,0)
track.rotation = RZ(inclination_i)
track.scale = (r_i,r_i,r_i)
```

기울기는 0,+45,-45도다. 원형 선은 XZ 평면의 단위원을 96개 선분으로 만들고 owner의 변환으로 확대한다. 행성 위치는 같은 기울기와 반지름을 사용하므로 선과 천체가 같은 궤도를 따른다.

### 달의 공전·궤도 회전·자전

```text
r_moon = (0.55 + 0.15×i) × radius
a = radians(moonAngle_i)
moon.basis = (r_moon×cos(a), 0, r_moon×sin(a))
moonFrames.rotation = RZ(moonTilt)
moon.rotation = RY(moonSpin)
```

planet 위치 그룹에는 행성 궤도의 기울기를 회전 상태로 넣지 않고 계산된 위치만 넣는다. 따라서 moonTilt=0인 초기 달 궤도 3개는 모두 월드 XZ 평면이다. 행성 궤도의 기울기가 달 궤도에 자동 상속되지 않는다.

3번 키로 rotateMoons_를 켜면 달 궤도 프레임은 Z축 25도/초, 달 자체는 로컬 Y축 90도/초로 회전한다. 달 공전각은 별도로 항상 70/100/130도/초 갱신한다. 노란 방향선이 달의 owner를 사용하므로 자전을 화면에서 확인할 수 있다.

### 크기 변화가 계층에 미치는 영향

태양은 root_의 자식이며 행성 그룹의 부모가 아니다. 태양 크기 ±18% 진동은 궤도로 전파되지 않는다. moonTracks_의 scale은 선만 확대하고 moons_는 그 자식이 아니므로 달 반지름을 늘리지 않는다. 궤도 크기는 radius_로 따로 계산한다.

1번 키는 targetRadius_를 1/1.3으로 전환하고 매 프레임 최대 0.3×dt씩 접근시킨다. 전체 0.3 차이는 약 1초에 반영한다. 현재 값에서 목표가 바뀌면 그 값부터 접근한다.

**API·코드.** addGroup, addSphere, setParent, addLine(owner), setPosition, setTransform, setTint. [initScene, circle, apply, onUpdate](../../openGL_3dAssign_18/Console.cpp).

**주의.** moonFrames_의 회전축은 현재 행성 그룹에 회전이 없기 때문에 월드 Z 방향과 일치한다. 나중에 부모 회전을 추가하면 그 축의 의미도 바뀐다. 단순히 행성 표시 모델 밑에 달을 붙이면 표시 확대축소가 달 궤도에 영향을 줄 수 있다.

**직접 확인.** 처음에 달 궤도 3개가 수평인지, 2번의 태양 크기 변화가 궤도 반지름에 영향을 주지 않는지, 3번 정지 후에도 달 공전은 계속되는지 본다.

## 10. 탱크: 접지·연결점·부품 회전 분리: 실습 19

**목적.** 하부 높이가 변해도 바닥에 붙어 있고, 중앙·상부·포신·깃대가 연결된 채 독립 동작하게 한다.

**실제 계층.** 접지 높이를 전달하는 역할은 central_의 basis가 맡는다. 별도의 높이 연결 노드를 추가한 구조는 아니다.

```text
root_ (탱크 이동)
├─ lower_[0/1] (높이가 변하는 하부 표시 모델)
└─ central_ (하부 높이만큼 위로 이동 + Y 자전)
   ├─ 중앙 표시 상자
   └─ upper_[0/1] (상부 교환 경로)
      ├─ 상부 표시 상자
      ├─ guns_[i] (포신 부착 피벗 + Y 회전)
      │  └─ 포신 표시 상자
      └─ poles_[i] (깃대 부착 피벗 + X 회전)
         └─ 깃대 표시 상자
별도 ground 그룹 → 바닥 표시 상자
```

### 10.1 하부 높이 변화와 접지

```text
factor = 1 + 0.25×sin(lowerTime)
height = 0.6×factor
lower.basis.y = height/2
lower.scale.y = factor
central.basis.y = height
```

하부 모델의 기본 높이는 0.6이고 중심 원점 형상이다. 중심을 height/2에 두면 최하단은 항상 y=0이다. 중앙 그룹은 height만큼 올려 연결 높이를 맞춘다. 중앙을 하부 표시 모델의 자식으로 만들지 않아 하부의 Y 확대축소가 중앙 부품을 같이 늘리지 않는다.

### 10.2 상부 교환·포신·깃대

```text
a = π×swapProgress
upper.basis = (sign×0.65×cos(a), 0.25, sign×0.65×sin(a))
gun.basis = (0,0.25,0.4), gun.rotation = RY(sign×gunAngle)
pole.basis = (0,0.6,0), pole.rotation = RX(sign×60×sin(poleTime))
```

상부 둘은 반대 부호로 반원을 따라 교환한다. swapProgress는 약 2초에 0→1로 이동하고 끝에서 정지한다. L을 다시 누르면 복귀하며 중간 입력은 정지·재개한다.

box는 중심 원점 모델을 부착점에서 반길이만큼 옮겨 만든다. 예를 들어 포신 상자를 Z=0.55에 두고 guns_ 그룹을 회전하면 뒤쪽 부착점을 중심으로 돈다. 깃대도 높이 0.85의 중심을 Y=0.425에 두어 아래 끝을 피벗에 맞춘다.

### 10.3 이동 제한과 색상

바닥은 12×12이고 root 위치의 X/Z를 ±3.5로 제한한다. 코드는 전체 동작 반경 2.5를 남기는 고정 여유를 사용한다. 실제 모델별 충돌 판정이나 자동 경계 계산은 아니다. 부품 길이·회전 범위를 바꾸면 이 제한도 다시 검토해야 한다.

recolor는 colored_에 보관한 각 표시 모델에 setTint를 호출한다. setTint가 자식에게 전파되지 않으므로 각 ID를 순회한다. box에서 면색을 흰색으로 만드는 것은 팔레트색에 기존 면색이 곱해져 왜곡되는 것을 피하려는 구성이다.

**API·코드.** addGroup, addModel(Preserve), setParent, setTransform, setPosition, setTint. [box, reset, apply, recolor, onUpdate](../../openGL_3dAssign_19/Console.cpp).

**주의.** 표시 모델 크기와 부착 위치를 한 scale로 묶지 않는다. root_만 이동하면 모든 탱크 부품이 함께 움직이고, 별도 ground는 움직이지 않는다.

**직접 확인.** D로 높이가 진동해도 하부가 바닥을 뚫거나 뜨지 않는지, T/G/P/L을 겹쳐도 부품 연결이 유지되는지 본다.

## 11. 카메라 이동·자전·공전: 실습 19

**목적.** 카메라의 위치 이동과 시선 회전, 원점 주위 이동을 구분한다.

**상태.** cameraX_/cameraZ_는 누르는 동안 이동, cameraYaw_/cameraOrbit_는 같은 방향 재입력으로 켜고 끄는 회전 방향이다.

| 동작 | 갱신하는 값 | 보존되는 관계 |
|---|---|---|
| 이동 x/X,z/Z | position과 target에 같은 delta를 더함 | target-position이 유지됨 |
| 자전 y/Y | position 고정, 시선과 up 회전 | 카메라 위치가 유지됨 |
| 공전 r/R | position,target,up을 월드 Y축으로 회전 | 원점 주위로 카메라 전체 자세가 이동 |

자전축은 단순히 현재 up을 그대로 신뢰하지 않고, 실제 시선으로 직교기저를 만들어 구한다.

```text
forward = normalize(target-position)
right   = normalize(cross(forward, up))
localUp = normalize(cross(right, forward))
R       = rotation(localUp, yawSpeed×dt)
target  = position + R×(target-position)
up      = R×localUp
```

공전은 월드 원점 기준이다. 현재 target을 공전 중심으로 고정하는 trackball 카메라는 아니다. onUpdate의 적용 순서는 이동→자전→공전이다. 동시에 켜면 이 순서로 합성된다.

**API·코드.** getCamera, CameraSystem::validCamera. [onUpdate와 onFocus](../../openGL_3dAssign_19/Console.cpp).

**주의.** 카메라 복사본을 변경하고 validCamera가 참일 때 반영한다. 포커스 이탈은 누름 이동 상태를 지우지만 자동 자전·공전 토글까지 끄지는 않는다.

**직접 확인.** Y만 켰을 때 position이 변하지 않는지, R에서는 position도 원점 주위로 변하는지 비교한다.

## 12. 모델·UV·텍스처·클릭 선택의 조합

이 절은 공통 API와 동봉 예제에서 제공하는 응용 방법이다. 실습 13~19 전체가 외부 모델·텍스처를 사용한다는 뜻은 아니다.

### 자산 로드

1. run 전에 config.features.models/textures를 필요한 만큼 켠다.
2. 초기화 콜백에서 app.getAssets().loadPrimitive 또는 loadModel을 호출한다.
3. 결과의 성공과 error를 확인한다.
4. 반환된 pivotMode/localBasis와 함께 addModel로 등록한다.
5. UV가 필요하면 generateTexcoords 또는 setTexcoords로 준비한 뒤 Material을 설정한다.

**API·코드.** [examples/assets.cpp](../examples/assets.cpp), [ModelLoader.h](../include/ModelLoader.h), [Geometry.h](../include/Geometry.h).

**주의.** generateTexcoords는 기존 UV를 바꾼다. import 모델에 이미 적절한 UV가 있으면 불필요하게 재생성하지 않는다. 텍스처 표시는 이미지·UV·Material.useTexture·기능 설정·RenderOptions가 서로 맞아야 한다.

### 클릭한 면에 추가 변환

```text
왼쪽 클릭 → pickAt / pickAtCursor
  → entityId + faceIndex
  → 면의 피벗 계산
  → splitFace / setFaceSRT
  → 부모·본체 변환을 유지한 면 이동·회전
```

[basic.cpp](../examples/basic.cpp)는 선택 면 정점의 산술평균을 localBasis로 지정하고, 면 법선 방향으로 이동한 뒤 Z축 회전을 추가한다. 이 평균은 면적 가중 무게중심 계산이 아니다. 일반 불규칙 메시에서는 목적에 맞는 피벗을 별도로 구한다.

**주의.** splitFace는 엔티티 복제나 메시 분할 연산이 아니다. 인포트 모델의 faceIndex는 여러 삼각형을 포함한 메시일 수 있다. 단일 삼각형 조작이 필요하면 triangleIndex와 메시 구조를 별도로 다룬다. 화면 피킹은 선이나 투명 화소를 그대로 따라 판정하는 기능이 아니다.

**직접 확인.** 변형 후 다시 클릭했을 때 새 위치에서 선택되는지 본다. 렌더링과 피킹이 같은 faceWorldMatrix를 사용하는지 소스 흐름도 따라간다.

## 13. 새 응용에 적용하는 순서

1. 무엇이 입력이고 무엇이 상태인지 적는다. 키 자체와 회전각·진행률을 구분한다.
2. 위치와 축이 모델·부모·월드 중 어디에 속하는지 정한다.
3. 함께 움직일 것은 그룹으로 연결하고, 독립 시각 크기는 별도 표시 노드에 둔다.
4. 시작 자세·진행률에서 매 프레임 최종 변환을 계산한다.
5. 일회성 명령, 누름 이동, 토글, 방향 반전을 구분한다.
6. 같은 위치·각도를 여러 동작이 제어하면 합성 순서와 우선순위를 명시한다.
7. Scene setter의 실패를 처리하고, clear 뒤의 ID와 포인터를 갱신한다.
8. 사용자가 빌드·실행해 시작 자세, 중간 경로, 끝 자세, 정지·재개·초기화를 각각 확인한다.

회상 질문: 태양의 표시 모델에 행성 궤도를 직접 연결하면, 태양의 scale을 바꿀 때 행성 위치와 달 궤도에 어떤 변화가 생기는가? 현재 실습 18의 계층이 그 영향을 어떻게 분리하는지 설명해 본다.

## 14. 참고와 검증 범위

기존 문서에 기록된 과거 참고 코드는 [ComputerGraphics 고정 리비전](https://github.com/maru3172/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a)이다. 이번 설명은 그 외부 저장소를 재검증하거나 폴더 번호를 매핑한 결과가 아니며, 위에 연결한 현재 로컬 코드를 근거로 한다. 기존 문서의 원본 직접 복사·GL_POLYGON·중복 타이머 도입을 피한다는 구분을 유지한다.

빌드·테스트는 사용자에게 맡긴다. 특히 실습 16의 회전 경로, 17의 정지·재개와 교환, 18의 초기 달 궤도 및 방향선, 19의 접지와 카메라는 화면에서 확인해야 한다. [사용자 확인 항목](USER_CHECKS.md)도 함께 참고한다.

## 15. Motion과 레이어로 공유 동작 조립 (2026-10-22)

**목적.** 실습별 각도·시간 계산을 대체할 수 있는 공통 동작 API를 제공한다. 기존 실습의 Console을 새 API로 자동 이식하지는 않는다. 기본 계약은 [사용설명서 13절](../README.md#13-변환motion레이어-트리-2026-10-22)에 있다.

**입력 -> 상태 -> 출력.** 축·중심·속도 또는 path/interpolation -> MotionStep 목록과 MotionId별 누적 시각 -> World/Local 행렬 -> 레이어 가지 -> Scene::worldMatrix/faceWorldMatrix -> Renderer의 uModel/uMVP/uNormalMatrix 및 같은 행렬을 사용하는 피킹.

**패턴 A: 공전과 자전 분리.** createOrbitAndSpin은 서로 다른 축·초당 각속도를 받는다. 공전은 World, 자전은 Local에 합성한다. 원형 공전이면 기존 배치의 중심을 축 주위로 돌리고, 타원·사각형·다각형 궤적이면 PathMotion과 createPathAndSpin을 사용한다. 경로 이동만 원하면 Motion::followPath 하나만 추가한다.

**패턴 B: 부모 그룹과 개별 자전.** addGroup/setParent는 기존 조립 위치를 관리한다. 그룹에 연결된 Local Motion은 자식 모두에 적용된다. 자식이 직접 다른 레이어에 연결되면 그 가지가 그룹에서 상속한 가지를 대체한다. 그룹 공전과 자식 자전을 함께 적용하려면 레이어를 `root -> 공전 -> 자전`으로 구성하여 자식을 말단에 연결한다. 같은 변환을 두 트리에서 중복 평가하지 않는다.

**패턴 C: 같은 경로를 공유하고 한쪽을 반사.** [motion.cpp](../examples/motion.cpp)는 다음 구조를 만든다.

```text
root (전체 Y 공전; 초기 기여 비활성)
├─ 일반 경로 (pathId) -> 개별 자전 (spinId) -> 첫 객체 바인딩
└─ 반사 행렬 -> 반사 쪽 경로 (같은 pathId) -> 개별 자전 (같은 spinId) -> 둘째 객체 바인딩
```

두 가지는 같은 pathId·spinId를 참조하므로 시계가 동일하다. 반사가 경로보다 바깥에 있어 경로 이동량과 형상을 함께 반사한다. 반사를 경로 아래에 놓으면 이동량은 그대로이고 그 안쪽 좌표만 반사된다. 일반 객체의 위치가 (2,1,0)이면 YZ평면 반사 객체는 (-2,1,0), 점대칭 객체는 (-2,-1,0), Y축 직선대칭 객체는 (-2,1,0)이다. 평면과 직선 대칭을 구분하려면 z가 0이 아닌 사용자 경로(4번)를 관찰한다.

**패턴 D: 스택 편집.** 예제의 I키는 자전 말단 위에 Local 1.5배 레이어를 삽입한다. 말단 ID와 객체 바인딩은 유지되며 크기만 바뀐다. 다시 I키를 누르면 removeLayer로 삽입 노드를 제거하고 자전 레이어를 원래 부모로 승격한다. pushLayer가 반환한 말단을 계속 부모로 사용하면 push, popLayer로 그 말단을 제거하면 pop이다. 분기 노드에 popLayer를 호출하면 실패하므로 removeLayer 또는 연결 변경을 명시적으로 사용한다.

**패턴 E: 시간 순서 실행.** append는 동시 평가, then은 유한한 이전 종료 시각 뒤에 배치한다. 이전 결과는 유지된다. 예를 들어 X 1단위 이동 후 Y 1단위 이동은 최종적으로 (1,1,0)의 추가 이동이다. 무기한 자전 뒤 then은 거부하므로 먼저 Step.durationSeconds를 지정하거나 서로 다른 레이어에서 동시에 실행한다.

**주의.** PathMotion의 결과는 기존 위치에 대한 추가 변환이다. 기존 모델 위치가 0이 아닐 때 경로의 중심을 그대로 넣으면 두 이동이 합성된다. 모델 원점·localBasis·축 기준점을 맞추고 World/Local을 선택한다. 타원 경로는 각도 매개변수 속도와 실제 이동 속도가 다르다. 모든 사용자 함수는 평가 중 Scene이나 MotionSystem을 수정하지 않는 계산 함수로 작성한다.

**사용자 확인.** 경로·반사·자전을 각각 켜고 끈 뒤 합성한다. Space는 마지막 자세에서 시간을 정지·재개하고, S는 자전 기여 자체를 제거·복구한다. 두 기능의 결과가 다름을 확인한다. 새 예제는 독립 main이며 기존 실습 13~19의 키 기능을 바꾸지 않는다.

