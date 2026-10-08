#pragma once

#include <glew.h>
#include <glm/glm.hpp>

#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_ENTITIES = 50;

using EntityId = std::size_t;

struct GlColor {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
};

enum class GLShape { None, Cube, Tetra };
enum class GLFace { None, Rect, Triangle };

struct Transform3D {
    glm::vec3 basis{ 0.0f };          // 월드 공간에서 localBasis가 놓일 위치.
    glm::vec3 localBasis{ 0.0f };     // 로컬 공간의 회전·크기 기준점.
    glm::vec3 rotationDegrees{ 0.0f };
    glm::vec3 scale{ 1.0f };
};

struct PlaneFigure {
    GLFace shape = GLFace::None;
    std::vector<glm::vec3> localVertices; // 정점은 CPU에서 월드 좌표로 바꾸지 않는다.
    GlColor color;
    glm::vec3 normal{ 0.0f };             // 로컬 면 법선. 현재는 색상 렌더링만 사용한다.
    bool independence = false;
    Transform3D transform;                // 독립 면일 때 사용할 월드 변환.
};

struct Polyhedron {
    GLShape shape = GLShape::None;
    std::vector<PlaneFigure> faces; // 육면체: 사각형 6개, 사면체: 삼각형 4개.
};

struct Component_Poly { // C: 상태만 보관한다. 계산은 DiagramSystem에 맡긴다.
    Polyhedron polyhedron;
    Transform3D transform;
};

struct FaceId {
    EntityId entityId = 0;
    std::size_t faceIndex = 0;
};

class Scene { // E: ID를 등록하고 같은 인덱스의 컴포넌트에 연결한다.
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_; // 입체와 면 데이터의 단일 소유자.
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::uint64_t geometryRevision_ = 0;
public:
    Scene();
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = {});
    bool setTransform(EntityId id, const Transform3D& transform);
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);
    void clear();

    const std::vector<EntityId>& getIds() const { return polyId_; }
    const Component_Poly* getComponent(EntityId id) const;
    const std::vector<FaceId>& getCubeFaceIds() const { return cubeFaceId_; }
    const std::vector<FaceId>& getTetraFaceIds() const { return tetraFaceId_; }
    std::uint64_t getGeometryRevision() const { return geometryRevision_; }
    
};

class DiagramSystem { // S: 형상 생성과 변환 계산. OpenGL 핸들은 다루지 않는다.
public:
    static Polyhedron makePolyhedron(GLShape shape);
    static bool validPolyhedron(const Polyhedron& polyhedron);
    static bool validTransform(const Transform3D& transform);
    static glm::mat4 modelMatrix(const Transform3D& transform);
    static glm::mat4 faceModelMatrix(const Component_Poly& component,
                                    const PlaneFigure& face);
};

struct Camera { // 렌더링할 형상이 없는 별도의 장면 객체.
    glm::vec3 position{ 2.8f, 2.0f, 5.0f };
    glm::vec3 target{ 0.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};

class CameraSystem {
public:
    static glm::mat4 viewMatrix(const Camera& camera);
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

std::string readTextFile(const std::filesystem::path& path);

class Renderer {
private:
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::size_t firstIndex = 0;
        GLsizei count = 0;
    };

    GLuint shaderProgramID_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_[2] = { 0, 0 }; // 로컬 위치와 색상의 분리 스트림.
    GLuint ebo_ = 0;
    GLint mvpLocation_ = -1;
    std::vector<DrawRange> drawRanges_;

public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    bool initShader(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    bool initBuffer();
    bool upload(const Scene& entities);
    bool render(const Scene& entities, const glm::mat4& view,
                const glm::mat4& projection) const;
    void destroy(); // OpenGL 컨텍스트가 살아 있을 때 호출한다.

};

} // namespace graphics
