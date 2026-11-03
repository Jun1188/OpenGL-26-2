# 사용자 확인 항목

빌드·테스트·브라우저 실행·IDE hover 확인은 Codex가 실행하지 않았다. 아래 항목은 사용자가 확인한다.

1. `standard/framework/docs/API_REFERENCE.html`을 브라우저에서 연다. 상단의 관계도 4개와 종합도 1개에서 텍스트·화살표가 겹치지 않는지 확인한다.
2. 호출 인덱스 C01~C05와 소유자 링크로 이동한다. `C04`, `Renderer`, `setParent`를 검색하고 해당 항목만 표시되는지 확인한다. C00은 실행 순서가 없는 공통 계약·도우미다.
3. Application::renderScene 항목에서 `[API]` 주석과 실제 선언을 확인한다. 호출됨에는 Console::onRender와 Application::run의 위치가 표시되어야 한다. 오버로드별 확정 연결은 아니다.
4. Visual Studio에서 `Application.h`의 setInitHandler, renderScene와 `Graphics.h`의 Scene::setParent에 커서를 올린다. 선언 바로 위의 `/// [API]` 설명이 Quick Info에 표시되는지 확인한다. 주석과 선언 사이에는 빈 줄이 없다.
5. 필요하면 평소 방법으로 standard를 빌드·실행한다. 입력·갱신·렌더링·종료가 기존과 같은지 확인한다. 코드 토큰은 변경하지 않았다. IDE 인덱싱·컴파일·실행 성공은 아직 확인하지 않았다.

외부 라이브러리는 변경하지 않았다. graphify의 AST 그래프에는 연결 대상 누락 123건과 같은 노드 쌍 관계 병합 150건이 있다. 원본 추출 결과를 보존했고 순서도는 실제 소스를 대조해 작성했다.
