#pragma once

#include "Common.h"

#include <cstdint>
#include <filesystem>
#include <limits>

struct GLFWwindow;

class Console {
public:
    explicit Console(GLFWwindow* window) : window_(window) {}
    bool initGraphics(const std::filesystem::path& shaderDirectory);
    void shutdownGraphics();
    void onKey(int key, int action);
    void resizeFramebuffer(int width, int height);
    bool render();

    Entity& getEntities() { return entities_; } // 후속 입력·시스템 코드의 연결점.
    Camera& getCamera() { return camera_; }

private:
    bool initScene();

    GLFWwindow* window_ = nullptr;
    Entity entities_;
    ShaderHandler shader_;
    Camera camera_;
    int framebufferWidth_ = 0;
    int framebufferHeight_ = 0;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
};
