# 전체 코드 변경 전후

기준 커밋 `e6af6deb4b2c8acc5028307592b654bcaeebbf4d`부터 현재 저장된 코드까지의 모든 변경 블록이다. 문서·그래프 자동 산출물은 manifest에 기록하고 본문 코드 대조에서 제외한다. 빌드·테스트·검증기는 실행하지 않았다.

## framework/Framework.h

### 블록 1 · before L8 / after L8

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp

#include "include/Dynamics.h"
````

## framework/detail/Dynamics.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
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
````

## framework/detail/Renderer.cpp

### 블록 1 · before L501 / after L501

위치: `glm::mat4 matrix(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    gl.Uniform1f(gl.GetUniformLocation(shaderProgramID_, config_.uniforms.directionalIntensity.c_str()), options.directionalIntensity);
    gl.Uniform1i(gl.GetUniformLocation(shaderProgramID_, config_.uniforms.pointLightCount.c_str()), int(options.pointLights.size()));
    for (std::size_t i=0; i<options.pointLights.size(); ++i) {
        const auto& light=options.pointLights[i];
        const auto prefix=config_.uniforms.pointLights + "[" + std::to_string(i) + "].";
        gl.Uniform3f(gl.GetUniformLocation(shaderProgramID_, (prefix+"position").c_str()), light.position.x,light.position.y,light.position.z);
        gl.Uniform3f(gl.GetUniformLocation(shaderProgramID_, (prefix+"color").c_str()), light.color.red,light.color.green,light.color.blue);
        gl.Uniform1f(gl.GetUniformLocation(shaderProgramID_, (prefix+"intensity").c_str()),light.intensity);
        gl.Uniform1f(gl.GetUniformLocation(shaderProgramID_, (prefix+"attenuation").c_str()),light.attenuation);
    }
````

### 블록 2 · before L660 / after L670

위치: `bool Renderer::renderPass(const Scene& scene, const RenderPass& pass, int width, int height) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (!CameraSystem::validCamera(camera) || !detail::finiteVector(options.lightDirection) ||
````

**after**

````cpp
    bool validLights=options.pointLights.size()<=8 && std::isfinite(options.directionalIntensity) && options.directionalIntensity>=0;
    for (const auto& light:options.pointLights)
        validLights=validLights && detail::finiteVector(light.position) && detail::validColor(light.color) &&
            std::isfinite(light.intensity) && light.intensity>=0 && std::isfinite(light.attenuation) && light.attenuation>=0;
    if (!validLights || !CameraSystem::validCamera(camera) || !detail::finiteVector(options.lightDirection) ||
````

## framework/detail/Scene.cpp

### 블록 1 · before L339 / after L339

위치: `bool Scene::setParent(EntityId id, std::optional<EntityId> parent) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
namespace {
glm::mat4 inversePlacement(const glm::mat4& m) {
    const auto linear=glm::inverse(glm::mat3(m));
    glm::mat4 result(linear);
    result[3]=glm::vec4(-linear*glm::vec3(m[3]),1);
    return result;
}
bool invertiblePlacement(const glm::mat4& m) {
    return TransformSystem::validAffine(m) && std::abs(glm::determinant(glm::mat3(m))) > 1e-18f;
}
}
std::optional<glm::mat4> Scene::placementMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto& c = components_[id];
    const auto parent = c.parent ? worldMatrix(*c.parent) : std::optional<glm::mat4>{glm::mat4(1)};
    if (!parent) return {};
    const auto matrix = *parent * c.attachment * detail::transformMatrix(c.transform);
    return TransformSystem::validAffine(matrix) ? std::optional<glm::mat4>{matrix} : std::nullopt;
}
bool Scene::setAttachment(EntityId id, const glm::mat4& matrix) {
    if (id >= components_.size() || !invertiblePlacement(matrix)) return false;
    components_[id].attachment = matrix;
    return true;
}
bool Scene::setLocalLayers(EntityId id, const std::vector<glm::mat4>& layers) {
    if (id >= components_.size() || layers.size() > 128) return false;
    glm::mat4 composed(1);
    for (const auto& layer : layers) {
        composed *= layer;
        if (!invertiblePlacement(layer) || !invertiblePlacement(composed)) return false;
    }
    components_[id].localLayers = layers;
    return true;
}
bool Scene::setWorldPlacement(EntityId id, const glm::mat4& world) {
    if (id >= components_.size() || !invertiblePlacement(world)) return false;
    const auto& c = components_[id];
    const auto parent = c.parent ? worldMatrix(*c.parent) : std::optional<glm::mat4>{glm::mat4(1)};
    const auto base = detail::transformMatrix(c.transform);
    if (!parent || !invertiblePlacement(*parent) || !invertiblePlacement(base)) return false;
    return setAttachment(id, inversePlacement(*parent) * world * inversePlacement(base));
}
bool Scene::setParentPreserveWorld(EntityId id, std::optional<EntityId> parent) {
    const auto placement = placementMatrix(id);
    if (!placement) return false;
    const auto oldParent = components_[id].parent;
    const auto oldAttachment = components_[id].attachment;
    if (!setParent(id, parent)) return false;
    if (setWorldPlacement(id, *placement)) return true;
    components_[id].parent = oldParent;
    components_[id].attachment = oldAttachment;
    return false;
}
````

### 블록 2 · before L388 / after L442

위치: `glm::mat4 prefix(1), suffix(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
````

**after**

````cpp
        const auto& component = components_[chain[i]];
        matrix *= component.attachment * detail::transformMatrix(component.transform);
        for (const auto& local : component.localLayers) matrix *= local;
````

## framework/detail/Shaders.h

### 블록 1 · before L15 / after L15

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
out vec3 WorldPosition;
````

### 블록 2 · before L20 / after L21

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    WorldPosition = (uModel * vec4(vPos, 1.0)).xyz;
````

### 블록 3 · before L26 / after L28

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
in vec3 WorldPosition;
````

### 블록 4 · before L31 / after L34

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
uniform float uDirectionalIntensity;
struct PointLight { vec3 position; vec3 color; float intensity; float attenuation; };
uniform int uPointLightCount;
uniform PointLight uPointLights[8];
````

### 블록 5 · before L39 / after L46

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (uLighting && length(Normal) > 0.0)
        color.rgb *= (0.25 + 0.75 * max(dot(normalize(Normal), normalize(uLightDirection)), 0.0)) * uLightColor;
````

**after**

````cpp
    if (uLighting && length(Normal) > 0.0) {
        vec3 n = normalize(Normal);
        vec3 illumination = (0.25 + 0.75 * max(dot(n, normalize(uLightDirection)), 0.0)) * uLightColor * uDirectionalIntensity;
        for (int i = 0; i < uPointLightCount; ++i) {
            vec3 delta = uPointLights[i].position - WorldPosition;
            float distanceSquared = dot(delta, delta);
            vec3 direction = delta / sqrt(max(distanceSquared, 1e-12));
            illumination += uPointLights[i].color * uPointLights[i].intensity * max(dot(n, direction), 0.0)
                / (1.0 + uPointLights[i].attenuation * distanceSquared);
        }
        color.rgb *= illumination;
    }
````

## framework/include/Dynamics.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {
struct ReferenceFrame {
    glm::mat3 rotation{1}; // Local to world; orthonormal, determinant +1.
    glm::vec3 acceleration{0}; // World units/s^2 of the frame origin.
    glm::vec3 angularVelocity{0}, angularAcceleration{0}; // World radians/s and radians/s^2.
};
struct ParticleState {
    glm::vec3 position{0}, velocity{0}; // Frame-local coordinates.
    float mass = 1;
};
using ForceFunction = std::function<glm::vec3(const ParticleState&,double)>; // Local newtons, time in seconds.

class DynamicsSystem {
public:
    static bool validFrame(const ReferenceFrame& frame);
    /// Gravity, translation, centrifugal, Coriolis and Euler terms in frame-local coordinates.
    static std::optional<glm::vec3> effectiveAcceleration(const ReferenceFrame& frame,
        glm::vec3 localPosition, glm::vec3 localVelocity, glm::vec3 gravityWorld = {0,-9.8f,0});
    /// Semi-implicit Euler, <=1/240s substeps. Commits only a fully finite result.
    static bool step(ParticleState& particle, const ReferenceFrame& frame, float dt,
        glm::vec3 gravityWorld = {0,-9.8f,0}, const ForceFunction& force = {}, double startTime = 0);
    /// Force required to hold a point stationary in this frame; local newtons.
    static std::optional<glm::vec3> holdingForce(const ReferenceFrame& frame,
        glm::vec3 localPosition, float mass, glm::vec3 gravityWorld = {0,-9.8f,0});
    /// Compression-only reading along a scale's local surface normal.
    static std::optional<float> normalLoad(const ReferenceFrame& frame, glm::vec3 localPosition,
        float mass, glm::vec3 normal = {0,1,0}, glm::vec3 gravityWorld = {0,-9.8f,0});
    /// Linear, zero-rest-length spring and viscous damper; all values in frame-local units.
    static ForceFunction spring(glm::vec3 anchor, float stiffness, float damping);
};
} // namespace graphics
````

## framework/include/Graphics.h

### 블록 1 · before L108 / after L108

위치: `bool setParent(EntityId id, std::optional<EntityId> parent);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp

    /// [API] Full affine placement before this object's local layers; parent's final layers are inherited.
    std::optional<glm::mat4> placementMatrix(EntityId id) const;
    /// [API] Sets world-authored placement without decomposing a potentially sheared matrix.
    bool setWorldPlacement(EntityId id, const glm::mat4& world);
    /// [API] Changes parent while preserving placement and this object's local layer list.
    bool setParentPreserveWorld(EntityId id, std::optional<EntityId> parent);
    /// [API] Sets the parent-relative affine attachment, before base SRT.
    bool setAttachment(EntityId id, const glm::mat4& matrix);
    /// [API] Replaces ordered local layers atomically. Each layer inherits all preceding layers.
    bool setLocalLayers(EntityId id, const std::vector<glm::mat4>& layers);
````

### 블록 2 · before L358 / after L370

위치: `struct PointLight {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
struct PointLight {
    glm::vec3 position{0};
    GlColor color{1,1,1};
    float intensity = 1, attenuation = .02f;
};
````

### 블록 3 · before L368 / after L386

위치: `struct RenderOptions {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    float directionalIntensity = 1;
    std::vector<PointLight> pointLights; // At most eight world-space lights in the default shader.
````

## framework/include/Model.h

### 블록 1 · before L146 / after L146

위치: `struct Component_Poly {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    glm::mat4 attachment{1.0f}; // Object-owned affine placement, including inherited shear.
    std::vector<glm::mat4> localLayers; // Ordered, object-local transformations.
````

## framework/include/RenderConfig.h

### 블록 1 · before L37 / after L37

위치: `struct UniformBindings {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    std::string pointLightCount = "uPointLightCount", pointLights = "uPointLights";
    std::string directionalIntensity = "uDirectionalIntensity";
````

## framework/shaders/FragmentShader.glsl

### 블록 1 · before L5 / after L5

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````glsl
in vec3 WorldPosition;
````

### 블록 2 · before L10 / after L11

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````glsl
uniform float uDirectionalIntensity;
struct PointLight { vec3 position; vec3 color; float intensity; float attenuation; };
uniform int uPointLightCount;
uniform PointLight uPointLights[8];
````

### 블록 3 · before L14 / after L19

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl

````

**after** (없음)


### 블록 4 · before L16 / after L20

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl
    if (uOverrideColor) {
        FragColor = vec4(uOutlineColor, 1.0);

        return;
````

**after**

````glsl
    if (uOverrideColor) { FragColor = vec4(uOutlineColor, 1.0); return; }
    vec4 color = vec4(outColor * uTint, 1.0);
    if (uHasTexture) color *= texture(uTexture, TexCoord);
    if (uLighting && length(Normal) > 0.0) {
        vec3 n = normalize(Normal);
        vec3 illumination = (0.25 + 0.75 * max(dot(n, normalize(uLightDirection)), 0.0)) * uLightColor * uDirectionalIntensity;
        for (int i = 0; i < uPointLightCount; ++i) {
            vec3 delta = uPointLights[i].position - WorldPosition;
            float distanceSquared = dot(delta, delta);
            vec3 direction = delta / sqrt(max(distanceSquared, 1e-12));
            illumination += uPointLights[i].color * uPointLights[i].intensity * max(dot(n, direction), 0.0)
                / (1.0 + uPointLights[i].attenuation * distanceSquared);
        }
        color.rgb *= illumination;
````

### 블록 5 · before L21 / after L35

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl
    vec4 color = vec4(outColor * uTint, 1.0);

    if (uHasTexture)
        color *= texture(uTexture, TexCoord);

    if (uLighting && length(Normal) > 0.0)
        color.rgb *= (0.25 + 0.75 * max(dot(normalize(Normal), normalize(uLightDirection)), 0.0)) * uLightColor;
````

**after** (없음)


## framework/shaders/VertexShader.glsl

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec3 vNormal;
````

**after**

````glsl
layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
layout(location=2) in vec2 vTexCoord;
layout(location=3) in vec3 vNormal;
````

### 블록 2 · before L12 / after L12

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl

````

**after**

````glsl
out vec3 WorldPosition;
````

### 블록 3 · before L18 / after L18

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````glsl
    WorldPosition = (uModel * vec4(vPos, 1.0)).xyz;
````

## standard/Console.cpp

### 블록 1 · before L5 / after L5

위치: `bool Console::initScene() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    printHelp();
    return activateMode(initialMode_.value_or(1));
}

bool Console::activateMode(int mode) {
    if (mode < 0 || mode > lastStandardMode)
        return false;
    if (mode >= 1 && mode <= 6 && demo_ == Demo::Motion && pathId_) {
        activeMode_ = mode;
        return choosePath(mode);
    }
    if (practiceMode_)
        practiceMode_->onFocus(false);
    practiceMode_.reset();
    activeMode_ = mode;
    animation_.clear();
    jointControls_.clear();
    selectedJoint_ = 0;
    selectedAxis_ = 0;
    roomControl_.reset();
    bodyControl_.reset();
    pathId_.reset();
    spinId_.reset();
    globalId_.reset();
    mirrorLayer_.reset();
    normalSpinLayer_.reset();
    insertedLayer_.reset();
    clock_.reset();
    app_.getCamera() = graphics::Camera{};
    app_.getRenderOptions() = graphics::RenderOptions{};
    if (mode == 0 || mode >= 10) {
        practiceMode_ = makePracticeMode(mode, app_);
        return practiceMode_ && practiceMode_->initScene();
    }
    const bool ok = mode <= 6 ? initMotion(mode) : mode == 7 ? initGrid() : mode == 8 ? initRoom() : initAnimation();
    if (!ok || !applyControls())
        return false;
    printSelectedJoint();
````

**after**

````cpp
    if (!assembly::defaultRecipe(workspace_, std::cout)) return false;
    workspace_.help(std::cout);
    input_.prompt();
````

### 블록 2 · before L45 / after L10

위치: `void Console::onKey(int key, int action, int mods) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp

/// 키 입력에 따라 동작을 변경한다.
````

**after** (없음)


### 블록 3 · before L48 / after L11

위치: `void Console::onKey(int key, int action, int mods) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (key == GLFW_KEY_F5 && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
        if (activeMode_ != 0 && !activateMode(0)) {
            std::cerr << "조립 작업공간 초기화 실패.\n";
            return;
        }
        if (practiceMode_) practiceMode_->onKey(key, action, mods);
````

**after**

````cpp
    if (action == GLFW_PRESS && key == GLFW_KEY_F12) {
        workspace_.help(std::cout);
````

### 블록 4 · before L56 / after L15

위치: `void Console::onKey(int key, int action, int mods) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (!initialMode_ && action == GLFW_PRESS) {
        int next = -1;
        if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F11)
            next = 10 + key - GLFW_KEY_F1;
        else if (key == GLFW_KEY_LEFT_BRACKET)
            next = activeMode_ == 0 ? lastStandardMode : activeMode_ - 1;
        else if (key == GLFW_KEY_RIGHT_BRACKET)
            next = activeMode_ == lastStandardMode ? 0 : activeMode_ + 1;
        else if (((activeMode_ <= 9 && !(activeMode_ == 0 && practiceMode_ && practiceMode_->capturesNumericKeys())) || (mods & GLFW_MOD_ALT)) && key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
            next = key - GLFW_KEY_0;
        if (next >= 0) {
            if (!activateMode(next))
                app_.requestClose();
            return;
        }
    }
    if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
        printHelp();
````

**after**

````cpp
    if (action == GLFW_PRESS && key == GLFW_KEY_F5 && (mods & GLFW_MOD_CONTROL)) {
        workspace_.execute("reload", std::cout);
````

### 블록 5 · before L76 / after L19

위치: `void Console::onKey(int key, int action, int mods) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app_.requestClose();
        return;
````

**after**

````cpp
    workspace_.onKey(key, action, mods, std::cout);
}
void Console::onMouse(const graphics::MouseEvent& event) { workspace_.onMouse(event, std::cout); }
void Console::onCursor(double x, double y) { workspace_.onCursor(x, y, std::cout); }
void Console::onFocus(bool focused) { workspace_.onFocus(focused); }
void Console::onUpdate(float dt) {
    for (unsigned i = 0; i < 8; ++i) {
        const auto line = input_.poll();
        if (!line) break;
        if (*line == "quit") { app_.requestClose(); return; }
        workspace_.execute(*line, std::cout);
        input_.prompt();
````

### 블록 6 · before L80 / after L32

위치: `void Console::onUpdate(float dt) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (practiceMode_) {
        practiceMode_->onKey(key, action, mods);
        return;
    }
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
        pathId_.reset();
        if (!activateMode(activeMode_))
            app_.requestClose();
        return;
    }
    if (action == GLFW_REPEAT) {
        handleJointKey(key);
        return;
    }
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_V) {
        threeViews_ = !threeViews_;

        return;
    }

    if (key == GLFW_KEY_D) {
        trace_ = !trace_;
        graphics::Diagnostics::configure({true, trace_});

        return;
    }

    if (handleJointKey(key))
        return;

    if (key == GLFW_KEY_F && demo_ == Demo::Room) {
        floorOpen_ = !floorOpen_;
        if (!updateRoom(0))
            app_.requestClose();

        return;
    }

    if (demo_ != Demo::Motion) {
        if (key == GLFW_KEY_SPACE)
            clock_.playing = !clock_.playing;
        else if (key == GLFW_KEY_ESCAPE)
            app_.requestClose();
        else if (key == GLFW_KEY_H)
            printHelp();

        return;
    }

    auto& motions = app_.getScene().getMotions();
    bool ok = true;

    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;

        ok = pathId_ && spinId_ && globalId_ && motions.setMotionPlaying(*pathId_, playing) &&
             motions.setMotionPlaying(*spinId_, playing) && motions.setMotionPlaying(*globalId_, playing);

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
````

**after**

````cpp
    workspace_.update(dt, std::cout);
````

### 블록 7 · before L174 / after L34

위치: `bool Console::onRender() { return workspace_.render(); }`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp

/// 마우스 버튼 입력을 처리한다.
void Console::onMouse(const graphics::MouseEvent& event) {
    if (practiceMode_) {
        practiceMode_->onMouse(event);
        return;
    }
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }

    std::optional<graphics::PickResult> hit;

    if (threeViews_) {
        const auto size = app_.getFramebufferSize();

        if (size[0] < 3 || size[1] < 2)
            return;

        const auto passes =
            graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

        for (const auto& pass : passes) {
            const auto& v = pass.viewport;
            const double top = size[1] - v.y - v.height;

            if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top &&
                event.framebufferY < top + v.height)
                hit = graphics::pick(app_.getScene(), pass.camera, event.framebufferX, event.framebufferY,
                                     {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
        }
    } else
        hit = app_.pickAt(event.windowX, event.windowY);

    selectPicked(hit);
}

/// 조작 방법을 출력한다.
void Console::printHelp() const {
    std::cout
        << "standard: initialMode=nullopt이면 모드 전환, 정수이면 해당 모드 고정\n"
        << "[ / ] 전체 0~22 모드 순환, Alt+0~9 데모 선택, F12 공통 도움말\n"
        << "Ctrl+F5: 조립 작업공간 진입/외부 레시피 재로드 (고정 모드에서도 사용)\n"
        << "F1~F11: 모드 10~20 = 실습 13~23 (각 실습의 기존 키 사용)\n"
        << "모드 21: WASD 카메라 충돌 / 모드 22: 내장 3D 메시 입력 처리\n"
        << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
        << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
        << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
        << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
        << "왼쪽 클릭: 사람·그네·상자 전체 선택 / 빈 공간: 선택 해제\n"
        << "0 기능 조립 콘솔: help / guide / save / load / 7 Grid / 8 방·물리\n"
        << "9 선형보간·진자·사람형·그네→사람 계층 애니메이션\n"
        << "1~9: Home 전체 복원 / Tab/Backspace 객체·관절 선택, X/Y/Z 축, 좌우 회전, 상하 이동, PageUp/Down 피벗, +/- 균일 배율, R 복원\n"
        << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

bool Console::applyControls() {
    if (!graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds))
        return false;
    return demo_ == Demo::Grid ? updateGrid() : demo_ == Demo::Room ? updateRoom(0) : true;
}

void Console::onUpdate(float dt) {
    if (practiceMode_) {
        practiceMode_->onUpdate(dt);
        return;
    }
    if (demo_ != Demo::Motion && !clock_.advance(dt)) {
        app_.requestClose();
        return;
    }
    bool ok = demo_ == Demo::Animation ? updateAnimation()
        : graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds);
    if (demo_ == Demo::Grid)
        ok = updateGrid() && ok;
    else if (demo_ == Demo::Room)
        ok = updateRoom(clock_.playing ? dt : 0) && ok;
    if (!ok)
        app_.requestClose();
}

/// 현재 장면을 지정한 뷰로 렌더링한다.
void Console::onCursor(double x, double y) {
    if (practiceMode_)
        practiceMode_->onCursor(x, y);
}

void Console::onFocus(bool focused) {
    if (practiceMode_)
        practiceMode_->onFocus(focused);
}

bool Console::onRender() {
    if (practiceMode_)
        return practiceMode_->onRender();
    if (!threeViews_)
        return app_.renderScene();

    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    return app_.renderViews(
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
````

**after**

````cpp
bool Console::onRender() { return workspace_.render(); }
````

## standard/Console.h

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "framework/Framework.h"
#include "modes/PracticeMode.h"
````

**after**

````cpp
#include "modes/Assembly.h"
#include "modes/AssemblyConsole.h"
````

### 블록 2 · before L5 / after L5

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
// Standard owns the editor; reference practices do not participate in its runtime.
````

### 블록 3 · before L7 / after L8

위치: `class Console {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    std::optional<int> initialMode_;
    int activeMode_ = 1;
    std::unique_ptr<PracticeMode> practiceMode_;
    bool activateMode(int mode);
    bool initMotion(int kind);
    bool updateGrid();
    bool updateRoom(float dt);
    bool updateAnimation();
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

    Demo demo_ = Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_ = 0, roomCube_ = 0;
    std::vector<graphics::JointAnimation> animation_;

    enum class ControlKind { Object, Joint, RoomFrame, RoomBody };

    struct JointControl {
        std::size_t track = 0;
        std::string name;
        graphics::Transform3D initialRest;
        ControlKind kind = ControlKind::Object;
    };

    std::vector<JointControl> jointControls_;
    std::optional<std::size_t> roomControl_, bodyControl_;
    graphics::EntityId roomFrame_ = 0, bodyFrame_ = 0;
    glm::mat3 releasedRotation_{1};
    bool addControl(graphics::EntityId id, const std::string& name, ControlKind kind = ControlKind::Object);
    bool applyControls();
    void selectPicked(std::optional<graphics::PickResult> hit);
    std::size_t selectedJoint_ = 0;
    int selectedAxis_ = 0;


    /// 선택한 관절의 기준 SRT와 피벗을 키 입력으로 편집한다.
    bool handleJointKey(int key);


    /// 현재 선택 관절과 조작 축을 콘솔에 출력한다.
    void printSelectedJoint() const;


    bool threeViews_ = false, trace_ = false, floorOpen_ = false;


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
````

**after**

````cpp
    assembly::Assembly workspace_;
    assembly::ConsoleInput input_;
````

### 블록 4 · before L96 / after L11

위치: `class Console {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app, std::optional<int> initialMode = std::nullopt)
        : app_(app), initialMode_(initialMode) {
    }


    /// 장면 객체와 초기 상태를 구성한다.
````

**after**

````cpp
    explicit Console(graphics::Application& app) : app_(app), workspace_(app) {}
````

### 블록 5 · before L104 / after L13

위치: `void onKey(int key, int action, int mods = 0);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    void selectFromContainer(std::optional<graphics::PickResult> hit) { if (!practiceMode_) selectPicked(hit); }


    /// 키 입력에 따라 동작을 변경한다.
````

**after** (없음)


### 블록 6 · before L109 / after L14

위치: `void onMouse(const graphics::MouseEvent& event);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    void onMouse(const graphics::MouseEvent& event);
````

### 블록 7 · before L111 / after L17

위치: `void onUpdate(float dt);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp


    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);


    /// 프레임 시간으로 상태와 장면을 갱신한다.
````

**after** (없음)


### 블록 8 · before L119 / after L18

위치: `bool onRender();`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp


    /// 현재 장면을 지정한 뷰로 렌더링한다.
````

**after** (없음)


## standard/framework/Framework.h

### 블록 1 · before L8 / after L8

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp

#include "include/Dynamics.h"
````

## standard/framework/detail/Dynamics.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
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
````

## standard/framework/detail/Renderer.cpp

### 블록 1 · before L501 / after L501

위치: `glm::mat4 matrix(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    gl.Uniform1f(gl.GetUniformLocation(shaderProgramID_, config_.uniforms.directionalIntensity.c_str()), options.directionalIntensity);
    gl.Uniform1i(gl.GetUniformLocation(shaderProgramID_, config_.uniforms.pointLightCount.c_str()), int(options.pointLights.size()));
    for (std::size_t i=0; i<options.pointLights.size(); ++i) {
        const auto& light=options.pointLights[i];
        const auto prefix=config_.uniforms.pointLights + "[" + std::to_string(i) + "].";
        gl.Uniform3f(gl.GetUniformLocation(shaderProgramID_, (prefix+"position").c_str()), light.position.x,light.position.y,light.position.z);
        gl.Uniform3f(gl.GetUniformLocation(shaderProgramID_, (prefix+"color").c_str()), light.color.red,light.color.green,light.color.blue);
        gl.Uniform1f(gl.GetUniformLocation(shaderProgramID_, (prefix+"intensity").c_str()),light.intensity);
        gl.Uniform1f(gl.GetUniformLocation(shaderProgramID_, (prefix+"attenuation").c_str()),light.attenuation);
    }
````

### 블록 2 · before L660 / after L670

위치: `bool Renderer::renderPass(const Scene& scene, const RenderPass& pass, int width, int height) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (!CameraSystem::validCamera(camera) || !detail::finiteVector(options.lightDirection) ||
````

**after**

````cpp
    bool validLights=options.pointLights.size()<=8 && std::isfinite(options.directionalIntensity) && options.directionalIntensity>=0;
    for (const auto& light:options.pointLights)
        validLights=validLights && detail::finiteVector(light.position) && detail::validColor(light.color) &&
            std::isfinite(light.intensity) && light.intensity>=0 && std::isfinite(light.attenuation) && light.attenuation>=0;
    if (!validLights || !CameraSystem::validCamera(camera) || !detail::finiteVector(options.lightDirection) ||
````

## standard/framework/detail/Scene.cpp

### 블록 1 · before L339 / after L339

위치: `bool Scene::setParent(EntityId id, std::optional<EntityId> parent) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
namespace {
glm::mat4 inversePlacement(const glm::mat4& m) {
    const auto linear=glm::inverse(glm::mat3(m));
    glm::mat4 result(linear);
    result[3]=glm::vec4(-linear*glm::vec3(m[3]),1);
    return result;
}
bool invertiblePlacement(const glm::mat4& m) {
    return TransformSystem::validAffine(m) && std::abs(glm::determinant(glm::mat3(m))) > 1e-18f;
}
}
std::optional<glm::mat4> Scene::placementMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto& c = components_[id];
    const auto parent = c.parent ? worldMatrix(*c.parent) : std::optional<glm::mat4>{glm::mat4(1)};
    if (!parent) return {};
    const auto matrix = *parent * c.attachment * detail::transformMatrix(c.transform);
    return TransformSystem::validAffine(matrix) ? std::optional<glm::mat4>{matrix} : std::nullopt;
}
bool Scene::setAttachment(EntityId id, const glm::mat4& matrix) {
    if (id >= components_.size() || !invertiblePlacement(matrix)) return false;
    components_[id].attachment = matrix;
    return true;
}
bool Scene::setLocalLayers(EntityId id, const std::vector<glm::mat4>& layers) {
    if (id >= components_.size() || layers.size() > 128) return false;
    glm::mat4 composed(1);
    for (const auto& layer : layers) {
        composed *= layer;
        if (!invertiblePlacement(layer) || !invertiblePlacement(composed)) return false;
    }
    components_[id].localLayers = layers;
    return true;
}
bool Scene::setWorldPlacement(EntityId id, const glm::mat4& world) {
    if (id >= components_.size() || !invertiblePlacement(world)) return false;
    const auto& c = components_[id];
    const auto parent = c.parent ? worldMatrix(*c.parent) : std::optional<glm::mat4>{glm::mat4(1)};
    const auto base = detail::transformMatrix(c.transform);
    if (!parent || !invertiblePlacement(*parent) || !invertiblePlacement(base)) return false;
    return setAttachment(id, inversePlacement(*parent) * world * inversePlacement(base));
}
bool Scene::setParentPreserveWorld(EntityId id, std::optional<EntityId> parent) {
    const auto placement = placementMatrix(id);
    if (!placement) return false;
    const auto oldParent = components_[id].parent;
    const auto oldAttachment = components_[id].attachment;
    if (!setParent(id, parent)) return false;
    if (setWorldPlacement(id, *placement)) return true;
    components_[id].parent = oldParent;
    components_[id].attachment = oldAttachment;
    return false;
}
````

### 블록 2 · before L388 / after L442

위치: `glm::mat4 prefix(1), suffix(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        matrix *= detail::transformMatrix(components_[chain[i]].transform);
````

**after**

````cpp
        const auto& component = components_[chain[i]];
        matrix *= component.attachment * detail::transformMatrix(component.transform);
        for (const auto& local : component.localLayers) matrix *= local;
````

## standard/framework/detail/Shaders.h

### 블록 1 · before L15 / after L15

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
out vec3 WorldPosition;
````

### 블록 2 · before L20 / after L21

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    WorldPosition = (uModel * vec4(vPos, 1.0)).xyz;
````

### 블록 3 · before L26 / after L28

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
in vec3 WorldPosition;
````

### 블록 4 · before L31 / after L34

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
uniform float uDirectionalIntensity;
struct PointLight { vec3 position; vec3 color; float intensity; float attenuation; };
uniform int uPointLightCount;
uniform PointLight uPointLights[8];
````

### 블록 5 · before L39 / after L46

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (uLighting && length(Normal) > 0.0)
        color.rgb *= (0.25 + 0.75 * max(dot(normalize(Normal), normalize(uLightDirection)), 0.0)) * uLightColor;
````

**after**

````cpp
    if (uLighting && length(Normal) > 0.0) {
        vec3 n = normalize(Normal);
        vec3 illumination = (0.25 + 0.75 * max(dot(n, normalize(uLightDirection)), 0.0)) * uLightColor * uDirectionalIntensity;
        for (int i = 0; i < uPointLightCount; ++i) {
            vec3 delta = uPointLights[i].position - WorldPosition;
            float distanceSquared = dot(delta, delta);
            vec3 direction = delta / sqrt(max(distanceSquared, 1e-12));
            illumination += uPointLights[i].color * uPointLights[i].intensity * max(dot(n, direction), 0.0)
                / (1.0 + uPointLights[i].attenuation * distanceSquared);
        }
        color.rgb *= illumination;
    }
````

## standard/framework/include/Dynamics.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
#pragma once
#include "Model.h"
#include <functional>

namespace graphics {
struct ReferenceFrame {
    glm::mat3 rotation{1}; // Local to world; orthonormal, determinant +1.
    glm::vec3 acceleration{0}; // World units/s^2 of the frame origin.
    glm::vec3 angularVelocity{0}, angularAcceleration{0}; // World radians/s and radians/s^2.
};
struct ParticleState {
    glm::vec3 position{0}, velocity{0}; // Frame-local coordinates.
    float mass = 1;
};
using ForceFunction = std::function<glm::vec3(const ParticleState&,double)>; // Local newtons, time in seconds.

class DynamicsSystem {
public:
    static bool validFrame(const ReferenceFrame& frame);
    /// Gravity, translation, centrifugal, Coriolis and Euler terms in frame-local coordinates.
    static std::optional<glm::vec3> effectiveAcceleration(const ReferenceFrame& frame,
        glm::vec3 localPosition, glm::vec3 localVelocity, glm::vec3 gravityWorld = {0,-9.8f,0});
    /// Semi-implicit Euler, <=1/240s substeps. Commits only a fully finite result.
    static bool step(ParticleState& particle, const ReferenceFrame& frame, float dt,
        glm::vec3 gravityWorld = {0,-9.8f,0}, const ForceFunction& force = {}, double startTime = 0);
    /// Force required to hold a point stationary in this frame; local newtons.
    static std::optional<glm::vec3> holdingForce(const ReferenceFrame& frame,
        glm::vec3 localPosition, float mass, glm::vec3 gravityWorld = {0,-9.8f,0});
    /// Compression-only reading along a scale's local surface normal.
    static std::optional<float> normalLoad(const ReferenceFrame& frame, glm::vec3 localPosition,
        float mass, glm::vec3 normal = {0,1,0}, glm::vec3 gravityWorld = {0,-9.8f,0});
    /// Linear, zero-rest-length spring and viscous damper; all values in frame-local units.
    static ForceFunction spring(glm::vec3 anchor, float stiffness, float damping);
};
} // namespace graphics
````

## standard/framework/include/Graphics.h

### 블록 1 · before L108 / after L108

위치: `bool setParent(EntityId id, std::optional<EntityId> parent);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp

    /// [API] Full affine placement before this object's local layers; parent's final layers are inherited.
    std::optional<glm::mat4> placementMatrix(EntityId id) const;
    /// [API] Sets world-authored placement without decomposing a potentially sheared matrix.
    bool setWorldPlacement(EntityId id, const glm::mat4& world);
    /// [API] Changes parent while preserving placement and this object's local layer list.
    bool setParentPreserveWorld(EntityId id, std::optional<EntityId> parent);
    /// [API] Sets the parent-relative affine attachment, before base SRT.
    bool setAttachment(EntityId id, const glm::mat4& matrix);
    /// [API] Replaces ordered local layers atomically. Each layer inherits all preceding layers.
    bool setLocalLayers(EntityId id, const std::vector<glm::mat4>& layers);
````

### 블록 2 · before L358 / after L370

위치: `struct PointLight {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
struct PointLight {
    glm::vec3 position{0};
    GlColor color{1,1,1};
    float intensity = 1, attenuation = .02f;
};
````

### 블록 3 · before L368 / after L386

위치: `struct RenderOptions {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    float directionalIntensity = 1;
    std::vector<PointLight> pointLights; // At most eight world-space lights in the default shader.
````

## standard/framework/include/Model.h

### 블록 1 · before L146 / after L146

위치: `struct Component_Poly {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    glm::mat4 attachment{1.0f}; // Object-owned affine placement, including inherited shear.
    std::vector<glm::mat4> localLayers; // Ordered, object-local transformations.
````

## standard/framework/include/RenderConfig.h

### 블록 1 · before L37 / after L37

위치: `struct UniformBindings {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    std::string pointLightCount = "uPointLightCount", pointLights = "uPointLights";
    std::string directionalIntensity = "uDirectionalIntensity";
````

## standard/framework/shaders/FragmentShader.glsl

### 블록 1 · before L5 / after L5

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````glsl
in vec3 WorldPosition;
````

### 블록 2 · before L10 / after L11

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````glsl
uniform float uDirectionalIntensity;
struct PointLight { vec3 position; vec3 color; float intensity; float attenuation; };
uniform int uPointLightCount;
uniform PointLight uPointLights[8];
````

### 블록 3 · before L14 / after L19

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl

````

**after** (없음)


### 블록 4 · before L16 / after L20

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl
    if (uOverrideColor) {
        FragColor = vec4(uOutlineColor, 1.0);

        return;
````

**after**

````glsl
    if (uOverrideColor) { FragColor = vec4(uOutlineColor, 1.0); return; }
    vec4 color = vec4(outColor * uTint, 1.0);
    if (uHasTexture) color *= texture(uTexture, TexCoord);
    if (uLighting && length(Normal) > 0.0) {
        vec3 n = normalize(Normal);
        vec3 illumination = (0.25 + 0.75 * max(dot(n, normalize(uLightDirection)), 0.0)) * uLightColor * uDirectionalIntensity;
        for (int i = 0; i < uPointLightCount; ++i) {
            vec3 delta = uPointLights[i].position - WorldPosition;
            float distanceSquared = dot(delta, delta);
            vec3 direction = delta / sqrt(max(distanceSquared, 1e-12));
            illumination += uPointLights[i].color * uPointLights[i].intensity * max(dot(n, direction), 0.0)
                / (1.0 + uPointLights[i].attenuation * distanceSquared);
        }
        color.rgb *= illumination;
````

### 블록 5 · before L21 / after L35

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl
    vec4 color = vec4(outColor * uTint, 1.0);

    if (uHasTexture)
        color *= texture(uTexture, TexCoord);

    if (uLighting && length(Normal) > 0.0)
        color.rgb *= (0.25 + 0.75 * max(dot(normalize(Normal), normalize(uLightDirection)), 0.0)) * uLightColor;
````

**after** (없음)


## standard/framework/shaders/VertexShader.glsl

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl
layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;
layout(location = 2) in vec2 vTexCoord;
layout(location = 3) in vec3 vNormal;
````

**after**

````glsl
layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
layout(location=2) in vec2 vTexCoord;
layout(location=3) in vec3 vNormal;
````

### 블록 2 · before L12 / after L12

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````glsl

````

**after**

````glsl
out vec3 WorldPosition;
````

### 블록 3 · before L18 / after L18

위치: `void main() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````glsl
    WorldPosition = (uModel * vec4(vPos, 1.0)).xyz;
````

## standard/main.cpp

### 블록 1 · before L8 / after L8

위치: `int main() try {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    // nullopt: 숫자 키로 전환. 0: 조립 콘솔 고정. 1~9: 기본 데모 고정. 10~22: 추가 실습·부록 고정.
    constexpr std::optional<int> initialMode = std::nullopt;
    static_assert(!initialMode || (*initialMode >= 0 && *initialMode <= lastStandardMode), "initialMode must be 0..22");
    Console sample(app, initialMode);
````

**after**

````cpp
    Console sample(app);
````

### 블록 2 · before L37 / after L34

위치: `int main() try {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    config.title = "standard - Modes 0..22, input, picking and practice patterns";
````

**after**

````cpp
    config.title = "Standard - Scene workspace (mode 0)";
````

## standard/modes/Assembly.cpp

### 블록 1 · before L102 / after L102

위치: `void Assembly::clear() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    editStart_.reset(); editScene_.reset(); editOperation_=0; editCursor_.reset();
````

### 블록 2 · before L104 / after L105

위치: `void Assembly::clear() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    app_.getRenderOptions().pointLights.clear();
````

### 블록 3 · before L140 / after L142

위치: `void Assembly::attach(Behavior behavior) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Conveyor)
````

**after**

````cpp
    if (behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Conveyor || behavior.effect == Effect::Dynamics)
````

### 블록 4 · before L168 / after L170

위치: `void Assembly::attach(Behavior behavior) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        const bool currentPhysics = current.effect == Effect::Body || current.effect == Effect::Walk || current.effect == Effect::Custom;
        const bool newPhysics = behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Custom;
````

**after**

````cpp
        const bool currentPhysics = current.effect == Effect::Body || current.effect == Effect::Walk || current.effect == Effect::Custom || current.effect == Effect::Dynamics;
        const bool newPhysics = behavior.effect == Effect::Body || behavior.effect == Effect::Walk || behavior.effect == Effect::Custom || behavior.effect == Effect::Dynamics;
````

### 블록 5 · before L201 / after L203

위치: `void Assembly::setRest(const std::string& target, const graphics::Transform3D& rest) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    if (const auto* driver=positionDriver(target); driver && driver->effect==Effect::Dynamics) {
        require(rest.basis==value.rest.basis && rest.localBasis==value.rest.localBasis && rest.scale==value.rest.scale &&
            rest.rotation[0]==value.rest.rotation[0] && rest.rotation[1]==value.rest.rotation[1] && rest.rotation[2]==value.rest.rotation[2],
            "힘 제어 루트는 force position/velocity로 이동하고 형상은 메시 자식에서 편집하세요.");
    }
````

### 블록 6 · before L217 / after L224

위치: `glm::all(glm::lessThanEqual(glm::abs(rest.localBasis), glm::vec3(1000000))), "위치/피벗 범위는 ±1000000입니다.");`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    const bool validScale = value.affineGroup
        ? glm::all(glm::greaterThan(glm::abs(rest.scale), glm::vec3(.0001f))) &&
          glm::all(glm::lessThanEqual(glm::abs(rest.scale), glm::vec3(100)))
        : rest.scale.x > .0001f && rest.scale.x <= 100 && std::abs(rest.scale.x - rest.scale.y) < .0001f &&
          std::abs(rest.scale.x - rest.scale.z) < .0001f;
````

**after**

````cpp
    const bool validScale = glm::all(glm::greaterThan(glm::abs(rest.scale), glm::vec3(.0001f))) &&
        glm::all(glm::lessThanEqual(glm::abs(rest.scale), glm::vec3(100)));
````

### 블록 7 · before L223 / after L227

위치: `glm::all(glm::lessThanEqual(glm::abs(rest.scale), glm::vec3(100)));`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
            "유효한 SRT가 필요합니다. 기본은 양수 균일 배율이며 warp 그룹은 0이 아닌 축별 배율(.0001~100)을 씁니다.");
````

**after**

````cpp
            "배율은 각 축 절댓값 .0001~100이며, 유효한 SRT가 필요합니다.");
````

### 블록 8 · before L272 / after L276

위치: `void Assembly::inspect(const std::string& target, std::ostream& out) const {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        << t.basis.x << ',' << t.basis.y << ',' << t.basis.z << " scale=" << t.scale.x << '\n';
````

**after**

````cpp
        << t.basis.x << ',' << t.basis.y << ',' << t.basis.z << " scale=" << t.scale.x << ',' << t.scale.y << ',' << t.scale.z << '\n';
````

### 블록 9 · before L295 / after L299

위치: `bool Assembly::execute(const std::string& line, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    const bool readOnly = args[0] == "save" || args[0] == "guide" || args[0] == "help" || args[0] == "list" || args[0] == "inspect" || args[0] == "surface" || args[0] == "diagnose" ||
        (args[0] == "feature" && args.size() > 1 && (args[1] == "list" || args[1] == "parts"));
````

**after**

````cpp
    const bool readOnly = args[0] == "save" || args[0] == "guide" || args[0] == "help" || args[0] == "list" || args[0] == "inspect" || args[0] == "surface" || args[0] == "diagnose" || (args[0] == "force" && args.size()>1 && args[1] == "read");
````

### 블록 10 · before L298 / after L301

위치: `bool Assembly::execute(const std::string& line, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
        args = resolveCommand(args);
````

### 블록 11 · before L301 / after L305

위치: `bool Assembly::execute(const std::string& line, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        if (!replaying_ && !readOnly && args[0] != "load" && args[0] != "reload" && args[0] != "select" && args[0] != "jump" && args[0] != "release" && !(args[0] == "feature" && args.size() > 1 && args[1] == "key"))
            state_.recipe.push_back(line);
````

**after**

````cpp
        if (!replaying_ && !readOnly && args[0] != "load" && args[0] != "reload" && args[0] != "select" && args[0] != "jump" && args[0] != "release")
        {
            std::string canonical;
            for (const auto& arg : args) { if (!canonical.empty()) canonical += ' '; canonical += quote(arg); }
            state_.recipe.push_back(canonical);
        }
````

### 블록 12 · before L304 / after L312

위치: `bool Assembly::execute(const std::string& line, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
        if (!replaying_ && !readOnly) {
            out << line << " [적용 완료] - 해석 인수:";
            for (const auto& arg:args) out << ' ' << quote(arg);
            out << ", 선택: " << (state_.selected.empty() ? "없음" : state_.selected);
            if (!state_.selectedLayer.empty()) out << ", 지역 레이어: " << state_.selectedLayer;
            out << '\n';
            if (!state_.selected.empty() && state_.nodes.count(state_.selected)) {
                const auto world = app_.getScene().placementMatrix(node(state_.selected).id);
                if (world) out << "월드 원점=" << (*world)[3].x << ',' << (*world)[3].y << ',' << (*world)[3].z << '\n';
            }
        }
````

## standard/modes/Assembly.h

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "FeatureContainer.h"
````

**after**

````cpp
#include "../framework/Framework.h"
````

### 블록 2 · before L19 / after L18

위치: `struct LocalLayer {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
struct LocalLayer {
    std::string name;
    glm::mat4 matrix{1};
    graphics::Motion motion;
    double seconds = 0;
    bool playing = true, enabled = true;
};
````

### 블록 3 · before L30 / after L36

위치: `struct Node {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    glm::mat4 attachment{1};
    std::vector<LocalLayer> layers;
    std::optional<graphics::PointLight> light;
    std::optional<graphics::Camera> camera;
````

### 블록 4 · before L32 / after L42

위치: `struct Node {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
enum class Effect { Motion, Face, FaceTrack, Wall, Body, Walk, Conveyor, Custom, Input };
````

**after**

````cpp
enum class Effect { Motion, Face, FaceTrack, Wall, Body, Walk, Conveyor, Custom, Input, Dynamics };
````

### 블록 5 · before L49 / after L59

위치: `struct Behavior {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    graphics::ParticleState particle;
    glm::mat4 particleShape{1};
    graphics::ReferenceFrame frame;
    graphics::ForceFunction force;
    glm::vec3 springAnchor{0}, previousOriginVelocity{0}, previousOmega{0}, lastForce{0};
    float stiffness=0, damping=0;
    int frameSamples=0;
    std::optional<graphics::LineId> tether;
````

### 블록 6 · before L74 / after L92

위치: `struct State {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        std::vector<FeatureInstance> features;
        std::string featureFocus, featureCamera;
````

**after** (없음)


### 블록 7 · before L78 / after L94

위치: `struct State {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        std::string selected;
````

**after**

````cpp
        std::string selected, selectedLayer, activeCamera;
        glm::vec3 cursor{0}, planePoint{0}, planeNormal{0,1,0};
        float snapSpacing = 0;
        std::optional<std::array<graphics::LineId,3>> axes;
        std::string editMode = "object";
        std::optional<graphics::PickResult> faceSelection;
````

### 블록 8 · before L91 / after L112

위치: `struct State {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    std::optional<State> editStart_;
    std::optional<graphics::Scene> editScene_;
    char editOperation_ = 0;
    std::optional<double> editCursor_;
    Tokens resolveCommand(const Tokens& args) const;
    bool sceneToolCommand(const Tokens& args, std::ostream& out);
    void syncSceneTools();
    bool workspaceCommand(const Tokens& args, std::ostream& out);
    void editStep(float amount, std::ostream& out);
    void finishEdit(bool keep, std::ostream& out);
    void setWorld(const std::string& target, const glm::mat4& matrix);
````

### 블록 9 · before L92 / after L124

위치: `bool dynamicsCommand(const Tokens& args, std::ostream& out);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    bool dynamicsCommand(const Tokens& args, std::ostream& out);
    void advanceDynamics(Behavior& behavior, float dt);
````

### 블록 10 · before L97 / after L131

위치: `bool inputCommand(const Tokens& args, std::ostream& out);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    bool featureCommand(const Tokens& args, std::ostream& out);
    void advanceFeatures(float dt);
    void syncFeatures();
    void featureAppearance(graphics::EntityId id, std::optional<graphics::GlColor> color, std::optional<bool> visible);
    bool routeFeatureKey(int key, int action, int mods, std::ostream& out);
    bool routeFeatureMouse(const graphics::MouseEvent& event, std::ostream& out);
    bool routeFeatureCursor(double x, double y);
    void queueFeatureFocus(bool value);
    void queueFeatureEvent(FeatureInstance& feature, FeatureEvent event);
````

**after** (없음)


### 블록 11 · before L124 / after L149

위치: `void add(const std::string& kind, const std::string& name, glm::vec3 position,`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    void addFeature(const std::string& name, int mode);
    void portFeature(const std::string& source, graphics::EntityId native, const std::string& name);
````

**after** (없음)


### 블록 12 · before L152 / after L175

위치: `void guide(const std::string& topic, std::ostream& out) const;`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    bool capturesNumericKeys() const { return !state_.featureFocus.empty(); }
````

**after** (없음)


## standard/modes/AssemblyCommands.cpp

### 블록 1 · before L10 / after L10

위치: `bool Assembly::objectCommand(const Tokens& a, std::ostream&) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
        state_.selected = a[2]; state_.selectedLayer.clear();
````

### 블록 2 · before L13 / after L14

위치: `bool Assembly::objectCommand(const Tokens& a, std::ostream&) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        require(a.size() == 3, "parent <자식> <부모 또는 none>. 좌표는 부모 로컬 값으로 유지합니다.");
````

**after**

````cpp
        require(a.size() == 3, "parent <자식> <부모 또는 none>. 현재 월드 배치와 지역 레이어를 유지합니다.");
````

### 블록 3 · before L17 / after L18

위치: `bool Assembly::objectCommand(const Tokens& a, std::ostream&) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        require(!driver || (driver->effect != Effect::Body && driver->effect != Effect::Walk), "물리 루트에는 부모를 추가할 수 없습니다.");
````

**after**

````cpp
        require(!driver || (driver->effect != Effect::Body && driver->effect != Effect::Walk && driver->effect != Effect::Dynamics && driver->effect != Effect::Custom), "물리 루트에는 부모를 추가할 수 없습니다.");
````

### 블록 4 · before L25 / after L26

위치: `bool Assembly::objectCommand(const Tokens& a, std::ostream&) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        require(app_.getScene().setParent(child.id, parent), "부모 연결 실패. 순환 여부를 확인하세요.");
````

**after**

````cpp
        require(app_.getScene().setParentPreserveWorld(child.id, parent), "부모 연결 실패. 순환 여부를 확인하세요.");
        child.attachment = app_.getScene().getComponent(child.id)->attachment;
````

### 블록 5 · before L41 / after L43

위치: `bool Assembly::objectCommand(const Tokens& a, std::ostream&) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
                if (command == "color") featureAppearance(id, graphics::GlColor{color.x, color.y, color.z}, {});
                else featureAppearance(id, {}, visible);
````

**after** (없음)


### 블록 6 · before L61 / after L61

위치: `bool Assembly::objectCommand(const Tokens& a, std::ostream&) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
            require(a.size() == 3, "scale <대상> <양수 균일 배율>");
            rest.scale = glm::vec3(float(number(a[2])));
````

**after**

````cpp
            require(a.size() == 3 || a.size() == 5, "scale <대상> <균일값 또는 SX SY SZ>");
            rest.scale = a.size() == 3 ? glm::vec3(float(number(a[2]))) : vector(a, 2);
````

### 블록 7 · before L83 / after L83

위치: `bool Assembly::dispatch(const Tokens& a, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    if (dynamicsCommand(a, out) || sceneToolCommand(a, out) || workspaceCommand(a, out)) return true;
````

### 블록 8 · before L109 / after L110

위치: `bool Assembly::dispatch(const Tokens& a, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        if (a[0] == "select") state_.selected = a[1];
````

**after**

````cpp
        if (a[0] == "select") { state_.selected = a[1]; state_.selectedLayer.clear(); }
````

### 블록 9 · before L131 / after L132

위치: `bool Assembly::dispatch(const Tokens& a, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (featureCommand(a, out) || surfaceCommand(a, out) || trackCommand(a, out) || hierarchyCommand(a, out) || objectCommand(a, out) || motionCommand(a, out) || inputCommand(a, out))
````

**after**

````cpp
    if (surfaceCommand(a, out) || trackCommand(a, out) || hierarchyCommand(a, out) || objectCommand(a, out) || motionCommand(a, out) || inputCommand(a, out))
````

## standard/modes/AssemblyConsole.h

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
````

**after**

````cpp
#include "../framework/Framework.h"
````

## standard/modes/AssemblyDiagnostics.cpp

### 블록 1 · before L39 / after L39

위치: `void Assembly::diagnose(const std::string& target, std::ostream& out) const {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    for (const auto& feature : state_.features) {
        bool member = feature.name == target;
        for (const auto& part : feature.entities) member = member || part.second.group == object.id || part.second.mesh == object.id;
        if (member) out << "feature owner=" << feature.name << " source mode=" << feature.program->mode()
            << " playing=" << feature.playing << " enabled=" << feature.enabled << " focus=" << (state_.featureFocus == feature.name)
            << " geometry-owner=native; authoring=extra SRT/face motion\n";
    }
````

**after** (없음)


### 블록 2 · before L49 / after L42

위치: `void Assembly::diagnose(const std::string& target, std::ostream& out) const {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    out << "1) pause on / feature play <id> off 2) surface <mesh> <face> 3) guide <feature>\n"
        << "4) inspect rest/pivot/parent/determinant; compare private controller vs extra authoring pose\n"
        << "5) recipe edit: reload / Ctrl+F5; C++ edit: rebuild by user, then recreate.\n";
````

**after**

````cpp
    out << "pause on [시간 정지] - on/off: 시간 진행 여부\n"
        << "diagnose <이름> [현재 행렬과 상태] - 이름: 객체 또는 부품\n";
````

## standard/modes/AssemblyDynamics.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
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
````

## standard/modes/AssemblyFeatureInput.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Assembly.h"
#include <algorithm>

namespace assembly {
void Assembly::queueFeatureEvent(FeatureInstance& feature, FeatureEvent event) {
    if (feature.events.size() >= 256) {
        feature.events.clear();
        FeatureEvent release; release.kind = FeatureEvent::Kind::Focus; release.focused = false;
        feature.events.push_back(release);
        return;
    }
    feature.events.push_back(std::move(event));
}
void Assembly::queueFeatureFocus(bool value) {
    for (auto& feature : state_.features) if (feature.name == state_.featureFocus) {
        FeatureEvent event; event.kind = FeatureEvent::Kind::Focus; event.focused = value;
        queueFeatureEvent(feature, event);
    }
}
bool Assembly::routeFeatureKey(int key, int action, int mods, std::ostream&) {
    for (auto& f : state_.features) if (f.name == state_.featureFocus && f.enabled) {
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) return true;
        if (key == GLFW_KEY_V && f.program->mode() <= 9) {
            if (action == GLFW_PRESS) state_.threeViews = !state_.threeViews;
            return true;
        }
        FeatureEvent event; event.kind = FeatureEvent::Kind::Key;
        event.key = key; event.action = action; event.mods = mods;
        queueFeatureEvent(f, event);
        return true;
    }
    return false;
}
bool Assembly::routeFeatureMouse(const graphics::MouseEvent& original, std::ostream& out) {
    for (auto& f : state_.features) if (f.name == state_.featureFocus && f.enabled) {
        try {
            if (f.program->mode() <= 9 && original.button == GLFW_MOUSE_BUTTON_LEFT && original.action == GLFW_PRESS) {
                std::optional<graphics::PickResult> hit;
                const auto size = app_.getFramebufferSize();
                if (state_.threeViews && size[0] >= 3 && size[1] >= 2) {
                    for (const auto& pass : graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions())) {
                        const auto& v = pass.viewport;
                        const double top = size[1] - v.y - v.height;
                        if (original.framebufferX >= v.x && original.framebufferX < v.x + v.width && original.framebufferY >= top && original.framebufferY < top + v.height)
                            hit = graphics::pick(app_.getScene(), pass.camera, original.framebufferX, original.framebufferY,
                                {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
                    }
                } else hit = app_.pickAt(original.windowX, original.windowY);
                FeatureEvent pending; pending.kind = FeatureEvent::Kind::Pick;
                if (hit) for (const auto& instance : state_.features) if (instance.program == f.program && instance.enabled)
                    for (const auto& entity : instance.entities) if (entity.second.mesh == hit->entityId) {
                        pending.hit = *hit; pending.hit->entityId = entity.first;
                    }
                queueFeatureEvent(f, pending);
                return true;
            }
            FeatureEvent pending; pending.kind = FeatureEvent::Kind::Mouse;
            pending.mouse = original; pending.view = f.program->camera();
            queueFeatureEvent(f, pending);
        } catch (const std::exception& error) { out << "기능 입력 거절: " << error.what() << '\n'; }
        return true;
    }
    return false;
}
bool Assembly::routeFeatureCursor(double x, double y) {
    for (auto& f : state_.features) if (f.name == state_.featureFocus && f.enabled) {
        FeatureEvent event; event.kind = FeatureEvent::Kind::Cursor; event.x = x; event.y = y;
        queueFeatureEvent(f, event); return true;
    }
    return false;
}
} // namespace assembly
````

**after** (없음)


## standard/modes/AssemblyFeatures.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Assembly.h"
#include <algorithm>
#include <limits>
#include <set>

namespace assembly {
namespace {
bool contains(const FeatureInstance& feature, graphics::EntityId id) {
    if (!feature.branch) return true;
    std::optional<graphics::EntityId> current = id;
    while (current) {
        if (*current == *feature.branch) return true;
        const auto* component = feature.program->scene().getComponent(*current);
        current = component ? component->parent : std::nullopt;
    }
    return false;
}
std::string entityName(const FeatureInstance& f, graphics::EntityId id) {
    return f.name + ".e" + std::to_string(id);
}
graphics::Model exportedModel(const graphics::Model& source) {
    auto result = source;
    for (auto& face : result.faces) {
        face.independence = false;
        face.transform = graphics::Transform3D{};
        face.transformMode = graphics::FaceTransformMode::Hierarchy;
        face.transformFrame = glm::mat4(1);
        face.localAnimationMatrix = glm::mat4(1);
        face.animationSpace = graphics::FaceAnimationSpace::Model;
    }
    return result;
}
std::optional<graphics::Transform3D> srtMatrix(const glm::mat4& matrix) {
    graphics::Transform3D t;
    t.basis = glm::vec3(matrix[3]);
    t.scale = {glm::length(glm::vec3(matrix[0])), glm::length(glm::vec3(matrix[1])), glm::length(glm::vec3(matrix[2]))};
    if (glm::any(glm::lessThanEqual(t.scale, glm::vec3(1e-8f)))) return {};
    if (glm::determinant(glm::mat3(matrix)) < 0) t.scale.x = -t.scale.x;
    for (int i = 0; i < 3; ++i) t.rotation[i] = glm::vec3(matrix[i]) / t.scale[i];
    return graphics::DiagramSystem::validTransform(t) ? std::optional<graphics::Transform3D>{t} : std::nullopt;
}
}

void Assembly::addFeature(const std::string& name, int mode) {
    require(state_.features.size() < 64 && mode >= 1 && mode <= lastStandardMode, "기능은 모드1~22, 인스턴스64개 이하여야 합니다.");
    group(name, {0, 0, 0});
    node(name).guideTopic = "mode" + std::to_string(mode);
    FeatureInstance feature;
    feature.name = name; feature.program = std::make_shared<FeatureContainer>(app_, mode);
    state_.features.push_back(std::move(feature));
}

void Assembly::portFeature(const std::string& source, graphics::EntityId native, const std::string& name) {
    require(state_.features.size() < 64, "기능 인스턴스64개 한도입니다.");
    const auto found = std::find_if(state_.features.begin(), state_.features.end(), [&](const auto& f) { return f.name == source; });
    require(found != state_.features.end() && found->program->scene().getComponent(native), "원본 기능과 native 엔티티 ID를 확인하세요.");
    FeatureInstance port;
    port.name = name; port.program = found->program; port.branch = native; port.playing = found->playing;
    group(name, {0, 0, 0}); node(name).guideTopic = "mode" + std::to_string(port.program->mode());
    state_.features.push_back(std::move(port));
}

bool Assembly::featureCommand(const Tokens& a, std::ostream& out) {
    if (a[0] != "feature") return false;
    require(a.size() >= 2, "feature add/port/list/parts/focus/camera/play/enable/reset/key ...");
    const auto find = [&](const std::string& name) -> FeatureInstance& {
        const auto it = std::find_if(state_.features.begin(), state_.features.end(), [&](const auto& f) { return f.name == name; });
        require(it != state_.features.end(), "없는 기능 컨테이너입니다: " + name);
        return *it;
    };
    if (a[1] == "add") {
        require(a.size() == 4 && state_.features.size() < 64, "feature add <이름> <모드1~22>. 컨테이너 한도64개.");
        const auto mode = indexNumber(a[3]);
        require(mode >= 1 && mode <= lastStandardMode, "feature 모드는 1~22입니다.");
        addFeature(a[2], int(mode));
        return true;
    }
    if (a[1] == "snapshot") {
        require(a.size() == 5, "feature snapshot <컨테이너> <nativeID> <새이름>");
        const auto& f = find(a[2]); const auto id = indexNumber(a[3]);
        const auto* c = f.program->scene().getComponent(id);
        require(c && !c->polyhedron.faces.empty(), "메시가 있는 native 엔티티 ID가 필요합니다.");
        auto geometry = exportedModel(c->polyhedron);
        for (std::size_t i = 0; i < geometry.faces.size(); ++i) {
            const auto matrix = f.program->scene().faceWorldMatrix(id, i);
            require(matrix.has_value(), "스냅샷 면 행렬이 없습니다.");
            const auto determinant = glm::determinant(glm::mat3(*matrix));
            require(std::abs(determinant) > 1e-18f, "특이한 면은 스냅샷으로 만들 수 없습니다.");
            auto& face = geometry.faces[i];
            if (face.vertexNormals.empty()) graphics::recalculateNormals(face);
            const auto normal = glm::transpose(glm::inverse(glm::mat3(*matrix)));
            for (auto& p : face.localVertices) p = glm::vec3(*matrix * glm::vec4(p, 1));
            for (auto& n : face.vertexNormals) if (glm::length(n) > 0) n = glm::normalize(normal * n);
            face.normal = glm::length(face.normal) > 0 ? glm::normalize(normal * face.normal) : glm::vec3(0);
            if (determinant < 0) {
                face.triangleIndices = graphics::detail::triangles(face);
                for (std::size_t t = 0; t < face.triangleIndices.size(); t += 3) std::swap(face.triangleIndices[t + 1], face.triangleIndices[t + 2]);
            }
        }
        model(a[4], geometry);
        require(app_.getScene().setTint(*node(a[4]).mesh, c->tint), "스냅샷 색 적용 실패.");
        require(app_.getScene().setParent(node(a[4]).id, node(f.name).id), "스냅샷 배치 부모 연결 실패.");
        return true;
    }
    if (a[1] == "port") {
        require(a.size() == 5 && state_.features.size() < 64, "feature port <기존이름> <native엔티티ID> <새이름>");
        portFeature(a[2], indexNumber(a[3]), a[4]);
        out << "라이브 포트는 원본의 상태/시계를 공유합니다. 독립 인스턴스는 feature add를 사용하세요.\n";
        return true;
    }
    if (a[1] == "list") {
        require(a.size() == 2, "feature list");
        for (const auto& f : state_.features)
            out << f.name << " mode=" << f.program->mode() << " enabled=" << f.enabled << " playing=" << f.playing
                << (f.branch ? " live-port" : " independent") << '\n';
        return true;
    }
    require(a.size() >= 3, "feature <동작> <이름 또는 none> ...");
    if (a[1] == "focus" || a[1] == "camera") {
        require(a.size() == 3, "feature focus/camera <이름 또는 none>");
        if (a[2] != "none") require(find(a[2]).enabled, "enabled 컨테이너를 선택하세요.");
        if (a[1] == "focus") {
            queueFeatureFocus(false);
            state_.featureFocus = a[2] == "none" ? "" : a[2];
            queueFeatureFocus(true);
        } else state_.featureCamera = a[2] == "none" ? "" : a[2];
        return true;
    }
    auto& f = find(a[2]);
    if (a[1] == "parts") {
        require(a.size() == 3, "feature parts <이름>");
        const auto& scene = f.program->scene();
        for (auto id : scene.getIds()) if (contains(f, id)) {
            const auto* c = scene.getComponent(id);
            out << entityName(f, id) << " native=" << id << " faces=" << c->polyhedron.faces.size();
            if (c->parent) out << " nativeParent=" << *c->parent;
            out << '\n';
        }
        return true;
    }
    if (a[1] == "play" || a[1] == "enable") {
        require(a.size() == 4, "feature play/enable <이름> on/off");
        const bool value = boolean(a[3]);
        if (a[1] == "play") {
            for (auto& item : state_.features) if (item.program == f.program) item.playing = value;
        } else {
            f.enabled = value;
            if (!value && state_.featureFocus == f.name) { queueFeatureFocus(false); state_.featureFocus.clear(); }
            if (!value && state_.featureCamera == f.name) state_.featureCamera.clear();
        }
        return true;
    }
    if (a[1] == "reset") {
        require(a.size() == 3, "feature reset <이름>. 모든 공유 라이브 포트가 함께 초기화됩니다.");
        const auto previous = f.program;
        const auto replacement = std::make_shared<FeatureContainer>(app_, previous->mode());
        for (auto& item : state_.features) if (item.program == previous) {
            item.program = replacement; item.events.clear(); item.revision = 0;
        }
        return true;
    }
    if (a[1] == "key") {
        require(a.size() == 5 && (a[4] == "press" || a[4] == "release"), "feature key <이름> <키/수정키+키> press/release");
        const auto key = keyCode(a[3], true);
        require(key.first != GLFW_KEY_Q && key.first != GLFW_KEY_ESCAPE, "Q/Esc 대신 feature enable을 사용하세요.");
        FeatureEvent event; event.kind = FeatureEvent::Kind::Key;
        event.key = key.first; event.mods = key.second; event.action = a[4] == "press" ? GLFW_PRESS : GLFW_RELEASE;
        queueFeatureEvent(f, event);
        return true;
    }
    require(false, "지원하는 feature 동작: add port list parts focus camera play enable reset key");
    return false;
}

void Assembly::advanceFeatures(float dt) {
    std::set<FeatureContainer*> advanced;
    for (auto& instance : state_.features) {
        if (!advanced.insert(instance.program.get()).second) continue;
        bool playing = false;
        for (auto& f : state_.features) if (f.program == instance.program) {
            playing = playing || (f.enabled && f.playing);
            for (const auto& event : f.events) {
                switch (event.kind) {
                case FeatureEvent::Kind::Pick: f.program->pick(event.hit); break;
                case FeatureEvent::Kind::Key: f.program->key(event.key, event.action, event.mods); break;
                case FeatureEvent::Kind::Mouse: f.program->mouse(event.mouse, event.view); break;
                case FeatureEvent::Kind::Cursor: f.program->cursor(event.x, event.y); break;
                case FeatureEvent::Kind::Focus: f.program->focus(event.focused); break;
                }
            }
            f.events.clear();
        }
        instance.program->step(!state_.paused && playing ? dt : 0);
    }
}

void Assembly::featureAppearance(graphics::EntityId id, std::optional<graphics::GlColor> color, std::optional<bool> visible) {
    for (auto& f : state_.features) {
        for (auto& part : f.entities) if (part.second.mesh == id) {
            if (color) part.second.tint = *color;
            if (visible) part.second.visible = *visible;
        }
        for (auto& line : f.lines) if (line.second.frame == id && visible) line.second.visible = *visible;
    }
}

void Assembly::syncFeatures() {
    auto& scene = app_.getScene();
    for (auto& feature : state_.features) {
        const auto& source = feature.program->scene();
        const auto revision = source.getGeometryRevision();
        const auto root = node(feature.name).id;
        std::set<graphics::EntityId> active;
        for (auto native : source.getIds()) {
            if (!contains(feature, native)) continue;
            active.insert(native);
            const auto* c = source.getComponent(native);
            auto it = feature.entities.find(native);
            if (it == feature.entities.end()) {
                const auto group = scene.addGroup(); require(group.has_value(), "기능 엔티티 포트 생성 실패.");
                FeatureEntity entity; entity.group = *group;
                registerNode(entityName(feature, native), *group, false, "feature-part");
                node(entityName(feature, native)).guideTopic = node(feature.name).guideTopic;
                it = feature.entities.emplace(native, entity).first;
            }
            auto& entity = it->second;
            if (c->polyhedron.faces.empty()) {
                if (entity.mesh) require(scene.setVisible(*entity.mesh, false), "기능의 이전 형상 숨기기 실패.");
                continue;
            }
            if (!entity.mesh) {
                const auto mesh = scene.push_entity(exportedModel(c->polyhedron)); require(mesh.has_value(), "기능 메시 포트 생성 실패.");
                require(scene.setParent(*mesh, entity.group), "기능 메시 부모 연결 실패.");
                entity.mesh = mesh; node(entityName(feature, native)).mesh = mesh;
            } else if (feature.revision != revision || scene.getComponent(*entity.mesh)->polyhedron.faces.size() != c->polyhedron.faces.size()) {
                auto geometry = exportedModel(c->polyhedron);
                const auto old = scene.getComponent(*entity.mesh)->polyhedron;
                for (std::size_t face = 0; face < std::min(old.faces.size(), geometry.faces.size()); ++face) {
                    geometry.faces[face].localAnimationMatrix = old.faces[face].localAnimationMatrix;
                    geometry.faces[face].animationSpace = old.faces[face].animationSpace;
                }
                require(scene.setModel(*entity.mesh, geometry), "기능 형상 갱신 실패.");
            }
            require(scene.setTint(*entity.mesh, {c->tint.red * entity.tint.red, c->tint.green * entity.tint.green, c->tint.blue * entity.tint.blue}) && scene.setVisible(*entity.mesh, feature.enabled && entity.visible && c->visible), "기능 표시 갱신 실패.");
            for (std::size_t face = 0; face < c->polyhedron.faces.size(); ++face) {
                const auto matrix = source.faceWorldMatrix(native, face);
                require(matrix && scene.setFaceFrame(*entity.mesh, face, *matrix), "기능 면 자세 갱신 실패.");
                const auto& original = c->polyhedron.faces[face];
                const auto cull = original.cull == graphics::CullMode::Inherit ? feature.program->options().cull : original.cull;
                require(scene.setFaceVisible(*entity.mesh, face, original.visible) && scene.setFaceCullMode(*entity.mesh, face, cull), "기능 면 표시 갱신 실패.");
            }
        }
        for (auto& item : feature.entities) {
            auto& entity = item.second;
            const auto* native = source.getComponent(item.first);
            if (!active.count(item.first) || !native) {
                if (entity.mesh) require(scene.setVisible(*entity.mesh, false), "이전 기능 메시 숨기기 실패.");
                continue;
            }
            if (!entity.parentKnown || entity.nativeParent != native->parent) {
                const auto found = native->parent ? feature.entities.find(*native->parent) : feature.entities.end();
                require(scene.setParent(entity.group, found == feature.entities.end() ? root : found->second.group), "기능 계층 연결 실패.");
                entity.nativeParent = native->parent; entity.parentKnown = true;
            }
        }
        const auto& lines = source.getLines();
        for (std::size_t i = 0; i < lines.size(); ++i) {
            const auto& line = lines[i];
            if (feature.branch && (!line.owner || !contains(feature, *line.owner))) continue;
            auto it = feature.lines.find(i);
            if (it == feature.lines.end()) {
                const auto frame = scene.addGroup(); require(frame.has_value(), "기능 선 프레임 생성 실패.");
                require(scene.setParent(*frame, root), "기능 선 부모 실패.");
                const auto id = scene.addLine(line.start, line.end, line.color, *frame); require(id.has_value(), "기능 선 생성 실패.");
                it = feature.lines.emplace(i, FeatureLine{*id, *frame, line.start, line.end}).first;
            }
            auto& output = it->second;
            const auto matrix = source.lineWorldMatrix(i); require(matrix.has_value(), "기능 선 행렬 실패.");
            auto transform = srtMatrix(*matrix);
            if (transform) {
                if (output.start != line.start || output.end != line.end) {
                    require(scene.setVector(output.line, line.start, line.end - line.start), "선 끝점 갱신 실패.");
                    output.start = line.start; output.end = line.end;
                }
            } else {
                transform = graphics::Transform3D{};
                const auto start = glm::vec3(*matrix * glm::vec4(line.start, 1)), end = glm::vec3(*matrix * glm::vec4(line.end, 1));
                if (output.start != start || output.end != end) {
                    require(scene.setVector(output.line, start, end - start), "아핀 선 갱신 실패.");
                    output.start = start; output.end = end;
                }
            }
            if (!feature.enabled || !output.visible) transform->scale = glm::vec3(0);
            require(scene.setTransform(output.frame, *transform), "기능 선 자세 실패.");
        }
        for (auto& item : feature.lines) if (item.first >= lines.size()) {
            auto hidden = graphics::Transform3D{}; hidden.scale = glm::vec3(0);
            require(scene.setTransform(item.second.frame, hidden), "이전 선 숨기기 실패.");
        }
        feature.revision = revision;
    }
    if (!state_.featureCamera.empty()) {
        const auto it = std::find_if(state_.features.begin(), state_.features.end(), [&](const auto& f) { return f.name == state_.featureCamera; });
        require(it != state_.features.end(), "카메라 소유 컨테이너가 없습니다.");
        const auto matrix = scene.worldMatrix(node(it->name).id); require(matrix.has_value(), "카메라 컨테이너 행렬 실패.");
        const auto scale = glm::length(glm::vec3((*matrix)[0]));
        require(scale > .0001f, "카메라 포트 배율이 너무 작습니다.");
        const auto frame = *matrix * glm::scale(glm::mat4(1), glm::vec3(1 / scale));
        require(rigid(frame), "카메라 포트의 부모 계층은 양수 균일 SRT여야 합니다.");
        auto camera = it->program->camera();
        camera.position = glm::vec3(*matrix * glm::vec4(camera.position, 1));
        camera.target = glm::vec3(*matrix * glm::vec4(camera.target, 1));
        camera.up = glm::normalize(glm::mat3(*matrix) * camera.up);
        camera.nearPlane *= scale; camera.farPlane *= scale; camera.orthographicHalfExtent *= scale;
        require(graphics::CameraSystem::validCamera(camera), "컨테이너 카메라가 유효하지 않습니다.");
        app_.getCamera() = camera; app_.getRenderOptions() = it->program->options();
    }
}
} // namespace assembly
````

**after** (없음)


## standard/modes/AssemblyGuide.cpp

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "GuideRecipes.h"
#include <algorithm>
#include <fstream>
````

**after** (없음)


### 블록 2 · before L7 / after L4

위치: `void Assembly::help(std::ostream& out) const {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
namespace {
std::filesystem::path projectRoot() {
    std::wstring binary(32768, L'\0');
    const DWORD count = GetModuleFileNameW(nullptr, binary.data(), DWORD(binary.size()));
    if (count > 0 && count < binary.size()) {
        binary.resize(count);
        auto path = std::filesystem::path(binary).parent_path();
        for (int i = 0; i < 6 && !path.empty(); ++i, path = path.parent_path())
            if (std::filesystem::is_regular_file(path / "modes/Assembly.h")) return path;
    }
    const auto source = std::filesystem::u8path(__FILE__).parent_path().parent_path();
    if (std::filesystem::is_regular_file(source / "modes/Assembly.h")) return source;
    return {};
````

**after**

````cpp
void Assembly::help(std::ostream& out) const {
    out << "Standard 0: 왼쪽 클릭: 물체, Ctrl+클릭: 부모 물체, Esc: 세부모드 종료. 도/초/월드 단위 사용.\n"
        << "add <종류> [이름] [at cursor 또는 at X Y Z] [생성 후 선택] - 종류: box/sphere/cone/pyramid/tetra/joint/room/humanoid, 이름 생략: 자동\n"
        << "t X Y Z [월드 원점 배치] - XYZ: 절대 좌표. 부모 상속 유지\n"
        << "r <축> <도> [월드 축 상대 회전] - 축: x/y/z, 도: 추가 회전량\n"
        << "s <값 또는 SX SY SZ> [월드 축 상대 배율] - S: 각 축 배율. 비균등/반사 지원, 0 제외\n"
        << "srt <이름> TX TY TZ RX RY RZ SX SY SZ [월드 기준 SRT 설정] - T: 위치, R: ZYX 회전 도, S: 축별 배율\n"
        << "pivot <center/cursor> [피벗 변경, 현재 배치 유지] - center: 형상 중심, cursor: 클릭 기준점\n"
        << "parent <부모/none> [선택 물체 상속 연결, 월드 배치 유지] - 부모: 물체 이름, none: 해제\n"
        << "layer add <이름> [지역 레이어 추가 후 선택] - 이름: 객체 내부 고유 이름\n"
        << "layer select <이름/none> [지역 편집 대상 지정] - none: 물체 배치 편집\n"
        << "layer t DX DY DZ [지역 이동 추가] - D: 선택 레이어 축의 이동 거리\n"
        << "layer r <축> <도> [지역 회전 추가] - 축: x/y/z, 도: 회전량\n"
        << "layer s <값 또는 SX SY SZ> [지역 배율 추가] - S: 축별 배율\n"
        << "layer before <이름> [선택 레이어 순서 변경] - 이름: 이 레이어 앞에 삽입\n"
        << "layer remove/reset/list [삭제/정적 변환 초기화/목록] - 인수 없음\n"
        << "layer enable/play <on/off> [기여/시계 제어] - off: 기여 제거/시간 정지\n"
        << "insert spin <축> <도/초> [자전 레이어 추가] - 축: 지역 축, 도/초: 각속도\n"
        << "insert orbit <축> <도/초> center <cursor 또는 X Y Z> [공전 레이어 추가] - 중심: 월드 점을 지역으로 변환\n"
        << "insert spin-axis AX AY AZ <도/초> [임의 지역 축 자전] - A: 0이 아닌 축 방향, 중심: 물체 피벗\n"
        << "insert orbit-axis AX AY AZ <도/초> center <cursor 또는 X Y Z> [임의 지역 축 공전] - A: 축 방향, 중심: 월드 점\n"
        << "insert linear DX DY DZ <초> [선형 이동 레이어] - D: 지역 변위, 초: 이동 시간\n"
        << "insert pendulum <축> <진폭 도> <주기 초> [진자 레이어] - 축: 지역 회전축\n"
        << "insert translate/rotate/scale <t/r/s 인수> [고정 변환 레이어] - 인수: 해당 지역 변환과 같음\n"
        << "light point [이름] [at cursor 또는 at X Y Z] [선택 가능한 점광원 생성] - 위치: 월드, 이름 생략: 자동\n"
        << "light color R G B [선택 광원 색] - RGB: 0~1\n"
        << "light intensity/attenuation <값> [선택 광원 강도/거리 감쇠] - 0 이상, intensity 0: 끄기\n"
        << "light sun <강도> [기존 방향광 강도] - 0: 끄기, 기본 1\n"
        << "camera add [이름] [at cursor 또는 at X Y Z] [카메라 객체 생성] - t/r/s/레이어로 이동\n"
        << "camera look <cursor 또는 X Y Z> [선택 카메라 시선 설정] - 목표: 월드 점\n"
        << "camera use [이름] [카메라로 전환] - 생략: 선택 카메라\n"
        << "camera free [일반 시점으로 복귀] - 마지막 시점은 유지\n"
        << "camera fov/extent <값> [투영 조정] - fov: 도, extent: 직교 반높이\n"
        << "camera clip <near> <far> [클리핑 조정] - 0 < near < far\n"
        << "cursor [X Y Z] [생성 기준점 조회/설정] - XYZ: 월드 좌표, 생략: 조회\n"
        << "plane <xy/xz/yz> <값> [빈 공간 클릭의 작업 평면] - 값: 평면의 고정 축 좌표\n"
        << "snap <간격> [생성 기준점 스냅] - 0: 끄기, 양수: 격자 간격\n"
        << "edit <object/part/face> [선택 세부모드] - 기본 object, part: 부품, face: 면\n"
        << "G/R/S [변환 미리보기] - X/Y/Z: 축, 마우스/방향키: 양, Shift+방향키: 미세, Enter: 확정, Esc: 취소\n"
        << "select <이름> [대상 지정] - 이름: 등록된 물체/부품\n"
        << "color <이름> R G B [색 변경] - RGB: 0~1\n"
        << "force add <ID> [물체] <기준/world> <질량> [힘 동작 연결] - 물체 생략: 선택, 기준: 강체 객체, 질량: kg\n"
        << "force gravity <ID> X Y Z [월드 중력 설정] - XYZ: m/s^2\n"
        << "force velocity/position <ID> X Y Z [지역 속도/위치] - XYZ: 기준 객체 좌표, m/s 또는 m\n"
        << "force spring <ID> <강성> <감쇠> AX AY AZ [스프링 저울 연결] - 강성: N/m, 감쇠: N*s/m, A: 지역 앵커\n"
        << "force read <ID> [하중 조회] - ID: 힘 동작. 스프링 힘과 정지 구속 반력을 구분\n"
        << "pause <on/off> [시간 정지/재개] - on: 정지, off: 재개. Space: 전환\n"
        << "bind <키> <press/hold> \"명령\" [입력 연결] - press: 1회, hold: 초당 move/rotate\n"
        << "save/load <파일> [레시피 저장/읽기] - 파일: 작업 폴더 기준 .assembly 경로\n"
        << "reload 또는 Ctrl+F5 [현재 레시피 재읽기] - C++ 재빌드와 별개\n"
        << "list/inspect/diagnose [이름] [상태 조회] - 이름 생략: 선택 물체\n"
        << "help 또는 F12 [도움말] - 인수 없음\n"
        << "clear [장면 비우기] - 인수 없음\n"
        << "quit [종료] - 인수 없음\n";
````

### 블록 3 · before L21 / after L59

위치: `void Assembly::guide(const std::string&, std::ostream& out) const { help(out); }`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp

void showSite(const GuideSite& site, std::ostream& out) {
    const auto root = projectRoot();
    const auto path = root.empty() ? std::filesystem::u8path(site.file) : root / site.file;
    std::ifstream input(path, std::ios::binary);
    if (!input) { out << path.u8string() << " : 소스가 없어 현재 줄 확인 불가. 표식=" << site.marker << '\n'; return; }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (lines.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
        lines.push_back(line);
    }
    out << "편집 위치: " << site.purpose << '\n';
    std::size_t matches = 0;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find(site.marker) == std::string::npos) continue;
        ++matches;
        if (matches > 4) continue;
        out << path.u8string() << ':' << i + 1 << '\n';
        const auto begin = i > 1 ? i - 2 : 0;
        const auto end = std::min(lines.size(), i + 4);
        for (auto n = begin; n < end; ++n) out << (n == i ? " > " : "   ") << n + 1 << " | " << lines[n] << '\n';
    }
    if (!matches) out << path.u8string() << " : 표식이 바뀌었습니다. 현재 줄을 추정하지 않습니다. 검색어=" << site.marker << '\n';
    if (matches > 1) out << "동일 표식 " << matches << "곳. 함수/부모 문맥을 비교하세요(최대4곳 표시).\n";
}
void showRecipe(const GuideRecipe& recipe, std::ostream& out) {
    out << "guide " << recipe.topic << ": " << recipe.goal << '\n';
    for (const auto& site : recipe.sites) showSite(site, out);
    out << "\n변경 예시 before (작성 기준값; 현재 코드는 위 출력 우선):\n" << recipe.before
        << "\nafter 예시:\n" << recipe.after << "\n변경 이유/단위/제약:\n" << recipe.reason
        << "\n디버깅-수정 순서:\n" << recipe.debug << "\n예상 관찰:\n" << recipe.expected << '\n';
    const auto root = projectRoot();
    out << "참고: " << (root.empty() ? std::filesystem::u8path(recipe.document) : root / recipe.document).u8string() << '\n'
        << "예시는 자동 패치가 아닙니다. 명령 예시는 콘솔/.assembly에서 실행합니다. C++ 예시는 표시한 함수/삽입 문맥에 맞춰 편집합니다.\nC++ 수정 후 빌드·실행 확인은 사용자가 수행합니다.\n";
}
}

void Assembly::help(std::ostream& out) const {
    out << "0번 조립 콘솔. 명세: docs/assembly/SPEC.md, 사용법: docs/assembly/USER_GUIDE.md\n"
        << "build tank/truck/person <이름> : C++ 예제 생성. 확장은 guide tank/truck/person으로 직접 편집\n"
        << "surface <객체> <면/all> | mount <객체> <관절> 면 삼각형 U V 거리 | anchor <객체> <관절> 면 XYZ\n"
        << "normal <객체> <면/all> XYZ 또는 auto/flip | winding <객체> <면/all> flip | warp <group> SX SY SZ\n"
        << "face-track <id> <객체> 면 시간 지연 once/loop/pingpong move/scale/turn ... | guide face-track\n"
        << "feature add <이름> 1~22 | feature parts <이름> | feature port <원본> <nativeID> <이름> | feature snapshot ...\n"
        << "feature focus/camera <이름/none> | feature play/enable <이름> on/off | Ctrl+F5 / hotload <파일> / reload\n"
        << "diagnose <객체> | guide <기능> : 현재 줄·변경 전후 예·이유·디버깅 단계\n"
        << "group <이름> X Y Z | part box <부모> <부품> 위치3 크기3 | joint <부모> <관절> 위치3\n"
        << "srt <그룹> 위치3 회전도3 균일배율 | pivot <그룹> center 또는 위치3 [keep/raw]\n"
        << "add <종류> <이름> X Y Z [크기3] | list | inspect <이름> | select <이름>\n"
        << "move/position <이름> X Y Z | rotate <이름> x/y/z 도 | scale <이름> 값 | parent <자식> <부모/none>\n"
        << "spin <id> <대상> 축 도/초 | path <id> <대상> 1~6 초 | pendulum <id> <대상> 축 진폭 초\n"
        << "linear <id> <대상> DX DY DZ 초 [once/loop/pingpong] | orbit <id> <대상> 축 도/초 중심3\n"
        << "face <id> <대상> 면 spin/orbit 축 도/초 또는 face <id> <대상> 면 hinge 모서리 도/초 | wall <id> <방> 면0~3 초\n"
        << "physics <id> <상자> <방> | walk <id> <walker> <방> 속도 | jump <보행id> 속도\n"
        << "bind RIGHT hold \"move hero 1 0 0\" | bind J press \"jump walking 5.5\" | unbind RIGHT\n"
        << "play/enable <id> on/off | seek <id> 초 | remove <id> | pause on/off | view 1/3\n"
        << "camera ... | render wire/lighting/depth on/off | save/load \"파일.assembly\" | clear\n"
        << "guide mount/normal/warp/face-track/cpp-effect/practice13~23 또는 guide <객체이름>, guide feature:<기능>\n"
        << "창: 클릭 선택, Tab/Backspace 부품, XYZ 축, 방향키 편집, Space 정지, Home 기본 레시피 복원\n";
}


void Assembly::guide(const std::string& requested, std::ostream& out) const {
    const bool feature = requested.compare(0, 8, "feature:") == 0;
    std::string topic = feature ? requested.substr(8) : requested;
    if (const auto found = state_.nodes.find(topic); !feature && found != state_.nodes.end()) {
        inspect(topic, out);
        std::optional<graphics::EntityId> id = found->second.id;
        topic = "objects";
        while (id) {
            bool matched = false;
            for (const auto& item : state_.nodes) if (item.second.id == *id && item.second.whole) {
                topic = !item.second.guideTopic.empty() ? item.second.guideTopic :
                    findGuideRecipe(item.second.kind) ? item.second.kind : "objects";
                matched = true; break;
            }
            if (matched) break;
            const auto* component = app_.getScene().getComponent(*id);
            id = component ? component->parent : std::nullopt;
        }
    }
    if (const auto* recipe = findGuideRecipe(topic)) { showRecipe(*recipe, out); return; }
    out << "지원 항목:";
    for (const auto& recipe : guideRecipes()) out << ' ' << recipe.topic;
    out << "\n객체 이름과 충돌하면 guide feature:<기능>을 사용하세요.\n";
}
````

**after**

````cpp
void Assembly::guide(const std::string&, std::ostream& out) const { help(out); }
````

## standard/modes/AssemblyHierarchy.cpp

### 블록 1 · before L81 / after L81

위치: `bool Assembly::hierarchyCommand(const Tokens& a, std::ostream&) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        require(a.size() == 9, "srt <대상> <위치3> <X/Y/Z 회전 도3> <균일배율>");
        srt(a[1], vector(a, 2), vector(a, 5), float(number(a[8])));
````

**after**

````cpp
        require(a.size() == 9 || a.size() == 11, "srt <대상> <월드 위치3> <회전 도3> <균일배율 또는 SX SY SZ>");
        graphics::Transform3D desired;
        desired.basis = vector(a,2);
        const auto angles=glm::radians(vector(a,5));
        desired.rotation=glm::mat3(glm::rotate(glm::mat4(1),angles.z,glm::vec3(0,0,1)) *
            glm::rotate(glm::mat4(1),angles.y,glm::vec3(0,1,0)) * glm::rotate(glm::mat4(1),angles.x,glm::vec3(1,0,0)));
        desired.scale=a.size()==9 ? glm::vec3(float(number(a[8]))) : vector(a,8);
        setWorld(a[1],graphics::DiagramSystem::modelMatrix(desired));
````

## standard/modes/AssemblyInput.cpp

### 블록 1 · before L170 / after L170

위치: `void Assembly::onKey(int key, int action, int mods, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (routeFeatureKey(key, action, mods, out)) return;
````

**after** (없음)


### 블록 2 · before L188 / after L187

위치: `void Assembly::onKey(int key, int action, int mods, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if ((key == GLFW_KEY_TAB || key == GLFW_KEY_BACKSPACE) && action == GLFW_PRESS && !state_.nodes.empty()) {
        auto it = state_.nodes.find(state_.selected);
        if (key == GLFW_KEY_TAB) {
            if (it == state_.nodes.end() || ++it == state_.nodes.end()) it = state_.nodes.begin();
        } else {
            if (it == state_.nodes.begin() || it == state_.nodes.end()) it = state_.nodes.end();
            --it;
        }
        state_.selected = it->first;
        inspect(it->first, out);
````

**after**

````cpp
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (editStart_) finishEdit(false,out);
        else { state_.editMode = "object"; state_.faceSelection.reset(); state_.selectedLayer.clear(); }
        return;
    }
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) { finishEdit(true,out); return; }
    if ((key == GLFW_KEY_G || key == GLFW_KEY_R || key == GLFW_KEY_S) && action == GLFW_PRESS && !state_.selected.empty()) {
        finishEdit(true,out);
        editStart_ = state_; editScene_=app_.getScene(); state_.paused = true;
        editOperation_ = key == GLFW_KEY_G ? 't' : key == GLFW_KEY_R ? 'r' : 's'; editCursor_.reset();
        out << "G/R/S [변환 미리보기] - X/Y/Z: 축, 마우스/방향키: 양, Shift: 미세, Enter: 확정, Esc: 취소\n";
````

### 블록 3 · before L200 / after L200

위치: `void Assembly::onKey(int key, int action, int mods, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    } else if (!state_.selected.empty()) {
        std::ostringstream command;
        const auto target = quote(state_.selected);
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT)
            command << "rotate " << target << ' ' << "xyz"[state_.axis] << ' ' << (key == GLFW_KEY_LEFT ? -5 : 5);
        else if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
            glm::vec3 delta(0); delta[state_.axis] = key == GLFW_KEY_UP ? .05f : -.05f;
            command << "move " << target << ' ' << delta.x << ' ' << delta.y << ' ' << delta.z;
        }
        if (!command.str().empty()) execute(command.str(), out);
````

**after**

````cpp
        out << "축 [" << "XYZ"[state_.axis] << "] - " << (state_.selectedLayer.empty() ? "월드" : "지역 레이어") << " 좌표계\n";
    } else if (editOperation_ && (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT || key == GLFW_KEY_UP || key == GLFW_KEY_DOWN)) {
        const float sign = key == GLFW_KEY_LEFT || key == GLFW_KEY_DOWN ? -1.0f : 1.0f;
        editStep(sign * ((mods & GLFW_MOD_SHIFT) ? .1f : 1.0f),out);
````

### 블록 4 · before L211 / after L205

위치: `void Assembly::onKey(int key, int action, int mods, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp

````

### 블록 5 · before L214 / after L209

위치: `void Assembly::onFocus(bool focused) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    queueFeatureFocus(focused);
````

**after** (없음)


### 블록 6 · before L216 / after L210

위치: `void Assembly::onFocus(bool focused) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
        finishEdit(false,std::cout);
````

### 블록 7 · before L223 / after L218

위치: `void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (routeFeatureMouse(event, out)) return;
````

**after** (없음)


### 블록 8 · before L234 / after L228

위치: `void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    if (editStart_) { finishEdit(true,out); return; }
````

### 블록 9 · before L235 / after L230

위치: `void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    const auto setPlaneCursor = [&](const graphics::Camera& camera, const graphics::PickViewport& viewport) {
        const auto point = graphics::PatternSystem::screenOnPlane(camera,event.framebufferX,event.framebufferY,
            viewport,state_.planePoint,state_.planeNormal);
        if (point) state_.cursor = *point;
    };
````

### 블록 10 · before L242 / after L242

위치: `void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
            {
                setPlaneCursor(pass.camera,{double(v.x),top,double(v.width),double(v.height)});
````

### 블록 11 · before L244 / after L246

위치: `void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
            }
````

### 블록 12 · before L245 / after L248

위치: `void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    } else hit = app_.pickAt(event.windowX, event.windowY);
````

**after**

````cpp
    } else {
        const auto size=app_.getFramebufferSize();
        setPlaneCursor(app_.getCamera(),{0,0,double(size[0]),double(size[1])});
        hit=app_.pickAt(event.windowX,event.windowY);
    }
    if (hit) state_.cursor = hit->worldPoint;
    if (state_.snapSpacing > 0) state_.cursor = graphics::PatternSystem::snap(state_.cursor,state_.snapSpacing);
    state_.selectedLayer.clear();
    state_.faceSelection = state_.editMode == "face" ? hit : std::nullopt;
````

### 블록 13 · before L249 / after L260

위치: `void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        for (const auto& item : state_.nodes)
            if (item.second.id == *entity && item.second.whole &&
                (state_.selected.empty() || item.second.kind == "group"))
                state_.selected = item.first;
````

**after**

````cpp
        for (const auto& item : state_.nodes) {
            const bool selectable = state_.editMode == "object" ? item.second.whole : true;
            if (item.second.id == *entity && selectable) { state_.selected = item.first; break; }
        }
        if (!state_.selected.empty()) break;
````

### 블록 14 · before L256 / after L268

위치: `void Assembly::onMouse(const graphics::MouseEvent& event, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    if ((event.mods & GLFW_MOD_CONTROL) && !state_.selected.empty() && state_.editMode=="object") {
        auto parent=app_.getScene().getComponent(node(state_.selected).id)->parent;
        bool found=false;
        while (parent && !found) {
            for (const auto& item:state_.nodes)
                if(item.second.id==*parent && item.second.whole) {state_.selected=item.first;found=true;break;}
            if(!found) parent=app_.getScene().getComponent(*parent)->parent;
        }
    }
    out << "cursor [생성 기준점] - 월드 XYZ=" << state_.cursor.x << ',' << state_.cursor.y << ',' << state_.cursor.z << '\n';
    if (state_.faceSelection) out << "face [세부 면 선택] - entity=" << state_.faceSelection->entityId << ", face=" << state_.faceSelection->faceIndex << '\n';
````

### 블록 15 · before L263 / after L286

위치: `void Assembly::onCursor(double x, double y, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    if (routeFeatureCursor(x, y)) return;
````

**after**

````cpp
    if (editStart_ && std::isfinite(x)) {
        if (editCursor_) editStep(float(x-*editCursor_)*.1f,out);
        editCursor_=x;
        return;
    }
````

## standard/modes/AssemblyMotion.cpp

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Paths.h"
````

**after**

````cpp
#include "AssemblyPaths.h"
````

### 블록 2 · before L37 / after L37

위치: `bool Assembly::motionCommand(const Tokens& a, std::ostream&) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
        if (it->tether) require(app_.getScene().setVector(*it->tether,{0,0,0},{0,0,0}),"힘 표시 선 제거 실패.");
````

### 블록 3 · before L110 / after L111

위치: `bool Assembly::motionCommand(const Tokens& a, std::ostream&) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        const Factory factories[]{standard_modes::path1, standard_modes::path2, standard_modes::path3,
            standard_modes::path4, standard_modes::path5, standard_modes::path6};
````

**after**

````cpp
        const Factory factories[]{assembly_paths::ellipse, assembly_paths::rectangle, assembly_paths::polygon,
            assembly_paths::spatial, assembly_paths::circle, assembly_paths::polyline};
````

## standard/modes/AssemblyParts.cpp

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeCommon.h"
````

**after**

````cpp
#include "Assembly.h"
````

### 블록 2 · before L5 / after L5

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
namespace {
graphics::EntityId requireEntity(std::optional<graphics::EntityId> id,const char* message) {
    require(id.has_value(),message); return *id;
}
}
````

### 블록 3 · before L8 / after L13

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        return practice_common::requireEntity(id, "Tank part creation failed.");
````

**after**

````cpp
        return requireEntity(id, "Tank part creation failed.");
````

### 블록 4 · before L38 / after L43

위치: `std::vector<graphics::EntityId> addGridParts(graphics::Scene& scene, const graphics::GridLayout& grid) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
            practice_common::require(center && size, "Invalid grid cell.");
            cells.push_back(practice_common::requireEntity(graphics::PatternSystem::addBox(scene, *size, *center,
````

**after**

````cpp
            require(center && size, "Invalid grid cell.");
            cells.push_back(requireEntity(graphics::PatternSystem::addBox(scene, *size, *center,
````

## standard/modes/AssemblyParts.h

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
````

**after**

````cpp
#include "../framework/Framework.h"
````

## standard/modes/AssemblyPaths.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
#include "AssemblyPaths.h"


graphics::PathMotion assembly_paths::ellipse() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
    return path;
}

graphics::PathMotion assembly_paths::rectangle() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
    return path;
}

graphics::PathMotion assembly_paths::polygon() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polygon(
        {{1, -.5f, 0}, {2.4f, -.4f, 0}, {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
    return path;
}

graphics::PathMotion assembly_paths::spatial() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = [center](double u) {
        const double angle = u * 6.283185307179586;
        return center + glm::vec3(float(.7 * std::sin(2 * angle)), float(.6 * std::sin(angle)),
                                  float(.3 * std::cos(angle)));
    };
    path.interpolation = graphics::PathSystem::smoothStep;
    return path;
}

graphics::PathMotion assembly_paths::circle() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    return path;
}

graphics::PathMotion assembly_paths::polyline() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
    path.playback = graphics::Playback::PingPong;
    return path;
}
````

## standard/modes/AssemblyPaths.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
#pragma once
#include "../framework/Framework.h"

namespace assembly_paths {
graphics::PathMotion ellipse();
graphics::PathMotion rectangle();
graphics::PathMotion polygon();
graphics::PathMotion spatial();
graphics::PathMotion circle();
graphics::PathMotion polyline();
}
````

## standard/modes/AssemblyPersistence.cpp

### 블록 1 · before L20 / after L20

위치: `std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
            stream << (item.second.affineGroup ? "rest3 " : "rest ") << quote(item.first) << ' ' << t.basis.x << ' ' << t.basis.y << ' ' << t.basis.z;
````

**after**

````cpp
            stream << "rest3 " << quote(item.first) << ' ' << t.basis.x << ' ' << t.basis.y << ' ' << t.basis.z;
````

### 블록 2 · before L25 / after L25

위치: `std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
            if (item.second.affineGroup) stream << ' ' << t.scale.y << ' ' << t.scale.z;
````

**after**

````cpp
            stream << ' ' << t.scale.y << ' ' << t.scale.z;
````

### 블록 3 · before L27 / after L27

위치: `std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
            stream << "attachment " << quote(item.first);
            for (int c=0;c<4;++c) for (int r=0;r<4;++r) stream << ' ' << item.second.attachment[c][r];
            stream << '\n';
````

### 블록 4 · before L28 / after L31

위치: `std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
        stream << "light sun " << app_.getRenderOptions().directionalIntensity << '\n';
        // Camera rig commands remain in the authored recipe. Free before serializing the final view.
        stream << "camera free\n";
````

### 블록 5 · before L42 / after L48

위치: `std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
        if (!state_.activeCamera.empty()) stream << "camera use " << quote(state_.activeCamera) << '\n';
````

## standard/modes/AssemblyRuntime.cpp

### 블록 1 · before L54 / after L54

위치: `glm::mat3 parentRotation(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    std::vector<graphics::JointAnimation> tracks;
````

**after** (없음)


### 블록 2 · before L57 / after L56

위치: `glm::mat3 parentRotation(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        if (driver && (driver->effect == Effect::Body || driver->effect == Effect::Walk || driver->effect == Effect::Custom))
````

**after**

````cpp
        if (driver && (driver->effect == Effect::Body || driver->effect == Effect::Walk || driver->effect == Effect::Custom || driver->effect == Effect::Dynamics))
````

### 블록 3 · before L59 / after L58

위치: `glm::mat3 parentRotation(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        if (item.second.affineGroup) {
            require(scene.setTransform(item.second.id, item.second.rest), "정적 warp 그룹 적용 실패.");
            continue;
````

**after**

````cpp
        const auto sample = motionFor(item.first).sample(0);
        require(sample.has_value(), "객체 모션 합성 실패.");
        require(scene.setTransform(item.second.id, item.second.rest) &&
            scene.setAttachment(item.second.id, item.second.attachment * sample->parent), "객체 기준 배치 실패.");
        std::vector<glm::mat4> layers{sample->local};
        for (const auto& layer : item.second.layers) {
            if (!layer.enabled) continue;
            const auto pose = layer.motion.sample(layer.seconds);
            require(pose.has_value(), "지역 레이어 모션 실패: " + layer.name);
            layers.push_back(layer.matrix * pose->parent * pose->local);
````

### 블록 4 · before L63 / after L69

위치: `glm::mat3 parentRotation(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        tracks.push_back({item.second.id, item.second.rest, motionFor(item.first)});
````

**after**

````cpp
        require(scene.setLocalLayers(item.second.id, layers), "지역 레이어 합성 실패.");
````

### 블록 5 · before L65 / after L71

위치: `glm::mat3 parentRotation(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    require(graphics::AnimationSystem::poseHierarchy(scene, tracks, 0), "기준 자세와 동작 합성 실패.");
````

**after** (없음)


### 블록 6 · before L68 / after L73

위치: `glm::mat3 parentRotation(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        const int order = b.effect == Effect::Body ? 1 : b.effect == Effect::Walk ? 2 : b.effect == Effect::Custom ? 3 : 0;
````

**after**

````cpp
        const int order = b.effect == Effect::Body ? 1 : b.effect == Effect::Walk ? 2 : (b.effect == Effect::Custom || b.effect == Effect::Dynamics) ? 3 : 0;
````

### 블록 7 · before L82 / after L87

위치: `glm::mat3 parentRotation(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
            require(graphics::AnimationSystem::poseHierarchy(scene, {{target.id, rest, motionFor(b.target)}}, 0), "컨베이어 자세 적용 실패.");
````

**after**

````cpp
            const auto pose=motionFor(b.target).sample(0);
            require(pose && scene.setTransform(target.id,rest) && scene.setAttachment(target.id,target.attachment*pose->parent),"컨베이어 자세 적용 실패.");
````

### 블록 8 · before L105 / after L111

위치: `glm::mat3 parentRotation(1);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
        } else if (b.effect == Effect::Dynamics) {
            const float stepTime=b.enabled && b.playing && !state_.paused ? dt : 0;
            advanceDynamics(b,stepTime);
````

### 블록 9 · before L121 / after L130

위치: `glm::vec3 half(0);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
            if (target.kind == "sphere") {
                float radius = 0;
                for (const auto& face : mesh->polyhedron.faces)
                    for (const auto& vertex : face.localVertices) radius = std::max(radius, glm::length(vertex));
                half = glm::vec3(radius * target.rest.scale.x);
            }
````

**after** (없음)


### 블록 10 · before L199 / after L202

위치: `glm::vec3 omega(0);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
                rest.scale = part.second.rest.scale;
                rest.localBasis = part.second.rest.localBasis;
                rest.rotation = glm::mat3(matrix) / rest.scale.x;
                rest.basis = glm::vec3(matrix[3]) + rest.rotation * (rest.scale * rest.localBasis);
                const std::vector<graphics::JointAnimation> edited{{part.second.id, rest, motionFor(part.first)}};
                require(graphics::AnimationSystem::poseHierarchy(scene, edited, 0), "보행과 부품 동작 합성 실패.");
````

**after**

````cpp
                const auto pose=motionFor(part.first).sample(0);
                require(pose && scene.setTransform(id,graphics::Transform3D{}) &&
                    scene.setAttachment(id,part.second.attachment*pose->parent*matrix),"보행 부품의 아핀 자세 합성 실패.");
````

### 블록 11 · before L209 / after L209

위치: `glm::vec3 omega(0);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    syncFeatures();
````

**after** (없음)


### 블록 12 · before L211 / after L210

위치: `glm::vec3 omega(0);`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
    syncSceneTools();
    glm::vec3 origin = state_.cursor;
    glm::mat3 axes(1);
    if (!state_.selected.empty()) {
        const auto m=scene.worldMatrix(node(state_.selected).id);
        if (m) {
            origin=glm::vec3((*m)[3]);
            if (!state_.selectedLayer.empty())
                for (int i=0;i<3;++i) axes[i]=glm::normalize(glm::vec3((*m)[i]));
        }
    }
    if (!state_.axes) state_.axes=scene.addBasis(origin,axes,glm::vec3(.8f));
    if (state_.axes)
        for (int i=0;i<3;++i) require(scene.setVector((*state_.axes)[i],origin,axes[i]*.8f),"조작 축 표시 실패");
````

### 블록 13 · before L228 / after L241

위치: `bool Assembly::update(float dt, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
            for (auto& item : state_.nodes)
                for (auto& layer : item.second.layers)
                    if (layer.playing && layer.enabled) layer.seconds += dt;
        if (!state_.paused)
````

### 블록 14 · before L231 / after L248

위치: `bool Assembly::update(float dt, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        advanceFeatures(dt);
````

**after** (없음)


## standard/modes/AssemblySceneTools.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
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
            const float value=float(number(a[2])); require(std::isfinite(value) && value>=0,"방향광 강도는 0 이상입니다.");
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
            const float value=float(number(a[2])); require(std::isfinite(value) && value>=0,"강도/감쇠는 0 이상입니다.");
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
````

## standard/modes/AssemblySurface.cpp

### 블록 1 · before L24 / after L24

위치: `void Assembly::requireFeatureEditable(graphics::EntityId mesh) const {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
    for (const auto& f : state_.features)
        for (const auto& part : f.entities)
            require(part.second.mesh != mesh, "라이브 기능의 형상/면 속성은 native 컨트롤러 소유입니다. feature snapshot으로 독립 복사 후 편집하세요.");
````

**after**

````cpp
    require(app_.getScene().getComponent(mesh) != nullptr, "메시가 없습니다.");
````

### 블록 2 · before L103 / after L101

위치: `void Assembly::applyMounts() {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        const auto pose = glm::translate(glm::mat4(1), anchor) * glm::mat4(rotation) *
            graphics::DiagramSystem::modelMatrix(scene.getComponent(object.id)->transform);
        auto result = object.rest;
        result.localBasis = {0, 0, 0};
        result.basis = glm::vec3(pose[3]);
        result.rotation = glm::mat3(pose) / result.scale.x;
        require(scene.setTransform(object.id, result), "면 관절 자세 합성 실패.");
````

**after**

````cpp
        const auto attached=glm::translate(glm::mat4(1),anchor)*glm::mat4(rotation)*scene.getComponent(object.id)->attachment;
        require(scene.setAttachment(object.id,attached),"면 관절의 아핀 부착 실패.");
````

## standard/modes/AssemblyWorkspace.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````cpp
#include "Assembly.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace assembly {
namespace {
std::string decimal(float value) {
    std::ostringstream out; out << std::setprecision(9) << value; return out.str();
}
Tokens pointTokens(glm::vec3 p) { return {decimal(p.x), decimal(p.y), decimal(p.z)}; }
bool affine(const glm::mat4& m) {
    return graphics::TransformSystem::validAffine(m) && std::abs(glm::determinant(glm::mat3(m))) > 1e-18f;
}
glm::mat4 inverseAffine(const glm::mat4& m) {
    require(affine(m),"역행렬이 없는 아핀 변환입니다.");
    const auto linear=glm::inverse(glm::mat3(m));
    glm::mat4 result(linear); result[3]=glm::vec4(-linear*glm::vec3(m[3]),1); return result;
}
glm::mat4 matrixTokens(const Tokens& a, std::size_t start) {
    require(a.size() == start + 16, "행렬은 열 우선 16개 값입니다.");
    glm::mat4 m(1);
    for (int c=0;c<4;++c) for (int r=0;r<4;++r) m[c][r] = float(number(a[start+c*4+r]));
    require(affine(m), "역행렬이 있는 유한 아핀 행렬이 필요합니다.");
    return m;
}
}

Tokens Assembly::resolveCommand(const Tokens& input) const {
    auto a = input;
    if (a.size()>=2 && ((a[0]=="light" && a[1]=="point") || (a[0]=="camera" && a[1]=="add"))) {
        const std::string kind=a[0]=="light" ? "light" : "camera";
        Tokens create{"add",kind}; create.insert(create.end(),a.begin()+2,a.end());
        auto resolved=resolveCommand(create); resolved[0]="tool-add"; return resolved;
    }
    if (a[0]=="force" && a.size()==5 && a[1]=="add") {
        require(!state_.selected.empty(),"물리 물체를 선택하세요."); a.insert(a.begin()+3,state_.selected);
    }
    if (a[0]=="camera" && a.size()==2 && a[1]=="use") a.push_back(state_.selected);
    if ((a[0]=="light" && a.size()>=2 && a[1]!="sun") ||
        (a[0]=="camera" && a.size()>=2 && a[1]=="look")) {
        require(!state_.selected.empty(),"먼저 광원/카메라를 선택하세요.");
        if (a.back()=="cursor") {a.pop_back();const auto p=pointTokens(state_.cursor);a.insert(a.end(),p.begin(),p.end());}
        Tokens scoped{"with",state_.selected,"none"}; scoped.insert(scoped.end(),a.begin(),a.end()); return scoped;
    }
    if (a[0] == "add" && a.size() >= 2 && (a.size() <= 3 || a[2] == "at" || (a.size() > 3 && a[3] == "at"))) {
        std::string name = a[1]; unsigned suffix = 1;
        while (state_.nodes.count(name + std::to_string(suffix))) ++suffix;
        name += std::to_string(suffix);
        std::size_t at = 2;
        if (a.size() > 2 && a[2] != "at") { name = a[2]; at = 3; }
        glm::vec3 point = state_.cursor;
        if (a.size() > at) {
            require(a[at] == "at", "생성 위치 앞에 at을 사용하세요.");
            if (a.size() == at+2) require(a[at+1] == "cursor", "at cursor 또는 at X Y Z");
            else { require(a.size() == at+4,"at cursor 또는 at X Y Z"); point = vector(a,at+1); }
        }
        a = {"add",a[1],name}; const auto p=pointTokens(point); a.insert(a.end(),p.begin(),p.end());
    }
    if (a[0] == "parent" && a.size() == 2) a.insert(a.begin()+1, state_.selected);
    if ((a[0] == "inspect" || a[0] == "diagnose" || a[0] == "center") && a.size() == 1)
        a.push_back(state_.selected);
    if (a[0] == "pivot" && a.size() >= 2 && a[1] == "cursor") {
        require(!state_.selected.empty(), "먼저 물체를 클릭하세요.");
        const auto m = app_.getScene().worldMatrix(node(state_.selected).id);
        require(m && affine(*m), "피벗 좌표 변환 실패.");
        const auto p = pointTokens(glm::vec3(inverseAffine(*m) * glm::vec4(state_.cursor,1)));
        a = {"pivot", state_.selected}; a.insert(a.end(),p.begin(),p.end()); a.push_back("keep");
    } else if (a[0] == "pivot" && a.size() == 2 && a[1] == "center") {
        a.insert(a.begin()+1,state_.selected);
    }
    if (a[0] == "insert" && a.back() == "cursor") {
        a.pop_back(); const auto p = pointTokens(state_.cursor); a.insert(a.end(),p.begin(),p.end());
    }
    if (state_.editMode=="face" && state_.faceSelection && (a[0]=="t" || a[0]=="r" || a[0]=="s")) {
        Tokens face{"face-edit",state_.selected,std::to_string(state_.faceSelection->faceIndex)};
        face.insert(face.end(),a.begin(),a.end()); return face;
    }
    if (a[0] == "t" || a[0] == "r" || a[0] == "s" || a[0] == "layer" || a[0] == "local" || a[0] == "insert") {
        require(!state_.selected.empty(), "먼저 물체를 클릭하세요. select <이름>도 가능합니다.");
        Tokens scoped{"with",state_.selected,state_.selectedLayer.empty() ? "none" : state_.selectedLayer};
        scoped.insert(scoped.end(),a.begin(),a.end()); return scoped;
    }
    return a;
}

void Assembly::setWorld(const std::string& target, const glm::mat4& matrix) {
    auto& object = node(target);
    const auto* driver = positionDriver(target);
    require(!object.mount && (!driver || (driver->effect != Effect::Body && driver->effect != Effect::Walk && driver->effect != Effect::Custom && driver->effect != Effect::Dynamics)),
            "물리/사용자 제어 루트 또는 면 관절의 배치는 제어 함수를 통해 변경하세요.");
    auto& scene = app_.getScene();
    const auto current = scene.getComponent(object.id)->attachment;
    const auto animation = inverseAffine(object.attachment) * current;
    require(affine(animation) && scene.setWorldPlacement(object.id,matrix), "월드 배치 실패: 부모/배율을 확인하세요.");
    object.attachment = scene.getComponent(object.id)->attachment * inverseAffine(animation);
}

bool Assembly::workspaceCommand(const Tokens& a, std::ostream& out) {
    const auto& command = a[0];
    if (command == "with") {
        require(a.size() >= 4, "with <객체> <레이어/none> <명령...>");
        node(a[1]); state_.selected = a[1]; state_.selectedLayer = a[2] == "none" ? "" : a[2];
        return dispatch(Tokens(a.begin()+3,a.end()),out);
    }
    if (command=="face-edit") {
        require(a.size()>=5,"face-edit <메시> <면> t/r/s ...");
        auto& scene=app_.getScene(); const auto mesh=meshEntity(a[1]); const auto faceIndex=indexNumber(a[2]);
        requireFaceEditable(mesh,faceIndex);
        const auto* face=scene.getMesh(mesh,faceIndex); require(face!=nullptr,"면이 없습니다.");
        const auto world=scene.faceWorldMatrix(mesh,faceIndex); require(world.has_value(),"면의 월드 변환이 없습니다.");
        const auto parent=face->transformMode==graphics::FaceTransformMode::Hierarchy ? scene.worldMatrix(mesh) : std::optional<glm::mat4>{glm::mat4(1)};
        require(parent.has_value(),"메시 프레임이 없습니다.");
        const auto frame=*parent*face->transformFrame;
        glm::vec3 localCenter(0); for (const auto& p:face->localVertices) localCenter+=p;
        require(!face->localVertices.empty(),"면 정점이 없습니다."); localCenter/=float(face->localVertices.size());
        const auto center=glm::vec3(*world*glm::vec4(localCenter,1));
        std::optional<glm::mat4> change;
        if (a[3]=="t") { require(a.size()==7,"면 t X Y Z: 면 중심의 월드 위치"); change=graphics::TransformSystem::translation(vector(a,4)-center); }
        else if (a[3]=="r") { require(a.size()==6,"면 r 축 도"); change=graphics::TransformSystem::rotation({center,axis(a[4])},number(a[5])); }
        else { require(a[3]=="s" && (a.size()==5 || a.size()==7),"면 s 배율 또는 SX SY SZ"); change=graphics::TransformSystem::scaling(a.size()==5 ? glm::vec3(float(number(a[4]))) : vector(a,4),center); }
        const auto srt=graphics::detail::faceSrtMatrix(*face);
        require(change && affine(*change) && affine(frame) && affine(srt),"면 변환이 특이합니다.");
        require(scene.setFaceLocalMatrix(mesh,faceIndex,inverseAffine(frame)* *change* *world*inverseAffine(srt),graphics::FaceAnimationSpace::Model),"면 변환 실패.");
        return true;
    }
    if (command == "attachment") {
        require(a.size() == 18,"attachment <객체> <열 우선 행렬16>");
        node(a[1]).attachment = matrixTokens(a,2); return true;
    }
    if (command == "edit") {
        require(a.size() == 2 && (a[1] == "object" || a[1] == "part" || a[1] == "face"),"edit object/part/face");
        state_.editMode = a[1]; state_.faceSelection.reset(); return true;
    }
    if (command == "cursor") {
        require(a.size() == 1 || a.size() == 4,"cursor [X Y Z]");
        if (a.size() == 4) state_.cursor = vector(a,1);
        out << "cursor [생성 기준점] - 월드 XYZ=" << state_.cursor.x << ',' << state_.cursor.y << ',' << state_.cursor.z << '\n';
        return true;
    }
    if (command == "plane") {
        require(a.size() == 3,"plane <xy/xz/yz> <해당 평면의 고정 좌표>");
        require(a[1] == "xy" || a[1] == "xz" || a[1] == "yz","평면은 xy/xz/yz입니다.");
        state_.planeNormal = a[1] == "xy" ? glm::vec3(0,0,1) : a[1] == "xz" ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
        state_.planePoint = state_.planeNormal * float(number(a[2])); return true;
    }
    if (command == "snap") {
        require(a.size() == 2,"snap <간격. 0은 끄기>");
        const float value = float(number(a[1])); require(value >= 0 && value <= 1000,"간격은 0~1000입니다.");
        state_.snapSpacing = value; return true;
    }
    if (command == "t" || command == "r" || command == "s") {
        auto& scene = app_.getScene();
        const auto placement = scene.placementMatrix(node(state_.selected).id);
        require(placement.has_value(),"객체 배치가 없습니다.");
        auto matrix = *placement;
        const auto final = scene.worldMatrix(node(state_.selected).id);
        require(final.has_value(),"피벗 월드 변환 실패.");
        const auto origin = glm::vec3(*final * glm::vec4(node(state_.selected).rest.localBasis,1));
        if (command == "t") {
            require(a.size() == 4,"t X Y Z [월드 원점 배치] - XYZ: 절대 좌표");
            matrix[3] = glm::vec4(vector(a,1),1);
        } else {
            std::optional<glm::mat4> change;
            if (command == "r") {
                require(a.size() == 3,"r <축> <도> [월드 축 상대 회전]");
                change = graphics::TransformSystem::rotation({origin,axis(a[1])},number(a[2]));
            } else {
                require(a.size() == 2 || a.size() == 4,"s <균일값 또는 SX SY SZ> [월드 축 상대 배율]");
                const auto factors = a.size() == 2 ? glm::vec3(float(number(a[1]))) : vector(a,1);
                require(glm::all(glm::greaterThan(glm::abs(factors),glm::vec3(.0001f))),"배율은 0이 아니어야 합니다.");
                change = graphics::TransformSystem::scaling(factors,origin);
            }
            require(change.has_value(),"변환 행렬 생성 실패."); matrix = *change * matrix;
        }
        setWorld(state_.selected,matrix); return true;
    }
    if (command == "local") {
        require(a.size() >= 2,"local t/r/s ...");
        Tokens local{"layer"}; local.insert(local.end(),a.begin()+1,a.end()); return workspaceCommand(local,out);
    }
    if (command == "layer" || command == "insert") {
        auto& object = node(state_.selected);
        auto& layers = object.layers;
        const auto* driver = positionDriver(state_.selected);
        require(!driver || (driver->effect != Effect::Body && driver->effect != Effect::Walk && driver->effect != Effect::Custom && driver->effect != Effect::Dynamics),
                "적분 물리/사용자 제어 루트의 레이어는 제어 함수와 별도의 자식 객체에 작성하세요.");
        if (command == "insert") {
            require(a.size() >= 2,"insert spin/orbit/linear/pendulum/translate/rotate/scale ...");
            std::string name = "layer" + std::to_string(layers.size()+1);
            while (std::any_of(layers.begin(),layers.end(),[&](const LocalLayer& l){return l.name == name;})) name += "_";
            LocalLayer layer; layer.name = name;
            if (a[1] == "spin" || a[1] == "orbit" || a[1] == "spin-axis" || a[1] == "orbit-axis") {
                const bool vectorAxis=a[1]=="spin-axis" || a[1]=="orbit-axis";
                const bool orbit=a[1]=="orbit" || a[1]=="orbit-axis";
                const std::size_t rateIndex=vectorAxis ? 5 : 3;
                require(orbit ? (a.size()==rateIndex+5 && a[rateIndex+1]=="center") : a.size()==rateIndex+1,
                    "insert spin[-axis] <축 또는 AX AY AZ> <도/초> 또는 orbit[-axis] ... center <월드 XYZ/cursor>");
                graphics::RotationMotion rotation;
                rotation.axis.direction=vectorAxis ? vector(a,2) : axis(a[2]);
                rotation.axis.point=object.rest.localBasis;
                rotation.degreesPerSecond=number(a[rateIndex]); rotation.space=graphics::TransformSpace::Local;
                if (orbit) {
                    const auto frame=app_.getScene().worldMatrix(object.id);
                    require(frame && affine(*frame),"공전 중심 변환 실패.");
                    rotation.axis.point=glm::vec3(inverseAffine(*frame)*glm::vec4(vector(a,rateIndex+2),1));
                }
                require(layer.motion.append(graphics::Motion::rotation(rotation)),"회전 템플릿 생성 실패.");
            } else if (a[1] == "linear") {
                require(a.size() == 6,"insert linear DX DY DZ 초");
                graphics::LinearMotion motion; motion.to = vector(a,2); motion.durationSeconds = number(a[5]);
                motion.space = graphics::TransformSpace::Local;
                require(layer.motion.append(graphics::Motion::linear(motion)),"선형 이동 템플릿 생성 실패.");
            } else if (a[1] == "pendulum") {
                require(a.size() == 5,"insert pendulum <축> <진폭 도> <주기 초>");
                graphics::PendulumMotion motion; motion.axis.direction = axis(a[2]); motion.axis.point=object.rest.localBasis;
                motion.amplitudeDegrees = number(a[3]); motion.periodSeconds = number(a[4]);
                require(layer.motion.append(graphics::Motion::pendulum(motion)),"진자 템플릿 생성 실패.");
            } else {
                require(a[1] == "translate" || a[1] == "rotate" || a[1] == "scale","지원하지 않는 변환 템플릿입니다.");
                layers.push_back(layer); state_.selectedLayer = name;
                Tokens edit{"layer",a[1] == "translate" ? "t" : a[1] == "rotate" ? "r" : "s"};
                edit.insert(edit.end(),a.begin()+2,a.end()); return workspaceCommand(edit,out);
            }
            require(layers.size() < 127,"지역 레이어는 127개까지입니다.");
            layers.push_back(std::move(layer)); state_.selectedLayer = name; return true;
        }
        require(a.size() >= 2,"layer add/select/remove/before/t/r/s/reset/list ...");
        if (a[1] == "list") {
            for (const auto& l : layers) out << l.name << " [지역 레이어] - 앞 레이어 결과를 상속, 초=" << l.seconds << '\n';
            return true;
        }
        if (a[1] == "add") {
            require(a.size() == 3 && a[2] != "none" && layers.size() < 127,"layer add <이름>. none은 예약어입니다.");
            for (const auto& l:layers) require(l.name != a[2],"중복 레이어 이름입니다.");
            LocalLayer layer; layer.name = a[2]; layers.push_back(layer); state_.selectedLayer = a[2]; return true;
        }
        if (a[1] == "select") {
            require(a.size() == 3,"layer select <이름/none>");
            if (a[2] == "none") { state_.selectedLayer.clear(); return true; }
            require(std::any_of(layers.begin(),layers.end(),[&](const LocalLayer& l){return l.name == a[2];}),"없는 레이어입니다.");
            state_.selectedLayer = a[2]; return true;
        }
        auto it = std::find_if(layers.begin(),layers.end(),[&](const LocalLayer& l){return l.name == state_.selectedLayer;});
        require(it != layers.end(),"layer add/select로 지역 레이어를 지정하세요.");
        if (a[1] == "remove") {
            require(a.size() == 2,"layer remove"); layers.erase(it); state_.selectedLayer.clear(); return true;
        }
        if (a[1] == "before") {
            require(a.size() == 3 && a[2] != it->name,"layer before <다른 레이어 이름>");
            const auto target = std::find_if(layers.begin(),layers.end(),[&](const LocalLayer& l){return l.name == a[2];});
            require(target != layers.end(),"대상 레이어가 없습니다.");
            const auto old = std::size_t(it-layers.begin()), to = std::size_t(target-layers.begin());
            auto moved = *it; layers.erase(layers.begin()+old);
            layers.insert(layers.begin()+(to > old ? to-1 : to),std::move(moved)); return true;
        }
        if (a[1] == "reset") { require(a.size()==2,"layer reset"); it->matrix=glm::mat4(1); return true; }
        if (a[1] == "enable" || a[1] == "play") {
            require(a.size()==3,"layer enable/play on/off");
            (a[1] == "enable" ? it->enabled : it->playing) = boolean(a[2]); return true;
        }
        std::optional<glm::mat4> change;
        if (a[1] == "t") { require(a.size()==5,"layer t DX DY DZ"); change=graphics::TransformSystem::translation(vector(a,2)); }
        else if (a[1] == "r") { require(a.size()==4,"layer r <지역 축> <도>"); change=graphics::TransformSystem::rotation({glm::vec3(0),axis(a[2])},number(a[3])); }
        else if (a[1] == "s") {
            require(a.size()==3 || a.size()==5,"layer s <균일값 또는 SX SY SZ>");
            change=graphics::TransformSystem::scaling(a.size()==3 ? glm::vec3(float(number(a[2]))) : vector(a,2));
        } else require(false,"지원하지 않는 레이어 명령입니다.");
        require(change && affine(it->matrix * *change),"레이어 변환은 역행렬이 있는 아핀 행렬이어야 합니다.");
        it->matrix *= *change; return true;
    }
    return false;
}

void Assembly::editStep(float amount, std::ostream& out) {
    if (!editOperation_ || state_.selected.empty()) return;
    const std::string axisName(1,"xyz"[state_.axis]);
    std::ostringstream command;
    const bool local = !state_.selectedLayer.empty();
    if (editOperation_ == 'r') command << (local ? "layer r " : "r ") << axisName << ' ' << amount*5;
    else if (editOperation_ == 's') {
        glm::vec3 scale(1); scale[state_.axis] = std::pow(1.05f,amount);
        command << (local ? "layer s " : "s ") << scale.x << ' ' << scale.y << ' ' << scale.z;
    } else {
        glm::vec3 p(0); p[state_.axis]=amount*.1f;
        if (!local) {
            const auto base=app_.getScene().placementMatrix(node(state_.selected).id);
            if (!base) return;
            if (state_.editMode=="face" && state_.faceSelection) {
                const auto& hit=*state_.faceSelection;
                const auto face=app_.getScene().getMesh(hit.entityId,hit.faceIndex);
                const auto matrix=app_.getScene().faceWorldMatrix(hit.entityId,hit.faceIndex);
                if (!face || !matrix || face->localVertices.empty()) return;
                glm::vec3 center(0);for(const auto& v:face->localVertices)center+=v;
                p+=glm::vec3(*matrix*glm::vec4(center/float(face->localVertices.size()),1));
            } else p += glm::vec3((*base)[3]);
        }
        command << (local ? "layer t " : "t ") << p.x << ' ' << p.y << ' ' << p.z;
    }
    execute(command.str(),out);
}
void Assembly::finishEdit(bool keep, std::ostream& out) {
    if (!editStart_) return;
    if (!keep) { state_ = *editStart_; if(editScene_) app_.getScene()=*editScene_; applyPoses(0); }
    else state_.paused = editStart_->paused;
    editStart_.reset(); editScene_.reset(); editOperation_=0; editCursor_.reset();
    out << (keep ? "Enter [변환 확정]" : "Esc [변환 취소]") << " - 인수 없음\n";
}
} // namespace assembly
````

## standard/modes/Editor.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "../Console.h"
#include <iostream>

bool Console::addControl(graphics::EntityId id, const std::string& name, ControlKind kind) {
    std::size_t index = 0;
    while (index < animation_.size() && animation_[index].joint != id)
        ++index;
    if (index == animation_.size()) {
        const auto track = graphics::AnimationSystem::createJointAnimation(app_.getScene(), id, graphics::Motion{});
        if (!track)
            return false;
        animation_.push_back(*track);
    }
    jointControls_.push_back({index, name, animation_[index].rest, kind});
    return true;
}

void Console::selectPicked(std::optional<graphics::PickResult> hit) {
    selectedJoint_ = jointControls_.size();
    if (hit) {
        std::optional<graphics::EntityId> entity = hit->entityId;
        while (entity) {
            for (std::size_t i = 0; i < jointControls_.size(); ++i) {
                const auto& control = jointControls_[i];
                if (animation_[control.track].joint == *entity && control.kind != ControlKind::Joint) {
                    selectedJoint_ = i;
                    printSelectedJoint();
                    return;
                }
            }
            const auto* component = app_.getScene().getComponent(*entity);
            entity = component ? component->parent : std::nullopt;
        }
    }
    std::cout << "선택 해제: 조작할 객체를 클릭하거나 Tab을 누르세요.\n";
}

bool Console::handleJointKey(int key) {
    if (jointControls_.empty())
        return false;
    if (key == GLFW_KEY_TAB) {
        selectedJoint_ = selectedJoint_ >= jointControls_.size() ? 0 : (selectedJoint_ + 1) % jointControls_.size();
    } else if (key == GLFW_KEY_BACKSPACE) {
        selectedJoint_ = selectedJoint_ >= jointControls_.size() ? jointControls_.size() - 1
            : (selectedJoint_ + jointControls_.size() - 1) % jointControls_.size();
    } else if (key == GLFW_KEY_X || key == GLFW_KEY_Y || key == GLFW_KEY_Z) {
        selectedAxis_ = key == GLFW_KEY_X ? 0 : key == GLFW_KEY_Y ? 1 : 2;
    } else {
        const bool edit = key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT || key == GLFW_KEY_UP ||
            key == GLFW_KEY_DOWN || key == GLFW_KEY_PAGE_UP || key == GLFW_KEY_PAGE_DOWN ||
            key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD || key == GLFW_KEY_MINUS ||
            key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_R;
        if (!edit)
            return false;
        if (selectedJoint_ >= jointControls_.size()) {
            std::cout << "편집 대상이 없습니다.\n";
            return true;
        }
        const auto& control = jointControls_[selectedJoint_];
        auto& rest = animation_[control.track].rest;
        const auto saved = rest;
        const auto savedBody = roomBody_;
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
            glm::vec3 axis(0);
            axis[selectedAxis_] = 1;
            rest.rotation *= glm::mat3(glm::rotate(glm::mat4(1),
                glm::radians(key == GLFW_KEY_LEFT ? -5.0f : 5.0f), axis));
            // Repeated edits must retain an orthonormal rotation basis.
            const auto x = glm::normalize(rest.rotation[0]);
            const auto z = glm::normalize(glm::cross(x, rest.rotation[1]));
            rest.rotation = glm::mat3(x, glm::cross(z, x), z);
        } else if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
            rest.basis[selectedAxis_] += key == GLFW_KEY_UP ? .05f : -.05f;
        } else if (key == GLFW_KEY_PAGE_UP || key == GLFW_KEY_PAGE_DOWN) {
            if (control.kind == ControlKind::RoomBody) {
                std::cout << "충돌 상자는 중심 피벗을 유지합니다.\n";
                return true;
            }
            rest.localBasis[selectedAxis_] += key == GLFW_KEY_PAGE_UP ? .05f : -.05f;
        } else if (key == GLFW_KEY_R) {
            rest = control.initialRest;
        } else {
            if (control.kind == ControlKind::RoomFrame) {
                std::cout << "물리 방은 강체이므로 배율 1을 유지합니다.\n";
                return true;
            }
            const bool grow = key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD;
            rest.scale = glm::vec3(std::clamp(rest.scale.x * (grow ? 1.1f : 1 / 1.1f), .1f, 3.0f));
        }
        if (control.kind == ControlKind::RoomBody) {
            if (key == GLFW_KEY_R) {
                roomBody_ = graphics::RoomBody{};
                roomBody_.localPosition = {0, -.75f, 0};
            } else if (roomBody_.released) {
                roomBody_.worldPosition += releasedRotation_ * (rest.basis - saved.basis);
            } else {
                roomBody_.localPosition += rest.basis - saved.basis;
            }
            rest.basis = glm::vec3(0);
        }
        if (!applyControls()) {
            rest = saved;
            roomBody_ = savedBody;
            if (!applyControls())
                app_.requestClose();
            std::cerr << "편집을 거절하고 이전 상태를 복원했습니다.\n";
        }
    }
    printSelectedJoint();
    return true;
}

void Console::printSelectedJoint() const {
    if (selectedJoint_ >= jointControls_.size())
        return;
    const auto& control = jointControls_[selectedJoint_];
    const auto& rest = animation_[control.track].rest;
    std::cout << "mode=" << activeMode_ << " selected=" << selectedJoint_ + 1 << '/' << jointControls_.size()
        << ": " << control.name << " axis=" << "XYZ"[selectedAxis_]
        << " position=" << rest.basis.x << ',' << rest.basis.y << ',' << rest.basis.z
        << " pivot=" << rest.localBasis.x << ',' << rest.localBasis.y << ',' << rest.localBasis.z
        << " scale=" << rest.scale.x << '\n';
}
````

**after** (없음)


## standard/modes/FaceCommon.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "FaceCommon.h"

using namespace face_common;
using practice_common::assignVertexColors;
using practice_common::rotateAround;
using practice_common::scaleAround;


/// 장면 객체와 초기 상태를 구성한다.

bool FaceMode::initScene() {
    auto& scene = application_.getScene();

    scene.clear();

    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(HALF_SIZE * 2.0f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(HALF_SIZE * 2.0f));

    assignVertexColors(cube);
    assignVertexColors(pyramid);

    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);

    if (!cubeId || !pyramidId) {
        return false;
    }

    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;

    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();
    const auto inverseView = glm::transpose(glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0})));

    camera.position = inverseView * glm::vec3(0, 0, 4);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetAll();
    std::cout
        << "Practice 15 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
        << "Cube only 모드: T == top Z rotation, F == front open/close, S == left/right face normal rotation\n"
        << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
        << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
        << "All face motions can overlap with object spin(중복 모션 가능).\n"
        << "T/S/1/2/4/5 repeat to pause/resume(한번 더 누르면 정지). F/B repeat to reverse. 2 swaps and returns.\n";

    return true;
}

void FaceMode::resetFaces() {
    auto& scene = application_.getScene();

    for (auto id : scene.getIds()) {
        const bool visible = id == (pyramidSelected_ ? pyramidId_ : cuboidId_);

        for (std::size_t i = 0; i < scene.getComponent(id)->polyhedron.faces.size(); ++i) {
            scene.setFaceLocalMatrix(id, i, glm::mat4(1.0f));
            scene.setFaceVisible(id, i, visible);
        }
    }
}

void FaceMode::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;
    motions_ = {};
    resetFaces();
    applyObjectTransform();
}

void FaceMode::resetAll() {
    spinning_ = false;
    spinDegrees_ = 0.0f;
    application_.getRenderOptions() = graphics::RenderOptions{};
    selectShape(false);
}

void FaceMode::applyObjectTransform() {
    graphics::Transform3D transform;

    transform.rotation = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(spinDegrees_), Y_AXIS));
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void FaceMode::beginMotion(Motion kind) {
    if (pyramidSelected_) {
        return;
    }

    auto& motion = motions_[static_cast<std::size_t>(kind)];

    if (motion.initialized) {
        if (kind == Motion::Front || kind == Motion::Back) {
            motion.direction = -motion.direction;
            motion.running = true;
        } else {
            motion.running = !motion.running;
        }
    } else {
        motion.initialized = true;
        motion.running = true;

        if (kind == Motion::Hinge || kind == Motion::Swap) {
            const std::array<std::array<std::size_t, 2>, 4> pairs{
                std::array<std::size_t, 2>{0, 3}, {3, 1}, {1, 2}, {2, 0}};
            const std::array<glm::vec3, 4> corners{glm::vec3{HALF_SIZE, 0.0f, HALF_SIZE},
                                                   {HALF_SIZE, 0.0f, -HALF_SIZE},
                                                   {-HALF_SIZE, 0.0f, -HALF_SIZE},
                                                   {-HALF_SIZE, 0.0f, HALF_SIZE}};
            const int index = std::uniform_int_distribution<int>(0, 3)(random_);

            motion.pair = pairs[index];
            motion.hinge = corners[index];
            std::cout << "선택면들 -Selected cube faces (1-based 면): " << motion.pair[0] + 1 << ", "
                      << motion.pair[1] + 1 << '\n';
        }
    }
    applyFaceMotion();
}

void FaceMode::applyFaceMotion() {
    auto& scene = application_.getScene();
    std::array<glm::mat4, 6> matrices;

    matrices.fill(glm::mat4(1.0f));

    const auto compose = [&](std::size_t face, const glm::mat4& matrix) {
        matrices[face] = matrix * matrices[face];
    };

    for (std::size_t index = 1; index < motions_.size(); ++index) {
        const auto kind = static_cast<Motion>(index);
        const auto& motion = motions_[index];

        if (!motion.initialized) {
            continue;
        }

        const float progress = motion.progress;

        switch (kind) {
        case Motion::Top:
            compose(4, rotateAround({0.0f, HALF_SIZE, 0.0f}, Z_AXIS, progress));
            break;
        case Motion::Front:
            compose(0, rotateAround({0.0f, HALF_SIZE, HALF_SIZE}, X_AXIS, -90.0f * progress));
            break;
        case Motion::Side:

            compose(2, rotateAround({-HALF_SIZE, 0.0f, 0.0f}, X_AXIS, progress));
            compose(3, rotateAround({HALF_SIZE, 0.0f, 0.0f}, X_AXIS, progress));
            break;
        case Motion::Back:
            compose(1, scaleAround({0.0f, 0.0f, -HALF_SIZE}, 1.0f - progress));
            break;
        case Motion::Hinge:
            for (auto face : motion.pair) {
                compose(face, rotateAround(motion.hinge, Y_AXIS, progress));
            }
            break;
        case Motion::Swap: {

            const float angle = progress <= 90.0f ? progress : 180.0f - progress;

            compose(motion.pair[0], rotateAround(glm::vec3(0.0f), Y_AXIS, angle));
            compose(motion.pair[1], rotateAround(glm::vec3(0.0f), Y_AXIS, -angle));
            break;
        }
        case Motion::OrbitY:
            for (std::size_t face : {0u, 1u, 2u, 3u}) {
                compose(face, rotateAround(glm::vec3(0.0f), Y_AXIS, progress));
            }
            break;
        case Motion::OrbitX:
            for (std::size_t face : {4u, 5u, 0u, 1u}) {
                compose(face, rotateAround(glm::vec3(0.0f), X_AXIS, progress));
            }
            break;
        default:
            break;
        }
    }

    const auto& back = motions_[static_cast<std::size_t>(Motion::Back)];

    for (std::size_t face = 0; face < matrices.size(); ++face) {
        scene.setFaceLocalMatrix(cuboidId_, face, matrices[face]);
        scene.setFaceVisible(cuboidId_, face,
                             !pyramidSelected_ && !(face == 1 && back.initialized && back.progress == 1.0f));
    }
}

void FaceMode::onKey(int key, int action, int) {
    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        application_.requestClose();
        break;
    case GLFW_KEY_H: {
        auto& options = application_.getRenderOptions();

        options.depthTest = !options.depthTest;
        break;
    }
    case GLFW_KEY_P:
        selectShape(!pyramidSelected_);
        break;
    case GLFW_KEY_Y:
        spinning_ = !spinning_;
        break;
    case GLFW_KEY_R:
        resetAll();
        break;
    case GLFW_KEY_T:
        beginMotion(Motion::Top);
        break;
    case GLFW_KEY_F:
        beginMotion(Motion::Front);
        break;
    case GLFW_KEY_S:
        beginMotion(Motion::Side);
        break;
    case GLFW_KEY_B:
        beginMotion(Motion::Back);
        break;
    case GLFW_KEY_1:
        beginMotion(Motion::Hinge);
        break;
    case GLFW_KEY_2:
        beginMotion(Motion::Swap);
        break;
    case GLFW_KEY_4:
        beginMotion(Motion::OrbitY);
        break;
    case GLFW_KEY_5:
        beginMotion(Motion::OrbitX);
        break;
    default:
        break;
    }
}

void FaceMode::onUpdate(float deltaTime) {
    if (spinning_) {
        spinDegrees_ = std::fmod(spinDegrees_ + ROTATION_SPEED * deltaTime, 360.0f);
        applyObjectTransform();
    }

    if (pyramidSelected_) {
        return;
    }

    for (std::size_t index = 1; index < motions_.size(); ++index) {
        auto& motion = motions_[index];

        if (!motion.running) {
            continue;
        }

        const auto kind = static_cast<Motion>(index);

        if (kind == Motion::Front || kind == Motion::Back) {
            motion.progress = std::clamp(motion.progress + motion.direction * deltaTime, 0.0f, 1.0f);

            if (motion.progress == 0.0f || motion.progress == 1.0f) {
                motion.running = false;
            }
        } else {
            const float period = kind == Motion::Swap ? 180.0f : 360.0f;

            motion.progress = std::fmod(motion.progress + ROTATION_SPEED * deltaTime, period);
        }
    }
    applyFaceMotion();
}
````

**after** (없음)


## standard/modes/FaceCommon.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#pragma once
#include "PracticeMode.h"
#include "PracticeCommon.h"

namespace face_common {
inline constexpr float HALF_SIZE = 0.4f;
inline constexpr float ROTATION_SPEED = 45.0f;
inline const glm::vec3 X_AXIS{1, 0, 0}, Y_AXIS{0, 1, 0}, Z_AXIS{0, 0, 1};
}

class FaceMode : public PracticeMode {
protected:
    enum class Motion { None, Top, Front, Side, Back, Hinge, Swap, OrbitY, OrbitX, Count };

    struct FaceMotion {
        bool initialized = false;
        bool running = false;
        float progress = 0.0f;
        float direction = 1.0f;
        std::array<std::size_t, 2> pair{0, 3};
        glm::vec3 hinge{0.4f, 0.0f, 0.4f};
    };

    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    bool spinning_ = false;
    float spinDegrees_ = 0.0f;
    std::array<FaceMotion, static_cast<std::size_t>(Motion::Count)> motions_{};
    std::mt19937 random_{std::random_device{}()};

    /// 조작할 도형을 선택한다.
    virtual void selectShape(bool pyramid);
    /// 면별 애니메이션을 초기화한다.
    void resetFaces();
    /// 전체 조작 상태를 초기화한다.
    void resetAll();
    /// 선택한 면 애니메이션을 시작한다.
    void beginMotion(Motion kind);
    /// 물체 변환을 장면에 반영한다.
    void applyObjectTransform();
    /// 면별 변환을 장면에 반영한다.
    void applyFaceMotion();

public:
    /// 애플리케이션 참조를 보관한다.
    explicit FaceMode(graphics::Application& application) : PracticeMode(application), application_(application) {
    }

    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene() override;
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int) override;
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float deltaTime) override;
};
````

**after** (없음)


## standard/modes/FeatureContainer.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "FeatureContainer.h"
#include "Assembly.h"
#include "../Console.h"

namespace assembly {
struct FeatureContainer::Impl {
    graphics::Application& host;
    int mode;
    graphics::Scene scene;
    graphics::Camera camera;
    graphics::RenderOptions options;
    Console controller;

    struct Context {
        Impl& owner;
        graphics::Scene saved;
        graphics::Camera camera;
        graphics::RenderOptions options;
        bool closing = false;
        explicit Context(Impl& value) : owner(value), saved(std::move(value.host.getScene())),
            camera(value.host.getCamera()), options(value.host.getRenderOptions()) {
            closing = owner.host.getWindow() && graphics::detail::glfw().WindowShouldClose(owner.host.getWindow());
            owner.host.getScene() = std::move(owner.scene);
            owner.host.getCamera() = owner.camera;
            owner.host.getRenderOptions() = owner.options;
        }
        void check() const {
            require(closing || !owner.host.getWindow() || !graphics::detail::glfw().WindowShouldClose(owner.host.getWindow()),
                "실습 컨테이너가 실패로 종료를 요청했습니다. guide runtime 또는 feature reset을 사용하세요.");
        }
        ~Context() {
            if (owner.host.getWindow() && !closing) graphics::detail::glfw().SetWindowShouldClose(owner.host.getWindow(), GLFW_FALSE);
            owner.scene = std::move(owner.host.getScene());
            owner.camera = owner.host.getCamera();
            owner.options = owner.host.getRenderOptions();
            owner.host.getScene() = std::move(saved);
            owner.host.getCamera() = camera;
            owner.host.getRenderOptions() = options;
        }
    };
    Impl(graphics::Application& app, int number) : host(app), mode(number), controller(app, number) {
        require(mode >= 1 && mode <= lastStandardMode, "feature 모드는 1~22입니다. 0번을 재귀 포함할 수 없습니다.");
        Context context(*this);
        require(controller.initScene(), "실습 컨테이너 초기화 실패: " + std::to_string(mode));
        context.check();
    }
};
FeatureContainer::FeatureContainer(graphics::Application& host, int mode) : impl_(std::make_unique<Impl>(host, mode)) {}
FeatureContainer::~FeatureContainer() = default;
int FeatureContainer::mode() const { return impl_->mode; }
const graphics::Scene& FeatureContainer::scene() const { return impl_->scene; }
const graphics::Camera& FeatureContainer::camera() const { return impl_->camera; }
const graphics::RenderOptions& FeatureContainer::options() const { return impl_->options; }
void FeatureContainer::step(float dt) {
    Impl::Context context(*impl_);
    impl_->controller.onUpdate(dt);
    require(impl_->host.getScene().updateMotions(dt), "실습 컨테이너 Motion 갱신 실패.");
    context.check();
}
void FeatureContainer::key(int key, int action, int mods) {
    require(!(key == GLFW_KEY_F5 && (mods & GLFW_MOD_CONTROL)), "컨테이너 내부에서는 작업공간을 재귀 생성하지 않습니다. 상위 Ctrl+F5를 사용하세요.");
    require(key != GLFW_KEY_Q && key != GLFW_KEY_ESCAPE, "Q/Esc는 컨테이너에 전달하지 않습니다. feature enable 또는 focus none을 사용하세요.");
    Impl::Context context(*impl_);
    impl_->controller.onKey(key, action, mods);
    context.check();
}
void FeatureContainer::mouse(const graphics::MouseEvent& event, const graphics::Camera& view) {
    Impl::Context context(*impl_);
    const auto saved = impl_->host.getCamera();
    impl_->host.getCamera() = view;
    try { impl_->controller.onMouse(event); }
    catch (...) { impl_->host.getCamera() = saved; throw; }
    impl_->host.getCamera() = saved;
    context.check();
}
void FeatureContainer::cursor(double x, double y) {
    Impl::Context context(*impl_); impl_->controller.onCursor(x, y); context.check();
}
void FeatureContainer::pick(std::optional<graphics::PickResult> hit) {
    Impl::Context context(*impl_); impl_->controller.selectFromContainer(hit); context.check();
}
void FeatureContainer::focus(bool value) {
    Impl::Context context(*impl_); impl_->controller.onFocus(value); context.check();
}
} // namespace assembly
````

**after** (없음)


## standard/modes/FeatureContainer.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#pragma once
#include "PracticeMode.h"
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace assembly {
// CPU-only state. The host remains the only Application and OpenGL context.
class FeatureContainer {
    struct Impl;
    std::unique_ptr<Impl> impl_;
public:
    FeatureContainer(graphics::Application& host, int mode);
    ~FeatureContainer();
    FeatureContainer(const FeatureContainer&) = delete;
    FeatureContainer& operator=(const FeatureContainer&) = delete;
    int mode() const;
    const graphics::Scene& scene() const;
    const graphics::Camera& camera() const;
    const graphics::RenderOptions& options() const;
    void step(float dt);
    void key(int key, int action, int mods);
    void mouse(const graphics::MouseEvent& event, const graphics::Camera& view);
    void cursor(double x, double y);
    void focus(bool value);
    void pick(std::optional<graphics::PickResult> hit);
};

struct FeatureEvent {
    enum class Kind { Key, Mouse, Cursor, Focus, Pick } kind = Kind::Focus;
    int key = 0, action = 0, mods = 0;
    double x = 0, y = 0;
    bool focused = false;
    graphics::MouseEvent mouse;
    graphics::Camera view;
    std::optional<graphics::PickResult> hit;
};
struct FeatureEntity {
    graphics::EntityId group = 0;
    std::optional<graphics::EntityId> mesh, nativeParent;
    bool parentKnown = false, visible = true;
    graphics::GlColor tint;
};
struct FeatureLine {
    graphics::LineId line = 0;
    graphics::EntityId frame = 0;
    glm::vec3 start{0}, end{0};
    bool visible = true;
};
struct FeatureInstance {
    std::string name;
    std::shared_ptr<FeatureContainer> program;
    std::optional<graphics::EntityId> branch;
    std::map<graphics::EntityId, FeatureEntity> entities;
    std::map<std::size_t, FeatureLine> lines;
    std::uint64_t revision = 0;
    bool enabled = true, playing = true;
    std::vector<FeatureEvent> events;
};
} // namespace assembly
````

**after** (없음)


## standard/modes/GuideRecipes.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "GuideRecipes.h"
#include <string>
#include <utility>

namespace assembly {
const std::vector<GuideRecipe>& guideRecipes() {
    static const std::vector<GuideRecipe> recipes{
        {"tank", "포신 길이와 중심을 함께 변경", "scene.part(\"box\", gun, \"barrel\", {0,0,1}, {.2f,.2f,2});\nscene.part(\"box\", gun, \"muzzle\", {0,0,2}, {.35f,.35f,.3f});", "scene.part(\"box\", gun, \"barrel\", {0,0,1.5f}, {.2f,.2f,3});\nscene.part(\"box\", gun, \"muzzle\", {0,0,3}, {.35f,.35f,.3f});", "길이는 모델 단위다. 길이 L이면 중심은 L/2, 총구는 L이다. 관절 위치는 유지한다.", "1. gun 기준점과 barrel/muzzle 위치, turretMotor.seconds를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "포탑 회전 중 포신 시작점은 고정되고 끝만 1 늘어난다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExamples.cpp", "scene.part(\"box\", gun, \"barrel\"", "포신 길이와 중심을 함께 변경"}, {"modes/AssemblyExamples.cpp", "rotation.degreesPerSecond =", "포탑 각속도: 도/초"}}},
        {"truck", "문 회전축과 열림 범위 수정", "pendulum(scene, name + \".leftDoorMotor\", left, {0,1,0}, 40,6,40,-90);", "pendulum(scene, name + \".leftDoorMotor\", left, {0,1,0}, 30,6,30,-90);", "offset±amplitude가 범위다. 예시는 0~80도를 0~60도로 바꾼다. 문 메시 중심을 힌지에서 떨어뜨려 둔다.", "1. leftDoor pivot와 panel 로컬 X=+.425, phase=-90을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "처음 닫힘을 유지하며 최대 열림각이 60도가 된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExamples.cpp", "const auto left = scene.part", "문 회전축과 열림 범위 수정"}, {"modes/AssemblyExamples.cpp", "void buildTruckExample(", "트럭/컨테이너 생성"}}},
        {"person", "사람 어깨·팔 비율 수정", "{sign * .38f, .17f, 0}", "{sign * .45f, .17f, 0}", "어깨 위치는 torso 로컬 좌표다. upperArm 길이를 바꾸면 elbow의 Y 오프셋도 같은 길이로 맞춘다.", "1. shoulder→elbow→forearm 부모 체인과 양팔 phase 차이180도를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "몸통 폭을 바꾸지 않고 어깨 간격이 넓어진다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExamples.cpp", "const auto shoulder = scene.part", "사람 어깨·팔 비율 수정"}, {"modes/AssemblyExamples.cpp", "void buildPersonExample(", "사람 전체 생성"}}},
        {"objects", "기본 형상 크기 지정", "scene.add(\"box\", \"crate\", {0,1,0}, {1,1,1});", "scene.add(\"box\", \"crate\", {0,1,0}, {2,1,.5f});", "생성 치수는 정점 데이터다. 전체 비율 변경은 scale/SRT와 구분한다. 크기는 양수다.", "1. Node.mesh와 실제 mesh.localVertices, 생성 실패 require 문구를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "폭2·높이1·깊이.5 상자를 생성한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "void Assembly::add(", "기본 형상 크기 지정"}}},
        {"geometry", "사용자 정점·삼각형 추가", "triangle plate 0 0 0 1 0 0 0 1 0", "triangle plate 0 0 0 2 0 0 0 1 0", "명령 예시다. 중복/일직선 점은 삼각형 면적0이므로 거절한다. C++에서는 Model→Assembly::model을 사용한다.", "1. triangleIndices 범위, cross(B-A,C-A), geometryRevision을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "가로 길이가2인 삼각형을 만든다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblySurface.cpp", "void Assembly::model(", "사용자 정점·삼각형 추가"}}},
        {"hierarchy", "관절을 부모에 붙이는 위치 수정", "joint robot elbow 0 -0.4 0", "joint robot elbow 0 -0.6 0", "부모 로컬 좌표다. parent는 현재 로컬 SRT를 유지하며 월드 자세를 자동 보존하지 않는다.", "1. 부모 ID, localBasis, worldMatrix의 부모 곱 순서를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "팔꿈치 연결점이 부모 아래로 .2 이동한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyHierarchy.cpp", "std::string Assembly::part(", "관절을 부모에 붙이는 위치 수정"}}},
        {"srt", "루트 위치·회전·균일 배율 변경", "srt hero 0 0 0 0 0 0 1", "srt hero 2 0 0 0 90 0 1.5", "각도는 도, 회전은 Rz*Ry*Rx다. 위치는 부모 공간 피벗 좌표다. 양수 균일 배율 .0001 초과100 이하.", "1. rest.basis/rotation/scale와 실제 Scene transform을 비교한다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "전체 계층이 X=2, Y회전90도, 1.5배가 된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyHierarchy.cpp", "void Assembly::srt(", "루트 위치·회전·균일 배율 변경"}}},
        {"pivot", "중심을 바꾸고 기준 배치 보존", "pivot hero 0 0 0 raw", "pivot hero 0 0.9 0 keep", "keep는 basis += R*S*(새중심-이전중심)이다. 동작 부착 전 또는 seek0에서 수정한다.", "1. rest.localBasis와 basis 보정, 중심을 둔 회전의 T(C)*R*T(-C)를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "기준 외형 위치를 유지하며 이후 자전 중심이 높이.9가 된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyHierarchy.cpp", "void Assembly::pivot(", "중심을 바꾸고 기준 배치 보존"}}},
        {"center", "하위 형상 AABB로 중심 계산", "center hero raw", "center hero keep", "현재 하위 메시의 로컬 AABB 중심이며 질량중심이 아니다. 숨긴 면도 포함한다.", "1. inverse(rootWorld)*faceWorld*vertex의 최소/최대, 활성 모션 시간을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "현재 형상 배치가 유지되며 자동 계산 중심을 사용한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyHierarchy.cpp", "glm::vec3 Assembly::center(", "하위 형상 AABB로 중심 계산"}}},
        {"mount", "면·삼각형 좌표에 관절 부착", "mount base hinge 4 0 0 0.5 0", "mount base hinge 4 0 0.25 0.25 0.1", "P=(1-U-V)A+UB+VC. U,V>=0,U+V<=1. 마지막 값은 메시 로컬 기하법선 거리다.", "1. surface의 삼각형 인덱스, U/V, 기하법선과 shading normal 차이를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "관절이 지정 표면 좌표에서 법선 방향 .1 떨어져 면 동작을 따른다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblySurface.cpp", "std::string Assembly::mount(", "면·삼각형 좌표에 관절 부착"}, {"modes/AssemblySurface.cpp", "void Assembly::applyMounts(", "프레임마다 부착점과 방향 추적"}}},
        {"normal", "조명 노멀과 winding 분리", "normal crate 0 0 0 1", "normal crate 0 0 0 -1", "노멀은 조명용이다. 컬링을 바꾸려면 cull/winding을 따로 바꾼다. 0 벡터 금지, 입력은 정규화한다.", "1. vertexNormals 크기, inverse-transpose(world)의 방향, render lighting on을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "빛 반응은 뒤집히며 삼각형 앞면과 mount 방향은 유지된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblySurface.cpp", "} else if (command == \"normal\")", "조명 노멀과 winding 분리"}}},
        {"warp", "정적 외곽 그룹의 비균일·반사", "warp frame 1 1 1", "warp frame -1 1.5 1", "0 배율 금지. 음수 X는 반사다. 일반 모션은 자식 그룹에 붙인다. 물리 방은 강체 조건을 유지한다.", "1. world determinant 음수와 피킹/렌더 컬링 일치를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "자식 전체를 X반사·Y1.5배로 표시한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblySurface.cpp", "if (command == \"warp\")", "정적 외곽 그룹의 비균일·반사"}}},
        {"face-track", "지연·왕복 면 회전", "face-track fold box 4 2 0 once turn 0 0 1 -1 1 0 90", "face-track fold box 4 3 1 pingpong turn 0 0 1 -1 1 0 60", "시간/지연은 초, 각도는 도다. 시간 .0001초 이상. 같은 면 소유자는 하나다.", "1. seconds/지연/진행률u/faceTrackBase를 seek로 비교한다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "1초 대기 후3초 동안60도 열리고3초 동안 닫힌다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyTracks.cpp", "bool Assembly::trackCommand(", "지연·왕복 면 회전"}}},
        {"cpp-effect", "절대 시각으로 효과 계산", "// USER_COMMAND_BEGIN 아래에 pulse 명령 분기 없음", "if (args[0] == \"pulse\") {\n    require(args.size() == 3, \"pulse <동작이름> <대상>\");\n    Behavior b;\n    b.name = args[1]; b.target = args[2];\n    b.effect = Effect::Custom; b.positionDriver = true;\n    const auto id = scene.node(args[2]).id;\n    b.custom = [id](graphics::Scene& output, const graphics::Transform3D& rest, double seconds, float) {\n        auto pose = rest;\n        pose.scale *= float(1 + .25 * std::sin(seconds * 2));\n        return output.setTransform(id, pose);\n    };\n    scene.attach(std::move(b));\n    return true;\n}", "AssemblyExtensions.cpp의 customCommand 안에 삽입하고 상단에 <cmath>/<utility>를 include한다. 사용자 재빌드 후 pulse breathing hero를 입력한다. 이전 배율 누적 대신 기준 rest로 매 프레임 재계산한다. 함수 지역 pose는 콜백 내부에서 선언한다.", "1. seconds와 dt, pause 때 dt=0인지, 대상 SRT 소유자를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "배율이 기준의 .75~1.25 범위에서만 움직인다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExtensions.cpp", "// USER_COMMAND_BEGIN", "customCommand 내부에 새 분기 추가. <cmath>와 <utility>도 include"}, {"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Custom)", "편집 위치가 아닌 콜백 실행 중단점"}}},
        {"humanoid", "리그 전체 크기·비율 변경", "config.grid.frame = glm::translate(glm::mat4(1), position);", "config.grid.frame = glm::translate(glm::mat4(1), position);\nconfig.grid.extent = {1, 2.1f, .6f};", "GridLayout extent는 모델 치수이며 각 관절 연결도 함께 재계산한다.", "1. shoulders/elbows/hips/knees 부모와 grid.frame을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "리그 전체 키가 커지며 관절 계층을 유지한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "config.grid.frame =", "add humanoid 생성 설정: 이 줄 아래에 extent 추가"}}},
        {"swing", "그네 진폭·주기 변경", "config.pivot += position;", "config.pivot += position;\nconfig.pendulum.amplitudeDegrees = 40;\nconfig.pendulum.periodSeconds = 4;", "도와 초를 사용한다. 지지대는 frame, 흔들리는 좌석/사람은 hinge 아래에 둔다.", "1. frame/hinge/seat/rider 부모 및 PendulumMotion phase를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "지지대는 고정되고 좌석과 탑승자가 더 크게, 느리게 흔들린다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "config.pivot += position;", "add swing 생성 설정: 이 줄 아래에 진폭/주기 추가"}}},
        {"tank-preset", "기존 실습19/22 공유 탱크 치수", "{.65f,.6f,1.8f}", "{.8f,.6f,2.0f}", "공유 생성기를 고치면 실습19/22 모두 바뀐다. 0번만 바꾸려면 buildTankExample을 편집한다.", "1. lower/central/upper/guns/poles ID와 형상 부모를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "양쪽 궤도의 폭·길이가 함께 커진다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyParts.cpp", "TankParts addTankParts(", "기존 실습19/22 공유 탱크 치수"}}},
        {"walker", "충돌 보행 리그의 비율 맞추기", "grid.frame = glm::translate(glm::mat4(1), position);\n// AssemblyMotion.cpp\nb.character.height = 1.8f;", "grid.frame = glm::translate(glm::mat4(1), position);\ngrid.extent.y = 2.0f;\n// AssemblyMotion.cpp\nb.character.height = 2.0f;", "충돌 높이와 생성 grid.extent.y를 함께 맞춘다. 임의 사람 그룹은 JointRig 어댑터가 필요하다.", "1. feet/grounded/height/halfWidth와 어깨·엉덩이 ID를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "그림의 머리 높이와 천장 충돌 높이가 일치한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "grid.frame =", "walker 그림 높이: 이 줄 아래에 extent.y 추가"}, {"modes/AssemblyMotion.cpp", "b.character.height =", "충돌 높이: 생성 높이와 함께 변경"}}},
        {"grid", "격자 수와 셀 치수 변경", "grid.cells = {6, 1, 4};", "grid.cells = {8, 1, 4};", "셀 개수는 정수이며 origin/extent와 별개다. 인덱스/번호·경로도 재계산한다.", "1. cells와 cellCenter, 상대 크기, 이동 상자의 기본 선택을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 면적을 X방향8칸으로 나눈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "grid.cells =", "조립 add grid 설정값"}, {"modes/Mode7Grid.cpp", "grid.cells =", "독립/컨테이너 모드7 설정값"}}},
        {"room", "방의 표시·충돌 경계 함께 변경", "const graphics::Bounds3 bounds{{-2,0,-2},{2,3,2}};", "const graphics::Bounds3 bounds{{-3,0,-3},{3,4,3}};", "room mesh와 Node.room에 같은 bounds를 전달한다. 방은 강체, walker 방은 월드 축 정렬이다.", "1. room.room bounds, walls face5 visible, RoomBody.localPosition을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "넓이6·높이4 방에서 그림과 충돌 경계가 일치한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyObjects.cpp", "} else if (kind == \"room\")", "방의 표시·충돌 경계 함께 변경"}}},
        {"patterns", "조합할 계층과 기능을 고르기", "build tank rover", "build truck carrier", "복합 객체를 큐브/관절/모션으로 분리한다. 예제 함수 변경은 재빌드가 필요하다.", "1. parts의 이름, 최상위 루트와 직접 편집할 예제 함수를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "별도 파일의 탱크·트럭·사람 예제로 이동한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"docs/assembly/PATTERN_COMBINATIONS.md", "# 기능 조합 패턴", "조합할 계층과 기능을 고르기"}}},
        {"motion", "이동과 자전의 소유권 분리", "path walk hero 5 8", "path walk hero 5 12", "초 단위 주기가 늘면 속도가 줄어든다. 경로+공전 등 위치 소유자는 같은 노드에 중복 부착하지 않는다.", "1. Behavior.target/positionDriver/seconds, rest와 Motion.sample을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를12초에 한 번 이동한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyMotion.cpp", "bool Assembly::motionCommand(", "이동과 자전의 소유권 분리"}}},
        {"face", "면 힌지 모서리·속도 변경", "face lid box 4 hinge 0 30", "face lid box 4 hinge 1 -15", "면/모서리는0부터, 속도는 도/초다. hinge축은 실제 모서리에서 구한다.", "1. surface 정점 순서와 모서리 양끝, 면 소유 ID를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "다른 모서리에서 반대 방향·절반 속도로 회전한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyMotion.cpp", "} else if (command == \"face\"", "면 힌지 모서리·속도 변경"}}},
        {"physics", "중력·탄성·이탈 진단", "body fall gravity 0 -9.8 0", "body fall gravity 0 -4.9 0", "중력은 월드 가속도다. 몸체 속도는 이탈 전 방 로컬, 이후 월드다. 큰 시간 간격을 피한다.", "1. released/localPosition/worldPosition/worldVelocity/previousRoom/halfExtent를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "낙하 가속도가 절반이 되며 바닥 이탈 후 월드 중력을 따른다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Body)", "중력·탄성·이탈 진단"}}},
        {"input", "기능별 입력과 해제 처리", "// USER_COMMAND_BEGIN 아래에 turn-key 명령 분기 없음", "if (args[0] == \"turn-key\") {\n    require(args.size() == 3, \"turn-key <동작이름> <대상>\");\n    Behavior input;\n    input.name = args[1]; input.target = args[2];\n    input.effect = Effect::Input;\n    input.key = [name = args[2]](Assembly& a, int key, int action, int) {\n        if (key != GLFW_KEY_H || action != GLFW_PRESS) return false;\n        a.rotate(name, {0,1,0}, 5);\n        return true;\n    };\n    scene.attach(std::move(input));\n    return true;\n}", "AssemblyExtensions.cpp의 customCommand 안에 삽입하고 <utility>를 include한다. 사용자 재빌드 후 turn-key steering hero를 입력한다. bool은 입력 소비 여부다. focus(false)에서 사용자 누름 상태도 해제한다.", "1. action PRESS/REPEAT/RELEASE, physicalMods, topologyRevision과 callback 수명을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "H 최초 누름만5도 회전하며 기본 입력으로 중복 전달하지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExtensions.cpp", "// USER_COMMAND_BEGIN", "customCommand 내부에 새 분기 추가. <utility>도 include"}, {"modes/AssemblyInput.cpp", "bool Assembly::dispatchInput(", "편집 위치가 아닌 입력 전달 중단점"}}},
        {"camera", "시점·주목점·투영 수정", "camera pose 4 3 6 0 1 0 0 1 0", "camera pose 8 5 10 0 1 0 0 1 0", "position과 target이 같으면 안 되고 up은 시선과 평행할 수 없다. feature camera 포트가 활성화되면 그 포트가 시점을 소유한다.", "1. position/target/up, near/far, viewport 및 DPI를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 주목점을 더 멀리서 본다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyInput.cpp", "if (a[1] == \"pose\")", "시점·주목점·투영 수정"}}},
        {"recipe", "기본 조립 목록 수정", "\"build tank rover\",", "\"build truck rover\",", "객체 이름을 재사용할 때 초기 clear 이후 순서를 지킨다. 이 C++ 배열 변경은 재빌드, 외부 .assembly 변경은 reload다.", "1. defaultRecipe의 실패 줄, 중복 이름, 입력 바인딩 대상 존재 여부를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "시작 구성에서 rover가 트럭으로 생성된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode0Recipe.cpp", "USER_RECIPE_BEGIN", "기본 조립 목록 수정"}}},
        {"custom", "사용자 조립 함수 등록", "scene.move(args[1], axis(args[2]) * float(number(args[3])));", "scene.move(args[1], axis(args[2]) * float(number(args[3])) * 2.0f);", "nudge의 거리 배율을 두 배로 하는 예다. 새 명령은 인자 수·수치·대상 존재 검사를 먼저 둔다.", "1. args token 수와 require 실패 위치, 명령 전후 rest.basis를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 nudge 거리 입력이 두 배 이동한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyExtensions.cpp", "USER_COMMAND_BEGIN", "사용자 조립 함수 등록"}}},
        {"runtime", "합성 순서와 덮어쓰기 진단", "rest.rotation = rotation;", "rest.rotation = baseRotation * rotation;", "새로운 절대 자세가 사용자 기준값을 지우지 않게 합성한다. 도입한 행렬의 좌표 공간을 명시한다.", "1. 부모/로컬 MotionSample과 rest, 한 프레임 dt 호출 수를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "수동 회전을 유지하면서 자동 모션이 중첩된다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyRuntime.cpp", "bool Assembly::applyPoses(", "합성 순서와 덮어쓰기 진단"}}},
        {"save", "파일 재로드·실패 복원", "load \"examples/assembly/tank.assembly\"", "hotload \"examples/assembly/workspace.assembly\"\nreload", "레시피는 현재 프레임 스냅샷이 아니다. 잘못된 파일은 outer execute가 CPU Scene/State를 복원한다.", "1. 실제 절대 경로, 실패 줄, CRLF 제거와 # @state-begin 경계를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "Ctrl+F5가 지정 파일을 다시 읽고 실패 시 기존 조립을 유지한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyPersistence.cpp", "bool Assembly::load(", "파일 재로드·실패 복원"}}},
        {"hotload", "조립 파일 핫로드와 코드 변경 구분", "hotload \"examples/assembly/workspace.assembly\"", "hotload \"my-scene.assembly\"", "Ctrl+F5는 외부 레시피 재로드다. C++ 소스 변경은 컴파일/링크 없이는 실행 코드가 바뀌지 않는다.", "1. reloadPath, load 실패 줄과 savedState, focus release를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "지정 파일의 구성으로 교체하고 실패 시 이전 구성을 보존한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode0Assembly.cpp", "key == GLFW_KEY_F5", "조립 파일 핫로드와 코드 변경 구분"}, {"modes/AssemblyCommands.cpp", "if (a[0] == \"reload\")", "재로드 진입"}}},
        {"feature", "기존 실습을 독립 컨테이너·라이브 포트로 조립", "scene.addFeature(\"a\", 9);", "scene.addFeature(\"a\", 9);\nscene.addFeature(\"b\", 16);", "1~22를 선택한다. add는 독립 시계, port는 원본과 공유 시계다. 라이브 형상은 원본 소유, 독립 편집은 snapshot이다.", "1. FeatureInstance.program/branch/events, private Scene과 exported face frame을 비교한다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "서로의 Scene.clear와 상태를 덮어쓰지 않고 두 실습을 함께 갱신한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyFeatures.cpp", "void Assembly::addFeature(", "기존 실습을 독립 컨테이너·라이브 포트로 조립"}, {"modes/FeatureContainer.cpp", "struct Context", "CPU 상태 교환/복원"}, {"modes/AssemblyFeatures.cpp", "void Assembly::syncFeatures(", "출력 포트와 추가 SRT 합성"}}},
        {"debug", "잘못된 입력 반응을 파이프라인별로 좁히기", "inspect hero", "diagnose hero", "diagnose는 등록된 실제 런타임 필드와 행렬을 출력한다. 임의 C++ 지역변수는 디버거에서 관찰한다.", "1. rest→parent→world→face frame→camera/viewport 순서로 첫 차이를 찾는다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "덮어쓰는 소유자, 잘못된 피벗/반사/뷰포트의 근거를 확인한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/AssemblyDiagnostics.cpp", "void Assembly::diagnose(", "잘못된 입력 반응을 파이프라인별로 좁히기"}}},
        {"mode1", "모드1 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode1Ellipse.cpp", "path.durationSeconds =", "모드1 경로 주기 수정"}}},
        {"mode2", "모드2 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode2Rectangle.cpp", "path.durationSeconds =", "모드2 경로 주기 수정"}}},
        {"mode3", "모드3 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode3Polygon.cpp", "path.durationSeconds =", "모드3 경로 주기 수정"}}},
        {"mode4", "모드4 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode4Spatial.cpp", "path.durationSeconds =", "모드4 경로 주기 수정"}}},
        {"mode5", "모드5 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode5Circle.cpp", "path.durationSeconds =", "모드5 경로 주기 수정"}}},
        {"mode6", "모드6 경로 주기 수정", "path.durationSeconds = 6;", "path.durationSeconds = 12;", "경로 파라미터 주기는 초다. 경로 함수 모양과 보간은 독립이다.", "1. PathMotion.durationSeconds와 Motion.seconds/u, 시작 offset을 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "같은 경로를 절반 속도로 돈다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode6Polyline.cpp", "path.durationSeconds =", "모드6 경로 주기 수정"}}},
        {"mode7", "격자 셀 수 수정", "grid.cells = {6,1,4};", "grid.cells = {8,1,4};", "단위는 소스의 모델 좌표/도/초를 따른다. 실습 전용 값과 조립 외곽 SRT를 구분한다.", "1. 원본 Console 상태와 기능 포트의 추가 SRT를 각각 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "격자 셀 수이 변경되며 다른 실습 인스턴스에는 직접 전달되지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode7Grid.cpp", "grid.cells =", "격자 셀 수 수정"}}},
        {"mode8", "물리 상자 시작 높이 수정", "roomBody_.localPosition = {0,-.75f,0};", "roomBody_.localPosition = {0,.5f,0};", "단위는 소스의 모델 좌표/도/초를 따른다. 실습 전용 값과 조립 외곽 SRT를 구분한다.", "1. 원본 Console 상태와 기능 포트의 추가 SRT를 각각 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "물리 상자 시작 높이이 변경되며 다른 실습 인스턴스에는 직접 전달되지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode8Room.cpp", "roomBody_.localPosition =", "물리 상자 시작 높이 수정"}}},
        {"mode9", "그네 진폭 수정", "swingConfig.pendulum.amplitudeDegrees = 25;", "swingConfig.pendulum.amplitudeDegrees = 40;", "단위는 소스의 모델 좌표/도/초를 따른다. 실습 전용 값과 조립 외곽 SRT를 구분한다.", "1. 원본 Console 상태와 기능 포트의 추가 SRT를 각각 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "그네 진폭이 변경되며 다른 실습 인스턴스에는 직접 전달되지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode9Animation.cpp", "swingConfig.pendulum.amplitudeDegrees =", "그네 진폭 수정"}}},
        {"practice13", "실습13 표시 면 조합", "selectFaces(cuboidId_, {0});", "selectFaces(cuboidId_, {0,4});", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. 면 인덱스·visible와 난수 첫/두 번째 선택 중복\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "표시 면 조합 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode10Practice13.cpp", "void Practice13::selectFaces", "실습13 표시 면 조합"}}},
        {"practice14", "실습14 자전 속도", "const float angle = glm::radians(45.0f * deltaTime);", "const float angle = glm::radians(90.0f * deltaTime);", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. angularDirection/rotation, 입력 release와 dt\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "자전 속도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode11Practice14.cpp", "void Practice14::onUpdate", "실습14 자전 속도"}}},
        {"practice15", "실습15 면 축소 특이행렬 방지", "compose(1, scaleAround({0.0f, 0.0f, -HALF_SIZE}, 1.0f - progress));", "compose(1, scaleAround({0.0f, 0.0f, -HALF_SIZE}, std::max(.01f, 1.0f - progress)));", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. motions_.progress/direction/running와 한 면에 합성한 행렬 순서\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "면 축소 특이행렬 방지 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/FaceCommon.cpp", "void FaceMode::applyFaceMotion", "실습15 면 축소 특이행렬 방지"}}},
        {"practice16", "실습16 순차 펼침 시간 분할", "const auto active = std::size_t(pyramidProgress_ / 2.0f) % 4;\nconst float phase = std::fmod(pyramidProgress_, 2.0f);", "const auto active = std::size_t(pyramidProgress_ / 3.0f) % 4;\nconst float phase = std::fmod(pyramidProgress_, 3.0f) * (2.0f/3.0f);", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. active와 phase를 같은 주기3으로 함께 변경; atan2(h,b) 각도\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "순차 펼침 시간 분할 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode13Practice16.cpp", "void Practice16::applyPyramid", "실습16 순차 펼침 시간 분할"}}},
        {"practice17", "실습17 교환 우회 높이", "point.y += (i == 0 ? 1.0f : -1.0f) * .9f * std::sin(3.14159265f * t);", "point.y += (i == 0 ? 1.0f : -1.0f) * 1.4f * std::sin(3.14159265f * t);", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. starts_/motionOffsets_/elapsed와 수동 이동 보존\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "교환 우회 높이 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode14Practice17.cpp", "void Practice17::onUpdate", "실습17 교환 우회 높이"}}},
        {"practice18", "실습18 행성 궤도 반지름", "constexpr std::array<float, 3> radii{2.0f, 2.8f, 3.6f};", "constexpr std::array<float, 3> radii{2.4f, 3.2f, 4.0f};", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. 행성/달 부모 프레임, orbit angle, 궤도선 평면을 같은 값으로 맞춤\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "행성 궤도 반지름 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode15Practice18.cpp", "constexpr std::array<float, 3> radii", "실습18 행성 궤도 반지름"}}},
        {"practice19", "실습19 탱크 이동 속도", "position_ += glm::normalize(movement) * dt * 1.3f;", "position_ += glm::normalize(movement) * dt * 1.8f;", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. moving direction 재입력 토글, 전역 이동과 부품 SRT 분리\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "탱크 이동 속도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/TankCommon.cpp", "position_ += glm::normalize(movement)", "실습19 탱크 이동 속도"}}},
        {"practice20", "실습20 컨베이어 거리 속도", "constexpr double beltSpeed = 1.25;", "constexpr double beltSpeed = 2.0;", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. distance 누적과 PathPose.tangent/yaw; 경로 길이/상자 위상\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "컨베이어 거리 속도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode17Practice20.cpp", "constexpr double beltSpeed", "실습20 컨베이어 거리 속도"}}},
        {"practice21", "실습21 마우스 회전 감도", "(previousMouseX_ - x) * .3", "(previousMouseX_ - x) * .15", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. pendingRotation_/roomAngle_/previousMouseX, 각속도와 이탈\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "마우스 회전 감도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode18Practice21.cpp", "void Practice21::onCursor", "실습21 마우스 회전 감도"}}},
        {"practice22", "실습22 공유 탱크의3뷰 출력", "view 1", "view 3", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. 렌더 패스에서 onUpdate가 재호출되지 않는지; workspace의 view 설정\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "공유 탱크의3뷰 출력 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/TankCommon.cpp", "bool TankMode::onRender", "실습22 공유 탱크의3뷰 출력"}}},
        {"practice23", "실습23 보행 점프 속도", "graphics::CollisionSystem::jump(body_, 5.5f);", "graphics::CollisionSystem::jump(body_, 4.5f);", "참조 예시의 식/변수 이름을 현재 출력한 함수 안의 실제 이름에 맞춘다. 임의로 새 지역변수를 더하지 말고 해당 계산의 계수를 교체한다. 도/라디안 변환과 초 단위를 유지한다.", "1. feet/grounded/verticalSpeed, body.height와 그림 리그 높이\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "보행 점프 속도 변경을 동일한 입력/시각에서 비교한다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode20Practice23.cpp", "graphics::CollisionSystem::jump(body_,", "실습23 보행 점프 속도"}}},
        {"mode21", "카메라 충돌 반지름 변경", "glm::dot(difference,difference) <= .15f*.15f", "glm::dot(difference,difference) <= .2f*.2f", "충돌 반지름과 이동 substep 간격을 같이 바꾼다. 모드21 카메라 포트를 선택해야 카메라 동작이 보인다.", "1. closest/difference/movement/steps와 카메라 포트 소유자를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "상자에서 더 먼 지점에 카메라가 멈춘다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode21CameraCollision.cpp", "bool blocked(", "카메라 충돌 반지름 변경"}}},
        {"mode22", "내장 정점·UV·삼각형 오류 수정", "{0,.5f,0}", "{0,.8f,0}", "위치는 과제 범위 [-1,1], UV는 [0,1]을 유지한다. 예시는 정점 배열에서 꼭짓점 Y만 .5에서 .8로 변경한다. 오류가 나면 입력 인덱스를 고치고 검사 조건을 완화하지 않는다.", "1. errors의 최초 항목, 중복 코너, cross 면적과 UV 혼합 여부를 본다.\n2. pause on 또는 feature play <이름> off 후 diagnose <대상>와 surface <대상> <면>을 읽는다.\n3. 출력한 줄에 중단점을 두고 입력→기준값→프레임 합성→faceWorldMatrix를 추적한다.\n4. 예시의 변경을 한 항목씩 적용한다. 레시피는 Ctrl+F5, C++는 사용자 재빌드 후 같은 입력으로 예상 결과를 비교한다.", "피라미드 꼭짓점이 .3 높아진다. 입력 오류는 errors의 최초 항목에서 고치고 범위 검사 자체를 약화하지 않는다.", "docs/assembly/EDIT_DEBUG_GUIDE.md", {{"modes/Mode22EmbeddedMesh.cpp", "const std::vector<glm::vec3> positions", "내장 정점·UV·삼각형 오류 수정"}}},
    };
    return recipes;
}

const GuideRecipe* findGuideRecipe(std::string_view requested) {
    std::string topic(requested);
    const std::pair<const char*, const char*> aliases[]{
        {"group","hierarchy"},{"part","hierarchy"},{"joint","hierarchy"},{"anchor","mount"},{"unmount","mount"},
        {"add","objects"},{"parent","hierarchy"},{"color","objects"},{"show","objects"},{"winding","normal"},{"orient","normal"},
        {"triangle","geometry"},{"triangle-add","geometry"},{"surface","geometry"},{"vertex","geometry"},{"face-srt","geometry"},{"face-mode","geometry"},{"facecolor","geometry"},{"outline","geometry"},{"line","geometry"},{"digit","geometry"},
        {"spin","motion"},{"orbit","motion"},{"path","motion"},{"linear","motion"},{"pendulum","motion"},{"remove","motion"},{"wall","face"},
        {"walk","physics"},{"jump","physics"},{"obstacle","physics"},{"body","physics"},{"release","physics"},{"bind","input"},{"unbind","input"},{"drag","input"},{"effects","input"},{"render","input"},
        {"load","save"},{"reload","hotload"},{"diagnose","debug"},{"tank-pattern","tank"},{"truck-pattern","truck"},{"person-pattern","person"},{"container","feature"}};
    for (const auto& alias : aliases) if (topic == alias.first) { topic = alias.second; break; }
    for (int mode = 10; mode <= 20; ++mode) if (topic == "mode" + std::to_string(mode)) topic = "practice" + std::to_string(mode + 3);
    for (const auto& recipe : guideRecipes()) if (topic == recipe.topic) return &recipe;
    return nullptr;
}
} // namespace assembly
````

**after** (없음)


## standard/modes/GuideRecipes.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#pragma once
#include <string_view>
#include <vector>

namespace assembly {
struct GuideSite {
    const char* file;
    const char* marker;
    const char* purpose;
};

struct GuideRecipe {
    const char* topic;
    const char* goal;
    const char* before;
    const char* after;
    const char* reason;
    const char* debug;
    const char* expected;
    const char* document;
    std::vector<GuideSite> sites;
};

const std::vector<GuideRecipe>& guideRecipes();
const GuideRecipe* findGuideRecipe(std::string_view topic);
} // namespace assembly
````

**after** (없음)


## standard/modes/Mode0Assembly.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Assembly.h"
#include "AssemblyConsole.h"
#include <iostream>

namespace {
class AssemblyMode final : public PracticeMode {
    graphics::Application& app_;
    assembly::Assembly scene_;
    assembly::ConsoleInput input_;
public:
    explicit AssemblyMode(graphics::Application& app) : PracticeMode(app), app_(app), scene_(app) {}
    bool initScene() override {
        if (!assembly::defaultRecipe(scene_, std::cout))
            std::cout << "기본 레시피가 중단됐습니다. 콘솔에서 수정하거나 guide recipe를 사용하세요.\n";
        scene_.help(std::cout);
        input_.prompt();
        return true;
    }
    bool capturesNumericKeys() const override { return scene_.capturesNumericKeys(); }
    void onKey(int key, int action, int mods) override {
        if (key == GLFW_KEY_F5 && action == GLFW_PRESS && (mods & GLFW_MOD_CONTROL)) {
            scene_.execute("reload", std::cout);
            return;
        }
        if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
            assembly::defaultRecipe(scene_, std::cout);
            return;
        }
        scene_.onKey(key, action, mods, std::cout);
    }
    void onMouse(const graphics::MouseEvent& event) override { scene_.onMouse(event, std::cout); }
    void onCursor(double x, double y) override { scene_.onCursor(x, y, std::cout); }
    void onFocus(bool focused) override { scene_.onFocus(focused); }
    void onUpdate(float dt) override {
        for (unsigned i = 0; i < 8; ++i) {
            const auto line = input_.poll();
            if (!line) break;
            if (*line == "quit") {
                app_.requestClose();
                return;
            }
            scene_.execute(*line, std::cout);
            input_.prompt();
        }
        scene_.update(dt, std::cout);
    }
    bool onRender() override { return scene_.render(); }
};
}

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app) {
    return std::make_unique<AssemblyMode>(app);
}
````

**after** (없음)


## standard/modes/Mode0Recipe.cpp

### 블록 1 · before L2 / after L2

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp

````

**after** (없음)


### 블록 2 · before L8 / after L7

위치: `bool defaultRecipe(Assembly& scene, std::ostream& out) {`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
        "camera pose 16 11 20 0 1 1 0 1 0",
        "camera fov 55",
        "add grid floor 0 0 0",
        "build person hero",
        "move hero -2 0 0",
        "path stroll hero 5 8",
        "add swing ride 2 0 0",
        "build tank rover",
        "move rover 5 0 0",
        "color rover 0.3 0.6 0.8",
        "add room lab -6 0 0",
        "add box cargo -6 1 0 0.5 0.5 0.5",
        "physics falling cargo lab",
        "add walker robot -5.5 0 -0.5",
        "walk walking robot lab 0.7",
        "obstacle walking cargo",
        "bind RIGHT hold \"move hero 1 0 0\"",
        "bind LEFT hold \"move hero -1 0 0\"",
        "bind J press \"jump walking 5.5\"",
        "bind F press \"visible lab 5 off\"",
        "bind G press \"visible lab 5 on\"",
        "select hero"
````

**after**

````cpp
        "camera pose 8 6 10 0 0 0 0 1 0",
        "add box box1 0 0 0",
        "select box1"
````

## standard/modes/Mode10Practice13.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <initializer_list>
#include <random>
#include <iostream>

namespace {
// 실습 13: 표시할 면과 난수 상태만 관리한다.
class Practice13 final : public PracticeMode {
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    std::mt19937 random_{std::random_device{}()};

    void selectFaces(graphics::EntityId id, std::initializer_list<std::size_t> faces);

public:
    explicit Practice13(graphics::Application& application) : PracticeMode(application), application_(application) {
    }

    bool initScene() override;

    void onKey(int key, int action, int) override;
};


bool Practice13::initScene() {
    graphics::Scene& scene = application_.getScene();

    scene.clear();

    const auto cube = scene.addCuboid(glm::vec3(0.8f), glm::vec3(0.0f));
    const auto pyramid = scene.addPyramid(glm::vec3(0.8f), glm::vec3(0.0f));

    if (!cube || !pyramid) {
        return false;
    }
    cuboidId_ = *cube;
    pyramidId_ = *pyramid;

    graphics::Transform3D initial;

    initial.rotation = glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0}));

    if (!scene.setTransform(cuboidId_, initial) || !scene.setTransform(pyramidId_, initial)) {
        return false;
    }
    // 축은 객체에 연결하지 않는다. 원점부터 양·음 방향으로 각각 1만큼 표시한다.
    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();

    camera.position = {1.8f, 1.2f, 3.0f};
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    selectFaces(cuboidId_, {0, 1, 2, 3, 4, 5});
    std::cout << "실습 13: 키 설명==면(색)\n"
              << "1==front(red), 2==back(green), 3==left(blue), 4==right(yellow)\n"
              << "5 top(cyan), 6 bottom(magenta)\n"
              << "7/8/9/0는 pyramid == front(red)/right(blue)/back(green)/left(yellow)\n"
              << "C: 랜덤한 cube 2개 faces, T: pyramid 밑변 + 랜덤한 1개 변\n"
              << "Axes: X red, Y green, Z blue. Esc: quit\n";

    return true;
}

void Practice13::selectFaces(graphics::EntityId id, std::initializer_list<std::size_t> faces) {
    auto& scene = application_.getScene();

    for (auto entity : scene.getIds()) {
        const auto* component = scene.getComponent(entity);

        for (std::size_t i = 0; i < component->polyhedron.faces.size(); ++i) {
            scene.setFaceVisible(entity, i, false);
        }
    }

    for (std::size_t face : faces) {
        scene.setFaceVisible(id, face, true);
    }
}

void Practice13::onKey(int key, int action, int) {
    if (action != GLFW_PRESS) {
        return; // 키 반복으로 무작위 선택을 여러 번 바꾸지 않는다.
    }

    if (key == GLFW_KEY_ESCAPE) {
        application_.requestClose();
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        selectFaces(cuboidId_, {static_cast<std::size_t>(key - GLFW_KEY_1)});
    } else if (key >= GLFW_KEY_7 && key <= GLFW_KEY_9) {
        selectFaces(pyramidId_, {static_cast<std::size_t>(key - GLFW_KEY_7)});
    } else if (key == GLFW_KEY_0) {
        selectFaces(pyramidId_, {3});
    } else if (key == GLFW_KEY_C) {
        const std::size_t first = std::uniform_int_distribution<int>(0, 5)(random_);
        std::size_t second = std::uniform_int_distribution<int>(0, 4)(random_);

        if (second >= first) {
            ++second; // 첫 면을 제외한 다섯 면 중 하나다.
        }
        selectFaces(cuboidId_, {first, second});
    } else if (key == GLFW_KEY_T) {
        const std::size_t side = std::uniform_int_distribution<int>(0, 3)(random_);

        selectFaces(pyramidId_, {4, side});
    }
}
}

std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app) {
    return std::make_unique<Practice13>(app);
}
````

**after** (없음)


## standard/modes/Mode11Practice14.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <iostream>

namespace {
// 실습 14: 도형 선택, 연속 입력, 위치와 로컬 회전행렬을 관리한다.
class Practice14 final : public PracticeMode {
private:
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0.0f};
    glm::mat3 rotation_{1.0f};
    glm::vec2 angularDirection_{0.0f};

    void selectShape(bool pyramid);
    void resetMotion();
    void applyTransform();

public:
    explicit Practice14(graphics::Application& application) : PracticeMode(application), application_(application) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float deltaTime) override;
    void onFocus(bool focused) override;
};

using practice_common::assignVertexColors;

namespace {

// 부호 조합은 육면체 8개 꼭짓점을 구분한다. 사각뿔은 바닥 4개와 꼭짓점 색이 다르다.


} // namespace

bool Practice14::initScene() {
    auto& scene = application_.getScene();

    scene.clear();

    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(0.8f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(0.8f));

    assignVertexColors(cube);

    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);

    if (!cubeId || !pyramidId) {
        return false;
    }
    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;

    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();
    const auto inverseView = glm::transpose(glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0})));

    camera.position = inverseView * glm::vec3(0, 0, 4);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetMotion();
    selectShape(false);
    std::cout << "실습 14 명령어: C== cube, P== pyramid, H== depth test\n"
              << "w== wireframe / W== solid (Shift or Caps Lock)\n"
              << "x/X: local X +/-, y/Y: local Y +/-\n"
              << "이동: world X/Y translation, S: reset motion, Esc: quit\n";

    return true;
}

void Practice14::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;

    auto& scene = application_.getScene();

    for (auto id : scene.getIds()) {
        const bool visible = id == (pyramidSelected_ ? pyramidId_ : cuboidId_);

        for (std::size_t i = 0; i < scene.getComponent(id)->polyhedron.faces.size(); ++i) {
            scene.setFaceVisible(id, i, visible);
        }
    }
    applyTransform();
}

void Practice14::resetMotion() {
    position_ = glm::vec3(0.0f);

    rotation_ = glm::mat3(1);
    angularDirection_ = glm::vec2(0.0f);
    pressed_.fill(false);
    applyTransform();
}

void Practice14::applyTransform() {
    graphics::Transform3D transform;

    transform.basis = position_;
    transform.rotation = rotation_;
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void Practice14::onKey(int key, int action, int mods) {
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            pressed_[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressed_[key] = false;
        }
    }

    if (action != GLFW_PRESS) {
        return;
    }

    const bool uppercase = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    auto& options = application_.getRenderOptions();

    switch (key) {
    case GLFW_KEY_ESCAPE:
        application_.requestClose();
        break;
    case GLFW_KEY_C:
        selectShape(false);
        break;
    case GLFW_KEY_P:
        selectShape(true);
        break;
    case GLFW_KEY_H:
        options.depthTest = !options.depthTest;
        break;
    case GLFW_KEY_W:
        options.wireframe = !uppercase;
        break;
    case GLFW_KEY_X:
        angularDirection_.x = uppercase ? -1.0f : 1.0f;
        break;
    case GLFW_KEY_Y:
        angularDirection_.y = uppercase ? -1.0f : 1.0f;
        break;
    case GLFW_KEY_S:
        resetMotion();
        break;
    default:
        break;
    }
}

void Practice14::onUpdate(float deltaTime) {
    const glm::vec2 movement{static_cast<float>(pressed_[GLFW_KEY_RIGHT]) - static_cast<float>(pressed_[GLFW_KEY_LEFT]),
                             static_cast<float>(pressed_[GLFW_KEY_UP]) - static_cast<float>(pressed_[GLFW_KEY_DOWN])};

    if (glm::dot(movement, movement) > 0.0f) {
        position_ += glm::vec3(glm::normalize(movement) * (0.5f * deltaTime), 0.0f);
    }
    // 오른쪽 곱은 현재 객체의 로컬 축 회전이다. 위치는 별도로 보존한다.
    const float angle = glm::radians(45.0f * deltaTime);

    rotation_ = glm::mat3(glm::rotate(glm::rotate(glm::mat4(rotation_), angle * angularDirection_.x, {1, 0, 0}),
                                      angle * angularDirection_.y, {0, 1, 0}));

    const auto x = glm::normalize(rotation_[0]);
    const auto z = glm::normalize(glm::cross(x, rotation_[1]));

    rotation_ = glm::mat3(x, glm::cross(z, x), z);
    applyTransform();
}

void Practice14::onFocus(bool focused) {
    if (!focused) {
        pressed_.fill(false); // 포커스 밖에서 놓친 release가 이동을 계속시키지 않는다.
    }
}
}

std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app) {
    return std::make_unique<Practice14>(app);
}
````

**after** (없음)


## standard/modes/Mode12Practice15.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "FaceCommon.h"

std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app) {
    return std::make_unique<FaceMode>(app);
}
````

**after** (없음)


## standard/modes/Mode13Practice16.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "FaceCommon.h"
#include <iostream>

using namespace face_common;
using practice_common::rotateAround;

namespace {
class Practice16 final : public FaceMode {
    enum class PyramidMotion { None, All, Sequence, Swap, Apex };
    PyramidMotion pyramidMotion_ = PyramidMotion::None;
    bool pyramidRunning_ = false;
    float pyramidProgress_ = 0, pyramidDirection_ = 1;
    void beginPyramid(PyramidMotion motion);
    void updatePyramid(float dt);
    void applyPyramid();
    void selectShape(bool pyramid) override {
        FaceMode::selectShape(pyramid);
        pyramidMotion_ = PyramidMotion::None;
        pyramidRunning_ = false;
        pyramidProgress_ = 0;
        pyramidDirection_ = 1;
    }
public:
    explicit Practice16(graphics::Application& app) : FaceMode(app) {}
    bool initScene() override;
    void onKey(int key, int action, int mods) override {
        if (action == GLFW_PRESS && key >= GLFW_KEY_6 && key <= GLFW_KEY_9) {
            const PyramidMotion modes[] = {PyramidMotion::All, PyramidMotion::Sequence,
                                           PyramidMotion::Swap, PyramidMotion::Apex};
            beginPyramid(modes[key - GLFW_KEY_6]);
            return;
        }
        FaceMode::onKey(key, action, mods);
    }
    void onUpdate(float dt) override {
        if (!pyramidSelected_) {
            FaceMode::onUpdate(dt);
            return;
        }
        if (spinning_) {
            spinDegrees_ = std::fmod(spinDegrees_ + ROTATION_SPEED * dt, 360.0f);
            applyObjectTransform();
        }
        updatePyramid(dt);
    }
};

bool Practice16::initScene() {
    auto& scene = application_.getScene();

    scene.clear();

    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(HALF_SIZE * 2.0f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(HALF_SIZE * 2.0f));
    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);

    if (!cubeId || !pyramidId) {
        return false;
    }

    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;

    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();
    const auto inverseView = glm::transpose(glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0})));

    camera.position = inverseView * glm::vec3(0, 0, 4);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.5f;
    resetAll();
    std::cout
        << "Practice 16 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
        << "Cube only 모드: T == top Z rotation, F == front open/close, S == left/right face normal rotation\n"
        << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
        << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
        << "All face motions can overlap with object spin(중복 모션 가능).\n"
        << "T/S/1/2/4/5 repeat to pause/resume(한번 더 누르면 정지). F/B repeat to reverse. 2 swaps and returns.\n";
    std::cout << "Pyramid 6: meet below, 7: sequential unfolding, 8: opposite swap, 9: apex opening\n";

    return true;
}


void Practice16::beginPyramid(PyramidMotion motion) {
    if (!pyramidSelected_)
        selectShape(true);

    if (pyramidMotion_ != motion) {
        pyramidMotion_ = motion;
        pyramidProgress_ = 0;
        pyramidDirection_ = 1;
        pyramidRunning_ = true;

    } else if (motion == PyramidMotion::All || motion == PyramidMotion::Apex) {
        pyramidDirection_ = -pyramidDirection_;
        pyramidRunning_ = true;
    } else if (motion == PyramidMotion::Swap && !pyramidRunning_ && (pyramidProgress_ == 0 || pyramidProgress_ == 1)) {
        pyramidDirection_ = -pyramidDirection_;
        pyramidRunning_ = true;
    } else
        pyramidRunning_ = !pyramidRunning_;

    applyPyramid();
}


void Practice16::updatePyramid(float dt) {
    if (pyramidRunning_) {
        if (pyramidMotion_ == PyramidMotion::Sequence)
            pyramidProgress_ = std::fmod(pyramidProgress_ + dt, 8.0f);
        else {
            pyramidProgress_ = std::clamp(pyramidProgress_ + pyramidDirection_ * dt * .5f, 0.0f, 1.0f);

            if (pyramidProgress_ == 0 || pyramidProgress_ == 1)
                pyramidRunning_ = false;
        }
    }
    applyPyramid();
}


void Practice16::applyPyramid() {
    auto& scene = application_.getScene();
    const float b = HALF_SIZE, h = 2 * HALF_SIZE;

    const float inward = glm::degrees(std::atan2(h, b));

    const std::array<glm::vec3, 4> hinges{glm::vec3{0, -b, b}, {b, -b, 0}, {0, -b, -b}, {-b, -b, 0}};
    const std::array<glm::vec3, 4> axes{X_AXIS, -Z_AXIS, -X_AXIS, Z_AXIS};

    const auto active = std::size_t(pyramidProgress_ / 2.0f) % 4;
    const float phase = std::fmod(pyramidProgress_, 2.0f);

    for (std::size_t i = 0; i < 4; ++i) {
        glm::mat4 matrix(1);

        if (pyramidMotion_ == PyramidMotion::All)
            matrix = rotateAround(hinges[i], axes[i], (360.0f - 2 * inward) * pyramidProgress_);

        else if (pyramidMotion_ == PyramidMotion::Sequence && i == active)
            matrix = rotateAround(hinges[i], axes[i], (180 - inward) * (phase <= 1 ? phase : 2 - phase));

        else if (pyramidMotion_ == PyramidMotion::Swap && (i == 0 || i == 2))
            matrix = rotateAround({0, b, 0}, Y_AXIS, (i == 0 ? 180.0f : -180.0f) * pyramidProgress_);

        else if (pyramidMotion_ == PyramidMotion::Apex)
            matrix = rotateAround({0, b, 0}, axes[i], -45.0f * pyramidProgress_);

        scene.setFaceLocalMatrix(pyramidId_, i, matrix);
    }
    scene.setFaceLocalMatrix(pyramidId_, 4, glm::mat4(1));
}

}

std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app) {
    return std::make_unique<Practice16>(app);
}
````

**after** (없음)


## standard/modes/Mode14Practice17.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
class Practice17 final : public PracticeMode {
    struct Object {
        graphics::EntityId orbit = 0, body = 0;
        glm::vec3 position{0};
        glm::mat3 rotation{1};
        glm::vec2 spin{0};
        float orbitDegrees = 0, orbitDirection = 0, scale = .65f;
        int shape = 0;
    };
    enum class Motion { None, Straight, Bypass, Combined };

    graphics::Application& app_;
    std::array<Object, 2> objects_{};
    int selected_ = 3;
    glm::vec2 movement_{0};
    Motion motion_ = Motion::None;
    bool running_ = false;
    float elapsed_ = 0;
    std::array<glm::vec3, 2> starts_{};
    std::array<float, 2> scales_{};
    std::array<glm::vec3, 2> motionOffsets_{glm::vec3(0), glm::vec3(0)};
    /// 배치와 조작 상태를 초기화한다.
    void reset();
    /// 계산한 변환을 장면에 반영한다.
    void apply();
    /// 선택한 이동을 시작한다.
    void begin(Motion motion);
    /// 선택한 도형의 모델을 만든다.
    graphics::Model model(int shape) const;
    /// 공전 회전 행렬을 계산한다.
    glm::mat3 orbitRotation(std::size_t i) const;
    /// 물체의 월드 위치를 계산한다.
    glm::vec3 worldPosition(std::size_t i) const;

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Practice17(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene() override;
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods) override;
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt) override;
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused) override;
};

using practice_common::turn;

namespace {
/// 축과 각도로 회전 행렬을 만든다.

}

/// 선택한 도형의 모델을 만든다.

graphics::Model Practice17::model(int shape) const {
    if (shape == 0) {
        auto cone = graphics::DiagramSystem::makeCone(.5f, 1.0f);

        for (auto& face : cone.faces)
            face.outline = graphics::OutlineMode::AllTriangleEdges;

        return cone;
    }

    if (shape == 1)
        return graphics::DiagramSystem::makeSphere(.5f);

    return graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube);
}

glm::mat3 Practice17::orbitRotation(std::size_t i) const {
    return turn(objects_[i].orbitDegrees, {0, 1, 0});
}

glm::vec3 Practice17::worldPosition(std::size_t i) const {
    return orbitRotation(i) * objects_[i].position;
}

bool Practice17::initScene() {
    reset();
    std::cout << "Practice 17: 1 left, 2 right, 3 both; x/X,y/Y spin; r/R orbit\n"
              << "a/A local scale, b/B origin scale, d/D left/right, e/E up/down\n"
              << "t straight swap, u bypass swap, v scale/spin/orbit; c shape, s reset, q quit\n";

    return true;
}

void Practice17::reset() {
    auto& scene = app_.getScene();

    scene.clear();
    objects_ = {};
    movement_ = glm::vec2(0);
    motionOffsets_.fill(glm::vec3(0));
    selected_ = 3;
    motion_ = Motion::None;
    running_ = false;
    elapsed_ = 0;

    for (std::size_t i = 0; i < 2; ++i) {
        auto& o = objects_[i];

        o.position = {i == 0 ? -1.0f : 1.0f, 0, 0};
        o.shape = i == 0 ? 0 : 2;
        o.orbit = scene.addGroup().value();
        o.body = scene.addModel(model(o.shape), graphics::Transform3D{}, graphics::PivotMode::Preserve).value();

        if (!scene.setParent(o.body, o.orbit) ||
            !scene.setTint(o.body, i == 0 ? graphics::GlColor{1, .65f, .3f} : graphics::GlColor{.4f, .8f, 1}))
            throw std::runtime_error("Cannot initialize object hierarchy.");
    }
    scene.addLine({-2, 0, 0}, {2, 0, 0}, {1, .2f, .2f});
    scene.addLine({0, -2, 0}, {0, 2, 0}, {.2f, 1, .2f});
    scene.addLine({0, 0, -2}, {0, 0, 2}, {.2f, .4f, 1});

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};

    const auto inverseView = glm::transpose(turn(30, {1, 0, 0}) * turn(30, {0, 1, 0}));

    camera.position = inverseView * glm::vec3(0, 0, 6);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 2.4f;
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().wireframe = true;
    apply();
}

void Practice17::apply() {
    auto& scene = app_.getScene();

    for (std::size_t i = 0; i < 2; ++i) {
        const auto& o = objects_[i];
        graphics::Transform3D orbit, body;

        orbit.rotation = orbitRotation(i);
        body.basis = o.position;
        body.rotation = o.rotation;

        if (o.shape == 0)
            body.rotation *= turn(-45.0f, {0, 0, 1});
        body.scale = glm::vec3(o.scale);

        if (!scene.setTransform(o.orbit, orbit) || !scene.setTransform(o.body, body))
            throw std::runtime_error("Invalid model transform.");
    }
}

void Practice17::begin(Motion motion) {
    if (motion_ == motion) {
        running_ = !running_;

        return;
    }
    motion_ = motion;
    motionOffsets_.fill(glm::vec3(0));
    running_ = true;
    elapsed_ = 0;

    for (std::size_t i = 0; i < 2; ++i) {
        starts_[i] = worldPosition(i);
        scales_[i] = objects_[i].scale;
    }
}

void Practice17::onKey(int key, int action, int mods) {
    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    const float sign = upper ? -1.0f : 1.0f;

    if (key == GLFW_KEY_D || key == GLFW_KEY_E) {
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            float& direction = key == GLFW_KEY_D ? movement_.x : movement_.y;

            direction = action == GLFW_RELEASE ? 0 : (key == GLFW_KEY_D ? -sign : sign);
        }

        return;
    }

    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
        app_.requestClose();

        return;
    }

    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3) {
        selected_ = key - GLFW_KEY_0;

        return;
    }

    if (key == GLFW_KEY_S) {
        reset();

        return;
    }

    if (key == GLFW_KEY_T) {
        begin(Motion::Straight);

        return;
    }

    if (key == GLFW_KEY_U) {
        begin(Motion::Bypass);

        return;
    }

    if (key == GLFW_KEY_V) {
        begin(Motion::Combined);

        return;
    }

    if (key == GLFW_KEY_C) {
        for (auto& o : objects_) {
            o.shape = (o.shape + 1) % 3;

            if (!app_.getScene().setModel(o.body, model(o.shape)))
                throw std::runtime_error("Cannot replace model.");
        }
        apply();

        return;
    }

    for (std::size_t i = 0; i < 2; ++i) {
        if (selected_ != 3 && selected_ != int(i + 1))
            continue;

        auto& o = objects_[i];
        const auto previousPosition = worldPosition(i);
        const auto previousScale = o.scale;

        switch (key) {
        case GLFW_KEY_X:
            o.spin.x = o.spin.x == sign ? 0 : sign;
            break;
        case GLFW_KEY_Y:
            o.spin.y = o.spin.y == sign ? 0 : sign;
            break;
        case GLFW_KEY_R:
            o.orbitDirection = o.orbitDirection == sign ? 0 : sign;
            break;
        case GLFW_KEY_A:
            o.scale = std::clamp(o.scale * (upper ? 1 / 1.1f : 1.1f), .15f, 3.0f);
            break;
        case GLFW_KEY_B: {
            const float old = o.scale;

            o.scale = std::clamp(old * (upper ? 1 / 1.1f : 1.1f), .15f, 3.0f);
            o.position *= o.scale / old;
            break;
        }
        default:
            break;
        }
        if (motion_ == Motion::Straight || motion_ == Motion::Bypass)
            motionOffsets_[i] += worldPosition(i) - previousPosition;
        if (motion_ == Motion::Combined)
            scales_[i] *= o.scale / previousScale;
    }

    apply();
}

void Practice17::onUpdate(float dt) {
    if (running_)
        elapsed_ += dt;

    for (std::size_t i = 0; i < 2; ++i) {
        auto& o = objects_[i];

        if (selected_ == 3 || selected_ == int(i + 1)) {
            const auto delta = glm::vec3(movement_ * .7f * dt, 0);
            o.position += glm::transpose(orbitRotation(i)) * delta;
            if (motion_ == Motion::Straight || motion_ == Motion::Bypass)
                motionOffsets_[i] += delta;
        }

        const bool combined = motion_ == Motion::Combined;
        const auto spin = o.spin + (combined && running_ ? glm::vec2(0, 1) : glm::vec2(0));

        const float orbit = o.orbitDirection + (combined && running_ ? 1.0f : 0.0f);

        o.rotation = o.rotation * turn(45 * dt * spin.x, {1, 0, 0}) * turn(45 * dt * spin.y, {0, 1, 0});

        const auto x = glm::normalize(o.rotation[0]), z = glm::normalize(glm::cross(x, o.rotation[1]));

        o.rotation = glm::mat3(x, glm::cross(z, x), z);
        o.orbitDegrees = std::fmod(o.orbitDegrees + 30 * dt * orbit, 360.0f);

        if (running_ && combined)
            o.scale = scales_[i] * (1 + (i == 0 ? .35f : -.35f) * std::sin(elapsed_));
        else if (running_ && (motion_ == Motion::Straight || motion_ == Motion::Bypass)) {
            const float t = std::min(elapsed_ / 2.0f, 1.0f);
            auto point = glm::mix(starts_[i], starts_[1 - i], t) + motionOffsets_[i];

            if (motion_ == Motion::Bypass)
                point.y += (i == 0 ? 1.0f : -1.0f) * .9f * std::sin(3.14159265f * t);
            o.position = glm::transpose(orbitRotation(i)) * point;
        }
    }

    if (running_ && (motion_ == Motion::Straight || motion_ == Motion::Bypass) && elapsed_ >= 2) {
        motion_ = Motion::None;
        running_ = false;
    }
    apply();
}

void Practice17::onFocus(bool focused) {
    if (!focused)
        movement_ = glm::vec2(0);
}
}

std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app) {
    return std::make_unique<Practice17>(app);
}
````

**after** (없음)


## standard/modes/Mode15Practice18.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
class Practice18 final : public PracticeMode {
    graphics::Application& app_;
    graphics::EntityId root_ = 0, sun_ = 0;
    std::array<graphics::EntityId, 3> tracks_{}, planets_{}, moonFrames_{}, moonTracks_{}, moons_{};
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0};
    std::array<float, 3> planetAngle_{0, 120, 240}, moonAngle_{0, 120, 240};
    float radius_ = 1, targetRadius_ = 1, sunTime_ = 0, moonTilt_ = 0, moonSpin_ = 0;
    bool pulse_ = false, rotateMoons_ = false;
    /// 계산한 변환을 장면에 반영한다.
    void apply();

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Practice18(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene() override;
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods) override;
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt) override;
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused) override;
};

using practice_common::turn;

namespace {
/// 축과 각도로 회전 행렬을 만든다.


constexpr std::array<float, 3> radii{2.0f, 2.8f, 3.6f};
constexpr std::array<float, 3> inclinations{0, 45, -45};

/// 부모에 연결한 원 궤도를 그린다.
void circle(graphics::Scene& scene, graphics::EntityId owner, graphics::GlColor color, bool xz) {
    constexpr int segments = 96;

    for (int i = 0; i < segments; ++i) {
        const float a = 6.283185307f * i / segments, b = 6.283185307f * (i + 1) / segments;
        const auto point = [&](float angle) {
            return xz ? glm::vec3(std::cos(angle), 0, std::sin(angle)) : glm::vec3(std::cos(angle), std::sin(angle), 0);
        };

        if (!scene.addLine(point(a), point(b), color, owner))
            throw std::runtime_error("Cannot add orbit line.");
    }
}
}

/// 장면 객체와 초기 상태를 구성한다.

bool Practice18::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    root_ = scene.addGroup().value();
    sun_ = scene.addSphere(.55f, {0, 0, 0}).value();

    if (!scene.setParent(sun_, root_))
        return false;

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {0, 5, 8};
    camera.target = {0, 0, 0};
    camera.orthographic = true;
    camera.orthographicHalfExtent = 5.2f;

    const std::array<graphics::GlColor, 3> colors{graphics::GlColor{1, .25f, .2f}, {.2f, .65f, 1}, {.3f, 1, .4f}};
    const std::array<graphics::GlColor, 3> moonColors{graphics::GlColor{.8f, .3f, 1}, {1, .5f, .15f}, {.4f, 1, .85f}};

    for (std::size_t i = 0; i < 3; ++i) {
        tracks_[i] = scene.addGroup().value();
        planets_[i] = scene.addGroup().value();
        moonFrames_[i] = scene.addGroup().value();
        moonTracks_[i] = scene.addGroup().value();

        const auto planet = scene.addSphere(.24f + .06f * float(i), {0, 0, 0}).value();
        const float moonRadius = .09f + .015f * float(i);

        moons_[i] = scene.addSphere(moonRadius, {0, 0, 0}).value();

        if (!scene.setParent(tracks_[i], root_) || !scene.setParent(planets_[i], root_) ||
            !scene.setParent(planet, planets_[i]) || !scene.setParent(moonFrames_[i], planets_[i]) ||
            !scene.setParent(moonTracks_[i], moonFrames_[i]) || !scene.setParent(moons_[i], moonFrames_[i]) ||
            !scene.setTint(planet, colors[i]) || !scene.setTint(moons_[i], moonColors[i]))
            return false;
        circle(scene, tracks_[i], colors[i], true);
        circle(scene, moonTracks_[i], moonColors[i], true);

        if (!scene.addLine({moonRadius, 0, 0}, {2 * moonRadius, 0, 0}, {1, 1, .2f}, moons_[i]))
            return false;
    }
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().lighting = true;
    apply();
    std::cout << "Practice 18: p orthographic / P perspective; m solid / M wire\n"
              << "WASD move XY, +/- move Z; 1 orbit size; 2 sun pulse/color; 3 moon orbit rotation/spin; q quit\n";

    return true;
}

void Practice18::apply() {
    auto& scene = app_.getScene();

    if (!scene.setPosition(root_, position_))
        throw std::runtime_error("Invalid system position.");

    graphics::Transform3D sun;

    sun.scale = glm::vec3(1 + .18f * std::sin(sunTime_));

    if (!scene.setTransform(sun_, sun) ||
        !scene.setTint(sun_, {1, .65f + .2f * std::sin(sunTime_), .15f + .1f * std::cos(sunTime_)}))
        throw std::runtime_error("Invalid sun transform.");

    for (std::size_t i = 0; i < 3; ++i) {
        const auto tilt = turn(inclinations[i], {0, 0, 1});
        graphics::Transform3D track;

        track.rotation = tilt;
        track.scale = glm::vec3(radii[i] * radius_);

        const auto planet = tilt * turn(planetAngle_[i], {0, 1, 0}) * glm::vec3(radii[i] * radius_, 0, 0);
        graphics::Transform3D frame;

        const auto& camera = app_.getCamera();
        const auto forward = glm::normalize(camera.target - camera.position);
        const auto right = glm::normalize(glm::cross(forward, camera.up));
        const auto screenUp = glm::cross(right, forward);
        frame.rotation = turn(moonTilt_, {0, 0, 1}) * glm::mat3(right, forward, screenUp);

        const float r = (.55f + .15f * float(i)) * radius_, a = glm::radians(moonAngle_[i]);
        graphics::Transform3D moonTrack;

        moonTrack.scale = glm::vec3(r);

        graphics::Transform3D moon;

        moon.basis = {r * std::cos(a), 0, r * std::sin(a)};
        moon.rotation = turn(moonSpin_, {0, 1, 0});

        if (!scene.setTransform(tracks_[i], track) || !scene.setPosition(planets_[i], planet) ||
            !scene.setTransform(moonFrames_[i], frame) || !scene.setTransform(moonTracks_[i], moonTrack) ||
            !scene.setTransform(moons_[i], moon))
            throw std::runtime_error("Invalid orbit transform.");
    }
}

void Practice18::onKey(int key, int action, int mods) {
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS)
            pressed_[key] = true;
        else if (action == GLFW_RELEASE)
            pressed_[key] = false;
    }

    if (action != GLFW_PRESS)
        return;

    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    switch (key) {
    case GLFW_KEY_P:
        app_.getCamera().orthographic = !upper;
        break;
    case GLFW_KEY_M:
        app_.getRenderOptions().wireframe = upper;
        break;
    case GLFW_KEY_1:
        targetRadius_ = targetRadius_ == 1 ? 1.3f : 1.0f;
        break;
    case GLFW_KEY_2:
        pulse_ = !pulse_;
        break;
    case GLFW_KEY_3:
        rotateMoons_ = !rotateMoons_;
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void Practice18::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_D]) - float(pressed_[GLFW_KEY_A]),
                       float(pressed_[GLFW_KEY_W]) - float(pressed_[GLFW_KEY_S]),
                       float(pressed_[GLFW_KEY_EQUAL] || pressed_[GLFW_KEY_KP_ADD]) -
                           float(pressed_[GLFW_KEY_MINUS] || pressed_[GLFW_KEY_KP_SUBTRACT])};

    if (glm::length(movement) > 0)
        position_ += glm::normalize(movement) * dt * 1.5f;
    radius_ += std::clamp(targetRadius_ - radius_, -.3f * dt, .3f * dt);

    if (pulse_)
        sunTime_ = std::fmod(sunTime_ + dt, 6.283185307f);

    if (rotateMoons_) {
        moonTilt_ = std::fmod(moonTilt_ + 25 * dt, 360.0f);
        moonSpin_ = std::fmod(moonSpin_ + 90 * dt, 360.0f);
    }

    for (std::size_t i = 0; i < 3; ++i) {
        planetAngle_[i] = std::fmod(planetAngle_[i] + (20 + 10 * float(i)) * dt, 360.0f);
        moonAngle_[i] = std::fmod(moonAngle_[i] + (70 + 30 * float(i)) * dt, 360.0f);
    }
    apply();
}

void Practice18::onFocus(bool focused) {
    if (!focused)
        pressed_.fill(false);
}
}

std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app) {
    return std::make_unique<Practice18>(app);
}
````

**after** (없음)


## standard/modes/Mode16Practice19.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "TankCommon.h"

std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app) {
    return makeTankMode(app, false);
}
````

**after** (없음)


## standard/modes/Mode17Practice20.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {
class Practice20 final : public PracticeMode {
    graphics::Application& app_;
    graphics::AnimationClock clock_;
    std::array<graphics::EntityId, 5> boxes_{};
    std::vector<graphics::EntityId> stripes_;
    bool leftHeld_ = false;
    bool rightHeld_ = false;

    void placeMovingObjects();
    void resetState();
    void zoom(float factor);

public:
    explicit Practice20(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onFocus(bool focused) override;
    void onUpdate(float dt) override;
};

using practice_common::require;

namespace {
constexpr float beltHeight = 1.0f;
constexpr float boxSize = .85f;
constexpr double beltSpeed = 1.25;
constexpr std::size_t stripeCount = 30;
constexpr float cornerRadius = .8f;
constexpr double halfPi = 1.57079632679489661923;
const glm::vec3 conveyorStart{-4, beltHeight, -2};
const glm::vec3 cornerPivot{2.2f, beltHeight, -1.2f};
constexpr float conveyorEndZ = 3.0f;
const double conveyorLength =
    (double(cornerPivot.x) - conveyorStart.x) + halfPi * double(cornerRadius) + (double(conveyorEndZ) - cornerPivot.z);


}

bool Practice20::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    stripes_.clear();

    require(
        graphics::PatternSystem::addPolygon(scene, {{-6, 0, -4}, {-6, 0, 5}, {6, 0, 5}, {6, 0, -4}}, {.25f, .30f, .34f})
            .has_value(),
        "Floor creation failed.");

    require(graphics::PatternSystem::addPolygon(scene,
                                                {{-4.8f, beltHeight, -2.8f},
                                                 {-4.8f, beltHeight, -1.2f},
                                                 {3.8f, beltHeight, -1.2f},
                                                 {3.8f, beltHeight, -2.8f}},
                                                {.09f, .12f, .15f})
                .has_value(),
            "Horizontal conveyor plane creation failed.");
    require(
        graphics::PatternSystem::addPolygon(
            scene,
            {{2.2f, beltHeight, -1.2f}, {2.2f, beltHeight, 3.8f}, {3.8f, beltHeight, 3.8f}, {3.8f, beltHeight, -1.2f}},
            {.09f, .12f, .15f})
            .has_value(),
        "Vertical conveyor plane creation failed.");

    for (std::size_t i = 0; i < stripeCount; ++i) {
        const auto stripe = graphics::PatternSystem::addBox(scene, {1.4f, .03f, .10f}, {0, 0, 0}, {.72f, .77f, .80f});

        require(stripe.has_value(), "Conveyor stripe creation failed.");
        stripes_.push_back(*stripe);
    }

    const std::array<graphics::GlColor, 5> colors{
        {{.95f, .30f, .23f}, {.23f, .72f, .96f}, {.98f, .75f, .20f}, {.38f, .86f, .43f}, {.78f, .44f, .96f}}};

    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const auto box = graphics::PatternSystem::addBox(scene, {boxSize, boxSize, boxSize}, {0, 0, 0}, colors[i]);

        require(box.has_value(), "Parcel creation failed.");
        boxes_[i] = *box;

        for (int face = 0; face < 4; ++face) {
            const auto label = graphics::PatternSystem::addJoint(scene, *box, {0, 0, 0});

            require(label.has_value(), "Parcel label frame creation failed.");
            require(graphics::PatternSystem::rotateJoint(scene, *label, {0, 1, 0}, float(face) * 90.0f),
                    "Parcel label frame rotation failed.");

            const int count = int(i + 1);
            const float z = boxSize * .5f + .008f;

            for (int mark = 0; mark < count; ++mark) {
                const float x = (float(mark) - float(count - 1) * .5f) * .14f;

                require(graphics::PatternSystem::addPolygon(
                            scene,
                            {{x - .05f, -.05f, z}, {x + .05f, -.05f, z}, {x + .05f, .05f, z}, {x - .05f, .05f, z}},
                            {0, 0, 0}, *label)
                            .has_value(),
                        "Parcel square mark creation failed.");
            }
        }
    }

    auto& options = app_.getRenderOptions();

    options = graphics::RenderOptions{};
    options.depthTest = true;
    options.lighting = true;
    options.background = {.08f, .10f, .14f, 1};
    resetState();
    std::cout << "실습 20: 택배 물류 센터\n"
              << "b 이동 시작/정지 | +/- 확대/축소 | p 원근 / P 직각\n"
              << "좌/우 중심 Y축 공전 | r 전체 리셋 | q 종료\n";

    return true;
}

void Practice20::placeMovingObjects() {
    auto& scene = app_.getScene();
    const double travelled = clock_.seconds * beltSpeed;
    const auto place = [&](graphics::EntityId entity, double distance, float height) {
        const auto pose = graphics::PatternSystem::conveyorCornerPose(distance, conveyorStart, cornerPivot,
                                                                      cornerRadius, conveyorEndZ);

        require(pose.has_value(), "Conveyor corner pose sampling failed.");

        const auto* component = scene.getComponent(entity);

        require(component != nullptr, "Conveyor moving entity is missing.");

        auto transform = component->transform;

        transform.basis = pose->position + glm::vec3(0, height, 0);
        transform.rotation = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(pose->yawDegrees), glm::vec3(0, 1, 0)));
        require(scene.setTransform(entity, transform), "Conveyor moving entity transform failed.");
    };

    for (std::size_t i = 0; i < stripes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(stripes_.size());

        place(stripes_[i], travelled + offset, .015f);
    }

    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(boxes_.size());

        place(boxes_[i], travelled + offset, .03f + boxSize * .5f);
    }
}

void Practice20::resetState() {
    clock_.reset();
    clock_.playing = false;
    leftHeld_ = false;
    rightHeld_ = false;

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {11, 10, 13};
    camera.target = {0, .7f, .3f};
    camera.up = {0, 1, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 7.5f;
    camera.nearPlane = .1f;
    camera.farPlane = 120;
    require(graphics::CameraSystem::validCamera(camera), "Reset camera is invalid.");
    placeMovingObjects();
}

void Practice20::zoom(float factor) {
    auto& camera = app_.getCamera();
    const float current =
        camera.orthographic ? camera.orthographicHalfExtent : glm::length(camera.position - camera.target);

    const float next = std::clamp(current * factor, 2.0f, camera.orthographic ? 35.0f : 60.0f);

    require(graphics::PatternSystem::zoomCamera(camera, next / current), "Camera zoom failed.");
}

void Practice20::onKey(int key, int action, int mods) {
    if (key == GLFW_KEY_LEFT)
        leftHeld_ = action != GLFW_RELEASE;

    if (key == GLFW_KEY_RIGHT)
        rightHeld_ = action != GLFW_RELEASE;

    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
        zoom(.9f);

    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
        zoom(1.0f / .9f);

    if (action != GLFW_PRESS)
        return;

    switch (key) {
    case GLFW_KEY_B:
        clock_.playing = !clock_.playing;
        break;
    case GLFW_KEY_P:

        app_.getCamera().orthographic = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
        break;
    case GLFW_KEY_R:
        resetState();
        break;
    case GLFW_KEY_Q:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void Practice20::onFocus(bool focused) {
    if (!focused) {
        leftHeld_ = false;
        rightHeld_ = false;
    }
}

void Practice20::onUpdate(float dt) {
    require(clock_.advance(dt), "Conveyor clock update failed.");
    placeMovingObjects();

    const int direction = int(rightHeld_) - int(leftHeld_);

    if (direction != 0) {
        const graphics::AxisLine axis{app_.getCamera().target, {0, 1, 0}};

        require(graphics::PatternSystem::orbitCamera(app_.getCamera(), axis, float(direction) * 65.0f * dt),
                "Camera orbit failed.");
    }
}
}

std::unique_ptr<PracticeMode> makePractice20(graphics::Application& app) {
    return std::make_unique<Practice20>(app);
}
````

**after** (없음)


## standard/modes/Mode18Practice21.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
class Practice21 final : public PracticeMode {
    struct BodyObject {
        graphics::EntityId entity = 0;
        graphics::RoomBody body;
        bool elastic = false;
        glm::mat3 releasedRotation{1};
    };

    graphics::Application& app_;
    const graphics::Bounds3 roomBounds_{{-3, -2, -3.5f}, {3, 2.5f, 3.5f}};
    graphics::EntityId room_ = 0;
    std::vector<BodyObject> objects_;

    unsigned ballCount_ = 0;

    float roomAngle_ = 0;
    float roomSpinAngle_ = 0;
    float pendingRotation_ = 0;
    bool roomSpinning_ = false;
    bool floorOpen_ = false;
    bool dragging_ = false;
    bool focused_ = true;
    float lastDt_ = 1.0f / 60.0f;
    double previousMouseX_ = 0;

    glm::mat4 roomWorld() const;
    bool addBall();

    void releaseBody(BodyObject& object) const;

    bool syncScene();

    void require(bool succeeded, const char* operation) const;
    void printHelp() const;

public:
    explicit Practice21(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onMouse(const graphics::MouseEvent& event) override;
    void onCursor(double x, double y) override;
    void onFocus(bool focused) override;
    void onUpdate(float dt) override;
};


void Practice21::require(bool succeeded, const char* operation) const {
    if (!succeeded)
        throw std::runtime_error(operation);
}

glm::mat4 Practice21::roomWorld() const {
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));

    return glm::rotate(spin, roomAngle_, glm::vec3(0, 0, 1));
}

void Practice21::releaseBody(BodyObject& object) const {
    const auto matrix = roomWorld();

    object.body.worldPosition = object.body.position(matrix);
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));
    const auto angularVelocity = glm::vec3(0, roomSpinning_ ? glm::radians(30.0f) : 0, 0) +
        glm::mat3(spin) * glm::vec3(0, 0, pendingRotation_ / lastDt_);
    object.body.worldVelocity = glm::mat3(matrix) * object.body.localVelocity +
        glm::cross(angularVelocity, object.body.worldPosition);
    object.releasedRotation = glm::mat3(matrix);
    object.body.released = true;
}

bool Practice21::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    objects_.clear();
    ballCount_ = 0;
    roomAngle_ = roomSpinAngle_ = pendingRotation_ = 0;
    floorOpen_ = dragging_ = roomSpinning_ = false;

    const auto room = graphics::PatternSystem::addRoom(scene, roomBounds_, true);

    if (!room)
        return false;
    room_ = *room;

    const graphics::GlColor colors[] = {{.6f, .6f, .6f},   {.35f, .5f, .8f}, {.85f, .4f, .4f},
                                        {.35f, .75f, .5f}, {.75f, .5f, .8f}, {.8f, .7f, .4f}};

    for (std::size_t face = 0; face < 6; ++face) {
        if (!scene.setFaceVertexColors(room_, face, std::vector<graphics::GlColor>(4, colors[face])))
            return false;
    }

    const glm::vec3 sizes[] = {{.65f, .65f, .65f}, {.9f, .9f, .9f}, {1.2f, 1.2f, 1.2f}};
    const graphics::GlColor boxColors[] = {{1, .45f, .15f}, {.2f, .85f, .95f}, {.85f, .95f, .3f}};

    for (unsigned i = 0; i < 3; ++i) {
        BodyObject box;

        box.body.halfExtent = sizes[i] * .5f;
        box.body.localPosition = {0, roomBounds_.minimum.y + box.body.halfExtent.y, -2.3f + 2.3f * i};

        const auto entity = graphics::PatternSystem::addBox(scene, sizes[i], box.body.localPosition, boxColors[i]);

        if (!entity)
            return false;
        box.entity = *entity;
        objects_.push_back(box);
    }

    if (!addBall())
        return false;

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {0, .8f, 13};
    camera.target = {0, .25f, 0};
    camera.farPlane = 200;
    camera.orthographic = false;

    auto& render = app_.getRenderOptions();

    render.depthTest = true;
    render.cull = graphics::CullMode::Back;
    render.lighting = true;
    render.lightDirection = {.5f, 1, 2};
    printHelp();

    return syncScene();
}

bool Practice21::addBall() {
    if (ballCount_ >= 5) {
        std::cout << "공은 최대 5개입니다. R로 초기화할 수 있습니다.\n";

        return true;
    }

    const float radius = .24f;
    std::optional<glm::vec3> freePosition;

    for (int y = 0; y < 4 && !freePosition; ++y) {
        for (int z = 0; z < 5 && !freePosition; ++z) {
            for (int x = 0; x < 5 && !freePosition; ++x) {
                const glm::vec3 p(-2.2f + x * 1.1f, .1f + y * .5f, -2.6f + z * 1.3f);
                const graphics::Bounds3 candidate{p - glm::vec3(radius), p + glm::vec3(radius)};

                if (!roomBounds_.contains(p, glm::vec3(radius)))
                    continue;

                bool occupied = false;

                for (const auto& object : objects_) {
                    if (object.body.released)
                        continue;

                    const graphics::Bounds3 bodyBounds{object.body.localPosition - object.body.halfExtent,
                                                       object.body.localPosition + object.body.halfExtent};

                    if (candidate.overlaps(bodyBounds)) {
                        occupied = true;
                        break;
                    }
                }

                if (!occupied)
                    freePosition = p;
            }
        }
    }

    if (!freePosition) {
        std::cout << "공을 생성할 빈 위치가 없습니다. 잠시 후 B를 누르세요.\n";

        return true;
    }

    auto& scene = app_.getScene();
    const auto sphere = scene.addSphere(radius, *freePosition);

    if (!sphere || !scene.setTint(*sphere, {1, .85f - .12f * ballCount_, .15f + .15f * ballCount_}) ||
        !scene.setFaceCullMode(*sphere, 0, graphics::CullMode::None))
        return false;

    BodyObject ball;

    ball.entity = *sphere;
    ball.elastic = true;
    ball.body.localPosition = *freePosition;
    ball.body.halfExtent = glm::vec3(radius);
    ball.body.localVelocity = {1.5f + .2f * ballCount_, -1.4f - .12f * ballCount_, ballCount_ % 2 ? -1.8f : 1.8f};

    if (floorOpen_)
        releaseBody(ball);
    objects_.push_back(ball);
    ++ballCount_;
    std::cout << "공 생성: " << ballCount_ << "/5\n";

    return true;
}

bool Practice21::syncScene() {
    auto& scene = app_.getScene();
    const auto matrix = roomWorld();
    graphics::Transform3D roomTransform;

    roomTransform.rotation = glm::mat3(matrix);

    roomTransform.basis = glm::vec3(matrix * glm::vec4((roomBounds_.minimum + roomBounds_.maximum) * .5f, 1));

    if (!scene.setTransform(room_, roomTransform) || !scene.setFaceVisible(room_, 5, !floorOpen_))
        return false;

    for (const auto& object : objects_) {
        graphics::Transform3D transform;

        transform.basis = object.body.position(matrix);

        if (!object.elastic)
            transform.rotation = object.body.released ? object.releasedRotation : glm::mat3(matrix);

        if (!scene.setTransform(object.entity, transform))
            return false;
    }

    return true;
}

void Practice21::onKey(int key, int action, int mods) {
    if (!focused_ || (action != GLFW_PRESS && action != GLFW_REPEAT))
        return;

    const bool uppercase = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    if (key == GLFW_KEY_Z) {
        require(graphics::PatternSystem::moveCamera(app_.getCamera(), {0, 0, uppercase ? -.35f : .35f}),
                "카메라 Z 이동 실패");
    } else if (key == GLFW_KEY_Y) {
        require(graphics::PatternSystem::yawCamera(app_.getCamera(), uppercase ? 4.0f : -4.0f),
                "카메라 로컬 Y 자전 실패");
    } else if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_B) {
            require(addBall() && syncScene(), "공 추가 또는 표시변환 실패");
        } else if (key == GLFW_KEY_E || key == GLFW_KEY_F) {
            if (key == GLFW_KEY_F)
                floorOpen_ = !floorOpen_;

            if (floorOpen_ || key == GLFW_KEY_E) {
                for (auto& object : objects_)
                    if (!object.body.released)
                        releaseBody(object);
            }
            require(syncScene(), "바닥 가시성 변경 실패");
            std::cout << (key == GLFW_KEY_E ? "모든 내부 객체 이탈: 바닥 상태 유지\n"
                : floorOpen_ ? "바닥 열림: 모든 내부 객체 낙하\n" : "바닥 닫힘: 이탈한 객체는 계속 낙하\n");
        } else if (key == GLFW_KEY_V) {
            roomSpinning_ = !roomSpinning_;
            std::cout << (roomSpinning_ ? "방 Y축 자전 시작\n" : "방 Y축 자전 정지\n");
        } else if (key == GLFW_KEY_R) {
            require(initScene(), "장면 초기화 실패");
        } else if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
            app_.requestClose();
        } else if (key == GLFW_KEY_H) {
            printHelp();
        }
    }
}

void Practice21::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (event.action == GLFW_RELEASE) {
        dragging_ = false;

        return;
    }

    if (event.action == GLFW_PRESS && focused_) {
        dragging_ = true;
        previousMouseX_ = event.windowX;
    }
}

void Practice21::onCursor(double x, double) {
    if (!dragging_ || !focused_)
        return;

    const float delta = glm::radians(float((previousMouseX_ - x) * .3));

    previousMouseX_ = x;

    if (!std::isfinite(delta))
        return;

    roomAngle_ = std::remainder(roomAngle_ + delta, glm::radians(360.0f));
    pendingRotation_ += delta;
}

void Practice21::onFocus(bool focused) {
    focused_ = focused;
    dragging_ = false;
    pendingRotation_ = 0;
}

void Practice21::onUpdate(float dt) {
    if (!focused_)
        return;

    if (dt > 0)
        lastDt_ = dt;
    const float spinSpeed = roomSpinning_ ? glm::radians(30.0f) : 0;

    roomSpinAngle_ = std::remainder(roomSpinAngle_ + spinSpeed * dt, glm::radians(360.0f));

    const auto matrix = roomWorld();
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));
    const glm::vec3 angularVelocity =
        glm::vec3(0, spinSpeed, 0) + glm::mat3(spin) * glm::vec3(0, 0, dt > 0 ? pendingRotation_ / dt : 0);

    if (dt > 0)
        pendingRotation_ = 0;

    for (auto& object : objects_) {
        const bool wasReleased = object.body.released;

        require(graphics::CollisionSystem::stepRoomBody(object.body, roomBounds_, matrix, dt, floorOpen_,
                                                        object.elastic, {0, -9.8f, 0}, angularVelocity),
                "실내 반사/미끄럼/낙하 계산 실패");

        if (!wasReleased && object.body.released)
            object.releasedRotation = glm::mat3(matrix);
    }
    require(syncScene(), "장면 표시변환 실패");
}

void Practice21::printHelp() const {
    std::cout << "실습 21: 실내 공 반사, 기울어진 바닥의 상자, 바닥 개방\n"
              << "z/Z: 카메라 월드 Z +/-, y/Y: 화면 로컬 Y 시계/반시계 자전\n"
              << "Shift XOR Caps Lock으로 소문자/대문자 구분. 카메라 키는 반복 허용.\n"
              << "B: 공 추가(초기 1개, 최대 5개), E: 전체 이탈, F: 바닥 열기/닫기\n"
              << "바닥 개방 즉시 모든 공/상자 중력 낙하. 개방 중 새 공도 낙하.\n"
              << "v: 방 Y축 30도/초 자전 시작/정지, 내부 벽 Front 컬링 유지\n"
              << "왼쪽 버튼 좌우 드래그: 공간 Z축 회전, 기울기에 따라 상자 미끄럼\n"
              << "R: 전체 초기화, H: 도움말, Q 또는 Esc: 종료\n"
              << "B/e/F/v/R/H/Q는 누른 순간 한 번만 적용. 포커스 상실 시 드래그 해제.\n";
}
}

std::unique_ptr<PracticeMode> makePractice21(graphics::Application& app) {
    return std::make_unique<Practice21>(app);
}
````

**after** (없음)


## standard/modes/Mode19Practice22.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "TankCommon.h"

std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app) {
    return makeTankMode(app, true);
}
````

**after** (없음)


## standard/modes/Mode1Ellipse.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path1() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
    return path;
}
````

**after** (없음)


## standard/modes/Mode20Practice23.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace {
class Practice23 final : public PracticeMode {
    graphics::Application& app_;
    graphics::CharacterBody body_;
    graphics::JointRig rig_;
    graphics::EntityId stage_ = 0;
    std::vector<graphics::Bounds3> obstacles_;
    graphics::Bounds3 bounds_{{-5, 0, -5}, {5, 4, 5}};
    std::array<float, 4> doors_{};
    std::array<bool, 4> open_{};
    std::optional<std::size_t> activeWall_;

    unsigned randomSeed_ = std::random_device{}();

    std::mt19937 random_{randomSeed_};
    bool walking_ = true;

public:
    explicit Practice23(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float dt) override;
};

using practice_common::require;

namespace {

}

bool Practice23::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    obstacles_.clear();
    body_ = graphics::CharacterBody{};
    body_.feet = {-3, 0, -3};
    body_.speed = 1;
    body_.height = 1.8f;
    body_.halfWidth = {.5f, .5f};
    doors_.fill(0);
    open_.fill(false);
    activeWall_.reset();
    walking_ = true;

    const auto stage = graphics::PatternSystem::addRoom(scene, bounds_, false);

    if (!stage)
        return false;
    stage_ = *stage;

    for (std::size_t face = 0; face < 4; ++face) {
        if (!graphics::PatternSystem::openRoomWall(scene, stage_, face, 0))
            return false;
    }

    graphics::GridLayout grid;

    grid.origin = {-.5f, 0, -.3f};
    grid.extent = {1, 1.8f, .6f};

    const auto rig = graphics::PatternSystem::addWalkingRig(scene, grid);

    if (!rig)
        return false;
    rig_ = *rig;
    std::cout << "실습 23 난수 시드=" << randomSeed_ << "; 초기화 시 기존 난수 엔진의 다음 수열을 사용합니다.\n";

    std::vector<graphics::Bounds3> occupied{{{-3.8f, 0, -3.8f}, {-2.2f, 2, -2.2f}}};

    for (unsigned i = 0; i < 3; ++i) {
        const glm::vec3 size{.9f + .2f * i, .6f + .25f * i, .9f + .15f * i};
        const auto box =
            graphics::CollisionSystem::randomFreeBox({{-4, 0, -4}, {4, size.y, 4}}, size, occupied, random_);

        if (!box)
            return false;

        if (!graphics::PatternSystem::addBox(scene, size, (box->minimum + box->maximum) * .5f,
                                             {.3f + .2f * i, .5f, .8f - .2f * i}))
            return false;
        occupied.push_back(*box);
        obstacles_.push_back(*box);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 11};
    app_.getCamera().target = {0, 1, 0};
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().depthTest = true;
    app_.getRenderOptions().lighting = true;

    return graphics::PatternSystem::poseWalkingRig(scene, rig_, body_.feet, 0, 0, body_.speed);
}

void Practice23::onKey(int key, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    auto& camera = app_.getCamera();

    if (key == GLFW_KEY_X || key == GLFW_KEY_Z) {
        glm::vec3 delta(0);

        delta[key == GLFW_KEY_X ? 0 : 2] = upper ? -.35f : .35f;
        require(graphics::PatternSystem::moveCamera(camera, delta), "실습 23 카메라 이동 실패.");

        return;
    }

    if (key == GLFW_KEY_Y) {
        require(graphics::PatternSystem::orbitCamera(camera, {camera.target, {0, 1, 0}}, upper ? 4.0f : -4.0f),
                "실습 23 카메라 공전 실패.");

        return;
    }

    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
        body_.speed = std::min(3.0f, body_.speed + .1f);

    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
        body_.speed = std::max(0.0f, body_.speed - .1f);

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        body_.direction = {0, 0, 1};
        walking_ = true;
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        body_.direction = {0, 0, -1};
        walking_ = true;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        body_.direction = {-1, 0, 0};
        walking_ = true;
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        body_.direction = {1, 0, 0};
        walking_ = true;
    }

    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_O) {
        if (activeWall_)
            open_[*activeWall_] = !open_[*activeWall_];
        else {
            activeWall_ = graphics::PatternSystem::cameraFacingWall(app_.getScene(), stage_, camera);

            if (activeWall_)
                open_[*activeWall_] = true;
        }

        if (activeWall_)
            std::cout << "선택한 벽=" << *activeWall_ << (open_[*activeWall_] ? " 열기\n" : " 원상복귀\n");
    }

    if (key == GLFW_KEY_SPACE)
        walking_ = !walking_;

    if (key == GLFW_KEY_J)
        graphics::CollisionSystem::jump(body_, 5.5f);

    if (key == GLFW_KEY_I)
        require(initScene(), "실습 23 초기화 실패: 앞선 API 오류를 확인하세요.");

    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
        app_.requestClose();
}

void Practice23::onUpdate(float dt) {
    const float requested = body_.speed;

    if (!walking_)
        body_.speed = 0;

    const auto result = graphics::CollisionSystem::stepCharacter(body_, bounds_, obstacles_, dt);

    body_.speed = requested;
    require(result.has_value(), "실습 23 캐릭터 충돌 계산 실패.");

    const float yaw = glm::degrees(std::atan2(body_.direction.x, body_.direction.z));

    require(graphics::PatternSystem::poseWalkingRig(app_.getScene(), rig_, body_.feet, yaw, body_.walkDistance,
                                                    body_.speed),
            "실습 23 로봇 관절 자세 적용 실패.");

    for (std::size_t face = 0; face < doors_.size(); ++face) {
        doors_[face] = std::clamp(doors_[face] + (open_[face] ? 1.0f : -1.0f) * dt, 0.0f, 1.0f);
        require(graphics::PatternSystem::openRoomWall(app_.getScene(), stage_, face, doors_[face]),
                "실습 23 벽 개방·복원 실패.");
    }

    if (activeWall_ && !open_[*activeWall_] && doors_[*activeWall_] == 0)
        activeWall_.reset();
}
}

std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app) {
    return std::make_unique<Practice23>(app);
}
````

**after** (없음)


## standard/modes/Mode21CameraCollision.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"


namespace {
class CameraCollision final : public PracticeMode {
    graphics::Application& app_;
    std::array<bool, GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 minimum_{-.6f}, maximum_{.6f};
    bool blocked(const glm::vec3& position) const {
        const auto closest = glm::clamp(position,minimum_,maximum_);
        const auto difference = position-closest;
        return glm::dot(difference,difference) <= .15f*.15f;
    }
public:
    explicit CameraCollision(graphics::Application& app) : PracticeMode(app), app_(app) {}
    bool initScene() override {
        pressed_.fill(false);
        auto& scene = app_.getScene(); scene.clear();
        const auto object = scene.addCuboid({1.2f,1.2f,1.2f},{0,0,0});
        if (!object || !scene.setTransform(*object,graphics::Transform3D{})) return false;
        app_.getCamera() = graphics::Camera{};
        app_.getCamera().position = {0,0,3};
        app_.getCamera().target = {0,0,0};
        return true;
    }
    void onKey(int key, int action, int) override {
        if (key>=0 && key<=GLFW_KEY_LAST) pressed_[key] = action != GLFW_RELEASE;
        if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) app_.requestClose();
    }
    void onFocus(bool focused) override { if (!focused) pressed_.fill(false); }
    void onUpdate(float dt) override {
        if (!std::isfinite(dt) || dt<0) throw std::runtime_error("Invalid dt.");
        auto& camera = app_.getCamera();
        const auto forward = glm::normalize(camera.target-camera.position);
        const auto right = glm::normalize(glm::cross(forward,camera.up));
        glm::vec3 direction(0);
        if (pressed_[GLFW_KEY_W]) direction += forward;
        if (pressed_[GLFW_KEY_S]) direction -= forward;
        if (pressed_[GLFW_KEY_A]) direction -= right;
        if (pressed_[GLFW_KEY_D]) direction += right;
        if (glm::dot(direction,direction)==0) return;
        const auto movement = glm::normalize(direction)*2.0f*dt;
        const int steps = std::max(1,int(std::ceil(glm::length(movement)/(.15f*.5f))));
        const auto step = movement/float(steps);
        for (int i=0; i<steps; ++i) {
            if (blocked(camera.position+step)) break;
            camera.position += step;
            camera.target += step;
        }
    }
};
}

std::unique_ptr<PracticeMode> makeCameraCollision(graphics::Application& app) {
    return std::make_unique<CameraCollision>(app);
}
````

**after** (없음)


## standard/modes/Mode22EmbeddedMesh.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"


namespace {
struct EmbeddedCorner {
    std::size_t vertex = 0;
    std::optional<std::size_t> uv;
};
using EmbeddedTriangle = std::array<EmbeddedCorner,3>;
struct EmbeddedResult {
    graphics::Model model;
    std::vector<std::string> errors;
};

inline EmbeddedResult validateEmbedded(const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec2>& uvs,const std::vector<EmbeddedTriangle>& triangles) {
    EmbeddedResult result;
    result.model.shape = graphics::GLShape::Custom;
    const auto validPosition = [](const glm::vec3& p) {
        return std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z)
            && glm::all(glm::lessThanEqual(glm::abs(p),glm::vec3(1)));
    };
    const auto validUV = [](const glm::vec2& uv) {
        return std::isfinite(uv.x) && std::isfinite(uv.y)
            && glm::all(glm::greaterThanEqual(uv,glm::vec2(0)))
            && glm::all(glm::lessThanEqual(uv,glm::vec2(1)));
    };
    for (std::size_t i=0; i<positions.size(); ++i)
        if (!validPosition(positions[i])) result.errors.push_back("Invalid vertex "+std::to_string(i));
    for (std::size_t i=0; i<uvs.size(); ++i)
        if (!validUV(uvs[i])) result.errors.push_back("Invalid UV "+std::to_string(i));
    for (std::size_t face=0; face<triangles.size(); ++face) {
        const auto& corners = triangles[face];
        const bool textured = corners[0].uv.has_value();
        bool valid = true;
        std::array<glm::vec3,3> points{};
        std::array<glm::vec2,3> texcoords{};
        for (std::size_t i=0; i<3; ++i) {
            if (corners[i].vertex>=positions.size() || corners[i].uv.has_value()!=textured) { valid=false; break; }
            points[i] = positions[corners[i].vertex];
            if (!validPosition(points[i])) { valid=false; break; }
            if (textured) {
                if (*corners[i].uv>=uvs.size()) { valid=false; break; }
                texcoords[i] = uvs[*corners[i].uv];
                if (!validUV(texcoords[i])) { valid=false; break; }
            }
        }
        if (valid)
            for (std::size_t i=0; i<3; ++i)
                for (std::size_t j=i+1; j<3; ++j)
                    if (corners[i].vertex==corners[j].vertex || glm::all(glm::equal(points[i],points[j]))
                        || (textured && glm::all(glm::equal(texcoords[i],texcoords[j])))) valid=false;
        glm::dvec3 normal(0);
        if (valid) {
            auto a=glm::dvec3(points[1])-glm::dvec3(points[0]);
            auto b=glm::dvec3(points[2])-glm::dvec3(points[0]);
            const auto scaleA=std::max({std::abs(a.x),std::abs(a.y),std::abs(a.z)});
            const auto scaleB=std::max({std::abs(b.x),std::abs(b.y),std::abs(b.z)});
            if (scaleA==0 || scaleB==0) valid=false;
            else {
                a/=scaleA; b/=scaleB; normal=glm::cross(a,b);
                if (glm::dot(normal,normal)<=1e-12*glm::dot(a,a)*glm::dot(b,b)) valid=false;
            }
        }
        if (!valid) { result.errors.push_back("Invalid triangle "+std::to_string(face)); continue; }
        std::vector<graphics::Vertex> vertices(3);
        for (std::size_t i=0; i<3; ++i) {
            vertices[i].position=points[i];
            vertices[i].normal=glm::vec3(glm::normalize(normal));
            vertices[i].uv=texcoords[i];
            vertices[i].color={.8f,.6f,.2f};
        }
        result.model.faces.push_back(graphics::Mesh::fromVertices(vertices,{0,1,2}));
    }
    if (result.model.faces.empty()) result.errors.push_back("No valid triangles.");
    return result;
}

class EmbeddedMesh final : public PracticeMode {
    graphics::Application& app_;
public:
    explicit EmbeddedMesh(graphics::Application& app) : PracticeMode(app), app_(app) {}
    bool initScene() override {
        const std::vector<glm::vec3> positions{
            {-.5f,-.5f,.5f},{.5f,-.5f,.5f},{0,.5f,0},{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f}};
        const std::vector<glm::vec2> uvs{{0,0},{1,0},{.5f,1}};
        const auto face=[](std::size_t a,std::size_t b,std::size_t c) {
            return EmbeddedTriangle{EmbeddedCorner{a,0},EmbeddedCorner{b,1},EmbeddedCorner{c,2}};
        };
        const auto result=validateEmbedded(positions,uvs,
            {face(0,1,2),face(1,4,2),face(4,3,2),face(3,0,2),face(0,3,4),face(0,4,1)});
        for (const auto& error : result.errors) std::cerr << error << '\n';
        if (!result.errors.empty()) return false;
        auto& scene=app_.getScene(); scene.clear();
        const auto model=scene.addModel(result.model,graphics::Transform3D{},graphics::PivotMode::Preserve);
        app_.getCamera()=graphics::Camera{};
        app_.getCamera().position={2,1.5f,3};
        return model.has_value();
    }
    void onKey(int key, int action, int) override {
        if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) app_.requestClose();
    }
};
}

std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app) {
    return std::make_unique<EmbeddedMesh>(app);
}
````

**after** (없음)


## standard/modes/Mode2Rectangle.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path2() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
    return path;
}
````

**after** (없음)


## standard/modes/Mode3Polygon.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path3() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polygon(
        {{1, -.5f, 0}, {2.4f, -.4f, 0}, {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
    return path;
}
````

**after** (없음)


## standard/modes/Mode4Spatial.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path4() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = [center](double u) {
        const double angle = u * 6.283185307179586;
        return center + glm::vec3(float(.7 * std::sin(2 * angle)), float(.6 * std::sin(angle)),
                                  float(.3 * std::cos(angle)));
    };
    path.interpolation = graphics::PathSystem::smoothStep;
    return path;
}
````

**after** (없음)


## standard/modes/Mode5Circle.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path5() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    return path;
}
````

**after** (없음)


## standard/modes/Mode6Polyline.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path6() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
    path.playback = graphics::Playback::PingPong;
    return path;
}
````

**after** (없음)


## standard/modes/Mode7Grid.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "../Console.h"
#include "AssemblyParts.h"

/// 거리 기반 격자 예제를 구성한다.
bool Console::initGrid() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Grid;
    clock_.reset();
    gridBoxes_.clear();

    graphics::GridLayout grid;

    grid.origin = {-3, -.1f, -2};
    grid.extent = {6, .2f, 4};
    grid.cells = {6, 1, 4};

    const auto cells = assembly::addGridParts(scene, grid);
    for (std::size_t i = 0; i < cells.size(); ++i)
        if (!addControl(cells[i], "grid tile " + std::to_string(i + 1)))
            return false;

    if (!distancePath_.setPoints({{-2.5f, .35f, -1.5f}, {2.5f, .35f, -1.5f}, {2.5f, .35f, 1.5f}}))
        return false;

    for (unsigned i = 0; i < 5; ++i) {
        const auto frame = scene.addGroup();
        if (!frame)
            return false;
        const auto box = graphics::PatternSystem::addBox(scene, {.4f, .5f, .4f}, {0, 0, 0},
            {.25f + .14f * i, .7f - .1f * i, .3f}, *frame);

        if (!box || !graphics::PatternSystem::addNumber(scene, i + 1, {-.07f, -.1f, .205f}, .2f, {1, 1, 1}, *box))
            return false;
        if (!addControl(*box, "moving box " + std::to_string(i + 1)))
            return false;
        gridBoxes_.push_back(*frame);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {7, 6, 8};
    app_.getCamera().target = {0, 0, 0};
    app_.getCamera().orthographicHalfExtent = 4;

    selectedJoint_ = 24; // 처음에는 첫 이동 상자를 조작한다.
    return true;
}

bool Console::updateGrid() {
    auto& scene = app_.getScene();
    bool ok = true;
    for (std::size_t i = 0; i < gridBoxes_.size(); ++i)
        ok = graphics::PatternSystem::follow(scene, gridBoxes_[i], distancePath_,
            clock_.seconds + distancePath_.length() * i / gridBoxes_.size()) && ok;
    return ok;
}
````

**after** (없음)


## standard/modes/Mode8Room.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "../Console.h"

bool Console::initRoom() {
    auto& scene = app_.getScene();
    scene.clear();
    demo_ = Demo::Room;
    floorOpen_ = false;
    const auto frame = scene.addGroup(), bodyFrame = scene.addGroup();
    const auto room = graphics::PatternSystem::addRoom(scene, {{-2, -1, -2}, {2, 2, 2}});
    if (!frame || !bodyFrame || !room || !scene.setParent(*room, *frame))
        return false;
    const auto cube = graphics::PatternSystem::addBox(scene, {.5f, .5f, .5f}, {0, 0, 0}, {1, .7f, .2f}, *bodyFrame);
    if (!cube)
        return false;
    room_ = *room;
    roomCube_ = *cube;
    roomFrame_ = *frame;
    bodyFrame_ = *bodyFrame;
    roomControl_ = jointControls_.size();
    if (!addControl(roomFrame_, "room", ControlKind::RoomFrame))
        return false;
    bodyControl_ = jointControls_.size();
    if (!addControl(roomCube_, "physical box", ControlKind::RoomBody))
        return false;
    roomBody_ = graphics::RoomBody{};
    roomBody_.localPosition = {0, -.75f, 0};
    releasedRotation_ = glm::mat3(1);
    app_.getCamera().position = {5, 3, 8};
    app_.getCamera().target = {0, 0, 0};
    return true;
}

bool Console::updateRoom(float dt) {
    auto& scene = app_.getScene();
    if (!roomControl_ || !bodyControl_)
        return false;
    const auto& manual = animation_[jointControls_[*roomControl_].track].rest;
    const auto& box = animation_[jointControls_[*bodyControl_].track].rest;
    const float angle = .4f * float(std::sin(clock_.seconds));
    const auto automatic = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));
    const auto matrix = graphics::DiagramSystem::modelMatrix(manual) * automatic;
    graphics::Transform3D roomTransform;
    roomTransform.rotation = glm::mat3(automatic);
    roomTransform.basis = glm::vec3(automatic * glm::vec4(0, .5f, 0, 1));
    const glm::vec3 radius(.25f * box.scale.x);
    roomBody_.halfExtent = glm::abs(box.rotation[0]) * radius.x + glm::abs(box.rotation[1]) * radius.y +
                          glm::abs(box.rotation[2]) * radius.z;
    if (!roomBody_.released) {
        const auto low = glm::vec3(-2, -1, -2) + roomBody_.halfExtent;
        const auto high = glm::vec3(2, 2, 2) - roomBody_.halfExtent;
        roomBody_.localPosition.x = std::clamp(roomBody_.localPosition.x, low.x, high.x);
        roomBody_.localPosition.z = std::clamp(roomBody_.localPosition.z, low.z, high.z);
        roomBody_.localPosition.y = floorOpen_ ? std::min(roomBody_.localPosition.y, high.y)
                                             : std::clamp(roomBody_.localPosition.y, low.y, high.y);
        releasedRotation_ = glm::mat3(matrix);
    }
    const auto angularVelocity = manual.rotation * glm::vec3(0, 0, clock_.playing ? .4f * float(std::cos(clock_.seconds)) : 0);
    if (!scene.setTransform(room_, roomTransform) || !scene.setFaceVisible(room_, 5, !floorOpen_) ||
        !graphics::CollisionSystem::stepRoomBody(roomBody_, {{-2, -1, -2}, {2, 2, 2}}, matrix, dt, floorOpen_,
                                                  false, {0, -9.8f, 0}, angularVelocity))
        return false;
    graphics::Transform3D bodyTransform;
    bodyTransform.basis = roomBody_.position(matrix);
    bodyTransform.rotation = releasedRotation_;
    return scene.setTransform(bodyFrame_, bodyTransform);
}
````

**after** (없음)


## standard/modes/Mode9Animation.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "../Console.h"
#include <iostream>

/// 선형보간과 그네·사람의 부모·자식 애니메이션을 구성한다.
bool Console::initAnimation() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Animation;
    clock_.reset();
    animation_.clear();
    jointControls_.clear();
    selectedJoint_ = 0;
    selectedAxis_ = 0;

    if (!graphics::PatternSystem::addBox(scene, {8, .15f, 6}, {0, -.15f, 0}, {.25f, .3f, .35f}))
        return false;

    graphics::SwingConfig swingConfig;

    swingConfig.pivot = {1, 3, 0};
    swingConfig.pendulum.amplitudeDegrees = 25;
    swingConfig.pendulum.periodSeconds = 3;

    const auto swing = graphics::AnimationSystem::addSwingWithRider(scene, swingConfig);

    if (!swing)
        return false;
    animation_ = swing->animation;

    graphics::HumanoidConfig personConfig;

    personConfig.grid.frame = glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0));

    const auto person = graphics::AnimationSystem::addHumanoid(scene, personConfig);

    if (!person)
        return false;

    auto pose = graphics::HumanoidPose::seated();

    pose.hips = {glm::vec3(0), glm::vec3(0)};
    pose.knees = {glm::vec3(0), glm::vec3(0)};

    if (!graphics::AnimationSystem::poseHumanoid(scene, *person, pose))
        return false;

    for (std::size_t i = 0; i < 2; ++i) {
        graphics::PendulumMotion sway;

        sway.amplitudeDegrees = 30;
        sway.periodSeconds = 2;
        sway.phaseDegrees = i == 0 ? 0 : 180;

        graphics::Motion motion;

        if (!motion.append(graphics::Motion::pendulum(sway)))
            return false;

        const auto track = graphics::AnimationSystem::createJointAnimation(scene, person->shoulders[i], motion);

        if (!track)
            return false;
        animation_.push_back(*track);
    }

    const auto marker = scene.addGroup();

    if (!marker || !graphics::PatternSystem::addBox(scene, {.4f, .4f, .4f}, {0, 0, 0}, {1, .85f, .15f}, *marker))
        return false;

    graphics::LinearMotion move;

    move.from = {-2, .3f, 2};
    move.to = {2, .3f, 2};
    move.durationSeconds = 4;
    move.playback = graphics::Playback::PingPong;

    graphics::Motion motion;

    if (!motion.append(graphics::Motion::linear(move)))
        return false;

    const auto track = graphics::AnimationSystem::createJointAnimation(scene, *marker, motion);

    if (!track)
        return false;
    animation_.push_back(*track);

    const auto control = [&](graphics::EntityId id, const std::string& name, bool whole = false) {
        return addControl(id, name, whole ? ControlKind::Object : ControlKind::Joint);
    };

    const auto personControls = [&](const graphics::HumanoidRig& rig, const std::string& prefix) {
        if (!control(rig.root, prefix + " root", true) || !control(rig.pelvis, prefix + " pelvis") ||
            !control(rig.torso, prefix + " torso") || !control(rig.neck, prefix + " neck"))
            return false;

        for (std::size_t i = 0; i < 2; ++i) {
            const auto side = prefix + (i == 0 ? " left " : " right ");

            if (!control(rig.shoulders[i], side + "shoulder") || !control(rig.elbows[i], side + "elbow") ||
                !control(rig.hands[i], side + "hand") || !control(rig.hips[i], side + "hip") ||
                !control(rig.knees[i], side + "knee") || !control(rig.feet[i], side + "foot"))
                return false;
        }

        return true;
    };

    if (!control(swing->frame, "swing frame", true) || !control(swing->hinge, "swing hinge") ||
        !control(swing->seat, "swing seat") || !personControls(swing->person, "rider") ||
        !personControls(*person, "standing") || !control(*marker, "linear marker", true))
        return false;
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 10};
    app_.getCamera().target = {0, 1.4f, 0};
    std::cout << "9: 노란 상자는 4초 편도 선형보간, 그네는 3초 주기 진자, 좌석의 사람은 계층 변환을 따릅니다.\n";

    return graphics::AnimationSystem::poseHierarchy(scene, animation_, 0);
}

bool Console::updateAnimation() {
    return graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds);
}
````

**after** (없음)


## standard/modes/MotionCommon.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "../Console.h"
#include "Paths.h"
#include <iostream>

/// 공통 Motion을 모드별 경로 정의로 교체한다.
bool Console::choosePath(int kind) {
    using namespace standard_modes;
    using PathFactory = graphics::PathMotion (*)();
    const PathFactory paths[] = {path1, path2, path3, path4, path5, path6};
    if (kind < 1 || kind > 6 || !pathId_)
        return false;
    graphics::Motion motion;
    auto& motions = app_.getScene().getMotions();
    return motion.append(graphics::Motion::followPath(paths[kind - 1]())) &&
           motions.setMotion(*pathId_, motion) && motions.seekMotion(*pathId_, 0);
}

/// 장면 객체와 초기 상태를 구성한다.
bool Console::initMotion(int kind) {
    demo_ = Demo::Motion;

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

    if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) {
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

    if (!pathId_ || !spinId_ || !globalId_ || !choosePath(kind)) {
        return false;
    }

    if (!motions.setMotionEnabled(*globalId_, false) ||
        !motions.bindMotion(graphics::MotionSystem::rootLayer(), *globalId_)) {
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

    if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId_) || !motions.bindMotion(*mirroredPath, *pathId_) ||
        !motions.bindMotion(*normalSpinLayer_, *spinId_) || !motions.bindMotion(*mirroredSpin, *spinId_) ||
        !scene.bindEntity(*first, *normalSpinLayer_) || !scene.bindEntity(*second, *mirroredSpin)) {
        return false;
    }

    if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first) ||
        !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second) ||
        !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) {
        return false;
    }

    if (!addControl(*first, "original box") || !addControl(*second, "reflected box"))
        return false;

    auto& camera = app_.getCamera();

    camera.position = {5, 3, 8};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};

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
````

**after** (없음)


## standard/modes/Paths.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#pragma once
#include "../framework/Framework.h"

namespace standard_modes {
graphics::PathMotion path1();
graphics::PathMotion path2();
graphics::PathMotion path3();
graphics::PathMotion path4();
graphics::PathMotion path5();
graphics::PathMotion path6();
}
````

**after** (없음)


## standard/modes/PracticeCommon.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#pragma once
#include "../framework/Framework.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace practice_common {
inline void assignVertexColors(graphics::Polyhedron& shape) {
    const std::array<graphics::GlColor, 8> palette{graphics::GlColor{1.0f, 0.2f, 0.2f},
                                                   {0.2f, 1.0f, 0.2f},
                                                   {0.2f, 0.4f, 1.0f},
                                                   {1.0f, 1.0f, 0.2f},
                                                   {1.0f, 0.2f, 1.0f},
                                                   {0.2f, 1.0f, 1.0f},
                                                   {1.0f, 0.6f, 0.2f},
                                                   {0.7f, 0.3f, 1.0f}};

    for (auto& face : shape.faces) {
        for (const auto& vertex : face.localVertices) {
            const int index = (vertex.x > 0.0f ? 1 : 0) | (vertex.y > 0.0f ? 2 : 0) | (vertex.z > 0.0f ? 4 : 0);

            face.vertexColors.push_back(palette[index]);
        }
    }
}

inline glm::mat4 rotateAround(const glm::vec3& pivot, const glm::vec3& axis, float degrees) {
    return glm::translate(glm::mat4(1.0f), pivot) * glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axis) *
           glm::translate(glm::mat4(1.0f), -pivot);
}

inline glm::mat4 scaleAround(const glm::vec3& pivot, float scale) {
    return glm::translate(glm::mat4(1.0f), pivot) * glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
           glm::translate(glm::mat4(1.0f), -pivot);
}

inline glm::mat3 turn(float degrees, glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1), glm::radians(degrees), axis));
}

inline void require(bool success, const char* operation) {
    if (!success)
        throw std::runtime_error(operation);
}

inline graphics::EntityId requireEntity(std::optional<graphics::EntityId> id, const char* reason) {
    if (!id)
        throw std::runtime_error(reason);

    return *id;
}
} // namespace practice_common
````

**after** (없음)


## standard/modes/PracticeMode.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#pragma once
#include "../framework/Framework.h"
#include <memory>

// Each practice owns its state; Console alone owns callback registration and switching.
class PracticeMode {
    graphics::Application& host_;
public:
    explicit PracticeMode(graphics::Application& app) : host_(app) {}
    virtual ~PracticeMode() = default;
    virtual bool initScene() = 0;
    virtual void onKey(int, int, int) = 0;
    virtual void onUpdate(float) {}
    virtual void onMouse(const graphics::MouseEvent&) {}
    virtual void onCursor(double, double) {}
    virtual void onFocus(bool) {}
    virtual bool capturesNumericKeys() const { return false; }
    virtual bool onRender() { return host_.renderScene(); }
};

constexpr int lastStandardMode = 22;
std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice20(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice21(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app);
std::unique_ptr<PracticeMode> makeCameraCollision(graphics::Application& app);
std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app);

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app);
````

**after** (없음)


## standard/modes/PracticeRegistry.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app) {
    switch (mode) {
    case 0: return makeAssemblyMode(app);
    case 10: return makePractice13(app);
    case 11: return makePractice14(app);
    case 12: return makePractice15(app);
    case 13: return makePractice16(app);
    case 14: return makePractice17(app);
    case 15: return makePractice18(app);
    case 16: return makePractice19(app);
    case 17: return makePractice20(app);
    case 18: return makePractice21(app);
    case 19: return makePractice22(app);
    case 20: return makePractice23(app);
    case 21: return makeCameraCollision(app);
    case 22: return makeEmbeddedMesh(app);
    default: return {};
    }
}
````

**after** (없음)


## standard/modes/TankCommon.cpp

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#include "TankCommon.h"
#include "AssemblyParts.h"
#include "PracticeCommon.h"
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
class TankMode final : public PracticeMode {
    graphics::Application& app_;
    bool threeViews_;
    graphics::EntityId root_ = 0, central_ = 0;
    std::array<graphics::EntityId, 2> lower_{}, upper_{}, guns_{}, poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_ = 0, centralAngle_ = 0, gunAngle_ = 0, poleTime_ = 0;
    float swapProgress_ = 0, swapDirection_ = 1;
    float cameraX_ = 0, cameraZ_ = 0, cameraYaw_ = 0, cameraOrbit_ = 0;
    bool lowerRunning_ = false, centralRunning_ = false, gunRunning_ = false, poleRunning_ = false;
    bool swapRunning_ = false, swapStarted_ = false;

    unsigned colorVersion_ = 0;

    graphics::EntityId box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent);

    void reset();
    void apply();
    void recolor();

public:
    TankMode(graphics::Application& app, bool threeViews) : PracticeMode(app), app_(app), threeViews_(threeViews) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float dt) override;
    void onFocus(bool focused) override;

    bool onRender() override;
};

using practice_common::requireEntity;

namespace {

}

graphics::EntityId TankMode::box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent) {
    const auto id = requireEntity(graphics::PatternSystem::addBox(app_.getScene(), size, position, {1, 1, 1}, parent),
                                  "탱크 상자 생성 실패: 크기·위치·부모 ID를 확인하세요.");

    colored_.push_back(id);

    return id;
}

bool TankMode::initScene() {
    reset();
    std::cout << (threeViews_ ? "실습 22: 원근 / XZ 평면도 / XY 정면도\n" : "실습 19: 탱크와 부품·카메라 조작\n")
              << "방향키 이동; d 하부 높이; t 중앙 자전; l 상부 교환\n"
              << "g 포신 반대 회전; p 깃대 반대 기울기; x/X,z/Z 카메라 이동\n"
              << "y/Y 카메라 시선 회전; r/R 공전(재입력 정지); O/o 색 변경; c 초기화; q 종료\n";

    return true;
}

void TankMode::reset() {
    auto& scene = app_.getScene();

    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_ = glm::vec3(0);
    lowerTime_ = centralAngle_ = gunAngle_ = poleTime_ = swapProgress_ = 0;
    swapDirection_ = 1;
    cameraX_ = cameraZ_ = cameraYaw_ = cameraOrbit_ = 0;
    lowerRunning_ = centralRunning_ = gunRunning_ = poleRunning_ = swapRunning_ = swapStarted_ = false;
    colorVersion_ = 0;
    const auto ground = requireEntity(scene.addGroup(), "바닥 그룹 생성 실패.");
    box({12, .1f, 12}, {0, -.05f, 0}, ground);
    const auto rig = assembly::addTankParts(scene);
    root_ = rig.root; central_ = rig.central;
    lower_ = rig.lower; upper_ = rig.upper; guns_ = rig.guns; poles_ = rig.poles;
    colored_.insert(colored_.end(), rig.colored.begin(), rig.colored.end());

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {-5, 4.5f, 7};
    camera.target = {0, .7f, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 6.5f;
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().lighting = true;
    recolor();
    apply();
}

void TankMode::recolor() {
    const std::array<graphics::GlColor, 10> palette{graphics::GlColor{.3f, .35f, .4f},
                                                    {.25f, .6f, .3f},
                                                    {.55f, .35f, .2f},
                                                    {.2f, .55f, .75f},
                                                    {.7f, .75f, .2f},
                                                    {.9f, .4f, .15f},
                                                    {1, .8f, .2f},
                                                    {.4f, .7f, .8f},
                                                    {.7f, .25f, .6f},
                                                    {.6f, .9f, .4f}};

    for (std::size_t i = 0; i < colored_.size(); ++i)
        if (!app_.getScene().setTint(colored_[i], palette[(i + colorVersion_) % palette.size()]))
            throw std::runtime_error("탱크 부품 색상 적용 실패.");
}

void TankMode::apply() {
    auto& scene = app_.getScene();

    if (!scene.setPosition(root_, position_))
        throw std::runtime_error("탱크 위치 적용 실패.");

    const float factor = 1 + .25f * std::sin(lowerTime_), height = .6f * factor;

    if (!scene.setPosition(central_, {0, height, 0}) ||
        !graphics::PatternSystem::rotateJoint(scene, central_, {0, 1, 0}, centralAngle_))
        throw std::runtime_error("중앙 관절 변환 적용 실패.");

    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        graphics::Transform3D low;

        low.basis = {sign * .8f, height * .5f, 0};
        low.scale = {1, factor, 1};

        const float a = 3.14159265f * swapProgress_;
        const glm::vec3 upperPosition{sign * .65f * std::cos(a), .25f, sign * .65f * std::sin(a)};

        if (!scene.setTransform(lower_[i], low) || !scene.setPosition(upper_[i], upperPosition) ||
            !graphics::PatternSystem::rotateJoint(scene, guns_[i], {0, 1, 0}, sign * gunAngle_) ||
            !graphics::PatternSystem::rotateJoint(scene, poles_[i], {1, 0, 0}, sign * 60 * std::sin(poleTime_)))
            throw std::runtime_error("탱크 부품 변환 적용 실패.");
    }
}

void TankMode::onKey(int key, int action, int mods) {
    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    if (key == GLFW_KEY_X || key == GLFW_KEY_Z) {
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            float& direction = key == GLFW_KEY_X ? cameraX_ : cameraZ_;

            direction = action == GLFW_RELEASE ? 0 : (upper ? -1.0f : 1.0f);
        }

        return;
    }

    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS)
            pressed_[key] = true;
        else if (action == GLFW_RELEASE)
            pressed_[key] = false;
    }

    if (action != GLFW_PRESS)
        return;

    const float direction = upper ? 1.0f : -1.0f;

    switch (key) {
    case GLFW_KEY_D:
        lowerRunning_ = !lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_ = !centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_ = !gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_ = !poleRunning_;
        break;
    case GLFW_KEY_L:
        if (!swapStarted_) {
            swapStarted_ = true;
            swapRunning_ = true;
        } else if (!swapRunning_ && (swapProgress_ == 0 || swapProgress_ == 1)) {
            swapDirection_ = -swapDirection_;
            swapRunning_ = true;
        } else
            swapRunning_ = !swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_ = cameraYaw_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_ = cameraOrbit_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_O:
        colorVersion_ = (colorVersion_ + 1) % 10;
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

void TankMode::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT]) - float(pressed_[GLFW_KEY_LEFT]), 0,
                       float(pressed_[GLFW_KEY_DOWN]) - float(pressed_[GLFW_KEY_UP])};

    if (glm::length(movement) > 0)
        position_ += glm::normalize(movement) * dt * 1.3f;

    position_.x = std::clamp(position_.x, -3.5f, 3.5f);
    position_.z = std::clamp(position_.z, -3.5f, 3.5f);

    if (lowerRunning_)
        lowerTime_ = std::fmod(lowerTime_ + dt, 6.283185307f);

    if (centralRunning_)
        centralAngle_ = std::fmod(centralAngle_ + 45 * dt, 360.0f);

    if (gunRunning_)
        gunAngle_ = std::fmod(gunAngle_ + 60 * dt, -360.0f);

    if (poleRunning_)
        poleTime_ = std::fmod(poleTime_ + dt, 6.283185307f);

    if (swapRunning_) {
        swapProgress_ = std::clamp(swapProgress_ + swapDirection_ * dt * .5f, 0.0f, 1.0f);

        if (swapProgress_ == 0 || swapProgress_ == 1)
            swapRunning_ = false;
    }

    auto camera = app_.getCamera();
    const glm::vec3 delta{cameraX_ * 2 * dt, 0, cameraZ_ * 2 * dt};

    if (!graphics::PatternSystem::moveCamera(camera, delta) ||
        (cameraYaw_ != 0 && !graphics::PatternSystem::yawCamera(camera, cameraYaw_ * 35 * dt)) ||
        (cameraOrbit_ != 0 &&
         !graphics::PatternSystem::orbitCamera(camera, {{0, 0, 0}, {0, 1, 0}}, cameraOrbit_ * 25 * dt)))
        throw std::runtime_error("카메라 갱신 실패: 위치·시선·회전축을 확인하세요.");
    app_.getCamera() = camera;
    apply();
}

void TankMode::onFocus(bool focused) {
    if (!focused) {
        pressed_.fill(false);
        cameraX_ = cameraZ_ = 0;
    }
}

bool TankMode::onRender() {
    if (!threeViews_)
        return app_.renderScene();
    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    const auto passes =
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

    if (passes.size() != 3)
        throw std::runtime_error("세 카메라 뷰 생성 실패: 프레임버퍼 크기와 카메라를 확인하세요.");

    return app_.renderViews(passes);
}
}

std::unique_ptr<PracticeMode> makeTankMode(graphics::Application& app, bool threeViews) {
    return std::make_unique<TankMode>(app, threeViews);
}
````

**after** (없음)


## standard/modes/TankCommon.h

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````cpp
#pragma once
#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makeTankMode(graphics::Application& app, bool threeViews);
````

**after** (없음)


## standard/standard.vcxproj

### 블록 1 · before L48 / after L48

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````xml
    <ClCompile Include="modes\*.cpp" />
    <ClInclude Include="modes\*.h" />
````

**after**

````xml
    <ClCompile Include="modes\Assembly.cpp" />
    <ClCompile Include="modes\AssemblyCommands.cpp" />
    <ClCompile Include="modes\AssemblyConsole.cpp" />
    <ClCompile Include="modes\AssemblyDiagnostics.cpp" />
    <ClCompile Include="modes\AssemblyDynamics.cpp" />
    <ClCompile Include="modes\AssemblyExamples.cpp" />
    <ClCompile Include="modes\AssemblyExtensions.cpp" />
    <ClCompile Include="modes\AssemblyGuide.cpp" />
    <ClCompile Include="modes\AssemblyHierarchy.cpp" />
    <ClCompile Include="modes\AssemblyInput.cpp" />
    <ClCompile Include="modes\AssemblyMotion.cpp" />
    <ClCompile Include="modes\AssemblyObjects.cpp" />
    <ClCompile Include="modes\AssemblyParts.cpp" />
    <ClCompile Include="modes\AssemblyPaths.cpp" />
    <ClCompile Include="modes\AssemblyPersistence.cpp" />
    <ClCompile Include="modes\AssemblyRuntime.cpp" />
    <ClCompile Include="modes\AssemblySceneTools.cpp" />
    <ClCompile Include="modes\AssemblySurface.cpp" />
    <ClCompile Include="modes\AssemblyTracks.cpp" />
    <ClCompile Include="modes\AssemblyWorkspace.cpp" />
    <ClCompile Include="modes\Mode0Recipe.cpp" />
    <ClInclude Include="modes\Assembly*.h" />
    <None Include="docs\reference\modes\*.md" />
````

### 블록 2 · before L63 / after L84

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````xml
    <AssemblyDocument Include="docs\assembly\**\*.md;docs\assembly\**\*.html;docs\assembly\**\*.json" />
````

**after** (없음)


### 블록 3 · before L67 / after L87

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````xml
    <Copy SourceFiles="@(AssemblyDocument)" DestinationFiles="@(AssemblyDocument->'$(TargetDir)docs\assembly\%(RecursiveDir)%(Filename)%(Extension)')" SkipUnchangedFiles="true" />
````

**after** (없음)


## tools/Refresh-ApiCatalog.py

### 블록 1 · before L7 / after L7

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
import os
````

### 블록 2 · before L77 / after L78

위치: `def state_owner(a):`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
    if owner == 'DynamicsSystem':
        return '호출자가 소유한 지역 질점·강체 기준과 순수 힘 콜백'
````

### 블록 3 · before L203 / after L206

위치: `def main():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
        # The previous inventory predates the default culling argument. Keep both pick overload anchors.
        for previous_record in previous:
            if previous_record['owner']=='' and previous_record['name']=='pick' and 'inheritedCull' not in previous_record['signature']:
                previous_record['signature']=previous_record['signature'][:-1]+', CullMode inheritedCull = CullMode::None)'
````

### 블록 4 · before L205 / after L212

위치: `def main():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````python
        if set(old) != set(map(key, records)):
            raise RuntimeError('Declaration inventory changed; reconcile API anchors before authoring.')
````

**after**

````python
        if set(old) - set(map(key, records)):
            raise RuntimeError('Removed/changed declarations need explicit anchor migration; additions preserve old anchors.')
````

### 블록 5 · before L222 / after L229

위치: `def main():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
        next_id=max(ids.values(),default=-1)+1
        for a in records:
            if key(a) in ids: continue
            idx=next_id; next_id+=1; ids[key(a)]=idx
            qualified=(a['owner']+'::' if a['owner'] else '')+a['name']
            existing[idx]='<article class="api-entry" id="api-'+str(idx)+'"><h3>'+esc(qualified)+'</h3><p>'+esc(a['description'])+'</p>'+architecture.guide.code(a['signature'])+'</article>'
````

### 블록 6 · before L273 / after L286

위치: `def main():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````python
                inventory.append({**old[key(a)], **a,
````

**after**

````python
                inventory.append({**old.get(key(a), {}), **a,
````

### 블록 7 · before L290 / after L303

위치: `def main():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
        dump(directory + '/API_KO.json', records)
        korean='# 공개 API 한글 목록\n\n현재 헤더 선언에서 생성했다. 빌드·테스트·브라우저 확인은 사용자가 수행한다.\n\n'
        for a in records:
            qualified=(a['owner']+'::' if a['owner'] else '')+a['name']
            relative=os.path.relpath(ROOT/a['file'],ROOT/directory).replace('\\','/')
            korean+='### '+qualified+'\n\n```cpp\n'+a['signature']+';\n```\n\n'+a['description']+'\n\n반환·실패: '+a['result']+'\n\n[선언 '+str(a['line'])+'행]('+relative+')\n\n'
        write(directory + '/API_KO.md', korean)
````

## tools/Update-StandardDocs.py

### 블록 1 · before L33 / after L33

위치: `def link(path,label):`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````python
        def link(path,label=None):
            return '<a href="'+html.escape(os.path.relpath(path,page.parent).replace('\\','/'))+'">'+html.escape(label or path.name)+'</a>'
        block='<section id="current-assembly-api"><h2>2026-11-03 갱신: 면 프레임·기능 조립 API</h2><p>기존 본문과 별도로 최신 추가 API를 정리했다. 빌드·실행 확인은 사용자에게 맡긴다. '+link(STANDARD/'index.html','전체 문서 목차')+'</p>'
        block+='<h3>Scene::setFaceFrame</h3><p>입력: EntityId, 면 인덱스, 유효한 affine mat4. 성공은 true, 없는 메시/면 또는 유효하지 않은 행렬은 false다. 엔티티 기준 변환이나 추가 면 모션을 대체하지 않고 최종 면 변환의 transformFrame을 설정한다. 기하 정점을 바꾸지 않는다.</p>'
        text=(base/'detail/Scene.cpp').read_text(encoding='utf-8-sig')
        begin=text.index('bool Scene::setFaceFrame('); end=text.index('\n}',begin)+2
        block+=code(text[begin:end])+'<p>'+link(base/'detail/Scene.cpp',f'Scene.cpp:{text[:begin].count(chr(10))+1}')+'</p>'
        block+='<p>입력 → native 최종 faceWorldMatrix를 프레임으로 저장 → 작성자 면 로컬 자세와 합성 → Renderer·피킹·표면 관절이 같은 최종 면 변환을 읽는다. normal은 inverse-transpose, cull은 winding을 기준으로 처리한다.</p>'
````

**after**

````python
        def link(path,label):
            return '<a href="'+html.escape(os.path.relpath(path,page.parent).replace('\\','/'))+'">'+html.escape(label)+'</a>'
        block='<section id="current-assembly-api"><h2>객체 상속·지역 레이어·응용 물리</h2>'
        block+='<p>공통 C++ API와 Standard 명령 파서를 분리했다. Standard는 0번 작업공간이며 과거 실습 컨테이너는 소스 참조다. 빌드·테스트는 사용자가 수행한다.</p>'
        block+='<p>'+link(base/'docs/WORKSPACE_API.md','공통 API 계약')+' · '+link(STANDARD/'docs/QUICK_COMMANDS.md','명령어 [결과] - 인수 의미')+' · '+link(STANDARD/'docs/TRANSFORM_WORKSPACE.md','월드 배치와 지역 레이어')+' · '+link(STANDARD/'docs/APPLICATION_RECIPES.md','조명·카메라·회전 저울')+'</p>'
        block+=code('W_child = W_parent * attachment * baseSRT * L1 * L2 * ... * Ln')
        block+='<p>월드 배치를 부모 기준 부착 행렬로 저장한다. 객체가 지역 레이어 목록을 소유하고 앞 레이어를 뒤 레이어가 상속한다. 비균등 배율·전단 결과를 SRT로 재분해하지 않는다.</p>'
        block+='<table><tr><th>API</th><th>계약</th></tr>'
        for api,desc in [
            ('Scene::placementMatrix','자기 지역 레이어 이전의 월드 배치'),
            ('Scene::setWorldPlacement','부모 역행렬로 부착 배치를 계산하고 자기 레이어 유지'),
            ('Scene::setParentPreserveWorld','순환 검사, 월드 배치 보존, 실패 시 이전 관계 유지'),
            ('Scene::setAttachment / setLocalLayers','객체 소유 아핀 부착과 순서 있는 지역 행렬 목록'),
            ('PointLight / RenderOptions','월드 점광원 최대8, 색·강도·거리 감쇠'),
            ('DynamicsSystem::effectiveAcceleration / step','중력·이동·원심·코리올리·오일러 항과 지역 힘 콜백'),
            ('DynamicsSystem::holdingForce / normalLoad','정지 구속력과 법선 압축력. 실제 접촉 솔버가 아님'),
            ('DynamicsSystem::spring','지역 앵커·강성·감쇠로 선형 힘 콜백 생성')]:
            block+='<tr><td><code>'+html.escape(api)+'</code></td><td>'+html.escape(desc)+'</td></tr>'
        block+='</table><h3>Dynamics 공개 선언</h3>'+code((base/'include/Dynamics.h').read_text(encoding='utf-8-sig'))
````

### 블록 2 · before L42 / after L53

위치: `def link(path,label):`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````python
            block+='<h3>assembly::Assembly — 공개 API</h3><p>standard 전용 조립 계층이다. framework 공통 API와 별도 소유자다. 위치는 부모 공간, 각도는 도, 시간은 초다. group/part/mount로 계층을 만들고 srt/pivot/center로 중심과 전체 변환을 지정한다. attach는 Motion/Custom/Input 등의 Behavior를 연결한다. 중복 이름·잘못된 값·충돌하는 제어 소유자는 require 예외로 거절한다.</p>'
````

**after**

````python
            block+='<h3>Standard Assembly 공개 선언</h3>'
````

### 블록 3 · before L44 / after L55

위치: `def link(path,label):`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````python
            block+=code(text)+'<p>'+link(STANDARD/'modes/Assembly.h',f'Assembly.h:{line}')+'</p>'
            block+='<table><tr><th>API</th><th>입력·반응·계약</th></tr>'
            for api,desc in [
                ('addFeature(name, mode)','1~22의 새 native 상태/CPU Scene/시계를 생성한다. 한 Application/GL 컨텍스트를 공유한다.'),
                ('portFeature(source, native, name)','실제 native ID와 자식을 출력하는 라이브 포트다. 원본 program/시계를 공유한다. feature parts로 ID를 확인한다.'),
                ('srt / pivot / center / setRest','루트의 기준값을 바꾼다. pivot keep은 기준 배치를 보정한다. center는 형상의 AABB 중심이며 질량중심이 아니다.'),
                ('model / mount / meshEntity','임의 Model을 등록하고 원본 메시의 면/삼각형 좌표에 관절을 붙인다. native 형상을 직접 편집하려면 snapshot으로 독립 복사한다.'),
                ('attach / bind','동작 또는 입력을 기능 단위로 연결한다. 사용자 콜백의 외부 IO/shared 상태까지 자동 복원하지 않는다.'),
                ('execute / update / render','execute 실패는 CPU 조립 상태를 복원한다. update는 초 단위 dt로 한 번 진행하고 실패 시 정지한다. render는 시간을 진행하지 않는다.'),
                ('guide / diagnose / inspect','guide는 현재 소스 줄·변경 전후 예·디버깅 순서를 출력한다. diagnose는 행렬/면/부모/기능 상태, inspect는 객체 등록 정보를 보여 준다.'),
                ('onKey / onMouse / onCursor / onFocus','입력을 원본 또는 공통 편집기로 보낸다. 클릭은 실제 뷰의 exported ID를 native ID로 역매핑한다. 포커스 이탈 시 누름/드래그를 해제한다.')]:
                block+='<tr><td><code>'+html.escape(api)+'</code></td><td>'+html.escape(desc)+'</td></tr>'
            block+='</table><h3>FeatureContainer와 가이드 메타데이터</h3>'
            for filename in ('FeatureContainer.h','GuideRecipes.h'):
                text,line=excerpt(STANDARD/'modes'/filename,'namespace assembly {')
                block+=code(text)+'<p>'+link(STANDARD/'modes'/filename,filename+':'+str(line))+'</p>'
            block+='<p>FeatureContainer의 scene/camera/options는 비소유 const 참조다. 소유 컨테이너의 수명 안에서만 사용한다. 복사는 금지한다. step은 native 상태를 한 번 갱신하고 내부 종료 요청은 오류로 전달한다. GuideSite는 현재 줄 검색 표식, GuideRecipe는 목적·전후 예·제약·디버깅·예상 결과다. findGuideRecipe는 미등록 항목에 nullptr을 반환한다.</p>'
            block+='<h3>사용 예와 핫로드</h3>'+code('assembly::Assembly scene(app);\nscene.addFeature("swingLab", 9);\nscene.addFeature("tankLab", 16);\nscene.move("tankLab", {5,0,0});\nscene.guide("tank", std::cout);\nscene.diagnose("tankLab", std::cout);')
            block+='<p>콘솔: feature parts tankLab → 실제 ID로 feature port → feature focus/camera. Ctrl+F5는 .assembly 설정 재로드이며 C++ 함수 변경은 사용자 재빌드 대상이다. add는 독립 시계, port는 공유 시계다. 컨테이너 사이 물리 충돌은 자동 병합하지 않는다. private 원본 상태의 임의 프레임 역실행은 제공하지 않는다.</p>'
            for name in ('EDIT_DEBUG_GUIDE','FEATURE_CONTAINERS','REFLECTION_AND_HOTLOAD','USER_CHECKS'):
                block+='<p><a href="../document.html?doc=docs/assembly/'+name+'.md">'+name+' · 브라우저 미리보기</a></p>'
        else:
            block+='<p>standard 전용 Assembly/FeatureContainer/GuideRecipe/diagnose는 '+link(STANDARD/'docs/API_REFERENCE.html','standard API Reference의 최신 조립 API')+'에서 확인한다.</p>'
````

**after**

````python
            block+=code(text)+'<p>'+link(STANDARD/'modes/Assembly.h','Assembly.h:'+str(line))+'</p>'
            block+='<p>명령 실행 실패는 CPU Scene·작성 상태·카메라·렌더 옵션을 복원한다. save는 대상과 레이어를 명시한 작성 레시피이며 현재 적분 프레임 스냅샷이 아니다.</p>'
````

### 블록 4 · before L118 / after L108

위치: `def collect_scenarios():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
        if p.stem.endswith('features-mix'): continue # Reference-only legacy controller composition.
````

### 블록 5 · before L138 / after L129

위치: `def write_document_index(docs, catalog):`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````python
    lines = ['[브라우저 목차](index.html) · [시험 시나리오](docs/EXAM_SCENARIOS.html) · [README](README.md)', '',
````

**after**

````python
    lines = ['[브라우저 목차](index.html) · [변환 템플릿](docs/TRANSFORM_TEMPLATES.html) · [README](README.md)', '',
````

### 블록 6 · before L213 / after L204

위치: `def main():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
    import runpy
    runpy.run_path(str(ROOT / 'tools/Write-WorkspaceDocs.py'))['refresh_documents']()
````

## tools/Update-StandardModesGraph.py

### 블록 1 · before L6 / after L6

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
import fnmatch
````

### 블록 2 · before L48 / after L49

위치: `def inventory():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before**

````python
                sources.update(p.resolve() for p in path.parent.glob(include.replace('\\', '/')) if p.is_file())
````

**after**

````python
                exclusions=[x.replace('\\','/') for x in element.get('Exclude','').split(';') if x]
                sources.update(p.resolve() for p in path.parent.glob(include.replace('\\', '/')) if p.is_file() and not any(fnmatch.fnmatch(p.relative_to(path.parent).as_posix(),pattern) for pattern in exclusions))
````

### 블록 3 · before L94 / after L96

위치: `def main():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
    if graph.number_of_nodes()>5000: print('Warning: interactive graph has more than 5000 nodes; initial layout may be slow.')
````

### 블록 4 · before L102 / after L105

위치: `def main():`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
        'source_state': 'Saved working tree; source_commit names the base commit, files records current source hashes.',
````

## tools/Write-ApiKo.py

### 블록 1 · before L217 / after L217

위치: `def description(r):`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
    workspace_descriptions={('Scene', 'placementMatrix'): '자기 지역 레이어 이전의 월드 기준 배치를 반환한다. 부모의 최종 레이어는 상속한다.', ('Scene', 'setWorldPlacement'): '월드 배치를 부모 기준 아핀 부착으로 변환한다. 전단을 보존하고 자기 지역 레이어는 유지한다.', ('Scene', 'setParentPreserveWorld'): '현재 월드 배치를 유지해 부모를 연결한다. 순환·특이행렬 실패 시 기존 관계를 보존한다.', ('Scene', 'setAttachment'): '객체가 소유한 부모 기준 아핀 부착 행렬을 설정한다. 특이행렬은 거부한다.', ('Scene', 'setLocalLayers'): '객체 소유의 순차 지역 행렬 목록을 교체한다. 실패 시 기존 목록을 유지한다.', ('DynamicsSystem', 'validFrame'): '기준 프레임의 정규직교 회전과 유한한 가속도·각속도·각가속도를 확인한다.', ('DynamicsSystem', 'effectiveAcceleration'): '월드 중력과 이동·회전 프레임의 관성항을 합성해 지역 가속도를 계산한다.', ('DynamicsSystem', 'step'): '지역 힘 콜백을 질량으로 나누고 점질량을 적분한다. 비유한 결과는 상태에 반영하지 않는다.', ('DynamicsSystem', 'holdingForce'): '질점을 기준 프레임의 해당 위치에 정지 구속하는 데 필요한 지역 힘을 계산한다.', ('DynamicsSystem', 'normalLoad'): '정지 구속력을 주어진 법선에 투영한 압축력을 반환한다. 실제 접촉 충돌 계산은 아니다.', ('DynamicsSystem', 'spring'): '앵커·강성·감쇠로 영자연길이 선형 스프링의 지역 힘 콜백을 만든다.'}
    if (owner,name) in workspace_descriptions: return workspace_descriptions[owner,name]
````

## tools/Write-TransformReference.py

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
"""Generate operation-based templates and references, independent of practice assignments."""
from pathlib import Path
import html
import json
import re

ROOT=Path(__file__).resolve().parents[1]
STANDARD=ROOT/'standard'
TEMPLATES=[
 ('01-local-order','지역 변환의 순서','이동한 축·회전한 축에서 다음 연산을 적용할 때',[
  ('기본 물체 생성','add box item at 0 0 0'),('회전 레이어 생성','layer add turn'),('지역 Y축으로 30도 회전','layer r y 30'),
  ('앞 레이어의 회전된 축을 상속하는 이동 레이어 생성','layer add offset'),('회전된 지역 X축으로 2 이동','layer t 2 0 0'),
  ('비균등 형상 레이어 생성','layer add shape'),('지역 세 축 배율을 따로 적용','layer s 2 1 0.5')]),
 ('02-pivot','피벗을 고정한 회전','중심·끝점·관절·임의 기준점 주위로 돌 때',[
  ('길이 4의 막대 생성','add box rod 0 0 0 4 0.2 0.2'),('막대 왼쪽 끝을 피벗으로 지정하되 배치는 유지','pivot rod -2 0 0 keep'),
  ('지정한 피벗 주위로 지역 Z축 자전','insert spin z 45')]),
 ('03-inherit-world','월드 좌표로 부착하고 상속','서로 다른 물체가 공통 이동을 따라갈 때',[
  ('부모 프레임을 월드 X=10에 생성','add joint base at 10 0 0'),('자식을 월드 X=12에 생성','add box item at 12 0 0'),
  ('현재 월드 배치를 유지해 부모 연결','parent base'),('자식 내부 회전 추가','insert spin y 45'),
  ('부모 선택','select base'),('부모를 월드 X=15로 이동. 자식 기준 배치는 X=17','t 15 0 0')]),
 ('04-orbit-spin','공전과 자전의 독립 조합','위치 변화와 자체 회전을 각각 조절할 때',[
  ('중심에서 떨어진 물체 생성','add box item at 3 0 0'),('월드 원점을 지역 중심으로 변환해 공전 레이어 생성','insert orbit y 30 center 0 0 0'),
  ('공전 레이어를 상속하는 자체 회전 레이어 생성','insert spin x 90')]),
 ('05-fixed-end-scale','끝점을 고정한 비균등 배율','한쪽 끝은 그대로 두고 길이·폭만 바꿀 때',[
  ('길이 4 막대 생성','add box rod 0 0 0 4 0.2 0.2'),('왼쪽 끝을 피벗으로 지정','pivot rod -2 0 0 keep'),
  ('월드 X축 길이만 두 배. 왼쪽 X=-2는 유지','s 2 1 1')]),
 ('06-periodic','시간 함수로 반복 회전','단순 주기 운동을 물리 적분 없이 작성할 때',[
  ('기본 물체 생성','add box item at 0 0 0'),('Z축 진폭25도·주기3초의 사인 회전','insert pendulum z 25 3')]),
 ('07-input-rate','입력량과 시간당 변화량','한 번 누름과 길게 누름을 구분할 때',[
  ('제어할 물체 생성','add box item at 0 0 0'),('누른 동안 초당 X+1 이동','bind RIGHT hold "move item 1 0 0"'),
  ('한 번 누를 때 Y축5도 회전','bind Q press "rotate item y 5"')]),
 ('08-light-camera','같은 변환을 조명·카메라에 사용','시각 객체와 관찰·광원 배치를 함께 조립할 때',[
  ('관찰할 물체 생성','add box item at 0 0 0'),('점광원 생성','light point lamp at 3 4 2'),
  ('광원 강도 설정','light intensity 2'),('카메라 생성','camera add observer at 6 4 8'),
  ('월드 원점을 바라보게 설정','camera look 0 0 0'),('선택 카메라로 전환','camera use')]),
 ('09-arbitrary-axis','임의 축 회전','좌표축이 아닌 방향 벡터를 기준으로 돌 때',[
  ('기본 물체 생성','add box item at 2 0 0'),('지역 (1,1,0) 방향 축의 공전','insert orbit-axis 1 1 0 30 center 0 0 0')]),
]

def write(path,text):
    path=ROOT/path;path.parent.mkdir(parents=True,exist_ok=True)
    text=text.rstrip()+'\n'
    if not path.exists() or path.read_text(encoding='utf-8-sig')!=text:path.write_text(text,encoding='utf-8-sig')

def main():
    md='''# 변환 조합 템플릿

기존 실습이 시험에 재등장한다는 가정은 사용하지 않는다. **요구한 움직임을 변환·좌표계·계층·시간 함수·힘으로 나누고 필요한 블록을 조합한다.** 실습 소스는 그 연산을 구현한 코드 위치를 찾을 때만 참고한다.

아래는 완성된 문제 풀이가 아닌 최소 동작 블록이다. 각 파일은 clear로 시작하므로 별도 장면에서 읽는다. 합칠 때는 clear·카메라 초기화를 제외하고 객체 이름을 구분한다. 입력은 실제 구현된 Standard 명령이며 실행 확인은 사용자가 수행한다.

| 필요한 연산 | 템플릿 | 바꿀 인수 |
|---|---|---|
'''
    body='<p><a href="../index.html">전체 문서</a> · <a href="../document.html?doc=docs/TRANSFORM_REFERENCE.md">변환 조합 레퍼런스</a> · <a href="../document.html?doc=docs/QUICK_COMMANDS.md">명령어</a></p>'
    body+='<p>실습 재출제를 가정하지 않는다. 요구한 움직임을 연산·좌표계·상속·시간·힘으로 분해한다. 파일마다 장면 초기화가 있으므로 조합할 때 clear와 카메라 초기화를 제외한다.</p><nav>'
    for slug,title,condition,rows in TEMPLATES:body+='<a href="#'+slug+'">'+html.escape(title)+'</a> · '
    body+='</nav>'
    for slug,title,condition,rows in TEMPLATES:
        recipe='# @title: '+title+'\n# @goal: '+condition+'\n#: 독립 템플릿의 시작 장면\nclear\n#: 관찰 카메라\ncamera pose 8 6 10 0 0 0 0 1 0\n'
        for note,command in rows:recipe+='\n#: '+note+'\n'+command+'\n'
        relative='examples/assembly/templates/'+slug+'.assembly';write('standard/'+relative,recipe)
        md+='| '+condition+' | ['+title+'](../'+relative+') | '+('축·중심·각속도' if any('spin' in c or 'orbit' in c for _,c in rows) else '좌표·배율·기간·대상')+' |\n'
        body+='<section id="'+slug+'"><h2>'+html.escape(title)+'</h2><p>'+html.escape(condition)+'</p><p><code>load '+relative+'</code></p><table><tr><th>명령어 [결과]</th><th>인수·관찰점</th></tr>'
        for note,command in rows:body+='<tr><td><code>'+html.escape(command)+'</code></td><td>'+html.escape(note)+'</td></tr>'
        body+='</table><details><summary>복사할 원문</summary><pre>'+html.escape(recipe)+'</pre></details></section>'
    md+='\n[HTML 템플릿 목차](TRANSFORM_TEMPLATES.html) · [연산 선택과 C++ 레퍼런스](TRANSFORM_REFERENCE.md) · [전체 명령](QUICK_COMMANDS.md)\n\n저울은 중력·기준 프레임·회전·힘 콜백을 조합하는 [응용 예제](APPLICATION_RECIPES.md) 하나다. 템플릿을 저울 형상에 종속시키지 않는다.\n'
    write('standard/docs/TRANSFORM_TEMPLATES.md',md)
    body+='<h2>힘을 포함하는 응용 한 가지: 회전 스프링 저울</h2><p><code>load examples/assembly/scenarios/s13-moving-scale.assembly</code></p><p>앞의 변환 블록에 중력·이동/회전 프레임·스프링 콜백을 추가한 예다. 영자연길이 선형 스프링 모델이며 시험 문제의 형상·제약 조건은 별도로 작성한다.</p>'
    body+='<p><a href="../document.html?doc=docs/APPLICATION_RECIPES.md">힘의 단위·가정·저울 예제 설명</a></p>'
    html_page='<!doctype html><html lang="ko"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>변환 조합 템플릿</title><style>body{font:16px/1.8 "Malgun Gothic",sans-serif;max-width:1100px;margin:32px auto;padding:0 24px}pre{background:#142d42;color:#fff;padding:18px;overflow:auto}td,th{border:1px solid #ccd6de;padding:10px}table{border-collapse:collapse;width:100%}section{margin:40px 0}a{color:#15649c}</style></head><body><h1>변환 조합 템플릿</h1>'+body+'</body></html>'
    write('standard/docs/TRANSFORM_TEMPLATES.html',html_page)
    write('standard/docs/EXAM_SCENARIOS.html','<!doctype html><html lang="ko"><meta charset="utf-8"><title>변환 템플릿으로 이동</title><h1>문서 기준 변경</h1><p>실습별 시나리오 안내를 변환 연산별 템플릿으로 바꿨습니다.</p><a href="TRANSFORM_TEMPLATES.html">변환 조합 템플릿 열기</a></html>')
    reference='''# 변환 조합 레퍼런스

SRT와 중심 회전의 뜻을 알고 있다는 전제로, **요구 문장 → 저장할 값 → 적용할 공간·순서 → 실제 API**를 연결한다. 특정 실습·형상을 정답으로 고정하지 않는다.

| 요구 문장 | 저장할 값 | 연산·공간 | Standard / C++ |
|---|---|---|---|
| 이 점에 물체를 둔다 | 월드 기준 배치 D | inverse(부모W)×D를 부착에 저장 | t / Scene::setWorldPlacement |
| 부모 이동을 함께 따른다 | 부모 ID와 부착 행렬 | 부모 최종W×자식 부착 | parent / setParentPreserveWorld |
| 물체 내부 동작만 바꾼다 | 그 객체의 지역 레이어 목록 | 앞 레이어 결과를 뒤에서 상속 | layer / setLocalLayers |
| 회전한 축으로 이동한다 | 각도, 지역 이동량 | R×T | 회전 레이어 뒤 이동 레이어 |
| 이동 후 월드 축으로 돌린다 | 월드 피벗 C와 축 | T(C)×R×T(-C)를 왼쪽에 곱함 | r / TransformSystem::rotation |
| 한쪽 끝을 유지하며 늘린다 | 모델의 고정점, 축별 배율 | 피벗 유지 보정 후 비균등 S | pivot keep + s |
| 점 주위로 공전한다 | 축 직선, 시간별 각도 | T(C)×R(θ(t))×T(-C) | insert orbit / RotationMotion |
| 임의 축으로 회전한다 | 0 아닌 방향 벡터, 축 위 점 | 축 정규화 후 중심 회전 | insert spin-axis/orbit-axis / AxisLine |
| 자전과 공전을 따로 제어한다 | 서로 다른 레이어·각속도 | 공전 프레임 아래 자전 | insert orbit → insert spin |
| 주기적으로 왕복한다 | 진폭, 주기, 위상 | θ(t)=offset+A sin(2πt/P+phase) | insert pendulum / PendulumMotion |
| 시간에 따라 위치를 바꾼다 | from/to, 기간, 보간 함수 | p(u), u(t), 재생 방식 | insert linear / PathMotion |
| 일정 시간 뒤 동작을 추가한다 | 시작 시각, 지속 시간 | 활성 구간에서 행렬 평가 | MotionStep::startSeconds/durationSeconds |
| 누른 시간만큼 바꾼다 | 속도와 키 누름 상태 | 변화량=속도×dt | bind hold / update 콜백 |
| 조명·시점을 이동시킨다 | 광원 위치·카메라 시선 | 같은 객체 상속 후 렌더 파라미터로 전달 | light/camera / PointLight·Camera |
| 중력·원심력 등이 작용한다 | 질량, 기준 프레임 운동, 힘 | 가속도 계산 → 적분 → 위치 변환 | force / DynamicsSystem |

## 행렬을 직접 끼우는 공통 API

다음 조각은 이미 생성한 `scene`과 `id`를 사용한다. 행렬 값은 실제 문제에서 계산해 넣는다. 클래스·문서·명령 파서를 복사할 필요가 없다.

```cpp
auto T = graphics::TransformSystem::translation({2,0,0});
auto R = graphics::TransformSystem::rotation({{0,0,0},{0,1,0}}, 30);
auto S = graphics::TransformSystem::scaling({2,1,0.5f});
if (T && R && S) {
    // Each matrix is a dependent local layer. Geometry receives S, R, T in that order.
    bool ok = scene.setLocalLayers(id, {*T, *R, *S});
}
```

부모와의 관계는 객체의 부착 행렬, 내부 조합은 자기 레이어에 저장한다. 월드 기준 배치를 설정할 때 자기 레이어를 다시 초기화하지 않는다. 비균등 배율과 회전의 합성으로 생긴 전단은 행렬 상태로 유지한다.

## 시간·조건을 문제에 맞게 작성

```cpp
graphics::Motion motion;
graphics::LinearMotion move;
move.to = {3,0,0}; move.durationSeconds = 2;
move.space = graphics::TransformSpace::Local;
bool moving = motion.append(graphics::Motion::linear(move));
graphics::RotationMotion turn;
turn.degreesPerSecond = 45;
turn.space = graphics::TransformSpace::Local;
auto rotate = graphics::Motion::rotation(turn);
rotate.startSeconds = 2;
bool turning = motion.append(rotate);
auto sample = moving && turning ? motion.sample(time) : std::nullopt;
if (sample) {
    bool ok = scene.setLocalLayers(id, {sample->parent, sample->local});
}
```

위 이동은 Once 재생이므로 끝 위치를 유지하고, 회전은 2초부터 시작한다. `durationSeconds`로 단계 자체를 끝내면 그 단계의 기여도 사라진다. `then`은 앞 단계들의 종료 시각이 모두 유한할 때만 사용할 수 있다. 조건에 따라 기준점·각속도·레이어를 바꾸려면 업데이트 콜백에서 조건을 판단하고 해당 값만 갱신한다. sample은 행렬 평가이고 의미 action 실행은 별도 계약이다.

## 여러 물체의 기준점·질량·힘

형상 크기와 부모 배치만으로 물리 질량·관성·제약이 자동 결정되지는 않는다. 기준 프레임 운동, 힘 법칙, 적분 상태, 시각적 형상을 별도 값으로 둔다. [사용자 힘 연결](APPLICATION_RECIPES.md)에서 콜백을 바꾸면 스프링·외력·감쇠 등을 조합할 수 있다. 저울·회전 구조물·매달린 추 등 문제 형상은 이 값들을 사용하는 응용 코드다.

## 빠른 소스 위치

- `modes/AssemblyWorkspace.cpp`: 선택 대상 해석, 피벗, 축별 배율, 객체 내부 레이어, 순서 변경, 템플릿 삽입.
- `framework/include/Transform.h`, `detail/Transform.cpp`: 이동·배율·임의 축 회전·반사 행렬.
- `framework/include/Motion.h`, `detail/Motion.cpp`: 시간 함수와 단계 삽입·활성 구간.
- `framework/detail/Scene.cpp`: 부착·상속·레이어·렌더/피킹의 공통 월드 합성.
- `framework/include/Dynamics.h`, `detail/Dynamics.cpp`: 기준 프레임 가속도·힘·적분.
- `modes/AssemblySceneTools.cpp`: 같은 변환을 광원·카메라에 연결.

[연산별 실행 템플릿](TRANSFORM_TEMPLATES.md) · [전체 명령과 인수](QUICK_COMMANDS.md) · [실습 소스 참조](REFERENCE_PRACTICES.md)
'''
    write('standard/docs/TRANSFORM_REFERENCE.md',reference)
    readme=(STANDARD/'README.md').read_text(encoding='utf-8-sig')
    note='[변환 조합 템플릿](docs/TRANSFORM_TEMPLATES.html) · [변환 조합 레퍼런스](document.html?doc=docs/TRANSFORM_REFERENCE.md)\n\n실습 재출제를 가정하지 않는다. 요구된 변환과 공간·순서·시간·힘을 조합하는 작업공간이다.\n\n'
    write('standard/README.md',readme.replace('# Standard: 0번 장면 작업공간\n\n','# Standard: 0번 장면 작업공간\n\n'+note))
    catalog=json.loads((STANDARD/'docs/DOCUMENT_CATALOG.json').read_text(encoding='utf-8-sig'))
    primary={'docs/TRANSFORM_TEMPLATES.html','docs/TRANSFORM_TEMPLATES.md','docs/TRANSFORM_REFERENCE.md'}
    for category in catalog['categories']:
        category['documents']=[d for d in category['documents'] if d['path'] not in primary]
        if category['id']=='start':
            for d in category['documents']:
                if d['path']=='docs/APPLICATION_RECIPES.md':d.update(title='광원·카메라·힘 API 연결',featured=False)
            category['documents'][1:1]=[{'path':'docs/TRANSFORM_TEMPLATES.html','title':'연산별 변환 템플릿','featured':True},
                {'path':'docs/TRANSFORM_REFERENCE.md','title':'요구 문장 → 변환·공간·API','featured':True},
                {'path':'docs/TRANSFORM_TEMPLATES.md','title':'템플릿 파일 목록'}]
        if category['id']=='exam':
            category['title']='변환과 응용';category['description']='재출제 시나리오 대신 연산·좌표계·시간·힘을 조합한다.'
            for d in category['documents']:
                if d['path']=='docs/EXAM_SCENARIOS.html':d.update(title='이전 시나리오 문서의 이동 안내',featured=False,summary='변환 조합 템플릿으로 통합')
        if category['id']=='cookbook':
            category.update(title='과거 실습 코드 참조',description='변환 구현을 찾는 참고 코드. 시험 유형이나 재출제 예상 목록이 아니다.')
    for category in catalog['categories']:
        if category['id']=='practice-reference':
            category['documents']=[{'path':'docs/REFERENCE_PRACTICES.md'}]+[{'path':p.relative_to(STANDARD).as_posix()} for p in sorted((STANDARD/'docs/reference/modes').glob('*.md'))]
    write('standard/docs/DOCUMENT_CATALOG.json',json.dumps(catalog,ensure_ascii=False,indent=2))

if __name__=='__main__':main()
````

## tools/Write-WorkspaceDocs.py

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
"""Author current workspace docs from source. No builds, tests or browser execution."""
from pathlib import Path
import html
import json
import re

ROOT = Path(__file__).resolve().parents[1]
STANDARD = ROOT / 'standard'

def write(path, text):
    path = ROOT / path
    path.parent.mkdir(parents=True, exist_ok=True)
    text = text.rstrip() + '\n'
    if not path.exists() or path.read_text(encoding='utf-8-sig') != text:
        path.write_text(text, encoding='utf-8-sig')

def page(title, body):
    return ('<!doctype html><html lang="ko"><head><meta charset="utf-8">'
            '<meta name="viewport" content="width=device-width,initial-scale=1"><title>' + html.escape(title) +
            '</title><style>body{font:16px/1.8 "Malgun Gothic",sans-serif;max-width:1050px;margin:32px auto;padding:0 24px}'
            'pre{overflow:auto;background:#152b3e;color:#eef5ff;padding:18px;border-radius:8px}a{color:#14649b}'
            'table{border-collapse:collapse}td,th{border:1px solid #ccd6de;padding:8px}</style></head><body><h1>' +
            html.escape(title) + '</h1>' + body + '</body></html>')

def refresh_documents():
    source = (STANDARD / 'modes/AssemblyGuide.cpp').read_text(encoding='utf-8-sig')
    help_body = source[source.index('void Assembly::help'):source.index('void Assembly::guide')]
    commands = ''.join(json.loads('"' + m[1] + '"') for m in re.finditer(r'"((?:\\.|[^"\\])*)"', help_body))
    write('standard/docs/QUICK_COMMANDS.md', '# 명령어 빠른 참조\n\n'
          '콘솔 F12/help와 같은 소스에서 만든다. 형식은 **명령어 [실행 결과] - 각 인수의 의미**다.\n\n'
          '```text\n' + commands.rstrip() + '\n```\n\n'
          '`t/r/s`는 월드 기준 물체 배치를 바꾼다. `layer t/r/s`는 선택 레이어의 지역 변환을 추가한다. '
          '입력량이 같아도 축·순서에 따라 결과가 다르다. `s`는 상대 배율이며 `srt`는 기준 SRT 설정이다.\n\n'
          '[좌표·레이어 계약](TRANSFORM_WORKSPACE.md) · [응용 레시피](APPLICATION_RECIPES.md)')
    write('standard/README.md', '''# Standard: 0번 장면 작업공간

[전체 문서](index.html) · [명령어](document.html?doc=docs/QUICK_COMMANDS.md) · [변환·상속](document.html?doc=docs/TRANSFORM_WORKSPACE.md) · [응용](document.html?doc=docs/APPLICATION_RECIPES.md)

Standard는 0번 작업공간으로 시작한다. 실습 1~22의 번호 전환은 제거했다. 기존 모드와 보조 컨트롤러는 `docs/reference/modes/*.md`에 전체 소스를 담은 레퍼런스 문서로 전환했다. Standard 프로젝트에는 현재 작업공간 구현 파일만 명시한다. `main.cpp`에서 숫자 모드를 지정할 필요가 없다.

## 바로 작성

```text
add box
t 2 0 0
s 2 1 0.5
layer add hinge
layer r z 30
insert spin y 45
save scene.assembly
```

왼쪽 클릭으로 물체를 선택하고 Ctrl+클릭으로 그 부모 물체를 선택한다. 빈 공간 클릭은 작업 평면의 생성 기준점을 지정하고 선택을 해제한다. 표면 클릭은 교차점을 생성 기준점으로 사용한다. `add`는 새 물체를 자동 선택하며 이름 생략 시 `box1`처럼 생성한다. 좌표를 직접 지정할 때는 `add box item at 2 0 0`을 사용한다.

## 입력

- G/R/S → X/Y/Z → 마우스 좌우 이동 또는 방향키: 이동/회전/배율. Shift+방향키는 미세 조작이다.
- Enter 또는 클릭: 확정. Esc: 미리보기 취소. 포커스 이탈도 미리보기를 취소한다.
- `layer select 이름` 상태에서는 같은 단축키가 해당 지역 레이어에 작용한다. `layer select none`으로 물체 배치 편집에 복귀한다.
- 기본 선택은 물체 전체다. `edit part` 또는 `edit face`를 명시한 경우에만 부품·면을 선택한다. 면 모드에서는 t/r/s와 단축키가 선택 면에 작용한다. Esc는 물체 모드로 복귀한다.
- Space: 시간 정지/재개. F12: 명령 도움말. Ctrl+F5: 마지막 읽은 레시피 재읽기. `quit`: 종료.

## 경계

`framework`는 공통 C++ API, Standard는 명령·선택·편집을 맡는다. 문서와 과거 가이드 소스는 실행에 필요하지 않다. 콜백은 main에서 한 번 등록하고 Application이 입력 → 사용자 갱신 → Scene 모션 → 렌더 → swap을 실행한다.

`save`는 현재 작성 상태를 재구성하는 레시피를 저장한다. 선택 대상·레이어·커서 좌표를 명시한 명령으로 기록한다. 물리·모션의 현재 프레임 전체를 저장하는 기능은 아니다. 상대 경로는 **프로세스 작업 폴더** 기준이다. 실행 파일 옆에 쓰려면 그 폴더를 작업 폴더로 지정하거나 절대 경로를 쓴다.

소스 검토와 문서 생성만 수행했다. 빌드·테스트·화면 확인은 [사용자 확인 절차](USER_CHECKS.md)에 따라 사용자가 수행한다. 변경 텍스트는 UTF-8 BOM이다.
''')
    write('standard/docs/TRANSFORM_WORKSPACE.md', '''# 월드 배치·객체 상속·지역 레이어

## 합성 계약

열벡터 기준으로 다음과 같이 합성한다. 레이어 목록에서 앞에 있는 항목은 뒤 항목의 좌표계를 제공한다. 정점에는 오른쪽 행렬부터 작용한다.

```text
W_child = W_parent × A_child × B_child × L1 × L2 × ... × Ln
B_child = T(basis) × R × S × T(-localBasis)
```

객체 상속은 Scene의 parent 관계다. `A`는 객체가 소유한 부모 기준 아핀 부착 행렬, `B`는 기존 SRT, `L`은 그 객체가 소유한 지역 레이어다. 별도 전역 그룹을 만들어 레이어를 여러 물체에 암묵적으로 적용하지 않는다. 공통 움직임이 필요하면 부모 객체에 레이어를 넣고 자식 객체를 연결한다.

## 전역 좌표로 부착 위치 지정

사용자가 입력한 월드 기준 배치 D를 다음과 같이 저장한다.

```text
A_child = inverse(W_parent) × D × inverse(B_child)
```

자기 레이어 L은 이 계산에서 바꾸지 않는다. 부모 W에는 부모 자신의 레이어까지 포함한다. `parent base`는 현재 배치와 자기 레이어를 보존해 연결하며 이후 부모의 움직임을 상속한다. `parent none`은 같은 방식으로 연결을 해제한다. `t X Y Z`는 자기 레이어 이전 기준 배치의 월드 원점이다. `r`과 `s`는 최종 피벗을 중심으로 월드 축에 상대 변환을 적용한다.

예: 부모가 월드 X=10, 자식이 X=12일 때 연결하면 부모 기준 간격은 2다. 이후 부모가 X=15로 이동하면 자식은 X=17이 된다. 자식 내부 `layer t 1 0 0`은 별도의 지역 변위다.

## 비균등 배율과 행렬 보존

`s 2 1 0.5`, `layer s 2 1 0.5`, 축별 SRT를 지원한다. `s`는 배율을 곱한다. 절대 SRT는 `srt 이름 TX TY TZ RX RY RZ SX SY SZ`이며 R은 도 단위, Rz×Ry×Rx 순서다. 반사 배율도 사용할 수 있다. 0배율과 역행렬이 없는 부착·레이어 행렬은 거부한다.

비균등 배율을 가진 부모와 회전한 자식을 합성하면 전단이 생길 수 있다. 부착과 지역 레이어는 4×4 행렬로 보존하며 균일 배율이나 SRT로 강제 환원하지 않는다. 렌더·피킹·소유 선은 Scene의 같은 월드 합성을 사용한다. 법선은 렌더러의 역전치 행렬, 반사 앞면은 determinant 기반 winding 보정을 따른다.

## 지역 레이어 작성과 의존 순서

```text
layer add offset
layer t 2 0 0
layer add tilt
layer r z 30
layer add shape
layer s 2 1 0.5
layer select shape
layer before tilt
layer list
```

각 객체 안의 목록이 곧 단일 의존 체인이다. 분기하는 레이어가 필요하면 별도 자식 객체로 만든다. 레이어 이름은 객체 안에서 고유하며 `none`은 예약어다. `layer reset`은 정적 행렬만 초기화하고 모션은 유지한다. `layer play off`는 시간을 멈추고, `layer enable off`는 그 레이어 기여를 제거한다. 레이어 순서 변경은 뒤에 있는 모든 지역 축에 영향을 준다.

`insert spin`, `orbit`, `linear`, `pendulum`은 새 지역 모션 레이어를 만든다. orbit의 월드 중심은 삽입 시점의 객체 프레임으로 변환해 저장한다. 나중에 부모를 움직이면 공전 중심도 그 객체와 함께 이동한다. 지정 중심을 계속 월드에 고정하는 제약 조건은 아니다.

## 소스

- `framework/include/Model.h`: Component_Poly의 attachment/localLayers.
- `framework/detail/Scene.cpp`: placementMatrix, setWorldPlacement, setParentPreserveWorld, setLocalLayers, worldMatrix.
- `standard/modes/AssemblyWorkspace.cpp`: 짧은 명령 해석, 레이어 작성, 변환 미리보기.
- `standard/modes/AssemblyRuntime.cpp`: 모션 샘플과 레이어를 행렬로 전달.
- `standard/modes/AssemblyPersistence.cpp`: 대상·레이어를 명시한 레시피와 부착 행렬 저장.
''')
    write('standard/docs/APPLICATION_RECIPES.md', '''# 응용 장면을 짧게 작성하기

[명령어](QUICK_COMMANDS.md) · [좌표와 레이어](TRANSFORM_WORKSPACE.md) · [확인 절차](../USER_CHECKS.md)

## 부모 이동과 자식의 지역 회전

```text
clear
add joint base at 0 0 0
insert linear 3 0 0 4
add box arm at 2 0 0
parent base
insert spin z 30
```

base의 이동은 arm에 상속된다. arm의 자전은 자기 지역 레이어다. 부모의 위치 제어와 자식의 회전 코드를 한 업데이트 함수에 섞지 않는다.

## 이동 가능한 조명

```text
light point lamp at 3 4 2
light color 1 0.8 0.5
light intensity 2
insert orbit y 30 center 0 0 0
```

점광원은 표시용 메시가 있어 클릭할 수 있다. t/r/s·부모·지역 레이어로 위치를 바꾼다. 기본 셰이더는 점광원 최대 8개다. `light sun 0`은 기존 방향광을 끈다. 점광원 강도 0은 해당 광원을 끈다. 거리 감쇠는 `1/(1+attenuation*distance²)`다.

## 카메라 추가 후 이동

```text
camera add observer at 6 4 8
camera look 0 0 0
camera use
t 5 3 7
camera fov 50
camera free
```

카메라도 같은 객체 상속과 레이어를 사용한다. 시선은 월드 목표점으로 입력한다. 상속된 배율·전단은 시선/up을 정규직교화해 카메라 자세에서 제거한다. near/far/FOV를 형상 배율로 자동 변경하지 않는다. 활성 카메라의 표시 메시는 자기 시야를 가리지 않도록 숨긴다. `camera free` 후에는 일반 시점의 camera move/yaw/orbit/zoom 명령을 사용할 수 있다.

## 이동·회전하는 스프링 저울

```text
load examples/assembly/scenarios/s13-moving-scale.assembly
force read load
pause on
```

이 예제는 **영자연길이의 3차원 선형 스프링과 점질량** 모델이다. 실제 시험의 저울 형상을 재현한 것은 아니다. 강체 carrier의 공전 레이어 → 기준 좌표계 가속도 → 중력·관성항·스프링 힘 → 추 위치 → 렌더 순서다. 선 길이와 기울기가 변하며 스프링 힘을 N으로 출력한다. 충돌·마찰·양팔 저울의 강체 토크는 자동 추가하지 않는다.

지역 유효 가속도는 `Rᵀ(g-a_origin) - ω×(ω×r) - 2ω×v - α×r`다. ω와 α는 지역 좌표로 바꾸며 단위는 rad/s와 rad/s²다. 콘솔 회전 입력은 도/초다. Standard는 프레임 이동·회전의 유한차분으로 가속도를 근사한다. C++에서는 ReferenceFrame에 해석적으로 계산한 값을 직접 넣을 수 있다.

`force read`의 스프링 힘은 실제 현재 스프링/감쇠 힘의 크기다. 같이 표시하는 접촉 반력은 **그 위치에 정지하도록 구속한다고 가정한** +Y 압축력이다. 움직이는 추의 실제 접촉력과 같다고 해석하지 않는다.

## C++에서 사용자 힘 추가

```cpp
graphics::ReferenceFrame frame;
graphics::ParticleState particle;
particle.mass = 1.0f;
auto spring = graphics::DynamicsSystem::spring({0, 0, 0}, 40.0f, 3.0f);
graphics::ForceFunction force = [spring](const graphics::ParticleState& p, double t) {
    return spring(p, t) + glm::vec3(float(2 * std::sin(t)), 0, 0);
};
bool ok = graphics::DynamicsSystem::step(particle, frame, dt, {0, -9.8f, 0}, force, time);
```

콜백은 **지역 뉴턴값**을 반환하는 순수 함수로 작성한다. 질량으로 나누는 처리는 step이 맡는다. 외부 파일·공유 상태를 변경하는 부수 효과는 복구하지 않는다. 기본 적분은 최대 1/240초 간격의 semi-implicit Euler이며 강성·질량·프레임 속도에 따른 수치 오차가 있다. 힘 기준 프레임은 강체여야 한다. 비균등 배율은 시각적 메시 자식에 적용한다.
''')
    checks = '''# 사용자 빌드·동작 확인

이 작업에서는 빌드·테스트·검증기·앱·브라우저를 실행하지 않았다. 아래 절차는 사용자가 수행한다.

1. Visual Studio에서 `standard/standard.sln`의 x64 구성을 빌드하고 실행한다. 0번 작업공간만 시작되고 숫자/F1~F11로 실습이 전환되지 않아야 한다.
2. 빈 공간 클릭 → `add box` → `s 2 1 0.5`: 생성 기준점에 새 물체가 생기고 축별 크기가 달라져야 한다. 콘솔은 명령 [결과] - 인수 설명과 실제 해석 인수·월드 원점을 출력해야 한다.
3. `add joint base at 10 0 0`, `add box child at 12 0 0`, `parent base`: 연결 직후 자식 위치가 유지되어야 한다. base를 `t 15 0 0`으로 옮기면 자기 레이어가 없는 child는 X=17이어야 한다.
4. 부모를 비균등 배율·회전시키고 자식을 다시 연결한다. 월드 배치가 유지되고 렌더·피킹·축 표시가 일치해야 한다. 0배율 또는 순환 부모 연결은 이전 상태를 유지하며 거부해야 한다.
5. `layer add a`, `layer t 2 0 0`, `layer add b`, `layer r z 30`: 목록 순서에 따른 결과를 관찰한다. b를 `layer before a`로 옮기면 결과가 바뀌어야 한다. 부모 이동은 계속 상속되어야 한다.
6. G/R/S 미리보기 → Enter, Esc, 포커스 이탈을 각각 수행한다. Enter만 확정하고 Esc/포커스 이탈은 이전 상태를 복원해야 한다. `edit face`에서도 취소를 확인한다. 기본 클릭은 면을 선택하지 않아야 한다.
7. 광원과 카메라 예제를 실행한다. 점광원 이동에 따라 조명이 바뀌고 카메라의 t/r/s·레이어가 시점에 반영되어야 한다. camera free 후 표시 메시가 다시 보여야 한다.
8. 서로 다른 물체·레이어를 마우스로 고른 뒤 편집하고 save/load한다. 대상이 바뀌거나 같은 변환이 중복 적용되지 않아야 한다. 모션·적분 시계는 초기부터 재시작한다.
9. 정지 ReferenceFrame에서 1kg의 normalLoad는 +Y 법선·중력 (0,-9.8,0)일 때 9.8N이어야 한다. 중력 0, ω=(0,0,2), r=(3,0,0)이면 유효 원심 가속도는 (+12,0,0), 정지 구속력은 (-12,0,0)N이다.
10. 정지 프레임·1kg·강성40의 영자연길이 수직 스프링은 감쇠 후 앵커 아래 약 0.245m에 접근해야 한다. 회전 저울 예제에서는 관성항으로 길이·기울기·힘이 변한다. 과도 진동을 정적 하중으로 해석하지 않는다.
11. `standard/index.html`에서 명령·변환·응용·API·실습 소스 참조를 연다. `s13-moving-scale`의 줄별 설명, 목차·검색·코드 링크를 확인한다. 문서를 제거한 소스 복사본도 Standard 런타임에서 문서 읽기를 요구하지 않아야 한다.

실패를 보고할 때는 콘솔 명령, 선택 물체·레이어, `diagnose 이름`, 화면, 빌드 오류 위치를 함께 기록한다.
'''
    write('standard/USER_CHECKS.md', checks)
    write('standard/docs/assembly/USER_CHECKS.md', checks.replace('# 사용자 빌드·동작 확인', '# 조립 작업공간 사용자 확인'))
    aliases = {
        'README': ('0번 작업공간 문서 지도', '../QUICK_COMMANDS.md', '모든 실습 실행을 합치는 컨테이너 구조는 참조용으로 전환했다. 기본 작업은 객체·상속·지역 레이어다.'),
        'USER_GUIDE': ('작업공간 사용 순서', '../APPLICATION_RECIPES.md', '클릭 → 생성 → 월드 배치 → 부모 연결 → 지역 레이어 → 저장 순서로 작성한다.'),
        'SPEC': ('명령과 좌표계 계약', '../QUICK_COMMANDS.md', '명령 도움말은 AssemblyGuide.cpp에서 생성한다. 내부 with/attachment/face-edit 명령은 저장 재현용이며 직접 입력할 필요가 없다.'),
        'CODE_GUIDE': ('C++ 확장 위치', '../APPLICATION_RECIPES.md', '`AssemblyWorkspace.cpp`는 선택·배치·레이어, `AssemblySceneTools.cpp`는 광원·카메라, `AssemblyDynamics.cpp`는 힘 연결이다. 새 물리 법칙은 framework/Dynamics API로 작성한다.'),
        'EDIT_DEBUG_GUIDE': ('편집과 오류 위치', '../../USER_CHECKS.md', '콘솔 실패는 명령 전 Scene·작성 상태·카메라·RenderOptions를 복원한다. 사용자 콜백의 외부 부수 효과는 복원하지 않는다. update 실패는 시간을 멈추고 원인을 출력한다.'),
        'FEATURE_CONTAINERS': ('과거 실습 컨테이너 소스 참조', '../REFERENCE_PRACTICES.md', 'FeatureContainer와 feature 모드 혼합은 현재 Standard 런타임에서 제외했다. 과거 소스의 구조를 읽는 참조 자료이며 새 Assembly API와 연결하는 사용법이 아니다.'),
        'REFLECTION_AND_HOTLOAD': ('레시피 재읽기와 C++ 변경', '../QUICK_COMMANDS.md', 'load/reload/Ctrl+F5는 데이터 명령을 다시 읽는다. C++ 수정은 사용자가 재빌드한다. 실행 중 C++ 리플렉션·함수 교체를 제공하지 않는다.'),
        'PATTERN_COMBINATIONS': ('객체와 변환 템플릿 조합', '../APPLICATION_RECIPES.md', '부모 객체가 공통 움직임을 제공하고 자식 객체가 지역 레이어를 소유한다. 기존 탱크·사람 생성 코드는 선택적으로 참고한다.'),
        'REQUIREMENT_AUDIT': ('현재 지원 범위', '../TRANSFORM_WORKSPACE.md', '월드 배치·부모 상속·비균등 배율·순차 지역 레이어·피킹·면 세부모드·점광원·카메라·사용자 힘을 제공한다. 강체 충돌·마찰·시험 문제별 제약 솔버는 별도 응용이다.'),
    }
    for name, (title, link, text) in aliases.items():
        write('standard/docs/assembly/' + name + '.md', '# ' + title + '\n\n' + text + '\n\n[현재 문서](' + link + ')')
    reference = '# 실습 코드 참조\n\n과거 모드 1~22와 컨트롤러 소스는 전체 코드를 담은 Markdown 문서로 전환했다. standard.vcxproj는 현재 작업공간 .cpp만 명시하며 이 문서는 None 항목이다. 공통 경로 연산은 AssemblyPaths.cpp에 독립 함수로 보존했다.\n\n'
    for p in sorted((STANDARD/'docs/reference/modes').glob('*.md')):
        reference += '- ['+p.name+'](reference/modes/'+p.name+')\n'
    reference += '\n각 문서는 당시 구조 참조다. 새 Assembly API와 함께 컴파일하는 코드가 아니다. 원래 독립 실습 프로젝트는 저장소의 각 openGL_* 폴더에 별도로 남아 있다.\n'
    write('standard/docs/REFERENCE_PRACTICES.md', reference)
    for name in ('MODE_ACTIONS', 'MODE_INPUT_AUDIT'):
        write('standard/docs/' + name + '.md', '# 실습 모드 안내 변경\n\n0번 작업공간으로 통합했다. 구형 숫자 모드 조작표는 현재 동작과 맞지 않아 제거했다.\n\n[실습 소스 참조](REFERENCE_PRACTICES.md) · [현재 명령](QUICK_COMMANDS.md)')
    framework = '''# 객체 부착·지역 레이어·힘 API

공통 API는 문서·Standard 명령 파서 없이 사용할 수 있다. 공개 진입점은 Framework.h다.

| API | 입력 → 결과·실패 계약 |
|---|---|
| Scene::placementMatrix(id) | 부모 최종 변환 × 객체 부착 × 기준 SRT. 자기 지역 레이어 이전의 월드 배치. 잘못된 ID/행렬은 빈 optional |
| Scene::setWorldPlacement(id,D) | D를 부모 기준 아핀 부착으로 변환. 자기 레이어 유지. 특이행렬/없는 ID는 false |
| Scene::setParentPreserveWorld(id,parent) | 순환 검사 후 배치를 유지해 연결. 실패 시 기존 부모·부착 유지 |
| Scene::setAttachment(id,A) | 부모 기준 행렬 직접 설정. 유한하고 역행렬 있는 아핀 행렬 필요 |
| Scene::setLocalLayers(id,matrices) | 순서대로 지역 합성. 최대128, 실패 시 기존 목록 유지 |
| RenderOptions::pointLights | 월드 점광원 목록. 기본 셰이더 최대8, 색·강도·거리 감쇠 |
| DynamicsSystem::effectiveAcceleration | 강체 기준, 지역 위치·속도, 월드 중력 → 지역 가속도 |
| DynamicsSystem::step | 점질량·힘 콜백·dt≤0.25초 → 상태 적분. 비유한 결과는 미반영 |
| DynamicsSystem::holdingForce | 그 위치를 기준 프레임에 고정할 때 필요한 지역 힘 |
| DynamicsSystem::normalLoad | 정지 구속 힘을 지정 법선에 투영한 압축력. 실제 접촉 솔버가 아님 |
| DynamicsSystem::spring | 지역 앵커·양수 강성·0이상 감쇠 → 힘 콜백. 잘못된 값은 빈 함수 |

기존 MotionSystem의 공유 모션 레이어와 Component_Poly의 객체 내부 localLayers는 별개다. 새 객체 작성은 parent + attachment + localLayers 계약을 사용한다. 기존 AnimationSystem::poseHierarchy는 여전히 양수 균일 기준 자세와 강체 모션을 전제하는 호환 API다. 비균등·전단 결과를 이 API로 분해하지 않는다.

```cpp
auto parent = scene.addGroup();
auto child = scene.addCuboid({1,1,1}, {2,0,0});
if (parent && child) {
    bool attached = scene.setParentPreserveWorld(*child, *parent);
    auto turn = graphics::TransformSystem::rotation({{0,0,0},{0,1,0}}, 30);
    auto scale = graphics::TransformSystem::scaling({2,1,0.5f});
    if (attached && turn && scale) scene.setLocalLayers(*child, {*turn, *scale});
}
```

모든 행렬은 4×4 아핀 행렬이다. setLocalLayers는 결과를 SRT로 재분해하지 않는다. 렌더와 피킹은 Scene::worldMatrix를 공유한다.
'''
    for base in ('framework', 'standard/framework'):
        write(base + '/docs/WORKSPACE_API.md', framework)
        path = ROOT / base / 'README.md'
        text = path.read_text(encoding='utf-8-sig')
        start, end = '<!-- workspace-current:start -->', '<!-- workspace-current:end -->'
        text = re.sub(re.escape(start) + r'.*?' + re.escape(end) + r'\n*', '', text, flags=re.S)
        note = start + '\n[객체 부착·지역 레이어·힘 API](docs/WORKSPACE_API.md). Standard는 0번 작업공간이며 실습은 코드 참조다.\n' + end + '\n\n'
        write(base + '/README.md', note + text)
    write('standard/PORTABLE.md', '# Standard 폴더 사용\n\nstandard 전체를 복사하고 standard.sln의 Windows x64 구성을 사용한다. C++17, Visual Studio C++ 도구와 OpenGL 3.3이 필요하다. framework/dependencies는 실행 의존성이다. docs·브라우저 목차·과거 실습 소스는 편집기 런타임 의존성이 아니다. 빌드 후에는 레시피가 실행 파일 옆 examples에 복사된다. `save` 상대 경로는 프로세스 작업 폴더 기준이다. 빌드·실행 확인은 사용자가 수행한다.')
    body = '<p><a href="../index.html">전체 문서</a> · <a href="../document.html?doc=docs/QUICK_COMMANDS.md">명령어</a> · <a href="../document.html?doc=docs/APPLICATION_RECIPES.md">응용 예제</a></p>'
    body += '<p>마우스로 대상 지정 → 월드 배치 → 객체 상속 → 지역 레이어 → 조명·카메라·힘 → 저장 순서로 작성한다. 빌드·테스트는 사용자가 수행한다.</p>'
    body += '<pre>' + html.escape('add box\ns 2 1 0.5\nlayer add hinge\nlayer r z 30\ninsert spin y 45\nlight point lamp at 3 4 2\nsave scene.assembly') + '</pre>'
    for p in sorted((STANDARD / 'examples/assembly/scenarios').glob('*.assembly')):
        if p.stem.endswith('features-mix'): continue
        text = p.read_text(encoding='utf-8-sig')
        match = re.search(r'#\s*@title:\s*(.*)', text)
        body += '<details><summary>' + html.escape(match[1] if match else p.stem) + '</summary><p><a href="../' + p.relative_to(STANDARD).as_posix() + '">레시피 원문</a></p><pre>' + html.escape(text) + '</pre></details>'
    body += '<p>기존 레시피의 긴 명령은 명시적 대상 이름을 사용하는 호환 경로다. 새 작성에는 짧은 명령과 지역 레이어를 우선한다. 과거 feature 모드 혼합은 코드 참조다.</p>'
    write('standard/docs/EXAM_SCENARIOS.html', page('응용 시나리오: 객체와 지역 레이어로 작성', body))
    for name in ('IMPLEMENTATION_PATTERNS.html', 'EXAM_IMPLEMENTATION_ORDER.html'):
        write('standard/docs/' + name, page('응용 장면 작성 순서', body.split('<details>', 1)[0] + '<p><a href="EXAM_SCENARIOS.html">전체 시나리오와 줄별 설명</a></p>'))
    catalog_path = STANDARD / 'docs/DOCUMENT_CATALOG.json'
    catalog = json.loads(catalog_path.read_text(encoding='utf-8-sig'))
    remove = {'README.md','USER_CHECKS.md','docs/MODE_INPUT_AUDIT.md','docs/MODE_ACTIONS.md','docs/QUICK_COMMANDS.md',
              'docs/TRANSFORM_WORKSPACE.md','docs/APPLICATION_RECIPES.md','docs/REFERENCE_PRACTICES.md'}
    for category in catalog['categories']:
        category['documents'] = [d for d in category['documents'] if d['path'] not in remove]
        if category['id'] == 'start':
            category['description'] = '0번 장면 작업공간의 입력·명령·좌표계. 실습은 소스 참조다.'
            category['documents'] = [{'path':'README.md','title':'0번 작업공간 시작','featured':True},
                {'path':'docs/QUICK_COMMANDS.md','title':'명령어 [결과] - 인수 의미','featured':True},
                {'path':'docs/TRANSFORM_WORKSPACE.md','title':'월드 배치·상속·지역 레이어','featured':True},
                {'path':'docs/APPLICATION_RECIPES.md','title':'조명·카메라·회전 저울 응용','featured':True},
                {'path':'USER_CHECKS.md','title':'사용자 빌드·동작 확인'}] + category['documents']
        if category['id'] == 'exam':
            category['description'] = '실습과 다른 조건도 객체·변환·힘으로 조합한다.'
            for d in category['documents']:
                if d['path']=='docs/EXAM_SCENARIOS.html': d.update(title='일반 응용 시나리오',summary='월드 배치·지역 레이어·회전 스프링 저울')
        if category['id'] == 'assembly':
            category['description'] = '짧은 명령과 객체별 지역 레이어로 작성·저장·확장한다.'
            for d in category['documents']:
                if d['path'].endswith('FEATURE_CONTAINERS.md'): d['summary']='과거 컨테이너 소스 참조; 현재 런타임 제외'
    reference_category = next((c for c in catalog['categories'] if c['id']=='practice-reference'), None)
    if reference_category is None:
        reference_category={'id':'practice-reference','title':'실습 소스 참조','description':'실행 모드가 아닌 코드 레퍼런스','documents':[]}
        catalog['categories'].append(reference_category)
    reference_category['documents']=[{'path':'docs/REFERENCE_PRACTICES.md'}]
    write('standard/docs/DOCUMENT_CATALOG.json', json.dumps(catalog, ensure_ascii=False, indent=2))
    refresh_transform_templates()

def refresh_transform_templates():
    import runpy
    runpy.run_path(str(ROOT/'tools/Write-TransformReference.py'))['main']()

if __name__ == '__main__':
    refresh_documents()
````

## tools/Write-WorkspaceReport.py

### 블록 1 · before L1 / after L1

위치: `file scope`

변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.

**before** (없음)


**after**

````python
"""Record source changes, entrypoint flows and unexecuted checks; no build/test execution."""
from pathlib import Path
import difflib
import hashlib
import json
import re
import subprocess

ROOT=Path(__file__).resolve().parents[1]
STANDARD=ROOT/'standard'
BASE='e6af6deb4b2c8acc5028307592b654bcaeebbf4d'
OUT=ROOT/'docs/workspace-redesign'

def git(*args):return subprocess.check_output(['git','-c','core.autocrlf=false',*args],cwd=ROOT).decode('utf-8')
def write(path,text):
    path.parent.mkdir(parents=True,exist_ok=True)
    path.write_text(text.rstrip()+'\n',encoding='utf-8-sig')
def source_before(path):
    result=subprocess.run(['git','show',BASE+':'+path],cwd=ROOT,capture_output=True)
    return result.stdout.decode('utf-8-sig') if result.returncode==0 else ''
def codeblock(text,language):
    fence='`'*max(4,1+max((len(x) for x in re.findall(r'`+',text)),default=0))
    return fence+language+'\n'+text.rstrip('\n')+'\n'+fence+'\n'
def location(lines,line):
    for i in range(min(line,len(lines))-1,-1,-1):
        if re.match(r'^\s*(?:def |class |struct |(?:static |std::\S+ |bool |void |Tokens |int |glm::\S+ )+(?:\w+::)*\w+\s*\()',lines[i]):
            return lines[i].strip()
    return 'file scope'

def main():
    baseline_files=set(git('ls-tree','-r','--name-only',BASE).splitlines())
    changed=set(git('diff','--name-only',BASE,'--').splitlines())
    new=git('ls-files','--others','--exclude-standard').splitlines()
    allowed=('framework/','standard/','tools/','docs/workspace-redesign/','graphify-out/standard-composition/','framework-analysis/api-reference/')
    changed.update(p for p in new if p.startswith(allowed) and '/__pycache__/' not in p)
    code_suffixes={'.cpp':'cpp','.h':'cpp','.inl':'cpp','.glsl':'glsl','.py':'python','.ps1':'powershell','.vcxproj':'xml'}
    report='# 전체 코드 변경 전후\n\n기준 커밋 `'+BASE+'`부터 현재 저장된 코드까지의 모든 변경 블록이다. 문서·그래프 자동 산출물은 manifest에 기록하고 본문 코드 대조에서 제외한다. 빌드·테스트·검증기는 실행하지 않았다.\n\n'
    manifest=[]
    for name in sorted(changed):
        path=ROOT/name
        data=path.read_bytes() if path.exists() else b''
        manifest.append({'path':name,'new':name not in baseline_files,'exists':path.exists(),'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest() if data else None})
        if path.suffix not in code_suffixes or '/dependencies/' in name:continue
        before=source_before(name).replace('\r\n','\n')
        after=data.decode('utf-8-sig').replace('\r\n','\n') if data else ''
        left=before.splitlines(keepends=True);right=after.splitlines(keepends=True)
        groups=list(difflib.SequenceMatcher(None,left,right,autojunk=False).get_grouped_opcodes(0))
        if not groups:continue
        report+='## '+name+'\n\n'
        for index,group in enumerate(groups,1):
            first,last=group[0],group[-1]
            i,j=first[1],last[2];a,b=first[3],last[4]
            old=''.join(left[i:j]);current=''.join(right[a:b])
            report+='### 블록 '+str(index)+' · before L'+str(i+1)+' / after L'+str(a+1)+'\n\n위치: `'+location(right,a+1).replace('`','')+'`\n\n'
            report+='변경 이유: 승인된 0번 작업공간·객체 상속·비균등 변환·지역 레이어·입력·광원/카메라·힘 연결과 문서 생성 계약을 해당 위치에 반영한다. 구체적 동작은 PLAN 및 변환 레퍼런스를 따른다.\n\n'
            report+='**before**'+(' (없음)' if not old else '')+'\n\n'+(codeblock(old,code_suffixes[path.suffix]) if old else '')+'\n'
            report+='**after**'+(' (없음)' if not current else '')+'\n\n'+(codeblock(current,code_suffixes[path.suffix]) if current else '')+'\n'
    write(OUT/'ALL_CODE_BEFORE_AFTER.md',report)
    write(OUT/'CHANGE_MANIFEST.json',json.dumps({'baseline':BASE,'source_state':'Saved working tree during step 5; existing user docs.zip excluded.','files':manifest},ensure_ascii=False,indent=2))
    graph_dir=ROOT/'graphify-out/standard-composition'
    source_manifest=json.loads((graph_dir/'SOURCE_MANIFEST.json').read_text(encoding='utf-8-sig'))
    projects=json.loads((graph_dir/'PROJECTS.json').read_text(encoding='utf-8-sig'))
    graph=json.loads((graph_dir/'graph.json').read_text(encoding='utf-8-sig'))
    nodes={n['id']:n for n in graph['nodes']}
    links=graph.get('links',graph.get('edges',[]))
    architecture='# 실행 구조와 소스 관계\n\n'
    architecture+='graphify AST 산출물: '+str(source_manifest['nodes'])+' nodes / '+str(source_manifest['edges'])+' edges. 코드 전용 추출이며 의미 분석 API 토큰 사용은 0이다. 빌드·테스트·진단기·앱·브라우저는 실행하지 않았다. 그래프는 정적 관계이고 동적 디스패치의 실제 실행 증거가 아니다.\n\n'
    architecture+='## 현재 Standard\n\n'
    architecture+='1. `standard/main.cpp::main`이 입력·초기화·갱신·렌더 콜백을 한 번 등록한 뒤 `Application::run`을 호출한다. 등록 시점과 콜백 실행 시점은 다르다.\n'
    architecture+='2. run 초기화 → Console::initScene → defaultRecipe → Assembly::execute → resolveCommand → dispatch → applyPoses(0). 실습 번호 라우팅·FeatureContainer·GuideRecipes는 Markdown 참조 문서로 전환했다. Standard는 작업공간 소스만 컴파일한다.\n'
    architecture+='3. 이벤트 루프 PollEvents → Console 입력 콜백 → Assembly 입력 → 피킹 또는 명령 실행. 단축키 미리보기는 작성 상태와 Scene 사본을 보관한다. Enter는 확정, Esc/포커스 이탈은 복원한다.\n'
    architecture+='4. Console::onUpdate → 콘솔 입력을 최대8줄 처리 → Assembly::update → 시계 진행 → applyPoses. 기준 SRT·부착·지역 모션을 Scene에 전달한 뒤 힘을 적분하고 광원·카메라·축을 동기화한다.\n'
    architecture+='5. 사용자 update 콜백 후 Application은 Scene::updateMotions를 한 번 실행한다. Console::onRender → Assembly::render → renderScene 또는 renderViews → Renderer. 렌더 패스에서는 시간을 진행하지 않는다.\n'
    architecture+='6. Scene::worldMatrix는 객체의 부모 체인과 attachment/base/localLayers를 합성한다. faceWorldMatrix를 렌더·피킹이 함께 사용한다. Renderer는 CPU 정점/인덱스를 VAO/VBO/EBO로 업로드하고 모델·정규행렬·점광원 uniform을 전송한다. Shaders.h와 shaders/*.glsl이 GPU 입력·출력 계약이다.\n'
    architecture+='7. 종료 요청 후 run의 정리 경로가 GPU/창/GLFW 자원을 해제한다. ConsoleInput 소멸자는 원래 Windows 콘솔 입력 모드를 복원한다. 소유 객체와 비소유 콜백 참조의 수명은 Application/Console 범위에 종속된다.\n\n'
    architecture+='## 독립 실행 프로젝트와 main\n\n각 행은 별도 실행 프로그램이다. 서로 다른 프로젝트의 Console/main을 하나의 호출 흐름으로 합치지 않는다. 부재 파일은 과거 기록이며 복원하지 않았다.\n\n'
    for project in projects:
        label=project.get('project',project.get('file','example'))
        architecture+='### '+label+'\n\n'
        if project.get('available') is False:
            architecture+='작업 전 부재. 진입점 정보는 historical이며 현재 실행 경로를 확인할 수 없다.\n\n';continue
        entries=project.get('entrypoints',[])
        if not entries and project.get('standalone_example'):entries=[{'file':project['file'],'name':project.get('entrypoint','main')}]
        for entry in entries:
            file=entry['file'];line=entry.get('line','?')
            architecture+='- `'+file+':'+str(line)+'` · '+entry.get('name','main')
            if entry.get('conditional_compilation'):architecture+=' · 조건부 컴파일 분기는 소스의 전처리 조건별로 구분한다.'
            architecture+='\n'
            mains=[n for n in nodes.values() if n.get('source_file','').replace('\\','/')==file and n.get('label','').split('::')[-1] in ('main','wmain')]
            callees=[]
            for n in mains:
                for edge in links:
                    if edge.get('source')==n['id'] and edge.get('relation')=='calls':
                        target=nodes.get(edge.get('target'),{})
                        if target:callees.append(target.get('label','?')+' ('+target.get('source_file','?')+':'+target.get('source_location','?')+', '+str(edge.get('confidence','EXTRACTED'))+')')
            if callees:architecture+='- AST 직접 관계: '+'; '.join(sorted(set(callees)))+'. 콜백 본문의 지연 실행은 위상상 별도다.\n'
            else:architecture+='- 이 그래프에서 직접 호출을 확정할 관계가 부족하다. 원본 main의 등록 콜백·프레임 루프·해제 지점을 개별 확인한다.\n'
        architecture+='\n'
    architecture+='## 제한과 참조\n\n과거 실습 소스도 AST에 남으므로 노드 존재만으로 Standard 실행 경로에 포함됐다고 판단하지 않는다. PROJECTS.json은 vcxproj의 명시 Compile 목록과 Exclude를 반영한다. 그래프의 같은 이름·서로 다른 실행 파일 사이 호출 후보는 AMBIGUOUS로 구분한다. 실제 오버로드·함수 포인터·가상 호출 전체를 증명하지 않는다. 해시 manifest는 생성 당시의 소스를 가리킨다.\n\n'
    architecture+='[대화형 그래프](graph.html) · [파일·프로젝트](PROJECTS.json) · [소스 해시](SOURCE_MANIFEST.json)\n'
    write(graph_dir/'GRAPH_REPORT.md',architecture)
    write(OUT/'ARCHITECTURE.md',architecture)
    portable=STANDARD/'docs/assembly/architecture'
    portable.mkdir(parents=True,exist_ok=True)
    for name in ('graph.json','graph.html','PROJECTS.json','SOURCE_MANIFEST.json','GRAPH_REPORT.md'):
        (portable/name).write_bytes((graph_dir/name).read_bytes())
    write(portable/'README.md','# 구조 분석\n\n현재 Standard 실행 흐름과 독립 프로젝트 구분은 [GRAPH_REPORT](GRAPH_REPORT.md)를 따른다. AST는 실행 확인이 아니다.\n')
    candidates=[]
    for p in sorted((ROOT/'tools/__pycache__').glob('*.pyc')):
        rel=p.relative_to(ROOT).as_posix()
        if rel in new:candidates.append((p,p.stat().st_size))
    cleanup='# 임시 파일 후보\n\n문서 생성기의 Python import가 만든 캐시다. 제품 실행에는 필요하지 않으며 Python이 다시 생성할 수 있다. 별도 삭제 승인이 없어 보존했다.\n\n| 정확한 경로 | 바이트 | 보존 이유 |\n|---|---:|---|\n'
    for path,size in candidates:cleanup+='| `'+str(path)+'` | '+str(size)+' | 삭제 승인 대기 |\n'
    cleanup+='\n기존 docs.zip, dependencies, 실행/설정/사용자 데이터, graphify 증분 캐시는 보존했다. 그래프 캐시는 후속 분석에 사용한다.\n'
    write(OUT/'CLEANUP_CANDIDATES.md',cleanup)
    write(OUT/'RESULT.md','''# 반영 결과와 확인 경계

- 승인된 0번 작업공간, 비균등 배율·부착·지역 레이어·피킹·단축키·광원·카메라·힘 연결을 소스에 반영했다.
- 실습 재출제를 가정하지 않는 연산별 템플릿 9개와 요구 문장 → 변환·공간·API 레퍼런스를 작성했다.
- 저울은 공통 변환과 힘을 조합한 영자연길이 선형 스프링의 응용 예제로 구분했다.
- 콘솔 도움말은 명령어 [결과] - 각 인수 의미 형식이다. 실행 결과에는 해석 인수와 현재 선택·월드 원점을 표시한다.
- API 카탈로그의 기존 pick 오버로드 앵커를 유지하고 새 공개 선언을 추가했다. 문서·그래프 생성기를 실행했다.
- 소스 검토와 산출물 생성만 수행했다. 빌드·테스트·검증기·앱·브라우저 실행은 하지 않았다. 수치 안정성·피킹/화면·셰이더·MSVC 컴파일은 사용자 확인 사항이다.
- 실제 절차와 예상 관찰점은 standard/USER_CHECKS.md에 있다. 외부 side effect가 있는 사용자 콜백, 충돌·마찰·강체 제약 솔버는 자동 처리하지 않는다.

## 단계별 커밋

1. c9fc9f4: 0번 독립 작업공간과 문서 런타임 분리.
2. 0c6aa6f: 월드 부착·비균등 변환·지역 레이어·피킹/단축키·명시적 저장.
3. d929c64: 점광원·카메라 객체와 셰이더.
4. 4e1a328: 이동 기준 프레임의 힘 API와 스프링 저울 조합.
5. 문서·템플릿·그래프·연결부 보완: 이 보고서를 포함한 최종 커밋이며 해시는 최종 응답에서 제공한다.

[전체 코드 before/after](ALL_CODE_BEFORE_AFTER.md) · [변경 파일](CHANGE_MANIFEST.json) · [실행 구조](ARCHITECTURE.md) · [정리 후보](CLEANUP_CANDIDATES.md)
''')
    # Hash the final outputs after generating them; the manifest excludes its own recursive hash.
    final_names=set(changed)
    final_names.update(p for p in git('ls-files','--others','--exclude-standard').splitlines() if p.startswith(allowed) and '/__pycache__/' not in p)
    final_manifest=[]
    for name in sorted(final_names):
        if name=='docs/workspace-redesign/CHANGE_MANIFEST.json':continue
        path=ROOT/name;data=path.read_bytes() if path.exists() else b''
        final_manifest.append({'path':name,'new':name not in baseline_files,'exists':path.exists(),'bytes':len(data),'sha256':hashlib.sha256(data).hexdigest() if data else None})
    write(OUT/'CHANGE_MANIFEST.json',json.dumps({'baseline':BASE,'source_state':'Saved working tree during step 5; docs.zip and manifest self-hash excluded.','files':final_manifest},ensure_ascii=False,indent=2))
    print('Wrote source before/after, manifests, architecture and execution limits. No build/tests run.')

if __name__=='__main__':main()
````
