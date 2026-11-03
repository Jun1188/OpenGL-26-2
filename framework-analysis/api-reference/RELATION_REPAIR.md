# 병렬 관계 복원

## 원인과 적용 범위

NetworkX DiGraph는 방향 노드 쌍당 간선 하나만 저장한다. 원본 134쌍에 2~3개씩 있던 관계 종류·소스 위치·문맥이 합쳐져 추가 관계 150개가 출력에서 구분되지 않았다.

기존 그래프를 MultiDiGraph로 바꾸고 해당 134쌍만 원본 레코드와 고유 key로 교체했다. 노드 3,528개는 유지하고 간선은 5,357개에서 5,507개로 늘었다. 원본 추출·기존 커뮤니티·나머지 단일 간선은 유지했다. 정적 데이터 대조에서 기존 노드 차이 0건, 비대상 단일 간선 차이 0건, 복원 레코드 차이 0건이다.

현재 출력의 대상 관계 병합 손실은 0건이다. 원본 진단의 DiGraph 투영 시 병합 예상 150건과 현재 MultiDiGraph를 구분한다. 원본 전체 추출 노드의 기존 정규화는 이번에 변경하지 않았다. 호출의 정확한 오버로드 해석을 새로 증명한 것은 아니다.

## 사용자 확인 범위

빌드·테스트·브라우저 실행은 하지 않았다. 병렬 간선의 곡선·문맥 표시·커서 설명·선택 상세는 사용자가 확인한다. 외부 vis-network 로딩이 필요하다.

## 대상 쌍별 전체 before / after

### framework_detail_animation_animationsystem_addhumanoid → framework_detail_animation_cpp_optional

#### before
```json
{
  "source": "framework_detail_animation_animationsystem_addhumanoid",
  "target": "framework_detail_animation_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Animation.cpp",
  "source_location": "L83",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_animation_animationsystem_addhumanoid",
    "target": "framework_detail_animation_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 22,
    "key": "relation-4659ab2cc6c8b3f053ef3f1246abb274014e977bad6cd7cfb2937d6bf1794f18",
    "source_file": "framework/detail/Animation.cpp",
    "source_location": "L83",
    "weight": 1.0
  },
  {
    "source": "framework_detail_animation_animationsystem_addhumanoid",
    "target": "framework_detail_animation_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 18,
    "key": "relation-1c78f62bc24b8a5b10034d16e999fd1ee211b2beb2df04de0ca2f64fc538aa60",
    "source_file": "framework/detail/Animation.cpp",
    "source_location": "L83",
    "weight": 1.0
  }
]
```

### framework_detail_animation_animationsystem_addswingwithrider → framework_detail_animation_cpp_optional

#### before
```json
{
  "source": "framework_detail_animation_animationsystem_addswingwithrider",
  "target": "framework_detail_animation_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Animation.cpp",
  "source_location": "L234",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_animation_animationsystem_addswingwithrider",
    "target": "framework_detail_animation_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 33,
    "key": "relation-2b50b06a1d24324a7aef1455b6ed43ca0d609af698c832b8108d3b0c234b2053",
    "source_file": "framework/detail/Animation.cpp",
    "source_location": "L234",
    "weight": 1.0
  },
  {
    "source": "framework_detail_animation_animationsystem_addswingwithrider",
    "target": "framework_detail_animation_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 29,
    "key": "relation-7524af1ed37f3d42c12dd861ab4901fafaf94dbeeab94c14b23a4d8d915d7472",
    "source_file": "framework/detail/Animation.cpp",
    "source_location": "L234",
    "weight": 1.0
  }
]
```

### framework_detail_cuboidmode_lookup → framework_detail_cuboidmode_cpp_string

#### before
```json
{
  "source": "framework_detail_cuboidmode_lookup",
  "target": "framework_detail_cuboidmode_cpp_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/CuboidMode.cpp",
  "source_location": "L25",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_cuboidmode_lookup",
    "target": "framework_detail_cuboidmode_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 92,
    "key": "relation-d997f5d3a53f64b1c89b967788631af673c605ac15a6b41e70718ba49a86c49e",
    "source_file": "framework/detail/CuboidMode.cpp",
    "source_location": "L25",
    "weight": 1.0
  },
  {
    "source": "framework_detail_cuboidmode_lookup",
    "target": "framework_detail_cuboidmode_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 93,
    "key": "relation-8ae40e0fcc7e4aed70bb00c8b44975a6d533a5a1591693fe2f43cbbd6302c99c",
    "source_file": "framework/detail/CuboidMode.cpp",
    "source_location": "L25",
    "weight": 1.0
  }
]
```

### framework_detail_motion_motionsystem_pushlayer → framework_detail_motion_cpp_layerid

#### before
```json
{
  "source": "framework_detail_motion_motionsystem_pushlayer",
  "target": "framework_detail_motion_cpp_layerid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Motion.cpp",
  "source_location": "L452",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_motion_motionsystem_pushlayer",
    "target": "framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 213,
    "key": "relation-781df02240b5363f620a43d19e52880f5090bf6732553b882d918f9d8b7905e4",
    "source_file": "framework/detail/Motion.cpp",
    "source_location": "L452",
    "weight": 1.0
  },
  {
    "source": "framework_detail_motion_motionsystem_pushlayer",
    "target": "framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 214,
    "key": "relation-c7ccb6fb2756575159be35c75a946206b4d6b0fd92eac320f94c38653b00b3e3",
    "source_file": "framework/detail/Motion.cpp",
    "source_location": "L452",
    "weight": 1.0
  }
]
```

### framework_detail_motion_motionsystem_insertlayer → framework_detail_motion_cpp_layerid

#### before
```json
{
  "source": "framework_detail_motion_motionsystem_insertlayer",
  "target": "framework_detail_motion_cpp_layerid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Motion.cpp",
  "source_location": "L472",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_motion_motionsystem_insertlayer",
    "target": "framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 218,
    "key": "relation-bfad5070c813d64023c183c535aaa3dd8e0572c5a7770068be89583aed4201bb",
    "source_file": "framework/detail/Motion.cpp",
    "source_location": "L472",
    "weight": 1.0
  },
  {
    "source": "framework_detail_motion_motionsystem_insertlayer",
    "target": "framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 219,
    "key": "relation-89575c6aca573fbeed51bcb0b16ce2c68762382cf5b54cc3e1ebcbfb86be19ca",
    "source_file": "framework/detail/Motion.cpp",
    "source_location": "L472",
    "weight": 1.0
  }
]
```

### framework_detail_motion_motionsystem_poplayer → framework_detail_motion_cpp_layerid

#### before
```json
{
  "source": "framework_detail_motion_motionsystem_poplayer",
  "target": "framework_detail_motion_cpp_layerid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Motion.cpp",
  "source_location": "L525",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_motion_motionsystem_poplayer",
    "target": "framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 225,
    "key": "relation-acad4a2abf30376964a891853b8425bd9b154e6dbab301e6a8d878b1e13587ac",
    "source_file": "framework/detail/Motion.cpp",
    "source_location": "L525",
    "weight": 1.0
  },
  {
    "source": "framework_detail_motion_motionsystem_poplayer",
    "target": "framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 226,
    "key": "relation-1742063b3c7c019b9f71703c2cd963848823f3da01713cc400a6b6e3afc37181",
    "source_file": "framework/detail/Motion.cpp",
    "source_location": "L525",
    "weight": 1.0
  }
]
```

### framework_detail_native_relativepathinside → framework_detail_native_h_path

#### before
```json
{
  "source": "framework_detail_native_relativepathinside",
  "target": "framework_detail_native_h_path",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Native.h",
  "source_location": "L36",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_native_relativepathinside",
    "target": "framework_detail_native_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 263,
    "key": "relation-6e305ba391a83fa115112b9e8e3d4ba80b3a14dc1a36f7cb99c612eaa38596d5",
    "source_file": "framework/detail/Native.h",
    "source_location": "L36",
    "weight": 1.0
  },
  {
    "source": "framework_detail_native_relativepathinside",
    "target": "framework_detail_native_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 262,
    "key": "relation-c6a81f820b75df64b7badd6899a6e11f898d13530de287f5422d30ceb6f48374",
    "source_file": "framework/detail/Native.h",
    "source_location": "L36",
    "weight": 1.0
  }
]
```

### framework_detail_objloader_objwarning → framework_detail_objloader_cpp_string

#### before
```json
{
  "source": "framework_detail_objloader_objwarning",
  "target": "framework_detail_objloader_cpp_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/ObjLoader.cpp",
  "source_location": "L28",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_objloader_objwarning",
    "target": "framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 432,
    "key": "relation-c22747eae6aed810cf868ed886f2675454853157a805c88e32a7e821fd752239",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L28",
    "weight": 1.0
  },
  {
    "source": "framework_detail_objloader_objwarning",
    "target": "framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 433,
    "key": "relation-e0f592de28f9c3c87f793a5a7644ab8e24aa6c36bdc1a09d79f4300d0ff45939",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L28",
    "weight": 1.0
  }
]
```

### framework_detail_objloader_objresolve → framework_detail_objloader_cpp_size_t

#### before
```json
{
  "source": "framework_detail_objloader_objresolve",
  "target": "framework_detail_objloader_cpp_size_t",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/ObjLoader.cpp",
  "source_location": "L83",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_objloader_objresolve",
    "target": "framework_detail_objloader_cpp_size_t",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 462,
    "key": "relation-92500f0cfe2698d8f637261c512338953d54a4a1947b58b46f2b3776d759148d",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L83",
    "weight": 1.0
  },
  {
    "source": "framework_detail_objloader_objresolve",
    "target": "framework_detail_objloader_cpp_size_t",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 439,
    "key": "relation-19830b0dafa09e1fdad37311fa2e66b79459c1af537044cd14e72a3e79fff49d",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L77",
    "weight": 1.0
  },
  {
    "source": "framework_detail_objloader_objresolve",
    "target": "framework_detail_objloader_cpp_size_t",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 438,
    "key": "relation-1c92dbdaef8028db9940f6beffa39f418b3a611f8f28d16d6d18388daad55eda",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L77",
    "weight": 1.0
  }
]
```

### framework_detail_objloader_objtext → framework_detail_objloader_cpp_string

#### before
```json
{
  "source": "framework_detail_objloader_objtext",
  "target": "framework_detail_objloader_cpp_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/detail/ObjLoader.cpp",
  "source_location": "L113",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_objloader_objtext",
    "target": "framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 447,
    "key": "relation-e6fc23d44c6a1a9ed8dab65a97bfb12bc6497c60969467149bd6e994912d7568",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L113",
    "weight": 1.0
  },
  {
    "source": "framework_detail_objloader_objtext",
    "target": "framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 445,
    "key": "relation-f227c3a80628a4161a29c3a2b858093f5053e1951cea1f59744342395a8984e7",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L113",
    "weight": 1.0
  },
  {
    "source": "framework_detail_objloader_objtext",
    "target": "framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 444,
    "key": "relation-1b0d67f8380cfcf5c2e8bedfba58cf9ee441e9c04a2f3cacdcf3181a42ed901a",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L113",
    "weight": 1.0
  }
]
```

### framework_detail_objloader_parseobj → framework_detail_objloader_cpp_string

