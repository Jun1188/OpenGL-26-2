[브라우저 목차](index.html) · [변환 템플릿](docs/TRANSFORM_TEMPLATES.html) · [README](README.md)

# standard 문서 목차

분류와 순서는 docs/DOCUMENT_CATALOG.json에서 정한다. 이 파일과 index.html은 `python tools/Update-StandardDocs.py`가 함께 다시 만든다.

## 시작하기

0번 장면 작업공간의 입력·명령·좌표계. 실습은 소스 참조다.

- [0번 작업공간 시작](README.md)
- [두 물체로 이해하는 Standard](docs/WORKSPACE_TUTORIAL.html) — 실제 명령 → 좌표 계산 → 관찰 → 이유
- [튜토리얼 원문과 전체 복사 명령](docs/WORKSPACE_TUTORIAL.md)
- [튜토리얼 학습 순서·소스 근거](docs/tutorials/Standard-Graph/index.md)
- [연산별 변환 템플릿](docs/TRANSFORM_TEMPLATES.html)
- [요구 문장 → 변환·공간·API](docs/TRANSFORM_REFERENCE.md)
- [템플릿 파일 목록](docs/TRANSFORM_TEMPLATES.md)
- [명령어 [결과] - 인수 의미](docs/QUICK_COMMANDS.md)
- [월드 배치·상속·지역 레이어](docs/TRANSFORM_WORKSPACE.md)
- [광원·카메라·힘 API 연결](docs/APPLICATION_RECIPES.md)
- [사용자 빌드·동작 확인](USER_CHECKS.md)
- [Standard 폴더 사용](PORTABLE.md) — 필요한 Visual Studio 구성과 독립 폴더 배포

## 변환과 응용

재출제 시나리오 대신 연산·좌표계·시간·힘을 조합한다.

- [이전 시나리오 문서의 이동 안내](docs/EXAM_SCENARIOS.html) — 변환 조합 템플릿으로 통합
- [응용 장면 작성 순서](docs/EXAM_IMPLEMENTATION_ORDER.html) — 육면체·렌더링·계층 변환을 처음부터 구현하는 순서
- [응용 장면 작성 순서](docs/IMPLEMENTATION_PATTERNS.html) — 응용 패턴별 흐름도와 실제 코드

## 0번 기능 조립

짧은 명령과 객체별 지역 레이어로 작성·저장·확장한다.

- [0번 작업공간 문서 지도](docs/assembly/README.md) — 0번 모드 개요와 문서 지도
- [작업공간 사용 순서](docs/assembly/USER_GUIDE.md) — 시작, 계층 구성, 저장·재구성 따라 하기
- [명령과 좌표계 계약](docs/assembly/SPEC.md) — 모든 명령의 문법과 계약
- [C++ 확장 위치](docs/assembly/CODE_GUIDE.md) — 기능별로 고칠 C++ 파일과 확장 예제
- [편집과 오류 위치](docs/assembly/EDIT_DEBUG_GUIDE.md) — guide 출력으로 고치고 디버깅하는 순서
- [과거 실습 컨테이너 소스 참조](docs/assembly/FEATURE_CONTAINERS.md) — 과거 컨테이너 소스 참조; 현재 런타임 제외
- [객체와 변환 템플릿 조합](docs/assembly/PATTERN_COMBINATIONS.md) — 탱크·트럭·사람 조립 패턴
- [현재 지원 범위](docs/assembly/REQUIREMENT_AUDIT.md) — 실습별로 명령만으로 되는 것과 C++가 필요한 것
- [레시피 재읽기와 C++ 변경](docs/assembly/REFLECTION_AND_HOTLOAD.md) — 레시피 핫로드와 C++ 재빌드의 차이
- [조립 작업공간 사용자 확인](docs/assembly/USER_CHECKS.md) — 0번 모드 직접 확인 절차
- [30MB 이하 소스 프로젝트 배포](docs/assembly/PORTABLE_PACKAGE.md) — 30MB 이하 소스 패키지 정책

## 과거 실습 코드 참조

변환 구현을 찾는 참고 코드. 시험 유형이나 재출제 예상 목록이 아니다.

