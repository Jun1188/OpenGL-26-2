#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_ENTITIES = 50;
constexpr std::size_t MAX_LINES = 500;

using EntityId = std::size_t;
using LineId = std::size_t;

struct GlColor {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
};

enum class GLShape { None, Cube, Tetra, Pyramid }; // Cube는 직육면체, Tetra는 직사면체.
enum class GLFace { None, Rect, Triangle };

struct Transform3D {
    glm::vec3 basis{ 0.0f }; // 월드 위치. 방향 기저는 addBasis의 mat3로 구분한다.
    glm::vec3 localBasis{ 0.0f };
    glm::vec3 rotationDegrees{ 0.0f };
    glm::vec3 scale{ 1.0f };
    glm::quat orientation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Euler 회전 다음에 적용한다.
};

struct PlaneFigure {
    GLFace shape = GLFace::None;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{ 0.0f }; // 로컬 법선 데이터. 조명은 현재 사용하지 않는다.
    bool independence = false;
    Transform3D transform;   // 독립 면일 때의 월드 변환.
    bool visible = true;
    glm::mat4 localAnimationMatrix{ 1.0f };
    std::vector<GlColor> vertexColors; // 비어 있으면 면의 단색을 사용한다.
};

struct Polyhedron {
    GLShape shape = GLShape::None;
    std::vector<PlaneFigure> faces;
};

struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{ 1.0f }; // scale과 구분한 실제 로컬 모서리 길이.
};

struct FaceId {
    EntityId entityId = 0;
    std::size_t faceIndex = 0;
};

struct Line3D {
    glm::vec3 start{ 0.0f };
    glm::vec3 end{ 0.0f };
    GlColor color;
    std::optional<EntityId> owner; // 없으면 월드 좌표, 있으면 객체의 로컬 좌표.
};

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;

public:
    Scene();
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = {});
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths,
                                     const glm::vec3& worldPosition);
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths,
                                               const glm::vec3& worldPosition);
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths,
                                      const glm::vec3& worldPosition);
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex,
                             const std::vector<GlColor>& colors);
    bool setPosition(EntityId id, const glm::vec3& worldPosition);
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
    bool setTransform(EntityId id, const Transform3D& transform);
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end,
                                 GlColor color, std::optional<EntityId> owner = {});
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector,
                                   GlColor color, std::optional<EntityId> owner = {});
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    std::optional<std::array<LineId, 3>> addBasis(
        const glm::vec3& origin, const glm::mat3& basisVectors = glm::mat3(1.0f),
        const glm::vec3& axisLengths = glm::vec3(1.0f),
        std::optional<EntityId> owner = {});
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.

    const std::vector<EntityId>& getIds() const { return polyId_; }
    const Component_Poly* getComponent(EntityId id) const;
    const std::vector<FaceId>& getCubeFaceIds() const { return cubeFaceId_; }
    const std::vector<FaceId>& getTetraFaceIds() const { return tetraFaceId_; }
    const std::vector<FaceId>& getPyramidFaceIds() const { return pyramidFaceId_; }
    const std::vector<Line3D>& getLines() const { return lines_; }
    std::uint64_t getGeometryRevision() const { return geometryRevision_; }
};

class DiagramSystem {
public:
    static Polyhedron makePolyhedron(GLShape shape,
                                     const glm::vec3& sideLengths = glm::vec3(1.0f));
    static bool validPolyhedron(const Polyhedron& polyhedron);
    static bool validTransform(const Transform3D& transform);
    static glm::mat4 modelMatrix(const Transform3D& transform);
    static glm::mat4 faceModelMatrix(const Component_Poly& component,
                                    const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{ 2.8f, 2.0f, 5.0f };
    glm::vec3 target{ 0.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    float fieldOfViewDegrees = 45.0f;//카메라가 이미 회전한 상태다 - 45 degrees

    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

class CameraSystem {
public:
    static bool validCamera(const Camera& camera);
    static glm::mat4 viewMatrix(const Camera& camera);
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

std::string readTextFile(const std::filesystem::path& path);

struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
};

class Renderer {
private:
    struct DrawRange {
        GLenum mode = GL_TRIANGLES;
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0;
        GLsizei count = 0;
        std::size_t outlineFirstIndex = 0;
        GLsizei outlineCount = 0;
    };

    GLuint shaderProgramID_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_[2] = { 0, 0 };
    GLuint ebo_ = 0;
    GLint mvpLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();

    bool initShader(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    bool initBuffer();
    bool upload(const Scene& scene);
    bool draw(const Scene& scene, const glm::mat4& view,
              const glm::mat4& projection, const RenderOptions& options) const;

public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // 호출자가 유효한 OpenGL 3.3 컨텍스트를 current로 설정한 뒤 호출한다.
    bool initialize(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    // viewport/depth/clear/program/VAO를 설정한다. 컨텍스트와 윈도우는 소유하지 않는다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = {});
    void destroy(); // 컨텍스트 종료 전에 호출한다. 여러 번 호출해도 된다.
};

} // namespace graphics
