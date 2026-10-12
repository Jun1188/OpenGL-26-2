# framework 전용 실행 흐름

분석 범위는 루트 framework의 C++ 20개 파일이다. 의존성 SDK·다른 과제·문서·GLSL 내부는 제외한다. 아래 호출 관계는 실제 소스에서 읽었다. AST의 EXTRACTED·INFERRED 관계는 graph.json의 confidence로 구분한다. 빌드·실행·테스트는 하지 않았다.

## 별도 진입점

프레임워크 자체에는 main이 없다. Framework.h → Application.h → Graphics.h/ModelLoader.h/Picking.h와 inline 구현을 포함한다. src의 두 cpp는 호환 include 래퍼다.

- basic.cpp: Application 생성 → init/mouse/key 콜백 등록 → run. init은 addCuboid/setTransform/setOutline, 클릭은 pickAt/getMesh/splitFace/setFaceSRT를 호출한다.
- assets.cpp: 모델·이미지 상대경로와 모드 입력 → config.features → init의 loadPrimitive 또는 loadModel → addModel → run 루프.
- hierarchy.cpp: init/update/mouse/key 등록 → run. init은 세 addCuboid → setParent(지구,태양)/setParent(달,지구) → generateTexcoords → setMaterial. update는 태양·지구 local 회전을 변경한다. 클릭은 pickAt으로 ID와 UV를 출력한다.

각 main은 별도 실행 프로그램이다. 한 실행 프로젝트에 세 main을 동시에 넣지 않는다. 콜백 등록과 본문 실행은 별개다. Application::run은 initialize 후 initHandler를 한 번 호출한다. initialize는 GLFW DLL·창·current GL 컨텍스트·Renderer·입력 콜백을 설정한다. 프레임 루프는 updateHandler → renderHandler 또는 renderScene → SwapBuffers를 수행하며 GLFW 이벤트 처리가 입력 콜백을 실행한다. C 콜백 예외는 dispatch가 보관한다.

## 모델과 UV 입력

ModelLoader.h는 AssetLoader의 공개 선언, 프로젝트 내부 경로 제한, 이미지 캐시, finishLoad를 유지한다. PrimitiveLoader.inl의 loadPrimitive는 v/vt/vn/f·음수 인덱스·평면 다각형 삼각분할·공면 병합을 처리한다. AssimpLoader.inl의 loadModel은 models 분기 → AssimpApi::load → aiImportFile → 노드 순회 → 프레임워크 Model/Mesh 복사 → aiReleaseImport 순서다. 노드 변환은 정점에 반영한다. 노드 계층과 뼈대 애니메이션은 보존하지 않는다. DLL 내부는 AST 범위 밖이다.

Geometry.h의 generateTexcoords는 메시 복사 → 경계 상자 → 투영 방식 → 필요 시 삼각형 정점 분리 → UV scale/rotation/offset → 유효 메시 반영 순서다. Planar는 지정 축, Box는 삼각형 법선 주축, Cylindrical/Spherical은 Y축 둘레 경도를 사용한다. U 경계의 좌표는 반복 샘플링을 위해 1을 넘을 수 있다. Scene의 UV setter는 setMesh를 통해 geometryRevision을 갱신한다. 외곽선 위치·법선·색상은 유지하지만 정점 번호는 바뀔 수 있다.

Texture.h의 WIC 이미지 디코딩 → TextureImage RGBA → Material.texture → Renderer::upload의 GPU 이미지 업로드 → UV 보간 → sampler2D 샘플링 순서다. textures 옵션이 꺼지면 GPU 텍스처 경로는 생략한다. 이미지 경로는 loadTexture 또는 LoadOptions.texturePath로 지정한다. UV가 없는 파일에 텍스처를 지정한 로딩은 기존처럼 오류를 반환한다. 자동 매핑은 비텍스처 상태로 로딩한 뒤 명시적으로 수행한다.

## 계층 좌표와 GPU

Transform3D의 basis·Euler 회전·quaternion orientation·scale·localBasis → detail::transformMatrix. SRT가 정점에 적용되는 순서는 피벗 이동 → 크기 → 회전 → 위치이며 열벡터 행렬로 합성한다.

Scene::setParent는 ID와 부모 사슬을 읽어 자기 연결·순환을 거부한다. worldMatrix는 현재 물체부터 조상을 순회하며 부모 local × 누적 local을 계산한다. faceWorldMatrix는 부모 world × 물체 및 면 행렬을 계산한다. parent는 Component_Poly 마지막 필드이므로 기존 중괄호 초기화 순서를 유지한다. 연결·해제는 local 값을 유지하며 world 값을 유지하지 않는다.

Renderer::draw와 Picking.h의 pick은 Scene::faceWorldMatrix를 호출한다. owner가 있는 선은 Scene::worldMatrix를 사용한다. 따라서 물체·면·선·교점이 같은 계층 변환을 사용하는 것은 소스에서 확인한 관계다. 부모 visibility는 자식에게 상속하지 않는다.

CPU Scene 형상 → Renderer::render의 scene/revision/textures 비교 → 필요한 경우 upload → GpuVertex 위치/색/UV/법선 → VBO/EBO/VAO → draw 범위별 world 행렬 → uMVP=projection×view×world → 셰이더 → 래스터화·텍스처·선택적 조명·깊이 처리 → 프레임버퍼. 법선 행렬은 누적 world의 inverse-transpose다. 변환과 부모 연결만 바뀌면 형상 버퍼를 다시 올리지 않는다.

Application::pickAt은 창 좌표를 프레임버퍼 좌표로 바꾸고 screenRay는 카메라 view/projection의 역변환을 사용한다. pick은 표시된 삼각형의 월드 교점을 비교하고 가까운 ID·면·삼각형·로컬 좌표·무게중심·UV를 반환한다. 선과 텍스처 투명 픽셀은 판정하지 않는다.

## 종료와 분석 한계

run 종료·예외 → shutdown → AssetLoader 해제 → current 컨텍스트에서 Renderer::destroy → 창 해제 → GLFW 종료. COM/WIC·DLL export·GPU 드라이버 내부는 그래프 범위 밖이다.

현재 그래프: 643 노드, 1013 방향 관계, 26 커뮤니티. 원시 추출에 해석되지 않은 끝점 관계 49개, 같은 끝점 방향 관계 병합 82개가 있다. GRAPH_HEALTH.md와 EXTRACTION.json에 기록한다. 그래프 관계 수를 모든 실제 호출 횟수로 해석하지 않는다. 실행 정확성은 사용자 테스트가 필요하다. 벤치마크는 실행하지 않았다.
