#pragma once

#include "Graphics.h"
#include <functional>

struct GLFWwindow;

namespace graphics {

struct ApplicationConfig {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    std::string title = "OpenGL";
    std::filesystem::path shaderDirectory = "framework/shaders";
    bool verticalSync = true;
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
    static std::filesystem::path findShaderDirectory();
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);
    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);
    int run(const ApplicationConfig& config = {});
    void requestClose();
    bool renderScene();

    RenderOptions& getRenderOptions() { return renderOptions_; }
    Scene& getScene() { return scene_; }
    Camera& getCamera() { return camera_; }
    GLFWwindow* getWindow() const { return window_; }
};

} // namespace graphics
