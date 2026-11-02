#include "Console.h"
#include <iostream>

/// 선택한 경로로 이동 모션을 바꾼다.
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

/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    demo_=Demo::Motion;
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

/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }
    if(key==GLFW_KEY_7 || key==GLFW_KEY_8 || key==GLFW_KEY_9 || key==GLFW_KEY_0) {
        const bool ok=key==GLFW_KEY_7?initGrid():key==GLFW_KEY_8?initRoom():key==GLFW_KEY_9?initAnimation():initScene();
        if(!ok) app_.requestClose();return;
    }
    if(key==GLFW_KEY_V) {threeViews_=!threeViews_;return;}
    if(key==GLFW_KEY_D) {trace_=!trace_;graphics::Diagnostics::configure({true,trace_});return;}
    if(key==GLFW_KEY_F && demo_==Demo::Room) {floorOpen_=!floorOpen_;return;}
    if(demo_!=Demo::Motion) {
        if(key==GLFW_KEY_SPACE) clock_.playing=!clock_.playing;
        else if(key==GLFW_KEY_ESCAPE) app_.requestClose();
        else if(key==GLFW_KEY_H) printHelp();
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

/// 마우스 버튼 입력을 처리한다.
void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }
    std::optional<graphics::PickResult> hit;
    if(threeViews_) {
        const auto size=app_.getFramebufferSize();
        if(size[0]<3 || size[1]<2) return;
        const auto passes=graphics::PatternSystem::threeViews(app_.getCamera(),size[0],size[1],app_.getRenderOptions());
        for(const auto& pass:passes) {
            const auto& v=pass.viewport;const double top=size[1]-v.y-v.height;
            if(event.framebufferX>=v.x && event.framebufferX<v.x+v.width && event.framebufferY>=top && event.framebufferY<top+v.height)
                hit=graphics::pick(app_.getScene(),pass.camera,event.framebufferX,event.framebufferY,{double(v.x),top,double(v.width),double(v.height)});
        }
    } else hit=app_.pickAt(event.windowX,event.windowY);
    if (hit) {
        std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    }
}

