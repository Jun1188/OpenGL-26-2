# 변환·피벗·상속을 코드로 결정하기

먼저 위치가 어느 좌표계인지 적는다. 동일한 (1,0,0)도 모델 지역 이동, 부모 기준 이동, 월드 이동 중 무엇인지에 따라 결과가 다르다. 이 구분을 변수 이름과 곱셈 순서에 남긴다.

## 1. 열벡터 규약에서 변환 순서

```cpp
glm::mat4 model = glm::translate(glm::mat4(1), worldPosition)
                * glm::rotate(glm::mat4(1), glm::radians(angle), axis)
                * glm::scale(glm::mat4(1), scale)
                * glm::translate(glm::mat4(1), -modelPivot);
```

정점에는 오른쪽부터 작용한다. 모델 피벗을 원점으로 이동하고, 축별 배율과 회전을 적용한 뒤 월드 위치에 놓는다. 이 정의에서 worldPosition은 지정 피벗이 갈 위치다. 피벗 숫자만 고쳐도 물체 중심이 그대로인 것은 아니다. 단위 상자에 scale=(2,1,0.5), pivot=(-0.5,0,0), position=(3,1,0), Z회전90도를 주면 피벗은 (3,1,0), 중심은 (3,2,0)이다. 이는 행렬로 계산한 예측값이다.

특정 점 C 주위에 이미 놓인 물체를 월드에서 회전시키려면 다음을 앞에 곱한다.

```cpp
glm::mat4 around = glm::translate(glm::mat4(1), C)
                 * glm::rotate(glm::mat4(1), glm::radians(degrees), axis)
                 * glm::translate(glm::mat4(1), -C);
model = around * model;
```

앞에 곱한 월드 회전과 뒤에 곱한 지역 회전을 구분한다. 자전과 공전은 같은 rotation 함수라도 기준점과 곱셈 위치가 다르다.

## 2. 월드 배치로 부모에 연결하기

```cpp
glm::mat4 attachment = glm::inverse(parentWorld) * desiredWorld;
glm::mat4 childWorld = parentWorld * attachment * localLayer1 * localLayer2;
```

연결 순간에는 desiredWorld를 유지하고, 이후 부모 변화는 상속한다. 부모가 비균등 배율과 회전을 가진다면 곱셈 결과에 전단이 생길 수 있다. 이를 억지로 Euler 각과 배율 세 값으로 다시 분해하지 않고 행렬로 보관한다. inverse가 필요한 작업에는 0 배율이나 특이 행렬을 허용하지 않는다. Standard의 Scene::setParentPreserveWorld와 setWorldPlacement가 이 연산을 맡는다.

## 3. 위치만 따르고 자세는 따로 정하기

예를 들어 끝점에 붙은 받침대의 수평을 유지한다. 아래 코드는 팔의 지역 원점이 관절이고 끝점이 (0,-armLength,0)이며 받침대 자신의 지역 레이어는 없다고 가정한다.

```cpp
glm::vec3 endWorld = glm::vec3(armWorld * glm::vec4(0,-armLength,0,1));
glm::mat4 trayWorld = glm::translate(glm::mat4(1), endWorld)
                    * glm::rotate(glm::mat4(1), glm::radians(columnYaw), glm::vec3(0,1,0))
                    * glm::scale(glm::mat4(1), trayScale);
// 순정: trayWorld를 model uniform으로 전달한다.
// Standard: scene.setWorldPlacement(trayId, trayWorld)의 성공 여부를 확인한다.
```

끝점 위치는 전체 armWorld로 계산하지만, 자세는 수직축 yaw만 포함한다. 여러 축이 합성된 상태에서 임의의 각도 하나를 음수로 넣는 것으로 모든 회전을 상쇄한다고 가정하지 않는다. 어떤 회전을 유지할지 먼저 정책을 정한다. 위치 제약과 방향 제약은 별도 계산이다.

## 4. 동일 원리의 전이

문제의 사물이 팔·문·차양·전시대·카메라 중 무엇인지보다, 기준 프레임과 고정점이 무엇인지가 중요하다. 지역 길이, 모델 형상 크기(size), 변환 배율(scale)을 분리한다. size로 정점을 크게 만든 것과 부모 scale로 자식 좌표계까지 늘린 것은 다른 결과다.

N02는 순정 행렬을 직접 작성하고, N08은 Standard에서 같은 조건을 구현한다. S02/S04에서는 고정 모서리와 유지할 방향을 동시에 요구한다.
