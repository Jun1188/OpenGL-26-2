# 문서 생성기 변경 전후

기준 커밋: `61274ebd850e224185ff7810ebbd65022e3fe801`. 신규 파일은 before가 없다. 전체 문서 생성 경로 실행은 사용자에게 맡긴다.

## tools/Format-ApiHeaders.py

before: 없음

### after
```python
"""Format owned C++ headers using whitespace edits; never build or run tests."""
from pathlib import Path
import difflib
import hashlib
import json
import re
import subprocess
from tree_sitter import Language, Parser
import tree_sitter_cpp

ROOT = Path(__file__).resolve().parents[1]
PARSER = Parser(Language(tree_sitter_cpp.language()))
OUT = ROOT / 'framework-analysis/api-reference'


def write(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding='utf-8-sig', newline='\n')


def functions(node):
    if node.type in ('function_declarator', 'operator_cast'):
        return True
    return any(functions(c) for c in node.named_children
               if c.type not in ('compound_statement', 'parameter_list', 'template_argument_list'))


def format_header(source):
    data = source.encode('utf-8')
    tree = PARSER.parse(data)
    edits = {}

    def gap(a, b, value, priority=0):
        if a <= b and not data[a:b].strip():
            old = edits.get((a, b))
            if old is None or priority >= old[0]:
                edits[a, b] = priority, value.encode('utf-8')

    def indent(node):
        start = data.rfind(b'\n', 0, node.start_byte) + 1
        return re.match(rb'[ \t]*', data[start:node.start_byte]).group().decode('utf-8')

    def walk(node):
        # Raw GLSL, strings, preprocessor lines and comments retain their bytes.
        if node.type in ('raw_string_literal', 'string_literal', 'comment') or node.type.startswith('preproc'):
            return
        children = node.named_children
        if node.type == 'compound_statement' and children:
            pad = indent(node.parent) if node.parent is not None else indent(node)
            gap(node.start_byte + 1, children[0].start_byte, '\n' + pad + '    ')
            for a, b in zip(children, children[1:]):
                if b.start_point.row == a.end_point.row:
                    gap(a.end_byte, b.start_byte, '\n' + pad + '    ')
            gap(children[-1].end_byte, node.end_byte - 1, '\n' + pad)
        if node.type == 'if_statement':
            consequence = node.child_by_field_name('consequence')
            condition = node.child_by_field_name('condition')
            if consequence and condition and consequence.type != 'compound_statement':
                if consequence.start_point.row == condition.end_point.row:
                    gap(condition.end_byte, consequence.start_byte, '\n' + indent(node) + '    ')
        if node.type in ('field_declaration_list', 'declaration_list', 'translation_unit'):
            for i, child in enumerate(children):
                if child.type not in ('declaration', 'field_declaration', 'function_definition', 'template_declaration'):
                    continue
                if not functions(child):
                    continue
                first = i
                while first and children[first - 1].type == 'comment':
                    comment = children[first - 1]
                    # A trailing comment belongs to the preceding declaration.
                    if first > 1 and children[first - 2].end_point.row == comment.start_point.row:
                        break
                    first -= 1
                for a, b in zip(children[first:i], children[first + 1:i + 1]):
                    gap(a.end_byte, b.start_byte, '\n' + indent(child), 2)
                if first:
                    previous = children[first - 1]
                    if previous.type != 'access_specifier':
                        gap(previous.end_byte, children[first].start_byte,
                            '\n\n\n' + indent(child), 3)
                last = i
                if i + 1 < len(children) and children[i + 1].type == 'comment':
                    if children[i + 1].start_point.row == child.end_point.row:
                        last += 1
                if last + 1 < len(children) and children[last + 1].type != 'comment':
                    gap(children[last].end_byte, children[last + 1].start_byte,
                        '\n\n\n' + indent(children[last + 1]), 1)
        for child in children:
            walk(child)

    walk(tree.root_node)
    result = data
    for (a, b), (_, value) in sorted(edits.items(), reverse=True):
        result = result[:a] + value + result[b:]
    return result.decode('utf-8')


def main():
    paths = [ROOT / 'standard/Console.h']
    for base in ('framework', 'standard/framework'):
        paths.extend(sorted(p for p in (ROOT / base).rglob('*.h') if 'dependencies' not in p.parts))
    inventory, report = [], ['# 헤더 변경 전후\n',
        '공백·줄바꿈만 변경했다. 주석은 선언에 붙여 유지했다. 빌드·테스트·IDE hover 확인은 사용자에게 맡긴다.\n']
    for path in paths:
        raw = path.read_bytes()
        before = subprocess.check_output(
            ['git', 'show', 'HEAD:' + path.relative_to(ROOT).as_posix()], cwd=ROOT).decode('utf-8-sig')
        before = before.replace('\r\n', '\n')
        # This formatter is a one-time migration against the approved clean HEAD.
        # Preserve all non-whitespace bytes, including complete literal contents.
        def tokens(text):
            data = text.encode('utf-8')
            def leaves(node):
                if not node.children or node.type in ('raw_string_literal', 'string_literal', 'char_literal', 'comment'):
                    return [data[node.start_byte:node.end_byte]]
                return [token for child in node.children for token in leaves(child)]
            return leaves(PARSER.parse(data).root_node)
        if tokens(before) != tokens(raw.decode('utf-8-sig').replace('\r\n', '\n')):
            raise RuntimeError('Existing non-format source change: ' + str(path))
        after = format_header(before)
        changed = after != before or not raw.startswith(b'\xef\xbb\xbf')
        inventory.append({'file': path.relative_to(ROOT).as_posix(), 'changed': changed,
                          'before_sha256': hashlib.sha256(raw).hexdigest()})
        if not changed:
            continue
        # Refuse to overwrite a file changed by another writer during generation.
        if path.read_bytes() != raw:
            raise RuntimeError('Concurrent source change: ' + str(path))
        write(path, after)
        report.append('\n## ' + path.relative_to(ROOT).as_posix() + '\n')
        a, b = before.splitlines(), after.splitlines()
        for group in difflib.SequenceMatcher(None, a, b, autojunk=False).get_grouped_opcodes(2):
            report.append('\n변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).\n')
            report.append('\n### before\n```cpp\n' + '\n'.join(a[group[0][1]:group[-1][2]]) + '\n```\n')
            report.append('\n### after\n```cpp\n' + '\n'.join(b[group[0][3]:group[-1][4]]) + '\n```\n')
    write(OUT / 'HEADERS_BEFORE_AFTER.md', ''.join(report))
    write(OUT / 'HEADER_INVENTORY.json', json.dumps(inventory, ensure_ascii=False, indent=2) + '\n')
    print('Owned headers:', len(paths), '; changed:', sum(r['changed'] for r in inventory))


if __name__ == '__main__':
    main()

```

## tools/Refresh-ApiArchitecture.py

before: 없음

