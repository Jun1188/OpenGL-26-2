# openGL_2dAssign_789 학습·구현 로그

- 기록일: 2026-09-27 KST
- 대화 범위: 2026-09-26~27, 실습 7부터의 구현 요청과 후속 코드 질문
- 근거: 현재 `Common.*`, `DiagramState.*`, `Console.*`, `main.cpp`, 두 GLSL 파일의 정적 확인 및 대화에서 실제로 나온 질문
- 검증 상태: 빌드·실행 테스트 없음. 아래의 이해 여부도 사용자의 직접 설명으로 확인되지 않은 항목은 `확인 필요`로 둔다.

## 1. 최초 요청과 현재 코드 범위

| 사용자 요청·수정 범위 | 현재 코드에서 확인한 결과 |
|---|---|
| 실습 7부터 셰이더 로드·컴파일·첨부·링크와 프로그램 사용 | `Common.cpp`의 `compileShader()`, `ShaderHandler::initShader()` 및 `render()`에 구현 |
| `vPos`, `vColor` 위치 조회, VAO·VBO·EBO 초기화 | `ShaderHandler::initBuffer()`에서 `glGetAttribLocation()`, `glGenVertexArrays()`, `glGenBuffers()` 사용 |
| `glBufferSubData()`로 데이터 교체 | `ShaderHandler::upload()`에서 위치·색상 VBO를 교체하고 선택 도형이 있으면 EBO를 교체 |
| 이전 명칭·클래스·함수 호출을 현재 셰이더에 맞춤 | `Console → PracticeConsole → Practice7/8/9Console`, `DiagramStore`, `Diagram`, `ShaderHandler` 구조를 현재 코드에서 확인 |
| Practice 7 SRT 결과가 NDC `[-1,1]` 밖으로 나가지 않도록 함 | `Diagram::fitsNdc()`가 변환 후 모든 정점의 x·y를 검사. 추가·이동·크기·회전 후보에서 사용 |
| 변경 내용을 before/after로 보여 달라는 후속 요청 | 과거 답변의 요청 사항으로 기록. 이 로그는 현재 구조와 질문·답을 정리하며, 과거 diff를 재구성해 단정하지 않는다. |

`main.cpp`의 현재 `PROB`는 `7`이다. Practice 8/9 클래스는 존재하지만 현재 실행 분기는 Practice 7이다. `Diagram`의 크기·회전 함수는 구현돼 있으나 Practice 7 키 입력에는 연결되지 않았다. 이 기록은 실습 PDF의 지시보다 현재 코드와 실제 사용자 요청을 우선한다.

## 2. 현재 코드의 책임과 실행 경로

| 소스 | 주요 책임 |
|---|---|
| `main.cpp` | GLFW/GLEW 초기화, 콘솔 선택, 입력 콜백, `update → render → glfwSwapBuffers → glfwPollEvents` 루프 |
| `Console.h/.cpp` | 실습별 입력, `DiagramStore`와 `ShaderHandler` 소유, revision 비교 후 업로드·그리기 |
| `DiagramState.h/.cpp` | 도형 목록, 선택 인덱스, 상태 변경 번호(`revision_`) 관리 |
| `Common.h/.cpp`의 `Diagram` | 로컬 정점·색·pivot·scale·rotation 보유, 월드 정점 계산·NDC 검사·클릭 판정 |
| `Common.h/.cpp`의 `ShaderHandler` | 셰이더 프로그램·VAO·위치 VBO·색상 VBO·선택 도형용 EBO 관리, 업로드·draw call |
| `VertexShader.glsl` / `FragmentShader.glsl` | 이미 변환된 `vPos`를 `gl_Position`에 사용 / 원래 색 또는 uniform의 외곽선 색 선택 |

### 실제 렌더링 순서

