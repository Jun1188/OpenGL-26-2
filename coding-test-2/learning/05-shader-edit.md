# 셰이더를 고쳤는데 안 바뀔 때부터 배우기

## 1. 지금 실행되는 소스부터 찾기

Standard main.cpp의 config.shaderDirectory는 framework/shaders다. 배포된 실행 파일 옆의 framework/shaders/VertexShader.glsl과 FragmentShader.glsl을 읽는다. 소스 폴더에서 파일을 편집한 뒤 사용자가 프로젝트를 Rebuild하면 Framework.props가 실행 폴더로 복사한다. 단순 실행 재시작만으로 원본 소스가 새로 복사되는 것은 아니다. native는 실행 파일 옆 shaders 두 파일을 읽는다.

shaderDirectory가 비어 있으면 framework/detail/Shaders.h의 내장 문자열을 사용한다. 이 작업은 Standard에서 외부 파일을 명시했지만 다른 실습의 선택을 일괄 변경하지 않았다. 파일 이름 대소문자, 실제 경로, 복사 시점, 내장/외부 선택을 먼저 확인한다.

## 2. CPU와 GLSL을 한 쌍으로 고치기

예를 들어 최종 색의 세기를 바꾸는 uExtraGain을 Standard에 추가한다. 아래는 사용자가 적용할 확장 예제이며 기본 프로젝트에 이미 선언된 uniform이 아니다.

```glsl
// FragmentShader.glsl의 다른 uniform 옆
uniform float uExtraGain;
// 최종 FragColor 대입 직전에
color.rgb *= uExtraGain;
```

```cpp
// main 함수에서 app.run이 끝날 때까지 살아 있는 변수
float extraGain = 1.2f;
// config.renderConfig.bindUniforms의 기존 study binding도 유지한다.
config.renderConfig.bindUniforms = [&](const graphics::DrawUniformContext& context) {
    return sample.bindUniforms(context) && context.setUniform("uExtraGain", extraGain);
};
```

이름, 타입(float→1f), program 바인딩 시점, draw 전에 전달하는 순서를 맞춘다. 사용되지 않는 uniform은 최적화로 사라질 수 있다. OpenGL은 location=-1의 uniform 쓰기를 무시하지만, 제공한 native location 함수와 Standard setUniform은 필요한 계약 누락을 드러내도록 실패를 보고한다. 오류를 피하려고 임의 location 숫자를 하드코딩하지 않는다.

## 3. 셰이더 인터페이스

vertex의 out과 fragment의 in은 이름/타입을 맞춘다. WorldPosition과 Normal은 vec3다. 참고 저장소의 조명 예제는 FragPos의 vec3/vec4가 버전 사이에서 다르므로 그대로 결합하지 않는다. 이 자료의 실행 코드와 연결된 두 GLSL을 기준으로 읽는다. #version은 첫 유효 지시문이어야 한다. 파일의 UTF-8 BOM은 제공 로더가 제거한다.

## 4. 증상에서 수정 위치로

| 증상 | 먼저 읽을 위치 |
|---|---|
| GLSL compile 실패 | 출력한 파일 경로와 컴파일 로그 행 번호 |
| link 실패 | vertex out / fragment in, 필요한 선언 |
| 변경해도 같은 화면 | shaderDirectory, 실행 폴더의 사본, Rebuild 여부 |
| 회전하면 밝은 면이 이상함 | model, normalMatrix, WorldPosition의 공간 |
| 시점 이동에 diffuse까지 끌려감 | 광원/법선 계산에 view-space와 world-space를 혼용했는지 |
| 광원 표시는 움직이나 조명은 그대로 | CPU light 값→uniform 업로드 시점 |
| uniform 미발견 | 철자·타입·실제 사용 여부·선택한 program |

수정은 한 번에 한 항목씩 한다. 상수색 출력→법선 색→diffuse→specular→합성을 순서대로 비교한다. 이 절차의 직접 실행과 결과 판정은 사용자 몫이며, 자료 생성으로 셰이더 컴파일이 성공했다고 보고하지 않는다.