### after
```python
"""Source-backed API architecture diagrams and a directed graphify code graph."""
from pathlib import Path
import argparse
import collections
import importlib.util
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'framework-analysis/api-reference'
DIRECTORIES = ('framework/docs', 'standard/docs', 'standard/framework/docs')


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, ROOT / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


guide = load('api_guide', 'tools/author_framework_guides.py')
esc = guide.esc


def read(path):
    return (ROOT / path).read_text(encoding='utf-8-sig')


def write(path, text):
    path = ROOT / path
    path.parent.mkdir(parents=True, exist_ok=True)
    content = text.replace('\r\n', '\n').encode('utf-8-sig')
    if path.exists() and path.read_bytes() == content:
        return
    # Match the existing API catalog writer: finish a sibling file before replacement.
    temporary = path.with_name(path.name + '.api.tmp')
    temporary.write_bytes(content)
    os.replace(temporary, path)


def dump(path, value):
    write(path, json.dumps(value, ensure_ascii=False, indent=2) + '\n')


def row(x, y, width, *lines, decision=False):
    return (x, y, width, 78 if not decision else 100, decision, list(lines))


def diagrams():
    result = {}
    def add(key, title, nodes, edges, height, evidence, note):
        result[key] = dict(title=title, html=guide.graph_svg('api-' + key, title, nodes, edges, 1200, height),
                           evidence=evidence, note=note)
    add('ownership', '01 소유·참조·상속 관계', [
        row(400,20,400,'main 스코프','Application app; Console sample(app)'),
        row(40,160,360,'Application','값 멤버와 창·GLFW 수명 소유'),
        row(800,160,360,'Console','app_은 Application& 비소유 참조'),
        row(10,330,275,'Scene','CPU 컴포넌트·MotionSystem 소유'),
        row(310,330,275,'Camera / RenderOptions','Application의 값 멤버'),
        row(610,330,275,'Renderer','GL 핸들·업로드 캐시 소유'),
        row(910,330,275,'optional<AssetLoader>','Application 안에 emplace / reset'),
        row(10,500,540,'Component_Poly → Polyhedron → Mesh','vector 값 소유; EntityId는 조회 번호'),
        row(610,500,575,'program / VAO / VBO / EBO / texture','Renderer가 생성·삭제; GL 컨텍스트 필요'),
        row(100,665,1000,'Material.texture: shared_ptr<const TextureImage>로 CPU 이미지 공유',
            '이 핵심 타입 간 C++ 상속 선언은 없음; setParent는 장면 변환 관계'),
    ], [('M600 98 V125 H220 V160','값으로 생성',240,117),
        ('M600 125 H980 V160','값으로 생성',800,117),
        ('M800 199 H400','비소유 참조',520,185),
        ('M220 238 V290 H147 V330','값 소유',35,286),
        ('M220 290 H447 V330','값 소유',340,280),
        ('M447 290 H747 V330','값 소유',640,280),
        ('M747 290 H1047 V330','실행 자산',930,280),
        ('M147 408 V455 H280 V500','컨테이너 소유',165,453),
        ('M747 408 V455 H897 V500','핸들 관리',777,453)], 790,
        ['standard/main.cpp','standard/Console.h','standard/framework/include/Application.h',
         'standard/framework/include/Graphics.h','standard/framework/include/Model.h'],
        '화살표의 소유·참조 표시를 구분한다. ID·parent·LayerId 연결은 메모리 소유권 이전이 아니다.')
    add('calls', '02 호출함·호출됨과 실제 프레임 루프', [
        row(390,15,420,'main: set…Handler → run(config)','람다 등록은 실행 예약'),
        row(390,135,420,'initialize → Renderer::initialize','initHandler → Console::initScene (1회)'),
        row(390,260,420,'계속 실행?', '닫기·실패면 정리',decision=True),
        row(870,270,310,'shutdown → run 반환','Renderer 정리 → 창·GLFW 종료'),
        row(390,410,420,'PollEvents → dispatch','키·마우스 콜백 → Console 입력 함수'),
        row(390,540,420,'framebuffer 양수?', '닫기·콜백 오류도 확인',decision=True),
        row(15,550,300,'WaitEvents','시간 기준 재설정 → 다음 반복'),
        row(390,690,420,'dt clamp → updateHandler','Console::onUpdate → Scene::updateMotions'),
        row(390,820,420,'ensureContext → renderHandler?', '등록 여부로 분기',decision=True),
        row(15,830,300,'등록됨: Console::onRender','renderScene / threeViews → renderViews'),
        row(870,830,310,'미등록: renderScene','기본 카메라로 단일 패스'),
        row(390,980,420,'Renderer::render / renderViews','renderPass → upload(필요 시) → draw'),
        row(390,1100,420,'detail::gl() → OpenGL 명령','DrawElements → CPU 호출 복귀'),
        row(390,1220,420,'렌더 성공 → SwapBuffers','다음 프레임; 실패·예외는 shutdown'),
    ], [('M600 93 V135','',0,0),('M600 213 V260','',0,0),
        ('M810 310 H870','아니오',815,298),('M600 360 V410','예',612,389),
        ('M600 488 V540','',0,0),('M390 590 H315','아니오',321,578),
        ('M15 589 H5 V310 H390','다음 반복',18,387),('M600 640 V690','예',612,669),
        ('M600 768 V820','',0,0),('M390 870 H315','예',335,856),
        ('M810 870 H870','아니오',814,856),('M165 908 V950 H600 V980','',0,0),
        ('M1025 908 V950 H600','',0,0),('M600 1058 V1100','',0,0),
        ('M600 1178 V1220','',0,0),('M600 1298 V1335 H345 V245 H600 V260','다음 반복',150,1320)], 1370,
        ['standard/main.cpp','standard/Console.cpp','standard/framework/detail/Application.cpp',
         'standard/framework/detail/Renderer.cpp','standard/framework/detail/Native.h'],
        'GPU 처리는 비동기다. GL 함수의 CPU 복귀는 GPU 완료를 뜻하지 않는다. 이벤트가 없으면 입력 콜백도 실행되지 않는다.')
    add('locals', '03 함수 지역 인스턴싱·반환·수명', [
        row(10,25,330,'함수 / 생성 위치','호출 시 지역 객체 구성'),
        row(420,25,350,'지역 객체','일시적 값과 빌린 참조 구분'),
        row(850,25,340,'반환·이전 대상','지역 스코프 종료 이후'),
        row(10,160,330,'main','run이 끝날 때까지 같은 스코프'),
        row(420,160,350,'Application app / Console sample','ApplicationConfig config'),
        row(850,160,340,'run → 정리 → main 반환','sample → app 역순 소멸'),
        row(10,300,330,'Renderer::render','단일 패스용 지역 값'),
        row(420,300,350,'RenderPass pass','camera/options/viewport를 값 복사'),
        row(850,300,340,'renderPass(scene, pass, …)','호출 중 const 참조로 사용'),
        row(10,440,330,'PatternSystem::threeViews','3개 패스 구성'),
        row(420,440,350,'RenderPass main / top / front','카메라·옵션·픽셀 영역 값'),
        row(850,440,340,'vector<RenderPass> 값 반환','renderViews 동안 임시 값 수명 유지'),
        row(10,580,330,'Renderer::upload','형상 revision 변경 시'),
        row(420,580,350,'vertices / indices / packed','ranges / stagedTextures'),
        row(850,580,340,'BufferData로 GPU 저장소에 복사','성공 시 ranges/texture 캐시 확정'),
        row(10,720,330,'addSwingWithRider','실패 시 원 Scene 유지'),
        row(420,720,350,'Scene staged = scene; SwingRig rig','Motion / 변환 / 관절 임시 값'),
        row(850,720,340,'성공: scene = move(staged)','rig 반환; ID가 객체 소유권은 아님'),
    ], [('M340 199 H420','생성',355,187),('M770 199 H850','수명',790,187),
        ('M340 339 H420','생성',355,327),('M770 339 H850','전달',790,327),
        ('M340 479 H420','생성',355,467),('M770 479 H850','반환',790,467),
        ('M340 619 H420','생성',355,607),('M770 619 H850','복사/이전',780,607),
        ('M340 759 H420','생성',355,747),('M770 759 H850','성공 시',785,747)], 840,
        ['standard/main.cpp','standard/framework/detail/Renderer.cpp','standard/framework/detail/Patterns.cpp',
         'standard/framework/detail/Animation.cpp'],
        '지역 CPU 컨테이너가 사라져도 업로드된 GPU 버퍼는 Renderer::destroy까지 유지된다. 참조·ID 반환은 객체 생성과 다르다.')
    add('layers', '04 엔티티 계층·모션 연결·변환·GPU 바인딩', [
        row(15,20,520,'Scene::setParent → Component_Poly.parent','루트 → 조상 → 대상; C++ 상속과 별개'),
        row(665,20,520,'MotionSystem: LayerId 부모 체인','bindMotion: 레이어 ↔ MotionId'),
        row(15,170,520,'Scene::bindEntity → entityLayers_','가장 가까운 조상 연결 지점(anchor) 선택'),
        row(665,170,520,'layerMatrices(leaf)','TransformSpace에 따라 parent / local 합성'),
        row(100,330,1000,'worldMatrix = motion.parent × prefix × motion.local × suffix',
            'prefix: 루트~anchor; suffix: anchor 다음~대상; L = T(basis)·R·S·T(-pivot)'),
        row(100,480,1000,'faceWorldMatrix = world × transformFrame × faceLocalMatrix',
            'Hierarchy면 worldMatrix; Independent면 I / 면 애니메이션은 Local·Model 순서 구분'),
        row(100,630,1000,'Renderer::renderPass → draw: model=M, view=V, projection=P',
            'GPU 바인딩: UseProgram + BindVertexArray + texture + Uniform(MVP=P·V·M)'),
        row(100,780,1000,'DrawElements: EBO 인덱스 → 정점 셰이더 gl_Position',
            '클리핑 → 원근 나눗셈 → viewport → 래스터화 → 프래그먼트 → 깊이/프레임버퍼'),
    ], [('M275 98 V170','부모 변환',290,143),('M925 98 V170','레이어 합성',940,143),
        ('M665 209 H535','ID 연결을 읽음',545,197),('M275 248 V285 H600 V330','',0,0),
        ('M925 248 V285 H600','',0,0),('M600 408 V480','면별 정책',615,453),
        ('M600 558 V630','최종 M',615,601),('M600 708 V780','명령 제출',615,751)], 920,
        ['standard/framework/detail/Scene.cpp','standard/framework/detail/Motion.cpp',
         'standard/framework/include/Model.h','standard/framework/detail/Renderer.cpp','standard/framework/detail/Shaders.h'],
        '엔티티 부모, 모션 레이어, GL 바인딩은 독립된 관계다. shader 이후 고정 단계는 OpenGL 파이프라인 설명이며 별도 C++ 함수 호출이 아니다.')
    add('summary', '종합: main에서 GPU까지, 다음 프레임과 종료까지', [
        row(350,20,500,'main: app·sample 지역 생성 → 콜백 등록','app이 Scene·Camera·Renderer를 소유'),
        row(350,160,500,'run 초기화 → initScene','CPU 형상·계층·모션·ID 구성'),
        row(350,300,500,'프레임: PollEvents → 입력 → dt 갱신','Console::onUpdate → Scene::updateMotions'),
        row(350,440,500,'onRender → renderScene / renderViews','지역 RenderPass → Renderer::renderPass'),
        row(350,580,500,'CPU: 필요 시 upload → draw','엔티티 부모 + 모션 레이어 + 면 변환 → M'),
        row(350,720,500,'바인딩: program·VAO·texture·uniform','MVP=P·V·M → DrawElements'),
        row(350,860,500,'GPU: vertex → fragment → framebuffer','CPU GL 호출 복귀와 GPU 완료는 별개'),
        row(350,1000,500,'렌더 성공: SwapBuffers → 다음 프레임','종료·실패: Renderer 정리 → 창·GLFW 해제'),
        row(890,1000,300,'run 반환 → main 반환','sample → app 소멸'),
    ], [('M600 98 V160','1 소유',615,135),('M600 238 V300','2 호출',615,275),
        ('M600 378 V440','3 지역 값',615,415),('M600 518 V580','',0,0),
        ('M600 658 V720','4 변환·연결',615,695),('M600 798 V860','GPU 제출',615,835),
        ('M600 938 V1000','표시',615,975),('M350 1039 H220 V339 H350','반복',230,696),
        ('M850 1039 H890','종료',851,1025)], 1130,
        ['standard/main.cpp','standard/Console.cpp','standard/framework/detail/Application.cpp',
         'standard/framework/detail/Renderer.cpp'],
        '한 실행 프로젝트 standard의 흐름이다. framework/examples의 독립 main을 이 실행 경로에 합치지 않는다.')
    return result


def source_link(directory, path):
    relative = os.path.relpath(ROOT / path, ROOT / directory).replace('\\', '/')
    return '<a href="' + esc(relative) + '">' + esc(path) + '</a>'


def refresh_diagrams():
    charts = diagrams()
    health_path = OUT / 'graphify-out/GRAPH_HEALTH.md'
    health = health_path.read_text(encoding='utf-8-sig') if health_path.exists() else ''
    missing = re.search(r'dangling_endpoint_edges: (\d+)', health)
    collapsed = re.search(r'directed_same_endpoint_collapsed_edges: (\d+)', health)
    for directory in DIRECTORIES:
        block = '<!-- API-ARCHITECTURE:BEGIN --><section id="api-architecture"><h2>API 코드베이스: 네 관계와 종합 흐름</h2>'
        block += '<p>정적 소스와 graphify AST 분석에 근거한다. 화살표의 소유·참조·호출·생성·ID 연결을 구분한다. 각 설명의 소스 링크에서 실제 구현을 읽는다.</p>'
        if missing and collapsed:
            block += '<p class="badge">graphify 추출 한계: 연결 대상 누락 ' + missing.group(1) + '건, 같은 노드 쌍의 관계 병합 ' + collapsed.group(1) + '건. 원본 간선은 EXTRACTION.json에 보존했다. 아래 순서도는 그래프만으로 확정하지 않고 실제 소스를 대조했다. 빌드·실행·브라우저 확인은 사용자에게 맡긴다.</p>'
        block += '<nav>'
        block += ''.join('<a href="#arch-' + key + '">' + esc(c['title']) + '</a>' for key, c in charts.items()) + '</nav>'
        for key, chart in charts.items():
            block += '<section id="arch-' + key + '"><h3>' + esc(chart['title']) + '</h3>' + chart['html']
            block += '<p>' + esc(chart['note']) + '</p><p class="badge">소스: '
            block += ' · '.join(source_link(directory, p) for p in chart['evidence']) + '</p></section>'
            if key == 'layers':
                block += '<p>코드 경계: Framework.h는 공개 헤더를 모으는 include 진입점이다. include/*.h의 선언 → detail/*.cpp의 구현 → detail/Native.h의 gl()/glfw() 함수 테이블 → GLFW/OpenGL 네이티브 함수로 내려간다. 헤더 include는 컴파일 의존성이며 실행 중 함수 호출이 아니다. 셰이더와 GPU 고정 단계는 그 이후의 처리다.</p>'
            svg = re.search(r'<svg[\s\S]*?</svg>', chart['html']).group()
            write(directory + '/flowcharts/api-' + key + '.svg', svg + '\n')
        block += '</section><!-- API-ARCHITECTURE:END -->'
        path = directory + '/API_REFERENCE.html'
        current = read(path)
        current = re.sub(r'<!-- API-ARCHITECTURE:BEGIN -->[\s\S]*?<!-- API-ARCHITECTURE:END -->', '', current)
        current = current.replace('</nav>', '</nav>' + block, 1)
        write(path, current)
    write(OUT / 'ARCHITECTURE.md', '# API 관계 분석\n\n' + '\n\n'.join(
        '## ' + c['title'] + '\n\n' + c['note'] + '\n\n' + '\n'.join('- `' + p + '`' for p in c['evidence'])
        for c in charts.values()) + '\n\n빌드·테스트·브라우저 확인은 사용자에게 맡긴다.\n')
    print('Wrote 4 relationship diagrams + 1 synthesis in 3 API documents.')


def build_graph():
    from graphify.detect import detect, save_manifest
    from graphify.extract import extract
    from graphify.build import build_from_json
    from graphify.cluster import cluster, score_all
    from graphify.analyze import god_nodes, surprising_connections, suggest_questions
    from graphify.export import to_json, to_html
    from graphify.report import generate
    from graphify.diagnostics import diagnose_extraction, format_diagnostic_report
    output = OUT / 'graphify-out'
    output.mkdir(parents=True, exist_ok=True)
    excluded = [p.name + '/**' for p in ROOT.iterdir() if p.is_dir() and p.name not in ('framework', 'standard')]
    excluded += ['**/dependencies/**','**/docs/**','**/x64/**','**/.vs/**','**/*.md','**/*.json','**/*.html','**/*.svg']
    detection = detect(ROOT, extra_excludes=excluded, cache_root=output / 'cache')
    files = sorted(p for base in ('framework', 'standard') for p in (ROOT / base).rglob('*')
                   if p.is_file() and p.suffix in ('.h', '.cpp', '.inl')
                   and not {'dependencies', 'x64', '.vs', 'docs'}.intersection(p.relative_to(ROOT).parts))
    detection.update(files={'code': [str(p) for p in files], 'document': [], 'paper': [], 'image': [], 'video': []},
                     total_files=len(files), total_words=sum(len(p.read_text(encoding='utf-8-sig').split()) for p in files))
    print(f'Corpus: {len(files)} code files, ~{detection["total_words"]} words; SDKs and unrelated projects excluded.', flush=True)
    if len(files) > 500 or detection['total_words'] > 2_000_000:
        raise RuntimeError('Corpus exceeds agreed scope; preserve previous graph.')
    if detection.get('skipped_sensitive'):
        print('Sensitive-name exclusions:', detection['skipped_sensitive'], flush=True)
    extraction = extract(files, root=ROOT, cache_root=output / 'cache')
    updater = load('api_graph_anchors', 'framework-analysis/update_graph.py')
    updater.add_source_anchors(extraction, files, ROOT)
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    if not graph.number_of_nodes():
        raise RuntimeError('Empty graph; preserve previous graph.')
    communities = cluster(graph)
    labels = {}
    for cid, members in communities.items():
        names = collections.Counter(Path(graph.nodes[n].get('source_file', '')).stem for n in members)
        labels[cid] = (names.most_common(1)[0][0] if names else '공통') + ' 관계'
    gods, surprises = god_nodes(graph), surprising_connections(graph, communities)
    questions = suggest_questions(graph, communities, labels)
    commit = subprocess.check_output(['git','rev-parse','HEAD'], cwd=ROOT, text=True).strip()
    to_json(graph, communities, str(output / 'graph.json'), force=True, community_labels=labels, built_at_commit=commit)
    to_html(graph, communities, str(output / 'graph.html'), community_labels=labels)
    report = generate(graph, communities, score_all(graph, communities), labels, gods, surprises, detection,
                      {'input': 0, 'output': 0}, str(ROOT), suggested_questions=questions, built_at_commit=commit)
    report += '\n## 범위와 한계\n\nframework와 standard/framework를 별도 경로로 추출했다. standard 실행은 standard/framework를 사용한다. 같은 이름의 C++ 메서드 간 AST 연결은 정확한 오버로드 해석이 아니다. 문서 순서도는 실제 소스를 추가로 읽어 작성했다. GPU 고정 단계는 C++ 호출 간선이 아니다. 빌드·테스트·브라우저 확인은 사용자에게 맡겼다.\n'
    write(output / 'GRAPH_REPORT.md', report)
    health = diagnose_extraction(extraction, directed=True, root=str(ROOT))
    write(output / 'GRAPH_HEALTH.md', format_diagnostic_report(health))
    dump(output / 'EXTRACTION.json', extraction)
    dump(output / 'DETECTION.json', detection)
    dump(output / 'ANALYSIS.json', {'gods': gods, 'surprises': surprises, 'questions': questions})
    dump(output / 'SOURCE_MANIFEST.json', {'scope': ['framework', 'standard'], 'source_commit': commit,
         'files': [p.relative_to(ROOT).as_posix() for p in files]})
    dump(output / 'cost.json', {'input_tokens': 0, 'output_tokens': 0, 'external_api_calls': 0, 'mode': 'code AST'})
    save_manifest({'code': [str(p) for p in files]}, str(output / 'manifest.json'), kind='ast', root=ROOT)
    for path in output.iterdir():
        if path.is_file() and path.suffix in ('.html','.md','.json'):
            value = path.read_text(encoding='utf-8-sig')
            if '\ufffd' in value:
                raise RuntimeError('Unicode output failure: ' + str(path))
            write(path, value)
    print(f'Graph: {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges. External token cost: 0.', flush=True)
    print(format_diagnostic_report(health), flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--graph', action='store_true')
    args = parser.parse_args()
    if args.graph:
        build_graph()
    refresh_diagrams()

```

