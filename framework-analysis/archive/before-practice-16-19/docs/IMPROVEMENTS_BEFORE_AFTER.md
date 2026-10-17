# Step 4: example and documentation before / after

## include/Model.h

```diff
--- before
+++ after
@@ -107,12 +107,12 @@
 using Model = Polyhedron;
 
 struct Component_Poly {
-    std::optional<EntityId> parent; // Local SRT is relative to this entity.
     Polyhedron polyhedron;
     Transform3D transform;
     glm::vec3 sideLengths{1.0f};
     bool visible = true;
     PivotMode pivotMode = PivotMode::Preserve;
+    std::optional<EntityId> parent; // Local SRT is relative to this entity.
 };
 struct FaceId { EntityId entityId = 0; std::size_t faceIndex = 0; };
 struct Line3D {
```

## examples/hierarchy.cpp

```diff
--- before: none
+++ after
@@ -0,0 +1,51 @@
+#include "../Framework.h"
+#include <array>
+#include <iostream>
+
+int main() {
+    graphics::Application app;
+    std::array<graphics::EntityId,3> bodies{};
+    app.setInitHandler([&](graphics::Application& app) {
+        auto& scene=app.getScene();
+        const auto sun=scene.addCuboid({.8f,.8f,.8f},{0,0,0});
+        const auto earth=scene.addCuboid({.4f,.4f,.4f},{2,0,0});
+        const auto moon=scene.addCuboid({.2f,.2f,.2f},{.8f,0,0});
+        if(!sun||!earth||!moon)return false;
+        bodies={*sun,*earth,*moon};
+        if(!scene.setParent(*earth,*sun)||!scene.setParent(*moon,*earth))return false;
+        auto image=std::make_shared<graphics::TextureImage>();
+        image->width=2;image->height=2;
+        image->rgba={255,255,255,255, 20,20,20,255,
+                     20,20,20,255, 255,255,255,255};
+        graphics::Material material;material.texture=image;material.useTexture=true;
+        graphics::UVOptions uv;uv.mapping=graphics::UVMapping::Box;uv.scale={2,2};
+        for(auto id:bodies) {
+            const auto count=scene.getComponent(id)->polyhedron.faces.size();
+            for(std::size_t mesh=0;mesh<count;++mesh) {
+                if(!scene.generateTexcoords(id,mesh,uv)||!scene.setMaterial(id,mesh,material))return false;
+            }
+        }
+        app.getCamera().position={0,4,8};
+        app.getCamera().target={0,0,0};
+        return true;
+    });
+    app.setUpdateHandler([&](graphics::Application& app,float dt) {
+        auto& scene=app.getScene();
+        for(std::size_t i=0;i<2;++i) {
+            auto transform=scene.getComponent(bodies[i])->transform;
+            transform.rotationDegrees.y=std::fmod(transform.rotationDegrees.y+dt*(i?80.0f:20.0f),360.0f);
+            if(!scene.setTransform(bodies[i],transform))app.requestClose();
+        }
+    });
+    app.setMouseHandler([](graphics::Application& app,const graphics::MouseEvent& event) {
+        if(event.button!=GLFW_MOUSE_BUTTON_LEFT||event.action!=GLFW_PRESS)return;
+        if(const auto hit=app.pickAt(event.windowX,event.windowY))
+            std::cout<<"entity="<<hit->entityId<<" UV="<<hit->uv.x<<","<<hit->uv.y<<'\n';
+    });
+    app.setKeyHandler([](graphics::Application& app,int key,int action) {
+        if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)app.requestClose();
+    });
+    graphics::ApplicationConfig config;config.features.textures=true;
+    config.title="Hierarchy and UV mapping";
+    return app.run(config);
+}
```

## README.md

