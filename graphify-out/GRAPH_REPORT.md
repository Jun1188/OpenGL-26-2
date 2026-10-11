# 정적 구조 분석 안내

- 이 그래프는 코드 107개의 AST를 추출한 결과다. 빌드·실행·테스트는 수행하지 않았다.
- 외부 의존성, 빌드 출력, PDF, 과거 문서는 분석에서 제외했다. 외부 API 호출은 실행되지 않았다.
- 추출 실패 파일은 없다. 노드 2,685개, 방향 간선 4,717개, 구조 묶음 185개다.
- 원시 간선의 미해결 끝점은 270개, 동일 끝점 병합은 93개, 자기 참조는 3개다.
  GRAPH_HEALTH.md에 전체 진단을 저장했다. 그래프가 모든 관계를 보존한다고 보장하지 않는다.
- 서로 다른 실행 프로젝트를 잇는 기호 관계 16개는 AMBIGUOUS로 표시했다.
  같은 이름의 random, Cell 등을 다른 프로젝트의 실제 호출로 단정하지 않는다.
- C++ 전처리 분기와 std::function 콜백 대상은 AST만으로 완전히 해석하지 못한다.
  EXECUTION_FLOW.md에서 프로젝트별 진입점, 활성 분기, 콜백 실행 순서를 소스와 대조했다.
- 그래프 생성의 외부 API 토큰 사용량은 0이다. 이 대화 자체의 토큰 사용량을 뜻하지 않는다.
- 사용자 지침에 따라 빌드·테스트와 graphify benchmark는 실행하지 않았다.

# Graph Report - OpenGL-26-2  (2026-10-11)

## Corpus Check
- 107 files · ~59,463 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 2685 nodes · 4717 edges · 185 communities (166 shown, 12 thin omitted)
- Extraction: 93% EXTRACTED · 7% INFERRED · 0% AMBIGUOUS · INFERRED: 314 edges (avg confidence: 0.84)
- Token cost: 0 input · 0 output

## Community Hubs (Navigation)
- 실습 7~9 입력 상태
- 실습 10 도형 상태
- 실습 10 입력 상태
- 워밍업 2 메시 데이터
- 워밍업 2 메시 검증
- 실습 10 렌더 데이터
- 실습 15 콜백·루프
- 실습 2~6 사각형 운동
- sample0 진입점
- 실습 14 입력 상태
- 실습 7~9 렌더 데이터
- 워밍업 1 콘솔 진입점
- 공용 프레임워크 콜백·루프
- 실습 13 콜백·루프
- 실습 14 콜백·루프
- 실습 10 렌더 데이터
- 실습 15 입력 상태
- 워밍업 2 메시 데이터
- 공용 프레임워크 장면·GPU 변환
- 실습 11 도형 상태
- 실습 7~9 도형 상태
- 실습 13 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 2~6 렌더 데이터
- 워밍업 1 콘솔 진입점
- 실습 11 렌더 데이터
- 실습 10 렌더 데이터
- 실습 7~9 렌더 데이터
- 실습 2~6 사각형 운동
- sample0 메시 검증
- 워밍업 2 검증 진입점
- 공용 프레임워크 장면·GPU 변환
- 실습 11 입력 상태
- 실습 7~9 렌더 데이터
- 실습 13 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 11 렌더 데이터
- 실습 12 입력 상태
- 워밍업 2 보드 상태
- 공용 프레임워크 장면·GPU 변환
- 실습 13 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 2~6 사각형 운동
- 공용 프레임워크 장면·GPU 변환
- 실습 11 입력 상태
- 실습 12 렌더 데이터
- 실습 13 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 2~6 사각형 운동
- 실습 2~6 렌더 데이터
- 워밍업 2 보드 처리
- 워밍업 2 보드 명령
- 워밍업 2 메시 파싱
- 워밍업 1 콘솔 진입점
- 워밍업 1 콘솔 진입점
- 공용 프레임워크 장면·GPU 변환
- 공용 프레임워크 장면·GPU 변환
- 실습 13 장면·GPU 변환
- 실습 13 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 2~6 입력 상태
- 실습 2~6 사각형 운동
- sample0 메시 파싱
- 워밍업 2 콜백·루프
- 워밍업 2 보드 출력
- 워밍업 2 입력 상태
- 실습 12 렌더 데이터
- 실습 12 렌더 데이터
- 실습 2~6 사각형 운동
- 실습 2~6 입력 상태
- 실습 13 입력 상태
- 실습 15 콜백·루프
- 실습 11 렌더 데이터
- 실습 11 렌더 데이터
- 실습 12 도형 상태
- 실습 7~9 입력 상태
- 실습 7~9 진입점
- 실습 15 콜백·루프
- 실습 2~6 입력 상태
- 워밍업 2 콜백·루프
- 공용 프레임워크 콜백·루프
- 실습 11 입력 상태
- 실습 13 콜백·루프
- 실습 14 콜백·루프
- 실습 2~6 진입점
- 실습 2~6 입력 상태
- 공용 프레임워크 장면·GPU 변환
- 공용 프레임워크 장면·GPU 변환
- 실습 11 입력 상태
- 실습 11 진입점
- 실습 10 렌더 데이터
- 실습 13 장면·GPU 변환
- 실습 13 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 공용 프레임워크 콜백·루프
- 실습 12 도형 상태
- 실습 14 콜백·루프
- sample0 메시 데이터
- 공용 프레임워크 장면·GPU 변환
- 실습 12 도형 상태
- 실습 12 진입점
- 실습 13 장면·GPU 변환
- 실습 13 장면·GPU 변환
- 실습 13 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 15 입력 상태
- 실습 15 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 공용 프레임워크 콜백·루프
- 공용 프레임워크 장면·GPU 변환
- 공용 프레임워크 장면·GPU 변환
- 실습 7~9 렌더 데이터
- 실습 13 콜백·루프
- 실습 13 장면·GPU 변환
- 실습 14 콜백·루프
- 실습 14 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 워밍업 1 콘솔 진입점
- 공용 프레임워크 장면·GPU 변환
- 실습 11 렌더 데이터
- 실습 11 렌더 데이터
- 실습 11 입력 상태
- 실습 12 렌더 데이터
- 실습 7~9 렌더 데이터
- 실습 13 콜백·루프
- sample0 메시 데이터
- sample0 메시 데이터
- sample0 메시 데이터
- 워밍업 2 코드 관계
- 공용 프레임워크 장면·GPU 변환
- 공용 프레임워크 장면·GPU 변환
- 실습 10 렌더 데이터
- 실습 10 렌더 데이터
- 실습 7~9 렌더 데이터
- 실습 13 장면·GPU 변환
- 실습 13 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 14 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 15 장면·GPU 변환
- 실습 2~6 입력 상태
- 실습 2~6 입력 상태
- sample0 메시 데이터
- sample0 메시 데이터
- 워밍업 2 검증 진입점
- 실습 12 렌더 데이터
- 실습 13 콜백·루프
- 실습 2~6 입력 상태
- 실습 2~6 사각형 운동
- sample0 메시 데이터
- 실습 11 입력 상태
- 실습 12 입력 상태
- 실습 12 도형 상태
- 실습 10 렌더 데이터
- 실습 11 코드 관계
- 실습 12 코드 관계
- 실습 10 코드 관계
- 실습 7~9 코드 관계
- 실습 13 코드 관계
- 실습 14 코드 관계
- 실습 15 코드 관계
- 실습 2~6 코드 관계
- 워밍업 2 코드 관계
- 워밍업 1 코드 관계
- sample0 코드 관계

