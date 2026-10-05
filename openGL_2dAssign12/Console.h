#pragma once

#include "DiagramState.h"

#include <cstdint>
#include <filesystem>
#include <limits>
#include <vector>

struct GLFWwindow;

class Console {
public:
    virtual ~Console() = default;
    virtual void update(float) {}
    virtual void onKey(int key, int action) = 0;
    virtual void onMouseButton(int button, int action, Point point) = 0;
    virtual void onCursorMove(Point) {}
    virtual bool render() = 0;
};

constexpr std::size_t EFFECT_STEP = 5;


class PracticeConsole : public Console {
public:
    explicit PracticeConsole(GLFWwindow* window)
        : window_(window) {}
    bool initGraphics(const std::filesystem::path& shaderDirectory);
    void shutdownGraphics();
    bool render() override;

protected:
    bool handleQuit(int key, int action);
    GLFWwindow* window_ = nullptr;
    DiagramStore store_;

private:
    ShaderHandler shader_;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
};

class Practice11Console final : public PracticeConsole {
public:
    explicit Practice11Console(GLFWwindow* window);
    void update(float deltaTime) override;
    void onKey(int key, int action) override;
    void onMouseButton(int, int, Point) override {} // Practice 11 has no mouse command.

private:
    float moveTime_ = 0.0f;
    float effectTime_ = 0.0f;
};
