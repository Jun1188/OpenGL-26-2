# 전체 코드 변경 전후

기준은 작업 시작 커밋 4eb0486cd770a6a302ac8d27b7b2f8667e21a0ea이다. 변경한 모든 원본 코드 블록의 before/after를 제공한다. 사본은 같은 원본 변경을 받는다.

단계: 정점 입력/셰이더 설정 → 의미 동작/육면체 모드 → 패턴 압축/문서 → 면 독립·계층/회전 분류. 빌드·테스트는 하지 않았다.

## framework-analysis/update_graph.py

### 기존 44행 / 변경 44행

before:

```python
                item={'file':relative,'line':line,'name':m.group(1)}
                if 'computer_graphics_warmUp1/' in relative:
                    problem=1 if line<550 else 2 if line<765 else 3 if line<876 else 4
                    item.update(condition=f'PROB == {problem}',active_by_source_default=problem==2)
                entries.append(item)
        projects.append({'project':p.relative_to(REPOSITORY).as_posix(),'sources':sources,'entrypoints':entries})
```

after:

```python
                item={'file':relative,'line':line,'name':m.group(1)}
                if 'computer_graphics_warmUp1/' in relative:
                    branches=list(re.finditer(r'^\s*#(?:if|elif)\s+PROB\s*==\s*(\d+)',text[:m.start()],re.M))
                    default=re.search(r'^\s*#define\s+PROB\s+(\d+)',text,re.M)
                    problem=int(branches[-1].group(1)) if branches else None
                    item.update(condition=f'PROB == {problem}',
                                active_by_source_default=bool(default and problem==int(default.group(1))))
                entries.append(item)
        projects.append({'project':p.relative_to(REPOSITORY).as_posix(),'sources':sources,'entrypoints':entries})
```

## framework/Framework.h

### 기존 4행 / 변경 4행

before:

```cpp
#include "include/Patterns.h"
#include "include/Animation.h"
```

after:

```cpp
#include "include/Patterns.h"
#include "include/Animation.h"
#include "include/Actions.h"
#include "include/CuboidMode.h"
```

## framework/detail/Application.cpp

### 기존 23행 / 변경 23행

before:

```cpp
Application::~Application() {
    shutdown();
}

```

after:

```cpp
Application::~Application() {
    shutdown();
}

bool Application::configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath,
                                    const std::filesystem::path& fragmentPath) {
    if (!ensureContext("configureRenderer", __FILE__, __LINE__))
        return false;
    if (renderer_.initialize(vertexPath, fragmentPath, config))
        return true;
    requestClose();
    return false;
}

```

### 기존 141행 / 변경 151행

before:

```cpp
    Diagnostics::locate(__FILE__, __LINE__, "initialize -> Renderer::initialize -> shader source/path setup");

    if (!renderer_.initialize(vertexPath, fragmentPath))
        return false;
    assets_.emplace(root, config.features);
```

after:

```cpp
    Diagnostics::locate(__FILE__, __LINE__, "initialize -> Renderer::initialize -> shader source/path setup");

    if (!renderer_.initialize(vertexPath, fragmentPath, config.renderConfig))
        return false;
    assets_.emplace(root, config.features);
```

## framework/detail/CuboidMode.cpp

### 기존 1행 / 변경 1행

before:

```cpp
// 없음
```

after:

```cpp
#include "../Framework.h"
#include <algorithm>

namespace graphics {
namespace {
bool validFaceRotation(const std::optional<FaceRotationConfig>& rotation) {
    if (!rotation) return true;
    if (!std::isfinite(rotation->degrees)) return false;
    if (rotation->kind == FaceRotationKind::EdgeHinge) return rotation->edge < 4;
    return (rotation->kind == FaceRotationKind::SelfSpin || rotation->kind == FaceRotationKind::ModelOrbit) &&
           detail::finiteVector(rotation->axis) && glm::length(glm::dvec3(rotation->axis)) > 1e-12;
}
bool validCuboid(const CuboidConfig& c) {
    if (c.name.empty() || !detail::finiteVector(c.size) || c.size.x <= 0 || c.size.y <= 0 || c.size.z <= 0 ||
        !detail::validColor(c.color) || !DiagramSystem::validTransform(c.transform))
        return false;
    for (const auto& face : c.faces)
        if (!DiagramSystem::validTransform(face.transform) || !detail::validColor(face.color) || !validFaceRotation(face.rotation) ||
            (face.mode != FaceTransformMode::Hierarchy && face.mode != FaceTransformMode::Independent) ||
            (face.cull != CullMode::Inherit && face.cull != CullMode::None &&
             face.cull != CullMode::Back && face.cull != CullMode::Front))
            return false;
    return !c.motion || bool(c.motion->sample(0));
}
template<class Id> std::optional<Id> lookup(const std::map<std::string, Id>& map, const std::string& name) {
    const auto found = map.find(name);
    return found == map.end() ? std::nullopt : std::optional<Id>(found->second);
}
}

bool CuboidMode::setConfig(const CuboidModeConfig& config) {
    const auto& options = config.renderOptions;
    if (!CameraSystem::validCamera(config.camera) || !detail::finiteVector(options.lightDirection) ||
        glm::length(options.lightDirection) < 1e-12f || !detail::validColor(options.lightColor) ||
        !detail::validColor(options.outlineColor) || !std::isfinite(options.background.x) ||
        !std::isfinite(options.background.y) || !std::isfinite(options.background.z) ||
        !std::isfinite(options.background.w) || (options.cull != CullMode::None &&
        options.cull != CullMode::Back && options.cull != CullMode::Front))
        return false;
    std::vector<std::string> names;
    for (const auto& cuboid : config.cuboids) {
        if (!validCuboid(cuboid) || std::find(names.begin(), names.end(), cuboid.name) != names.end())
            return false;
        names.push_back(cuboid.name);
    }
    config_ = config;
    return true;
}
bool CuboidMode::setCuboid(const CuboidConfig& cuboid) {
    auto next = config_;
    const auto found = std::find_if(next.cuboids.begin(), next.cuboids.end(),
                                    [&](const auto& c) { return c.name == cuboid.name; });
    if (found == next.cuboids.end()) next.cuboids.push_back(cuboid);
    else *found = cuboid;
    return setConfig(next);
}
bool CuboidMode::bindKey(const CuboidKeyBinding& binding) {
    if (!binding.handler || (binding.action != GLFW_PRESS && binding.action != GLFW_RELEASE && binding.action != GLFW_REPEAT))
        return false;
    const auto found = std::find_if(bindings_.begin(), bindings_.end(), [&](const auto& b) {
        return b.key == binding.key && b.action == binding.action && b.mods == binding.mods;
    });
    if (found == bindings_.end()) bindings_.push_back(binding);
    else *found = binding;
    return true;
}
bool CuboidMode::unbindKey(int key, int action, int mods) {
    const auto found = std::find_if(bindings_.begin(), bindings_.end(), [&](const auto& b) {
        return b.key == key && b.action == action && b.mods == mods;
    });
    if (found == bindings_.end()) return false;
    bindings_.erase(found);
    return true;
}
std::optional<EntityId> CuboidMode::entity(const std::string& name) const { return lookup(entities_, name); }
std::optional<LayerId> CuboidMode::layer(const std::string& name) const { return lookup(layers_, name); }
std::optional<MotionId> CuboidMode::motion(const std::string& name) const { return lookup(motions_, name); }

bool CuboidMode::apply(Application& app) {
    Scene staged;
    std::map<std::string, EntityId> entities;
    std::map<std::string, LayerId> layers;
    std::map<std::string, MotionId> motions;
    for (const auto& c : config_.cuboids) {
        if (!validCuboid(c)) return false;
        auto model = DiagramSystem::makePolyhedron(GLShape::Cube, c.size);
        for (std::size_t face = 0; face < c.faces.size(); ++face)
            model.faces[face].color = c.faces[face].color;
        const auto id = staged.addModel(model, c.transform, PivotMode::Preserve);
        if (!id || !staged.setTransform(*id, c.transform) || !staged.setTint(*id, c.color) ||
            !staged.setVisible(*id, c.visible)) return false;
        for (std::size_t face = 0; face < c.faces.size(); ++face)
            if (!staged.setFaceTransform(*id, face, c.faces[face].transform, true) ||
                !staged.setFaceVisible(*id, face, c.faces[face].visible) ||
                !staged.setFaceCullMode(*id, face, c.faces[face].cull) ||
                (c.faces[face].rotation && !PatternSystem::rotateFace(staged, *id, face, *c.faces[face].rotation))) return false;
        entities.emplace(c.name, *id);
        if (c.motion) {
            const auto motion = staged.getMotions().createMotion(*c.motion);
            if (!motion) return false;
            LayerConfig layer;
            layer.motion = *motion;
            const auto idLayer = staged.getMotions().pushLayer(MotionSystem::rootLayer(), layer);
            if (!idLayer || !staged.bindEntity(*id, *idLayer)) return false;
            layers.emplace(c.name, *idLayer);
            motions.emplace(c.name, *motion);
        }
        // Freeze independent faces only after the initial motion/layer binding exists.
        for (std::size_t face = 0; face < c.faces.size(); ++face)
            if (!staged.setFaceTransformMode(*id, face, c.faces[face].mode)) return false;
    }
    app.getScene() = std::move(staged);
    app.getCamera() = config_.camera;
    app.getRenderOptions() = config_.renderOptions;
    entities_ = std::move(entities);
    layers_ = std::move(layers);
    motions_ = std::move(motions);
    const auto setup = setup_;
    return !setup || setup(app, *this);
}
bool CuboidMode::update(Application& app, float dt) {
    const auto handler = update_;
    return !handler || handler(app, *this, dt);
}
bool CuboidMode::dispatch(Application& app, int key, int action, int mods) {
    for (const auto& binding : bindings_)
        if (binding.key == key && binding.action == action && binding.mods == mods) {
            const auto handler = binding.handler;
            return handler(app, *this);
        }
    return true; // Unbound keys are normal.
}
CuboidHandler CuboidMode::action(std::string name, CuboidCommand command) {
    return [name = std::move(name), command](Application& app, CuboidMode& mode) {
        const auto id = mode.entity(name);
        auto& scene = app.getScene();
        if (!id) return false;
        const auto* component = scene.getComponent(*id);
        if (!component) return false;
        auto value = component->transform;
        MotionAction action;
        switch (command.operation) {
        case CuboidOperation::Position: action = SceneActions::position(scene, *id, command.value); break;
        case CuboidOperation::Translate:
            action = SceneActions::position(scene, *id, value.basis + command.value); break;
        case CuboidOperation::Rotate: {
            const auto rotation = TransformSystem::rotation({glm::vec3(0), command.value}, command.degrees);
            if (!rotation) return false;
            value.rotation = value.rotation * glm::mat3(*rotation);
            action = SceneActions::transform(scene, *id, value); break;
        }
        case CuboidOperation::Scale: action = SceneActions::scale(scene, *id, command.value); break;
        case CuboidOperation::Transform: action = SceneActions::transform(scene, *id, command.transform); break;
        case CuboidOperation::Pivot: action = SceneActions::pivot(scene, *id, command.value); break;
        case CuboidOperation::Visible: action = SceneActions::visible(scene, *id, command.enabled); break;
        case CuboidOperation::ToggleVisible: action = SceneActions::visible(scene, *id, !component->visible); break;
        case CuboidOperation::Layer: action = SceneActions::layer(scene, *id, command.layer); break;
        case CuboidOperation::Motion: {
            const auto layer = scene.getEntityLayer(*id);
            return layer && scene.getMotions().bindMotion(*layer, command.motion);
        }
        case CuboidOperation::Playing: {
            const auto layer = scene.getEntityLayer(*id);
            const auto* state = layer ? scene.getMotions().getLayer(*layer) : nullptr;
            return state && state->config.motion && scene.getMotions().setMotionPlaying(*state->config.motion, command.enabled);
        }
        default: return false;
        }
        return action(0, MotionSample{});
    };
}
bool bindCuboidMode(Application& app, std::shared_ptr<CuboidMode> mode) {
    if (!mode) return false;
    app.setInitHandler([mode](Application& app) { return mode->apply(app); });
    app.setUpdateHandler([mode](Application& app, float dt) {
        if (!mode->update(app, dt)) app.requestClose();
    });
    app.setModifiedKeyHandler([mode](Application& app, int key, int action, int mods) {
        if (!mode->dispatch(app, key, action, mods)) app.requestClose();
    });
    return true;
}
} // namespace graphics
```

