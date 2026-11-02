#include "BoardRenderer.h"
#include "Console.h"
#include <iostream>
#include <stdexcept>

namespace warmup {
namespace {
int wrap(int coordinate, int size) {
    return (coordinate % size + size) % size;
}

// At most two visible intervals exist on one axis.
struct AxisSegments {
    int begin[2] = {};
    int end[2] = {};
    int count = 1;
};

AxisSegments splitAxis(int logicalStart, int length, int boardSize) {
    int start = wrap(logicalStart, boardSize);
    int finish = wrap(logicalStart + length - 1, boardSize);
    AxisSegments segments;

    segments.begin[0] = start;

    if (start <= finish) {
        segments.end[0] = finish;
    } else {
        // The displayed end is before the start: split at the board edge.
        segments.end[0] = boardSize - 1;
        segments.begin[1] = 0;
        segments.end[1] = finish;
        segments.count = 2;
    }

    return segments;
}

Cell mark(Cell current, Cell owner) {
    if (current == Cell::empty || current == owner)
        return owner;

    return Cell::overlap;
}
}

Cell BoardFrame::at(int x, int y) const {
    if (x < 0 || y < 0 || x >= size || y >= size)
        throw std::out_of_range("BoardFrame coordinates");

    return cells.at(static_cast<std::size_t>(y * size + x));
}

BoardFrame BoardRenderer::project(const Board& board) const {
    BoardFrame frame{board.size(),
                     std::vector<Cell>(static_cast<std::size_t>(board.size() * board.size()), Cell::empty)};
    const Cell owners[2] = {Cell::first, Cell::second};

    for (std::size_t id = 0; id < board.rectangles().size(); ++id) {

        const Rectangle& rectangle = board.rectangles()[id];
        const Point origin = rectangle.position();
        const Size dimensions = rectangle.size();

        AxisSegments columns = splitAxis(origin.x, dimensions.width, frame.size);
        AxisSegments rows = splitAxis(origin.y, dimensions.height, frame.size);

        for (int row = 0; row < rows.count; ++row) {
            for (int column = 0; column < columns.count; ++column) {
                for (int y = rows.begin[row]; y <= rows.end[row]; ++y) {
                    for (int x = columns.begin[column]; x <= columns.end[column]; ++x) {
                        Cell& cell = frame.cells[static_cast<std::size_t>(y * frame.size + x)];

                        cell = mark(cell, owners[id]);
                    }
                }
            }
        }
    }

    return frame;
}

void BoardRenderer::draw(const Board& board, Console* console) const {
    const BoardFrame frame = project(board);

    if (console)
        console->clear();
    std::cout << "Board " << frame.size << 'x' << frame.size << " (0: rectangle 1, x: rectangle 2, #: overlap)\n";

    const char symbols[4] = {'.', '0', 'x', '#'};

    for (int y = 0; y < frame.size; ++y) {
        for (int x = 0; x < frame.size; ++x) {
            const Cell cell = frame.at(x, y);

            if (console)
                console->setHighlight(cell == Cell::overlap);
            std::cout << symbols[static_cast<std::size_t>(cell)] << ' ';
        }

        if (console)
            console->setHighlight(false);
        std::cout << '\n';
    }
}
}
