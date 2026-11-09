"""Author current workspace docs from source. No builds, tests or browser execution."""
from pathlib import Path
import html
import json
import re

ROOT = Path(__file__).resolve().parents[1]
STANDARD = ROOT / 'standard'

def write(path, text):
    path = ROOT / path
    path.parent.mkdir(parents=True, exist_ok=True)
    text = text.rstrip() + '\n'
    if not path.exists() or path.read_text(encoding='utf-8-sig') != text:
        path.write_text(text, encoding='utf-8-sig')

def page(title, body):
    return ('<!doctype html><html lang="ko"><head><meta charset="utf-8">'
            '<meta name="viewport" content="width=device-width,initial-scale=1"><title>' + html.escape(title) +
            '</title><style>body{font:16px/1.8 "Malgun Gothic",sans-serif;max-width:1050px;margin:32px auto;padding:0 24px}'
            'pre{overflow:auto;background:#152b3e;color:#eef5ff;padding:18px;border-radius:8px}a{color:#14649b}'
            'table{border-collapse:collapse}td,th{border:1px solid #ccd6de;padding:8px}</style></head><body><h1>' +
            html.escape(title) + '</h1>' + body + '</body></html>')

def refresh_documents():
    source = (STANDARD / 'modes/AssemblyGuide.cpp').read_text(encoding='utf-8-sig')
    help_body = source[source.index('void Assembly::help'):source.index('void Assembly::guide')]
    commands = ''.join(json.loads('"' + m[1] + '"') for m in re.finditer(r'"((?:\\.|[^"\\])*)"', help_body))
    write('standard/docs/QUICK_COMMANDS.md', '# 명령어 빠른 참조\n\n'
          '콘솔 F12/help와 같은 소스에서 만든다. 형식은 **명령어 [실행 결과] - 각 인수의 의미**다.\n\n'
          '```text\n' + commands.rstrip() + '\n```\n\n'
          '`t/r/s`는 월드 기준 물체 배치를 바꾼다. `layer t/r/s`는 선택 레이어의 지역 변환을 추가한다. '
          '입력량이 같아도 축·순서에 따라 결과가 다르다. `s`는 상대 배율이며 `srt`는 기준 SRT 설정이다.\n\n'
          '[좌표·레이어 계약](TRANSFORM_WORKSPACE.md) · [응용 레시피](APPLICATION_RECIPES.md)')
    write('standard/README.md', '''# Standard: 0번 장면 작업공간

[전체 문서](index.html) · [명령어](document.html?doc=docs/QUICK_COMMANDS.md) · [변환·상속](document.html?doc=docs/TRANSFORM_WORKSPACE.md) · [응용](document.html?doc=docs/APPLICATION_RECIPES.md)

Standard는 0번 작업공간으로 시작한다. 실습 1~22의 번호 전환은 제거했다. 기존 모드와 보조 컨트롤러는 `docs/reference/modes/*.md`에 전체 소스를 담은 레퍼런스 문서로 전환했다. Standard 프로젝트에는 현재 작업공간 구현 파일만 명시한다. `main.cpp`에서 숫자 모드를 지정할 필요가 없다.

## 바로 작성

```text
add box
t 2 0 0
s 2 1 0.5
layer add hinge
layer r z 30
insert spin y 45
save scene.assembly
```

왼쪽 클릭으로 물체를 선택하고 Ctrl+클릭으로 그 부모 물체를 선택한다. 빈 공간 클릭은 작업 평면의 생성 기준점을 지정하고 선택을 해제한다. 표면 클릭은 교차점을 생성 기준점으로 사용한다. `add`는 새 물체를 자동 선택하며 이름 생략 시 `box1`처럼 생성한다. 좌표를 직접 지정할 때는 `add box item at 2 0 0`을 사용한다.

## 입력

- G/R/S → X/Y/Z → 마우스 좌우 이동 또는 방향키: 이동/회전/배율. Shift+방향키는 미세 조작이다.
- Enter 또는 클릭: 확정. Esc: 미리보기 취소. 포커스 이탈도 미리보기를 취소한다.
- `layer select 이름` 상태에서는 같은 단축키가 해당 지역 레이어에 작용한다. `layer select none`으로 물체 배치 편집에 복귀한다.
- 기본 선택은 물체 전체다. `edit part` 또는 `edit face`를 명시한 경우에만 부품·면을 선택한다. 면 모드에서는 t/r/s와 단축키가 선택 면에 작용한다. Esc는 물체 모드로 복귀한다.
- Space: 시간 정지/재개. F12: 명령 도움말. Ctrl+F5: 마지막 읽은 레시피 재읽기. `quit`: 종료.

## 경계

`framework`는 공통 C++ API, Standard는 명령·선택·편집을 맡는다. 문서와 과거 가이드 소스는 실행에 필요하지 않다. 콜백은 main에서 한 번 등록하고 Application이 입력 → 사용자 갱신 → Scene 모션 → 렌더 → swap을 실행한다.

`save`는 현재 작성 상태를 재구성하는 레시피를 저장한다. 선택 대상·레이어·커서 좌표를 명시한 명령으로 기록한다. 물리·모션의 현재 프레임 전체를 저장하는 기능은 아니다. 상대 경로는 **프로세스 작업 폴더** 기준이다. 실행 파일 옆에 쓰려면 그 폴더를 작업 폴더로 지정하거나 절대 경로를 쓴다.

소스 검토와 문서 생성만 수행했다. 빌드·테스트·화면 확인은 [사용자 확인 절차](USER_CHECKS.md)에 따라 사용자가 수행한다. 변경 텍스트는 UTF-8 BOM이다.
''')
    write('standard/docs/TRANSFORM_WORKSPACE.md', '''# 월드 배치·객체 상속·지역 레이어

## 합성 계약

열벡터 기준으로 다음과 같이 합성한다. 레이어 목록에서 앞에 있는 항목은 뒤 항목의 좌표계를 제공한다. 정점에는 오른쪽 행렬부터 작용한다.

```text
W_child = W_parent × A_child × B_child × L1 × L2 × ... × Ln
B_child = T(basis) × R × S × T(-localBasis)
```

객체 상속은 Scene의 parent 관계다. `A`는 객체가 소유한 부모 기준 아핀 부착 행렬, `B`는 기존 SRT, `L`은 그 객체가 소유한 지역 레이어다. 별도 전역 그룹을 만들어 레이어를 여러 물체에 암묵적으로 적용하지 않는다. 공통 움직임이 필요하면 부모 객체에 레이어를 넣고 자식 객체를 연결한다.

## 전역 좌표로 부착 위치 지정

사용자가 입력한 월드 기준 배치 D를 다음과 같이 저장한다.

```text
A_child = inverse(W_parent) × D × inverse(B_child)
```

자기 레이어 L은 이 계산에서 바꾸지 않는다. 부모 W에는 부모 자신의 레이어까지 포함한다. `parent base`는 현재 배치와 자기 레이어를 보존해 연결하며 이후 부모의 움직임을 상속한다. `parent none`은 같은 방식으로 연결을 해제한다. `t X Y Z`는 자기 레이어 이전 기준 배치의 월드 원점이다. `r`과 `s`는 최종 피벗을 중심으로 월드 축에 상대 변환을 적용한다.

예: 부모가 월드 X=10, 자식이 X=12일 때 연결하면 부모 기준 간격은 2다. 이후 부모가 X=15로 이동하면 자식은 X=17이 된다. 자식 내부 `layer t 1 0 0`은 별도의 지역 변위다.

## 비균등 배율과 행렬 보존

`s 2 1 0.5`, `layer s 2 1 0.5`, 축별 SRT를 지원한다. `s`는 배율을 곱한다. 절대 SRT는 `srt 이름 TX TY TZ RX RY RZ SX SY SZ`이며 R은 도 단위, Rz×Ry×Rx 순서다. 반사 배율도 사용할 수 있다. 0배율과 역행렬이 없는 부착·레이어 행렬은 거부한다.

비균등 배율을 가진 부모와 회전한 자식을 합성하면 전단이 생길 수 있다. 부착과 지역 레이어는 4×4 행렬로 보존하며 균일 배율이나 SRT로 강제 환원하지 않는다. 렌더·피킹·소유 선은 Scene의 같은 월드 합성을 사용한다. 법선은 렌더러의 역전치 행렬, 반사 앞면은 determinant 기반 winding 보정을 따른다.

## 지역 레이어 작성과 의존 순서

```text
layer add offset
layer t 2 0 0
layer add tilt
layer r z 30
layer add shape
layer s 2 1 0.5
layer select shape
layer before tilt
layer list
```

각 객체 안의 목록이 곧 단일 의존 체인이다. 분기하는 레이어가 필요하면 별도 자식 객체로 만든다. 레이어 이름은 객체 안에서 고유하며 `none`은 예약어다. `layer reset`은 정적 행렬만 초기화하고 모션은 유지한다. `layer play off`는 시간을 멈추고, `layer enable off`는 그 레이어 기여를 제거한다. 레이어 순서 변경은 뒤에 있는 모든 지역 축에 영향을 준다.

`insert spin`, `orbit`, `linear`, `pendulum`은 새 지역 모션 레이어를 만든다. orbit의 월드 중심은 삽입 시점의 객체 프레임으로 변환해 저장한다. 나중에 부모를 움직이면 공전 중심도 그 객체와 함께 이동한다. 지정 중심을 계속 월드에 고정하는 제약 조건은 아니다.

## 소스

- `framework/include/Model.h`: Component_Poly의 attachment/localLayers.
- `framework/detail/Scene.cpp`: placementMatrix, setWorldPlacement, setParentPreserveWorld, setLocalLayers, worldMatrix.
- `standard/modes/AssemblyWorkspace.cpp`: 짧은 명령 해석, 레이어 작성, 변환 미리보기.
- `standard/modes/AssemblyRuntime.cpp`: 모션 샘플과 레이어를 행렬로 전달.
- `standard/modes/AssemblyPersistence.cpp`: 대상·레이어를 명시한 레시피와 부착 행렬 저장.
''')
    write('standard/docs/APPLICATION_RECIPES.md', '''# 응용 장면을 짧게 작성하기

[명령어](QUICK_COMMANDS.md) · [좌표와 레이어](TRANSFORM_WORKSPACE.md) · [확인 절차](../USER_CHECKS.md)

## 부모 이동과 자식의 지역 회전

```text
clear
add joint base at 0 0 0
insert linear 3 0 0 4
add box arm at 2 0 0
parent base
insert spin z 30
```

base의 이동은 arm에 상속된다. arm의 자전은 자기 지역 레이어다. 부모의 위치 제어와 자식의 회전 코드를 한 업데이트 함수에 섞지 않는다.

## 이동 가능한 조명

```text
light point lamp at 3 4 2
light color 1 0.8 0.5
light intensity 2
insert orbit y 30 center 0 0 0
```

점광원은 표시용 메시가 있어 클릭할 수 있다. t/r/s·부모·지역 레이어로 위치를 바꾼다. 기본 셰이더는 점광원 최대 8개다. `light sun 0`은 기존 방향광을 끈다. 점광원 강도 0은 해당 광원을 끈다. 거리 감쇠는 `1/(1+attenuation*distance²)`다.

## 카메라 추가 후 이동

```text
camera add observer at 6 4 8
camera look 0 0 0
camera use
t 5 3 7
camera fov 50
camera free
```

카메라도 같은 객체 상속과 레이어를 사용한다. 시선은 월드 목표점으로 입력한다. 상속된 배율·전단은 시선/up을 정규직교화해 카메라 자세에서 제거한다. near/far/FOV를 형상 배율로 자동 변경하지 않는다. 활성 카메라의 표시 메시는 자기 시야를 가리지 않도록 숨긴다. `camera free` 후에는 일반 시점의 camera move/yaw/orbit/zoom 명령을 사용할 수 있다.

## 이동·회전하는 스프링 저울

```text
load examples/assembly/scenarios/s13-moving-scale.assembly
force read load
pause on
```

이 예제는 **영자연길이의 3차원 선형 스프링과 점질량** 모델이다. 실제 시험의 저울 형상을 재현한 것은 아니다. 강체 carrier의 공전 레이어 → 기준 좌표계 가속도 → 중력·관성항·스프링 힘 → 추 위치 → 렌더 순서다. 선 길이와 기울기가 변하며 스프링 힘을 N으로 출력한다. 충돌·마찰·양팔 저울의 강체 토크는 자동 추가하지 않는다.

지역 유효 가속도는 `Rᵀ(g-a_origin) - ω×(ω×r) - 2ω×v - α×r`다. ω와 α는 지역 좌표로 바꾸며 단위는 rad/s와 rad/s²다. 콘솔 회전 입력은 도/초다. Standard는 프레임 이동·회전의 유한차분으로 가속도를 근사한다. C++에서는 ReferenceFrame에 해석적으로 계산한 값을 직접 넣을 수 있다.

`force read`의 스프링 힘은 실제 현재 스프링/감쇠 힘의 크기다. 같이 표시하는 접촉 반력은 **그 위치에 정지하도록 구속한다고 가정한** +Y 압축력이다. 움직이는 추의 실제 접촉력과 같다고 해석하지 않는다.

## C++에서 사용자 힘 추가

```cpp
graphics::ReferenceFrame frame;
graphics::ParticleState particle;
particle.mass = 1.0f;
auto spring = graphics::DynamicsSystem::spring({0, 0, 0}, 40.0f, 3.0f);
graphics::ForceFunction force = [spring](const graphics::ParticleState& p, double t) {
    return spring(p, t) + glm::vec3(float(2 * std::sin(t)), 0, 0);
};
bool ok = graphics::DynamicsSystem::step(particle, frame, dt, {0, -9.8f, 0}, force, time);
```

콜백은 **지역 뉴턴값**을 반환하는 순수 함수로 작성한다. 질량으로 나누는 처리는 step이 맡는다. 외부 파일·공유 상태를 변경하는 부수 효과는 복구하지 않는다. 기본 적분은 최대 1/240초 간격의 semi-implicit Euler이며 강성·질량·프레임 속도에 따른 수치 오차가 있다. 힘 기준 프레임은 강체여야 한다. 비균등 배율은 시각적 메시 자식에 적용한다.
''')
    checks = '''# 사용자 빌드·동작 확인

이 작업에서는 빌드·테스트·검증기·앱·브라우저를 실행하지 않았다. 아래 절차는 사용자가 수행한다.

1. Visual Studio에서 `standard/standard.sln`의 x64 구성을 빌드하고 실행한다. 0번 작업공간만 시작되고 숫자/F1~F11로 실습이 전환되지 않아야 한다.
2. 빈 공간 클릭 → `add box` → `s 2 1 0.5`: 생성 기준점에 새 물체가 생기고 축별 크기가 달라져야 한다. 콘솔은 명령 [결과] - 인수 설명과 실제 해석 인수·월드 원점을 출력해야 한다.
3. `add joint base at 10 0 0`, `add box child at 12 0 0`, `parent base`: 연결 직후 자식 위치가 유지되어야 한다. base를 `t 15 0 0`으로 옮기면 자기 레이어가 없는 child는 X=17이어야 한다.
4. 부모를 비균등 배율·회전시키고 자식을 다시 연결한다. 월드 배치가 유지되고 렌더·피킹·축 표시가 일치해야 한다. 0배율 또는 순환 부모 연결은 이전 상태를 유지하며 거부해야 한다.
5. `layer add a`, `layer t 2 0 0`, `layer add b`, `layer r z 30`: 목록 순서에 따른 결과를 관찰한다. b를 `layer before a`로 옮기면 결과가 바뀌어야 한다. 부모 이동은 계속 상속되어야 한다.
6. G/R/S 미리보기 → Enter, Esc, 포커스 이탈을 각각 수행한다. Enter만 확정하고 Esc/포커스 이탈은 이전 상태를 복원해야 한다. `edit face`에서도 취소를 확인한다. 기본 클릭은 면을 선택하지 않아야 한다.
7. 광원과 카메라 예제를 실행한다. 점광원 이동에 따라 조명이 바뀌고 카메라의 t/r/s·레이어가 시점에 반영되어야 한다. camera free 후 표시 메시가 다시 보여야 한다.
8. 서로 다른 물체·레이어를 마우스로 고른 뒤 편집하고 save/load한다. 대상이 바뀌거나 같은 변환이 중복 적용되지 않아야 한다. 모션·적분 시계는 초기부터 재시작한다.
9. 정지 ReferenceFrame에서 1kg의 normalLoad는 +Y 법선·중력 (0,-9.8,0)일 때 9.8N이어야 한다. 중력 0, ω=(0,0,2), r=(3,0,0)이면 유효 원심 가속도는 (+12,0,0), 정지 구속력은 (-12,0,0)N이다.
10. 정지 프레임·1kg·강성40의 영자연길이 수직 스프링은 감쇠 후 앵커 아래 약 0.245m에 접근해야 한다. 회전 저울 예제에서는 관성항으로 길이·기울기·힘이 변한다. 과도 진동을 정적 하중으로 해석하지 않는다.
11. `standard/index.html`에서 명령·변환·응용·API·실습 소스 참조를 연다. `s13-moving-scale`의 줄별 설명, 목차·검색·코드 링크를 확인한다. 문서를 제거한 소스 복사본도 Standard 런타임에서 문서 읽기를 요구하지 않아야 한다.

실패를 보고할 때는 콘솔 명령, 선택 물체·레이어, `diagnose 이름`, 화면, 빌드 오류 위치를 함께 기록한다.
'''
    write('standard/USER_CHECKS.md', checks)
    write('standard/docs/assembly/USER_CHECKS.md', checks.replace('# 사용자 빌드·동작 확인', '# 조립 작업공간 사용자 확인'))
    aliases = {
        'README': ('0번 작업공간 문서 지도', '../QUICK_COMMANDS.md', '모든 실습 실행을 합치는 컨테이너 구조는 참조용으로 전환했다. 기본 작업은 객체·상속·지역 레이어다.'),
        'USER_GUIDE': ('작업공간 사용 순서', '../APPLICATION_RECIPES.md', '클릭 → 생성 → 월드 배치 → 부모 연결 → 지역 레이어 → 저장 순서로 작성한다.'),
        'SPEC': ('명령과 좌표계 계약', '../QUICK_COMMANDS.md', '명령 도움말은 AssemblyGuide.cpp에서 생성한다. 내부 with/attachment/face-edit 명령은 저장 재현용이며 직접 입력할 필요가 없다.'),
        'CODE_GUIDE': ('C++ 확장 위치', '../APPLICATION_RECIPES.md', '`AssemblyWorkspace.cpp`는 선택·배치·레이어, `AssemblySceneTools.cpp`는 광원·카메라, `AssemblyDynamics.cpp`는 힘 연결이다. 새 물리 법칙은 framework/Dynamics API로 작성한다.'),
        'EDIT_DEBUG_GUIDE': ('편집과 오류 위치', '../../USER_CHECKS.md', '콘솔 실패는 명령 전 Scene·작성 상태·카메라·RenderOptions를 복원한다. 사용자 콜백의 외부 부수 효과는 복원하지 않는다. update 실패는 시간을 멈추고 원인을 출력한다.'),
        'FEATURE_CONTAINERS': ('과거 실습 컨테이너 소스 참조', '../REFERENCE_PRACTICES.md', 'FeatureContainer와 feature 모드 혼합은 현재 Standard 런타임에서 제외했다. 과거 소스의 구조를 읽는 참조 자료이며 새 Assembly API와 연결하는 사용법이 아니다.'),
        'REFLECTION_AND_HOTLOAD': ('레시피 재읽기와 C++ 변경', '../QUICK_COMMANDS.md', 'load/reload/Ctrl+F5는 데이터 명령을 다시 읽는다. C++ 수정은 사용자가 재빌드한다. 실행 중 C++ 리플렉션·함수 교체를 제공하지 않는다.'),
        'PATTERN_COMBINATIONS': ('객체와 변환 템플릿 조합', '../APPLICATION_RECIPES.md', '부모 객체가 공통 움직임을 제공하고 자식 객체가 지역 레이어를 소유한다. 기존 탱크·사람 생성 코드는 선택적으로 참고한다.'),
        'REQUIREMENT_AUDIT': ('현재 지원 범위', '../TRANSFORM_WORKSPACE.md', '월드 배치·부모 상속·비균등 배율·순차 지역 레이어·피킹·면 세부모드·점광원·카메라·사용자 힘을 제공한다. 강체 충돌·마찰·시험 문제별 제약 솔버는 별도 응용이다.'),
    }
    for name, (title, link, text) in aliases.items():
        write('standard/docs/assembly/' + name + '.md', '# ' + title + '\n\n' + text + '\n\n[현재 문서](' + link + ')')
    reference = '# 실습 코드 참조\n\n과거 모드 1~22와 컨트롤러 소스는 전체 코드를 담은 Markdown 문서로 전환했다. standard.vcxproj는 현재 작업공간 .cpp만 명시하며 이 문서는 None 항목이다. 공통 경로 연산은 AssemblyPaths.cpp에 독립 함수로 보존했다.\n\n'
    for p in sorted((STANDARD/'docs/reference/modes').glob('*.md')):
        reference += '- ['+p.name+'](reference/modes/'+p.name+')\n'
    reference += '\n각 문서는 당시 구조 참조다. 새 Assembly API와 함께 컴파일하는 코드가 아니다. 원래 독립 실습 프로젝트는 저장소의 각 openGL_* 폴더에 별도로 남아 있다.\n'
    write('standard/docs/REFERENCE_PRACTICES.md', reference)
    for name in ('MODE_ACTIONS', 'MODE_INPUT_AUDIT'):
        write('standard/docs/' + name + '.md', '# 실습 모드 안내 변경\n\n0번 작업공간으로 통합했다. 구형 숫자 모드 조작표는 현재 동작과 맞지 않아 제거했다.\n\n[실습 소스 참조](REFERENCE_PRACTICES.md) · [현재 명령](QUICK_COMMANDS.md)')
    framework = '''# 객체 부착·지역 레이어·힘 API

공통 API는 문서·Standard 명령 파서 없이 사용할 수 있다. 공개 진입점은 Framework.h다.

| API | 입력 → 결과·실패 계약 |
|---|---|
| Scene::placementMatrix(id) | 부모 최종 변환 × 객체 부착 × 기준 SRT. 자기 지역 레이어 이전의 월드 배치. 잘못된 ID/행렬은 빈 optional |
| Scene::setWorldPlacement(id,D) | D를 부모 기준 아핀 부착으로 변환. 자기 레이어 유지. 특이행렬/없는 ID는 false |
| Scene::setParentPreserveWorld(id,parent) | 순환 검사 후 배치를 유지해 연결. 실패 시 기존 부모·부착 유지 |
| Scene::setAttachment(id,A) | 부모 기준 행렬 직접 설정. 유한하고 역행렬 있는 아핀 행렬 필요 |
| Scene::setLocalLayers(id,matrices) | 순서대로 지역 합성. 최대128, 실패 시 기존 목록 유지 |
| RenderOptions::pointLights | 월드 점광원 목록. 기본 셰이더 최대8, 색·강도·거리 감쇠 |
| DynamicsSystem::effectiveAcceleration | 강체 기준, 지역 위치·속도, 월드 중력 → 지역 가속도 |
| DynamicsSystem::step | 점질량·힘 콜백·dt≤0.25초 → 상태 적분. 비유한 결과는 미반영 |
| DynamicsSystem::holdingForce | 그 위치를 기준 프레임에 고정할 때 필요한 지역 힘 |
| DynamicsSystem::normalLoad | 정지 구속 힘을 지정 법선에 투영한 압축력. 실제 접촉 솔버가 아님 |
| DynamicsSystem::spring | 지역 앵커·양수 강성·0이상 감쇠 → 힘 콜백. 잘못된 값은 빈 함수 |

기존 MotionSystem의 공유 모션 레이어와 Component_Poly의 객체 내부 localLayers는 별개다. 새 객체 작성은 parent + attachment + localLayers 계약을 사용한다. 기존 AnimationSystem::poseHierarchy는 여전히 양수 균일 기준 자세와 강체 모션을 전제하는 호환 API다. 비균등·전단 결과를 이 API로 분해하지 않는다.

```cpp
auto parent = scene.addGroup();
auto child = scene.addCuboid({1,1,1}, {2,0,0});
if (parent && child) {
    bool attached = scene.setParentPreserveWorld(*child, *parent);
    auto turn = graphics::TransformSystem::rotation({{0,0,0},{0,1,0}}, 30);
    auto scale = graphics::TransformSystem::scaling({2,1,0.5f});
    if (attached && turn && scale) scene.setLocalLayers(*child, {*turn, *scale});
}
```

모든 행렬은 4×4 아핀 행렬이다. setLocalLayers는 결과를 SRT로 재분해하지 않는다. 렌더와 피킹은 Scene::worldMatrix를 공유한다.
'''
    for base in ('framework', 'standard/framework'):
        write(base + '/docs/WORKSPACE_API.md', framework)
        path = ROOT / base / 'README.md'
        text = path.read_text(encoding='utf-8-sig')
        start, end = '<!-- workspace-current:start -->', '<!-- workspace-current:end -->'
        text = re.sub(re.escape(start) + r'.*?' + re.escape(end) + r'\n*', '', text, flags=re.S)
        note = start + '\n[객체 부착·지역 레이어·힘 API](docs/WORKSPACE_API.md). Standard는 0번 작업공간이며 실습은 코드 참조다.\n' + end + '\n\n'
        write(base + '/README.md', note + text)
    write('standard/PORTABLE.md', '# Standard 폴더 사용\n\nstandard 전체를 복사하고 standard.sln의 Windows x64 구성을 사용한다. C++17, Visual Studio C++ 도구와 OpenGL 3.3이 필요하다. framework/dependencies는 실행 의존성이다. docs·브라우저 목차·과거 실습 소스는 편집기 런타임 의존성이 아니다. 빌드 후에는 레시피가 실행 파일 옆 examples에 복사된다. `save` 상대 경로는 프로세스 작업 폴더 기준이다. 빌드·실행 확인은 사용자가 수행한다.')
    body = '<p><a href="../index.html">전체 문서</a> · <a href="../document.html?doc=docs/QUICK_COMMANDS.md">명령어</a> · <a href="../document.html?doc=docs/APPLICATION_RECIPES.md">응용 예제</a></p>'
    body += '<p>마우스로 대상 지정 → 월드 배치 → 객체 상속 → 지역 레이어 → 조명·카메라·힘 → 저장 순서로 작성한다. 빌드·테스트는 사용자가 수행한다.</p>'
    body += '<pre>' + html.escape('add box\ns 2 1 0.5\nlayer add hinge\nlayer r z 30\ninsert spin y 45\nlight point lamp at 3 4 2\nsave scene.assembly') + '</pre>'
    for p in sorted((STANDARD / 'examples/assembly/scenarios').glob('*.assembly')):
        if p.stem.endswith('features-mix'): continue
        text = p.read_text(encoding='utf-8-sig')
        match = re.search(r'#\s*@title:\s*(.*)', text)
        body += '<details><summary>' + html.escape(match[1] if match else p.stem) + '</summary><p><a href="../' + p.relative_to(STANDARD).as_posix() + '">레시피 원문</a></p><pre>' + html.escape(text) + '</pre></details>'
    body += '<p>기존 레시피의 긴 명령은 명시적 대상 이름을 사용하는 호환 경로다. 새 작성에는 짧은 명령과 지역 레이어를 우선한다. 과거 feature 모드 혼합은 코드 참조다.</p>'
    write('standard/docs/EXAM_SCENARIOS.html', page('응용 시나리오: 객체와 지역 레이어로 작성', body))
    for name in ('IMPLEMENTATION_PATTERNS.html', 'EXAM_IMPLEMENTATION_ORDER.html'):
        write('standard/docs/' + name, page('응용 장면 작성 순서', body.split('<details>', 1)[0] + '<p><a href="EXAM_SCENARIOS.html">전체 시나리오와 줄별 설명</a></p>'))
    catalog_path = STANDARD / 'docs/DOCUMENT_CATALOG.json'
    catalog = json.loads(catalog_path.read_text(encoding='utf-8-sig'))
    remove = {'README.md','USER_CHECKS.md','docs/MODE_INPUT_AUDIT.md','docs/MODE_ACTIONS.md','docs/QUICK_COMMANDS.md',
              'docs/TRANSFORM_WORKSPACE.md','docs/APPLICATION_RECIPES.md','docs/REFERENCE_PRACTICES.md'}
    for category in catalog['categories']:
        category['documents'] = [d for d in category['documents'] if d['path'] not in remove]
        if category['id'] == 'start':
            category['description'] = '0번 장면 작업공간의 입력·명령·좌표계. 실습은 소스 참조다.'
            category['documents'] = [{'path':'README.md','title':'0번 작업공간 시작','featured':True},
                {'path':'docs/QUICK_COMMANDS.md','title':'명령어 [결과] - 인수 의미','featured':True},
                {'path':'docs/TRANSFORM_WORKSPACE.md','title':'월드 배치·상속·지역 레이어','featured':True},
                {'path':'docs/APPLICATION_RECIPES.md','title':'조명·카메라·회전 저울 응용','featured':True},
                {'path':'USER_CHECKS.md','title':'사용자 빌드·동작 확인'}] + category['documents']
        if category['id'] == 'exam':
            category['description'] = '실습과 다른 조건도 객체·변환·힘으로 조합한다.'
            for d in category['documents']:
                if d['path']=='docs/EXAM_SCENARIOS.html': d.update(title='일반 응용 시나리오',summary='월드 배치·지역 레이어·회전 스프링 저울')
        if category['id'] == 'assembly':
            category['description'] = '짧은 명령과 객체별 지역 레이어로 작성·저장·확장한다.'
            for d in category['documents']:
                if d['path'].endswith('FEATURE_CONTAINERS.md'): d['summary']='과거 컨테이너 소스 참조; 현재 런타임 제외'
    reference_category = next((c for c in catalog['categories'] if c['id']=='practice-reference'), None)
    if reference_category is None:
        reference_category={'id':'practice-reference','title':'실습 소스 참조','description':'실행 모드가 아닌 코드 레퍼런스','documents':[]}
        catalog['categories'].append(reference_category)
    reference_category['documents']=[{'path':'docs/REFERENCE_PRACTICES.md'}]
    write('standard/docs/DOCUMENT_CATALOG.json', json.dumps(catalog, ensure_ascii=False, indent=2))
    refresh_transform_templates()

def refresh_transform_templates():
    import runpy
    runpy.run_path(str(ROOT/'tools/Write-TransformReference.py'))['main']()

if __name__ == '__main__':
    refresh_documents()
