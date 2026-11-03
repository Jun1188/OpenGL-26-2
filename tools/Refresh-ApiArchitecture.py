"""Source-backed API architecture diagrams and a directed graphify code graph."""
from pathlib import Path
import argparse
import collections
import importlib.util
import json
import os
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'framework-analysis/api-reference'
DIRECTORIES = ('framework/docs', 'standard/docs', 'standard/framework/docs')


def load(name, path):
    spec = importlib.util.spec_from_file_location(name, ROOT / path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


guide = load('api_guide', 'tools/author_framework_guides.py')
esc = guide.esc


def read(path):
    return (ROOT / path).read_text(encoding='utf-8-sig')


def write(path, text):
    path = ROOT / path
    path.parent.mkdir(parents=True, exist_ok=True)
    content = text.replace('\r\n', '\n').encode('utf-8-sig')
    if path.exists() and path.read_bytes() == content:
        return
    # Match the existing API catalog writer: finish a sibling file before replacement.
    temporary = path.with_name(path.name + '.api.tmp')
    temporary.write_bytes(content)
    os.replace(temporary, path)


def dump(path, value):
    write(path, json.dumps(value, ensure_ascii=False, indent=2) + '\n')


def row(x, y, width, *lines, decision=False):
    return (x, y, width, 78 if not decision else 100, decision, list(lines))


def diagrams():
    result = {}
    def add(key, title, nodes, edges, height, evidence, note):
        result[key] = dict(title=title, html=guide.graph_svg('api-' + key, title, nodes, edges, 1200, height),
                           evidence=evidence, note=note)
    add('ownership', '01 소유·참조·상속 관계', [
        row(400,20,400,'main 스코프','Application app; Console sample(app)'),
        row(40,160,360,'Application','값 멤버와 창·GLFW 수명 소유'),
        row(800,160,360,'Console','app_은 Application& 비소유 참조'),
        row(10,330,275,'Scene','CPU 컴포넌트·MotionSystem 소유'),
        row(310,330,275,'Camera / RenderOptions','Application의 값 멤버'),
        row(610,330,275,'Renderer','GL 핸들·업로드 캐시 소유'),
        row(910,330,275,'optional<AssetLoader>','Application 안에 emplace / reset'),
        row(10,500,540,'Component_Poly → Polyhedron → Mesh','vector 값 소유; EntityId는 조회 번호'),
        row(610,500,575,'program / VAO / VBO / EBO / texture','Renderer가 생성·삭제; GL 컨텍스트 필요'),
        row(100,665,1000,'Material.texture: shared_ptr<const TextureImage>로 CPU 이미지 공유',
            '이 핵심 타입 간 C++ 상속 선언은 없음; setParent는 장면 변환 관계'),
    ], [('M600 98 V125 H220 V160','값으로 생성',240,117),
        ('M600 125 H980 V160','값으로 생성',800,117),
        ('M800 199 H400','비소유 참조',520,185),
        ('M220 238 V290 H147 V330','값 소유',35,286),
        ('M220 290 H447 V330','값 소유',340,280),
        ('M447 290 H747 V330','값 소유',640,280),
        ('M747 290 H1047 V330','실행 자산',930,280),
        ('M147 408 V455 H280 V500','컨테이너 소유',165,453),
        ('M747 408 V455 H897 V500','핸들 관리',777,453)], 790,
        ['standard/main.cpp','standard/Console.h','standard/framework/include/Application.h',
         'standard/framework/include/Graphics.h','standard/framework/include/Model.h'],
        '화살표의 소유·참조 표시를 구분한다. ID·parent·LayerId 연결은 메모리 소유권 이전이 아니다.')
    add('calls', '02 호출함·호출됨과 실제 프레임 루프', [
        row(390,15,420,'main: set…Handler → run(config)','람다 등록은 실행 예약'),
        row(390,135,420,'initialize → Renderer::initialize','initHandler → Console::initScene (1회)'),
        row(390,260,420,'계속 실행?', '닫기·실패면 정리',decision=True),
        row(870,270,310,'shutdown → run 반환','Renderer 정리 → 창·GLFW 종료'),
        row(390,410,420,'PollEvents → dispatch','키·마우스 콜백 → Console 입력 함수'),
        row(390,540,420,'framebuffer 양수?', '닫기·콜백 오류도 확인',decision=True),
        row(15,550,300,'WaitEvents','시간 기준 재설정 → 다음 반복'),
        row(390,690,420,'dt clamp → updateHandler','Console::onUpdate → Scene::updateMotions'),
        row(390,820,420,'ensureContext → renderHandler?', '등록 여부로 분기',decision=True),
        row(15,830,300,'등록됨: Console::onRender','renderScene / threeViews → renderViews'),
        row(870,830,310,'미등록: renderScene','기본 카메라로 단일 패스'),
        row(390,980,420,'Renderer::render / renderViews','renderPass → upload(필요 시) → draw'),
        row(390,1100,420,'detail::gl() → OpenGL 명령','DrawElements → CPU 호출 복귀'),
        row(390,1220,420,'렌더 성공 → SwapBuffers','다음 프레임; 실패·예외는 shutdown'),
    ], [('M600 93 V135','',0,0),('M600 213 V260','',0,0),
        ('M810 310 H870','아니오',815,298),('M600 360 V410','예',612,389),
        ('M600 488 V540','',0,0),('M390 590 H315','아니오',321,578),
        ('M15 589 H5 V310 H390','다음 반복',18,387),('M600 640 V690','예',612,669),
        ('M600 768 V820','',0,0),('M390 870 H315','예',335,856),
        ('M810 870 H870','아니오',814,856),('M165 908 V950 H600 V980','',0,0),
        ('M1025 908 V950 H600','',0,0),('M600 1058 V1100','',0,0),
        ('M600 1178 V1220','',0,0),('M600 1298 V1335 H345 V245 H600 V260','다음 반복',150,1320)], 1370,
        ['standard/main.cpp','standard/Console.cpp','standard/framework/detail/Application.cpp',
         'standard/framework/detail/Renderer.cpp','standard/framework/detail/Native.h'],
        'GPU 처리는 비동기다. GL 함수의 CPU 복귀는 GPU 완료를 뜻하지 않는다. 이벤트가 없으면 입력 콜백도 실행되지 않는다.')
    add('locals', '03 함수 지역 인스턴싱·반환·수명', [
        row(10,25,330,'함수 / 생성 위치','호출 시 지역 객체 구성'),
        row(420,25,350,'지역 객체','일시적 값과 빌린 참조 구분'),
        row(850,25,340,'반환·이전 대상','지역 스코프 종료 이후'),
        row(10,160,330,'main','run이 끝날 때까지 같은 스코프'),
        row(420,160,350,'Application app / Console sample','ApplicationConfig config'),
        row(850,160,340,'run → 정리 → main 반환','sample → app 역순 소멸'),
        row(10,300,330,'Renderer::render','단일 패스용 지역 값'),
        row(420,300,350,'RenderPass pass','camera/options/viewport를 값 복사'),
        row(850,300,340,'renderPass(scene, pass, …)','호출 중 const 참조로 사용'),
        row(10,440,330,'PatternSystem::threeViews','3개 패스 구성'),
        row(420,440,350,'RenderPass main / top / front','카메라·옵션·픽셀 영역 값'),
        row(850,440,340,'vector<RenderPass> 값 반환','renderViews 동안 임시 값 수명 유지'),
        row(10,580,330,'Renderer::upload','형상 revision 변경 시'),
        row(420,580,350,'vertices / indices / packed','ranges / stagedTextures'),
        row(850,580,340,'BufferData로 GPU 저장소에 복사','성공 시 ranges/texture 캐시 확정'),
        row(10,720,330,'addSwingWithRider','실패 시 원 Scene 유지'),
        row(420,720,350,'Scene staged = scene; SwingRig rig','Motion / 변환 / 관절 임시 값'),
        row(850,720,340,'성공: scene = move(staged)','rig 반환; ID가 객체 소유권은 아님'),
    ], [('M340 199 H420','생성',355,187),('M770 199 H850','수명',790,187),
        ('M340 339 H420','생성',355,327),('M770 339 H850','전달',790,327),
        ('M340 479 H420','생성',355,467),('M770 479 H850','반환',790,467),
        ('M340 619 H420','생성',355,607),('M770 619 H850','복사/이전',780,607),
        ('M340 759 H420','생성',355,747),('M770 759 H850','성공 시',785,747)], 840,
        ['standard/main.cpp','standard/framework/detail/Renderer.cpp','standard/framework/detail/Patterns.cpp',
         'standard/framework/detail/Animation.cpp'],
        '지역 CPU 컨테이너가 사라져도 업로드된 GPU 버퍼는 Renderer::destroy까지 유지된다. 참조·ID 반환은 객체 생성과 다르다.')
    add('layers', '04 엔티티 계층·모션 연결·변환·GPU 바인딩', [
        row(15,20,520,'Scene::setParent → Component_Poly.parent','루트 → 조상 → 대상; C++ 상속과 별개'),
        row(665,20,520,'MotionSystem: LayerId 부모 체인','bindMotion: 레이어 ↔ MotionId'),
        row(15,170,520,'Scene::bindEntity → entityLayers_','가장 가까운 조상 연결 지점(anchor) 선택'),
        row(665,170,520,'layerMatrices(leaf)','TransformSpace에 따라 parent / local 합성'),
        row(100,330,1000,'worldMatrix = motion.parent × prefix × motion.local × suffix',
            'prefix: 루트~anchor; suffix: anchor 다음~대상; L = T(basis)·R·S·T(-pivot)'),
        row(100,480,1000,'faceWorldMatrix = world × transformFrame × faceLocalMatrix',
            'Hierarchy면 worldMatrix; Independent면 I / 면 애니메이션은 Local·Model 순서 구분'),
        row(100,630,1000,'Renderer::renderPass → draw: model=M, view=V, projection=P',
            'GPU 바인딩: UseProgram + BindVertexArray + texture + Uniform(MVP=P·V·M)'),
        row(100,780,1000,'DrawElements: EBO 인덱스 → 정점 셰이더 gl_Position',
            '클리핑 → 원근 나눗셈 → viewport → 래스터화 → 프래그먼트 → 깊이/프레임버퍼'),
    ], [('M275 98 V170','부모 변환',290,143),('M925 98 V170','레이어 합성',940,143),
        ('M665 209 H535','ID 연결을 읽음',545,197),('M275 248 V285 H600 V330','',0,0),
        ('M925 248 V285 H600','',0,0),('M600 408 V480','면별 정책',615,453),
        ('M600 558 V630','최종 M',615,601),('M600 708 V780','명령 제출',615,751)], 920,
        ['standard/framework/detail/Scene.cpp','standard/framework/detail/Motion.cpp',
         'standard/framework/include/Model.h','standard/framework/detail/Renderer.cpp','standard/framework/detail/Shaders.h'],
        '엔티티 부모, 모션 레이어, GL 바인딩은 독립된 관계다. shader 이후 고정 단계는 OpenGL 파이프라인 설명이며 별도 C++ 함수 호출이 아니다.')
    add('summary', '종합: main에서 GPU까지, 다음 프레임과 종료까지', [
        row(350,20,500,'main: app·sample 지역 생성 → 콜백 등록','app이 Scene·Camera·Renderer를 소유'),
        row(350,160,500,'run 초기화 → initScene','CPU 형상·계층·모션·ID 구성'),
        row(350,300,500,'프레임: PollEvents → 입력 → dt 갱신','Console::onUpdate → Scene::updateMotions'),
        row(350,440,500,'onRender → renderScene / renderViews','지역 RenderPass → Renderer::renderPass'),
        row(350,580,500,'CPU: 필요 시 upload → draw','엔티티 부모 + 모션 레이어 + 면 변환 → M'),
        row(350,720,500,'바인딩: program·VAO·texture·uniform','MVP=P·V·M → DrawElements'),
        row(350,860,500,'GPU: vertex → fragment → framebuffer','CPU GL 호출 복귀와 GPU 완료는 별개'),
        row(350,1000,500,'렌더 성공: SwapBuffers → 다음 프레임','종료·실패: Renderer 정리 → 창·GLFW 해제'),
        row(890,1000,300,'run 반환 → main 반환','sample → app 소멸'),
    ], [('M600 98 V160','1 소유',615,135),('M600 238 V300','2 호출',615,275),
        ('M600 378 V440','3 지역 값',615,415),('M600 518 V580','',0,0),
        ('M600 658 V720','4 변환·연결',615,695),('M600 798 V860','GPU 제출',615,835),
        ('M600 938 V1000','표시',615,975),('M350 1039 H220 V339 H350','반복',230,696),
        ('M850 1039 H890','종료',851,1025)], 1130,
        ['standard/main.cpp','standard/Console.cpp','standard/framework/detail/Application.cpp',
         'standard/framework/detail/Renderer.cpp'],
        '한 실행 프로젝트 standard의 흐름이다. framework/examples의 독립 main을 이 실행 경로에 합치지 않는다.')
    return result


def source_link(directory, path):
    relative = os.path.relpath(ROOT / path, ROOT / directory).replace('\\', '/')
    return '<a href="' + esc(relative) + '">' + esc(path) + '</a>'


def refresh_diagrams():
    charts = diagrams()
    health_path = OUT / 'graphify-out/GRAPH_HEALTH.md'
    health = health_path.read_text(encoding='utf-8-sig') if health_path.exists() else ''
    missing = re.search(r'dangling_endpoint_edges: (\d+)', health)
    collapsed = re.search(r'directed_same_endpoint_collapsed_edges: (\d+)', health)
    for directory in DIRECTORIES:
        block = '<!-- API-ARCHITECTURE:BEGIN --><section id="api-architecture"><h2>API 코드베이스: 네 관계와 종합 흐름</h2>'
        block += '<p>정적 소스와 graphify AST 분석에 근거한다. 화살표의 소유·참조·호출·생성·ID 연결을 구분한다. 각 설명의 소스 링크에서 실제 구현을 읽는다.</p>'
        if missing and collapsed:
            block += '<p class="badge">graphify 추출 한계: 연결 대상 누락 ' + missing.group(1) + '건, 같은 노드 쌍의 관계 병합 ' + collapsed.group(1) + '건. 원본 간선은 EXTRACTION.json에 보존했다. 아래 순서도는 그래프만으로 확정하지 않고 실제 소스를 대조했다. 빌드·실행·브라우저 확인은 사용자에게 맡긴다.</p>'
        block += '<nav>'
        block += ''.join('<a href="#arch-' + key + '">' + esc(c['title']) + '</a>' for key, c in charts.items()) + '</nav>'
        for key, chart in charts.items():
            block += '<section id="arch-' + key + '"><h3>' + esc(chart['title']) + '</h3>' + chart['html']
            block += '<p>' + esc(chart['note']) + '</p><p class="badge">소스: '
            block += ' · '.join(source_link(directory, p) for p in chart['evidence']) + '</p></section>'
            if key == 'layers':
                block += '<p>코드 경계: Framework.h는 공개 헤더를 모으는 include 진입점이다. include/*.h의 선언 → detail/*.cpp의 구현 → detail/Native.h의 gl()/glfw() 함수 테이블 → GLFW/OpenGL 네이티브 함수로 내려간다. 헤더 include는 컴파일 의존성이며 실행 중 함수 호출이 아니다. 셰이더와 GPU 고정 단계는 그 이후의 처리다.</p>'
            svg = re.search(r'<svg[\s\S]*?</svg>', chart['html']).group()
            write(directory + '/flowcharts/api-' + key + '.svg', svg + '\n')
        block += '</section><!-- API-ARCHITECTURE:END -->'
        path = directory + '/API_REFERENCE.html'
        current = read(path)
        current = re.sub(r'<!-- API-ARCHITECTURE:BEGIN -->[\s\S]*?<!-- API-ARCHITECTURE:END -->', '', current)
        current = current.replace('</nav>', '</nav>' + block, 1)
        write(path, current)
    write(OUT / 'ARCHITECTURE.md', '# API 관계 분석\n\n' + '\n\n'.join(
        '## ' + c['title'] + '\n\n' + c['note'] + '\n\n' + '\n'.join('- `' + p + '`' for p in c['evidence'])
        for c in charts.values()) + '\n\n빌드·테스트·브라우저 확인은 사용자에게 맡긴다.\n')
    print('Wrote 4 relationship diagrams + 1 synthesis in 3 API documents.')


def build_graph():
    from graphify.detect import detect, save_manifest
    from graphify.extract import extract
    from graphify.build import build_from_json
    from graphify.cluster import cluster, score_all
    from graphify.analyze import god_nodes, surprising_connections, suggest_questions
    from graphify.export import to_json, to_html
    from graphify.report import generate
    from graphify.diagnostics import diagnose_extraction, format_diagnostic_report
    output = OUT / 'graphify-out'
    output.mkdir(parents=True, exist_ok=True)
    excluded = [p.name + '/**' for p in ROOT.iterdir() if p.is_dir() and p.name not in ('framework', 'standard')]
    excluded += ['**/dependencies/**','**/docs/**','**/x64/**','**/.vs/**','**/*.md','**/*.json','**/*.html','**/*.svg']
    detection = detect(ROOT, extra_excludes=excluded, cache_root=output / 'cache')
    files = sorted(p for base in ('framework', 'standard') for p in (ROOT / base).rglob('*')
                   if p.is_file() and p.suffix in ('.h', '.cpp', '.inl')
                   and not {'dependencies', 'x64', '.vs', 'docs'}.intersection(p.relative_to(ROOT).parts))
    detection.update(files={'code': [str(p) for p in files], 'document': [], 'paper': [], 'image': [], 'video': []},
                     total_files=len(files), total_words=sum(len(p.read_text(encoding='utf-8-sig').split()) for p in files))
    print(f'Corpus: {len(files)} code files, ~{detection["total_words"]} words; SDKs and unrelated projects excluded.', flush=True)
    if len(files) > 500 or detection['total_words'] > 2_000_000:
        raise RuntimeError('Corpus exceeds agreed scope; preserve previous graph.')
    if detection.get('skipped_sensitive'):
        print('Sensitive-name exclusions:', detection['skipped_sensitive'], flush=True)
    extraction = extract(files, root=ROOT, cache_root=output / 'cache')
    updater = load('api_graph_anchors', 'framework-analysis/update_graph.py')
    updater.add_source_anchors(extraction, files, ROOT)
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    if not graph.number_of_nodes():
        raise RuntimeError('Empty graph; preserve previous graph.')
    communities = cluster(graph)
    labels = {}
    for cid, members in communities.items():
        names = collections.Counter(Path(graph.nodes[n].get('source_file', '')).stem for n in members)
        labels[cid] = (names.most_common(1)[0][0] if names else '공통') + ' 관계'
    gods, surprises = god_nodes(graph), surprising_connections(graph, communities)
    questions = suggest_questions(graph, communities, labels)
    commit = subprocess.check_output(['git','rev-parse','HEAD'], cwd=ROOT, text=True).strip()
    to_json(graph, communities, str(output / 'graph.json'), force=True, community_labels=labels, built_at_commit=commit)
    to_html(graph, communities, str(output / 'graph.html'), community_labels=labels)
    report = generate(graph, communities, score_all(graph, communities), labels, gods, surprises, detection,
                      {'input': 0, 'output': 0}, str(ROOT), suggested_questions=questions, built_at_commit=commit)
    report += '\n## 범위와 한계\n\nframework와 standard/framework를 별도 경로로 추출했다. standard 실행은 standard/framework를 사용한다. 같은 이름의 C++ 메서드 간 AST 연결은 정확한 오버로드 해석이 아니다. 문서 순서도는 실제 소스를 추가로 읽어 작성했다. GPU 고정 단계는 C++ 호출 간선이 아니다. 빌드·테스트·브라우저 확인은 사용자에게 맡겼다.\n'
    write(output / 'GRAPH_REPORT.md', report)
    health = diagnose_extraction(extraction, directed=True, root=str(ROOT))
    write(output / 'GRAPH_HEALTH.md', format_diagnostic_report(health))
    dump(output / 'EXTRACTION.json', extraction)
    dump(output / 'DETECTION.json', detection)
    dump(output / 'ANALYSIS.json', {'gods': gods, 'surprises': surprises, 'questions': questions})
    dump(output / 'SOURCE_MANIFEST.json', {'scope': ['framework', 'standard'], 'source_commit': commit,
         'files': [p.relative_to(ROOT).as_posix() for p in files]})
    dump(output / 'cost.json', {'input_tokens': 0, 'output_tokens': 0, 'external_api_calls': 0, 'mode': 'code AST'})
    save_manifest({'code': [str(p) for p in files]}, str(output / 'manifest.json'), kind='ast', root=ROOT)
    for path in output.iterdir():
        if path.is_file() and path.suffix in ('.html','.md','.json'):
            value = path.read_text(encoding='utf-8-sig')
            if '\ufffd' in value:
                raise RuntimeError('Unicode output failure: ' + str(path))
            write(path, value)
    print(f'Graph: {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges. External token cost: 0.', flush=True)
    print(format_diagnostic_report(health), flush=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--graph', action='store_true')
    args = parser.parse_args()
    if args.graph:
        build_graph()
    refresh_diagrams()
