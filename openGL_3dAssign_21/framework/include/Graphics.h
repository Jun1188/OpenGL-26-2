#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include <map>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;
    void rebuildFaceIds();

public:
    Scene();
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position,
                                      unsigned slices = 32, unsigned stacks = 16);
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position,
                                    unsigned slices = 32);
    bool setTint(EntityId id, GlColor color);
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = Transform3D{});
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths,
                                     const glm::vec3& worldPosition);
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths,
                                               const glm::vec3& worldPosition);
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths,
                                      const glm::vec3& worldPosition);
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex,
                             const std::vector<GlColor>& colors);
    bool setPosition(EntityId id, const glm::vec3& worldPosition);
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
    bool setTransform(EntityId id, const Transform3D& transform);
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
    MotionSystem& getMotions() { return motions_; }
    const MotionSystem& getMotions() const { return motions_; }
    bool bindEntity(EntityId id, LayerId layer);
    bool unbindEntity(EntityId id);
    std::optional<LayerId> getEntityLayer(EntityId id) const;
    bool updateMotions(double deltaSeconds) { return motions_.update(deltaSeconds); }
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));
    bool setModel(EntityId id, const Model& model);
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;
    bool setMeshVertices(EntityId id, std::size_t meshIndex,
                         const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);
    bool setVertexPosition(EntityId id, std::size_t meshIndex,
                           std::size_t vertexIndex, const glm::vec3& position);
    bool setTriangleIndices(EntityId id, std::size_t meshIndex,
                            const std::vector<std::uint32_t>& indices);
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale,
                            const glm::vec2& offset, float rotationDegrees = 0);
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));
    bool setVisible(EntityId id, bool visible);
    bool splitFace(EntityId id, std::size_t faceIndex);
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);

    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end,
                                 GlColor color, std::optional<EntityId> owner = std::nullopt);
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector,
                                   GlColor color, std::optional<EntityId> owner = std::nullopt);
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    std::optional<std::array<LineId, 3>> addBasis(
        const glm::vec3& origin, const glm::mat3& basisVectors = glm::mat3(1.0f),
        const glm::vec3& axisLengths = glm::vec3(1.0f),
        std::optional<EntityId> owner = std::nullopt);
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
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);
    static Model makeCone(float radius, float height, unsigned slices = 32);
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
    float fieldOfViewDegrees = 45.0f;
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
    CullMode cull = CullMode::None;
    bool depthTest = true;
    bool wireframe = false;
    bool textures = false; // Explicit opt-in; no texture GPU work when false.
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
};

struct Viewport { int x=0, y=0, width=0, height=0; }; // Bottom-left framebuffer pixels.
struct RenderPass {
    Camera camera;
    Viewport viewport;
    RenderOptions options;
    bool clearColor=true, clearDepth=true;
};

class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1, tintLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    using TextureMap = std::map<std::shared_ptr<const TextureImage>, GLuint,
                               std::owner_less<std::shared_ptr<const TextureImage>>>;
    TextureMap textures_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;
    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    bool upload(const Scene& scene, bool textures);
    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;
public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{}, const std::filesystem::path& fragmentPath = std::filesystem::path{});
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});
    bool renderPass(const Scene& scene,const RenderPass& pass,int framebufferWidth,int framebufferHeight);
    bool renderViews(const Scene& scene,const std::vector<RenderPass>& passes,int width,int height);
    void destroy(); // Call while the owning context is current, before it is destroyed.
};
} // namespace graphics
#include "../detail/Scene.inl"
#include "../detail/Renderer.inl"
