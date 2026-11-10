# API Reference·브라우저 문서 갱신 변경 전후

기준: 04557ff. API Reference에 조립 API 누락을 확인하여 현재 소스 절을 보완했다. 최상위 목차와 오프라인 Markdown 미리보기를 추가했다. 기존 사용자 HTML 편집은 보존하며, API 생성기의 마지막 단계에서 이 생성기를 호출한다. 빌드·테스트·브라우저 실행은 하지 않았다.

각 생성 소스 전체의 before/after를 수록한다. index/document HTML과 viewer JS/CSS는 Update-StandardDocs.py에 생성 문자열 전체가 있으므로 같은 코드를 다시 중복 수록하지 않는다. documents.js는 원본 Markdown 데이터 스냅샷이다. 외부 Marked는 원본 배포물과 라이선스를 vendor에서 확인한다.

입력(.md/.html와 실제 API 헤더) → Python 생성기 → API 최신 절과 정적 인덱스/문서 데이터 → 브라우저 Marked 미리보기. C++ 런타임에는 영향이 없다. 문서 변경 후 생성기를 실행해야 스냅샷이 갱신된다.

사용자 확인은 DOCUMENT_VIEWER.md에 있다.

## tools/Update-StandardDocs.py

### before

없음.

### after

````python
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

def refresh_index():
    docs=[]
    for p in sorted(STANDARD.rglob('*')):
        rel=p.relative_to(STANDARD)
        if not p.is_file() or p.suffix.lower() not in ('.md','.html'): continue
        if any(part in ('.vs','x64','dependencies','viewer') for part in rel.parts): continue
        if rel.as_posix() in ('index.html','document.html'): continue
        text=p.read_text(encoding='utf-8-sig')
        if p.suffix.lower()=='.md':
            match=re.search(r'^#\s+(.+)',text,re.M)
        else: match=re.search(r'<title[^>]*>(.*?)</title>',text,re.S|re.I)
        docs.append({'path':rel.as_posix(),'title':html.unescape(match[1].strip()) if match else p.stem,'markdown':text if p.suffix.lower()=='.md' else None})
    data=json.dumps(docs,ensure_ascii=False).replace('<','\\u003c').replace('\u2028','\\u2028').replace('\u2029','\\u2029')
    write(STANDARD/'docs/viewer/documents.js','window.STANDARD_DOCUMENTS = '+data+';\n')
    style='''*{box-sizing:border-box}body{margin:0;background:#1e1e1e;color:#ddd;font:16px/1.7 system-ui,"Malgun Gothic",sans-serif}header{padding:18px 28px;background:#252526;position:sticky;top:0;border-bottom:1px solid #444}a{color:#73bdff}main{max-width:1100px;margin:auto;padding:28px}input,button,select{background:#303033;color:#eee;border:1px solid #666;padding:9px;border-radius:5px}input{width:min(600px,90%)}pre{background:#141414;padding:18px;overflow:auto;border:1px solid #444;border-radius:6px;tab-size:4}code{font-family:Consolas,monospace;font-size:.9em}p code,li code,td code{background:#303033;padding:2px 4px}table{border-collapse:collapse;display:block;overflow:auto}th,td{border:1px solid #555;padding:9px 14px}blockquote{border-left:4px solid #777;margin-left:0;padding-left:20px;color:#bbb}h1,h2,h3{line-height:1.4;border-bottom:1px solid #444;padding-bottom:8px;scroll-margin-top:110px}img{max-width:100%}.doc{padding:14px;border-bottom:1px solid #444}.path{color:#aaa;font-size:.85em}#toc{border:1px solid #555;padding:14px}#source{white-space:pre-wrap}button{cursor:pointer}a:focus,button:focus{outline:2px solid #73bdff}'''
    write(STANDARD/'docs/viewer/style.css',style)
    head='<!doctype html><html lang="ko"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><link rel="stylesheet" href="docs/viewer/style.css">'
    write(STANDARD/'index.html',head+'''<title>standard 문서 목차</title></head><body><header><strong>standard · 전체 문서</strong><p><input id="search" type="search" placeholder="제목·경로 검색" aria-label="문서 검색"> <select id="kind" aria-label="문서 형식"><option value="all">전체</option><option value="md">Markdown</option><option value="html">HTML</option></select></p></header><main><h1>OpenGL standard 문서</h1><p>API · 0~22 실습 · 기능 조립 · 직접 편집 · 사용자 확인. Markdown은 오프라인 미리보기로 연다.</p><p><a href="docs/API_REFERENCE.html#current-assembly-api">최신 API Reference</a> · <a href="document.html?doc=docs/assembly/README.md">조립 문서 시작</a></p><p id="count"></p><div id="list"></div></main><script src="docs/viewer/documents.js"></script><script src="docs/viewer/index.js"></script></body></html>''')
    write(STANDARD/'docs/viewer/index.js','''const records=window.STANDARD_DOCUMENTS;
function render(){const query=document.querySelector('#search').value.toLocaleLowerCase();const kind=document.querySelector('#kind').value;const rows=records.filter(d=>(d.title+' '+d.path).toLocaleLowerCase().includes(query)&&(kind==='all'||(kind==='md')===(d.markdown!==null)));const list=document.querySelector('#list');list.replaceChildren();for(const d of rows){const box=document.createElement('div');box.className='doc';const link=document.createElement('a');link.textContent=d.title;link.href=d.markdown===null?d.path:'document.html?doc='+encodeURIComponent(d.path);const path=document.createElement('div');path.className='path';path.textContent=d.path;box.append(link,path);list.append(box)}document.querySelector('#count').textContent=rows.length+' / '+records.length+'개 문서'}
document.querySelector('#search').addEventListener('input',render);document.querySelector('#kind').addEventListener('change',render);render();''')
    write(STANDARD/'document.html',head+'''<title>Markdown 미리보기</title></head><body><header><a href="index.html">전체 문서</a> · <a id="original">원본 Markdown</a> <button id="toggle">원문 보기</button><div id="path" class="path"></div></header><main><nav id="toc" aria-label="문서 목차"></nav><article id="content"></article><pre id="source" hidden></pre></main><script src="docs/viewer/vendor/marked.umd.js"></script><script src="docs/viewer/documents.js"></script><script src="docs/viewer/preview.js"></script></body></html>''')
    write(STANDARD/'docs/viewer/preview.js','''const root=new URL('./',location.href);const docs=window.STANDARD_DOCUMENTS;const path=new URLSearchParams(location.search).get('doc')||'README.md';const record=docs.find(d=>d.path===path&&d.markdown!==null);const content=document.querySelector('#content');
if(!record){content.textContent='문서를 찾을 수 없습니다. 전체 목차에서 선택하세요.'}else{
 document.title=record.title+' · Markdown';document.querySelector('#path').textContent=path;document.querySelector('#original').href=new URL(path,root).href;document.querySelector('#source').textContent=record.markdown;
 const escape=s=>s.replace(/[&<>"']/g,c=>({'&':'&amp;','<':'&lt;','>':'&gt;','"':'&quot;',"'":'&#39;'}[c]));
 const renderer=new marked.Renderer();renderer.html=token=>escape(token.text);content.innerHTML=marked.parse(record.markdown,{gfm:true,renderer});
 for(const a of content.querySelectorAll('a[href]')){const raw=a.getAttribute('href');if(raw.startsWith('#'))continue;const url=new URL(raw,new URL(path,root));if(!['file:','http:','https:','mailto:'].includes(url.protocol)){a.removeAttribute('href');continue}const relative=decodeURIComponent(url.pathname.slice(root.pathname.length));const md=docs.find(d=>d.path===relative&&d.markdown!==null);if(url.origin===root.origin&&url.pathname.startsWith(root.pathname)&&md){a.href=new URL('document.html?doc='+encodeURIComponent(relative)+url.hash,root).href}else a.href=url.href}
 for(const img of content.querySelectorAll('img[src]')){const url=new URL(img.getAttribute('src'),new URL(path,root));if(['file:','http:','https:'].includes(url.protocol))img.src=url.href;else img.removeAttribute('src')}
 const toc=document.querySelector('#toc');let count=0;for(const h of content.querySelectorAll('h1,h2,h3,h4,h5,h6')){const slug=h.textContent.trim().toLowerCase().replace(/[^\\p{L}\\p{N}_ -]/gu,'').replace(/ /g,'-');h.id=slug||'heading-'+(++count);if(document.getElementById(h.id)!==h)h.id+='-'+(++count);const a=document.createElement('a');a.href='#'+encodeURIComponent(h.id);a.textContent=h.textContent;const item=document.createElement('div');item.style.paddingLeft=(Number(h.tagName[1])-1)*12+'px';item.append(a);toc.append(item)}
 document.querySelector('#toggle').onclick=()=>{const source=document.querySelector('#source');source.hidden=!source.hidden;content.hidden=!source.hidden;toc.hidden=!source.hidden;document.querySelector('#toggle').textContent=source.hidden?'원문 보기':'미리보기'};
 if(location.hash){const target=document.getElementById(decodeURIComponent(location.hash.slice(1)));if(target)target.scrollIntoView()}
}
''')
    print('Indexed',len(docs),'documents; Markdown text bundled for file:// use. No browser/tests run.')
