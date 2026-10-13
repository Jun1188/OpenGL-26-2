#include "../include/Graphics.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace graphics {
namespace {

bool finiteVector(const glm::vec3& value) {
    return std::isfinite(value.x) && std::isfinite(value.y)
        && std::isfinite(value.z);
}

bool finiteMatrix(const glm::mat4& matrix) {
    for (int column = 0; column < 4; ++column) {
        for (int row = 0; row < 4; ++row) {
            if (!std::isfinite(matrix[column][row])) {
                return false;
            }
        }
    }
    return true;
}

bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f
        && value.y > 0.0f && value.z > 0.0f;
}

bool validColor(GlColor color) {
    return std::isfinite(color.red) && std::isfinite(color.green)
        && std::isfinite(color.blue);
}

std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;
    return ++revision;
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

Scene::Scene() {
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
    pyramidFaceId_.reserve(MAX_ENTITIES * 5);
    lines_.reserve(MAX_LINES);
}

std::optional<EntityId> Scene::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (components_.size() >= MAX_ENTITIES
        || !DiagramSystem::validPolyhedron(polyhedron)
        || !DiagramSystem::validTransform(transform)) {
        return std::nullopt;
    }
    glm::vec3 minimum = polyhedron.faces[0].localVertices[0];
    glm::vec3 maximum = minimum;
    for (const PlaneFigure& face : polyhedron.faces) {
        for (const glm::vec3& vertex : face.localVertices) {
            minimum = glm::min(minimum, vertex);
            maximum = glm::max(maximum, vertex);
        }
    }
    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform, maximum - minimum });
    polyId_.push_back(id);
    std::vector<FaceId>& faceIds = polyhedron.shape == GLShape::Cube
        ? cubeFaceId_ : (polyhedron.shape == GLShape::Pyramid
            ? pyramidFaceId_ : tetraFaceId_);
    for (std::size_t i = 0; i < polyhedron.faces.size(); ++i) {
        faceIds.push_back({ id, i });
    }
    geometryRevision_ = nextGeometryRevision();
    return id;
}

std::optional<EntityId> Scene::addCuboid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths),
                       transform);
}

std::optional<EntityId> Scene::addRightTetrahedron(
    const glm::vec3& edgeLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths),
                       transform);
}

std::optional<EntityId> Scene::addPyramid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths),
                       transform);
}

bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;
    return true;
}

bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex,
                               const glm::mat4& matrix) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !finiteMatrix(matrix)) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    return true;
}

bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex,
                                const std::vector<GlColor>& colors) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    if (!colors.empty() && colors.size() != face.localVertices.size()) {
        return false;
    }
    for (GlColor color : colors) {
        if (!validColor(color)) {
            return false;
        }
    }
    face.vertexColors = colors;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return false;
    }
    components_[id].transform.basis = worldPosition;
    return true;
}

bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if (id >= components_.size() || !positiveLengths(sideLengths)) {
        return false;
    }
    Component_Poly& component = components_[id];
    Polyhedron replacement = DiagramSystem::makePolyhedron(
        component.polyhedron.shape, sideLengths);
    if (!DiagramSystem::validPolyhedron(replacement)) {
        return false;
    }
    for (std::size_t i = 0; i < replacement.faces.size(); ++i) {
        replacement.faces[i].color = component.polyhedron.faces[i].color;
        replacement.faces[i].independence = component.polyhedron.faces[i].independence;
        replacement.faces[i].transform = component.polyhedron.faces[i].transform;
        replacement.faces[i].visible = component.polyhedron.faces[i].visible;
        replacement.faces[i].localAnimationMatrix =
            component.polyhedron.faces[i].localAnimationMatrix;
        replacement.faces[i].vertexColors = component.polyhedron.faces[i].vertexColors;
    }
    component.polyhedron = std::move(replacement);
    component.sideLengths = sideLengths;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true;
}

