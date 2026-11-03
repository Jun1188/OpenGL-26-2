# 2단계: 의미 동작·육면체 모드 변경 전후

-는 before, +는 after다. 신설 코드는 before 없음으로 전문을 아래에 제공한다. 사본에도 같은 변경을 반영했다.
설정 데이터 → Scene의 컴포넌트 생성 → 이름/ID 연결 → 키·프레임 동작 → 모션/렌더 순서다.
조회 sample와 외부 상태 변경 execute를 나눠 등록·seek의 예상치 못한 실행을 막는다.
빌드·테스트는 사용자 담당이다.

```diff
diff --git a/framework/Framework.h b/framework/Framework.h
index 5fb1573..27064a4 100644
--- a/framework/Framework.h
+++ b/framework/Framework.h
@@ -3,3 +3,5 @@
 #include "include/Application.h"
 #include "include/Patterns.h"
 #include "include/Animation.h"
+#include "include/Actions.h"
+#include "include/CuboidMode.h"
diff --git a/framework/detail/Motion.cpp b/framework/detail/Motion.cpp
index 4f96938..c7ed50a 100644
--- a/framework/detail/Motion.cpp
+++ b/framework/detail/Motion.cpp
@@ -5,7 +5,7 @@
 
 namespace graphics {
 bool Motion::validStep(const MotionStep& step) {
-    return bool(step.transform) && std::isfinite(step.startSeconds) && step.startSeconds >= 0 &&
+    return (bool(step.transform) != bool(step.action)) && std::isfinite(step.startSeconds) && step.startSeconds >= 0 &&
            (!step.durationSeconds || (std::isfinite(*step.durationSeconds) && *step.durationSeconds >= 0 &&
                                       std::isfinite(step.startSeconds + *step.durationSeconds)));
 }
@@ -25,6 +25,19 @@ bool Motion::insert(std::size_t index, const MotionStep& step) {
     return true;
 }
 
+MotionStep Motion::action(MotionAction callback, double startSeconds, std::optional<double> durationSeconds) {
+    MotionStep step;
+    step.action = std::move(callback);
+    step.startSeconds = startSeconds;
+    step.durationSeconds = durationSeconds;
+    return step;
+}
+
+bool Motion::insertAction(std::size_t index, MotionAction callback, double startSeconds,
+                          std::optional<double> durationSeconds) {
+    return insert(index, action(std::move(callback), startSeconds, durationSeconds));
+}
+
 bool Motion::replace(std::size_t index, const MotionStep& step) {
     if (index >= steps_.size() || !validStep(step))
         return Diagnostics::failAt(
@@ -63,6 +76,16 @@ bool Motion::then(MotionStep step) {
 }
 
 std::optional<MotionSample> Motion::sample(double seconds) const {
+    return evaluate(seconds, false);
+}
+
+std::optional<MotionSample> Motion::execute(double seconds) const {
+    // Snapshot permits callbacks to edit the original sequence without invalidating iteration.
+    const Motion snapshot = *this;
+    return snapshot.evaluate(seconds, true);
+}
+
+std::optional<MotionSample> Motion::evaluate(double seconds, bool actions) const {
     if (!std::isfinite(seconds) || seconds < 0)
         return (
             Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "Motion::sample",
@@ -81,6 +104,12 @@ std::optional<MotionSample> Motion::sample(double seconds) const {
         if (step.durationSeconds)
             time = std::min(time, *step.durationSeconds);
 
+        if (step.action) {
+            if (actions && !step.action(time, result))
+                return (Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "Motion::execute",
+                                            "Action returned false; prior external effects are retained."), std::nullopt);
+            continue;
+        }
         const auto matrix = step.transform(time);
 
         if (!TransformSystem::validAffine(matrix))
@@ -364,6 +393,7 @@ bool MotionSystem::update(double deltaSeconds) {
         MotionId id;
         double time;
         MotionSample sample;
+        std::optional<Motion> actions;
     };
 
     std::vector<Pending> pending;
@@ -383,7 +413,10 @@ bool MotionSystem::update(double deltaSeconds) {
             return Diagnostics::failAt(
                 __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::update", "Rejected condition: !sample",
                 "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
-        pending.push_back({entry.first, time, *sample});
+        const auto& steps = state.motion.getSteps();
+        const bool actions = state.enabled && std::any_of(steps.begin(), steps.end(),
+                                                         [](const auto& step) { return bool(step.action); });
+        pending.push_back({entry.first, time, *sample, actions ? std::optional<Motion>(state.motion) : std::nullopt});
     }
 
     for (const auto& item : pending) {
@@ -392,6 +425,9 @@ bool MotionSystem::update(double deltaSeconds) {
         state.timeSeconds = item.time;
         state.sampled = item.sample;
     }
+    for (const auto& item : pending)
+        if (item.actions && !item.actions->execute(item.time))
+            return false;
 
     return true;
 }
diff --git a/framework/include/Motion.h b/framework/include/Motion.h
index b31d178..4631243 100644
--- a/framework/include/Motion.h
+++ b/framework/include/Motion.h
@@ -13,6 +13,8 @@ struct MotionSample {
     glm::mat4 local{1.0f};
 };
 
+using MotionAction = std::function<bool(double, const MotionSample&)>;
+
 struct RotationMotion {
     AxisLine axis;
     double degreesPerSecond = 45;
@@ -54,17 +56,22 @@ struct MotionStep {
     double startSeconds = 0;
     std::optional<double> durationSeconds;
     bool enabled = true;
+    MotionAction action; // A step contains either transform or action.
 };
 
 class Motion {
     std::vector<MotionStep> steps_;
     static bool validStep(const MotionStep& step);
+    std::optional<MotionSample> evaluate(double seconds, bool actions) const;
 
 public:
     /// [API] 모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.
     bool append(const MotionStep& step);
     /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
     bool insert(std::size_t index, const MotionStep& step);
+    /// [API] 변환 사이의 인덱스에 의미 동작 또는 람다·함수 포인터를 삽입한다.
+    bool insertAction(std::size_t index, MotionAction action, double startSeconds = 0,
+                      std::optional<double> durationSeconds = std::nullopt);
     /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
     bool replace(std::size_t index, const MotionStep& step);
     /// [API] 주어진 인덱스의 모션 단계를 삭제한다.
@@ -79,6 +86,11 @@ public:
 
     /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬 기여를 합성하여 MotionSample을 반환한다.
     std::optional<MotionSample> sample(double seconds) const;
+    /// [API] 변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.
+    std::optional<MotionSample> execute(double seconds) const;
+    /// [API] 기존 append/insert/then/replace와 조립하는 비변환 동작 단계를 만든다.
+    static MotionStep action(MotionAction action, double startSeconds = 0,
+                             std::optional<double> durationSeconds = std::nullopt);
     /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
     static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);
     /// [API] 축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.
@@ -139,7 +151,7 @@ public:
     bool seekMotion(MotionId id, double seconds);
     /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
     bool removeMotion(MotionId id);
-    /// [API] 시간 또는 프레임 상태를 진행한다. MotionSystem은 임시 샘플을 모은 뒤 성공한 경우 반영한다.
+    /// [API] 모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.
     bool update(double deltaSeconds);
     /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
     void clear(); // Identifiers are never reused by this system, including after clear.

```

