# Graph Report - OpenGL-26-2  (2026-11-02)

## Corpus Check
- Large corpus: 484 files · ~3,185,311 words. Semantic extraction will be expensive (many Claude tokens). Consider running on a subfolder.

## Summary
- 2031 nodes · 3181 edges · 135 communities (93 shown, 13 thin omitted)
- Extraction: 95% EXTRACTED · 5% INFERRED · 0% AMBIGUOUS · INFERRED: 172 edges (avg confidence: 0.83)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `b246256f`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_21 코드 구조
- framework 코드 구조
- openGL_3dAssign_22 코드 구조
- framework 코드 구조
- openGL_3dAssign_16 코드 구조
- openGL_3dAssign_19 코드 구조
- standard 코드 구조
- openGL_3dAssign_17 코드 구조
- openGL_3dAssign_15 코드 구조
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
- openGL_sample0 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_20 코드 구조
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
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- openGL_3dAssign_21 코드 구조
- openGL_3dAssign_16 코드 구조
- standard 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
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
- openGL_3dAssign_22 코드 구조
- openGL_3dAssign_15 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- openGL_3dAssign_19 코드 구조
- framework 코드 구조
- framework 코드 구조
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
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_sample0 코드 구조
- framework 코드 구조
- framework 코드 구조
- framework 코드 구조
- openGL_3dAssign_23 코드 구조
-  코드 구조
- openGL_3dAssign_15 코드 구조
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
4. `Console` - 46 edges
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

## Communities (135 total, 13 thin omitted)

### Community 0 - "framework 코드 구조"
Cohesion: 0.02
Nodes (82): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+74 more)

### Community 1 - "framework 코드 구조"
Cohesion: 0.03
Nodes (58): Scene::worldMatrix, Scene::setFaceVertexColors, Scene::Scene, Scene::bindEntity, CameraSystem::validCamera, Scene::setVisible, Scene::setFaceTransform, Scene::addCuboid (+50 more)

### Community 2 - "framework 코드 구조"
Cohesion: 0.03
Nodes (57): LayerId, Scene, addBasis, addCone, addCuboid, addGroup, addLine, addModel (+49 more)

### Community 3 - "framework 코드 구조"
Cohesion: 0.09
Nodes (52): EntityId, AnimationSystem::addHumanoid(), AnimationSystem::createJointAnimation(), appendFace(), DiagramSystem::makeCone(), DiagramSystem::makePolyhedron(), DiagramSystem::modelMatrix(), DiagramSystem::validPolyhedron() (+44 more)

### Community 4 - "framework 코드 구조"
Cohesion: 0.08
Nodes (45): Motion::append(), Motion::erase(), Motion::fixed(), Motion::followPath(), Motion::insert(), Motion::linear(), Motion::pendulum(), Motion::replace() (+37 more)

### Community 5 - "openGL_3dAssign_21 코드 구조"
Cohesion: 0.07
Nodes (43): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY (+35 more)

### Community 6 - "framework 코드 구조"
Cohesion: 0.08
Nodes (40): AssetLoader::loadObj(), ObjCorner, normal, position, uv, ObjFace, batch, corners (+32 more)

### Community 7 - "openGL_3dAssign_22 코드 구조"
Cohesion: 0.06
Nodes (44): Console, apply, box, cameraOrbit_, cameraX_, cameraYaw_, cameraZ_, central_ (+36 more)

### Community 8 - "framework 코드 구조"
Cohesion: 0.08
Nodes (32): wglGetProc, T, resolve(), symbol(), Diagnostic, action, api, cause (+24 more)

### Community 9 - "openGL_3dAssign_16 코드 구조"
Cohesion: 0.08
Nodes (43): Console, applyFaceMotion, applyObjectTransform, applyPyramid, beginMotion, beginPyramid, cuboidId_, initScene (+35 more)

### Community 10 - "openGL_3dAssign_19 코드 구조"
Cohesion: 0.06
Nodes (43): Console, apply, box, cameraOrbit_, cameraX_, cameraYaw_, cameraZ_, central_ (+35 more)