- [3D 실습 세부 요구사항 구현 패턴](docs/practice-cookbook/README.md) — 실습별 문서 목록과 읽는 방법
- [공통 구현: standard 안에서만 사용하는 보조 코드](docs/practice-cookbook/COMMON.md) — 모든 실습이 공유하는 공통 코드
- [실습 13: 면 선택과 표시](docs/practice-cookbook/P13.md)
- [실습 14: 색·자전·이동](docs/practice-cookbook/P14.md)
- [실습 15: 육면체 면의 개별·복합 동작](docs/practice-cookbook/P15.md)
- [실습 16: 사각뿔 펼침과 교환](docs/practice-cookbook/P16.md)
- [실습 17: 두 객체의 선택·변환·자리 교환](docs/practice-cookbook/P17.md)
- [실습 18: 행성·달·궤도 계층](docs/practice-cookbook/P18.md)
- [실습 19: 탱크 부품과 카메라](docs/practice-cookbook/P19.md)
- [실습 20: 컨베이어·줄무늬·상자](docs/practice-cookbook/P20.md)
- [실습 21: 방·공·상자·낙하](docs/practice-cookbook/P21.md)
- [실습 22: 탱크의 세 뷰포트](docs/practice-cookbook/P22.md)
- [실습 23: 로봇·장애물·점프·벽](docs/practice-cookbook/P23.md)
- [이웃한 면의 공유 모서리 회전과 자리 교환](docs/practice-cookbook/ADJACENT_FACES.md) — 이웃한 두 면 교환을 따로 조립하는 예
- [이전 3D 작업 부록](docs/practice-cookbook/EARLIER_3D.md) — 번호 없는 이전 3D 실습 부록
- [원문·현재 소스·문서 코드의 범위](docs/practice-cookbook/SOURCE_NOTES.md) — 요구사항과 현재 소스의 차이

## API 레퍼런스

framework와 standard의 공개 API, 렌더링 설정, 외부 라이브러리 명세.

- [공통 구현과 전체 API 사례 코드](docs/API_REFERENCE.html) — 전체 API와 사례 코드(최신 조립 API 포함)
- [공개 API 한글 목록](docs/API_KO.md) — framework 공개 API 한글 목록
- [응용 API 읽는 순서](docs/APPLICATION_API.md) — Application API 읽는 순서
- [그네·사람형·계층 SRT 빠른 참조](docs/ANIMATION_API.md) — 그네·사람형·선형보간 SRT 애니메이션
- [응용패턴·그네 통합 설명](docs/APPLICATION_PATTERNS.md) — 응용 패턴 사례
- [응용 패턴은 의미 단위 API의 조립이다](docs/PATTERN_COMPOSITION.md) — 패턴을 의미 단위 API로 조합
- [육면체와 면의 변환: 독립·계층과 회전 종류](docs/FACE_TRANSFORMS.md) — 육면체 면 변환과 중심·회전 순서
- [정점 입력과 셰이더 연결 편집](docs/RENDER_CONFIGURATION.md) — 정점 입력과 셰이더 설정
- [framework 사용 설명서](framework/README.md) — 동봉 framework 사용 설명
- [OBJ 모델 불러오기](framework/docs/OBJ_LOADING.md) — OBJ 모델 불러오기
- [GLFW API 명세](framework/docs/GLFW_API_REFERENCE.html) — GLFW 창·입력 API
- [GLEW API 명세](framework/docs/GLEW_API_REFERENCE.html) — GLEW 확장 로딩 API
- [GLM API 명세](framework/docs/GLM_API_REFERENCE.html) — GLM 수학 API

## 구조 분석

가이드·기능 컨테이너 사이의 의존 관계 그래프.

- [구조 분석](docs/assembly/architecture/README.md) — 그래프 읽는 법
- [실행 구조와 소스 관계](docs/assembly/architecture/GRAPH_REPORT.md) — 그래프 분석 보고서
- [구조 그래프 (대화형)](docs/assembly/architecture/graph.html) — 대화형 그래프(인터넷 필요)

## framework 동봉 사본

framework 폴더에 함께 들어 있는 문서 사본. 내용은 위 API 레퍼런스와 같은 계열이다.

