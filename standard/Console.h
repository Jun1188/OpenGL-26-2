#pragma once
#include "framework/Framework.h"

class Console {
    graphics::Application& app_;
    std::optional<graphics::MotionId> pathId_;
    std::optional<graphics::MotionId> spinId_;
    std::optional<graphics::MotionId> globalId_;
    std::optional<graphics::LayerId> mirrorLayer_;
    std::optional<graphics::LayerId> normalSpinLayer_;
    std::optional<graphics::LayerId> insertedLayer_;
    bool paused_ = false;
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;
    enum class Demo { Motion, Grid, Room };
    Demo demo_=Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_=0, roomCube_=0;
    bool threeViews_=false, trace_=false, floorOpen_=false;
    bool initGrid();
    bool initRoom();

    bool choosePath(int kind);
    bool changeReflection();
    bool toggleInsertedLayer();
    void printHelp() const;

public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action);
    void onMouse(const graphics::MouseEvent& event);
    void onUpdate(float dt);
    bool onRender();
};