def main():
    refresh_api()
    refresh_index()
if __name__=='__main__': main()
````

## tools/Refresh-ApiCatalog.py

### before

````python
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
````

### after

````python
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
````

## tools/Write-ApiKo.py

### before

````python
"""공개 C++ 선언에 한글 기능 주석을 붙이고 API 목록을 만든다. 빌드/테스트 없음."""
from pathlib import Path
import importlib.util
import json
import os
import re
from tree_sitter import Language, Parser
import tree_sitter_cpp
ROOT=Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('guide',ROOT/'tools/Write-CodeGuide.py')
guide=importlib.util.module_from_spec(spec);spec.loader.exec_module(guide)
PARSER=Parser(Language(tree_sitter_cpp.language()))
DESCRIPTIONS={
'addPolygon':'점 목록으로 단일 다각형 면을 만들고 색·부모 관계와 함께 Scene에 등록한다. 오목 다각형의 자동 분할을 가정하지 않는다.',
'lineWorldMatrix':'선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.',
'setInitHandler':'장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.',
'setUpdateHandler':'매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.',
'setKeyHandler':'키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.',
'setModifiedKeyHandler':'키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.',
'setFocusHandler':'창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.',
'setRenderHandler':'사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.',
'setMouseHandler':'마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.',
'setCursorHandler':'창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.',
'pickAtCursor':'현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.',
'getAssets':'실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.',
'requestClose':'창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.',
'renderScene':'기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.',
'getRenderOptions':'애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.',
'getScene':'애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.',
'getCamera':'기본 카메라의 수정 가능한 참조를 반환한다.',
'getWindow':'GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.',
'context':'진단에 첨부할 현재 파일·줄·호출 경로 문맥의 참조를 반환한다.',
'locate':'다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.',
'failAt':'주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.',
'stageName':'파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.',
'configure':'오류 출력과 단계 추적 출력의 사용 여부를 설정한다.',
'lastError':'마지막으로 기록한 오류의 optional 참조를 반환한다.',
'trace':'추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.',
'fail':'현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.',
'addGroup':'메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.',
'addSphere':'반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.',
'addCone':'반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.',
'setTint':'엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.',
'push_entity':'주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.',
'addCuboid':'중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.',
'addRightTetrahedron':'세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.',
'addPyramid':'가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.',
'setFaceVisible':'해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.',
'setFaceCullMode':'해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.',
'setFaceLocalMatrix':'해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.',
'setFaceVertexColors':'면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.',
'getMotions':'Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.',
'bindEntity':'엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.',
'unbindEntity':'엔티티와 모션 레이어의 연결을 해제한다.',
'getEntityLayer':'엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.',
'updateMotions':'Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.',
'setFaceTransform':'면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.',
'addModel':'CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.',
'setModel':'기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.',
'setMesh':'기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.',
'getMesh':'지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.',
'setMeshVertices':'메시의 정점과 삼각형 인덱스를 함께 교체한다.',
'setVertexPosition':'메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.',
'setTriangleIndices':'메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.',
'generateTexcoords':'평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.',
'transformTexcoords':'메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.',
'setTexcoords':'메시의 정점별 UV 배열을 교체한다.',
'setVertexNormals':'메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.',
'setMaterial':'메시의 색상·텍스처 등 재질 데이터를 교체한다.',
'setOutline':'윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.',
'setVisible':'엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.',
'splitFace':'지정 면을 독립 변환 가능한 면으로 분리한다.',
'setFaceSRT':'면의 크기·회전·이동 변환을 설정한다.',
'addLine':'두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.',
'addVector':'시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.',
'addBasis':'기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.',
'setVector':'기존 선을 시작점과 방향/길이 벡터로 갱신한다.',
'getIds':'현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.',
'getComponent':'엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.',
'getCubeFaceIds':'직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.',
'getTetraFaceIds':'사면체 면 ID 목록을 읽기 전용 참조로 반환한다.',
'getPyramidFaceIds':'피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.',
'getLines':'Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.',
'getGeometryRevision':'렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.',
'makePolyhedron':'형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.',
'makeSphere':'반지름·분할 수로 CPU 구 메시 모델을 생성한다.',
'makeCone':'반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.',
'validPolyhedron':'다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.',
'validTransform':'변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.',
'modelMatrix':'T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.',
'faceModelMatrix':'컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.',
'validCamera':'시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.',
'viewMatrix':'카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.',
'projectionMatrix':'카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.',
'readTextFile':'지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.',
'render':'Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.',
'valid':'해당 데이터가 계산에 필요한 유효 범위와 유한성 조건을 만족하는지 반환한다.',
'fromVertices':'정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.',
'meshes':'모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.',
'features':'로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.',
'loadTexture':'프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.',
'loadObj':'프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.',
'loadPrimitive':'기본 도형 OBJ를 읽는 호환 진입점이다. OBJ 로더로 처리를 위임한다.',
'loadModel':'모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.',
'append':'모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.',
'insert':'주어진 인덱스에 모션 단계를 삽입한다.',
'replace':'주어진 인덱스의 모션 단계를 새 단계로 교체한다.',
'erase':'주어진 인덱스의 모션 단계를 삭제한다.',
'getSteps':'등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.',
'fixed':'고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.',
'followPath':'경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.',
'rootLayer':'삭제할 수 없는 루트 레이어의 ID 0을 반환한다.',
'createMotion':'모션을 시스템에 등록하고 새 MotionId를 반환한다.',
'setMotion':'기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.',
'getMotion':'모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.',
'setMotionPlaying':'모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.',
'setMotionEnabled':'모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.',
'removeMotion':'모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.',
'setLayer':'기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.',
'getLayer':'레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.',
'createSpin':'로컬 공간 회전 모션을 등록하여 자전에 사용한다.',
'createOrbit':'월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.',
'contains':'점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.',
'overlaps':'두 축 정렬 경계 상자가 겹치는지 검사한다.',
'setPoints':'연속 점들의 누적 거리 경로를 구성한다. closed면 마지막에서 처음으로 연결한다.',
'length':'경로 전체 누적 길이를 반환한다. 경로가 비어 있으면 0이다.',
'snakeCells':'셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.',
'aligned':'두 값의 차이가 허용 오차 이하인지 검사한다.',
'screenRay':'왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.',
'pick':'ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.',
'recalculateNormals':'메시의 삼각형 기하로 정점 법선을 다시 계산한다.',
'meshOutline':'메시의 윤곽 모드에 따라 선으로 그릴 정점 인덱스 쌍 목록을 계산한다.',
'validAffine':'행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.',
'translation':'이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.',
'scaling':'중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.',
'reflectPoint':'주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.',
'reflectLine':'주어진 직선을 기준으로 선 반사 행렬을 만든다.',
'reflectPlane':'평면의 점과 법선으로 평면 반사 행렬을 만든다.',
'ellipse':'중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.',
'circle':'중심·반지름·평면 법선으로 원 경로 함수를 만든다.',
'polyline':'점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.',
'polygon':'마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.',
'rectangle':'중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.',
'linear':'입력 진행률을 선형 보간 진행률로 반환한다.',
'smoothStep':'진행률에 3t²-2t³ 완화 곡선을 적용한다.',
}
SPECIAL={
('GridLayout','valid'):'origin·extent·셀 개수와 frame의 유한성·양수 크기·가역성을 검사한다.',
('GridLayout','position'):'[0,1] 비율을 origin+extent*ratio로 바꾼 뒤 frame으로 월드 위치를 계산한다.',
('GridLayout','cellCenter'):'0부터 시작하는 셀 인덱스를 해당 셀 중심의 월드 위치로 변환한다.',
('GridLayout','relativeSize'):'양수 비율과 extent를 곱해 그리드 로컬 크기를 반환한다. frame 회전/배율은 적용하지 않는다.',
('GridLayout','positionInParent'):'비율 위치를 월드로 계산한 뒤 parentWorld의 역행렬로 부모 로컬 위치를 반환한다.',
('GridLayout','cellAt'):'월드 점을 frame 역변환으로 그리드 셀 인덱스에 대응시킨다. 그리드 밖이면 빈 결과다.',
('RoomBody','position'):'이탈 전에는 roomWorld로 로컬 중심을 월드로 변환하고 이탈 후에는 worldPosition을 반환한다.',
('TextureImage','valid'):'이미지 너비·높이와 RGBA 바이트 수가 맞는지 검사한다.',
('Diagnostics','clear'):'마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.',
('Scene','clear'):'Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.',
('MotionSystem','clear'):'모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.',
('Motion','sample'):'절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성한다. 의미 동작은 실행하지 않는다.',
('DistancePath','sample'):'누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.',
('Motion','rotation'):'축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.',
('TransformSystem','rotation'):'직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.',
('AnimationClock','reset'):'누적 시간을 0으로 되돌리고 재생 상태를 켠다.',
('Application','renderViews'):'현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.',
('Renderer','initialize'):'현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.',
}
def save(p,text):
    p.parent.mkdir(parents=True,exist_ok=True)
    if p.exists() and p.read_text(encoding='utf-8-sig')==text:return
    temp=p.with_name(p.name+'.api.tmp');temp.write_text(text,encoding='utf-8-sig',newline='\n');os.replace(temp,p)
