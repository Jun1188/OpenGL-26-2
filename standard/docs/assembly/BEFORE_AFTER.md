# 0번 조립 API 전체 변경 전후

2026-11-03. before 기준 `2e2e821402e3740d91a79a43d167144a77dffdd6`, after 소스 커밋 `23a1eaef967df1e2e3b9312023feefc881d59d88`. 마지막 문서 단계의 도구/레시피는 현재 저장된 파일이다. 빌드·테스트·검증기·앱 실행은 하지 않았다.

이 보고서는 수정한 코드·설정·레시피의 **파일 전문**을 짝지어 싣는다. 따라서 함수별 변경 블록도 빠짐없이 포함한다. 새 파일은 before 없음이다. 문서 자체와 생성 그래프 데이터는 재귀적인 전후 코드 대상에서 제외한다. 기존 사용자 API 문서·Framework.props·부재 실습 파일은 이 작업에 포함하지 않았다.

[명세](SPEC.md) · [직접 편집](CODE_GUIDE.md) · [패턴](PATTERN_COMBINATIONS.md) · [반복 소스 검토](REQUIREMENT_AUDIT.md) · [사용자 확인](USER_CHECKS.md)

## 단계와 로컬 커밋

| 단계 | 목적 | 커밋 |
|---|---|---|
| 1 | 이름 있는 조립 API·공통 부품 | 23cc0d4 |
| 2 | 모드0 콘솔·소스 안내 | 36c3c26 |
| 3 | 소유권·레시피 재구성 | 7af4268 |
| 4 | 보행 바닥 이탈 | 585cdd8 |
| 5 | 원시 부품 계층·탱크·트럭·사람 | 014b7d1 |
| 6 | 물리 형상 잠금·배포 경로·부모축 자전 | 8c4bf20 |
| 7 | 면 관절·노멀·비균일/반사·Custom | ec1e5eb |
| 8 | CRLF 표식·드래그 복구·guide 목록 | ecbeba5 |
| 9 | 기능 단위 입력 콜백·수명 | 23a1eae |
| 10 | 문서·레시피·구조 그래프 | 이 보고서와 함께 별도 커밋 |

## 파일 목차