1. 입력 콜백에서 `Practice7Console`이 `DiagramStore`를 변경한다. 성공한 변경은 `revision_`을 올린다.
2. 다음 `PracticeConsole::render()`는 색 버퍼를 지운다. `uploadedRevision_ != revision_`이면 `ShaderHandler::upload()`를 호출한다.
3. `upload()`는 각 `Diagram::worldVertices()`를 호출한다. CPU에서 로컬 좌표를 변환하여 위치·색상 배열과 도형별 `DrawRange {mode, first, count}`를 구성한다.
4. 위치 VBO와 색상 VBO에 `glBufferSubData()`를 호출한다. 선택 도형이 있으면 상대 인덱스를 EBO에도 올린다. 업로드가 성공하면 `uploadedRevision_ = revision_`이다.
5. `ShaderHandler::render()`는 셰이더 프로그램과 VAO를 바인딩한다. 비선택 도형은 `glDrawArrays()`, 선택 도형 본체는 `glDrawElementsBaseVertex()`를 사용한다.
6. 모든 본체를 그린 뒤 선택 도형이 있으면 흰색 외곽선을 추가로 그린다. 선 도형은 흰색 굵은 선 위에 원래 색의 가는 선을 한 번 더 그린다.
7. 정점 셰이더 → 기본도형 조립 → 클리핑·뷰포트 변환 → 래스터화 → 프래그먼트 셰이더 → 프레임버퍼 기록 후 `glfwSwapBuffers()`가 표시한다. 이 코드에서 별도의 뷰·투영 행렬은 사용하지 않는다.

## 3. 사용자 질문(query) → 답변·현재 이해 상태

### Q1. 선언·소유·상속 관계와 실제 렌더링 파이프라인은?

- 답: `PracticeConsole`은 `Console`을 상속하고 Practice 7/8/9 클래스가 이를 상속한다. `PracticeConsole`은 `DiagramStore`와 `ShaderHandler`를 멤버로 소유한다. `DiagramStore`는 `Diagram` 목록을 소유한다. CPU 상태·변환·업로드와 GPU draw·셰이더·화면 표시를 구분해 설명했다.
- 확인: 그래프를 요청했고 이후 각 단계에 관한 질문을 계속했다. 전체 흐름을 스스로 재구성했는지는 확인 필요.

### Q2. `revision_`은 무엇이며 `uploadedRevision_`과 어떻게 비교·대입하나? 계속 증가하면 넘치거나 비효율적인가?

- 답: `revision_`은 저장소의 상태 변경 번호이며 성공한 추가·이동·선택 변경 등에서 증가한다. `uploadedRevision_`은 마지막으로 GPU 업로드가 성공한 번호다. 처음에는 각각 `0`, `uint64_t` 최댓값이라 첫 프레임에 업로드한다. 다르면 업로드하고 성공 후 현재 번호를 대입한다. 같으면 업로드를 건너뛰지만 draw는 매 프레임 수행한다. 업로드 실패 시 대입 전에 `render()`가 실패를 반환한다.
- 정확성·비용: 부호 없는 64비트 값은 최댓값 다음에 0으로 순환한다. `!=` 비교는 일반적인 한 번의 순환에서 변화를 감지한다. 현실적인 변경 횟수에서 한 바퀴는 문제되지 않는다. 현재 최적화 여지는 선택 변경에도 전체 위치·색상 VBO를 재구성·업로드한다는 점이며, 화면의 매 프레임 재그리기와는 별개다.
- 확인: 사용자가 넘침과 전체 재렌더 비용을 지적했다. 두 작업의 구분을 스스로 설명하는지는 확인 필요.

### Q3. `glGenVertexArrays(1, &vao_)`, `glGenBuffers(1, &ebo_)`의 `1`과 VAO/VBO/EBO 역할은?

- 답: `1`은 생성할 객체 이름의 개수다. VAO는 `glGenVertexArrays()`로, 버퍼는 `glGenBuffers()`로 생성한다. VAO는 정점 속성 설정과 EBO 바인딩 상태를 보관하고, 위치·색상·인덱스 데이터는 VBO/EBO에 둔다.
- 확인: 대상별 차이를 다시 설명할 수 있는지는 확인 필요.

### Q4. `upload()`에서 `glBufferSubData()`가 VAO·VBO·EBO 순으로 네 번 호출되나?

- 답: 아니다. 호출은 위치 VBO, 색상 VBO에 각 1번으로 기본 2번이다. 선택 도형이 있으면 EBO에 1번 더 호출해 총 3번이다. VAO에는 `glBufferSubData()`를 호출하지 않는다. `glBindBuffer(target, id)` 뒤의 `glBufferSubData(target, ...)`가 해당 target에 바인딩된 버퍼 내용을 교체한다. 초기 `glBufferData()`는 공간 할당이다.
- 확인: 사용자가 호출 대상과 횟수를 직접 질문했다. 이후 코드에서 구분하는지는 확인 필요.

