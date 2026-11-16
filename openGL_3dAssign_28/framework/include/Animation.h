#pragma once
#include "Patterns.h"

namespace graphics {
struct JointAnimation {
    EntityId joint = 0;
    Transform3D rest;
    Motion motion;
};

struct HumanoidConfig {
    GridLayout grid{glm::vec3(-.5f, 0, -.3f), glm::vec3(1, 1.8f, .6f)};
    float upperArmFraction = .5f, upperLegFraction = .5f;
    GlColor torsoColor{.65f, .65f, .7f}, headColor{1, .8f, .5f};
    GlColor leftColor{.2f, .6f, 1}, rightColor{1, .4f, .2f};
};

struct HumanoidRig {
    EntityId root = 0, pelvis = 0, torso = 0, neck = 0;
    EntityId torsoMesh = 0, head = 0, nose = 0;
    std::array<EntityId, 2> shoulders{}, elbows{}, hands{}, hips{}, knees{}, feet{};
    std::array<EntityId, 2> upperArms{}, forearms{}, thighs{}, shins{};
};

struct HumanoidPose {
    glm::vec3 pelvisDegrees{0}, torsoDegrees{0}, neckDegrees{0};
    std::array<glm::vec3, 2> shoulders{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> elbows{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> hips{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> knees{glm::vec3(0), glm::vec3(0)};


    /// [API] 허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.
    static HumanoidPose seated();
};

struct SwingConfig {
    glm::vec3 pivot{0, 3, 0}, seatSize{1.4f, .12f, .7f};
    float length = 2, ropeThickness = .035f;
    PendulumMotion pendulum;
    HumanoidConfig person;
    double riderSwayDegrees = 8;
};

struct SwingRig {
    EntityId frame = 0, hinge = 0, seat = 0;
    std::array<EntityId, 2> ropes{};
    HumanoidRig person;
    std::vector<JointAnimation> animation;
};

class AnimationSystem {
public:
    /// [API] 관절의 현재 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율만 허용한다.
    static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);


    /// [API] 기준 SRT·피벗과 강체 모션을 합성해 모두 유효할 때 반영한다. 부모 계층과 균일 배율을 유지한다.
    static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);


    /// [API] 실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.
    static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{},
                                                  std::optional<EntityId> parent = std::nullopt);


    /// [API] 관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.
    static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);


    /// [API] 고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.
    static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{},
                                                     std::optional<EntityId> parent = std::nullopt);
};
} // namespace graphics
