# 전체 코드 변경 전후

작업 시작 기준: 4eb0486cd770a6a302ac8d27b7b2f8667e21a0ea

단계 1~4의 API 확장, 5의 기존 벽 동작 복원, 6의 코드 모음 작성 도구를 포함한다. 원본 코드 블록을 짝지어 제공한다. 사본은 같은 원본 변경을 받았다. 빌드·테스트는 하지 않았다.

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

### 기존 660행 / 변경 692행

before:

```cpp
                                   "Select one of the four side walls of addRoom().");

    glm::vec3 pivot(0);
    float top = mesh->localVertices.front().y;

    for (const auto& vertex : mesh->localVertices) {
        pivot += vertex;
        top = std::max(top, vertex.y);
    }
    pivot /= float(mesh->localVertices.size());
    pivot.y = top;

    return openFace(scene, room, face, progress, glm::vec3(0), pivot, true);
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

```

after:

```cpp
                                   "Select one of the four side walls of addRoom().");

    if (!std::isfinite(progress) || progress < 0 || progress > 1)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "openRoomWall",
                                   "Need progress in [0,1].");

    glm::vec3 pivot(0);

    for (const auto& vertex : mesh->localVertices)
        pivot += vertex;
    pivot /= float(mesh->localVertices.size());
    // Fully open faces are hidden; retain a nonsingular matrix for normals/picking.
    const float factor = std::max(1e-3f, 1 - progress);
    const auto scale = TransformSystem::scaling(glm::vec3(factor), pivot);

    return scale && scene.setFaceLocalMatrix(room, face, *scale) && scene.setFaceVisible(room, face, progress < 1);
}

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

### 기존 127행 / 변경 137행

before:

```cpp
    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);
    /// [API] 선택 옆벽의 정점에서 상단 피벗을 계산하고 openFace로 높이를 축소/복원한다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
    /// [API] 두 ID의 위치는 유지하고 모델을 교환한다.
    static bool swapModels(Scene& scene, EntityId first, EntityId second);
    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
```

after:

```cpp
    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);
    /// [API] 선택 옆벽의 원래 중심을 기준으로 모든 축을 균등 축소/복원하며 완전 개방 시 숨긴다.
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

## tools/Write-PracticeCookbook.py

### 기존 1행 / 변경 1행

before:

```python
// 없음
```

after:

