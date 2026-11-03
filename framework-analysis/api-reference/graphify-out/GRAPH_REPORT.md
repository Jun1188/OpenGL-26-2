# Graph Report - OpenGL-26-2  (2026-11-03)

## Corpus Check
- 69 files · ~68,248 words
- Verdict: corpus is large enough that graph structure adds value.

## Summary
- 3528 nodes · 5357 edges · 238 communities (182 shown, 20 thin omitted)
- Extraction: 94% EXTRACTED · 6% INFERRED · 0% AMBIGUOUS · INFERRED: 309 edges (avg confidence: 0.85)
- Token cost: 0 input · 0 output

## Graph Freshness
- Built from commit: `e0d5fc25`
- Run `git rev-parse HEAD` and compare to check if the graph is stale.
- Run `graphify update .` after code changes (no API cost).

## Community Hubs (Navigation)
- Native 관계
- Native 관계
- ObjLoader 관계
- Model 관계
- ModelLoader 관계
- RenderConfig 관계
- CuboidMode 관계
- Model 관계
- ModelLoader 관계
- RenderConfig 관계
- Model 관계
- Texture 관계
- Diagnostics 관계
- Model 관계
- Texture 관계
- Animation 관계
- Animation 관계
- Application 관계
- Model 관계
- Model 관계
- ModelLoader 관계
- Patterns 관계
- RenderConfig 관계
- Animation 관계
- Animation 관계
- Application 관계
- Model 관계
- Model 관계
- ModelLoader 관계
- Patterns 관계
- RenderConfig 관계
- Animation 관계
- Motion 관계
- Animation 관계
- Animation 관계
- Animation 관계
- Application 관계
- Model 관계
- Application 관계
- Model 관계
- Patterns 관계
- Patterns 관계
- RenderConfig 관계
- Console 관계
- CuboidMode 관계
- Patterns 관계
- Patterns 관계
- RenderConfig 관계
- Model 관계
- Model 관계
-  관계
- RenderConfig 관계
- Model 관계
- Model 관계
-  관계
- RenderConfig 관계
- Console 관계
- Patterns 관계
- RenderConfig 관계
- Patterns 관계
- Patterns 관계
- RenderConfig 관계
- Texture 관계
- Patterns 관계
- CuboidMode 관계
- Texture 관계
- ModelLoader 관계
- ModelLoader 관계
- Transform 관계
- Motion 관계
- CuboidMode 관계
- Graphics 관계
- Graphics 관계
- Motion 관계
-  관계
- Motion 관계
- Graphics 관계
- Transform 관계
- Picking 관계
- Actions 관계
- Picking 관계
- Native 관계
- Native 관계
- Model 관계
- Patterns 관계
- Model 관계
- Animation 관계
- Motion 관계
- Native 관계
- Motion 관계
- CuboidMode 관계
- CuboidMode 관계
- Patterns 관계
- Patterns 관계
- Patterns 관계
- Patterns 관계
- Graphics 관계
- Graphics 관계
- Motion 관계
- Motion 관계
- Native 관계
- Animation 관계
- Animation 관계
- RenderConfig 관계
- Graphics 관계
- Patterns 관계
- Animation 관계
- Application 관계
- RenderConfig 관계
- Application 관계
- Diagnostics 관계
- ModelLoader 관계
- Motion 관계
- CuboidMode 관계
- Diagnostics 관계
- ModelLoader 관계
- ObjLoader 관계
- Graphics 관계
- Graphics 관계
- Graphics 관계
- Graphics 관계
- Graphics 관계
-  관계
-  관계
- Motion 관계
- Patterns 관계
- Patterns 관계
- Model 관계
- Application 관계
- Model 관계
- Application 관계
- Application 관계
- Scene 관계
- Scene 관계
- Application 관계
- Application 관계
-  관계
- Application 관계
- Application 관계
- Graphics 관계
- CuboidMode 관계
- Graphics 관계
- CuboidMode 관계
- Scene 관계
- Motion 관계
- Scene 관계
- Scene 관계
- Scene 관계
- Scene 관계
- CuboidMode 관계
- Scene 관계
- Renderer 관계
- Motion 관계
- Scene 관계
- Scene 관계
- Patterns 관계
- Patterns 관계
- Patterns 관계
- Patterns 관계
- Renderer 관계
- Application 관계
- Application 관계
- Patterns 관계
-  관계
- Motion 관계
- Patterns 관계
-  관계
- CuboidMode 관계
- CuboidMode 관계
- Renderer 관계
- Renderer 관계
- Motion 관계
- Animation 관계
- Animation 관계
- Motion 관계
- Shaders 관계
- basic 관계
- ModelLoader 관계
- ObjLoader 관계
- Patterns 관계
- basic 관계
- GridLayout 관계
- ObjLoader 관계
- ModelLoader 관계
- motion 관계
- PrimitiveLoader 관계
- motion 관계
- GridLayout 관계
- PrimitiveLoader 관계
- assets 관계
- Patterns 관계
- Shaders 관계
- assets 관계
- Scene 관계
- Application 관계
- Application 관계
- Scene 관계
- Console 관계
- Transform 관계
- Transform 관계
-  관계
- Console 관계

## God Nodes (most connected - your core abstractions)
1. `GlApi` - 90 edges
2. `GlApi` - 90 edges
3. `Scene` - 70 edges
4. `Scene` - 70 edges
5. `Application` - 53 edges
6. `Application` - 53 edges
7. `Console` - 53 edges
8. `Renderer` - 41 edges
9. `Renderer` - 41 edges
10. `PlaneFigure` - 39 edges

## Surprising Connections (you probably didn't know these)
- `main()` --calls--> `choosePath`  [INFERRED]
  framework/examples/motion.cpp → standard/Console.h
- `MotionSystem::bindMotion()` --calls--> `setLayer`  [INFERRED]
  framework/detail/Motion.cpp → framework/include/Motion.h
- `MotionSystem::popLayer()` --calls--> `removeLayer`  [INFERRED]
  framework/detail/Motion.cpp → framework/include/Motion.h
- `Scene::addVector()` --calls--> `addLine`  [INFERRED]
  framework/detail/Scene.cpp → framework/include/Graphics.h
- `Scene::worldMatrix()` --calls--> `getEntityLayer`  [INFERRED]
  framework/detail/Scene.cpp → framework/include/Graphics.h

