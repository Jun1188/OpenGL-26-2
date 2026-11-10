# 전체 변경 전후

2026-11-03. before는 작업 시작 HEAD eaa99b531a3c9191671495a30f9a65605fc1f20d의 원문이다. 아래 기존 파일은 시작 시 수정되지 않은 상태였음을 git status로 확인했다. 새 파일은 before 없음이다. 파일 전문을 짝지어 모든 변경 코드 블록을 포함한다. 문서와 생성된 그래프 데이터는 코드 전문 비교에서 제외한다.

1단계 badd1bd: 모드 파일 분리·고정 시작. 2단계 da2611e: 공통 입력·의미상 객체 선택·물리·피킹. 3단계 c451e57: 추가 실습·부록·공통 구현과 입력 보완. 4단계 bb44f05: 고정 모드 전체 복원·초기 선택. 5단계 cb244b5: 원본 실습의 예외 보고·실패 코드 보존. 마지막 단계는 문서와 구조 그래프다.

빌드·테스트·실행·검증기는 수행하지 않았다. 정적 소스 대조와 graphify AST는 실행 결과를 증명하지 않는다. 변경 이유와 예상 동작은 [입력 평가](../MODE_INPUT_AUDIT.md)와 [사용자 확인](../../USER_CHECKS.md)을 읽는다.

## framework/detail/Application.cpp

### before

```cpp
#include "../Framework.h"
#include "Internal.h"
#include <algorithm>
#include <iostream>
#include <utility>

namespace graphics::detail {
bool& applicationActive() {
    static bool active = false;

    return active;
}

unsigned& contextErrorSerial() {
    static unsigned value = 0;

    return value;
}
}

namespace graphics {

Application::~Application() {
    shutdown();
}

bool Application::configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath,
                                    const std::filesystem::path& fragmentPath) {
    if (!ensureContext("configureRenderer", __FILE__, __LINE__))
        return false;
    if (renderer_.initialize(vertexPath, fragmentPath, config))
        return true;
    requestClose();
    return false;
}

void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

void Application::setModifiedKeyHandler(std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::ensureContext(const char* operation, const char* file, int line) {
    Diagnostics::locate(file, line, operation);

    if (contextFailed_)
        return false;

    if (!window_ || GetCurrentThreadId() != contextThread_) {
        contextFailed_ = true;

        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::ensureContext",
            "창이 없거나 컨텍스트를 생성한 스레드와 다른 스레드에서 사용했습니다.",
            "창 생성·렌더링·종료를 같은 스레드에서 수행하세요. Application::run과 사용자 렌더 콜백을 확인하세요.");
    }
    using CurrentProc = void*(WINAPI*)();

    const auto module = GetModuleHandleW(L"opengl32.dll");
    const auto current =
        module ? reinterpret_cast<CurrentProc>(::GetProcAddress(module, "wglGetCurrentContext")) : nullptr;

    if (nativeContext_ && detail::glfw().GetCurrentContext() == window_ && current && current() == nativeContext_)
        return true;

    const auto serial = detail::contextErrorSerial();

    detail::glfw().MakeContextCurrent(window_);

    if (serial != detail::contextErrorSerial() || detail::glfw().GetCurrentContext() != window_ || !current ||
        !current()) {
        contextFailed_ = true;

        return Diagnostics::fail(
            PipelineStage::Runtime, "glfwMakeContextCurrent",
            "WGL이 요청한 컨텍스트를 연결하지 못하여 GL 명령 전에 렌더링을 중단했습니다.",
            "소스 위치는 연결 실패 호출입니다. 창 수명·렌더 스레드·컨텍스트를 해제/파괴하는 외부 콜백을 먼저 확인하세요. 이상이 없으면 GPU 드라이버·세션을 확인하세요. 이 로그만으로 드라이버 원인을 확정할 수 없습니다.");
    }
    nativeContext_ = current();

    return true;
}

bool Application::initialize(const ApplicationConfig& config) {
    contextThread_ = GetCurrentThreadId();
    contextFailed_ = false;
    nativeContext_ = nullptr;
    Diagnostics::trace(PipelineStage::Runtime, "Application::initialize");

    const auto root = detail::relativePathInside(detail::executableDirectory(), config.projectRoot);
    const auto runtime = detail::relativePathInside(root, config.runtimeDirectory);

    detail::glfw().load(runtime);
    detail::glfw().SetErrorCallback([](int code, const char* message) {
        ++detail::contextErrorSerial();
        Diagnostics::fail(
            PipelineStage::Runtime, "GLFW/WGL", std::to_string(code) + ": " + (message ? message : "알 수 없는 오류"),
            "표시된 소스 위치·호출 경로에서 컨텍스트 연결이 실패했습니다. Application의 창 수명과 스레드를 먼저 확인하세요. OS/드라이버 원문도 함께 보세요. 렌더 패스마다 별도 WGL 컨텍스트를 만들 필요는 없습니다.");
    });

    if (config.width <= 0 || config.height <= 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::initialize",
                                   "창 너비와 높이는 양수여야 합니다.");

    if (!detail::glfw().Init())
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwInit",
                                   "GLFW 초기화 실패: 앞선 GLFW 오류를 확인하세요.");
    glfwInitialized_ = true;
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    detail::glfw().WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    detail::glfw().WindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    detail::glfw().WindowHint(GLFW_DEPTH_BITS, 24);
    window_ = detail::glfw().createWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);

    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwCreateWindow",
                                   "OpenGL 3.3 코어 창 생성 실패.",
                                   "앞선 GLFW 오류와 그래픽 드라이버의 OpenGL 지원을 확인하세요.");
    }

    if (!ensureContext("initialize -> attach new window", __FILE__, __LINE__))
        return false;
    detail::glfw().SwapInterval(config.verticalSync ? 1 : 0);

    const auto vertexPath = config.shaderDirectory.empty()
                                ? std::filesystem::path{}
                                : detail::relativePathInside(root, config.shaderDirectory) / "VertexShader.glsl";
    const auto fragmentPath = config.shaderDirectory.empty()
                                  ? std::filesystem::path{}
                                  : detail::relativePathInside(root, config.shaderDirectory) / "FragmentShader.glsl";

    Diagnostics::locate(__FILE__, __LINE__, "initialize -> Renderer::initialize -> shader source/path setup");

    if (!renderer_.initialize(vertexPath, fragmentPath, config.renderConfig))
        return false;
    assets_.emplace(root, config.features);
    renderOptions_.textures = config.features.textures;

    detail::glfw().SetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    detail::glfw().SetWindowUserPointer(window_, this);
    detail::glfw().SetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.modifiedKeyHandler_)
                app.modifiedKeyHandler_(app, key, action, mods);
            else if (app.keyHandler_)
                app.keyHandler_(app, key, action);
        });
    });
    detail::glfw().SetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.focusHandler_)
                app.focusHandler_(app, focused == GLFW_TRUE);
        });
    });
    detail::glfw().SetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (!app.mouseHandler_)
                return;

            MouseEvent event;

            event.button = button;
            event.action = action;
            event.mods = mods;
            detail::glfw().GetCursorPos(window, &event.windowX, &event.windowY);

            int ww = 0, wh = 0, fw = 0, fh = 0;

            detail::glfw().GetWindowSize(window, &ww, &wh);
            detail::glfw().GetFramebufferSize(window, &fw, &fh);

            if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
                return;
            event.framebufferX = event.windowX * fw / ww;
            event.framebufferY = event.windowY * fh / wh;
            app.mouseHandler_(app, event);
        });
    });
    detail::glfw().SetCursorPosCallback(window_, [](GLFWwindow* window, double x, double y) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.cursorHandler_)
                app.cursorHandler_(app, x, y);
        });
    });

    return true;
}

void Application::shutdown() {
    assets_.reset();

    if (window_ != nullptr) {
        if (!contextFailed_ && ensureContext("shutdown -> GPU cleanup", __FILE__, __LINE__))
            renderer_.destroy();
        else
            renderer_.abandon();
        detail::glfw().SetWindowUserPointer(window_, nullptr);
        detail::glfw().DestroyWindow(window_);
        window_ = nullptr;
    }

    if (glfwInitialized_) {
        detail::glfw().Terminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        detail::glfw().SetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderScene",
                                   "활성 창 또는 컨텍스트가 없습니다.");
    }

    if (!ensureContext("renderScene -> single pass", __FILE__, __LINE__))
        return false;

    int width = 0;
    int height = 0;

    detail::glfw().GetFramebufferSize(window_, &width, &height);

    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

std::array<int, 2> Application::getFramebufferSize() const {
    std::array<int, 2> size{};

    if (window_)
        detail::glfw().GetFramebufferSize(window_, &size[0], &size[1]);

    return size;
}

bool Application::renderViews(const std::vector<RenderPass>& passes) {
    if (!window_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderViews",
                                   "활성 창 또는 컨텍스트가 없습니다.");

    if (!ensureContext("renderViews -> shared context for all viewports", __FILE__, __LINE__))
        return false;

    const auto size = getFramebufferSize();

    return renderer_.renderViews(scene_, passes, size[0], size[1]);
}

int Application::run(const ApplicationConfig& config) {
    Diagnostics::configure(config.diagnostics);
    Diagnostics::clear();

    if (running_) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "run() 재귀 호출은 허용하지 않습니다.");

        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }

    if (detail::applicationActive()) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "동시에 실행 중인 Application은 하나만 지원합니다.");

        return -1;
    }
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    detail::applicationActive() = true;
    callbackError_ = nullptr;
    running_ = true;

    int exitCode = 0;

    try {
        if (!initialize(config)) {
            std::cerr << "애플리케이션 초기화 실패.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Application::initHandler",
                                "사용자 초기화 콜백이 false를 반환했습니다.",
                                "앞선 API 오류와 장면 생성 콜백을 확인하세요.");
            std::cerr << "사용자 장면 초기화 실패.\n";
            exitCode = -1;
        } else {
            double previousTime = detail::glfw().GetTime();

            while (!detail::glfw().WindowShouldClose(window_)) {
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwPollEvents/input callbacks");
                detail::glfw().PollEvents();

                if (callbackError_)
                    std::rethrow_exception(callbackError_);

                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }

                int width = 0;
                int height = 0;

                detail::glfw().GetFramebufferSize(window_, &width, &height);

                if (width <= 0 || height <= 0) {
                    detail::glfw().WaitEvents();

                    if (callbackError_)
                        std::rethrow_exception(callbackError_);
                    previousTime = detail::glfw().GetTime();
                    continue;
                }

                const double currentTime = detail::glfw().GetTime();
                const float deltaTime = static_cast<float>(std::clamp(currentTime - previousTime, 0.0, 0.05));

                previousTime = currentTime;

                if (updateHandler_) {
                    Diagnostics::trace(PipelineStage::Input, "Application::updateHandler");
                    updateHandler_(*this, deltaTime);
                }

                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }

                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "모션 갱신 실패.\n";
                    exitCode = -1;
                    break;
                }

                if (!ensureContext("run -> render callback", __FILE__, __LINE__)) {
                    exitCode = -1;
                    break;
                }

                const bool rendered = renderHandler_ ? renderHandler_(*this) : renderScene();

                if (!rendered) {
                    std::cerr << "렌더링 실패.\n";
                    exitCode = -1;
                    break;
                }
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwSwapBuffers");

                const auto swapSerial = detail::contextErrorSerial();

                detail::glfw().SwapBuffers(window_);

                if (swapSerial != detail::contextErrorSerial()) {
                    contextFailed_ = true;
                    exitCode = -1;
                    break;
                }
            }
        }
    } catch (const std::exception& error) {
        Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::run", error.what(),
            "앞선 파이프라인 단계/API 오류를 확인하세요. 사용자 콜백 예외는 자원 정리 후 다시 전달됩니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw;
    } catch (...) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "콜백 또는 실행 처리에서 표준 예외가 아닌 예외가 발생했습니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    detail::applicationActive() = false;

    return exitCode;
}

void Application::setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler) {
    mouseHandler_ = std::move(handler);
}

void Application::setCursorHandler(std::function<void(Application&, double, double)> handler) {
    cursorHandler_ = std::move(handler);
}

std::optional<PickResult> Application::pickAt(double x, double y) const {
    if (!window_)
        return {};

    int ww = 0, wh = 0, fw = 0, fh = 0;

    detail::glfw().GetWindowSize(window_, &ww, &wh);
    detail::glfw().GetFramebufferSize(window_, &fw, &fh);

    if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
        return {};

    return graphics::pick(scene_, camera_, x * fw / ww, y * fh / wh, {0, 0, double(fw), double(fh)});
}

std::optional<PickResult> Application::pickAtCursor() const {
    if (!window_)
        return {};

    double x = 0, y = 0;

    detail::glfw().GetCursorPos(window_, &x, &y);

    return pickAt(x, y);
}

AssetLoader& Application::getAssets() {
    if (!assets_)
        throw std::logic_error("AssetLoader는 run 실행 중 콜백에서만 사용할 수 있습니다.");

    return *assets_;
}
} // namespace graphics
```

### after

```cpp
#include "../Framework.h"
#include "Internal.h"
#include <algorithm>
#include <iostream>
#include <utility>

namespace graphics::detail {
bool& applicationActive() {
    static bool active = false;

    return active;
}

unsigned& contextErrorSerial() {
    static unsigned value = 0;

    return value;
}
}

namespace graphics {

Application::~Application() {
    shutdown();
}

bool Application::configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath,
                                    const std::filesystem::path& fragmentPath) {
    if (!ensureContext("configureRenderer", __FILE__, __LINE__))
        return false;
    if (renderer_.initialize(vertexPath, fragmentPath, config))
        return true;
    requestClose();
    return false;
}

void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

void Application::setModifiedKeyHandler(std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::ensureContext(const char* operation, const char* file, int line) {
    Diagnostics::locate(file, line, operation);

    if (contextFailed_)
        return false;

    if (!window_ || GetCurrentThreadId() != contextThread_) {
        contextFailed_ = true;

        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::ensureContext",
            "창이 없거나 컨텍스트를 생성한 스레드와 다른 스레드에서 사용했습니다.",
            "창 생성·렌더링·종료를 같은 스레드에서 수행하세요. Application::run과 사용자 렌더 콜백을 확인하세요.");
    }
    using CurrentProc = void*(WINAPI*)();

    const auto module = GetModuleHandleW(L"opengl32.dll");
    const auto current =
        module ? reinterpret_cast<CurrentProc>(::GetProcAddress(module, "wglGetCurrentContext")) : nullptr;

    if (nativeContext_ && detail::glfw().GetCurrentContext() == window_ && current && current() == nativeContext_)
        return true;

    const auto serial = detail::contextErrorSerial();

    detail::glfw().MakeContextCurrent(window_);

    if (serial != detail::contextErrorSerial() || detail::glfw().GetCurrentContext() != window_ || !current ||
        !current()) {
        contextFailed_ = true;

        return Diagnostics::fail(
            PipelineStage::Runtime, "glfwMakeContextCurrent",
            "WGL이 요청한 컨텍스트를 연결하지 못하여 GL 명령 전에 렌더링을 중단했습니다.",
            "소스 위치는 연결 실패 호출입니다. 창 수명·렌더 스레드·컨텍스트를 해제/파괴하는 외부 콜백을 먼저 확인하세요. 이상이 없으면 GPU 드라이버·세션을 확인하세요. 이 로그만으로 드라이버 원인을 확정할 수 없습니다.");
    }
    nativeContext_ = current();

    return true;
}

bool Application::initialize(const ApplicationConfig& config) {
    contextThread_ = GetCurrentThreadId();
    contextFailed_ = false;
    nativeContext_ = nullptr;
    Diagnostics::trace(PipelineStage::Runtime, "Application::initialize");

    const auto root = detail::relativePathInside(detail::executableDirectory(), config.projectRoot);
    const auto runtime = detail::relativePathInside(root, config.runtimeDirectory);

    detail::glfw().load(runtime);
    detail::glfw().SetErrorCallback([](int code, const char* message) {
        ++detail::contextErrorSerial();
        Diagnostics::fail(
            PipelineStage::Runtime, "GLFW/WGL", std::to_string(code) + ": " + (message ? message : "알 수 없는 오류"),
            "표시된 소스 위치·호출 경로에서 컨텍스트 연결이 실패했습니다. Application의 창 수명과 스레드를 먼저 확인하세요. OS/드라이버 원문도 함께 보세요. 렌더 패스마다 별도 WGL 컨텍스트를 만들 필요는 없습니다.");
    });

    if (config.width <= 0 || config.height <= 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::initialize",
                                   "창 너비와 높이는 양수여야 합니다.");

    if (!detail::glfw().Init())
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwInit",
                                   "GLFW 초기화 실패: 앞선 GLFW 오류를 확인하세요.");
    glfwInitialized_ = true;
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    detail::glfw().WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    detail::glfw().WindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    detail::glfw().WindowHint(GLFW_DEPTH_BITS, 24);
    window_ = detail::glfw().createWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);

    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwCreateWindow",
                                   "OpenGL 3.3 코어 창 생성 실패.",
                                   "앞선 GLFW 오류와 그래픽 드라이버의 OpenGL 지원을 확인하세요.");
    }

    if (!ensureContext("initialize -> attach new window", __FILE__, __LINE__))
        return false;
    detail::glfw().SwapInterval(config.verticalSync ? 1 : 0);

    const auto vertexPath = config.shaderDirectory.empty()
                                ? std::filesystem::path{}
                                : detail::relativePathInside(root, config.shaderDirectory) / "VertexShader.glsl";
    const auto fragmentPath = config.shaderDirectory.empty()
                                  ? std::filesystem::path{}
                                  : detail::relativePathInside(root, config.shaderDirectory) / "FragmentShader.glsl";

    Diagnostics::locate(__FILE__, __LINE__, "initialize -> Renderer::initialize -> shader source/path setup");

    if (!renderer_.initialize(vertexPath, fragmentPath, config.renderConfig))
        return false;
    assets_.emplace(root, config.features);
    renderOptions_.textures = config.features.textures;

    detail::glfw().SetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    detail::glfw().SetWindowUserPointer(window_, this);
    detail::glfw().SetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.modifiedKeyHandler_)
                app.modifiedKeyHandler_(app, key, action, mods);
            else if (app.keyHandler_)
                app.keyHandler_(app, key, action);
        });
    });
    detail::glfw().SetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.focusHandler_)
                app.focusHandler_(app, focused == GLFW_TRUE);
        });
    });
    detail::glfw().SetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (!app.mouseHandler_)
                return;

            MouseEvent event;

            event.button = button;
            event.action = action;
            event.mods = mods;
            detail::glfw().GetCursorPos(window, &event.windowX, &event.windowY);

            int ww = 0, wh = 0, fw = 0, fh = 0;

            detail::glfw().GetWindowSize(window, &ww, &wh);
            detail::glfw().GetFramebufferSize(window, &fw, &fh);

            if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
                return;
            event.framebufferX = event.windowX * fw / ww;
            event.framebufferY = event.windowY * fh / wh;
            app.mouseHandler_(app, event);
        });
    });
    detail::glfw().SetCursorPosCallback(window_, [](GLFWwindow* window, double x, double y) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.cursorHandler_)
                app.cursorHandler_(app, x, y);
        });
    });

    return true;
}

void Application::shutdown() {
    assets_.reset();

    if (window_ != nullptr) {
        if (!contextFailed_ && ensureContext("shutdown -> GPU cleanup", __FILE__, __LINE__))
            renderer_.destroy();
        else
            renderer_.abandon();
        detail::glfw().SetWindowUserPointer(window_, nullptr);
        detail::glfw().DestroyWindow(window_);
        window_ = nullptr;
    }

    if (glfwInitialized_) {
        detail::glfw().Terminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        detail::glfw().SetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderScene",
                                   "활성 창 또는 컨텍스트가 없습니다.");
    }

    if (!ensureContext("renderScene -> single pass", __FILE__, __LINE__))
        return false;

    int width = 0;
    int height = 0;

    detail::glfw().GetFramebufferSize(window_, &width, &height);

    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

std::array<int, 2> Application::getFramebufferSize() const {
    std::array<int, 2> size{};

    if (window_)
        detail::glfw().GetFramebufferSize(window_, &size[0], &size[1]);

    return size;
}

bool Application::renderViews(const std::vector<RenderPass>& passes) {
    if (!window_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderViews",
                                   "활성 창 또는 컨텍스트가 없습니다.");

    if (!ensureContext("renderViews -> shared context for all viewports", __FILE__, __LINE__))
        return false;

    const auto size = getFramebufferSize();

    return renderer_.renderViews(scene_, passes, size[0], size[1]);
}

int Application::run(const ApplicationConfig& config) {
    Diagnostics::configure(config.diagnostics);
    Diagnostics::clear();

    if (running_) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "run() 재귀 호출은 허용하지 않습니다.");

        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }

    if (detail::applicationActive()) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "동시에 실행 중인 Application은 하나만 지원합니다.");

        return -1;
    }
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    detail::applicationActive() = true;
    callbackError_ = nullptr;
    running_ = true;

    int exitCode = 0;

    try {
        if (!initialize(config)) {
            std::cerr << "애플리케이션 초기화 실패.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Application::initHandler",
                                "사용자 초기화 콜백이 false를 반환했습니다.",
                                "앞선 API 오류와 장면 생성 콜백을 확인하세요.");
            std::cerr << "사용자 장면 초기화 실패.\n";
            exitCode = -1;
        } else {
            double previousTime = detail::glfw().GetTime();

            while (!detail::glfw().WindowShouldClose(window_)) {
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwPollEvents/input callbacks");
                detail::glfw().PollEvents();

                if (callbackError_)
                    std::rethrow_exception(callbackError_);

                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }

                int width = 0;
                int height = 0;

                detail::glfw().GetFramebufferSize(window_, &width, &height);

                if (width <= 0 || height <= 0) {
                    detail::glfw().WaitEvents();

                    if (callbackError_)
                        std::rethrow_exception(callbackError_);
                    previousTime = detail::glfw().GetTime();
                    continue;
                }

                const double currentTime = detail::glfw().GetTime();
                const float deltaTime = static_cast<float>(std::clamp(currentTime - previousTime, 0.0, 0.05));

                previousTime = currentTime;

                if (updateHandler_) {
                    Diagnostics::trace(PipelineStage::Input, "Application::updateHandler");
                    updateHandler_(*this, deltaTime);
                }

                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }

                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "모션 갱신 실패.\n";
                    exitCode = -1;
                    break;
                }

                if (!ensureContext("run -> render callback", __FILE__, __LINE__)) {
                    exitCode = -1;
                    break;
                }

                const bool rendered = renderHandler_ ? renderHandler_(*this) : renderScene();

                if (!rendered) {
                    std::cerr << "렌더링 실패.\n";
                    exitCode = -1;
                    break;
                }
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwSwapBuffers");

                const auto swapSerial = detail::contextErrorSerial();

                detail::glfw().SwapBuffers(window_);

                if (swapSerial != detail::contextErrorSerial()) {
                    contextFailed_ = true;
                    exitCode = -1;
                    break;
                }
            }
        }
    } catch (const std::exception& error) {
        Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::run", error.what(),
            "앞선 파이프라인 단계/API 오류를 확인하세요. 사용자 콜백 예외는 자원 정리 후 다시 전달됩니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw;
    } catch (...) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "콜백 또는 실행 처리에서 표준 예외가 아닌 예외가 발생했습니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    detail::applicationActive() = false;

    return exitCode;
}

void Application::setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler) {
    mouseHandler_ = std::move(handler);
}

void Application::setCursorHandler(std::function<void(Application&, double, double)> handler) {
    cursorHandler_ = std::move(handler);
}

std::optional<PickResult> Application::pickAt(double x, double y) const {
    if (!window_)
        return {};

    int ww = 0, wh = 0, fw = 0, fh = 0;

    detail::glfw().GetWindowSize(window_, &ww, &wh);
    detail::glfw().GetFramebufferSize(window_, &fw, &fh);

    if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
        return {};

    return graphics::pick(scene_, camera_, x * fw / ww, y * fh / wh, {0, 0, double(fw), double(fh)},
                          renderOptions_.cull);
}

std::optional<PickResult> Application::pickAtCursor() const {
    if (!window_)
        return {};

    double x = 0, y = 0;

    detail::glfw().GetCursorPos(window_, &x, &y);

    return pickAt(x, y);
}

AssetLoader& Application::getAssets() {
    if (!assets_)
        throw std::logic_error("AssetLoader는 run 실행 중 콜백에서만 사용할 수 있습니다.");

    return *assets_;
}
} // namespace graphics
```

## framework/include/Picking.h

### before

```cpp
#pragma once
#include "Graphics.h"

namespace graphics {
struct Ray {
    glm::vec3 origin{0}, direction{0, 0, -1};
    float maxDistance = std::numeric_limits<float>::infinity();
};

struct PickResult {
    EntityId entityId = 0;
    std::size_t faceIndex = 0, triangleIndex = 0;
    glm::vec3 worldPoint{0}, localPoint{0}, barycentric{0};
    glm::vec2 uv{0};
    bool hasUV = false;
    float distance = 0; // Distance from ray origin (near plane for a screen ray).
};

struct PickViewport {
    double x = 0, y = 0, width = 0, height = 0;
}; // top-left framebuffer pixels


/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
inline std::optional<Ray> screenRay(const Camera& camera, double x, double y, const PickViewport& viewport) {
    if (!CameraSystem::validCamera(camera) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(viewport.x) ||
        !std::isfinite(viewport.y) || !std::isfinite(viewport.width) || !std::isfinite(viewport.height) ||
        viewport.width <= 0 || viewport.height <= 0 || x < viewport.x || y < viewport.y ||
        x >= viewport.x + viewport.width || y >= viewport.y + viewport.height)
        return {};

    const auto pv = CameraSystem::projectionMatrix(camera, float(viewport.width / viewport.height)) *
                    CameraSystem::viewMatrix(camera);
    const auto determinant = glm::determinant(pv);

    if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
        return {};

    const auto inverse = glm::inverse(pv);
    const float nx = float(2 * (x - viewport.x) / viewport.width - 1),
                ny = float(1 - 2 * (y - viewport.y) / viewport.height);
    auto nearPoint = inverse * glm::vec4(nx, ny, -1, 1), farPoint = inverse * glm::vec4(nx, ny, 1, 1);

    if (std::abs(nearPoint.w) < 1e-12f || std::abs(farPoint.w) < 1e-12f)
        return {};
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    const auto direction = glm::vec3(farPoint - nearPoint);
    const auto length = glm::length(direction);

    if (!detail::finiteVector(glm::vec3(nearPoint)) || !detail::finiteVector(direction) || !std::isfinite(length) ||
        length <= 0)
        return {};

    return Ray{glm::vec3(nearPoint), direction / length, length};
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Ray& input) {
    if (!detail::finiteVector(input.origin) || !detail::finiteVector(input.direction) ||
        !std::isfinite(glm::length(input.direction)) || glm::length(input.direction) < 1e-12f ||
        std::isnan(input.maxDistance) || input.maxDistance < 0)
        return {};

    const auto direction = glm::dvec3(glm::normalize(input.direction));
    const auto origin = glm::dvec3(input.origin);
    double nearest = input.maxDistance;
    std::optional<PickResult> result;

    for (auto id : scene.getIds()) {
        const auto* component = scene.getComponent(id);

        if (!component || !component->visible)
            continue;

        for (std::size_t f = 0; f < component->polyhedron.faces.size(); ++f) {
            const auto& face = component->polyhedron.faces[f];

            if (!face.visible)
                continue;

            const auto world = scene.faceWorldMatrix(id, f);

            if (!world)
                continue;

            const auto matrix = *world;
            const auto indices = detail::triangles(face);

            for (std::size_t t = 0; t < indices.size(); t += 3) {
                const auto ia = indices[t], ib = indices[t + 1], ic = indices[t + 2];
                const auto a4 = matrix * glm::vec4(face.localVertices[ia], 1),
                           b4 = matrix * glm::vec4(face.localVertices[ib], 1),
                           c4 = matrix * glm::vec4(face.localVertices[ic], 1);

                if (std::abs(a4.w) < 1e-12f || std::abs(b4.w) < 1e-12f || std::abs(c4.w) < 1e-12f)
                    continue;

                const auto a = glm::dvec3(a4 / a4.w), b = glm::dvec3(b4 / b4.w), c = glm::dvec3(c4 / c4.w);
                const auto edge1 = b - a, edge2 = c - a, p = glm::cross(direction, edge2);
                const auto determinant = glm::dot(edge1, p);
                const auto tolerance = 1e-12 * glm::length(edge1) * glm::length(edge2);

                if (!std::isfinite(determinant) || std::abs(determinant) <= tolerance)
                    continue;

                const auto relative = origin - a;
                const auto u = glm::dot(relative, p) / determinant;

                if (u < 0 || u > 1)
                    continue;

                const auto q = glm::cross(relative, edge1);
                const auto v = glm::dot(direction, q) / determinant;

                if (v < 0 || u + v > 1)
                    continue;

                const auto distance = glm::dot(edge2, q) / determinant;

                if (!std::isfinite(distance) || distance < 0 || distance > nearest)
                    continue;

                const glm::dvec3 weights((1 - u - v) / a4.w, u / b4.w, v / c4.w);
                const auto sum = weights.x + weights.y + weights.z;

                if (std::abs(sum) < 1e-12)
                    continue;

                PickResult hit;

                hit.entityId = id;
                hit.faceIndex = f;
                hit.triangleIndex = t / 3;
                hit.distance = float(distance);
                hit.worldPoint = glm::vec3(origin + direction * distance);
                hit.barycentric = glm::vec3(weights / sum);
                hit.localPoint = face.localVertices[ia] * hit.barycentric.x +
                                 face.localVertices[ib] * hit.barycentric.y +
                                 face.localVertices[ic] * hit.barycentric.z;
                hit.hasUV = !face.texcoords.empty();

                if (hit.hasUV)
                    hit.uv = face.texcoords[ia] * hit.barycentric.x + face.texcoords[ib] * hit.barycentric.y +
                             face.texcoords[ic] * hit.barycentric.z;
                nearest = distance;
                result = hit;
            }
        }
    }

    return result;
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
                                      const PickViewport& viewport) {
    const auto ray = screenRay(camera, x, y, viewport);

    return ray ? pick(scene, *ray) : std::nullopt;
}
} // namespace graphics
```

