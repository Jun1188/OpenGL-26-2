# 분석 문서 before / after

## framework-analysis/README.md

```diff
--- before
+++ after
@@ -1,22 +1,18 @@
 # framework 전용 graphify 분석
 
-입력: 저장소 루트 framework의 C++ 17개 파일. 출력: framework-analysis/graphify-out. 분석 자료는 복사용 framework 폴더와 분리한다. 저장소 루트의 기존 graphify-out은 별개 분석이며 수정하지 않았다.
+2026-10-12 계층 SRT·UV 생성·로더 분리를 반영했다. 입력은 루트 framework의 C++ 20개 파일이며 출력은 framework-analysis/graphify-out이다. SDK·문서·과제 사본·GLSL 내부는 그래프 입력에서 제외했다. examples의 세 main은 별도 프로그램이다.
 
-- graphify-out/graph.html: 인터랙티브 그래프
-- graphify-out/graph.json: 노드·관계와 source_file/source_location
-- graphify-out/GRAPH_REPORT.md: 중심 노드, 커뮤니티, 교차 관계, 탐색 질문
-- graphify-out/FRAMEWORK_FLOW.md: 실제 소스와 대조한 초기화·입력·렌더링·정리 흐름
-- graphify-out/GRAPH_HEALTH.md 및 EXTRACTION.json: 손실·미해석 관계 진단과 원시 추출
-- graphify-out/SOURCE_MANIFEST.json: 입력 범위와 소스 SHA-256
-- graphify-out/BENCHMARK.json: 구조 검색 결과의 추정 크기
-- CHANGES.md: 이번 이동·재분석의 변경 전후
+- graph.html: 인터랙티브 구조 그래프.
+- graph.json: 643개 노드와 1013개 방향 관계, 소스 파일·위치·confidence.
+- GRAPH_REPORT.md, COMMUNITIES.json, ANALYSIS.json: 26개 커뮤니티와 구조 요약.
+- FRAMEWORK_FLOW.md: 실제 소스와 대조한 초기화·계층 변환·UV·GPU·피킹·종료 흐름.
+- GRAPH_HEALTH.md, EXTRACTION.json: 해석되지 않은 끝점 49개, 같은 끝점 방향 관계 병합 82개 및 원시 관계.
+- SOURCE_MANIFEST.json: 입력 경로·SHA-256·기준 커밋. includes_uncommitted_step4는 최종 문서 커밋 전의 예제와 호환성 수정도 입력에 포함했다는 뜻이다.
+- BENCHMARK.json: 미실행 상태. 이전 검색 크기 비교는 갱신된 그래프의 결과로 사용하지 않는다.
+- CHANGES.md, BEFORE_AFTER.md: 변경과 분석 문서의 전후.
 
-SDK, 변경 전후 보고서, 프로젝트 밖 파일은 그래프 입력에서 제외했다. examples의 두 main은 서로 다른 사용자 호출 예제다. .inl은 이 추출 프로세스에서만 C++ 추출기에 등록했다. graphify 설치 파일은 변경하지 않았다. GPU 셰이더는 내장 문자열로 존재하며 C++ 구조 그래프가 GLSL 내부를 별도 해석하지는 않는다.
+update_graph.py는 현재 설치된 graphify의 AST·분석·내보내기 API를 사용한다. .inl은 이 프로세스에서만 C++ 추출기에 등록한다. 설치 파일을 수정하지 않는다. UTF-8 BOM JSON은 utf-8-sig로 읽고, 새 그래프를 별도 파일로 내보낸 뒤 기존 노드 수보다 작지 않을 때 교체한다. 런타임과 cache_root는 framework-analysis이며 기본 CLI의 소스 내부 출력으로 되돌리지 않는다.
 
-613개 노드, 964개 방향 관계, 23개 커뮤니티. 원시 관계에서 미해석 끝점 48개와 같은 끝점 병합 77개를 확인했다. 코드는 AST로만 분석했고 의미 분석 API 비용은 없다. BENCHMARK는 전체 10,621 추정 토큰 대비 검색 평균 3,066 추정 토큰(3.5배)의 크기 비교다. 실제 토큰 과금이나 답변 품질·속도 측정이 아니다.
+재분석은 저장된 graphify Python 인터프리터로 update_graph.py를 실행한다. Windows 쉘에 PYTHONUTF8=1, PYTHONIOENCODING=utf-8 및 콘솔 UTF-8을 설정한다. 사용자 산출물은 UTF-8 BOM으로 저장한다. 소스 구조 분석만 수행하며 빌드·테스트·벤치마크는 실행하지 않는다. 현재 분석은 외부 의미 분석 API를 호출하지 않았다.
 
-이전 framework/graphify-out 전체는 archive/framework-graphify-out-a194f47로 이동했다. 이전 전체 저장소 PROJECTS/EXECUTION_FLOW는 보관본에만 있다. 현재 전용 그래프에는 포함하지 않았다. 캐시는 보관하되 Git에 추가하지 않는다.
-
-다시 분석할 때 소스 루트는 ../framework, 작업·출력 위치는 이 framework-analysis 폴더로 설정한다. SOURCE_MANIFEST의 파일 목록만 AST 입력에 넣고 cache_root도 framework-analysis로 지정한다. 기본 CLI의 소스 내부 출력으로 되돌리지 않는다. Windows 실행 쉘에 PYTHONUTF8/PYTHONIOENCODING=UTF-8을 설정하고, 사용자 산출물은 UTF-8 BOM으로 읽고 쓴다. JSON을 읽는 도구는 utf-8-sig로 BOM을 처리한다. .graphify_root/.graphify_python은 로컬 경로이므로 Git에서 제외한다.
-
-빌드·framework 실행·동작 테스트는 하지 않았다. framework/docs/USER_CHECKS.md의 테스트는 사용자가 수행한다.
+archive/framework-graphify-out-a194f47과 ARCHIVE_MANIFEST.json은 과거 보관 자료다. 수정하지 않았다. 저장소 루트의 다른 graphify-out도 수정하지 않았다. 사용자 실행 항목은 ../framework/docs/USER_CHECKS.md에 있다.
```

