"""Generate complete source before/after and architecture reports, without builds or tests."""
from pathlib import Path
import collections
import difflib
import hashlib
import json
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BASE = '01db4ff48ade5f8d8941110408852762cb0f3f64'
OUT = ROOT / 'docs/source-debugger'
GRAPH = ROOT / 'graphify-out/source-debugger'
OWNED = ('framework/', 'standard/', 'old_standard/', 'debugger/', 'docs/source-debugger/',
         'graphify-out/source-debugger/', 'tools/templates/standard-viewer/')
TOOLS = {'tools/Write-SourceDebuggerDocs.py', 'tools/Write-SourceDebuggerReport.py',
         'tools/Update-StandardDocs.py', 'tools/Update-StandardModesGraph.py', 'OpenGL-26-2.sln'}


def git(*args):
    return subprocess.check_output(['git', '-c', 'core.quotePath=false', *args], cwd=ROOT)


def write(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text.rstrip() + '\n', encoding='utf-8-sig')


def code(text, language):
    fence = '`' * max(4, max((len(x) for x in re.findall(r'`+', text)), default=0) + 1)
    return fence + language + '\n' + text.rstrip('\n') + '\n' + fence + '\n'


def before(name):
    name = 'standard/' + name[len('old_standard/'):] if name.startswith('old_standard/') else name
    p = subprocess.run(['git', 'show', BASE + ':' + name], cwd=ROOT, capture_output=True)
    return p.stdout.decode('utf-8-sig').replace('\r\n', '\n') if p.returncode == 0 else ''


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    names = set(git('diff', '--no-renames', '--name-only', BASE, '--').decode('utf-8').splitlines())
    names.update(git('ls-files', '--others', '--exclude-standard').decode('utf-8').splitlines())
    names = sorted(n for n in names if n in TOOLS or n.startswith(OWNED))
    manifest = []
    report = ['# 전체 코드 변경 전후', '', '기준 커밋 `' + BASE + '`에서 이번 저장된 소스까지의 모든 변경 블록이다.', '',
              '이전 프로젝트의 이동은 old_standard의 파일을 원래 standard 경로와 비교한다. 내용이 같은 이동은 코드 변경으로 중복 표기하지 않는다. '
              '자동 생성 문서·그래프·기존 SDK 복사본은 manifest에 기록하고 코드 블록 대조에서는 제외한다. 빌드·테스트·검증기·앱은 실행하지 않았다.', '']
    kinds = {'.cpp': 'cpp', '.h': 'cpp', '.hpp': 'cpp', '.glsl': 'glsl', '.py': 'python',
             '.props': 'xml', '.vcxproj': 'xml', '.sln': 'text'}
    blocks = 0
    for name in names:
        path = ROOT / name
        if path == OUT / 'ALL_CODE_BEFORE_AFTER.md' or path == OUT / 'CHANGE_MANIFEST.json': continue
        data = path.read_bytes() if path.is_file() else b''
        moved = name.startswith('old_standard/')
        if not path.exists() and name.startswith('standard/'):
            moved = (ROOT / 'old_standard' / name[len('standard/'):]).is_file()
        manifest.append({'path': name, 'exists': path.is_file(), 'bytes': len(data),
                         'sha256': hashlib.sha256(data).hexdigest() if path.is_file() else None,
                         'preserved_project_relocation': moved})
        language = kinds.get(path.suffix)
        if name.startswith('tools/templates/') and path.suffix in {'.html', '.js', '.css'}: language = path.suffix[1:]
        if not language or '/dependencies/' in name or '/vendor/' in name: continue
        if not path.is_file() and moved: continue
        left = before(name).splitlines(keepends=True)
        right = data.decode('utf-8-sig').replace('\r\n', '\n').splitlines(keepends=True) if data else []
        groups = list(difflib.SequenceMatcher(None, left, right, autojunk=False).get_grouped_opcodes(0))
        if not groups: continue
        report += ['## ' + name, '']
        for n, group in enumerate(groups, 1):
            first, last = group[0], group[-1]
            i, j, a, b = first[1], last[2], first[3], last[4]
            old, new = ''.join(left[i:j]), ''.join(right[a:b])
            blocks += 1
            report += [f'### 블록 {n} · before L{i+1} / after L{a+1}', '',
                       '변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. '
                       '함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.', '',
                       '**before**' if old else '**before: 없음**', '', code(old, language) if old else '',
                       '**after**' if new else '**after: 없음**', '', code(new, language) if new else '', '']
    write(OUT / 'ALL_CODE_BEFORE_AFTER.md', '\n'.join(report))
    write(OUT / 'CHANGE_MANIFEST.json', json.dumps({'baseline': BASE, 'changed_code_blocks': blocks,
        'source_state': 'Saved source during final documentation stage; hashes include current generated artifacts.',
        'excluded_preexisting_user_items': ['openGL_3dAssign_26/framework/detail/Renderer.cpp', 'docs.zip',
            'lecture_pdf/homework/*', 'tools/__pycache__/*'], 'files': manifest}, ensure_ascii=False, indent=2))

    projects = json.loads((GRAPH / 'PROJECTS.json').read_text(encoding='utf-8-sig'))
    graph = json.loads((GRAPH / 'graph.json').read_text(encoding='utf-8-sig'))
    source = json.loads((GRAPH / 'SOURCE_MANIFEST.json').read_text(encoding='utf-8-sig'))
    nodes = {n['id']: n for n in graph['nodes']}
    links = graph.get('links', graph.get('edges', []))
    degree = collections.Counter()
    for link in links:
        degree[link['source']] += 1; degree[link['target']] += 1
    lines = ['# 소스 구조와 실행 프로젝트', '',
        f"graphify AST: {source['nodes']} nodes / {source['edges']} edges / {len(source['files'])} source files.", '',
        '코드 전용 AST 추출이며 의미 분석 API 토큰 사용은 0이다. 코드 관계를 소스와 함께 읽는 자료다. '
        '빌드·테스트·그래프 진단기·인코딩 검증기·앱·브라우저를 실행하지 않았다. 동적 콜백의 실제 실행이나 화면을 입증하지 않는다.', '',
        '## 현재 Standard', '',
        '1. main이 Console과 Application을 만들고 초기화·키·마우스·업데이트·렌더 콜백을 등록한다. 등록은 실행이 아니다.',
        '2. Application::run 초기화 후 Console::initScene → SceneAuthoring::clear → configureScene → addObject/addLayer/addLight/setCamera → update(0).',
        '3. 입력은 Application의 이벤트 전달 뒤 Console::onKey/onMouse로 들어온다. 클릭은 pickAt의 결과 EntityId를 이름으로 바꾸어 출력한다. 명령 파서와 assembly 런타임은 없다.',
        '4. Console::onUpdate가 절대 시간을 계산해 SceneAuthoring::update를 호출한다. 지역 레이어를 평가한 뒤 Scene::worldMatrix로 부착 광원·카메라를 평가한다.',
        '5. Application의 기존 Scene 모션 갱신 후 Console::onRender → Application::renderScene → Renderer의 CPU 정점/인덱스 업로드와 VAO/VBO/EBO·uniform·draw 호출로 이어진다.',
        '6. Shaders.h 또는 외부 VertexShader/FragmentShader가 모델/정규행렬·조명·재질·텍스처를 처리한다. 프레임 종료 뒤 run이 GPU·창·GLFW를 정리한다.', '',
        '## 별도 debugger', '',
        '1. wmain은 UTF-8 콘솔을 설정하고 인수 또는 project 명령으로 대상 폴더를 고른다. 독립 실행 프로그램이며 Standard의 메모리/렌더러를 제어하지 않는다.',
        '2. command는 문자열을 토큰화하고 SourceIndex::refresh로 실제 디스크 소스를 다시 읽는다. 시작/끝 구획과 현행 행 번호를 만든다.',
        '3. set/parent는 유일한 현재 대입/등록 줄을 찾고, add는 실제 @debug insert 앞의 전체 C++ 삽입안을 만든다. find/code는 일반 소스 위치를 제공한다.',
        '4. proposal은 실제 before와 생성한 after, 경로·행·주변 코드를 stdout에 출력한다. 대상 파일 저장·프로세스 실행·컴파일 호출은 없다.',
        '5. EOF 또는 quit로 끝난다. Windows 파일 경로/콘솔 API와 C++ 표준 기능만 사용하며 OpenGL SDK 링크는 없다.', '',
        '## 전체 독립 프로젝트와 진입점', '',
        '각 행은 별도 실행 대상이다. 같은 이름의 Console/main을 하나의 호출 흐름으로 합치지 않는다. '
        'old_standard는 보존한 과거 프로젝트로 등록만 기록하고 현재 AST 추출에서는 제외한다. 프레임워크 예제의 main도 동시에 컴파일하지 않는다.', '',
        '| 프로젝트/독립 예제 | 확인한 진입점 |', '|---|---|']
    for project in projects:
        name = project.get('project', project.get('file', 'example'))
        if project.get('available') is False:
            entries = '작업 전 부재, 이전 기록만 존재'
        else:
            entries = '; '.join(f"{e['file']}:{e.get('line', '?')} {e.get('name', 'main')}" for e in project.get('entrypoints', []))
            if not entries and project.get('standalone_example'): entries = project['file'] + ' main'
            if not entries: entries = '프로젝트 파일/소스의 조건부 진입점을 별도 확인'
        lines.append('| ' + name + ' | ' + entries + ' |')
    lines += ['', '## God Nodes', '', 'AST 연결 차수다. 이름 중복과 동적 호출을 고려해 실행 순서로 해석하지 않는다.', '',
              '| 노드 | 연결 차수 | 소스 |', '|---|---:|---|']
    for node, count in degree.most_common(8):
        item = nodes.get(node, {})
        lines.append('| ' + str(item.get('label', node)).replace('|', '/') + ' | ' + str(count) + ' | ' + str(item.get('source_file', '')) + ' |')
    lines += ['', '## Surprising Connections', '',
        '- 렌더와 피킹은 Scene의 월드/면 행렬을 공유한다. 새 작성 API는 그 앞의 attachment와 지역 레이어만 구성한다. 별도 피킹 좌표 계산기를 추가하지 않았다.',
        '- 광원·카메라 부착은 기존 Scene 월드 행렬과 CameraSystem::attached를 재사용한다. C++ 구획 주석은 GPU나 실행 상태에 참여하지 않는다.',
        '- debugger는 프레임워크를 링크하지 않지만 생성 코드가 실제 공개 API를 호출한다. SourceIndex는 컴파일러의 활성 분기나 AST 의미를 판정하지 않는다.', '',
        '## Suggested Questions', '',
        '- 객체의 월드 배치를 지정한 뒤 부모의 지역 레이어가 회전하면 자식과 광원은 어떤 행렬을 상속하는가?',
        '- source 파일에 빈 줄을 넣었을 때 debugger의 before와 행 번호는 어디서 다시 계산되는가?', '',
        '## 산출물과 미확인 범위', '',
        'graph.json/graph.html/EXTRACTION.json/PROJECTS.json/SOURCE_MANIFEST.json을 함께 보존한다. '
        '빌드·런타임·화면·폴더 분리 실행 확인은 standard/USER_CHECKS.md에 따라 사용자가 수행한다.']
    write(OUT / 'ARCHITECTURE.md', '\n'.join(lines))
    write(GRAPH / 'GRAPH_REPORT.md', '\n'.join(lines))
    write(OUT / 'PLAN.md', '''# 직접 C++ 작성과 source debugger 개편

- 날짜: 2026-11-10
- 승인 범위: 프레임워크 원본과 Standard 직접 작성 전환, 별도 debugger, 실제 소스의 경로·행·before/after 안내, 조명·재질·카메라와 문서·튜토리얼·예제 개편.
- 추가 승인: 기존 Standard를 old_standard로 이동하고 그 안의 x64/.vs만 삭제. 두 폴더가 없어 실제 삭제·회수 용량은 0바이트.
- 기존 사용자 Renderer.cpp 변경, docs.zip, PDF, 기존 pycache는 수정·삭제·커밋하지 않음.

| 단계 | 커밋 | 내용 |
|---|---|---|
| 1 | b6daa40 | SceneAuthoring 원본/배포 사본, 직접 C++ 작성 API |
| 2 | 14fefc4 | old_standard 보존, 새 Standard의 SceneSetup·콜백·컴파일 목록 |
| 3 | d0ac31a | debugger, 실제 소스 색인·수정안·합본/단독 솔루션 |
| 4 | 50d73c5 | 튜토리얼·예제·조명 문서·목차·생성기, 초기 부모 레이어 적용 |
| 5 | 최종 구조/보고 커밋 | graphify AST, 프로젝트 진입점, 전체 before/after·manifest |

문서 생성: python -B tools/Update-StandardDocs.py. 코드 작성·문서 생성·구조 자료 생성은 수행했고, 컴파일·테스트·검증기·앱 실행·화면 확인은 수행하지 않았다.

새 debugger는 지원 속성과 구획 표시를 사용한다. 일반 C++/GLSL은 find/code/replace/insert로 실제 행의 수정안을 안내한다. 임의 자연어·C++ 의미 분석·소스 자동 저장·자동 컴파일은 구현하지 않았다. 추가 개념은 지정된 Jun1188/ComputerGraphics의 해당 실제 소스를 참고한다.

별도 삭제할 임시파일을 만들지 않았다. graphify-out/source-debugger의 추출/캐시는 구조 자료와 재생성 근거로 보존한다. 삭제 후보로 사용자 자료를 임의 분류하지 않는다.
''')
    print(f'Generated complete before/after ({blocks} blocks), source manifest and architecture reports. No builds/tests run.')


if __name__ == '__main__':
    main()
