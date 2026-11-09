# 코드 구조 그래프

156개 파일, 3703 nodes, 7125 edges. graphify AST 기반이며 의미 분석 API·빌드·테스트·실행은 사용하지 않았다.

[전체 실행 구조와 진입점](../../docs/practice-24-26-27/ARCHITECTURE.md) · [대화형 그래프](graph.html) · [소스 범위](SOURCE_MANIFEST.json)

독립 프로젝트의 같은 클래스 이름을 하나의 실행 흐름으로 해석하지 않는다. 콜백·동적 분기는 소스 검토가 필요하다. 중복 배포 프레임워크와 SDK는 그래프 노드 추출에서 제외하고 각 프로젝트의 실제 컴파일 파일은 PROJECTS.json에 기록했다.
