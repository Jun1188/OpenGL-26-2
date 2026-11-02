#include "../Application.h"
#include "../Board.h"
#include "../BoardCommand.h"
#include "../BoardRenderer.h"
#include "../Mesh.h"
#include <array>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>
#include <vector>

using namespace warmup;

namespace {
int checks = 0;

void expect(bool condition, const std::string& name) {
    ++checks;

    if (!condition)
        throw std::runtime_error(name);
}

Board makeBoard(Point point, Size size) {
    Board board;

    expect(board.reset({Rectangle(point, size), Rectangle({10, 10}, {2, 2})}) == ChangeError::none, "fixture reset");

    return board;
}

void expectRejected(Board& board, Delta delta, const std::string& name) {
    const auto before = board.rectangles();

    expect(board.resizeRectangle(0, delta) != ChangeError::none, name + " rejected");
    expect(board.rectangles() == before, name + " leaves BOTH shapes unchanged");
}

void expectSize(Board& board, Delta delta, Size expected, const std::string& name) {
    expect(board.resizeRectangle(0, delta) == ChangeError::none, name + " accepted");
    expect(board.rectangles()[0].size() == expected, name + " changes only valid axes");
}

void testBoardTransactions() {
    auto board = makeBoard({1, 1}, {2, 1});

    for (int i = 0; i < 10; ++i)
        expectRejected(board, {1, -1}, "a at height 1");
    board = makeBoard({1, 1}, {1, 2});
    expectRejected(board, {-1, 1}, "A at width 1");
    expectRejected(board, {-1, -1}, "s at width 1");
    board = makeBoard({28, 1}, {2, 2});
    expectRejected(board, {1, 1}, "S at x boundary");
    expectRejected(board, {1, -1}, "a at x boundary");
    board = makeBoard({1, 28}, {2, 2});
    expectRejected(board, {-1, 1}, "A at y boundary");
    expectRejected(board, {1, 1}, "S at y boundary");
    board = makeBoard({0, 0}, {2, 2});
    expect(board.moveRectangle(0, {-1, 0}) == ChangeError::none, "cross x seam");
    expectRejected(board, {1, 1}, "S while x is wrapped");
    board = makeBoard({0, 0}, {2, 2});
    expect(board.moveRectangle(0, {-1, 0}) == ChangeError::none, "cross x seam for dual wrap");
    expect(board.moveRectangle(0, {0, -1}) == ChangeError::none, "cross y seam for dual wrap");

    for (int i = 0; i < 10; ++i)
        expectRejected(board, {1, 1}, "S while both axes are wrapped");
    expectSize(board, {-1, -1}, {1, 1}, "shrink while both axes are wrapped");
    board = makeBoard({0, 0}, {30, 2});
    expect(board.moveRectangle(0, {-1, 0}) == ChangeError::none, "wrap maximum width");
    expectRejected(board, {1, -1}, "a at maximum width");
    expectRejected(board, {1, 1}, "S at maximum width");

    const auto before = board.rectangles();

    expect(board.resizeBoard(-1) == ChangeError::boundary, "shrink refuses negative coordinate");
    expect(board.rectangles() == before && board.size() == 30, "board shrink atomic");
    expect(board.moveRectangle(99, {1, 0}) == ChangeError::invalidRectangle, "invalid id");
    expect(board.resizeRectangle(0, {100, 0}) == ChangeError::invalidStep, "invalid resize delta");
    expect(board.moveRectangle(0, {1, 1}) == ChangeError::invalidStep, "invalid movement delta");

    bool threw = false;

    try {
        const Rectangle invalid({0, 0}, {0, 1});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    expect(threw, "rectangle enforces positive size");
}

void testCommands() {
    BoardCommandParser parser;

    for (char key : std::string("xXyYsSiIjJaAb")) {
        for (char prefix : std::string("12")) {
            const std::string command{prefix, key};
            BoardCommand parsed;
            bool valid = parser.parse("  " + command + "  ", parsed);

            expect(valid && parsed.rectangle == static_cast<std::size_t>(prefix - '1'), "all shape prefixes");
        }
    }

    BoardCommand parsed;

    for (const auto* invalid : {"", "3x", "1q", "x junk", "11x", "aA", "Q", "1 x"})
        expect(!parser.parse(invalid, parsed), "reject malformed command");
    expect(parser.parse(" q ", parsed) && parsed.kind == CommandKind::quit, "trimmed q");
    expect(parser.parse("a", parsed) && parsed.delta.x == 1 && parsed.delta.y == -1, "a meaning");
}

void testBoardProperties() {
    auto board = makeBoard({0, 0}, {4, 3});
    BoardRenderer renderer;
    std::mt19937 random(12345);
    const std::array movements{Delta{1, 0}, Delta{-1, 0}, Delta{0, 1}, Delta{0, -1}};
    const std::array changes{Delta{1, 1}, Delta{-1, -1}, Delta{1, -1}, Delta{-1, 1}, Delta{1, 0}, Delta{0, -1}};

    for (int step = 0; step < 1500; ++step) {
        const auto before = board.rectangles();
        const auto id = static_cast<std::size_t>(random() % 2);
        ChangeError error;

        if (random() % 2 == 0)
            error = board.moveRectangle(id, movements[random() % movements.size()]);
        else
            error = board.resizeRectangle(id, changes[random() % changes.size()]);

        if (error != ChangeError::none)
            expect(board.rectangles() == before, "random rejection atomic");
        expect(board.rectangles()[1 - id] == before[1 - id], "other shape unchanged");

        const auto snapshot = board.rectangles();
        const BoardFrame frame = renderer.project(board);

        expect(board.rectangles() == snapshot, "projection does not mutate geometry");

        std::array<int, 2> occupied{};

        for (const Cell cell : frame.cells) {
            if (cell == Cell::first || cell == Cell::overlap)
                ++occupied[0];

            if (cell == Cell::second || cell == Cell::overlap)
                ++occupied[1];
        }

        for (std::size_t shape = 0; shape < 2; ++shape) {
            const auto size = board.rectangles()[shape].size();

            expect(size.width >= 1 && size.height >= 1 && size.width <= board.size() && size.height <= board.size(),
                   "size invariant");
            expect(occupied[shape] == board.rectangles()[shape].area(), "displayed cells equal real area");
        }
    }
}

const std::string base = "v 0 0 0\nv 1 0 0\nv 0 1 0\nvt 0 0\nvt 1 0\nvt 0 1\n";

struct MeshCase {
    std::string name;
    std::string input;
    bool valid;
    std::size_t faceCount;
};

void testMesh() {
    const std::vector<MeshCase> cases{
        {"valid vertex", base + "f 1 2 3\n", true, 1},
        {"valid textured", base + "f 1/1 2/2 3/3\n", true, 1},
        {"comments BOM", "\xef\xbb\xbf# hello\r\n" + base + " f\t1/1\t2/2\t3/3 # face\r\n", true, 1},
        {"forward reference", "f 1 2 3\n" + base, true, 1},
        {"reused coordinates", base + "v 0 0 0\nvt 0 0\nf 1/1 2/2 3/3\nf 4/4 2/2 3/3\n", true, 2},
        {"equal UV components allowed", base + "f 1/1 2/2 3/3\n", true, 1},
        {"distinct collinear UV allowed", base + "vt .5 0\nf 1/1 2/2 3/4\n", true, 1},
        {"duplicate vertex index", base + "f 1 1 3\n", false, 0},
        {"duplicate vertex value", base + "v 0 0 0\nf 1 4 3\n", false, 0},
        {"duplicate UV index", base + "f 1/1 2/1 3/3\n", false, 0},
        {"duplicate UV value", base + "vt 0 0\nf 1/1 2/4 3/3\n", false, 0},
        {"two corners", base + "f 1 2\n", false, 0},
        {"four corners", base + "f 1 2 3 1\n", false, 0},
        {"no corners", base + "f\n", false, 0},
        {"zero index", base + "f 0 2 3\n", false, 0},
        {"negative index", base + "f -1 2 3\n", false, 0},
        {"signed index", base + "f +1 2 3\n", false, 0},
        {"decimal index", base + "f 1.0 2 3\n", false, 0},
        {"huge index", base + "f 999999999999999999999999999 2 3\n", false, 0},
        {"out of range vertex", base + "f 4 2 3\n", false, 0},
        {"out of range texture", base + "f 1/4 2/2 3/3\n", false, 0},
        {"missing texture", base + "f 1/ 2/2 3/3\n", false, 0},
        {"extra slash", base + "f 1//1 2/2 3/3\n", false, 0},
        {"mixed syntax", base + "f 1/1 2 3\n", false, 0},
        {"trailing index garbage", base + "f 1 2 3x\n", false, 0},
        {"upper vertex bound", "v 1.1 0 0\n" + base + "f 1 3 4\n", false, 0},
        {"lower vertex bound", "v -1.1 0 0\n" + base + "f 1 3 4\n", false, 0},
        {"upper UV bound", base + "vt 1.01 0\nf 1/4 2/2 3/3\n", false, 0},
        {"lower UV bound", base + "vt -.01 0\nf 1/4 2/2 3/3\n", false, 0},
        {"bad declaration slot", "v garbage 0 0\n" + base + "f 2 3 4\n", false, 1},
        {"bad declaration reference", "v garbage 0 0\n" + base + "f 1 3 4\n", false, 0},
        {"bad UV slot", "vt garbage 0\n" + base + "f 1/2 2/3 3/4\n", false, 1},
        {"multiple failed faces preserve numbering", base + "f 1 2\nf 1 2 3\nf 1 9 3\n", false, 1},
        {"overflow number", "v 1e999 0 0\n" + base + "f 2 3 4\n", false, 1},
        {"nan", "v nan 0 0\n" + base + "f 2 3 4\n", false, 1},
        {"infinity", "v inf 0 0\n" + base + "f 2 3 4\n", false, 1},
        {"hex rejected", "v 0x1 0 0\n" + base + "f 2 3 4\n", false, 1},
        {"double sign", "v +-1 0 0\n" + base + "f 2 3 4\n", false, 1},
        {"unknown record", base + "vn 0 0 1\nf 1 2 3\n", false, 1},
        {"missing component", "v 0 0\n" + base + "f 2 3 4\n", false, 1},
        {"extra component", "vt 0 0 0\n" + base + "f 1 2 3\n", false, 1},
        {"scientific plus decimal", "v +0.0 0 0\nv 1e0 0 0\nv 0 .5 0\nf 1 2 3\n", true, 1},
        {"exact bounds", "v -1 -1 -1\nv 1 -1 -1\nv -1 1 1\nvt 0 0\nvt 1 0\nvt 1 1\nf 1/1 2/2 3/3\n", true, 1},
        {"collinear decimal", "v .1 .3 0\nv .2 .6 0\nv .3 .9 0\nf 1 2 3\n", false, 0},
        {"tiny valid triangle", "v 0 0 0\nv 1e-200 0 0\nv 0 1e-200 0\nf 1 2 3\n", true, 1},
        {"thin valid triangle", "v 0 0 0\nv 1 0 0\nv 1 1e-8 0\nf 1 2 3\n", true, 1},
        {"essentially collinear", "v 0 0 0\nv 1 0 0\nv 1 1e-12 0\nf 1 2 3\n", false, 0},
        {"empty", "", false, 0},
        {"comments only", "# nothing\n", false, 0},
        {"no faces", base, false, 0}};

    for (const auto& test : cases) {
        std::istringstream input(test.input);
        const auto parsed = MeshParser{}.parse(input);
        const auto validated = MeshValidator{}.validate(parsed.mesh);

        expect(succeeded(parsed, validated) == test.valid, test.name + " validity");
        expect(validated.faces.size() == test.faceCount, test.name + " face count");

        std::ostringstream output;
        // Only the test captures cout; application code uses cin/cout directly.
        std::streamsize previousPrecision = std::cout.precision(4);
        std::streambuf* previousBuffer = std::cout.rdbuf(output.rdbuf());

        MeshReporter{}.print(parsed, validated);

        bool restoredPrecision = std::cout.precision() == 4;

        std::cout.rdbuf(previousBuffer);
        std::cout.precision(previousPrecision);
        expect(restoredPrecision, "reporter restores precision");
        expect((output.str().find("Validation succeeded") != std::string::npos) == test.valid, test.name + " summary");

        if (test.name == "multiple failed faces preserve numbering") {
            expect(validated.faces[0].number == 2, "original face number retained");
            expect(output.str().find("Face 1:") != std::string::npos &&
                       output.str().find("Face 3:") != std::string::npos,
                   "both invalid faces diagnosed");
        }
    }

    std::istringstream badInput;

    badInput.setstate(std::ios::badbit);
    expect(!MeshParser{}.parse(badInput).diagnostics.empty(), "input failure diagnosed");
    std::cout << "Mesh cases: " << cases.size() << '\n';
}

}

int main() {
    try {
        testBoardTransactions();
        testCommands();
        testBoardProperties();
        testMesh();
        std::cout << "PASS " << checks << " assertions; 1500 deterministic board operations.\n";

        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL after " << checks << " checks: " << error.what() << '\n';

        return 1;
    }
}
