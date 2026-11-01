"""Code-only graphify extraction. No builds, tests, benchmarks, or semantic API calls."""
from pathlib import Path
import argparse
import collections
import datetime
import hashlib
import json
import re
import subprocess
import xml.etree.ElementTree as ET
import graphify.extract as extraction_api
from graphify.detect import detect, save_manifest
from graphify.build import build_from_json
from graphify.cluster import cluster, score_all
from graphify.analyze import god_nodes, surprising_connections, suggest_questions
from graphify.report import generate
from graphify.export import to_json, to_html
from graphify.diagnostics import diagnose_extraction, format_diagnostic_report

REPOSITORY = Path(__file__).resolve().parent.parent

def write(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(value.rstrip()+'\n', encoding='utf-8-sig')

def dump(path, value):
    write(path, json.dumps(value, ensure_ascii=False, indent=2))

def project_inventory():
    ns={'m':'http://schemas.microsoft.com/developer/msbuild/2003'}
    projects=[]
    for p in sorted(REPOSITORY.rglob('*.vcxproj')):
        if any(x in p.parts for x in ['dependencies','packages','.vs','x64','Debug','Release','.obj-work','dist']):continue
        sources=[];entries=[]
        for e in ET.parse(p).findall('.//m:ClCompile',ns):
            include=e.get('Include')
            if not include:continue
            source=(p.parent/include.replace('\\','/')).resolve()
            if not source.is_file():continue
            relative=source.relative_to(REPOSITORY).as_posix();sources.append(relative)
            text=source.read_text(encoding='utf-8-sig')
            for m in re.finditer(r'\b(?:int|void)\s+(main|wmain)\s*\(',text):
                line=text[:m.start()].count('\n')+1
                item={'file':relative,'line':line,'name':m.group(1)}
                if 'computer_graphics_warmUp1/' in relative:
                    problem=1 if line<550 else 2 if line<765 else 3 if line<876 else 4
                    item.update(condition=f'PROB == {problem}',active_by_source_default=problem==2)
                entries.append(item)
        projects.append({'project':p.relative_to(REPOSITORY).as_posix(),'sources':sources,'entrypoints':entries})
    for name in ['basic','assets','motion']:
        projects.append({'project':None,'standalone_example':True,'sources':[f'framework/examples/{name}.cpp'],
                         'entrypoints':[{'file':f'framework/examples/{name}.cpp','name':'main'}]})
    return projects

def add_source_anchors(extraction, files, root):
    """Anchor qualified inline definitions and project-specific callback registrations."""
    anchors={}
    for p in files:
        rel=p.relative_to(root).as_posix();text=p.read_text(encoding='utf-8-sig')
        file_id='review_file_'+hashlib.sha256(rel.encode()).hexdigest()[:20]
        extraction['nodes'].append({'id':file_id,'label':rel,'source_file':rel,
                                    'source_location':'L1','file_type':'code','_origin':'source_definition_review'})
        for m in re.finditer(r'^(?:inline\s+)?[\w:<>,*& ]+\s+((?:\w+::)+\w+|main|wmain)\s*\([^;]*?\)\s*(?:const\s*)?\{',text,re.M):
            symbol=m.group(1);line=text[:m.start()].count('\n')+1
            ident='source_'+hashlib.sha256(f'{rel}:{line}:{symbol}'.encode()).hexdigest()[:20]
            node={'id':ident,'label':symbol,'source_file':rel,'source_location':f'L{line}',
                  'file_type':'code','_callable':True,'_origin':'source_definition_review'}
            extraction['nodes'].append(node);anchors[(rel,symbol)]=node
            extraction['edges'].append({'source':file_id,'target':ident,'relation':'defines',
                'confidence':'EXTRACTED','source_file':rel,'source_location':f'L{line}'})
    callback_projects=[(f'openGL_3dAssign_{number}','practice') for number in range(13,24)]
    callback_projects.append(('standard','sample'))
    for directory,receiver in callback_projects:
        rel=f'{directory}/main.cpp'
        if (rel,'main') not in anchors:continue
        text=(root/rel).read_text(encoding='utf-8-sig')
        for match in re.finditer(r'\b\w+\.(initScene|onKey|onUpdate|onFocus|onMouse|onRender|onCursor)\s*\(',text):
            target=anchors.get((f'{directory}/Console.cpp','Console::'+match.group(1)))
            if not target:continue
            extraction['edges'].append({'source':anchors[(rel,'main')]['id'],'target':target['id'],
                'relation':'registered_callback_body_calls','confidence':'EXTRACTED','source_file':rel,
                'source_location':'L'+str(text[:match.start()].count('\n')+1),
                'context':f'{directory}: lambda executes later through Application callbacks'})
    return len(anchors)

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--scope',choices=['framework','repository'],default='framework')
    args=parser.parse_args();framework=args.scope=='framework'
    root=REPOSITORY/'framework' if framework else REPOSITORY
    out=REPOSITORY/'framework-analysis/graphify-out' if framework else REPOSITORY/'graphify-out'
    cache=out/'cache';out.mkdir(parents=True,exist_ok=True)
    excluded={'dependencies','packages','.vs','x64','Debug','Release','archive','graphify-out','framework-analysis','.agents','docs','.obj-work','dist'}
    files=[]
    for p in root.rglob('*'):
        relative=p.relative_to(root)
        if not p.is_file() or p.suffix.lower() not in {'.cpp','.h','.hpp','.inl'}:continue
        if any(part in excluded for part in relative.parts):continue
        if not framework and 'framework' in relative.parts and relative.parts[0]!='framework':continue
        files.append(p)
    files.sort()
    detection=detect(root,extra_excludes=['**/dependencies/**','**/packages/**','**/x64/**','**/archive/**','**/.vs/**','**/.obj-work/**','**/dist/**'],cache_root=cache)
    detection['files']={'code':[str(p) for p in files],'document':[],'paper':[],'image':[],'video':[]}
    detection['total_files']=len(files)
    detection['total_words']=sum(len(p.read_text(encoding='utf-8-sig').split()) for p in files)
    print(f'Corpus: {len(files)} code files, {detection["total_words"]} words; SDKs and duplicate distributions excluded.',flush=True)
    if not files or len(files)>500 or detection['total_words']>2000000:raise RuntimeError('Unexpected corpus size; preserve existing outputs.')
    if detection.get('skipped_sensitive'):print('Detection skipped sensitive names:',detection['skipped_sensitive'],flush=True)
    extraction_api._DISPATCH['.inl']=extraction_api.extract_cpp
    ordinary=[p for p in files if p.suffix.lower()!='.inl']
    extraction=extraction_api.extract(ordinary,cache_root=cache,root=root)
    # Worker processes do not inherit the process-local .inl dispatcher.
    for inline in (p for p in files if p.suffix.lower()=='.inl'):
        part=extraction_api.extract([inline],cache_root=cache,root=root)
        if not part.get('nodes'):raise RuntimeError('Inline extraction failed: '+str(inline))
        extraction['nodes'].extend(part['nodes']);extraction['edges'].extend(part['edges'])
    if not extraction.get('nodes'):raise RuntimeError('Empty AST extraction; preserve existing graph.')
    anchors=add_source_anchors(extraction,files,root)
    nodes={n['id']:n for n in extraction['nodes']};ambiguous=0
    for e in extraction['edges']:
        a=nodes.get(e.get('source'),{}).get('source_file','').replace('\\','/').split('/')[0]
        b=nodes.get(e.get('target'),{}).get('source_file','').replace('\\','/').split('/')[0]
        if not framework and a!=b and a and b and b!='framework' and e.get('relation')=='calls':
            e['confidence']='AMBIGUOUS';e['audit_note']='Same symbol across independent programs; not a confirmed runtime call.';ambiguous+=1
    graph=build_from_json(extraction,root=str(root),directed=True)
    if not graph.number_of_nodes():raise RuntimeError('Empty graph; preserve existing outputs.')
    # A smaller graph is valid when obsolete source files have been removed.
    communities=cluster(graph);scores=score_all(graph,communities)
    labels={}
    for cid,members in communities.items():
        counts=collections.Counter(graph.nodes[n].get('source_file','').replace('\\','/').split('/')[0] for n in members)
        labels[cid]=(counts.most_common(1)[0][0] if counts else '공통')+' 코드 구조'
    gods=god_nodes(graph);surprises=surprising_connections(graph,communities);questions=suggest_questions(graph,communities,labels)
    commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=REPOSITORY,text=True).strip()
    staged=out/'graph.generated.json'
    if not to_json(graph,communities,str(staged),force=True,built_at_commit=commit,community_labels=labels):raise RuntimeError('Graph export failed.')
    write(staged,staged.read_text(encoding='utf-8-sig'));staged.replace(out/'graph.json')
    report=generate(graph,communities,scores,labels,gods,surprises,detection,{'input':0,'output':0},str(root),suggested_questions=questions,built_at_commit=commit)
    report+='\n## 해석 범위\n\n코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.\n'
    write(out/'GRAPH_REPORT.md',report)
    health=diagnose_extraction(extraction,directed=True,root=str(root))
    write(out/'GRAPH_HEALTH.md',format_diagnostic_report(health)+f'\nCross-program calls marked AMBIGUOUS: {ambiguous}. Source anchors: {anchors}.\n')
    if graph.number_of_nodes()>5000:print('Warning: HTML graph exceeds 5000 nodes.',flush=True)
    if not to_html(graph,communities,str(out/'graph.html'),community_labels=labels):raise RuntimeError('HTML export failed.')
    write(out/'graph.html',(out/'graph.html').read_text(encoding='utf-8-sig'))
    dump(out/'EXTRACTION.json',extraction);dump(out/'RELATIONS.json',extraction);dump(out/'DETECTION.json',detection)
    dump(out/'COMMUNITIES.json',labels);dump(out/'ANALYSIS.json',{'gods':gods,'surprises':surprises,'questions':questions,'cohesion':scores})
    dump(out/'PROJECTS.json',project_inventory())
    hashes={p.relative_to(root).as_posix():hashlib.sha256(p.read_bytes()).hexdigest() for p in files}
    dump(out/'SOURCE_HASHES.json',hashes)
    dump(out/'SOURCE_MANIFEST.json',{'source_root':'.' if not framework else 'framework','scope':args.scope,
         'source_commit':commit,'includes_uncommitted_changes':True,'files':hashes})
    temporary=out/'manifest.generated.json'
    save_manifest({'code':[str(p) for p in files]},str(temporary),kind='ast',root=root,scan_corpus=[str(p) for p in files])
    write(out/'manifest.json',temporary.read_text(encoding='utf-8-sig'));temporary.unlink()
    dump(out/'cost.json',{'date':datetime.datetime.now(datetime.timezone.utc).isoformat(),'mode':'AST + source anchors',
        'input_tokens':0,'output_tokens':0,'external_api_calls':0,'files':len(files)})
    dump(out/'BENCHMARK.json',{'status':'not_run','reason':'Builds, tests and benchmarks are delegated to the user.'})
    # Artifact encoding checks are part of graph generation, not executable tests.
    for name in ['graph.json','graph.html','GRAPH_REPORT.md','EXTRACTION.json','PROJECTS.json']:
        value=(out/name).read_text(encoding='utf-8-sig')
        if '\ufffd' in value:raise RuntimeError('Unicode replacement character in '+name)
    print(f'Graph complete: {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges, {len(communities)} communities.',flush=True)
    print(format_diagnostic_report(health),flush=True)

if __name__=='__main__':main()