## include/Actions.h

before: 없음

after:

```cpp
#pragma once
#include "Application.h"

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

## include/CuboidMode.h

before: 없음

after:

```cpp
#pragma once
#include "Actions.h"

namespace graphics {
struct CuboidFaceConfig {
    Transform3D transform;
    bool visible = true;
    CullMode cull = CullMode::Inherit;
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

## detail/CuboidMode.cpp

before: 없음

after:

```cpp
#include "../Framework.h"
#include <algorithm>

namespace graphics {
namespace {
bool validCuboid(const CuboidConfig& c) {
    if (c.name.empty() || !detail::finiteVector(c.size) || c.size.x <= 0 || c.size.y <= 0 || c.size.z <= 0 ||
        !detail::validColor(c.color) || !DiagramSystem::validTransform(c.transform))
        return false;
    for (const auto& face : c.faces)
        if (!DiagramSystem::validTransform(face.transform) ||
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
        const auto id = staged.addCuboid(c.size, c.transform.basis);
        if (!id || !staged.setTransform(*id, c.transform) || !staged.setTint(*id, c.color) ||
            !staged.setVisible(*id, c.visible)) return false;
        for (std::size_t face = 0; face < c.faces.size(); ++face)
            if (!staged.setFaceTransform(*id, face, c.faces[face].transform, false) ||
                !staged.setFaceVisible(*id, face, c.faces[face].visible) ||
                !staged.setFaceCullMode(*id, face, c.faces[face].cull)) return false;
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
