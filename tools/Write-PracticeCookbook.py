"""Author source-backed, standard-local 3D practice code blocks. No builds/tests/app execution."""
from pathlib import Path
import hashlib
import json
import re
import subprocess
from tree_sitter import Language, Parser
import tree_sitter_cpp

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "standard/docs/practice-cookbook"
PARSER = Parser(Language(tree_sitter_cpp.language()))

# ID | requirement | input | implementation methods | behavior / important boundary
DATA = {
13: """
01|육면체·사각뿔의 초기 배치와 기울기|시작|initScene|초기 모델 회전과 표시용 카메라를 함께 확인한다.
02|움직이지 않는 XYZ 좌표축|시작|initScene|좌표축 선을 객체 owner 없이 생성한다.
03|육면체의 지정한 한 면만 표시|1~6|onKey,selectFaces|이전 표시를 먼저 지우고 선택 면만 켠다.
04|사각뿔의 지정한 옆면만 표시|7~0|onKey,selectFaces|삼각 옆면 인덱스 0~3을 선택한다.
05|서로 다른 육면체 두 면 무작위 선택|C|onKey,selectFaces|5개 후보에서 뽑은 두 번째 번호를 첫 번호 이후에서 보정한다.
06|사각뿔 바닥과 임의 옆면 표시|T|onKey,selectFaces|바닥 4와 옆면 0~3 하나를 함께 켠다.
07|면마다 다른 색과 깊이 가림|시작|initScene|기본 모델의 면 색과 RenderOptions.depthTest를 사용한다.
""",
14: """
01|같은 꼭짓점에 같은 색을 배정|시작|initScene|공통 assignVertexColors가 좌표 부호 3비트를 색 인덱스로 사용한다.
02|도형을 바꿔도 위치·회전 보존|C/P|selectShape,applyTransform|도형 선택 상태와 조작 변환을 분리한다.
03|은면 제거 켜기·끄기|H|onKey|렌더 옵션만 바꾼다.
04|와이어·솔리드 선택|w/W|onKey|Shift와 Caps Lock의 조합으로 대문자를 해석한다.
05|현재 로컬 X축 양·음 자전|x/X|onKey,onUpdate,applyTransform|회전 기저의 오른쪽에 증분 회전을 곱한다.
06|현재 로컬 Y축 양·음 자전|y/Y|onKey,onUpdate,applyTransform|초기 표시 각도를 별도 카메라에 둔다.
07|누르는 동안 XY 이동|방향키|onKey,onUpdate|키 해제에서 속도 입력을 지우며 dt로 이동한다.
08|객체가 움직여도 좌표축 고정|이동·회전|initScene,applyTransform|축 선을 모델 자식으로 넣지 않는다.
09|조작 상태·속도 초기화|S|resetMotion,onKey|이동·회전을 지우고 모든 애니메이션을 멈춘다.
10|포커스 이탈 때 지속 입력 해제|포커스|onFocus|키 해제 이벤트가 없어도 이동을 정지한다.
""",
15: """
01|모델 전체 Y 자전과 면 동작 중첩|Y|onKey,onUpdate,applyObjectTransform|본체 SRT와 면별 행렬을 따로 저장한다.
02|윗면 중심 Z축 자전|T|beginMotion,applyFaceMotion,onUpdate|Top case: 윗면 중심을 고정한다.
03|앞면 상단 모서리 힌지 열기·닫기|F|beginMotion,applyFaceMotion,onUpdate|Front case: 진행률 0~1을 -90도로 바꾼다.
04|왼쪽·오른쪽 면의 제자리 X축 회전|S|beginMotion,applyFaceMotion,onUpdate|Side case: 두 면의 중심을 각각 사용한다.
05|뒷면 중심 축소·소멸·복원|B|beginMotion,applyFaceMotion,onUpdate|Back case: 0 크기에 도달한 면을 숨긴다.
06|서로 이웃한 옆면 쌍 선택 유지|1/2 첫 입력|beginMotion|순환 쌍 0-3, 3-1, 1-2, 2-0과 공유 모서리를 함께 저장한다.
07|이웃한 두 면이 공유 모서리를 중심으로 360도 회전|1|beginMotion,applyFaceMotion,onUpdate|Hinge case: 선택한 두 면에 같은 모서리 회전을 적용한다.
08|이웃한 두 면의 반대 방향 90도 공전 교환|2|beginMotion,applyFaceMotion,onUpdate|Swap case: 0→90→0도의 삼각 위상으로 교환·복귀한다.
09|네 옆면의 모델 중심 Y축 공전|4|beginMotion,applyFaceMotion,onUpdate|OrbitY case: 면 0,1,2,3을 함께 돌린다.
10|위·아래·앞·뒤 면의 모델 중심 X축 공전|5|beginMotion,applyFaceMotion,onUpdate|OrbitX case: 면 4,5,0,1을 함께 돌린다.
11|동작별 정지·재개와 방향 반전 구분|반복 키|beginMotion,onUpdate|F/B는 방향 반전, 나머지는 각 동작의 running만 토글한다.
12|여러 면 동작의 고정 순서 합성|동시 입력|applyFaceMotion|매 프레임 항등 행렬에서 시작해 enum 순서로 왼쪽 곱한다.
13|도형 전환과 전체 리셋|P/R|selectShape,resetFaces,resetAll|면 상태 초기화와 본체 전체 초기화를 구분한다.
""",
16: """
01|실습 15의 육면체 동작 유지|T/F/S/B/1/2/4/5|beginMotion,applyFaceMotion,onUpdate|육면체 부분은 실습 15와 같은 상태·합성 패턴이다.
02|사각뿔 네 면을 아래에서 만나도록 열기|6|beginPyramid,applyPyramid,updatePyramid|All: 실제 기하에서 회전각을 구한다. PDF의 목표 자세와 행렬 각도를 구분한다.
03|사각뿔 네 면을 순서대로 열고 닫기|7|beginPyramid,applyPyramid,updatePyramid|8초 주기, 각 면의 2초 구간에서 0→평면→0을 계산한다.
04|마주보는 두 사각뿔 옆면의 꼭짓점 기준 교환|8|beginPyramid,applyPyramid,updatePyramid|Swap: 꼭대기를 지나는 Y축에서 두 면에 반대 부호를 준다.
05|사각뿔 꼭대기 고정 바깥쪽 펼치기|9|beginPyramid,applyPyramid,updatePyramid|Apex: 면별 축으로 -45도까지 연다.
06|사각뿔 모드 전환·정지·역방향 복귀|6~9 재입력|beginPyramid,selectShape|새 모드는 닫힌 자세에서 시작하고 완료한 교환은 반대 방향으로 복귀한다.
07|사각뿔 바닥의 고정|모든 사각뿔 동작|applyPyramid|면 4의 추가 행렬은 항등으로 둔다.
""",
17: """
01|왼쪽·오른쪽·두 객체의 선택|1/2/3|onKey|선택 상태로 개별 조작의 적용 대상을 정한다.
02|공전 그룹과 표시 메시 분리|시작|reset,apply,orbitRotation,worldPosition|공전 프레임 아래에 몸체의 위치·자전·크기를 둔다.
03|선택 객체의 로컬 X/Y 자전|x/X/y/Y|onKey,onUpdate,apply|자전 기저를 보관하고 수치 드리프트를 다시 직교화한다.
04|월드 원점 Y축 공전|r/R|onKey,orbitRotation,apply|본체 자전과 공전 그룹의 회전 상태를 분리한다.
05|제자리 확대·축소|a/A|onKey,apply|표시 크기만 바꾸고 위치는 유지한다.
06|원점 기준 위치와 크기 동시 확대·축소|b/B|onKey,apply|위치와 배율을 같은 비율로 바꾼다.
07|공전 중 화면 XY 방향 이동|d/D/e/E|onKey,onUpdate|원하는 월드 이동을 공전 회전의 전치로 부모 공간에 옮긴다.
08|두 객체의 원점을 통과하는 직선 교환|T|begin,onUpdate|시작 월드 위치를 저장하고 2초 동안 서로의 위치로 보간한다.
09|위·아래로 우회하는 교환|U|begin,onUpdate|직선 보간에 부호가 반대인 sin(pi*t) 높이를 더한다.
10|반대 위상 확대·축소와 자전·공전|V|begin,onUpdate|시작 배율과 위상을 저장하며 모드 정지는 합성 동작만 멈춘다.
11|진행 중 모드 전환과 같은 모드 정지·재개|T/U/V 반복|begin,onUpdate|전환 시 현재 월드 위치를 새 시작점으로 잡는다.
12|두 객체의 형상 순환과 SRT 유지|C|model,onKey,apply|원뿔→구→육면체 모델만 교체한다.
13|초기화·포커스 이탈 입력 정리|S/포커스|reset,onFocus|선택·모션·이동 입력을 초기 상태로 되돌린다.
""",
18: """
01|중심·행성·달의 계층 구성|시작|initScene|표시 메시 크기를 궤도 프레임 배율과 분리한다.
02|세 행성의 서로 다른 궤도 기울기·속도|자동|initScene,apply,onUpdate|XZ와 Z축 ±45도 기울기를 위치와 선에 동일 적용한다.
03|행성별 달의 공전과 자체 자전|자동|apply,onUpdate|행성 중심 프레임 아래에서 달 위치와 회전을 계산한다.
04|보이는 궤도와 실제 천체 경로 일치|시작·궤도 변환|initScene,apply|공통 반지름과 프레임을 선과 천체가 공유한다.
05|직각·원근 투영 선택|p/P|onKey|카메라 투영 상태만 바꾼다.
06|솔리드·와이어 선택|m/M|onKey|같은 장면을 다른 렌더 옵션으로 그린다.
07|전체 시스템 XYZ 이동|W/A/S/D/±|onKey,onUpdate,apply|루트의 위치만 바꾼다.
08|모든 궤도 반지름의 부드러운 확대·축소|1|onKey,onUpdate,apply|현재 반지름을 목표 반지름으로 보간한다.
09|중심 구의 크기 진동과 색 변화|2|onKey,onUpdate,apply|중심 메시만 바꾸며 행성 거리에는 전파하지 않는다.
10|달의 궤도 프레임과 달의 함께 회전|3|onKey,onUpdate,apply|궤도 그룹 회전과 달 로컬 자전을 분리한다.
11|전체 이동 중 포커스 해제|포커스|onFocus|눌린 키 상태를 비운다.
""",
19: """
01|바닥·탱크·상부·포신·깃대의 계층 조립|시작|initScene,reset,box|관절/프레임과 표시 육면체를 분리한다.
02|바닥 범위 안에서 탱크 XZ 이동|방향키|onKey,onUpdate,apply|부품 회전 범위를 포함하는 여유 반경으로 루트 위치를 제한한다.
03|아래 몸체 높이 진동|D|onKey,onUpdate,apply|바닥에 놓이는 하단 위치와 높이를 함께 계산한다.
04|중앙 몸체 Y축 회전과 자식 전파|T|onKey,onUpdate,apply|중앙 그룹 아래의 상부 부품이 같이 돈다.
05|두 상부 몸체의 반원 경로 교환|L|onKey,onUpdate,apply|sin/cos와 반대 부호로 서로 다른 반원을 지나 교환한다.
06|양쪽 포신의 반대 방향 Y회전|G|onKey,onUpdate,apply|회전 중심과 포신 오프셋을 함께 회전시킨다.
07|양쪽 깃대의 반대 방향 X진동|P|onKey,onUpdate,apply|같은 위상에 양쪽 부호만 반대로 둔다.
08|카메라 위치와 target의 함께 이동|x/X/z/Z|onKey,onUpdate|시선 방향을 유지하며 평행 이동한다.
09|카메라 로컬 Y축 자전|y/Y|onKey,onUpdate|position 고정, 시선과 up을 함께 회전한다.
10|화면 중심 Y축 기준 카메라 공전|r/R|onKey,onUpdate|월드 원점 축에서 카메라의 position/target/up을 변환한다.
11|전체 부품 색 순환|O|recolor,onKey|위치·계층을 바꾸지 않고 색만 갱신한다.
12|탱크·카메라·애니메이션 전체 리셋|C|reset,onKey|부품별 시간·방향·입력도 함께 초기화한다.
13|포커스 이탈 때 카메라/탱크 입력 해제|포커스|onFocus|애니메이션 토글과 지속 입력을 구분한다.
""",
20: """
01|바닥과 일정 높이의 ㄱ자 평면 벨트|시작|initScene|벨트 면과 줄무늬 표시 육면체를 분리한다.
02|벨트 표면의 다수 줄무늬 배치|시작|initScene,placeMovingObjects|30개 줄무늬를 전체 거리의 등간격으로 배치한다.
03|색·번호가 다른 택배 상자 다섯 개|시작|initScene|상자와 번호 선분을 owner 관계로 묶는다.
04|벨트·상자에 공통 거리 시계 사용|자동|placeMovingObjects,onUpdate|거리=속도×같은 누적 시간+개별 간격이다.
05|직선→90도 원호→직선에서 코너 방향 회전|코너|placeMovingObjects|위치와 접선 yaw를 같은 거리에서 샘플링한다.
06|끝에서 시작점으로 순환|벨트 끝|placeMovingObjects|전체 경로 길이로 거리를 순환한다. 끝점과 시작점은 별도 위치다.
07|이동 정지·재개|B|onKey,onUpdate|시계의 playing만 바꿔 객체 간 간격을 보존한다.
08|쿼터뷰 초기 시점과 카메라 줌|±|resetState,zoom,onKey|원근 거리와 직각 범위를 각각 조절한다.
09|원근·직각 투영 선택|p/P|onKey|대문자 처리와 카메라 상태를 분리한다.
10|중심 Y축 카메라 공전|좌우 방향키|onKey,onUpdate,onFocus|키 유지 중 dt로 회전하고 포커스 이탈에서 입력을 지운다.
11|벨트 시간·카메라 리셋|R|resetState,onKey|정지 상태와 초기 시점으로 돌아간다.
""",
21: """
01|안쪽이 보이는 면별 색 직육면체 방|시작|initScene|front-open 방과 Front 컬링을 사용한다.
02|벽 충돌 때 방향을 바꾸는 공|자동|addBall,onUpdate,syncScene|RoomBody 상태를 계산한 뒤 구 메시 위치에 반영한다.
03|공을 추가하고 최대 다섯 개로 제한|B|addBall,onKey|추가 개수와 이미 생성된 객체 상태를 따로 관리한다.
04|Z축 줄의 서로 다른 크기 상자 세 개|시작|initScene|공과 상자의 elastic 상태를 다르게 둔다.
05|드래그로 방 Z축 회전|마우스 좌/우|onMouse,onCursor,roomWorld|커서의 차이만 회전량에 더한다.
06|기울어진 방 바닥을 따르는 상자 이동|방 회전|onUpdate,syncScene|방 로컬 중력을 계산하고 월드 표시 위치로 옮긴다.
07|방 자체 Y축 자전|V|onKey,roomWorld,onUpdate|드래그 회전과 독립된 시간 상태를 둔다.
08|바닥을 열고 객체를 월드 낙하로 전환|F|onKey,onUpdate,releaseBody|이탈 시 월드 위치·속도·상자 회전을 보관한다.
09|모든 객체를 즉시 이탈시켜 낙하|E|onKey,releaseBody|바닥 개방 진행과 독립된 강제 이탈이다.
10|회전 방의 각속도를 이탈 속도에 반영|이탈|releaseBody,onUpdate|드래그·자전 각속도의 기여를 함께 전달한다.
11|카메라 Z이동과 로컬 Y자전|z/Z/y/Y|onKey|방 회전 상태와 카메라를 별도로 변경한다.
12|포커스 이탈에서 드래그·입력 멈춤|포커스|onFocus,onUpdate|직전 커서와 pending 회전량을 정리한다.
""",
22: """
01|실습 19의 탱크와 조작 재사용|방향키·D/T/L/G/P|reset,apply,onKey,onUpdate|같은 Scene의 동일한 부품 ID를 모든 뷰에서 사용한다.
02|원근 시점의 장면|프레임|onRender|화면 크기에서 perspective pass를 구성한다.
03|XZ 평면도의 직각 투영|프레임|onRender|위에서 아래로 보며 up 방향을 -Z로 둔다.
04|XY 정면도의 직각 투영|프레임|onRender|같은 모델을 +Z 시점에서 본다.
05|세 뷰포트의 같은 이동·같은 시간|이동·애니메이션|onUpdate,onRender|갱신은 한 번, renderViews는 여러 번 그리기만 한다.
06|카메라·색·리셋·포커스 상태 재사용|실습 19 조작|onKey,recolor,reset,onFocus|실습 19와 같은 요구를 함수별로 함께 추적한다.
""",
23: """
01|안쪽이 보이는 무대와 면별 색|시작|initScene|앞벽도 유지하고 컬링으로 시점에 따라 내부를 보이게 한다.
02|머리·코·몸통·다른 색 팔다리 로봇|시작|initScene|위치/크기는 GridLayout 비율로 조립한다.
03|겹치지 않는 랜덤 장애물 세 개|시작·I|initScene|난수 엔진을 유지하고 제한 횟수 안에서 배치한다.
04|방향을 지정하고 계속 걷기|W/A/S/D|onKey,onUpdate|키가 진행 방향을 바꾸며 보행 상태를 켠다.
05|무대 경계에서 방향 반전|경계|onUpdate|CharacterBody를 stepCharacter로 진행한다.
06|장애물 충돌에서 이동 거부·방향 반전|장애물|onUpdate|충돌 결과를 적용한 최종 위치로 리그를 갱신한다.
07|속도와 팔다리 진폭의 연동|±|onKey,onUpdate|요청 속도와 실제 이동 거리로 보행 진폭·위상을 정한다.
08|보행 정지·재개|Space|onKey,onUpdate|시간과 수직 운동을 모두 멈추는 대신 수평 속도만 0으로 계산한다.
09|제자리·이동 점프 및 장애물 위 착지|J|onKey,onUpdate|grounded에서 점프하고 지지면 높이를 계산한다.
10|카메라 방향 벽의 균등 축소·개방|O|onKey,onUpdate|카메라로 한 벽을 선택하고 중심 기준 균등 축소한다.
11|카메라 이동 후에도 같은 벽 복원|O 반복|onKey,onUpdate|activeWall은 복원 완료 때까지 유지한다.
12|카메라 XYZ 이동·target 중심 Y공전|x/X/z/Z/y/Y|onKey|로봇의 이동 상태와 독립적으로 조절한다.
13|무대·로봇·장애물·입력 리셋|I|initScene,onKey|ID를 다시 받고 기존 난수 엔진에서 새 장애물을 생성한다.
"""}
PDF = {
13:("지엘3 좌표계 변환 (실습 131415).pdf",2),14:("지엘3 좌표계 변환 (실습 131415).pdf",3),15:("지엘3 좌표계 변환 (실습 131415).pdf",4),
16:("지엘3 좌표계 변환 (실습 1617).pdf",2),17:("지엘3 좌표계 변환 (실습 1617).pdf",3),18:("지엘3 좌표계 변환 (실습 1819).pdf",2),19:("지엘3 좌표계 변환 (실습 1819).pdf",3),
20:("지엘3 좌표계 변환 2 (실습 20).pdf",1),21:("지엘3 좌표계 변환 (실습 2122).pdf",2),22:("지엘3 좌표계 변환 (실습 2122).pdf",3),23:("지엘3 좌표계 변환 (실습 23).pdf",2)}
TITLES={13:"면 선택과 표시",14:"색·자전·이동",15:"육면체 면의 개별·복합 동작",16:"사각뿔 펼침과 교환",17:"두 객체의 선택·변환·자리 교환",18:"행성·달·궤도 계층",19:"탱크 부품과 카메라",20:"컨베이어·줄무늬·상자",21:"방·공·상자·낙하",22:"탱크의 세 뷰포트",23:"로봇·장애물·점프·벽"}
SHARED={"assignVertexColors":15,"rotateAround":15,"scaleAround":15,"turn":17,"require":20,"requireEntity":22}

