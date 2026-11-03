#pragma once
#include "framework/Framework.h"

class Console {
    graphics::Application& app_;
    std::optional<int> initialMode_;
    int activeMode_ = 1;
    bool activateMode(int mode);
    bool initMotion(int kind);
    bool updateGrid();
    bool updateRoom(float dt);
    bool updateAnimation();
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

    enum class Demo { Motion, Grid, Room, Animation };

    Demo demo_ = Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_ = 0, roomCube_ = 0;
    std::vector<graphics::JointAnimation> animation_;

    struct JointControl {
        std::size_t track = 0;
        std::string name;
        graphics::Transform3D initialRest;
    };

    std::vector<JointControl> jointControls_;
    std::size_t selectedJoint_ = 0;
    int selectedAxis_ = 0;


    /// 선택한 관절의 기준 SRT와 피벗을 키 입력으로 편집한다.
    bool handleJointKey(int key);


    /// 현재 선택 관절과 조작 축을 콘솔에 출력한다.
    void printSelectedJoint() const;


    bool threeViews_ = false, trace_ = false, floorOpen_ = false;


    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();


    /// 회전하는 방 예제를 구성한다.
    bool initRoom();


    /// 선형보간·그네·사람 관절의 계층 애니메이션 예제를 구성한다.
    bool initAnimation();


    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);


    /// 반사 변환을 순서대로 변경한다.
    bool changeReflection();


    /// 크기 레이어를 삽입하거나 제거한다.
    bool toggleInsertedLayer();


    /// 조작 방법을 출력한다.
    void printHelp() const;


public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app, std::optional<int> initialMode = std::nullopt)
        : app_(app), initialMode_(initialMode) {
    }


    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();


    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action);


    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);


    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);


    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
};