### after

```cpp
#pragma once
#include "Graphics.h"

namespace graphics {
struct Ray {
    glm::vec3 origin{0}, direction{0, 0, -1};
    float maxDistance = std::numeric_limits<float>::infinity();
};

struct PickResult {
    EntityId entityId = 0;
    std::size_t faceIndex = 0, triangleIndex = 0;
    glm::vec3 worldPoint{0}, localPoint{0}, barycentric{0};
    glm::vec2 uv{0};
    bool hasUV = false;
    float distance = 0; // Distance from ray origin (near plane for a screen ray).
};

struct PickViewport {
    double x = 0, y = 0, width = 0, height = 0;
}; // top-left framebuffer pixels


/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
inline std::optional<Ray> screenRay(const Camera& camera, double x, double y, const PickViewport& viewport) {
    if (!CameraSystem::validCamera(camera) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(viewport.x) ||
        !std::isfinite(viewport.y) || !std::isfinite(viewport.width) || !std::isfinite(viewport.height) ||
        viewport.width <= 0 || viewport.height <= 0 || x < viewport.x || y < viewport.y ||
        x >= viewport.x + viewport.width || y >= viewport.y + viewport.height)
        return {};

    const auto pv = CameraSystem::projectionMatrix(camera, float(viewport.width / viewport.height)) *
                    CameraSystem::viewMatrix(camera);
    const auto determinant = glm::determinant(pv);

    if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
        return {};

    const auto inverse = glm::inverse(pv);
    const float nx = float(2 * (x - viewport.x) / viewport.width - 1),
                ny = float(1 - 2 * (y - viewport.y) / viewport.height);
    auto nearPoint = inverse * glm::vec4(nx, ny, -1, 1), farPoint = inverse * glm::vec4(nx, ny, 1, 1);

    if (std::abs(nearPoint.w) < 1e-12f || std::abs(farPoint.w) < 1e-12f)
        return {};
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    const auto direction = glm::vec3(farPoint - nearPoint);
    const auto length = glm::length(direction);

    if (!detail::finiteVector(glm::vec3(nearPoint)) || !detail::finiteVector(direction) || !std::isfinite(length) ||
        length <= 0)
        return {};

    return Ray{glm::vec3(nearPoint), direction / length, length};
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Ray& input, CullMode inheritedCull = CullMode::None) {
    if (!detail::finiteVector(input.origin) || !detail::finiteVector(input.direction) ||
        !std::isfinite(glm::length(input.direction)) || glm::length(input.direction) < 1e-12f ||
        std::isnan(input.maxDistance) || input.maxDistance < 0)
        return {};

    const auto direction = glm::dvec3(glm::normalize(input.direction));
    const auto origin = glm::dvec3(input.origin);
    double nearest = input.maxDistance;
    std::optional<PickResult> result;

    for (auto id : scene.getIds()) {
        const auto* component = scene.getComponent(id);

        if (!component || !component->visible)
            continue;

        for (std::size_t f = 0; f < component->polyhedron.faces.size(); ++f) {
            const auto& face = component->polyhedron.faces[f];

            if (!face.visible)
                continue;

            const auto world = scene.faceWorldMatrix(id, f);

            if (!world)
                continue;

            const auto matrix = *world;
            const auto cull = face.cull == CullMode::Inherit ? inheritedCull : face.cull;
            const bool mirrored = glm::determinant(glm::mat3(matrix)) < 0;
            const auto indices = detail::triangles(face);

            for (std::size_t t = 0; t < indices.size(); t += 3) {
                const auto ia = indices[t], ib = indices[t + 1], ic = indices[t + 2];
                const auto a4 = matrix * glm::vec4(face.localVertices[ia], 1),
                           b4 = matrix * glm::vec4(face.localVertices[ib], 1),
                           c4 = matrix * glm::vec4(face.localVertices[ic], 1);

                if (std::abs(a4.w) < 1e-12f || std::abs(b4.w) < 1e-12f || std::abs(c4.w) < 1e-12f)
                    continue;

                const auto a = glm::dvec3(a4 / a4.w), b = glm::dvec3(b4 / b4.w), c = glm::dvec3(c4 / c4.w);
                const auto edge1 = b - a, edge2 = c - a, p = glm::cross(direction, edge2);
                const auto determinant = glm::dot(edge1, p);
                const auto tolerance = 1e-12 * glm::length(edge1) * glm::length(edge2);

                if (!std::isfinite(determinant) || std::abs(determinant) <= tolerance)
                    continue;

                const bool frontFacing = (determinant > 0) != mirrored;
                if ((cull == CullMode::Front && frontFacing) || (cull == CullMode::Back && !frontFacing))
                    continue;

                const auto relative = origin - a;
                const auto u = glm::dot(relative, p) / determinant;

                if (u < 0 || u > 1)
                    continue;

                const auto q = glm::cross(relative, edge1);
                const auto v = glm::dot(direction, q) / determinant;

                if (v < 0 || u + v > 1)
                    continue;

                const auto distance = glm::dot(edge2, q) / determinant;

                if (!std::isfinite(distance) || distance < 0 || distance > nearest)
                    continue;

                const glm::dvec3 weights((1 - u - v) / a4.w, u / b4.w, v / c4.w);
                const auto sum = weights.x + weights.y + weights.z;

                if (std::abs(sum) < 1e-12)
                    continue;

                PickResult hit;

                hit.entityId = id;
                hit.faceIndex = f;
                hit.triangleIndex = t / 3;
                hit.distance = float(distance);
                hit.worldPoint = glm::vec3(origin + direction * distance);
                hit.barycentric = glm::vec3(weights / sum);
                hit.localPoint = face.localVertices[ia] * hit.barycentric.x +
                                 face.localVertices[ib] * hit.barycentric.y +
                                 face.localVertices[ic] * hit.barycentric.z;
                hit.hasUV = !face.texcoords.empty();

                if (hit.hasUV)
                    hit.uv = face.texcoords[ia] * hit.barycentric.x + face.texcoords[ib] * hit.barycentric.y +
                             face.texcoords[ic] * hit.barycentric.z;
                nearest = distance;
                result = hit;
            }
        }
    }

    return result;
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
                                      const PickViewport& viewport, CullMode inheritedCull = CullMode::None) {
    const auto ray = screenRay(camera, x, y, viewport);

    return ray ? pick(scene, *ray, inheritedCull) : std::nullopt;
}
} // namespace graphics
```

## standard/Console.cpp

### before

```cpp
#include "Console.h"
#include <iostream>

/// 선택한 경로로 이동 모션을 바꾼다.
bool Console::choosePath(int kind) {
    graphics::PathMotion path;

    path.durationSeconds = 6;

    const glm::vec3 center(1.7f, 0, 0);

    if (kind == 1) {
        path.path = graphics::PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 2) {
        path.path = graphics::PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 3) {
        path.path = graphics::PathSystem::polygon(
            {{1, -.5f, 0}, {2.4f, -.4f, 0}, {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
    } else if (kind == 4) {
        path.path = [center](double u) {
            const double angle = u * 6.283185307179586;

            return center +
                   glm::vec3(float(.7 * std::sin(2 * angle)), float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
        };
        path.interpolation = graphics::PathSystem::smoothStep;
    } else if (kind == 5) {
        path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    } else if (kind == 6) {
        path.path = graphics::PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
        path.playback = graphics::Playback::PingPong;
    } else {
        return false;
    }

    graphics::Motion motion;
    auto& motions = app_.getScene().getMotions();

    if (!pathId_ || !motion.append(graphics::Motion::followPath(path))) {
        return false;
    }

    if (!motions.setMotion(*pathId_, motion) || !motions.seekMotion(*pathId_, 0)) {
        return false;
    }
    std::cout << "경로 " << kind << ": 시작 시각 0초로 변경\n";

    return true;
}

/// 장면 객체와 초기 상태를 구성한다.
bool Console::initScene() {
    demo_ = Demo::Motion;

    auto& scene = app_.getScene();

    scene.clear();
    paused_ = false;
    spinEnabled_ = true;
    globalEnabled_ = false;
    reflection_ = 1;
    insertedLayer_.reset();

    const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
    const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});

    if (!first || !second) {
        return false;
    }

    if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) {
        return false;
    }

    auto& motions = scene.getMotions();

    pathId_ = motions.createMotion(graphics::Motion{});

    graphics::RotationMotion spin;

    spin.degreesPerSecond = 90;
    spinId_ = motions.createSpin(spin);

    graphics::RotationMotion global;

    global.degreesPerSecond = 15;
    globalId_ = motions.createOrbit(global);

    if (!pathId_ || !spinId_ || !globalId_ || !choosePath(1)) {
        return false;
    }

    if (!motions.setMotionEnabled(*globalId_, false) ||
        !motions.bindMotion(graphics::MotionSystem::rootLayer(), *globalId_)) {
        return false;
    }

    const auto normalPath = motions.pushLayer(graphics::MotionSystem::rootLayer());
    const auto reflection = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});

    if (!normalPath || !reflection) {
        return false;
    }

    graphics::LayerConfig mirror;

    mirror.transform = *reflection;
    mirrorLayer_ = motions.pushLayer(graphics::MotionSystem::rootLayer(), mirror);

    if (!mirrorLayer_) {
        return false;
    }

    const auto mirroredPath = motions.pushLayer(*mirrorLayer_);

    normalSpinLayer_ = motions.pushLayer(*normalPath);

    if (!mirroredPath || !normalSpinLayer_) {
        return false;
    }

    const auto mirroredSpin = motions.pushLayer(*mirroredPath);

    if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId_) || !motions.bindMotion(*mirroredPath, *pathId_) ||
        !motions.bindMotion(*normalSpinLayer_, *spinId_) || !motions.bindMotion(*mirroredSpin, *spinId_) ||
        !scene.bindEntity(*first, *normalSpinLayer_) || !scene.bindEntity(*second, *mirroredSpin)) {
        return false;
    }

    if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first) ||
        !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second) ||
        !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) {
        return false;
    }

    auto& camera = app_.getCamera();

    camera.position = {5, 3, 8};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    printHelp();

    return true;
}

/// 반사 변환을 순서대로 변경한다.
bool Console::changeReflection() {
    const int next = (reflection_ + 1) % 4;
    std::optional<glm::mat4> matrix = glm::mat4(1);

    if (next == 1) {
        matrix = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
    } else if (next == 2) {
        matrix = graphics::TransformSystem::reflectPoint({0, 0, 0});
    } else if (next == 3) {
        matrix = graphics::TransformSystem::reflectLine(graphics::AxisLine{});
    }

    if (!mirrorLayer_ || !matrix) {
        return false;
    }

    graphics::LayerConfig config;

    config.transform = *matrix;

    if (!app_.getScene().getMotions().setLayer(*mirrorLayer_, config)) {
        return false;
    }
    reflection_ = next;

    const char* names[] = {"없음", "YZ평면 반사", "원점 점대칭", "Y축 직선대칭"};

    std::cout << "반사: " << names[reflection_] << '\n';

    return true;
}

/// 크기 레이어를 삽입하거나 제거한다.
bool Console::toggleInsertedLayer() {
    auto& motions = app_.getScene().getMotions();

    if (insertedLayer_) {
        if (!motions.removeLayer(*insertedLayer_)) {
            return false;
        }
        insertedLayer_.reset();
        std::cout << "추가 크기 레이어 삭제\n";

        return true;
    }

    const auto scale = graphics::TransformSystem::scaling(glm::vec3(1.5f));

    if (!normalSpinLayer_ || !scale) {
        return false;
    }

    graphics::LayerConfig config;

    config.space = graphics::TransformSpace::Local;
    config.transform = *scale;
    insertedLayer_ = motions.insertLayer(*normalSpinLayer_, config);

    if (!insertedLayer_) {
        return false;
    }
    std::cout << "첫 객체에 1.5배 Local 크기 레이어 삽입\n";

    return true;
}

/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }

    if (key == GLFW_KEY_7 || key == GLFW_KEY_8 || key == GLFW_KEY_9 || key == GLFW_KEY_0) {
        const bool ok = key == GLFW_KEY_7   ? initGrid()
                        : key == GLFW_KEY_8 ? initRoom()
                        : key == GLFW_KEY_9 ? initAnimation()
                                            : initScene();

        if (!ok)
            app_.requestClose();

        return;
    }

    if (key == GLFW_KEY_V) {
        threeViews_ = !threeViews_;

        return;
    }

    if (key == GLFW_KEY_D) {
        trace_ = !trace_;
        graphics::Diagnostics::configure({true, trace_});

        return;
    }

    if (demo_ == Demo::Animation && handleJointKey(key))
        return;

    if (key == GLFW_KEY_F && demo_ == Demo::Room) {
        floorOpen_ = !floorOpen_;

        return;
    }

    if (demo_ != Demo::Motion) {
        if (key == GLFW_KEY_SPACE)
            clock_.playing = !clock_.playing;
        else if (key == GLFW_KEY_ESCAPE)
            app_.requestClose();
        else if (key == GLFW_KEY_H)
            printHelp();

        return;
    }

    auto& motions = app_.getScene().getMotions();
    bool ok = true;

    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        ok = choosePath(key - GLFW_KEY_1 + 1);
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;

        ok = pathId_ && spinId_ && globalId_ && motions.setMotionPlaying(*pathId_, playing) &&
             motions.setMotionPlaying(*spinId_, playing) && motions.setMotionPlaying(*globalId_, playing);

        if (ok) {
            paused_ = !paused_;
            std::cout << (paused_ ? "시간 정지\n" : "시간 재개\n");
        }
    } else if (key == GLFW_KEY_S) {
        ok = spinId_ && motions.setMotionEnabled(*spinId_, !spinEnabled_);

        if (ok) {
            spinEnabled_ = !spinEnabled_;
            std::cout << "자전 기여: " << spinEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_G) {
        ok = globalId_ && motions.setMotionEnabled(*globalId_, !globalEnabled_);

        if (ok) {
            globalEnabled_ = !globalEnabled_;
            std::cout << "전체 공전 기여: " << globalEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_M) {
        ok = changeReflection();
    } else if (key == GLFW_KEY_I) {
        ok = toggleInsertedLayer();
    } else if (key == GLFW_KEY_H) {
        printHelp();
    }

    if (!ok) {
        std::cerr << "샘플 명령 적용 실패. 장면을 종료합니다.\n";
        app_.requestClose();
    }
}

/// 마우스 버튼 입력을 처리한다.
void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }

    std::optional<graphics::PickResult> hit;

    if (threeViews_) {
        const auto size = app_.getFramebufferSize();

        if (size[0] < 3 || size[1] < 2)
            return;

        const auto passes =
            graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

        for (const auto& pass : passes) {
            const auto& v = pass.viewport;
            const double top = size[1] - v.y - v.height;

            if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top &&
                event.framebufferY < top + v.height)
                hit = graphics::pick(app_.getScene(), pass.camera, event.framebufferX, event.framebufferY,
                                     {double(v.x), top, double(v.width), double(v.height)});
        }
    } else
        hit = app_.pickAt(event.windowX, event.windowY);

    if (hit) {
        std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    }
}

/// 조작 방법을 출력한다.
void Console::printHelp() const {
    std::cout
        << "standard: 프레임워크 Motion·반사·레이어 샘플\n"
        << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
        << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
        << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
        << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
        << "왼쪽 클릭: 물체의 entity/face ID 출력\n"
        << "7 Grid/거리 기반 배치 / 8 실내 미끄럼·낙하 / 0 기존 Motion 예제\n"
        << "9 선형보간·진자·사람형·그네→사람 계층 애니메이션\n"
        << "9번 관절: Tab/Backspace 선택, X/Y/Z 축, 좌우 회전, 상하 이동, PageUp/Down 피벗, +/- 균일 배율, R 복원\n"
        << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

/// 거리 기반 격자 예제를 구성한다.
bool Console::initGrid() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Grid;
    clock_.reset();
    gridBoxes_.clear();

    graphics::GridLayout grid;

    grid.origin = {-3, -.1f, -2};
    grid.extent = {6, .2f, 4};
    grid.cells = {6, 1, 4};

    for (int z = 0; z < 4; ++z)
        for (int x = 0; x < 6; ++x) {
            const auto center = grid.cellCenter({x, 0, z}), size = grid.relativeSize({.15f, 1, .22f});

            if (!center || !size ||
                !graphics::PatternSystem::addBox(scene, *size, *center,
                                                 (x + z) % 2 ? graphics::GlColor{.25f, .3f, .4f}
                                                             : graphics::GlColor{.4f, .45f, .55f}))
                return false;
        }

    if (!distancePath_.setPoints({{-2.5f, .35f, -1.5f}, {2.5f, .35f, -1.5f}, {2.5f, .35f, 1.5f}}))
        return false;

    for (unsigned i = 0; i < 5; ++i) {
        const auto box =
            graphics::PatternSystem::addBox(scene, {.4f, .5f, .4f}, {0, 0, 0}, {.25f + .14f * i, .7f - .1f * i, .3f});

        if (!box || !graphics::PatternSystem::addNumber(scene, i + 1, {-.07f, -.1f, .205f}, .2f, {1, 1, 1}, *box))
            return false;
        gridBoxes_.push_back(*box);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {7, 6, 8};
    app_.getCamera().target = {0, 0, 0};
    app_.getCamera().orthographicHalfExtent = 4;

    return true;
}

/// 회전하는 방 예제를 구성한다.
bool Console::initRoom() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Room;
    clock_.reset();
    floorOpen_ = false;

    const auto room = graphics::PatternSystem::addRoom(scene, {{-2, -1, -2}, {2, 2, 2}});
    const auto cube = graphics::PatternSystem::addBox(scene, {.5f, .5f, .5f}, {0, -.75f, 0}, {1, .7f, .2f});

    if (!room || !cube)
        return false;
    room_ = *room;
    roomCube_ = *cube;
    roomBody_ = graphics::RoomBody{};
    roomBody_.localPosition = {0, -.75f, 0};
    roomBody_.halfExtent = {.25f, .25f, .25f};
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {5, 3, 8};
    app_.getCamera().target = {0, 0, 0};

    return true;
}

/// 선형보간과 그네·사람의 부모·자식 애니메이션을 구성한다.
bool Console::initAnimation() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Animation;
    clock_.reset();
    animation_.clear();
    jointControls_.clear();
    selectedJoint_ = 0;
    selectedAxis_ = 0;

    if (!graphics::PatternSystem::addBox(scene, {8, .15f, 6}, {0, -.15f, 0}, {.25f, .3f, .35f}))
        return false;

    graphics::SwingConfig swingConfig;

    swingConfig.pivot = {1, 3, 0};
    swingConfig.pendulum.amplitudeDegrees = 25;
    swingConfig.pendulum.periodSeconds = 3;

    const auto swing = graphics::AnimationSystem::addSwingWithRider(scene, swingConfig);

    if (!swing)
        return false;
    animation_ = swing->animation;

    graphics::HumanoidConfig personConfig;

    personConfig.grid.frame = glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0));

    const auto person = graphics::AnimationSystem::addHumanoid(scene, personConfig);

    if (!person)
        return false;

    auto pose = graphics::HumanoidPose::seated();

    pose.hips = {glm::vec3(0), glm::vec3(0)};
    pose.knees = {glm::vec3(0), glm::vec3(0)};

    if (!graphics::AnimationSystem::poseHumanoid(scene, *person, pose))
        return false;

    for (std::size_t i = 0; i < 2; ++i) {
        graphics::PendulumMotion sway;

        sway.amplitudeDegrees = 30;
        sway.periodSeconds = 2;
        sway.phaseDegrees = i == 0 ? 0 : 180;

        graphics::Motion motion;

        if (!motion.append(graphics::Motion::pendulum(sway)))
            return false;

        const auto track = graphics::AnimationSystem::createJointAnimation(scene, person->shoulders[i], motion);

        if (!track)
            return false;
        animation_.push_back(*track);
    }

    const auto marker = scene.addGroup();

    if (!marker || !graphics::PatternSystem::addBox(scene, {.4f, .4f, .4f}, {0, 0, 0}, {1, .85f, .15f}, *marker))
        return false;

    graphics::LinearMotion move;

    move.from = {-2, .3f, 2};
    move.to = {2, .3f, 2};
    move.durationSeconds = 4;
    move.playback = graphics::Playback::PingPong;

    graphics::Motion motion;

    if (!motion.append(graphics::Motion::linear(move)))
        return false;

    const auto track = graphics::AnimationSystem::createJointAnimation(scene, *marker, motion);

    if (!track)
        return false;
    animation_.push_back(*track);

    const auto control = [&](graphics::EntityId id, const std::string& name) {
        std::size_t index = 0;

        while (index < animation_.size() && animation_[index].joint != id)
            ++index;

        if (index == animation_.size()) {
            auto fixed = graphics::AnimationSystem::createJointAnimation(scene, id, graphics::Motion{});

            if (!fixed)
                return false;
            animation_.push_back(*fixed);
        }
        jointControls_.push_back({index, name, animation_[index].rest});

        return true;
    };

    const auto personControls = [&](const graphics::HumanoidRig& rig, const std::string& prefix) {
        if (!control(rig.root, prefix + " root") || !control(rig.pelvis, prefix + " pelvis") ||
            !control(rig.torso, prefix + " torso") || !control(rig.neck, prefix + " neck"))
            return false;

        for (std::size_t i = 0; i < 2; ++i) {
            const auto side = prefix + (i == 0 ? " left " : " right ");

            if (!control(rig.shoulders[i], side + "shoulder") || !control(rig.elbows[i], side + "elbow") ||
                !control(rig.hands[i], side + "hand") || !control(rig.hips[i], side + "hip") ||
                !control(rig.knees[i], side + "knee") || !control(rig.feet[i], side + "foot"))
                return false;
        }

        return true;
    };

    if (!control(swing->frame, "swing frame") || !control(swing->hinge, "swing hinge") ||
        !control(swing->seat, "swing seat") || !personControls(swing->person, "rider") ||
        !personControls(*person, "standing"))
        return false;
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 10};
    app_.getCamera().target = {0, 1.4f, 0};
    std::cout << "9: 노란 상자는 4초 편도 선형보간, 그네는 3초 주기 진자, 좌석의 사람은 계층 변환을 따릅니다.\n";
    printSelectedJoint();

    return graphics::AnimationSystem::poseHierarchy(scene, animation_, 0);
}

/// 선택한 관절의 기준 자세를 편집하고 같은 시각의 애니메이션에 반영한다.
bool Console::handleJointKey(int key) {
    if (jointControls_.empty())
        return false;

    if (key == GLFW_KEY_TAB)
        selectedJoint_ = (selectedJoint_ + 1) % jointControls_.size();
    else if (key == GLFW_KEY_BACKSPACE)
        selectedJoint_ = (selectedJoint_ + jointControls_.size() - 1) % jointControls_.size();
    else if (key == GLFW_KEY_X || key == GLFW_KEY_Y || key == GLFW_KEY_Z)
        selectedAxis_ = key == GLFW_KEY_X ? 0 : key == GLFW_KEY_Y ? 1 : 2;
    else {
        auto& track = animation_[jointControls_[selectedJoint_].track];
        const auto saved = track.rest;

        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
            glm::vec3 axis(0);

            axis[selectedAxis_] = 1;
            track.rest.rotation =
                track.rest.rotation *
                glm::mat3(glm::rotate(glm::mat4(1), glm::radians(key == GLFW_KEY_LEFT ? -5.0f : 5.0f), axis));
        } else if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN)
            track.rest.basis[selectedAxis_] += key == GLFW_KEY_UP ? .05f : -.05f;
        else if (key == GLFW_KEY_PAGE_UP || key == GLFW_KEY_PAGE_DOWN)
            track.rest.localBasis[selectedAxis_] += key == GLFW_KEY_PAGE_UP ? .05f : -.05f;
        else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD || key == GLFW_KEY_MINUS ||
                 key == GLFW_KEY_KP_SUBTRACT) {
            const bool grow = key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD;

            track.rest.scale = glm::vec3(std::clamp(track.rest.scale.x * (grow ? 1.1f : 1 / 1.1f), .1f, 3.0f));
        } else if (key == GLFW_KEY_R)
            track.rest = jointControls_[selectedJoint_].initialRest;
        else
            return false;

        if (!graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds)) {
            track.rest = saved;
            std::cerr << "관절 편집 거절: 기준 자세를 복원했습니다.\n";
        }
    }
    printSelectedJoint();

    return true;
}

/// 선택 관절·축·이동·피벗·균일 배율을 출력한다.
void Console::printSelectedJoint() const {
    if (jointControls_.empty())
        return;

    const auto& control = jointControls_[selectedJoint_];
    const auto& rest = animation_[control.track].rest;

    std::cout << "joint " << selectedJoint_ + 1 << '/' << jointControls_.size() << ": " << control.name
              << " axis=" << "XYZ"[selectedAxis_] << " position=" << rest.basis.x << ',' << rest.basis.y << ','
              << rest.basis.z << " pivot=" << rest.localBasis.x << ',' << rest.localBasis.y << ',' << rest.localBasis.z
              << " scale=" << rest.scale.x << '\n';
}

/// 프레임 시간으로 상태와 장면을 갱신한다.
void Console::onUpdate(float dt) {
    if (demo_ == Demo::Motion || !clock_.playing)
        return;

    if (!clock_.advance(dt)) {
        app_.requestClose();

        return;
    }

    auto& scene = app_.getScene();
    bool ok = true;

    if (demo_ == Demo::Animation) {
        ok = graphics::AnimationSystem::poseHierarchy(scene, animation_, clock_.seconds);
    } else if (demo_ == Demo::Grid) {
        for (std::size_t i = 0; i < gridBoxes_.size(); ++i)
            ok = graphics::PatternSystem::follow(scene, gridBoxes_[i], distancePath_,
                                                 clock_.seconds + distancePath_.length() * i / 5) &&
                 ok;
    } else {
        const float angle = .4f * float(std::sin(clock_.seconds));
        const auto matrix = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));
        graphics::Transform3D roomTransform;

        roomTransform.rotation = glm::mat3(matrix);
        roomTransform.basis = glm::vec3(matrix * glm::vec4(0, .5f, 0, 1));
        ok =
            scene.setTransform(room_, roomTransform) && scene.setFaceVisible(room_, 5, !floorOpen_) &&
            graphics::CollisionSystem::stepRoomBody(roomBody_, {{-2, -1, -2}, {2, 2, 2}}, matrix, dt, floorOpen_, false,
                                                    {0, -9.8f, 0}, {0, 0, .4f * float(std::cos(clock_.seconds))});

        graphics::Transform3D cubeTransform;

        cubeTransform.basis = roomBody_.position(matrix);

        if (!roomBody_.released)
            cubeTransform.rotation = glm::mat3(matrix);
        ok = scene.setTransform(roomCube_, cubeTransform) && ok;
    }

    if (!ok)
        app_.requestClose();
}

/// 현재 장면을 지정한 뷰로 렌더링한다.
bool Console::onRender() {
    if (!threeViews_)
        return app_.renderScene();

    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    return app_.renderViews(
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
```