## tools/Refresh-ApiCatalog.py

before: 없음

### after
```python
"""Refresh API ownership/call indexes and embedded source without broad guide rewrites."""
from pathlib import Path
import collections
import importlib.util
import html
import json
import re
import textwrap

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'framework-analysis/api-reference'
spec = importlib.util.spec_from_file_location('architecture', ROOT / 'tools/Refresh-ApiArchitecture.py')
architecture = importlib.util.module_from_spec(spec)
spec.loader.exec_module(architecture)
catalog = architecture.load('api_catalog', 'tools/Write-ApiKo.py')
esc, read, write, dump = architecture.esc, architecture.read, architecture.write, architecture.dump

PHASES = {
    'C00': ('계약·값 생성·공통 도우미', '실행 순서를 고정하지 않는 값 계산·검사·진단. 삭제된 복사는 호출 불가.'),
    'C01': ('초기화·생성·콜백 등록', 'main → run 초기화 → Console::initScene. 설정 함수는 갱신 도중에도 사용할 수 있다.'),
    'C02': ('입력·조회·종료 요청', 'PollEvents → dispatch → Console::onKey/onMouse. 조회 API는 다른 단계에서도 사용한다.'),
    'C03': ('프레임 갱신·장면 상태·모션', 'Console::onUpdate → Scene::updateMotions. 렌더 패스 수와 무관하게 모션은 프레임당 한 번 갱신.'),
    'C04': ('렌더링·변환 평가·GPU 바인딩', 'Console::onRender → renderScene/renderViews → Renderer::renderPass → draw → GL 명령.'),
    'C05': ('자원 정리·종료', 'run의 shutdown → Renderer::destroy/abandon → 창 파괴 → GLFW 종료 → run 반환.'),
}


def phase(a):
    owner, name = a['owner'], a['name']
    if '= delete' in a['signature']:
        return 'C00'
    if name.startswith('~') or (owner == 'Renderer' and name in ('destroy', 'abandon')):
        return 'C05'
    if owner == 'Diagnostics':
        return 'C00'
    if owner == 'Application':
        if name.startswith('get') or name in ('pickAt', 'pickAtCursor', 'requestClose'):
            return 'C02'
        if name in ('renderScene', 'renderViews'):
            return 'C04'
        return 'C01'
    if owner in ('Renderer', 'DrawUniformContext', 'CameraSystem'):
        return 'C01' if name in ('Renderer', 'initialize') else 'C04'
    if owner == 'AssetLoader':
        return 'C01'
    if name.startswith('get') or owner == 'CollisionSystem':
        return 'C02'
    if owner == 'Scene':
        if name in ('worldMatrix', 'faceWorldMatrix', 'lineWorldMatrix'):
            return 'C04'
        if name == 'Scene' or name.startswith(('add', 'push_')):
            return 'C01'
        return 'C03'
    if owner == 'AnimationSystem':
        return 'C01' if name.startswith(('add', 'create')) else 'C03'
    if owner == 'MotionSystem':
        return 'C01' if name.startswith(('create', 'pushLayer')) else 'C03'
    if owner in ('AnimationClock', 'Motion', 'RoomBody', 'CuboidMode'):
        return 'C03'
    if owner == 'SceneActions':
        return 'C01'
    if owner == 'PatternSystem':
        if name == 'threeViews':
            return 'C04'
        return 'C01' if name.startswith(('add', 'create')) else 'C03'
    return 'C00'


def state_owner(a):
    owner = a['owner']
    if owner == 'Application':
        return 'Application: 창·컨텍스트·Scene·Camera·Renderer·콜백의 수명'
    if owner == 'Scene':
        return 'Application.scene_: CPU 장면·형상·엔티티 ID·모션 연결'
    if owner == 'Renderer':
        return 'Application.renderer_: GPU 핸들·draw range·업로드 캐시'
    if owner == 'MotionSystem':
        return 'Scene.motions_: MotionState·LayerState와 ID'
    if owner == 'AssetLoader':
        return 'Application.assets_: 로딩 경로·기능 설정; 반환 모델은 CPU 값'
    if owner == 'DrawUniformContext':
        return 'draw 호출 중 빌린 Scene 참조·program 핸들; 소유권 이전 없음'
    if owner == 'SceneActions':
        return '호출자가 반환 MotionAction 소유; Scene/Camera는 참조 캡처'
    if owner in ('AnimationSystem', 'PatternSystem'):
        return '전달받은 Scene/Camera의 소유자; static 도우미 자체는 대상 상태를 소유하지 않음'
    if owner == 'Diagnostics':
        return 'Diagnostics의 정적 진단 상태; Application 인스턴스 멤버가 아님'
    if owner in ('TransformSystem', 'PathSystem', 'CameraSystem', 'CollisionSystem', 'DiagramSystem'):
        return '호출자가 입력·반환값 소유; static 계산 도우미'
    return (owner or '호출자') + ': 해당 값 인스턴스 또는 호출자가 소유한 반환값'


def definition_calls(base):
    """Resolve only explicit class names and known receiver names; retain source syntax."""
    definitions, incoming = collections.defaultdict(list), collections.defaultdict(list)
    files = [ROOT / 'standard/main.cpp', ROOT / 'standard/Console.cpp'] if base == 'standard/framework' else []
    files += sorted(p for p in (ROOT / base).rglob('*') if p.suffix in ('.h', '.cpp')
                    and 'dependencies' not in p.parts and p.is_file())
    receivers = {'app': 'Application', 'app_': 'Application', 'current': 'Application',
                 'scene': 'Scene', 'scene_': 'Scene', 'staged': 'Scene', 'renderer_': 'Renderer',
                 'motions_': 'MotionSystem'}
    for path in files:
        data = path.read_bytes()
        def text(n):
            return data[n.start_byte:n.end_byte].decode('utf-8') if n else ''
        def find(n):
            if n.type == 'function_declarator':
                return n
            return next((v for c in n.named_children if c.type not in ('parameter_list', 'compound_statement')
                         if (v := find(c)) is not None), None)
        def walk(node, owner=''):
            if node.type in ('struct_specifier', 'class_specifier'):
                owner = text(node.child_by_field_name('name'))
            if node.type == 'function_definition':
                declarator = find(node)
                if declarator is None:
                    return
                name = text(declarator.child_by_field_name('declarator'))
                if '::' not in name and owner:
                    name = owner + '::' + name
                name = name.removeprefix('graphics::')
                function_owner = name.rsplit('::', 1)[0] if '::' in name else owner
                calls = []
                def scan(n, deferred=False):
                    deferred = deferred or n.type == 'lambda_expression'
                    if n.type == 'call_expression':
                        function = n.child_by_field_name('function')
                        syntax = text(function)
                        target = syntax.removeprefix('graphics::') if re.fullmatch(r'(?:graphics::)?\w+::\w+', syntax) else None
                        if function and function.type == 'field_expression':
                            argument = text(function.child_by_field_name('argument'))
                            field = text(function.child_by_field_name('field'))
                            if argument in receivers:
                                target = receivers[argument] + '::' + field
                        if not target and re.fullmatch(r'\w+', syntax) and function_owner:
                            target = function_owner + '::' + syntax
                        item = dict(target=target, syntax=syntax, file=path.relative_to(ROOT).as_posix(),
                                    line=n.start_point.row + 1, deferred=deferred)
                        calls.append(item)
                        if target:
                            incoming[target].append(dict(caller=name, **item))
                    for c in n.named_children:
                        scan(c, deferred)
                body = node.child_by_field_name('body')
                if body:
                    scan(body)
                definitions[name].append(dict(file=path.relative_to(ROOT).as_posix(),
                                             line=node.start_point.row + 1, calls=calls))
                return
            for c in node.named_children:
                walk(c, owner)
        walk(catalog.PARSER.parse(data).root_node)
    return definitions, incoming


def location(directory, item):
    return architecture.source_link(directory, item['file']) + ':L' + str(item['line'])


def declaration_block(record):
    source = read(record['file'])
    data = source.encode('utf-8')
    def find(node):
        if node.start_point.row + 1 == record['line'] and node.type in (
                'declaration', 'field_declaration', 'function_definition'):
            return node
        return next((found for child in node.named_children if (found := find(child)) is not None), None)
    node = find(catalog.PARSER.parse(data).root_node)
    if node is None:
        raise RuntimeError('Declaration source missing: ' + record['file'])
    lines = source.splitlines()
    first = record['line'] - 1
    while first and lines[first - 1].lstrip().startswith('//'):
        first -= 1
    return textwrap.dedent('\n'.join(lines[first:node.end_point.row + 1]))


def refresh_sources(text, base):
    def replace(match):
        original = match.group(1)
        path = original
        if path.startswith('framework/'):
            path = base + path[len('framework'):]
        if not (ROOT / path).is_file() and path.endswith('.inl'):
            path = str(Path(path).with_suffix('.cpp')).replace('\\', '/')
        if not (ROOT / path).is_file():
            return match.group()
        return '<details><summary>실제 저장소 소스 · ' + esc(path) + '</summary>' + architecture.guide.code(read(path)) + '</details>'
    return re.sub(r'<details><summary>실제 저장소 소스 · ([^<]+)</summary><pre><code>[\s\S]*?</code></pre></details>', replace, text)


def main():
    count = 0
    for directory in architecture.DIRECTORIES:
        base = 'standard/framework' if directory.startswith('standard/') else 'framework'
        records = []
        for path in sorted((ROOT / base / 'include').glob('*.h')):
            for a in catalog.declarations(path):
                a.update(description=catalog.description(a), result=catalog.result_contract(a))
                records.append(a)
        previous = json.loads(read(directory + '/API_CASE_MAP.json'))
        key = lambda a: (a['owner'], a['name'], a['signature'])
        old = {key(a): a for a in previous}
        if set(old) != set(map(key, records)):
            raise RuntimeError('Declaration inventory changed; reconcile API anchors before authoring.')
        definitions, incoming = definition_calls(base)
        current = read(directory + '/API_REFERENCE.html')
        existing = {int(m.group(1)): m.group() for m in re.finditer(
            r'<article class="api-entry" id="api-(\d+)">[\s\S]*?</article>', current)}
        # Several owners share signatures such as clear(); include the heading too.
        article_keys = {}
        for idx, article in existing.items():
            qualified = html.unescape(re.search(r'<h3>([\s\S]*?)</h3>', article).group(1))
            qualified = re.sub(r'^C\d+\.\d+ ', '', qualified)
            signature = re.sub(r'\s+', ' ', html.unescape(re.search(
                r'<pre><code>([\s\S]*?)</code></pre>', article).group(1)).strip().rstrip(';'))
            article_keys[qualified, signature] = idx
        ids = {key(a): int(a['api_id'].split('-')[1]) if a.get('api_id') else
               article_keys[((a['owner'] + '::') if a['owner'] else '') + a['name'], a['signature']]
               for a in previous}
        block = '<!-- API-CATALOG:BEGIN --><section id="api"><h2>전체 API: 호출 단계 → 상태 소유자 → 선언</h2>'
        block += '<p>인덱스 C01~C05는 호출 목적별 탐색 번호이며 모든 함수가 이 순서로 자동 실행된다는 뜻은 아니다. C00은 공통 계약·도우미다. 오버로드는 별도 항목이며 기존 api-N 링크는 유지한다.</p>'
        block += '<p>선언 소속 클래스와 실제 상태 소유자를 구분한다. 소유권·참조·ID 계약은 각 선언 설명을 따른다. 아래 호출 위치는 명시한 타입/수신자 구문에서 추출했으며 오버로드 선택·함수 포인터·동적 호출 전체를 증명하지 않는다. 지연 람다 본문도 따로 표시한다.</p>'
        block += '<nav>' + ''.join('<a href="#phase-' + p + '">' + p + ' ' + title + '</a>' for p, (title, _) in PHASES.items()) + '</nav>'
        block += '<h3>실제 standard 호출 단위</h3><ol><li>C01: main → run.initialize → Console::initScene</li>'
        block += '<li>C02: PollEvents → dispatch → Console::onKey/onMouse</li><li>C03: Console::onUpdate → Scene::updateMotions</li>'
        block += '<li>C04: Console::onRender → renderScene/renderViews → renderPass → draw</li><li>C05: shutdown → Renderer 정리 → GLFW 종료</li></ol>'
        block += '<h3>소유·선언 소속 인덱스</h3><nav>'
        owner_phases = collections.defaultdict(list)
        for a in records:
            owner_phases[a['owner'] or '자유 함수'].append(phase(a))
        for owner, phases in sorted(owner_phases.items()):
            block += '<span>' + esc(owner) + ': ' + ' / '.join('<a href="#owner-' + p + '-' + esc(owner) + '">' + p + '</a>' for p in sorted(set(phases))) + '</span> '
        block += '</nav><label for="api-search">함수·소유자·호출 번호 검색</label><input id="api-search" placeholder="예: C04, Renderer, setParent"></section>'
        inventory = []
        for p, (title, context) in PHASES.items():
            group = sorted((a for a in records if phase(a) == p), key=lambda a: (a['owner'], ids[key(a)]))
            block += '<section class="api-phase" id="phase-' + p + '"><h2>' + p + ' ' + esc(title) + '</h2><p>' + esc(context) + '</p>'
            owner = None
            for number, a in enumerate(group, 1):
                qualified = (a['owner'] + '::' if a['owner'] else '') + a['name']
                index, idx = p + '.' + str(number).zfill(3), ids[key(a)]
                if a['owner'] != owner:
                    owner = a['owner']
                    block += '<h3 id="owner-' + p + '-' + esc(owner or '자유 함수') + '">' + esc(owner or '자유 함수') + '</h3>'
                article = existing[idx]
                article = re.sub(r'<h3>[\s\S]*?</h3>', '<h3>' + index + ' ' + esc(qualified) + '</h3>', article, count=1)
                article = re.sub(r'<p class="badge">선언 위치:[\s\S]*?</p>', '', article)
                article = re.sub(r'<!-- API-METADATA:BEGIN -->[\s\S]*?<!-- API-METADATA:END -->', '', article)
                article = re.sub(r'<pre><code>[\s\S]*?</code></pre>',
                                 lambda _: architecture.guide.code(declaration_block(a)), article, count=1)
                meta = '<!-- API-METADATA:BEGIN --><dl><dt>호출 단위</dt><dd>' + index + ' · ' + esc(title) + '</dd>'
                meta += '<dt>선언 소속</dt><dd>' + esc(a['owner'] or '자유 함수') + '</dd><dt>상태 소유·대상</dt><dd>' + esc(state_owner(a)) + '</dd>'
                meta += '<dt>반환·실패 계약</dt><dd>' + esc(a['result']) + '</dd><dt>현재 선언</dt><dd>' + location(directory, a) + '</dd></dl>'
                meta += '<details><summary>호출됨 / 호출함 · 실제 소스 위치</summary>'
                callers = incoming.get(qualified, [])
                meta += '<p>호출됨: ' + ('다음 소스 구문에서 확인(오버로드별 분리 아님).' if callers else '추출 범위에서 명시적 수신자 호출을 찾지 못함. 호출 불가를 뜻하지 않음.') + '</p><ul>'
                for item in callers:
                    meta += '<li>' + esc(item['caller']) + (' [지연 람다 본문]' if item['deferred'] else '') + ' → ' + esc(item['syntax']) + ' · ' + location(directory, item) + '</li>'
                meta += '</ul><p>호출함: 구현 정의별 호출 구문. 람다 본문은 등록 시 즉시 실행되지 않는다.</p>'
                implementations = definitions.get(qualified, [])
                if not implementations:
                    meta += '<p>정의 연결 미확인 또는 default/delete 선언. 현재 선언·사례 코드를 참조한다.</p>'
                for implementation in implementations:
                    meta += '<details><summary>정의 · ' + esc(implementation['file']) + ':L' + str(implementation['line']) + '</summary><ul>'
                    for item in implementation['calls']:
                        meta += '<li>' + esc(item['syntax']) + (' [지연 람다 본문]' if item['deferred'] else '') + ' · ' + location(directory, item) + '</li>'
                    meta += '</ul></details>'
                meta += '</details><!-- API-METADATA:END -->'
                article = article.replace('</h3>', '</h3>' + meta, 1)
                block += article
                inventory.append({**old[key(a)], **a,
                                  'api_id': 'api-' + str(idx), 'call_index': index, 'call_phase': p,
                                  'state_owner': state_owner(a), 'called_by': callers,
                                  'definitions': implementations})
            block += '</section>'
        block += '<script>document.getElementById("api-search").addEventListener("input",e=>{const q=e.target.value.toLowerCase();document.querySelectorAll(".api-entry").forEach(n=>n.hidden=!n.textContent.toLowerCase().includes(q));});</script><!-- API-CATALOG:END -->'
        if '<!-- API-CATALOG:BEGIN -->' in current:
            current = re.sub(r'<!-- API-CATALOG:BEGIN -->[\s\S]*?<!-- API-CATALOG:END -->', lambda _: block, current)
        else:
            current, replacements = re.subn(r'<section id="api">[\s\S]*?<script>document.getElementById\(\'api-search\'\)[\s\S]*?</script>', lambda _: block, current, count=1)
            if replacements != 1:
                raise RuntimeError('API catalog boundaries missing: ' + directory)
        current = refresh_sources(current, base)
        write(directory + '/API_REFERENCE.html', current)
        # Preserve the original declaration order so existing api-N anchors survive regeneration.
        inventory.sort(key=lambda a: int(a['api_id'].split('-')[1]))
        dump(directory + '/API_CASE_MAP.json', inventory)
        for page in sorted((ROOT / directory).glob('*.html')):
            if page.name == 'API_REFERENCE.html':
                continue
            original = page.read_text(encoding='utf-8-sig')
            updated = refresh_sources(original, base)
            if original != updated:
                write(page, updated)
        count = len(records)
    write(OUT / 'CATALOG.md', '# API 분류와 갱신 범위\n\n' + f'- 공개 선언: {count}개 / 문서 3곳.\n' +
          '- 기존 api-N 앵커와 오버로드 개수를 유지했다. 호출 단계 C00~C05 아래 상태 소유·선언 소속별로 정렬했다.\n' +
          '- 실제 소스 블록은 문서가 속한 framework 사본에서 다시 읽는다. 선언 줄 번호와 JSON도 갱신했다.\n' +
          '- 호출 구문 목록은 제한된 정적 연결이며 오버로드·동적 호출을 완전히 해석하지 않는다.\n' +
          '- 빌드·테스트·브라우저·IDE hover 확인은 사용자에게 맡긴다.\n')
    print('Refreshed', count, 'API declarations in 3 document locations, plus embedded source blocks.')


if __name__ == '__main__':
    main()

```

