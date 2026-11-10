# 2차 코딩테스트: 직접 작성과 낯선 조건 구현

시험일: **2026-11-24 화요일**. 준비 기간은 11월 10~23일이다. 실제 시험 문제·시험 시간은 제공되지 않았다. 이 폴더는 실습 23까지의 변환·상태·패턴 응용과 추가 조명 26/27/28의 개념을 다루는 코딩 준비 자료다.

1. [오프라인 학습 HTML](index.html)을 연다. 전체 설명·코드·문제·힌트·해설이 포함된다. index.html과 assets 폴더를 함께 복사한다. 서버·CDN·인터넷을 요구하지 않는다.
2. [14회 준비 흐름](ROADMAP.md)을 따라 순정 작성과 Standard 실전을 번갈아 연습한다. 진도 체크는 사용자가 읽음/작성/실행 확인을 구분해서 기록한다.
3. 순정은 [native/native.sln](native/native.sln)이다. Framework.h 없이 GLFW·GLEW·GLM과 OpenGL을 직접 사용한다. 필요한 SDK는 native 내부에 있다. 빈 코드에서 다시 쓰는 연습은 main.cpp를 별도 보존한 뒤 사용자가 작업 복사본에서 한다.
4. Standard는 [../standard/standard.sln](../standard/standard.sln)이다. 장면은 SceneSetup.cpp, 상태·입력·갱신은 ExamController, GLSL은 framework/shaders에서 편집한다. debugger는 내부의 별도 선택 도구다.
5. [N제 12문항](exercises/README.md)과 [통합 6시나리오](scenarios/README.md)를 해설을 닫고 푼 뒤 비교한다. 숫자·색만 바꾼 재출제 예측이 아니라 서로 다른 상태·시간·기준 프레임 조합이다.

## 범위와 제출

여기서 이론코드 학습은 프레임워크 없이 버퍼·셰이더·입력·변환을 직접 작성하는 능력이다. 별도 채팅의 중간고사 필기 대비 및 나중에 그 채팅에서 요청할 기말고사 자료와는 별개다. 해당 자료를 이 폴더에서 변경하지 않았다.

학습 HTML·튜토리얼·N제·native 프로젝트는 Standard 제출 입력이 아니다. 기본 제출 패키지는 Standard에서 debugger, 프로젝트 x64/.vs, 문서·튜토리얼을 제외한다. SDK의 x64 라이브러리와 필수 DLL·라이선스·GLSL은 보존한다. 제출 크기 기준은 압축 전 **30,000,000바이트 이하**다. OS·Windows SDK·Visual Studio C++ 도구·호환 GPU 드라이버는 실행/개발 환경의 전제다. 상위/형제 폴더나 개인 절대 경로에 대한 프로젝트 의존성을 만들지 않는다.

## 작성·생성·검증 상태

소스와 학습 자료를 작성했다. HTML/문서/소스 구조/패키지 생성은 빌드 성공을 뜻하지 않는다. 빌드·테스트·검증기·앱/브라우저 실행·화면 판정은 사용자에게 맡긴다. 문항의 관찰 기준은 수행할 체크이며 확인된 실행 결과가 아니다.

원문은 learning/*.md, implementation/WORKFLOW.md, requirements/COVERAGE.md, exercises/problems.json, scenarios/scenarios.json이다. tools/Write-CodingTest2Guide.py는 원문을 읽어 HTML·문항별 Markdown·실제 소스 스냅샷을 생성하며 원문을 덮어쓰지 않는다. [출처](sources.md)와 [학습 관계](learning-graph.md), [사용자 확인](USER_CHECKS.md)을 함께 제공한다.
