#pragma once

#include <glew.h>

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_DIAGRAMS = 50;

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
};





enum class BoundsCase{ Fit, Unfit };

enum class GLShape { Spot, Line, Triangle, Rect };

enum class TriangleCase { Equilateral, RightAngled };





class Diagram {
public:
    Diagram(GLShape shape, const std::vector<Point>& localVertices,
            GlColor color, Point pivot, Point localPivot = {});



    GLShape getShape() const { return shape_; }
    const GlColor& getColor() const { return color_; }
    std::size_t getVertexCount() const { return localVertices_.size(); }
    Point getPivot() const { return pivot_; }
    const std::vector<Point>& getOldPivots() const { return oldPivots; };
    Point getScale() const { return scale_; }
    BoundsCase getBoundCase() const { return bounds; }
    std::optional<TriangleCase> getTriangleCase() const { return tricase; }

    bool canTranslate(Point direction) const;
    bool translate(Point direction);
    void setDirection(Point direction, float speed);
    bool setScale(Point scale);
    bool setRotation(float degrees);
    bool setPose(Point pivot, float rotationDegrees);
    bool advance(float deltaTime);
    void saveOldPivots();
    std::vector<Point> worldVertices() const;
    std::vector<Point> worldVerticesAt(Point pivot, float rotationDegrees) const;
    bool fitsNdc() const;
    bool contains(Point point) const;

private:
    GLShape shape_;

    std::optional<TriangleCase> tricase = std::nullopt;
    BoundsCase bounds;

    std::vector<Point> localVertices_;
    GlColor color_;
    Point pivot_;       // World position. Translation and direction move this point.
    Point localPivot_;  // Rotation and scale center in local coordinates.
    std::vector<Point> oldPivots;
    Point scale_{ 1.0f, 1.0f };
    float rotationDegrees_ = 0.0f;
    Point direction_{};
    float speed_ = 0.0f;
};

float randomFloat(float minimum, float maximum);
std::string readTextFile(const std::filesystem::path& path);
std::vector<Point> setRandomTriPointByCase(const TriangleCase case_);





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
    GLuint pathVao_ = 0;
    GLuint pathVbo_ = 0;
    GLsizeiptr pathBufferBytes_ = 0;
    GLuint ebo_ = 0;          // Shared outline and fill indices for all shapes.
    GLint usePathPositionLocation_ = -1;
    GLint useOverrideLocation_ = -1;
    GLint overrideColorLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    std::vector<DrawRange> pathRanges_;
    std::optional<std::size_t> selectedIndex_;
    GLsizei selectedOutlineCount_ = 0;
    GLenum selectedOutlineMode_ = GL_LINE_LOOP;
};
