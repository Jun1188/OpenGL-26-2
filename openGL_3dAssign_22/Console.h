#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::EntityId root_ = 0, central_ = 0;
    std::array<graphics::EntityId, 2> lower_{}, upper_{}, guns_{}, poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_ = 0, centralAngle_ = 0, gunAngle_ = 0, poleTime_ = 0;
    float swapProgress_ = 0, swapDirection_ = 1;
    float cameraX_ = 0, cameraZ_ = 0, cameraYaw_ = 0, cameraOrbit_ = 0;
    bool lowerRunning_ = false, centralRunning_ = false, gunRunning_ = false, poleRunning_ = false;
    bool swapRunning_ = false, swapStarted_ = false;

    unsigned colorVersion_ = 0;

    graphics::EntityId box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent);

    void reset();
    void apply();
    void recolor();

public:
    explicit Console(graphics::Application& app) : app_(app) {
    }

    bool initScene();

    void onKey(int key, int action, int mods);
    void onUpdate(float dt);
    void onFocus(bool focused);

    bool onRender();
};
