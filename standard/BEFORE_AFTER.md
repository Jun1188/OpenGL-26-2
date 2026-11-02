# 현재 standard 변경 전후

2026-11-02. 과거 최초 생성 기록 대신 현재 착석·관절 조작 변경을 보여준다. 빌드·테스트는 수행하지 않았다. 문서의 일반 구현·API 사례는 docs/API_REFERENCE.html, 응용 사례는 docs/IMPLEMENTATION_PATTERNS.html을 따른다.

## standard/Console.h

### before
```cpp
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
    enum class Demo { Motion, Grid, Room, Animation };
    Demo demo_=Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_=0, roomCube_=0;
    std::vector<graphics::JointAnimation> animation_;
    bool threeViews_=false, trace_=false, floorOpen_=false;
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
    explicit Console(graphics::Application& app) : app_(app) {}
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
```

### after
```cpp
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
    enum class Demo { Motion, Grid, Room, Animation };
    Demo demo_=Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_=0, roomCube_=0;
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
    bool threeViews_=false, trace_=false, floorOpen_=false;
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
    explicit Console(graphics::Application& app) : app_(app) {}
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
```

## standard/Console.cpp

### before
```cpp
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
```

### after
```cpp
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
    if(demo_==Demo::Animation && handleJointKey(key)) return;
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
              << "9번 관절: Tab/Backspace 선택, X/Y/Z 축, 좌우 회전, 상하 이동, PageUp/Down 피벗, +/- 균일 배율, R 복원\n"
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
    jointControls_.clear(); selectedJoint_=0; selectedAxis_=0;
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
    const auto control = [&](graphics::EntityId id, const std::string& name) {
        std::size_t index=0;
        while(index<animation_.size() && animation_[index].joint!=id) ++index;
        if(index==animation_.size()) {
            auto fixed=graphics::AnimationSystem::createJointAnimation(scene,id,graphics::Motion{});
            if(!fixed) return false;
            animation_.push_back(*fixed);
        }
        jointControls_.push_back({index,name,animation_[index].rest}); return true;
    };
    const auto personControls = [&](const graphics::HumanoidRig& rig,const std::string& prefix) {
        if(!control(rig.root,prefix+" root") || !control(rig.pelvis,prefix+" pelvis")
           || !control(rig.torso,prefix+" torso") || !control(rig.neck,prefix+" neck")) return false;
        for(std::size_t i=0;i<2;++i) {
            const auto side=prefix+(i==0?" left ":" right ");
            if(!control(rig.shoulders[i],side+"shoulder") || !control(rig.elbows[i],side+"elbow")
               || !control(rig.hands[i],side+"hand") || !control(rig.hips[i],side+"hip")
               || !control(rig.knees[i],side+"knee") || !control(rig.feet[i],side+"foot")) return false;
        }
        return true;
    };
    if(!control(swing->frame,"swing frame") || !control(swing->hinge,"swing hinge")
       || !control(swing->seat,"swing seat") || !personControls(swing->person,"rider")
       || !personControls(*person,"standing")) return false;
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 10}; app_.getCamera().target = {0, 1.4f, 0};
    std::cout << "9: 노란 상자는 4초 편도 선형보간, 그네는 3초 주기 진자, 좌석의 사람은 계층 변환을 따릅니다.\n";
    printSelectedJoint();
    return graphics::AnimationSystem::poseHierarchy(scene, animation_, 0);
}
/// 선택한 관절의 기준 자세를 편집하고 같은 시각의 애니메이션에 반영한다.
bool Console::handleJointKey(int key) {
    if(jointControls_.empty()) return false;
    if(key==GLFW_KEY_TAB) selectedJoint_=(selectedJoint_+1)%jointControls_.size();
    else if(key==GLFW_KEY_BACKSPACE) selectedJoint_=(selectedJoint_+jointControls_.size()-1)%jointControls_.size();
    else if(key==GLFW_KEY_X || key==GLFW_KEY_Y || key==GLFW_KEY_Z)
        selectedAxis_=key==GLFW_KEY_X?0:key==GLFW_KEY_Y?1:2;
    else {
        auto& track=animation_[jointControls_[selectedJoint_].track];
        const auto saved=track.rest;
        if(key==GLFW_KEY_LEFT || key==GLFW_KEY_RIGHT) {
            glm::vec3 axis(0);axis[selectedAxis_]=1;
            track.rest.rotation=track.rest.rotation*glm::mat3(glm::rotate(glm::mat4(1),
                glm::radians(key==GLFW_KEY_LEFT?-5.0f:5.0f),axis));
        } else if(key==GLFW_KEY_UP || key==GLFW_KEY_DOWN)
            track.rest.basis[selectedAxis_]+=key==GLFW_KEY_UP?.05f:-.05f;
        else if(key==GLFW_KEY_PAGE_UP || key==GLFW_KEY_PAGE_DOWN)
            track.rest.localBasis[selectedAxis_]+=key==GLFW_KEY_PAGE_UP?.05f:-.05f;
        else if(key==GLFW_KEY_EQUAL || key==GLFW_KEY_KP_ADD || key==GLFW_KEY_MINUS || key==GLFW_KEY_KP_SUBTRACT) {
            const bool grow=key==GLFW_KEY_EQUAL || key==GLFW_KEY_KP_ADD;
            track.rest.scale=glm::vec3(std::clamp(track.rest.scale.x*(grow?1.1f:1/1.1f),.1f,3.0f));
        } else if(key==GLFW_KEY_R) track.rest=jointControls_[selectedJoint_].initialRest;
        else return false;
        if(!graphics::AnimationSystem::poseHierarchy(app_.getScene(),animation_,clock_.seconds)) {
            track.rest=saved;
            std::cerr<<"관절 편집 거절: 기준 자세를 복원했습니다.\n";
        }
    }
    printSelectedJoint();return true;
}
/// 선택 관절·축·이동·피벗·균일 배율을 출력한다.
void Console::printSelectedJoint() const {
    if(jointControls_.empty()) return;
    const auto& control=jointControls_[selectedJoint_];
    const auto& rest=animation_[control.track].rest;
    std::cout<<"joint "<<selectedJoint_+1<<'/'<<jointControls_.size()<<": "<<control.name
        <<" axis="<<"XYZ"[selectedAxis_]<<" position="<<rest.basis.x<<','<<rest.basis.y<<','<<rest.basis.z
        <<" pivot="<<rest.localBasis.x<<','<<rest.localBasis.y<<','<<rest.localBasis.z
        <<" scale="<<rest.scale.x<<'\n';
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
```

