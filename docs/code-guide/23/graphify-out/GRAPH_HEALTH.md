[graphify] MultiDiGraph edge-collapse diagnostic
input: <in-memory>
input_stage: provided JSON (normal graph.json is post-build)
effective_directed: <direct-call>
nodes: 1294
unverified_code_nodes: 0
raw_edges: 2128
valid_candidate_edges: 2071
missing_endpoint_edges: 0
dangling_endpoint_edges: 57
self_loop_edges: 0
exact_duplicate_edges: 35
directed_unique_endpoint_pairs: 1968
directed_same_endpoint_collapsed_edges: 103
undirected_unique_endpoint_pairs: 1962
undirected_same_endpoint_collapsed_edges: 109
same_endpoint_group_count: 86
relation_variant_groups: 31
source_file_variant_groups: 0
source_location_variant_groups: 9
context_variant_groups: 35
post_build_graph_type: DiGraph
post_build_edges: 1966
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
  - framework_detail_objloader_objresolve -> size_t edges=4 relations=['calls', 'references'] locations=['L51', 'L55'] contexts=['call', 'parameter_type', 'return_type']
  - framework_detail_patterns_patternsystem_screenonplane -> vec3 edges=4 relations=['calls', 'references'] locations=['L332', 'L339'] contexts=['call', 'generic_arg', 'parameter_type']
  - framework_include_gridlayout_gridlayout_position -> framework_include_gridlayout_h_vec3 edges=3 relations=['calls', 'references'] locations=['L19', 'L25'] contexts=['call', 'generic_arg', 'parameter_type']
  - framework_include_gridlayout_gridlayout_positioninparent -> framework_include_gridlayout_h_vec3 edges=3 relations=['calls', 'references'] locations=['L51', 'L57'] contexts=['call', 'generic_arg', 'parameter_type']
  - framework_detail_objloader_objtext -> string edges=3 relations=['references'] locations=['L74'] contexts=['generic_arg', 'parameter_type', 'return_type']
note: normal graph.json is post-build; raw producer loss must be measured earlier.
Cross-program calls marked AMBIGUOUS: 0. Source anchors: 175.
