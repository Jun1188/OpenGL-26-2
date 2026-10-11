# 단계 9: framework 전용 graphify 분리

## 폴더

before: framework/graphify-out에 framework AST 그래프와 전체 저장소 프로젝트 목록이 함께 있었다.

after: framework-analysis/graphify-out에 framework 코드·예제 17개를 재분석한 전용 그래프를 생성했다. 기존 폴더 전체는 framework-analysis/archive/framework-graphify-out-a194f47로 이동했다. 저장소 루트의 기존 graphify-out은 수정하지 않았다. ARCHIVE_MANIFEST.json은 이전 커밋과 보관본을 대조한다.

## framework/README.md의 링크

before:
```markdown
- [코드 구조 분석](graphify-out/GRAPH_REPORT.md), [실행 흐름과 프로젝트 범위](graphify-out/EXECUTION_FLOW.md)
- [인터랙티브 코드 그래프](graphify-out/graph.html)
```

after:
```markdown
- [framework 전용 코드 구조 분석](../framework-analysis/graphify-out/GRAPH_REPORT.md), [framework 실행 흐름](../framework-analysis/graphify-out/FRAMEWORK_FLOW.md)
- [인터랙티브 코드 그래프](../framework-analysis/graphify-out/graph.html)
```

## framework/docs/CHANGES.md의 링크와 기록

before:
```markdown
- [framework AST 구조](../graphify-out/GRAPH_REPORT.md)
- [전체 실행 프로젝트의 개별 진입점](../graphify-out/EXECUTION_FLOW.md)
```

after:
```markdown
- [framework 전용 AST 구조](../../framework-analysis/graphify-out/GRAPH_REPORT.md)
- [framework 전용 실행 흐름](../../framework-analysis/graphify-out/FRAMEWORK_FLOW.md)
```

추가 문단의 before는 없음이며 after 전체는 다음과 같다.

```markdown
추가 단계 9: 사용자 요청으로 분석 결과를 framework-analysis/graphify-out에 분리하고 framework 코드만 재분석했다. 이전 그래프와 전체 프로젝트 목록은 같은 별도 분석 폴더의 archive에 보관했다. CODE_BEFORE_AFTER와 ASSIMP_BEFORE_AFTER는 단계 1~8의 기록이며, 이번 폴더·문서 이동은 [별도 변경 전후](../../framework-analysis/CHANGES.md)에 기록한다. framework 실행 코드는 이번 단계에서 수정하지 않았다.
```

8단계 표의 자기 참조는 실제 커밋 a194f47로 확정했다. 이전 CODE_BEFORE_AFTER 스냅샷은 당시의 내용을 그대로 유지한다.

## framework/.gitignore

before:
```text
!dependencies/assimp/bin/assimp-vc145-mt.dll
!dependencies/assimp/lib/assimp-vc145-mt.lib
/graphify-out/cache/
```

after:
```text
!dependencies/assimp/bin/assimp-vc145-mt.dll
!dependencies/assimp/lib/assimp-vc145-mt.lib
```

이전 캐시 경로를 제거했다. 새 framework-analysis/.gitignore는 해당 분석 폴더의 로컬 경로·캐시만 제외한다. 새 문서·분석 파일의 before는 없음이며 전체 내용은 각 파일에 있다. 보관된 옛 그래프는 내용 변경 없이 이동했다.

framework 실행 코드와 다른 프로젝트의 framework는 변경하지 않았다. graphify 설치 파일도 변경하지 않았다. 빌드·테스트는 수행하지 않았다.
