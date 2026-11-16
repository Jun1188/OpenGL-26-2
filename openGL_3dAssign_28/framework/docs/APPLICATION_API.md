# 응용 API 읽는 순서

## 편집 가능한 API와 패턴 조립 (2026-11-03)

[의미 동작·육면체 모드](MODE_ACTIONS.md) · [정점 입력·셰이더 연결과 내부 수정 순서](RENDER_CONFIGURATION.md) · [패턴 조립·API 압축 기준](PATTERN_COMPOSITION.md)

sample/seek는 동작을 실행하지 않는다. 자동 모션 갱신은 활성 모션의 동작을 실행한다. 기존 공전+자전·경로+자전 전용 함수는 createMotion 단계 목록으로 대체했다. 모델 교환은 조립 문서를 따른다.

1. [전체 API와 실제 사례](API_REFERENCE.html): 렌더링 루프·계층 행렬·GPU 업로드·자원 수명은 상세히 읽는다.
2. [응용패턴 통합](IMPLEMENTATION_PATTERNS.html): 패턴별 흐름도와 응용 코드를 간결하게 읽는다.
3. [그네·사람형 SRT](ANIMATION_API.md): 착석과 관절 조작을 확인한다.

현재 Scene은 다중 뷰·면 컬링·관절·충돌·보행 API를 제공하며 실습 23은 구현되어 있다. 투영 그림자·FBX/glTF·MTL 자동 로드는 제공하지 않는다.

## 면의 독립·계층과 회전 의미

[면 변환과 회전 분류](FACE_TRANSFORMS.md): 가장자리 힌지·면 자전·원본 모델 중심 공전을 독립/계층 모드와 조합한다. 기존 independence 플래그는 면 SRT 활성화이며 월드 독립과 구분한다.
