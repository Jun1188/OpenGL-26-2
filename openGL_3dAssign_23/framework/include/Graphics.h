#pragma once

#include "Geometry.h"
#include "Motion.h"
#include "../detail/Native.h"
#include "RenderConfig.h"
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

    /// [API] 직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getCubeFaceIds() const {
        return cubeFaceId_;
    }

    /// [API] 사면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getTetraFaceIds() const {
        return tetraFaceId_;
    }

    /// [API] 피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getPyramidFaceIds() const {
        return pyramidFaceId_;
    }

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
    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);
    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);

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