### after

```cpp
#include "Console.h"
#include <iostream>

bool Console::initScene() {
    printHelp();
    return activateMode(initialMode_.value_or(1));
}

bool Console::activateMode(int mode) {
    if (mode < 1 || mode > lastStandardMode)
        return false;
    if (mode <= 6 && demo_ == Demo::Motion && pathId_) {
        activeMode_ = mode;
        return choosePath(mode);
    }
    if (practiceMode_)
        practiceMode_->onFocus(false);
    practiceMode_.reset();
    activeMode_ = mode;
    animation_.clear();
    jointControls_.clear();
    selectedJoint_ = 0;
    selectedAxis_ = 0;
    roomControl_.reset();
    bodyControl_.reset();
    pathId_.reset();
    spinId_.reset();
    globalId_.reset();
    mirrorLayer_.reset();
    normalSpinLayer_.reset();
    insertedLayer_.reset();
    clock_.reset();
    app_.getCamera() = graphics::Camera{};
    app_.getRenderOptions() = graphics::RenderOptions{};
    if (mode >= 10) {
        practiceMode_ = makePracticeMode(mode, app_);
        return practiceMode_ && practiceMode_->initScene();
    }
    const bool ok = mode <= 6 ? initMotion(mode) : mode == 7 ? initGrid() : mode == 8 ? initRoom() : initAnimation();
    if (!ok || !applyControls())
        return false;
    printSelectedJoint();
    return true;
}

/// 키 입력에 따라 동작을 변경한다.
void Console::onKey(int key, int action, int mods) {
    if (!initialMode_ && action == GLFW_PRESS) {
        int next = 0;
        if (key >= GLFW_KEY_F1 && key <= GLFW_KEY_F11)
            next = 10 + key - GLFW_KEY_F1;
        else if (key == GLFW_KEY_LEFT_BRACKET)
            next = activeMode_ == 1 ? lastStandardMode : activeMode_ - 1;
        else if (key == GLFW_KEY_RIGHT_BRACKET)
            next = activeMode_ == lastStandardMode ? 1 : activeMode_ + 1;
        else if ((activeMode_ <= 9 || (mods & GLFW_MOD_ALT)) && key >= GLFW_KEY_1 && key <= GLFW_KEY_9)
            next = key - GLFW_KEY_1 + 1;
        if (next) {
            if (!activateMode(next))
                app_.requestClose();
            return;
        }
    }
    if (key == GLFW_KEY_F12 && action == GLFW_PRESS) {
        printHelp();
        return;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app_.requestClose();
        return;
    }
    if (practiceMode_) {
        practiceMode_->onKey(key, action, mods);
        return;
    }
    if (key == GLFW_KEY_HOME && action == GLFW_PRESS) {
        pathId_.reset();
        if (!activateMode(activeMode_))
            app_.requestClose();
        return;
    }
    if (action == GLFW_REPEAT) {
        handleJointKey(key);
        return;
    }
    if (action != GLFW_PRESS)
        return;

    if (!initialMode_ && key == GLFW_KEY_0) {
        pathId_.reset();
        if (!activateMode(1))
            app_.requestClose();
        return;
    }

    if (key == GLFW_KEY_V) {
        threeViews_ = !threeViews_;

        return;
    }

    if (key == GLFW_KEY_D) {
        trace_ = !trace_;
        graphics::Diagnostics::configure({true, trace_});

        return;
    }

    if (handleJointKey(key))
        return;

    if (key == GLFW_KEY_F && demo_ == Demo::Room) {
        floorOpen_ = !floorOpen_;
        if (!updateRoom(0))
            app_.requestClose();

        return;
    }

    if (demo_ != Demo::Motion) {
        if (key == GLFW_KEY_SPACE)
            clock_.playing = !clock_.playing;
        else if (key == GLFW_KEY_ESCAPE)
            app_.requestClose();
        else if (key == GLFW_KEY_H)
            printHelp();

        return;
    }

    auto& motions = app_.getScene().getMotions();
    bool ok = true;

    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;

        ok = pathId_ && spinId_ && globalId_ && motions.setMotionPlaying(*pathId_, playing) &&
             motions.setMotionPlaying(*spinId_, playing) && motions.setMotionPlaying(*globalId_, playing);

        if (ok) {
            paused_ = !paused_;
            std::cout << (paused_ ? "시간 정지\n" : "시간 재개\n");
        }
    } else if (key == GLFW_KEY_S) {
        ok = spinId_ && motions.setMotionEnabled(*spinId_, !spinEnabled_);

        if (ok) {
            spinEnabled_ = !spinEnabled_;
            std::cout << "자전 기여: " << spinEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_G) {
        ok = globalId_ && motions.setMotionEnabled(*globalId_, !globalEnabled_);

        if (ok) {
            globalEnabled_ = !globalEnabled_;
            std::cout << "전체 공전 기여: " << globalEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_M) {
        ok = changeReflection();
    } else if (key == GLFW_KEY_I) {
        ok = toggleInsertedLayer();
    } else if (key == GLFW_KEY_H) {
        printHelp();
    }

    if (!ok) {
        std::cerr << "샘플 명령 적용 실패. 장면을 종료합니다.\n";
        app_.requestClose();
    }
}

/// 마우스 버튼 입력을 처리한다.
void Console::onMouse(const graphics::MouseEvent& event) {
    if (practiceMode_) {
        practiceMode_->onMouse(event);
        return;
    }
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }

    std::optional<graphics::PickResult> hit;

    if (threeViews_) {
        const auto size = app_.getFramebufferSize();

        if (size[0] < 3 || size[1] < 2)
            return;

        const auto passes =
            graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

        for (const auto& pass : passes) {
            const auto& v = pass.viewport;
            const double top = size[1] - v.y - v.height;

            if (event.framebufferX >= v.x && event.framebufferX < v.x + v.width && event.framebufferY >= top &&
                event.framebufferY < top + v.height)
                hit = graphics::pick(app_.getScene(), pass.camera, event.framebufferX, event.framebufferY,
                                     {double(v.x), top, double(v.width), double(v.height)}, pass.options.cull);
        }
    } else
        hit = app_.pickAt(event.windowX, event.windowY);

    selectPicked(hit);
}

/// 조작 방법을 출력한다.
void Console::printHelp() const {
    std::cout
        << "standard: initialMode=nullopt이면 모드 전환, 정수이면 해당 모드 고정\n"
        << "[ / ] 전체 1~22 모드 순환, Alt+1~9 데모 선택, F12 공통 도움말\n"
        << "F1~F11: 모드 10~20 = 실습 13~23 (각 실습의 기존 키 사용)\n"
        << "모드 21: WASD 카메라 충돌 / 모드 22: 내장 3D 메시 입력 처리\n"
        << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
        << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
        << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
        << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
        << "왼쪽 클릭: 사람·그네·상자 전체 선택 / 빈 공간: 선택 해제\n"
        << "7 Grid/거리 기반 배치 / 8 실내 미끄럼·낙하 / 0 기존 Motion 예제\n"
        << "9 선형보간·진자·사람형·그네→사람 계층 애니메이션\n"
        << "1~9: Home 전체 복원 / Tab/Backspace 객체·관절 선택, X/Y/Z 축, 좌우 회전, 상하 이동, PageUp/Down 피벗, +/- 균일 배율, R 복원\n"
        << "V 3뷰 전환 / D 단계 추적 로그 / F 실내 예제 바닥 개방 / Space 정지·재개\n";
}

bool Console::applyControls() {
    if (!graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds))
        return false;
    return demo_ == Demo::Grid ? updateGrid() : demo_ == Demo::Room ? updateRoom(0) : true;
}

void Console::onUpdate(float dt) {
    if (practiceMode_) {
        practiceMode_->onUpdate(dt);
        return;
    }
    if (demo_ != Demo::Motion && !clock_.advance(dt)) {
        app_.requestClose();
        return;
    }
    bool ok = demo_ == Demo::Animation ? updateAnimation()
        : graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds);
    if (demo_ == Demo::Grid)
        ok = updateGrid() && ok;
    else if (demo_ == Demo::Room)
        ok = updateRoom(clock_.playing ? dt : 0) && ok;
    if (!ok)
        app_.requestClose();
}

/// 현재 장면을 지정한 뷰로 렌더링한다.
void Console::onCursor(double x, double y) {
    if (practiceMode_)
        practiceMode_->onCursor(x, y);
}

void Console::onFocus(bool focused) {
    if (practiceMode_)
        practiceMode_->onFocus(focused);
}

bool Console::onRender() {
    if (practiceMode_)
        return practiceMode_->onRender();
    if (!threeViews_)
        return app_.renderScene();

    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    return app_.renderViews(
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions()));
}
```

## standard/Console.h

### before

```cpp
#pragma once
#include "framework/Framework.h"

class Console {
    graphics::Application& app_;
    std::optional<graphics::MotionId> pathId_;
    std::optional<graphics::MotionId> spinId_;
    std::optional<graphics::MotionId> globalId_;
    std::optional<graphics::LayerId> mirrorLayer_;
    std::optional<graphics::LayerId> normalSpinLayer_;
    std::optional<graphics::LayerId> insertedLayer_;
    bool paused_ = false;
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;

    enum class Demo { Motion, Grid, Room, Animation };

    Demo demo_ = Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_ = 0, roomCube_ = 0;
    std::vector<graphics::JointAnimation> animation_;

    struct JointControl {
        std::size_t track = 0;
        std::string name;
        graphics::Transform3D initialRest;
    };

    std::vector<JointControl> jointControls_;
    std::size_t selectedJoint_ = 0;
    int selectedAxis_ = 0;


    /// 선택한 관절의 기준 SRT와 피벗을 키 입력으로 편집한다.
    bool handleJointKey(int key);


    /// 현재 선택 관절과 조작 축을 콘솔에 출력한다.
    void printSelectedJoint() const;


    bool threeViews_ = false, trace_ = false, floorOpen_ = false;


    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();


    /// 회전하는 방 예제를 구성한다.
    bool initRoom();


    /// 선형보간·그네·사람 관절의 계층 애니메이션 예제를 구성한다.
    bool initAnimation();


    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);


    /// 반사 변환을 순서대로 변경한다.
    bool changeReflection();


    /// 크기 레이어를 삽입하거나 제거한다.
    bool toggleInsertedLayer();


    /// 조작 방법을 출력한다.
    void printHelp() const;


public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app) : app_(app) {
    }


    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();


    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action);


    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);


    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);


    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
};
```

### after

```cpp
#pragma once
#include "framework/Framework.h"
#include "modes/PracticeMode.h"

class Console {
    graphics::Application& app_;
    std::optional<int> initialMode_;
    int activeMode_ = 1;
    std::unique_ptr<PracticeMode> practiceMode_;
    bool activateMode(int mode);
    bool initMotion(int kind);
    bool updateGrid();
    bool updateRoom(float dt);
    bool updateAnimation();
    std::optional<graphics::MotionId> pathId_;
    std::optional<graphics::MotionId> spinId_;
    std::optional<graphics::MotionId> globalId_;
    std::optional<graphics::LayerId> mirrorLayer_;
    std::optional<graphics::LayerId> normalSpinLayer_;
    std::optional<graphics::LayerId> insertedLayer_;
    bool paused_ = false;
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;

    enum class Demo { Motion, Grid, Room, Animation };

    Demo demo_ = Demo::Motion;
    graphics::AnimationClock clock_;
    graphics::DistancePath distancePath_;
    std::vector<graphics::EntityId> gridBoxes_;
    graphics::RoomBody roomBody_;
    graphics::EntityId room_ = 0, roomCube_ = 0;
    std::vector<graphics::JointAnimation> animation_;

    enum class ControlKind { Object, Joint, RoomFrame, RoomBody };

    struct JointControl {
        std::size_t track = 0;
        std::string name;
        graphics::Transform3D initialRest;
        ControlKind kind = ControlKind::Object;
    };

    std::vector<JointControl> jointControls_;
    std::optional<std::size_t> roomControl_, bodyControl_;
    graphics::EntityId roomFrame_ = 0, bodyFrame_ = 0;
    glm::mat3 releasedRotation_{1};
    bool addControl(graphics::EntityId id, const std::string& name, ControlKind kind = ControlKind::Object);
    bool applyControls();
    void selectPicked(std::optional<graphics::PickResult> hit);
    std::size_t selectedJoint_ = 0;
    int selectedAxis_ = 0;


    /// 선택한 관절의 기준 SRT와 피벗을 키 입력으로 편집한다.
    bool handleJointKey(int key);


    /// 현재 선택 관절과 조작 축을 콘솔에 출력한다.
    void printSelectedJoint() const;


    bool threeViews_ = false, trace_ = false, floorOpen_ = false;


    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();


    /// 회전하는 방 예제를 구성한다.
    bool initRoom();


    /// 선형보간·그네·사람 관절의 계층 애니메이션 예제를 구성한다.
    bool initAnimation();


    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);


    /// 반사 변환을 순서대로 변경한다.
    bool changeReflection();


    /// 크기 레이어를 삽입하거나 제거한다.
    bool toggleInsertedLayer();


    /// 조작 방법을 출력한다.
    void printHelp() const;


public:
    /// 애플리케이션 참조를 보관한다.
    explicit Console(graphics::Application& app, std::optional<int> initialMode = std::nullopt)
        : app_(app), initialMode_(initialMode) {
    }


    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();


    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods = 0);
    void onCursor(double x, double y);
    void onFocus(bool focused);


    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);


    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);


    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
};
```

## standard/framework/detail/Application.cpp

### before

```cpp
#include "../Framework.h"
#include "Internal.h"
#include <algorithm>
#include <iostream>
#include <utility>

namespace graphics::detail {
bool& applicationActive() {
    static bool active = false;

    return active;
}

unsigned& contextErrorSerial() {
    static unsigned value = 0;

    return value;
}
}

namespace graphics {

Application::~Application() {
    shutdown();
}

bool Application::configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath,
                                    const std::filesystem::path& fragmentPath) {
    if (!ensureContext("configureRenderer", __FILE__, __LINE__))
        return false;
    if (renderer_.initialize(vertexPath, fragmentPath, config))
        return true;
    requestClose();
    return false;
}

void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

void Application::setModifiedKeyHandler(std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::ensureContext(const char* operation, const char* file, int line) {
    Diagnostics::locate(file, line, operation);

    if (contextFailed_)
        return false;

    if (!window_ || GetCurrentThreadId() != contextThread_) {
        contextFailed_ = true;

        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::ensureContext",
            "창이 없거나 컨텍스트를 생성한 스레드와 다른 스레드에서 사용했습니다.",
            "창 생성·렌더링·종료를 같은 스레드에서 수행하세요. Application::run과 사용자 렌더 콜백을 확인하세요.");
    }
    using CurrentProc = void*(WINAPI*)();

    const auto module = GetModuleHandleW(L"opengl32.dll");
    const auto current =
        module ? reinterpret_cast<CurrentProc>(::GetProcAddress(module, "wglGetCurrentContext")) : nullptr;

    if (nativeContext_ && detail::glfw().GetCurrentContext() == window_ && current && current() == nativeContext_)
        return true;

    const auto serial = detail::contextErrorSerial();

    detail::glfw().MakeContextCurrent(window_);

    if (serial != detail::contextErrorSerial() || detail::glfw().GetCurrentContext() != window_ || !current ||
        !current()) {
        contextFailed_ = true;

        return Diagnostics::fail(
            PipelineStage::Runtime, "glfwMakeContextCurrent",
            "WGL이 요청한 컨텍스트를 연결하지 못하여 GL 명령 전에 렌더링을 중단했습니다.",
            "소스 위치는 연결 실패 호출입니다. 창 수명·렌더 스레드·컨텍스트를 해제/파괴하는 외부 콜백을 먼저 확인하세요. 이상이 없으면 GPU 드라이버·세션을 확인하세요. 이 로그만으로 드라이버 원인을 확정할 수 없습니다.");
    }
    nativeContext_ = current();

    return true;
}

bool Application::initialize(const ApplicationConfig& config) {
    contextThread_ = GetCurrentThreadId();
    contextFailed_ = false;
    nativeContext_ = nullptr;
    Diagnostics::trace(PipelineStage::Runtime, "Application::initialize");

    const auto root = detail::relativePathInside(detail::executableDirectory(), config.projectRoot);
    const auto runtime = detail::relativePathInside(root, config.runtimeDirectory);

    detail::glfw().load(runtime);
    detail::glfw().SetErrorCallback([](int code, const char* message) {
        ++detail::contextErrorSerial();
        Diagnostics::fail(
            PipelineStage::Runtime, "GLFW/WGL", std::to_string(code) + ": " + (message ? message : "알 수 없는 오류"),
            "표시된 소스 위치·호출 경로에서 컨텍스트 연결이 실패했습니다. Application의 창 수명과 스레드를 먼저 확인하세요. OS/드라이버 원문도 함께 보세요. 렌더 패스마다 별도 WGL 컨텍스트를 만들 필요는 없습니다.");
    });

    if (config.width <= 0 || config.height <= 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::initialize",
                                   "창 너비와 높이는 양수여야 합니다.");

    if (!detail::glfw().Init())
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwInit",
                                   "GLFW 초기화 실패: 앞선 GLFW 오류를 확인하세요.");
    glfwInitialized_ = true;
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    detail::glfw().WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    detail::glfw().WindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    detail::glfw().WindowHint(GLFW_DEPTH_BITS, 24);
    window_ = detail::glfw().createWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);

    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwCreateWindow",
                                   "OpenGL 3.3 코어 창 생성 실패.",
                                   "앞선 GLFW 오류와 그래픽 드라이버의 OpenGL 지원을 확인하세요.");
    }

    if (!ensureContext("initialize -> attach new window", __FILE__, __LINE__))
        return false;
    detail::glfw().SwapInterval(config.verticalSync ? 1 : 0);

    const auto vertexPath = config.shaderDirectory.empty()
                                ? std::filesystem::path{}
                                : detail::relativePathInside(root, config.shaderDirectory) / "VertexShader.glsl";
    const auto fragmentPath = config.shaderDirectory.empty()
                                  ? std::filesystem::path{}
                                  : detail::relativePathInside(root, config.shaderDirectory) / "FragmentShader.glsl";

    Diagnostics::locate(__FILE__, __LINE__, "initialize -> Renderer::initialize -> shader source/path setup");

    if (!renderer_.initialize(vertexPath, fragmentPath, config.renderConfig))
        return false;
    assets_.emplace(root, config.features);
    renderOptions_.textures = config.features.textures;

    detail::glfw().SetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    detail::glfw().SetWindowUserPointer(window_, this);
    detail::glfw().SetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.modifiedKeyHandler_)
                app.modifiedKeyHandler_(app, key, action, mods);
            else if (app.keyHandler_)
                app.keyHandler_(app, key, action);
        });
    });
    detail::glfw().SetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.focusHandler_)
                app.focusHandler_(app, focused == GLFW_TRUE);
        });
    });
    detail::glfw().SetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (!app.mouseHandler_)
                return;

            MouseEvent event;

            event.button = button;
            event.action = action;
            event.mods = mods;
            detail::glfw().GetCursorPos(window, &event.windowX, &event.windowY);

            int ww = 0, wh = 0, fw = 0, fh = 0;

            detail::glfw().GetWindowSize(window, &ww, &wh);
            detail::glfw().GetFramebufferSize(window, &fw, &fh);

            if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
                return;
            event.framebufferX = event.windowX * fw / ww;
            event.framebufferY = event.windowY * fh / wh;
            app.mouseHandler_(app, event);
        });
    });
    detail::glfw().SetCursorPosCallback(window_, [](GLFWwindow* window, double x, double y) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.cursorHandler_)
                app.cursorHandler_(app, x, y);
        });
    });

    return true;
}

void Application::shutdown() {
    assets_.reset();

    if (window_ != nullptr) {
        if (!contextFailed_ && ensureContext("shutdown -> GPU cleanup", __FILE__, __LINE__))
            renderer_.destroy();
        else
            renderer_.abandon();
        detail::glfw().SetWindowUserPointer(window_, nullptr);
        detail::glfw().DestroyWindow(window_);
        window_ = nullptr;
    }

    if (glfwInitialized_) {
        detail::glfw().Terminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        detail::glfw().SetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderScene",
                                   "활성 창 또는 컨텍스트가 없습니다.");
    }

    if (!ensureContext("renderScene -> single pass", __FILE__, __LINE__))
        return false;

    int width = 0;
    int height = 0;

    detail::glfw().GetFramebufferSize(window_, &width, &height);

    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

std::array<int, 2> Application::getFramebufferSize() const {
    std::array<int, 2> size{};

    if (window_)
        detail::glfw().GetFramebufferSize(window_, &size[0], &size[1]);

    return size;
}

bool Application::renderViews(const std::vector<RenderPass>& passes) {
    if (!window_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderViews",
                                   "활성 창 또는 컨텍스트가 없습니다.");

    if (!ensureContext("renderViews -> shared context for all viewports", __FILE__, __LINE__))
        return false;

    const auto size = getFramebufferSize();

    return renderer_.renderViews(scene_, passes, size[0], size[1]);
}

int Application::run(const ApplicationConfig& config) {
    Diagnostics::configure(config.diagnostics);
    Diagnostics::clear();

    if (running_) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "run() 재귀 호출은 허용하지 않습니다.");

        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }

    if (detail::applicationActive()) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "동시에 실행 중인 Application은 하나만 지원합니다.");

        return -1;
    }
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    detail::applicationActive() = true;
    callbackError_ = nullptr;
    running_ = true;

    int exitCode = 0;

    try {
        if (!initialize(config)) {
            std::cerr << "애플리케이션 초기화 실패.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Application::initHandler",
                                "사용자 초기화 콜백이 false를 반환했습니다.",
                                "앞선 API 오류와 장면 생성 콜백을 확인하세요.");
            std::cerr << "사용자 장면 초기화 실패.\n";
            exitCode = -1;
        } else {
            double previousTime = detail::glfw().GetTime();

            while (!detail::glfw().WindowShouldClose(window_)) {
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwPollEvents/input callbacks");
                detail::glfw().PollEvents();

                if (callbackError_)
                    std::rethrow_exception(callbackError_);

                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }

                int width = 0;
                int height = 0;

                detail::glfw().GetFramebufferSize(window_, &width, &height);

                if (width <= 0 || height <= 0) {
                    detail::glfw().WaitEvents();

                    if (callbackError_)
                        std::rethrow_exception(callbackError_);
                    previousTime = detail::glfw().GetTime();
                    continue;
                }

                const double currentTime = detail::glfw().GetTime();
                const float deltaTime = static_cast<float>(std::clamp(currentTime - previousTime, 0.0, 0.05));

                previousTime = currentTime;

                if (updateHandler_) {
                    Diagnostics::trace(PipelineStage::Input, "Application::updateHandler");
                    updateHandler_(*this, deltaTime);
                }

                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }

                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "모션 갱신 실패.\n";
                    exitCode = -1;
                    break;
                }

                if (!ensureContext("run -> render callback", __FILE__, __LINE__)) {
                    exitCode = -1;
                    break;
                }

                const bool rendered = renderHandler_ ? renderHandler_(*this) : renderScene();

                if (!rendered) {
                    std::cerr << "렌더링 실패.\n";
                    exitCode = -1;
                    break;
                }
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwSwapBuffers");

                const auto swapSerial = detail::contextErrorSerial();

                detail::glfw().SwapBuffers(window_);

                if (swapSerial != detail::contextErrorSerial()) {
                    contextFailed_ = true;
                    exitCode = -1;
                    break;
                }
            }
        }
    } catch (const std::exception& error) {
        Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::run", error.what(),
            "앞선 파이프라인 단계/API 오류를 확인하세요. 사용자 콜백 예외는 자원 정리 후 다시 전달됩니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw;
    } catch (...) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "콜백 또는 실행 처리에서 표준 예외가 아닌 예외가 발생했습니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    detail::applicationActive() = false;

    return exitCode;
}

void Application::setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler) {
    mouseHandler_ = std::move(handler);
}

void Application::setCursorHandler(std::function<void(Application&, double, double)> handler) {
    cursorHandler_ = std::move(handler);
}

std::optional<PickResult> Application::pickAt(double x, double y) const {
    if (!window_)
        return {};

    int ww = 0, wh = 0, fw = 0, fh = 0;

    detail::glfw().GetWindowSize(window_, &ww, &wh);
    detail::glfw().GetFramebufferSize(window_, &fw, &fh);

    if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
        return {};

    return graphics::pick(scene_, camera_, x * fw / ww, y * fh / wh, {0, 0, double(fw), double(fh)});
}

std::optional<PickResult> Application::pickAtCursor() const {
    if (!window_)
        return {};

    double x = 0, y = 0;

    detail::glfw().GetCursorPos(window_, &x, &y);

    return pickAt(x, y);
}

AssetLoader& Application::getAssets() {
    if (!assets_)
        throw std::logic_error("AssetLoader는 run 실행 중 콜백에서만 사용할 수 있습니다.");

    return *assets_;
}
} // namespace graphics
```

### after