#### before
```json
{
  "source": "framework_detail_objloader_parseobj",
  "target": "framework_detail_objloader_cpp_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/detail/ObjLoader.cpp",
  "source_location": "L189",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_objloader_parseobj",
    "target": "framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 453,
    "key": "relation-629754e07a3a74024841275ef9db17efcf1fe8e7c2125ad311d12549cd5b0823",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L189",
    "weight": 1.0
  },
  {
    "source": "framework_detail_objloader_parseobj",
    "target": "framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 450,
    "key": "relation-d3f32711e5a5c50b2076d94f53749a1ee2a2a446ed30f71a72bd3ae63ef52461",
    "source_file": "framework/detail/ObjLoader.cpp",
    "source_location": "L189",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_conveyorcornerpose → framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_conveyorcornerpose",
  "target": "framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L28",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_conveyorcornerpose",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 626,
    "key": "relation-2e019be5582bdb04f61f1044cc3b8fc8c48c44d22f537db02826c9e9bd990840",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L28",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_conveyorcornerpose",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 481,
    "key": "relation-b4f38e85738888fc2801af0d7b05bc47b1591db1dc2a89b6c9c2036ceb0c8148",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L7",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_collisionsystem_steproombody → framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "framework_detail_patterns_collisionsystem_steproombody",
  "target": "framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L203",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_collisionsystem_steproombody",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 631,
    "key": "relation-04772b1ed0357db825609939b9453dc2f17a5f6d56da1b95a50ad25ce57aa438",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L203",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_collisionsystem_steproombody",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 505,
    "key": "relation-d8c48b5bde7e25eeeedb1e6d3da419e33666bb2fecd4d2a812b2aec044283667",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L166",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_collisionsystem_stepcharacter → framework_detail_patterns_cpp_bounds3

#### before
```json
{
  "source": "framework_detail_patterns_collisionsystem_stepcharacter",
  "target": "framework_detail_patterns_cpp_bounds3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L268",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_collisionsystem_stepcharacter",
    "target": "framework_detail_patterns_cpp_bounds3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 514,
    "key": "relation-b3f9c392338992518d22319209271ac161e75a137e7b25920d53a2ca4a900194",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L268",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_collisionsystem_stepcharacter",
    "target": "framework_detail_patterns_cpp_bounds3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 512,
    "key": "relation-b7a4c78e30e5f06b8a72d0363f241ed6a81a26577927fd72902055ffc4b18e4e",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L268",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_collisionsystem_randomfreebox → framework_detail_patterns_cpp_bounds3

#### before
```json
{
  "source": "framework_detail_patterns_collisionsystem_randomfreebox",
  "target": "framework_detail_patterns_cpp_bounds3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L394",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_collisionsystem_randomfreebox",
    "target": "framework_detail_patterns_cpp_bounds3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 517,
    "key": "relation-c931b229f309be8f2134fcaba1b72633a970f31e7e2107a9de6a87d05bc55469",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L394",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_collisionsystem_randomfreebox",
    "target": "framework_detail_patterns_cpp_bounds3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 518,
    "key": "relation-5c6821d1cad24d23ea1871c64035ec0bbc1f50a7ab51e30d8ba3fb7d258836b9",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L394",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_addbox → framework_detail_patterns_cpp_optional

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_addbox",
  "target": "framework_detail_patterns_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L434",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_addbox",
    "target": "framework_detail_patterns_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 528,
    "key": "relation-b5341527721959b20fd23003eddf646c612d15a2a1bb9b2a113e2d71e38a17c5",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L434",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_addbox",
    "target": "framework_detail_patterns_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 523,
    "key": "relation-b270745a19fd04450c2cb21c565ec32bf9c24a10011ca391f5d4c689f2de52c7",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L434",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_addpolygon → framework_detail_patterns_cpp_optional

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_addpolygon",
  "target": "framework_detail_patterns_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L461",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_addpolygon",
    "target": "framework_detail_patterns_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 536,
    "key": "relation-760cf6551d98d87b84eb0dbbb856bcb743877e5b3e83420a5f50fc5a5e24c84d",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L461",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_addpolygon",
    "target": "framework_detail_patterns_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 530,
    "key": "relation-9ea100f6b60b1d779f2b6e13810f8989dd67506c0ef10564803acb0b7ffbb200",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L461",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_addjoint → framework_detail_patterns_cpp_entityid

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_addjoint",
  "target": "framework_detail_patterns_cpp_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L517",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_addjoint",
    "target": "framework_detail_patterns_cpp_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 544,
    "key": "relation-ca6284d6594ae77a0c98818046f6e99812e56c967dabcf8358a75c56834ed2e9",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L517",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_addjoint",
    "target": "framework_detail_patterns_cpp_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 546,
    "key": "relation-e1d040d0dd8b81f319362a91a60ad30ad4a72ac0c5da3e490cd33ee4bc460d52",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L517",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_follow → framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_follow",
  "target": "framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L566",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_follow",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 635,
    "key": "relation-13a8597d382cfdb733e33707d2bf26b1829a68dd04d5d99e9af70aefcc95e818",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L566",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_follow",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 556,
    "key": "relation-66f53fe5d46b13d9fe01dc6f4f6dcc9b5d0caafe2bee9d101e12068901b94e8f",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L552",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_addnumber → framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_addnumber",
  "target": "framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L585",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_addnumber",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 636,
    "key": "relation-0a67d85adfd8223a51dfd65be3bace21ce47cca69d26630a615ecdb38ab0ce58",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L585",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_addnumber",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 559,
    "key": "relation-63d13468ebcc3ae786b4eef625e8d517b829e849abe61de01fd85bfccac67c17",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L571",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_snakecells → framework_detail_patterns_cpp_ivec3

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_snakecells",
  "target": "framework_detail_patterns_cpp_ivec3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L710",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_snakecells",
    "target": "framework_detail_patterns_cpp_ivec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 590,
    "key": "relation-c16d498650c549842e2dff1a8bfc6b66b093b67e22c0c5f963c1f0d5aca14e12",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L710",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_snakecells",
    "target": "framework_detail_patterns_cpp_ivec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 591,
    "key": "relation-de9b49fad76471298ccf83b811b62bdd054cb2da883bdb89fd0e0c00f9b10713",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L710",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_snap → framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_snap",
  "target": "framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L743",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_snap",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 640,
    "key": "relation-5e2ccc7ca5630168ef174c2bb654d071830383863208b3fd4de4f8869f8f3e55",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L743",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_snap",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 595,
    "key": "relation-1b67a672ccb54c890267c5908e8f218f147fec35b3d0927a7abd61621ffc13ae",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L735",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_snap",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 594,
    "key": "relation-a586a71f69184feccdf1dd28655fa4f510c02239bff5a40ba2d7ba460ba5260f",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L735",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_screenonplane → framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_screenonplane",
  "target": "framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L777",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_screenonplane",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 641,
    "key": "relation-1a684ec4007b6444326a638d22d66d46c05e2d9c27d527a855685fccfd433059",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L777",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_screenonplane",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 598,
    "key": "relation-53ce11b8406a98fc4932ff84d4ee650d5c7dc88a5497ddffbe15474b640ddd20",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L754",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_screenonplane",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 601,
    "key": "relation-ce9e0c18d5cc1b209c223946a5476938fcabfce7102fb7076b7f7c97cd9b0a74",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L754",
    "weight": 1.0
  }
]
```

### framework_detail_patterns_patternsystem_posewalkingrig → framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "framework_detail_patterns_patternsystem_posewalkingrig",
  "target": "framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Patterns.cpp",
  "source_location": "L980",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_patterns_patternsystem_posewalkingrig",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 648,
    "key": "relation-9b54c10a5e448a534c240a6104ce38fd89268d4a1f9f901f459b47961780222d",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L980",
    "weight": 1.0
  },
  {
    "source": "framework_detail_patterns_patternsystem_posewalkingrig",
    "target": "framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 625,
    "key": "relation-8fb5f0eca9825d48a36deaf9de5588c5bede1485950c700d6fd561135300c4d0",
    "source_file": "framework/detail/Patterns.cpp",
    "source_location": "L954",
    "weight": 1.0
  }
]
```

### framework_detail_scene_scene_setparent → framework_detail_scene_cpp_entityid

#### before
```json
{
  "source": "framework_detail_scene_scene_setparent",
  "target": "framework_detail_scene_cpp_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/detail/Scene.cpp",
  "source_location": "L309",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_scene_scene_setparent",
    "target": "framework_detail_scene_cpp_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 786,
    "key": "relation-f04e260c18ea60bc2afd1d66e47f685713013636c107008d881356de30f04af0",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L309",
    "weight": 1.0
  },
  {
    "source": "framework_detail_scene_scene_setparent",
    "target": "framework_detail_scene_cpp_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 784,
    "key": "relation-09709d52384f11aa43b9bb311a1c62199b59c8f8571491a5785d1c8ae9970505",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L309",
    "weight": 1.0
  }
]
```

### framework_detail_scene_scene_faceworldmatrix → framework_detail_scene_cpp_mat4

#### before
```json
{
  "source": "framework_detail_scene_scene_faceworldmatrix",
  "target": "framework_detail_scene_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Scene.cpp",
  "source_location": "L408",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_scene_scene_faceworldmatrix",
    "target": "framework_detail_scene_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 954,
    "key": "relation-66d340dbc8fec27135e903595fb85f114ee001e0fe2f48deea1c417bb96e3fc7",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L408",
    "weight": 1.0
  },
  {
    "source": "framework_detail_scene_scene_faceworldmatrix",
    "target": "framework_detail_scene_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 793,
    "key": "relation-8272eb5aed9c7cf21df9189488bae382eabd860b1a4686ba498c7a000db5a089",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L397",
    "weight": 1.0
  }
]
```

### framework_detail_scene_scene_addline → framework_detail_scene_cpp_optional

#### before
```json
{
  "source": "framework_detail_scene_scene_addline",
  "target": "framework_detail_scene_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Scene.cpp",
  "source_location": "L534",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_scene_scene_addline",
    "target": "framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 825,
    "key": "relation-57b732e7fbc8679e395178dfde34df77e04884dfdc6775fd8f36a3df810ef6a9",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L534",
    "weight": 1.0
  },
  {
    "source": "framework_detail_scene_scene_addline",
    "target": "framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 821,
    "key": "relation-3d6c374b861ce5b54a7da6d89d832f8183a0eb89f5718778da596f120a3502a5",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L534",
    "weight": 1.0
  }
]
```

### framework_detail_scene_scene_addvector → framework_detail_scene_cpp_optional

#### before
```json
{
  "source": "framework_detail_scene_scene_addvector",
  "target": "framework_detail_scene_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Scene.cpp",
  "source_location": "L554",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_scene_scene_addvector",
    "target": "framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 832,
    "key": "relation-5de245bbca0f83945e4066584f4510eb0d8b9633435984d6c89301d9bf5e3bc4",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L554",
    "weight": 1.0
  },
  {
    "source": "framework_detail_scene_scene_addvector",
    "target": "framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 828,
    "key": "relation-c583fa8027571d380cf9a2af7796de94dfdd0280056ba68d689f329da87fb773",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L554",
    "weight": 1.0
  }
]
```

### framework_detail_scene_scene_addbasis → framework_detail_scene_cpp_optional

#### before
```json
{
  "source": "framework_detail_scene_scene_addbasis",
  "target": "framework_detail_scene_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/detail/Scene.cpp",
  "source_location": "L559",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_scene_scene_addbasis",
    "target": "framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 840,
    "key": "relation-d89556f8c899c55851732458436be455e2ed73e0932383c9ea585b637274b42c",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L559",
    "weight": 1.0
  },
  {
    "source": "framework_detail_scene_scene_addbasis",
    "target": "framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 835,
    "key": "relation-6517a8fd1853955b98e27e1397664a1c469cb9eb81ef97691b04c73ea475e7c8",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L559",
    "weight": 1.0
  }
]
```

### framework_detail_scene_diagramsystem_makepolyhedron → framework_detail_scene_cpp_vec3

#### before
```json
{
  "source": "framework_detail_scene_diagramsystem_makepolyhedron",
  "target": "framework_detail_scene_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Scene.cpp",
  "source_location": "L687",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_scene_diagramsystem_makepolyhedron",
    "target": "framework_detail_scene_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 964,
    "key": "relation-8252fcb3281bddc2a7e75088a8de7bd942085ffe4935215b12c0c1108623d7f7",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L687",
    "weight": 1.0
  },
  {
    "source": "framework_detail_scene_diagramsystem_makepolyhedron",
    "target": "framework_detail_scene_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 853,
    "key": "relation-2abf1923ea3e2b3ccc55006a6231e8dd367df26816819c44b4b88f5a116db8e3",
    "source_file": "framework/detail/Scene.cpp",
    "source_location": "L646",
    "weight": 1.0
  }
]
```

### framework_detail_transform_motionunitvector → framework_detail_transform_cpp_vec3

#### before
```json
{
  "source": "framework_detail_transform_motionunitvector",
  "target": "framework_detail_transform_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L16",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_motionunitvector",
    "target": "framework_detail_transform_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1027,
    "key": "relation-4657123a48c7ba042a5303b6eae26bd45af9a4a88b27c0e5e3a77af6ff22b589",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L16",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_motionunitvector",
    "target": "framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 973,
    "key": "relation-5093471af2d0e7baf82cee3c7f8bad5bbe1817cd5d1bbd4ddb138be8f28a2aee",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L7",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_motionunitvector",
    "target": "framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 974,
    "key": "relation-e90a287adbb6e327aae37bbb735c5a2d97c9e3a4e2db9d30dea13926d6263c2e",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L7",
    "weight": 1.0
  }
]
```

### framework_detail_transform_motionaround → framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "framework_detail_transform_motionaround",
  "target": "framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L23",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_motionaround",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1028,
    "key": "relation-097fed34b985213849d1e4d879a6979a6d7c45ff76000c08f16ae93658206a00",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L23",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_motionaround",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 977,
    "key": "relation-b8a7fee25c37e72b4d8ba4f051fc7c8ae29b4e6632cb7462113a2b0bac9a9abb",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L19",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_motionaround",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 979,
    "key": "relation-d69dc54bfee2a412c778fcf7a012ccf5ea0fc6a77e1b54bc8e8544d1f4ecfcff",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L19",
    "weight": 1.0
  }
]
```

### framework_detail_transform_transformsystem_translation → framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "framework_detail_transform_transformsystem_translation",
  "target": "framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L47",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_transformsystem_translation",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1030,
    "key": "relation-ecb5a683b4167e193d31681cac67813b7c59d5884fbd0127ce6c13db33530ca2",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L47",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_transformsystem_translation",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 986,
    "key": "relation-70ced4653b8a9220aa31bb7c05f7f80932d7e62ae16cc8c31e96fae0ecaf94ec",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L39",
    "weight": 1.0
  }
]
```

### framework_detail_transform_transformsystem_scaling → framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "framework_detail_transform_transformsystem_scaling",
  "target": "framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L58",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_transformsystem_scaling",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1032,
    "key": "relation-1318ca0cba725799b37825ac743147033f8add5013180102206181d5dc45f746",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L58",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_transformsystem_scaling",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 990,
    "key": "relation-f127316fac469bf3b95cead40262a58cba052f88a9cc8d43a175c2efd3af3880",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L50",
    "weight": 1.0
  }
]
```

### framework_detail_transform_transformsystem_rotation → framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "framework_detail_transform_transformsystem_rotation",
  "target": "framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L73",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_transformsystem_rotation",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1035,
    "key": "relation-53634f7e823b432a8238e12918675d5434fcaeeedc77316d49b910a24cd57716",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L73",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_transformsystem_rotation",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 994,
    "key": "relation-f468c1ab62b9a27f7cf57c4953d4c370de32981c69634baa135540dc5d5a93e2",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L61",
    "weight": 1.0
  }
]
```

### framework_detail_transform_transformsystem_reflectpoint → framework_detail_transform_cpp_vec3

#### before
```json
{
  "source": "framework_detail_transform_transformsystem_reflectpoint",
  "target": "framework_detail_transform_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L77",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_transformsystem_reflectpoint",
    "target": "framework_detail_transform_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1036,
    "key": "relation-004801197533670afbf632f3229bfb01e5f3b9a63d5bcf73fb9fd4ad39a50c85",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L77",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_transformsystem_reflectpoint",
    "target": "framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 999,
    "key": "relation-afb337740542e1780ae7a7e16d549aef492582200000d8a2d1b1c26980ff1ff8",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L76",
    "weight": 1.0
  }
]
```

### framework_detail_transform_transformsystem_reflectline → framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "framework_detail_transform_transformsystem_reflectline",
  "target": "framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L96",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_transformsystem_reflectline",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1039,
    "key": "relation-9a73eb19e88aad844a635dd96baaf5c90bfda9b738baa21d7a59342ba1a86c9a",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L96",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_transformsystem_reflectline",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1002,
    "key": "relation-8f123786987da09556c882fe510b794ec9b9e56455894cec22e9203df8f851a1",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L80",
    "weight": 1.0
  }
]
```

