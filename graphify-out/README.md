# 저장소 코드 구조

2026-11-02. graphify AST와 기존 update_graph.py의 소스 앵커 절차로 코드 64개 파일을 분석했다. SDK·문서·PDF·중복 프레임워크는 제외했다. 실습 23에서 수정된 Patterns.inl/Patterns.h는 루트 프레임워크와 별도로 포함했다.

PROJECTS.json은 각 실행 프로젝트와 독립 예제의 진입점을 구분한다. EXECUTION_FLOW.md는 콜백 등록·실행 조건과 CPU → GPU 흐름을 설명한다. PRACTICE23.md는 이번 코드의 전체 변경 전후와 사용자 확인 절차다.

SOURCE_MANIFEST/SOURCE_HASHES는 생성 시점 작업 트리의 소스 스냅샷이다. source_commit은 그 시점 HEAD이며 includes_uncommitted_changes=true이다. 기존 사용자 변경도 입력에 포함되므로 이번 수정과 구분해야 한다.

그래프는 2,083개 노드와 3,306개 관계다. 같은 이름·간접 호출의 관계는 실제 소스와 대조해야 한다. 빌드·실행·테스트·벤치마크·검증기·그래프 무결성 진단은 실행하지 않았다. cost.json의 0 tokens는 외부 의미 분석 API 비용이며 대화 토큰이 아니다.
