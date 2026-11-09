#pragma once
#include "Patterns.h"

namespace graphics {
// Value-configured operations. Scene and camera references are non-owning.
class SceneActions {
public:
    static MotionAction transform(Scene& scene, EntityId id, Transform3D value) {
        return [&scene, id, value](double, const MotionSample&) {
            return scene.setTransform(id, value);
        };
    }


    static MotionAction position(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            return scene.setPosition(id, value);
        };
    }


    static MotionAction scale(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component)
                return false;
            auto transform = component->transform;
            transform.scale = value;
            return scene.setTransform(id, transform);
        };
    }


    static MotionAction rotation(Scene& scene, EntityId id, glm::mat3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component)
                return false;
            auto transform = component->transform;
            transform.rotation = value;
            return scene.setTransform(id, transform);
        };
    }


    static MotionAction tint(Scene& scene, EntityId id, GlColor value) {
        return [&scene, id, value](double, const MotionSample&) {
            return scene.setTint(id, value);
        };
    }


    static MotionAction pivot(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            return scene.setLocalBasis(id, PivotMode::Custom, value);
        };
    }


    static MotionAction visible(Scene& scene, EntityId id, bool value) {
        return [&scene, id, value](double, const MotionSample&) {
            return scene.setVisible(id, value);
        };
    }


    static MotionAction faceVisible(Scene& scene, EntityId id, std::size_t face, bool value) {
        return [&scene, id, face, value](double, const MotionSample&) {
            return scene.setFaceVisible(id, face, value);
        };
    }


    static MotionAction faceRotation(Scene& scene, EntityId id, std::size_t face, FaceRotationConfig config) {
        return [&scene, id, face, config](double, const MotionSample&) {
            return PatternSystem::rotateFace(scene, id, face, config);
        };
    }


    static MotionAction faceMode(Scene& scene, EntityId id, std::size_t face, FaceTransformMode mode,
                                 bool preserveWorld = true) {
        return [&scene, id, face, mode, preserveWorld](double, const MotionSample&) {
            return scene.setFaceTransformMode(id, face, mode, preserveWorld);
        };
    }


    static MotionAction layer(Scene& scene, EntityId id, std::optional<LayerId> value) {
        return [&scene, id, value](double, const MotionSample&) {
            return value ? scene.bindEntity(id, *value) : scene.unbindEntity(id);
        };
    }


    static MotionAction motion(Scene& scene, LayerId layer, std::optional<MotionId> value) {
        return [&scene, layer, value](double, const MotionSample&) {
            return scene.getMotions().bindMotion(layer, value);
        };
    }


    static MotionAction parent(Scene& scene, EntityId id, std::optional<EntityId> value) {
        return [&scene, id, value](double, const MotionSample&) {
            return scene.setParent(id, value);
        };
    }


    static MotionAction camera(Camera& camera, Camera value) {
        return [&camera, value](double, const MotionSample&) {
            if (!CameraSystem::validCamera(value))
                return false;
            camera = value;
            return true;
        };
    }
};
} // namespace graphics