- [공통 구현과 전체 API 사례 코드](framework/docs/API_REFERENCE.html)
- [공개 API 한글 목록](framework/docs/API_KO.md)
- [응용 API 읽는 순서](framework/docs/APPLICATION_API.md)
- [그네·사람형·계층 SRT 빠른 참조](framework/docs/ANIMATION_API.md)
- [응용패턴·그네 통합 설명](framework/docs/APPLICATION_PATTERNS.md)
- [응용 패턴은 의미 단위 API의 조립이다](framework/docs/PATTERN_COMPOSITION.md)
- [육면체와 면의 변환: 독립·계층과 회전 종류](framework/docs/FACE_TRANSFORMS.md)
- [정점 입력과 셰이더 연결 편집](framework/docs/RENDER_CONFIGURATION.md)
- [의미 단위 동작과 육면체 모드](framework/docs/MODE_ACTIONS.md)
- [시험 현장 구현 순서: 육면체·렌더링·계층 변환](framework/docs/EXAM_IMPLEMENTATION_ORDER.html)
- [응용패턴 통합: 흐름도와 실제 코드](framework/docs/IMPLEMENTATION_PATTERNS.html)
- [사용자 빌드·실행 확인 (framework 폴더)](framework/docs/USER_CHECKS.md)
- [사용자 빌드·실행 확인 (docs 폴더)](docs/USER_CHECKS.md) — framework 공통 빌드·실행 확인

## 변경 기록·문서 관리

작업 단계별 변경 전후 코드, 문서 뷰어 관리 방법, 보관 자료. 현재 사용법은 위 분류를 따른다.

- [모드 분리·입력·피킹 변경 전후](docs/input-modes/BEFORE_AFTER.md) — 모드 분리·입력·피킹 전체 변경 전후
- [0번 조립 API 전체 변경 전후](docs/assembly/BEFORE_AFTER.md) — 0번 조립 API 변경 전후
- [가이드·컨테이너 작업 전체 변경 전후](docs/assembly/guide-containers/BEFORE_AFTER.md) — 가이드·컨테이너 작업 변경 전후
- [API Reference·브라우저 문서 갱신 변경 전후](docs/DOCUMENT_UPDATE_BEFORE_AFTER.md) — API Reference·문서 갱신 변경 전후
- [착석·관절 조작 변경 전후 (보관)](BEFORE_AFTER.md) — 착석·관절 조작 변경 전후(2026-11-02 보관)
- [브라우저 문서 목차와 Markdown 미리보기](docs/DOCUMENT_VIEWER.md) — 목차 분류를 고치고 문서를 다시 만드는 방법
- [standard 전체 폴더 용량](SIZE_REPORT.md) — standard 폴더 용량 합계
- [문서 목차 (Markdown 판)](DOCUMENT_INDEX.md) — 이 목차의 Markdown 판

## 실습 소스 참조

실행 모드가 아닌 코드 레퍼런스

