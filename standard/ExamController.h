#pragma once
#include "framework/Framework.h"
#include <array>

class ExamController {
    graphics::Application& app_;
    graphics::SceneAuthoring& scene_;
    // @debug hook state
    struct State {
        graphics::PointLight light;
        graphics::Camera camera;
        glm::vec3 orbitCenter{0};
        double lightDegrees = 0, spinDegrees = 0;
        float radius = 1, height = 4, moveSpeed = 3;
        int lightDirection = 0, cameraDirection = 0, spinDirection = 0;
        int color = 0, shaderView = 0;
        bool lighting = true;
    } state_, initial_;
    std::string lightParent_;
    graphics::EntityId marker_ = 0;
    std::vector<graphics::LineId> ring_;
    glm::mat4 ringFrame_{1};
    glm::vec3 ringCenter_{0};
    float ringRadius_ = -1, ringHeight_ = 0;
    std::array<bool, 4> move_{};
    glm::vec3 orbitPoint(double degrees) const;
public:
    ExamController(graphics::Application& app, graphics::SceneAuthoring& scene) : app_(app), scene_(scene) {}
    void initialize();
    void reset();
    void onKey(int key, int action, int mods);
    void onFocus(bool focused);
    void update(float dt);
    void syncVisuals();
    bool bindUniforms(const graphics::DrawUniformContext& context) const;
    void help() const;
};
