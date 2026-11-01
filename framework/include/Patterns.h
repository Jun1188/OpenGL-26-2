#pragma once
#include "Application.h"
#include "GridLayout.h"
#include <random>

namespace graphics {
struct Bounds3 {
    glm::vec3 minimum{-1}, maximum{1};
    bool valid() const;
    bool contains(const glm::vec3& point,const glm::vec3& margin=glm::vec3(0)) const;
    bool overlaps(const Bounds3& other) const;
};
struct PathPose { glm::vec3 position{0}, tangent{0,0,1}; float yawDegrees=0; };
class DistancePath {
    std::vector<glm::vec3> points_;
    std::vector<double> distances_;
public:
    bool setPoints(const std::vector<glm::vec3>& points,bool closed=false);
    double length() const { return distances_.empty()?0:distances_.back(); }
    std::optional<PathPose> sample(double distance,Playback playback=Playback::Loop) const;
};
struct AnimationClock {
    double seconds=0;
    bool playing=true;
    bool advance(double dt);
    void reset() { seconds=0;playing=true; }
};
struct RoomBody {
    glm::vec3 localPosition{0}, localVelocity{0}, halfExtent{.2f};
    glm::vec3 worldPosition{0}, worldVelocity{0};
    bool released=false;
    glm::vec3 position(const glm::mat4& roomWorld) const {
        return released?worldPosition:glm::vec3(roomWorld*glm::vec4(localPosition,1));
    }
};
struct CharacterBody {
    glm::vec3 feet{0}, direction{0,0,1};
    glm::vec2 halfWidth{.25f};
    float height=1.6f, speed=1, verticalSpeed=0;
    double walkDistance=0;
    bool grounded=true;
};
struct CharacterStep { bool blocked=false, turned=false, landed=false; };
class CollisionSystem {
public:
    // Rigid, kinematic room. Boxes stay aligned to its axes until release.
    static bool stepRoomBody(RoomBody& body,const Bounds3& room,const glm::mat4& roomWorld,
                             float dt,bool floorOpen,bool elastic,
                             const glm::vec3& gravity=glm::vec3(0,-9.8f,0),
                             const glm::vec3& angularVelocity=glm::vec3(0));
    static bool jump(CharacterBody& body,float speed);
    static std::optional<CharacterStep> stepCharacter(CharacterBody& body,const Bounds3& stage,
                             const std::vector<Bounds3>& obstacles,float dt,float gravity=9.8f);
    static std::optional<Bounds3> randomFreeBox(const Bounds3& area,const glm::vec3& size,
                             const std::vector<Bounds3>& occupied,std::mt19937& random,unsigned attempts=128);
};
struct JointRig {
    EntityId root=0, head=0;
    std::array<EntityId,2> shoulders{}, hips{};
};
class PatternSystem {
public:
    static std::optional<EntityId> addBox(Scene& scene,const glm::vec3& size,const glm::vec3& position,
                                         GlColor color,std::optional<EntityId> parent=std::nullopt);
    static std::optional<EntityId> addPolygon(Scene& scene,const std::vector<glm::vec3>& points,
                                         GlColor color,std::optional<EntityId> parent=std::nullopt);
    static std::optional<EntityId> addRoom(Scene& scene,const Bounds3& bounds,bool frontOpen=true);
    static std::optional<EntityId> addJoint(Scene& scene,EntityId parent,const glm::vec3& anchor);
    static bool rotateJoint(Scene& scene,EntityId joint,const glm::vec3& axis,float degrees);
    static bool follow(Scene& scene,EntityId entity,const DistancePath& path,double distance,
                        const glm::vec3& offset=glm::vec3(0),bool orient=true);
    static bool addNumber(Scene& scene,unsigned digit,const glm::vec3& position,float height,
                          GlColor color,std::optional<EntityId> parent=std::nullopt);
    static bool foldFace(Scene& scene,EntityId entity,std::size_t face,const AxisLine& hinge,float degrees);
    static bool openFace(Scene& scene,EntityId entity,std::size_t face,float progress,
                         const glm::vec3& displacement,const glm::vec3& topPivot,bool scaleOpen);
    static bool swapModels(Scene& scene,EntityId first,EntityId second);
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
    static bool aligned(float first,float second,float tolerance);
    static glm::vec3 snap(const glm::vec3& point,float spacing);
    static std::optional<glm::vec3> screenOnPlane(const Camera& camera,double x,double y,
                         const PickViewport& viewport,const glm::vec3& planePoint,const glm::vec3& normal);
    static bool moveCamera(Camera& camera,const glm::vec3& offset);
    static bool yawCamera(Camera& camera,float degrees);
    static bool orbitCamera(Camera& camera,const AxisLine& axis,float degrees);
    static bool zoomCamera(Camera& camera,float factor);
    static std::optional<JointRig> addWalkingRig(Scene& scene,const GridLayout& bodyGrid);
    static bool poseWalkingRig(Scene& scene,const JointRig& rig,const glm::vec3& feet,float yawDegrees,
                               double walkDistance,float speed);
};
} // namespace graphics
#include "../detail/Patterns.inl"
