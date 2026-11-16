#include "../include/Dynamics.h"
#include <algorithm>

namespace graphics {
bool DynamicsSystem::validFrame(const ReferenceFrame& f) {
    const auto r=f.rotation;
    return detail::finiteVector(r[0]) && detail::finiteVector(r[1]) && detail::finiteVector(r[2]) &&
        detail::finiteVector(f.acceleration) && detail::finiteVector(f.angularVelocity) && detail::finiteVector(f.angularAcceleration) &&
        std::abs(glm::determinant(r)-1)<.001f && glm::length(glm::transpose(r)*r[0]-glm::vec3(1,0,0))<.001f &&
        glm::length(glm::transpose(r)*r[1]-glm::vec3(0,1,0))<.001f && glm::length(glm::transpose(r)*r[2]-glm::vec3(0,0,1))<.001f;
}
std::optional<glm::vec3> DynamicsSystem::effectiveAcceleration(const ReferenceFrame& f, glm::vec3 p, glm::vec3 v, glm::vec3 gravity) {
    if (!validFrame(f) || !detail::finiteVector(p) || !detail::finiteVector(v) || !detail::finiteVector(gravity)) return {};
    const auto inverse=glm::transpose(f.rotation);
    const auto omega=inverse*f.angularVelocity, alpha=inverse*f.angularAcceleration;
    const auto a=inverse*(gravity-f.acceleration)-glm::cross(omega,glm::cross(omega,p))
        -2.0f*glm::cross(omega,v)-glm::cross(alpha,p);
    return detail::finiteVector(a) ? std::optional<glm::vec3>{a} : std::nullopt;
}
bool DynamicsSystem::step(ParticleState& particle, const ReferenceFrame& f, float dt,
                         glm::vec3 gravity, const ForceFunction& force, double start) {
    if (!validFrame(f) || !detail::finiteVector(particle.position) || !detail::finiteVector(particle.velocity) ||
        !std::isfinite(particle.mass) || particle.mass<=0 || !std::isfinite(dt) || dt<0 || dt>.25f || !std::isfinite(start)) return false;
    auto next=particle;
    const int count=std::max(1,int(std::ceil(dt*240)));
    const float h=dt/count;
    for (int i=0;i<count;++i) {
        const auto acceleration=effectiveAcceleration(f,next.position,next.velocity,gravity);
        if (!acceleration) return false;
        const auto external=force ? force(next,start+i*h) : glm::vec3(0);
        if (!detail::finiteVector(external)) return false;
        next.velocity+=(*acceleration+external/next.mass)*h;
        next.position+=next.velocity*h;
        if (!detail::finiteVector(next.position) || !detail::finiteVector(next.velocity)) return false;
    }
    particle=next; return true;
}
std::optional<glm::vec3> DynamicsSystem::holdingForce(const ReferenceFrame& f, glm::vec3 p, float mass, glm::vec3 gravity) {
    if (!std::isfinite(mass) || mass<=0) return {};
    const auto a=effectiveAcceleration(f,p,glm::vec3(0),gravity);
    if (!a) return {};
    const auto force=-mass * *a;
    return detail::finiteVector(force) ? std::optional<glm::vec3>{force} : std::nullopt;
}
std::optional<float> DynamicsSystem::normalLoad(const ReferenceFrame& f, glm::vec3 p, float mass, glm::vec3 normal, glm::vec3 gravity) {
    if (!detail::finiteVector(normal) || glm::length(normal)<1e-8f) return {};
    const auto force=holdingForce(f,p,mass,gravity); if (!force) return {};
    return std::max(0.0f,glm::dot(*force,glm::normalize(normal)));
}
ForceFunction DynamicsSystem::spring(glm::vec3 anchor, float stiffness, float damping) {
    if (!detail::finiteVector(anchor) || !std::isfinite(stiffness) || stiffness<=0 || !std::isfinite(damping) || damping<0) return {};
    return [=](const ParticleState& p,double) {return stiffness*(anchor-p.position)-damping*p.velocity;};
}
} // namespace graphics
