# 편집 가능한 API 변경 안내

- [의미 동작·육면체 초기 설정/바인딩](../../framework/docs/MODE_ACTIONS.md)
- [정점 입력·셰이더 연결·내부 코드 수정 순서](../../framework/docs/RENDER_CONFIGURATION.md)
- [주요 실습 패턴의 의미 단위 조립](../../framework/docs/PATTERN_COMPOSITION.md)
- [면 독립·계층과 힌지·자전·공전](../../framework/docs/FACE_TRANSFORMS.md)
- [전체 코드 before/after](ALL_CODE_BEFORE_AFTER.md)
- [프로젝트 진입점과 CPU/GPU 흐름](../../graphify-out/EXECUTION_FLOW.md)
- [사용자 빌드·실행 확인](../../framework/docs/USER_CHECKS.md)
- [보존·정리 후보](PRESERVATION.md)

1단계 a447999: 정점 입력·셰이더 설정. 2단계 621b20c: 의미 동작·육면체 모드. 3단계 9410b43: 패턴 API 압축과 문서. 4단계는 현재 변경의 별도 커밋이다.

루트 framework와 standard/실습 13~23의 관련 framework 파일만 동기화했다. 사용자 실습 Console 코드는 바꾸지 않았다.
정적 소스·graphify 분석을 수행했으며 빌드·테스트·앱·브라우저 실행은 사용자에게 맡긴다.
세 전용 조합 API 제거는 저장소 외부의 호출부에는 수정이 필요하다. sample/seek의 동작 실행과 일회 이벤트를 혼동하지 않는다.
