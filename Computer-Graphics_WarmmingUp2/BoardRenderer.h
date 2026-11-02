#pragma once
#include "Board.h"
#include <vector>

namespace warmup {
class Console;
enum class Cell : unsigned char { empty, first, second, overlap };

struct BoardFrame {
    int size = 0;
    std::vector<Cell> cells;
    Cell at(int x, int y) const;
};

class BoardRenderer {
public:
    BoardFrame project(const Board& board) const;

    void draw(const Board& board, Console* console = nullptr) const;
};
}
