# 재사용 가능한 OpenGL framework

Windows x64 · C++17 · OpenGL 3.3. `framework` 폴더 전체를 프로젝트에 복사한 뒤 다음 헤더를 include한다.

```cpp
#include "framework/Framework.h"
```

구현은 inline 헤더에 있다. framework의 .cpp 등록, Framework.props 가져오기, 외부 SDK의 include/LIB 경로 지정은 필요하지 않다. 기본 셰이더도 헤더에 포함된다. C++17 이상과 UTF-8 소스 해석(`/utf-8`)은 프로젝트에서 선택한다. Framework.props는 이 설정을 위한 선택적 편의 파일이다.

GLFW와 Assimp의 헤더·DLL·라이선스는 dependencies 안에 포함된다. GLFW는 창 생성 시, Assimp는 모델 모드를 켜고 loadModel을 호출할 때 동적으로 로드한다. GLEW 헤더는 OpenGL 선언에만 사용한다. Windows SDK, 그래픽 드라이버와 해당 DLL에 필요한 Microsoft VC 런타임은 실행 환경에 필요하다. 별도 Assimp 설치나 저장소 경로는 사용하지 않는다.

빌드와 실행 테스트는 수행하지 않았다. [사용자 확인 절차](docs/USER_CHECKS.md)를 따른다.

## 기본 도형과 클릭

```cpp
int main() {
    graphics::Application app;
    app.setInitHandler([](graphics::Application& app) {
        return app.getScene().addCuboid({1, 1, 1}, {0, 0, 0}).has_value();
    });
    app.setMouseHandler([](graphics::Application& app, const graphics::MouseEvent& event) {
        if (event.button == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS) {
            if (const auto hit = app.pickAt(event.windowX, event.windowY)) {
                app.getScene().splitFace(hit->entityId, hit->faceIndex);
            }
        }
    });
    app.setKeyHandler([](graphics::Application& app, int key, int action) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) app.requestClose();
    });
    return app.run();
}
```

기본 features는 `models=false`, `textures=false`다. addCuboid, addRightTetrahedron, addPyramid는 파일 없이 도형 메시를 만든다. [basic.cpp](examples/basic.cpp)는 부모 SRT와 클릭한 면의 추가 SRT를 보여 준다.

## 서로 다른 두 로더

`AssetLoader::loadPrimitive`는 기본 도형 텍스트를 읽는다. OBJ 형식의 v/vt/vn/f, 음수 인덱스, 삼각형·사각형·단순 평면 다각형을 처리한다. 기본적으로 같은 평면의 인접 삼각형을 논리적 면 메시로 병합한다. mtllib/usemtl 재질 처리는 이 모드에서 생략한다. 텍스처는 별도 상대경로 인자로 지정한다.

`AssetLoader::loadModel`은 Assimp로 일반 정적 모델을 읽는다. 노드의 변환을 로컬 정점에 반영하고, 각 메시의 인덱스·법선·색상과 선택적 UV·텍스처를 가져온다. 뼈대 애니메이션은 제공하지 않는다.

```cpp
graphics::ApplicationConfig config;
config.projectRoot = std::filesystem::current_path(); // 실제 프로젝트 루트로 지정한다.
config.features.models = true;   // loadModel을 사용할 때만 켠다.
config.features.textures = true; // UV와 이미지가 필요할 때만 켠다.

graphics::Application app;
app.setInitHandler([](graphics::Application& app) {
    graphics::LoadOptions options;
    options.pivot = graphics::PivotMode::BottomCenter;
    options.texturePath = "textures/override.png"; // 생략하면 모델의 재질 참조를 사용한다.
    const auto result = app.getAssets().loadModel("models/example.obj", options);
    if (!result) return false; // result.error에 원인이 들어 있다.
    return app.getScene().addModel(*result.model, graphics::Transform3D{},
                                  result.pivotMode, result.localBasis).has_value();
});
return app.run(config); // 사용자 main 안에서 호출한다.
```

기본 도형 파일에는 위의 모델 모드를 켤 필요가 없다. 호출만 다음과 같이 바꾼다.