## God Nodes (most connected - your core abstractions)
1. `Diagram` - 51 edges
2. `Grid` - 40 edges
3. `Rect` - 40 edges
4. `Application` - 35 edges
5. `DiagramStore` - 35 edges
6. `Application` - 35 edges
7. `Application` - 35 edges
8. `Application` - 35 edges
9. `Diagram` - 34 edges
10. `Scene` - 33 edges

## Surprising Connections (you probably didn't know these)
- `testBoardProperties()` --calls--> `random`  [AMBIGUOUS]
  Computer-Graphics_WarmmingUp2/tests/Tests.cpp → computer_graphics_warmUp1/FileName.cpp
- `mark()` --references--> `Cell`  [AMBIGUOUS]
  Computer-Graphics_WarmmingUp2/BoardRenderer.cpp → openGL_2dAssign11/Console.h
- `BoardFrame` --references--> `Cell`  [AMBIGUOUS]
  Computer-Graphics_WarmmingUp2/BoardRenderer.h → openGL_2dAssign11/Console.h
- `at` --references--> `Cell`  [AMBIGUOUS]
  Computer-Graphics_WarmmingUp2/BoardRenderer.h → openGL_2dAssign11/Console.h
- `FaceMotion` --references--> `Motion`  [AMBIGUOUS]
  openGL_3dAssign_15/Console.h → openGL_2dAssign_789/Console.h

## Import Cycles
- None detected.

## Communities (185 total, 12 thin omitted)

### Community 0 - "실습 7~9 입력 상태"
Cohesion: 0.05
Nodes (74): randomFloat(), boundsAt(), clampPivot(), Console, onKey, onMouseButton, render, Command (+66 more)

### Community 1 - "실습 10 도형 상태"
Cohesion: 0.05
Nodes (75): DiagCaseTable, NumOfEqual, NumOfLine, NumOfRect, NumOfRight, NumOfSpot, rectColumns, rectRows (+67 more)

### Community 2 - "실습 10 입력 상태"
Cohesion: 0.06
Nodes (44): Console, onKey, onMouseButton, render, GLFWwindow, path, Point, GLFWwindow (+36 more)

### Community 3 - "워밍업 2 메시 데이터"
Cohesion: 0.05
Nodes (47): CornerIndex, hasTexture, texture, vertex, Diagnostic, face, line, message (+39 more)

### Community 4 - "워밍업 2 메시 검증"
Cohesion: 0.11
Nodes (33): ParseResult, ResolvedFace, Texture, ValidationResult, Vertex, MeshReporter::print(), printDiagnostic(), printFace() (+25 more)

### Community 5 - "실습 10 렌더 데이터"
Cohesion: 0.07
Nodes (23): Diagram, boardId_, bounds_, color_, direction_, glshape_, id_, localPivot_ (+15 more)

### Community 6 - "실습 15 콜백·루프"
Cohesion: 0.07
Nodes (33): Application, camera_, findShaderDirectory, focusHandler_, glfwInitialized_, initHandler_, initialize, keyHandler_ (+25 more)

### Community 7 - "실습 2~6 사각형 운동"
Cohesion: 0.13
Nodes (30): randomBool(), isDragging, Practice4RectState, calculateDiagonalMovement, calculateOrbitMovement, calculateZigzagMovement, canCreateRectAt, captureIfNeeded (+22 more)

### Community 8 - "sample0 진입점"
Cohesion: 0.12
Nodes (28): Bounds, maximum, minimum, Camera, forward, position, radius, speed (+20 more)

### Community 9 - "실습 14 입력 상태"
Cohesion: 0.11
Nodes (24): array, assignVertexColors(), Console, angularDirection_, applyTransform, cuboidId_, initScene, onFocus (+16 more)

### Community 10 - "실습 7~9 렌더 데이터"
Cohesion: 0.08
Nodes (28): GLsizeiptr, DrawRange, count, first, mode, GLenum, GLint, GLsizei (+20 more)

