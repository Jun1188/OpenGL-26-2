#include "Common.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <utility>

namespace {

constexpr std::size_t QUAD_OUTLINE_FIRST = 0;
constexpr std::size_t QUAD_FILL_FIRST = 4;
constexpr std::size_t TRIANGLE_OUTLINE_FIRST = 10;
constexpr std::size_t TRIANGLE_FILL_FIRST = 13;
constexpr std::size_t LINE_FIRST = 16;

const void* indexByteOffset(std::size_t firstIndex) {
    return reinterpret_cast<const void*>(firstIndex * sizeof(GLuint));
}

std::random_device randomDevice;
std::default_random_engine randomEngine(randomDevice());

GLuint compileShader(GLenum type, const std::filesystem::path& path) {
    const std::string sourceText = readTextFile(path);
    if (sourceText.empty()) {
        return 0;
    }
    const char* source = sourceText.c_str();
    const GLuint shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), nullptr, log.data());
        std::cerr << "Shader compile failed: " << path << '\n' << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

std::size_t requiredVertices(GLShape shape) {
    switch (shape) {
    case GLShape::Spot: return 4;     // A small square, not GL_POINTS.
    case GLShape::Line: return 2;
    case GLShape::Triangle: return 3;
    case GLShape::Rect: return 4;
    }
    return 0;
}

} // namespace

float randomFloat(float minimum, float maximum) {
    std::uniform_real_distribution<float> distribution(minimum, maximum);
    return distribution(randomEngine);
}

void GlColor::set(float r, float g, float b) {
    red = r;
    green = g;
    blue = b;
}

void GlColor::setRandom() {
    set(randomFloat(0.15f, 0.95f),
        randomFloat(0.15f, 0.95f),
        randomFloat(0.15f, 0.95f));
}

Diagram::Diagram(GLShape shape, const std::vector<Point>& localVertices,
                 GlColor color, Point pivot, bool filled)
    : shape_(shape), localVertices_(localVertices), color_(color),
      pivot_(pivot), filled_(filled) {
}

bool Diagram::setPivot(Point pivot) {
    Diagram candidate = *this;
    candidate.pivot_ = pivot;
    if (!candidate.fitsNdc()) {
        return false;
    }
    pivot_ = pivot;
    return true;
}

std::vector<Point> Diagram::worldVertices() const {
    // Practice 12 uses translation only: local vertex + world pivot.
    std::vector<Point> world;
    world.reserve(localVertices_.size());
    for (const Point& local : localVertices_) {
        world.push_back({ local.x + pivot_.x, local.y + pivot_.y });
    }
    return world;
}

bool Diagram::fitsNdc() const {
    const std::vector<Point> world = worldVertices();
    if (world.size() != requiredVertices(shape_)) {
        return false;
    }
    for (const Point& point : world) {
        if (!std::isfinite(point.x) || !std::isfinite(point.y)
            || point.x < -1.0f || point.x > 1.0f
            || point.y < -1.0f || point.y > 1.0f) {
            return false;
        }
    }
    return true;
}

std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open shader file: " << path << '\n';
        return {};
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();
    if (text.size() >= 3 && static_cast<unsigned char>(text[0]) == 0xEF
        && static_cast<unsigned char>(text[1]) == 0xBB
        && static_cast<unsigned char>(text[2]) == 0xBF) {
        text.erase(0, 3); // GLSL #version must be the first source directive.
    }
    return text;
}