bool Scene::setFaceTransform(
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

std::optional<LineId> Scene::addLine(
    const glm::vec3& start, const glm::vec3& end,
    GlColor color, std::optional<EntityId> owner
) {
    if (lines_.size() >= MAX_LINES || !finiteVector(start) || !finiteVector(end)
        || !validColor(color) || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    const LineId id = lines_.size();
    lines_.push_back({ start, end, color, owner });
    geometryRevision_ = nextGeometryRevision();
    return id;
}

std::optional<LineId> Scene::addVector(
    const glm::vec3& origin, const glm::vec3& vector,
    GlColor color, std::optional<EntityId> owner
) {
    return addLine(origin, origin + vector, color, owner);
}

std::optional<std::array<LineId, 3>> Scene::addBasis(
    const glm::vec3& origin, const glm::mat3& basisVectors,
    const glm::vec3& axisLengths, std::optional<EntityId> owner
) {
    if (lines_.size() > MAX_LINES - 3 || !finiteVector(origin)
        || !positiveLengths(axisLengths)
        || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    for (int i = 0; i < 3; ++i) {
        if (!finiteVector(basisVectors[i])
            || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f
            || !finiteVector(origin + basisVectors[i] * axisLengths[i])) {
            return std::nullopt;
        }
    }
    // 사전 검증 후 세 선을 함께 등록한다. 실패 입력은 장면을 변경하지 않는다.
    const std::array<GlColor, 3> colors{
        GlColor{ 1.0f, 0.2f, 0.2f },
        GlColor{ 0.2f, 1.0f, 0.2f },
        GlColor{ 0.2f, 0.4f, 1.0f }
    };
    std::array<LineId, 3> ids{};
    for (int i = 0; i < 3; ++i) {
        ids[i] = lines_.size();
        lines_.push_back({
            origin, origin + basisVectors[i] * axisLengths[i], colors[i], owner });
    }
    geometryRevision_ = nextGeometryRevision();
    return ids;
}

bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin)
        || !finiteVector(origin + vector)) {
        return false;
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    cubeFaceId_.clear();
    tetraFaceId_.clear();
    pyramidFaceId_.clear();
    lines_.clear();
    geometryRevision_ = nextGeometryRevision();
}

bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale)
        && std::isfinite(transform.orientation.w)
        && std::isfinite(transform.orientation.x)
        && std::isfinite(transform.orientation.y)
        && std::isfinite(transform.orientation.z)
        && std::isfinite(glm::dot(transform.orientation, transform.orientation))
        && glm::dot(transform.orientation, transform.orientation) > 0.000001f;
}

bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    std::size_t expectedFaces = 0;
    switch (polyhedron.shape) {
    case GLShape::Cube: expectedFaces = 6; break;
    case GLShape::Tetra: expectedFaces = 4; break;
    case GLShape::Pyramid: expectedFaces = 5; break;
    default: return false;
    }
    if (polyhedron.faces.size() != expectedFaces) {
        return false;
    }
    for (std::size_t i = 0; i < expectedFaces; ++i) {
        const PlaneFigure& face = polyhedron.faces[i];
        const bool rectangle = polyhedron.shape == GLShape::Cube
            || (polyhedron.shape == GLShape::Pyramid && i == 4);
        if (face.shape != (rectangle ? GLFace::Rect : GLFace::Triangle)
            || face.localVertices.size() != (rectangle ? 4u : 3u)
            || !finiteVector(face.normal) || !validColor(face.color)
            || !validTransform(face.transform) || !finiteMatrix(face.localAnimationMatrix)
            || (!face.vertexColors.empty()
                && face.vertexColors.size() != face.localVertices.size())) {
            return false;
        }
        for (const glm::vec3& vertex : face.localVertices) {
            if (!finiteVector(vertex)) {
                return false;
            }
        }
        for (GlColor color : face.vertexColors) {
            if (!validColor(color)) {
                return false;
            }
        }
    }
    return true;
}

Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
    Polyhedron polyhedron;
    if (!positiveLengths(sideLengths)) {
        return polyhedron;
    }
    polyhedron.shape = shape;
    if (shape == GLShape::Cube) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        polyhedron.faces.reserve(6);
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, z }, { x, -y, z }, { x, y, z }, { -x, y, z } },
            { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, -z }, { -x, -y, -z }, { -x, y, -z }, { x, y, -z } },
            { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { -x, -y, z }, { -x, y, z }, { -x, y, -z } },
            { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, z }, { x, -y, -z }, { x, y, -z }, { x, y, z } },
            { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, y, z }, { x, y, z }, { x, y, -z }, { -x, y, -z } },
            { .3f, .8f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { x, -y, -z }, { x, -y, z }, { -x, -y, z } },
            { .8f, .3f, .9f });
    } else if (shape == GLShape::Pyramid) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        const glm::vec3 frontLeft{ -x, -y, z };
        const glm::vec3 frontRight{ x, -y, z };
        const glm::vec3 backRight{ x, -y, -z };
        const glm::vec3 backLeft{ -x, -y, -z };
        const glm::vec3 apex{ 0.0f, y, 0.0f };
        polyhedron.faces.reserve(5);
        // 앞, 오른쪽, 뒤, 왼쪽, 바닥. 삼각형과 사각형은 바깥쪽 winding이다.
        appendFace(polyhedron, GLFace::Triangle,
            { frontLeft, frontRight, apex }, { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { frontRight, backRight, apex }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { backRight, backLeft, apex }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { backLeft, frontLeft, apex }, { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { backLeft, backRight, frontRight, frontLeft }, { .8f, .3f, .9f });
    } else if (shape == GLShape::Tetra) {
        // 원점에서 직교하는 세 모서리를 만들고, 무게중심을 로컬 원점으로 옮긴다.
        const glm::vec3 center = sideLengths * 0.25f;
        const glm::vec3 origin = -center;
        const glm::vec3 x = glm::vec3(sideLengths.x, 0.0f, 0.0f) - center;
        const glm::vec3 y = glm::vec3(0.0f, sideLengths.y, 0.0f) - center;
        const glm::vec3 z = glm::vec3(0.0f, 0.0f, sideLengths.z) - center;
        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle,
            { origin, y, x }, { .9f, .4f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, x, z }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, z, y }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { x, y, z }, { .9f, .8f, .3f });
    }
    return polyhedron;
}

glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    // 열벡터 기준: localBasis 이동 -> S -> Rz -> Ry -> Rx -> 월드 위치.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.basis);
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model *= glm::mat4_cast(glm::normalize(transform.orientation));
    model = glm::scale(model, transform.scale);
    return glm::translate(model, -transform.localBasis);
}

glm::mat4 DiagramSystem::faceModelMatrix(
    const Component_Poly& component, const PlaneFigure& face
) {
    // 기존 독립 면은 월드 transform만 사용한다. 일반 면은 객체를 따라간다.
    return face.independence ? modelMatrix(face.transform)
        : modelMatrix(component.transform) * face.localAnimationMatrix;
}

bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);
    return finiteVector(camera.position) && finiteVector(camera.target)
        && finiteVector(camera.up) && finiteVector(direction) && finiteVector(right)
        && glm::dot(direction, direction) > 0.000001f
        && glm::dot(right, right) > 0.000001f
        && std::isfinite(camera.fieldOfViewDegrees)
        && camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f
        && std::isfinite(camera.nearPlane) && std::isfinite(camera.farPlane)
        && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane
        && std::isfinite(camera.orthographicHalfExtent)
        && camera.orthographicHalfExtent > 0.0f;
}

glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                          camera.nearPlane, camera.farPlane);
    }
    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect,
                            camera.nearPlane, camera.farPlane);
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

