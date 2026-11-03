# 6단계: 3D 실습의 세부 패턴 코드 모음

- before: 실습별 세부 동작을 코드 블록과 연결하는 이 문서 묶음은 없었다.
- after: standard/docs/practice-cookbook의 16개 MD와 SOURCE_INVENTORY.json.
- 실제 standard의 Console.h/Console.cpp/main.cpp는 바꾸지 않았다.
- 공유 helper 전문을 제공하고 각 실습의 private 상태·모든 함수·main 콜백을 같은 standard 폴더에서 구성하도록 작성했다.
- 외부 실습 include/개인 경로/외부 SDK 경로는 사용 코드에서 제거했다. 동봉 framework와 C++ 표준 라이브러리를 전제로 한다.
- 실습별 세부 116개, 공통 10묶음, 이전 3D 부록 10개를 구분한다.

[목차와 전체 결과](../../standard/docs/practice-cookbook/README.md) · [완결된 이웃 면 예제](../../standard/docs/practice-cookbook/ADJACENT_FACES.md) · [전체 소스 before/after](ALL_CODE_BEFORE_AFTER.md)

공통 helper는 원래 함수 본문을 재사용했다. 원문 PDF와 현재 소스 차이는 SOURCE_NOTES.md에서 분리했다. 기존 실습 23의 균등 벽 개방은 ea1ebb7로 복원했다.
재발 방지 사건 로그와 ponytail 규칙은 공유 저장소 f58aa97에 커밋·push했고 설치본에도 해당 규칙을 추가했다. 재발 방지 효과와 화면 동작은 미확인이다.

수행 범위: 원문 텍스트·C++ 소스·프로젝트 구성·include/경로·Git 변경의 정적 검토, 문서 생성, graphify 구조 갱신.
미실행: 빌드·테스트·검증기·앱·브라우저 실행. 사용자가 COMMON.md의 순서로 파일을 구성하고 각 실습의 입력·리셋·포커스·화면을 확인한다.
