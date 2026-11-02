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

    virtual void update(float) {
    }

    virtual void onKey(int key, int action) = 0;
    virtual void onMouseButton(int button, int action, Point point) = 0;

    virtual void onCursorMove(Point) {
    }

    virtual bool render() = 0;
};

class PracticeConsole : public Console {
public:
    explicit PracticeConsole(GLFWwindow* window) : window_(window) {
    }

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
    explicit Practice8Console(GLFWwindow* window);
    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, Point point) override;

    bool render() override;

private:
    enum class Command { None, Fill, Outline, Grow, Shrink, Reset, Quit };

    Command parseKey(int key, int action) const;

    void input(Command command);
    void reset();

    bool replaceItems(const std::vector<Diagram>& items);

    bool outline_ = false;
    float resizeStep_ = 0.02f;
};

class Practice9Console final : public PracticeConsole {
public:
    explicit Practice9Console(GLFWwindow* window);
    void update(float deltaTime) override;
    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, Point point) override;

    bool render() override;

private:
    enum class Mode { Bounce, HorizontalZigzag, VerticalZigzag, Spiral };

    struct Motion {
        Point anchor{};
        Point velocity{};

        float speed = 0.0f;
        float angle = 0.0f;
        bool zigzagHorMov = true;
        float zigSignY = 1.0f;

        float angularSpeed = 0.0f;
        float radius = 0.0f;
        float radialSpeed = 0.0f;
        float radialDirection = 1.0f;

        size_t stepYcnt = 0;
    };

    void reset();
    void resetMotion(std::size_t triangleIndex);
    void selectMode(Mode mode);
    void advanceLinear(std::size_t triangleIndex, float deltaTime);
    void advanceSpiral(std::size_t triangleIndex, float deltaTime);

    std::vector<Motion> motions_;
    Mode mode_ = Mode::Bounce;
    bool outline_ = false;
    float resizeStep_ = 0.02f;
};
