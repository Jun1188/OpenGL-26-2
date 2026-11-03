#include "Assembly.h"
#include <algorithm>

namespace assembly {
void Assembly::queueFeatureEvent(FeatureInstance& feature, FeatureEvent event) {
    if (feature.events.size() >= 256) {
        feature.events.clear();
        FeatureEvent release; release.kind = FeatureEvent::Kind::Focus; release.focused = false;
        feature.events.push_back(release);
        return;
    }
    feature.events.push_back(std::move(event));
}
void Assembly::queueFeatureFocus(bool value) {
    for (auto& feature : state_.features) if (feature.name == state_.featureFocus) {
        FeatureEvent event; event.kind = FeatureEvent::Kind::Focus; event.focused = value;
        queueFeatureEvent(feature, event);
    }
}
bool Assembly::routeFeatureKey(int key, int action, int mods, std::ostream&) {
    for (auto& f : state_.features) if (f.name == state_.featureFocus && f.enabled) {
        if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) return true;
        if (key == GLFW_KEY_V && f.program->mode() <= 9) {
            if (action == GLFW_PRESS) state_.threeViews = !state_.threeViews;
            return true;
        }
        FeatureEvent event; event.kind = FeatureEvent::Kind::Key;
        event.key = key; event.action = action; event.mods = mods;
        queueFeatureEvent(f, event);
        return true;
    }
    return false;
}
bool Assembly::routeFeatureMouse(const graphics::MouseEvent& original, std::ostream& out) {
    for (auto& f : state_.features) if (f.name == state_.featureFocus && f.enabled) {
        try {
            auto camera = app_.getCamera();
            auto event = original;
            const auto size = app_.getFramebufferSize();
            if (size[0] <= 0 || size[1] <= 0) return true;
            if (state_.threeViews && f.program->mode() <= 9) {
                for (const auto& pass : graphics::PatternSystem::threeViews(camera, size[0], size[1], app_.getRenderOptions())) {
                    const auto& v = pass.viewport;
                    const double top = size[1] - v.y - v.height;
                    if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top && event.framebufferY < top + v.height) {
                        camera = pass.camera;
                        event.framebufferX = (event.framebufferX - v.x) * size[0] / v.width;
                        event.framebufferY = (event.framebufferY - top) * size[1] / v.height;
                        int width = 0, height = 0;
                        graphics::detail::glfw().GetWindowSize(app_.getWindow(), &width, &height);
                        event.windowX = event.framebufferX * width / size[0];
                        event.windowY = event.framebufferY * height / size[1];
                        break;
                    }
                }
            }
            const auto world = app_.getScene().worldMatrix(node(f.name).id);
            require(world.has_value(), "입력 컨테이너 행렬이 없습니다.");
            const auto scale = glm::length(glm::vec3((*world)[0]));
            require(scale > .0001f && rigid(*world * glm::scale(glm::mat4(1), glm::vec3(1 / scale))),
                    "실습 마우스 입력은 양수 균일 부모 SRT를 요구합니다.");
            const auto inverse = glm::inverse(*world);
            camera.position = glm::vec3(inverse * glm::vec4(camera.position, 1));
            camera.target = glm::vec3(inverse * glm::vec4(camera.target, 1));
            camera.up = glm::normalize(glm::mat3(inverse) * camera.up);
            camera.nearPlane /= scale; camera.farPlane /= scale; camera.orthographicHalfExtent /= scale;
            FeatureEvent pending; pending.kind = FeatureEvent::Kind::Mouse; pending.mouse = event; pending.view = camera;
            queueFeatureEvent(f, pending);
        } catch (const std::exception& error) { out << "기능 입력 거절: " << error.what() << '\n'; }
        return true;
    }
    return false;
}
bool Assembly::routeFeatureCursor(double x, double y) {
    for (auto& f : state_.features) if (f.name == state_.featureFocus && f.enabled) {
        FeatureEvent event; event.kind = FeatureEvent::Kind::Cursor; event.x = x; event.y = y;
        queueFeatureEvent(f, event); return true;
    }
    return false;
}
} // namespace assembly
