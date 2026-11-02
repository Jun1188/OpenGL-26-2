#pragma once
#include "Model.h"
#include <functional>

namespace graphics {

struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

using PathFunction = std::function<glm::vec3(double)>;
using InterpolationFunction = std::function<double(double)>;

// These functions return no matrix/path when their parameters are invalid.
class TransformSystem {
public:
    static bool validAffine(const glm::mat4& matrix);
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    static std::optional<glm::mat4> scaling(const glm::vec3& factors, const glm::vec3& center = glm::vec3(0));
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point, const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU, const glm::vec3& axisV);
    static PathFunction circle(const glm::vec3& center, float radius, const glm::vec3& normal = glm::vec3(0, 1, 0));
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    static PathFunction polygon(std::vector<glm::vec3> points);
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU, const glm::vec3& halfV);
    static double linear(double progress);
    static double smoothStep(double progress);
};
} // namespace graphics
