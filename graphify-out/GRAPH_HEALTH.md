[graphify] MultiDiGraph edge-collapse diagnostic
input: <in-memory>
input_stage: provided JSON (normal graph.json is post-build)
effective_directed: <direct-call>
nodes: 4233
unverified_code_nodes: 0
raw_edges: 7472
valid_candidate_edges: 7175
missing_endpoint_edges: 0
dangling_endpoint_edges: 297
self_loop_edges: 0
exact_duplicate_edges: 0
directed_unique_endpoint_pairs: 7021
directed_same_endpoint_collapsed_edges: 154
undirected_unique_endpoint_pairs: 7009
undirected_same_endpoint_collapsed_edges: 166
same_endpoint_group_count: 146
relation_variant_groups: 63
source_file_variant_groups: 0
source_location_variant_groups: 25
context_variant_groups: 90
post_build_graph_type: DiGraph
post_build_edges: 7000
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
  - framework_detail_objloader_objresolve -> framework_detail_objloader_cpp_size_t edges=3 relations=['calls', 'references'] locations=['L77', 'L83'] contexts=['call', 'parameter_type', 'return_type']
  - framework_detail_objloader_objtext -> framework_detail_objloader_cpp_string edges=3 relations=['references'] locations=['L113'] contexts=['generic_arg', 'parameter_type', 'return_type']
  - framework_detail_patterns_patternsystem_snap -> framework_detail_patterns_cpp_vec3 edges=3 relations=['calls', 'references'] locations=['L735', 'L743'] contexts=['call', 'parameter_type', 'return_type']
  - framework_detail_patterns_patternsystem_screenonplane -> framework_detail_patterns_cpp_vec3 edges=3 relations=['calls', 'references'] locations=['L754', 'L777'] contexts=['call', 'generic_arg', 'parameter_type']
  - framework_detail_transform_motionunitvector -> framework_detail_transform_cpp_vec3 edges=3 relations=['calls', 'references'] locations=['L16', 'L7'] contexts=['call', 'generic_arg', 'parameter_type']
note: normal graph.json is post-build; raw producer loss must be measured earlier.
Cross-program calls marked AMBIGUOUS: 3. Source anchors: 664.