```diff
--- before
+++ after
@@ -137,3 +137,32 @@
 - [인터랙티브 코드 그래프](../framework-analysis/graphify-out/graph.html)
 
 참고 자료: [ComputerGraphics](https://github.com/maru3172/ComputerGraphics)의 컴그_29번_텍스처매핑과 컴그_파이프라인1차. 정점·UV·법선 입력과 모델/뷰/투영·재질 흐름을 참고했다. 프레임워크의 자원 수명, 일반 인덱스 메시와 피킹은 별도 구현이다.
+
+## 엔티티 계층 SRT
+
+`Scene::setParent(child, parent)`로 계층을 연결하고 `setParent(child, std::nullopt)`로 해제한다. ID 오류·자기 연결·순환 연결은 false를 반환하며 기존 연결을 유지한다. 부모 연결·해제 시 local SRT를 유지하므로 월드 위치는 바뀔 수 있다.
+
+`worldMatrix(id)`는 부모 world × 자식 local 행렬을 반환한다. `faceWorldMatrix(id, meshIndex)`는 면 변환까지 포함한다. `setTransform`과 `setPosition`은 부모가 있는 경우 부모 좌표의 값을 설정한다. 부모가 없으면 기존 월드 기준 동작이다. 부모 회전·비균일 크기·피벗은 자식에게 전달된다. 연속된 회전과 비균일 크기는 월드 공간에서 전단을 만들 수 있으므로 행렬을 SRT로 다시 분해하지 않는다.
+
+입력 local 정점 → 조상 SRT 누적 → 물체·면 변환 → view/projection → 화면이다. 렌더링, 법선 변환, 피킹, owner가 지정된 선·축은 같은 계층 행렬을 사용한다. 부모 visible=false는 부모 자체만 숨긴다. 자식의 표시 상태는 독립적이다. 변환만 변경하면 GPU 정점 버퍼를 다시 업로드하지 않는다.
+
+## UV 자동 생성과 조정
+
+`Scene::generateTexcoords(id, meshIndex, options)`와 독립 메시용 `graphics::generateTexcoords(mesh, options)`를 제공한다. 기존 UV를 교체하는 명시적 작업이다. 재질을 활성화하기 전에 호출한다.
+
+- Planar: axis=0/1/2에 해당하는 축을 버리고 메시 경계 범위를 0~1로 정규화한다. 기본 axis=2는 XY 투영이다.
+- Box: 삼각형 법선의 가장 큰 축을 버리고 투영한다. 면 방향마다 정점을 분리한다. 텍스처 아틀라스 배치는 제공하지 않는다.
+- Cylindrical: Y축 둘레 atan2로 U, 높이로 V를 계산한다. 원통 측면용이다. 뚜껑은 별도 메시에서 Planar로 매핑한다.
+- Spherical: 메시 경계 상자의 중심을 기준으로 경도 atan2와 위도 asin을 계산한다. 구면에 적합하며 임의 모델을 왜곡 없이 펼치는 알고리즘은 아니다.
+
+원통·구면의 U 경계는 정점을 분리하며 일부 U가 1을 넘을 수 있다. `Material::repeatTexture=true`를 사용한다. Box·Cylindrical·Spherical은 삼각형별 정점으로 바꾸므로 정점 번호와 삼각형 번호를 별도로 저장한 호출부는 다시 조회한다. 색상·법선·재질·면 SRT·명시적 외곽선 위치는 유지한다.
+
+`UVOptions::scale`, `offset`, `rotationDegrees`는 원점 기준으로 UV 크기 → 회전 → 이동을 적용한다. 기존 UV만 조정하려면 `Scene::transformTexcoords` 또는 `graphics::transformTexcoords`를 사용한다. 기본 도형은 자동으로 UV를 갖지 않는다. `generateTexcoords` → `setMaterial` → textures 렌더 옵션 순서로 연결한다.
+
+[examples/hierarchy.cpp](examples/hierarchy.cpp)는 기존 직육면체를 태양·지구·달의 자리로 사용한다. 태양 회전은 지구 공전을, 지구 회전은 달 공전을 만든다. 실제 천체 크기나 독립 자전·공전 구조의 구현은 아니다. 메모리 체크무늬 이미지는 추가 파일 없이 UV 반복을 보여준다. 파일 이미지는 기존 `AssetLoader::loadTexture` 결과를 material.texture에 지정한다.
+
+## 로더 구현 분리
+
+공개 진입점은 `include/ModelLoader.h`의 AssetLoader다. 기본 도형·OBJ 부분집합 파서는 `detail/PrimitiveLoader.inl`, 일반 정적 모델·재질·이미지 로딩은 `detail/AssimpLoader.inl`에 있다. 사용자는 .inl을 직접 포함하지 않는다. loadPrimitive와 loadModel의 활성화 조건·오류·경로 제한은 유지한다. 일반 모델의 지원 형식은 포함된 Assimp DLL의 지원 범위에 따른다. 가져온 노드 변환은 정점에 반영되며 노드 계층·뼈대 애니메이션은 보존하지 않는다.
+
+참고: [Khronos sampler](https://wikis.khronos.org/opengl/Sampler_%28GLSL%29), [Khronos sampler wrap](https://wikis.khronos.org/opengl/Sampler_Object). 기존 UV를 보간해 샘플링하는 경로는 유지하고, CPU에서 사용할 투영 UV 생성 기능을 추가했다.
```

