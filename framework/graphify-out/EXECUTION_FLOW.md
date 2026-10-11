# 실행 프로젝트와 데이터 흐름

현재 .vcxproj 12개에서 진입점 15개를 확인했다. 조건부 main은 각각 기록했다. 원본 프로젝트는 읽기만 했다. PROJECTS.json은 소스 호출 위치와 전처리 지시문을 보존한다. 전처리·빌드·실행 없이 실제 선택된 분기나 동적 호출 순서를 확정하지 않는다.

루트 framework는 실행 프로젝트가 아니다. 과제 13·14·15는 각 프로젝트 안의 기존 framework 사본을 사용한다. 이번 작업으로 이 사본이나 프로젝트 연결을 바꾸지 않았다. 새 루트 framework 그래프를 기존 과제의 실행 흐름으로 합치지 않는다.

## Computer-Graphics_WarmmingUp2/Computer-Graphics_WarmmingUp2.vcxproj

- `Computer-Graphics_WarmmingUp2/FileName.cpp:43`의 `wmain`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `arguments.push_back` L47, `toUtf8` L47, `outputDirectory` L48, `application.run` L49, `error.what` L51.

## Computer-Graphics_WarmmingUp2/tests/WarmupTests.vcxproj

- `Computer-Graphics_WarmmingUp2/tests/Tests.cpp:199`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `testBoardTransactions` L201, `testCommands` L202, `testBoardProperties` L203, `testMesh` L204, `error.what` L208.

## computer_graphics_warmUp1/computer_graphics_warmUp1.vcxproj

- `computer_graphics_warmUp1/FileName.cpp:534`의 `main`. 조건: #if PROB == 1
  직접 호출 위치: `uid` L539, `matrix.command` L543.
- `computer_graphics_warmUp1/FileName.cpp:755`의 `main`. 조건: #if PROB == 1 → #elif PROB >= 2 && PROB <= 4 → #if PROB == 2
  직접 호출 위치: `std::getline` L758, `editor.load` L760, `editor.run` L760.
- `computer_graphics_warmUp1/FileName.cpp:873`의 `main`. 조건: #if PROB == 1 → #elif PROB >= 2 && PROB <= 4 → #if PROB == 2 → #elif PROB == 3
  직접 호출 위치: `list.run` L873.
- `computer_graphics_warmUp1/FileName.cpp:998`의 `main`. 조건: #if PROB == 1 → #elif PROB >= 2 && PROB <= 4 → #if PROB == 2 → #elif PROB == 3 → #elif PROB == 4
  직접 호출 위치: `game.run` L998.

## openGL_2dAssign11/openGL_sample2.vcxproj

- `openGL_2dAssign11/main.cpp:96`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `readGridSize` L98, `glfwInit` L100, `glfwWindowHint` L105, `glfwWindowHint` L106, `glfwWindowHint` L107, `glfwWindowHint` L108, `glfwCreateWindow` L110, `glfwTerminate` L115, `glfwMakeContextCurrent` L119, `glfwSwapInterval` L120, `glewInit` L123, `glfwDestroyWindow` L125, `glfwTerminate` L126, `findShaderDirectory` L130, `shaderDirectory.empty` L131, `glfwDestroyWindow` L133, `glfwTerminate` L134, `glfwGetFramebufferSize` L140, `framebufferSizeCallback` L141, `createConsole` L144, `console->initGraphics` L145, `glfwDestroyWindow` L146, `glfwTerminate` L147, `glfwSetWindowUserPointer` L151, `console.get` L151, `glfwSetKeyCallback` L152, `glfwSetFramebufferSizeCallback` L153, `glfwGetTime` L156, `glfwWindowShouldClose` L157, `glfwGetTime` L158, `static_cast<float>` L159, `console->update` L165, `console->render` L166, `glfwSwapBuffers` L170, `glfwPollEvents` L171, `console->shutdownGraphics` L174, `console.reset` L175, `glfwDestroyWindow` L176, `glfwTerminate` L177.

