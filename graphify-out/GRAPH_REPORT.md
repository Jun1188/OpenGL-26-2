# Graph Report - OpenGL-26-2  (2026-11-02)

## Corpus Check
- Large corpus: 476 files · ~3,148,863 words. Semantic extraction will be expensive (many Claude tokens). Consider running on a subfolder.

## Summary
- 2083 nodes · 3306 edges · 131 communities (90 shown, 12 thin omitted)
- Extraction: 95% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 168 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `1b0e5bd3`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- standard 코드 구조
- framework 코드 구조
- openGL_3dAssign_22 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_16 코드 구조
- openGL_3dAssign_19 코드 구조
- openGL_3dAssign_18 코드 구조
- openGL_3dAssign_17 코드 구조
- openGL_3dAssign_21 코드 구조
- openGL_3dAssign_23 코드 구조
- openGL_3dAssign_15 코드 구조
- openGL_3dAssign_23 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- openGL_sample0 코드 구조
- openGL_3dAssign_23 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_23 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_20 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_21 코드 구조
- openGL_3dAssign_16 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- standard 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- openGL_sample0 코드 구조
- openGL_3dAssign_17 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_22 코드 구조
- openGL_3dAssign_15 코드 구조
- openGL_3dAssign_19 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_23 코드 구조
- openGL_3dAssign_23 코드 구조
- openGL_3dAssign_20 코드 구조
- framework 코드 구조
- openGL_3dAssign_23 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_18 코드 구조
- framework 코드 구조
- openGL_3dAssign_23 코드 구조
- openGL_3dAssign_23 코드 구조
- framework 코드 구조
- openGL_3dAssign_23 코드 구조
- openGL_3dAssign_23 코드 구조
- framework 코드 구조
- openGL_3dAssign_23 코드 구조
- openGL_3dAssign_23 코드 구조
-  코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
-  코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 86 edges
2. `Application` - 75 edges
3. `Scene` - 72 edges
4. `Console` - 43 edges
5. `Console` - 42 edges
6. `Console` - 41 edges
7. `Renderer` - 40 edges
8. `MotionSystem` - 35 edges
9. `PlaneFigure` - 34 edges
10. `Console` - 33 edges

## Surprising Connections (you probably didn't know these)
- `main()` --calls--> `choosePath`  [AMBIGUOUS]
  framework/examples/motion.cpp → standard/Console.h
- `triangulate()` --calls--> `cross()`  [AMBIGUOUS]
  framework/include/Geometry.h → openGL_sample0/MeshValidator.cpp
- `choosePath` --calls--> `append`  [INFERRED]
  standard/Console.h → framework/include/Motion.h
- `circle()` --calls--> `point`  [INFERRED]
  openGL_3dAssign_18/Console.cpp → framework/include/Transform.h
- `main()` --calls--> `setCursorHandler`  [INFERRED]
  openGL_3dAssign_21/main.cpp → framework/include/Application.h

## Import Cycles
- None detected.

## Communities (131 total, 12 thin omitted)

### Community 0 - "framework 코드 구조"
Cohesion: 0.02
Nodes (82): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+74 more)

### Community 1 - "framework 코드 구조"
Cohesion: 0.06
Nodes (80): CullMode, EntityId, PatternSystem::addJoint(), PatternSystem::addRoom(), PatternSystem::cameraFacingWall(), PatternSystem::openRoomWall(), appendFace(), DiagramSystem::makeCone() (+72 more)

### Community 2 - "framework 코드 구조"
Cohesion: 0.03
Nodes (58): Scene::worldMatrix, Scene::setFaceVertexColors, Scene::Scene, Scene::bindEntity, CameraSystem::validCamera, Scene::setVisible, Scene::setFaceTransform, Scene::addCuboid (+50 more)

### Community 3 - "framework 코드 구조"
Cohesion: 0.03
Nodes (57): LayerId, Scene, addBasis, addCone, addCuboid, addGroup, addLine, addModel (+49 more)

### Community 4 - "standard 코드 구조"
Cohesion: 0.05
Nodes (49): Demo, MouseEvent, action, button, framebufferX, framebufferY, mods, windowX (+41 more)

### Community 5 - "framework 코드 구조"
Cohesion: 0.08
Nodes (40): AssetLoader::loadObj(), ObjCorner, normal, position, uv, ObjFace, batch, corners (+32 more)

### Community 6 - "openGL_3dAssign_22 코드 구조"
Cohesion: 0.06
Nodes (44): Console, apply, box, cameraOrbit_, cameraX_, cameraYaw_, cameraZ_, central_ (+36 more)

