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