## Import Cycles
- None detected.

## Communities (238 total, 20 thin omitted)

### Community 0 - "Native 관계"
Cohesion: 0.02
Nodes (86): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+78 more)

### Community 1 - "Native 관계"
Cohesion: 0.02
Nodes (86): GlApi, ActiveTexture, AttachShader, BindBuffer, BindTexture, BindVertexArray, BufferData, Clear (+78 more)

### Community 10 - "ObjLoader 관계"
Cohesion: 0.07
Nodes (49): ObjCorner, ObjFace, ObjParseError, UVOptions, AssetLoader::loadObj(), objInteger(), objNumbers(), objResolve() (+41 more)

### Community 101 - "Model 관계"
Cohesion: 0.17
Nodes (11): FaceId, Line3D, entityId, faceIndex, EntityId, optional, size_t, color (+3 more)

### Community 102 - "ModelLoader 관계"
Cohesion: 0.18
Nodes (12): ModelResult, finishLoad(), Model, optional, vec3, vector, error, localBasis (+4 more)

### Community 103 - "RenderConfig 관계"
Cohesion: 0.17
Nodes (12): UniformBindings, hasTexture, lightColor, lightDirection, lighting, model, mvp, normal (+4 more)

### Community 107 - "CuboidMode 관계"
Cohesion: 0.17
Nodes (12): CuboidCommand, degrees, enabled, layer, motion, operation, transform, value (+4 more)

### Community 108 - "Model 관계"
Cohesion: 0.17
Nodes (11): FaceId, Line3D, entityId, faceIndex, EntityId, optional, size_t, color (+3 more)

### Community 109 - "ModelLoader 관계"
Cohesion: 0.18
Nodes (12): ModelResult, finishLoad(), Model, optional, vec3, vector, error, localBasis (+4 more)

### Community 110 - "RenderConfig 관계"
Cohesion: 0.17
Nodes (12): UniformBindings, hasTexture, lightColor, lightDirection, lighting, model, mvp, normal (+4 more)

### Community 112 - "Model 관계"
Cohesion: 0.22
Nodes (11): Vertex, bottomCenter(), bounds(), Model, pair, vec2, vec3, color (+3 more)

### Community 113 - "Texture 관계"
Cohesion: 0.21
Nodes (10): ComApartment, checkCom(), decodeImage(), result, HRESULT, path, TextureImage, uint8_t (+2 more)

### Community 115 - "Diagnostics 관계"
Cohesion: 0.18
Nodes (11): Diagnostic, action, api, cause, file, line, object, operation (+3 more)

### Community 116 - "Model 관계"
Cohesion: 0.22
Nodes (11): Vertex, bottomCenter(), bounds(), Model, pair, vec2, vec3, color (+3 more)

### Community 117 - "Texture 관계"
Cohesion: 0.24
Nodes (9): ComApartment, checkCom(), decodeImage(), result, HRESULT, path, TextureImage, uint8_t (+1 more)

### Community 118 - "Animation 관계"
Cohesion: 0.20
Nodes (10): HumanoidConfig, GlColor, GridLayout, grid, headColor, leftColor, rightColor, torsoColor (+2 more)

### Community 119 - "Animation 관계"
Cohesion: 0.20
Nodes (10): HumanoidPose, vec3, elbows, hips, knees, neckDegrees, pelvisDegrees, seated (+2 more)

### Community 12 - "Application 관계"
Cohesion: 0.05
Nodes (41): Application, GLFWwindow, assets_, callbackError_, camera_, configureRenderer, contextFailed_, contextThread_ (+33 more)

### Community 121 - "Model 관계"
Cohesion: 0.20
Nodes (10): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+2 more)

### Community 122 - "Model 관계"
Cohesion: 0.20
Nodes (10): GlColor, Material, blue, green, red, shared_ptr, color, repeatTexture (+2 more)

### Community 123 - "ModelLoader 관계"
Cohesion: 0.20
Nodes (10): LoadOptions, OutlineMode, PivotMode, allowObjFallback, customPivot, flipTextureRows, mergeCoplanarFaces, outline (+2 more)

### Community 124 - "Patterns 관계"
Cohesion: 0.20
Nodes (10): CharacterBody, direction, feet, grounded, halfWidth, height, speed, verticalSpeed (+2 more)

### Community 125 - "RenderConfig 관계"
Cohesion: 0.20
Nodes (10): VertexAttribute, GLenum, GLint, GLuint, components, integer, location, normalized (+2 more)

### Community 127 - "Animation 관계"
Cohesion: 0.20
Nodes (10): HumanoidConfig, GlColor, GridLayout, grid, headColor, leftColor, rightColor, torsoColor (+2 more)

### Community 128 - "Animation 관계"
Cohesion: 0.20
Nodes (10): HumanoidPose, vec3, elbows, hips, knees, neckDegrees, pelvisDegrees, seated (+2 more)

### Community 13 - "Application 관계"
Cohesion: 0.05
Nodes (41): Application, GLFWwindow, assets_, callbackError_, camera_, configureRenderer, contextFailed_, contextThread_ (+33 more)

### Community 130 - "Model 관계"
Cohesion: 0.20
Nodes (10): Component_Poly, parent, pivotMode, polyhedron, sideLengths, tint, transform, transformOnly (+2 more)

### Community 131 - "Model 관계"
Cohesion: 0.20
Nodes (10): GlColor, Material, blue, green, red, shared_ptr, color, repeatTexture (+2 more)

### Community 132 - "ModelLoader 관계"
Cohesion: 0.20
Nodes (10): LoadOptions, OutlineMode, PivotMode, allowObjFallback, customPivot, flipTextureRows, mergeCoplanarFaces, outline (+2 more)

### Community 133 - "Patterns 관계"
Cohesion: 0.20
Nodes (10): CharacterBody, direction, feet, grounded, halfWidth, height, speed, verticalSpeed (+2 more)

### Community 134 - "RenderConfig 관계"
Cohesion: 0.20
Nodes (10): VertexAttribute, GLenum, GLint, GLuint, components, integer, location, normalized (+2 more)

### Community 136 - "Animation 관계"
Cohesion: 0.22
Nodes (9): SwingConfig, PendulumMotion, length, pendulum, person, pivot, riderSwayDegrees, ropeThickness (+1 more)