- [실습 코드 참조](docs/REFERENCE_PRACTICES.md)
- [과거 구현 참조: AssemblyFeatureInput.cpp](docs/reference/modes/AssemblyFeatureInput.cpp.md)
- [과거 구현 참조: AssemblyFeatures.cpp](docs/reference/modes/AssemblyFeatures.cpp.md)
- [과거 구현 참조: Editor.cpp](docs/reference/modes/Editor.cpp.md)
- [과거 구현 참조: FaceCommon.cpp](docs/reference/modes/FaceCommon.cpp.md)
- [과거 구현 참조: FaceCommon.h](docs/reference/modes/FaceCommon.h.md)
- [과거 구현 참조: FeatureContainer.cpp](docs/reference/modes/FeatureContainer.cpp.md)
- [과거 구현 참조: FeatureContainer.h](docs/reference/modes/FeatureContainer.h.md)
- [과거 구현 참조: GuideRecipes.cpp](docs/reference/modes/GuideRecipes.cpp.md)
- [과거 구현 참조: GuideRecipes.h](docs/reference/modes/GuideRecipes.h.md)
- [과거 구현 참조: Mode0Assembly.cpp](docs/reference/modes/Mode0Assembly.cpp.md)
- [과거 구현 참조: Mode10Practice13.cpp](docs/reference/modes/Mode10Practice13.cpp.md)
- [과거 구현 참조: Mode11Practice14.cpp](docs/reference/modes/Mode11Practice14.cpp.md)
- [과거 구현 참조: Mode12Practice15.cpp](docs/reference/modes/Mode12Practice15.cpp.md)
- [과거 구현 참조: Mode13Practice16.cpp](docs/reference/modes/Mode13Practice16.cpp.md)
- [과거 구현 참조: Mode14Practice17.cpp](docs/reference/modes/Mode14Practice17.cpp.md)
- [과거 구현 참조: Mode15Practice18.cpp](docs/reference/modes/Mode15Practice18.cpp.md)
- [과거 구현 참조: Mode16Practice19.cpp](docs/reference/modes/Mode16Practice19.cpp.md)
- [과거 구현 참조: Mode17Practice20.cpp](docs/reference/modes/Mode17Practice20.cpp.md)
- [과거 구현 참조: Mode18Practice21.cpp](docs/reference/modes/Mode18Practice21.cpp.md)
- [과거 구현 참조: Mode19Practice22.cpp](docs/reference/modes/Mode19Practice22.cpp.md)
- [과거 구현 참조: Mode1Ellipse.cpp](docs/reference/modes/Mode1Ellipse.cpp.md)
- [과거 구현 참조: Mode20Practice23.cpp](docs/reference/modes/Mode20Practice23.cpp.md)
- [과거 구현 참조: Mode21CameraCollision.cpp](docs/reference/modes/Mode21CameraCollision.cpp.md)
- [과거 구현 참조: Mode22EmbeddedMesh.cpp](docs/reference/modes/Mode22EmbeddedMesh.cpp.md)
- [과거 구현 참조: Mode2Rectangle.cpp](docs/reference/modes/Mode2Rectangle.cpp.md)
- [과거 구현 참조: Mode3Polygon.cpp](docs/reference/modes/Mode3Polygon.cpp.md)
- [과거 구현 참조: Mode4Spatial.cpp](docs/reference/modes/Mode4Spatial.cpp.md)
- [과거 구현 참조: Mode5Circle.cpp](docs/reference/modes/Mode5Circle.cpp.md)
- [과거 구현 참조: Mode6Polyline.cpp](docs/reference/modes/Mode6Polyline.cpp.md)
- [과거 구현 참조: Mode7Grid.cpp](docs/reference/modes/Mode7Grid.cpp.md)
- [과거 구현 참조: Mode8Room.cpp](docs/reference/modes/Mode8Room.cpp.md)
- [과거 구현 참조: Mode9Animation.cpp](docs/reference/modes/Mode9Animation.cpp.md)
- [과거 구현 참조: MotionCommon.cpp](docs/reference/modes/MotionCommon.cpp.md)
- [과거 구현 참조: Paths.h](docs/reference/modes/Paths.h.md)
- [과거 구현 참조: PracticeCommon.h](docs/reference/modes/PracticeCommon.h.md)
- [과거 구현 참조: PracticeMode.h](docs/reference/modes/PracticeMode.h.md)
- [과거 구현 참조: PracticeRegistry.cpp](docs/reference/modes/PracticeRegistry.cpp.md)
- [과거 구현 참조: TankCommon.cpp](docs/reference/modes/TankCommon.cpp.md)
- [과거 구현 참조: TankCommon.h](docs/reference/modes/TankCommon.h.md)

## 기타 문서

분류에 아직 넣지 않은 문서다. DOCUMENT_CATALOG.json에 추가하면 위 분류로 옮겨진다.

- [실습 모드 안내 변경](docs/MODE_ACTIONS.md)
- [실습 모드 안내 변경](docs/MODE_INPUT_AUDIT.md)
- [학습 순서](docs/tutorials/Standard-Graph/learning-graph.md)
- [작성과 확인 경계](docs/tutorials/Standard-Graph/review-log.md)
- [출처와 점검 범위](docs/tutorials/Standard-Graph/sources.md)
- [객체 부착·지역 레이어·힘 API](framework/docs/WORKSPACE_API.md)

## 흐름도

<!-- flowchart-index:start -->

- [프레임 루프·조건 분기](docs/flowcharts/frame-loop.svg)
- [엔티티 계층·모션 레이어](docs/flowcharts/entity-layers.svg)
- [그네·착석 사람 계층](docs/flowcharts/swing-hierarchy.svg)
- [렌더링 파이프라인](docs/flowcharts/render-pipeline.svg)

<!-- flowchart-index:end -->
