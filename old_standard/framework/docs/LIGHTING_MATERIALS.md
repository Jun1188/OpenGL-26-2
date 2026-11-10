# 조명·재질·부착 카메라 계약

CPU 메시의 색·법선과 Material → 최종 월드 행렬 → Renderer uniform → 프래그먼트 셰이더 → 화면 순서다. 빌드·GLSL 컴파일·화면은 사용자 확인 전이다.

## 재질

Material에는 `ambientStrength=.25`, `diffuseStrength=1`, `specularStrength=0`, `shininess=32`, `unlit=false`가 있다. Ka/Kd/Ks는 유한한 0~1, 광택은 1~1024다. 기존 색·텍스처 필드 뒤에 추가해 기존 aggregate 초기화 순서를 유지했다. `Scene::setMaterial(id,face,material)`은 그 메시의 재질을 교체한다. 필요한 필드만 바꿀 때 기존 재질을 복사해 수정한다.

`RenderOptions::ambientColor`는 기본 (0,0,0)의 독립 주변광이다. 방향광은 기존 주변 항과 .75 확산 항을 유지한다. 기본 재질·주변광에서는 이전 방향광·점광원의 계산을 유지하고 정반사만 0이다. 점광원은 최대 8개, 감쇠는 `1/(1+attenuation*distanceSquared)`다. 감쇠를 생략하려면 0으로 둔다.

```text
ambient = Ka * ambientColor
directional = (Ka + .75*Kd*max(N·L,0) + specular) * sunColor * sunIntensity
point = (Kd*max(N·L,0) + specular) * pointColor * intensity / (1+a*d²)
specular = Ks * pow(max(V·reflect(-L,N),0), shininess), N·L>0일 때
finalRGB = vertexColor * materialColor * tint * texture * (ambient+directional+sum(point))
```

정반사도 객체색과 곱하는 강의 모델이다. 에너지 보존 PBR 모델은 아니다. `unlit`은 해당 표면의 조명 계산을 건너뛰며 광원 표시·표지 등에 사용한다. 실제 그림자·스포트라이트·강체 접촉은 이 셰이더의 기능이 아니다.

## 좌표와 확장

WorldPosition과 N/L/V는 월드 공간이다. N은 `transpose(inverse(mat3(model)))`로 변환한 뒤 프래그먼트에서 정규화한다. 전체 부착·지역 레이어·면 변환이 같은 모델 행렬에 들어간다. 뒷면의 정반사는 0이며, 광원/시점과 표면이 일치할 때 분모가 0이 되지 않게 제한한다.

`CameraSystem::attached(local,frame)`은 지역 카메라의 위치·시선·up을 아핀 프레임에 옮기고 직교화한다. 퇴화한 시선은 nullopt다. 위치는 부모 배율을 따르지만 FOV·near/far는 그대로다. 카메라와 광원을 형상 S와 독립된 관절에 붙이면 모양 배율과 관찰 기준을 분리할 수 있다.

`UniformBindings`에 viewPosition/ambientColor/materialAmbient/materialDiffuse/materialSpecular/shininess 이름을 추가했다. 사용자 셰이더는 이를 사용하거나 `RenderConfig::bindUniforms`로 자체 계약을 연결한다. 기본 셰이더는 `detail/Shaders.h`이며 외부 `shaders/*.glsl`과 같은 계산을 사용한다.

## Standard 명령

```text
add sphere sample at 0 0 0
material phong 0.12 1 0.7 96
render ambient 1 1 1
light sun 0
light point lamp at 2 2 3
light color 1 0.4 0.2
material sample unlit on
material sample unlit off
```

material의 대상 생략은 선택 물체다. 명시한 대상은 해당 물체의 기본 메시 전체 면에 적용하며 자손 전체에 재귀 적용하지 않는다. `unlit` 변경은 색·계수·텍스처를 초기화하지 않는다. 명령 실패는 Assembly의 장면·작성 상태 복원 경로를 사용한다. `save`는 명시적 대상 material 명령과 최종 ambient 값을 저장한다. 사용자 콜백이 직접 바꾼 임의 외부 데이터의 직렬화는 별도 계약이다.
