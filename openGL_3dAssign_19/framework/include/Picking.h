#pragma once
#include "Graphics.h"

namespace graphics {
struct Ray {
    glm::vec3 origin{0}, direction{0,0,-1};
    float maxDistance = std::numeric_limits<float>::infinity();
};
struct PickResult {
    EntityId entityId = 0;
    std::size_t faceIndex = 0, triangleIndex = 0;
    glm::vec3 worldPoint{0}, localPoint{0}, barycentric{0};
    glm::vec2 uv{0};
    bool hasUV = false;
    float distance = 0; // Distance from ray origin (near plane for a screen ray).
};
struct PickViewport { double x = 0, y = 0, width = 0, height = 0; }; // top-left framebuffer pixels

/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
inline std::optional<Ray> screenRay(const Camera& camera,double x,double y,const PickViewport& viewport) {
    if(!CameraSystem::validCamera(camera)||!std::isfinite(x)||!std::isfinite(y)
        ||!std::isfinite(viewport.x)||!std::isfinite(viewport.y)
        ||!std::isfinite(viewport.width)||!std::isfinite(viewport.height)
        ||viewport.width<=0||viewport.height<=0||x<viewport.x||y<viewport.y
        ||x>=viewport.x+viewport.width||y>=viewport.y+viewport.height)return {};
    const auto pv=CameraSystem::projectionMatrix(camera,float(viewport.width/viewport.height))*CameraSystem::viewMatrix(camera);
    const auto determinant=glm::determinant(pv);
    if(!std::isfinite(determinant)||std::abs(determinant)<1e-12f)return {};
    const auto inverse=glm::inverse(pv);
    const float nx=float(2*(x-viewport.x)/viewport.width-1),ny=float(1-2*(y-viewport.y)/viewport.height);
    auto nearPoint=inverse*glm::vec4(nx,ny,-1,1),farPoint=inverse*glm::vec4(nx,ny,1,1);
    if(std::abs(nearPoint.w)<1e-12f||std::abs(farPoint.w)<1e-12f)return {};
    nearPoint/=nearPoint.w;farPoint/=farPoint.w;
    const auto direction=glm::vec3(farPoint-nearPoint);const auto length=glm::length(direction);
    if(!detail::finiteVector(glm::vec3(nearPoint))||!detail::finiteVector(direction)||!std::isfinite(length)||length<=0)return {};
    return Ray{glm::vec3(nearPoint),direction/length,length};
}
/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene,const Ray& input) {
    if(!detail::finiteVector(input.origin)||!detail::finiteVector(input.direction)
        ||!std::isfinite(glm::length(input.direction))||glm::length(input.direction)<1e-12f||std::isnan(input.maxDistance)||input.maxDistance<0)return {};
    const auto direction=glm::dvec3(glm::normalize(input.direction));
    const auto origin=glm::dvec3(input.origin);
    double nearest=input.maxDistance;std::optional<PickResult> result;
    for(auto id:scene.getIds()) {
        const auto* component=scene.getComponent(id);if(!component||!component->visible)continue;
        for(std::size_t f=0;f<component->polyhedron.faces.size();++f) {
            const auto& face=component->polyhedron.faces[f];if(!face.visible)continue;
            const auto world=scene.faceWorldMatrix(id,f);if(!world)continue;
            const auto matrix=*world;
            const auto indices=detail::triangles(face);
            for(std::size_t t=0;t<indices.size();t+=3) {
                const auto ia=indices[t],ib=indices[t+1],ic=indices[t+2];
                const auto a4=matrix*glm::vec4(face.localVertices[ia],1),b4=matrix*glm::vec4(face.localVertices[ib],1),c4=matrix*glm::vec4(face.localVertices[ic],1);
                if(std::abs(a4.w)<1e-12f||std::abs(b4.w)<1e-12f||std::abs(c4.w)<1e-12f)continue;
                const auto a=glm::dvec3(a4/a4.w),b=glm::dvec3(b4/b4.w),c=glm::dvec3(c4/c4.w);
                const auto edge1=b-a,edge2=c-a,p=glm::cross(direction,edge2);
                const auto determinant=glm::dot(edge1,p);
                const auto tolerance=1e-12*glm::length(edge1)*glm::length(edge2);
                if(!std::isfinite(determinant)||std::abs(determinant)<=tolerance)continue;
                const auto relative=origin-a;const auto u=glm::dot(relative,p)/determinant;
                if(u<0||u>1)continue;
                const auto q=glm::cross(relative,edge1);const auto v=glm::dot(direction,q)/determinant;
                if(v<0||u+v>1)continue;
                const auto distance=glm::dot(edge2,q)/determinant;
                if(!std::isfinite(distance)||distance<0||distance>nearest)continue;
                const glm::dvec3 weights((1-u-v)/a4.w,u/b4.w,v/c4.w);
                const auto sum=weights.x+weights.y+weights.z;if(std::abs(sum)<1e-12)continue;
                PickResult hit;hit.entityId=id;hit.faceIndex=f;hit.triangleIndex=t/3;
                hit.distance=float(distance);hit.worldPoint=glm::vec3(origin+direction*distance);
                hit.barycentric=glm::vec3(weights/sum);
                hit.localPoint=face.localVertices[ia]*hit.barycentric.x+face.localVertices[ib]*hit.barycentric.y+face.localVertices[ic]*hit.barycentric.z;
                hit.hasUV=!face.texcoords.empty();
                if(hit.hasUV)hit.uv=face.texcoords[ia]*hit.barycentric.x+face.texcoords[ib]*hit.barycentric.y+face.texcoords[ic]*hit.barycentric.z;
                nearest=distance;result=hit;
            }
        }
    }
    return result;
}
/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene,const Camera& camera,double x,double y,
                                     const PickViewport& viewport) {
    const auto ray=screenRay(camera,x,y,viewport);return ray?pick(scene,*ray):std::nullopt;
}
} // namespace graphics
