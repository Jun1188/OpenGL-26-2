"""Refresh current assembly API notes and offline standard document navigation. No build/tests."""
from pathlib import Path
import html
import json
import os
import re
ROOT = Path(__file__).resolve().parents[1]
STANDARD = ROOT / 'standard'
START = '<!-- current-assembly-api:start -->'
END = '<!-- current-assembly-api:end -->'
def write(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(value, encoding='utf-8-sig')
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
        def link(path,label=None):
            return '<a href="'+html.escape(os.path.relpath(path,page.parent).replace('\\','/'))+'">'+html.escape(label or path.name)+'</a>'
        block='<section id="current-assembly-api"><h2>2026-11-03 갱신: 면 프레임·기능 조립 API</h2><p>기존 본문과 별도로 최신 추가 API를 정리했다. 빌드·실행 확인은 사용자에게 맡긴다. '+link(STANDARD/'index.html','전체 문서 목차')+'</p>'
        block+='<h3>Scene::setFaceFrame</h3><p>입력: EntityId, 면 인덱스, 유효한 affine mat4. 성공은 true, 없는 메시/면 또는 유효하지 않은 행렬은 false다. 엔티티 기준 변환이나 추가 면 모션을 대체하지 않고 최종 면 변환의 transformFrame을 설정한다. 기하 정점을 바꾸지 않는다.</p>'
        text=(base/'detail/Scene.cpp').read_text(encoding='utf-8-sig')
        begin=text.index('bool Scene::setFaceFrame('); end=text.index('\n}',begin)+2
        block+=code(text[begin:end])+'<p>'+link(base/'detail/Scene.cpp',f'Scene.cpp:{text[:begin].count(chr(10))+1}')+'</p>'
        block+='<p>입력 → native 최종 faceWorldMatrix를 프레임으로 저장 → 작성자 면 로컬 자세와 합성 → Renderer·피킹·표면 관절이 같은 최종 면 변환을 읽는다. normal은 inverse-transpose, cull은 winding을 기준으로 처리한다.</p>'
        if relative=='standard/docs/API_REFERENCE.html':
            block+='<h3>assembly::Assembly — 공개 API</h3><p>standard 전용 조립 계층이다. framework 공통 API와 별도 소유자다. 위치는 부모 공간, 각도는 도, 시간은 초다. group/part/mount로 계층을 만들고 srt/pivot/center로 중심과 전체 변환을 지정한다. attach는 Motion/Custom/Input 등의 Behavior를 연결한다. 중복 이름·잘못된 값·충돌하는 제어 소유자는 require 예외로 거절한다.</p>'
            text,line=excerpt(STANDARD/'modes/Assembly.h','public:\n','\n};')
            block+=code(text)+'<p>'+link(STANDARD/'modes/Assembly.h',f'Assembly.h:{line}')+'</p>'
            block+='<table><tr><th>API</th><th>입력·반응·계약</th></tr>'
            for api,desc in [
                ('addFeature(name, mode)','1~22의 새 native 상태/CPU Scene/시계를 생성한다. 한 Application/GL 컨텍스트를 공유한다.'),
                ('portFeature(source, native, name)','실제 native ID와 자식을 출력하는 라이브 포트다. 원본 program/시계를 공유한다. feature parts로 ID를 확인한다.'),
                ('srt / pivot / center / setRest','루트의 기준값을 바꾼다. pivot keep은 기준 배치를 보정한다. center는 형상의 AABB 중심이며 질량중심이 아니다.'),
                ('model / mount / meshEntity','임의 Model을 등록하고 원본 메시의 면/삼각형 좌표에 관절을 붙인다. native 형상을 직접 편집하려면 snapshot으로 독립 복사한다.'),
                ('attach / bind','동작 또는 입력을 기능 단위로 연결한다. 사용자 콜백의 외부 IO/shared 상태까지 자동 복원하지 않는다.'),
                ('execute / update / render','execute 실패는 CPU 조립 상태를 복원한다. update는 초 단위 dt로 한 번 진행하고 실패 시 정지한다. render는 시간을 진행하지 않는다.'),
                ('guide / diagnose / inspect','guide는 현재 소스 줄·변경 전후 예·디버깅 순서를 출력한다. diagnose는 행렬/면/부모/기능 상태, inspect는 객체 등록 정보를 보여 준다.'),
                ('onKey / onMouse / onCursor / onFocus','입력을 원본 또는 공통 편집기로 보낸다. 클릭은 실제 뷰의 exported ID를 native ID로 역매핑한다. 포커스 이탈 시 누름/드래그를 해제한다.')]:
                block+='<tr><td><code>'+html.escape(api)+'</code></td><td>'+html.escape(desc)+'</td></tr>'
            block+='</table><h3>FeatureContainer와 가이드 메타데이터</h3>'
            for filename in ('FeatureContainer.h','GuideRecipes.h'):
                text,line=excerpt(STANDARD/'modes'/filename,'namespace assembly {')
                block+=code(text)+'<p>'+link(STANDARD/'modes'/filename,filename+':'+str(line))+'</p>'
            block+='<p>FeatureContainer의 scene/camera/options는 비소유 const 참조다. 소유 컨테이너의 수명 안에서만 사용한다. 복사는 금지한다. step은 native 상태를 한 번 갱신하고 내부 종료 요청은 오류로 전달한다. GuideSite는 현재 줄 검색 표식, GuideRecipe는 목적·전후 예·제약·디버깅·예상 결과다. findGuideRecipe는 미등록 항목에 nullptr을 반환한다.</p>'
            block+='<h3>사용 예와 핫로드</h3>'+code('assembly::Assembly scene(app);\nscene.addFeature("swingLab", 9);\nscene.addFeature("tankLab", 16);\nscene.move("tankLab", {5,0,0});\nscene.guide("tank", std::cout);\nscene.diagnose("tankLab", std::cout);')
            block+='<p>콘솔: feature parts tankLab → 실제 ID로 feature port → feature focus/camera. Ctrl+F5는 .assembly 설정 재로드이며 C++ 함수 변경은 사용자 재빌드 대상이다. add는 독립 시계, port는 공유 시계다. 컨테이너 사이 물리 충돌은 자동 병합하지 않는다. private 원본 상태의 임의 프레임 역실행은 제공하지 않는다.</p>'
            for name in ('EDIT_DEBUG_GUIDE','FEATURE_CONTAINERS','REFLECTION_AND_HOTLOAD','USER_CHECKS'):
                block+='<p><a href="../document.html?doc=docs/assembly/'+name+'.md">'+name+' · 브라우저 미리보기</a></p>'
        else:
            block+='<p>standard 전용 Assembly/FeatureContainer/GuideRecipe/diagnose는 '+link(STANDARD/'docs/API_REFERENCE.html','standard API Reference의 최신 조립 API')+'에서 확인한다.</p>'
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
    """Scenario cards come from the '# @key: value' headers of the recipe files."""
    scenarios = []
    for p in sorted(SCENARIOS.glob('*.assembly')):
        meta, commands = {}, []
        for line in p.read_text(encoding='utf-8-sig').splitlines():
            match = re.match(r'#\s*@(\w+):\s*(.*)', line)
            if match: meta[match[1]] = match[2].strip()
            elif line.strip() and not line.startswith('#'): commands.append(line)
        if 'title' not in meta: raise SystemExit(p.name + ': # @title header required')
        scenarios.append({'id': p.stem, 'file': p.relative_to(STANDARD).as_posix(), **meta, 'commands': '\n'.join(commands)})
    return scenarios

def write_document_index(docs, catalog):
    """Markdown version of the catalog; keeps the flowchart block that author_framework_guides.py maintains."""
    titles = {d['path']: d['title'] for d in docs}
    old = (STANDARD / 'DOCUMENT_INDEX.md').read_text(encoding='utf-8-sig')
    begin, end = '<!-- flowchart-index:start -->', '<!-- flowchart-index:end -->'
    flow = old[old.index(begin):old.index(end) + len(end)] if begin in old and end in old else ''
    lines = ['[브라우저 목차](index.html) · [시험 시나리오](docs/EXAM_SCENARIOS.html) · [README](README.md)', '',
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
    write_document_index(docs, catalog)
    print('Indexed', len(docs), 'documents in', len(catalog['categories']), 'categories; scenarios bundled. No browser/tests run.')

def main():
    refresh_api()
    refresh_index()
if __name__=='__main__': main()
