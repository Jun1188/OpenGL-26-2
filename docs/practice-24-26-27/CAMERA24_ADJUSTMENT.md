# 실습 24 카메라·트럭 배치 수정 — 2026-11-09

승인한 범위의 소스와 참조 문서를 저장했다. 빌드·테스트·검증기·앱 실행은 하지 않았다. 아래 화면 결과는 사용자 확인 전이다.

## 변경과 커밋

- 1단계 `6cefc23`: 상자 지역 카메라 Z를 0.505에서 -1.425로 변경. 지역 높이 0.775와 +Z 시선은 유지한다.
- 2단계 `d277470`: 트럭 중심을 (-7,0,7.5)에서 (5.5,0,1.0)으로 변경. Z 이동 경계 -7.0~9.3은 유지한다.
- 3단계: 문서 생성기의 배치·카메라 설명, 전체 코드 참조, 예상 좌표, 사용자 확인 절차, 이 변경 보고서를 갱신한다. 문서 커밋 해시는 최종 응답에 기록한다.

입력 v/n/N → 선택 배열 인덱스 → 상자의 최종 월드 행렬 × 지역 카메라 → 화면 순서다. 미적재 상자는 컨베이어 변환을, 적재 상자는 트럭 부모와 적재 자세를 함께 따른다. 카메라를 전방에서 후방으로 옮겨 상자 윗부분이 화면 아래에 들어오도록 했으며, 실제 보이는 범위와 다른 상자에 의한 가림은 실행 확인이 필요하다.

트럭 최소 X는 바퀴를 포함해 4.275다. 벨트 세로 구간의 바깥 X=3.8과 간격 0.475를 둔다. 초기 적재함 Z 범위 -0.6~2.6은 세로 벨트 Z=-1.2~3.8 옆에 놓인다. [PDF 원본](../../lecture_pdf/homework/지엘3%20좌표계%20변환%20(실습%2024).pdf) p.1의 바깥 옆 배치를 따른다.

## 전체 코드 블록 before / after

before는 작업 시작 커밋 `219615a`, after는 최종 저장한 코드다. 변경한 블록을 모두 포함한다. 생성 문서 `standard/docs/reference/practices/P24.md`에도 아래 Console.h와 Console.cpp 블록을 그대로 반영했다.

### openGL_3dAssign_24/Console.cpp

트럭의 초기 중심을 세로 벨트 바깥 옆으로 옮기고, 선택 상자의 지역 카메라를 뒤로 물린다.

before — L9

```cpp
constexpr float truckX = -7.0f;
constexpr float truckStartZ = 7.5f, truckMinZ = -7.0f, truckMaxZ = 9.3f;
```

after — L9

```cpp
constexpr float truckX = 5.5f;
constexpr float truckStartZ = 1.0f, truckMinZ = -7.0f, truckMaxZ = 9.3f;
```

before — L312

```cpp
    local.position = {0, boxSize * .5f + .35f, boxSize * .5f + .08f};
```

after — L312

```cpp
    local.position = {0, boxSize * .5f + .35f, -boxSize * .5f - 1.0f};
```

### openGL_3dAssign_24/Console.h

트럭 Z 멤버의 초기값을 새 시작 위치와 맞춘다.

before — L15

```cpp
    float truckZ_ = 7.5f;
```

after — L15

```cpp
    float truckZ_ = 1.0f;
```

### tools/Write-Practice24Plus.py

문서를 다시 생성할 때 변경한 배치와 카메라 설명을 유지한다.

before — L12

```python
        ('컨베이어 옆 육면체 트럭', '시작', 'addTruck', '차체·적재함·운전석·바퀴를 트럭 루트 아래 생성; X=-7로 L자 벨트 바깥 배치, 바닥 22×21'),
```

after — L12

```python
        ('컨베이어 옆 육면체 트럭', '시작', 'addTruck', '차체·적재함·운전석·바퀴를 트럭 루트 아래 생성; 중심 (5.5,0,1.0), L자 벨트 세로 구간의 X 바깥에 나란히 배치, 바닥 22×21'),
```

before — L14

```python
        ('상자 시점/전체 시점 전환', 'V', 'onKey/updateCamera', '전체 시점은 저장·복귀; 후보 5개를 매 프레임 각 상자의 최종 월드 변환에서 계산'),
```

after — L14

```python
        ('상자 시점/전체 시점 전환', 'V', 'onKey/updateCamera', '전체 시점은 저장·복귀; 지역 카메라 (0,0.775,-1.425)를 각 상자의 최종 월드 변환으로 옮겨 후보 5개를 갱신; 화면 아래 선택 상자 일부가 보이는지 확인'),
```

## 구조 자료

[기존 저장소 전체 실행 프로젝트·진입점](ARCHITECTURE.md)과 [graphify 그래프](../../graphify-out/practice-24-26-27/graph.html)를 재사용했다. 그래프는 이전 AST 추출 자료이며 이번에 재추출하지 않았다. 숫자 리터럴만 변경했으므로 함수·클래스·호출 관계는 유지한다. 조회 어휘는 기존 그래프에 있는 console/camera/truck/main이다. 이번 작업에 별도 모델 추출 호출은 없다.

현재 소스로 대조한 경로:

- `main.cpp:5`: 독립 Application과 Console을 생성하고 초기화·입력·포커스·업데이트 콜백을 등록한 뒤 run을 호출한다. 등록 시점과 콜백 실행 시점을 구분한다.
- `Console.cpp:27` initScene → `Console.cpp:273` addTruck → `Console.cpp:142` resetState. 새 중심 좌표는 트럭 루트와 초기 상태에 적용된다.
- `Console.cpp:180` onKey의 v/n/N → `Console.cpp:310` updateCamera. `Console.cpp:247` onUpdate는 트럭 이동과 미적재 물체 배치 뒤 같은 카메라 갱신을 호출한다.
- `Console.cpp:293` loadParcel → 트럭 부모·적재 슬롯 → 최종 worldMatrix. `CameraSystem::attached`가 지역 카메라를 월드 공간으로 옮긴다. 카메라 이후의 MVP·셰이더·렌더러 계약은 변경하지 않았다.

그래프의 addTruck → 다른 프로젝트 part 간선은 AMBIGUOUS다. 현재 소스의 part는 addTruck 안의 지역 람다다. 또한 그래프에 기록된 루트 framework 클래스는 구조 분석용 공통 원본이며 실제 실습 빌드 입력은 이 폴더의 framework 사본이다. 프로젝트 사이에 호출하거나 외부 프레임워크에 의존하는 것으로 해석하지 않는다.

## 사용자 확인

1. 실습 24를 직접 재빌드하고 실행한다. 트럭이 세로 벨트의 바깥 옆에 서 있고 벨트와 겹치지 않는지 확인한다.
2. 미적재 상태에서 v, n/N을 누른다. 선택 상자의 윗부분이 화면 아래에 보이고 다섯 상자 시점이 순환해야 한다.
3. 일부 적재·전체 적재 후에도 v, n/N을 확인한다. [예상 좌표 표](CAMERA24.md)와 콘솔 좌표를 비교하고, 다른 상자나 운전석의 가림을 확인한다.
4. g 이동·끝점 정지·반대 방향 출발, v 전체 시점 복귀, r 초기화를 확인한다.

원본 사용자 변경인 실습 26 Renderer.cpp, PDF, docs.zip, 기존 Python 캐시는 수정·삭제·커밋하지 않았다. 별도 임시 소스 파일은 만들지 않았다. 소스·문서 반영과 화면 확인 완료는 서로 다르다.
