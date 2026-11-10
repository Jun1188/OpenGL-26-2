# Source debugger: 실제 C++ 소스 편집 안내

원하는 변화를 명령으로 입력하면 현재 디스크 소스의 절대 경로, 1부터 시작하는 행 번호, 수정 전 코드, 복사할 수정 후 코드를 출력한다. 사용자가 코드를 직접 편집하고 빌드한다. 프로그램은 대상 파일을 쓰지 않는다.

## 시작

저장소 기준 `standard/debugger/debugger.sln`에서 debugger를 빌드한다. `standard/standard.sln`은 Standard만 참조한다. 루트 `OpenGL-26-2.sln`은 debugger도 표시하는 편의 솔루션이며 기본 전체 빌드는 Standard만 대상으로 한다. 이 편의 솔루션에서는 debugger 프로젝트를 선택해 별도로 빌드한다. debugger의 단독 솔루션에서는 debugger를 빌드한다. Standard와 framework는 debugger의 소스·헤더·라이브러리·실행 파일을 참조하지 않는다. debugger도 OpenGL·GLFW·GLEW·Standard 실행 파일에 의존하지 않는 Windows C++17 콘솔 프로그램이다.

사용자가 Debug/x64로 debugger를 빌드한 뒤 실행할 경로는 다음과 같다.

```powershell
$glStandard = 'C:/Work/OpenGL-26-2/standard'
& "$glStandard/debugger/x64/Debug/debugger.exe" "$glStandard"
```

이 경로는 자기 복사 위치로 바꾼다. Standard 실행 파일은 `standard/x64/Debug/standard.exe`이며 debugger 실행 파일과 별개다.

실행 시 프로젝트 폴더 또는 `.vcxproj` 경로를 인수 하나로 전달하거나 아래처럼 입력한다. 예제 경로는 자기 복사 위치로 바꾼다.

```text
project "C:/Work/OpenGL-26-2/standard/standard.vcxproj"
list
show object panel
object panel position 2 1 0
object panel scale 2 1 0.5
layer hinge pivot -1 0 0
layer hinge spin 30
light key position 2 4 1
object panel specular 0.8
camera main fov 55
render main ambient 0.2 0.2 0.2
```

위 명령은 수정 제안만 출력한다. `.assembly`를 만들거나 장면을 실시간 변경하지 않는다. 파일을 저장한 다음 같은 명령을 다시 입력하면 **변경한 소스의 행과 기존 값**을 다시 읽는다. IDE의 저장하지 않은 버퍼는 읽을 수 없다. 변경 감시는 명령을 처리하는 시점에 수행하며, 창을 띄워 계속 감시하는 기능은 아니다.

## 출력 계약

`[수정 제안] 실제 절대 경로:현재 행`, `[위치·의미] 구획/속성/좌표계/단위`, `[before] 디스크에서 읽은 코드와 행 번호`, `[after] 복사 가능한 C++ 코드`, `[주변 코드]`를 출력한다. 신규 코드에서는 실제 삽입 지점과 전체 등록 코드를 보여주고 before는 없다고 표시한다. 행 번호 예시를 고정 값으로 제공하지 않는다.

## 명령과 인수

| 명령 | 결과와 인수 의미 |
|---|---|
| `object 이름 position/rotation/size/scale/pivot X Y Z` | 객체의 월드 배치, Euler XYZ(도), 모델 크기, 비균등 배율, 모델 지역 피벗 대입문 제안 |
| `object 이름 color R G B` | 재질 RGB, 각 값 0~1 |
| `object 이름 ambient/diffuse/specular/shininess 값` | Ka/Kd/Ks 또는 정반사 지수 |
| `object 이름 unlit on/off` | 재질 조명 계산 생략 |
| `object 이름 shape box/tetra/pyramid` | 기본 형상 변경 |
| `layer 이름 translation/rotation/scale/pivot/axis X Y Z` | 선행 레이어 기준 지역 변환 또는 회전축 |
| `layer 이름 spin 도/초` | 절대 시간에서 평가하는 지역 회전 속도 |
| `light 이름 position X Y Z`, `light 이름 color R G B` | 점광원 위치/색. parent가 있으면 위치는 부모 지역 좌표 |
| `light 이름 intensity/attenuation 값` | 강도/거리 제곱 감쇠 계수, 0 이상 |
| `camera 이름 position/target/up X Y Z` | 시점/주목점/up. parent가 있으면 부모 지역 좌표 |
| `camera 이름 fov/near/far/extent 값` | 수직 화각/클립 거리/직교 투영 짧은 축 반길이 |
| `camera 이름 ortho on/off` | 직교 투영 또는 원근 투영 |
| `render main lighting/textures on/off` | 렌더 기능 사용 |
| `render main ambient/sun-color R G B` | 독립 주변광/방향광 색 |
| `render main sun-direction X Y Z`, `render main sun-intensity 값` | 표면에서 광원을 향하는 방향/방향광 강도 |
| `render main background R G B A` | 배경 RGBA |
| `object/light/camera 이름 parent 부모/none` | 등록 호출의 부모 인수 변경. 같은 파일의 앞 구획에 부모가 있어야 함 |
| `add object 이름 box/tetra/pyramid [부모]` | 객체 선언·모든 편집 속성·등록 호출의 전체 삽입안 |
| `add layer 이름 객체` | 지역 레이어 선언·속성·등록 호출 삽입안 |
| `add light 이름 [부모]`, `add camera 이름 [부모]` | 점광원 또는 활성 카메라 삽입안 |
| `find "문자열"` | 현재 소스에서 실제 파일·행·해당 줄 검색 |
| `code "파일" 시작행 끝행` | 실제 소스 행 범위 표시 |
| `replace "파일" 시작행 끝행 "코드"` | 임의 C++/GLSL/프로젝트 파일의 정확한 수정 전후 제안 |
| `insert "파일" 앞행 "코드"` | 해당 행 앞의 삽입안. 마지막 행+1은 파일 끝 |
| `help`, `quit` | 도움말, 종료 |

