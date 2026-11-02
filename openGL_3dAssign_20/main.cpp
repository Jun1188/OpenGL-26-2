#include "Console.h"
#include <exception>
#include <iostream>

int main() {
    try {
        graphics::Application app;
        Console conveyor(app);

        app.setInitHandler([&](graphics::Application&) {
            return conveyor.initScene();
        });
        app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
            conveyor.onKey(key, action, mods);
        });
        app.setFocusHandler([&](graphics::Application&, bool focused) {
            conveyor.onFocus(focused);
        });
        app.setUpdateHandler([&](graphics::Application&, float dt) {
            conveyor.onUpdate(dt);
        });

        graphics::ApplicationConfig config;

        config.title = "Practice 20 - Parcel conveyor";
        config.width = 1100;
        config.height = 800;
        config.projectRoot = ".";
        config.runtimeDirectory = "framework";

        const int result = app.run(config);

        if (result != 0)
            std::cerr << "Practice 20 failed. Read the preceding framework diagnostic.\n";

        return result;
    } catch (const std::exception& error) {
        std::cerr << "Practice 20: " << error.what() << '\n';

        return 1;
    } catch (...) {
        std::cerr << "Practice 20: unknown exception.\n";

        return 1;
    }
}
