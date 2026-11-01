[graphify] MultiDiGraph edge-collapse diagnostic
input: <in-memory>
input_stage: provided JSON (normal graph.json is post-build)
effective_directed: <direct-call>
nodes: 1266
unverified_code_nodes: 0
raw_edges: 2159
valid_candidate_edges: 2104
missing_endpoint_edges: 0
dangling_endpoint_edges: 55
self_loop_edges: 0
exact_duplicate_edges: 41
directed_unique_endpoint_pairs: 1943
directed_same_endpoint_collapsed_edges: 161
undirected_unique_endpoint_pairs: 1937
undirected_same_endpoint_collapsed_edges: 167
same_endpoint_group_count: 119
relation_variant_groups: 31
source_file_variant_groups: 0
source_location_variant_groups: 38
context_variant_groups: 34
post_build_graph_type: DiGraph
post_build_edges: 1941
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
  - include_application_application -> include_application_h_function edges=8 relations=['references'] locations=['L47', 'L48', 'L53', 'L54', 'L55', 'L56', 'L57', 'L58'] contexts=['field']
  - include_graphics_scene -> include_graphics_h_vector edges=6 relations=['references'] locations=['L21', 'L22', 'L23', 'L24', 'L25', 'L26'] contexts=['field']
  - include_model_planefigure -> include_model_h_vector edges=6 relations=['references'] locations=['L68', 'L76', 'L77', 'L78', 'L79', 'L80'] contexts=['field']
  - include_graphics_renderer -> glint edges=4 relations=['references'] locations=['L178', 'L179', 'L180', 'L181'] contexts=['field']
  - detail_objloader_objresolve -> size_t edges=4 relations=['calls', 'references'] locations=['L51', 'L55'] contexts=['call', 'parameter_type', 'return_type']
note: normal graph.json is post-build; raw producer loss must be measured earlier.
Cross-program calls marked AMBIGUOUS: 0. Source anchors: 172.
