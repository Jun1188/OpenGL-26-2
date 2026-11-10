#include "Assembly.h"
#include <algorithm>

namespace assembly {
bool Assembly::trackCommand(const Tokens& a, std::ostream&) {
    if (a[0] != "face-track") return false;
    require(a.size() >= 8, "face-track <ID> <객체> <면> <시간> <지연> <once/loop/pingpong> <move/scale/turn> ...");
    const auto duration = number(a[4]), delay = number(a[5]);
    require(duration >= .0001 && delay >= 0, ".0001초 이상 시간과 0 이상 지연이 필요합니다.");
    require(a[6] == "once" || a[6] == "loop" || a[6] == "pingpong", "재생은 once/loop/pingpong입니다.");
    const auto operation = a[7], playback = a[6];
    glm::vec3 value(0), pivot(0);
    double degrees = 0;
    if (operation == "move") {
        require(a.size() == 11, "face-track ... move <DX DY DZ>");
        value = vector(a, 8);
    } else if (operation == "scale") {
        require(a.size() == 14, "face-track ... scale <SX SY SZ> <CX CY CZ>");
        value = vector(a, 8); pivot = vector(a, 11);
        require(glm::all(glm::greaterThan(value, glm::vec3(.0001f))) && glm::all(glm::lessThanEqual(value, glm::vec3(100))),
                "애니메이션 배율은 양수 .0001~100입니다. 완전 개방은 visible off로 처리하세요.");
    } else {
        require(operation == "turn" && a.size() == 15, "face-track ... turn <축3> <중심3> <도>");
        value = vector(a, 8); pivot = vector(a, 11); degrees = number(a[14]);
        require(glm::length(value) > .0001f, "회전축은 0일 수 없습니다.");
        value = glm::normalize(value);
    }
    graphics::MotionStep step;
    step.startSeconds = delay;
    step.space = graphics::TransformSpace::World;
    step.transform = [=](double time) {
        double u = time / duration;
        if (playback == "once") u = std::min(u, 1.0);
        else if (playback == "loop") u = std::fmod(u, 1.0);
        else { u = std::fmod(u, 2.0); if (u > 1) u = 2 - u; }
        if (operation == "move") return glm::translate(glm::mat4(1), value * float(u));
        if (operation == "scale") return *graphics::TransformSystem::scaling(glm::mix(glm::vec3(1), value, float(u)), pivot);
        return *graphics::TransformSystem::rotation({pivot, value}, degrees * u);
    };
    Behavior behavior;
    behavior.name = a[1]; behavior.target = a[2]; behavior.faceIndex = indexNumber(a[3]);
    behavior.effect = Effect::FaceTrack;
    require(behavior.motion.append(step), "면 트랙 생성 실패.");
    attach(std::move(behavior));
    return true;
}
} // namespace assembly