```cpp
graphics::LoadOptions options;
options.outline = graphics::OutlineMode::FeatureEdges;
const auto result = app.getAssets().loadPrimitive("data/cube.txt", options);
```

models가 꺼진 loadModel, textures가 꺼진 loadTexture는 오류를 반환한다. textures가 꺼진 도형·모델 로드는 UV와 이미지 작업을 생략한다. 렌더링의 `getRenderOptions().textures`도 텍스처 사용을 제어한다. Application 초기화 때 features.textures에서 초기값을 가져온다.

모델·이미지 인자는 projectRoot 내부 상대경로여야 한다. 절대경로와 루트 밖으로 나가는 경로는 오류다. 모델이 참조하는 텍스처는 모델 폴더를 기준으로 찾고, 역시 projectRoot 안에 있어야 한다. `ApplicationConfig::runtimeDirectory`는 자산 루트와 다르다. 필요할 때 번들 framework 폴더의 실제 경로를 지정한다. 보통 실행 파일·작업 폴더의 상위 경로에서 framework를 찾으므로 생략할 수 있다. 배포할 때도 완전한 framework 폴더를 실행 파일 옆에 둔다.

[assets.cpp](examples/assets.cpp)는 primitive/model과 상대경로를 인자로 받는 예제다. [cube.txt](examples/data/cube.txt)는 기존 sample0 파일의 복사본이다.

## 로컬 기준점과 부모·면 SRT

`Transform3D::basis`는 월드 위치 또는 면의 추가 이동량이다. `localBasis`는 로컬 기준점이다. 변환은 열 벡터 기준으로 다음 순서다.

```text
부모 M = T(basis) * Rx * Ry * Rz * Quaternion * S * T(-localBasis)
분리 면 M = 부모 M * T(면 localBasis) * 면 SRT 행렬 * localAnimationMatrix
```

addModel의 기본 PivotMode::BottomCenter는 원본 로컬 AABB의 하단 중점을 localBasis로 설정한다. 정점을 강제로 변경하지 않는다. 따라서 부모 basis가 모델의 바닥 중점 위치가 된다. Preserve는 제공한 변환의 기준점을 유지한다. Custom은 인자로 전달한 기준점을 사용한다. setLocalBasis로 변경할 수 있다. BottomCenter 상태에서 메시를 수정하면 새 AABB에 맞춰 기준점을 다시 계산한다.

splitFace는 해당 면에 추가 SRT를 허용한다. 기본 추가 SRT는 항등 변환이므로 분리 자체로 위치가 바뀌지 않는다. 부모 SRT는 계속 적용된다. 분리 후 부모를 움직여도 면은 부모를 따른다. 면 중심 회전은 child.localBasis에 면의 로컬 중심을 넣고 setFaceSRT를 호출한다. child.basis에는 원본 모델 로컬 축 기준 추가 이동을 넣는다. 독립 월드 객체를 생성하는 기능은 아니다.

## 편집 API

Model은 기존 Polyhedron, Mesh는 기존 PlaneFigure의 별칭이다. 모델은 faces/meshes 목록을 갖고, 각 메시에는 임의 개수의 localVertices와 triangleIndices가 있다. Mesh::fromVertices로 일반 정점을 만들 수 있다.

| 목적 | API |
| --- | --- |
| 모델·메시 교체 | addModel, setModel, setMesh, getMesh |
| 정점과 삼각형 | setMeshVertices, setVertexPosition, setTriangleIndices |
| UV와 법선 | setTexcoords, setVertexNormals |
| 재질과 이미지 | setMaterial, AssetLoader::loadTexture |
| 기준점과 표시 | setLocalBasis, setVisible, setFaceVisible |
| 부모·면 변환 | setTransform, splitFace, setFaceSRT |
| 경계선 | setOutline |

getMesh/getComponent는 읽기용 포인터다. 장면 수정 후에는 다시 조회한다. setter는 잘못된 인덱스·속성 개수·비유한 값 등에 false를 반환한다. 검증이 실패하면 기존 메시를 유지한다. 편집 후 GPU 버퍼는 다음 render에서 갱신한다. setSideLengths는 기존 정점을 비례 조정하며 UV·재질·토폴로지를 유지한다. 길이가 0인 축을 가진 평면에는 사용하지 않는다.

