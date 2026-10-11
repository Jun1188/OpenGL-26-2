"""Rebuild the code-only graph. Run with graphify's Python from repository root."""
import collections
import datetime
import hashlib
import json
import re
import subprocess
import xml.etree.ElementTree as ET
from pathlib import Path

from graphify.analyze import god_nodes, surprising_connections, suggest_questions
from graphify.build import build_from_json
from graphify.cluster import cluster, score_all
from graphify.detect import detect, save_manifest
from graphify.diagnostics import diagnose_extraction, format_diagnostic_report
from graphify.export import to_html, to_json
from graphify.extract import extract
from graphify.report import generate

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / 'graphify-out'
EXCLUDES = ['**/dependencies/**', '**/packages/**', '**/x64/**', '**/Debug/**',
            '**/Release/**', '**/framework-analysis/**', '**/graphify-out/**',
            '**/.agents/**', '**/docs/**', '**/*.md', '**/*.pdf', '**/*.json',
            '**/*.html', '**/*.dll', '**/*.lib', '**/*.png', '**/*.jpg', '**/*.ico']


def write_text(path, text):
    path.write_text(text.rstrip() + '\n', encoding='utf-8-sig')


def write_json(path, value):
    write_text(path, json.dumps(value, ensure_ascii=False, indent=2))


