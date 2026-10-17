[graphify] MultiDiGraph edge-collapse diagnostic
input: <in-memory>
input_stage: provided JSON (normal graph.json is post-build)
effective_directed: <direct-call>
nodes: 3117
unverified_code_nodes: 0
raw_edges: 5290
valid_candidate_edges: 5035
missing_endpoint_edges: 0
dangling_endpoint_edges: 255
self_loop_edges: 0
exact_duplicate_edges: 13
directed_unique_endpoint_pairs: 4931
directed_same_endpoint_collapsed_edges: 104
undirected_unique_endpoint_pairs: 4920
undirected_same_endpoint_collapsed_edges: 115
same_endpoint_group_count: 103
relation_variant_groups: 36
source_file_variant_groups: 0
source_location_variant_groups: 15
context_variant_groups: 55
post_build_graph_type: DiGraph
post_build_edges: 4912
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
  - computer_graphics_warmmingup2_application_trimmed -> computer_graphics_warmmingup2_application_cpp_string edges=2 relations=['references'] locations=['L13'] contexts=['parameter_type', 'return_type']
  - computer_graphics_warmmingup2_application_parserectangle -> computer_graphics_warmmingup2_board_rectangle edges=2 relations=['calls', 'references'] locations=['L25', 'L43'] contexts=['call', 'parameter_type']
  - computer_graphics_warmmingup2_application_utf8filename -> computer_graphics_warmmingup2_application_cpp_string edges=2 relations=['calls', 'references'] locations=['L75', 'L77'] contexts=['call', 'return_type']
  - computer_graphics_warmmingup2_board_rectangle -> computer_graphics_warmmingup2_board_rectangle_position edges=2 relations=['defines', 'method'] locations=['L40', 'L48'] contexts=['', 'field']
note: normal graph.json is post-build; raw producer loss must be measured earlier.
Cross-program calls marked AMBIGUOUS: 2. Source anchors: 496.
