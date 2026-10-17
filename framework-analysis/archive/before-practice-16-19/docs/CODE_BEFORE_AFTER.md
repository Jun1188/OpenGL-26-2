# 프레임워크 코드 전체 before / after

기준 before: `0e8389e`. after: 최종 저장 파일. 파일 전체를 짝지어 모든 함수·클래스와 코드 블록을 포함한다. BOM은 코드 표시에서 제외하고 파일에는 유지한다. 중간 단계는 커밋에서 확인한다.

## framework/.gitignore

번들 바이너리 추적과 framework 내부 분석 캐시 제외에 한정한다.

### before

없음.

### after

```
!dependencies/assimp/bin/assimp-vc145-mt.dll
!dependencies/assimp/lib/assimp-vc145-mt.lib
/graphify-out/cache/
```

## framework/Framework.h

사용자 include 한 곳으로 공개 API를 제공한다.

### before

없음.

### after

```
#pragma once
// Copy this complete folder, then include this entry point. C++17, Windows x64.
#include "include/Application.h"
```

## framework/Framework.props

필수 외부 링크 설정을 없애고 선택적 언어/인코딩 편의 설정으로 정리한다.

### before

```
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <GraphicsFrameworkDirectory>$(MSBuildThisFileDirectory)</GraphicsFrameworkDirectory>
    <GraphicsFrameworkGlewRoot>$(GraphicsFrameworkDirectory)dependencies\glew-2.3.1\</GraphicsFrameworkGlewRoot>
    <GraphicsFrameworkGlfwRoot>$(GraphicsFrameworkDirectory)dependencies\glfw-3.4.bin.WIN64\</GraphicsFrameworkGlfwRoot>
    <GraphicsFrameworkGlmRoot>$(GraphicsFrameworkDirectory)dependencies\glm\</GraphicsFrameworkGlmRoot>
    <LocalDebuggerWorkingDirectory>$(ProjectDir)</LocalDebuggerWorkingDirectory>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <AdditionalIncludeDirectories>$(GraphicsFrameworkDirectory)include;$(GraphicsFrameworkGlewRoot)include;$(GraphicsFrameworkGlewRoot)include\GL;$(GraphicsFrameworkGlfwRoot)include;$(GraphicsFrameworkGlfwRoot)include\GLFW;$(GraphicsFrameworkGlmRoot);%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>GLFW_DLL;GLFW_INCLUDE_NONE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebugDLL</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreadedDLL</RuntimeLibrary>
    </ClCompile>
    <Link>
      <AdditionalLibraryDirectories>$(GraphicsFrameworkGlewRoot)lib\Release\x64;$(GraphicsFrameworkGlfwRoot)lib-vc2022;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <AdditionalDependencies>opengl32.lib;glew32.lib;glfw3dll.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>
  <Target Name="CheckGraphicsFrameworkDependencies" BeforeTargets="PrepareForBuild">
    <Error Condition="'$(Platform)'!='x64'" Text="The bundled framework libraries support x64. Select Debug|x64 or Release|x64." />
    <ItemGroup>
      <GraphicsFrameworkRequiredFile Include="$(GraphicsFrameworkGlewRoot)include\GL\glew.h;$(GraphicsFrameworkGlewRoot)lib\Release\x64\glew32.lib;$(GraphicsFrameworkGlewRoot)bin\Release\x64\glew32.dll;$(GraphicsFrameworkGlfwRoot)include\GLFW\glfw3.h;$(GraphicsFrameworkGlfwRoot)lib-vc2022\glfw3dll.lib;$(GraphicsFrameworkGlfwRoot)lib-vc2022\glfw3.dll;$(GraphicsFrameworkGlmRoot)glm\glm.hpp" />
    </ItemGroup>
    <Error Condition="!Exists('%(GraphicsFrameworkRequiredFile.Identity)')" Text="Missing framework dependency: %(GraphicsFrameworkRequiredFile.Identity). Copy the complete framework folder including dependencies." />
  </Target>
  <Target Name="CopyGraphicsFrameworkRuntimeDlls" AfterTargets="Build">
    <ItemGroup>
      <GraphicsFrameworkRuntimeDll Include="$(GraphicsFrameworkGlewRoot)bin\Release\x64\glew32.dll;$(GraphicsFrameworkGlfwRoot)lib-vc2022\glfw3.dll" />
    </ItemGroup>
    <Copy SourceFiles="@(GraphicsFrameworkRuntimeDll)" DestinationFolder="$(TargetDir)" SkipUnchangedFiles="true" />
  </Target>
  <Target Name="CopyGraphicsFrameworkShaders" AfterTargets="Build">
    <ItemGroup>
      <GraphicsFrameworkShader Include="$(GraphicsFrameworkDirectory)shaders\VertexShader.glsl;$(GraphicsFrameworkDirectory)shaders\FragmentShader.glsl" />
    </ItemGroup>
    <Copy SourceFiles="@(GraphicsFrameworkShader)" DestinationFolder="$(TargetDir)shaders\" SkipUnchangedFiles="true" />
  </Target>
</Project>
```

### after

```
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <!-- Optional convenience sheet. Framework.h does not require this import. -->
  <PropertyGroup>
    <GraphicsFrameworkDirectory>$(MSBuildThisFileDirectory)</GraphicsFrameworkDirectory>
    <LocalDebuggerWorkingDirectory>$(ProjectDir)</LocalDebuggerWorkingDirectory>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>
      <AdditionalIncludeDirectories>$(GraphicsFrameworkDirectory)include;$(GraphicsFrameworkDirectory)dependencies\glm;$(GraphicsFrameworkDirectory)dependencies\glew-2.3.1\include\GL;$(GraphicsFrameworkDirectory)dependencies\glfw-3.4.bin.WIN64\include\GLFW;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ClCompile>
  </ItemDefinitionGroup>
  <Target Name="CheckGraphicsFrameworkPlatform" BeforeTargets="PrepareForBuild">
    <Error Condition="'$(Platform)'!='x64'" Text="Bundled framework runtime DLLs require x64." />
  </Target>
</Project>
```

## framework/README.md

새 호출 방식·계약·실행 제한과 확인 절차를 설명한다.

### before

````
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
   GLFW_DLL, GLFW_INCLUDE_NONE, /utf-8, Debug /MDd 및 Release /MD를 적용한다.
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
````

### after

````
# 재사용 가능한 OpenGL framework

Windows x64 · C++17 · OpenGL 3.3. `framework` 폴더 전체를 프로젝트에 복사한 뒤 다음 헤더를 include한다.

```cpp
#include "framework/Framework.h"
```

구현은 inline 헤더에 있다. framework의 .cpp 등록, Framework.props 가져오기, 외부 SDK의 include/LIB 경로 지정은 필요하지 않다. 기본 셰이더도 헤더에 포함된다. C++17 이상과 UTF-8 소스 해석(`/utf-8`)은 프로젝트에서 선택한다. Framework.props는 이 설정을 위한 선택적 편의 파일이다.

GLFW와 Assimp의 헤더·DLL·라이선스는 dependencies 안에 포함된다. GLFW는 창 생성 시, Assimp는 모델 모드를 켜고 loadModel을 호출할 때 동적으로 로드한다. GLEW 헤더는 OpenGL 선언에만 사용한다. Windows SDK, 그래픽 드라이버와 해당 DLL에 필요한 Microsoft VC 런타임은 실행 환경에 필요하다. 별도 Assimp 설치나 저장소 경로는 사용하지 않는다.

빌드와 실행 테스트는 수행하지 않았다. [사용자 확인 절차](docs/USER_CHECKS.md)를 따른다.

## 기본 도형과 클릭

```cpp
int main() {
    graphics::Application app;
    app.setInitHandler([](graphics::Application& app) {
        return app.getScene().addCuboid({1, 1, 1}, {0, 0, 0}).has_value();
    });
    app.setMouseHandler([](graphics::Application& app, const graphics::MouseEvent& event) {
        if (event.button == GLFW_MOUSE_BUTTON_LEFT && event.action == GLFW_PRESS) {
            if (const auto hit = app.pickAt(event.windowX, event.windowY)) {
                app.getScene().splitFace(hit->entityId, hit->faceIndex);
            }
        }
    });
    app.setKeyHandler([](graphics::Application& app, int key, int action) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) app.requestClose();
    });
    return app.run();
}
```

기본 features는 `models=false`, `textures=false`다. addCuboid, addRightTetrahedron, addPyramid는 파일 없이 도형 메시를 만든다. [basic.cpp](examples/basic.cpp)는 부모 SRT와 클릭한 면의 추가 SRT를 보여 준다.

## 서로 다른 두 로더

`AssetLoader::loadPrimitive`는 기본 도형 텍스트를 읽는다. OBJ 형식의 v/vt/vn/f, 음수 인덱스, 삼각형·사각형·단순 평면 다각형을 처리한다. 기본적으로 같은 평면의 인접 삼각형을 논리적 면 메시로 병합한다. mtllib/usemtl 재질 처리는 이 모드에서 생략한다. 텍스처는 별도 상대경로 인자로 지정한다.

`AssetLoader::loadModel`은 Assimp로 일반 정적 모델을 읽는다. 노드의 변환을 로컬 정점에 반영하고, 각 메시의 인덱스·법선·색상과 선택적 UV·텍스처를 가져온다. 뼈대 애니메이션은 제공하지 않는다.

```cpp
graphics::ApplicationConfig config;
config.projectRoot = std::filesystem::current_path(); // 실제 프로젝트 루트로 지정한다.
config.features.models = true;   // loadModel을 사용할 때만 켠다.
config.features.textures = true; // UV와 이미지가 필요할 때만 켠다.

graphics::Application app;
app.setInitHandler([](graphics::Application& app) {
    graphics::LoadOptions options;
    options.pivot = graphics::PivotMode::BottomCenter;
    options.texturePath = "textures/override.png"; // 생략하면 모델의 재질 참조를 사용한다.
    const auto result = app.getAssets().loadModel("models/example.obj", options);
    if (!result) return false; // result.error에 원인이 들어 있다.
    return app.getScene().addModel(*result.model, graphics::Transform3D{},
                                  result.pivotMode, result.localBasis).has_value();
});
return app.run(config); // 사용자 main 안에서 호출한다.
```

기본 도형 파일에는 위의 모델 모드를 켤 필요가 없다. 호출만 다음과 같이 바꾼다.

```cpp
graphics::LoadOptions options;
options.outline = graphics::OutlineMode::FeatureEdges;
const auto result = app.getAssets().loadPrimitive("data/cube.txt", options);
```

models가 꺼진 loadModel, textures가 꺼진 loadTexture는 오류를 반환한다. textures가 꺼진 도형·모델 로드는 UV와 이미지 작업을 생략한다. 렌더링의 `getRenderOptions().textures`도 텍스처 사용을 제어한다. Application 초기화 때 features.textures에서 초기값을 가져온다.

모델·이미지 인자는 projectRoot 내부 상대경로여야 한다. 절대경로와 루트 밖으로 나가는 경로는 오류다. 모델이 참조하는 텍스처는 모델 폴더를 기준으로 찾고, 역시 projectRoot 안에 있어야 한다. `ApplicationConfig::runtimeDirectory`는 자산 루트와 다르다. 필요할 때 번들 framework 폴더의 실제 경로를 지정한다. 보통 실행 파일·작업 폴더의 상위 경로에서 framework를 찾으므로 생략할 수 있다. 배포할 때도 완전한 framework 폴더를 실행 파일 옆에 둔다.

[assets.cpp](examples/assets.cpp)는 primitive/model과 상대경로를 인자로 받는 예제다. [cube.txt](examples/data/cube.txt)는 기존 sample0 파일의 복사본이다.

## 로컬 기준점과 부모·면 SRT

`Transform3D::basis`는 월드 위치 또는 면의 추가 이동량이다. `localBasis`는 로컬 기준점이다. 변환은 열 벡터 기준으로 다음 순서다.

```text
부모 M = T(basis) * Rx * Ry * Rz * Quaternion * S * T(-localBasis)
분리 면 M = 부모 M * T(면 localBasis) * 면 SRT 행렬 * localAnimationMatrix
```

addModel의 기본 PivotMode::BottomCenter는 원본 로컬 AABB의 하단 중점을 localBasis로 설정한다. 정점을 강제로 변경하지 않는다. 따라서 부모 basis가 모델의 바닥 중점 위치가 된다. Preserve는 제공한 변환의 기준점을 유지한다. Custom은 인자로 전달한 기준점을 사용한다. setLocalBasis로 변경할 수 있다. BottomCenter 상태에서 메시를 수정하면 새 AABB에 맞춰 기준점을 다시 계산한다.

splitFace는 해당 면에 추가 SRT를 허용한다. 기본 추가 SRT는 항등 변환이므로 분리 자체로 위치가 바뀌지 않는다. 부모 SRT는 계속 적용된다. 분리 후 부모를 움직여도 면은 부모를 따른다. 면 중심 회전은 child.localBasis에 면의 로컬 중심을 넣고 setFaceSRT를 호출한다. child.basis에는 원본 모델 로컬 축 기준 추가 이동을 넣는다. 독립 월드 객체를 생성하는 기능은 아니다.

## 편집 API

Model은 기존 Polyhedron, Mesh는 기존 PlaneFigure의 별칭이다. 모델은 faces/meshes 목록을 갖고, 각 메시에는 임의 개수의 localVertices와 triangleIndices가 있다. Mesh::fromVertices로 일반 정점을 만들 수 있다.

| 목적 | API |
| --- | --- |
| 모델·메시 교체 | addModel, setModel, setMesh, getMesh |
| 정점과 삼각형 | setMeshVertices, setVertexPosition, setTriangleIndices |
| UV와 법선 | setTexcoords, setVertexNormals |
| 재질과 이미지 | setMaterial, AssetLoader::loadTexture |
| 기준점과 표시 | setLocalBasis, setVisible, setFaceVisible |
| 부모·면 변환 | setTransform, splitFace, setFaceSRT |
| 경계선 | setOutline |

getMesh/getComponent는 읽기용 포인터다. 장면 수정 후에는 다시 조회한다. setter는 잘못된 인덱스·속성 개수·비유한 값 등에 false를 반환한다. 검증이 실패하면 기존 메시를 유지한다. 편집 후 GPU 버퍼는 다음 render에서 갱신한다. setSideLengths는 기존 정점을 비례 조정하며 UV·재질·토폴로지를 유지한다. 길이가 0인 축을 가진 평면에는 사용하지 않는다.

```cpp
graphics::Material material;
const auto texture = app.getAssets().loadTexture("textures/example.png");
if (texture) {
    material.texture = texture.image;
    material.useTexture = true;
    app.getScene().setMaterial(id, meshIndex, material); // 메시 UV가 필요하다.
}
```

TextureImage에는 width/height와 RGBA8 데이터가 있다. 직접 만든 이미지도 shared_ptr<const TextureImage>로 재질에 지정할 수 있다. 공유 후 데이터를 변경하지 않는다. 변경하려면 새 이미지와 setMaterial을 사용한다. 파일·압축 임베디드 이미지는 Windows WIC로 읽는다. 지원하지 않는 형식은 오류를 반환한다. flipTextureRows와 Material::repeatTexture로 방향·반복을 지정한다.

OutlineMode::None은 경계를 끈다. FeatureEdges는 공면 내부 대각선을 제외하고 열린 경계·접힌 경계를 표시한다. AllTriangleEdges는 모든 삼각형 변을 표시한다. Explicit는 직접 전달한 인덱스 쌍을 사용한다. UV·법선 때문에 분리된 같은 좌표의 정점은 경계 계산에서 연결한다. 선택적 조명은 RenderOptions::lighting으로 켠다.

## 피킹과 수명

Application::pickAt은 GLFW 창 좌표를 실제 프레임버퍼 좌표로 바꾼다. pickAtCursor는 현재 커서를 사용한다. 직접 사용하려면 screenRay 또는 pick(scene, camera, x, y, viewport)를 호출한다. viewport는 좌상단 원점의 프레임버퍼 픽셀 영역이다.

PickResult는 가장 가까운 표시된 삼각형의 entityId/faceIndex/triangleIndex, 월드·로컬 교점, 무게중심 좌표와 선택적 UV를 반환한다. 원근·직교 카메라와 부모·면 변환을 사용한다. 선·축과 텍스처 투명 픽셀은 별도 판정하지 않는다.

Application은 한 스레드에서 한 실행 루프를 소유한다. 콜백은 컨텍스트 초기화 후 호출된다. getAssets는 run의 콜백 동안 유효하다. 콜백에 캡처한 객체는 run 종료까지 유지한다. 콜백 예외는 C 콜백 경계를 통과하지 않고, GPU·창 정리 후 다시 전달된다.

Renderer만 사용하면 호출자가 OpenGL 3.3 컨텍스트를 current로 유지한다. initialize → render → destroy 순서를 사용하고 컨텍스트를 마지막에 종료한다. Scene::clear 이후 ID는 재사용되므로 기존 ID를 유지하지 않는다.

## 검토 자료

- [전체 변경 전후](docs/CHANGES.md)
- [정적 검토와 번들 출처](docs/STATIC_REVIEW.md)
- [사용자가 수행할 빌드·실행 확인](docs/USER_CHECKS.md)
- [코드 구조 분석](graphify-out/GRAPH_REPORT.md), [실행 흐름과 프로젝트 범위](graphify-out/EXECUTION_FLOW.md)
- [인터랙티브 코드 그래프](graphify-out/graph.html)

