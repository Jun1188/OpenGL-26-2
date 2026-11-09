# 실습 24: 1초 적재 선형보간

2026-11-09 승인 범위. 소스 커밋 `7a9f8ed`. 문서 커밋은 최종 응답에 기록한다. 빌드·테스트·검증기·앱 실행은 사용자에게 맡긴다.

## 동작과 책임

1~5 → loadParcel → 시작 월드 행렬 저장·이동 플래그 설정 → onUpdate의 updateLoading(dt) → 1초 동안 위치 보간 → 완료 시 트럭 부모 연결·슬롯 자세 적용·적재 확정 순서다.

`glm::mix(start, currentSlot, elapsed)`를 사용한다. elapsed는 각 상자마다 0~1초로 제한한다. 트럭이 정지하면 고정된 직선 경로를 따른다. 트럭 이동 중에는 현재 슬롯을 목표로 갱신하므로 경로가 고정 직선은 아니다. 이동 중 자세는 시작 자세를 유지하고, 도착 시 기존 적재 방향으로 맞춘다. 회전 보간이나 충돌 회피는 구현 범위가 아니다.

이동 플래그가 켜진 상자는 벨트 배치에서 제외한다. 이동 시간은 컨베이어 시계와 독립된 프레임 dt를 사용한다. 따라서 b로 벨트를 정지해도 적재 이동은 진행한다. 상자별 상태를 사용하므로 서로 다른 번호는 동시에 이동할 수 있다. loaded는 도착 후에만 설정한다. 이동 중/적재 후 같은 번호는 무시한다. r은 initScene/resetState로 장면과 시작 행렬·경과 시간·플래그를 초기화한다.

onUpdate: 트럭 이동 → 벨트 배치 → 적재 이동 → 카메라 갱신 순서다. 기존 boxes_의 EntityId를 그대로 사용해 v/n/N은 이동 중에도 해당 상자의 최종 월드 변환을 따른다.

Motion::linear도 확인했다. 이번에는 움직이는 목표를 갱신하고 카메라보다 먼저 위치를 적용해야 하므로 기존 Scene 배치 API와 GLM mix를 사용해 Console 안에서 상태를 관리했다. 프레임워크 사본과 시스템 API는 수정하지 않았다.

## 실습 20 입력 대조

실습 20 Console.cpp의 onKey/onUpdate와 대조했다. b 시작·정지, +/- 줌, p/P 투영, 좌/우 공전, r 초기화, q 종료가 실습 24에 있다. 실습 24의 줌·투영·공전은 전체 시점에서만 동작한다. 이 입력들은 이번에 새로 추가하지 않았다.

## 코드 블록 전체 before / after

before는 시작 커밋 c13eeb7, after는 저장된 코드다. 표준 문서의 P24.md에도 Console 원문을 재생성했다.

### openGL_3dAssign_24/Console.h

before L11

없음

after L11

```cpp
    std::array<bool, 5> loading_{};
    std::array<float, 5> loadingSeconds_{};
    std::array<glm::mat4, 5> loadingStart_{};
```

before L26

없음

after L29

```cpp
    void updateLoading(float dt);
```

### openGL_3dAssign_24/Console.cpp

before L102

```cpp
              << "1~5 적재 | V 상자/전체 시점 | n/N 다음/이전 상자 (처음 5번)\n"
```

after L102

```cpp
              << "1~5 1초 선형보간 적재 | V 상자/전체 시점 | n/N 다음/이전 상자 (처음 5번)\n"
```

before L138

```cpp
        if (!loaded_[i]) place(boxes_[i], travelled + offset, .03f + boxSize * .5f);
```

after L138

```cpp
        if (!loaded_[i] && !loading_[i]) place(boxes_[i], travelled + offset, .03f + boxSize * .5f);
```

before L146

없음

after L146

```cpp
    loading_.fill(false);
    loadingSeconds_.fill(0);
    loadingStart_.fill(glm::mat4(1));
```

before L261

없음

after L264

```cpp
    updateLoading(dt);
```

before L294

```cpp
    if (loaded_[index]) {
        std::cout << "이미 적재한 상자: " << index + 1 << '\n';
```

after L298

```cpp
    if (loaded_[index] || loading_[index]) {
        std::cout << "이동 중이거나 이미 적재한 상자: " << index + 1 << '\n';
```

before L298

없음

after L302

```cpp
    const auto start = app_.getScene().worldMatrix(boxes_[index]);
    require(start.has_value(), "Parcel starting placement missing.");
    loadingStart_[index] = *start;
    loadingSeconds_[index] = 0;
    loading_[index] = true;
    std::cout << "상자 이동 시작: " << index + 1 << '\n';
}

void Console::updateLoading(float dt) {
```

