# Graph Report - OpenGL-26-2  (2026-11-02)

## Corpus Check
- Large corpus: 507 files · ~3,555,005 words. Semantic extraction will be expensive (many Claude tokens). Consider running on a subfolder.

## Summary
- 2045 nodes · 3205 edges · 130 communities (88 shown, 12 thin omitted)
- Extraction: 94% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 175 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `dd914ec1`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- standard 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_21 코드 구조
- openGL_3dAssign_22 코드 구조
- framework 코드 구조
- openGL_3dAssign_16 코드 구조
- openGL_3dAssign_19 코드 구조
- openGL_3dAssign_17 코드 구조
- openGL_sample0 코드 구조
- openGL_3dAssign_15 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_18 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_23 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_20 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- standard 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- openGL_3dAssign_21 코드 구조
- openGL_3dAssign_16 코드 구조
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
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_22 코드 구조
- openGL_3dAssign_15 코드 구조
- framework 코드 구조
- openGL_3dAssign_19 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_20 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_18 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_15 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_23 코드 구조
-  코드 구조
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
4. `Console` - 53 edges
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
- `circle()` --calls--> `point`  [INFERRED]
  openGL_3dAssign_18/Console.cpp → framework/include/Transform.h
- `addBall` --calls--> `overlaps`  [INFERRED]
  openGL_3dAssign_21/Console.h → framework/include/Patterns.h
- `main()` --calls--> `run`  [INFERRED]
  openGL_3dAssign_15/main.cpp → framework/include/Application.h

## Import Cycles
- None detected.

## Communities (130 total, 12 thin omitted)

### Community 0 - "framework 코드 구조"
Cohesion: 0.02
Nodes (82): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+74 more)

### Community 1 - "framework 코드 구조"
Cohesion: 0.03
Nodes (58): Scene::worldMatrix, Scene::setFaceVertexColors, Scene::Scene, Scene::bindEntity, CameraSystem::validCamera, Scene::setVisible, Scene::setFaceTransform, Scene::addCuboid (+50 more)

### Community 2 - "framework 코드 구조"
Cohesion: 0.03
Nodes (57): LayerId, Scene, addBasis, addCone, addCuboid, addGroup, addLine, addModel (+49 more)

### Community 3 - "standard 코드 구조"
Cohesion: 0.06
Nodes (51): Demo, append, Console, animation_, changeReflection, choosePath, clock_, demo_ (+43 more)

### Community 4 - "framework 코드 구조"
Cohesion: 0.10
Nodes (49): EntityId, PatternSystem::addBox(), PatternSystem::addJoint(), PatternSystem::addNumber(), PatternSystem::addPolygon(), PatternSystem::addRoom(), PatternSystem::cameraFacingWall(), DiagramSystem::makeCone() (+41 more)

### Community 5 - "framework 코드 구조"
Cohesion: 0.08
Nodes (45): Motion::append(), Motion::erase(), Motion::fixed(), Motion::followPath(), Motion::insert(), Motion::linear(), Motion::pendulum(), Motion::replace() (+37 more)

### Community 6 - "framework 코드 구조"
Cohesion: 0.08
Nodes (41): AssetLoader::loadObj(), ObjCorner, normal, position, uv, ObjFace, batch, corners (+33 more)

### Community 7 - "openGL_3dAssign_21 코드 구조"
Cohesion: 0.07
Nodes (43): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY (+35 more)

### Community 8 - "openGL_3dAssign_22 코드 구조"
Cohesion: 0.06
Nodes (44): Console, apply, box, cameraOrbit_, cameraX_, cameraYaw_, cameraZ_, central_ (+36 more)

### Community 9 - "framework 코드 구조"
Cohesion: 0.08
Nodes (32): checkGlAt(), GetError, wglGetProc, PipelineStage, T, resolve(), symbol(), Diagnostic (+24 more)

### Community 10 - "openGL_3dAssign_16 코드 구조"
Cohesion: 0.08
Nodes (43): Console, applyFaceMotion, applyObjectTransform, applyPyramid, beginMotion, beginPyramid, cuboidId_, initScene (+35 more)

### Community 11 - "openGL_3dAssign_19 코드 구조"
Cohesion: 0.06
Nodes (43): Console, apply, box, cameraOrbit_, cameraX_, cameraYaw_, cameraZ_, central_ (+35 more)

