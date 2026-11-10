# Standard 제출 입력 용량

Package-Standard.py와 같은 정책으로 계산한다. 논리 파일 크기의 합이며 ZIP 크기·디스크 할당 크기와 다르다. 문서 자체는 제출에 포함하지 않는다.

- 제출 입력: **6,378,808바이트** (6.38 MB), 497개
- 기준: 30,000,000바이트 이하 / 현재 이하
- 개발 사본(문서·debugger 포함, 빌드/IDE 제외): 11,306,780바이트
- 제외: debugger, 프로젝트 x64/.vs, docs, 튜토리얼/학습 HTML, 예제
- 보존: C++/프로젝트/props/GLSL, bundled SDK 헤더·라이브러리·DLL·라이선스. SDK의 x64 폴더는 빌드 산출물이 아니다.
- 빌드·테스트·앱 실행·재배치 확인은 사용자 수행. 이 보고서는 실행 검증이 아니다.

| 제출 최상위 항목 | 바이트 |
|---|---:|
| Console.cpp | 1,633 |
| Console.h | 896 |
| ExamController.cpp | 7,124 |
| ExamController.h | 1,294 |
| SceneSetup.cpp | 3,057 |
| SceneSetup.h | 224 |
| framework | 6,359,855 |
| main.cpp | 1,374 |
| standard.sln | 845 |
| standard.vcxproj | 2,506 |
