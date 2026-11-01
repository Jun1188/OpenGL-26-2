#include "Console.h"
#include <algorithm>
#include <stdexcept>
namespace {
void require(bool ok,const char* message) {if(!ok) throw std::runtime_error(message);}
}
bool Console::initScene() {
    auto& scene=app_.getScene();scene.clear();obstacles_.clear();
    body_=graphics::CharacterBody{};body_.feet={-3,0,-3};body_.speed=1;
    body_.height=1.8f;body_.halfWidth={.5f,.5f};
    door_=0;open_=false;walking_=true;
    const auto stage=graphics::PatternSystem::addRoom(scene,bounds_,false);
    if(!stage) return false;stage_=*stage;
    graphics::GridLayout grid;grid.origin={-.5f,0,-.3f};grid.extent={1,1.8f,.6f};
    const auto rig=graphics::PatternSystem::addWalkingRig(scene,grid);if(!rig) return false;rig_=*rig;
    std::mt19937 random(2301);
    std::vector<graphics::Bounds3> occupied{{{-3.8f,0,-3.8f},{-2.2f,2,-2.2f}}};
    for(unsigned i=0;i<3;++i) {
        const glm::vec3 size{.9f+.2f*i,.6f+.25f*i,.9f+.15f*i};
        const auto box=graphics::CollisionSystem::randomFreeBox({{-4,0,-4},{4,size.y,4}},size,occupied,random);
        if(!box) return false;
        if(!graphics::PatternSystem::addBox(scene,size,(box->minimum+box->maximum)*.5f,{.3f+.2f*i,.5f,.8f-.2f*i})) return false;
        occupied.push_back(*box);obstacles_.push_back(*box);
    }
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={8,5,11};app_.getCamera().target={0,1,0};
    app_.getRenderOptions()=graphics::RenderOptions{};app_.getRenderOptions().depthTest=true;
    app_.getRenderOptions().lighting=true;
    return graphics::PatternSystem::poseWalkingRig(scene,rig_,body_.feet,0,0,body_.speed);
}
void Console::onKey(int key,int action,int mods) {
    if(action!=GLFW_PRESS && action!=GLFW_REPEAT) return;
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    auto& camera=app_.getCamera();
    if(key==GLFW_KEY_X || key==GLFW_KEY_Z) {
        glm::vec3 delta(0);delta[key==GLFW_KEY_X?0:2]=upper?-.35f:.35f;
        require(graphics::PatternSystem::moveCamera(camera,delta),"Practice23 camera move failed.");return;
    }
    if(key==GLFW_KEY_Y) {require(graphics::PatternSystem::orbitCamera(camera,{camera.target,{0,1,0}},upper?4.0f:-4.0f),"Practice23 camera orbit failed.");return;}
    if(key==GLFW_KEY_EQUAL || key==GLFW_KEY_KP_ADD) body_.speed=std::min(3.0f,body_.speed+.1f);
    if(key==GLFW_KEY_MINUS || key==GLFW_KEY_KP_SUBTRACT) body_.speed=std::max(0.0f,body_.speed-.1f);
    if(key==GLFW_KEY_W) {body_.direction={0,0,1};walking_=true;}
    if(key==GLFW_KEY_S) {body_.direction={0,0,-1};walking_=true;}
    if(key==GLFW_KEY_A) {body_.direction={-1,0,0};walking_=true;}
    if(key==GLFW_KEY_D) {body_.direction={1,0,0};walking_=true;}
    if(action!=GLFW_PRESS) return;
    if(key==GLFW_KEY_O) open_=!open_;
    if(key==GLFW_KEY_SPACE) walking_=!walking_;
    if(key==GLFW_KEY_J) graphics::CollisionSystem::jump(body_,5.5f);
    if(key==GLFW_KEY_I) require(initScene(),"Practice23 reset failed.");
    if(key==GLFW_KEY_Q || key==GLFW_KEY_ESCAPE) app_.requestClose();
}
void Console::onUpdate(float dt) {
    const float requested=body_.speed;
    if(!walking_) body_.speed=0;
    const auto result=graphics::CollisionSystem::stepCharacter(body_,bounds_,obstacles_,dt);
    body_.speed=requested;
    require(result.has_value(),"Practice23 character collision failed.");
    const float yaw=glm::degrees(std::atan2(body_.direction.x,body_.direction.z));
    require(graphics::PatternSystem::poseWalkingRig(app_.getScene(),rig_,body_.feet,yaw,body_.walkDistance,body_.speed),"Practice23 rig pose failed.");
    door_=std::clamp(door_+(open_?1.0f:-1.0f)*dt,0.0f,1.0f);
    require(graphics::PatternSystem::openFace(app_.getScene(),stage_,0,door_,{0,4,0},{0,2,5},true),"Practice23 door failed.");
}
