# 사용자가 수행할 빌드·테스트·화면 확인

에이전트는 소스 작성·소스 열람·문서/구조 자료 생성만 수행했다. 아래 항목은 실행하지 않았다.

1. `standard/standard.sln`은 Standard만 참조하고 루트 `OpenGL-26-2.sln`은 두 프로젝트 중 Standard만 기본 빌드하는지 확인한다. 루트 솔루션에서 debugger는 프로젝트를 선택해 별도로 빌드한다. `standard/debugger/debugger.sln`에서는 도구만 빌드한다. Standard 컴파일 목록에 Assembly*.cpp/debugger 소스/ProjectReference가 없고 SceneSetup.cpp와 framework/detail/SceneAuthoring.cpp가 있는지 확인한다.
2. debugger를 실행하고 `project "자신의 경로/standard/standard.vcxproj"`, `list`, `show light key`, `light key position 5 6 7`을 입력한다. 절대 경로·실제 행·before의 기존 position·after `{5.f, 6.f, 7.f}`를 소스와 대조한다. 생성 숫자는 유효한 동등 float 표기로 나올 수 있다.
3. 소스 구획 앞에 빈 줄 두 개를 넣어 저장한 뒤 같은 명령을 반복한다. 안내 행은 두 줄 이동해야 한다. position 기존 값을 수동 수정해 저장한 뒤 before도 새 값으로 바뀌는지 확인한다. 대상 소스가 프로그램에 의해 저장되지 않는지도 확인한다.
4. `add object gate box base`의 전체 after를 안내 위치에 반영한다. 저장 후 `show object gate`, `object gate position 3 1 0`이 새 구획과 실제 변수를 찾는지 확인한다.
5. `layer hinge spin 30`을 수동 반영하고 Standard를 재빌드·실행한다. 부모/지역 레이어의 회전을 관찰한다. Space 정지·Home 시간 초기화·클릭의 객체 이름 출력을 확인한다.
6. `light key parent base`와 조명 위치·강도·재질 Ks/N 수정안을 수동 반영한다. 재빌드해 광원과 재질의 효과를 각각 관찰한다. 카메라 변경 시 position/target/up과 near/far가 함께 유효한지 확인한다.
7. 잘못된 숫자, 0 배율, 0 회전축, 없는 구획, 중복 구획, 여러 대입문, 존재하지 않는 파일/행을 입력한다. 잘못된 위치를 성공으로 안내하지 않고 오류를 표시해야 한다. 임시로 바꾼 사용자 소스는 직접 복원한다.
8. `find "SceneAuthoring::update"`를 framework 대상으로 실행하고 실제 결과를 `code`로 읽는다. Standard 대상으로는 하위 debugger 소스가 검색되지 않고 debugger 폴더를 명시한 project 대상에서는 그 구현이 검색되는지 확인한다. 구획이 없는 일반 실습에도 find/code/replace/insert의 before가 실제 파일과 같은지 확인한다. 이 명령은 MSBuild 활성 분기나 C++ 의미를 판정하지 않는다.
9. 한글·공백 경로에서 UTF-8 BOM 파일을 읽고 EOF/quit로 종료한다. 링크·재분석 지점 아래로 순회하지 않는지 확인한다.
10. standard 폴더 전체와 그 안의 debugger 폴더를 각각 다른 위치로 복사해 단독 프로젝트 빌드·실행을 확인한다. 별도 복사본에서 debugger를 빼도 standard.sln/standard.vcxproj와 framework가 도구 없이 빌드·실행되는지 확인한다. Standard의 framework 배포 사본과 DLL/셰이더 복사 결과도 확인한다. Package-Standard와 용량 집계는 standard 및 내부 debugger의 x64/.vs를 제외하지만 framework/dependencies의 SDK 입력은 보존한다.

`old_standard`는 이전 프로젝트를 보존한 폴더다. 기존 standard 내부에 x64/.vs가 없었으므로 이 이동에서 삭제된 용량은 0바이트다. 저장소 루트의 .vs, 다른 실습의 x64, 사용자 docs.zip·PDF·기존 Renderer.cpp 변경은 정리 범위에서 제외했다.
