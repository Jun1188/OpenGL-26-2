// OpenGL Practice 28 Framework - Single-File Standalone Edition
// Dependencies: GLEW (runtime loader), GLFW (delay-loaded DLL), GLM, Windows SDK (opengl32.lib, delayimp.lib, ole32.lib, windowscodecs.lib)
// C++17+, Windows x64, MSVC default encoding (CP949/ANSI) or /utf-8
//
// [빠른 시작] main.cpp / OpenGL.props / 수동 DLL 복사 없이 gllib.cpp + dependencies\ 만으로 실행 가능:
//   1) VS 프로젝트 플랫폼: x64 / C++ 언어 표준: ISO C++17(/std:c++17) 이상
//   2) 이 파일을 프로젝트 소스 파일로 직접 포함해 빌드 (파일 하단에 시험장 가이드 및 main() 포함)
//   3) 런타임 DLL(glew32.dll, glfw3.dll)은 dependencies\dll\ 에서 자동 매핑·로드됨
//   * 외부 main.cpp에서 #include "gllib.cpp"로 쓸 때는 #define GLLIB_NO_MAIN 먼저 선언

#ifndef _WIN32
#error This framework targets Windows x64.
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#if __has_include(<GL/glew.h>)
#include <GL/glew.h>
#elif __has_include("dependencies/glew-2.3.1/include/GL/glew.h")
#include "dependencies/glew-2.3.1/include/GL/glew.h"
#elif __has_include("../dependencies/glew-2.3.1/include/GL/glew.h")
#include "../dependencies/glew-2.3.1/include/GL/glew.h"
#else
#error "GLEW header not found. Place dependencies/ next to gllib.cpp or configure AdditionalIncludeDirectories."
#endif

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#ifndef GLFW_DLL
#define GLFW_DLL
#endif
#if __has_include(<GLFW/glfw3.h>)
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3dll.lib")
#elif __has_include("dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h")
#include "dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#pragma comment(lib, "dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib")
#elif __has_include("../dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h")
#include "../dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#pragma comment(lib, "../dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib")
#else
#error "GLFW header not found. Place dependencies/ next to gllib.cpp or configure AdditionalIncludeDirectories."
#endif
#pragma comment(lib, "delayimp.lib")
#pragma comment(linker, "/DELAYLOAD:glfw3.dll")

#if __has_include(<glm/glm.hpp>)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#elif __has_include("dependencies/glm/glm/glm.hpp")
#include "dependencies/glm/glm/glm.hpp"
#include "dependencies/glm/glm/gtc/matrix_transform.hpp"
#include "dependencies/glm/glm/gtc/type_ptr.hpp"
#elif __has_include("../dependencies/glm/glm/glm.hpp")
#include "../dependencies/glm/glm/glm.hpp"
#include "../dependencies/glm/glm/gtc/matrix_transform.hpp"
#include "../dependencies/glm/glm/gtc/type_ptr.hpp"
#else
#error "GLM header not found. Place dependencies/ next to gllib.cpp or configure AdditionalIncludeDirectories."
#endif

#include <wincodec.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "windowscodecs.lib")

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

static_assert(sizeof(void*) == 8, "x64 build required.");

// --- Diagnostics.h ---

namespace graphics {
enum class PipelineStage { Runtime, Input, Scene, Motion, VertexInput, Shader, Transform, Raster, Framebuffer, Asset };

struct DiagnosticOptions {
    bool errors = true;
    bool trace = false;
};

struct Diagnostic {
    PipelineStage stage = PipelineStage::Runtime;
    std::string api, cause, action;
    std::optional<std::uint64_t> object;
    std::string file, operation;
    int line = 0;
};

class Diagnostics {
    static DiagnosticOptions& options() {
        static DiagnosticOptions value;

        return value;
    }

    static std::optional<Diagnostic>& issue() {
        static std::optional<Diagnostic> value;

        return value;
    }

public:
    /// [API] 진단에 첨부할 현재 파일·줄·호출 경로 문맥의 참조를 반환한다.
    static Diagnostic& context() {
        static Diagnostic value;

        return value;
    }

    /// [API] 다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.
    static void locate(const char* file, int line, const char* operation) {
        context().file = file;
        context().line = line;
        context().operation = operation;
    }

    /// [API] 주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.
    static bool failAt(const char* file, int line, PipelineStage stage, const char* api, const std::string& cause,
                       const std::string& action = "소스 위치와 해당 API의 입력 조건을 확인하세요.",
                       std::optional<std::uint64_t> object = std::nullopt) {
        const auto saved = context();

        locate(file, line, api);

        if (!saved.operation.empty())
            context().operation = saved.operation + " -> " + api;

        const bool result = fail(stage, api, cause, action, object);

        context() = saved;

        return result;
    }

    /// [API] 파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.
    static const char* stageName(PipelineStage stage) {
        switch (stage) {
            case PipelineStage::Runtime:
            return "runtime";
        case PipelineStage::Input:
            return "input";
        case PipelineStage::Scene:
            return "scene";
        case PipelineStage::Motion:
            return "motion";
        case PipelineStage::VertexInput:
            return "vertex-input";
        case PipelineStage::Shader:
            return "shader";
        case PipelineStage::Transform:
            return "transform";
        case PipelineStage::Raster:
            return "raster";
        case PipelineStage::Framebuffer:
            return "framebuffer";
        case PipelineStage::Asset:
            return "asset";
        }

        return "알 수 없음";
    }

    /// [API] 오류 출력과 단계 추적 출력의 사용 여부를 설정한다.
    static void configure(DiagnosticOptions value) {
        options() = value;
    }

    /// [API] 마지막으로 기록한 오류의 optional 참조를 반환한다.
    static const std::optional<Diagnostic>& lastError() {
        return issue();
    }

    /// [API] 마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.
    static void clear() {
        issue().reset();
    }

    /// [API] 추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.
    static void trace(PipelineStage stage, const char* api) {
        if (options().trace)
            std::clog << "[OpenGL][" << stageName(stage) << "][" << api << "] 시작\n";
    }

    /// [API] 현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.
    static bool fail(
        PipelineStage stage, const char* api, const std::string& cause,
        const std::string& action = "framework/docs/API_KO.md에서 해당 API의 입력 조건과 반환값을 확인하세요.",
        std::optional<std::uint64_t> object = std::nullopt) {
        issue() = Diagnostic{stage, api, cause, action, object};
        issue()->file = context().file;
        issue()->line = context().line;
        issue()->operation = context().operation;

        if (options().errors) {
            std::cerr << "[OpenGL][" << stageName(stage) << "][" << api << "] 오류";

            if (object)
                std::cerr << " 객체=" << *object;
            std::cerr << ": " << cause << "\n  조치: " << action << '\n';
            std::cerr << "  소스: " << context().file << ':' << context().line
                      << "\n  호출 경로: " << context().operation << '\n';
        }

        return false;
    }
};
} // namespace graphics

// --- Model.h ---

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
// Initial allocation hints, not scene capacity limits.
constexpr std::size_t MAX_ENTITIES = 50;
constexpr std::size_t MAX_LINES = 500;

using EntityId = std::size_t;
using LineId = std::size_t;

struct GlColor {
    float red = 1.0f, green = 1.0f, blue = 1.0f;
};
enum class GLShape { None, Cube, Tetra, Pyramid, Custom, Imported };
enum class GLFace { None, Rect, Triangle, Mesh };
enum class PivotMode { Preserve, BottomCenter, Custom };
enum class OutlineMode { None, FeatureEdges, AllTriangleEdges, Explicit };
enum class CullMode { Inherit, None, Back, Front };
enum class FaceTransformMode { Hierarchy, Independent };
enum class FaceAnimationSpace { Local, Model };

struct Transform3D {
    glm::vec3 basis{0.0f};
    glm::vec3 localBasis{0.0f};
    glm::mat3 rotation{1.0f};
    glm::vec3 scale{1.0f};
};

struct TextureImage {
    std::uint32_t width = 0, height = 0;
    std::vector<std::uint8_t> rgba;

    // Row zero maps to UV v=0. File decoding flips top-down images by default.
    /// [API] 이미지 너비·높이와 RGBA 바이트 수가 맞는지 검사한다.
    bool valid() const {
        return width && height && width <= 16384 && height <= 16384 && rgba.size() == std::size_t(width) * height * 4;
    }
};

struct Material {
    GlColor color;
    std::shared_ptr<const TextureImage> texture;
    bool useTexture = false;
    bool repeatTexture = true;
    float ambientStrength = .25f, diffuseStrength = 1;
    float specularStrength = 0, shininess = 32;
    bool unlit = false;
};

struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f};
    glm::vec2 uv{0.0f};
    GlColor color;
};

struct PlaneFigure {
    GLFace shape = GLFace::Mesh;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{0.0f};
    // Compatibility flag: true enables child-local SRT, never world detachment.
    bool independence = false;
    Transform3D transform;
    bool visible = true;
    glm::mat4 localAnimationMatrix{1.0f};
    std::vector<GlColor> vertexColors;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> texcoords;
    std::vector<std::uint32_t> triangleIndices;
    std::vector<std::uint32_t> outlineIndices; // GL_LINES pairs, mesh-local indices.
    Material material;
    OutlineMode outline = OutlineMode::None;
    float creaseAngleDegrees = 1.0f;
    std::string name;
    CullMode cull = CullMode::Inherit;
    FaceTransformMode transformMode = FaceTransformMode::Hierarchy;
    glm::mat4 transformFrame{1.0f}; // Parent-local correction, or a frozen world frame.
    FaceAnimationSpace animationSpace = FaceAnimationSpace::Local;

    /// [API] 정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& triangles) {
        PlaneFigure mesh;

        mesh.triangleIndices = triangles;

        for (const auto& vertex : vertices) {
            mesh.localVertices.push_back(vertex.position);
            mesh.vertexNormals.push_back(vertex.normal);
            mesh.texcoords.push_back(vertex.uv);
            mesh.vertexColors.push_back(vertex.color);
        }

        return mesh;
    }
};

using Mesh = PlaneFigure;

struct Polyhedron {
    GLShape shape = GLShape::Custom;
    std::vector<PlaneFigure> faces;
    std::string name;

    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    std::vector<Mesh>& meshes() {
        return faces;
    }

    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const std::vector<Mesh>& meshes() const {
        return faces;
    }
};

using Model = Polyhedron;

struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{1.0f};
    bool visible = true;
    PivotMode pivotMode = PivotMode::Preserve;
    std::optional<EntityId> parent; // Local SRT is relative to this entity.
    GlColor tint;                   // Per-entity multiplier; does not propagate to children.
    bool transformOnly = false;
    glm::mat4 attachment{1.0f}; // Object-owned affine placement, including inherited shear.
    std::vector<glm::mat4> localLayers; // Ordered, object-local transformations.
};

struct FaceId {
    EntityId entityId = 0;
    std::size_t faceIndex = 0;
};

struct Line3D {
    glm::vec3 start{0.0f}, end{0.0f};
    GlColor color;
    std::optional<EntityId> owner;
};

namespace detail {
inline bool finiteVector(const glm::vec3& v) {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}

inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            if (!std::isfinite(m[c][r]))
                return false;

    return true;
}

inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}

inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
    if (!mesh.triangleIndices.empty())
        return mesh.triangleIndices;

    if (mesh.shape == GLFace::Triangle)
        return {0, 1, 2};

    if (mesh.shape == GLFace::Rect)
        return {0, 1, 2, 0, 2, 3};

    return {};
}

inline bool validMesh(const Mesh& m) {
    if (m.cull != CullMode::Inherit && m.cull != CullMode::None && m.cull != CullMode::Back &&
        m.cull != CullMode::Front)
        return false;

    if ((m.transformMode != FaceTransformMode::Hierarchy && m.transformMode != FaceTransformMode::Independent) ||
        !finiteMatrix(m.transformFrame) ||
        (m.animationSpace != FaceAnimationSpace::Local && m.animationSpace != FaceAnimationSpace::Model))
        return false;

    if (m.localVertices.size() < 3 || !validColor(m.color) || !validColor(m.material.color) ||
        !finiteVector(m.normal) || !finiteMatrix(m.localAnimationMatrix) || !std::isfinite(m.creaseAngleDegrees) ||
        m.creaseAngleDegrees < 0 || m.creaseAngleDegrees > 180)
        return false;

    const auto& material = m.material;
    for (float value : {material.ambientStrength, material.diffuseStrength, material.specularStrength})
        if (!std::isfinite(value) || value < 0 || value > 1)
            return false;
    if (!std::isfinite(material.shininess) || material.shininess < 1 || material.shininess > 1024)
        return false;

    const auto count = m.localVertices.size();

    if (count > std::numeric_limits<std::uint32_t>::max())
        return false;

    if ((!m.vertexColors.empty() && m.vertexColors.size() != count) ||
        (!m.vertexNormals.empty() && m.vertexNormals.size() != count) ||
        (!m.texcoords.empty() && m.texcoords.size() != count) || (m.material.texture && !m.material.texture->valid()) ||
        (m.material.useTexture && (!m.material.texture || m.texcoords.empty())))
        return false;

    for (const auto& v : m.localVertices)
        if (!finiteVector(v))
            return false;

    for (const auto& v : m.vertexNormals)
        if (!finiteVector(v))
            return false;

    for (const auto& v : m.texcoords)
        if (!std::isfinite(v.x) || !std::isfinite(v.y))
            return false;

    for (auto c : m.vertexColors)
        if (!validColor(c))
            return false;

    const auto indices = triangles(m);

    if (indices.empty() || indices.size() % 3 || m.outlineIndices.size() % 2)
        return false;

    for (auto i : indices)
        if (i >= count)
            return false;

    for (auto i : m.outlineIndices)
        if (i >= count)
            return false;

    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = glm::dvec3(m.localVertices[indices[i]]);
        const auto b = glm::dvec3(m.localVertices[indices[i + 1]]);
        const auto c = glm::dvec3(m.localVertices[indices[i + 2]]);
        const auto normal = glm::cross(b - a, c - a);
        const double area = glm::dot(normal, normal);

        if (!std::isfinite(area) || area <= 0)
            return false;
    }

    return true;
}

inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());

    for (const auto& face : model.faces)
        for (const auto& p : face.localVertices) {
            low = glm::min(low, p);
            high = glm::max(high, p);
        }

    return {low, high};
}

inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);

    return {box.first.x * .5f + box.second.x * .5f, box.first.y, box.first.z * .5f + box.second.z * .5f};
}

inline glm::mat4 transformMatrix(const Transform3D& t) {
    glm::mat4 m = glm::translate(glm::mat4(1), t.basis);

    m *= glm::mat4(t.rotation);

    return glm::translate(glm::scale(m, t.scale), -t.localBasis);
}

inline glm::mat4 faceSrtMatrix(const Mesh& face) {
    return face.independence
        ? glm::translate(glm::mat4(1), face.transform.localBasis) * transformMatrix(face.transform)
        : glm::mat4(1);
}

inline glm::mat4 faceLocalMatrix(const Mesh& face) {
    const auto child = faceSrtMatrix(face);
    return face.animationSpace == FaceAnimationSpace::Model
        ? face.localAnimationMatrix * child : child * face.localAnimationMatrix;
}

inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
    const auto parent = face.transformMode == FaceTransformMode::Hierarchy ? transformMatrix(c.transform) : glm::mat4(1);
    return parent * face.transformFrame * faceLocalMatrix(face);
}

} // namespace detail
} // namespace graphics

// --- Transform.h ---

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

// --- Motion.h ---

namespace graphics {
using MotionId = std::uint64_t;
using LayerId = std::uint64_t;
enum class TransformSpace { World, Local };
enum class Playback { Once, Loop, PingPong };

struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};

using MotionAction = std::function<bool(double, const MotionSample&)>;

struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};

struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};

struct LinearMotion {
    glm::vec3 from{0}, to{1, 0, 0};
    double durationSeconds = 1;
    Playback playback = Playback::Once;
    TransformSpace space = TransformSpace::Local;
};

struct PendulumMotion {
    AxisLine axis{{0, 0, 0}, {1, 0, 0}};
    double amplitudeDegrees = 25;
    double periodSeconds = 3;
    double phaseDegrees = 0;
    double offsetDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};

struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
    MotionAction action; // A step contains either transform or action.
};

class Motion {
    std::vector<MotionStep> steps_;

    static bool validStep(const MotionStep& step);

    std::optional<MotionSample> evaluate(double seconds, bool actions) const;

public:
    /// [API] 모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.
    bool append(const MotionStep& step);

    /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
    bool insert(std::size_t index, const MotionStep& step);

    /// [API] 변환 사이의 인덱스에 의미 동작 또는 람다·함수 포인터를 삽입한다.
    bool insertAction(std::size_t index, MotionAction action, double startSeconds = 0,
                      std::optional<double> durationSeconds = std::nullopt);

    /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
    bool replace(std::size_t index, const MotionStep& step);

    /// [API] 주어진 인덱스의 모션 단계를 삭제한다.
    bool erase(std::size_t index);

    /// [API] 모든 앞 단계의 유한 종료 시각을 구한 뒤 새 단계의 시작을 그 뒤로 이동해 순차 실행을 만든다.
    bool then(MotionStep step); // Requires all previous steps to have finite durations.

    /// [API] 등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.
    const std::vector<MotionStep>& getSteps() const {
        return steps_;
    }

    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다. 의미 동작은 실행하지 않는다.
    std::optional<MotionSample> sample(double seconds) const;

    /// [API] 변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.
    std::optional<MotionSample> execute(double seconds) const;

    /// [API] 기존 append/insert/then/replace와 조립하는 비변환 동작 단계를 만든다.
    static MotionStep action(MotionAction action, double startSeconds = 0,
                             std::optional<double> durationSeconds = std::nullopt);

    /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
    static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);

    /// [API] 축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.
    static MotionStep rotation(const RotationMotion& config);

    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);

    /// [API] 시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.
    static MotionStep linear(const LinearMotion& config);

    /// [API] 지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.
    static MotionStep pendulum(const PendulumMotion& config);
};

struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};

struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;

    bool validLayer(const LayerConfig& config) const;

public:
    /// [API] 삭제할 수 없는 루트 레이어의 ID 0을 반환한다.
    static constexpr LayerId rootLayer() {
        return 0;
    }

    /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
    std::optional<MotionId> createMotion(const Motion& motion);

    /// [API] 의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.
    std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);

    /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
    bool setMotion(MotionId id, const Motion& motion);

    /// [API] 모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const MotionState* getMotion(MotionId id) const;

    /// [API] 모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.
    bool setMotionPlaying(MotionId id, bool playing);

    /// [API] 모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.
    bool setMotionEnabled(MotionId id, bool enabled);

    /// [API] 지정 시각을 샘플링하고 유효할 때 모션의 시간과 샘플을 함께 바꾼다.
    bool seekMotion(MotionId id, double seconds);

    /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
    bool removeMotion(MotionId id);

    /// [API] 모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.
    bool update(double deltaSeconds);

    /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
    void clear(); // Identifiers are never reused by this system, including after clear.

    /// [API] 유효한 부모 아래 새 자식 레이어를 추가한다. 복사한 임시 맵에 반영한 뒤 swap하여 상태를 확정한다.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});

    /// [API] 기존 child와 그 parent 사이에 새 레이어를 끼운다. root 위 삽입은 허용하지 않는다.
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});

    /// [API] 자식이 없는 비root leaf만 제거하고 그 부모 ID를 반환한다.
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.

    /// [API] root 이외 레이어를 제거하며 자식들을 부모의 해당 위치로 승격한다. 월드 자세 보존을 자동 약속하지 않는다.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.

    /// [API] 레이어의 부모를 바꾸며 자신의 자손으로 연결되는 순환을 거부한다.
    bool reparentLayer(LayerId id, LayerId parent);

    /// [API] 기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.
    bool setLayer(LayerId id, const LayerConfig& config);

    /// [API] 레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const LayerState* getLayer(LayerId id) const;

    /// [API] 기존 레이어에 모션 ID를 연결하거나 해제한다. 모션과 레이어가 존재하는지 검사한다.
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);

    /// [API] leaf까지의 계층을 따라 transform과 motion 샘플을 World/Local 공간에 맞게 parent/local 행렬로 합성한다.
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;

    /// [API] 로컬 공간 회전 모션을 등록하여 자전에 사용한다.
    std::optional<MotionId> createSpin(RotationMotion config);

    /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
    std::optional<MotionId> createOrbit(RotationMotion config);

};
} // namespace graphics

// --- Geometry.h ---

