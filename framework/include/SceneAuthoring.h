#pragma once
#include "Application.h"

namespace graphics {
// Direct C++ authoring data. No command parser or assembly file is required.
struct ObjectSetup {
    GLShape shape = GLShape::Cube;
    glm::vec3 size{1}, position{0}, rotationDegrees{0}, scale{1}, pivot{0};
    Material material;
};

struct LayerSetup {
    glm::vec3 translation{0}, rotationDegrees{0}, scale{1}, pivot{0};
    glm::vec3 spinAxis{0, 1, 0};
    double spinDegreesPerSecond = 0;
};

class SceneAuthoring {
    struct Layer { std::string object, name; LayerSetup setup; };
    struct Light { std::string name, parent; PointLight value; };
    Application& app_;
    std::map<std::string, EntityId> objects_;
    std::vector<Layer> layers_;
    std::vector<Light> lights_;
    std::optional<Camera> camera_;
    std::string cameraParent_;
public:
    explicit SceneAuthoring(Application& app) : app_(app) {}
    /// [API] Clear Scene and authored handles together; call before rebuilding the scene.
    void clear();
    /// [API] Create a cube/tetrahedron/pyramid at a world-authored pose; parent must already exist.
    EntityId addObject(const std::string& name, const ObjectSetup& setup, const std::string& parent = "");
    /// [API] Register prepared geometry, retaining materials and a world-authored pose.
    EntityId addModel(const std::string& name, const Model& model, const Transform3D& worldPose = Transform3D{},
                      const std::string& parent = "");
    /// [API] Append an object-local layer. Later layers inherit all earlier layers.
    void addLayer(const std::string& object, const std::string& name, const LayerSetup& setup);
    /// [API] Replace an existing layer; evaluate at the next update without resetting time.
    void setLayer(const std::string& object, const std::string& name, const LayerSetup& setup);
    /// [API] Register a point light, optionally expressed in a named object's local frame.
    void addLight(const std::string& name, const PointLight& light, const std::string& parent = "");
    /// [API] Read the named light's authored value (local when parented) and its frame name.
    PointLight light(const std::string& name) const;
    std::string lightParent(const std::string& name) const;
    /// [API] Replace a named light's value and parent; output is rebuilt by syncTools.
    void setLight(const std::string& name, const PointLight& light, const std::string& parent = "");
    /// [API] Select one camera, optionally expressed in a named object's local frame.
    void setCamera(const Camera& camera, const std::string& parent = "");
    /// [API] Evaluate local layers, attached lights and camera at absolute seconds; throws on invalid input.
    void update(double seconds);
    /// [API] Synchronize attached lights/camera after all Scene motions; does not advance time.
    void syncTools();
    /// [API] Resolve a source-authored object name; throws if the name is absent.
    EntityId object(const std::string& name) const;
    /// [API] Resolve a picked object ID to its source-authored name, or return an empty string.
    std::string name(EntityId id) const;
};
} // namespace graphics