### Community 12 - "openGL_3dAssign_17 코드 구조"
Cohesion: 0.09
Nodes (39): Console, apply, begin, elapsed_, initScene, model, motion_, movement_ (+31 more)

### Community 13 - "openGL_sample0 코드 구조"
Cohesion: 0.06
Nodes (38): CornerIndex, hasTexture, texture, vertex, FaceRecord, line, valid, value (+30 more)

### Community 14 - "openGL_3dAssign_15 코드 구조"
Cohesion: 0.10
Nodes (35): assignVertexColors(), Console, applyFaceMotion, applyObjectTransform, beginMotion, cuboidId_, initScene, motions_ (+27 more)

### Community 15 - "framework 코드 구조"
Cohesion: 0.06
Nodes (35): MotionSystem::setMotionEnabled, MotionSystem::createOrbitAndSpin, MotionSystem::setLayer, Motion::rotation, MotionSystem::clear, MotionSystem::seekMotion, MotionSystem::layerMatrices, Motion::linear (+27 more)

### Community 16 - "framework 코드 구조"
Cohesion: 0.06
Nodes (34): PatternSystem::addNumber, PatternSystem::addWalkingRig, Bounds3::valid, PatternSystem::orbitCamera, PatternSystem::cameraFacingWall, PatternSystem::addPolygon, AnimationClock::advance, Bounds3::contains (+26 more)

### Community 17 - "framework 코드 구조"
Cohesion: 0.08
Nodes (30): AxisLine, Bounds3, Camera, CharacterBody, CharacterStep, Bounds3::contains(), Bounds3::overlaps(), CollisionSystem::jump() (+22 more)

### Community 18 - "framework 코드 구조"
Cohesion: 0.06
Nodes (33): GLuint, vector, Renderer, destroy, draw, drawRanges_, ebo_, hasTextureLocation_ (+25 more)

### Community 19 - "openGL_3dAssign_18 코드 구조"
Cohesion: 0.08
Nodes (32): circle(), Console, apply, initScene, moonAngle_, moonFrames_, moons_, moonSpin_ (+24 more)

### Community 20 - "framework 코드 구조"
Cohesion: 0.06
Nodes (31): GlfwApi, createWindow, DestroyWindow, GetCurrentContext, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize (+23 more)

### Community 21 - "framework 코드 구조"
Cohesion: 0.08
Nodes (32): CullMode, Application::renderViews(), DistancePath::setPoints(), PatternSystem::snakeCells(), Renderer::renderPass(), Renderer::renderViews(), DiagramSystem::makeSphere(), Scene::generateTexcoords() (+24 more)

### Community 22 - "framework 코드 구조"
Cohesion: 0.06
Nodes (32): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+24 more)

### Community 23 - "framework 코드 구조"
Cohesion: 0.09
Nodes (31): CameraSystem, projectionMatrix, validCamera, viewMatrix, Camera, EntityId, optional, size_t (+23 more)

### Community 24 - "framework 코드 구조"
Cohesion: 0.07
Nodes (31): CullMode, OutlineMode, string, uint32_t, vector, PlaneFigure, color, creaseAngleDegrees (+23 more)

### Community 25 - "framework 코드 구조"
Cohesion: 0.15
Nodes (28): DistancePath, CollisionSystem::stepRoomBody(), PatternSystem::addWalkingRig(), PatternSystem::follow(), PatternSystem::openFace(), PatternSystem::poseWalkingRig(), rigidRoom(), motionAround() (+20 more)

### Community 26 - "framework 코드 구조"
Cohesion: 0.07
Nodes (30): exception_ptr, Application, assets_, callbackError_, camera_, contextFailed_, contextThread_, cursorHandler_ (+22 more)

### Community 27 - "framework 코드 구조"
Cohesion: 0.07
Nodes (30): LayerId, map, MotionSystem, bindMotion, clear, createMotion, createOrbit, createOrbitAndSpin (+22 more)

### Community 28 - "openGL_sample0 코드 구조"
Cohesion: 0.12
Nodes (27): GLFWwindow, Bounds, maximum, minimum, Camera, forward, position, radius (+19 more)

### Community 29 - "framework 코드 구조"
Cohesion: 0.14
Nodes (24): appendMesh(), generateTexcoords(), Model, uint32_t, vec2, vec3, vector, mergeCoplanar() (+16 more)