### Community 11 - "워밍업 1 콘솔 진입점"
Cohesion: 0.19
Nodes (25): ARR, array, determinate(), Matrix, command, eCapture, eOperandCapture, fCapture (+17 more)

### Community 12 - "공용 프레임워크 콜백·루프"
Cohesion: 0.08
Nodes (26): Application, camera_, findShaderDirectory, focusHandler_, glfwInitialized_, initialize, keyHandler_, modifiedKeyHandler_ (+18 more)

### Community 13 - "실습 13 콜백·루프"
Cohesion: 0.08
Nodes (26): Application, camera_, findShaderDirectory, focusHandler_, glfwInitialized_, initialize, keyHandler_, modifiedKeyHandler_ (+18 more)

### Community 14 - "실습 14 콜백·루프"
Cohesion: 0.08
Nodes (26): Application, camera_, findShaderDirectory, focusHandler_, glfwInitialized_, initialize, keyHandler_, modifiedKeyHandler_ (+18 more)

### Community 15 - "실습 10 렌더 데이터"
Cohesion: 0.15
Nodes (25): BoundsCase, GlColor, GLShape, Point, size_t, TriangleCase, vector, cross() (+17 more)

### Community 16 - "실습 15 입력 상태"
Cohesion: 0.16
Nodes (24): Console, applyFaceMotion, applyObjectTransform, beginMotion, cuboidId_, initScene, motion_, onKey (+16 more)

### Community 17 - "워밍업 2 메시 데이터"
Cohesion: 0.14
Nodes (15): Console, vector, path, string, outputDirectory(), toUtf8(), wmain(), string (+7 more)

### Community 18 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.08
Nodes (24): uint64_t, Scene, addBasis, addCuboid, addLine, addPyramid, addRightTetrahedron, addVector (+16 more)

### Community 19 - "실습 11 도형 상태"
Cohesion: 0.14
Nodes (22): Diagram, Point, size_t, DiagramStore, advance, clear, diagrams_, push (+14 more)

### Community 20 - "실습 7~9 도형 상태"
Cohesion: 0.14
Nodes (22): Diagram, Point, size_t, DiagramStore, advance, clear, diagrams_, push (+14 more)

### Community 21 - "실습 13 장면·GPU 변환"
Cohesion: 0.08
Nodes (24): uint64_t, Scene, addBasis, addCuboid, addLine, addPyramid, addRightTetrahedron, addVector (+16 more)

### Community 22 - "실습 14 장면·GPU 변환"
Cohesion: 0.08
Nodes (24): uint64_t, Scene, addBasis, addCuboid, addLine, addPyramid, addRightTetrahedron, addVector (+16 more)

### Community 23 - "실습 15 장면·GPU 변환"
Cohesion: 0.08
Nodes (24): uint64_t, Scene, addBasis, addCuboid, addLine, addPyramid, addRightTetrahedron, addVector (+16 more)

### Community 24 - "실습 2~6 렌더 데이터"
Cohesion: 0.09
Nodes (21): AnimatedRect, diagonalVelocity, growing, isZigzagMove, orbitEdge, orbitReachedBorder, recolorElapsed, rect (+13 more)

### Community 25 - "워밍업 1 콘솔 진입점"
Cohesion: 0.13
Nodes (13): CardGame, attempts, cards, console, height, matched, random, score (+5 more)

### Community 26 - "실습 11 렌더 데이터"
Cohesion: 0.10
Nodes (21): DrawRange, count, first, mode, GLenum, GLint, GLsizei, GLuint (+13 more)

### Community 27 - "실습 10 렌더 데이터"
Cohesion: 0.10
Nodes (22): DrawRange, count, first, mode, GLenum, GLint, GLsizei, GLuint (+14 more)

### Community 28 - "실습 7~9 렌더 데이터"
Cohesion: 0.10
Nodes (18): Diagram, color_, direction_, localPivot_, localVertices_, oldPivots, pivot_, rotationDegrees_ (+10 more)

### Community 29 - "실습 2~6 사각형 운동"
Cohesion: 0.15
Nodes (22): getHeight, getWidth, moveCenterWithin, Animation, emissionElapsed, emitting, source, splitRects (+14 more)

### Community 30 - "sample0 메시 검증"
Cohesion: 0.19
Nodes (21): FaceRecord, Mesh, ParseResult, ResolvedFace, string, Texture, ValidationResult, Vertex (+13 more)

### Community 31 - "워밍업 2 검증 진입점"
Cohesion: 0.28
Nodes (19): runBoard, Board, moveRectangle, rectangles_, reset, resizeBoard, resizeRectangle, Rectangles (+11 more)

### Community 32 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.18
Nodes (21): getComponent, push_entity, array, EntityId, GlColor, LineId, mat3, optional (+13 more)

### Community 33 - "실습 11 입력 상태"
Cohesion: 0.10
Nodes (20): Grid, boardLines_, cells_, effects_, effectSteps_, gridX, gridY, movingRight_ (+12 more)

### Community 34 - "실습 7~9 렌더 데이터"
Cohesion: 0.22
Nodes (20): GlColor, GLShape, Point, vector, cross(), advance, canTranslate, contains (+12 more)

### Community 35 - "실습 13 장면·GPU 변환"
Cohesion: 0.18
Nodes (21): getComponent, push_entity, array, EntityId, GlColor, LineId, mat3, optional (+13 more)

### Community 36 - "실습 14 장면·GPU 변환"
Cohesion: 0.18
Nodes (21): getComponent, push_entity, array, EntityId, GlColor, LineId, mat3, optional (+13 more)

### Community 37 - "실습 15 장면·GPU 변환"
Cohesion: 0.18
Nodes (21): getComponent, push_entity, array, EntityId, GlColor, LineId, mat3, optional (+13 more)