```cpp
#include "../Framework.h"
#include "Internal.h"
#include <algorithm>
#include <iostream>
#include <utility>

namespace graphics::detail {
bool& applicationActive() {
    static bool active = false;

    return active;
}

unsigned& contextErrorSerial() {
    static unsigned value = 0;

    return value;
}
}

namespace graphics {

Application::~Application() {
    shutdown();
}

bool Application::configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath,
                                    const std::filesystem::path& fragmentPath) {
    if (!ensureContext("configureRenderer", __FILE__, __LINE__))
        return false;
    if (renderer_.initialize(vertexPath, fragmentPath, config))
        return true;
    requestClose();
    return false;
}

void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

void Application::setModifiedKeyHandler(std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::ensureContext(const char* operation, const char* file, int line) {
    Diagnostics::locate(file, line, operation);

    if (contextFailed_)
        return false;

    if (!window_ || GetCurrentThreadId() != contextThread_) {
        contextFailed_ = true;

        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::ensureContext",
            "창이 없거나 컨텍스트를 생성한 스레드와 다른 스레드에서 사용했습니다.",
            "창 생성·렌더링·종료를 같은 스레드에서 수행하세요. Application::run과 사용자 렌더 콜백을 확인하세요.");
    }
    using CurrentProc = void*(WINAPI*)();

    const auto module = GetModuleHandleW(L"opengl32.dll");
    const auto current =
        module ? reinterpret_cast<CurrentProc>(::GetProcAddress(module, "wglGetCurrentContext")) : nullptr;

    if (nativeContext_ && detail::glfw().GetCurrentContext() == window_ && current && current() == nativeContext_)
        return true;

    const auto serial = detail::contextErrorSerial();

    detail::glfw().MakeContextCurrent(window_);

    if (serial != detail::contextErrorSerial() || detail::glfw().GetCurrentContext() != window_ || !current ||
        !current()) {
        contextFailed_ = true;

        return Diagnostics::fail(
            PipelineStage::Runtime, "glfwMakeContextCurrent",
            "WGL이 요청한 컨텍스트를 연결하지 못하여 GL 명령 전에 렌더링을 중단했습니다.",
            "소스 위치는 연결 실패 호출입니다. 창 수명·렌더 스레드·컨텍스트를 해제/파괴하는 외부 콜백을 먼저 확인하세요. 이상이 없으면 GPU 드라이버·세션을 확인하세요. 이 로그만으로 드라이버 원인을 확정할 수 없습니다.");
    }
    nativeContext_ = current();

    return true;
}

bool Application::initialize(const ApplicationConfig& config) {
    contextThread_ = GetCurrentThreadId();
    contextFailed_ = false;
    nativeContext_ = nullptr;
    Diagnostics::trace(PipelineStage::Runtime, "Application::initialize");

    const auto root = detail::relativePathInside(detail::executableDirectory(), config.projectRoot);
    const auto runtime = detail::relativePathInside(root, config.runtimeDirectory);

    detail::glfw().load(runtime);
    detail::glfw().SetErrorCallback([](int code, const char* message) {
        ++detail::contextErrorSerial();
        Diagnostics::fail(
            PipelineStage::Runtime, "GLFW/WGL", std::to_string(code) + ": " + (message ? message : "알 수 없는 오류"),
            "표시된 소스 위치·호출 경로에서 컨텍스트 연결이 실패했습니다. Application의 창 수명과 스레드를 먼저 확인하세요. OS/드라이버 원문도 함께 보세요. 렌더 패스마다 별도 WGL 컨텍스트를 만들 필요는 없습니다.");
    });

    if (config.width <= 0 || config.height <= 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::initialize",
                                   "창 너비와 높이는 양수여야 합니다.");

    if (!detail::glfw().Init())
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwInit",
                                   "GLFW 초기화 실패: 앞선 GLFW 오류를 확인하세요.");
    glfwInitialized_ = true;
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    detail::glfw().WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    detail::glfw().WindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    detail::glfw().WindowHint(GLFW_DEPTH_BITS, 24);
    window_ = detail::glfw().createWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);

    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwCreateWindow",
                                   "OpenGL 3.3 코어 창 생성 실패.",
                                   "앞선 GLFW 오류와 그래픽 드라이버의 OpenGL 지원을 확인하세요.");
    }

    if (!ensureContext("initialize -> attach new window", __FILE__, __LINE__))
        return false;
    detail::glfw().SwapInterval(config.verticalSync ? 1 : 0);

    const auto vertexPath = config.shaderDirectory.empty()
                                ? std::filesystem::path{}
                                : detail::relativePathInside(root, config.shaderDirectory) / "VertexShader.glsl";
    const auto fragmentPath = config.shaderDirectory.empty()
                                  ? std::filesystem::path{}
                                  : detail::relativePathInside(root, config.shaderDirectory) / "FragmentShader.glsl";

    Diagnostics::locate(__FILE__, __LINE__, "initialize -> Renderer::initialize -> shader source/path setup");

    if (!renderer_.initialize(vertexPath, fragmentPath, config.renderConfig))
        return false;
    assets_.emplace(root, config.features);
    renderOptions_.textures = config.features.textures;

    detail::glfw().SetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    detail::glfw().SetWindowUserPointer(window_, this);
    detail::glfw().SetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.modifiedKeyHandler_)
                app.modifiedKeyHandler_(app, key, action, mods);
            else if (app.keyHandler_)
                app.keyHandler_(app, key, action);
        });
    });
    detail::glfw().SetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.focusHandler_)
                app.focusHandler_(app, focused == GLFW_TRUE);
        });
    });
    detail::glfw().SetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (!app.mouseHandler_)
                return;

            MouseEvent event;

            event.button = button;
            event.action = action;
            event.mods = mods;
            detail::glfw().GetCursorPos(window, &event.windowX, &event.windowY);

            int ww = 0, wh = 0, fw = 0, fh = 0;

            detail::glfw().GetWindowSize(window, &ww, &wh);
            detail::glfw().GetFramebufferSize(window, &fw, &fh);

            if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
                return;
            event.framebufferX = event.windowX * fw / ww;
            event.framebufferY = event.windowY * fh / wh;
            app.mouseHandler_(app, event);
        });
    });
    detail::glfw().SetCursorPosCallback(window_, [](GLFWwindow* window, double x, double y) {
        auto& app = *static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.cursorHandler_)
                app.cursorHandler_(app, x, y);
        });
    });

    return true;
}

void Application::shutdown() {
    assets_.reset();

    if (window_ != nullptr) {
        if (!contextFailed_ && ensureContext("shutdown -> GPU cleanup", __FILE__, __LINE__))
            renderer_.destroy();
        else
            renderer_.abandon();
        detail::glfw().SetWindowUserPointer(window_, nullptr);
        detail::glfw().DestroyWindow(window_);
        window_ = nullptr;
    }

    if (glfwInitialized_) {
        detail::glfw().Terminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        detail::glfw().SetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderScene",
                                   "활성 창 또는 컨텍스트가 없습니다.");
    }

    if (!ensureContext("renderScene -> single pass", __FILE__, __LINE__))
        return false;

    int width = 0;
    int height = 0;

    detail::glfw().GetFramebufferSize(window_, &width, &height);

    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

std::array<int, 2> Application::getFramebufferSize() const {
    std::array<int, 2> size{};

    if (window_)
        detail::glfw().GetFramebufferSize(window_, &size[0], &size[1]);

    return size;
}

bool Application::renderViews(const std::vector<RenderPass>& passes) {
    if (!window_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderViews",
                                   "활성 창 또는 컨텍스트가 없습니다.");

    if (!ensureContext("renderViews -> shared context for all viewports", __FILE__, __LINE__))
        return false;

    const auto size = getFramebufferSize();

    return renderer_.renderViews(scene_, passes, size[0], size[1]);
}

int Application::run(const ApplicationConfig& config) {
    Diagnostics::configure(config.diagnostics);
    Diagnostics::clear();

    if (running_) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "run() 재귀 호출은 허용하지 않습니다.");

        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }

    if (detail::applicationActive()) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "동시에 실행 중인 Application은 하나만 지원합니다.");

        return -1;
    }
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    detail::applicationActive() = true;
    callbackError_ = nullptr;
    running_ = true;

    int exitCode = 0;

    try {
        if (!initialize(config)) {
            std::cerr << "애플리케이션 초기화 실패.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Application::initHandler",
                                "사용자 초기화 콜백이 false를 반환했습니다.",
                                "앞선 API 오류와 장면 생성 콜백을 확인하세요.");
            std::cerr << "사용자 장면 초기화 실패.\n";
            exitCode = -1;
        } else {
            double previousTime = detail::glfw().GetTime();

            while (!detail::glfw().WindowShouldClose(window_)) {
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwPollEvents/input callbacks");
                detail::glfw().PollEvents();

                if (callbackError_)
                    std::rethrow_exception(callbackError_);

                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }

                int width = 0;
                int height = 0;

                detail::glfw().GetFramebufferSize(window_, &width, &height);

                if (width <= 0 || height <= 0) {
                    detail::glfw().WaitEvents();

                    if (callbackError_)
                        std::rethrow_exception(callbackError_);
                    previousTime = detail::glfw().GetTime();
                    continue;
                }

                const double currentTime = detail::glfw().GetTime();
                const float deltaTime = static_cast<float>(std::clamp(currentTime - previousTime, 0.0, 0.05));

                previousTime = currentTime;

                if (updateHandler_) {
                    Diagnostics::trace(PipelineStage::Input, "Application::updateHandler");
                    updateHandler_(*this, deltaTime);
                }

                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }

                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "모션 갱신 실패.\n";
                    exitCode = -1;
                    break;
                }

                if (!ensureContext("run -> render callback", __FILE__, __LINE__)) {
                    exitCode = -1;
                    break;
                }

                const bool rendered = renderHandler_ ? renderHandler_(*this) : renderScene();

                if (!rendered) {
                    std::cerr << "렌더링 실패.\n";
                    exitCode = -1;
                    break;
                }
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwSwapBuffers");

                const auto swapSerial = detail::contextErrorSerial();

                detail::glfw().SwapBuffers(window_);

                if (swapSerial != detail::contextErrorSerial()) {
                    contextFailed_ = true;
                    exitCode = -1;
                    break;
                }
            }
        }
    } catch (const std::exception& error) {
        Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::run", error.what(),
            "앞선 파이프라인 단계/API 오류를 확인하세요. 사용자 콜백 예외는 자원 정리 후 다시 전달됩니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw;
    } catch (...) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "콜백 또는 실행 처리에서 표준 예외가 아닌 예외가 발생했습니다.");
        shutdown();
        running_ = false;
        detail::applicationActive() = false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    detail::applicationActive() = false;

    return exitCode;
}

void Application::setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler) {
    mouseHandler_ = std::move(handler);
}

void Application::setCursorHandler(std::function<void(Application&, double, double)> handler) {
    cursorHandler_ = std::move(handler);
}

std::optional<PickResult> Application::pickAt(double x, double y) const {
    if (!window_)
        return {};

    int ww = 0, wh = 0, fw = 0, fh = 0;

    detail::glfw().GetWindowSize(window_, &ww, &wh);
    detail::glfw().GetFramebufferSize(window_, &fw, &fh);

    if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
        return {};

    return graphics::pick(scene_, camera_, x * fw / ww, y * fh / wh, {0, 0, double(fw), double(fh)},
                          renderOptions_.cull);
}

std::optional<PickResult> Application::pickAtCursor() const {
    if (!window_)
        return {};

    double x = 0, y = 0;

    detail::glfw().GetCursorPos(window_, &x, &y);

    return pickAt(x, y);
}

AssetLoader& Application::getAssets() {
    if (!assets_)
        throw std::logic_error("AssetLoader는 run 실행 중 콜백에서만 사용할 수 있습니다.");

    return *assets_;
}
} // namespace graphics
```

## standard/framework/include/Picking.h

### before

```cpp
#pragma once
#include "Graphics.h"

namespace graphics {
struct Ray {
    glm::vec3 origin{0}, direction{0, 0, -1};
    float maxDistance = std::numeric_limits<float>::infinity();
};

struct PickResult {
    EntityId entityId = 0;
    std::size_t faceIndex = 0, triangleIndex = 0;
    glm::vec3 worldPoint{0}, localPoint{0}, barycentric{0};
    glm::vec2 uv{0};
    bool hasUV = false;
    float distance = 0; // Distance from ray origin (near plane for a screen ray).
};

struct PickViewport {
    double x = 0, y = 0, width = 0, height = 0;
}; // top-left framebuffer pixels


/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
inline std::optional<Ray> screenRay(const Camera& camera, double x, double y, const PickViewport& viewport) {
    if (!CameraSystem::validCamera(camera) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(viewport.x) ||
        !std::isfinite(viewport.y) || !std::isfinite(viewport.width) || !std::isfinite(viewport.height) ||
        viewport.width <= 0 || viewport.height <= 0 || x < viewport.x || y < viewport.y ||
        x >= viewport.x + viewport.width || y >= viewport.y + viewport.height)
        return {};

    const auto pv = CameraSystem::projectionMatrix(camera, float(viewport.width / viewport.height)) *
                    CameraSystem::viewMatrix(camera);
    const auto determinant = glm::determinant(pv);

    if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
        return {};

    const auto inverse = glm::inverse(pv);
    const float nx = float(2 * (x - viewport.x) / viewport.width - 1),
                ny = float(1 - 2 * (y - viewport.y) / viewport.height);
    auto nearPoint = inverse * glm::vec4(nx, ny, -1, 1), farPoint = inverse * glm::vec4(nx, ny, 1, 1);

    if (std::abs(nearPoint.w) < 1e-12f || std::abs(farPoint.w) < 1e-12f)
        return {};
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    const auto direction = glm::vec3(farPoint - nearPoint);
    const auto length = glm::length(direction);

    if (!detail::finiteVector(glm::vec3(nearPoint)) || !detail::finiteVector(direction) || !std::isfinite(length) ||
        length <= 0)
        return {};

    return Ray{glm::vec3(nearPoint), direction / length, length};
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Ray& input) {
    if (!detail::finiteVector(input.origin) || !detail::finiteVector(input.direction) ||
        !std::isfinite(glm::length(input.direction)) || glm::length(input.direction) < 1e-12f ||
        std::isnan(input.maxDistance) || input.maxDistance < 0)
        return {};

    const auto direction = glm::dvec3(glm::normalize(input.direction));
    const auto origin = glm::dvec3(input.origin);
    double nearest = input.maxDistance;
    std::optional<PickResult> result;

    for (auto id : scene.getIds()) {
        const auto* component = scene.getComponent(id);

        if (!component || !component->visible)
            continue;

        for (std::size_t f = 0; f < component->polyhedron.faces.size(); ++f) {
            const auto& face = component->polyhedron.faces[f];

            if (!face.visible)
                continue;

            const auto world = scene.faceWorldMatrix(id, f);

            if (!world)
                continue;

            const auto matrix = *world;
            const auto indices = detail::triangles(face);

            for (std::size_t t = 0; t < indices.size(); t += 3) {
                const auto ia = indices[t], ib = indices[t + 1], ic = indices[t + 2];
                const auto a4 = matrix * glm::vec4(face.localVertices[ia], 1),
                           b4 = matrix * glm::vec4(face.localVertices[ib], 1),
                           c4 = matrix * glm::vec4(face.localVertices[ic], 1);

                if (std::abs(a4.w) < 1e-12f || std::abs(b4.w) < 1e-12f || std::abs(c4.w) < 1e-12f)
                    continue;

                const auto a = glm::dvec3(a4 / a4.w), b = glm::dvec3(b4 / b4.w), c = glm::dvec3(c4 / c4.w);
                const auto edge1 = b - a, edge2 = c - a, p = glm::cross(direction, edge2);
                const auto determinant = glm::dot(edge1, p);
                const auto tolerance = 1e-12 * glm::length(edge1) * glm::length(edge2);

                if (!std::isfinite(determinant) || std::abs(determinant) <= tolerance)
                    continue;

                const auto relative = origin - a;
                const auto u = glm::dot(relative, p) / determinant;

                if (u < 0 || u > 1)
                    continue;

                const auto q = glm::cross(relative, edge1);
                const auto v = glm::dot(direction, q) / determinant;

                if (v < 0 || u + v > 1)
                    continue;

                const auto distance = glm::dot(edge2, q) / determinant;

                if (!std::isfinite(distance) || distance < 0 || distance > nearest)
                    continue;

                const glm::dvec3 weights((1 - u - v) / a4.w, u / b4.w, v / c4.w);
                const auto sum = weights.x + weights.y + weights.z;

                if (std::abs(sum) < 1e-12)
                    continue;

                PickResult hit;

                hit.entityId = id;
                hit.faceIndex = f;
                hit.triangleIndex = t / 3;
                hit.distance = float(distance);
                hit.worldPoint = glm::vec3(origin + direction * distance);
                hit.barycentric = glm::vec3(weights / sum);
                hit.localPoint = face.localVertices[ia] * hit.barycentric.x +
                                 face.localVertices[ib] * hit.barycentric.y +
                                 face.localVertices[ic] * hit.barycentric.z;
                hit.hasUV = !face.texcoords.empty();

                if (hit.hasUV)
                    hit.uv = face.texcoords[ia] * hit.barycentric.x + face.texcoords[ib] * hit.barycentric.y +
                             face.texcoords[ic] * hit.barycentric.z;
                nearest = distance;
                result = hit;
            }
        }
    }

    return result;
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
                                      const PickViewport& viewport) {
    const auto ray = screenRay(camera, x, y, viewport);

    return ray ? pick(scene, *ray) : std::nullopt;
}
} // namespace graphics
```

### after

```cpp
#pragma once
#include "Graphics.h"

namespace graphics {
struct Ray {
    glm::vec3 origin{0}, direction{0, 0, -1};
    float maxDistance = std::numeric_limits<float>::infinity();
};

struct PickResult {
    EntityId entityId = 0;
    std::size_t faceIndex = 0, triangleIndex = 0;
    glm::vec3 worldPoint{0}, localPoint{0}, barycentric{0};
    glm::vec2 uv{0};
    bool hasUV = false;
    float distance = 0; // Distance from ray origin (near plane for a screen ray).
};

struct PickViewport {
    double x = 0, y = 0, width = 0, height = 0;
}; // top-left framebuffer pixels


/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
inline std::optional<Ray> screenRay(const Camera& camera, double x, double y, const PickViewport& viewport) {
    if (!CameraSystem::validCamera(camera) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(viewport.x) ||
        !std::isfinite(viewport.y) || !std::isfinite(viewport.width) || !std::isfinite(viewport.height) ||
        viewport.width <= 0 || viewport.height <= 0 || x < viewport.x || y < viewport.y ||
        x >= viewport.x + viewport.width || y >= viewport.y + viewport.height)
        return {};

    const auto pv = CameraSystem::projectionMatrix(camera, float(viewport.width / viewport.height)) *
                    CameraSystem::viewMatrix(camera);
    const auto determinant = glm::determinant(pv);

    if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
        return {};

    const auto inverse = glm::inverse(pv);
    const float nx = float(2 * (x - viewport.x) / viewport.width - 1),
                ny = float(1 - 2 * (y - viewport.y) / viewport.height);
    auto nearPoint = inverse * glm::vec4(nx, ny, -1, 1), farPoint = inverse * glm::vec4(nx, ny, 1, 1);

    if (std::abs(nearPoint.w) < 1e-12f || std::abs(farPoint.w) < 1e-12f)
        return {};
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    const auto direction = glm::vec3(farPoint - nearPoint);
    const auto length = glm::length(direction);

    if (!detail::finiteVector(glm::vec3(nearPoint)) || !detail::finiteVector(direction) || !std::isfinite(length) ||
        length <= 0)
        return {};

    return Ray{glm::vec3(nearPoint), direction / length, length};
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Ray& input, CullMode inheritedCull = CullMode::None) {
    if (!detail::finiteVector(input.origin) || !detail::finiteVector(input.direction) ||
        !std::isfinite(glm::length(input.direction)) || glm::length(input.direction) < 1e-12f ||
        std::isnan(input.maxDistance) || input.maxDistance < 0)
        return {};

    const auto direction = glm::dvec3(glm::normalize(input.direction));
    const auto origin = glm::dvec3(input.origin);
    double nearest = input.maxDistance;
    std::optional<PickResult> result;

    for (auto id : scene.getIds()) {
        const auto* component = scene.getComponent(id);

        if (!component || !component->visible)
            continue;

        for (std::size_t f = 0; f < component->polyhedron.faces.size(); ++f) {
            const auto& face = component->polyhedron.faces[f];

            if (!face.visible)
                continue;

            const auto world = scene.faceWorldMatrix(id, f);

            if (!world)
                continue;

            const auto matrix = *world;
            const auto cull = face.cull == CullMode::Inherit ? inheritedCull : face.cull;
            const bool mirrored = glm::determinant(glm::mat3(matrix)) < 0;
            const auto indices = detail::triangles(face);

            for (std::size_t t = 0; t < indices.size(); t += 3) {
                const auto ia = indices[t], ib = indices[t + 1], ic = indices[t + 2];
                const auto a4 = matrix * glm::vec4(face.localVertices[ia], 1),
                           b4 = matrix * glm::vec4(face.localVertices[ib], 1),
                           c4 = matrix * glm::vec4(face.localVertices[ic], 1);

                if (std::abs(a4.w) < 1e-12f || std::abs(b4.w) < 1e-12f || std::abs(c4.w) < 1e-12f)
                    continue;

                const auto a = glm::dvec3(a4 / a4.w), b = glm::dvec3(b4 / b4.w), c = glm::dvec3(c4 / c4.w);
                const auto edge1 = b - a, edge2 = c - a, p = glm::cross(direction, edge2);
                const auto determinant = glm::dot(edge1, p);
                const auto tolerance = 1e-12 * glm::length(edge1) * glm::length(edge2);

                if (!std::isfinite(determinant) || std::abs(determinant) <= tolerance)
                    continue;

                const bool frontFacing = (determinant > 0) != mirrored;
                if ((cull == CullMode::Front && frontFacing) || (cull == CullMode::Back && !frontFacing))
                    continue;

                const auto relative = origin - a;
                const auto u = glm::dot(relative, p) / determinant;

                if (u < 0 || u > 1)
                    continue;

                const auto q = glm::cross(relative, edge1);
                const auto v = glm::dot(direction, q) / determinant;

                if (v < 0 || u + v > 1)
                    continue;

                const auto distance = glm::dot(edge2, q) / determinant;

                if (!std::isfinite(distance) || distance < 0 || distance > nearest)
                    continue;

                const glm::dvec3 weights((1 - u - v) / a4.w, u / b4.w, v / c4.w);
                const auto sum = weights.x + weights.y + weights.z;

                if (std::abs(sum) < 1e-12)
                    continue;

                PickResult hit;

                hit.entityId = id;
                hit.faceIndex = f;
                hit.triangleIndex = t / 3;
                hit.distance = float(distance);
                hit.worldPoint = glm::vec3(origin + direction * distance);
                hit.barycentric = glm::vec3(weights / sum);
                hit.localPoint = face.localVertices[ia] * hit.barycentric.x +
                                 face.localVertices[ib] * hit.barycentric.y +
                                 face.localVertices[ic] * hit.barycentric.z;
                hit.hasUV = !face.texcoords.empty();

                if (hit.hasUV)
                    hit.uv = face.texcoords[ia] * hit.barycentric.x + face.texcoords[ib] * hit.barycentric.y +
                             face.texcoords[ic] * hit.barycentric.z;
                nearest = distance;
                result = hit;
            }
        }
    }

    return result;
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
                                      const PickViewport& viewport, CullMode inheritedCull = CullMode::None) {
    const auto ray = screenRay(camera, x, y, viewport);

    return ray ? pick(scene, *ray, inheritedCull) : std::nullopt;
}
} // namespace graphics
```

## standard/main.cpp

### before

```cpp
#include "Console.h"

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    graphics::Application app;
    Console sample(app);

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setKeyHandler([&](graphics::Application&, int key, int action) {
        sample.onKey(key, action);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) {
        sample.onUpdate(dt);
    });
    app.setRenderHandler([&](graphics::Application&) {
        return sample.onRender();
    });

    graphics::ApplicationConfig config;

    config.title = "standard - Motion, Grid, diagnostics and render passes";

    return app.run(config);
}
```

### after

```cpp
#include "Console.h"
#include <exception>
#include <iostream>

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() try {
    graphics::Application app;
    // nullopt: 숫자 키로 전환. 1~9: 기본 데모 고정. 10~22: 추가 실습·부록 고정.
    constexpr std::optional<int> initialMode = std::nullopt;
    static_assert(!initialMode || (*initialMode >= 1 && *initialMode <= lastStandardMode), "initialMode must be 1..22");
    Console sample(app, initialMode);

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
        sample.onKey(key, action, mods);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });
    app.setCursorHandler([&](graphics::Application&, double x, double y) {
        sample.onCursor(x, y);
    });
    app.setFocusHandler([&](graphics::Application&, bool focused) {
        sample.onFocus(focused);
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) {
        sample.onUpdate(dt);
    });
    app.setRenderHandler([&](graphics::Application&) {
        return sample.onRender();
    });

    graphics::ApplicationConfig config;

    config.title = "standard - Modes 1..22, input, picking and practice patterns";

    return app.run(config);
} catch (const std::exception& error) {
    std::cerr << "standard 실행 실패: " << error.what() << '\n';
    return 1;
} catch (...) {
    std::cerr << "standard 실행 실패: 앞선 프레임워크 진단을 확인하세요.\n";
    return 1;
}
```

## standard/modes/Editor.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "../Console.h"
#include <iostream>

bool Console::addControl(graphics::EntityId id, const std::string& name, ControlKind kind) {
    std::size_t index = 0;
    while (index < animation_.size() && animation_[index].joint != id)
        ++index;
    if (index == animation_.size()) {
        const auto track = graphics::AnimationSystem::createJointAnimation(app_.getScene(), id, graphics::Motion{});
        if (!track)
            return false;
        animation_.push_back(*track);
    }
    jointControls_.push_back({index, name, animation_[index].rest, kind});
    return true;
}

void Console::selectPicked(std::optional<graphics::PickResult> hit) {
    selectedJoint_ = jointControls_.size();
    if (hit) {
        std::optional<graphics::EntityId> entity = hit->entityId;
        while (entity) {
            for (std::size_t i = 0; i < jointControls_.size(); ++i) {
                const auto& control = jointControls_[i];
                if (animation_[control.track].joint == *entity && control.kind != ControlKind::Joint) {
                    selectedJoint_ = i;
                    printSelectedJoint();
                    return;
                }
            }
            const auto* component = app_.getScene().getComponent(*entity);
            entity = component ? component->parent : std::nullopt;
        }
    }
    std::cout << "선택 해제: 조작할 객체를 클릭하거나 Tab을 누르세요.\n";
}

bool Console::handleJointKey(int key) {
    if (jointControls_.empty())
        return false;
    if (key == GLFW_KEY_TAB) {
        selectedJoint_ = selectedJoint_ >= jointControls_.size() ? 0 : (selectedJoint_ + 1) % jointControls_.size();
    } else if (key == GLFW_KEY_BACKSPACE) {
        selectedJoint_ = selectedJoint_ >= jointControls_.size() ? jointControls_.size() - 1
            : (selectedJoint_ + jointControls_.size() - 1) % jointControls_.size();
    } else if (key == GLFW_KEY_X || key == GLFW_KEY_Y || key == GLFW_KEY_Z) {
        selectedAxis_ = key == GLFW_KEY_X ? 0 : key == GLFW_KEY_Y ? 1 : 2;
    } else {
        const bool edit = key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT || key == GLFW_KEY_UP ||
            key == GLFW_KEY_DOWN || key == GLFW_KEY_PAGE_UP || key == GLFW_KEY_PAGE_DOWN ||
            key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD || key == GLFW_KEY_MINUS ||
            key == GLFW_KEY_KP_SUBTRACT || key == GLFW_KEY_R;
        if (!edit)
            return false;
        if (selectedJoint_ >= jointControls_.size()) {
            std::cout << "편집 대상이 없습니다.\n";
            return true;
        }
        const auto& control = jointControls_[selectedJoint_];
        auto& rest = animation_[control.track].rest;
        const auto saved = rest;
        const auto savedBody = roomBody_;
        if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
            glm::vec3 axis(0);
            axis[selectedAxis_] = 1;
            rest.rotation *= glm::mat3(glm::rotate(glm::mat4(1),
                glm::radians(key == GLFW_KEY_LEFT ? -5.0f : 5.0f), axis));
            // Repeated edits must retain an orthonormal rotation basis.
            const auto x = glm::normalize(rest.rotation[0]);
            const auto z = glm::normalize(glm::cross(x, rest.rotation[1]));
            rest.rotation = glm::mat3(x, glm::cross(z, x), z);
        } else if (key == GLFW_KEY_UP || key == GLFW_KEY_DOWN) {
            rest.basis[selectedAxis_] += key == GLFW_KEY_UP ? .05f : -.05f;
        } else if (key == GLFW_KEY_PAGE_UP || key == GLFW_KEY_PAGE_DOWN) {
            if (control.kind == ControlKind::RoomBody) {
                std::cout << "충돌 상자는 중심 피벗을 유지합니다.\n";
                return true;
            }
            rest.localBasis[selectedAxis_] += key == GLFW_KEY_PAGE_UP ? .05f : -.05f;
        } else if (key == GLFW_KEY_R) {
            rest = control.initialRest;
        } else {
            if (control.kind == ControlKind::RoomFrame) {
                std::cout << "물리 방은 강체이므로 배율 1을 유지합니다.\n";
                return true;
            }
            const bool grow = key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD;
            rest.scale = glm::vec3(std::clamp(rest.scale.x * (grow ? 1.1f : 1 / 1.1f), .1f, 3.0f));
        }
        if (control.kind == ControlKind::RoomBody) {
            if (key == GLFW_KEY_R) {
                roomBody_ = graphics::RoomBody{};
                roomBody_.localPosition = {0, -.75f, 0};
            } else if (roomBody_.released) {
                roomBody_.worldPosition += releasedRotation_ * (rest.basis - saved.basis);
            } else {
                roomBody_.localPosition += rest.basis - saved.basis;
            }
            rest.basis = glm::vec3(0);
        }
        if (!applyControls()) {
            rest = saved;
            roomBody_ = savedBody;
            if (!applyControls())
                app_.requestClose();
            std::cerr << "편집을 거절하고 이전 상태를 복원했습니다.\n";
        }
    }
    printSelectedJoint();
    return true;
}