bool ShaderHandler::initShader(const std::filesystem::path& vertexPath,
                               const std::filesystem::path& fragmentPath) {
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
    if (vertexShader == 0) {
        return false;
    }
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    shaderProgramID_ = glCreateProgram();
    if (shaderProgramID_ == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    glAttachShader(shaderProgramID_, vertexShader);
    glAttachShader(shaderProgramID_, fragmentShader);
    glLinkProgram(shaderProgramID_);

    GLint linked = GL_FALSE;
    glGetProgramiv(shaderProgramID_, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(shaderProgramID_, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetProgramInfoLog(shaderProgramID_, static_cast<GLsizei>(log.size()),
                            nullptr, log.data());
        std::cerr << "Shader link failed:\n" << log << '\n';
    }
    glDetachShader(shaderProgramID_, vertexShader);
    glDetachShader(shaderProgramID_, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (linked != GL_TRUE) {
        destroy();
        return false;
    }

    useOverrideLocation_ = glGetUniformLocation(shaderProgramID_, "uUseOverrideColor");
    if (useOverrideLocation_ < 0) {
        std::cerr << "Color mode uniform is missing.\n";
        destroy();
        return false;
    }
    return true;
}

bool ShaderHandler::initBuffer() {
    if (shaderProgramID_ == 0) return false;
    const GLint position = glGetAttribLocation(shaderProgramID_, "vPos");
    const GLint color = glGetAttribLocation(shaderProgramID_, "vColor");
    if (position < 0 || color < 0) return false;

    glGenVertexArrays(1, &vao_);
    glBindVertexArray(vao_);
    glGenBuffers(2, vbo_);
    const GLuint attributes[] = {
        static_cast<GLuint>(position), static_cast<GLuint>(color)
    };
    for (int i = 0; i < 2; ++i) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_[i]);
        glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(attributes[i], 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(attributes[i]);
    }
    const GLuint indices[] = {
        0, 1, 3, 2,       // Rectangle perimeter.
        0, 1, 2, 2, 1, 3, // Rectangle fill.
        0, 1, 2,          // Triangle perimeter.
        0, 1, 2,          // Triangle fill.
        0, 1              // Line.
    };
    glGenBuffers(1, &ebo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vao_ != 0 && vbo_[0] != 0 && vbo_[1] != 0 && ebo_ != 0;
}

bool ShaderHandler::upload(const std::vector<Diagram>& diagrams) {
    if (vao_ == 0) return false;
    std::vector<GLfloat> positions;
    std::vector<GLfloat> colors;
    std::vector<DrawRange> ranges;
    GLint first = 0;
    for (const Diagram& diagram : diagrams) {
        if (!diagram.fitsNdc()) return false;
        const std::vector<Point> world = diagram.worldVertices();
        const GlColor& color = diagram.getColor();
        for (Point point : world) {
            positions.insert(positions.end(), { point.x, point.y, 0.0f });
            colors.insert(colors.end(), { color.red, color.green, color.blue });
        }
        const GLenum mode = diagram.getShape() == GLShape::Line
            ? GL_LINES : (diagram.isFilled() ? GL_TRIANGLES : GL_LINE_LOOP);
        ranges.push_back({ mode, first, static_cast<GLsizei>(world.size()) });
        first += static_cast<GLint>(world.size());
    }
    const GLfloat zero[3] = {};
    const GLsizeiptr bytes = positions.empty() ? sizeof(zero)
        : static_cast<GLsizeiptr>(positions.size() * sizeof(GLfloat));
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glBufferData(GL_ARRAY_BUFFER, bytes, positions.empty() ? zero : positions.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glBufferData(GL_ARRAY_BUFFER, bytes, colors.empty() ? zero : colors.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    drawRanges_ = std::move(ranges);
    return true;
}

void ShaderHandler::render() const {
    glUseProgram(shaderProgramID_);
    glBindVertexArray(vao_);
    glUniform1i(useOverrideLocation_, GL_FALSE);
    glLineWidth(1.0f);
    for (const DrawRange& range : drawRanges_) {
        std::size_t firstIndex = LINE_FIRST;
        GLsizei count = range.count;
        if (range.mode == GL_TRIANGLES) {
            firstIndex = range.count == 4 ? QUAD_FILL_FIRST : TRIANGLE_FILL_FIRST;
            if (range.count == 4) count = 6;
        } else if (range.mode == GL_LINE_LOOP) {
            firstIndex = range.count == 4 ? QUAD_OUTLINE_FIRST : TRIANGLE_OUTLINE_FIRST;
        }
        glDrawElementsBaseVertex(range.mode, count, GL_UNSIGNED_INT,
                                 indexByteOffset(firstIndex), range.first);
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

void ShaderHandler::destroy() {
    if (ebo_ != 0) glDeleteBuffers(1, &ebo_);
    if (vbo_[0] != 0 || vbo_[1] != 0) glDeleteBuffers(2, vbo_);
    if (vao_ != 0) glDeleteVertexArrays(1, &vao_);
    if (shaderProgramID_ != 0) glDeleteProgram(shaderProgramID_);
    ebo_ = vao_ = shaderProgramID_ = 0;
    vbo_[0] = vbo_[1] = 0;
    useOverrideLocation_ = -1;
    drawRanges_.clear();
}