## standard/framework/include/Animation.h

### before
```cpp
#pragma once
#include "Patterns.h"

namespace graphics {
struct JointAnimation {
    EntityId joint = 0;
    Transform3D rest;
    Motion motion;
};
struct HumanoidConfig {
    GridLayout grid{glm::vec3(-.5f, 0, -.3f), glm::vec3(1, 1.8f, .6f)};
    float upperArmFraction = .5f, upperLegFraction = .5f;
    GlColor torsoColor{.65f, .65f, .7f}, headColor{1, .8f, .5f};
    GlColor leftColor{.2f, .6f, 1}, rightColor{1, .4f, .2f};
};
struct HumanoidRig {
    EntityId root = 0, pelvis = 0, torso = 0, neck = 0;
    EntityId torsoMesh = 0, head = 0, nose = 0;
    std::array<EntityId, 2> shoulders{}, elbows{}, hands{}, hips{}, knees{}, feet{};
    std::array<EntityId, 2> upperArms{}, forearms{}, thighs{}, shins{};
};
struct HumanoidPose {
    glm::vec3 pelvisDegrees{0}, torsoDegrees{0}, neckDegrees{0};
    std::array<glm::vec3, 2> shoulders{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> elbows{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> hips{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> knees{glm::vec3(0), glm::vec3(0)};
    /// [API] 허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.
    static HumanoidPose seated();
};
struct SwingConfig {
    glm::vec3 pivot{0, 3, 0}, seatSize{1.4f, .12f, .7f};
    float length = 2, ropeThickness = .035f;
    PendulumMotion pendulum;
    HumanoidConfig person;
    double riderSwayDegrees = 8;
};
struct SwingRig {
    EntityId frame = 0, hinge = 0, seat = 0;
    std::array<EntityId, 2> ropes{};
    HumanoidRig person;
    std::vector<JointAnimation> animation;
};
class AnimationSystem {
public:
    /// [API] 관절의 현재 강체 자세와 모션을 보관한다. 자세를 편집한 뒤 다시 만들면 새 기준 자세를 사용한다.
    static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);
    /// [API] 초 단위 절대 시간으로 관절들을 샘플링한 뒤 모두 유효할 때 반영한다. Scene 부모 계층을 유지한다.
    static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);
    /// [API] 실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.
    static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{},
                                                 std::optional<EntityId> parent = std::nullopt);
    /// [API] 관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.
    static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);
    /// [API] 고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.
    static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{},
                                                   std::optional<EntityId> parent = std::nullopt);
};
} // namespace graphics
#include "../detail/Animation.cpp"
```

