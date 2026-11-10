# 순정 OpenGL: 빈 main에서 화면까지

목표는 Framework.h 없이 GLFW·GLEW·GLM과 직접 OpenGL 호출로 시작하는 것이다. GLFW는 창/입력/컨텍스트, GLEW는 GL 함수 로딩, GLM은 수학 계산이다. 이 라이브러리를 사용해도 프로젝트 자체의 Scene/Renderer/SceneAuthoring를 사용하지 않으면 이 자료에서 말하는 순정 코드 경로다. 중간고사 개념·계산 문제 정리와 기말고사 준비는 별도 범위다.

## 1. 순서를 외우고 이유로 복원하기

```text
glfwInit → core 3.3 hints → glfwCreateWindow → glfwMakeContextCurrent
→ glewInit → shader compile/link → VAO/VBO 생성 → callback 등록
→ poll → 상태 갱신 → clear → glUseProgram → uniform → VAO → draw → swap
→ GPU 자원 삭제 → window 삭제 → glfwTerminate
```

OpenGL 함수 주소를 로딩할 때는 컨텍스트가 먼저 있어야 한다. VAO/VBO는 생성만으로 화면을 만들지 않는다. 셰이더, 입력 형식, 실제 draw 호출까지 연결되어야 한다. 제공한 native/main.cpp의 main, program, upload, draw 함수가 위 책임을 나누지만, 직접 GL 호출은 모두 그 파일 안에 있다.

## 2. CPU 구조체가 GPU 입력으로 가는 과정

```cpp
struct Vertex { glm::vec3 position, normal; };
glBindVertexArray(vao);
glBindBuffer(GL_ARRAY_BUFFER, vbo);
glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size()*sizeof(Vertex)), vertices.data(), GL_STATIC_DRAW);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex,position)));
glEnableVertexAttribArray(0);
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex,normal)));
glEnableVertexAttribArray(1);
```

0번 입력은 위치, 1번은 법선이다. 이 계약은 vertex GLSL의 layout(location=0/1)과 같아야 한다. stride는 다음 정점까지의 바이트 간격이고 offset은 해당 멤버의 시작 바이트다. sizeof/offsetof를 쓰므로 vec3 정렬 옵션을 바꾸어도 숫자 24/12를 임의로 고정하지 않는다. glVertexAttribPointer를 호출할 때 바인딩한 VBO와 입력 형식이 VAO에 연결된다. VBO를 나중에 바인딩했다고 기존 VAO의 입력 연결이 바뀌는 것은 아니다.

## 3. 인덱스가 있을 때

위치4개로 사각형을 만들려면 삼각형 인덱스6개를 쓴다. EBO 연결은 VAO 상태이므로 그 VAO가 바인딩된 동안 설정한다.

```cpp
const unsigned indices[] = {0,1,2, 0,2,3};
glBindVertexArray(vao);
glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
// 그리기 시 shader, uniform 설정 후:
glBindVertexArray(vao);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
```

drawArrays의 count는 정점 수, drawElements의 count는 인덱스 수다. 육면체에서 모서리를 공유해도 면 법선이 다르면 정점 입력을 나눠야 한다. native 예제는 각 면의 법선을 명확히 하기 위해 삼각형 정점을 반복한 drawArrays 방식이다. EBO를 쓴다는 사실 자체가 항상 더 간단한 것은 아니다.

## 4. 빈 화면을 좁히는 순서

1. 컴파일/링크 로그와 실제 GLSL 경로를 읽는다.
2. fragment 출력을 임시 상수색으로 바꿔 조명 계산을 분리한다.
3. 카메라 앞에 있는 작은 원점 도형인지, MVP 순서와 near/far가 맞는지 확인한다.
4. VAO location/stride/offset과 draw count를 소스에서 대조한다.
5. 깊이 버퍼 clear, viewport의 프레임버퍼 크기, winding/cull 정책을 확인한다.

처음부터 광원 세기를 계속 올리는 방식으로 모든 화면 오류를 처리하지 않는다. 어느 단계에서 정보가 사라졌는지 한 항목씩 좁힌다. 에이전트가 실행한 결과가 아니므로 실제 화면 확인은 사용자가 수행한다.

## 직접 작성 과제

소스를 닫고 창·프로그램·VAO·draw의 실행 순서를 종이에 적는다. 그 뒤 N01에서 정점4개와 인덱스6개의 사각형으로 바꾸고, N07에서 같은 책임을 Standard의 어떤 API가 맡는지 대응한다. 프로그램을 그대로 복사한 것과 빈 파일에서 필요한 부분을 다시 작성한 것을 별도로 기록한다.