## openGL_2dAssign12/openGL_sample2.vcxproj

- `openGL_2dAssign12/main.cpp:53`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `glfwInit` L54, `glfwWindowHint` L59, `glfwWindowHint` L60, `glfwWindowHint` L61, `glfwWindowHint` L62, `glfwCreateWindow` L64, `glfwTerminate` L69, `glfwMakeContextCurrent` L73, `glfwSwapInterval` L74, `glewInit` L77, `glfwDestroyWindow` L79, `glfwTerminate` L80, `findShaderDirectory` L84, `shaderDirectory.empty` L85, `glfwDestroyWindow` L87, `glfwTerminate` L88, `glfwGetFramebufferSize` L94, `framebufferSizeCallback` L95, `createConsole` L98, `console->initGraphics` L99, `glfwDestroyWindow` L100, `glfwTerminate` L101, `glfwSetWindowUserPointer` L105, `console.get` L105, `glfwSetKeyCallback` L106, `glfwSetFramebufferSizeCallback` L107, `glfwGetTime` L110, `glfwWindowShouldClose` L111, `glfwGetTime` L112, `static_cast<float>` L113, `console->update` L119, `console->render` L120, `glfwSwapBuffers` L124, `glfwPollEvents` L125, `console->shutdownGraphics` L128, `console.reset` L129, `glfwDestroyWindow` L130, `glfwTerminate` L131.

## openGL_2dAssign_10/openGL_sample2.vcxproj

- `openGL_2dAssign_10/main.cpp:83`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `glfwInit` L84, `glfwWindowHint` L89, `glfwWindowHint` L90, `glfwWindowHint` L91, `glfwWindowHint` L92, `glfwCreateWindow` L94, `glfwTerminate` L99, `glfwSetWindowAspectRatio` L104, `glfwMakeContextCurrent` L106, `glfwSwapInterval` L107, `glewInit` L110, `glfwDestroyWindow` L112, `glfwTerminate` L113, `findShaderDirectory` L117, `shaderDirectory.empty` L118, `glfwDestroyWindow` L120, `glfwTerminate` L121, `glfwGetFramebufferSize` L127, `glViewport` L128, `createConsole` L130, `console->initGraphics` L131, `console->shutdownGraphics` L132, `glfwDestroyWindow` L133, `glfwTerminate` L134, `glfwSetWindowUserPointer` L138, `console.get` L138, `glfwSetKeyCallback` L139, `glfwSetMouseButtonCallback` L140, `glfwSetCursorPosCallback` L141, `glfwSetFramebufferSizeCallback` L142, `glfwGetTime` L145, `glfwWindowShouldClose` L146, `glfwGetTime` L147, `static_cast<float>` L148, `console->update` L154, `console->render` L155, `glfwSwapBuffers` L159, `glfwPollEvents` L160, `console->shutdownGraphics` L163, `console.reset` L164, `glfwDestroyWindow` L165, `glfwTerminate` L166.

## openGL_2dAssign_789/openGL_sample2.vcxproj

- `openGL_2dAssign_789/main.cpp:95`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `glfwInit` L96, `glfwWindowHint` L101, `glfwWindowHint` L102, `glfwWindowHint` L103, `glfwWindowHint` L104, `glfwCreateWindow` L106, `glfwTerminate` L111, `glfwMakeContextCurrent` L115, `glfwSwapInterval` L116, `glewInit` L119, `glfwDestroyWindow` L121, `glfwTerminate` L122, `findShaderDirectory` L126, `shaderDirectory.empty` L127, `glfwDestroyWindow` L129, `glfwTerminate` L130, `glfwGetFramebufferSize` L136, `glViewport` L137, `createConsole` L139, `console->initGraphics` L140, `glfwDestroyWindow` L141, `glfwTerminate` L142, `glfwSetWindowUserPointer` L146, `console.get` L146, `glfwSetKeyCallback` L147, `glfwSetMouseButtonCallback` L148, `glfwSetCursorPosCallback` L149, `glfwSetFramebufferSizeCallback` L150, `glfwGetTime` L153, `glfwWindowShouldClose` L154, `glfwGetTime` L155, `static_cast<float>` L156, `console->update` L162, `console->render` L163, `glfwSwapBuffers` L167, `glfwPollEvents` L168, `console->shutdownGraphics` L171, `console.reset` L172, `glfwDestroyWindow` L173, `glfwTerminate` L174.

