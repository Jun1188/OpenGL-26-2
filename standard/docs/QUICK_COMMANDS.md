# 명령어 빠른 참조

콘솔 F12/help와 같은 소스에서 만든다. 형식은 **명령어 [실행 결과] - 각 인수의 의미**다.

```text
Standard 0: 왼쪽 클릭: 물체, Ctrl+클릭: 부모 물체, Esc: 세부모드 종료. 도/초/월드 단위 사용.
add <종류> [이름] [at cursor 또는 at X Y Z] [생성 후 선택] - 종류: box/sphere/cone/pyramid/tetra/joint/room/humanoid, 이름 생략: 자동
t X Y Z [월드 원점 배치] - XYZ: 절대 좌표. 부모 상속 유지
r <축> <도> [월드 축 상대 회전] - 축: x/y/z, 도: 추가 회전량
s <값 또는 SX SY SZ> [월드 축 상대 배율] - S: 각 축 배율. 비균등/반사 지원, 0 제외
srt <이름> TX TY TZ RX RY RZ SX SY SZ [월드 기준 SRT 설정] - T: 위치, R: ZYX 회전 도, S: 축별 배율
pivot <center/cursor> [피벗 변경, 현재 배치 유지] - center: 형상 중심, cursor: 클릭 기준점
parent <부모/none> [선택 물체 상속 연결, 월드 배치 유지] - 부모: 물체 이름, none: 해제
layer add <이름> [지역 레이어 추가 후 선택] - 이름: 객체 내부 고유 이름
layer select <이름/none> [지역 편집 대상 지정] - none: 물체 배치 편집
layer t DX DY DZ [지역 이동 추가] - D: 선택 레이어 축의 이동 거리
layer r <축> <도> [지역 회전 추가] - 축: x/y/z, 도: 회전량
layer s <값 또는 SX SY SZ> [지역 배율 추가] - S: 축별 배율
layer before <이름> [선택 레이어 순서 변경] - 이름: 이 레이어 앞에 삽입
layer remove/reset/list [삭제/정적 변환 초기화/목록] - 인수 없음
layer enable/play <on/off> [기여/시계 제어] - off: 기여 제거/시간 정지
insert spin <축> <도/초> [자전 레이어 추가] - 축: 지역 축, 도/초: 각속도
insert orbit <축> <도/초> center <cursor 또는 X Y Z> [공전 레이어 추가] - 중심: 월드 점을 지역으로 변환
insert spin-axis AX AY AZ <도/초> [임의 지역 축 자전] - A: 0이 아닌 축 방향, 중심: 물체 피벗
insert orbit-axis AX AY AZ <도/초> center <cursor 또는 X Y Z> [임의 지역 축 공전] - A: 축 방향, 중심: 월드 점
insert linear DX DY DZ <초> [선형 이동 레이어] - D: 지역 변위, 초: 이동 시간
insert pendulum <축> <진폭 도> <주기 초> [진자 레이어] - 축: 지역 회전축
insert translate/rotate/scale <t/r/s 인수> [고정 변환 레이어] - 인수: 해당 지역 변환과 같음
light point [이름] [at cursor 또는 at X Y Z] [선택 가능한 점광원 생성] - 위치: 월드, 이름 생략: 자동
light color R G B [선택 광원 색] - RGB: 0~1
light intensity/attenuation <값> [선택 광원 강도/거리 감쇠] - 0 이상, intensity 0: 끄기
light sun <강도> [기존 방향광 강도] - 0: 끄기, 기본 1
render ambient R G B [독립 주변광 색] - RGB: 0~1, 기본 0 0 0
material [대상] phong Ka Kd Ks N [메시 전체 재질 설정] - Ka/Kd/Ks: 0~1, N: 광택 1~1024, 생략 대상: 선택
material [대상] unlit on/off [조명 계산 제외/적용] - 광원 표시/표지판에 사용, 다른 재질·텍스처 유지
camera add [이름] [at cursor 또는 at X Y Z] [카메라 객체 생성] - t/r/s/레이어로 이동
camera look <cursor 또는 X Y Z> [선택 카메라 시선 설정] - 목표: 월드 점
camera use [이름] [카메라로 전환] - 생략: 선택 카메라
camera free [일반 시점으로 복귀] - 마지막 시점은 유지
camera fov/extent <값> [투영 조정] - fov: 도, extent: 직교 반높이
camera clip <near> <far> [클리핑 조정] - 0 < near < far
cursor [X Y Z] [생성 기준점 조회/설정] - XYZ: 월드 좌표, 생략: 조회
plane <xy/xz/yz> <값> [빈 공간 클릭의 작업 평면] - 값: 평면의 고정 축 좌표
snap <간격> [생성 기준점 스냅] - 0: 끄기, 양수: 격자 간격
edit <object/part/face> [선택 세부모드] - 기본 object, part: 부품, face: 면
G/R/S [변환 미리보기] - X/Y/Z: 축, 마우스/방향키: 양, Shift+방향키: 미세, Enter: 확정, Esc: 취소
select <이름> [대상 지정] - 이름: 등록된 물체/부품
color <이름> R G B [색 변경] - RGB: 0~1
force add <ID> [물체] <기준/world> <질량> [힘 동작 연결] - 물체 생략: 선택, 기준: 강체 객체, 질량: kg
force gravity <ID> X Y Z [월드 중력 설정] - XYZ: m/s^2
force velocity/position <ID> X Y Z [지역 속도/위치] - XYZ: 기준 객체 좌표, m/s 또는 m
force spring <ID> <강성> <감쇠> AX AY AZ [스프링 저울 연결] - 강성: N/m, 감쇠: N*s/m, A: 지역 앵커
force read <ID> [하중 조회] - ID: 힘 동작. 스프링 힘과 정지 구속 반력을 구분
pause <on/off> [시간 정지/재개] - on: 정지, off: 재개. Space: 전환
bind <키> <press/hold> "명령" [입력 연결] - press: 1회, hold: 초당 move/rotate
save/load <파일> [레시피 저장/읽기] - 파일: 작업 폴더 기준 .assembly 경로
reload 또는 Ctrl+F5 [현재 레시피 재읽기] - C++ 재빌드와 별개
list/inspect/diagnose [이름] [상태 조회] - 이름 생략: 선택 물체
help 또는 F12 [도움말] - 인수 없음
clear [장면 비우기] - 인수 없음
quit [종료] - 인수 없음
```

`t/r/s`는 월드 기준 물체 배치를 바꾼다. `layer t/r/s`는 선택 레이어의 지역 변환을 추가한다. 입력량이 같아도 축·순서에 따라 결과가 다르다. `s`는 상대 배율이며 `srt`는 기준 SRT 설정이다.

[좌표·레이어 계약](TRANSFORM_WORKSPACE.md) · [응용 레시피](APPLICATION_RECIPES.md)
