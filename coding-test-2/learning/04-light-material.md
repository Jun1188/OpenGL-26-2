# 광원·법선·재질: 같은 공간에서 계산하기

광원 표시 도형을 그리는 것과 조명을 계산하는 것을 구분한다. 표시 도형이 움직여도 lightPosition uniform이 바뀌지 않으면 표면 조명은 바뀌지 않는다. 반대로 조명만 움직이면 표시 도형이 실제 위치를 설명하지 못한다.

## 1. 월드 공간 계약

```glsl
vec3 N = normalize(worldNormal);
vec3 L = normalize(lightPosition - worldPosition);
vec3 V = normalize(viewPosition - worldPosition);
float diffuse = max(dot(N,L), 0.0);
float specular = dot(N,L) > 0.0
    ? pow(max(dot(V,reflect(-L,N)),0.0), shininess) : 0.0;
```

worldPosition, lightPosition, viewPosition, N은 모두 같은 공간이어야 한다. 물체의 모델 지역 법선과 월드 광원 방향을 바로 내적하지 않는다. 카메라를 이동하면 V가 달라져 정반사가 달라진다. 고정된 표면과 광원에서 diffuse 자체는 카메라 위치에 의존하지 않는다. 화면에 보이는 면과 픽셀은 달라질 수 있으므로 동일 표면 지점을 비교한다.

## 2. 비균등 배율의 법선

```cpp
glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
glUniformMatrix3fv(normalLocation,1,GL_FALSE,glm::value_ptr(normalMatrix));
```

접선 t와 법선 n의 내적은 0이다. 접선을 A*t로 변환할 때 법선을 inverse(transpose(A))*n으로 바꾸면 변환 뒤에도 직교성이 유지된다. 일반적인 A*n은 비균등 배율에서 이 성질을 유지하지 못한다. fragment에서는 보간된 법선을 다시 normalize한다. 위치는 w=1, 방향은 w=0이며 법선에는 이동 성분을 넣지 않는다. 0 배율에서는 역행렬이 없으므로 유효한 조명 변환으로 사용하지 않는다.

## 3. 바꿀 값을 구분하기

| 값 | 맡는 일 | 관찰점 |
|---|---|---|
| 광원 color/intensity | 들어오는 빛의 색/세기 | 여러 물체가 함께 영향 |
| 재질 color | 표면의 기본 색 | 같은 광원에서도 물체별 차이 |
| Ka | 주변광 반응 | 방향과 무관한 바탕 |
| Kd | 확산 반응 | N·L이 변할 때 밝기 변화 |
| Ks | 정반사 반응 세기 | 시점/광원 변화에 따른 하이라이트 |
| shininess | 정반사 지수 | 값이 커지면 일반적으로 더 좁은 하이라이트 |
| attenuation | 거리 감쇠 계수 | 예제는 1/(1+a*거리²) |
| unlit | 해당 재질의 조명 생략 | 광원 표시는 조명 on/off와 무관하게 보임 |

물체의 vertex color/tint/재질 색이 곱해지는 경로도 확인한다. Standard의 기존 방향광 식에는 0.75 확산 계수가 있고 점광원은 별도로 감쇠한다. native 예제는 하나의 점광원과 별도 주변광으로 식을 단순하게 노출한다. 두 경로의 기본 수치와 식이 완전히 같다고 가정하지 않는다. 이 예제는 학습용 Phong 계열 계산이며 그림자·PBR·실제 힘 해석을 제공하지 않는다.

## 4. 계산 항을 분리해 읽기

Standard와 native에서 F2는 합성→법선 색→확산→정반사 순서로 전환한다. 조명 m이 켜진 상태에서 비교한다. unlit 도형과 선은 디버그 항의 대상이 아니다. 법선 색은 (N+1)/2라서 +X는 붉은 성분이 강하고 +Y는 초록 성분이 강하다. 이는 세계 좌표축 기준 색이다.

N03에서 normalMatrix를 고의로 잘못 바꾸었다가 복원하고, N11에서 같은 광원 아래 재질만 다른 표면을 비교한다. 밝아 보이는가보다 어떤 항이 왜 달라졌는지 설명할 수 있어야 한다.
