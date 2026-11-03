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
