[graphify] MultiDiGraph edge-collapse diagnostic
input: <in-memory>
input_stage: provided JSON (normal graph.json is post-build)
effective_directed: <direct-call>
nodes: 1013
unverified_code_nodes: 0
raw_edges: 1698
valid_candidate_edges: 1649
missing_endpoint_edges: 0
dangling_endpoint_edges: 49
self_loop_edges: 0
exact_duplicate_edges: 31
directed_unique_endpoint_pairs: 1526
directed_same_endpoint_collapsed_edges: 123
undirected_unique_endpoint_pairs: 1520
undirected_same_endpoint_collapsed_edges: 129
same_endpoint_group_count: 89
relation_variant_groups: 18
source_file_variant_groups: 0
source_location_variant_groups: 34
context_variant_groups: 22
post_build_graph_type: DiGraph
post_build_edges: 1524
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
  - include_application_application -> include_application_h_function edges=8 relations=['references'] locations=['L42', 'L43', 'L48', 'L49', 'L50', 'L51', 'L52', 'L53'] contexts=['field']
  - include_graphics_scene -> include_graphics_h_vector edges=6 relations=['references'] locations=['L21', 'L22', 'L23', 'L24', 'L25', 'L26'] contexts=['field']
  - include_model_planefigure -> include_model_h_vector edges=6 relations=['references'] locations=['L66', 'L74', 'L75', 'L76', 'L77', 'L78'] contexts=['field']
  - include_graphics_renderer -> glint edges=4 relations=['references'] locations=['L168', 'L169', 'L170', 'L171'] contexts=['field']
  - detail_objloader_objresolve -> size_t edges=4 relations=['calls', 'references'] locations=['L50', 'L54'] contexts=['call', 'parameter_type', 'return_type']
note: normal graph.json is post-build; raw producer loss must be measured earlier.
Cross-program calls marked AMBIGUOUS: 0. Source anchors: 134.