### after
```cpp
#pragma once
#include "Patterns.h"

namespace graphics {
struct JointAnimation {
    EntityId joint = 0;
    Transform3D rest;
    Motion motion;
};
struct HumanoidConfig {
    GridLayout grid{glm::vec3(-.5f, 0, -.3f), glm::vec3(1, 1.8f, .6f)};
    float upperArmFraction = .5f, upperLegFraction = .5f;
    GlColor torsoColor{.65f, .65f, .7f}, headColor{1, .8f, .5f};
    GlColor leftColor{.2f, .6f, 1}, rightColor{1, .4f, .2f};
};
struct HumanoidRig {
    EntityId root = 0, pelvis = 0, torso = 0, neck = 0;
    EntityId torsoMesh = 0, head = 0, nose = 0;
    std::array<EntityId, 2> shoulders{}, elbows{}, hands{}, hips{}, knees{}, feet{};
    std::array<EntityId, 2> upperArms{}, forearms{}, thighs{}, shins{};
};
struct HumanoidPose {
    glm::vec3 pelvisDegrees{0}, torsoDegrees{0}, neckDegrees{0};
    std::array<glm::vec3, 2> shoulders{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> elbows{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> hips{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> knees{glm::vec3(0), glm::vec3(0)};
    /// [API] 허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.
    static HumanoidPose seated();
};
struct SwingConfig {
    glm::vec3 pivot{0, 3, 0}, seatSize{1.4f, .12f, .7f};
    float length = 2, ropeThickness = .035f;
    PendulumMotion pendulum;
    HumanoidConfig person;
    double riderSwayDegrees = 8;
};
struct SwingRig {
    EntityId frame = 0, hinge = 0, seat = 0;
    std::array<EntityId, 2> ropes{};
    HumanoidRig person;
    std::vector<JointAnimation> animation;
};
class AnimationSystem {
public:
    /// [API] 관절의 현재 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율만 허용한다.
    static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);
    /// [API] 기준 SRT·피벗과 강체 모션을 합성해 모두 유효할 때 반영한다. 부모 계층과 균일 배율을 유지한다.
    static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);
    /// [API] 실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.
    static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{},
                                                 std::optional<EntityId> parent = std::nullopt);
    /// [API] 관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.
    static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);
    /// [API] 고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.
    static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{},
                                                   std::optional<EntityId> parent = std::nullopt);
};
} // namespace graphics
#include "../detail/Animation.cpp"
```

## standard/framework/detail/Animation.cpp

