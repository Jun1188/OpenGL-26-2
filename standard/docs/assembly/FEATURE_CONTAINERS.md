# 1~22 기능 컨테이너와 핫로드 조립

2026-11-03. 사용자 확정 범위는 standard의1~22 모드다. 원본 2D 프로젝트 추가 이식은 이 범위에 포함하지 않는다. 빌드·실행 검증은 사용자에게 맡긴다.

## 시작

어느 실행 모드에서든 **Ctrl+F5**를 누르면0번 작업공간에 들어가 `examples/assembly/workspace.assembly`를 읽는다. 고정 initialMode에서도 이 전용 편집 키는 사용할 수 있다. 숫자/F키를 통한 일반 모드 전환은 기존 고정 규칙을 유지한다. 이후 Ctrl+F5는 마지막으로 성공한 load의 파일을 다시 읽는다.

```text
hotload "examples/assembly/workspace.assembly"
reload
```

hotload는 재로드할 경로를 지정하고 reload/Ctrl+F5가 실제로 읽는다. 파일을 편집하다 잘못된 줄이 생기면 이전 조립 상태를 유지한다. C++ 소스 변경은 사용자 재빌드 후 실행해야 한다.

## 실습 전체와 부분 기능 조립

```text
clear
feature add swingLab 9
feature add tankLab 16
move swingLab -5 0 0
move tankLab 5 0 0
feature parts tankLab
feature list
```

add는 각 실습의 원래 Console/PracticeMode 상태, CPU Scene, Motion 시계, 카메라, 표시 옵션을 따로 보관한다. 기존 실습의 initScene이 Scene.clear를 호출해도 다른 컨테이너를 지우지 않는다. Application/GLFW 창/GL 컨텍스트는 기존 하나만 사용한다. 렌더 콜백을 여러 개 중첩 등록하지 않는다.

출력은 컨테이너 루트와 `이름.e<원본EntityId>` 포트로 분리된다. 단순히 정지 화면을 복사하는 것이 아니다. 원본 update와 Motion을 프레임당 한 번 실행하고 최종 면 프레임을 동기화한다. 기존 면 변환·노멀·반사·면 가시성을 그대로 사용하고, 조립 외곽 SRT/관절/추가 면 모션은 별도 층으로 합성한다.

```text
feature port tankLab 3 turretPort
feature enable tankLab off
center turretPort keep
spin extraSpin turretPort y 15
```

위3은 형식 예다. **먼저 feature parts가 출력한 실제 원본 엔티티 ID를 선택한다.** port는 그 엔티티와 하위 계층만 꺼내는 라이브 포트다. 원본과 상태·시계를 공유하므로 play/reset도 함께 적용한다. 독립 동작을 원하면 같은 모드로 add를 다시 한다. 인스턴스/포트 합계64개, 등록 이름1000개 제한이다.

라이브 형상/면 속성은 원본 제어기가 소유한다. 정점/노멀/면 속성을 자유롭게 편집하려면 독립 복사한다.

```text
feature snapshot tankLab 3 frozenPart
normal frozenPart all auto
cull frozenPart all back
pivot frozenPart center keep
```

snapshot도 실제 메시 ID를 선택한다. 당시 면의 자세와 노멀을 구워 새 Model로 만든다. 원본의 외곽 배치는 부모로 유지하지만 이후 원본 관절 동작은 따라가지 않는다. 저장 파일은 레시피이므로 재로드 시 원본 초기 상태에서 다시 만드는 차이가 있다. 정확한 실행 프레임/난수/물리 속도 저장은 제공하지 않는다.

## 기능 포트의 종류

| 포트 | 조립/제어 |
|---|---|
| 구조·형상 | 전체 루트/하위 entity 포트, parent/SRT/pivot/center, snapshot 후 원시 형상 편집 |
| 시간·동작 | feature play, 원본 실습의 토글·역재생·단계 동작, 추가 Motion/관절 |
| 입력 | feature focus, feature key, 실제 키·마우스·커서·focus 전달 |
| 카메라·표시 | feature camera, 공통 view1/3, 원본의 투영·깊이·와이어·조명 옵션 |
| 독립 조립 기능 | add/part/joint/mount, Motion/FaceTrack/Body/Walk/Custom/Input API |

구조 포트를 꺼낸다고 원본 클래스의 비공개 물리 변수까지 독립 시뮬레이터로 자동 분리되는 것은 아니다. 실습 내부 충돌은 각 컨테이너 로컬 공간에 유지한다. 컨테이너 사이의 물리 상호작용은 자동 생성하지 않는다. 여러 객체가 같은 방/장애물과 충돌해야 하면 공통 Assembly의 room/physics/walk/obstacle을 사용한다. 새로운 결합 규칙은 GuideRecipes가 안내하는 C++ 기능으로 확장한다.

