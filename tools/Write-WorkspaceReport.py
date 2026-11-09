"""Record source changes, entrypoint flows and unexecuted checks; no build/test execution."""
from pathlib import Path
import difflib
import hashlib
import json
import re
import subprocess

ROOT=Path(__file__).resolve().parents[1]
STANDARD=ROOT/'standard'
BASE='e6af6deb4b2c8acc5028307592b654bcaeebbf4d'
OUT=ROOT/'docs/workspace-redesign'

def git(*args):return subprocess.check_output(['git','-c','core.autocrlf=false',*args],cwd=ROOT).decode('utf-8')
def write(path,text):
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_text(text.rstrip()+'\n',encoding='utf-8-sig')
def source_before(path):
    result=subprocess.run(['git','show',BASE+':'+path],cwd=ROOT,capture_output=True)
    return result.stdout.decode('utf-8-sig') if result.returncode==0 else ''
def codeblock(text,language):
    fence='`'*max(4,1+max((len(x) for x in re.findall(r'`+',text)),default=0))
    return fence+language+'\n'+text.rstrip('\n')+'\n'+fence+'\n'
def location(lines,line):
    for i in range(min(line,len(lines))-1,-1,-1):
        if re.match(r'^\s*(?:def |class |struct |(?:static |std::\S+ |bool |void |Tokens |int |glm::\S+ )+(?:\w+::)*\w+\s*\()',lines[i]):
            return lines[i].strip()
    return 'file scope'

