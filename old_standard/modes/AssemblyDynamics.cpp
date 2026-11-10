#include "Assembly.h"
#include <algorithm>

namespace assembly {
bool Assembly::dynamicsCommand(const Tokens& a, std::ostream& out) {
    if (a[0]!="force") return false;
    require(a.size()>=3,"force add/read/gravity/velocity/position/spring ...");
    if (a[1]=="add") {
        require(a.size()==6,"force add <ID> <물체> <기준 물체/world> <질량 kg>");
        Behavior b; b.name=a[2]; b.target=a[3]; b.reference=a[4]; b.effect=Effect::Dynamics;
        b.particle.mass=float(number(a[5])); require(std::isfinite(b.particle.mass) && b.particle.mass>0,"질량은 양수입니다.");
        auto& scene=app_.getScene();
        const auto frame=b.reference=="world" ? std::optional<glm::mat4>{glm::mat4(1)} : scene.worldMatrix(node(b.reference).id);
        require(frame && rigid(*frame),"힘의 기준 객체는 배율/전단 없는 강체여야 합니다. 형상 배율은 메시 자식에 적용하세요.");
        if (b.reference!="world") {
            auto ancestor=std::optional<graphics::EntityId>{node(b.reference).id};
            while (ancestor) {
                require(*ancestor!=node(b.target).id,"물리 기준 객체가 물리 대상의 자식일 수 없습니다.");
                ancestor=scene.getComponent(*ancestor)->parent;
            }
        }
        require(node(b.target).layers.empty() && !node(b.target).mount,"물리 대상 루트에는 지역 레이어/면 부착 대신 메시 자식을 사용하세요.");
        const auto world=scene.worldMatrix(node(b.target).id); require(world.has_value(),"물리 대상의 월드 좌표가 없습니다.");
        b.particle.position=glm::vec3(glm::inverse(*frame)*(*world)[3]); b.previousRoom=*frame;
        b.particleShape=glm::inverse(*frame)* *world; b.particleShape[3]=glm::vec4(0,0,0,1);
        attach(std::move(b)); return true;
    }
    const auto it=std::find_if(state_.behaviors.begin(),state_.behaviors.end(),[&](const Behavior& b){return b.effect==Effect::Dynamics && b.name==a[2];});
    require(it!=state_.behaviors.end(),"등록된 force ID가 필요합니다.");
    auto& b=*it;
    if (a[1]=="read") {
        require(a.size()==3,"force read <ID>");
        const auto load=graphics::DynamicsSystem::normalLoad(b.frame,b.particle.position,b.particle.mass,{0,1,0},b.gravity);
        out << "force read " << b.name << " [하중 조회] - 스프링 힘=" << glm::length(b.lastForce)
            << " N, 현재 위치를 고정할 때 +Y 접촉 반력=" << (load ? *load : 0) << " N\n";
        return true;
    }
    if (a[1]=="spring") {
        require(a.size()==8,"force spring <ID> <강성 N/m> <감쇠 N*s/m> <기준 객체의 앵커 XYZ>");
        const float stiffness=float(number(a[3])), damping=float(number(a[4])); const auto anchor=vector(a,5);
        const auto force=graphics::DynamicsSystem::spring(anchor,stiffness,damping);
        require(bool(force),"스프링 강성은 양수, 감쇠는 0 이상입니다.");
        b.stiffness=stiffness; b.damping=damping; b.springAnchor=anchor; b.force=force; return true;
    }
    require(a.size()==6 && (a[1]=="gravity" || a[1]=="velocity" || a[1]=="position"),"force gravity/velocity/position <ID> X Y Z");
    const auto value=vector(a,3);
    if (a[1]=="gravity") b.gravity=value;
    else if (a[1]=="velocity") b.particle.velocity=value;
    else b.particle.position=value;
    return true;
}

void Assembly::advanceDynamics(Behavior& b, float dt) {
    auto& scene=app_.getScene(); auto& target=node(b.target);
    const auto frameMatrix=b.reference=="world" ? std::optional<glm::mat4>{glm::mat4(1)} : scene.worldMatrix(node(b.reference).id);
    require(frameMatrix && rigid(*frameMatrix),"힘의 기준 객체에 비균등 배율/전단을 적용할 수 없습니다.");
    auto frame=b.frame; frame.rotation=glm::mat3(*frameMatrix);
    if (dt>0) {
        const auto before=glm::mat3(b.previousRoom), now=frame.rotation;
        const auto velocity=(glm::vec3((*frameMatrix)[3])-glm::vec3(b.previousRoom[3]))/dt;
        const auto omega=(glm::cross(before[0],now[0])+glm::cross(before[1],now[1])+glm::cross(before[2],now[2]))/(2*dt);
        frame.acceleration=b.frameSamples ? (velocity-b.previousOriginVelocity)/dt : glm::vec3(0);
        frame.angularAcceleration=b.frameSamples ? (omega-b.previousOmega)/dt : glm::vec3(0);
        frame.angularVelocity=omega;
        require(graphics::DynamicsSystem::step(b.particle,frame,dt,b.gravity,b.force,std::max(0.0,b.seconds-dt)),"힘 적분 실패. dt/질량/힘/강체 기준을 확인하세요.");
        b.previousOriginVelocity=velocity; b.previousOmega=omega; ++b.frameSamples;
    } else {
        b.frameSamples=0; frame.acceleration={0,0,0}; frame.angularVelocity={0,0,0}; frame.angularAcceleration={0,0,0};
    }
    b.frame=frame; b.previousRoom=*frameMatrix;
    b.lastForce=b.force ? b.force(b.particle,b.seconds) : glm::vec3(0);
    require(graphics::detail::finiteVector(b.lastForce),"힘 콜백이 비유한 값을 반환했습니다.");
    const auto desired=*frameMatrix*glm::translate(glm::mat4(1),b.particle.position)*b.particleShape;
    require(scene.setTransform(target.id,target.rest) && scene.setWorldPlacement(target.id,desired),"물리 월드 배치 실패.");
    if (b.stiffness>0) {
        const auto start=glm::vec3(*frameMatrix*glm::vec4(b.springAnchor,1));
        const auto end=glm::vec3(*frameMatrix*glm::vec4(b.particle.position,1));
        if (!b.tether) b.tether=scene.addLine(start,end,{1,.8f,.2f});
        require(b.tether && scene.setVector(*b.tether,start,end-start),"스프링 선 표시 실패.");
    }
}
} // namespace assembly
