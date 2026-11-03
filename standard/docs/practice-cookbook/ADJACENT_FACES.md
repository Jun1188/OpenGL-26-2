# 이웃한 면의 공유 모서리 회전과 자리 교환

[목차](README.md) · [전체 실습 15 코드](P15.md) · [공통 보조 코드](COMMON.md)

이 예제는 실습 15의 1/2 동작만 분리한 응용 코드다. 클래스 상태·초기화·입력·시간 진행·행렬 반영을 모두 제공한다.
다른 실습 파일을 include하지 않는다. 같은 standard 폴더의 PracticeCommon.h는 COMMON.md의 전문을 사용한다.
아래 세 파일을 기존 standard의 같은 이름 파일에 넣는다. 기존 Console과 동시에 정의하지 않는다.

## A01 상태 — Console.h

```cpp
#pragma once
#include "PracticeCommon.h"

class Console {
    struct FaceMotion {
        bool initialized = false, running = false;
        float degrees = 0;
        std::array<std::size_t, 2> pair{0, 3};
        glm::vec3 hinge{.4f, 0, .4f};
    };
    graphics::Application& app_;
    graphics::EntityId box_ = 0;
    std::array<FaceMotion, 2> effects_{}; // 0: 공유 모서리, 1: 자리 교환.
    std::mt19937 random_{std::random_device{}()};
    bool spinning_ = false;
    float bodyDegrees_ = 0;
    void toggle(std::size_t effect);
    void apply();
public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action);
    void onUpdate(float dt);
};
```

## A02 초기화·재입력 — Console.cpp 앞부분

면 순서는 앞 0, 뒤 1, 왼쪽 2, 오른쪽 3, 위 4, 아래 5다. 옆면의 순환 쌍과 공유 모서리 좌표를 같은 인덱스로 보관한다.
선택은 최초 시작 때 한 번만 한다. 같은 키를 다시 누르면 선택한 쌍을 유지하며 정지·재개한다.

```cpp
#include "Console.h"

bool Console::initScene() {
    auto& scene = app_.getScene();
    scene.clear();
    const auto box = scene.addCuboid({.8f, .8f, .8f}, {0, 0, 0});
    if (!box) return false;
    box_ = *box;
    effects_ = {};
    spinning_ = false;
    bodyDegrees_ = 0;
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {2.8f, 2, 5};
    app_.getCamera().target = {0, 0, 0};
    app_.getCamera().orthographic = true;
    app_.getCamera().orthographicHalfExtent = 1.3f;
    app_.getRenderOptions() = graphics::RenderOptions{};
    apply();
    return true;
}

void Console::toggle(std::size_t effect) {
    auto& motion = effects_[effect];
    if (motion.initialized) {
        motion.running = !motion.running;
        return;
    }
    const std::array<std::array<std::size_t, 2>, 4> pairs{{{0,3}, {3,1}, {1,2}, {2,0}}};
    const std::array<glm::vec3, 4> hinges{
        glm::vec3(.4f,0,.4f), glm::vec3(.4f,0,-.4f),
        glm::vec3(-.4f,0,-.4f), glm::vec3(-.4f,0,.4f)};
    const auto chosen = std::uniform_int_distribution<int>(0, 3)(random_);
    motion.pair = pairs[chosen];
    motion.hinge = hinges[chosen];
    motion.initialized = motion.running = true;
    std::cout << "선택 면: " << motion.pair[0]+1 << ", " << motion.pair[1]+1 << '\n';
}
```

## A03 두 패턴의 합성·반영 — Console.cpp에 이어 붙이기

매 프레임 여섯 항등 행렬에서 다시 계산한다. Hinge와 Swap이 같은 면을 제어하면 아래 코드의 Hinge→Swap 순서로 합성한다.
본체 회전은 별도 SRT다. 기본 계층 모드이므로 두 면 동작 위에 본체 자전이 함께 적용된다.

```cpp
void Console::apply() {
    auto& scene = app_.getScene();
    std::array<glm::mat4, 6> faces;
    faces.fill(glm::mat4(1));
    const auto compose = [&](std::size_t face, const glm::mat4& matrix) {
        faces[face] = matrix * faces[face];
    };
    const auto& hinge = effects_[0];
    if (hinge.initialized) {
        const auto rotation = practice_common::rotateAround(hinge.hinge, {0,1,0}, hinge.degrees);
        for (auto face : hinge.pair) compose(face, rotation);
    }
    const auto& swap = effects_[1];
    if (swap.initialized) {
        const float angle = swap.degrees <= 90 ? swap.degrees : 180-swap.degrees;
        compose(swap.pair[0], practice_common::rotateAround({0,0,0}, {0,1,0}, angle));
        compose(swap.pair[1], practice_common::rotateAround({0,0,0}, {0,1,0}, -angle));
    }
    for (std::size_t face=0; face<faces.size(); ++face)
        practice_common::require(scene.setFaceLocalMatrix(box_, face, faces[face]), "Face composition failed.");
    graphics::Transform3D body;
    body.rotation = practice_common::turn(bodyDegrees_, {0,1,0});
    practice_common::require(scene.setTransform(box_, body), "Body rotation failed.");
}

void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_1) toggle(0);
    if (key == GLFW_KEY_2) toggle(1);
    if (key == GLFW_KEY_Y) spinning_ = !spinning_;
    if (key == GLFW_KEY_R) practice_common::require(initScene(), "Reset failed.");
    if (key == GLFW_KEY_ESCAPE) app_.requestClose();
}

void Console::onUpdate(float dt) {
    if (!std::isfinite(dt) || dt < 0) throw std::runtime_error("Invalid frame delta.");
    if (spinning_) bodyDegrees_ = std::fmod(bodyDegrees_ + 45*dt, 360.0f);
    for (std::size_t i=0; i<effects_.size(); ++i)
        if (effects_[i].running)
            effects_[i].degrees = std::fmod(effects_[i].degrees + 45*dt, i == 0 ? 360.0f : 180.0f);
    apply();
}
```

## A04 연결 — main.cpp

```cpp
#include "Console.h"
int main() {
    graphics::Application app;
    Console pattern(app);
    app.setInitHandler([&](graphics::Application&) { return pattern.initScene(); });
    app.setKeyHandler([&](graphics::Application&, int key, int action) { pattern.onKey(key, action); });
    app.setUpdateHandler([&](graphics::Application&, float dt) { pattern.onUpdate(dt); });
    graphics::ApplicationConfig config;
    config.title = "standard - adjacent face hinge and swap";
    return app.run(config);
}
```

## 확인과 확장

사용자가 빌드 후 1/2/Y/R과 재입력을 확인한다. 교환은 ±90도에서 자리를 바꾸고 0도로 복귀한다. 모서리 회전은 같은 공유 선을 고정한다.
단독 동작을 먼저 확인한 뒤 두 동작을 함께 켠다. 서로 다른 랜덤 쌍을 동시에 선택하면 보이는 겹침은 합성 결과다.
한 동작만 필요하면 해당 effects 항목·키·합성 부분을 선택한다. 추가 동작은 같은 faces 배열에 합성한 뒤 한 번 반영한다.
실제 standard 파일은 이 문서 제작에서 바꾸지 않았고 빌드·테스트·화면 확인도 하지 않았다.