## tools/author_framework_guides.py

### before
```python

def main():
    entries=apis()
    intro='<p><a href="MODE_ACTIONS.md">의미 동작·육면체 모드</a> · <a href="RENDER_CONFIGURATION.md">정점 입력·내부 수정 순서</a> · <a href="PATTERN_COMPOSITION.md">패턴 조립·API 압축</a></p>'+para('함수 이름을 외우는 대신 실제 상황에서 무엇을 저장하고 무엇을 바꾸는지 읽는다. 공개 함수·오버로드·생성자·소멸자·삭제된 복사 연산을 현재 framework/include 헤더에서 수록했다. graphics::detail 및 private 구현은 공개 호출 대상으로 나열하지 않고 공통 구현 설명에서 다룬다.')
    intro+='<p class="note">모든 코드는 현재 선언·구현에 근거한 문서다. 새 사용 사례는 함께 쓸 API를 묶은 함수 단편이며, 독립 main인 예제는 별도로 표시한다. 실행 성공을 확인한 코드라는 뜻은 아니다. 외부 자산 경로는 사용자가 준비한다.</p>'
    body=intro+runtime()+'<section id="cases"><h2>함께 사용하는 API의 실제 사례 코드</h2>'+para('기본 포함은 framework/Framework.h다. 출력 사례는 iostream, 예외 처리는 stdexcept를 추가한다. 아래 독립 함수들은 graphics 이름을 완전히 적었고, 입력 Scene/ID/컨텍스트의 전제는 각 사례에 명시한다.')+'</section>'
```