## framework/detail/Motion.cpp

### 기존 6행 / 변경 6행

before:

```cpp
namespace graphics {
bool Motion::validStep(const MotionStep& step) {
    return bool(step.transform) && std::isfinite(step.startSeconds) && step.startSeconds >= 0 &&
           (!step.durationSeconds || (std::isfinite(*step.durationSeconds) && *step.durationSeconds >= 0 &&
                                      std::isfinite(step.startSeconds + *step.durationSeconds)));
```

after:

```cpp
namespace graphics {
bool Motion::validStep(const MotionStep& step) {
    return (bool(step.transform) != bool(step.action)) && std::isfinite(step.startSeconds) && step.startSeconds >= 0 &&
           (!step.durationSeconds || (std::isfinite(*step.durationSeconds) && *step.durationSeconds >= 0 &&
                                      std::isfinite(step.startSeconds + *step.durationSeconds)));
```

### 기존 24행 / 변경 24행

before:

```cpp

    return true;
}

```

after:

```cpp

    return true;
}

MotionStep Motion::action(MotionAction callback, double startSeconds, std::optional<double> durationSeconds) {
    MotionStep step;
    step.action = std::move(callback);
    step.startSeconds = startSeconds;
    step.durationSeconds = durationSeconds;
    return step;
}

bool Motion::insertAction(std::size_t index, MotionAction callback, double startSeconds,
                          std::optional<double> durationSeconds) {
    return insert(index, action(std::move(callback), startSeconds, durationSeconds));
}

```

### 기존 64행 / 변경 77행

before:

```cpp

std::optional<MotionSample> Motion::sample(double seconds) const {
    if (!std::isfinite(seconds) || seconds < 0)
        return (
```

after:

```cpp

std::optional<MotionSample> Motion::sample(double seconds) const {
    return evaluate(seconds, false);
}

std::optional<MotionSample> Motion::execute(double seconds) const {
    // Snapshot permits callbacks to edit the original sequence without invalidating iteration.
    const Motion snapshot = *this;
    return snapshot.evaluate(seconds, true);
}

std::optional<MotionSample> Motion::evaluate(double seconds, bool actions) const {
    if (!std::isfinite(seconds) || seconds < 0)
        return (
```

### 기존 82행 / 변경 105행

before:

```cpp
            time = std::min(time, *step.durationSeconds);

        const auto matrix = step.transform(time);

```

after:

```cpp
            time = std::min(time, *step.durationSeconds);

        if (step.action) {
            if (actions && !step.action(time, result))
                return (Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "Motion::execute",
                                            "Action returned false; prior external effects are retained."), std::nullopt);
            continue;
        }
        const auto matrix = step.transform(time);

```

### 기존 365행 / 변경 394행

before:

```cpp
        double time;
        MotionSample sample;
    };

```

after:

```cpp
        double time;
        MotionSample sample;
        std::optional<Motion> actions;
    };

```

### 기존 384행 / 변경 414행

before:

```cpp
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::update", "Rejected condition: !sample",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
        pending.push_back({entry.first, time, *sample});
    }

```

after:

```cpp
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::update", "Rejected condition: !sample",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
        const auto& steps = state.motion.getSteps();
        const bool actions = state.enabled && std::any_of(steps.begin(), steps.end(),
                                                         [](const auto& step) { return bool(step.action); });
        pending.push_back({entry.first, time, *sample, actions ? std::optional<Motion>(state.motion) : std::nullopt});
    }

```

### 기존 393행 / 변경 426행

before:

```cpp
        state.sampled = item.sample;
    }

    return true;
```

after:

```cpp
        state.sampled = item.sample;
    }
    for (const auto& item : pending)
        if (item.actions && !item.actions->execute(item.time))
            return false;

    return true;
```

### 기존 639행 / 변경 675행

before:

```cpp
}

std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;

    Motion motion;

    if (!motion.append(Motion::rotation(config)))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createSpin",
                                "Rejected condition: !motion.append(Motion::rotation(config))",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return createMotion(motion);
}

std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;

    Motion motion;

    if (!motion.append(Motion::rotation(config)))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createOrbit",
                                "Rejected condition: !motion.append(Motion::rotation(config))",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return createMotion(motion);
}

std::optional<MotionId> MotionSystem::createOrbitAndSpin(RotationMotion orbit, RotationMotion spin) {
    orbit.space = TransformSpace::World;
    spin.space = TransformSpace::Local;

    Motion motion;

    if (!motion.append(Motion::rotation(orbit)) || !motion.append(Motion::rotation(spin)))
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createOrbitAndSpin",
                "Rejected condition: !motion.append(Motion::rotation(orbit)) || !motion.append(Motion::rotation(spin))",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return createMotion(motion);
}

std::optional<MotionId> MotionSystem::createPathAndSpin(const PathMotion& path, RotationMotion spin) {
    spin.space = TransformSpace::Local;

    Motion motion;

    if (!motion.append(Motion::followPath(path)) || !motion.append(Motion::rotation(spin)))
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createPathAndSpin",
                "Rejected condition: !motion.append(Motion::followPath(path)) || !motion.append(Motion::rotation(spin))",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return createMotion(motion);
}
} // namespace graphics
```

after:

```cpp
}

std::optional<MotionId> MotionSystem::createMotion(std::initializer_list<MotionStep> steps) {
    Motion motion;
    for (const auto& step : steps)
        if (!motion.append(step)) return std::nullopt;
    return createMotion(motion);
}

std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    return createMotion({Motion::rotation(config)});
}

std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    return createMotion({Motion::rotation(config)});
}
} // namespace graphics
```

## framework/detail/Native.h

### 기존 206행 / 변경 206행

before:

```cpp
    PFNGLDELETEBUFFERSPROC DeleteBuffers = nullptr;
    PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer = nullptr;
    PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray = nullptr;
    PFNGLCREATESHADERPROC CreateShader = nullptr;
```

after:

```cpp
    PFNGLDELETEBUFFERSPROC DeleteBuffers = nullptr;
    PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer = nullptr;
    PFNGLVERTEXATTRIBIPOINTERPROC VertexAttribIPointer = nullptr;
    PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray = nullptr;
    PFNGLCREATESHADERPROC CreateShader = nullptr;
```

### 기존 226행 / 변경 227행

before:

```cpp
    PFNGLUNIFORM3FPROC Uniform3f = nullptr;
    PFNGLUNIFORM1IPROC Uniform1i = nullptr;
    PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;

```

after:

```cpp
    PFNGLUNIFORM3FPROC Uniform3f = nullptr;
    PFNGLUNIFORM1IPROC Uniform1i = nullptr;
    PFNGLUNIFORM1FPROC Uniform1f = nullptr;
    PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;

```

### 기존 277행 / 변경 279행

before:

```cpp
        DeleteBuffers = resolve<decltype(DeleteBuffers)>("glDeleteBuffers");
        VertexAttribPointer = resolve<decltype(VertexAttribPointer)>("glVertexAttribPointer");
        EnableVertexAttribArray = resolve<decltype(EnableVertexAttribArray)>("glEnableVertexAttribArray");
        CreateShader = resolve<decltype(CreateShader)>("glCreateShader");
```

after:

```cpp
        DeleteBuffers = resolve<decltype(DeleteBuffers)>("glDeleteBuffers");
        VertexAttribPointer = resolve<decltype(VertexAttribPointer)>("glVertexAttribPointer");
        VertexAttribIPointer = resolve<decltype(VertexAttribIPointer)>("glVertexAttribIPointer");
        EnableVertexAttribArray = resolve<decltype(EnableVertexAttribArray)>("glEnableVertexAttribArray");
        CreateShader = resolve<decltype(CreateShader)>("glCreateShader");
```

