# 소스 구조와 실행 프로젝트

graphify AST: 3599 nodes / 6739 edges / 141 source files.

코드 전용 AST 추출이며 의미 분석 API 토큰 사용은 0이다. 코드 관계를 소스와 함께 읽는 자료다. 빌드·테스트·그래프 진단기·인코딩 검증기·앱·브라우저를 실행하지 않았다. 동적 콜백의 실제 실행이나 화면을 입증하지 않는다.

## 현재 Standard

1. main이 Console과 Application을 만들고 초기화·키·마우스·업데이트·렌더 콜백을 등록한다. 등록은 실행이 아니다.
2. Application::run 초기화 후 Console::initScene → SceneAuthoring::clear → configureScene → addObject/addLayer/addLight/setCamera → update(0).
3. 입력은 Application의 이벤트 전달 뒤 Console::onKey/onMouse로 들어온다. 클릭은 pickAt의 결과 EntityId를 이름으로 바꾸어 출력한다. 명령 파서와 assembly 런타임은 없다.
4. Console::onUpdate가 절대 시간을 계산해 SceneAuthoring::update를 호출한다. 지역 레이어를 평가한 뒤 Scene::worldMatrix로 부착 광원·카메라를 평가한다.
5. Application의 기존 Scene 모션 갱신 후 Console::onRender → Application::renderScene → Renderer의 CPU 정점/인덱스 업로드와 VAO/VBO/EBO·uniform·draw 호출로 이어진다.
6. Shaders.h 또는 외부 VertexShader/FragmentShader가 모델/정규행렬·조명·재질·텍스처를 처리한다. 프레임 종료 뒤 run이 GPU·창·GLFW를 정리한다.

## 별도 debugger

1. wmain은 UTF-8 콘솔을 설정하고 인수 또는 project 명령으로 대상 폴더를 고른다. 독립 실행 프로그램이며 Standard의 메모리/렌더러를 제어하지 않는다.
2. command는 문자열을 토큰화하고 SourceIndex::refresh로 실제 디스크 소스를 다시 읽는다. 시작/끝 구획과 현행 행 번호를 만든다.
3. set/parent는 유일한 현재 대입/등록 줄을 찾고, add는 실제 @debug insert 앞의 전체 C++ 삽입안을 만든다. find/code는 일반 소스 위치를 제공한다.
4. proposal은 실제 before와 생성한 after, 경로·행·주변 코드를 stdout에 출력한다. 대상 파일 저장·프로세스 실행·컴파일 호출은 없다.
5. EOF 또는 quit로 끝난다. Windows 파일 경로/콘솔 API와 C++ 표준 기능만 사용하며 OpenGL SDK 링크는 없다.

## 전체 독립 프로젝트와 진입점

각 행은 별도 실행 대상이다. 같은 이름의 Console/main을 하나의 호출 흐름으로 합치지 않는다. old_standard는 보존한 과거 프로젝트로 등록만 기록하고 현재 AST 추출에서는 제외한다. 프레임워크 예제의 main도 동시에 컴파일하지 않는다.

