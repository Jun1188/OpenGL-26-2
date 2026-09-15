# Graph Report - openGL_sample0  (2026-09-08)

## Corpus Check
- Corpus is ~6,011 words - fits in a single context window. You may not need a graph.

## Summary
- 11 nodes · 12 edges · 3 communities (1 shown, 1 thin omitted)
- Extraction: 100% EXTRACTED · 0% INFERRED · 0% AMBIGUOUS
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- FreeGLUT 호환 계층
- Visual Studio 프로젝트

## God Nodes (most connected - your core abstractions)
1. `openGL_sample0` - 2 edges
2. `glutInit_ATEXIT_HACK()` - 2 edges
3. `glutCreateWindow_ATEXIT_HACK()` - 2 edges
4. `glutCreateMenu_ATEXIT_HACK()` - 2 edges

## Surprising Connections (you probably didn't know these)
- None detected - all connections are within the same source files.

## Import Cycles
- None detected.

## Communities (3 total, 1 thin omitted)

### Community 0 - "FreeGLUT 호환 계층"
Cohesion: 0.47
Nodes (4): FGUNUSED, glutCreateMenu_ATEXIT_HACK(), glutCreateWindow_ATEXIT_HACK(), glutInit_ATEXIT_HACK()

## Knowledge Gaps
- **1 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Not enough signal to generate questions. This usually means the corpus has no AMBIGUOUS edges, no bridge nodes, no INFERRED relationships, and all communities are tightly cohesive. Add more files or run with --mode deep to extract richer edges._