### Community 14 - "Motion 관계"
Cohesion: 0.06
Nodes (38): CameraSystem, LayerConfig, LayerState, MotionSample, MotionState, MotionStep, map, projectionMatrix (+30 more)

### Community 140 - "Animation 관계"
Cohesion: 0.22
Nodes (9): SwingConfig, PendulumMotion, length, pendulum, person, pivot, riderSwayDegrees, ropeThickness (+1 more)

### Community 142 - "Animation 관계"
Cohesion: 0.25
Nodes (7): JointAnimation, EntityId, Motion, Transform3D, joint, motion, rest

### Community 143 - "Animation 관계"
Cohesion: 0.25
Nodes (8): SwingRig, array, animation, frame, hinge, person, ropes, seat

### Community 144 - "Application 관계"
Cohesion: 0.25
Nodes (8): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY

### Community 145 - "Model 관계"
Cohesion: 0.29
Nodes (7): TextureImage, uint32_t, uint8_t, vector, height, rgba, width

### Community 147 - "Application 관계"
Cohesion: 0.25
Nodes (8): MouseEvent, action, button, framebufferX, framebufferY, mods, windowX, windowY

### Community 148 - "Model 관계"
Cohesion: 0.29
Nodes (7): TextureImage, uint32_t, uint8_t, vector, height, rgba, width

### Community 150 - "Patterns 관계"
Cohesion: 0.29
Nodes (6): DistancePath, distances_, points_, sample, setPoints, vector

### Community 151 - "Patterns 관계"
Cohesion: 0.29
Nodes (7): JointRig, array, EntityId, head, hips, root, shoulders

### Community 152 - "RenderConfig 관계"
Cohesion: 0.29
Nodes (6): RenderVertex, Scene, color, normal, position, uv

### Community 155 - "Console 관계"
Cohesion: 0.29
Nodes (7): JointControl, size_t, string, Transform3D, initialRest, name, track

### Community 156 - "CuboidMode 관계"
Cohesion: 0.29
Nodes (7): CuboidModeConfig, camera, cuboids, renderOptions, Camera, RenderOptions, vector

### Community 157 - "Patterns 관계"
Cohesion: 0.29
Nodes (7): FaceRotationConfig, axis, degrees, edge, kind, FaceRotationKind, size_t

### Community 158 - "Patterns 관계"
Cohesion: 0.29
Nodes (7): JointRig, array, EntityId, head, hips, root, shoulders

### Community 159 - "RenderConfig 관계"
Cohesion: 0.29
Nodes (6): RenderVertex, Scene, color, normal, position, uv

### Community 162 - "Model 관계"
Cohesion: 0.33
Nodes (6): Polyhedron, GLShape, string, faces, name, shape

### Community 163 - "Model 관계"
Cohesion: 0.33
Nodes (6): Transform3D, mat3, basis, localBasis, rotation, scale

### Community 164 - " 관계"
Cohesion: 0.33
Nodes (6): TextureResult, shared_ptr, string, TextureImage, error, image

### Community 165 - "RenderConfig 관계"
Cohesion: 0.33
Nodes (6): RenderConfig, function, bindUniforms, packVertices, uniforms, vertexLayout

### Community 166 - "Model 관계"
Cohesion: 0.33
Nodes (6): Polyhedron, GLShape, string, faces, name, shape

### Community 167 - "Model 관계"
Cohesion: 0.33
Nodes (6): Transform3D, mat3, basis, localBasis, rotation, scale

### Community 168 - " 관계"
Cohesion: 0.33
Nodes (6): TextureResult, shared_ptr, string, TextureImage, error, image

### Community 169 - "RenderConfig 관계"
Cohesion: 0.33
Nodes (6): RenderConfig, function, bindUniforms, packVertices, uniforms, vertexLayout

### Community 17 - "Console 관계"
Cohesion: 0.06
Nodes (36): Console, AnimationClock, Demo, animation_, clock_, demo_, distancePath_, floorOpen_ (+28 more)

### Community 171 - "Patterns 관계"
Cohesion: 0.40
Nodes (4): AnimationClock, advance, playing, seconds

### Community 172 - "RenderConfig 관계"
Cohesion: 0.40
Nodes (5): VertexLayout, size_t, vector, attributes, stride

### Community 174 - "Patterns 관계"
Cohesion: 0.40
Nodes (4): AnimationClock, advance, playing, seconds

### Community 175 - "Patterns 관계"
Cohesion: 0.40
Nodes (5): CollisionSystem, jump, randomFreeBox, stepCharacter, stepRoomBody

### Community 176 - "RenderConfig 관계"
Cohesion: 0.40
Nodes (5): VertexLayout, size_t, vector, attributes, stride

### Community 177 - "Texture 관계"
Cohesion: 0.67
Nodes (3): ComOwner, value, T

### Community 179 - "Patterns 관계"
Cohesion: 0.50
Nodes (4): PathPose, position, tangent, yawDegrees

### Community 18 - "CuboidMode 관계"
Cohesion: 0.06
Nodes (36): CuboidCommand, CuboidConfig, CuboidFaceConfig, degrees, enabled, layer, motion, operation (+28 more)

### Community 180 - "Texture 관계"
Cohesion: 0.67
Nodes (3): ComOwner, value, T

### Community 2 - "Transform 관계"
Cohesion: 0.07
Nodes (49): GridLayout, AxisLine, PathSystem, TransformSystem, motionAround(), motionPlane(), motionUnitVector(), PathSystem::circle() (+41 more)

### Community 22 - "Motion 관계"
Cohesion: 0.07
Nodes (34): Motion, Motion::append(), Motion::erase(), Motion::fixed(), Motion::followPath(), Motion::insert(), Motion::linear(), Motion::pendulum() (+26 more)

### Community 23 - "CuboidMode 관계"
Cohesion: 0.07
Nodes (31): CuboidKeyBinding, CuboidMode, CuboidModeConfig, action, key, mods, action, apply (+23 more)

### Community 24 - "Graphics 관계"
Cohesion: 0.06
Nodes (33): Camera, RenderOptions, RenderPass, Viewport, farPlane, fieldOfViewDegrees, nearPlane, orthographic (+25 more)

