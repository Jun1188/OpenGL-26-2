# 0번 기능 조립과 직접 코드 편집

2026-11-03. 명령은 배치·동작 실험을 위한 보조 수단이다. 구조와 동작의 상세 변경은 `guide <기능>`이 안내하는 C++ 함수를 직접 편집한다. 빌드·테스트는 사용자가 수행한다.

- [명세와 명령/API 계약](SPEC.md)
- [시작과 콘솔 사용](USER_GUIDE.md)
- [기능별 직접 편집 안내](CODE_GUIDE.md)
- [탱크·트럭·사람 패턴 조합](PATTERN_COMBINATIONS.md)
- [실습별 표현력과 반복 소스 검토](REQUIREMENT_AUDIT.md)
- [사용자 빌드·입력 확인](USER_CHECKS.md)
- [이번 작업 전체 변경 전후](BEFORE_AFTER.md)

0번은 기존 실습 Console 여러 개를 동시에 초기화하지 않는다. 한 Scene에서 이름 있는 객체·관절·기능을 조립한다. 기본 객체 생성과 기존 Motion/Animation/Collision/Pattern API를 재사용한다. 기존 1~22 모드는 각각의 독립 상태를 유지한다.
