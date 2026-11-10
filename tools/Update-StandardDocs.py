"""Refresh current assembly API notes and offline standard document navigation. No build/tests."""
from pathlib import Path
import html
import json
import os
import re
import time
ROOT = Path(__file__).resolve().parents[1]
STANDARD = ROOT / 'standard'
START = '<!-- current-assembly-api:start -->'
END = '<!-- current-assembly-api:end -->'
def write(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    # Large HTML files are sometimes briefly locked on Windows (scanner/preview); retry before failing.
    for attempt in range(5):
        try:
            path.write_text(value, encoding='utf-8-sig')
            return
        except OSError:
            if attempt == 4: raise
            time.sleep(.5)
def code(value): return '<pre><code>'+html.escape(value)+'</code></pre>'
def excerpt(path, start, end=None):
    text=path.read_text(encoding='utf-8-sig')
    begin=text.index(start)
    stop=text.index(end,begin) if end else len(text)
    return text[begin:stop], text[:begin].count('\n')+1

def refresh_api():
    for relative in ('framework/docs/API_REFERENCE.html','standard/docs/API_REFERENCE.html','standard/framework/docs/API_REFERENCE.html'):
        page=ROOT/relative
        base=ROOT/('framework' if relative.startswith('framework/') else 'standard/framework')
        def link(path,label):
            return '<a href="'+html.escape(os.path.relpath(path,page.parent).replace('\\','/'))+'">'+html.escape(label)+'</a>'
        block='<section id="current-assembly-api"><h2>객체 상속·지역 레이어·응용 물리</h2>'
        block+='<p>공통 C++ API와 Standard 명령 파서를 분리했다. Standard는 0번 작업공간이며 과거 실습 컨테이너는 소스 참조다. 빌드·테스트는 사용자가 수행한다.</p>'
        block+='<p>'+link(base/'docs/WORKSPACE_API.md','공통 API 계약')+' · '+link(STANDARD/'docs/QUICK_COMMANDS.md','명령어 [결과] - 인수 의미')+' · '+link(STANDARD/'docs/TRANSFORM_WORKSPACE.md','월드 배치와 지역 레이어')+' · '+link(STANDARD/'docs/APPLICATION_RECIPES.md','조명·카메라·회전 저울')+'</p>'
        block+=code('W_child = W_parent * attachment * baseSRT * L1 * L2 * ... * Ln')
        block+='<p>월드 배치를 부모 기준 부착 행렬로 저장한다. 객체가 지역 레이어 목록을 소유하고 앞 레이어를 뒤 레이어가 상속한다. 비균등 배율·전단 결과를 SRT로 재분해하지 않는다.</p>'
        block+='<table><tr><th>API</th><th>계약</th></tr>'
        for api,desc in [
            ('Scene::placementMatrix','자기 지역 레이어 이전의 월드 배치'),
            ('Scene::setWorldPlacement','부모 역행렬로 부착 배치를 계산하고 자기 레이어 유지'),
            ('Scene::setParentPreserveWorld','순환 검사, 월드 배치 보존, 실패 시 이전 관계 유지'),
            ('Scene::setAttachment / setLocalLayers','객체 소유 아핀 부착과 순서 있는 지역 행렬 목록'),
            ('PointLight / RenderOptions','월드 점광원 최대8, 색·강도·거리 감쇠'),
            ('Material','Ka/Kd/Ks·shininess·unlit; 메시별 재질 응답과 비조명 표시'),
            ('RenderOptions::ambientColor','방향광과 독립된 주변광 색, 기본 0 0 0'),
            ('CameraSystem::attached','지역 카메라를 부모 프레임에 부착하고 시선/up을 직교화'),
            ('DynamicsSystem::effectiveAcceleration / step','중력·이동·원심·코리올리·오일러 항과 지역 힘 콜백'),
            ('DynamicsSystem::holdingForce / normalLoad','정지 구속력과 법선 압축력. 실제 접촉 솔버가 아님'),
            ('DynamicsSystem::spring','지역 앵커·강성·감쇠로 선형 힘 콜백 생성')]:
            block+='<tr><td><code>'+html.escape(api)+'</code></td><td>'+html.escape(desc)+'</td></tr>'
        block+='</table><h3>Dynamics 공개 선언</h3>'+code((base/'include/Dynamics.h').read_text(encoding='utf-8-sig'))
        if relative=='standard/docs/API_REFERENCE.html':
            block+='<h3>Standard Assembly 공개 선언</h3>'
            text,line=excerpt(STANDARD/'modes/Assembly.h','public:\n','\n};')
            block+=code(text)+'<p>'+link(STANDARD/'modes/Assembly.h','Assembly.h:'+str(line))+'</p>'
            block+='<p>명령 실행 실패는 CPU Scene·작성 상태·카메라·렌더 옵션을 복원한다. save는 대상과 레이어를 명시한 작성 레시피이며 현재 적분 프레임 스냅샷이 아니다.</p>'
        block+='</section>'
        current=page.read_text(encoding='utf-8-sig')
        current=re.sub(re.escape(START)+r'.*?'+re.escape(END),'',current,flags=re.S)
        insertion=START+block+END
        current=re.sub(r'(<body\b[^>]*>)',lambda m:m[0]+insertion,current,count=1)
        write(page,current)

VIEWER_TEMPLATES = ROOT / 'tools/templates/standard-viewer'
CATALOG = STANDARD / 'docs/DOCUMENT_CATALOG.json'
SCENARIOS = STANDARD / 'examples/assembly/scenarios'
NAV_START = '<!-- standard-nav:start -->'
NAV_END = '<!-- standard-nav:end -->'

def js_data(name, value):
    data = json.dumps(value, ensure_ascii=False).replace('<', '\\u003c').replace('\u2028', '\\u2028').replace('\u2029', '\\u2029')
    return 'window.' + name + ' = ' + data + ';\n'

def collect_documents():
    docs = []
    for p in sorted(STANDARD.rglob('*')):
        rel = p.relative_to(STANDARD)
        if not p.is_file() or p.suffix.lower() not in ('.md', '.html'): continue
        if any(part in ('.vs', 'x64', 'dependencies', 'viewer') for part in rel.parts): continue
        if rel.as_posix() in ('index.html', 'document.html'): continue
        text = p.read_text(encoding='utf-8-sig')
        if p.suffix.lower() == '.md':
            match = re.search(r'^#\s+(.+)', text, re.M)
        else: match = re.search(r'<title[^>]*>(.*?)</title>', text, re.S | re.I)
        docs.append({'path': rel.as_posix(), 'title': html.unescape(match[1].strip()) if match else p.stem,
                     'markdown': text if p.suffix.lower() == '.md' else None})
    return docs

def load_catalog(docs):
    """Validate DOCUMENT_CATALOG.json and append the documents it does not list."""
    catalog = json.loads(CATALOG.read_text(encoding='utf-8-sig'))
    if catalog.get('schemaVersion') != 1: raise SystemExit('DOCUMENT_CATALOG.json: schemaVersion 1 expected')
    paths = {d['path'] for d in docs}
    listed = set()
    for category in catalog['categories']:
        for entry in category['documents']:
            if entry['path'] not in paths: raise SystemExit('DOCUMENT_CATALOG.json: missing document ' + entry['path'])
            if entry['path'] in listed: raise SystemExit('DOCUMENT_CATALOG.json: listed twice ' + entry['path'])
            listed.add(entry['path'])
    rest = [{'path': d['path']} for d in docs if d['path'] not in listed]
    if rest: catalog['categories'].append({**catalog['uncategorized'], 'documents': rest})
    return catalog

def collect_scenarios():
    """Scenario cards come from the '# @key: value' headers; each command needs a '#: note' line above it."""
    scenarios = []
    for p in sorted(SCENARIOS.glob('*.assembly')):
        if p.stem.endswith('features-mix'): continue # Reference-only legacy controller composition.
        meta, commands, note = {}, [], None
        for line in p.read_text(encoding='utf-8-sig').splitlines():
            header = re.match(r'#\s*@(\w+):\s*(.*)', line)
            if header: meta[header[1]] = header[2].strip()
            elif line.startswith('#:'): note = line[2:].strip()
            elif line.strip() and not line.startswith('#'):
                if not note: raise SystemExit(p.name + ': missing "#:" note above: ' + line)
                commands.append({'command': line, 'note': note})
                note = None
        if 'title' not in meta: raise SystemExit(p.name + ': # @title header required')
        scenarios.append({'id': p.stem, 'file': p.relative_to(STANDARD).as_posix(), **meta,
                          'commands': '\n'.join(c['command'] for c in commands), 'lines': commands})
    return scenarios

def write_document_index(docs, catalog):
    """Markdown version of the catalog; keeps the flowchart block that author_framework_guides.py maintains."""
    titles = {d['path']: d['title'] for d in docs}
    index = STANDARD / 'DOCUMENT_INDEX.md'
    old = index.read_text(encoding='utf-8-sig') if index.exists() else '# Standard 문서 목차\n\n'
    begin, end = '<!-- flowchart-index:start -->', '<!-- flowchart-index:end -->'
    flow = old[old.index(begin):old.index(end) + len(end)] if begin in old and end in old else ''
    lines = ['[브라우저 목차](index.html) · [C++ 예제](docs/EXAMPLES.md) · [README](README.md)', '',
             '# standard 문서 목차', '',
             '분류와 순서는 docs/DOCUMENT_CATALOG.json에서 정한다. 이 파일과 index.html은 `python tools/Update-StandardDocs.py`가 함께 다시 만든다.', '']
    for category in catalog['categories']:
        lines += ['## ' + category['title'], '', category['description'], '']
        for entry in category['documents']:
            path = entry['path']
            summary = ' — ' + entry['summary'] if entry.get('summary') else ''
            lines.append('- [' + entry.get('title', titles[path]) + '](' + path + ')' + summary)
        lines.append('')
    if flow: lines += ['## 흐름도', '', flow, '']
    write(STANDARD / 'DOCUMENT_INDEX.md', '\n'.join(lines))

def add_navigation(docs, catalog):
    """Give standalone HTML documents the viewer's top bar. Idempotent through the markers."""
    category_of = {e['path']: c for c in catalog['categories'] for e in c['documents']}
    for d in docs:
        if d['markdown'] is not None or d['path'] in ('docs/EXAM_SCENARIOS.html', 'docs/assembly/architecture/graph.html'): continue
        page = STANDARD / d['path']
        up = os.path.relpath(STANDARD, page.parent).replace('\\', '/')
        category = category_of.get(d['path'])
        crumb = ('<a href="' + up + '/index.html#cat-' + category['id'] + '" style="color:#cfe3ff;text-decoration:none">'
                 + html.escape(category['title']) + '</a>') if category else ''
        bar = (NAV_START + '<nav style="position:sticky;top:0;z-index:50;display:flex;gap:18px;flex-wrap:wrap;align-items:center;'
               'padding:10px 24px;background:#142c43;color:#fff;font:15px/1.6 \'Malgun Gothic\',sans-serif">'
               '<a href="' + up + '/index.html" style="color:#fff;font-weight:700;text-decoration:none">← 전체 문서</a>' + crumb +
               '<span style="flex:1"></span><span style="color:#9fb6cf;font:13px Consolas,monospace">' + html.escape(d['path'])
               + '</span></nav>' + NAV_END)
        current = page.read_text(encoding='utf-8-sig')
        text = re.sub(re.escape(NAV_START) + r'.*?' + re.escape(NAV_END), '', current, flags=re.S)
        text = re.sub(r'(<body\b[^>]*>)', lambda m: m[0] + bar, text, count=1)
        if text != current: write(page, text)

def write_size_report():
    from standard_package_policy import LIMIT, inventory
    included,excluded=inventory(STANDARD)
    development,_=inventory(STANDARD,include_learning=True)
    total=sum(e['bytes'] for e in included)
    lines=['# Standard 제출 입력 용량','',
      'Package-Standard.py와 같은 정책으로 계산한다. 논리 파일 크기의 합이며 ZIP 크기·디스크 할당 크기와 다르다. 문서 자체는 제출에 포함하지 않는다.','',
      f'- 제출 입력: **{total:,}바이트** ({total/1e6:.2f} MB), {len(included)}개',
      f'- 기준: {LIMIT:,}바이트 이하 / 현재 {"이하" if total<=LIMIT else "초과"}',
      f'- 개발 사본(문서·debugger 포함, 빌드/IDE 제외): {sum(e["bytes"] for e in development):,}바이트',
      '- 제외: debugger, 프로젝트 x64/.vs, docs, 튜토리얼/학습 HTML, 예제',
      '- 보존: C++/프로젝트/props/GLSL, bundled SDK 헤더·라이브러리·DLL·라이선스. SDK의 x64 폴더는 빌드 산출물이 아니다.',
      '- 빌드·테스트·앱 실행·재배치 확인은 사용자 수행. 이 보고서는 실행 검증이 아니다.','',
      '| 제출 최상위 항목 | 바이트 |','|---|---:|']
    tops={}
    for e in included:
        top=e['path'].split('/')[0];tops[top]=tops.get(top,0)+e['bytes']
    lines += [f'| {name} | {size:,} |' for name,size in sorted(tops.items())]
    write(STANDARD/'SIZE_REPORT.md','\n'.join(lines)+'\n')


def refresh_index():
    docs = collect_documents()
    catalog = load_catalog(docs)
    add_navigation(docs, catalog)
    docs = collect_documents()
    write(STANDARD / 'docs/viewer/documents.js', js_data('STANDARD_DOCUMENTS', docs) + js_data('STANDARD_CATALOG', catalog))
    write(STANDARD / 'docs/viewer/scenarios.js', js_data('STANDARD_SCENARIOS', collect_scenarios()))
    for name, target in (('style.css', 'docs/viewer/style.css'), ('index.js', 'docs/viewer/index.js'),
                         ('preview.js', 'docs/viewer/preview.js'), ('index.html', 'index.html'), ('document.html', 'document.html')):
        write(STANDARD / target, (VIEWER_TEMPLATES / name).read_text(encoding='utf-8-sig'))
    for path in sorted((VIEWER_TEMPLATES / 'vendor').glob('*')):
        if path.is_file(): write(STANDARD / 'docs/viewer/vendor' / path.name, path.read_text(encoding='utf-8-sig'))
    write_document_index(docs, catalog)
    write_size_report()
    print('Indexed', len(docs), 'documents in', len(catalog['categories']), 'categories; scenarios bundled. No browser/tests run.')

def main():
    import runpy
    runpy.run_path(str(ROOT / 'tools/Write-SourceDebuggerDocs.py'))['refresh_documents']()
    refresh_index()
if __name__=='__main__': main()