def declarations(path):
    b=path.read_bytes();out=[];class_names=set()
    def text(n):return b[n.start_byte:n.end_byte].decode('utf-8') if n else ''
    def find(n):
        if n.type in ('function_declarator','operator_cast'):return n
        for c in n.named_children:
            if c.type not in ('parameter_list','compound_statement','template_argument_list'):
                d=find(c)
                if d:return d
    def walk(n,scope='',public=True):
        if n.type=='namespace_definition' and text(n.child_by_field_name('name'))=='detail':return
        if n.type in ('class_specifier','struct_specifier'):
            name=text(n.child_by_field_name('name'));body=n.child_by_field_name('body');vis=n.type=='struct_specifier'
            class_names.add(name)
            if body:
                for c in body.named_children:
                    if c.type=='access_specifier':vis=text(c)=='public'
                    else:walk(c,name,vis)
            return
        if n.type in ('declaration','field_declaration','function_definition'):
            d=find(n)
            if d and public:
                name='operator bool' if d.type=='operator_cast' else text(d.child_by_field_name('declarator'))
                owner=scope
                if '::' in name:
                    owner,name=name.rsplit('::',1)
                    # Out-of-class member definitions repeat the class declaration.
                    if n.type=='function_definition' and not scope and owner.rsplit('::',1)[-1] in class_names:
                        return
                body=n.child_by_field_name('body');end=body.start_byte if body else n.end_byte
                sig=b[n.start_byte:end].decode('utf-8').strip().rstrip(';').strip()
                out.append({'owner':owner,'name':name,'signature':re.sub(r'\s+',' ',sig),'file':path.relative_to(ROOT).as_posix(),'line':n.start_point.row+1})
            return
        for c in n.named_children:walk(c,scope,public)
    walk(PARSER.parse(b).root_node);return out
