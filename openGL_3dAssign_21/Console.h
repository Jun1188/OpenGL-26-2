#pragma once
#include "framework/Framework.h"

class Console {
    struct BodyObject {
        graphics::EntityId entity = 0;
        graphics::RoomBody body;
        bool elastic = false;
        glm::mat3 releasedRotation{1};
    };
    graphics::Application& app_;
    const graphics::Bounds3 roomBounds_{{-3, -2, -3.5f}, {3, 2.5f, 3.5f}};
    graphics::EntityId room_ = 0;
    std::vector<BodyObject> objects_;
    unsigned ballCount_ = 0;
    float roomAngle_ = 0;
    float roomSpinAngle_ = 0;
    float pendingRotation_ = 0;
    bool roomSpinning_ = false;
    bool floorOpen_ = false;
    bool dragging_ = false;
    bool focused_ = true;
    double previousMouseX_ = 0;

    glm::mat4 roomWorld() const;
    bool addBall();
    void releaseBody(BodyObject& object) const;
    bool syncScene();
    void require(bool succeeded, const char* operation) const;
    void printHelp() const;

public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action, int mods);
    void onMouse(const graphics::MouseEvent& event);
    void onCursor(double x, double y);
    void onFocus(bool focused);
    void onUpdate(float dt);
};
