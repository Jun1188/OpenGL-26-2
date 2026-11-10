# 정점 입력과 셰이더 연결 편집

근거: RenderConfig.h, Renderer.cpp, Application.cpp의 정적 소스. 빌드·실행은 사용자 담당이다.

## 인자만 바꾸는 경우

ApplicationConfig.renderConfig를 run 전에 설정한다. 기본 RenderVertex는 position/color/uv/normal의 float 배열이다.
vertexLayout.attributes의 location을 GLSL layout(location=...)과 맞춘다. attributes에서 사용하지 않는 입력을 빼도 된다.
stride/offset은 실제 업로드한 바이트 배열을 설명한다. GLSL 변수 이름만 바뀌고 location이 같으면 속성 매핑은 그대로다.

```cpp
graphics::ApplicationConfig config;
auto& render = config.renderConfig;
render.vertexLayout.attributes[0].location = 5; // GLSL: layout(location=5) in vec3 position;
render.uniforms.mvp = "objectToClip"; // GLSL: uniform mat4 objectToClip;
app.run(config);
```

GL_FLOAT 등 부동소수 입력은 VertexAttribPointer, ivec/uvec 정수 입력은 integer=true와 VertexAttribIPointer를 사용한다.
허용 성분 자료형은 byte/unsigned byte/short/unsigned short/int/unsigned int/float다. double, packed format, instancing은 현재 제공하지 않는다.
location 중복·장치 범위 초과·잘못된 성분 수·stride/offset 범위 오류를 거부한다.

## 새 데이터가 필요한 경우

packVertices(input, output)는 RenderVertex 배열을 받아 input.size()*stride 바이트를 작성한다.
입력 순서와 정점 수를 보존한다. 모든 면·선 정점이 같은 형식을 사용한다. 사용자 속성의 값은 캡처한 데이터 등으로 채운다.
새 데이터가 Scene에 영속적으로 저장돼야 하면 아래 내부 수정 순서를 따른다.
기본 packer가 없으면 RenderVertex 바이트를 그대로 복사한다. 자료형/offset을 바꿨다면 해당 바이트 해석도 반드시 일치시킨다.

uniforms는 기본 uniform의 이름 매핑이다. mvp에 이름을 지정하면 활성 mat4 uniform으로 존재해야 한다.
사용자 셰이더가 view/projection 등 별도 행렬을 사용하면 uniforms.mvp를 빈 문자열로 두고 bindUniforms에서 필요한 uniform을 모두 설정한다.
추가 uniform은 bindUniforms(context)에 등록한다. program/scene/entity/line/face/model/view/projection/depthOnly를 받는다.
현재 program이 바인딩되고 기본 uniform을 설정한 뒤 호출한다. 이 핸들러에서는 추가 uniform만 설정하고 program/VAO/VBO를 교체하지 않는다.
깊이 전용 패스에서도 호출되므로 변형에 필요한 uniform을 동일하게 설정한다. 윤곽선은 기본 렌더 정책을 사용한다.
추가 uniform의 자료형과 수명, GLSL stages의 입출력 호환은 사용자가 맞춘다. 이름 변경만으로 자료형 변경을 해결하지 않는다.

실행 중에는 app.configureRenderer(render, vertexPath, fragmentPath)를 명시적으로 호출한다.
경로는 작업 디렉터리 기준이며 빈 경로는 내장 셰이더다. 재생성 뒤 다음 렌더에서 Scene을 재업로드한다.
잘못된 layout은 기존 GPU 자원 삭제 전에 거부한다. 셰이더 재생성 실패는 기존 렌더러를 복구하지 않고 종료를 요청한다.

## 내부 코드를 편집해야 할 때의 순서

