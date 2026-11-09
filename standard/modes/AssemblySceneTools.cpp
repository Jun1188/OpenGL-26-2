#include "Assembly.h"
#include <algorithm>

namespace assembly {
bool Assembly::sceneToolCommand(const Tokens& a, std::ostream&) {
    if (a[0] == "tool-add") {
        require(a.size()==6 && (a[1]=="light" || a[1]=="camera"),"tool-add light/camera <이름> X Y Z");
        if (a[1]=="light") {
            const auto count=std::count_if(state_.nodes.begin(),state_.nodes.end(),[](const auto& item){return item.second.light.has_value();});
            require(count<8,"기본 셰이더의 점광원은 8개까지입니다.");
        }
        add("box",a[2],vector(a,3),glm::vec3(.2f));
        auto& object=node(a[2]); object.kind=a[1];
        if (a[1]=="light") {
            object.light=graphics::PointLight{};
            require(app_.getScene().setTint(*object.mesh,{1,.85f,.2f}),"광원 표시 실패.");
            app_.getRenderOptions().lighting=true;
        } else {
            auto camera=app_.getCamera(); camera.position={0,0,0}; camera.target={0,0,-1}; camera.up={0,1,0};
            object.camera=camera;
            require(app_.getScene().setTint(*object.mesh,{.2f,.7f,1}),"카메라 표시 실패.");
        }
        state_.selected=a[2]; state_.selectedLayer.clear(); return true;
    }
    if (a[0]=="light") {
        require(a.size()>=2,"light point/color/intensity/attenuation/sun ...");
        if (a[1]=="sun") {
            require(a.size()==3,"light sun <0 이상 강도>");
            const float value=float(number(a[2])); require(value>=0,"방향광 강도는 0 이상입니다.");
            app_.getRenderOptions().directionalIntensity=value; return true;
        }
        auto& object=node(state_.selected); require(object.light.has_value(),"선택한 물체는 점광원이 아닙니다.");
        auto next=*object.light;
        if (a[1]=="color") {
            require(a.size()==5,"light color R G B"); const auto color=vector(a,2);
            require(glm::all(glm::greaterThanEqual(color,glm::vec3(0))) && glm::all(glm::lessThanEqual(color,glm::vec3(1))),"RGB는 0~1입니다.");
            next.color={color.x,color.y,color.z};
        } else {
            require(a.size()==3 && (a[1]=="intensity" || a[1]=="attenuation"),"light intensity/attenuation <0 이상 값>");
            const float value=float(number(a[2])); require(value>=0,"강도/감쇠는 0 이상입니다.");
            (a[1]=="intensity" ? next.intensity : next.attenuation)=value;
        }
        object.light=next; return true;
    }
    if (a[0]!="camera" || a.size()<2) return false;
    if (a[1]=="use") {
        require(a.size()==3 && node(a[2]).camera.has_value(),"camera use <카메라 이름>");
        state_.activeCamera=a[2]; return true;
    }
    if (a[1]=="free") { require(a.size()==2,"camera free"); state_.activeCamera.clear(); return true; }
    if (a[1]=="look") {
        require(a.size()==5,"camera look <월드 XYZ 또는 cursor>");
        auto& object=node(state_.selected); require(object.camera.has_value(),"카메라를 선택하세요.");
        const auto world=app_.getScene().worldMatrix(object.id);
        require(world.has_value(),"카메라 좌표가 없습니다.");
        auto camera=*object.camera;
        camera.target=glm::vec3(glm::inverse(*world)*glm::vec4(vector(a,2),1));
        require(graphics::CameraSystem::validCamera(camera),"카메라의 시점/시선/up이 유효하지 않습니다.");
        object.camera=camera; return true;
    }
    if (state_.activeCamera.empty()) return false;
    auto& object=node(state_.activeCamera);
    auto camera=*object.camera;
    if (a[1]=="fov" || a[1]=="extent") {
        require(a.size()==3,"camera fov/extent <값>");
        (a[1]=="fov" ? camera.fieldOfViewDegrees : camera.orthographicHalfExtent)=float(number(a[2]));
    } else if (a[1]=="clip") {
        require(a.size()==4,"camera clip <near> <far>"); camera.nearPlane=float(number(a[2]));camera.farPlane=float(number(a[3]));
    } else if (a[1]=="projection") {
        require(a.size()==3 && (a[2]=="ortho" || a[2]=="perspective"),"camera projection ortho/perspective");
        camera.orthographic=a[2]=="ortho";
    } else {
        require(false,"사용 중인 카메라는 클릭/select 후 t/r/s 또는 지역 레이어로 이동하세요. camera free로 일반 시점에 복귀합니다.");
    }
    require(graphics::CameraSystem::validCamera(camera),"카메라 투영 범위를 확인하세요.");
    object.camera=camera; return true;
}

void Assembly::syncSceneTools() {
    auto& scene=app_.getScene();
    auto& lights=app_.getRenderOptions().pointLights; lights.clear();
    for (const auto& item:state_.nodes) {
        const auto& object=item.second;
        if (!object.light && !object.camera) continue;
        const auto world=scene.worldMatrix(object.id); require(world.has_value(),"광원/카메라 월드 변환 실패.");
        if (object.light) { auto light=*object.light; light.position=glm::vec3((*world)[3]); lights.push_back(light); }
        if (object.camera) {
            const bool active=state_.activeCamera==item.first;
            if (object.mesh) require(scene.setVisible(*object.mesh,!active),"카메라 표시 실패.");
            if (!active) continue;
            auto camera=*object.camera;
            const auto position=glm::vec3(*world*glm::vec4(camera.position,1));
            const auto forward=glm::vec3(*world*glm::vec4(camera.target-camera.position,0));
            const auto up=glm::vec3(*world*glm::vec4(camera.up,0));
            require(glm::length(forward)>.000001f && glm::length(glm::cross(forward,up))>.000001f,"카메라 방향이 퇴화했습니다.");
            const auto direction=glm::normalize(forward), right=glm::normalize(glm::cross(direction,up));
            camera.position=position; camera.target=position+direction; camera.up=glm::normalize(glm::cross(right,direction));
            require(graphics::CameraSystem::validCamera(camera),"월드 카메라 자세가 유효하지 않습니다.");
            app_.getCamera()=camera;
        }
    }
}
} // namespace assembly