namespace graphics {
enum class UVMapping { Planar, Box, Cylindrical, Spherical };

struct UVOptions {
    UVMapping mapping = UVMapping::Planar;
    int axis = 2; // Planar projection drops X=0, Y=1, or Z=2.
    glm::vec2 scale{1};
    glm::vec2 offset{0};
    float rotationDegrees = 0;
};

/// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
inline bool transformTexcoords(Mesh& mesh, const glm::vec2& scale, const glm::vec2& offset, float rotationDegrees = 0) {
    if (mesh.texcoords.size() != mesh.localVertices.size() || mesh.texcoords.empty() || !std::isfinite(scale.x) ||
        !std::isfinite(scale.y) || !std::isfinite(offset.x) || !std::isfinite(offset.y) ||
        !std::isfinite(rotationDegrees))
        return false;

    auto uvs = mesh.texcoords;
    const double angle = glm::radians(double(rotationDegrees));
    const double c = std::cos(angle), s = std::sin(angle);

    for (auto& uv : uvs) {
        const auto p = glm::dvec2(uv) * glm::dvec2(scale);

        uv = glm::vec2(c * p.x - s * p.y, s * p.x + c * p.y) + offset;

        if (!std::isfinite(uv.x) || !std::isfinite(uv.y))
            return false;
    }
    mesh.texcoords = std::move(uvs);

    return true;
}

/// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
inline bool generateTexcoords(Mesh& mesh, const UVOptions& options = UVOptions{}) {
    if (!detail::validMesh(mesh) || options.axis < 0 || options.axis > 2)
        return false;

    switch (options.mapping) {
        case UVMapping::Planar:
    case UVMapping::Box:
    case UVMapping::Cylindrical:
    case UVMapping::Spherical:
        break;
    default:
        return false;
    }

    Mesh result = mesh;
    glm::dvec3 low(mesh.localVertices[0]), high(low);

    for (const auto& p : mesh.localVertices) {
        low = glm::min(low, glm::dvec3(p));
        high = glm::max(high, glm::dvec3(p));
    }

    const auto size = high - low, center = (low + high) * 0.5;
    const double pi = std::acos(-1.0);
    auto project = [&](std::uint32_t index, int axis) {
        const auto p = glm::dvec3(mesh.localVertices[index]);

        if (options.mapping == UVMapping::Planar || options.mapping == UVMapping::Box) {
            const int u = axis == 0 ? 2 : 0, v = axis == 1 ? 2 : 1;

            return glm::vec2(size[u] > 0 ? (p[u] - low[u]) / size[u] : 0.5,
                             size[v] > 0 ? (p[v] - low[v]) / size[v] : 0.5);
        }

        const auto d = p - center;
        const double radius = glm::length(d);

        return glm::vec2(std::atan2(d.z, d.x) / (2 * pi) + 0.5,
                         options.mapping == UVMapping::Spherical
                             ? (radius > 0 ? std::asin(std::clamp(d.y / radius, -1.0, 1.0)) / pi + 0.5 : 0.5)
                             : (size.y > 0 ? (p.y - low.y) / size.y : 0.5));
    };

    if (options.mapping == UVMapping::Planar) {
        result.texcoords.clear();

        for (std::uint32_t i = 0; i < mesh.localVertices.size(); ++i)
            result.texcoords.push_back(project(i, options.axis));
    } else {
        const auto indices = detail::triangles(mesh);

        if (indices.size() > std::numeric_limits<std::uint32_t>::max())
            return false;
        result.localVertices.clear();
        result.vertexNormals.clear();
        result.vertexColors.clear();
        result.texcoords.clear();
        result.triangleIndices.clear();
        result.outlineIndices.clear();
        result.shape = GLFace::Mesh;

        std::vector<std::uint32_t> first(mesh.localVertices.size(), std::numeric_limits<std::uint32_t>::max());

        for (std::size_t t = 0; t < indices.size(); t += 3) {
            const auto a = glm::dvec3(mesh.localVertices[indices[t]]);
            const auto b = glm::dvec3(mesh.localVertices[indices[t + 1]]);
            const auto d = glm::dvec3(mesh.localVertices[indices[t + 2]]);
            const auto normal = glm::abs(glm::cross(b - a, d - a));
            const int axis = normal.x > normal.y ? (normal.x > normal.z ? 0 : 2) : (normal.y > normal.z ? 1 : 2);
            std::array<glm::vec2, 3> uv{project(indices[t], axis), project(indices[t + 1], axis),
                                        project(indices[t + 2], axis)};

            if (options.mapping != UVMapping::Box) {
                // Pole U is taken from neighboring corners, where longitude is defined.
                for (int k = 0; k < 3; ++k) {
                    const auto p = glm::dvec3(mesh.localVertices[indices[t + k]]) - center;

                    if (p.x == 0 && p.z == 0) {
                        const auto u = uv[(k + 1) % 3].x, v = uv[(k + 2) % 3].x;

                        uv[k].x = (u + v + (std::abs(u - v) > 0.5f ? 1.0f : 0.0f)) * 0.5f;
                    }
                }

                const float lo = std::min({uv[0].x, uv[1].x, uv[2].x});
                const float hi = std::max({uv[0].x, uv[1].x, uv[2].x});

                if (hi - lo > 0.5f)
                    for (auto& p : uv)
                        if (p.x < 0.5f)
                            p.x += 1;
            }

            for (int k = 0; k < 3; ++k) {
                const auto source = indices[t + k], target = std::uint32_t(result.localVertices.size());

                if (first[source] == std::numeric_limits<std::uint32_t>::max())
                    first[source] = target;
                result.localVertices.push_back(mesh.localVertices[source]);

                if (!mesh.vertexNormals.empty())
                    result.vertexNormals.push_back(mesh.vertexNormals[source]);

                if (!mesh.vertexColors.empty())
                    result.vertexColors.push_back(mesh.vertexColors[source]);
                result.texcoords.push_back(uv[k]);
                result.triangleIndices.push_back(target);
            }
        }

        for (auto index : mesh.outlineIndices) {
            if (first[index] == std::numeric_limits<std::uint32_t>::max()) {
                first[index] = std::uint32_t(result.localVertices.size());
                result.localVertices.push_back(mesh.localVertices[index]);

                if (!mesh.vertexNormals.empty())
                    result.vertexNormals.push_back(mesh.vertexNormals[index]);

                if (!mesh.vertexColors.empty())
                    result.vertexColors.push_back(mesh.vertexColors[index]);
                result.texcoords.push_back(project(index, options.axis));
            }
            result.outlineIndices.push_back(first[index]);
        }
    }

    if (!transformTexcoords(result, options.scale, options.offset, options.rotationDegrees) ||
        !detail::validMesh(result))
        return false;
    mesh = std::move(result);

    return true;
}

/// [API] 메시의 삼각형 기하로 정점 법선을 다시 계산한다.
inline void recalculateNormals(Mesh& mesh) {
    mesh.vertexNormals.assign(mesh.localVertices.size(), glm::vec3(0));

    const auto indices = detail::triangles(mesh);

    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i + 1], c = indices[i + 2];
        const auto n =
            glm::cross(mesh.localVertices[b] - mesh.localVertices[a], mesh.localVertices[c] - mesh.localVertices[a]);

        mesh.vertexNormals[a] += n;
        mesh.vertexNormals[b] += n;
        mesh.vertexNormals[c] += n;
    }

    for (auto& n : mesh.vertexNormals) {
        const float size = glm::length(n);

        n = size > 1e-12f ? n / size : glm::vec3(0);
    }
}

// Weld equal positions for edge topology, while preserving separate UV/normal vertices.
/// [API] 메시의 윤곽 모드에 따라 선으로 그릴 정점 인덱스 쌍 목록을 계산한다.
inline std::vector<std::uint32_t> meshOutline(const Mesh& mesh, OutlineMode mode = OutlineMode::FeatureEdges) {
    if (mode == OutlineMode::None)
        return {};

    if (mode == OutlineMode::Explicit)
        return mesh.outlineIndices;
    using Position = std::array<float, 3>;

    std::map<Position, std::uint32_t> points;
    std::vector<std::uint32_t> welded;

    for (std::uint32_t i = 0; i < mesh.localVertices.size(); ++i) {
        const auto& p = mesh.localVertices[i];
        const auto result = points.emplace(Position{p.x, p.y, p.z}, i);

        welded.push_back(result.first->second);
    }

    struct Edge {
        std::uint32_t a = 0, b = 0;
        std::vector<glm::vec3> normals;
    };

    std::map<std::pair<std::uint32_t, std::uint32_t>, Edge> edges;
    const auto indices = detail::triangles(mesh);

    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i + 1], c = indices[i + 2];
        auto normal =
            glm::cross(mesh.localVertices[b] - mesh.localVertices[a], mesh.localVertices[c] - mesh.localVertices[a]);
        const auto length = glm::length(normal);

        if (length <= 1e-12f)
            continue;
        normal /= length;

        const std::array<std::uint32_t, 3> corners{a, b, c};

        for (int e = 0; e < 3; ++e) {
            auto first = welded[corners[e]], second = welded[corners[(e + 1) % 3]];

            if (first == second)
                continue;

            if (first > second)
                std::swap(first, second);

            auto& edge = edges[{first, second}];

            edge.a = first;
            edge.b = second;
            edge.normals.push_back(normal);
        }
    }

    const float threshold = std::cos(glm::radians(mesh.creaseAngleDegrees));
    std::vector<std::uint32_t> result;

    for (const auto& item : edges) {
        const auto& edge = item.second;
        const bool show = mode == OutlineMode::AllTriangleEdges || edge.normals.size() != 2 ||
                          glm::dot(edge.normals[0], edge.normals[1]) < threshold;

        if (show) {
            result.push_back(edge.a);
            result.push_back(edge.b);
        }
    }

    return result;
}

namespace detail {
// Ear clipping handles planar simple concave polygons, with either winding.
inline std::vector<std::uint32_t> triangulate(const std::vector<glm::vec3>& points) {
    if (points.size() < 3)
        throw std::runtime_error("Face has fewer than three corners.");

    glm::vec3 normal(0);

    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto& a = points[i];
        const auto& b = points[(i + 1) % points.size()];

        normal += glm::vec3((a.y - b.y) * (a.z + b.z), (a.z - b.z) * (a.x + b.x), (a.x - b.x) * (a.y + b.y));
    }

    if (glm::length(normal) < 1e-12f)
        throw std::runtime_error("Degenerate polygon.");

    const auto absNormal = glm::abs(normal);
    const int axis =
        absNormal.x > absNormal.y ? (absNormal.x > absNormal.z ? 0 : 2) : (absNormal.y > absNormal.z ? 1 : 2);
    std::vector<glm::dvec2> projected;

    for (const auto& p : points)
        projected.push_back(axis == 0 ? glm::dvec2(p.y, p.z) : axis == 1 ? glm::dvec2(p.x, p.z) : glm::dvec2(p.x, p.y));

    double area = 0;

    for (std::size_t i = 0; i < projected.size(); ++i) {
        const auto& a = projected[i];
        const auto& b = projected[(i + 1) % projected.size()];

        area += a.x * b.y - b.x * a.y;
    }

    if (std::abs(area) < 1e-14)
        throw std::runtime_error("Degenerate polygon projection.");

    const double direction = area > 0 ? 1 : -1;
    auto cross = [](glm::dvec2 a, glm::dvec2 b, glm::dvec2 c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    };

    std::vector<std::uint32_t> polygon(points.size()), result;

    std::iota(polygon.begin(), polygon.end(), 0);

    while (polygon.size() > 3) {
        bool clipped = false;

        for (std::size_t i = 0; i < polygon.size(); ++i) {
            const auto a = polygon[(i + polygon.size() - 1) % polygon.size()], b = polygon[i],
                       c = polygon[(i + 1) % polygon.size()];

            if (direction * cross(projected[a], projected[b], projected[c]) <= 1e-14)
                continue;

            bool inside = false;

            for (auto p : polygon)
                if (p != a && p != b && p != c &&
                    direction * cross(projected[a], projected[b], projected[p]) >= -1e-14 &&
                    direction * cross(projected[b], projected[c], projected[p]) >= -1e-14 &&
                    direction * cross(projected[c], projected[a], projected[p]) >= -1e-14) {
                    inside = true;
                    break;
                }

            if (inside)
                continue;
            result.insert(result.end(), {a, b, c});
            polygon.erase(polygon.begin() + i);
            clipped = true;
            break;
        }

        if (!clipped)
            throw std::runtime_error("Cannot triangulate polygon; check repeated corners or self-intersections.");
    }
    result.insert(result.end(), polygon.begin(), polygon.end());

    return result;
}

inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first = std::uint32_t(destination.localVertices.size());

    destination.localVertices.insert(destination.localVertices.end(), source.localVertices.begin(),
                                     source.localVertices.end());
    destination.vertexNormals.insert(destination.vertexNormals.end(), source.vertexNormals.begin(),
                                     source.vertexNormals.end());
    destination.texcoords.insert(destination.texcoords.end(), source.texcoords.begin(), source.texcoords.end());

    for (auto i : triangles(source))
        destination.triangleIndices.push_back(first + i);
    destination.shape = GLFace::Mesh;
}

inline bool shareEdge(const Mesh& a, const Mesh& b) {
    const auto ae = meshOutline(a), be = meshOutline(b);

    for (std::size_t i = 0; i < ae.size(); i += 2)
        for (std::size_t j = 0; j < be.size(); j += 2) {
            const auto p = a.localVertices[ae[i]], q = a.localVertices[ae[i + 1]], r = b.localVertices[be[j]],
                       s = b.localVertices[be[j + 1]];

            if ((p == r && q == s) || (p == s && q == r))
                return true;
        }

    return false;
}

inline void mergeCoplanar(Model& model) {
    for (std::size_t i = 0; i < model.faces.size(); ++i) {
        auto& face = model.faces[i];

        if (glm::length(face.normal) < 1e-12f)
            continue;

        for (std::size_t j = i + 1; j < model.faces.size();) {
            const auto& other = model.faces[j];
            const bool samePlane =
                glm::dot(face.normal, other.normal) > 1 - 1e-6f &&
                std::abs(glm::dot(face.normal, other.localVertices[0] - face.localVertices[0])) < 1e-6f;

            if (samePlane && face.texcoords.empty() == other.texcoords.empty() && shareEdge(face, other)) {
                appendMesh(face, other);
                model.faces.erase(model.faces.begin() + j);
                j = i + 1;
            } else
                ++j;
        }
    }
}
} // namespace detail
} // namespace graphics

// --- Texture.h ---

namespace graphics {
namespace detail {
template <class T> struct ComOwner {
    T* value = nullptr;

    ~ComOwner() {
        if (value)
            value->Release();
    }

    ComOwner() = default;

    ComOwner(const ComOwner&) = delete;

    ComOwner& operator=(const ComOwner&) = delete;
};

inline void checkCom(HRESULT result) {
    if (FAILED(result))
        throw std::runtime_error("Image decoder failed (HRESULT " + std::to_string(result) + ").");
}

struct ComApartment {
    HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    ComApartment() {
        if (result != RPC_E_CHANGED_MODE)
            checkCom(result);
    }

    ~ComApartment() {
        if (SUCCEEDED(result))
            CoUninitialize();
    }
};

inline TextureImage decodeImage(const std::filesystem::path& file, const std::vector<std::uint8_t>* encoded,
                                bool flipRows) {
    ComApartment apartment;
    ComOwner<IWICImagingFactory> factory;

    checkCom(CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&factory.value)));

    ComOwner<IWICStream> stream;
    ComOwner<IWICBitmapDecoder> decoder;

    if (encoded) {
        if (encoded->empty() || encoded->size() > std::numeric_limits<DWORD>::max())
            throw std::runtime_error("Invalid encoded texture size.");
        checkCom(factory.value->CreateStream(&stream.value));
        checkCom(stream.value->InitializeFromMemory(const_cast<BYTE*>(encoded->data()), DWORD(encoded->size())));
        checkCom(factory.value->CreateDecoderFromStream(stream.value, nullptr, WICDecodeMetadataCacheOnLoad,
                                                        &decoder.value));
    } else
        checkCom(factory.value->CreateDecoderFromFilename(file.c_str(), nullptr, GENERIC_READ,
                                                          WICDecodeMetadataCacheOnLoad, &decoder.value));

    ComOwner<IWICBitmapFrameDecode> frame;
    ComOwner<IWICFormatConverter> converter;

    checkCom(decoder.value->GetFrame(0, &frame.value));
    checkCom(factory.value->CreateFormatConverter(&converter.value));
    checkCom(converter.value->Initialize(frame.value, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0,
                                         WICBitmapPaletteTypeCustom));

    TextureImage image;

    checkCom(converter.value->GetSize(&image.width, &image.height));

    if (!image.width || !image.height || image.width > 16384 || image.height > 16384)
        throw std::runtime_error("Texture dimensions must be within 1..16384.");
    image.rgba.resize(std::size_t(image.width) * image.height * 4);
    checkCom(converter.value->CopyPixels(nullptr, image.width * 4, UINT(image.rgba.size()), image.rgba.data()));

    if (flipRows) {
        const std::size_t stride = std::size_t(image.width) * 4;

        for (std::uint32_t y = 0; y < image.height / 2; ++y)
            std::swap_ranges(image.rgba.begin() + y * stride, image.rgba.begin() + (y + 1) * stride,
                             image.rgba.begin() + (image.height - 1 - y) * stride);
    }

    return image;
}
} // namespace detail
} // namespace graphics

// --- ModelLoader.h ---

namespace graphics {
struct FeatureOptions {
    bool textures = false;
};

struct LoadOptions {
    PivotMode pivot = PivotMode::BottomCenter;
    glm::vec3 customPivot{0};
    OutlineMode outline = OutlineMode::None;
    bool mergeCoplanarFaces = true; // within each OBJ object/group/material/smoothing segment
    bool flipTextureRows = true;

    std::filesystem::path texturePath; // project-relative explicit override

    bool allowObjFallback = true;
};

struct ModelResult {
    std::optional<Model> model;
    glm::vec3 localBasis{0};
    PivotMode pivotMode = PivotMode::Preserve;
    std::string error;
    std::vector<std::string> warnings;
    bool usedFallback = false;

    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
    explicit operator bool() const {
        return model.has_value();
    }
};

struct TextureResult {
    std::shared_ptr<const TextureImage> image;
    std::string error;

    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
    explicit operator bool() const {
        return bool(image);
    }
};

namespace detail {
inline std::filesystem::path executableDirectory();

inline ModelResult finishLoad(Model model, const LoadOptions& options) {
    if (model.faces.empty())
        throw std::runtime_error("Model contains no triangle meshes.");

    for (auto& face : model.faces) {
        face.outline = options.outline;

        if (!validMesh(face))
            throw std::runtime_error("Invalid model mesh data.");
    }

    ModelResult result;

    if (options.pivot == PivotMode::BottomCenter)
        result.localBasis = bottomCenter(model);
    else if (options.pivot == PivotMode::Custom)
        result.localBasis = options.customPivot;

    if (!finiteVector(result.localBasis))
        throw std::runtime_error("Invalid local basis.");
    result.pivotMode = options.pivot;
    result.model = std::move(model);

    return result;
}
} // namespace detail

class AssetLoader {
    std::filesystem::path projectRoot_;

    FeatureOptions features_;

    mutable std::map<std::filesystem::path, std::weak_ptr<const TextureImage>> imageCache_;

    std::filesystem::path resolve(const std::filesystem::path& relative) const {
        if (relative.empty() || relative.is_absolute() || relative.has_root_name())
            throw std::runtime_error("Asset path must be project-relative.");

        const auto root = std::filesystem::weakly_canonical(projectRoot_);
        const auto full = std::filesystem::weakly_canonical(root / relative);
        auto a = root.begin(), b = full.begin();

        for (; a != root.end(); ++a, ++b)
            if (b == full.end() || *a != *b)
                throw std::runtime_error("Asset path is outside projectRoot.");

        return full;
    }

    std::shared_ptr<const TextureImage> texture(const std::filesystem::path& relative, bool flip) const {
        if (!features_.textures)
            throw std::runtime_error("Texture mode is disabled.");

        const auto full = resolve(relative);
        // The row convention is part of the cache key.
        const auto key = full / (flip ? "#flipped" : "#native");

        if (auto cached = imageCache_[key].lock())
            return cached;

        auto image = std::make_shared<const TextureImage>(detail::decodeImage(full, nullptr, flip));

        imageCache_[key] = image;

        return image;
    }

public:
    /// [API] 프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.
    explicit AssetLoader(std::filesystem::path projectRoot = detail::executableDirectory(),
                         FeatureOptions features = FeatureOptions{})
        : projectRoot_(std::filesystem::absolute(projectRoot)), features_(features) {
    }

    /// [API] 로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.
    const FeatureOptions& features() const {
        return features_;
    }

    /// [API] 프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.
    TextureResult loadTexture(const std::filesystem::path& relative, bool flipRows = true) const {
        try {
            return {texture(relative, flipRows), {}};
        } catch (const std::exception& e) {
            return {nullptr, e.what()};
        }
    }

