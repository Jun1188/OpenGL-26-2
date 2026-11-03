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