### Community 38 - "실습 11 렌더 데이터"
Cohesion: 0.12
Nodes (15): Diagram, color_, direction_, filled_, localPivot_, localVertices_, pivot_, rotationDegrees_ (+7 more)

### Community 39 - "실습 12 입력 상태"
Cohesion: 0.15
Nodes (16): path, GLFWwindow, DiagramStore, ShaderHandler, uint64_t, PracticeConsole, handleQuit, initGraphics (+8 more)

### Community 40 - "워밍업 2 보드 상태"
Cohesion: 0.15
Nodes (12): areaMessage(), size_t, Point, x, y, Rectangle, fitsInside, Size (+4 more)

### Community 41 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.12
Nodes (18): GLint, GLuint, Renderer, destroy, drawRanges_, ebo_, initBuffer, initialize (+10 more)

### Community 42 - "실습 13 장면·GPU 변환"
Cohesion: 0.12
Nodes (18): GLint, GLuint, Renderer, destroy, drawRanges_, ebo_, initBuffer, initialize (+10 more)

### Community 43 - "실습 14 장면·GPU 변환"
Cohesion: 0.12
Nodes (18): GLint, GLuint, Renderer, destroy, drawRanges_, ebo_, initBuffer, initialize (+10 more)

### Community 44 - "실습 15 장면·GPU 변환"
Cohesion: 0.12
Nodes (18): GLint, GLuint, Renderer, destroy, drawRanges_, ebo_, initBuffer, initialize (+10 more)

### Community 45 - "실습 2~6 사각형 운동"
Cohesion: 0.19
Nodes (19): Rect, color_, getBottom, getLeft, getRight, getTop, maximum_, minimum_ (+11 more)

### Community 46 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.20
Nodes (15): CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix(), Camera, mat4, size_t, Transform3D, DiagramSystem::modelMatrix() (+7 more)

### Community 47 - "실습 11 입력 상태"
Cohesion: 0.22
Nodes (17): checkedDimension(), GLFWwindow, size_t, advanceEffects, changeSpeed, getIsEndPos, getStepDuration, moveByStep (+9 more)

### Community 48 - "실습 12 렌더 데이터"
Cohesion: 0.18
Nodes (17): Diagram, GlColor, GLShape, Point, size_t, vector, Diagram::Diagram(), fitsNdc (+9 more)

### Community 49 - "실습 13 장면·GPU 변환"
Cohesion: 0.20
Nodes (15): CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix(), Camera, mat4, size_t, Transform3D, DiagramSystem::modelMatrix() (+7 more)

### Community 50 - "실습 14 장면·GPU 변환"
Cohesion: 0.20
Nodes (15): CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix(), Camera, mat4, size_t, Transform3D, DiagramSystem::modelMatrix() (+7 more)

### Community 51 - "실습 15 장면·GPU 변환"
Cohesion: 0.20
Nodes (15): CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix(), Camera, mat4, size_t, Transform3D, DiagramSystem::modelMatrix() (+7 more)

### Community 52 - "실습 2~6 사각형 운동"
Cohesion: 0.17
Nodes (18): Bounds, scaleWithin, setRandomColor, Bounds, getSection(), getSectionBounds(), getSectionCenter(), Practice2RectState (+10 more)

### Community 53 - "실습 2~6 렌더 데이터"
Cohesion: 0.18
Nodes (17): Point, GlColor, blue, green, red, set, setRandom, randomFloat() (+9 more)

### Community 54 - "워밍업 2 보드 처리"
Cohesion: 0.20
Nodes (15): ChangeError, Delta, x, y, Board::moveRectangle(), Board::reset(), Board::resizeBoard(), Board::resizeRectangle() (+7 more)

### Community 55 - "워밍업 2 보드 명령"
Cohesion: 0.14
Nodes (16): Binding, delta, description, key, kind, BoardCommand, delta, kind (+8 more)

### Community 56 - "워밍업 2 메시 파싱"
Cohesion: 0.23
Nodes (16): CornerIndex, istream, ParseResult, size_t, string, Texture, TriangleIndices, vector (+8 more)

### Community 57 - "워밍업 1 콘솔 진입점"
Cohesion: 0.18
Nodes (11): Character, insertedStar, value, optional, StringEditor, capitalColor, captures, console (+3 more)

### Community 58 - "워밍업 1 콘솔 진입점"
Cohesion: 0.18
Nodes (11): noExtraInput(), Point, x, y, z, PointList, beforeSort, points (+3 more)

### Community 59 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.12
Nodes (17): DrawRange, count, entityId, faceIndex, firstIndex, lineId, mode, outlineCount (+9 more)

### Community 60 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.12
Nodes (17): GLFace, GLShape, mat4, vector, PlaneFigure, color, independence, localAnimationMatrix (+9 more)

### Community 61 - "실습 13 장면·GPU 변환"
Cohesion: 0.12
Nodes (17): DrawRange, count, entityId, faceIndex, firstIndex, lineId, mode, outlineCount (+9 more)

### Community 62 - "실습 13 장면·GPU 변환"
Cohesion: 0.12
Nodes (17): GLFace, GLShape, mat4, vector, PlaneFigure, color, independence, localAnimationMatrix (+9 more)

### Community 63 - "실습 14 장면·GPU 변환"
Cohesion: 0.12
Nodes (17): DrawRange, count, entityId, faceIndex, firstIndex, lineId, mode, outlineCount (+9 more)

### Community 64 - "실습 14 장면·GPU 변환"
Cohesion: 0.12
Nodes (17): GLFace, GLShape, mat4, vector, PlaneFigure, color, independence, localAnimationMatrix (+9 more)