```cpp
graphics::Material material;
const auto texture = app.getAssets().loadTexture("textures/example.png");
if (texture) {
    material.texture = texture.image;
    material.useTexture = true;
    app.getScene().setMaterial(id, meshIndex, material); // 메시 UV가 필요하다.
}
```

TextureImage에는 width/height와 RGBA8 데이터가 있다. 직접 만든 이미지도 shared_ptr<const TextureImage>로 재질에 지정할 수 있다. 공유 후 데이터를 변경하지 않는다. 변경하려면 새 이미지와 setMaterial을 사용한다. 파일·압축 임베디드 이미지는 Windows WIC로 읽는다. 지원하지 않는 형식은 오류를 반환한다. flipTextureRows와 Material::repeatTexture로 방향·반복을 지정한다.

OutlineMode::None은 경계를 끈다. FeatureEdges는 공면 내부 대각선을 제외하고 열린 경계·접힌 경계를 표시한다. AllTriangleEdges는 모든 삼각형 변을 표시한다. Explicit는 직접 전달한 인덱스 쌍을 사용한다. UV·법선 때문에 분리된 같은 좌표의 정점은 경계 계산에서 연결한다. 선택적 조명은 RenderOptions::lighting으로 켠다.

## 피킹과 수명

Application::pickAt은 GLFW 창 좌표를 실제 프레임버퍼 좌표로 바꾼다. pickAtCursor는 현재 커서를 사용한다. 직접 사용하려면 screenRay 또는 pick(scene, camera, x, y, viewport)를 호출한다. viewport는 좌상단 원점의 프레임버퍼 픽셀 영역이다.

PickResult는 가장 가까운 표시된 삼각형의 entityId/faceIndex/triangleIndex, 월드·로컬 교점, 무게중심 좌표와 선택적 UV를 반환한다. 원근·직교 카메라와 부모·면 변환을 사용한다. 선·축과 텍스처 투명 픽셀은 별도 판정하지 않는다.

Application은 한 스레드에서 한 실행 루프를 소유한다. 콜백은 컨텍스트 초기화 후 호출된다. getAssets는 run의 콜백 동안 유효하다. 콜백에 캡처한 객체는 run 종료까지 유지한다. 콜백 예외는 C 콜백 경계를 통과하지 않고, GPU·창 정리 후 다시 전달된다.

Renderer만 사용하면 호출자가 OpenGL 3.3 컨텍스트를 current로 유지한다. initialize → render → destroy 순서를 사용하고 컨텍스트를 마지막에 종료한다. Scene::clear 이후 ID는 재사용되므로 기존 ID를 유지하지 않는다.

## 검토 자료

- [전체 변경 전후](docs/CHANGES.md)
- [정적 검토와 번들 출처](docs/STATIC_REVIEW.md)
- [사용자가 수행할 빌드·실행 확인](docs/USER_CHECKS.md)
- [framework 전용 코드 구조 분석](../framework-analysis/graphify-out/GRAPH_REPORT.md), [framework 실행 흐름](../framework-analysis/graphify-out/FRAMEWORK_FLOW.md)
- [인터랙티브 코드 그래프](../framework-analysis/graphify-out/graph.html)