    /// [API] 프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.
    ModelResult loadObj(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
};
} // namespace graphics

namespace graphics::detail {
inline std::filesystem::path executableDirectory() {
    std::wstring path(32768, L'\0');
    const DWORD size = GetModuleFileNameW(nullptr, path.data(), DWORD(path.size()));
    if (!size || size >= path.size())
        throw std::runtime_error("Cannot locate the executable directory.");
    path.resize(size);
    return std::filesystem::path(path).parent_path();
}

inline HMODULE loadBundledDll(const wchar_t* dllFileName, const wchar_t* fallbackSubpath) {
    if (const HMODULE loaded = GetModuleHandleW(dllFileName))
        return loaded;

    std::vector<std::filesystem::path> roots;
    std::error_code ec;
    try {
#if defined(_MSVC_EXECUTION_CHARACTER_SET) && _MSVC_EXECUTION_CHARACTER_SET == 65001
#if defined(__cpp_char8_t)
        const auto sourceDir =
            std::filesystem::path(reinterpret_cast<const char8_t*>(__FILE__)).parent_path();
#else
        const auto sourceDir = std::filesystem::u8path(__FILE__).parent_path();
#endif
#else
        const auto sourceDir = std::filesystem::path(__FILE__).parent_path();
#endif
        if (!sourceDir.empty()) {
            roots.push_back(sourceDir);
            if (sourceDir.has_parent_path())
                roots.push_back(sourceDir.parent_path());
        }
    } catch (...) {
    }

    try {
        const auto exeDir = executableDirectory();
        roots.push_back(exeDir);
        if (exeDir.has_parent_path()) {
            const auto parent = exeDir.parent_path();
            roots.push_back(parent);
            if (parent.has_parent_path())
                roots.push_back(parent.parent_path());
        }
    } catch (...) {
    }

    const auto cwd = std::filesystem::current_path(ec);
    if (!ec && !cwd.empty())
        roots.push_back(cwd);

    for (const auto& root : roots) {
        for (const auto& candidate : {root / L"dependencies" / L"dll" / dllFileName, root / fallbackSubpath}) {
            ec.clear();
            if (std::filesystem::exists(candidate, ec) && !ec) {
                SetDllDirectoryW(candidate.parent_path().c_str());
                if (const HMODULE module = LoadLibraryW(candidate.c_str()))
                    return module;
            }
        }
    }
    return nullptr;
}

inline void preloadBundledDlls() {
    loadBundledDll(L"glfw3.dll", L"dependencies\\glfw-3.4.bin.WIN64\\lib-vc2022\\glfw3.dll");
    loadBundledDll(L"glew32.dll", L"dependencies\\glew-2.3.1\\bin\\Release\\x64\\glew32.dll");
}

inline const bool bundledDllsPreloaded = (preloadBundledDlls(), true);
} // namespace graphics::detail

extern "C" {
GLboolean glewExperimental = GL_FALSE;
PFNGLACTIVETEXTUREPROC __glewActiveTexture = nullptr;
PFNGLATTACHSHADERPROC __glewAttachShader = nullptr;
PFNGLBINDBUFFERPROC __glewBindBuffer = nullptr;
PFNGLBINDVERTEXARRAYPROC __glewBindVertexArray = nullptr;
PFNGLBUFFERDATAPROC __glewBufferData = nullptr;
PFNGLCOMPILESHADERPROC __glewCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC __glewCreateProgram = nullptr;
PFNGLCREATESHADERPROC __glewCreateShader = nullptr;
PFNGLDELETEBUFFERSPROC __glewDeleteBuffers = nullptr;
PFNGLDELETEPROGRAMPROC __glewDeleteProgram = nullptr;
PFNGLDELETESHADERPROC __glewDeleteShader = nullptr;
PFNGLDELETEVERTEXARRAYSPROC __glewDeleteVertexArrays = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC __glewEnableVertexAttribArray = nullptr;
PFNGLGENBUFFERSPROC __glewGenBuffers = nullptr;
PFNGLGENVERTEXARRAYSPROC __glewGenVertexArrays = nullptr;
PFNGLGETPROGRAMINFOLOGPROC __glewGetProgramInfoLog = nullptr;
PFNGLGETPROGRAMIVPROC __glewGetProgramiv = nullptr;
PFNGLGETSHADERINFOLOGPROC __glewGetShaderInfoLog = nullptr;
PFNGLGETSHADERIVPROC __glewGetShaderiv = nullptr;
PFNGLGETUNIFORMLOCATIONPROC __glewGetUniformLocation = nullptr;
PFNGLLINKPROGRAMPROC __glewLinkProgram = nullptr;
PFNGLSHADERSOURCEPROC __glewShaderSource = nullptr;
PFNGLUNIFORM1FPROC __glewUniform1f = nullptr;
PFNGLUNIFORM1IPROC __glewUniform1i = nullptr;
PFNGLUNIFORM3FPROC __glewUniform3f = nullptr;
PFNGLUNIFORMMATRIX3FVPROC __glewUniformMatrix3fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC __glewUniformMatrix4fv = nullptr;
PFNGLUSEPROGRAMPROC __glewUseProgram = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC __glewVertexAttribIPointer = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC __glewVertexAttribPointer = nullptr;

GLenum GLEWAPIENTRY glewInit(void) {
    graphics::detail::preloadBundledDlls();
    struct Entry {
        void** target;
        const char* name;
    };
    const Entry entries[] = {
        {reinterpret_cast<void**>(&__glewActiveTexture), "glActiveTexture"},
        {reinterpret_cast<void**>(&__glewAttachShader), "glAttachShader"},
        {reinterpret_cast<void**>(&__glewBindBuffer), "glBindBuffer"},
        {reinterpret_cast<void**>(&__glewBindVertexArray), "glBindVertexArray"},
        {reinterpret_cast<void**>(&__glewBufferData), "glBufferData"},
        {reinterpret_cast<void**>(&__glewCompileShader), "glCompileShader"},
        {reinterpret_cast<void**>(&__glewCreateProgram), "glCreateProgram"},
        {reinterpret_cast<void**>(&__glewCreateShader), "glCreateShader"},
        {reinterpret_cast<void**>(&__glewDeleteBuffers), "glDeleteBuffers"},
        {reinterpret_cast<void**>(&__glewDeleteProgram), "glDeleteProgram"},
        {reinterpret_cast<void**>(&__glewDeleteShader), "glDeleteShader"},
        {reinterpret_cast<void**>(&__glewDeleteVertexArrays), "glDeleteVertexArrays"},
        {reinterpret_cast<void**>(&__glewEnableVertexAttribArray), "glEnableVertexAttribArray"},
        {reinterpret_cast<void**>(&__glewGenBuffers), "glGenBuffers"},
        {reinterpret_cast<void**>(&__glewGenVertexArrays), "glGenVertexArrays"},
        {reinterpret_cast<void**>(&__glewGetProgramInfoLog), "glGetProgramInfoLog"},
        {reinterpret_cast<void**>(&__glewGetProgramiv), "glGetProgramiv"},
        {reinterpret_cast<void**>(&__glewGetShaderInfoLog), "glGetShaderInfoLog"},
        {reinterpret_cast<void**>(&__glewGetShaderiv), "glGetShaderiv"},
        {reinterpret_cast<void**>(&__glewGetUniformLocation), "glGetUniformLocation"},
        {reinterpret_cast<void**>(&__glewLinkProgram), "glLinkProgram"},
        {reinterpret_cast<void**>(&__glewShaderSource), "glShaderSource"},
        {reinterpret_cast<void**>(&__glewUniform1f), "glUniform1f"},
        {reinterpret_cast<void**>(&__glewUniform1i), "glUniform1i"},
        {reinterpret_cast<void**>(&__glewUniform3f), "glUniform3f"},
        {reinterpret_cast<void**>(&__glewUniformMatrix3fv), "glUniformMatrix3fv"},
        {reinterpret_cast<void**>(&__glewUniformMatrix4fv), "glUniformMatrix4fv"},
        {reinterpret_cast<void**>(&__glewUseProgram), "glUseProgram"},
        {reinterpret_cast<void**>(&__glewVertexAttribIPointer), "glVertexAttribIPointer"},
        {reinterpret_cast<void**>(&__glewVertexAttribPointer), "glVertexAttribPointer"},
    };
    for (const auto& entry : entries) {
        void* proc = reinterpret_cast<void*>(glfwGetProcAddress(entry.name));
        if (!proc) {
            proc = reinterpret_cast<void*>(wglGetProcAddress(entry.name));
            const auto code = reinterpret_cast<std::uintptr_t>(proc);
            if (code == 0 || code == 1 || code == 2 || code == 3 || code == static_cast<std::uintptr_t>(-1))
                proc = nullptr;
        }
        *entry.target = proc;
        if (!proc)
            return GLEW_ERROR_NO_GL_VERSION;
    }
    return GLEW_OK;
}
} // extern "C"

namespace graphics::detail {

inline std::filesystem::path relativePathInside(const std::filesystem::path& root,
                                                const std::filesystem::path& relative) {
    if (relative.empty() || relative.is_absolute() || relative.has_root_name() || relative.has_root_directory())
        throw std::runtime_error("Use a relative path inside the project.");
    const auto base = std::filesystem::weakly_canonical(root);
    const auto full = std::filesystem::weakly_canonical(base / relative);
    auto a = base.begin(), b = full.begin();
    for (; a != base.end(); ++a, ++b)
        if (b == full.end() || *a != *b)
            throw std::runtime_error("Path leaves the project directory.");
    return full;
}

inline bool checkGlAt(const char* file, int line, PipelineStage stage, const char* api) {
    bool success = true;
    for (GLenum code = glGetError(); code != GL_NO_ERROR; code = glGetError()) {
        const char* cause =
            code == GL_INVALID_ENUM                    ? "GL_INVALID_ENUM: unsupported enum"
            : code == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE: value outside allowed range"
            : code == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION: incompatible binding, state or program"
            : code == GL_OUT_OF_MEMORY                 ? "GL_OUT_OF_MEMORY: GPU allocation failed"
            : code == GL_INVALID_FRAMEBUFFER_OPERATION ? "GL_INVALID_FRAMEBUFFER_OPERATION: incomplete framebuffer"
                                                       : "OpenGL error";
        Diagnostics::failAt(
            file, line, stage, api, std::string(cause) + " (" + std::to_string(code) + ")",
            "Inspect this stage's bindings, buffer sizes, vertex layout, shader log and current context. A GL error does not identify a single root cause.");
        success = false;
    }
    return success;
}
} // namespace graphics::detail

// --- RenderConfig.h ---

namespace graphics {
class Scene;

// Canonical CPU input for packing; independent of GL attribute locations.
struct RenderVertex {
    float position[3], color[3], uv[2], normal[3];
};

struct VertexAttribute {
    GLuint location = 0;
    GLint components = 3;
    GLenum type = GL_FLOAT;
    bool normalized = false;
    std::size_t offset = 0;
    bool integer = false; // ivec/uvec inputs use VertexAttribIPointer.
};

struct VertexLayout {
    std::size_t stride = sizeof(RenderVertex);
    std::vector<VertexAttribute> attributes{
        {0, 3, GL_FLOAT, false, offsetof(RenderVertex, position)},
        {1, 3, GL_FLOAT, false, offsetof(RenderVertex, color)},
        {2, 2, GL_FLOAT, false, offsetof(RenderVertex, uv)},
        {3, 3, GL_FLOAT, false, offsetof(RenderVertex, normal)}};
};

struct UniformBindings {
    std::string mvp = "uMVP", model = "uModel", normal = "uNormalMatrix";
    std::string texture = "uTexture", hasTexture = "uHasTexture", lighting = "uLighting";
    std::string lightDirection = "uLightDirection", lightColor = "uLightColor";
    std::string overrideColor = "uOverrideColor", outlineColor = "uOutlineColor", tint = "uTint";
    std::string pointLightCount = "uPointLightCount", pointLights = "uPointLights";
    std::string directionalIntensity = "uDirectionalIntensity";
    std::string viewPosition = "uViewPosition", ambientColor = "uAmbientColor";
    std::string materialAmbient = "uMaterialAmbient", materialDiffuse = "uMaterialDiffuse";
    std::string materialSpecular = "uMaterialSpecular", shininess = "uShininess";
};

struct DrawUniformContext {
    GLuint program;
    const Scene& scene;
    std::optional<EntityId> entity;
    std::optional<LineId> line;
    std::size_t face;
    glm::mat4 model, view, projection;
    bool depthOnly = false;

    /// [API] 현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.
    bool setUniform(const std::string& name, float value) const {
        const auto location = glGetUniformLocation(program, name.c_str());
        if (location < 0 || !std::isfinite(value))
            return false;
        glUniform1f(location, value);
        return true;
    }

    /// [API] 현재 program의 추가 int/bool/sampler uniform을 설정한다.
    bool setUniform(const std::string& name, int value) const {
        const auto location = glGetUniformLocation(program, name.c_str());
        if (location < 0)
            return false;
        glUniform1i(location, value);
        return true;
    }

    /// [API] 현재 program의 추가 vec3 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::vec3& value) const {
        const auto location = glGetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteVector(value))
            return false;
        glUniform3f(location, value.x, value.y, value.z);
        return true;
    }

    /// [API] 현재 program의 추가 mat4 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::mat4& value) const {
        const auto location = glGetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteMatrix(value))
            return false;
        glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
        return true;
    }
};

struct RenderConfig {
    VertexLayout vertexLayout;
    UniformBindings uniforms;
    // Must output exactly input.size()*stride bytes, preserving vertex order/count.
    std::function<bool(const std::vector<RenderVertex>&, std::vector<std::uint8_t>&)> packVertices;
    // Runs with program bound, after built-in uniforms, before each range/pass.
    std::function<bool(const DrawUniformContext&)> bindUniforms;
};
} // namespace graphics

// --- Graphics.h ---

namespace graphics {

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;

public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();

    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});

    /// [API] 반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32,
                                      unsigned stacks = 16);

    /// [API] 반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);

    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);

    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});

    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);

    /// [API] 세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);

    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);

    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);

    /// [API] 형상 재업로드 없이 면 앞쪽의 부모 프레임을 설정한다. 렌더·피킹·법선 변환에 함께 반영한다.
    bool setFaceFrame(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);

    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);

    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);

    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);

    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);

    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);

    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);

    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.
    bool setParent(EntityId id, std::optional<EntityId> parent);

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

    /// [API] 부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;

    /// [API] 엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;

    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;

    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    MotionSystem& getMotions() {
        return motions_;
    }

    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const MotionSystem& getMotions() const {
        return motions_;
    }

    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);

    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);

    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;

    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
    bool updateMotions(double deltaSeconds) {
        return motions_.update(deltaSeconds);
    }

    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);

    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);

    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));

    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);

    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);

    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;

    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);

    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);

    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);

    /// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});

    /// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                            float rotationDegrees = 0);

    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);

    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);

    /// [API] 메시의 색상·텍스처 등 재질 데이터를 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);

    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});

    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));

    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);

    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);

    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);

    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);

    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);

    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
    std::optional<std::array<LineId, 3>> addBasis(const glm::vec3& origin,
                                                  const glm::mat3& basisVectors = glm::mat3(1.0f),
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);

    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);

    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.

    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
    const std::vector<EntityId>& getIds() const {
        return polyId_;
    }

    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;

    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const {
        return lines_;
    }

    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
    std::uint64_t getGeometryRevision() const {
        return geometryRevision_;
    }
};

class DiagramSystem {
public:
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));

    /// [API] 반지름·분할 수로 CPU 구 메시 모델을 생성한다.
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);

    /// [API] 반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.
    static Model makeCone(float radius, float height, unsigned slices = 32);

    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);

    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);

    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);

    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{2.8f, 2.0f, 5.0f};
    glm::vec3 target{0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

class CameraSystem {
public:
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);

    /// [API] 지역 카메라를 아핀 프레임에 부착한다. 시선/up을 직교화하며 퇴화하면 실패한다.
    static std::optional<Camera> attached(const Camera& local, const glm::mat4& frame);

    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);

    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);

struct PointLight {
    glm::vec3 position{0};
    GlColor color{1,1,1};
    float intensity = 1, attenuation = .02f;
};

struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
    bool textures = false; // Explicit opt-in; no texture GPU work when false.
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
    CullMode cull = CullMode::None; // Append to preserve existing aggregate initializers.
    float directionalIntensity = 1; // 점광원의 방향성 강도 0 ~ 1 
    std::vector<PointLight> pointLights; // At most eight world-space lights in the default shader.
    GlColor ambientColor{0, 0, 0}; // Independent ambient light; legacy directional ambient remains.
};

struct Viewport {
    int x = 0, y = 0, width = 0, height = 0;
}; // Bottom-left framebuffer pixels.

struct RenderPass {
    Camera camera;
    Viewport viewport;
    RenderOptions options;
    bool clearColor = true, clearDepth = true;
};

class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };

    RenderConfig config_;
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1, tintLocation_ = -1;
    std::vector<DrawRange> drawRanges_;

    using TextureMap =
        std::map<std::shared_ptr<const TextureImage>, GLuint, std::owner_less<std::shared_ptr<const TextureImage>>>;

    TextureMap textures_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;

    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

    bool upload(const Scene& scene, bool textures);

    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;

public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;

    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer(const Renderer&) = delete;

    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer& operator=(const Renderer&) = delete;

    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{},
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});

    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});

    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);

    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);

    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy(); // Call while the owning context is current, before it is destroyed.

    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.
    void abandon() {
        shaderProgramID_ = vao_ = vbo_ = ebo_ = 0;
        textures_.clear();
        drawRanges_.clear();
        uploadedScene_ = nullptr;
        uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    }
};
} // namespace graphics

// --- Picking.h ---

namespace graphics {
struct Ray {
    glm::vec3 origin{0}, direction{0, 0, -1};
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

struct PickViewport {
    double x = 0, y = 0, width = 0, height = 0;
}; // top-left framebuffer pixels

/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
inline std::optional<Ray> screenRay(const Camera& camera, double x, double y, const PickViewport& viewport) {
    if (!CameraSystem::validCamera(camera) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(viewport.x) ||
        !std::isfinite(viewport.y) || !std::isfinite(viewport.width) || !std::isfinite(viewport.height) ||
        viewport.width <= 0 || viewport.height <= 0 || x < viewport.x || y < viewport.y ||
        x >= viewport.x + viewport.width || y >= viewport.y + viewport.height)
        return {};

    const auto pv = CameraSystem::projectionMatrix(camera, float(viewport.width / viewport.height)) *
                    CameraSystem::viewMatrix(camera);
    const auto determinant = glm::determinant(pv);

    if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
        return {};

    const auto inverse = glm::inverse(pv);
    const float nx = float(2 * (x - viewport.x) / viewport.width - 1),
                ny = float(1 - 2 * (y - viewport.y) / viewport.height);
    auto nearPoint = inverse * glm::vec4(nx, ny, -1, 1), farPoint = inverse * glm::vec4(nx, ny, 1, 1);

    if (std::abs(nearPoint.w) < 1e-12f || std::abs(farPoint.w) < 1e-12f)
        return {};
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    const auto direction = glm::vec3(farPoint - nearPoint);
    const auto length = glm::length(direction);

    if (!detail::finiteVector(glm::vec3(nearPoint)) || !detail::finiteVector(direction) || !std::isfinite(length) ||
        length <= 0)
        return {};

    return Ray{glm::vec3(nearPoint), direction / length, length};
}

/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Ray& input, CullMode inheritedCull = CullMode::None) {
    if (!detail::finiteVector(input.origin) || !detail::finiteVector(input.direction) ||
        !std::isfinite(glm::length(input.direction)) || glm::length(input.direction) < 1e-12f ||
        std::isnan(input.maxDistance) || input.maxDistance < 0)
        return {};

    const auto direction = glm::dvec3(glm::normalize(input.direction));
    const auto origin = glm::dvec3(input.origin);
    double nearest = input.maxDistance;
    std::optional<PickResult> result;

    for (auto id : scene.getIds()) {
        const auto* component = scene.getComponent(id);

        if (!component || !component->visible)
            continue;

        for (std::size_t f = 0; f < component->polyhedron.faces.size(); ++f) {
            const auto& face = component->polyhedron.faces[f];

            if (!face.visible)
                continue;

            const auto world = scene.faceWorldMatrix(id, f);

            if (!world)
                continue;

            const auto matrix = *world;
            const auto cull = face.cull == CullMode::Inherit ? inheritedCull : face.cull;
            const bool mirrored = glm::determinant(glm::mat3(matrix)) < 0;
            const auto indices = detail::triangles(face);

            for (std::size_t t = 0; t < indices.size(); t += 3) {
                const auto ia = indices[t], ib = indices[t + 1], ic = indices[t + 2];
                const auto a4 = matrix * glm::vec4(face.localVertices[ia], 1),
                           b4 = matrix * glm::vec4(face.localVertices[ib], 1),
                           c4 = matrix * glm::vec4(face.localVertices[ic], 1);

                if (std::abs(a4.w) < 1e-12f || std::abs(b4.w) < 1e-12f || std::abs(c4.w) < 1e-12f)
                    continue;

                const auto a = glm::dvec3(a4 / a4.w), b = glm::dvec3(b4 / b4.w), c = glm::dvec3(c4 / c4.w);
                const auto edge1 = b - a, edge2 = c - a, p = glm::cross(direction, edge2);
                const auto determinant = glm::dot(edge1, p);
                const auto tolerance = 1e-12 * glm::length(edge1) * glm::length(edge2);

                if (!std::isfinite(determinant) || std::abs(determinant) <= tolerance)
                    continue;

                const bool frontFacing = (determinant > 0) != mirrored;
                if ((cull == CullMode::Front && frontFacing) || (cull == CullMode::Back && !frontFacing))
                    continue;

                const auto relative = origin - a;
                const auto u = glm::dot(relative, p) / determinant;

                if (u < 0 || u > 1)
                    continue;

                const auto q = glm::cross(relative, edge1);
                const auto v = glm::dot(direction, q) / determinant;

                if (v < 0 || u + v > 1)
                    continue;

                const auto distance = glm::dot(edge2, q) / determinant;

                if (!std::isfinite(distance) || distance < 0 || distance > nearest)
                    continue;

                const glm::dvec3 weights((1 - u - v) / a4.w, u / b4.w, v / c4.w);
                const auto sum = weights.x + weights.y + weights.z;

                if (std::abs(sum) < 1e-12)
                    continue;

                PickResult hit;

                hit.entityId = id;
                hit.faceIndex = f;
                hit.triangleIndex = t / 3;
                hit.distance = float(distance);
                hit.worldPoint = glm::vec3(origin + direction * distance);
                hit.barycentric = glm::vec3(weights / sum);
                hit.localPoint = face.localVertices[ia] * hit.barycentric.x +
                                 face.localVertices[ib] * hit.barycentric.y +
                                 face.localVertices[ic] * hit.barycentric.z;
                hit.hasUV = !face.texcoords.empty();

                if (hit.hasUV)
                    hit.uv = face.texcoords[ia] * hit.barycentric.x + face.texcoords[ib] * hit.barycentric.y +
                             face.texcoords[ic] * hit.barycentric.z;
                nearest = distance;
                result = hit;
            }
        }
    }

    return result;
}

/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
                                      const PickViewport& viewport, CullMode inheritedCull = CullMode::None) {
    const auto ray = screenRay(camera, x, y, viewport);

    return ray ? pick(scene, *ray, inheritedCull) : std::nullopt;
}
} // namespace graphics

// --- Application.h ---

struct GLFWwindow;

namespace graphics {

struct ApplicationConfig {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    std::string title = "OpenGL";

    std::filesystem::path shaderDirectory;                // Empty selects embedded shaders.
    std::filesystem::path projectRoot = ".";              // Relative to the executable.
    std::filesystem::path runtimeDirectory = "framework"; // Relative to projectRoot.

    FeatureOptions features;
    bool verticalSync = true;
    DiagnosticOptions diagnostics;
    RenderConfig renderConfig;
};

struct MouseEvent {
    int button = 0, action = 0, mods = 0;
    double windowX = 0, windowY = 0, framebufferX = 0, framebufferY = 0;
};

// 콜백에 캡처한 객체는 run()이 끝날 때까지 유지한다.
// 한 스레드에서 Application 하나를 사용한다. GLFW 초기화와 종료를 소유한다.
class Application {
private:
    GLFWwindow* window_ = nullptr;
    bool glfwInitialized_ = false;
    bool running_ = false;
    DWORD contextThread_ = 0;
    bool contextFailed_ = false;

    void* nativeContext_ = nullptr;

    bool ensureContext(const char* operation, const char* file, int line);

    Scene scene_;
    Camera camera_;
    Renderer renderer_;
    RenderOptions renderOptions_;
    std::optional<AssetLoader> assets_;
    std::exception_ptr callbackError_;
    std::function<void(Application&, const MouseEvent&)> mouseHandler_;
    std::function<void(Application&, double, double)> cursorHandler_;

    template <class Handler> void dispatch(Handler&& handler) noexcept {
        if (callbackError_)
            return;

        try {
            handler();
        } catch (...) {
            callbackError_ = std::current_exception();
            requestClose();
        }
    }

    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
    std::function<void(Application&, int, int)> keyHandler_;
    std::function<void(Application&, int, int, int)> modifiedKeyHandler_;
    std::function<void(Application&, bool)> focusHandler_;
    std::function<bool(Application&)> renderHandler_;

    bool initialize(const ApplicationConfig& config);

    void shutdown();

public:
    /// [API] 초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.
    Application() = default;

    /// [API] 실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.
    ~Application();

    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application(const Application&) = delete;

    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application& operator=(const Application&) = delete;

    /// [API] 장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.
    void setInitHandler(std::function<bool(Application&)> handler);

    /// [API] 매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.
    void setUpdateHandler(std::function<void(Application&, float)> handler);

    /// [API] 키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);

    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    /// [API] 키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.
    void setModifiedKeyHandler(std::function<void(Application&, int key, int action, int mods)> handler);

    /// [API] 창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);

    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    /// [API] 사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);

    /// [API] 마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.
    void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);

    /// [API] 창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.
    void setCursorHandler(std::function<void(Application&, double x, double y)> handler);

    /// [API] 창 좌표를 framebuffer 좌표로 보정하고 기본 카메라의 전체 viewport로 ray picking을 수행한다.
    std::optional<PickResult> pickAt(double windowX, double windowY) const;

    /// [API] 현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.
    std::optional<PickResult> pickAtCursor() const;

    /// [API] 실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.
    AssetLoader& getAssets(); // Available from initialization callback until run() ends.

    /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.
    int run(const ApplicationConfig& config = ApplicationConfig{});

    /// [API] 현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.
    bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {},
                           const std::filesystem::path& fragmentPath = {});

    /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
    void requestClose();

    /// [API] 기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.
    bool renderScene();

    /// [API] 현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.
    bool renderViews(const std::vector<RenderPass>& passes);

    /// [API] 현재 창의 실제 픽셀 크기를 반환한다. DPI 배율이 있으면 창 좌표 크기와 다를 수 있다.
    std::array<int, 2> getFramebufferSize() const;

    /// [API] 애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.
    RenderOptions& getRenderOptions() {
        return renderOptions_;
    }

    /// [API] 애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.
    Scene& getScene() {
        return scene_;
    }

    /// [API] 기본 카메라의 수정 가능한 참조를 반환한다.
    Camera& getCamera() {
        return camera_;
    }

    /// [API] GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.
    GLFWwindow* getWindow() const {
        return window_;
    }
};

} // namespace graphics

// --- Dynamics.h ---

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

// --- Shaders.h ---

