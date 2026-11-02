#pragma once
#include "Application.h"
#include "GridLayout.h"
#include <random>

namespace graphics {
struct Bounds3 {
    glm::vec3 minimum{-1}, maximum{1};
    bool valid() const;
    bool contains(const glm::vec3& point,
                  const glm::vec3& margin = glm::vec3(0)) const; // margin만큼 줄인 내부 영역을 검사한다.
    bool overlaps(const Bounds3& other) const;
};

struct PathPose {
    glm::vec3 position{0}, tangent{0, 0, 1};
    float yawDegrees = 0;
};

class DistancePath {
    std::vector<glm::vec3> points_;
    std::vector<double> distances_;

public:
    bool setPoints(const std::vector<glm::vec3>& points, bool closed = false);

    double length() const {
        return distances_.empty() ? 0 : distances_.back();
    }

    std::optional<PathPose> sample(double distance, Playback playback = Playback::Loop) const;
};

struct AnimationClock {
    double seconds = 0;
    bool playing = true;
    bool advance(double dt); // playing일 때만 진행. dt는 초 단위.

    void reset() {
        seconds = 0;
        playing = true;
    }
};

struct RoomBody {
    glm::vec3 localPosition{0}, localVelocity{0}, halfExtent{.2f};
    glm::vec3 worldPosition{0}, worldVelocity{0};
    bool released = false;

    glm::vec3 position(const glm::mat4& roomWorld) const {
        return released ? worldPosition : glm::vec3(roomWorld * glm::vec4(localPosition, 1));
    }
};

struct CharacterBody {
    glm::vec3 feet{0}, direction{0, 0, 1};
    glm::vec2 halfWidth{.25f};
    float height = 1.6f, speed = 1, verticalSpeed = 0;
    double walkDistance = 0;
    bool grounded = true;
};

struct CharacterStep {
    bool blocked = false, turned = false, landed = false;
};

class CollisionSystem {
public:
    // Rigid, kinematic room. Boxes stay aligned to its axes until release.
    static bool stepRoomBody(RoomBody& body, const Bounds3& room, const glm::mat4& roomWorld, float dt, bool floorOpen,
                             bool elastic, const glm::vec3& gravity = glm::vec3(0, -9.8f, 0),
                             const glm::vec3& angularVelocity = glm::vec3(0));
    static bool jump(CharacterBody& body, float speed); // 공중 입력은 오류 없이 false를 반환한다.
    static std::optional<CharacterStep> stepCharacter(CharacterBody& body, const Bounds3& stage,
                                                      const std::vector<Bounds3>& obstacles, float dt,
                                                      float gravity = 9.8f);
    static std::optional<Bounds3> randomFreeBox(const Bounds3& area, const glm::vec3& size,
                                                const std::vector<Bounds3>& occupied, std::mt19937& random,
                                                unsigned attempts = 128); // 난수 엔진의 시드·수명은 호출자가 관리한다.
};

struct JointRig {
    EntityId root = 0, head = 0;
    std::array<EntityId, 2> shoulders{}, hips{};
};

class PatternSystem {
public:
    static std::optional<PathPose> conveyorCornerPose(double distance, const glm::vec3& start, const glm::vec3& pivot,
                                                      float radius,
                                                      float endZ); // 직선 → 코너 원호 → 직선 경로를 순환한다.
    static std::optional<EntityId> addBox(Scene& scene, const glm::vec3& size, const glm::vec3& position, GlColor color,
                                          std::optional<EntityId> parent = std::nullopt);
    static std::optional<EntityId> addPolygon(
        Scene& scene, const std::vector<glm::vec3>& points, GlColor color,
        std::optional<EntityId> parent = std::nullopt); // 단일 면 생성. 오목 다각형 자동 분할은 지원하지 않는다.
    static std::optional<EntityId> addRoom(
        Scene& scene, const Bounds3& bounds,
        bool frontOpen = true); // 면 0~3: 옆벽, 4: 천장, 5: 바닥. 안쪽 법선과 Front cull 사용.
    static std::optional<EntityId> addJoint(Scene& scene, EntityId parent, const glm::vec3& anchor);
    static bool rotateJoint(Scene& scene, EntityId joint, const glm::vec3& axis, float degrees);
    static bool follow(Scene& scene, EntityId entity, const DistancePath& path, double distance,
                       const glm::vec3& offset = glm::vec3(0), bool orient = true); // 방향은 +Z를 기준으로 Y축 회전.
    static bool addNumber(Scene& scene, unsigned digit, const glm::vec3& position, float height, GlColor color,
                          std::optional<EntityId> parent = std::nullopt); // 숫자 0~9를 부모 로컬 XY 평면에 표시한다.
    static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);
    static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement,
                         const glm::vec3& topPivot,
                         bool scaleOpen); // progress는 [0,1]. 완전 개방 시 숨겨 0 배율을 피한다.
    static std::optional<std::size_t> cameraFacingWall(
        const Scene& scene, EntityId room,
        const Camera& camera); // 시선과 바깥 법선의 내적이 최대인 양수 면. 수직 시선은 빈 결과 가능.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face,
                             float progress);                              // 상단 피벗을 기준으로 높이를 축소·복원한다.
    static bool swapModels(Scene& scene, EntityId first, EntityId second); // 위치는 유지한다.
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
    static bool aligned(float first, float second, float tolerance);
    static glm::vec3 snap(const glm::vec3& point, float spacing); // 잘못된 간격이면 입력 점을 유지한다.
    static std::optional<glm::vec3> screenOnPlane(
        const Camera& camera, double x, double y, const PickViewport& viewport, const glm::vec3& planePoint,
        const glm::vec3& normal); // 평행하거나 카메라 뒤에서 교차하면 빈 결과.
    static bool moveCamera(Camera& camera, const glm::vec3& offset);
    static bool yawCamera(Camera& camera, float degrees); // 시점 고정. 카메라 up축 기준 회전.
    static bool orbitCamera(Camera& camera, const AxisLine& axis, float degrees);
    static bool zoomCamera(Camera& camera, float factor);
    static std::vector<RenderPass> threeViews(
        const Camera& perspective, int width, int height,
        const RenderOptions& options = RenderOptions{}); // 원근·XZ 평면도·XY 정면도. 평면도의 up은 -Z.
    static std::optional<JointRig> addWalkingRig(Scene& scene, const GridLayout& bodyGrid);
    static bool poseWalkingRig(Scene& scene, const JointRig& rig, const glm::vec3& feet, float yawDegrees,
                               double walkDistance, float speed);
};
} // namespace graphics
