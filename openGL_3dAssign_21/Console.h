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

    /// 방의 월드 회전 행렬을 계산한다.
    glm::mat4 roomWorld() const;
    /// 허용 범위에 공을 추가한다.
    bool addBall();
    /// 방 안의 물체를 월드 낙하 상태로 전환한다.
    void releaseBody(BodyObject& object) const;
    /// 물리 상태를 표시 객체에 반영한다.
    bool syncScene();
    /// 실패한 작업을 예외로 전달한다.
    void require(bool succeeded, const char* operation) const;
    /// 조작 방법을 출력한다.
    void printHelp() const;

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app) : app_(app) {}
    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods);
    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);
    /// 커서 이동으로 조작 상태를 갱신한다.
    void onCursor(double x, double y);
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
};
