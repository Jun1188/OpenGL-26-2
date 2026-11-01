#include "../Framework.h"
#include <iostream>

// assets.exe obj framework/examples/data/cube.obj
// assets.exe obj models/example.obj textures/override.png
// The old primitive/model mode names remain aliases of obj.
int main(int argc,char** argv) {
    if(argc<3){std::cerr<<"Use: assets.exe obj|primitive|model relative-path [relative-texture-path]\n";return 1;}
    const std::string mode=argv[1];
    if(mode!="obj"&&mode!="primitive"&&mode!="model"){std::cerr<<"Unknown mode\n";return 1;}
    const auto path=std::filesystem::u8path(argv[2]);
    const auto texturePath=argc>3?std::filesystem::u8path(argv[3]):std::filesystem::path{};
    graphics::ApplicationConfig config;
    config.projectRoot=".";
    config.features.textures=!texturePath.empty();
    graphics::Application app;
    app.setInitHandler([&](graphics::Application& app) {
        graphics::LoadOptions options;
        options.outline=graphics::OutlineMode::FeatureEdges;
        options.texturePath=texturePath;
        const auto loaded=app.getAssets().loadObj(path,options);
        if(!loaded) return false; // loadObj already printed the error and format guidance.
        for(const auto& warning:loaded.warnings) std::cerr<<warning<<'\n';
        return app.getScene().addModel(*loaded.model,graphics::Transform3D{},
                                      loaded.pivotMode,loaded.localBasis).has_value();
    });
    app.setMouseHandler([](graphics::Application& app,const graphics::MouseEvent& event) {
        if(event.button==GLFW_MOUSE_BUTTON_LEFT&&event.action==GLFW_PRESS)
            if(const auto hit=app.pickAtCursor())
                std::cout<<"entity="<<hit->entityId<<" mesh="<<hit->faceIndex<<" triangle="<<hit->triangleIndex<<'\n';
    });
    app.setKeyHandler([](graphics::Application& app,int key,int action){
        if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)app.requestClose();
    });
    return app.run(config);
}