1. [standard/Console.cpp](#file-1)
2. [standard/examples/assembly/person.assembly](#file-2)
3. [standard/examples/assembly/surface-lab.assembly](#file-3)
4. [standard/examples/assembly/tank.assembly](#file-4)
5. [standard/examples/assembly/truck.assembly](#file-5)
6. [standard/main.cpp](#file-6)
7. [standard/modes/Assembly.cpp](#file-7)
8. [standard/modes/Assembly.h](#file-8)
9. [standard/modes/AssemblyCommands.cpp](#file-9)
10. [standard/modes/AssemblyConsole.cpp](#file-10)
11. [standard/modes/AssemblyConsole.h](#file-11)
12. [standard/modes/AssemblyExamples.cpp](#file-12)
13. [standard/modes/AssemblyExamples.h](#file-13)
14. [standard/modes/AssemblyExtensions.cpp](#file-14)
15. [standard/modes/AssemblyGuide.cpp](#file-15)
16. [standard/modes/AssemblyHierarchy.cpp](#file-16)
17. [standard/modes/AssemblyInput.cpp](#file-17)
18. [standard/modes/AssemblyMotion.cpp](#file-18)
19. [standard/modes/AssemblyObjects.cpp](#file-19)
20. [standard/modes/AssemblyParts.cpp](#file-20)
21. [standard/modes/AssemblyParts.h](#file-21)
22. [standard/modes/AssemblyPersistence.cpp](#file-22)
23. [standard/modes/AssemblyRuntime.cpp](#file-23)
24. [standard/modes/AssemblySurface.cpp](#file-24)
25. [standard/modes/AssemblyTracks.cpp](#file-25)
26. [standard/modes/Mode0Assembly.cpp](#file-26)
27. [standard/modes/Mode0Recipe.cpp](#file-27)
28. [standard/modes/Mode7Grid.cpp](#file-28)
29. [standard/modes/PracticeMode.h](#file-29)
30. [standard/modes/PracticeRegistry.cpp](#file-30)
31. [standard/modes/TankCommon.cpp](#file-31)
32. [standard/standard.vcxproj](#file-32)
33. [tools/Update-StandardModesGraph.py](#file-33)

<a id="file-1"></a>
## standard/Console.cpp

변경 이유: 0번 팩토리 선택, nullopt 숫자0/Alt+0 전환과0~22 순환을 연결한다.

### before

```cpp
#include "Console.h"
#include <iostream>

bool Console::initScene() {
    printHelp();
    return activateMode(initialMode_.value_or(1));
}

bool Console::activateMode(int mode) {
    if (mode < 1 || mode > lastStandardMode)
        return false;
    if (mode <= 6 && demo_ == Demo::Motion && pathId_) {
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
    if (mode >= 10) {
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
        int next = 0;
        if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F11)
            next = 10 + key - GLFW_KEY_F1;
        else if (key == GLFW_KEY_LEFT_BRACKET)
            next = activeMode_ == 1 ? lastStandardMode : activeMode_ - 1;
        else if (key == GLFW_KEY_RIGHT_BRACKET)
            next = activeMode_ == lastStandardMode ? 1 : activeMode_ + 1;
        else if ((activeMode_ <= 9 || (mods & GLFW_MOD_ALT)) && key >= GLFW_KEY_1 && key <= GLFW_KEY_9)
            next = key - GLFW_KEY_1 + 1;
        if (next) {
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

    if (!initialMode_ && key == GLFW_KEY_0) {
        pathId_.reset();
        if (!activateMode(1))
            app_.requestClose();
        return;
    }

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
        << "[ / ] 전체 1~22 모드 순환, Alt+1~9 데모 선택, F12 공통 도움말\n"
        << "F1~F11: 모드 10~20 = 실습 13~23 (각 실습의 기존 키 사용)\n"
        << "모드 21: WASD 카메라 충돌 / 모드 22: 내장 3D 메시 입력 처리\n"
        << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
        << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
        << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
        << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
        << "왼쪽 클릭: 사람·그네·상자 전체 선택 / 빈 공간: 선택 해제\n"
        << "7 Grid/거리 기반 배치 / 8 실내 미끄럼·낙하 / 0 기존 Motion 예제\n"
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
```

### after

```cpp
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
```

<a id="file-2"></a>
## standard/examples/assembly/person.assembly

변경 이유: 부품 조합과 동작을 재구성할 수 있는 원시 명령 예제를 추가한다.

### before

없음. 새 파일이다.

### after

```text
# 육면체 계층: 사람. 충돌 보행이 아닌 관절 모션 예제. guide person-pattern
clear
camera pose 4 3 6 0 1 0 0 1 0
group person 0 0 0
joint person pelvis 0 0.8 0
part box person.pelvis body 0 0 0 0.4 0.25 0.3
joint person.pelvis torso 0 0.35 0
part box person.pelvis.torso body 0 0 0 0.55 0.6 0.3
joint person.pelvis.torso neck 0 0.35 0
part box person.pelvis.torso.neck head 0 0.18 0 0.35 0.35 0.35
part box person.pelvis.torso.neck nose 0 0.18 0.21 0.1 0.08 0.12
joint person.pelvis.torso leftShoulder -0.38 0.17 0
joint person.pelvis.torso rightShoulder 0.38 0.17 0
part box person.pelvis.torso.leftShoulder upperArm 0 -0.2 0 0.14 0.4 0.16
part box person.pelvis.torso.rightShoulder upperArm 0 -0.2 0 0.14 0.4 0.16
joint person.pelvis.torso.leftShoulder elbow 0 -0.4 0
joint person.pelvis.torso.rightShoulder elbow 0 -0.4 0
part box person.pelvis.torso.leftShoulder.elbow forearm 0 -0.18 0 0.13 0.36 0.15
part box person.pelvis.torso.rightShoulder.elbow forearm 0 -0.18 0 0.13 0.36 0.15
joint person.pelvis leftHip -0.13 0 0
joint person.pelvis rightHip 0.13 0 0
part box person.pelvis.leftHip thigh 0 -0.2 0 0.17 0.4 0.2
part box person.pelvis.rightHip thigh 0 -0.2 0 0.17 0.4 0.2
joint person.pelvis.leftHip knee 0 -0.4 0
joint person.pelvis.rightHip knee 0 -0.4 0
part box person.pelvis.leftHip.knee shin 0 -0.2 0 0.17 0.4 0.2
part box person.pelvis.rightHip.knee shin 0 -0.2 0 0.17 0.4 0.2
part box person.pelvis.leftHip.knee foot 0 -0.4 0.08 0.2 0.1 0.35
part box person.pelvis.rightHip.knee foot 0 -0.4 0.08 0.2 0.1 0.35
pendulum leftArm person.pelvis.torso.leftShoulder x 25 2
pendulum rightArm person.pelvis.torso.rightShoulder x 25 2
seek rightArm 1
pendulum leftLeg person.pelvis.leftHip x 25 2
pendulum rightLeg person.pelvis.rightHip x 25 2
seek leftLeg 1
pivot person 0 0.9 0 keep
linear stroll person 3 0 0 5 pingpong
select person
```

<a id="file-3"></a>
## standard/examples/assembly/surface-lab.assembly

변경 이유: 부품 조합과 동작을 재구성할 수 있는 원시 명령 예제를 추가한다.

### before

없음. 새 파일이다.

### after

```text
# 면에 붙인 관절, 노멀, 컬링, 지연 면 트랙. guide mount / guide face-track
clear
camera pose 6 5 8 0 0.8 0 0 1 0
add box base 0 0 0 2 2 2
mount base connector 4 0 0 0.5 0
part box base.connector pole 0 0 0.5 0.15 0.15 1
pendulum poleMotor base.connector y 25 3
face-track fold base 4 2 0 pingpong turn 0 0 1 -1 1 0 90
face-track shrink base 1 2 1 pingpong scale 0.2 0.2 0.2 0 0 -1
normal base all auto
cull base all back
outline base all features
render lighting on
render light-direction 1 2 1
render light-color 1 0.95 0.9
select base
```

<a id="file-4"></a>
## standard/examples/assembly/tank.assembly

변경 이유: 부품 조합과 동작을 재구성할 수 있는 원시 명령 예제를 추가한다.

### before

없음. 새 파일이다.

### after

```text
# 육면체 계층: 탱크. 세부 C++ 예제는 guide tank-pattern
clear
camera pose 10 7 12 0 1 0 0 1 0
group tank 0 0 0
part box tank body 0 0.45 0 2.2 0.5 2.6
part box tank leftTrack -1.05 0.3 0 0.45 0.6 3
part box tank rightTrack 1.05 0.3 0 0.45 0.6 3
joint tank turret 0 0.8 0
part box tank.turret body 0 0.25 0 1.4 0.5 1.5
joint tank.turret gun 0 0.3 0.65
part box tank.turret.gun barrel 0 0 1 0.2 0.2 2
part box tank.turret.gun muzzle 0 0 2 0.35 0.35 0.3
pivot tank 0 0.6 0 keep
color tank 0.3 0.6 0.4
spin turretMotor tank.turret y 25
pendulum gunMotor tank.turret.gun x 10 4
move tank 3 0 0
spin tankSpin tank y 20
group orbitFrame 0 0 0
parent tank orbitFrame
orbit orbitMotor orbitFrame y 12 0 0 0
select tank
```

<a id="file-5"></a>
## standard/examples/assembly/truck.assembly

변경 이유: 부품 조합과 동작을 재구성할 수 있는 원시 명령 예제를 추가한다.

### before

없음. 새 파일이다.

### after

```text
# 육면체 계층: 트럭과 컨테이너. guide truck-pattern
clear
camera pose 6 4 -8 0 1 0 0 1 0
group truck 0 0 0
part box truck chassis 0 0.6 0 1.8 0.3 3.2
part box truck cabin 0 1.2 1.1 1.8 1.2 1.2
part box truck wheel1 -0.95 0.3 -1.05 0.4 0.6 0.6
part box truck wheel2 0.95 0.3 -1.05 0.4 0.6 0.6
part box truck wheel3 -0.95 0.3 1.05 0.4 0.6 0.6
part box truck wheel4 0.95 0.3 1.05 0.4 0.6 0.6
joint truck container 0 0.85 -0.65
part box truck.container floor 0 0 0 1.8 0.15 1.9
part box truck.container roof 0 1.25 0 1.8 0.15 1.9
part box truck.container leftWall -0.85 0.625 0 0.1 1.2 1.9
part box truck.container rightWall 0.85 0.625 0 0.1 1.2 1.9
part box truck.container frontWall 0 0.625 0.9 1.8 1.2 0.1
joint truck.container leftDoor -0.85 0 -0.95
joint truck.container rightDoor 0.85 0 -0.95
part box truck.container.leftDoor panel 0.425 0.625 0 0.85 1.2 0.1
part box truck.container.rightDoor panel -0.425 0.625 0 0.85 1.2 0.1
pivot truck 0 0.8 0 keep
pendulum leftDoorMotor truck.container.leftDoor y 40 6
pendulum rightDoorMotor truck.container.rightDoor y 40 6
seek rightDoorMotor 3
bind RIGHT hold "move truck 1 0 0"
bind LEFT hold "move truck -1 0 0"
select truck
```

<a id="file-6"></a>
## standard/main.cpp

변경 이유: 단일 optional 상수의 허용 범위를0~22로 확장한다.

### before

```cpp
#include "Console.h"
#include <exception>
#include <iostream>

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() try {
    graphics::Application app;
    // nullopt: 숫자 키로 전환. 1~9: 기본 데모 고정. 10~22: 추가 실습·부록 고정.
    constexpr std::optional<int> initialMode = std::nullopt;
    static_assert(!initialMode || (*initialMode >= 1 && *initialMode <= lastStandardMode), "initialMode must be 1..22");
    Console sample(app, initialMode);

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
        sample.onKey(key, action, mods);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });
    app.setCursorHandler([&](graphics::Application&, double x, double y) {
        sample.onCursor(x, y);
    });
    app.setFocusHandler([&](graphics::Application&, bool focused) {
        sample.onFocus(focused);
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) {
        sample.onUpdate(dt);
    });
    app.setRenderHandler([&](graphics::Application&) {
        return sample.onRender();
    });

    graphics::ApplicationConfig config;

    config.title = "standard - Modes 1..22, input, picking and practice patterns";

    return app.run(config);
} catch (const std::exception& error) {
    std::cerr << "standard 실행 실패: " << error.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "standard 실행 실패: 앞선 프레임워크 진단을 확인하세요.\n";
    return 1;
}
```

### after

```cpp
#include "Console.h"
#include <exception>
#include <iostream>

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() try {
    graphics::Application app;
    // nullopt: 숫자 키로 전환. 0: 조립 콘솔 고정. 1~9: 기본 데모 고정. 10~22: 추가 실습·부록 고정.
    constexpr std::optional<int> initialMode = std::nullopt;
    static_assert(!initialMode || (*initialMode >= 0 && *initialMode <= lastStandardMode), "initialMode must be 0..22");
    Console sample(app, initialMode);

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
        sample.onKey(key, action, mods);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });
    app.setCursorHandler([&](graphics::Application&, double x, double y) {
        sample.onCursor(x, y);
    });
    app.setFocusHandler([&](graphics::Application&, bool focused) {
        sample.onFocus(focused);
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) {
        sample.onUpdate(dt);
    });
    app.setRenderHandler([&](graphics::Application&) {
        return sample.onRender();
    });

    graphics::ApplicationConfig config;

    config.title = "standard - Modes 0..22, input, picking and practice patterns";

    return app.run(config);
} catch (const std::exception& error) {
    std::cerr << "standard 실행 실패: " << error.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "standard 실행 실패: 앞선 프레임워크 진단을 확인하세요.\n";
    return 1;
}
```

<a id="file-7"></a>
## standard/modes/Assembly.cpp

변경 이유: 명령 원자성, 노드/동작 소유권, SRT 계약을 공유한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <limits>
#include <sstream>
#include <stdexcept>

namespace assembly {
void require(bool success, const std::string& message) {
    if (!success)
        throw std::runtime_error(message);
}

Tokens tokenize(const std::string& line) {
    Tokens result;
    std::string token;
    bool quoted = false, started = false;
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (c == '#' && !quoted)
            break;
        if (c == '"') {
            quoted = !quoted;
            started = true;
        } else if (c == '\\' && quoted && i + 1 < line.size() && (line[i + 1] == '"' || line[i + 1] == '\\')) {
            token += line[++i];
        } else if (!quoted && std::isspace(static_cast<unsigned char>(c))) {
            if (started) {
                result.push_back(token);
                token.clear();
                started = false;
            }
        } else {
            token += c;
            started = true;
        }
    }
    require(!quoted, "닫히지 않은 따옴표입니다.");
    if (started)
        result.push_back(token);
    return result;
}

std::string quote(const std::string& text) {
    std::ostringstream out;
    out << std::quoted(text);
    return out.str();
}

double number(const std::string& text) {
    std::size_t consumed = 0;
    const double value = std::stod(text, &consumed);
    require(consumed == text.size() && std::isfinite(value) && std::abs(value) <= 1000000,
            "유한한 수(-1000000~1000000)가 필요합니다: " + text);
    return value;
}

std::size_t indexNumber(const std::string& text) {
    const auto value = number(text);
    require(value >= 0 && value == std::floor(value), "0 이상 정수 인덱스가 필요합니다.");
    return std::size_t(value);
}

glm::vec3 vector(const Tokens& args, std::size_t offset) {
    require(offset + 3 <= args.size(), "XYZ 값 3개가 필요합니다.");
    return {float(number(args[offset])), float(number(args[offset + 1])), float(number(args[offset + 2]))};
}

glm::vec3 axis(const std::string& text) {
    require(text == "x" || text == "y" || text == "z", "축은 x/y/z 중 하나입니다.");
    return text == "x" ? glm::vec3(1, 0, 0) : text == "y" ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, 1);
}

bool boolean(const std::string& text) {
    require(text == "on" || text == "off", "on 또는 off가 필요합니다.");
    return text == "on";
}

bool rigid(const glm::mat4& matrix) {
    const auto r = glm::mat3(matrix), gram = glm::transpose(r) * r;
    return graphics::TransformSystem::validAffine(matrix) && std::abs(glm::determinant(r) - 1) < .001f &&
        glm::length(gram[0] - glm::vec3(1, 0, 0)) < .001f &&
        glm::length(gram[1] - glm::vec3(0, 1, 0)) < .001f &&
        glm::length(gram[2] - glm::vec3(0, 0, 1)) < .001f;
}

Node& Assembly::node(const std::string& name) {
    const auto found = state_.nodes.find(name);
    require(found != state_.nodes.end(), "없는 객체/부품입니다: " + name + ". list 또는 guide objects를 사용하세요.");
    return found->second;
}

const Node& Assembly::node(const std::string& name) const {
    const auto found = state_.nodes.find(name);
    require(found != state_.nodes.end(), "없는 객체/부품입니다: " + name);
    return found->second;
}

void Assembly::clear() {
    ++topologyRevision_;
    app_.getScene().clear();
    state_ = State{};
    faulted_ = false;
}

void Assembly::registerNode(const std::string& name, graphics::EntityId id, bool whole,
                            const std::string& kind, std::optional<graphics::EntityId> mesh) {
    require(state_.nodes.size() < 1000, "객체/부품 이름 1000개 한도입니다.");
    require(!name.empty() && !state_.nodes.count(name), "중복 이름입니다: " + name);
    const auto* component = app_.getScene().getComponent(id);
    require(component != nullptr, "등록할 엔티티가 없습니다.");
    for (const auto& item : state_.nodes)
        require(item.second.id != id, "같은 엔티티를 두 이름으로 등록할 수 없습니다.");
    ++topologyRevision_;
    state_.nodes.emplace(name, Node{id, mesh, component->transform, component->transform, kind, whole, {}, {}});
    if (state_.selected.empty() && whole)
        state_.selected = name;
}

Behavior* Assembly::positionDriver(const std::string& target) {
    for (auto& behavior : state_.behaviors)
        if (behavior.target == target && behavior.positionDriver)
            return &behavior;
    return nullptr;
}

void Assembly::attach(Behavior behavior) {
    require(!behavior.name.empty(), "동작 이름이 필요합니다.");
    const auto& targetNode = node(behavior.target);
    require(!targetNode.affineGroup || behavior.effect == Effect::Custom || behavior.effect == Effect::Input,
            "비균일/반사 그룹은 정적 계층입니다. 자식 group/joint에 모션을 붙이세요.");
    if (behavior.effect == Effect::Input)
        require(bool(behavior.key) || bool(behavior.mouse) || bool(behavior.cursor) || bool(behavior.focus), "Input에는 입력 콜백이 필요합니다.");
    if (behavior.effect == Effect::Custom) {
        require(bool(behavior.custom), "Custom 동작 함수가 필요합니다.");
        behavior.positionDriver = true;
    }
    if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Conveyor)
        behavior.positionDriver = true;
    const auto checkGeometryOwner = [&](const Behavior& physics, const Behavior& other) {
        if (other.effect == Effect::Input || (physics.effect != Effect::Body && physics.effect != Effect::Walk)) return;
        const auto& object = node(other.target);
        const auto& room = node(physics.reference);
        const bool face = faceEffect(other.effect);
        if (physics.effect == Effect::Body && face)
            require(object.mesh != node(physics.target).mesh, "물리 상자 면은 별도로 변형할 수 없습니다.");
        if (!face)
            require(!room.mesh || object.id != *room.mesh, "물리 방 walls에는 별도 이동/회전을 붙일 수 없습니다. 방 루트를 사용하세요.");
    };
    for (const auto& current : state_.behaviors) {
        checkGeometryOwner(current, behavior);
        checkGeometryOwner(behavior, current);
        require(current.name != behavior.name, "중복 동작 이름입니다: " + behavior.name);
        require(behavior.effect == Effect::Input || current.effect == Effect::Input ||
                (!(current.effect == Effect::Body && behavior.target == current.target + ".mesh") &&
                 !(behavior.effect == Effect::Body && current.target == behavior.target + ".mesh")),
                "물리 메시에는 별도 동작을 붙일 수 없습니다. 상자 루트를 사용하세요.");
        const bool currentFace = faceEffect(current.effect);
        const bool newFace = faceEffect(behavior.effect);
        if (currentFace && newFace)
            require(node(current.target).mesh != node(behavior.target).mesh || current.faceIndex != behavior.faceIndex,
                    "같은 메시의 같은 면에 동작이 이미 있습니다.");
        if (current.target != behavior.target)
            continue;
        require(!(current.positionDriver && behavior.positionDriver), "위치 제어가 충돌합니다. 기존 동작을 remove 하거나 자식 부품에 붙이세요.");
        const bool currentPhysics = current.effect == Effect::Body || current.effect == Effect::Walk || current.effect == Effect::Custom;
        const bool newPhysics = behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Custom;
        require(!((currentPhysics && behavior.effect == Effect::Motion) || (newPhysics && current.effect == Effect::Motion)),
                "물리 루트와 Motion은 같은 SRT를 소유할 수 없습니다. 자식 부품에 동작을 붙이세요.");
    }
    if (behavior.effect == Effect::Motion || behavior.effect == Effect::FaceTrack) {
        for (const auto& step : behavior.motion.getSteps())
            require(!step.action, "변환 모션에 action을 넣을 수 없습니다. bind 명령 또는 customCommand를 사용하세요.");
        const auto sample = behavior.motion.sample(0);
        require(sample.has_value(), "유효한 모션 샘플이 필요합니다.");
        if (behavior.effect == Effect::Motion)
            require(rigid(sample->parent) && rigid(sample->local), "객체 Motion은 유효한 강체 변환이어야 합니다.");
    }
    if (faceEffect(behavior.effect)) {
        const auto& target = node(behavior.target);
        require(target.mesh.has_value(), "면 동작에는 메시가 필요합니다.");
        const auto* component = app_.getScene().getComponent(*target.mesh);
        require(component && behavior.faceIndex < component->polyhedron.faces.size(), "면 번호 범위를 확인하세요.");
        const auto& face = component->polyhedron.faces[behavior.faceIndex];
        behavior.initialFaceMatrix = face.localAnimationMatrix;
        behavior.initialFaceSpace = face.animationSpace;
        behavior.initialFaceVisible = face.visible;
        const auto srt = graphics::detail::faceSrtMatrix(face);
        behavior.faceTrackBase = face.animationSpace == graphics::FaceAnimationSpace::Model
            ? face.localAnimationMatrix : srt * face.localAnimationMatrix * glm::inverse(srt);
        require(graphics::TransformSystem::validAffine(behavior.faceTrackBase), "면의 기준 행렬이 특이합니다.");
    }
    state_.behaviors.push_back(std::move(behavior));
    ++topologyRevision_;
}

void Assembly::setRest(const std::string& target, const graphics::Transform3D& rest) {
    auto& value = node(target);
    bool immutable = false;
    for (const auto& behavior : state_.behaviors) {
        immutable = immutable || (behavior.effect == Effect::Body && target == behavior.target + ".mesh");
        if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk) {
            const auto& room = node(behavior.reference);
            immutable = immutable || (room.mesh && value.id == *room.mesh);
        }
    }
    if (immutable) {
        require(rest.basis == value.rest.basis && rest.localBasis == value.rest.localBasis && rest.scale == value.rest.scale &&
            rest.rotation[0] == value.rest.rotation[0] && rest.rotation[1] == value.rest.rotation[1] &&
            rest.rotation[2] == value.rest.rotation[2], "물리 메시/walls 대신 객체 루트를 편집하세요.");
        return;
    }
    require(glm::all(glm::lessThanEqual(glm::abs(rest.basis), glm::vec3(1000000))) &&
            glm::all(glm::lessThanEqual(glm::abs(rest.localBasis), glm::vec3(1000000))), "위치/피벗 범위는 ±1000000입니다.");
    const bool validScale = value.affineGroup
        ? glm::all(glm::greaterThan(glm::abs(rest.scale), glm::vec3(.0001f))) &&
          glm::all(glm::lessThanEqual(glm::abs(rest.scale), glm::vec3(100)))
        : rest.scale.x > .0001f && rest.scale.x <= 100 && std::abs(rest.scale.x - rest.scale.y) < .0001f &&
          std::abs(rest.scale.x - rest.scale.z) < .0001f;
    require(graphics::DiagramSystem::validTransform(rest) && validScale,
            "유효한 SRT가 필요합니다. 기본은 양수 균일 배율이며 warp 그룹은 0이 아닌 축별 배율(.0001~100)을 씁니다.");
    for (const auto& behavior : state_.behaviors) {
        if ((behavior.effect == Effect::Body || behavior.effect == Effect::Walk) && behavior.reference == target)
            require(rigid(graphics::DiagramSystem::modelMatrix(rest)), "물리 공간은 강체 변환을 유지해야 합니다.");
        if (behavior.effect == Effect::Walk && behavior.reference == target)
            require(glm::length(rest.rotation[0] - glm::vec3(1, 0, 0)) < .001f &&
                    glm::length(rest.rotation[1] - glm::vec3(0, 1, 0)) < .001f,
                    "보행 충돌 공간은 월드 축 정렬을 유지해야 합니다.");
    }
    if (auto* driver = positionDriver(target)) {
        if (driver->effect == Effect::Body) {
            require(glm::length(rest.localBasis) < .0001f, "물리 상자의 피벗은 중심을 유지합니다.");
            const auto matrix = app_.getScene().worldMatrix(node(driver->reference).id);
            require(matrix && rigid(*matrix), "방의 강체 행렬이 필요합니다.");
            const auto delta = rest.basis - value.rest.basis;
            if (driver->body.released)
                driver->body.worldPosition += delta;
            else
                driver->body.localPosition += glm::transpose(glm::mat3(*matrix)) * delta;
        } else if (driver->effect == Effect::Walk) {
            require(rest.scale == glm::vec3(1) && glm::length(rest.localBasis) < .0001f,
                    "보행 루트는 배율 1과 중심 피벗을 유지합니다.");
            require(std::abs(rest.rotation[1].y - 1) < .001f, "보행 루트는 Y축 회전만 지원합니다.");
            driver->character.feet += rest.basis - value.rest.basis;
            driver->character.direction = rest.rotation * glm::vec3(0, 0, 1);
        }
    }
    value.rest = rest;
}

void Assembly::move(const std::string& target, glm::vec3 delta) {
    auto rest = node(target).rest;
    rest.basis += delta;
    setRest(target, rest);
}

void Assembly::rotate(const std::string& target, glm::vec3 direction, float degrees) {
    auto rest = node(target).rest;
    rest.rotation *= glm::mat3(glm::rotate(glm::mat4(1), glm::radians(degrees), direction));
    const auto x = glm::normalize(rest.rotation[0]);
    const auto z = glm::normalize(glm::cross(x, rest.rotation[1]));
    rest.rotation = glm::mat3(x, glm::cross(z, x), z);
    setRest(target, rest);
}

void Assembly::inspect(const std::string& target, std::ostream& out) const {
    const auto& value = node(target);
    const auto& t = value.rest;
    out << target << " kind=" << value.kind << " id=" << value.id << " position="
        << t.basis.x << ',' << t.basis.y << ',' << t.basis.z << " scale=" << t.scale.x << '\n';
    for (const auto& behavior : state_.behaviors)
        if (behavior.target == target)
            out << "  " << behavior.name << " time=" << behavior.seconds << " playing=" << behavior.playing
                << " enabled=" << behavior.enabled << '\n';
}

bool Assembly::execute(const std::string& line, std::ostream& out) {
    Tokens args;
    try {
        require(line.size() <= 4096, "명령은 4096바이트 이하입니다.");
        args = tokenize(line);
    } catch (const std::exception& error) {
        out << "명령 거절: " << error.what() << '\n';
        return false;
    }
    if (args.empty())
        return true;
    auto savedScene = app_.getScene();
    const auto savedState = state_;
    const auto savedCamera = app_.getCamera();
    const auto savedOptions = app_.getRenderOptions();
    const bool savedFault = faulted_;
    const bool readOnly = args[0] == "save" || args[0] == "guide" || args[0] == "help" || args[0] == "list" || args[0] == "inspect" || args[0] == "surface";
    try {
        require(dispatch(args, out), "지원하지 않는 명령입니다. help 또는 guide custom을 사용하세요.");
        if (!readOnly)
            require(applyPoses(0), "조립 결과의 상태/변환이 유효하지 않습니다.");
        if (!replaying_ && !readOnly && args[0] != "load" && args[0] != "select" && args[0] != "jump" && args[0] != "release")
            state_.recipe.push_back(line);
        if (!readOnly) faulted_ = false;
        return true;
    } catch (const std::exception& error) {
        app_.getScene() = std::move(savedScene);
        state_ = savedState;
        faulted_ = savedFault;
        app_.getCamera() = savedCamera;
        app_.getRenderOptions() = savedOptions;
        out << "명령 거절: " << error.what() << "\n이전 조립 상태를 유지했습니다. guide " << args[0] << '\n';
        return false;
    }
}
} // namespace assembly
```

<a id="file-8"></a>
## standard/modes/Assembly.h

변경 이유: 이름 있는 부품과 동작 상태, 직접 편집용 API의 계약을 선언한다.

### before

없음. 새 파일이다.

### after

```cpp
#pragma once
#include "PracticeMode.h"
#include <functional>
#include <map>
#include <ostream>

namespace assembly {
using Tokens = std::vector<std::string>;
class Assembly;

struct FaceMount {
    graphics::EntityId mesh = 0;
    std::size_t face = 0, triangle = 0;
    glm::vec3 coordinate{0};
    bool barycentric = true;
};

struct Node {
    graphics::EntityId id = 0;
    std::optional<graphics::EntityId> mesh;
    graphics::Transform3D rest, initial;
    std::string kind;
    bool whole = false;
    std::optional<graphics::Bounds3> room;
    std::optional<graphics::JointRig> walker;
    std::string guideTopic;
    std::optional<FaceMount> mount;
    bool affineGroup = false;
};

enum class Effect { Motion, Face, FaceTrack, Wall, Body, Walk, Conveyor, Custom, Input };
inline bool faceEffect(Effect effect) { return effect == Effect::Face || effect == Effect::FaceTrack || effect == Effect::Wall; }
struct Behavior {
    std::string name, target, reference;
    Effect effect = Effect::Motion;
    graphics::Motion motion;
    double seconds = 0;
    bool playing = true, enabled = true, released = false;
    bool positionDriver = false, parentSpin = false;
    graphics::FaceRotationConfig face;
    std::size_t faceIndex = 0;
    glm::mat4 initialFaceMatrix{1};
    graphics::FaceAnimationSpace initialFaceSpace = graphics::FaceAnimationSpace::Local;
    bool initialFaceVisible = true;
    glm::mat4 faceTrackBase{1};
    float rate = 1, phase = 0, height = .455f;
    graphics::RoomBody body;
    glm::vec3 gravity{0, -9.8f, 0}, angularVelocity{0};
    bool elastic = false, releaseRequested = false;
    glm::mat3 releasedRotation{1};
    graphics::CharacterBody character;
    std::vector<std::string> obstacles;
    glm::mat4 previousRoom{1};
    std::function<bool(graphics::Scene&, const graphics::Transform3D&, double, float)> custom;
    std::function<bool(Assembly&, int, int, int)> key;
    std::function<bool(Assembly&, const graphics::MouseEvent&)> mouse;
    std::function<bool(Assembly&, double, double)> cursor;
    std::function<void(Assembly&, bool)> focus;
};

struct Binding {
    int key = 0, mods = 0;
    bool hold = false, down = false;
    std::string command;
    Tokens tokens;
};

class Assembly {
    struct State {
        std::map<std::string, Node> nodes;
        std::vector<Behavior> behaviors;
        std::vector<Binding> bindings;
        std::vector<std::string> recipe;
        std::string selected;
        int axis = 0;
        std::string dragTarget;
        glm::vec3 dragAxis{0, 1, 0};
        float dragSensitivity = .35f;
        bool dragging = false;
        std::optional<double> dragX;
        bool paused = false, threeViews = false;
    } state_;
    graphics::Application& app_;
    bool replaying_ = false, faulted_ = false;
    std::uint64_t topologyRevision_ = 0;

    bool dispatch(const Tokens& args, std::ostream& out);
    bool motionCommand(const Tokens& args, std::ostream& out);
    bool objectCommand(const Tokens& args, std::ostream& out);
    bool hierarchyCommand(const Tokens& args, std::ostream& out);
    bool surfaceCommand(const Tokens& args, std::ostream& out);
    bool trackCommand(const Tokens& args, std::ostream& out);
    bool inputCommand(const Tokens& args, std::ostream& out);
    bool applyPoses(float dt);
    bool dispatchInput(const std::function<bool(const Behavior&)>& callback, std::ostream& out);
    void applyMounts();
    void registerNode(const std::string& name, graphics::EntityId id, bool whole,
                      const std::string& kind = "part", std::optional<graphics::EntityId> mesh = std::nullopt);
    void registerPerson(const std::string& prefix, const graphics::HumanoidRig& rig);
    Behavior* positionDriver(const std::string& target);
    bool save(const std::string& path, std::ostream& out) const;
    bool load(const std::string& path, std::ostream& out);
public:
    explicit Assembly(graphics::Application& app) : app_(app) {}
    graphics::Application& application() { return app_; }
    const std::map<std::string, Node>& nodes() const { return state_.nodes; }
    const std::vector<Behavior>& behaviors() const { return state_.behaviors; }
    Node& node(const std::string& name);
    const Node& node(const std::string& name) const;
    void clear();
    void add(const std::string& kind, const std::string& name, glm::vec3 position,
             glm::vec3 size = glm::vec3(1));
    void group(const std::string& name, glm::vec3 position);
    std::string part(const std::string& kind, const std::string& parent, const std::string& localName,
                     glm::vec3 position, glm::vec3 size = glm::vec3(1));
    void srt(const std::string& target, glm::vec3 position, glm::vec3 degrees, float scale);
    void pivot(const std::string& target, glm::vec3 point, bool keep = true);
    glm::vec3 center(const std::string& target) const;
    void attach(Behavior behavior);
    void move(const std::string& target, glm::vec3 delta);
    void rotate(const std::string& target, glm::vec3 axis, float degrees);
    void setRest(const std::string& target, const graphics::Transform3D& rest);
    graphics::EntityId meshEntity(const std::string& target) const;
    void requireGeometryEditable(graphics::EntityId mesh) const;
    void requireFaceEditable(graphics::EntityId mesh, std::size_t face) const;
    void model(const std::string& name, const graphics::Model& model, glm::vec3 position = glm::vec3(0));
    std::string mount(const std::string& parent, const std::string& localName, FaceMount attachment);
    void bind(Binding binding);
    bool execute(const std::string& line, std::ostream& out);
    bool update(float dt, std::ostream& out);
    bool render();
    void onKey(int key, int action, int mods, std::ostream& out);
    void onMouse(const graphics::MouseEvent& event, std::ostream& out);
    void onFocus(bool focused);
    void onCursor(double x, double y, std::ostream& out);
    void guide(const std::string& topic, std::ostream& out) const;
    void help(std::ostream& out) const;
    void inspect(const std::string& target, std::ostream& out) const;
};

Tokens tokenize(const std::string& line);
std::string quote(const std::string& text);
double number(const std::string& text);
std::size_t indexNumber(const std::string& text);
glm::vec3 vector(const Tokens& args, std::size_t offset);
glm::vec3 axis(const std::string& text);
bool boolean(const std::string& text);
void require(bool success, const std::string& message);
bool rigid(const glm::mat4& matrix);
std::optional<bool> customCommand(Assembly& scene, const Tokens& args, std::ostream& out);
bool defaultRecipe(Assembly& scene, std::ostream& out);
} // namespace assembly
```

<a id="file-9"></a>
## standard/modes/AssemblyCommands.cpp

변경 이유: 조회·계층·형상·동작·입력 명령을 한 진입점에서 분배한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include <algorithm>

namespace assembly {
bool Assembly::objectCommand(const Tokens& a, std::ostream&) {
    const auto& command = a[0];
    if (command == "add") {
        require(a.size() == 6 || a.size() == 9, "add <종류> <이름> <X> <Y> <Z> [크기X 크기Y 크기Z]");
        add(a[1], a[2], vector(a, 3), a.size() == 9 ? vector(a, 6) : glm::vec3(1));
        return true;
    }
    if (command == "parent") {
        require(a.size() == 3, "parent <자식> <부모 또는 none>. 좌표는 부모 로컬 값으로 유지합니다.");
        auto& child = node(a[1]);
        require(!child.mount, "면 부착 관절은 unmount 후 부모를 바꾸세요.");
        auto* driver = positionDriver(a[1]);
        require(!driver || (driver->effect != Effect::Body && driver->effect != Effect::Walk), "물리 루트에는 부모를 추가할 수 없습니다.");
        for (const auto& behavior : state_.behaviors) {
            if (behavior.effect != Effect::Body && behavior.effect != Effect::Walk) continue;
            const auto& room = node(behavior.reference);
            require((!room.mesh || child.id != *room.mesh) && a[1] != behavior.target + ".mesh",
                    "물리 메시/방 walls의 부모를 바꿀 수 없습니다.");
        }
        const auto parent = a[2] == "none" ? std::optional<graphics::EntityId>{} : std::optional<graphics::EntityId>{node(a[2]).id};
        require(app_.getScene().setParent(child.id, parent), "부모 연결 실패. 순환 여부를 확인하세요.");
        return true;
    }
    if (command == "color" || command == "show") {
        require(command == "color" ? a.size() == 5 : a.size() == 3, "color <대상> <R> <G> <B> 또는 show <대상> <on/off>");
        const auto root = node(a[1]).id;
        const auto color = command == "color" ? vector(a, 2) : glm::vec3(1);
        require(glm::all(glm::greaterThanEqual(color, glm::vec3(0))) && glm::all(glm::lessThanEqual(color, glm::vec3(1))), "색은 0~1입니다.");
        const bool visible = command == "show" ? boolean(a[2]) : true;
        auto& scene = app_.getScene();
        for (auto id : scene.getIds()) {
            std::optional<graphics::EntityId> ancestor = id;
            while (ancestor && *ancestor != root)
                ancestor = scene.getComponent(*ancestor)->parent;
            if (ancestor)
                require(command == "color" ? scene.setTint(id, {color.x, color.y, color.z}) : scene.setVisible(id, visible), "색/가시성 적용 실패.");
        }
        return true;
    }
    if (command == "move") {
        require(a.size() == 5, "move <대상> <DX> <DY> <DZ>");
        move(a[1], vector(a, 2));
        return true;
    }
    if (command == "rotate") {
        require(a.size() == 4, "rotate <대상> <축> <도>");
        rotate(a[1], axis(a[2]), float(number(a[3])));
        return true;
    }
    if (command == "scale" || command == "pivot" || command == "position" || command == "rest" || command == "rest3") {
        require(a.size() >= 3, command + " <대상> <값>");
        auto rest = node(a[1]).rest;
        if (command == "scale") {
            require(a.size() == 3, "scale <대상> <양수 균일 배율>");
            rest.scale = glm::vec3(float(number(a[2])));
        } else if (command == "rest" || command == "rest3") {
            require(command == "rest" ? a.size() == 18 : a.size() == 20, "rest <대상> <위치3> <회전행렬 열우선9> <균일배율> <피벗3>");
            rest.basis = vector(a, 2);
            for (int c = 0; c < 3; ++c)
                for (int r = 0; r < 3; ++r)
                    rest.rotation[c][r] = float(number(a[5 + c * 3 + r]));
            rest.scale = command == "rest3" ? vector(a, 14) : glm::vec3(float(number(a[14])));
            rest.localBasis = vector(a, command == "rest3" ? 17 : 15);
        } else {
            require(a.size() == 5, command + " <대상> <X> <Y> <Z>");
            if (command == "pivot") rest.localBasis = vector(a, 2);
            else rest.basis = vector(a, 2);
        }
        setRest(a[1], rest);
        return true;
    }
    return false;
}

bool Assembly::dispatch(const Tokens& a, std::ostream& out) {
    if (a[0] == "help") {
        require(a.size() <= 2, "help [기능]");
        if (a.size() == 2) guide(a[1], out);
        else help(out);
        return true;
    }
    if (a[0] == "guide") {
        require(a.size() == 2, "guide <기능 또는 객체 이름>");
        guide(a[1], out);
        return true;
    }
    if (a[0] == "list") {
        require(a.size() == 1, "list");
        for (const auto& item : state_.nodes) inspect(item.first, out);
        for (const auto& binding : state_.bindings)
            out << "binding key=" << binding.key << " mods=" << binding.mods << (binding.hold ? " hold " : " press ") << binding.command << '\n';
        return true;
    }
    if (a[0] == "inspect" || a[0] == "select") {
        require(a.size() == 2, a[0] + " <객체/부품 이름>");
        inspect(a[1], out);
        if (a[0] == "select") state_.selected = a[1];
        return true;
    }
    if (a[0] == "clear") {
        require(a.size() == 1, "clear");
        clear();
        return true;
    }
    if (a[0] == "save" || a[0] == "load") {
        require(a.size() == 2 && !replaying_, "save/load <파일>. 레시피 안에서 파일 명령을 중첩할 수 없습니다.");
        return a[0] == "save" ? save(a[1], out) : load(a[1], out);
    }
    if (surfaceCommand(a, out) || trackCommand(a, out) || hierarchyCommand(a, out) || objectCommand(a, out) || motionCommand(a, out) || inputCommand(a, out))
        return true;
    if (const auto handled = customCommand(*this, a, out))
        return *handled;
    return false;
}
} // namespace assembly
```

<a id="file-10"></a>
## standard/modes/AssemblyConsole.cpp

변경 이유: Windows 콘솔/파이프의 준비된 입력만 읽고 콘솔 모드를 복원한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "AssemblyConsole.h"
#include <algorithm>
#include <iostream>

namespace assembly {
ConsoleInput::ConsoleInput() {
    input_ = GetStdHandle(STD_INPUT_HANDLE);
    output_ = GetStdHandle(STD_OUTPUT_HANDLE);
    if (input_ != INVALID_HANDLE_VALUE && input_ && GetConsoleMode(input_, &originalMode_)) {
        const DWORD mode = (originalMode_ | ENABLE_EXTENDED_FLAGS) &
            ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_QUICK_EDIT_MODE);
        console_ = SetConsoleMode(input_, mode) != 0;
    }
}

ConsoleInput::~ConsoleInput() {
    if (console_) {
        SetConsoleMode(input_, originalMode_);
        std::cout << '\n';
    }
}

void ConsoleInput::prompt() {
    if (!console_) return;
    std::cout << "asm> " << std::flush;
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(output_, &info)) origin_ = info.dwCursorPosition;
    echoed_ = 0;
}

void ConsoleInput::redraw() {
    DWORD written = 0;
    if (!SetConsoleCursorPosition(output_, origin_)) return;
    FillConsoleOutputCharacterW(output_, L' ', echoed_, origin_, &written);
    WriteConsoleW(output_, line_.data(), DWORD(line_.size()), &written, nullptr);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(output_, &info)) {
        const int count = (info.dwCursorPosition.Y - origin_.Y) * info.dwSize.X + info.dwCursorPosition.X - origin_.X;
        echoed_ = DWORD(std::max(0, count));
    }
}

std::optional<std::string> ConsoleInput::poll() {
    if (console_) {
        for (unsigned processed = 0; processed < 128; ++processed) {
            DWORD available = 0;
            if (!GetNumberOfConsoleInputEvents(input_, &available) || available == 0) return {};
            INPUT_RECORD record{};
            DWORD read = 0;
            if (!ReadConsoleInputW(input_, &record, 1, &read) || read == 0) return {};
            if (record.EventType != KEY_EVENT || !record.Event.KeyEvent.bKeyDown) continue;
            const auto& key = record.Event.KeyEvent;
            if (key.wVirtualKeyCode == VK_RETURN) {
                std::cout << '\n';
                const int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, line_.data(), int(line_.size()), nullptr, 0, nullptr, nullptr);
                std::string command(std::max(0, bytes), '\0');
                if (bytes > 0) WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, line_.data(), int(line_.size()), command.data(), bytes, nullptr, nullptr);
                line_.clear();
                return command;
            }
            if (key.wVirtualKeyCode == VK_ESCAPE) line_.clear();
            else if (key.wVirtualKeyCode == VK_BACK && !line_.empty()) {
                const auto last = line_.back();
                line_.pop_back();
                if (last >= 0xDC00 && last <= 0xDFFF && !line_.empty() && line_.back() >= 0xD800 && line_.back() <= 0xDBFF)
                    line_.pop_back();
            } else if (key.uChar.UnicodeChar >= 32 && line_.size() < 2048) {
                for (WORD repeat = 0; repeat < key.wRepeatCount && line_.size() < 2048; ++repeat)
                    line_ += key.uChar.UnicodeChar;
            }
            redraw();
        }
        return {};
    }
    const auto newline = redirected_.find('\n');
    if (newline != std::string::npos) {
        auto command = redirected_.substr(0, newline);
        redirected_.erase(0, newline + 1);
        if (!command.empty() && command.back() == '\r') command.pop_back();
        if (command.compare(0, 3, "\xEF\xBB\xBF") == 0) command.erase(0, 3);
        return command;
    }
    if (eof_) {
        if (redirected_.empty()) return {};
        auto command = std::move(redirected_);
        redirected_.clear();
        return command;
    }
    if (!input_ || input_ == INVALID_HANDLE_VALUE) { eof_ = true; return {}; }
    const DWORD type = GetFileType(input_);
    DWORD available = 0;
    if (type == FILE_TYPE_PIPE) {
        if (!PeekNamedPipe(input_, nullptr, 0, nullptr, &available, nullptr)) { eof_ = true; return {}; }
        if (available == 0) return {};
    } else if (type != FILE_TYPE_DISK) return {};
    char buffer[512];
    DWORD read = 0;
    const DWORD count = type == FILE_TYPE_PIPE ? std::min<DWORD>(available, DWORD(sizeof(buffer))) : DWORD(sizeof(buffer));
    if (!ReadFile(input_, buffer, count, &read, nullptr) || read == 0) { eof_ = true; return {}; }
    redirected_.append(buffer, read);
    if (redirected_.size() > 65536) {
        redirected_.clear();
        std::cout << "표준 입력 행이 너무 깁니다. 4096바이트 이하 명령을 사용하세요.\n";
    }
    return {};
}
} // namespace assembly
```

<a id="file-11"></a>
## standard/modes/AssemblyConsole.h

변경 이유: 콘솔 핸들과 입력 버퍼의 수명을 모드에 묶는다.

### before

없음. 새 파일이다.

### after

```cpp
#pragma once
#include "PracticeMode.h"

namespace assembly {
class ConsoleInput {
    HANDLE input_ = INVALID_HANDLE_VALUE, output_ = INVALID_HANDLE_VALUE;
    DWORD originalMode_ = 0;
    bool console_ = false, eof_ = false;
    std::wstring line_;
    std::string redirected_;
    COORD origin_{};
    DWORD echoed_ = 0;
    void redraw();
public:
    ConsoleInput();
    ~ConsoleInput();
    ConsoleInput(const ConsoleInput&) = delete;
    ConsoleInput& operator=(const ConsoleInput&) = delete;
    std::optional<std::string> poll();
    void prompt();
};
} // namespace assembly
```

<a id="file-12"></a>
## standard/modes/AssemblyExamples.cpp

변경 이유: 육면체와 관절로 탱크·트럭·사람을 직접 조립한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "AssemblyExamples.h"

namespace assembly {
namespace {
void pendulum(Assembly& scene, const std::string& id, const std::string& target, glm::vec3 axis,
              double amplitude, double period, double offset = 0, double phase = 0) {
    graphics::PendulumMotion config;
    config.axis.direction = axis; config.amplitudeDegrees = amplitude;
    config.periodSeconds = period; config.offsetDegrees = offset; config.phaseDegrees = phase;
    Behavior behavior;
    behavior.name = id; behavior.target = target;
    require(behavior.motion.append(graphics::Motion::pendulum(config)), "예제 진자 생성 실패.");
    scene.attach(std::move(behavior));
}
}

void buildTankExample(Assembly& scene, const std::string& name) {
    scene.group(name, {0, 0, 0});
    scene.node(name).guideTopic = "tank";
    scene.part("box", name, "body", {0, .45f, 0}, {2.2f, .5f, 2.6f});
    for (int i = 0; i < 2; ++i)
        scene.part("box", name, i == 0 ? "leftTrack" : "rightTrack",
            {i == 0 ? -1.05f : 1.05f, .3f, 0}, {.45f, .6f, 3});
    const auto turret = scene.part("joint", name, "turret", {0, .8f, 0});
    scene.part("box", turret, "body", {0, .25f, 0}, {1.4f, .5f, 1.5f});
    const auto gun = scene.part("joint", turret, "gun", {0, .3f, .65f});
    scene.part("box", gun, "barrel", {0, 0, 1}, {.2f, .2f, 2});
    scene.part("box", gun, "muzzle", {0, 0, 2}, {.35f, .35f, .3f});
    scene.pivot(name, {0, .6f, 0});
    graphics::RotationMotion rotation;
    rotation.degreesPerSecond = 25;
    Behavior motor;
    motor.name = name + ".turretMotor"; motor.target = turret;
    require(motor.motion.append(graphics::Motion::rotation(rotation)), "포탑 회전 생성 실패.");
    scene.attach(std::move(motor));
    pendulum(scene, name + ".gunMotor", gun, {1, 0, 0}, 10, 4);
}

void buildTruckExample(Assembly& scene, const std::string& name) {
    scene.group(name, {0, 0, 0});
    scene.node(name).guideTopic = "truck";
    scene.part("box", name, "chassis", {0, .6f, 0}, {1.8f, .3f, 3.2f});
    scene.part("box", name, "cabin", {0, 1.2f, 1.1f}, {1.8f, 1.2f, 1.2f});
    for (int i = 0; i < 4; ++i)
        scene.part("box", name, "wheel" + std::to_string(i + 1),
            {i % 2 == 0 ? -.95f : .95f, .3f, i < 2 ? -1.05f : 1.05f}, {.4f, .6f, .6f});
    const auto cargo = scene.part("joint", name, "container", {0, .85f, -.65f});
    scene.part("box", cargo, "floor", {0, 0, 0}, {1.8f, .15f, 1.9f});
    scene.part("box", cargo, "roof", {0, 1.25f, 0}, {1.8f, .15f, 1.9f});
    scene.part("box", cargo, "leftWall", {-.85f, .625f, 0}, {.1f, 1.2f, 1.9f});
    scene.part("box", cargo, "rightWall", {.85f, .625f, 0}, {.1f, 1.2f, 1.9f});
    scene.part("box", cargo, "frontWall", {0, .625f, .9f}, {1.8f, 1.2f, .1f});
    const auto left = scene.part("joint", cargo, "leftDoor", {-.85f, 0, -.95f});
    const auto right = scene.part("joint", cargo, "rightDoor", {.85f, 0, -.95f});
    scene.part("box", left, "panel", {.425f, .625f, 0}, {.85f, 1.2f, .1f});
    scene.part("box", right, "panel", {-.425f, .625f, 0}, {.85f, 1.2f, .1f});
    scene.pivot(name, {0, .8f, 0});
    pendulum(scene, name + ".leftDoorMotor", left, {0, 1, 0}, 40, 6, 40, -90);
    pendulum(scene, name + ".rightDoorMotor", right, {0, 1, 0}, 40, 6, -40, 90);
}

void buildPersonExample(Assembly& scene, const std::string& name) {
    scene.group(name, {0, 0, 0});
    scene.node(name).guideTopic = "person";
    const auto pelvis = scene.part("joint", name, "pelvis", {0, .8f, 0});
    scene.part("box", pelvis, "body", {0, 0, 0}, {.4f, .25f, .3f});
    const auto torso = scene.part("joint", pelvis, "torso", {0, .35f, 0});
    scene.part("box", torso, "body", {0, 0, 0}, {.55f, .6f, .3f});
    const auto neck = scene.part("joint", torso, "neck", {0, .35f, 0});
    scene.part("box", neck, "head", {0, .18f, 0}, {.35f, .35f, .35f});
    scene.part("box", neck, "nose", {0, .18f, .21f}, {.1f, .08f, .12f});
    for (int i = 0; i < 2; ++i) {
        const std::string side = i == 0 ? "left" : "right";
        const float sign = i == 0 ? -1.0f : 1.0f;
        const auto shoulder = scene.part("joint", torso, side + "Shoulder", {sign * .38f, .17f, 0});
        scene.part("box", shoulder, "upperArm", {0, -.2f, 0}, {.14f, .4f, .16f});
        const auto elbow = scene.part("joint", shoulder, "elbow", {0, -.4f, 0});
        scene.part("box", elbow, "forearm", {0, -.18f, 0}, {.13f, .36f, .15f});
        const auto hip = scene.part("joint", pelvis, side + "Hip", {sign * .13f, 0, 0});
        scene.part("box", hip, "thigh", {0, -.2f, 0}, {.17f, .4f, .2f});
        const auto knee = scene.part("joint", hip, "knee", {0, -.4f, 0});
        scene.part("box", knee, "shin", {0, -.2f, 0}, {.17f, .4f, .2f});
        scene.part("box", knee, "foot", {0, -.4f, .08f}, {.2f, .1f, .35f});
        pendulum(scene, name + "." + side + "ArmMotor", shoulder, {1, 0, 0}, 25, 2, 0, i * 180);
        pendulum(scene, name + "." + side + "LegMotor", hip, {1, 0, 0}, 25, 2, 0, (1 - i) * 180);
    }
    scene.pivot(name, {0, .9f, 0});
}
}
```

<a id="file-13"></a>
## standard/modes/AssemblyExamples.h

변경 이유: 직접 편집할 예제 C++ 함수의 진입점을 선언한다.

### before

없음. 새 파일이다.

### after

```cpp
#pragma once
#include "Assembly.h"

namespace assembly {
void buildTankExample(Assembly& scene, const std::string& name);
void buildTruckExample(Assembly& scene, const std::string& name);
void buildPersonExample(Assembly& scene, const std::string& name);
}
```

<a id="file-14"></a>
## standard/modes/AssemblyExtensions.cpp

변경 이유: 사용자 정의 명령과 C++ 조립 함수를 연결한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"

#include "AssemblyExamples.h"

namespace assembly {
std::optional<bool> customCommand(Assembly& scene, const Tokens& args, std::ostream&) {
    // USER_COMMAND_BEGIN
    if (args[0] == "build") {
        require(args.size() == 3, "build <tank/truck/person> <이름>");
        if (args[1] == "tank") buildTankExample(scene, args[2]);
        else if (args[1] == "truck") buildTruckExample(scene, args[2]);
        else if (args[1] == "person") buildPersonExample(scene, args[2]);
        else require(false, "예제는 tank/truck/person입니다. guide patterns");
        return true;
    }
    if (args[0] == "nudge") {
        require(args.size() == 4, "nudge <대상> <축> <거리>");
        scene.move(args[1], axis(args[2]) * float(number(args[3])));
        return true;
    }
    // USER_COMMAND_END
    return std::nullopt;
}
} // namespace assembly
```

<a id="file-15"></a>
## standard/modes/AssemblyGuide.cpp

변경 이유: 실제 소스 표식에서 현재 함수·줄 번호를 찾고 기능별 편집 안내를 출력한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include <fstream>

namespace assembly {
namespace {
struct Location { const char* file; const char* marker; const char* edit; };

std::filesystem::path projectRoot() {
    std::wstring binary(32768, L'\0');
    const DWORD count = GetModuleFileNameW(nullptr, binary.data(), DWORD(binary.size()));
    if (count > 0 && count < binary.size()) {
        binary.resize(count);
        auto path = std::filesystem::path(binary).parent_path();
        for (int i = 0; i < 6 && !path.empty(); ++i, path = path.parent_path())
            if (std::filesystem::is_regular_file(path / "modes/Assembly.h")) return path;
    }
    const auto source = std::filesystem::u8path(__FILE__).parent_path().parent_path();
    if (std::filesystem::is_regular_file(source / "modes/Assembly.h")) return source;
    return {};
}

void show(const Location& location, std::ostream& out) {
    const auto root = projectRoot();
    const auto path = root.empty() ? std::filesystem::u8path(location.file) : root / location.file;
    out << "문서: " << (root.empty() ? std::filesystem::path("docs/assembly/CODE_GUIDE.md") : root / "docs/assembly/CODE_GUIDE.md").u8string() << '\n';
    std::ifstream source(path, std::ios::binary);
    std::string line;
    std::size_t number = 0;
    bool found = false;
    while (std::getline(source, line)) {
        ++number;
        if (line.find(location.marker) != std::string::npos) {
            out << path.u8string() << ':' << number << "\n  " << line << '\n';
            found = true;
            break;
        }
    }
    if (!found) out << path.u8string() << " : 소스/표식을 찾지 못해 현재 줄 번호를 확인할 수 없습니다.\n";
    out << "응용 조합: docs/assembly/PATTERN_COMBINATIONS.md\n";
    out << "수정 항목: " << location.edit << "\n레시피는 다시 load하고, C++ 수정 후 빌드·실행은 사용자가 수행하세요.\n";
}
}

void Assembly::help(std::ostream& out) const {
    out << "0번 조립 콘솔. 명세: docs/assembly/SPEC.md, 사용법: docs/assembly/USER_GUIDE.md\n"
        << "build tank/truck/person <이름> : C++ 예제 생성. 확장은 guide tank/truck/person으로 직접 편집\n"
        << "surface <객체> <면/all> | mount <객체> <관절> 면 삼각형 U V 거리 | anchor <객체> <관절> 면 XYZ\n"
        << "normal <객체> <면/all> XYZ 또는 auto/flip | winding <객체> <면/all> flip | warp <group> SX SY SZ\n"
        << "face-track <id> <객체> 면 시간 지연 once/loop/pingpong move/scale/turn ... | guide face-track\n"
        << "group <이름> X Y Z | part box <부모> <부품> 위치3 크기3 | joint <부모> <관절> 위치3\n"
        << "srt <그룹> 위치3 회전도3 균일배율 | pivot <그룹> center 또는 위치3 [keep/raw]\n"
        << "add <종류> <이름> X Y Z [크기3] | list | inspect <이름> | select <이름>\n"
        << "move/position <이름> X Y Z | rotate <이름> x/y/z 도 | scale <이름> 값 | parent <자식> <부모/none>\n"
        << "spin <id> <대상> 축 도/초 | path <id> <대상> 1~6 초 | pendulum <id> <대상> 축 진폭 초\n"
        << "linear <id> <대상> DX DY DZ 초 [once/loop/pingpong] | orbit <id> <대상> 축 도/초 중심3\n"
        << "face <id> <대상> 면 spin/orbit 축 도/초 또는 face <id> <대상> 면 hinge 모서리 도/초 | wall <id> <방> 면0~3 초\n"
        << "physics <id> <상자> <방> | walk <id> <walker> <방> 속도 | jump <보행id> 속도\n"
        << "bind RIGHT hold \"move hero 1 0 0\" | bind J press \"jump walking 5.5\" | unbind RIGHT\n"
        << "play/enable <id> on/off | seek <id> 초 | remove <id> | pause on/off | view 1/3\n"
        << "camera ... | render wire/lighting/depth on/off | save/load \"파일.assembly\" | clear\n"
        << "guide mount/normal/warp/face-track/cpp-effect/practice13~23 또는 guide <객체이름>, guide feature:<기능>\n"
        << "창: 클릭 선택, Tab/Backspace 부품, XYZ 축, 방향키 편집, Space 정지, Home 기본 레시피 복원\n";
}

void Assembly::guide(const std::string& requested, std::ostream& out) const {
    const bool feature = requested.compare(0, 8, "feature:") == 0;
    std::string topic = feature ? requested.substr(8) : requested;
    if (const auto found = state_.nodes.find(topic); !feature && found != state_.nodes.end()) {
        inspect(topic, out);
        std::optional<graphics::EntityId> id = found->second.id;
        topic = "objects";
        while (id) {
            bool matched = false;
            for (const auto& item : state_.nodes) {
                if (item.second.id == *id && item.second.whole) {
                    const auto& kind = item.second.kind;
                    if (!item.second.guideTopic.empty()) { topic = item.second.guideTopic; matched = true; break; }
                    if (kind == "humanoid" || kind == "swing" || kind == "tank" || kind == "walker" || kind == "grid" || kind == "room" || kind == "group" || kind == "joint")
                        topic = kind;
                    matched = true;
                    break;
                }
            }
            if (matched) break;
            const auto* component = app_.getScene().getComponent(*id);
            id = component ? component->parent : std::nullopt;
        }
    }
    const std::map<std::string, Location> locations{
        {"mount", {"modes/AssemblySurface.cpp", "std::string Assembly::mount(", "객체·면·삼각형 좌표 관절 부착과 프레임 추적"}},
        {"normal", {"modes/AssemblySurface.cpp", "if (command == \"normal\")", "조명 노멀·winding·면별 정점 데이터"}},
        {"geometry", {"modes/AssemblySurface.cpp", "void Assembly::model(", "사용자 Model 등록, 삼각형·정점·면 변환"}},
        {"warp", {"modes/AssemblySurface.cpp", "if (command == \"warp\")", "외곽 group의 비균일 배율과 반사"}},
        {"face-track", {"modes/AssemblyTracks.cpp", "bool Assembly::trackCommand(", "지연·반복·왕복 면 이동/축소/임의 축 회전"}},
        {"cpp-effect", {"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Custom)", "사용자 C++ 프레임 동작과 기준 SRT 합성"}},
        {"objects", {"modes/AssemblyObjects.cpp", "void Assembly::add(", "객체 종류 분기와 크기·색·부품 등록"}},
        {"humanoid", {"framework/detail/Animation.cpp", "std::optional<HumanoidRig> AnimationSystem::addHumanoid", "사람 비율·관절 연결점·머리/팔/다리 생성"}},
        {"swing", {"framework/detail/Animation.cpp", "std::optional<SwingRig> AnimationSystem::addSwingWithRider", "그네 지지대·줄·좌석·탑승자 연결"}},
        {"tank", {"modes/AssemblyExamples.cpp", "void buildTankExample(", "육면체 기반 차체·궤도·포탑·포신의 계층과 동작"}},
        {"tank-preset", {"modes/AssemblyParts.cpp", "TankParts addTankParts(", "기존 실습19·22와 공유하는 탱크 부품"}},
        {"walker", {"framework/detail/Patterns.cpp", "std::optional<JointRig> PatternSystem::addWalkingRig", "보행 리그의 몸체 비율·관절 연결점"}},
        {"grid", {"modes/AssemblyParts.cpp", "std::vector<graphics::EntityId> addGridParts(", "셀 배치·크기 비율·교대 색"}},
        {"room", {"modes/AssemblyObjects.cpp", "} else if (kind == \"room\")", "방 경계와 벽 메시 생성"}},
        {"hierarchy", {"modes/AssemblyHierarchy.cpp", "void Assembly::group(", "group/part/joint 조립 API와 최상위 변환"}},
        {"srt", {"modes/AssemblyHierarchy.cpp", "void Assembly::srt(", "최상위 위치·회전·균일배율을 한 번에 설정"}},
        {"pivot", {"modes/AssemblyHierarchy.cpp", "void Assembly::pivot(", "중심점 변경과 기준 배치 보존"}},
        {"center", {"modes/AssemblyHierarchy.cpp", "glm::vec3 Assembly::center(", "하위 메시 전체의 로컬 AABB 중심 계산"}},
        {"patterns", {"docs/assembly/PATTERN_COMBINATIONS.md", "# 기능 조합 패턴", "탱크·트럭·사람의 계층 조립과 C++ 수정 위치"}},
        {"truck", {"modes/AssemblyExamples.cpp", "void buildTruckExample(", "차체·바퀴·컨테이너·문 관절 조합"}},
        {"person", {"modes/AssemblyExamples.cpp", "void buildPersonExample(", "육면체·관절로 몸체·팔·다리 조합"}},
        {"motion", {"modes/AssemblyMotion.cpp", "bool Assembly::motionCommand(", "경로·회전·진자·면 동작 설정"}},
        {"face", {"modes/AssemblyMotion.cpp", "} else if (command == \"face\"", "힌지·자전·공전 종류, 면/모서리, 속도"}},
        {"physics", {"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Body)", "중력·충돌 경계·이탈/보행 계산"}},
        {"input", {"modes/AssemblyInput.cpp", "bool Assembly::inputCommand(", "키 이름·수정키·press/hold 문법과 동작"}},
        {"camera", {"modes/AssemblyInput.cpp", "if (a[1] == \"pose\")", "카메라 이동·투영·시야각과 줌"}},
        {"recipe", {"modes/Mode0Recipe.cpp", "USER_RECIPE_BEGIN", "기본 조립 명령 문자열을 추가·수정"}},
        {"custom", {"modes/AssemblyExtensions.cpp", "USER_COMMAND_BEGIN", "새 명령 분기와 typed Assembly API 호출"}},
        {"runtime", {"modes/AssemblyRuntime.cpp", "bool Assembly::applyPoses(", "프레임별 합성 순서와 기능 업데이트"}},
        {"save", {"modes/AssemblyPersistence.cpp", "bool Assembly::save(", "레시피 기록과 기준 SRT 저장 형식"}}};
    if (topic == "anchor" || topic == "unmount") topic = "mount";
    if (topic == "winding" || topic == "orient") topic = "normal";
    if (topic == "triangle" || topic == "triangle-add" || topic == "surface" || topic == "vertex" || topic == "face-srt" || topic == "face-mode" || topic == "facecolor" || topic == "outline" || topic == "line" || topic == "digit") topic = "geometry";
    if (topic == "effects" || topic == "render") topic = "input";
    if (topic.size() == 10 && topic.compare(0, 8, "practice") == 0) {
        const std::map<std::string, Location> practices{
            {"practice13", {"modes/Mode10Practice13.cpp", "bool Practice13::initScene()", "면 선택·무작위 선택 상태를 customCommand/Custom 동작으로 옮기기"}},
            {"practice14", {"modes/Mode11Practice14.cpp", "void Practice14::onUpdate(", "입력 상태·형상 교체·로컬 회전"}},
            {"practice15", {"modes/FaceCommon.cpp", "void FaceMode::applyFaceMotion()", "한 면의 복합 행렬을 하나의 FaceTrack Motion에 조립"}},
            {"practice16", {"modes/Mode13Practice16.cpp", "void Practice16::applyPyramid(", "순차 펼침과 다중 면 동기화"}},
            {"practice17", {"modes/Mode14Practice17.cpp", "void Practice17::onUpdate(", "교환·우회·사인 배율·조건 전이"}},
            {"practice18", {"modes/Mode15Practice18.cpp", "void Practice18::onUpdate(", "기울어진 궤도·달 중심·축과 반지름"}},
            {"practice19", {"modes/TankCommon.cpp", "void TankMode::onUpdate(", "탱크 부품·이동 상태·카메라 추적"}},
            {"practice20", {"modes/Mode17Practice20.cpp", "void Practice20::onUpdate(", "거리 기반 컨베이어·택배 간격·접선 자세"}},
            {"practice21", {"modes/Mode18Practice21.cpp", "void Practice21::onUpdate(", "드래그 방 회전·다중 상자/공 이탈 상태"}},
            {"practice22", {"modes/TankCommon.cpp", "bool TankMode::onRender()", "동일 상태의 3뷰 렌더와 카메라"}},
            {"practice23", {"modes/Mode20Practice23.cpp", "void Practice23::onUpdate(", "보행 충돌·장애물 난수 배치·벽 개방 상태"}}};
        if (const auto found = practices.find(topic); found != practices.end()) { show(found->second, out); return; }
    }
    if (topic == "truck-pattern") topic = "truck";
    if (topic == "tank-pattern") topic = "tank";
    if (topic == "person-pattern") topic = "person";
    if (topic == "group" || topic == "joint" || topic == "part") topic = "hierarchy";
    if (topic == "add" || topic == "parent" || topic == "color" || topic == "show") topic = "objects";
    if (topic == "spin" || topic == "orbit" || topic == "path" || topic == "linear" || topic == "pendulum" || topic == "remove") topic = "motion";
    if (topic == "wall") topic = "face";
    if (topic == "walk" || topic == "jump" || topic == "obstacle" || topic == "body" || topic == "release") topic = "physics";
    if (topic == "bind" || topic == "unbind" || topic == "drag") topic = "input";
    if (topic == "load") topic = "save";
    const auto found = locations.find(topic);
    if (found == locations.end()) {
        out << "지원 항목:";
        for (const auto& item : locations) out << ' ' << item.first;
        out << " practice13~23\n";
        return;
    }
    show(found->second, out);
}
} // namespace assembly
```

<a id="file-16"></a>
## standard/modes/AssemblyHierarchy.cpp

변경 이유: 부품의 부모 연결, 전체 SRT·피벗·중심 계산을 공유한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include <limits>

namespace assembly {
void Assembly::group(const std::string& name, glm::vec3 position) {
    add("joint", name, position);
    node(name).kind = "group";
}

std::string Assembly::part(const std::string& kind, const std::string& parentName, const std::string& localName,
                           glm::vec3 position, glm::vec3 size) {
    require(!localName.empty() && localName.find('.') == std::string::npos, "부품 이름은 점 없는 한 구간입니다.");
    const auto parent = node(parentName).id;
    const auto name = parentName + "." + localName;
    add(kind, name, position, size);
    require(app_.getScene().setParent(node(name).id, parent), "부품 부모 연결 실패.");
    node(name).whole = false;
    return name;
}

void Assembly::srt(const std::string& name, glm::vec3 position, glm::vec3 degrees, float scale) {
    auto rest = node(name).rest;
    const auto angles = glm::radians(degrees);
    rest.basis = position;
    rest.rotation = glm::mat3(glm::rotate(glm::mat4(1), angles.z, glm::vec3(0, 0, 1)) *
        glm::rotate(glm::mat4(1), angles.y, glm::vec3(0, 1, 0)) *
        glm::rotate(glm::mat4(1), angles.x, glm::vec3(1, 0, 0)));
    rest.scale = glm::vec3(scale);
    setRest(name, rest);
}

void Assembly::pivot(const std::string& name, glm::vec3 point, bool keep) {
    auto rest = node(name).rest;
    if (keep) rest.basis += rest.rotation * (rest.scale * (point - rest.localBasis));
    rest.localBasis = point;
    setRest(name, rest);
}

glm::vec3 Assembly::center(const std::string& name) const {
    const auto& target = node(name);
    auto& scene = app_.getScene();
    const auto world = scene.worldMatrix(target.id);
    require(world.has_value(), "그룹 행렬이 없습니다.");
    const auto inverse = glm::inverse(*world);
    glm::vec3 low(std::numeric_limits<float>::max()), high(std::numeric_limits<float>::lowest());
    bool geometry = false;
    for (auto id : scene.getIds()) {
        std::optional<graphics::EntityId> parent = id;
        while (parent && *parent != target.id) parent = scene.getComponent(*parent)->parent;
        if (!parent) continue;
        const auto* component = scene.getComponent(id);
        for (std::size_t f = 0; f < component->polyhedron.faces.size(); ++f) {
            const auto matrix = scene.faceWorldMatrix(id, f);
            require(matrix.has_value(), "부품 행렬이 없습니다.");
            for (const auto& vertex : component->polyhedron.faces[f].localVertices) {
                const auto point = glm::vec3(inverse * *matrix * glm::vec4(vertex, 1));
                low = glm::min(low, point); high = glm::max(high, point); geometry = true;
            }
        }
    }
    require(geometry, "중심을 계산할 메시 부품이 없습니다.");
    return (low + high) * .5f;
}

bool Assembly::hierarchyCommand(const Tokens& a, std::ostream&) {
    if (a[0] == "group") {
        require(a.size() == 5, "group <이름> <X> <Y> <Z>");
        group(a[1], vector(a, 2));
        return true;
    }
    if (a[0] == "part" || a[0] == "joint") {
        const bool joint = a[0] == "joint";
        require(joint ? a.size() == 6 : (a.size() == 7 || a.size() == 10),
                "part <종류> <부모> <부품이름> <위치3> [크기3] 또는 joint <부모> <관절이름> <위치3>");
        const std::size_t parentIndex = joint ? 1 : 2;
        part(joint ? "joint" : a[1], a[parentIndex], a[parentIndex + 1], vector(a, parentIndex + 2),
             !joint && a.size() == 10 ? vector(a, 7) : glm::vec3(1));
        return true;
    }
    if (a[0] == "srt") {
        require(a.size() == 9, "srt <대상> <위치3> <X/Y/Z 회전 도3> <균일배율>");
        srt(a[1], vector(a, 2), vector(a, 5), float(number(a[8])));
        return true;
    }
    if (a[0] != "pivot" && a[0] != "center") return false;
    const bool automatic = a[0] == "center" || (a.size() >= 3 && a[2] == "center");
    require(automatic ? (a[0] == "center" ? (a.size() == 2 || a.size() == 3) : (a.size() == 3 || a.size() == 4)) :
        (a.size() == 5 || a.size() == 6), "pivot <대상> <중심3 또는 center> [keep/raw] 또는 center <대상> [keep/raw]");
    const std::size_t option = a[0] == "center" ? 2 : automatic ? 3 : 5;
    const bool keep = a.size() <= option || a[option] == "keep";
    require(a.size() <= option || a[option] == "keep" || a[option] == "raw", "피벗 옵션은 keep 또는 raw입니다.");
    pivot(a[1], automatic ? center(a[1]) : vector(a, 2), keep);
    return true;
}
} // namespace assembly
```

<a id="file-17"></a>
## standard/modes/AssemblyInput.cpp

변경 이유: 키 바인딩·드래그·카메라·컬링·광원·의미상 선택을 처리한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include <iostream>

namespace assembly {
namespace {
std::pair<int, int> keyCode(std::string text) {
    std::transform(text.begin(), text.end(), text.begin(), [](unsigned char c) { return char(std::toupper(c)); });
    int mods = 0;
    for (;;) {
        const auto plus = text.find('+');
        if (plus == std::string::npos) break;
        const auto modifier = text.substr(0, plus);
        if (modifier == "CTRL") mods |= GLFW_MOD_CONTROL;
        else if (modifier == "SHIFT") mods |= GLFW_MOD_SHIFT;
        else if (modifier == "ALT") mods |= GLFW_MOD_ALT;
        else require(false, "수정키는 CTRL/SHIFT/ALT입니다.");
        text.erase(0, plus + 1);
    }
    const std::map<std::string, int> keys{{"LEFT", GLFW_KEY_LEFT}, {"RIGHT", GLFW_KEY_RIGHT},
        {"UP", GLFW_KEY_UP}, {"DOWN", GLFW_KEY_DOWN}, {"SPACE", GLFW_KEY_SPACE}, {"TAB", GLFW_KEY_TAB},
        {"BACKSPACE", GLFW_KEY_BACKSPACE}, {"ENTER", GLFW_KEY_ENTER}, {"PLUS", GLFW_KEY_EQUAL},
        {"MINUS", GLFW_KEY_MINUS}, {"PAGEUP", GLFW_KEY_PAGE_UP}, {"PAGEDOWN", GLFW_KEY_PAGE_DOWN}};
    int key = text.size() == 1 && text[0] >= 'A' && text[0] <= 'Z' ? int(text[0]) : 0;
    if (const auto found = keys.find(text); found != keys.end()) key = found->second;
    require(key != 0, "키는 영문자·방향키·SPACE·TAB·BACKSPACE·ENTER·PLUS·MINUS·PAGEUP·PAGEDOWN입니다.");
    return {key, mods};
}
}

void Assembly::bind(Binding binding) {
    require(binding.key > 0 && !binding.command.empty(), "키와 실행할 명령이 필요합니다.");
    for (const auto& current : state_.bindings)
        require(current.key != binding.key || current.mods != binding.mods, "중복 키입니다. unbind 후 다시 연결하세요.");
    binding.tokens = tokenize(binding.command);
    require(!binding.tokens.empty(), "빈 바인딩 명령입니다.");
    if (binding.hold) {
        require((binding.tokens[0] == "move" && binding.tokens.size() == 5) ||
                (binding.tokens[0] == "rotate" && binding.tokens.size() == 4), "hold는 move/rotate 속도 명령만 지원합니다.");
        node(binding.tokens[1]);
        if (binding.tokens[0] == "move") vector(binding.tokens, 2);
        else { axis(binding.tokens[2]); number(binding.tokens[3]); }
    }
    state_.bindings.push_back(std::move(binding));
}

bool Assembly::inputCommand(const Tokens& a, std::ostream&) {
    if (a[0] == "drag") {
        require((a.size() == 2 && a[1] == "off") || a.size() == 4, "drag <객체> <축> <도/픽셀> 또는 drag off");
        state_.dragging = false; state_.dragX.reset();
        if (a.size() == 2) state_.dragTarget.clear();
        else {
            node(a[1]); state_.dragTarget = a[1]; state_.dragAxis = axis(a[2]); state_.dragSensitivity = float(number(a[3]));
        }
        return true;
    }
    if (a[0] == "bind") {
        require(a.size() == 4 && (a[2] == "press" || a[2] == "hold"), "bind <키> <press/hold> \"명령\"");
        const auto key = keyCode(a[1]);
        Binding binding;
        binding.key = key.first; binding.mods = key.second; binding.hold = a[2] == "hold"; binding.command = a[3];
        bind(std::move(binding));
        return true;
    }
    if (a[0] == "unbind") {
        require(a.size() == 2, "unbind <키>");
        const auto key = keyCode(a[1]);
        const auto it = std::find_if(state_.bindings.begin(), state_.bindings.end(), [&](const Binding& b) {
            return b.key == key.first && b.mods == key.second;
        });
        require(it != state_.bindings.end(), "등록되지 않은 키입니다.");
        state_.bindings.erase(it);
        return true;
    }
    if (a[0] == "pause" || a[0] == "view") {
        require(a.size() == 2, "pause <on/off> 또는 view <1/3>");
        if (a[0] == "pause") state_.paused = boolean(a[1]);
        else {
            require(a[1] == "1" || a[1] == "3", "뷰는 1 또는 3입니다.");
            state_.threeViews = a[1] == "3";
        }
        return true;
    }
    if (a[0] == "render") {
        require(a.size() >= 3, "render <wire/lighting/depth/cull/light-direction/light-color/background> ...");
        auto& options = app_.getRenderOptions();
        if (a[1] == "cull") {
            require(a.size() == 3 && (a[2] == "none" || a[2] == "back" || a[2] == "front"), "render cull none/back/front");
            options.cull = a[2] == "none" ? graphics::CullMode::None : a[2] == "back" ? graphics::CullMode::Back : graphics::CullMode::Front;
            return true;
        }
        if (a[1] == "light-direction" || a[1] == "light-color" || a[1] == "background") {
            require(a.size() == 5, "render light-direction/light-color/background <값3>");
            const auto value = vector(a, 2);
            if (a[1] == "light-direction") {
                require(glm::length(value) > .0001f, "광원 방향은 0일 수 없습니다.");
                options.lightDirection = glm::normalize(value);
            } else {
                require(glm::all(glm::greaterThanEqual(value, glm::vec3(0))) && glm::all(glm::lessThanEqual(value, glm::vec3(1))), "색은 0~1입니다.");
                if (a[1] == "background") options.background = glm::vec4(value, 1);
                else options.lightColor = {value.x, value.y, value.z};
            }
            return true;
        }
        require(a.size() == 3, "render <wire/lighting/depth> <on/off>");
        const bool enabled = boolean(a[2]);
        if (a[1] == "wire") options.wireframe = enabled;
        else if (a[1] == "lighting") options.lighting = enabled;
        else if (a[1] == "depth") options.depthTest = enabled;
        else require(false, "렌더 설정은 wire/lighting/depth입니다.");
        return true;
    }
    if (a[0] != "camera") return false;
    require(a.size() >= 3, "camera <pose/move/yaw/orbit/zoom/projection/extent/fov/clip> ...");
    auto camera = app_.getCamera();
    if (a[1] == "pose") {
        require(a.size() == 11, "camera pose <위치3> <target3> <up3>");
        camera.position = vector(a, 2); camera.target = vector(a, 5); camera.up = vector(a, 8);
    } else if (a[1] == "move") {
        require(a.size() == 5, "camera move <DX> <DY> <DZ>");
        require(graphics::PatternSystem::moveCamera(camera, vector(a, 2)), "카메라 이동 실패.");
    } else if (a[1] == "yaw" || a[1] == "zoom") {
        require(a.size() == 3, "camera yaw <도> 또는 camera zoom <배율>");
        require(a[1] == "yaw" ? graphics::PatternSystem::yawCamera(camera, float(number(a[2]))) :
            graphics::PatternSystem::zoomCamera(camera, float(number(a[2]))), "카메라 회전/줌 실패.");
    } else if (a[1] == "orbit") {
        require(a.size() == 7, "camera orbit <축> <도> <CX> <CY> <CZ>");
        require(graphics::PatternSystem::orbitCamera(camera, {vector(a, 4), axis(a[2])}, float(number(a[3]))), "카메라 공전 실패.");
    } else if (a[1] == "projection") {
        require(a.size() == 3 && (a[2] == "ortho" || a[2] == "perspective"), "camera projection <ortho/perspective>");
        camera.orthographic = a[2] == "ortho";
    } else if (a[1] == "extent" || a[1] == "fov") {
        require(a.size() == 3, "camera extent <값> 또는 camera fov <도>");
        if (a[1] == "extent") camera.orthographicHalfExtent = float(number(a[2]));
        else camera.fieldOfViewDegrees = float(number(a[2]));
    } else if (a[1] == "clip") {
        require(a.size() == 4, "camera clip <near> <far>");
        camera.nearPlane = float(number(a[2])); camera.farPlane = float(number(a[3]));
    } else require(false, "지원하지 않는 카메라 명령입니다. guide camera");
    require(graphics::CameraSystem::validCamera(camera), "카메라의 위치·target·up·투영 범위를 확인하세요.");
    app_.getCamera() = camera;
    return true;
}

bool Assembly::dispatchInput(const std::function<bool(const Behavior&)>& callback, std::ostream& out) {
    try {
        // A callback may execute clear/load or remove itself.
        const auto snapshot = state_.behaviors;
        const auto revision = topologyRevision_;
        for (const auto& behavior : snapshot) {
            if (behavior.enabled && callback(behavior)) return true;
            if (topologyRevision_ != revision) return true;
        }
    } catch (const std::exception& error) {
        faulted_ = true; state_.paused = true;
        state_.dragging = false; state_.dragX.reset();
        for (auto& binding : state_.bindings) binding.down = false;
        out << "사용자 입력 정지: " << error.what() << "\nguide input 또는 remove <입력ID>로 수정하세요.\n";
        return true;
    }
    return false;
}

void Assembly::onKey(int key, int action, int mods, std::ostream& out) {
    if (action == GLFW_RELEASE) {
        for (auto& binding : state_.bindings)
            if (binding.key == key) binding.down = false;
    }
    if (dispatchInput([&](const Behavior& b) { return b.key && b.key(*this, key, action, mods); }, out)) return;
    if (action == GLFW_RELEASE) return;
    const int physicalMods = mods & (GLFW_MOD_SHIFT | GLFW_MOD_CONTROL | GLFW_MOD_ALT);
    for (auto& binding : state_.bindings) {
        if (binding.key != key || binding.mods != physicalMods) continue;
        if (binding.hold) binding.down = true;
        else if (action == GLFW_PRESS) {
            const auto command = binding.command;
            execute(command, out);
        }
        return;
    }
    if (action != GLFW_PRESS && action != GLFW_REPEAT) return;
    if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        execute(state_.paused ? "pause off" : "pause on", out);
        return;
    }
    if ((key == GLFW_KEY_TAB || key == GLFW_KEY_BACKSPACE) && action == GLFW_PRESS && !state_.nodes.empty()) {
        auto it = state_.nodes.find(state_.selected);
        if (key == GLFW_KEY_TAB) {
            if (it == state_.nodes.end() || ++it == state_.nodes.end()) it = state_.nodes.begin();
        } else {
            if (it == state_.nodes.begin() || it == state_.nodes.end()) it = state_.nodes.end();
            --it;
        }
        state_.selected = it->first;
        inspect(it->first, out);
    } else if (key == GLFW_KEY_X || key == GLFW_KEY_Y || key == GLFW_KEY_Z) {
        state_.axis = key == GLFW_KEY_X ? 0 : key == GLFW_KEY_Y ? 1 : 2;
    } else if (!state_.selected.empty()) {
        std::ostringstream command;
        const auto target = quote(state_.selected);
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
            command << "rotate " << target << ' ' << "xyz"[state_.axis] << ' ' << (key == GLFW_KEY_LEFT ? -5 : 5);
        else if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
            glm::vec3 delta(0); delta[state_.axis] = key == GLFW_KEY_UP ? .05f : -.05f;
            command << "move " << target << ' ' << delta.x << ' ' << delta.y << ' ' << delta.z;
        }
        if (!command.str().empty()) execute(command.str(), out);
    }
}

void Assembly::onFocus(bool focused) {
    if (!focused) {
        for (auto& binding : state_.bindings) binding.down = false;
        state_.dragging = false; state_.dragX.reset();
    }
    dispatchInput([&](const Behavior& b) { if (b.focus) b.focus(*this, focused); return false; }, std::cout);
}

void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {
    if (event.button == GLFW_MOUSE_BUTTON_RIGHT && event.action == GLFW_RELEASE) {
        state_.dragging = false; state_.dragX.reset();
    }
    if (dispatchInput([&](const Behavior& b) { return b.mouse && b.mouse(*this, event); }, out)) return;
    if (event.button == GLFW_MOUSE_BUTTON_RIGHT && !state_.dragTarget.empty()) {
        state_.dragging = event.action == GLFW_PRESS;
        state_.dragX.reset();
        return;
    }
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
    std::optional<graphics::PickResult> hit;
    if (state_.threeViews) {
        const auto size = app_.getFramebufferSize();
        if (size[0] < 3 || size[1] < 2) return;
        for (const auto& pass : graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions())) {
            const auto& v = pass.viewport;
            const double top = size[1] - v.y - v.height;
            if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top && event.framebufferY < top + v.height)
                hit = graphics::pick(app_.getScene(), pass.camera, event.framebufferX, event.framebufferY,
                    {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
        }
    } else hit = app_.pickAt(event.windowX, event.windowY);
    state_.selected.clear();
    std::optional<graphics::EntityId> entity = hit ? std::optional<graphics::EntityId>{hit->entityId} : std::nullopt;
    while (entity) {
        for (const auto& item : state_.nodes)
            if (item.second.id == *entity && item.second.whole &&
                (state_.selected.empty() || item.second.kind == "group"))
                state_.selected = item.first;
        const auto* component = app_.getScene().getComponent(*entity);
        entity = component ? component->parent : std::nullopt;
    }
    if (!state_.selected.empty()) {
        inspect(state_.selected, out);
        return;
    }
    out << "선택 해제\n";
}
void Assembly::onCursor(double x, double y, std::ostream& out) {
    if (dispatchInput([&](const Behavior& b) { return b.cursor && b.cursor(*this, x, y); }, out)) return;
    if (!state_.dragging || !std::isfinite(x)) return;
    try {
        if (state_.dragX) rotate(state_.dragTarget, state_.dragAxis, float(x - *state_.dragX) * state_.dragSensitivity);
        state_.dragX = x;
    } catch (const std::exception& error) {
        state_.dragging = false; state_.dragX.reset();
        out << "드래그 거절: " << error.what() << '\n';
    }
}
} // namespace assembly
```

<a id="file-18"></a>
## standard/modes/AssemblyMotion.cpp

변경 이유: 기존 Motion과 물리/보행을 부착하고 제어 ID를 검사한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include "Paths.h"
#include <algorithm>

namespace assembly {
bool Assembly::motionCommand(const Tokens& a, std::ostream&) {
    const auto& command = a[0];
    if (command == "body" || command == "release") {
        require(a.size() >= 2, "body <물리ID> velocity/gravity/bounce ... 또는 release <물리ID>");
        const auto it = std::find_if(state_.behaviors.begin(), state_.behaviors.end(), [&](const Behavior& b) {
            return b.name == a[1] && b.effect == Effect::Body;
        });
        require(it != state_.behaviors.end(), "물리 동작 ID가 필요합니다.");
        if (command == "release") { require(a.size() == 2, "release <물리ID>"); it->releaseRequested = true; }
        else {
            require(a.size() >= 4, "body <물리ID> velocity/gravity <XYZ> 또는 bounce on/off");
            if (a[2] == "bounce") { require(a.size() == 4, "body <ID> bounce on/off"); it->elastic = boolean(a[3]); }
            else {
                require(a.size() == 6 && (a[2] == "velocity" || a[2] == "gravity"), "body <ID> velocity/gravity <XYZ>");
                const auto value = vector(a, 3);
                if (a[2] == "gravity") it->gravity = value;
                else if (it->body.released) it->body.worldVelocity = value;
                else it->body.localVelocity = value;
            }
        }
        return true;
    }
    if (command == "remove") {
        require(a.size() == 2, "remove <동작 이름>");
        const auto it = std::find_if(state_.behaviors.begin(), state_.behaviors.end(), [&](const Behavior& b) { return b.name == a[1]; });
        require(it != state_.behaviors.end(), "없는 동작입니다: " + a[1]);
        if (faceEffect(it->effect) && node(it->target).mesh) {
            const auto mesh = *node(it->target).mesh;
            require(app_.getScene().setFaceLocalMatrix(mesh, it->faceIndex, it->initialFaceMatrix, it->initialFaceSpace) &&
                    app_.getScene().setFaceVisible(mesh, it->faceIndex, it->initialFaceVisible), "면 동작 복원 실패.");
        }
        state_.behaviors.erase(it);
        ++topologyRevision_;
        return true;
    }
    if (command == "play" || command == "enable" || command == "seek") {
        require(a.size() == 3, command + " <동작 이름> <on/off 또는 초>");
        const auto it = std::find_if(state_.behaviors.begin(), state_.behaviors.end(), [&](const Behavior& b) { return b.name == a[1]; });
        require(it != state_.behaviors.end(), "없는 동작입니다: " + a[1]);
        if (command == "play") it->playing = boolean(a[2]);
        else if (command == "enable") it->enabled = boolean(a[2]);
        else {
            require(it->effect != Effect::Body && it->effect != Effect::Walk, "적분 물리는 seek 대신 재조립을 사용하세요.");
            it->seconds = number(a[2]);
            require(it->seconds >= 0, "시간은 0 이상입니다.");
        }
        return true;
    }
    if (command == "jump" || command == "speed" || command == "direction" || command == "obstacle") {
        require(a.size() >= 3, command + " <보행 동작 이름> <값/객체>");
        const auto it = std::find_if(state_.behaviors.begin(), state_.behaviors.end(), [&](const Behavior& b) {
            return b.name == a[1] && b.effect == Effect::Walk;
        });
        require(it != state_.behaviors.end(), "보행 동작 이름이 필요합니다.");
        if (command == "obstacle") {
            require(a.size() == 3 && node(a[2]).mesh.has_value(), "obstacle <보행 동작> <메시 객체>");
            require(node(a[2]).mesh != node(it->reference).mesh, "방 walls를 장애물로 중복 등록할 수 없습니다.");
            std::optional<graphics::EntityId> ancestor = *node(a[2]).mesh;
            while (ancestor && *ancestor != node(it->target).id) ancestor = app_.getScene().getComponent(*ancestor)->parent;
            require(!ancestor, "보행 리그 자신의 부품을 장애물로 등록할 수 없습니다.");
            require(std::find(it->obstacles.begin(), it->obstacles.end(), a[2]) == it->obstacles.end(), "이미 등록한 장애물입니다.");
            it->obstacles.push_back(a[2]);
        } else if (command == "direction") {
            require(a.size() == 4, "direction <보행 동작> <X> <Z>");
            const glm::vec3 direction(float(number(a[2])), 0, float(number(a[3])));
            require(glm::length(direction) > .0001f, "0이 아닌 보행 방향이 필요합니다.");
            it->character.direction = glm::normalize(direction);
        } else {
            require(a.size() == 3, command + " <보행 동작> <속도>");
            const float speed = float(number(a[2]));
            require(speed >= 0 && speed <= 10, "속도는 0~10입니다.");
            if (command == "speed") it->character.speed = speed;
            else require(graphics::CollisionSystem::jump(it->character, speed), "접지 중에만 점프할 수 있습니다.");
        }
        return true;
    }
    const std::vector<std::string> supported{"spin", "orbit", "path", "linear", "pendulum", "face", "wall", "physics", "walk", "belt"};
    if (std::find(supported.begin(), supported.end(), command) == supported.end())
        return false;
    require(a.size() >= 3, command + " <동작 이름> <대상> ...");
    Behavior b;
    b.name = a[1]; b.target = a[2];
    auto& object = node(b.target);
    if (command == "spin" || command == "orbit") {
        require(command == "spin" ? (a.size() == 5 || a.size() == 6) : a.size() == 8,
                "spin <id> <대상> <축> <도/초> [local/world] 또는 orbit <id> <대상> <축> <도/초> <CX> <CY> <CZ>");
        graphics::RotationMotion config;
        config.axis.direction = axis(a[3]);
        config.degreesPerSecond = number(a[4]);
        if (command == "orbit") {
            config.axis.point = vector(a, 5);
            config.space = graphics::TransformSpace::World;
            b.positionDriver = true;
        } else if (a.size() == 6) {
            require(a[5] == "local" || a[5] == "world", "공간은 local/world입니다.");
            config.space = a[5] == "local" ? graphics::TransformSpace::Local : graphics::TransformSpace::World;
            b.parentSpin = config.space == graphics::TransformSpace::World;
        }
        require(b.motion.append(graphics::Motion::rotation(config)), "회전 동작 생성 실패.");
    } else if (command == "path") {
        require(a.size() == 5, "path <id> <대상> <1~6> <주기 초>");
        const double index = number(a[3]);
        require(index >= 1 && index <= 6 && index == std::floor(index), "경로 번호는 1~6입니다.");
        using Factory = graphics::PathMotion (*)();
        const Factory factories[]{standard_modes::path1, standard_modes::path2, standard_modes::path3,
            standard_modes::path4, standard_modes::path5, standard_modes::path6};
        auto config = factories[int(index) - 1]();
        config.durationSeconds = number(a[4]);
        require(config.durationSeconds > 0, "주기는 양수입니다.");
        config.relativeToStart = true;
        b.positionDriver = true;
        require(b.motion.append(graphics::Motion::followPath(config)), "경로 동작 생성 실패.");
    } else if (command == "linear") {
        require(a.size() == 7 || a.size() == 8, "linear <id> <대상> <DX> <DY> <DZ> <초> [once/loop/pingpong]");
        graphics::LinearMotion config;
        config.to = vector(a, 3); config.durationSeconds = number(a[6]);
        require(config.durationSeconds > 0, "이동 시간은 양수입니다.");
        if (a.size() == 8) {
            require(a[7] == "once" || a[7] == "loop" || a[7] == "pingpong", "재생은 once/loop/pingpong입니다.");
            config.playback = a[7] == "once" ? graphics::Playback::Once : a[7] == "loop" ? graphics::Playback::Loop : graphics::Playback::PingPong;
        }
        b.positionDriver = true;
        require(b.motion.append(graphics::Motion::linear(config)), "선형 이동 생성 실패.");
    } else if (command == "pendulum") {
        require(a.size() == 6, "pendulum <id> <대상> <축> <진폭 도> <주기 초>");
        graphics::PendulumMotion config;
        config.axis.direction = axis(a[3]); config.amplitudeDegrees = number(a[4]); config.periodSeconds = number(a[5]);
        require(config.periodSeconds > 0 && config.amplitudeDegrees >= 0, "양수 주기와 0 이상 진폭이 필요합니다.");
        require(b.motion.append(graphics::Motion::pendulum(config)), "진자 생성 실패.");
    } else if (command == "face" || command == "wall") {
        require(object.mesh.has_value(), "면을 가진 대상이 필요합니다.");
        require(command == "face" ? a.size() == 7 : a.size() == 5,
                "face <id> <대상> <면> <spin/orbit> <축> <도/초> 또는 face <id> <대상> <면> hinge <모서리> <도/초> 또는 wall <id> <방> <면0~3> <초>");
        const double face = number(a[3]);
        const auto* mesh = app_.getScene().getComponent(*object.mesh);
        require(face >= 0 && face == std::floor(face) && face < mesh->polyhedron.faces.size(), "면 번호 범위를 확인하세요.");
        b.faceIndex = std::size_t(face);
        if (command == "wall") {
            require(object.room && face < 4, "방 옆벽 0~3이 필요합니다.");
            b.effect = Effect::Wall; b.rate = float(number(a[4]));
            require(b.rate > 0, "개방 시간은 양수입니다.");
        } else {
            b.effect = Effect::Face;
            require(a[4] == "hinge" || a[4] == "spin" || a[4] == "orbit", "면 동작은 hinge/spin/orbit입니다.");
            b.face.kind = a[4] == "hinge" ? graphics::FaceRotationKind::EdgeHinge :
                a[4] == "spin" ? graphics::FaceRotationKind::SelfSpin : graphics::FaceRotationKind::ModelOrbit;
            b.rate = float(number(a[6]));
            if (a[4] == "hinge") {
                const double edge = number(a[5]);
                require(edge >= 0 && edge == std::floor(edge) && edge < mesh->polyhedron.faces[b.faceIndex].localVertices.size(), "모서리 범위를 확인하세요.");
                b.face.edge = std::size_t(edge);
            } else b.face.axis = axis(a[5]);
        }
    } else if (command == "physics" || command == "walk") {
        require(command == "physics" ? a.size() == 4 : a.size() == 5,
                "physics <id> <상자> <방> 또는 walk <id> <walker> <방> <속도>");
        b.reference = a[3];
        const auto& room = node(b.reference);
        require(room.room.has_value(), "add room으로 만든 방이 필요합니다.");
        require(room.mesh.has_value(), "방 메시가 필요합니다.");
        const auto& wall = node(b.reference + ".walls");
        const auto originalWalls = graphics::DiagramSystem::modelMatrix(wall.initial);
        const auto currentWalls = graphics::DiagramSystem::modelMatrix(wall.rest);
        for (int c = 0; c < 4; ++c)
            require(glm::length(originalWalls[c] - currentWalls[c]) < .000001f,
                    "방 walls의 초기 변환을 유지한 뒤 물리를 붙이세요. 방 루트를 조작하세요.");
        const auto matrix = app_.getScene().worldMatrix(room.id);
        const auto world = app_.getScene().worldMatrix(object.id);
        require(matrix && world && rigid(*matrix) && !app_.getScene().getComponent(object.id)->parent,
                "강체 방과 부모 없는 물리 루트가 필요합니다.");
        b.previousRoom = *matrix; b.positionDriver = true;
        if (command == "physics") {
            require(object.kind == "box" || object.kind == "sphere", "physics는 중심 기준 box/sphere를 지원합니다.");
            b.elastic = object.kind == "sphere";
            const auto* mesh = object.mesh ? app_.getScene().getComponent(*object.mesh) : nullptr;
            require(mesh && glm::length(object.rest.localBasis) < .0001f &&
                    glm::length(mesh->transform.basis) < .0001f && glm::length(mesh->transform.localBasis) < .0001f &&
                    glm::length(mesh->transform.scale - glm::vec3(1)) < .0001f &&
                    glm::length(mesh->transform.rotation[0] - glm::vec3(1, 0, 0)) < .0001f &&
                    glm::length(mesh->transform.rotation[1] - glm::vec3(0, 1, 0)) < .0001f,
                    "상자의 중심 피벗과 초기 mesh 변환을 유지한 뒤 physics를 붙이세요.");
            b.effect = Effect::Body;
            b.body.localPosition = glm::vec3(glm::inverse(*matrix) * (*world)[3]);
        } else {
            require(object.walker.has_value(), "add walker로 만든 리그가 필요합니다.");
            require(object.rest.scale == glm::vec3(1) && glm::length(object.rest.localBasis) < .0001f && rigid(*world),
                    "보행 리그는 배율 1과 중심 피벗을 유지해야 합니다.");
            b.effect = Effect::Walk;
            require(std::abs(object.rest.rotation[1].y - 1) < .001f, "보행 루트의 초기 회전은 Y축 회전만 지원합니다.");
            b.character.direction = object.rest.rotation * glm::vec3(0, 0, 1);
            b.character.feet = glm::vec3((*world)[3]); b.character.height = 1.8f;
            b.character.speed = float(number(a[4]));
            require(b.character.speed >= 0 && b.character.speed <= 10, "보행 속도는 0~10입니다.");
        }
    } else {
        require(a.size() == 5 || a.size() == 6, "belt <id> <대상> <속도> <높이 보정> [위상0~1]");
        b.effect = Effect::Conveyor; b.positionDriver = true;
        b.rate = float(number(a[3])); b.height = float(number(a[4]));
        b.phase = a.size() == 6 ? float(number(a[5])) : 0;
        require(b.phase >= 0 && b.phase <= 1, "위상은 0~1입니다.");
    }
    attach(std::move(b));
    return true;
}
} // namespace assembly
```

<a id="file-19"></a>
## standard/modes/AssemblyObjects.cpp

변경 이유: 기존 기본 형상·리그·방·격자·컨베이어·행성 생성기를 재사용한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include "AssemblyParts.h"
#include <cctype>

namespace assembly {
void Assembly::registerPerson(const std::string& name, const graphics::HumanoidRig& rig) {
    registerNode(name, rig.root, true, "humanoid");
    registerNode(name + ".pelvis", rig.pelvis, false);
    registerNode(name + ".torso", rig.torso, false, "joint", rig.torsoMesh);
    registerNode(name + ".neck", rig.neck, false, "joint", rig.head);
    registerNode(name + ".head", rig.head, false, "mesh", rig.head);
    const char* sides[] = {".left", ".right"};
    for (std::size_t i = 0; i < 2; ++i) {
        const std::string prefix = name + sides[i];
        registerNode(prefix + "Shoulder", rig.shoulders[i], false);
        registerNode(prefix + "Elbow", rig.elbows[i], false);
        registerNode(prefix + "Hand", rig.hands[i], false);
        registerNode(prefix + "Hip", rig.hips[i], false);
        registerNode(prefix + "Knee", rig.knees[i], false);
        registerNode(prefix + "Foot", rig.feet[i], false);
    }
}

void Assembly::add(const std::string& kind, const std::string& name, glm::vec3 position, glm::vec3 size) {
    require(!name.empty() && name.size() <= 64 && !state_.nodes.count(name), "사용하지 않은 객체 이름(1~64바이트)이 필요합니다.");
    for (unsigned char c : name)
        require(c > 32 && c != '"' && c != '#' && c != '\\', "이름에는 공백·따옴표·#·역슬래시를 쓰지 마세요.");
    require(name.front() != '.' && name.back() != '.' && name.find("..") == std::string::npos, "이름의 각 구간은 비어 있으면 안 됩니다.");
    require(state_.nodes.size() < 1000, "객체/부품 이름 1000개 한도입니다.");
    require(graphics::detail::finiteVector(position) && graphics::detail::finiteVector(size) && glm::all(glm::greaterThan(size, glm::vec3(0))), "위치와 양수 크기를 확인하세요.");
    const bool sized = kind == "box" || kind == "pyramid" || kind == "sphere" || kind == "cone" || kind == "tetra";
    require(sized || size == glm::vec3(1), "복합 객체는 크기 인자 대신 scale 명령을 사용하세요.");
    auto& scene = app_.getScene();
    const auto entity = [](std::optional<graphics::EntityId> id) {
        require(id.has_value(), "객체 생성 실패. 앞선 프레임워크 진단을 확인하세요.");
        return *id;
    };
    if (kind == "humanoid") {
        graphics::HumanoidConfig config;
        config.grid.frame = glm::translate(glm::mat4(1), position);
        const auto rig = graphics::AnimationSystem::addHumanoid(scene, config);
        require(rig.has_value(), "사람 생성 실패.");
        registerPerson(name, *rig);
        return;
    }
    if (kind == "swing") {
        graphics::SwingConfig config;
        config.pivot += position;
        const auto rig = graphics::AnimationSystem::addSwingWithRider(scene, config);
        require(rig.has_value(), "그네 생성 실패.");
        registerNode(name, rig->frame, true, kind);
        registerNode(name + ".hinge", rig->hinge, false);
        registerNode(name + ".seat", rig->seat, false, "mesh", rig->seat);
        registerPerson(name + ".rider", rig->person);
        for (std::size_t i = 0; i < rig->animation.size(); ++i) {
            Behavior b;
            b.name = name + (i == 0 ? ".swing" : ".sway");
            b.target = name + (i == 0 ? ".hinge" : ".rider.torso");
            b.motion = rig->animation[i].motion;
            attach(std::move(b));
        }
        return;
    }
    if (kind == "tank") {
        const auto rig = addTankParts(scene);
        graphics::Transform3D rest;
        rest.basis = position;
        require(scene.setTransform(rig.root, rest), "탱크 위치 설정 실패.");
        registerNode(name, rig.root, true, kind);
        node(name).guideTopic = "tank-preset";
        registerNode(name + ".central", rig.central, false);
        for (std::size_t i = 0; i < 2; ++i) {
            const auto prefix = name + (i == 0 ? ".left" : ".right");
            registerNode(prefix + "Lower", rig.lower[i], false, "mesh", rig.lower[i]);
            registerNode(prefix + "Upper", rig.upper[i], false);
            registerNode(prefix + "Gun", rig.guns[i], false);
            registerNode(prefix + "Pole", rig.poles[i], false);
        }
        return;
    }
    if (kind == "walker") {
        auto grid = graphics::HumanoidConfig{}.grid;
        grid.frame = glm::translate(glm::mat4(1), position);
        const auto rig = graphics::PatternSystem::addWalkingRig(scene, grid);
        require(rig.has_value(), "보행 리그 생성 실패.");
        registerNode(name, rig->root, true, kind);
        node(name).walker = *rig;
        registerNode(name + ".head", rig->head, false);
        for (std::size_t i = 0; i < 2; ++i) {
            const auto prefix = name + (i == 0 ? ".left" : ".right");
            registerNode(prefix + "Shoulder", rig->shoulders[i], false);
            registerNode(prefix + "Hip", rig->hips[i], false);
        }
        return;
    }
    const bool primitive = kind == "box" || kind == "pyramid" || kind == "sphere" || kind == "cone" || kind == "tetra";
    require(primitive || kind == "room" || kind == "grid" || kind == "joint" || kind == "conveyor" || kind == "planets",
            "지원 종류: box pyramid sphere cone tetra humanoid swing tank walker room grid joint conveyor planets");
    graphics::Transform3D rest;
    rest.basis = position;
    const auto root = entity(scene.addGroup(rest));
    registerNode(name, root, true, kind);
    if (primitive) {
        graphics::EntityId mesh;
        if (kind == "box")
            mesh = entity(scene.addCuboid(size, {0, 0, 0}));
        else if (kind == "tetra")
            mesh = entity(scene.addRightTetrahedron(size, {0, 0, 0}));
        else if (kind == "pyramid")
            mesh = entity(scene.addPyramid(size, {0, 0, 0}));
        else if (kind == "sphere") {
            require(size.x == size.y && size.x == size.z, "sphere의 크기는 세 축이 같은 지름입니다.");
            mesh = entity(scene.addSphere(size.x * .5f, {0, 0, 0}));
        } else {
            require(size.x == size.z, "cone의 X/Z 지름은 같아야 합니다.");
            mesh = entity(scene.addCone(size.x * .5f, size.y, {0, 0, 0}));
        }
        require(scene.setParent(mesh, root), "메시 부모 연결 실패.");
        node(name).mesh = mesh;
        registerNode(name + ".mesh", mesh, false, "mesh", mesh);
    } else if (kind == "room") {
        const graphics::Bounds3 bounds{{-2, 0, -2}, {2, 3, 2}};
        const auto mesh = entity(graphics::PatternSystem::addRoom(scene, bounds));
        require(scene.setParent(mesh, root), "방 부모 연결 실패.");
        node(name).mesh = mesh;
        node(name).room = bounds;
        registerNode(name + ".walls", mesh, false, "mesh", mesh);
    } else if (kind == "grid") {
        graphics::GridLayout grid;
        grid.origin = {-3, -.1f, -2};
        grid.extent = {6, .2f, 4};
        grid.cells = {6, 1, 4};
        const auto cells = addGridParts(scene, grid);
        for (std::size_t i = 0; i < cells.size(); ++i) {
            require(scene.setParent(cells[i], root), "격자 부모 연결 실패.");
            registerNode(name + ".cell" + std::to_string(i + 1), cells[i], false, "mesh", cells[i]);
        }
    } else if (kind == "conveyor") {
        for (const auto& spec : std::vector<std::pair<glm::vec3, glm::vec3>>{
            {{7.2f, .15f, 1.6f}, {-.8f, .925f, -2}}, {{1.6f, .15f, 5}, {3, .925f, 1.3f}}})
            entity(graphics::PatternSystem::addBox(scene, spec.first, spec.second, {.15f, .2f, .25f}, root));
        for (unsigned i = 0; i < 5; ++i) {
            const auto parcel = entity(scene.addGroup());
            require(scene.setParent(parcel, root), "상자 부모 연결 실패.");
            const auto mesh = entity(graphics::PatternSystem::addBox(scene, {.85f, .85f, .85f}, {0, 0, 0},
                {.3f + .12f * i, .7f, .3f}, parcel));
            const auto label = name + ".parcel" + std::to_string(i + 1);
            registerNode(label, parcel, true, "parcel", mesh);
            require(graphics::PatternSystem::addNumber(scene, i + 1, {-.1f, 0, .43f}, .3f, {1, 1, 1}, parcel),
                    "택배 번호 생성 실패.");
            Behavior b;
            b.name = label + ".belt"; b.target = label; b.effect = Effect::Conveyor;
            b.positionDriver = true; b.rate = 1.25f; b.phase = float(i) / 5;
            attach(std::move(b));
        }
    } else if (kind == "planets") {
        const auto sun = entity(scene.addSphere(.55f, {0, 0, 0}));
        require(scene.setParent(sun, root) && scene.setTint(sun, {1, .8f, .2f}), "태양 생성 실패.");
        registerNode(name + ".sun", sun, true, "sun", sun);
        for (unsigned i = 0; i < 3; ++i) {
            const auto planet = entity(scene.addGroup());
            const auto mesh = entity(scene.addSphere(.25f, {0, 0, 0}));
            require(scene.setParent(planet, root) && scene.setParent(mesh, planet), "행성 부모 연결 실패.");
            const auto label = name + ".planet" + std::to_string(i + 1);
            registerNode(label, planet, true, "planet", mesh);
            node(label).rest.basis = {2 + .8f * i, 0, 0};
            Behavior orbit;
            orbit.name = label + ".orbit"; orbit.target = label; orbit.positionDriver = true;
            graphics::RotationMotion config;
            config.space = graphics::TransformSpace::World; config.degreesPerSecond = 20 + 10 * i;
            require(orbit.motion.append(graphics::Motion::rotation(config)), "행성 공전 생성 실패.");
            attach(std::move(orbit));
            const auto moon = entity(scene.addGroup());
            const auto moonMesh = entity(scene.addSphere(.1f, {0, 0, 0}));
            require(scene.setParent(moon, planet) && scene.setParent(moonMesh, moon), "달 부모 연결 실패.");
            registerNode(label + ".moon", moon, true, "moon", moonMesh);
            node(label + ".moon").rest.basis = {.6f, 0, 0};
            Behavior lunar;
            lunar.name = label + ".moon.orbit"; lunar.target = label + ".moon"; lunar.positionDriver = true;
            config.degreesPerSecond = 70 + 30 * i;
            require(lunar.motion.append(graphics::Motion::rotation(config)), "달 공전 생성 실패.");
            attach(std::move(lunar));
        }
    }
}
} // namespace assembly
```

<a id="file-20"></a>
## standard/modes/AssemblyParts.cpp

변경 이유: 기존 격자·탱크 부품 생성의 공통 구현을 둔다.

### before

없음. 새 파일이다.

### after

```cpp
#include "AssemblyParts.h"
#include "PracticeCommon.h"

namespace assembly {
TankParts addTankParts(graphics::Scene& scene) {
    TankParts rig;
    const auto entity = [](std::optional<graphics::EntityId> id) {
        return practice_common::requireEntity(id, "Tank part creation failed.");
    };
    const auto box = [&](glm::vec3 size, glm::vec3 position, graphics::EntityId parent) {
        const auto id = entity(graphics::PatternSystem::addBox(scene, size, position, {1, 1, 1}, parent));
        rig.colored.push_back(id);
        return id;
    };
    rig.root = entity(scene.addGroup());
    rig.central = entity(graphics::PatternSystem::addJoint(scene, rig.root, {0, .6f, 0}));
    for (std::size_t i = 0; i < 2; ++i)
        rig.lower[i] = box({.65f, .6f, 1.8f}, {i == 0 ? -.8f : .8f, .3f, 0}, rig.root);
    box({2.3f, .25f, 1.4f}, {0, .125f, 0}, rig.central);
    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        rig.upper[i] = entity(graphics::PatternSystem::addJoint(scene, rig.central, {sign * .65f, .25f, 0}));
        rig.guns[i] = entity(graphics::PatternSystem::addJoint(scene, rig.upper[i], {0, .3f, 0}));
        rig.poles[i] = entity(graphics::PatternSystem::addJoint(scene, rig.upper[i], {0, .6f, 0}));
        box({.65f, .6f, .8f}, {0, .3f, 0}, rig.upper[i]);
        box({.14f, .14f, 1.1f}, {0, -.05f, .95f}, rig.guns[i]);
        box({.08f, .85f, .08f}, {0, .425f, 0}, rig.poles[i]);
    }
    return rig;
}

std::vector<graphics::EntityId> addGridParts(graphics::Scene& scene, const graphics::GridLayout& grid) {
    std::vector<graphics::EntityId> cells;
    for (int z = 0; z < grid.cells.z; ++z)
        for (int x = 0; x < grid.cells.x; ++x) {
            const auto center = grid.cellCenter({x, 0, z});
            const auto size = grid.relativeSize({.15f, 1, .22f});
            practice_common::require(center && size, "Invalid grid cell.");
            cells.push_back(practice_common::requireEntity(graphics::PatternSystem::addBox(scene, *size, *center,
                (x + z) % 2 ? graphics::GlColor{.25f, .3f, .4f} : graphics::GlColor{.4f, .45f, .55f}),
                "Grid cell creation failed."));
        }
    return cells;
}
} // namespace assembly
```

<a id="file-21"></a>
## standard/modes/AssemblyParts.h

변경 이유: 공유 생성기의 부품 ID 묶음과 함수를 선언한다.

### before

없음. 새 파일이다.

### after

```cpp
#pragma once
#include "PracticeMode.h"

namespace assembly {
struct TankParts {
    graphics::EntityId root = 0, central = 0;
    std::array<graphics::EntityId, 2> lower{}, upper{}, guns{}, poles{};
    std::vector<graphics::EntityId> colored;
};
TankParts addTankParts(graphics::Scene& scene);
std::vector<graphics::EntityId> addGridParts(graphics::Scene& scene, const graphics::GridLayout& grid);
} // namespace assembly
```

<a id="file-22"></a>
## standard/modes/AssemblyPersistence.cpp

변경 이유: BOM 레시피·rest/rest3·표시 설정과 실패 시 파일 보존을 처리한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include <fstream>
#include <iomanip>
#include <sstream>

namespace assembly {
bool Assembly::save(const std::string& path, std::ostream& out) const {
    const auto file = std::filesystem::absolute(std::filesystem::u8path(path));
    auto temporary = file;
    temporary += L".saving-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(GetTickCount64());
    try {
        std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);
        require(bool(stream), "레시피 저장 파일을 열 수 없습니다.");
        stream << "\xEF\xBB\xBF# standard assembly recipe v1\nclear\n" << std::setprecision(9);
        for (const auto& line : state_.recipe)
            stream << line << '\n';
        stream << "# @state-begin\n";
        for (const auto& item : state_.nodes) {
            const auto& t = item.second.rest;
            stream << (item.second.affineGroup ? "rest3 " : "rest ") << quote(item.first) << ' ' << t.basis.x << ' ' << t.basis.y << ' ' << t.basis.z;
            for (int c = 0; c < 3; ++c)
                for (int r = 0; r < 3; ++r)
                    stream << ' ' << t.rotation[c][r];
            stream << ' ' << t.scale.x;
            if (item.second.affineGroup) stream << ' ' << t.scale.y << ' ' << t.scale.z;
            stream << ' ' << t.localBasis.x << ' ' << t.localBasis.y << ' ' << t.localBasis.z << '\n';
        }
        const auto& c = app_.getCamera();
        const auto& r = app_.getRenderOptions();
        stream << "camera pose " << c.position.x << ' ' << c.position.y << ' ' << c.position.z << ' '
               << c.target.x << ' ' << c.target.y << ' ' << c.target.z << ' ' << c.up.x << ' ' << c.up.y << ' ' << c.up.z << '\n'
               << "camera projection " << (c.orthographic ? "ortho" : "perspective") << '\n'
               << "camera extent " << c.orthographicHalfExtent << "\ncamera fov " << c.fieldOfViewDegrees
               << "\ncamera clip " << c.nearPlane << ' ' << c.farPlane << '\n'
               << "render wire " << (r.wireframe ? "on" : "off") << "\nrender lighting " << (r.lighting ? "on" : "off")
               << "\nrender depth " << (r.depthTest ? "on" : "off") << "\nview " << (state_.threeViews ? 3 : 1)
               << "\npause " << (state_.paused ? "on" : "off") << '\n'
               << "render cull " << (r.cull == graphics::CullMode::Front ? "front" : r.cull == graphics::CullMode::Back ? "back" : "none") << '\n'
               << "render light-direction " << r.lightDirection.x << ' ' << r.lightDirection.y << ' ' << r.lightDirection.z << '\n'
               << "render light-color " << r.lightColor.red << ' ' << r.lightColor.green << ' ' << r.lightColor.blue << '\n'
               << "render background " << r.background.r << ' ' << r.background.g << ' ' << r.background.b << '\n';
        stream.flush();
        require(bool(stream), "레시피 쓰기에 실패했습니다.");
        stream.close();
        require(bool(stream) && std::filesystem::file_size(temporary) <= 2 * 1024 * 1024, "저장 결과가 2MiB 한도를 넘거나 쓰기를 완료하지 못했습니다.");
        require(MoveFileExW(temporary.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0,
                "저장 파일 교체 실패. 기존 파일을 보존했습니다.");
        out << "조립 레시피 저장: " << file.u8string() << " (물리/애니메이션 현재 프레임의 스냅샷은 아님)\n";
        return true;
    } catch (...) {
        std::error_code ignored;
        std::filesystem::remove(temporary, ignored);
        throw;
    }
}

bool Assembly::load(const std::string& path, std::ostream& out) {
    const auto requested = std::filesystem::u8path(path);
    auto file = std::filesystem::absolute(requested);
    if (!requested.is_absolute() && !std::filesystem::exists(file)) {
        std::wstring binary(32768, L'\0');
        const DWORD count = GetModuleFileNameW(nullptr, binary.data(), DWORD(binary.size()));
        if (count > 0 && count < binary.size()) {
            binary.resize(count);
            const auto deployed = std::filesystem::path(binary).parent_path() / requested;
            if (std::filesystem::is_regular_file(deployed)) file = deployed;
        }
    }
    require(std::filesystem::file_size(file) <= 2 * 1024 * 1024, "레시피는 2MiB 이하입니다.");
    std::ifstream stream(file, std::ios::binary);
    require(bool(stream), "레시피 파일을 열 수 없습니다.");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (lines.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);
        require(line.size() <= 4096 && lines.size() < 10000, "명령은 4096바이트 이하, 레시피는 10000줄 미만입니다.");
        lines.push_back(line);
    }
    require(!stream.bad(), "레시피 읽기 실패.");
    clear();
    replaying_ = true;
    std::vector<std::string> authored;
    bool snapshot = false;
    try {
        for (std::size_t i = 0; i < lines.size(); ++i) {
            if (lines[i] == "# @state-begin") snapshot = true;
            if (!snapshot) authored.push_back(lines[i]);
            const auto tokens = tokenize(lines[i]);
            require(tokens.empty() || (tokens[0] != "save" && tokens[0] != "load"), "레시피 안의 save/load는 허용하지 않습니다.");
            require(execute(lines[i], out), "레시피 " + std::to_string(i + 1) + "줄 적용 실패.");
        }
        state_.recipe = std::move(authored);
        replaying_ = false;
        out << "조립 레시피 불러오기: " << file.u8string() << '\n';
        return true;
    } catch (...) {
        replaying_ = false;
        throw;
    }
}
} // namespace assembly
```

<a id="file-23"></a>
## standard/modes/AssemblyRuntime.cpp

변경 이유: 기준값·모션·면·물리·보행·Custom·부착 관절을 순서대로 합성한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include <algorithm>
#include <limits>

namespace assembly {
namespace {
graphics::Bounds3 bounds(const graphics::Scene& scene, graphics::EntityId mesh) {
    const auto* component = scene.getComponent(mesh);
    const auto matrix = scene.worldMatrix(mesh);
    require(component && matrix, "경계 계산에 필요한 메시/행렬이 없습니다.");
    graphics::Bounds3 box{glm::vec3(std::numeric_limits<float>::max()), glm::vec3(std::numeric_limits<float>::lowest())};
    for (const auto& face : component->polyhedron.faces)
        for (const auto& vertex : face.localVertices) {
            const auto p = glm::vec3(*matrix * glm::vec4(vertex, 1));
            box.minimum = glm::min(box.minimum, p);
            box.maximum = glm::max(box.maximum, p);
        }
    require(box.valid(), "양수 크기의 메시 경계가 필요합니다.");
    return box;
}
}

bool Assembly::applyPoses(float dt) {
    auto& scene = app_.getScene();
    const auto motionFor = [&](const std::string& name) {
        graphics::Motion motion;
        glm::mat3 parentRotation(1);
        for (const auto& b : state_.behaviors) {
            if (!b.enabled || b.target != name || b.effect != Effect::Motion || b.parentSpin) continue;
            const auto sample = b.motion.sample(b.seconds);
            require(sample.has_value(), "부모 공간 모션 샘플 실패: " + b.name);
            parentRotation = glm::mat3(sample->parent) * parentRotation;
        }
        for (const auto& b : state_.behaviors) {
            if (!b.enabled || b.target != name || b.effect != Effect::Motion)
                continue;
            const auto sample = b.motion.sample(b.seconds);
            require(sample.has_value(), "Motion 샘플 실패: " + b.name);
            const auto& rest = node(name).rest;
            const auto pivot = glm::translate(glm::mat4(1), rest.localBasis);
            const auto inversePivot = glm::translate(glm::mat4(1), -rest.localBasis);
            auto parent = sample->parent, local = sample->local;
            if (b.parentSpin) {
                const auto rotation = glm::mat4(parentRotation * rest.rotation);
                local = glm::transpose(rotation) * parent * rotation;
                parent = glm::mat4(1);
            }
            require(motion.append(graphics::Motion::fixed(parent, graphics::TransformSpace::World)) &&
                    motion.append(graphics::Motion::fixed(pivot * local * inversePivot, graphics::TransformSpace::Local)),
                    "Motion 합성 실패: " + b.name);
        }
        return motion;
    };
    std::vector<graphics::JointAnimation> tracks;
    for (const auto& item : state_.nodes) {
        auto* driver = positionDriver(item.first);
        if (driver && (driver->effect == Effect::Body || driver->effect == Effect::Walk || driver->effect == Effect::Custom))
            continue;
        if (item.second.affineGroup) {
            require(scene.setTransform(item.second.id, item.second.rest), "정적 warp 그룹 적용 실패.");
            continue;
        }
        tracks.push_back({item.second.id, item.second.rest, motionFor(item.first)});
    }
    require(graphics::AnimationSystem::poseHierarchy(scene, tracks, 0), "기준 자세와 동작 합성 실패.");
    for (int phase = 0; phase < 4; ++phase) {
    for (auto& b : state_.behaviors) {
        const int order = b.effect == Effect::Body ? 1 : b.effect == Effect::Walk ? 2 : b.effect == Effect::Custom ? 3 : 0;
        if (phase != order)
            continue;
        auto& target = node(b.target);
        if (b.effect == Effect::Conveyor) {
            if (!b.enabled) continue;
            const double length = 6.2 + 1.5707963267948966 * .8 + 4.2;
            const auto pose = graphics::PatternSystem::conveyorCornerPose(b.seconds * b.rate + b.phase * length,
                {-4, 1, -2}, {2.2f, 1, -1.2f}, .8f, 3);
            require(pose.has_value(), "컨베이어 경로 계산 실패.");
            auto rest = target.rest;
            const auto turn = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(pose->yawDegrees), glm::vec3(0, 1, 0)));
            rest.basis = pose->position + glm::vec3(0, b.height, 0) + turn * rest.basis;
            rest.rotation = turn * rest.rotation;
            require(graphics::AnimationSystem::poseHierarchy(scene, {{target.id, rest, motionFor(b.target)}}, 0), "컨베이어 자세 적용 실패.");
        } else if (faceEffect(b.effect)) {
            require(target.mesh.has_value(), "면 동작에는 메시가 필요합니다.");
            if (!b.enabled) {
                require(scene.setFaceLocalMatrix(*target.mesh, b.faceIndex, b.initialFaceMatrix, b.initialFaceSpace) &&
                        scene.setFaceVisible(*target.mesh, b.faceIndex, b.initialFaceVisible), "면 기준값 복원 실패.");
                continue;
            }
            if (b.effect == Effect::FaceTrack) {
                const auto sample = b.motion.sample(b.seconds);
                require(sample.has_value(), "면 트랙 샘플 실패.");
                const auto matrix = sample->parent * b.faceTrackBase * sample->local;
                require(std::abs(glm::determinant(glm::mat3(matrix))) > 1e-18f &&
                        scene.setFaceLocalMatrix(*target.mesh, b.faceIndex, matrix, graphics::FaceAnimationSpace::Model),
                        "면 트랙이 특이하거나 유효하지 않습니다.");
            } else if (b.effect == Effect::Face) {
                auto config = b.face;
                config.degrees = b.enabled ? b.seconds * b.rate : 0;
                require(graphics::PatternSystem::rotateFace(scene, *target.mesh, b.faceIndex, config), "면 회전 실패.");
            } else {
                const float u = b.enabled ? float(std::clamp(b.seconds / b.rate, 0.0, 1.0)) : 0;
                require(graphics::PatternSystem::openRoomWall(scene, *target.mesh, b.faceIndex, u), "벽 개방 실패.");
            }
        } else if (b.effect == Effect::Custom) {
            const float stepTime = b.enabled && b.playing && !state_.paused ? dt : 0;
            if (b.enabled) require(b.custom && b.custom(scene, target.rest, b.seconds, stepTime), "C++ 사용자 동작 실패: " + b.name);
        } else if (b.effect == Effect::Body) {
            const auto& room = node(b.reference);
            const auto matrix = scene.worldMatrix(room.id);
            require(room.room && matrix && rigid(*matrix), "물리 방은 강체여야 합니다.");
            const auto* walls = room.mesh ? scene.getComponent(*room.mesh) : nullptr;
            require(walls && walls->polyhedron.faces.size() > 5, "방 바닥이 없습니다.");
            const bool open = !walls->polyhedron.faces[5].visible;
            const auto* mesh = target.mesh ? scene.getComponent(*target.mesh) : nullptr;
            require(mesh != nullptr, "물리 상자 메시가 없습니다.");
            glm::vec3 half(0);
            for (const auto& face : mesh->polyhedron.faces)
                for (const auto& vertex : face.localVertices)
                    half = glm::max(half, glm::abs(target.rest.rotation * (vertex * target.rest.scale)));
            if (target.kind == "sphere") {
                float radius = 0;
                for (const auto& face : mesh->polyhedron.faces)
                    for (const auto& vertex : face.localVertices) radius = std::max(radius, glm::length(vertex));
                half = glm::vec3(radius * target.rest.scale.x);
            }
            b.body.halfExtent = half;
            if (!b.body.released) {
                b.releasedRotation = glm::mat3(*matrix);
                const auto low = room.room->minimum + half, high = room.room->maximum - half;
                require(glm::all(glm::lessThan(low, high)), "상자가 방보다 큽니다.");
                b.body.localPosition.x = std::clamp(b.body.localPosition.x, low.x, high.x);
                b.body.localPosition.z = std::clamp(b.body.localPosition.z, low.z, high.z);
                b.body.localPosition.y = open ? std::min(b.body.localPosition.y, high.y)
                                              : std::clamp(b.body.localPosition.y, low.y, high.y);
            }
            glm::vec3 omega(0);
            if (dt > 0) {
                const auto previous = glm::mat3(b.previousRoom), current = glm::mat3(*matrix);
                omega = (glm::cross(previous[0], current[0]) + glm::cross(previous[1], current[1]) +
                         glm::cross(previous[2], current[2])) / (2 * dt);
            }
            if (dt > 0) b.angularVelocity = omega;
            if (b.releaseRequested && !b.body.released) {
                b.body.worldPosition = b.body.position(*matrix);
                b.body.worldVelocity = glm::mat3(*matrix) * b.body.localVelocity +
                    glm::cross(b.angularVelocity, b.body.worldPosition - glm::vec3((*matrix)[3]));
                b.body.released = true;
            }
            b.releaseRequested = false;
            require(graphics::CollisionSystem::stepRoomBody(b.body, *room.room, *matrix,
                b.enabled && b.playing && !state_.paused ? dt : 0, open, b.elastic, b.gravity, b.angularVelocity), "방 충돌 계산 실패.");
            b.previousRoom = *matrix;
            auto rest = target.rest;
            rest.basis = b.body.position(*matrix);
            rest.rotation = b.releasedRotation * rest.rotation;
            require(scene.setTransform(target.id, rest), "물리 상자 자세 적용 실패.");
        } else if (b.effect == Effect::Walk) {
            const auto& room = node(b.reference);
            const auto matrix = scene.worldMatrix(room.id);
            require(room.room && target.walker && matrix && rigid(*matrix), "보행 리그와 방이 필요합니다.");
            const auto r = glm::mat3(*matrix);
            require(glm::length(r[0] - glm::vec3(1, 0, 0)) < .001f && glm::length(r[1] - glm::vec3(0, 1, 0)) < .001f,
                    "보행 공간은 월드 축 정렬을 유지해야 합니다.");
            const auto offset = glm::vec3((*matrix)[3]);
            const graphics::Bounds3 stage{room.room->minimum + offset, room.room->maximum + offset};
            std::vector<graphics::Bounds3> obstacles;
            for (const auto& obstacle : b.obstacles) {
                const auto& object = node(obstacle);
                require(object.mesh.has_value(), "장애물 메시가 없습니다.");
                obstacles.push_back(bounds(scene, *object.mesh));
            }
            const auto* walls = room.mesh ? scene.getComponent(*room.mesh) : nullptr;
            require(walls && walls->polyhedron.faces.size() > 5, "보행 공간의 바닥이 없습니다.");
            b.released = b.released || !walls->polyhedron.faces[5].visible;
            const float stepTime = b.enabled && b.playing && !state_.paused ? dt : 0;
            if (b.released) {
                const auto delta = b.character.direction * b.character.speed * stepTime;
                b.character.verticalSpeed -= 9.8f * stepTime;
                b.character.feet += delta + glm::vec3(0, b.character.verticalSpeed * stepTime, 0);
                b.character.walkDistance += glm::length(glm::vec2(delta.x, delta.z));
                b.character.grounded = false;
            } else {
                const auto step = graphics::CollisionSystem::stepCharacter(b.character, stage, obstacles, stepTime);
                require(step.has_value(), "보행 충돌 계산 실패.");
            }
            const float yaw = glm::degrees(std::atan2(b.character.direction.x, b.character.direction.z));
            require(graphics::PatternSystem::poseWalkingRig(scene, *target.walker, b.character.feet, yaw,
                b.character.walkDistance, b.character.speed), "보행 자세 적용 실패.");
            for (const auto& part : state_.nodes) {
                const auto id = part.second.id;
                const auto& rig = *target.walker;
                if (id != rig.head && id != rig.shoulders[0] && id != rig.shoulders[1] && id != rig.hips[0] && id != rig.hips[1])
                    continue;
                auto rest = scene.getComponent(id)->transform;
                const auto matrix = graphics::DiagramSystem::modelMatrix(rest) *
                    glm::inverse(graphics::DiagramSystem::modelMatrix(part.second.initial)) *
                    graphics::DiagramSystem::modelMatrix(part.second.rest);
                rest.scale = part.second.rest.scale;
                rest.localBasis = part.second.rest.localBasis;
                rest.rotation = glm::mat3(matrix) / rest.scale.x;
                rest.basis = glm::vec3(matrix[3]) + rest.rotation * (rest.scale * rest.localBasis);
                const std::vector<graphics::JointAnimation> edited{{part.second.id, rest, motionFor(part.first)}};
                require(graphics::AnimationSystem::poseHierarchy(scene, edited, 0), "보행과 부품 동작 합성 실패.");
            }
        }
    }
    }
    applyMounts();
    return true;
}

bool Assembly::update(float dt, std::ostream& out) {
    if (faulted_)
        return false;
    try {
        require(std::isfinite(dt) && dt >= 0 && dt <= .25f, "dt는 0~.25초여야 합니다.");
        for (const auto& binding : state_.bindings) {
            if (!binding.hold || !binding.down)
                continue;
            if (binding.tokens[0] == "move")
                move(binding.tokens[1], vector(binding.tokens, 2) * dt);
            else
                rotate(binding.tokens[1], axis(binding.tokens[2]), float(number(binding.tokens[3])) * dt);
        }
        if (!state_.paused)
            for (auto& behavior : state_.behaviors)
                if (behavior.playing && behavior.enabled)
                    behavior.seconds += dt;
        return applyPoses(dt);
    } catch (const std::exception& error) {
        faulted_ = true;
        state_.paused = true;
        state_.dragging = false; state_.dragX.reset();
        for (auto& binding : state_.bindings)
            binding.down = false;
        out << "업데이트 정지: " << error.what() << "\nremove <동작> 또는 clear로 조립을 수정하세요. guide physics\n";
        return false;
    }
}

bool Assembly::render() {
    if (!state_.threeViews)
        return app_.renderScene();
    const auto size = app_.getFramebufferSize();
    return size[0] < 3 || size[1] < 2 || app_.renderViews(graphics::PatternSystem::threeViews(
        app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
} // namespace assembly
```

<a id="file-24"></a>
## standard/modes/AssemblySurface.cpp

변경 이유: 면 좌표 관절, 원시 삼각형, 노멀·winding·정적 비균일/반사를 구현한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include <algorithm>
#include <sstream>

namespace assembly {
namespace {
glm::mat3 frame(glm::vec3 forward, glm::vec3 up) {
    require(graphics::detail::finiteVector(forward) && graphics::detail::finiteVector(up) &&
            glm::length(forward) > .0001f && glm::length(up) > .0001f, "0이 아닌 유한한 방향이 필요합니다.");
    const auto z = glm::normalize(forward), cross = glm::cross(up, z);
    require(glm::length(cross) > .0001f, "normal과 up은 평행할 수 없습니다.");
    const auto x = glm::normalize(cross);
    return glm::mat3(x, glm::cross(z, x), z);
}
}

graphics::EntityId Assembly::meshEntity(const std::string& name) const {
    const auto& object = node(name);
    require(object.mesh.has_value(), "메시가 있는 객체/부품을 지정하세요.");
    return *object.mesh;
}

void Assembly::requireGeometryEditable(graphics::EntityId mesh) const {
    for (const auto& b : state_.behaviors) {
        if (b.effect != Effect::Body && b.effect != Effect::Walk) continue;
        require(node(b.reference).mesh != mesh && (b.effect != Effect::Body || node(b.target).mesh != mesh),
                "물리에 사용 중인 형상은 편집할 수 없습니다. 물리를 remove한 뒤 재구성하세요.");
    }
}

void Assembly::requireFaceEditable(graphics::EntityId mesh, std::size_t face) const {
    requireGeometryEditable(mesh);
    for (const auto& b : state_.behaviors)
        require(!faceEffect(b.effect) || node(b.target).mesh != mesh || b.faceIndex != face,
                "해당 면의 동작을 remove한 뒤 기준 형상을 편집하세요.");
}

void Assembly::model(const std::string& name, const graphics::Model& geometry, glm::vec3 position) {
    require(graphics::DiagramSystem::validPolyhedron(geometry), "삼각형·색·노멀·인덱스를 갖춘 유효한 모델이 필요합니다.");
    group(name, position);
    const auto mesh = app_.getScene().push_entity(geometry);
    require(mesh.has_value(), "사용자 모델 생성 실패.");
    require(app_.getScene().setParent(*mesh, node(name).id), "사용자 모델 연결 실패.");
    node(name).mesh = *mesh;
    node(name).guideTopic = "geometry";
    registerNode(name + ".mesh", *mesh, false, "mesh", *mesh);
}

std::string Assembly::mount(const std::string& parent, const std::string& localName, FaceMount attachment) {
    attachment.mesh = meshEntity(parent);
    const auto* mesh = app_.getScene().getMesh(attachment.mesh, attachment.face);
    require(mesh != nullptr, "부착할 면 번호를 확인하세요. surface <객체> <면>");
    const auto indices = graphics::detail::triangles(*mesh);
    require(attachment.triangle < indices.size() / 3, "부착 삼각형 번호가 범위를 벗어났습니다.");
    if (attachment.barycentric)
        require(attachment.coordinate.x >= 0 && attachment.coordinate.y >= 0 &&
                attachment.coordinate.x + attachment.coordinate.y <= 1, "삼각형 좌표는 U,V>=0, U+V<=1입니다.");
    require(graphics::detail::finiteVector(attachment.coordinate), "유한한 부착 좌표가 필요합니다.");
    const auto name = part("joint", parent, localName, {0, 0, 0});
    require(app_.getScene().setParent(node(name).id, attachment.mesh), "면 부착 부모 연결 실패.");
    node(name).mount = attachment;
    return name;
}

void Assembly::applyMounts() {
    auto& scene = app_.getScene();
    std::vector<std::pair<std::size_t, std::string>> ordered;
    for (const auto& item : state_.nodes) {
        if (!item.second.mount) continue;
        std::size_t depth = 0;
        auto id = scene.getComponent(item.second.id)->parent;
        while (id) { ++depth; id = scene.getComponent(*id)->parent; }
        ordered.emplace_back(depth, item.first);
    }
    std::sort(ordered.begin(), ordered.end());
    for (const auto& item : ordered) {
        const auto& object = node(item.second);
        const auto& a = *object.mount;
        const auto* mesh = scene.getMesh(a.mesh, a.face);
        require(mesh != nullptr, "부착한 면이 없습니다.");
        const auto indices = graphics::detail::triangles(*mesh);
        require(a.triangle < indices.size() / 3, "부착 삼각형이 변경됐습니다.");
        const auto p0 = mesh->localVertices[indices[a.triangle * 3]],
                   p1 = mesh->localVertices[indices[a.triangle * 3 + 1]],
                   p2 = mesh->localVertices[indices[a.triangle * 3 + 2]];
        const auto world = scene.worldMatrix(a.mesh), faceWorld = scene.faceWorldMatrix(a.mesh, a.face);
        require(world && faceWorld, "부착할 면의 행렬이 없습니다.");
        const auto inverse = glm::inverse(*world);
        require(graphics::TransformSystem::validAffine(inverse), "특이한 부모에는 관절을 부착할 수 없습니다.");
        const auto local = inverse * *faceWorld;
        const auto edge = glm::normalize(glm::vec3(local * glm::vec4(p1 - p0, 0)));
        const auto normal = glm::cross(edge, glm::normalize(glm::vec3(local * glm::vec4(p2 - p0, 0))));
        const auto rotation = frame(normal, edge);
        const auto point = a.barycentric ? p0 + (p1 - p0) * a.coordinate.x + (p2 - p0) * a.coordinate.y : a.coordinate;
        const auto anchor = glm::vec3(local * glm::vec4(point, 1)) + (a.barycentric ? rotation[2] * a.coordinate.z : glm::vec3(0));
        const auto pose = glm::translate(glm::mat4(1), anchor) * glm::mat4(rotation) *
            graphics::DiagramSystem::modelMatrix(scene.getComponent(object.id)->transform);
        auto result = object.rest;
        result.localBasis = {0, 0, 0};
        result.basis = glm::vec3(pose[3]);
        result.rotation = glm::mat3(pose) / result.scale.x;
        require(scene.setTransform(object.id, result), "면 관절 자세 합성 실패.");
    }
}

bool Assembly::surfaceCommand(const Tokens& a, std::ostream& out) {
    const auto& command = a[0];
    if (command == "warp") {
        require(a.size() == 5, "warp <group> <SX> <SY> <SZ>. 자식 group/joint에 동작을 붙이세요.");
        auto& object = node(a[1]);
        require(object.kind == "group" && !object.mount, "warp는 일반 group에 적용합니다.");
        for (const auto& b : state_.behaviors) require(b.target != a[1], "동작이 없는 외곽 group을 warp로 사용하세요.");
        object.affineGroup = true;
        auto rest = object.rest; rest.scale = vector(a, 2); setRest(a[1], rest);
        return true;
    }
    if (command == "orient") {
        require(a.size() == 8, "orient <대상> <+Z 방향3> <up 방향3>");
        auto rest = node(a[1]).rest; rest.rotation = frame(vector(a, 2), vector(a, 5)); setRest(a[1], rest);
        return true;
    }
    if (command == "mount" || command == "anchor") {
        require(command == "mount" ? a.size() == 8 : a.size() == 7,
                "mount <객체> <관절이름> <면> <삼각형> <U> <V> <법선거리> 또는 anchor <객체> <관절이름> <면> <모델좌표3>");
        FaceMount location;
        location.face = indexNumber(a[3]); location.barycentric = command == "mount";
        location.triangle = command == "mount" ? indexNumber(a[4]) : 0;
        location.coordinate = vector(a, command == "mount" ? 5 : 4);
        mount(a[1], a[2], location);
        return true;
    }
    if (command == "unmount") {
        require(a.size() == 2 && node(a[1]).mount.has_value(), "unmount <면 관절>. 부모 로컬 기준 자세로 돌아갑니다.");
        node(a[1]).mount.reset();
        return true;
    }
    if (command == "triangle" || command == "triangle-add") {
        require(a.size() == 11, "triangle 또는 triangle-add <이름> <A3> <B3> <C3>");
        graphics::Mesh mesh;
        mesh.shape = graphics::GLFace::Triangle;
        mesh.localVertices = {vector(a, 2), vector(a, 5), vector(a, 8)};
        graphics::recalculateNormals(mesh);
        graphics::Model geometry;
        if (command == "triangle-add") {
            const auto id = meshEntity(a[1]); requireGeometryEditable(id);
            geometry = app_.getScene().getComponent(id)->polyhedron;
            geometry.faces.push_back(mesh);
            require(app_.getScene().setModel(id, geometry), "삼각형 추가 실패.");
        } else { geometry.faces.push_back(mesh); model(a[1], geometry); }
        return true;
    }
    if (command == "line" || command == "digit") {
        require(command == "line" ? a.size() == 11 : a.size() == 10,
                "line <부모> <A3> <B3> <RGB> 또는 digit <부모> <0~9> <위치3> <높이> <RGB>");
        const auto color = vector(a, command == "line" ? 8 : 7);
        require(glm::all(glm::greaterThanEqual(color, glm::vec3(0))) && glm::all(glm::lessThanEqual(color, glm::vec3(1))), "색은 0~1입니다.");
        if (command == "line") require(app_.getScene().addLine(vector(a, 2), vector(a, 5),
            {color.x, color.y, color.z}, node(a[1]).id).has_value(), "선 생성 실패.");
        else {
            const auto digit = indexNumber(a[2]); require(digit <= 9, "숫자는 0~9입니다.");
            require(graphics::PatternSystem::addNumber(app_.getScene(), unsigned(digit), vector(a, 3), float(number(a[6])),
                {color.x, color.y, color.z}, node(a[1]).id), "숫자 생성 실패.");
        }
        return true;
    }
    const std::vector<std::string> supported{"surface", "normal", "winding", "vertex", "face-srt", "face-mode", "facecolor", "outline", "cull", "visible"};
    if (std::find(supported.begin(), supported.end(), command) == supported.end()) return false;
    require(a.size() >= 3, command + " <객체> <면/all> ...");
    auto& scene = app_.getScene();
    std::vector<std::pair<graphics::EntityId, std::size_t>> faces;
    if (a[2] == "all") {
        const auto root = node(a[1]).id;
        for (auto mesh : scene.getIds()) {
            std::optional<graphics::EntityId> ancestor = mesh;
            while (ancestor && *ancestor != root) ancestor = scene.getComponent(*ancestor)->parent;
            if (!ancestor) continue;
            for (std::size_t i = 0; i < scene.getComponent(mesh)->polyhedron.faces.size(); ++i) faces.emplace_back(mesh, i);
        }
    } else {
        const auto id = meshEntity(a[1]), face = indexNumber(a[2]);
        require(scene.getMesh(id, face) != nullptr, "면 번호 범위를 확인하세요."); faces.emplace_back(id, face);
    }
    require(!faces.empty(), "편집할 면이 없습니다.");
    for (const auto& location : faces) {
        const auto id = location.first, f = location.second;
        auto mesh = *scene.getMesh(id, f);
        if (command == "surface") {
            require(a.size() == 3, "surface <객체> <면/all>");
            out << a[1] << " mesh=" << id << " face=" << f << " triangles=" << graphics::detail::triangles(mesh).size() / 3 << '\n';
            for (std::size_t i = 0; i < mesh.localVertices.size(); ++i) {
                const auto p = mesh.localVertices[i]; out << "  v" << i << '=' << p.x << ',' << p.y << ',' << p.z << '\n';
            }
            const auto triangles = graphics::detail::triangles(mesh);
            for (std::size_t i = 0; i < triangles.size(); i += 3)
                out << "  triangle " << i / 3 << ": " << triangles[i] << ',' << triangles[i + 1] << ',' << triangles[i + 2] << '\n';
            if (!mesh.vertexNormals.empty()) {
                const auto n = mesh.vertexNormals.front(); out << "  first shading normal=" << n.x << ',' << n.y << ',' << n.z << '\n';
            }
            continue;
        }
        if (command == "cull" || command == "visible") {
            require(a.size() == 4, "cull/visible <객체> <면/all> <값>");
            if (command == "visible") {
                for (const auto& b : state_.behaviors)
                    require(!faceEffect(b.effect) || node(b.target).mesh != id || b.faceIndex != f,
                            "면 동작을 remove한 뒤 가시성을 변경하세요.");
                require(scene.setFaceVisible(id, f, boolean(a[3])), "면 가시성 변경 실패.");
            } else {
                require(a[3] == "inherit" || a[3] == "none" || a[3] == "back" || a[3] == "front", "컬링은 inherit/none/back/front입니다.");
                require(scene.setFaceCullMode(id, f, a[3] == "inherit" ? graphics::CullMode::Inherit : a[3] == "none" ? graphics::CullMode::None : a[3] == "back" ? graphics::CullMode::Back : graphics::CullMode::Front), "면 컬링 변경 실패.");
            }
        } else if (command == "normal") {
            require(a.size() == 6 || (a.size() == 4 && (a[3] == "auto" || a[3] == "flip")),
                    "normal <객체> <면/all> <NX NY NZ 또는 auto/flip>");
            if (a.size() == 6) {
                const auto n = vector(a, 3); require(glm::length(n) > .0001f, "0 노멀은 허용하지 않습니다.");
                mesh.normal = glm::normalize(n); mesh.vertexNormals.assign(mesh.localVertices.size(), mesh.normal);
            } else {
                if (a[3] == "auto" || mesh.vertexNormals.empty()) graphics::recalculateNormals(mesh);
                if (a[3] == "flip") { for (auto& n : mesh.vertexNormals) n = -n; mesh.normal = -mesh.normal; }
            }
            require(scene.setMesh(id, f, mesh), "노멀 변경 실패.");
        } else if (command == "facecolor") {
            require(a.size() == 6, "facecolor <객체> <면/all> <R G B>");
            const auto color = vector(a, 3);
            require(glm::all(glm::greaterThanEqual(color, glm::vec3(0))) && glm::all(glm::lessThanEqual(color, glm::vec3(1))), "색은 0~1입니다.");
            require(scene.setFaceVertexColors(id, f, std::vector<graphics::GlColor>(mesh.localVertices.size(), {color.x, color.y, color.z})), "면 색 변경 실패.");
        } else if (command == "outline") {
            require(a.size() == 4 && (a[3] == "none" || a[3] == "features" || a[3] == "triangles"), "outline <객체> <면/all> <none/features/triangles>");
            require(scene.setOutline(id, f, a[3] == "none" ? graphics::OutlineMode::None : a[3] == "features" ? graphics::OutlineMode::FeatureEdges : graphics::OutlineMode::AllTriangleEdges), "윤곽 변경 실패.");
        } else {
            requireFaceEditable(id, f);
            if (command == "winding") {
                require(a.size() == 4 && a[3] == "flip", "winding <객체> <면/all> flip. 조명 노멀은 별도입니다.");
                mesh.triangleIndices = graphics::detail::triangles(mesh);
                for (std::size_t i = 0; i < mesh.triangleIndices.size(); i += 3) std::swap(mesh.triangleIndices[i + 1], mesh.triangleIndices[i + 2]);
                require(scene.setMesh(id, f, mesh), "winding 변경 실패.");
            } else if (command == "vertex") {
                require(a.size() == 7 && faces.size() == 1, "vertex <객체> <면> <정점> <XYZ>");
                require(scene.setVertexPosition(id, f, indexNumber(a[3]), vector(a, 4)), "정점 변경 실패.");
            } else if (command == "face-mode") {
                require(a.size() == 4 && (a[3] == "hierarchy" || a[3] == "independent"), "face-mode <객체> <면/all> <hierarchy/independent>");
                require(scene.setFaceTransformMode(id, f, a[3] == "hierarchy" ? graphics::FaceTransformMode::Hierarchy : graphics::FaceTransformMode::Independent, true), "면 종속 변경 실패.");
            } else {
                require(a.size() == 15, "face-srt <객체> <면/all> <이동3> <회전도3> <배율3> <중심3>");
                graphics::Transform3D t;
                t.basis = vector(a, 3); t.scale = vector(a, 9); t.localBasis = vector(a, 12);
                const auto r = glm::radians(vector(a, 6));
                t.rotation = glm::mat3(glm::rotate(glm::mat4(1), r.z, glm::vec3(0, 0, 1)) * glm::rotate(glm::mat4(1), r.y, glm::vec3(0, 1, 0)) * glm::rotate(glm::mat4(1), r.x, glm::vec3(1, 0, 0)));
                require(glm::all(glm::greaterThan(glm::abs(t.scale), glm::vec3(.0001f))) && glm::all(glm::lessThanEqual(glm::abs(t.scale), glm::vec3(100))), "면 배율은 0이 아니며 절댓값 .0001~100입니다.");
                require(scene.setFaceSRT(id, f, t), "면 SRT 변경 실패.");
            }
        }
    }
    return true;
}
} // namespace assembly
```

<a id="file-25"></a>
## standard/modes/AssemblyTracks.cpp

변경 이유: 지연과 반복을 가진 면 이동·축소·임의 중심 회전 트랙을 구성한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include <algorithm>

namespace assembly {
bool Assembly::trackCommand(const Tokens& a, std::ostream&) {
    if (a[0] != "face-track") return false;
    require(a.size() >= 8, "face-track <ID> <객체> <면> <시간> <지연> <once/loop/pingpong> <move/scale/turn> ...");
    const auto duration = number(a[4]), delay = number(a[5]);
    require(duration >= .0001 && delay >= 0, ".0001초 이상 시간과 0 이상 지연이 필요합니다.");
    require(a[6] == "once" || a[6] == "loop" || a[6] == "pingpong", "재생은 once/loop/pingpong입니다.");
    const auto operation = a[7], playback = a[6];
    glm::vec3 value(0), pivot(0);
    double degrees = 0;
    if (operation == "move") {
        require(a.size() == 11, "face-track ... move <DX DY DZ>");
        value = vector(a, 8);
    } else if (operation == "scale") {
        require(a.size() == 14, "face-track ... scale <SX SY SZ> <CX CY CZ>");
        value = vector(a, 8); pivot = vector(a, 11);
        require(glm::all(glm::greaterThan(value, glm::vec3(.0001f))) && glm::all(glm::lessThanEqual(value, glm::vec3(100))),
                "애니메이션 배율은 양수 .0001~100입니다. 완전 개방은 visible off로 처리하세요.");
    } else {
        require(operation == "turn" && a.size() == 15, "face-track ... turn <축3> <중심3> <도>");
        value = vector(a, 8); pivot = vector(a, 11); degrees = number(a[14]);
        require(glm::length(value) > .0001f, "회전축은 0일 수 없습니다.");
        value = glm::normalize(value);
    }
    graphics::MotionStep step;
    step.startSeconds = delay;
    step.space = graphics::TransformSpace::World;
    step.transform = [=](double time) {
        double u = time / duration;
        if (playback == "once") u = std::min(u, 1.0);
        else if (playback == "loop") u = std::fmod(u, 1.0);
        else { u = std::fmod(u, 2.0); if (u > 1) u = 2 - u; }
        if (operation == "move") return glm::translate(glm::mat4(1), value * float(u));
        if (operation == "scale") return *graphics::TransformSystem::scaling(glm::mix(glm::vec3(1), value, float(u)), pivot);
        return *graphics::TransformSystem::rotation({pivot, value}, degrees * u);
    };
    Behavior behavior;
    behavior.name = a[1]; behavior.target = a[2]; behavior.faceIndex = indexNumber(a[3]);
    behavior.effect = Effect::FaceTrack;
    require(behavior.motion.append(step), "면 트랙 생성 실패.");
    attach(std::move(behavior));
    return true;
}
} // namespace assembly
```

<a id="file-26"></a>
## standard/modes/Mode0Assembly.cpp

변경 이유: 모드0 콜백과 조립 상태·콘솔 입력을 한 곳에서 소유한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"
#include "AssemblyConsole.h"
#include <iostream>

namespace {
class AssemblyMode final : public PracticeMode {
    graphics::Application& app_;
    assembly::Assembly scene_;
    assembly::ConsoleInput input_;
public:
    explicit AssemblyMode(graphics::Application& app) : PracticeMode(app), app_(app), scene_(app) {}
    bool initScene() override {
        if (!assembly::defaultRecipe(scene_, std::cout))
            std::cout << "기본 레시피가 중단됐습니다. 콘솔에서 수정하거나 guide recipe를 사용하세요.\n";
        scene_.help(std::cout);
        input_.prompt();
        return true;
    }
    void onKey(int key, int action, int mods) override {
        if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
            assembly::defaultRecipe(scene_, std::cout);
            return;
        }
        scene_.onKey(key, action, mods, std::cout);
    }
    void onMouse(const graphics::MouseEvent& event) override { scene_.onMouse(event, std::cout); }
    void onCursor(double x, double y) override { scene_.onCursor(x, y, std::cout); }
    void onFocus(bool focused) override { scene_.onFocus(focused); }
    void onUpdate(float dt) override {
        for (unsigned i = 0; i < 8; ++i) {
            const auto line = input_.poll();
            if (!line) break;
            if (*line == "quit") {
                app_.requestClose();
                return;
            }
            scene_.execute(*line, std::cout);
            input_.prompt();
        }
        scene_.update(dt, std::cout);
    }
    bool onRender() override { return scene_.render(); }
};
}

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app) {
    return std::make_unique<AssemblyMode>(app);
}
```

<a id="file-27"></a>
## standard/modes/Mode0Recipe.cpp

변경 이유: 초기 구성을 사용자가 직접 편집하는 C++ 명령 목록으로 분리한다.

### before

없음. 새 파일이다.

### after

```cpp
#include "Assembly.h"

namespace assembly {
bool defaultRecipe(Assembly& scene, std::ostream& out) {
    scene.clear();
    // USER_RECIPE_BEGIN
    const char* commands[] = {
        "camera pose 16 11 20 0 1 1 0 1 0",
        "camera fov 55",
        "add grid floor 0 0 0",
        "build person hero",
        "move hero -2 0 0",
        "path stroll hero 5 8",
        "add swing ride 2 0 0",
        "build tank rover",
        "move rover 5 0 0",
        "color rover 0.3 0.6 0.8",
        "add room lab -6 0 0",
        "add box cargo -6 1 0 0.5 0.5 0.5",
        "physics falling cargo lab",
        "add walker robot -5.5 0 -0.5",
        "walk walking robot lab 0.7",
        "obstacle walking cargo",
        "bind RIGHT hold \"move hero 1 0 0\"",
        "bind LEFT hold \"move hero -1 0 0\"",
        "bind J press \"jump walking 5.5\"",
        "bind F press \"visible lab 5 off\"",
        "bind G press \"visible lab 5 on\"",
        "select hero"
    };
    // USER_RECIPE_END
    for (const auto* command : commands)
        if (!scene.execute(command, out)) return false;
    return true;
}
} // namespace assembly
```

<a id="file-28"></a>
## standard/modes/Mode7Grid.cpp

변경 이유: 기존 격자 동작을 유지하고 공통 부품 생성기를 사용한다.

### before

```cpp
#include "../Console.h"

/// 거리 기반 격자 예제를 구성한다.
bool Console::initGrid() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Grid;
    clock_.reset();
    gridBoxes_.clear();

    graphics::GridLayout grid;

    grid.origin = {-3, -.1f, -2};
    grid.extent = {6, .2f, 4};
    grid.cells = {6, 1, 4};

    for (int z = 0; z < 4; ++z)
        for (int x = 0; x < 6; ++x) {
            const auto center = grid.cellCenter({x, 0, z}), size = grid.relativeSize({.15f, 1, .22f});

            if (!center || !size)
                return false;
            const auto tile = graphics::PatternSystem::addBox(scene, *size, *center,
                (x + z) % 2 ? graphics::GlColor{.25f, .3f, .4f} : graphics::GlColor{.4f, .45f, .55f});
            if (!tile || !addControl(*tile, "grid tile " + std::to_string(z * 6 + x + 1)))
                return false;
        }

    if (!distancePath_.setPoints({{-2.5f, .35f, -1.5f}, {2.5f, .35f, -1.5f}, {2.5f, .35f, 1.5f}}))
        return false;

    for (unsigned i = 0; i < 5; ++i) {
        const auto frame = scene.addGroup();
        if (!frame)
            return false;
        const auto box = graphics::PatternSystem::addBox(scene, {.4f, .5f, .4f}, {0, 0, 0},
            {.25f + .14f * i, .7f - .1f * i, .3f}, *frame);

        if (!box || !graphics::PatternSystem::addNumber(scene, i + 1, {-.07f, -.1f, .205f}, .2f, {1, 1, 1}, *box))
            return false;
        if (!addControl(*box, "moving box " + std::to_string(i + 1)))
            return false;
        gridBoxes_.push_back(*frame);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {7, 6, 8};
    app_.getCamera().target = {0, 0, 0};
    app_.getCamera().orthographicHalfExtent = 4;

    selectedJoint_ = 24; // 처음에는 첫 이동 상자를 조작한다.
    return true;
}

bool Console::updateGrid() {
    auto& scene = app_.getScene();
    bool ok = true;
    for (std::size_t i = 0; i < gridBoxes_.size(); ++i)
        ok = graphics::PatternSystem::follow(scene, gridBoxes_[i], distancePath_,
            clock_.seconds + distancePath_.length() * i / gridBoxes_.size()) && ok;
    return ok;
}
```

### after

```cpp
#include "../Console.h"
#include "AssemblyParts.h"

/// 거리 기반 격자 예제를 구성한다.
bool Console::initGrid() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Grid;
    clock_.reset();
    gridBoxes_.clear();

    graphics::GridLayout grid;

    grid.origin = {-3, -.1f, -2};
    grid.extent = {6, .2f, 4};
    grid.cells = {6, 1, 4};

    const auto cells = assembly::addGridParts(scene, grid);
    for (std::size_t i = 0; i < cells.size(); ++i)
        if (!addControl(cells[i], "grid tile " + std::to_string(i + 1)))
            return false;

    if (!distancePath_.setPoints({{-2.5f, .35f, -1.5f}, {2.5f, .35f, -1.5f}, {2.5f, .35f, 1.5f}}))
        return false;

    for (unsigned i = 0; i < 5; ++i) {
        const auto frame = scene.addGroup();
        if (!frame)
            return false;
        const auto box = graphics::PatternSystem::addBox(scene, {.4f, .5f, .4f}, {0, 0, 0},
            {.25f + .14f * i, .7f - .1f * i, .3f}, *frame);

        if (!box || !graphics::PatternSystem::addNumber(scene, i + 1, {-.07f, -.1f, .205f}, .2f, {1, 1, 1}, *box))
            return false;
        if (!addControl(*box, "moving box " + std::to_string(i + 1)))
            return false;
        gridBoxes_.push_back(*frame);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {7, 6, 8};
    app_.getCamera().target = {0, 0, 0};
    app_.getCamera().orthographicHalfExtent = 4;

    selectedJoint_ = 24; // 처음에는 첫 이동 상자를 조작한다.
    return true;
}

bool Console::updateGrid() {
    auto& scene = app_.getScene();
    bool ok = true;
    for (std::size_t i = 0; i < gridBoxes_.size(); ++i)
        ok = graphics::PatternSystem::follow(scene, gridBoxes_[i], distancePath_,
            clock_.seconds + distancePath_.length() * i / gridBoxes_.size()) && ok;
    return ok;
}
```

<a id="file-29"></a>
## standard/modes/PracticeMode.h

변경 이유: 조립 모드의 팩토리를 선언한다.

### before

```cpp
#pragma once
#include "../framework/Framework.h"
#include <memory>

// Each practice owns its state; Console alone owns callback registration and switching.
class PracticeMode {
    graphics::Application& host_;
public:
    explicit PracticeMode(graphics::Application& app) : host_(app) {}
    virtual ~PracticeMode() = default;
    virtual bool initScene() = 0;
    virtual void onKey(int, int, int) = 0;
    virtual void onUpdate(float) {}
    virtual void onMouse(const graphics::MouseEvent&) {}
    virtual void onCursor(double, double) {}
    virtual void onFocus(bool) {}
    virtual bool onRender() { return host_.renderScene(); }
};

constexpr int lastStandardMode = 22;
std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice20(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice21(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app);
std::unique_ptr<PracticeMode> makeCameraCollision(graphics::Application& app);
std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app);
```

### after

```cpp
#pragma once
#include "../framework/Framework.h"
#include <memory>

// Each practice owns its state; Console alone owns callback registration and switching.
class PracticeMode {
    graphics::Application& host_;
public:
    explicit PracticeMode(graphics::Application& app) : host_(app) {}
    virtual ~PracticeMode() = default;
    virtual bool initScene() = 0;
    virtual void onKey(int, int, int) = 0;
    virtual void onUpdate(float) {}
    virtual void onMouse(const graphics::MouseEvent&) {}
    virtual void onCursor(double, double) {}
    virtual void onFocus(bool) {}
    virtual bool onRender() { return host_.renderScene(); }
};

constexpr int lastStandardMode = 22;
std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice20(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice21(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app);
std::unique_ptr<PracticeMode> makeCameraCollision(graphics::Application& app);
std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app);

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app);
```

<a id="file-30"></a>
## standard/modes/PracticeRegistry.cpp

변경 이유: 0번을 조립 팩토리로 연결한다.

### before

```cpp
#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app) {
    switch (mode) {
    case 10: return makePractice13(app);
    case 11: return makePractice14(app);
    case 12: return makePractice15(app);
    case 13: return makePractice16(app);
    case 14: return makePractice17(app);
    case 15: return makePractice18(app);
    case 16: return makePractice19(app);
    case 17: return makePractice20(app);
    case 18: return makePractice21(app);
    case 19: return makePractice22(app);
    case 20: return makePractice23(app);
    case 21: return makeCameraCollision(app);
    case 22: return makeEmbeddedMesh(app);
    default: return {};
    }
}
```

### after

```cpp
#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app) {
    switch (mode) {
    case 0: return makeAssemblyMode(app);
    case 10: return makePractice13(app);
    case 11: return makePractice14(app);
    case 12: return makePractice15(app);
    case 13: return makePractice16(app);
    case 14: return makePractice17(app);
    case 15: return makePractice18(app);
    case 16: return makePractice19(app);
    case 17: return makePractice20(app);
    case 18: return makePractice21(app);
    case 19: return makePractice22(app);
    case 20: return makePractice23(app);
    case 21: return makeCameraCollision(app);
    case 22: return makeEmbeddedMesh(app);
    default: return {};
    }
}
```

<a id="file-31"></a>
## standard/modes/TankCommon.cpp

변경 이유: 기존 탱크 크기·색 순서·제어 ID를 유지하고 공통 부품 생성기를 사용한다.

### before

```cpp
#include "TankCommon.h"
#include "PracticeCommon.h"
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
class TankMode final : public PracticeMode {
    graphics::Application& app_;
    bool threeViews_;
    graphics::EntityId root_ = 0, central_ = 0;
    std::array<graphics::EntityId, 2> lower_{}, upper_{}, guns_{}, poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_ = 0, centralAngle_ = 0, gunAngle_ = 0, poleTime_ = 0;
    float swapProgress_ = 0, swapDirection_ = 1;
    float cameraX_ = 0, cameraZ_ = 0, cameraYaw_ = 0, cameraOrbit_ = 0;
    bool lowerRunning_ = false, centralRunning_ = false, gunRunning_ = false, poleRunning_ = false;
    bool swapRunning_ = false, swapStarted_ = false;

    unsigned colorVersion_ = 0;

    graphics::EntityId box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent);

    void reset();
    void apply();
    void recolor();

public:
    TankMode(graphics::Application& app, bool threeViews) : PracticeMode(app), app_(app), threeViews_(threeViews) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float dt) override;
    void onFocus(bool focused) override;

    bool onRender() override;
};

using practice_common::requireEntity;

namespace {

}

graphics::EntityId TankMode::box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent) {
    const auto id = requireEntity(graphics::PatternSystem::addBox(app_.getScene(), size, position, {1, 1, 1}, parent),
                                  "탱크 상자 생성 실패: 크기·위치·부모 ID를 확인하세요.");

    colored_.push_back(id);

    return id;
}

bool TankMode::initScene() {
    reset();
    std::cout << (threeViews_ ? "실습 22: 원근 / XZ 평면도 / XY 정면도\n" : "실습 19: 탱크와 부품·카메라 조작\n")
              << "방향키 이동; d 하부 높이; t 중앙 자전; l 상부 교환\n"
              << "g 포신 반대 회전; p 깃대 반대 기울기; x/X,z/Z 카메라 이동\n"
              << "y/Y 카메라 시선 회전; r/R 공전(재입력 정지); O/o 색 변경; c 초기화; q 종료\n";

    return true;
}

void TankMode::reset() {
    auto& scene = app_.getScene();

    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_ = glm::vec3(0);
    lowerTime_ = centralAngle_ = gunAngle_ = poleTime_ = swapProgress_ = 0;
    swapDirection_ = 1;
    cameraX_ = cameraZ_ = cameraYaw_ = cameraOrbit_ = 0;
    lowerRunning_ = centralRunning_ = gunRunning_ = poleRunning_ = swapRunning_ = swapStarted_ = false;
    colorVersion_ = 0;
    root_ = requireEntity(scene.addGroup(), "탱크 루트 생성 실패.");
    central_ = requireEntity(graphics::PatternSystem::addJoint(scene, root_, {0, .6f, 0}), "중앙 관절 생성 실패.");

    const auto ground = requireEntity(scene.addGroup(), "바닥 그룹 생성 실패.");

    box({12, .1f, 12}, {0, -.05f, 0}, ground);

    for (std::size_t i = 0; i < 2; ++i)
        lower_[i] = box({.65f, .6f, 1.8f}, {i == 0 ? -.8f : .8f, .3f, 0}, root_);
    box({2.3f, .25f, 1.4f}, {0, .125f, 0}, central_);

    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;

        upper_[i] = requireEntity(graphics::PatternSystem::addJoint(scene, central_, {sign * .65f, .25f, 0}),
                                  "상부 관절 생성 실패.");
        guns_[i] = requireEntity(graphics::PatternSystem::addJoint(scene, upper_[i], {0, .3f, 0}),
                                 "포신 중심 피벗 생성 실패.");
        poles_[i] = requireEntity(graphics::PatternSystem::addJoint(scene, upper_[i], {0, .6f, 0}),
                                  "깃대 밑점 피벗 생성 실패.");
        box({.65f, .6f, .8f}, {0, .3f, 0}, upper_[i]);

        box({.14f, .14f, 1.1f}, {0, -.05f, .95f}, guns_[i]);
        box({.08f, .85f, .08f}, {0, .425f, 0}, poles_[i]);
    }

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {-5, 4.5f, 7};
    camera.target = {0, .7f, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 6.5f;
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().lighting = true;
    recolor();
    apply();
}

void TankMode::recolor() {
    const std::array<graphics::GlColor, 10> palette{graphics::GlColor{.3f, .35f, .4f},
                                                    {.25f, .6f, .3f},
                                                    {.55f, .35f, .2f},
                                                    {.2f, .55f, .75f},
                                                    {.7f, .75f, .2f},
                                                    {.9f, .4f, .15f},
                                                    {1, .8f, .2f},
                                                    {.4f, .7f, .8f},
                                                    {.7f, .25f, .6f},
                                                    {.6f, .9f, .4f}};

    for (std::size_t i = 0; i < colored_.size(); ++i)
        if (!app_.getScene().setTint(colored_[i], palette[(i + colorVersion_) % palette.size()]))
            throw std::runtime_error("탱크 부품 색상 적용 실패.");
}

void TankMode::apply() {
    auto& scene = app_.getScene();

    if (!scene.setPosition(root_, position_))
        throw std::runtime_error("탱크 위치 적용 실패.");

    const float factor = 1 + .25f * std::sin(lowerTime_), height = .6f * factor;

    if (!scene.setPosition(central_, {0, height, 0}) ||
        !graphics::PatternSystem::rotateJoint(scene, central_, {0, 1, 0}, centralAngle_))
        throw std::runtime_error("중앙 관절 변환 적용 실패.");

    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        graphics::Transform3D low;

        low.basis = {sign * .8f, height * .5f, 0};
        low.scale = {1, factor, 1};

        const float a = 3.14159265f * swapProgress_;
        const glm::vec3 upperPosition{sign * .65f * std::cos(a), .25f, sign * .65f * std::sin(a)};

        if (!scene.setTransform(lower_[i], low) || !scene.setPosition(upper_[i], upperPosition) ||
            !graphics::PatternSystem::rotateJoint(scene, guns_[i], {0, 1, 0}, sign * gunAngle_) ||
            !graphics::PatternSystem::rotateJoint(scene, poles_[i], {1, 0, 0}, sign * 60 * std::sin(poleTime_)))
            throw std::runtime_error("탱크 부품 변환 적용 실패.");
    }
}

void TankMode::onKey(int key, int action, int mods) {
    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    if (key == GLFW_KEY_X || key == GLFW_KEY_Z) {
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            float& direction = key == GLFW_KEY_X ? cameraX_ : cameraZ_;

            direction = action == GLFW_RELEASE ? 0 : (upper ? -1.0f : 1.0f);
        }

        return;
    }

    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS)
            pressed_[key] = true;
        else if (action == GLFW_RELEASE)
            pressed_[key] = false;
    }

    if (action != GLFW_PRESS)
        return;

    const float direction = upper ? 1.0f : -1.0f;

    switch (key) {
    case GLFW_KEY_D:
        lowerRunning_ = !lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_ = !centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_ = !gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_ = !poleRunning_;
        break;
    case GLFW_KEY_L:
        if (!swapStarted_) {
            swapStarted_ = true;
            swapRunning_ = true;
        } else if (!swapRunning_ && (swapProgress_ == 0 || swapProgress_ == 1)) {
            swapDirection_ = -swapDirection_;
            swapRunning_ = true;
        } else
            swapRunning_ = !swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_ = cameraYaw_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_ = cameraOrbit_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_O:
        colorVersion_ = (colorVersion_ + 1) % 10;
        recolor();
        break;
    case GLFW_KEY_C:
        reset();
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void TankMode::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT]) - float(pressed_[GLFW_KEY_LEFT]), 0,
                       float(pressed_[GLFW_KEY_DOWN]) - float(pressed_[GLFW_KEY_UP])};

    if (glm::length(movement) > 0)
        position_ += glm::normalize(movement) * dt * 1.3f;

    position_.x = std::clamp(position_.x, -3.5f, 3.5f);
    position_.z = std::clamp(position_.z, -3.5f, 3.5f);

    if (lowerRunning_)
        lowerTime_ = std::fmod(lowerTime_ + dt, 6.283185307f);

    if (centralRunning_)
        centralAngle_ = std::fmod(centralAngle_ + 45 * dt, 360.0f);

    if (gunRunning_)
        gunAngle_ = std::fmod(gunAngle_ + 60 * dt, -360.0f);

    if (poleRunning_)
        poleTime_ = std::fmod(poleTime_ + dt, 6.283185307f);

    if (swapRunning_) {
        swapProgress_ = std::clamp(swapProgress_ + swapDirection_ * dt * .5f, 0.0f, 1.0f);

        if (swapProgress_ == 0 || swapProgress_ == 1)
            swapRunning_ = false;
    }

    auto camera = app_.getCamera();
    const glm::vec3 delta{cameraX_ * 2 * dt, 0, cameraZ_ * 2 * dt};

    if (!graphics::PatternSystem::moveCamera(camera, delta) ||
        (cameraYaw_ != 0 && !graphics::PatternSystem::yawCamera(camera, cameraYaw_ * 35 * dt)) ||
        (cameraOrbit_ != 0 &&
         !graphics::PatternSystem::orbitCamera(camera, {{0, 0, 0}, {0, 1, 0}}, cameraOrbit_ * 25 * dt)))
        throw std::runtime_error("카메라 갱신 실패: 위치·시선·회전축을 확인하세요.");
    app_.getCamera() = camera;
    apply();
}

void TankMode::onFocus(bool focused) {
    if (!focused) {
        pressed_.fill(false);
        cameraX_ = cameraZ_ = 0;
    }
}

bool TankMode::onRender() {
    if (!threeViews_)
        return app_.renderScene();
    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    const auto passes =
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

    if (passes.size() != 3)
        throw std::runtime_error("세 카메라 뷰 생성 실패: 프레임버퍼 크기와 카메라를 확인하세요.");

    return app_.renderViews(passes);
}
}

std::unique_ptr<PracticeMode> makeTankMode(graphics::Application& app, bool threeViews) {
    return std::make_unique<TankMode>(app, threeViews);
}
```

### after

```cpp
#include "TankCommon.h"
#include "AssemblyParts.h"
#include "PracticeCommon.h"
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
class TankMode final : public PracticeMode {
    graphics::Application& app_;
    bool threeViews_;
    graphics::EntityId root_ = 0, central_ = 0;
    std::array<graphics::EntityId, 2> lower_{}, upper_{}, guns_{}, poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_ = 0, centralAngle_ = 0, gunAngle_ = 0, poleTime_ = 0;
    float swapProgress_ = 0, swapDirection_ = 1;
    float cameraX_ = 0, cameraZ_ = 0, cameraYaw_ = 0, cameraOrbit_ = 0;
    bool lowerRunning_ = false, centralRunning_ = false, gunRunning_ = false, poleRunning_ = false;
    bool swapRunning_ = false, swapStarted_ = false;

    unsigned colorVersion_ = 0;

    graphics::EntityId box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent);

    void reset();
    void apply();
    void recolor();

public:
    TankMode(graphics::Application& app, bool threeViews) : PracticeMode(app), app_(app), threeViews_(threeViews) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float dt) override;
    void onFocus(bool focused) override;

    bool onRender() override;
};

using practice_common::requireEntity;

namespace {

}

graphics::EntityId TankMode::box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent) {
    const auto id = requireEntity(graphics::PatternSystem::addBox(app_.getScene(), size, position, {1, 1, 1}, parent),
                                  "탱크 상자 생성 실패: 크기·위치·부모 ID를 확인하세요.");

    colored_.push_back(id);

    return id;
}

bool TankMode::initScene() {
    reset();
    std::cout << (threeViews_ ? "실습 22: 원근 / XZ 평면도 / XY 정면도\n" : "실습 19: 탱크와 부품·카메라 조작\n")
              << "방향키 이동; d 하부 높이; t 중앙 자전; l 상부 교환\n"
              << "g 포신 반대 회전; p 깃대 반대 기울기; x/X,z/Z 카메라 이동\n"
              << "y/Y 카메라 시선 회전; r/R 공전(재입력 정지); O/o 색 변경; c 초기화; q 종료\n";

    return true;
}

void TankMode::reset() {
    auto& scene = app_.getScene();

    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_ = glm::vec3(0);
    lowerTime_ = centralAngle_ = gunAngle_ = poleTime_ = swapProgress_ = 0;
    swapDirection_ = 1;
    cameraX_ = cameraZ_ = cameraYaw_ = cameraOrbit_ = 0;
    lowerRunning_ = centralRunning_ = gunRunning_ = poleRunning_ = swapRunning_ = swapStarted_ = false;
    colorVersion_ = 0;
    const auto ground = requireEntity(scene.addGroup(), "바닥 그룹 생성 실패.");
    box({12, .1f, 12}, {0, -.05f, 0}, ground);
    const auto rig = assembly::addTankParts(scene);
    root_ = rig.root; central_ = rig.central;
    lower_ = rig.lower; upper_ = rig.upper; guns_ = rig.guns; poles_ = rig.poles;
    colored_.insert(colored_.end(), rig.colored.begin(), rig.colored.end());

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {-5, 4.5f, 7};
    camera.target = {0, .7f, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 6.5f;
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().lighting = true;
    recolor();
    apply();
}

void TankMode::recolor() {
    const std::array<graphics::GlColor, 10> palette{graphics::GlColor{.3f, .35f, .4f},
                                                    {.25f, .6f, .3f},
                                                    {.55f, .35f, .2f},
                                                    {.2f, .55f, .75f},
                                                    {.7f, .75f, .2f},
                                                    {.9f, .4f, .15f},
                                                    {1, .8f, .2f},
                                                    {.4f, .7f, .8f},
                                                    {.7f, .25f, .6f},
                                                    {.6f, .9f, .4f}};

    for (std::size_t i = 0; i < colored_.size(); ++i)
        if (!app_.getScene().setTint(colored_[i], palette[(i + colorVersion_) % palette.size()]))
            throw std::runtime_error("탱크 부품 색상 적용 실패.");
}

void TankMode::apply() {
    auto& scene = app_.getScene();

    if (!scene.setPosition(root_, position_))
        throw std::runtime_error("탱크 위치 적용 실패.");

    const float factor = 1 + .25f * std::sin(lowerTime_), height = .6f * factor;

    if (!scene.setPosition(central_, {0, height, 0}) ||
        !graphics::PatternSystem::rotateJoint(scene, central_, {0, 1, 0}, centralAngle_))
        throw std::runtime_error("중앙 관절 변환 적용 실패.");

    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        graphics::Transform3D low;

        low.basis = {sign * .8f, height * .5f, 0};
        low.scale = {1, factor, 1};

        const float a = 3.14159265f * swapProgress_;
        const glm::vec3 upperPosition{sign * .65f * std::cos(a), .25f, sign * .65f * std::sin(a)};

        if (!scene.setTransform(lower_[i], low) || !scene.setPosition(upper_[i], upperPosition) ||
            !graphics::PatternSystem::rotateJoint(scene, guns_[i], {0, 1, 0}, sign * gunAngle_) ||
            !graphics::PatternSystem::rotateJoint(scene, poles_[i], {1, 0, 0}, sign * 60 * std::sin(poleTime_)))
            throw std::runtime_error("탱크 부품 변환 적용 실패.");
    }
}

void TankMode::onKey(int key, int action, int mods) {
    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    if (key == GLFW_KEY_X || key == GLFW_KEY_Z) {
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            float& direction = key == GLFW_KEY_X ? cameraX_ : cameraZ_;

            direction = action == GLFW_RELEASE ? 0 : (upper ? -1.0f : 1.0f);
        }

        return;
    }

    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS)
            pressed_[key] = true;
        else if (action == GLFW_RELEASE)
            pressed_[key] = false;
    }

    if (action != GLFW_PRESS)
        return;

    const float direction = upper ? 1.0f : -1.0f;

    switch (key) {
    case GLFW_KEY_D:
        lowerRunning_ = !lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_ = !centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_ = !gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_ = !poleRunning_;
        break;
    case GLFW_KEY_L:
        if (!swapStarted_) {
            swapStarted_ = true;
            swapRunning_ = true;
        } else if (!swapRunning_ && (swapProgress_ == 0 || swapProgress_ == 1)) {
            swapDirection_ = -swapDirection_;
            swapRunning_ = true;
        } else
            swapRunning_ = !swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_ = cameraYaw_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_ = cameraOrbit_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_O:
        colorVersion_ = (colorVersion_ + 1) % 10;
        recolor();
        break;
    case GLFW_KEY_C:
        reset();
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void TankMode::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT]) - float(pressed_[GLFW_KEY_LEFT]), 0,
                       float(pressed_[GLFW_KEY_DOWN]) - float(pressed_[GLFW_KEY_UP])};

    if (glm::length(movement) > 0)
        position_ += glm::normalize(movement) * dt * 1.3f;

    position_.x = std::clamp(position_.x, -3.5f, 3.5f);
    position_.z = std::clamp(position_.z, -3.5f, 3.5f);

    if (lowerRunning_)
        lowerTime_ = std::fmod(lowerTime_ + dt, 6.283185307f);

    if (centralRunning_)
        centralAngle_ = std::fmod(centralAngle_ + 45 * dt, 360.0f);

    if (gunRunning_)
        gunAngle_ = std::fmod(gunAngle_ + 60 * dt, -360.0f);

    if (poleRunning_)
        poleTime_ = std::fmod(poleTime_ + dt, 6.283185307f);

    if (swapRunning_) {
        swapProgress_ = std::clamp(swapProgress_ + swapDirection_ * dt * .5f, 0.0f, 1.0f);

        if (swapProgress_ == 0 || swapProgress_ == 1)
            swapRunning_ = false;
    }

    auto camera = app_.getCamera();
    const glm::vec3 delta{cameraX_ * 2 * dt, 0, cameraZ_ * 2 * dt};

    if (!graphics::PatternSystem::moveCamera(camera, delta) ||
        (cameraYaw_ != 0 && !graphics::PatternSystem::yawCamera(camera, cameraYaw_ * 35 * dt)) ||
        (cameraOrbit_ != 0 &&
         !graphics::PatternSystem::orbitCamera(camera, {{0, 0, 0}, {0, 1, 0}}, cameraOrbit_ * 25 * dt)))
        throw std::runtime_error("카메라 갱신 실패: 위치·시선·회전축을 확인하세요.");
    app_.getCamera() = camera;
    apply();
}

