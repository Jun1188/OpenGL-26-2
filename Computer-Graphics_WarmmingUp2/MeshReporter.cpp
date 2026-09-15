#include "Mesh.h"
#include <iostream>

namespace warmup {
namespace {
void printVertex(const Vertex& vertex, std::ostream& output) {
    output << '(' << vertex.x << ", " << vertex.y << ", " << vertex.z << ')';
}
void printTexture(const Texture& texture, std::ostream& output) {
    output << '(' << texture.s << ", " << texture.t << ')';
}
void printDiagnostic(const Diagnostic& diagnostic, std::ostream& output) {
    output << "Error";
    if (diagnostic.line != 0) output << " line " << diagnostic.line;
    if (diagnostic.face != 0) output << ", Face " << diagnostic.face;
    output << ": " << diagnostic.message << '\n';
}
void printFace(const ResolvedFace& face, std::ostream& output) {
    output << "Face " << face.number << " (";
    for (int i = 0; i < mesh_rules::triangleCorners; ++i) {
        if (i != 0) output << ", ";
        output << face.vertexIndices[i] + 1;
    }
    output << "): vertex ";
    for (const Vertex& vertex : face.vertices) {
        printVertex(vertex, output);
        output << ' ';
    }
    output << "\n  texture ";
    if (face.hasTextures) {
        for (const Texture& texture : face.textures) {
            printTexture(texture, output);
            output << ' ';
        }
    } else output << "not specified";
    output << '\n';
}
}

void MeshReporter::print(const ParseResult& parsed, const ValidationResult& validated) const {
    print(parsed, validated, std::cout);
}

void MeshReporter::print(const ParseResult& parsed, const ValidationResult& validated,
                         std::ostream& output) const {
    std::streamsize oldPrecision = output.precision();
    output.precision(mesh_rules::outputPrecision);

    // Show syntax errors first, then validation errors. Every error includes its source line.
    for (const Diagnostic& diagnostic : parsed.diagnostics) printDiagnostic(diagnostic, output);
    for (const Diagnostic& diagnostic : validated.diagnostics) printDiagnostic(diagnostic, output);

    for (const ResolvedFace& face : validated.faces) printFace(face, output);

    if (succeeded(parsed, validated)) output << "Validation succeeded. No duplicate coordinates within faces.\n";
    else output << "Validation failed.\n";

    std::size_t errorCount = parsed.diagnostics.size() + validated.diagnostics.size();
    output << "Faces: " << validated.faces.size() << '/' << parsed.mesh.faces.size()
           << " valid; errors: " << errorCount << '\n';
    output.precision(oldPrecision);
}
}
