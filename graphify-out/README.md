# 구조 그래프 사용법

- graph.html: 서버 없이 브라우저에서 여는 대화형 그래프.
- GRAPH_REPORT.md: graphify 구조 묶음, 주요 연결, 불확실한 관계와 한계.
- EXECUTION_FLOW.md: 전체 실행 프로젝트와 각 실습의 콜백·CPU→GPU 흐름.
- GRAPH_HEALTH.md: 미해결 끝점과 관계 병합 진단.
- PROJECTS.json: 12개 프로젝트의 컴파일 항목과 15개 물리적 진입점.
- RELATIONS.json: 병합 전 노드·관계와 AMBIGUOUS 표시를 보존한 원시 추출.
- graph.json: 방향 그래프와 한국어 구조 묶음 이름.

전달하는 텍스트와 JSON은 UTF-8 BOM이다. Python에서는 다음처럼 읽는다.

```python
import json
from pathlib import Path

data = json.loads(Path("graphify-out/graph.json").read_text(encoding="utf-8-sig"))
```

현재 graphify CLI의 일부 로더는 encoding="utf-8"만 사용하므로 BOM JSON 읽기에
제한이 있다. graph.html은 이미 데이터를 포함하여 이 제한의 영향을 받지 않는다.
후속 구조 질의는 utf-8-sig로 JSON을 읽어 NetworkX에서 탐색하거나 소스와 함께 검토한다.
재생성할 때는 이 인코딩 조건을 반영하고 사용자 변경을 보존한다.

정적 그래프는 실제 실행을 증명하지 않는다. 미해결 끝점 270개, 관계 병합 93개,
자기 참조 3개가 있으며 프로젝트 간 기호 관계 16개는 AMBIGUOUS이다.
C++ 전처리 분기와 콜백의 실제 대상은 EXECUTION_FLOW.md에서 소스와 대조했다.
빌드와 테스트 실행은 사용자에게 맡긴다.