### Community 65 - "실습 15 장면·GPU 변환"
Cohesion: 0.12
Nodes (17): DrawRange, count, entityId, faceIndex, firstIndex, lineId, mode, outlineCount (+9 more)

### Community 66 - "실습 15 장면·GPU 변환"
Cohesion: 0.12
Nodes (17): GLFace, GLShape, mat4, vector, PlaneFigure, color, independence, localAnimationMatrix (+9 more)

### Community 67 - "실습 2~6 입력 상태"
Cohesion: 0.14
Nodes (17): Point, GLFWwindow, onCursorMove, onMouseButton, Practice3Console, onCursorMove, onKey, onMouseButton (+9 more)

### Community 68 - "실습 2~6 사각형 운동"
Cohesion: 0.18
Nodes (17): Point, optional, size_t, beginDrag, dragTo, Practice5RectState, beginDrag, createdRectCount_ (+9 more)

### Community 69 - "sample0 메시 파싱"
Cohesion: 0.23
Nodes (16): CornerIndex, istream, ParseResult, size_t, string, Texture, TriangleIndices, vector (+8 more)

### Community 70 - "워밍업 2 콜백·루프"
Cohesion: 0.26
Nodes (15): Application::Application(), readRectangles, run, runMesh, Console, path, Rectangles, string (+7 more)

### Community 71 - "워밍업 2 보드 출력"
Cohesion: 0.19
Nodes (15): AxisSegments, begin, count, end, BoardFrame, at, cells, size (+7 more)

### Community 72 - "워밍업 2 입력 상태"
Cohesion: 0.14
Nodes (11): Console, available_, handle_, highlighted_, originalAttributes_, originalInputPage_, originalOutputPage_, Console::clear() (+3 more)

### Community 73 - "실습 12 렌더 데이터"
Cohesion: 0.15
Nodes (11): Diagram, color_, filled_, localVertices_, pivot_, shape_, GLShape, size_t (+3 more)

### Community 74 - "실습 12 렌더 데이터"
Cohesion: 0.13
Nodes (16): DrawRange, count, first, mode, GLenum, GLint, GLsizei, GLuint (+8 more)

### Community 75 - "실습 2~6 사각형 운동"
Cohesion: 0.19
Nodes (16): Index_, optional, RectStore, capture_, captureCurrentState, clear, clearSelection, erase (+8 more)

### Community 76 - "실습 2~6 입력 상태"
Cohesion: 0.22
Nodes (15): Command, GLFWwindow, input, parseKey, Practice2Console::Practice2Console(), input, parseKey, Practice3Console::Practice3Console() (+7 more)

### Community 77 - "실습 13 입력 상태"
Cohesion: 0.18
Nodes (13): Console, cuboidId_, initScene, onKey, pyramidId_, random_, selectFaces, EntityId (+5 more)

### Community 78 - "실습 15 콜백·루프"
Cohesion: 0.18
Nodes (11): Application::findShaderDirectory(), Application::initialize(), Application::setFocusHandler(), Application::setInitHandler(), Application::setKeyHandler(), Application::setModifiedKeyHandler(), Application::setRenderHandler(), Application::setUpdateHandler() (+3 more)

### Community 79 - "실습 11 렌더 데이터"
Cohesion: 0.19
Nodes (14): Diagram, GlColor, GLShape, optional, size_t, vector, Diagram::Diagram(), fitsNdc (+6 more)

### Community 80 - "실습 11 렌더 데이터"
Cohesion: 0.30
Nodes (13): Point, cross(), advance, canTranslate, contains, setDirection, setEffectScale, setPose (+5 more)

### Community 81 - "실습 12 도형 상태"
Cohesion: 0.25
Nodes (13): Bounds, bottom, left, right, top, boundsOf(), advance, checkHit (+5 more)

### Community 82 - "실습 7~9 입력 상태"
Cohesion: 0.14
Nodes (13): Point, Motion, anchor, angle, angularSpeed, radialDirection, radialSpeed, radius (+5 more)

### Community 83 - "실습 7~9 진입점"
Cohesion: 0.24
Nodes (13): GLFWwindow, path, Point, PracticeConsole, unique_ptr, createConsole(), cursorPositionCallback(), findShaderDirectory() (+5 more)

### Community 84 - "실습 15 콜백·루프"
Cohesion: 0.14
Nodes (9): ApplicationConfig, height, shaderDirectory, title, verticalSync, width, GLFWwindow, path (+1 more)

### Community 85 - "실습 2~6 입력 상태"
Cohesion: 0.20
Nodes (14): GlColor, resetAssignedColorAt, renderFilledRect(), renderRectOutline(), setOpenGlColor(), render, renderBase, render (+6 more)

### Community 86 - "워밍업 2 콜백·루프"
Cohesion: 0.15
Nodes (13): Application, boardRenderer_, commandParser_, console_, meshParser_, meshReporter_, meshValidator_, outputDirectory_ (+5 more)

### Community 87 - "공용 프레임워크 콜백·루프"
Cohesion: 0.22
Nodes (9): Application::findShaderDirectory(), Application::setFocusHandler(), Application::setInitHandler(), Application::setKeyHandler(), Application::setModifiedKeyHandler(), Application::setRenderHandler(), Application::setUpdateHandler(), function (+1 more)

### Community 88 - "실습 11 입력 상태"
Cohesion: 0.15
Nodes (9): Console, onKey, onMouseButton, render, Point, Practice11Console, effectTime_, moveTime_ (+1 more)

### Community 89 - "실습 13 콜백·루프"
Cohesion: 0.22
Nodes (9): Application::findShaderDirectory(), Application::setFocusHandler(), Application::setInitHandler(), Application::setKeyHandler(), Application::setModifiedKeyHandler(), Application::setRenderHandler(), Application::setUpdateHandler(), function (+1 more)

