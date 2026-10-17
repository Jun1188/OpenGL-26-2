# 저장소 코드 구조

2026-10-17. regenerate.py는 framework-analysis/update_graph.py --scope repository를 호출한다. 코드 105개 파일을 분석한다. SDK·문서·PDF·과제별 프레임워크 복사본은 제외하고 루트 원본을 사용한다.

PROJECTS.json은 각 실행 프로젝트와 조건부 main을 구분한다. EXECUTION_FLOW.md는 콜백 등록과 실행 순서·CPU→GPU 흐름이다. 같은 이름 메서드·간접 호출은 소스와 대조해야 한다. GRAPH_HEALTH.md의 미해결 관계는 분석 한계다.

SOURCE_MANIFEST/SOURCE_HASHES는 최종 소스 스냅샷이다. source_commit은 분석 시점의 HEAD이고 includes_uncommitted_changes=true로 해당 작업의 최종 소스를 포함한다. 문서 커밋 뒤 HEAD가 달라지는 것은 정상이다. 빌드·실행·테스트·벤치마크는 수행하지 않았다. cost.json의 0 tokens는 graphify 외부 의미 분석 비용이며 대화의 전체 토큰 사용량이 아니다.

PRACTICE15.md와 TOPDOWN_13_14_15.md는 이전 버전 기록이다. 현재 설명은 EXECUTION_FLOW.md와 ../docs/practice-14-19/README.md를 우선한다.
