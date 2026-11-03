# 1단계: 정점 입력·셰이더 설정 변경 전후

수정 블록은 아래 diff의 -가 before, +가 after다. 새 RenderConfig 선언은 아래 전문을 제공한다. 프로젝트 사본은 같은 변경을 받는다.
입력 설정 → 정점 재포장 → VAO 속성 등록 → uniform 연결 → 렌더 순서다. 기존 인자 호출은 기본 RenderConfig를 사용한다.
빌드·테스트는 실행하지 않았다.

```diff
diff --git a/framework/detail/Application.cpp b/framework/detail/Application.cpp
index 673bc25..817554c 100644
--- a/framework/detail/Application.cpp
+++ b/framework/detail/Application.cpp
@@ -24,6 +24,16 @@ Application::~Application() {
     shutdown();
 }
 
+bool Application::configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath,
+                                    const std::filesystem::path& fragmentPath) {
+    if (!ensureContext("configureRenderer", __FILE__, __LINE__))
+        return false;
+    if (renderer_.initialize(vertexPath, fragmentPath, config))
+        return true;
+    requestClose();
+    return false;
+}
+
 void Application::setInitHandler(std::function<bool(Application&)> handler) {
     initHandler_ = std::move(handler);
 }
@@ -140,7 +150,7 @@ bool Application::initialize(const ApplicationConfig& config) {
 
     Diagnostics::locate(__FILE__, __LINE__, "initialize -> Renderer::initialize -> shader source/path setup");
 
-    if (!renderer_.initialize(vertexPath, fragmentPath))
+    if (!renderer_.initialize(vertexPath, fragmentPath, config.renderConfig))
         return false;
     assets_.emplace(root, config.features);
     renderOptions_.textures = config.features.textures;
diff --git a/framework/detail/Native.h b/framework/detail/Native.h
index 1e6c2db..7c1538a 100644
--- a/framework/detail/Native.h
+++ b/framework/detail/Native.h
@@ -205,6 +205,7 @@ struct GlApi {
     PFNGLBUFFERDATAPROC BufferData = nullptr;
     PFNGLDELETEBUFFERSPROC DeleteBuffers = nullptr;
     PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer = nullptr;
+    PFNGLVERTEXATTRIBIPOINTERPROC VertexAttribIPointer = nullptr;
     PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray = nullptr;
     PFNGLCREATESHADERPROC CreateShader = nullptr;
     PFNGLSHADERSOURCEPROC ShaderSource = nullptr;
@@ -276,6 +277,7 @@ struct GlApi {
         BufferData = resolve<decltype(BufferData)>("glBufferData");
         DeleteBuffers = resolve<decltype(DeleteBuffers)>("glDeleteBuffers");
         VertexAttribPointer = resolve<decltype(VertexAttribPointer)>("glVertexAttribPointer");
+        VertexAttribIPointer = resolve<decltype(VertexAttribIPointer)>("glVertexAttribIPointer");
         EnableVertexAttribArray = resolve<decltype(EnableVertexAttribArray)>("glEnableVertexAttribArray");
         CreateShader = resolve<decltype(CreateShader)>("glCreateShader");
         ShaderSource = resolve<decltype(ShaderSource)>("glShaderSource");
diff --git a/framework/detail/Renderer.cpp b/framework/detail/Renderer.cpp
index 11a19a8..10611f3 100644
--- a/framework/detail/Renderer.cpp
+++ b/framework/detail/Renderer.cpp
@@ -6,6 +6,7 @@
 #include <fstream>
 #include <iostream>
 #include <sstream>
+#include <cstring>
 
 namespace graphics {
 std::string readTextFile(const std::filesystem::path& path) {
@@ -68,9 +69,29 @@ GLuint compileShader(GLenum kind, const std::string& sourceText, const std::file
     return shader;
 }
 
-struct GpuVertex {
-    float position[3], color[3], uv[2], normal[3];
-};
+bool validLayout(const VertexLayout& layout) {
+    if (!layout.stride || layout.stride > std::size_t(std::numeric_limits<GLsizei>::max()) || layout.attributes.empty())
+        return false;
+    GLint maximum = 0;
+    gl().GetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maximum);
+    std::vector<GLuint> used;
+    for (const auto& a : layout.attributes) {
+        std::size_t bytes = 0;
+        switch (a.type) {
+        case GL_BYTE: case GL_UNSIGNED_BYTE: bytes = 1; break;
+        case GL_SHORT: case GL_UNSIGNED_SHORT: bytes = 2; break;
+        case GL_INT: case GL_UNSIGNED_INT: case GL_FLOAT: bytes = 4; break;
+        default: return false;
+        }
+        if (a.location >= GLuint(maximum) || a.components < 1 || a.components > 4 ||
+            a.offset > layout.stride || std::size_t(a.components)*bytes > layout.stride-a.offset ||
+            (a.integer && (a.normalized || a.type == GL_FLOAT)) ||
+            std::find(used.begin(), used.end(), a.location) != used.end())
+            return false;
+        used.push_back(a.location);
+    }
+    return true;
+}
 
 GLuint uploadTexture(const TextureImage& image, bool repeat) {
     auto& api = gl();
@@ -166,25 +187,32 @@ bool Renderer::initShader(const std::filesystem::path& vertexPath, const std::fi
         return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glLinkProgram", log,
                                    "Match vertex outputs and fragment inputs; inspect the linker log.");
     }
-    mvpLocation_ = gl.GetUniformLocation(shaderProgramID_, "uMVP");
-    modelLocation_ = gl.GetUniformLocation(shaderProgramID_, "uModel");
-    normalLocation_ = gl.GetUniformLocation(shaderProgramID_, "uNormalMatrix");
-    textureLocation_ = gl.GetUniformLocation(shaderProgramID_, "uTexture");
-    hasTextureLocation_ = gl.GetUniformLocation(shaderProgramID_, "uHasTexture");
-    lightingLocation_ = gl.GetUniformLocation(shaderProgramID_, "uLighting");
-    lightDirectionLocation_ = gl.GetUniformLocation(shaderProgramID_, "uLightDirection");
-    lightColorLocation_ = gl.GetUniformLocation(shaderProgramID_, "uLightColor");
-    overrideLocation_ = gl.GetUniformLocation(shaderProgramID_, "uOverrideColor");
-    outlineColorLocation_ = gl.GetUniformLocation(shaderProgramID_, "uOutlineColor");
-    tintLocation_ = gl.GetUniformLocation(shaderProgramID_, "uTint");
+    mvpLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.mvp.c_str());
+    modelLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.model.c_str());
+    normalLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.normal.c_str());
+    textureLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.texture.c_str());
+    hasTextureLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.hasTexture.c_str());
+    lightingLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.lighting.c_str());
+    lightDirectionLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.lightDirection.c_str());
+    lightColorLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.lightColor.c_str());
+    overrideLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.overrideColor.c_str());
+    outlineColorLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.outlineColor.c_str());
+    tintLocation_ = gl.GetUniformLocation(shaderProgramID_, config_.uniforms.tint.c_str());
 
     return mvpLocation_ >= 0 || Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glGetUniformLocation",
                                                     "Required uMVP uniform is missing or optimized away.");
 }
 
-bool Renderer::initialize(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
+bool Renderer::initialize(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath,
+                          const RenderConfig& config) {
     detail::gl().load();
+    if (!detail::validLayout(config.vertexLayout) ||
+        (!config.packVertices && config.vertexLayout.stride != sizeof(RenderVertex)))
+        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::initialize",
+                                   "Invalid layout, or custom stride without a vertex packer.");
+    const auto nextConfig = config; // Copy before destroying resources; config may alias current settings.
     destroy();
+    config_ = nextConfig;
 
     if (!initShader(vertexPath, fragmentPath)) {
         destroy();
@@ -207,14 +235,17 @@ bool Renderer::initialize(const std::filesystem::path& vertexPath, const std::fi
     gl.BindVertexArray(vao_);
     gl.BindBuffer(GL_ARRAY_BUFFER, vbo_);
     gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
-    using V = detail::GpuVertex;
-    gl.VertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(V), reinterpret_cast<void*>(offsetof(V, position)));
-    gl.VertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(V), reinterpret_cast<void*>(offsetof(V, color)));
-    gl.VertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(V), reinterpret_cast<void*>(offsetof(V, uv)));
-    gl.VertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(V), reinterpret_cast<void*>(offsetof(V, normal)));
-
-    for (GLuint i = 0; i < 4; ++i)
-        gl.EnableVertexAttribArray(i);
+    for (const auto& attribute : config_.vertexLayout.attributes) {
+        const auto pointer = reinterpret_cast<const void*>(attribute.offset);
+        if (attribute.integer)
+            gl.VertexAttribIPointer(attribute.location, attribute.components, attribute.type,
+                                    GLsizei(config_.vertexLayout.stride), pointer);
+        else
+            gl.VertexAttribPointer(attribute.location, attribute.components, attribute.type,
+                                   attribute.normalized ? GL_TRUE : GL_FALSE,
+                                   GLsizei(config_.vertexLayout.stride), pointer);
+        gl.EnableVertexAttribArray(attribute.location);
+    }
     gl.BindVertexArray(0);
     gl.BindBuffer(GL_ARRAY_BUFFER, 0);
 
@@ -224,7 +255,7 @@ bool Renderer::initialize(const std::filesystem::path& vertexPath, const std::fi
 bool Renderer::upload(const Scene& scene, bool textureMode) {
     Diagnostics::trace(PipelineStage::VertexInput, "Renderer::upload");
 
-    std::vector<detail::GpuVertex> vertices;
+    std::vector<RenderVertex> vertices;
     std::vector<GLuint> indices;
     std::vector<DrawRange> ranges;
     TextureMap stagedTextures;
@@ -359,7 +390,7 @@ bool Renderer::upload(const Scene& scene, bool textureMode) {
             indices.insert(indices.end(), {first, first + 1});
         }
 
-        if (vertices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / sizeof(detail::GpuVertex) ||
+        if (vertices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / config_.vertexLayout.stride ||
             indices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / sizeof(GLuint)) {
             cleanup();
 
@@ -367,6 +398,20 @@ bool Renderer::upload(const Scene& scene, bool textureMode) {
                                        "Buffer byte size exceeds GLsizeiptr range.");
         }
 
+        const auto byteCount = vertices.size() * config_.vertexLayout.stride;
+        std::vector<std::uint8_t> packed;
+        if (config_.packVertices) {
+            if (!config_.packVertices(vertices, packed) || packed.size() != byteCount) {
+                cleanup();
+                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "packVertices",
+                                           "Packer failed or byte count does not match vertex count times stride.");
+            }
+        } else {
+            packed.resize(byteCount);
+            if (byteCount)
+                std::memcpy(packed.data(), vertices.data(), byteCount);
+        }
+
         if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::VertexInput, "before buffer upload")) {
             cleanup();
 
@@ -374,8 +419,8 @@ bool Renderer::upload(const Scene& scene, bool textureMode) {
         }
         gl.BindVertexArray(vao_);
         gl.BindBuffer(GL_ARRAY_BUFFER, vbo_);
-        gl.BufferData(GL_ARRAY_BUFFER, GLsizeiptr(vertices.size() * sizeof(detail::GpuVertex)),
-                      vertices.empty() ? nullptr : vertices.data(), GL_STATIC_DRAW);
+        gl.BufferData(GL_ARRAY_BUFFER, GLsizeiptr(packed.size()),
+                      packed.empty() ? nullptr : packed.data(), GL_STATIC_DRAW);
         gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
         gl.BufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indices.size() * sizeof(GLuint)),
                       indices.empty() ? nullptr : indices.data(), GL_STATIC_DRAW);
@@ -477,6 +522,9 @@ bool Renderer::draw(const Scene& scene, const glm::mat4& view, const glm::mat4&
             const auto matrix = projection * view * model;
 
             gl.UniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(matrix));
+            if (config_.bindUniforms && !config_.bindUniforms(
+                    {shaderProgramID_, scene, range.entityId, range.lineId, range.faceIndex, model, view, projection, true}))
+                return false;
             gl.DrawElements(GL_TRIANGLES, range.count, GL_UNSIGNED_INT,
                             reinterpret_cast<const void*>(range.firstIndex * sizeof(GLuint)));
         }
@@ -550,6 +598,11 @@ bool Renderer::draw(const Scene& scene, const glm::mat4& view, const glm::mat4&
         } else
             gl.BindTexture(GL_TEXTURE_2D, 0);
 
+        if (config_.bindUniforms && !config_.bindUniforms(
+                {shaderProgramID_, scene, range.lineId ? std::nullopt : std::optional<EntityId>(range.entityId),
+                 range.lineId, range.faceIndex, model, view, projection, false}))
+            return false;
+
         if (!face || !options.wireframe) {
             gl.DrawElements(face ? GL_TRIANGLES : GL_LINES, range.count, GL_UNSIGNED_INT,
                             reinterpret_cast<const void*>(range.firstIndex * sizeof(GLuint)));
diff --git a/framework/include/Application.h b/framework/include/Application.h
index 12691d6..a7b4fc2 100644
--- a/framework/include/Application.h
+++ b/framework/include/Application.h
@@ -19,6 +19,7 @@ struct ApplicationConfig {
     std::filesystem::path projectRoot = ".";              // Relative to the executable.
     std::filesystem::path runtimeDirectory = "framework"; // Relative to projectRoot.
 
+    RenderConfig renderConfig;
     FeatureOptions features;
     bool verticalSync = true;
     DiagnosticOptions diagnostics;
@@ -109,6 +110,9 @@ public:
     AssetLoader& getAssets(); // Available from initialization callback until run() ends.
     /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.
     int run(const ApplicationConfig& config = ApplicationConfig{});
+    /// [API] 현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.
+    bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {},
+                           const std::filesystem::path& fragmentPath = {});
     /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
     void requestClose();
     /// [API] 기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.
diff --git a/framework/include/Graphics.h b/framework/include/Graphics.h
index 44f6412..e416853 100644
--- a/framework/include/Graphics.h
+++ b/framework/include/Graphics.h
@@ -3,6 +3,7 @@
 #include "Geometry.h"
 #include "Motion.h"
 #include "../detail/Native.h"
+#include "RenderConfig.h"
 #include <map>
 
 #include <array>
@@ -263,6 +264,7 @@ class Renderer {
         GLsizei count = 0, outlineCount = 0;
     };
 
+    RenderConfig config_;
     GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
     GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
     GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
@@ -292,7 +294,8 @@ public:
     // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
     /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
     bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{},
-                    const std::filesystem::path& fragmentPath = std::filesystem::path{});
+                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
+                    const RenderConfig& config = RenderConfig{});
     /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
     bool render(const Scene& scene, const Camera& camera, int width, int height,
                 const RenderOptions& options = RenderOptions{});

```
## include/RenderConfig.h