### 기존 297행 / 변경 300행

before:

```cpp
        Uniform3f = resolve<decltype(Uniform3f)>("glUniform3f");
        Uniform1i = resolve<decltype(Uniform1i)>("glUniform1i");
        ActiveTexture = resolve<decltype(ActiveTexture)>("glActiveTexture");
    }
```

after:

```cpp
        Uniform3f = resolve<decltype(Uniform3f)>("glUniform3f");
        Uniform1i = resolve<decltype(Uniform1i)>("glUniform1i");
        Uniform1f = resolve<decltype(Uniform1f)>("glUniform1f");
        ActiveTexture = resolve<decltype(ActiveTexture)>("glActiveTexture");
    }
```

## framework/detail/Patterns.cpp

### 기존 593행 / 변경 593행

before:

```cpp
}

bool PatternSystem::foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees) {
    const auto matrix = TransformSystem::rotation(hinge, degrees);
```

after:

```cpp
}

bool PatternSystem::rotateFace(Scene& scene, EntityId entity, std::size_t face, const FaceRotationConfig& config) {
    const auto* component = scene.getComponent(entity);
    const auto* mesh = scene.getMesh(entity, face);
    if (!component || !mesh || mesh->localVertices.size() < 3 || !std::isfinite(config.degrees))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "rotateFace",
                                   "Need a valid polygon face and finite angle.");
    const auto srt = detail::faceSrtMatrix(*mesh);
    auto point = [&](const glm::vec3& vertex) { return glm::vec3(srt * glm::vec4(vertex, 1)); };
    AxisLine axis;
    switch (config.kind) {
    case FaceRotationKind::EdgeHinge:
        if (config.edge >= mesh->localVertices.size()) return false;
        axis.point = point(mesh->localVertices[config.edge]);
        axis.direction = point(mesh->localVertices[(config.edge + 1) % mesh->localVertices.size()]) - axis.point;
        break;
    case FaceRotationKind::SelfSpin:
        for (const auto& vertex : mesh->localVertices) axis.point += point(vertex);
        axis.point /= float(mesh->localVertices.size());
        axis.direction = config.axis;
        break;
    case FaceRotationKind::ModelOrbit: {
        const auto bounds = detail::bounds(component->polyhedron);
        axis.point = bounds.first*.5f + bounds.second*.5f;
        axis.direction = config.axis;
        break;
    }
    default: return false;
    }
    const auto matrix = TransformSystem::rotation(axis, config.degrees);
    return matrix && scene.setFaceLocalMatrix(entity, face, *matrix, FaceAnimationSpace::Model);
}

bool PatternSystem::foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees) {
    const auto matrix = TransformSystem::rotation(hinge, degrees);
```

### 기존 673행 / 변경 705행

before:

```cpp
}

bool PatternSystem::swapModels(Scene& scene, EntityId first, EntityId second) {
    const auto* a = scene.getComponent(first);
    const auto* b = scene.getComponent(second);

    if (!a || !b || a->transformOnly || b->transformOnly)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "PatternSystem::swapModels",
                                   "Both entities must contain a model.");

    const auto firstModel = a->polyhedron, secondModel = b->polyhedron;

    if (!scene.setModel(first, secondModel))
        return false;

    if (!scene.setModel(second, firstModel)) {
        scene.setModel(first, firstModel);

        return false;
    }

    return true;
}

std::vector<glm::ivec3> PatternSystem::snakeCells(const glm::ivec3& cells) {
    std::vector<glm::ivec3> result;
```

after:

```cpp
}

std::vector<glm::ivec3> PatternSystem::snakeCells(const glm::ivec3& cells) {
    std::vector<glm::ivec3> result;
```

## framework/detail/Renderer.cpp

### 기존 7행 / 변경 7행

before:

```cpp
#include <iostream>
#include <sstream>

namespace graphics {
```

after:

```cpp
#include <iostream>
#include <sstream>
#include <cstring>

namespace graphics {
```

### 기존 69행 / 변경 70행

before:

```cpp
}

struct GpuVertex {
    float position[3], color[3], uv[2], normal[3];
};

GLuint uploadTexture(const TextureImage& image, bool repeat) {
```

after:

```cpp
}

bool validLayout(const VertexLayout& layout) {
    if (!layout.stride || layout.stride > std::size_t(std::numeric_limits<GLsizei>::max()) || layout.attributes.empty())
        return false;
    GLint maximum = 0;
    gl().GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maximum);
    std::vector<GLuint> used;
    for (const auto& a : layout.attributes) {
        std::size_t bytes = 0;
        switch (a.type) {
        case GL_BYTE: case GL_UNSIGNED_BYTE: bytes = 1; break;
        case GL_SHORT: case GL_UNSIGNED_SHORT: bytes = 2; break;
        case GL_INT: case GL_UNSIGNED_INT: case GL_FLOAT: bytes = 4; break;
        default: return false;
        }
        if (a.location >= GLuint(maximum) || a.components < 1 || a.components > 4 ||
            a.offset > layout.stride || std::size_t(a.components)*bytes > layout.stride-a.offset ||
            (a.integer && (a.normalized || a.type == GL_FLOAT)) ||
            std::find(used.begin(), used.end(), a.location) != used.end())
            return false;
        used.push_back(a.location);
    }
    return true;
}

GLuint uploadTexture(const TextureImage& image, bool repeat) {
```

### 기존 167행 / 변경 188행

before:

```cpp
                                   "Match vertex outputs and fragment inputs; inspect the linker log.");
    }
    mvpLocation_ = gl.GetUniformLocation(shaderProgramID_, "uMVP");
    modelLocation_ = gl.GetUniformLocation(shaderProgramID_, "uModel");
    normalLocation_ = gl.GetUniformLocation(shaderProgramID_, "uNormalMatrix");
    textureLocation_ = gl.GetUniformLocation(shaderProgramID_, "uTexture");
    hasTextureLocation_ = gl.GetUniformLocation(shaderProgramID_, "uHasTexture");
    lightingLocation_ = gl.GetUniformLocation(shaderProgramID_, "uLighting");
    lightDirectionLocation_ = gl.GetUniformLocation(shaderProgramID_, "uLightDirection");
    lightColorLocation_ = gl.GetUniformLocation(shaderProgramID_, "uLightColor");
    overrideLocation_ = gl.GetUniformLocation(shaderProgramID_, "uOverrideColor");
    outlineColorLocation_ = gl.GetUniformLocation(shaderProgramID_, "uOutlineColor");
    tintLocation_ = gl.GetUniformLocation(shaderProgramID_, "uTint");

    return mvpLocation_ >= 0 || Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glGetUniformLocation",
                                                    "Required uMVP uniform is missing or optimized away.");
}

bool Renderer::initialize(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
    detail::gl().load();
    destroy();

    if (!initShader(vertexPath, fragmentPath)) {
```

after:

```cpp
                                   "Match vertex outputs and fragment inputs; inspect the linker log.");
    }
    mvpLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.mvp.c_str());
    modelLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.model.c_str());
    normalLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.normal.c_str());
    textureLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.texture.c_str());
    hasTextureLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.hasTexture.c_str());
    lightingLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.lighting.c_str());
    lightDirectionLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.lightDirection.c_str());
    lightColorLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.lightColor.c_str());
    overrideLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.overrideColor.c_str());
    outlineColorLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.outlineColor.c_str());
    tintLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.tint.c_str());

    return mvpLocation_ >= 0 || (config_.uniforms.mvp.empty() && bool(config_.bindUniforms)) || Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glGetUniformLocation",
                                                    "Required MVP uniform is missing or optimized away: " + config_.uniforms.mvp);
}

bool Renderer::initialize(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath,
                          const RenderConfig& config) {
    detail::gl().load();
    if (!detail::validLayout(config.vertexLayout) ||
        (!config.packVertices && config.vertexLayout.stride != sizeof(RenderVertex)))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::initialize",
                                   "Invalid layout, or custom stride without a vertex packer.");
    const auto nextConfig = config; // Copy before destroying resources; config may alias current settings.
    destroy();
    config_ = nextConfig;

    if (!initShader(vertexPath, fragmentPath)) {
```

### 기존 208행 / 변경 236행

before:

```cpp
    gl.BindBuffer(GL_ARRAY_BUFFER, vbo_);
    gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    using V = detail::GpuVertex;
    gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), reinterpret_cast<void*>(offsetof(V, position)));
    gl.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(V), reinterpret_cast<void*>(offsetof(V, color)));
    gl.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V), reinterpret_cast<void*>(offsetof(V, uv)));
    gl.VertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(V), reinterpret_cast<void*>(offsetof(V, normal)));

    for (GLuint i = 0; i < 4; ++i)
        gl.EnableVertexAttribArray(i);
    gl.BindVertexArray(0);
    gl.BindBuffer(GL_ARRAY_BUFFER, 0);
```

after:

```cpp
    gl.BindBuffer(GL_ARRAY_BUFFER, vbo_);
    gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    for (const auto& attribute : config_.vertexLayout.attributes) {
        const auto pointer = reinterpret_cast<const void*>(attribute.offset);
        if (attribute.integer)
            gl.VertexAttribIPointer(attribute.location, attribute.components, attribute.type,
                                    GLsizei(config_.vertexLayout.stride), pointer);
        else
            gl.VertexAttribPointer(attribute.location, attribute.components, attribute.type,
                                   attribute.normalized ? GL_TRUE : GL_FALSE,
                                   GLsizei(config_.vertexLayout.stride), pointer);
        gl.EnableVertexAttribArray(attribute.location);
    }
    gl.BindVertexArray(0);
    gl.BindBuffer(GL_ARRAY_BUFFER, 0);
```

### 기존 225행 / 변경 256행

before:

