# 직접 C++ 작성과 source debugger 개편

- 날짜: 2026-11-10
- 승인 범위: 프레임워크 원본과 Standard 직접 작성 전환, 별도 debugger, 실제 소스의 경로·행·before/after 안내, 조명·재질·카메라와 문서·튜토리얼·예제 개편.
- 추가 승인: 기존 Standard를 old_standard로 이동하고 그 안의 x64/.vs만 삭제. 두 폴더가 없어 실제 삭제·회수 용량은 0바이트.
- 기존 사용자 Renderer.cpp 변경, docs.zip, PDF, 기존 pycache는 수정·삭제·커밋하지 않음.

| 단계 | 커밋 | 내용 |
|---|---|---|
| 1 | b6daa40 | SceneAuthoring 원본/배포 사본, 직접 C++ 작성 API |
| 2 | 14fefc4 | old_standard 보존, 새 Standard의 SceneSetup·콜백·컴파일 목록 |
| 3 | d0ac31a | debugger, 실제 소스 색인·수정안·합본/단독 솔루션 |
| 4 | 50d73c5 | 튜토리얼·예제·조명 문서·목차·생성기, 초기 부모 레이어 적용 |
| 5 | 최종 구조/보고 커밋 | graphify AST, 프로젝트 진입점, 전체 before/after·manifest |

문서 생성: python -B tools/Update-StandardDocs.py. 코드 작성·문서 생성·구조 자료 생성은 수행했고, 컴파일·테스트·검증기·앱 실행·화면 확인은 수행하지 않았다.

새 debugger는 지원 속성과 구획 표시를 사용한다. 일반 C++/GLSL은 find/code/replace/insert로 실제 행의 수정안을 안내한다. 임의 자연어·C++ 의미 분석·소스 자동 저장·자동 컴파일은 구현하지 않았다. 추가 개념은 지정된 Jun1188/ComputerGraphics의 해당 실제 소스를 참고한다.

별도 삭제할 임시파일을 만들지 않았다. graphify-out/source-debugger의 추출/캐시는 구조 자료와 재생성 근거로 보존한다. 삭제 후보로 사용자 자료를 임의 분류하지 않는다.
