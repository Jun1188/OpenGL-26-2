#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "../include/Application.h"
#include <glfw3.h>

#include <algorithm>
#include <iostream>
#include <utility>
#ifdef _WIN32
#include <windows.h>
#endif

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

void Application::setModifiedKeyHandler(
    std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

std::filesystem::path Application::findShaderDirectory() {
    const auto containsShaders = [](const std::filesystem::path& path) {
        return std::filesystem::is_regular_file(path / "VertexShader.glsl")
            && std::filesystem::is_regular_file(path / "FragmentShader.glsl");
    };
#ifdef _WIN32
    // 실행 파일 옆의 shaders를 우선한다. 작업 폴더와 프로젝트 이름에 의존하지 않는다.
    std::wstring executable(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(),
                                           static_cast<DWORD>(executable.size()));
    if (length > 0 && length < executable.size()) {
        executable.resize(length);
        const auto candidate = std::filesystem::path(executable).parent_path() / "shaders";
        if (containsShaders(candidate)) {
            return candidate;
        }
    }
#endif
    auto directory = std::filesystem::current_path();
    for (;;) {
        const auto candidate = directory / "framework" / "shaders";
        if (containsShaders(candidate)) {
            return candidate;
        }
        if (directory == directory.parent_path()) {
            return {};
        }
        directory = directory.parent_path();
    }
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::initialize(const ApplicationConfig& config) {
    if (config.width <= 0 || config.height <= 0 || !glfwInit()) {
        return false;
    }
    glfwInitialized_ = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    window_ = glfwCreateWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(config.verticalSync ? 1 : 0);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return false;
    }
    if (!renderer_.initialize(config.shaderDirectory / "VertexShader.glsl",
                              config.shaderDirectory / "FragmentShader.glsl")) {
        return false;
    }

    glfwSetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app.modifiedKeyHandler_) {
            app.modifiedKeyHandler_(app, key, action, mods);
        } else if (app.keyHandler_) {
            app.keyHandler_(app, key, action);
        }
    });
    glfwSetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app.focusHandler_) {
            app.focusHandler_(app, focused == GLFW_TRUE);
        }
    });
    return true;
}

void Application::shutdown() {
    if (window_ != nullptr) {
        glfwMakeContextCurrent(window_);
        renderer_.destroy();
        glfwSetWindowUserPointer(window_, nullptr);
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    if (glfwInitialized_) {
        glfwTerminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window_);
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

int Application::run(const ApplicationConfig& config) {
    if (running_) {
        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }
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
            double previousTime = glfwGetTime();
            while (!glfwWindowShouldClose(window_)) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window_)) {
                    break;
                }
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(window_, &width, &height);
                if (width <= 0 || height <= 0) {
                    glfwWaitEvents();
                    previousTime = glfwGetTime();
                    continue;
                }
                const double currentTime = glfwGetTime();
                const float deltaTime = static_cast<float>(
                    std::clamp(currentTime - previousTime, 0.0, 0.05));
                previousTime = currentTime;
                if (updateHandler_) {
                    updateHandler_(*this, deltaTime);
                }
                if (glfwWindowShouldClose(window_)) {
                    break;
                }
                glfwMakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
                if (!rendered) {
                    std::cerr << "Rendering failed.\n";
                    exitCode = -1;
                    break;
                }
                glfwSwapBuffers(window_);
            }
        }
    } catch (...) {
        shutdown();
        running_ = false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    return exitCode;
}

} // namespace graphics