### Community 11 - "standard 코드 구조"
Cohesion: 0.07
Nodes (40): Demo, append, Console, animation_, changeReflection, choosePath, clock_, demo_ (+32 more)

### Community 12 - "openGL_3dAssign_17 코드 구조"
Cohesion: 0.09
Nodes (39): Console, apply, begin, elapsed_, initScene, model, motion_, movement_ (+31 more)

### Community 13 - "openGL_3dAssign_15 코드 구조"
Cohesion: 0.10
Nodes (35): assignVertexColors(), Console, applyFaceMotion, applyObjectTransform, beginMotion, cuboidId_, initScene, motions_ (+27 more)

### Community 14 - "framework 코드 구조"
Cohesion: 0.06
Nodes (35): MotionSystem::setMotionEnabled, MotionSystem::createOrbitAndSpin, MotionSystem::setLayer, Motion::rotation, MotionSystem::clear, MotionSystem::seekMotion, MotionSystem::layerMatrices, Motion::linear (+27 more)

### Community 15 - "framework 코드 구조"
Cohesion: 0.06
Nodes (34): PatternSystem::addNumber, PatternSystem::addWalkingRig, Bounds3::valid, PatternSystem::orbitCamera, PatternSystem::cameraFacingWall, PatternSystem::addPolygon, AnimationClock::advance, Bounds3::contains (+26 more)

### Community 16 - "framework 코드 구조"
Cohesion: 0.06
Nodes (33): GLuint, vector, Renderer, destroy, draw, drawRanges_, ebo_, hasTextureLocation_ (+25 more)

### Community 17 - "openGL_3dAssign_18 코드 구조"
Cohesion: 0.08
Nodes (32): circle(), Console, apply, initScene, moonAngle_, moonFrames_, moons_, moonSpin_ (+24 more)

### Community 18 - "framework 코드 구조"
Cohesion: 0.06
Nodes (31): GlfwApi, createWindow, DestroyWindow, GetCurrentContext, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize (+23 more)

### Community 19 - "framework 코드 구조"
Cohesion: 0.11
Nodes (28): Bounds3, CharacterBody, CharacterStep, DistancePath, Bounds3::overlaps(), CollisionSystem::jump(), CollisionSystem::stepCharacter(), CollisionSystem::stepRoomBody() (+20 more)

### Community 20 - "framework 코드 구조"
Cohesion: 0.06
Nodes (32): Camera, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent, position, target (+24 more)

### Community 21 - "framework 코드 구조"
Cohesion: 0.09
Nodes (31): CameraSystem, projectionMatrix, validCamera, viewMatrix, Camera, EntityId, optional, size_t (+23 more)

### Community 22 - "framework 코드 구조"
Cohesion: 0.07
Nodes (31): CullMode, OutlineMode, string, uint32_t, vector, PlaneFigure, color, creaseAngleDegrees (+23 more)

### Community 23 - "framework 코드 구조"
Cohesion: 0.07
Nodes (30): exception_ptr, Application, assets_, callbackError_, camera_, contextFailed_, contextThread_, cursorHandler_ (+22 more)

### Community 24 - "framework 코드 구조"
Cohesion: 0.07
Nodes (30): LayerId, map, MotionSystem, bindMotion, clear, createMotion, createOrbit, createOrbitAndSpin (+22 more)

### Community 25 - "openGL_sample0 코드 구조"
Cohesion: 0.13
Nodes (26): GLenum, Bounds, maximum, minimum, Camera, forward, position, radius (+18 more)

### Community 26 - "framework 코드 구조"
Cohesion: 0.20
Nodes (22): AxisLine, Bounds3::contains(), PatternSystem::snap(), Scene::setPosition(), motionAround(), motionPlane(), motionUnitVector(), PathSystem::circle() (+14 more)