## openGL_3dAssign_13/openGL_sample2.vcxproj

- `openGL_3dAssign_13/main.cpp:4`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `graphics::Application::findShaderDirectory` L9, `config.shaderDirectory.empty` L10, `application.setInitHandler` L14, `practice.initScene` L15, `application.setKeyHandler` L17, `practice.onKey` L18, `application.run` L20.

## openGL_3dAssign_14/openGL_sample2.vcxproj

- `openGL_3dAssign_14/main.cpp:4`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `graphics::Application::findShaderDirectory` L9, `config.shaderDirectory.empty` L10, `application.setInitHandler` L14, `practice.initScene` L15, `application.setModifiedKeyHandler` L17, `practice.onKey` L18, `application.setUpdateHandler` L20, `practice.onUpdate` L21, `application.setFocusHandler` L23, `practice.onFocus` L24, `application.run` L26.

## openGL_3dAssign_15/openGL_sample2.vcxproj

- `openGL_3dAssign_15/main.cpp:4`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `graphics::Application::findShaderDirectory` L9, `config.shaderDirectory.empty` L10, `application.setInitHandler` L14, `practice.initScene` L15, `application.setKeyHandler` L17, `practice.onKey` L18, `application.setUpdateHandler` L20, `practice.onUpdate` L21, `application.run` L23.

## openGL_assign123456/openGL_sample2.vcxproj

- `openGL_assign123456/main.cpp:75`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `glfwInit` L76, `glfwWindowHint` L81, `glfwWindowHint` L82, `glfwWindowHint` L83, `glfwCreateWindow` L85, `glfwTerminate` L95, `glfwMakeContextCurrent` L99, `glfwSwapInterval` L100, `glewInit` L103, `glfwDestroyWindow` L105, `glfwTerminate` L106, `glfwGetFramebufferSize` L112, `glViewport` L113, `createConsole` L115, `glfwSetWindowUserPointer` L116, `console.get` L116, `glfwSetKeyCallback` L117, `glfwSetMouseButtonCallback` L118, `glfwSetCursorPosCallback` L119, `glfwSetFramebufferSizeCallback` L120, `glfwGetTime` L122, `glfwWindowShouldClose` L124, `glfwGetTime` L125, `static_cast<float>` L126, `console->update` L132, `console->render` L133, `glfwSwapBuffers` L134, `glfwPollEvents` L135, `console.reset` L138, `glfwDestroyWindow` L139, `glfwTerminate` L140.

## openGL_sample0/openGL_sample0.vcxproj

