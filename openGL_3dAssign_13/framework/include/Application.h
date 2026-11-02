#pragma once

#include "Graphics.h"
#include "ModelLoader.h"
#include "Picking.h"
#include <functional>
#include <exception>

struct GLFWwindow;

namespace graphics {

struct ApplicationConfig {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    std::string title = "OpenGL";
    std::filesystem::path shaderDirectory; // Empty selects embedded shaders.
    std::filesystem::path projectRoot = "."; // Relative to the executable.
    std::filesystem::path runtimeDirectory = "framework"; // Relative to projectRoot.
    FeatureOptions features;
    bool verticalSync = true;
};

struct MouseEvent {
    int button = 0, action = 0, mods = 0;
    double windowX = 0, windowY = 0, framebufferX = 0, framebufferY = 0;
};

// 콜백에 캡처한 객체는 run()이 끝날 때까지 유지한다.
// 한 스레드에서 Application 하나를 사용한다. GLFW 초기화와 종료를 소유한다.
class Application {
private:
    GLFWwindow* window_ = nullptr;
    bool glfwInitialized_ = false;
    bool running_ = false;
    Scene scene_;
    Camera camera_;
    Renderer renderer_;
    RenderOptions renderOptions_;
    std::optional<AssetLoader> assets_;
    std::exception_ptr callbackError_;
    std::function<void(Application&, const MouseEvent&)> mouseHandler_;
    std::function<void(Application&, double, double)> cursorHandler_;
    template<class Handler> void dispatch(Handler&& handler) noexcept {
        if(callbackError_) return;
        try { handler(); } catch(...) { callbackError_=std::current_exception(); requestClose(); }
    }
    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
    std::function<void(Application&, int, int)> keyHandler_;
    std::function<void(Application&, int, int, int)> modifiedKeyHandler_;
    std::function<void(Application&, bool)> focusHandler_;
    std::function<bool(Application&)> renderHandler_;

    bool initialize(const ApplicationConfig& config);
    void shutdown();

public:
    Application() = default;
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void setInitHandler(std::function<bool(Application&)> handler);
    void setUpdateHandler(std::function<void(Application&, float)> handler);
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);
    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    void setModifiedKeyHandler(
        std::function<void(Application&, int key, int action, int mods)> handler);
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);
    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);
    void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);
    void setCursorHandler(std::function<void(Application&, double x, double y)> handler);
    std::optional<PickResult> pickAt(double windowX, double windowY) const;
    std::optional<PickResult> pickAtCursor() const;
    AssetLoader& getAssets(); // Available from initialization callback until run() ends.
    int run(const ApplicationConfig& config = ApplicationConfig{});
    void requestClose();
    bool renderScene();

    RenderOptions& getRenderOptions() { return renderOptions_; }
    Scene& getScene() { return scene_; }
    Camera& getCamera() { return camera_; }
    GLFWwindow* getWindow() const { return window_; }
};

} // namespace graphics

