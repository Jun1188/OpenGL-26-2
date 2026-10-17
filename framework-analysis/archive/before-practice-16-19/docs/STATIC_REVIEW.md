# 정적 검토와 번들 출처

확인 범위는 소스·파일·DLL 메타데이터다. 작성자는 컴파일, 링크, 테스트 프로그램 또는 OpenGL 실행을 수행하지 않았다. 실행 결과는 USER_CHECKS.md에 따라 사용자가 확인한다.

## 확인한 자료

- 직접 작성한 C++ 헤더/inl/호환 cpp/예제 17개를 tree-sitter C++로 구문 분석했다. 오류 노드와 누락 노드가 없었다. 이는 MSVC의 전처리·타입 검사·링크 확인이 아니다.
- Framework.h부터 상대 include로 연결된 250개 파일을 조사했다. 찾지 못한 상대 include와 외부 assimp/glm/GLFW/GLEW include 경로는 없었다. 조건부 코드와 Windows SDK·표준 라이브러리의 적합성은 사용자 빌드에서 확인해야 한다.
- Shaders.h의 기본 GLSL 문자열은 shaders의 두 파일과 일치한다. GLSL 컴파일은 실행하지 않았다.
- 작성·추가·수정한 텍스트는 UTF-8 BOM이다. 바이너리에는 인코딩 변환을 적용하지 않았다.
- Assimp C API 5개와 GLFW 입력·창 함수의 DLL export를 확인했다. 함수 포인터의 실제 호출은 실행하지 않았다.
- Assimp DLL의 /DEPENDENTS는 Windows·Microsoft VC 런타임을 표시했다. 별도 Assimp/zlib DLL 의존은 표시되지 않았다. 현재 PC나 배포 PC의 런타임 동작은 확인하지 않았다.
- 각 구현 단계의 커밋 파일은 루트 framework 안에 한정했다. 동시에 진행된 다른 작업의 변경과 커밋은 보존한다.

자세한 파일 수·SHA-256·원본 대조 결과는 STATIC_EVIDENCE.json에 있다.

## Assimp 번들

출처는 사용자가 지정한 Documents/GitHub/assimp의 기존 include 및 build 산출물이다. 이번 작업에서 Assimp를 빌드하지 않았다. 생성 revision.h는 버전 6.0.5를 표시한다. DLL과 LIB는 원본 build/bin 및 build/lib 파일과 바이트가 같다. build/config.h와 revision.h는 원본 빌드의 생성 헤더에서 가져왔다.

framework/dependencies/assimp/include/assimp 안의 SDK include 경로를 같은 번들 안의 상대 include로 바꿨다. 텍스트에는 BOM을 적용했다. 라이선스는 dependencies/assimp/LICENSE에 보존했다. DLL은 assimp-vc145-mt.dll이며 Ninja의 기존 컴파일 플래그는 -MD다. 이것은 원본 빌드 설정의 관찰이다. 새 Debug/Release 프로젝트와의 호환을 실제 링크로 확인한 것은 아니다.

프레임워크는 import LIB를 링크하지 않고 C API를 GetProcAddress로 연결한다. Assimp가 소유한 aiScene은 같은 DLL의 aiReleaseImport로 해제한다. 프레임워크 데이터는 별도 Model/Mesh로 복사한다. aiScene이나 DLL에서 생성한 STL 객체를 사용자에게 넘기지 않는다. LIB는 번들의 원본 산출물로 함께 보존했다.

## 요구 사항과 코드 위치

| 요구 | 구현 위치 | 사용자 관찰 지점 |
| --- | --- | --- |
| 기본 도형 / 일반 모델 분리 | ModelLoader.h의 loadPrimitive/loadModel | features 비활성 오류, cube 텍스트와 실제 모델 비교 |
| 모델·텍스처 옵트인 | FeatureOptions, Application::initialize | 기본 모드의 Assimp 미로드, textures 사용 시 UV·이미지 |
| 상대 자산 경로 | AssetLoader::resolve, projectRoot | 작업 폴더 변경·한글 경로·루트 밖 경로 오류 |
| 하단 중점 | Model.h bottomCenter, Scene::addModel/setLocalBasis | cube 기준점 (0,-0.5,0), basis에 바닥 배치 |
| 일반 메시 편집 | Scene.inl의 setModel/setMesh와 속성 setter | 수정 후 GPU 갱신, 실패 입력의 기존 상태 유지 |
| 부모 SRT와 분리 면 합성 | Model.h faceMatrix, splitFace/setFaceSRT | 분리 직후 항등, 부모 재이동 후 면 추종 |
| 선택적 외곽선 | Geometry.h meshOutline, Renderer.inl | 공면 내부 대각선 제거, Explicit 인덱스 |
| 클릭 피킹 | Picking.h, Application::pickAt | 가까운 면·비균일 SRT·DPI·직교/원근 |
| include 진입점 | Framework.h, Native.h, inline 구현 | 빈 프로젝트·여러 번역 단위·배포 폴더 |

## 범위의 한계

Windows x64의 정적 삼각형 모델과 WIC 지원 이미지를 대상으로 한다. 스키닝·애니메이션, 투명 픽셀 피킹, 선/축 피킹, 프레임워크의 다른 플랫폼 포팅은 포함하지 않는다. 원본 도형을 별도 월드 객체로 떼는 방식도 제공하지 않는다. 분리 면은 원본 모델 아래의 추가 변환이다.

참고 저장소의 텍스처·파이프라인 예제는 입력/좌표/재질 흐름의 참고 자료다. 새 코드의 빌드·실행 증거로 사용하지 않았다.

확인 질문: 부모가 비균일 scale과 회전을 가진 상태에서 면의 추가 SRT를 항등으로 설정하면, 분리 전과 후의 월드 정점은 왜 같아야 하는가? Model.h의 faceMatrix 순서를 기준으로 설명할 수 있다.
