#pragma once
#include <algorithm>
#include <iostream>
#include <utility>
namespace graphics::detail {
inline bool& applicationActive() { static bool active=false;return active; }
}
namespace graphics {

inline Application::~Application() {
    shutdown();
}

inline void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

inline void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

inline void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

inline void Application::setModifiedKeyHandler(
    std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

inline void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

inline void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

inline bool Application::initialize(const ApplicationConfig& config) {
    Diagnostics::trace(PipelineStage::Runtime, "Application::initialize");
    const auto root=detail::relativePathInside(detail::executableDirectory(),config.projectRoot);
    const auto runtime=detail::relativePathInside(root,config.runtimeDirectory);
    detail::glfw().load(runtime);
    detail::glfw().SetErrorCallback([](int code, const char* message) {
        Diagnostics::fail(PipelineStage::Runtime, "GLFW", std::to_string(code) + ": " + (message ? message : "unknown error"),
            "Check the graphics driver, OpenGL 3.3 support, window dimensions and the bundled GLFW DLL.");
    });
    if (config.width <= 0 || config.height <= 0)
        return Diagnostics::fail(PipelineStage::Runtime, "Application::initialize", "Window width and height must be positive.");
    if (!detail::glfw().Init())
        return Diagnostics::fail(PipelineStage::Runtime, "glfwInit", "GLFW initialization failed; see the GLFW error above.");
    glfwInitialized_ = true;
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    detail::glfw().WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    detail::glfw().WindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    detail::glfw().WindowHint(GLFW_DEPTH_BITS, 24);
    window_ = detail::glfw().createWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        return Diagnostics::fail(PipelineStage::Runtime, "glfwCreateWindow", "Could not create an OpenGL 3.3 core window.", "Check the GLFW error above and update the graphics driver.");
    }
    detail::glfw().MakeContextCurrent(window_);
    detail::glfw().SwapInterval(config.verticalSync ? 1 : 0);
    const auto vertexPath=config.shaderDirectory.empty()?std::filesystem::path{}:detail::relativePathInside(root,config.shaderDirectory)/"VertexShader.glsl";
    const auto fragmentPath=config.shaderDirectory.empty()?std::filesystem::path{}:detail::relativePathInside(root,config.shaderDirectory)/"FragmentShader.glsl";
    if(!renderer_.initialize(vertexPath,fragmentPath)) return false;
    assets_.emplace(root,config.features);
    renderOptions_.textures=config.features.textures;

    detail::glfw().SetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    detail::glfw().SetWindowUserPointer(window_, this);
    detail::glfw().SetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&] {
            if(app.modifiedKeyHandler_)app.modifiedKeyHandler_(app,key,action,mods);
            else if(app.keyHandler_)app.keyHandler_(app,key,action);
        });
    });
    detail::glfw().SetWindowFocusCallback(window_, [](GLFWwindow* window,int focused) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&]{if(app.focusHandler_)app.focusHandler_(app,focused==GLFW_TRUE);});
    });
    detail::glfw().SetMouseButtonCallback(window_, [](GLFWwindow* window,int button,int action,int mods) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&] {
            if(!app.mouseHandler_)return;
            MouseEvent event;event.button=button;event.action=action;event.mods=mods;
            detail::glfw().GetCursorPos(window,&event.windowX,&event.windowY);
            int ww=0,wh=0,fw=0,fh=0;
            detail::glfw().GetWindowSize(window,&ww,&wh);detail::glfw().GetFramebufferSize(window,&fw,&fh);
            if(ww<=0||wh<=0||fw<=0||fh<=0)return;
            event.framebufferX=event.windowX*fw/ww;event.framebufferY=event.windowY*fh/wh;
            app.mouseHandler_(app,event);
        });
    });
    detail::glfw().SetCursorPosCallback(window_, [](GLFWwindow* window,double x,double y) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&]{if(app.cursorHandler_)app.cursorHandler_(app,x,y);});
    });
    return true;
}

inline void Application::shutdown() {
    assets_.reset();
    if (window_ != nullptr) {
        detail::glfw().MakeContextCurrent(window_);
        renderer_.destroy();
        detail::glfw().SetWindowUserPointer(window_, nullptr);
        detail::glfw().DestroyWindow(window_);
        window_ = nullptr;
    }
    if (glfwInitialized_) {
        detail::glfw().Terminate();
        glfwInitialized_ = false;
    }
}

