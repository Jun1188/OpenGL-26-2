#include "Console.h"
#include <iostream>

bool Console::initScene() {
    printHelp();
    return activateMode(initialMode_.value_or(1));
}

bool Console::activateMode(int mode) {
    if (mode < 0 || mode > lastStandardMode)
        return false;
    if (mode >= 1 && mode <= 6 && demo_ == Demo::Motion && pathId_) {
        activeMode_ = mode;
        return choosePath(mode);
    }
    if (practiceMode_)
        practiceMode_->onFocus(false);
    practiceMode_.reset();
    activeMode_ = mode;
    animation_.clear();
    jointControls_.clear();
    selectedJoint_ = 0;
    selectedAxis_ = 0;
    roomControl_.reset();
    bodyControl_.reset();
    pathId_.reset();
    spinId_.reset();
    globalId_.reset();
    mirrorLayer_.reset();
    normalSpinLayer_.reset();
    insertedLayer_.reset();
    clock_.reset();
    app_.getCamera() = graphics::Camera{};
    app_.getRenderOptions() = graphics::RenderOptions{};
    if (mode == 0 || mode >= 10) {
        practiceMode_ = makePracticeMode(mode, app_);
        return practiceMode_ && practiceMode_->initScene();
    }
    const bool ok = mode <= 6 ? initMotion(mode) : mode == 7 ? initGrid() : mode == 8 ? initRoom() : initAnimation();
    if (!ok || !applyControls())
        return false;
    printSelectedJoint();
    return true;
}

/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key, int action, int mods) {
    if (!initialMode_ && action == GLFW_PRESS) {
        int next = -1;
        if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F11)
            next = 10 + key - GLFW_KEY_F1;
        else if (key == GLFW_KEY_LEFT_BRACKET)
            next = activeMode_ == 0 ? lastStandardMode : activeMode_ - 1;
        else if (key == GLFW_KEY_RIGHT_BRACKET)
            next = activeMode_ == lastStandardMode ? 0 : activeMode_ + 1;
        else if ((activeMode_ <= 9 || (mods & GLFW_MOD_ALT)) && key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
            next = key - GLFW_KEY_0;
        if (next >= 0) {
            if (!activateMode(next))
                app_.requestClose();
            return;
        }
    }
    if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
        printHelp();
        return;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app_.requestClose();
        return;
    }
    if (practiceMode_) {
        practiceMode_->onKey(key, action, mods);
        return;
    }
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
        pathId_.reset();
        if (!activateMode(activeMode_))
            app_.requestClose();
        return;
    }
    if (action == GLFW_REPEAT) {
        handleJointKey(key);
        return;
    }
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_V) {
        threeViews_ = !threeViews_;

        return;
    }

    if (key == GLFW_KEY_D) {
        trace_ = !trace_;
        graphics::Diagnostics::configure({true, trace_});

        return;
    }

    if (handleJointKey(key))
        return;

    if (key == GLFW_KEY_F && demo_ == Demo::Room) {
        floorOpen_ = !floorOpen_;
        if (!updateRoom(0))
            app_.requestClose();

        return;
    }

    if (demo_ != Demo::Motion) {
        if (key == GLFW_KEY_SPACE)
            clock_.playing = !clock_.playing;
        else if (key == GLFW_KEY_ESCAPE)
            app_.requestClose();
        else if (key == GLFW_KEY_H)
            printHelp();

        return;
    }

    auto& motions = app_.getScene().getMotions();
    bool ok = true;

    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;

        ok = pathId_ && spinId_ && globalId_ && motions.setMotionPlaying(*pathId_, playing) &&
             motions.setMotionPlaying(*spinId_, playing) && motions.setMotionPlaying(*globalId_, playing);

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

/// 마우스 버튼 입력을 처리한다.
void Console::onMouse(const graphics::MouseEvent& event) {
    if (practiceMode_) {
        practiceMode_->onMouse(event);
        return;
    }
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }

    std::optional<graphics::PickResult> hit;

    if (threeViews_) {
        const auto size = app_.getFramebufferSize();

        if (size[0] < 3 || size[1] < 2)
            return;

        const auto passes =
            graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

        for (const auto& pass : passes) {
            const auto& v = pass.viewport;
            const double top = size[1] - v.y - v.height;

            if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top &&
                event.framebufferY < top + v.height)
                hit = graphics::pick(app_.getScene(), pass.camera, event.framebufferX, event.framebufferY,
                                     {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
        }
    } else
        hit = app_.pickAt(event.windowX, event.windowY);

    selectPicked(hit);
}

/// 조작 방법을 출력한다.
void Console::printHelp() const {
    std::cout
        << "standard: initialMode=nullopt이면 모드 전환, 정수이면 해당 모드 고정\n"
        << "[ / ] 전체 0~22 모드 순환, Alt+0~9 데모 선택, F12 공통 도움말\n"
        << "F1~F11: 모드 10~20 = 실습 13~23 (각 실습의 기존 키 사용)\n"
        << "모드 21: WASD 카메라 충돌 / 모드 22: 내장 3D 메시 입력 처리\n"
        << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
        << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
        << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
        << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
        << "왼쪽 클릭: 사람·그네·상자 전체 선택 / 빈 공간: 선택 해제\n"
        << "0 기능 조립 콘솔: help / guide / save / load / 7 Grid / 8 방·물리\n"
        << "9 선형보간·진자·사람형·그네→사람 계층 애니메이션\n"
        << "1~9: Home 전체 복원 / Tab/Backspace 객체·관절 선택, X/Y/Z 축, 좌우 회전, 상하 이동, PageUp/Down 피벗, +/- 균일 배율, R 복원\n"
        << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

bool Console::applyControls() {
    if (!graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds))
        return false;
    return demo_ == Demo::Grid ? updateGrid() : demo_ == Demo::Room ? updateRoom(0) : true;
}

void Console::onUpdate(float dt) {
    if (practiceMode_) {
        practiceMode_->onUpdate(dt);
        return;
    }
    if (demo_ != Demo::Motion && !clock_.advance(dt)) {
        app_.requestClose();
        return;
    }
    bool ok = demo_ == Demo::Animation ? updateAnimation()
        : graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds);
    if (demo_ == Demo::Grid)
        ok = updateGrid() && ok;
    else if (demo_ == Demo::Room)
        ok = updateRoom(clock_.playing ? dt : 0) && ok;
    if (!ok)
        app_.requestClose();
}

/// 현재 장면을 지정한 뷰로 렌더링한다.
void Console::onCursor(double x, double y) {
    if (practiceMode_)
        practiceMode_->onCursor(x, y);
}

void Console::onFocus(bool focused) {
    if (practiceMode_)
        practiceMode_->onFocus(focused);
}

bool Console::onRender() {
    if (practiceMode_)
        return practiceMode_->onRender();
    if (!threeViews_)
        return app_.renderScene();

    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    return app_.renderViews(
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