### Community 7 - "framework 코드 구조"
Cohesion: 0.08
Nodes (42): Motion::append(), Motion::erase(), Motion::fixed(), Motion::followPath(), Motion::insert(), Motion::replace(), Motion::rotation(), Motion::sample() (+34 more)

### Community 8 - "framework 코드 구조"
Cohesion: 0.08
Nodes (32): wglGetProc, T, resolve(), symbol(), Diagnostic, action, api, cause (+24 more)

### Community 9 - "openGL_3dAssign_16 코드 구조"
Cohesion: 0.08
Nodes (43): Console, applyFaceMotion, applyObjectTransform, applyPyramid, beginMotion, beginPyramid, cuboidId_, initScene (+35 more)

### Community 10 - "openGL_3dAssign_19 코드 구조"
Cohesion: 0.06
Nodes (43): Console, apply, box, cameraOrbit_, cameraX_, cameraYaw_, cameraZ_, central_ (+35 more)

### Community 11 - "openGL_3dAssign_18 코드 구조"
Cohesion: 0.06
Nodes (41): RotationMotion, axis, degreesPerSecond, initialDegrees, space, AxisLine, direction, point (+33 more)

### Community 12 - "openGL_3dAssign_17 코드 구조"
Cohesion: 0.09
Nodes (39): Console, apply, begin, elapsed_, initScene, model, motion_, movement_ (+31 more)

### Community 13 - "openGL_3dAssign_21 코드 구조"
Cohesion: 0.09
Nodes (35): BodyObject, body, elastic, entity, releasedRotation, Console, addBall, ballCount_ (+27 more)

### Community 14 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.09
Nodes (33): Bounds3, CharacterBody, CharacterStep, Application::renderViews(), Bounds3::overlaps(), CollisionSystem::jump(), CollisionSystem::randomFreeBox(), CollisionSystem::stepCharacter() (+25 more)

### Community 15 - "openGL_3dAssign_15 코드 구조"
Cohesion: 0.10
Nodes (35): assignVertexColors(), Console, applyFaceMotion, applyObjectTransform, beginMotion, cuboidId_, initScene, motions_ (+27 more)

### Community 16 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.06
Nodes (34): PatternSystem::conveyorCornerPose, PatternSystem::orbitCamera, PatternSystem::rotateJoint, PatternSystem::threeViews, PatternSystem::openRoomWall, Bounds3::contains, PatternSystem::poseWalkingRig, PatternSystem::addJoint (+26 more)

### Community 17 - "framework 코드 구조"
Cohesion: 0.06
Nodes (34): PatternSystem::addNumber, PatternSystem::addWalkingRig, Bounds3::valid, PatternSystem::orbitCamera, PatternSystem::cameraFacingWall, PatternSystem::addPolygon, AnimationClock::advance, Bounds3::contains (+26 more)

### Community 18 - "framework 코드 구조"
Cohesion: 0.15
Nodes (31): CollisionSystem::stepRoomBody(), PatternSystem::addWalkingRig(), PatternSystem::openFace(), PatternSystem::poseWalkingRig(), rigidRoom(), motionAround(), motionPlane(), motionUnitVector() (+23 more)

### Community 19 - "framework 코드 구조"
Cohesion: 0.06
Nodes (33): GLuint, vector, Renderer, destroy, draw, drawRanges_, ebo_, hasTextureLocation_ (+25 more)

### Community 20 - "framework 코드 구조"
Cohesion: 0.06
Nodes (33): MotionSystem::createSpin, MotionSystem::setLayer, Motion::rotation, MotionSystem::bindMotion, MotionSystem::update, MotionSystem::createPathAndSpin, MotionSystem::removeLayer, MotionSystem::reparentLayer (+25 more)

### Community 21 - "framework 코드 구조"
Cohesion: 0.06
Nodes (31): GlfwApi, createWindow, DestroyWindow, GetCurrentContext, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize (+23 more)

### Community 22 - "framework 코드 구조"
Cohesion: 0.14
Nodes (21): main(), pickAtCursor, run, setCursorHandler, setFocusHandler, setInitHandler, setKeyHandler, setModifiedKeyHandler (+13 more)

### Community 23 - "framework 코드 구조"
Cohesion: 0.06
Nodes (32): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+24 more)

### Community 24 - "framework 코드 구조"
Cohesion: 0.09
Nodes (31): CameraSystem, projectionMatrix, validCamera, viewMatrix, Camera, EntityId, optional, size_t (+23 more)

### Community 25 - "framework 코드 구조"
Cohesion: 0.07
Nodes (30): exception_ptr, Application, assets_, callbackError_, camera_, contextFailed_, contextThread_, cursorHandler_ (+22 more)

