# API 관계도·명세 정리

- 1단계 `e0d5fc2`: 자체 헤더 41개를 조사하고 37개에 공백·줄바꿈만 반영. 주석·코드·리터럴 토큰을 대조했으며 동일했다.
- 2단계 `4386ff7`: API 문서 최상단에 관계도 4개와 종합도 1개. graphify AST는 69개 코드 파일, 3,495개 노드, 5,234개 간선. 외부 API 호출·토큰 비용 0.
- 3단계: 문서 3곳의 공개 선언 264개를 호출 단위 C00~C05와 상태 소유자별로 정리. 기존 api-N 링크를 유지하고 선언 주석·소스 블록·줄 번호·JSON을 갱신.

## 읽을 파일

- [관계 설명](ARCHITECTURE.md)
- [헤더 전체 변경 전후](HEADERS_BEFORE_AFTER.md)
- [HTML 코드 블록 전체 변경 전후](HTML_CODE_BEFORE_AFTER.md)
- [문서 생성기 전체 변경 전후](TOOLS_BEFORE_AFTER.md)
- [graphify 그래프](graphify-out/graph.html), [분석 보고서](graphify-out/GRAPH_REPORT.md), [추출 한계](graphify-out/GRAPH_HEALTH.md)
- [사용자 확인 항목](USER_CHECKS.md)

## 갱신

공개 문서만 갱신할 때 기존 graphify 인터프리터로 `tools/Refresh-ApiArchitecture.py`와 `tools/Refresh-ApiCatalog.py`를 순서대로 실행한다. AST도 다시 추출할 때 첫 명령에 `--graph`를 지정한다. 기존 `author_framework_guides.py`의 전체 문서 생성 경로에도 두 갱신 절차를 연결했다. 그 전체 생성 경로는 이번 작업에서 실행하지 않았다.

빌드·테스트·브라우저·IDE hover 확인은 사용자에게 맡겼다. 코드 정적 대조를 실행 성공으로 간주하지 않는다. 자동 정리·기존 임시파일 삭제·push는 하지 않았다.

## 외부 헤더 경계 보완

기존 그래프에서 빠졌던 외부 헤더 33개와 include 간선 123개를 보완했다. 내부 코드 재추출·커뮤니티 재분류 없이 기존 노드·간선을 유지했다. 현재 그래프는 3,528개 노드·5,357개 간선이며 연결 대상 누락은 0건이다. 같은 노드 쌍 관계 병합 150건은 별도로 남아 있다.

`tools/Refresh-ApiArchitecture.py --repair-graph`는 저장된 그래프의 누락된 외부 include 경계만 보완한다. 이후 전체 추출에서도 같은 경계 노드를 생성하도록 연결했다. [상세 근거와 변경 전후](BOUNDARY_REPAIR.md), [복원한 원본 간선](graphify-out/BOUNDARY_REPAIR.json)을 참조한다.