def description(r):
    owner,name=r['owner'],r['name']
    source_lines=(ROOT/r['file']).read_text(encoding='utf-8-sig').splitlines()
    preceding=r['line']-2
    if preceding>=0 and '/// [API]' in source_lines[preceding]:
        return source_lines[preceding].split('/// [API]',1)[1].strip()
    if '= delete' in r['signature']:return '이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.'
    if name==owner:return {'Application':'초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.','Scene':'빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.','Renderer':'GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.','AssetLoader':'프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.'}[owner]
    if name.startswith('~'):return '실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.'
    if name=='operator bool':return '모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.'
    additions={
        ('HumanoidPose','seated'):'허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다.',
        ('AnimationSystem','createJointAnimation'):'관절의 기준 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율이다.',
        ('AnimationSystem','poseHierarchy'):'기준 SRT와 모션을 합성하고 모든 관절이 유효하면 자세를 반영한다.',
        ('AnimationSystem','addHumanoid'):'편집 가능한 사람형 관절·메시 계층을 만든다. 구성 실패는 기존 Scene을 유지한다.',
        ('AnimationSystem','poseHumanoid'):'각 관절 위치를 유지하며 로컬 Euler 자세를 적용한다.',
        ('AnimationSystem','addSwingWithRider'):'고정 프레임·진자 관절·좌석·앉은 사람과 모션을 구성한다.',
        ('Motion','pendulum'):'축·진폭·주기·위상의 사인 진자 회전 단계를 만든다.',
        ('Scene','setFaceTransformMode'):'면의 계층 상속·월드 독립을 전환한다. 기본값은 현재 자세 보존이다.',
        ('PatternSystem','rotateFace'):'현재 면의 모서리 힌지·중심 자전·원본 모델 중심 공전을 구분해 회전을 설정한다.'}
    if (owner,name) in additions:return additions[owner,name]
    if owner=='SceneActions':return '지정 값과 대상 ID를 기존 API에 전달하는 '+name+' 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.'
    if owner=='DrawUniformContext':return '현재 바인딩된 program의 추가 uniform을 설정한다. 이름·GLSL 자료형과 값의 유한성을 맞춘다.'
    if owner=='CuboidMode':return {
        'setConfig':'초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.',
        'getConfig':'초기 설정의 읽기 전용 참조를 반환한다.',
        'setCuboid':'이름별 육면체 초기 설정을 교체하거나 추가한다.',
        'bindKey':'같은 key/action/mods 바인딩을 교체하거나 추가한다.',
        'unbindKey':'같은 key/action/mods 바인딩을 해제한다.',
        'getBindings':'키 바인딩 목록의 읽기 전용 참조를 반환한다.',
        'setSetupHandler':'새 장면 구성 후 실행할 설정 핸들러를 교체한다.',
        'setUpdateHandler':'프레임별 응용 갱신 핸들러를 교체한다.',
        'apply':'임시 CPU Scene에 육면체를 구성하고 성공하면 장면 전체를 교체한다.',
        'update':'등록된 모드 프레임 핸들러를 호출한다.',
        'dispatch':'일치하는 키 바인딩을 실행한다. 없는 키는 정상 처리한다.',
        'entity':'이름에 대응하는 마지막 생성 엔티티 ID를 조회한다.',
        'layer':'이름에 대응하는 초기 생성 레이어 ID를 조회한다.',
        'motion':'이름에 대응하는 초기 생성 모션 ID를 조회한다.',
        'action':'실행 시 육면체 이름을 해석하는 의미 명령 핸들러를 만든다.'}[name]
    if name=='bindCuboidMode':return '육면체 모드의 init/update/수정키 핸들러를 설치한다. 기존 해당 핸들러를 교체한다.'
    if name=='configureRenderer':return '현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.'
    if owner=='Motion' and name in ('action','insertAction','execute'):
        return {'action':'다른 단계와 조립할 의미 동작 단계를 만든다.',
                'insertAction':'주어진 인덱스에 람다·함수 포인터·의미 동작을 삽입한다.',
                'execute':'변환과 동작을 순서대로 평가한다. 외부 부수 효과는 복구하지 않는다.'}[name]
    if (owner,name) in SPECIAL:return SPECIAL[owner,name]
    if name in DESCRIPTIONS:return DESCRIPTIONS[name]
    if name in guide.PURPOSE:return guide.PURPOSE[name]
    raise ValueError('한글 기능 설명 누락: '+owner+'::'+name)
