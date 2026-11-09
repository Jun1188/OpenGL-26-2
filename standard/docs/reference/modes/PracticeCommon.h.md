# 과거 구현 참조: PracticeCommon.h

- 원래 위치: `standard/modes/PracticeCommon.h`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

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