```cpp
    Diagnostics::trace(PipelineStage::VertexInput, "Renderer::upload");

    std::vector<detail::GpuVertex> vertices;
    std::vector<GLuint> indices;
    std::vector<DrawRange> ranges;
```

after:

```cpp
    Diagnostics::trace(PipelineStage::VertexInput, "Renderer::upload");

    std::vector<RenderVertex> vertices;
    std::vector<GLuint> indices;
    std::vector<DrawRange> ranges;
```

### 기존 360행 / 변경 391행

before:

```cpp
        }

        if (vertices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / sizeof(detail::GpuVertex) ||
            indices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / sizeof(GLuint)) {
            cleanup();
```

after:

```cpp
        }

        if (vertices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / config_.vertexLayout.stride ||
            indices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / sizeof(GLuint)) {
            cleanup();
```

### 기존 368행 / 변경 399행

before:

```cpp
        }

        if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::VertexInput, "before buffer upload")) {
            cleanup();
```

after:

```cpp
        }

        const auto byteCount = vertices.size() * config_.vertexLayout.stride;
        std::vector<std::uint8_t> packed;
        if (config_.packVertices) {
            if (!config_.packVertices(vertices, packed) || packed.size() != byteCount) {
                cleanup();
                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "packVertices",
                                           "Packer failed or byte count does not match vertex count times stride.");
            }
        } else {
            packed.resize(byteCount);
            if (byteCount)
                std::memcpy(packed.data(), vertices.data(), byteCount);
        }

        if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::VertexInput, "before buffer upload")) {
            cleanup();
```

### 기존 375행 / 변경 420행

before:

```cpp
        gl.BindVertexArray(vao_);
        gl.BindBuffer(GL_ARRAY_BUFFER, vbo_);
        gl.BufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(detail::GpuVertex)),
                      vertices.empty() ? nullptr : vertices.data(), GL_STATIC_DRAW);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indices.size() * sizeof(GLuint)),
```

after:

```cpp
        gl.BindVertexArray(vao_);
        gl.BindBuffer(GL_ARRAY_BUFFER, vbo_);
        gl.BufferData(GL_ARRAY_BUFFER, GLsizeiptr(packed.size()),
                      packed.empty() ? nullptr : packed.data(), GL_STATIC_DRAW);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indices.size() * sizeof(GLuint)),
```

### 기존 478행 / 변경 523행

before:

```cpp

            gl.UniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(matrix));
            gl.DrawElements(GL_TRIANGLES, range.count, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(range.firstIndex * sizeof(GLuint)));
```

after:

```cpp

            gl.UniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(matrix));
            if (config_.bindUniforms && !config_.bindUniforms(
                    {shaderProgramID_, scene, range.entityId, range.lineId, range.faceIndex, model, view, projection, true}))
                return false;
            gl.DrawElements(GL_TRIANGLES, range.count, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(range.firstIndex * sizeof(GLuint)));
```

### 기존 550행 / 변경 598행

before:

```cpp
        } else
            gl.BindTexture(GL_TEXTURE_2D, 0);

        if (!face || !options.wireframe) {
```

after:

```cpp
        } else
            gl.BindTexture(GL_TEXTURE_2D, 0);

        if (config_.bindUniforms && !config_.bindUniforms(
                {shaderProgramID_, scene, range.lineId ? std::nullopt : std::optional<EntityId>(range.entityId),
                 range.lineId, range.faceIndex, model, view, projection, false}))
            return false;

        if (!face || !options.wireframe) {
```

## framework/detail/Scene.cpp

### 기존 199행 / 변경 199행

before:

```cpp
}

bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceLocalMatrix",
```

after:

```cpp
}

bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix, FaceAnimationSpace space) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix) ||
        (space != FaceAnimationSpace::Local && space != FaceAnimationSpace::Model)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceLocalMatrix",
```

### 기존 207행 / 변경 208행

before:

```cpp
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;

    return true;
```

after:

```cpp
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    components_[id].polyhedron.faces[faceIndex].animationSpace = space;

    return true;
```

### 기존 402행 / 변경 404행

before:

```cpp
            std::nullopt);

    const auto world = worldMatrix(id);

    if (!world)
```

after:

```cpp
            std::nullopt);

    const auto& face = components_[id].polyhedron.faces[faceIndex];
    const auto world = face.transformMode == FaceTransformMode::Hierarchy
                           ? worldMatrix(id) : std::optional<glm::mat4>(glm::mat4(1));

    if (!world)
```

### 기존 410행 / 변경 414행

before:

```cpp
                std::nullopt);

    const auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 child(1);

    if (face.independence)
        child = glm::translate(glm::mat4(1), face.transform.localBasis) * detail::transformMatrix(face.transform);

    const auto result = *world * child * face.localAnimationMatrix;

    if (!detail::finiteMatrix(result)) {
```

after:

```cpp
                std::nullopt);

    const auto result = *world * face.transformFrame * detail::faceLocalMatrix(face);

    if (!detail::finiteMatrix(result)) {
```

### 기존 488행 / 변경 486행

before:

```cpp
    components_[id].transform = transform;

    return true;
}
```

after:

```cpp
    components_[id].transform = transform;

    return true;
}

bool Scene::setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld) {
    if (!getMesh(id, faceIndex) || (mode != FaceTransformMode::Hierarchy && mode != FaceTransformMode::Independent))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
                                   "Invalid entity, face index or transform mode.");
    auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 frame(1);
    if (preserveWorld) {
        if (mode == face.transformMode) return true;
        const auto world = worldMatrix(id);
        if (!world) return false;
        const auto oldParent = face.transformMode == FaceTransformMode::Hierarchy ? *world : glm::mat4(1);
        frame = oldParent * face.transformFrame;
        if (mode == FaceTransformMode::Hierarchy) {
            const float determinant = glm::determinant(*world);
            if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
                                           "Cannot preserve world pose under a singular parent transform.");
            frame = glm::inverse(*world) * frame;
        }
    }
    if (!TransformSystem::validAffine(frame)) return false;
    face.transformMode = mode;
    face.transformFrame = frame;
    return true;
}
```

## framework/include/Actions.h

### 기존 1행 / 변경 1행

before:

```cpp
// 없음
```

after:

```cpp
#pragma once
#include "Patterns.h"

namespace graphics {
// Value-configured operations. Scene and camera references are non-owning.
class SceneActions {
public:
    static MotionAction transform(Scene& scene, EntityId id, Transform3D value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setTransform(id, value); };
    }
    static MotionAction position(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setPosition(id, value); };
    }
    static MotionAction scale(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component) return false;
            auto transform = component->transform;
            transform.scale = value;
            return scene.setTransform(id, transform);
        };
    }
    static MotionAction rotation(Scene& scene, EntityId id, glm::mat3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component) return false;
            auto transform = component->transform;
            transform.rotation = value;
            return scene.setTransform(id, transform);
        };
    }
    static MotionAction tint(Scene& scene, EntityId id, GlColor value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setTint(id, value); };
    }
    static MotionAction pivot(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            return scene.setLocalBasis(id, PivotMode::Custom, value);
        };
    }
    static MotionAction visible(Scene& scene, EntityId id, bool value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setVisible(id, value); };
    }
    static MotionAction faceVisible(Scene& scene, EntityId id, std::size_t face, bool value) {
        return [&scene, id, face, value](double, const MotionSample&) { return scene.setFaceVisible(id, face, value); };
    }
    static MotionAction faceRotation(Scene& scene, EntityId id, std::size_t face, FaceRotationConfig config) {
        return [&scene, id, face, config](double, const MotionSample&) {
            return PatternSystem::rotateFace(scene, id, face, config);
        };
    }
    static MotionAction faceMode(Scene& scene, EntityId id, std::size_t face, FaceTransformMode mode,
                                 bool preserveWorld = true) {
        return [&scene, id, face, mode, preserveWorld](double, const MotionSample&) {
            return scene.setFaceTransformMode(id, face, mode, preserveWorld);
        };
    }
    static MotionAction layer(Scene& scene, EntityId id, std::optional<LayerId> value) {
        return [&scene, id, value](double, const MotionSample&) {
            return value ? scene.bindEntity(id, *value) : scene.unbindEntity(id);
        };
    }
    static MotionAction motion(Scene& scene, LayerId layer, std::optional<MotionId> value) {
        return [&scene, layer, value](double, const MotionSample&) { return scene.getMotions().bindMotion(layer, value); };
    }
    static MotionAction parent(Scene& scene, EntityId id, std::optional<EntityId> value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setParent(id, value); };
    }
    static MotionAction camera(Camera& camera, Camera value) {
        return [&camera, value](double, const MotionSample&) {
            if (!CameraSystem::validCamera(value)) return false;
            camera = value; return true;
        };
    }
};
} // namespace graphics
```

## framework/include/Application.h

### 기존 23행 / 변경 23행

before:

```cpp
    bool verticalSync = true;
    DiagnosticOptions diagnostics;
};

```

after:

```cpp
    bool verticalSync = true;
    DiagnosticOptions diagnostics;
    RenderConfig renderConfig;
};

```

### 기존 110행 / 변경 111행

before:

```cpp
    /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.
    int run(const ApplicationConfig& config = ApplicationConfig{});
    /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
    void requestClose();
```

after:

```cpp
    /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.
    int run(const ApplicationConfig& config = ApplicationConfig{});
    /// [API] 현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.
    bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {},
                           const std::filesystem::path& fragmentPath = {});
    /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
    void requestClose();
```

## framework/include/CuboidMode.h

### 기존 1행 / 변경 1행

before:

```cpp
// 없음
```

after:

