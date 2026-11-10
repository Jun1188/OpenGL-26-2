# standard 전체 폴더 용량

`python tools/Update-StandardDocs.py`가 실행할 때마다 다시 계산한다. 빌드 산출물 `x64/`와 Visual Studio 캐시 `.vs/`는 지워도 다시 만들어지므로 제외한다. 논리 파일 크기의 합이며 디스크 할당 크기나 ZIP 크기와 다르다.

- 파일 수: 557
- 전체: **11,205,011바이트** = 11.21 MB = 10.69 MiB
- 한도 30 MB(30,000,000바이트) 대비: **이하**, 남은 여유 18.79 MB

| 최상위 항목 | 바이트 |
|---|---:|
| framework | 10,822,845 |
| docs | 358,093 |
| SceneSetup.cpp | 3,057 |
| USER_CHECKS.md | 3,026 |
| examples | 2,874 |
| DOCUMENT_INDEX.md | 2,654 |
| standard.vcxproj | 2,467 |
| SIZE_REPORT.md | 1,831 |
| README.md | 1,694 |
| index.html | 1,590 |
| Console.cpp | 1,394 |
| main.cpp | 1,061 |
| document.html | 889 |
| standard.sln | 845 |
| Console.h | 467 |
| SceneSetup.h | 224 |

## 큰 파일 10개

지우기 전에 다른 문서나 도구가 참조하는지 확인한다. 예: graph.json은 GRAPH_REPORT.md가, framework/ 아래는 Sync-Framework.ps1 -Standard가 루트 framework에서 다시 복사한다.

| 파일 | 바이트 |
|---|---:|
| framework/docs/API_REFERENCE.html | 2,376,807 |
| framework/dependencies/glew-2.3.1/include/GL/glew.h | 1,362,237 |
| framework/docs/API_CASE_MAP.json | 870,589 |
| framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib | 770,808 |
| framework/docs/IMPLEMENTATION_PATTERNS.html | 690,861 |
| framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll | 472,576 |
| docs/viewer/documents.js | 269,772 |
| framework/dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h | 248,376 |
| framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll | 232,448 |
| framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/vcruntime140.dll | 178,616 |