### Community 27 - "framework 코드 구조"
Cohesion: 0.08
Nodes (25): PatternSystem, addBox, addJoint, addNumber, addPolygon, addRoom, addWalkingRig, aligned (+17 more)

### Community 28 - "framework 코드 구조"
Cohesion: 0.09
Nodes (24): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+16 more)

### Community 29 - "openGL_3dAssign_20 코드 구조"
Cohesion: 0.14
Nodes (21): AnimationClock, advance, playing, seconds, Console, boxes_, clock_, initScene (+13 more)

### Community 30 - "framework 코드 구조"
Cohesion: 0.08
Nodes (23): Application::setUpdateHandler, detail::glfw, detail::glfw, Application::setMouseHandler, Application::renderScene, Application::shutdown, Application::renderViews, Application::run (+15 more)

### Community 31 - "framework 코드 구조"
Cohesion: 0.12
Nodes (17): ApplicationConfig, array, Application::getFramebufferSize(), Application::initialize(), Application::pickAt(), Application::pickAtCursor(), Application::run(), Application::setCursorHandler() (+9 more)

### Community 32 - "framework 코드 구조"
Cohesion: 0.13
Nodes (18): relativePathInside(), AssetLoader, features_, imageCache_, loadModel, loadObj, loadPrimitive, projectRoot_ (+10 more)

### Community 33 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): run, setFocusHandler, setInitHandler, setModifiedKeyHandler, setUpdateHandler, main(), main(), main() (+4 more)

### Community 34 - "framework 코드 구조"
Cohesion: 0.10
Nodes (22): finishLoad(), Model, optional, OutlineMode, PivotMode, vec3, vector, LoadOptions (+14 more)

### Community 35 - "openGL_sample0 코드 구조"
Cohesion: 0.14
Nodes (13): DWORD, executableDirectory(), frameworkDirectory(), array, vector, optional, string, MeshParser (+5 more)

### Community 36 - "framework 코드 구조"
Cohesion: 0.11
Nodes (20): CullMode, DistancePath::sample(), PatternSystem::conveyorCornerPose(), Scene::generateTexcoords(), Scene::getMesh(), Scene::setFaceCullMode(), Scene::setFaceLocalMatrix(), Scene::setFaceVisible() (+12 more)

### Community 37 - "framework 코드 구조"
Cohesion: 0.18
Nodes (18): bottomCenter(), bounds(), faceMatrix(), finiteMatrix(), finiteVector(), mat3, mat4, Model (+10 more)

### Community 38 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.11
Nodes (18): Console, activeWall_, body_, bounds_, doors_, obstacles_, open_, random_ (+10 more)

### Community 39 - "framework 코드 구조"
Cohesion: 0.16
Nodes (16): main(), PathSystem, circle, ellipse, linear, polygon, polyline, rectangle (+8 more)

### Community 40 - "framework 코드 구조"
Cohesion: 0.11
Nodes (18): HumanoidRig, elbows, feet, forearms, hands, head, hips, knees (+10 more)

### Community 41 - "framework 코드 구조"
Cohesion: 0.22
Nodes (17): appendMesh(), generateTexcoords(), Model, uint32_t, vec2, vec3, vector, mergeCoplanar() (+9 more)

### Community 42 - "framework 코드 구조"
Cohesion: 0.14
Nodes (17): Application::renderViews(), CollisionSystem::randomFreeBox(), DistancePath::setPoints(), PatternSystem::snakeCells(), Renderer::renderPass(), Renderer::renderViews(), DiagramSystem::makeSphere(), Scene::setMeshVertices() (+9 more)

### Community 43 - "framework 코드 구조"
Cohesion: 0.12
Nodes (17): PendulumMotion, amplitudeDegrees, axis, offsetDegrees, periodSeconds, phaseDegrees, space, RotationMotion (+9 more)

### Community 44 - "framework 코드 구조"
Cohesion: 0.15
Nodes (9): main(), main(), pickAt, pickAtCursor, requestClose, dispatch(), GLFWwindow, Handler (+1 more)

