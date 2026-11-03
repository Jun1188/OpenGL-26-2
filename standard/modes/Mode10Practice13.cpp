#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <initializer_list>
#include <random>
#include <iostream>

namespace {
// 실습 13: 표시할 면과 난수 상태만 관리한다.
class Practice13 final : public PracticeMode {
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    std::mt19937 random_{std::random_device{}()};

    void selectFaces(graphics::EntityId id, std::initializer_list<std::size_t> faces);

public:
    explicit Practice13(graphics::Application& application) : PracticeMode(application), application_(application) {
    }

    bool initScene() override;

    void onKey(int key, int action, int) override;
};


bool Practice13::initScene() {
    graphics::Scene& scene = application_.getScene();

    scene.clear();

    const auto cube = scene.addCuboid(glm::vec3(0.8f), glm::vec3(0.0f));
    const auto pyramid = scene.addPyramid(glm::vec3(0.8f), glm::vec3(0.0f));

    if (!cube || !pyramid) {
        return false;
    }
    cuboidId_ = *cube;
    pyramidId_ = *pyramid;

    graphics::Transform3D initial;

    initial.rotation = glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0}));

    if (!scene.setTransform(cuboidId_, initial) || !scene.setTransform(pyramidId_, initial)) {
        return false;
    }
    // 축은 객체에 연결하지 않는다. 원점부터 양·음 방향으로 각각 1만큼 표시한다.
    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();

    camera.position = {1.8f, 1.2f, 3.0f};
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    selectFaces(cuboidId_, {0, 1, 2, 3, 4, 5});
    std::cout << "실습 13: 키 설명==면(색)\n"
              << "1==front(red), 2==back(green), 3==left(blue), 4==right(yellow)\n"
              << "5 top(cyan), 6 bottom(magenta)\n"
              << "7/8/9/0는 pyramid == front(red)/right(blue)/back(green)/left(yellow)\n"
              << "C: 랜덤한 cube 2개 faces, T: pyramid 밑변 + 랜덤한 1개 변\n"
              << "Axes: X red, Y green, Z blue. Esc: quit\n";

    return true;
}

void Practice13::selectFaces(graphics::EntityId id, std::initializer_list<std::size_t> faces) {
    auto& scene = application_.getScene();

    for (auto entity : scene.getIds()) {
        const auto* component = scene.getComponent(entity);

        for (std::size_t i = 0; i < component->polyhedron.faces.size(); ++i) {
            scene.setFaceVisible(entity, i, false);
        }
    }

    for (std::size_t face : faces) {
        scene.setFaceVisible(id, face, true);
    }
}

void Practice13::onKey(int key, int action, int) {
    if (action != GLFW_PRESS) {
        return; // 키 반복으로 무작위 선택을 여러 번 바꾸지 않는다.
    }

    if (key == GLFW_KEY_ESCAPE) {
        application_.requestClose();
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        selectFaces(cuboidId_, {static_cast<std::size_t>(key - GLFW_KEY_1)});
    } else if (key >= GLFW_KEY_7 && key <= GLFW_KEY_9) {
        selectFaces(pyramidId_, {static_cast<std::size_t>(key - GLFW_KEY_7)});
    } else if (key == GLFW_KEY_0) {
        selectFaces(pyramidId_, {3});
    } else if (key == GLFW_KEY_C) {
        const std::size_t first = std::uniform_int_distribution<int>(0, 5)(random_);
        std::size_t second = std::uniform_int_distribution<int>(0, 4)(random_);

        if (second >= first) {
            ++second; // 첫 면을 제외한 다섯 면 중 하나다.
        }
        selectFaces(cuboidId_, {first, second});
    } else if (key == GLFW_KEY_T) {
        const std::size_t side = std::uniform_int_distribution<int>(0, 3)(random_);

        selectFaces(pyramidId_, {4, side});
    }
}
}

std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app) {
    return std::make_unique<Practice13>(app);
}
