#pragma once
#include "Application.h"
#include "GridLayout.h"
#include <random>

namespace graphics {
struct Bounds3 {
    glm::vec3 minimum{-1}, maximum{1};
    /// [API] 해당 데이터가 계산에 필요한 유효 범위와 유한성 조건을 만족하는지 반환한다.
    bool valid() const;
    /// [API] 점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.
    bool contains(const glm::vec3& point, const glm::vec3& margin = glm::vec3(0)) const;
    /// [API] 두 축 정렬 경계 상자가 겹치는지 검사한다.
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
    /// [API] 연속 점들의 누적 거리 경로를 구성한다. closed면 마지막에서 처음으로 연결한다.
    bool setPoints(const std::vector<glm::vec3>& points, bool closed = false);

    /// [API] 경로 전체 누적 길이를 반환한다. 경로가 비어 있으면 0이다.
    double length() const {
        return distances_.empty() ? 0 : distances_.back();
    }

    /// [API] 누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.
    std::optional<PathPose> sample(double distance, Playback playback = Playback::Loop) const;
};

struct AnimationClock {
    double seconds = 0;
    bool playing = true;
    /// [API] playing일 때만 누적 시간을 늘린다. dt와 누적 값의 유한성·범위를 검사한다.
    bool advance(double dt);

    /// [API] 누적 시간을 0으로 되돌리고 재생 상태를 켠다.
    void reset() {
        seconds = 0;
        playing = true;
    }
};

struct RoomBody {
    glm::vec3 localPosition{0}, localVelocity{0}, halfExtent{.2f};
    glm::vec3 worldPosition{0}, worldVelocity{0};
    bool released = false;