### before
```cpp
#pragma once
#include <set>

namespace graphics {
inline HumanoidPose HumanoidPose::seated() {
    HumanoidPose pose;
    for (std::size_t i = 0; i < 2; ++i) {
        pose.hips[i].x = -90;
        pose.knees[i].x = 90;
        pose.shoulders[i].x = -20;
        pose.elbows[i].x = -40;
    }
    return pose;
}
inline std::optional<JointAnimation> AnimationSystem::createJointAnimation(
        const Scene& scene, EntityId joint, const Motion& motion) {
    const auto* component = scene.getComponent(joint);
    const auto sample = motion.sample(0);
    if (!component || !sample || !detail::rigidRoom(DiagramSystem::modelMatrix(component->transform))
        || !detail::rigidRoom(sample->parent) || !detail::rigidRoom(sample->local)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::createJointAnimation",
            "Need an existing rigid joint and a rigid motion sample. Scale the child mesh instead of the joint.");
        return {};
    }
    return JointAnimation{joint, component->transform, motion};
}
inline bool AnimationSystem::poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds) {
    if (!std::isfinite(seconds) || seconds < 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy", "Need nonnegative finite seconds.");
    std::set<EntityId> seen;
    std::vector<std::pair<EntityId, Transform3D>> staged;
    staged.reserve(animation.size());
    for (const auto& track : animation) {
        const auto sample = track.motion.sample(seconds);
        const auto rest = DiagramSystem::modelMatrix(track.rest);
        if (!scene.getComponent(track.joint) || !seen.insert(track.joint).second || !sample
            || !detail::rigidRoom(rest) || !detail::rigidRoom(sample->parent) || !detail::rigidRoom(sample->local))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy",
                "Need unique existing joints, rigid rest poses and rigid motion samples.");
        // World contributions are in the joint parent's space, before its rest pose.
        const auto matrix = sample->parent * rest * sample->local;
        Transform3D transform;
        transform.basis = glm::vec3(matrix[3]);
        transform.rotation = glm::mat3(matrix);
        if (!DiagramSystem::validTransform(transform))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy", "Joint composition overflow.");
        staged.emplace_back(track.joint, transform);
    }
    for (const auto& item : staged) if (!scene.setTransform(item.first, item.second)) return false;
    return true;
}
inline std::optional<HumanoidRig> AnimationSystem::addHumanoid(Scene& scene, const HumanoidConfig& config,
                                                               std::optional<EntityId> parent) {
    if (!config.grid.valid() || !detail::rigidRoom(config.grid.frame)
        || !std::isfinite(config.upperArmFraction) || config.upperArmFraction <= 0 || config.upperArmFraction >= 1
        || !std::isfinite(config.upperLegFraction) || config.upperLegFraction <= 0 || config.upperLegFraction >= 1
        || !detail::validColor(config.torsoColor) || !detail::validColor(config.headColor)
        || !detail::validColor(config.leftColor) || !detail::validColor(config.rightColor)
        || (parent && !scene.getComponent(*parent))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addHumanoid", "Invalid grid, limb fractions, colors or parent.");
        return {};
    }
    Scene staged = scene;
    HumanoidRig rig;
    const auto extent = config.grid.extent;
    Transform3D rootTransform;
    rootTransform.basis = glm::vec3(config.grid.frame[3]);
    rootTransform.rotation = glm::mat3(config.grid.frame);
    const auto root = staged.addGroup(rootTransform);
    if (!root || !staged.setParent(*root, parent)) return {};
    rig.root = *root;
    const auto pelvis = PatternSystem::addJoint(staged, rig.root, config.grid.origin + extent * glm::vec3(.5f, .4f, .5f));
    if (!pelvis) return {};
    rig.pelvis = *pelvis;
    const auto torso = PatternSystem::addJoint(staged, rig.pelvis, {0, extent.y * .18f, 0});
    if (!torso) return {};
    rig.torso = *torso;
    const auto torsoMesh = PatternSystem::addBox(staged, extent * glm::vec3(.45f, .35f, .7f), {0, 0, 0}, config.torsoColor, rig.torso);
    const auto neck = PatternSystem::addJoint(staged, rig.torso, {0, extent.y * .22f, 0});
    if (!torsoMesh || !neck) return {};
    rig.torsoMesh = *torsoMesh; rig.neck = *neck;
    const auto head = PatternSystem::addBox(staged, extent * glm::vec3(.3f, .2f, .6f), {0, extent.y * .1f, 0}, config.headColor, rig.neck);
    const auto nose = PatternSystem::addBox(staged, extent * glm::vec3(.1f, .05f, .2f), {0, extent.y * .1f, extent.z * .4f}, {1, .2f, .2f}, rig.neck);
    if (!head || !nose) return {};
    rig.head = *head; rig.nose = *nose;
    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto color = i == 0 ? config.leftColor : config.rightColor;
        const float upperArm = extent.y * .32f * config.upperArmFraction;
        const float forearm = extent.y * .32f - upperArm;
        const float thigh = extent.y * .4f * config.upperLegFraction;
        const float shin = extent.y * .4f - thigh;
        const auto shoulder = PatternSystem::addJoint(staged, rig.torso, {side * extent.x * .3f, extent.y * .14f, 0});
        const auto hip = PatternSystem::addJoint(staged, rig.pelvis, {side * extent.x * .15f, 0, 0});
        if (!shoulder || !hip) return {};
        rig.shoulders[i] = *shoulder; rig.hips[i] = *hip;
        const auto elbow = PatternSystem::addJoint(staged, *shoulder, {0, -upperArm, 0});
        const auto knee = PatternSystem::addJoint(staged, *hip, {0, -thigh, 0});
        if (!elbow || !knee) return {};
        rig.elbows[i] = *elbow; rig.knees[i] = *knee;
        const auto hand = PatternSystem::addJoint(staged, *elbow, {0, -forearm, 0});
        const auto foot = PatternSystem::addJoint(staged, *knee, {0, -shin, 0});
        const auto upper = PatternSystem::addBox(staged, {extent.x * .15f, upperArm, extent.z * .3f}, {0, -upperArm / 2, 0}, color, *shoulder);
        const auto lower = PatternSystem::addBox(staged, {extent.x * .15f, forearm, extent.z * .3f}, {0, -forearm / 2, 0}, color, *elbow);
        const auto thighMesh = PatternSystem::addBox(staged, {extent.x * .18f, thigh, extent.z * .4f}, {0, -thigh / 2, 0}, color, *hip);
        const auto shinMesh = PatternSystem::addBox(staged, {extent.x * .18f, shin, extent.z * .4f}, {0, -shin / 2, 0}, color, *knee);
        if (!hand || !foot || !upper || !lower || !thighMesh || !shinMesh) return {};
        rig.hands[i] = *hand; rig.feet[i] = *foot;
        rig.upperArms[i] = *upper; rig.forearms[i] = *lower;
        rig.thighs[i] = *thighMesh; rig.shins[i] = *shinMesh;
    }
    scene = std::move(staged);
    return rig;
}
inline bool AnimationSystem::poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose) {
    const std::array<EntityId, 11> joints{rig.pelvis, rig.torso, rig.neck,
        rig.shoulders[0], rig.shoulders[1], rig.elbows[0], rig.elbows[1],
        rig.hips[0], rig.hips[1], rig.knees[0], rig.knees[1]};
    const std::array<glm::vec3, 11> angles{pose.pelvisDegrees, pose.torsoDegrees, pose.neckDegrees,
        pose.shoulders[0], pose.shoulders[1], pose.elbows[0], pose.elbows[1],
        pose.hips[0], pose.hips[1], pose.knees[0], pose.knees[1]};
    std::set<EntityId> seen;
    std::array<Transform3D, 11> transforms;
    for (std::size_t i = 0; i < joints.size(); ++i) {
        const auto* component = scene.getComponent(joints[i]);
        if (!component || !seen.insert(joints[i]).second || !detail::finiteVector(angles[i]))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHumanoid", "Invalid joint or Euler angles.");
        glm::mat4 rotation(1);
        for (int axis = 0; axis < 3; ++axis) {
            glm::vec3 direction(0); direction[axis] = 1;
            const auto turn = TransformSystem::rotation({{0, 0, 0}, direction}, angles[i][axis]);
            if (!turn) return false;
            rotation = *turn * rotation;
        }
        transforms[i] = component->transform;
        transforms[i].rotation = glm::mat3(rotation);
        if (!DiagramSystem::validTransform(transforms[i])) return false;
    }
    for (std::size_t i = 0; i < joints.size(); ++i) if (!scene.setTransform(joints[i], transforms[i])) return false;
    return true;
}
inline std::optional<SwingRig> AnimationSystem::addSwingWithRider(Scene& scene, const SwingConfig& config,
                                                                 std::optional<EntityId> parent) {
    Motion swingMotion;
    auto pendulum = config.pendulum;
    pendulum.axis.point = {0, 0, 0}; pendulum.space = TransformSpace::Local;
    if (!detail::finiteVector(config.pivot) || !detail::positiveLengths(config.seatSize)
        || !std::isfinite(config.length) || config.length <= 0
        || !std::isfinite(config.ropeThickness) || config.ropeThickness <= 0
        || !std::isfinite(config.riderSwayDegrees) || config.riderSwayDegrees < 0
        || (parent && !scene.getComponent(*parent)) || !swingMotion.append(Motion::pendulum(pendulum))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addSwingWithRider", "Invalid swing configuration or parent.");
        return {};
    }
    Scene staged = scene;
    SwingRig rig;
    Transform3D transform; transform.basis = config.pivot;
    const auto frame = staged.addGroup(transform);
    if (!frame || !staged.setParent(*frame, parent)) return {};
    rig.frame = *frame;
    const auto hinge = PatternSystem::addJoint(staged, rig.frame, {0, 0, 0});
    if (!hinge) return {};
    rig.hinge = *hinge;
    const auto seat = PatternSystem::addBox(staged, config.seatSize, {0, -config.length, 0}, {.55f, .3f, .1f}, rig.hinge);
    if (!seat) return {};
    rig.seat = *seat;
    if (!PatternSystem::addBox(staged, {config.seatSize.x + .6f, .12f, .12f}, {0, 0, 0}, {.3f, .35f, .4f}, rig.frame)) return {};
    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto rope = PatternSystem::addBox(staged, {config.ropeThickness, config.length, config.ropeThickness},
            {side * config.seatSize.x * .43f, -config.length / 2, 0}, {.75f, .75f, .75f}, rig.hinge);
        if (!rope || !PatternSystem::addBox(staged, {.12f, config.length + 1, .12f},
            {side * (config.seatSize.x / 2 + .2f), -(config.length + 1) / 2, 0}, {.3f, .35f, .4f}, rig.frame)) return {};
        rig.ropes[i] = *rope;
    }
    const auto person = addHumanoid(staged, config.person, rig.seat);
    if (!person || !poseHumanoid(staged, *person, HumanoidPose::seated())) return {};
    rig.person = *person;
    const auto* root = staged.getComponent(rig.person.root);
    auto personTransform = root->transform;
    const auto hipCenter = config.person.grid.origin + config.person.grid.extent * glm::vec3(.5f, .4f, .5f);
    personTransform.basis = glm::vec3(0, config.seatSize.y / 2, 0) - personTransform.rotation * hipCenter;
    if (!staged.setTransform(rig.person.root, personTransform)) return {};
    const auto swingTrack = createJointAnimation(staged, rig.hinge, swingMotion);
    PendulumMotion rider;
    rider.periodSeconds = pendulum.periodSeconds; rider.amplitudeDegrees = config.riderSwayDegrees;
    rider.phaseDegrees = std::remainder(pendulum.phaseDegrees, 360.0) + 180;
    Motion riderMotion;
    if (!riderMotion.append(Motion::pendulum(rider))) return {};
    const auto riderTrack = createJointAnimation(staged, rig.person.torso, riderMotion);
    if (!swingTrack || !riderTrack) return {};
    rig.animation = {*swingTrack, *riderTrack};
    if (!poseHierarchy(staged, rig.animation, 0)) return {};
    scene = std::move(staged);
    return rig;
}
} // namespace graphics
```

