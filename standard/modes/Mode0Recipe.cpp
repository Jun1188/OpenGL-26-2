#include "Assembly.h"
namespace assembly {
bool defaultRecipe(Assembly& scene, std::ostream& out) {
    scene.clear();
    // USER_RECIPE_BEGIN
    const char* commands[] = {
        "camera pose 8 6 10 0 0 0 0 1 0",
        "add box box1 0 0 0",
        "select box1"
    };
    // USER_RECIPE_END
    for (const auto* command : commands)
        if (!scene.execute(command, out)) return false;
    return true;
}
} // namespace assembly
