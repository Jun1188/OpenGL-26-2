#include "Assembly.h"

namespace assembly {
std::optional<bool> customCommand(Assembly& scene, const Tokens& args, std::ostream&) {
    // USER_COMMAND_BEGIN
    if (args[0] == "nudge") {
        require(args.size() == 4, "nudge <대상> <축> <거리>");
        scene.move(args[1], axis(args[2]) * float(number(args[3])));
        return true;
    }
    // USER_COMMAND_END
    return std::nullopt;
}
} // namespace assembly