### after
```python

def main():
    entries=apis()
    previous_path = ROOT / 'framework/docs/API_CASE_MAP.json'
    previous = json.loads(read('framework/docs/API_CASE_MAP.json')) if previous_path.exists() else []
    stable_ids = {(a['owner'], a['name'], a['signature']): int(a['api_id'].split('-')[1])
                  for a in previous if a.get('api_id')}
    next_id = max(stable_ids.values(), default=-1) + 1
    intro='<p><a href="MODE_ACTIONS.md">의미 동작·육면체 모드</a> · <a href="RENDER_CONFIGURATION.md">정점 입력·내부 수정 순서</a> · <a href="PATTERN_COMPOSITION.md">패턴 조립·API 압축</a></p>'+para('함수 이름을 외우는 대신 실제 상황에서 무엇을 저장하고 무엇을 바꾸는지 읽는다. 공개 함수·오버로드·생성자·소멸자·삭제된 복사 연산을 현재 framework/include 헤더에서 수록했다. graphics::detail 및 private 구현은 공개 호출 대상으로 나열하지 않고 공통 구현 설명에서 다룬다.')
    intro+='<p class="note">모든 코드는 현재 선언·구현에 근거한 문서다. 새 사용 사례는 함께 쓸 API를 묶은 함수 단편이며, 독립 main인 예제는 별도로 표시한다. 실행 성공을 확인한 코드라는 뜻은 아니다. 외부 자산 경로는 사용자가 준비한다.</p>'
    body=intro+runtime()+'<section id="cases"><h2>함께 사용하는 API의 실제 사례 코드</h2>'+para('기본 포함은 framework/Framework.h다. 출력 사례는 iostream, 예외 처리는 stdexcept를 추가한다. 아래 독립 함수들은 graphics 이름을 완전히 적었고, 입력 Scene/ID/컨텍스트의 전제는 각 사례에 명시한다.')+'</section>'
```
### before
```python
    body+='<section id="api"><h2>전체 공개 API → 사례 연결</h2>'+para(f'현재 헤더에서 수록한 선언 항목은 {len(entries)}개다. 각 오버로드는 선언을 따로 표시하고 같은 사례에서 함께 설명한다. 삭제된 복사 연산은 호출 예제를 만드는 대신 금지 이유를 설명한다.')+'<label for="api-search">함수·클래스·설명 검색</label><input id="api-search" placeholder="예: setParent, Motion, 좌석"></section>'
    inventory=[];missing=[]
    for idx,a in enumerate(entries):
        key=choose(a);g=GROUPS[key];name=a['name'];qualified=(a['owner']+'::' if a['owner'] else '')+name
        lines=[line.strip() for line in g['code'].splitlines() if re.search(r'\b'+re.escape(name)+r'\s*\(',line)]
        deleted='= delete' in a['signature']
```

