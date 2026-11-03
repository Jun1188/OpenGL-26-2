"""Generate a code-only structure graph. Does not build, test, or run diagnostics."""
from pathlib import Path
import argparse
import datetime
import hashlib
import json
import re
import subprocess
import xml.etree.ElementTree as ET

from graphify.extract import extract
from graphify.build import build_from_json
from graphify.cluster import cluster
from graphify.export import to_json, to_html

ROOT = Path(__file__).resolve().parent.parent
_parser = argparse.ArgumentParser(description=__doc__)
_parser.add_argument('--output', default='graphify-out/standard-input')
_args = _parser.parse_args()
OUT = (ROOT / _args.output).resolve()
OUT.relative_to(ROOT)


def write(name, value):
    (OUT / name).write_text(value.rstrip() + '\n', encoding='utf-8-sig')


def dump(name, value):
    write(name, json.dumps(value, ensure_ascii=False, indent=2))


def inventory():
    projects = []
    ns = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}
    tracked = subprocess.check_output(['git', 'ls-files', '*.vcxproj'], cwd=ROOT, text=True, encoding='utf-8')
    historical = json.loads((ROOT / 'graphify-out/PROJECTS.json').read_text(encoding='utf-8-sig'))
    for relative in tracked.splitlines():
        path = ROOT / relative
        if not path.is_file():
            prior = next((p for p in historical if p.get('project') == relative), {})
            projects.append({**prior, 'project': relative, 'available': False,
                             'note': 'Absent before this task. Entry information is historical; no file was restored or deleted.'})
            continue
        sources = set()
        for element in ET.parse(path).findall('.//m:ClCompile', ns):
            include = element.get('Include')
            if include and '$(' not in include:
                sources.update(p.resolve() for p in path.parent.glob(include.replace('\\', '/')) if p.is_file())
        bundled = path.parent / 'framework/detail'
        if bundled.is_dir():
            sources.update(p.resolve() for p in bundled.glob('*.cpp'))
        entries = []
        for source in sorted(sources):
            text = source.read_text(encoding='utf-8-sig')
            for match in re.finditer(r'\b(?:int|void)\s+(main|wmain)\s*\(', text):
                entries.append({'file': source.relative_to(ROOT).as_posix(), 'name': match[1],
                                'line': text[:match.start()].count('\n') + 1,
                                'conditional_compilation': 'Review preprocessor branches in this source.' if '#if' in text else None})
        projects.append({'project': relative, 'available': True,
                         'sources': [p.relative_to(ROOT).as_posix() for p in sorted(sources)], 'entrypoints': entries})
    for name in ('basic', 'assets', 'motion'):
        projects.append({'standalone_example': True, 'file': f'framework/examples/{name}.cpp', 'entrypoint': 'main'})
    return projects


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    excluded = {'.git', '.vs', '.agents', '.codex', 'dependencies', 'packages', 'archive',
                'x64', 'Debug', 'Release', '.obj-work', 'dist', 'tests'}
    files = []
    for path in ROOT.rglob('*'):
        rel = path.relative_to(ROOT)
        if path.suffix.lower() not in {'.cpp', '.h', '.hpp'} or any(p in excluded for p in rel.parts):
            continue
        if 'framework' in rel.parts and rel.parts[0] != 'framework':
            continue
        files.append(path)
    # Include the two owning standard copies changed in this task, rather than every SDK distribution.
    files += [ROOT / 'standard/framework/include/Picking.h', ROOT / 'standard/framework/detail/Application.cpp']
    files = sorted(set(files))
    extraction = extract(files, cache_root=OUT, root=ROOT)
    node_sources = {n['id']: n.get('source_file', '').replace('\\', '/') for n in extraction['nodes']}
    for edge in extraction['edges']:
        a = node_sources.get(edge.get('source'), '').split('/')[0]
        b = node_sources.get(edge.get('target'), '').split('/')[0]
        if a and b and a != b and b != 'framework' and edge.get('relation') == 'calls':
            edge['confidence'] = 'AMBIGUOUS'
            edge['audit_note'] = 'Independent executables can have identically named classes. Confirm against owning sources.'
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    communities = cluster(graph)
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    to_json(graph, communities, str(OUT / 'graph.json'), force=True, built_at_commit=commit)
    to_html(graph, communities, str(OUT / 'graph.html'))
    for name in ('graph.json', 'graph.html'):
        write(name, (OUT / name).read_text(encoding='utf-8-sig'))
    dump('EXTRACTION.json', extraction)
    dump('PROJECTS.json', inventory())
    dump('SOURCE_MANIFEST.json', {
        'date': datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).date().isoformat(), 'source_commit': commit,
        'method': 'graphify AST; no semantic API, build, tests, app run, or extraction diagnostics',
        'nodes': graph.number_of_nodes(), 'edges': graph.number_of_edges(),
        'files': {p.relative_to(ROOT).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest() for p in files},
        'limits': ['Dynamic callback and virtual-dispatch order requires source review.',
                   'SDKs, tests, archives and duplicate framework distributions are excluded.',
                   'Missing pre-existing projects remain historical entries.']})
    print(f'graphify AST: {len(files)} files, {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges. No tests or diagnostics run.')


if __name__ == '__main__':
    main()
