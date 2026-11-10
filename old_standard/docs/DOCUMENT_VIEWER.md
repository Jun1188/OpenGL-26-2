# 브라우저 문서 목차와 Markdown 미리보기

`standard/index.html`을 브라우저에서 파일로 바로 연다. 서버나 인터넷이 없어도 된다. 대화형 구조 그래프(graph.html)만 외부 스크립트를 읽는다.

## 화면 구성

- **목차(index.html)**: 문서를 분류별 카드로 보여 준다. 맨 위 강조 카드, 분류 바로가기, 제목·설명·경로 검색, Markdown/HTML 필터가 있다. 사본과 변경 기록 분류는 접혀 있고, 검색하면 펼쳐진다.
- **Markdown 미리보기(document.html?doc=경로)**: 왼쪽에 문서 목차, 오른쪽에 본문을 보여 준다. 상단 막대에서 전체 목차, 문서가 속한 분류, 원본 파일로 이동하고 원문 보기로 전환한다. 문서 안의 저장소 상대 링크는 미리보기 링크로 바뀐다. Markdown의 raw HTML은 실행하지 않고 텍스트로 표시한다.
- **HTML 문서**: API Reference 같은 독립 HTML 문서 맨 위에도 같은 색의 막대(전체 문서 · 분류 · 경로)를 넣는다.
- **시험 시나리오(docs/EXAM_SCENARIOS.html)**: `examples/assembly/scenarios/*.assembly` 레시피의 `# @` 머리말로 카드를 만든다.

## 분류를 고치는 방법

분류와 순서, 목차에 보일 제목과 한 줄 설명은 `docs/DOCUMENT_CATALOG.json` 한 곳에서 정한다.

```json
{ "path": "docs/assembly/SPEC.md", "title": "생략하면 문서 첫 제목", "summary": "한 줄 설명", "featured": false }
```

카탈로그에 없는 새 문서는 자동으로 **기타 문서** 분류에 들어간다. 카탈로그에 적었는데 파일이 없거나, 같은 문서를 두 번 적으면 갱신 도구가 오류로 멈춘다.

## 갱신

소스 문서나 카탈로그를 고친 뒤 저장소 루트에서 실행한다.

```bash
python tools/Update-StandardDocs.py
```

이 도구가 함께 다시 만드는 파일:

| 파일 | 내용 |
|---|---|
| `docs/viewer/documents.js` | 모든 문서 원문과 카탈로그 데이터 |
| `docs/viewer/scenarios.js` | 시나리오 레시피 데이터 |
| `index.html`, `document.html`, `docs/viewer/*.js`, `docs/viewer/style.css` | `tools/templates/standard-viewer/`의 템플릿 사본 |
| `DOCUMENT_INDEX.md` | 카탈로그의 Markdown 판(흐름도 목록 블록은 보존) |
| 독립 HTML 문서의 상단 막대 | `<!-- standard-nav:start -->` 표식 사이만 교체 |
| API Reference의 최신 조립 API 절 | `<!-- current-assembly-api:start -->` 표식 사이만 교체 |

생성된 파일을 직접 고치지 않는다. 화면 모양은 템플릿을, 분류는 카탈로그를, 시나리오는 레시피 파일을 고친다. Marked 16.4.2는 `docs/viewer/vendor`에 라이선스와 함께 들어 있다.
