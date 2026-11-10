# 실행 구조와 소스 관계

graphify AST 산출물: 3607 nodes / 6942 edges. 코드 전용 추출이며 의미 분석 API 토큰 사용은 0이다. 빌드·테스트·진단기·앱·브라우저는 실행하지 않았다. 그래프는 정적 관계이고 동적 디스패치의 실제 실행 증거가 아니다.

## 현재 Standard

1. `standard/main.cpp::main`이 입력·초기화·갱신·렌더 콜백을 한 번 등록한 뒤 `Application::run`을 호출한다. 등록 시점과 콜백 실행 시점은 다르다.
2. run 초기화 → Console::initScene → defaultRecipe → Assembly::execute → resolveCommand → dispatch → applyPoses(0). 실습 번호 라우팅·FeatureContainer·GuideRecipes는 Markdown 참조 문서로 전환했다. Standard는 작업공간 소스만 컴파일한다.
3. 이벤트 루프 PollEvents → Console 입력 콜백 → Assembly 입력 → 피킹 또는 명령 실행. 단축키 미리보기는 작성 상태와 Scene 사본을 보관한다. Enter는 확정, Esc/포커스 이탈은 복원한다.
4. Console::onUpdate → 콘솔 입력을 최대8줄 처리 → Assembly::update → 시계 진행 → applyPoses. 기준 SRT·부착·지역 모션을 Scene에 전달한 뒤 힘을 적분하고 광원·카메라·축을 동기화한다.
5. 사용자 update 콜백 후 Application은 Scene::updateMotions를 한 번 실행한다. Console::onRender → Assembly::render → renderScene 또는 renderViews → Renderer. 렌더 패스에서는 시간을 진행하지 않는다.
6. Scene::worldMatrix는 객체의 부모 체인과 attachment/base/localLayers를 합성한다. faceWorldMatrix를 렌더·피킹이 함께 사용한다. Renderer는 CPU 정점/인덱스를 VAO/VBO/EBO로 업로드하고 모델·정규행렬·점광원 uniform을 전송한다. Shaders.h와 shaders/*.glsl이 GPU 입력·출력 계약이다.
7. 종료 요청 후 run의 정리 경로가 GPU/창/GLFW 자원을 해제한다. ConsoleInput 소멸자는 원래 Windows 콘솔 입력 모드를 복원한다. 소유 객체와 비소유 콜백 참조의 수명은 Application/Console 범위에 종속된다.

## 독립 실행 프로젝트와 main

각 행은 별도 실행 프로그램이다. 서로 다른 프로젝트의 Console/main을 하나의 호출 흐름으로 합치지 않는다. 부재 파일은 과거 기록이며 복원하지 않았다.

### openGL_2dAssign11/openGL_sample2.vcxproj

- `openGL_2dAssign11/main.cpp:109` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_2dAssign12/openGL_sample2.vcxproj

- `openGL_2dAssign12/main.cpp:59` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_2dAssign_10/openGL_sample2.vcxproj

- `openGL_2dAssign_10/main.cpp:86` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_2dAssign_789/openGL_sample2.vcxproj

- `openGL_2dAssign_789/main.cpp:103` · main · 조건부 컴파일 분기는 소스의 전처리 조건별로 구분한다.
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_13/openGL_sample2.vcxproj

- `openGL_3dAssign_13/main.cpp:4` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_14/openGL_sample2.vcxproj

- `openGL_3dAssign_14/main.cpp:4` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_15/openGL_sample2.vcxproj

- `openGL_3dAssign_15/main.cpp:5` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_16/openGL_assign_16.vcxproj

- `openGL_3dAssign_16/main.cpp:5` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_17/openGL_assign_17.vcxproj

- `openGL_3dAssign_17/main.cpp:4` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_18/openGL_assign_18.vcxproj

- `openGL_3dAssign_18/main.cpp:4` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_19/openGL_assign_19.vcxproj

- `openGL_3dAssign_19/main.cpp:4` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_20/openGL_assign_20.vcxproj

- `openGL_3dAssign_20/main.cpp:5` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_21/openGL_assign_21.vcxproj

- `openGL_3dAssign_21/main.cpp:5` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_22/openGL_assign_22.vcxproj

- `openGL_3dAssign_22/main.cpp:5` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_3dAssign_23/openGL_assign_23.vcxproj

- `openGL_3dAssign_23/main.cpp:5` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### openGL_assign123456/openGL_sample2.vcxproj

- `openGL_assign123456/main.cpp:79` · main · 조건부 컴파일 분기는 소스의 전처리 조건별로 구분한다.
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### standard/standard.vcxproj

- `standard/main.cpp:6` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### framework/examples/basic.cpp

- `framework/examples/basic.cpp:?` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### framework/examples/assets.cpp

- `framework/examples/assets.cpp:?` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

### framework/examples/motion.cpp

- `framework/examples/motion.cpp:?` · main
- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.

## 제한과 참조

과거 실습 소스도 AST에 남으므로 노드 존재만으로 Standard 실행 경로에 포함됐다고 판단하지 않는다. PROJECTS.json은 vcxproj의 명시 Compile 목록과 Exclude를 반영한다. 그래프의 같은 이름·서로 다른 실행 파일 사이 호출 후보는 AMBIGUOUS로 구분한다. 실제 오버로드·함수 포인터·가상 호출 전체를 증명하지 않는다. 해시 manifest는 생성 당시의 소스를 가리킨다.

[대화형 그래프](graph.html) · [파일·프로젝트](PROJECTS.json) · [소스 해시](SOURCE_MANIFEST.json)
