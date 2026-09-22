#pragma once

#include "Common.h"

#include <cstddef>
#include <optional>
#include <vector>






class RectStore {
public:
    using Index = std::size_t;

    const std::vector<AnimatedRect>& getItems() const;
    Rect* getSelected();
    const Rect* getSelected() const;
    std::optional<Index> getSelectedIndex() const;

    std::optional<Index> findTopmostAt(
        const Point& point,
        std::optional<Index> excludedIndex = std::nullopt
    ) const;

    void select(std::optional<Index> index);
    void clearSelection();
    void clear();
    void push(const Rect& rect);
    void push(const AnimatedRect& rect);
    bool erase(Index index);
    std::size_t size() const;
    AnimatedRect& get(Index index);
    const AnimatedRect& get(Index index) const;
    void captureCurrentState();
    void updateCapture(const Rect& r);
    bool restoreCapturedState();

private:
    std::vector<AnimatedRect> rects_;
    std::vector<AnimatedRect> capture_;
    std::optional<Index> selectedIndex_;
};





enum class Section {
    Section1,
    Section2,
    Section3,
    Section4,
    Error
};

class Practice2RectState final {
public:
    bool push(Section section);
    Rect* select(const Point& point);
    bool scaleSelected(float amount);
    bool recolorSelected();
    void removeAll();

    const std::vector<AnimatedRect>& getItems() const;
    Rect* getSelected();
    const Rect* getSelected() const;
    std::vector<std::size_t> getRenderOrder() const;

private:
    std::size_t countInSection(Section section) const;
    void removeOldestInSection(Section section);

    RectStore store_;
};





class Practice3RectState final {
public:
    bool pushRandom();
    Rect* select(const Point& point);

    bool beginDrag(const Point& point);
    void dragTo(const Point& point);
    bool endDrag();
    bool splitAt(const Point& point);

    bool isDragging() const;
    const std::vector<AnimatedRect>& getItems() const;
    Rect* getSelected();
    const Rect* getSelected() const;
    std::vector<std::size_t> getRenderOrder() const;

private:
    std::optional<RectStore::Index> findTopmostOverlap(
        RectStore::Index draggedIndex
    ) const;
    bool merge(RectStore::Index firstIndex, RectStore::Index secondIndex);

    RectStore store_;
    bool dragging_ = false;
    Point dragOffset_{};
};



class Practice4RectState final {
public:
    void update(float deltaTime);
    bool tryCreateRectAt(const Point& point);

    bool rollbackAtCapture();

    void toggleDiagonal();
    void toggleHorizontalZigzag();
    void toggleOrbit();
    void toggleResize();
    void toggleRecolor();
    void stopAll();
    void reset();
    void printFlag();

    const std::vector<AnimatedRect>& getItems() const;

private:
    bool canCreateRectAt(const Point& center) const;
    bool hasActiveAnimation() const;
    void captureIfNeeded();
    void initializeOrbitState();

    Point calculateDiagonalMovement(
        const AnimatedRect& item,
        float deltaTime
    ) const;
    Point calculateZigzagMovement(
        const AnimatedRect& item,
        float deltaTime
    ) const;
    Point calculateOrbitMovement(
        AnimatedRect& item,
        float deltaTime,
        const Point& additionalMovement
    ) const;
    void moveWithinWindow(
        AnimatedRect& item,
        const Point& movement
    );
    void updateResize(AnimatedRect& item, float deltaTime);
    void updateRecolor(AnimatedRect& item, float deltaTime);

    RectStore store_;
    bool isDiagonal_ = false;
    bool isHorizontalZigzag_ = false;
    bool isOrbit_ = false;
    bool isResize_ = false;
    bool isRecolor_ = false;
    bool hasCapture_ = false;
};





class Practice5RectState final {
public:
    Practice5RectState();
    bool tryCreateRectAt(const Point& point);
    bool beginDrag(const Point& point);
    void dragTo(const Point& point);
    bool endDrag();

    const std::vector<AnimatedRect>& getItems() const;
    const std::optional<Rect>& getEraser() const;
    void reset();

private:
    void createInitialRects();
    void pushRandomRect();
    void pushRectAt(const Point& point);
    bool tryCreateEraserAt(const Point& point);
    bool resizeEraser(float halfSizeDelta);
    std::vector<std::size_t> findOverlaps() const;

    RectStore store_;
    std::optional<Rect> eraser_;
    std::size_t createdRectCount_ = 0;
};