COMMON_EXTRA='\n## 공통 요구사항 묶음\n\n| ID | 공통 패턴 | 가져올 위치 |\n|---|---|---|\n| C01 | Q/Esc 종료와 정상 자원 정리 | 각 문서의 onKey와 main |\n| C02 | 깊이·와이어·투영 옵션의 독립 설정 | 14/18/20의 onKey, 22의 onRender |\n| C03 | 토글/반전/키 유지/키 해제의 구분 | 15의 beginMotion, 17/19의 onKey |\n| C04 | Shift·Caps Lock 조합 | 14/17~23의 onKey |\n| C05 | 포커스 이탈의 입력 초기화 | 14/17~22의 onFocus |\n| C06 | 기준 상태와 현재 시간의 분리 | 15의 applyFaceMotion, 17의 begin/onUpdate |\n| C07 | SRT·계층·표시 크기의 분리 | 17~19의 apply, 22의 reset/apply |\n| C08 | 여러 뷰에서 한 번만 갱신 | 22의 onUpdate/onRender |\n| C09 | 리셋 후 ID 재발급과 초기 상태 | 각 문서의 initScene/reset |\n| C10 | 결과 확인과 실패 전파 | require/requireEntity 및 각 initScene |\n\n위 공통 패턴은 실습별 세부 116개와 구분한다. 같은 공통 코드가 필요하다고 여러 실습의 전체 Console을 중복 정의하지 않는다.\n'
INDEX_EXTRA='\n## 외부 의존성이 없는 구성\n\n코드 블록이 참조하는 사용자 파일은 같은 standard 폴더에 작성할 Console.h/PracticeCommon.h뿐이다. 내용은 모두 이 묶음에 있다.\nframework는 standard에 동봉된 폴더다. 다른 실습 파일, 사용자 홈 경로, 외부 모델·텍스처 파일은 코드에서 읽지 않는다.\n근거용 원본 링크와 문서 생성 도구는 실행 코드 의존성과 구분한다.\n\n실습별 세부 116개, 공통 묶음 10개, 이전 3D 부록 10개를 구분해 제공한다. 실제 빌드·화면 확인은 사용자 담당이다.\n'