```python
"""Author source-backed, standard-local 3D practice code blocks. No builds/tests/app execution."""
from pathlib import Path
import hashlib
import json
import re
import subprocess
from tree_sitter import Language, Parser
import tree_sitter_cpp

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / "standard/docs/practice-cookbook"
PARSER = Parser(Language(tree_sitter_cpp.language()))

# ID | requirement | input | implementation methods | behavior / important boundary
DATA = {
13: """
01|육면체·사각뿔의 초기 배치와 기울기|시작|initScene|초기 모델 회전과 표시용 카메라를 함께 확인한다.
02|움직이지 않는 XYZ 좌표축|시작|initScene|좌표축 선을 객체 owner 없이 생성한다.
03|육면체의 지정한 한 면만 표시|1~6|onKey,selectFaces|이전 표시를 먼저 지우고 선택 면만 켠다.
04|사각뿔의 지정한 옆면만 표시|7~0|onKey,selectFaces|삼각 옆면 인덱스 0~3을 선택한다.
05|서로 다른 육면체 두 면 무작위 선택|C|onKey,selectFaces|5개 후보에서 뽑은 두 번째 번호를 첫 번호 이후에서 보정한다.
06|사각뿔 바닥과 임의 옆면 표시|T|onKey,selectFaces|바닥 4와 옆면 0~3 하나를 함께 켠다.
07|면마다 다른 색과 깊이 가림|시작|initScene|기본 모델의 면 색과 RenderOptions.depthTest를 사용한다.
""",
14: """
01|같은 꼭짓점에 같은 색을 배정|시작|initScene|공통 assignVertexColors가 좌표 부호 3비트를 색 인덱스로 사용한다.
02|도형을 바꿔도 위치·회전 보존|C/P|selectShape,applyTransform|도형 선택 상태와 조작 변환을 분리한다.
03|은면 제거 켜기·끄기|H|onKey|렌더 옵션만 바꾼다.
04|와이어·솔리드 선택|w/W|onKey|Shift와 Caps Lock의 조합으로 대문자를 해석한다.
05|현재 로컬 X축 양·음 자전|x/X|onKey,onUpdate,applyTransform|회전 기저의 오른쪽에 증분 회전을 곱한다.
06|현재 로컬 Y축 양·음 자전|y/Y|onKey,onUpdate,applyTransform|초기 표시 각도를 별도 카메라에 둔다.
07|누르는 동안 XY 이동|방향키|onKey,onUpdate|키 해제에서 속도 입력을 지우며 dt로 이동한다.
08|객체가 움직여도 좌표축 고정|이동·회전|initScene,applyTransform|축 선을 모델 자식으로 넣지 않는다.
09|조작 상태·속도 초기화|S|resetMotion,onKey|이동·회전을 지우고 모든 애니메이션을 멈춘다.
10|포커스 이탈 때 지속 입력 해제|포커스|onFocus|키 해제 이벤트가 없어도 이동을 정지한다.
""",
15: """
01|모델 전체 Y 자전과 면 동작 중첩|Y|onKey,onUpdate,applyObjectTransform|본체 SRT와 면별 행렬을 따로 저장한다.
02|윗면 중심 Z축 자전|T|beginMotion,applyFaceMotion,onUpdate|Top case: 윗면 중심을 고정한다.
03|앞면 상단 모서리 힌지 열기·닫기|F|beginMotion,applyFaceMotion,onUpdate|Front case: 진행률 0~1을 -90도로 바꾼다.
04|왼쪽·오른쪽 면의 제자리 X축 회전|S|beginMotion,applyFaceMotion,onUpdate|Side case: 두 면의 중심을 각각 사용한다.
05|뒷면 중심 축소·소멸·복원|B|beginMotion,applyFaceMotion,onUpdate|Back case: 0 크기에 도달한 면을 숨긴다.
06|서로 이웃한 옆면 쌍 선택 유지|1/2 첫 입력|beginMotion|순환 쌍 0-3, 3-1, 1-2, 2-0과 공유 모서리를 함께 저장한다.
07|이웃한 두 면이 공유 모서리를 중심으로 360도 회전|1|beginMotion,applyFaceMotion,onUpdate|Hinge case: 선택한 두 면에 같은 모서리 회전을 적용한다.
08|이웃한 두 면의 반대 방향 90도 공전 교환|2|beginMotion,applyFaceMotion,onUpdate|Swap case: 0→90→0도의 삼각 위상으로 교환·복귀한다.
09|네 옆면의 모델 중심 Y축 공전|4|beginMotion,applyFaceMotion,onUpdate|OrbitY case: 면 0,1,2,3을 함께 돌린다.
10|위·아래·앞·뒤 면의 모델 중심 X축 공전|5|beginMotion,applyFaceMotion,onUpdate|OrbitX case: 면 4,5,0,1을 함께 돌린다.
11|동작별 정지·재개와 방향 반전 구분|반복 키|beginMotion,onUpdate|F/B는 방향 반전, 나머지는 각 동작의 running만 토글한다.
12|여러 면 동작의 고정 순서 합성|동시 입력|applyFaceMotion|매 프레임 항등 행렬에서 시작해 enum 순서로 왼쪽 곱한다.
13|도형 전환과 전체 리셋|P/R|selectShape,resetFaces,resetAll|면 상태 초기화와 본체 전체 초기화를 구분한다.
""",
16: """
01|실습 15의 육면체 동작 유지|T/F/S/B/1/2/4/5|beginMotion,applyFaceMotion,onUpdate|육면체 부분은 실습 15와 같은 상태·합성 패턴이다.
02|사각뿔 네 면을 아래에서 만나도록 열기|6|beginPyramid,applyPyramid,updatePyramid|All: 실제 기하에서 회전각을 구한다. PDF의 목표 자세와 행렬 각도를 구분한다.
03|사각뿔 네 면을 순서대로 열고 닫기|7|beginPyramid,applyPyramid,updatePyramid|8초 주기, 각 면의 2초 구간에서 0→평면→0을 계산한다.
04|마주보는 두 사각뿔 옆면의 꼭짓점 기준 교환|8|beginPyramid,applyPyramid,updatePyramid|Swap: 꼭대기를 지나는 Y축에서 두 면에 반대 부호를 준다.
05|사각뿔 꼭대기 고정 바깥쪽 펼치기|9|beginPyramid,applyPyramid,updatePyramid|Apex: 면별 축으로 -45도까지 연다.
06|사각뿔 모드 전환·정지·역방향 복귀|6~9 재입력|beginPyramid,selectShape|새 모드는 닫힌 자세에서 시작하고 완료한 교환은 반대 방향으로 복귀한다.
07|사각뿔 바닥의 고정|모든 사각뿔 동작|applyPyramid|면 4의 추가 행렬은 항등으로 둔다.
""",
17: """
01|왼쪽·오른쪽·두 객체의 선택|1/2/3|onKey|선택 상태로 개별 조작의 적용 대상을 정한다.
02|공전 그룹과 표시 메시 분리|시작|reset,apply,orbitRotation,worldPosition|공전 프레임 아래에 몸체의 위치·자전·크기를 둔다.
03|선택 객체의 로컬 X/Y 자전|x/X/y/Y|onKey,onUpdate,apply|자전 기저를 보관하고 수치 드리프트를 다시 직교화한다.
04|월드 원점 Y축 공전|r/R|onKey,orbitRotation,apply|본체 자전과 공전 그룹의 회전 상태를 분리한다.
05|제자리 확대·축소|a/A|onKey,apply|표시 크기만 바꾸고 위치는 유지한다.
06|원점 기준 위치와 크기 동시 확대·축소|b/B|onKey,apply|위치와 배율을 같은 비율로 바꾼다.
07|공전 중 화면 XY 방향 이동|d/D/e/E|onKey,onUpdate|원하는 월드 이동을 공전 회전의 전치로 부모 공간에 옮긴다.
08|두 객체의 원점을 통과하는 직선 교환|T|begin,onUpdate|시작 월드 위치를 저장하고 2초 동안 서로의 위치로 보간한다.
09|위·아래로 우회하는 교환|U|begin,onUpdate|직선 보간에 부호가 반대인 sin(pi*t) 높이를 더한다.
10|반대 위상 확대·축소와 자전·공전|V|begin,onUpdate|시작 배율과 위상을 저장하며 모드 정지는 합성 동작만 멈춘다.
11|진행 중 모드 전환과 같은 모드 정지·재개|T/U/V 반복|begin,onUpdate|전환 시 현재 월드 위치를 새 시작점으로 잡는다.
12|두 객체의 형상 순환과 SRT 유지|C|model,onKey,apply|원뿔→구→육면체 모델만 교체한다.
13|초기화·포커스 이탈 입력 정리|S/포커스|reset,onFocus|선택·모션·이동 입력을 초기 상태로 되돌린다.
""",
18: """
01|중심·행성·달의 계층 구성|시작|initScene|표시 메시 크기를 궤도 프레임 배율과 분리한다.
02|세 행성의 서로 다른 궤도 기울기·속도|자동|initScene,apply,onUpdate|XZ와 Z축 ±45도 기울기를 위치와 선에 동일 적용한다.
03|행성별 달의 공전과 자체 자전|자동|apply,onUpdate|행성 중심 프레임 아래에서 달 위치와 회전을 계산한다.
04|보이는 궤도와 실제 천체 경로 일치|시작·궤도 변환|initScene,apply|공통 반지름과 프레임을 선과 천체가 공유한다.
05|직각·원근 투영 선택|p/P|onKey|카메라 투영 상태만 바꾼다.
06|솔리드·와이어 선택|m/M|onKey|같은 장면을 다른 렌더 옵션으로 그린다.
07|전체 시스템 XYZ 이동|W/A/S/D/±|onKey,onUpdate,apply|루트의 위치만 바꾼다.
08|모든 궤도 반지름의 부드러운 확대·축소|1|onKey,onUpdate,apply|현재 반지름을 목표 반지름으로 보간한다.
09|중심 구의 크기 진동과 색 변화|2|onKey,onUpdate,apply|중심 메시만 바꾸며 행성 거리에는 전파하지 않는다.
10|달의 궤도 프레임과 달의 함께 회전|3|onKey,onUpdate,apply|궤도 그룹 회전과 달 로컬 자전을 분리한다.
11|전체 이동 중 포커스 해제|포커스|onFocus|눌린 키 상태를 비운다.
""",
19: """
01|바닥·탱크·상부·포신·깃대의 계층 조립|시작|initScene,reset,box|관절/프레임과 표시 육면체를 분리한다.
02|바닥 범위 안에서 탱크 XZ 이동|방향키|onKey,onUpdate,apply|부품 회전 범위를 포함하는 여유 반경으로 루트 위치를 제한한다.
03|아래 몸체 높이 진동|D|onKey,onUpdate,apply|바닥에 놓이는 하단 위치와 높이를 함께 계산한다.
04|중앙 몸체 Y축 회전과 자식 전파|T|onKey,onUpdate,apply|중앙 그룹 아래의 상부 부품이 같이 돈다.
05|두 상부 몸체의 반원 경로 교환|L|onKey,onUpdate,apply|sin/cos와 반대 부호로 서로 다른 반원을 지나 교환한다.
06|양쪽 포신의 반대 방향 Y회전|G|onKey,onUpdate,apply|회전 중심과 포신 오프셋을 함께 회전시킨다.
07|양쪽 깃대의 반대 방향 X진동|P|onKey,onUpdate,apply|같은 위상에 양쪽 부호만 반대로 둔다.
08|카메라 위치와 target의 함께 이동|x/X/z/Z|onKey,onUpdate|시선 방향을 유지하며 평행 이동한다.
09|카메라 로컬 Y축 자전|y/Y|onKey,onUpdate|position 고정, 시선과 up을 함께 회전한다.
10|화면 중심 Y축 기준 카메라 공전|r/R|onKey,onUpdate|월드 원점 축에서 카메라의 position/target/up을 변환한다.
11|전체 부품 색 순환|O|recolor,onKey|위치·계층을 바꾸지 않고 색만 갱신한다.
12|탱크·카메라·애니메이션 전체 리셋|C|reset,onKey|부품별 시간·방향·입력도 함께 초기화한다.
13|포커스 이탈 때 카메라/탱크 입력 해제|포커스|onFocus|애니메이션 토글과 지속 입력을 구분한다.
""",
20: """
01|바닥과 일정 높이의 ㄱ자 평면 벨트|시작|initScene|벨트 면과 줄무늬 표시 육면체를 분리한다.
02|벨트 표면의 다수 줄무늬 배치|시작|initScene,placeMovingObjects|30개 줄무늬를 전체 거리의 등간격으로 배치한다.
03|색·번호가 다른 택배 상자 다섯 개|시작|initScene|상자와 번호 선분을 owner 관계로 묶는다.
04|벨트·상자에 공통 거리 시계 사용|자동|placeMovingObjects,onUpdate|거리=속도×같은 누적 시간+개별 간격이다.
05|직선→90도 원호→직선에서 코너 방향 회전|코너|placeMovingObjects|위치와 접선 yaw를 같은 거리에서 샘플링한다.
06|끝에서 시작점으로 순환|벨트 끝|placeMovingObjects|전체 경로 길이로 거리를 순환한다. 끝점과 시작점은 별도 위치다.
07|이동 정지·재개|B|onKey,onUpdate|시계의 playing만 바꿔 객체 간 간격을 보존한다.
08|쿼터뷰 초기 시점과 카메라 줌|±|resetState,zoom,onKey|원근 거리와 직각 범위를 각각 조절한다.
09|원근·직각 투영 선택|p/P|onKey|대문자 처리와 카메라 상태를 분리한다.
10|중심 Y축 카메라 공전|좌우 방향키|onKey,onUpdate,onFocus|키 유지 중 dt로 회전하고 포커스 이탈에서 입력을 지운다.
11|벨트 시간·카메라 리셋|R|resetState,onKey|정지 상태와 초기 시점으로 돌아간다.
""",
21: """
01|안쪽이 보이는 면별 색 직육면체 방|시작|initScene|front-open 방과 Front 컬링을 사용한다.
02|벽 충돌 때 방향을 바꾸는 공|자동|addBall,onUpdate,syncScene|RoomBody 상태를 계산한 뒤 구 메시 위치에 반영한다.
03|공을 추가하고 최대 다섯 개로 제한|B|addBall,onKey|추가 개수와 이미 생성된 객체 상태를 따로 관리한다.
04|Z축 줄의 서로 다른 크기 상자 세 개|시작|initScene|공과 상자의 elastic 상태를 다르게 둔다.
05|드래그로 방 Z축 회전|마우스 좌/우|onMouse,onCursor,roomWorld|커서의 차이만 회전량에 더한다.
06|기울어진 방 바닥을 따르는 상자 이동|방 회전|onUpdate,syncScene|방 로컬 중력을 계산하고 월드 표시 위치로 옮긴다.
07|방 자체 Y축 자전|V|onKey,roomWorld,onUpdate|드래그 회전과 독립된 시간 상태를 둔다.
08|바닥을 열고 객체를 월드 낙하로 전환|F|onKey,onUpdate,releaseBody|이탈 시 월드 위치·속도·상자 회전을 보관한다.
09|모든 객체를 즉시 이탈시켜 낙하|E|onKey,releaseBody|바닥 개방 진행과 독립된 강제 이탈이다.
10|회전 방의 각속도를 이탈 속도에 반영|이탈|releaseBody,onUpdate|드래그·자전 각속도의 기여를 함께 전달한다.
11|카메라 Z이동과 로컬 Y자전|z/Z/y/Y|onKey|방 회전 상태와 카메라를 별도로 변경한다.
12|포커스 이탈에서 드래그·입력 멈춤|포커스|onFocus,onUpdate|직전 커서와 pending 회전량을 정리한다.
""",
22: """
01|실습 19의 탱크와 조작 재사용|방향키·D/T/L/G/P|reset,apply,onKey,onUpdate|같은 Scene의 동일한 부품 ID를 모든 뷰에서 사용한다.
02|원근 시점의 장면|프레임|onRender|화면 크기에서 perspective pass를 구성한다.
03|XZ 평면도의 직각 투영|프레임|onRender|위에서 아래로 보며 up 방향을 -Z로 둔다.
04|XY 정면도의 직각 투영|프레임|onRender|같은 모델을 +Z 시점에서 본다.
05|세 뷰포트의 같은 이동·같은 시간|이동·애니메이션|onUpdate,onRender|갱신은 한 번, renderViews는 여러 번 그리기만 한다.
06|카메라·색·리셋·포커스 상태 재사용|실습 19 조작|onKey,recolor,reset,onFocus|실습 19와 같은 요구를 함수별로 함께 추적한다.
""",
23: """
01|안쪽이 보이는 무대와 면별 색|시작|initScene|앞벽도 유지하고 컬링으로 시점에 따라 내부를 보이게 한다.
02|머리·코·몸통·다른 색 팔다리 로봇|시작|initScene|위치/크기는 GridLayout 비율로 조립한다.
03|겹치지 않는 랜덤 장애물 세 개|시작·I|initScene|난수 엔진을 유지하고 제한 횟수 안에서 배치한다.
04|방향을 지정하고 계속 걷기|W/A/S/D|onKey,onUpdate|키가 진행 방향을 바꾸며 보행 상태를 켠다.
05|무대 경계에서 방향 반전|경계|onUpdate|CharacterBody를 stepCharacter로 진행한다.
06|장애물 충돌에서 이동 거부·방향 반전|장애물|onUpdate|충돌 결과를 적용한 최종 위치로 리그를 갱신한다.
07|속도와 팔다리 진폭의 연동|±|onKey,onUpdate|요청 속도와 실제 이동 거리로 보행 진폭·위상을 정한다.
08|보행 정지·재개|Space|onKey,onUpdate|시간과 수직 운동을 모두 멈추는 대신 수평 속도만 0으로 계산한다.
09|제자리·이동 점프 및 장애물 위 착지|J|onKey,onUpdate|grounded에서 점프하고 지지면 높이를 계산한다.
10|카메라 방향 벽의 균등 축소·개방|O|onKey,onUpdate|카메라로 한 벽을 선택하고 중심 기준 균등 축소한다.
11|카메라 이동 후에도 같은 벽 복원|O 반복|onKey,onUpdate|activeWall은 복원 완료 때까지 유지한다.
12|카메라 XYZ 이동·target 중심 Y공전|x/X/z/Z/y/Y|onKey|로봇의 이동 상태와 독립적으로 조절한다.
13|무대·로봇·장애물·입력 리셋|I|initScene,onKey|ID를 다시 받고 기존 난수 엔진에서 새 장애물을 생성한다.
"""}
PDF = {
13:("지엘3 좌표계 변환 (실습 131415).pdf",2),14:("지엘3 좌표계 변환 (실습 131415).pdf",3),15:("지엘3 좌표계 변환 (실습 131415).pdf",4),
16:("지엘3 좌표계 변환 (실습 1617).pdf",2),17:("지엘3 좌표계 변환 (실습 1617).pdf",3),18:("지엘3 좌표계 변환 (실습 1819).pdf",2),19:("지엘3 좌표계 변환 (실습 1819).pdf",3),
20:("지엘3 좌표계 변환 2 (실습 20).pdf",1),21:("지엘3 좌표계 변환 (실습 2122).pdf",2),22:("지엘3 좌표계 변환 (실습 2122).pdf",3),23:("지엘3 좌표계 변환 (실습 23).pdf",2)}
TITLES={13:"면 선택과 표시",14:"색·자전·이동",15:"육면체 면의 개별·복합 동작",16:"사각뿔 펼침과 교환",17:"두 객체의 선택·변환·자리 교환",18:"행성·달·궤도 계층",19:"탱크 부품과 카메라",20:"컨베이어·줄무늬·상자",21:"방·공·상자·낙하",22:"탱크의 세 뷰포트",23:"로봇·장애물·점프·벽"}
SHARED={"assignVertexColors":15,"rotateAround":15,"scaleAround":15,"turn":17,"require":20,"requireEntity":22}

COMMON_EXTRA='\n## 공통 요구사항 묶음\n\n| ID | 공통 패턴 | 가져올 위치 |\n|---|---|---|\n| C01 | Q/Esc 종료와 정상 자원 정리 | 각 문서의 onKey와 main |\n| C02 | 깊이·와이어·투영 옵션의 독립 설정 | 14/18/20의 onKey, 22의 onRender |\n| C03 | 토글/반전/키 유지/키 해제의 구분 | 15의 beginMotion, 17/19의 onKey |\n| C04 | Shift·Caps Lock 조합 | 14/17~23의 onKey |\n| C05 | 포커스 이탈의 입력 초기화 | 14/17~22의 onFocus |\n| C06 | 기준 상태와 현재 시간의 분리 | 15의 applyFaceMotion, 17의 begin/onUpdate |\n| C07 | SRT·계층·표시 크기의 분리 | 17~19의 apply, 22의 reset/apply |\n| C08 | 여러 뷰에서 한 번만 갱신 | 22의 onUpdate/onRender |\n| C09 | 리셋 후 ID 재발급과 초기 상태 | 각 문서의 initScene/reset |\n| C10 | 결과 확인과 실패 전파 | require/requireEntity 및 각 initScene |\n\n위 공통 패턴은 실습별 세부 116개와 구분한다. 같은 공통 코드가 필요하다고 여러 실습의 전체 Console을 중복 정의하지 않는다.\n'
INDEX_EXTRA='\n## 외부 의존성이 없는 구성\n\n코드 블록이 참조하는 사용자 파일은 같은 standard 폴더에 작성할 Console.h/PracticeCommon.h뿐이다. 내용은 모두 이 묶음에 있다.\nframework는 standard에 동봉된 폴더다. 다른 실습 파일, 사용자 홈 경로, 외부 모델·텍스처 파일은 코드에서 읽지 않는다.\n근거용 원본 링크와 문서 생성 도구는 실행 코드 의존성과 구분한다.\n\n실습별 세부 116개, 공통 묶음 10개, 이전 3D 부록 10개를 구분해 제공한다. 실제 빌드·화면 확인은 사용자 담당이다.\n'

def write(path,text):
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_text(text,encoding="utf-8-sig",newline="\n")

def functions(source):
    raw=source.encode("utf-8");tree=PARSER.parse(raw);out=[]
    def visit(node):
        if node.type=="function_definition":
            body=node.child_by_field_name("body")
            signature=raw[node.start_byte:body.start_byte].decode("utf-8")
            match=re.search(r"([\w:]+)\s*\(",signature)
            if match:out.append(dict(name=match[1],start=node.start_byte,end=node.end_byte,line=node.start_point.row+1,text=raw[node.start_byte:node.end_byte].decode("utf-8")))
            return
        for child in node.named_children:visit(child)
    visit(tree.root_node)
    return out

def block(code):return "```cpp\n"+code.rstrip()+"\n```\n"
def anchor(name):return "fn-"+name.lower().replace("::","-")
def rows(number):
    return [dict(id=f"P{number}-{parts[0]}",title=parts[1],input=parts[2],methods=parts[3].split(","),behavior=parts[4])
            for line in DATA[number].strip().splitlines() if (parts:=line.split("|"))]

def main():
    OUT.mkdir(parents=True,exist_ok=True)
    sources={};inventory=[];hashes={}
    for number in range(13,24):
        folder=ROOT/f"openGL_3dAssign_{number}"
        sources[number]={name:(folder/name).read_text(encoding="utf-8-sig") for name in ["Console.h","Console.cpp","main.cpp"]}
        for name in sources[number]:hashes[f"openGL_3dAssign_{number}/{name}"]=hashlib.sha256((folder/name).read_bytes()).hexdigest()
    helpers={}
    for name,number in SHARED.items():
        helpers[name]=next(f["text"] for f in functions(sources[number]["Console.cpp"]) if f["name"]==name)
    common_header='#pragma once\n#include "framework/Framework.h"\n#include <algorithm>\n#include <array>\n#include <cmath>\n#include <iostream>\n#include <random>\n#include <stdexcept>\n#include <string>\n#include <vector>\n\nnamespace practice_common {\n'
    common_header+="\n\n".join("inline "+code for code in helpers.values())+"\n} // namespace practice_common\n"
    common="# 공통 구현: standard 안에서만 사용하는 보조 코드\n\n이 문서의 첫 블록 전문을 `standard/PracticeCommon.h`로 저장한다. C++17과 standard에 동봉된 framework만 사용한다. 다른 실습 폴더, 개인 경로, 외부 SDK, 추가 라이브러리를 참조하지 않는다.\n\n## C00 공통 헤더 전문\n\n"+block(common_header)
    common+="\n## 공통 패턴을 가져오는 위치\n\n| 공통 처리 | 실제 코드 | 사용하는 곳 |\n|---|---|---|\n| 꼭짓점 색 일관성 | assignVertexColors | 14·15의 초기 모델 구성 |\n| 점/선 중심 회전 | rotateAround | 15·16의 면·꼭짓점 회전 |\n| 중심 고정 축소 | scaleAround | 15·16의 뒷면 축소 |\n| 도 단위 회전 기저 | turn | 17·18·19 |\n| 실패 전파 | require / requireEntity | 20·22·23 |\n\n"
    common+="C++ 블록은 하나만 골라도 필요한 클래스 상태와 다른 함수가 있을 수 있다. 각 실습 문서의 `Console.h`, `Console.cpp` 보조부와 필요한 함수 블록을 함께 사용한다. 한 실습을 통째로 옮기려면 해당 문서의 블록을 순서대로 복사한다. 파일 내용은 모두 이 문서 묶음에 제공하며 원본 실습 파일을 include하지 않는다.\n\n"
    common+="## 공통 실행 계약\n\n- 초기화는 ID와 기준 상태를 만들고, 입력은 상태를 바꾸며, onUpdate는 dt초로 상태를 진행한 뒤 Scene에 반영한다.\n- 토글은 GLFW_PRESS에서 처리한다. 지속 입력은 PRESS/RELEASE 상태와 포커스 해제를 처리한다.\n- Shift와 Caps Lock은 XOR로 대문자를 판단한다.\n- Scene.clear 뒤에는 모든 ID와 포인터를 다시 받는다.\n- 본체 회전과 면 행렬을 분리하고 매 프레임 기준 상태에서 계산한다.\n- 여러 뷰라도 상태·시간 갱신은 한 번이다. Application이 모션을 자동 진행하므로 추가 updateMotions를 호출하지 않는다.\n- 기존 standard 데모 Console과 새 Console을 동시에 정의하지 않는다. 교체할 파일은 아래 3개뿐이다. PracticeCommon.h는 위 전문을 추가한다.\n\n| 파일 | 넣을 코드 |\n|---|---|\n| standard/Console.h | 선택한 실습의 전체 클래스 상태·선언 블록 |\n| standard/Console.cpp | 보조부 뒤에 모든 함수 블록을 순서대로 연결 |\n| standard/main.cpp | 해당 실습의 콜백 연결 블록 |\n\n기존 내용을 보존한 뒤 사용자가 교체한다. 이 작업은 실제 standard 런타임 파일을 교체하지 않았다. 빌드·실행·입력·화면 확인은 사용자 담당이다.\n"
    write(OUT/"COMMON.md",common+COMMON_EXTRA)
    index="# 3D 실습 세부 요구사항 구현 패턴\n\n공통 framework를 쓰는 실습 13~23의 **세부 동작**을 대상으로 한다. API 목록 대신 상태·시작/재입력·프레임 계산·반영 함수를 연결한다. 번호 없는 이전 3D 예제와 워밍업의 3D 데이터 부분은 [별도 부록](EARLIER_3D.md)에 둔다. 1~12의 2D 구현은 제외한다.\n\n[공통 구현과 standard 복사 순서](COMMON.md) · [이웃 면 교환을 따로 조립하는 완결 예](ADJACENT_FACES.md) · [요구사항과 현재 소스의 차이](SOURCE_NOTES.md)\n\n새 API를 추가하지 않는다. 모든 C++ 블록은 standard 내부 파일 또는 동봉 framework만 참조한다. 원본 경로 링크는 근거이며 코드 의존성이 아니다.\n\n## 실습별 문서\n\n"
    for number in range(13,24):
        source=sources[number];methods=[f for f in functions(source["Console.cpp"]) if f["name"].startswith("Console::")]
        by_name={f["name"].split("::")[-1]:f for f in methods}
        entries=rows(number);inventory.extend(dict(practice=number,**row) for row in entries)
        filename=f"P{number}.md";index+=f"- [{number}: {TITLES[number]}]({filename}) — {len(entries)}개 세부 패턴\n"
        pdf,page=PDF[number]
        md=f"# 실습 {number}: {TITLES[number]}\n\n[전체 목차](README.md) · [공통 보조 코드](COMMON.md)\n\n"
        md+=f"근거: 과제 PDF `{pdf}` {page}쪽 이후와 현재 실습 소스. PDF를 그대로 구현했다고 단정하지 않으며 [현재 소스와 차이](SOURCE_NOTES.md)를 함께 읽는다.\n\n"
        md+="## 세부 요구사항 → 구현 블록\n\n| ID | 구현 패턴 | 입력 | 상태/시작/프레임/반영 블록 | 동작과 경계 |\n|---|---|---|---|---|\n"
        for row in entries:
            links=" · ".join(f"[{name}](#{anchor(name)})" for name in row["methods"])
            md+=f"| {row['id']} | {row['title']} | {row['input']} | {links} | {row['behavior']} |\n"
        header=re.sub(r'#include "framework/[^"\n]+"','#include "PracticeCommon.h"',source["Console.h"])
        header=re.sub(r'^#include <(?:glfw3\.h|glm/[^>]+)>\s*\n','',header,flags=re.M)
        md+="\n## 상태와 선언 — standard/Console.h\n\n이 블록 전문을 Console.h로 사용한다. private 멤버와 보조 함수 선언도 포함한다.\n\n"+block(header)
        raw=source["Console.cpp"].encode("utf-8");removed=[];used=[]
        for f in functions(source["Console.cpp"]):
            if "::" not in f["name"] and f["name"] in SHARED:
                removed.append((f["start"],f["end"]));used.append(f["name"])
        # Function bodies are verbatim; only duplicate local helper definitions move to COMMON.md.
        transformed=raw
        for start,end in sorted(removed,reverse=True):transformed=transformed[:start]+transformed[end:]
        cpp=transformed.decode("utf-8")
        cpp=re.sub(r'^#include <(?:glfw3\.h|glm/[^>]+)>\s*\n','',cpp,flags=re.M)
        if used:cpp=cpp.replace('#include "Console.h"','#include "Console.h"\n'+"\n".join("using practice_common::"+name+";" for name in used),1)
        final_methods=[f for f in functions(cpp) if f["name"].startswith("Console::")]
        preamble=cpp.encode("utf-8")[:final_methods[0]["start"]].decode("utf-8")
        md+="\n## Console.cpp 보조부\n\n이 블록을 Console.cpp 맨 앞에 두고 아래 함수 블록을 순서대로 붙인다. 공유 보조 함수는 위 Common 헤더에서 제공한다.\n\n"+block(preamble)
        for f in final_methods:
            name=f["name"].split("::")[-1]
            using=[row["id"] for row in entries if name in row["methods"]]
            original=by_name[name]
            md+=f'\n<a id="{anchor(name)}"></a>\n\n## {name} 구현 블록\n\n'
            md+="연결 요구사항: "+(", ".join(using) or "다른 함수의 보조 처리")+".\n\n"
            md+=f"원본 위치: [Console.cpp {original['line']}행](../../../openGL_3dAssign_{number}/Console.cpp). 본문은 현재 소스에서 가져왔으며 함수 전체를 제공한다.\n\n"+block(f["text"])
        md+="\n## standard/main.cpp 콜백 연결\n\n선택한 Console에 실제 존재하는 콜백만 등록한다. 수정키·마우스·커서·포커스·사용자 렌더 콜백을 누락하지 않는다. 이 블록 전문으로 main.cpp를 교체한다.\n\n"+block(source["main.cpp"])
        md+="\n## 사용자가 확인할 사항\n\n이 문서의 블록을 순서대로 같은 standard 폴더에 구성한 뒤 C++17/x64로 빌드한다. 위 요구사항의 키·재입력·초기화·포커스·화면 결과를 확인한다. 이 문서 제작에서는 빌드·테스트·앱 실행을 하지 않았다.\n"
        write(OUT/filename,md)
    index+="\n## 읽는 방법\n\n1. 공통 헤더 전문을 같은 standard 폴더에 준비한다.\n2. 원하는 실습의 상태·보조부·함수·main 블록을 순서대로 복사한다.\n3. 요구사항 표에서 원하는 세부 동작을 찾고 상태를 켜는 함수와 프레임 계산을 함께 읽는다.\n4. 여러 실습을 합칠 때 클래스 Console을 중복 정의하지 말고 필요한 상태·시작 함수·갱신 함수만 한 Console로 옮긴다.\n5. [차이와 사용자 확인](SOURCE_NOTES.md)을 읽고 직접 빌드·조작한다.\n\n"
    index+=f"세부 패턴 {len(inventory)}개, 실습 문서 11개다. 예: 이웃한 두 면의 선택·공유 모서리 회전·반대 공전 교환은 P15-06~08로 나눴다. 공통 기능은 COMMON.md에 한 번만 둔다.\n"
    write(OUT/"README.md",index+INDEX_EXTRA)
    for name in ["openGL_sample0/main.cpp","Computer-Graphics_WarmmingUp2/README.md","Computer-Graphics_WarmmingUp2/MeshValidator.cpp"]:
        hashes[name]=hashlib.sha256((ROOT/name).read_bytes()).hexdigest()
    for path in (ROOT/"lecture_pdf/homework").glob("*.pdf"):
        if any(term in path.name for term in ["131415","1617","1819","실습 20","2122","실습 23"]):
            hashes[path.relative_to(ROOT).as_posix()]=hashlib.sha256(path.read_bytes()).hexdigest()
    commit=subprocess.check_output(["git","rev-parse","HEAD"],cwd=ROOT,text=True).strip()
    write(OUT/"SOURCE_INVENTORY.json",json.dumps({"source_commit":commit,"source_files":hashes,"requirements":inventory,"common_patterns":["C"+str(i).zfill(2) for i in range(1,11)],"earlier_3d_patterns":["E00-01","E00-02","E00-03","E00-04","E06-01","E06-02","E06-03","E06-04","E06-05","E06-06"],"generated_by":"tools/Write-PracticeCookbook.py","build_test_app_execution":"not_run","dependencies":"C++17 standard library + framework bundled within standard + complete local helper block"},ensure_ascii=False,indent=2)+"\n")
    print(f"Cookbook: {len(inventory)} detailed requirements, 11 complete standard-local recipes, shared helpers. No build/tests executed.")

if __name__=="__main__":main()
```