namespace graphics::detail {
inline constexpr char vertexShader[] = R"GLSL(#version 330 core
layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
layout(location=2) in vec2 vTexCoord;
layout(location=3) in vec3 vNormal;
uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;
out vec3 outColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 WorldPosition;
void main() {
    gl_Position = uMVP * vec4(vPos, 1.0);
    outColor = vColor;
    TexCoord = vTexCoord;
    Normal = uNormalMatrix * vNormal;
    WorldPosition = (uModel * vec4(vPos, 1.0)).xyz;
}
)GLSL";
inline constexpr char fragmentShader[] = R"GLSL(#version 330 core
in vec3 outColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 WorldPosition;
uniform sampler2D uTexture;
uniform bool uHasTexture;
uniform bool uLighting;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform float uDirectionalIntensity;
uniform vec3 uViewPosition;
uniform vec3 uAmbientColor;
uniform float uMaterialAmbient;
uniform float uMaterialDiffuse;
uniform float uMaterialSpecular;
uniform float uShininess;
struct PointLight { vec3 position; vec3 color; float intensity; float attenuation; };
uniform int uPointLightCount;
uniform PointLight uPointLights[8];
uniform bool uOverrideColor;
uniform vec3 uOutlineColor;
uniform vec3 uTint;
out vec4 FragColor;
float highlight(vec3 n, vec3 lightDirection, vec3 viewDirection) {
    if (dot(n, lightDirection) <= 0.0) return 0.0;
    return uMaterialSpecular * pow(max(dot(viewDirection, reflect(-lightDirection, n)), 0.0), uShininess);
}
void main() {
    if (uOverrideColor) { FragColor = vec4(uOutlineColor, 1.0); return; }
    vec4 color = vec4(outColor * uTint, 1.0);
    if (uHasTexture) color *= texture(uTexture, TexCoord);
    if (uLighting && length(Normal) > 0.0) {
        vec3 n = normalize(Normal);
        vec3 viewDelta = uViewPosition - WorldPosition;
        vec3 viewDirection = viewDelta / sqrt(max(dot(viewDelta, viewDelta), 1e-12));
        vec3 sunDirection = normalize(uLightDirection);
        vec3 illumination = uMaterialAmbient * uAmbientColor
            + (uMaterialAmbient + 0.75 * uMaterialDiffuse * max(dot(n, sunDirection), 0.0)
               + highlight(n, sunDirection, viewDirection)) * uLightColor * uDirectionalIntensity;
        for (int i = 0; i < uPointLightCount; ++i) {
            vec3 delta = uPointLights[i].position - WorldPosition;
            float distanceSquared = dot(delta, delta);
            vec3 direction = delta / sqrt(max(distanceSquared, 1e-12));
            illumination += uPointLights[i].color * uPointLights[i].intensity
                * (uMaterialDiffuse * max(dot(n, direction), 0.0) + highlight(n, direction, viewDirection))
                / (1.0 + uPointLights[i].attenuation * distanceSquared);
        }
        color.rgb *= illumination;
    }
    FragColor = color;
}
)GLSL";
}

// --- Internal.h ---

// Shared declarations for separately compiled framework implementation files.
namespace graphics::detail {
std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction);

std::optional<glm::mat4> motionAround(const glm::vec3& center, const glm::mat4& matrix);

bool motionPlane(const glm::vec3& u, const glm::vec3& v);

bool positiveLengths(const glm::vec3& value);

std::uint64_t nextGeometryRevision();

void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color);
} // namespace graphics::detail

// --- Transform.cpp ---

namespace graphics {
namespace detail {
std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction))
        return {};

    const double length = glm::length(glm::dvec3(direction));

    if (!std::isfinite(length) || length <= 1e-12)
        return {};

    return glm::vec3(glm::dvec3(direction) / length);
}

std::optional<glm::mat4> motionAround(const glm::vec3& center, const glm::mat4& matrix) {
    if (!finiteVector(center))
        return {};

    const auto result = glm::translate(glm::mat4(1), center) * matrix * glm::translate(glm::mat4(1), -center);

    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);

    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0 && m[2][3] == 0 && m[3][3] == 1;
}

std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::translation",
                                "Rejected condition: !detail::finiteVector(offset)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return glm::translate(glm::mat4(1), offset);
}

std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors, const glm::vec3& center) {
    if (!detail::finiteVector(factors))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::scaling",
                                "Rejected condition: !detail::finiteVector(factors)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}

std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis, double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);

    if (!unit || !std::isfinite(degrees))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::rotation",
                                "Rejected condition: !unit || !std::isfinite(degrees)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const float radians = static_cast<float>(std::remainder(degrees, 360.0) * 3.14159265358979323846 / 180.0);

    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}

std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}

std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);

    if (!unit)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::reflectLine",
                                "Rejected condition: !unit",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    glm::mat3 matrix(0);

    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);

    return detail::motionAround(line.point, glm::mat4(matrix));
}

std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point, const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);

    if (!unit)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::reflectPlane",
                                "Rejected condition: !unit",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    glm::mat3 matrix(1);

    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];

    return detail::motionAround(point, glm::mat4(matrix));
}

PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u, const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::ellipse",
                                "Rejected condition: !detail::finiteVector(center) || !detail::motionPlane(u, v)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;

        return center + u * static_cast<float>(std::cos(angle)) + v * static_cast<float>(std::sin(angle));
    };
}

PathFunction PathSystem::circle(const glm::vec3& center, float radius, const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);

    if (!n || !std::isfinite(radius) || radius <= 0)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::circle",
                                "Rejected condition: !n || !std::isfinite(radius) || radius <= 0",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));

    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}

PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::polyline",
                                "Rejected condition: points.size() < 2",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    for (const auto& p : points)
        if (!detail::finiteVector(p))
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Transform, "PathSystem::polyline",
                        "Rejected condition: !detail::finiteVector(p)",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    PathFunction{});

    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());

        if (points.size() < 4)
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Transform, "PathSystem::polyline",
                        "Rejected condition: points.size() < 4",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    PathFunction{});
    }

    std::vector<double> distances{0};

    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));

        if (!std::isfinite(length) || length <= 1e-12)
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Transform, "PathSystem::polyline",
                        "Rejected condition: !std::isfinite(length) || length <= 1e-12",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    PathFunction{});
        distances.push_back(distances.back() + length);
    }

    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1 : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);

        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight) + glm::dvec3(points[i]) * weight);
    };
}

PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}

PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u, const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::rectangle",
                                "Rejected condition: !detail::finiteVector(center) || !detail::motionPlane(u, v)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);

    if (std::abs(glm::dot(*a, *b)) > 1e-5f)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::rectangle",
                                "Rejected condition: std::abs(glm::dot(*a, *b)) > 1e-5f",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}

double PathSystem::linear(double progress) {
    return progress;
}

double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics

// --- Dynamics.cpp ---

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

// --- ObjLoader.cpp ---

namespace graphics::detail {
struct ObjParseError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct ObjCorner {
    long long position = 0, uv = 0, normal = 0;

    bool operator==(const ObjCorner& b) const {
        return position == b.position && uv == b.uv && normal == b.normal;
    }
};

struct ObjFace {
    std::vector<ObjCorner> corners;
    std::size_t positions = 0, uvs = 0, normals = 0, line = 0, batch = 0;
    std::string name;
};

void objWarning(std::vector<std::string>& warnings, const std::string& message) {
    if (std::find(warnings.begin(), warnings.end(), message) == warnings.end())
        warnings.push_back(message);
}

long long objInteger(const std::string& text) {
    const auto start = (!text.empty() && (text[0] == '-' || text[0] == '+')) ? 1u : 0u;

    if (start == text.size() || text.find_first_not_of("0123456789", start) != std::string::npos)
        throw ObjParseError("Invalid OBJ index: " + text);

    try {
        const auto value = std::stoll(text);

        if (value == 0)
            throw ObjParseError("OBJ indices start at 1; zero is invalid.");

        return value;
    } catch (const std::out_of_range&) {
        throw ObjParseError("OBJ index is too large.");
    }
}

ObjCorner objCorner(const std::string& text) {
    ObjCorner result;
    const auto a = text.find('/'), b = a == std::string::npos ? a : text.find('/', a + 1);

    if (b != std::string::npos && text.find('/', b + 1) != std::string::npos)
        throw ObjParseError("Too many fields in OBJ corner: " + text);
    result.position = objInteger(text.substr(0, a));

    if (a != std::string::npos) {
        const auto part = text.substr(a + 1, b == std::string::npos ? b : b - a - 1);

        if (!part.empty())
            result.uv = objInteger(part);
        else if (b == std::string::npos)
            throw ObjParseError("Missing UV after '/'.");
    }

    if (b != std::string::npos) {
        if (b + 1 == text.size())
            throw ObjParseError("Missing normal after '//'.");
        result.normal = objInteger(text.substr(b + 1));
    }

    return result;
}

std::size_t objResolve(long long value, std::size_t atFace, std::size_t total, bool compatible) {
    // Negative references use counts at the face, even during the second pass.
    if (value < 0) {
        if (value < -static_cast<long long>(atFace))
            throw ObjParseError("Negative OBJ index is out of range.");

        return std::size_t(static_cast<long long>(atFace) + value);
    }

    if (static_cast<unsigned long long>(value) > (compatible ? total : atFace))
        throw ObjParseError("Positive OBJ index is out of range or references a later record.");

    return std::size_t(value - 1);
}

std::vector<float> objNumbers(std::istringstream& row) {
    std::vector<float> result;
    std::string token;

    while (row >> token) {
        std::istringstream number(token);

        number.imbue(std::locale::classic());

        double value = 0;
        char extra = 0;

        if (!(number >> value) || (number >> extra) || !std::isfinite(value) ||
            std::abs(value) > std::numeric_limits<float>::max())
            throw ObjParseError("Invalid finite OBJ number: " + token);
        result.push_back(float(value));
    }

    return result;
}

std::string objText(const std::string& bytes, bool compatible, std::vector<std::string>& warnings) {
    std::string text = bytes;

    if (bytes.size() >= 2 &&
        ((static_cast<unsigned char>(bytes[0]) == 0xff && static_cast<unsigned char>(bytes[1]) == 0xfe) ||
         (static_cast<unsigned char>(bytes[0]) == 0xfe && static_cast<unsigned char>(bytes[1]) == 0xff))) {
        if (!compatible)
            throw ObjParseError("UTF-16 BOM requires the compatible text reader.");

        if (bytes.size() % 2)
            throw ObjParseError("Truncated UTF-16 text.");

        const bool little = static_cast<unsigned char>(bytes[0]) == 0xff;
        auto unit = [&](std::size_t i) {
            const auto a = static_cast<unsigned char>(bytes[i]), b = static_cast<unsigned char>(bytes[i + 1]);

            return unsigned(little ? (a | (b << 8)) : ((a << 8) | b));
        };
        text.clear();

        for (std::size_t i = 2; i < bytes.size(); i += 2) {
            unsigned c = unit(i);

            if (c >= 0xd800 && c <= 0xdbff) {
                if (i + 3 >= bytes.size())
                    throw ObjParseError("Truncated UTF-16 surrogate pair.");

                const auto low = unit(i + 2);

                if (low < 0xdc00 || low > 0xdfff)
                    throw ObjParseError("Invalid UTF-16 surrogate pair.");
                c = 0x10000 + ((c - 0xd800) << 10) + (low - 0xdc00);
                i += 2;
            } else if (c >= 0xdc00 && c <= 0xdfff)
                throw ObjParseError("Unpaired UTF-16 surrogate.");

            if (c < 0x80)
                text += char(c);
            else if (c < 0x800) {
                text += char(0xc0 | (c >> 6));
                text += char(0x80 | (c & 63));
            } else if (c < 0x10000) {
                text += char(0xe0 | (c >> 12));
                text += char(0x80 | ((c >> 6) & 63));
                text += char(0x80 | (c & 63));
            } else {
                text += char(0xf0 | (c >> 18));
                text += char(0x80 | ((c >> 12) & 63));
                text += char(0x80 | ((c >> 6) & 63));
                text += char(0x80 | (c & 63));
            }
        }
        objWarning(warnings, "Decoded UTF-16 BOM text as UTF-8.");
    }

    if (text.compare(0, 3, "\xEF\xBB\xBF") == 0)
        text.erase(0, 3);

    if (text.find('\0') != std::string::npos)
        throw ObjParseError("NUL byte in OBJ text; unsupported encoding or binary file.");

    std::string lines;

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\r') {
            lines += '\n';

            if (i + 1 < text.size() && text[i + 1] == '\n')
                ++i;
        } else
            lines += text[i];
    }

    return lines;
}

Model parseObj(const std::string& bytes, const std::string& name, const LoadOptions& options, bool compatible,
               std::vector<std::string>& warnings) {
    std::istringstream input(objText(bytes, compatible, warnings));
    std::vector<glm::vec3> positions, normals;
    std::vector<glm::vec2> uvs;
    std::vector<GlColor> colors;
    std::vector<ObjFace> faces;
    bool hasColors = false;
    std::string physical, line, group = "face";
    std::size_t lineNumber = 0, start = 0, batch = 0;

    while (std::getline(input, physical)) {
        ++lineNumber;

        if (line.empty())
            start = lineNumber;
        physical = physical.substr(0, physical.find('#'));

        const auto end = physical.find_last_not_of(" \t");
        const bool continued = end != std::string::npos && physical[end] == '\\';

        line += continued ? physical.substr(0, end) : physical;

        if (continued) {
            line += ' ';
            continue;
        }

        std::istringstream row(line);

        row.imbue(std::locale::classic());

        std::string kind;

        row >> kind;

        try {
            if (kind == "v") {
                const auto values = objNumbers(row);

                if (values.size() != 3 && values.size() != 4 && values.size() != 6)
                    throw ObjParseError("Expected v x y z [w] or v x y z r g b.");

                glm::vec3 p(values[0], values[1], values[2]);

                if (values.size() == 4) {
                    if (values[3] == 0)
                        throw ObjParseError("Homogeneous vertex has zero w.");
                    p /= values[3];
                }

                if (!finiteVector(p))
                    throw ObjParseError("Vertex overflows after division by w.");
                positions.push_back(p);
                colors.push_back(values.size() == 6 ? GlColor{values[3], values[4], values[5]} : GlColor{});
                hasColors = hasColors || values.size() == 6;
            } else if (kind == "vt") {
                const auto values = objNumbers(row);

                if (values.empty() || values.size() > 3 || (values.size() == 3 && values[2] != 0))
                    throw ObjParseError("Expected vt u [v [0]]; volume texture coordinates are unsupported.");
                uvs.push_back({values[0], values.size() > 1 ? values[1] : 0});
            } else if (kind == "vn") {
                const auto values = objNumbers(row);

                if (values.size() != 3)
                    throw ObjParseError("Expected vn x y z.");

                const glm::dvec3 normal(values[0], values[1], values[2]);

                if (glm::length(normal) == 0)
                    throw ObjParseError("Zero OBJ normal.");
                normals.push_back(glm::vec3(glm::normalize(normal)));
            } else if (kind == "f") {
                ObjFace face;

                face.positions = positions.size();
                face.uvs = uvs.size();
                face.normals = normals.size();
                face.line = start;
                face.batch = batch;
                face.name = group;

                std::string token;

                while (row >> token)
                    face.corners.push_back(objCorner(token));

                if (face.corners.size() < 3)
                    throw ObjParseError("Face has fewer than three corners.");
                faces.push_back(std::move(face));
            } else if (kind == "o" || kind == "g") {
                std::getline(row, group);

                const auto first = group.find_first_not_of(" \t");

                group = first == std::string::npos ? "face" : group.substr(first);
                ++batch;
            } else if (kind == "mtllib" || kind == "usemtl") {
                ++batch;
                objWarning(warnings,
                           "MTL materials are not loaded; set LoadOptions.texturePath or a Scene material explicitly.");
            } else if (kind == "s") {
                ++batch;

                std::string smoothing;

                row >> smoothing;

                if (smoothing != "off" && smoothing != "0")
                    objWarning(
                        warnings,
                        "Smoothing groups are not interpolated; supplied vn values are preserved and missing normals are generated per face.");
            } else if (!kind.empty())
                throw ObjParseError("Unsupported OBJ record: " + kind);
        } catch (const ObjParseError& e) {
            throw ObjParseError("line " + std::to_string(start) + ": " + e.what());
        }
        line.clear();
    }

    if (!line.empty())
        throw ObjParseError("line " + std::to_string(start) + ": unterminated line continuation.");

    Model model;

    model.shape = GLShape::Custom;
    model.name = name;

    Model segment;
    std::size_t activeBatch = 0;
    auto flush = [&] {
        if (options.mergeCoplanarFaces && !hasColors)
            mergeCoplanar(segment);

        for (auto& face : segment.faces)
            model.faces.push_back(std::move(face));
        segment.faces.clear();
    };

    for (auto& face : faces) {
        try {
            if (face.batch != activeBatch) {
                flush();
                activeBatch = face.batch;
            }

            if (compatible) {
                const auto old = face.corners.size();

                face.corners.erase(std::unique(face.corners.begin(), face.corners.end()), face.corners.end());

                if (face.corners.size() > 1 && face.corners.front() == face.corners.back())
                    face.corners.pop_back();

                if (face.corners.size() != old)
                    objWarning(warnings,
                               "Removed repeated identical OBJ corners at line " + std::to_string(face.line) + ".");
            }

            Mesh mesh;

            mesh.name = face.name + "_" + std::to_string(face.line);

            std::size_t uvCount = 0;
            bool forward = false;

            for (const auto& corner : face.corners) {
                const auto p = objResolve(corner.position, face.positions, positions.size(), compatible);

                mesh.localVertices.push_back(positions[p]);

                if (hasColors)
                    mesh.vertexColors.push_back(colors[p]);
                forward = forward ||
                          (corner.position > 0 && static_cast<unsigned long long>(corner.position) > face.positions);

                glm::vec2 uv(0);

                if (corner.uv) {
                    uv = uvs[objResolve(corner.uv, face.uvs, uvs.size(), compatible)];
                    ++uvCount;
                    forward = forward || (corner.uv > 0 && static_cast<unsigned long long>(corner.uv) > face.uvs);
                }
                mesh.texcoords.push_back(uv);
            }
            // Normalize only the triangulation workspace; retain original coordinates.
            if (mesh.localVertices.size() < 3)
                throw ObjParseError("Face has fewer than three distinct corners.");

            glm::dvec3 low(mesh.localVertices[0]), high(low);

            for (const auto& p : mesh.localVertices) {
                low = glm::min(low, glm::dvec3(p));
                high = glm::max(high, glm::dvec3(p));
            }

            const auto size = high - low;
            const double scale = std::max({size.x, size.y, size.z});

            if (scale == 0)
                throw ObjParseError("Degenerate polygon.");

            Mesh workspace;

            for (const auto& p : mesh.localVertices)
                workspace.localVertices.push_back(glm::vec3((glm::dvec3(p) - low) / scale));

            if (workspace.localVertices.size() > 3) {
                glm::dvec3 plane(0);
                const auto count = workspace.localVertices.size();

                for (std::size_t i = 0; i < count; ++i) {
                    const auto a = glm::dvec3(workspace.localVertices[i]);
                    const auto b = glm::dvec3(workspace.localVertices[(i + 1) % count]);

                    plane += glm::cross(a, b);
                }

                if (glm::length(plane) < 1e-12)
                    throw ObjParseError("Degenerate polygon plane.");
                plane = glm::normalize(plane);

                const auto absolute = glm::abs(plane);
                const int axis =
                    absolute.x > absolute.y ? (absolute.x > absolute.z ? 0 : 2) : (absolute.y > absolute.z ? 1 : 2);
                std::vector<glm::dvec2> projected;

                for (const auto& p : workspace.localVertices) {
                    if (std::abs(glm::dot(plane, glm::dvec3(p - workspace.localVertices[0]))) > 1e-5)
                        throw ObjParseError("Non-planar OBJ polygon; export triangulated faces.");
                    projected.push_back(axis == 0   ? glm::dvec2(p.y, p.z)
                                        : axis == 1 ? glm::dvec2(p.x, p.z)
                                                    : glm::dvec2(p.x, p.y));
                }

                auto cross = [](glm::dvec2 a, glm::dvec2 b, glm::dvec2 c) {
                    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
                };

                auto on = [](glm::dvec2 a, glm::dvec2 b, glm::dvec2 p) {
                    return p.x >= std::min(a.x, b.x) - 1e-14 && p.x <= std::max(a.x, b.x) + 1e-14 &&
                           p.y >= std::min(a.y, b.y) - 1e-14 && p.y <= std::max(a.y, b.y) + 1e-14;
                };

                for (std::size_t i = 0; i < count; ++i)
                    for (std::size_t j = i + 1; j < count; ++j) {
                        if ((i + 1) % count == j || (j + 1) % count == i)
                            continue;

                        const auto a = projected[i], b = projected[(i + 1) % count], c = projected[j],
                                   d = projected[(j + 1) % count];
                        const auto abC = cross(a, b, c), abD = cross(a, b, d), cdA = cross(c, d, a),
                                   cdB = cross(c, d, b);
                        const bool crossing = ((abC > 1e-14 && abD < -1e-14) || (abC < -1e-14 && abD > 1e-14)) &&
                                              ((cdA > 1e-14 && cdB < -1e-14) || (cdA < -1e-14 && cdB > 1e-14));

                        if (crossing || (std::abs(abC) <= 1e-14 && on(a, b, c)) ||
                            (std::abs(abD) <= 1e-14 && on(a, b, d)) || (std::abs(cdA) <= 1e-14 && on(c, d, a)) ||
                            (std::abs(cdB) <= 1e-14 && on(c, d, b)))
                            throw ObjParseError("Self-intersection or repeated corner in OBJ polygon.");
                    }
            }

            try {
                mesh.triangleIndices = triangulate(workspace.localVertices);
            } catch (const std::runtime_error& e) {
                throw ObjParseError(e.what());
            }
            workspace.triangleIndices = mesh.triangleIndices;
            recalculateNormals(workspace);
            mesh.vertexNormals = std::move(workspace.vertexNormals);

            for (std::size_t i = 0; i < face.corners.size(); ++i)
                if (face.corners[i].normal) {
                    const auto value = face.corners[i].normal;

                    mesh.vertexNormals[i] = normals[objResolve(value, face.normals, normals.size(), compatible)];
                    forward = forward || (value > 0 && static_cast<unsigned long long>(value) > face.normals);
                }

            if (forward)
                objWarning(warnings, "Resolved positive forward references at line " + std::to_string(face.line) + ".");

            if (uvCount == 0)
                mesh.texcoords.clear();
            else if (uvCount != face.corners.size())
                objWarning(warnings, "Filled missing UV corners with (0,0) at line " + std::to_string(face.line) + ".");

            const auto& indices = mesh.triangleIndices;

            mesh.normal =
                glm::normalize(glm::cross(workspace.localVertices[indices[1]] - workspace.localVertices[indices[0]],
                                          workspace.localVertices[indices[2]] - workspace.localVertices[indices[0]]));

            if (!validMesh(mesh))
                throw ObjParseError("Invalid or degenerate OBJ mesh.");
            segment.faces.push_back(std::move(mesh));
        } catch (const ObjParseError& e) {
            throw ObjParseError("line " + std::to_string(face.line) + ": " + e.what());
        }
    }
    flush();

    if (options.mergeCoplanarFaces && hasColors)
        objWarning(warnings, "Coplanar merging is skipped to preserve vertex colors.");

    if (model.faces.empty())
        throw ObjParseError("OBJ contains no polygon faces.");

    return model;
}
} // namespace graphics::detail