bool Renderer::initShader(const std::filesystem::path& vertexPath,
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

bool Renderer::initBuffer() {
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

bool Renderer::upload(const Scene& scene) {
    if (vao_ == 0) {
        return false;
    }
    std::vector<GLfloat> positions;
    std::vector<GLfloat> colors;
    std::vector<GLuint> indices;
    std::vector<DrawRange> ranges;
    for (EntityId id : scene.getIds()) {
        const Component_Poly* component = scene.getComponent(id);
        if (component == nullptr
            || !DiagramSystem::validPolyhedron(component->polyhedron)) {
            return false;
        }
        const auto& faces = component->polyhedron.faces;
        for (std::size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex) {
            const PlaneFigure& face = faces[faceIndex];
            const GLuint firstVertex = static_cast<GLuint>(positions.size() / 3);
            const std::size_t firstIndex = indices.size();
            for (std::size_t i = 0; i < face.localVertices.size(); ++i) {
                const glm::vec3& vertex = face.localVertices[i];
                const GlColor color = face.vertexColors.empty() ? face.color : face.vertexColors[i];
                positions.insert(positions.end(), { vertex.x, vertex.y, vertex.z });
                colors.insert(colors.end(), { color.red, color.green, color.blue });
            }
            indices.insert(indices.end(),
                { firstVertex, firstVertex + 1, firstVertex + 2 });
            if (face.shape == GLFace::Rect) {
                indices.insert(indices.end(),
                    { firstVertex, firstVertex + 2, firstVertex + 3 });
            }
            const GLsizei triangleCount = static_cast<GLsizei>(indices.size() - firstIndex);
            const std::size_t outlineFirstIndex = indices.size();
            for (std::size_t i = 0; i < face.localVertices.size(); ++i) {
                indices.push_back(firstVertex + static_cast<GLuint>(i));
            }
            ranges.push_back({ GL_TRIANGLES, id, faceIndex, std::nullopt,
                firstIndex, triangleCount, outlineFirstIndex,
                static_cast<GLsizei>(face.localVertices.size()) });
        }
    }
    const auto& lines = scene.getLines();
    for (LineId id = 0; id < lines.size(); ++id) {
        const Line3D& line = lines[id];
        const GLuint firstVertex = static_cast<GLuint>(positions.size() / 3);
        const std::size_t firstIndex = indices.size();
        positions.insert(positions.end(),
            { line.start.x, line.start.y, line.start.z,
              line.end.x, line.end.y, line.end.z });
        for (int i = 0; i < 2; ++i) {
            colors.insert(colors.end(),
                { line.color.red, line.color.green, line.color.blue });
        }
        indices.insert(indices.end(), { firstVertex, firstVertex + 1 });
        ranges.push_back({ GL_LINES, 0, 0, id, firstIndex, 2 });
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

bool Renderer::draw(const Scene& scene, const glm::mat4& view,
                    const glm::mat4& projection, const RenderOptions& options) const {
    if (shaderProgramID_ == 0 || vao_ == 0 || mvpLocation_ < 0) {
        return false;
    }
    // 사용되는 ID를 먼저 검사한다. 잘못된 데이터로 일부만 그리지 않는다.
    for (const DrawRange& range : drawRanges_) {
        if (range.lineId) {
            if (*range.lineId >= scene.getLines().size()) {
                return false;
            }
            const auto owner = scene.getLines()[*range.lineId].owner;
            if (owner && scene.getComponent(*owner) == nullptr) {
                return false;
            }
        } else {
            const Component_Poly* component = scene.getComponent(range.entityId);
            if (component == nullptr
                || range.faceIndex >= component->polyhedron.faces.size()) {
                return false;
            }
        }
    }
    glUseProgram(shaderProgramID_);
    glBindVertexArray(vao_);
    for (const DrawRange& range : drawRanges_) {
        glm::mat4 model(1.0f);
        if (range.lineId) {
            const auto owner = scene.getLines()[*range.lineId].owner;
            if (owner) {
                model = DiagramSystem::modelMatrix(scene.getComponent(*owner)->transform);
            }
        } else {
            const Component_Poly* component = scene.getComponent(range.entityId);
            const PlaneFigure& face = component->polyhedron.faces[range.faceIndex];
            if (!face.visible) {
                continue;
            }
            model = DiagramSystem::faceModelMatrix(*component, face);
        }
        const glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(mvp));
        const bool outline = options.wireframe && !range.lineId.has_value();
        const std::size_t firstIndex = outline ? range.outlineFirstIndex : range.firstIndex;
        const void* offset = reinterpret_cast<const void*>(firstIndex * sizeof(GLuint));
        glDrawElements(outline ? GL_LINE_LOOP : range.mode,
                       outline ? range.outlineCount : range.count, GL_UNSIGNED_INT, offset);
    }
    glBindVertexArray(0);
    glUseProgram(0);
    return true;
}

bool Renderer::initialize(const std::filesystem::path& vertexPath,
                          const std::filesystem::path& fragmentPath) {
    if (!initShader(vertexPath, fragmentPath) || !initBuffer()) {
        destroy();
        return false;
    }
    return true;
}

bool Renderer::render(const Scene& scene, const Camera& camera, int width, int height,
                      const RenderOptions& options) {
    if (shaderProgramID_ == 0 || vao_ == 0) {
        return false;
    }
    if (width <= 0 || height <= 0) {
        return true; // 최소화 중에는 투영과 드로우를 건너뛴다.
    }
    if (!CameraSystem::validCamera(camera)) {
        return false;
    }
    if (uploadedScene_ != &scene || uploadedRevision_ != scene.getGeometryRevision()) {
        if (!upload(scene)) {
            return false;
        }
        uploadedScene_ = &scene;
        uploadedRevision_ = scene.getGeometryRevision();
    }
    glViewport(0, 0, width, height);
    if (options.depthTest) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glClearColor(0.12f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    return draw(scene, CameraSystem::viewMatrix(camera),
                CameraSystem::projectionMatrix(camera, aspect), options);
}

void Renderer::destroy() {
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
    uploadedScene_ = nullptr;
    uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
}

} // namespace graphics
