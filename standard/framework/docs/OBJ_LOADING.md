# 현재 배포본: 2026-11-02

아래 OBJ·텍스처 계약은 현재 배포 헤더와 함께 읽는다. [전체 API 사례](API_REFERENCE.html#case-assets)와 [응용패턴](IMPLEMENTATION_PATTERNS.html#pattern-assets)에서 초기화 연결 코드를 제공한다. 빌드·실행은 사용자에게 맡긴다.

# OBJ 텍스트 로드 API

Assimp와 DLL·헤더 종속성을 제거했다. C++17, Windows x64 API다. `loadObj`는 확장자가 `.obj` 또는 `.txt`인 OBJ 텍스트를 같은 방식으로 읽는다. 경로는 실행 파일 디렉터리를 기준으로 한 `projectRoot` 내부 상대경로다. 외부 경로는 거부한다.

```cpp
// Application 초기화 콜백 안에서 사용한다.
graphics::LoadOptions options;
options.pivot = graphics::PivotMode::Preserve;
options.outline = graphics::OutlineMode::FeatureEdges;
const auto result = app.getAssets().loadObj("data/cube.txt", options);
if (!result) {
    return false; // loadObj가 오류 원인과 서식 안내를 이미 출력했다.
}
for (const auto& warning : result.warnings) std::cerr << warning << '\n';
return app.getScene().addModel(*result.model, graphics::Transform3D{},
    result.pivotMode, result.localBasis).has_value();
```

`<iostream>`를 포함한다. 파일 읽기는 OpenGL 컨텍스트 없이 `graphics::AssetLoader assets("."); assets.loadObj("data/cube.txt");`로도 호출할 수 있다. 이때 루트는 생성자에 전달한 경로다. `loadPrimitive`와 `loadModel`은 `loadObj`의 호환 함수다. `FeatureOptions.models`와 AssetLoader 생성자의 세 번째 런타임 경로 인자는 제거했다. OBJ 읽기는 항상 가능하며 텍스처만 `config.features.textures=true`로 켠다.

## 입력 → 메시 → 표시

1. 텍스트의 `v/vt/vn` 배열과 `f`의 각 코너를 읽는다. 위치·UV·법선 인덱스는 서로 독립적이다.
2. `f`를 삼각형으로 분할하고 원래 좌표를 보존한다. UV 경계와 법선 경계를 가진 코너를 별도 정점으로 유지한다. 누락된 법선만 면 안에서 계산한다.
3. `ModelResult`의 model·pivotMode·localBasis를 `Scene::addModel`로 전달한다.
4. Renderer가 기존 VAO/VBO/EBO 경로로 업로드한다. 로더는 GPU 자원을 만들지 않는다.

| 처리 | 지원 범위 |
|---|---|
| 면 표기 | `v`, `v/vt`, `v//vn`, `v/vt/vn`; 양수 1기반·음수 상대 인덱스 |
| 정점 | xyz, xyzw(w로 나눔), xyz rgb 확장 |
| UV | u, uv, uv0; 텍스처 표시가 꺼져도 보존 |
| 텍스트 | UTF-8 BOM 유무, LF/CRLF/CR, 주석, 탭·공백, 역슬래시 줄 연결 |
| 면 | 삼각형, 평면 단순 볼록·오목 다각형. 원래 winding 보존 |
| 그룹 | o/g 이름 보존; o/g/usemtl/s 경계 밖으로 면을 병합하지 않음 |
| 일부 속성 누락 | 법선은 누락 코너만 생성; UV가 일부만 있으면 누락 코너를 (0,0)으로 채우고 경고 |
| 공면 병합 | 기본 true. 정점 색이 있으면 색 보존을 위해 병합을 생략하고 경고 |

## 실패 시 다른 읽기 경로

표준 읽기가 `ObjParseError`로 실패하면 파일 전체를 호환 읽기로 다시 파싱한다. 부분 모델은 반환하지 않는다. `allowObjFallback=false`로 재시도를 끌 수 있다.

- UTF-16 LE/BE BOM 텍스트를 UTF-8로 디코딩한다. 잘못된 surrogate·잘린 텍스트는 거부한다.
- 양수 인덱스는 파일 전체의 배열로 해석해 뒤에서 정의된 정점·UV·법선을 허용한다. 음수 인덱스는 해당 면을 만났을 때의 배열 길이를 유지한다.
- 연속 중복 코너와 첫 코너를 끝에 다시 쓴 표기를 제거한다. 위치·UV·법선 인덱스가 모두 같은 코너만 제거한다.

성공하면 `usedFallback=true`와 `warnings`에 재시도 원인·보정 내용을 담는다. 두 읽기가 실패하면 `error`에 파일 경로, 행 번호, 두 실패 원인을 담는다. 파일 열기·텍스처 오류는 기하 파싱을 재시도하지 않는다.

0·범위 밖 인덱스, 비정상 숫자, 퇴화면, 자기 교차, 비평면 다각형은 임의 삼각형 fan이나 면 누락으로 숨기지 않고 오류로 반환한다. 비평면 면은 내보내기 도구에서 먼저 삼각형으로 나눈다. 아주 얇은 면은 부동소수점 허용오차로 거부될 수 있다.

## 재질과 형식의 제한

MTL 파일 자동 읽기, `usemtl` 재질 적용, smoothing group 보간, 선·점·곡면, glTF·FBX·스켈레톤은 지원하지 않는다. `mtllib/usemtl`은 경계를 유지하고 경고한다. `s`는 경계를 유지하며 off/0 이외에는 미보간 경고를 반환한다. 선·점·곡면 등 지원하지 않는 레코드는 파일 전체를 실패시킨다.

텍스처는 `LoadOptions.texturePath` 또는 Scene 재질 API로 명시한다. texturePath가 있으면 텍스처 기능을 켜야 하며 모든 면에 UV가 있어야 한다. 이 제한은 결과 경고를 확인해 판단한다. OBJ 지원은 모든 OBJ 확장 기능 지원을 뜻하지 않는다.

## 사용자가 실행할 확인

빌드·실행·테스트는 에이전트가 수행하지 않았다. Visual Studio C++17·x64에서 `examples/assets.cpp`를 유일한 main으로 등록하고 Framework.props를 적용한다. 실행 파일 아래 `framework/examples/data`에 동봉 데이터를 복사한다.

| 입력 또는 조작 | 예상 결과 |
|---|---|
| `assets.exe obj framework/examples/data/cube.obj` | 큐브, 기본 공면 병합 시 6개 면·12개 삼각형 |
| pyramid.obj / tetrahedron.obj | 원본 data와 같은 형상 |
| corner_formats.obj | 네 표기 모두 로드; `mergeCoplanarFaces=false`면 4개 면·4개 삼각형 |
| forward_closed.obj | 1개 삼각형, usedFallback=true, 보정 경고 |
| 위 파일을 UTF-16 LE/BE BOM으로 저장한 사용자 사본 | UTF-16 디코딩 경고, 같은 기하 |
| allowObjFallback=false + forward_closed.obj | 실패하며 model 없음 |
| 코너 인덱스를 0 또는 999로 수정한 사용자 사본 | 실패하며 행 번호·원인 표시, 부분 모델 없음 |
| 정점 세 개를 일직선으로 만든 사용자 사본 | 퇴화면 오류 |
| 오목한 5각형 / 자기 교차 다각형 | 오목한 평면은 분할, 자기 교차는 오류 |
| 텍스처·클릭·이동·회전 | UV 표시, 기존 피킹·변환이 메시와 일치 |

회상 질문: UV 경계에서는 위치 인덱스가 같아도 왜 서로 다른 렌더링 정점이 필요한가?

## 최종 실패 시 콘솔 안내

표준 읽기와 허용된 호환 읽기가 모두 실패하면 loadObj가 std::cerr에 파일 경로·오류 원인, 지원하는 v/vt/vn/f 서식·코너 표기·인덱스 규칙, 최소 삼각형 예제를 출력한다. allowObjFallback=false이면 표준 읽기의 최종 실패에서 같은 안내를 출력한다. loadPrimitive/loadModel도 공통 경로를 사용한다. 호환 읽기로 성공한 경우에는 이 실패 안내를 출력하지 않는다.

파일 열기·경로·텍스처 오류도 최종 로드 실패이므로 원인을 먼저 확인하라고 출력한다. 이 경우에는 파서부터 수정하지 않는다. 그래도 서식이 맞지 않으면 원본 텍스트를 확인하고 해당 서식의 파서를 직접 작성한다. 파서는 graphics::Mesh.localVertices와 triangleIndices를 채우고 graphics::recalculateNormals(mesh)를 호출한 뒤 graphics::Model.faces에 넣는다. Scene::addModel로 모델을 등록한다. UV·재질이 필요하면 같은 정점 개수에 맞춰 추가한다. 이 안내는 파서를 대신 실행하지 않는다.

ModelResult.error 반환은 유지한다. 콘솔 출력이 필요하므로 이제 로더에 실패 시 출력 부작용이 있다. 호출자는 같은 error를 다시 출력하면 중복된다. 동봉 assets 예제는 중복 출력을 제거했다. 정상 로드와 성공한 호환 읽기의 warnings 반환은 기존과 같다.

사용자가 직접 확인: 없는 경로 또는 f 0 2 3인 사본을 로드하면 최종 실패 안내 한 묶음과 model 없음이 예상된다. forward_closed.obj는 재시도로 성공하므로 최종 실패 안내가 없어야 한다. allowObjFallback=false로 같은 파일을 읽으면 실패 안내가 예상된다. assets 예제에서 오류 원인이 두 번 출력되지 않는지 확인한다. 빌드·실행·테스트는 에이전트가 수행하지 않았다.