def main():
    baseline_files=set(git('ls-tree','-r','--name-only',BASE).splitlines())
    changed=set(git('diff','--name-only',BASE,'--').splitlines())
    new=git('ls-files','--others','--exclude-standard').splitlines()
    allowed=('framework/','standard/','tools/','docs/workspace-redesign/','graphify-out/standard-composition/','framework-analysis/api-reference/')
    changed.update(p for p in new if p.startswith(allowed) and '/__pycache__/' not in p)
    code_suffixes={'.cpp':'cpp','.h':'cpp','.inl':'cpp','.glsl':'glsl','.py':'python','.ps1':'powershell','.vcxproj':'xml'}
    report='# 전체 코드 변경 전후\n\n기준 커밋 `'+BASE+'`부터 현재 저장된 코드까지의 모든 변경 블록이다. 문서·그래프 자동 산출물은 manifest에 기록하고 본문 코드 대조에서 제외한다. 빌드·테스트·검증기는 실행하지 않았다.\n\n'
    manifest=[]
    for name in sorted(changed):
        path=ROOT/name
        data=path.read_bytes() if path.exists() else b''
        manifest.append({'path':name,'new':name not in baseline_files,'exists':path.exists(),'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest() if data else None})
        if path.suffix not in code_suffixes or '/dependencies/' in name:continue
        before=source_before(name).replace('\r\n','\n')
        after=data.decode('utf-8-sig').replace('\r\n','\n') if data else ''
        left=before.splitlines(keepends=True);right=after.splitlines(keepends=True)
        groups=list(difflib.SequenceMatcher(None,left,right,autojunk=False).get_grouped_opcodes(0))
        if not groups:continue
        report+='## '+name+'\n\n'
        for index,group in enumerate(groups,1):
            first,last=group[0],group[-1]
            i,j=first[1],last[2];a,b=first[3],last[4]
            old=''.join(left[i:j]);current=''.join(right[a:b])
            report+='### 블록 '+str(index)+' · before L'+str(i+1)+' / after L'+str(a+1)+'\n\n위치: `'+location(right,a+1).replace('`','')+'`\n\n'
            report+='변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.\n\n'
            report+='**before**'+(' (없음)' if not old else '')+'\n\n'+(codeblock(old,code_suffixes[path.suffix]) if old else '')+'\n'
            report+='**after**'+(' (없음)' if not current else '')+'\n\n'+(codeblock(current,code_suffixes[path.suffix]) if current else '')+'\n'
    write(OUT/'ALL_CODE_BEFORE_AFTER.md',report)
    write(OUT/'CHANGE_MANIFEST.json',json.dumps({'baseline':BASE,'source_state':'Saved working tree during step 5; existing user docs.zip excluded.','files':manifest},ensure_ascii=False,indent=2))
    graph_dir=ROOT/'graphify-out/standard-composition'
    source_manifest=json.loads((graph_dir/'SOURCE_MANIFEST.json').read_text(encoding='utf-8-sig'))
    projects=json.loads((graph_dir/'PROJECTS.json').read_text(encoding='utf-8-sig'))
    graph=json.loads((graph_dir/'graph.json').read_text(encoding='utf-8-sig'))
    nodes={n['id']:n for n in graph['nodes']}
    links=graph.get('links',graph.get('edges',[]))
    architecture='# 실행 구조와 소스 관계\n\n'
    architecture+='graphify AST 산출물: '+str(source_manifest['nodes'])+' nodes / '+str(source_manifest['edges'])+' edges. 코드 전용 추출이며 의미 분석 API 토큰 사용은 0이다. 빌드·테스트·진단기·앱·브라우저는 실행하지 않았다. 그래프는 정적 관계이고 동적 디스패치의 실제 실행 증거가 아니다.\n\n'
    architecture+='## 현재 Standard\n\n'
    architecture+='1. `standard/main.cpp::main`이 입력·초기화·갱신·렌더 콜백을 한 번 등록한 뒤 `Application::run`을 호출한다. 등록 시점과 콜백 실행 시점은 다르다.\n'
    architecture+='2. run 초기화 → Console::initScene → defaultRecipe → Assembly::execute → resolveCommand → dispatch → applyPoses(0). 실습 번호 라우팅·FeatureContainer·GuideRecipes는 Markdown 참조 문서로 전환했다. Standard는 작업공간 소스만 컴파일한다.\n'
    architecture+='3. 이벤트 루프 PollEvents → Console 입력 콜백 → Assembly 입력 → 피킹 또는 명령 실행. 단축키 미리보기는 작성 상태와 Scene 사본을 보관한다. Enter는 확정, Esc/포커스 이탈은 복원한다.\n'
    architecture+='4. Console::onUpdate → 콘솔 입력을 최대8줄 처리 → Assembly::update → 시계 진행 → applyPoses. 기준 SRT·부착·지역 모션을 Scene에 전달한 뒤 힘을 적분하고 광원·카메라·축을 동기화한다.\n'
    architecture+='5. 사용자 update 콜백 후 Application은 Scene::updateMotions를 한 번 실행한다. Console::onRender → Assembly::render → renderScene 또는 renderViews → Renderer. 렌더 패스에서는 시간을 진행하지 않는다.\n'
    architecture+='6. Scene::worldMatrix는 객체의 부모 체인과 attachment/base/localLayers를 합성한다. faceWorldMatrix를 렌더·피킹이 함께 사용한다. Renderer는 CPU 정점/인덱스를 VAO/VBO/EBO로 업로드하고 모델·정규행렬·점광원 uniform을 전송한다. Shaders.h와 shaders/*.glsl이 GPU 입력·출력 계약이다.\n'
    architecture+='7. 종료 요청 후 run의 정리 경로가 GPU/창/GLFW 자원을 해제한다. ConsoleInput 소멸자는 원래 Windows 콘솔 입력 모드를 복원한다. 소유 객체와 비소유 콜백 참조의 수명은 Application/Console 범위에 종속된다.\n\n'
    architecture+='## 독립 실행 프로젝트와 main\n\n각 행은 별도 실행 프로그램이다. 서로 다른 프로젝트의 Console/main을 하나의 호출 흐름으로 합치지 않는다. 부재 파일은 과거 기록이며 복원하지 않았다.\n\n'
    for project in projects:
        label=project.get('project',project.get('file','example'))
        architecture+='### '+label+'\n\n'
        if project.get('available') is False:
            architecture+='작업 전 부재. 진입점 정보는 historical이며 현재 실행 경로를 확인할 수 없다.\n\n';continue
        entries=project.get('entrypoints',[])
        if not entries and project.get('standalone_example'):entries=[{'file':project['file'],'name':project.get('entrypoint','main')}]
        for entry in entries:
            file=entry['file'];line=entry.get('line','?')
            architecture+='- `'+file+':'+str(line)+'` · '+entry.get('name','main')
            if entry.get('conditional_compilation'):architecture+=' · 조건부 컴파일 분기는 소스의 전처리 조건별로 구분한다.'
            architecture+='\n'
            mains=[n for n in nodes.values() if n.get('source_file','').replace('\\','/')==file and n.get('label','').split('::')[-1] in ('main','wmain')]
            callees=[]
            for n in mains:
                for edge in links:
                    if edge.get('source')==n['id'] and edge.get('relation')=='calls':
                        target=nodes.get(edge.get('target'),{})
                        if target:callees.append(target.get('label','?')+' ('+target.get('source_file','?')+':'+target.get('source_location','?')+', '+str(edge.get('confidence','EXTRACTED'))+')')
            if callees:architecture+='- AST 직접 관계: '+'; '.join(sorted(set(callees)))+'. 콜백 본문의 지연 실행은 위상상 별도다.\n'
            else:architecture+='- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.\n'
        architecture+='\n'
    architecture+='## 제한과 참조\n\n과거 실습 소스도 AST에 남으므로 노드 존재만으로 Standard 실행 경로에 포함됐다고 판단하지 않는다. PROJECTS.json은 vcxproj의 명시 Compile 목록과 Exclude를 반영한다. 그래프의 같은 이름·서로 다른 실행 파일 사이 호출 후보는 AMBIGUOUS로 구분한다. 실제 오버로드·함수 포인터·가상 호출 전체를 증명하지 않는다. 해시 manifest는 생성 당시의 소스를 가리킨다.\n\n'
    architecture+='[대화형 그래프](graph.html) · [파일·프로젝트](PROJECTS.json) · [소스 해시](SOURCE_MANIFEST.json)\n'
    write(graph_dir/'GRAPH_REPORT.md',architecture)
    write(OUT/'ARCHITECTURE.md',architecture)
    portable=STANDARD/'docs/assembly/architecture'
    portable.mkdir(parents=True,exist_ok=True)
    for name in ('graph.json','graph.html','PROJECTS.json','SOURCE_MANIFEST.json','GRAPH_REPORT.md'):
        (portable/name).write_bytes((graph_dir/name).read_bytes())
    write(portable/'README.md','# 구조 분석\n\n현재 Standard 실행 흐름과 독립 프로젝트 구분은 [GRAPH_REPORT](GRAPH_REPORT.md)를 따른다. AST는 실행 확인이 아니다.\n')
    candidates=[]
    for p in sorted((ROOT/'tools/__pycache__').glob('*.pyc')):
        rel=p.relative_to(ROOT).as_posix()
        if rel in new:candidates.append((p,p.stat().st_size))
    cleanup='# 임시 파일 후보\n\n문서 생성기의 Python import가 만든 캐시다. 제품 실행에는 필요하지 않으며 Python이 다시 생성할 수 있다. 별도 삭제 승인이 없어 보존했다.\n\n| 정확한 경로 | 바이트 | 보존 이유 |\n|---|---:|---|\n'
    for path,size in candidates:cleanup+='| `'+str(path)+'` | '+str(size)+' | 삭제 승인 대기 |\n'
    cleanup+='\n기존 docs.zip, dependencies, 실행/설정/사용자 데이터, graphify 증분 캐시는 보존했다. 그래프 캐시는 후속 분석에 사용한다.\n'
    write(OUT/'CLEANUP_CANDIDATES.md',cleanup)
    write(OUT/'RESULT.md','''# 반영 결과와 확인 경계

- 승인된 0번 작업공간, 비균등 배율·부착·지역 레이어·피킹·단축키·광원·카메라·힘 연결을 소스에 반영했다.
- 실습 재출제를 가정하지 않는 연산별 템플릿 9개와 요구 문장 → 변환·공간·API 레퍼런스를 작성했다.
- 저울은 공통 변환과 힘을 조합한 영자연길이 선형 스프링의 응용 예제로 구분했다.
- 콘솔 도움말은 명령어 [결과] - 각 인수 의미 형식이다. 실행 결과에는 해석 인수와 현재 선택·월드 원점을 표시한다.
- API 카탈로그의 기존 pick 오버로드 앵커를 유지하고 새 공개 선언을 추가했다. 문서·그래프 생성기를 실행했다.
- 소스 검토와 산출물 생성만 수행했다. 빌드·테스트·검증기·앱·브라우저 실행은 하지 않았다. 수치 안정성·피킹/화면·셰이더·MSVC 컴파일은 사용자 확인 사항이다.
- 실제 절차와 예상 관찰점은 standard/USER_CHECKS.md에 있다. 외부 side effect가 있는 사용자 콜백, 충돌·마찰·강체 제약 솔버는 자동 처리하지 않는다.

## 단계별 커밋

1. c9fc9f4: 0번 독립 작업공간과 문서 런타임 분리.
2. 0c6aa6f: 월드 부착·비균등 변환·지역 레이어·피킹/단축키·명시적 저장.
3. d929c64: 점광원·카메라 객체와 셰이더.
4. 4e1a328: 이동 기준 프레임의 힘 API와 스프링 저울 조합.
5. 문서·템플릿·그래프·연결부 보완: 이 보고서를 포함한 최종 커밋이며 해시는 최종 응답에서 제공한다.

[전체 코드 before/after](ALL_CODE_BEFORE_AFTER.md) · [변경 파일](CHANGE_MANIFEST.json) · [실행 구조](ARCHITECTURE.md) · [정리 후보](CLEANUP_CANDIDATES.md)
''')
    # Hash the final outputs after generating them; the manifest excludes its own recursive hash.
    final_names=set(changed)
    final_names.update(p for p in git('ls-files','--others','--exclude-standard').splitlines() if p.startswith(allowed) and '/__pycache__/' not in p)
    final_manifest=[]
    for name in sorted(final_names):
        if name=='docs/workspace-redesign/CHANGE_MANIFEST.json':continue
        path=ROOT/name;data=path.read_bytes() if path.exists() else b''
        final_manifest.append({'path':name,'new':name not in baseline_files,'exists':path.exists(),'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest() if data else None})
    write(OUT/'CHANGE_MANIFEST.json',json.dumps({'baseline':BASE,'source_state':'Saved working tree during step 5; docs.zip and manifest self-hash excluded.','files':final_manifest},ensure_ascii=False,indent=2))
    print('Wrote source before/after, manifests, architecture and execution limits. No build/tests run.')

if __name__=='__main__':main()
