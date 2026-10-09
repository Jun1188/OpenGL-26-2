# Graphics framework 사용법

이 폴더는 소스 형태로 재사용하는 라이브러리다. 과제의 main.cpp, Console.h,
Console.cpp, Common.h는 필요 없다. 미리 빌드된 .lib나 헤더만으로 동작하는
라이브러리는 아니다. 변경 코드는 UTF-8 BOM이다.

## 다른 프로젝트에 가져오기

1. framework 폴더를 복사한다.
2. include/Application.h 또는 include/Graphics.h를 include한다.
3. src/Graphics.cpp와 src/Application.cpp를 프로젝트의 컴파일 항목에 추가한다.
   .cpp 파일 자체를 #include하지 않는다.
4. C++17 이상, GLEW, GLFW, GLM과 OpenGL 링크 설정을 제공한다.
   이 저장소에서는 OpenGL.props가 의존성을 설정한다.
   현재 include 형태에 맞게 GLEW의 include/GL, GLFW의 include/GLFW,
   GLM의 루트 폴더를 헤더 검색 경로에 넣는다.
5. Windows 동적 라이브러리 설정에서는 GLFW_DLL, GLFW_INCLUDE_NONE을 정의하고
   opengl32.lib, glew32.lib, glfw3dll.lib를 링크한다.
   glew32.dll과 glfw3.dll이 실행 파일에서 검색되도록 배치한다.
6. ApplicationConfig::shaderDirectory에 shaders 폴더 경로를 지정한다.
   상대 경로는 실행 파일 위치가 아니라 프로세스의 현재 작업 폴더 기준이다.

Renderer만 사용할 때는 Graphics.cpp만 컴파일해도 된다.
이 경우 GLFW는 필수가 아니다. 호출자가 OpenGL 3.3 컨텍스트와 GLEW 초기화를
제공하고 컨텍스트 종료 전에 Renderer::destroy()를 호출한다.

## 사용자 main 예시

아래 코드는 문서 예제다. 빌드와 실행은 수행하지 않았다.

```cpp
#include "framework/include/Application.h"
#include <glfw3.h>

bool createScene(graphics::Application& app) {
    const glm::vec3 lengths{ 1.0f, 2.0f, 0.5f };
    const glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    return app.getScene().addCuboid(lengths, position).has_value();
}

void onKey(graphics::Application& app, int key, int action) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app.requestClose();
    }
}

int main() {
    graphics::Application app;
    graphics::ApplicationConfig config;
    config.title = "My graphics program";
    config.shaderDirectory = "framework/shaders";
    app.setInitHandler(createScene);
    app.setKeyHandler(onKey);
    return app.run(config);
}
```

createScene, onKey, onUpdate는 사용자가 작성한 다른 .h/.cpp에 선언하고
정의해도 된다. main은 해당 헤더를 include하고 콜백을 등록한다.
사용자 상태는 람다로 캡처할 수 있다. 캡처한 객체는 run()이 끝날 때까지 유지한다.

## API와 실행 순서

- ApplicationConfig: 창 크기, 제목, 셰이더 경로, 수직 동기화를 지정한다.
- setInitHandler: 컨텍스트와 렌더러 초기화 후 한 번 호출한다. bool을 반환한다.
- setKeyHandler: GLFW의 key와 action을 전달한다. 종료 키는 사용자가 지정한다.
- setFocusHandler: 창의 포커스 변경을 bool로 전달한다.
- setUpdateHandler: 프레임마다 deltaTime(초)을 전달한다. 최대 0.05초로 제한한다.
- setRenderHandler: 등록하지 않으면 기본 Scene/Camera를 렌더링한다.
  등록하면 기본 렌더링을 대체한다. 필요하면 콜백에서 renderScene()을 호출한다.
  bool 반환이 false면 run()이 -1로 끝난다.
- getScene/getCamera: 사용자 코드에서 장면과 카메라를 변경한다.
- requestClose: 반복 루프 종료를 요청한다.
- run: 창 생성 → 초기화 콜백 → 이벤트/업데이트/렌더링/화면 교체 → 자원 해제.
  초기화 실패도 자원을 해제하고 -1을 반환한다. 중첩 run 호출은 허용하지 않는다.
  최소화 상태에서는 업데이트와 렌더링을 멈추고 이벤트를 기다린다.

Application은 한 스레드에서 하나씩 사용하며 GLFW 초기화/종료를 소유한다.
다른 GLFW 프로그램과 컨텍스트를 공유해야 하면 Application 대신 Renderer를 쓴다.
GLFW window user pointer와 key/focus callback은 Application이 관리한다.
getWindow()로 이를 덮어쓰지 않는다. 콜백 실행 중 콜백 자체를 교체하지 않는다.
초기화 콜백에서 사용자가 장면을 구성하기 전에는 Scene이 비어 있다.
run을 다시 호출하면 기존 Scene/Camera 데이터는 유지되고 GPU 자원은 다시 생성된다.

CPU Scene 데이터 → VAO/VBO/EBO → CPU의 P*V*M 계산 → uMVP 전달 →
vertex shader → 삼각형/선 → 깊이 검사 → 화면 순서다.
GLSL은 layout 0의 vec3 위치와 layout 1의 vec3 색상, mat4 uMVP를 사용한다.

Scene은 직육면체, 세 모서리가 직교하는 직사면체, Line, vec3 벡터, basis를
제공한다. 현재 용량은 MAX_ENTITIES=50, MAX_LINES=500이다.
도형의 worldPosition은 로컬 무게중심을 옮긴 위치다.
addBasis의 mat3 열이 기저 벡터이며, owner를 주면 객체 변환을 따른다.

## 사용자가 수행할 확인

빌드와 테스트는 실행하지 않았다. Visual Studio에서 Debug|x64 및 Release|x64를
직접 빌드한다. 기존 예제에서 도형/선 표시, 이동/크기 변경, 카메라 이동,
창 크기 변경, 최소화/복원, 포커스 변경, Esc/Q 종료를 확인한다.
다른 프로젝트에서는 Console과 기존 main을 제외하고 위 main 예시로 연결한다.
셰이더 경로 오류 시 초기화 실패, init callback의 false 반환 시 정상 정리를 확인한다.

전체 저장소의 graphify 그래프 생성/갱신은 이번 승인 범위에 포함되지 않아
수행하지 않았다. 위 흐름은 소스 정적 확인에 따른 설명이다.
