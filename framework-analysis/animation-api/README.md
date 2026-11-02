# 애니메이션 API 및 주석 정리

실습 소스의 주석을 함수 기능 한 줄로 정리했다. framework와 standard에 선형보간·진자·관절 계층·편집 가능한 사람형·그네→사람 패턴을 추가했다. 실습 23의 첫 cout 전에 UTF-8 콘솔 설정을 적용했다.

- [통합 응용패턴·사례 코드·패턴별 흐름도](../../standard/docs/IMPLEMENTATION_PATTERNS.html)
- [공통 구현·전체 공개 API 사례](../../standard/docs/API_REFERENCE.html)
- [시험 현장 구현 순서](../../standard/docs/EXAM_IMPLEMENTATION_ORDER.html)
- [현재 standard 전체 용량](../../standard/SIZE_REPORT.md)
- [API 사용법과 사용자 확인 절차](../../framework/docs/ANIMATION_API.md)
- [1단계 전체 전후 코드](STEP1_BEFORE_AFTER.md)
- [2단계 전체 변경 블록](STEP2_BEFORE_AFTER.md)
- [3단계 전체 전후 코드](STEP3_BEFORE_AFTER.md)
- [4단계 소스 정리와 분석 생성 코드](STEP4_BEFORE_AFTER.md)
- [5단계 sample0 BOM 반영](STEP5_ENCODING.md)
- [렌더링·계층 흐름도 HTML](flowcharts.html)
- [전체 실행 구조와 호출·소유·GPU 흐름](STRUCTURE.md)
- [상호작용 그래프](graphify-out/graph.html), [그래프 JSON](graphify-out/graph.json), [graphify 보고서](graphify-out/GRAPH_REPORT.md)
- [임시 파일 보존 목록](CLEANUP_CANDIDATES.md)

커밋: ed24821(주석), bed6ae9(API·예제), b246256(한글 출력). 4단계 e2aae85는 구조 자료·최종 소스 정리를 포함한다. 5단계는 sample0 진입점의 BOM을 커밋한다.
변경 전후 보고서는 다른 작업의 변경을 보존한 실제 작업 시점 소스를 기준으로 한다. 전체 파일을 실은 경우 원래 코드도 함께 표시된다.
별도 작업 중인 docs/code-guide HTML과 관련 기존 변경은 수정·커밋하지 않았다.

빌드·테스트·검증기·앱 실행·브라우저 확인·배포 ZIP 생성은 수행하지 않았다. 사용자가 standard의 9번 예제와 실습 23의 첫 한글 안내 출력을 직접 확인한다.

추가 단계: dd914ec에서 착석 높이·관절 SRT 편집을 반영했다. 최신 문서 작성 범위는 API_CASE_INVENTORY.md, 관절 전체 전후는 STEP7_JOINT_CONTROLS.md를 따른다. 기존 시험 문서는 docs/code-guide/exam.html에 있었으며 standard 독립 문서로 현재 조작을 연결했다.
