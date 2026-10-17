#pragma once
#include "framework/Framework.h"
#include <array>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_=0,sun_=0;
    std::array<graphics::EntityId,3> tracks_{},planets_{},moonFrames_{},moonTracks_{},moons_{};
    std::array<bool,GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 position_{0};
    glm::mat3 screenPlane_{1};
    std::array<float,3> planetAngle_{0,120,240},moonAngle_{0,120,240};
    float radius_=1,targetRadius_=1,sunTime_=0,moonTilt_=0;
    bool pulse_=false,rotateMoons_=false;
    void apply();
public:
    explicit Console(graphics::Application& app):app_(app){}
    bool initScene();
    void onKey(int key,int action,int mods);
    void onUpdate(float dt);
    void onFocus(bool focused);
};
