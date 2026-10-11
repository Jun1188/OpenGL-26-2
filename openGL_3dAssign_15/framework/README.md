# Graphics framework 사용법

이 폴더는 소스 형태로 재사용하는 라이브러리다. 과제의 main.cpp, Console.h,
Console.cpp, Common.h는 필요 없다. 미리 빌드된 .lib나 헤더만으로 동작하는
라이브러리는 아니다. 변경 코드는 UTF-8 BOM이다.

## 다른 프로젝트에 가져오기

1. framework 폴더를 복사한다.
2. include/Application.h 또는 include/Graphics.h를 include한다.
3. src/Graphics.cpp와 src/Application.cpp를 프로젝트의 컴파일 항목에 추가한다.
   .cpp 파일 자체를 #include하지 않는다.
4. C++17 이상과 x64 구성을 선택한다. assign13의 기존 언어 표준은 유지한다.
   dependencies에는 GLEW 2.3.1, GLFW 3.4, GLM 1.0.3과 라이선스가 포함된다.
5. .vcxproj에서 Microsoft.Cpp.targets 직전에 Framework.props를 가져온다.
   프로젝트 폴더 바로 아래에 framework를 복사한 경우 다음과 같이 작성한다.

   <Import Project="framework\Framework.props" />
   <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />

   Framework.props는 자신의 위치인 $(MSBuildThisFileDirectory)를 기준으로
   include와 LIB 경로를 지정한다. 저장소 루트의 OpenGL.props 또는
   컴퓨터별 OpenGL.local.props 설정은 필요하지 않다.
   기존 속성 시트를 대체하며 OpenGL.props와 중복해서 가져오지 않는다.
   GLFW_DLL, GLFW_INCLUDE_NONE, /source-charset:utf-8, Debug /MDd 및 Release /MD를 적용한다.
   GLEW_STATIC은 정의하지 않는다. opengl32.lib, glew32.lib, glfw3dll.lib를 링크한다.
   빌드 후 glew32.dll과 glfw3.dll을 $(TargetDir)에 자동 복사한다.
   함께 제공한 배포본은 x64 전용이며 Win32를 선택하면 오류를 표시한다.
6. ApplicationConfig::shaderDirectory에 shaders 폴더 경로를 지정한다.
   상대 경로는 프로세스의 현재 작업 폴더 기준이다. Visual Studio 디버깅 시
   Framework.props는 $(ProjectDir)를 작업 폴더로 지정한다.
   터미널에서 실행할 때도 프로젝트 폴더에서 시작하거나 셰이더 절대경로를 지정한다.
   셰이더는 빌드 후 실행 파일 옆의 shaders 폴더에 자동 복사한다.
   Application::findShaderDirectory()는 실행 파일 옆을 먼저 확인하고,
   현재 작업 폴더와 상위 폴더의 framework/shaders를 확인한다.

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

## 의존성 포함 변경

CHANGES-DEPENDENCIES.md는 이번 작업 시작 시점과 최종 설정·사용법을 모두 대조한다.
DEPENDENCIES.patch는 추가한 의존성 전체와 LIB/DLL의 Git binary patch를 포함한다.
제3자 텍스트는 UTF-8 BOM만 적용했으며 내용은 원본과 같다. 바이너리는 그대로 복사했다.

## 상대경로와 DLL 확인은 사용자가 수행

1. Visual Studio에서 openGL_assign_13.slnx를 열고 Debug|x64와 Release|x64를 빌드한다.
2. 각 출력 폴더의 실행 파일 옆에 glew32.dll과 glfw3.dll이 있는지 확인한다.
3. F5로 실행하고 도형·선·카메라 및 기존 입력 동작을 확인한다.
4. framework 폴더를 다른 x64 프로젝트에 복사한다. 두 .cpp를 등록하고
   Framework.props를 가져온 뒤 사용자 main으로 빌드·실행한다.
5. 프로젝트 속성에서 include/LIB 경로가 복사한 framework 내부를 가리키는지 확인한다.

위 빌드 및 실행 테스트는 이 작업에서 수행하지 않았다.

## 실습 13·14·15 공용 확장

- addPyramid: 바닥 X폭, Y높이, 바닥 Z폭. 면 순서는 앞·오른쪽·뒤·왼쪽·바닥이다.
- addCuboid: 기존 앞·뒤·왼쪽·오른쪽·위·아래 면 순서를 유지한다.
- addRightTetrahedron: 기존 직사면체 API를 유지한다. 사각뿔과 별도 형상이다.
- setFaceVisible: draw 단계에서 표시 여부를 읽으며 geometry revision은 바꾸지 않는다.
- setFaceLocalMatrix: 일반 면의 Model은 객체 Model × localAnimationMatrix이다.
  독립 면은 기존 월드 transform만 사용한다. 행렬은 유한한 값이어야 한다.
- setFaceVertexColors: 로컬 꼭짓점 수와 같은 색상 배열을 지정한다.
  빈 배열은 단색으로 복원한다. 색상 변경은 geometry revision을 갱신한다.
- Transform3D::orientation: 정규화한 쿼터니언을 Euler 회전 다음, scale 전에 적용한다.
  기본 단위 쿼터니언은 기존 Euler 코드의 결과를 유지한다.
- Camera::orthographic: 짧은 화면 축의 범위는 ±orthographicHalfExtent이다.
  긴 축은 aspect에 맞춰 넓혀 화면 비율을 유지한다. 기본값은 기존 원근 투영이다.
- getRenderOptions(): depthTest와 wireframe을 설정한다. 기본은 깊이 켜짐·솔리드이다.
  와이어는 GL_LINE_LOOP로 면 테두리를 그리며 삼각분할 대각선은 제외한다.
- setModifiedKeyHandler: key/action/mods를 전달한다. 설정하면 기존 keyHandler를 대체한다.
  GLFW_LOCK_KEY_MODS를 켜므로 Shift와 Caps Lock을 함께 판별할 수 있다.

입력 → 장면 상태 → 면 행렬 → 객체 Model → View/Projection → uMVP → 화면.
형상·색상 변경만 GPU 버퍼를 다시 올린다. 면 표시·변환은 draw 때 읽는다.
빌드와 실행 테스트는 사용자에게 맡긴다. 정적 검토만으로 실행 성공을 보장하지 않는다.

소스는 UTF-8 BOM으로 저장하며 /source-charset:utf-8로 읽는다. 일반 문자열의 실행 인코딩은 MSVC의 시스템 기본 문자 집합을 사용한다. 한국어 Windows의 기본 949 콘솔을 기준으로 하며, UTF-8로 강제 설정한 콘솔은 별도 인코딩 조건이다. 실행 중 코드 페이지 설정 함수는 호출하지 않는다.
