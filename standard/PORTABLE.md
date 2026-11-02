# 독립 배포와 환경

현재 기준: 2026-11-02. 새 ZIP을 만들거나 앱을 실행하지 않았다.

standard 폴더 전체가 프로젝트 단위다. standard.sln을 우선 사용하고 최신 VS에서는 .slnx도 사용한다. C++ 데스크톱 도구·Windows SDK·OpenGL 3.3 드라이버가 필요하다. Toolchain.props는 설치된 기본 도구 집합을 우선하고 필요 시 v143/v145를 선택한다. x64만 지원하며 C++17, /utf-8, /MTd 또는 /MT를 적용한다.

Framework.props는 include 경로와 런타임 배치를 정의한다. 실행 파일 옆 framework/dependencies 내부 경로의 GLFW DLL, VCRUNTIME140 DLL, 원본 라이선스를 유지한다. 외부 PC 절대경로·NuGet·vcpkg 설치 없이 동봉된 소스를 사용한다는 구성 의도이며, 다른 PC에서의 실제 빌드·실행 성공은 확인하지 않았다.

기본 모드 0/7/8/9와 관절 조작은 [README](README.md), 현장 구현 순서는 [시험 문서](docs/EXAM_IMPLEMENTATION_ORDER.html), 사용자 확인은 [USER_CHECKS](USER_CHECKS.md)를 따른다. 9번은 착석 사람·그네·개별 관절 SRT 편집을 포함한다. 실습 23 보류라는 과거 문구는 현재 상태가 아니다.

이 작업의 확인 기준은 압축 전 standard 폴더 전체 30 MB다. 과거 문서의 ZIP 3,000,000바이트 제한과 다른 기준이다. 실제 합계와 MB/MiB는 [SIZE_REPORT](SIZE_REPORT.md)를 따른다. 빌드 산출물·IDE 캐시가 나중에 생기면 폴더 크기도 바뀐다. 파일을 삭제하여 용량을 줄이는 작업은 하지 않았다.
