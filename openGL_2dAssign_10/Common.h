#pragma once

#include <glew.h>

#include <cstddef>
#include <filesystem>

#include <optional>
#include <string>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>

#include <fstream>
#include <iostream>
#include <sstream>
#include <utility>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_DIAGRAMS = 10;

struct Point {
    float x = 0.0f;
    float y = 0.0f;
};

struct GlColor {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;

    void set(float r, float g, float b);
    void setRandom();
};


struct VertexBounds {
    float left = 1.0f;
    float right = -1.0f;
    float bottom = 1.0f;
    float top = -1.0f;

    void setUnfit();
    void setFit();
};

struct DiagCaseTable {
    size_t NumOfEqual = 0;
    size_t NumOfRight = 0;
    size_t NumOfRect = 0;
    size_t NumOfLine = 0;
    size_t NumOfSpot = 0;
};





enum class BoundsCase{ Fit, Unfit };

enum class GLShape { Spot, Line, Triangle, Rect, None };

enum class TriangleCase { Equilateral, RightAngled, None };


struct Shape {
    GLShape gs;
    TriangleCase tc;
};

DiagCaseTable makeDCT();



class Diagram {
public:
    explicit Diagram(GLShape shape, const std::vector<Point>& localVertices,
            GlColor color, Point pivot, Point localPivot, BoundsCase bounds);
    explicit Diagram(TriangleCase tc, const std::vector<Point>& localVertices,
        GlColor color, Point pivot, Point localPivot, BoundsCase bounds);



    GLShape getShape() const { return glshape_; }
    const GlColor& getColor() const { return color_; }
    std::size_t getVertexCount() const { return localVertices_.size(); }
    Point getPivot() const { return pivot_; }


    Point getScale() const { return scale_; }
    size_t getId() const { return id; }
    float getDistanceByPivot(const Point& pivot) const { return (std::abs(pivot.x * pivot.x + pivot.y * pivot.y - pivot_.x * pivot_.x + pivot_.y * pivot_.y)); }

    BoundsCase getBoundCase() const { return bounds; }
    std::optional<TriangleCase> getTriangleCase() const { return tricase; }

    bool canTranslate(Point direction) const;
    bool translate(Point direction);
    void setDirection(Point direction, float speed);
    bool setScale(Point scale);
    bool setRotation(float degrees);
    bool setPose(Point pivot, float rotationDegrees);
    bool advance(float deltaTime);

    std::vector<Point> worldVertices() const;
    std::vector<Point> worldVerticesAt(Point pivot, float rotationDegrees) const;
    bool fitsNdc() const;
    bool contains(Point point) const;

private:
    GLShape glshape_;

    std::optional<TriangleCase> tricase = std::nullopt;
    BoundsCase bounds;
 
    size_t id = 0;    //동일 도형을 더 빠르게 판별하기 위한 id다(한번 push 될 때 동일 id 배정, 그 후 id는 겹치지 않게 생성한다)
    // unfit과 fit에 동일한 도형이 무조건 2번 생긴다. 
    //그러나 id가 같지 않아도, 크기와 방향(scale, rotateDegrees, direction), Shape{glshape, tricase}이 같고 world pivot 간 거리가 일정 거리 이하면 Align된다 

    std::vector<Point> localVertices_;
    GlColor color_;
    Point pivot_;       // World position. Translation and direction move this point.
    Point localPivot_;  // Rotation and scale center in local coordinates.

    Point scale_{ 1.0f, 1.0f };
    float rotationDegrees_ = 0.0f;
    Point direction_{};
    float speed_ = 0.0f;
};

float randomFloat(float minimum, float maximum);
std::string readTextFile(const std::filesystem::path& path);







class ShaderHandler {
public:
    bool initShader(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    bool initBuffer();
    bool upload(const std::vector<Diagram>& diagrams,
                std::optional<std::size_t> selectedIndex);
    void render() const;
    void destroy();

private:
    struct DrawRange {
        GLenum mode = GL_TRIANGLES;
        GLint first = 0;
        GLsizei count = 0;
    };

    static constexpr std::size_t MAX_VERTICES = MAX_DIAGRAMS * 4;
    GLuint shaderProgramID_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_[2] = { 0, 0 }; // Position and color streams.
 
    
    GLuint ebo_ = 0;          // Shared outline and fill indices for all shapes.

    GLint useOverrideLocation_ = -1;
    GLint overrideColorLocation_ = -1;
    std::vector<DrawRange> drawRanges_;

    std::optional<std::size_t> selectedIndex_;
    GLsizei selectedOutlineCount_ = 0;
    GLenum selectedOutlineMode_ = GL_LINE_LOOP;
};
