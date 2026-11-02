#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"

#include <cstddef>
#include <algorithm>
#include <cmath>
#include <limits>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace {

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
constexpr char kVertexShaderPath[] = "shaders/basic.vert";
constexpr char kFragmentShaderPath[] = "shaders/basic.frag";
constexpr char kMeshPath[] = "data/cube.txt";

struct RenderVertex {
    glm::vec3 position;
    glm::vec3 normal;
};

struct Bounds {
    glm::vec3 minimum{std::numeric_limits<float>::max()};
    glm::vec3 maximum{std::numeric_limits<float>::lowest()};
};

struct Camera {
    glm::vec3 position{0.0f, 0.0f, 2.5f};
    glm::vec3 forward{0.0f, 0.0f, -1.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float speed = 2.0f;
    float radius = 0.15f;
};

/// 모델 경계를 월드 경계로 변환한다.
Bounds transformBounds(const Bounds& local, const glm::mat4& model) {
    Bounds world;

    for (int corner = 0; corner < 8; ++corner) {
        const glm::vec3 point(
            corner & 1 ? local.maximum.x : local.minimum.x,
            corner & 2 ? local.maximum.y : local.minimum.y,
            corner & 4 ? local.maximum.z : local.minimum.z);
        const glm::vec3 transformed = glm::vec3(model * glm::vec4(point, 1.0f));
        world.minimum = glm::min(world.minimum, transformed);
        world.maximum = glm::max(world.maximum, transformed);
    }
    return world;
}

/// 카메라와 물체 경계의 충돌을 검사한다.
bool intersectsObject(const glm::vec3& position, float radius, const Bounds& bounds) {
    const glm::vec3 closest = glm::clamp(position, bounds.minimum, bounds.maximum);
    const glm::vec3 difference = position - closest;
    return glm::dot(difference, difference) <= radius * radius;
}

/// 입력과 충돌에 따라 카메라를 이동한다.
void updateCamera(GLFWwindow* window, Camera& camera, const Bounds& bounds, float deltaTime) {
    if (glfwGetWindowAttrib(window, GLFW_FOCUSED) != GLFW_TRUE) {
        return;
    }
    const glm::vec3 right = glm::normalize(glm::cross(camera.forward, camera.up));
    glm::vec3 direction(0.0f);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) direction += camera.forward;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) direction -= camera.forward;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) direction -= right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) direction += right;
    if (glm::dot(direction, direction) == 0.0f) {
        return;
    }
    const glm::vec3 movement = glm::normalize(direction) * camera.speed * deltaTime;

    const int steps = std::max(1, static_cast<int>(std::ceil(
        glm::length(movement) / (camera.radius * 0.5f))));
    const glm::vec3 step = movement / static_cast<float>(steps);
    for (int i = 0; i < steps; ++i) {
        const glm::vec3 candidate = camera.position + step;
        if (intersectsObject(candidate, camera.radius, bounds)) {
            break;
        }
        camera.position = candidate;
    }
}

/// GLFW 오류를 출력한다.
void glfwErrorCallback(int errorCode, const char* description) {
    std::cerr << "[GLFW 오류 " << errorCode << "] " << description << '\n';
}

/// 프레임버퍼 크기로 뷰포트를 갱신한다.
void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

/// 텍스트 파일에서 셰이더를 읽는다.
bool readTextFile(const char* path, std::string& contents) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "[파일 오류] 셰이더 파일을 열 수 없습니다: " << path << '\n';
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    contents = buffer.str();

    if (contents.size() >= 3 &&
        static_cast<unsigned char>(contents[0]) == 0xEF &&
        static_cast<unsigned char>(contents[1]) == 0xBB &&
        static_cast<unsigned char>(contents[2]) == 0xBF) {
        contents.erase(0, 3);
    }
    return true;
}

/// 셰이더 소스를 컴파일한다.
GLuint compileShader(GLenum shaderType, const std::string& source, const char* path) {
    const GLuint shader = glCreateShader(shaderType);
    const char* sourcePointer = source.c_str();
    glShaderSource(shader, 1, &sourcePointer, nullptr);
    glCompileShader(shader);

    GLint compileSucceeded = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSucceeded);
    if (compileSucceeded == GL_TRUE) {
        return shader;
    }

    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<std::size_t>(logLength > 0 ? logLength : 1), '\0');
    glGetShaderInfoLog(shader, logLength, nullptr, log.data());

    std::cerr << "[셰이더 컴파일 오류] " << path << '\n' << log << '\n';
    glDeleteShader(shader);
    return 0;
}

