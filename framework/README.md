# 재사용 가능한 OpenGL 프레임워크

Windows x64, C++17, OpenGL 3.3. framework 폴더 전체를 복사하고 `#include "framework/Framework.h"`를 사용한다. Framework.props는 include 경로와 실행 DLL·자산 배치를 제공한다. 프레임워크 구현 .cpp 등록과 외부 SDK 설정은 필요 없다.

## 변환

Transform3D는 basis(부모 좌표의 위치), localBasis(로컬 피벗), rotation(항등값 mat3), scale을 가진다. 행렬은 `T(basis) * R * S * T(-localBasis)`다. 쿼터니언·Euler 병행 표현은 없다. 로컬 자전은 rotation의 오른쪽에 회전행렬을 곱한다. 공전·부품 연결은 addGroup/setParent를 사용한다. 연결·해제는 로컬 값을 유지하며 월드 위치는 달라질 수 있다. 순환 계층은 거부한다.

초기 표시 기울기는 Camera의 position/target/up으로 지정한다. 모델 정점·로컬 기저·입력 회전에는 초기 표시 각도를 넣지 않는다. 렌더링·법선·선·피킹은 같은 계층 행렬을 사용한다.

## 공통 API

- Scene: addGroup, addCuboid, addPyramid, addRightTetrahedron, addSphere, addCone, addModel.
- DiagramSystem: makePolyhedron, makeSphere, makeCone. 구는 중심 원점, 원뿔은 Y축 중심이다. 크기는 양수, 구 slices 3~512/stacks 2~256, 원뿔 slices 3~512다. 잘못된 값은 빈 모델/빈 optional을 반환한다.
- setTransform/setPosition/setParent와 worldMatrix/faceWorldMatrix: 로컬·월드 변환.
- splitFace/setFaceSRT/setFaceLocalMatrix: 부모를 따르는 면 피벗과 추가 변환.
- setModel/setMesh/setMeshVertices/setVertexPosition/setTriangleIndices: 형상 편집.
- setTint: 자체 색상 배율. 자식과 선에는 전파하지 않으며 GPU 정점 버퍼를 다시 업로드하지 않는다.
- setMaterial/setTexcoords/generateTexcoords/transformTexcoords/setOutline: 재질·UV·경계선.
- addLine/addBasis: 그룹을 owner로 지정하면 선도 계층 변환을 따른다.
- addGroup은 보이지 않는 변환 노드다. 일반 addModel/push_entity는 빈 모델을 거부한다. setModel로 그룹에 유효한 모델을 지정하면 일반 엔티티가 된다. 그룹은 BottomCenter 피벗을 사용할 수 없다.

AssetLoader::loadPrimitive는 도형 텍스트·OBJ 부분집합, loadModel은 Assimp 정적 모델을 읽는다. models/textures는 ApplicationConfig.features에서 명시적으로 켠다. 뼈대 애니메이션은 제공하지 않는다. UV는 Planar/Box/Cylindrical/Spherical 투영을 지원한다.

## 상대경로와 배포

ApplicationConfig.projectRoot 기본값 `.`은 실행 파일 폴더다. runtimeDirectory 기본값 `framework`와 선택적 shaderDirectory는 projectRoot 내부 상대경로다. 자산 인자는 projectRoot 내부 상대경로여야 한다. 절대경로·루트 이탈은 거부한다. 기본 셰이더는 내장되어 있다.

Framework.props는 DLL을 실행 폴더의 framework/dependencies에, 선택적 셰이더를 framework/shaders에 복사한다. data/models/textures는 실행 폴더의 같은 상대경로에 복사한다. 다른 작업 폴더에서 실행해도 소스 트리 탐색을 하지 않는다. 직접 배포할 때도 이 배치를 유지한다.

운영체제의 Windows SDK·OpenGL 드라이버·VC 런타임은 별도 실행 환경이다. 기존 실습용 LegacyOpenGL.props는 내부 GLEW/GLFW import library를 사용한다.

## 수명과 오류

Application은 한 스레드에서 실행 루프와 컨텍스트를 소유한다. getAssets는 run 콜백 동안 유효하다. 콜백 캡처 객체는 run 종료까지 유지한다. 예외는 C 콜백 밖으로 직접 전달하지 않고 정리 후 다시 전달한다. Renderer 직접 사용 시 current 컨텍스트에서 initialize/render/destroy를 호출하고 컨텍스트를 마지막에 해제한다.

setter의 false와 로더의 error를 확인한다. clear 뒤에는 ID가 재사용되므로 보관한 ID를 폐기한다. getComponent/getMesh 포인터는 장면 변경 뒤 다시 조회한다.

[응용 패턴](docs/APPLICATION_PATTERNS.md), [사용자 확인](docs/USER_CHECKS.md). 빌드·테스트는 사용자가 수행한다. 이전 변경 보고는 저장소의 framework-analysis/archive에 보관한다.
