# Framework 의존성 및 상대경로 설정: 전체 before / after

기준 커밋: 97e6f3c9b72672e5fd490e874a57941b4e3bc484. 이번 작업에서 렌더링 소스와 셰이더는 수정하지 않았다.

1단계 b4e8d72: 의존성 포함 및 Git 제외 규칙. 2단계 36cbe2d: 속성 시트 및 assign13 연결. 3단계: 사용법과 이 대조 문서.

## .gitignore

before:
~~~~text
# Prerequisites
*.d

# Compiled Object files
*.slo
*.lo
*.o
*.obj

# Precompiled Headers
*.gch
*.pch

# Compiled Dynamic libraries
*.so
*.dylib
*.dll

# Fortran module files
*.mod
*.smod

# Compiled Static libraries
*.lai
*.la
*.a
*.lib

# Executables
*.exe
*.out
*.app

# VS File for Copliot, vs set, and else..
*.vs

**/x64/
*.aps
*.vcxproj.user
.vs/

# Local OpenGL dependency locations and downloaded packages
OpenGL.local.props
/opengl_libs/
/opengl_libs.zip

# Vendored OpenGL x64 runtime and import libraries (source dependencies)
!/dependencies/glew-2.3.1/lib/Release/x64/
!/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/dependencies/glew-2.3.1/bin/Release/x64/
!/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll
~~~~

after:
~~~~text
# Prerequisites
*.d

# Compiled Object files
*.slo
*.lo
*.o
*.obj

# Precompiled Headers
*.gch
*.pch

# Compiled Dynamic libraries
*.so
*.dylib
*.dll

# Fortran module files
*.mod
*.smod

# Compiled Static libraries
*.lai
*.la
*.a
*.lib

# Executables
*.exe
*.out
*.app

# VS File for Copliot, vs set, and else..
*.vs

**/x64/
*.aps
*.vcxproj.user
.vs/

# Local OpenGL dependency locations and downloaded packages
OpenGL.local.props
/opengl_libs/
/opengl_libs.zip

# Vendored OpenGL x64 runtime and import libraries (source dependencies)
!/dependencies/glew-2.3.1/lib/Release/x64/
!/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/dependencies/glew-2.3.1/bin/Release/x64/
!/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll

# Framework-local x64 dependency runtime and import libraries
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/lib/Release/x64/
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/bin/Release/x64/
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/openGL_3dAssign_13/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/openGL_3dAssign_13/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll

~~~~

## openGL_3dAssign_13/openGL_sample2.vcxproj

before:
~~~~text
﻿<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>18.0</VCProjectVersion>
    <Keyword>Win32Proj</Keyword>
    <ProjectGuid>{3f7bde5d-cd40-4419-9023-66456a3a28eb}</ProjectGuid>
    <RootNamespace>openGL_assign_789</RootNamespace>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
    <ProjectName>openGL_assign_13</ProjectName>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpplatest</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="framework\src\Graphics.cpp" />
    <ClCompile Include="framework\src\Application.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="main.cpp" />
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="framework\include\Graphics.h" />
    <ClInclude Include="framework\include\Application.h" />
    <ClInclude Include="Common.h" />
    <ClInclude Include="Graphics.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="resource.h" />
  </ItemGroup>
  <ItemGroup>
    <None Include="framework\shaders\FragmentShader.glsl" />
    <None Include="packages.config" />
    <None Include="framework\shaders\VertexShader.glsl" />
  </ItemGroup>
  <ItemGroup>
    <ResourceCompile Include="openGL_assign_789.rc" />
  </ItemGroup>
  <Import Project="..\OpenGL.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets" />
</Project>
~~~~

after:
~~~~text
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>18.0</VCProjectVersion>
    <Keyword>Win32Proj</Keyword>
    <ProjectGuid>{3f7bde5d-cd40-4419-9023-66456a3a28eb}</ProjectGuid>
    <RootNamespace>openGL_assign_789</RootNamespace>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
    <ProjectName>openGL_assign_13</ProjectName>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpplatest</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="framework\src\Graphics.cpp" />
    <ClCompile Include="framework\src\Application.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="main.cpp" />
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="framework\include\Graphics.h" />
    <ClInclude Include="framework\include\Application.h" />
    <ClInclude Include="Common.h" />
    <ClInclude Include="Graphics.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="resource.h" />
  </ItemGroup>
  <ItemGroup>
    <None Include="framework\shaders\FragmentShader.glsl" />
    <None Include="packages.config" />
    <None Include="framework\Framework.props" />
    <None Include="framework\shaders\VertexShader.glsl" />
  </ItemGroup>
  <ItemGroup>
    <ResourceCompile Include="openGL_assign_789.rc" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets" />
</Project>
~~~~

## openGL_3dAssign_13/framework/Framework.props

before: 없음. after: framework 자체 위치에 의존성을 연결하는 새 속성 시트.

after:
~~~~text
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
</Project>

~~~~

## openGL_3dAssign_13/framework/README.md

before:
~~~~text
﻿# Graphics framework 사용법

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
~~~~

after:
~~~~text
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
   셰이더는 자동 복사하지 않으므로 framework/shaders 폴더를 함께 가져간다.

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

~~~~

## 변경 이유

- .gitignore: framework 안의 필수 x64 LIB/DLL도 커밋할 수 있도록 제외 규칙을 좁게 해제한다.
- vcxproj: 루트 OpenGL.props 대신 framework/Framework.props를 가져온다. 솔루션 내부에서 자체 의존성을 찾는다.
- Framework.props: include/LIB 경로, 동적 링크 정의, 런타임, 디버깅 작업 폴더와 DLL 복사를 함께 제공한다. 구성별 기존 매크로와 검색 경로 상속을 보존한다.
- README: framework 폴더를 복사하고 속성 시트와 소스를 등록하는 사용법 및 사용자 확인 절차를 제공한다.

## 추가한 제3자 의존성 전체

각 파일의 before는 없음이다. DEPENDENCIES.patch의 각 new file 블록이 after 전체이다. 텍스트 442개는 생략 없이 전체 줄을 포함하고 LIB/DLL 4개는 Git binary patch로 기록했다. 라이선스 3개도 포함한다. 패치는 UTF-8 BOM 문서이므로 git apply로 사용할 때는 맨 앞 BOM을 제거한다.

제3자 텍스트는 UTF-8 BOM을 적용했다. 인코딩을 제외한 내용이 원본과 같은지 442개 모두 대조했다. 바이너리 4개는 SHA-256이 원본과 같다. 배포본 자체에 남아 있는 공백 경고는 원본 보존을 위해 유지했다.

## 정적 확인과 사용자가 수행할 테스트

프로젝트/props XML, 필수 파일 7개, 모든 include/LIB 디렉터리, 인코딩과 원본 내용 일치 및 직접 작성한 변경의 공백을 확인했다. 빌드·실행 테스트는 수행하지 않았다.

사용자는 Debug|x64와 Release|x64를 빌드하고 실행 파일 옆 DLL 2개, F5 실행, 도형·선·카메라·입력을 확인한다. 다른 프로젝트에 framework만 복사한 뒤 사용자 main과 2개 .cpp 및 Framework.props를 등록해 재사용을 확인한다.

전체 저장소 graphify 생성/갱신은 승인 범위에 없으므로 수행하지 않았다. 렌더링 구조 변경은 없으며 여기의 설명은 정적 소스·설정 확인이다.
