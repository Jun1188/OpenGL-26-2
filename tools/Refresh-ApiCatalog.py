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
    # Preserve the current assembly API and regenerate offline document navigation.
    import runpy
    runpy.run_path(str(ROOT / 'tools/Update-StandardDocs.py'))['main']()
    print('Refreshed', count, 'API declarations in 3 document locations, plus embedded source blocks.')


if __name__ == '__main__':
    main()
