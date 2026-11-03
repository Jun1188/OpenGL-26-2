# 문서 작성 범위

- 공개 선언: 265개 (오버로드·수명·삭제 연산 포함)
- 사례 묶음: 23개
- 응용패턴과 SVG 흐름도: 25개

기준은 framework/include의 공개 API 주석이 붙은 선언이다. API_CASE_MAP.json은 선언별 사례 위치와 호출 코드를 연결한다. 외부 라이브러리·private·graphics::detail 함수는 공개 API에서 제외한다. API_KO.json과 API_CASE_MAP.json은 같은 최신 선언·사례 정보를 제공한다. 빌드·테스트·브라우저 확인은 수행하지 않았다.
