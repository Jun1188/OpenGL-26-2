#pragma once
#include "../include/Model.h"
#include <initializer_list>

// Shared declarations for separately compiled framework implementation files.
namespace graphics::detail {
std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction);
std::optional<glm::mat4> motionAround(const glm::vec3& center, const glm::mat4& matrix);
bool motionPlane(const glm::vec3& u, const glm::vec3& v);
bool positiveLengths(const glm::vec3& value);
std::uint64_t nextGeometryRevision();
void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color);
} // namespace graphics::detail
