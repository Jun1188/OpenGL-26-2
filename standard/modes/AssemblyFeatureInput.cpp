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
            if (f.program->mode() <= 9 && original.button == GLFW_MOUSE_BUTTON_LEFT && original.action == GLFW_PRESS) {
                std::optional<graphics::PickResult> hit;
                const auto size = app_.getFramebufferSize();
                if (state_.threeViews && size[0] >= 3 && size[1] >= 2) {
                    for (const auto& pass : graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions())) {
                        const auto& v = pass.viewport;
                        const double top = size[1] - v.y - v.height;
                        if (original.framebufferX >= v.x && original.framebufferX < v.x + v.width && original.framebufferY >= top && original.framebufferY < top + v.height)
                            hit = graphics::pick(app_.getScene(), pass.camera, original.framebufferX, original.framebufferY,
                                {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
                    }
                } else hit = app_.pickAt(original.windowX, original.windowY);
                FeatureEvent pending; pending.kind = FeatureEvent::Kind::Pick;
                if (hit) for (const auto& instance : state_.features) if (instance.program == f.program && instance.enabled)
                    for (const auto& entity : instance.entities) if (entity.second.mesh == hit->entityId) {
                        pending.hit = *hit; pending.hit->entityId = entity.first;
                    }
                queueFeatureEvent(f, pending);
                return true;
            }
            FeatureEvent pending; pending.kind = FeatureEvent::Kind::Mouse;
            pending.mouse = original; pending.view = f.program->camera();
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