참고 자료: [ComputerGraphics](https://github.com/maru3172/ComputerGraphics)의 컴그_29번_텍스처매핑과 컴그_파이프라인1차. 정점·UV·법선 입력과 모델/뷰/투영·재질 흐름을 참고했다. 프레임워크의 자원 수명, 일반 인덱스 메시와 피킹은 별도 구현이다.
````

## framework/detail/Application.inl

앱 수명과 이벤트 루프를 inline으로 옮기고 C 콜백 예외를 정리한다.

### before

없음.

### after

```
#pragma once
#include <algorithm>
#include <iostream>
#include <utility>
namespace graphics::detail {
inline bool& applicationActive() { static bool active=false;return active; }
}
namespace graphics {

inline Application::~Application() {
    shutdown();
}

inline void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

inline void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

inline void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

inline void Application::setModifiedKeyHandler(
    std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

inline std::filesystem::path Application::findShaderDirectory() {
    const auto containsShaders = [](const std::filesystem::path& path) {
        return std::filesystem::is_regular_file(path / "VertexShader.glsl")
            && std::filesystem::is_regular_file(path / "FragmentShader.glsl");
    };
#ifdef _WIN32
    // 실행 파일 옆의 shaders를 우선한다. 작업 폴더와 프로젝트 이름에 의존하지 않는다.
    std::wstring executable(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(),
                                           static_cast<DWORD>(executable.size()));
    if (length > 0 && length < executable.size()) {
        executable.resize(length);
        const auto candidate = std::filesystem::path(executable).parent_path() / "shaders";
        if (containsShaders(candidate)) {
            return candidate;
        }
    }
#endif
    auto directory = std::filesystem::current_path();
    for (;;) {
        const auto candidate = directory / "framework" / "shaders";
        if (containsShaders(candidate)) {
            return candidate;
        }
        if (directory == directory.parent_path()) {
            return {};
        }
        directory = directory.parent_path();
    }
}

inline void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

inline void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

inline bool Application::initialize(const ApplicationConfig& config) {
    detail::glfw().load(config.runtimeDirectory);
    if (config.width <= 0 || config.height <= 0 || !detail::glfw().Init()) {
        return false;
    }
    glfwInitialized_ = true;
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    detail::glfw().WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    detail::glfw().WindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    detail::glfw().WindowHint(GLFW_DEPTH_BITS, 24);
    window_ = detail::glfw().CreateWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        return false;
    }
    detail::glfw().MakeContextCurrent(window_);
    detail::glfw().SwapInterval(config.verticalSync ? 1 : 0);
    const auto vertexPath=config.shaderDirectory.empty()?std::filesystem::path{}:config.shaderDirectory/"VertexShader.glsl";
    const auto fragmentPath=config.shaderDirectory.empty()?std::filesystem::path{}:config.shaderDirectory/"FragmentShader.glsl";
    if(!renderer_.initialize(vertexPath,fragmentPath)) return false;
    assets_.emplace(config.projectRoot,config.features,config.runtimeDirectory);
    renderOptions_.textures=config.features.textures;

    detail::glfw().SetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    detail::glfw().SetWindowUserPointer(window_, this);
    detail::glfw().SetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&] {
            if(app.modifiedKeyHandler_)app.modifiedKeyHandler_(app,key,action,mods);
            else if(app.keyHandler_)app.keyHandler_(app,key,action);
        });
    });
    detail::glfw().SetWindowFocusCallback(window_, [](GLFWwindow* window,int focused) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&]{if(app.focusHandler_)app.focusHandler_(app,focused==GLFW_TRUE);});
    });
    detail::glfw().SetMouseButtonCallback(window_, [](GLFWwindow* window,int button,int action,int mods) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&] {
            if(!app.mouseHandler_)return;
            MouseEvent event;event.button=button;event.action=action;event.mods=mods;
            detail::glfw().GetCursorPos(window,&event.windowX,&event.windowY);
            int ww=0,wh=0,fw=0,fh=0;
            detail::glfw().GetWindowSize(window,&ww,&wh);detail::glfw().GetFramebufferSize(window,&fw,&fh);
            if(ww<=0||wh<=0||fw<=0||fh<=0)return;
            event.framebufferX=event.windowX*fw/ww;event.framebufferY=event.windowY*fh/wh;
            app.mouseHandler_(app,event);
        });
    });
    detail::glfw().SetCursorPosCallback(window_, [](GLFWwindow* window,double x,double y) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&]{if(app.cursorHandler_)app.cursorHandler_(app,x,y);});
    });
    return true;
}

inline void Application::shutdown() {
    assets_.reset();
    if (window_ != nullptr) {
        detail::glfw().MakeContextCurrent(window_);
        renderer_.destroy();
        detail::glfw().SetWindowUserPointer(window_, nullptr);
        detail::glfw().DestroyWindow(window_);
        window_ = nullptr;
    }
    if (glfwInitialized_) {
        detail::glfw().Terminate();
        glfwInitialized_ = false;
    }
}

inline void Application::requestClose() {
    if (window_ != nullptr) {
        detail::glfw().SetWindowShouldClose(window_, GLFW_TRUE);
    }
}

inline bool Application::renderScene() {
    if (window_ == nullptr) {
        return false;
    }
    detail::glfw().MakeContextCurrent(window_);
    int width = 0;
    int height = 0;
    detail::glfw().GetFramebufferSize(window_, &width, &height);
    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

inline int Application::run(const ApplicationConfig& config) {
    if (running_) {
        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }
    if(detail::applicationActive()) return -1;
    detail::applicationActive()=true;
    callbackError_=nullptr;
    running_ = true;
    int exitCode = 0;
    try {
        if (!initialize(config)) {
            std::cerr << "Application initialization failed.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            std::cerr << "User scene initialization failed.\n";
            exitCode = -1;
        } else {
            double previousTime = detail::glfw().GetTime();
            while (!detail::glfw().WindowShouldClose(window_)) {
                detail::glfw().PollEvents();
                if(callbackError_)std::rethrow_exception(callbackError_);
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                int width = 0;
                int height = 0;
                detail::glfw().GetFramebufferSize(window_, &width, &height);
                if (width <= 0 || height <= 0) {
                    detail::glfw().WaitEvents();
                    if(callbackError_)std::rethrow_exception(callbackError_);
                    previousTime = detail::glfw().GetTime();
                    continue;
                }
                const double currentTime = detail::glfw().GetTime();
                const float deltaTime = static_cast<float>(
                    std::clamp(currentTime - previousTime, 0.0, 0.05));
                previousTime = currentTime;
                if (updateHandler_) {
                    updateHandler_(*this, deltaTime);
                }
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
                if (!rendered) {
                    std::cerr << "Rendering failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().SwapBuffers(window_);
            }
        }
    } catch (...) {
        shutdown();
        running_ = false;
        detail::applicationActive()=false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    detail::applicationActive()=false;
    return exitCode;
}

inline void Application::setMouseHandler(std::function<void(Application&,const MouseEvent&)> handler) {
    mouseHandler_=std::move(handler);
}
inline void Application::setCursorHandler(std::function<void(Application&,double,double)> handler) {
    cursorHandler_=std::move(handler);
}
inline std::optional<PickResult> Application::pickAt(double x,double y) const {
    if(!window_)return {};
    int ww=0,wh=0,fw=0,fh=0;
    detail::glfw().GetWindowSize(window_,&ww,&wh);detail::glfw().GetFramebufferSize(window_,&fw,&fh);
    if(ww<=0||wh<=0||fw<=0||fh<=0)return {};
    return graphics::pick(scene_,camera_,x*fw/ww,y*fh/wh,{0,0,double(fw),double(fh)});
}
inline std::optional<PickResult> Application::pickAtCursor() const {
    if(!window_)return {};
    double x=0,y=0;detail::glfw().GetCursorPos(window_,&x,&y);return pickAt(x,y);
}
inline AssetLoader& Application::getAssets() {
    if(!assets_)throw std::logic_error("AssetLoader is available during run callbacks only.");
    return *assets_;
}
} // namespace graphics
```

## framework/detail/Native.h

번들 DLL·시스템 OpenGL 함수를 런타임에 연결한다.

### before

없음.

### after

```
#pragma once
#ifndef _WIN32
#error This bundled framework targets Windows x64.
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "../dependencies/glew-2.3.1/include/GL/glew.h"
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include "../dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include <filesystem>
#include <string>
#include <stdexcept>
#include <vector>

#define GRAPHICS_WIDEN_IMPL(x) L##x
#define GRAPHICS_WIDEN(x) GRAPHICS_WIDEN_IMPL(x)
namespace graphics::detail {
static_assert(sizeof(void*) == 8, "Bundled DLLs require x64.");
inline std::filesystem::path executableDirectory() {
    std::wstring path(32768, L'\0');
    const DWORD size = GetModuleFileNameW(nullptr, path.data(), DWORD(path.size()));
    if (!size || size >= path.size()) return {};
    path.resize(size);
    return std::filesystem::path(path).parent_path();
}
inline std::filesystem::path frameworkDirectory() {
    std::error_code error;
    const auto source = std::filesystem::path(GRAPHICS_WIDEN(__FILE__)).parent_path().parent_path();
    std::vector<std::filesystem::path> starts{executableDirectory(), std::filesystem::current_path(error)};
    for (auto directory : starts) {
        while (!directory.empty()) {
            const auto candidate = directory / "framework";
            if (std::filesystem::is_regular_file(candidate / "include/Model.h", error)) return candidate;
            const auto parent = directory.parent_path();
            if (parent == directory) break;
            directory = parent;
        }
    }
    if (std::filesystem::is_regular_file(source / "include/Model.h", error))
        return std::filesystem::absolute(source);
    throw std::runtime_error("Cannot locate bundled framework. Place framework beside the executable or set runtimeDirectory.");
}
class Module {
    HMODULE handle_ = nullptr;
public:
    Module() = default;
    Module(const Module&) = delete;
    Module& operator=(const Module&) = delete;
    ~Module() { if (handle_) FreeLibrary(handle_); }
    void open(const std::filesystem::path& path) {
        if (handle_) return;
        handle_ = LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if (!handle_) throw std::runtime_error("Cannot load bundled runtime DLL (Win32 error " + std::to_string(GetLastError()) + ").");
    }
    template<class T> T symbol(const char* name) const {
        const auto address = ::GetProcAddress(handle_, name);
        if (!address) throw std::runtime_error(std::string("Missing runtime export: ") + name);
        return reinterpret_cast<T>(address);
    }
};
struct GlfwApi {
    Module module;
    bool ready = false;
    decltype(&::glfwInit) Init = nullptr;
    decltype(&::glfwTerminate) Terminate = nullptr;
    decltype(&::glfwWindowHint) WindowHint = nullptr;
    decltype(&::glfwCreateWindow) CreateWindow = nullptr;
    decltype(&::glfwDestroyWindow) DestroyWindow = nullptr;
    decltype(&::glfwMakeContextCurrent) MakeContextCurrent = nullptr;
    decltype(&::glfwSwapInterval) SwapInterval = nullptr;
    decltype(&::glfwSetInputMode) SetInputMode = nullptr;
    decltype(&::glfwSetWindowUserPointer) SetWindowUserPointer = nullptr;
    decltype(&::glfwGetWindowUserPointer) GetWindowUserPointer = nullptr;
    decltype(&::glfwSetKeyCallback) SetKeyCallback = nullptr;
    decltype(&::glfwSetWindowFocusCallback) SetWindowFocusCallback = nullptr;
    decltype(&::glfwSetMouseButtonCallback) SetMouseButtonCallback = nullptr;
    decltype(&::glfwSetCursorPosCallback) SetCursorPosCallback = nullptr;
    decltype(&::glfwGetCursorPos) GetCursorPos = nullptr;
    decltype(&::glfwGetWindowSize) GetWindowSize = nullptr;
    decltype(&::glfwGetFramebufferSize) GetFramebufferSize = nullptr;
    decltype(&::glfwWindowShouldClose) WindowShouldClose = nullptr;
    decltype(&::glfwSetWindowShouldClose) SetWindowShouldClose = nullptr;
    decltype(&::glfwGetTime) GetTime = nullptr;
    decltype(&::glfwPollEvents) PollEvents = nullptr;
    decltype(&::glfwWaitEvents) WaitEvents = nullptr;
    decltype(&::glfwSwapBuffers) SwapBuffers = nullptr;
    void load(const std::filesystem::path& root = std::filesystem::path{}) {
        if (ready) return;
        module.open((root.empty() ? frameworkDirectory() : std::filesystem::absolute(root)) / "dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll");
        Init = module.symbol<decltype(Init)>("glfwInit");
        Terminate = module.symbol<decltype(Terminate)>("glfwTerminate");
        WindowHint = module.symbol<decltype(WindowHint)>("glfwWindowHint");
        CreateWindow = module.symbol<decltype(CreateWindow)>("glfwCreateWindow");
        DestroyWindow = module.symbol<decltype(DestroyWindow)>("glfwDestroyWindow");
        MakeContextCurrent = module.symbol<decltype(MakeContextCurrent)>("glfwMakeContextCurrent");
        SwapInterval = module.symbol<decltype(SwapInterval)>("glfwSwapInterval");
        SetInputMode = module.symbol<decltype(SetInputMode)>("glfwSetInputMode");
        SetWindowUserPointer = module.symbol<decltype(SetWindowUserPointer)>("glfwSetWindowUserPointer");
        GetWindowUserPointer = module.symbol<decltype(GetWindowUserPointer)>("glfwGetWindowUserPointer");
        SetKeyCallback = module.symbol<decltype(SetKeyCallback)>("glfwSetKeyCallback");
        SetWindowFocusCallback = module.symbol<decltype(SetWindowFocusCallback)>("glfwSetWindowFocusCallback");
        SetMouseButtonCallback = module.symbol<decltype(SetMouseButtonCallback)>("glfwSetMouseButtonCallback");
        SetCursorPosCallback = module.symbol<decltype(SetCursorPosCallback)>("glfwSetCursorPosCallback");
        GetCursorPos = module.symbol<decltype(GetCursorPos)>("glfwGetCursorPos");
        GetWindowSize = module.symbol<decltype(GetWindowSize)>("glfwGetWindowSize");
        GetFramebufferSize = module.symbol<decltype(GetFramebufferSize)>("glfwGetFramebufferSize");
        WindowShouldClose = module.symbol<decltype(WindowShouldClose)>("glfwWindowShouldClose");
        SetWindowShouldClose = module.symbol<decltype(SetWindowShouldClose)>("glfwSetWindowShouldClose");
        GetTime = module.symbol<decltype(GetTime)>("glfwGetTime");
        PollEvents = module.symbol<decltype(PollEvents)>("glfwPollEvents");
        WaitEvents = module.symbol<decltype(WaitEvents)>("glfwWaitEvents");
        SwapBuffers = module.symbol<decltype(SwapBuffers)>("glfwSwapBuffers");
        ready = true;
    }
};
inline GlfwApi& glfw() { static GlfwApi api; return api; }

struct GlApi {
    Module module;
    using WglGetProc = PROC (WINAPI*)(LPCSTR);
    WglGetProc wglGetProc = nullptr;
    decltype(&::glClearColor) ClearColor = nullptr;
    decltype(&::glClear) Clear = nullptr;
    decltype(&::glEnable) Enable = nullptr;
    decltype(&::glDisable) Disable = nullptr;
    decltype(&::glDepthMask) DepthMask = nullptr;
    decltype(&::glDepthFunc) DepthFunc = nullptr;
    decltype(&::glViewport) Viewport = nullptr;
    decltype(&::glPolygonMode) PolygonMode = nullptr;
    decltype(&::glDrawElements) DrawElements = nullptr;
    decltype(&::glGenTextures) GenTextures = nullptr;
    decltype(&::glBindTexture) BindTexture = nullptr;
    decltype(&::glTexParameteri) TexParameteri = nullptr;
    decltype(&::glTexImage2D) TexImage2D = nullptr;
    decltype(&::glPixelStorei) PixelStorei = nullptr;
    decltype(&::glDeleteTextures) DeleteTextures = nullptr;
    decltype(&::glGetIntegerv) GetIntegerv = nullptr;
    decltype(&::glGetError) GetError = nullptr;
    decltype(&::glLineWidth) LineWidth = nullptr;
    PFNGLGENVERTEXARRAYSPROC GenVertexArrays = nullptr;
    PFNGLBINDVERTEXARRAYPROC BindVertexArray = nullptr;
    PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays = nullptr;
    PFNGLGENBUFFERSPROC GenBuffers = nullptr;
    PFNGLBINDBUFFERPROC BindBuffer = nullptr;
    PFNGLBUFFERDATAPROC BufferData = nullptr;
    PFNGLDELETEBUFFERSPROC DeleteBuffers = nullptr;
    PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer = nullptr;
    PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray = nullptr;
    PFNGLCREATESHADERPROC CreateShader = nullptr;
    PFNGLSHADERSOURCEPROC ShaderSource = nullptr;
    PFNGLCOMPILESHADERPROC CompileShader = nullptr;
    PFNGLGETSHADERIVPROC GetShaderiv = nullptr;
    PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog = nullptr;
    PFNGLDELETESHADERPROC DeleteShader = nullptr;
    PFNGLCREATEPROGRAMPROC CreateProgram = nullptr;
    PFNGLATTACHSHADERPROC AttachShader = nullptr;
    PFNGLLINKPROGRAMPROC LinkProgram = nullptr;
    PFNGLGETPROGRAMIVPROC GetProgramiv = nullptr;
    PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog = nullptr;
    PFNGLDETACHSHADERPROC DetachShader = nullptr;
    PFNGLDELETEPROGRAMPROC DeleteProgram = nullptr;
    PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation = nullptr;
    PFNGLUSEPROGRAMPROC UseProgram = nullptr;
    PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv = nullptr;
    PFNGLUNIFORMMATRIX3FVPROC UniformMatrix3fv = nullptr;
    PFNGLUNIFORM3FPROC Uniform3f = nullptr;
    PFNGLUNIFORM1IPROC Uniform1i = nullptr;
    PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;
    template<class T> T resolve(const char* name) {
        PROC p = wglGetProc(name);
        if (!p || p == reinterpret_cast<PROC>(1) || p == reinterpret_cast<PROC>(2)
            || p == reinterpret_cast<PROC>(3) || p == reinterpret_cast<PROC>(-1))
            p = module.symbol<PROC>(name);
        return reinterpret_cast<T>(p);
    }
    void load() {
        std::wstring directory(32768, L'\0');
        const UINT size = GetSystemDirectoryW(directory.data(), UINT(directory.size()));
        if (!size || size >= directory.size()) throw std::runtime_error("Cannot find Windows system directory.");
        directory.resize(size);
        module.open(std::filesystem::path(directory) / "opengl32.dll");
        wglGetProc = module.symbol<WglGetProc>("wglGetProcAddress");
        ClearColor = resolve<decltype(ClearColor)>("glClearColor");
        Clear = resolve<decltype(Clear)>("glClear");
        Enable = resolve<decltype(Enable)>("glEnable");
        Disable = resolve<decltype(Disable)>("glDisable");
        DepthMask = resolve<decltype(DepthMask)>("glDepthMask");
        DepthFunc = resolve<decltype(DepthFunc)>("glDepthFunc");
        Viewport = resolve<decltype(Viewport)>("glViewport");
        PolygonMode = resolve<decltype(PolygonMode)>("glPolygonMode");
        DrawElements = resolve<decltype(DrawElements)>("glDrawElements");
        GenTextures = resolve<decltype(GenTextures)>("glGenTextures");
        BindTexture = resolve<decltype(BindTexture)>("glBindTexture");
        TexParameteri = resolve<decltype(TexParameteri)>("glTexParameteri");
        TexImage2D = resolve<decltype(TexImage2D)>("glTexImage2D");
        PixelStorei = resolve<decltype(PixelStorei)>("glPixelStorei");
        DeleteTextures = resolve<decltype(DeleteTextures)>("glDeleteTextures");
        GetIntegerv = resolve<decltype(GetIntegerv)>("glGetIntegerv");
        GetError = resolve<decltype(GetError)>("glGetError");
        LineWidth = resolve<decltype(LineWidth)>("glLineWidth");
        GenVertexArrays = resolve<decltype(GenVertexArrays)>("glGenVertexArrays");
        BindVertexArray = resolve<decltype(BindVertexArray)>("glBindVertexArray");
        DeleteVertexArrays = resolve<decltype(DeleteVertexArrays)>("glDeleteVertexArrays");
        GenBuffers = resolve<decltype(GenBuffers)>("glGenBuffers");
        BindBuffer = resolve<decltype(BindBuffer)>("glBindBuffer");
        BufferData = resolve<decltype(BufferData)>("glBufferData");
        DeleteBuffers = resolve<decltype(DeleteBuffers)>("glDeleteBuffers");
        VertexAttribPointer = resolve<decltype(VertexAttribPointer)>("glVertexAttribPointer");
        EnableVertexAttribArray = resolve<decltype(EnableVertexAttribArray)>("glEnableVertexAttribArray");
        CreateShader = resolve<decltype(CreateShader)>("glCreateShader");
        ShaderSource = resolve<decltype(ShaderSource)>("glShaderSource");
        CompileShader = resolve<decltype(CompileShader)>("glCompileShader");
        GetShaderiv = resolve<decltype(GetShaderiv)>("glGetShaderiv");
        GetShaderInfoLog = resolve<decltype(GetShaderInfoLog)>("glGetShaderInfoLog");
        DeleteShader = resolve<decltype(DeleteShader)>("glDeleteShader");
        CreateProgram = resolve<decltype(CreateProgram)>("glCreateProgram");
        AttachShader = resolve<decltype(AttachShader)>("glAttachShader");
        LinkProgram = resolve<decltype(LinkProgram)>("glLinkProgram");
        GetProgramiv = resolve<decltype(GetProgramiv)>("glGetProgramiv");
        GetProgramInfoLog = resolve<decltype(GetProgramInfoLog)>("glGetProgramInfoLog");
        DetachShader = resolve<decltype(DetachShader)>("glDetachShader");
        DeleteProgram = resolve<decltype(DeleteProgram)>("glDeleteProgram");
        GetUniformLocation = resolve<decltype(GetUniformLocation)>("glGetUniformLocation");
        UseProgram = resolve<decltype(UseProgram)>("glUseProgram");
        UniformMatrix4fv = resolve<decltype(UniformMatrix4fv)>("glUniformMatrix4fv");
        UniformMatrix3fv = resolve<decltype(UniformMatrix3fv)>("glUniformMatrix3fv");
        Uniform3f = resolve<decltype(Uniform3f)>("glUniform3f");
        Uniform1i = resolve<decltype(Uniform1i)>("glUniform1i");
        ActiveTexture = resolve<decltype(ActiveTexture)>("glActiveTexture");
    }
};
inline GlApi& gl() { static GlApi api; return api; }
} // namespace graphics::detail
#undef GRAPHICS_WIDEN
#undef GRAPHICS_WIDEN_IMPL
```

## framework/detail/Renderer.inl

고정 도형 드로우를 일반 인덱스 메시·텍스처·경계 드로우로 확장한다.

### before

없음.

### after

```
#pragma once
#include "Shaders.h"
#include "../dependencies/glm/glm/gtc/type_ptr.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace graphics {
inline std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream input(path,std::ios::binary);
    if(!input) return {};
    std::ostringstream buffer;buffer<<input.rdbuf();auto text=buffer.str();
    if(text.compare(0,3,"\xEF\xBB\xBF")==0) text.erase(0,3);
    return text;
}
namespace detail {
inline GLuint compileShader(GLenum kind,const std::string& sourceText) {
    auto& api=gl();const auto shader=api.CreateShader(kind);if(!shader)return 0;
    const char* text=sourceText.c_str();api.ShaderSource(shader,1,&text,nullptr);api.CompileShader(shader);
    GLint success=0;api.GetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success) {
        GLint length=0;api.GetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
        std::string log(std::max(length,1), '\0');api.GetShaderInfoLog(shader,GLsizei(log.size()),nullptr,log.data());
        std::cerr<<"Shader compile failed: "<<log<<'\n';api.DeleteShader(shader);return 0;
    }
    return shader;
}
struct GpuVertex { float position[3], color[3], uv[2], normal[3]; };
inline GLuint uploadTexture(const TextureImage& image,bool repeat) {
    auto& api=gl();GLuint id=0;api.GenTextures(1,&id);if(!id)return 0;
    api.ActiveTexture(GL_TEXTURE0);api.BindTexture(GL_TEXTURE_2D,id);
    GLint previousAlignment=4;api.GetIntegerv(GL_UNPACK_ALIGNMENT,&previousAlignment);
    api.PixelStorei(GL_UNPACK_ALIGNMENT,1);
    api.TexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,GLsizei(image.width),GLsizei(image.height),0,GL_RGBA,GL_UNSIGNED_BYTE,image.rgba.data());
    api.PixelStorei(GL_UNPACK_ALIGNMENT,previousAlignment);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
    if(api.GetError()!=GL_NO_ERROR) {api.DeleteTextures(1,&id);return 0;}
    return id;
}
} // namespace detail

inline bool Renderer::initShader(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    auto& gl=detail::gl();
    const auto vertexText=vertexPath.empty()?std::string(detail::vertexShader):readTextFile(vertexPath);
    const auto fragmentText=fragmentPath.empty()?std::string(detail::fragmentShader):readTextFile(fragmentPath);
    if(vertexText.empty()||fragmentText.empty()) return false;
    const auto vertex=detail::compileShader(GL_VERTEX_SHADER,vertexText);if(!vertex)return false;
    const auto fragment=detail::compileShader(GL_FRAGMENT_SHADER,fragmentText);
    if(!fragment){gl.DeleteShader(vertex);return false;}
    shaderProgramID_=gl.CreateProgram();
    if(!shaderProgramID_){gl.DeleteShader(vertex);gl.DeleteShader(fragment);return false;}
    gl.AttachShader(shaderProgramID_,vertex);gl.AttachShader(shaderProgramID_,fragment);gl.LinkProgram(shaderProgramID_);
    gl.DeleteShader(vertex);gl.DeleteShader(fragment);
    GLint success=0;gl.GetProgramiv(shaderProgramID_,GL_LINK_STATUS,&success);
    if(!success) {
        GLint size=0;gl.GetProgramiv(shaderProgramID_,GL_INFO_LOG_LENGTH,&size);
        std::string log(std::max(size,1),'\0');gl.GetProgramInfoLog(shaderProgramID_,GLsizei(log.size()),nullptr,log.data());
        std::cerr<<"Shader link failed: "<<log<<'\n';return false;
    }
    mvpLocation_=gl.GetUniformLocation(shaderProgramID_,"uMVP");
    modelLocation_=gl.GetUniformLocation(shaderProgramID_,"uModel");
    normalLocation_=gl.GetUniformLocation(shaderProgramID_,"uNormalMatrix");
    textureLocation_=gl.GetUniformLocation(shaderProgramID_,"uTexture");
    hasTextureLocation_=gl.GetUniformLocation(shaderProgramID_,"uHasTexture");
    lightingLocation_=gl.GetUniformLocation(shaderProgramID_,"uLighting");
    lightDirectionLocation_=gl.GetUniformLocation(shaderProgramID_,"uLightDirection");
    lightColorLocation_=gl.GetUniformLocation(shaderProgramID_,"uLightColor");
    overrideLocation_=gl.GetUniformLocation(shaderProgramID_,"uOverrideColor");
    outlineColorLocation_=gl.GetUniformLocation(shaderProgramID_,"uOutlineColor");
    return mvpLocation_>=0;
}
inline bool Renderer::initialize(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    detail::gl().load();destroy();
    if(!initShader(vertexPath,fragmentPath)){destroy();return false;}
    auto& gl=detail::gl();gl.GenVertexArrays(1,&vao_);gl.GenBuffers(1,&vbo_);gl.GenBuffers(1,&ebo_);
    if(!vao_||!vbo_||!ebo_){destroy();return false;}
    gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    using V=detail::GpuVertex;
    gl.VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,position)));
    gl.VertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,color)));
    gl.VertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,uv)));
    gl.VertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,normal)));
    for(GLuint i=0;i<4;++i)gl.EnableVertexAttribArray(i);
    gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);return true;
}
inline bool Renderer::upload(const Scene& scene,bool textureMode) {
    std::vector<detail::GpuVertex> vertices;
    std::vector<GLuint> indices;std::vector<DrawRange> ranges;
    TextureMap stagedTextures;auto& gl=detail::gl();
    auto cleanup=[&](){for(const auto& item:stagedTextures)gl.DeleteTextures(1,&item.second);};
    try {
        for(auto id:scene.getIds()) {
            const auto* c=scene.getComponent(id);if(!c||!DiagramSystem::validPolyhedron(c->polyhedron)){cleanup();return false;}
            for(std::size_t faceIndex=0;faceIndex<c->polyhedron.faces.size();++faceIndex) {
                const auto& face=c->polyhedron.faces[faceIndex];
                if(vertices.size()+face.localVertices.size()>std::numeric_limits<GLuint>::max()) {cleanup();return false;}
                const auto first=GLuint(vertices.size());
                Mesh normals=face;if(normals.vertexNormals.empty())recalculateNormals(normals);
                for(std::size_t i=0;i<face.localVertices.size();++i) {
                    const auto p=face.localVertices[i];const auto n=normals.vertexNormals[i];
                    auto color=face.vertexColors.empty()?face.color:face.vertexColors[i];
                    color.red*=face.material.color.red;color.green*=face.material.color.green;color.blue*=face.material.color.blue;
                    const auto uv=face.texcoords.empty()?glm::vec2(0):face.texcoords[i];
                    vertices.push_back({{p.x,p.y,p.z},{color.red,color.green,color.blue},{uv.x,uv.y},{n.x,n.y,n.z}});
                }
                const auto triangles=detail::triangles(face);
                const auto outline=meshOutline(face,face.outline==OutlineMode::None?OutlineMode::FeatureEdges:face.outline);
                if(triangles.size()>std::numeric_limits<GLsizei>::max()||outline.size()>std::numeric_limits<GLsizei>::max()){cleanup();return false;}
                DrawRange range;range.entityId=id;range.faceIndex=faceIndex;range.firstIndex=indices.size();range.count=GLsizei(triangles.size());
                for(auto index:triangles)indices.push_back(first+index);
                range.outlineFirstIndex=indices.size();range.outlineCount=GLsizei(outline.size());
                for(auto index:outline)indices.push_back(first+index);
                ranges.push_back(range);
                if(textureMode&&face.material.useTexture&&stagedTextures.find(face.material.texture)==stagedTextures.end()) {
                    while(gl.GetError()!=GL_NO_ERROR){}
                    const auto texture=detail::uploadTexture(*face.material.texture,face.material.repeatTexture);
                    if(!texture){cleanup();return false;}
                    try{stagedTextures.emplace(face.material.texture,texture);}catch(...){gl.DeleteTextures(1,&texture);throw;}
                }
            }
        }
        for(std::size_t lineIndex=0;lineIndex<scene.getLines().size();++lineIndex) {
            const auto& line=scene.getLines()[lineIndex];
            if(vertices.size()>std::numeric_limits<GLuint>::max()-2){cleanup();return false;}
            const auto first=GLuint(vertices.size());
            for(auto p:{line.start,line.end})vertices.push_back({{p.x,p.y,p.z},{line.color.red,line.color.green,line.color.blue},{0,0},{0,0,0}});
            DrawRange range;range.lineId=lineIndex;range.firstIndex=indices.size();range.count=2;
            ranges.push_back(range);indices.insert(indices.end(),{first,first+1});
        }
        if(vertices.size()>std::size_t(std::numeric_limits<GLsizeiptr>::max())/sizeof(detail::GpuVertex)
            ||indices.size()>std::size_t(std::numeric_limits<GLsizeiptr>::max())/sizeof(GLuint)){cleanup();return false;}
        while(gl.GetError()!=GL_NO_ERROR){}
        gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);
        gl.BufferData(GL_ARRAY_BUFFER,GLsizeiptr(vertices.size()*sizeof(detail::GpuVertex)),vertices.empty()?nullptr:vertices.data(),GL_STATIC_DRAW);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
        gl.BufferData(GL_ELEMENT_ARRAY_BUFFER,GLsizeiptr(indices.size()*sizeof(GLuint)),indices.empty()?nullptr:indices.data(),GL_STATIC_DRAW);
        gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);
        if(gl.GetError()!=GL_NO_ERROR){cleanup();return false;}
        for(const auto& item:textures_)gl.DeleteTextures(1,&item.second);
        textures_=std::move(stagedTextures);drawRanges_=std::move(ranges);return true;
    } catch(...) {cleanup();throw;}
}
inline bool Renderer::draw(const Scene& scene,const glm::mat4& view,const glm::mat4& projection,
                           const RenderOptions& options) const {
    if(!detail::finiteMatrix(view)||!detail::finiteMatrix(projection))return false;
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) matrix=DiagramSystem::modelMatrix(scene.getComponent(*line.owner)->transform);
        } else {
            const auto* c=scene.getComponent(range.entityId);
            matrix=DiagramSystem::faceModelMatrix(*c,c->polyhedron.faces[range.faceIndex]);
        }
        if(!detail::finiteMatrix(matrix)||!detail::finiteMatrix(projection*view*matrix))return false;
    }
    auto& gl=detail::gl();gl.UseProgram(shaderProgramID_);gl.BindVertexArray(vao_);
    gl.ActiveTexture(GL_TEXTURE0);gl.Uniform1i(textureLocation_,0);
    gl.Uniform3f(lightDirectionLocation_,options.lightDirection.x,options.lightDirection.y,options.lightDirection.z);
    gl.Uniform3f(lightColorLocation_,options.lightColor.red,options.lightColor.green,options.lightColor.blue);
    gl.Uniform3f(outlineColorLocation_,options.outlineColor.red,options.outlineColor.green,options.outlineColor.blue);
    for(const auto& range:drawRanges_) {
        glm::mat4 model(1);const Mesh* face=nullptr;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;model=DiagramSystem::modelMatrix(owner->transform);}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;
            model=DiagramSystem::faceModelMatrix(*c,*face);
        }
        const auto mvp=projection*view*model;
        const float determinant=glm::determinant(glm::mat3(model));
        const auto normalMatrix=std::isfinite(determinant)&&std::abs(determinant)>1e-12f
            ?glm::transpose(glm::inverse(glm::mat3(model))):glm::mat3(1);
        gl.UniformMatrix4fv(mvpLocation_,1,GL_FALSE,glm::value_ptr(mvp));
        gl.UniformMatrix4fv(modelLocation_,1,GL_FALSE,glm::value_ptr(model));
        gl.UniformMatrix3fv(normalLocation_,1,GL_FALSE,glm::value_ptr(normalMatrix));
        gl.Uniform1i(lightingLocation_,face&&options.lighting);gl.Uniform1i(overrideLocation_,0);
        const bool textured=face&&options.textures&&face->material.useTexture;
        gl.Uniform1i(hasTextureLocation_,textured);
        if(textured){gl.BindTexture(GL_TEXTURE_2D,textures_.at(face->material.texture));
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);}
        else gl.BindTexture(GL_TEXTURE_2D,0);
        if(!face||!options.wireframe)
            gl.DrawElements(face?GL_TRIANGLES:GL_LINES,range.count,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.firstIndex*sizeof(GLuint)));
        if(face&&(options.wireframe||face->outline!=OutlineMode::None)) {
            gl.Uniform1i(hasTextureLocation_,0);gl.Uniform1i(lightingLocation_,0);gl.Uniform1i(overrideLocation_,1);
            gl.DepthFunc(GL_LEQUAL);gl.DepthMask(GL_FALSE);
            gl.DrawElements(GL_LINES,range.outlineCount,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.outlineFirstIndex*sizeof(GLuint)));
            gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);
        }
    }
    gl.Uniform1i(overrideLocation_,0);gl.BindTexture(GL_TEXTURE_2D,0);gl.BindVertexArray(0);gl.UseProgram(0);return gl.GetError()==GL_NO_ERROR;
}
inline bool Renderer::render(const Scene& scene,const Camera& camera,int width,int height,const RenderOptions& options) {
    if(!shaderProgramID_||!vao_)return false;if(width<=0||height<=0)return true;
    if(!CameraSystem::validCamera(camera)||!detail::finiteVector(options.lightDirection)
        ||glm::length(options.lightDirection)<1e-12f||!detail::validColor(options.lightColor)
        ||!detail::validColor(options.outlineColor)
        ||!std::isfinite(options.background.r)||!std::isfinite(options.background.g)
        ||!std::isfinite(options.background.b)||!std::isfinite(options.background.a))return false;
    if(uploadedScene_!=&scene||uploadedRevision_!=scene.getGeometryRevision()||uploadedTextures_!=options.textures) {
        if(!upload(scene,options.textures))return false;
        uploadedScene_=&scene;uploadedRevision_=scene.getGeometryRevision();uploadedTextures_=options.textures;
    }
    auto& gl=detail::gl();while(gl.GetError()!=GL_NO_ERROR){}
    gl.Viewport(0,0,width,height);gl.LineWidth(1);
    if(options.depthTest)gl.Enable(GL_DEPTH_TEST);else gl.Disable(GL_DEPTH_TEST);
    gl.Disable(GL_CULL_FACE);gl.Disable(GL_BLEND);gl.Disable(GL_SCISSOR_TEST);
    gl.PolygonMode(GL_FRONT_AND_BACK,GL_FILL);gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);
    gl.ClearColor(options.background.r,options.background.g,options.background.b,options.background.a);
    gl.Clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    return draw(scene,CameraSystem::viewMatrix(camera),CameraSystem::projectionMatrix(camera,float(width)/height),options);
}
inline void Renderer::destroy() {
    auto& gl=detail::gl();
    if(ebo_)gl.DeleteBuffers(1,&ebo_);if(vbo_)gl.DeleteBuffers(1,&vbo_);if(vao_)gl.DeleteVertexArrays(1,&vao_);
    if(shaderProgramID_)gl.DeleteProgram(shaderProgramID_);
    for(const auto& item:textures_)gl.DeleteTextures(1,&item.second);
    textures_.clear();drawRanges_.clear();ebo_=vbo_=vao_=shaderProgramID_=0;
    uploadedScene_=nullptr;uploadedRevision_=std::numeric_limits<std::uint64_t>::max();uploadedTextures_=false;
}
} // namespace graphics
```

## framework/detail/Scene.inl

CPU Scene 구현을 inline으로 옮기고 검증·갱신·면 분리 계약을 구현한다.

### before

없음.

### after

```
#pragma once
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace graphics {
namespace detail {

inline bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f
        && value.y > 0.0f && value.z > 0.0f;
}

inline std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;
    return ++revision;
}

inline void appendFace(Polyhedron& polyhedron, GLFace shape,
                std::initializer_list<glm::vec3> vertices, GlColor color) {
    PlaneFigure face;
    face.shape = shape;
    face.localVertices = vertices;
    face.color = color;
    face.normal = glm::normalize(glm::cross(
        face.localVertices[1] - face.localVertices[0],
        face.localVertices[2] - face.localVertices[0]));
    polyhedron.faces.push_back(face);
}

} // namespace detail
using detail::finiteVector;
using detail::finiteMatrix;
using detail::validColor;
using detail::positiveLengths;
using detail::nextGeometryRevision;
using detail::appendFace;

inline Scene::Scene() {
    geometryRevision_ = nextGeometryRevision();
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
    pyramidFaceId_.reserve(MAX_ENTITIES * 5);
    lines_.reserve(MAX_LINES);
}

inline std::optional<EntityId> Scene::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (!DiagramSystem::validPolyhedron(polyhedron)
        || !DiagramSystem::validTransform(transform)) {
        return std::nullopt;
    }
    glm::vec3 minimum = polyhedron.faces[0].localVertices[0];
    glm::vec3 maximum = minimum;
    for (const PlaneFigure& face : polyhedron.faces) {
        for (const glm::vec3& vertex : face.localVertices) {
            minimum = glm::min(minimum, vertex);
            maximum = glm::max(maximum, vertex);
        }
    }
    if (!finiteVector(maximum - minimum)) return {};
    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform, maximum - minimum });
    polyId_.push_back(id);
    std::vector<FaceId>* faceIds = nullptr;
    if (polyhedron.shape == GLShape::Cube) faceIds = &cubeFaceId_;
    if (polyhedron.shape == GLShape::Tetra) faceIds = &tetraFaceId_;
    if (polyhedron.shape == GLShape::Pyramid) faceIds = &pyramidFaceId_;
    if (faceIds) for (std::size_t i = 0; i < polyhedron.faces.size(); ++i)
        faceIds->push_back({id, i});
    geometryRevision_ = nextGeometryRevision();
    return id;
}

inline std::optional<EntityId> Scene::addCuboid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths),
                       transform);
}

inline std::optional<EntityId> Scene::addRightTetrahedron(
    const glm::vec3& edgeLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths),
                       transform);
}

inline std::optional<EntityId> Scene::addPyramid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths),
                       transform);
}

inline bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;
    return true;
}

inline bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex,
                               const glm::mat4& matrix) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !finiteMatrix(matrix)) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    return true;
}

inline bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex,
                                const std::vector<GlColor>& colors) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    if (!colors.empty() && colors.size() != face.localVertices.size()) {
        return false;
    }
    for (GlColor color : colors) {
        if (!validColor(color)) {
            return false;
        }
    }
    face.vertexColors = colors;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

inline const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

inline bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return false;
    }
    auto candidate = components_[id].transform;
    candidate.basis = worldPosition;
    return setTransform(id, candidate);
}

inline bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if(id>=components_.size()||!positiveLengths(sideLengths))return false;
    const auto& c=components_[id];
    auto replacement=c.polyhedron;
    const auto box=detail::bounds(replacement);const auto oldSize=box.second-box.first;
    if(!positiveLengths(oldSize))return false;
    const auto ratio=sideLengths/oldSize;
    if(!finiteVector(ratio))return false;
    // Preserve topology/materials. Resize mesh coordinates around the model pivot.
    for(auto& face:replacement.faces) {
        for(auto& vertex:face.localVertices)vertex=c.transform.localBasis+(vertex-c.transform.localBasis)*ratio;
        face.transform.localBasis=c.transform.localBasis+(face.transform.localBasis-c.transform.localBasis)*ratio;
        recalculateNormals(face);
    }
    return setModel(id,replacement);
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true;
}

inline bool Scene::setFaceTransform(
    EntityId id, std::size_t faceIndex,
    const Transform3D& transform, bool independence
) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    face.transform = transform;
    face.independence = independence;
    return true;
}

inline std::optional<LineId> Scene::addLine(
    const glm::vec3& start, const glm::vec3& end,
    GlColor color, std::optional<EntityId> owner
) {
    if (!finiteVector(start) || !finiteVector(end)
        || !validColor(color) || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    const LineId id = lines_.size();
    lines_.push_back({ start, end, color, owner });
    geometryRevision_ = nextGeometryRevision();
    return id;
}

inline std::optional<LineId> Scene::addVector(
    const glm::vec3& origin, const glm::vec3& vector,
    GlColor color, std::optional<EntityId> owner
) {
    return addLine(origin, origin + vector, color, owner);
}

inline std::optional<std::array<LineId, 3>> Scene::addBasis(
    const glm::vec3& origin, const glm::mat3& basisVectors,
    const glm::vec3& axisLengths, std::optional<EntityId> owner
) {
    if (!finiteVector(origin)
        || !positiveLengths(axisLengths)
        || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    for (int i = 0; i < 3; ++i) {
        if (!finiteVector(basisVectors[i])
            || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f
            || !finiteVector(origin + basisVectors[i] * axisLengths[i])) {
            return std::nullopt;
        }
    }
    // 사전 검증 후 세 선을 함께 등록한다. 실패 입력은 장면을 변경하지 않는다.
    const std::array<GlColor, 3> colors{
        GlColor{ 1.0f, 0.2f, 0.2f },
        GlColor{ 0.2f, 1.0f, 0.2f },
        GlColor{ 0.2f, 0.4f, 1.0f }
    };
    std::array<LineId, 3> ids{};
    for (int i = 0; i < 3; ++i) {
        ids[i] = lines_.size();
        lines_.push_back({
            origin, origin + basisVectors[i] * axisLengths[i], colors[i], owner });
    }
    geometryRevision_ = nextGeometryRevision();
    return ids;
}

inline bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin)
        || !finiteVector(origin + vector)) {
        return false;
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

inline void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    cubeFaceId_.clear();
    tetraFaceId_.clear();
    pyramidFaceId_.clear();
    lines_.clear();
    geometryRevision_ = nextGeometryRevision();
}

inline bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale)
        && std::isfinite(transform.orientation.w)
        && std::isfinite(transform.orientation.x)
        && std::isfinite(transform.orientation.y)
        && std::isfinite(transform.orientation.z)
        && std::isfinite(glm::dot(transform.orientation, transform.orientation))
        && glm::dot(transform.orientation, transform.orientation) > 0.000001f
        && finiteMatrix(detail::transformMatrix(transform));
}

inline bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    if (polyhedron.faces.empty()) return false;
    for (const auto& face : polyhedron.faces)
        if (!detail::validMesh(face) || !validTransform(face.transform)) return false;
    return true;
}

inline Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
    Polyhedron polyhedron;
    if (!positiveLengths(sideLengths)) {
        return polyhedron;
    }
    polyhedron.shape = shape;
    if (shape == GLShape::Cube) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        polyhedron.faces.reserve(6);
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, z }, { x, -y, z }, { x, y, z }, { -x, y, z } },
            { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, -z }, { -x, -y, -z }, { -x, y, -z }, { x, y, -z } },
            { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { -x, -y, z }, { -x, y, z }, { -x, y, -z } },
            { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, z }, { x, -y, -z }, { x, y, -z }, { x, y, z } },
            { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, y, z }, { x, y, z }, { x, y, -z }, { -x, y, -z } },
            { .3f, .8f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { x, -y, -z }, { x, -y, z }, { -x, -y, z } },
            { .8f, .3f, .9f });
    } else if (shape == GLShape::Pyramid) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        const glm::vec3 frontLeft{ -x, -y, z };
        const glm::vec3 frontRight{ x, -y, z };
        const glm::vec3 backRight{ x, -y, -z };
        const glm::vec3 backLeft{ -x, -y, -z };
        const glm::vec3 apex{ 0.0f, y, 0.0f };
        polyhedron.faces.reserve(5);
        // 앞, 오른쪽, 뒤, 왼쪽, 바닥. 삼각형과 사각형은 바깥쪽 winding이다.
        appendFace(polyhedron, GLFace::Triangle,
            { frontLeft, frontRight, apex }, { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { frontRight, backRight, apex }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { backRight, backLeft, apex }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { backLeft, frontLeft, apex }, { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { backLeft, backRight, frontRight, frontLeft }, { .8f, .3f, .9f });
    } else if (shape == GLShape::Tetra) {
        // 원점에서 직교하는 세 모서리를 만들고, 무게중심을 로컬 원점으로 옮긴다.
        const glm::vec3 center = sideLengths * 0.25f;
        const glm::vec3 origin = -center;
        const glm::vec3 x = glm::vec3(sideLengths.x, 0.0f, 0.0f) - center;
        const glm::vec3 y = glm::vec3(0.0f, sideLengths.y, 0.0f) - center;
        const glm::vec3 z = glm::vec3(0.0f, 0.0f, sideLengths.z) - center;
        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle,
            { origin, y, x }, { .9f, .4f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, x, z }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, z, y }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { x, y, z }, { .9f, .8f, .3f });
    }
    return polyhedron;
}

inline glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    return detail::transformMatrix(transform);
}

inline glm::mat4 DiagramSystem::faceModelMatrix(const Component_Poly& component,
                                         const PlaneFigure& face) {
    return detail::faceMatrix(component, face);
}

inline bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);
    return finiteVector(camera.position) && finiteVector(camera.target)
        && finiteVector(camera.up) && finiteVector(direction) && finiteVector(right)
        && std::isfinite(glm::dot(direction, direction))
        && std::isfinite(glm::dot(right, right))
        && glm::dot(direction, direction) > 0.000001f
        && glm::dot(right, right) > 0.000001f
        && std::isfinite(camera.fieldOfViewDegrees)
        && camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f
        && std::isfinite(camera.nearPlane) && std::isfinite(camera.farPlane)
        && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane
        && std::isfinite(camera.orthographicHalfExtent)
        && camera.orthographicHalfExtent > 0.0f;
}

inline glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

inline glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                          camera.nearPlane, camera.farPlane);
    }
    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect,
                            camera.nearPlane, camera.farPlane);
}


inline void Scene::rebuildFaceIds() {
    cubeFaceId_.clear(); tetraFaceId_.clear(); pyramidFaceId_.clear();
    for (EntityId id : polyId_) {
        const auto& model = components_[id].polyhedron;
        auto* ids = model.shape == GLShape::Cube ? &cubeFaceId_
            : model.shape == GLShape::Tetra ? &tetraFaceId_
            : model.shape == GLShape::Pyramid ? &pyramidFaceId_ : nullptr;
        if(ids) for(std::size_t f=0; f<model.faces.size(); ++f) ids->push_back({id,f});
    }
}
inline std::optional<EntityId> Scene::addModel(const Model& model, const Transform3D& transform,
                                             PivotMode pivot, const glm::vec3& customPivot) {
    if(!DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot)) return {};
    auto initial=transform;
    if(pivot==PivotMode::BottomCenter) initial.localBasis=detail::bottomCenter(model);
    else if(pivot==PivotMode::Custom) initial.localBasis=customPivot;
    auto id=push_entity(model,initial);
    if(id) components_[*id].pivotMode=pivot;
    return id;
}
inline bool Scene::setModel(EntityId id, const Model& model) {
    if(id>=components_.size() || !DiagramSystem::validPolyhedron(model)) return false;
    const auto box=detail::bounds(model);
    if(!detail::finiteVector(box.second-box.first)) return false;
    auto replacement=components_[id];
    replacement.polyhedron=model;replacement.sideLengths=box.second-box.first;
    if(replacement.pivotMode==PivotMode::BottomCenter)
        replacement.transform.localBasis=detail::bottomCenter(model);
    components_[id]=std::move(replacement);
    rebuildFaceIds();geometryRevision_=nextGeometryRevision();return true;
}
inline const Mesh* Scene::getMesh(EntityId id, std::size_t meshIndex) const {
    const auto* c=getComponent(id);
    return c && meshIndex<c->polyhedron.faces.size()?&c->polyhedron.faces[meshIndex]:nullptr;
}
inline bool Scene::setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh) {
    if(!getMesh(id,meshIndex)) return false;
    Model replacement=components_[id].polyhedron;
    replacement.faces[meshIndex]=mesh;
    return setModel(id,replacement);
}
inline bool Scene::setMeshVertices(EntityId id,std::size_t meshIndex,
                                   const std::vector<Vertex>& vertices,
                                   const std::vector<std::uint32_t>& triangles) {
    const auto* original=getMesh(id,meshIndex);if(!original)return false;
    auto replacement=*original;
    const auto geometry=Mesh::fromVertices(vertices,triangles);
    replacement.localVertices=geometry.localVertices;replacement.vertexNormals=geometry.vertexNormals;
    replacement.texcoords=geometry.texcoords;replacement.vertexColors=geometry.vertexColors;
    replacement.triangleIndices=triangles;replacement.shape=GLFace::Mesh;
    replacement.outlineIndices.clear();
    if(replacement.outline==OutlineMode::Explicit) replacement.outline=OutlineMode::None;
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setVertexPosition(EntityId id,std::size_t meshIndex,std::size_t vertexIndex,
                                     const glm::vec3& position) {
    const auto* mesh=getMesh(id,meshIndex);
    if(!mesh||vertexIndex>=mesh->localVertices.size()||!detail::finiteVector(position))return false;
    auto replacement=*mesh;replacement.localVertices[vertexIndex]=position;
    recalculateNormals(replacement);
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setTriangleIndices(EntityId id,std::size_t meshIndex,
                                      const std::vector<std::uint32_t>& indices) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.shape=GLFace::Mesh;replacement.triangleIndices=indices;
    if(!detail::validMesh(replacement))return false;
    recalculateNormals(replacement);return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setTexcoords(EntityId id,std::size_t meshIndex,const std::vector<glm::vec2>& uvs) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.texcoords=uvs;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setVertexNormals(EntityId id,std::size_t meshIndex,const std::vector<glm::vec3>& normals) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.vertexNormals=normals;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setMaterial(EntityId id,std::size_t meshIndex,const Material& material) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.material=material;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setOutline(EntityId id,std::size_t meshIndex,OutlineMode mode,
                              const std::vector<std::uint32_t>& explicitEdges) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.outline=mode;replacement.outlineIndices=explicitEdges;
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setLocalBasis(EntityId id,PivotMode mode,const glm::vec3& customPivot) {
    if(id>=components_.size()||!detail::finiteVector(customPivot))return false;
    auto& c=components_[id];
    if(mode==PivotMode::BottomCenter)c.transform.localBasis=detail::bottomCenter(c.polyhedron);
    else if(mode==PivotMode::Custom)c.transform.localBasis=customPivot;
    c.pivotMode=mode;return true;
}
inline bool Scene::setVisible(EntityId id,bool visible) {
    if(id>=components_.size())return false;
    components_[id].visible=visible;return true;
}
inline bool Scene::splitFace(EntityId id,std::size_t faceIndex) {
    if(!getMesh(id,faceIndex))return false;
    // The same face remains under its parent; vertices and indices are already face-local storage.
    auto& face=components_[id].polyhedron.faces[faceIndex];
    if(!face.independence) {face.transform=Transform3D{};face.independence=true;}
    return true;
}
inline bool Scene::setFaceSRT(EntityId id,std::size_t faceIndex,const Transform3D& transform) {
    return setFaceTransform(id,faceIndex,transform,true);
}
} // namespace graphics
```

## framework/detail/Shaders.h

기본 셰이더를 코드에 포함해 경로 설정 없이 사용한다.

### before

없음.

### after

```
#pragma once
namespace graphics::detail {
inline constexpr char vertexShader[] = R"GLSL(#version 330 core
layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
layout(location=2) in vec2 vTexCoord;
layout(location=3) in vec3 vNormal;
uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;
out vec3 outColor;
out vec2 TexCoord;
out vec3 Normal;
void main() {
    gl_Position = uMVP * vec4(vPos, 1.0);
    outColor = vColor;
    TexCoord = vTexCoord;
    Normal = uNormalMatrix * vNormal;
}
)GLSL";
inline constexpr char fragmentShader[] = R"GLSL(#version 330 core
in vec3 outColor;
in vec2 TexCoord;
in vec3 Normal;
uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform bool uLighting;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform bool uOverrideColor;
uniform vec3 uOutlineColor;
out vec4 FragColor;
void main() {
    if (uOverrideColor) { FragColor = vec4(uOutlineColor, 1.0); return; }
    vec4 color = vec4(outColor, 1.0);
    if (uHasTexture) color *= texture(uTexture, TexCoord);
    if (uLighting && length(Normal) > 0.0)
        color.rgb *= (0.25 + 0.75 * max(dot(normalize(Normal), normalize(uLightDirection)), 0.0)) * uLightColor;
    FragColor = color;
}
)GLSL";
}
```

## framework/examples/assets.cpp

두 로더와 상대경로·텍스처 옵트인의 호출 예제를 제공한다.

### before

없음.

### after

```
#include "../Framework.h"
#include <iostream>

// Usage, with the project root as working directory:
// assets.exe primitive framework/examples/data/cube.txt
// assets.exe primitive framework/examples/data/cube.txt textures/example.png
// assets.exe model models/example.obj
// assets.exe model models/example.gltf textures/override.png
int main(int argc,char** argv) {
    if(argc<3){std::cerr<<"Use: assets.exe primitive|model relative-model-path [relative-texture-path]\n";return 1;}
    const std::string mode=argv[1];
    if(mode!="primitive"&&mode!="model"){std::cerr<<"Unknown mode\n";return 1;}
    const auto path=std::filesystem::u8path(argv[2]);
    const auto texturePath=argc>3?std::filesystem::u8path(argv[3]):std::filesystem::path{};
    graphics::ApplicationConfig config;
    config.projectRoot=std::filesystem::current_path();
    config.features.models=mode=="model";
    config.features.textures=!texturePath.empty();
    // To load model-referenced/embedded textures without an override, opt in here:
    // config.features.textures=true;
    graphics::Application app;
    app.setInitHandler([&](graphics::Application& app) {
        graphics::LoadOptions options;
        options.outline=mode=="primitive"?graphics::OutlineMode::FeatureEdges:graphics::OutlineMode::None;
        options.texturePath=texturePath;
        const auto loaded=mode=="model"?app.getAssets().loadModel(path,options)
                                      :app.getAssets().loadPrimitive(path,options);
        if(!loaded){std::cerr<<loaded.error<<'\n';return false;}
        return app.getScene().addModel(*loaded.model,graphics::Transform3D{},
                                      loaded.pivotMode,loaded.localBasis).has_value();
    });
    app.setMouseHandler([](graphics::Application& app,const graphics::MouseEvent& event) {
        if(event.button==GLFW_MOUSE_BUTTON_LEFT&&event.action==GLFW_PRESS)
            if(const auto hit=app.pickAtCursor())
                std::cout<<"entity="<<hit->entityId<<" mesh="<<hit->faceIndex<<" triangle="<<hit->triangleIndex<<'\n';
    });
    app.setKeyHandler([](graphics::Application& app,int key,int action){
        if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)app.requestClose();
    });
    return app.run(config);
}
```

## framework/examples/basic.cpp

기본 모드·부모 SRT·클릭 면의 추가 SRT 예제를 제공한다.

### before

없음.

### after

```
#include "../Framework.h"
#include <iostream>

int main() {
    graphics::Application app;
    app.setInitHandler([](graphics::Application& app) {
        auto& scene=app.getScene();
        const auto id=scene.addCuboid({1,1,1},{0,0,0});
        if(!id)return false;
        graphics::Transform3D parent;
        parent.rotationDegrees={15,25,0};parent.scale={1.2f,.8f,1};
        if(!scene.setTransform(*id,parent))return false;
        for(std::size_t face=0;face<6;++face)
            if(!scene.setOutline(*id,face,graphics::OutlineMode::FeatureEdges))return false;
        return true;
    });
    app.setMouseHandler([](graphics::Application& app,const graphics::MouseEvent& event) {
        if(event.button!=GLFW_MOUSE_BUTTON_LEFT||event.action!=GLFW_PRESS)return;
        const auto hit=app.pickAt(event.windowX,event.windowY);
        if(!hit)return;
        auto& scene=app.getScene();const auto* face=scene.getMesh(hit->entityId,hit->faceIndex);
        graphics::Transform3D child;
        for(const auto& vertex:face->localVertices)child.localBasis+=vertex;
        child.localBasis/=float(face->localVertices.size());
        child.basis=face->normal*.25f;child.rotationDegrees.z=20;
        scene.splitFace(hit->entityId,hit->faceIndex);
        scene.setFaceSRT(hit->entityId,hit->faceIndex,child);
        std::cout<<"entity="<<hit->entityId<<" face="<<hit->faceIndex<<'\n';
    });
    app.setKeyHandler([](graphics::Application& app,int key,int action) {
        if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)app.requestClose();
    });
    return app.run();
}
```

## framework/examples/data/cube.txt

원본 프로젝트를 바꾸지 않고 기본 도형 데이터 복사본을 제공한다.

### before

없음.

### after

```
v -0.5 -0.5 0.5
v 0.5 -0.5 0.5
v 0.5 0.5 0.5
v -0.5 0.5 0.5
v -0.5 -0.5 -0.5
v 0.5 -0.5 -0.5
v 0.5 0.5 -0.5
v -0.5 0.5 -0.5
vt 0.0 0.0
vt 1.0 0.0
vt 1.0 1.0
vt 0.0 1.0
f 1/1 2/2 3/3
f 1/1 3/3 4/4
f 6/1 5/2 8/3
f 6/1 8/3 7/4
f 4/1 3/2 7/3
f 4/1 7/3 8/4
f 5/1 6/2 2/3
f 5/1 2/3 1/4
f 2/1 6/2 7/3
f 2/1 7/3 3/4
f 5/1 1/2 4/3
f 5/1 4/3 8/4
```

## framework/include/Application.h

로딩 설정·마우스 입력·피킹의 사용자 진입점을 추가한다.

### before

```
#pragma once

#include "Graphics.h"
#include <functional>

struct GLFWwindow;

namespace graphics {

struct ApplicationConfig {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    std::string title = "OpenGL";
    std::filesystem::path shaderDirectory = "framework/shaders";
    bool verticalSync = true;
};

// 콜백에 캡처한 객체는 run()이 끝날 때까지 유지한다.
// 한 스레드에서 Application 하나를 사용한다. GLFW 초기화와 종료를 소유한다.
class Application {
private:
    GLFWwindow* window_ = nullptr;
    bool glfwInitialized_ = false;
    bool running_ = false;
    Scene scene_;
    Camera camera_;
    Renderer renderer_;
    RenderOptions renderOptions_;
    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
    std::function<void(Application&, int, int)> keyHandler_;
    std::function<void(Application&, int, int, int)> modifiedKeyHandler_;
    std::function<void(Application&, bool)> focusHandler_;
    std::function<bool(Application&)> renderHandler_;

    bool initialize(const ApplicationConfig& config);
    void shutdown();

public:
    Application() = default;
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void setInitHandler(std::function<bool(Application&)> handler);
    void setUpdateHandler(std::function<void(Application&, float)> handler);
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);
    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    void setModifiedKeyHandler(
        std::function<void(Application&, int key, int action, int mods)> handler);
    static std::filesystem::path findShaderDirectory();
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);
    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);
    int run(const ApplicationConfig& config = {});
    void requestClose();
    bool renderScene();

    RenderOptions& getRenderOptions() { return renderOptions_; }
    Scene& getScene() { return scene_; }
    Camera& getCamera() { return camera_; }
    GLFWwindow* getWindow() const { return window_; }
};

} // namespace graphics
```

### after

```
#pragma once

#include "Graphics.h"
#include "ModelLoader.h"
#include "Picking.h"
#include <functional>
#include <exception>

struct GLFWwindow;

namespace graphics {

struct ApplicationConfig {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    std::string title = "OpenGL";
    std::filesystem::path shaderDirectory; // Empty selects embedded shaders.
    std::filesystem::path projectRoot = std::filesystem::current_path();
    std::filesystem::path runtimeDirectory; // Optional bundled framework directory override.
    FeatureOptions features;
    bool verticalSync = true;
};

struct MouseEvent {
    int button = 0, action = 0, mods = 0;
    double windowX = 0, windowY = 0, framebufferX = 0, framebufferY = 0;
};

// 콜백에 캡처한 객체는 run()이 끝날 때까지 유지한다.
// 한 스레드에서 Application 하나를 사용한다. GLFW 초기화와 종료를 소유한다.
class Application {
private:
    GLFWwindow* window_ = nullptr;
    bool glfwInitialized_ = false;
    bool running_ = false;
    Scene scene_;
    Camera camera_;
    Renderer renderer_;
    RenderOptions renderOptions_;
    std::optional<AssetLoader> assets_;
    std::exception_ptr callbackError_;
    std::function<void(Application&, const MouseEvent&)> mouseHandler_;
    std::function<void(Application&, double, double)> cursorHandler_;
    template<class Handler> void dispatch(Handler&& handler) noexcept {
        if(callbackError_) return;
        try { handler(); } catch(...) { callbackError_=std::current_exception(); requestClose(); }
    }
    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
    std::function<void(Application&, int, int)> keyHandler_;
    std::function<void(Application&, int, int, int)> modifiedKeyHandler_;
    std::function<void(Application&, bool)> focusHandler_;
    std::function<bool(Application&)> renderHandler_;

    bool initialize(const ApplicationConfig& config);
    void shutdown();

public:
    Application() = default;
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void setInitHandler(std::function<bool(Application&)> handler);
    void setUpdateHandler(std::function<void(Application&, float)> handler);
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);
    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    void setModifiedKeyHandler(
        std::function<void(Application&, int key, int action, int mods)> handler);
    static std::filesystem::path findShaderDirectory();
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);
    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);
    void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);
    void setCursorHandler(std::function<void(Application&, double x, double y)> handler);
    std::optional<PickResult> pickAt(double windowX, double windowY) const;
    std::optional<PickResult> pickAtCursor() const;
    AssetLoader& getAssets(); // Available from initialization callback until run() ends.
    int run(const ApplicationConfig& config = ApplicationConfig{});
    void requestClose();
    bool renderScene();

    RenderOptions& getRenderOptions() { return renderOptions_; }
    Scene& getScene() { return scene_; }
    Camera& getCamera() { return camera_; }
    GLFWwindow* getWindow() const { return window_; }
};

} // namespace graphics

#include "../detail/Application.inl"
```

## framework/include/Geometry.h

법선·삼각화·경계 계산을 렌더러와 분리한다.

### before

없음.

### after

```
#pragma once
#include "Model.h"
#include <algorithm>
#include <map>
#include <numeric>
#include <stdexcept>

namespace graphics {
inline void recalculateNormals(Mesh& mesh) {
    mesh.vertexNormals.assign(mesh.localVertices.size(), glm::vec3(0));
    const auto indices = detail::triangles(mesh);
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i+1], c = indices[i+2];
        const auto n = glm::cross(mesh.localVertices[b] - mesh.localVertices[a],
                                  mesh.localVertices[c] - mesh.localVertices[a]);
        mesh.vertexNormals[a] += n; mesh.vertexNormals[b] += n; mesh.vertexNormals[c] += n;
    }
    for (auto& n : mesh.vertexNormals) {
        const float size = glm::length(n);
        n = size > 1e-12f ? n / size : glm::vec3(0);
    }
}

// Weld equal positions for edge topology, while preserving separate UV/normal vertices.
inline std::vector<std::uint32_t> meshOutline(const Mesh& mesh,
                                             OutlineMode mode = OutlineMode::FeatureEdges) {
    if (mode == OutlineMode::None) return {};
    if (mode == OutlineMode::Explicit) return mesh.outlineIndices;
    using Position = std::array<float, 3>;
    std::map<Position, std::uint32_t> points;
    std::vector<std::uint32_t> welded;
    for (std::uint32_t i = 0; i < mesh.localVertices.size(); ++i) {
        const auto& p = mesh.localVertices[i];
        const auto result = points.emplace(Position{p.x, p.y, p.z}, i);
        welded.push_back(result.first->second);
    }
    struct Edge { std::uint32_t a = 0, b = 0; std::vector<glm::vec3> normals; };
    std::map<std::pair<std::uint32_t, std::uint32_t>, Edge> edges;
    const auto indices = detail::triangles(mesh);
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i+1], c = indices[i+2];
        auto normal = glm::cross(mesh.localVertices[b] - mesh.localVertices[a],
                                 mesh.localVertices[c] - mesh.localVertices[a]);
        const auto length = glm::length(normal);
        if (length <= 1e-12f) continue;
        normal /= length;
        const std::array<std::uint32_t, 3> corners{a,b,c};
        for (int e = 0; e < 3; ++e) {
            auto first = welded[corners[e]], second = welded[corners[(e+1)%3]];
            if (first == second) continue;
            if (first > second) std::swap(first, second);
            auto& edge = edges[{first, second}];
            edge.a = first; edge.b = second; edge.normals.push_back(normal);
        }
    }
    const float threshold = std::cos(glm::radians(mesh.creaseAngleDegrees));
    std::vector<std::uint32_t> result;
    for (const auto& item : edges) {
        const auto& edge = item.second;
        const bool show = mode == OutlineMode::AllTriangleEdges || edge.normals.size() != 2
            || glm::dot(edge.normals[0], edge.normals[1]) < threshold;
        if (show) { result.push_back(edge.a); result.push_back(edge.b); }
    }
    return result;
}

namespace detail {
// Ear clipping handles planar simple concave polygons, with either winding.
inline std::vector<std::uint32_t> triangulate(const std::vector<glm::vec3>& points) {
    if (points.size() < 3) throw std::runtime_error("Face has fewer than three corners.");
    glm::vec3 normal(0);
    for (std::size_t i=0; i<points.size(); ++i) {
        const auto& a=points[i]; const auto& b=points[(i+1)%points.size()];
        normal += glm::vec3((a.y-b.y)*(a.z+b.z), (a.z-b.z)*(a.x+b.x), (a.x-b.x)*(a.y+b.y));
    }
    if (glm::length(normal) < 1e-12f) throw std::runtime_error("Degenerate polygon.");
    const auto absNormal=glm::abs(normal);
    const int axis=absNormal.x>absNormal.y ? (absNormal.x>absNormal.z?0:2) : (absNormal.y>absNormal.z?1:2);
    std::vector<glm::dvec2> projected;
    for (const auto& p : points) projected.push_back(axis==0 ? glm::dvec2(p.y,p.z)
        : axis==1 ? glm::dvec2(p.x,p.z) : glm::dvec2(p.x,p.y));
    double area=0;
    for(std::size_t i=0;i<projected.size();++i) {
        const auto& a=projected[i]; const auto& b=projected[(i+1)%projected.size()];
        area+=a.x*b.y-b.x*a.y;
    }
    if (std::abs(area)<1e-14) throw std::runtime_error("Degenerate polygon projection.");
    const double direction=area>0?1:-1;
    auto cross=[](glm::dvec2 a,glm::dvec2 b,glm::dvec2 c) {return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);};
    std::vector<std::uint32_t> polygon(points.size()), result;
    std::iota(polygon.begin(),polygon.end(),0);
    while(polygon.size()>3) {
        bool clipped=false;
        for(std::size_t i=0;i<polygon.size();++i) {
            const auto a=polygon[(i+polygon.size()-1)%polygon.size()],b=polygon[i],c=polygon[(i+1)%polygon.size()];
            if(direction*cross(projected[a],projected[b],projected[c])<=1e-14) continue;
            bool inside=false;
            for(auto p:polygon) if(p!=a&&p!=b&&p!=c
                && direction*cross(projected[a],projected[b],projected[p])>=-1e-14
                && direction*cross(projected[b],projected[c],projected[p])>=-1e-14
                && direction*cross(projected[c],projected[a],projected[p])>=-1e-14) {inside=true;break;}
            if(inside) continue;
            result.insert(result.end(),{a,b,c}); polygon.erase(polygon.begin()+i); clipped=true; break;
        }
        if(!clipped) throw std::runtime_error("Cannot triangulate polygon; check repeated corners or self-intersections.");
    }
    result.insert(result.end(),polygon.begin(),polygon.end());
    return result;
}
inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first=std::uint32_t(destination.localVertices.size());
    destination.localVertices.insert(destination.localVertices.end(),source.localVertices.begin(),source.localVertices.end());
    destination.vertexNormals.insert(destination.vertexNormals.end(),source.vertexNormals.begin(),source.vertexNormals.end());
    destination.texcoords.insert(destination.texcoords.end(),source.texcoords.begin(),source.texcoords.end());
    for(auto i:triangles(source)) destination.triangleIndices.push_back(first+i);
    destination.shape=GLFace::Mesh;
}
inline bool shareEdge(const Mesh& a,const Mesh& b) {
    const auto ae=meshOutline(a),be=meshOutline(b);
    for(std::size_t i=0;i<ae.size();i+=2) for(std::size_t j=0;j<be.size();j+=2) {
        const auto p=a.localVertices[ae[i]],q=a.localVertices[ae[i+1]],r=b.localVertices[be[j]],s=b.localVertices[be[j+1]];
        if((p==r&&q==s)||(p==s&&q==r)) return true;
    }
    return false;
}
inline void mergeCoplanar(Model& model) {
    for(std::size_t i=0;i<model.faces.size();++i) {
        auto& face=model.faces[i];
        if(glm::length(face.normal)<1e-12f) continue;
        for(std::size_t j=i+1;j<model.faces.size();) {
            const auto& other=model.faces[j];
            const bool samePlane=glm::dot(face.normal,other.normal)>1-1e-6f
                && std::abs(glm::dot(face.normal,other.localVertices[0]-face.localVertices[0]))<1e-6f;
            if(samePlane && face.texcoords.empty()==other.texcoords.empty() && shareEdge(face,other)) {
                appendMesh(face,other); model.faces.erase(model.faces.begin()+j); j=i+1;
            } else ++j;
        }
    }
}
} // namespace detail
} // namespace graphics
```

## framework/include/Graphics.h

일반 Scene 편집 및 Renderer API를 노출한다.

### before

```
#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_ENTITIES = 50;
constexpr std::size_t MAX_LINES = 500;

using EntityId = std::size_t;
using LineId = std::size_t;

struct GlColor {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
};

enum class GLShape { None, Cube, Tetra, Pyramid }; // Cube는 직육면체, Tetra는 직사면체.
enum class GLFace { None, Rect, Triangle };

struct Transform3D {
    glm::vec3 basis{ 0.0f }; // 월드 위치. 방향 기저는 addBasis의 mat3로 구분한다.
    glm::vec3 localBasis{ 0.0f };
    glm::vec3 rotationDegrees{ 0.0f };
    glm::vec3 scale{ 1.0f };
    glm::quat orientation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Euler 회전 다음에 적용한다.
};

struct PlaneFigure {
    GLFace shape = GLFace::None;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{ 0.0f }; // 로컬 법선 데이터. 조명은 현재 사용하지 않는다.
    bool independence = false;
    Transform3D transform;   // 독립 면일 때의 월드 변환.
    bool visible = true;
    glm::mat4 localAnimationMatrix{ 1.0f };
    std::vector<GlColor> vertexColors; // 비어 있으면 면의 단색을 사용한다.
};

struct Polyhedron {
    GLShape shape = GLShape::None;
    std::vector<PlaneFigure> faces;
};

struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{ 1.0f }; // scale과 구분한 실제 로컬 모서리 길이.
};

struct FaceId {
    EntityId entityId = 0;
    std::size_t faceIndex = 0;
};

struct Line3D {
    glm::vec3 start{ 0.0f };
    glm::vec3 end{ 0.0f };
    GlColor color;
    std::optional<EntityId> owner; // 없으면 월드 좌표, 있으면 객체의 로컬 좌표.
};

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;

public:
    Scene();
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = {});
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths,
                                     const glm::vec3& worldPosition);
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths,
                                               const glm::vec3& worldPosition);
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths,
                                      const glm::vec3& worldPosition);
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex,
                             const std::vector<GlColor>& colors);
    bool setPosition(EntityId id, const glm::vec3& worldPosition);
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
    bool setTransform(EntityId id, const Transform3D& transform);
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end,
                                 GlColor color, std::optional<EntityId> owner = {});
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector,
                                   GlColor color, std::optional<EntityId> owner = {});
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    std::optional<std::array<LineId, 3>> addBasis(
        const glm::vec3& origin, const glm::mat3& basisVectors = glm::mat3(1.0f),
        const glm::vec3& axisLengths = glm::vec3(1.0f),
        std::optional<EntityId> owner = {});
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.

    const std::vector<EntityId>& getIds() const { return polyId_; }
    const Component_Poly* getComponent(EntityId id) const;
    const std::vector<FaceId>& getCubeFaceIds() const { return cubeFaceId_; }
    const std::vector<FaceId>& getTetraFaceIds() const { return tetraFaceId_; }
    const std::vector<FaceId>& getPyramidFaceIds() const { return pyramidFaceId_; }
    const std::vector<Line3D>& getLines() const { return lines_; }
    std::uint64_t getGeometryRevision() const { return geometryRevision_; }
};

class DiagramSystem {
public:
    static Polyhedron makePolyhedron(GLShape shape,
                                     const glm::vec3& sideLengths = glm::vec3(1.0f));
    static bool validPolyhedron(const Polyhedron& polyhedron);
    static bool validTransform(const Transform3D& transform);
    static glm::mat4 modelMatrix(const Transform3D& transform);
    static glm::mat4 faceModelMatrix(const Component_Poly& component,
                                    const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{ 2.8f, 2.0f, 5.0f };
    glm::vec3 target{ 0.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

class CameraSystem {
public:
    static bool validCamera(const Camera& camera);
    static glm::mat4 viewMatrix(const Camera& camera);
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

std::string readTextFile(const std::filesystem::path& path);

struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
};

class Renderer {
private:
    struct DrawRange {
        GLenum mode = GL_TRIANGLES;
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0;
        GLsizei count = 0;
        std::size_t outlineFirstIndex = 0;
        GLsizei outlineCount = 0;
    };

    GLuint shaderProgramID_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_[2] = { 0, 0 };
    GLuint ebo_ = 0;
    GLint mvpLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();

    bool initShader(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    bool initBuffer();
    bool upload(const Scene& scene);
    bool draw(const Scene& scene, const glm::mat4& view,
              const glm::mat4& projection, const RenderOptions& options) const;

public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // 호출자가 유효한 OpenGL 3.3 컨텍스트를 current로 설정한 뒤 호출한다.
    bool initialize(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    // viewport/depth/clear/program/VAO를 설정한다. 컨텍스트와 윈도우는 소유하지 않는다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = {});
    void destroy(); // 컨텍스트 종료 전에 호출한다. 여러 번 호출해도 된다.
};

} // namespace graphics
```

### after

```
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
    Scene();
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = Transform3D{});
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths,
                                     const glm::vec3& worldPosition);
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths,
                                               const glm::vec3& worldPosition);
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths,
                                      const glm::vec3& worldPosition);
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex,
                             const std::vector<GlColor>& colors);
    bool setPosition(EntityId id, const glm::vec3& worldPosition);
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
    bool setTransform(EntityId id, const Transform3D& transform);
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));
    bool setModel(EntityId id, const Model& model);
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;
    bool setMeshVertices(EntityId id, std::size_t meshIndex,
                         const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);
    bool setVertexPosition(EntityId id, std::size_t meshIndex,
                           std::size_t vertexIndex, const glm::vec3& position);
    bool setTriangleIndices(EntityId id, std::size_t meshIndex,
                            const std::vector<std::uint32_t>& indices);
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));
    bool setVisible(EntityId id, bool visible);
    bool splitFace(EntityId id, std::size_t faceIndex);
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);

    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end,
                                 GlColor color, std::optional<EntityId> owner = std::nullopt);
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector,
                                   GlColor color, std::optional<EntityId> owner = std::nullopt);
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    std::optional<std::array<LineId, 3>> addBasis(
        const glm::vec3& origin, const glm::mat3& basisVectors = glm::mat3(1.0f),
        const glm::vec3& axisLengths = glm::vec3(1.0f),
        std::optional<EntityId> owner = std::nullopt);
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.

    const std::vector<EntityId>& getIds() const { return polyId_; }
    const Component_Poly* getComponent(EntityId id) const;
    const std::vector<FaceId>& getCubeFaceIds() const { return cubeFaceId_; }
    const std::vector<FaceId>& getTetraFaceIds() const { return tetraFaceId_; }
    const std::vector<FaceId>& getPyramidFaceIds() const { return pyramidFaceId_; }
    const std::vector<Line3D>& getLines() const { return lines_; }
    std::uint64_t getGeometryRevision() const { return geometryRevision_; }
};

class DiagramSystem {
public:
    static Polyhedron makePolyhedron(GLShape shape,
                                     const glm::vec3& sideLengths = glm::vec3(1.0f));
    static bool validPolyhedron(const Polyhedron& polyhedron);
    static bool validTransform(const Transform3D& transform);
    static glm::mat4 modelMatrix(const Transform3D& transform);
    static glm::mat4 faceModelMatrix(const Component_Poly& component,
                                    const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{ 2.8f, 2.0f, 5.0f };
    glm::vec3 target{ 0.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

class CameraSystem {
public:
    static bool validCamera(const Camera& camera);
    static glm::mat4 viewMatrix(const Camera& camera);
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

std::string readTextFile(const std::filesystem::path& path);

struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
    bool textures = false; // Explicit opt-in; no texture GPU work when false.
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
};

class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    using TextureMap = std::map<std::shared_ptr<const TextureImage>, GLuint,
                               std::owner_less<std::shared_ptr<const TextureImage>>>;
    TextureMap textures_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;
    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    bool upload(const Scene& scene, bool textures);
    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;
public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{}, const std::filesystem::path& fragmentPath = std::filesystem::path{});
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});
    void destroy(); // Call while the owning context is current, before it is destroyed.
};
} // namespace graphics
#include "../detail/Scene.inl"
#include "../detail/Renderer.inl"
```

## framework/include/Model.h

도형 제한을 임의 메시 데이터로 확장하고 부모·면 변환과 기준점을 정의한다.

### before

없음.

### after

```
#pragma once

#include "../dependencies/glm/glm/glm.hpp"
#include "../dependencies/glm/glm/gtc/quaternion.hpp"
#include "../dependencies/glm/glm/gtc/matrix_transform.hpp"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <utility>

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
// Initial allocation hints, not scene capacity limits.
constexpr std::size_t MAX_ENTITIES = 50;
constexpr std::size_t MAX_LINES = 500;
using EntityId = std::size_t;
using LineId = std::size_t;

struct GlColor { float red = 1.0f, green = 1.0f, blue = 1.0f; };
enum class GLShape { None, Cube, Tetra, Pyramid, Custom, Imported };
enum class GLFace { None, Rect, Triangle, Mesh };
enum class PivotMode { Preserve, BottomCenter, Custom };
enum class OutlineMode { None, FeatureEdges, AllTriangleEdges, Explicit };

struct Transform3D {
    glm::vec3 basis{0.0f};
    glm::vec3 localBasis{0.0f};
    glm::vec3 rotationDegrees{0.0f};
    glm::vec3 scale{1.0f};
    glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
};

struct TextureImage {
    std::uint32_t width = 0, height = 0;
    std::vector<std::uint8_t> rgba;
    // Row zero maps to UV v=0. File decoding flips top-down images by default.
    bool valid() const {
        return width && height && width <= 16384 && height <= 16384
            && rgba.size() == std::size_t(width) * height * 4;
    }
};

struct Material {
    GlColor color;
    std::shared_ptr<const TextureImage> texture;
    bool useTexture = false;
    bool repeatTexture = true;
};

struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f};
    glm::vec2 uv{0.0f};
    GlColor color;
};

struct PlaneFigure {
    GLFace shape = GLFace::Mesh;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{0.0f};
    // Compatibility flag: true enables child-local SRT, never world detachment.
    bool independence = false;
    Transform3D transform;
    bool visible = true;
    glm::mat4 localAnimationMatrix{1.0f};
    std::vector<GlColor> vertexColors;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> texcoords;
    std::vector<std::uint32_t> triangleIndices;
    std::vector<std::uint32_t> outlineIndices; // GL_LINES pairs, mesh-local indices.
    Material material;
    OutlineMode outline = OutlineMode::None;
    float creaseAngleDegrees = 1.0f;
    std::string name;
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices,
                                   const std::vector<std::uint32_t>& triangles) {
        PlaneFigure mesh;
        mesh.triangleIndices = triangles;
        for (const auto& vertex : vertices) {
            mesh.localVertices.push_back(vertex.position);
            mesh.vertexNormals.push_back(vertex.normal);
            mesh.texcoords.push_back(vertex.uv);
            mesh.vertexColors.push_back(vertex.color);
        }
        return mesh;
    }
};
using Mesh = PlaneFigure;

struct Polyhedron {
    GLShape shape = GLShape::Custom;
    std::vector<PlaneFigure> faces;
    std::string name;
    std::vector<Mesh>& meshes() { return faces; }
    const std::vector<Mesh>& meshes() const { return faces; }
};
using Model = Polyhedron;

struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{1.0f};
    bool visible = true;
    PivotMode pivotMode = PivotMode::Preserve;
};
struct FaceId { EntityId entityId = 0; std::size_t faceIndex = 0; };
struct Line3D {
    glm::vec3 start{0.0f}, end{0.0f};
    GlColor color;
    std::optional<EntityId> owner;
};

namespace detail {
inline bool finiteVector(const glm::vec3& v) {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}
inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c) for (int r = 0; r < 4; ++r)
        if (!std::isfinite(m[c][r])) return false;
    return true;
}
inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}
inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
    if (!mesh.triangleIndices.empty()) return mesh.triangleIndices;
    if (mesh.shape == GLFace::Triangle) return {0, 1, 2};
    if (mesh.shape == GLFace::Rect) return {0, 1, 2, 0, 2, 3};
    return {};
}
inline bool validMesh(const Mesh& m) {
    if (m.localVertices.size() < 3 || !validColor(m.color)
        || !validColor(m.material.color) || !finiteVector(m.normal)
        || !finiteMatrix(m.localAnimationMatrix)
        || !std::isfinite(m.creaseAngleDegrees) || m.creaseAngleDegrees < 0
        || m.creaseAngleDegrees > 180) return false;
    const auto count = m.localVertices.size();
    if (count > std::numeric_limits<std::uint32_t>::max()) return false;
    if ((!m.vertexColors.empty() && m.vertexColors.size() != count)
        || (!m.vertexNormals.empty() && m.vertexNormals.size() != count)
        || (!m.texcoords.empty() && m.texcoords.size() != count)
        || (m.material.texture && !m.material.texture->valid())
        || (m.material.useTexture && (!m.material.texture || m.texcoords.empty()))) return false;
    for (const auto& v : m.localVertices) if (!finiteVector(v)) return false;
    for (const auto& v : m.vertexNormals) if (!finiteVector(v)) return false;
    for (const auto& v : m.texcoords) if (!std::isfinite(v.x) || !std::isfinite(v.y)) return false;
    for (auto c : m.vertexColors) if (!validColor(c)) return false;
    const auto indices = triangles(m);
    if (indices.empty() || indices.size() % 3 || m.outlineIndices.size() % 2) return false;
    for (auto i : indices) if (i >= count) return false;
    for (auto i : m.outlineIndices) if (i >= count) return false;
    for (std::size_t i=0;i<indices.size();i+=3) {
        const auto a=glm::dvec3(m.localVertices[indices[i]]);
        const auto b=glm::dvec3(m.localVertices[indices[i+1]]);
        const auto c=glm::dvec3(m.localVertices[indices[i+2]]);
        const auto normal=glm::cross(b-a,c-a);
        const double area=glm::dot(normal,normal);
        if(!std::isfinite(area)||area<=0) return false;
    }
    return true;
}
inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());
    for (const auto& face : model.faces) for (const auto& p : face.localVertices) {
        low = glm::min(low, p); high = glm::max(high, p);
    }
    return {low, high};
}
inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);
    return {box.first.x * .5f + box.second.x * .5f, box.first.y,
            box.first.z * .5f + box.second.z * .5f};
}
inline glm::mat4 transformMatrix(const Transform3D& t) {
    glm::mat4 m = glm::translate(glm::mat4(1), t.basis);
    m = glm::rotate(m, glm::radians(t.rotationDegrees.x), {1, 0, 0});
    m = glm::rotate(m, glm::radians(t.rotationDegrees.y), {0, 1, 0});
    m = glm::rotate(m, glm::radians(t.rotationDegrees.z), {0, 0, 1});
    m *= glm::mat4_cast(glm::normalize(t.orientation));
    return glm::translate(glm::scale(m, t.scale), -t.localBasis);
}
inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
    glm::mat4 child(1);
    if (face.independence) {
        // Rotate/scale around a face pivot without moving an identity child.
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * transformMatrix(face.transform);
    }
    return transformMatrix(c.transform) * child * face.localAnimationMatrix;
}
} // namespace detail
} // namespace graphics
```

## framework/include/ModelLoader.h

기본 도형 텍스트와 Assimp 모델을 서로 다른 옵트인 API로 읽는다.

### before

없음.

### after

```
#pragma once
#include "Geometry.h"
#include "Texture.h"
#include "../dependencies/assimp/include/assimp/cimport.h"
#include "../dependencies/assimp/include/assimp/scene.h"
#include "../dependencies/assimp/include/assimp/postprocess.h"
#include <fstream>
#include <sstream>
#include <functional>
#include <locale>

namespace graphics {
struct FeatureOptions { bool models = false; bool textures = false; };
struct LoadOptions {
    PivotMode pivot = PivotMode::BottomCenter;
    glm::vec3 customPivot{0};
    OutlineMode outline = OutlineMode::None;
    bool mergeCoplanarFaces = true; // primitive text only
    bool flipTextureRows = true;
    std::filesystem::path texturePath; // project-relative explicit override
};
struct ModelResult {
    std::optional<Model> model;
    glm::vec3 localBasis{0};
    PivotMode pivotMode = PivotMode::Preserve;
    std::string error;
    explicit operator bool() const { return model.has_value(); }
};
struct TextureResult {
    std::shared_ptr<const TextureImage> image;
    std::string error;
    explicit operator bool() const { return bool(image); }
};

namespace detail {
struct AssimpApi {
    Module module;
    bool ready = false;
    decltype(&::aiImportFile) importFile = nullptr;
    decltype(&::aiReleaseImport) releaseImport = nullptr;
    decltype(&::aiGetErrorString) getError = nullptr;
    decltype(&::aiGetMaterialColor) getColor = nullptr;
    decltype(&::aiGetMaterialTexture) getTexture = nullptr;
    void load(const std::filesystem::path& root) {
        if(ready) return;
        module.open((root.empty()?frameworkDirectory():std::filesystem::absolute(root))
                    / "dependencies/assimp/bin/assimp-vc145-mt.dll");
        importFile=module.symbol<decltype(importFile)>("aiImportFile");
        releaseImport=module.symbol<decltype(releaseImport)>("aiReleaseImport");
        getError=module.symbol<decltype(getError)>("aiGetErrorString");
        getColor=module.symbol<decltype(getColor)>("aiGetMaterialColor");
        getTexture=module.symbol<decltype(getTexture)>("aiGetMaterialTexture");
        ready=true;
    }
};
inline AssimpApi& assimp() { static AssimpApi api; return api; }
inline glm::mat4 assimpMatrix(const aiMatrix4x4& a) {
    return glm::mat4(a.a1,a.b1,a.c1,a.d1, a.a2,a.b2,a.c2,a.d2,
                     a.a3,a.b3,a.c3,a.d3, a.a4,a.b4,a.c4,a.d4);
}
inline std::size_t objIndex(const std::string& text,std::size_t count) {
    std::size_t consumed=0;
    const auto index=std::stoll(text,&consumed);
    if(consumed!=text.size() || index==0) throw std::runtime_error("Invalid OBJ index.");
    const auto resolved=index>0?index-1:static_cast<long long>(count)+index;
    if(resolved<0 || static_cast<unsigned long long>(resolved)>=count)
        throw std::runtime_error("OBJ index is out of range.");
    return std::size_t(resolved);
}
inline ModelResult finishLoad(Model model,const LoadOptions& options) {
    if(model.faces.empty()) throw std::runtime_error("Model contains no triangle meshes.");
    for(auto& face:model.faces) {
        face.outline=options.outline;
        if(!validMesh(face)) throw std::runtime_error("Invalid model mesh data.");
    }
    ModelResult result;
    if(options.pivot==PivotMode::BottomCenter) result.localBasis=bottomCenter(model);
    else if(options.pivot==PivotMode::Custom) result.localBasis=options.customPivot;
    if(!finiteVector(result.localBasis)) throw std::runtime_error("Invalid local basis.");
    result.pivotMode=options.pivot;
    result.model=std::move(model);
    return result;
}
} // namespace detail

class AssetLoader {
    std::filesystem::path projectRoot_,runtimeRoot_;
    FeatureOptions features_;
    mutable std::map<std::filesystem::path,std::weak_ptr<const TextureImage>> imageCache_;
    std::filesystem::path resolve(const std::filesystem::path& relative) const {
        if(relative.empty() || relative.is_absolute() || relative.has_root_name())
            throw std::runtime_error("Asset path must be project-relative.");
        const auto root=std::filesystem::weakly_canonical(projectRoot_);
        const auto full=std::filesystem::weakly_canonical(root/relative);
        auto a=root.begin(), b=full.begin();
        for(;a!=root.end();++a,++b) if(b==full.end() || *a!=*b)
            throw std::runtime_error("Asset path is outside projectRoot.");
        return full;
    }
    std::shared_ptr<const TextureImage> texture(const std::filesystem::path& relative,bool flip) const {
        if(!features_.textures) throw std::runtime_error("Texture mode is disabled.");
        const auto full=resolve(relative);
        // The row convention is part of the cache key.
        const auto key=full / (flip?"#flipped":"#native");
        if(auto cached=imageCache_[key].lock()) return cached;
        auto image=std::make_shared<const TextureImage>(detail::decodeImage(full,nullptr,flip));
        imageCache_[key]=image;
        return image;
    }
public:
    explicit AssetLoader(std::filesystem::path projectRoot=std::filesystem::current_path(),
                         FeatureOptions features=FeatureOptions{},std::filesystem::path runtimeRoot=std::filesystem::path{})
        : projectRoot_(std::filesystem::absolute(projectRoot)),runtimeRoot_(std::move(runtimeRoot)),features_(features) {}
    const FeatureOptions& features() const { return features_; }
    TextureResult loadTexture(const std::filesystem::path& relative,bool flipRows=true) const {
        try { return {texture(relative,flipRows),{}}; }
        catch(const std::exception& e) { return {nullptr,e.what()}; }
    }
    ModelResult loadPrimitive(const std::filesystem::path& relative,const LoadOptions& options=LoadOptions{}) const {
        try {
            if(!options.texturePath.empty()&&!features_.textures)
                throw std::runtime_error("Enable texture mode before specifying texturePath.");
            std::ifstream input(resolve(relative),std::ios::binary);
            if(!input) throw std::runtime_error("Cannot open primitive data.");
            input.imbue(std::locale::classic());
            std::vector<glm::vec3> positions,normals;
            std::vector<glm::vec2> uvs;
            Model model; model.shape=GLShape::Custom; model.name=relative.u8string();
            std::string line; std::size_t lineNumber=0;
            while(std::getline(input,line)) {
                ++lineNumber;
                if(lineNumber==1 && line.compare(0,3,"\xEF\xBB\xBF")==0) line.erase(0,3);
                line=line.substr(0,line.find('#'));
                std::istringstream row(line); row.imbue(std::locale::classic());
                std::string kind; row>>kind;
                if(kind=="v"||kind=="vn") {
                    glm::vec3 v; if(!(row>>v.x>>v.y>>v.z)||!detail::finiteVector(v))
                        throw std::runtime_error("Invalid vector at line "+std::to_string(lineNumber));
                    (kind=="v"?positions:normals).push_back(v);
                } else if(kind=="vt") {
                    glm::vec2 uv; if(!(row>>uv.x>>uv.y)||!std::isfinite(uv.x)||!std::isfinite(uv.y))
                        throw std::runtime_error("Invalid UV at line "+std::to_string(lineNumber));
                    uvs.push_back(uv);
                } else if(kind=="f") {
                    Mesh mesh; mesh.name="face_"+std::to_string(model.faces.size());
                    std::string corner; bool hasUV=false,hasNormal=false; std::size_t uvCount=0,normalCount=0;
                    while(row>>corner) {
                        const auto first=corner.find('/');
                        const auto second=first==std::string::npos?first:corner.find('/',first+1);
                        mesh.localVertices.push_back(positions.at(detail::objIndex(corner.substr(0,first),positions.size())));
                        glm::vec2 uv(0); glm::vec3 normal(0);
                        if(first!=std::string::npos) {
                            const auto part=corner.substr(first+1,second==std::string::npos?second:second-first-1);
                            if(!part.empty()) {uv=uvs.at(detail::objIndex(part,uvs.size()));hasUV=true;++uvCount;}
                        }
                        if(second!=std::string::npos) {
                            normal=normals.at(detail::objIndex(corner.substr(second+1),normals.size()));hasNormal=true;++normalCount;
                        }
                        mesh.texcoords.push_back(uv);mesh.vertexNormals.push_back(normal);
                    }
                    if((hasUV&&uvCount!=mesh.localVertices.size())||(hasNormal&&normalCount!=mesh.localVertices.size()))
                        throw std::runtime_error("Mixed OBJ corner attributes in one face.");
                    mesh.triangleIndices=detail::triangulate(mesh.localVertices);
                    if(!hasNormal) recalculateNormals(mesh);
                    if(!hasUV || !features_.textures) mesh.texcoords.clear();
                    const auto& indices=mesh.triangleIndices;
                    mesh.normal=glm::normalize(glm::cross(mesh.localVertices[indices[1]]-mesh.localVertices[indices[0]],
                                                          mesh.localVertices[indices[2]]-mesh.localVertices[indices[0]]));
                    model.faces.push_back(std::move(mesh));
                } else if(!kind.empty()&&kind!="g"&&kind!="o"&&kind!="s"&&kind!="mtllib"&&kind!="usemtl")
                    throw std::runtime_error("Unsupported primitive record at line "+std::to_string(lineNumber));
            }
            if(input.bad()) throw std::runtime_error("Primitive file read failed.");
            if(options.mergeCoplanarFaces) detail::mergeCoplanar(model);
            if(!options.texturePath.empty()) {
                auto image=texture(options.texturePath,options.flipTextureRows);
                for(auto& mesh:model.faces) {mesh.material.texture=image;mesh.material.useTexture=true;}
            }
            return detail::finishLoad(std::move(model),options);
        } catch(const std::exception& e) {ModelResult result;result.error=e.what();return result;}
    }
    ModelResult loadModel(const std::filesystem::path& relative,const LoadOptions& options=LoadOptions{}) const {
        ModelResult result;
        if(!features_.models) {result.error="Assimp model mode is disabled.";return result;}
        if(!features_.textures&&!options.texturePath.empty()) {result.error="Texture mode is disabled.";return result;}
        try {
            const auto path=resolve(relative);
            auto& api=detail::assimp(); api.load(runtimeRoot_);
            const aiScene* scene=api.importFile(path.u8string().c_str(),aiProcess_Triangulate
                |aiProcess_JoinIdenticalVertices|aiProcess_GenSmoothNormals|aiProcess_ValidateDataStructure);
            if(!scene) throw std::runtime_error(api.getError());
            struct Release { detail::AssimpApi& api; const aiScene* scene; ~Release(){api.releaseImport(scene);} } release{api,scene};
            if(!scene->mRootNode) throw std::runtime_error("Model has no root node.");
            Model model; model.shape=GLShape::Imported; model.name=relative.u8string();
            std::map<unsigned int,Material> materials;
            std::map<unsigned int,unsigned int> uvChannels;
            for(unsigned int i=0;i<scene->mNumMaterials;++i) {
                Material material; aiColor4D color;
                if(api.getColor(scene->mMaterials[i],AI_MATKEY_BASE_COLOR,&color)==AI_SUCCESS
                    ||api.getColor(scene->mMaterials[i],AI_MATKEY_COLOR_DIFFUSE,&color)==AI_SUCCESS)
                    material.color={color.r,color.g,color.b};
                unsigned int uvChannel=0;
                if(features_.textures) {
                    if(!options.texturePath.empty()) material.texture=texture(options.texturePath,options.flipTextureRows);
                    else {
                        aiString texturePath;
                        aiReturn found=api.getTexture(scene->mMaterials[i],aiTextureType_BASE_COLOR,0,&texturePath,nullptr,&uvChannel,nullptr,nullptr,nullptr,nullptr);
                        if(found!=AI_SUCCESS) found=api.getTexture(scene->mMaterials[i],aiTextureType_DIFFUSE,0,&texturePath,nullptr,&uvChannel,nullptr,nullptr,nullptr,nullptr);
                        if(found==AI_SUCCESS) {
                            const aiTexture* embedded=scene->GetEmbeddedTexture(texturePath.C_Str());
                            if(embedded) {
                                TextureImage image;
                                if(embedded->mHeight==0) {
                                    const auto* bytes=reinterpret_cast<const std::uint8_t*>(embedded->pcData);
                                    std::vector<std::uint8_t> data(bytes,bytes+embedded->mWidth);
                                    image=detail::decodeImage({},&data,options.flipTextureRows);
                                } else {
                                    image.width=embedded->mWidth;image.height=embedded->mHeight;
                                    if(!image.width||!image.height||image.width>16384||image.height>16384)
                                        throw std::runtime_error("Invalid embedded texture size.");
                                    image.rgba.resize(std::size_t(image.width)*image.height*4);
                                    for(unsigned int y=0;y<image.height;++y) for(unsigned int x=0;x<image.width;++x) {
                                        const auto& pixel=embedded->pcData[std::size_t(y)*image.width+x];
                                        const auto dest=(std::size_t(options.flipTextureRows?image.height-1-y:y)*image.width+x)*4;
                                        image.rgba[dest]=pixel.r;image.rgba[dest+1]=pixel.g;image.rgba[dest+2]=pixel.b;image.rgba[dest+3]=pixel.a;
                                    }
                                }
                                material.texture=std::make_shared<const TextureImage>(std::move(image));
                            } else material.texture=texture(relative.parent_path()/std::filesystem::u8path(texturePath.C_Str()),options.flipTextureRows);
                        }
                    }
                    material.useTexture=bool(material.texture);
                }
                materials[i]=material;uvChannels[i]=uvChannel;
            }
            std::function<void(const aiNode*,const glm::mat4&)> visit;
            visit=[&](const aiNode* node,const glm::mat4& parent) {
                const auto matrix=parent*detail::assimpMatrix(node->mTransformation);
                const auto linear=glm::mat3(matrix);const auto determinant=glm::determinant(linear);
                const bool invertible=std::isfinite(determinant)&&std::abs(determinant)>1e-12f;
                const auto normalMatrix=invertible?glm::transpose(glm::inverse(linear)):glm::mat3(1);
                for(unsigned int n=0;n<node->mNumMeshes;++n) {
                    if(node->mMeshes[n]>=scene->mNumMeshes) throw std::runtime_error("Invalid node mesh index.");
                    const auto* source=scene->mMeshes[node->mMeshes[n]];
                    if(!source->HasPositions()) continue;
                    Mesh mesh; mesh.name=node->mName.C_Str();mesh.material=materials[source->mMaterialIndex];
                    const auto uvChannel=uvChannels[source->mMaterialIndex];
                    for(unsigned int v=0;v<source->mNumVertices;++v) {
                        const auto p=source->mVertices[v];mesh.localVertices.push_back(glm::vec3(matrix*glm::vec4(p.x,p.y,p.z,1)));
                        if(source->HasNormals()&&invertible) {
                            const auto normal=source->mNormals[v];const auto transformed=normalMatrix*glm::vec3(normal.x,normal.y,normal.z);
                            mesh.vertexNormals.push_back(glm::length(transformed)>1e-12f?glm::normalize(transformed):glm::vec3(0));
                        }
                        if(features_.textures&&source->HasTextureCoords(uvChannel)) {
                            const auto uv=source->mTextureCoords[uvChannel][v];mesh.texcoords.push_back({uv.x,uv.y});
                        }
                        if(source->HasVertexColors(0)) {const auto c=source->mColors[0][v];mesh.vertexColors.push_back({c.r,c.g,c.b});}
                    }
                    for(unsigned int f=0;f<source->mNumFaces;++f) {
                        const auto& face=source->mFaces[f];if(face.mNumIndices!=3) continue;
                        mesh.triangleIndices.insert(mesh.triangleIndices.end(),{face.mIndices[0],
                            face.mIndices[determinant<0?2:1],face.mIndices[determinant<0?1:2]});
                    }
                    if(mesh.triangleIndices.empty()) continue;
                    if(mesh.vertexNormals.empty()) recalculateNormals(mesh);
                    if(mesh.material.useTexture&&mesh.texcoords.empty()) throw std::runtime_error("Textured mesh is missing the requested UV channel.");
                    model.faces.push_back(std::move(mesh));
                }
                for(unsigned int i=0;i<node->mNumChildren;++i) visit(node->mChildren[i],matrix);
            };
            visit(scene->mRootNode,glm::mat4(1));
            return detail::finishLoad(std::move(model),options);
        } catch(const std::exception& e) {result.error=e.what();return result;}
    }
};
} // namespace graphics
```

## framework/include/Picking.h

표시된 삼각형의 동일 변환으로 클릭 교점을 계산한다.

### before

없음.

### after

```
#pragma once
#include "Graphics.h"

namespace graphics {
struct Ray {
    glm::vec3 origin{0}, direction{0,0,-1};
    float maxDistance = std::numeric_limits<float>::infinity();
};
struct PickResult {
    EntityId entityId = 0;
    std::size_t faceIndex = 0, triangleIndex = 0;
    glm::vec3 worldPoint{0}, localPoint{0}, barycentric{0};
    glm::vec2 uv{0};
    bool hasUV = false;
    float distance = 0; // Distance from ray origin (near plane for a screen ray).
};
struct PickViewport { double x = 0, y = 0, width = 0, height = 0; }; // top-left framebuffer pixels

inline std::optional<Ray> screenRay(const Camera& camera,double x,double y,const PickViewport& viewport) {
    if(!CameraSystem::validCamera(camera)||!std::isfinite(x)||!std::isfinite(y)
        ||!std::isfinite(viewport.x)||!std::isfinite(viewport.y)
        ||!std::isfinite(viewport.width)||!std::isfinite(viewport.height)
        ||viewport.width<=0||viewport.height<=0||x<viewport.x||y<viewport.y
        ||x>=viewport.x+viewport.width||y>=viewport.y+viewport.height)return {};
    const auto pv=CameraSystem::projectionMatrix(camera,float(viewport.width/viewport.height))*CameraSystem::viewMatrix(camera);
    const auto determinant=glm::determinant(pv);
    if(!std::isfinite(determinant)||std::abs(determinant)<1e-12f)return {};
    const auto inverse=glm::inverse(pv);
    const float nx=float(2*(x-viewport.x)/viewport.width-1),ny=float(1-2*(y-viewport.y)/viewport.height);
    auto nearPoint=inverse*glm::vec4(nx,ny,-1,1),farPoint=inverse*glm::vec4(nx,ny,1,1);
    if(std::abs(nearPoint.w)<1e-12f||std::abs(farPoint.w)<1e-12f)return {};
    nearPoint/=nearPoint.w;farPoint/=farPoint.w;
    const auto direction=glm::vec3(farPoint-nearPoint);const auto length=glm::length(direction);
    if(!detail::finiteVector(glm::vec3(nearPoint))||!detail::finiteVector(direction)||!std::isfinite(length)||length<=0)return {};
    return Ray{glm::vec3(nearPoint),direction/length,length};
}
inline std::optional<PickResult> pick(const Scene& scene,const Ray& input) {
    if(!detail::finiteVector(input.origin)||!detail::finiteVector(input.direction)
        ||!std::isfinite(glm::length(input.direction))||glm::length(input.direction)<1e-12f||std::isnan(input.maxDistance)||input.maxDistance<0)return {};
    const auto direction=glm::dvec3(glm::normalize(input.direction));
    const auto origin=glm::dvec3(input.origin);
    double nearest=input.maxDistance;std::optional<PickResult> result;
    for(auto id:scene.getIds()) {
        const auto* component=scene.getComponent(id);if(!component||!component->visible)continue;
        for(std::size_t f=0;f<component->polyhedron.faces.size();++f) {
            const auto& face=component->polyhedron.faces[f];if(!face.visible)continue;
            const auto matrix=DiagramSystem::faceModelMatrix(*component,face);
            const auto indices=detail::triangles(face);
            for(std::size_t t=0;t<indices.size();t+=3) {
                const auto ia=indices[t],ib=indices[t+1],ic=indices[t+2];
                const auto a4=matrix*glm::vec4(face.localVertices[ia],1),b4=matrix*glm::vec4(face.localVertices[ib],1),c4=matrix*glm::vec4(face.localVertices[ic],1);
                if(std::abs(a4.w)<1e-12f||std::abs(b4.w)<1e-12f||std::abs(c4.w)<1e-12f)continue;
                const auto a=glm::dvec3(a4/a4.w),b=glm::dvec3(b4/b4.w),c=glm::dvec3(c4/c4.w);
                const auto edge1=b-a,edge2=c-a,p=glm::cross(direction,edge2);
                const auto determinant=glm::dot(edge1,p);
                const auto tolerance=1e-12*glm::length(edge1)*glm::length(edge2);
                if(!std::isfinite(determinant)||std::abs(determinant)<=tolerance)continue;
                const auto relative=origin-a;const auto u=glm::dot(relative,p)/determinant;
                if(u<0||u>1)continue;
                const auto q=glm::cross(relative,edge1);const auto v=glm::dot(direction,q)/determinant;
                if(v<0||u+v>1)continue;
                const auto distance=glm::dot(edge2,q)/determinant;
                if(!std::isfinite(distance)||distance<0||distance>nearest)continue;
                const glm::dvec3 weights((1-u-v)/a4.w,u/b4.w,v/c4.w);
                const auto sum=weights.x+weights.y+weights.z;if(std::abs(sum)<1e-12)continue;
                PickResult hit;hit.entityId=id;hit.faceIndex=f;hit.triangleIndex=t/3;
                hit.distance=float(distance);hit.worldPoint=glm::vec3(origin+direction*distance);
                hit.barycentric=glm::vec3(weights/sum);
                hit.localPoint=face.localVertices[ia]*hit.barycentric.x+face.localVertices[ib]*hit.barycentric.y+face.localVertices[ic]*hit.barycentric.z;
                hit.hasUV=!face.texcoords.empty();
                if(hit.hasUV)hit.uv=face.texcoords[ia]*hit.barycentric.x+face.texcoords[ib]*hit.barycentric.y+face.texcoords[ic]*hit.barycentric.z;
                nearest=distance;result=hit;
            }
        }
    }
    return result;
}
inline std::optional<PickResult> pick(const Scene& scene,const Camera& camera,double x,double y,
                                     const PickViewport& viewport) {
    const auto ray=screenRay(camera,x,y,viewport);return ray?pick(scene,*ray):std::nullopt;
}
} // namespace graphics
```

## framework/include/Texture.h

외부 이미지 라이브러리 설정 없이 WIC로 RGBA 데이터를 읽는다.

### before

없음.

### after

```
#pragma once
#include "Model.h"
#include "../detail/Native.h"
#include <wincodec.h>
#include <stdexcept>
#include <algorithm>
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "windowscodecs.lib")

namespace graphics {
namespace detail {
template<class T> struct ComOwner {
    T* value = nullptr;
    ~ComOwner() { if(value) value->Release(); }
    ComOwner() = default;
    ComOwner(const ComOwner&) = delete;
    ComOwner& operator=(const ComOwner&) = delete;
};
inline void checkCom(HRESULT result) {
    if(FAILED(result)) throw std::runtime_error("Image decoder failed (HRESULT "+std::to_string(result)+").");
}
struct ComApartment {
    HRESULT result = CoInitializeEx(nullptr,COINIT_MULTITHREADED);
    ComApartment() { if(result!=RPC_E_CHANGED_MODE) checkCom(result); }
    ~ComApartment() { if(SUCCEEDED(result)) CoUninitialize(); }
};
inline TextureImage decodeImage(const std::filesystem::path& file,
                               const std::vector<std::uint8_t>* encoded,
                               bool flipRows) {
    ComApartment apartment;
    ComOwner<IWICImagingFactory> factory;
    checkCom(CoCreateInstance(CLSID_WICImagingFactory,nullptr,CLSCTX_INPROC_SERVER,
                             IID_PPV_ARGS(&factory.value)));
    ComOwner<IWICStream> stream;
    ComOwner<IWICBitmapDecoder> decoder;
    if(encoded) {
        if(encoded->empty() || encoded->size()>std::numeric_limits<DWORD>::max())
            throw std::runtime_error("Invalid encoded texture size.");
        checkCom(factory.value->CreateStream(&stream.value));
        checkCom(stream.value->InitializeFromMemory(const_cast<BYTE*>(encoded->data()),DWORD(encoded->size())));
        checkCom(factory.value->CreateDecoderFromStream(stream.value,nullptr,WICDecodeMetadataCacheOnLoad,&decoder.value));
    } else checkCom(factory.value->CreateDecoderFromFilename(file.c_str(),nullptr,GENERIC_READ,
                                                            WICDecodeMetadataCacheOnLoad,&decoder.value));
    ComOwner<IWICBitmapFrameDecode> frame;
    ComOwner<IWICFormatConverter> converter;
    checkCom(decoder.value->GetFrame(0,&frame.value));
    checkCom(factory.value->CreateFormatConverter(&converter.value));
    checkCom(converter.value->Initialize(frame.value,GUID_WICPixelFormat32bppRGBA,WICBitmapDitherTypeNone,
                                        nullptr,0,WICBitmapPaletteTypeCustom));
    TextureImage image;
    checkCom(converter.value->GetSize(&image.width,&image.height));
    if(!image.width || !image.height || image.width>16384 || image.height>16384)
        throw std::runtime_error("Texture dimensions must be within 1..16384.");
    image.rgba.resize(std::size_t(image.width)*image.height*4);
    checkCom(converter.value->CopyPixels(nullptr,image.width*4,UINT(image.rgba.size()),image.rgba.data()));
    if(flipRows) {
        const std::size_t stride=std::size_t(image.width)*4;
        for(std::uint32_t y=0;y<image.height/2;++y)
            std::swap_ranges(image.rgba.begin()+y*stride,image.rgba.begin()+(y+1)*stride,
                             image.rgba.begin()+(image.height-1-y)*stride);
    }
    return image;
}
} // namespace detail
} // namespace graphics
```

## framework/shaders/FragmentShader.glsl

선택적 이미지·조명과 외곽선 색상을 처리한다.

### before

```
#version 330 core

in vec3 outColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(outColor, 1.0);
}
```

### after

```
#version 330 core
in vec3 outColor;
in vec2 TexCoord;
in vec3 Normal;
uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform bool uLighting;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform bool uOverrideColor;
uniform vec3 uOutlineColor;
out vec4 FragColor;
void main() {
    if (uOverrideColor) { FragColor = vec4(uOutlineColor, 1.0); return; }
    vec4 color = vec4(outColor, 1.0);
    if (uHasTexture) color *= texture(uTexture, TexCoord);
    if (uLighting && length(Normal) > 0.0)
        color.rgb *= (0.25 + 0.75 * max(dot(normalize(Normal), normalize(uLightDirection)), 0.0)) * uLightColor;
    FragColor = color;
}
```

## framework/shaders/VertexShader.glsl

일반 정점/색상/UV/법선 인터페이스를 사용한다.

### before

```
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;

uniform mat4 uMVP;
out vec3 outColor;

void main()
{
    gl_Position = uMVP * vec4(vPos, 1.0);
    outColor = vColor;
}
```

### after

```
#version 330 core
layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
layout(location=2) in vec2 vTexCoord;
layout(location=3) in vec3 vNormal;
uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;
out vec3 outColor;
out vec2 TexCoord;
out vec3 Normal;
void main() {
    gl_Position = uMVP * vec4(vPos, 1.0);
    outColor = vColor;
    TexCoord = vTexCoord;
    Normal = uNormalMatrix * vNormal;
}
```

## framework/src/Application.cpp

기존 전체 구현을 헤더로 이동했다. 호환 include 래퍼만 유지한다.

### before

```
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "../include/Application.h"
#include <glfw3.h>

#include <algorithm>
#include <iostream>
#include <utility>
#ifdef _WIN32
#include <windows.h>
#endif

namespace graphics {

Application::~Application() {
    shutdown();
}

void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

void Application::setModifiedKeyHandler(
    std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

std::filesystem::path Application::findShaderDirectory() {
    const auto containsShaders = [](const std::filesystem::path& path) {
        return std::filesystem::is_regular_file(path / "VertexShader.glsl")
            && std::filesystem::is_regular_file(path / "FragmentShader.glsl");
    };
#ifdef _WIN32
    // 실행 파일 옆의 shaders를 우선한다. 작업 폴더와 프로젝트 이름에 의존하지 않는다.
    std::wstring executable(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(),
                                           static_cast<DWORD>(executable.size()));
    if (length > 0 && length < executable.size()) {
        executable.resize(length);
        const auto candidate = std::filesystem::path(executable).parent_path() / "shaders";
        if (containsShaders(candidate)) {
            return candidate;
        }
    }
#endif
    auto directory = std::filesystem::current_path();
    for (;;) {
        const auto candidate = directory / "framework" / "shaders";
        if (containsShaders(candidate)) {
            return candidate;
        }
        if (directory == directory.parent_path()) {
            return {};
        }
        directory = directory.parent_path();
    }
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::initialize(const ApplicationConfig& config) {
    if (config.width <= 0 || config.height <= 0 || !glfwInit()) {
        return false;
    }
    glfwInitialized_ = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    window_ = glfwCreateWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(config.verticalSync ? 1 : 0);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return false;
    }
    if (!renderer_.initialize(config.shaderDirectory / "VertexShader.glsl",
                              config.shaderDirectory / "FragmentShader.glsl")) {
        return false;
    }

    glfwSetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app.modifiedKeyHandler_) {
            app.modifiedKeyHandler_(app, key, action, mods);
        } else if (app.keyHandler_) {
            app.keyHandler_(app, key, action);
        }
    });
    glfwSetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app.focusHandler_) {
            app.focusHandler_(app, focused == GLFW_TRUE);
        }
    });
    return true;
}

void Application::shutdown() {
    if (window_ != nullptr) {
        glfwMakeContextCurrent(window_);
        renderer_.destroy();
        glfwSetWindowUserPointer(window_, nullptr);
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    if (glfwInitialized_) {
        glfwTerminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window_);
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

int Application::run(const ApplicationConfig& config) {
    if (running_) {
        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }
    running_ = true;
    int exitCode = 0;
    try {
        if (!initialize(config)) {
            std::cerr << "Application initialization failed.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            std::cerr << "User scene initialization failed.\n";
            exitCode = -1;
        } else {
            double previousTime = glfwGetTime();
            while (!glfwWindowShouldClose(window_)) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window_)) {
                    break;
                }
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(window_, &width, &height);
                if (width <= 0 || height <= 0) {
                    glfwWaitEvents();
                    previousTime = glfwGetTime();
                    continue;
                }
                const double currentTime = glfwGetTime();
                const float deltaTime = static_cast<float>(
                    std::clamp(currentTime - previousTime, 0.0, 0.05));
                previousTime = currentTime;
                if (updateHandler_) {
                    updateHandler_(*this, deltaTime);
                }
                if (glfwWindowShouldClose(window_)) {
                    break;
                }
                glfwMakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
                if (!rendered) {
                    std::cerr << "Rendering failed.\n";
                    exitCode = -1;
                    break;
                }
                glfwSwapBuffers(window_);
            }
        }
    } catch (...) {
        shutdown();
        running_ = false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    return exitCode;
}

} // namespace graphics
```

### after

```
// Compatibility compilation unit. All implementation is inline.
#include "../include/Application.h"
```

## framework/src/Graphics.cpp

기존 전체 구현을 헤더로 이동했다. 호환 include 래퍼만 유지한다.

### before

```
#include "../include/Graphics.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace graphics {
namespace {

bool finiteVector(const glm::vec3& value) {
    return std::isfinite(value.x) && std::isfinite(value.y)
        && std::isfinite(value.z);
}

bool finiteMatrix(const glm::mat4& matrix) {
    for (int column = 0; column < 4; ++column) {
        for (int row = 0; row < 4; ++row) {
            if (!std::isfinite(matrix[column][row])) {
                return false;
            }
        }
    }
    return true;
}

bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f
        && value.y > 0.0f && value.z > 0.0f;
}

bool validColor(GlColor color) {
    return std::isfinite(color.red) && std::isfinite(color.green)
        && std::isfinite(color.blue);
}

std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;
    return ++revision;
}

void appendFace(Polyhedron& polyhedron, GLFace shape,
                std::initializer_list<glm::vec3> vertices, GlColor color) {
    PlaneFigure face;
    face.shape = shape;
    face.localVertices = vertices;
    face.color = color;
    face.normal = glm::normalize(glm::cross(
        face.localVertices[1] - face.localVertices[0],
        face.localVertices[2] - face.localVertices[0]));
    polyhedron.faces.push_back(face);
}

} // namespace

Scene::Scene() {
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
    pyramidFaceId_.reserve(MAX_ENTITIES * 5);
    lines_.reserve(MAX_LINES);
}

std::optional<EntityId> Scene::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (components_.size() >= MAX_ENTITIES
        || !DiagramSystem::validPolyhedron(polyhedron)
        || !DiagramSystem::validTransform(transform)) {
        return std::nullopt;
    }
    glm::vec3 minimum = polyhedron.faces[0].localVertices[0];
    glm::vec3 maximum = minimum;
    for (const PlaneFigure& face : polyhedron.faces) {
        for (const glm::vec3& vertex : face.localVertices) {
            minimum = glm::min(minimum, vertex);
            maximum = glm::max(maximum, vertex);
        }
    }
    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform, maximum - minimum });
    polyId_.push_back(id);
    std::vector<FaceId>& faceIds = polyhedron.shape == GLShape::Cube
        ? cubeFaceId_ : (polyhedron.shape == GLShape::Pyramid
            ? pyramidFaceId_ : tetraFaceId_);
    for (std::size_t i = 0; i < polyhedron.faces.size(); ++i) {
        faceIds.push_back({ id, i });
    }
    geometryRevision_ = nextGeometryRevision();
    return id;
}

std::optional<EntityId> Scene::addCuboid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths),
                       transform);
}

std::optional<EntityId> Scene::addRightTetrahedron(
    const glm::vec3& edgeLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths),
                       transform);
}

std::optional<EntityId> Scene::addPyramid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths),
                       transform);
}

bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;
    return true;
}

bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex,
                               const glm::mat4& matrix) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !finiteMatrix(matrix)) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    return true;
}

bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex,
                                const std::vector<GlColor>& colors) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    if (!colors.empty() && colors.size() != face.localVertices.size()) {
        return false;
    }
    for (GlColor color : colors) {
        if (!validColor(color)) {
            return false;
        }
    }
    face.vertexColors = colors;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return false;
    }
    components_[id].transform.basis = worldPosition;
    return true;
}

bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if (id >= components_.size() || !positiveLengths(sideLengths)) {
        return false;
    }
    Component_Poly& component = components_[id];
    Polyhedron replacement = DiagramSystem::makePolyhedron(
        component.polyhedron.shape, sideLengths);
    if (!DiagramSystem::validPolyhedron(replacement)) {
        return false;
    }
    for (std::size_t i = 0; i < replacement.faces.size(); ++i) {
        replacement.faces[i].color = component.polyhedron.faces[i].color;
        replacement.faces[i].independence = component.polyhedron.faces[i].independence;
        replacement.faces[i].transform = component.polyhedron.faces[i].transform;
        replacement.faces[i].visible = component.polyhedron.faces[i].visible;
        replacement.faces[i].localAnimationMatrix =
            component.polyhedron.faces[i].localAnimationMatrix;
        replacement.faces[i].vertexColors = component.polyhedron.faces[i].vertexColors;
    }
    component.polyhedron = std::move(replacement);
    component.sideLengths = sideLengths;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true;
}

bool Scene::setFaceTransform(
    EntityId id, std::size_t faceIndex,
    const Transform3D& transform, bool independence
) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    face.transform = transform;
    face.independence = independence;
    return true;
}

std::optional<LineId> Scene::addLine(
    const glm::vec3& start, const glm::vec3& end,
    GlColor color, std::optional<EntityId> owner
) {
    if (lines_.size() >= MAX_LINES || !finiteVector(start) || !finiteVector(end)
        || !validColor(color) || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    const LineId id = lines_.size();
    lines_.push_back({ start, end, color, owner });
    geometryRevision_ = nextGeometryRevision();
    return id;
}

std::optional<LineId> Scene::addVector(
    const glm::vec3& origin, const glm::vec3& vector,
    GlColor color, std::optional<EntityId> owner
) {
    return addLine(origin, origin + vector, color, owner);
}

std::optional<std::array<LineId, 3>> Scene::addBasis(
    const glm::vec3& origin, const glm::mat3& basisVectors,
    const glm::vec3& axisLengths, std::optional<EntityId> owner
) {
    if (lines_.size() > MAX_LINES - 3 || !finiteVector(origin)
        || !positiveLengths(axisLengths)
        || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    for (int i = 0; i < 3; ++i) {
        if (!finiteVector(basisVectors[i])
            || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f
            || !finiteVector(origin + basisVectors[i] * axisLengths[i])) {
            return std::nullopt;
        }
    }
    // 사전 검증 후 세 선을 함께 등록한다. 실패 입력은 장면을 변경하지 않는다.
    const std::array<GlColor, 3> colors{
        GlColor{ 1.0f, 0.2f, 0.2f },
        GlColor{ 0.2f, 1.0f, 0.2f },
        GlColor{ 0.2f, 0.4f, 1.0f }
    };
    std::array<LineId, 3> ids{};
    for (int i = 0; i < 3; ++i) {
        ids[i] = lines_.size();
        lines_.push_back({
            origin, origin + basisVectors[i] * axisLengths[i], colors[i], owner });
    }
    geometryRevision_ = nextGeometryRevision();
    return ids;
}

bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin)
        || !finiteVector(origin + vector)) {
        return false;
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    cubeFaceId_.clear();
    tetraFaceId_.clear();
    pyramidFaceId_.clear();
    lines_.clear();
    geometryRevision_ = nextGeometryRevision();
}

bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale)
        && std::isfinite(transform.orientation.w)
        && std::isfinite(transform.orientation.x)
        && std::isfinite(transform.orientation.y)
        && std::isfinite(transform.orientation.z)
        && std::isfinite(glm::dot(transform.orientation, transform.orientation))
        && glm::dot(transform.orientation, transform.orientation) > 0.000001f;
}

bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    std::size_t expectedFaces = 0;
    switch (polyhedron.shape) {
    case GLShape::Cube: expectedFaces = 6; break;
    case GLShape::Tetra: expectedFaces = 4; break;
    case GLShape::Pyramid: expectedFaces = 5; break;
    default: return false;
    }
    if (polyhedron.faces.size() != expectedFaces) {
        return false;
    }
    for (std::size_t i = 0; i < expectedFaces; ++i) {
        const PlaneFigure& face = polyhedron.faces[i];
        const bool rectangle = polyhedron.shape == GLShape::Cube
            || (polyhedron.shape == GLShape::Pyramid && i == 4);
        if (face.shape != (rectangle ? GLFace::Rect : GLFace::Triangle)
            || face.localVertices.size() != (rectangle ? 4u : 3u)
            || !finiteVector(face.normal) || !validColor(face.color)
            || !validTransform(face.transform) || !finiteMatrix(face.localAnimationMatrix)
            || (!face.vertexColors.empty()
                && face.vertexColors.size() != face.localVertices.size())) {
            return false;
        }
        for (const glm::vec3& vertex : face.localVertices) {
            if (!finiteVector(vertex)) {
                return false;
            }
        }
        for (GlColor color : face.vertexColors) {
            if (!validColor(color)) {
                return false;
            }
        }
    }
    return true;
}

Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
    Polyhedron polyhedron;
    if (!positiveLengths(sideLengths)) {
        return polyhedron;
    }
    polyhedron.shape = shape;
    if (shape == GLShape::Cube) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        polyhedron.faces.reserve(6);
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, z }, { x, -y, z }, { x, y, z }, { -x, y, z } },
            { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, -z }, { -x, -y, -z }, { -x, y, -z }, { x, y, -z } },
            { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { -x, -y, z }, { -x, y, z }, { -x, y, -z } },
            { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, z }, { x, -y, -z }, { x, y, -z }, { x, y, z } },
            { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, y, z }, { x, y, z }, { x, y, -z }, { -x, y, -z } },
            { .3f, .8f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { x, -y, -z }, { x, -y, z }, { -x, -y, z } },
            { .8f, .3f, .9f });
    } else if (shape == GLShape::Pyramid) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        const glm::vec3 frontLeft{ -x, -y, z };
        const glm::vec3 frontRight{ x, -y, z };
        const glm::vec3 backRight{ x, -y, -z };
        const glm::vec3 backLeft{ -x, -y, -z };
        const glm::vec3 apex{ 0.0f, y, 0.0f };
        polyhedron.faces.reserve(5);
        // 앞, 오른쪽, 뒤, 왼쪽, 바닥. 삼각형과 사각형은 바깥쪽 winding이다.
        appendFace(polyhedron, GLFace::Triangle,
            { frontLeft, frontRight, apex }, { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { frontRight, backRight, apex }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { backRight, backLeft, apex }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { backLeft, frontLeft, apex }, { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { backLeft, backRight, frontRight, frontLeft }, { .8f, .3f, .9f });
    } else if (shape == GLShape::Tetra) {
        // 원점에서 직교하는 세 모서리를 만들고, 무게중심을 로컬 원점으로 옮긴다.
        const glm::vec3 center = sideLengths * 0.25f;
        const glm::vec3 origin = -center;
        const glm::vec3 x = glm::vec3(sideLengths.x, 0.0f, 0.0f) - center;
        const glm::vec3 y = glm::vec3(0.0f, sideLengths.y, 0.0f) - center;
        const glm::vec3 z = glm::vec3(0.0f, 0.0f, sideLengths.z) - center;
        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle,
            { origin, y, x }, { .9f, .4f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, x, z }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, z, y }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { x, y, z }, { .9f, .8f, .3f });
    }
    return polyhedron;
}

glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    // 열벡터 기준: localBasis 이동 -> S -> Rz -> Ry -> Rx -> 월드 위치.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.basis);
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model *= glm::mat4_cast(glm::normalize(transform.orientation));
    model = glm::scale(model, transform.scale);
    return glm::translate(model, -transform.localBasis);
}

glm::mat4 DiagramSystem::faceModelMatrix(
    const Component_Poly& component, const PlaneFigure& face
) {
    // 기존 독립 면은 월드 transform만 사용한다. 일반 면은 객체를 따라간다.
    return face.independence ? modelMatrix(face.transform)
        : modelMatrix(component.transform) * face.localAnimationMatrix;
}

bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);
    return finiteVector(camera.position) && finiteVector(camera.target)
        && finiteVector(camera.up) && finiteVector(direction) && finiteVector(right)
        && glm::dot(direction, direction) > 0.000001f
        && glm::dot(right, right) > 0.000001f
        && std::isfinite(camera.fieldOfViewDegrees)
        && camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f
        && std::isfinite(camera.nearPlane) && std::isfinite(camera.farPlane)
        && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane
        && std::isfinite(camera.orthographicHalfExtent)
        && camera.orthographicHalfExtent > 0.0f;
}

glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                          camera.nearPlane, camera.farPlane);
    }
    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect,
                            camera.nearPlane, camera.farPlane);
}

namespace {

GLuint compileShader(GLenum type, const std::filesystem::path& path) {
    const std::string sourceText = readTextFile(path);
    if (sourceText.empty()) {
        return 0;
    }
    const char* source = sourceText.c_str();
    const GLuint shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), nullptr, log.data());
        std::cerr << "Shader compile failed: " << path << '\n' << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

} // namespace

std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open shader file: " << path << '\n';
        return {};
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();
    if (text.size() >= 3 && static_cast<unsigned char>(text[0]) == 0xEF
        && static_cast<unsigned char>(text[1]) == 0xBB
        && static_cast<unsigned char>(text[2]) == 0xBF) {
        text.erase(0, 3); // 저장 파일은 BOM을 유지하고 GLSL 입력에서만 제거한다.
    }
    return text;
}

bool Renderer::initShader(const std::filesystem::path& vertexPath,
                          const std::filesystem::path& fragmentPath) {
    destroy();
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
    if (vertexShader == 0) {
        return false;
    }
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    shaderProgramID_ = glCreateProgram();
    if (shaderProgramID_ == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    glAttachShader(shaderProgramID_, vertexShader);
    glAttachShader(shaderProgramID_, fragmentShader);
    glLinkProgram(shaderProgramID_);

    GLint linked = GL_FALSE;
    glGetProgramiv(shaderProgramID_, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(shaderProgramID_, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetProgramInfoLog(shaderProgramID_, static_cast<GLsizei>(log.size()),
                            nullptr, log.data());
        std::cerr << "Shader link failed:\n" << log << '\n';
    }
    glDetachShader(shaderProgramID_, vertexShader);
    glDetachShader(shaderProgramID_, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (linked != GL_TRUE) {
        destroy();
        return false;
    }

    mvpLocation_ = glGetUniformLocation(shaderProgramID_, "uMVP");
    if (mvpLocation_ < 0) {
        std::cerr << "Shader uniform uMVP is missing.\n";
        destroy();
        return false;
    }
    return true;
}

bool Renderer::initBuffer() {
    if (shaderProgramID_ == 0 || vao_ != 0) {
        return false;
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(2, vbo_);
    glGenBuffers(1, &ebo_);
    if (vao_ == 0 || vbo_[0] == 0 || vbo_[1] == 0 || ebo_ == 0) {
        destroy();
        return false;
    }

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBindVertexArray(0); // EBO 연결은 VAO의 상태로 유지한다.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool Renderer::upload(const Scene& scene) {
    if (vao_ == 0) {
        return false;
    }
    std::vector<GLfloat> positions;
    std::vector<GLfloat> colors;
    std::vector<GLuint> indices;
    std::vector<DrawRange> ranges;
    for (EntityId id : scene.getIds()) {
        const Component_Poly* component = scene.getComponent(id);
        if (component == nullptr
            || !DiagramSystem::validPolyhedron(component->polyhedron)) {
            return false;
        }
        const auto& faces = component->polyhedron.faces;
        for (std::size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex) {
            const PlaneFigure& face = faces[faceIndex];
            const GLuint firstVertex = static_cast<GLuint>(positions.size() / 3);
            const std::size_t firstIndex = indices.size();
            for (std::size_t i = 0; i < face.localVertices.size(); ++i) {
                const glm::vec3& vertex = face.localVertices[i];
                const GlColor color = face.vertexColors.empty() ? face.color : face.vertexColors[i];
                positions.insert(positions.end(), { vertex.x, vertex.y, vertex.z });
                colors.insert(colors.end(), { color.red, color.green, color.blue });
            }
            indices.insert(indices.end(),
                { firstVertex, firstVertex + 1, firstVertex + 2 });
            if (face.shape == GLFace::Rect) {
                indices.insert(indices.end(),
                    { firstVertex, firstVertex + 2, firstVertex + 3 });
            }
            const GLsizei triangleCount = static_cast<GLsizei>(indices.size() - firstIndex);
            const std::size_t outlineFirstIndex = indices.size();
            for (std::size_t i = 0; i < face.localVertices.size(); ++i) {
                indices.push_back(firstVertex + static_cast<GLuint>(i));
            }
            ranges.push_back({ GL_TRIANGLES, id, faceIndex, std::nullopt,
                firstIndex, triangleCount, outlineFirstIndex,
                static_cast<GLsizei>(face.localVertices.size()) });
        }
    }
    const auto& lines = scene.getLines();
    for (LineId id = 0; id < lines.size(); ++id) {
        const Line3D& line = lines[id];
        const GLuint firstVertex = static_cast<GLuint>(positions.size() / 3);
        const std::size_t firstIndex = indices.size();
        positions.insert(positions.end(),
            { line.start.x, line.start.y, line.start.z,
              line.end.x, line.end.y, line.end.z });
        for (int i = 0; i < 2; ++i) {
            colors.insert(colors.end(),
                { line.color.red, line.color.green, line.color.blue });
        }
        indices.insert(indices.end(), { firstVertex, firstVertex + 1 });
        ranges.push_back({ GL_LINES, 0, 0, id, firstIndex, 2 });
    }
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(positions.size() * sizeof(GLfloat)),
        positions.empty() ? nullptr : positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(colors.size() * sizeof(GLfloat)),
        colors.empty() ? nullptr : colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)),
        indices.empty() ? nullptr : indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    drawRanges_ = std::move(ranges);
    return true;
}

bool Renderer::draw(const Scene& scene, const glm::mat4& view,
                    const glm::mat4& projection, const RenderOptions& options) const {
    if (shaderProgramID_ == 0 || vao_ == 0 || mvpLocation_ < 0) {
        return false;
    }
    // 사용되는 ID를 먼저 검사한다. 잘못된 데이터로 일부만 그리지 않는다.
    for (const DrawRange& range : drawRanges_) {
        if (range.lineId) {
            if (*range.lineId >= scene.getLines().size()) {
                return false;
            }
            const auto owner = scene.getLines()[*range.lineId].owner;
            if (owner && scene.getComponent(*owner) == nullptr) {
                return false;
            }
        } else {
            const Component_Poly* component = scene.getComponent(range.entityId);
            if (component == nullptr
                || range.faceIndex >= component->polyhedron.faces.size()) {
                return false;
            }
        }
    }
    glUseProgram(shaderProgramID_);
    glBindVertexArray(vao_);
    for (const DrawRange& range : drawRanges_) {
        glm::mat4 model(1.0f);
        if (range.lineId) {
            const auto owner = scene.getLines()[*range.lineId].owner;
            if (owner) {
                model = DiagramSystem::modelMatrix(scene.getComponent(*owner)->transform);
            }
        } else {
            const Component_Poly* component = scene.getComponent(range.entityId);
            const PlaneFigure& face = component->polyhedron.faces[range.faceIndex];
            if (!face.visible) {
                continue;
            }
            model = DiagramSystem::faceModelMatrix(*component, face);
        }
        const glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(mvp));
        const bool outline = options.wireframe && !range.lineId.has_value();
        const std::size_t firstIndex = outline ? range.outlineFirstIndex : range.firstIndex;
        const void* offset = reinterpret_cast<const void*>(firstIndex * sizeof(GLuint));
        glDrawElements(outline ? GL_LINE_LOOP : range.mode,
                       outline ? range.outlineCount : range.count, GL_UNSIGNED_INT, offset);
    }
    glBindVertexArray(0);
    glUseProgram(0);
    return true;
}

bool Renderer::initialize(const std::filesystem::path& vertexPath,
                          const std::filesystem::path& fragmentPath) {
    if (!initShader(vertexPath, fragmentPath) || !initBuffer()) {
        destroy();
        return false;
    }
    return true;
}

bool Renderer::render(const Scene& scene, const Camera& camera, int width, int height,
                      const RenderOptions& options) {
    if (shaderProgramID_ == 0 || vao_ == 0) {
        return false;
    }
    if (width <= 0 || height <= 0) {
        return true; // 최소화 중에는 투영과 드로우를 건너뛴다.
    }
    if (!CameraSystem::validCamera(camera)) {
        return false;
    }
    if (uploadedScene_ != &scene || uploadedRevision_ != scene.getGeometryRevision()) {
        if (!upload(scene)) {
            return false;
        }
        uploadedScene_ = &scene;
        uploadedRevision_ = scene.getGeometryRevision();
    }
    glViewport(0, 0, width, height);
    if (options.depthTest) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glClearColor(0.12f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    return draw(scene, CameraSystem::viewMatrix(camera),
                CameraSystem::projectionMatrix(camera, aspect), options);
}

void Renderer::destroy() {
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    if (vbo_[0] != 0 || vbo_[1] != 0) {
        glDeleteBuffers(2, vbo_);
        vbo_[0] = vbo_[1] = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (shaderProgramID_ != 0) {
        glDeleteProgram(shaderProgramID_);
        shaderProgramID_ = 0;
    }
    mvpLocation_ = -1;
    drawRanges_.clear();
    uploadedScene_ = nullptr;
    uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
}

} // namespace graphics
```

### after

```
// Compatibility compilation unit. All framework implementation is inline.
#include "../include/Graphics.h"
```