void TankMode::onFocus(bool focused) {
    if (!focused) {
        pressed_.fill(false);
        cameraX_ = cameraZ_ = 0;
    }
}

bool TankMode::onRender() {
    if (!threeViews_)
        return app_.renderScene();
    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    const auto passes =
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

    if (passes.size() != 3)
        throw std::runtime_error("세 카메라 뷰 생성 실패: 프레임버퍼 크기와 카메라를 확인하세요.");

    return app_.renderViews(passes);
}
}

std::unique_ptr<PracticeMode> makeTankMode(graphics::Application& app, bool threeViews) {
    return std::make_unique<TankMode>(app, threeViews);
}
```

<a id="file-32"></a>
## standard/standard.vcxproj

변경 이유: 새 모드 소스는 기존 wildcard로 컴파일하고 예제·문서를 출력 폴더로 복사한다.

### before

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{AD50DC3F-7731-4954-923F-907BB2A8277C}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <ProjectName>standard</ProjectName>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <Import Project="framework\Toolchain.props" />
  <PropertyGroup Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries Condition="'$(Configuration)'=='Debug'">true</UseDebugLibraries>
    <UseDebugLibraries Condition="'$(Configuration)'=='Release'">false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <OutDir>$(ProjectDir)x64\$(Configuration)\</OutDir>
    <IntDir>$(ProjectDir)x64\$(Configuration)\obj\</IntDir>
    <TargetName>standard</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <WarningLevel>Level4</WarningLevel>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebug</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreaded</RuntimeLibrary>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="modes\*.cpp" />
    <ClInclude Include="modes\*.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Animation.h" />
    <ClInclude Include="framework\include\Transform.h" />
    <ClInclude Include="framework\include\Patterns.h" />
    <ClInclude Include="framework\include\GridLayout.h" />
    <ClInclude Include="framework\include\Diagnostics.h" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
```

