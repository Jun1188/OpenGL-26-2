#include "Common.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
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
    case GLShape::Spot: return 4;     // A small square, not GL_POINTS
    case GLShape::Line: return 2;
    case GLShape::Triangle: return 3;
    case GLShape::Rect: return 4;
    }
    return 0;
}

float cross(Point a, Point b, Point c) {
    return (b.x - a.x) * (c.y - a.y)
         - (b.y - a.y) * (c.x - a.x);
}

bool insideTriangle(Point p, Point a, Point b, Point c) {
    if (std::abs(cross(a, b, c)) < 0.000001f) {
        return false;
    }
    const float ab = cross(a, b, p);
    const float bc = cross(b, c, p);
    const float ca = cross(c, a, p);
    return !(ab < 0.0f || bc < 0.0f || ca < 0.0f)
        || !(ab > 0.0f || bc > 0.0f || ca > 0.0f);
}

bool nearLine(Point p, Point a, Point b) {
    const float dx = b.x - a.x;
    const float dy = b.y - a.y;
    const float lengthSquared = dx * dx + dy * dy;
    if (lengthSquared == 0.0f) {
        return false;
    }
    const float t = std::clamp(
        ((p.x - a.x) * dx + (p.y - a.y) * dy) / lengthSquared,
        0.0f, 1.0f
    );
    const float distanceX = p.x - (a.x + t * dx);
    const float distanceY = p.y - (a.y + t * dy);
    return distanceX * distanceX + distanceY * distanceY <= 0.025f * 0.025f;
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
                 GlColor color, Point pivot, Point localPivot)
    : shape_(shape), localVertices_(localVertices), color_(color),
      pivot_(pivot), localPivot_(localPivot) {
}

bool Diagram::canTranslate(Point direction) const {
    Diagram candidate = *this;
    candidate.pivot_.x += direction.x;
    candidate.pivot_.y += direction.y;
    return candidate.fitsNdc();
}


bool Diagram::setEffectScale(Point scale) {
    if (!std::isfinite(scale.x) || !std::isfinite(scale.y)
        || scale.x <= 0.0f || scale.y <= 0.0f) {
        return false;
    }
    scale_ = scale;
    return true;
}

bool Diagram::translate(Point direction) {
    if (!canTranslate(direction)) {
        return false;
    }
    pivot_.x += direction.x;
    pivot_.y += direction.y;
    return true;
}

void Diagram::setDirection(Point direction, float speed) {
    direction_ = direction;
    speed_ = speed;
}

bool Diagram::setScale(Point scale) {
    Diagram candidate = *this;
    candidate.scale_ = scale;
    if (!candidate.fitsNdc()) {
        return false;
    }
    scale_ = scale;
    return true;
}

bool Diagram::setRotation(float degrees) {
    Diagram candidate = *this;
    candidate.rotationDegrees_ = degrees;
    if (!candidate.fitsNdc()) {
        return false;
    }
    rotationDegrees_ = degrees;
    return true;
}



bool Diagram::setPose(Point pivot, float rotationDegrees) {
    Diagram candidate = *this;
    candidate.pivot_ = pivot;
    candidate.rotationDegrees_ = rotationDegrees;
    if (!candidate.fitsNdc()) {
        return false;
    }
    pivot_ = pivot;
    rotationDegrees_ = rotationDegrees;
    return true;
}

bool Diagram::advance(float deltaTime) {
    if (deltaTime <= 0.0f || speed_ == 0.0f
        || (direction_.x == 0.0f && direction_.y == 0.0f)) {
        return false;
    }
    return translate({ direction_.x * speed_ * deltaTime,
                       direction_.y * speed_ * deltaTime });
}

std::vector<Point> Diagram::worldVertices() const {
    return worldVerticesAt(pivot_, rotationDegrees_);
}

std::vector<Point> Diagram::worldVerticesAt(//model space -> TRS -> world space
    Point pivot, float rotationDegrees
) const {
    // Local point -> pivot-relative Scale -> Rotate -> world Translation.
    glm::mat4 model(1.0f);
    model = glm::translate(model, glm::vec3(pivot.x, pivot.y, 0.0f));
    model = glm::rotate(model, glm::radians(rotationDegrees),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(scale_.x, scale_.y, 1.0f));
    model = glm::translate(model,
        glm::vec3(-localPivot_.x, -localPivot_.y, 0.0f));

    std::vector<Point> world;
    world.reserve(localVertices_.size());
    for (const Point& local : localVertices_) {
        const glm::vec4 p = model * glm::vec4(local.x, local.y, 0.0f, 1.0f);
        world.push_back({ p.x, p.y });
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

bool Diagram::contains(Point point) const {
    const std::vector<Point> p = worldVertices();
    if (p.size() != requiredVertices(shape_)) {
        return false;
    }
    if (shape_ == GLShape::Line) {
        return nearLine(point, p[0], p[1]);
    }
    if (shape_ == GLShape::Triangle) {
        return insideTriangle(point, p[0], p[1], p[2]);
    }
    // Quad fill is 0-1-2 and 2-1-3; its perimeter is 0-1-3-2.
    return insideTriangle(point, p[0], p[1], p[2])
        || insideTriangle(point, p[2], p[1], p[3]);
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
    overrideColorLocation_ = glGetUniformLocation(shaderProgramID_, "uOverrideColor");
    if (useOverrideLocation_ < 0 || overrideColorLocation_ < 0) {
        std::cerr << "Selection color uniforms are missing.\n";
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

bool ShaderHandler::upload(const std::vector<Diagram>& diagrams,
                           std::optional<std::size_t> selectedIndex) {
    if (vao_ == 0 || (selectedIndex && *selectedIndex >= diagrams.size())) return false;
    std::vector<GLfloat> positions;
    std::vector<GLfloat> colors;
    std::vector<DrawRange> ranges;
    GLint first = 0;
    for (const Diagram& diagram : diagrams) {
        if (diagram.getVertexCount() != requiredVertices(diagram.getShape())) return false;
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
    selectedIndex_ = selectedIndex;
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
    if (selectedIndex_) {
        const DrawRange& selected = drawRanges_[*selectedIndex_];
        glUniform1i(useOverrideLocation_, GL_TRUE);
        glUniform3f(overrideColorLocation_, 1.0f, 1.0f, 1.0f);
        const std::size_t firstIndex = selected.count == 4
            ? QUAD_OUTLINE_FIRST : TRIANGLE_OUTLINE_FIRST;
        glDrawElementsBaseVertex(GL_LINE_LOOP, selected.count, GL_UNSIGNED_INT,
                                 indexByteOffset(firstIndex), selected.first);
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
    useOverrideLocation_ = overrideColorLocation_ = -1;
    drawRanges_.clear();
    selectedIndex_.reset();
}
