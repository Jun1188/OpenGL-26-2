# OpenGL 프로젝트 설정

OpenGL 프로젝트 6개(`openGL_sample0`, `openGL_assign123456`,
`openGL_2dAssign_789`, `openGL_2dAssign_10`, `openGL_2dAssign11`,
`openGL_2dAssign12`)는 저장소 루트의 `OpenGL.props`를 사용한다.
콘솔 전용 warm-up 프로젝트와 WarmupTests에는 OpenGL 설정을 추가하지 않았다.

## 저장소에 포함된 라이브러리

사용자가 준비한 `opengl_libs` 배포본에서 필요한 헤더, x64 import library,
DLL 및 라이선스를 아래 `dependencies` 폴더로 복사했다.
6개 프로젝트 모두 저장소 위치에 상대적인 공용 경로를 사용하므로,
GitHub에서 저장소를 clone하면 별도의 라이브러리 다운로드나 경로 입력 없이 사용할 수 있다.
`dependencies`도 프로젝트 변경과 함께 커밋해야 한다.

기본 디렉터리는 다음과 같다. 압축 파일을 풀었을 때 같은 이름의 폴더가
이중으로 생기는 경우 아래 실제 파일 위치에 맞춰 배치한다.

```text
dependencies/
  glew-2.3.1/
    include/GL/glew.h
    lib/Release/x64/glew32.lib
    bin/Release/x64/glew32.dll
  glfw-3.4.bin.WIN64/
    include/GLFW/glfw3.h
    lib-vc2022/glfw3dll.lib
    lib-vc2022/glfw3.dll
  glm/
    glm/glm.hpp
    glm/gtc/...
```

- GLEW Windows 바이너리: https://glew.sourceforge.net/
- GLFW 64-bit Windows 바이너리: https://www.glfw.org/download.html
- GLM 헤더: https://github.com/g-truc/glm/releases

외부 폴더를 그대로 사용하려면 `OpenGL.local.props.example`을
`OpenGL.local.props`로 복사하고 `GlewRoot`, `GlfwRoot`, `GlmRoot`를 실제 위치로 수정한다.
`GlmRoot`는 `glm/glm.hpp`를 포함하는 상위 폴더이며, 모든 디렉터리 값 끝에는 `\`를 붙인다.
GLFW import library와 DLL은 같은 배포본의 `lib-vc2022`에서 가져온다.
다른 MSVC 디렉터리를 사용하려면 `GlfwLibraryDir`도 수정한다.
로컬 설정과 원본 `opengl_libs` 폴더/ZIP은 Git에서 제외된다.
`dependencies`의 헤더·라이선스 및 지정한 4개의 LIB/DLL은 Git에 포함된다.
다른 빌드 산출물의 LIB/DLL과 x64 출력 폴더는 계속 제외된다.
공용 설정의 기본 경로를 사용하면 `OpenGL.local.props`는 필요 없다.

## 적용된 설정

- Debug: `/MDd`, Release: `/MD`.
- 라이브러리: `opengl32.lib;glew32.lib;glfw3dll.lib`.
- GLFW DLL 선언: `GLFW_DLL`; GLFW의 OpenGL 헤더 중복 포함 방지: `GLFW_INCLUDE_NONE`.
- 컴파일 옵션: `/utf-8`.
- 빌드 완료 시 `glew32.dll`과 `glfw3.dll`을 `$(TargetDir)`에 자동 복사한다.
- 디버깅 작업 디렉터리: `$(ProjectDir)` (프로젝트의 상대 shader/data 경로용).
- 기존 `<glew.h>`/`<glfw3.h>`와 표준 `<GL/glew.h>`/`<GLFW/glfw3.h>`를 모두 지원한다.
- 사용하지 않는 FreeGLUT의 props import 및 복원 강제 검사를 제거했다.
  기존 packages 파일은 보존했으며 현재 빌드에서는 사용하지 않는다.
- Win32 구성은 보존했지만 현재 의존성은 x64 전용이다.
  Win32 선택 시 명확한 안내 오류를 내며 x64 라이브러리를 잘못 링크하지 않는다.
- 누락된 헤더/LIB/DLL은 컴파일 전에 실제 누락 경로를 표시한다.

## 직접 확인할 순서

1. GitHub에 올릴 때 `dependencies`와 `OpenGL.props`, 프로젝트 설정 파일을 함께 커밋한다.
2. Visual Studio에서 솔루션을 다시 열고 `Debug | x64`를 선택한다.
3. 기존 설정이 저장된 개별 `.vcxproj.user`가 있다면 디버깅 작업 디렉터리를
   `$(ProjectDir)`로 맞춘다.
4. 솔루션 정리 후 다시 빌드하고 실행한다. 실행 파일 옆에 두 DLL이 복사되는지 확인한다.
5. `Release | x64`에서도 빌드·실행한다.
6. v145 도구 집합 또는 Windows SDK가 없다는 오류가 나오면 Visual Studio Installer에서
   해당 C++ 도구를 준비하거나 설치된 도구 집합으로 프로젝트를 재대상 지정한다.

빌드·테스트·실행은 자동 수행하지 않았다. 설정 XML과 변경 내용의 정적 검토만 수행했다.
따라서 컴파일 성공과 실제 그래픽 동작은 아직 검증되지 않았다.

참고: [GLFW 애플리케이션 빌드](https://www.glfw.org/docs/latest/build.html),
[GLEW 설치 및 링크](https://glew.sourceforge.net/install.html).
