#include <glew.h>
#include <glfw3.h>

#include "Console.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <initializer_list>

namespace {

constexpr const char* WINDOW_TITLE = "OpenGL Practice 13";

struct Controls {
    enum class Target { Cuboid, Tetrahedron, Camera };
    Target target = Target::Cuboid;
    int sizeAxis = 0;
    float moveSpeed = 1.0f;
    float resizeSpeed = 0.6f;
    std::array<bool, GLFW_KEY_LAST + 1> pressed{};
};

// 이 두 함수는 다른 .cpp로 옮겨도 같은 콜백 API로 등록할 수 있다.
void onKey(graphics::Application& app, int key, int action, void* userData) {
    Controls& controls = *static_cast<Controls*>(userData);
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            controls.pressed[key] = true;
        } else if (action == GLFW_RELEASE) {
            controls.pressed[key] = false;
        }
    }
    if (action != GLFW_PRESS) {
        return;
    }
    if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) {
        app.requestClose();
    }
    switch (key) {
    case GLFW_KEY_1: controls.target = Controls::Target::Cuboid; break;
    case GLFW_KEY_2: controls.target = Controls::Target::Tetrahedron; break;
    case GLFW_KEY_3: controls.target = Controls::Target::Camera; break;
    case GLFW_KEY_X: controls.sizeAxis = 0; break;
    case GLFW_KEY_Y: controls.sizeAxis = 1; break;
    case GLFW_KEY_Z: controls.sizeAxis = 2; break;
    default: break;
    }
}

void onUpdate(Console& console, float deltaTime, void* userData) {
    const Controls& controls = *static_cast<Controls*>(userData);
    const auto& key = controls.pressed;
    glm::vec3 direction{
        static_cast<float>(key[GLFW_KEY_RIGHT]) - static_cast<float>(key[GLFW_KEY_LEFT]),
        static_cast<float>(key[GLFW_KEY_UP]) - static_cast<float>(key[GLFW_KEY_DOWN]),
        static_cast<float>(key[GLFW_KEY_PAGE_UP]) - static_cast<float>(key[GLFW_KEY_PAGE_DOWN])
    };
    const bool moving = glm::dot(direction, direction) > 0.0f;
    const glm::vec3 movement = moving
        ? glm::normalize(direction) * controls.moveSpeed * deltaTime : glm::vec3(0.0f);

    if (controls.target == Controls::Target::Camera) {
        // 카메라만 이동하고 target은 유지해 입체를 다른 방향에서 볼 수 있다.
        if (moving) {
            graphics::Camera& camera = console.getCamera();
            graphics::Camera candidate = camera;
            candidate.position += movement;
            if (graphics::CameraSystem::validCamera(candidate)) {
                camera = candidate;
            }
        }
        return;
    }

    const auto id = controls.target == Controls::Target::Cuboid
        ? console.getCuboidId() : console.getTetrahedronId();
    if (!id) {
        return;
    }
    graphics::Scene& scene = console.getScene();
    const graphics::Component_Poly* component = scene.getComponent(*id);
    if (component == nullptr) {
        return;
    }
    if (moving) {
        scene.setPosition(*id, component->transform.basis + movement);
    }

    const float sizeDirection =
        static_cast<float>(key[GLFW_KEY_EQUAL] || key[GLFW_KEY_KP_ADD])
        - static_cast<float>(key[GLFW_KEY_MINUS] || key[GLFW_KEY_KP_SUBTRACT]);
    if (sizeDirection != 0.0f) {
        glm::vec3 lengths = component->sideLengths;
        const int axis = controls.sizeAxis;
        lengths[axis] = std::max(0.05f,
            lengths[axis] + sizeDirection * controls.resizeSpeed * deltaTime);
        if (lengths[axis] != component->sideLengths[axis]) {
            scene.setSideLengths(*id, lengths);
        }
    }
}

std::filesystem::path findShaderDirectory() {
    std::filesystem::path directory = std::filesystem::current_path();
    for (int depth = 0; depth < 6; ++depth) {
        const auto local = directory / "framework" / "shaders";
        const auto project = directory / "openGL_3dAssign_13" / "framework" / "shaders";
        for (const auto& candidate : { local, project }) {
            if (std::filesystem::exists(candidate / "VertexShader.glsl")
                && std::filesystem::exists(candidate / "FragmentShader.glsl")) {
                return candidate;
            }
        }
        if (directory == directory.parent_path()) {
            break;
        }
        directory = directory.parent_path();
    }
    return {};
}

} // namespace

int main() {
    graphics::Application application;
    Console example(application);
    Controls controls;
    graphics::ApplicationConfig config;
    config.title = WINDOW_TITLE;
    config.shaderDirectory = findShaderDirectory();
    if (config.shaderDirectory.empty()) {
        std::cerr << "framework/shaders was not found.\n";
        return -1;
    }

    // 장면과 키 명세는 사용자 코드에서 등록한다.
    application.setInitHandler([&](graphics::Application&) {
        return example.initScene();
    });
    application.setKeyHandler([&](graphics::Application& app, int key, int action) {
        onKey(app, key, action, &controls);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        onUpdate(example, deltaTime, &controls);
    });
    application.setFocusHandler([&](graphics::Application&, bool focused) {
        if (!focused) {
            controls.pressed.fill(false);
        }
    });
    std::cout << "1: cuboid, 2: right tetrahedron, 3: camera\n"
              << "Arrows: world X/Y, PageUp/PageDown: world Z\n"
              << "X/Y/Z: resize axis, +/-: resize, Esc/Q: quit\n";
    return application.run(config);
}
