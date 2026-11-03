# 브라우저 문서 목차와 Markdown 미리보기

standard/index.html을 브라우저에서 직접 연다. 제목/경로 검색과 Markdown/HTML 필터를 지원한다. Markdown은 document.html?doc=저장소상대경로로 표시한다. 제목별 목차, 표, 중첩 목록, 인용, fenced 코드, 이미지, 링크와 원문 전환을 제공한다. 편집은 원본 .md를 VS Code에서 수행한다.

Marked 16.4.2를 docs/viewer/vendor에 라이선스와 함께 보관한다. 인터넷·서버·fetch 없이 documents.js의 문서를 읽는다. Markdown의 raw HTML은 텍스트로 표시하고 실행하지 않는다. 코드 블록은 읽기용이며 구문 색상, Mermaid/수식 확장 및 VS Code 편집 기능은 제공하지 않는다.

소스 문서를 고친 뒤 저장소에서 python tools/Update-StandardDocs.py를 실행하면 API의 최신 조립 API 절, 전체 목차, Markdown 스냅샷을 함께 갱신한다. API Reference 본문 전체를 다시 생성하지 않고 기존 편집을 보존한다. Refresh-ApiCatalog.py의 정상 생성 경로도 이 단계를 마지막에 호출한다. docs/viewer/documents.js를 직접 편집하지 않는다.

API Reference 세 사본의 최신 절은 setFaceFrame을 설명한다. standard/docs/API_REFERENCE.html은 Assembly의 공개 선언 전체, FeatureContainer/GuideRecipe, 컨테이너/피킹/합성/핫로드·실패 계약과 사용 예를 추가한다. 기존 본문·API ID는 보존한다.

빌드·테스트·브라우저 확인은 수행하지 않았다. 사용자는 index.html을 파일로 열고 검색 → API Reference → 조립 README 미리보기 → 표/코드 블록/문서 내 링크 → 원문 전환을 확인한다. 인터넷이 꺼진 상태에서도 문서를 읽을 수 있어야 한다. 원본 .md에 줄을 추가한 뒤 갱신 도구를 실행하고 미리보기에 반영되는지 확인한다.