void Console::printSelectedJoint() const {
    if (selectedJoint_ >= jointControls_.size())
        return;
    const auto& control = jointControls_[selectedJoint_];
    const auto& rest = animation_[control.track].rest;
    std::cout << "mode=" << activeMode_ << " selected=" << selectedJoint_ + 1 << '/' << jointControls_.size()
        << ": " << control.name << " axis=" << "XYZ"[selectedAxis_]
        << " position=" << rest.basis.x << ',' << rest.basis.y << ',' << rest.basis.z
        << " pivot=" << rest.localBasis.x << ',' << rest.localBasis.y << ',' << rest.localBasis.z
        << " scale=" << rest.scale.x << '\n';
}
```

## standard/modes/FaceCommon.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "FaceCommon.h"

using namespace face_common;
using practice_common::assignVertexColors;
using practice_common::rotateAround;
using practice_common::scaleAround;


/// 장면 객체와 초기 상태를 구성한다.

bool FaceMode::initScene() {
    auto& scene = application_.getScene();

    scene.clear();

    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(HALF_SIZE * 2.0f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(HALF_SIZE * 2.0f));

    assignVertexColors(cube);
    assignVertexColors(pyramid);

    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);

    if (!cubeId || !pyramidId) {
        return false;
    }

    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;

    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();
    const auto inverseView = glm::transpose(glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0})));

    camera.position = inverseView * glm::vec3(0, 0, 4);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetAll();
    std::cout
        << "Practice 15 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
        << "Cube only 모드: T == top Z rotation, F == front open/close, S == left/right face normal rotation\n"
        << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
        << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
        << "All face motions can overlap with object spin(중복 모션 가능).\n"
        << "T/S/1/2/4/5 repeat to pause/resume(한번 더 누르면 정지). F/B repeat to reverse. 2 swaps and returns.\n";

    return true;
}

void FaceMode::resetFaces() {
    auto& scene = application_.getScene();

    for (auto id : scene.getIds()) {
        const bool visible = id == (pyramidSelected_ ? pyramidId_ : cuboidId_);

        for (std::size_t i = 0; i < scene.getComponent(id)->polyhedron.faces.size(); ++i) {
            scene.setFaceLocalMatrix(id, i, glm::mat4(1.0f));
            scene.setFaceVisible(id, i, visible);
        }
    }
}

void FaceMode::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;
    motions_ = {};
    resetFaces();
    applyObjectTransform();
}

void FaceMode::resetAll() {
    spinning_ = false;
    spinDegrees_ = 0.0f;
    application_.getRenderOptions() = graphics::RenderOptions{};
    selectShape(false);
}

void FaceMode::applyObjectTransform() {
    graphics::Transform3D transform;

    transform.rotation = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(spinDegrees_), Y_AXIS));
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void FaceMode::beginMotion(Motion kind) {
    if (pyramidSelected_) {
        return;
    }

    auto& motion = motions_[static_cast<std::size_t>(kind)];

    if (motion.initialized) {
        if (kind == Motion::Front || kind == Motion::Back) {
            motion.direction = -motion.direction;
            motion.running = true;
        } else {
            motion.running = !motion.running;
        }
    } else {
        motion.initialized = true;
        motion.running = true;

        if (kind == Motion::Hinge || kind == Motion::Swap) {
            const std::array<std::array<std::size_t, 2>, 4> pairs{
                std::array<std::size_t, 2>{0, 3}, {3, 1}, {1, 2}, {2, 0}};
            const std::array<glm::vec3, 4> corners{glm::vec3{HALF_SIZE, 0.0f, HALF_SIZE},
                                                   {HALF_SIZE, 0.0f, -HALF_SIZE},
                                                   {-HALF_SIZE, 0.0f, -HALF_SIZE},
                                                   {-HALF_SIZE, 0.0f, HALF_SIZE}};
            const int index = std::uniform_int_distribution<int>(0, 3)(random_);

            motion.pair = pairs[index];
            motion.hinge = corners[index];
            std::cout << "선택면들 -Selected cube faces (1-based 면): " << motion.pair[0] + 1 << ", "
                      << motion.pair[1] + 1 << '\n';
        }
    }
    applyFaceMotion();
}

void FaceMode::applyFaceMotion() {
    auto& scene = application_.getScene();
    std::array<glm::mat4, 6> matrices;

    matrices.fill(glm::mat4(1.0f));

    const auto compose = [&](std::size_t face, const glm::mat4& matrix) {
        matrices[face] = matrix * matrices[face];
    };

    for (std::size_t index = 1; index < motions_.size(); ++index) {
        const auto kind = static_cast<Motion>(index);
        const auto& motion = motions_[index];

        if (!motion.initialized) {
            continue;
        }

        const float progress = motion.progress;

        switch (kind) {
        case Motion::Top:
            compose(4, rotateAround({0.0f, HALF_SIZE, 0.0f}, Z_AXIS, progress));
            break;
        case Motion::Front:
            compose(0, rotateAround({0.0f, HALF_SIZE, HALF_SIZE}, X_AXIS, -90.0f * progress));
            break;
        case Motion::Side:

            compose(2, rotateAround({-HALF_SIZE, 0.0f, 0.0f}, X_AXIS, progress));
            compose(3, rotateAround({HALF_SIZE, 0.0f, 0.0f}, X_AXIS, progress));
            break;
        case Motion::Back:
            compose(1, scaleAround({0.0f, 0.0f, -HALF_SIZE}, 1.0f - progress));
            break;
        case Motion::Hinge:
            for (auto face : motion.pair) {
                compose(face, rotateAround(motion.hinge, Y_AXIS, progress));
            }
            break;
        case Motion::Swap: {

            const float angle = progress <= 90.0f ? progress : 180.0f - progress;

            compose(motion.pair[0], rotateAround(glm::vec3(0.0f), Y_AXIS, angle));
            compose(motion.pair[1], rotateAround(glm::vec3(0.0f), Y_AXIS, -angle));
            break;
        }
        case Motion::OrbitY:
            for (std::size_t face : {0u, 1u, 2u, 3u}) {
                compose(face, rotateAround(glm::vec3(0.0f), Y_AXIS, progress));
            }
            break;
        case Motion::OrbitX:
            for (std::size_t face : {4u, 5u, 0u, 1u}) {
                compose(face, rotateAround(glm::vec3(0.0f), X_AXIS, progress));
            }
            break;
        default:
            break;
        }
    }

    const auto& back = motions_[static_cast<std::size_t>(Motion::Back)];

    for (std::size_t face = 0; face < matrices.size(); ++face) {
        scene.setFaceLocalMatrix(cuboidId_, face, matrices[face]);
        scene.setFaceVisible(cuboidId_, face,
                             !pyramidSelected_ && !(face == 1 && back.initialized && back.progress == 1.0f));
    }
}

void FaceMode::onKey(int key, int action, int) {
    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        application_.requestClose();
        break;
    case GLFW_KEY_H: {
        auto& options = application_.getRenderOptions();

        options.depthTest = !options.depthTest;
        break;
    }
    case GLFW_KEY_P:
        selectShape(!pyramidSelected_);
        break;
    case GLFW_KEY_Y:
        spinning_ = !spinning_;
        break;
    case GLFW_KEY_R:
        resetAll();
        break;
    case GLFW_KEY_T:
        beginMotion(Motion::Top);
        break;
    case GLFW_KEY_F:
        beginMotion(Motion::Front);
        break;
    case GLFW_KEY_S:
        beginMotion(Motion::Side);
        break;
    case GLFW_KEY_B:
        beginMotion(Motion::Back);
        break;
    case GLFW_KEY_1:
        beginMotion(Motion::Hinge);
        break;
    case GLFW_KEY_2:
        beginMotion(Motion::Swap);
        break;
    case GLFW_KEY_4:
        beginMotion(Motion::OrbitY);
        break;
    case GLFW_KEY_5:
        beginMotion(Motion::OrbitX);
        break;
    default:
        break;
    }
}

void FaceMode::onUpdate(float deltaTime) {
    if (spinning_) {
        spinDegrees_ = std::fmod(spinDegrees_ + ROTATION_SPEED * deltaTime, 360.0f);
        applyObjectTransform();
    }

    if (pyramidSelected_) {
        return;
    }

    for (std::size_t index = 1; index < motions_.size(); ++index) {
        auto& motion = motions_[index];

        if (!motion.running) {
            continue;
        }

        const auto kind = static_cast<Motion>(index);

        if (kind == Motion::Front || kind == Motion::Back) {
            motion.progress = std::clamp(motion.progress + motion.direction * deltaTime, 0.0f, 1.0f);

            if (motion.progress == 0.0f || motion.progress == 1.0f) {
                motion.running = false;
            }
        } else {
            const float period = kind == Motion::Swap ? 180.0f : 360.0f;

            motion.progress = std::fmod(motion.progress + ROTATION_SPEED * deltaTime, period);
        }
    }
    applyFaceMotion();
}
```

## standard/modes/FaceCommon.h

### before

없음. 새 파일이다.

### after

```cpp
#pragma once
#include "PracticeMode.h"
#include "PracticeCommon.h"

namespace face_common {
inline constexpr float HALF_SIZE = 0.4f;
inline constexpr float ROTATION_SPEED = 45.0f;
inline const glm::vec3 X_AXIS{1, 0, 0}, Y_AXIS{0, 1, 0}, Z_AXIS{0, 0, 1};
}

class FaceMode : public PracticeMode {
protected:
    enum class Motion { None, Top, Front, Side, Back, Hinge, Swap, OrbitY, OrbitX, Count };

    struct FaceMotion {
        bool initialized = false;
        bool running = false;
        float progress = 0.0f;
        float direction = 1.0f;
        std::array<std::size_t, 2> pair{0, 3};
        glm::vec3 hinge{0.4f, 0.0f, 0.4f};
    };

    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    bool spinning_ = false;
    float spinDegrees_ = 0.0f;
    std::array<FaceMotion, static_cast<std::size_t>(Motion::Count)> motions_{};
    std::mt19937 random_{std::random_device{}()};

    /// 조작할 도형을 선택한다.
    virtual void selectShape(bool pyramid);
    /// 면별 애니메이션을 초기화한다.
    void resetFaces();
    /// 전체 조작 상태를 초기화한다.
    void resetAll();
    /// 선택한 면 애니메이션을 시작한다.
    void beginMotion(Motion kind);
    /// 물체 변환을 장면에 반영한다.
    void applyObjectTransform();
    /// 면별 변환을 장면에 반영한다.
    void applyFaceMotion();

public:
    /// 애플리케이션 참조를 보관한다.
    explicit FaceMode(graphics::Application& application) : PracticeMode(application), application_(application) {
    }

    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene() override;
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int) override;
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float deltaTime) override;
};
```

## standard/modes/Mode10Practice13.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <initializer_list>
#include <random>
#include <iostream>

namespace {
// 실습 13: 표시할 면과 난수 상태만 관리한다.
class Practice13 final : public PracticeMode {
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    std::mt19937 random_{std::random_device{}()};

    void selectFaces(graphics::EntityId id, std::initializer_list<std::size_t> faces);

public:
    explicit Practice13(graphics::Application& application) : PracticeMode(application), application_(application) {
    }

    bool initScene() override;

    void onKey(int key, int action, int) override;
};


bool Practice13::initScene() {
    graphics::Scene& scene = application_.getScene();

    scene.clear();

    const auto cube = scene.addCuboid(glm::vec3(0.8f), glm::vec3(0.0f));
    const auto pyramid = scene.addPyramid(glm::vec3(0.8f), glm::vec3(0.0f));

    if (!cube || !pyramid) {
        return false;
    }
    cuboidId_ = *cube;
    pyramidId_ = *pyramid;

    graphics::Transform3D initial;

    initial.rotation = glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0}));

    if (!scene.setTransform(cuboidId_, initial) || !scene.setTransform(pyramidId_, initial)) {
        return false;
    }
    // 축은 객체에 연결하지 않는다. 원점부터 양·음 방향으로 각각 1만큼 표시한다.
    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();

    camera.position = {1.8f, 1.2f, 3.0f};
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    selectFaces(cuboidId_, {0, 1, 2, 3, 4, 5});
    std::cout << "실습 13: 키 설명==면(색)\n"
              << "1==front(red), 2==back(green), 3==left(blue), 4==right(yellow)\n"
              << "5 top(cyan), 6 bottom(magenta)\n"
              << "7/8/9/0는 pyramid == front(red)/right(blue)/back(green)/left(yellow)\n"
              << "C: 랜덤한 cube 2개 faces, T: pyramid 밑변 + 랜덤한 1개 변\n"
              << "Axes: X red, Y green, Z blue. Esc: quit\n";

    return true;
}

void Practice13::selectFaces(graphics::EntityId id, std::initializer_list<std::size_t> faces) {
    auto& scene = application_.getScene();

    for (auto entity : scene.getIds()) {
        const auto* component = scene.getComponent(entity);

        for (std::size_t i = 0; i < component->polyhedron.faces.size(); ++i) {
            scene.setFaceVisible(entity, i, false);
        }
    }

    for (std::size_t face : faces) {
        scene.setFaceVisible(id, face, true);
    }
}

void Practice13::onKey(int key, int action, int) {
    if (action != GLFW_PRESS) {
        return; // 키 반복으로 무작위 선택을 여러 번 바꾸지 않는다.
    }

    if (key == GLFW_KEY_ESCAPE) {
        application_.requestClose();
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        selectFaces(cuboidId_, {static_cast<std::size_t>(key - GLFW_KEY_1)});
    } else if (key >= GLFW_KEY_7 && key <= GLFW_KEY_9) {
        selectFaces(pyramidId_, {static_cast<std::size_t>(key - GLFW_KEY_7)});
    } else if (key == GLFW_KEY_0) {
        selectFaces(pyramidId_, {3});
    } else if (key == GLFW_KEY_C) {
        const std::size_t first = std::uniform_int_distribution<int>(0, 5)(random_);
        std::size_t second = std::uniform_int_distribution<int>(0, 4)(random_);

        if (second >= first) {
            ++second; // 첫 면을 제외한 다섯 면 중 하나다.
        }
        selectFaces(cuboidId_, {first, second});
    } else if (key == GLFW_KEY_T) {
        const std::size_t side = std::uniform_int_distribution<int>(0, 3)(random_);

        selectFaces(pyramidId_, {4, side});
    }
}
}

std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app) {
    return std::make_unique<Practice13>(app);
}
```

## standard/modes/Mode11Practice14.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <iostream>

namespace {
// 실습 14: 도형 선택, 연속 입력, 위치와 로컬 회전행렬을 관리한다.
class Practice14 final : public PracticeMode {
private:
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0.0f};
    glm::mat3 rotation_{1.0f};
    glm::vec2 angularDirection_{0.0f};

    void selectShape(bool pyramid);
    void resetMotion();
    void applyTransform();

public:
    explicit Practice14(graphics::Application& application) : PracticeMode(application), application_(application) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float deltaTime) override;
    void onFocus(bool focused) override;
};

using practice_common::assignVertexColors;

namespace {

// 부호 조합은 육면체 8개 꼭짓점을 구분한다. 사각뿔은 바닥 4개와 꼭짓점 색이 다르다.


} // namespace

bool Practice14::initScene() {
    auto& scene = application_.getScene();

    scene.clear();

    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(0.8f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(0.8f));

    assignVertexColors(cube);

    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);

    if (!cubeId || !pyramidId) {
        return false;
    }
    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;

    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();
    const auto inverseView = glm::transpose(glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0})));

    camera.position = inverseView * glm::vec3(0, 0, 4);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetMotion();
    selectShape(false);
    std::cout << "실습 14 명령어: C== cube, P== pyramid, H== depth test\n"
              << "w== wireframe / W== solid (Shift or Caps Lock)\n"
              << "x/X: local X +/-, y/Y: local Y +/-\n"
              << "이동: world X/Y translation, S: reset motion, Esc: quit\n";

    return true;
}

void Practice14::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;

    auto& scene = application_.getScene();

    for (auto id : scene.getIds()) {
        const bool visible = id == (pyramidSelected_ ? pyramidId_ : cuboidId_);

        for (std::size_t i = 0; i < scene.getComponent(id)->polyhedron.faces.size(); ++i) {
            scene.setFaceVisible(id, i, visible);
        }
    }
    applyTransform();
}

void Practice14::resetMotion() {
    position_ = glm::vec3(0.0f);

    rotation_ = glm::mat3(1);
    angularDirection_ = glm::vec2(0.0f);
    pressed_.fill(false);
    applyTransform();
}

void Practice14::applyTransform() {
    graphics::Transform3D transform;

    transform.basis = position_;
    transform.rotation = rotation_;
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void Practice14::onKey(int key, int action, int mods) {
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            pressed_[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressed_[key] = false;
        }
    }

    if (action != GLFW_PRESS) {
        return;
    }

    const bool uppercase = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    auto& options = application_.getRenderOptions();

    switch (key) {
    case GLFW_KEY_ESCAPE:
        application_.requestClose();
        break;
    case GLFW_KEY_C:
        selectShape(false);
        break;
    case GLFW_KEY_P:
        selectShape(true);
        break;
    case GLFW_KEY_H:
        options.depthTest = !options.depthTest;
        break;
    case GLFW_KEY_W:
        options.wireframe = !uppercase;
        break;
    case GLFW_KEY_X:
        angularDirection_.x = uppercase ? -1.0f : 1.0f;
        break;
    case GLFW_KEY_Y:
        angularDirection_.y = uppercase ? -1.0f : 1.0f;
        break;
    case GLFW_KEY_S:
        resetMotion();
        break;
    default:
        break;
    }
}

void Practice14::onUpdate(float deltaTime) {
    const glm::vec2 movement{static_cast<float>(pressed_[GLFW_KEY_RIGHT]) - static_cast<float>(pressed_[GLFW_KEY_LEFT]),
                             static_cast<float>(pressed_[GLFW_KEY_UP]) - static_cast<float>(pressed_[GLFW_KEY_DOWN])};

    if (glm::dot(movement, movement) > 0.0f) {
        position_ += glm::vec3(glm::normalize(movement) * (0.5f * deltaTime), 0.0f);
    }
    // 오른쪽 곱은 현재 객체의 로컬 축 회전이다. 위치는 별도로 보존한다.
    const float angle = glm::radians(45.0f * deltaTime);

    rotation_ = glm::mat3(glm::rotate(glm::rotate(glm::mat4(rotation_), angle * angularDirection_.x, {1, 0, 0}),
                                      angle * angularDirection_.y, {0, 1, 0}));

    const auto x = glm::normalize(rotation_[0]);
    const auto z = glm::normalize(glm::cross(x, rotation_[1]));

    rotation_ = glm::mat3(x, glm::cross(z, x), z);
    applyTransform();
}

void Practice14::onFocus(bool focused) {
    if (!focused) {
        pressed_.fill(false); // 포커스 밖에서 놓친 release가 이동을 계속시키지 않는다.
    }
}
}

std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app) {
    return std::make_unique<Practice14>(app);
}
```

## standard/modes/Mode12Practice15.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "FaceCommon.h"

std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app) {
    return std::make_unique<FaceMode>(app);
}
```

## standard/modes/Mode13Practice16.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "FaceCommon.h"
#include <iostream>

using namespace face_common;
using practice_common::rotateAround;

namespace {
class Practice16 final : public FaceMode {
    enum class PyramidMotion { None, All, Sequence, Swap, Apex };
    PyramidMotion pyramidMotion_ = PyramidMotion::None;
    bool pyramidRunning_ = false;
    float pyramidProgress_ = 0, pyramidDirection_ = 1;
    void beginPyramid(PyramidMotion motion);
    void updatePyramid(float dt);
    void applyPyramid();
    void selectShape(bool pyramid) override {
        FaceMode::selectShape(pyramid);
        pyramidMotion_ = PyramidMotion::None;
        pyramidRunning_ = false;
        pyramidProgress_ = 0;
        pyramidDirection_ = 1;
    }
public:
    explicit Practice16(graphics::Application& app) : FaceMode(app) {}
    bool initScene() override;
    void onKey(int key, int action, int mods) override {
        if (action == GLFW_PRESS && key >= GLFW_KEY_6 && key <= GLFW_KEY_9) {
            const PyramidMotion modes[] = {PyramidMotion::All, PyramidMotion::Sequence,
                                           PyramidMotion::Swap, PyramidMotion::Apex};
            beginPyramid(modes[key - GLFW_KEY_6]);
            return;
        }
        FaceMode::onKey(key, action, mods);
    }
    void onUpdate(float dt) override {
        if (!pyramidSelected_) {
            FaceMode::onUpdate(dt);
            return;
        }
        if (spinning_) {
            spinDegrees_ = std::fmod(spinDegrees_ + ROTATION_SPEED * dt, 360.0f);
            applyObjectTransform();
        }
        updatePyramid(dt);
    }
};

bool Practice16::initScene() {
    auto& scene = application_.getScene();

    scene.clear();

    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(HALF_SIZE * 2.0f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(HALF_SIZE * 2.0f));
    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);

    if (!cubeId || !pyramidId) {
        return false;
    }

    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;

    if (!scene.addLine({-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.2f, 0.2f}) ||
        !scene.addLine({0.0f, -1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.2f, 1.0f, 0.2f}) ||
        !scene.addLine({0.0f, 0.0f, -1.0f}, {0.0f, 0.0f, 1.0f}, {0.2f, 0.4f, 1.0f})) {
        return false;
    }

    auto& camera = application_.getCamera();
    const auto inverseView = glm::transpose(glm::mat3(
        glm::rotate(glm::rotate(glm::mat4(1), glm::radians(30.0f), {1, 0, 0}), glm::radians(30.0f), {0, 1, 0})));

    camera.position = inverseView * glm::vec3(0, 0, 4);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.target = glm::vec3(0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.5f;
    resetAll();
    std::cout
        << "Practice 16 키 조작: H==depth, P==cube/pyramid, Y==자전/pause, R reset, Q/Esc quit\n"
        << "Cube only 모드: T == top Z rotation, F == front open/close, S == left/right face normal rotation\n"
        << "B == back face shrink/restore, 1- 같은 모서리 면 2개 y축회전 360, 2- 같은 모서리 면 2개 y축반대공전 90\n"
        << "4 four side faces orbit공전 - Y축 기준, 5 top/bottom/front/back orbit공전 - X축 기준\n"
        << "All face motions can overlap with object spin(중복 모션 가능).\n"
        << "T/S/1/2/4/5 repeat to pause/resume(한번 더 누르면 정지). F/B repeat to reverse. 2 swaps and returns.\n";
    std::cout << "Pyramid 6: meet below, 7: sequential unfolding, 8: opposite swap, 9: apex opening\n";

    return true;
}


void Practice16::beginPyramid(PyramidMotion motion) {
    if (!pyramidSelected_)
        selectShape(true);

    if (pyramidMotion_ != motion) {
        pyramidMotion_ = motion;
        pyramidProgress_ = 0;
        pyramidDirection_ = 1;
        pyramidRunning_ = true;

    } else if (motion == PyramidMotion::All || motion == PyramidMotion::Apex) {
        pyramidDirection_ = -pyramidDirection_;
        pyramidRunning_ = true;
    } else if (motion == PyramidMotion::Swap && !pyramidRunning_ && (pyramidProgress_ == 0 || pyramidProgress_ == 1)) {
        pyramidDirection_ = -pyramidDirection_;
        pyramidRunning_ = true;
    } else
        pyramidRunning_ = !pyramidRunning_;

    applyPyramid();
}


void Practice16::updatePyramid(float dt) {
    if (pyramidRunning_) {
        if (pyramidMotion_ == PyramidMotion::Sequence)
            pyramidProgress_ = std::fmod(pyramidProgress_ + dt, 8.0f);
        else {
            pyramidProgress_ = std::clamp(pyramidProgress_ + pyramidDirection_ * dt * .5f, 0.0f, 1.0f);

            if (pyramidProgress_ == 0 || pyramidProgress_ == 1)
                pyramidRunning_ = false;
        }
    }
    applyPyramid();
}


void Practice16::applyPyramid() {
    auto& scene = application_.getScene();
    const float b = HALF_SIZE, h = 2 * HALF_SIZE;

    const float inward = glm::degrees(std::atan2(h, b));

    const std::array<glm::vec3, 4> hinges{glm::vec3{0, -b, b}, {b, -b, 0}, {0, -b, -b}, {-b, -b, 0}};
    const std::array<glm::vec3, 4> axes{X_AXIS, -Z_AXIS, -X_AXIS, Z_AXIS};

    const auto active = std::size_t(pyramidProgress_ / 2.0f) % 4;
    const float phase = std::fmod(pyramidProgress_, 2.0f);

    for (std::size_t i = 0; i < 4; ++i) {
        glm::mat4 matrix(1);

        if (pyramidMotion_ == PyramidMotion::All)
            matrix = rotateAround(hinges[i], axes[i], (360.0f - 2 * inward) * pyramidProgress_);

        else if (pyramidMotion_ == PyramidMotion::Sequence && i == active)
            matrix = rotateAround(hinges[i], axes[i], (180 - inward) * (phase <= 1 ? phase : 2 - phase));

        else if (pyramidMotion_ == PyramidMotion::Swap && (i == 0 || i == 2))
            matrix = rotateAround({0, b, 0}, Y_AXIS, (i == 0 ? 180.0f : -180.0f) * pyramidProgress_);

        else if (pyramidMotion_ == PyramidMotion::Apex)
            matrix = rotateAround({0, b, 0}, axes[i], -45.0f * pyramidProgress_);

        scene.setFaceLocalMatrix(pyramidId_, i, matrix);
    }
    scene.setFaceLocalMatrix(pyramidId_, 4, glm::mat4(1));
}

}

std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app) {
    return std::make_unique<Practice16>(app);
}
```

## standard/modes/Mode14Practice17.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
class Practice17 final : public PracticeMode {
    struct Object {
        graphics::EntityId orbit = 0, body = 0;
        glm::vec3 position{0};
        glm::mat3 rotation{1};
        glm::vec2 spin{0};
        float orbitDegrees = 0, orbitDirection = 0, scale = .65f;
        int shape = 0;
    };
    enum class Motion { None, Straight, Bypass, Combined };

    graphics::Application& app_;
    std::array<Object, 2> objects_{};
    int selected_ = 3;
    glm::vec2 movement_{0};
    Motion motion_ = Motion::None;
    bool running_ = false;
    float elapsed_ = 0;
    std::array<glm::vec3, 2> starts_{};
    std::array<float, 2> scales_{};
    std::array<glm::vec3, 2> motionOffsets_{glm::vec3(0), glm::vec3(0)};
    /// 배치와 조작 상태를 초기화한다.
    void reset();
    /// 계산한 변환을 장면에 반영한다.
    void apply();
    /// 선택한 이동을 시작한다.
    void begin(Motion motion);
    /// 선택한 도형의 모델을 만든다.
    graphics::Model model(int shape) const;
    /// 공전 회전 행렬을 계산한다.
    glm::mat3 orbitRotation(std::size_t i) const;
    /// 물체의 월드 위치를 계산한다.
    glm::vec3 worldPosition(std::size_t i) const;

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Practice17(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene() override;
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods) override;
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt) override;
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused) override;
};

using practice_common::turn;

namespace {
/// 축과 각도로 회전 행렬을 만든다.

}

/// 선택한 도형의 모델을 만든다.

graphics::Model Practice17::model(int shape) const {
    if (shape == 0) {
        auto cone = graphics::DiagramSystem::makeCone(.5f, 1.0f);

        for (auto& face : cone.faces)
            face.outline = graphics::OutlineMode::AllTriangleEdges;

        return cone;
    }

    if (shape == 1)
        return graphics::DiagramSystem::makeSphere(.5f);

    return graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube);
}

glm::mat3 Practice17::orbitRotation(std::size_t i) const {
    return turn(objects_[i].orbitDegrees, {0, 1, 0});
}

glm::vec3 Practice17::worldPosition(std::size_t i) const {
    return orbitRotation(i) * objects_[i].position;
}

bool Practice17::initScene() {
    reset();
    std::cout << "Practice 17: 1 left, 2 right, 3 both; x/X,y/Y spin; r/R orbit\n"
              << "a/A local scale, b/B origin scale, d/D left/right, e/E up/down\n"
              << "t straight swap, u bypass swap, v scale/spin/orbit; c shape, s reset, q quit\n";

    return true;
}

void Practice17::reset() {
    auto& scene = app_.getScene();

    scene.clear();
    objects_ = {};
    movement_ = glm::vec2(0);
    motionOffsets_.fill(glm::vec3(0));
    selected_ = 3;
    motion_ = Motion::None;
    running_ = false;
    elapsed_ = 0;

    for (std::size_t i = 0; i < 2; ++i) {
        auto& o = objects_[i];

        o.position = {i == 0 ? -1.0f : 1.0f, 0, 0};
        o.shape = i == 0 ? 0 : 2;
        o.orbit = scene.addGroup().value();
        o.body = scene.addModel(model(o.shape), graphics::Transform3D{}, graphics::PivotMode::Preserve).value();

        if (!scene.setParent(o.body, o.orbit) ||
            !scene.setTint(o.body, i == 0 ? graphics::GlColor{1, .65f, .3f} : graphics::GlColor{.4f, .8f, 1}))
            throw std::runtime_error("Cannot initialize object hierarchy.");
    }
    scene.addLine({-2, 0, 0}, {2, 0, 0}, {1, .2f, .2f});
    scene.addLine({0, -2, 0}, {0, 2, 0}, {.2f, 1, .2f});
    scene.addLine({0, 0, -2}, {0, 0, 2}, {.2f, .4f, 1});

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};

    const auto inverseView = glm::transpose(turn(30, {1, 0, 0}) * turn(30, {0, 1, 0}));

    camera.position = inverseView * glm::vec3(0, 0, 6);
    camera.up = inverseView * glm::vec3(0, 1, 0);
    camera.orthographic = true;
    camera.orthographicHalfExtent = 2.4f;
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().wireframe = true;
    apply();
}

