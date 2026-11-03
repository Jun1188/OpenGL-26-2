#pragma once
#include "PracticeMode.h"
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace assembly {
// CPU-only state. The host remains the only Application and OpenGL context.
class FeatureContainer {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    FeatureContainer(graphics::Application& host, int mode);
    ~FeatureContainer();
    FeatureContainer(const FeatureContainer&) = delete;
    FeatureContainer& operator=(const FeatureContainer&) = delete;
    int mode() const;
    const graphics::Scene& scene() const;
    const graphics::Camera& camera() const;
    const graphics::RenderOptions& options() const;
    void step(float dt);
    void key(int key, int action, int mods);
    void mouse(const graphics::MouseEvent& event, const graphics::Camera& view);
    void cursor(double x, double y);
    void focus(bool value);
    void pick(std::optional<graphics::PickResult> hit);
};

struct FeatureEvent {
    enum class Kind { Key, Mouse, Cursor, Focus, Pick } kind = Kind::Focus;
    int key = 0, action = 0, mods = 0;
    double x = 0, y = 0;
    bool focused = false;
    graphics::MouseEvent mouse;
    graphics::Camera view;
    std::optional<graphics::PickResult> hit;
};
struct FeatureEntity {
    graphics::EntityId group = 0;
    std::optional<graphics::EntityId> mesh, nativeParent;
    bool parentKnown = false, visible = true;
    graphics::GlColor tint;
};
struct FeatureLine {
    graphics::LineId line = 0;
    graphics::EntityId frame = 0;
    glm::vec3 start{0}, end{0};
    bool visible = true;
};
struct FeatureInstance {
    std::string name;
    std::shared_ptr<FeatureContainer> program;
    std::optional<graphics::EntityId> branch;
    std::map<graphics::EntityId, FeatureEntity> entities;
    std::map<std::size_t, FeatureLine> lines;
    std::uint64_t revision = 0;
    bool enabled = true, playing = true;
    std::vector<FeatureEvent> events;
};
} // namespace assembly