def audit_practice(extraction, number):
    """Add source-checked qualified methods omitted by the C++ AST extractor."""
    prefix = f'openGL_3dAssign_{number}/'
    files = {'console': prefix + 'Console.cpp', 'application': prefix + 'framework/src/Application.cpp',
             'graphics': prefix + 'framework/src/Graphics.cpp', 'main': prefix + 'main.cpp'}
    definitions = {}
    for area, filename in files.items():
        text = (ROOT / filename).read_text(encoding='utf-8-sig')
        if area == 'main':
            matches = list(re.finditer(r'^int (main)\s*\(', text, re.M))
        else:
            matches = list(re.finditer(
                r'^(?:bool|void|int|glm::mat4|std::optional<(?:EntityId|LineId)>|Polyhedron)\s+'
                r'((?:Console|Application|Scene|Renderer|DiagramSystem)::\w+)\s*\(', text, re.M))
        for index, match in enumerate(matches):
            symbol = match.group(1)
            end = matches[index + 1].start() if index + 1 < len(matches) else len(text)
            definitions[symbol] = {'area': area, 'file': filename, 'start': match.start(),
                                   'text': text, 'body': text[match.start():end],
                                   'line': text[:match.start()].count('\n') + 1}
    selected = ['main', 'Console::initScene', 'Console::onKey',
                'Application::run', 'Application::initialize', 'Application::shutdown',
                'Application::renderScene', 'Renderer::initialize', 'Renderer::destroy',
                'Renderer::render', 'Renderer::upload', 'Renderer::draw',
                'Scene::push_entity', 'Scene::setFaceLocalMatrix', 'Scene::setFaceVisible',
                'Scene::setTransform', 'Scene::addCuboid', 'Scene::addPyramid', 'Scene::addLine',
                'DiagramSystem::makePolyhedron', 'DiagramSystem::faceModelMatrix']
    if number == 13:
        selected += ['Console::selectFaces']
    elif number == 14:
        selected += ['Console::selectShape', 'Console::resetMotion', 'Console::applyTransform',
                     'Console::onUpdate', 'Console::onFocus']
    else:
        selected += ['Console::resetFaces', 'Console::selectShape', 'Console::resetAll',
                     'Console::applyObjectTransform', 'Console::beginMotion',
                     'Console::applyFaceMotion', 'Console::onUpdate']
    for symbol in selected:
        definition = definitions[symbol]
        definition['id'] = f'audit_practice{number}_' + symbol.replace('::', '_').lower()
        extraction['nodes'].append({'id': definition['id'], 'label': symbol,
            'file_type': 'code', 'source_file': definition['file'],
            'source_location': 'L' + str(definition['line']), '_origin': 'source_review',
            '_callable': True, 'audit_note': 'Source-checked definition; AST may also contain header declarations.'})
        file_node = next(n for n in extraction['nodes'] if n.get('source_file') == definition['file']
                         and n.get('source_location') == 'L1')
        extraction['edges'].append({'source': file_node['id'], 'target': definition['id'],
            'relation': 'defines', 'confidence': 'EXTRACTED', '_origin': 'source_review',
            'source_file': definition['file'], 'source_location': 'L' + str(definition['line'])})
    calls = {
        'main': ['Console::initScene', 'Console::onKey', 'Console::onUpdate'],
        'Console::initScene': ['DiagramSystem::makePolyhedron', 'Scene::push_entity', 'Console::resetAll'],
        'Console::resetFaces': ['Scene::setFaceLocalMatrix', 'Scene::setFaceVisible'],
        'Console::selectShape': ['Console::resetFaces', 'Console::applyObjectTransform'],
        'Console::resetAll': ['Console::selectShape'],
        'Console::applyObjectTransform': ['Scene::setTransform'],
        'Console::beginMotion': ['Console::applyFaceMotion'],
        'Console::applyFaceMotion': ['Scene::setFaceLocalMatrix', 'Scene::setFaceVisible'],
        'Console::onKey': ['Console::beginMotion', 'Console::selectShape', 'Console::resetAll'],
        'Console::onUpdate': ['Console::applyObjectTransform', 'Console::applyFaceMotion'],
        'Application::renderScene': ['Renderer::render'],
        'Renderer::render': ['Renderer::upload', 'Renderer::draw'],
        'Renderer::draw': ['DiagramSystem::faceModelMatrix']}
    if number == 13:
        calls = {
            'main': ['Console::initScene', 'Console::onKey'],
            'Console::initScene': ['Scene::addCuboid', 'Scene::addPyramid', 'Scene::setTransform',
                                   'Scene::addLine', 'Console::selectFaces'],
            'Console::selectFaces': ['Scene::setFaceVisible'],
            'Console::onKey': ['Console::selectFaces']}
    elif number == 14:
        calls = {
            'main': ['Console::initScene', 'Console::onKey', 'Console::onUpdate', 'Console::onFocus'],
            'Console::initScene': ['DiagramSystem::makePolyhedron', 'Scene::push_entity',
                                   'Scene::addLine', 'Console::resetMotion', 'Console::selectShape'],
            'Console::selectShape': ['Scene::setFaceVisible', 'Console::applyTransform'],
            'Console::resetMotion': ['Console::applyTransform'],
            'Console::applyTransform': ['Scene::setTransform'],
            'Console::onKey': ['Console::selectShape', 'Console::resetMotion'],
            'Console::onUpdate': ['Console::applyTransform']}
    calls.update({
        'main': calls['main'] + ['Application::run'],
        'Application::run': ['Application::initialize', 'Application::renderScene', 'Application::shutdown'],
        'Application::initialize': ['Renderer::initialize'],
        'Application::shutdown': ['Renderer::destroy'],
        'Application::renderScene': ['Renderer::render'],
        'Renderer::render': ['Renderer::upload', 'Renderer::draw'],
        'Renderer::draw': ['DiagramSystem::faceModelMatrix'],
        'Scene::addCuboid': ['Scene::push_entity', 'DiagramSystem::makePolyhedron'],
        'Scene::addPyramid': ['Scene::push_entity', 'DiagramSystem::makePolyhedron']})
    for caller, targets in calls.items():
        source = definitions[caller]
        for callee in targets:
            pattern = r'\b' + re.escape(callee.split('::')[-1]) + r'\s*\('
            found = list(re.finditer(pattern, source['body']))
            if not found:
                raise RuntimeError('Audited call no longer found: ' + caller + ' -> ' + callee)
            for match in found:
                line = source['text'][:source['start'] + match.start()].count('\n') + 1
                extraction['edges'].append({'source': source['id'], 'target': definitions[callee]['id'],
                    'relation': 'callback_body_calls' if caller == 'main' and callee.startswith('Console::') else 'calls',
                    'confidence': 'EXTRACTED', '_origin': 'source_review', 'source_file': source['file'],
                    'source_location': 'L' + str(line),
                    'context': 'registered lambda body; invoked later'
                    if caller == 'main' and callee.startswith('Console::') else 'source-checked call'})
    helpers = [] if number == 13 else [('Console::initScene', 'assignVertexColors')]
    if number == 15:
        helpers += [('Console::applyFaceMotion', 'rotateAround'), ('Console::applyFaceMotion', 'scaleAround')]
    for caller, helper in helpers:
        source = definitions[caller]
        target = next(n for n in extraction['nodes'] if n.get('source_file') == files['console']
                      and n.get('label') == helper + '()')
        for match in re.finditer(r'\b' + helper + r'\s*\(', source['body']):
            line = source['text'][:source['start'] + match.start()].count('\n') + 1
            extraction['edges'].append({'source': source['id'], 'target': target['id'],
                'relation': 'calls', 'confidence': 'EXTRACTED', '_origin': 'source_review',
                'source_file': source['file'], 'source_location': 'L' + str(line)})
    return len(selected)


