# 모드별 입력 → 상태 → 출력 평가

2026-11-03. 소스 대조 결과다. 빌드·테스트·앱 실행으로 정상 동작을 입증한 문서가 아니다.

## 발견과 반영

| 이전 코드 | 반영한 처리 | 범위 |
|---|---|---|
| 9에서만 관절 키 처리 | 공통 Editor와 기준 자세 트랙 | 모드 1~9 |
| 클릭은 ID만 출력 | 메시→부모 체인→등록 객체 루트 선택 | 모드 1~9 |
| 다음 프레임에 경로/물리가 변환 덮어쓰기 | 경로 프레임과 편집 자식 분리, 물리 상태 이동 | 7·8 |
| 7~9에서 1~6 처리 누락 | 공통 모드 라우팅 | nullopt |
| 컬링된 방 벽도 ray와 교차 | 면·상속 컬링과 반사 winding 반영 | 루트 framework와 standard 동봉본 |
| 초기화가 9 키에 의존 | Home으로 전체 복원 | 고정 모드 포함 1~9 |
| 실습 패턴은 문서 코드만 존재 | 상태별 실행 모드와 모든 콜백 연결 | 10~22 |
| 실습의 숫자 키와 모드 전환 충돌 | 데모 숫자 / 실습 숫자 / Alt+숫자 / F키 구분 | 공통 라우터 |
| 교환 중 수동 위치 덮어쓰기 | motionOffsets를 기준 경로에 합성 | 14(실습17) |
| 합성 동작이 개별 spin/orbit를 대체 | 각 기여를 합산, 배율 기준 수정 유지 | 14(실습17) |
| 달 궤도가 로컬 XZ 기준 | 카메라 right/forward/up 프레임 공유 | 15(실습18) |
| E/F가 같은 토글 | E는 이탈, F는 바닥 토글 | 18(실습21) |
| 즉시 이탈 경로에서 각속도 누락 | omega × 위치를 월드 속도에 합산 | 18(실습21) |
| WASD 반복이 보행 정지를 해제 | 방향 설정·보행 시작은 PRESS에서만 | 20(실습23) |

## 재사용과 소유권

기존 실습 Console 코드와 문서 코드를 대조했다. 변경 전 차이는 공통 보조 함수 추출·include·설명 주석이었다. 현재 standard 실행용 코드는 원본 실습 폴더를 include하지 않는다. 원본 실습 프로젝트의 입력 정책을 이번 작업에서 일괄 교체하지 않았다.

FaceCommon은 실습 15의 육면체 동작을 공유하고 실습16은 피라미드 상태만 확장한다. 초기 색과 카메라 범위의 차이는 각 초기화에 유지한다. TankCommon은 실습19/22의 상태·입력·물리 배치·카메라·색을 공유하며 세 뷰 여부만 파일별 팩토리에서 선택한다.

## 세부 패턴 대응

아래 116개 표 항목은 요구한 코드 경로가 실행 모드에 연결됐다는 뜻이다. 모든 요구사항의 화면 결과를 검증했다는 뜻은 아니다. SOURCE_NOTES의 원본 문서 차이 중 화면 평행 궤도·수동 이동 덮어쓰기·E/F는 위 standard 실행 모드에서 보완했다. 실제 과제 원문과 기존 실습의 나머지 차이는 보존한다.

## 모드 10: 실습 13

