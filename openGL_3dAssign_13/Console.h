#pragma once

#include "framework/include/Application.h"
#include <initializer_list>
#include <random>

// 실습 13: 표시할 면과 난수 상태만 관리한다.
class Console {
private:
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    std::mt19937 random_{ std::random_device{}() };

    void selectFaces(graphics::EntityId id, std::initializer_list<std::size_t> faces);

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    void onKey(int key, int action);
};