def write(path,text):
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_text(text,encoding="utf-8-sig",newline="\n")

def functions(source):
    raw=source.encode("utf-8");tree=PARSER.parse(raw);out=[]
    def visit(node):
        if node.type=="function_definition":
            body=node.child_by_field_name("body")
            signature=raw[node.start_byte:body.start_byte].decode("utf-8")
            match=re.search(r"([\w:]+)\s*\(",signature)
            if match:out.append(dict(name=match[1],start=node.start_byte,end=node.end_byte,line=node.start_point.row+1,text=raw[node.start_byte:node.end_byte].decode("utf-8")))
            return
        for child in node.named_children:visit(child)
    visit(tree.root_node)
    return out

def block(code):return "```cpp\n"+code.rstrip()+"\n```\n"
def anchor(name):return "fn-"+name.lower().replace("::","-")
def rows(number):
    return [dict(id=f"P{number}-{parts[0]}",title=parts[1],input=parts[2],methods=parts[3].split(","),behavior=parts[4])
            for line in DATA[number].strip().splitlines() if (parts:=line.split("|"))]

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    sources={};inventory=[];hashes={}
    for number in range(13,24):
        folder=ROOT/f"openGL_3dAssign_{number}"
        sources[number]={name:(folder/name).read_text(encoding="utf-8-sig") for name in ["Console.h","Console.cpp","main.cpp"]}
        for name in sources[number]:hashes[f"openGL_3dAssign_{number}/{name}"]=hashlib.sha256((folder/name).read_bytes()).hexdigest()
    helpers={}
    for name,number in SHARED.items():
        helpers[name]=next(f["text"] for f in functions(sources[number]["Console.cpp"]) if f["name"]==name)
    common_header='#pragma once\n#include "framework/Framework.h"\n#include <algorithm>\n#include <array>\n#include <cmath>\n#include <iostream>\n#include <random>\n#include <stdexcept>\n#include <string>\n#include <vector>\n\nnamespace practice_common {\n'
    common_header+="\n\n".join("inline "+code for code in helpers.values())+"\n} // namespace practice_common\n"
    common="# 공통 구현: standard 안에서만 사용하는 보조 코드\n\n이 문서의 첫 블록 전문을 `standard/PracticeCommon.h`로 저장한다. C++17과 standard에 동봉된 framework만 사용한다. 다른 실습 폴더, 개인 경로, 외부 SDK, 추가 라이브러리를 참조하지 않는다.\n\n## C00 공통 헤더 전문\n\n"+block(common_header)
    common+="\n## 공통 패턴을 가져오는 위치\n\n| 공통 처리 | 실제 코드 | 사용하는 곳 |\n|---|---|---|\n| 꼭짓점 색 일관성 | assignVertexColors | 14·15의 초기 모델 구성 |\n| 점/선 중심 회전 | rotateAround | 15·16의 면·꼭짓점 회전 |\n| 중심 고정 축소 | scaleAround | 15·16의 뒷면 축소 |\n| 도 단위 회전 기저 | turn | 17·18·19 |\n| 실패 전파 | require / requireEntity | 20·22·23 |\n\n"
    common+="C++ 블록은 하나만 골라도 필요한 클래스 상태와 다른 함수가 있을 수 있다. 각 실습 문서의 `Console.h`, `Console.cpp` 보조부와 필요한 함수 블록을 함께 사용한다. 한 실습을 통째로 옮기려면 해당 문서의 블록을 순서대로 복사한다. 파일 내용은 모두 이 문서 묶음에 제공하며 원본 실습 파일을 include하지 않는다.\n\n"
    common+="## 공통 실행 계약\n\n- 초기화는 ID와 기준 상태를 만들고, 입력은 상태를 바꾸며, onUpdate는 dt초로 상태를 진행한 뒤 Scene에 반영한다.\n- 토글은 GLFW_PRESS에서 처리한다. 지속 입력은 PRESS/RELEASE 상태와 포커스 해제를 처리한다.\n- Shift와 Caps Lock은 XOR로 대문자를 판단한다.\n- Scene.clear 뒤에는 모든 ID와 포인터를 다시 받는다.\n- 본체 회전과 면 행렬을 분리하고 매 프레임 기준 상태에서 계산한다.\n- 여러 뷰라도 상태·시간 갱신은 한 번이다. Application이 모션을 자동 진행하므로 추가 updateMotions를 호출하지 않는다.\n- 기존 standard 데모 Console과 새 Console을 동시에 정의하지 않는다. 교체할 파일은 아래 3개뿐이다. PracticeCommon.h는 위 전문을 추가한다.\n\n| 파일 | 넣을 코드 |\n|---|---|\n| standard/Console.h | 선택한 실습의 전체 클래스 상태·선언 블록 |\n| standard/Console.cpp | 보조부 뒤에 모든 함수 블록을 순서대로 연결 |\n| standard/main.cpp | 해당 실습의 콜백 연결 블록 |\n\n기존 내용을 보존한 뒤 사용자가 교체한다. 이 작업은 실제 standard 런타임 파일을 교체하지 않았다. 빌드·실행·입력·화면 확인은 사용자 담당이다.\n"
    write(OUT/"COMMON.md",common+COMMON_EXTRA)
    index="# 3D 실습 세부 요구사항 구현 패턴\n\n공통 framework를 쓰는 실습 13~23의 **세부 동작**을 대상으로 한다. API 목록 대신 상태·시작/재입력·프레임 계산·반영 함수를 연결한다. 번호 없는 이전 3D 예제와 워밍업의 3D 데이터 부분은 [별도 부록](EARLIER_3D.md)에 둔다. 1~12의 2D 구현은 제외한다.\n\n[공통 구현과 standard 복사 순서](COMMON.md) · [이웃 면 교환을 따로 조립하는 완결 예](ADJACENT_FACES.md) · [요구사항과 현재 소스의 차이](SOURCE_NOTES.md)\n\n새 API를 추가하지 않는다. 모든 C++ 블록은 standard 내부 파일 또는 동봉 framework만 참조한다. 원본 경로 링크는 근거이며 코드 의존성이 아니다.\n\n## 실습별 문서\n\n"
    for number in range(13,24):
        source=sources[number];methods=[f for f in functions(source["Console.cpp"]) if f["name"].startswith("Console::")]
        by_name={f["name"].split("::")[-1]:f for f in methods}
        entries=rows(number);inventory.extend(dict(practice=number,**row) for row in entries)
        filename=f"P{number}.md";index+=f"- [{number}: {TITLES[number]}]({filename}) — {len(entries)}개 세부 패턴\n"
        pdf,page=PDF[number]
        md=f"# 실습 {number}: {TITLES[number]}\n\n[전체 목차](README.md) · [공통 보조 코드](COMMON.md)\n\n"
        md+=f"근거: 과제 PDF `{pdf}` {page}쪽 이후와 현재 실습 소스. PDF를 그대로 구현했다고 단정하지 않으며 [현재 소스와 차이](SOURCE_NOTES.md)를 함께 읽는다.\n\n"
        md+="## 세부 요구사항 → 구현 블록\n\n| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |\n|---|---|---|---|---|\n"
        for row in entries:
            links=" · ".join(f"[{name}](#{anchor(name)})" for name in row["methods"])
            md+=f"| {row['id']} | {row['title']} | {row['input']} | {links} | {row['behavior']} |\n"
        header=re.sub(r'#include "framework/[^"\n]+"','#include "PracticeCommon.h"',source["Console.h"])
        header=re.sub(r'^#include <(?:glfw3\.h|glm/[^>]+)>\s*\n','',header,flags=re.M)
        md+="\n## 상태와 선언 — standard/Console.h\n\n이 블록 전문을 Console.h로 사용한다. private 멤버와 보조 함수 선언도 포함한다.\n\n"+block(header)
        raw=source["Console.cpp"].encode("utf-8");removed=[];used=[]
        for f in functions(source["Console.cpp"]):
            if "::" not in f["name"] and f["name"] in SHARED:
                removed.append((f["start"],f["end"]));used.append(f["name"])
        # Function bodies are verbatim; only duplicate local helper definitions move to COMMON.md.
        transformed=raw
        for start,end in sorted(removed,reverse=True):transformed=transformed[:start]+transformed[end:]
        cpp=transformed.decode("utf-8")
        cpp=re.sub(r'^#include <(?:glfw3\.h|glm/[^>]+)>\s*\n','',cpp,flags=re.M)
        if used:cpp=cpp.replace('#include "Console.h"','#include "Console.h"\n'+"\n".join("using practice_common::"+name+";" for name in used),1)
        final_methods=[f for f in functions(cpp) if f["name"].startswith("Console::")]
        preamble=cpp.encode("utf-8")[:final_methods[0]["start"]].decode("utf-8")
        md+="\n## Console.cpp 보조부\n\n이 블록을 Console.cpp 맨 앞에 두고 아래 함수 블록을 순서대로 붙인다. 공유 보조 함수는 위 Common 헤더에서 제공한다.\n\n"+block(preamble)
        for f in final_methods:
            name=f["name"].split("::")[-1]
            using=[row["id"] for row in entries if name in row["methods"]]
            original=by_name[name]
            md+=f'\n<a id="{anchor(name)}"></a>\n\n## {name} 구현 블록\n\n'
            md+="연결 요구사항: "+(", ".join(using) or "다른 함수의 보조 처리")+".\n\n"
            md+=f"원본 위치: [Console.cpp {original['line']}행](../../../openGL_3dAssign_{number}/Console.cpp). 본문은 현재 소스에서 가져왔으며 함수 전체를 제공한다.\n\n"+block(f["text"])
        md+="\n## standard/main.cpp 콜백 연결\n\n선택한 Console에 실제 존재하는 콜백만 등록한다. 수정키·마우스·커서·포커스·사용자 렌더 콜백을 누락하지 않는다. 이 블록 전문으로 main.cpp를 교체한다.\n\n"+block(source["main.cpp"])
        md+="\n## 사용자가 확인할 사항\n\n이 문서의 블록을 순서대로 같은 standard 폴더에 구성한 뒤 C++17/x64로 빌드한다. 위 요구사항의 키·재입력·초기화·포커스·화면 결과를 확인한다. 이 문서 제작에서는 빌드·테스트·앱 실행을 하지 않았다.\n"
        write(OUT/filename,md)
    index+="\n## 읽는 방법\n\n1. 공통 헤더 전문을 같은 standard 폴더에 준비한다.\n2. 원하는 실습의 상태·보조부·함수·main 블록을 순서대로 복사한다.\n3. 요구사항 표에서 원하는 세부 동작을 찾고 상태를 켜는 함수와 프레임 계산을 함께 읽는다.\n4. 여러 실습을 합칠 때 클래스 Console을 중복 정의하지 말고 필요한 상태·시작 함수·갱신 함수만 한 Console로 옮긴다.\n5. [차이와 사용자 확인](SOURCE_NOTES.md)을 읽고 직접 빌드·조작한다.\n\n"
    index+=f"세부 패턴 {len(inventory)}개, 실습 문서 11개다. 예: 이웃한 두 면의 선택·공유 모서리 회전·반대 공전 교환은 P15-06~08로 나눴다. 공통 기능은 COMMON.md에 한 번만 둔다.\n"
    write(OUT/"README.md",index+INDEX_EXTRA)
    for name in ["openGL_sample0/main.cpp","Computer-Graphics_WarmmingUp2/README.md","Computer-Graphics_WarmmingUp2/MeshValidator.cpp"]:
        hashes[name]=hashlib.sha256((ROOT/name).read_bytes()).hexdigest()
    for path in (ROOT/"lecture_pdf/homework").glob("*.pdf"):
        if any(term in path.name for term in ["131415","1617","1819","실습 20","2122","실습 23"]):
            hashes[path.relative_to(ROOT).as_posix()]=hashlib.sha256(path.read_bytes()).hexdigest()
    commit=subprocess.check_output(["git","rev-parse","HEAD"],cwd=ROOT,text=True).strip()
    write(OUT/"SOURCE_INVENTORY.json",json.dumps({"source_commit":commit,"source_files":hashes,"requirements":inventory,"common_patterns":["C"+str(i).zfill(2) for i in range(1,11)],"earlier_3d_patterns":["E00-01","E00-02","E00-03","E00-04","E06-01","E06-02","E06-03","E06-04","E06-05","E06-06"],"generated_by":"tools/Write-PracticeCookbook.py","build_test_app_execution":"not_run","dependencies":"C++17 standard library + framework bundled within standard + complete local helper block"},ensure_ascii=False,indent=2)+"\n")
    print(f"Cookbook: {len(inventory)} detailed requirements, 11 complete standard-local recipes, shared helpers. No build/tests executed.")

if __name__=="__main__":main()
