# 모드0 조립 API 구조 그래프

2026-11-03. 169개 C++ 파일, 3,654개 노드, 6,922개 관계. 소스 커밋 `23a1eaef967df1e2e3b9312023feefc881d59d88`.

- [전체 구조·프로젝트 진입점·CPU/GPU 흐름](GRAPH_REPORT.md)
- [인터랙티브 그래프](graph.html)
- [Graph JSON](graph.json)
- [프로젝트 목록](PROJECTS.json)
- [입력 파일 해시·방법·한계](SOURCE_MANIFEST.json)
- [추출 원본](EXTRACTION.json)
- [조립 사용 문서](../../standard/docs/assembly/README.md)
- [사용자 실행 확인](../../standard/docs/assembly/USER_CHECKS.md)
- [정리 후보·보존 이유](CLEANUP_CANDIDATES.md)

빌드·테스트·앱 실행·추출 진단 검증기는 실행하지 않았다. 콜백/가상 호출 및 별도 프로그램 사이의 동명 연결은 실제 소스와 구분해 읽는다. 기존 그래프 스냅샷은 보존했다.