| 프로젝트/독립 예제 | 확인한 진입점 |
|---|---|
| debugger/debugger.vcxproj | debugger/main.cpp:8 wmain |
| old_standard/standard.vcxproj | old_standard/main.cpp:6 main |
| openGL_2dAssign11/openGL_sample2.vcxproj | openGL_2dAssign11/main.cpp:109 main |
| openGL_2dAssign12/openGL_sample2.vcxproj | openGL_2dAssign12/main.cpp:59 main |
| openGL_2dAssign_10/openGL_sample2.vcxproj | openGL_2dAssign_10/main.cpp:86 main |
| openGL_2dAssign_789/openGL_sample2.vcxproj | openGL_2dAssign_789/main.cpp:103 main |
| openGL_3dAssign_13/openGL_sample2.vcxproj | openGL_3dAssign_13/main.cpp:4 main |
| openGL_3dAssign_14/openGL_sample2.vcxproj | openGL_3dAssign_14/main.cpp:4 main |
| openGL_3dAssign_15/openGL_sample2.vcxproj | openGL_3dAssign_15/main.cpp:5 main |
| openGL_3dAssign_16/openGL_assign_16.vcxproj | openGL_3dAssign_16/main.cpp:5 main |
| openGL_3dAssign_17/openGL_assign_17.vcxproj | openGL_3dAssign_17/main.cpp:4 main |
| openGL_3dAssign_18/openGL_assign_18.vcxproj | openGL_3dAssign_18/main.cpp:4 main |
| openGL_3dAssign_19/openGL_assign_19.vcxproj | openGL_3dAssign_19/main.cpp:4 main |
| openGL_3dAssign_20/openGL_assign_20.vcxproj | openGL_3dAssign_20/main.cpp:5 main |
| openGL_3dAssign_21/openGL_assign_21.vcxproj | openGL_3dAssign_21/main.cpp:5 main |
| openGL_3dAssign_22/openGL_assign_22.vcxproj | openGL_3dAssign_22/main.cpp:5 main |
| openGL_3dAssign_23/openGL_assign_23.vcxproj | openGL_3dAssign_23/main.cpp:5 main |
| openGL_3dAssign_24/openGL_assign_24.vcxproj | openGL_3dAssign_24/main.cpp:5 main |
| openGL_3dAssign_26/openGL_assign_26.vcxproj | openGL_3dAssign_26/main.cpp:5 main |
| openGL_3dAssign_27/openGL_assign_27.vcxproj | openGL_3dAssign_27/main.cpp:5 main |
| openGL_assign123456/openGL_sample2.vcxproj | openGL_assign123456/main.cpp:79 main |
| standard/standard.vcxproj | standard/main.cpp:5 main |
| framework/examples/basic.cpp | framework/examples/basic.cpp main |
| framework/examples/assets.cpp | framework/examples/assets.cpp main |
| framework/examples/motion.cpp | framework/examples/motion.cpp main |
| framework/examples/source_authoring.cpp | framework/examples/source_authoring.cpp main |

## God Nodes

AST 연결 차수다. 이름 중복과 동적 호출을 고려해 실행 순서로 해석하지 않는다.

| 노드 | 연결 차수 | 소스 |
|---|---:|---|
| .failAt() | 167 | framework/include/Diagnostics.h |
| Application | 95 | framework/include/Application.h |
| GlApi | 90 | framework/detail/Native.h |
| Scene | 76 | framework/include/Graphics.h |
| Scene | 76 | standard/framework/include/Graphics.h |
| RectState.cpp | 76 | openGL_assign123456/RectState.cpp |
| framework/detail/Scene.cpp | 74 | framework/detail/Scene.cpp |
| standard/framework/detail/Scene.cpp | 72 | standard/framework/detail/Scene.cpp |

## Surprising Connections

- 렌더와 피킹은 Scene의 월드/면 행렬을 공유한다. 새 작성 API는 그 앞의 attachment와 지역 레이어만 구성한다. 별도 피킹 좌표 계산기를 추가하지 않았다.
- 광원·카메라 부착은 기존 Scene 월드 행렬과 CameraSystem::attached를 재사용한다. C++ 구획 주석은 GPU나 실행 상태에 참여하지 않는다.
- debugger는 프레임워크를 링크하지 않지만 생성 코드가 실제 공개 API를 호출한다. SourceIndex는 컴파일러의 활성 분기나 AST 의미를 판정하지 않는다.

## Suggested Questions

- 객체의 월드 배치를 지정한 뒤 부모의 지역 레이어가 회전하면 자식과 광원은 어떤 행렬을 상속하는가?
- source 파일에 빈 줄을 넣었을 때 debugger의 before와 행 번호는 어디서 다시 계산되는가?

## 산출물과 미확인 범위

graph.json/graph.html/EXTRACTION.json/PROJECTS.json/SOURCE_MANIFEST.json을 함께 보존한다. 빌드·런타임·화면·폴더 분리 실행 확인은 standard/USER_CHECKS.md에 따라 사용자가 수행한다.
