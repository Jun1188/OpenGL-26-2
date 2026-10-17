#include "../Framework.h"
#include <iostream>

// Usage, with the project root as working directory:
// assets.exe primitive framework/examples/data/cube.txt
// assets.exe primitive framework/examples/data/cube.txt textures/example.png
// assets.exe model models/example.obj
// assets.exe model models/example.gltf textures/override.png
int main(int argc,char** argv) {
    if(argc<3){std::cerr<<"Use: assets.exe primitive|model relative-model-path [relative-texture-path]\n";return 1;}
    const std::string mode=argv[1];
    if(mode!="primitive"&&mode!="model"){std::cerr<<"Unknown mode\n";return 1;}
    const auto path=std::filesystem::u8path(argv[2]);
    const auto texturePath=argc>3?std::filesystem::u8path(argv[3]):std::filesystem::path{};
    graphics::ApplicationConfig config;
    config.projectRoot=".";
    config.features.models=mode=="model";
    config.features.textures=!texturePath.empty();
    // To load model-referenced/embedded textures without an override, opt in here:
    // config.features.textures=true;
    graphics::Application app;
    app.setInitHandler([&](graphics::Application& app) {
        graphics::LoadOptions options;
        options.outline=mode=="primitive"?graphics::OutlineMode::FeatureEdges:graphics::OutlineMode::None;
        options.texturePath=texturePath;
        const auto loaded=mode=="model"?app.getAssets().loadModel(path,options)
                                      :app.getAssets().loadPrimitive(path,options);
        if(!loaded){std::cerr<<loaded.error<<'\n';return false;}
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
