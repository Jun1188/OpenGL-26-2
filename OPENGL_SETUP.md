# OpenGL 의존성 설정

모든 외부 라이브러리는 framework/dependencies 안에 둔다. Windows SDK, 시스템 OpenGL과 VC 런타임은 운영 환경에 필요하다.

- 기존 실습 1~12: OpenGL.props가 framework/LegacyOpenGL.props를 가져온다. GLEW/GLFW import library와 DLL을 사용한다.
- 실습 13~19: 각 프로젝트 안의 framework/Framework.props를 사용한다. 헤더 구현과 번들 DLL을 사용한다.
- 개인 SDK 경로, OpenGL.local.props, NuGet FreeGLUT 복원은 사용하지 않는다.
- x64 구성만 번들 라이브러리와 호환된다. 기존 프로젝트의 언어 표준과 도구 집합을 유지한다.

빌드와 테스트는 사용자가 수행한다. Visual Studio에서 Debug/Release x64를 선택하고 DLL 배치와 화면 표시를 확인한다.