/// 셰이더를 연결해 프로그램을 만든다.
GLuint createShaderProgram(const char* vertexPath, const char* fragmentPath) {
    std::string vertexSource;
    std::string fragmentSource;
    if (!readTextFile(vertexPath, vertexSource) ||
        !readTextFile(fragmentPath, fragmentSource)) {
        return 0;
    }

    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexSource, vertexPath);
    if (vertexShader == 0) {
        return 0;
    }

    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource, fragmentPath);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return 0;
    }

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    GLint linkSucceeded = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSucceeded);
    if (linkSucceeded == GL_TRUE) {
        return program;
    }

    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::string log(static_cast<std::size_t>(logLength > 0 ? logLength : 1), '\0');
    glGetProgramInfoLog(program, logLength, nullptr, log.data());

    std::cerr << "[셰이더 링크 오류]\n" << log << '\n';
    glDeleteProgram(program);
    return 0;
}

/// 메시를 읽어 렌더링 정점과 경계를 만든다.
bool loadMeshVertices(const char* path, std::vector<RenderVertex>& vertices, Bounds& bounds) {
    std::ifstream input(path);
    if (!input) {
        std::cerr << "[파일 오류] 메시 파일을 열 수 없습니다: " << path << '\n';
        return false;
    }

    const warmup::ParseResult parsed = warmup::MeshParser().parse(input);
    const warmup::ValidationResult validated = warmup::MeshValidator().validate(parsed.mesh);

    for (const warmup::Diagnostic& diagnostic : parsed.diagnostics) {
        std::cerr << "[메시 파싱 오류] 줄 " << diagnostic.line
                  << ": " << diagnostic.message << '\n';
    }
    for (const warmup::Diagnostic& diagnostic : validated.diagnostics) {
        std::cerr << "[메시 검증 오류] 줄 " << diagnostic.line;
        if (diagnostic.face != 0) {
            std::cerr << ", 면 " << diagnostic.face;
        }
        std::cerr << ": " << diagnostic.message << '\n';
    }

    if (!warmup::succeeded(parsed, validated)) {
        return false;
    }

    constexpr std::size_t verticesPerFace = 3;
    vertices.clear();
    vertices.reserve(validated.faces.size() * verticesPerFace);
    bounds = Bounds{};

    for (const warmup::ResolvedFace& face : validated.faces) {
        glm::dvec3 positions[3];
        for (int corner = 0; corner < 3; ++corner) {
            const warmup::Vertex& p = face.vertices[corner];
            positions[corner] = glm::dvec3(p.x, p.y, p.z);
        }

        glm::dvec3 edgeA = positions[1] - positions[0];
        glm::dvec3 edgeB = positions[2] - positions[0];

        const glm::dvec3 absoluteA = glm::abs(edgeA);
        const glm::dvec3 absoluteB = glm::abs(edgeB);
        edgeA /= std::max({absoluteA.x, absoluteA.y, absoluteA.z});
        edgeB /= std::max({absoluteB.x, absoluteB.y, absoluteB.z});
        const glm::dvec3 faceNormal = glm::normalize(glm::cross(edgeA, edgeB));
        for (int corner = 0; corner < warmup::mesh_rules::triangleCorners; ++corner) {
            const glm::vec3 position(positions[corner]);
            vertices.push_back({position, glm::vec3(faceNormal)});
            bounds.minimum = glm::min(bounds.minimum, position);
            bounds.maximum = glm::max(bounds.maximum, position);
        }
    }

    if (vertices.empty()) {
        std::cerr << "[메시 오류] 렌더링할 삼각형이 없습니다.\n";
        return false;
    }

    std::cout << "메시 로드 완료: " << validated.faces.size()
              << "개 삼각형, " << vertices.size()
              << "개 렌더링 정점\n";
    return true;
}

}

