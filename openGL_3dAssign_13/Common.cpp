#include "Common.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace {

bool finiteVector(const glm::vec3& value) {
    return std::isfinite(value.x) && std::isfinite(value.y)
        && std::isfinite(value.z);
}

void appendFace(Polyhedron& polyhedron, GLFace shape,
                std::initializer_list<glm::vec3> vertices, GlColor color) {
    PlaneFigure face;
    face.shape = shape;
    face.localVertices = vertices;
    face.color = color;
    face.normal = glm::normalize(glm::cross(
        face.localVertices[1] - face.localVertices[0],
        face.localVertices[2] - face.localVertices[0]));
    polyhedron.faces.push_back(face);
}

} // namespace

Entity::Entity() {
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
}

std::optional<EntityId> Entity::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (components_.size() >= MAX_ENTITIES
        || !DiagramSystem::validPolyhedron(polyhedron)
        || !DiagramSystem::validTransform(transform)) {
        return std::nullopt;
    }

    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform });
    polyId_.push_back(id);
    std::vector<FaceId>& faceIds = polyhedron.shape == GLShape::Cube
        ? cubeFaceId_ : tetraFaceId_;
    for (std::size_t i = 0; i < polyhedron.faces.size(); ++i) {
        faceIds.push_back({ id, i });
    }
    ++geometryRevision_;
    return id;
}

const Component_Poly* Entity::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

bool Entity::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true; // 변환만 바뀌므로 로컬 정점 버퍼를 다시 올리지 않는다.
}

bool Entity::setFaceTransform(
    EntityId id, std::size_t faceIndex,
    const Transform3D& transform, bool independence
) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    face.transform = transform;
    face.independence = independence;
    return true;
}

void Entity::clear() {
    if (components_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    cubeFaceId_.clear();
    tetraFaceId_.clear();
    ++geometryRevision_;
}

bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale);
}

bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    const bool cube = polyhedron.shape == GLShape::Cube;
    if (!cube && polyhedron.shape != GLShape::Tetra) {
        return false;
    }
    if (polyhedron.faces.size() != (cube ? 6u : 4u)) {
        return false;
    }
    for (const PlaneFigure& face : polyhedron.faces) {
        if (face.shape != (cube ? GLFace::Rect : GLFace::Triangle)
            || face.localVertices.size() != (cube ? 4u : 3u)
            || !finiteVector(face.normal)
            || !std::isfinite(face.color.red)
            || !std::isfinite(face.color.green)
            || !std::isfinite(face.color.blue)
            || !validTransform(face.transform)) {
            return false;
        }
        for (const glm::vec3& vertex : face.localVertices) {
            if (!finiteVector(vertex)) {
                return false;
            }
        }
    }
    return true;
}

Polyhedron DiagramSystem::makePolyhedron(GLShape shape) {
    Polyhedron polyhedron;
    polyhedron.shape = shape;

    if (shape == GLShape::Cube) {
        polyhedron.faces.reserve(6);
        // 각 면의 네 정점은 바깥에서 볼 때 반시계 방향이다.
        appendFace(polyhedron, GLFace::Rect,
            { { -.5f, -.5f, .5f }, { .5f, -.5f, .5f },
              { .5f, .5f, .5f }, { -.5f, .5f, .5f } }, { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { .5f, -.5f, -.5f }, { -.5f, -.5f, -.5f },
              { -.5f, .5f, -.5f }, { .5f, .5f, -.5f } }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Rect,
            { { -.5f, -.5f, -.5f }, { -.5f, -.5f, .5f },
              { -.5f, .5f, .5f }, { -.5f, .5f, -.5f } }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { .5f, -.5f, .5f }, { .5f, -.5f, -.5f },
              { .5f, .5f, -.5f }, { .5f, .5f, .5f } }, { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { -.5f, .5f, .5f }, { .5f, .5f, .5f },
              { .5f, .5f, -.5f }, { -.5f, .5f, -.5f } }, { .3f, .8f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { -.5f, -.5f, -.5f }, { .5f, -.5f, -.5f },
              { .5f, -.5f, .5f }, { -.5f, -.5f, .5f } }, { .8f, .3f, .9f });
    } else if (shape == GLShape::Tetra) {
        const glm::vec3 top{ 0.0f, .65f, 0.0f };
        const glm::vec3 left{ -.6f, -.35f, .45f };
        const glm::vec3 right{ .6f, -.35f, .45f };
        const glm::vec3 back{ 0.0f, -.35f, -.6f };
        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle,
            { top, left, right }, { .9f, .4f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { top, right, back }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { top, back, left }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { left, back, right }, { .9f, .8f, .3f });
    }
    return polyhedron;
}

glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    // 열벡터 기준: 기준점 이동 -> S -> Rz -> Ry -> Rx -> 월드 위치.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.basis);
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, transform.scale);
    return glm::translate(model, -transform.localBasis);
}