### Community 25 - "Graphics 관계"
Cohesion: 0.06
Nodes (31): Renderer, GLint, GLuint, RenderConfig, TextureMap, vector, config_, drawRanges_ (+23 more)

### Community 26 - "Motion 관계"
Cohesion: 0.06
Nodes (33): LinearMotion, PathMotion, PendulumMotion, RotationMotion, AxisLine, InterpolationFunction, PathFunction, Playback (+25 more)

### Community 27 - " 관계"
Cohesion: 0.09
Nodes (29): AnimationSystem, AnimationSystem::addHumanoid(), AnimationSystem::addSwingWithRider(), AnimationSystem::createJointAnimation(), AnimationSystem::poseHierarchy(), AnimationSystem::poseHumanoid(), HumanoidPose::seated(), validJointRest() (+21 more)

### Community 28 - "Motion 관계"
Cohesion: 0.07
Nodes (31): LayerConfig, LayerState, MotionSample, MotionState, MotionStep, function, mat4, MotionAction (+23 more)

### Community 29 - "Graphics 관계"
Cohesion: 0.07
Nodes (30): Renderer, GLint, GLuint, RenderConfig, TextureMap, vector, config_, drawRanges_ (+22 more)

### Community 3 - "Transform 관계"
Cohesion: 0.07
Nodes (49): GridLayout, AxisLine, PathSystem, TransformSystem, motionAround(), motionPlane(), motionUnitVector(), PathSystem::circle() (+41 more)

### Community 30 - "Picking 관계"
Cohesion: 0.10
Nodes (28): PickResult, PickViewport, Ray, pick(), screenRay(), Camera, EntityId, optional (+20 more)

### Community 31 - "Actions 관계"
Cohesion: 0.17
Nodes (15): SceneActions, Camera, EntityId, FaceRotationConfig, FaceTransformMode, GlColor, LayerId, mat3 (+7 more)

### Community 32 - "Picking 관계"
Cohesion: 0.10
Nodes (28): PickResult, PickViewport, Ray, pick(), screenRay(), Camera, EntityId, optional (+20 more)

### Community 33 - "Native 관계"
Cohesion: 0.07
Nodes (28): GlfwApi, createWindow, DestroyWindow, GetCurrentContext, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize (+20 more)

### Community 36 - "Native 관계"
Cohesion: 0.07
Nodes (28): GlfwApi, createWindow, DestroyWindow, GetCurrentContext, GetCursorPos, GetFramebufferSize, GetTime, GetWindowSize (+20 more)

### Community 38 - "Model 관계"
Cohesion: 0.07
Nodes (27): PlaneFigure, CullMode, FaceAnimationSpace, FaceTransformMode, GLFace, OutlineMode, animationSpace, color (+19 more)

### Community 39 - "Patterns 관계"
Cohesion: 0.08
Nodes (26): Bounds3, FaceRotationConfig, PathPose, RoomBody, contains, maximum, minimum, overlaps (+18 more)

### Community 40 - "Model 관계"
Cohesion: 0.07
Nodes (27): PlaneFigure, CullMode, FaceAnimationSpace, FaceTransformMode, GLFace, OutlineMode, animationSpace, color (+19 more)

### Community 41 - "Animation 관계"
Cohesion: 0.12
Nodes (26): AnimationSystem, AnimationSystem::addHumanoid(), AnimationSystem::addSwingWithRider(), AnimationSystem::createJointAnimation(), AnimationSystem::poseHierarchy(), AnimationSystem::poseHumanoid(), HumanoidPose::seated(), validJointRest() (+18 more)

### Community 42 - "Motion 관계"
Cohesion: 0.08
Nodes (25): MotionSystem, MotionSystem::clear(), LayerId, map, bindMotion, clear, createOrbit, createSpin (+17 more)

### Community 44 - "Native 관계"
Cohesion: 0.09
Nodes (24): Module, CameraSystem, checkGlAt(), executableDirectory(), frameworkDirectory(), relativePathInside(), resolve(), symbol() (+16 more)

### Community 48 - "Motion 관계"
Cohesion: 0.08
Nodes (24): MotionSystem, MotionSystem::clear(), LayerId, map, bindMotion, clear, createOrbit, createSpin (+16 more)

### Community 52 - "CuboidMode 관계"
Cohesion: 0.09
Nodes (24): CuboidConfig, CuboidFaceConfig, color, faces, motion, name, size, transform (+16 more)

### Community 53 - "CuboidMode 관계"
Cohesion: 0.09
Nodes (21): CuboidMode, action, apply, bindings_, bindKey, config_, dispatch, entities_ (+13 more)

### Community 54 - "Patterns 관계"
Cohesion: 0.09
Nodes (21): Bounds3, DistancePath, RoomBody, contains, maximum, minimum, overlaps, valid (+13 more)

### Community 55 - "Patterns 관계"
Cohesion: 0.09
Nodes (22): PatternSystem, addNumber, addPolygon, addRoom, addWalkingRig, aligned, cameraFacingWall, conveyorCornerPose (+14 more)

### Community 56 - "Patterns 관계"
Cohesion: 0.09
Nodes (22): PatternSystem, addNumber, addPolygon, addRoom, addWalkingRig, aligned, cameraFacingWall, conveyorCornerPose (+14 more)

### Community 57 - "Patterns 관계"
Cohesion: 0.10
Nodes (14): CharacterStep, CollisionSystem, main(), blocked, landed, turned, jump, randomFreeBox (+6 more)

### Community 58 - "Graphics 관계"
Cohesion: 0.10
Nodes (20): Camera, RenderPass, Viewport, farPlane, fieldOfViewDegrees, nearPlane, orthographic, orthographicHalfExtent (+12 more)

### Community 6 - "Graphics 관계"
Cohesion: 0.04
Nodes (55): Scene, Component_Poly, FaceId, LayerId, Line3D, map, MotionSystem, uint64_t (+47 more)

### Community 60 - "Motion 관계"
Cohesion: 0.11
Nodes (19): LinearMotion, PathMotion, InterpolationFunction, PathFunction, Playback, vec3, durationSeconds, from (+11 more)

### Community 61 - "Motion 관계"
Cohesion: 0.11
Nodes (19): Motion, Motion::linear(), Motion::then(), LinearMotion, action, append, erase, evaluate (+11 more)