### Community 26 - "framework 코드 구조"
Cohesion: 0.08
Nodes (30): CullMode, OutlineMode, string, uint32_t, vector, PlaneFigure, color, creaseAngleDegrees (+22 more)

### Community 27 - "framework 코드 구조"
Cohesion: 0.07
Nodes (30): LayerId, map, MotionSystem, bindMotion, clear, createMotion, createOrbit, createOrbitAndSpin (+22 more)

### Community 28 - "framework 코드 구조"
Cohesion: 0.08
Nodes (25): Bounds3, contains, maximum, minimum, overlaps, valid, DistancePath, distances_ (+17 more)

### Community 29 - "openGL_sample0 코드 구조"
Cohesion: 0.10
Nodes (24): optional, string, vector, CornerIndex, hasTexture, texture, vertex, FaceRecord (+16 more)

### Community 30 - "openGL_sample0 코드 구조"
Cohesion: 0.13
Nodes (26): GLenum, Bounds, maximum, minimum, Camera, forward, position, radius (+18 more)

### Community 31 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.10
Nodes (23): Console, activeWall_, body_, bounds_, doors_, initScene, obstacles_, onKey (+15 more)

### Community 32 - "framework 코드 구조"
Cohesion: 0.13
Nodes (21): AxisLine, DistancePath, Bounds3::contains(), PatternSystem::addBox(), PatternSystem::addNumber(), PatternSystem::addPolygon(), PatternSystem::foldFace(), PatternSystem::follow() (+13 more)

### Community 33 - "framework 코드 구조"
Cohesion: 0.08
Nodes (25): PatternSystem, addBox, addJoint, addNumber, addPolygon, addRoom, addWalkingRig, aligned (+17 more)

### Community 34 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.08
Nodes (25): PatternSystem, addBox, addJoint, addNumber, addPolygon, addRoom, addWalkingRig, aligned (+17 more)

### Community 35 - "framework 코드 구조"
Cohesion: 0.14
Nodes (23): appendMesh(), generateTexcoords(), Model, uint32_t, vec2, vec3, mergeCoplanar(), meshOutline() (+15 more)

### Community 36 - "framework 코드 구조"
Cohesion: 0.08
Nodes (23): Application::setUpdateHandler, detail::glfw, detail::glfw, Application::setMouseHandler, Application::renderScene, Application::shutdown, Application::renderViews, Application::run (+15 more)

### Community 37 - "framework 코드 구조"
Cohesion: 0.12
Nodes (17): ApplicationConfig, array, Application::getFramebufferSize(), Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler() (+9 more)

### Community 38 - "framework 코드 구조"
Cohesion: 0.16
Nodes (21): bottomCenter(), bounds(), faceMatrix(), finiteMatrix(), finiteVector(), mat3, mat4, Model (+13 more)

### Community 39 - "framework 코드 구조"
Cohesion: 0.10
Nodes (21): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+13 more)

### Community 40 - "openGL_3dAssign_20 코드 구조"
Cohesion: 0.18
Nodes (18): Console, boxes_, clock_, initScene, leftHeld_, onFocus, onKey, onUpdate (+10 more)

### Community 41 - "openGL_sample0 코드 구조"
Cohesion: 0.25
Nodes (17): Texture, s, t, string, Vertex, cross(), inRange(), isCollinear() (+9 more)

### Community 42 - "framework 코드 구조"
Cohesion: 0.18
Nodes (16): main(), PathSystem, circle, ellipse, linear, polygon, polyline, rectangle (+8 more)

### Community 43 - "framework 코드 구조"
Cohesion: 0.12
Nodes (16): GlColor, blue, green, red, shared_ptr, vec2, Material, color (+8 more)

### Community 44 - "framework 코드 구조"
Cohesion: 0.20
Nodes (11): relativePathInside(), AssetLoader, features_, imageCache_, loadModel, loadObj, loadPrimitive, projectRoot_ (+3 more)

### Community 45 - "framework 코드 구조"
Cohesion: 0.12
Nodes (15): CharacterBody, direction, feet, grounded, halfWidth, height, speed, verticalSpeed (+7 more)

### Community 46 - "openGL_3dAssign_21 코드 구조"
Cohesion: 0.18
Nodes (13): main, Console::onFocus, Console::onMouse, Console::onCursor, Console::onUpdate, Console::printHelp, Console::initScene, Console::addBall (+5 more)

### Community 47 - "openGL_3dAssign_16 코드 구조"
Cohesion: 0.15
Nodes (13): Console::initScene, Console::onUpdate, Console::updatePyramid, Console::selectShape, main, Console::onKey, Console::beginMotion, Console::resetFaces (+5 more)

