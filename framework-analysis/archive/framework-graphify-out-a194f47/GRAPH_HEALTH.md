[graphify] MultiDiGraph edge-collapse diagnostic
input: <in-memory>
input_stage: provided JSON (normal graph.json is post-build)
effective_directed: <direct-call>
nodes: 609
unverified_code_nodes: 0
raw_edges: 1128
valid_candidate_edges: 1082
missing_endpoint_edges: 0
dangling_endpoint_edges: 46
self_loop_edges: 0
exact_duplicate_edges: 16
directed_unique_endpoint_pairs: 1005
directed_same_endpoint_collapsed_edges: 77
undirected_unique_endpoint_pairs: 1000
undirected_same_endpoint_collapsed_edges: 82
same_endpoint_group_count: 52
relation_variant_groups: 7
source_file_variant_groups: 0
source_location_variant_groups: 24
context_variant_groups: 9
post_build_graph_type: DiGraph
post_build_edges: 946
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
  - include_graphics_scene -> include_graphics_h_vector edges=6 relations=['references'] locations=['L20', 'L21', 'L22', 'L23', 'L24', 'L25'] contexts=['field']
  - include_model_planefigure -> include_model_h_vector edges=6 relations=['references'] locations=['L68', 'L76', 'L77', 'L78', 'L79', 'L80'] contexts=['field']
  - include_graphics_renderer -> glint edges=4 relations=['references'] locations=['L144', 'L145', 'L146', 'L147'] contexts=['field']
  - include_model_transform3d -> include_model_h_vec3 edges=4 relations=['references'] locations=['L35', 'L36', 'L37', 'L38'] contexts=['field']
note: normal graph.json is post-build; raw producer loss must be measured earlier.