### Community 62 - "Native 관계"
Cohesion: 0.12
Nodes (17): Module, checkGlAt(), executableDirectory(), frameworkDirectory(), relativePathInside(), resolve(), symbol(), GetError (+9 more)

### Community 63 - "Animation 관계"
Cohesion: 0.11
Nodes (18): HumanoidRig, elbows, feet, forearms, hands, head, hips, knees (+10 more)

### Community 65 - "Animation 관계"
Cohesion: 0.11
Nodes (18): HumanoidRig, elbows, feet, forearms, hands, head, hips, knees (+10 more)

### Community 68 - "RenderConfig 관계"
Cohesion: 0.13
Nodes (15): DrawUniformContext, depthOnly, entity, face, line, model, program, projection (+7 more)

### Community 7 - "Graphics 관계"
Cohesion: 0.04
Nodes (55): Scene, Component_Poly, FaceId, LayerId, Line3D, map, MotionSystem, uint64_t (+47 more)

### Community 70 - "Patterns 관계"
Cohesion: 0.16
Nodes (5): CharacterStep, main(), blocked, landed, turned

### Community 71 - "Animation 관계"
Cohesion: 0.13
Nodes (16): JointAnimation, SwingRig, array, EntityId, Motion, Transform3D, vector, joint (+8 more)

### Community 72 - "Application 관계"
Cohesion: 0.12
Nodes (16): ApplicationConfig, diagnostics, features, height, projectRoot, renderConfig, runtimeDirectory, shaderDirectory (+8 more)

### Community 73 - "RenderConfig 관계"
Cohesion: 0.13
Nodes (15): DrawUniformContext, depthOnly, entity, face, line, model, program, projection (+7 more)

### Community 76 - "Application 관계"
Cohesion: 0.13
Nodes (15): ApplicationConfig, diagnostics, features, height, projectRoot, renderConfig, runtimeDirectory, shaderDirectory (+7 more)

### Community 8 - "Diagnostics 관계"
Cohesion: 0.07
Nodes (33): SceneActions, Diagnostic, DiagnosticOptions, Diagnostics, Camera, EntityId, FaceRotationConfig, FaceTransformMode (+25 more)

### Community 83 - "ModelLoader 관계"
Cohesion: 0.22
Nodes (10): AssetLoader, loadModel, features_, imageCache_, loadObj, loadPrimitive, projectRoot_, map (+2 more)

### Community 84 - "Motion 관계"
Cohesion: 0.15
Nodes (14): PendulumMotion, RotationMotion, AxisLine, TransformSpace, amplitudeDegrees, axis, offsetDegrees, periodSeconds (+6 more)

### Community 87 - "CuboidMode 관계"
Cohesion: 0.15
Nodes (14): CuboidKeyBinding, bindCuboidMode(), CuboidMode::dispatch(), CuboidMode::update(), dispatch(), Application, CuboidMode, shared_ptr (+6 more)

### Community 88 - "Diagnostics 관계"
Cohesion: 0.25
Nodes (7): DiagnosticOptions, Diagnostics, errors, trace, PipelineStage, string, options

### Community 89 - "ModelLoader 관계"
Cohesion: 0.22
Nodes (10): AssetLoader, loadModel, features_, imageCache_, loadObj, loadPrimitive, projectRoot_, map (+2 more)

### Community 9 - "ObjLoader 관계"
Cohesion: 0.06
Nodes (54): ObjCorner, ObjFace, ObjParseError, UVOptions, AssetLoader::loadObj(), objInteger(), objNumbers(), objResolve() (+46 more)

### Community 92 - "Graphics 관계"
Cohesion: 0.17
Nodes (13): DiagramSystem, DiagramSystem::makeCone(), DiagramSystem::makeSphere(), Scene::setModel(), Model, faceModelMatrix, makeCone, makePolyhedron (+5 more)

### Community 93 - "Graphics 관계"
Cohesion: 0.15
Nodes (13): DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount, outlineFirstIndex (+5 more)

### Community 97 - "Graphics 관계"
Cohesion: 0.17
Nodes (13): DiagramSystem, DiagramSystem::makeCone(), DiagramSystem::makeSphere(), Scene::setModel(), Model, faceModelMatrix, makeCone, makePolyhedron (+5 more)

### Community 98 - "Graphics 관계"
Cohesion: 0.15
Nodes (13): DrawRange, count, entityId, faceIndex, firstIndex, lineId, outlineCount, outlineFirstIndex (+5 more)

### Community 99 - "Graphics 관계"
Cohesion: 0.15
Nodes (13): RenderOptions, CullMode, GlColor, vec4, background, cull, depthTest, lightColor (+5 more)

### Community 100 - " 관계"
Cohesion: 0.29
Nodes (12): DistancePath::sample(), PatternSystem::addBox(), PatternSystem::addNumber(), PatternSystem::addPolygon(), PatternSystem::conveyorCornerPose(), PatternSystem::screenOnPlane(), GlColor, optional (+4 more)

### Community 106 - " 관계"
Cohesion: 0.29
Nodes (12): DistancePath::sample(), PatternSystem::addBox(), PatternSystem::addNumber(), PatternSystem::addPolygon(), PatternSystem::conveyorCornerPose(), PatternSystem::screenOnPlane(), GlColor, optional (+4 more)

### Community 11 - "Motion 관계"
Cohesion: 0.10
Nodes (42): Motion::action(), Motion::evaluate(), Motion::execute(), Motion::insertAction(), Motion::rotation(), Motion::sample(), MotionSystem::bindMotion(), MotionSystem::createMotion() (+34 more)

### Community 111 - "Patterns 관계"
Cohesion: 0.22
Nodes (11): CollisionSystem::stepRoomBody(), PatternSystem::addWalkingRig(), PatternSystem::poseWalkingRig(), rigidRoom(), GridLayout, JointRig, mat4, RoomBody (+3 more)

### Community 114 - "Patterns 관계"
Cohesion: 0.22
Nodes (11): CollisionSystem::stepRoomBody(), PatternSystem::addWalkingRig(), PatternSystem::poseWalkingRig(), rigidRoom(), GridLayout, JointRig, mat4, RoomBody (+3 more)

### Community 120 - "Model 관계"
Cohesion: 0.33
Nodes (12): faceLocalMatrix(), faceMatrix(), faceSrtMatrix(), finiteMatrix(), transformMatrix(), triangles(), validMesh(), mat4 (+4 more)

