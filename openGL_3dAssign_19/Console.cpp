#include "Console.h"
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
glm::mat3 turn(float degrees,glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1),glm::radians(degrees),axis));
}
}
graphics::EntityId Console::box(glm::vec3 size,glm::vec3 position,graphics::EntityId parent) {
    auto model=graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube,size);
    for(auto& face:model.faces)face.color={1,1,1};
    graphics::Transform3D transform;transform.basis=position;
    auto& scene=app_.getScene();
    const auto id=scene.addModel(model,transform,graphics::PivotMode::Preserve).value();
    if(!scene.setParent(id,parent))throw std::runtime_error("Cannot connect tank part.");
    colored_.push_back(id);return id;
}
bool Console::initScene() {
    reset();
    std::cout << "Practice 19: arrows move tank; d lower height; t central spin; l upper swap\n"
              << "g guns opposite yaw; p poles opposite pitch; x/X,z/Z camera translation\n"
              << "y/Y camera yaw, r/R camera orbit (repeat pauses); o colors; c reset; q quit\n";
    return true;
}
void Console::reset() {
    auto& scene=app_.getScene();scene.clear();colored_.clear();pressed_.fill(false);
    position_=glm::vec3(0);lowerTime_=centralAngle_=gunAngle_=poleTime_=swapProgress_=0;
    swapDirection_=1;cameraX_=cameraZ_=cameraYaw_=cameraOrbit_=0;
    lowerRunning_=centralRunning_=gunRunning_=poleRunning_=swapRunning_=swapStarted_=false;colorVersion_=0;
    root_=scene.addGroup().value();central_=scene.addGroup().value();
    if(!scene.setParent(central_,root_))throw std::runtime_error("Cannot create tank root.");
    const auto ground=scene.addGroup().value();
    box({12,.1f,12},{0,-.05f,0},ground);
    for(std::size_t i=0;i<2;++i)lower_[i]=box({.65f,.6f,1.8f},{i==0?-.8f:.8f,.3f,0},root_);
    box({2.3f,.25f,1.4f},{0,.125f,0},central_);
    for(std::size_t i=0;i<2;++i) {
        upper_[i]=scene.addGroup().value();guns_[i]=scene.addGroup().value();poles_[i]=scene.addGroup().value();
        if(!scene.setParent(upper_[i],central_)||!scene.setParent(guns_[i],upper_[i])||!scene.setParent(poles_[i],upper_[i]))
            throw std::runtime_error("Cannot create upper tank hierarchy.");
        box({.65f,.6f,.8f},{0,.3f,0},upper_[i]);
        box({.14f,.14f,1.1f},{0,0,.55f},guns_[i]);
        box({.08f,.85f,.08f},{0,.425f,0},poles_[i]);
    }
    auto& camera=app_.getCamera();camera=graphics::Camera{};
    camera.position={-5,4.5f,7};camera.target={0,.7f,0};camera.orthographic=false;
    app_.getRenderOptions()=graphics::RenderOptions{};app_.getRenderOptions().lighting=true;
    recolor();apply();
}
void Console::recolor() {
    const std::array<graphics::GlColor,10> palette{
        graphics::GlColor{.3f,.35f,.4f},{.25f,.6f,.3f},{.55f,.35f,.2f},{.2f,.55f,.75f},
        {.7f,.75f,.2f},{.9f,.4f,.15f},{1,.8f,.2f},{.4f,.7f,.8f},{.7f,.25f,.6f},{.6f,.9f,.4f}};
    for(std::size_t i=0;i<colored_.size();++i)
        if(!app_.getScene().setTint(colored_[i],palette[(i+colorVersion_)%palette.size()]))
            throw std::runtime_error("Cannot color tank part.");
}
void Console::apply() {
    auto& scene=app_.getScene();
    if(!scene.setPosition(root_,position_))throw std::runtime_error("Invalid tank position.");
    const float factor=1+.25f*std::sin(lowerTime_),height=.6f*factor;
    graphics::Transform3D center;center.basis={0,height,0};center.rotation=turn(centralAngle_,{0,1,0});
    if(!scene.setTransform(central_,center))throw std::runtime_error("Invalid central transform.");
    for(std::size_t i=0;i<2;++i) {
        const float sign=i==0?-1.0f:1.0f;
        graphics::Transform3D low;low.basis={sign*.8f,height*.5f,0};low.scale={1,factor,1};
        graphics::Transform3D upper;const float a=3.14159265f*swapProgress_;
        upper.basis={sign*.65f*std::cos(a),.25f,sign*.65f*std::sin(a)};
        graphics::Transform3D gun;gun.basis={0,.25f,.4f};gun.rotation=turn(sign*gunAngle_,{0,1,0});
        graphics::Transform3D pole;pole.basis={0,.6f,0};pole.rotation=turn(sign*60*std::sin(poleTime_),{1,0,0});
        if(!scene.setTransform(lower_[i],low)||!scene.setTransform(upper_[i],upper)
            ||!scene.setTransform(guns_[i],gun)||!scene.setTransform(poles_[i],pole))
            throw std::runtime_error("Invalid tank part transform.");
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
    case GLFW_KEY_D:lowerRunning_=!lowerRunning_;break;
    case GLFW_KEY_T:centralRunning_=!centralRunning_;break;
    case GLFW_KEY_G:gunRunning_=!gunRunning_;break;
    case GLFW_KEY_P:poleRunning_=!poleRunning_;break;
    case GLFW_KEY_L:
        if(!swapStarted_){swapStarted_=true;swapRunning_=true;}
        else if(!swapRunning_&&(swapProgress_==0||swapProgress_==1)){swapDirection_=-swapDirection_;swapRunning_=true;}
        else swapRunning_=!swapRunning_;
        break;
    case GLFW_KEY_Y:cameraYaw_=cameraYaw_==direction?0:direction;break;
    case GLFW_KEY_R:cameraOrbit_=cameraOrbit_==direction?0:direction;break;
    case GLFW_KEY_O:colorVersion_=(colorVersion_+1)%10;recolor();break;
    case GLFW_KEY_C:reset();break;
    case GLFW_KEY_Q:case GLFW_KEY_ESCAPE:app_.requestClose();break;
    default:break;
    }
}
void Console::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT])-float(pressed_[GLFW_KEY_LEFT]),0,
        float(pressed_[GLFW_KEY_DOWN])-float(pressed_[GLFW_KEY_UP])};
    if(glm::length(movement)>0)position_+=glm::normalize(movement)*dt*1.3f;
    // 12x12 floor minus a 2.5-unit radius covers every animated part's sweep.
    position_.x=std::clamp(position_.x,-3.5f,3.5f);position_.z=std::clamp(position_.z,-3.5f,3.5f);
    if(lowerRunning_)lowerTime_=std::fmod(lowerTime_+dt,6.283185307f);
    if(centralRunning_)centralAngle_=std::fmod(centralAngle_+45*dt,360.0f);
    if(gunRunning_)gunAngle_=std::fmod(gunAngle_+60*dt,360.0f);
    if(poleRunning_)poleTime_=std::fmod(poleTime_+dt,6.283185307f);
    if(swapRunning_) {
        swapProgress_=std::clamp(swapProgress_+swapDirection_*dt*.5f,0.0f,1.0f);
        if(swapProgress_==0||swapProgress_==1)swapRunning_=false;
    }
    auto camera=app_.getCamera();const glm::vec3 delta{cameraX_*2*dt,0,cameraZ_*2*dt};
    camera.position+=delta;camera.target+=delta;
    if(cameraYaw_!=0) {
        const auto rotation=turn(cameraYaw_*35*dt,glm::normalize(camera.up));
        camera.target=camera.position+rotation*(camera.target-camera.position);camera.up=rotation*camera.up;
    }
    if(cameraOrbit_!=0) {
        const auto rotation=turn(cameraOrbit_*25*dt,{0,1,0});
        camera.position=rotation*camera.position;camera.target=rotation*camera.target;camera.up=rotation*camera.up;
    }
    if(graphics::CameraSystem::validCamera(camera))app_.getCamera()=camera;
    apply();
}
void Console::onFocus(bool focused) {
    if(!focused){pressed_.fill(false);cameraX_=cameraZ_=0;}
}
