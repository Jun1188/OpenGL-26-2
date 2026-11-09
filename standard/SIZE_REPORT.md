# standard 전체 폴더 용량

`python tools/Update-StandardDocs.py`가 실행할 때마다 다시 계산한다. 빌드 산출물 `x64/`와 Visual Studio 캐시 `.vs/`는 지워도 다시 만들어지므로 제외한다. 논리 파일 크기의 합이며 디스크 할당 크기나 ZIP 크기와 다르다.

- 파일 수: 758
- 전체: **26,599,756바이트** = 26.60 MB = 25.37 MiB
- 한도 30 MB(30,000,000바이트) 대비: **이하**, 남은 여유 3.40 MB

| 최상위 항목 | 바이트 |
|---|---:|
| docs | 15,417,144 |
| framework | 10,840,786 |
| modes | 181,279 |
| BEFORE_AFTER.md | 75,914 |
| examples | 47,592 |
| DOCUMENT_INDEX.md | 14,380 |
| standard.vcxproj | 4,288 |
| USER_CHECKS.md | 3,756 |
| README.md | 3,608 |
| SIZE_REPORT.md | 1,855 |
| standard.vcxproj.filters | 1,693 |
| index.html | 1,582 |
| main.cpp | 1,412 |
| Console.cpp | 1,254 |
| document.html | 889 |
| standard.sln | 827 |
| Console.h | 641 |
| PORTABLE.md | 515 |
| standard.slnx | 173 |
| standard.vcxproj.user | 168 |

## 큰 파일 10개

지우기 전에 다른 문서나 도구가 참조하는지 확인한다. 예: graph.json은 GRAPH_REPORT.md가, framework/ 아래는 Sync-Framework.ps1 -Standard가 루트 framework에서 다시 복사한다.

| 파일 | 바이트 |
|---|---:|
| docs/assembly/architecture/graph.json | 3,956,078 |
| docs/assembly/architecture/graph.html | 3,182,325 |
| docs/viewer/documents.js | 2,519,710 |
| docs/API_REFERENCE.html | 2,414,129 |
| framework/docs/API_REFERENCE.html | 2,378,692 |
| framework/dependencies/glew-2.3.1/include/GL/glew.h | 1,362,237 |
| docs/API_CASE_MAP.json | 870,589 |
| framework/docs/API_CASE_MAP.json | 870,589 |
| framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib | 770,808 |
| framework/docs/IMPLEMENTATION_PATTERNS.html | 676,577 |