### Community 45 - "openGL_sample0 코드 구조"
Cohesion: 0.32
Nodes (14): string, Vertex, cross(), inRange(), isCollinear(), MeshValidator::validate(), normalizeScale(), resolveFace() (+6 more)

### Community 46 - "openGL_3dAssign_21 코드 구조"
Cohesion: 0.18
Nodes (13): Console::printHelp, Console::roomWorld, Console::onUpdate, Console::releaseBody, Console::addBall, Console::syncScene, Console::onCursor, Console::initScene (+5 more)

### Community 47 - "openGL_3dAssign_16 코드 구조"
Cohesion: 0.15
Nodes (13): Console::applyObjectTransform, Console::selectShape, Console::applyPyramid, main, Console::resetFaces, Console::initScene, Console::resetAll, Console::beginMotion (+5 more)

### Community 48 - "standard 코드 구조"
Cohesion: 0.17
Nodes (13): Console::initGrid, Console::initAnimation, Console::onUpdate, Console::onKey, Console::initScene, Console::initRoom, Console::printHelp, Console::onRender (+5 more)

### Community 49 - "framework 코드 구조"
Cohesion: 0.13
Nodes (14): PathSystem::circle, TransformSystem::reflectLine, PathSystem::rectangle, TransformSystem::reflectPlane, PathSystem::polygon, TransformSystem::reflectPoint, TransformSystem::scaling, PathSystem::linear (+6 more)

### Community 50 - "framework 코드 구조"
Cohesion: 0.15
Nodes (14): Camera, PatternSystem::moveCamera(), PatternSystem::orbitCamera(), PatternSystem::screenOnPlane(), PatternSystem::threeViews(), PatternSystem::yawCamera(), PatternSystem::zoomCamera(), Renderer::draw() (+6 more)

### Community 51 - "framework 코드 구조"
Cohesion: 0.14
Nodes (13): CharacterStep, blocked, landed, turned, CollisionSystem, jump, randomFreeBox, stepCharacter (+5 more)

### Community 52 - "openGL_sample0 코드 구조"
Cohesion: 0.14
Nodes (14): ResolvedFace, hasTextures, number, textures, vertexIndices, vertices, Vertex, x (+6 more)

### Community 53 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): map, function, mat4, MotionSample, local, parent, MotionStep, durationSeconds (+4 more)

### Community 54 - "framework 코드 구조"
Cohesion: 0.15
Nodes (12): shared_ptr, size_t, uint8_t, Material, color, repeatTexture, texture, useTexture (+4 more)

### Community 55 - "framework 코드 구조"
Cohesion: 0.15
Nodes (13): Motion, erase, fixed, followPath, insert, linear, pendulum, replace (+5 more)

### Community 56 - "framework 코드 구조"
Cohesion: 0.21
Nodes (10): checkCom(), ComApartment, result, ComOwner, value, decodeImage(), T, uint8_t (+2 more)

### Community 57 - "openGL_sample0 코드 구조"
Cohesion: 0.35
Nodes (12): istream, size_t, string, vector, Vertex, MeshParser::parse(), readCorner(), readFace() (+4 more)

### Community 58 - "openGL_sample0 코드 구조"
Cohesion: 0.18
Nodes (13): Diagnostic, face, line, message, string, vector, ParseResult, diagnostics (+5 more)

### Community 59 - "openGL_3dAssign_17 코드 구조"
Cohesion: 0.19
Nodes (11): Console::begin, Console::onFocus, Console::onKey, Console::orbitRotation, Console::onUpdate, Console::worldPosition, Console::model, Console::reset (+3 more)

### Community 60 - "framework 코드 구조"
Cohesion: 0.18
Nodes (12): EntityId, JointAnimation, joint, motion, rest, SwingRig, animation, frame (+4 more)

### Community 61 - "framework 코드 구조"
Cohesion: 0.17
Nodes (12): DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount, outlineFirstIndex (+4 more)