### Q5. `worldVertices()` 공식과 `pivot`·`localPivot`은 무엇을 뜻하나?

- 답: 각 로컬 정점 `p`에 `p_world = pivot + R × S × (p - localPivot)`을 적용한다. `localPivot`은 로컬 회전·확대 중심, `pivot`은 그 중심의 최종 위치다. GLM 코드는 `T(pivot) × R × S × T(-localPivot)` 행렬을 만들고 각 정점에 오른쪽부터 적용한다. 원본 로컬 정점은 변경하지 않는다. Practice 7 도형은 기본 `localPivot=(0,0)`을 사용한다. 결과 좌표는 NDC 가드·선택 판정·VBO 업로드에 쓰인다.
- 확인: 수식이 의미하는 바를 사용자에게 설명했으나 사용자가 수치 예제를 스스로 계산했는지는 확인 필요.

### Q6. EBO 인덱스 순서, 외곽선 바이트 오프셋, `ranges`의 관계는?

- 답: Practice 7 사각형의 로컬 번호는 `0=좌하, 1=우하, 2=좌상, 3=우상`이다. 선택 EBO는 `[0,1,2,3 | 0,1,3,2]`를 담는다. 앞 절반은 본체 `GL_TRIANGLE_STRIP`, 뒤 절반은 흰색 외곽선 `GL_LINE_LOOP`용이다. `selected.count * sizeof(GLuint)`는 뒤 절반에서 읽기 위한 EBO 바이트 오프셋이다. `range.first`는 EBO의 상대 인덱스에 더하는 전체 VBO의 시작 정점 번호다.
- 구분: `DrawRange`는 모드·시작 정점·개수를 담고, EBO는 실제 방문 인덱스 순서를 담는다. 선택 도형의 모양·정점 수와 선택 인덱스가 EBO 구성을 결정하며 `ranges`만으로 결정되지는 않는다.
- 확인: 사용자가 바이트 오프셋과 순서의 기준을 질문했다. 실제 숫자로 추적하는지는 확인 필요.

### Q7. EBO 뒤 절반을 쓰면 앞의 `0,1,2,3` 사각형이 덮어써지나? 본체에는 `1→2` 대각선이 생기지 않나?

- 답: EBO 앞·뒤 구간은 동시에 존재한다. 본체 draw call은 앞 구간, 외곽선 draw call은 뒤 구간을 읽는다. 앞 구간의 `GL_TRIANGLE_STRIP`은 두 삼각형을 **채우는** 모드이며 `1–2`는 내부 공유 경계이지 별도로 그리는 선이 아니다. 뒤 구간은 `GL_LINE_LOOP`이 `0→1→3→2→0`을 선분으로 연결한다.
- 확인: 사용자가 본체 인덱스로 내부 대각선을 예상했다. draw mode별 결과를 스스로 예측하는지는 확인 필요.

### Q8. `selectedOutlineMode_`는 마우스 선택 효과인가? 선택하지 않은 도형은 어떻게 그리나?

- 답: 선택 여부는 `selectedIndex_`가 결정한다. `selectedOutlineMode_`는 선택 도형의 외곽선을 `GL_LINES`(선) 또는 `GL_LINE_LOOP`(그 외) 중 어떻게 조립할지 정한다. 선택하지 않은 도형은 `glDrawArrays(range.mode, range.first, range.count)`로 원래 색의 본체를 그리며 외곽선은 추가하지 않는다.
- 확인: 사용자가 선택 효과와 비선택 경로를 연속 질문했다. 둘의 책임 구분은 확인 필요.

### Q9. 선택하지 않은 도형도 EBO를 쓰면 정점 중복을 줄일 수 있지 않나?

- 답: 설계상 가능하지만 현재의 선 2개, 삼각형 3개, 사각형·점 표시용 사각형 4개에는 본체를 그리기 위한 중복 VBO 정점이 없다. 사각형은 `GL_TRIANGLE_STRIP`이 4개 정점으로 두 삼각형을 만든다. 선택 도형의 EBO는 같은 네 VBO 정점을 본체와 다른 외곽선 순서로 재사용하기 위해 유용하다. 전부 EBO로 통일하려면 현재 8개짜리 선택 EBO 대신 모든 도형의 인덱스 범위·용량을 관리해야 한다. 복잡한 공유 정점 메시라면 통일의 이점이 커질 수 있다.
- 확인: 사용자가 대체 데이터 구조를 제안했다. 이 설계 판단을 스스로 적용할 수 있는지는 확인 필요.

