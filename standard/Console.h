#pragma once
#include "framework/Framework.h"
#include "ExamController.h"
class Console {
    graphics::Application& app_;
    graphics::SceneAuthoring scene_;
    ExamController controller_;
    double seconds_ = 0;
    bool paused_ = false;
public:
    explicit Console(graphics::Application& app) : app_(app), scene_(app), controller_(app, scene_) {}
    bool initScene();
    void onKey(int key, int action, int mods = 0);
    void onMouse(const graphics::MouseEvent& event);
    void onFocus(bool focused);
    void onUpdate(float dt);
    bool onRender();
    bool bindUniforms(const graphics::DrawUniformContext& context) const { return controller_.bindUniforms(context); }
};