before: 없음

```cpp
#pragma once
#include "Model.h"
#include "../detail/Native.h"
#include <functional>

namespace graphics {
class Scene;

// Canonical CPU input for packing; independent of GL attribute locations.
struct RenderVertex {
    float position[3], color[3], uv[2], normal[3];
};

struct VertexAttribute {
    GLuint location = 0;
    GLint components = 3;
    GLenum type = GL_FLOAT;
    bool normalized = false;
    std::size_t offset = 0;
    bool integer = false; // ivec/uvec inputs use VertexAttribIPointer.
};

struct VertexLayout {
    std::size_t stride = sizeof(RenderVertex);
    std::vector<VertexAttribute> attributes{
        {0, 3, GL_FLOAT, false, offsetof(RenderVertex, position)},
        {1, 3, GL_FLOAT, false, offsetof(RenderVertex, color)},
        {2, 2, GL_FLOAT, false, offsetof(RenderVertex, uv)},
        {3, 3, GL_FLOAT, false, offsetof(RenderVertex, normal)}};
};

struct UniformBindings {
    std::string mvp = "uMVP", model = "uModel", normal = "uNormalMatrix";
    std::string texture = "uTexture", hasTexture = "uHasTexture", lighting = "uLighting";
    std::string lightDirection = "uLightDirection", lightColor = "uLightColor";
    std::string overrideColor = "uOverrideColor", outlineColor = "uOutlineColor", tint = "uTint";
};

struct DrawUniformContext {
    GLuint program;
    const Scene& scene;
    std::optional<EntityId> entity;
    std::optional<LineId> line;
    std::size_t face;
    glm::mat4 model, view, projection;
    bool depthOnly = false;
};

struct RenderConfig {
    VertexLayout vertexLayout;
    UniformBindings uniforms;
    // Must output exactly input.size()*stride bytes, preserving vertex order/count.
    std::function<bool(const std::vector<RenderVertex>&, std::vector<std::uint8_t>&)> packVertices;
    // Runs with program bound, after built-in uniforms, before each range/pass.
    std::function<bool(const DrawUniformContext&)> bindUniforms;
};
} // namespace graphics

```