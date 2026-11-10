"""Write complete own-code before/after and source architecture reports; no builds/tests."""
from pathlib import Path
import datetime
import hashlib
import json
import re
import subprocess
ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'coding-test-2/reports'
BASE='5487f4d0514598dca6c3b567e616b836e8e97773'
SCOPES=['framework','standard','coding-test-2','tools/Write-CodingTest2Guide.py','tools/Write-CodingTest2Report.py',
        'tools/Write-SourceDebuggerDocs.py','tools/Update-StandardDocs.py','tools/Package-Standard.py','tools/standard_package_policy.py','.gitignore']
EXT={'.h','.cpp','.glsl','.py','.js','.css','.props','.sln','.vcxproj'}
def git(*args): return subprocess.check_output(['git',*args],cwd=ROOT)
def write(p,s):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(s.rstrip()+'\n',encoding='utf-8-sig')
def fence(text):
    runs=re.findall(r'`+',text)
    return '`'*max(3,max((len(x)+1 for x in runs),default=3))
def block(text,path):
    f=fence(text);lang={'.cpp':'cpp','.h':'cpp','.glsl':'glsl','.py':'python','.js':'javascript','.css':'css','.props':'xml','.vcxproj':'xml'}.get(Path(path).suffix,'text')
    return f+lang+'\n'+text.rstrip()+'\n'+f

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    paths=set(git('diff','--name-only','-z',BASE,'--',*SCOPES).decode('utf-8').split('\0'))
    paths.update(git('ls-files','--others','--exclude-standard','-z','--',*SCOPES).decode('utf-8').split('\0'))
    paths.discard('')
    own=sorted(p for p in paths if (Path(p).suffix in EXT or p=='.gitignore')
        and '/dependencies/' not in p and p!='coding-test-2/assets/marked.umd.js')
    before=['# 전체 코드 수정 전후','',f'기준: `{BASE}`. 이 코딩테스트 준비 작업을 시작하기 직전 커밋이다. 아래는 승인 범위의 작성 코드 전체 before/after이며 diff 일부가 아니다. 새 파일은 before 없음으로 표시한다. SDK·marked의 배포 사본은 별도 provenance에 경로·해시·원본 대응을 기록한다.','',
      '빌드·테스트·검증기·앱·브라우저는 실행하지 않았다. 행 번호 안내와 코드 스냅샷은 저장된 소스 기준이며 실행 성공 증거가 아니다.','']
    manifest=[]
    for path in own:
        exists=subprocess.run(['git','cat-file','-e',BASE+':'+path],cwd=ROOT,stdout=subprocess.DEVNULL,stderr=subprocess.DEVNULL).returncode==0
        old=git('show',BASE+':'+path).decode('utf-8-sig') if exists else ''
        p=ROOT/path;new=p.read_text(encoding='utf-8-sig') if p.exists() else ''
        before += ['## '+path,'','### Before','',block(old,path) if exists else '(새 파일: before 없음)','','### After','',block(new,path) if p.exists() else '(삭제됨)','']
        manifest.append({'path':path,'before_exists':exists,'before_lines':len(old.splitlines()),'after_lines':len(new.splitlines()),'sha256':hashlib.sha256(p.read_bytes()).hexdigest() if p.exists() else None})
    write(OUT/'ALL_CODE_BEFORE_AFTER.md','\n'.join(before))
    # Exact copied third-party inputs are documented by hash and local origin, not pasted as authored code.
    origins={}
    for p in (ROOT/'standard/framework/dependencies').rglob('*'):
        if p.is_file(): origins.setdefault(hashlib.sha256(p.read_bytes()).hexdigest(),[]).append(p.relative_to(ROOT).as_posix())
    provenance=[]
    for p in sorted((ROOT/'coding-test-2/native/dependencies').rglob('*')):
        if p.is_file():
            digest=hashlib.sha256(p.read_bytes()).hexdigest()
            provenance.append({'path':p.relative_to(ROOT).as_posix(),'bytes':p.stat().st_size,'sha256':digest,'matching_local_origins':origins.get(digest,[])})
    for name in ('marked.umd.js','marked-LICENSE.md'):
        p=ROOT/'coding-test-2/assets'/name;original=ROOT/'tools/templates/standard-viewer/vendor'/name
        provenance.append({'path':p.relative_to(ROOT).as_posix(),'bytes':p.stat().st_size,'sha256':hashlib.sha256(p.read_bytes()).hexdigest(),'local_origin':original.relative_to(ROOT).as_posix(),'change':'UTF-8 BOM text distribution copy; algorithm unchanged'})
    write(OUT/'THIRD_PARTY_COPIES.json',json.dumps(provenance,ensure_ascii=False,indent=2))
    head=git('rev-parse','HEAD').decode().strip()
    graph=json.loads((ROOT/'graphify-out/coding-test-2/SOURCE_MANIFEST.json').read_text(encoding='utf-8-sig'))
    write(OUT/'CHANGE_MANIFEST.json',json.dumps({'baseline':BASE,'head_when_generated':head,'source_state':'saved working tree','own_code':manifest,'graph':{k:graph[k] for k in ('nodes','edges','method')},'not_run':['build','test','validator','application','browser','archive extraction/run']},ensure_ascii=False,indent=2))
    architecture='''# 실행 흐름과 구조 근거

기준 소스는 SOURCE_GUIDE.md의 실제 파일·행과 graphify-out/coding-test-2/SOURCE_MANIFEST.json의 파일 해시다. 소스 AST 그래프는 실행 순서의 증명이 아니므로 콜백 순서는 Application.cpp·Console.cpp를 함께 읽었다. 동명이인 함수의 프로젝트 간 연결은 AMBIGUOUS로 표시될 수 있다.

```mermaid
flowchart TD
  Native[native main: GLFW/GLEW 초기화] --> Buffers[정점/VAO/VBO]
  Buffers --> NativeLoop[입력 상태 / dt 갱신]
  NativeLoop --> NativeGPU[MVP/normal/light uniform -> 외부 GLSL -> draw]
  Std[Standard main + Application] --> Setup[SceneSetup: 초기 기하/재질/프레임]
  Setup --> Controller[ExamController: 입력/상태/update dt]
  Controller --> Author[SceneAuthoring update seconds]
  Author --> Motions[Application: native Scene motions]
  Motions --> Final[Console onRender: syncTools + syncVisuals]
  Final --> Renderer[Renderer: model/normal/light/material/custom uniform]
  Renderer --> GLSL[실행 폴더의 framework/shaders]
  Debugger[선택 debugger] --> Disk[저장된 소스 읽기]
  Disk --> Proposal[실제 경로/행/before/after]
  Proposal --> User[사용자 직접 편집/저장/빌드]
```

## 모듈 경계

- framework 원본과 standard/framework 배포 사본에 동일한 재사용 API를 반영했다: addModel/setLayer/light/lightParent/setLight/syncTools 및 항별 셰이더 출력. 다른 실습의 framework 복사본은 일괄 덮어쓰지 않았다.
- Standard는 SceneSetup/Console/ExamController와 자기 framework를 사용한다. debugger와 학습 HTML이 없을 때도 프로젝트 참조를 바꾸지 않는다. 예제 전체 교체 시 Console(app,false)로 기본 시험 컨트롤러를 끌 수 있다.
- native는 Framework.h 없이 직접 GL 자원을 생성·해제한다. 자체 SDK와 DLL을 포함하며 Standard/상위 저장소를 링크하지 않는다.
- debugger는 독립 Windows C++17 프로그램이다. 대상 소스를 실행/저장하지 않는다. where는 소스 hook과 현재 행을 찾는다. MSBuild 분기·C++ 의미까지 자동 판정하지 않는다.
- root coding-test-2의 HTML/N제/시나리오는 학습 자료다. 제출 ZIP에는 포함하지 않는다. 타이머·진도는 브라우저 기능이며 C++ 런타임의 종속성이 아니다.

## 상태와 공간의 소유권

공전 위상은 dt 적분으로 정지/방향 전환에 연속적이다. key의 부모 지역 좌표, 최종 부모 월드 변환, 표시·궤도 파생을 나누었다. 카메라는 기본 컨트롤러가 월드 상태로 소유한다. 다른 부착 카메라와 동시 제어하지 않는다. Space는 authoring 시간과 이 컨트롤러만 정지한다. 별도 native Motion은 별도 제어가 필요하다. 접촉·강체·관절 솔버를 새로 제공한다고 주장하지 않는다.

## 그래프와 전체 코드

[인터랙티브 소스 구조](../../graphify-out/coding-test-2/graph.html) · [전체 before/after](ALL_CODE_BEFORE_AFTER.md) · [원본/배포 SDK 대응](THIRD_PARTY_COPIES.json) · [수정 파일 manifest](CHANGE_MANIFEST.json)
'''
    architecture += f'\n생성 결과: {len(graph["files"])}개 소스, {graph["nodes"]:,} 노드, {graph["edges"]:,} 연결. AST/관계 자료 생성만 수행했으며 진단 검증기는 실행하지 않았다.\n'
    write(OUT/'ARCHITECTURE.md',architecture)
    commits=git('log','--reverse','--format=%h %s',BASE+'..HEAD').decode('utf-8')
    checks='''# 단계와 완료 상태

## 근거

- 소스 작성: framework 원본·Standard 배포 사본, 선택 debugger, 독립 native 프로젝트 반영.
- 학습 원문: 12 N제(순정6/Standard6), 통합6시나리오, 6학습 장, 14회 로드맵.
- 생성: 오프라인 HTML(전체 소스 포함), 문서 목차, AST 구조 그래프, 전체 before/after, 제출 ZIP/manifest/용량 보고.
- 미수행: 컴파일, 테스트, 검증기, GLFW/OpenGL/브라우저 실행, 화면 확인, 타 PC/폴더 재배치 동작. 사용자 수행 항목은 ../USER_CHECKS.md.

## 단계별 커밋

```
'''+commits+'''```

보고서 자체 커밋은 이 목록 생성 다음 단계이므로 git log에서 확인한다. 커밋은 소스/문서 반영 근거이며 빌드/실행 증거가 아니다.

## 보존과 임시 파일

사용자의 openGL_3dAssign_26/framework/detail/Renderer.cpp 변경, 첨부 PDF, 새 제안서 DOCX, 기존 docs.zip과 캐시를 정리하지 않았다. old_standard도 보존한다. SDK·런타임·설정·소스는 임시 파일이 아니다. dist ZIP과 manifest는 요청한 제출 산출물이므로 보존한다. 생성 HTML/그래프도 복습 근거로 보존한다. 삭제한 용량은 0바이트다.

PDF 열람 임시 이미지의 정확한 경로·현재 크기를 아래에 기록한다. 근거 재열람용으로 보존하며 자동 삭제하지 않는다.
'''
    candidates=[]
    for folder in (Path.home()/'AppData/Local/Temp/codex-test2-pdfs-i21cpa3b',Path.home()/'AppData/Local/Temp/codex-scale-exam-y7tpovqd'):
        if folder.exists():
            for p in sorted(folder.iterdir()):
                if p.is_file(): candidates.append(f'- `{p}` · {p.stat().st_size:,}바이트 · PDF 그림/키 입력 재열람 근거')
    write(OUT/'STATUS_AND_CHECKS.md',checks+'\n'+'\n'.join(candidates))
    write(OUT/'README.md','''# 코딩테스트 2 변경 근거

[실행/구조](ARCHITECTURE.md) · [전체 코드 수정 전후](ALL_CODE_BEFORE_AFTER.md) · [파일 manifest](CHANGE_MANIFEST.json) · [SDK 배포 대응](THIRD_PARTY_COPIES.json) · [단계/사용자 확인](STATUS_AND_CHECKS.md) · [제출 용량](SUBMISSION_SIZE.md)

문서/소스 구조/패키지 생성과 컴파일/실행 성공을 구분한다. 빌드·테스트·검증기·앱/브라우저 실행은 사용자가 수행한다.
''')
    print(f'Wrote complete before/after for {len(manifest)} own-code files, {len(provenance)} third-party provenance entries and architecture/status reports.')
if __name__=='__main__':main()