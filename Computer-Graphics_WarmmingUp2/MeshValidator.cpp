#include "Mesh.h"
#include <cmath>

namespace warmup {
namespace {
bool inRange(double value, double minimum, double maximum) {
    return std::isfinite(value) && value >= minimum && value <= maximum;
}

bool vertexInRange(const Vertex& vertex) {
    return inRange(vertex.x, mesh_rules::vertexMinimum, mesh_rules::vertexMaximum) &&
           inRange(vertex.y, mesh_rules::vertexMinimum, mesh_rules::vertexMaximum) &&
           inRange(vertex.z, mesh_rules::vertexMinimum, mesh_rules::vertexMaximum);
}

bool textureInRange(const Texture& texture) {
    return inRange(texture.s, mesh_rules::textureMinimum, mesh_rules::textureMaximum) &&
           inRange(texture.t, mesh_rules::textureMinimum, mesh_rules::textureMaximum);
}

bool sameVertex(const Vertex& a, const Vertex& b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool sameTexture(const Texture& a, const Texture& b) {
    return a.s == b.s && a.t == b.t;
}

Vertex subtract(const Vertex& a, const Vertex& b) {
    return {a.x - b.x, a.y - b.y, a.z - b.z};
}

Vertex cross(const Vertex& a, const Vertex& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}

double squaredLength(const Vertex& vertex) {
    return vertex.x * vertex.x + vertex.y * vertex.y + vertex.z * vertex.z;
}

bool normalizeScale(Vertex& vertex) {
    double scale = std::abs(vertex.x);

    if (std::abs(vertex.y) > scale)
        scale = std::abs(vertex.y);

    if (std::abs(vertex.z) > scale)
        scale = std::abs(vertex.z);

    if (scale == 0)
        return false;
    vertex.x /= scale;
    vertex.y /= scale;
    vertex.z /= scale;

    return true;
}

bool isCollinear(const Vertex& a, const Vertex& b, const Vertex& c) {
    Vertex edgeA = subtract(b, a);
    Vertex edgeB = subtract(c, a);
    // Preserve the previous fix for very small, valid triangles.
    if (!normalizeScale(edgeA) || !normalizeScale(edgeB))
        return true;

    return squaredLength(cross(edgeA, edgeB)) <=
           mesh_rules::collinearSinSquared * squaredLength(edgeA) * squaredLength(edgeB);
}

bool resolveFace(const Mesh& mesh, const FaceRecord& record, ResolvedFace& face, std::string& error) {
    const TriangleIndices& triangle = record.value;

    face.hasTextures = triangle.corners[0].hasTexture;

    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        const CornerIndex& corner = triangle.corners[i];

        if (corner.vertex >= mesh.vertices.size()) {
            error = "Vertex index out of range at corner " + std::to_string(i + 1) + '.';

            return false;
        }

        const VertexRecord& vertex = mesh.vertices[corner.vertex];

        if (!vertex.valid || !vertexInRange(vertex.value)) {
            error = "Face references an invalid vertex declaration.";

            return false;
        }
        face.vertexIndices[i] = corner.vertex;
        face.vertices[i] = vertex.value;

        if (corner.hasTexture != face.hasTextures) {
            error = "Mixed face formats.";

            return false;
        }

        if (corner.hasTexture) {
            if (corner.texture >= mesh.textures.size()) {
                error = "Texture index out of range at corner " + std::to_string(i + 1) + '.';

                return false;
            }

            const TextureRecord& texture = mesh.textures[corner.texture];

            if (!texture.valid || !textureInRange(texture.value)) {
                error = "Face references an invalid texture declaration.";

                return false;
            }
            face.textures[i] = texture.value;
        }
    }
    // Compare each pair of corners within this face only.
    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        for (int j = i + 1; j < mesh_rules::triangleCorners; ++j) {
            if (face.vertexIndices[i] == face.vertexIndices[j]) {
                error = "Duplicate vertex index within this face.";

                return false;
            }

            if (sameVertex(face.vertices[i], face.vertices[j])) {
                error = "Duplicate vertex coordinates within this face.";

                return false;
            }

            if (face.hasTextures && sameTexture(face.textures[i], face.textures[j])) {
                error = "Duplicate texture coordinates within this face.";

                return false;
            }
        }
    }

    if (isCollinear(face.vertices[0], face.vertices[1], face.vertices[2])) {
        error = "Collinear vertices do not form a triangle.";

        return false;
    }

    return true;
}
}

ValidationResult MeshValidator::validate(const Mesh& mesh) const {
    ValidationResult result;

    for (std::size_t i = 0; i < mesh.vertices.size(); ++i) {
        const VertexRecord& record = mesh.vertices[i];

        if (record.valid && !vertexInRange(record.value)) {
            result.diagnostics.push_back(
                {record.line, 0, "Vertex " + std::to_string(i + 1) + " coordinate outside [-1, 1]."});
        }
    }

    for (std::size_t i = 0; i < mesh.textures.size(); ++i) {
        const TextureRecord& record = mesh.textures[i];

        if (record.valid && !textureInRange(record.value)) {
            result.diagnostics.push_back(
                {record.line, 0, "Texture " + std::to_string(i + 1) + " coordinate outside [0, 1]."});
        }
    }

    if (mesh.faces.empty())
        result.diagnostics.push_back({0, 0, "No faces found."});

    for (std::size_t i = 0; i < mesh.faces.size(); ++i) {
        const FaceRecord& record = mesh.faces[i];

        if (!record.valid)
            continue; // The parser already reported this syntax error.

        ResolvedFace face;

        face.number = i + 1;

        std::string error;

        if (resolveFace(mesh, record, face, error))
            result.faces.push_back(face);
        else
            result.diagnostics.push_back({record.line, i + 1, error});
    }

    return result;
}

bool succeeded(const ParseResult& parsed, const ValidationResult& validated) {
    return parsed.diagnostics.empty() && validated.diagnostics.empty() && !parsed.mesh.faces.empty() &&
           validated.faces.size() == parsed.mesh.faces.size();
}
}
