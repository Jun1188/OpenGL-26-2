#include "Assembly.h"

namespace assembly {
bool defaultRecipe(Assembly& scene, std::ostream& out) {
    scene.clear();
    // USER_RECIPE_BEGIN
    const char* commands[] = {
        "camera pose 16 11 20 0 1 1 0 1 0",
        "camera fov 55",
        "add grid floor 0 0 0",
        "build person hero",
        "move hero -2 0 0",
        "path stroll hero 5 8",
        "add swing ride 2 0 0",
        "build tank rover",
        "move rover 5 0 0",
        "color rover 0.3 0.6 0.8",
        "add room lab -6 0 0",
        "add box cargo -6 1 0 0.5 0.5 0.5",
        "physics falling cargo lab",
        "add walker robot -5.5 0 -0.5",
        "walk walking robot lab 0.7",
        "obstacle walking cargo",
        "bind RIGHT hold \"move hero 1 0 0\"",
        "bind LEFT hold \"move hero -1 0 0\"",
        "bind J press \"jump walking 5.5\"",
        "bind F press \"visible lab 5 off\"",
        "bind G press \"visible lab 5 on\"",
        "select hero"
    };
    // USER_RECIPE_END
    for (const auto* command : commands)
        if (!scene.execute(command, out)) return false;
    return true;
}
} // namespace assembly
