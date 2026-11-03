# 외부 include 대상 보완

## 문제·원인

누락 간선 123개는 내부 함수 호출 누락이 아니라 외부 include 대상 노드 33개 부재였다. SDK·시스템 헤더를 코드 추출 범위에서 제외하면서 imports 간선의 끝점이 없는 상태로 남았다.

## 적용 범위

- 표준·Windows 헤더 참조 23개: 실제 include 원문·소스 줄 번호로 경계 노드를 생성했다. 시스템 헤더 내부나 설치 경로를 분석한 것으로 표시하지 않는다.
- 배포 SDK 헤더 10개: include 상대 경로와 실제 파일 존재를 확인했다. SDK 내부 정의는 추출하지 않았다.
- 기존 그래프 노드 3,495개·간선 5,234개·커뮤니티를 유지했다. 외부 경계 33개·include 간선 123개만 추가했다.
- 기존 노드·간선의 데이터 차이: 0개 / 0개. 원본 추출 간선 배열은 변경하지 않았다.
- 현재 그래프: 3,528개 노드·5,357개 간선. 누락 대상 0건. 관계 병합 150건은 이번 보완과 별도다.
- 빌드·테스트·브라우저 실행은 하지 않았다. graphify 생성과 소스·데이터 대조를 실행 성공으로 간주하지 않는다.

## 주된 변경 전후

### before

`Application.cpp → algorithm` imports 간선은 있었으나 algorithm 대상 노드는 없었다.

### after

동일 간선을 유지하고 `외부 헤더: algorithm` 경계 노드를 추가했다. include 원문과 파일·줄 번호를 노드 근거로 보관한다. 정의·상속·외부 함수 호출은 추가하지 않았다.

## 생성기 전체 변경 블록

### before
```python

def dump(path, value):
    write(path, json.dumps(value, ensure_ascii=False, indent=2) + '\n')


def row(x, y, width, *lines, decision=False):
```