### Community 90 - "실습 14 콜백·루프"
Cohesion: 0.22
Nodes (9): Application::findShaderDirectory(), Application::setFocusHandler(), Application::setInitHandler(), Application::setKeyHandler(), Application::setModifiedKeyHandler(), Application::setRenderHandler(), Application::setUpdateHandler(), function (+1 more)

### Community 91 - "실습 2~6 진입점"
Cohesion: 0.23
Nodes (11): GLFWwindow, screenToOpenGl(), Console, GLFWwindow, unique_ptr, createConsole(), cursorPositionCallback(), framebufferSizeCallback() (+3 more)

### Community 92 - "실습 2~6 입력 상태"
Cohesion: 0.15
Nodes (13): BaseColorState, backgroundColor, section1Color, section2Color, section3Color, section4Color, setRandomSections, GlColor (+5 more)

### Community 93 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.18
Nodes (12): Component_Poly, polyhedron, sideLengths, transform, quat, vec3, Transform3D, basis (+4 more)

### Community 94 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.27
Nodes (12): appendFace(), GLFace, GLShape, initializer_list, Polyhedron, vec3, DiagramSystem::makePolyhedron(), finiteVector() (+4 more)

### Community 95 - "실습 11 입력 상태"
Cohesion: 0.18
Nodes (11): path, GLFWwindow, ShaderHandler, PracticeConsole, grid_, initGraphics, render, shader_ (+3 more)

### Community 96 - "실습 11 진입점"
Cohesion: 0.27
Nodes (11): GLFWwindow, path, PracticeConsole, size_t, unique_ptr, createConsole(), findShaderDirectory(), framebufferSizeCallback() (+3 more)

### Community 97 - "실습 10 렌더 데이터"
Cohesion: 0.17
Nodes (11): GlColor, blue, green, red, set, setRandom, VertexBounds, bottom (+3 more)

### Community 98 - "실습 13 장면·GPU 변환"
Cohesion: 0.18
Nodes (12): Component_Poly, polyhedron, sideLengths, transform, quat, vec3, Transform3D, basis (+4 more)

### Community 99 - "실습 13 장면·GPU 변환"
Cohesion: 0.27
Nodes (12): appendFace(), GLFace, GLShape, initializer_list, Polyhedron, vec3, DiagramSystem::makePolyhedron(), finiteVector() (+4 more)

### Community 100 - "실습 14 장면·GPU 변환"
Cohesion: 0.18
Nodes (12): Component_Poly, polyhedron, sideLengths, transform, quat, vec3, Transform3D, basis (+4 more)

### Community 101 - "실습 14 장면·GPU 변환"
Cohesion: 0.27
Nodes (12): appendFace(), GLFace, GLShape, initializer_list, Polyhedron, vec3, DiagramSystem::makePolyhedron(), finiteVector() (+4 more)

### Community 102 - "실습 15 장면·GPU 변환"
Cohesion: 0.18
Nodes (12): Component_Poly, polyhedron, sideLengths, transform, quat, vec3, Transform3D, basis (+4 more)

### Community 103 - "실습 15 장면·GPU 변환"
Cohesion: 0.27
Nodes (12): appendFace(), GLFace, GLShape, initializer_list, Polyhedron, vec3, DiagramSystem::makePolyhedron(), finiteVector() (+4 more)

### Community 104 - "공용 프레임워크 콜백·루프"
Cohesion: 0.18
Nodes (9): ApplicationConfig, height, shaderDirectory, title, verticalSync, width, GLFWwindow, path (+1 more)

### Community 105 - "실습 12 도형 상태"
Cohesion: 0.18
Nodes (11): optional, Point, size_t, RectAttribute, direction, rs, speed, start (+3 more)

### Community 106 - "실습 14 콜백·루프"
Cohesion: 0.18
Nodes (9): ApplicationConfig, height, shaderDirectory, title, verticalSync, width, GLFWwindow, path (+1 more)

### Community 107 - "sample0 메시 데이터"
Cohesion: 0.20
Nodes (11): CornerIndex, hasTexture, texture, vertex, FaceRecord, line, valid, value (+3 more)

### Community 108 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.20
Nodes (10): DiagramSystem, faceModelMatrix, makePolyhedron, modelMatrix, validPolyhedron, validTransform, Component_Poly, PlaneFigure (+2 more)

### Community 109 - "실습 12 도형 상태"
Cohesion: 0.22
Nodes (9): DiagramStore, animationTime_, fixedDiagrams_, rects_, revision_, stackedDiagrams_, Diagram, uint64_t (+1 more)

### Community 110 - "실습 12 진입점"
Cohesion: 0.31
Nodes (9): GLFWwindow, path, PracticeConsole, unique_ptr, createConsole(), findShaderDirectory(), framebufferSizeCallback(), keyCallback() (+1 more)

### Community 111 - "실습 13 장면·GPU 변환"
Cohesion: 0.20
Nodes (7): CameraSystem, projectionMatrix, validCamera, viewMatrix, RenderOptions, depthTest, wireframe

### Community 112 - "실습 13 장면·GPU 변환"
Cohesion: 0.20
Nodes (10): DiagramSystem, faceModelMatrix, makePolyhedron, modelMatrix, validPolyhedron, validTransform, Component_Poly, PlaneFigure (+2 more)

### Community 113 - "실습 13 장면·GPU 변환"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, optional, Line3D, color, end (+2 more)

### Community 114 - "실습 14 장면·GPU 변환"
Cohesion: 0.20
Nodes (7): CameraSystem, projectionMatrix, validCamera, viewMatrix, RenderOptions, depthTest, wireframe

### Community 115 - "실습 14 장면·GPU 변환"
Cohesion: 0.20
Nodes (10): DiagramSystem, faceModelMatrix, makePolyhedron, modelMatrix, validPolyhedron, validTransform, Component_Poly, PlaneFigure (+2 more)