### framework_detail_transform_transformsystem_reflectplane → framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "framework_detail_transform_transformsystem_reflectplane",
  "target": "framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L115",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_transformsystem_reflectplane",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1042,
    "key": "relation-717efd39385000b4a2dc90b88b5f412db0b30a756590e3a96ee2fb7aea9f21e1",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L115",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_transformsystem_reflectplane",
    "target": "framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1006,
    "key": "relation-26113b31d5f9e37bf6527aa6a280c34972f402c3cceb15bb32daf691dbc45f05",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L99",
    "weight": 1.0
  }
]
```

### framework_detail_transform_pathsystem_circle → framework_detail_transform_cpp_vec3

#### before
```json
{
  "source": "framework_detail_transform_pathsystem_circle",
  "target": "framework_detail_transform_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L143",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_pathsystem_circle",
    "target": "framework_detail_transform_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1045,
    "key": "relation-d090a78a77c03a4d2838aefa1aea4bb9cf61ef1eb8aece99c36c8553400b02df",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L143",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_pathsystem_circle",
    "target": "framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 1013,
    "key": "relation-89bb101e307487240c7cc4ec2a2ff66ed76d2f36187459830d9fbb99ae40b1fd",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L133",
    "weight": 1.0
  }
]
```

### framework_detail_transform_pathsystem_polyline → framework_detail_transform_cpp_vec3

#### before
```json
{
  "source": "framework_detail_transform_pathsystem_polyline",
  "target": "framework_detail_transform_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/detail/Transform.cpp",
  "source_location": "L197",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_detail_transform_pathsystem_polyline",
    "target": "framework_detail_transform_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1046,
    "key": "relation-4d8e9e96735643d5bba55a00b820cd42c9ee55214958506f92abb78a6d66066c",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L197",
    "weight": 1.0
  },
  {
    "source": "framework_detail_transform_pathsystem_polyline",
    "target": "framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1017,
    "key": "relation-fb461304b173991409fd76c053fd9c04c1fab0f4e50c9e39ae9e5e84ab07b54a",
    "source_file": "framework/detail/Transform.cpp",
    "source_location": "L149",
    "weight": 1.0
  }
]
```

### framework_include_actions_sceneactions_parent → framework_include_actions_h_entityid

#### before
```json
{
  "source": "framework_include_actions_sceneactions_parent",
  "target": "framework_include_actions_h_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/include/Actions.h",
  "source_location": "L103",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_actions_sceneactions_parent",
    "target": "framework_include_actions_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1133,
    "key": "relation-2a89227655b466a8b8f6c02a8bdeadf71fdd69d03b305621147f248442000b40",
    "source_file": "framework/include/Actions.h",
    "source_location": "L103",
    "weight": 1.0
  },
  {
    "source": "framework_include_actions_sceneactions_parent",
    "target": "framework_include_actions_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 1131,
    "key": "relation-5f61a6e7ec6bbfce2a812da1836d017a5f8f47bee46fb0e6b3f42c96b689cba5",
    "source_file": "framework/include/Actions.h",
    "source_location": "L103",
    "weight": 1.0
  }
]
```

### framework_include_animation_humanoidrig → framework_include_animation_h_entityid

#### before
```json
{
  "source": "framework_include_animation_humanoidrig",
  "target": "framework_include_animation_h_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/include/Animation.h",
  "source_location": "L21",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_animation_humanoidrig",
    "target": "framework_include_animation_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 1156,
    "key": "relation-154372690833dd672ad8825067c9930e3d95d9b4842753ffe92ab3a67248255b",
    "source_file": "framework/include/Animation.h",
    "source_location": "L19",
    "weight": 1.0
  },
  {
    "source": "framework_include_animation_humanoidrig",
    "target": "framework_include_animation_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1165,
    "key": "relation-c1a7e339d7be9de35f68a3060c33302747c3add3524c394783b1e773cd98fcff",
    "source_file": "framework/include/Animation.h",
    "source_location": "L21",
    "weight": 1.0
  }
]
```

### framework_include_animation_humanoidpose → framework_include_animation_h_vec3

#### before
```json
{
  "source": "framework_include_animation_humanoidpose",
  "target": "framework_include_animation_h_vec3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/include/Animation.h",
  "source_location": "L27",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_animation_humanoidpose",
    "target": "framework_include_animation_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 1177,
    "key": "relation-320549bc010a8859859b52c72a4f35ddc7e4e9a36a1c840ae1f05469630c280b",
    "source_file": "framework/include/Animation.h",
    "source_location": "L26",
    "weight": 1.0
  },
  {
    "source": "framework_include_animation_humanoidpose",
    "target": "framework_include_animation_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1182,
    "key": "relation-2c20512fecd3322e1740655d6263fd21da73d6fcfb8e22752b04b5806545fb4f",
    "source_file": "framework/include/Animation.h",
    "source_location": "L27",
    "weight": 1.0
  }
]
```

### framework_include_animation_swingrig → framework_include_animation_h_entityid

#### before
```json
{
  "source": "framework_include_animation_swingrig",
  "target": "framework_include_animation_h_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/include/Animation.h",
  "source_location": "L47",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_animation_swingrig",
    "target": "framework_include_animation_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 1200,
    "key": "relation-77f9af3fe856d3d6ebee4865f49dc30dbbb76a78449bd29473ca43d17ed4d055",
    "source_file": "framework/include/Animation.h",
    "source_location": "L46",
    "weight": 1.0
  },
  {
    "source": "framework_include_animation_swingrig",
    "target": "framework_include_animation_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1205,
    "key": "relation-69d1e7921e48f74c725001b9ffdc4ad8dbabfb0f156901943962b16fac031f25",
    "source_file": "framework/include/Animation.h",
    "source_location": "L47",
    "weight": 1.0
  }
]
```

### framework_include_geometry_transformtexcoords → framework_include_geometry_h_vec2

#### before
```json
{
  "source": "framework_include_geometry_transformtexcoords",
  "target": "framework_include_geometry_h_vec2",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/Geometry.h",
  "source_location": "L33",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_geometry_transformtexcoords",
    "target": "framework_include_geometry_h_vec2",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1469,
    "key": "relation-f359c6508577b80f5f6841af2cea756667f8dc7d2d0ffc0450defa3a7099a654",
    "source_file": "framework/include/Geometry.h",
    "source_location": "L33",
    "weight": 1.0
  },
  {
    "source": "framework_include_geometry_transformtexcoords",
    "target": "framework_include_geometry_h_vec2",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 1449,
    "key": "relation-0affdda04411a40753cf189d4ca10d2ed9198acf910e6588bb54266e98422047",
    "source_file": "framework/include/Geometry.h",
    "source_location": "L20",
    "weight": 1.0
  }
]
```

### framework_include_geometry_triangulate → framework_include_geometry_h_vector

#### before
```json
{
  "source": "framework_include_geometry_triangulate",
  "target": "framework_include_geometry_h_vector",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/include/Geometry.h",
  "source_location": "L281",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_geometry_triangulate",
    "target": "framework_include_geometry_h_vector",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 1461,
    "key": "relation-6a81c42c08c7e986712300f7f63ad272fcf6af5913000aad96dad54c84f396d2",
    "source_file": "framework/include/Geometry.h",
    "source_location": "L281",
    "weight": 1.0
  },
  {
    "source": "framework_include_geometry_triangulate",
    "target": "framework_include_geometry_h_vector",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 1459,
    "key": "relation-f6d420996028e99ab9bb86876d88662b385fb6a8df95cdeb1ed915aa1496ee83",
    "source_file": "framework/include/Geometry.h",
    "source_location": "L281",
    "weight": 1.0
  }
]
```

### framework_include_geometry_triangulate → framework_include_geometry_h_vec3

#### before
```json
{
  "source": "framework_include_geometry_triangulate",
  "target": "framework_include_geometry_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/Geometry.h",
  "source_location": "L291",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_geometry_triangulate",
    "target": "framework_include_geometry_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1474,
    "key": "relation-884c39155641d047597df13107bde2372729e0f7d14982ca853250f8abf1d23d",
    "source_file": "framework/include/Geometry.h",
    "source_location": "L291",
    "weight": 1.0
  },
  {
    "source": "framework_include_geometry_triangulate",
    "target": "framework_include_geometry_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1462,
    "key": "relation-cedcee4b517a1237d5897cd049aa4861971d8340e995355f84a6eb5e4dfd7645",
    "source_file": "framework/include/Geometry.h",
    "source_location": "L281",
    "weight": 1.0
  }
]
```

### framework_include_graphics_renderer → framework_include_graphics_drawrange

#### before
```json
{
  "source": "framework_include_graphics_renderer",
  "target": "framework_include_graphics_drawrange",
  "relation": "contains",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "source_file": "framework/include/Graphics.h",
  "source_location": "L379",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_graphics_renderer",
    "target": "framework_include_graphics_drawrange",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1646,
    "key": "relation-29aa3051f64580e60bd186cb3ac3aed75dd945a03fc5589887f5e9bd49a6478b",
    "source_file": "framework/include/Graphics.h",
    "source_location": "L393",
    "weight": 1.0
  },
  {
    "source": "framework_include_graphics_renderer",
    "target": "framework_include_graphics_drawrange",
    "relation": "contains",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "extraction_index": 1613,
    "key": "relation-d700b475f9acad5f1da1dd2e2ced884f99e99220196f3f240d9d55441aebef02",
    "source_file": "framework/include/Graphics.h",
    "source_location": "L379",
    "weight": 1.0
  }
]
```

### framework_include_gridlayout_gridlayout_position → framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "framework_include_gridlayout_gridlayout_position",
  "target": "framework_include_gridlayout_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/GridLayout.h",
  "source_location": "L32",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_gridlayout_gridlayout_position",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1699,
    "key": "relation-e07b29d787108a482187b347d958dd2c7ff7818de19eb0063774de204dd2375f",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L32",
    "weight": 1.0
  },
  {
    "source": "framework_include_gridlayout_gridlayout_position",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1679,
    "key": "relation-f5d9adcb75704c7cb81183507c31edaeb622bb977470f2acc13b67ac06204cd7",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L23",
    "weight": 1.0
  },
  {
    "source": "framework_include_gridlayout_gridlayout_position",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 1680,
    "key": "relation-a4cffe11c7f0480816d4ee4aeb03a69581ad652cf0746d5a3cf4a89fe37872cd",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L23",
    "weight": 1.0
  }
]
```

### framework_include_gridlayout_gridlayout_cellcenter → framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "framework_include_gridlayout_gridlayout_cellcenter",
  "target": "framework_include_gridlayout_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/GridLayout.h",
  "source_location": "L55",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_gridlayout_gridlayout_cellcenter",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1702,
    "key": "relation-3ddaf0b007d857b7722812524e4a9648680f4b7a1fc2ee84ce93c7ea29b1b9c1",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L55",
    "weight": 1.0
  },
  {
    "source": "framework_include_gridlayout_gridlayout_cellcenter",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1683,
    "key": "relation-82881dee70301468b36af2f6eb4a507249007edca35f6b1f2e43634b433e60fe",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L46",
    "weight": 1.0
  }
]
```

### framework_include_gridlayout_gridlayout_relativesize → framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "framework_include_gridlayout_gridlayout_relativesize",
  "target": "framework_include_gridlayout_h_vec3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/include/GridLayout.h",
  "source_location": "L60",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_gridlayout_gridlayout_relativesize",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1687,
    "key": "relation-83ec008e8977d75252b63b8b8a17aaefa6cfb58f40f6ba06ff543d0135c65bff",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L60",
    "weight": 1.0
  },
  {
    "source": "framework_include_gridlayout_gridlayout_relativesize",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 1688,
    "key": "relation-0f38e1eccd9a400bc1aa939fa50e8801a2b2dce330a7c1c8b24238f24bc8fca1",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L60",
    "weight": 1.0
  }
]
```

### framework_include_gridlayout_gridlayout_positioninparent → framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "framework_include_gridlayout_gridlayout_positioninparent",
  "target": "framework_include_gridlayout_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/GridLayout.h",
  "source_location": "L95",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_gridlayout_gridlayout_positioninparent",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1705,
    "key": "relation-dec150177d1aa2d7ed9ca56799b8fb2f733e21a1c3bb76b1a3979c6ffacb5c65",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L95",
    "weight": 1.0
  },
  {
    "source": "framework_include_gridlayout_gridlayout_positioninparent",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1691,
    "key": "relation-a469b8e774f41d9853399c61184272962c7a5c1f4c0916b6f3f17775fa700af7",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L83",
    "weight": 1.0
  },
  {
    "source": "framework_include_gridlayout_gridlayout_positioninparent",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 1692,
    "key": "relation-a0e967085eabdd7b0ab5fb7b544756c3bcaa3eb2ce2657c504bc23fcd4db837a",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L83",
    "weight": 1.0
  }
]
```

### framework_include_gridlayout_gridlayout_cellat → framework_include_gridlayout_h_ivec3

#### before
```json
{
  "source": "framework_include_gridlayout_gridlayout_cellat",
  "target": "framework_include_gridlayout_h_ivec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/GridLayout.h",
  "source_location": "L124",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_gridlayout_gridlayout_cellat",
    "target": "framework_include_gridlayout_h_ivec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1708,
    "key": "relation-2176462f18661d1397e5bc689e538f8ebd19fd7147b7d4dd8e4ead7e41e24770",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L124",
    "weight": 1.0
  },
  {
    "source": "framework_include_gridlayout_gridlayout_cellat",
    "target": "framework_include_gridlayout_h_ivec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1696,
    "key": "relation-da9b6098b92155711b507fd7731314c4426058703b2a239dc8548e356ed90599",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L109",
    "weight": 1.0
  }
]
```

### framework_include_gridlayout_gridlayout_cellat → framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "framework_include_gridlayout_gridlayout_cellat",
  "target": "framework_include_gridlayout_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/GridLayout.h",
  "source_location": "L117",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_gridlayout_gridlayout_cellat",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1707,
    "key": "relation-c9a575ed6be4aff85d66f30409660370eb23b82435c98bb574fcb3b4f372e9d3",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L117",
    "weight": 1.0
  },
  {
    "source": "framework_include_gridlayout_gridlayout_cellat",
    "target": "framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 1697,
    "key": "relation-4d8b6a4b094ce0e46f9644df6ba42bcf5dfae77981a5aeb8eec382503376efea",
    "source_file": "framework/include/GridLayout.h",
    "source_location": "L109",
    "weight": 1.0
  }
]
```

### framework_include_model_planefigure → framework_include_model_h_vec3

#### before
```json
{
  "source": "framework_include_model_planefigure",
  "target": "framework_include_model_h_vec3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "field",
  "source_file": "framework/include/Model.h",
  "source_location": "L77",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_model_planefigure",
    "target": "framework_include_model_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 1766,
    "key": "relation-2f44bd728abe729c981e6cea4a224ecf1c985b056151ae054ba42388ca1891de",
    "source_file": "framework/include/Model.h",
    "source_location": "L77",
    "weight": 1.0
  },
  {
    "source": "framework_include_model_planefigure",
    "target": "framework_include_model_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1762,
    "key": "relation-292b19366ee9496455a3f323a12e335b6f92270252404dbca5867034404e4087",
    "source_file": "framework/include/Model.h",
    "source_location": "L75",
    "weight": 1.0
  }
]
```

### framework_include_model_planefigure → framework_include_model_glcolor

#### before
```json
{
  "source": "framework_include_model_planefigure",
  "target": "framework_include_model_glcolor",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/include/Model.h",
  "source_location": "L83",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_model_planefigure",
    "target": "framework_include_model_glcolor",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 1764,
    "key": "relation-fa0c22dd62ff1bd07cca791b9482f95ce1385f84160cfb9a70028af90438fb5f",
    "source_file": "framework/include/Model.h",
    "source_location": "L76",
    "weight": 1.0
  },
  {
    "source": "framework_include_model_planefigure",
    "target": "framework_include_model_glcolor",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1774,
    "key": "relation-c407c4e75fc03ffbf45edcdd3c077c1b374e56e20930eb5b8226f5d7e14a1dd0",
    "source_file": "framework/include/Model.h",
    "source_location": "L83",
    "weight": 1.0
  }
]
```

### framework_include_model_transformmatrix → framework_include_model_h_mat4

#### before
```json
{
  "source": "framework_include_model_transformmatrix",
  "target": "framework_include_model_h_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/Model.h",
  "source_location": "L285",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_model_transformmatrix",
    "target": "framework_include_model_h_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1878,
    "key": "relation-8a8aea77bf6e154de23974dfae106438da829738c82738fc6805a4f3dcd67ee4",
    "source_file": "framework/include/Model.h",
    "source_location": "L285",
    "weight": 1.0
  },
  {
    "source": "framework_include_model_transformmatrix",
    "target": "framework_include_model_h_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 1859,
    "key": "relation-87554137001cb7f4ffd541ecbc99979e973836ef5e1c15ed1a1f86ecccd2f679",
    "source_file": "framework/include/Model.h",
    "source_location": "L284",
    "weight": 1.0
  }
]
```

### framework_include_model_facesrtmatrix → framework_include_model_h_mat4

#### before
```json
{
  "source": "framework_include_model_facesrtmatrix",
  "target": "framework_include_model_h_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/Model.h",
  "source_location": "L295",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_model_facesrtmatrix",
    "target": "framework_include_model_h_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1880,
    "key": "relation-c2a9224bed3a6d2154d75b9bb6260d042ad0c55c62be5e537fdcf04f12e691d4",
    "source_file": "framework/include/Model.h",
    "source_location": "L295",
    "weight": 1.0
  },
  {
    "source": "framework_include_model_facesrtmatrix",
    "target": "framework_include_model_h_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 1862,
    "key": "relation-31892437d19b0c452f5ae7fc12933e18f223f2e4ab0e06f4aecbab2a7c4fdb69",
    "source_file": "framework/include/Model.h",
    "source_location": "L293",
    "weight": 1.0
  }
]
```

### framework_include_model_facematrix → framework_include_model_h_mat4