def main():
    detected = detect(ROOT, extra_excludes=EXCLUDES)
    files = [Path(p) for p in detected['files'].get('code', [])
             if Path(p).suffix.lower() in {'.cpp', '.h', '.hpp', '.c', '.glsl'}]
    print(f'Code-only scope: {len(files)} files. Dependencies/build outputs excluded.', flush=True)
    if not files or len(files) > 500:
        raise RuntimeError('Empty or unexpectedly large code corpus')
    if detected.get('skipped_sensitive'):
        print('Sensitive files skipped:', detected['skipped_sensitive'], flush=True)
    detected['files'] = {'code': [str(p) for p in files]}
    detected['total_files'] = len(files)
    detected['total_words'] = sum(len(p.read_text(encoding='utf-8-sig', errors='replace').split())
                                  for p in files)
    extraction = extract(files, cache_root=ROOT, root=ROOT)
    if not extraction.get('nodes'):
        raise RuntimeError('AST extraction produced no nodes')
    audited = sum(audit_practice(extraction, number) for number in (13, 14, 15))
    print(f'Source audit: {audited} practice 13-15 definition nodes added.', flush=True)
    nodes = {n['id']: n for n in extraction['nodes']}
    cross = 0
    for edge in extraction['edges']:
        source = nodes.get(edge.get('source'), {}).get('source_file', '')
        target = nodes.get(edge.get('target'), {}).get('source_file', '')
        if source and target and source.split('/')[0] != target.split('/')[0] and edge.get('relation') == 'calls':
            edge['confidence'] = 'AMBIGUOUS'
            edge['audit_note'] = 'Same symbol across projects; runtime target requires source review.'
            cross += 1
    write_json(OUT / 'RELATIONS.json', extraction)
    health = diagnose_extraction(extraction, directed=True, root=ROOT)
    write_text(OUT / 'GRAPH_HEALTH.md', format_diagnostic_report(health) +
               f'\n\nCross-project call edges marked AMBIGUOUS: {cross}.\n'
               'AST is a static map; callbacks, preprocessing, and C++ overloads require source review.\n'
               '빌드·실행 테스트는 수행하지 않았다.\n')
    graph = build_from_json(extraction, root=ROOT, directed=True)
    communities = cluster(graph)
    scores = score_all(graph, communities)
    labels = {}
    names = {'openGL_3dAssign_13': '실습 13 면 선택', 'openGL_3dAssign_14': '실습 14 객체 조작',
             'openGL_3dAssign_15': '실습 15 중첩 모션', 'framework': '공용 3D 프레임워크'}
    for cid, members in communities.items():
        areas = collections.Counter(graph.nodes[n].get('source_file', '').split('/')[0] for n in members)
        area = areas.most_common(1)[0][0]
        labels[cid] = names.get(area, area + ' 코드 구조')
    gods = god_nodes(graph)
    surprises = surprising_connections(graph, communities)
    questions = suggest_questions(graph, communities, labels)
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    # Full rescan intentionally drops the three deleted Common.h compatibility files.
    if not to_json(graph, communities, str(OUT / 'graph.json'), force=True,
                   built_at_commit=commit, community_labels=labels):
        raise RuntimeError('Graph export failed')
    report = generate(graph, communities, scores, labels, gods, surprises, detected,
                      {'input': 0, 'output': 0}, str(ROOT), suggested_questions=questions,
                      built_at_commit=commit)
    report += '\n## 실습 15 최신 동작\n\n'
    report += 'S는 마주 보는 왼쪽·오른쪽 면을 회전한다. 1은 360도 반복, 2는 90도 자리 교체와 복귀를 반복한다.\n'
    report += '모션별 initialized/running/progress 상태가 독립이며 T/F/S/B/1/2/4/5 행렬을 합성한다.\n'
    report += '실습별 호출 순서와 CPU→GPU 흐름은 EXECUTION_FLOW.md, 상세 모션은 PRACTICE15.md에서 확인한다.\n'
    report += '코드만 AST 분석했다. 의존성·출력·문서·PDF는 그래프 추출에서 제외했다. 외부 API 및 LLM 토큰 비용은 0이다.\n'
    report += '행렬 중첩은 기준 로컬 축과 고정 합성 순서에 따른다. 실제 화면·빌드 성공은 사용자 확인 필요.\n'
    write_text(OUT / 'GRAPH_REPORT.md', report)
    if not to_html(graph, communities, str(OUT / 'graph.html'), community_labels=labels):
        raise RuntimeError('HTML export failed')
    for name in ['graph.json', 'graph.html']:
        write_text(OUT / name, (OUT / name).read_text(encoding='utf-8-sig'))
    write_json(OUT / '.graphify_labels.json', labels)
    # The manifest library expects a BOM-free intermediate file.
    temp_manifest = OUT / '.graphify_detect.json'
    save_manifest({'code': [str(p) for p in files]}, str(temp_manifest), kind='ast', root=ROOT,
                  scan_corpus={str(p) for p in files})
    write_text(OUT / 'manifest.json', temp_manifest.read_text(encoding='utf-8-sig'))
    temp_manifest.unlink()
    ns = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}
    projects = []
    for p in sorted(ROOT.rglob('*.vcxproj')):
        if any(x in {'dependencies', 'packages', '.vs', 'Debug', 'Release', 'x64'} for x in p.parts):
            continue
        sources = []
        entrypoints = []
        for element in ET.parse(p).findall('.//m:ClCompile', ns):
            include = element.get('Include')
            if not include:
                continue
            source = (p.parent / include.replace('\\', '/')).resolve()
            if not source.is_file():
                continue
            rel = source.relative_to(ROOT).as_posix()
            sources.append(rel)
            text = source.read_text(encoding='utf-8-sig', errors='replace')
            for match in re.finditer(r'\b(?:int|void)\s+(main|wmain)\s*\(', text):
                entrypoints.append({'file': rel, 'line': text[:match.start()].count('\n') + 1,
                                    'name': match.group(1)})
        projects.append({'project': p.relative_to(ROOT).as_posix(), 'sources': sources,
                         'entrypoints': entrypoints})
    write_json(OUT / 'PROJECTS.json', projects)
    write_json(OUT / 'SOURCE_HASHES.json', {p.relative_to(ROOT).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest()
                                         for p in files})
    cost_path = OUT / 'cost.json'
    cost = json.loads(cost_path.read_text(encoding='utf-8-sig')) if cost_path.exists() else {'runs': []}
    cost['runs'].append({'date': datetime.datetime.now(datetime.timezone.utc).isoformat(),
                         'input_tokens': 0, 'output_tokens': 0, 'files': len(files)})
    write_json(cost_path, cost)
    for name in ['graph.json', 'graph.html', 'GRAPH_REPORT.md', 'RELATIONS.json', 'PROJECTS.json']:
        text = (OUT / name).read_text(encoding='utf-8-sig')
        if '\ufffd' in text:
            raise RuntimeError('Unicode replacement character: ' + name)
    json.loads((OUT / 'graph.json').read_text(encoding='utf-8-sig'))
    print(f'Graph: {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges, '
          f'{len(communities)} communities; {len(projects)} projects.', flush=True)
    print(format_diagnostic_report(health), flush=True)


if __name__ == '__main__':
    main()
