# Standard·framework·debugger 모듈 경계

```text
standard/
  standard.sln             # Standard만 참조
  standard.vcxproj         # main/Console/SceneSetup/ExamController + 자기 framework
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
| Standard | main.cpp, Console.cpp, SceneSetup.cpp, ExamController.cpp, framework/Framework.props | framework API 사용. debugger 소스·프로젝트·실행 파일 참조 없음 |
| framework 원본/배포 사본 | 자기 헤더·detail 소스·셰이더·SDK·런타임 DLL | debugger를 include/import/link/실행하지 않음 |
| debugger | 자기 main.cpp, Commands.cpp, SourceIndex.cpp, SourceIndex.h, Toolchain.props | Standard/framework를 링크하지 않음. 사용자가 고른 소스 파일만 읽음 |

저장소 루트 OpenGL-26-2.sln은 두 프로젝트를 함께 보여주는 편의 파일이다. debugger의 기본 Build.0 선택을 두지 않는다. Standard만 빌드할 때는 standard.sln을 사용하고, 도구는 내부 debugger.sln이나 루트 솔루션의 debugger 프로젝트를 명시적으로 선택한다. 프로젝트 사이의 ProjectReference/ProjectDependencies를 추가하지 않는다.

debugger의 출력은 C++ 편집 제안이며 실행 중인 Standard에 호출하는 API가 아니다. @debug 표시는 위치 검색용 주석이라서 Standard/framework 실행 의존성이 아니다. Standard를 대상으로 색인할 때 내부 debugger 폴더는 순회하지 않는다. 도구 자체를 편집하려면 debugger 폴더를 project 대상으로 명시한다.

standard 전체 폴더 또는 debugger 폴더만 별도 위치로 복사할 수 있다. debugger를 뺀 Standard 복사본도 standard.sln/standard.vcxproj와 framework의 참조를 그대로 사용한다. 폴더 분리 후 빌드·실행 확인은 사용자가 수행한다. 이번 작업에서는 소스 참조와 프로젝트 구성을 읽었고 빌드·테스트·앱은 실행하지 않았다.


## 2차 코딩테스트 작업공간

2026-11-24 화요일 시험 준비의 전체 학습 HTML·순정 OpenGL·N제·시나리오는 저장소의 coding-test-2 폴더에 별도로 둔다. Standard 제출 입력은 해당 학습 폴더를 참조하지 않는다. 같은 저장소에서는 [오프라인 학습](../../coding-test-2/index.html)으로 이동할 수 있다. Standard만 복사한 배포본에서는 이 링크가 학습 참고 링크이며 빌드/실행 입력이 아니다.

ExamController는 key 광원과 월드 카메라, studySphere/studySpin, lightMarker를 소유한다. 기존 초기 광원을 읽고 부모 이름을 보존한다. 다른 소스가 같은 상태를 동시에 쓰지 않는다. 전체 장면을 기존 examples 함수로 교체할 때는 main.cpp의 Console sample(app, false)로 기본 컨트롤러를 끈다. 기본 실행은 true다.

외부 GLSL은 main의 shaderDirectory=framework/shaders로 선택한다. 소스 GLSL 수정 후 사용자가 Rebuild하여 출력 폴더의 사본도 갱신한다. 내장 detail/Shaders.h만 바꿔도 이 외부 경로가 바뀌는 것은 아니다. uStudyView=0/1/2/3은 합성/법선/확산/정반사이며 기존 내장 셰이더 사용자에게 기본 0을 유지한다.

제출은 debugger, 프로젝트 x64/.vs, docs, 튜토리얼·학습 HTML·예제를 뺀 소스 기준 30,000,000바이트 이하다. SDK의 x64 라이브러리·DLL·헤더·라이선스는 남긴다. Package-Standard.py는 실제 파일 삭제 없이 이 범위로 ZIP을 만든다. 표준 개발 환경(OS·MSVC·Windows SDK·호환 드라이버)은 필요하다. 상위/형제 폴더·개인 절대 경로의 실행/빌드 입력을 두지 않는다.

빌드·테스트·브라우저/앱 실행·화면 결과는 사용자 확인 대상이다. 소스·문서 생성과 실행 성공을 구분한다.
