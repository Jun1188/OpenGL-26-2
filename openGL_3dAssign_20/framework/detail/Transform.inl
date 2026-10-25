#pragma once
#include <algorithm>

namespace graphics {
namespace detail {
inline std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction)) return {};
    const double length = glm::length(glm::dvec3(direction));
    if (!std::isfinite(length) || length <= 1e-12) return {};
    return glm::vec3(glm::dvec3(direction) / length);
}
inline std::optional<glm::mat4> motionAround(const glm::vec3& center,
                                           const glm::mat4& matrix) {
    if (!finiteVector(center)) return {};
    const auto result = glm::translate(glm::mat4(1), center) * matrix
                      * glm::translate(glm::mat4(1), -center);
    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result)
                                                : std::nullopt;
}
inline bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);
    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

inline bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0
        && m[2][3] == 0 && m[3][3] == 1;
}
inline std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset)) return {};
    return glm::translate(glm::mat4(1), offset);
}
inline std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors,
                                                       const glm::vec3& center) {
    if (!detail::finiteVector(factors)) return {};
    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}
inline std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis,
                                                        double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);
    if (!unit || !std::isfinite(degrees)) return {};
    const float radians = static_cast<float>(std::remainder(degrees, 360.0)
                                            * 3.14159265358979323846 / 180.0);
    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}
inline std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}
inline std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);
    if (!unit) return {};
    glm::mat3 matrix(0);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);
    return detail::motionAround(line.point, glm::mat4(matrix));
}
inline std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point,
                                                            const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);
    if (!unit) return {};
    glm::mat3 matrix(1);
    for (int c = 0; c < 3; ++c) for (int r = 0; r < 3; ++r)
        matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];
    return detail::motionAround(point, glm::mat4(matrix));
}
inline PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u,
                                       const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;
        return center + u * static_cast<float>(std::cos(angle))
                      + v * static_cast<float>(std::sin(angle));
    };
}
inline PathFunction PathSystem::circle(const glm::vec3& center, float radius,
                                      const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);
    if (!n || !std::isfinite(radius) || radius <= 0) return {};
    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));
    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}
inline PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2) return {};
    for (const auto& p : points) if (!detail::finiteVector(p)) return {};
    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());
        if (points.size() < 4) return {};
    }
    std::vector<double> distances{0};
    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));
        if (!std::isfinite(length) || length <= 1e-12) return {};
        distances.push_back(distances.back() + length);
    }
    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1
                                                   : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);
        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight)
                       + glm::dvec3(points[i]) * weight);
    };
}
inline PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}
inline PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u,
                                         const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v)) return {};
    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);
    if (std::abs(glm::dot(*a, *b)) > 1e-5f) return {};
    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}
inline double PathSystem::linear(double progress) { return progress; }
inline double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics
