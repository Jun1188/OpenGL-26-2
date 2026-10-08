#pragma once

#include "Graphics.h"

#include <filesystem>
#include <optional>

struct GLFWwindow;

class Console {
public:
    using KeyHandler = void (*)(Console&, int key, int action, void* userData);
    using UpdateHandler = void (*)(Console&, float deltaTime, void* userData);

private:
    GLFWwindow* window_ = nullptr;
    graphics::Scene scene_;
    graphics::Renderer renderer_;
    graphics::Camera camera_;
    std::optional<graphics::EntityId> cuboidId_;
    std::optional<graphics::EntityId> tetrahedronId_;
    int framebufferWidth_ = 0;
    int framebufferHeight_ = 0;
    KeyHandler keyHandler_ = nullptr;
    UpdateHandler updateHandler_ = nullptr;
    void* keyUserData_ = nullptr;
    void* updateUserData_ = nullptr;

    bool initScene();

public:
    explicit Console(GLFWwindow* window) : window_(window) {}
    Console(const Console&) = delete;
    Console& operator=(const Console&) = delete;

    bool initGraphics(const std::filesystem::path& shaderDirectory);
    void shutdownGraphics();
    // userData는 등록된 콜백을 사용하는 동안 호출자가 유지한다.
    void setKeyHandler(KeyHandler handler, void* userData = nullptr);
    void setUpdateHandler(UpdateHandler handler, void* userData = nullptr);
    void onKey(int key, int action);
    void update(float deltaTime);
    void resizeFramebuffer(int width, int height);
    bool hasFramebuffer() const;
    bool render();

    graphics::Scene& getScene() { return scene_; }
    graphics::Camera& getCamera() { return camera_; }
    std::optional<graphics::EntityId> getCuboidId() const { return cuboidId_; }
    std::optional<graphics::EntityId> getTetrahedronId() const { return tetrahedronId_; }
};
