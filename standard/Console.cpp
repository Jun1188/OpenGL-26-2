#include "Console.h"
#include <iostream>

bool Console::choosePath(int kind) {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);

    if (kind == 1) {
        path.path = graphics::PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 2) {
        path.path = graphics::PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 3) {
        path.path = graphics::PathSystem::polygon({
            {1, -.5f, 0},
            {2.4f, -.4f, 0},
            {2.1f, .6f, 0},
            {1.4f, .8f, 0},
            {.9f, .2f, 0}
        });
    } else if (kind == 4) {
        path.path = [center](double u) {
            const double angle = u * 6.283185307179586;
            return center + glm::vec3(
                float(.7 * std::sin(2 * angle)),
                float(.6 * std::sin(angle)),
                float(.3 * std::cos(angle)));
        };
        path.interpolation = graphics::PathSystem::smoothStep;
    } else if (kind == 5) {
        path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    } else if (kind == 6) {
        path.path = graphics::PathSystem::polyline({
            {1, -.5f, 0},
            {2.4f, -.5f, 0},
            {2, .6f, 0}
        });
        path.playback = graphics::Playback::PingPong;
    } else {
        return false;
    }

    graphics::Motion motion;
    auto& motions = app_.getScene().getMotions();
    if (!pathId_ || !motion.append(graphics::Motion::followPath(path))) {
        return false;
    }
    if (!motions.setMotion(*pathId_, motion) || !motions.seekMotion(*pathId_, 0)) {
        return false;
    }
    std::cout << "경로 " << kind << ": 시작 시각 0초로 변경\n";
    return true;
}

bool Console::initScene() {
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
    if (!scene.setTint(*first, {1, .35f, .2f})
        || !scene.setTint(*second, {.2f, .7f, 1})) {
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
    if (!pathId_ || !spinId_ || !globalId_ || !choosePath(1)) {
        return false;
    }
    if (!motions.setMotionEnabled(*globalId_, false)
        || !motions.bindMotion(graphics::MotionSystem::rootLayer(), *globalId_)) {
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
    if (!mirroredSpin
        || !motions.bindMotion(*normalPath, *pathId_)
        || !motions.bindMotion(*mirroredPath, *pathId_)
        || !motions.bindMotion(*normalSpinLayer_, *spinId_)
        || !motions.bindMotion(*mirroredSpin, *spinId_)
        || !scene.bindEntity(*first, *normalSpinLayer_)
        || !scene.bindEntity(*second, *mirroredSpin)) {
        return false;
    }
    if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
        || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
        || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) {
        return false;
    }

    auto& camera = app_.getCamera();
    camera.position = {5, 3, 8};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    printHelp();
    return true;
}

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

void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }
    auto& motions = app_.getScene().getMotions();
    bool ok = true;
    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        ok = choosePath(key - GLFW_KEY_1 + 1);
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;
        ok = pathId_ && spinId_ && globalId_
            && motions.setMotionPlaying(*pathId_, playing)
            && motions.setMotionPlaying(*spinId_, playing)
            && motions.setMotionPlaying(*globalId_, playing);
        if (ok) {
            paused_ = !paused_;
            std::cout << (paused_ ? "시간 정지\n" : "시간 재개\n");
        }
    } else if (key == GLFW_KEY_S) {
        ok = spinId_ && motions.setMotionEnabled(*spinId_, !spinEnabled_);
        if (ok) {
            spinEnabled_ = !spinEnabled_;
            std::cout << "자전 기여: " << spinEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_G) {
        ok = globalId_ && motions.setMotionEnabled(*globalId_, !globalEnabled_);
        if (ok) {
            globalEnabled_ = !globalEnabled_;
            std::cout << "전체 공전 기여: " << globalEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_M) {
        ok = changeReflection();
    } else if (key == GLFW_KEY_I) {
        ok = toggleInsertedLayer();
    } else if (key == GLFW_KEY_H) {
        printHelp();
    }
    if (!ok) {
        std::cerr << "샘플 명령 적용 실패. 장면을 종료합니다.\n";
        app_.requestClose();
    }
}

void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }
    const auto hit = app_.pickAt(event.windowX, event.windowY);
    if (hit) {
        std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    }
}

void Console::printHelp() const {
    std::cout << "standard: 프레임워크 Motion·반사·레이어 샘플\n"
              << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
              << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
              << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
              << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
              << "왼쪽 클릭: 물체의 entity/face ID 출력\n";
}