## docs/USER_CHECKS.md

```diff
--- before
+++ after
@@ -62,3 +62,16 @@
 
 실제 사용한 MSVC 버전, Debug/Release, 모델·이미지 파일, 창 배율과 결과를 기록한다.
 실패 시 빌드 로그 또는 오류 문자열과 해당 호출을 전달한다. 실행 확인 전 성공으로 기록하지 않는다.
+
+## 계층·UV·로더 개선: 사용자 실행 항목
+
+빌드·실행·테스트는 Codex가 수행하지 않았다. 사용자가 Debug|x64 및 Release|x64에서 각각 확인한다. examples의 main은 서로 별도 프로그램이며 한 프로젝트에 동시에 넣지 않는다.
+
+1. hierarchy.cpp를 진입점으로 실행한다. 세 직육면체와 반복 체크무늬가 보이고 작은 두 물체가 계층을 따라 이동해야 한다. 클릭 시 해당 물체 ID와 UV가 출력되어야 한다.
+2. 부모 위치 (10,0,0), 자식 local 위치 (2,0,0), 손자 local 위치 (1,0,0), 나머지 SRT 단위 상태에서 worldMatrix의 원점이 각각 (10,0,0), (12,0,0), (13,0,0)인지 확인한다. 피벗은 0으로 둔다.
+3. 손자를 부모로 지정하는 순환 연결, 자기 연결, 잘못된 ID는 false를 반환하고 기존 계층을 유지해야 한다. 연결 해제는 local SRT를 유지한다. 부모 비균일 크기와 회전도 자식·피킹·owner 선에 반영되어야 한다.
+4. XY 평면은 Planar axis=2로 0~1 범위에 매핑되어야 한다. Box는 각 면에 체크무늬가 나타나야 한다. Y축 원통 측면과 구체 메시를 별도로 준비해 Cylindrical/Spherical의 U 경계와 극점을 관찰한다. 경계의 U>1은 repeatTexture=true로 반복되어야 한다.
+5. UV scale=(2,2), offset=(.25,0), rotationDegrees=90을 차례로 적용해 반복·이동·회전을 확인한다. NaN 옵션은 false를 반환하고 메시를 유지해야 한다. UV 생성 후 명시적 외곽선과 색상·법선도 확인한다.
+6. 기존 cube.txt와 v/vt/vn/f OBJ 부분집합을 loadPrimitive로 읽고, OBJ+MTL 및 UV가 있는 일반 모델을 loadModel로 읽는다. 지정 이미지 override, 누락 파일·누락 UV·프로젝트 밖 경로·비활성 기능의 오류 결과가 유지되어야 한다.
+
+Box/원통/구면 매핑은 정점 수를 늘린다. 복잡한 모델의 최소 왜곡 UV·아틀라스·자동 원통 뚜껑 분류는 제공하지 않는다.
```

## CHANGES.md

```diff
--- before
+++ after
@@ -3503,3 +3503,12 @@
 전체 저장소의 graphify 그래프 생성/갱신은 이번 승인 범위에 포함되지 않아
 수행하지 않았다. 위 흐름은 소스 정적 확인에 따른 설명이다.
 ````
+
+## 2026-10-12 개선
+
+1. 엔티티 계층 SRT와 렌더·피킹·owner 선 연결.
+2. 평면·박스·원통·구면 UV 생성 및 UV 조정.
+3. 기본 도형 파서와 Assimp 로더 구현 분리.
+4. 예제·설명·framework-analysis 갱신.
+
+빌드·테스트 미실행. 전체 변경 블록은 docs/HIERARCHY_BEFORE_AFTER.md, UV_BEFORE_AFTER.md, LOADERS_BEFORE_AFTER.md, IMPROVEMENTS_BEFORE_AFTER.md에 기록한다.
```

