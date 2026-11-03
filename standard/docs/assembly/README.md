# 0번 기능 조립과 직접 코드 편집

2026-11-03. 명령은 배치·동작 실험을 위한 보조 수단이다. 구조와 동작의 상세 변경은 `guide <기능>`이 안내하는 C++ 함수를 직접 편집한다. 빌드·테스트는 사용자가 수행한다.

- [현재 줄·값 변경·디버깅 안내](EDIT_DEBUG_GUIDE.md)
- [1~22 기능 컨테이너·핫로드](FEATURE_CONTAINERS.md)
- [C++26 리플렉션 판단](REFLECTION_AND_HOTLOAD.md)
- [이번 가이드·컨테이너 작업 전체 변경 전후](guide-containers/BEFORE_AFTER.md)
- [전체 구조와 그래프](architecture/GRAPH_REPORT.md)
- [배포와 30MB 제한](PORTABLE_PACKAGE.md)
- [명세와 명령/API 계약](SPEC.md)
- [시작과 콘솔 사용](USER_GUIDE.md)
- [기능별 직접 편집 안내](CODE_GUIDE.md)
- [탱크·트럭·사람 패턴 조합](PATTERN_COMBINATIONS.md)
- [실습별 표현력과 반복 소스 검토](REQUIREMENT_AUDIT.md)
- [사용자 빌드·입력 확인](USER_CHECKS.md)
- [이번 작업 전체 변경 전후](BEFORE_AFTER.md)

0번은 공통 Scene에서 이름 있는 객체·관절·기능을 조립한다. 기능 컨테이너는 각자의 CPU Scene과 Console 상태에서 기존 실습을 실행하고 출력만 공통 Scene에 연결한다. 같은 Scene에 여러 초기화를 겹치지 않는다. 기본 객체 생성과 Motion/Animation/Collision/Pattern API는 공유한다.