### Community 116 - "실습 14 장면·GPU 변환"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, optional, Line3D, color, end (+2 more)

### Community 117 - "실습 15 장면·GPU 변환"
Cohesion: 0.20
Nodes (7): CameraSystem, projectionMatrix, validCamera, viewMatrix, RenderOptions, depthTest, wireframe

### Community 118 - "실습 15 입력 상태"
Cohesion: 0.20
Nodes (10): FaceMotion, direction, hinge, kind, pair, progress, running, array (+2 more)

### Community 119 - "실습 15 장면·GPU 변환"
Cohesion: 0.20
Nodes (10): DiagramSystem, faceModelMatrix, makePolyhedron, modelMatrix, validPolyhedron, validTransform, Component_Poly, PlaneFigure (+2 more)

### Community 120 - "실습 15 장면·GPU 변환"
Cohesion: 0.20
Nodes (10): GlColor, blue, green, red, optional, Line3D, color, end (+2 more)

### Community 121 - "공용 프레임워크 콜백·루프"
Cohesion: 0.22
Nodes (9): initHandler_, renderHandler_, renderScene, shutdown, updateHandler_, Application::~Application(), Application::initialize(), Application::run() (+1 more)

### Community 122 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.22
Nodes (9): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+1 more)

### Community 123 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.22
Nodes (9): GlColor, blue, green, red, Line3D, color, end, owner (+1 more)

### Community 124 - "실습 7~9 렌더 데이터"
Cohesion: 0.25
Nodes (7): optional, GlColor, blue, green, red, set, setRandom

### Community 125 - "실습 13 콜백·루프"
Cohesion: 0.22
Nodes (9): initHandler_, renderHandler_, renderScene, shutdown, updateHandler_, Application::~Application(), Application::initialize(), Application::run() (+1 more)

### Community 126 - "실습 13 장면·GPU 변환"
Cohesion: 0.22
Nodes (9): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+1 more)

### Community 127 - "실습 14 콜백·루프"
Cohesion: 0.22
Nodes (9): initHandler_, renderHandler_, renderScene, shutdown, updateHandler_, Application::~Application(), Application::initialize(), Application::run() (+1 more)

### Community 128 - "실습 14 장면·GPU 변환"
Cohesion: 0.22
Nodes (9): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+1 more)

### Community 129 - "실습 15 장면·GPU 변환"
Cohesion: 0.22
Nodes (9): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+1 more)

### Community 130 - "워밍업 1 콘솔 진입점"
Cohesion: 0.29
Nodes (6): Console, available, handle, original, HANDLE, WORD

### Community 131 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.25
Nodes (7): CameraSystem, projectionMatrix, validCamera, viewMatrix, RenderOptions, depthTest, wireframe

### Community 132 - "실습 11 렌더 데이터"
Cohesion: 0.32
Nodes (8): compileShader(), GLenum, GLuint, path, string, readTextFile(), destroy, initShader

### Community 133 - "실습 11 렌더 데이터"
Cohesion: 0.25
Nodes (7): GlColor, blue, green, red, set, setRandom, randomFloat()

### Community 134 - "실습 11 입력 상태"
Cohesion: 0.25
Nodes (8): Cell, x, y, Point, cellCenter, Grid::Grid(), resetBoard, size_t

### Community 135 - "실습 12 렌더 데이터"
Cohesion: 0.32
Nodes (8): compileShader(), GLenum, GLuint, path, string, readTextFile(), destroy, initShader

### Community 136 - "실습 7~9 렌더 데이터"
Cohesion: 0.25
Nodes (8): Diagram, GLenum, optional, size_t, drawMode(), indexByteOffset(), render, upload

### Community 137 - "실습 13 콜백·루프"
Cohesion: 0.25
Nodes (8): ApplicationConfig, height, shaderDirectory, title, verticalSync, width, path, string

### Community 138 - "sample0 메시 데이터"
Cohesion: 0.25
Nodes (8): Diagnostic, face, line, message, string, ValidationResult, diagnostics, faces

### Community 139 - "sample0 메시 데이터"
Cohesion: 0.29
Nodes (8): vector, Mesh, faces, textures, vertices, ParseResult, diagnostics, mesh

### Community 140 - "sample0 메시 데이터"
Cohesion: 0.25
Nodes (8): Vertex, x, y, z, VertexRecord, line, valid, value

### Community 141 - "워밍업 2 코드 관계"
Cohesion: 0.48
Nodes (6): board(), check(), frames(), Black-box regression checks; uses only the Python standard library., run(), run_with_output()

### Community 142 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.33
Nodes (7): draw, upload, RenderOptions, Scene, Renderer::draw(), Renderer::render(), Renderer::upload()

### Community 143 - "공용 프레임워크 장면·GPU 변환"
Cohesion: 0.38
Nodes (7): compileShader(), GLenum, GLuint, path, string, readTextFile(), Renderer::initShader()

### Community 144 - "실습 10 렌더 데이터"
Cohesion: 0.38
Nodes (7): compileShader(), GLuint, path, string, readTextFile(), destroy, initShader

### Community 145 - "실습 10 렌더 데이터"
Cohesion: 0.33
Nodes (7): Diagram, GLenum, optional, hasSameShape, makeUnfitCopy, drawMode(), upload

### Community 146 - "실습 7~9 렌더 데이터"
Cohesion: 0.38
Nodes (7): compileShader(), GLuint, path, string, readTextFile(), destroy, initShader

### Community 147 - "실습 13 장면·GPU 변환"
Cohesion: 0.33
Nodes (7): draw, upload, RenderOptions, Scene, Renderer::draw(), Renderer::render(), Renderer::upload()

