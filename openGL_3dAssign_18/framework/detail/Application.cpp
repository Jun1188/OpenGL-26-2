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

    if (!renderer_.initialize(vertexPath, fragmentPath))
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
