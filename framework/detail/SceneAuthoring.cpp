#include "../include/SceneAuthoring.h"
#include <stdexcept>

namespace graphics {
namespace {
void requireAuthoring(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
bool finite(const glm::vec3& value) {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}
bool nonsingular(const glm::mat4& matrix) {
    const auto determinant = glm::determinant(glm::mat3(matrix));
    return TransformSystem::validAffine(matrix) && std::isfinite(determinant) && std::abs(determinant) > 1e-18f;
}
glm::mat4 rotation(const glm::vec3& degrees) {
    requireAuthoring(finite(degrees), "SceneAuthoring: rotation must be finite.");
    glm::mat4 result(1);
    result = glm::rotate(result, glm::radians(degrees.z), glm::vec3(0, 0, 1));
    result = glm::rotate(result, glm::radians(degrees.y), glm::vec3(0, 1, 0));
    return glm::rotate(result, glm::radians(degrees.x), glm::vec3(1, 0, 0));
}
glm::mat4 layerMatrix(const LayerSetup& setup, double seconds) {
    requireAuthoring(finite(setup.translation) && finite(setup.scale) && finite(setup.pivot) &&
        finite(setup.spinAxis) && std::isfinite(setup.spinDegreesPerSecond), "SceneAuthoring: invalid layer values.");
    glm::mat4 spin(1);
    if (setup.spinDegreesPerSecond != 0) {
        requireAuthoring(glm::length(setup.spinAxis) > 1e-6f, "SceneAuthoring: spin axis is zero.");
        const double angle = setup.spinDegreesPerSecond * seconds;
        requireAuthoring(std::isfinite(angle), "SceneAuthoring: spin time overflow.");
        spin = glm::rotate(glm::mat4(1), float(glm::radians(std::remainder(angle, 360.0))), glm::normalize(setup.spinAxis));
    }
    const auto result = glm::translate(glm::mat4(1), setup.translation + setup.pivot) *
        rotation(setup.rotationDegrees) * spin * glm::scale(glm::mat4(1), setup.scale) *
        glm::translate(glm::mat4(1), -setup.pivot);
    requireAuthoring(nonsingular(result),
        "SceneAuthoring: layer scale must be invertible.");
    return result;
}
bool validColor(GlColor color) {
    return std::isfinite(color.red) && std::isfinite(color.green) && std::isfinite(color.blue) &&
        color.red >= 0 && color.green >= 0 && color.blue >= 0;
}
}

void SceneAuthoring::clear() {
    app_.getScene().clear(); objects_.clear(); layers_.clear(); lights_.clear();
    camera_.reset(); cameraParent_.clear(); app_.getRenderOptions().pointLights.clear();
}
EntityId SceneAuthoring::object(const std::string& name) const {
    const auto found = objects_.find(name);
    if (found == objects_.end()) throw std::runtime_error("SceneAuthoring: missing object: " + name);
    return found->second;
}
std::string SceneAuthoring::name(EntityId id) const {
    for (const auto& item : objects_) if (item.second == id) return item.first;
    return {};
}
EntityId SceneAuthoring::addObject(const std::string& name, const ObjectSetup& setup, const std::string& parent) {
    requireAuthoring(!name.empty() && !objects_.count(name), "SceneAuthoring: duplicate/empty object name.");
    requireAuthoring(setup.shape == GLShape::Cube || setup.shape == GLShape::Tetra || setup.shape == GLShape::Pyramid,
        "SceneAuthoring: use native Scene APIs for other model types.");
    requireAuthoring(finite(setup.size) && glm::all(glm::greaterThan(setup.size, glm::vec3(0))) &&
        finite(setup.position) && finite(setup.pivot) && finite(setup.scale), "SceneAuthoring: invalid object values.");
    const auto parentId = parent.empty() ? std::optional<EntityId>{} : std::optional<EntityId>{object(parent)};
    Transform3D pose;
    pose.basis = setup.position; pose.rotation = glm::mat3(rotation(setup.rotationDegrees));
    pose.scale = setup.scale; pose.localBasis = setup.pivot;
    const auto world = DiagramSystem::modelMatrix(pose);
    requireAuthoring(nonsingular(world),
        "SceneAuthoring: object scale must be invertible.");
    auto model = DiagramSystem::makePolyhedron(setup.shape, setup.size);
    for (auto& face : model.faces) face.material = setup.material;
    auto saved = app_.getScene();
    try {
        const auto id = app_.getScene().push_entity(model);
        requireAuthoring(id.has_value(), "SceneAuthoring: object creation failed.");
        requireAuthoring(app_.getScene().setParent(*id, parentId) && app_.getScene().setWorldPlacement(*id, world),
            "SceneAuthoring: parent/world placement failed.");
        objects_.emplace(name, *id);
        return *id;
    } catch (...) { app_.getScene() = std::move(saved); throw; }
}
void SceneAuthoring::addLayer(const std::string& objectName, const std::string& name, const LayerSetup& setup) {
    object(objectName);
    requireAuthoring(!name.empty(), "SceneAuthoring: empty layer name.");
    std::size_t count = 0;
    for (const auto& layer : layers_) if (layer.object == objectName) {
        requireAuthoring(layer.name != name, "SceneAuthoring: duplicate object-local layer name."); ++count;
    }
    requireAuthoring(count < 128, "SceneAuthoring: at most 128 layers per object.");
    layerMatrix(setup, 0);
    layers_.push_back({objectName, name, setup});
}
void SceneAuthoring::addLight(const std::string& name, const PointLight& light, const std::string& parent) {
    requireAuthoring(!name.empty() && lights_.size() < 8, "SceneAuthoring: empty name or more than eight point lights.");
    for (const auto& item : lights_) requireAuthoring(item.name != name, "SceneAuthoring: duplicate light name.");
    if (!parent.empty()) object(parent);
    requireAuthoring(finite(light.position) && validColor(light.color) && std::isfinite(light.intensity) &&
        light.intensity >= 0 && std::isfinite(light.attenuation) && light.attenuation >= 0, "SceneAuthoring: invalid point light.");
    lights_.push_back({name, parent, light});
}
void SceneAuthoring::setCamera(const Camera& camera, const std::string& parent) {
    if (!parent.empty()) object(parent);
    requireAuthoring(CameraSystem::validCamera(camera), "SceneAuthoring: invalid camera.");
    camera_ = camera; cameraParent_ = parent;
}
void SceneAuthoring::update(double seconds) {
    requireAuthoring(std::isfinite(seconds) && seconds >= 0, "SceneAuthoring: time must be finite and nonnegative.");
    std::map<EntityId, std::vector<glm::mat4>> matrices;
    for (const auto& layer : layers_) matrices[object(layer.object)].push_back(layerMatrix(layer.setup, seconds));
    for (const auto& item : matrices)
        requireAuthoring(app_.getScene().setLocalLayers(item.first, item.second), "SceneAuthoring: invalid composed local layers.");
    std::vector<PointLight> lights;
    for (const auto& item : lights_) {
        auto light = item.value;
        if (!item.parent.empty()) {
            const auto frame = app_.getScene().worldMatrix(object(item.parent));
            requireAuthoring(frame.has_value(), "SceneAuthoring: invalid light frame.");
            light.position = glm::vec3(*frame * glm::vec4(light.position, 1));
        }
        requireAuthoring(finite(light.position), "SceneAuthoring: light position overflow.");
        lights.push_back(light);
    }
    app_.getRenderOptions().pointLights = std::move(lights);
    if (camera_) {
        if (cameraParent_.empty()) app_.getCamera() = *camera_;
        else {
            const auto frame = app_.getScene().worldMatrix(object(cameraParent_));
            requireAuthoring(frame.has_value(), "SceneAuthoring: invalid camera frame.");
            const auto camera = CameraSystem::attached(*camera_, *frame);
            requireAuthoring(camera.has_value(), "SceneAuthoring: degenerate camera frame.");
            app_.getCamera() = *camera;
        }
    }
}
} // namespace graphics
