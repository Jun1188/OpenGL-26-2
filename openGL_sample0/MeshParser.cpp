#include "Mesh.h"
#include <cmath>
#include <istream>
#include <sstream>

namespace warmup {
namespace {

/// 숫자 토큰 전체를 해석한다.
bool readNumber(const std::string& token, double& value, std::string& error) {

    if (token.find('x') != std::string::npos || token.find('X') != std::string::npos) {
        error = "Invalid number: " + token;
        return false;
    }
    std::istringstream input(token);
    double number = 0;
    char extra;
    if (!(input >> number) || input >> extra || !std::isfinite(number)) {
        error = "Invalid number or numeric range exceeded: " + token;
        return false;
    }
    value = number;
    return true;
}

/// 외부 인덱스를 내부 인덱스로 변환한다.
bool readIndex(const std::string& token, std::size_t& index, std::string& error) {
    if (token.empty()) {
        error = "Missing index.";
        return false;
    }
    for (char ch : token) {
        if (ch < '0' || ch > '9') {
            error = "Invalid positive index: " + token;
            return false;
        }
    }
    std::istringstream input(token);
    std::size_t number = 0;
    if (!(input >> number) || number == 0) {
        error = "Index is outside the supported positive integer range: " + token;
        return false;
    }
    index = number - 1;
    return true;
}

/// 정점 좌표를 해석한다.
bool readVertex(const std::vector<std::string>& tokens, Vertex& vertex, std::string& error) {
    if (tokens.size() != 3) {
        error = "A vertex requires three coordinate components.";
        return false;
    }
    return readNumber(tokens[0], vertex.x, error)
        && readNumber(tokens[1], vertex.y, error)
        && readNumber(tokens[2], vertex.z, error);
}

/// 텍스처 좌표를 해석한다.
bool readTexture(const std::vector<std::string>& tokens, Texture& texture, std::string& error) {
    if (tokens.size() != 2) {
        error = "A texture requires two coordinate components.";
        return false;
    }
    return readNumber(tokens[0], texture.s, error)
        && readNumber(tokens[1], texture.t, error);
}

/// 면의 정점·텍스처 인덱스를 해석한다.
bool readCorner(const std::string& token, CornerIndex& corner, std::string& error) {
    CornerIndex candidate;
    std::size_t slash = token.find('/');
    if (slash != std::string::npos && token.find('/', slash + 1) != std::string::npos) {
        error = "Expected vertex or vertex/texture: " + token;
        return false;
    }
    if (!readIndex(token.substr(0, slash), candidate.vertex, error)) return false;
    if (slash != std::string::npos) {
        if (!readIndex(token.substr(slash + 1), candidate.texture, error)) return false;
        candidate.hasTexture = true;
    }
    corner = candidate;
    return true;
}

/// 삼각형 면을 해석한다.
bool readFace(const std::vector<std::string>& tokens, TriangleIndices& face, std::string& error) {
    if (tokens.size() != mesh_rules::triangleCorners) {
        error = "A face requires exactly three corners.";
        return false;
    }
    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        if (!readCorner(tokens[i], face.corners[i], error)) {
            error = "Corner " + std::to_string(i + 1) + ": " + error;
            return false;
        }
        if (face.corners[i].hasTexture != face.corners[0].hasTexture) {
            error = "Mixed face formats: use three vertex indices or three vertex/texture pairs.";
            return false;
        }
    }
    return true;
}
}

/// 입력 메시와 문법 오류를 읽는다.
ParseResult MeshParser::parse(std::istream& input) const {
    ParseResult result;
    std::string line;
    std::size_t lineNumber = 0;
    while (std::getline(input, line)) {
        ++lineNumber;

        if (lineNumber == 1 && line.compare(0, 3, "\xef\xbb\xbf") == 0) line.erase(0, 3);
        std::size_t comment = line.find('#');
        if (comment != std::string::npos) line.erase(comment);
        std::istringstream row(line);
        std::string kind, token;
        if (!(row >> kind)) continue;
        std::vector<std::string> tokens;
        while (row >> token) tokens.push_back(token);

        std::string error;
        std::size_t faceNumber = 0;
        if (kind == "v") {
            VertexRecord record;
            record.line = lineNumber;
            record.valid = readVertex(tokens, record.value, error);
            result.mesh.vertices.push_back(record);
        } else if (kind == "vt") {
            TextureRecord record;
            record.line = lineNumber;
            record.valid = readTexture(tokens, record.value, error);
            result.mesh.textures.push_back(record);
        } else if (kind == "f") {
            FaceRecord record;
            record.line = lineNumber;
            record.valid = readFace(tokens, record.value, error);
            result.mesh.faces.push_back(record);
            faceNumber = result.mesh.faces.size();
        } else {
            error = "Unsupported record: " + kind;
        }
        if (!error.empty()) result.diagnostics.push_back({lineNumber, faceNumber, error});
    }
    if (input.bad() || (input.fail() && !input.eof())) {
        result.diagnostics.push_back({lineNumber, 0, "Input stream read failed."});
    }
    return result;
}
}
