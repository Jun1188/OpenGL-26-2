#include "FeatureContainer.h"
#include "Assembly.h"
#include "../Console.h"

namespace assembly {
struct FeatureContainer::Impl {
    graphics::Application& host;
    int mode;
    graphics::Scene scene;
    graphics::Camera camera;
    graphics::RenderOptions options;
    Console controller;

    struct Context {
        Impl& owner;
        graphics::Scene saved;
        graphics::Camera camera;
        graphics::RenderOptions options;
        bool closing = false;
        explicit Context(Impl& value) : owner(value), saved(std::move(value.host.getScene())),
            camera(value.host.getCamera()), options(value.host.getRenderOptions()) {
            closing = owner.host.getWindow() && graphics::detail::glfw().WindowShouldClose(owner.host.getWindow());
            owner.host.getScene() = std::move(owner.scene);
            owner.host.getCamera() = owner.camera;
            owner.host.getRenderOptions() = owner.options;
        }
        void check() const {
            require(closing || !owner.host.getWindow() || !graphics::detail::glfw().WindowShouldClose(owner.host.getWindow()),
                "실습 컨테이너가 실패로 종료를 요청했습니다. guide runtime 또는 feature reset을 사용하세요.");
        }
        ~Context() {
            if (owner.host.getWindow() && !closing) graphics::detail::glfw().SetWindowShouldClose(owner.host.getWindow(), GLFW_FALSE);
            owner.scene = std::move(owner.host.getScene());
            owner.camera = owner.host.getCamera();
            owner.options = owner.host.getRenderOptions();
            owner.host.getScene() = std::move(saved);
            owner.host.getCamera() = camera;
            owner.host.getRenderOptions() = options;
        }
    };
    Impl(graphics::Application& app, int number) : host(app), mode(number), controller(app, number) {
        require(mode >= 1 && mode <= lastStandardMode, "feature 모드는 1~22입니다. 0번을 재귀 포함할 수 없습니다.");
        Context context(*this);
        require(controller.initScene(), "실습 컨테이너 초기화 실패: " + std::to_string(mode));
        context.check();
    }
};
FeatureContainer::FeatureContainer(graphics::Application& host, int mode) : impl_(std::make_unique<Impl>(host, mode)) {}
FeatureContainer::~FeatureContainer() = default;
int FeatureContainer::mode() const { return impl_->mode; }
const graphics::Scene& FeatureContainer::scene() const { return impl_->scene; }
const graphics::Camera& FeatureContainer::camera() const { return impl_->camera; }
const graphics::RenderOptions& FeatureContainer::options() const { return impl_->options; }
void FeatureContainer::step(float dt) {
    Impl::Context context(*impl_);
    impl_->controller.onUpdate(dt);
    require(impl_->host.getScene().updateMotions(dt), "실습 컨테이너 Motion 갱신 실패.");
    context.check();
}
void FeatureContainer::key(int key, int action, int mods) {
    require(key != GLFW_KEY_Q && key != GLFW_KEY_ESCAPE, "Q/Esc는 컨테이너에 전달하지 않습니다. feature enable 또는 focus none을 사용하세요.");
    Impl::Context context(*impl_);
    impl_->controller.onKey(key, action, mods);
    context.check();
}
void FeatureContainer::mouse(const graphics::MouseEvent& event, const graphics::Camera& view) {
    Impl::Context context(*impl_);
    const auto saved = impl_->host.getCamera();
    impl_->host.getCamera() = view;
    try { impl_->controller.onMouse(event); }
    catch (...) { impl_->host.getCamera() = saved; throw; }
    impl_->host.getCamera() = saved;
    context.check();
}
void FeatureContainer::cursor(double x, double y) {
    Impl::Context context(*impl_); impl_->controller.onCursor(x, y); context.check();
}
void FeatureContainer::pick(std::optional<graphics::PickResult> hit) {
    Impl::Context context(*impl_); impl_->controller.selectFromContainer(hit); context.check();
}
void FeatureContainer::focus(bool value) {
    Impl::Context context(*impl_); impl_->controller.onFocus(value); context.check();
}
} // namespace assembly