#### before
```json
{
  "source": "framework_include_model_facematrix",
  "target": "framework_include_model_h_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/Model.h",
  "source_location": "L308",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_model_facematrix",
    "target": "framework_include_model_h_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 1884,
    "key": "relation-64e97627f354fa89177f5699679cd3954d8cdb9a7e37e56ec1f4248c60e9d475",
    "source_file": "framework/include/Model.h",
    "source_location": "L308",
    "weight": 1.0
  },
  {
    "source": "framework_include_model_facematrix",
    "target": "framework_include_model_h_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 1868,
    "key": "relation-de3a1a7eccdad9a49bc8a614b4cf467a8dfb8b464a2e7fbba95f404616288be2",
    "source_file": "framework/include/Model.h",
    "source_location": "L307",
    "weight": 1.0
  }
]
```

### framework_include_modelloader_modelresult → framework_include_modelloader_h_string

#### before
```json
{
  "source": "framework_include_modelloader_modelresult",
  "target": "framework_include_modelloader_h_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/include/ModelLoader.h",
  "source_location": "L27",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_modelloader_modelresult",
    "target": "framework_include_modelloader_h_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 1910,
    "key": "relation-f568de57985495bf90394481e38d41c18d174b1ba67493f37434c51016a23978",
    "source_file": "framework/include/ModelLoader.h",
    "source_location": "L26",
    "weight": 1.0
  },
  {
    "source": "framework_include_modelloader_modelresult",
    "target": "framework_include_modelloader_h_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1913,
    "key": "relation-d7a7679b2cc628ee5e68eb73a3011b37d8b22842a5237df4c5aff96ae32ecd94",
    "source_file": "framework/include/ModelLoader.h",
    "source_location": "L27",
    "weight": 1.0
  }
]
```

### framework_include_modelloader_assetloader → framework_include_modelloader_h_path

#### before
```json
{
  "source": "framework_include_modelloader_assetloader",
  "target": "framework_include_modelloader_h_path",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/include/ModelLoader.h",
  "source_location": "L81",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_modelloader_assetloader",
    "target": "framework_include_modelloader_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 1927,
    "key": "relation-b16f2406a011a216dc02959e334e01ed82ace948ce5cdd87ac49f9898d0d6255",
    "source_file": "framework/include/ModelLoader.h",
    "source_location": "L77",
    "weight": 1.0
  },
  {
    "source": "framework_include_modelloader_assetloader",
    "target": "framework_include_modelloader_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 1932,
    "key": "relation-b0864fc5978eebf091b252bdcc8a667f4df22ce9c48c428b3c56460d521b3352",
    "source_file": "framework/include/ModelLoader.h",
    "source_location": "L81",
    "weight": 1.0
  }
]
```

### framework_include_modelloader_assetloader_resolve → framework_include_modelloader_h_path

#### before
```json
{
  "source": "framework_include_modelloader_assetloader_resolve",
  "target": "framework_include_modelloader_h_path",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "framework/include/ModelLoader.h",
  "source_location": "L84",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_modelloader_assetloader_resolve",
    "target": "framework_include_modelloader_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 1938,
    "key": "relation-14f0a7c4c5e521269a52d252f98c3f6efbd6c3917914c011de1609b7a873e98c",
    "source_file": "framework/include/ModelLoader.h",
    "source_location": "L84",
    "weight": 1.0
  },
  {
    "source": "framework_include_modelloader_assetloader_resolve",
    "target": "framework_include_modelloader_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 1937,
    "key": "relation-0ddac93658103af158cdb775ad699548ea78fba7bd6a3584eeb4f97281afe4c9",
    "source_file": "framework/include/ModelLoader.h",
    "source_location": "L84",
    "weight": 1.0
  }
]
```

### framework_include_motion_motionsystem → framework_include_motion_h_motionid

#### before
```json
{
  "source": "framework_include_motion_motionsystem",
  "target": "framework_include_motion_h_motionid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "field",
  "source_file": "framework/include/Motion.h",
  "source_location": "L161",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_motion_motionsystem",
    "target": "framework_include_motion_h_motionid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 2064,
    "key": "relation-87a89d1f8fa06e9a3460f031307605721845575833d0ba56d9e4973a780aa90b",
    "source_file": "framework/include/Motion.h",
    "source_location": "L161",
    "weight": 1.0
  },
  {
    "source": "framework_include_motion_motionsystem",
    "target": "framework_include_motion_h_motionid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2061,
    "key": "relation-087b38eeb10e20a2e1551065183fda7dcc03ba8fb14f4c620a484e72d9c29ded",
    "source_file": "framework/include/Motion.h",
    "source_location": "L160",
    "weight": 1.0
  }
]
```

### framework_include_motion_motionsystem → framework_include_motion_h_layerid

#### before
```json
{
  "source": "framework_include_motion_motionsystem",
  "target": "framework_include_motion_h_layerid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "field",
  "source_file": "framework/include/Motion.h",
  "source_location": "L163",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_motion_motionsystem",
    "target": "framework_include_motion_h_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 2069,
    "key": "relation-6a542356b391172c13155393cc1eec18b8bfffb066e0d0b452197586d70ccc1e",
    "source_file": "framework/include/Motion.h",
    "source_location": "L163",
    "weight": 1.0
  },
  {
    "source": "framework_include_motion_motionsystem",
    "target": "framework_include_motion_h_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2066,
    "key": "relation-0b1a4c144053f0428488f3f488206a8fc64a6988a52f81d016bd1ea0be582d85",
    "source_file": "framework/include/Motion.h",
    "source_location": "L162",
    "weight": 1.0
  }
]
```

### framework_include_patterns_roombody_position → framework_include_patterns_h_vec3

#### before
```json
{
  "source": "framework_include_patterns_roombody_position",
  "target": "framework_include_patterns_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "framework/include/Patterns.h",
  "source_location": "L71",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_patterns_roombody_position",
    "target": "framework_include_patterns_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 2194,
    "key": "relation-b876e08cccc511d95826c13747919c0394f56fcbe1b1f8661e31b2fa48dd3ddb",
    "source_file": "framework/include/Patterns.h",
    "source_location": "L71",
    "weight": 1.0
  },
  {
    "source": "framework_include_patterns_roombody_position",
    "target": "framework_include_patterns_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 2131,
    "key": "relation-40af3adf90211fe8bed56b9a532ad46a4d507bfd0af3e25e73cc96723398e13a",
    "source_file": "framework/include/Patterns.h",
    "source_location": "L70",
    "weight": 1.0
  }
]
```

### framework_include_patterns_jointrig → framework_include_patterns_h_entityid

#### before
```json
{
  "source": "framework_include_patterns_jointrig",
  "target": "framework_include_patterns_h_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "framework/include/Patterns.h",
  "source_location": "L114",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "framework_include_patterns_jointrig",
    "target": "framework_include_patterns_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 2154,
    "key": "relation-b70efb0047bc60617b67b533e00824e0a21ddc733742687571e3913fa4e33aff",
    "source_file": "framework/include/Patterns.h",
    "source_location": "L113",
    "weight": 1.0
  },
  {
    "source": "framework_include_patterns_jointrig",
    "target": "framework_include_patterns_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2158,
    "key": "relation-76a4a65f9ed49b9a4a5c13023f53533a353b3a03dd68b6f7fd3af5f1d5646c5e",
    "source_file": "framework/include/Patterns.h",
    "source_location": "L114",
    "weight": 1.0
  }
]
```

### standard_console_console → standard_console_h_entityid

#### before
```json
{
  "source": "standard_console_console",
  "target": "standard_console_h_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "field",
  "source_file": "standard/Console.h",
  "source_location": "L24",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_console_console",
    "target": "standard_console_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 2391,
    "key": "relation-7269752119e62badf4ad62f00e225ebccf6275168802572b959da142e692f3b8",
    "source_file": "standard/Console.h",
    "source_location": "L24",
    "weight": 1.0
  },
  {
    "source": "standard_console_console",
    "target": "standard_console_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2387,
    "key": "relation-c5ba00a5042843b4f1f700dcc3df29316cb8eef48b4383887e4a670907d131ec",
    "source_file": "standard/Console.h",
    "source_location": "L22",
    "weight": 1.0
  }
]
```

### standard_console_console → standard_console_jointcontrol

#### before
```json
{
  "source": "standard_console_console",
  "target": "standard_console_jointcontrol",
  "relation": "contains",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "source_file": "standard/Console.h",
  "source_location": "L27",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_console_console",
    "target": "standard_console_jointcontrol",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2403,
    "key": "relation-92f59a373dea38c7f2b4cad01ef4a9a64c05dfa5431b586a46210922330195ec",
    "source_file": "standard/Console.h",
    "source_location": "L33",
    "weight": 1.0
  },
  {
    "source": "standard_console_console",
    "target": "standard_console_jointcontrol",
    "relation": "contains",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "extraction_index": 2396,
    "key": "relation-c4f89ec8265c6ec32e7488a229df0f2cf76e019ce7fd9af2f0f9ac086da4ad1d",
    "source_file": "standard/Console.h",
    "source_location": "L27",
    "weight": 1.0
  }
]
```

### standard_framework_detail_animation_animationsystem_addhumanoid → standard_framework_detail_animation_cpp_optional

#### before
```json
{
  "source": "standard_framework_detail_animation_animationsystem_addhumanoid",
  "target": "standard_framework_detail_animation_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Animation.cpp",
  "source_location": "L83",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_animation_animationsystem_addhumanoid",
    "target": "standard_framework_detail_animation_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2449,
    "key": "relation-8e645e821448d7583bdf173dfb7e0870ffc5fc52cb12f188d83ede52e4a2ce56",
    "source_file": "standard/framework/detail/Animation.cpp",
    "source_location": "L83",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_animation_animationsystem_addhumanoid",
    "target": "standard_framework_detail_animation_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 2445,
    "key": "relation-0188d57370468238048f58d7f20b79f5d93504a14f94952d1e38aaebe9563061",
    "source_file": "standard/framework/detail/Animation.cpp",
    "source_location": "L83",
    "weight": 1.0
  }
]
```

### standard_framework_detail_animation_animationsystem_addswingwithrider → standard_framework_detail_animation_cpp_optional

#### before
```json
{
  "source": "standard_framework_detail_animation_animationsystem_addswingwithrider",
  "target": "standard_framework_detail_animation_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Animation.cpp",
  "source_location": "L234",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_animation_animationsystem_addswingwithrider",
    "target": "standard_framework_detail_animation_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2460,
    "key": "relation-12745dad335683ffa758a987f57e5a930397adaf5f5aa949c48dd97954635542",
    "source_file": "standard/framework/detail/Animation.cpp",
    "source_location": "L234",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_animation_animationsystem_addswingwithrider",
    "target": "standard_framework_detail_animation_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 2456,
    "key": "relation-7cffeaee8efff0efa911ef52c5811128580b9b1ef391d811c95e64ab7ea25275",
    "source_file": "standard/framework/detail/Animation.cpp",
    "source_location": "L234",
    "weight": 1.0
  }
]
```

### standard_framework_detail_cuboidmode_lookup → standard_framework_detail_cuboidmode_cpp_string

#### before
```json
{
  "source": "standard_framework_detail_cuboidmode_lookup",
  "target": "standard_framework_detail_cuboidmode_cpp_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/CuboidMode.cpp",
  "source_location": "L25",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_cuboidmode_lookup",
    "target": "standard_framework_detail_cuboidmode_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2519,
    "key": "relation-85c06056a3830516936cf27790d99e78285402e687ea855dc2663d0f9f5fa053",
    "source_file": "standard/framework/detail/CuboidMode.cpp",
    "source_location": "L25",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_cuboidmode_lookup",
    "target": "standard_framework_detail_cuboidmode_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2520,
    "key": "relation-f62e7925712820246c4fb572e895214ebb7abc1114ab2a8a16241433df9a49c3",
    "source_file": "standard/framework/detail/CuboidMode.cpp",
    "source_location": "L25",
    "weight": 1.0
  }
]
```

### standard_framework_detail_motion_motionsystem_pushlayer → standard_framework_detail_motion_cpp_layerid

#### before
```json
{
  "source": "standard_framework_detail_motion_motionsystem_pushlayer",
  "target": "standard_framework_detail_motion_cpp_layerid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Motion.cpp",
  "source_location": "L452",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_motion_motionsystem_pushlayer",
    "target": "standard_framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2640,
    "key": "relation-a73e5ea532c8f59d571bdf233bdc1b3ea7420d383e38742dbcf87f762a7969b4",
    "source_file": "standard/framework/detail/Motion.cpp",
    "source_location": "L452",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_motion_motionsystem_pushlayer",
    "target": "standard_framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2641,
    "key": "relation-9ca0c8e37a4837e6f017a5cae8b0748c6f8e00934252aac6823497f4821ae75c",
    "source_file": "standard/framework/detail/Motion.cpp",
    "source_location": "L452",
    "weight": 1.0
  }
]
```

### standard_framework_detail_motion_motionsystem_insertlayer → standard_framework_detail_motion_cpp_layerid

#### before
```json
{
  "source": "standard_framework_detail_motion_motionsystem_insertlayer",
  "target": "standard_framework_detail_motion_cpp_layerid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Motion.cpp",
  "source_location": "L472",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_motion_motionsystem_insertlayer",
    "target": "standard_framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2645,
    "key": "relation-b0a856d03a57443cc7552dea29a28e66f10e11b0c7ee80427bc20a462b4c973f",
    "source_file": "standard/framework/detail/Motion.cpp",
    "source_location": "L472",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_motion_motionsystem_insertlayer",
    "target": "standard_framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2646,
    "key": "relation-bcc27fba676dc8f36d0271d4560f4f37648532c564bb9475139dc25541dfc1f9",
    "source_file": "standard/framework/detail/Motion.cpp",
    "source_location": "L472",
    "weight": 1.0
  }
]
```

### standard_framework_detail_motion_motionsystem_poplayer → standard_framework_detail_motion_cpp_layerid

#### before
```json
{
  "source": "standard_framework_detail_motion_motionsystem_poplayer",
  "target": "standard_framework_detail_motion_cpp_layerid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Motion.cpp",
  "source_location": "L525",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_motion_motionsystem_poplayer",
    "target": "standard_framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2652,
    "key": "relation-5cf34d2d9c7e95e9a6f39d87e40f96230e1c95efb26f507f30197cbe0536a104",
    "source_file": "standard/framework/detail/Motion.cpp",
    "source_location": "L525",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_motion_motionsystem_poplayer",
    "target": "standard_framework_detail_motion_cpp_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2653,
    "key": "relation-7cef42779ab8ce3c47abaa7cfb1280fcf6b04d225133264ff8436f7982c82fff",
    "source_file": "standard/framework/detail/Motion.cpp",
    "source_location": "L525",
    "weight": 1.0
  }
]
```

### standard_framework_detail_native_relativepathinside → standard_framework_detail_native_h_path

#### before
```json
{
  "source": "standard_framework_detail_native_relativepathinside",
  "target": "standard_framework_detail_native_h_path",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Native.h",
  "source_location": "L36",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_native_relativepathinside",
    "target": "standard_framework_detail_native_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2690,
    "key": "relation-adb610e229d728a6b2f1ee405337ff6a9f379d4256ee5dd9881dd7674814f385",
    "source_file": "standard/framework/detail/Native.h",
    "source_location": "L36",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_native_relativepathinside",
    "target": "standard_framework_detail_native_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 2689,
    "key": "relation-d373369a4341d3cffd3ebc6a51532725629e6b8c5e82ced204b8f56a8c747c91",
    "source_file": "standard/framework/detail/Native.h",
    "source_location": "L36",
    "weight": 1.0
  }
]
```

### standard_framework_detail_objloader_objwarning → standard_framework_detail_objloader_cpp_string