namespace graphics {
ModelResult AssetLoader::loadObj(const std::filesystem::path& relative, const LoadOptions& options) const {
    ModelResult result;
    const auto utf8 = relative.u8string();
    const std::string name(utf8.begin(), utf8.end());

    try {
        if (!options.texturePath.empty() && !features_.textures)
            throw std::runtime_error("Enable texture mode before specifying texturePath.");

        std::ifstream input(resolve(relative), std::ios::binary);

        if (!input)
            throw std::runtime_error("Cannot open OBJ text.");

        const std::string bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

        if (input.bad())
            throw std::runtime_error("OBJ file read failed.");

        Model model;

        try {
            model = detail::parseObj(bytes, name, options, false, result.warnings);
        } catch (const detail::ObjParseError& first) {
            if (!options.allowObjFallback)
                throw;
            result.warnings.clear();

            try {
                model = detail::parseObj(bytes, name, options, true, result.warnings);
            } catch (const detail::ObjParseError& second) {
                throw std::runtime_error(std::string("Standard reader: ") + first.what() +
                                         "; compatible reader: " + second.what());
            }
            result.usedFallback = true;
            detail::objWarning(result.warnings, std::string("Compatible OBJ reader used after: ") + first.what());
        }

        if (!options.texturePath.empty()) {
            auto image = texture(options.texturePath, options.flipTextureRows);

            for (auto& mesh : model.faces) {
                if (mesh.texcoords.empty())
                    throw std::runtime_error("Explicit texture requires UV coordinates on every face.");
                mesh.material.texture = image;
                mesh.material.useTexture = true;
            }
        }

        auto finished = detail::finishLoad(std::move(model), options);

        finished.warnings = std::move(result.warnings);
        finished.usedFallback = result.usedFallback;

        return finished;
    } catch (const std::exception& e) {
        result.error = name + ": " + e.what();
        std::cerr << "[OBJ load failed] " << result.error << '\n'
                  << "Check the file path and the reported cause first, including texture settings.\n"
                  << "Supported OBJ text format (.obj or .txt), one record per line:\n"
                  << "  v x y z       : vertex position\n"
                  << "  vt u [v]      : optional texture coordinate\n"
                  << "  vn x y z      : optional nonzero normal\n"
                  << "  f 1 2 3       : face using 1-based vertex indices\n"
                  << "  Corner forms: v, v/vt, v//vn, v/vt/vn; negative relative indices are allowed.\n"
                  << "Minimal triangle example:\n"
                  << "  v 0 0 0\n  v 1 0 0\n  v 0 1 0\n  f 1 2 3\n"
                  << "If parsing still fails, inspect the model's source format and write a parser for it.\n"
                  << "Fill graphics::Mesh.localVertices and triangleIndices, then call\n"
                  << "graphics::recalculateNormals(mesh), add the mesh to graphics::Model.faces,\n"
                  << "and pass that model to Scene::addModel. See framework/docs/OBJ_LOADING.md.\n";

        return result;
    }
}
} // namespace graphics

// --- Motion.cpp ---

namespace graphics {
bool Motion::validStep(const MotionStep& step) {
    return (bool(step.transform) != bool(step.action)) && std::isfinite(step.startSeconds) && step.startSeconds >= 0 &&
           (!step.durationSeconds || (std::isfinite(*step.durationSeconds) && *step.durationSeconds >= 0 &&
                                      std::isfinite(step.startSeconds + *step.durationSeconds)));
}

bool Motion::append(const MotionStep& step) {
    return insert(steps_.size(), step);
}

bool Motion::insert(std::size_t index, const MotionStep& step) {
    if (index > steps_.size() || !validStep(step))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "Motion::insert",
            "Rejected condition: index > steps_.size() || !validStep(step)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
    steps_.insert(steps_.begin() + index, step);

    return true;
}

MotionStep Motion::action(MotionAction callback, double startSeconds, std::optional<double> durationSeconds) {
    MotionStep step;
    step.action = std::move(callback);
    step.startSeconds = startSeconds;
    step.durationSeconds = durationSeconds;
    return step;
}

bool Motion::insertAction(std::size_t index, MotionAction callback, double startSeconds,
                          std::optional<double> durationSeconds) {
    return insert(index, action(std::move(callback), startSeconds, durationSeconds));
}

bool Motion::replace(std::size_t index, const MotionStep& step) {
    if (index >= steps_.size() || !validStep(step))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "Motion::replace",
            "Rejected condition: index >= steps_.size() || !validStep(step)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
    steps_[index] = step;

    return true;
}

bool Motion::erase(std::size_t index) {
    if (index >= steps_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "Motion::erase", "Rejected condition: index >= steps_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
    steps_.erase(steps_.begin() + index);

    return true;
}

bool Motion::then(MotionStep step) {
    double end = 0;

    for (const auto& current : steps_) {
        if (!current.durationSeconds)
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "Motion::then",
                "Rejected condition: !current.durationSeconds",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
        end = std::max(end, current.startSeconds + *current.durationSeconds);
    }
    step.startSeconds += end;

    return append(step);
}

std::optional<MotionSample> Motion::sample(double seconds) const {
    return evaluate(seconds, false);
}

std::optional<MotionSample> Motion::execute(double seconds) const {
    // Snapshot permits callbacks to edit the original sequence without invalidating iteration.
    const Motion snapshot = *this;
    return snapshot.evaluate(seconds, true);
}

std::optional<MotionSample> Motion::evaluate(double seconds, bool actions) const {
    if (!std::isfinite(seconds) || seconds < 0)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "Motion::sample",
                                "Rejected condition: !std::isfinite(seconds) || seconds < 0",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    MotionSample result;

    for (const auto& step : steps_) {
        if (!step.enabled || seconds < step.startSeconds)
            continue;

        double time = seconds - step.startSeconds;

        if (step.durationSeconds)
            time = std::min(time, *step.durationSeconds);

        if (step.action) {
            if (actions && !step.action(time, result))
                return (Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "Motion::execute",
                                            "Action returned false; prior external effects are retained."), std::nullopt);
            continue;
        }
        const auto matrix = step.transform(time);

        if (!TransformSystem::validAffine(matrix))
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Motion, "Motion::sample",
                        "Rejected condition: !TransformSystem::validAffine(matrix)",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    std::nullopt);

        auto& target = step.space == TransformSpace::World ? result.parent : result.local;

        target = matrix * target; // Within each space, API order is application order.

        if (!TransformSystem::validAffine(target))
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Motion, "Motion::sample",
                        "Rejected condition: !TransformSystem::validAffine(target)",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    std::nullopt);
    }

    return result;
}

MotionStep Motion::fixed(const glm::mat4& matrix, TransformSpace space) {
    MotionStep step;

    if (TransformSystem::validAffine(matrix))
        step.transform = [matrix](double) {
            return matrix;
        };
    step.space = space;
    step.durationSeconds = 0;

    return step;
}

MotionStep Motion::rotation(const RotationMotion& config) {
    MotionStep step;

    if (!std::isfinite(config.degreesPerSecond) || !std::isfinite(config.initialDegrees) ||
        !TransformSystem::rotation(config.axis, config.initialDegrees))
        return step;
    step.space = config.space;
    step.transform = [config](double seconds) {
        const auto matrix =
            TransformSystem::rotation(config.axis, config.initialDegrees + config.degreesPerSecond * seconds);

        if (!matrix)
            throw std::domain_error("Rotation motion produced an invalid matrix.");

        return *matrix;
    };

    return step;
}

MotionStep Motion::followPath(const PathMotion& config) {
    MotionStep step;

    if (!config.path || !config.interpolation || !std::isfinite(config.durationSeconds) ||
        config.durationSeconds <= 0 || !std::isfinite(config.rate) || config.rate == 0 ||
        !std::isfinite(config.phase) || config.phase < 0 || config.phase > 1)
        return step;
    step.space = config.space;

    if (config.playback == Playback::Once) {
        step.durationSeconds =
            (config.rate > 0 ? 1 - config.phase : config.phase) * config.durationSeconds / std::abs(config.rate);

        if (!std::isfinite(*step.durationSeconds))
            return MotionStep{};
    }
    step.transform = [config](double seconds) {
        double progress = config.phase + seconds / config.durationSeconds * config.rate;

        if (!std::isfinite(progress))
            throw std::domain_error("Path time overflow.");

        if (config.playback == Playback::Once)
            progress = std::clamp(progress, 0.0, 1.0);
        else if (config.playback == Playback::Loop)
            progress -= std::floor(progress);
        else {
            progress -= std::floor(progress / 2) * 2;

            if (progress > 1)
                progress = 2 - progress;
        }

        const double u = config.interpolation(progress);

        if (!std::isfinite(u))
            throw std::domain_error("Interpolation returned non-finite progress.");

        glm::vec3 offset = config.path(u);

        if (config.relativeToStart) {
            const double initial = config.interpolation(config.phase);

            if (!std::isfinite(initial))
                throw std::domain_error("Invalid initial interpolation.");
            offset -= config.path(initial);
        }

        const auto matrix = TransformSystem::translation(offset);

        if (!matrix)
            throw std::domain_error("Path returned a non-finite position.");

        return *matrix;
    };

    return step;
}

MotionStep Motion::linear(const LinearMotion& config) {
    if (!detail::finiteVector(config.from) || !detail::finiteVector(config.to) ||
        !std::isfinite(config.durationSeconds) || config.durationSeconds <= 0 ||
        (config.playback != Playback::Once && config.playback != Playback::Loop &&
         config.playback != Playback::PingPong) ||
        (config.space != TransformSpace::World && config.space != TransformSpace::Local))
        return {};

    PathMotion path;

    path.path = [from = glm::dvec3(config.from), to = glm::dvec3(config.to)](double u) {
        return glm::vec3((1 - u) * from + u * to);
    };
    path.durationSeconds = config.durationSeconds;
    path.playback = config.playback;
    path.space = config.space;

    return followPath(path);
}

MotionStep Motion::pendulum(const PendulumMotion& config) {
    MotionStep step;

    if (!std::isfinite(config.amplitudeDegrees) || config.amplitudeDegrees < 0 ||
        !std::isfinite(config.periodSeconds) || config.periodSeconds <= 0 || !std::isfinite(config.phaseDegrees) ||
        !std::isfinite(config.offsetDegrees) ||
        !std::isfinite(std::abs(config.offsetDegrees) + config.amplitudeDegrees) ||
        (config.space != TransformSpace::World && config.space != TransformSpace::Local) ||
        !TransformSystem::rotation(config.axis, config.offsetDegrees))
        return step;
    step.space = config.space;
    step.transform = [config](double seconds) {
        constexpr double pi = 3.14159265358979323846;
        const double phase = std::remainder(seconds, config.periodSeconds) / config.periodSeconds * 2 * pi +
                             std::remainder(config.phaseDegrees, 360.0) * pi / 180;
        const auto matrix =
            TransformSystem::rotation(config.axis, config.offsetDegrees + config.amplitudeDegrees * std::sin(phase));

        if (!matrix)
            throw std::domain_error("Pendulum motion produced an invalid matrix.");

        return *matrix;
    };

    return step;
}

std::optional<MotionId> MotionSystem::createMotion(const Motion& motion) {
    const auto sample = motion.sample(0);

    if (!sample || nextMotion_ == std::numeric_limits<MotionId>::max())
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createMotion",
                                "Rejected condition: !sample || nextMotion_ == std::numeric_limits<MotionId>::max()",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const auto id = nextMotion_;
    MotionState state;

    state.motion = motion;
    state.sampled = *sample;
    motions_.emplace(id, std::move(state));
    ++nextMotion_;

    return id;
}

const MotionState* MotionSystem::getMotion(MotionId id) const {
    const auto found = motions_.find(id);

    return found == motions_.end() ? nullptr : &found->second;
}

bool MotionSystem::setMotion(MotionId id, const Motion& motion) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setMotion",
            "Rejected condition: found == motions_.end()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto sample = motion.sample(found->second.timeSeconds);

    if (!sample)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setMotion", "Rejected condition: !sample",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.motion = motion;
    found->second.sampled = *sample;

    return true;
}

bool MotionSystem::setMotionPlaying(MotionId id, bool playing) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setMotionPlaying",
            "Rejected condition: found == motions_.end()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.playing = playing;

    return true;
}

bool MotionSystem::setMotionEnabled(MotionId id, bool enabled) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setMotionEnabled",
            "Rejected condition: found == motions_.end()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.enabled = enabled;

    return true;
}

bool MotionSystem::seekMotion(MotionId id, double seconds) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::seekMotion",
            "Rejected condition: found == motions_.end()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto sample = found->second.motion.sample(seconds);

    if (!sample)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::seekMotion", "Rejected condition: !sample",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.timeSeconds = seconds;
    found->second.sampled = *sample;

    return true;
}

bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::removeMotion",
            "Rejected condition: !motions_.erase(id)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    for (auto& layer : layers_)
        if (layer.second.config.motion == id)
            layer.second.config.motion.reset();

    return true;
}

bool MotionSystem::update(double deltaSeconds) {
    if (!std::isfinite(deltaSeconds) || deltaSeconds < 0)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::update",
            "Rejected condition: !std::isfinite(deltaSeconds) || deltaSeconds < 0",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");

    struct Pending {
        MotionId id;
        double time;
        MotionSample sample;
        std::optional<Motion> actions;
    };

    std::vector<Pending> pending;

    pending.reserve(motions_.size());

    for (const auto& entry : motions_) {
        const auto& state = entry.second;

        if (!state.playing)
            continue;

        const double time = state.timeSeconds + deltaSeconds;
        const auto sample = state.motion.sample(time);

        if (!sample)
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::update", "Rejected condition: !sample",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
        const auto& steps = state.motion.getSteps();
        const bool actions = state.enabled && std::any_of(steps.begin(), steps.end(),
                                                         [](const auto& step) { return bool(step.action); });
        pending.push_back({entry.first, time, *sample, actions ? std::optional<Motion>(state.motion) : std::nullopt});
    }

    for (const auto& item : pending) {
        auto& state = motions_.at(item.id);

        state.timeSeconds = item.time;
        state.sampled = item.sample;
    }
    for (const auto& item : pending)
        if (item.actions && !item.actions->execute(item.time))
            return false;

    return true;
}

void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};

    motions_.clear();
    layers_.swap(emptyLayers);
}

bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform) && (!config.motion || getMotion(*config.motion));
}

const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);

    return found == layers_.end() ? nullptr : &found->second;
}

std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max())
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::pushLayer",
                "Rejected condition: !getLayer(parent) || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max()",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    auto staged = layers_;
    const auto id = nextLayer_;

    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;

    return id;
}

std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);

    if (!current || !current->parent || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max())
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::insertLayer",
                "Rejected condition: !current || !current->parent || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max()",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const auto parent = *current->parent;
    const auto id = nextLayer_;
    auto staged = layers_;

    staged.emplace(id, LayerState{config, parent, {child}});

    auto& siblings = staged.at(parent).children;

    std::replace(siblings.begin(), siblings.end(), child, id);
    staged.at(child).parent = id;
    layers_.swap(staged);
    ++nextLayer_;

    return id;
}

bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);

    if (!current || !current->parent)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::removeLayer",
            "Rejected condition: !current || !current->parent",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());

    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());

    for (auto child : current->children)
        staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);

    return true;
}

std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);

    if (!current || !current->parent || !current->children.empty())
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::popLayer",
                                "Rejected condition: !current || !current->parent || !current->children.empty()",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const auto parent = *current->parent;

    if (!removeLayer(top))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::popLayer",
                                "Rejected condition: !removeLayer(top)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return parent;
}

bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);

    if (!current || !current->parent || !getLayer(parent))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::reparentLayer",
            "Rejected condition: !current || !current->parent || !getLayer(parent)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    if (current->parent == parent)
        return true;

    auto ancestor = std::optional<LayerId>(parent);

    while (ancestor) {
        if (*ancestor == id)
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::reparentLayer",
                "Rejected condition: *ancestor == id",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
        ancestor = layers_.at(*ancestor).parent;
    }

    auto staged = layers_;
    auto& old = staged.at(*current->parent).children;

    old.erase(std::find(old.begin(), old.end(), id));
    staged.at(parent).children.push_back(id);
    staged.at(id).parent = parent;
    layers_.swap(staged);

    return true;
}

bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);

    if (found == layers_.end() || !validLayer(config))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setLayer",
            "Rejected condition: found == layers_.end() || !validLayer(config)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.config = config;

    return true;
}

bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);

    if (!current || (motion && !getMotion(*motion)))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::bindMotion",
            "Rejected condition: !current || (motion && !getMotion(*motion))",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");

    auto config = current->config;

    config.motion = motion;

    return setLayer(layer, config);
}

std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);

    while (current) {
        const auto layer = getLayer(*current);

        if (!layer || chain.size() >= layers_.size())
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::layerMatrices",
                        "Rejected condition: !layer || chain.size() >= layers_.size()",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    std::nullopt);
        chain.push_back(*current);
        current = layer->parent;
    }

    MotionSample result;

    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;

        if (!config.enabled)
            continue;

        MotionSample own;

        if (config.space == TransformSpace::World)
            own.parent = config.transform;
        else
            own.local = config.transform;

        if (config.motion) {
            const auto state = getMotion(*config.motion);

            if (!state)
                return (Diagnostics::failAt(
                            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::layerMatrices",
                            "Rejected condition: !state",
                            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                        std::nullopt);

            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }

        if (*i == rootLayer())
            result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }

        if (!TransformSystem::validAffine(result.parent) || !TransformSystem::validAffine(result.local))
            return (
                Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::layerMatrices",
                    "Rejected condition: !TransformSystem::validAffine(result.parent) || !TransformSystem::validAffine(result.local)",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                std::nullopt);
    }

    return result;
}

std::optional<MotionId> MotionSystem::createMotion(std::initializer_list<MotionStep> steps) {
    Motion motion;
    for (const auto& step : steps)
        if (!motion.append(step)) return std::nullopt;
    return createMotion(motion);
}

std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    return createMotion({Motion::rotation(config)});
}

std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    return createMotion({Motion::rotation(config)});
}
} // namespace graphics

// --- Scene.cpp ---

namespace graphics {
namespace detail {

bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f && value.y > 0.0f && value.z > 0.0f;
}

std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;

    return ++revision;
}

void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color) {
    PlaneFigure face;

    face.shape = shape;
    face.localVertices = vertices;
    face.color = color;
    face.normal = glm::normalize(
        glm::cross(face.localVertices[1] - face.localVertices[0], face.localVertices[2] - face.localVertices[0]));
    polyhedron.faces.push_back(face);
}

} // namespace detail

using detail::finiteVector;
using detail::finiteMatrix;
using detail::validColor;
using detail::positiveLengths;
using detail::nextGeometryRevision;
using detail::appendFace;

Scene::Scene() {
    geometryRevision_ = nextGeometryRevision();
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    lines_.reserve(MAX_LINES);
}

std::optional<EntityId> Scene::addGroup(const Transform3D& transform) {
    if (!DiagramSystem::validTransform(transform))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Scene::addGroup",
                                "Rejected condition: !DiagramSystem::validTransform(transform)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const auto id = components_.size();
    Component_Poly group;

    group.polyhedron.shape = GLShape::None;
    group.transform = transform;
    group.sideLengths = glm::vec3(0);
    group.transformOnly = true;
    components_.push_back(group);
    polyId_.push_back(id);
    geometryRevision_ = nextGeometryRevision();

    return id;
}

bool Scene::setTint(EntityId id, GlColor color) {
    if (id >= components_.size() || !validColor(color))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTint",
            "Rejected condition: id>=components_.size() || !validColor(color)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    components_[id].tint = color;

    return true;
}

std::optional<EntityId> Scene::addSphere(float radius, const glm::vec3& position, unsigned slices, unsigned stacks) {
    Transform3D transform;

    transform.basis = position;

    return push_entity(DiagramSystem::makeSphere(radius, slices, stacks), transform);
}

std::optional<EntityId> Scene::addCone(float radius, float height, const glm::vec3& position, unsigned slices) {
    Transform3D transform;

    transform.basis = position;

    return push_entity(DiagramSystem::makeCone(radius, height, slices), transform);
}

std::optional<EntityId> Scene::push_entity(const Polyhedron& polyhedron, const Transform3D& transform) {
    if (!DiagramSystem::validPolyhedron(polyhedron) || !DiagramSystem::validTransform(transform)) {
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::push_entity",
                "Rejected condition: !DiagramSystem::validPolyhedron(polyhedron) || !DiagramSystem::validTransform(transform)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);
    }

    glm::vec3 minimum = polyhedron.faces[0].localVertices[0];
    glm::vec3 maximum = minimum;

    for (const PlaneFigure& face : polyhedron.faces) {
        for (const glm::vec3& vertex : face.localVertices) {
            minimum = glm::min(minimum, vertex);
            maximum = glm::max(maximum, vertex);
        }
    }

    if (!finiteVector(maximum - minimum))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Scene::push_entity",
                                "Rejected condition: !finiteVector(maximum - minimum)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const EntityId id = components_.size();

    components_.push_back({polyhedron, transform, maximum - minimum});
    polyId_.push_back(id);
    geometryRevision_ = nextGeometryRevision();

    return id;
}

std::optional<EntityId> Scene::addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition) {
    Transform3D transform;

    transform.basis = worldPosition;

    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths), transform);
}

std::optional<EntityId> Scene::addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition) {
    Transform3D transform;

    transform.basis = worldPosition;

    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths), transform);
}

std::optional<EntityId> Scene::addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition) {
    Transform3D transform;

    transform.basis = worldPosition;

    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths), transform);
}

bool Scene::setFaceFrame(EntityId id, std::size_t faceIndex, const glm::mat4& matrix) {
    if (!getMesh(id, faceIndex) || !TransformSystem::validAffine(matrix))
        return false;
    components_[id].polyhedron.faces[faceIndex].transformFrame = matrix;
    return true;
}

bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVisible",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;

    return true;
}

bool Scene::setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() ||
        (mode != CullMode::Inherit && mode != CullMode::None && mode != CullMode::Back && mode != CullMode::Front))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Raster, "Scene::setFaceCullMode",
                                   "Invalid entity, face index or culling mode.",
                                   "Check the model's face list and CullMode enum.", id);
    components_[id].polyhedron.faces[faceIndex].cull = mode;

    return true;
}

bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix, FaceAnimationSpace space) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix) ||
        (space != FaceAnimationSpace::Local && space != FaceAnimationSpace::Model)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceLocalMatrix",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    components_[id].polyhedron.faces[faceIndex].animationSpace = space;

    return true;
}

bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVertexColors",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];

    if (!colors.empty() && colors.size() != face.localVertices.size()) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVertexColors",
            "Rejected condition: !colors.empty() && colors.size() != face.localVertices.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    for (GlColor color : colors) {
        if (!validColor(color)) {
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVertexColors",
                "Rejected condition: !validColor(color)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
        }
    }
    face.vertexColors = colors;
    geometryRevision_ = nextGeometryRevision();

    return true;
}

const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    // For a parented entity this position is in parent coordinates.
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setPosition",
            "Rejected condition: id >= components_.size() || !finiteVector(worldPosition)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    auto candidate = components_[id].transform;

    candidate.basis = worldPosition;

    return setTransform(id, candidate);
}

bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if (id >= components_.size() || !positiveLengths(sideLengths))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths",
            "Rejected condition: id>=components_.size()||!positiveLengths(sideLengths)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto& c = components_[id];

    if (c.transformOnly)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths", "Rejected condition: c.transformOnly",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = c.polyhedron;
    const auto box = detail::bounds(replacement);
    const auto oldSize = box.second - box.first;

    if (!positiveLengths(oldSize))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths",
            "Rejected condition: !positiveLengths(oldSize)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto ratio = sideLengths / oldSize;

    if (!finiteVector(ratio))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths",
            "Rejected condition: !finiteVector(ratio)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    // Preserve topology/materials. Resize mesh coordinates around the model pivot.
    for (auto& face : replacement.faces) {
        for (auto& vertex : face.localVertices)
            vertex = c.transform.localBasis + (vertex - c.transform.localBasis) * ratio;
        face.transform.localBasis =
            c.transform.localBasis + (face.transform.localBasis - c.transform.localBasis) * ratio;
        recalculateNormals(face);
    }

    return setModel(id, replacement);
}

bool Scene::setParent(EntityId id, std::optional<EntityId> parent) {
    if (id >= components_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setParent",
            "Rejected condition: id >= components_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto ancestor = parent;
    std::size_t remaining = components_.size();

    while (ancestor) {
        if (*ancestor >= components_.size() || *ancestor == id || remaining-- == 0)
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::setParent",
                "Rejected condition: *ancestor >= components_.size() || *ancestor == id || remaining-- == 0",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
        ancestor = components_[*ancestor].parent;
    }
    components_[id].parent = parent;

    return true;
}

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

std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size())
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::worldMatrix",
                    "Rejected condition: id >= components_.size()",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;

    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size())
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Scene, "Scene::worldMatrix",
                        "Rejected condition: *current >= components_.size() || chain.size() >= components_.size()",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                    std::nullopt);
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());

    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;

    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);

        if (binding) {
            layer = *binding;
            anchor = i - 1;
            break;
        }
    }

    const auto motion = motions_.layerMatrices(layer);

    if (!motion)
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::worldMatrix", "Rejected condition: !motion",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    glm::mat4 prefix(1), suffix(1);

    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;

        const auto& component = components_[chain[i]];
        matrix *= component.attachment * detail::transformMatrix(component.transform);
        for (const auto& local : component.localLayers) matrix *= local;
    }

    const auto result = motion->parent * prefix * motion->local * suffix;

    if (!detail::finiteMatrix(result)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Scene::worldMatrix",
                            "World transform overflow after parent and motion composition.",
                            "Reduce coordinate/scale magnitudes and inspect the parent/layer chain.", id);

        return {};
    }

    return result;
}

std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size())
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::faceWorldMatrix",
                "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
            std::nullopt);

    const auto& face = components_[id].polyhedron.faces[faceIndex];
    const auto world = face.transformMode == FaceTransformMode::Hierarchy
                           ? worldMatrix(id) : std::optional<glm::mat4>(glm::mat4(1));

    if (!world)
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::faceWorldMatrix", "Rejected condition: !world",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    const auto result = *world * face.transformFrame * detail::faceLocalMatrix(face);

    if (!detail::finiteMatrix(result)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Scene::faceWorldMatrix",
                            "Face world transform overflow.", "Inspect local face animation and entity transforms.",
                            id);

        return {};
    }

    return result;
}

std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size())
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::lineWorldMatrix",
                    "Rejected condition: id >= lines_.size()",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    if (lines_[id].owner)
        return worldMatrix(*lines_[id].owner);

    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());

    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}

bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::bindEntity",
            "Rejected condition: id >= components_.size() || !motions_.getLayer(layer)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    entityLayers_[id] = layer;

    return true;
}

bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::unbindEntity",
            "Rejected condition: id >= components_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    entityLayers_.erase(id);

    return true;
}

std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size())
        return {};

    const auto found = entityLayers_.find(id);

    if (found == entityLayers_.end() || !motions_.getLayer(found->second))
        return {};

    return found->second;
}

bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTransform",
            "Rejected condition: id >= components_.size() || !DiagramSystem::validTransform(transform)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    components_[id].transform = transform;

    return true;
}

bool Scene::setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld) {
    if (!getMesh(id, faceIndex) || (mode != FaceTransformMode::Hierarchy && mode != FaceTransformMode::Independent))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
                                   "Invalid entity, face index or transform mode.");
    auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 frame(1);
    if (preserveWorld) {
        if (mode == face.transformMode) return true;
        const auto world = worldMatrix(id);
        if (!world) return false;
        const auto oldParent = face.transformMode == FaceTransformMode::Hierarchy ? *world : glm::mat4(1);
        frame = oldParent * face.transformFrame;
        if (mode == FaceTransformMode::Hierarchy) {
            const float determinant = glm::determinant(*world);
            if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
                                           "Cannot preserve world pose under a singular parent transform.");
            frame = glm::inverse(*world) * frame;
        }
    }
    if (!TransformSystem::validAffine(frame)) return false;
    face.transformMode = mode;
    face.transformFrame = frame;
    return true;
}

bool Scene::setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() ||
        !DiagramSystem::validTransform(transform)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceTransform",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !DiagramSystem::validTransform(transform)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];

    face.transform = transform;
    face.independence = independence;

    return true;
}

std::optional<LineId> Scene::addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                     std::optional<EntityId> owner) {
    if (!finiteVector(start) || !finiteVector(end) || !validColor(color) ||
        (owner && getComponent(*owner) == nullptr)) {
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::addLine",
                "Rejected condition: !finiteVector(start) || !finiteVector(end) || !validColor(color) || (owner && getComponent(*owner) == nullptr)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);
    }

    const LineId id = lines_.size();

    lines_.push_back({start, end, color, owner});
    geometryRevision_ = nextGeometryRevision();

    return id;
}

std::optional<LineId> Scene::addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                       std::optional<EntityId> owner) {
    return addLine(origin, origin + vector, color, owner);
}

std::optional<std::array<LineId, 3>> Scene::addBasis(const glm::vec3& origin, const glm::mat3& basisVectors,
                                                     const glm::vec3& axisLengths, std::optional<EntityId> owner) {
    if (!finiteVector(origin) || !positiveLengths(axisLengths) || (owner && getComponent(*owner) == nullptr)) {
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::addBasis",
                "Rejected condition: !finiteVector(origin) || !positiveLengths(axisLengths) || (owner && getComponent(*owner) == nullptr)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);
    }

    for (int i = 0; i < 3; ++i) {
        if (!finiteVector(basisVectors[i]) || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f ||
            !finiteVector(origin + basisVectors[i] * axisLengths[i])) {
            return (
                Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::addBasis",
                    "Rejected condition: !finiteVector(basisVectors[i]) || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f || !finiteVector(origin + basisVectors[i] * axisLengths[i])",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                std::nullopt);
        }
    }
    // 사전 검증 후 세 선을 함께 등록한다. 실패 입력은 장면을 변경하지 않는다.
    const std::array<GlColor, 3> colors{GlColor{1.0f, 0.2f, 0.2f}, GlColor{0.2f, 1.0f, 0.2f},
                                        GlColor{0.2f, 0.4f, 1.0f}};
    std::array<LineId, 3> ids{};

    for (int i = 0; i < 3; ++i) {
        ids[i] = lines_.size();
        lines_.push_back({origin, origin + basisVectors[i] * axisLengths[i], colors[i], owner});
    }
    geometryRevision_ = nextGeometryRevision();

    return ids;
}

bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin) || !finiteVector(origin + vector)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVector",
            "Rejected condition: id >= lines_.size() || !finiteVector(origin) || !finiteVector(origin + vector)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();

    return true;
}

void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();

    if (components_.empty() && lines_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    lines_.clear();
    geometryRevision_ = nextGeometryRevision();
}

bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis) && finiteVector(transform.scale) &&
           finiteVector(transform.rotation[0]) && finiteVector(transform.rotation[1]) &&
           finiteVector(transform.rotation[2]) && std::abs(glm::determinant(transform.rotation) - 1.0f) < 0.01f &&
           glm::length(glm::transpose(transform.rotation) * transform.rotation[0] - glm::vec3(1, 0, 0)) < 0.01f &&
           glm::length(glm::transpose(transform.rotation) * transform.rotation[1] - glm::vec3(0, 1, 0)) < 0.01f &&
           glm::length(glm::transpose(transform.rotation) * transform.rotation[2] - glm::vec3(0, 0, 1)) < 0.01f &&
           finiteMatrix(detail::transformMatrix(transform));
}

bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    if (polyhedron.faces.empty())
        return false;

    for (const auto& face : polyhedron.faces)
        if (!detail::validMesh(face) || !validTransform(face.transform))
            return false;

    return true;
}

Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
    Polyhedron polyhedron;

    if (!positiveLengths(sideLengths)) {
        return polyhedron;
    }
    polyhedron.shape = shape;

    if (shape == GLShape::Cube) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;

        polyhedron.faces.reserve(6);
        appendFace(polyhedron, GLFace::Rect, {{-x, -y, z}, {x, -y, z}, {x, y, z}, {-x, y, z}}, {.9f, .3f, .3f});
        appendFace(polyhedron, GLFace::Rect, {{x, -y, -z}, {-x, -y, -z}, {-x, y, -z}, {x, y, -z}}, {.3f, .8f, .4f});
        appendFace(polyhedron, GLFace::Rect, {{-x, -y, -z}, {-x, -y, z}, {-x, y, z}, {-x, y, -z}}, {.3f, .5f, .9f});
        appendFace(polyhedron, GLFace::Rect, {{x, -y, z}, {x, -y, -z}, {x, y, -z}, {x, y, z}}, {.9f, .8f, .3f});
        appendFace(polyhedron, GLFace::Rect, {{-x, y, z}, {x, y, z}, {x, y, -z}, {-x, y, -z}}, {.3f, .8f, .9f});
        appendFace(polyhedron, GLFace::Rect, {{-x, -y, -z}, {x, -y, -z}, {x, -y, z}, {-x, -y, z}}, {.8f, .3f, .9f});
    } else if (shape == GLShape::Pyramid) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        const glm::vec3 frontLeft{-x, -y, z};
        const glm::vec3 frontRight{x, -y, z};
        const glm::vec3 backRight{x, -y, -z};
        const glm::vec3 backLeft{-x, -y, -z};
        const glm::vec3 apex{0.0f, y, 0.0f};

        polyhedron.faces.reserve(5);
        // 앞, 오른쪽, 뒤, 왼쪽, 바닥. 삼각형과 사각형은 바깥쪽 winding이다.
        appendFace(polyhedron, GLFace::Triangle, {frontLeft, frontRight, apex}, {.9f, .3f, .3f});
        appendFace(polyhedron, GLFace::Triangle, {frontRight, backRight, apex}, {.3f, .5f, .9f});
        appendFace(polyhedron, GLFace::Triangle, {backRight, backLeft, apex}, {.3f, .8f, .4f});
        appendFace(polyhedron, GLFace::Triangle, {backLeft, frontLeft, apex}, {.9f, .8f, .3f});
        appendFace(polyhedron, GLFace::Rect, {backLeft, backRight, frontRight, frontLeft}, {.8f, .3f, .9f});
    } else if (shape == GLShape::Tetra) {
        // 원점에서 직교하는 세 모서리를 만들고, 무게중심을 로컬 원점으로 옮긴다.
        const glm::vec3 center = sideLengths * 0.25f;
        const glm::vec3 origin = -center;
        const glm::vec3 x = glm::vec3(sideLengths.x, 0.0f, 0.0f) - center;
        const glm::vec3 y = glm::vec3(0.0f, sideLengths.y, 0.0f) - center;
        const glm::vec3 z = glm::vec3(0.0f, 0.0f, sideLengths.z) - center;

        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle, {origin, y, x}, {.9f, .4f, .3f});
        appendFace(polyhedron, GLFace::Triangle, {origin, x, z}, {.3f, .8f, .4f});
        appendFace(polyhedron, GLFace::Triangle, {origin, z, y}, {.3f, .5f, .9f});
        appendFace(polyhedron, GLFace::Triangle, {x, y, z}, {.9f, .8f, .3f});
    }

    return polyhedron;
}

Model DiagramSystem::makeSphere(float radius, unsigned slices, unsigned stacks) {
    Model model;

    if (!std::isfinite(radius) || radius <= 0 || slices < 3 || slices > 512 || stacks < 2 || stacks > 256)
        return model;

    Mesh mesh;

    mesh.localVertices.push_back({0, radius, 0});

    constexpr double pi = 3.14159265358979323846;

    for (unsigned row = 1; row < stacks; ++row) {
        const double latitude = pi * row / stacks;

        for (unsigned col = 0; col < slices; ++col) {
            const double longitude = 2 * pi * col / slices;

            mesh.localVertices.push_back(radius * glm::vec3(std::sin(latitude) * std::cos(longitude),
                                                            std::cos(latitude),
                                                            std::sin(latitude) * std::sin(longitude)));
        }
    }

    const auto bottom = std::uint32_t(mesh.localVertices.size());

    mesh.localVertices.push_back({0, -radius, 0});

    for (unsigned col = 0; col < slices; ++col) {
        const auto next = (col + 1) % slices;

        mesh.triangleIndices.insert(mesh.triangleIndices.end(), {0, 1 + next, 1 + col});

        for (unsigned row = 0; row + 2 < stacks; ++row) {
            const auto a = 1 + row * slices + col, b = 1 + row * slices + next, c = a + slices, d = b + slices;

            mesh.triangleIndices.insert(mesh.triangleIndices.end(), {a, b, c, b, d, c});
        }

        const auto a = 1 + (stacks - 2) * slices + col, b = 1 + (stacks - 2) * slices + next;

        mesh.triangleIndices.insert(mesh.triangleIndices.end(), {a, b, bottom});
    }

    for (const auto& p : mesh.localVertices)
        mesh.vertexNormals.push_back(glm::normalize(p));
    model.faces.push_back(std::move(mesh));

    return validPolyhedron(model) ? model : Model{};
}

Model DiagramSystem::makeCone(float radius, float height, unsigned slices) {
    Model model;

    if (!std::isfinite(radius) || !std::isfinite(height) || radius <= 0 || height <= 0 || slices < 3 || slices > 512)
        return model;

    constexpr double pi = 3.14159265358979323846;
    // Separate sides/base keep the bottom rim sharp for normals and outlines.
    Mesh sides, base;

    sides.localVertices.push_back({0, height * .5f, 0});
    base.localVertices.push_back({0, -height * .5f, 0});

    for (unsigned i = 0; i < slices; ++i) {
        const double a = 2 * pi * i / slices;
        const glm::vec3 p{radius * float(std::cos(a)), -height * .5f, radius * float(std::sin(a))};

        sides.localVertices.push_back(p);
        base.localVertices.push_back(p);
    }

    for (unsigned i = 0; i < slices; ++i) {
        const auto a = 1 + i, b = 1 + (i + 1) % slices;

        sides.triangleIndices.insert(sides.triangleIndices.end(), {0, b, a});
        base.triangleIndices.insert(base.triangleIndices.end(), {0, a, b});
    }
    recalculateNormals(sides);
    recalculateNormals(base);
    model.faces = {std::move(sides), std::move(base)};

    return validPolyhedron(model) ? model : Model{};
}

glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    return detail::transformMatrix(transform);
}

glm::mat4 DiagramSystem::faceModelMatrix(const Component_Poly& component, const PlaneFigure& face) {
    return detail::faceMatrix(component, face);
}

bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);

    return finiteVector(camera.position) && finiteVector(camera.target) && finiteVector(camera.up) &&
           finiteVector(direction) && finiteVector(right) && std::isfinite(glm::dot(direction, direction)) &&
           std::isfinite(glm::dot(right, right)) && glm::dot(direction, direction) > 0.000001f &&
           glm::dot(right, right) > 0.000001f && std::isfinite(camera.fieldOfViewDegrees) &&
           camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f && std::isfinite(camera.nearPlane) &&
           std::isfinite(camera.farPlane) && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane &&
           std::isfinite(camera.orthographicHalfExtent) && camera.orthographicHalfExtent > 0.0f;
}

std::optional<Camera> CameraSystem::attached(const Camera& local, const glm::mat4& frame) {
    if (!validCamera(local) || !TransformSystem::validAffine(frame))
        return std::nullopt;
    const auto forward = glm::vec3(frame * glm::vec4(local.target - local.position, 0));
    const auto up = glm::vec3(frame * glm::vec4(local.up, 0));
    if (glm::length(forward) < 1e-6f || glm::length(glm::cross(forward, up)) < 1e-6f)
        return std::nullopt;
    auto result = local;
    const auto direction = glm::normalize(forward);
    const auto right = glm::normalize(glm::cross(direction, up));
    result.position = glm::vec3(frame * glm::vec4(local.position, 1));
    result.target = result.position + direction;
    result.up = glm::normalize(glm::cross(right, direction));
    return validCamera(result) ? std::optional<Camera>{result} : std::nullopt;
}

glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);

        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, camera.nearPlane, camera.farPlane);
    }

    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect, camera.nearPlane, camera.farPlane);
}

std::optional<EntityId> Scene::addModel(const Model& model, const Transform3D& transform, PivotMode pivot,
                                        const glm::vec3& customPivot) {
    if (!DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot))
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::addModel",
                    "Rejected condition: !DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot)",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                std::nullopt);

    auto initial = transform;

    if (pivot == PivotMode::BottomCenter)
        initial.localBasis = detail::bottomCenter(model);
    else if (pivot == PivotMode::Custom)
        initial.localBasis = customPivot;

    auto id = push_entity(model, initial);

    if (id)
        components_[*id].pivotMode = pivot;

    return id;
}

bool Scene::setModel(EntityId id, const Model& model) {
    if (id >= components_.size() || !DiagramSystem::validPolyhedron(model))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setModel",
            "Rejected condition: id>=components_.size() || !DiagramSystem::validPolyhedron(model)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto box = detail::bounds(model);

    if (!detail::finiteVector(box.second - box.first))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setModel",
            "Rejected condition: !detail::finiteVector(box.second-box.first)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = components_[id];

    replacement.polyhedron = model;
    replacement.sideLengths = box.second - box.first;
    replacement.transformOnly = false;

    if (replacement.pivotMode == PivotMode::BottomCenter)
        replacement.transform.localBasis = detail::bottomCenter(model);
    components_[id] = std::move(replacement);
    geometryRevision_ = nextGeometryRevision();

    return true;
}

const Mesh* Scene::getMesh(EntityId id, std::size_t meshIndex) const {
    const auto* c = getComponent(id);

    return c && meshIndex < c->polyhedron.faces.size() ? &c->polyhedron.faces[meshIndex] : nullptr;
}

bool Scene::setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh) {
    if (!getMesh(id, meshIndex))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setMesh", "Rejected condition: !getMesh(id,meshIndex)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    Model replacement = components_[id].polyhedron;

    replacement.faces[meshIndex] = mesh;

    return setModel(id, replacement);
}

bool Scene::setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                            const std::vector<std::uint32_t>& triangles) {
    const auto* original = getMesh(id, meshIndex);

    if (!original)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setMeshVertices", "Rejected condition: !original",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *original;
    const auto geometry = Mesh::fromVertices(vertices, triangles);

    replacement.localVertices = geometry.localVertices;
    replacement.vertexNormals = geometry.vertexNormals;
    replacement.texcoords = geometry.texcoords;
    replacement.vertexColors = geometry.vertexColors;
    replacement.triangleIndices = triangles;
    replacement.shape = GLFace::Mesh;
    replacement.outlineIndices.clear();

    if (replacement.outline == OutlineMode::Explicit)
        replacement.outline = OutlineMode::None;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh || vertexIndex >= mesh->localVertices.size() || !detail::finiteVector(position))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVertexPosition",
            "Rejected condition: !mesh||vertexIndex>=mesh->localVertices.size()||!detail::finiteVector(position)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.localVertices[vertexIndex] = position;
    recalculateNormals(replacement);

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTriangleIndices", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.shape = GLFace::Mesh;
    replacement.triangleIndices = indices;

    if (!detail::validMesh(replacement))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTriangleIndices",
            "Rejected condition: !detail::validMesh(replacement)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    recalculateNormals(replacement);

    return setMesh(id, meshIndex, replacement);
}

bool Scene::generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::generateTexcoords", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    return graphics::generateTexcoords(replacement, options) && setMesh(id, meshIndex, replacement);
}

bool Scene::transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                               float rotationDegrees) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::transformTexcoords", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    return graphics::transformTexcoords(replacement, scale, offset, rotationDegrees) &&
           setMesh(id, meshIndex, replacement);
}

bool Scene::setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTexcoords", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.texcoords = uvs;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVertexNormals", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.vertexNormals = normals;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setMaterial(EntityId id, std::size_t meshIndex, const Material& material) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setMaterial", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.material = material;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                       const std::vector<std::uint32_t>& explicitEdges) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setOutline", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.outline = mode;
    replacement.outlineIndices = explicitEdges;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot) {
    if (id >= components_.size() || !detail::finiteVector(customPivot))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setLocalBasis",
            "Rejected condition: id>=components_.size()||!detail::finiteVector(customPivot)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto& c = components_[id];

    if (c.transformOnly && mode == PivotMode::BottomCenter)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setLocalBasis",
            "Rejected condition: c.transformOnly && mode==PivotMode::BottomCenter",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    if (mode == PivotMode::BottomCenter)
        c.transform.localBasis = detail::bottomCenter(c.polyhedron);
    else if (mode == PivotMode::Custom)
        c.transform.localBasis = customPivot;
    c.pivotMode = mode;

    return true;
}

bool Scene::setVisible(EntityId id, bool visible) {
    if (id >= components_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVisible", "Rejected condition: id>=components_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    components_[id].visible = visible;

    return true;
}

bool Scene::splitFace(EntityId id, std::size_t faceIndex) {
    if (!getMesh(id, faceIndex))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::splitFace", "Rejected condition: !getMesh(id,faceIndex)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    // The same face remains under its parent; vertices and indices are already face-local storage.
    auto& face = components_[id].polyhedron.faces[faceIndex];

    if (!face.independence) {
        face.transform = Transform3D{};
        face.independence = true;
    }

    return true;
}

bool Scene::setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform) {
    return setFaceTransform(id, faceIndex, transform, true);
}
} // namespace graphics

// --- Renderer.cpp ---

