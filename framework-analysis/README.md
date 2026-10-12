# framework 전용 graphify 분석

2026-10-12 계층 SRT·UV 생성·로더 분리를 반영했다. 입력은 루트 framework의 C++ 20개 파일이며 출력은 framework-analysis/graphify-out이다. SDK·문서·과제 사본·GLSL 내부는 그래프 입력에서 제외했다. examples의 세 main은 별도 프로그램이다.

- graph.html: 인터랙티브 구조 그래프.
- graph.json: 643개 노드와 1013개 방향 관계, 소스 파일·위치·confidence.
- GRAPH_REPORT.md, COMMUNITIES.json, ANALYSIS.json: 26개 커뮤니티와 구조 요약.
- FRAMEWORK_FLOW.md: 실제 소스와 대조한 초기화·계층 변환·UV·GPU·피킹·종료 흐름.
- GRAPH_HEALTH.md, EXTRACTION.json: 해석되지 않은 끝점 49개, 같은 끝점 방향 관계 병합 82개 및 원시 관계.
- SOURCE_MANIFEST.json: 입력 경로·SHA-256·기준 커밋. includes_uncommitted_step4는 최종 문서 커밋 전의 예제와 호환성 수정도 입력에 포함했다는 뜻이다.
- BENCHMARK.json: 미실행 상태. 이전 검색 크기 비교는 갱신된 그래프의 결과로 사용하지 않는다.
- CHANGES.md, BEFORE_AFTER.md: 변경과 분석 문서의 전후.

update_graph.py는 현재 설치된 graphify의 AST·분석·내보내기 API를 사용한다. .inl은 이 프로세스에서만 C++ 추출기에 등록한다. 설치 파일을 수정하지 않는다. UTF-8 BOM JSON은 utf-8-sig로 읽고, 새 그래프를 별도 파일로 내보낸 뒤 기존 노드 수보다 작지 않을 때 교체한다. 런타임과 cache_root는 framework-analysis이며 기본 CLI의 소스 내부 출력으로 되돌리지 않는다.

재분석은 저장된 graphify Python 인터프리터로 update_graph.py를 실행한다. Windows 쉘에 PYTHONUTF8=1, PYTHONIOENCODING=utf-8 및 콘솔 UTF-8을 설정한다. 사용자 산출물은 UTF-8 BOM으로 저장한다. 소스 구조 분석만 수행하며 빌드·테스트·벤치마크는 실행하지 않는다. 현재 분석은 외부 의미 분석 API를 호출하지 않았다.

archive/framework-graphify-out-a194f47과 ARCHIVE_MANIFEST.json은 과거 보관 자료다. 수정하지 않았다. 저장소 루트의 다른 graphify-out도 수정하지 않았다. 사용자 실행 항목은 ../framework/docs/USER_CHECKS.md에 있다.