- `openGL_sample0/main.cpp:258`의 `main`. 조건: 추출된 상위 전처리 조건 없음
  직접 호출 위치: `glfwSetErrorCallback` L259, `glfwInit` L260, `glfwWindowHint` L265, `glfwWindowHint` L266, `glfwWindowHint` L267, `glfwWindowHint` L268, `glfwCreateWindow` L270, `glfwTerminate` L278, `glfwMakeContextCurrent` L282, `glfwSetFramebufferSizeCallback` L283, `glfwSwapInterval` L284, `glewInit` L287, `glewGetErrorString` L290, `glfwDestroyWindow` L291, `glfwTerminate` L292, `glGetError` L297, `glfwGetFramebufferSize` L301, `glViewport` L302, `createShaderProgram` L304, `glfwDestroyWindow` L306, `glfwTerminate` L307, `loadMeshVertices` L314, `glDeleteProgram` L315, `glfwDestroyWindow` L316, `glfwTerminate` L317, `glGenVertexArrays` L323, `glGenBuffers` L324, `glBindVertexArray` L326, `glBindBuffer` L327, `glBufferData` L328, `static_cast<GLsizeiptr>` L330, `vertices.size` L330, `vertices.data` L331, `glVertexAttribPointer` L335, `reinterpret_cast<const void*>` L336, `glEnableVertexAttribArray` L337, `glVertexAttribPointer` L338, `reinterpret_cast<const void*>` L344, `glEnableVertexAttribArray` L345, `glBindBuffer` L347, `glBindVertexArray` L348, `glGetUniformLocation` L350, `glGetUniformLocation` L351, `glGetUniformLocation` L352, `glGetUniformLocation` L353, `glDeleteBuffers` L357, `glDeleteVertexArrays` L358, `glDeleteProgram` L359, `glfwDestroyWindow` L360, `glfwTerminate` L361, `glEnable` L365, `glEnable` L366, `glCullFace` L367, `glFrontFace` L368, `glGetString` L370, `glfwGetTime` L376, `transformBounds` L379, `glm::normalize` L380, `glm::vec3` L380, `glfwWindowShouldClose` L382, `glfwPollEvents` L383, `glfwGetTime` L384, `static_cast<float>` L385, `std::clamp` L385, `glfwGetKey` L388, `glfwSetWindowShouldClose` L389, `glfwWindowShouldClose` L391, `glfwGetFramebufferSize` L393, `glfwWaitEventsTimeout` L395, `glfwGetTime` L396, `static_cast<float>` L399, `updateCamera` L401, `std::fmod` L402, `glm::radians` L403, `glm::radians` L403, `glm::rotate` L404, `glm::mat4` L405, `glm::normalize` L407, `glm::vec3` L407, `transformBounds` L408, `intersectsObject` L410, `glm::lookAt` L415, `glm::perspective` L417, `glm::radians` L418, `glClearColor` L420, `glClear` L421, `glUseProgram` L423, `glUniformMatrix4fv` L424, `glm::value_ptr` L424, `glUniformMatrix4fv` L425, `glm::value_ptr` L425, `glUniformMatrix4fv` L426, `glm::value_ptr` L426, `glUniform3fv` L427, `glm::value_ptr` L427, `glBindVertexArray` L428, `glDrawArrays` L431, `static_cast<GLsizei>` L434, `vertices.size` L434, `glfwSwapBuffers` L436, `glBindVertexArray` L439, `glUseProgram` L440, `glDeleteBuffers` L441, `glDeleteVertexArrays` L442, `glDeleteProgram` L443, `glfwDestroyWindow` L445, `glfwTerminate` L446.

## 프로젝트별 흐름 해석

