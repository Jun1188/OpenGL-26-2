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

    std::filesystem::path shaderDirectory;                // Empty selects embedded shaders.
    std::filesystem::path projectRoot = ".";              // Relative to the executable.
    std::filesystem::path runtimeDirectory = "framework"; // Relative to projectRoot.

    FeatureOptions features;
    bool verticalSync = true;
    DiagnosticOptions diagnostics;
    RenderConfig renderConfig;
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
    DWORD contextThread_ = 0;
    bool contextFailed_ = false;

    void* nativeContext_ = nullptr;

    bool ensureContext(const char* operation, const char* file, int line);
    Scene scene_;
    Camera camera_;
    Renderer renderer_;
    RenderOptions renderOptions_;
    std::optional<AssetLoader> assets_;
    std::exception_ptr callbackError_;
    std::function<void(Application&, const MouseEvent&)> mouseHandler_;
    std::function<void(Application&, double, double)> cursorHandler_;

    template <class Handler> void dispatch(Handler&& handler) noexcept {
        if (callbackError_)
            return;

        try {
            handler();
        } catch (...) {
            callbackError_ = std::current_exception();
            requestClose();
        }
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
    /// [API] 초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.
    Application() = default;
    /// [API] 실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.
    ~Application();
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application(const Application&) = delete;
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application& operator=(const Application&) = delete;
    /// [API] 장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.
    void setInitHandler(std::function<bool(Application&)> handler);
    /// [API] 매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.
    void setUpdateHandler(std::function<void(Application&, float)> handler);
    /// [API] 키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);
    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    /// [API] 키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.
    void setModifiedKeyHandler(std::function<void(Application&, int key, int action, int mods)> handler);
    /// [API] 창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);
    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    /// [API] 사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);
    /// [API] 마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.
    void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);
    /// [API] 창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.
    void setCursorHandler(std::function<void(Application&, double x, double y)> handler);
    /// [API] 창 좌표를 framebuffer 좌표로 보정하고 기본 카메라의 전체 viewport로 ray picking을 수행한다.
    std::optional<PickResult> pickAt(double windowX, double windowY) const;
    /// [API] 현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.
    std::optional<PickResult> pickAtCursor() const;
    /// [API] 실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.
    AssetLoader& getAssets(); // Available from initialization callback until run() ends.
    /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.
    int run(const ApplicationConfig& config = ApplicationConfig{});
    /// [API] 현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.
    bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {},
                           const std::filesystem::path& fragmentPath = {});
    /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
    void requestClose();
    /// [API] 기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.
    bool renderScene();
    /// [API] 현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.
    bool renderViews(const std::vector<RenderPass>& passes);
    /// [API] 현재 창의 실제 픽셀 크기를 반환한다. DPI 배율이 있으면 창 좌표 크기와 다를 수 있다.
    std::array<int, 2> getFramebufferSize() const;

    /// [API] 애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.
    RenderOptions& getRenderOptions() {
        return renderOptions_;
    }

    /// [API] 애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.
    Scene& getScene() {
        return scene_;
    }

    /// [API] 기본 카메라의 수정 가능한 참조를 반환한다.
    Camera& getCamera() {
        return camera_;
    }

    /// [API] GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.
    GLFWwindow* getWindow() const {
        return window_;
    }
};

} // namespace graphics