### after

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{AD50DC3F-7731-4954-923F-907BB2A8277C}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <ProjectName>standard</ProjectName>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <Import Project="framework\Toolchain.props" />
  <PropertyGroup Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries Condition="'$(Configuration)'=='Debug'">true</UseDebugLibraries>
    <UseDebugLibraries Condition="'$(Configuration)'=='Release'">false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <OutDir>$(ProjectDir)x64\$(Configuration)\</OutDir>
    <IntDir>$(ProjectDir)x64\$(Configuration)\obj\</IntDir>
    <TargetName>standard</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <WarningLevel>Level4</WarningLevel>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebug</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreaded</RuntimeLibrary>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="modes\*.cpp" />
    <ClInclude Include="modes\*.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Animation.h" />
    <ClInclude Include="framework\include\Transform.h" />
    <ClInclude Include="framework\include\Patterns.h" />
    <ClInclude Include="framework\include\GridLayout.h" />
    <ClInclude Include="framework\include\Diagnostics.h" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ItemGroup>
    <AssemblyRecipe Include="examples\assembly\**\*.assembly" />
    <AssemblyDocument Include="docs\assembly\**\*.md" />
  </ItemGroup>
  <Target Name="CopyAssemblyLearningFiles" AfterTargets="Build">
    <Copy SourceFiles="@(AssemblyRecipe)" DestinationFiles="@(AssemblyRecipe->'$(TargetDir)examples\assembly\%(RecursiveDir)%(Filename)%(Extension)')" SkipUnchangedFiles="true" />
    <Copy SourceFiles="@(AssemblyDocument)" DestinationFiles="@(AssemblyDocument->'$(TargetDir)docs\assembly\%(RecursiveDir)%(Filename)%(Extension)')" SkipUnchangedFiles="true" />
  </Target>