### after
```python

def dump(path, value):
    write(path, json.dumps(value, ensure_ascii=False, indent=2) + '\n')


def complete_include_boundaries(extraction):
    """Add only missing external include endpoints, proved by the current include line."""
    known = {n['id'] for n in extraction['nodes']}
    pending = collections.defaultdict(list)
    for edge in extraction['edges']:
        if edge.get('source') not in known:
            raise RuntimeError('Missing source node requires separate investigation: ' + str(edge))
        if edge.get('target') in known:
            continue
        if edge.get('relation') != 'imports':
            raise RuntimeError('Missing non-include endpoint requires separate investigation: ' + str(edge))
        file = edge['source_file']
        line = int(re.fullmatch(r'L(\d+)', edge['source_location']).group(1))
        source = read(file).splitlines()[line - 1]
        include = re.fullmatch(r'\s*#\s*include\s*([<"])([^>"]+)[>"]\s*(?://.*)?', source)
        if not include:
            raise RuntimeError('Include evidence changed: ' + file + ':' + str(line))
        delimiter, header = include.groups()
        evidence = dict(source_file=file, source_location='L' + str(line), include=source.strip())
        if delimiter == '"':
            target = (ROOT / file).parent.joinpath(header).resolve()
            relative = target.relative_to(ROOT).as_posix()
            if 'dependencies' not in target.parts or not target.is_file():
                raise RuntimeError('Internal or absent header requires separate investigation: ' + relative)
            evidence.update(boundary_kind='vendored_sdk_header', header_path=relative)
        else:
            evidence.update(boundary_kind='system_header_reference', header_path=header)
        pending[edge['target']].append(evidence)
    nodes = []
    for ident, evidence in sorted(pending.items()):
        first = evidence[0]
        nodes.append(dict(id=ident, label='외부 헤더: ' + first['header_path'],
                          file_type='concept', source_file='', source_location='',
                          _origin='source_verified_include_boundary', boundary_kind=first['boundary_kind'],
                          header_path=first['header_path'], include_evidence=evidence,
                          confidence='EXTRACTED',
                          description='include 지시문으로 확인한 외부 경계. 헤더 내부 정의·함수 호출을 분석한 노드가 아님.'))
    extraction['nodes'].extend(nodes)
    return nodes


def repair_saved_graph():
    """Patch saved graph endpoints without extracting or reclustering existing code."""
    import networkx as nx
    from graphify.export import to_json, to_html
    from graphify.analyze import god_nodes, surprising_connections, suggest_questions
    from graphify.cluster import score_all
    from graphify.report import generate
    from graphify.diagnostics import diagnose_extraction, format_diagnostic_report
    output = OUT / 'graphify-out'
    extraction = json.loads(read(output / 'EXTRACTION.json'))
    original = json.loads(read(output / 'graph.json'))
    graph = nx.node_link_graph(original, edges='links' if 'links' in original else 'edges')
    before = dict(nodes=graph.number_of_nodes(), edges=graph.number_of_edges())
    added = complete_include_boundaries(extraction)
    ids = {n['id'] for n in added}
    edges = [e for e in extraction['edges'] if e.get('target') in ids]
    for node in added:
        neighbours = [e['source'] for e in edges if e['target'] == node['id']]
        if any(n not in graph for n in neighbours):
            raise RuntimeError('Saved graph has a missing include source; preserve outputs.')
        votes = collections.Counter(graph.nodes[n]['community'] for n in neighbours)
        cid = votes.most_common(1)[0][0]
        graph.add_node(node['id'], **{k: v for k, v in node.items() if k != 'id'}, community=cid)
    for edge in edges:
        graph.add_edge(edge['source'], edge['target'], **{k: v for k, v in edge.items() if k not in ('source', 'target')})
    communities, labels = collections.defaultdict(list), {}
    for ident, attrs in graph.nodes(data=True):
        cid = attrs['community']
        communities[cid].append(ident)
        if attrs.get('community_name'):
            labels[cid] = attrs['community_name']
    gods, surprises = god_nodes(graph), surprising_connections(graph, communities)
    questions = suggest_questions(graph, communities, labels)
    health = diagnose_extraction(extraction, directed=True, root=str(ROOT))
    detection = json.loads(read(output / 'DETECTION.json'))
    report = generate(graph, communities, score_all(graph, communities), labels, gods, surprises, detection,
                      {'input': 0, 'output': 0}, str(ROOT), suggested_questions=questions,
                      built_at_commit=original.get('built_at_commit'))
    report += '\n## 외부 헤더 경계 보완'
    report += '\n\n기존 코드 노드·간선·커뮤니티를 유지하고 include 원문에 근거한 외부 헤더 경계만 보완했다. 표준·Windows 헤더는 include 참조만, SDK 헤더는 로컬 파일 존재까지 확인했다. 외부 함수 정의를 추정하지 않았다. 관계 병합 150건은 별도 항목이며 원본 간선은 EXTRACTION.json에 유지된다. framework와 standard/framework는 별도 경로이며 standard 실행은 후자를 사용한다. 같은 이름의 메서드 간 연결은 정확한 오버로드 해석이 아니다. GPU 고정 단계는 C++ 호출 간선이 아니다. 빌드·테스트·브라우저 확인은 사용자에게 맡긴다.\n'
    audit = dict(mode='saved_graph_endpoint_patch', before=before,
                 after=dict(nodes=graph.number_of_nodes(), edges=graph.number_of_edges()),
                 added_nodes=added, restored_edges=edges, remaining_dangling=health['dangling_endpoint_edges'],
                 existing_code_reextracted=False, existing_communities_reclustered=False)
    staged = output / 'graph.boundary-patch.json'
    if not to_json(graph, communities, str(staged), community_labels=labels,
                   built_at_commit=original.get('built_at_commit')):
        raise RuntimeError('Graph export refused; preserve previous report.')
    os.replace(staged, output / 'graph.json')
    to_html(graph, communities, str(output / 'graph.html'), community_labels=labels)
    dump(output / 'EXTRACTION.json', extraction)
    dump(output / 'BOUNDARY_REPAIR.json', audit)
    dump(output / 'ANALYSIS.json', dict(gods=gods, surprises=surprises, questions=questions))
    write(output / 'GRAPH_REPORT.md', report)
    write(output / 'GRAPH_HEALTH.md', format_diagnostic_report(health))
    for name in ('graph.json', 'graph.html'):
        write(output / name, read(output / name))
    print(f'Boundary patch: +{len(added)} nodes, +{len(edges)} include edges; remaining dangling: {health["dangling_endpoint_edges"]}.')


def row(x, y, width, *lines, decision=False):
```