### Community 62 - "framework 코드 구조"
Cohesion: 0.18
Nodes (12): MotionId, optional, vector, LayerConfig, enabled, motion, space, transform (+4 more)

### Community 63 - "openGL_3dAssign_22 코드 구조"
Cohesion: 0.23
Nodes (10): Console::initScene, Console::box, Console::reset, Console::recolor, Console::onKey, Console::onFocus, Console::onRender, Console::apply (+2 more)

### Community 64 - "openGL_3dAssign_15 코드 구조"
Cohesion: 0.20
Nodes (10): Console::onUpdate, Console::selectShape, Console::beginMotion, Console::applyFaceMotion, main, Console::resetAll, Console::applyObjectTransform, Console::onKey (+2 more)

### Community 65 - "framework 코드 구조"
Cohesion: 0.24
Nodes (10): Bounds3, contains, maximum, minimum, overlaps, valid, initScene, onKey (+2 more)

### Community 66 - "openGL_sample0 코드 구조"
Cohesion: 0.20
Nodes (11): CornerIndex, hasTexture, texture, vertex, FaceRecord, line, valid, value (+3 more)

### Community 67 - "openGL_3dAssign_19 코드 구조"
Cohesion: 0.24
Nodes (9): main, Console::apply, Console::onUpdate, Console::initScene, Console::recolor, Console::onKey, Console::box, Console::reset (+1 more)

### Community 68 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): AnimationSystem::addSwingWithRider(), AnimationSystem::poseHierarchy(), AnimationSystem::poseHumanoid(), HumanoidPose::seated(), HumanoidPose, HumanoidRig, JointAnimation, SwingConfig (+1 more)

### Community 69 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): ApplicationConfig, diagnostics, features, height, projectRoot, runtimeDirectory, shaderDirectory, title (+2 more)

### Community 70 - "framework 코드 구조"
Cohesion: 0.20
Nodes (10): CharacterBody, direction, feet, grounded, halfWidth, height, speed, verticalSpeed (+2 more)

### Community 71 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): mat4, vec3, RoomBody, halfExtent, localPosition, localVelocity, released, worldPosition (+1 more)

### Community 72 - "openGL_3dAssign_20 코드 구조"
Cohesion: 0.27
Nodes (8): Console::zoom, Console::placeMovingObjects, Console::resetState, Console::initScene, Console::onUpdate, Console::onFocus, Console::onKey, main

### Community 73 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): vec3, SwingConfig, length, pendulum, person, pivot, riderSwayDegrees, ropeThickness (+1 more)

### Community 74 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): HumanoidPose, elbows, hips, knees, neckDegrees, pelvisDegrees, seated, shoulders (+1 more)

### Community 75 - "framework 코드 구조"
Cohesion: 0.22
Nodes (9): PathMotion, durationSeconds, interpolation, phase, playback, rate, relativeToStart, space (+1 more)

### Community 76 - "framework 코드 구조"
Cohesion: 0.22
Nodes (8): Renderer::initShader, Renderer::destroy, Renderer::render, Renderer::initialize, Renderer::upload, Renderer::renderViews, Renderer::renderPass, Renderer::draw

### Community 77 - "framework 코드 구조"
Cohesion: 0.25
Nodes (8): HumanoidConfig, grid, headColor, leftColor, rightColor, torsoColor, upperArmFraction, upperLegFraction

### Community 78 - "framework 코드 구조"
Cohesion: 0.25
Nodes (8): DiagramSystem, faceModelMatrix, makeCone, makePolyhedron, makeSphere, modelMatrix, validPolyhedron, validTransform

### Community 79 - "framework 코드 구조"
Cohesion: 0.25
Nodes (8): vec3, LinearMotion, durationSeconds, from, playback, space, to, Playback

### Community 80 - "openGL_3dAssign_18 코드 구조"
Cohesion: 0.36
Nodes (6): Console::onKey, Console::onFocus, Console::apply, Console::initScene, Console::onUpdate, main

