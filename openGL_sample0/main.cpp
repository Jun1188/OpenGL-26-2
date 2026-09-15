#include <glew.h> // GLEW must be included before GLFW.
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "freeglut.lib")
#pragma comment(lib, "glfw3.lib")

namespace {

constexpr int kWindowWidth = 800;
constexpr int kWindowHeight = 600;
constexpr char kVertexShaderPath[] = "shaders/basic.vert";
constexpr char kFragmentShaderPath[] = "shaders/basic.frag";
constexpr char kMeshPath[] = "data/cube.txt";

void glfwErrorCallback(int errorCode, const char* description) {
    std::cerr << "[GLFW 오류 " << errorCode << "] " << description << '\n';
}

void framebufferSizeCallback(GLFWwindow*, int width, int height) {
    glViewport(0, 0, width, height);
}

bool readTextFile(const char* path, std::string& contents) {
    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "[파일 오류] 셰이더 파일을 열 수 없습니다: " << path << '\n';
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    contents = buffer.str();
    return true;
}

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

bool loadMeshVertices(const char* path, std::vector<GLfloat>& vertices) {
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

    constexpr std::size_t componentsPerVertex = 5;
    constexpr std::size_t verticesPerFace = 3;
    vertices.clear();
    vertices.reserve(validated.faces.size() * verticesPerFace * componentsPerVertex);

    for (const warmup::ResolvedFace& face : validated.faces) {
        for (int corner = 0; corner < warmup::mesh_rules::triangleCorners; ++corner) {
            const warmup::Vertex& position = face.vertices[corner];
            vertices.push_back(static_cast<GLfloat>(position.x));
            vertices.push_back(static_cast<GLfloat>(position.y));
            vertices.push_back(static_cast<GLfloat>(position.z));

            if (face.hasTextures) {
                const warmup::Texture& texture = face.textures[corner];
                vertices.push_back(static_cast<GLfloat>(texture.s));
                vertices.push_back(static_cast<GLfloat>(texture.t));
            } else {
                vertices.push_back(0.0f);
                vertices.push_back(0.0f);
            }
        }
    }

    std::cout << "메시 로드 완료: " << validated.faces.size()
              << "개 삼각형, " << vertices.size() / componentsPerVertex
              << "개 렌더링 정점\n";
    return true;
}

} // namespace

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
        "OpenGL 3D Brown Cube - Warm-up 2 Problem 6",
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

    // GLEW can generate GL_INVALID_ENUM while loading a core profile context.
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

    // Problem 6 mesh data becomes a non-indexed position/UV vertex stream.
    std::vector<GLfloat> vertices;
    if (!loadMeshVertices(kMeshPath, vertices)) {
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
        static_cast<GLsizeiptr>(vertices.size() * sizeof(GLfloat)),
        vertices.data(),
        GL_STATIC_DRAW);

    constexpr GLsizei stride = 5 * sizeof(GLfloat);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, nullptr);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        stride,
        reinterpret_cast<const void*>(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    const GLint mvpLocation = glGetUniformLocation(shaderProgram, "uMVP");
    const GLint textureLocation = glGetUniformLocation(shaderProgram, "uTexture");
    if (mvpLocation == -1 || textureLocation == -1) {
        std::cerr << "[셰이더 오류] uMVP 또는 uTexture uniform을 찾을 수 없습니다.\n";
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteProgram(shaderProgram);
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    constexpr GLubyte brownPixel[] = {139, 90, 43, 255};
    GLuint brownTexture = 0;
    glGenTextures(1, &brownTexture);
    glBindTexture(GL_TEXTURE_2D, brownTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        1,
        1,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        brownPixel);

    glUseProgram(shaderProgram);
    glUniform1i(textureLocation, 0);
    glUseProgram(0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    std::cout << "OpenGL " << glGetString(GL_VERSION) << '\n'
              << "Esc 키를 누르거나 창을 닫으면 종료됩니다.\n";

    while (glfwWindowShouldClose(window) == GLFW_FALSE) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
        const float aspect = framebufferHeight > 0
            ? static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight)
            : 1.0f;

        const glm::mat4 model = glm::rotate(
            glm::mat4(1.0f),
            static_cast<float>(glfwGetTime()) * glm::radians(35.0f),
            glm::normalize(glm::vec3(0.6f, 1.0f, 0.2f)));
        const glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 2.5f),
            glm::vec3(0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        const glm::mat4 projection = glm::perspective(
            glm::radians(45.0f), aspect, 0.1f, 100.0f);
        const glm::mat4 mvp = projection * view * model;

        glClearColor(0.08f, 0.10f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, glm::value_ptr(mvp));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, brownTexture);
        glBindVertexArray(vertexArray);
        glDrawArrays(
            GL_TRIANGLES,
            0,
            static_cast<GLsizei>(vertices.size() / 5));

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glBindVertexArray(0);
    glUseProgram(0);
    glDeleteTextures(1, &brownTexture);
    glDeleteBuffers(1, &vertexBuffer);
    glDeleteVertexArrays(1, &vertexArray);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