## framework-analysis/CHANGES.md

```diff
--- before
+++ after
@@ -60,3 +60,12 @@
 이전 캐시 경로를 제거했다. 새 framework-analysis/.gitignore는 해당 분석 폴더의 로컬 경로·캐시만 제외한다. 새 문서·분석 파일의 before는 없음이며 전체 내용은 각 파일에 있다. 보관된 옛 그래프는 내용 변경 없이 이동했다.
 
 framework 실행 코드와 다른 프로젝트의 framework는 변경하지 않았다. graphify 설치 파일도 변경하지 않았다. 빌드·테스트는 수행하지 않았다.
+
+## 2026-10-12 개선 반영
+
+- 입력 17 → 20 파일: PrimitiveLoader.inl, AssimpLoader.inl, hierarchy.cpp 추가.
+- 계층 SRT와 UV 생성·조정 API, 렌더·피킹·owner 선의 월드 행렬 연결 반영.
+- 노드 613 → 643, 방향 관계 964 → 1013, 커뮤니티 23 → 26.
+- 끝점 미해석 48 → 49, 방향 관계 병합 77 → 82.
+- GPU와 DLL 내부는 분석하지 않음. 빌드·실행·테스트·벤치마크 미실행.
+- 이전 archive는 유지. 코드 전체 before/after는 framework/docs/FULL_CODE_BEFORE_AFTER.md 참조.
```

## framework-analysis/graphify-out/FRAMEWORK_FLOW.md

