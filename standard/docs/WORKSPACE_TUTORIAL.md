# 두 물체를 C++ 코드로 작성하기

디버거 명령으로 수정안을 받고 사용자가 소스를 편집하는 순서다. 아래 명령 자체는 Standard 장면을 변경하지 않는다.

## 1. 부모와 자식의 월드 배치

기본 `SceneSetup.cpp`는 base를 `(0,0,0)`, panel을 `(0,1,0)`에 생성하고 panel을 base에 연결한다. 두 값은 등록 순간의 월드 기준 배치다.

```text
show object base
show object panel
object panel position 3 1 0
```

출력한 실제 파일·행의 after를 직접 반영하고 저장한다. panel 등록 시 base의 현재 월드 행렬 역행렬로 attachment를 계산한다. 초기 base 위치를 바꾸어 재빌드해도 panel은 자신이 지정한 월드 배치에서 시작한다. 이후 실행 중 base가 변하면 panel은 그 변화를 상속한다.

## 2. 비균등 배율과 피벗

```text
object panel scale 2 1 0.5
layer hinge pivot -1 0 0
layer hinge rotation 0 90 0
```

각 명령의 before가 현재 소스와 같은지 확인한 뒤 수동으로 반영한다. panel의 형상 크기는 기본 `(2,1,0.2)`다. object scale과 hinge 지역 회전은 서로 다른 행렬이다. 비균등 배율 뒤의 지역 회전은 전단을 포함할 수 있다. 객체 자체의 단순 월드 Y 회전을 원하면 layer 회전 대신 object rotation을 편집한다.

## 3. 시간 회전과 지역 이동 레이어

```text
layer hinge rotation 0 0 0
layer hinge spin 30
add layer shift panel
```

shift 전체 코드를 실제 삽입 지점에 반영한 뒤 다음 명령을 입력한다.

```text
layer shift translation 1 0 0
```

힌지 회전을 shift가 상속하므로 이동 방향은 회전한다. shift를 먼저 적용하려면 C++의 두 `scene.addLayer` 호출 순서를 바꾼다. 피벗·배율·이동의 순서를 눈으로 예측하고 사용자가 빌드 후 관찰한다. Space는 시간 정지, Home은 0초 복귀다.

## 4. 조명과 카메라

```text
light key position 2 4 1
light key intensity 2
object panel specular 0.8
object panel shininess 128
camera main position 8 5 10
camera main target 0 1 0
```

부모에 광원을 붙이려면 `light key parent base`의 수정안을 반영한다. 이후 position은 부모 지역 좌표다. 재빌드 후 광원 방향, 표면 법선, 카메라 위치가 정반사에 주는 영향을 각각 관찰한다. 이 문서의 화면 결과는 에이전트가 실행해 확인하지 않았다.

회상 질문: 같은 지역 X 이동 1이 object scale과 앞 레이어의 회전 뒤에서 어떤 월드 방향·거리로 나타나는가?