</Project>
```

<a id="file-33"></a>
## tools/Update-StandardModesGraph.py

변경 이유: 기존 입력 그래프를 보존하도록 출력 경로를 선택한다.

### before

```python
"""Generate a code-only structure graph. Does not build, test, or run diagnostics."""
from pathlib import Path
import datetime
import hashlib
import json
import re
import subprocess
import xml.etree.ElementTree as ET

from graphify.extract import extract
from graphify.build import build_from_json
from graphify.cluster import cluster
from graphify.export import to_json, to_html

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / 'graphify-out/standard-input'


def write(name, value):
    (OUT / name).write_text(value.rstrip() + '\n', encoding='utf-8-sig')


def dump(name, value):
    write(name, json.dumps(value, ensure_ascii=False, indent=2))


def inventory():
    projects = []
    ns = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}
    tracked = subprocess.check_output(['git', 'ls-files', '*.vcxproj'], cwd=ROOT, text=True, encoding='utf-8')
    historical = json.loads((ROOT / 'graphify-out/PROJECTS.json').read_text(encoding='utf-8-sig'))
    for relative in tracked.splitlines():
        path = ROOT / relative
        if not path.is_file():
            prior = next((p for p in historical if p.get('project') == relative), {})
            projects.append({**prior, 'project': relative, 'available': False,
                             'note': 'Absent before this task. Entry information is historical; no file was restored or deleted.'})
            continue
        sources = set()
        for element in ET.parse(path).findall('.//m:ClCompile', ns):
            include = element.get('Include')
            if include and '$(' not in include:
                sources.update(p.resolve() for p in path.parent.glob(include.replace('\\', '/')) if p.is_file())
        bundled = path.parent / 'framework/detail'
        if bundled.is_dir():
            sources.update(p.resolve() for p in bundled.glob('*.cpp'))
        entries = []
        for source in sorted(sources):
            text = source.read_text(encoding='utf-8-sig')
            for match in re.finditer(r'\b(?:int|void)\s+(main|wmain)\s*\(', text):
                entries.append({'file': source.relative_to(ROOT).as_posix(), 'name': match[1],
                                'line': text[:match.start()].count('\n') + 1,
                                'conditional_compilation': 'Review preprocessor branches in this source.' if '#if' in text else None})
        projects.append({'project': relative, 'available': True,
                         'sources': [p.relative_to(ROOT).as_posix() for p in sorted(sources)], 'entrypoints': entries})
    for name in ('basic', 'assets', 'motion'):
        projects.append({'standalone_example': True, 'file': f'framework/examples/{name}.cpp', 'entrypoint': 'main'})
    return projects


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    excluded = {'.git', '.vs', '.agents', '.codex', 'dependencies', 'packages', 'archive',
                'x64', 'Debug', 'Release', '.obj-work', 'dist', 'tests'}
    files = []
    for path in ROOT.rglob('*'):
        rel = path.relative_to(ROOT)
        if path.suffix.lower() not in {'.cpp', '.h', '.hpp'} or any(p in excluded for p in rel.parts):
            continue
        if 'framework' in rel.parts and rel.parts[0] != 'framework':
            continue
        files.append(path)
    # Include the two owning standard copies changed in this task, rather than every SDK distribution.
    files += [ROOT / 'standard/framework/include/Picking.h', ROOT / 'standard/framework/detail/Application.cpp']
    files = sorted(set(files))
    extraction = extract(files, cache_root=OUT, root=ROOT)
    node_sources = {n['id']: n.get('source_file', '').replace('\\', '/') for n in extraction['nodes']}
    for edge in extraction['edges']:
        a = node_sources.get(edge.get('source'), '').split('/')[0]
        b = node_sources.get(edge.get('target'), '').split('/')[0]
        if a and b and a != b and b != 'framework' and edge.get('relation') == 'calls':
            edge['confidence'] = 'AMBIGUOUS'
            edge['audit_note'] = 'Independent executables can have identically named classes. Confirm against owning sources.'
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    communities = cluster(graph)
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    to_json(graph, communities, str(OUT / 'graph.json'), force=True, built_at_commit=commit)
    to_html(graph, communities, str(OUT / 'graph.html'))
    for name in ('graph.json', 'graph.html'):
        write(name, (OUT / name).read_text(encoding='utf-8-sig'))
    dump('EXTRACTION.json', extraction)
    dump('PROJECTS.json', inventory())
    dump('SOURCE_MANIFEST.json', {
        'date': datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).date().isoformat(), 'source_commit': commit,
        'method': 'graphify AST; no semantic API, build, tests, app run, or extraction diagnostics',
        'nodes': graph.number_of_nodes(), 'edges': graph.number_of_edges(),
        'files': {p.relative_to(ROOT).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest() for p in files},
        'limits': ['Dynamic callback and virtual-dispatch order requires source review.',
                   'SDKs, tests, archives and duplicate framework distributions are excluded.',
                   'Missing pre-existing projects remain historical entries.']})
    print(f'graphify AST: {len(files)} files, {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges. No tests or diagnostics run.')