/// 조작 방법을 출력한다.
void Console::printHelp() const {
    std::cout << "standard: 프레임워크 Motion·반사·레이어 샘플\n"
              << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
              << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
              << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
              << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
              << "왼쪽 클릭: 물체의 entity/face ID 출력\n"
              << "7 Grid/거리 기반 배치 / 8 실내 미끄럼·낙하 / 0 기존 Motion 예제\n"
              << "9 선형보간·진자·사람형·그네→사람 계층 애니메이션\n"
              << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

/// 거리 기반 격자 예제를 구성한다.
bool Console::initGrid() {
    auto& scene=app_.getScene();scene.clear();demo_=Demo::Grid;clock_.reset();gridBoxes_.clear();
    graphics::GridLayout grid;grid.origin={-3,-.1f,-2};grid.extent={6,.2f,4};grid.cells={6,1,4};
    for(int z=0;z<4;++z) for(int x=0;x<6;++x) {
        const auto center=grid.cellCenter({x,0,z}),size=grid.relativeSize({.15f,1,.22f});
        if(!center || !size || !graphics::PatternSystem::addBox(scene,*size,*center,(x+z)%2?graphics::GlColor{.25f,.3f,.4f}:graphics::GlColor{.4f,.45f,.55f})) return false;
    }
    if(!distancePath_.setPoints({{-2.5f,.35f,-1.5f},{2.5f,.35f,-1.5f},{2.5f,.35f,1.5f}})) return false;
    for(unsigned i=0;i<5;++i) {
        const auto box=graphics::PatternSystem::addBox(scene,{.4f,.5f,.4f},{0,0,0},{.25f+.14f*i,.7f-.1f*i,.3f});
        if(!box || !graphics::PatternSystem::addNumber(scene,i+1,{-.07f,-.1f,.205f},.2f,{1,1,1},*box)) return false;
        gridBoxes_.push_back(*box);
    }
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={7,6,8};app_.getCamera().target={0,0,0};
    app_.getCamera().orthographicHalfExtent=4;return true;
}
/// 회전하는 방 예제를 구성한다.
bool Console::initRoom() {
    auto& scene=app_.getScene();scene.clear();demo_=Demo::Room;clock_.reset();floorOpen_=false;
    const auto room=graphics::PatternSystem::addRoom(scene,{{-2,-1,-2},{2,2,2}});
    const auto cube=graphics::PatternSystem::addBox(scene,{.5f,.5f,.5f},{0,-.75f,0},{1,.7f,.2f});
    if(!room || !cube) return false;room_=*room;roomCube_=*cube;
    roomBody_=graphics::RoomBody{};roomBody_.localPosition={0,-.75f,0};roomBody_.halfExtent={.25f,.25f,.25f};
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={5,3,8};app_.getCamera().target={0,0,0};
    return true;
}
/// 선형보간과 그네·사람의 부모·자식 애니메이션을 구성한다.
bool Console::initAnimation() {
    auto& scene = app_.getScene();
    scene.clear(); demo_ = Demo::Animation; clock_.reset(); animation_.clear();
    if (!graphics::PatternSystem::addBox(scene, {8, .15f, 6}, {0, -.15f, 0}, {.25f, .3f, .35f})) return false;
    graphics::SwingConfig swingConfig;
    swingConfig.pivot = {1, 3, 0};
    swingConfig.pendulum.amplitudeDegrees = 25;
    swingConfig.pendulum.periodSeconds = 3;
    const auto swing = graphics::AnimationSystem::addSwingWithRider(scene, swingConfig);
    if (!swing) return false;
    animation_ = swing->animation;

    graphics::HumanoidConfig personConfig;
    personConfig.grid.frame = glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0));
    const auto person = graphics::AnimationSystem::addHumanoid(scene, personConfig);
    if (!person) return false;
    auto pose = graphics::HumanoidPose::seated();
    pose.hips = {glm::vec3(0), glm::vec3(0)};
    pose.knees = {glm::vec3(0), glm::vec3(0)};
    if (!graphics::AnimationSystem::poseHumanoid(scene, *person, pose)) return false;
    for (std::size_t i = 0; i < 2; ++i) {
        graphics::PendulumMotion sway;
        sway.amplitudeDegrees = 30; sway.periodSeconds = 2;
        sway.phaseDegrees = i == 0 ? 0 : 180;
        graphics::Motion motion;
        if (!motion.append(graphics::Motion::pendulum(sway))) return false;
        const auto track = graphics::AnimationSystem::createJointAnimation(scene, person->shoulders[i], motion);
        if (!track) return false;
        animation_.push_back(*track);
    }
    const auto marker = scene.addGroup();
    if (!marker || !graphics::PatternSystem::addBox(scene, {.4f, .4f, .4f}, {0, 0, 0}, {1, .85f, .15f}, *marker)) return false;
    graphics::LinearMotion move;
    move.from = {-2, .3f, 2}; move.to = {2, .3f, 2};
    move.durationSeconds = 4; move.playback = graphics::Playback::PingPong;
    graphics::Motion motion;
    if (!motion.append(graphics::Motion::linear(move))) return false;
    const auto track = graphics::AnimationSystem::createJointAnimation(scene, *marker, motion);
    if (!track) return false;
    animation_.push_back(*track);
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 10}; app_.getCamera().target = {0, 1.4f, 0};
    std::cout << "9: 노란 상자는 4초 편도 선형보간, 그네는 3초 주기 진자, 좌석의 사람은 계층 변환을 따릅니다.\n";
    return graphics::AnimationSystem::poseHierarchy(scene, animation_, 0);
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    if(demo_==Demo::Motion || !clock_.playing) return;
    if(!clock_.advance(dt)) {app_.requestClose();return;}
    auto& scene=app_.getScene();bool ok=true;
    if (demo_ == Demo::Animation) {
        ok = graphics::AnimationSystem::poseHierarchy(scene, animation_, clock_.seconds);
    } else if(demo_==Demo::Grid) {
        for(std::size_t i=0;i<gridBoxes_.size();++i)
            ok=graphics::PatternSystem::follow(scene,gridBoxes_[i],distancePath_,clock_.seconds+distancePath_.length()*i/5)&&ok;
    } else {
        const float angle=.4f*float(std::sin(clock_.seconds));
        const auto matrix=glm::rotate(glm::mat4(1),angle,glm::vec3(0,0,1));
        graphics::Transform3D roomTransform;roomTransform.rotation=glm::mat3(matrix);roomTransform.basis=glm::vec3(matrix*glm::vec4(0,.5f,0,1));
        ok=scene.setTransform(room_,roomTransform) && scene.setFaceVisible(room_,5,!floorOpen_)
            && graphics::CollisionSystem::stepRoomBody(roomBody_,{{-2,-1,-2},{2,2,2}},matrix,dt,floorOpen_,false,{0,-9.8f,0},{0,0,.4f*float(std::cos(clock_.seconds))});
        graphics::Transform3D cubeTransform;cubeTransform.basis=roomBody_.position(matrix);
        if(!roomBody_.released) cubeTransform.rotation=glm::mat3(matrix);
        ok=scene.setTransform(roomCube_,cubeTransform)&&ok;
    }
    if(!ok) app_.requestClose();
}
/// 현재 장면을 지정한 뷰로 렌더링한다.
bool Console::onRender() {
    if(!threeViews_) return app_.renderScene();
    const auto size=app_.getFramebufferSize();if(size[0]<3 || size[1]<2) return true;
    return app_.renderViews(graphics::PatternSystem::threeViews(app_.getCamera(),size[0],size[1],app_.getRenderOptions()));
}
