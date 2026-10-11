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
- [코드 구조 분석](graphify-out/GRAPH_REPORT.md), [실행 흐름과 프로젝트 범위](graphify-out/EXECUTION_FLOW.md)
- [인터랙티브 코드 그래프](graphify-out/graph.html)

참고 자료: [ComputerGraphics](https://github.com/maru3172/ComputerGraphics)의 컴그_29번_텍스처매핑과 컴그_파이프라인1차. 정점·UV·법선 입력과 모델/뷰/투영·재질 흐름을 참고했다. 프레임워크의 자원 수명, 일반 인덱스 메시와 피킹은 별도 구현이다.