void Practice17::apply() {
    auto& scene = app_.getScene();

    for (std::size_t i = 0; i < 2; ++i) {
        const auto& o = objects_[i];
        graphics::Transform3D orbit, body;

        orbit.rotation = orbitRotation(i);
        body.basis = o.position;
        body.rotation = o.rotation;

        if (o.shape == 0)
            body.rotation *= turn(-45.0f, {0, 0, 1});
        body.scale = glm::vec3(o.scale);

        if (!scene.setTransform(o.orbit, orbit) || !scene.setTransform(o.body, body))
            throw std::runtime_error("Invalid model transform.");
    }
}

void Practice17::begin(Motion motion) {
    if (motion_ == motion) {
        running_ = !running_;

        return;
    }
    motion_ = motion;
    motionOffsets_.fill(glm::vec3(0));
    running_ = true;
    elapsed_ = 0;

    for (std::size_t i = 0; i < 2; ++i) {
        starts_[i] = worldPosition(i);
        scales_[i] = objects_[i].scale;
    }
}

void Practice17::onKey(int key, int action, int mods) {
    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    const float sign = upper ? -1.0f : 1.0f;

    if (key == GLFW_KEY_D || key == GLFW_KEY_E) {
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            float& direction = key == GLFW_KEY_D ? movement_.x : movement_.y;

            direction = action == GLFW_RELEASE ? 0 : (key == GLFW_KEY_D ? -sign : sign);
        }

        return;
    }

    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
        app_.requestClose();

        return;
    }

    if (key >= GLFW_KEY_1 && key <= GLFW_KEY_3) {
        selected_ = key - GLFW_KEY_0;

        return;
    }

    if (key == GLFW_KEY_S) {
        reset();

        return;
    }

    if (key == GLFW_KEY_T) {
        begin(Motion::Straight);

        return;
    }

    if (key == GLFW_KEY_U) {
        begin(Motion::Bypass);

        return;
    }

    if (key == GLFW_KEY_V) {
        begin(Motion::Combined);

        return;
    }

    if (key == GLFW_KEY_C) {
        for (auto& o : objects_) {
            o.shape = (o.shape + 1) % 3;

            if (!app_.getScene().setModel(o.body, model(o.shape)))
                throw std::runtime_error("Cannot replace model.");
        }
        apply();

        return;
    }

    for (std::size_t i = 0; i < 2; ++i) {
        if (selected_ != 3 && selected_ != int(i + 1))
            continue;

        auto& o = objects_[i];
        const auto previousPosition = worldPosition(i);
        const auto previousScale = o.scale;

        switch (key) {
        case GLFW_KEY_X:
            o.spin.x = o.spin.x == sign ? 0 : sign;
            break;
        case GLFW_KEY_Y:
            o.spin.y = o.spin.y == sign ? 0 : sign;
            break;
        case GLFW_KEY_R:
            o.orbitDirection = o.orbitDirection == sign ? 0 : sign;
            break;
        case GLFW_KEY_A:
            o.scale = std::clamp(o.scale * (upper ? 1 / 1.1f : 1.1f), .15f, 3.0f);
            break;
        case GLFW_KEY_B: {
            const float old = o.scale;

            o.scale = std::clamp(old * (upper ? 1 / 1.1f : 1.1f), .15f, 3.0f);
            o.position *= o.scale / old;
            break;
        }
        default:
            break;
        }
        if (motion_ == Motion::Straight || motion_ == Motion::Bypass)
            motionOffsets_[i] += worldPosition(i) - previousPosition;
        if (motion_ == Motion::Combined)
            scales_[i] *= o.scale / previousScale;
    }

    apply();
}

void Practice17::onUpdate(float dt) {
    if (running_)
        elapsed_ += dt;

    for (std::size_t i = 0; i < 2; ++i) {
        auto& o = objects_[i];

        if (selected_ == 3 || selected_ == int(i + 1)) {
            const auto delta = glm::vec3(movement_ * .7f * dt, 0);
            o.position += glm::transpose(orbitRotation(i)) * delta;
            if (motion_ == Motion::Straight || motion_ == Motion::Bypass)
                motionOffsets_[i] += delta;
        }

        const bool combined = motion_ == Motion::Combined;
        const auto spin = o.spin + (combined && running_ ? glm::vec2(0, 1) : glm::vec2(0));

        const float orbit = o.orbitDirection + (combined && running_ ? 1.0f : 0.0f);

        o.rotation = o.rotation * turn(45 * dt * spin.x, {1, 0, 0}) * turn(45 * dt * spin.y, {0, 1, 0});

        const auto x = glm::normalize(o.rotation[0]), z = glm::normalize(glm::cross(x, o.rotation[1]));

        o.rotation = glm::mat3(x, glm::cross(z, x), z);
        o.orbitDegrees = std::fmod(o.orbitDegrees + 30 * dt * orbit, 360.0f);

        if (running_ && combined)
            o.scale = scales_[i] * (1 + (i == 0 ? .35f : -.35f) * std::sin(elapsed_));
        else if (running_ && (motion_ == Motion::Straight || motion_ == Motion::Bypass)) {
            const float t = std::min(elapsed_ / 2.0f, 1.0f);
            auto point = glm::mix(starts_[i], starts_[1 - i], t) + motionOffsets_[i];

            if (motion_ == Motion::Bypass)
                point.y += (i == 0 ? 1.0f : -1.0f) * .9f * std::sin(3.14159265f * t);
            o.position = glm::transpose(orbitRotation(i)) * point;
        }
    }

    if (running_ && (motion_ == Motion::Straight || motion_ == Motion::Bypass) && elapsed_ >= 2) {
        motion_ = Motion::None;
        running_ = false;
    }
    apply();
}

void Practice17::onFocus(bool focused) {
    if (!focused)
        movement_ = glm::vec2(0);
}
}

std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app) {
    return std::make_unique<Practice17>(app);
}
```

## standard/modes/Mode15Practice18.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {
class Practice18 final : public PracticeMode {
    graphics::Application& app_;
    graphics::EntityId root_ = 0, sun_ = 0;
    std::array<graphics::EntityId, 3> tracks_{}, planets_{}, moonFrames_{}, moonTracks_{}, moons_{};
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0};
    std::array<float, 3> planetAngle_{0, 120, 240}, moonAngle_{0, 120, 240};
    float radius_ = 1, targetRadius_ = 1, sunTime_ = 0, moonTilt_ = 0, moonSpin_ = 0;
    bool pulse_ = false, rotateMoons_ = false;
    /// 계산한 변환을 장면에 반영한다.
    void apply();

public:
    /// 애플리케이션 참조를 보관한다.
    explicit Practice18(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene() override;
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action, int mods) override;
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt) override;
    /// 포커스가 없으면 지속 입력을 해제한다.
    void onFocus(bool focused) override;
};

using practice_common::turn;

namespace {
/// 축과 각도로 회전 행렬을 만든다.


constexpr std::array<float, 3> radii{2.0f, 2.8f, 3.6f};
constexpr std::array<float, 3> inclinations{0, 45, -45};

/// 부모에 연결한 원 궤도를 그린다.
void circle(graphics::Scene& scene, graphics::EntityId owner, graphics::GlColor color, bool xz) {
    constexpr int segments = 96;

    for (int i = 0; i < segments; ++i) {
        const float a = 6.283185307f * i / segments, b = 6.283185307f * (i + 1) / segments;
        const auto point = [&](float angle) {
            return xz ? glm::vec3(std::cos(angle), 0, std::sin(angle)) : glm::vec3(std::cos(angle), std::sin(angle), 0);
        };

        if (!scene.addLine(point(a), point(b), color, owner))
            throw std::runtime_error("Cannot add orbit line.");
    }
}
}

/// 장면 객체와 초기 상태를 구성한다.

bool Practice18::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    root_ = scene.addGroup().value();
    sun_ = scene.addSphere(.55f, {0, 0, 0}).value();

    if (!scene.setParent(sun_, root_))
        return false;

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {0, 5, 8};
    camera.target = {0, 0, 0};
    camera.orthographic = true;
    camera.orthographicHalfExtent = 5.2f;

    const std::array<graphics::GlColor, 3> colors{graphics::GlColor{1, .25f, .2f}, {.2f, .65f, 1}, {.3f, 1, .4f}};
    const std::array<graphics::GlColor, 3> moonColors{graphics::GlColor{.8f, .3f, 1}, {1, .5f, .15f}, {.4f, 1, .85f}};

    for (std::size_t i = 0; i < 3; ++i) {
        tracks_[i] = scene.addGroup().value();
        planets_[i] = scene.addGroup().value();
        moonFrames_[i] = scene.addGroup().value();
        moonTracks_[i] = scene.addGroup().value();

        const auto planet = scene.addSphere(.24f + .06f * float(i), {0, 0, 0}).value();
        const float moonRadius = .09f + .015f * float(i);

        moons_[i] = scene.addSphere(moonRadius, {0, 0, 0}).value();

        if (!scene.setParent(tracks_[i], root_) || !scene.setParent(planets_[i], root_) ||
            !scene.setParent(planet, planets_[i]) || !scene.setParent(moonFrames_[i], planets_[i]) ||
            !scene.setParent(moonTracks_[i], moonFrames_[i]) || !scene.setParent(moons_[i], moonFrames_[i]) ||
            !scene.setTint(planet, colors[i]) || !scene.setTint(moons_[i], moonColors[i]))
            return false;
        circle(scene, tracks_[i], colors[i], true);
        circle(scene, moonTracks_[i], moonColors[i], true);

        if (!scene.addLine({moonRadius, 0, 0}, {2 * moonRadius, 0, 0}, {1, 1, .2f}, moons_[i]))
            return false;
    }
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().lighting = true;
    apply();
    std::cout << "Practice 18: p orthographic / P perspective; m solid / M wire\n"
              << "WASD move XY, +/- move Z; 1 orbit size; 2 sun pulse/color; 3 moon orbit rotation/spin; q quit\n";

    return true;
}

void Practice18::apply() {
    auto& scene = app_.getScene();

    if (!scene.setPosition(root_, position_))
        throw std::runtime_error("Invalid system position.");

    graphics::Transform3D sun;

    sun.scale = glm::vec3(1 + .18f * std::sin(sunTime_));

    if (!scene.setTransform(sun_, sun) ||
        !scene.setTint(sun_, {1, .65f + .2f * std::sin(sunTime_), .15f + .1f * std::cos(sunTime_)}))
        throw std::runtime_error("Invalid sun transform.");

    for (std::size_t i = 0; i < 3; ++i) {
        const auto tilt = turn(inclinations[i], {0, 0, 1});
        graphics::Transform3D track;

        track.rotation = tilt;
        track.scale = glm::vec3(radii[i] * radius_);

        const auto planet = tilt * turn(planetAngle_[i], {0, 1, 0}) * glm::vec3(radii[i] * radius_, 0, 0);
        graphics::Transform3D frame;

        const auto& camera = app_.getCamera();
        const auto forward = glm::normalize(camera.target - camera.position);
        const auto right = glm::normalize(glm::cross(forward, camera.up));
        const auto screenUp = glm::cross(right, forward);
        frame.rotation = turn(moonTilt_, {0, 0, 1}) * glm::mat3(right, forward, screenUp);

        const float r = (.55f + .15f * float(i)) * radius_, a = glm::radians(moonAngle_[i]);
        graphics::Transform3D moonTrack;

        moonTrack.scale = glm::vec3(r);

        graphics::Transform3D moon;

        moon.basis = {r * std::cos(a), 0, r * std::sin(a)};
        moon.rotation = turn(moonSpin_, {0, 1, 0});

        if (!scene.setTransform(tracks_[i], track) || !scene.setPosition(planets_[i], planet) ||
            !scene.setTransform(moonFrames_[i], frame) || !scene.setTransform(moonTracks_[i], moonTrack) ||
            !scene.setTransform(moons_[i], moon))
            throw std::runtime_error("Invalid orbit transform.");
    }
}

void Practice18::onKey(int key, int action, int mods) {
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS)
            pressed_[key] = true;
        else if (action == GLFW_RELEASE)
            pressed_[key] = false;
    }

    if (action != GLFW_PRESS)
        return;

    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    switch (key) {
    case GLFW_KEY_P:
        app_.getCamera().orthographic = !upper;
        break;
    case GLFW_KEY_M:
        app_.getRenderOptions().wireframe = upper;
        break;
    case GLFW_KEY_1:
        targetRadius_ = targetRadius_ == 1 ? 1.3f : 1.0f;
        break;
    case GLFW_KEY_2:
        pulse_ = !pulse_;
        break;
    case GLFW_KEY_3:
        rotateMoons_ = !rotateMoons_;
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void Practice18::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_D]) - float(pressed_[GLFW_KEY_A]),
                       float(pressed_[GLFW_KEY_W]) - float(pressed_[GLFW_KEY_S]),
                       float(pressed_[GLFW_KEY_EQUAL] || pressed_[GLFW_KEY_KP_ADD]) -
                           float(pressed_[GLFW_KEY_MINUS] || pressed_[GLFW_KEY_KP_SUBTRACT])};

    if (glm::length(movement) > 0)
        position_ += glm::normalize(movement) * dt * 1.5f;
    radius_ += std::clamp(targetRadius_ - radius_, -.3f * dt, .3f * dt);

    if (pulse_)
        sunTime_ = std::fmod(sunTime_ + dt, 6.283185307f);

    if (rotateMoons_) {
        moonTilt_ = std::fmod(moonTilt_ + 25 * dt, 360.0f);
        moonSpin_ = std::fmod(moonSpin_ + 90 * dt, 360.0f);
    }

    for (std::size_t i = 0; i < 3; ++i) {
        planetAngle_[i] = std::fmod(planetAngle_[i] + (20 + 10 * float(i)) * dt, 360.0f);
        moonAngle_[i] = std::fmod(moonAngle_[i] + (70 + 30 * float(i)) * dt, 360.0f);
    }
    apply();
}

void Practice18::onFocus(bool focused) {
    if (!focused)
        pressed_.fill(false);
}
}

std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app) {
    return std::make_unique<Practice18>(app);
}
```

## standard/modes/Mode16Practice19.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "TankCommon.h"

std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app) {
    return makeTankMode(app, false);
}
```

## standard/modes/Mode17Practice20.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <array>
#include <vector>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace {
class Practice20 final : public PracticeMode {
    graphics::Application& app_;
    graphics::AnimationClock clock_;
    std::array<graphics::EntityId, 5> boxes_{};
    std::vector<graphics::EntityId> stripes_;
    bool leftHeld_ = false;
    bool rightHeld_ = false;

    void placeMovingObjects();
    void resetState();
    void zoom(float factor);

public:
    explicit Practice20(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onFocus(bool focused) override;
    void onUpdate(float dt) override;
};

using practice_common::require;

namespace {
constexpr float beltHeight = 1.0f;
constexpr float boxSize = .85f;
constexpr double beltSpeed = 1.25;
constexpr std::size_t stripeCount = 30;
constexpr float cornerRadius = .8f;
constexpr double halfPi = 1.57079632679489661923;
const glm::vec3 conveyorStart{-4, beltHeight, -2};
const glm::vec3 cornerPivot{2.2f, beltHeight, -1.2f};
constexpr float conveyorEndZ = 3.0f;
const double conveyorLength =
    (double(cornerPivot.x) - conveyorStart.x) + halfPi * double(cornerRadius) + (double(conveyorEndZ) - cornerPivot.z);


}

bool Practice20::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    stripes_.clear();

    require(
        graphics::PatternSystem::addPolygon(scene, {{-6, 0, -4}, {-6, 0, 5}, {6, 0, 5}, {6, 0, -4}}, {.25f, .30f, .34f})
            .has_value(),
        "Floor creation failed.");

    require(graphics::PatternSystem::addPolygon(scene,
                                                {{-4.8f, beltHeight, -2.8f},
                                                 {-4.8f, beltHeight, -1.2f},
                                                 {3.8f, beltHeight, -1.2f},
                                                 {3.8f, beltHeight, -2.8f}},
                                                {.09f, .12f, .15f})
                .has_value(),
            "Horizontal conveyor plane creation failed.");
    require(
        graphics::PatternSystem::addPolygon(
            scene,
            {{2.2f, beltHeight, -1.2f}, {2.2f, beltHeight, 3.8f}, {3.8f, beltHeight, 3.8f}, {3.8f, beltHeight, -1.2f}},
            {.09f, .12f, .15f})
            .has_value(),
        "Vertical conveyor plane creation failed.");

    for (std::size_t i = 0; i < stripeCount; ++i) {
        const auto stripe = graphics::PatternSystem::addBox(scene, {1.4f, .03f, .10f}, {0, 0, 0}, {.72f, .77f, .80f});

        require(stripe.has_value(), "Conveyor stripe creation failed.");
        stripes_.push_back(*stripe);
    }

    const std::array<graphics::GlColor, 5> colors{
        {{.95f, .30f, .23f}, {.23f, .72f, .96f}, {.98f, .75f, .20f}, {.38f, .86f, .43f}, {.78f, .44f, .96f}}};

    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const auto box = graphics::PatternSystem::addBox(scene, {boxSize, boxSize, boxSize}, {0, 0, 0}, colors[i]);

        require(box.has_value(), "Parcel creation failed.");
        boxes_[i] = *box;

        for (int face = 0; face < 4; ++face) {
            const auto label = graphics::PatternSystem::addJoint(scene, *box, {0, 0, 0});

            require(label.has_value(), "Parcel label frame creation failed.");
            require(graphics::PatternSystem::rotateJoint(scene, *label, {0, 1, 0}, float(face) * 90.0f),
                    "Parcel label frame rotation failed.");

            const int count = int(i + 1);
            const float z = boxSize * .5f + .008f;

            for (int mark = 0; mark < count; ++mark) {
                const float x = (float(mark) - float(count - 1) * .5f) * .14f;

                require(graphics::PatternSystem::addPolygon(
                            scene,
                            {{x - .05f, -.05f, z}, {x + .05f, -.05f, z}, {x + .05f, .05f, z}, {x - .05f, .05f, z}},
                            {0, 0, 0}, *label)
                            .has_value(),
                        "Parcel square mark creation failed.");
            }
        }
    }

    auto& options = app_.getRenderOptions();

    options = graphics::RenderOptions{};
    options.depthTest = true;
    options.lighting = true;
    options.background = {.08f, .10f, .14f, 1};
    resetState();
    std::cout << "실습 20: 택배 물류 센터\n"
              << "b 이동 시작/정지 | +/- 확대/축소 | p 원근 / P 직각\n"
              << "좌/우 중심 Y축 공전 | r 전체 리셋 | q 종료\n";

    return true;
}

void Practice20::placeMovingObjects() {
    auto& scene = app_.getScene();
    const double travelled = clock_.seconds * beltSpeed;
    const auto place = [&](graphics::EntityId entity, double distance, float height) {
        const auto pose = graphics::PatternSystem::conveyorCornerPose(distance, conveyorStart, cornerPivot,
                                                                      cornerRadius, conveyorEndZ);

        require(pose.has_value(), "Conveyor corner pose sampling failed.");

        const auto* component = scene.getComponent(entity);

        require(component != nullptr, "Conveyor moving entity is missing.");

        auto transform = component->transform;

        transform.basis = pose->position + glm::vec3(0, height, 0);
        transform.rotation = glm::mat3(glm::rotate(glm::mat4(1), glm::radians(pose->yawDegrees), glm::vec3(0, 1, 0)));
        require(scene.setTransform(entity, transform), "Conveyor moving entity transform failed.");
    };

    for (std::size_t i = 0; i < stripes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(stripes_.size());

        place(stripes_[i], travelled + offset, .015f);
    }

    for (std::size_t i = 0; i < boxes_.size(); ++i) {
        const double offset = conveyorLength * double(i) / double(boxes_.size());

        place(boxes_[i], travelled + offset, .03f + boxSize * .5f);
    }
}

void Practice20::resetState() {
    clock_.reset();
    clock_.playing = false;
    leftHeld_ = false;
    rightHeld_ = false;

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {11, 10, 13};
    camera.target = {0, .7f, .3f};
    camera.up = {0, 1, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 7.5f;
    camera.nearPlane = .1f;
    camera.farPlane = 120;
    require(graphics::CameraSystem::validCamera(camera), "Reset camera is invalid.");
    placeMovingObjects();
}

void Practice20::zoom(float factor) {
    auto& camera = app_.getCamera();
    const float current =
        camera.orthographic ? camera.orthographicHalfExtent : glm::length(camera.position - camera.target);

    const float next = std::clamp(current * factor, 2.0f, camera.orthographic ? 35.0f : 60.0f);

    require(graphics::PatternSystem::zoomCamera(camera, next / current), "Camera zoom failed.");
}

void Practice20::onKey(int key, int action, int mods) {
    if (key == GLFW_KEY_LEFT)
        leftHeld_ = action != GLFW_RELEASE;

    if (key == GLFW_KEY_RIGHT)
        rightHeld_ = action != GLFW_RELEASE;

    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
        zoom(.9f);

    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
        zoom(1.0f / .9f);

    if (action != GLFW_PRESS)
        return;

    switch (key) {
    case GLFW_KEY_B:
        clock_.playing = !clock_.playing;
        break;
    case GLFW_KEY_P:

        app_.getCamera().orthographic = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
        break;
    case GLFW_KEY_R:
        resetState();
        break;
    case GLFW_KEY_Q:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void Practice20::onFocus(bool focused) {
    if (!focused) {
        leftHeld_ = false;
        rightHeld_ = false;
    }
}

void Practice20::onUpdate(float dt) {
    require(clock_.advance(dt), "Conveyor clock update failed.");
    placeMovingObjects();

    const int direction = int(rightHeld_) - int(leftHeld_);

    if (direction != 0) {
        const graphics::AxisLine axis{app_.getCamera().target, {0, 1, 0}};

        require(graphics::PatternSystem::orbitCamera(app_.getCamera(), axis, float(direction) * 65.0f * dt),
                "Camera orbit failed.");
    }
}
}

std::unique_ptr<PracticeMode> makePractice20(graphics::Application& app) {
    return std::make_unique<Practice20>(app);
}
```

## standard/modes/Mode18Practice21.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
class Practice21 final : public PracticeMode {
    struct BodyObject {
        graphics::EntityId entity = 0;
        graphics::RoomBody body;
        bool elastic = false;
        glm::mat3 releasedRotation{1};
    };

    graphics::Application& app_;
    const graphics::Bounds3 roomBounds_{{-3, -2, -3.5f}, {3, 2.5f, 3.5f}};
    graphics::EntityId room_ = 0;
    std::vector<BodyObject> objects_;

    unsigned ballCount_ = 0;

    float roomAngle_ = 0;
    float roomSpinAngle_ = 0;
    float pendingRotation_ = 0;
    bool roomSpinning_ = false;
    bool floorOpen_ = false;
    bool dragging_ = false;
    bool focused_ = true;
    float lastDt_ = 1.0f / 60.0f;
    double previousMouseX_ = 0;

    glm::mat4 roomWorld() const;
    bool addBall();

    void releaseBody(BodyObject& object) const;

    bool syncScene();

    void require(bool succeeded, const char* operation) const;
    void printHelp() const;

public:
    explicit Practice21(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onMouse(const graphics::MouseEvent& event) override;
    void onCursor(double x, double y) override;
    void onFocus(bool focused) override;
    void onUpdate(float dt) override;
};


void Practice21::require(bool succeeded, const char* operation) const {
    if (!succeeded)
        throw std::runtime_error(operation);
}

glm::mat4 Practice21::roomWorld() const {
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));

    return glm::rotate(spin, roomAngle_, glm::vec3(0, 0, 1));
}

void Practice21::releaseBody(BodyObject& object) const {
    const auto matrix = roomWorld();

    object.body.worldPosition = object.body.position(matrix);
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));
    const auto angularVelocity = glm::vec3(0, roomSpinning_ ? glm::radians(30.0f) : 0, 0) +
        glm::mat3(spin) * glm::vec3(0, 0, pendingRotation_ / lastDt_);
    object.body.worldVelocity = glm::mat3(matrix) * object.body.localVelocity +
        glm::cross(angularVelocity, object.body.worldPosition);
    object.releasedRotation = glm::mat3(matrix);
    object.body.released = true;
}

bool Practice21::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    objects_.clear();
    ballCount_ = 0;
    roomAngle_ = roomSpinAngle_ = pendingRotation_ = 0;
    floorOpen_ = dragging_ = roomSpinning_ = false;

    const auto room = graphics::PatternSystem::addRoom(scene, roomBounds_, true);

    if (!room)
        return false;
    room_ = *room;

    const graphics::GlColor colors[] = {{.6f, .6f, .6f},   {.35f, .5f, .8f}, {.85f, .4f, .4f},
                                        {.35f, .75f, .5f}, {.75f, .5f, .8f}, {.8f, .7f, .4f}};

    for (std::size_t face = 0; face < 6; ++face) {
        if (!scene.setFaceVertexColors(room_, face, std::vector<graphics::GlColor>(4, colors[face])))
            return false;
    }

    const glm::vec3 sizes[] = {{.65f, .65f, .65f}, {.9f, .9f, .9f}, {1.2f, 1.2f, 1.2f}};
    const graphics::GlColor boxColors[] = {{1, .45f, .15f}, {.2f, .85f, .95f}, {.85f, .95f, .3f}};

    for (unsigned i = 0; i < 3; ++i) {
        BodyObject box;

        box.body.halfExtent = sizes[i] * .5f;
        box.body.localPosition = {0, roomBounds_.minimum.y + box.body.halfExtent.y, -2.3f + 2.3f * i};

        const auto entity = graphics::PatternSystem::addBox(scene, sizes[i], box.body.localPosition, boxColors[i]);

        if (!entity)
            return false;
        box.entity = *entity;
        objects_.push_back(box);
    }

    if (!addBall())
        return false;

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {0, .8f, 13};
    camera.target = {0, .25f, 0};
    camera.farPlane = 200;
    camera.orthographic = false;

    auto& render = app_.getRenderOptions();

    render.depthTest = true;
    render.cull = graphics::CullMode::Back;
    render.lighting = true;
    render.lightDirection = {.5f, 1, 2};
    printHelp();

    return syncScene();
}

bool Practice21::addBall() {
    if (ballCount_ >= 5) {
        std::cout << "공은 최대 5개입니다. R로 초기화할 수 있습니다.\n";

        return true;
    }

    const float radius = .24f;
    std::optional<glm::vec3> freePosition;

    for (int y = 0; y < 4 && !freePosition; ++y) {
        for (int z = 0; z < 5 && !freePosition; ++z) {
            for (int x = 0; x < 5 && !freePosition; ++x) {
                const glm::vec3 p(-2.2f + x * 1.1f, .1f + y * .5f, -2.6f + z * 1.3f);
                const graphics::Bounds3 candidate{p - glm::vec3(radius), p + glm::vec3(radius)};

                if (!roomBounds_.contains(p, glm::vec3(radius)))
                    continue;

                bool occupied = false;

                for (const auto& object : objects_) {
                    if (object.body.released)
                        continue;

                    const graphics::Bounds3 bodyBounds{object.body.localPosition - object.body.halfExtent,
                                                       object.body.localPosition + object.body.halfExtent};

                    if (candidate.overlaps(bodyBounds)) {
                        occupied = true;
                        break;
                    }
                }

                if (!occupied)
                    freePosition = p;
            }
        }
    }

    if (!freePosition) {
        std::cout << "공을 생성할 빈 위치가 없습니다. 잠시 후 B를 누르세요.\n";

        return true;
    }

    auto& scene = app_.getScene();
    const auto sphere = scene.addSphere(radius, *freePosition);

    if (!sphere || !scene.setTint(*sphere, {1, .85f - .12f * ballCount_, .15f + .15f * ballCount_}) ||
        !scene.setFaceCullMode(*sphere, 0, graphics::CullMode::None))
        return false;

    BodyObject ball;

    ball.entity = *sphere;
    ball.elastic = true;
    ball.body.localPosition = *freePosition;
    ball.body.halfExtent = glm::vec3(radius);
    ball.body.localVelocity = {1.5f + .2f * ballCount_, -1.4f - .12f * ballCount_, ballCount_ % 2 ? -1.8f : 1.8f};

    if (floorOpen_)
        releaseBody(ball);
    objects_.push_back(ball);
    ++ballCount_;
    std::cout << "공 생성: " << ballCount_ << "/5\n";

    return true;
}

