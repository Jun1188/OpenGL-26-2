# 변경 기록

수정 범위는 저장소 루트 framework다. 다른 프로젝트의 framework와 프로젝트 설정을 변경하지 않았다. 기존 사용자 작업은 커밋에 섞지 않았다.

| 단계 | 커밋 | 변경 |
| --- | --- | --- |
| 1 | 2b6adc3 | 일반 메시 데이터와 부모·면 SRT |
| 2 | a0043a4 | Assimp 번들, 모델/텍스처 옵트인 로더 |
| 3 | 1a7bff9 | Scene 편집 API와 하단 기준점 |
| 4 | aaf1b12 | 일반 메시·이미지·경계 렌더링 |
| 5 | 6e0a386 | 클릭 피킹과 include 진입점 |
| 6 | b21619d | 크기 변경 시 메시 속성 보존·검증 보정 |
| 7 | 6535855 | 비유한 합성 변환·불완전 초기화 방어 |
| 8 | a194f47 | 예제·전체 전후 기록·정적 분석 자료 |

before는 작업 시작 기준 0e8389e다. after는 단계 1~7 구현과 최종 예제·공개 문서다. 원래 src의 전체 코드와 헤더로 옮긴 전체 코드를 함께 보여 준다.

- [직접 수정한 코드·예제·설정·README 전체](CODE_BEFORE_AFTER.md)
- [추가 Assimp SDK와 라이선스 전체](ASSIMP_BEFORE_AFTER.md)
- [바이너리 SHA-256과 정적 결과](STATIC_EVIDENCE.json)
- [사용자 빌드·실행 확인](USER_CHECKS.md)
- [framework 전용 AST 구조](../../framework-analysis/graphify-out/GRAPH_REPORT.md)
- [framework 전용 실행 흐름](../../framework-analysis/graphify-out/FRAMEWORK_FLOW.md)

새 코드의 before는 없음으로 표시했다. 문서 자체와 생성된 그래프의 내용은 각 파일에 제공하며, 자기 자신을 before/after에 재귀 포함하지 않는다. 바이너리는 텍스트 코드가 아니므로 해시·원본 바이트 일치로 기록한다. 중간 단계의 구체적인 변경은 위 커밋의 diff로 구분할 수 있다.

파이프라인 변화는 입력(Model/Mesh/UV) → Scene 검증·부모/면 변환 → VAO/VBO/EBO → 셰이더·삼각형/경계 → 프레임버퍼 순서다. 클릭은 화면 좌표 → 카메라 광선 → 같은 면 변환의 삼각형 교점 → Scene setter 순서다.

빌드·테스트는 수행하지 않았다. 사용자 확인 결과가 없으므로 실행 성공 또는 이해 완료로 기록하지 않는다.

전체 전후 스냅샷에는 원본의 줄 끝 공백도 보존한다. 이 복사된 코드의 whitespace 경고는 원본 보존에서 발생하며, 실행 검증 결과가 아니다. 직접 구현한 코드와 나머지 문서는 별도 diff --check로 확인한다.

추가 단계 9: 사용자 요청으로 분석 결과를 framework-analysis/graphify-out에 분리하고 framework 코드만 재분석했다. 이전 그래프와 전체 프로젝트 목록은 같은 별도 분석 폴더의 archive에 보관했다. CODE_BEFORE_AFTER와 ASSIMP_BEFORE_AFTER는 단계 1~8의 기록이며, 이번 폴더·문서 이동은 [별도 변경 전후](../../framework-analysis/CHANGES.md)에 기록한다. framework 실행 코드는 이번 단계에서 수정하지 않았다.
