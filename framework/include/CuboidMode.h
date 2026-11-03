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
