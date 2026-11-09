"""Author requirement maps, full code references and links. Does not execute builds/tests/apps."""
from pathlib import Path
import json
import os
import re

ROOT = Path(__file__).resolve().parents[1]
STANDARD = ROOT / 'standard'
DATA = {
    24: [
        ('실습 20 물류 센터·컨베이어·상자 5개 유지', '시작/b', 'initScene/placeMovingObjects/onKey', '기존 경로·검은 사각형 개수 표식·컨베이어 시계 보존'),
        ('컨베이어 옆 육면체 트럭', '시작', 'addTruck', '차체·적재함·운전석·바퀴를 트럭 루트 아래 생성; X=-7로 L자 벨트 바깥 배치, 바닥 22×21'),
        ('번호에 해당하는 상자 적재', '1~5', 'loadParcel', '트럭에 부착 후 지정 적재 위치로 배치; 재입력은 중복 적재 방지'),
        ('상자 시점/전체 시점 전환', 'V', 'onKey/updateCamera', '전체 시점은 저장·복귀; 후보 5개를 매 프레임 각 상자의 최종 월드 변환에서 계산'),
        ('처음 선택한 상자 시점은 5번', '초기/V', 'resetState', 'viewBox_=4; 배열 인덱스와 상자 번호 구분'),
        ('상자 다음/이전 시점', 'n/N', 'onKey', '1인칭에서만 5→1, 1→5 순환; 번호·entity ID·후보 5개 월드 좌표 출력'),
        ('트럭 출발/정지', 'g', 'onKey/onUpdate', '중간 정지는 방향 유지'),
        ('바닥 끝에서 정지하고 다음 g에 반대 방향 이동', '끝점/g', 'onUpdate', '트럭 전체 길이를 고려한 Z=-6.5~7.5 범위; clamp 후 정지·다음 방향 반전'),
        ('전체 리셋', 'r', 'initScene/resetState', '장면·적재·부모 관계·트럭 방향·카메라·시계를 초기화'),
        ('종료', 'q', 'onKey', 'Application::requestClose'),
        ('기존 실습 20 입력 유지', 'b,+/-,p/P,좌/우', 'onKey/onFocus/onUpdate', '컨베이어 시작/정지, 줌, 투영, 전체 시점 공전'),
    ],
    26: [
        ('육면체·사각뿔에 조명 적용', '시작/n', 'initScene/apply', '두 형상을 생성하고 하나만 표시; 같은 회전 상태 유지'),
        ('객체 앞쪽 광원과 작은 도형 표시', '시작', 'initScene/apply', '초기 광원 (0,0,3), .25 크기 비조명 표시'),
        ('육면체/사각뿔 전환', 'n', 'onKey/apply', 'shape_ 전환, 객체 위치·회전 유지'),
        ('조명 켜기/끄기', 'm', 'onKey', '조명 계산을 켜거나 끄며 표시 객체는 unlit 유지'),
        ('객체 중심 Y축 자전', 'y', 'onKey/onUpdate/apply', '현재 위치를 유지하고 Y 회전, 재입력으로 정지'),
        ('광원 양/음 방향 중심 Y축 공전', 'r/R', 'onKey/onUpdate/apply', 'PDF의 양/음 방향 요구를 r/R로 구분; 같은 방향 재입력은 정지'),
        ('광원 공전 궤도를 원으로 표시', '시작/거리 변경', 'initScene/apply', '단위 원 선분에 radius_ 배율 적용'),
        ('광원 가까이/멀리 이동', 'z/Z', 'onKey/apply', '1.8~8 범위에서 반지름 조절'),
        ('광원 거리와 궤도 크기 동기화', 'z/Z', 'apply', '광원 위치와 원 배율이 동일 radius_ 사용; 카메라도 궤도 전체가 보이게 조정'),
        ('종료', 'q', 'onKey', 'Application::requestClose'),
    ],
    27: [
        ('구 3개: 중앙 1개와 다른 위치 2개', '시작', 'initScene', '중앙 (0,0,0), 왼쪽 (-3,0,0), 오른쪽 (3,0,0); 구별 반지름·재질색'),
        ('화면 앞쪽 광원과 작은 표시 객체', '시작/apply', 'initScene/apply', '초기 (0,2,5), .2 크기 비조명 표시'),
        ('광원색 3종 전환', 'c', 'onKey/apply', '백색/적색 계열/청색 계열을 순환; 표면 재질색과 별도로 저장'),
        ('중앙 구의 Y축 기준 양/음 공전', 'r/R', 'onKey/onUpdate/apply', '중심 (0,0,0)의 Y축 둘레, 일정 높이 2; 재입력으로 정지'),
        ('종료', 'q', 'onKey', 'Application::requestClose'),
    ],
}


def write(relative, value):
    path = ROOT / relative
    path.parent.mkdir(parents=True, exist_ok=True)
    value = value.rstrip() + '\n'
    if not path.exists() or path.read_text(encoding='utf-8-sig') != value:
        path.write_text(value, encoding='utf-8-sig')