### before
```python
        block = '<!-- API-ARCHITECTURE:BEGIN --><section id="api-architecture"><h2>API 코드베이스: 네 관계와 종합 흐름</h2>'
        block += '<p>정적 소스와 graphify AST 분석에 근거한다. 화살표의 소유·참조·호출·생성·ID 연결을 구분한다. 각 설명의 소스 링크에서 실제 구현을 읽는다.</p>'
        if missing and collapsed:
            block += '<p class="badge">graphify 추출 한계: 연결 대상 누락 ' + missing.group(1) + '건, 같은 노드 쌍의 관계 병합 ' + collapsed.group(1) + '건. 원본 간선은 EXTRACTION.json에 보존했다. 아래 순서도는 그래프만으로 확정하지 않고 실제 소스를 대조했다. 빌드·실행·브라우저 확인은 사용자에게 맡긴다.</p>'
        block += '<nav>'
        block += ''.join('<a href="#arch-' + key + '">' + esc(c['title']) + '</a>' for key, c in charts.items()) + '</nav>'
        for key, chart in charts.items():
```

### after
```python
        block = '<!-- API-ARCHITECTURE:BEGIN --><section id="api-architecture"><h2>API 코드베이스: 네 관계와 종합 흐름</h2>'
        block += '<p>정적 소스와 graphify AST 분석에 근거한다. 화살표의 소유·참조·호출·생성·ID 연결을 구분한다. 각 설명의 소스 링크에서 실제 구현을 읽는다.</p>'
        if missing and collapsed:
            block += '<p class="badge">graphify 연결 대상 누락: ' + missing.group(1) + '건. 외부 헤더는 include 근거를 가진 경계 노드이며 내부 함수 분석을 뜻하지 않는다. 같은 노드 쌍의 관계 병합 ' + collapsed.group(1) + '건은 원본 EXTRACTION.json에 보존했다. 아래 순서도는 실제 소스를 대조했다. 빌드·실행·브라우저 확인은 사용자에게 맡긴다.</p>'
        block += '<nav>'
        block += ''.join('<a href="#arch-' + key + '">' + esc(c['title']) + '</a>' for key, c in charts.items()) + '</nav>'
        for key, chart in charts.items():
```

### before
```python
    extraction = extract(files, root=ROOT, cache_root=output / 'cache')
    updater = load('api_graph_anchors', 'framework-analysis/update_graph.py')
    updater.add_source_anchors(extraction, files, ROOT)
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    if not graph.number_of_nodes():
        raise RuntimeError('Empty graph; preserve previous graph.')
```

### after
```python
    extraction = extract(files, root=ROOT, cache_root=output / 'cache')
    updater = load('api_graph_anchors', 'framework-analysis/update_graph.py')
    updater.add_source_anchors(extraction, files, ROOT)
    complete_include_boundaries(extraction)
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    if not graph.number_of_nodes():
        raise RuntimeError('Empty graph; preserve previous graph.')
```

### before
```python
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--graph', action='store_true')
    args = parser.parse_args()
    if args.graph:
        build_graph()
    refresh_diagrams()
```

### after
```python
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--graph', action='store_true')
    parser.add_argument('--repair-graph', action='store_true', help='Patch only missing include endpoints in the saved graph.')
    args = parser.parse_args()
    if args.repair_graph:
        repair_saved_graph()
    elif args.graph:
        build_graph()
    refresh_diagrams()
```