### Community 81 - "framework 코드 구조"
Cohesion: 0.29
Nodes (6): AnimationSystem, addHumanoid, addSwingWithRider, createJointAnimation, poseHierarchy, poseHumanoid

### Community 82 - "framework 코드 구조"
Cohesion: 0.29
Nodes (5): setCursorHandler, setMouseHandler, setRenderHandler, main(), main()

### Community 83 - "framework 코드 구조"
Cohesion: 0.29
Nodes (7): UVOptions, axis, mapping, offset, rotationDegrees, scale, UVMapping

### Community 84 - "framework 코드 구조"
Cohesion: 0.29
Nodes (6): DistancePath, distances_, points_, sample, setPoints, vector

### Community 85 - "framework 코드 구조"
Cohesion: 0.29
Nodes (7): array, EntityId, JointRig, head, hips, root, shoulders

### Community 86 - "openGL_sample0 코드 구조"
Cohesion: 0.29
Nodes (7): Texture, s, t, TextureRecord, line, valid, value

### Community 87 - "framework 코드 구조"
Cohesion: 0.29
Nodes (6): AnimationSystem::poseHierarchy, AnimationSystem::poseHumanoid, AnimationSystem::addSwingWithRider, HumanoidPose::seated, AnimationSystem::createJointAnimation, AnimationSystem::addHumanoid

### Community 88 - "framework 코드 구조"
Cohesion: 0.33
Nodes (6): vec2, Vertex, color, normal, position, uv

### Community 89 - "framework 코드 구조"
Cohesion: 0.33
Nodes (6): MotionState, enabled, motion, playing, sampled, timeSeconds

### Community 90 - "openGL_3dAssign_23 코드 구조"
Cohesion: 0.47
Nodes (4): main, Console::initScene, Console::onKey, Console::onUpdate

### Community 91 - " 코드 구조"
Cohesion: 0.50
Nodes (4): Component_Poly, DiagramSystem::faceModelMatrix(), Scene::getComponent(), PlaneFigure

### Community 93 - "framework 코드 구조"
Cohesion: 0.67
Nodes (3): checkGlAt(), GetError, PipelineStage

## Ambiguous Edges - Review These
- `main()` → `choosePath`  [AMBIGUOUS]
  framework/examples/motion.cpp · relation: calls
- `triangulate()` → `cross()`  [AMBIGUOUS]
  framework/include/Geometry.h · relation: calls

## Knowledge Gaps
- **1015 isolated node(s):** `handle_`, `module`, `ready`, `Init`, `SetErrorCallback` (+1010 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 1238 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **13 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **What is the exact relationship between `main()` and `choosePath`?**
  _Edge tagged AMBIGUOUS (relation: calls) - confidence is low._
- **What is the exact relationship between `triangulate()` and `cross()`?**
  _Edge tagged AMBIGUOUS (relation: calls) - confidence is low._
- **Why does `Application` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `openGL_sample0 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `openGL_3dAssign_15 코드 구조`?**
  _High betweenness centrality (0.005) - this node is a cross-community bridge._
- **Why does `Console` connect `openGL_3dAssign_23 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.004) - this node is a cross-community bridge._
- **Why does `Scene` connect `framework 코드 구조` to `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, `framework 코드 구조`, ` 코드 구조`, `framework 코드 구조`?**
  _High betweenness centrality (0.003) - this node is a cross-community bridge._
- **What connects `handle_`, `module`, `ready` to the rest of the system?**
  _1015 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `framework 코드 구조` be split into smaller, more focused modules?**
  _Cohesion score 0.024096385542168676 - nodes in this community are weakly interconnected._
## 해석 범위

코드 AST와 소스 정의·콜백 등록 위치를 분석했다. 빌드·테스트·실행은 하지 않았다. SDK와 과제별 프레임워크 복사본은 제외했다. 프로젝트별 실제 컴파일은 동일한 배포본을 사용한다. 동적 디스패치와 같은 이름의 메서드 연결은 소스와 대조해야 한다.