```cpp
#pragma once
#include "Actions.h"

namespace graphics {
struct CuboidFaceConfig {
    Transform3D transform;
    GlColor color;
    bool visible = true;
    CullMode cull = CullMode::Inherit;
    FaceTransformMode mode = FaceTransformMode::Hierarchy;
    std::optional<FaceRotationConfig> rotation;
};
struct CuboidConfig {
    std::string name;
    glm::vec3 size{1};
    Transform3D transform;
    GlColor color;
    bool visible = true;
    std::array<CuboidFaceConfig, 6> faces;
    std::optional<Motion> motion;
};
struct CuboidModeConfig {
    std::vector<CuboidConfig> cuboids;
    Camera camera;
    RenderOptions renderOptions;
};

enum class CuboidOperation {
    Position, Translate, Rotate, Scale, Transform, Pivot, Visible, ToggleVisible, Layer, Motion, Playing
};
struct CuboidCommand {
    CuboidOperation operation = CuboidOperation::Position;
    glm::vec3 value{0}; // Rotate: local axis; Scale: absolute scale.
    float degrees = 0;
    Transform3D transform;
    bool enabled = true;
    std::optional<LayerId> layer;
    std::optional<MotionId> motion;
};
class CuboidMode;
using CuboidHandler = std::function<bool(Application&, CuboidMode&)>;
struct CuboidKeyBinding {
    int key = 0, action = GLFW_PRESS, mods = 0;
    CuboidHandler handler;
};

// Configuration and bindings are data. Scene stores ECS components; this class coordinates systems.
class CuboidMode {
    CuboidModeConfig config_;
    std::vector<CuboidKeyBinding> bindings_;
    std::map<std::string, EntityId> entities_;
    std::map<std::string, LayerId> layers_;
    std::map<std::string, MotionId> motions_;
    CuboidHandler setup_;
    std::function<bool(Application&, CuboidMode&, float)> update_;
public:
    /// [API] 초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.
    bool setConfig(const CuboidModeConfig& config);
    const CuboidModeConfig& getConfig() const { return config_; }
    /// [API] 같은 이름의 육면체 초기 설정을 교체하거나 새로 등록한다.
    bool setCuboid(const CuboidConfig& config);
    /// [API] 같은 key/action/mods 바인딩을 교체하거나 추가한다.
    bool bindKey(const CuboidKeyBinding& binding);
    bool unbindKey(int key, int action = GLFW_PRESS, int mods = 0);
    const std::vector<CuboidKeyBinding>& getBindings() const { return bindings_; }
    void setSetupHandler(CuboidHandler handler) { setup_ = std::move(handler); }
    void setUpdateHandler(std::function<bool(Application&, CuboidMode&, float)> handler) { update_ = std::move(handler); }
    /// [API] 별도 CPU Scene에 육면체와 모션을 구성하고 성공하면 장면 전체를 교체한다.
    bool apply(Application& app);
    bool update(Application& app, float dt);
    bool dispatch(Application& app, int key, int action, int mods);
    std::optional<EntityId> entity(const std::string& name) const;
    std::optional<LayerId> layer(const std::string& name) const;
    std::optional<MotionId> motion(const std::string& name) const;
    /// [API] 실행 시 이름을 해석하므로 초기 설정 재적용 뒤에도 같은 바인딩을 사용한다.
    static CuboidHandler action(std::string name, CuboidCommand command);
};
/// [API] 육면체 모드의 init/update/수정키 콜백을 설치한다. 기존 해당 핸들러를 교체한다.
bool bindCuboidMode(Application& app, std::shared_ptr<CuboidMode> mode);
} // namespace graphics
```

## framework/include/Graphics.h

### 기존 4행 / 변경 4행

before:

```cpp
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

```

after:

```cpp
#include "Motion.h"
#include "../detail/Native.h"
#include "RenderConfig.h"
#include <map>

```

### 기존 56행 / 변경 57행

before:

```cpp
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);
    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);
    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);
```

after:

```cpp
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);
    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);
    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);
```

### 기존 98행 / 변경 100행

before:

```cpp
    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);

    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
```

after:

```cpp
    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);
    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);

    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
```

### 기존 264행 / 변경 268행

before:

```cpp
    };

    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
```

after:

```cpp
    };

    RenderConfig config_;
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
```

### 기존 293행 / 변경 298행

before:

```cpp
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{},
                    const std::filesystem::path& fragmentPath = std::filesystem::path{});
    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
```

after:

```cpp
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{},
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});
    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
```

## framework/include/Model.h

### 기존 35행 / 변경 35행

before:

```cpp
enum class OutlineMode { None, FeatureEdges, AllTriangleEdges, Explicit };
enum class CullMode { Inherit, None, Back, Front };

struct Transform3D {
```

after:

```cpp
enum class OutlineMode { None, FeatureEdges, AllTriangleEdges, Explicit };
enum class CullMode { Inherit, None, Back, Front };
enum class FaceTransformMode { Hierarchy, Independent };
enum class FaceAnimationSpace { Local, Model };

struct Transform3D {
```

### 기존 88행 / 변경 90행

before:

```cpp
    std::string name;
    CullMode cull = CullMode::Inherit;

    /// [API] 정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.
```

after:

```cpp
    std::string name;
    CullMode cull = CullMode::Inherit;
    FaceTransformMode transformMode = FaceTransformMode::Hierarchy;
    glm::mat4 transformFrame{1.0f}; // Parent-local correction, or a frozen world frame.
    FaceAnimationSpace animationSpace = FaceAnimationSpace::Local;

    /// [API] 정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.
```

### 기존 184행 / 변경 189행

before:

```cpp
        return false;

    if (m.localVertices.size() < 3 || !validColor(m.color) || !validColor(m.material.color) ||
        !finiteVector(m.normal) || !finiteMatrix(m.localAnimationMatrix) || !std::isfinite(m.creaseAngleDegrees) ||
```

after:

```cpp
        return false;

    if ((m.transformMode != FaceTransformMode::Hierarchy && m.transformMode != FaceTransformMode::Independent) ||
        !finiteMatrix(m.transformFrame) ||
        (m.animationSpace != FaceAnimationSpace::Local && m.animationSpace != FaceAnimationSpace::Model))
        return false;

    if (m.localVertices.size() < 3 || !validColor(m.color) || !validColor(m.material.color) ||
        !finiteVector(m.normal) || !finiteMatrix(m.localAnimationMatrix) || !std::isfinite(m.creaseAngleDegrees) ||
```

### 기존 269행 / 변경 279행

before:

```cpp
}

inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
    glm::mat4 child(1);

    if (face.independence) {
        // Rotate/scale around a face pivot without moving an identity child.
        child = glm::translate(glm::mat4(1), face.transform.localBasis) * transformMatrix(face.transform);
    }

    return transformMatrix(c.transform) * child * face.localAnimationMatrix;
}
} // namespace detail
} // namespace graphics
```

after:

```cpp
}

inline glm::mat4 faceSrtMatrix(const Mesh& face) {
    return face.independence
        ? glm::translate(glm::mat4(1), face.transform.localBasis) * transformMatrix(face.transform)
        : glm::mat4(1);
}

inline glm::mat4 faceLocalMatrix(const Mesh& face) {
    const auto child = faceSrtMatrix(face);
    return face.animationSpace == FaceAnimationSpace::Model
        ? face.localAnimationMatrix * child : child * face.localAnimationMatrix;
}

inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
    const auto parent = face.transformMode == FaceTransformMode::Hierarchy ? transformMatrix(c.transform) : glm::mat4(1);
    return parent * face.transformFrame * faceLocalMatrix(face);
}

} // namespace detail
} // namespace graphics
```

## framework/include/Motion.h

### 기존 2행 / 변경 2행

before:

```cpp
#include "Transform.h"
#include <map>

namespace graphics {
```

after:

```cpp
#include "Transform.h"
#include <map>
#include <initializer_list>

namespace graphics {
```

### 기존 13행 / 변경 14행

before:

```cpp
    glm::mat4 local{1.0f};
};

struct RotationMotion {
```

after:

```cpp
    glm::mat4 local{1.0f};
};

using MotionAction = std::function<bool(double, const MotionSample&)>;

struct RotationMotion {
```

### 기존 55행 / 변경 58행

before:

```cpp
    std::optional<double> durationSeconds;
    bool enabled = true;
};

```

after:

```cpp
    std::optional<double> durationSeconds;
    bool enabled = true;
    MotionAction action; // A step contains either transform or action.
};

```

### 기존 60행 / 변경 64행

before:

```cpp
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);

public:
```

after:

```cpp
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
    std::optional<MotionSample> evaluate(double seconds, bool actions) const;

public:
```

### 기존 66행 / 변경 71행

before:

```cpp
    /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
    bool insert(std::size_t index, const MotionStep& step);
    /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
    bool replace(std::size_t index, const MotionStep& step);
```

after:

```cpp
    /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
    bool insert(std::size_t index, const MotionStep& step);
    /// [API] 변환 사이의 인덱스에 의미 동작 또는 람다·함수 포인터를 삽입한다.
    bool insertAction(std::size_t index, MotionAction action, double startSeconds = 0,
                      std::optional<double> durationSeconds = std::nullopt);
    /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
    bool replace(std::size_t index, const MotionStep& step);
```

### 기존 78행 / 변경 86행

before:

```cpp
    }

    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성하여 MotionSample을 반환한다.
    std::optional<MotionSample> sample(double seconds) const;
    /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
    static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);
```

after:

```cpp
    }

    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다. 의미 동작은 실행하지 않는다.
    std::optional<MotionSample> sample(double seconds) const;
    /// [API] 변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.
    std::optional<MotionSample> execute(double seconds) const;
    /// [API] 기존 append/insert/then/replace와 조립하는 비변환 동작 단계를 만든다.
    static MotionStep action(MotionAction action, double startSeconds = 0,
                             std::optional<double> durationSeconds = std::nullopt);
    /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
    static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);
```

### 기존 128행 / 변경 141행

before:

```cpp
    /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
    std::optional<MotionId> createMotion(const Motion& motion);
    /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
    bool setMotion(MotionId id, const Motion& motion);
```

after:

```cpp
    /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
    std::optional<MotionId> createMotion(const Motion& motion);
    /// [API] 의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.
    std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);
    /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
    bool setMotion(MotionId id, const Motion& motion);
```

### 기존 140행 / 변경 155행

before:

