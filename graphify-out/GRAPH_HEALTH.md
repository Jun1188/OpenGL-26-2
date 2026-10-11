# Graphify 정적 그래프 진단

[graphify] MultiDiGraph edge-collapse diagnostic
input: <in-memory>
input_stage: provided JSON (normal graph.json is post-build)
effective_directed: <direct-call>
nodes: 2685
unverified_code_nodes: 0
raw_edges: 5080
valid_candidate_edges: 4810
missing_endpoint_edges: 0
dangling_endpoint_edges: 270
self_loop_edges: 3
exact_duplicate_edges: 0
directed_unique_endpoint_pairs: 4717
directed_same_endpoint_collapsed_edges: 93
undirected_unique_endpoint_pairs: 4703
undirected_same_endpoint_collapsed_edges: 107
same_endpoint_group_count: 93
relation_variant_groups: 32
source_file_variant_groups: 0
source_location_variant_groups: 18
context_variant_groups: 61
post_build_graph_type: DiGraph
post_build_edges: 4717
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
  - computer_graphics_warmmingup2_application_trimmed -> computer_graphics_warmmingup2_application_cpp_string edges=2 relations=['references'] locations=['L13'] contexts=['parameter_type', 'return_type']
  - computer_graphics_warmmingup2_application_parserectangle -> computer_graphics_warmmingup2_board_rectangle edges=2 relations=['calls', 'references'] locations=['L25', 'L43'] contexts=['call', 'parameter_type']
  - computer_graphics_warmmingup2_application_utf8filename -> computer_graphics_warmmingup2_application_cpp_string edges=2 relations=['calls', 'references'] locations=['L75', 'L77'] contexts=['call', 'return_type']
  - computer_graphics_warmmingup2_board_rectangle -> computer_graphics_warmmingup2_board_rectangle_position edges=2 relations=['defines', 'method'] locations=['L40', 'L48'] contexts=['', 'field']
  - computer_graphics_warmmingup2_board_rectangle -> computer_graphics_warmmingup2_board_rectangle_size edges=2 relations=['defines', 'method'] locations=['L41', 'L49'] contexts=['', 'field']
note: normal graph.json is post-build; raw producer loss must be measured earlier.

실제 빌드와 테스트는 실행하지 않았다.
