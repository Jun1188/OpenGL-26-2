#pragma once

#include "DiagramState.h"

#include <cstdint>
#include <filesystem>
#include <limits>

struct GLFWwindow;

class Console {
public:
    virtual ~Console() = default;
    virtual void update(float deltaTime) = 0;
    virtual void onKey(int key, int action) = 0;
    virtual bool render() = 0;
};

class PracticeConsole : public Console {
public:
    explicit PracticeConsole(GLFWwindow* window) : window_(window) {
    }

    bool initGraphics(const std::filesystem::path& shaderDirectory);

    void shutdownGraphics();

    bool render() override;

    void update(float deltaTime) override;
    void onKey(int key, int action) override;

private:
    bool handleQuit(int key, int action);

    GLFWwindow* window_ = nullptr;
    DiagramStore store_;
    ShaderHandler shader_;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
};
