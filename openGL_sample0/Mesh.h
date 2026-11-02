#pragma once
#include <cstddef>
#include <iosfwd>
#include <string>
#include <vector>

namespace warmup {
namespace mesh_rules {
const double vertexMinimum = -1.0;
const double vertexMaximum = 1.0;
const double textureMinimum = 0.0;
const double textureMaximum = 1.0;
const double collinearSinSquared = 1e-20;
const int outputPrecision = 9;
const int triangleCorners = 3;
}

struct Vertex {
    double x = 0, y = 0, z = 0;
};

struct Texture {
    double s = 0, t = 0;
};

struct CornerIndex {
    std::size_t vertex = 0;
    std::size_t texture = 0;
    bool hasTexture = false;
};

struct TriangleIndices {
    CornerIndex corners[mesh_rules::triangleCorners];
};

struct VertexRecord {
    std::size_t line = 0;
    bool valid = false;
    Vertex value;
};

struct TextureRecord {
    std::size_t line = 0;
    bool valid = false;
    Texture value;
};

struct FaceRecord {
    std::size_t line = 0;
    bool valid = false;
    TriangleIndices value;
};

struct Mesh {
    std::vector<VertexRecord> vertices;
    std::vector<TextureRecord> textures;
    std::vector<FaceRecord> faces;
};

struct Diagnostic {
    std::size_t line = 0;
    std::size_t face = 0;
    std::string message;
};

struct ParseResult {
    Mesh mesh;
    std::vector<Diagnostic> diagnostics;
};

struct ResolvedFace {
    std::size_t number = 0;
    std::size_t vertexIndices[mesh_rules::triangleCorners] = {};
    Vertex vertices[mesh_rules::triangleCorners];
    Texture textures[mesh_rules::triangleCorners];
    bool hasTextures = false;
};

struct ValidationResult {
    std::vector<ResolvedFace> faces;
    std::vector<Diagnostic> diagnostics;
};

class MeshParser {
public:
    /// 입력 메시와 문법 오류를 읽는다.
    ParseResult parse(std::istream& input) const;
};

class MeshValidator {
public:
    /// 메시의 좌표와 면을 검사한다.
    ValidationResult validate(const Mesh& mesh) const;
};

class MeshReporter {
public:
    /// 메시 검사 결과와 오류를 출력한다.
    void print(const ParseResult& parsed, const ValidationResult& validated) const;
    /// 메시 검사 결과와 오류를 출력한다.
    void print(const ParseResult& parsed, const ValidationResult& validated, std::ostream& output) const;
};

/// 파싱과 검사 결과가 성공했는지 반환한다.
bool succeeded(const ParseResult& parsed, const ValidationResult& validated);
}
