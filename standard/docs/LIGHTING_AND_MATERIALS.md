# 조명·재질·텍스처를 코드에서 편집하기

추가 개념의 참고 저장소는 [Jun1188/ComputerGraphics](https://github.com/Jun1188/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a)다. 이번 대응은 확인한 커밋 `f42b28a`를 기준으로 기록한다. 향후 기능은 해당 주제의 실제 소스를 다시 확인한 뒤 추가한다.

## 확인한 원본과 대응

| 참고 소스 | 확인한 개념 | 현재 C++ 작성 위치 |
|---|---|---|
| [조명 fragment.glsl](https://github.com/Jun1188/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a/컴그_조명까지/컴그17/fragment.glsl) | 주변광·확산광·reflect 기반 정반사, 광원/시점/물체 색, shininess | RenderOptions, PointLight, ObjectSetup.material |
| [조명 ComputerGraphics17.cpp](https://github.com/Jun1188/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a/컴그_조명까지/컴그17/ComputerGraphics17.cpp) | CPU에서 광원 위치·색 uniform 전달 | Renderer의 기존 uniform 전달, SceneSetup.cpp의 light 구획 |
| [텍스처 vertex.glsl](https://github.com/Jun1188/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a/컴그_29번_텍스처매핑/컴그17/vertex.glsl) | 모델 역전치 법선 변환과 UV 전달 | 기존 uNormalMatrix, TexCoord 계약 |
| [텍스처 fragment.glsl](https://github.com/Jun1188/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a/컴그_29번_텍스처매핑/컴그17/fragment.glsl) | 텍스처 샘플과 조명 결과 결합 | Material.texture/useTexture와 RenderOptions.textures |

원본을 그대로 복제하지 않는다. 확인한 조명 버전은 vertex의 FragPos가 vec3인데 fragment에서는 vec4이며, 그 vertex는 법선을 모델 공간 그대로 전달한다. 텍스처 버전은 FragPos가 vec3으로 일치하고 역전치 법선 변환을 사용한다. 현재 프레임워크는 기존 CPU 정규행렬 전달을 유지한다. 두 원본의 정상 실행 여부는 확인하지 않았다.

## 직접 편집할 값

- `render main`: lighting, 독립 ambientColor, 방향광 lightDirection/lightColor/directionalIntensity, 배경. 기본 예제는 방향광 강도 0으로 점광원의 효과를 구분한다.
- `light key`: position, color, intensity, attenuation. 기본 셰이더는 점광원 최대 8개이며 감쇠는 `1 / (1 + attenuation × distance²)`다.
- `object panel`: material.color, ambientStrength, diffuseStrength, specularStrength, shininess, unlit. unlit는 조명을 생략한다.
- `camera main`: position이 시점 벡터와 정반사 방향 계산에 반영된다. FOV·near/far·직교 투영은 별도다.

현재 셰이더는 ambientColor와 재질 Ka를 곱한 독립 주변광에 기존 방향광의 주변광·확산·정반사 항을 더한다. 방향광에는 기존 0.75 확산 계수가 있고 점광원에는 거리 감쇠를 적용한다. 이 기존 식을 참고 저장소의 수치와 완전히 동일하다고 설명하지 않는다. 정확한 식은 `framework/detail/Shaders.h`와 `framework/shaders/FragmentShader.glsl`을 따른다.

## 텍스처의 직접 코드 경로

파일 로딩은 Application의 AssetLoader와 기존 모델 API를 사용한다. 디버거의 `render main textures on`만으로 이미지나 UV가 생성되지 않는다. Material.texture에 유효한 TextureImage를 연결하고 useTexture를 켜며, 메시 UV와 RenderOptions.textures를 함께 설정해야 한다. 현재 구조화 명령은 이미지 로딩 코드 전체를 생성하지 않는다. `find/code/replace/insert`와 기존 모델 예제를 이용한다.

그림자·스포트라이트·PBR·접촉/강체 솔버는 이번에 구현한 기능이 아니다. 앞으로 추가할 때 실제 참고 파일, CPU 상태, 셰이더 입력, 직접 편집 위치, 사용자 관찰 절차를 함께 문서화한다.


## 2차 코딩테스트 작업공간

2026-11-24 화요일 시험 준비의 전체 학습 HTML·순정 OpenGL·N제·시나리오는 저장소의 coding-test-2 폴더에 별도로 둔다. Standard 제출 입력은 해당 학습 폴더를 참조하지 않는다. 같은 저장소에서는 [오프라인 학습](../../coding-test-2/index.html)으로 이동할 수 있다. Standard만 복사한 배포본에서는 이 링크가 학습 참고 링크이며 빌드/실행 입력이 아니다.

ExamController는 key 광원과 월드 카메라, studySphere/studySpin, lightMarker를 소유한다. 기존 초기 광원을 읽고 부모 이름을 보존한다. 다른 소스가 같은 상태를 동시에 쓰지 않는다. 전체 장면을 기존 examples 함수로 교체할 때는 main.cpp의 Console sample(app, false)로 기본 컨트롤러를 끈다. 기본 실행은 true다.

외부 GLSL은 main의 shaderDirectory=framework/shaders로 선택한다. 소스 GLSL 수정 후 사용자가 Rebuild하여 출력 폴더의 사본도 갱신한다. 내장 detail/Shaders.h만 바꿔도 이 외부 경로가 바뀌는 것은 아니다. uStudyView=0/1/2/3은 합성/법선/확산/정반사이며 기존 내장 셰이더 사용자에게 기본 0을 유지한다.

제출은 debugger, 프로젝트 x64/.vs, docs, 튜토리얼·학습 HTML·예제를 뺀 소스 기준 30,000,000바이트 이하다. SDK의 x64 라이브러리·DLL·헤더·라이선스는 남긴다. Package-Standard.py는 실제 파일 삭제 없이 이 범위로 ZIP을 만든다. 표준 개발 환경(OS·MSVC·Windows SDK·호환 드라이버)은 필요하다. 상위/형제 폴더·개인 절대 경로의 실행/빌드 입력을 두지 않는다.

빌드·테스트·브라우저/앱 실행·화면 결과는 사용자 확인 대상이다. 소스·문서 생성과 실행 성공을 구분한다.
