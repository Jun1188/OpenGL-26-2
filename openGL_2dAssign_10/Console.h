#pragma once

#include "DiagramState.h"

#include <cstdint>
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
    explicit PracticeConsole(GLFWwindow* window, DiagCaseTable dct);
    bool initGraphics(const std::filesystem::path& shaderDirectory);
    void shutdownGraphics();
    bool render() override;
    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, Point point) override;

    


private:
    bool handleQuit(int key, int action);
    GLFWwindow* window_ = nullptr;
    DiagramStore store_;
    ShaderHandler shader_;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
};


