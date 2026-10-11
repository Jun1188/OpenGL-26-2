[graphify] MultiDiGraph edge-collapse diagnostic
input: <in-memory>
input_stage: provided JSON (normal graph.json is post-build)
effective_directed: <direct-call>
nodes: 2999
unverified_code_nodes: 0
raw_edges: 5550
valid_candidate_edges: 5266
missing_endpoint_edges: 0
dangling_endpoint_edges: 284
self_loop_edges: 0
exact_duplicate_edges: 1
directed_unique_endpoint_pairs: 5132
directed_same_endpoint_collapsed_edges: 134
undirected_unique_endpoint_pairs: 5115
undirected_same_endpoint_collapsed_edges: 151
same_endpoint_group_count: 115
relation_variant_groups: 35
source_file_variant_groups: 0
source_location_variant_groups: 37
context_variant_groups: 62
post_build_graph_type: DiGraph
post_build_edges: 5132
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
  - audit_practice15_console_applyfacemotion -> opengl_3dassign_15_console_rotatearound edges=9 relations=['calls'] locations=['L170', 'L173', 'L177', 'L178', 'L185', 'L191', 'L192', 'L197', 'L202'] contexts=['']
  - audit_practice15_console_onkey -> audit_practice15_console_beginmotion edges=8 relations=['calls'] locations=['L231', 'L232', 'L233', 'L234', 'L235', 'L236', 'L237', 'L238'] contexts=['source-checked call']
  - audit_practice13_console_onkey -> audit_practice13_console_selectfaces edges=5 relations=['calls'] locations=['L60', 'L62', 'L64', 'L71', 'L74'] contexts=['source-checked call']
  - framework_include_modelloader_objindex -> framework_include_modelloader_h_size_t edges=3 relations=['calls', 'references'] locations=['L61', 'L68'] contexts=['call', 'parameter_type', 'return_type']
  - audit_practice13_console_initscene -> audit_practice13_scene_addline edges=3 relations=['calls'] locations=['L21', 'L22', 'L23'] contexts=['source-checked call']
note: normal graph.json is post-build; raw producer loss must be measured earlier.

Cross-project call edges marked AMBIGUOUS: 1.
AST is a static map; callbacks, preprocessing, and C++ overloads require source review.
빌드·실행 테스트는 수행하지 않았다.
