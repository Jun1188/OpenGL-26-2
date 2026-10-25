#include "Console.h"
int main() {
    graphics::Application app;
    Console practice(app);

    app.setInitHandler([&](graphics::Application&){
        return practice.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){
        practice.onKey(key,action,mods);
    });
    app.setUpdateHandler([&](graphics::Application&,float dt){
        practice.onUpdate(dt);
    });
    app.setFocusHandler([&](graphics::Application&,bool focused){
        practice.onFocus(focused);
    });

    graphics::ApplicationConfig config;
    config.title="OpenGL Practice 17";
    return app.run(config);
}
