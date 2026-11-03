# standard 전체 폴더 용량

`python tools/Update-StandardDocs.py`가 실행할 때마다 다시 계산한다. 빌드 산출물 `x64/`와 Visual Studio 캐시 `.vs/`는 지워도 다시 만들어지므로 제외한다. 논리 파일 크기의 합이며 디스크 할당 크기나 ZIP 크기와 다르다.

- 파일 수: 708
- 전체: **28,313,465바이트** = 28.31 MB = 27.00 MiB
- 한도 30 MB(30,000,000바이트) 대비: **이하**, 남은 여유 1.69 MB

| 최상위 항목 | 바이트 |
|---|---:|
| docs | 16,681,710 |
| framework | 11,119,453 |
| modes | 353,086 |
| BEFORE_AFTER.md | 75,914 |
| examples | 33,148 |
| Console.cpp | 9,758 |
| DOCUMENT_INDEX.md | 9,408 |
| README.md | 8,707 |
| USER_CHECKS.md | 5,978 |
| Console.h | 3,754 |
| standard.vcxproj | 3,403 |
| SIZE_REPORT.md | 1,796 |
| main.cpp | 1,762 |
| PORTABLE.md | 1,597 |
| index.html | 1,557 |
| document.html | 889 |
| standard.sln | 845 |
| standard.vcxproj.filters | 521 |
| standard.slnx | 179 |

## 큰 파일 10개

지우기 전에 다른 문서나 도구가 참조하는지 확인한다. 예: graph.json은 GRAPH_REPORT.md가, framework/ 아래는 Sync-Framework.ps1 -Standard가 루트 framework에서 다시 복사한다.

| 파일 | 바이트 |
|---|---:|
| docs/assembly/architecture/graph.json | 4,396,326 |
| docs/assembly/architecture/graph.html | 3,550,496 |
| docs/API_REFERENCE.html | 2,479,939 |
| framework/docs/API_REFERENCE.html | 2,442,005 |
| docs/viewer/documents.js | 2,245,886 |
| framework/dependencies/glew-2.3.1/include/GL/glew.h | 1,362,237 |
| framework/docs/IMPLEMENTATION_PATTERNS.html | 897,775 |
| docs/IMPLEMENTATION_PATTERNS.html | 897,737 |
| docs/API_CASE_MAP.json | 883,991 |
| framework/docs/API_CASE_MAP.json | 883,991 |
