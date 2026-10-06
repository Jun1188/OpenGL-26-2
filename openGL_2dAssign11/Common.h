#pragma once

#include <glew.h>

#include <cstddef>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_DIAGRAMS = 100;

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

enum class GLShape { Spot, Line, Triangle, Rect };


constexpr float boardTop = 1.0f;
constexpr float boardBottom = -1.0f;
constexpr float boardLeft = -1.0f;
constexpr float boardRight = 1.0f;


class Diagram {
public:
    Diagram(GLShape shape, const std::vector<Point>& localVertices,
            GlColor color, Point pivot, Point localPivot = {});

    GLShape getShape() const { return shape_; }
    const GlColor& getColor() const { return color_; }
    std::size_t getVertexCount() const { return localVertices_.size(); }
    Point getPivot() const { return pivot_; }
    Point getScale() const { return scale_; }

    bool canTranslate(Point direction) const;
    bool translate(Point direction);
    void setDirection(Point direction, float speed);
    bool setScale(Point scale);
    bool setRotation(float degrees);
    bool setPose(Point pivot, float rotationDegrees);
    bool advance(float deltaTime);
    bool isFilled() const { return filled_; }
    void setFilled(bool filled) { filled_ = filled; }
    void setColor(GlColor color) { color_ = color; }

    bool setEffectScale(Point scale);
    std::vector<Point> worldVertices() const;
    std::vector<Point> worldVerticesAt(Point pivot, float rotationDegrees) const;
    bool fitsNdc() const;
    bool contains(Point point) const;

private:
    GLShape shape_;
    std::vector<Point> localVertices_;
    GlColor color_;
    Point pivot_;       
    Point localPivot_;  
    bool filled_ = true;
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
    GLuint shaderProgramID_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_[2] = { 0, 0 };
    GLuint ebo_ = 0;
    GLint useOverrideLocation_ = -1;
    GLint overrideColorLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    std::optional<std::size_t> selectedIndex_;
};
