#include "BoardCommand.h"
#include <vector>
#include <sstream>

namespace warmup {
namespace {
struct Binding {
    char key;
    CommandKind kind;
    Delta delta;
    std::string description;
};

const std::vector<Binding> bindings{Binding{'x', CommandKind::move, {1, 0}, "right"},
                                    Binding{'X', CommandKind::move, {-1, 0}, "left"},
                                    Binding{'y', CommandKind::move, {0, 1}, "down"},
                                    Binding{'Y', CommandKind::move, {0, -1}, "up"},
                                    Binding{'s', CommandKind::resize, {-1, -1}, "shrink width and height"},
                                    Binding{'S', CommandKind::resize, {1, 1}, "grow width and height"},
                                    Binding{'i', CommandKind::resize, {1, 0}, "grow width"},
                                    Binding{'I', CommandKind::resize, {-1, 0}, "shrink width"},
                                    Binding{'j', CommandKind::resize, {0, 1}, "grow height"},
                                    Binding{'J', CommandKind::resize, {0, -1}, "shrink height"},
                                    Binding{'a', CommandKind::resize, {1, -1}, "grow width, shrink height"},
                                    Binding{'A', CommandKind::resize, {-1, 1}, "shrink width, grow height"},
                                    Binding{'b', CommandKind::area, {}, "show area"}};
}

bool BoardCommandParser::parse(const std::string& text, BoardCommand& command) const {
    std::istringstream input(text);
    std::string token, extra;

    if (!(input >> token) || input >> extra)
        return false;

    BoardCommand candidate;

    if (token == "q")
        candidate.kind = CommandKind::quit;
    else if (token == "r")
        candidate.kind = CommandKind::reset;
    else if (token == "c" || token == "d") {
        candidate.kind = CommandKind::resizeBoard;
        candidate.delta.x = token == "c" ? 1 : -1;
    } else {

        if (token.size() == 2 && (token[0] == '1' || token[0] == '2')) {
            candidate.rectangle = token[0] - '1';
            token.erase(0, 1);
        }

        if (token.size() != 1)
            return false;

        bool found = false;

        for (const Binding& binding : bindings) {
            if (binding.key == token[0]) {
                candidate.kind = binding.kind;
                candidate.delta = binding.delta;
                found = true;
                break;
            }
        }

        if (!found)
            return false;
    }
    command = candidate;

    return true;
}

std::string BoardCommandParser::help() const {
    std::ostringstream output;

    output << "Prefix EVERY shape command with 1 or 2 (1x, 2y, 1S, 2a, 1b). Default: 1.\n";

    for (std::size_t i = 0; i < bindings.size(); ++i) {
        output << bindings[i].key << ": " << bindings[i].description;
        output << ((i + 1) % 4 == 0 || i + 1 == bindings.size() ? "\n" : " | ");
    }
    output << "c/d: grow/shrink board | r: reset | q: quit\n"
           << "Compound resize commands are all-or-nothing.\n";

    return output.str();
}
}