def result_contract(r):
    s=r['signature']
    if '= delete' in s:return '호출 불가(복사 금지 선언).'
    if 'std::optional<' in s.split('(')[0]:return '값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.'
    if re.search(r'\bbool\b',s.split('(')[0]):return 'true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.'
    if '*' in s.split('(')[0]:return '비소유 포인터다. nullptr과 원본 객체 수명·컨테이너 변경을 확인한다.'
    if '&' in s.split('(')[0]:return '비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.'
    return '반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.'
def main():
    paths=sorted((ROOT/'framework/include').glob('*.h'))
    # 기존 생성 주석을 제거하고 AST 위치를 다시 구해 반복 실행 시 중복을 막는다.
    for p in paths:
        s=re.sub(r'^[ \t]*/// \[API\].*\n','',p.read_text(encoding='utf-8-sig'),flags=re.M)
        save(p,s)
        records=declarations(p);lines=s.splitlines()
        for r in sorted(records,key=lambda r:r['line'],reverse=True):
            indent=re.match(r'\s*',lines[r['line']-1])[0]
            lines.insert(r['line']-1,indent+'/// [API] '+description(r))
        save(p,'\n'.join(lines)+'\n')
    records=[]
    for p in paths:
        for r in declarations(p):r.update(description=description(r),result=result_contract(r));records.append(r)
    save(ROOT/'framework/docs/API_KO.json',json.dumps(records,ensure_ascii=False,indent=2)+'\n')
    md='# 프레임워크 공개 API 한글 목록\n\n대상: `framework/include/*.h`의 공개 함수·메서드·생성/소멸·변환 연산자. 비공개 멤버, detail 구현, SDK 함수는 제외한다. 오버로드와 const 구분은 각각 기록한다. 클래스 외부의 멤버 정의는 클래스 내부 선언과 중복 수록하지 않는다. 각 함수의 한글 주석은 선언 바로 위에 있다.\n\n'
    md+='이 문서는 API의 전체 탐색 목록이다. 시험장에서 사용할 호출 순서는 [공통 행동 그래프](../../docs/code-guide/exam.html)와 [요구사항별 API 호출](../../docs/code-guide/api.html)을 먼저 읽는다.\n\n'
    md+='## 공통 계약\n\n- 시간은 초, 각도는 별도 표기가 없으면 도다. `glm::radians`에 넣은 뒤 GLM 회전 행렬을 만든다.\n- 부모가 있는 엔티티의 위치는 부모 로컬이다. GridLayout.position은 월드 위치, relativeSize는 그리드 로컬 크기다.\n- bool/optional/포인터를 검사한 뒤 결과를 사용한다. 빈 피킹 결과·공중 jump의 false는 정상적인 판정일 수 있다.\n- Application은 모션을 프레임당 한 번 갱신한다. 여러 RenderPass에서도 물리와 시간을 중복 갱신하지 않는다.\n- ID는 소유 포인터가 아니다. Scene.clear 후 엔티티 ID를 다시 받는다. GPU 자원은 현재 컨텍스트에서 생성·해제한다.\n- Front cull은 winding에 따른 면 제거다. 법선 기반 명암과 다르다. 현재 셰이더에는 투영 그림자가 없다.\n\n## 파일별 목록\n\n'
    for p in paths:
        rows=[r for r in records if r['file']==p.relative_to(ROOT).as_posix()]
        md+='- ['+p.name+'](#'+p.stem.lower()+'): '+str(len(rows))+'개 선언\n'
    for p in paths:
        rows=[r for r in records if r['file']==p.relative_to(ROOT).as_posix()]
        md+='\n## '+p.stem+'\n\n'
        if not rows:md+='공개 함수 없음. 데이터/내부 지원 헤더다.\n'
        for r in rows:
            full=(r['owner']+'::' if r['owner'] else '')+r['name']
            md+='### '+full+'\n\n```cpp\n'+r['signature']+';\n```\n\n'+r['description']+'\n\n입력: 위 시그니처의 타입·기본값과 기능 설명을 함께 읽는다.\n\n반환·실패: '+r['result']+'\n\n소스: [`'+r['file']+':'+str(r['line'])+'`](../include/'+p.name+')\n\n'
    md+='\n## 확인 범위\n\n소스 선언을 AST로 읽어 만든 목록이며 빌드·테스트·실행을 수행하지 않았다. 사용자에게 API 사용 코드와 화면 확인을 맡긴다.\n'
    save(ROOT/'framework/docs/API_KO.md',md)
    print('공개 API 한글 주석 및 목록 생성: '+str(len(records))+'개 선언')
if __name__=='__main__':main()
````

### after