### Community 126 - "Application 관계"
Cohesion: 0.20
Nodes (10): Application::configureRenderer(), Application::renderScene(), Application::renderViews(), Application::shutdown(), path, RenderConfig, RenderPass, vector (+2 more)

### Community 129 - "Model 관계"
Cohesion: 0.38
Nodes (11): faceLocalMatrix(), faceMatrix(), faceSrtMatrix(), finiteMatrix(), transformMatrix(), triangles(), validMesh(), mat4 (+3 more)

### Community 135 - "Application 관계"
Cohesion: 0.22
Nodes (9): Application::initialize(), Application::renderScene(), Application::renderViews(), Application::shutdown(), ApplicationConfig, RenderPass, vector, ensureContext (+1 more)

### Community 139 - "Application 관계"
Cohesion: 0.22
Nodes (9): Application::~Application(), Application::initialize(), Application::run(), ApplicationConfig, initHandler_, renderHandler_, renderScene, shutdown (+1 more)

### Community 141 - "Scene 관계"
Cohesion: 0.29
Nodes (8): nextGeometryRevision(), Scene::clear(), Scene::Scene(), Scene::setFaceVertexColors(), Scene::setTint(), validColor(), GlColor, uint64_t

### Community 146 - "Scene 관계"
Cohesion: 0.29
Nodes (8): nextGeometryRevision(), Scene::clear(), Scene::Scene(), Scene::setFaceVertexColors(), Scene::setTint(), validColor(), GlColor, uint64_t

### Community 149 - "Application 관계"
Cohesion: 0.29
Nodes (7): Application::~Application(), Application::run(), initHandler_, renderHandler_, renderScene, shutdown, updateHandler_

### Community 160 - "Application 관계"
Cohesion: 0.33
Nodes (6): Application::configureRenderer(), dispatch(), path, RenderConfig, requestClose, Handler

### Community 161 - " 관계"
Cohesion: 0.29
Nodes (5): AssetLoader::loadPrimitive(), LoadOptions, ModelResult, path, 외부 헤더: initializer_list

### Community 170 - "Application 관계"
Cohesion: 0.50
Nodes (5): Application::pickAt(), Application::pickAtCursor(), optional, PickResult, pickAt

### Community 173 - "Application 관계"
Cohesion: 0.50
Nodes (5): Application::pickAt(), Application::pickAtCursor(), optional, PickResult, pickAt

### Community 178 - "Graphics 관계"
Cohesion: 0.50
Nodes (4): Renderer::initialize(), RenderConfig, destroy, initShader

### Community 183 - "CuboidMode 관계"
Cohesion: 0.67
Nodes (3): CuboidMode::setCuboid(), CuboidConfig, setConfig

### Community 21 - "CuboidMode 관계"
Cohesion: 0.09
Nodes (32): bindCuboidMode(), CuboidMode::action(), CuboidMode::apply(), CuboidMode::bindKey(), CuboidMode::dispatch(), CuboidMode::entity(), CuboidMode::layer(), CuboidMode::motion() (+24 more)

### Community 34 - "Scene 관계"
Cohesion: 0.16
Nodes (28): Scene::addBasis(), Scene::addCone(), Scene::addCuboid(), Scene::addGroup(), Scene::addLine(), Scene::addModel(), Scene::addPyramid(), Scene::addRightTetrahedron() (+20 more)

### Community 35 - "Motion 관계"
Cohesion: 0.14
Nodes (26): Motion::action(), Motion::evaluate(), Motion::execute(), Motion::rotation(), Motion::sample(), MotionSystem::bindMotion(), MotionSystem::createMotion(), MotionSystem::createOrbit() (+18 more)

### Community 37 - "Scene 관계"
Cohesion: 0.16
Nodes (28): Scene::addBasis(), Scene::addCone(), Scene::addCuboid(), Scene::addGroup(), Scene::addLine(), Scene::addModel(), Scene::addPyramid(), Scene::addRightTetrahedron() (+20 more)

### Community 43 - "Scene 관계"
Cohesion: 0.13
Nodes (24): appendFace(), DiagramSystem::makePolyhedron(), DiagramSystem::modelMatrix(), DiagramSystem::validPolyhedron(), DiagramSystem::validTransform(), positiveLengths(), Scene::push_entity(), Scene::setFaceSRT() (+16 more)

### Community 45 - "Scene 관계"
Cohesion: 0.13
Nodes (24): appendFace(), DiagramSystem::makePolyhedron(), DiagramSystem::modelMatrix(), DiagramSystem::validPolyhedron(), DiagramSystem::validTransform(), positiveLengths(), Scene::push_entity(), Scene::setFaceSRT() (+16 more)

### Community 49 - "Scene 관계"
Cohesion: 0.18
Nodes (24): Scene::generateTexcoords(), Scene::setFaceCullMode(), Scene::setMaterial(), Scene::setMesh(), Scene::setMeshVertices(), Scene::setOutline(), Scene::setTexcoords(), Scene::setTriangleIndices() (+16 more)

### Community 50 - "CuboidMode 관계"
Cohesion: 0.13
Nodes (22): CuboidMode::action(), CuboidMode::apply(), CuboidMode::bindKey(), CuboidMode::entity(), CuboidMode::layer(), CuboidMode::motion(), CuboidMode::setConfig(), lookup() (+14 more)

### Community 51 - "Scene 관계"
Cohesion: 0.18
Nodes (24): Scene::generateTexcoords(), Scene::setFaceCullMode(), Scene::setMaterial(), Scene::setMesh(), Scene::setMeshVertices(), Scene::setOutline(), Scene::setTexcoords(), Scene::setTriangleIndices() (+16 more)

### Community 59 - "Renderer 관계"
Cohesion: 0.15
Nodes (18): compileShader(), readTextFile(), Renderer::initialize(), Renderer::initShader(), Renderer::upload(), uploadTexture(), validLayout(), GLenum (+10 more)

### Community 64 - "Motion 관계"
Cohesion: 0.13
Nodes (18): Motion::append(), Motion::erase(), Motion::fixed(), Motion::followPath(), Motion::insert(), Motion::insertAction(), Motion::pendulum(), Motion::replace() (+10 more)

