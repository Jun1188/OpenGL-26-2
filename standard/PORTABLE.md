# 독립 배포와 사용자 확인

1. standard.zip을 풀고 standard.sln을 연다. .slnx도 유지하지만 이전 VS 2022에서는 .sln을 사용한다.
2. Visual Studio의 C++ 데스크톱 개발 도구와 Windows SDK가 필요하다. 설치된 기본 PlatformToolset을 유지하고, 비어 있으면 framework/Toolchain.props에서 설치된 v145/v143을 선택한다. 필요하면 프로젝트 속성에서 v143을 선택한다.
3. x64 Debug/Release 구성이다. C++17과 /utf-8을 사용한다. C++ 런타임은 /MTd 또는 /MT로 정적 연결한다. GLFW는 동봉한 x64 DLL을 동적 로딩한다. GLFW 옆 VCRUNTIME140.dll과 Microsoft 원본 라이선스도 동봉한다. OpenGL 드라이버와 Windows 시스템 DLL은 운영체제에서 제공한다.
4. framework 폴더 전체를 유지한다. 외부 PC 경로·NuGet·vcpkg·네트워크 다운로드는 필요 없다. 빌드 후 DLL과 라이선스, 셰이더는 실행 파일 옆 framework 아래로 복사된다.
5. 기본 0 예제는 기존 Motion·반사·레이어·피킹이다. 7은 Grid·거리 기반 배치, 8은 회전 실내의 미끄럼·낙하다. V는 3뷰, D는 단계 로그, F는 실내 바닥 열기/닫기, Space는 정지/재개다. 기존 모드 키는 H로 확인한다.
6. 분할 피킹은 각 뷰포트의 카메라와 DPI 보정 좌표를 사용한다. 리사이즈와 최소화를 직접 확인한다.
7. 각 독립 ZIP의 한도는 3,000,000바이트 미만이다. 컴파일 산출물·IDE 상태를 제외하며 라이선스와 필요한 SDK를 보존한다.

빌드·실행·호환성·화면·테스트는 사용자가 확인한다. 에이전트가 수행하지 않았다. Debug 산출물의 배포보다 대상 PC에서 소스를 빌드하는 흐름이다. 실습 23은 공동 개발을 위해 보류한다.