#### before
```json
{
  "source": "standard_framework_detail_objloader_objwarning",
  "target": "standard_framework_detail_objloader_cpp_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/ObjLoader.cpp",
  "source_location": "L28",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_objloader_objwarning",
    "target": "standard_framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2859,
    "key": "relation-1ca146231fed205b758c973702008ef8b15bd06dca086642399a6d4889c17029",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L28",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_objloader_objwarning",
    "target": "standard_framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2860,
    "key": "relation-3fd5c685f4b42d43926cfdd4133797681d7a4a6caf31febc2a8b791a9ddfebc7",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L28",
    "weight": 1.0
  }
]
```

### standard_framework_detail_objloader_objresolve → standard_framework_detail_objloader_cpp_size_t

#### before
```json
{
  "source": "standard_framework_detail_objloader_objresolve",
  "target": "standard_framework_detail_objloader_cpp_size_t",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/ObjLoader.cpp",
  "source_location": "L83",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_objloader_objresolve",
    "target": "standard_framework_detail_objloader_cpp_size_t",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 2889,
    "key": "relation-74a2f5cc1869dcf30e555c048871b89126fe047361a27af33f827783542df320",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L83",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_objloader_objresolve",
    "target": "standard_framework_detail_objloader_cpp_size_t",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2866,
    "key": "relation-c4fbd812191e7682bea38b4758b735e872b26cbc33d6a113a322b1cf79603103",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L77",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_objloader_objresolve",
    "target": "standard_framework_detail_objloader_cpp_size_t",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 2865,
    "key": "relation-4968c7918e436844bb6b79ac67395c2d6782eb3a33224e7b2ea3b212e791b300",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L77",
    "weight": 1.0
  }
]
```

### standard_framework_detail_objloader_objtext → standard_framework_detail_objloader_cpp_string

#### before
```json
{
  "source": "standard_framework_detail_objloader_objtext",
  "target": "standard_framework_detail_objloader_cpp_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/detail/ObjLoader.cpp",
  "source_location": "L113",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_objloader_objtext",
    "target": "standard_framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2874,
    "key": "relation-937b64409324ef75d3f0371792d28b38919f05d9d64f887c9a723b758667079b",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L113",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_objloader_objtext",
    "target": "standard_framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2872,
    "key": "relation-2b8db019b7c152133fe9f7d2640485d6a0ef93671aadb5fac2c6a58e12c0ba23",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L113",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_objloader_objtext",
    "target": "standard_framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 2871,
    "key": "relation-11c022433efaa68cb26b5ac624522658a4b8e0ae5becedff6c0a6a2434ee5add",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L113",
    "weight": 1.0
  }
]
```

### standard_framework_detail_objloader_parseobj → standard_framework_detail_objloader_cpp_string

