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
    /// [API] 행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.
    static bool validAffine(const glm::mat4& matrix);
    /// [API] 이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    /// [API] 중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.
    static std::optional<glm::mat4> scaling(const glm::vec3& factors, const glm::vec3& center = glm::vec3(0));
    /// [API] 직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    /// [API] 주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    /// [API] 주어진 직선을 기준으로 선 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    /// [API] 평면의 점과 법선으로 평면 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point, const glm::vec3& normal);
};

class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    /// [API] 중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU, const glm::vec3& axisV);
    /// [API] 중심·반지름·평면 법선으로 원 경로 함수를 만든다.
    static PathFunction circle(const glm::vec3& center, float radius, const glm::vec3& normal = glm::vec3(0, 1, 0));
    /// [API] 점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    /// [API] 마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.
    static PathFunction polygon(std::vector<glm::vec3> points);
    /// [API] 중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU, const glm::vec3& halfV);
    /// [API] 입력 진행률을 선형 보간 진행률로 반환한다.
    static double linear(double progress);
    /// [API] 진행률에 3t²-2t³ 완화 곡선을 적용한다.
    static double smoothStep(double progress);
};
} // namespace graphics
