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
    /// [API] 현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.
    bool setUniform(const std::string& name, float value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !std::isfinite(value)) return false;
        detail::gl().Uniform1f(location, value); return true;
    }
    /// [API] 현재 program의 추가 int/bool/sampler uniform을 설정한다.
    bool setUniform(const std::string& name, int value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0) return false;
        detail::gl().Uniform1i(location, value); return true;
    }
    /// [API] 현재 program의 추가 vec3 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::vec3& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteVector(value)) return false;
        detail::gl().Uniform3f(location, value.x, value.y, value.z); return true;
    }
    /// [API] 현재 program의 추가 mat4 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::mat4& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteMatrix(value)) return false;
        detail::gl().UniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]); return true;
    }
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