    /// [API] 이탈 전에는 roomWorld로 로컬 중심을 월드로 변환하고 이탈 후에는 worldPosition을 반환한다.
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
    /// [API] 방 내부에서는 로컬 반사/중력을 계산하고 released 뒤에는 월드 중력으로 적분한다.
    static bool stepRoomBody(RoomBody& body, const Bounds3& room, const glm::mat4& roomWorld, float dt, bool floorOpen,
                             bool elastic, const glm::vec3& gravity = glm::vec3(0, -9.8f, 0),
                             const glm::vec3& angularVelocity = glm::vec3(0));
    /// [API] 접지 상태에서만 위쪽 속도를 설정한다. 공중 입력 거절은 정상적인 false다.
    static bool jump(CharacterBody& body, float speed);
    /// [API] 수평 이동·장애물/경계 반전·중력·윗면 착지·지지면 이탈을 하위 단계로 계산한다.
    static std::optional<CharacterStep> stepCharacter(CharacterBody& body, const Bounds3& stage,
                                                      const std::vector<Bounds3>& obstacles, float dt,
                                                      float gravity = 9.8f);
    /// [API] 제한 횟수 안에서 occupied와 겹치지 않는 배치를 찾는다. 난수 엔진의 시드·수명은 호출자가 관리한다.
    static std::optional<Bounds3> randomFreeBox(const Bounds3& area, const glm::vec3& size,
                                                const std::vector<Bounds3>& occupied, std::mt19937& random,
                                                unsigned attempts = 128);
};

struct JointRig {
    EntityId root = 0, head = 0;
    std::array<EntityId, 2> shoulders{}, hips{};
};

class PatternSystem {
public:
    /// [API] 직선 → 안쪽 코너 중심 원호 → 직선의 거리에서 위치·접선·yaw를 계산한다. 끝에서 처음으로 순환한다.
    static std::optional<PathPose> conveyorCornerPose(double distance, const glm::vec3& start, const glm::vec3& pivot,
                                                      float radius, float endZ);
    /// [API] 양수 크기의 중심 기준 모델을 만들고 부모 로컬 위치·색·부모 관계를 설정한다.
    static std::optional<EntityId> addBox(Scene& scene, const glm::vec3& size, const glm::vec3& position, GlColor color,
                                          std::optional<EntityId> parent = std::nullopt);
    /// [API] 점 목록으로 단일 다각형 면을 만들고 색·부모 관계와 함께 Scene에 등록한다. 오목 다각형의 자동 분할을 가정하지 않는다.
    static std::optional<EntityId> addPolygon(Scene& scene, const std::vector<glm::vec3>& points, GlColor color,
                                              std::optional<EntityId> parent = std::nullopt);
    /// [API] 여섯 면의 방을 만들고 바깥 winding에 Front cull, 안쪽 조명 법선을 적용한다. face 0..3은 옆벽, 4 천장, 5 바닥이다.
    static std::optional<EntityId> addRoom(Scene& scene, const Bounds3& bounds, bool frontOpen = true);
    /// [API] 부모에 부착된 transform-only 그룹을 만든다. 표시 메시 크기와 회전 연결 위치를 분리한다.
    static std::optional<EntityId> addJoint(Scene& scene, EntityId parent, const glm::vec3& anchor);
    /// [API] 연결 위치와 크기를 유지하면서 회전 기저를 변경한다.
    static bool rotateJoint(Scene& scene, EntityId joint, const glm::vec3& axis, float degrees);
    /// [API] DistancePath 샘플을 엔티티 위치와 +Z 기준 Y 회전에 반영한다.
    static bool follow(Scene& scene, EntityId entity, const DistancePath& path, double distance,
                       const glm::vec3& offset = glm::vec3(0), bool orient = true);
    /// [API] 0~9를 선분으로 구성한다. 부모 로컬 XY 평면의 표시이며 일반 폰트/텍스트 렌더러는 아니다.
    static bool addNumber(Scene& scene, unsigned digit, const glm::vec3& position, float height, GlColor color,
                          std::optional<EntityId> parent = std::nullopt);
    /// [API] 지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.
    static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);
    /// [API] 진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.
    static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement,
                         const glm::vec3& topPivot, bool scaleOpen);
    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);
    /// [API] 선택 옆벽의 정점에서 상단 피벗을 계산하고 openFace로 높이를 축소/복원한다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
    /// [API] 두 값의 차이가 허용 오차 이하인지 검사한다.
    static bool aligned(float first, float second, float tolerance);
    /// [API] 좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.
    static glm::vec3 snap(const glm::vec3& point, float spacing);
    /// [API] 화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.
    static std::optional<glm::vec3> screenOnPlane(const Camera& camera, double x, double y,
                                                  const PickViewport& viewport, const glm::vec3& planePoint,
                                                  const glm::vec3& normal);
    /// [API] 시점과 주목점을 동일하게 이동해 시선 방향을 보존한다.
    static bool moveCamera(Camera& camera, const glm::vec3& offset);
    /// [API] 시점은 고정하고 카메라 로컬 up축을 기준으로 시선을 회전한다.
    static bool yawCamera(Camera& camera, float degrees);
    /// [API] 월드 축에 대해 시점·주목점·up을 함께 변환한다. 축을 target에 두면 target은 고정된다.
    static bool orbitCamera(Camera& camera, const AxisLine& axis, float degrees);
    /// [API] 원근은 target까지 거리, 직각은 orthographicHalfExtent를 배율로 바꾼다.
    static bool zoomCamera(Camera& camera, float factor);
    /// [API] 원근·XZ 평면도·XY 정면도 카메라와 픽셀 영역을 구성한다. 상단 뷰의 up은 -Z다.
    static std::vector<RenderPass> threeViews(const Camera& perspective, int width, int height,
                                              const RenderOptions& options = RenderOptions{});
    /// [API] 그리드 비율로 머리·코·몸통·어깨·엉덩이·팔·다리 계층을 조립하고 ID 묶음을 반환한다.
    static std::optional<JointRig> addWalkingRig(Scene& scene, const GridLayout& bodyGrid);
    /// [API] 루트 이동/yaw와 실제 이동 거리 기반 팔다리 반대 위상을 적용한다. 속도는 보행 진폭에 반영한다.
    static bool poseWalkingRig(Scene& scene, const JointRig& rig, const glm::vec3& feet, float yawDegrees,
                               double walkDistance, float speed);
};
} // namespace graphics