### Community 48 - "framework 코드 구조"
Cohesion: 0.13
Nodes (14): PathSystem::circle, TransformSystem::reflectLine, PathSystem::rectangle, TransformSystem::reflectPlane, PathSystem::polygon, TransformSystem::reflectPoint, TransformSystem::scaling, PathSystem::linear (+6 more)

### Community 49 - "openGL_sample0 코드 구조"
Cohesion: 0.14
Nodes (14): ResolvedFace, hasTextures, number, textures, vertexIndices, vertices, Vertex, x (+6 more)

### Community 50 - "standard 코드 구조"
Cohesion: 0.19
Nodes (12): Console::onMouse, Console::initGrid, main, Console::printHelp, Console::changeReflection, Console::initRoom, Console::initScene, Console::onUpdate (+4 more)

### Community 51 - "framework 코드 구조"
Cohesion: 0.17
Nodes (13): Camera, PatternSystem::moveCamera(), PatternSystem::screenOnPlane(), PatternSystem::yawCamera(), PatternSystem::zoomCamera(), CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix() (+5 more)

### Community 52 - "framework 코드 구조"
Cohesion: 0.17
Nodes (7): main(), pickAt, requestClose, dispatch(), GLFWwindow, Handler, framebufferSizeCallback()

### Community 53 - "framework 코드 구조"
Cohesion: 0.15
Nodes (12): ApplicationConfig, diagnostics, features, height, projectRoot, runtimeDirectory, shaderDirectory, title (+4 more)

### Community 54 - "framework 코드 구조"
Cohesion: 0.15
Nodes (7): array, EntityId, JointRig, head, hips, root, shoulders

### Community 55 - "framework 코드 구조"
Cohesion: 0.21
Nodes (10): checkCom(), ComApartment, result, ComOwner, value, decodeImage(), T, uint8_t (+2 more)

### Community 56 - "openGL_sample0 코드 구조"
Cohesion: 0.35
Nodes (12): istream, size_t, string, vector, Vertex, MeshParser::parse(), readCorner(), readFace() (+4 more)

### Community 57 - "openGL_sample0 코드 구조"
Cohesion: 0.18
Nodes (13): Diagnostic, face, line, message, string, vector, ParseResult, diagnostics (+5 more)

### Community 58 - "openGL_3dAssign_17 코드 구조"
Cohesion: 0.19
Nodes (11): Console::apply, Console::model, Console::initScene, main, Console::worldPosition, Console::onFocus, Console::reset, Console::begin (+3 more)

### Community 59 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount, outlineFirstIndex (+4 more)

### Community 60 - "framework 코드 구조"
Cohesion: 0.18
Nodes (11): map, mat4, MotionSample, local, parent, MotionState, enabled, motion (+3 more)

### Community 61 - "framework 코드 구조"
Cohesion: 0.18
Nodes (12): MotionId, optional, vector, LayerConfig, enabled, motion, space, transform (+4 more)

### Community 62 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): Motion, append, erase, fixed, followPath, insert, replace, rotation (+4 more)

### Community 63 - "openGL_3dAssign_22 코드 구조"
Cohesion: 0.23
Nodes (10): Console::reset, Console::onUpdate, Console::box, Console::initScene, Console::onFocus, Console::apply, main, Console::recolor (+2 more)

### Community 64 - "openGL_3dAssign_15 코드 구조"
Cohesion: 0.20
Nodes (10): Console::resetAll, main, Console::selectShape, Console::resetFaces, Console::onUpdate, Console::applyFaceMotion, Console::beginMotion, Console::onKey (+2 more)

### Community 65 - "openGL_3dAssign_19 코드 구조"
Cohesion: 0.24
Nodes (9): Console::recolor, Console::onKey, Console::onFocus, Console::box, Console::apply, Console::initScene, Console::reset, main (+1 more)

### Community 66 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): uint8_t, TextureImage, height, rgba, width, shared_ptr, string, TextureResult (+2 more)

### Community 67 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): optional, PivotMode, vector, ModelResult, error, localBasis, model, pivotMode (+2 more)

### Community 68 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): OutlineMode, vec3, LoadOptions, allowObjFallback, customPivot, flipTextureRows, mergeCoplanarFaces, outline (+2 more)

### Community 69 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.20
Nodes (10): CharacterBody, direction, feet, grounded, halfWidth, height, speed, verticalSpeed (+2 more)

### Community 70 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.22
Nodes (9): mat4, vec3, RoomBody, halfExtent, localPosition, localVelocity, released, worldPosition (+1 more)