### Community 67 - "Scene 관계"
Cohesion: 0.15
Nodes (16): CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix(), DiagramSystem::faceModelMatrix(), Scene::faceWorldMatrix(), Scene::getComponent(), Scene::lineWorldMatrix(), Scene::setFaceLocalMatrix() (+8 more)

### Community 69 - "Scene 관계"
Cohesion: 0.15
Nodes (16): CameraSystem::projectionMatrix(), CameraSystem::validCamera(), CameraSystem::viewMatrix(), DiagramSystem::faceModelMatrix(), Scene::faceWorldMatrix(), Scene::getComponent(), Scene::lineWorldMatrix(), Scene::setFaceLocalMatrix() (+8 more)

### Community 74 - "Patterns 관계"
Cohesion: 0.16
Nodes (12): Bounds3::contains(), PatternSystem::moveCamera(), PatternSystem::orbitCamera(), PatternSystem::snap(), PatternSystem::threeViews(), PatternSystem::yawCamera(), PatternSystem::zoomCamera(), AxisLine (+4 more)

### Community 75 - "Patterns 관계"
Cohesion: 0.25
Nodes (15): PatternSystem::addJoint(), PatternSystem::addRoom(), PatternSystem::cameraFacingWall(), PatternSystem::foldFace(), PatternSystem::follow(), PatternSystem::openFace(), PatternSystem::openRoomWall(), PatternSystem::rotateFace() (+7 more)

### Community 79 - "Patterns 관계"
Cohesion: 0.17
Nodes (11): Bounds3::contains(), PatternSystem::moveCamera(), PatternSystem::orbitCamera(), PatternSystem::snap(), PatternSystem::threeViews(), PatternSystem::yawCamera(), PatternSystem::zoomCamera(), AxisLine (+3 more)

### Community 80 - "Patterns 관계"
Cohesion: 0.25
Nodes (15): PatternSystem::addJoint(), PatternSystem::addRoom(), PatternSystem::cameraFacingWall(), PatternSystem::foldFace(), PatternSystem::follow(), PatternSystem::openFace(), PatternSystem::openRoomWall(), PatternSystem::rotateFace() (+7 more)

### Community 81 - "Renderer 관계"
Cohesion: 0.21
Nodes (13): compileShader(), readTextFile(), Renderer::initShader(), Renderer::upload(), uploadTexture(), validLayout(), GLenum, GLuint (+5 more)

