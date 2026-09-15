#pragma once
#include "Board.h"
#include <string>

namespace warmup {
enum class CommandKind { move, resize, area, resizeBoard, reset, quit };
struct BoardCommand {
    CommandKind kind = CommandKind::quit;
    std::size_t rectangle = 0;
    Delta delta{};
};

class BoardCommandParser {
public:
    bool parse(const std::string& text, BoardCommand& command) const;
    std::string help() const;
};
}
