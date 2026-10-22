# 재사용 가능한 OpenGL 프레임워크 사용설명서

이 프레임워크는 Windows x64, C++17, OpenGL 3.3 Core Profile용이다. 창·입력·렌더링·자산 로드를 공통 코드로 제공한다. 응용 프로그램은 장면을 만들고 입력에 따라 상태를 바꾼다.

- 처음 사용: 1~4절을 읽고 최소 예제로 시작한다.
- 변환을 구현: 5~7절을 읽는다.
- 모델·텍스처·선택을 구현: 8~10절을 읽는다.
- 실제 실습을 응용: [응용 패턴 설명서](docs/APPLICATION_PATTERNS.md)를 읽는다.
- 직접 실행 확인: [사용자 확인 항목](docs/USER_CHECKS.md)을 참고한다.

문서 기준은 2026-10-20의 로컬 소스다. 설명은 소스 읽기에 근거한다. 빌드·실행·화면 검증 결과를 뜻하지 않는다. 실습 13~19는 각각 독립 실행 프로그램이며 자체 framework 사본을 사용한다. 이 문서는 저장소 루트 framework를 기준으로 한다.

## 1. 폴더와 책임

| 경로 | 책임 | 읽는 시점 |
|---|---|---|
| [Framework.h](Framework.h) | 공개 진입 헤더. Application.h를 포함한다 | 응용 프로그램에서 include |
| [Framework.props](Framework.props) | include 경로, /utf-8, x64 제한, DLL·자산 배치 | Visual Studio 프로젝트 설정 |
| [include/Application.h](include/Application.h) | ApplicationConfig와 콜백 인터페이스 | 프로그램 골격 작성 |
| [include/Model.h](include/Model.h) | Transform3D, Model, Mesh, Material, ID | 데이터·수명 이해 |
| [include/Graphics.h](include/Graphics.h) | Scene, DiagramSystem, Camera, Renderer | 장면 생성·변환 |
| [include/Geometry.h](include/Geometry.h) | UV, 법선, 경계선 계산 | 메시 가공 |
| [include/ModelLoader.h](include/ModelLoader.h) | FeatureOptions, LoadOptions, AssetLoader | 외부 자산 읽기 |
| [include/Texture.h](include/Texture.h) | 이미지 디코딩 | 텍스처 오류 추적 |
| [include/Picking.h](include/Picking.h) | 화면 광선과 삼각형 교차 판정 | 클릭 선택 |
| detail/*.inl | 공개 API의 inline 구현 | 내부 동작 추적 |
| detail/Native.h | GLFW·OpenGL 함수 로딩과 실행 경로 처리 | 초기화·배포 오류 추적 |
| detail/Shaders.h, shaders/*.glsl | 내장 셰이더와 외부 셰이더 파일 | GPU 입력·출력 확인 |
| examples/basic.cpp, examples/assets.cpp | 면 선택과 자산 로드 예제 | 기존 코드 재사용 |
| dependencies/ | 배포에 포함된 라이브러리·헤더·라이선스 | 폴더 전체 복사 |

프레임워크는 헤더와 inline 구현으로 구성된다. detail의 파일을 별도 .cpp처럼 프로젝트에 추가할 필요가 없다. 응용 프로그램에서 detail 함수를 직접 쓰기보다 공개 API를 사용한다.

## 2. 새 프로젝트에 연결

1. 프로젝트 폴더 아래에 framework 폴더 전체를 복사한다. dependencies도 포함한다.
2. Visual Studio에서 C++17 이상, x64를 선택한다.
3. 속성 관리자에서 해당 구성에 framework/Framework.props를 추가한다. Debug와 Release 양쪽에 적용한다.
4. main.cpp에서 `#include "framework/Framework.h"`를 사용한다.
5. 실행할 main은 하나만 등록한다. examples/basic.cpp와 assets.cpp에도 각각 main이 있다.

현재 Framework.props는 include 경로와 배포를 설정한다. 새 Application 경로는 Native.h에서 필요한 함수를 로딩한다. 기존 직접 GLEW/GLFW 호출 방식에 사용하는 LegacyOpenGL.props와 용도를 구분한다. 별도 SDK 경로를 하드코딩하지 않는다.

Windows SDK, OpenGL 3.3을 지원하는 그래픽 드라이버, 필요한 VC 런타임은 운영체제 실행 환경이다. framework 폴더가 이 환경까지 설치하지는 않는다.

### 최소 사용 예제

아래 코드는 새 main.cpp에 넣는 사용 예제다. 이 문서 작성 중 컴파일하지 않았다.

```cpp
#include "framework/Framework.h"

int main() {
    graphics::Application app;
    std::optional<graphics::EntityId> cube;
    float angleDegrees = 0.0f;

    app.setInitHandler([&](graphics::Application& current) {
        cube = current.getScene().addCuboid({1, 1, 1}, {0, 0, 0});
        return cube.has_value();
    });
    app.setUpdateHandler([&](graphics::Application& current, float dt) {
        angleDegrees += 45.0f * dt;
        graphics::Transform3D transform;
        transform.rotation = glm::mat3(glm::rotate(
            glm::mat4(1.0f), glm::radians(angleDegrees), glm::vec3(0, 1, 0)));
        if (!current.getScene().setTransform(*cube, transform))
            current.requestClose();
    });
    app.setKeyHandler([](graphics::Application& current, int key, int action) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            current.requestClose();
    });

    graphics::ApplicationConfig config;
    config.title = "Framework example";
    return app.run(config);
}
```

입력은 한 변이 1인 입방체와 경과 시간 dt, 상태는 Y 회전각, 출력은 초당 45도로 회전하는 입방체다. cube는 초기화 성공 후 업데이트에서 사용한다. 캡처한 변수는 run 종료까지 유지한다. init이 false면 업데이트 루프에 진입하지 않는다.

## 3. Application의 실행 순서

구현: [detail/Application.inl](detail/Application.inl). set...Handler는 함수를 등록한다. 등록하는 자리에서 함수 본문을 실행하는 것은 아니다.

```text
main: Application·응용 상태 생성 → 콜백 등록 → run(config)
  run: 콘솔 출력을 UTF-8로 설정
    initialize: 실행 기준 경로 → GLFW → 창·컨텍스트
                → Renderer 초기화 → AssetLoader 생성 → GLFW 콜백 등록
    사용자 init 콜백
    반복:
      PollEvents → 입력·포커스 등의 콜백 실행
      종료 요청이면 반복 종료
      프레임버퍼 크기가 0이면 WaitEvents 후 시간 갱신, 다음 반복으로 이동
      dt 계산 → update → 종료 요청 확인
      render 콜백, 미등록이면 renderScene → SwapBuffers
    shutdown: AssetLoader 해제 → Renderer 파괴 → 창 파괴 → GLFW 종료
  main: run의 결과 반환
```

| 등록 API | 인자와 반환값 | 용도·주의 |
|---|---|---|
| setInitHandler | Application& → bool | 초기화 후 한 번, 도형과 상태 생성. false면 실패 종료 |
| setUpdateHandler | Application&, float dt → void | 매 프레임 위치·각도·진행률 갱신 |
| setKeyHandler | Application&, key, action → void | 수정키가 필요 없는 키 입력 |
| setModifiedKeyHandler | Application&, key, action, mods → void | Shift/Caps Lock을 포함한 입력. 등록하면 setKeyHandler 대신 실행 |
| setFocusHandler | Application&, bool focused → void | 포커스 상실 시 누름 상태 해제 |
| setMouseHandler | Application&, const MouseEvent& → void | 버튼, action, mods, 창 좌표·픽셀 좌표 |
| setCursorHandler | Application&, double x, double y → void | 창 좌표로 전달하는 커서 이동 |
| setRenderHandler | Application& → bool | 기본 렌더링을 대체. 기본 장면도 필요하면 직접 renderScene 호출 |

- dt의 단위는 초다. 계산값을 0~0.05초로 제한한다. 긴 정지 시간을 그대로 이동량으로 사용하지 않는다. 실제 시간의 완전한 재현을 보장하는 시계는 아니다.
- 최소화 등으로 그리기 영역이 0인 동안 update와 렌더링을 건너뛴다. 복귀 직후 대기 시간 전체를 dt에 넣지 않는다.
- requestClose는 종료 플래그를 설정한다. 호출한 C++ 함수에서 즉시 return하는 명령은 아니다.
- run은 정상 종료 시 0, 초기화·사용자 init·렌더링 실패나 중복 run 거부 시 -1을 반환한다. 예외는 자원을 정리한 뒤 다시 던진다. 모든 실패가 -1이 되는 것은 아니다.
- GLFW 입력 콜백의 예외는 dispatch가 저장하고 이벤트 처리 후 다시 던진다. C 콜백 경계를 직접 넘어가지 않게 한다.
- 한 스레드에서 Application 하나를 실행한다. 콜백에서 run을 재귀 호출하지 않는다.

## 4. Scene과 도형 생성

Scene은 CPU 측 도형·변환·선을 보관한다. EntityId는 도형이나 그룹, faceIndex는 해당 모델 안의 면 또는 메시를 지정한다.

| API | 목적 | 반환값·주의 |
|---|---|---|
| addCuboid(size, position) | 직육면체 | optional<EntityId>. size는 전체 변 길이 |
| addPyramid(size, position) | 사각뿔 | size.y는 전체 높이 |
| addRightTetrahedron(edgeLengths, position) | 직교하는 세 변을 갖는 사면체 | 정사면체 전용 API가 아님 |
| addSphere(radius, position, slices, stacks) | 원점 중심 구 | slices 기본 32, stacks 기본 16 |
| addCone(radius, height, position, slices) | Y축 방향의 원점 중심 원뿔 | slices 기본 32 |
| addGroup(transform) | 그리지 않는 변환 노드 | 자식과 선의 기준으로 사용 |
| push_entity(model, transform) | 만든 Model을 장면에 등록 | 지정 transform 사용 |
| addModel(model, transform, pivot, customPivot) | 피벗을 지정하여 등록 | pivot 기본값은 BottomCenter |

구·원뿔의 반지름과 높이, 기본 다면체의 변 길이는 양수다. 구는 slices 3~512, stacks 2~256, 원뿔은 slices 3~512다. 값이 잘못되면 생성 함수는 빈 Model, 등록 함수는 빈 optional을 반환한다.

색이나 면을 먼저 편집하려면 DiagramSystem::makePolyhedron/makeSphere/makeCone으로 Model을 얻고, 편집한 뒤 등록한다. Model과 Polyhedron은 같은 타입이며 Mesh와 PlaneFigure도 같은 타입의 별명이다. 가져온 모델의 faceIndex는 보통 메시 번호다. 기하학적으로 평평한 면 하나나 삼각형 하나와 같다고 가정하지 않는다.

### 기본 도형의 면 번호

| 도형 | faceIndex와 방향 |
|---|---|
| Cuboid | 0:앞+Z, 1:뒤-Z, 2:왼쪽-X, 3:오른쪽+X, 4:위+Y, 5:아래-Y |
| Pyramid | 0:앞, 1:오른쪽, 2:뒤, 3:왼쪽, 4:바닥 |

번호는 생성 시의 로컬 좌표를 기준으로 한다. 회전 후 화면 오른쪽에 와도 면 번호는 변하지 않는다. 가져온 Model에는 이 대응표를 그대로 적용하지 않는다.

## 5. Transform3D와 좌표

정의: [include/Model.h](include/Model.h). GLM의 열벡터 형식으로 행렬을 사용한다.

| 멤버 | 의미 | 초기값 |
|---|---|---|
| basis | 로컬 기준점을 놓을 부모 좌표의 위치 | (0,0,0) |
| localBasis | 원래 모델 좌표 안의 피벗 | (0,0,0) |
| rotation | 회전 mat3 | 항등행렬 |
| scale | 축별 크기 배율 | (1,1,1) |

```text
M_local = T(basis) × R(rotation) × S(scale) × T(-localBasis)
p_parent = M_local × p_model
M_world = M_parentWorld × M_local
p_clip = Projection × View × M_world × p_model
```

오른쪽부터 적용한다. 모델의 피벗을 원점으로 옮기고, 크기를 바꾸고, 회전한 뒤 부모 좌표의 basis에 놓는다. localBasis 자체를 입력하면 basis로 변환된다.

- 부모가 없으면 basis는 월드 위치, 부모가 있으면 부모 좌표의 위치다. setPosition의 인자 이름 worldPosition만 보고 월드 좌표라고 판단하지 않는다.
- setTransform은 변환 전체를 교체한다. 새 Transform3D를 매번 만들면 이전 위치나 크기는 자동으로 남지 않는다. 응용 코드에서 필요한 상태를 보관한다.
- rotation은 Euler 각도나 쿼터니언이 아니다. 이전 rotationDegrees/orientation을 사용하는 코드는 현재 API와 다르다.
- rotation의 열은 기저 방향이다. 회전행렬의 직교성과 행렬식 약 1을 검사한다. scale을 rotation에 섞지 않는다.
- `rotation = rotation * deltaRotation`은 현재 로컬 축의 추가 회전이다. 왼쪽에 곱하면 부모 좌표의 축으로 추가 회전한다. 부모가 회전해 있으면 부모 축과 월드 축은 다르다.
- 초기 입체 표시는 카메라만으로 만드는 방법과 모델을 기울이는 방법이 있다. 실습 13은 모델에도 초기 회전을 둔다. 실습 14의 독립된 카메라·입력 회전과 혼동하지 않는다.

### 피벗 지정의 차이

addCuboid 등과 push_entity는 지정한 변환의 localBasis를 사용한다. addModel은 기본으로 BottomCenter를 사용하므로 같은 형상도 배치가 달라질 수 있다.

- Preserve: 전달한 transform.localBasis를 유지한다.
- BottomCenter: Model 경계 상자의 아래쪽 중심을 localBasis로 설정한다.
- Custom: customPivot을 localBasis로 설정한다.

모델 중심을 원점으로 조립하려면 addModel에 PivotMode::Preserve를 명시한다. setModel은 BottomCenter를 사용하는 경우 새 모델에서 피벗을 다시 계산한다. 형상 교체 후 중심을 유지해야 한다면 이 차이를 확인한다.

## 6. 부모·자식 계층과 면의 추가 변환

### 부모·자식 계층

```cpp
auto orbit = scene.addGroup();
auto body = scene.addSphere(0.3f, {2, 0, 0});
if (!orbit || !body || !scene.setParent(*body, *orbit))
    return false; // bool 초기화 함수 안에서 사용하는 코드 조각
```

orbit을 회전하면 body의 위치도 회전한다. body만 회전하면 자기 로컬 원점 주위로 자전한다.

setParent는 잘못된 ID, 자기 자신, 순환 연결을 거부한다. 연결·해제 시 자식의 로컬 변환을 유지하며 월드 위치 보정은 하지 않는다. 해제는 setParent(id, std::nullopt)다. worldMatrix로 최종 합성 행렬을 얻는다.

부모의 확대축소는 자식의 배치에도 적용된다. 태양의 표시 크기를 행성 궤도에 전달하지 않으려면 태양 표시 노드와 궤도 노드를 같은 부모 아래에 분리한다. tint와 visible은 자식에게 일괄 전파되는 기능이 아니다. 표시 대상에 각각 설정한다.

### 면의 추가 변환

| API | 용도 |
|---|---|
| setFaceLocalMatrix(id, face, matrix) | 면의 추가 애니메이션 행렬을 교체 |
| splitFace(id, face) | 면의 자식 로컬 SRT 활성화 |
| setFaceSRT(id, face, transform) | 자식 로컬 SRT 설정과 활성화 |
| setFaceTransform(id, face, transform, independence) | 자식 SRT와 활성 여부 설정 |
| faceWorldMatrix(id, face) | 조상·본체·면을 포함한 행렬 조회 |

```text
C_face = identity                              (자식 SRT 비활성)
C_face = T(face.localBasis) × M(face.transform) (자식 SRT 활성)
M_faceWorld = M_parentWorld × M_object × C_face × localAnimationMatrix
```

면 SRT 바깥에 T(localBasis)가 있으므로 basis=0, rotation=I, scale=1이면 면을 움직이지 않고 피벗만 지정할 수 있다. 면의 추가 basis는 이동량으로 쓴다. 본체 Transform3D의 basis 의미와 구분한다.

splitFace는 새 EntityId나 정점 배열을 만들지 않는다. independence는 호환용 이름이며 본체에서 월드 공간으로 분리한다는 의미가 아니다. 면은 본체와 조상의 변환을 계속 받는다. setFaceLocalMatrix는 splitFace 없이도 사용한다.

## 7. 렌더링·색·카메라

### CPU에서 GPU로 가는 흐름

```text
Scene의 면·선 → Renderer::upload → VAO/VBO/EBO
  → faceWorldMatrix / 선 owner의 worldMatrix
  → uMVP = Projection × View × Model
  → 정점 셰이더 → 래스터화 → 프래그먼트 셰이더
  → 깊이 처리 → 프레임버퍼 → SwapBuffers
```

정점 속성은 location 0=위치, 1=색, 2=UV, 3=법선이다. 정점색 또는 면색에 Material.color를 곱해 업로드하고, 그릴 때 엔티티의 tint를 곱한다. 텍스처를 사용하면 이미지 색도 곱한다. lighting은 법선행렬과 lightDirection을 사용해 환경광에 해당하는 0.25와 확산항 0.75를 합성한다. 기본 렌더러는 블렌딩을 끄므로 이미지 알파만으로 반투명 합성을 기대하지 않는다.

형상이나 UV 등의 변경은 geometryRevision을 갱신하여 다음 렌더링에서 재업로드하는 계기가 된다. 변환, 표시 여부, setTint는 기존 버퍼를 사용한다. 단순한 이동이나 색 배율 애니메이션에 매번 setModel을 호출할 필요는 없다. Renderer는 장면·revision·textures 모드가 바뀌면 업로드한다.

### RenderOptions

| 항목 | 기본값 | 의미 |
|---|---|---|
| depthTest | true | 깊이로 앞뒤 판정 |
| wireframe | false | 면을 선으로 표시 |
| textures | false | GPU 텍스처 사용 |
| lighting | false | 법선에 따른 밝기 사용 |
| lightDirection / lightColor | (1,1,1) / 흰색 | 방향과 색. 방향은 0 벡터가 아니어야 함 |
| outlineColor | 어두운 회색 | 솔리드 표시의 경계선 색 |
| background | (0.12,0.12,0.16,1) | 화면 지우기 색 |

wireframe과 depthTest를 모두 켜면 전체 대상 면의 깊이를 먼저 기록하여 뒤의 선을 숨긴다. 와이어 색은 정점색과 tint를 사용한다. 솔리드의 경계선은 outlineColor를 사용한다.

setOutline은 None, FeatureEdges, AllTriangleEdges, Explicit 중에서 선택한다. 사각형 내부의 삼각형 대각선을 숨기려면 FeatureEdges, 삼각형 분할을 보려면 AllTriangleEdges를 쓴다. Explicit의 인덱스는 메시 내부 정점 번호를 2개씩 짝짓는다. None이어도 wireframe에서는 기본 특징선을 사용한다.

### Camera

position은 시점, target은 주시점, up은 위쪽 방향이다. orthographic=false면 원근 투영, true면 직교 투영이다. fieldOfViewDegrees는 도 단위다. nearPlane은 양수, farPlane은 nearPlane보다 크게 설정한다.

직교 투영의 orthographicHalfExtent는 화면에서 짧은 축의 절반 길이다. 가로가 길면 가로 범위를 종횡비만큼 확장한다. 창 크기가 바뀔 때 독자적인 고정 aspect를 덮어쓸 필요는 없다.

CameraSystem::validCamera는 시선이 0이 아니고 up과 평행하지 않은지, 투영값이 유효한지 등을 확인한다. 멤버를 직접 바꾸는 API이므로 복잡한 조작은 카메라 복사본에 적용하고 검사 후 반영한다. 잘못된 카메라는 렌더링 실패로 이어진다.

## 8. 형상 편집·선·UV

| API | 바꾸는 대상 | 주의 |
|---|---|---|
| setModel / setMesh | 모델 전체 / 메시 | 모델 교체는 부모·본체 상태를 유지하나 면 내부 설정은 새 모델 사용 |
| setMeshVertices / setVertexPosition | 정점 속성·위치 | 유효한 삼각형과 속성 대응 필요 |
| setTriangleIndices | 삼각형 정점 번호 | 3개 단위. 범위 밖 번호와 면적 0인 삼각형을 피함 |
| setFaceVertexColors | 면 내부 정점색 | 빈 배열 또는 정점 수와 같은 크기 |
| setMaterial / setVertexNormals | 재질 / 정점 법선 | 배열·이미지·UV의 관계 유지 |
| setTexcoords / generateTexcoords / transformTexcoords | UV | 정점과 대응 유지 |
| addLine / addVector / addBasis | 보조선 / 벡터 / 기저 3개 | owner가 있으면 해당 좌표계에 배치 |
| setVector | 기존 선의 시작점과 벡터 | 끝점은 origin+vector |

generateTexcoords는 기존 UV를 투영 결과로 교체한다. Planar, Box, Cylindrical, Spherical을 지원한다. 최적 UV 전개나 아틀라스 생성 기능은 아니다. UVOptions.axis는 평면 투영에서 제외하는 축(X=0, Y=1, Z=2)이다. transformTexcoords는 확대축소→회전→오프셋 순서다. 회전은 도 단위이며 UV 원점을 중심으로 한다.

addBasis의 mat3는 각 열이 X/Y/Z 기저다. 정규화하지 않고 사용하므로 기저 길이와 axisLengths가 모두 표시 길이에 영향을 준다. 선의 owner를 생략하면 월드 공간, 지정하면 owner의 로컬 공간에 둔다. 선은 owner의 tint를 상속하지 않는다.

## 9. 모델·텍스처와 상대 경로

### 사용할 기능을 명시

OBJ 로드는 항상 사용할 수 있다. FeatureOptions.textures의 기본값은 false이며 이미지 읽기와 텍스처 사용에 관여한다. Application 초기화 때 config.features.textures를 RenderOptions.textures에 반영한다.

```cpp
// run 전에 설정한다.
graphics::ApplicationConfig config;
config.features.textures = true;

// 아래는 setInitHandler 본문의 코드 조각이다. app은 콜백 인자다.
graphics::LoadOptions options;
options.pivot = graphics::PivotMode::Preserve;
const auto loaded = app.getAssets().loadObj("models/example.obj", options);
if (!loaded) {
    std::cerr << loaded.error << '\n'; // 이 코드는 <iostream>도 포함해야 한다.
    return false;
}
return app.getScene().addModel(*loaded.model, graphics::Transform3D{},
    loaded.pivotMode, loaded.localBasis).has_value();
```

설정과 초기화의 역할을 보여주는 코드 조각이다. 하나의 함수로 그대로 붙이는 예제는 아니다. 전체 예제는 [examples/assets.cpp](examples/assets.cpp)를 읽는다.

- loadObj: OBJ 텍스트 API. 표기 변형·음수 인덱스·평면 다각형과 실패 시 호환 재시도를 처리한다. loadPrimitive는 같은 API의 호환 함수다.
- loadModel: loadObj의 호환 함수다. OBJ 이외 형식은 지원하지 않는다.
- LoadOptions.texturePath: 모델의 이미지를 지정한 상대 경로 이미지로 덮어쓸 때 사용한다.
- loadTexture: TextureResult.image로 이미지를 얻는다. Material.texture와 useTexture를 설정할 때 UV도 준비한다.
- 실패하면 ModelResult.error 또는 TextureResult.error를 읽는다. getAssets는 run의 초기화 콜백부터 run 종료 전까지 사용할 수 있다.

### 실행 시의 기준 경로

| 설정 | 기본값 | 기준 |
|---|---|---|
| projectRoot | . | 실행 파일 디렉터리 |
| runtimeDirectory | framework | projectRoot 내부 |
| shaderDirectory | 빈 값 | 비어 있으면 내장 셰이더. 그 외에는 projectRoot 내부 상대 경로 |
| 로더의 인자 | 직접 지정 | projectRoot 내부 상대 경로 |

소스 프로젝트 폴더나 프로세스의 작업 폴더에서 자동 탐색하는 방식은 아니다. 절대 경로나 projectRoot 밖으로 나가는 경로는 거부한다.

Framework.props의 빌드 후 복사는 DLL과 라이선스를 실행 위치의 framework/dependencies 아래, 외부 셰이더를 framework/shaders 아래에 둔다. 프로젝트 바로 아래의 data/models/textures는 실행 위치의 같은 이름 폴더로 복사한다.

```text
실행 위치/
  app.exe
  framework/
    dependencies/   ← 원래 의존성 폴더 안의 상대 구조 유지
    shaders/        ← 외부 셰이더를 지정할 때 사용
  data/
  models/
  textures/
```

examples/assets.cpp 주석의 framework/examples/data/cube.txt는 그 경로에 실제로 배치한 경우의 예다. Framework.props는 examples 폴더를 자동 배포하지 않는다. 기본 복사 기능을 쓰려면 프로젝트의 data에 넣고 data/cube.txt로 지정한다.

shaderDirectory를 지정하면 그 안의 VertexShader.glsl과 FragmentShader.glsl을 사용한다. 지정하지 않았다면 외부 파일을 편집해도 내장 셰이더 동작은 바뀌지 않는다.

## 10. 마우스로 면 선택

```cpp
app.setMouseHandler([](graphics::Application& current,
                       const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
        return;
    if (const auto hit = current.pickAt(event.windowX, event.windowY)) {
        graphics::Transform3D face;
        face.basis = {0, 0, 0.2f};
        current.getScene().setFaceSRT(hit->entityId, hit->faceIndex, face);
    }
});
```

pickAt에는 왼쪽 위가 원점인 창 좌표를 넣는다. Application이 프레임버퍼의 픽셀 배율로 변환한다. MouseEvent.framebufferX/Y를 다시 pickAt에 넣으면 이중 변환이 된다. 하위 graphics::pick과 PickViewport에 직접 전달할 때는 프레임버퍼 좌표를 사용한다.

screenRay로 근·원 클립면의 점을 역변환하고 각 표시 면의 삼각형과 교차 판정한다. 가장 가까운 결과에는 entityId, faceIndex, triangleIndex, worldPoint, localPoint, barycentric, uv 등이 있다. 화면 광선의 distance는 근 클립면 위 광선 시작점부터의 거리이며 카메라부터의 거리와 같지는 않다.

피킹은 렌더링과 같은 faceWorldMatrix를 사용한다. CPU 삼각형 교차 판정이며 선, 텍스처의 투명 픽셀, 와이어의 선만 선택하는 기능은 아니다. 교차하지 않으면 빈 optional을 반환한다.

## 11. 수명과 오류 처리

- 생성 API의 optional과 setter의 bool을 확인한다. .value()나 *id는 성공이 확정된 뒤 사용한다.
- Scene::clear는 도형과 선을 비운다. ID를 재사용하므로 이전 ID를 상태에 남겨 계속 쓰지 않는다.
- getComponent/getMesh는 읽기 전용 포인터를 반환한다. 추가·모델 교체·clear 등으로 무효가 될 수 있다. Scene 변경을 넘어 보관하지 말고 ID로 다시 조회한다.
- addGroup은 빈 모델을 갖는 변환 노드다. 일반 addModel/push_entity는 빈 모델을 거부한다. setModel로 유효 모델을 넣은 그룹은 일반 도형이 된다. 그룹에는 BottomCenter를 사용할 수 없다.
- Renderer를 직접 사용하면 OpenGL 컨텍스트를 current로 만든 뒤 initialize/render/destroy를 호출한다. 컨텍스트를 먼저 파괴하지 않는다. 보통은 Application에 맡긴다.
- BOM은 소스 파일의 인코딩, /utf-8은 컴파일러의 해석과 문자열 인코딩, SetConsoleOutputCP(CP_UTF8)는 Windows 콘솔 출력의 해석을 다룬다. 서로 다른 단계다.

## 12. 사용자가 수행할 확인

이번 문서 업데이트에서는 빌드·테스트를 실행하지 않았다. 다음 확인은 사용자가 수행한다.

| 조작 | 예상 결과·관찰점 |
|---|---|
| Debug/Release x64 빌드 | include·링크·DLL 배치 오류가 없음 |
| 최소 예제 실행 | 입방체가 Y축으로 자전하고 Esc로 종료 |
| 실행 파일의 작업 폴더 변경 | 실행 위치의 의존성과 자산을 사용하여 실행 |
| 부모 회전과 자식 회전을 각각 변경 | 공전과 자전이 분리됨 |
| 면 변형 후 클릭 | 표시된 변형과 선택 결과가 대응 |
| 이동 중 포커스 이탈 | 응용의 onFocus에서 누름 상태 해제 |
| 최소화·복귀·창 크기 변경 | 렌더링 복귀와 투영 비율 확인 |
| 기능 활성화 후 자산 읽기 | 내부 상대 경로는 성공, 루트 밖 경로는 실패로 처리 |

화면이 보이지 않으면 초기화 오류→등록 ID→visible→변환·카메라→렌더링 결과 순서로 살펴본다. 텍스처만 보이지 않으면 features, RenderOptions, Material.useTexture, 이미지, UV 순서로 확인한다.

다음 문서: [구현된 응용 패턴](docs/APPLICATION_PATTERNS.md). 이전 변경 보고는 저장소의 framework-analysis/archive를 참고한다.

## 13. 변환·Motion·레이어 트리 (2026-10-22)

이 절은 공통 프레임워크에 추가한 API를 설명한다. 기존 실습의 Console 애니메이션은 유지한다. 새 API를 사용하지 않으면 Motion과 레이어는 항등변환이다. 카메라는 Scene 객체와 독립적이며 루트 레이어로 이동하지 않는다. 이 절과 예제는 빌드·실행하지 않았다.

### 13.1 구성과 프레임 순서

- `include/Transform.h`, `detail/Transform.inl`: 반사·회전·이동·크기 행렬 및 경로 함수.
- `include/Motion.h`, `detail/Motion.inl`: 시간에 따른 행렬 합성, 동작 식별자, 레이어 트리.
- `Scene::getMotions()`: 해당 장면의 MotionSystem 참조. `Framework.h` 또는 `Graphics.h`로 공개된다.
- `examples/motion.cpp`: 경로 선택, 공유 동작, 반사, 레이어 삽입·삭제, 클릭 피킹 예제. 다른 예제와 main이 중복되므로 하나만 프로젝트에 등록한다.

현재 프레임 순서는 `PollEvents -> 사용자 update 콜백 -> 종료 확인 -> Scene::updateMotions(dt) -> render 콜백/기본 렌더 -> SwapBuffers`다. Application::run 안에서는 updateMotions를 다시 호출하지 않는다. 콜백에서 만든 동작은 그 프레임의 dt만큼 진행된 뒤 렌더된다. run 밖에서 Scene과 Renderer를 직접 사용하면 호출자가 한 프레임에 한 번 updateMotions를 호출한다. 기존 dt의 0~0.05초 제한과 최소화 대기 처리는 유지한다. 실제 벽시계가 아니라 전달받은 dt의 누적 시간이다.

### 13.2 기본 행렬과 반사

```cpp
const auto rotate = graphics::TransformSystem::rotation(
    graphics::AxisLine{{1, 0, 0}, {0, 1, 0}}, 30.0);
const auto mirror = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
const auto pointSymmetry = graphics::TransformSystem::reflectPoint({1, 0, 0});
const auto lineSymmetry = graphics::TransformSystem::reflectLine(
    graphics::AxisLine{{0, 0, 0}, {0, 1, 0}});
```

반사는 회전 API와 분리한다. rotation은 축 위의 한 점과 축 방향을 받으며 양의 각도는 오른손 방향이다. 축 방향을 반대로 지정하면 같은 양의 각도도 반대 방향이다. 각도 단위는 도, 속도는 도/초다.

- `translation(offset)`: 이동 행렬.
- `scaling(factors, center)`: 지정 중심의 크기 변경. 음수와 0 배율도 허용한다.
- `rotation(axis, degrees)`: 지정 직선 주위의 회전.
- `reflectPlane(point, normal)`: 평면 양쪽의 거리를 뒤집는 평면 반사. determinant는 -1이다.
- `reflectPoint(center)`: 중심을 기준으로 세 좌표를 반전하는 점대칭. 3D에서는 determinant가 -1이다.
- `reflectLine(axis)`: 직선 방향 성분은 보존하고 수직 성분 둘을 뒤집는다. 3D에서 같은 직선 주위의 180도 회전과 수학적으로 같지만 별도 대칭 API로 제공한다.

기본식은 `T(center) * A * T(-center)`다. 평면 반사에서 A는 I-2nnᵀ, 직선 대칭에서는 2nnᵀ-I다. 방향 벡터는 내부에서 정규화한다. 영벡터·NaN·무한대나 계산 결과가 유효하지 않으면 optional이 비어 있다. 행렬은 유한한 아핀 행렬이어야 하며 투영 행렬은 허용하지 않는다.

기존 Transform3D.rotation은 순수 회전 조건을 유지한다. 반사 행렬을 이 필드에 억지로 넣지 않고 LayerConfig.transform 또는 Motion::fixed에 넣는다. Renderer는 면 컬링을 끄고 역전치 법선을 사용하며, 피킹은 양면 교차 검사와 같은 faceWorldMatrix를 사용한다. 0 배율로 붕괴한 면과 특이 행렬의 조명은 일반적인 입체 표면 결과를 보장하지 않는다.

### 13.3 경로 함수와 시간 보간

```cpp
graphics::PathMotion path;
path.path = graphics::PathSystem::ellipse({0, 0, 0}, {2, 0, 0}, {0, 0, 1});
path.interpolation = graphics::PathSystem::smoothStep;
path.durationSeconds = 4;
path.playback = graphics::Playback::Loop;
graphics::Motion motion;
if (!motion.append(graphics::Motion::followPath(path))) return false;
const auto id = scene.getMotions().createMotion(motion);
```

`PathFunction`은 `glm::vec3(double u)`, `InterpolationFunction`은 `double(double progress)` 형태의 std::function이다. 내부 흐름은 다음과 같다.

```text
q = phase + 경과초 / durationSeconds * rate
progress = Once의 clamp / Loop의 [0,1) 순환 / PingPong의 0↔1 왕복
u = interpolation(progress)
offset = path(u)
relativeToStart이면 offset -= path(interpolation(phase))
결과 = T(offset)
```

path의 반환값은 객체의 기존 위치를 직접 대입하는 절대 월드 좌표가 아니라 추가 이동량이다. 기존 배치가 원점이면 경로 좌표와 결과 위치가 일치한다. relativeToStart=true는 시작 위치를 보존하는 상대 이동에 사용한다. World/Local 공간에 따라 이 이동을 배치 앞/뒤에 적용한다. 객체가 경로 접선 방향을 자동으로 바라보지는 않는다. 방향 변화가 필요하면 별도 회전 Step 또는 사용자 행렬 함수를 합성한다.

| 설정 | 계약 |
|---|---|
| durationSeconds | 양의 유한값. rate=1일 때 경로 한 번의 진행 시간 |
| rate | 0이 아닌 유한값. 부호로 진행 방향 지정 |
| phase | 0~1의 시작 진행률. Once를 역방향으로 시작하려면 phase=1, rate<0 |
| playback | Once는 끝에서 유지, Loop는 순환, PingPong은 왕복 |
| relativeToStart | 시작 경로 위치를 차감할지 여부. 기본 false |
| space | World 또는 Local. 기본 World |

순환에서 열린 경로는 끝과 시작 사이에 점프한다. 보간 함수는 원래 시간 진행률 0~1을 받는다. 반환 u는 제한하지 않으므로 사용자 경로는 필요한 구간 밖 값도 처리해야 한다. 내장 polyline은 u를 0~1로 제한하고, ellipse는 주기 함수로 평가한다. 함수는 유한한 결과를 반환해야 한다.

PathSystem::ellipse(center, axisU, axisV)의 두 벡터는 반지름과 평면을 함께 정한다. 서로 평행하거나 길이가 0인 벡터는 거부한다. circle(center, radius, normal)은 법선에 수직인 원을 만든다. rectangle의 halfU/halfV는 수직인 반변 벡터다. polygon은 입력 순서대로 닫힌 경계를, polyline(points, closed=false)은 입력 순서대로 선분을 만든다. 연속 중복점·길이 0 선분은 거부한다. 닫힌 polygon은 마지막 점을 다시 적어도 된다.

polyline/polygon/rectangle은 선분 길이 누적으로 매개변수를 배분하므로 linear 보간에서 실제 이동 속도가 일정하다. 꼭짓점에서는 방향이 즉시 바뀐다. ellipse와 임의 사용자 경로는 매개변수 속도가 일정해도 실제 속도가 일정하지 않을 수 있다. smoothStep을 적용하면 선분 경로도 가감속한다. 일반 영역의 경계·내부 궤적은 사용자의 path 함수가 결정하며 영역만으로 경로를 자동 추론하지 않는다.

### 13.4 Motion 합성과 편의 API

Motion은 MotionStep의 목록이다. append/insert(index, step)/replace(index, step)/erase(index)로 편집하고 getSteps로 읽는다. 변경한 Motion을 setMotion(id, motion)에 다시 전달한다. 인덱스는 삽입·삭제 후 달라진다.

각 Step은 transform(elapsedSeconds), space, startSeconds, 선택적 durationSeconds, enabled를 가진다. 시작 전에는 기여하지 않고, 종료 시각 이후에는 마지막 행렬을 유지한다. 사용자 transform 콜백은 아핀 행렬을 반환한다. 같은 공간 안에서는 API로 추가한 순서대로 적용한다. 예를 들어 A 다음 B는 B*A다. World와 Local은 별도 목록으로 합성하므로 서로 다른 공간의 추가 순서로 두 공간을 교차 배치하지 않는다. 공간을 넘는 계층은 레이어로 표현한다.

```cpp
graphics::Motion combo;
graphics::RotationMotion orbit;
orbit.degreesPerSecond = -20;
orbit.axis = {{0, 0, 0}, {0, 1, 0}};
orbit.space = graphics::TransformSpace::World;
if (!combo.append(graphics::Motion::rotation(orbit))) return false;
const auto reflect = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
if (!reflect || !combo.append(graphics::Motion::fixed(*reflect))) return false;
```

append로 넣은 Step들은 각자의 startSeconds 기준으로 동시에 평가한다. then(step)은 앞선 모든 Step의 종료 시각 뒤에 배치한다. 앞선 Step 중 무기한 회전·반복 경로가 있으면 false다. 시간 순서 실행은 이전 변환을 삭제하거나 대체하지 않고 종료 행렬을 유지하며 다음 변환을 합성한다.

```cpp
graphics::Motion sequential;
graphics::MotionStep x;
x.durationSeconds = 1;
x.transform = [](double t) { return glm::translate(glm::mat4(1), glm::vec3(float(t), 0, 0)); };
graphics::MotionStep y;
y.durationSeconds = 1;
y.transform = [](double t) { return glm::translate(glm::mat4(1), glm::vec3(0, float(t), 0)); };
if (!sequential.append(x) || !sequential.then(y)) return false;
// t=0.5: (0.5,0,0), t=1.5: (1,0.5,0), t>=2: (1,1,0)의 추가 이동.
```

MotionSystem의 편의 생성 API는 기본 Step 합성을 재사용한다.

| API | 생성 결과 |
|---|---|
| createMotion(motion) | 사용자 조합 |
| createSpin(rotation) | space를 Local로 지정한 자전 |
| createOrbit(rotation) | space를 World로 지정한 공전 |
| createOrbitAndSpin(orbit, spin) | World 공전과 Local 자전의 독립 축·각속도 |
| createPathAndSpin(path, spin) | 사용자 경로의 이동과 Local 자전 |

createSpin의 기본 축은 모델 원점을 지나는 Y축이다. 모델의 localBasis를 중심으로 자전하려면 axis.point에도 같은 모델 좌표를 지정한다. createOrbit은 기존 모델 배치 전체를 지정 월드 직선 주위로 회전시킨다.

### 13.5 식별자와 재생 상태

- createMotion은 optional<MotionId>를 반환한다. ID는 MotionSystem 인스턴스에 속한다.
- getMotion은 const MotionState*를 반환한다. motion, timeSeconds, playing, enabled, sampled를 조회한다.
- setMotion은 설정을 교체하되 현재 시각·재생·활성 상태를 유지한다. 처음부터 시작하려면 seekMotion(id, 0)을 호출한다.
- setMotionPlaying(false)는 시계를 멈추고 마지막 변환을 유지한다. true는 그 시각부터 재개한다.
- setMotionEnabled(false)는 변환의 기여만 항등행렬로 바꾼다. playing=true이면 보이지 않는 동안에도 시간은 흐른다. 다시 활성화하면 현재 시각의 변환이 나타난다.
- seekMotion은 0 이상의 유한한 시각으로 이동하고 즉시 샘플을 바꾼다.
- removeMotion은 관련 레이어에서 그 동작 바인딩도 해제한다. 레이어의 고정 transform은 유지한다.
- update는 재생 중인 각 Motion을 한 번 평가한다. 같은 ID를 여러 레이어가 공유해도 시계가 여러 번 진행하지 않는다.
- Motion·Layer ID는 삭제나 clear 후에도 재사용하지 않는다. rootLayer()의 0만 장면 전체 루트로 유지한다. EntityId는 기존처럼 Scene::clear 후 재사용되므로 새로 받는다.

생성·설정·seek는 해당 시각의 콜백을 실행할 수 있다. bool/optional 실패는 식별자·매개변수 또는 행렬이 유효하지 않다는 뜻이다. 사용자 콜백 예외와 내장 경로의 비유한 결과 예외는 호출자에게 전파된다. Application::run은 기존 예외 정리 경로로 GPU·창 자원을 정리하고 재전파한다. update는 모든 샘플을 준비한 뒤 시계와 캐시를 반영하므로 중간 실패 시 일부 Motion만 시간이 진행하지 않는다. 콜백의 외부 부작용까지 되돌리지는 않는다.

콜백은 결정적 계산 함수로 사용하고, 평가 중 Scene·MotionSystem을 수정하거나 재진입하지 않는다. 캡처한 객체의 수명은 동작이 삭제될 때까지 유지한다. getMotion/getLayer/getComponent의 포인터를 수정 API 호출 전후로 장기 보관하지 않는다. API는 기존 Scene처럼 단일 렌더링 스레드에서 사용한다.

### 13.6 레이어 트리와 객체 바인딩

```cpp
auto& motions = scene.getMotions();
graphics::RotationMotion spin;
const auto spinId = motions.createSpin(spin);
const auto groupLayer = motions.pushLayer(graphics::MotionSystem::rootLayer());
if (!spinId || !groupLayer || !motions.bindMotion(*groupLayer, *spinId)) return false;
if (!scene.bindEntity(firstEntity, *groupLayer) || !scene.bindEntity(secondEntity, *groupLayer)) return false;
```

이 코드는 같은 자전 시각·파라미터를 공유한다. 각 객체에 직접 바인딩하면 각 객체의 모델 좌표에서 자전한다. 객체들의 공통 중심을 기준으로 함께 돌리려면 World 공전을 사용하거나, Scene::addGroup/setParent로 조립한 부모 객체에 Local 자전 레이어를 바인딩한다.

레이어는 root -> branch -> leaf 형태의 트리다. 각 루트-말단 경로가 하나의 변환 스택이며 형제 가지는 서로 영향을 주지 않는다. root의 고정 변환과 Motion은 미바인딩 객체·소유자 없는 선을 포함한 전체 Scene에 한 번 적용한다. LayerConfig는 transform, space, 선택적 motion, enabled를 가진다. 고정 transform과 Motion이 함께 있으면 같은 공간에서 고정 변환 다음 Motion을 적용한다. enabled=false는 그 레이어의 기여만 생략하며 자식과 공유 Motion 시계는 유지한다.

| API | 효과와 제한 |
|---|---|
| pushLayer(parent, config) | 부모의 마지막 자식으로 추가. 반환 ID를 다음 push의 부모로 쓰면 stack push |
| insertLayer(child, config) | 기존 child와 부모 사이에 삽입. child ID·객체 바인딩 보존 |
| popLayer(top) | 자식 없는 말단만 제거하고 부모 ID 반환. 루트·분기는 거부 |
| removeLayer(id) | 해당 노드만 제거. 자식은 기존 형제 위치에 부모로 승격. 루트 삭제 거부 |
| reparentLayer(id, parent) | 가지를 새 부모의 마지막 자식으로 이동. 자기 자신·자손 밑 연결 거부 |
| setLayer(id, config) | 고정 변환·공간·동작·활성 상태 수정. 트리 연결 보존 |
| getLayer(id) | const LayerState*로 설정·부모·자식 조회 |
| bindMotion(layer, id/nullopt) | Motion 연결/해제. 레이어당 하나이며 여러 Step은 Motion 안에서 합성 |
| Scene::bindEntity(entity, layer) | 객체당 말단 하나 연결. 기존 바인딩 교체 |
| Scene::unbindEntity(entity) | 직접 바인딩 해제. 부모의 바인딩 또는 루트로 복귀 |
| Scene::getEntityLayer(entity) | 직접 바인딩된 유효 ID만 반환. 상속된 ID는 반환하지 않음 |

같은 Scene 객체의 서로 다른 자식들을 독립시키려면 각자 다른 말단에 연결한다. 객체에 직접 연결이 없으면 가장 가까운 객체 조상의 바인딩을 사용한다. 직접 연결이 있으면 상속된 가지를 대체한다. 부모·자식에서 바인딩을 중복 누적하지 않는다. 부모와 자식의 효과를 모두 원하면 자식의 레이어 가지가 원하는 상위 레이어를 포함하도록 구성한다. 객체 부모 트리와 레이어 트리 사이에 포인터나 같은 모양을 강제하지 않는다.

삭제된 레이어의 직접 바인딩은 조회 시 무효 처리한다. 해당 객체는 가까운 객체 조상 바인딩 또는 루트로 복귀한다. 승격된 자식 레이어의 바인딩은 보존한다. reparent/remove는 월드 자세를 보존하는 역행렬 보정을 하지 않으므로 새 합성 결과로 바뀐다.

최종 행렬은 다음과 같다.

```text
P = 루트의 전역 변환 * 가지의 World 변환들
L = 가지의 Local 변환들
A = 객체 부모 계층의 기존 SRT 중 바인딩된 객체까지
D = 바인딩된 객체 아래 자손의 기존 SRT
M_entity = P * A * L * D
M_face = M_entity * 면의 기존 추가 SRT * localAnimationMatrix
```

직접 바인딩된 객체는 D=I다. 상속이면 Local 변환은 바인딩한 조상 위치에 들어가므로 그 그룹 전체가 함께 움직인다. 가지 내부는 루트에서 말단 순서로 행렬을 오른쪽에 쌓는다. 루트의 Local 기여도 장면 전체 변환으로 처리한다. World는 객체의 Scene 부모 좌표가 아니라 기존 객체 배치 전체의 바깥쪽, 즉 월드 측 공간이다.

소유자가 있는 선은 owner의 M_entity를, 소유자가 없는 선은 루트의 전역 행렬만 사용한다. 색·표시 여부·카메라·메시 정점은 이 레이어에서 수정하지 않는다. 월드 행렬을 조회할 때 객체·레이어의 깊이에 따라 CPU 순회와 임시 저장이 필요하다. 성능 측정은 하지 않았다.

실행 확인 절차는 [USER_CHECKS.md](docs/USER_CHECKS.md)의 Motion 절, 조립 예시는 [APPLICATION_PATTERNS.md](docs/APPLICATION_PATTERNS.md)의 추가 절을 참고한다.


## OBJ 로더 변경 (2026-10-22)

Assimp 의존성을 제거했다. loadObj(path, options)를 사용하며 loadPrimitive/loadModel도 같은 OBJ 텍스트 로더를 호출한다. [표기·호환 재시도·제한·직접 확인](docs/OBJ_LOADING.md)을 참고한다.