before L301

```cpp
    const glm::vec3 slot{index % 2 == 0 ? -.48f : .48f, 1.365f, -.85f + .9f * float(index / 2)};
    const auto destination = *truckWorld * glm::translate(glm::mat4(1), slot)
        * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
    require(scene.setParentPreserveWorld(boxes_[index], truck_), "Parcel attachment failed.");
    require(scene.setWorldPlacement(boxes_[index], destination), "Parcel slot placement failed.");
    loaded_[index] = true;
    std::cout << "트럭 적재: " << index + 1 << '\n';
```

after L314

```cpp
    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        if (!loading_[i]) continue;
        loadingSeconds_[i] = std::clamp(loadingSeconds_[i] + dt, 0.0f, 1.0f);
        const glm::vec3 slot{i % 2 == 0 ? -.48f : .48f, 1.365f, -.85f + .9f * float(i / 2)};
        const auto destination = *truckWorld * glm::translate(glm::mat4(1), slot)
            * glm::rotate(glm::mat4(1), glm::radians(180.0f), glm::vec3(0, 1, 0));
        auto placement = loadingStart_[i];
        placement[3] = glm::vec4(glm::mix(glm::vec3(loadingStart_[i][3]), glm::vec3(destination[3]),
                                        loadingSeconds_[i]), 1.0f);
        if (loadingSeconds_[i] >= 1.0f) {
            require(scene.setParentPreserveWorld(boxes_[i], truck_), "Parcel attachment failed.");
            placement = destination;
        }
        require(scene.setWorldPlacement(boxes_[i], placement), "Parcel transfer placement failed.");
        if (loadingSeconds_[i] >= 1.0f) {
            loaded_[i] = true;
            loading_[i] = false;
            std::cout << "트럭 적재 완료: " << i + 1 << '\n';
        }
    }
```

before L328

```cpp
                      << (loaded_[i] ? ", 적재)" : ", 벨트)") << " camera="
```

after L354

```cpp
                      << (loaded_[i] ? ", 적재)" : loading_[i] ? ", 이동 중)" : ", 벨트)") << " camera="
```

### tools/Write-Practice24Plus.py

before L13

```python
        ('번호에 해당하는 상자 적재', '1~5', 'loadParcel', '트럭에 부착 후 지정 적재 위치로 배치; 재입력은 중복 적재 방지'),
```

after L13

```python
        ('번호에 해당하는 상자 적재', '1~5', 'loadParcel/updateLoading', '현재 월드 위치에서 현재 트럭 슬롯까지 1초 위치 선형보간; 도착 후 부착·적재 확정; 이동 중/적재 후 중복 입력 무시'),
```

## 구조 자료의 범위

[기존 전체 프로젝트 진입점](ARCHITECTURE.md)과 [graphify 그래프](../../graphify-out/practice-24-26-27/graph.html)는 이전 추출 자료다. 이번에는 그래프를 재추출하지 않았다. 새 updateLoading 함수와 변경된 줄 번호는 그래프에 아직 반영되지 않았다. 현재 호출 흐름은 위의 소스 대조 설명과 전체 변경 코드로 보충한다. 정적 소스 설명을 새 graphify 추출 결과나 실행 증거로 보지 않는다.

## 사용자가 확인할 항목

1. 직접 재빌드·실행 후 1을 누른다. 순간 이동하지 않고 약 1초 동안 현재 위치에서 슬롯으로 이동해야 한다.
2. 1을 이동 중·완료 후 반복한다. 이동이 재시작되거나 상자가 복제되지 않아야 한다. 1~5를 빠르게 누르면 각자 이동해야 한다.
3. b 시작·정지 상태 모두에서 적재한다. 이동 상자는 벨트에 끌려가지 않아야 한다.
4. g로 트럭을 움직인 상태에서 적재한다. 현재 슬롯에 도착하고 이후 트럭을 따라가야 한다.
5. 이동 중 v/n/N으로 선택 상자의 시점을 관찰한다. 도착 시 적재 방향으로 시선이 바뀔 수 있다.
6. 이동 도중 r을 누른다. 모든 이동이 취소되고 상자 5개가 초기 벨트 위치로 돌아와야 한다.
7. 전체 시점에서 b, +/-, p/P, 좌/우, r, q를 확인한다.

실행·화면은 미확인이다. 사용자 원본 Renderer.cpp, PDF, docs.zip, 기존 캐시는 보존했다. 임시 소스 파일은 만들지 않았다.