namespace graphics {
std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);

    if (!input)
        return {};

    std::ostringstream buffer;

    buffer << input.rdbuf();

    auto text = buffer.str();

    if (text.compare(0, 3, "\xEF\xBB\xBF") == 0)
        text.erase(0, 3);

    return text;
}

namespace detail {
inline std::string pathForDiagnostics(const std::filesystem::path& path) {
#if defined(_MSVC_EXECUTION_CHARACTER_SET) && _MSVC_EXECUTION_CHARACTER_SET == 65001
    const auto utf8 = path.u8string();
    return std::string(utf8.begin(), utf8.end());
#else
    return path.string();
#endif
}

GLuint compileShader(GLenum kind, const std::string& sourceText, const std::filesystem::path& sourcePath) {
    Diagnostics::trace(PipelineStage::Shader, "compileShader");
    const auto shader = glCreateShader(kind);

    if (!shader) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glCreateShader", "Shader allocation failed.");

        return 0;
    }

    const char* text = sourceText.c_str();

    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);

    GLint success = 0;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint length = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::string log(std::max(length, 1), '\0');

        glGetShaderInfoLog(shader, GLsizei(log.size()), nullptr, log.data());
        Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Shader, kind == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader",
            "GLSL file=" + pathForDiagnostics(sourcePath) + "\n" + log,
            "Read the compiler line number; check GLSL version, declarations and shader inputs/outputs.");
        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

bool validLayout(const VertexLayout& layout) {
    if (!layout.stride || layout.stride > std::size_t(std::numeric_limits<GLsizei>::max()) || layout.attributes.empty())
        return false;
    GLint maximum = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maximum);
    std::vector<GLuint> used;
    for (const auto& a : layout.attributes) {
        std::size_t bytes = 0;
        switch (a.type) {
        case GL_BYTE: case GL_UNSIGNED_BYTE: bytes = 1; break;
        case GL_SHORT: case GL_UNSIGNED_SHORT: bytes = 2; break;
        case GL_INT: case GL_UNSIGNED_INT: case GL_FLOAT: bytes = 4; break;
        default: return false;
        }
        if (a.location >= GLuint(maximum) || a.components < 1 || a.components > 4 ||
            a.offset > layout.stride || std::size_t(a.components)*bytes > layout.stride-a.offset ||
            (a.integer && (a.normalized || a.type == GL_FLOAT)) ||
            std::find(used.begin(), used.end(), a.location) != used.end())
            return false;
        used.push_back(a.location);
    }
    return true;
}

GLuint uploadTexture(const TextureImage& image, bool repeat) {
    GLuint id = 0;

    glGenTextures(1, &id);

    if (!id) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Asset, "glGenTextures", "Texture allocation failed.");

        return 0;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    GLint previousAlignment = 4;

    glGetIntegerv(GL_UNPACK_ALIGNMENT, &previousAlignment);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, GLsizei(image.width), GLsizei(image.height), 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, image.rgba.data());
    glPixelStorei(GL_UNPACK_ALIGNMENT, previousAlignment);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, repeat ? GL_REPEAT : GL_CLAMP_TO_EDGE);

    if (!checkGlAt(__FILE__, __LINE__, PipelineStage::Asset, "uploadTexture")) {
        glDeleteTextures(1, &id);

        return 0;
    }

    return id;
}
} // namespace detail

bool Renderer::initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
    const auto vertexText = vertexPath.empty() ? std::string(detail::vertexShader) : readTextFile(vertexPath);
    const auto fragmentText = fragmentPath.empty() ? std::string(detail::fragmentShader) : readTextFile(fragmentPath);

    if (vertexText.empty() || fragmentText.empty())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Shader, "Renderer::initShader",
            "Shader source is empty or unreadable: vertex=" + detail::pathForDiagnostics(vertexPath) +
                "; fragment=" + detail::pathForDiagnostics(fragmentPath),
            "Use embedded shaders (empty shaderDirectory), or copy both GLSL files to the configured project-relative directory.");
#if defined(_MSVC_EXECUTION_CHARACTER_SET) && _MSVC_EXECUTION_CHARACTER_SET == 65001
    // /utf-8 /FC encodes __FILE__ as UTF-8, not the Windows ANSI code page.
    const auto embeddedSource = std::filesystem::u8path(__FILE__).parent_path() / "Shaders.h";
#else
    const auto embeddedSource = std::filesystem::path(__FILE__).parent_path() / "Shaders.h";
#endif
    const auto vertex =
        detail::compileShader(GL_VERTEX_SHADER, vertexText, vertexPath.empty() ? embeddedSource : vertexPath);

    if (!vertex)
        return false;

    const auto fragment =
        detail::compileShader(GL_FRAGMENT_SHADER, fragmentText, fragmentPath.empty() ? embeddedSource : fragmentPath);

    if (!fragment) {
        glDeleteShader(vertex);

        return false;
    }
    shaderProgramID_ = glCreateProgram();

    if (!shaderProgramID_) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glCreateProgram",
                                   "Program allocation failed.");
    }
    glAttachShader(shaderProgramID_, vertex);
    glAttachShader(shaderProgramID_, fragment);
    glLinkProgram(shaderProgramID_);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint success = 0;

    glGetProgramiv(shaderProgramID_, GL_LINK_STATUS, &success);

    if (!success) {
        GLint size = 0;

        glGetProgramiv(shaderProgramID_, GL_INFO_LOG_LENGTH, &size);

        std::string log(std::max(size, 1), '\0');

        glGetProgramInfoLog(shaderProgramID_, GLsizei(log.size()), nullptr, log.data());

        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glLinkProgram", log,
                                   "Match vertex outputs and fragment inputs; inspect the linker log.");
    }
    mvpLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.mvp.c_str());
    modelLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.model.c_str());
    normalLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.normal.c_str());
    textureLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.texture.c_str());
    hasTextureLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.hasTexture.c_str());
    lightingLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.lighting.c_str());
    lightDirectionLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.lightDirection.c_str());
    lightColorLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.lightColor.c_str());
    overrideLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.overrideColor.c_str());
    outlineColorLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.outlineColor.c_str());
    tintLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.tint.c_str());

    return mvpLocation_ >= 0 || (config_.uniforms.mvp.empty() && bool(config_.bindUniforms)) || Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glGetUniformLocation",
                                                    "Required MVP uniform is missing or optimized away: " + config_.uniforms.mvp);
}

bool Renderer::initialize(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath,
                          const RenderConfig& config) {
    if (!detail::validLayout(config.vertexLayout) ||
        (!config.packVertices && config.vertexLayout.stride != sizeof(RenderVertex)))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::initialize",
                                   "Invalid layout, or custom stride without a vertex packer.");
    const auto nextConfig = config; // Copy before destroying resources; config may alias current settings.
    destroy();
    config_ = nextConfig;

    if (!initShader(vertexPath, fragmentPath)) {
        destroy();

        return false;
    }
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    if (!vao_ || !vbo_ || !ebo_) {
        destroy();

        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::initialize",
                                   "VAO/VBO/EBO allocation failed.");
    }
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    for (const auto& attribute : config_.vertexLayout.attributes) {
        const auto pointer = reinterpret_cast<const void*>(attribute.offset);
        if (attribute.integer)
            glVertexAttribIPointer(attribute.location, attribute.components, attribute.type,
                                    GLsizei(config_.vertexLayout.stride), pointer);
        else
            glVertexAttribPointer(attribute.location, attribute.components, attribute.type,
                                   attribute.normalized ? GL_TRUE : GL_FALSE,
                                   GLsizei(config_.vertexLayout.stride), pointer);
        glEnableVertexAttribArray(attribute.location);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return detail::checkGlAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::initialize vertex layout");
}

bool Renderer::upload(const Scene& scene, bool textureMode) {
    Diagnostics::trace(PipelineStage::VertexInput, "Renderer::upload");

    std::vector<RenderVertex> vertices;
    std::vector<GLuint> indices;
    std::vector<DrawRange> ranges;
    TextureMap stagedTextures;
    auto cleanup = [&]() {
        for (const auto& item : stagedTextures)
            glDeleteTextures(1, &item.second);
    };

    try {
        for (auto id : scene.getIds()) {
            const auto* c = scene.getComponent(id);

            if (c && c->transformOnly)
                continue;

            if (!c || !DiagramSystem::validPolyhedron(c->polyhedron)) {
                cleanup();

                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload",
                                           "Missing component or invalid mesh, indices, colors, UVs or normals.",
                                           "Inspect the entity's mesh data before GPU upload.", id);
            }

            for (std::size_t faceIndex = 0; faceIndex < c->polyhedron.faces.size(); ++faceIndex) {
                const auto& face = c->polyhedron.faces[faceIndex];

                if (vertices.size() + face.localVertices.size() > std::numeric_limits<GLuint>::max()) {
                    cleanup();

                    return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload",
                                               "Vertex count exceeds the GPU index range.");
                }

                const auto first = GLuint(vertices.size());
                Mesh normals = face;

                if (normals.vertexNormals.empty())
                    recalculateNormals(normals);

                for (std::size_t i = 0; i < face.localVertices.size(); ++i) {
                    const auto p = face.localVertices[i];
                    const auto n = normals.vertexNormals[i];
                    auto color = face.vertexColors.empty() ? face.color : face.vertexColors[i];

                    color.red *= face.material.color.red;
                    color.green *= face.material.color.green;
                    color.blue *= face.material.color.blue;

                    const auto uv = face.texcoords.empty() ? glm::vec2(0) : face.texcoords[i];

                    vertices.push_back(
                        {{p.x, p.y, p.z}, {color.red, color.green, color.blue}, {uv.x, uv.y}, {n.x, n.y, n.z}});
                }

                const auto triangles = detail::triangles(face);
                const auto outline =
                    meshOutline(face, face.outline == OutlineMode::None ? OutlineMode::FeatureEdges : face.outline);

                if (triangles.size() > std::numeric_limits<GLsizei>::max() ||
                    outline.size() > std::numeric_limits<GLsizei>::max()) {
                    cleanup();

                    return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload",
                                               "Draw count exceeds GLsizei range.");
                }

                DrawRange range;

                range.entityId = id;
                range.faceIndex = faceIndex;
                range.firstIndex = indices.size();
                range.count = GLsizei(triangles.size());

                for (auto index : triangles)
                    indices.push_back(first + index);
                range.outlineFirstIndex = indices.size();
                range.outlineCount = GLsizei(outline.size());

                for (auto index : outline)
                    indices.push_back(first + index);
                ranges.push_back(range);

                if (textureMode && face.material.useTexture &&
                    stagedTextures.find(face.material.texture) == stagedTextures.end()) {
                    if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Asset, "before uploadTexture")) {
                        cleanup();

                        return false;
                    }

                    const auto texture = detail::uploadTexture(*face.material.texture, face.material.repeatTexture);

                    if (!texture) {
                        cleanup();

                        return false;
                    }

                    try {
                        stagedTextures.emplace(face.material.texture, texture);
                    } catch (...) {
                        glDeleteTextures(1, &texture);
                        throw;
                    }
                }
            }
        }

        for (std::size_t lineIndex = 0; lineIndex < scene.getLines().size(); ++lineIndex) {
            const auto& line = scene.getLines()[lineIndex];

            if (vertices.size() > std::numeric_limits<GLuint>::max() - 2) {
                cleanup();

                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload lines",
                                           "Vertex count exceeds the GPU index range.");
            }

            const auto first = GLuint(vertices.size());

            for (auto p : {line.start, line.end})
                vertices.push_back(
                    {{p.x, p.y, p.z}, {line.color.red, line.color.green, line.color.blue}, {0, 0}, {0, 0, 0}});

            DrawRange range;

            range.lineId = lineIndex;
            range.firstIndex = indices.size();
            range.count = 2;
            ranges.push_back(range);
            indices.insert(indices.end(), {first, first + 1});
        }

        if (vertices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / config_.vertexLayout.stride ||
            indices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / sizeof(GLuint)) {
            cleanup();

            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload",
                                       "Buffer byte size exceeds GLsizeiptr range.");
        }

        const auto byteCount = vertices.size() * config_.vertexLayout.stride;
        std::vector<std::uint8_t> packed;
        if (config_.packVertices) {
            if (!config_.packVertices(vertices, packed) || packed.size() != byteCount) {
                cleanup();
                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "packVertices",
                                           "Packer failed or byte count does not match vertex count times stride.");
            }
        } else {
            packed.resize(byteCount);
            if (byteCount)
                std::memcpy(packed.data(), vertices.data(), byteCount);
        }

        if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::VertexInput, "before buffer upload")) {
            cleanup();

            return false;
        }
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(packed.size()),
                      packed.empty() ? nullptr : packed.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indices.size() * sizeof(GLuint)),
                      indices.empty() ? nullptr : indices.data(), GL_STATIC_DRAW);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::VertexInput, "glBufferData")) {
            cleanup();

            return false;
        }

        for (const auto& item : textures_)
            glDeleteTextures(1, &item.second);
        textures_ = std::move(stagedTextures);
        drawRanges_ = std::move(ranges);

        return true;
    } catch (...) {
        cleanup();
        throw;
    }
}

bool Renderer::draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
                    const RenderOptions& options) const {
    Diagnostics::trace(PipelineStage::Transform, "Renderer::draw");

    if (!detail::finiteMatrix(view) || !detail::finiteMatrix(projection))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Renderer::draw",
                                   "View or projection matrix contains NaN/Inf.");

    for (const auto& range : drawRanges_) {
        glm::mat4 matrix(1);

        if (range.lineId) {
            const auto world = scene.lineWorldMatrix(*range.lineId);

            if (!world)
                return false;
            matrix = *world;
        } else {
            const auto* c = scene.getComponent(range.entityId);
            const auto world = scene.faceWorldMatrix(range.entityId, range.faceIndex);

            if (!world)
                return false;
            matrix = *world;
        }

        if (!detail::finiteMatrix(matrix) || !detail::finiteMatrix(projection * view * matrix))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Renderer::draw",
                                       "World or MVP matrix contains NaN/Inf.",
                                       "Check parent transforms, scales and motion samples.", range.entityId);
    }
    glUseProgram(shaderProgramID_);
    glBindVertexArray(vao_);

    auto cull = [&](const Mesh* face, const glm::mat4& model) {
        const auto mode = face && face->cull != CullMode::Inherit ? face->cull : options.cull;

        glFrontFace(glm::determinant(glm::mat3(model)) < 0 ? GL_CW : GL_CCW);

        if (!face || mode == CullMode::None || mode == CullMode::Inherit)
            glDisable(GL_CULL_FACE);
        else {
            glEnable(GL_CULL_FACE);
            glCullFace(mode == CullMode::Front ? GL_FRONT : GL_BACK);
        }
    };
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(textureLocation_, 0);
    glUniform3f(lightDirectionLocation_, options.lightDirection.x, options.lightDirection.y, options.lightDirection.z);
    glUniform3f(lightColorLocation_, options.lightColor.red, options.lightColor.green, options.lightColor.blue);
    const auto eye = glm::vec3(glm::inverse(view)[3]);
    glUniform3f(glGetUniformLocation(shaderProgramID_, config_.uniforms.viewPosition.c_str()), eye.x, eye.y, eye.z);
    const bool lightOn = options.lighting;
    glUniform3f(glGetUniformLocation(shaderProgramID_, config_.uniforms.ambientColor.c_str()),
                 lightOn ? options.ambientColor.red : 0.0f,
                 lightOn ? options.ambientColor.green : 0.0f,
                 lightOn ? options.ambientColor.blue : 0.0f);
    glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.directionalIntensity.c_str()),
                 lightOn ? options.directionalIntensity : 0.0f);
    glUniform1i(glGetUniformLocation(shaderProgramID_, config_.uniforms.pointLightCount.c_str()),
                 lightOn ? int(options.pointLights.size()) : 0);
    for (std::size_t i=0; lightOn && i<options.pointLights.size(); ++i) {
        const auto& light=options.pointLights[i];
        const auto prefix=config_.uniforms.pointLights + "[" + std::to_string(i) + "].";
        glUniform3f(glGetUniformLocation(shaderProgramID_, (prefix+"position").c_str()), light.position.x,light.position.y,light.position.z);
        glUniform3f(glGetUniformLocation(shaderProgramID_, (prefix+"color").c_str()), light.color.red,light.color.green,light.color.blue);
        glUniform1f(glGetUniformLocation(shaderProgramID_, (prefix+"intensity").c_str()),light.intensity);
        glUniform1f(glGetUniformLocation(shaderProgramID_, (prefix+"attenuation").c_str()),light.attenuation);
    }
    glUniform3f(outlineColorLocation_, options.outlineColor.red, options.outlineColor.green,
                 options.outlineColor.blue);

    if (options.wireframe && options.depthTest) {
        // All surfaces write depth first, so a later object's front face hides earlier back edges.
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        for (const auto& range : drawRanges_) {
            if (range.lineId)
                continue;

            const auto* c = scene.getComponent(range.entityId);
            const auto& face = c->polyhedron.faces[range.faceIndex];

            if (!c->visible || !face.visible)
                continue;

            const auto model = *scene.faceWorldMatrix(range.entityId, range.faceIndex);

            cull(&face, model);

            const auto matrix = projection * view * model;

            glUniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(matrix));
            if (config_.bindUniforms && !config_.bindUniforms(
                    {shaderProgramID_, scene, range.entityId, range.lineId, range.faceIndex, model, view, projection, true}))
                return false;
            glDrawElements(GL_TRIANGLES, range.count, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(range.firstIndex * sizeof(GLuint)));
        }
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    for (const auto& range : drawRanges_) {
        glm::mat4 model(1);
        const Mesh* face = nullptr;
        GlColor tint;

        if (range.lineId) {
            const auto& line = scene.getLines()[*range.lineId];

            if (line.owner) {
                const auto* owner = scene.getComponent(*line.owner);

                if (!owner || !owner->visible)
                    continue;
            }

            const auto world = scene.lineWorldMatrix(*range.lineId);

            if (!world)
                return false;
            model = *world;
        } else {
            const auto* c = scene.getComponent(range.entityId);

            if (!c || !c->visible)
                continue;
            face = &c->polyhedron.faces[range.faceIndex];

            if (!face->visible)
                continue;
            tint = c->tint;

            const auto world = scene.faceWorldMatrix(range.entityId, range.faceIndex);

            if (!world)
                return false;
            model = *world;
        }

        const auto mvp = projection * view * model;

        cull(face, model);

        const float determinant = glm::determinant(glm::mat3(model));
        const auto normalMatrix = std::isfinite(determinant) && std::abs(determinant) > 1e-12f
                                      ? glm::transpose(glm::inverse(glm::mat3(model)))
                                      : glm::mat3(1);

        glUniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(modelLocation_, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(normalLocation_, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3f(tintLocation_, tint.red, tint.green, tint.blue);
        glUniform1i(lightingLocation_, face && !face->material.unlit);
        const Material material = face ? face->material : Material{};
        glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.materialAmbient.c_str()), material.ambientStrength);
        glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.materialDiffuse.c_str()), material.diffuseStrength);
        glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.materialSpecular.c_str()), material.specularStrength);
        glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.shininess.c_str()), material.shininess);
        glUniform1i(overrideLocation_, 0);

        const bool textured = face && options.textures && face->material.useTexture;

        glUniform1i(hasTextureLocation_, textured);

        if (textured) {
            glBindTexture(GL_TEXTURE_2D, textures_.at(face->material.texture));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                             face->material.repeatTexture ? GL_REPEAT : GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                             face->material.repeatTexture ? GL_REPEAT : GL_CLAMP_TO_EDGE);
        } else
            glBindTexture(GL_TEXTURE_2D, 0);

        if (config_.bindUniforms && !config_.bindUniforms(
                {shaderProgramID_, scene, range.lineId ? std::nullopt : std::optional<EntityId>(range.entityId),
                 range.lineId, range.faceIndex, model, view, projection, false}))
            return false;

        if (!face || !options.wireframe) {
            glDrawElements(face ? GL_TRIANGLES : GL_LINES, range.count, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(range.firstIndex * sizeof(GLuint)));

            if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Raster, "glDrawElements")) {
                glBindVertexArray(0);
                glUseProgram(0);

                return false;
            }
        }

        if (face && (options.wireframe || face->outline != OutlineMode::None)) {
            glUniform1i(hasTextureLocation_, 0);
            glUniform1i(lightingLocation_, 0);
            glUniform1i(overrideLocation_, options.wireframe ? 0 : 1);
            glDepthFunc(GL_LEQUAL);
            glDepthMask(GL_FALSE);
            glDrawElements(GL_LINES, range.outlineCount, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(range.outlineFirstIndex * sizeof(GLuint)));
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
        }
    }
    glUniform1i(overrideLocation_, 0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    return detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Raster, "Renderer::draw outlines and cleanup");
}

bool Renderer::renderPass(const Scene& scene, const RenderPass& pass, int width, int height) {
    Diagnostics::trace(PipelineStage::Framebuffer, "Renderer::renderPass");

    const auto& camera = pass.camera;
    const auto& options = pass.options;
    const auto& viewport = pass.viewport;

    if (!shaderProgramID_ || !vao_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::renderPass",
                                   "Renderer is not initialized.");

    if (width <= 0 || height <= 0)
        return true;

    if (viewport.x < 0 || viewport.y < 0 || viewport.x > width || viewport.y > height || viewport.width < 0 ||
        viewport.height < 0 || viewport.width > width - viewport.x || viewport.height > height - viewport.y)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Renderer::renderPass",
                                   "Viewport lies outside the framebuffer.");

    if (viewport.width == 0 || viewport.height == 0)
        return true;

    bool validLights=options.pointLights.size()<=8 && std::isfinite(options.directionalIntensity) && options.directionalIntensity>=0;

    for (const auto& light:options.pointLights)
        validLights  =    validLights && detail::finiteVector(light.position) && detail::validColor(light.color) &&
            std::isfinite(light.intensity) && light.intensity>=0 && std::isfinite(light.attenuation) && light.attenuation>=0;

    if (!validLights || !CameraSystem::validCamera(camera) || !detail::finiteVector(options.lightDirection) ||
        glm::length(options.lightDirection) < 1e-12f || !detail::validColor(options.lightColor) ||
        !detail::validColor(options.outlineColor) || !detail::validColor(options.ambientColor) || !std::isfinite(options.background.r) ||
        !std::isfinite(options.background.g) || !std::isfinite(options.background.b) ||
        !std::isfinite(options.background.a) ||
        (options.cull != CullMode::Inherit && options.cull != CullMode::None && options.cull != CullMode::Back &&
         options.cull != CullMode::Front))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Transform, "Renderer::renderPass",
            "Invalid camera, light, color or culling mode.",
            "Check near/far planes, FOV, target/up directions, positive orthographic extent and finite render options.");

    if (uploadedScene_ != &scene || uploadedRevision_ != scene.getGeometryRevision() ||
        uploadedTextures_ != options.textures) {
        if (!upload(scene, options.textures))
            return false;
        uploadedScene_ = &scene;
        uploadedRevision_ = scene.getGeometryRevision();
        uploadedTextures_ = options.textures;
    }
    if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "before Renderer::render"))
        return false;
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glLineWidth(1);

    if (options.depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    glScissor(viewport.x, viewport.y, viewport.width, viewport.height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(options.background.r, options.background.g, options.background.b, options.background.a);

    const GLbitfield mask = (pass.clearColor ? GL_COLOR_BUFFER_BIT : 0) | (pass.clearDepth ? GL_DEPTH_BUFFER_BIT : 0);

    if (mask)
        glClear(mask);

    if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "viewport/scissor/clear")) {
        glDisable(GL_SCISSOR_TEST);

        return false;
    }

    const bool success = draw(scene, CameraSystem::viewMatrix(camera),
                              CameraSystem::projectionMatrix(camera, float(viewport.width) / viewport.height), options);

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    return success;
}

