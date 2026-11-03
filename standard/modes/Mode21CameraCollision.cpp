#include "PracticeMode.h"
#include "PracticeCommon.h"


namespace {
class CameraCollision final : public PracticeMode {
    graphics::Application& app_;
    std::array<bool, GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 minimum_{-.6f}, maximum_{.6f};
    bool blocked(const glm::vec3& position) const {
        const auto closest = glm::clamp(position,minimum_,maximum_);
        const auto difference = position-closest;
        return glm::dot(difference,difference) <= .15f*.15f;
    }
public:
    explicit CameraCollision(graphics::Application& app) : PracticeMode(app), app_(app) {}
    bool initScene() override {
        pressed_.fill(false);
        auto& scene = app_.getScene(); scene.clear();
        const auto object = scene.addCuboid({1.2f,1.2f,1.2f},{0,0,0});
        if (!object || !scene.setTransform(*object,graphics::Transform3D{})) return false;
        app_.getCamera() = graphics::Camera{};
        app_.getCamera().position = {0,0,3};
        app_.getCamera().target = {0,0,0};
        return true;
    }
    void onKey(int key, int action, int) override {
        if (key>=0 && key<=GLFW_KEY_LAST) pressed_[key] = action != GLFW_RELEASE;
        if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) app_.requestClose();
    }
    void onFocus(bool focused) override { if (!focused) pressed_.fill(false); }
    void onUpdate(float dt) override {
        if (!std::isfinite(dt) || dt<0) throw std::runtime_error("Invalid dt.");
        auto& camera = app_.getCamera();
        const auto forward = glm::normalize(camera.target-camera.position);
        const auto right = glm::normalize(glm::cross(forward,camera.up));
        glm::vec3 direction(0);
        if (pressed_[GLFW_KEY_W]) direction += forward;
        if (pressed_[GLFW_KEY_S]) direction -= forward;
        if (pressed_[GLFW_KEY_A]) direction -= right;
        if (pressed_[GLFW_KEY_D]) direction += right;
        if (glm::dot(direction,direction)==0) return;
        const auto movement = glm::normalize(direction)*2.0f*dt;
        const int steps = std::max(1,int(std::ceil(glm::length(movement)/(.15f*.5f))));
        const auto step = movement/float(steps);
        for (int i=0; i<steps; ++i) {
            if (blocked(camera.position+step)) break;
            camera.position += step;
            camera.target += step;
        }
    }
};
}

std::unique_ptr<PracticeMode> makeCameraCollision(graphics::Application& app) {
    return std::make_unique<CameraCollision>(app);
}
