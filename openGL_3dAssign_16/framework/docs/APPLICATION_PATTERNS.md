# 프레임워크 응용 패턴

참고: [ComputerGraphics](https://github.com/maru3172/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a). 폴더 번호는 이번 PDF 실습 번호와 다르다. 원본 코드를 복사하지 않고 변환·입력·자산 흐름을 참고했다. GL_POLYGON, 전역 상태와 타이머 중복 등록은 도입하지 않는다.

## 초기 표시와 객체 자전

API: Camera, Transform3D, Scene::setTransform. 입력은 기본 기저의 정점, 상태는 객체 회전과 독립 카메라, 출력은 projection * view * model * vertex다.

```cpp
auto viewRotation = glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1,0,0}), glm::radians(30.0f), {0,1,0});
auto inverseView = glm::transpose(glm::mat3(viewRotation));
camera.position = inverseView * glm::vec3(0,0,4);
camera.up = inverseView * glm::vec3(0,1,0);
transform.rotation = glm::mat3(glm::rotate(glm::mat4(transform.rotation), deltaRadians, {0,1,0}));
scene.setTransform(id, transform);
```

## 기준점 회전과 순차 면 애니메이션

참고: 컴그17의 면별 상태, 컴그_15_16의 시간 변화. API: setFaceLocalMatrix, setFaceSRT. 원본 정점에서 매 프레임 `T(pivot) * R * T(-pivot)`를 계산한다. 닫힌 행렬은 항등이다. 상태는 진행률·방향·실행 여부이며 타이머를 재등록하지 않는다. 한 면씩 동작하려면 전체 진행시간에서 면 번호와 해당 면의 진행률을 계산한다.

```cpp
auto hinge = glm::translate(glm::mat4(1), pivot)
           * glm::rotate(glm::mat4(1), angleRadians, axis)
           * glm::translate(glm::mat4(1), -pivot);
scene.setFaceLocalMatrix(id, faceIndex, hinge);
```

실습 16: 높이 h, 바닥 반폭 b에서 안쪽 평면까지는 atan2(h,b), 바깥쪽 평면까지는 180°-atan2(h,b), 아래쪽 공통 꼭짓점까지는 안쪽 방향 2*atan2(h,b)다. h=0.8,b=0.4에서 실습 6은 -126.870°, 실습 7은 바깥쪽 116.565°를 사용한다. PDF의 90°·180°는 목표 자세를 가리키는 표현으로 해석했다. 실제 면을 늘이거나 정점 하나만 이동하지 않는다.

## 자전·공전과 계층 조립

참고: 컴그_15_16의 자전·자리 교환, 컴그_파이프라인1차의 모델 변환. API: addGroup, setParent, setPosition, setTransform. 공전 노드와 시각 모델을 분리한다. 태양의 크기 변화는 행성 궤도 부모에 넣지 않는다. 달 궤도는 행성의 위치 노드 아래에 두고 행성의 시각적 크기 노드와 분리한다.

```cpp
auto orbit = scene.addGroup();
auto body = scene.addSphere(0.3f, {2,0,0});
if (orbit && body) scene.setParent(*body, *orbit);
```

탱크는 이동 루트 -> 높이 연결점 -> 회전 중앙 노드 -> 상부 이동 노드 -> 포신/깃대 피벗 순서다. 메시 크기 변화와 자식 부착 높이는 분리하여 접지와 연결을 유지한다.

## 카메라 자전과 공전

API: Camera, CameraSystem. 자전은 camera.position을 고정하고 target-position과 up을 카메라 Y축 주위로 회전한다. 공전은 position/target/up을 원점의 월드 Y축 주위로 함께 회전한다. 카메라 이동은 position과 target을 함께 이동하여 방향을 유지한다. 변환 결과가 validCamera를 만족할 때만 적용한다.

## 재질·UV·피킹

참고: 컴그_조명까지, 컴그_29번_텍스처매핑. API: AssetLoader, generateTexcoords, setMaterial, pickAt. UV를 생성한 뒤 재질을 연결하고 textures 기능을 켠다. 임의의 상대경로만 인자로 받는다. 선택한 면의 피벗과 SRT는 setFaceSRT로 응용한다. setTint로 색상만 바꾸면 정점을 재업로드하지 않는다.

```cpp
if (auto hit = app.pickAtCursor()) {
    scene.splitFace(hit->entityId, hit->faceIndex);
}
```

과제별 키·교환 경로·진동 범위·부품 구성은 응용 코드의 책임이다. 프레임워크에는 태양계·탱크·과제 번호 전용 API를 만들지 않는다.

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