### after
```cpp
#pragma once
#include <set>

namespace graphics {
namespace detail {
inline bool validJointRest(const Transform3D& rest) {
    return DiagramSystem::validTransform(rest) && rest.scale.x > 1e-6f
        && std::abs(rest.scale.x - rest.scale.y) < 1e-6f
        && std::abs(rest.scale.x - rest.scale.z) < 1e-6f;
}
}
inline HumanoidPose HumanoidPose::seated() {
    HumanoidPose pose;
    for (std::size_t i = 0; i < 2; ++i) {
        pose.hips[i].x = -90;
        pose.knees[i].x = 90;
        pose.shoulders[i].x = -20;
        pose.elbows[i].x = -40;
    }
    return pose;
}
inline std::optional<JointAnimation> AnimationSystem::createJointAnimation(
        const Scene& scene, EntityId joint, const Motion& motion) {
    const auto* component = scene.getComponent(joint);
    const auto sample = motion.sample(0);
    if (!component || !sample || !detail::validJointRest(component->transform)
        || !detail::rigidRoom(sample->parent) || !detail::rigidRoom(sample->local)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::createJointAnimation",
            "Need a joint with positive uniform rest scale and a rigid motion sample.");
        return {};
    }
    return JointAnimation{joint, component->transform, motion};
}
inline bool AnimationSystem::poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds) {
    if (!std::isfinite(seconds) || seconds < 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy", "Need nonnegative finite seconds.");
    std::set<EntityId> seen;
    std::vector<std::pair<EntityId, Transform3D>> staged;
    staged.reserve(animation.size());
    for (const auto& track : animation) {
        const auto sample = track.motion.sample(seconds);
        const auto rest = DiagramSystem::modelMatrix(track.rest);
        if (!scene.getComponent(track.joint) || !seen.insert(track.joint).second || !sample
            || !detail::validJointRest(track.rest) || !detail::rigidRoom(sample->parent) || !detail::rigidRoom(sample->local))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy",
                "Need unique existing joints, positive uniform rest scales and rigid motion samples.");
        // World contributions are in the joint parent's space, before its rest pose.
        const auto matrix = sample->parent * rest * sample->local;
        Transform3D transform;
        transform.basis = glm::vec3(matrix[3]);
        transform.scale = track.rest.scale;
        transform.rotation = glm::mat3(matrix) / track.rest.scale.x;
        if (!DiagramSystem::validTransform(transform))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHierarchy", "Joint composition overflow.");
        staged.emplace_back(track.joint, transform);
    }
    for (const auto& item : staged) if (!scene.setTransform(item.first, item.second)) return false;
    return true;
}
inline std::optional<HumanoidRig> AnimationSystem::addHumanoid(Scene& scene, const HumanoidConfig& config,
                                                               std::optional<EntityId> parent) {
    if (!config.grid.valid() || !detail::rigidRoom(config.grid.frame)
        || !std::isfinite(config.upperArmFraction) || config.upperArmFraction <= 0 || config.upperArmFraction >= 1
        || !std::isfinite(config.upperLegFraction) || config.upperLegFraction <= 0 || config.upperLegFraction >= 1
        || !detail::validColor(config.torsoColor) || !detail::validColor(config.headColor)
        || !detail::validColor(config.leftColor) || !detail::validColor(config.rightColor)
        || (parent && !scene.getComponent(*parent))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addHumanoid", "Invalid grid, limb fractions, colors or parent.");
        return {};
    }
    Scene staged = scene;
    HumanoidRig rig;
    const auto extent = config.grid.extent;
    Transform3D rootTransform;
    rootTransform.basis = glm::vec3(config.grid.frame[3]);
    rootTransform.rotation = glm::mat3(config.grid.frame);
    const auto root = staged.addGroup(rootTransform);
    if (!root || !staged.setParent(*root, parent)) return {};
    rig.root = *root;
    const auto pelvis = PatternSystem::addJoint(staged, rig.root, config.grid.origin + extent * glm::vec3(.5f, .4f, .5f));
    if (!pelvis) return {};
    rig.pelvis = *pelvis;
    const auto torso = PatternSystem::addJoint(staged, rig.pelvis, {0, extent.y * .18f, 0});
    if (!torso) return {};
    rig.torso = *torso;
    const auto torsoMesh = PatternSystem::addBox(staged, extent * glm::vec3(.45f, .35f, .7f), {0, 0, 0}, config.torsoColor, rig.torso);
    const auto neck = PatternSystem::addJoint(staged, rig.torso, {0, extent.y * .22f, 0});
    if (!torsoMesh || !neck) return {};
    rig.torsoMesh = *torsoMesh; rig.neck = *neck;
    const auto head = PatternSystem::addBox(staged, extent * glm::vec3(.3f, .2f, .6f), {0, extent.y * .1f, 0}, config.headColor, rig.neck);
    const auto nose = PatternSystem::addBox(staged, extent * glm::vec3(.1f, .05f, .2f), {0, extent.y * .1f, extent.z * .4f}, {1, .2f, .2f}, rig.neck);
    if (!head || !nose) return {};
    rig.head = *head; rig.nose = *nose;
    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto color = i == 0 ? config.leftColor : config.rightColor;
        const float upperArm = extent.y * .32f * config.upperArmFraction;
        const float forearm = extent.y * .32f - upperArm;
        const float thigh = extent.y * .4f * config.upperLegFraction;
        const float shin = extent.y * .4f - thigh;
        const auto shoulder = PatternSystem::addJoint(staged, rig.torso, {side * extent.x * .3f, extent.y * .14f, 0});
        const auto hip = PatternSystem::addJoint(staged, rig.pelvis, {side * extent.x * .15f, 0, 0});
        if (!shoulder || !hip) return {};
        rig.shoulders[i] = *shoulder; rig.hips[i] = *hip;
        const auto elbow = PatternSystem::addJoint(staged, *shoulder, {0, -upperArm, 0});
        const auto knee = PatternSystem::addJoint(staged, *hip, {0, -thigh, 0});
        if (!elbow || !knee) return {};
        rig.elbows[i] = *elbow; rig.knees[i] = *knee;
        const auto hand = PatternSystem::addJoint(staged, *elbow, {0, -forearm, 0});
        const auto foot = PatternSystem::addJoint(staged, *knee, {0, -shin, 0});
        const auto upper = PatternSystem::addBox(staged, {extent.x * .15f, upperArm, extent.z * .3f}, {0, -upperArm / 2, 0}, color, *shoulder);
        const auto lower = PatternSystem::addBox(staged, {extent.x * .15f, forearm, extent.z * .3f}, {0, -forearm / 2, 0}, color, *elbow);
        const auto thighMesh = PatternSystem::addBox(staged, {extent.x * .18f, thigh, extent.z * .4f}, {0, -thigh / 2, 0}, color, *hip);
        const auto shinMesh = PatternSystem::addBox(staged, {extent.x * .18f, shin, extent.z * .4f}, {0, -shin / 2, 0}, color, *knee);
        if (!hand || !foot || !upper || !lower || !thighMesh || !shinMesh) return {};
        rig.hands[i] = *hand; rig.feet[i] = *foot;
        rig.upperArms[i] = *upper; rig.forearms[i] = *lower;
        rig.thighs[i] = *thighMesh; rig.shins[i] = *shinMesh;
    }
    scene = std::move(staged);
    return rig;
}
inline bool AnimationSystem::poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose) {
    const std::array<EntityId, 11> joints{rig.pelvis, rig.torso, rig.neck,
        rig.shoulders[0], rig.shoulders[1], rig.elbows[0], rig.elbows[1],
        rig.hips[0], rig.hips[1], rig.knees[0], rig.knees[1]};
    const std::array<glm::vec3, 11> angles{pose.pelvisDegrees, pose.torsoDegrees, pose.neckDegrees,
        pose.shoulders[0], pose.shoulders[1], pose.elbows[0], pose.elbows[1],
        pose.hips[0], pose.hips[1], pose.knees[0], pose.knees[1]};
    std::set<EntityId> seen;
    std::array<Transform3D, 11> transforms;
    for (std::size_t i = 0; i < joints.size(); ++i) {
        const auto* component = scene.getComponent(joints[i]);
        if (!component || !seen.insert(joints[i]).second || !detail::finiteVector(angles[i]))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "AnimationSystem::poseHumanoid", "Invalid joint or Euler angles.");
        glm::mat4 rotation(1);
        for (int axis = 0; axis < 3; ++axis) {
            glm::vec3 direction(0); direction[axis] = 1;
            const auto turn = TransformSystem::rotation({{0, 0, 0}, direction}, angles[i][axis]);
            if (!turn) return false;
            rotation = *turn * rotation;
        }
        transforms[i] = component->transform;
        transforms[i].rotation = glm::mat3(rotation);
        if (!DiagramSystem::validTransform(transforms[i])) return false;
    }
    for (std::size_t i = 0; i < joints.size(); ++i) if (!scene.setTransform(joints[i], transforms[i])) return false;
    return true;
}
inline std::optional<SwingRig> AnimationSystem::addSwingWithRider(Scene& scene, const SwingConfig& config,
                                                                 std::optional<EntityId> parent) {
    Motion swingMotion;
    auto pendulum = config.pendulum;
    pendulum.axis.point = {0, 0, 0}; pendulum.space = TransformSpace::Local;
    if (!detail::finiteVector(config.pivot) || !detail::positiveLengths(config.seatSize)
        || !std::isfinite(config.length) || config.length <= 0
        || !std::isfinite(config.ropeThickness) || config.ropeThickness <= 0
        || !std::isfinite(config.riderSwayDegrees) || config.riderSwayDegrees < 0
        || (parent && !scene.getComponent(*parent)) || !swingMotion.append(Motion::pendulum(pendulum))) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "AnimationSystem::addSwingWithRider", "Invalid swing configuration or parent.");
        return {};
    }
    Scene staged = scene;
    SwingRig rig;
    Transform3D transform; transform.basis = config.pivot;
    const auto frame = staged.addGroup(transform);
    if (!frame || !staged.setParent(*frame, parent)) return {};
    rig.frame = *frame;
    const auto hinge = PatternSystem::addJoint(staged, rig.frame, {0, 0, 0});
    if (!hinge) return {};
    rig.hinge = *hinge;
    const auto seat = PatternSystem::addBox(staged, config.seatSize, {0, -config.length, 0}, {.55f, .3f, .1f}, rig.hinge);
    if (!seat) return {};
    rig.seat = *seat;
    if (!PatternSystem::addBox(staged, {config.seatSize.x + .6f, .12f, .12f}, {0, 0, 0}, {.3f, .35f, .4f}, rig.frame)) return {};
    for (std::size_t i = 0; i < 2; ++i) {
        const float side = i == 0 ? -1.0f : 1.0f;
        const auto rope = PatternSystem::addBox(staged, {config.ropeThickness, config.length, config.ropeThickness},
            {side * config.seatSize.x * .43f, -config.length / 2, 0}, {.75f, .75f, .75f}, rig.hinge);
        if (!rope || !PatternSystem::addBox(staged, {.12f, config.length + 1, .12f},
            {side * (config.seatSize.x / 2 + .2f), -(config.length + 1) / 2, 0}, {.3f, .35f, .4f}, rig.frame)) return {};
        rig.ropes[i] = *rope;
    }
    const auto person = addHumanoid(staged, config.person, rig.seat);
    if (!person || !poseHumanoid(staged, *person, HumanoidPose::seated())) return {};
    rig.person = *person;
    const auto* root = staged.getComponent(rig.person.root);
    auto personTransform = root->transform;
    const auto hipCenter = config.person.grid.origin + config.person.grid.extent * glm::vec3(.5f, .4f, .5f);
    const float thighHalfThickness = config.person.grid.extent.z * .2f;
    personTransform.basis = glm::vec3(0, config.seatSize.y / 2 + thighHalfThickness, 0)
                          - personTransform.rotation * hipCenter;
    if (!staged.setTransform(rig.person.root, personTransform)) return {};
    const auto swingTrack = createJointAnimation(staged, rig.hinge, swingMotion);
    PendulumMotion rider;
    rider.periodSeconds = pendulum.periodSeconds; rider.amplitudeDegrees = config.riderSwayDegrees;
    rider.phaseDegrees = std::remainder(pendulum.phaseDegrees, 360.0) + 180;
    Motion riderMotion;
    if (!riderMotion.append(Motion::pendulum(rider))) return {};
    const auto riderTrack = createJointAnimation(staged, rig.person.torso, riderMotion);
    if (!swingTrack || !riderTrack) return {};
    rig.animation = {*swingTrack, *riderTrack};
    if (!poseHierarchy(staged, rig.animation, 0)) return {};
    scene = std::move(staged);
    return rig;
}
} // namespace graphics
```