`replace/insert`의 코드 인수에서 `\n`은 줄바꿈으로 해석한다. 코드 안의 큰따옴표는 `\"`로 입력한다. Windows 폴더 경로는 따옴표로 감싸고 닫는 따옴표 앞의 불필요한 끝 역슬래시는 생략한다.

프레임워크 자체를 수정할 위치도 찾을 수 있다.

```text
project "C:/Work/OpenGL-26-2/framework"
find "SceneAuthoring::update"
find "uMaterialSpecular"
```

검색 결과에서 현재 행을 확인하고 `code` 또는 `replace`로 범위를 지정한다. `find` 결과에는 헤더 선언·사용부 등 여러 후보가 나올 수 있으며, 첫 결과를 자동 정답으로 선택하지 않는다.

## 구획 표시와 지원 경계

```cpp
// @debug begin light key
graphics::PointLight cfg_key;
cfg_key.position = {2.f, 4.f, 3.f};
cfg_key.color = {1.f, 1.f, 1.f};
cfg_key.intensity = 1.f;
cfg_key.attenuation = 0.02f;
scene.addLight("key", cfg_key);
// @debug end light key
```

이 표시는 C++ 주석이다. 실행·빌드에는 필요하지 않으며 디버거의 위치 기준으로만 사용한다. 종류+이름 구획은 선택한 프로젝트 안에서 유일해야 한다. 구획 이름은 ASCII 식별자를 쓴다. 속성 대입은 한 줄에 한 문장으로 작성한다. 프로그램은 현재 변수 이름을 대입문에서 읽는다. 구획이 중복되거나 대입이 여러 개면 추정하지 않고 거절한다. 구획이 없는 일반 실습은 `find/code/replace/insert`를 사용할 수 있다.

새 코드 삽입에는 `configureScene(graphics::SceneAuthoring& scene, graphics::RenderOptions& options)` 안의 독립된 `// @debug insert` 한 줄을 사용한다. 부모와 대상 객체를 먼저 등록한다. 새 코드를 수동으로 반영한 뒤 다음 명령을 입력해야 새 구획을 찾을 수 있다.

색인은 선택한 폴더의 `.cpp/.h/.hpp/.glsl/.props/.vcxproj` 텍스트를 읽는다. `.vcxproj` 입력은 해당 파일의 상위 폴더를 선택한다. MSBuild의 조건·매크로·실제 컴파일 목록을 평가하는 기능은 아니다. `dependencies`, `docs`, 하위 `debugger`, `old_standard`, 빌드/IDE/그래프 폴더와 링크·재분석 지점은 순회하지 않는다. 따라서 Standard를 대상으로 찾을 때 편집 도구 자체의 구현은 섞이지 않는다. debugger 폴더 자체를 project 대상으로 지정하면 그 안의 소스는 읽을 수 있다. 파일은 UTF-8(BOM 허용), 개별 8 MiB 이하여야 한다. 다른 인코딩이나 읽기 오류는 경로와 함께 거절한다.

이 프로그램은 지원 문법과 실제 텍스트를 연결하는 편집 보조 도구다. 자연어 해석기, C++ AST 분석기, 중단점/메모리 디버거 또는 컴파일러는 아니다. 임의 함수·매크로·여러 줄 초기화의 의미는 추론하지 않는다. 카메라 시선·near/far 등 속성 사이의 관계, 부모 실행 순서, 모델별 조건은 사용자가 소스와 실행으로 확인한다. 숫자의 기본 범위 검사만으로 컴파일·화면 성공을 보장하지 않는다.