````python
"""공개 C++ 선언에 한글 기능 주석을 붙이고 API 목록을 만든다. 빌드/테스트 없음."""
from pathlib import Path
import importlib.util
import json
import os
import re
from tree_sitter import Language, Parser
import tree_sitter_cpp
ROOT=Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('guide',ROOT/'tools/Write-CodeGuide.py')
guide=importlib.util.module_from_spec(spec);spec.loader.exec_module(guide)
PARSER=Parser(Language(tree_sitter_cpp.language()))
DESCRIPTIONS={
'setFaceFrame':'면의 별도 affine 변환 프레임을 설정한다. 없는 메시/면 또는 유효하지 않은 affine 행렬이면 false를 반환한다. 추가 면 로컬 모션과 합성한다.',
'addPolygon':'점 목록으로 단일 다각형 면을 만들고 색·부모 관계와 함께 Scene에 등록한다. 오목 다각형의 자동 분할을 가정하지 않는다.',
'lineWorldMatrix':'선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.',
'setInitHandler':'장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.',
'setUpdateHandler':'매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.',
'setKeyHandler':'키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.',
'setModifiedKeyHandler':'키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.',
'setFocusHandler':'창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.',
'setRenderHandler':'사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.',
'setMouseHandler':'마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.',
'setCursorHandler':'창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.',
'pickAtCursor':'현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.',
'getAssets':'실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.',
'requestClose':'창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.',
'renderScene':'기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.',
'getRenderOptions':'애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.',
'getScene':'애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.',
'getCamera':'기본 카메라의 수정 가능한 참조를 반환한다.',
'getWindow':'GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.',
'context':'진단에 첨부할 현재 파일·줄·호출 경로 문맥의 참조를 반환한다.',
'locate':'다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.',
'failAt':'주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.',
'stageName':'파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.',
'configure':'오류 출력과 단계 추적 출력의 사용 여부를 설정한다.',
'lastError':'마지막으로 기록한 오류의 optional 참조를 반환한다.',
'trace':'추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.',
'fail':'현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.',
'addGroup':'메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.',
'addSphere':'반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.',
'addCone':'반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.',
'setTint':'엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.',
'push_entity':'주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.',
'addCuboid':'중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.',
'addRightTetrahedron':'세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.',
'addPyramid':'가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.',
'setFaceVisible':'해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.',
'setFaceCullMode':'해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.',
'setFaceLocalMatrix':'해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.',
'setFaceVertexColors':'면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.',
'getMotions':'Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.',
'bindEntity':'엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.',
'unbindEntity':'엔티티와 모션 레이어의 연결을 해제한다.',
'getEntityLayer':'엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.',
'updateMotions':'Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.',
'setFaceTransform':'면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.',
'addModel':'CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.',
'setModel':'기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.',
'setMesh':'기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.',
'getMesh':'지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.',
'setMeshVertices':'메시의 정점과 삼각형 인덱스를 함께 교체한다.',
'setVertexPosition':'메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.',
'setTriangleIndices':'메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.',
'generateTexcoords':'평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.',
'transformTexcoords':'메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.',
'setTexcoords':'메시의 정점별 UV 배열을 교체한다.',
'setVertexNormals':'메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.',
'setMaterial':'메시의 색상·텍스처 등 재질 데이터를 교체한다.',
'setOutline':'윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.',
'setVisible':'엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.',
'splitFace':'지정 면을 독립 변환 가능한 면으로 분리한다.',
'setFaceSRT':'면의 크기·회전·이동 변환을 설정한다.',
'addLine':'두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.',
'addVector':'시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.',
'addBasis':'기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.',
'setVector':'기존 선을 시작점과 방향/길이 벡터로 갱신한다.',
'getIds':'현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.',
'getComponent':'엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.',
'getCubeFaceIds':'직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.',
'getTetraFaceIds':'사면체 면 ID 목록을 읽기 전용 참조로 반환한다.',
'getPyramidFaceIds':'피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.',
'getLines':'Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.',
'getGeometryRevision':'렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.',
'makePolyhedron':'형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.',
'makeSphere':'반지름·분할 수로 CPU 구 메시 모델을 생성한다.',
'makeCone':'반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.',
'validPolyhedron':'다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.',
'validTransform':'변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.',
'modelMatrix':'T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.',
'faceModelMatrix':'컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.',
'validCamera':'시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.',
'viewMatrix':'카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.',
'projectionMatrix':'카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.',
'readTextFile':'지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.',
'render':'Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.',
'valid':'해당 데이터가 계산에 필요한 유효 범위와 유한성 조건을 만족하는지 반환한다.',
'fromVertices':'정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.',
'meshes':'모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.',
'features':'로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.',
'loadTexture':'프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.',
'loadObj':'프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.',
'loadPrimitive':'기본 도형 OBJ를 읽는 호환 진입점이다. OBJ 로더로 처리를 위임한다.',
'loadModel':'모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.',
'append':'모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.',
'insert':'주어진 인덱스에 모션 단계를 삽입한다.',
'replace':'주어진 인덱스의 모션 단계를 새 단계로 교체한다.',
'erase':'주어진 인덱스의 모션 단계를 삭제한다.',
'getSteps':'등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.',
'fixed':'고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.',
'followPath':'경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.',
'rootLayer':'삭제할 수 없는 루트 레이어의 ID 0을 반환한다.',
'createMotion':'모션을 시스템에 등록하고 새 MotionId를 반환한다.',
'setMotion':'기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.',
'getMotion':'모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.',
'setMotionPlaying':'모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.',
'setMotionEnabled':'모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.',
'removeMotion':'모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.',
'setLayer':'기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.',
'getLayer':'레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.',
'createSpin':'로컬 공간 회전 모션을 등록하여 자전에 사용한다.',
'createOrbit':'월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.',
'contains':'점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.',
'overlaps':'두 축 정렬 경계 상자가 겹치는지 검사한다.',
'setPoints':'연속 점들의 누적 거리 경로를 구성한다. closed면 마지막에서 처음으로 연결한다.',
'length':'경로 전체 누적 길이를 반환한다. 경로가 비어 있으면 0이다.',
'snakeCells':'셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.',
'aligned':'두 값의 차이가 허용 오차 이하인지 검사한다.',
'screenRay':'왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.',
'pick':'ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.',
'recalculateNormals':'메시의 삼각형 기하로 정점 법선을 다시 계산한다.',
'meshOutline':'메시의 윤곽 모드에 따라 선으로 그릴 정점 인덱스 쌍 목록을 계산한다.',
'validAffine':'행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.',
'translation':'이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.',
'scaling':'중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.',
'reflectPoint':'주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.',
'reflectLine':'주어진 직선을 기준으로 선 반사 행렬을 만든다.',
'reflectPlane':'평면의 점과 법선으로 평면 반사 행렬을 만든다.',
'ellipse':'중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.',
'circle':'중심·반지름·평면 법선으로 원 경로 함수를 만든다.',
'polyline':'점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.',
'polygon':'마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.',
'rectangle':'중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.',
'linear':'입력 진행률을 선형 보간 진행률로 반환한다.',
'smoothStep':'진행률에 3t²-2t³ 완화 곡선을 적용한다.',
}
SPECIAL={
('GridLayout','valid'):'origin·extent·셀 개수와 frame의 유한성·양수 크기·가역성을 검사한다.',
('GridLayout','position'):'[0,1] 비율을 origin+extent*ratio로 바꾼 뒤 frame으로 월드 위치를 계산한다.',
('GridLayout','cellCenter'):'0부터 시작하는 셀 인덱스를 해당 셀 중심의 월드 위치로 변환한다.',
('GridLayout','relativeSize'):'양수 비율과 extent를 곱해 그리드 로컬 크기를 반환한다. frame 회전/배율은 적용하지 않는다.',
('GridLayout','positionInParent'):'비율 위치를 월드로 계산한 뒤 parentWorld의 역행렬로 부모 로컬 위치를 반환한다.',
('GridLayout','cellAt'):'월드 점을 frame 역변환으로 그리드 셀 인덱스에 대응시킨다. 그리드 밖이면 빈 결과다.',
('RoomBody','position'):'이탈 전에는 roomWorld로 로컬 중심을 월드로 변환하고 이탈 후에는 worldPosition을 반환한다.',
('TextureImage','valid'):'이미지 너비·높이와 RGBA 바이트 수가 맞는지 검사한다.',
('Diagnostics','clear'):'마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.',
('Scene','clear'):'Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.',
('MotionSystem','clear'):'모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.',
('Motion','sample'):'절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성한다. 의미 동작은 실행하지 않는다.',
('DistancePath','sample'):'누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.',
('Motion','rotation'):'축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.',
('TransformSystem','rotation'):'직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.',
('AnimationClock','reset'):'누적 시간을 0으로 되돌리고 재생 상태를 켠다.',
('Application','renderViews'):'현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.',
('Renderer','initialize'):'현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.',
}
def save(p,text):
    p.parent.mkdir(parents=True,exist_ok=True)
    if p.exists() and p.read_text(encoding='utf-8-sig')==text:return
    temp=p.with_name(p.name+'.api.tmp');temp.write_text(text,encoding='utf-8-sig',newline='\n');os.replace(temp,p)
