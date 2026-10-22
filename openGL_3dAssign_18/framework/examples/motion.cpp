#include "../Framework.h"
#include <iostream>

int main() {
    using namespace graphics;
    Application app;
    std::optional<MotionId> pathId, spinId, globalId;
    std::optional<LayerId> mirrorLayer, normalSpinLayer, insertedLayer;
    bool paused = false, spinEnabled = true, globalEnabled = false;
    int reflection = 1;

    const auto choosePath = [&](int kind) {
        PathMotion path;
        path.durationSeconds = 6;
        const glm::vec3 center(1.7f, 0, 0);
        if (kind == 1) path.path = PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 2) path.path = PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
        else if (kind == 3)
            path.path = PathSystem::polygon({{1, -.5f, 0}, {2.4f, -.4f, 0},
                {2.1f, .6f, 0}, {1.4f, .8f, 0}, {.9f, .2f, 0}});
        else if (kind == 4) {
            path.path = [center](double u) {
                const double angle = u * 6.283185307179586;
                return center + glm::vec3(float(.7 * std::sin(2 * angle)),
                    float(.6 * std::sin(angle)), float(.3 * std::cos(angle)));
            };
            path.interpolation = PathSystem::smoothStep;
        } else if (kind == 5) path.path = PathSystem::circle(center, .65f, {0, 0, 1});
        else {
            path.path = PathSystem::polyline({{1, -.5f, 0}, {2.4f, -.5f, 0}, {2, .6f, 0}});
            path.playback = Playback::PingPong;
        }
        Motion motion;
        auto& motions = app.getScene().getMotions();
        return pathId && motion.append(Motion::followPath(path))
            && motions.setMotion(*pathId, motion) && motions.seekMotion(*pathId, 0);
    };

    app.setInitHandler([&](Application& current) {
        auto& scene = current.getScene();
        auto& motions = scene.getMotions();
        const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
        if (!first || !second) return false;
        if (!scene.setTint(*first, {1, .35f, .2f}) || !scene.setTint(*second, {.2f, .7f, 1})) return false;
        pathId = motions.createMotion(Motion{});
        RotationMotion spin;
        spin.degreesPerSecond = 90;
        spinId = motions.createSpin(spin);
        RotationMotion global;
        global.degreesPerSecond = 15;
        globalId = motions.createOrbit(global);
        if (!pathId || !spinId || !globalId || !choosePath(1)) return false;
        if (!motions.setMotionEnabled(*globalId, false)
            || !motions.bindMotion(MotionSystem::rootLayer(), *globalId)) return false;

        const auto normalPath = motions.pushLayer(MotionSystem::rootLayer());
        const auto reflectionMatrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
        if (!normalPath || !reflectionMatrix) return false;
        LayerConfig mirror;
        mirror.transform = *reflectionMatrix;
        mirrorLayer = motions.pushLayer(MotionSystem::rootLayer(), mirror);
        if (!mirrorLayer) return false;
        const auto mirroredPath = motions.pushLayer(*mirrorLayer);
        normalSpinLayer = motions.pushLayer(*normalPath);
        if (!mirroredPath || !normalSpinLayer) return false;
        const auto mirroredSpin = motions.pushLayer(*mirroredPath);
        if (!mirroredSpin || !motions.bindMotion(*normalPath, *pathId)
            || !motions.bindMotion(*mirroredPath, *pathId)
            || !motions.bindMotion(*normalSpinLayer, *spinId)
            || !motions.bindMotion(*mirroredSpin, *spinId)
            || !scene.bindEntity(*first, *normalSpinLayer)
            || !scene.bindEntity(*second, *mirroredSpin)) return false;
        if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
            || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
            || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) return false;
        current.getCamera().position = {5, 3, 8};
        std::cout << "1 타원, 2 사각형, 3 다각형, 4 사용자 경로+보간, 5 원, 6 왕복 꺾은선\n"
                  << "Space 시간 정지/재개, S 자전 표시, G 전체 공전, M 반사 변경, I 레이어 삽입/삭제\n"
                  << "M: 없음 -> YZ평면 반사 -> 점대칭 -> Y직선 대칭 순환. 클릭: 객체 ID. Esc 종료\n";
        return true;
    });
    app.setKeyHandler([&](Application& current, int key, int action) {
        if (action != GLFW_PRESS) return;
        auto& motions = current.getScene().getMotions();
        bool ok = true;
        if (key == GLFW_KEY_ESCAPE) current.requestClose();
        else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) ok = choosePath(key - GLFW_KEY_1 + 1);
        else if (key == GLFW_KEY_SPACE) {
            paused = !paused;
            ok = motions.setMotionPlaying(*pathId, !paused)
                && motions.setMotionPlaying(*spinId, !paused)
                && motions.setMotionPlaying(*globalId, !paused);
        } else if (key == GLFW_KEY_S) {
            spinEnabled = !spinEnabled;
            ok = motions.setMotionEnabled(*spinId, spinEnabled);
        } else if (key == GLFW_KEY_G) {
            globalEnabled = !globalEnabled;
            ok = motions.setMotionEnabled(*globalId, globalEnabled);
        } else if (key == GLFW_KEY_M) {
            reflection = (reflection + 1) % 4;
            std::optional<glm::mat4> matrix = glm::mat4(1);
            if (reflection == 1) matrix = TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
            if (reflection == 2) matrix = TransformSystem::reflectPoint({0, 0, 0});
            if (reflection == 3) matrix = TransformSystem::reflectLine(AxisLine{});
            LayerConfig config;
            if (!matrix) ok = false;
            else { config.transform = *matrix; ok = motions.setLayer(*mirrorLayer, config); }
        } else if (key == GLFW_KEY_I) {
            if (insertedLayer) {
                ok = motions.removeLayer(*insertedLayer);
                if (ok) insertedLayer.reset();
            } else {
                LayerConfig config;
                config.space = TransformSpace::Local;
                const auto scale = TransformSystem::scaling(glm::vec3(1.5f));
                if (!scale) ok = false;
                else {
                    config.transform = *scale;
                    insertedLayer = motions.insertLayer(*normalSpinLayer, config);
                    ok = insertedLayer.has_value();
                }
            }
        }
        if (!ok) current.requestClose();
    });
    app.setMouseHandler([](Application& current, const MouseEvent& event) {
        if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
        const auto hit = current.pickAt(event.windowX, event.windowY);
        if (hit) std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    });
    ApplicationConfig config;
    config.title = "Motion and layer tree";
    return app.run(config);
}
