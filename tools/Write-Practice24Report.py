"""Write the complete source delta and static architecture report; never build or execute apps."""
from pathlib import Path
import difflib
import hashlib
import json
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'docs/practice-24-26-27'
BASE = '9d01320'


def git(*args):
    return subprocess.check_output(['git', '-c', 'core.quotepath=false', '-c', 'core.autocrlf=false', *args], cwd=ROOT).decode('utf-8')


def write(name, text):
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / name).write_text(text.rstrip() + '\n', encoding='utf-8-sig')


def main():
    names = set(git('diff', '--name-only', BASE, '--').splitlines())
    baseline_paths = set(git('ls-tree', '-r', '--name-only', BASE).splitlines())
    untracked = git('ls-files', '--others', '--exclude-standard').splitlines()
    allowed = ('framework/', 'standard/', 'openGL_3dAssign_24/', 'openGL_3dAssign_26/', 'openGL_3dAssign_27/', 'docs/practice-24-26-27/', 'graphify-out/practice-24-26-27/')
    names.update(name for name in untracked if name.startswith(allowed))
    names.update(name for name in untracked if name in ('AGENTS.md', 'tools/Write-Practice24Plus.py', 'tools/Write-Practice24Report.py'))
    report = '# 전체 코드 before / after\n\n기준 '+BASE+'는 태그한 튜토리얼 작업이 완료된 커밋이다. 이번 작업 시작 시점부터 최종 저장 소스까지 모든 변경 코드 블록을 기록한다. 신규 코드의 before는 없음이다. SDK 원본은 수정 없이 배포 사본으로 포함했으며 소스 본문 중복 대조에서 제외하고 manifest에 경로·크기·해시를 기록한다. 코드와 프로젝트 사본은 SDK 제외 모두 포함한다. 문서·그래프·바이너리는 manifest로 구분한다. 빌드·테스트·검증기·앱·브라우저 실행은 하지 않았다.\n\n'
    languages = {'.cpp':'cpp', '.h':'cpp', '.hpp':'cpp', '.glsl':'glsl', '.py':'python', '.ps1':'powershell', '.vcxproj':'xml', '.props':'xml', '.sln':'text'}
    manifest = []
    user_changes = {'openGL_3dAssign_26/framework/detail/Renderer.cpp'}
    for name in sorted(names):
        path = ROOT / name
        if '/__pycache__/' in name or '/.cache/' in name or name.startswith('graphify-out/practice-24-26-27/.'): continue
        if name.startswith('docs/practice-24-26-27/') and path.name in ('ALL_CODE_BEFORE_AFTER.md', 'CHANGE_MANIFEST.json', 'ARCHITECTURE.md', 'COMMITS.md'): continue
        data = path.read_bytes() if path.is_file() else b''
        manifest.append(dict(path=name, bytes=len(data), sha256=hashlib.sha256(data).hexdigest(), exists=path.is_file(), concurrent_user_edit=name in user_changes))
        if path.suffix not in languages or '/dependencies/' in name: continue
        prior = subprocess.run(['git', 'show', BASE+':'+name], cwd=ROOT, capture_output=True) if name in baseline_paths else None
        before = prior.stdout.decode('utf-8-sig').replace('\r\n','\n') if prior and prior.returncode == 0 else ''
        after = data.decode('utf-8-sig').replace('\r\n','\n') if data else ''
        left, right = before.splitlines(keepends=True), after.splitlines(keepends=True)
        groups = list(difflib.SequenceMatcher(None,left,right,autojunk=False).get_grouped_opcodes(0))
        if not groups: continue
        report += '## '+name+'\n\n'
        if name in user_changes:
            report += '이 사본에는 작성 이후 사용자의 추가 공백/빈 줄 변경이 있다. 현재 원문을 보존해 대조에 포함하지만 해당 미커밋 변경은 작업 커밋에서 제외한다. 에이전트가 작성한 변경으로 보고하지 않는다.\n\n'
        for i, group in enumerate(groups, 1):
            a,b,c,d = group[0][1],group[-1][2],group[0][3],group[-1][4]
            report += f'### 블록 {i}: before L{a+1}, after L{c+1}\n\n'
            if '/framework/' in name and not name.startswith('standard/framework/'):
                reason = '실습 폴더 하나로 복사·빌드·실행할 수 있도록 공통 원본의 독립 배포 사본을 포함한다.'
            elif name.startswith('openGL_3dAssign_24/'):
                reason = '물류 센터·트럭·적재·끝점 상태·상자별 최종 월드 변환의 카메라 매핑을 구현한다.'
            elif name.startswith(('openGL_3dAssign_26/','openGL_3dAssign_27/')):
                reason = '독립 실습의 형상·조명·회전·색·거리 조작을 구현한다.'
            else:
                reason = '공통 재질·조명·시점 계약, 범용 조합·확장, 소스에 맞춘 문서 생성에 필요한 변경이다. 요구사항 대응표와 확장 문서에서 동작을 설명한다.'
            report += '변경 이유: '+reason+'\n\n'
            for label, text in (('before',''.join(left[a:b])),('after',''.join(right[c:d]))):
                report += '**'+label+'**'+(' (없음)' if not text else '')+'\n\n'
                if text:
                    fence='`'*max(4,1+max((len(t) for t in re.findall(r'`+',text)),default=0))
                    report += fence+languages[path.suffix]+'\n'+text.rstrip()+'\n'+fence+'\n\n'
    write('ALL_CODE_BEFORE_AFTER.md', report)
    write('CHANGE_MANIFEST.json', json.dumps(dict(baseline=BASE, state='saved_source_not_built_or_run', files=manifest),ensure_ascii=False,indent=2))
    graphdir = ROOT / 'graphify-out/practice-24-26-27'
    source = json.loads((graphdir/'SOURCE_MANIFEST.json').read_text(encoding='utf-8-sig'))
    projects = json.loads((graphdir/'PROJECTS.json').read_text(encoding='utf-8-sig'))
    architecture = '# 최종 실행 구조\n\n'
    architecture += f'graphify 코드 AST: {source["nodes"]} nodes / {source["edges"]} edges. [HTML](../../graphify-out/practice-24-26-27/graph.html), [JSON](../../graphify-out/practice-24-26-27/graph.json), [소스 범위](../../graphify-out/practice-24-26-27/SOURCE_MANIFEST.json). SDK·아카이브·중복 프레임워크 배포본은 추출에서 제외한다. 새 프로젝트의 실제 컴파일 입력 목록은 PROJECTS.json에 각 폴더의 로컬 framework와 함께 기록한다.\n\n'
    architecture += '''## 공통 파이프라인

각 독립 main은 Application과 해당 폴더의 Console을 생성하고 초기화·입력·갱신 콜백을 등록한다. 콜백 등록은 실제 프레임 실행이 아니다. Application::run이 초기화 후 이벤트 루프에서 입력과 갱신을 호출한다. 사용자 update 다음 Scene::updateMotions를 진행하고 렌더한다. 종료에서는 컨텍스트가 살아 있는 동안 GPU 자원을 해제한다.

24: initScene → 기존 L자 벨트/상자 + addTruck → resetState. 숫자 입력 → loadParcel → 부모/적재 슬롯 변경. onUpdate → 트럭 상태·경계 처리 → 미적재 물체 경로 갱신 → 후보 카메라 5개 갱신 → 선택 카메라 적용. 상자 번호와 EntityId를 분리한다.

26: initScene → 육면체/사각뿔/광원 표시/단위 궤도. 입력은 형상·조명·자전/공전 방향·반지름 상태를 바꾼다. onUpdate → 각도 → apply → 두 형상 자세·가시성·광원 위치·궤도 크기·관찰 거리. 같은 radius가 광원과 궤도에 사용된다.

27: initScene → 구 3개와 광원 표시. 입력은 광원 색 인덱스와 공전 방향을 바꾼다. onUpdate → 각도 → apply → 점광원 위치·색 및 표시 객체 변환·색.

Standard: main → Console → defaultRecipe → Assembly::execute → resolveCommand/dispatch → applyPoses. 일반 객체·지역 레이어 뒤 사용자/물리 제어기를 갱신하고 syncSceneTools에서 광원·부착 카메라를 얻는다. material 명령은 기존 재질을 복사해 지정 필드만 바꾼다. 새로운 실습 모드를 컴파일하지 않는다. MovingBeacon.h는 등록 방법을 설명한 C++ 참조다.

Renderer는 Scene의 CPU 정점/색/법선/UV를 VAO/VBO/EBO로 올린다. 각 draw에서 faceWorldMatrix, MVP, 역전치 법선 행렬, 재질 계수·패스 시점·광원을 전송한다. 정점 셰이더는 월드 위치/법선을 전달하고 프래그먼트 셰이더가 Phong 조명을 계산한다. unlit 표면과 선·윤곽선은 조명 계산을 분리한다.

## 그래프의 해석 경계

AST calls/include 관계는 정적 소스 추출이다. std::function 콜백의 실제 실행 순서, 사용자 조건과 런타임 바인딩은 해당 소스와 대조해야 한다. 독립 프로젝트의 Console/main 이름이 같아도 서로 호출하는 것이 아니다. generator는 프로젝트를 넘나드는 calls를 AMBIGUOUS로 표시한다. 실행·화면·컴파일 증거가 아니다.

## 저장소의 독립 진입점 목록

각 행은 서로 다른 실행 프로젝트다. 부재 파일은 이전 sparse/historical 기록이며 이번 작업에서 삭제한 것으로 보지 않는다.

'''
    for project in projects:
        title=project.get('project',project.get('file','example'))
        architecture += '- `'+title+'`: '
        if project.get('available') is False:
            architecture += '현재 부재, 과거 정보.\n'
        else:
            entries=project.get('entrypoints',[])
            if project.get('standalone_example'):
                entries=[dict(file=project['file'],name=project.get('entrypoint','main'))]
            architecture += '; '.join('`'+e['file']+':'+str(e.get('line','?'))+'` '+e.get('name','main')+(' (조건부 분기 확인)' if e.get('conditional_compilation') else '') for e in entries)+'\n'
    write('ARCHITECTURE.md', architecture)
    (graphdir / 'GRAPH_REPORT.md').write_text('# 코드 구조 그래프\n\n'+f'{len(source["files"])}개 파일, {source["nodes"]} nodes, {source["edges"]} edges. graphify AST 기반이며 의미 분석 API·빌드·테스트·실행은 사용하지 않았다.\n\n[전체 실행 구조와 진입점](../../docs/practice-24-26-27/ARCHITECTURE.md) · [대화형 그래프](graph.html) · [소스 범위](SOURCE_MANIFEST.json)\n\n독립 프로젝트의 같은 클래스 이름을 하나의 실행 흐름으로 해석하지 않는다. 콜백·동적 분기는 소스 검토가 필요하다. 중복 배포 프레임워크와 SDK는 그래프 노드 추출에서 제외하고 각 프로젝트의 실제 컴파일 파일은 PROJECTS.json에 기록했다.\n',encoding='utf-8-sig')
    write('COMMITS.md', '# 이번 작업의 커밋\n\n문서 보고서 생성 시점까지의 단계 및 교정 커밋이다. 최종 문서 커밋 해시는 최종 응답에서 제공한다.\n\n```text\n'+git('log','--reverse','--format=%h %s',BASE+'..HEAD')+'```\n\n공유 스킬/사건 로그 기준 저장소: 5597a28, origin/main 일반 push 완료. 과제 저장소는 별도 push 요청 없이 로컬 커밋으로 남긴다.\n')
    print('Wrote full before/after, manifest, architecture and commit history. No builds/tests/apps run.')


if __name__ == '__main__':
    main()
