from pathlib import Path
import json
import hashlib
import subprocess
import graphify.extract as extraction_api
from graphify.detect import detect, save_manifest
from graphify.build import build_from_json
from graphify.cluster import cluster, score_all
from graphify.analyze import god_nodes, surprising_connections, suggest_questions
from graphify.report import generate
from graphify.export import to_json, to_html
from graphify.diagnostics import diagnose_extraction, format_diagnostic_report

base = Path(__file__).resolve().parent
root = base.parent / 'framework'
output = base / 'graphify-out'
extraction_api._DISPATCH['.inl'] = extraction_api.extract_cpp
files = sorted(p for p in root.rglob('*') if p.is_file()
               and p.suffix in {'.h', '.cpp', '.inl'} and 'dependencies' not in p.parts)
detection = detect(root, extra_excludes=['dependencies/**', 'docs/**', '*.md', 'shaders/**'], cache_root=base)
detection['files'] = {'code': [str(p) for p in files], 'document': [], 'paper': [], 'image': [], 'video': []}
detection['total_files'] = len(files)
detection['total_words'] = sum(len(p.read_text(encoding='utf-8-sig').split()) for p in files)
extraction = extraction_api.extract(files, cache_root=base, root=root)
graph = build_from_json(extraction, root=str(root), directed=True)
old = json.loads((output / 'graph.json').read_text(encoding='utf-8-sig'))
if not graph.number_of_nodes() or graph.number_of_nodes() < len(old['nodes']):
    raise RuntimeError('Empty or smaller graph: existing graph preserved')
communities = cluster(graph)
cohesion = score_all(graph, communities)
names = {'Native.h': '네이티브 함수 연결', 'Geometry.h': 'UV와 기하 계산',
         'Scene.inl': '장면과 계층 변환', 'Renderer.inl': 'GPU 렌더링',
         'Picking.h': '교점과 UV 피킹', 'PrimitiveLoader.inl': '기본 도형 파서',
         'AssimpLoader.inl': '일반 모델 로딩', 'Application.inl': '앱 실행과 입력',
         'Model.h': '모델과 재질 데이터', 'Graphics.h': '장면 공개 API',
         'Texture.h': '이미지 디코딩', 'Shaders.h': '내장 셰이더'}
labels = {}
for cid, members in communities.items():
    counts = {}
    for node in members:
        source = Path(graph.nodes[node].get('source_file', '')).name
        counts[source] = counts.get(source, 0) + 1
    source = max(counts, key=counts.get) if counts else 'Framework'
    labels[cid] = names.get(source, source + ' 구조')
gods = god_nodes(graph)
surprises = surprising_connections(graph, communities)
questions = suggest_questions(graph, communities, labels)
staged = output / 'graph.generated.json'
if not to_json(graph, communities, str(staged), force=True, community_labels=labels):
    raise RuntimeError('Graph export failed')
staged.write_text(staged.read_text(encoding='utf-8-sig'), encoding='utf-8-sig')
staged.replace(output / 'graph.json')

def dump(name, value):
    (output / name).write_text(json.dumps(value, ensure_ascii=False, indent=2), encoding='utf-8-sig')

dump('EXTRACTION.json', extraction)
dump('DETECTION.json', detection)
dump('COMMUNITIES.json', {str(k): v for k, v in labels.items()})
dump('ANALYSIS.json', {'communities': {str(k): v for k, v in communities.items()},
                     'cohesion': {str(k): v for k, v in cohesion.items()},
                     'gods': gods, 'surprises': surprises, 'questions': questions})
report = generate(graph, communities, cohesion, labels, gods, surprises, detection,
                  {'input': 0, 'output': 0}, str(root), suggested_questions=questions)
(output / 'GRAPH_REPORT.md').write_text(report, encoding='utf-8-sig')
health = diagnose_extraction(extraction, directed=True, root=str(root))
(output / 'GRAPH_HEALTH.md').write_text(format_diagnostic_report(health), encoding='utf-8-sig')
if not to_html(graph, communities, str(output / 'graph.html'), community_labels=labels):
    raise RuntimeError('HTML export incomplete')
html = output / 'graph.html'
html.write_text(html.read_text(encoding='utf-8-sig'), encoding='utf-8-sig')
save_manifest({'code': [str(p) for p in files]}, str(output / 'manifest.json'),
              kind='ast', root=root, scan_corpus=[str(p) for p in files])
manifest = output / 'manifest.json'
manifest.write_text(manifest.read_text(encoding='utf-8-sig'), encoding='utf-8-sig')
commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=base, text=True).strip()
dump('SOURCE_MANIFEST.json', {'source_root': str(root), 'output_root': str(output),
                            'source_commit': commit, 'includes_uncommitted_step4': True,
                            'files': [{'path': p.relative_to(root).as_posix(),
                                       'sha256': hashlib.sha256(p.read_bytes()).hexdigest()} for p in files]})
dump('BENCHMARK.json', {'status': 'not_run', 'reason': 'User delegates tests and benchmarks; previous estimate is obsolete.'})
dump('cost.json', {'run': '2026-10-12', 'mode': 'AST only', 'input_tokens': 0,
                   'output_tokens': 0, 'external_api_calls': 0})
print(json.dumps({'files': len(files), 'nodes': graph.number_of_nodes(),
                  'edges': graph.number_of_edges(), 'communities': len(communities)}))
print(format_diagnostic_report(health))