def declarations(path):
    b=path.read_bytes();out=[];class_names=set()
    def text(n):return b[n.start_byte:n.end_byte].decode('utf-8') if n else ''
    def find(n):
        if n.type in ('function_declarator','operator_cast'):return n
        for c in n.named_children:
            if c.type not in ('parameter_list','compound_statement','template_argument_list'):
                d=find(c)
                if d:return d
    def walk(n,scope='',public=True):
        if n.type=='namespace_definition' and text(n.child_by_field_name('name'))=='detail':return
        if n.type in ('class_specifier','struct_specifier'):
            name=text(n.child_by_field_name('name'));body=n.child_by_field_name('body');vis=n.type=='struct_specifier'
            class_names.add(name)
            if body:
                for c in body.named_children:
                    if c.type=='access_specifier':vis=text(c)=='public'
                    else:walk(c,name,vis)
            return
        if n.type in ('declaration','field_declaration','function_definition'):
            d=find(n)
            if d and public:
                name='operator bool' if d.type=='operator_cast' else text(d.child_by_field_name('declarator'))
                owner=scope
                if '::' in name:
                    owner,name=name.rsplit('::',1)
                    # Out-of-class member definitions repeat the class declaration.
                    if n.type=='function_definition' and not scope and owner.rsplit('::',1)[-1] in class_names:
                        return
                body=n.child_by_field_name('body');end=body.start_byte if body else n.end_byte
                sig=b[n.start_byte:end].decode('utf-8').strip().rstrip(';').strip()
                out.append({'owner':owner,'name':name,'signature':re.sub(r'\s+',' ',sig),'file':path.relative_to(ROOT).as_posix(),'line':n.start_point.row+1})
            return
        for c in n.named_children:walk(c,scope,public)
    walk(PARSER.parse(b).root_node);return out
def description(r):
    owner,name=r['owner'],r['name']
    source_lines=(ROOT/r['file']).read_text(encoding='utf-8-sig').splitlines()
    preceding=r['line']-2
    if preceding>=0 and '/// [API]' in source_lines[preceding]:
        return source_lines[preceding].split('/// [API]',1)[1].strip()
    if '= delete' in r['signature']:return '이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.'
    if name==owner:return {'Application':'초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.','Scene':'빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.','Renderer':'GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.','AssetLoader':'프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.'}[owner]
    if name.startswith('~'):return '실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.'
    if name=='operator bool':return '모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.'
    additions={
        ('HumanoidPose','seated'):'허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다.',
        ('AnimationSystem','createJointAnimation'):'관절의 기준 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율이다.',
        ('AnimationSystem','poseHierarchy'):'기준 SRT와 모션을 합성하고 모든 관절이 유효하면 자세를 반영한다.',
        ('AnimationSystem','addHumanoid'):'편집 가능한 사람형 관절·메시 계층을 만든다. 구성 실패는 기존 Scene을 유지한다.',
        ('AnimationSystem','poseHumanoid'):'각 관절 위치를 유지하며 로컬 Euler 자세를 적용한다.',
        ('AnimationSystem','addSwingWithRider'):'고정 프레임·진자 관절·좌석·앉은 사람과 모션을 구성한다.',
        ('Motion','pendulum'):'축·진폭·주기·위상의 사인 진자 회전 단계를 만든다.',
        ('Scene','setFaceTransformMode'):'면의 계층 상속·월드 독립을 전환한다. 기본값은 현재 자세 보존이다.',
        ('PatternSystem','rotateFace'):'현재 면의 모서리 힌지·중심 자전·원본 모델 중심 공전을 구분해 회전을 설정한다.'}
    if (owner,name) in additions:return additions[owner,name]
    if owner=='SceneActions':return '지정 값과 대상 ID를 기존 API에 전달하는 '+name+' 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.'
    if owner=='DrawUniformContext':return '현재 바인딩된 program의 추가 uniform을 설정한다. 이름·GLSL 자료형과 값의 유한성을 맞춘다.'
    if owner=='CuboidMode':return {
        'setConfig':'초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.',
        'getConfig':'초기 설정의 읽기 전용 참조를 반환한다.',
        'setCuboid':'이름별 육면체 초기 설정을 교체하거나 추가한다.',
        'bindKey':'같은 key/action/mods 바인딩을 교체하거나 추가한다.',
        'unbindKey':'같은 key/action/mods 바인딩을 해제한다.',
        'getBindings':'키 바인딩 목록의 읽기 전용 참조를 반환한다.',
        'setSetupHandler':'새 장면 구성 후 실행할 설정 핸들러를 교체한다.',
        'setUpdateHandler':'프레임별 응용 갱신 핸들러를 교체한다.',
        'apply':'임시 CPU Scene에 육면체를 구성하고 성공하면 장면 전체를 교체한다.',
        'update':'등록된 모드 프레임 핸들러를 호출한다.',
        'dispatch':'일치하는 키 바인딩을 실행한다. 없는 키는 정상 처리한다.',
        'entity':'이름에 대응하는 마지막 생성 엔티티 ID를 조회한다.',
        'layer':'이름에 대응하는 초기 생성 레이어 ID를 조회한다.',
        'motion':'이름에 대응하는 초기 생성 모션 ID를 조회한다.',
        'action':'실행 시 육면체 이름을 해석하는 의미 명령 핸들러를 만든다.'}[name]
    if name=='bindCuboidMode':return '육면체 모드의 init/update/수정키 핸들러를 설치한다. 기존 해당 핸들러를 교체한다.'
    if name=='configureRenderer':return '현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.'
    if owner=='Motion' and name in ('action','insertAction','execute'):
        return {'action':'다른 단계와 조립할 의미 동작 단계를 만든다.',
                'insertAction':'주어진 인덱스에 람다·함수 포인터·의미 동작을 삽입한다.',
                'execute':'변환과 동작을 순서대로 평가한다. 외부 부수 효과는 복구하지 않는다.'}[name]
    if (owner,name) in SPECIAL:return SPECIAL[owner,name]
    if name in DESCRIPTIONS:return DESCRIPTIONS[name]
    if name in guide.PURPOSE:return guide.PURPOSE[name]
    raise ValueError('한글 기능 설명 누락: '+owner+'::'+name)
