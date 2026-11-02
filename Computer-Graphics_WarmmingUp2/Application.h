#pragma once
#include "Board.h"
#include "BoardCommand.h"
#include "BoardRenderer.h"
#include "Mesh.h"
#include <filesystem>
#include <vector>
#include <string>

namespace warmup {
class Console;

// Coordinates the menu, board commands and file processing.
class Application {
public:
    explicit Application(Console* console = nullptr, std::filesystem::path outputDirectory = {});

    int run(const std::vector<std::string>& arguments);

private:
    int runBoard();
    int runMesh(const std::filesystem::path& path);
    bool readRectangles(Board::Rectangles& rectangles);
    Console* console_;
    BoardCommandParser commandParser_;
    BoardRenderer boardRenderer_;
    MeshParser meshParser_;
    MeshValidator meshValidator_;
    MeshReporter meshReporter_;

    std::filesystem::path outputDirectory_;
};
}
