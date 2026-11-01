#include "Console.h"
#include <iostream>
int main() {
    try {
        graphics::Application app;Console practice(app);
        app.setInitHandler([&](graphics::Application&){return practice.initScene();});
        app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){practice.onKey(key,action,mods);});
        app.setUpdateHandler([&](graphics::Application&,float dt){practice.onUpdate(dt);});
        graphics::ApplicationConfig config;config.title="실습 23 - 로봇";config.width=1100;config.height=800;
        std::cout<<"실습 23: WASD 방향, +/- 속도, J 점프, Space 보행 정지/재개, O 벽 열기/같은 벽 복원, I 초기화, x/X z/Z 카메라 이동, y/Y 공전, Q 종료\n";
        return app.run(config);
    } catch(const std::exception& error) {std::cerr<<"실습 23 실패: "<<error.what()<<'\n';return 1;}
}
