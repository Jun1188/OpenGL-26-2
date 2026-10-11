# framework 전용 graphify 분석

입력: 저장소 루트 framework의 C++ 17개 파일. 출력: framework-analysis/graphify-out. 분석 자료는 복사용 framework 폴더와 분리한다. 저장소 루트의 기존 graphify-out은 별개 분석이며 수정하지 않았다.

- graphify-out/graph.html: 인터랙티브 그래프
- graphify-out/graph.json: 노드·관계와 source_file/source_location
- graphify-out/GRAPH_REPORT.md: 중심 노드, 커뮤니티, 교차 관계, 탐색 질문
- graphify-out/FRAMEWORK_FLOW.md: 실제 소스와 대조한 초기화·입력·렌더링·정리 흐름
- graphify-out/GRAPH_HEALTH.md 및 EXTRACTION.json: 손실·미해석 관계 진단과 원시 추출
- graphify-out/SOURCE_MANIFEST.json: 입력 범위와 소스 SHA-256
- graphify-out/BENCHMARK.json: 구조 검색 결과의 추정 크기
- CHANGES.md: 이번 이동·재분석의 변경 전후

SDK, 변경 전후 보고서, 프로젝트 밖 파일은 그래프 입력에서 제외했다. examples의 두 main은 서로 다른 사용자 호출 예제다. .inl은 이 추출 프로세스에서만 C++ 추출기에 등록했다. graphify 설치 파일은 변경하지 않았다. GPU 셰이더는 내장 문자열로 존재하며 C++ 구조 그래프가 GLSL 내부를 별도 해석하지는 않는다.

613개 노드, 964개 방향 관계, 23개 커뮤니티. 원시 관계에서 미해석 끝점 48개와 같은 끝점 병합 77개를 확인했다. 코드는 AST로만 분석했고 의미 분석 API 비용은 없다. BENCHMARK는 전체 10,621 추정 토큰 대비 검색 평균 3,066 추정 토큰(3.5배)의 크기 비교다. 실제 토큰 과금이나 답변 품질·속도 측정이 아니다.

이전 framework/graphify-out 전체는 archive/framework-graphify-out-a194f47로 이동했다. 이전 전체 저장소 PROJECTS/EXECUTION_FLOW는 보관본에만 있다. 현재 전용 그래프에는 포함하지 않았다. 캐시는 보관하되 Git에 추가하지 않는다.

다시 분석할 때 소스 루트는 ../framework, 작업·출력 위치는 이 framework-analysis 폴더로 설정한다. SOURCE_MANIFEST의 파일 목록만 AST 입력에 넣고 cache_root도 framework-analysis로 지정한다. 기본 CLI의 소스 내부 출력으로 되돌리지 않는다. Windows 실행 쉘에 PYTHONUTF8/PYTHONIOENCODING=UTF-8을 설정하고, 사용자 산출물은 UTF-8 BOM으로 읽고 쓴다. JSON을 읽는 도구는 utf-8-sig로 BOM을 처리한다. .graphify_root/.graphify_python은 로컬 경로이므로 Git에서 제외한다.

빌드·framework 실행·동작 테스트는 하지 않았다. framework/docs/USER_CHECKS.md의 테스트는 사용자가 수행한다.