참고 자료: [ComputerGraphics](https://github.com/maru3172/ComputerGraphics)의 컴그_29번_텍스처매핑과 컴그_파이프라인1차. 정점·UV·법선 입력과 모델/뷰/투영·재질 흐름을 참고했다. 프레임워크의 자원 수명, 일반 인덱스 메시와 피킹은 별도 구현이다.

## 엔티티 계층 SRT

`Scene::setParent(child, parent)`로 계층을 연결하고 `setParent(child, std::nullopt)`로 해제한다. ID 오류·자기 연결·순환 연결은 false를 반환하며 기존 연결을 유지한다. 부모 연결·해제 시 local SRT를 유지하므로 월드 위치는 바뀔 수 있다.

`worldMatrix(id)`는 부모 world × 자식 local 행렬을 반환한다. `faceWorldMatrix(id, meshIndex)`는 면 변환까지 포함한다. `setTransform`과 `setPosition`은 부모가 있는 경우 부모 좌표의 값을 설정한다. 부모가 없으면 기존 월드 기준 동작이다. 부모 회전·비균일 크기·피벗은 자식에게 전달된다. 연속된 회전과 비균일 크기는 월드 공간에서 전단을 만들 수 있으므로 행렬을 SRT로 다시 분해하지 않는다.

입력 local 정점 → 조상 SRT 누적 → 물체·면 변환 → view/projection → 화면이다. 렌더링, 법선 변환, 피킹, owner가 지정된 선·축은 같은 계층 행렬을 사용한다. 부모 visible=false는 부모 자체만 숨긴다. 자식의 표시 상태는 독립적이다. 변환만 변경하면 GPU 정점 버퍼를 다시 업로드하지 않는다.

## UV 자동 생성과 조정

`Scene::generateTexcoords(id, meshIndex, options)`와 독립 메시용 `graphics::generateTexcoords(mesh, options)`를 제공한다. 기존 UV를 교체하는 명시적 작업이다. 재질을 활성화하기 전에 호출한다.

- Planar: axis=0/1/2에 해당하는 축을 버리고 메시 경계 범위를 0~1로 정규화한다. 기본 axis=2는 XY 투영이다.
- Box: 삼각형 법선의 가장 큰 축을 버리고 투영한다. 면 방향마다 정점을 분리한다. 텍스처 아틀라스 배치는 제공하지 않는다.
- Cylindrical: Y축 둘레 atan2로 U, 높이로 V를 계산한다. 원통 측면용이다. 뚜껑은 별도 메시에서 Planar로 매핑한다.
- Spherical: 메시 경계 상자의 중심을 기준으로 경도 atan2와 위도 asin을 계산한다. 구면에 적합하며 임의 모델을 왜곡 없이 펼치는 알고리즘은 아니다.

원통·구면의 U 경계는 정점을 분리하며 일부 U가 1을 넘을 수 있다. `Material::repeatTexture=true`를 사용한다. Box·Cylindrical·Spherical은 삼각형별 정점으로 바꾸므로 정점 번호와 삼각형 번호를 별도로 저장한 호출부는 다시 조회한다. 색상·법선·재질·면 SRT·명시적 외곽선 위치는 유지한다.

`UVOptions::scale`, `offset`, `rotationDegrees`는 원점 기준으로 UV 크기 → 회전 → 이동을 적용한다. 기존 UV만 조정하려면 `Scene::transformTexcoords` 또는 `graphics::transformTexcoords`를 사용한다. 기본 도형은 자동으로 UV를 갖지 않는다. `generateTexcoords` → `setMaterial` → textures 렌더 옵션 순서로 연결한다.

[examples/hierarchy.cpp](examples/hierarchy.cpp)는 기존 직육면체를 태양·지구·달의 자리로 사용한다. 태양 회전은 지구 공전을, 지구 회전은 달 공전을 만든다. 실제 천체 크기나 독립 자전·공전 구조의 구현은 아니다. 메모리 체크무늬 이미지는 추가 파일 없이 UV 반복을 보여준다. 파일 이미지는 기존 `AssetLoader::loadTexture` 결과를 material.texture에 지정한다.

## 로더 구현 분리

공개 진입점은 `include/ModelLoader.h`의 AssetLoader다. 기본 도형·OBJ 부분집합 파서는 `detail/PrimitiveLoader.inl`, 일반 정적 모델·재질·이미지 로딩은 `detail/AssimpLoader.inl`에 있다. 사용자는 .inl을 직접 포함하지 않는다. loadPrimitive와 loadModel의 활성화 조건·오류·경로 제한은 유지한다. 일반 모델의 지원 형식은 포함된 Assimp DLL의 지원 범위에 따른다. 가져온 노드 변환은 정점에 반영되며 노드 계층·뼈대 애니메이션은 보존하지 않는다.

참고: [Khronos sampler](https://wikis.khronos.org/opengl/Sampler_%28GLSL%29), [Khronos sampler wrap](https://wikis.khronos.org/opengl/Sampler_Object). 기존 UV를 보간해 샘플링하는 경로는 유지하고, CPU에서 사용할 투영 UV 생성 기능을 추가했다.
