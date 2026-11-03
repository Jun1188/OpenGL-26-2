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