def block(relative, key, note):
    path = ROOT / relative
    text = path.read_text(encoding='utf-8-sig')
    start, end = '<!-- ' + key + ':start -->', '<!-- ' + key + ':end -->'
    text = re.sub(re.escape(start) + r'.*?' + re.escape(end) + r'\n*', '', text, flags=re.S)
    write(relative, text.rstrip() + '\n\n' + start + '\n' + note + '\n' + end)


def main():
    table = '# 실습 24·26·27 요구사항 대응표\n\n'
    table += 'PDF의 글·그림·입력을 소스와 대조한 결과다. 아래의 반영은 **소스 작성** 상태다. 컴파일·실행·화면 확인 성공을 뜻하지 않는다. 빌드·테스트·검증기·앱 실행은 하지 않았다.\n\n'
    table += '24: `lecture_pdf/homework/지엘3 좌표계 변환 (실습 24).pdf` p.1. 26·27: `lecture_pdf/homework/지엘4 조명 (실습 2627).pdf` p.2~3. 조명 구성: 사용자가 지정한 `지엘4 조명.pdf` p.4~19. 25는 자료가 없어 추정하지 않았다.\n\n'
    table += '[응용 N제 12문제](exercises/README.md) · [확장 방법](EXTENDING_MECHANISMS.md) · [변환 템플릿](TRANSFORM_TEMPLATES.md)\n\n'
    manifest = []
    for number, rows in DATA.items():
        folder = ROOT / f'openGL_3dAssign_{number}'
        table += f'## 실습 {number}\n\n[전체 코드와 프로젝트](reference/practices/P{number}.md)\n\n| 요구사항 | 입력 | 구현 함수 | 처리 | 상태 |\n|---|---|---|---|---|\n'
        checks = f'# 실습 {number}: 사용자 확인\n\n이 폴더 전체를 저장소 밖의 별도 경로로 **복사**한 뒤 `.sln`의 Debug/Release x64를 사용한다. 원본을 이동·삭제할 필요는 없다. 폴더 내부 framework에 소스·헤더·GLM/GLFW·런타임 DLL·셰이더·라이선스가 포함돼 있다. 상위/형제 framework를 참조하지 않는다. Visual Studio C++ 도구·Windows SDK·OpenGL 3.3 지원 드라이버는 시스템 요구다.\n\n빌드·테스트·앱 실행은 에이전트가 수행하지 않았다. 아래 확인은 사용자가 수행한다.\n\n'
        for index, (requirement, keys, methods, behavior) in enumerate(rows, 1):
            table += f'| {requirement} | {keys} | `{methods}` | {behavior} | 소스 반영, 실행 미확인 |\n'
            checks += f'{index}. **{keys}**: {requirement}. 예상: {behavior}.\n'
            manifest.append(dict(practice=number, requirement=requirement, input=keys, methods=methods, source_status='implemented', runtime_status='not_run'))
        checks += '\n공통 관찰: 셰이더 오류·GL 오류가 없어야 한다. 프레임률이 달라도 시간당 속도는 같아야 한다. 리셋과 방향 전환을 반복해도 객체가 늘거나 잘못된 부모에 남지 않아야 한다.\n'
        if number == 24:
            checks += '\n[상자 5개 시점의 매핑·예상 좌표·부분/전체 적재 확인](CAMERA_MAPPING.md)을 적재 전후 모두 확인한다.\n'
            write('openGL_3dAssign_24/CAMERA_MAPPING.md', (ROOT / 'docs/practice-24-26-27/CAMERA24.md').read_text(encoding='utf-8-sig'))
        write(folder.relative_to(ROOT) / 'USER_CHECKS.md', checks)
        readme = f'# 실습 {number}\n\n`openGL_assign_{number}.sln`을 연다. 이 폴더 전체가 독립 프로젝트다. 실행·빌드 입력은 폴더 내부 framework에 포함했다. 다른 실습·Standard·루트 framework는 필요하지 않다.\n\n[요구사항과 사용자 확인](USER_CHECKS.md) · [공통 조명 계약](framework/docs/LIGHTING_MATERIALS.md)\n\nConsole이 실습의 상태와 입력을 담당하고, framework가 형상·변환·카메라·렌더링을 담당한다. 빌드·실행·폴더 분리 확인은 사용자에게 맡겼다.\n'
        write(folder.relative_to(ROOT) / 'README.md', readme)
        ref = f'# 실습 {number} 전체 코드 참조\n\n독립 프로젝트의 현재 원문이다. Standard 실행 모드나 컴파일 입력이 아니다. 실습 전체를 복사하기보다 필요한 연산·상태 처리를 참고한다. 원문이 바뀌면 `tools/Write-Practice24Plus.py`로 다시 생성한다.\n\n'
        refdir = STANDARD / 'docs/reference/practices'
        for name in ('Console.h', 'Console.cpp', 'main.cpp', f'openGL_assign_{number}.vcxproj', f'openGL_assign_{number}.sln'):
            source = folder / name
            language = 'cpp' if source.suffix in ('.h', '.cpp') else 'xml' if source.suffix == '.vcxproj' else 'text'
            link = os.path.relpath(source, refdir).replace('\\', '/')
            ref += f'## {name}\n\n[저장소 원본]({link})\n\n```{language}\n' + source.read_text(encoding='utf-8-sig').rstrip() + '\n```\n\n'
        write(f'standard/docs/reference/practices/P{number}.md', ref)
    table += '\n## 조명 강의 요소의 반영\n\n`Material`의 Ka/Kd/Ks/shininess, 패스별 카메라 위치, 월드 위치·역전치 법선, 프래그먼트 Phong 계산, 거리 감쇠, 광원 표시 unlit을 연결했다. 강의 p.19의 `float diffuse = ... * lightColor`는 RGB 결과이므로 vec3로 구현한다. 강의의 `Normal=vNormal`을 그대로 복사하지 않고 전체 모델 변환에 대한 역전치 법선을 유지한다.\n\n[공통 계약](../framework/docs/LIGHTING_MATERIALS.md) · [새 동작 연결](EXTENDING_MECHANISMS.md)\n'
    write('standard/docs/PRACTICE_24_PLUS.md', table)
    write('docs/practice-24-26-27/REQUIREMENTS.json', json.dumps(manifest, ensure_ascii=False, indent=2))

    lighting = '''# 조명·재질·부착 카메라 계약

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
'''
    for folder in ('framework', 'standard/framework', 'openGL_3dAssign_24/framework', 'openGL_3dAssign_26/framework', 'openGL_3dAssign_27/framework'):
        write(folder + '/docs/LIGHTING_MATERIALS.md', lighting)
    note = '[실습 24·26·27 요구사항/코드](docs/PRACTICE_24_PLUS.md) · [새 기믹의 확장 방법](docs/EXTENDING_MECHANISMS.md) · [응용 N제 12문제](docs/exercises/README.md)'
    block('standard/README.md', 'practice-24-plus', note)
    block('standard/docs/REFERENCE_PRACTICES.md', 'practice-24-plus', '[실습 24·26·27 대응표](PRACTICE_24_PLUS.md) · [24](reference/practices/P24.md) · [26](reference/practices/P26.md) · [27](reference/practices/P27.md)\n\n새 실습도 독립 프로젝트이며 Standard 실행 모드는 추가하지 않는다.')
    block('standard/docs/TRANSFORM_REFERENCE.md', 'practice-24-plus', '[부착·카메라·공전·재질 추가 사례](PRACTICE_24_PLUS.md) · [처음 보는 기믹 작성](EXTENDING_MECHANISMS.md) · [응용 N제](exercises/README.md)')
    block('standard/USER_CHECKS.md', 'practice-24-plus', '1. 템플릿 10~13을 각각 load하고 부착 시점·L/K 인계·U/J 반지름·H/N 광택을 관찰한다.\n2. `material sample phong 0.12 1 0.7 96`, `render ambient 1 1 1`을 저장·다시 읽고 같은 값을 확인한다.\n3. 범위를 벗어난 계수나 N=0 명령은 거절되고 기존 장면이 유지돼야 한다.\n4. material unlit on/off는 다른 색·텍스처 상태를 유지해야 한다. 광원 표시 색은 light color를 따라야 한다.\n5. MovingBeacon 참조는 C++ 확장 예제이며 등록 후 사용자 재빌드가 필요하다. load가 C++을 컴파일하지 않는다.\n\n[독립 실습의 전체 요구사항과 확인 절차](docs/PRACTICE_24_PLUS.md) · [응용 N제](docs/exercises/README.md)')
    catalog_path = STANDARD / 'docs/DOCUMENT_CATALOG.json'
    catalog = json.loads(catalog_path.read_text(encoding='utf-8-sig'))
    category = next((c for c in catalog['categories'] if c['id'] == 'practice-24-plus'), None)
    if category is None:
        category = dict(id='practice-24-plus', title='실습 24 이후와 응용 N제', description='요구사항·전체 코드·새로운 사물과 규칙의 설계', documents=[])
        catalog['categories'].append(category)
    category['documents'] = [dict(path=path, title=title, featured=featured) for path, title, featured in [
        ('docs/PRACTICE_24_PLUS.md', '실습 24·26·27 요구사항 대응표', True),
        ('docs/EXTENDING_MECHANISMS.md', '새로운 사물·기믹을 연결하는 방법', True),
        ('docs/exercises/README.md', '응용 N제 12문제', True),
        ('docs/exercises/P24.md', 'P24: 움직이는 기준과 관찰', False),
        ('docs/exercises/P26.md', 'P26: 독립 동작과 광원', False),
        ('docs/exercises/P27.md', 'P27: 재질과 새로운 규칙', False),
        ('framework/docs/LIGHTING_MATERIALS.md', '조명·재질·부착 카메라 계약', False),
        ('docs/reference/practices/P24.md', '실습 24 전체 코드', False),
        ('docs/reference/practices/P26.md', '실습 26 전체 코드', False),
        ('docs/reference/practices/P27.md', '실습 27 전체 코드', False),
    ]]
    write('standard/docs/DOCUMENT_CATALOG.json', json.dumps(catalog, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    main()
