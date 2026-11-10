# 과거 구현 참조: Editor.cpp

- 원래 위치: `standard/modes/Editor.cpp`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#include "../Console.h"
#include <iostream>

bool Console::addControl(graphics::EntityId id, const std::string& name, ControlKind kind) {
    std::size_t index = 0;
    while (index < animation_.size() && animation_[index].joint != id)
        ++index;
    if (index == animation_.size()) {
        const auto track = graphics::AnimationSystem::createJointAnimation(app_.getScene(), id, graphics::Motion{});
        if (!track)
            return false;
        animation_.push_back(*track);
    }
    jointControls_.push_back({index, name, animation_[index].rest, kind});
    return true;
}

void Console::selectPicked(std::optional<graphics::PickResult> hit) {
    selectedJoint_ = jointControls_.size();
    if (hit) {
        std::optional<graphics::EntityId> entity = hit->entityId;
        while (entity) {
            for (std::size_t i = 0; i < jointControls_.size(); ++i) {
                const auto& control = jointControls_[i];
                if (animation_[control.track].joint == *entity && control.kind != ControlKind::Joint) {
                    selectedJoint_ = i;
                    printSelectedJoint();
                    return;
                }
            }
            const auto* component = app_.getScene().getComponent(*entity);
            entity = component ? component->parent : std::nullopt;
        }
    }
    std::cout << "선택 해제: 조작할 객체를 클릭하거나 Tab을 누르세요.\n";
}

bool Console::handleJointKey(int key) {
    if (jointControls_.empty())
        return false;
    if (key == GLFW_KEY_TAB) {
        selectedJoint_ = selectedJoint_ >= jointControls_.size() ? 0 : (selectedJoint_ + 1) % jointControls_.size();
    } else if (key == GLFW_KEY_BACKSPACE) {
        selectedJoint_ = selectedJoint_ >= jointControls_.size() ? jointControls_.size() - 1
            : (selectedJoint_ + jointControls_.size() - 1) % jointControls_.size();
    } else if (key == GLFW_KEY_X || key == GLFW_KEY_Y || key == GLFW_KEY_Z) {
        selectedAxis_ = key == GLFW_KEY_X ? 0 : key == GLFW_KEY_Y ? 1 : 2;
    } else {
        const bool edit = key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT || key == GLFW_KEY_UP ||
            key == GLFW_KEY_DOWN || key == GLFW_KEY_PAGE_UP || key == GLFW_KEY_PAGE_DOWN ||
            key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD || key == GLFW_KEY_MINUS ||
            key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_R;
        if (!edit)
            return false;
        if (selectedJoint_ >= jointControls_.size()) {
            std::cout << "편집 대상이 없습니다.\n";
            return true;
        }
        const auto& control = jointControls_[selectedJoint_];
        auto& rest = animation_[control.track].rest;
        const auto saved = rest;
        const auto savedBody = roomBody_;
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
            glm::vec3 axis(0);
            axis[selectedAxis_] = 1;
            rest.rotation *= glm::mat3(glm::rotate(glm::mat4(1),
                glm::radians(key == GLFW_KEY_LEFT ? -5.0f : 5.0f), axis));
            // Repeated edits must retain an orthonormal rotation basis.
            const auto x = glm::normalize(rest.rotation[0]);
            const auto z = glm::normalize(glm::cross(x, rest.rotation[1]));
            rest.rotation = glm::mat3(x, glm::cross(z, x), z);
        } else if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
            rest.basis[selectedAxis_] += key == GLFW_KEY_UP ? .05f : -.05f;
        } else if (key == GLFW_KEY_PAGE_UP || key == GLFW_KEY_PAGE_DOWN) {
            if (control.kind == ControlKind::RoomBody) {
                std::cout << "충돌 상자는 중심 피벗을 유지합니다.\n";
                return true;
            }
            rest.localBasis[selectedAxis_] += key == GLFW_KEY_PAGE_UP ? .05f : -.05f;
        } else if (key == GLFW_KEY_R) {
            rest = control.initialRest;
        } else {
            if (control.kind == ControlKind::RoomFrame) {
                std::cout << "물리 방은 강체이므로 배율 1을 유지합니다.\n";
                return true;
            }
            const bool grow = key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD;
            rest.scale = glm::vec3(std::clamp(rest.scale.x * (grow ? 1.1f : 1 / 1.1f), .1f, 3.0f));
        }
        if (control.kind == ControlKind::RoomBody) {
            if (key == GLFW_KEY_R) {
                roomBody_ = graphics::RoomBody{};
                roomBody_.localPosition = {0, -.75f, 0};
            } else if (roomBody_.released) {
                roomBody_.worldPosition += releasedRotation_ * (rest.basis - saved.basis);
            } else {
                roomBody_.localPosition += rest.basis - saved.basis;
            }
            rest.basis = glm::vec3(0);
        }
        if (!applyControls()) {
            rest = saved;
            roomBody_ = savedBody;
            if (!applyControls())
                app_.requestClose();
            std::cerr << "편집을 거절하고 이전 상태를 복원했습니다.\n";
        }
    }
    printSelectedJoint();
    return true;
}

void Console::printSelectedJoint() const {
    if (selectedJoint_ >= jointControls_.size())
        return;
    const auto& control = jointControls_[selectedJoint_];
    const auto& rest = animation_[control.track].rest;
    std::cout << "mode=" << activeMode_ << " selected=" << selectedJoint_ + 1 << '/' << jointControls_.size()
        << ": " << control.name << " axis=" << "XYZ"[selectedAxis_]
        << " position=" << rest.basis.x << ',' << rest.basis.y << ',' << rest.basis.z
        << " pivot=" << rest.localBasis.x << ',' << rest.localBasis.y << ',' << rest.localBasis.z
        << " scale=" << rest.scale.x << '\n';
}
````