inline void Application::requestClose() {
    if (window_ != nullptr) {
        detail::glfw().SetWindowShouldClose(window_, GLFW_TRUE);
    }
}

inline bool Application::renderScene() {
    if (window_ == nullptr) {
        return false;
    }
    detail::glfw().MakeContextCurrent(window_);
    int width = 0;
    int height = 0;
    detail::glfw().GetFramebufferSize(window_, &width, &height);
    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

inline std::array<int,2> Application::getFramebufferSize() const {
    std::array<int,2> size{};
    if(window_) detail::glfw().GetFramebufferSize(window_,&size[0],&size[1]);
    return size;
}
inline bool Application::renderViews(const std::vector<RenderPass>& passes) {
    if(!window_) return Diagnostics::fail(PipelineStage::Framebuffer,"Application::renderViews","No active window/context.");
    detail::glfw().MakeContextCurrent(window_);
    const auto size=getFramebufferSize();
    return renderer_.renderViews(scene_,passes,size[0],size[1]);
}

inline int Application::run(const ApplicationConfig& config) {
    Diagnostics::configure(config.diagnostics);
    Diagnostics::clear();
    if (running_) {
        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }
    if(detail::applicationActive()) return -1;
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    detail::applicationActive()=true;
    callbackError_=nullptr;
    running_ = true;
    int exitCode = 0;
    try {
        if (!initialize(config)) {
            std::cerr << "Application initialization failed.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            std::cerr << "User scene initialization failed.\n";
            exitCode = -1;
        } else {
            double previousTime = detail::glfw().GetTime();
            while (!detail::glfw().WindowShouldClose(window_)) {
                detail::glfw().PollEvents();
                if(callbackError_)std::rethrow_exception(callbackError_);
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                int width = 0;
                int height = 0;
                detail::glfw().GetFramebufferSize(window_, &width, &height);
                if (width <= 0 || height <= 0) {
                    detail::glfw().WaitEvents();
                    if(callbackError_)std::rethrow_exception(callbackError_);
                    previousTime = detail::glfw().GetTime();
                    continue;
                }
                const double currentTime = detail::glfw().GetTime();
                const float deltaTime = static_cast<float>(
                    std::clamp(currentTime - previousTime, 0.0, 0.05));
                previousTime = currentTime;
                if (updateHandler_) {
                    Diagnostics::trace(PipelineStage::Input, "Application::updateHandler");
                    updateHandler_(*this, deltaTime);
                }
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().MakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
                if (!rendered) {
                    std::cerr << "Rendering failed.\n";
                    exitCode = -1;
                    break;
                }
                detail::glfw().SwapBuffers(window_);
            }
        }
    } catch (const std::exception& error) {
        Diagnostics::fail(PipelineStage::Runtime, "Application::run", error.what(), "Read the preceding stage/API error; user callback exceptions are rethrown after cleanup.");
        shutdown();
        running_ = false;
        detail::applicationActive()=false;
        throw;
    } catch (...) {
        Diagnostics::fail(PipelineStage::Runtime, "Application::run", "Non-standard exception in a callback or runtime operation.");
        shutdown();
        running_ = false;
        detail::applicationActive()=false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    detail::applicationActive()=false;
    return exitCode;
}

inline void Application::setMouseHandler(std::function<void(Application&,const MouseEvent&)> handler) {
    mouseHandler_=std::move(handler);
}
inline void Application::setCursorHandler(std::function<void(Application&,double,double)> handler) {
    cursorHandler_=std::move(handler);
}
inline std::optional<PickResult> Application::pickAt(double x,double y) const {
    if(!window_)return {};
    int ww=0,wh=0,fw=0,fh=0;
    detail::glfw().GetWindowSize(window_,&ww,&wh);detail::glfw().GetFramebufferSize(window_,&fw,&fh);
    if(ww<=0||wh<=0||fw<=0||fh<=0)return {};
    return graphics::pick(scene_,camera_,x*fw/ww,y*fh/wh,{0,0,double(fw),double(fh)});
}
inline std::optional<PickResult> Application::pickAtCursor() const {
    if(!window_)return {};
    double x=0,y=0;detail::glfw().GetCursorPos(window_,&x,&y);return pickAt(x,y);
}
inline AssetLoader& Application::getAssets() {
    if(!assets_)throw std::logic_error("AssetLoader is available during run callbacks only.");
    return *assets_;
}
} // namespace graphics
