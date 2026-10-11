# framework 전용 흐름

이 문서는 새 루트 framework만 대상으로 한다. 다른 과제의 main, Console, framework 사본과 실행 프로젝트는 포함하지 않는다. AST의 타입 참조와 실제 소스 호출을 구분했다. 빌드·테스트는 수행하지 않았다.

## 진입점과 콜백

Framework.h → include/Application.h를 통해 공개 API와 inline 구현을 가져온다. src/Application.cpp와 Graphics.cpp는 호환 include 래퍼다. 프레임워크 자체에 main은 없다. examples/basic.cpp와 assets.cpp의 main은 서로 다른 사용자 호출 예제다. 하나의 실행 프로젝트에 두 예제 main을 함께 넣지 않는다.

basic.cpp의 main → Application 생성 → init/mouse/key 콜백 등록 → run. init 콜백은 addCuboid, setTransform, setOutline으로 장면을 만든다. mouse 콜백은 pickAt → getMesh → splitFace → setFaceSRT를 호출한다. assets.cpp의 main은 상대 모델·이미지 경로와 모드를 받아 config.features를 설정한다. init 콜백은 loadPrimitive 또는 loadModel → addModel로 장면을 만든다.

콜백 등록은 콜백 본문 실행이 아니다. detail/Application.inl의 Application::run은 initialize 성공 후 initHandler를 한 번 호출한다. initialize는 번들 GLFW의 창·GL 컨텍스트와 Renderer를 초기화하고 입력 콜백을 등록한다. PollEvents/WaitEvents가 키·마우스·커서·포커스 콜백을 실행한다. 프레임 루프는 updateHandler 뒤에 renderHandler 또는 renderScene을 호출한다. C 콜백 내부 예외는 dispatch가 보관하고 루프에서 다시 전달한다.

## 데이터와 GPU

ModelLoader.h의 loadPrimitive는 기본 도형의 v/vt/vn/f를 읽는다. loadModel은 models 모드 분기를 확인한 다음 AssimpApi::load와 aiImportFile로 일반 모델을 읽는다. textures 모드가 꺼져 있으면 UV·이미지 작업을 생략한다. 모델이 DLL에서 반환된 뒤 프레임워크의 Model/Mesh에 복사되며 aiScene은 같은 DLL에서 해제된다.

Model.h의 Model/Mesh/Vertex/Material → Scene.inl의 addModel 또는 setter 검증 → CPU 장면 상태와 geometryRevision. Scene::setModel은 Scene.inl:430, 검증 후 revision 갱신은 :439에 있다. Renderer.inl의 Renderer::render는 장면 주소·revision·textures 모드가 바뀌었는지 확인한 뒤 upload를 호출한다. upload는 임의 정점·인덱스를 GpuVertex/VBO/EBO에 묶고 드로우 범위를 만든다.

VAO 속성 pos/color/UV/normal → Shaders.h의 내장 GLSL → uMVP 정점 변환 → 삼각형 조립·클리핑·래스터화 → 이미지·선택적 조명 → 깊이 처리 → 프레임버퍼 → SwapBuffers. GLSL 파일은 내장 문자열의 별도 사본이며, 문자열 내부의 GLSL 호출 관계는 C++ AST 그래프에 별도 추출되지 않았다.

## 부모·면 변환과 피킹

Model.h의 detail::transformMatrix는 basis·회전·scale·localBasis를 합성한다. faceMatrix는 부모 행렬을 먼저 적용하고 분리 면의 추가 SRT를 뒤에 합성한다. splitFace/setFaceSRT는 Scene.inl:508/:515에 정의되어 있다. Scene.inl:372의 DiagramSystem::faceModelMatrix가 공통 진입점이다.

Renderer.inl:157/:174와 Picking.h:47은 이 faceModelMatrix를 직접 호출한다. 따라서 피킹이 렌더링과 같은 변환을 사용하는 것은 소스에서 확인한 관계다. Application::pickAt은 창 좌표를 프레임버퍼 좌표로 바꾸고, screenRay는 카메라의 역 view/projection으로 광선을 만든다. pick은 표시된 삼각형들의 교점을 비교해 가까운 객체·면·삼각형을 반환한다. 변환 분기와 수학의 실제 동작은 사용자가 테스트해야 한다.

## 종료와 분석 한계

run 정상 종료 또는 예외 → shutdown → AssetLoader 해제 → current 컨텍스트에서 Renderer::destroy → 창 해제 → GLFW 종료. Native.h의 DLL 연결은 함수 포인터다. AST에서 DLL export의 실행 내부를 추적하지 않는다. COM/WIC와 그래픽 드라이버 내부 동작도 그래프 범위 밖이다.

GRAPH_HEALTH.md에는 해석되지 않은 관계 48개, 방향 관계의 같은 끝점 병합 77개가 기록된다. 원시 EXTRACTION.json은 추출된 관계를 보존한다. graph.json의 엣지 개수를 모든 호출 위치의 개수로 해석하지 않는다. INFERRED 관계는 보고서에 별도로 표시된다.