### after
```python
    body+='<section id="api"><h2>전체 공개 API → 사례 연결</h2>'+para(f'현재 헤더에서 수록한 선언 항목은 {len(entries)}개다. 각 오버로드는 선언을 따로 표시하고 같은 사례에서 함께 설명한다. 삭제된 복사 연산은 호출 예제를 만드는 대신 금지 이유를 설명한다.')+'<label for="api-search">함수·클래스·설명 검색</label><input id="api-search" placeholder="예: setParent, Motion, 좌석"></section>'
    inventory=[];missing=[]
    for idx,a in enumerate(entries):
        identity = (a['owner'], a['name'], a['signature'])
        if stable_ids:
            if identity in stable_ids:
                idx = stable_ids[identity]
            else:
                idx = next_id
                next_id += 1
        key=choose(a);g=GROUPS[key];name=a['name'];qualified=(a['owner']+'::' if a['owner'] else '')+name
        lines=[line.strip() for line in g['code'].splitlines() if re.search(r'\b'+re.escape(name)+r'\s*\(',line)]
        deleted='= delete' in a['signature']
```
### before
```python
        body+=para('사례 맥락: '+g['story'])+'<p><a href="#case-'+key+'">'+esc(g['title'])+' — 입력·관련 변수·전체 호출 묶음</a></p>'
        body+=code('\n'.join(lines)) if lines else para('이 함수의 호출 위치는 아래 연결된 전체 사례와 원본 선언에서 읽는다.')
        body+=para('호출 뒤 확인할 점: '+g['after'])+f'<p class="badge">선언 위치: {esc(a["file"])}:{a["line"]} · 반환형·기본값·const 여부는 위 선언을 따른다.</p></article>'
        inventory.append(dict(**a,case=key,example_lines=lines,callable=not deleted))
    body+='''<script>document.getElementById('api-search').addEventListener('input',e=>{const q=e.target.value.toLowerCase();document.querySelectorAll('.api-entry').forEach(n=>n.hidden=!n.textContent.toLowerCase().includes(q));});</script>'''
    api_html=page('공통 구현과 전체 API 사례 코드',body)
    patterns=intro+para('응용패턴은 목적 → 변수 역할 → 흐름도 → 응용 코드 → 예상 결과 순으로 짧게 읽는다. 반복되는 렌더링 루프·GPU 자원 구현은 API 문서의 공통 구현 절에서 자세히 설명한다.')
```