```cpp
    /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
    bool removeMotion(MotionId id);
    /// [API] 시간 또는 프레임 상태를 진행한다. MotionSystem은 임시 샘플을 모은 뒤 성공한 경우 반영한다.
    bool update(double deltaSeconds);
    /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
```

after:

```cpp
    /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
    bool removeMotion(MotionId id);
    /// [API] 모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.
    bool update(double deltaSeconds);
    /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
```

### 기존 166행 / 변경 181행

before:

```cpp
    /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
    std::optional<MotionId> createOrbit(RotationMotion config);
    /// [API] 월드 공전과 로컬 자전을 하나의 모션에 등록한다.
    std::optional<MotionId> createOrbitAndSpin(RotationMotion orbit, RotationMotion spin);
    /// [API] 경로 이동과 로컬 자전을 하나의 모션에 등록한다.
    std::optional<MotionId> createPathAndSpin(const PathMotion& path, RotationMotion spin);
};
} // namespace graphics
```

after:

```cpp
    /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
    std::optional<MotionId> createOrbit(RotationMotion config);

};
} // namespace graphics
```

## framework/include/Patterns.h

### 기존 97행 / 변경 97행

before:

```cpp
};

class PatternSystem {
public:
```

after:

```cpp
};

enum class FaceRotationKind { EdgeHinge, SelfSpin, ModelOrbit };
struct FaceRotationConfig {
    FaceRotationKind kind = FaceRotationKind::EdgeHinge;
    std::size_t edge = 0; // Cyclic boundary edge of the original polygon.
    glm::vec3 axis{0, 1, 0}; // Model-local direction for self spin / model orbit.
    double degrees = 0; // Absolute angle, not a frame increment.
};

class PatternSystem {
public:
```

### 기존 122행 / 변경 130행

before:

```cpp
    /// [API] 지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.
    static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);
    /// [API] 진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.
    static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement,
```

after:

```cpp
    /// [API] 지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.
    static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);
    /// [API] 원래 모서리 힌지·면 중심 자전·원본 모델 중심 공전을 구분하여 면 회전 행렬을 설정한다.
    static bool rotateFace(Scene& scene, EntityId entity, std::size_t face, const FaceRotationConfig& config);
    /// [API] 진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.
    static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement,
```

### 기존 129행 / 변경 139행

before:

```cpp
    /// [API] 선택 옆벽의 정점에서 상단 피벗을 계산하고 openFace로 높이를 축소/복원한다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
    /// [API] 두 ID의 위치는 유지하고 모델을 교환한다.
    static bool swapModels(Scene& scene, EntityId first, EntityId second);
    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
```

after:

```cpp
    /// [API] 선택 옆벽의 정점에서 상단 피벗을 계산하고 openFace로 높이를 축소/복원한다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
```

## framework/include/RenderConfig.h

### 기존 1행 / 변경 1행

before:

```cpp
// 없음
```

after:

```cpp
#pragma once
#include "Model.h"
#include "../detail/Native.h"
#include <functional>

namespace graphics {
class Scene;

// Canonical CPU input for packing; independent of GL attribute locations.
struct RenderVertex {
    float position[3], color[3], uv[2], normal[3];
};

struct VertexAttribute {
    GLuint location = 0;
    GLint components = 3;
    GLenum type = GL_FLOAT;
    bool normalized = false;
    std::size_t offset = 0;
    bool integer = false; // ivec/uvec inputs use VertexAttribIPointer.
};

struct VertexLayout {
    std::size_t stride = sizeof(RenderVertex);
    std::vector<VertexAttribute> attributes{
        {0, 3, GL_FLOAT, false, offsetof(RenderVertex, position)},
        {1, 3, GL_FLOAT, false, offsetof(RenderVertex, color)},
        {2, 2, GL_FLOAT, false, offsetof(RenderVertex, uv)},
        {3, 3, GL_FLOAT, false, offsetof(RenderVertex, normal)}};
};

struct UniformBindings {
    std::string mvp = "uMVP", model = "uModel", normal = "uNormalMatrix";
    std::string texture = "uTexture", hasTexture = "uHasTexture", lighting = "uLighting";
    std::string lightDirection = "uLightDirection", lightColor = "uLightColor";
    std::string overrideColor = "uOverrideColor", outlineColor = "uOutlineColor", tint = "uTint";
};

struct DrawUniformContext {
    GLuint program;
    const Scene& scene;
    std::optional<EntityId> entity;
    std::optional<LineId> line;
    std::size_t face;
    glm::mat4 model, view, projection;
    bool depthOnly = false;
    /// [API] 현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.
    bool setUniform(const std::string& name, float value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !std::isfinite(value)) return false;
        detail::gl().Uniform1f(location, value); return true;
    }
    /// [API] 현재 program의 추가 int/bool/sampler uniform을 설정한다.
    bool setUniform(const std::string& name, int value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0) return false;
        detail::gl().Uniform1i(location, value); return true;
    }
    /// [API] 현재 program의 추가 vec3 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::vec3& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteVector(value)) return false;
        detail::gl().Uniform3f(location, value.x, value.y, value.z); return true;
    }
    /// [API] 현재 program의 추가 mat4 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::mat4& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteMatrix(value)) return false;
        detail::gl().UniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]); return true;
    }
};

struct RenderConfig {
    VertexLayout vertexLayout;
    UniformBindings uniforms;
    // Must output exactly input.size()*stride bytes, preserving vertex order/count.
    std::function<bool(const std::vector<RenderVertex>&, std::vector<std::uint8_t>&)> packVertices;
    // Runs with program bound, after built-in uniforms, before each range/pass.
    std::function<bool(const DrawUniformContext&)> bindUniforms;
};
} // namespace graphics
```

## tools/Write-ApiKo.py

### 기존 121행 / 변경 121행

before:

```python
'createSpin':'로컬 공간 회전 모션을 등록하여 자전에 사용한다.',
'createOrbit':'월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.',
'createOrbitAndSpin':'월드 공전과 로컬 자전을 하나의 모션에 등록한다.',
'createPathAndSpin':'경로 이동과 로컬 자전을 하나의 모션에 등록한다.',
'contains':'점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.',
'overlaps':'두 축 정렬 경계 상자가 겹치는지 검사한다.',
```

after:

```python
'createSpin':'로컬 공간 회전 모션을 등록하여 자전에 사용한다.',
'createOrbit':'월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.',
'contains':'점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.',
'overlaps':'두 축 정렬 경계 상자가 겹치는지 검사한다.',
```

### 기존 159행 / 변경 157행

before:

```python
('Scene','clear'):'Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.',
('MotionSystem','clear'):'모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.',
('Motion','sample'):'절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성하여 MotionSample을 반환한다.',
('DistancePath','sample'):'누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.',
('Motion','rotation'):'축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.',
```

after:

```python
('Scene','clear'):'Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.',
('MotionSystem','clear'):'모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.',
('Motion','sample'):'절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성한다. 의미 동작은 실행하지 않는다.',
('DistancePath','sample'):'누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.',
('Motion','rotation'):'축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.',
```

### 기존 203행 / 변경 201행

before:

```python
def description(r):
    owner,name=r['owner'],r['name']
    if '= delete' in r['signature']:return '이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.'
    if name==owner:return {'Application':'초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.','Scene':'빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.','Renderer':'GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.','AssetLoader':'프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.'}[owner]
    if name.startswith('~'):return '실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.'
    if name=='operator bool':return '모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.'
    if (owner,name) in SPECIAL:return SPECIAL[owner,name]
    if name in DESCRIPTIONS:return DESCRIPTIONS[name]
```

after:

```python
def description(r):
    owner,name=r['owner'],r['name']
    source_lines=(ROOT/r['file']).read_text(encoding='utf-8-sig').splitlines()
    preceding=r['line']-2
    if preceding>=0 and '/// [API]' in source_lines[preceding]:
        return source_lines[preceding].split('/// [API]',1)[1].strip()
    if '= delete' in r['signature']:return '이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.'
    if name==owner:return {'Application':'초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.','Scene':'빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.','Renderer':'GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.','AssetLoader':'프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.'}[owner]
    if name.startswith('~'):return '실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.'
    if name=='operator bool':return '모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.'
    additions={
        ('HumanoidPose','seated'):'허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다.',
        ('AnimationSystem','createJointAnimation'):'관절의 기준 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율이다.',
        ('AnimationSystem','poseHierarchy'):'기준 SRT와 모션을 합성하고 모든 관절이 유효하면 자세를 반영한다.',
        ('AnimationSystem','addHumanoid'):'편집 가능한 사람형 관절·메시 계층을 만든다. 구성 실패는 기존 Scene을 유지한다.',
        ('AnimationSystem','poseHumanoid'):'각 관절 위치를 유지하며 로컬 Euler 자세를 적용한다.',
        ('AnimationSystem','addSwingWithRider'):'고정 프레임·진자 관절·좌석·앉은 사람과 모션을 구성한다.',
        ('Motion','pendulum'):'축·진폭·주기·위상의 사인 진자 회전 단계를 만든다.',
        ('Scene','setFaceTransformMode'):'면의 계층 상속·월드 독립을 전환한다. 기본값은 현재 자세 보존이다.',
        ('PatternSystem','rotateFace'):'현재 면의 모서리 힌지·중심 자전·원본 모델 중심 공전을 구분해 회전을 설정한다.'}
    if (owner,name) in additions:return additions[owner,name]
    if owner=='SceneActions':return '지정 값과 대상 ID를 기존 API에 전달하는 '+name+' 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.'
    if owner=='DrawUniformContext':return '현재 바인딩된 program의 추가 uniform을 설정한다. 이름·GLSL 자료형과 값의 유한성을 맞춘다.'
    if owner=='CuboidMode':return {
        'setConfig':'초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.',
        'getConfig':'초기 설정의 읽기 전용 참조를 반환한다.',
        'setCuboid':'이름별 육면체 초기 설정을 교체하거나 추가한다.',
        'bindKey':'같은 key/action/mods 바인딩을 교체하거나 추가한다.',
        'unbindKey':'같은 key/action/mods 바인딩을 해제한다.',
        'getBindings':'키 바인딩 목록의 읽기 전용 참조를 반환한다.',
        'setSetupHandler':'새 장면 구성 후 실행할 설정 핸들러를 교체한다.',
        'setUpdateHandler':'프레임별 응용 갱신 핸들러를 교체한다.',
        'apply':'임시 CPU Scene에 육면체를 구성하고 성공하면 장면 전체를 교체한다.',
        'update':'등록된 모드 프레임 핸들러를 호출한다.',
        'dispatch':'일치하는 키 바인딩을 실행한다. 없는 키는 정상 처리한다.',
        'entity':'이름에 대응하는 마지막 생성 엔티티 ID를 조회한다.',
        'layer':'이름에 대응하는 초기 생성 레이어 ID를 조회한다.',
        'motion':'이름에 대응하는 초기 생성 모션 ID를 조회한다.',
        'action':'실행 시 육면체 이름을 해석하는 의미 명령 핸들러를 만든다.'}[name]
    if name=='bindCuboidMode':return '육면체 모드의 init/update/수정키 핸들러를 설치한다. 기존 해당 핸들러를 교체한다.'
    if name=='configureRenderer':return '현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.'
    if owner=='Motion' and name in ('action','insertAction','execute'):
        return {'action':'다른 단계와 조립할 의미 동작 단계를 만든다.',
                'insertAction':'주어진 인덱스에 람다·함수 포인터·의미 동작을 삽입한다.',
                'execute':'변환과 동작을 순서대로 평가한다. 외부 부수 효과는 복구하지 않는다.'}[name]
    if (owner,name) in SPECIAL:return SPECIAL[owner,name]
    if name in DESCRIPTIONS:return DESCRIPTIONS[name]
```