[실행 파일](../modes/Mode10Practice13.cpp) · [상세 원문 패턴](practice-cookbook/P13.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P13-01 | 육면체·사각뿔의 초기 배치와 기울기 | 시작 | [initScene](practice-cookbook/P13.md#fn-initscene) | 초기 모델 회전과 표시용 카메라를 함께 확인한다. |
| P13-02 | 움직이지 않는 XYZ 좌표축 | 시작 | [initScene](practice-cookbook/P13.md#fn-initscene) | 좌표축 선을 객체 owner 없이 생성한다. |
| P13-03 | 육면체의 지정한 한 면만 표시 | 1~6 | [onKey](practice-cookbook/P13.md#fn-onkey) · [selectFaces](practice-cookbook/P13.md#fn-selectfaces) | 이전 표시를 먼저 지우고 선택 면만 켠다. |
| P13-04 | 사각뿔의 지정한 옆면만 표시 | 7~0 | [onKey](practice-cookbook/P13.md#fn-onkey) · [selectFaces](practice-cookbook/P13.md#fn-selectfaces) | 삼각 옆면 인덱스 0~3을 선택한다. |
| P13-05 | 서로 다른 육면체 두 면 무작위 선택 | C | [onKey](practice-cookbook/P13.md#fn-onkey) · [selectFaces](practice-cookbook/P13.md#fn-selectfaces) | 5개 후보에서 뽑은 두 번째 번호를 첫 번호 이후에서 보정한다. |
| P13-06 | 사각뿔 바닥과 임의 옆면 표시 | T | [onKey](practice-cookbook/P13.md#fn-onkey) · [selectFaces](practice-cookbook/P13.md#fn-selectfaces) | 바닥 4와 옆면 0~3 하나를 함께 켠다. |
| P13-07 | 면마다 다른 색과 깊이 가림 | 시작 | [initScene](practice-cookbook/P13.md#fn-initscene) | 기본 모델의 면 색과 RenderOptions.depthTest를 사용한다. |

## 모드 11: 실습 14

[실행 파일](../modes/Mode11Practice14.cpp) · [상세 원문 패턴](practice-cookbook/P14.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P14-01 | 같은 꼭짓점에 같은 색을 배정 | 시작 | [initScene](practice-cookbook/P14.md#fn-initscene) | 공통 assignVertexColors가 좌표 부호 3비트를 색 인덱스로 사용한다. |
| P14-02 | 도형을 바꿔도 위치·회전 보존 | C/P | [selectShape](practice-cookbook/P14.md#fn-selectshape) · [applyTransform](practice-cookbook/P14.md#fn-applytransform) | 도형 선택 상태와 조작 변환을 분리한다. |
| P14-03 | 은면 제거 켜기·끄기 | H | [onKey](practice-cookbook/P14.md#fn-onkey) | 렌더 옵션만 바꾼다. |
| P14-04 | 와이어·솔리드 선택 | w/W | [onKey](practice-cookbook/P14.md#fn-onkey) | Shift와 Caps Lock의 조합으로 대문자를 해석한다. |
| P14-05 | 현재 로컬 X축 양·음 자전 | x/X | [onKey](practice-cookbook/P14.md#fn-onkey) · [onUpdate](practice-cookbook/P14.md#fn-onupdate) · [applyTransform](practice-cookbook/P14.md#fn-applytransform) | 회전 기저의 오른쪽에 증분 회전을 곱한다. |
| P14-06 | 현재 로컬 Y축 양·음 자전 | y/Y | [onKey](practice-cookbook/P14.md#fn-onkey) · [onUpdate](practice-cookbook/P14.md#fn-onupdate) · [applyTransform](practice-cookbook/P14.md#fn-applytransform) | 초기 표시 각도를 별도 카메라에 둔다. |
| P14-07 | 누르는 동안 XY 이동 | 방향키 | [onKey](practice-cookbook/P14.md#fn-onkey) · [onUpdate](practice-cookbook/P14.md#fn-onupdate) | 키 해제에서 속도 입력을 지우며 dt로 이동한다. |
| P14-08 | 객체가 움직여도 좌표축 고정 | 이동·회전 | [initScene](practice-cookbook/P14.md#fn-initscene) · [applyTransform](practice-cookbook/P14.md#fn-applytransform) | 축 선을 모델 자식으로 넣지 않는다. |
| P14-09 | 조작 상태·속도 초기화 | S | [resetMotion](practice-cookbook/P14.md#fn-resetmotion) · [onKey](practice-cookbook/P14.md#fn-onkey) | 이동·회전을 지우고 모든 애니메이션을 멈춘다. |
| P14-10 | 포커스 이탈 때 지속 입력 해제 | 포커스 | [onFocus](practice-cookbook/P14.md#fn-onfocus) | 키 해제 이벤트가 없어도 이동을 정지한다. |

## 모드 12: 실습 15

[실행 파일](../modes/Mode12Practice15.cpp) · [공통 면 동작](../modes/FaceCommon.cpp) · [상세 원문 패턴](practice-cookbook/P15.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P15-01 | 모델 전체 Y 자전과 면 동작 중첩 | Y | [onKey](practice-cookbook/P15.md#fn-onkey) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) · [applyObjectTransform](practice-cookbook/P15.md#fn-applyobjecttransform) | 본체 SRT와 면별 행렬을 따로 저장한다. |
| P15-02 | 윗면 중심 Z축 자전 | T | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) · [applyFaceMotion](practice-cookbook/P15.md#fn-applyfacemotion) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) | Top case: 윗면 중심을 고정한다. |
| P15-03 | 앞면 상단 모서리 힌지 열기·닫기 | F | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) · [applyFaceMotion](practice-cookbook/P15.md#fn-applyfacemotion) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) | Front case: 진행률 0~1을 -90도로 바꾼다. |
| P15-04 | 왼쪽·오른쪽 면의 제자리 X축 회전 | S | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) · [applyFaceMotion](practice-cookbook/P15.md#fn-applyfacemotion) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) | Side case: 두 면의 중심을 각각 사용한다. |
| P15-05 | 뒷면 중심 축소·소멸·복원 | B | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) · [applyFaceMotion](practice-cookbook/P15.md#fn-applyfacemotion) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) | Back case: 0 크기에 도달한 면을 숨긴다. |
| P15-06 | 서로 이웃한 옆면 쌍 선택 유지 | 1/2 첫 입력 | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) | 순환 쌍 0-3, 3-1, 1-2, 2-0과 공유 모서리를 함께 저장한다. |
| P15-07 | 이웃한 두 면이 공유 모서리를 중심으로 360도 회전 | 1 | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) · [applyFaceMotion](practice-cookbook/P15.md#fn-applyfacemotion) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) | Hinge case: 선택한 두 면에 같은 모서리 회전을 적용한다. |
| P15-08 | 이웃한 두 면의 반대 방향 90도 공전 교환 | 2 | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) · [applyFaceMotion](practice-cookbook/P15.md#fn-applyfacemotion) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) | Swap case: 0→90→0도의 삼각 위상으로 교환·복귀한다. |
| P15-09 | 네 옆면의 모델 중심 Y축 공전 | 4 | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) · [applyFaceMotion](practice-cookbook/P15.md#fn-applyfacemotion) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) | OrbitY case: 면 0,1,2,3을 함께 돌린다. |
| P15-10 | 위·아래·앞·뒤 면의 모델 중심 X축 공전 | 5 | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) · [applyFaceMotion](practice-cookbook/P15.md#fn-applyfacemotion) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) | OrbitX case: 면 4,5,0,1을 함께 돌린다. |
| P15-11 | 동작별 정지·재개와 방향 반전 구분 | 반복 키 | [beginMotion](practice-cookbook/P15.md#fn-beginmotion) · [onUpdate](practice-cookbook/P15.md#fn-onupdate) | F/B는 방향 반전, 나머지는 각 동작의 running만 토글한다. |
| P15-12 | 여러 면 동작의 고정 순서 합성 | 동시 입력 | [applyFaceMotion](practice-cookbook/P15.md#fn-applyfacemotion) | 매 프레임 항등 행렬에서 시작해 enum 순서로 왼쪽 곱한다. |
| P15-13 | 도형 전환과 전체 리셋 | P/R | [selectShape](practice-cookbook/P15.md#fn-selectshape) · [resetFaces](practice-cookbook/P15.md#fn-resetfaces) · [resetAll](practice-cookbook/P15.md#fn-resetall) | 면 상태 초기화와 본체 전체 초기화를 구분한다. |

## 모드 13: 실습 16

[실행 파일](../modes/Mode13Practice16.cpp) · [공통 육면체](../modes/FaceCommon.cpp) · [상세 원문 패턴](practice-cookbook/P16.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P16-01 | 실습 15의 육면체 동작 유지 | T/F/S/B/1/2/4/5 | [beginMotion](practice-cookbook/P16.md#fn-beginmotion) · [applyFaceMotion](practice-cookbook/P16.md#fn-applyfacemotion) · [onUpdate](practice-cookbook/P16.md#fn-onupdate) | 육면체 부분은 실습 15와 같은 상태·합성 패턴이다. |
| P16-02 | 사각뿔 네 면을 아래에서 만나도록 열기 | 6 | [beginPyramid](practice-cookbook/P16.md#fn-beginpyramid) · [applyPyramid](practice-cookbook/P16.md#fn-applypyramid) · [updatePyramid](practice-cookbook/P16.md#fn-updatepyramid) | All: 실제 기하에서 회전각을 구한다. PDF의 목표 자세와 행렬 각도를 구분한다. |
| P16-03 | 사각뿔 네 면을 순서대로 열고 닫기 | 7 | [beginPyramid](practice-cookbook/P16.md#fn-beginpyramid) · [applyPyramid](practice-cookbook/P16.md#fn-applypyramid) · [updatePyramid](practice-cookbook/P16.md#fn-updatepyramid) | 8초 주기, 각 면의 2초 구간에서 0→평면→0을 계산한다. |
| P16-04 | 마주보는 두 사각뿔 옆면의 꼭짓점 기준 교환 | 8 | [beginPyramid](practice-cookbook/P16.md#fn-beginpyramid) · [applyPyramid](practice-cookbook/P16.md#fn-applypyramid) · [updatePyramid](practice-cookbook/P16.md#fn-updatepyramid) | Swap: 꼭대기를 지나는 Y축에서 두 면에 반대 부호를 준다. |
| P16-05 | 사각뿔 꼭대기 고정 바깥쪽 펼치기 | 9 | [beginPyramid](practice-cookbook/P16.md#fn-beginpyramid) · [applyPyramid](practice-cookbook/P16.md#fn-applypyramid) · [updatePyramid](practice-cookbook/P16.md#fn-updatepyramid) | Apex: 면별 축으로 -45도까지 연다. |
| P16-06 | 사각뿔 모드 전환·정지·역방향 복귀 | 6~9 재입력 | [beginPyramid](practice-cookbook/P16.md#fn-beginpyramid) · [selectShape](practice-cookbook/P16.md#fn-selectshape) | 새 모드는 닫힌 자세에서 시작하고 완료한 교환은 반대 방향으로 복귀한다. |
| P16-07 | 사각뿔 바닥의 고정 | 모든 사각뿔 동작 | [applyPyramid](practice-cookbook/P16.md#fn-applypyramid) | 면 4의 추가 행렬은 항등으로 둔다. |

## 모드 14: 실습 17

[실행 파일](../modes/Mode14Practice17.cpp) · [상세 원문 패턴](practice-cookbook/P17.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P17-01 | 왼쪽·오른쪽·두 객체의 선택 | 1/2/3 | [onKey](practice-cookbook/P17.md#fn-onkey) | 선택 상태로 개별 조작의 적용 대상을 정한다. |
| P17-02 | 공전 그룹과 표시 메시 분리 | 시작 | [reset](practice-cookbook/P17.md#fn-reset) · [apply](practice-cookbook/P17.md#fn-apply) · [orbitRotation](practice-cookbook/P17.md#fn-orbitrotation) · [worldPosition](practice-cookbook/P17.md#fn-worldposition) | 공전 프레임 아래에 몸체의 위치·자전·크기를 둔다. |
| P17-03 | 선택 객체의 로컬 X/Y 자전 | x/X/y/Y | [onKey](practice-cookbook/P17.md#fn-onkey) · [onUpdate](practice-cookbook/P17.md#fn-onupdate) · [apply](practice-cookbook/P17.md#fn-apply) | 자전 기저를 보관하고 수치 드리프트를 다시 직교화한다. |
| P17-04 | 월드 원점 Y축 공전 | r/R | [onKey](practice-cookbook/P17.md#fn-onkey) · [orbitRotation](practice-cookbook/P17.md#fn-orbitrotation) · [apply](practice-cookbook/P17.md#fn-apply) | 본체 자전과 공전 그룹의 회전 상태를 분리한다. |
| P17-05 | 제자리 확대·축소 | a/A | [onKey](practice-cookbook/P17.md#fn-onkey) · [apply](practice-cookbook/P17.md#fn-apply) | 표시 크기만 바꾸고 위치는 유지한다. |
| P17-06 | 원점 기준 위치와 크기 동시 확대·축소 | b/B | [onKey](practice-cookbook/P17.md#fn-onkey) · [apply](practice-cookbook/P17.md#fn-apply) | 위치와 배율을 같은 비율로 바꾼다. |
| P17-07 | 공전 중 화면 XY 방향 이동 | d/D/e/E | [onKey](practice-cookbook/P17.md#fn-onkey) · [onUpdate](practice-cookbook/P17.md#fn-onupdate) | 원하는 월드 이동을 공전 회전의 전치로 부모 공간에 옮긴다. |
| P17-08 | 두 객체의 원점을 통과하는 직선 교환 | T | [begin](practice-cookbook/P17.md#fn-begin) · [onUpdate](practice-cookbook/P17.md#fn-onupdate) | 시작 월드 위치를 저장하고 2초 동안 서로의 위치로 보간한다. |
| P17-09 | 위·아래로 우회하는 교환 | U | [begin](practice-cookbook/P17.md#fn-begin) · [onUpdate](practice-cookbook/P17.md#fn-onupdate) | 직선 보간에 부호가 반대인 sin(pi*t) 높이를 더한다. |
| P17-10 | 반대 위상 확대·축소와 자전·공전 | V | [begin](practice-cookbook/P17.md#fn-begin) · [onUpdate](practice-cookbook/P17.md#fn-onupdate) | 시작 배율과 위상을 저장하며 모드 정지는 합성 동작만 멈춘다. |
| P17-11 | 진행 중 모드 전환과 같은 모드 정지·재개 | T/U/V 반복 | [begin](practice-cookbook/P17.md#fn-begin) · [onUpdate](practice-cookbook/P17.md#fn-onupdate) | 전환 시 현재 월드 위치를 새 시작점으로 잡는다. |
| P17-12 | 두 객체의 형상 순환과 SRT 유지 | C | [model](practice-cookbook/P17.md#fn-model) · [onKey](practice-cookbook/P17.md#fn-onkey) · [apply](practice-cookbook/P17.md#fn-apply) | 원뿔→구→육면체 모델만 교체한다. |
| P17-13 | 초기화·포커스 이탈 입력 정리 | S/포커스 | [reset](practice-cookbook/P17.md#fn-reset) · [onFocus](practice-cookbook/P17.md#fn-onfocus) | 선택·모션·이동 입력을 초기 상태로 되돌린다. |

## 모드 15: 실습 18

[실행 파일](../modes/Mode15Practice18.cpp) · [상세 원문 패턴](practice-cookbook/P18.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P18-01 | 중심·행성·달의 계층 구성 | 시작 | [initScene](practice-cookbook/P18.md#fn-initscene) | 표시 메시 크기를 궤도 프레임 배율과 분리한다. |
| P18-02 | 세 행성의 서로 다른 궤도 기울기·속도 | 자동 | [initScene](practice-cookbook/P18.md#fn-initscene) · [apply](practice-cookbook/P18.md#fn-apply) · [onUpdate](practice-cookbook/P18.md#fn-onupdate) | XZ와 Z축 ±45도 기울기를 위치와 선에 동일 적용한다. |
| P18-03 | 행성별 달의 공전과 자체 자전 | 자동 | [apply](practice-cookbook/P18.md#fn-apply) · [onUpdate](practice-cookbook/P18.md#fn-onupdate) | 행성 중심 프레임 아래에서 달 위치와 회전을 계산한다. |
| P18-04 | 보이는 궤도와 실제 천체 경로 일치 | 시작·궤도 변환 | [initScene](practice-cookbook/P18.md#fn-initscene) · [apply](practice-cookbook/P18.md#fn-apply) | 공통 반지름과 프레임을 선과 천체가 공유한다. |
| P18-05 | 직각·원근 투영 선택 | p/P | [onKey](practice-cookbook/P18.md#fn-onkey) | 카메라 투영 상태만 바꾼다. |
| P18-06 | 솔리드·와이어 선택 | m/M | [onKey](practice-cookbook/P18.md#fn-onkey) | 같은 장면을 다른 렌더 옵션으로 그린다. |
| P18-07 | 전체 시스템 XYZ 이동 | W/A/S/D/± | [onKey](practice-cookbook/P18.md#fn-onkey) · [onUpdate](practice-cookbook/P18.md#fn-onupdate) · [apply](practice-cookbook/P18.md#fn-apply) | 루트의 위치만 바꾼다. |
| P18-08 | 모든 궤도 반지름의 부드러운 확대·축소 | 1 | [onKey](practice-cookbook/P18.md#fn-onkey) · [onUpdate](practice-cookbook/P18.md#fn-onupdate) · [apply](practice-cookbook/P18.md#fn-apply) | 현재 반지름을 목표 반지름으로 보간한다. |
| P18-09 | 중심 구의 크기 진동과 색 변화 | 2 | [onKey](practice-cookbook/P18.md#fn-onkey) · [onUpdate](practice-cookbook/P18.md#fn-onupdate) · [apply](practice-cookbook/P18.md#fn-apply) | 중심 메시만 바꾸며 행성 거리에는 전파하지 않는다. |
| P18-10 | 달의 궤도 프레임과 달의 함께 회전 | 3 | [onKey](practice-cookbook/P18.md#fn-onkey) · [onUpdate](practice-cookbook/P18.md#fn-onupdate) · [apply](practice-cookbook/P18.md#fn-apply) | 궤도 그룹 회전과 달 로컬 자전을 분리한다. |
| P18-11 | 전체 이동 중 포커스 해제 | 포커스 | [onFocus](practice-cookbook/P18.md#fn-onfocus) | 눌린 키 상태를 비운다. |

## 모드 16: 실습 19

[실행 파일](../modes/Mode16Practice19.cpp) · [공통 탱크](../modes/TankCommon.cpp) · [상세 원문 패턴](practice-cookbook/P19.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P19-01 | 바닥·탱크·상부·포신·깃대의 계층 조립 | 시작 | [initScene](practice-cookbook/P19.md#fn-initscene) · [reset](practice-cookbook/P19.md#fn-reset) · [box](practice-cookbook/P19.md#fn-box) | 관절/프레임과 표시 육면체를 분리한다. |
| P19-02 | 바닥 범위 안에서 탱크 XZ 이동 | 방향키 | [onKey](practice-cookbook/P19.md#fn-onkey) · [onUpdate](practice-cookbook/P19.md#fn-onupdate) · [apply](practice-cookbook/P19.md#fn-apply) | 부품 회전 범위를 포함하는 여유 반경으로 루트 위치를 제한한다. |
| P19-03 | 아래 몸체 높이 진동 | D | [onKey](practice-cookbook/P19.md#fn-onkey) · [onUpdate](practice-cookbook/P19.md#fn-onupdate) · [apply](practice-cookbook/P19.md#fn-apply) | 바닥에 놓이는 하단 위치와 높이를 함께 계산한다. |
| P19-04 | 중앙 몸체 Y축 회전과 자식 전파 | T | [onKey](practice-cookbook/P19.md#fn-onkey) · [onUpdate](practice-cookbook/P19.md#fn-onupdate) · [apply](practice-cookbook/P19.md#fn-apply) | 중앙 그룹 아래의 상부 부품이 같이 돈다. |
| P19-05 | 두 상부 몸체의 반원 경로 교환 | L | [onKey](practice-cookbook/P19.md#fn-onkey) · [onUpdate](practice-cookbook/P19.md#fn-onupdate) · [apply](practice-cookbook/P19.md#fn-apply) | sin/cos와 반대 부호로 서로 다른 반원을 지나 교환한다. |
| P19-06 | 양쪽 포신의 반대 방향 Y회전 | G | [onKey](practice-cookbook/P19.md#fn-onkey) · [onUpdate](practice-cookbook/P19.md#fn-onupdate) · [apply](practice-cookbook/P19.md#fn-apply) | 회전 중심과 포신 오프셋을 함께 회전시킨다. |
| P19-07 | 양쪽 깃대의 반대 방향 X진동 | P | [onKey](practice-cookbook/P19.md#fn-onkey) · [onUpdate](practice-cookbook/P19.md#fn-onupdate) · [apply](practice-cookbook/P19.md#fn-apply) | 같은 위상에 양쪽 부호만 반대로 둔다. |
| P19-08 | 카메라 위치와 target의 함께 이동 | x/X/z/Z | [onKey](practice-cookbook/P19.md#fn-onkey) · [onUpdate](practice-cookbook/P19.md#fn-onupdate) | 시선 방향을 유지하며 평행 이동한다. |
| P19-09 | 카메라 로컬 Y축 자전 | y/Y | [onKey](practice-cookbook/P19.md#fn-onkey) · [onUpdate](practice-cookbook/P19.md#fn-onupdate) | position 고정, 시선과 up을 함께 회전한다. |
| P19-10 | 화면 중심 Y축 기준 카메라 공전 | r/R | [onKey](practice-cookbook/P19.md#fn-onkey) · [onUpdate](practice-cookbook/P19.md#fn-onupdate) | 월드 원점 축에서 카메라의 position/target/up을 변환한다. |
| P19-11 | 전체 부품 색 순환 | O | [recolor](practice-cookbook/P19.md#fn-recolor) · [onKey](practice-cookbook/P19.md#fn-onkey) | 위치·계층을 바꾸지 않고 색만 갱신한다. |
| P19-12 | 탱크·카메라·애니메이션 전체 리셋 | C | [reset](practice-cookbook/P19.md#fn-reset) · [onKey](practice-cookbook/P19.md#fn-onkey) | 부품별 시간·방향·입력도 함께 초기화한다. |
| P19-13 | 포커스 이탈 때 카메라/탱크 입력 해제 | 포커스 | [onFocus](practice-cookbook/P19.md#fn-onfocus) | 애니메이션 토글과 지속 입력을 구분한다. |

## 모드 17: 실습 20

[실행 파일](../modes/Mode17Practice20.cpp) · [상세 원문 패턴](practice-cookbook/P20.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P20-01 | 바닥과 일정 높이의 ㄱ자 평면 벨트 | 시작 | [initScene](practice-cookbook/P20.md#fn-initscene) | 벨트 면과 줄무늬 표시 육면체를 분리한다. |
| P20-02 | 벨트 표면의 다수 줄무늬 배치 | 시작 | [initScene](practice-cookbook/P20.md#fn-initscene) · [placeMovingObjects](practice-cookbook/P20.md#fn-placemovingobjects) | 30개 줄무늬를 전체 거리의 등간격으로 배치한다. |
| P20-03 | 색·번호가 다른 택배 상자 다섯 개 | 시작 | [initScene](practice-cookbook/P20.md#fn-initscene) | 상자와 번호 선분을 owner 관계로 묶는다. |
| P20-04 | 벨트·상자에 공통 거리 시계 사용 | 자동 | [placeMovingObjects](practice-cookbook/P20.md#fn-placemovingobjects) · [onUpdate](practice-cookbook/P20.md#fn-onupdate) | 거리=속도×같은 누적 시간+개별 간격이다. |
| P20-05 | 직선→90도 원호→직선에서 코너 방향 회전 | 코너 | [placeMovingObjects](practice-cookbook/P20.md#fn-placemovingobjects) | 위치와 접선 yaw를 같은 거리에서 샘플링한다. |
| P20-06 | 끝에서 시작점으로 순환 | 벨트 끝 | [placeMovingObjects](practice-cookbook/P20.md#fn-placemovingobjects) | 전체 경로 길이로 거리를 순환한다. 끝점과 시작점은 별도 위치다. |
| P20-07 | 이동 정지·재개 | B | [onKey](practice-cookbook/P20.md#fn-onkey) · [onUpdate](practice-cookbook/P20.md#fn-onupdate) | 시계의 playing만 바꿔 객체 간 간격을 보존한다. |
| P20-08 | 쿼터뷰 초기 시점과 카메라 줌 | ± | [resetState](practice-cookbook/P20.md#fn-resetstate) · [zoom](practice-cookbook/P20.md#fn-zoom) · [onKey](practice-cookbook/P20.md#fn-onkey) | 원근 거리와 직각 범위를 각각 조절한다. |
| P20-09 | 원근·직각 투영 선택 | p/P | [onKey](practice-cookbook/P20.md#fn-onkey) | 대문자 처리와 카메라 상태를 분리한다. |
| P20-10 | 중심 Y축 카메라 공전 | 좌우 방향키 | [onKey](practice-cookbook/P20.md#fn-onkey) · [onUpdate](practice-cookbook/P20.md#fn-onupdate) · [onFocus](practice-cookbook/P20.md#fn-onfocus) | 키 유지 중 dt로 회전하고 포커스 이탈에서 입력을 지운다. |
| P20-11 | 벨트 시간·카메라 리셋 | R | [resetState](practice-cookbook/P20.md#fn-resetstate) · [onKey](practice-cookbook/P20.md#fn-onkey) | 정지 상태와 초기 시점으로 돌아간다. |

## 모드 18: 실습 21

[실행 파일](../modes/Mode18Practice21.cpp) · [상세 원문 패턴](practice-cookbook/P21.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P21-01 | 안쪽이 보이는 면별 색 직육면체 방 | 시작 | [initScene](practice-cookbook/P21.md#fn-initscene) | front-open 방과 Front 컬링을 사용한다. |
| P21-02 | 벽 충돌 때 방향을 바꾸는 공 | 자동 | [addBall](practice-cookbook/P21.md#fn-addball) · [onUpdate](practice-cookbook/P21.md#fn-onupdate) · [syncScene](practice-cookbook/P21.md#fn-syncscene) | RoomBody 상태를 계산한 뒤 구 메시 위치에 반영한다. |
| P21-03 | 공을 추가하고 최대 다섯 개로 제한 | B | [addBall](practice-cookbook/P21.md#fn-addball) · [onKey](practice-cookbook/P21.md#fn-onkey) | 추가 개수와 이미 생성된 객체 상태를 따로 관리한다. |
| P21-04 | Z축 줄의 서로 다른 크기 상자 세 개 | 시작 | [initScene](practice-cookbook/P21.md#fn-initscene) | 공과 상자의 elastic 상태를 다르게 둔다. |
| P21-05 | 드래그로 방 Z축 회전 | 마우스 좌/우 | [onMouse](practice-cookbook/P21.md#fn-onmouse) · [onCursor](practice-cookbook/P21.md#fn-oncursor) · [roomWorld](practice-cookbook/P21.md#fn-roomworld) | 커서의 차이만 회전량에 더한다. |
| P21-06 | 기울어진 방 바닥을 따르는 상자 이동 | 방 회전 | [onUpdate](practice-cookbook/P21.md#fn-onupdate) · [syncScene](practice-cookbook/P21.md#fn-syncscene) | 방 로컬 중력을 계산하고 월드 표시 위치로 옮긴다. |
| P21-07 | 방 자체 Y축 자전 | V | [onKey](practice-cookbook/P21.md#fn-onkey) · [roomWorld](practice-cookbook/P21.md#fn-roomworld) · [onUpdate](practice-cookbook/P21.md#fn-onupdate) | 드래그 회전과 독립된 시간 상태를 둔다. |
| P21-08 | 바닥을 열고 객체를 월드 낙하로 전환 | F | [onKey](practice-cookbook/P21.md#fn-onkey) · [onUpdate](practice-cookbook/P21.md#fn-onupdate) · [releaseBody](practice-cookbook/P21.md#fn-releasebody) | 이탈 시 월드 위치·속도·상자 회전을 보관한다. |
| P21-09 | 모든 객체를 즉시 이탈시켜 낙하 | E | [onKey](practice-cookbook/P21.md#fn-onkey) · [releaseBody](practice-cookbook/P21.md#fn-releasebody) | 바닥 개방 진행과 독립된 강제 이탈이다. |
| P21-10 | 회전 방의 각속도를 이탈 속도에 반영 | 이탈 | [releaseBody](practice-cookbook/P21.md#fn-releasebody) · [onUpdate](practice-cookbook/P21.md#fn-onupdate) | 드래그·자전 각속도의 기여를 함께 전달한다. |
| P21-11 | 카메라 Z이동과 로컬 Y자전 | z/Z/y/Y | [onKey](practice-cookbook/P21.md#fn-onkey) | 방 회전 상태와 카메라를 별도로 변경한다. |
| P21-12 | 포커스 이탈에서 드래그·입력 멈춤 | 포커스 | [onFocus](practice-cookbook/P21.md#fn-onfocus) · [onUpdate](practice-cookbook/P21.md#fn-onupdate) | 직전 커서와 pending 회전량을 정리한다. |

## 모드 19: 실습 22

[실행 파일](../modes/Mode19Practice22.cpp) · [공통 탱크](../modes/TankCommon.cpp) · [상세 원문 패턴](practice-cookbook/P22.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P22-01 | 실습 19의 탱크와 조작 재사용 | 방향키·D/T/L/G/P | [reset](practice-cookbook/P22.md#fn-reset) · [apply](practice-cookbook/P22.md#fn-apply) · [onKey](practice-cookbook/P22.md#fn-onkey) · [onUpdate](practice-cookbook/P22.md#fn-onupdate) | 같은 Scene의 동일한 부품 ID를 모든 뷰에서 사용한다. |
| P22-02 | 원근 시점의 장면 | 프레임 | [onRender](practice-cookbook/P22.md#fn-onrender) | 화면 크기에서 perspective pass를 구성한다. |
| P22-03 | XZ 평면도의 직각 투영 | 프레임 | [onRender](practice-cookbook/P22.md#fn-onrender) | 위에서 아래로 보며 up 방향을 -Z로 둔다. |
| P22-04 | XY 정면도의 직각 투영 | 프레임 | [onRender](practice-cookbook/P22.md#fn-onrender) | 같은 모델을 +Z 시점에서 본다. |
| P22-05 | 세 뷰포트의 같은 이동·같은 시간 | 이동·애니메이션 | [onUpdate](practice-cookbook/P22.md#fn-onupdate) · [onRender](practice-cookbook/P22.md#fn-onrender) | 갱신은 한 번, renderViews는 여러 번 그리기만 한다. |
| P22-06 | 카메라·색·리셋·포커스 상태 재사용 | 실습 19 조작 | [onKey](practice-cookbook/P22.md#fn-onkey) · [recolor](practice-cookbook/P22.md#fn-recolor) · [reset](practice-cookbook/P22.md#fn-reset) · [onFocus](practice-cookbook/P22.md#fn-onfocus) | 실습 19와 같은 요구를 함수별로 함께 추적한다. |

## 모드 20: 실습 23

[실행 파일](../modes/Mode20Practice23.cpp) · [상세 원문 패턴](practice-cookbook/P23.md)

| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |
|---|---|---|---|---|
| P23-01 | 안쪽이 보이는 무대와 면별 색 | 시작 | [initScene](practice-cookbook/P23.md#fn-initscene) | 앞벽도 유지하고 컬링으로 시점에 따라 내부를 보이게 한다. |
| P23-02 | 머리·코·몸통·다른 색 팔다리 로봇 | 시작 | [initScene](practice-cookbook/P23.md#fn-initscene) | 위치/크기는 GridLayout 비율로 조립한다. |
| P23-03 | 겹치지 않는 랜덤 장애물 세 개 | 시작·I | [initScene](practice-cookbook/P23.md#fn-initscene) | 난수 엔진을 유지하고 제한 횟수 안에서 배치한다. |
| P23-04 | 방향을 지정하고 계속 걷기 | W/A/S/D | [onKey](practice-cookbook/P23.md#fn-onkey) · [onUpdate](practice-cookbook/P23.md#fn-onupdate) | 키가 진행 방향을 바꾸며 보행 상태를 켠다. |
| P23-05 | 무대 경계에서 방향 반전 | 경계 | [onUpdate](practice-cookbook/P23.md#fn-onupdate) | CharacterBody를 stepCharacter로 진행한다. |
| P23-06 | 장애물 충돌에서 이동 거부·방향 반전 | 장애물 | [onUpdate](practice-cookbook/P23.md#fn-onupdate) | 충돌 결과를 적용한 최종 위치로 리그를 갱신한다. |
| P23-07 | 속도와 팔다리 진폭의 연동 | ± | [onKey](practice-cookbook/P23.md#fn-onkey) · [onUpdate](practice-cookbook/P23.md#fn-onupdate) | 요청 속도와 실제 이동 거리로 보행 진폭·위상을 정한다. |
| P23-08 | 보행 정지·재개 | Space | [onKey](practice-cookbook/P23.md#fn-onkey) · [onUpdate](practice-cookbook/P23.md#fn-onupdate) | 시간과 수직 운동을 모두 멈추는 대신 수평 속도만 0으로 계산한다. |
| P23-09 | 제자리·이동 점프 및 장애물 위 착지 | J | [onKey](practice-cookbook/P23.md#fn-onkey) · [onUpdate](practice-cookbook/P23.md#fn-onupdate) | grounded에서 점프하고 지지면 높이를 계산한다. |
| P23-10 | 카메라 방향 벽의 균등 축소·개방 | O | [onKey](practice-cookbook/P23.md#fn-onkey) · [onUpdate](practice-cookbook/P23.md#fn-onupdate) | 카메라로 한 벽을 선택하고 중심 기준 균등 축소한다. |
| P23-11 | 카메라 이동 후에도 같은 벽 복원 | O 반복 | [onKey](practice-cookbook/P23.md#fn-onkey) · [onUpdate](practice-cookbook/P23.md#fn-onupdate) | activeWall은 복원 완료 때까지 유지한다. |
| P23-12 | 카메라 XYZ 이동·target 중심 Y공전 | x/X/z/Z/y/Y | [onKey](practice-cookbook/P23.md#fn-onkey) | 로봇의 이동 상태와 독립적으로 조절한다. |
| P23-13 | 무대·로봇·장애물·입력 리셋 | I | [initScene](practice-cookbook/P23.md#fn-initscene) · [onKey](practice-cookbook/P23.md#fn-onkey) | ID를 다시 받고 기존 난수 엔진에서 새 장애물을 생성한다. |

## 공통 10개와 이전 3D 10개

C01~C10은 main/Console의 콜백·전환, PracticeMode, PracticeCommon, 각 모드의 원래 상태에 연결된다. 세부 목록은 [COMMON](practice-cookbook/COMMON.md)에 보존한다.

E00-01~04는 모드21의 WASD·방향 정규화·구/AABB 충돌·서브스텝·포커스 해제에, E06-01~06은 모드22의 정점·UV·인덱스·중복·일직선 판정과 모델 등록에 연결된다. [이전 부록](practice-cookbook/EARLIER_3D.md)을 참조한다. ADJACENT_FACES의 공유 모서리·교환은 모드12의 1/2와 FaceCommon에서 제공한다.

## 확인 경계

구조 그래프는 `graphify-out/standard-input/`에 별도로 생성한다. 이전 전역 그래프를 최신으로 꾸미거나 미리 없던 워밍업 파일을 복구하지 않는다. 동적 디스패치와 콜백 호출 순서는 소스 흐름 문서로 보완한다. 사용자 변경인 Framework.props·API 문서·도구와 기존 부재 파일은 별도 상태로 남겼다.

[사용자 확인](../USER_CHECKS.md)에서 빌드·입력·렌더링을 수행한다. 8번 방 배율과 물리 상자 피벗 제한, 교육용 AABB, 선·투명 픽셀 피킹 차이를 포함한다.