glm::mat4 DiagramSystem::faceModelMatrix(
    const Component_Poly& component, const PlaneFigure& face
) {
    // 독립 면의 transform은 부모에 상대적인 값이 아닌 월드 변환이다.
    return modelMatrix(face.independence ? face.transform : component.transform);
}

namespace {

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

} // namespace

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
        text.erase(0, 3); // 저장 파일은 BOM을 유지하고 GLSL 입력에서만 제거한다.
    }
    return text;
}

bool ShaderHandler::initShader(const std::filesystem::path& vertexPath,
                               const std::filesystem::path& fragmentPath) {
    destroy();
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

    mvpLocation_ = glGetUniformLocation(shaderProgramID_, "uMVP");
    if (mvpLocation_ < 0) {
        std::cerr << "Shader uniform uMVP is missing.\n";
        destroy();
        return false;
    }
    return true;
}

bool ShaderHandler::initBuffer() {
    if (shaderProgramID_ == 0 || vao_ != 0) {
        return false;
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(2, vbo_);
    glGenBuffers(1, &ebo_);
    if (vao_ == 0 || vbo_[0] == 0 || vbo_[1] == 0 || ebo_ == 0) {
        destroy();
        return false;
    }

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBindVertexArray(0); // EBO 연결은 VAO의 상태로 유지한다.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool ShaderHandler::upload(const Entity& entities) {
    if (vao_ == 0) {
        return false;
    }

    std::vector<GLfloat> positions;
    std::vector<GLfloat> colors;
    std::vector<GLuint> indices;
    std::vector<DrawRange> ranges;

    for (EntityId id : entities.getIds()) {
        const Component_Poly* component = entities.getComponent(id);
        if (component == nullptr
            || !DiagramSystem::validPolyhedron(component->polyhedron)) {
            std::cerr << "Invalid polyhedron data.\n";
            return false;
        }

        const std::vector<PlaneFigure>& faces = component->polyhedron.faces;
        for (std::size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex) {
            const PlaneFigure& face = faces[faceIndex];
            const GLuint firstVertex = static_cast<GLuint>(positions.size() / 3);
            const std::size_t firstIndex = indices.size();
            for (const glm::vec3& vertex : face.localVertices) {
                positions.insert(positions.end(), { vertex.x, vertex.y, vertex.z });
                colors.insert(colors.end(),
                    { face.color.red, face.color.green, face.color.blue });
            }
            indices.insert(indices.end(),
                { firstVertex, firstVertex + 1, firstVertex + 2 });
            if (face.shape == GLFace::Rect) {
                // 반시계 방향 사각형: (0,1,2), (0,2,3).
                indices.insert(indices.end(),
                    { firstVertex, firstVertex + 2, firstVertex + 3 });
            }
            ranges.push_back({ id, faceIndex, firstIndex,
                static_cast<GLsizei>(indices.size() - firstIndex) });
        }
    }

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(positions.size() * sizeof(GLfloat)),
        positions.empty() ? nullptr : positions.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(colors.size() * sizeof(GLfloat)),
        colors.empty() ? nullptr : colors.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)),
        indices.empty() ? nullptr : indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    drawRanges_ = std::move(ranges);
    return true;
}

bool ShaderHandler::render(const Entity& entities, const glm::mat4& view,
                           const glm::mat4& projection) const {
    if (shaderProgramID_ == 0 || vao_ == 0 || mvpLocation_ < 0) {
        return false;
    }
    for (const DrawRange& range : drawRanges_) {
        const Component_Poly* component = entities.getComponent(range.entityId);
        if (component == nullptr
            || range.faceIndex >= component->polyhedron.faces.size()) {
            return false;
        }
    }

    glUseProgram(shaderProgramID_);
    glBindVertexArray(vao_);
    for (const DrawRange& range : drawRanges_) {
        const Component_Poly* component = entities.getComponent(range.entityId);
        const PlaneFigure& face = component->polyhedron.faces[range.faceIndex];
        const glm::mat4 model = DiagramSystem::faceModelMatrix(*component, face);
        const glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(mvp));

        const void* offset = reinterpret_cast<const void*>(
            range.firstIndex * sizeof(GLuint));
        glDrawElements(GL_TRIANGLES, range.count, GL_UNSIGNED_INT, offset);
    }
    glBindVertexArray(0);
    glUseProgram(0);
    return true;
}

void ShaderHandler::destroy() {
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    if (vbo_[0] != 0 || vbo_[1] != 0) {
        glDeleteBuffers(2, vbo_);
        vbo_[0] = vbo_[1] = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (shaderProgramID_ != 0) {
        glDeleteProgram(shaderProgramID_);
        shaderProgramID_ = 0;
    }
    mvpLocation_ = -1;
    drawRanges_.clear();
}