### Community 30 - "framework 코드 구조"
Cohesion: 0.08
Nodes (25): PatternSystem, addBox, addJoint, addNumber, addPolygon, addRoom, addWalkingRig, aligned (+17 more)

### Community 31 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.10
Nodes (22): Console, activeWall_, body_, bounds_, doors_, initScene, obstacles_, onKey (+14 more)

### Community 32 - "framework 코드 구조"
Cohesion: 0.18
Nodes (16): run, setCursorHandler, setFocusHandler, setInitHandler, setModifiedKeyHandler, setMouseHandler, setRenderHandler, setUpdateHandler (+8 more)

### Community 33 - "framework 코드 구조"
Cohesion: 0.11
Nodes (24): bottomCenter(), bounds(), Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint (+16 more)

### Community 34 - "openGL_3dAssign_20 코드 구조"
Cohesion: 0.14
Nodes (21): AnimationClock, advance, playing, seconds, Console, boxes_, clock_, initScene (+13 more)

### Community 35 - "framework 코드 구조"
Cohesion: 0.08
Nodes (23): Application::setUpdateHandler, detail::glfw, detail::glfw, Application::setMouseHandler, Application::renderScene, Application::shutdown, Application::renderViews, Application::run (+15 more)

### Community 36 - "framework 코드 구조"
Cohesion: 0.12
Nodes (17): ApplicationConfig, array, Application::getFramebufferSize(), Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler() (+9 more)

### Community 37 - "framework 코드 구조"
Cohesion: 0.19
Nodes (10): array, vector, optional, string, finiteMatrix(), validColor(), validMesh(), ComOwner (+2 more)

### Community 38 - "framework 코드 구조"
Cohesion: 0.17
Nodes (17): AnimationSystem::addHumanoid(), AnimationSystem::addSwingWithRider(), AnimationSystem::createJointAnimation(), AnimationSystem::poseHierarchy(), AnimationSystem::poseHumanoid(), HumanoidPose::seated(), validJointRest(), PatternSystem::openRoomWall() (+9 more)

### Community 39 - "framework 코드 구조"
Cohesion: 0.16
Nodes (16): main(), PathSystem, circle, ellipse, linear, polygon, polyline, rectangle (+8 more)

### Community 40 - "framework 코드 구조"
Cohesion: 0.11
Nodes (18): HumanoidRig, elbows, feet, forearms, hands, head, hips, knees (+10 more)

### Community 41 - "framework 코드 구조"
Cohesion: 0.12
Nodes (17): PendulumMotion, amplitudeDegrees, axis, offsetDegrees, periodSeconds, phaseDegrees, space, RotationMotion (+9 more)

### Community 42 - "standard 코드 구조"
Cohesion: 0.15
Nodes (15): Console::onRender, Console::onMouse, Console::onKey, Console::initScene, Console::initGrid, Console::initAnimation, Console::printHelp, Console::initRoom (+7 more)

### Community 43 - "framework 코드 구조"
Cohesion: 0.12
Nodes (16): GlColor, blue, green, red, shared_ptr, vec2, Material, color (+8 more)

### Community 44 - "framework 코드 구조"
Cohesion: 0.13
Nodes (15): Bounds3, contains, maximum, minimum, overlaps, valid, mat4, vec3 (+7 more)

### Community 45 - "framework 코드 구조"
Cohesion: 0.20
Nodes (11): relativePathInside(), AssetLoader, features_, imageCache_, loadModel, loadObj, loadPrimitive, projectRoot_ (+3 more)

### Community 46 - "framework 코드 구조"
Cohesion: 0.13
Nodes (15): ApplicationConfig, diagnostics, features, height, projectRoot, runtimeDirectory, shaderDirectory, title (+7 more)

### Community 47 - "openGL_sample0 코드 구조"
Cohesion: 0.32
Nodes (14): string, Vertex, cross(), inRange(), isCollinear(), MeshValidator::validate(), normalizeScale(), resolveFace() (+6 more)

### Community 48 - "openGL_3dAssign_21 코드 구조"
Cohesion: 0.18
Nodes (13): main, Console::addBall, Console::onCursor, Console::initScene, Console::onUpdate, Console::require, Console::printHelp, Console::onMouse (+5 more)

