#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
graphics::EntityId requireEntity(std::optional<graphics::EntityId> id,const char* reason) {
    if(!id)throw std::runtime_error(reason);
    return *id;
}
}
graphics::EntityId Console::box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent) {
    const auto id=requireEntity(graphics::PatternSystem::addBox(
        app_.getScene(),size,position,{1,1,1},parent),"탱크 상자 생성 실패: 크기·위치·부모 ID를 확인하세요.");
    colored_.push_back(id);
    return id;
}
bool Console::initScene() {
    reset();
    std::cout << "실습 22: 같은 장면을 원근 / XZ 평면도 / XY 정면도로 표시\n"
              << "방향키 이동; d 하부 높이; t 중앙 자전; l 상부 교환\n"
              << "g 포신 반대 회전; p 깃대 반대 기울기; x/X,z/Z 카메라 이동\n"
              << "y/Y 카메라 시선 회전; r/R 공전(재입력 정지); O/o 색 변경; c 초기화; q 종료\n";
    return true;
}
void Console::reset() {
    auto& scene=app_.getScene();
    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_=glm::vec3(0);
    lowerTime_=centralAngle_=gunAngle_=poleTime_=swapProgress_=0;
    swapDirection_=1;
    cameraX_=cameraZ_=cameraYaw_=cameraOrbit_=0;
    lowerRunning_=centralRunning_=gunRunning_=poleRunning_=swapRunning_=swapStarted_=false;
    colorVersion_=0;
    root_=requireEntity(scene.addGroup(),"탱크 루트 생성 실패.");
    central_=requireEntity(graphics::PatternSystem::addJoint(scene,root_,{0,.6f,0}),
        "중앙 관절 생성 실패.");
    const auto ground=requireEntity(scene.addGroup(),"바닥 그룹 생성 실패.");
    box({12,.1f,12},{0,-.05f,0},ground);
    for(std::size_t i=0;i<2;++i)lower_[i]=box({.65f,.6f,1.8f},{i==0?-.8f:.8f,.3f,0},root_);
    box({2.3f,.25f,1.4f},{0,.125f,0},central_);
    for(std::size_t i=0;i<2;++i) {
        const float sign=i==0?-1.0f:1.0f;
        upper_[i]=requireEntity(graphics::PatternSystem::addJoint(scene,central_,{sign*.65f,.25f,0}),
            "상부 관절 생성 실패.");
        guns_[i]=requireEntity(graphics::PatternSystem::addJoint(scene,upper_[i],{0,.3f,0}),
            "포신 중심 피벗 생성 실패.");
        poles_[i]=requireEntity(graphics::PatternSystem::addJoint(scene,upper_[i],{0,.6f,0}),
            "깃대 밑점 피벗 생성 실패.");
        box({.65f,.6f,.8f},{0,.3f,0},upper_[i]);
        // Gun offsets stay relative to the original body-center pivot (0,.3,0).
        box({.14f,.14f,1.1f},{0,-.05f,.95f},guns_[i]);
        box({.08f,.85f,.08f},{0,.425f,0},poles_[i]);
    }
    auto& camera=app_.getCamera();
    camera=graphics::Camera{};
    camera.position={-5,4.5f,7};
    camera.target={0,.7f,0};
    camera.orthographic=false;
    camera.orthographicHalfExtent=6.5f;
    app_.getRenderOptions()=graphics::RenderOptions{};
    app_.getRenderOptions().lighting=true;
    recolor();
    apply();
}
void Console::recolor() {
    const std::array<graphics::GlColor,10> palette{
        graphics::GlColor{.3f,.35f,.4f},{.25f,.6f,.3f},{.55f,.35f,.2f},{.2f,.55f,.75f},
        {.7f,.75f,.2f},{.9f,.4f,.15f},{1,.8f,.2f},{.4f,.7f,.8f},{.7f,.25f,.6f},{.6f,.9f,.4f}};
    for(std::size_t i=0;i<colored_.size();++i)
        if(!app_.getScene().setTint(colored_[i],palette[(i+colorVersion_)%palette.size()]))
            throw std::runtime_error("탱크 부품 색상 적용 실패.");
}
void Console::apply() {
    auto& scene=app_.getScene();
    if(!scene.setPosition(root_,position_))throw std::runtime_error("탱크 위치 적용 실패.");
    const float factor=1+.25f*std::sin(lowerTime_),height=.6f*factor;
    if(!scene.setPosition(central_,{0,height,0})
        ||!graphics::PatternSystem::rotateJoint(scene,central_,{0,1,0},centralAngle_))
        throw std::runtime_error("중앙 관절 변환 적용 실패.");
    for(std::size_t i=0;i<2;++i) {
        const float sign=i==0?-1.0f:1.0f;
        graphics::Transform3D low;
        low.basis={sign*.8f,height*.5f,0};
        low.scale={1,factor,1};
        const float a=3.14159265f*swapProgress_;
        const glm::vec3 upperPosition{sign*.65f*std::cos(a),.25f,sign*.65f*std::sin(a)};
        if(!scene.setTransform(lower_[i],low)||!scene.setPosition(upper_[i],upperPosition)
            ||!graphics::PatternSystem::rotateJoint(scene,guns_[i],{0,1,0},sign*gunAngle_)
            ||!graphics::PatternSystem::rotateJoint(scene,poles_[i],{1,0,0},sign*60*std::sin(poleTime_)))
            throw std::runtime_error("탱크 부품 변환 적용 실패.");
    }
}
void Console::onKey(int key,int action,int mods) {
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    if(key==GLFW_KEY_X||key==GLFW_KEY_Z) {
        if(action==GLFW_PRESS||action==GLFW_RELEASE) {
            float& direction=key==GLFW_KEY_X?cameraX_:cameraZ_;
            direction=action==GLFW_RELEASE?0:(upper?-1.0f:1.0f);
        }
        return;
    }
    if(key>=0&&key<=GLFW_KEY_LAST) {
        if(action==GLFW_PRESS)pressed_[key]=true;
        else if(action==GLFW_RELEASE)pressed_[key]=false;
    }
    if(action!=GLFW_PRESS)return;
    const float direction=upper?1.0f:-1.0f;
    switch(key) {
    case GLFW_KEY_D:
        lowerRunning_=!lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_=!centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_=!gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_=!poleRunning_;
        break;
    case GLFW_KEY_L:
        if(!swapStarted_){
            swapStarted_=true;
            swapRunning_=true;
        }
        else if(!swapRunning_&&(swapProgress_==0||swapProgress_==1)){
            swapDirection_=-swapDirection_;
            swapRunning_=true;
        }
        else swapRunning_=!swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_=cameraYaw_==direction?0:direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_=cameraOrbit_==direction?0:direction;
        break;
    case GLFW_KEY_O:
        colorVersion_=(colorVersion_+1)%10;
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
void Console::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT])-float(pressed_[GLFW_KEY_LEFT]),0,
        float(pressed_[GLFW_KEY_DOWN])-float(pressed_[GLFW_KEY_UP])};
    if(glm::length(movement)>0)position_+=glm::normalize(movement)*dt*1.3f;
    // 12x12 floor minus a 2.5-unit radius covers every animated part's sweep.
    position_.x=std::clamp(position_.x,-3.5f,3.5f);
    position_.z=std::clamp(position_.z,-3.5f,3.5f);
    if(lowerRunning_)lowerTime_=std::fmod(lowerTime_+dt,6.283185307f);
    if(centralRunning_)centralAngle_=std::fmod(centralAngle_+45*dt,360.0f);
    if(gunRunning_)gunAngle_= std::fmod(gunAngle_+60*dt,-360.0f);
    if(poleRunning_)poleTime_=std::fmod(poleTime_+dt,6.283185307f);
    if(swapRunning_) {
        swapProgress_=std::clamp(swapProgress_+swapDirection_*dt*.5f,0.0f,1.0f);
        if(swapProgress_==0||swapProgress_==1)swapRunning_=false;
    }
    auto camera=app_.getCamera();
    const glm::vec3 delta{cameraX_*2*dt,0,cameraZ_*2*dt};
    if(!graphics::PatternSystem::moveCamera(camera,delta)
        ||(cameraYaw_!=0&&!graphics::PatternSystem::yawCamera(camera,cameraYaw_*35*dt))
        ||(cameraOrbit_!=0&&!graphics::PatternSystem::orbitCamera(camera,{{0,0,0},{0,1,0}},cameraOrbit_*25*dt)))
        throw std::runtime_error("카메라 갱신 실패: 위치·시선·회전축을 확인하세요.");
    app_.getCamera()=camera;
    apply();
}
void Console::onFocus(bool focused) {
    if(!focused){
        pressed_.fill(false);
        cameraX_=cameraZ_=0;
    }
}

bool Console::onRender() {
    const auto size=app_.getFramebufferSize();
    if(size[0]<3||size[1]<2)return true;
    const auto passes=graphics::PatternSystem::threeViews(
        app_.getCamera(),size[0],size[1],app_.getRenderOptions());
    if(passes.size()!=3)throw std::runtime_error("세 카메라 뷰 생성 실패: 프레임버퍼 크기와 카메라를 확인하세요.");
    return app_.renderViews(passes);
}
