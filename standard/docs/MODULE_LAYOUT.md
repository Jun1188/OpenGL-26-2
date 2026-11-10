# Standard·framework·debugger 모듈 경계

```text
standard/
  standard.sln             # Standard만 참조
  standard.vcxproj         # main/Console/SceneSetup + 자기 framework
  SceneSetup.cpp           # 사용자가 직접 편집하는 C++ 원본
  framework/               # 독립 배포 사본, debugger 참조 없음
  debugger/
    debugger.sln           # 도구만 빌드하는 단독 솔루션
    debugger.vcxproj       # 자기 C++ 소스 + Windows/표준 라이브러리
    Toolchain.props        # 도구 내부 설정
    x64/Debug/debugger.exe # 사용자가 빌드한 뒤 생기는 실행 파일
```

| 모듈 | 실제 빌드 입력 | 다른 모듈과의 관계 |
|---|---|---|
| Standard | main.cpp, Console.cpp, SceneSetup.cpp, framework/Framework.props | framework API 사용. debugger 소스·프로젝트·실행 파일 참조 없음 |
| framework 원본/배포 사본 | 자기 헤더·detail 소스·셰이더·SDK·런타임 DLL | debugger를 include/import/link/실행하지 않음 |
| debugger | 자기 main.cpp, Commands.cpp, SourceIndex.cpp, SourceIndex.h, Toolchain.props | Standard/framework를 링크하지 않음. 사용자가 고른 소스 파일만 읽음 |

저장소 루트 OpenGL-26-2.sln은 두 프로젝트를 함께 보여주는 편의 파일이다. debugger의 기본 Build.0 선택을 두지 않는다. Standard만 빌드할 때는 standard.sln을 사용하고, 도구는 내부 debugger.sln이나 루트 솔루션의 debugger 프로젝트를 명시적으로 선택한다. 프로젝트 사이의 ProjectReference/ProjectDependencies를 추가하지 않는다.

debugger의 출력은 C++ 편집 제안이며 실행 중인 Standard에 호출하는 API가 아니다. @debug 표시는 위치 검색용 주석이라서 Standard/framework 실행 의존성이 아니다. Standard를 대상으로 색인할 때 내부 debugger 폴더는 순회하지 않는다. 도구 자체를 편집하려면 debugger 폴더를 project 대상으로 명시한다.

standard 전체 폴더 또는 debugger 폴더만 별도 위치로 복사할 수 있다. debugger를 뺀 Standard 복사본도 standard.sln/standard.vcxproj와 framework의 참조를 그대로 사용한다. 폴더 분리 후 빌드·실행 확인은 사용자가 수행한다. 이번 작업에서는 소스 참조와 프로젝트 구성을 읽었고 빌드·테스트·앱은 실행하지 않았다.
