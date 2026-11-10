"""Write native C++ authoring and source-debugger documentation. No build, tests or app run."""
from pathlib import Path
import html
import json
import re

ROOT = Path(__file__).resolve().parents[1]
REFERENCE = 'https://github.com/Jun1188/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a'


def write(relative, text):
    path = ROOT / relative
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text.strip() + '\n', encoding='utf-8-sig')


DEBUGGER = r'''# Source debugger: 실제 C++ 소스 편집 안내

원하는 변화를 명령으로 입력하면 현재 디스크 소스의 절대 경로, 1부터 시작하는 행 번호, 수정 전 코드, 복사할 수정 후 코드를 출력한다. 사용자가 코드를 직접 편집하고 빌드한다. 프로그램은 대상 파일을 쓰지 않는다.

## 시작

`debugger/debugger.sln`에서 debugger를 빌드한다. 저장소 루트 `OpenGL-26-2.sln`에는 Standard와 debugger가 별도 프로젝트로 들어 있다. 각 프로젝트의 단독 솔루션도 사용할 수 있다. debugger는 OpenGL·GLFW·GLEW·Standard 실행 파일에 의존하지 않는 Windows C++17 콘솔 프로그램이다.

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

색인은 선택한 폴더의 `.cpp/.h/.hpp/.glsl/.props/.vcxproj` 텍스트를 읽는다. `.vcxproj` 입력은 해당 파일의 상위 폴더를 선택한다. MSBuild의 조건·매크로·실제 컴파일 목록을 평가하는 기능은 아니다. `dependencies`, `docs`, `old_standard`, 빌드/IDE/그래프 폴더와 링크·재분석 지점은 순회하지 않는다. 파일은 UTF-8(BOM 허용), 개별 8 MiB 이하여야 한다. 다른 인코딩이나 읽기 오류는 경로와 함께 거절한다.

이 프로그램은 지원 문법과 실제 텍스트를 연결하는 편집 보조 도구다. 자연어 해석기, C++ AST 분석기, 중단점/메모리 디버거 또는 컴파일러는 아니다. 임의 함수·매크로·여러 줄 초기화의 의미는 추론하지 않는다. 카메라 시선·near/far 등 속성 사이의 관계, 부모 실행 순서, 모델별 조건은 사용자가 소스와 실행으로 확인한다. 숫자의 기본 범위 검사만으로 컴파일·화면 성공을 보장하지 않는다.
'''


