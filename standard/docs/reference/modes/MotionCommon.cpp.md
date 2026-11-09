# 과거 구현 참조: MotionCommon.cpp

- 원래 위치: `standard/modes/MotionCommon.cpp`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#include "../Console.h"
#include "Paths.h"
#include <iostream>

/// 공통 Motion을 모드별 경로 정의로 교체한다.
bool Console::choosePath(int kind) {
    using namespace standard_modes;
    using PathFactory = graphics::PathMotion (*)();
    const PathFactory paths[] = {path1, path2, path3, path4, path5, path6};
    if (kind < 1 || kind > 6 || !pathId_)
        return false;
    graphics::Motion motion;
    auto& motions = app_.getScene().getMotions();
    return motion.append(graphics::Motion::followPath(paths[kind - 1]())) &&
           motions.setMotion(*pathId_, motion) && motions.seekMotion(*pathId_, 0);
}

/// 장면 객체와 초기 상태를 구성한다.
bool Console::initMotion(int kind) {
    demo_ = Demo::Motion;

    auto& scene = app_.getScene();

    scene.clear();
    paused_ = false;
    spinEnabled_ = true;
    globalEnabled_ = false;
    reflection_ = 1;
    insertedLayer_.reset();

    const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
    const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});

    if (!first || !second) {
        return false;
    }

    if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) {
        return false;
    }

    auto& motions = scene.getMotions();

    pathId_ = motions.createMotion(graphics::Motion{});

    graphics::RotationMotion spin;

    spin.degreesPerSecond = 90;
    spinId_ = motions.createSpin(spin);

    graphics::RotationMotion global;

    global.degreesPerSecond = 15;
    globalId_ = motions.createOrbit(global);

    if (!pathId_ || !spinId_ || !globalId_ || !choosePath(kind)) {
        return false;
    }

    if (!motions.setMotionEnabled(*globalId_, false) ||
        !motions.bindMotion(graphics::MotionSystem::rootLayer(), *globalId_)) {
        return false;
    }

    const auto normalPath = motions.pushLayer(graphics::MotionSystem::rootLayer());
    const auto reflection = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});

    if (!normalPath || !reflection) {
        return false;
    }

    graphics::LayerConfig mirror;

    mirror.transform = *reflection;
    mirrorLayer_ = motions.pushLayer(graphics::MotionSystem::rootLayer(), mirror);

    if (!mirrorLayer_) {
        return false;
    }

    const auto mirroredPath = motions.pushLayer(*mirrorLayer_);

    normalSpinLayer_ = motions.pushLayer(*normalPath);

    if (!mirroredPath || !normalSpinLayer_) {
        return false;
    }

    const auto mirroredSpin = motions.pushLayer(*mirroredPath);

    if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId_) || !motions.bindMotion(*mirroredPath, *pathId_) ||
        !motions.bindMotion(*normalSpinLayer_, *spinId_) || !motions.bindMotion(*mirroredSpin, *spinId_) ||
        !scene.bindEntity(*first, *normalSpinLayer_) || !scene.bindEntity(*second, *mirroredSpin)) {
        return false;
    }

    if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first) ||
        !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second) ||
        !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) {
        return false;
    }

    if (!addControl(*first, "original box") || !addControl(*second, "reflected box"))
        return false;

    auto& camera = app_.getCamera();

    camera.position = {5, 3, 8};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};

    return true;
}

/// 반사 변환을 순서대로 변경한다.
bool Console::changeReflection() {
    const int next = (reflection_ + 1) % 4;
    std::optional<glm::mat4> matrix = glm::mat4(1);

    if (next == 1) {
        matrix = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
    } else if (next == 2) {
        matrix = graphics::TransformSystem::reflectPoint({0, 0, 0});
    } else if (next == 3) {
        matrix = graphics::TransformSystem::reflectLine(graphics::AxisLine{});
    }

    if (!mirrorLayer_ || !matrix) {
        return false;
    }

    graphics::LayerConfig config;

    config.transform = *matrix;

    if (!app_.getScene().getMotions().setLayer(*mirrorLayer_, config)) {
        return false;
    }
    reflection_ = next;

    const char* names[] = {"없음", "YZ평면 반사", "원점 점대칭", "Y축 직선대칭"};

    std::cout << "반사: " << names[reflection_] << '\n';

    return true;
}

/// 크기 레이어를 삽입하거나 제거한다.
bool Console::toggleInsertedLayer() {
    auto& motions = app_.getScene().getMotions();

    if (insertedLayer_) {
        if (!motions.removeLayer(*insertedLayer_)) {
            return false;
        }
        insertedLayer_.reset();
        std::cout << "추가 크기 레이어 삭제\n";

        return true;
    }

    const auto scale = graphics::TransformSystem::scaling(glm::vec3(1.5f));

    if (!normalSpinLayer_ || !scale) {
        return false;
    }

    graphics::LayerConfig config;

    config.space = graphics::TransformSpace::Local;
    config.transform = *scale;
    insertedLayer_ = motions.insertLayer(*normalSpinLayer_, config);

    if (!insertedLayer_) {
        return false;
    }
    std::cout << "첫 객체에 1.5배 Local 크기 레이어 삽입\n";

    return true;
}
````
