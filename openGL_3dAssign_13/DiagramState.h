#pragma once

#include "Common.h"

#include <cstdint>
#include <optional>
#include <vector>

struct FaceId {
    EntityId entityId = 0;
    std::size_t faceIndex = 0;
};

class Entity { // E: ID를 등록하고 같은 인덱스의 컴포넌트에 연결한다.
public:
    Entity();
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = {});
    bool setTransform(EntityId id, const Transform3D& transform);
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);
    void clear();

    const std::vector<EntityId>& getIds() const { return polyId_; }
    const Component_Poly* getComponent(EntityId id) const;
    const std::vector<FaceId>& getCubeFaceIds() const { return cubeFaceId_; }
    const std::vector<FaceId>& getTetraFaceIds() const { return tetraFaceId_; }
    std::uint64_t getGeometryRevision() const { return geometryRevision_; }

private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_; // 입체와 면 데이터의 단일 소유자.
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::uint64_t geometryRevision_ = 0;
};

class DiagramSystem { // S: 형상 생성과 변환 계산. OpenGL 핸들은 다루지 않는다.
public:
    static Polyhedron makePolyhedron(GLShape shape);
    static bool validPolyhedron(const Polyhedron& polyhedron);
    static bool validTransform(const Transform3D& transform);
    static glm::mat4 modelMatrix(const Transform3D& transform);
    static glm::mat4 faceModelMatrix(const Component_Poly& component,
                                    const PlaneFigure& face);
};