def result_contract(r):
    s=r['signature']
    if '= delete' in s:return '호출 불가(복사 금지 선언).'
    if 'std::optional<' in s.split('(')[0]:return '값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.'
    if re.search(r'\bbool\b',s.split('(')[0]):return 'true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.'
    if '*' in s.split('(')[0]:return '비소유 포인터다. nullptr과 원본 객체 수명·컨테이너 변경을 확인한다.'
    if '&' in s.split('(')[0]:return '비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.'
    return '반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.'
def main():
    paths=sorted((ROOT/'framework/include').glob('*.h'))
    # 기존 생성 주석을 제거하고 AST 위치를 다시 구해 반복 실행 시 중복을 막는다.
    for p in paths:
        s=re.sub(r'^[ \t]*/// \[API\].*\n','',p.read_text(encoding='utf-8-sig'),flags=re.M)
        save(p,s)
        records=declarations(p);lines=s.splitlines()
        for r in sorted(records,key=lambda r:r['line'],reverse=True):
            indent=re.match(r'\s*',lines[r['line']-1])[0]
            lines.insert(r['line']-1,indent+'/// [API] '+description(r))
        save(p,'\n'.join(lines)+'\n')
    records=[]
    for p in paths:
        for r in declarations(p):r.update(description=description(r),result=result_contract(r));records.append(r)
    save(ROOT/'framework/docs/API_KO.json',json.dumps(records,ensure_ascii=False,indent=2)+'\n')
    md='# 프레임워크 공개 API 한글 목록\n\n대상: `framework/include/*.h`의 공개 함수·메서드·생성/소멸·변환 연산자. 비공개 멤버, detail 구현, SDK 함수는 제외한다. 오버로드와 const 구분은 각각 기록한다. 클래스 외부의 멤버 정의는 클래스 내부 선언과 중복 수록하지 않는다. 각 함수의 한글 주석은 선언 바로 위에 있다.\n\n'
    md+='이 문서는 API의 전체 탐색 목록이다. 시험장에서 사용할 호출 순서는 [공통 행동 그래프](../../docs/code-guide/exam.html)와 [요구사항별 API 호출](../../docs/code-guide/api.html)을 먼저 읽는다.\n\n'
    md+='## 공통 계약\n\n- 시간은 초, 각도는 별도 표기가 없으면 도다. `glm::radians`에 넣은 뒤 GLM 회전 행렬을 만든다.\n- 부모가 있는 엔티티의 위치는 부모 로컬이다. GridLayout.position은 월드 위치, relativeSize는 그리드 로컬 크기다.\n- bool/optional/포인터를 검사한 뒤 결과를 사용한다. 빈 피킹 결과·공중 jump의 false는 정상적인 판정일 수 있다.\n- Application은 모션을 프레임당 한 번 갱신한다. 여러 RenderPass에서도 물리와 시간을 중복 갱신하지 않는다.\n- ID는 소유 포인터가 아니다. Scene.clear 후 엔티티 ID를 다시 받는다. GPU 자원은 현재 컨텍스트에서 생성·해제한다.\n- Front cull은 winding에 따른 면 제거다. 법선 기반 명암과 다르다. 현재 셰이더에는 투영 그림자가 없다.\n\n## 파일별 목록\n\n'
    for p in paths:
        rows=[r for r in records if r['file']==p.relative_to(ROOT).as_posix()]
        md+='- ['+p.name+'](#'+p.stem.lower()+'): '+str(len(rows))+'개 선언\n'
    for p in paths:
        rows=[r for r in records if r['file']==p.relative_to(ROOT).as_posix()]
        md+='\n## '+p.stem+'\n\n'
        if not rows:md+='공개 함수 없음. 데이터/내부 지원 헤더다.\n'
        for r in rows:
            full=(r['owner']+'::' if r['owner'] else '')+r['name']
            md+='### '+full+'\n\n```cpp\n'+r['signature']+';\n```\n\n'+r['description']+'\n\n입력: 위 시그니처의 타입·기본값과 기능 설명을 함께 읽는다.\n\n반환·실패: '+r['result']+'\n\n소스: [`'+r['file']+':'+str(r['line'])+'`](../include/'+p.name+')\n\n'
    md+='\n## 확인 범위\n\n소스 선언을 AST로 읽어 만든 목록이며 빌드·테스트·실행을 수행하지 않았다. 사용자에게 API 사용 코드와 화면 확인을 맡긴다.\n'
    save(ROOT/'framework/docs/API_KO.md',md)
    print('공개 API 한글 주석 및 목록 생성: '+str(len(records))+'개 선언')
if __name__=='__main__':main()
````
