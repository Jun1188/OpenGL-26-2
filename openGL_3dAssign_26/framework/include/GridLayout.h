#pragma once
#include "Transform.h"

namespace graphics {
// origin/extent are grid-local. frame converts grid-local coordinates to world.
struct GridLayout {
    glm::vec3 origin{0}, extent{1};
    glm::ivec3 cells{1};
    glm::mat4 frame{1};


    /// [API] origin·extent·셀 개수와 frame의 유한성·양수 크기·가역성을 검사한다.
    bool valid() const {
        const float determinant = glm::determinant(glm::mat3(frame));

        return detail::finiteVector(origin) && detail::finiteVector(extent) && extent.x > 0 && extent.y > 0 &&
               extent.z > 0 && cells.x > 0 && cells.y > 0 && cells.z > 0 && TransformSystem::validAffine(frame) &&
               std::isfinite(determinant) && std::abs(determinant) > 1e-8f;
    }


    /// [API] [0,1] 비율을 origin+extent*ratio로 바꾼 뒤 frame으로 월드 위치를 계산한다.
    std::optional<glm::vec3> position(const glm::vec3& ratio) const {
        if (!valid() || !detail::finiteVector(ratio) || ratio.x < 0 || ratio.y < 0 || ratio.z < 0 || ratio.x > 1 ||
            ratio.y > 1 || ratio.z > 1) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "GridLayout::position",
                                "Need a valid grid and ratios in [0,1].");

            return {};
        }

        const auto result = glm::vec3(frame * glm::vec4(origin + extent * ratio, 1));

        if (!detail::finiteVector(result)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "GridLayout::position",
                                "Grid transformation overflow.");

            return {};
        }

        return result;
    }


    /// [API] 0부터 시작하는 셀 인덱스를 해당 셀 중심의 월드 위치로 변환한다.
    std::optional<glm::vec3> cellCenter(const glm::ivec3& index) const {
        if (!valid() || index.x < 0 || index.y < 0 || index.z < 0 || index.x >= cells.x || index.y >= cells.y ||
            index.z >= cells.z) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "GridLayout::cellCenter",
                                "Cell index is outside the grid.");

            return {};
        }

        return position((glm::vec3(index) + glm::vec3(.5f)) / glm::vec3(cells));
    }


    /// [API] 양수 비율과 extent를 곱해 그리드 로컬 크기를 반환한다. frame 회전/배율은 적용하지 않는다.
    std::optional<glm::vec3> relativeSize(const glm::vec3& ratio) const {
        if (!valid() || !detail::finiteVector(ratio) || ratio.x <= 0 || ratio.y <= 0 || ratio.z <= 0) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "GridLayout::relativeSize",
                                "Need positive finite size ratios.");

            return {};
        }

        const auto result = extent * ratio;

        if (!detail::finiteVector(result)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "GridLayout::relativeSize",
                                "Size overflow.");

            return {};
        }

        return result;
        // Local dimensions. frame orientation is kept separate.
    }


    /// [API] 비율 위치를 월드로 계산한 뒤 parentWorld의 역행렬로 부모 로컬 위치를 반환한다.
    std::optional<glm::vec3> positionInParent(const glm::vec3& ratio, const glm::mat4& parentWorld) const {
        const auto world = position(ratio);
        const float determinant = glm::determinant(glm::mat3(parentWorld));

        if (!world || !TransformSystem::validAffine(parentWorld) || !std::isfinite(determinant) ||
            std::abs(determinant) < 1e-8f) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "GridLayout::positionInParent",
                                "Parent transform must be finite, affine and invertible.");

            return {};
        }

        const auto result = glm::vec3(glm::inverse(parentWorld) * glm::vec4(*world, 1));

        if (!detail::finiteVector(result)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "GridLayout::positionInParent",
                                "Parent inverse overflow.");

            return {};
        }

        return result;
    }


    /// [API] 월드 점을 frame 역변환으로 그리드 셀 인덱스에 대응시킨다. 그리드 밖이면 빈 결과다.
    std::optional<glm::ivec3> cellAt(const glm::vec3& world) const {
        if (!valid() || !detail::finiteVector(world)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "GridLayout::cellAt",
                                "Invalid grid or world point.");

            return {};
        }

        const auto local = glm::vec3(glm::inverse(frame) * glm::vec4(world, 1));
        const auto ratio = (local - origin) / extent;

        if (!detail::finiteVector(ratio) || ratio.x < 0 || ratio.y < 0 || ratio.z < 0 || ratio.x > 1 || ratio.y > 1 ||
            ratio.z > 1)
            return {};

        return glm::min(glm::ivec3(glm::floor(ratio * glm::vec3(cells))), cells - glm::ivec3(1));
    }
};
} // namespace graphics
