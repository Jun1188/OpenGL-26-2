#include "../Framework.h"
#include <array>
#include <iostream>

int main() {
    graphics::Application app;
    std::array<graphics::EntityId,3> bodies{};
    app.setInitHandler([&](graphics::Application& app) {
        auto& scene=app.getScene();
        const auto sun=scene.addCuboid({.8f,.8f,.8f},{0,0,0});
        const auto earth=scene.addCuboid({.4f,.4f,.4f},{2,0,0});
        const auto moon=scene.addCuboid({.2f,.2f,.2f},{.8f,0,0});
        if(!sun||!earth||!moon)return false;
        bodies={*sun,*earth,*moon};
        if(!scene.setParent(*earth,*sun)||!scene.setParent(*moon,*earth))return false;
        auto image=std::make_shared<graphics::TextureImage>();
        image->width=2;image->height=2;
        image->rgba={255,255,255,255, 20,20,20,255,
                     20,20,20,255, 255,255,255,255};
        graphics::Material material;material.texture=image;material.useTexture=true;
        graphics::UVOptions uv;uv.mapping=graphics::UVMapping::Box;uv.scale={2,2};
        for(auto id:bodies) {
            const auto count=scene.getComponent(id)->polyhedron.faces.size();
            for(std::size_t mesh=0;mesh<count;++mesh) {
                if(!scene.generateTexcoords(id,mesh,uv)||!scene.setMaterial(id,mesh,material))return false;
            }
        }
        app.getCamera().position={0,4,8};
        app.getCamera().target={0,0,0};
        return true;
    });
    app.setUpdateHandler([&](graphics::Application& app,float dt) {
        auto& scene=app.getScene();
        for(std::size_t i=0;i<2;++i) {
            auto transform=scene.getComponent(bodies[i])->transform;
            transform.rotationDegrees.y=std::fmod(transform.rotationDegrees.y+dt*(i?80.0f:20.0f),360.0f);
            if(!scene.setTransform(bodies[i],transform))app.requestClose();
        }
    });
    app.setMouseHandler([](graphics::Application& app,const graphics::MouseEvent& event) {
        if(event.button!=GLFW_MOUSE_BUTTON_LEFT||event.action!=GLFW_PRESS)return;
        if(const auto hit=app.pickAt(event.windowX,event.windowY))
            std::cout<<"entity="<<hit->entityId<<" UV="<<hit->uv.x<<","<<hit->uv.y<<'\n';
    });
    app.setKeyHandler([](graphics::Application& app,int key,int action) {
        if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)app.requestClose();
    });
    graphics::ApplicationConfig config;config.features.textures=true;
    config.title="Hierarchy and UV mapping";
    return app.run(config);
}
