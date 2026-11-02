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
        config.title="실습 22 - 원근 / XZ / XY";
        config.width=1200;
        config.height=800;
        return app.run(config);
    } catch(const std::exception& error) {
        std::cerr<<"실습 22 실패: "<<error.what()<<'\n';
    } catch(...) {
        std::cerr<<"실습 22 실패: 알 수 없는 예외.\n";
    }
    return 1;
}
