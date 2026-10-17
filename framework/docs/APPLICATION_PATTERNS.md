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

실습 16: 높이 h, 바닥 반폭 b에서 바닥 평면까지의 힌지 회전은 atan2(h,b), 아래쪽 공통 꼭짓점까지는 2*atan2(h,b)다. h=0.8,b=0.4이면 약 63.435°와 126.870°다. PDF의 90°·180°는 목표 자세를 가리키는 표현으로 해석했다. 실제 면을 늘이거나 정점 하나만 이동하지 않는다.

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
