#pragma once
#include "framework/Framework.h"
#include <array>

class Console {
    struct Object {
        graphics::EntityId orbit=0,body=0;
        glm::vec3 position{0};
        glm::mat3 rotation{1};
        glm::vec2 spin{0};
        float orbitDegrees=0,orbitDirection=0,scale=.65f;
        int shape=0;
    };
    enum class Motion { None, Straight, Bypass, Combined };
    graphics::Application& app_;
    std::array<Object,2> objects_{};
    int selected_=3;
    glm::vec2 movement_{0};
    Motion motion_=Motion::None;
    bool running_=false;
    float elapsed_=0;
    std::array<glm::vec3,2> starts_{};
    std::array<float,2> scales_{};
    void reset();
    void apply();
    void begin(Motion motion);
    graphics::Model model(int shape) const;
    glm::mat3 orbitRotation(std::size_t i) const;
    glm::vec3 worldPosition(std::size_t i) const;
public:
    explicit Console(graphics::Application& app):app_(app){}
    bool initScene();
    void onKey(int key,int action,int mods);
    void onUpdate(float dt);
    void onFocus(bool focused);
};