bool Practice21::syncScene() {
    auto& scene = app_.getScene();
    const auto matrix = roomWorld();
    graphics::Transform3D roomTransform;

    roomTransform.rotation = glm::mat3(matrix);

    roomTransform.basis = glm::vec3(matrix * glm::vec4((roomBounds_.minimum + roomBounds_.maximum) * .5f, 1));

    if (!scene.setTransform(room_, roomTransform) || !scene.setFaceVisible(room_, 5, !floorOpen_))
        return false;

    for (const auto& object : objects_) {
        graphics::Transform3D transform;

        transform.basis = object.body.position(matrix);

        if (!object.elastic)
            transform.rotation = object.body.released ? object.releasedRotation : glm::mat3(matrix);

        if (!scene.setTransform(object.entity, transform))
            return false;
    }

    return true;
}

void Practice21::onKey(int key, int action, int mods) {
    if (!focused_ || (action != GLFW_PRESS && action != GLFW_REPEAT))
        return;

    const bool uppercase = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    if (key == GLFW_KEY_Z) {
        require(graphics::PatternSystem::moveCamera(app_.getCamera(), {0, 0, uppercase ? -.35f : .35f}),
                "카메라 Z 이동 실패");
    } else if (key == GLFW_KEY_Y) {
        require(graphics::PatternSystem::yawCamera(app_.getCamera(), uppercase ? 4.0f : -4.0f),
                "카메라 로컬 Y 자전 실패");
    } else if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_B) {
            require(addBall() && syncScene(), "공 추가 또는 표시변환 실패");
        } else if (key == GLFW_KEY_E || key == GLFW_KEY_F) {
            if (key == GLFW_KEY_F)
                floorOpen_ = !floorOpen_;

            if (floorOpen_ || key == GLFW_KEY_E) {
                for (auto& object : objects_)
                    if (!object.body.released)
                        releaseBody(object);
            }
            require(syncScene(), "바닥 가시성 변경 실패");
            std::cout << (key == GLFW_KEY_E ? "모든 내부 객체 이탈: 바닥 상태 유지\n"
                : floorOpen_ ? "바닥 열림: 모든 내부 객체 낙하\n" : "바닥 닫힘: 이탈한 객체는 계속 낙하\n");
        } else if (key == GLFW_KEY_V) {
            roomSpinning_ = !roomSpinning_;
            std::cout << (roomSpinning_ ? "방 Y축 자전 시작\n" : "방 Y축 자전 정지\n");
        } else if (key == GLFW_KEY_R) {
            require(initScene(), "장면 초기화 실패");
        } else if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) {
            app_.requestClose();
        } else if (key == GLFW_KEY_H) {
            printHelp();
        }
    }
}

void Practice21::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT)
        return;

    if (event.action == GLFW_RELEASE) {
        dragging_ = false;

        return;
    }

    if (event.action == GLFW_PRESS && focused_) {
        dragging_ = true;
        previousMouseX_ = event.windowX;
    }
}

void Practice21::onCursor(double x, double) {
    if (!dragging_ || !focused_)
        return;

    const float delta = glm::radians(float((previousMouseX_ - x) * .3));

    previousMouseX_ = x;

    if (!std::isfinite(delta))
        return;

    roomAngle_ = std::remainder(roomAngle_ + delta, glm::radians(360.0f));
    pendingRotation_ += delta;
}

void Practice21::onFocus(bool focused) {
    focused_ = focused;
    dragging_ = false;
    pendingRotation_ = 0;
}

void Practice21::onUpdate(float dt) {
    if (!focused_)
        return;

    if (dt > 0)
        lastDt_ = dt;
    const float spinSpeed = roomSpinning_ ? glm::radians(30.0f) : 0;

    roomSpinAngle_ = std::remainder(roomSpinAngle_ + spinSpeed * dt, glm::radians(360.0f));

    const auto matrix = roomWorld();
    const auto spin = glm::rotate(glm::mat4(1), roomSpinAngle_, glm::vec3(0, 1, 0));
    const glm::vec3 angularVelocity =
        glm::vec3(0, spinSpeed, 0) + glm::mat3(spin) * glm::vec3(0, 0, dt > 0 ? pendingRotation_ / dt : 0);

    if (dt > 0)
        pendingRotation_ = 0;

    for (auto& object : objects_) {
        const bool wasReleased = object.body.released;

        require(graphics::CollisionSystem::stepRoomBody(object.body, roomBounds_, matrix, dt, floorOpen_,
                                                        object.elastic, {0, -9.8f, 0}, angularVelocity),
                "실내 반사/미끄럼/낙하 계산 실패");

        if (!wasReleased && object.body.released)
            object.releasedRotation = glm::mat3(matrix);
    }
    require(syncScene(), "장면 표시변환 실패");
}

void Practice21::printHelp() const {
    std::cout << "실습 21: 실내 공 반사, 기울어진 바닥의 상자, 바닥 개방\n"
              << "z/Z: 카메라 월드 Z +/-, y/Y: 화면 로컬 Y 시계/반시계 자전\n"
              << "Shift XOR Caps Lock으로 소문자/대문자 구분. 카메라 키는 반복 허용.\n"
              << "B: 공 추가(초기 1개, 최대 5개), E: 전체 이탈, F: 바닥 열기/닫기\n"
              << "바닥 개방 즉시 모든 공/상자 중력 낙하. 개방 중 새 공도 낙하.\n"
              << "v: 방 Y축 30도/초 자전 시작/정지, 내부 벽 Front 컬링 유지\n"
              << "왼쪽 버튼 좌우 드래그: 공간 Z축 회전, 기울기에 따라 상자 미끄럼\n"
              << "R: 전체 초기화, H: 도움말, Q 또는 Esc: 종료\n"
              << "B/e/F/v/R/H/Q는 누른 순간 한 번만 적용. 포커스 상실 시 드래그 해제.\n";
}
}

std::unique_ptr<PracticeMode> makePractice21(graphics::Application& app) {
    return std::make_unique<Practice21>(app);
}
```

## standard/modes/Mode19Practice22.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "TankCommon.h"

std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app) {
    return makeTankMode(app, true);
}
```

## standard/modes/Mode1Ellipse.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path1() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
    return path;
}
```

## standard/modes/Mode20Practice23.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

namespace {
class Practice23 final : public PracticeMode {
    graphics::Application& app_;
    graphics::CharacterBody body_;
    graphics::JointRig rig_;
    graphics::EntityId stage_ = 0;
    std::vector<graphics::Bounds3> obstacles_;
    graphics::Bounds3 bounds_{{-5, 0, -5}, {5, 4, 5}};
    std::array<float, 4> doors_{};
    std::array<bool, 4> open_{};
    std::optional<std::size_t> activeWall_;

    unsigned randomSeed_ = std::random_device{}();

    std::mt19937 random_{randomSeed_};
    bool walking_ = true;

public:
    explicit Practice23(graphics::Application& app) : PracticeMode(app), app_(app) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float dt) override;
};

using practice_common::require;

namespace {

}

bool Practice23::initScene() {
    auto& scene = app_.getScene();

    scene.clear();
    obstacles_.clear();
    body_ = graphics::CharacterBody{};
    body_.feet = {-3, 0, -3};
    body_.speed = 1;
    body_.height = 1.8f;
    body_.halfWidth = {.5f, .5f};
    doors_.fill(0);
    open_.fill(false);
    activeWall_.reset();
    walking_ = true;

    const auto stage = graphics::PatternSystem::addRoom(scene, bounds_, false);

    if (!stage)
        return false;
    stage_ = *stage;

    for (std::size_t face = 0; face < 4; ++face) {
        if (!graphics::PatternSystem::openRoomWall(scene, stage_, face, 0))
            return false;
    }

    graphics::GridLayout grid;

    grid.origin = {-.5f, 0, -.3f};
    grid.extent = {1, 1.8f, .6f};

    const auto rig = graphics::PatternSystem::addWalkingRig(scene, grid);

    if (!rig)
        return false;
    rig_ = *rig;
    std::cout << "실습 23 난수 시드=" << randomSeed_ << "; 초기화 시 기존 난수 엔진의 다음 수열을 사용합니다.\n";

    std::vector<graphics::Bounds3> occupied{{{-3.8f, 0, -3.8f}, {-2.2f, 2, -2.2f}}};

    for (unsigned i = 0; i < 3; ++i) {
        const glm::vec3 size{.9f + .2f * i, .6f + .25f * i, .9f + .15f * i};
        const auto box =
            graphics::CollisionSystem::randomFreeBox({{-4, 0, -4}, {4, size.y, 4}}, size, occupied, random_);

        if (!box)
            return false;

        if (!graphics::PatternSystem::addBox(scene, size, (box->minimum + box->maximum) * .5f,
                                             {.3f + .2f * i, .5f, .8f - .2f * i}))
            return false;
        occupied.push_back(*box);
        obstacles_.push_back(*box);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 11};
    app_.getCamera().target = {0, 1, 0};
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().depthTest = true;
    app_.getRenderOptions().lighting = true;

    return graphics::PatternSystem::poseWalkingRig(scene, rig_, body_.feet, 0, 0, body_.speed);
}

void Practice23::onKey(int key, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT)
        return;

    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    auto& camera = app_.getCamera();

    if (key == GLFW_KEY_X || key == GLFW_KEY_Z) {
        glm::vec3 delta(0);

        delta[key == GLFW_KEY_X ? 0 : 2] = upper ? -.35f : .35f;
        require(graphics::PatternSystem::moveCamera(camera, delta), "실습 23 카메라 이동 실패.");

        return;
    }

    if (key == GLFW_KEY_Y) {
        require(graphics::PatternSystem::orbitCamera(camera, {camera.target, {0, 1, 0}}, upper ? 4.0f : -4.0f),
                "실습 23 카메라 공전 실패.");

        return;
    }

    if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD)
        body_.speed = std::min(3.0f, body_.speed + .1f);

    if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT)
        body_.speed = std::max(0.0f, body_.speed - .1f);

    if (key == GLFW_KEY_W && action == GLFW_PRESS) {
        body_.direction = {0, 0, 1};
        walking_ = true;
    }

    if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        body_.direction = {0, 0, -1};
        walking_ = true;
    }

    if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        body_.direction = {-1, 0, 0};
        walking_ = true;
    }

    if (key == GLFW_KEY_D && action == GLFW_PRESS) {
        body_.direction = {1, 0, 0};
        walking_ = true;
    }

    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_O) {
        if (activeWall_)
            open_[*activeWall_] = !open_[*activeWall_];
        else {
            activeWall_ = graphics::PatternSystem::cameraFacingWall(app_.getScene(), stage_, camera);

            if (activeWall_)
                open_[*activeWall_] = true;
        }

        if (activeWall_)
            std::cout << "선택한 벽=" << *activeWall_ << (open_[*activeWall_] ? " 열기\n" : " 원상복귀\n");
    }

    if (key == GLFW_KEY_SPACE)
        walking_ = !walking_;

    if (key == GLFW_KEY_J)
        graphics::CollisionSystem::jump(body_, 5.5f);

    if (key == GLFW_KEY_I)
        require(initScene(), "실습 23 초기화 실패: 앞선 API 오류를 확인하세요.");

    if (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE)
        app_.requestClose();
}

void Practice23::onUpdate(float dt) {
    const float requested = body_.speed;

    if (!walking_)
        body_.speed = 0;

    const auto result = graphics::CollisionSystem::stepCharacter(body_, bounds_, obstacles_, dt);

    body_.speed = requested;
    require(result.has_value(), "실습 23 캐릭터 충돌 계산 실패.");

    const float yaw = glm::degrees(std::atan2(body_.direction.x, body_.direction.z));

    require(graphics::PatternSystem::poseWalkingRig(app_.getScene(), rig_, body_.feet, yaw, body_.walkDistance,
                                                    body_.speed),
            "실습 23 로봇 관절 자세 적용 실패.");

    for (std::size_t face = 0; face < doors_.size(); ++face) {
        doors_[face] = std::clamp(doors_[face] + (open_[face] ? 1.0f : -1.0f) * dt, 0.0f, 1.0f);
        require(graphics::PatternSystem::openRoomWall(app_.getScene(), stage_, face, doors_[face]),
                "실습 23 벽 개방·복원 실패.");
    }

    if (activeWall_ && !open_[*activeWall_] && doors_[*activeWall_] == 0)
        activeWall_.reset();
}
}

std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app) {
    return std::make_unique<Practice23>(app);
}
```

## standard/modes/Mode21CameraCollision.cpp

### before

없음. 새 파일이다.

### after

```cpp
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
```

## standard/modes/Mode22EmbeddedMesh.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "PracticeMode.h"
#include "PracticeCommon.h"


namespace {
struct EmbeddedCorner {
    std::size_t vertex = 0;
    std::optional<std::size_t> uv;
};
using EmbeddedTriangle = std::array<EmbeddedCorner,3>;
struct EmbeddedResult {
    graphics::Model model;
    std::vector<std::string> errors;
};

inline EmbeddedResult validateEmbedded(const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec2>& uvs,const std::vector<EmbeddedTriangle>& triangles) {
    EmbeddedResult result;
    result.model.shape = graphics::GLShape::Custom;
    const auto validPosition = [](const glm::vec3& p) {
        return std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z)
            && glm::all(glm::lessThanEqual(glm::abs(p),glm::vec3(1)));
    };
    const auto validUV = [](const glm::vec2& uv) {
        return std::isfinite(uv.x) && std::isfinite(uv.y)
            && glm::all(glm::greaterThanEqual(uv,glm::vec2(0)))
            && glm::all(glm::lessThanEqual(uv,glm::vec2(1)));
    };
    for (std::size_t i=0; i<positions.size(); ++i)
        if (!validPosition(positions[i])) result.errors.push_back("Invalid vertex "+std::to_string(i));
    for (std::size_t i=0; i<uvs.size(); ++i)
        if (!validUV(uvs[i])) result.errors.push_back("Invalid UV "+std::to_string(i));
    for (std::size_t face=0; face<triangles.size(); ++face) {
        const auto& corners = triangles[face];
        const bool textured = corners[0].uv.has_value();
        bool valid = true;
        std::array<glm::vec3,3> points{};
        std::array<glm::vec2,3> texcoords{};
        for (std::size_t i=0; i<3; ++i) {
            if (corners[i].vertex>=positions.size() || corners[i].uv.has_value()!=textured) { valid=false; break; }
            points[i] = positions[corners[i].vertex];
            if (!validPosition(points[i])) { valid=false; break; }
            if (textured) {
                if (*corners[i].uv>=uvs.size()) { valid=false; break; }
                texcoords[i] = uvs[*corners[i].uv];
                if (!validUV(texcoords[i])) { valid=false; break; }
            }
        }
        if (valid)
            for (std::size_t i=0; i<3; ++i)
                for (std::size_t j=i+1; j<3; ++j)
                    if (corners[i].vertex==corners[j].vertex || glm::all(glm::equal(points[i],points[j]))
                        || (textured && glm::all(glm::equal(texcoords[i],texcoords[j])))) valid=false;
        glm::dvec3 normal(0);
        if (valid) {
            auto a=glm::dvec3(points[1])-glm::dvec3(points[0]);
            auto b=glm::dvec3(points[2])-glm::dvec3(points[0]);
            const auto scaleA=std::max({std::abs(a.x),std::abs(a.y),std::abs(a.z)});
            const auto scaleB=std::max({std::abs(b.x),std::abs(b.y),std::abs(b.z)});
            if (scaleA==0 || scaleB==0) valid=false;
            else {
                a/=scaleA; b/=scaleB; normal=glm::cross(a,b);
                if (glm::dot(normal,normal)<=1e-12*glm::dot(a,a)*glm::dot(b,b)) valid=false;
            }
        }
        if (!valid) { result.errors.push_back("Invalid triangle "+std::to_string(face)); continue; }
        std::vector<graphics::Vertex> vertices(3);
        for (std::size_t i=0; i<3; ++i) {
            vertices[i].position=points[i];
            vertices[i].normal=glm::vec3(glm::normalize(normal));
            vertices[i].uv=texcoords[i];
            vertices[i].color={.8f,.6f,.2f};
        }
        result.model.faces.push_back(graphics::Mesh::fromVertices(vertices,{0,1,2}));
    }
    if (result.model.faces.empty()) result.errors.push_back("No valid triangles.");
    return result;
}

class EmbeddedMesh final : public PracticeMode {
    graphics::Application& app_;
public:
    explicit EmbeddedMesh(graphics::Application& app) : PracticeMode(app), app_(app) {}
    bool initScene() override {
        const std::vector<glm::vec3> positions{
            {-.5f,-.5f,.5f},{.5f,-.5f,.5f},{0,.5f,0},{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f}};
        const std::vector<glm::vec2> uvs{{0,0},{1,0},{.5f,1}};
        const auto face=[](std::size_t a,std::size_t b,std::size_t c) {
            return EmbeddedTriangle{EmbeddedCorner{a,0},EmbeddedCorner{b,1},EmbeddedCorner{c,2}};
        };
        const auto result=validateEmbedded(positions,uvs,
            {face(0,1,2),face(1,4,2),face(4,3,2),face(3,0,2),face(0,3,4),face(0,4,1)});
        for (const auto& error : result.errors) std::cerr << error << '\n';
        if (!result.errors.empty()) return false;
        auto& scene=app_.getScene(); scene.clear();
        const auto model=scene.addModel(result.model,graphics::Transform3D{},graphics::PivotMode::Preserve);
        app_.getCamera()=graphics::Camera{};
        app_.getCamera().position={2,1.5f,3};
        return model.has_value();
    }
    void onKey(int key, int action, int) override {
        if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) app_.requestClose();
    }
};
}

std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app) {
    return std::make_unique<EmbeddedMesh>(app);
}
```

## standard/modes/Mode2Rectangle.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path2() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
    return path;
}
```

## standard/modes/Mode3Polygon.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path3() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polygon(
        {{1, -.5f, 0}, {2.4f, -.4f, 0}, {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
    return path;
}
```

## standard/modes/Mode4Spatial.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path4() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = [center](double u) {
        const double angle = u * 6.283185307179586;
        return center + glm::vec3(float(.7 * std::sin(2 * angle)), float(.6 * std::sin(angle)),
                                  float(.3 * std::cos(angle)));
    };
    path.interpolation = graphics::PathSystem::smoothStep;
    return path;
}
```

## standard/modes/Mode5Circle.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path5() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    return path;
}
```

## standard/modes/Mode6Polyline.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path6() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    path.path = graphics::PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
    path.playback = graphics::Playback::PingPong;
    return path;
}
```

## standard/modes/Mode7Grid.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "../Console.h"

/// 거리 기반 격자 예제를 구성한다.
bool Console::initGrid() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Grid;
    clock_.reset();
    gridBoxes_.clear();

    graphics::GridLayout grid;

    grid.origin = {-3, -.1f, -2};
    grid.extent = {6, .2f, 4};
    grid.cells = {6, 1, 4};

    for (int z = 0; z < 4; ++z)
        for (int x = 0; x < 6; ++x) {
            const auto center = grid.cellCenter({x, 0, z}), size = grid.relativeSize({.15f, 1, .22f});

            if (!center || !size)
                return false;
            const auto tile = graphics::PatternSystem::addBox(scene, *size, *center,
                (x + z) % 2 ? graphics::GlColor{.25f, .3f, .4f} : graphics::GlColor{.4f, .45f, .55f});
            if (!tile || !addControl(*tile, "grid tile " + std::to_string(z * 6 + x + 1)))
                return false;
        }

    if (!distancePath_.setPoints({{-2.5f, .35f, -1.5f}, {2.5f, .35f, -1.5f}, {2.5f, .35f, 1.5f}}))
        return false;

    for (unsigned i = 0; i < 5; ++i) {
        const auto frame = scene.addGroup();
        if (!frame)
            return false;
        const auto box = graphics::PatternSystem::addBox(scene, {.4f, .5f, .4f}, {0, 0, 0},
            {.25f + .14f * i, .7f - .1f * i, .3f}, *frame);

        if (!box || !graphics::PatternSystem::addNumber(scene, i + 1, {-.07f, -.1f, .205f}, .2f, {1, 1, 1}, *box))
            return false;
        if (!addControl(*box, "moving box " + std::to_string(i + 1)))
            return false;
        gridBoxes_.push_back(*frame);
    }
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {7, 6, 8};
    app_.getCamera().target = {0, 0, 0};
    app_.getCamera().orthographicHalfExtent = 4;

    selectedJoint_ = 24; // 처음에는 첫 이동 상자를 조작한다.
    return true;
}

bool Console::updateGrid() {
    auto& scene = app_.getScene();
    bool ok = true;
    for (std::size_t i = 0; i < gridBoxes_.size(); ++i)
        ok = graphics::PatternSystem::follow(scene, gridBoxes_[i], distancePath_,
            clock_.seconds + distancePath_.length() * i / gridBoxes_.size()) && ok;
    return ok;
}
```

## standard/modes/Mode8Room.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "../Console.h"

bool Console::initRoom() {
    auto& scene = app_.getScene();
    scene.clear();
    demo_ = Demo::Room;
    floorOpen_ = false;
    const auto frame = scene.addGroup(), bodyFrame = scene.addGroup();
    const auto room = graphics::PatternSystem::addRoom(scene, {{-2, -1, -2}, {2, 2, 2}});
    if (!frame || !bodyFrame || !room || !scene.setParent(*room, *frame))
        return false;
    const auto cube = graphics::PatternSystem::addBox(scene, {.5f, .5f, .5f}, {0, 0, 0}, {1, .7f, .2f}, *bodyFrame);
    if (!cube)
        return false;
    room_ = *room;
    roomCube_ = *cube;
    roomFrame_ = *frame;
    bodyFrame_ = *bodyFrame;
    roomControl_ = jointControls_.size();
    if (!addControl(roomFrame_, "room", ControlKind::RoomFrame))
        return false;
    bodyControl_ = jointControls_.size();
    if (!addControl(roomCube_, "physical box", ControlKind::RoomBody))
        return false;
    roomBody_ = graphics::RoomBody{};
    roomBody_.localPosition = {0, -.75f, 0};
    releasedRotation_ = glm::mat3(1);
    app_.getCamera().position = {5, 3, 8};
    app_.getCamera().target = {0, 0, 0};
    return true;
}

bool Console::updateRoom(float dt) {
    auto& scene = app_.getScene();
    if (!roomControl_ || !bodyControl_)
        return false;
    const auto& manual = animation_[jointControls_[*roomControl_].track].rest;
    const auto& box = animation_[jointControls_[*bodyControl_].track].rest;
    const float angle = .4f * float(std::sin(clock_.seconds));
    const auto automatic = glm::rotate(glm::mat4(1), angle, glm::vec3(0, 0, 1));
    const auto matrix = graphics::DiagramSystem::modelMatrix(manual) * automatic;
    graphics::Transform3D roomTransform;
    roomTransform.rotation = glm::mat3(automatic);
    roomTransform.basis = glm::vec3(automatic * glm::vec4(0, .5f, 0, 1));
    const glm::vec3 radius(.25f * box.scale.x);
    roomBody_.halfExtent = glm::abs(box.rotation[0]) * radius.x + glm::abs(box.rotation[1]) * radius.y +
                          glm::abs(box.rotation[2]) * radius.z;
    if (!roomBody_.released) {
        const auto low = glm::vec3(-2, -1, -2) + roomBody_.halfExtent;
        const auto high = glm::vec3(2, 2, 2) - roomBody_.halfExtent;
        roomBody_.localPosition.x = std::clamp(roomBody_.localPosition.x, low.x, high.x);
        roomBody_.localPosition.z = std::clamp(roomBody_.localPosition.z, low.z, high.z);
        roomBody_.localPosition.y = floorOpen_ ? std::min(roomBody_.localPosition.y, high.y)
                                             : std::clamp(roomBody_.localPosition.y, low.y, high.y);
        releasedRotation_ = glm::mat3(matrix);
    }
    const auto angularVelocity = manual.rotation * glm::vec3(0, 0, clock_.playing ? .4f * float(std::cos(clock_.seconds)) : 0);
    if (!scene.setTransform(room_, roomTransform) || !scene.setFaceVisible(room_, 5, !floorOpen_) ||
        !graphics::CollisionSystem::stepRoomBody(roomBody_, {{-2, -1, -2}, {2, 2, 2}}, matrix, dt, floorOpen_,
                                                  false, {0, -9.8f, 0}, angularVelocity))
        return false;
    graphics::Transform3D bodyTransform;
    bodyTransform.basis = roomBody_.position(matrix);
    bodyTransform.rotation = releasedRotation_;
    return scene.setTransform(bodyFrame_, bodyTransform);
}
```

## standard/modes/Mode9Animation.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "../Console.h"
#include <iostream>

/// 선형보간과 그네·사람의 부모·자식 애니메이션을 구성한다.
bool Console::initAnimation() {
    auto& scene = app_.getScene();

    scene.clear();
    demo_ = Demo::Animation;
    clock_.reset();
    animation_.clear();
    jointControls_.clear();
    selectedJoint_ = 0;
    selectedAxis_ = 0;

    if (!graphics::PatternSystem::addBox(scene, {8, .15f, 6}, {0, -.15f, 0}, {.25f, .3f, .35f}))
        return false;

    graphics::SwingConfig swingConfig;

    swingConfig.pivot = {1, 3, 0};
    swingConfig.pendulum.amplitudeDegrees = 25;
    swingConfig.pendulum.periodSeconds = 3;

    const auto swing = graphics::AnimationSystem::addSwingWithRider(scene, swingConfig);

    if (!swing)
        return false;
    animation_ = swing->animation;

    graphics::HumanoidConfig personConfig;

    personConfig.grid.frame = glm::translate(glm::mat4(1), glm::vec3(-2, 0, 0));

    const auto person = graphics::AnimationSystem::addHumanoid(scene, personConfig);

    if (!person)
        return false;

    auto pose = graphics::HumanoidPose::seated();

    pose.hips = {glm::vec3(0), glm::vec3(0)};
    pose.knees = {glm::vec3(0), glm::vec3(0)};

    if (!graphics::AnimationSystem::poseHumanoid(scene, *person, pose))
        return false;

    for (std::size_t i = 0; i < 2; ++i) {
        graphics::PendulumMotion sway;

        sway.amplitudeDegrees = 30;
        sway.periodSeconds = 2;
        sway.phaseDegrees = i == 0 ? 0 : 180;

        graphics::Motion motion;

        if (!motion.append(graphics::Motion::pendulum(sway)))
            return false;

        const auto track = graphics::AnimationSystem::createJointAnimation(scene, person->shoulders[i], motion);

        if (!track)
            return false;
        animation_.push_back(*track);
    }

    const auto marker = scene.addGroup();

    if (!marker || !graphics::PatternSystem::addBox(scene, {.4f, .4f, .4f}, {0, 0, 0}, {1, .85f, .15f}, *marker))
        return false;

    graphics::LinearMotion move;

    move.from = {-2, .3f, 2};
    move.to = {2, .3f, 2};
    move.durationSeconds = 4;
    move.playback = graphics::Playback::PingPong;

    graphics::Motion motion;

    if (!motion.append(graphics::Motion::linear(move)))
        return false;

    const auto track = graphics::AnimationSystem::createJointAnimation(scene, *marker, motion);

    if (!track)
        return false;
    animation_.push_back(*track);

    const auto control = [&](graphics::EntityId id, const std::string& name, bool whole = false) {
        return addControl(id, name, whole ? ControlKind::Object : ControlKind::Joint);
    };

    const auto personControls = [&](const graphics::HumanoidRig& rig, const std::string& prefix) {
        if (!control(rig.root, prefix + " root", true) || !control(rig.pelvis, prefix + " pelvis") ||
            !control(rig.torso, prefix + " torso") || !control(rig.neck, prefix + " neck"))
            return false;

        for (std::size_t i = 0; i < 2; ++i) {
            const auto side = prefix + (i == 0 ? " left " : " right ");

            if (!control(rig.shoulders[i], side + "shoulder") || !control(rig.elbows[i], side + "elbow") ||
                !control(rig.hands[i], side + "hand") || !control(rig.hips[i], side + "hip") ||
                !control(rig.knees[i], side + "knee") || !control(rig.feet[i], side + "foot"))
                return false;
        }

        return true;
    };

    if (!control(swing->frame, "swing frame", true) || !control(swing->hinge, "swing hinge") ||
        !control(swing->seat, "swing seat") || !personControls(swing->person, "rider") ||
        !personControls(*person, "standing") || !control(*marker, "linear marker", true))
        return false;
    app_.getCamera() = graphics::Camera{};
    app_.getCamera().position = {8, 5, 10};
    app_.getCamera().target = {0, 1.4f, 0};
    std::cout << "9: 노란 상자는 4초 편도 선형보간, 그네는 3초 주기 진자, 좌석의 사람은 계층 변환을 따릅니다.\n";

    return graphics::AnimationSystem::poseHierarchy(scene, animation_, 0);
}

