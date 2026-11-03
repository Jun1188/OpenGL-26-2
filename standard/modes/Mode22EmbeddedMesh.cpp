#include "PracticeMode.h"
#include "PracticeCommon.h"


namespace {
struct EmbeddedCorner {
    std::size_t vertex = 0;
    std::optional<std::size_t> uv;
};
using EmbeddedTriangle = std::array<EmbeddedCorner,3>;
struct EmbeddedResult {
    graphics::Model model;
    std::vector<std::string> errors;
};

inline EmbeddedResult validateEmbedded(const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec2>& uvs,const std::vector<EmbeddedTriangle>& triangles) {
    EmbeddedResult result;
    result.model.shape = graphics::GLShape::Custom;
    const auto validPosition = [](const glm::vec3& p) {
        return std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z)
            && glm::all(glm::lessThanEqual(glm::abs(p),glm::vec3(1)));
    };
    const auto validUV = [](const glm::vec2& uv) {
        return std::isfinite(uv.x) && std::isfinite(uv.y)
            && glm::all(glm::greaterThanEqual(uv,glm::vec2(0)))
            && glm::all(glm::lessThanEqual(uv,glm::vec2(1)));
    };
    for (std::size_t i=0; i<positions.size(); ++i)
        if (!validPosition(positions[i])) result.errors.push_back("Invalid vertex "+std::to_string(i));
    for (std::size_t i=0; i<uvs.size(); ++i)
        if (!validUV(uvs[i])) result.errors.push_back("Invalid UV "+std::to_string(i));
    for (std::size_t face=0; face<triangles.size(); ++face) {
        const auto& corners = triangles[face];
        const bool textured = corners[0].uv.has_value();
        bool valid = true;
        std::array<glm::vec3,3> points{};
        std::array<glm::vec2,3> texcoords{};
        for (std::size_t i=0; i<3; ++i) {
            if (corners[i].vertex>=positions.size() || corners[i].uv.has_value()!=textured) { valid=false; break; }
            points[i] = positions[corners[i].vertex];
            if (!validPosition(points[i])) { valid=false; break; }
            if (textured) {
                if (*corners[i].uv>=uvs.size()) { valid=false; break; }
                texcoords[i] = uvs[*corners[i].uv];
                if (!validUV(texcoords[i])) { valid=false; break; }
            }
        }
        if (valid)
            for (std::size_t i=0; i<3; ++i)
                for (std::size_t j=i+1; j<3; ++j)
                    if (corners[i].vertex==corners[j].vertex || glm::all(glm::equal(points[i],points[j]))
                        || (textured && glm::all(glm::equal(texcoords[i],texcoords[j])))) valid=false;
        glm::dvec3 normal(0);
        if (valid) {
            auto a=glm::dvec3(points[1])-glm::dvec3(points[0]);
            auto b=glm::dvec3(points[2])-glm::dvec3(points[0]);
            const auto scaleA=std::max({std::abs(a.x),std::abs(a.y),std::abs(a.z)});
            const auto scaleB=std::max({std::abs(b.x),std::abs(b.y),std::abs(b.z)});
            if (scaleA==0 || scaleB==0) valid=false;
            else {
                a/=scaleA; b/=scaleB; normal=glm::cross(a,b);
                if (glm::dot(normal,normal)<=1e-12*glm::dot(a,a)*glm::dot(b,b)) valid=false;
            }
        }
        if (!valid) { result.errors.push_back("Invalid triangle "+std::to_string(face)); continue; }
        std::vector<graphics::Vertex> vertices(3);
        for (std::size_t i=0; i<3; ++i) {
            vertices[i].position=points[i];
            vertices[i].normal=glm::vec3(glm::normalize(normal));
            vertices[i].uv=texcoords[i];
            vertices[i].color={.8f,.6f,.2f};
        }
        result.model.faces.push_back(graphics::Mesh::fromVertices(vertices,{0,1,2}));
    }
    if (result.model.faces.empty()) result.errors.push_back("No valid triangles.");
    return result;
}

class EmbeddedMesh final : public PracticeMode {
    graphics::Application& app_;
public:
    explicit EmbeddedMesh(graphics::Application& app) : PracticeMode(app), app_(app) {}
    bool initScene() override {
        const std::vector<glm::vec3> positions{
            {-.5f,-.5f,.5f},{.5f,-.5f,.5f},{0,.5f,0},{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f}};
        const std::vector<glm::vec2> uvs{{0,0},{1,0},{.5f,1}};
        const auto face=[](std::size_t a,std::size_t b,std::size_t c) {
            return EmbeddedTriangle{EmbeddedCorner{a,0},EmbeddedCorner{b,1},EmbeddedCorner{c,2}};
        };
        const auto result=validateEmbedded(positions,uvs,
            {face(0,1,2),face(1,4,2),face(4,3,2),face(3,0,2),face(0,3,4),face(0,4,1)});
        for (const auto& error : result.errors) std::cerr << error << '\n';
        if (!result.errors.empty()) return false;
        auto& scene=app_.getScene(); scene.clear();
        const auto model=scene.addModel(result.model,graphics::Transform3D{},graphics::PivotMode::Preserve);
        app_.getCamera()=graphics::Camera{};
        app_.getCamera().position={2,1.5f,3};
        return model.has_value();
    }
    void onKey(int key, int action, int) override {
        if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) app_.requestClose();
    }
};
}

std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app) {
    return std::make_unique<EmbeddedMesh>(app);
}
