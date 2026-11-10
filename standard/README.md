# Standard: C++ 직접 작성 프로젝트

`SceneSetup.cpp`에 객체·변환·조명·재질·카메라를 C++ 코드로 작성한다. 수정 후 사용자가 빌드·실행한다.

[전체 문서](index.html) · [모듈 경계](docs/MODULE_LAYOUT.md) · [작성 흐름](docs/DIRECT_CPP_WORKFLOW.md) · [두 물체 튜토리얼](docs/WORKSPACE_TUTORIAL.md) · [디버거 명령](docs/SOURCE_DEBUGGER.md) · [조명](docs/LIGHTING_AND_MATERIALS.md) · [사용자 확인](USER_CHECKS.md)

- `standard.sln`: Standard만 참조하는 직접 작성 프로젝트. debugger 폴더가 없어도 이 솔루션에 도구 참조가 남지 않는다.
- `SceneSetup.cpp`: 사용자가 직접 고치는 장면 설정. 문자열 명령·외부 assembly 파일을 실행하지 않는다.
- `Console.cpp`: 초기화·마우스 객체 이름 확인·시간 갱신·렌더 콜백.
- `framework/`: 이 폴더 안의 배포 사본. 상위·형제 폴더 없이 빌드/실행에 사용한다.
- `debugger/`: 소스 편집 안내용 독립 콘솔 프로젝트. Standard와 framework의 필수 모듈이 아니다. `debugger/debugger.sln`으로 따로 연다.
- `old_standard/`: 저장소의 형제 폴더에 보존한 이전 명령·레시피 버전. 그 폴더의 `standard.sln`으로 별도 연다.

클릭은 선택한 객체 이름과 `show object 이름` 안내만 출력한다. Space는 시간 정지/재개, Home은 시간 초기화, Esc는 종료다. 객체 속성은 코드에서 편집한다. 물체 전체 선택이 기본이며 면 편집 명령 모드는 현재 Standard 실행에 포함하지 않는다. 필요하면 기존 면 변환 C++ API를 직접 사용한다.

`debugger`에 원하는 변경을 입력하면 현재 소스의 파일·행·수정 전후 코드가 출력된다. 소스 저장, 재빌드, 화면 확인은 사용자가 수행한다. 빌드·테스트·앱 실행은 에이전트가 수행하지 않았다.
