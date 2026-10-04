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




constexpr size_t EFFECT_STEP = 5;

class Grid { // 격자판 관리 클래스
    const size_t gridX, gridY;
    const float squareX, squareY;
    float speedVertical = 1.0f;
    float speedHorizontal = 1.0f;
    size_t selectedIndex_;
    DiagramStore store_; // Diagrams들은 Grid class 레이어에서 grid에 fitting하게 하는 보정을 받는다
    std::vector<Diagram> effects_{}; //effect가 되는 diagram들을 제어한다.

    std::vector<size_t> effectSteps_{}; //effects_와 idx를 공유하며 동시에 관리(push, pop, reference)된다. effect timer step으로, 이후 deltaTime 등 수치와 곱해져 실질 effect 속도가 나온다.
    
public:
    explicit Grid(size_t gridX = 20, size_t gridY = 20) : gridX{ gridX }, gridY{ gridY },
        squareX{ 2.0f / static_cast<float>(gridX) }, squareY{ 2.0f / static_cast<float>(gridY) } {
    }

    bool getIsEndPos() const { }// end pos면 movebystep에 따른 이동이 끝난다.
    bool stepEffect(const size_t idx) { return (effectSteps_[idx] = (effectSteps_[idx] + 1) % EFFECT_STEP) ? true : false; }
    //해당 idx에 맞는 effect들이 호출된다. 처음 호출 후 true 동안 effect를 하고, false가 되면 effect가 끝난다.
    //effect 처리 자체는 console 레이어가 처리한다.
    //effect는 line loop로 채워지지 않은 도형을 effects에 push하고 step마다 모션 블러 형태로 scale up하되, 자연스러운 모션을 위해 선형 보간보다는 루트(abs()) 바탕의 증가율이 둔화하는 보간을 한다.

    bool moveUp(const size_t step, const size_t idx);
    bool moveDown(const size_t step, const size_t idx);
    bool moveLeft(const size_t step, const size_t idx);
    bool moveRight(const size_t step, const size_t idx);
    Diagram effectBy(const Diagram& changed); // chaged에 맞는 effect diagram을 반환한다. 해당 diagram은 발생후 일정 step만큼 effect를 발생한다. 

    void resetBoard(); //reset 단계에서 clear 후, diagram들은 grid에 맞게 store에 push된다. selected는 (0, 0) grid 위치에 테두리 사각형으로 push된다.
    
    void moveSelectedTo();// diagrams[selectedIndex_]가 diagram이 있는 grid로 이동하면, selectedIndex가 해당 diagram의 index으로 옮겨간다.
    //그러면 render에서 보이는 주인공의 색과 모양이 바뀌게 된다

    void moveByStep(); // 1 step마다 diagrams[selectedIndex_]의 direction을 이동시킨다.

    
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
    Grid grid_;

private:
    ShaderHandler shader_;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
};


class Practice11Console final : public PracticeConsole {
public:
    explicit Practice11Console(GLFWwindow* window);
    void update(float deltaTime) override;
    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, Point point) override;
    bool render() override;

private:
    

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
    void advanceLinear(std::size_t triangleIndex, float deltaTime);

    std::vector<Motion> motions_;
    bool outline_ = false;
    float resizeStep_ = 0.02f;
};