- warmUp1: PROB=1은 Matrix 생성 → 난수 입력 → Matrix::command. PROB=2는 StringEditor 생성 → 파일 입력/load → run. PROB=3은 PointList::run. PROB=4는 CardGame::run. 클래스는 같은 FileName.cpp에 정의되어 있다. 네 main은 동시에 실행되지 않는다.
- WarmmingUp2: FileName.cpp의 wmain → Console 생성 → UTF-8 인자 변환 → Application::run(Application.cpp). 명령과 보드/메시 처리 책임은 BoardCommand, MeshParser/Validator, BoardRenderer 등으로 분리된다. tests 프로젝트의 main은 테스트 함수 호출 진입점이며 이번 작업에서 실행하지 않았다.
- assign123456: main의 createConsole이 PROB=2..6별 Console 구현을 고른다. 창/컨텍스트/GLEW 초기화 → 콘솔 생성 → GLFW 입력·크기 콜백 등록 → 프레임의 update/render → swap/poll → 콘솔·창 정리. Console.cpp와 RectState.cpp가 상태·도형 처리에 참여한다.
- 2dAssign_789: PROB=7..9의 콘솔 분기를 사용한다. 초기화 → initGraphics → 콜백 등록 → update/render → swap/poll → shutdownGraphics → 창/GLFW 종료. DiagramState.cpp와 Console.cpp가 메시와 동작을 담당한다.
- 2dAssign_10·11·12: 각각 별도 main과 콘솔을 사용한다. 11은 그리드 크기를 먼저 입력한다. 이벤트 등록은 즉시 onKey/onMouse 실행을 뜻하지 않는다. GLFW poll이 입력 콜백을 실행하고 루프가 update/render를 호출한다. VAO/VBO/EBO·셰이더의 생성과 해제는 각 콘솔의 그래픽 초기화/종료 책임이다.
- sample0: main → GLFW/GLEW·셰이더 초기화 → loadMeshVertices(MeshParser/Validator) → position/normal VBO와 VAO → 반복 프레임의 모델/뷰/투영·조명 uniform → draw → GPU·창 정리. 이 프로젝트의 메시 파서는 이번 로더와 별개다.
- 3dAssign_13: main → Application/Console 객체 생성 → 셰이더 경로 확인 → init/key 콜백 등록 → 해당 프로젝트 framework의 Application::run. initScene과 onKey는 Console.cpp에서 실행된다.
- 3dAssign_14: 같은 소유 관계에 init/modifiedKey/update/focus 콜백을 등록한다. 실행 루프가 initScene을 한 번, 이벤트가 onKey/onFocus를, 프레임이 onUpdate를 실행한다.
- 3dAssign_15: init/key/update 콜백을 등록한다. initScene, onKey, onUpdate는 Console.cpp에 있다. 기본 렌더 콜백을 별도 등록하지 않았으므로 해당 사본의 renderScene이 실행된다.

## 새 루트 framework의 흐름

사용자 main 또는 examples/basic.cpp·assets.cpp → Application 객체/콜백 등록 → Application::run(detail/Application.inl) → initialize → 번들 GLFW 창/컨텍스트 → Renderer::initialize(detail/Renderer.inl) → initHandler.

initHandler의 도형 생성 또는 AssetLoader(ModelLoader.h) → Scene::addModel/setMesh(detail/Scene.inl) → CPU Model/각 Mesh의 정점·인덱스·UV·재질. run 루프의 pollEvents는 등록된 키·마우스·커서 콜백을 실행한다. 마우스 좌표 → Application::pickAt → screenRay/pick(Picking.h) → 표시된 삼각형의 가장 가까운 교점. Scene 수정 → geometryRevision 증가.

각 프레임의 updateHandler → renderHandler 또는 renderScene → Renderer::render → 변경된 정점/인덱스를 upload → VAO/VBO/EBO → draw. pos/color/UV/normal 속성 → Shaders.h의 정점 셰이더(uMVP, 법선 행렬) → 삼각형 조립·클리핑·래스터화 → 프래그먼트 셰이더(색상/선택적 이미지·조명) → 깊이 테스트 → swapBuffers. 선택적 외곽선은 같은 메시 인덱스 공간의 GL_LINES로 그린다.

월드 정점은 부모 모델 행렬 × 선택적 면 추가 행렬 × 로컬 정점이다. 렌더링과 피킹이 DiagramSystem::faceModelMatrix를 공유한다. shutDown에서 자산 → Renderer::destroy → 창 → GLFW를 정리한다. DLL 함수 포인터와 사용자 콜백의 실제 호출은 AST의 타입 참조 엣지와 구분해 소스로 확인했다.

GRAPH_REPORT/graph.html은 새 루트 framework 15개 코드 파일의 AST 결과다. 위 프로젝트 목록은 별도 정적 소스 조사다. 이 자료들은 실행 증거가 아니다.