### Community 148 - "실습 13 장면·GPU 변환"
Cohesion: 0.38
Nodes (7): compileShader(), GLenum, GLuint, path, string, readTextFile(), Renderer::initShader()

### Community 149 - "실습 14 장면·GPU 변환"
Cohesion: 0.33
Nodes (7): draw, upload, RenderOptions, Scene, Renderer::draw(), Renderer::render(), Renderer::upload()

### Community 150 - "실습 14 장면·GPU 변환"
Cohesion: 0.38
Nodes (7): compileShader(), GLenum, GLuint, path, string, readTextFile(), Renderer::initShader()

### Community 151 - "실습 15 장면·GPU 변환"
Cohesion: 0.33
Nodes (7): draw, upload, RenderOptions, Scene, Renderer::draw(), Renderer::render(), Renderer::upload()

### Community 152 - "실습 15 장면·GPU 변환"
Cohesion: 0.38
Nodes (7): compileShader(), GLenum, GLuint, path, string, readTextFile(), Renderer::initShader()

### Community 153 - "실습 2~6 입력 상태"
Cohesion: 0.29
Nodes (5): Console, onCursorMove, onKey, onMouseButton, render

### Community 154 - "실습 2~6 입력 상태"
Cohesion: 0.29
Nodes (7): Practice4Console, onCursorMove, onKey, onMouseButton, rectState_, update, window_

### Community 155 - "sample0 메시 데이터"
Cohesion: 0.29
Nodes (6): MeshParser, parse, MeshReporter, print, MeshValidator, validate

### Community 156 - "sample0 메시 데이터"
Cohesion: 0.29
Nodes (7): Texture, s, t, TextureRecord, line, valid, value

### Community 157 - "워밍업 2 검증 진입점"
Cohesion: 0.33
Nodes (6): size_t, MeshCase, faceCount, input, name, valid

### Community 158 - "실습 12 렌더 데이터"
Cohesion: 0.33
Nodes (5): GlColor, blue, green, red, set

### Community 160 - "실습 2~6 입력 상태"
Cohesion: 0.33
Nodes (6): Practice5Console, onCursorMove, onKey, onMouseButton, rectState_, window_

### Community 161 - "실습 2~6 사각형 운동"
Cohesion: 0.47
Nodes (6): size_t, vector, getRenderOrder, getRenderOrder, findOverlaps, size

### Community 162 - "sample0 메시 데이터"
Cohesion: 0.33
Nodes (6): ResolvedFace, hasTextures, number, textures, vertexIndices, vertices

### Community 163 - "실습 11 입력 상태"
Cohesion: 0.40
Nodes (5): Diagram, vector, effectBy, getRenderItems, moveSelectedTo

### Community 164 - "실습 12 입력 상태"
Cohesion: 0.40
Nodes (4): Console, onKey, render, update

### Community 165 - "실습 12 도형 상태"
Cohesion: 0.40
Nodes (5): Diagram, GlColor, vector, getItems, makeRectangle()

### Community 166 - "실습 10 렌더 데이터"
Cohesion: 0.67
Nodes (3): advance, canTranslate, translate

## Ambiguous Edges - Review These
- `mark()` → `Cell`  [AMBIGUOUS]
  Computer-Graphics_WarmmingUp2/BoardRenderer.cpp · relation: references
- `BoardFrame` → `Cell`  [AMBIGUOUS]
  Computer-Graphics_WarmmingUp2/BoardRenderer.h · relation: references
- `at` → `Cell`  [AMBIGUOUS]
  Computer-Graphics_WarmmingUp2/BoardRenderer.cpp · relation: references
- `testBoardProperties()` → `random`  [AMBIGUOUS]
  Computer-Graphics_WarmmingUp2/tests/Tests.cpp · relation: calls
- `FaceMotion` → `Motion`  [AMBIGUOUS]
  openGL_3dAssign_15/Console.h · relation: references
- `beginMotion` → `Motion`  [AMBIGUOUS]
  openGL_3dAssign_15/Console.cpp · relation: references
- `RectStore` → `Index_`  [AMBIGUOUS]
  openGL_assign123456/RectState.h · relation: references
- `getSelectedIndex` → `Index_`  [AMBIGUOUS]
  openGL_assign123456/RectState.cpp · relation: references
- `findTopmostAt` → `Index_`  [AMBIGUOUS]
  openGL_assign123456/RectState.cpp · relation: references
- `select` → `Index_`  [AMBIGUOUS]
  openGL_assign123456/RectState.cpp · relation: references
- `erase` → `Index_`  [AMBIGUOUS]
  openGL_assign123456/RectState.cpp · relation: references
- `findTopmostOverlap` → `Index_`  [AMBIGUOUS]
  openGL_assign123456/RectState.cpp · relation: references
- `merge` → `Index_`  [AMBIGUOUS]
  openGL_assign123456/RectState.cpp · relation: references
- `splitRectToAnimationAt` → `Index_`  [AMBIGUOUS]
  openGL_assign123456/RectState.cpp · relation: references

## Knowledge Gaps
- **909 isolated node(s):** `console_`, `commandParser_`, `boardRenderer_`, `meshParser_`, `meshValidator_` (+904 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1258 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **12 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **실습 15의 면 회전이 객체 전체 자전과 어떻게 합성되는가?**
  _Console::applyFaceMotion, DiagramSystem::faceModelMatrix, Renderer::draw의 소스를 연결한다._
- **실습 14에서 입력은 쿼터니언과 uMVP까지 어떤 순서로 전달되는가?**
  _등록된 콜백의 나중 실행 조건과 CPU→GPU 경로를 함께 확인한다._
- **면 표시와 행렬 변경은 왜 geometry revision을 바꾸지 않는가?**
  _Scene setter와 Renderer upload/draw의 책임을 비교한다._