/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    glfwSetErrorCallback(glfwErrorCallback);
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "[초기화 오류] GLFW 초기화에 실패했습니다.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        kWindowWidth,
        kWindowHeight,
        "OpenGL Model / View / Projection - Grayscale Lighting",
        nullptr,
        nullptr);
    if (window == nullptr) {
        std::cerr << "[초기화 오류] GLFW 창을 생성하지 못했습니다.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    const GLenum glewError = glewInit();
    if (glewError != GLEW_OK) {
        std::cerr << "[초기화 오류] GLEW 초기화 실패: "
                  << glewGetErrorString(glewError) << '\n';
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glGetError();

    int framebufferWidth = 0;
    int framebufferHeight = 0;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    const GLuint shaderProgram = createShaderProgram(kVertexShaderPath, kFragmentShaderPath);
    if (shaderProgram == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    std::vector<RenderVertex> vertices;
    Bounds localBounds;
    if (!loadMeshVertices(kMeshPath, vertices, localBounds)) {
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    GLuint vertexArray = 0;
    GLuint vertexBuffer = 0;
    glGenVertexArrays(1, &vertexArray);
    glGenBuffers(1, &vertexBuffer);

    glBindVertexArray(vertexArray);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(
        GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(vertices.size() * sizeof(RenderVertex)),
        vertices.data(),
        GL_STATIC_DRAW);

    constexpr GLsizei stride = sizeof(RenderVertex);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride,
        reinterpret_cast<const void*>(offsetof(RenderVertex, position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        stride,
        reinterpret_cast<const void*>(offsetof(RenderVertex, normal)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const GLint modelLocation = glGetUniformLocation(shaderProgram, "uModel");
    const GLint viewLocation = glGetUniformLocation(shaderProgram, "uView");
    const GLint projectionLocation = glGetUniformLocation(shaderProgram, "uProjection");
    const GLint lightLocation = glGetUniformLocation(shaderProgram, "uLightDirection");
    if (modelLocation == -1 || viewLocation == -1 ||
        projectionLocation == -1 || lightLocation == -1) {
        std::cerr << "[셰이더 오류] 변환 또는 조명 uniform을 찾을 수 없습니다.\n";
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n'
              << "W/S: 전진/후진, A/D: 좌우 이동 (시선 방향 고정)\n"
              << "카메라 구와 오브젝트 AABB 충돌 시 이동 또는 회전을 멈춥니다.\n"
              << "Esc 키를 누르거나 창을 닫으면 종료됩니다.\n";

    Camera camera;
    double previousTime = glfwGetTime();
    float modelAngle = 0.0f;
    glm::mat4 model(1.0f);
    Bounds worldBounds = transformBounds(localBounds, model);
    const glm::vec3 lightDirection = glm::normalize(glm::vec3(-0.5f, -1.0f, -0.3f));

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        glfwPollEvents();
        const double currentTime = glfwGetTime();
        const float deltaTime = static_cast<float>(std::clamp(
            currentTime - previousTime, 0.0, 0.05));
        previousTime = currentTime;
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
        if (glfwWindowShouldClose(window) == GLFW_TRUE) break;

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        if (framebufferWidth <= 0 || framebufferHeight <= 0) {
            glfwWaitEventsTimeout(0.05);
            previousTime = glfwGetTime();
            continue;
        }
        const float aspect = static_cast<float>(framebufferWidth) / framebufferHeight;

        updateCamera(window, camera, worldBounds, deltaTime);
        const float candidateAngle = std::fmod(
            modelAngle + deltaTime * glm::radians(35.0f), glm::radians(360.0f));
        const glm::mat4 candidateModel = glm::rotate(
            glm::mat4(1.0f),
            candidateAngle,
            glm::normalize(glm::vec3(0.6f, 1.0f, 0.2f)));
        const Bounds candidateBounds = transformBounds(localBounds, candidateModel);

        if (!intersectsObject(camera.position, camera.radius, candidateBounds)) {
            modelAngle = candidateAngle;
            model = candidateModel;
            worldBounds = candidateBounds;
        }
        const glm::mat4 view = glm::lookAt(
            camera.position, camera.position + camera.forward, camera.up);
        const glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), aspect, 0.1f, 100.0f);

        glClearColor(0.08f, 0.08f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(lightLocation, 1, glm::value_ptr(lightDirection));
        glBindVertexArray(vertexArray);

        glDrawArrays(
            GL_TRIANGLES,
            0,
            static_cast<GLsizei>(vertices.size()));

        glfwSwapBuffers(window);
    }

    glBindVertexArray(0);
    glUseProgram(0);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
