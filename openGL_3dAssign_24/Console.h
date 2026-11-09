#pragma once
#include "framework/Framework.h"
#include <array>
#include <vector>

class Console {
    graphics::Application& app_;
    graphics::AnimationClock clock_;
    std::array<graphics::EntityId, 5> boxes_{};
    std::array<bool, 5> loaded_{};
    std::vector<graphics::EntityId> stripes_;
    graphics::EntityId truck_ = 0;
    graphics::Camera thirdPerson_;
    std::array<graphics::Camera, 5> parcelViews_{};
    float truckZ_ = 1.0f;
    int truckDirection_ = -1;
    std::size_t viewBox_ = 4;
    bool truckMoving_ = false, firstPerson_ = false;
    bool leftHeld_ = false, rightHeld_ = false;

    void placeMovingObjects();
    void resetState();
    void zoom(float factor);
    void addTruck();
    void loadParcel(std::size_t index);
    void updateCamera(bool report = false);

public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action, int mods);
    void onFocus(bool focused);
    void onUpdate(float dt);
};
