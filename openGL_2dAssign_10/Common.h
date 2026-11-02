#pragma once

#include <glew.h>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_DIAGRAMS = 40;
constexpr float REGION_SPLIT_X = 0.2f;
constexpr std::size_t BOARD_COUNT = 5;
// 800x600 화면에서도 정사각형·정삼각형의 실제 비율을 유지한다.
constexpr float LOCAL_TO_NDC_X = static_cast<float>(WINDOW_HEIGHT) / WINDOW_WIDTH;

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

// 실제 정점의 min/max만 저장한다. Fit/Unfit 영역과 구분한다.
struct VertexBounds {
    float left = std::numeric_limits<float>::max();
    float right = std::numeric_limits<float>::lowest();
    float bottom = std::numeric_limits<float>::max();
    float top = std::numeric_limits<float>::lowest();
};

// 개수는 Fit/Unfit 한 쌍을 기준으로 한다.
struct DiagCaseTable {
    std::size_t NumOfEqual = 0;
    std::size_t NumOfRight = 0;
    std::size_t NumOfRect = 0;
    std::size_t NumOfLine = 0;
    std::size_t NumOfSpot = 0;
    int rectRows = 2;
    int rectColumns = 2;
};

enum class BoundsCase { Fit, Unfit };
enum class GLShape { Spot, Line, Triangle, Rect, None };
enum class TriangleCase { Equilateral, RightAngled, None };

DiagCaseTable makeDCT();

class Diagram {
public:
    explicit Diagram(GLShape shape, const std::vector<Point>& localVertices, GlColor color, Point pivot,
                     Point localPivot, BoundsCase bounds);
    explicit Diagram(TriangleCase tc, const std::vector<Point>& localVertices, GlColor color, Point pivot,
                     Point localPivot, BoundsCase bounds);

    GLShape getShape() const {
        return glshape_;
    }

    const GlColor& getColor() const {
        return color_;
    }

    std::size_t getVertexCount() const {
        return localVertices_.size();
    }

    Point getPivot() const {
        return pivot_;
    }

    Point getScale() const {
        return scale_;
    }

    float getRotation() const {
        return rotationDegrees_;
    }

    std::size_t getId() const {
        return id_;
    }

    std::size_t getBoardId() const {
        return boardId_;
    }

    BoundsCase getBoundCase() const {
        return bounds_;
    }

    std::optional<TriangleCase> getTriangleCase() const {
        return tricase_;
    }

    bool isMatched() const {
        return matched_;
    }

    bool isMovable() const {
        return bounds_ == BoundsCase::Unfit && !matched_;
    }

    bool isOutline() const {
        return bounds_ == BoundsCase::Fit && !matched_;
    }

    Diagram makeUnfitCopy() const;

    void setIdentity(std::size_t pairId, std::size_t boardId);
    void markMatched(GlColor color);

    float getDistanceByPivot(const Point& pivot) const;
    bool hasSameShape(const Diagram& other) const;

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
    std::optional<TriangleCase> tricase_ = std::nullopt;
    BoundsCase bounds_;
    std::size_t id_ = 0;
    std::size_t boardId_ = 0;
    bool matched_ = false;

    std::vector<Point> localVertices_;
    GlColor color_;
    Point pivot_;      // 월드 위치.
    Point localPivot_; // 로컬 회전·크기 중심.
    Point scale_{1.0f, 1.0f};
    float rotationDegrees_ = 0.0f;
    Point direction_{};
    float speed_ = 0.0f;
};

float randomFloat(float minimum, float maximum);
int randomInt(int minimum, int maximum);
std::string readTextFile(const std::filesystem::path& path);

class ShaderHandler {
public:
    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    bool initBuffer();
    bool upload(const std::vector<Diagram>& diagrams, std::optional<std::size_t> selectedIndex);

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
    GLuint vbo_[2] = {0, 0}; // 위치와 색상.
    GLuint ebo_ = 0;         // 모든 도형이 공유하는 외곽선·채움 인덱스.

    GLint useOverrideLocation_ = -1;
    GLint overrideColorLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    std::optional<std::size_t> selectedIndex_;
    GLsizei selectedOutlineCount_ = 0;
    GLenum selectedOutlineMode_ = GL_LINE_LOOP;
};
