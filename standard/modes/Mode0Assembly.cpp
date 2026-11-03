#include "Assembly.h"
#include "AssemblyConsole.h"
#include <iostream>

namespace {
class AssemblyMode final : public PracticeMode {
    graphics::Application& app_;
    assembly::Assembly scene_;
    assembly::ConsoleInput input_;
public:
    explicit AssemblyMode(graphics::Application& app) : PracticeMode(app), app_(app), scene_(app) {}
    bool initScene() override {
        if (!assembly::defaultRecipe(scene_, std::cout))
            std::cout << "기본 레시피가 중단됐습니다. 콘솔에서 수정하거나 guide recipe를 사용하세요.\n";
        scene_.help(std::cout);
        input_.prompt();
        return true;
    }
    void onKey(int key, int action, int mods) override {
        if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
            assembly::defaultRecipe(scene_, std::cout);
            return;
        }
        scene_.onKey(key, action, mods, std::cout);
    }
    void onMouse(const graphics::MouseEvent& event) override { scene_.onMouse(event, std::cout); }
    void onCursor(double x, double y) override { scene_.onCursor(x, y, std::cout); }
    void onFocus(bool focused) override { scene_.onFocus(focused); }
    void onUpdate(float dt) override {
        for (unsigned i = 0; i < 8; ++i) {
            const auto line = input_.poll();
            if (!line) break;
            if (*line == "quit") {
                app_.requestClose();
                return;
            }
            scene_.execute(*line, std::cout);
            input_.prompt();
        }
        scene_.update(dt, std::cout);
    }
    bool onRender() override { return scene_.render(); }
};
}

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app) {
    return std::make_unique<AssemblyMode>(app);
}