def refresh_documents():
    write('debugger/README.md', DEBUGGER)
    write('standard/docs/SOURCE_DEBUGGER.md', DEBUGGER)
    examples = (ROOT / 'standard/examples/SourceExamples.h').read_text(encoding='utf-8-sig')
    write('standard/docs/EXAMPLES.md', '''# C++ 변환·조명·카메라 예제

원문은 `examples/SourceExamples.h`다. 외부 assembly 파일이나 명령 문자열을 실행하지 않는 C++ 함수 두 개를 제공한다. 현재 Standard의 기본 장면 대신 확인하려면 `SceneSetup.cpp`에서 이 헤더를 include하고 configureScene 본문을 아래 호출 중 하나로 직접 바꾼 뒤 사용자가 재빌드한다.

```cpp
#include "examples/SourceExamples.h"
// configureScene 본문에서 하나를 선택한다.
source_examples::rotatingDisplay(scene, options);
// 또는 source_examples::attachedLightAndCamera(scene, options);
```

`rotatingDisplay`는 회전 지지대와 표시판이다. 객체 부모 상속, 비균등 배율, 왼쪽 끝 피벗 회전, 후행 지역 이동을 조합한다. `attachedLightAndCamera`는 회전하는 운반대에 광원과 카메라를 붙여 고정 조형물을 관찰한다. 원래 실습의 사물 이름만 바꾼 재출제 예측이 아니라 연산·시간·기준 프레임을 조합하는 예다.

둘 다 시간0 등록 후 `SceneAuthoring::update(seconds)`로 갱신한다. 부모의 등록된 지역 레이어는 0초에서 먼저 반영되어 나중에 등록하는 자식이 지정 월드 배치에서 시작한다. 등록 함수는 장면 초기화 단계에서 사용한다. 런타임에서 레이어를 추가하면 기존 지역 레이어가 우선 0초로 초기화되므로 다음 update까지의 중간 상태를 별도 동작으로 사용하지 않는다.

이 헤더는 참조 함수이며 독립 main을 추가하지 않는다. 구획 주석을 쓰는 기본 SceneSetup.cpp는 구조화 debugger 명령 대상이다. 예제 함수로 교체한 뒤에는 find/code/replace/insert를 사용하거나 편집할 코드에 유일한 @debug 구획을 직접 추가한다. 예제의 실행 결과는 아직 확인하지 않았다.

프레임워크 자체의 별도 main 예제는 `framework/examples/source_authoring.cpp`다. 이를 별도 프로젝트의 진입점으로 선택하고 그 폴더의 Framework.props를 import한다. Standard의 main과 동시에 컴파일하지 않는다.

## 전체 예제 코드

```cpp
''' + examples.rstrip() + '\n```')
    write('standard/README.md', '''# Standard: C++ 직접 작성 프로젝트

`SceneSetup.cpp`에 객체·변환·조명·재질·카메라를 C++ 코드로 작성한다. 수정 후 사용자가 빌드·실행한다.

[전체 문서](index.html) · [작성 흐름](docs/DIRECT_CPP_WORKFLOW.md) · [두 물체 튜토리얼](docs/WORKSPACE_TUTORIAL.md) · [디버거 명령](docs/SOURCE_DEBUGGER.md) · [조명](docs/LIGHTING_AND_MATERIALS.md) · [사용자 확인](USER_CHECKS.md)

- `standard.sln`: 현재 직접 작성 프로젝트. 루트 솔루션에는 별도 debugger 프로젝트도 있다.
- `SceneSetup.cpp`: 사용자가 직접 고치는 장면 설정. 문자열 명령·외부 assembly 파일을 실행하지 않는다.
- `Console.cpp`: 초기화·마우스 객체 이름 확인·시간 갱신·렌더 콜백.
- `framework/`: 이 폴더 안의 배포 사본. 상위·형제 폴더 없이 빌드/실행에 사용한다.
- `old_standard/`: 저장소의 형제 폴더에 보존한 이전 명령·레시피 버전. 그 폴더의 `standard.sln`으로 별도 연다.

클릭은 선택한 객체 이름과 `show object 이름` 안내만 출력한다. Space는 시간 정지/재개, Home은 시간 초기화, Esc는 종료다. 객체 속성은 코드에서 편집한다. 물체 전체 선택이 기본이며 면 편집 명령 모드는 현재 Standard 실행에 포함하지 않는다. 필요하면 기존 면 변환 C++ API를 직접 사용한다.

`debugger`에 원하는 변경을 입력하면 현재 소스의 파일·행·수정 전후 코드가 출력된다. 소스 저장, 재빌드, 화면 확인은 사용자가 수행한다. 빌드·테스트·앱 실행은 에이전트가 수행하지 않았다.
''')
    write('standard/docs/DIRECT_CPP_WORKFLOW.md', '''# C++ 직접 작성과 실행 흐름

작업의 원본은 `SceneSetup.cpp`이다. 명령·레시피가 C++ 원본을 대신하지 않는다.

## 입력 → 상태/변환 → 출력

1. 사용자가 `ObjectSetup`, `LayerSetup`, `PointLight`, `Camera`, `RenderOptions` 값을 직접 편집한다.
2. `main`은 Application 콜백을 한 번 등록한다. `run` 초기화 후 `Console::initScene`이 `configureScene`을 호출한다.
3. `SceneAuthoring::addObject`는 월드 배치를 부모 기준 attachment에 저장한다. 비균등 배율 결과를 SRT로 다시 분해하지 않는다.
4. `Console::onUpdate`가 절대 시간을 누적하고 `SceneAuthoring::update`가 지역 레이어와 부모를 따르는 광원·카메라를 평가한다. Application의 기존 Scene 모션 갱신은 그 뒤 한 번 실행된다.
5. `Console::onRender` → `Application::renderScene` → Renderer가 모델·정규행렬·광원·재질을 GPU에 전달한다. Scene의 월드/면 행렬은 렌더와 피킹이 함께 사용한다.
6. 종료 시 Application이 GPU·창·GLFW 수명을 정리한다. debugger는 이 실행 흐름에 참여하지 않는다.

## 편집 위치

| 바꿀 내용 | 작성 위치 / API |
|---|---|
| 객체 생성·위치·회전·비균등 배율·모델 피벗·재질 | `SceneSetup.cpp` object 구획 / `ObjectSetup`, `addObject` |
| 부모 연결 | `scene.addObject("자식", 설정, "부모")`, 부모를 먼저 등록 |
| 지역 레이어와 지속 회전 | layer 구획 / `LayerSetup`, `addLayer` |
| 독립 주변광·방향광·배경 | render 구획 / `RenderOptions` |
| 점광원·부모 부착 | light 구획 / `PointLight`, `addLight` |
| 투영·시점·부모 부착 | camera 구획 / `Camera`, `setCamera` |
| 특정 기믹의 상태·힘·입력 법칙 | 독립 컨트롤러 또는 `Console.cpp` 콜백에서 기존 공통 API 호출 |

새 사물마다 프레임워크 enum이나 명령 파서를 늘리지 않는다. 기본 형상 도우미는 box/tetra/pyramid를 지원한다. 구·원뿔·모델·면·경로·힘 등은 기존 Scene/PatternSystem/Motion/Dynamics API로 직접 연결할 수 있다. 이들 모두를 디버거의 구조화 명령으로 변환한다고 주장하지 않는다. 그런 소스는 `find/code/replace/insert`로 위치와 코드를 안내받는다.

`SceneAuthoring::update`가 자신에게 등록한 객체의 지역 레이어, 점광원 목록, 등록 카메라를 관리한다. 같은 상태에 별도 컨트롤러를 동시에 쓰면 덮어쓸 수 있다. 네이티브 모션은 별도 객체에 연결하거나 해당 상태의 소유자를 명확히 정한다. `clear` 후 이전 EntityId는 재사용하지 않는다.

각 프레임의 잘못된 입력은 예외로 Application 종료 경로에 전달한다. `addObject` 생성 실패는 생성 전 Scene으로 복원한다. 다른 등록 함수는 값을 확인한 뒤 목록에 추가한다. `update`는 여러 객체에 대한 트랜잭션이 아니며 중간 실패 시 계속 진행하지 않는다.

## 배포 경계

Standard는 자체 framework·셰이더·헤더·라이브러리·실행 DLL을 가진다. debugger는 자체 소스와 Toolchain.props만 사용한다. 합본 솔루션의 프로젝트 경로는 편의용이며 각 단독 솔루션의 빌드/실행 의존성은 아니다. 실제 소스 위치는 debugger 실행 인수나 project 명령으로 선택한다. 폴더를 다른 위치로 복사한 뒤 빌드·실행하는 확인은 사용자가 수행한다.
''')
    write('standard/docs/TRANSFORM_REFERENCE.md', '''# 객체 월드 배치와 지역 레이어

`ObjectSetup`은 `size`로 모델을 만든 뒤 다음 월드 배치를 지정한다.

```text
D = T(position) × Rz × Ry × Rx × S(scale) × T(-pivot)
attachment = inverse(parentWorld) × D
W = parentWorld × attachment × L1 × L2 × ...
```

기본 SRT는 단위행렬이며 attachment에 배치가 들어간다. 부모는 자식 등록 전에 존재해야 한다. 연결 순간에는 지정한 월드 배치를 유지하고 이후 부모 움직임을 상속한다. `size`는 모델 형상 크기, `scale`은 변환 배율이므로 지역 레이어의 이동에도 scale의 영향이 남는다.

각 `LayerSetup`의 행렬은 다음과 같다.

```text
L = T(translation + pivot) × Rz × Ry × Rx × Rspin(seconds) × S(scale) × T(-pivot)
```

translation과 pivot은 선행 레이어의 지역 좌표다. spin은 초당 도 단위이며 `spinAxis`를 정규화한다. 회전 순서를 명시하고 절대 시간에서 계산하므로 프레임별 각도 누적을 사용하지 않는다. 객체 pivot은 `T(-pivot)`으로 모델 원점을 재지정한다. 지역 레이어 pivot은 `T(pivot)...T(-pivot)`으로 해당 점 주위 변환을 수행한다. 피벗 숫자만 수정할 때 기존 화면 배치를 자동 보존하지 않는다.

비균등 배율과 회전을 계층으로 합성한 전단을 행렬 그대로 유지한다. 0 배율, 유한하지 않은 값, 특이 변환은 거절한다. `addLayer` 등록 순서가 의존 순서다. 특정 사물·시험 문제에 고정된 그룹이 아니다.

부모가 있는 점광원의 position과 카메라 position/target/up은 부모 지역 좌표다. 점광원은 점을 월드로 변환한다. 카메라는 `CameraSystem::attached`로 시선/up을 직교화한다. 광원 색·강도·거리 감쇠는 형상 배율과 별개다.
''')
    write('standard/docs/WORKSPACE_TUTORIAL.md', '''# 두 물체를 C++ 코드로 작성하기

디버거 명령으로 수정안을 받고 사용자가 소스를 편집하는 순서다. 아래 명령 자체는 Standard 장면을 변경하지 않는다.

## 1. 부모와 자식의 월드 배치

기본 `SceneSetup.cpp`는 base를 `(0,0,0)`, panel을 `(0,1,0)`에 생성하고 panel을 base에 연결한다. 두 값은 등록 순간의 월드 기준 배치다.

```text
show object base
show object panel
object panel position 3 1 0
```

출력한 실제 파일·행의 after를 직접 반영하고 저장한다. panel 등록 시 base의 현재 월드 행렬 역행렬로 attachment를 계산한다. 초기 base 위치를 바꾸어 재빌드해도 panel은 자신이 지정한 월드 배치에서 시작한다. 이후 실행 중 base가 변하면 panel은 그 변화를 상속한다.

## 2. 비균등 배율과 피벗

```text
object panel scale 2 1 0.5
layer hinge pivot -1 0 0
layer hinge rotation 0 90 0
```

각 명령의 before가 현재 소스와 같은지 확인한 뒤 수동으로 반영한다. panel의 형상 크기는 기본 `(2,1,0.2)`다. object scale과 hinge 지역 회전은 서로 다른 행렬이다. 비균등 배율 뒤의 지역 회전은 전단을 포함할 수 있다. 객체 자체의 단순 월드 Y 회전을 원하면 layer 회전 대신 object rotation을 편집한다.

## 3. 시간 회전과 지역 이동 레이어

```text
layer hinge rotation 0 0 0
layer hinge spin 30
add layer shift panel
```

shift 전체 코드를 실제 삽입 지점에 반영한 뒤 다음 명령을 입력한다.

```text
layer shift translation 1 0 0
```

힌지 회전을 shift가 상속하므로 이동 방향은 회전한다. shift를 먼저 적용하려면 C++의 두 `scene.addLayer` 호출 순서를 바꾼다. 피벗·배율·이동의 순서를 눈으로 예측하고 사용자가 빌드 후 관찰한다. Space는 시간 정지, Home은 0초 복귀다.

## 4. 조명과 카메라

```text
light key position 2 4 1
light key intensity 2
object panel specular 0.8
object panel shininess 128
camera main position 8 5 10
camera main target 0 1 0
```

부모에 광원을 붙이려면 `light key parent base`의 수정안을 반영한다. 이후 position은 부모 지역 좌표다. 재빌드 후 광원 방향, 표면 법선, 카메라 위치가 정반사에 주는 영향을 각각 관찰한다. 이 문서의 화면 결과는 에이전트가 실행해 확인하지 않았다.

회상 질문: 같은 지역 X 이동 1이 object scale과 앞 레이어의 회전 뒤에서 어떤 월드 방향·거리로 나타나는가?
''')
    write('standard/docs/LIGHTING_AND_MATERIALS.md', f'''# 조명·재질·텍스처를 코드에서 편집하기

추가 개념의 참고 저장소는 [Jun1188/ComputerGraphics]({REFERENCE})다. 이번 대응은 확인한 커밋 `f42b28a`를 기준으로 기록한다. 향후 기능은 해당 주제의 실제 소스를 다시 확인한 뒤 추가한다.

## 확인한 원본과 대응

| 참고 소스 | 확인한 개념 | 현재 C++ 작성 위치 |
|---|---|---|
| [조명 fragment.glsl]({REFERENCE}/컴그_조명까지/컴그17/fragment.glsl) | 주변광·확산광·reflect 기반 정반사, 광원/시점/물체 색, shininess | RenderOptions, PointLight, ObjectSetup.material |
| [조명 ComputerGraphics17.cpp]({REFERENCE}/컴그_조명까지/컴그17/ComputerGraphics17.cpp) | CPU에서 광원 위치·색 uniform 전달 | Renderer의 기존 uniform 전달, SceneSetup.cpp의 light 구획 |
| [텍스처 vertex.glsl]({REFERENCE}/컴그_29번_텍스처매핑/컴그17/vertex.glsl) | 모델 역전치 법선 변환과 UV 전달 | 기존 uNormalMatrix, TexCoord 계약 |
| [텍스처 fragment.glsl]({REFERENCE}/컴그_29번_텍스처매핑/컴그17/fragment.glsl) | 텍스처 샘플과 조명 결과 결합 | Material.texture/useTexture와 RenderOptions.textures |

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
''')
    write('standard/USER_CHECKS.md', '''# 사용자가 수행할 빌드·테스트·화면 확인

에이전트는 소스 작성·소스 열람·문서/구조 자료 생성만 수행했다. 아래 항목은 실행하지 않았다.

1. 루트 `OpenGL-26-2.sln`을 Visual Studio에서 열고 Standard와 debugger를 각각 빌드한다. Standard 컴파일 목록에 Assembly*.cpp가 없고 SceneSetup.cpp와 framework/detail/SceneAuthoring.cpp가 있는지 확인한다.
2. debugger를 실행하고 `project "자신의 경로/standard/standard.vcxproj"`, `list`, `show light key`, `light key position 5 6 7`을 입력한다. 절대 경로·실제 행·before의 기존 position·after `{5.f, 6.f, 7.f}`를 소스와 대조한다. 생성 숫자는 유효한 동등 float 표기로 나올 수 있다.
3. 소스 구획 앞에 빈 줄 두 개를 넣어 저장한 뒤 같은 명령을 반복한다. 안내 행은 두 줄 이동해야 한다. position 기존 값을 수동 수정해 저장한 뒤 before도 새 값으로 바뀌는지 확인한다. 대상 소스가 프로그램에 의해 저장되지 않는지도 확인한다.
4. `add object gate box base`의 전체 after를 안내 위치에 반영한다. 저장 후 `show object gate`, `object gate position 3 1 0`이 새 구획과 실제 변수를 찾는지 확인한다.
5. `layer hinge spin 30`을 수동 반영하고 Standard를 재빌드·실행한다. 부모/지역 레이어의 회전을 관찰한다. Space 정지·Home 시간 초기화·클릭의 객체 이름 출력을 확인한다.
6. `light key parent base`와 조명 위치·강도·재질 Ks/N 수정안을 수동 반영한다. 재빌드해 광원과 재질의 효과를 각각 관찰한다. 카메라 변경 시 position/target/up과 near/far가 함께 유효한지 확인한다.
7. 잘못된 숫자, 0 배율, 0 회전축, 없는 구획, 중복 구획, 여러 대입문, 존재하지 않는 파일/행을 입력한다. 잘못된 위치를 성공으로 안내하지 않고 오류를 표시해야 한다. 임시로 바꾼 사용자 소스는 직접 복원한다.
8. `find "SceneAuthoring::update"`를 framework 대상으로 실행하고 실제 결과를 `code`로 읽는다. 구획이 없는 일반 실습에도 find/code/replace/insert의 before가 실제 파일과 같은지 확인한다. 이 명령은 MSBuild 활성 분기나 C++ 의미를 판정하지 않는다.
9. 한글·공백 경로에서 UTF-8 BOM 파일을 읽고 EOF/quit로 종료한다. 링크·재분석 지점 아래로 순회하지 않는지 확인한다.
10. standard 폴더와 debugger 폴더를 각각 다른 위치로 복사해 단독 솔루션 빌드·실행을 확인한다. Standard의 framework 배포 사본과 DLL/셰이더 복사 결과를 확인한다.

`old_standard`는 이전 프로젝트를 보존한 폴더다. 기존 standard 내부에 x64/.vs가 없었으므로 이 이동에서 삭제된 용량은 0바이트다. 저장소 루트의 .vs, 다른 실습의 x64, 사용자 docs.zip·PDF·기존 Renderer.cpp 변경은 정리 범위에서 제외했다.
''')
    api = '''# SceneAuthoring: C++ 직접 작성 API

원본은 `include/SceneAuthoring.h`, 구현은 `detail/SceneAuthoring.cpp`다. Standard에 동일한 배포 소스를 포함한다.

| API | 계약 |
|---|---|
| ObjectSetup / addObject | box/tetra/pyramid, 모델 크기, 월드 배치·비균등 배율·피벗·재질. 부모를 먼저 등록 |
| LayerSetup / addLayer | 객체 소유 지역 레이어, T(p+pivot) Rz Ry Rx Rspin S T(-pivot), 등록 순서 상속 |
| PointLight / addLight | 최대8 점광원. 선택 부모가 있으면 지역 위치에서 월드로 변환 |
| Camera / setCamera | 하나의 활성 카메라. 선택 부모에 부착, 시선/up 직교화 |
| update(seconds) | 절대 시간으로 레이어·광원·카메라 평가. 프레임 dt를 직접 전달하지 않음 |
| object(name) / name(id) | 이름↔Scene 엔티티 ID 연결. 없는 이름은 예외, 없는 ID는 빈 이름 |
| clear | Scene과 작성 핸들을 함께 초기화. 이전 ID를 재사용하지 않음 |

직접 C++로 설정하고 사용자 콜백에서 갱신한다. 소스 편집용 주석은 debugger만 읽으며 런타임 의존성이 아니다. 새로운 사물의 상태·법칙은 독립 컨트롤러에서 기존 변환·Motion·Dynamics API로 연결한다. 재질과 셰이더의 기존 광원 계산을 재사용하며 이번 작업은 새로운 그림자·강체 솔버를 추가하지 않는다.

자세한 작성 순서와 확인 절차는 Standard의 DIRECT_CPP_WORKFLOW.md, SOURCE_DEBUGGER.md, LIGHTING_AND_MATERIALS.md를 참조한다. 소스·문서 반영과 컴파일·실행 성공은 구분한다. 빌드·테스트는 사용자가 수행한다.
'''
    for base in ('framework', 'standard/framework'):
        write(base + '/docs/SOURCE_AUTHORING.md', api)
        write(base + '/README.md', '''# Modern OpenGL C++ framework

이 폴더 전체를 복사하고 Framework.props를 프로젝트에서 import한다. C++17·Windows x64·Visual Studio C++ 도구와 Windows SDK가 필요하다. 헤더·라이브러리·런타임 DLL·셰이더·라이선스는 폴더 안의 배포 사본을 사용한다.

현재 작성 방식은 [SceneAuthoring C++ API](docs/SOURCE_AUTHORING.md)다. ObjectSetup·LayerSetup·PointLight·Camera 값을 소스에서 직접 작성한다. 프레임워크에 명령 파서나 assembly 파일 실행 의존성을 추가하지 않는다. 낮은 수준의 Scene·TransformSystem·PatternSystem·Motion·Dynamics API도 그대로 직접 사용할 수 있다.

[기존 공통 API 레퍼런스](docs/API_REFERENCE.html)는 계속 제공한다. 과거 Standard 명령·레시피·모드 설명은 이전 버전 자료이며 저장소의 old_standard에 보존한다. 현재 Standard의 작성 흐름은 SceneSetup.cpp와 새 문서를 따른다. 기존 명령 예제가 새 Standard에서 실행된다고 간주하지 않는다.

조명은 기존 방향광·점광원·독립 주변광·재질 Ka/Kd/Ks/shininess와 법선 변환을 사용한다. 추가 개념은 사용자가 지정한 ComputerGraphics 저장소의 해당 소스를 확인한 뒤 반영한다. 빌드·실행·화면 확인은 사용자가 수행한다.
''')
        page = ROOT / base / 'docs/API_REFERENCE.html'
        if page.exists():
            text = page.read_text(encoding='utf-8-sig')
            text = re.sub(r'<!-- current-assembly-api:start -->.*?<!-- current-assembly-api:end -->', '', text, flags=re.S)
            text = re.sub(r'<!-- source-authoring:start -->.*?<!-- source-authoring:end -->', '', text, flags=re.S)
            section = '<!-- source-authoring:start --><section><h2>C++ 직접 작성 API</h2><p>현재 Standard는 SceneSetup.cpp를 직접 편집한다. 과거 명령·레시피 설명은 old_standard 자료다.</p><p><a href="SOURCE_AUTHORING.md">계약과 작성 흐름</a></p><pre><code>'
            section += html.escape((ROOT / base / 'include/SceneAuthoring.h').read_text(encoding='utf-8-sig')) + '</code></pre></section><!-- source-authoring:end -->'
            text = re.sub(r'(<body\b[^>]*>)', lambda m: m[0] + section, text, count=1)
            write(base + '/docs/API_REFERENCE.html', text)
    catalog = {'schemaVersion': 1, 'id': 'standard-document-catalog', 'source': 'tools/Write-SourceDebuggerDocs.py',
        'uncategorized': {'id': 'other', 'title': '공통 API·기존 참고 자료', 'description': '현재 작성 흐름은 시작하기 문서를 우선한다. 과거 명령 예제는 old_standard용이다.'},
        'categories': [{'id': 'start', 'title': 'C++ 직접 작성과 편집 안내', 'description': '수동 소스 편집 → 사용자 빌드·실행',
            'documents': [{'path': path, 'title': title, 'featured': True} for path, title in [
                ('README.md', 'Standard 시작'), ('docs/DIRECT_CPP_WORKFLOW.md', '작성 위치와 실행 흐름'),
                ('docs/SOURCE_DEBUGGER.md', 'debugger 명령과 실제 소스 위치'), ('docs/WORKSPACE_TUTORIAL.md', '두 물체 C++ 튜토리얼'),
                ('docs/TRANSFORM_REFERENCE.md', '월드 배치·지역 레이어'), ('docs/EXAMPLES.md', 'C++ 예제 전체 코드'), ('docs/LIGHTING_AND_MATERIALS.md', '조명·재질·참고 소스'),
                ('framework/docs/SOURCE_AUTHORING.md', '공통 작성 API'), ('USER_CHECKS.md', '사용자 확인 절차')]]}]}
    write('standard/docs/DOCUMENT_CATALOG.json', json.dumps(catalog, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    refresh_documents()