#### before
```json
{
  "source": "standard_framework_detail_objloader_parseobj",
  "target": "standard_framework_detail_objloader_cpp_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/detail/ObjLoader.cpp",
  "source_location": "L189",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_objloader_parseobj",
    "target": "standard_framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2880,
    "key": "relation-aa80b2f73f0e5fd6119bc705cebb651ae7f6197daf893c1888ae2c7c408dcb09",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L189",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_objloader_parseobj",
    "target": "standard_framework_detail_objloader_cpp_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2877,
    "key": "relation-3dfa28bf357989498380e02bac0696f209256dc375a9a4b0a3b6b0a64845ffad",
    "source_file": "standard/framework/detail/ObjLoader.cpp",
    "source_location": "L189",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_conveyorcornerpose → standard_framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_conveyorcornerpose",
  "target": "standard_framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L28",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_conveyorcornerpose",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3053,
    "key": "relation-a3db0b552cf2fff620478de7f4b3b4f8a038e54f5fa48c1663a96b2512c3cadb",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L28",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_conveyorcornerpose",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2908,
    "key": "relation-175f53e9bdb98b54161563458a5e2d2f3e7f309c7892fa0a1331e67a0dbdffb9",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L7",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_collisionsystem_steproombody → standard_framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_patterns_collisionsystem_steproombody",
  "target": "standard_framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L203",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_collisionsystem_steproombody",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3058,
    "key": "relation-1203787461e82ac8533950f269d8ae38f3ad2cc34435ee5d5dac80d62897e010",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L203",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_collisionsystem_steproombody",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2932,
    "key": "relation-0bb77d69b08e528d0d0da3d29b207b1d6aed3e4bd0c26b0b067042723265dff7",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L166",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_collisionsystem_stepcharacter → standard_framework_detail_patterns_cpp_bounds3

#### before
```json
{
  "source": "standard_framework_detail_patterns_collisionsystem_stepcharacter",
  "target": "standard_framework_detail_patterns_cpp_bounds3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L268",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_collisionsystem_stepcharacter",
    "target": "standard_framework_detail_patterns_cpp_bounds3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2941,
    "key": "relation-30cfa396bcac3d3361da0501563d57089e588d333cd038d2b5ede354fca3bd5e",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L268",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_collisionsystem_stepcharacter",
    "target": "standard_framework_detail_patterns_cpp_bounds3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2939,
    "key": "relation-9585b08a6258ea0c58c441799cfd25f9c27509a317199813cb34b1f0c1e56590",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L268",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_collisionsystem_randomfreebox → standard_framework_detail_patterns_cpp_bounds3

#### before
```json
{
  "source": "standard_framework_detail_patterns_collisionsystem_randomfreebox",
  "target": "standard_framework_detail_patterns_cpp_bounds3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L394",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_collisionsystem_randomfreebox",
    "target": "standard_framework_detail_patterns_cpp_bounds3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2944,
    "key": "relation-8d6b0149d09ac0849089e50845780faca273e7dacbaf333a757841808bc7d3a6",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L394",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_collisionsystem_randomfreebox",
    "target": "standard_framework_detail_patterns_cpp_bounds3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2945,
    "key": "relation-a101dfaad551baa13c753594ead5ba603f8d0b1676d3f7b4ebed0921b2e8e6de",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L394",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_addbox → standard_framework_detail_patterns_cpp_optional

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_addbox",
  "target": "standard_framework_detail_patterns_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L434",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_addbox",
    "target": "standard_framework_detail_patterns_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2955,
    "key": "relation-d5c23fff45fd4ac9b7a6b13a8ecbf4963d4bef5fc7fbd45e15e069d365e8c517",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L434",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_addbox",
    "target": "standard_framework_detail_patterns_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 2950,
    "key": "relation-3837f58701d054be98d44ca215e7e29653d47acf13f1a15f1f634b7183a7d787",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L434",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_addpolygon → standard_framework_detail_patterns_cpp_optional

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_addpolygon",
  "target": "standard_framework_detail_patterns_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L461",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_addpolygon",
    "target": "standard_framework_detail_patterns_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2963,
    "key": "relation-872e2ecab54e4fa1adde5d1cf2468a42f6c3e049f6e08e700b38ae6bb5cfce51",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L461",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_addpolygon",
    "target": "standard_framework_detail_patterns_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 2957,
    "key": "relation-582d75ff9b4643cbc70d5c31badfdc31b54b0e24159350ed0d581170f7c7982e",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L461",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_addjoint → standard_framework_detail_patterns_cpp_entityid

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_addjoint",
  "target": "standard_framework_detail_patterns_cpp_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L517",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_addjoint",
    "target": "standard_framework_detail_patterns_cpp_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 2971,
    "key": "relation-d95198a2e1a68379d4e55f2995ad62b3664839bd6c4310eebfffa336db7e46a1",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L517",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_addjoint",
    "target": "standard_framework_detail_patterns_cpp_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2973,
    "key": "relation-81d4f0348dd12d5c02a5611caf3807a808e6d2a22692fe9ea984d2b890bae588",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L517",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_follow → standard_framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_follow",
  "target": "standard_framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L566",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_follow",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3062,
    "key": "relation-4458c93385f744a9939d9b3ce52ea2524a7b6e309443369ae2286865f6048b13",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L566",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_follow",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2983,
    "key": "relation-479b349166e07ce35d4eb2eaea1196c2cbc0f14191250bfbd18486480346b58a",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L552",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_addnumber → standard_framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_addnumber",
  "target": "standard_framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L585",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_addnumber",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3063,
    "key": "relation-a6b9d7d304fdb86b99466d4127be521c18f7f4f02c22f356d09e5b918b6b34ad",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L585",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_addnumber",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 2986,
    "key": "relation-3da869f67fe2c28a2864199ba4c93ae535930830212c7fc5e74a2a1242c5abac",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L571",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_snakecells → standard_framework_detail_patterns_cpp_ivec3

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_snakecells",
  "target": "standard_framework_detail_patterns_cpp_ivec3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L710",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_snakecells",
    "target": "standard_framework_detail_patterns_cpp_ivec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3017,
    "key": "relation-8220c59d69b87b1ddb4db2e3db40c017ae20c4ca8d1325cf0af2b34ca06d3af9",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L710",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_snakecells",
    "target": "standard_framework_detail_patterns_cpp_ivec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3018,
    "key": "relation-d85ffd717c9cca52ac53e24988141df06b0f3719e31d82ac3ba3a4814d798f74",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L710",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_snap → standard_framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_snap",
  "target": "standard_framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L743",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_snap",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3067,
    "key": "relation-505fbe02e335daf8e9a9bf6624a0235f8169e635db05a98318d2a10bf7cffbdf",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L743",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_snap",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3022,
    "key": "relation-1c6e4ecab80ddfad50f5f42b2a77d56954c9d443289a51b4111f525c5c4a7496",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L735",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_snap",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 3021,
    "key": "relation-f3ac181b0dbba2204a0fd9e1dd3db382fd991c04cee21ed254a2204fd706aaca",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L735",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_screenonplane → standard_framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_screenonplane",
  "target": "standard_framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L777",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_screenonplane",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3068,
    "key": "relation-657a94d09b6d0785342c4abee13e479c6f684542a3983a380c123d144a86ca51",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L777",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_screenonplane",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3025,
    "key": "relation-6a2ca6c70caa9a412c5ee1294716c5ecee2b0cb0a162f508c565485fc4ad6c00",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L754",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_screenonplane",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3028,
    "key": "relation-e2691e84fc1f580c8fbb24bb16daf0e54f40dcd31c9c9a6b71f164dff1b18ee7",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L754",
    "weight": 1.0
  }
]
```

### standard_framework_detail_patterns_patternsystem_posewalkingrig → standard_framework_detail_patterns_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_patterns_patternsystem_posewalkingrig",
  "target": "standard_framework_detail_patterns_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Patterns.cpp",
  "source_location": "L980",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_patterns_patternsystem_posewalkingrig",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3075,
    "key": "relation-d2f7b0926dcee81f4c4006b5bbe50a5cc5e8cdb6ff3cce03859f8f7ef990fa4b",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L980",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_patterns_patternsystem_posewalkingrig",
    "target": "standard_framework_detail_patterns_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3052,
    "key": "relation-63661b7d7b0fb2d297823791dd74d7bbfa6051def1a2e362d270f26ced7aa5f0",
    "source_file": "standard/framework/detail/Patterns.cpp",
    "source_location": "L954",
    "weight": 1.0
  }
]
```

### standard_framework_detail_scene_scene_setparent → standard_framework_detail_scene_cpp_entityid

#### before
```json
{
  "source": "standard_framework_detail_scene_scene_setparent",
  "target": "standard_framework_detail_scene_cpp_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/detail/Scene.cpp",
  "source_location": "L309",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_scene_scene_setparent",
    "target": "standard_framework_detail_scene_cpp_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3213,
    "key": "relation-feccde1268f1f32b8e00f01cd102c59839c223b2503ec4fe3b57ace3e5d640e7",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L309",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_scene_scene_setparent",
    "target": "standard_framework_detail_scene_cpp_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3211,
    "key": "relation-d5de45b5279d0cec426f68df179fa881d0651c1c3a35bb86a259f5f4c42563fc",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L309",
    "weight": 1.0
  }
]
```

### standard_framework_detail_scene_scene_faceworldmatrix → standard_framework_detail_scene_cpp_mat4

#### before
```json
{
  "source": "standard_framework_detail_scene_scene_faceworldmatrix",
  "target": "standard_framework_detail_scene_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Scene.cpp",
  "source_location": "L408",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_scene_scene_faceworldmatrix",
    "target": "standard_framework_detail_scene_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3381,
    "key": "relation-54171f19d234455e96e06aab44597c0e94b1e0034fc3081b63d5b2e603203ef8",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L408",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_scene_scene_faceworldmatrix",
    "target": "standard_framework_detail_scene_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3220,
    "key": "relation-55f1aefbbde217ad8eda60519929e2fb91a037f5710528b6712544bb33ce3001",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L397",
    "weight": 1.0
  }
]
```

### standard_framework_detail_scene_scene_addline → standard_framework_detail_scene_cpp_optional

#### before
```json
{
  "source": "standard_framework_detail_scene_scene_addline",
  "target": "standard_framework_detail_scene_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Scene.cpp",
  "source_location": "L534",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_scene_scene_addline",
    "target": "standard_framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3252,
    "key": "relation-9e26b8e583288c8a10d2460a7b8683845f2af41b07bdbac59a6f08539c04144a",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L534",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_scene_scene_addline",
    "target": "standard_framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 3248,
    "key": "relation-e53d505f501c026a1a1bc41ce3eca1f60c41aab36f4d26cb013c6e586451e756",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L534",
    "weight": 1.0
  }
]
```

### standard_framework_detail_scene_scene_addvector → standard_framework_detail_scene_cpp_optional

#### before
```json
{
  "source": "standard_framework_detail_scene_scene_addvector",
  "target": "standard_framework_detail_scene_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Scene.cpp",
  "source_location": "L554",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_scene_scene_addvector",
    "target": "standard_framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3259,
    "key": "relation-d006969a1cef31e5b7e1a99b7964a0cf52cd1685f375ececd5d3018f85463ae2",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L554",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_scene_scene_addvector",
    "target": "standard_framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 3255,
    "key": "relation-55be0fa3720650ec933ce9b07a85803f0b080e4f1127b65dc2f7893fc0a80ff5",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L554",
    "weight": 1.0
  }
]
```

### standard_framework_detail_scene_scene_addbasis → standard_framework_detail_scene_cpp_optional

#### before
```json
{
  "source": "standard_framework_detail_scene_scene_addbasis",
  "target": "standard_framework_detail_scene_cpp_optional",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/detail/Scene.cpp",
  "source_location": "L559",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_scene_scene_addbasis",
    "target": "standard_framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3267,
    "key": "relation-36c721c3609cdf30f465beabed0016f495e08bcf5739eae7b82489c128b3d324",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L559",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_scene_scene_addbasis",
    "target": "standard_framework_detail_scene_cpp_optional",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 3262,
    "key": "relation-b9121ff24c9a3ce7baf2aa18f954e6047ad3c1b631ba2c033f4f1bd23ac5677e",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L559",
    "weight": 1.0
  }
]
```

### standard_framework_detail_scene_diagramsystem_makepolyhedron → standard_framework_detail_scene_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_scene_diagramsystem_makepolyhedron",
  "target": "standard_framework_detail_scene_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Scene.cpp",
  "source_location": "L687",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_scene_diagramsystem_makepolyhedron",
    "target": "standard_framework_detail_scene_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3391,
    "key": "relation-b59516a702faf20c4323fd077ed6dcf42998622645e03aca8f2d28e91f51eeb2",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L687",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_scene_diagramsystem_makepolyhedron",
    "target": "standard_framework_detail_scene_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3280,
    "key": "relation-6c2fbbcded809eb2f15545c327fd80cce7797460ae23f9f3e2631d23ecec13a1",
    "source_file": "standard/framework/detail/Scene.cpp",
    "source_location": "L646",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_motionunitvector → standard_framework_detail_transform_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_transform_motionunitvector",
  "target": "standard_framework_detail_transform_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L16",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_motionunitvector",
    "target": "standard_framework_detail_transform_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3454,
    "key": "relation-58f0eec0cba7692d7d13e8c5f508709909a4e0c0b2a522636413bf38f8ee35e2",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L16",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_motionunitvector",
    "target": "standard_framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3400,
    "key": "relation-a785701b23be44d6c231a9557e60562a6f40b7b36af7a613f9a73bfc887188d7",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L7",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_motionunitvector",
    "target": "standard_framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3401,
    "key": "relation-3b5233091d6bfbec7c13c18685c676b191ca08644b955eb43c91e2084bea0f9f",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L7",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_motionaround → standard_framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "standard_framework_detail_transform_motionaround",
  "target": "standard_framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L23",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_motionaround",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3455,
    "key": "relation-9786c3459847756406ac247dab9206c1de14a587be819e0d345cb0eef1b3fbac",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L23",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_motionaround",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3404,
    "key": "relation-b9ae4a409134e04390b2955382b4fe319c02a46569cc57df5b6e7f623e3adb02",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L19",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_motionaround",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3406,
    "key": "relation-dea93b784d492e92b31e13b6e8068b65a2729287066bbb719bd48de9c47dc8d2",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L19",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_transformsystem_translation → standard_framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "standard_framework_detail_transform_transformsystem_translation",
  "target": "standard_framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L47",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_transformsystem_translation",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3457,
    "key": "relation-920c898c1b755cc839b5896ec641453c30515dbad3036e8e0ed2cedcea946448",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L47",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_transformsystem_translation",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3413,
    "key": "relation-6d35e75ff14b1d18a5d763e0c38717600e0efb88c5e2cb6a0d06a32156e6eac8",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L39",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_transformsystem_scaling → standard_framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "standard_framework_detail_transform_transformsystem_scaling",
  "target": "standard_framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L58",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_transformsystem_scaling",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3459,
    "key": "relation-bb90bb3cf8949b64aba616d2dbbfa89f16c72d5bc0e14dc3dfa45e8af77b61ac",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L58",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_transformsystem_scaling",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3417,
    "key": "relation-2b44462cc395a55c3b3634492ffeb4f40b338530b2a488d2ef83db9992a7467e",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L50",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_transformsystem_rotation → standard_framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "standard_framework_detail_transform_transformsystem_rotation",
  "target": "standard_framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L73",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_transformsystem_rotation",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3462,
    "key": "relation-cb1d8c99c3be01ecd430ef7406c6275b218a3596dae7d9a6b51c2f0acf91e1e6",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L73",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_transformsystem_rotation",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3421,
    "key": "relation-6141de4f67333479621a77d9e20ccd5cd64c02a42b6d6bbadd0de81e0a5888a4",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L61",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_transformsystem_reflectpoint → standard_framework_detail_transform_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_transform_transformsystem_reflectpoint",
  "target": "standard_framework_detail_transform_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L77",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_transformsystem_reflectpoint",
    "target": "standard_framework_detail_transform_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3463,
    "key": "relation-8c2352f0458f8999c87b7ecfd94b9dd796a7de93a724ad6134394523db02b3bd",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L77",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_transformsystem_reflectpoint",
    "target": "standard_framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3426,
    "key": "relation-046ad540ab4d2cab32af0ebe621feb700d3be27e181bedb29098413c546a006f",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L76",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_transformsystem_reflectline → standard_framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "standard_framework_detail_transform_transformsystem_reflectline",
  "target": "standard_framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L96",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_transformsystem_reflectline",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3466,
    "key": "relation-4a81a4fad805e41f48a91b0865eb16d44f79cb8f5e46d93de142edf272028a0c",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L96",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_transformsystem_reflectline",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3429,
    "key": "relation-8c395ed9c3a4ab46e2bf8869e32fe528b09b74791c378de25c6f152413118c23",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L80",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_transformsystem_reflectplane → standard_framework_detail_transform_cpp_mat4

#### before
```json
{
  "source": "standard_framework_detail_transform_transformsystem_reflectplane",
  "target": "standard_framework_detail_transform_cpp_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L115",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_transformsystem_reflectplane",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3469,
    "key": "relation-0bec1d2c8c270c7df2ea55923417752dd4588a44065bf559ddea8e4d7df320a9",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L115",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_transformsystem_reflectplane",
    "target": "standard_framework_detail_transform_cpp_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3433,
    "key": "relation-f446c96a1ecf47ee6b5ab7dddfec080df72983bedc8e61df06949813ad9391b4",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L99",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_pathsystem_circle → standard_framework_detail_transform_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_transform_pathsystem_circle",
  "target": "standard_framework_detail_transform_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L143",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_pathsystem_circle",
    "target": "standard_framework_detail_transform_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3472,
    "key": "relation-c18a44b2dc794ad837060ade592b4b3843df64cdd2db485bd2d6a2fc2fdbb363",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L143",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_pathsystem_circle",
    "target": "standard_framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3440,
    "key": "relation-739d57a85b6d4a397c0142a771b7f75bdb7f4e27638f964ef2c2dc987c20dd7f",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L133",
    "weight": 1.0
  }
]
```

### standard_framework_detail_transform_pathsystem_polyline → standard_framework_detail_transform_cpp_vec3

#### before
```json
{
  "source": "standard_framework_detail_transform_pathsystem_polyline",
  "target": "standard_framework_detail_transform_cpp_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/detail/Transform.cpp",
  "source_location": "L197",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_detail_transform_pathsystem_polyline",
    "target": "standard_framework_detail_transform_cpp_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3473,
    "key": "relation-7292821d63fb336bd5add0ec27a02668c551d9b6034b7615a44b062cc783b4c3",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L197",
    "weight": 1.0
  },
  {
    "source": "standard_framework_detail_transform_pathsystem_polyline",
    "target": "standard_framework_detail_transform_cpp_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3444,
    "key": "relation-5761f3fc3bc4d04bf7403da279277fa871058be8245c0bb783bb43e894a23815",
    "source_file": "standard/framework/detail/Transform.cpp",
    "source_location": "L149",
    "weight": 1.0
  }
]
```

### standard_framework_include_actions_sceneactions_parent → standard_framework_include_actions_h_entityid

#### before
```json
{
  "source": "standard_framework_include_actions_sceneactions_parent",
  "target": "standard_framework_include_actions_h_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/include/Actions.h",
  "source_location": "L103",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_actions_sceneactions_parent",
    "target": "standard_framework_include_actions_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3560,
    "key": "relation-33ce0ebdfd5a2a01a68ff15d75db9722bb927c9c11aa9961e90180db7b571f56",
    "source_file": "standard/framework/include/Actions.h",
    "source_location": "L103",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_actions_sceneactions_parent",
    "target": "standard_framework_include_actions_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3558,
    "key": "relation-4460ba8db0d5908ff796243ea07bcf55cbb68eb62294b94d571da8008dcf55ce",
    "source_file": "standard/framework/include/Actions.h",
    "source_location": "L103",
    "weight": 1.0
  }
]
```

### standard_framework_include_animation_humanoidrig → standard_framework_include_animation_h_entityid

#### before
```json
{
  "source": "standard_framework_include_animation_humanoidrig",
  "target": "standard_framework_include_animation_h_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/include/Animation.h",
  "source_location": "L21",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_animation_humanoidrig",
    "target": "standard_framework_include_animation_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 3583,
    "key": "relation-cddd6155197d9dd9f1e0177e867d3d47182d5dcbeca1a564383d0fa1c4d78644",
    "source_file": "standard/framework/include/Animation.h",
    "source_location": "L19",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_animation_humanoidrig",
    "target": "standard_framework_include_animation_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3592,
    "key": "relation-f41c6dcbade20c8290c6ef6c4c8e6f6f9328b2e38817c3da673c349da9910e41",
    "source_file": "standard/framework/include/Animation.h",
    "source_location": "L21",
    "weight": 1.0
  }
]
```

### standard_framework_include_animation_humanoidpose → standard_framework_include_animation_h_vec3

#### before
```json
{
  "source": "standard_framework_include_animation_humanoidpose",
  "target": "standard_framework_include_animation_h_vec3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/include/Animation.h",
  "source_location": "L27",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_animation_humanoidpose",
    "target": "standard_framework_include_animation_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 3604,
    "key": "relation-24942beae13c0c93e2187b6abdf9a0cd519c62767d22b9daaebd8ed939554e40",
    "source_file": "standard/framework/include/Animation.h",
    "source_location": "L26",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_animation_humanoidpose",
    "target": "standard_framework_include_animation_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3609,
    "key": "relation-59c3efa37b6048f69f52aa3a792ec2fda7e8f2f11c1dd9ed5af9080f360bc168",
    "source_file": "standard/framework/include/Animation.h",
    "source_location": "L27",
    "weight": 1.0
  }
]
```

### standard_framework_include_animation_swingrig → standard_framework_include_animation_h_entityid

#### before
```json
{
  "source": "standard_framework_include_animation_swingrig",
  "target": "standard_framework_include_animation_h_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/include/Animation.h",
  "source_location": "L47",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_animation_swingrig",
    "target": "standard_framework_include_animation_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 3627,
    "key": "relation-ed4ead7d3ef9c4ac49ca7a2e3115e4207692a63c9a35235b436a82e3c37605b7",
    "source_file": "standard/framework/include/Animation.h",
    "source_location": "L46",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_animation_swingrig",
    "target": "standard_framework_include_animation_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3632,
    "key": "relation-977597d5fbce263caad51860c219158936e55786c0a5b12f6c957a19b6f6554b",
    "source_file": "standard/framework/include/Animation.h",
    "source_location": "L47",
    "weight": 1.0
  }
]
```

### standard_framework_include_geometry_transformtexcoords → standard_framework_include_geometry_h_vec2

#### before
```json
{
  "source": "standard_framework_include_geometry_transformtexcoords",
  "target": "standard_framework_include_geometry_h_vec2",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/Geometry.h",
  "source_location": "L33",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_geometry_transformtexcoords",
    "target": "standard_framework_include_geometry_h_vec2",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3896,
    "key": "relation-6fcaa76d44dcf5da00478fabe232eca80a74da440d49379684cc15f70bbb2c96",
    "source_file": "standard/framework/include/Geometry.h",
    "source_location": "L33",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_geometry_transformtexcoords",
    "target": "standard_framework_include_geometry_h_vec2",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3876,
    "key": "relation-bc6d2b2523f7444e99d2dc1df0157f79a337b388e618f28442b4b8e82d500cd4",
    "source_file": "standard/framework/include/Geometry.h",
    "source_location": "L20",
    "weight": 1.0
  }
]
```

### standard_framework_include_geometry_triangulate → standard_framework_include_geometry_h_vector

#### before
```json
{
  "source": "standard_framework_include_geometry_triangulate",
  "target": "standard_framework_include_geometry_h_vector",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/include/Geometry.h",
  "source_location": "L281",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_geometry_triangulate",
    "target": "standard_framework_include_geometry_h_vector",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 3888,
    "key": "relation-bc9d41303af2dfa0e232e34b13352eeb21b57faceb9c3a0ca1a7517294a95daf",
    "source_file": "standard/framework/include/Geometry.h",
    "source_location": "L281",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_geometry_triangulate",
    "target": "standard_framework_include_geometry_h_vector",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 3886,
    "key": "relation-e6946425e40dfdc2b9d4f1c2feacd91532eb9254629b8825add830395897247b",
    "source_file": "standard/framework/include/Geometry.h",
    "source_location": "L281",
    "weight": 1.0
  }
]
```

### standard_framework_include_geometry_triangulate → standard_framework_include_geometry_h_vec3

#### before
```json
{
  "source": "standard_framework_include_geometry_triangulate",
  "target": "standard_framework_include_geometry_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/Geometry.h",
  "source_location": "L291",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_geometry_triangulate",
    "target": "standard_framework_include_geometry_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 3901,
    "key": "relation-24b927199cc806d3b3cad42f18440261a43253ad2c0baf919d52eaff458a1c5b",
    "source_file": "standard/framework/include/Geometry.h",
    "source_location": "L291",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_geometry_triangulate",
    "target": "standard_framework_include_geometry_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 3889,
    "key": "relation-df4be2a8888fe52b64c690ae7a97993f8fbc9edf1b70338e7ceaf432e1888d34",
    "source_file": "standard/framework/include/Geometry.h",
    "source_location": "L281",
    "weight": 1.0
  }
]
```

### standard_framework_include_graphics_renderer → standard_framework_include_graphics_drawrange

#### before
```json
{
  "source": "standard_framework_include_graphics_renderer",
  "target": "standard_framework_include_graphics_drawrange",
  "relation": "contains",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "source_file": "standard/framework/include/Graphics.h",
  "source_location": "L379",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_graphics_renderer",
    "target": "standard_framework_include_graphics_drawrange",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4073,
    "key": "relation-31574d7a2a8441d4ae460babf593c6b0a193312fdc8dbf4d4ab35d7e99220ee2",
    "source_file": "standard/framework/include/Graphics.h",
    "source_location": "L393",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_graphics_renderer",
    "target": "standard_framework_include_graphics_drawrange",
    "relation": "contains",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "extraction_index": 4040,
    "key": "relation-2dc0405594c06c46fbb3fc0918fd93f20a5ad2e246e30af093ee726edf598751",
    "source_file": "standard/framework/include/Graphics.h",
    "source_location": "L379",
    "weight": 1.0
  }
]
```

### standard_framework_include_gridlayout_gridlayout_position → standard_framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "standard_framework_include_gridlayout_gridlayout_position",
  "target": "standard_framework_include_gridlayout_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/GridLayout.h",
  "source_location": "L32",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_gridlayout_gridlayout_position",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 4126,
    "key": "relation-386a135575015f6d3af17cd3fc33358debebc79adbe3e0fd2a73ccc72c24eb9a",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L32",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_gridlayout_gridlayout_position",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4106,
    "key": "relation-fef25afaa3fd7de9f72bac59d952c86e7b3401d33d5b39442a2c8dd259fbf4d8",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L23",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_gridlayout_gridlayout_position",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 4107,
    "key": "relation-c7bdf4d388a50d3b72c75f5b3d815d75959440a918fae0d179df550a695ec058",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L23",
    "weight": 1.0
  }
]
```

### standard_framework_include_gridlayout_gridlayout_cellcenter → standard_framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "standard_framework_include_gridlayout_gridlayout_cellcenter",
  "target": "standard_framework_include_gridlayout_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/GridLayout.h",
  "source_location": "L55",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_gridlayout_gridlayout_cellcenter",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 4129,
    "key": "relation-01b25d9817cee145a636a80e86a14ecfaced471ac3bd1c65021ae9fc9d80f64e",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L55",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_gridlayout_gridlayout_cellcenter",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4110,
    "key": "relation-63fbde3d534f70408fd4347dd8b5ad2eb5365c2f9afaba1e324735129173fdc7",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L46",
    "weight": 1.0
  }
]
```

### standard_framework_include_gridlayout_gridlayout_relativesize → standard_framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "standard_framework_include_gridlayout_gridlayout_relativesize",
  "target": "standard_framework_include_gridlayout_h_vec3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/include/GridLayout.h",
  "source_location": "L60",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_gridlayout_gridlayout_relativesize",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4114,
    "key": "relation-ee2cce470f52e623e5b7808b0c5a6bb39c1ce15d49ce6c4d09fc5182e4a811ac",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L60",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_gridlayout_gridlayout_relativesize",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 4115,
    "key": "relation-2d989624bc7f878c989c872cc047a7c3be6b22929a690aee97465a83530efa31",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L60",
    "weight": 1.0
  }
]
```