## 입력과 카메라 선택

```text
feature focus tankLab
feature camera tankLab
feature key tankLab D press
feature focus none
feature camera none
```

포커스가 있으면 숫자1~9도 모드 전환 대신 해당 실습에 전달한다. Alt+숫자는 기존 모드 전환이다. 포커스 해제는 조립 선택/편집으로 복귀한다. Native 컨테이너로 Q/Esc/내부 Ctrl+F5는 보내지 않는다. 앱 종료는 상위 Esc, 숨기기는 feature enable off를 사용한다.

focus/창 포커스 손실은 누름/드래그를 해제한다. 원본1~9의 선택은 실제 조립 Scene과 현재 뷰의 피킹 결과를 원본 ID로 변환한다. 외곽 이동·회전·배율·반사와3뷰에서도 표시 위치로 선택한다. 원본1~9의 V는 작업공간 view1/3에 연결한다. 나머지 실습의 전체3뷰는 view3으로 지정한다.

feature camera는 그 컨테이너 카메라와 RenderOptions를 작업공간의 공통 뷰에 연결한다. 카메라 포트의 외곽 부모는 양수 균일 SRT여야 한다. 렌더 옵션/카메라는 작업공간 공통 자원이며 동시에 여러 소유자를 두지 않는다. 카메라 none이면 콘솔 camera/render 명령을 사용한다.

## 상태 경계와 실패 복원

native 콜백을 호출하는 동안만 CPU Scene/카메라/옵션을 교환하고 RAII로 복원한다. 원본 컨트롤러가 실패 종료를 요청하면 호스트 종료 플래그를 복원하고 작업공간을 정지시킨다. 이미 적분한 원본 내부 상태를 역연산하지는 않는다. feature reset 이름 또는 reload로 재구성한다. 기능 입력 큐는256개 한도이며 초과 시 누름 상태를 해제한다.

라이브 포트는 같은 프로그램을 공유하므로 update를 중복 실행하지 않는다. eN 번호는 원본 구조 변경/재빌드 뒤 다시 확인한다. source initScene에서 엔티티 생성 순서를 바꿨다면 parts와 기존 조립 참조를 함께 갱신한다.

## C++ API

```cpp
scene.addFeature("swingLab", 9);
scene.addFeature("tankLab", 16);
scene.move("tankLab", {5,0,0});
// parts에서 확인한 native id를 사용한다.
scene.portFeature("tankLab", nativeId, "turretPort");
```

AssemblyFeatures.cpp의 addFeature/portFeature와 FeatureContainer.cpp의 Context를 guide feature로 찾는다. 기본 형상·물리·입력 기능을 직접 만드는 API와 실습 컨테이너 어댑터를 같은 Assembly에 조립할 수 있다.

## 모드별 원본과 분해 지점

| 모드 | 원본 구현 | 재사용되는 상태/기능 |
|---|---|---|
| 1~6 | Mode1Ellipse~Mode6Polyline + MotionCommon + Editor | 경로·자전·공전·반사·크기·선·객체 편집 |
| 7 | Mode7Grid + AssemblyParts | 격자·거리 경로·이동 상자·번호 |
| 8 | Mode8Room | 방·물리 상자·바닥 개방·이탈 |
| 9 | Mode9Animation + Animation | 사람·그네·관절·보간 |
| 10 | Mode10Practice13 | 면 표시·다른 두 면 선택·난수 |
| 11 | Mode11Practice14 | 형상·로컬 SRT·깊이·와이어 |
| 12 | Mode12Practice15 + FaceCommon | 면별 복합 변환·토글·역재생 |
| 13 | Mode13Practice16 | 사각뿔·순차 펼침·대향면 교환 |
| 14 | Mode14Practice17 | 교환·우회·자전·공전·배율 |
| 15 | Mode15Practice18 | 태양·행성·달·기울어진 궤도 |
| 16 | Mode16Practice19 + TankCommon | 탱크 부품·이동·교환·카메라 |
| 17 | Mode17Practice20 | 컨베이어·택배·거리 자세·카메라 |
| 18 | Mode18Practice21 | 방 드래그·다중 공·이탈·낙하 |
| 19 | Mode19Practice22 + TankCommon | 탱크 상태 + 작업공간 view3 |
| 20 | Mode20Practice23 | 보행·점프·장애물·벽 개방 |
| 21 | Mode21CameraCollision | 카메라 충돌·연속 입력·포커스 해제 |
| 22 | Mode22EmbeddedMesh | 내장 정점·UV·삼각형 검사·메시 |

`examples/assembly/modes/mode01.assembly`부터 mode22.assembly까지 각 모드를 컨테이너로 여는 파일을 제공한다. 실행 확인은 [사용자 절차](USER_CHECKS.md)를 따른다.