### Community 49 - "openGL_3dAssign_16 코드 구조"
Cohesion: 0.15
Nodes (13): Console::applyObjectTransform, Console::selectShape, Console::applyPyramid, main, Console::resetFaces, Console::initScene, Console::resetAll, Console::beginMotion (+5 more)

### Community 50 - "framework 코드 구조"
Cohesion: 0.13
Nodes (14): PathSystem::circle, TransformSystem::reflectLine, PathSystem::rectangle, TransformSystem::reflectPlane, PathSystem::polygon, TransformSystem::reflectPoint, TransformSystem::scaling, PathSystem::linear (+6 more)

### Community 51 - "framework 코드 구조"
Cohesion: 0.14
Nodes (13): CharacterStep, blocked, landed, turned, CollisionSystem, jump, randomFreeBox, stepCharacter (+5 more)

### Community 52 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): map, function, mat4, MotionSample, local, parent, MotionStep, durationSeconds (+4 more)

### Community 53 - "framework 코드 구조"
Cohesion: 0.18
Nodes (12): finiteVector(), finishLoad(), Model, optional, vector, ModelResult, error, localBasis (+4 more)

### Community 54 - "framework 코드 구조"
Cohesion: 0.15
Nodes (13): Motion, erase, fixed, followPath, insert, linear, pendulum, replace (+5 more)

### Community 55 - "openGL_sample0 코드 구조"
Cohesion: 0.35
Nodes (12): istream, size_t, string, vector, Vertex, MeshParser::parse(), readCorner(), readFace() (+4 more)

### Community 56 - "openGL_sample0 코드 구조"
Cohesion: 0.18
Nodes (13): Diagnostic, face, line, message, string, vector, ParseResult, diagnostics (+5 more)

### Community 57 - "openGL_3dAssign_17 코드 구조"
Cohesion: 0.19
Nodes (11): Console::begin, Console::onFocus, Console::onKey, Console::orbitRotation, Console::onUpdate, Console::worldPosition, Console::model, Console::reset (+3 more)

### Community 58 - "framework 코드 구조"
Cohesion: 0.18
Nodes (10): DWORD, executableDirectory(), frameworkDirectory(), checkCom(), ComApartment, result, decodeImage(), uint8_t (+2 more)

### Community 59 - "framework 코드 구조"
Cohesion: 0.20
Nodes (7): main(), main(), pickAt, pickAtCursor, requestClose, dispatch(), Handler

### Community 60 - "framework 코드 구조"
Cohesion: 0.18
Nodes (12): EntityId, JointAnimation, joint, motion, rest, SwingRig, animation, frame (+4 more)

### Community 61 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount, outlineFirstIndex (+4 more)

### Community 62 - "framework 코드 구조"
Cohesion: 0.17
Nodes (11): FaceId, entityId, faceIndex, EntityId, optional, size_t, Line3D, color (+3 more)

### Community 63 - "framework 코드 구조"
Cohesion: 0.18
Nodes (12): MotionId, optional, vector, LayerConfig, enabled, motion, space, transform (+4 more)

### Community 64 - "openGL_3dAssign_22 코드 구조"
Cohesion: 0.23
Nodes (10): Console::reset, Console::onUpdate, Console::box, Console::initScene, Console::onFocus, Console::apply, main, Console::recolor (+2 more)

### Community 65 - "openGL_3dAssign_15 코드 구조"
Cohesion: 0.20
Nodes (10): Console::onUpdate, Console::selectShape, Console::beginMotion, Console::applyFaceMotion, main, Console::resetAll, Console::applyObjectTransform, Console::onKey (+2 more)

### Community 66 - "framework 코드 구조"
Cohesion: 0.18
Nodes (11): OutlineMode, PivotMode, vec3, LoadOptions, allowObjFallback, customPivot, flipTextureRows, mergeCoplanarFaces (+3 more)

### Community 67 - "openGL_3dAssign_19 코드 구조"
Cohesion: 0.24
Nodes (9): main, Console::apply, Console::onUpdate, Console::initScene, Console::recolor, Console::onKey, Console::box, Console::reset (+1 more)

### Community 68 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): uint8_t, TextureImage, height, rgba, width, shared_ptr, string, TextureResult (+2 more)

### Community 69 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): CharacterBody, direction, feet, grounded, halfWidth, height, speed, verticalSpeed (+2 more)

### Community 70 - "openGL_3dAssign_20 코드 구조"
Cohesion: 0.27
Nodes (8): main, Console::zoom, Console::onUpdate, Console::initScene, Console::onKey, Console::onFocus, Console::resetState, Console::placeMovingObjects

