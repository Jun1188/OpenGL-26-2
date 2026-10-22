# 프레임워크 구조 분석

2026-10-22. 루트 framework의 C++ 22개 파일을 graphify AST로 분석한다. SDK·문서·PDF·셰이더 내부·과제 사본은 제외한다. graphify-out의 graph.html/graph.json/GRAPH_REPORT.md는 구조, FRAMEWORK_FLOW.md는 소스와 대조한 실행 흐름이다. EXTRACTION.json/GRAPH_HEALTH.md는 원시 관계와 한계를 기록한다.

update_graph.py는 --scope framework(기본) 또는 repository를 받는다. 전체 프로젝트·조건부 진입점은 저장소 graphify-out/PROJECTS.json과 EXECUTION_FLOW.md에 있다. .inl은 프로세스별 C++ 추출기로 각각 처리한다. 코드 삭제에 따른 노드 수 감소를 허용하고 빈 추출은 실패로 처리한다.

저장된 graphify Python으로 실행하고 프로세스별 PYTHONUTF8/PYTHONIOENCODING=utf-8을 설정한다. 산출물은 UTF-8 BOM이다. 외부 의미 분석 API·빌드·테스트·벤치마크는 실행하지 않았다. archive는 과거 산출물과 배포본 밖으로 옮긴 변경 보고다. 현재 변경 보고는 ../docs/practice-14-19에 있다.

Motion·Transform 헤더/구현과 독립 motion 예제를 포함했다. 소스·배포본·문서 변경 전후의 전체 코드 블록은 MOTION_BEFORE_AFTER.md에 있다. PROJECTS.json은 독립 예제 3개를 구분한다. 그래프의 동적 콜백 연결·미해결 끝점 등 한계는 GRAPH_HEALTH.md와 소스 대조를 함께 읽는다. 빌드·테스트·벤치마크는 사용자에게 맡긴다.
