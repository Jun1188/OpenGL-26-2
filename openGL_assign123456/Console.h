#pragma once

#include "RectState.h"

class Console {
public:
    virtual ~Console() = default;
    virtual void update(float) {}
    virtual void onKey(int key, int action) = 0;
    virtual void onMouseButton(int button, int action, const Point& point) = 0;
    virtual void onCursorMove(const Point& point) = 0;
    virtual void render() const = 0;
};

struct BaseColorState {
    GlColor backgroundColor{ 0.12f, 0.12f, 0.16f };
    GlColor section1Color;
    GlColor section2Color;
    GlColor section3Color;
    GlColor section4Color;

    void setRandomSections();
};

class Practice2Console final : public Console {
public:
    explicit Practice2Console(GLFWwindow* window);

    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, const Point& point) override;
    void onCursorMove(const Point& point) override;
    void render() const override;

private:
    enum class Command {
        None,
        AddSection1,
        AddSection2,
        AddSection3,
        AddSection4,
        ScaleUp,
        ScaleDown,
        Recolor,
        Reset,
        Quit
    };

    Command parseKey(int key, int action) const;
    void input(Command command);
    void renderBase() const;

    GLFWwindow* window_ = nullptr;
    Practice2RectState rectState_;
    BaseColorState baseColorState_;
};

class Practice3Console final : public Console {
public:
    explicit Practice3Console(GLFWwindow* window);

    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, const Point& point) override;
    void onCursorMove(const Point& point) override;
    void render() const override;

private:
    enum class Command {
        None,
        AddRandomRect,
        Quit
    };

    Command parseKey(int key, int action) const;
    void input(Command command);

    GLFWwindow* window_ = nullptr;
    Practice3RectState rectState_;
};




class Practice4Console final : public Console {
public:
    explicit Practice4Console(GLFWwindow* window);

    void update(float deltaTime) override;
    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, const Point& point) override;
    void onCursorMove(const Point& point) override;
    void render() const override;

private:
    enum class Command {
        None,
        Diagonal,
        HorizontalZigzag,
        Orbit,
        Resize,
        Recolor,
        Stop,
        Rollback,
        Reset,
        Quit,
        Print
    };

    Command parseKey(int key, int action) const;
    void input(Command command);

    GLFWwindow* window_ = nullptr;
    Practice4RectState rectState_;
};



class Practice5Console final : public Console {
public:
    explicit Practice5Console(GLFWwindow* window);

    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, const Point& point) override;
    void onCursorMove(const Point& point) override;
    void render() const override;

private:
    enum class Command {
        None,
        Reset,
        Quit
    };

    Command parseKey(int key, int action) const;
    void input(Command command);
    

    GLFWwindow* window_ = nullptr;
    Practice5RectState rectState_;
    
};

class Practice6Console final : public Console {
public:
    explicit Practice6Console(GLFWwindow* window);

    void update(float deltaTime) override;
    void onKey(int key, int action) override;
    void onMouseButton(int button, int action, const Point& point) override;
    void onCursorMove(const Point& point) override;
    void render() const override;

private:
    GLFWwindow* window_ = nullptr;
    Practice6RectState rectState_;
};