bool Renderer::render(const Scene& scene, const Camera& camera, int width, int height, const RenderOptions& options) {
    RenderPass pass;

    pass.camera = camera;
    pass.options = options;
    pass.viewport = {0, 0, width, height};

    return renderPass(scene, pass, width, height);
}

bool Renderer::renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height) {
    if (width <= 0 || height <= 0)
        return true;

    if (passes.empty())
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Renderer::renderViews",
                                   "Need at least one render pass.");

    if (!shaderProgramID_ || !vao_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::renderViews",
                                   "Renderer is not initialized.");
    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, width, height);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(.08f, .08f, .1f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Renderer::renderViews full-frame clear"))
        return false;

    for (const auto& pass : passes)
        if (!renderPass(scene, pass, width, height))
            return false;

    return true;
}

void Renderer::destroy() {
    if (ebo_)
        glDeleteBuffers(1, &ebo_);

    if (vbo_)
        glDeleteBuffers(1, &vbo_);

    if (vao_)
        glDeleteVertexArrays(1, &vao_);

    if (shaderProgramID_)
        glDeleteProgram(shaderProgramID_);

    for (const auto& item : textures_)
        glDeleteTextures(1, &item.second);
    textures_.clear();
    drawRanges_.clear();
    ebo_ = vbo_ = vao_ = shaderProgramID_ = 0;
    uploadedScene_ = nullptr;
    uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    uploadedTextures_ = false;
}
} // namespace graphics

// --- Application.cpp ---

namespace graphics::detail {
bool& applicationActive() {
    static bool active = false;

    return active;
}

unsigned& contextErrorSerial() {
    static unsigned value = 0;

    return value;
}
}

namespace graphics {

Application::~Application() {
    shutdown();
}

bool Application::configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath,
                                    const std::filesystem::path& fragmentPath) {
    if (!ensureContext("configureRenderer", __FILE__, __LINE__))
        return false;
    if (renderer_.initialize(vertexPath, fragmentPath, config))
        return true;
    requestClose();
    return false;
}

void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

void Application::setModifiedKeyHandler(std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::ensureContext(const char* operation, const char* file, int line) {
    Diagnostics::locate(file, line, operation);

    if (contextFailed_)
        return false;

    if (!window_ || GetCurrentThreadId() != contextThread_) {
        contextFailed_ = true;

        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::ensureContext",
            "창이 없거나 컨텍스트를 생성한 스레드와 다른 스레드에서 사용했습니다.",
            "창 생성·렌더링·종료를 같은 스레드에서 수행하세요. Application::run과 사용자 렌더 콜백을 확인하세요.");
    }
    using CurrentProc = void*(WINAPI*)();

    const auto module = GetModuleHandleW(L"opengl32.dll");
    const auto current =
        module ? reinterpret_cast<CurrentProc>(::GetProcAddress(module, "wglGetCurrentContext")) : nullptr;

    if (nativeContext_ && glfwGetCurrentContext() == window_ && current && current() == nativeContext_)
        return true;

    const auto serial = detail::contextErrorSerial();

    glfwMakeContextCurrent(window_);

    if (serial != detail::contextErrorSerial() || glfwGetCurrentContext() != window_ || !current ||
        !current()) {
        contextFailed_ = true;

        return Diagnostics::fail(
            PipelineStage::Runtime, "glfwMakeContextCurrent",
            "WGL이 요청한 컨텍스트를 연결하지 못하여 GL 명령 전에 렌더링을 중단했습니다.",
            "소스 위치는 연결 실패 호출입니다. 창 수명·렌더 스레드·컨텍스트를 해제/파괴하는 외부 콜백을 먼저 확인하세요. 이상이 없으면 GPU 드라이버·세션을 확인하세요. 이 로그만으로 드라이버 원인을 확정할 수 없습니다.");
    }
    nativeContext_ = current();

    return true;
}

bool Application::initialize(const ApplicationConfig& config) {
    contextThread_ = GetCurrentThreadId();
    contextFailed_ = false;
    nativeContext_ = nullptr;
    Diagnostics::trace(PipelineStage::Runtime, "Application::initialize");

    const auto root = detail::relativePathInside(detail::executableDirectory(), config.projectRoot);
    glfwSetErrorCallback([](int code, const char* message) {
        ++detail::contextErrorSerial();
        Diagnostics::fail(
            PipelineStage::Runtime, "GLFW/WGL", std::to_string(code) + ": " + (message ? message : "알 수 없는 오류"),
            "표시된 소스 위치·호출 경로에서 컨텍스트 연결이 실패했습니다. Application의 창 수명과 스레드를 먼저 확인하세요. OS/드라이버 원문도 함께 보세요. 렌더 패스마다 별도 WGL 컨텍스트를 만들 필요는 없습니다.");
    });

    if (config.width <= 0 || config.height <= 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::initialize",
                                   "창 너비와 높이는 양수여야 합니다.");

    if (!glfwInit())
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwInit",
                                   "GLFW 초기화 실패: 앞선 GLFW 오류를 확인하세요.");
    glfwInitialized_ = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    window_ = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);

    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwCreateWindow",
                                   "OpenGL 3.3 코어 창 생성 실패.",
                                   "앞선 GLFW 오류와 그래픽 드라이버의 OpenGL 지원을 확인하세요.");
    }

    if (!ensureContext("initialize -> attach new window", __FILE__, __LINE__))
        return false;

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glewInit", "GLEW 초기화 실패.");
    glfwSwapInterval(config.verticalSync ? 1 : 0);

    const auto vertexPath = config.shaderDirectory.empty()
                                ? std::filesystem::path{}
                                : detail::relativePathInside(root, config.shaderDirectory) / "VertexShader.glsl";
    const auto fragmentPath = config.shaderDirectory.empty()
                                  ? std::filesystem::path{}
                                  : detail::relativePathInside(root, config.shaderDirectory) / "FragmentShader.glsl";

    Diagnostics::locate(__FILE__, __LINE__, "initialize -> Renderer::initialize -> shader source/path setup");

    if (!renderer_.initialize(vertexPath, fragmentPath, config.renderConfig))
        return false;
    assets_.emplace(root, config.features);
    renderOptions_.textures = config.features.textures;

    glfwSetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.modifiedKeyHandler_)
                app.modifiedKeyHandler_(app, key, action, mods);
            else if (app.keyHandler_)
                app.keyHandler_(app, key, action);
        });
    });
    glfwSetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.focusHandler_)
                app.focusHandler_(app, focused == GLFW_TRUE);
        });
    });
    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));

        app.dispatch([&] {
            if (!app.mouseHandler_)
                return;

            MouseEvent event;

            event.button = button;
            event.action = action;
            event.mods = mods;
            glfwGetCursorPos(window, &event.windowX, &event.windowY);

            int ww = 0, wh = 0, fw = 0, fh = 0;

            glfwGetWindowSize(window, &ww, &wh);
            glfwGetFramebufferSize(window, &fw, &fh);

            if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
                return;
            event.framebufferX = event.windowX * fw / ww;
            event.framebufferY = event.windowY * fh / wh;
            app.mouseHandler_(app, event);
        });
    });
    glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double x, double y) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.cursorHandler_)
                app.cursorHandler_(app, x, y);
        });
    });

    return true;
}

void Application::shutdown() {
    assets_.reset();

    if (window_ != nullptr) {
        if (!contextFailed_ && ensureContext("shutdown -> GPU cleanup", __FILE__, __LINE__))
            renderer_.destroy();
        else
            renderer_.abandon();
        glfwSetWindowUserPointer(window_, nullptr);
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    if (glfwInitialized_) {
        glfwTerminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderScene",
                                   "활성 창 또는 컨텍스트가 없습니다.");
    }

    if (!ensureContext("renderScene -> single pass", __FILE__, __LINE__))
        return false;

    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window_, &width, &height);

    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

std::array<int, 2> Application::getFramebufferSize() const {
    std::array<int, 2> size{};

    if (window_)
        glfwGetFramebufferSize(window_, &size[0], &size[1]);

    return size;
}

bool Application::renderViews(const std::vector<RenderPass>& passes) {
    if (!window_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderViews",
                                   "활성 창 또는 컨텍스트가 없습니다.");

    if (!ensureContext("renderViews -> shared context for all viewports", __FILE__, __LINE__))
        return false;

    const auto size = getFramebufferSize();

    return renderer_.renderViews(scene_, passes, size[0], size[1]);
}

int Application::run(const ApplicationConfig& config) {
    Diagnostics::configure(config.diagnostics);
    Diagnostics::clear();

    if (running_) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "run() 재귀 호출은 허용하지 않습니다.");

        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }

    if (detail::applicationActive()) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "동시에 실행 중인 Application은 하나만 지원합니다.");

        return -1;
    }
#if defined(_MSVC_EXECUTION_CHARACTER_SET) && _MSVC_EXECUTION_CHARACTER_SET == 65001
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
#else
    // Default MSVC execution charset uses the Windows ANSI code page (e.g. CP949).
    SetConsoleOutputCP(GetACP());
#endif
    detail::applicationActive() = true;
    callbackError_ = nullptr;
    running_ = true;

    int exitCode = 0;

    try {
        if (!initialize(config)) {
            std::cerr << "애플리케이션 초기화 실패.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Application::initHandler",
                                "사용자 초기화 콜백이 false를 반환했습니다.",
                                "앞선 API 오류와 장면 생성 콜백을 확인하세요.");
            std::cerr << "사용자 장면 초기화 실패.\n";
            exitCode = -1;
        } else {
            double previousTime = glfwGetTime();

            while (!glfwWindowShouldClose(window_)) {
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwPollEvents/input callbacks");
                glfwPollEvents();

                if (callbackError_)
                    std::rethrow_exception(callbackError_);

                if (glfwWindowShouldClose(window_)) {
                    break;
                }

                int width = 0;
                int height = 0;

                glfwGetFramebufferSize(window_, &width, &height);

                if (width <= 0 || height <= 0) {
                    glfwWaitEvents();

                    if (callbackError_)
                        std::rethrow_exception(callbackError_);
                    previousTime = glfwGetTime();
                    continue;
                }

                const double currentTime = glfwGetTime();
                const float deltaTime = static_cast<float>(std::clamp(currentTime - previousTime, 0.0, 0.05));

                previousTime = currentTime;

                if (updateHandler_) {
                    Diagnostics::trace(PipelineStage::Input, "Application::updateHandler");
                    updateHandler_(*this, deltaTime);
                }

                if (glfwWindowShouldClose(window_)) {
                    break;
                }

                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "모션 갱신 실패.\n";
                    exitCode = -1;
                    break;
                }

                if (!ensureContext("run -> render callback", __FILE__, __LINE__)) {
                    exitCode = -1;
                    break;
                }

                const bool rendered = renderHandler_ ? renderHandler_(*this) : renderScene();

                if (!rendered) {
                    std::cerr << "렌더링 실패.\n";
                    exitCode = -1;
                    break;
                }
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwSwapBuffers");

                const auto swapSerial = detail::contextErrorSerial();

                glfwSwapBuffers(window_);

                if (swapSerial != detail::contextErrorSerial()) {
                    contextFailed_ = true;
                    exitCode = -1;
                    break;
                }
            }
        }
    } catch (const std::exception& error) {
        Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::run", error.what(),
            "앞선 파이프라인 단계/API 오류를 확인하세요. 사용자 콜백 예외는 자원 정리 후 다시 전달됩니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw;
    } catch (...) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "콜백 또는 실행 처리에서 표준 예외가 아닌 예외가 발생했습니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    detail::applicationActive() = false;

    return exitCode;
}

void Application::setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler) {
    mouseHandler_ = std::move(handler);
}

void Application::setCursorHandler(std::function<void(Application&, double, double)> handler) {
    cursorHandler_ = std::move(handler);
}

std::optional<PickResult> Application::pickAt(double x, double y) const {
    if (!window_)
        return {};

    int ww = 0, wh = 0, fw = 0, fh = 0;

    glfwGetWindowSize(window_, &ww, &wh);
    glfwGetFramebufferSize(window_, &fw, &fh);

    if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
        return {};

    return graphics::pick(scene_, camera_, x * fw / ww, y * fh / wh, {0, 0, double(fw), double(fh)},
                          renderOptions_.cull);
}

std::optional<PickResult> Application::pickAtCursor() const {
    if (!window_)
        return {};

    double x = 0, y = 0;

    glfwGetCursorPos(window_, &x, &y);

    return pickAt(x, y);
}

AssetLoader& Application::getAssets() {
    if (!assets_)
        throw std::logic_error("AssetLoader는 run 실행 중 콜백에서만 사용할 수 있습니다.");

    return *assets_;
}
} // namespace graphics

/*
================================================================================
  OpenGL Framework & Practice 28 시험장 / 기본 가이드
================================================================================

0. 시험장 빠른 세팅 (새 기본 VS 프로젝트에 gllib.cpp + dependencies\만 넣을 때)
--------------------------------------------------------------------------------
- 준비물:
  * `gllib.cpp` + `dependencies\` 폴더 (OpenGL.props / 별도 main.cpp / 수동 DLL 복사 없이 바로 빌드·실행 가능)
  * 위 둘을 새 프로젝트 폴더(`$(ProjectDir)`, .vcxproj와 같은 위치)에 복사
- 새 VS 프로젝트(빈 프로젝트 / 콘솔 앱)에서 할 일:
  1) 솔루션 플랫폼을 반드시 **x64**로 선택 (`dependencies\`는 x64 전용)
  2) 프로젝트 속성 -> C/C++ -> 언어 -> C++ 언어 표준:
     **ISO C++17 표준(/std:c++17)** 이상 선택 (기본값 C++14에서는 std::filesystem 등 컴파일 불가, C++20/latest도 지원)
  3) 이 파일(`gllib.cpp`)을 프로젝트 `소스 파일`에 추가하고, 기존 템플릿 `main.cpp`가 있다면 제거
     * 파일 하단(`#ifndef GLLIB_NO_MAIN`)에 기본 `Console` 골격과 `int main()`이 포함되어 있음
     * (선택) 별도 `main.cpp`에서 `#include "gllib.cpp"`를 쓰고 싶다면 include 전에 `#define GLLIB_NO_MAIN` 선언 후 `gllib.cpp`는 빌드에서 제외
- 별도 설정이 필요 없는 항목 (`gllib.cpp`가 자동 처리):
  * Include / Library 경로 및 `.lib` 링크 (`__has_include` + `#pragma comment(lib, ...)`로 `dependencies\` 자동 연결)
  * 런타임 DLL 매핑 (`dependencies\dll\glfw3.dll`, `dependencies\dll\glew32.dll` 우선 로드)
    -> `/DELAYLOAD:glfw3.dll` + 시작 시 `LoadLibraryW` 프리로드 + `GLEW_STATIC` 내장 `glfwGetProcAddress` 로더로
       `$(OutDir)`에 DLL을 복사하거나 `PATH`를 바꾸지 않아도 자동 실행됨
  * 전처리기 `GLEW_STATIC`, `GLFW_DLL`, `GLFW_INCLUDE_NONE` (상단에 자동 정의)
  * `/utf-8` 옵션 (미지정 기본값 CP949/ANSI 및 `/utf-8` 모두 자동 대응)
  * `opengl32.lib`, `delayimp.lib`, `ole32.lib`, `windowscodecs.lib` (`#pragma comment(lib, ...)`로 자동 링크)
- 실행 시 주의:
  * 기본 내장 셰이더 사용 시 별도 GLSL 파일 불필요; 외부 셰이더/모델 사용 시 실행 파일 기준 상대 경로 확인

1. 의존성 및 빌드 구성 요약
--------------------------------------------------------------------------------
- 포함 헤더/라이브러리:
  * GLEW (`GLEW_STATIC` + 내장 `glfwGetProcAddress` 로더; `dependencies\dll\glew32.dll` 보관)
  * GLFW (`glfw3dll.lib` 지연 로드 + `dependencies\dll\glfw3.dll`), GLM
  * Windows SDK: `opengl32.lib`, `delayimp.lib`, `ole32.lib`, `windowscodecs.lib`
- 프로젝트 설정 요약:
  * C++17 이상 (/std:c++17, /std:c++20, /std:c++latest), x64
  * `gllib.cpp` 단독 컴파일(내장 `main()` 실행) 권장 (`OpenGL.props` 없이도 빌드·실행 가능)

2. 기본 실행 골격 (docs/code-guide/api.html · exam.html)
--------------------------------------------------------------------------------
  graphics::Application app;
  Console console(app); // run() 종료까지 수명 유지
  app.setInitHandler([&](graphics::Application&) { return console.initScene(); });
  app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
      console.onKey(key, action, mods);
  });
  app.setUpdateHandler([&](graphics::Application&, float dt) { console.onUpdate(dt); });
  app.setFocusHandler([&](graphics::Application&, bool focused) { console.onFocus(focused); });
  graphics::ApplicationConfig config;
  config.title = "Title";
  return app.run(config);

- run() 내부 순서:
  1) initialize(): GLFW/OpenGL 3.3 Core 창 생성 -> 컨텍스트 바인딩 -> glewInit()
                   -> 내장/외부 GLSL 셰이더 및 VAO/VBO/EBO 초기화
  2) initHandler() 1회 호출 (false 반환 시 종료)
  3) 매 프레임 루프:
     glfwPollEvents() -> 최소화 시(fb <= 0) glfwWaitEvents() 대기
     -> updateHandler(dt) (dt는 0~0.05초로 클램프)
     -> scene_.updateMotions(dt)
     -> renderHandler() 또는 기본 renderScene()
     -> glfwSwapBuffers()

3. 요구사항 -> 행동 -> API 호출 매핑 (docs/code-guide/api.html · exam.html)
--------------------------------------------------------------------------------
- 정적 장면 & 계층/관절:
  * Scene::addGroup(), addCuboid(), addSphere(), addCone(), addModel()
  * DiagramSystem::makePolyhedron(GLShape::Cube, size)
  * Scene::setParent(child, parent)로 트리 계층 구성 (순환 금지)
  * Transform3D: T(basis) * R(rotation) * S(scale) * T(-localBasis)
    -> 메시 배율(scale)과 관절 피벗/위치를 분리해야 비례 왜곡을 막는다.
- 키 입력 & 상태 관리:
  * 단발 토글: action == GLFW_PRESS 일 때만 상태 반전 (GLFW_REPEAT 무시)
  * 지속 이동: PRESS/RELEASE 시 pressed_[key] 갱신 -> onUpdate(dt)에서 dt 곱해 반영
  * onFocus(false)에서 pressed_ 등 지속 입력 상태를 모두 초기화
  * Scene::clear() 호출 후에는 기존 EntityId를 재사용하지 말고 새로 저장
- 모션 & 경로:
  * Motion::fixed / rotation / followPath / linear / pendulum / then / action
  * MotionSystem: createSpin(로컬 자전), createOrbit(월드 공전), pushLayer, bindEntity
- 다중 카메라 & 뷰포트:
  * Application::getFramebufferSize()로 실제 픽셀 크기 확인
  * RenderPass 목록 구성 후 Application::renderViews(passes) 호출
  * 업데이트(시간/충돌)는 프레임당 1회만 수행하고 렌더만 패스별로 분할
- 조명·명암·컬링:
  * RenderOptions: lighting, directionalIntensity, ambientColor, pointLights(최대 8), cull
  * Material: ambientStrength, diffuseStrength, specularStrength, shininess, unlit
  * 내장 셰이더는 방향광+점광원 Phong 표면 명암을 계산하며 cast shadow는 미지원
- 오류 진단 & 역추적:
  * ApplicationConfig::diagnostics = { .errors = true, .trace = true }
  * Diagnostics::lastError() 또는 Diagnostics::failAt(__FILE__, __LINE__, ...) 확인

4. 실습 28 아키텍처 및 기본 조작 요약 (openGL_3dAssign_28)
--------------------------------------------------------------------------------
- 4계층 구조:
  App(main + Console) -> Application(창/루프/콜백)
  -> Scene / MotionSystem / TransformSystem / Picking / AssetLoader
  -> Renderer(GPU VAO/VBO/EBO/Shader/Texture)
- 실습 28 키 조작 (테스트용 main.cpp 기준):
  * 방향키      : 탱크 XZ 평면 이동 (X/Z ±3.5 제한)
  * h           : 하부 궤도 높이 진동 토글
  * t           : 중앙 몸체 Y축 자전 토글
  * l           : 상부 두 블록 반원 경로 교환 / 완료 후 재입력 시 복귀
  * g           : 포신 반대 방향 Y축 회전 토글
  * p           : 깃대 반대 방향 X축 회전(±60도) 토글
  * w/a/s/d     : 카메라 수평 이동 (시선 기준 앞/뒤/좌/우)
  * x/X, z/Z    : 카메라 X / Z 축 평행 이동
  * r / R       : 원점 기준 카메라 Y축 공전 토글
  * y / Y       : 점광원 공전 토글
  * m           : 조명(lighting) On/Off
  * c           : 점광원 색상 순환 (흰/빨/초/파)
  * o           : 바닥·탱크 팔레트 색상 순환
  * q / Esc     : 종료
================================================================================
*/

#ifndef GLLIB_NO_MAIN
class Console {
    graphics::Application& app_;
    std::optional<graphics::EntityId> cube_;

public:
    explicit Console(graphics::Application& app) : app_(app) {}

    bool initScene() {
        auto& scene = app_.getScene();
        if (!scene.addBasis(glm::vec3(0.0f), glm::mat3(1.0f), glm::vec3(2.0f)))
            return false;

        cube_ = scene.addCuboid(glm::vec3(1.0f), glm::vec3(0.0f));
        if (!cube_)
            return false;

        return scene.setTint(*cube_, graphics::GlColor{0.35f, 0.7f, 0.95f});
    }

    void onKey(int key, int action, int mods) {
        (void)mods;
        if (action == GLFW_PRESS && (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q))
            app_.requestClose();
    }

    void onUpdate(float dt) {
        (void)dt;
    }

    void onFocus(bool focused) {
        (void)focused;
    }
};

int main() {
    graphics::Application app;
    Console console(app);
    app.setInitHandler([&](graphics::Application&) { return console.initScene(); });
    app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
        console.onKey(key, action, mods);
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) { console.onUpdate(dt); });
    app.setFocusHandler([&](graphics::Application&, bool focused) { console.onFocus(focused); });

    graphics::ApplicationConfig config;
    config.title = "OpenGL";
    return app.run(config);
}
#endif
