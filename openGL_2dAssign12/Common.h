#pragma once

#include <glew.h>

#include <cstddef>
#include <filesystem>
#include <string>
#include <vector>

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;

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

class Diagram {
public:
    Diagram(GLShape shape, const std::vector<Point>& localVertices, GlColor color, Point pivot, bool filled = true);

    GLShape getShape() const {
        return shape_;
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

    bool isFilled() const {
        return filled_;
    }

    void setColor(GlColor color) {
        color_ = color;
    }

    bool setPivot(Point pivot);
    std::vector<Point> worldVertices() const;
    bool fitsNdc() const;

private:
    GLShape shape_;
    std::vector<Point> localVertices_;
    GlColor color_;
    Point pivot_;
    bool filled_ = true;
};

float randomFloat(float minimum, float maximum);
std::string readTextFile(const std::filesystem::path& path);

class ShaderHandler {
public:
    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    bool initBuffer();
    bool upload(const std::vector<Diagram>& diagrams);

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
    GLuint vbo_[2] = {0, 0};
    GLuint ebo_ = 0;
    GLint useOverrideLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
};