### standard_framework_include_gridlayout_gridlayout_positioninparent → standard_framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "standard_framework_include_gridlayout_gridlayout_positioninparent",
  "target": "standard_framework_include_gridlayout_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/GridLayout.h",
  "source_location": "L95",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_gridlayout_gridlayout_positioninparent",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 4132,
    "key": "relation-47986dc4149579e947fc7b3ca99ee8534a382d909eadc6a8a60ff8e434d4f4cc",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L95",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_gridlayout_gridlayout_positioninparent",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4118,
    "key": "relation-90865c58b5829786974056fee2b1e70609f84a2d0797e96dba7a34aeb2ab7dbb",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L83",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_gridlayout_gridlayout_positioninparent",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 4119,
    "key": "relation-26bcad18bd9ed78dc6c2444e4e5adba2384f8807959a1ffacd0c23d8473083bf",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L83",
    "weight": 1.0
  }
]
```

### standard_framework_include_gridlayout_gridlayout_cellat → standard_framework_include_gridlayout_h_ivec3

#### before
```json
{
  "source": "standard_framework_include_gridlayout_gridlayout_cellat",
  "target": "standard_framework_include_gridlayout_h_ivec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/GridLayout.h",
  "source_location": "L124",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_gridlayout_gridlayout_cellat",
    "target": "standard_framework_include_gridlayout_h_ivec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 4135,
    "key": "relation-b5bdc0b21767ddd3226ea39f0df951a3e06d7f41f221b299633f664c3cdc7062",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L124",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_gridlayout_gridlayout_cellat",
    "target": "standard_framework_include_gridlayout_h_ivec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4123,
    "key": "relation-431eb5eedf426e457b4cdd6719523a997d8b05e150268566b9414d3fc6ceb976",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L109",
    "weight": 1.0
  }
]
```

### standard_framework_include_gridlayout_gridlayout_cellat → standard_framework_include_gridlayout_h_vec3

#### before
```json
{
  "source": "standard_framework_include_gridlayout_gridlayout_cellat",
  "target": "standard_framework_include_gridlayout_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/GridLayout.h",
  "source_location": "L117",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_gridlayout_gridlayout_cellat",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 4134,
    "key": "relation-cc99343724a88f059e390262332526cfe0301f03e459a6d34be81cd792bf7142",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L117",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_gridlayout_gridlayout_cellat",
    "target": "standard_framework_include_gridlayout_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 4124,
    "key": "relation-069570b69bfb62cf6012ed34419ee20c9a738d611e34d55be06e5be7a574d4f3",
    "source_file": "standard/framework/include/GridLayout.h",
    "source_location": "L109",
    "weight": 1.0
  }
]
```

### standard_framework_include_model_planefigure → standard_framework_include_model_h_vec3

#### before
```json
{
  "source": "standard_framework_include_model_planefigure",
  "target": "standard_framework_include_model_h_vec3",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "field",
  "source_file": "standard/framework/include/Model.h",
  "source_location": "L77",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_model_planefigure",
    "target": "standard_framework_include_model_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 4193,
    "key": "relation-1a6b83fc01fce7fde9bae7ab708e63125135341282214952673f2a325feecd40",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L77",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_model_planefigure",
    "target": "standard_framework_include_model_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4189,
    "key": "relation-80a75d5f7d28c96dc8a5ec0ef7de67285cf554b9b7154424db0529546b638946",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L75",
    "weight": 1.0
  }
]
```

### standard_framework_include_model_planefigure → standard_framework_include_model_glcolor

#### before
```json
{
  "source": "standard_framework_include_model_planefigure",
  "target": "standard_framework_include_model_glcolor",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/include/Model.h",
  "source_location": "L83",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_model_planefigure",
    "target": "standard_framework_include_model_glcolor",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 4191,
    "key": "relation-0dba9751cc9e05911e90623ed0b44e5f584f468ba977df8d67e0b63ef55d9d9a",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L76",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_model_planefigure",
    "target": "standard_framework_include_model_glcolor",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4201,
    "key": "relation-8726a3d86ace60949751e09ec39d74f0a8ef34e494cc362af73f200e122744ca",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L83",
    "weight": 1.0
  }
]
```

### standard_framework_include_model_transformmatrix → standard_framework_include_model_h_mat4

#### before
```json
{
  "source": "standard_framework_include_model_transformmatrix",
  "target": "standard_framework_include_model_h_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/Model.h",
  "source_location": "L285",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_model_transformmatrix",
    "target": "standard_framework_include_model_h_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 4305,
    "key": "relation-36fb5a2a61ad60896f80421d6d81512eaef6b17d14319c05a4e6e77bd4dab048",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L285",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_model_transformmatrix",
    "target": "standard_framework_include_model_h_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 4286,
    "key": "relation-df8fead17bf40d7d2a07dcff44cfc3e8141fc2e9f8d945da6c5536a31e97da50",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L284",
    "weight": 1.0
  }
]
```

### standard_framework_include_model_facesrtmatrix → standard_framework_include_model_h_mat4

#### before
```json
{
  "source": "standard_framework_include_model_facesrtmatrix",
  "target": "standard_framework_include_model_h_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/Model.h",
  "source_location": "L295",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_model_facesrtmatrix",
    "target": "standard_framework_include_model_h_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 4307,
    "key": "relation-1b0b53b34cc043f51a2e57e2de1709b03580201b0884b40da192da0f11ab3bde",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L295",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_model_facesrtmatrix",
    "target": "standard_framework_include_model_h_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 4289,
    "key": "relation-9f23d70716147c5c2c26aef9a19b3a6f551536c1b5f86d05ef80b4e3066886cc",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L293",
    "weight": 1.0
  }
]
```

### standard_framework_include_model_facematrix → standard_framework_include_model_h_mat4

#### before
```json
{
  "source": "standard_framework_include_model_facematrix",
  "target": "standard_framework_include_model_h_mat4",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/Model.h",
  "source_location": "L308",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_model_facematrix",
    "target": "standard_framework_include_model_h_mat4",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 4311,
    "key": "relation-a21cb70a346e9729e8743a283390f80c8ff8f13c9b44faa6c5ee85c9914da1d1",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L308",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_model_facematrix",
    "target": "standard_framework_include_model_h_mat4",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 4295,
    "key": "relation-149ac8b40d69ec4604a223f0892d710aa29a9acc8c411aab6d66f5edeebc1e92",
    "source_file": "standard/framework/include/Model.h",
    "source_location": "L307",
    "weight": 1.0
  }
]
```

### standard_framework_include_modelloader_modelresult → standard_framework_include_modelloader_h_string

#### before
```json
{
  "source": "standard_framework_include_modelloader_modelresult",
  "target": "standard_framework_include_modelloader_h_string",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/include/ModelLoader.h",
  "source_location": "L27",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_modelloader_modelresult",
    "target": "standard_framework_include_modelloader_h_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 4337,
    "key": "relation-16aae60364aeceda1ae6d577a4721f1134839ac7a1caad957d4b48f371073850",
    "source_file": "standard/framework/include/ModelLoader.h",
    "source_location": "L26",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_modelloader_modelresult",
    "target": "standard_framework_include_modelloader_h_string",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4340,
    "key": "relation-607048c12bafcfd7e95fe8054225d61e8f8bccda68a91f21db61871a6ae5f06a",
    "source_file": "standard/framework/include/ModelLoader.h",
    "source_location": "L27",
    "weight": 1.0
  }
]
```

### standard_framework_include_modelloader_assetloader → standard_framework_include_modelloader_h_path

#### before
```json
{
  "source": "standard_framework_include_modelloader_assetloader",
  "target": "standard_framework_include_modelloader_h_path",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/include/ModelLoader.h",
  "source_location": "L81",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_modelloader_assetloader",
    "target": "standard_framework_include_modelloader_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 4354,
    "key": "relation-7ad9bdf12ffd41e422f351232d704736111f999df9e740239befdc55455a09ca",
    "source_file": "standard/framework/include/ModelLoader.h",
    "source_location": "L77",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_modelloader_assetloader",
    "target": "standard_framework_include_modelloader_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4359,
    "key": "relation-4939ac02c06b9531eb5f13b7faa2e099d1f2e91baa204f7dd08e18fa757debd4",
    "source_file": "standard/framework/include/ModelLoader.h",
    "source_location": "L81",
    "weight": 1.0
  }
]
```

### standard_framework_include_modelloader_assetloader_resolve → standard_framework_include_modelloader_h_path

#### before
```json
{
  "source": "standard_framework_include_modelloader_assetloader_resolve",
  "target": "standard_framework_include_modelloader_h_path",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "parameter_type",
  "source_file": "standard/framework/include/ModelLoader.h",
  "source_location": "L84",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_modelloader_assetloader_resolve",
    "target": "standard_framework_include_modelloader_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "parameter_type",
    "extraction_index": 4365,
    "key": "relation-97564207d15642aa7d91c6771ad645c59d888f9076ff9b7f00f2d85409a3e939",
    "source_file": "standard/framework/include/ModelLoader.h",
    "source_location": "L84",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_modelloader_assetloader_resolve",
    "target": "standard_framework_include_modelloader_h_path",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 4364,
    "key": "relation-55351c6f41ed14c07811673645dd911012ff7ab5075e387b274f6f0f2e1494f0",
    "source_file": "standard/framework/include/ModelLoader.h",
    "source_location": "L84",
    "weight": 1.0
  }
]
```

### standard_framework_include_motion_motionsystem → standard_framework_include_motion_h_motionid

#### before
```json
{
  "source": "standard_framework_include_motion_motionsystem",
  "target": "standard_framework_include_motion_h_motionid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "field",
  "source_file": "standard/framework/include/Motion.h",
  "source_location": "L161",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_motion_motionsystem",
    "target": "standard_framework_include_motion_h_motionid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 4491,
    "key": "relation-15a7e93e3e10a2b9992b70b50191f013bc1879cc052da7215c9d1faaa8f01630",
    "source_file": "standard/framework/include/Motion.h",
    "source_location": "L161",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_motion_motionsystem",
    "target": "standard_framework_include_motion_h_motionid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4488,
    "key": "relation-e1840f2e85031acd3f28d2c383d1492c556d485eef6bce438e95a54b1c2b18bd",
    "source_file": "standard/framework/include/Motion.h",
    "source_location": "L160",
    "weight": 1.0
  }
]
```

### standard_framework_include_motion_motionsystem → standard_framework_include_motion_h_layerid

#### before
```json
{
  "source": "standard_framework_include_motion_motionsystem",
  "target": "standard_framework_include_motion_h_layerid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "field",
  "source_file": "standard/framework/include/Motion.h",
  "source_location": "L163",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_motion_motionsystem",
    "target": "standard_framework_include_motion_h_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 4496,
    "key": "relation-134fddef942c427240efac23ecae0bf597525ccd394897adf8343967988793aa",
    "source_file": "standard/framework/include/Motion.h",
    "source_location": "L163",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_motion_motionsystem",
    "target": "standard_framework_include_motion_h_layerid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4493,
    "key": "relation-241f974bd8e615156dd6dbb80f55a5dbd63cf34dfd355e14d0677af660774b4b",
    "source_file": "standard/framework/include/Motion.h",
    "source_location": "L162",
    "weight": 1.0
  }
]
```

### standard_framework_include_patterns_roombody_position → standard_framework_include_patterns_h_vec3

#### before
```json
{
  "source": "standard_framework_include_patterns_roombody_position",
  "target": "standard_framework_include_patterns_h_vec3",
  "relation": "calls",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "call",
  "source_file": "standard/framework/include/Patterns.h",
  "source_location": "L71",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_patterns_roombody_position",
    "target": "standard_framework_include_patterns_h_vec3",
    "relation": "calls",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "call",
    "extraction_index": 4621,
    "key": "relation-693028ee3be625ca878692a5c51c3578e49b5766f8595c7e2887289e6ff89995",
    "source_file": "standard/framework/include/Patterns.h",
    "source_location": "L71",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_patterns_roombody_position",
    "target": "standard_framework_include_patterns_h_vec3",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "return_type",
    "extraction_index": 4558,
    "key": "relation-ae7deeb58488a99cf398fdd791fca871705cc7a2a928ccc15d4b1f9617ffc1e1",
    "source_file": "standard/framework/include/Patterns.h",
    "source_location": "L70",
    "weight": 1.0
  }
]
```

### standard_framework_include_patterns_jointrig → standard_framework_include_patterns_h_entityid

#### before
```json
{
  "source": "standard_framework_include_patterns_jointrig",
  "target": "standard_framework_include_patterns_h_entityid",
  "relation": "references",
  "_origin": "ast",
  "confidence": "EXTRACTED",
  "confidence_score": 1.0,
  "context": "generic_arg",
  "source_file": "standard/framework/include/Patterns.h",
  "source_location": "L114",
  "weight": 1.0
}
```

#### after
```json
[
  {
    "source": "standard_framework_include_patterns_jointrig",
    "target": "standard_framework_include_patterns_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "field",
    "extraction_index": 4581,
    "key": "relation-10c81a355720dce644c2becbb94e8f511eb351fcc48af884af0a91fbefbc0c4f",
    "source_file": "standard/framework/include/Patterns.h",
    "source_location": "L113",
    "weight": 1.0
  },
  {
    "source": "standard_framework_include_patterns_jointrig",
    "target": "standard_framework_include_patterns_h_entityid",
    "relation": "references",
    "_origin": "ast",
    "confidence": "EXTRACTED",
    "confidence_score": 1.0,
    "context": "generic_arg",
    "extraction_index": 4585,
    "key": "relation-50a679805b3f1385590d89a70bd98e751888a16ab37a7f35f502a6b57225a4d0",
    "source_file": "standard/framework/include/Patterns.h",
    "source_location": "L114",
    "weight": 1.0
  }
]
```

## 생성기 전체 변경 블록

### before
```python
import argparse
import collections
import importlib.util
import json
import os
import re
```

### after
```python
import argparse
import collections
import importlib.util
import hashlib
import json
import os
import re
```

### before
```python
    return nodes


def repair_saved_graph():
    """Patch saved graph endpoints without extracting or reclustering existing code."""
    import networkx as nx
    from graphify.export import to_json, to_html
    from graphify.analyze import god_nodes, surprising_connections, suggest_questions
    from graphify.cluster import score_all
    from graphify.report import generate
```

### after
```python
    return nodes


def restore_parallel_relations(graph, extraction):
    """Restore raw records only for pairs that a DiGraph previously collapsed."""
    import networkx as nx
    result = nx.MultiDiGraph(graph)
    groups = collections.defaultdict(list)
    for index, edge in enumerate(extraction['edges']):
        groups[edge['source'], edge['target']].append((index, edge))
    restored = []
    for (source, target), records in groups.items():
        if len(records) < 2:
            continue
        if source not in result or target not in result or not result.has_edge(source, target):
            raise RuntimeError('Parallel relation endpoints require separate source investigation: ' + source)
        result.remove_edges_from([(source, target, key) for key in list(result[source][target])])
        keys = []
        for index, edge in records:
            key = 'relation-' + hashlib.sha256(json.dumps(edge, sort_keys=True, ensure_ascii=False).encode('utf-8')).hexdigest()
            attrs = {k: v for k, v in edge.items() if k not in ('source', 'target')}
            result.add_edge(source, target, key=key, **attrs, extraction_index=index)
            keys.append(key)
        restored.append(dict(source=source, target=target, records=[e for _, e in records], keys=keys))
    return result, dict(graph_type='MultiDiGraph', affected_pairs=len(restored),
                        restored_additional_relations=sum(len(g['records']) - 1 for g in restored),
                        remaining_collapsed_relations=0, nodes=result.number_of_nodes(),
                        edges=result.number_of_edges(), groups=restored)


