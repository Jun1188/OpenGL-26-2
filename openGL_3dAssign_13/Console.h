#pragma once

#include "framework/include/Application.h"
#include <optional>

// 과제 예제 전용 상태. framework를 사용하는 다른 프로젝트에는 필요 없다.
class Console {
private:
    graphics::Application& application_;
    std::optional<graphics::EntityId> cuboidId_;
    std::optional<graphics::EntityId> tetrahedronId_;

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    graphics::Scene& getScene() { return application_.getScene(); }
    graphics::Camera& getCamera() { return application_.getCamera(); }
    std::optional<graphics::EntityId> getCuboidId() const { return cuboidId_; }
    std::optional<graphics::EntityId> getTetrahedronId() const { return tetrahedronId_; }
};
