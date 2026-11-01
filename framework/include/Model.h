#pragma once

#include "Diagnostics.h"
#include "../dependencies/glm/glm/glm.hpp"
#include "../dependencies/glm/glm/gtc/matrix_transform.hpp"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <utility>

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
// Initial allocation hints, not scene capacity limits.
constexpr std::size_t MAX_ENTITIES = 50;
constexpr std::size_t MAX_LINES = 500;
using EntityId = std::size_t;
using LineId = std::size_t;

struct GlColor { float red = 1.0f, green = 1.0f, blue = 1.0f; };
enum class GLShape { None, Cube, Tetra, Pyramid, Custom, Imported };
enum class GLFace { None, Rect, Triangle, Mesh };
enum class PivotMode { Preserve, BottomCenter, Custom };
enum class OutlineMode { None, FeatureEdges, AllTriangleEdges, Explicit };

struct Transform3D {
    glm::vec3 basis{0.0f};
    glm::vec3 localBasis{0.0f};
    glm::mat3 rotation{1.0f};
    glm::vec3 scale{1.0f};
};

struct TextureImage {
    std::uint32_t width = 0, height = 0;
    std::vector<std::uint8_t> rgba;
    // Row zero maps to UV v=0. File decoding flips top-down images by default.
    bool valid() const {
        return width && height && width <= 16384 && height <= 16384
            && rgba.size() == std::size_t(width) * height * 4;
    }
};

struct Material {
    GlColor color;
    std::shared_ptr<const TextureImage> texture;
    bool useTexture = false;
    bool repeatTexture = true;
};

struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f};
    glm::vec2 uv{0.0f};
    GlColor color;
};

struct PlaneFigure {
    GLFace shape = GLFace::Mesh;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{0.0f};
    // Compatibility flag: true enables child-local SRT, never world detachment.
    bool independence = false;
    Transform3D transform;
    bool visible = true;
    glm::mat4 localAnimationMatrix{1.0f};
    std::vector<GlColor> vertexColors;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> texcoords;
    std::vector<std::uint32_t> triangleIndices;
    std::vector<std::uint32_t> outlineIndices; // GL_LINES pairs, mesh-local indices.
    Material material;
    OutlineMode outline = OutlineMode::None;
    float creaseAngleDegrees = 1.0f;
    std::string name;
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices,
                                   const std::vector<std::uint32_t>& triangles) {
        PlaneFigure mesh;
        mesh.triangleIndices = triangles;
        for (const auto& vertex : vertices) {
            mesh.localVertices.push_back(vertex.position);
            mesh.vertexNormals.push_back(vertex.normal);
            mesh.texcoords.push_back(vertex.uv);
            mesh.vertexColors.push_back(vertex.color);
        }
        return mesh;
    }
};
using Mesh = PlaneFigure;

struct Polyhedron {
    GLShape shape = GLShape::Custom;
    std::vector<PlaneFigure> faces;
    std::string name;
    std::vector<Mesh>& meshes() { return faces; }
    const std::vector<Mesh>& meshes() const { return faces; }
};
using Model = Polyhedron;

struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{1.0f};
    bool visible = true;
    PivotMode pivotMode = PivotMode::Preserve;
    std::optional<EntityId> parent; // Local SRT is relative to this entity.
    GlColor tint; // Per-entity multiplier; does not propagate to children.
    bool transformOnly = false;
};
struct FaceId { EntityId entityId = 0; std::size_t faceIndex = 0; };
struct Line3D {
    glm::vec3 start{0.0f}, end{0.0f};
    GlColor color;
    std::optional<EntityId> owner;
};

namespace detail {
inline bool finiteVector(const glm::vec3& v) {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}
inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c) for (int r = 0; r < 4; ++r)
        if (!std::isfinite(m[c][r])) return false;
    return true;
}
inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}
inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
    if (!mesh.triangleIndices.empty()) return mesh.triangleIndices;
    if (mesh.shape == GLFace::Triangle) return {0, 1, 2};
    if (mesh.shape == GLFace::Rect) return {0, 1, 2, 0, 2, 3};
    return {};
}
inline bool validMesh(const Mesh& m) {
    if (m.localVertices.size() < 3 || !validColor(m.color)
        || !validColor(m.material.color) || !finiteVector(m.normal)
        || !finiteMatrix(m.localAnimationMatrix)
        || !std::isfinite(m.creaseAngleDegrees) || m.creaseAngleDegrees < 0
        || m.creaseAngleDegrees > 180) return false;
    const auto count = m.localVertices.size();
    if (count > std::numeric_limits<std::uint32_t>::max()) return false;
    if ((!m.vertexColors.empty() && m.vertexColors.size() != count)
        || (!m.vertexNormals.empty() && m.vertexNormals.size() != count)
        || (!m.texcoords.empty() && m.texcoords.size() != count)
        || (m.material.texture && !m.material.texture->valid())
        || (m.material.useTexture && (!m.material.texture || m.texcoords.empty()))) return false;
    for (const auto& v : m.localVertices) if (!finiteVector(v)) return false;
    for (const auto& v : m.vertexNormals) if (!finiteVector(v)) return false;
    for (const auto& v : m.texcoords) if (!std::isfinite(v.x) || !std::isfinite(v.y)) return false;
    for (auto c : m.vertexColors) if (!validColor(c)) return false;
    const auto indices = triangles(m);
    if (indices.empty() || indices.size() % 3 || m.outlineIndices.size() % 2) return false;
    for (auto i : indices) if (i >= count) return false;
    for (auto i : m.outlineIndices) if (i >= count) return false;
    for (std::size_t i=0;i<indices.size();i+=3) {
        const auto a=glm::dvec3(m.localVertices[indices[i]]);
        const auto b=glm::dvec3(m.localVertices[indices[i+1]]);
        const auto c=glm::dvec3(m.localVertices[indices[i+2]]);
        const auto normal=glm::cross(b-a,c-a);
        const double area=glm::dot(normal,normal);
        if(!std::isfinite(area)||area<=0) return false;
    }
    return true;
}
inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());
    for (const auto& face : model.faces) for (const auto& p : face.localVertices) {
        low = glm::min(low, p); high = glm::max(high, p);
    }
    return {low, high};
}
inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);
    return {box.first.x * .5f + box.second.x * .5f, box.first.y,
            box.first.z * .5f + box.second.z * .5f};
}
inline glm::mat4 transformMatrix(const Transform3D& t) {
    glm::mat4 m = glm::translate(glm::mat4(1), t.basis);
    m *= glm::mat4(t.rotation);
    return glm::translate(glm::scale(m, t.scale), -t.localBasis);
}
inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
    glm::mat4 child(1);
    if (face.independence) {
        // Rotate/scale around a face pivot without moving an identity child.
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * transformMatrix(face.transform);
    }
    return transformMatrix(c.transform) * child * face.localAnimationMatrix;
}
} // namespace detail
} // namespace graphics