if __name__ == '__main__':
    main()
```

### after

```python
"""Generate a code-only structure graph. Does not build, test, or run diagnostics."""
from pathlib import Path
import argparse
import datetime
import hashlib
import json
import re
import subprocess
import xml.etree.ElementTree as ET

from graphify.extract import extract
from graphify.build import build_from_json
from graphify.cluster import cluster
from graphify.export import to_json, to_html

ROOT = Path(__file__).resolve().parent.parent
_parser = argparse.ArgumentParser(description=__doc__)
_parser.add_argument('--output', default='graphify-out/standard-input')
_args = _parser.parse_args()
OUT = (ROOT / _args.output).resolve()
OUT.relative_to(ROOT)


def write(name, value):
    (OUT / name).write_text(value.rstrip() + '\n', encoding='utf-8-sig')


def dump(name, value):
    write(name, json.dumps(value, ensure_ascii=False, indent=2))


def inventory():
    projects = []
    ns = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}
    tracked = subprocess.check_output(['git', 'ls-files', '*.vcxproj'], cwd=ROOT, text=True, encoding='utf-8')
    historical = json.loads((ROOT / 'graphify-out/PROJECTS.json').read_text(encoding='utf-8-sig'))
    for relative in tracked.splitlines():
        path = ROOT / relative
        if not path.is_file():
            prior = next((p for p in historical if p.get('project') == relative), {})
            projects.append({**prior, 'project': relative, 'available': False,
                             'note': 'Absent before this task. Entry information is historical; no file was restored or deleted.'})
            continue
        sources = set()
        for element in ET.parse(path).findall('.//m:ClCompile', ns):
            include = element.get('Include')
            if include and '$(' not in include:
                sources.update(p.resolve() for p in path.parent.glob(include.replace('\\', '/')) if p.is_file())
        bundled = path.parent / 'framework/detail'
        if bundled.is_dir():
            sources.update(p.resolve() for p in bundled.glob('*.cpp'))
        entries = []
        for source in sorted(sources):
            text = source.read_text(encoding='utf-8-sig')
            for match in re.finditer(r'\b(?:int|void)\s+(main|wmain)\s*\(', text):
                entries.append({'file': source.relative_to(ROOT).as_posix(), 'name': match[1],
                                'line': text[:match.start()].count('\n') + 1,
                                'conditional_compilation': 'Review preprocessor branches in this source.' if '#if' in text else None})
        projects.append({'project': relative, 'available': True,
                         'sources': [p.relative_to(ROOT).as_posix() for p in sorted(sources)], 'entrypoints': entries})
    for name in ('basic', 'assets', 'motion'):
        projects.append({'standalone_example': True, 'file': f'framework/examples/{name}.cpp', 'entrypoint': 'main'})
    return projects


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    excluded = {'.git', '.vs', '.agents', '.codex', 'dependencies', 'packages', 'archive',
                'x64', 'Debug', 'Release', '.obj-work', 'dist', 'tests'}
    files = []
    for path in ROOT.rglob('*'):
        rel = path.relative_to(ROOT)
        if path.suffix.lower() not in {'.cpp', '.h', '.hpp'} or any(p in excluded for p in rel.parts):
            continue
        if 'framework' in rel.parts and rel.parts[0] != 'framework':
            continue
        files.append(path)
    # Include the two owning standard copies changed in this task, rather than every SDK distribution.
    files += [ROOT / 'standard/framework/include/Picking.h', ROOT / 'standard/framework/detail/Application.cpp']
    files = sorted(set(files))
    extraction = extract(files, cache_root=OUT, root=ROOT)
    node_sources = {n['id']: n.get('source_file', '').replace('\\', '/') for n in extraction['nodes']}
    for edge in extraction['edges']:
        a = node_sources.get(edge.get('source'), '').split('/')[0]
        b = node_sources.get(edge.get('target'), '').split('/')[0]
        if a and b and a != b and b != 'framework' and edge.get('relation') == 'calls':
            edge['confidence'] = 'AMBIGUOUS'
            edge['audit_note'] = 'Independent executables can have identically named classes. Confirm against owning sources.'
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    communities = cluster(graph)
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    to_json(graph, communities, str(OUT / 'graph.json'), force=True, built_at_commit=commit)
    to_html(graph, communities, str(OUT / 'graph.html'))
    for name in ('graph.json', 'graph.html'):
        write(name, (OUT / name).read_text(encoding='utf-8-sig'))
    dump('EXTRACTION.json', extraction)
    dump('PROJECTS.json', inventory())
    dump('SOURCE_MANIFEST.json', {
        'date': datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).date().isoformat(), 'source_commit': commit,
        'method': 'graphify AST; no semantic API, build, tests, app run, or extraction diagnostics',
        'nodes': graph.number_of_nodes(), 'edges': graph.number_of_edges(),
        'files': {p.relative_to(ROOT).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest() for p in files},
        'limits': ['Dynamic callback and virtual-dispatch order requires source review.',
                   'SDKs, tests, archives and duplicate framework distributions are excluded.',
                   'Missing pre-existing projects remain historical entries.']})
    print(f'graphify AST: {len(files)} files, {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges. No tests or diagnostics run.')


if __name__ == '__main__':
    main()
```