def export_relation_html(graph, communities, path, labels):
    """Use graphify's view and add source evidence and separate arcs for parallel records."""
    from graphify.export import to_html
    to_html(graph, communities, str(path), community_labels=labels)
    records = []
    for source, target, attrs in graph.edges(data=True):
        records.append(dict(source=source, target=target, relation=attrs.get('relation', ''),
                            context=attrs.get('context', ''), file=attrs.get('source_file', ''),
                            location=attrs.get('source_location', ''), confidence=attrs.get('confidence', '')))
    data = json.dumps(records, ensure_ascii=False).replace('</', '<\\/')
    script = '''<script id="relation-evidence">
const RELATION_EVIDENCE = __DATA__;
const parallelGroups = new Map();
RELATION_EVIDENCE.forEach((r, i) => {
  const key = JSON.stringify([r.source, r.target]);
  if (!parallelGroups.has(key)) parallelGroups.set(key, []);
  parallelGroups.get(key).push(i);
});
const evidenceUpdates = [];
parallelGroups.forEach(ids => ids.forEach((id, offset) => {
  const r = RELATION_EVIDENCE[id];
  const title = document.createElement('div');
  title.textContent = `${r.relation} [${r.context}] · ${r.file}:${r.location} · ${r.confidence}`;
  const update = {id, title};
  if (ids.length > 1) {
    update.label = `${r.relation} · ${r.context} · ${r.location}`;
    update.smooth = {enabled: true, type: offset % 2 ? 'curvedCCW' : 'curvedCW',
                     roundness: 0.15 + Math.floor(offset / 2) * 0.18};
  }
  evidenceUpdates.push(update);
}));
edgesDS.update(evidenceUpdates);
network.on('click', params => {
  if (params.nodes.length || !params.edges.length) return;
  const panel = document.getElementById('info-content');
  panel.replaceChildren();
  const heading = document.createElement('strong');
  heading.textContent = '관계 근거';
  panel.appendChild(heading);
  params.edges.forEach(id => {
    const r = RELATION_EVIDENCE[id];
    if (!r) return;
    const detail = document.createElement('p');
    detail.textContent = `${r.source} → ${r.target}: ${r.relation} / ${r.context} / ${r.file}:${r.location} [${r.confidence}]`;
    panel.appendChild(detail);
  });
});
</script>'''.replace('__DATA__', data)
    write(path, read(path).replace('</body>', script + '</body>'))


def relation_health(extraction, audit):
    from graphify.diagnostics import diagnose_extraction, format_diagnostic_report
    health = diagnose_extraction(extraction, directed=True, root=str(ROOT))
    return ('# 실제 출력 그래프\n\n'
            f'- graph_type: MultiDiGraph\n- nodes: {audit["nodes"]}\n- edges: {audit["edges"]}\n'
            f'- restored_parallel_pairs: {audit["affected_pairs"]}\n'
            f'- preserved_additional_relations: {audit["restored_additional_relations"]}\n'
            '- output_collapsed_relations: 0\n'
            f'- dangling_endpoint_edges: {health["dangling_endpoint_edges"]}\n\n'
            '## 원본 추출을 단순 DiGraph로 만들 경우의 진단\n\n'
            '아래 collapsed 값은 단순 그래프로 투영할 때의 값이다. 현재 MultiDiGraph의 손실 개수가 아니다.\n\n'
            + format_diagnostic_report(health))


def repair_saved_relations():
    import networkx as nx
    from graphify.export import to_json
    output = OUT / 'graphify-out'
    original = json.loads(read(output / 'graph.json'))
    extraction = json.loads(read(output / 'EXTRACTION.json'))
    graph = nx.node_link_graph(original, edges='links' if 'links' in original else 'edges')
    before_edges = graph.number_of_edges()
    graph, audit = restore_parallel_relations(graph, extraction)
    audit.update(before_edges=before_edges, existing_nodes_reextracted=False,
                 existing_communities_reclustered=False)
    communities, labels = collections.defaultdict(list), {}
    for ident, attrs in graph.nodes(data=True):
        cid = attrs['community']
        communities[cid].append(ident)
        labels[cid] = attrs.get('community_name', labels.get(cid, '공통 관계'))
    staged = output / 'graph.relation-patch.json'
    if not to_json(graph, communities, str(staged), community_labels=labels,
                   built_at_commit=original.get('built_at_commit')):
        raise RuntimeError('Relation export refused; preserve previous graph.')
    os.replace(staged, output / 'graph.json')
    export_relation_html(graph, communities, output / 'graph.html', labels)
    write(output / 'graph.json', read(output / 'graph.json'))
    dump(output / 'RELATION_REPAIR.json', audit)
    write(output / 'GRAPH_HEALTH.md', relation_health(extraction, audit))
    report = read(output / 'GRAPH_REPORT.md')
    report = re.sub(r'<!-- RELATION-REPAIR:BEGIN -->[\s\S]*?<!-- RELATION-REPAIR:END -->', '', report)
    report += ('\n<!-- RELATION-REPAIR:BEGIN -->\n## 병렬 관계 복원 후 현재 상태\n\n'
               f'위 초기 분석의 {before_edges:,}개 단순 간선을 {audit["edges"]:,}개 병렬 간선으로 보완했다. '
               f'대상 {audit["affected_pairs"]}쌍의 추가 관계 {audit["restored_additional_relations"]}건을 원본 종류·문맥·소스 줄과 함께 복원했다. '
               '기존 노드·커뮤니티·단일 간선과 원본 추출은 유지했다. 기존 순위·응집도는 단순 연결 구조 기준이다. '
               '코드 재추출·빌드·테스트·브라우저 확인은 하지 않았다.\n<!-- RELATION-REPAIR:END -->\n')
    write(output / 'GRAPH_REPORT.md', report)
    print(f'Parallel relations restored: {audit["restored_additional_relations"]} across {audit["affected_pairs"]} pairs; edges {before_edges} -> {audit["edges"]}.')


def repair_saved_graph():
    """Patch saved graph endpoints without extracting or reclustering existing code."""
    import networkx as nx
    from graphify.export import to_json
    from graphify.analyze import god_nodes, surprising_connections, suggest_questions
    from graphify.cluster import score_all
    from graphify.report import generate
```

### before
```python
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
```

### after
```python
                   built_at_commit=original.get('built_at_commit')):
        raise RuntimeError('Graph export refused; preserve previous report.')
    os.replace(staged, output / 'graph.json')
    export_relation_html(graph, communities, output / 'graph.html', labels)
    dump(output / 'EXTRACTION.json', extraction)
    dump(output / 'BOUNDARY_REPAIR.json', audit)
    dump(output / 'ANALYSIS.json', dict(gods=gods, surprises=surprises, questions=questions))
    write(output / 'GRAPH_REPORT.md', report)
    if graph.is_multigraph():
        _, relation_audit = restore_parallel_relations(graph, extraction)
        write(output / 'GRAPH_HEALTH.md', relation_health(extraction, relation_audit))
        dump(output / 'RELATION_REPAIR.json', relation_audit)
    else:
        write(output / 'GRAPH_HEALTH.md', format_diagnostic_report(health))
    for name in ('graph.json', 'graph.html'):
        write(output / name, read(output / name))
    print(f'Boundary patch: +{len(added)} nodes, +{len(edges)} include edges; remaining dangling: {health["dangling_endpoint_edges"]}.')
```

### before
```python
    health = health_path.read_text(encoding='utf-8-sig') if health_path.exists() else ''
    missing = re.search(r'dangling_endpoint_edges: (\d+)', health)
    collapsed = re.search(r'directed_same_endpoint_collapsed_edges: (\d+)', health)
    for directory in DIRECTORIES:
        block = '<!-- API-ARCHITECTURE:BEGIN --><section id="api-architecture"><h2>API 코드베이스: 네 관계와 종합 흐름</h2>'
        block += '<p>정적 소스와 graphify AST 분석에 근거한다. 화살표의 소유·참조·호출·생성·ID 연결을 구분한다. 각 설명의 소스 링크에서 실제 구현을 읽는다.</p>'
        if missing and collapsed:
            block += '<p class="badge">graphify 연결 대상 누락: ' + missing.group(1) + '건. 외부 헤더는 include 근거를 가진 경계 노드이며 내부 함수 분석을 뜻하지 않는다. 같은 노드 쌍의 관계 병합 ' + collapsed.group(1) + '건은 원본 EXTRACTION.json에 보존했다. 아래 순서도는 실제 소스를 대조했다. 빌드·실행·브라우저 확인은 사용자에게 맡긴다.</p>'
        block += '<nav>'
        block += ''.join('<a href="#arch-' + key + '">' + esc(c['title']) + '</a>' for key, c in charts.items()) + '</nav>'
        for key, chart in charts.items():
```

### after
```python
    health = health_path.read_text(encoding='utf-8-sig') if health_path.exists() else ''
    missing = re.search(r'dangling_endpoint_edges: (\d+)', health)
    collapsed = re.search(r'directed_same_endpoint_collapsed_edges: (\d+)', health)
    relation_path = OUT / 'graphify-out/RELATION_REPAIR.json'
    relation_audit = json.loads(relation_path.read_text(encoding='utf-8-sig')) if relation_path.exists() else None
    for directory in DIRECTORIES:
        block = '<!-- API-ARCHITECTURE:BEGIN --><section id="api-architecture"><h2>API 코드베이스: 네 관계와 종합 흐름</h2>'
        block += '<p>정적 소스와 graphify AST 분석에 근거한다. 화살표의 소유·참조·호출·생성·ID 연결을 구분한다. 각 설명의 소스 링크에서 실제 구현을 읽는다.</p>'
        if missing and collapsed:
            relation_note = ('같은 노드 쌍의 관계 병합 ' + collapsed.group(1) + '건은 원본 EXTRACTION.json에 보존했다.')
            if relation_audit:
                relation_note = (f'병합되었던 {relation_audit["restored_additional_relations"]}개 추가 관계를 '
                                 f'{relation_audit["affected_pairs"]}쌍의 병렬 간선으로 복원했다. 현재 관계 병합 손실은 '
                                 f'{relation_audit["remaining_collapsed_relations"]}건이다. 관계·문맥·소스 위치를 구분한다.')
            block += '<p class="badge">graphify 연결 대상 누락: ' + missing.group(1) + '건. 외부 헤더는 include 근거를 가진 경계 노드이며 내부 함수 분석을 뜻하지 않는다. ' + relation_note + ' 아래 순서도는 실제 소스를 대조했다. 빌드·실행·브라우저 확인은 사용자에게 맡긴다.</p>'
        block += '<nav>'
        block += ''.join('<a href="#arch-' + key + '">' + esc(c['title']) + '</a>' for key, c in charts.items()) + '</nav>'
        for key, chart in charts.items():
```

### before
```python
    from graphify.build import build_from_json
    from graphify.cluster import cluster, score_all
    from graphify.analyze import god_nodes, surprising_connections, suggest_questions
    from graphify.export import to_json, to_html
    from graphify.report import generate
    from graphify.diagnostics import diagnose_extraction, format_diagnostic_report
    output = OUT / 'graphify-out'
```

### after
```python
    from graphify.build import build_from_json
    from graphify.cluster import cluster, score_all
    from graphify.analyze import god_nodes, surprising_connections, suggest_questions
    from graphify.export import to_json
    from graphify.report import generate
    from graphify.diagnostics import diagnose_extraction, format_diagnostic_report
    output = OUT / 'graphify-out'
```

### before
```python
    gods, surprises = god_nodes(graph), surprising_connections(graph, communities)
    questions = suggest_questions(graph, communities, labels)
    commit = subprocess.check_output(['git','rev-parse','HEAD'], cwd=ROOT, text=True).strip()
    to_json(graph, communities, str(output / 'graph.json'), force=True, community_labels=labels, built_at_commit=commit)
    to_html(graph, communities, str(output / 'graph.html'), community_labels=labels)
    report = generate(graph, communities, score_all(graph, communities), labels, gods, surprises, detection,
                      {'input': 0, 'output': 0}, str(ROOT), suggested_questions=questions, built_at_commit=commit)
    report += '\n## 범위와 한계\n\nframework와 standard/framework를 별도 경로로 추출했다. standard 실행은 standard/framework를 사용한다. 같은 이름의 C++ 메서드 간 AST 연결은 정확한 오버로드 해석이 아니다. 문서 순서도는 실제 소스를 추가로 읽어 작성했다. GPU 고정 단계는 C++ 호출 간선이 아니다. 빌드·테스트·브라우저 확인은 사용자에게 맡겼다.\n'
    write(output / 'GRAPH_REPORT.md', report)
    health = diagnose_extraction(extraction, directed=True, root=str(ROOT))
    write(output / 'GRAPH_HEALTH.md', format_diagnostic_report(health))
    dump(output / 'EXTRACTION.json', extraction)
    dump(output / 'DETECTION.json', detection)
    dump(output / 'ANALYSIS.json', {'gods': gods, 'surprises': surprises, 'questions': questions})
```

### after
```python
    gods, surprises = god_nodes(graph), surprising_connections(graph, communities)
    questions = suggest_questions(graph, communities, labels)
    commit = subprocess.check_output(['git','rev-parse','HEAD'], cwd=ROOT, text=True).strip()
    cohesion = score_all(graph, communities)
    graph, relation_audit = restore_parallel_relations(graph, extraction)
    to_json(graph, communities, str(output / 'graph.json'), force=True, community_labels=labels, built_at_commit=commit)
    export_relation_html(graph, communities, output / 'graph.html', labels)
    report = generate(graph, communities, cohesion, labels, gods, surprises, detection,
                      {'input': 0, 'output': 0}, str(ROOT), suggested_questions=questions, built_at_commit=commit)
    report += '\n## 범위와 한계\n\nframework와 standard/framework를 별도 경로로 추출했다. standard 실행은 standard/framework를 사용한다. 같은 이름의 C++ 메서드 간 AST 연결은 정확한 오버로드 해석이 아니다. 문서 순서도는 실제 소스를 추가로 읽어 작성했다. GPU 고정 단계는 C++ 호출 간선이 아니다. 빌드·테스트·브라우저 확인은 사용자에게 맡겼다.\n'
    write(output / 'GRAPH_REPORT.md', report)
    health = diagnose_extraction(extraction, directed=True, root=str(ROOT))
    write(output / 'GRAPH_HEALTH.md', relation_health(extraction, relation_audit))
    dump(output / 'RELATION_REPAIR.json', relation_audit)
    dump(output / 'EXTRACTION.json', extraction)
    dump(output / 'DETECTION.json', detection)
    dump(output / 'ANALYSIS.json', {'gods': gods, 'surprises': surprises, 'questions': questions})
```

### before
```python
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--graph', action='store_true')
    parser.add_argument('--repair-graph', action='store_true', help='Patch only missing include endpoints in the saved graph.')
    args = parser.parse_args()
    if args.repair_graph:
        repair_saved_graph()
    elif args.graph:
        build_graph()
```

### after
```python
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--graph', action='store_true')
    parser.add_argument('--repair-graph', action='store_true', help='Patch only missing include endpoints in the saved graph.')
    parser.add_argument('--repair-relations', action='store_true', help='Restore parallel relation records without reextracting code.')
    args = parser.parse_args()
    if args.repair_relations:
        repair_saved_relations()
    elif args.repair_graph:
        repair_saved_graph()
    elif args.graph:
        build_graph()
```
