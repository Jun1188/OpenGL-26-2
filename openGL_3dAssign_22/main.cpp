#include "Console.h"
#include <exception>
#include <iostream>

int main() {
    try {
        graphics::Application app;
        Console practice(app);
        app.setInitHandler([&](graphics::Application&){return practice.initScene();});
        app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
            practice.onKey(key,action,mods);
        });
        app.setUpdateHandler([&](graphics::Application&,float dt){practice.onUpdate(dt);});
        app.setFocusHandler([&](graphics::Application&,bool focused){practice.onFocus(focused);});
        app.setRenderHandler([&](graphics::Application&){return practice.onRender();});
        graphics::ApplicationConfig config;
        config.title="OpenGL Practice 22 - Perspective / XZ / XY";
        config.width=1200;
        config.height=800;
        return app.run(config);
    } catch(const std::exception& error) {
        std::cerr<<"Practice 22 failed: "<<error.what()<<'\n';
    } catch(...) {
        std::cerr<<"Practice 22 failed: unknown exception.\n";
    }
    return 1;
}