### Community 71 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): vec3, SwingConfig, length, pendulum, person, pivot, riderSwayDegrees, ropeThickness (+1 more)

### Community 72 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): HumanoidPose, elbows, hips, knees, neckDegrees, pelvisDegrees, seated, shoulders (+1 more)

### Community 73 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): PathMotion, durationSeconds, interpolation, phase, playback, rate, relativeToStart, space (+1 more)

### Community 74 - "framework 코드 구조"
Cohesion: 0.22
Nodes (8): Renderer::initShader, Renderer::destroy, Renderer::render, Renderer::initialize, Renderer::upload, Renderer::renderViews, Renderer::renderPass, Renderer::draw

### Community 75 - "framework 코드 구조"
Cohesion: 0.25
Nodes (8): HumanoidConfig, grid, headColor, leftColor, rightColor, torsoColor, upperArmFraction, upperLegFraction

### Community 76 - "framework 코드 구조"
Cohesion: 0.25
Nodes (8): DiagramSystem, faceModelMatrix, makeCone, makePolyhedron, makeSphere, modelMatrix, validPolyhedron, validTransform

### Community 77 - "framework 코드 구조"
Cohesion: 0.25
Nodes (8): vec3, LinearMotion, durationSeconds, from, playback, space, to, Playback

### Community 78 - "openGL_3dAssign_18 코드 구조"
Cohesion: 0.36
Nodes (6): Console::onKey, Console::onFocus, Console::apply, Console::initScene, Console::onUpdate, main

### Community 79 - "framework 코드 구조"
Cohesion: 0.33
Nodes (7): appendFace(), DiagramSystem::makePolyhedron(), DiagramSystem::validPolyhedron(), positiveLengths(), Scene::setSideLengths(), initializer_list, Polyhedron

### Community 80 - "framework 코드 구조"
Cohesion: 0.29
Nodes (6): AnimationSystem, addHumanoid, addSwingWithRider, createJointAnimation, poseHierarchy, poseHumanoid

### Community 81 - "openGL_3dAssign_15 코드 구조"
Cohesion: 0.29
Nodes (3): setKeyHandler, main(), main()

### Community 82 - "framework 코드 구조"
Cohesion: 0.29
Nodes (6): DistancePath, distances_, points_, sample, setPoints, vector

### Community 83 - "framework 코드 구조"
Cohesion: 0.29
Nodes (7): array, EntityId, JointRig, head, hips, root, shoulders

### Community 84 - "framework 코드 구조"
Cohesion: 0.29
Nodes (6): HumanoidPose::seated, AnimationSystem::addSwingWithRider, AnimationSystem::createJointAnimation, AnimationSystem::poseHierarchy, AnimationSystem::addHumanoid, AnimationSystem::poseHumanoid

### Community 85 - "framework 코드 구조"
Cohesion: 0.33
Nodes (6): MotionState, enabled, motion, playing, sampled, timeSeconds

### Community 86 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.47
Nodes (4): Console::initScene, Console::onKey, Console::onUpdate, main

### Community 87 - " 코드 구조"
Cohesion: 0.50
Nodes (4): Component_Poly, DiagramSystem::faceModelMatrix(), Scene::getComponent(), PlaneFigure

## Ambiguous Edges - Review These
- `main()` → `choosePath`  [AMBIGUOUS]
  framework/examples/motion.cpp · relation: calls
- `triangulate()` → `cross()`  [AMBIGUOUS]
  framework/include/Geometry.h · relation: calls

## Knowledge Gaps
- **1023 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `SetErrorCallback` (+1018 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1247 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **12 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `main()` and `choosePath`?**
  _Edge tagged AMBIGUOUS (relation: calls) - confidence is low._
- **What is the exact relationship between `triangulate()` and `cross()`?**
  _Edge tagged AMBIGUOUS (relation: calls) - confidence is low._
- **Why does `Application` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `openGL_3dAssign_15 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `openGL_sample0 코드 구조`?**
  _High betweenness centrality (0.005) - this node is a cross-community bridge._
- **Why does `Console` connect `openGL_3dAssign_23 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.004) - this node is a cross-community bridge._
- **Why does `Scene` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, ` 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.003) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _1023 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `framework 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.024096385542168676 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.
