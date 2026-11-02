#include "Console.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>
namespace {
/// 실패한 작업을 예외로 전달한다.
void require(bool ok,const char* message) {if(!ok) throw std::runtime_error(message);}
}
/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    auto& scene=app_.getScene();scene.clear();obstacles_.clear();
    body_=graphics::CharacterBody{};body_.feet={-3,0,-3};body_.speed=1;
    body_.height=1.8f;body_.halfWidth={.5f,.5f};
    doors_.fill(0);open_.fill(false);activeWall_.reset();walking_=true;
    const auto stage=graphics::PatternSystem::addRoom(scene,bounds_,false);
    if(!stage) return false;stage_=*stage;
    for(std::size_t face=0;face<4;++face) {
        if(!graphics::PatternSystem::openRoomWall(scene,stage_,face,0)) return false;
    }
    graphics::GridLayout grid;grid.origin={-.5f,0,-.3f};grid.extent={1,1.8f,.6f};
    const auto rig=graphics::PatternSystem::addWalkingRig(scene,grid);if(!rig) return false;rig_=*rig;
    std::cout << "실습 23 난수 시드=" << randomSeed_ << "; 초기화 시 기존 난수 엔진의 다음 수열을 사용합니다.\n";
    std::vector<graphics::Bounds3> occupied{{{-3.8f,0,-3.8f},{-2.2f,2,-2.2f}}};
    for(unsigned i=0;i<3;++i) {
        const glm::vec3 size{.9f+.2f*i,.6f+.25f*i,.9f+.15f*i};
        const auto box=graphics::CollisionSystem::randomFreeBox({{-4,0,-4},{4,size.y,4}},size,occupied,random_);
        if(!box) return false;
        if(!graphics::PatternSystem::addBox(scene,size,(box->minimum+box->maximum)*.5f,{.3f+.2f*i,.5f,.8f-.2f*i})) return false;
        occupied.push_back(*box);obstacles_.push_back(*box);
    }
    app_.getCamera()=graphics::Camera{};app_.getCamera().position={8,5,11};app_.getCamera().target={0,1,0};
    app_.getRenderOptions()=graphics::RenderOptions{};app_.getRenderOptions().depthTest=true;
    app_.getRenderOptions().lighting=true;
    return graphics::PatternSystem::poseWalkingRig(scene,rig_,body_.feet,0,0,body_.speed);
}
/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key,int action,int mods) {
    if(action!=GLFW_PRESS && action!=GLFW_REPEAT) return;
    const bool upper=((mods&GLFW_MOD_SHIFT)!=0)!=((mods&GLFW_MOD_CAPS_LOCK)!=0);
    auto& camera=app_.getCamera();
    if(key==GLFW_KEY_X || key==GLFW_KEY_Z) {
        glm::vec3 delta(0);delta[key==GLFW_KEY_X?0:2]=upper?-.35f:.35f;
        require(graphics::PatternSystem::moveCamera(camera,delta),"실습 23 카메라 이동 실패.");return;
    }
    if(key==GLFW_KEY_Y) {require(graphics::PatternSystem::orbitCamera(camera,{camera.target,{0,1,0}},upper?4.0f:-4.0f),"실습 23 카메라 공전 실패.");return;}
    if(key==GLFW_KEY_EQUAL || key==GLFW_KEY_KP_ADD) body_.speed=std::min(3.0f,body_.speed+.1f);
    if(key==GLFW_KEY_MINUS || key==GLFW_KEY_KP_SUBTRACT) body_.speed=std::max(0.0f,body_.speed-.1f);
    if(key==GLFW_KEY_W) {body_.direction={0,0,1};walking_=true;}
    if(key==GLFW_KEY_S) {body_.direction={0,0,-1};walking_=true;}
    if(key==GLFW_KEY_A) {body_.direction={-1,0,0};walking_=true;}
    if(key==GLFW_KEY_D) {body_.direction={1,0,0};walking_=true;}
    if(action!=GLFW_PRESS) return;
    if(key==GLFW_KEY_O) {
        if(activeWall_) open_[*activeWall_]=!open_[*activeWall_];
        else {
            activeWall_=graphics::PatternSystem::cameraFacingWall(app_.getScene(),stage_,camera);
            if(activeWall_) open_[*activeWall_]=true;
        }
        if(activeWall_) std::cout<<"선택한 벽="<<*activeWall_<<(open_[*activeWall_]?" 열기\n":" 원상복귀\n");
    }
    if(key==GLFW_KEY_SPACE) walking_=!walking_;
    if(key==GLFW_KEY_J) graphics::CollisionSystem::jump(body_,5.5f);
    if(key==GLFW_KEY_I) require(initScene(),"실습 23 초기화 실패: 앞선 API 오류를 확인하세요.");
    if(key==GLFW_KEY_Q || key==GLFW_KEY_ESCAPE) app_.requestClose();
}
/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    const float requested=body_.speed;
    if(!walking_) body_.speed=0;
    const auto result=graphics::CollisionSystem::stepCharacter(body_,bounds_,obstacles_,dt);
    body_.speed=requested;
    require(result.has_value(),"실습 23 캐릭터 충돌 계산 실패.");
    const float yaw=glm::degrees(std::atan2(body_.direction.x,body_.direction.z));
    require(graphics::PatternSystem::poseWalkingRig(app_.getScene(),rig_,body_.feet,yaw,body_.walkDistance,body_.speed),"실습 23 로봇 관절 자세 적용 실패.");
    for(std::size_t face=0;face<doors_.size();++face) {
        doors_[face]=std::clamp(doors_[face]+(open_[face]?1.0f:-1.0f)*dt,0.0f,1.0f);
        require(graphics::PatternSystem::openRoomWall(app_.getScene(),stage_,face,doors_[face]),"실습 23 벽 개방·복원 실패.");
    }
    if(activeWall_ && !open_[*activeWall_] && doors_[*activeWall_]==0) activeWall_.reset();
}
