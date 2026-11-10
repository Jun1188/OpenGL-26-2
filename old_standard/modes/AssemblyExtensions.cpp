#include "Assembly.h"

#include "AssemblyExamples.h"

namespace assembly {
std::optional<bool> customCommand(Assembly& scene, const Tokens& args, std::ostream&) {
    // USER_COMMAND_BEGIN
    if (args[0] == "build") {
        require(args.size() == 3, "build <tank/truck/person> <이름>");
        if (args[1] == "tank") buildTankExample(scene, args[2]);
        else if (args[1] == "truck") buildTruckExample(scene, args[2]);
        else if (args[1] == "person") buildPersonExample(scene, args[2]);
        else require(false, "예제는 tank/truck/person입니다. guide patterns");
        return true;
    }
    if (args[0] == "nudge") {
        require(args.size() == 4, "nudge <대상> <축> <거리>");
        scene.move(args[1], axis(args[2]) * float(number(args[3])));
        return true;
    }
    // USER_COMMAND_END
    return std::nullopt;
}
} // namespace assembly