### Community 82 - "Application 관계"
Cohesion: 0.18
Nodes (13): Application::getFramebufferSize(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler(), Application::setKeyHandler(), Application::setModifiedKeyHandler(), Application::setMouseHandler(), Application::setRenderHandler() (+5 more)

### Community 86 - "Application 관계"
Cohesion: 0.22
Nodes (11): Application::getFramebufferSize(), Application::setCursorHandler(), Application::setFocusHandler(), Application::setInitHandler(), Application::setKeyHandler(), Application::setModifiedKeyHandler(), Application::setMouseHandler(), Application::setRenderHandler() (+3 more)

### Community 90 - "Patterns 관계"
Cohesion: 0.19
Nodes (13): Bounds3::overlaps(), CollisionSystem::jump(), CollisionSystem::randomFreeBox(), CollisionSystem::stepCharacter(), DistancePath::setPoints(), horizontalOverlap(), PatternSystem::snakeCells(), Bounds3 (+5 more)

### Community 91 - " 관계"
Cohesion: 0.19
Nodes (13): Renderer::draw(), Renderer::render(), Renderer::renderPass(), Renderer::renderViews(), Camera, mat4, RenderOptions, RenderPass (+5 more)

### Community 94 - "Motion 관계"
Cohesion: 0.26
Nodes (13): MotionSystem::getLayer(), MotionSystem::insertLayer(), MotionSystem::popLayer(), MotionSystem::pushLayer(), MotionSystem::removeLayer(), MotionSystem::reparentLayer(), MotionSystem::setLayer(), LayerConfig (+5 more)

### Community 95 - "Patterns 관계"
Cohesion: 0.19
Nodes (13): Bounds3::overlaps(), CollisionSystem::jump(), CollisionSystem::randomFreeBox(), CollisionSystem::stepCharacter(), DistancePath::setPoints(), horizontalOverlap(), PatternSystem::snakeCells(), Bounds3 (+5 more)

### Community 96 - " 관계"
Cohesion: 0.19
Nodes (13): Renderer::draw(), Renderer::render(), Renderer::renderPass(), Renderer::renderViews(), Camera, mat4, RenderOptions, RenderPass (+5 more)

### Community 104 - "CuboidMode 관계"
Cohesion: 0.17
Nodes (11): CuboidMode::apply, CuboidMode::unbindKey, CuboidMode::setConfig, CuboidMode::motion, CuboidMode::setCuboid, CuboidMode::update, CuboidMode::dispatch, CuboidMode::bindKey (+3 more)

### Community 105 - "CuboidMode 관계"
Cohesion: 0.17
Nodes (11): CuboidMode::update, CuboidMode::dispatch, CuboidMode::action, CuboidMode::motion, CuboidMode::unbindKey, CuboidMode::entity, CuboidMode::bindKey, CuboidMode::setCuboid (+3 more)

### Community 137 - "Renderer 관계"
Cohesion: 0.22
Nodes (8): Renderer::initShader, Renderer::initialize, Renderer::render, Renderer::destroy, Renderer::upload, Renderer::renderViews, Renderer::draw, Renderer::renderPass

### Community 138 - "Renderer 관계"
Cohesion: 0.22
Nodes (8): Renderer::renderViews, Renderer::initShader, Renderer::render, Renderer::draw, Renderer::destroy, Renderer::upload, Renderer::initialize, Renderer::renderPass

### Community 15 - "Motion 관계"
Cohesion: 0.05
Nodes (37): MotionSystem::setMotionEnabled, Motion::validStep, Motion::append, MotionSystem::layerMatrices, MotionSystem::getMotion, Motion::linear, MotionSystem::bindMotion, MotionSystem::update (+29 more)

### Community 153 - "Animation 관계"
Cohesion: 0.29
Nodes (6): AnimationSystem::createJointAnimation, HumanoidPose::seated, AnimationSystem::addHumanoid, AnimationSystem::addSwingWithRider, AnimationSystem::poseHierarchy, AnimationSystem::poseHumanoid

### Community 154 - "Animation 관계"
Cohesion: 0.29
Nodes (6): HumanoidPose::seated, AnimationSystem::poseHumanoid, AnimationSystem::createJointAnimation, AnimationSystem::addSwingWithRider, AnimationSystem::poseHierarchy, AnimationSystem::addHumanoid

### Community 16 - "Motion 관계"
Cohesion: 0.05
Nodes (37): Motion::followPath, Motion::evaluate, MotionSystem::createMotion, MotionSystem::validLayer, MotionSystem::getMotion, Motion::sample, Motion::execute, MotionSystem::insertLayer (+29 more)

### Community 19 - "Patterns 관계"
Cohesion: 0.06
Nodes (35): PatternSystem::snap, Bounds3::valid, CollisionSystem::stepCharacter, PatternSystem::conveyorCornerPose, DistancePath::setPoints, PatternSystem::addRoom, PatternSystem::openFace, PatternSystem::moveCamera (+27 more)

### Community 20 - "Patterns 관계"
Cohesion: 0.06
Nodes (35): PatternSystem::addPolygon, PatternSystem::screenOnPlane, PatternSystem::yawCamera, PatternSystem::addRoom, Bounds3::contains, PatternSystem::conveyorCornerPose, PatternSystem::cameraFacingWall, PatternSystem::orbitCamera (+27 more)

### Community 4 - "Scene 관계"
Cohesion: 0.03
Nodes (58): Scene::addCuboid, Scene::addGroup, Scene::faceWorldMatrix, DiagramSystem::faceModelMatrix, Scene::setPosition, Scene::addSphere, Scene::setLocalBasis, CameraSystem::viewMatrix (+50 more)

### Community 46 - "Application 관계"
Cohesion: 0.08
Nodes (24): detail::glfw, Application::setKeyHandler, Application::configureRenderer, detail::glfw, Application::setInitHandler, Application::run, Application::getAssets, detail::glfw (+16 more)

### Community 47 - "Application 관계"
Cohesion: 0.08
Nodes (24): Application::setKeyHandler, Application::setRenderHandler, Application::renderScene, detail::glfw, detail::glfw, detail::glfw, Application::pickAtCursor, Application::setInitHandler (+16 more)

### Community 5 - "Scene 관계"
Cohesion: 0.03
Nodes (58): Scene::setVector, Scene::addRightTetrahedron, Scene::rebuildFaceIds, CameraSystem::viewMatrix, Scene::setParent, CameraSystem::projectionMatrix, Scene::clear, Scene::setFaceCullMode (+50 more)

### Community 66 - "Console 관계"
Cohesion: 0.15
Nodes (15): Console::toggleInsertedLayer, Console::initAnimation, Console::onKey, Console::initRoom, Console::onUpdate, Console::handleJointKey, Console::printSelectedJoint, Console::initGrid (+7 more)

### Community 77 - "Transform 관계"
Cohesion: 0.13
Nodes (14): PathSystem::smoothStep, TransformSystem::validAffine, TransformSystem::scaling, TransformSystem::reflectLine, TransformSystem::reflectPlane, PathSystem::linear, PathSystem::rectangle, PathSystem::polyline (+6 more)

### Community 78 - "Transform 관계"
Cohesion: 0.13
Nodes (14): PathSystem::polygon, TransformSystem::reflectPoint, PathSystem::circle, TransformSystem::reflectPlane, TransformSystem::reflectLine, TransformSystem::translation, TransformSystem::validAffine, PathSystem::polyline (+6 more)

### Community 85 - "Console 관계"
Cohesion: 0.29
Nodes (13): changeReflection, toggleInsertedLayer, onKey, printHelp, initGrid, initRoom, initAnimation, choosePath (+5 more)

## Knowledge Gaps
- **1708 isolated node(s):** `CuboidMode::apply`, `CuboidMode::unbindKey`, `CuboidMode::setConfig`, `CuboidMode::motion`, `CuboidMode::setCuboid` (+1703 more)
  These have ≤1 connection - possible missing edges or undocumented components. (Counts symbols only; 2239 node(s) total have ≤1 connection when file, concept and rationale nodes are included.)
- **20 thin communities (<3 nodes) omitted from report** — run `graphify query` to explore isolated nodes.

## Suggested Questions
_Questions this graph is uniquely positioned to answer:_

- **Why does `MotionState` connect `Motion 관계` to `Motion 관계`?**
  _High betweenness centrality (0.000) - this node is a cross-community bridge._
- **Why does `Motion` connect `Motion 관계` to `Motion 관계`, `Motion 관계`?**
  _High betweenness centrality (0.000) - this node is a cross-community bridge._
- **Why does `LayerState` connect `Motion 관계` to `Motion 관계`?**
  _High betweenness centrality (0.000) - this node is a cross-community bridge._
- **What connects `CuboidMode::apply`, `CuboidMode::unbindKey`, `CuboidMode::setConfig` to the rest of the system?**
  _1708 weakly-connected nodes found - possible documentation gaps or missing edges._
- **Should `Native 관계` be split into smaller, more focused modules?**
  _Cohesion score 0.022988505747126436 - nodes in this community are weakly interconnected._
- **Should `Native 관계` be split into smaller, more focused modules?**
  _Cohesion score 0.022988505747126436 - nodes in this community are weakly interconnected._
- **Should `ObjLoader 관계` be split into smaller, more focused modules?**
  _Cohesion score 0.07088989441930618 - nodes in this community are weakly interconnected._
## 외부 헤더 경계 보완

기존 코드 노드·간선·커뮤니티를 유지하고 include 원문에 근거한 외부 헤더 경계만 보완했다. 표준·Windows 헤더는 include 참조만, SDK 헤더는 로컬 파일 존재까지 확인했다. 외부 함수 정의를 추정하지 않았다. 관계 병합 150건은 별도 항목이며 원본 간선은 EXTRACTION.json에 유지된다. framework와 standard/framework는 별도 경로이며 standard 실행은 후자를 사용한다. 같은 이름의 메서드 간 연결은 정확한 오버로드 해석이 아니다. GPU 고정 단계는 C++ 호출 간선이 아니다. 빌드·테스트·브라우저 확인은 사용자에게 맡긴다.
