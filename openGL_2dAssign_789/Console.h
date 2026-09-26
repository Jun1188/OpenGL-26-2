#pragma once

#include "DiagramState.h"

#include <cstdint>
#include <filesystem>
#include <limits>

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

class PracticeConsole : public Console {
public:
    explicit PracticeConsole(GLFWwindow* window) : window_(window) {}
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

class Practice7Console final : public PracticeConsole {
public:
    using PracticeConsole::PracticeConsole;
    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, Point point) override;
};

class Practice8Console final : public PracticeConsole {
public:
    using PracticeConsole::PracticeConsole;
    void onKey(int key, int action) override;
    void onMouseButton(int, int, Point) override {}
};

class Practice9Console final : public PracticeConsole {
public:
    using PracticeConsole::PracticeConsole;
    void update(float deltaTime) override { store_.advance(deltaTime); }
    void onKey(int key, int action) override;
    void onMouseButton(int, int, Point) override {}
};