```diff
--- before
+++ after
@@ -1,31 +1,39 @@
-# framework 전용 흐름
+# framework 전용 실행 흐름
 
-이 문서는 새 루트 framework만 대상으로 한다. 다른 과제의 main, Console, framework 사본과 실행 프로젝트는 포함하지 않는다. AST의 타입 참조와 실제 소스 호출을 구분했다. 빌드·테스트는 수행하지 않았다.
+분석 범위는 루트 framework의 C++ 20개 파일이다. 의존성 SDK·다른 과제·문서·GLSL 내부는 제외한다. 아래 호출 관계는 실제 소스에서 읽었다. AST의 EXTRACTED·INFERRED 관계는 graph.json의 confidence로 구분한다. 빌드·실행·테스트는 하지 않았다.
 
-## 진입점과 콜백
+## 별도 진입점
 
-Framework.h → include/Application.h를 통해 공개 API와 inline 구현을 가져온다. src/Application.cpp와 Graphics.cpp는 호환 include 래퍼다. 프레임워크 자체에 main은 없다. examples/basic.cpp와 assets.cpp의 main은 서로 다른 사용자 호출 예제다. 하나의 실행 프로젝트에 두 예제 main을 함께 넣지 않는다.
+프레임워크 자체에는 main이 없다. Framework.h → Application.h → Graphics.h/ModelLoader.h/Picking.h와 inline 구현을 포함한다. src의 두 cpp는 호환 include 래퍼다.
 
-basic.cpp의 main → Application 생성 → init/mouse/key 콜백 등록 → run. init 콜백은 addCuboid, setTransform, setOutline으로 장면을 만든다. mouse 콜백은 pickAt → getMesh → splitFace → setFaceSRT를 호출한다. assets.cpp의 main은 상대 모델·이미지 경로와 모드를 받아 config.features를 설정한다. init 콜백은 loadPrimitive 또는 loadModel → addModel로 장면을 만든다.
+- basic.cpp: Application 생성 → init/mouse/key 콜백 등록 → run. init은 addCuboid/setTransform/setOutline, 클릭은 pickAt/getMesh/splitFace/setFaceSRT를 호출한다.
+- assets.cpp: 모델·이미지 상대경로와 모드 입력 → config.features → init의 loadPrimitive 또는 loadModel → addModel → run 루프.
+- hierarchy.cpp: init/update/mouse/key 등록 → run. init은 세 addCuboid → setParent(지구,태양)/setParent(달,지구) → generateTexcoords → setMaterial. update는 태양·지구 local 회전을 변경한다. 클릭은 pickAt으로 ID와 UV를 출력한다.
 
-콜백 등록은 콜백 본문 실행이 아니다. detail/Application.inl의 Application::run은 initialize 성공 후 initHandler를 한 번 호출한다. initialize는 번들 GLFW의 창·GL 컨텍스트와 Renderer를 초기화하고 입력 콜백을 등록한다. PollEvents/WaitEvents가 키·마우스·커서·포커스 콜백을 실행한다. 프레임 루프는 updateHandler 뒤에 renderHandler 또는 renderScene을 호출한다. C 콜백 내부 예외는 dispatch가 보관하고 루프에서 다시 전달한다.
+각 main은 별도 실행 프로그램이다. 한 실행 프로젝트에 세 main을 동시에 넣지 않는다. 콜백 등록과 본문 실행은 별개다. Application::run은 initialize 후 initHandler를 한 번 호출한다. initialize는 GLFW DLL·창·current GL 컨텍스트·Renderer·입력 콜백을 설정한다. 프레임 루프는 updateHandler → renderHandler 또는 renderScene → SwapBuffers를 수행하며 GLFW 이벤트 처리가 입력 콜백을 실행한다. C 콜백 예외는 dispatch가 보관한다.
 
-## 데이터와 GPU
+## 모델과 UV 입력
 
-ModelLoader.h의 loadPrimitive는 기본 도형의 v/vt/vn/f를 읽는다. loadModel은 models 모드 분기를 확인한 다음 AssimpApi::load와 aiImportFile로 일반 모델을 읽는다. textures 모드가 꺼져 있으면 UV·이미지 작업을 생략한다. 모델이 DLL에서 반환된 뒤 프레임워크의 Model/Mesh에 복사되며 aiScene은 같은 DLL에서 해제된다.
+ModelLoader.h는 AssetLoader의 공개 선언, 프로젝트 내부 경로 제한, 이미지 캐시, finishLoad를 유지한다. PrimitiveLoader.inl의 loadPrimitive는 v/vt/vn/f·음수 인덱스·평면 다각형 삼각분할·공면 병합을 처리한다. AssimpLoader.inl의 loadModel은 models 분기 → AssimpApi::load → aiImportFile → 노드 순회 → 프레임워크 Model/Mesh 복사 → aiReleaseImport 순서다. 노드 변환은 정점에 반영한다. 노드 계층과 뼈대 애니메이션은 보존하지 않는다. DLL 내부는 AST 범위 밖이다.
 
-Model.h의 Model/Mesh/Vertex/Material → Scene.inl의 addModel 또는 setter 검증 → CPU 장면 상태와 geometryRevision. Scene::setModel은 Scene.inl:430, 검증 후 revision 갱신은 :439에 있다. Renderer.inl의 Renderer::render는 장면 주소·revision·textures 모드가 바뀌었는지 확인한 뒤 upload를 호출한다. upload는 임의 정점·인덱스를 GpuVertex/VBO/EBO에 묶고 드로우 범위를 만든다.
+Geometry.h의 generateTexcoords는 메시 복사 → 경계 상자 → 투영 방식 → 필요 시 삼각형 정점 분리 → UV scale/rotation/offset → 유효 메시 반영 순서다. Planar는 지정 축, Box는 삼각형 법선 주축, Cylindrical/Spherical은 Y축 둘레 경도를 사용한다. U 경계의 좌표는 반복 샘플링을 위해 1을 넘을 수 있다. Scene의 UV setter는 setMesh를 통해 geometryRevision을 갱신한다. 외곽선 위치·법선·색상은 유지하지만 정점 번호는 바뀔 수 있다.
 
-VAO 속성 pos/color/UV/normal → Shaders.h의 내장 GLSL → uMVP 정점 변환 → 삼각형 조립·클리핑·래스터화 → 이미지·선택적 조명 → 깊이 처리 → 프레임버퍼 → SwapBuffers. GLSL 파일은 내장 문자열의 별도 사본이며, 문자열 내부의 GLSL 호출 관계는 C++ AST 그래프에 별도 추출되지 않았다.
+Texture.h의 WIC 이미지 디코딩 → TextureImage RGBA → Material.texture → Renderer::upload의 GPU 이미지 업로드 → UV 보간 → sampler2D 샘플링 순서다. textures 옵션이 꺼지면 GPU 텍스처 경로는 생략한다. 이미지 경로는 loadTexture 또는 LoadOptions.texturePath로 지정한다. UV가 없는 파일에 텍스처를 지정한 로딩은 기존처럼 오류를 반환한다. 자동 매핑은 비텍스처 상태로 로딩한 뒤 명시적으로 수행한다.
 
-## 부모·면 변환과 피킹
+## 계층 좌표와 GPU
 
-Model.h의 detail::transformMatrix는 basis·회전·scale·localBasis를 합성한다. faceMatrix는 부모 행렬을 먼저 적용하고 분리 면의 추가 SRT를 뒤에 합성한다. splitFace/setFaceSRT는 Scene.inl:508/:515에 정의되어 있다. Scene.inl:372의 DiagramSystem::faceModelMatrix가 공통 진입점이다.
+Transform3D의 basis·Euler 회전·quaternion orientation·scale·localBasis → detail::transformMatrix. SRT가 정점에 적용되는 순서는 피벗 이동 → 크기 → 회전 → 위치이며 열벡터 행렬로 합성한다.
 
-Renderer.inl:157/:174와 Picking.h:47은 이 faceModelMatrix를 직접 호출한다. 따라서 피킹이 렌더링과 같은 변환을 사용하는 것은 소스에서 확인한 관계다. Application::pickAt은 창 좌표를 프레임버퍼 좌표로 바꾸고, screenRay는 카메라의 역 view/projection으로 광선을 만든다. pick은 표시된 삼각형들의 교점을 비교해 가까운 객체·면·삼각형을 반환한다. 변환 분기와 수학의 실제 동작은 사용자가 테스트해야 한다.
+Scene::setParent는 ID와 부모 사슬을 읽어 자기 연결·순환을 거부한다. worldMatrix는 현재 물체부터 조상을 순회하며 부모 local × 누적 local을 계산한다. faceWorldMatrix는 부모 world × 물체 및 면 행렬을 계산한다. parent는 Component_Poly 마지막 필드이므로 기존 중괄호 초기화 순서를 유지한다. 연결·해제는 local 값을 유지하며 world 값을 유지하지 않는다.
+
+Renderer::draw와 Picking.h의 pick은 Scene::faceWorldMatrix를 호출한다. owner가 있는 선은 Scene::worldMatrix를 사용한다. 따라서 물체·면·선·교점이 같은 계층 변환을 사용하는 것은 소스에서 확인한 관계다. 부모 visibility는 자식에게 상속하지 않는다.
+
+CPU Scene 형상 → Renderer::render의 scene/revision/textures 비교 → 필요한 경우 upload → GpuVertex 위치/색/UV/법선 → VBO/EBO/VAO → draw 범위별 world 행렬 → uMVP=projection×view×world → 셰이더 → 래스터화·텍스처·선택적 조명·깊이 처리 → 프레임버퍼. 법선 행렬은 누적 world의 inverse-transpose다. 변환과 부모 연결만 바뀌면 형상 버퍼를 다시 올리지 않는다.
+
+Application::pickAt은 창 좌표를 프레임버퍼 좌표로 바꾸고 screenRay는 카메라 view/projection의 역변환을 사용한다. pick은 표시된 삼각형의 월드 교점을 비교하고 가까운 ID·면·삼각형·로컬 좌표·무게중심·UV를 반환한다. 선과 텍스처 투명 픽셀은 판정하지 않는다.
 
 ## 종료와 분석 한계
 
-run 정상 종료 또는 예외 → shutdown → AssetLoader 해제 → current 컨텍스트에서 Renderer::destroy → 창 해제 → GLFW 종료. Native.h의 DLL 연결은 함수 포인터다. AST에서 DLL export의 실행 내부를 추적하지 않는다. COM/WIC와 그래픽 드라이버 내부 동작도 그래프 범위 밖이다.
+run 종료·예외 → shutdown → AssetLoader 해제 → current 컨텍스트에서 Renderer::destroy → 창 해제 → GLFW 종료. COM/WIC·DLL export·GPU 드라이버 내부는 그래프 범위 밖이다.
 
-GRAPH_HEALTH.md에는 해석되지 않은 관계 48개, 방향 관계의 같은 끝점 병합 77개가 기록된다. 원시 EXTRACTION.json은 추출된 관계를 보존한다. graph.json의 엣지 개수를 모든 호출 위치의 개수로 해석하지 않는다. INFERRED 관계는 보고서에 별도로 표시된다.
+현재 그래프: 643 노드, 1013 방향 관계, 26 커뮤니티. 원시 추출에 해석되지 않은 끝점 관계 49개, 같은 끝점 방향 관계 병합 82개가 있다. GRAPH_HEALTH.md와 EXTRACTION.json에 기록한다. 그래프 관계 수를 모든 실제 호출 횟수로 해석하지 않는다. 실행 정확성은 사용자 테스트가 필요하다. 벤치마크는 실행하지 않았다.
```

