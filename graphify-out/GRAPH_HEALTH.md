[graphify] MultiDiGraph edge-collapse diagnostic
input: <in-memory>
input_stage: provided JSON (normal graph.json is post-build)
effective_directed: <direct-call>
nodes: 3348
unverified_code_nodes: 0
raw_edges: 5706
valid_candidate_edges: 5446
missing_endpoint_edges: 0
dangling_endpoint_edges: 260
self_loop_edges: 0
exact_duplicate_edges: 23
directed_unique_endpoint_pairs: 5314
directed_same_endpoint_collapsed_edges: 132
undirected_unique_endpoint_pairs: 5303
undirected_same_endpoint_collapsed_edges: 143
same_endpoint_group_count: 126
relation_variant_groups: 47
source_file_variant_groups: 0
source_location_variant_groups: 17
context_variant_groups: 62
post_build_graph_type: DiGraph
post_build_edges: 5295
producer_suppression_sites: 12
producer_suppression_examples:
  - L1204 seen_ids arity=unknown
  - L1520 seen_ids arity=unknown
  - L1522 seen_doc_refs arity=unknown
  - L1885 seen_ids arity=unknown
  - L2032 seen_ids arity=unknown
  - L2670 seen_keys arity=unknown
  - L2839 seen_keys arity=unknown
  - L4436 seen_ids arity=unknown
examples:
  - framework_detail_primitiveloader_objindex -> size_t edges=3 relations=['calls', 'references'] locations=['L14', 'L7'] contexts=['call', 'parameter_type', 'return_type']
  - framework_detail_transform_motionunitvector -> vec3 edges=3 relations=['calls', 'references'] locations=['L10', 'L6'] contexts=['call', 'generic_arg', 'parameter_type']
  - framework_detail_transform_motionaround -> mat4 edges=3 relations=['calls', 'references'] locations=['L12', 'L15'] contexts=['call', 'generic_arg', 'parameter_type']
  - framework_detail_transform_pathsystem_ellipse -> vec3 edges=3 relations=['references'] locations=['L67'] contexts=['parameter_type']
  - framework_detail_transform_pathsystem_circle -> vec3 edges=3 relations=['calls', 'references'] locations=['L76', 'L80'] contexts=['call', 'parameter_type']
note: normal graph.json is post-build; raw producer loss must be measured earlier.
Cross-program calls marked AMBIGUOUS: 2. Source anchors: 548.
