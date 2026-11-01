#include "Console.h"
#include <iostream>
int main() {
    try {
        graphics::Application app;Console practice(app);
        app.setInitHandler([&](graphics::Application&){return practice.initScene();});
        app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){practice.onKey(key,action,mods);});
        app.setUpdateHandler([&](graphics::Application&,float dt){practice.onUpdate(dt);});
        graphics::ApplicationConfig config;config.title="Practice23 - Robot";config.width=1100;config.height=800;
        std::cout<<"23: WASD direction, +/- speed, J jump, Space pause walk, O door, I reset, x/X z/Z camera move, y/Y orbit, Q quit\n";
        return app.run(config);
    } catch(const std::exception& error) {std::cerr<<error.what()<<'\n';return 1;}
}