bool Console::updateAnimation() {
    return graphics::AnimationSystem::poseHierarchy(app_.getScene(), animation_, clock_.seconds);
}
```

## standard/modes/MotionCommon.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "../Console.h"
#include "Paths.h"
#include <iostream>

/// 공통 Motion을 모드별 경로 정의로 교체한다.
bool Console::choosePath(int kind) {
    using namespace standard_modes;
    using PathFactory = graphics::PathMotion (*)();
    const PathFactory paths[] = {path1, path2, path3, path4, path5, path6};
    if (kind < 1 || kind > 6 || !pathId_)
        return false;
    graphics::Motion motion;
    auto& motions = app_.getScene().getMotions();
    return motion.append(graphics::Motion::followPath(paths[kind - 1]())) &&
           motions.setMotion(*pathId_, motion) && motions.seekMotion(*pathId_, 0);
}

/// 장면 객체와 초기 상태를 구성한다.
bool Console::initMotion(int kind) {
    demo_ = Demo::Motion;

    auto& scene = app_.getScene();

    scene.clear();
    paused_ = false;
    spinEnabled_ = true;
    globalEnabled_ = false;
    reflection_ = 1;
    insertedLayer_.reset();

    const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
    const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});

    if (!first || !second) {
        return false;
    }

    if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) {
        return false;
    }

    auto& motions = scene.getMotions();

    pathId_ = motions.createMotion(graphics::Motion{});

    graphics::RotationMotion spin;

    spin.degreesPerSecond = 90;
    spinId_ = motions.createSpin(spin);

    graphics::RotationMotion global;

    global.degreesPerSecond = 15;
    globalId_ = motions.createOrbit(global);

    if (!pathId_ || !spinId_ || !globalId_ || !choosePath(kind)) {
        return false;
    }

    if (!motions.setMotionEnabled(*globalId_, false) ||
        !motions.bindMotion(graphics::MotionSystem::rootLayer(), *globalId_)) {
        return false;
    }

    const auto normalPath = motions.pushLayer(graphics::MotionSystem::rootLayer());
    const auto reflection = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});

    if (!normalPath || !reflection) {
        return false;
    }

    graphics::LayerConfig mirror;

    mirror.transform = *reflection;
    mirrorLayer_ = motions.pushLayer(graphics::MotionSystem::rootLayer(), mirror);

    if (!mirrorLayer_) {
        return false;
    }

    const auto mirroredPath = motions.pushLayer(*mirrorLayer_);

    normalSpinLayer_ = motions.pushLayer(*normalPath);

    if (!mirroredPath || !normalSpinLayer_) {
        return false;
    }

    const auto mirroredSpin = motions.pushLayer(*mirroredPath);

    if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId_) || !motions.bindMotion(*mirroredPath, *pathId_) ||
        !motions.bindMotion(*normalSpinLayer_, *spinId_) || !motions.bindMotion(*mirroredSpin, *spinId_) ||
        !scene.bindEntity(*first, *normalSpinLayer_) || !scene.bindEntity(*second, *mirroredSpin)) {
        return false;
    }

    if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first) ||
        !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second) ||
        !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) {
        return false;
    }

    if (!addControl(*first, "original box") || !addControl(*second, "reflected box"))
        return false;

    auto& camera = app_.getCamera();

    camera.position = {5, 3, 8};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};

    return true;
}

/// 반사 변환을 순서대로 변경한다.
bool Console::changeReflection() {
    const int next = (reflection_ + 1) % 4;
    std::optional<glm::mat4> matrix = glm::mat4(1);

    if (next == 1) {
        matrix = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
    } else if (next == 2) {
        matrix = graphics::TransformSystem::reflectPoint({0, 0, 0});
    } else if (next == 3) {
        matrix = graphics::TransformSystem::reflectLine(graphics::AxisLine{});
    }

    if (!mirrorLayer_ || !matrix) {
        return false;
    }

    graphics::LayerConfig config;

    config.transform = *matrix;

    if (!app_.getScene().getMotions().setLayer(*mirrorLayer_, config)) {
        return false;
    }
    reflection_ = next;

    const char* names[] = {"없음", "YZ평면 반사", "원점 점대칭", "Y축 직선대칭"};

    std::cout << "반사: " << names[reflection_] << '\n';

    return true;
}

/// 크기 레이어를 삽입하거나 제거한다.
bool Console::toggleInsertedLayer() {
    auto& motions = app_.getScene().getMotions();

    if (insertedLayer_) {
        if (!motions.removeLayer(*insertedLayer_)) {
            return false;
        }
        insertedLayer_.reset();
        std::cout << "추가 크기 레이어 삭제\n";

        return true;
    }

    const auto scale = graphics::TransformSystem::scaling(glm::vec3(1.5f));

    if (!normalSpinLayer_ || !scale) {
        return false;
    }

    graphics::LayerConfig config;

    config.space = graphics::TransformSpace::Local;
    config.transform = *scale;
    insertedLayer_ = motions.insertLayer(*normalSpinLayer_, config);

    if (!insertedLayer_) {
        return false;
    }
    std::cout << "첫 객체에 1.5배 Local 크기 레이어 삽입\n";

    return true;
}
```

## standard/modes/Paths.h

### before

없음. 새 파일이다.

### after

```cpp
#pragma once
#include "../framework/Framework.h"

namespace standard_modes {
graphics::PathMotion path1();
graphics::PathMotion path2();
graphics::PathMotion path3();
graphics::PathMotion path4();
graphics::PathMotion path5();
graphics::PathMotion path6();
}
```

## standard/modes/PracticeCommon.h

### before

없음. 새 파일이다.

### after

```cpp
#pragma once
#include "../framework/Framework.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace practice_common {
inline void assignVertexColors(graphics::Polyhedron& shape) {
    const std::array<graphics::GlColor, 8> palette{graphics::GlColor{1.0f, 0.2f, 0.2f},
                                                   {0.2f, 1.0f, 0.2f},
                                                   {0.2f, 0.4f, 1.0f},
                                                   {1.0f, 1.0f, 0.2f},
                                                   {1.0f, 0.2f, 1.0f},
                                                   {0.2f, 1.0f, 1.0f},
                                                   {1.0f, 0.6f, 0.2f},
                                                   {0.7f, 0.3f, 1.0f}};

    for (auto& face : shape.faces) {
        for (const auto& vertex : face.localVertices) {
            const int index = (vertex.x > 0.0f ? 1 : 0) | (vertex.y > 0.0f ? 2 : 0) | (vertex.z > 0.0f ? 4 : 0);

            face.vertexColors.push_back(palette[index]);
        }
    }
}

inline glm::mat4 rotateAround(const glm::vec3& pivot, const glm::vec3& axis, float degrees) {
    return glm::translate(glm::mat4(1.0f), pivot) * glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axis) *
           glm::translate(glm::mat4(1.0f), -pivot);
}

inline glm::mat4 scaleAround(const glm::vec3& pivot, float scale) {
    return glm::translate(glm::mat4(1.0f), pivot) * glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
           glm::translate(glm::mat4(1.0f), -pivot);
}

inline glm::mat3 turn(float degrees, glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1), glm::radians(degrees), axis));
}

inline void require(bool success, const char* operation) {
    if (!success)
        throw std::runtime_error(operation);
}

inline graphics::EntityId requireEntity(std::optional<graphics::EntityId> id, const char* reason) {
    if (!id)
        throw std::runtime_error(reason);

    return *id;
}
} // namespace practice_common
```

## standard/modes/PracticeMode.h

### before

없음. 새 파일이다.

### after

```cpp
#pragma once
#include "../framework/Framework.h"
#include <memory>

// Each practice owns its state; Console alone owns callback registration and switching.
class PracticeMode {
    graphics::Application& host_;
public:
    explicit PracticeMode(graphics::Application& app) : host_(app) {}
    virtual ~PracticeMode() = default;
    virtual bool initScene() = 0;
    virtual void onKey(int, int, int) = 0;
    virtual void onUpdate(float) {}
    virtual void onMouse(const graphics::MouseEvent&) {}
    virtual void onCursor(double, double) {}
    virtual void onFocus(bool) {}
    virtual bool onRender() { return host_.renderScene(); }
};

constexpr int lastStandardMode = 22;
std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice20(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice21(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app);
std::unique_ptr<PracticeMode> makeCameraCollision(graphics::Application& app);
std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app);
```

## standard/modes/PracticeRegistry.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app) {
    switch (mode) {
    case 10: return makePractice13(app);
    case 11: return makePractice14(app);
    case 12: return makePractice15(app);
    case 13: return makePractice16(app);
    case 14: return makePractice17(app);
    case 15: return makePractice18(app);
    case 16: return makePractice19(app);
    case 17: return makePractice20(app);
    case 18: return makePractice21(app);
    case 19: return makePractice22(app);
    case 20: return makePractice23(app);
    case 21: return makeCameraCollision(app);
    case 22: return makeEmbeddedMesh(app);
    default: return {};
    }
}
```

## standard/modes/TankCommon.cpp

### before

없음. 새 파일이다.

### after

```cpp
#include "TankCommon.h"
#include "PracticeCommon.h"
#include <array>
#include <vector>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>

namespace {
class TankMode final : public PracticeMode {
    graphics::Application& app_;
    bool threeViews_;
    graphics::EntityId root_ = 0, central_ = 0;
    std::array<graphics::EntityId, 2> lower_{}, upper_{}, guns_{}, poles_{};
    std::vector<graphics::EntityId> colored_;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{0};
    float lowerTime_ = 0, centralAngle_ = 0, gunAngle_ = 0, poleTime_ = 0;
    float swapProgress_ = 0, swapDirection_ = 1;
    float cameraX_ = 0, cameraZ_ = 0, cameraYaw_ = 0, cameraOrbit_ = 0;
    bool lowerRunning_ = false, centralRunning_ = false, gunRunning_ = false, poleRunning_ = false;
    bool swapRunning_ = false, swapStarted_ = false;

    unsigned colorVersion_ = 0;

    graphics::EntityId box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent);

    void reset();
    void apply();
    void recolor();

public:
    TankMode(graphics::Application& app, bool threeViews) : PracticeMode(app), app_(app), threeViews_(threeViews) {
    }

    bool initScene() override;

    void onKey(int key, int action, int mods) override;
    void onUpdate(float dt) override;
    void onFocus(bool focused) override;

    bool onRender() override;
};

using practice_common::requireEntity;

namespace {

}

graphics::EntityId TankMode::box(glm::vec3 size, glm::vec3 position, graphics::EntityId parent) {
    const auto id = requireEntity(graphics::PatternSystem::addBox(app_.getScene(), size, position, {1, 1, 1}, parent),
                                  "탱크 상자 생성 실패: 크기·위치·부모 ID를 확인하세요.");

    colored_.push_back(id);

    return id;
}

bool TankMode::initScene() {
    reset();
    std::cout << (threeViews_ ? "실습 22: 원근 / XZ 평면도 / XY 정면도\n" : "실습 19: 탱크와 부품·카메라 조작\n")
              << "방향키 이동; d 하부 높이; t 중앙 자전; l 상부 교환\n"
              << "g 포신 반대 회전; p 깃대 반대 기울기; x/X,z/Z 카메라 이동\n"
              << "y/Y 카메라 시선 회전; r/R 공전(재입력 정지); O/o 색 변경; c 초기화; q 종료\n";

    return true;
}

void TankMode::reset() {
    auto& scene = app_.getScene();

    scene.clear();
    colored_.clear();
    pressed_.fill(false);
    position_ = glm::vec3(0);
    lowerTime_ = centralAngle_ = gunAngle_ = poleTime_ = swapProgress_ = 0;
    swapDirection_ = 1;
    cameraX_ = cameraZ_ = cameraYaw_ = cameraOrbit_ = 0;
    lowerRunning_ = centralRunning_ = gunRunning_ = poleRunning_ = swapRunning_ = swapStarted_ = false;
    colorVersion_ = 0;
    root_ = requireEntity(scene.addGroup(), "탱크 루트 생성 실패.");
    central_ = requireEntity(graphics::PatternSystem::addJoint(scene, root_, {0, .6f, 0}), "중앙 관절 생성 실패.");

    const auto ground = requireEntity(scene.addGroup(), "바닥 그룹 생성 실패.");

    box({12, .1f, 12}, {0, -.05f, 0}, ground);

    for (std::size_t i = 0; i < 2; ++i)
        lower_[i] = box({.65f, .6f, 1.8f}, {i == 0 ? -.8f : .8f, .3f, 0}, root_);
    box({2.3f, .25f, 1.4f}, {0, .125f, 0}, central_);

    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;

        upper_[i] = requireEntity(graphics::PatternSystem::addJoint(scene, central_, {sign * .65f, .25f, 0}),
                                  "상부 관절 생성 실패.");
        guns_[i] = requireEntity(graphics::PatternSystem::addJoint(scene, upper_[i], {0, .3f, 0}),
                                 "포신 중심 피벗 생성 실패.");
        poles_[i] = requireEntity(graphics::PatternSystem::addJoint(scene, upper_[i], {0, .6f, 0}),
                                  "깃대 밑점 피벗 생성 실패.");
        box({.65f, .6f, .8f}, {0, .3f, 0}, upper_[i]);

        box({.14f, .14f, 1.1f}, {0, -.05f, .95f}, guns_[i]);
        box({.08f, .85f, .08f}, {0, .425f, 0}, poles_[i]);
    }

    auto& camera = app_.getCamera();

    camera = graphics::Camera{};
    camera.position = {-5, 4.5f, 7};
    camera.target = {0, .7f, 0};
    camera.orthographic = false;
    camera.orthographicHalfExtent = 6.5f;
    app_.getRenderOptions() = graphics::RenderOptions{};
    app_.getRenderOptions().lighting = true;
    recolor();
    apply();
}

void TankMode::recolor() {
    const std::array<graphics::GlColor, 10> palette{graphics::GlColor{.3f, .35f, .4f},
                                                    {.25f, .6f, .3f},
                                                    {.55f, .35f, .2f},
                                                    {.2f, .55f, .75f},
                                                    {.7f, .75f, .2f},
                                                    {.9f, .4f, .15f},
                                                    {1, .8f, .2f},
                                                    {.4f, .7f, .8f},
                                                    {.7f, .25f, .6f},
                                                    {.6f, .9f, .4f}};

    for (std::size_t i = 0; i < colored_.size(); ++i)
        if (!app_.getScene().setTint(colored_[i], palette[(i + colorVersion_) % palette.size()]))
            throw std::runtime_error("탱크 부품 색상 적용 실패.");
}

void TankMode::apply() {
    auto& scene = app_.getScene();

    if (!scene.setPosition(root_, position_))
        throw std::runtime_error("탱크 위치 적용 실패.");

    const float factor = 1 + .25f * std::sin(lowerTime_), height = .6f * factor;

    if (!scene.setPosition(central_, {0, height, 0}) ||
        !graphics::PatternSystem::rotateJoint(scene, central_, {0, 1, 0}, centralAngle_))
        throw std::runtime_error("중앙 관절 변환 적용 실패.");

    for (std::size_t i = 0; i < 2; ++i) {
        const float sign = i == 0 ? -1.0f : 1.0f;
        graphics::Transform3D low;

        low.basis = {sign * .8f, height * .5f, 0};
        low.scale = {1, factor, 1};

        const float a = 3.14159265f * swapProgress_;
        const glm::vec3 upperPosition{sign * .65f * std::cos(a), .25f, sign * .65f * std::sin(a)};

        if (!scene.setTransform(lower_[i], low) || !scene.setPosition(upper_[i], upperPosition) ||
            !graphics::PatternSystem::rotateJoint(scene, guns_[i], {0, 1, 0}, sign * gunAngle_) ||
            !graphics::PatternSystem::rotateJoint(scene, poles_[i], {1, 0, 0}, sign * 60 * std::sin(poleTime_)))
            throw std::runtime_error("탱크 부품 변환 적용 실패.");
    }
}

void TankMode::onKey(int key, int action, int mods) {
    const bool upper = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);

    if (key == GLFW_KEY_X || key == GLFW_KEY_Z) {
        if (action == GLFW_PRESS || action == GLFW_RELEASE) {
            float& direction = key == GLFW_KEY_X ? cameraX_ : cameraZ_;

            direction = action == GLFW_RELEASE ? 0 : (upper ? -1.0f : 1.0f);
        }

        return;
    }

    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS)
            pressed_[key] = true;
        else if (action == GLFW_RELEASE)
            pressed_[key] = false;
    }

    if (action != GLFW_PRESS)
        return;

    const float direction = upper ? 1.0f : -1.0f;

    switch (key) {
    case GLFW_KEY_D:
        lowerRunning_ = !lowerRunning_;
        break;
    case GLFW_KEY_T:
        centralRunning_ = !centralRunning_;
        break;
    case GLFW_KEY_G:
        gunRunning_ = !gunRunning_;
        break;
    case GLFW_KEY_P:
        poleRunning_ = !poleRunning_;
        break;
    case GLFW_KEY_L:
        if (!swapStarted_) {
            swapStarted_ = true;
            swapRunning_ = true;
        } else if (!swapRunning_ && (swapProgress_ == 0 || swapProgress_ == 1)) {
            swapDirection_ = -swapDirection_;
            swapRunning_ = true;
        } else
            swapRunning_ = !swapRunning_;
        break;
    case GLFW_KEY_Y:
        cameraYaw_ = cameraYaw_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_R:
        cameraOrbit_ = cameraOrbit_ == direction ? 0 : direction;
        break;
    case GLFW_KEY_O:
        colorVersion_ = (colorVersion_ + 1) % 10;
        recolor();
        break;
    case GLFW_KEY_C:
        reset();
        break;
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE:
        app_.requestClose();
        break;
    default:
        break;
    }
}

void TankMode::onUpdate(float dt) {
    glm::vec3 movement{float(pressed_[GLFW_KEY_RIGHT]) - float(pressed_[GLFW_KEY_LEFT]), 0,
                       float(pressed_[GLFW_KEY_DOWN]) - float(pressed_[GLFW_KEY_UP])};

    if (glm::length(movement) > 0)
        position_ += glm::normalize(movement) * dt * 1.3f;

    position_.x = std::clamp(position_.x, -3.5f, 3.5f);
    position_.z = std::clamp(position_.z, -3.5f, 3.5f);

    if (lowerRunning_)
        lowerTime_ = std::fmod(lowerTime_ + dt, 6.283185307f);

    if (centralRunning_)
        centralAngle_ = std::fmod(centralAngle_ + 45 * dt, 360.0f);

    if (gunRunning_)
        gunAngle_ = std::fmod(gunAngle_ + 60 * dt, -360.0f);

    if (poleRunning_)
        poleTime_ = std::fmod(poleTime_ + dt, 6.283185307f);

    if (swapRunning_) {
        swapProgress_ = std::clamp(swapProgress_ + swapDirection_ * dt * .5f, 0.0f, 1.0f);

        if (swapProgress_ == 0 || swapProgress_ == 1)
            swapRunning_ = false;
    }

    auto camera = app_.getCamera();
    const glm::vec3 delta{cameraX_ * 2 * dt, 0, cameraZ_ * 2 * dt};

    if (!graphics::PatternSystem::moveCamera(camera, delta) ||
        (cameraYaw_ != 0 && !graphics::PatternSystem::yawCamera(camera, cameraYaw_ * 35 * dt)) ||
        (cameraOrbit_ != 0 &&
         !graphics::PatternSystem::orbitCamera(camera, {{0, 0, 0}, {0, 1, 0}}, cameraOrbit_ * 25 * dt)))
        throw std::runtime_error("카메라 갱신 실패: 위치·시선·회전축을 확인하세요.");
    app_.getCamera() = camera;
    apply();
}

void TankMode::onFocus(bool focused) {
    if (!focused) {
        pressed_.fill(false);
        cameraX_ = cameraZ_ = 0;
    }
}

bool TankMode::onRender() {
    if (!threeViews_)
        return app_.renderScene();
    const auto size = app_.getFramebufferSize();

    if (size[0] < 3 || size[1] < 2)
        return true;

    const auto passes =
        graphics::PatternSystem::threeViews(app_.getCamera(), size[0], size[1], app_.getRenderOptions());

    if (passes.size() != 3)
        throw std::runtime_error("세 카메라 뷰 생성 실패: 프레임버퍼 크기와 카메라를 확인하세요.");

    return app_.renderViews(passes);
}
}

std::unique_ptr<PracticeMode> makeTankMode(graphics::Application& app, bool threeViews) {
    return std::make_unique<TankMode>(app, threeViews);
}
```

## standard/modes/TankCommon.h

### before

없음. 새 파일이다.

### after

```cpp
#pragma once
#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makeTankMode(graphics::Application& app, bool threeViews);
```

## standard/standard.vcxproj

### before

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{AD50DC3F-7731-4954-923F-907BB2A8277C}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <ProjectName>standard</ProjectName>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <Import Project="framework\Toolchain.props" />
  <PropertyGroup Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries Condition="'$(Configuration)'=='Debug'">true</UseDebugLibraries>
    <UseDebugLibraries Condition="'$(Configuration)'=='Release'">false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <OutDir>$(ProjectDir)x64\$(Configuration)\</OutDir>
    <IntDir>$(ProjectDir)x64\$(Configuration)\obj\</IntDir>
    <TargetName>standard</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <WarningLevel>Level4</WarningLevel>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebug</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreaded</RuntimeLibrary>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Animation.h" />
    <ClInclude Include="framework\include\Transform.h" />
    <ClInclude Include="framework\include\Patterns.h" />
    <ClInclude Include="framework\include\GridLayout.h" />
    <ClInclude Include="framework\include\Diagnostics.h" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
```

### after

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{AD50DC3F-7731-4954-923F-907BB2A8277C}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <ProjectName>standard</ProjectName>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <Import Project="framework\Toolchain.props" />
  <PropertyGroup Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries Condition="'$(Configuration)'=='Debug'">true</UseDebugLibraries>
    <UseDebugLibraries Condition="'$(Configuration)'=='Release'">false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <OutDir>$(ProjectDir)x64\$(Configuration)\</OutDir>
    <IntDir>$(ProjectDir)x64\$(Configuration)\obj\</IntDir>
    <TargetName>standard</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <WarningLevel>Level4</WarningLevel>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebug</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreaded</RuntimeLibrary>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="modes\*.cpp" />
    <ClInclude Include="modes\*.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Animation.h" />
    <ClInclude Include="framework\include\Transform.h" />
    <ClInclude Include="framework\include\Patterns.h" />
    <ClInclude Include="framework\include\GridLayout.h" />
    <ClInclude Include="framework\include\Diagnostics.h" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
```

## standard/standard.vcxproj.filters

### before

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Transform.h" />
  </ItemGroup>
</Project>
```

### after

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="modes\*.cpp" />
    <ClInclude Include="modes\*.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Transform.h" />
  </ItemGroup>
</Project>
```

## tools/Update-StandardModesGraph.py

### before

없음. 새 파일이다.

### after

```python
"""Generate a code-only structure graph. Does not build, test, or run diagnostics."""
from pathlib import Path
import datetime
import hashlib
import json
import re
import subprocess
import xml.etree.ElementTree as ET

from graphify.extract import extract
from graphify.build import build_from_json
from graphify.cluster import cluster
from graphify.export import to_json, to_html

ROOT = Path(__file__).resolve().parent.parent
OUT = ROOT / 'graphify-out/standard-input'


def write(name, value):
    (OUT / name).write_text(value.rstrip() + '\n', encoding='utf-8-sig')


def dump(name, value):
    write(name, json.dumps(value, ensure_ascii=False, indent=2))


def inventory():
    projects = []
    ns = {'m': 'http://schemas.microsoft.com/developer/msbuild/2003'}
    tracked = subprocess.check_output(['git', 'ls-files', '*.vcxproj'], cwd=ROOT, text=True, encoding='utf-8')
    historical = json.loads((ROOT / 'graphify-out/PROJECTS.json').read_text(encoding='utf-8-sig'))
    for relative in tracked.splitlines():
        path = ROOT / relative
        if not path.is_file():
            prior = next((p for p in historical if p.get('project') == relative), {})
            projects.append({**prior, 'project': relative, 'available': False,
                             'note': 'Absent before this task. Entry information is historical; no file was restored or deleted.'})
            continue
        sources = set()
        for element in ET.parse(path).findall('.//m:ClCompile', ns):
            include = element.get('Include')
            if include and '$(' not in include:
                sources.update(p.resolve() for p in path.parent.glob(include.replace('\\', '/')) if p.is_file())
        bundled = path.parent / 'framework/detail'
        if bundled.is_dir():
            sources.update(p.resolve() for p in bundled.glob('*.cpp'))
        entries = []
        for source in sorted(sources):
            text = source.read_text(encoding='utf-8-sig')
            for match in re.finditer(r'\b(?:int|void)\s+(main|wmain)\s*\(', text):
                entries.append({'file': source.relative_to(ROOT).as_posix(), 'name': match[1],
                                'line': text[:match.start()].count('\n') + 1,
                                'conditional_compilation': 'Review preprocessor branches in this source.' if '#if' in text else None})
        projects.append({'project': relative, 'available': True,
                         'sources': [p.relative_to(ROOT).as_posix() for p in sorted(sources)], 'entrypoints': entries})
    for name in ('basic', 'assets', 'motion'):
        projects.append({'standalone_example': True, 'file': f'framework/examples/{name}.cpp', 'entrypoint': 'main'})
    return projects


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    excluded = {'.git', '.vs', '.agents', '.codex', 'dependencies', 'packages', 'archive',
                'x64', 'Debug', 'Release', '.obj-work', 'dist', 'tests'}
    files = []
    for path in ROOT.rglob('*'):
        rel = path.relative_to(ROOT)
        if path.suffix.lower() not in {'.cpp', '.h', '.hpp'} or any(p in excluded for p in rel.parts):
            continue
        if 'framework' in rel.parts and rel.parts[0] != 'framework':
            continue
        files.append(path)
    # Include the two owning standard copies changed in this task, rather than every SDK distribution.
    files += [ROOT / 'standard/framework/include/Picking.h', ROOT / 'standard/framework/detail/Application.cpp']
    files = sorted(set(files))
    extraction = extract(files, cache_root=OUT, root=ROOT)
    node_sources = {n['id']: n.get('source_file', '').replace('\\', '/') for n in extraction['nodes']}
    for edge in extraction['edges']:
        a = node_sources.get(edge.get('source'), '').split('/')[0]
        b = node_sources.get(edge.get('target'), '').split('/')[0]
        if a and b and a != b and b != 'framework' and edge.get('relation') == 'calls':
            edge['confidence'] = 'AMBIGUOUS'
            edge['audit_note'] = 'Independent executables can have identically named classes. Confirm against owning sources.'
    graph = build_from_json(extraction, root=str(ROOT), directed=True)
    communities = cluster(graph)
    commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=ROOT, text=True).strip()
    to_json(graph, communities, str(OUT / 'graph.json'), force=True, built_at_commit=commit)
    to_html(graph, communities, str(OUT / 'graph.html'))
    for name in ('graph.json', 'graph.html'):
        write(name, (OUT / name).read_text(encoding='utf-8-sig'))
    dump('EXTRACTION.json', extraction)
    dump('PROJECTS.json', inventory())
    dump('SOURCE_MANIFEST.json', {
        'date': datetime.datetime.now(datetime.timezone(datetime.timedelta(hours=9))).date().isoformat(), 'source_commit': commit,
        'method': 'graphify AST; no semantic API, build, tests, app run, or extraction diagnostics',
        'nodes': graph.number_of_nodes(), 'edges': graph.number_of_edges(),
        'files': {p.relative_to(ROOT).as_posix(): hashlib.sha256(p.read_bytes()).hexdigest() for p in files},
        'limits': ['Dynamic callback and virtual-dispatch order requires source review.',
                   'SDKs, tests, archives and duplicate framework distributions are excluded.',
                   'Missing pre-existing projects remain historical entries.']})
    print(f'graphify AST: {len(files)} files, {graph.number_of_nodes()} nodes, {graph.number_of_edges()} edges. No tests or diagnostics run.')


if __name__ == '__main__':
    main()
```
