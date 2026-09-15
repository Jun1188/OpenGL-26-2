#include "Application.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <system_error>

namespace warmup {
namespace {
bool isWhitespace(char ch) {
    return ch == ' ' || ch == '\t' || ch == '\r' || ch == '\n';
}

std::string trimmed(const std::string& text) {
    std::size_t begin = 0;
    std::size_t end = text.size();
    while (begin < end && isWhitespace(text[begin])) ++begin;
    while (end > begin && isWhitespace(text[end - 1])) --end;
    return text.substr(begin, end - begin);
}

std::filesystem::path utf8Path(const std::string& text) {
    return std::filesystem::path(std::u8string(text.begin(), text.end()));
}

bool parseRectangle(const std::string& line, Rectangle& rectangle) {
    std::istringstream input(line);
    int coordinates[4] = {};
    for (int i = 0; i < 4; ++i) {
        std::string token;
        if (!(input >> token)) return false;
        std::istringstream number(token);
        char extra;
        if (!(number >> coordinates[i]) || number >> extra) return false;
        if (coordinates[i] < 1 || coordinates[i] > board_rules::initialSize) return false;
    }
    std::string extra;
    if (input >> extra) return false;

    int x1 = coordinates[0], y1 = coordinates[1];
    int x2 = coordinates[2], y2 = coordinates[3];
    if (x1 > x2) { int temporary = x1; x1 = x2; x2 = temporary; }
    if (y1 > y2) { int temporary = y1; y1 = y2; y2 = temporary; }
    rectangle = Rectangle({x1 - 1, y1 - 1}, {x2 - x1 + 1, y2 - y1 + 1});
    return true;
}


std::string areaMessage(std::size_t id, const Rectangle& rectangle) {
    const Size size = rectangle.size();
    return "Area " + std::to_string(id + 1) + ": " + std::to_string(size.width)
        + " x " + std::to_string(size.height) + " = " + std::to_string(rectangle.area());
}

bool nextOutputPath(const std::filesystem::path& directory,
                    const std::filesystem::path& inputPath,
                    std::filesystem::path& outputPath) {
    std::error_code error;
    std::filesystem::create_directories(directory, error);
    if (error) return false;
    std::filesystem::path stem = inputPath.stem();
    if (stem.empty()) stem = "output";
    for (std::size_t number = 1; number != 0; ++number) {
        const std::filesystem::path candidate = directory
            / (stem.wstring() + L"-" + std::to_wstring(number) + L".txt");
        const bool exists = std::filesystem::exists(candidate, error);
        if (error) return false;
        if (!exists) {
            outputPath = candidate;
            return true;
        }
    }
    return false;
}

std::string utf8Filename(const std::filesystem::path& path) {
    const std::u8string text = path.filename().u8string();
    return std::string(text.begin(), text.end());
}
}

Application::Application(Console* console, std::filesystem::path outputDirectory)
    : console_(console), outputDirectory_(std::move(outputDirectory)) {
    if (outputDirectory_.empty()) outputDirectory_ = std::filesystem::current_path() / "outout-6";
}

bool Application::readRectangles(Board::Rectangles& rectangles) {
    Board::Rectangles candidate;
    for (std::size_t id = 0; id < candidate.size(); ++id) {
        for (;;) {
            std::cout << "Rectangle " << id + 1 << " (x1 y1 x2 y2, 1.."
                    << board_rules::initialSize << "; q exits): ";
            std::string line;
            if (!std::getline(std::cin, line) || trimmed(line) == "q") return false;
            Rectangle rectangle;
            if (!parseRectangle(line, rectangle)) {
                std::cout << "Invalid coordinates.\n";
                continue;
            }
            candidate[id] = rectangle;
            break;
        }
    }
    rectangles = candidate;
    return true;
}

int Application::runBoard() {
    Board board;
    Board::Rectangles rectangles;


    if (!readRectangles(rectangles)) return 0;
    if (board.reset(rectangles) != ChangeError::none) return 1;

    std::string status;

    while(1) {
        boardRenderer_.draw(board, console_);
        if (!status.empty()) { std::cout << status << '\n'; status.clear(); }
        std::cout << commandParser_.help() << "Command: ";
        std::string line;
        if (!std::getline(std::cin, line)) return 0;
        BoardCommand command;
        if (!commandParser_.parse(line, command)) { status = "Invalid command."; continue; }
        ChangeError error = ChangeError::none;
        switch (command.kind) {
        case CommandKind::quit:
            return 0;
        case CommandKind::reset:
            if (!readRectangles(rectangles)) return 0;
            error = board.reset(rectangles);
            break;
        case CommandKind::resizeBoard:
            error = board.resizeBoard(command.delta.x);
            break;
        case CommandKind::move:
            error = board.moveRectangle(command.rectangle, command.delta);
            break;
        case CommandKind::resize:
            error = board.resizeRectangle(command.rectangle, command.delta);
            break;
        case CommandKind::area:
            status = areaMessage(command.rectangle, board.rectangles()[command.rectangle]);
            break;
        }
        if (error != ChangeError::none) status = std::string(describe(error));
    }
}

int Application::runMesh(const std::filesystem::path& path) {
    std::ifstream input(path);
    if (!input) { std::cout << "Cannot open input file.\n"; return 1; }
    const ParseResult parsed = meshParser_.parse(input);
    const ValidationResult validated = meshValidator_.validate(parsed.mesh);
    std::ostringstream report;
    meshReporter_.print(parsed, validated, report);
    const std::string reportText = report.str();
    std::cout << reportText;

    std::filesystem::path outputPath;
    if (!nextOutputPath(outputDirectory_, path, outputPath)) {
        std::cout << "Cannot select output file.\n";
        return 1;
    }
    std::ofstream output(outputPath, std::ios::binary);
    if (!output) {
        std::cout << "Cannot create output file.\n";
        return 1;
    }
    output << reportText;
    output.close();
    if (!output) {
        std::cout << "Cannot write output file.\n";
        return 1;
    }
    std::cout << "Output file: " << utf8Filename(outputPath) << '\n';
    if (!std::cout) return 1;
    return succeeded(parsed, validated) ? 0 : 2;
}

int Application::run(const std::vector<std::string>& arguments) {
    if (arguments.size() == 1 && arguments[0] == "--board") return runBoard();
    if (arguments.size() == 2 && arguments[0] == "--parse") return runMesh(utf8Path(arguments[1]));
    if (!arguments.empty()) {
        std::cout << "Usage: --board OR --parse input.txt\n";
        return 1;
    }
    for (;;) {
        std::cout << "5: rectangles, 6: parse file, q: quit\n> ";
        std::string choice;
        if (!std::getline(std::cin, choice)) return 0;
        choice = trimmed(choice);
        if (choice == "q") return 0;
        if (choice == "5") return runBoard();
        if (choice == "6") {
            std::cout << "Input file path (without quotes; q returns to menu): ";
            std::string path;
            if (!std::getline(std::cin, path)) return 0;
            if (trimmed(path) != "q") runMesh(utf8Path(path));
        } else std::cout << "Invalid selection.\n";
    }
}
}