## tools/Write-CodeGuide.py

### 기존 118행 / 변경 118행

before:

```python
'screenOnPlane':'화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.',
'snap':'좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.',
'swapModels':'두 ID의 위치는 유지하고 모델을 교환한다.',
'reset':'응용 시간·ID·계층·카메라·입력과 모드를 초기 상태로 복구한다.',
'placeMovingObjects':'상자와 줄무늬가 공유하는 거리 시계에 offset을 더해 각 최종 자세를 계산한다.',
```

after:

```python
'screenOnPlane':'화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.',
'snap':'좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.',
'reset':'응용 시간·ID·계층·카메라·입력과 모드를 초기 상태로 복구한다.',
'placeMovingObjects':'상자와 줄무늬가 공유하는 거리 시계에 offset을 더해 각 최종 자세를 계산한다.',
```

## tools/author_framework_guides.py

### 기존 28행 / 변경 28행

before:

```python

def apis():
    result = []
    for p in sorted((ROOT/'framework/include').glob('*.h')):
        s = p.read_text(encoding='utf-8-sig')
        mask = re.sub(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"', lambda m: ''.join('\n' if c=='\n' else ' ' for c in m.group()), s)
        spans = []
        for m in re.finditer(r'\b(?:class|struct)\s+(\w+)\s*\{', mask):
            start=m.end()-1; depth=1; end=start+1
            while end<len(mask) and depth:
                depth += (mask[end]=='{')-(mask[end]=='}'); end+=1
            spans.append((start,end,m.group(1)))
        for m in re.finditer(r'^[ \t]*///\s*\[API\]\s*(.+)\n',s,re.M):
            start=m.end(); i=start; parens=0
            while i<len(s):
                c=s[i]
                if c=='(': parens+=1
                elif c==')': parens-=1
                elif c in ';{' and parens==0: break
                i+=1
            signature=' '.join(s[start:i].strip().split())
            if '(' not in signature: continue
            prefix=signature.split('(',1)[0]
            nm=re.search(r'(operator\s+bool|operator\s*=|~?\w+)\s*$',prefix)
            if not nm: continue
            name=nm.group(1)
            if re.search(r'\w+::'+re.escape(name)+r'\s*$',prefix):continue
            enclosing=[v for v in spans if v[0]<start<v[1]]
            owner=min(enclosing,key=lambda v:v[1]-v[0])[2] if enclosing else ''
            result.append(dict(owner=owner,name=name,signature=signature,file=p.relative_to(ROOT).as_posix(),line=s[:start].count('\n')+1,description=m.group(1)))
            if name=='readTextFile': result[-1]['description']='파일을 문자열로 읽고 UTF-8 BOM을 제거한다. 파일을 열지 못하면 빈 문자열을 반환한다.'
    return result

GROUPS={}
```

after:

```python

def apis():
    # Reuse the public-declaration extractor so inline APIs without generated comments are included.
    import importlib.util
    spec = importlib.util.spec_from_file_location('framework_api_catalog', ROOT/'tools/Write-ApiKo.py')
    catalog = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(catalog)
    result = []
    for path in sorted((ROOT/'framework/include').glob('*.h')):
        for record in catalog.declarations(path):
            record['description'] = catalog.description(record)
            record['result'] = catalog.result_contract(record)
            result.append(record)
    return result


GROUPS={}
```

### 기존 293행 / 변경 276행

before:

```python
    auto spin=system.createSpin(rotation);
    auto orbit=system.createOrbit(rotation);
    auto combined=system.createOrbitAndSpin(rotation,rotation);
    graphics::PathMotion path; path.path=graphics::PathSystem::circle({0,0,0},1);
    auto moving=system.createPathAndSpin(path,rotation);
    if(!spin||!orbit||!combined||!moving) return false;
    system.clear(); // 기존 레이어 연결을 사용하지 않는 별도 예제 종료 지점
```

after:

```python
    auto spin=system.createSpin(rotation);
    auto orbit=system.createOrbit(rotation);
    auto orbitConfig=rotation; orbitConfig.space=graphics::TransformSpace::World;
    auto spinConfig=rotation; spinConfig.space=graphics::TransformSpace::Local;
    auto combined=system.createMotion({graphics::Motion::rotation(orbitConfig), graphics::Motion::rotation(spinConfig)});
    graphics::PathMotion path; path.path=graphics::PathSystem::circle({0,0,0},1);
    auto moving=system.createMotion({graphics::Motion::followPath(path), graphics::Motion::rotation(spinConfig)});
    if(!spin||!orbit||!combined||!moving) return false;
    system.clear(); // 기존 레이어 연결을 사용하지 않는 별도 예제 종료 지점
```

### 기존 499행 / 변경 484행

before:

```python
    if(selected&&!graphics::PatternSystem::openRoomWall(scene,room,*selected,.5f)) return false;
    auto other=scene.addCuboid({1,1,1},{2,0,0}); if(!other) return false;
    if(!graphics::PatternSystem::swapModels(scene,body,*other)) return false;
    if(!graphics::PatternSystem::moveCamera(camera,{.2f,0,0})
       ||!graphics::PatternSystem::yawCamera(camera,5)
```

after:

```python
    if(selected&&!graphics::PatternSystem::openRoomWall(scene,room,*selected,.5f)) return false;
    auto other=scene.addCuboid({1,1,1},{2,0,0}); if(!other) return false;
    auto staged=scene;
    const auto firstModel=scene.getComponent(body)->polyhedron;
    const auto secondModel=scene.getComponent(*other)->polyhedron;
    if(!staged.setModel(body,secondModel)||!staged.setModel(*other,firstModel)) return false;
    scene=std::move(staged);
    if(!graphics::PatternSystem::moveCamera(camera,{.2f,0,0})
       ||!graphics::PatternSystem::yawCamera(camera,5)
```

### 기존 567행 / 변경 556행

before:

```python
''','시계를 정지해도 seconds는 유지되므로 같은 시각에서 편집 결과를 즉시 볼 수 있다. 입력 후 재생을 재개해도 rest가 남는다. standard의 handleJointKey는 실패 시 rest를 복원하고, R은 선택 관절의 최초 rest를 복원한다.',('standard/Console.h','standard/Console.cpp'))

def choose(a):
    o,n=a['owner'],a['name']
    if o=='Application': return 'assets' if n=='getAssets' else 'app'
    if o=='Scene':
        if n in 'setFaceVisible setFaceCullMode setFaceLocalMatrix setPosition setTransform setParent worldMatrix faceWorldMatrix setFaceTransform setLocalBasis splitFace setFaceSRT addGroup'.split():return 'transform'
```

after:

```python
''','시계를 정지해도 seconds는 유지되므로 같은 시각에서 편집 결과를 즉시 볼 수 있다. 입력 후 재생을 재개해도 rest가 남는다. standard의 handleJointKey는 실패 시 rest를 복원하고, R은 선택 관절의 최초 rest를 복원한다.',('standard/Console.h','standard/Console.cpp'))

group('editable','의미 동작·육면체 모드·셰이더 연결 편집',
 '설정과 작은 핸들러를 편집하여 기존 Scene/ECS에 동작을 연결한다. 세부 계약과 내부 코드 수정 순서는 MODE_ACTIONS.md, RENDER_CONFIGURATION.md, PATTERN_COMPOSITION.md를 읽는다.',
 'mode는 설정과 이름 바인딩을 보관한다. Scene은 컴포넌트 데이터, MotionSystem은 시간, Renderer는 GPU 자원을 소유한다.',r'''
bool configureEditable(graphics::Application& app) {
    auto mode=std::make_shared<graphics::CuboidMode>();
    graphics::CuboidConfig box; box.name="box"; box.size={1,2,1};
    if(!mode->setCuboid(box)) return false;
    graphics::CuboidCommand command;
    command.operation=graphics::CuboidOperation::Translate; command.value={.1f,0,0};
    if(!mode->bindKey({GLFW_KEY_RIGHT,GLFW_PRESS,0,graphics::CuboidMode::action("box",command)})) return false;
    mode->setSetupHandler([](graphics::Application& app,graphics::CuboidMode& mode) {
        const auto id=mode.entity("box"); if(!id) return false;
        graphics::Motion motion;
        graphics::RotationMotion spin;
        if(!motion.append(graphics::Motion::rotation(spin))) return false;
        if(!motion.insertAction(1,graphics::SceneActions::visible(app.getScene(),*id,true))) return false;
        const auto track=app.getScene().getMotions().createMotion(motion); if(!track) return false;
        graphics::LayerConfig layer; layer.motion=*track;
        const auto binding=app.getScene().getMotions().pushLayer(graphics::MotionSystem::rootLayer(),layer);
        return binding && app.getScene().bindEntity(*id,*binding);
    });
    return graphics::bindCuboidMode(app,mode);
}
// Run before app.run(config), not inside an existing init callback.
// config.renderConfig.bindUniforms = [](const graphics::DrawUniformContext& context) {
//     return context.setUniform("uAmplitude", .1f);
// };
''','sample/seek는 의미 동작을 실행하지 않는다. 모션 갱신은 활성 동작을 실행한다. mode.apply는 Scene 전체를 교체한다. 사용자 셰이더 입력은 실제 바이트와 맞춰야 한다.',
 ('framework/include/RenderConfig.h','framework/include/Actions.h','framework/include/CuboidMode.h','framework/detail/CuboidMode.cpp'))

group('face-modes','면의 독립·계층과 힌지·자전·공전',
 '육면체의 변환 상속과 면 자체의 회전 중심을 별도 설정으로 조합한다.',
 'FaceTransformMode는 상속, FaceRotationKind는 축의 위치/방향, FaceAnimationSpace는 면 SRT와 추가 행렬의 적용 순서다.',r'''
bool faceMotion(graphics::Scene& scene,graphics::EntityId box,std::size_t face) {
    graphics::Transform3D pose; pose.basis={0,.2f,0};
    if(!scene.setFaceSRT(box,face,pose)) return false;
    if(!scene.setFaceTransformMode(box,face,graphics::FaceTransformMode::Independent)) return false;
    graphics::FaceRotationConfig rotation;
    rotation.kind=graphics::FaceRotationKind::EdgeHinge;
    rotation.edge=0; rotation.degrees=90;
    if(!graphics::PatternSystem::rotateFace(scene,box,face,rotation)) return false;
    rotation.kind=graphics::FaceRotationKind::SelfSpin; rotation.axis={0,1,0};
    if(!graphics::PatternSystem::rotateFace(scene,box,face,rotation)) return false;
    rotation.kind=graphics::FaceRotationKind::ModelOrbit;
    if(!graphics::PatternSystem::rotateFace(scene,box,face,rotation)) return false;
    graphics::Motion actions;
    if(!actions.append(graphics::Motion::action(graphics::SceneActions::faceRotation(scene,box,face,rotation)))) return false;
    return actions.insertAction(1,graphics::SceneActions::faceMode(scene,box,face,graphics::FaceTransformMode::Hierarchy));
}
''','독립 면은 전환 순간의 기준을 고정하며 기본 계층 모드는 본체 변환을 따른다. 위 회전 호출은 각 종류의 시연이며 마지막 행렬이 최종 값이다. 자세 보존과 적용 순서는 FACE_TRANSFORMS.md를 읽는다.',
 ('framework/include/Model.h','framework/detail/Scene.cpp','framework/include/Patterns.h','framework/detail/Patterns.cpp'))

def choose(a):
    o,n=a['owner'],a['name']
    if (o=='Scene' and n=='setFaceTransformMode') or (o=='PatternSystem' and n=='rotateFace') or (o=='SceneActions' and n in ('faceMode','faceRotation')):return 'face-modes'
    if o=='Application': return 'editable' if n=='configureRenderer' else ('assets' if n=='getAssets' else 'app')
    if o=='Scene':
        if n in 'setFaceVisible setFaceCullMode setFaceLocalMatrix setPosition setTransform setParent worldMatrix faceWorldMatrix setFaceTransform setLocalBasis splitFace setFaceSRT addGroup'.split():return 'transform'
```

### 기존 580행 / 변경 622행

before:

```python
    if o in ('AssetLoader','ModelResult','TextureResult','TextureImage'):return 'assets'
    if o in ('PlaneFigure','Polyhedron'):return 'mesh'
    if o=='Motion':return 'motion'
    if o=='MotionSystem':return 'layers'
```

after:

```python
    if o in ('AssetLoader','ModelResult','TextureResult','TextureImage'):return 'assets'
    if o in ('PlaneFigure','Polyhedron'):return 'mesh'
    if o in ('SceneActions','CuboidMode','DrawUniformContext') or n in ('bindCuboidMode','configureRenderer'):return 'editable'
    if o=='Motion':return 'motion'
    if o=='MotionSystem':return 'layers'
```

### 기존 660행 / 변경 703행

before:

```python
PATTERNS=[
 ('bootstrap','콜백으로 응용 상태 연결','app',['상태와 ID 보관','init에서 Scene 생성','입력으로 상태 변경','update로 변환 반영','렌더·종료는 Application에 위임']),
 ('spin-orbit','자전과 공전을 분리','layers',['공전축·자전축 지정','createOrbitAndSpin으로 모션 등록','레이어에 모션 연결','객체 바인딩 → 자동 시간 진행']),
 ('path','경로 전환·왕복','math',['PathSystem으로 경로 생성','PathMotion의 기간·재생 정책 지정','Motion.followPath 구성','MotionSystem.setMotion → seekMotion(0)']),
 ('sequence','순차 동작과 끝 자세 유지','motion',['유한 duration의 단계 생성','append로 첫 단계 등록','then으로 다음 시작 시각 배치','sample이 종료 자세를 유지']),
```

after:

```python
PATTERNS=[
 ('bootstrap','콜백으로 응용 상태 연결','app',['상태와 ID 보관','init에서 Scene 생성','입력으로 상태 변경','update로 변환 반영','렌더·종료는 Application에 위임']),
 ('spin-orbit','자전과 공전을 분리','layers',['공전축·자전축 지정','createMotion 단계 목록으로 모션 등록','레이어에 모션 연결','객체 바인딩 → 자동 시간 진행']),
 ('path','경로 전환·왕복','math',['PathSystem으로 경로 생성','PathMotion의 기간·재생 정책 지정','Motion.followPath 구성','MotionSystem.setMotion → seekMotion(0)']),
 ('sequence','순차 동작과 끝 자세 유지','motion',['유한 duration의 단계 생성','append로 첫 단계 등록','then으로 다음 시작 시각 배치','sample이 종료 자세를 유지']),
```

### 기존 683행 / 변경 726행

before:

```python
 ('views','여러 카메라로 같은 Scene 표시','parts',['update에서 Scene 상태 한 번 갱신','threeViews로 카메라·픽셀 영역 구성','Application.renderViews 호출','패스마다 같은 시간의 장면 표시'])]

def main():
    entries=apis()
    intro=para('함수 이름을 외우는 대신 실제 상황에서 무엇을 저장하고 무엇을 바꾸는지 읽는다. 공개 함수·오버로드·생성자·소멸자·삭제된 복사 연산을 현재 framework/include 헤더에서 수록했다. graphics::detail 및 private 구현은 공개 호출 대상으로 나열하지 않고 공통 구현 설명에서 다룬다.')
    intro+='<p class="note">모든 코드는 현재 선언·구현에 근거한 문서다. 새 사용 사례는 함께 쓸 API를 묶은 함수 단편이며, 독립 main인 예제는 별도로 표시한다. 실행 성공을 확인한 코드라는 뜻은 아니다. 외부 자산 경로는 사용자가 준비한다.</p>'
    body=intro+runtime()+'<section id="cases"><h2>함께 사용하는 API의 실제 사례 코드</h2>'+para('기본 포함은 framework/Framework.h다. 출력 사례는 iostream, 예외 처리는 stdexcept를 추가한다. 아래 독립 함수들은 graphics 이름을 완전히 적었고, 입력 Scene/ID/컨텍스트의 전제는 각 사례에 명시한다.')+'</section>'
```

after:

```python
 ('views','여러 카메라로 같은 Scene 표시','parts',['update에서 Scene 상태 한 번 갱신','threeViews로 카메라·픽셀 영역 구성','Application.renderViews 호출','패스마다 같은 시간의 장면 표시'])]

PATTERNS.append(('editable','의미 동작·육면체 모드·정점 입력 편집','editable',['초기 설정과 의미 동작 구성','이름 기반 키 바인딩 등록','프레임당 모션 갱신과 동작 실행','RenderConfig로 정점 입력·uniform 연결']))

PATTERNS.append(('face-modes','면의 독립·계층과 힌지·자전·공전','face-modes',['변환 상속 모드 선택','면 SRT 설정','회전 종류와 축 설정','본체 변환과 면 변환을 합성']))

def main():
    entries=apis()
    intro='<p><a href="MODE_ACTIONS.md">의미 동작·육면체 모드</a> · <a href="RENDER_CONFIGURATION.md">정점 입력·내부 수정 순서</a> · <a href="PATTERN_COMPOSITION.md">패턴 조립·API 압축</a></p>'+para('함수 이름을 외우는 대신 실제 상황에서 무엇을 저장하고 무엇을 바꾸는지 읽는다. 공개 함수·오버로드·생성자·소멸자·삭제된 복사 연산을 현재 framework/include 헤더에서 수록했다. graphics::detail 및 private 구현은 공개 호출 대상으로 나열하지 않고 공통 구현 설명에서 다룬다.')
    intro+='<p class="note">모든 코드는 현재 선언·구현에 근거한 문서다. 새 사용 사례는 함께 쓸 API를 묶은 함수 단편이며, 독립 main인 예제는 별도로 표시한다. 실행 성공을 확인한 코드라는 뜻은 아니다. 외부 자산 경로는 사용자가 준비한다.</p>'
    body=intro+runtime()+'<section id="cases"><h2>함께 사용하는 API의 실제 사례 코드</h2>'+para('기본 포함은 framework/Framework.h다. 출력 사례는 iostream, 예외 처리는 stdexcept를 추가한다. 아래 독립 함수들은 graphics 이름을 완전히 적었고, 입력 Scene/ID/컨텍스트의 전제는 각 사례에 명시한다.')+'</section>'
```