1. GLSL 입력·uniform 계약을 적는다. 이름, location, 타입, 성분 수, 값의 출처를 정한다.
2. 설정으로 해결되는지 먼저 확인한다. location 변경은 attributes, 기본 uniform 이름은 uniforms, 추가 uniform은 bindUniforms를 사용한다.
3. 데이터 재포장만 필요하면 packVertices와 stride/offset을 함께 바꾼다. 정점 수·순서는 유지한다.
4. 새 영속 속성이 필요하면 include/Model.h의 Vertex 및 Mesh(PlaneFigure) 저장 데이터를 추가한다. 기본값과 정점 수 조건도 정의한다.
5. Geometry/Scene의 생성·편집·유효성 검사와 geometry revision 갱신 경로를 연결한다. 모든 기존 모델에 기본값을 제공한다.
6. include/RenderConfig.h의 RenderVertex와 detail/Renderer.cpp의 면·선 정점 구성 부분을 함께 수정한다. 기본 layout/packer도 맞춘다.
7. 필요한 GL 함수가 없으면 detail/Native.h의 함수 포인터 선언과 load를 함께 추가한다. 컨텍스트 생성 이후에만 호출한다.
8. 새 uniform 처리라면 RenderConfig의 이름 매핑과 Renderer의 위치 조회·업로드를 같은 변경에서 연결한다. GLSL 타입에 맞는 업로드 함수를 쓴다.
9. 외부 shaders/VertexShader.glsl·FragmentShader.glsl와 내장 detail/Shaders.h를 함께 수정한다. fragment 입력, 깊이 패스, 선·윤곽선도 고려한다.
10. API 문서와 사용자 확인 항목을 갱신하고 독립 프로젝트의 해당 framework 파일만 동기화한다.
11. 사용자가 빌드하고 기본 셰이더·변경 셰이더, 면·선·윤곽선·다중 뷰를 확인한다. location 변경, 새 속성, 잘못된 stride를 각각 확인한다.

핸들러가 캡처한 참조는 run과 해당 핸들러의 사용 기간 동안 유효해야 한다. GL 자원의 소유권은 Renderer에 남는다.

## 사용자 데이터 구성과 추가 uniform 예

다음 코드는 run 전에 config.renderConfig에 설정한다. include <cstring>을 추가한다.
float4 position과 unsigned int 태그를 출력하고, GLSL에서 위치는 location=5의 vec4, 태그는 location=6의 uint로 선언한다.
태그는 여기서는 정점 인덱스이며 영속적인 엔티티 ID가 아니다.

```cpp
struct CustomVertex { float position[4]; unsigned int tag; };
auto& render = config.renderConfig;
render.vertexLayout.stride = sizeof(CustomVertex);
render.vertexLayout.attributes = {
    {5, 4, GL_FLOAT, false, offsetof(CustomVertex, position), false},
    {6, 1, GL_UNSIGNED_INT, false, offsetof(CustomVertex, tag), true}};
render.packVertices = [](const std::vector<graphics::RenderVertex>& source,
                         std::vector<std::uint8_t>& bytes) {
    bytes.resize(source.size()*sizeof(CustomVertex));
    for (std::size_t i = 0; i < source.size(); ++i) {
        CustomVertex v{{source[i].position[0], source[i].position[1], source[i].position[2], 1},
                       static_cast<unsigned int>(i)};
        std::memcpy(bytes.data()+i*sizeof(CustomVertex), &v, sizeof(v));
    }
    return true;
};
render.bindUniforms = [](const graphics::DrawUniformContext& context) {
    return context.setUniform("uAmplitude", .1f);
};
```

uniform helper는 float/int/vec3/mat4를 제공한다. 해당 uniform은 실제로 사용되어 활성화돼 있어야 한다.
GLSL 자료형도 일치시킨다. helper는 이름 존재와 값의 유한성을 검사하며 전체 GLSL 타입을 자동 변환하지 않는다.
사용자 셰이더는 지정한 행렬 uniform과 fragment 연결 계약도 유지한다. MVP 이름을 비우면 bindUniforms가 필수다. 없는 기본 선택 uniform은 GL의 -1 위치로 무시된다.

```cpp
render.uniforms.mvp.clear(); // 기본 uMVP 대신 view/projection을 사용하는 GLSL.
render.bindUniforms = [](const graphics::DrawUniformContext& context) {
    return context.setUniform("uView", context.view)
        && context.setUniform("uProjection", context.projection)
        && context.setUniform("uObject", context.model);
};
```