### Community 71 - "openGL_3dAssign_20 코드 구조"
Cohesion: 0.27
Nodes (8): main, Console::initScene, Console::onFocus, Console::onUpdate, Console::placeMovingObjects, Console::resetState, Console::onKey, Console::zoom

### Community 72 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): PathMotion, durationSeconds, interpolation, phase, playback, rate, relativeToStart, space (+1 more)

### Community 73 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.22
Nodes (8): CharacterStep, blocked, landed, turned, PathPose, position, tangent, yawDegrees

### Community 74 - "framework 코드 구조"
Cohesion: 0.22
Nodes (8): Renderer::initShader, Renderer::destroy, Renderer::render, Renderer::initialize, Renderer::upload, Renderer::renderViews, Renderer::renderPass, Renderer::draw

### Community 75 - "framework 코드 구조"
Cohesion: 0.25
Nodes (8): DiagramSystem, faceModelMatrix, makeCone, makePolyhedron, makeSphere, modelMatrix, validPolyhedron, validTransform

### Community 76 - "openGL_3dAssign_18 코드 구조"
Cohesion: 0.36
Nodes (6): Console::onFocus, main, Console::apply, Console::initScene, Console::onUpdate, Console::onKey

### Community 77 - "framework 코드 구조"
Cohesion: 0.29
Nodes (7): function, MotionStep, durationSeconds, enabled, space, startSeconds, transform

### Community 78 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.29
Nodes (6): DistancePath, distances_, points_, sample, setPoints, vector

### Community 79 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.29
Nodes (7): array, EntityId, JointRig, head, hips, root, shoulders

### Community 80 - "framework 코드 구조"
Cohesion: 0.40
Nodes (6): DistancePath::sample(), PatternSystem::conveyorCornerPose(), DistancePath::sample(), PatternSystem::conveyorCornerPose(), PathPose, Playback

### Community 81 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.33
Nodes (6): Bounds3, contains, maximum, minimum, overlaps, valid

### Community 82 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.47
Nodes (4): main, Console::initScene, Console::onKey, Console::onUpdate

### Community 83 - "framework 코드 구조"
Cohesion: 0.40
Nodes (4): AnimationClock, advance, playing, seconds

### Community 84 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.40
Nodes (4): AnimationClock, advance, playing, seconds

### Community 85 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.40
Nodes (5): CollisionSystem, jump, randomFreeBox, stepCharacter, stepRoomBody

### Community 86 - " 코드 구조"
Cohesion: 0.50
Nodes (4): Component_Poly, DiagramSystem::faceModelMatrix(), Scene::getComponent(), PlaneFigure

### Community 87 - "framework 코드 구조"
Cohesion: 0.50
Nodes (3): DWORD, executableDirectory(), frameworkDirectory()

### Community 88 - "framework 코드 구조"
Cohesion: 0.50
Nodes (4): CharacterStep, blocked, landed, turned

### Community 89 - "framework 코드 구조"
Cohesion: 0.67
Nodes (3): checkGlAt(), GetError, PipelineStage

## Ambiguous Edges - Review These
- `main()` → `choosePath`  [AMBIGUOUS]
  framework/examples/motion.cpp · relation: calls
- `triangulate()` → `cross()`  [AMBIGUOUS]
  framework/include/Geometry.h · relation: calls

## Knowledge Gaps
- **1038 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `SetErrorCallback` (+1033 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1264 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **12 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `main()` and `choosePath`?**
  _Edge tagged AMBIGUOUS (relation: calls) - confidence is low._
- **What is the exact relationship between `triangulate()` and `cross()`?**
  _Edge tagged AMBIGUOUS (relation: calls) - confidence is low._
- **Why does `Application` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `openGL_sample0 코드 구조`?**
  _High betweenness centrality (0.007) - this node is a cross-community bridge._
- **Why does `Scene` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, ` 코드 구조`, `framework 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.004) - this node is a cross-community bridge._
- **Why does `main()` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.001) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _1038 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `framework 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.024096385542168676 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.

## 실습 23 사본과 확인 경계 (2026-11-02)

수정된 openGL_3dAssign_23/framework/detail/Patterns.inl 및 include/Patterns.h를 별도로 포함했다. 다른 과제의 중복 프레임워크 사본은 제외했다. 루트 프레임워크의 같은 이름 메서드를 실습 23의 새 구현과 동일하게 취급하지 않는다. 기존 사용자 미커밋 소스도 분석 입력에 포함되었다. 검증기·그래프 무결성 진단은 실행하지 않았다. 상세 소스 경로와 입력 → 변환 → 출력은 EXECUTION_FLOW.md, 코드 전후는 PRACTICE23.md를 따른다.