### after
```python
        body+=para('사례 맥락: '+g['story'])+'<p><a href="#case-'+key+'">'+esc(g['title'])+' — 입력·관련 변수·전체 호출 묶음</a></p>'
        body+=code('\n'.join(lines)) if lines else para('이 함수의 호출 위치는 아래 연결된 전체 사례와 원본 선언에서 읽는다.')
        body+=para('호출 뒤 확인할 점: '+g['after'])+f'<p class="badge">선언 위치: {esc(a["file"])}:{a["line"]} · 반환형·기본값·const 여부는 위 선언을 따른다.</p></article>'
        inventory.append(dict(**a,case=key,example_lines=lines,callable=not deleted,api_id='api-'+str(idx)))
    body+='''<script>document.getElementById('api-search').addEventListener('input',e=>{const q=e.target.value.toLowerCase();document.querySelectorAll('.api-entry').forEach(n=>n.hidden=!n.textContent.toLowerCase().includes(q));});</script>'''
    api_html=page('공통 구현과 전체 API 사례 코드',body)
    patterns=intro+para('응용패턴은 목적 → 변수 역할 → 흐름도 → 응용 코드 → 예상 결과 순으로 짧게 읽는다. 반복되는 렌더링 루프·GPU 자원 구현은 API 문서의 공통 구현 절에서 자세히 설명한다.')
```
### before
```python
    refresh_cube_docs()
    refresh_doc_flowcharts()
    refresh_size_report()
```

### after
```python
    refresh_cube_docs()
    refresh_doc_flowcharts()
    refresh_size_report()
    # Keep the ownership/call index and source snapshots when regenerating guides.
    import runpy
    runpy.run_path(str(ROOT / 'tools/Refresh-ApiArchitecture.py'), run_name='api_architecture_refresh')['refresh_diagrams']()
    runpy.run_path(str(ROOT / 'tools/Refresh-ApiCatalog.py'), run_name='api_catalog_refresh')['main']()
```