### Q10. 마우스 선택 사각형을 실제로 어떤 순서로 그리나?

- 답: 클릭 좌표를 변환하고 맨 나중에 그려진 도형부터 hit test한다. 선택 변경 시 `revision_` 증가 → 다음 렌더에서 전체 VBO와 선택 EBO 업로드 → 모든 도형 본체 그리기. 선택 사각형 본체는 EBO 앞 절반과 `GL_TRIANGLE_STRIP`으로 원래 색을 채운다. 모든 본체 후 EBO 뒤 절반과 `GL_LINE_LOOP`으로 흰색 외곽선을 그린다. 사각형 본체는 한 번만 채운다.
- 확인: 사용자가 전체 경로 리뷰를 요청했다. 실행 결과를 직접 확인했는지는 미확인.

### Q11. `glDrawElementsBaseVertex()`는 무엇인가?

- 답: `mode, count, type, indices, basevertex`를 받는다. `indices`는 EBO의 읽기 시작 **바이트 오프셋**이고 `basevertex`는 각 EBO 인덱스에 더할 **VBO 정점 번호**다. 현재 코드는 `range.first`를 base vertex로 쓴다. EBO 값을 실제로 수정하지 않는다.
- 확인: 인덱스 위치·값·base vertex의 세 단위를 구별하는지는 확인 필요.

### Q12. 선택된 선의 원래 색을 다시 그리는 블록은 왜 있나?

- 답: 선택된 선 본체를 원래 색·두께 2로 그린 후 흰색·두께 5로 덮고, `uUseOverrideColor=false`로 바꿔 같은 선을 원래 색·두께 2로 다시 그린다. 선의 중앙 색을 복원하고 양옆에 흰 선택 표시를 남기려는 의도다. 사각형·삼각형은 채워진 내부와 외곽선이 구별되므로 이 추가 선 draw call이 없다.
- 확인: 사용자 질문에 실행 순서로 답했으나 실제 화면 검증은 없다.

### Q13. `glUniform1i`, `glUniform3f`와 외곽선 색은 어떻게 매핑되나?

- 답: 셰이더 프로그램 링크 후 `glGetUniformLocation(program, "uUseOverrideColor")`의 반환 위치를 `useOverrideLocation_`에, `"uOverrideColor"`의 반환 위치를 `overrideColorLocation_`에 저장한다. `glUniform1i(위치, GL_TRUE)`는 GLSL `bool`에 정수값 1개를, `glUniform3f(위치, 1,1,1)`는 GLSL `vec3`에 실수값 3개를 준다. 프래그먼트 셰이더가 덮어쓰기 여부에 따라 원래 `outColor` 또는 지정 색을 고른다. 외곽선 색은 두 번째 호출의 RGB를 바꾸면 된다.
- 확인: 사용자가 `1i`·`3f`와 uniform 변수 연결을 직접 질문했다. 위치 조회부터 셰이더 사용까지 스스로 추적하는지는 확인 필요.

## 4. 다음 학습에서 확인할 지점

- 사각형 정점 번호 `0,1,2,3`을 두고 `GL_TRIANGLE_STRIP`과 `GL_LINE_LOOP` 각각의 결과를 사용자가 직접 그려 볼 것.
- 앞 도형에 정점 3개가 있을 때 선택 사각형의 `range.first`, EBO 바이트 오프셋, 최종 VBO 정점 번호를 각각 계산해 볼 것.
- 선택 변경만 있는 프레임에 `revision_`, `uploadedRevision_`, VBO/EBO 업로드, draw call이 어떻게 되는지 순서대로 설명해 볼 것.
- `glGetUniformLocation()`의 반환 위치와 `glUniform1i`·`glUniform3f`가 업데이트하는 GLSL 변수를 연결해 볼 것.
- 빌드·실행 결과나 사용자의 이해 완료는 이 기록만으로 확정하지 않는다. 이후 직접 테스트·설명을 확인하면 로그를 갱신한다.
