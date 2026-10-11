#include "../Framework.h"
#include <iostream>

int main() {
    graphics::Application app;
    app.setInitHandler([](graphics::Application& app) {
        auto& scene=app.getScene();
        const auto id=scene.addCuboid({1,1,1},{0,0,0});
        if(!id)return false;
        graphics::Transform3D parent;
        parent.rotationDegrees={15,25,0};parent.scale={1.2f,.8f,1};
        if(!scene.setTransform(*id,parent))return false;
        for(std::size_t face=0;face<6;++face)
            if(!scene.setOutline(*id,face,graphics::OutlineMode::FeatureEdges))return false;
        return true;
    });
    app.setMouseHandler([](graphics::Application& app,const graphics::MouseEvent& event) {
        if(event.button!=GLFW_MOUSE_BUTTON_LEFT||event.action!=GLFW_PRESS)return;
        const auto hit=app.pickAt(event.windowX,event.windowY);
        if(!hit)return;
        auto& scene=app.getScene();const auto* face=scene.getMesh(hit->entityId,hit->faceIndex);
        graphics::Transform3D child;
        for(const auto& vertex:face->localVertices)child.localBasis+=vertex;
        child.localBasis/=float(face->localVertices.size());
        child.basis=face->normal*.25f;child.rotationDegrees.z=20;
        scene.splitFace(hit->entityId,hit->faceIndex);
        scene.setFaceSRT(hit->entityId,hit->faceIndex,child);
        std::cout<<"entity="<<hit->entityId<<" face="<<hit->faceIndex<<'\n';
    });
    app.setKeyHandler([](graphics::Application& app,int key,int action) {
        if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)app.requestClose();
    });
    return app.run();
}
