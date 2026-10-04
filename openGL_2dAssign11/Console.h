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

class Grid { // Board owns diagrams, the active index, and collision effects.
public:
    explicit Grid(std::size_t gridX = 20, std::size_t gridY = 20);
    bool getIsEndPos() const;
    bool stepEffect(std::size_t idx);
    bool moveUp(std::size_t step, std::size_t idx);
    bool moveDown(std::size_t step, std::size_t idx);
    bool moveLeft(std::size_t step, std::size_t idx);
    bool moveRight(std::size_t step, std::size_t idx);
    Diagram effectBy(const Diagram& changed) const;
    void resetBoard();
    void moveSelectedTo();
    void moveByStep();
    void advanceEffects();
    float getStepDuration() const;
    std::vector<Diagram> getRenderItems() const;
    std::uint64_t getRevision() const { return revision_; }

private:
    struct Cell {
        std::size_t x = 0;
        std::size_t y = 0;
    };
    Point cellCenter(Cell cell) const;
    bool moveTo(Cell cell, std::size_t idx);

    const std::size_t gridX, gridY; // Constructor inputs, never hard-coded in movement.
    const float squareX, squareY;
    float speedVertical = 5.0f;   // Cells per second.
    float speedHorizontal = 8.0f;
    std::size_t selectedIndex_ = 0;
    bool movingRight_ = true;
    DiagramStore store_;
    std::vector<Cell> cells_; // Same indices as store_.getItems().
    std::vector<Diagram> boardLines_;
    std::vector<Diagram> effects_;
    std::vector<std::size_t> effectSteps_; // Same indices as effects_.
    std::uint64_t revision_ = 0;
};

class PracticeConsole : public Console {
public:
    explicit PracticeConsole(GLFWwindow* window, std::size_t gridX = 20,
                             std::size_t gridY = 20)
        : window_(window), grid_(gridX, gridY) {}
    bool initGraphics(const std::filesystem::path& shaderDirectory);
    void shutdownGraphics();
    bool render() override;

protected:
    bool handleQuit(int key, int action);
    GLFWwindow* window_ = nullptr;
    Grid grid_;

private:
    ShaderHandler shader_;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
};

class Practice11Console final : public PracticeConsole {
public:
    explicit Practice11Console(GLFWwindow* window, std::size_t gridX = 20,
                               std::size_t gridY = 20);
    void update(float deltaTime) override;
    void onKey(int key, int action) override;
    void onMouseButton(int, int, Point) override {} // Practice 11 has no mouse command.

private:
    float moveTime_ = 0.0f;
    float effectTime_ = 0.0f;
};
