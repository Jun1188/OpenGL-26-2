#pragma once

#include "Common.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <vector>

constexpr float hitX_Left = -0.9f;
constexpr float hitX_Right = 0.3f;
constexpr float hitY_Min = -0.4f;
constexpr float hitY_Max = 0.4f;
constexpr std::size_t max_step = 10;

enum class RectState { None, Moving, HitAnimated, Fixed };

struct RectAttribute {
    RectState rs = RectState::None;
    std::optional<std::size_t> Index_;
    std::size_t step = 0;
    float direction = 1.0f;
    float speed = 0.0f;
    Point start{};
    Point target{};
};

class DiagramStore {
public:
    DiagramStore();
    void reset();
    bool advance(float deltaTime);
    bool checkHit();
    bool hitAnimation();
    std::vector<Diagram> getItems() const;
    std::uint64_t getRevision() const { return revision_; }

private:
    void spawnPair();

    // 0-1: moving rectangles, 2-3: movement areas, 4: hit area.
    std::array<Diagram, 5> fixedDiagrams_;
    std::array<RectAttribute, 2> rects_{};
    std::vector<Diagram> stackedDiagrams_;
    float animationTime_ = 0.0f;
    std::uint64_t revision_ = 0;
};
