#pragma once
#include "Model.h"
#include <algorithm>
#include <map>
#include <numeric>
#include <stdexcept>

namespace graphics {
inline void recalculateNormals(Mesh& mesh) {
    mesh.vertexNormals.assign(mesh.localVertices.size(), glm::vec3(0));
    const auto indices = detail::triangles(mesh);
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i+1], c = indices[i+2];
        const auto n = glm::cross(mesh.localVertices[b] - mesh.localVertices[a],
                                  mesh.localVertices[c] - mesh.localVertices[a]);
        mesh.vertexNormals[a] += n; mesh.vertexNormals[b] += n; mesh.vertexNormals[c] += n;
    }
    for (auto& n : mesh.vertexNormals) {
        const float size = glm::length(n);
        n = size > 1e-12f ? n / size : glm::vec3(0);
    }
}

// Weld equal positions for edge topology, while preserving separate UV/normal vertices.
inline std::vector<std::uint32_t> meshOutline(const Mesh& mesh,
                                             OutlineMode mode = OutlineMode::FeatureEdges) {
    if (mode == OutlineMode::None) return {};
    if (mode == OutlineMode::Explicit) return mesh.outlineIndices;
    using Position = std::array<float, 3>;
    std::map<Position, std::uint32_t> points;
    std::vector<std::uint32_t> welded;
    for (std::uint32_t i = 0; i < mesh.localVertices.size(); ++i) {
        const auto& p = mesh.localVertices[i];
        const auto result = points.emplace(Position{p.x, p.y, p.z}, i);
        welded.push_back(result.first->second);
    }
    struct Edge { std::uint32_t a = 0, b = 0; std::vector<glm::vec3> normals; };
    std::map<std::pair<std::uint32_t, std::uint32_t>, Edge> edges;
    const auto indices = detail::triangles(mesh);
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i+1], c = indices[i+2];
        auto normal = glm::cross(mesh.localVertices[b] - mesh.localVertices[a],
                                 mesh.localVertices[c] - mesh.localVertices[a]);
        const auto length = glm::length(normal);
        if (length <= 1e-12f) continue;
        normal /= length;
        const std::array<std::uint32_t, 3> corners{a,b,c};
        for (int e = 0; e < 3; ++e) {
            auto first = welded[corners[e]], second = welded[corners[(e+1)%3]];
            if (first == second) continue;
            if (first > second) std::swap(first, second);
            auto& edge = edges[{first, second}];
            edge.a = first; edge.b = second; edge.normals.push_back(normal);
        }
    }
    const float threshold = std::cos(glm::radians(mesh.creaseAngleDegrees));
    std::vector<std::uint32_t> result;
    for (const auto& item : edges) {
        const auto& edge = item.second;
        const bool show = mode == OutlineMode::AllTriangleEdges || edge.normals.size() != 2
            || glm::dot(edge.normals[0], edge.normals[1]) < threshold;
        if (show) { result.push_back(edge.a); result.push_back(edge.b); }
    }
    return result;
}

namespace detail {
// Ear clipping handles planar simple concave polygons, with either winding.
inline std::vector<std::uint32_t> triangulate(const std::vector<glm::vec3>& points) {
    if (points.size() < 3) throw std::runtime_error("Face has fewer than three corners.");
    glm::vec3 normal(0);
    for (std::size_t i=0; i<points.size(); ++i) {
        const auto& a=points[i]; const auto& b=points[(i+1)%points.size()];
        normal += glm::vec3((a.y-b.y)*(a.z+b.z), (a.z-b.z)*(a.x+b.x), (a.x-b.x)*(a.y+b.y));
    }
    if (glm::length(normal) < 1e-12f) throw std::runtime_error("Degenerate polygon.");
    const auto absNormal=glm::abs(normal);
    const int axis=absNormal.x>absNormal.y ? (absNormal.x>absNormal.z?0:2) : (absNormal.y>absNormal.z?1:2);
    std::vector<glm::dvec2> projected;
    for (const auto& p : points) projected.push_back(axis==0 ? glm::dvec2(p.y,p.z)
        : axis==1 ? glm::dvec2(p.x,p.z) : glm::dvec2(p.x,p.y));
    double area=0;
    for(std::size_t i=0;i<projected.size();++i) {
        const auto& a=projected[i]; const auto& b=projected[(i+1)%projected.size()];
        area+=a.x*b.y-b.x*a.y;
    }
    if (std::abs(area)<1e-14) throw std::runtime_error("Degenerate polygon projection.");
    const double direction=area>0?1:-1;
    auto cross=[](glm::dvec2 a,glm::dvec2 b,glm::dvec2 c) {return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);};
    std::vector<std::uint32_t> polygon(points.size()), result;
    std::iota(polygon.begin(),polygon.end(),0);
    while(polygon.size()>3) {
        bool clipped=false;
        for(std::size_t i=0;i<polygon.size();++i) {
            const auto a=polygon[(i+polygon.size()-1)%polygon.size()],b=polygon[i],c=polygon[(i+1)%polygon.size()];
            if(direction*cross(projected[a],projected[b],projected[c])<=1e-14) continue;
            bool inside=false;
            for(auto p:polygon) if(p!=a&&p!=b&&p!=c
                && direction*cross(projected[a],projected[b],projected[p])>=-1e-14
                && direction*cross(projected[b],projected[c],projected[p])>=-1e-14
                && direction*cross(projected[c],projected[a],projected[p])>=-1e-14) {inside=true;break;}
            if(inside) continue;
            result.insert(result.end(),{a,b,c}); polygon.erase(polygon.begin()+i); clipped=true; break;
        }
        if(!clipped) throw std::runtime_error("Cannot triangulate polygon; check repeated corners or self-intersections.");
    }
    result.insert(result.end(),polygon.begin(),polygon.end());
    return result;
}
inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first=std::uint32_t(destination.localVertices.size());
    destination.localVertices.insert(destination.localVertices.end(),source.localVertices.begin(),source.localVertices.end());
    destination.vertexNormals.insert(destination.vertexNormals.end(),source.vertexNormals.begin(),source.vertexNormals.end());
    destination.texcoords.insert(destination.texcoords.end(),source.texcoords.begin(),source.texcoords.end());
    for(auto i:triangles(source)) destination.triangleIndices.push_back(first+i);
    destination.shape=GLFace::Mesh;
}
inline bool shareEdge(const Mesh& a,const Mesh& b) {
    const auto ae=meshOutline(a),be=meshOutline(b);
    for(std::size_t i=0;i<ae.size();i+=2) for(std::size_t j=0;j<be.size();j+=2) {
        const auto p=a.localVertices[ae[i]],q=a.localVertices[ae[i+1]],r=b.localVertices[be[j]],s=b.localVertices[be[j+1]];
        if((p==r&&q==s)||(p==s&&q==r)) return true;
    }
    return false;
}
inline void mergeCoplanar(Model& model) {
    for(std::size_t i=0;i<model.faces.size();++i) {
        auto& face=model.faces[i];
        if(glm::length(face.normal)<1e-12f) continue;
        for(std::size_t j=i+1;j<model.faces.size();) {
            const auto& other=model.faces[j];
            const bool samePlane=glm::dot(face.normal,other.normal)>1-1e-6f
                && std::abs(glm::dot(face.normal,other.localVertices[0]-face.localVertices[0]))<1e-6f;
            if(samePlane && face.texcoords.empty()==other.texcoords.empty() && shareEdge(face,other)) {
                appendMesh(face,other); model.faces.erase(model.faces.begin()+j); j=i+1;
            } else ++j;
        }
    }
}
} // namespace detail
} // namespace graphics
