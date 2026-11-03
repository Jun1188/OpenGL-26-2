# 기존 실습 23 벽 축소 복원

사본 동기화에서 잃은 벽 중심 기준 균등 축소를 시작 커밋 4eb0486의 함수 원문에서 복원했다. 새 면 모드·회전 API는 유지했다.

확인: Git 원문·변경 블록의 정적 대조. 미실행: 빌드·테스트·화면 확인.

사용자 확인: O로 벽을 열 때 가로·세로가 함께 줄고, 완전 개방에서 숨겨지며 다시 O로 같은 벽이 원래 크기로 돌아와야 한다.

## framework/detail/Patterns.cpp 변경 전후

before:

```cpp
                                   "Select one of the four side walls of addRoom().");

    glm::vec3 pivot(0);
    float top = mesh->localVertices.front().y;

    for (const auto& vertex : mesh->localVertices) {
        pivot += vertex;
        top = std::max(top, vertex.y);
    }
    pivot /= float(mesh->localVertices.size());
    pivot.y = top;

    return openFace(scene, room, face, progress, glm::vec3(0), pivot, true);
}

```

after:

```cpp
                                   "Select one of the four side walls of addRoom().");

    if (!std::isfinite(progress) || progress < 0 || progress > 1)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "openRoomWall",
                                   "Need progress in [0,1].");

    glm::vec3 pivot(0);

    for (const auto& vertex : mesh->localVertices)
        pivot += vertex;
    pivot /= float(mesh->localVertices.size());
    // Fully open faces are hidden; retain a nonsingular matrix for normals/picking.
    const float factor = std::max(1e-3f, 1 - progress);
    const auto scale = TransformSystem::scaling(glm::vec3(factor), pivot);

    return scale && scene.setFaceLocalMatrix(room, face, *scale) && scene.setFaceVisible(room, face, progress < 1);
}

```

## framework/include/Patterns.h 변경 전후

before:

```cpp
    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);
    /// [API] 선택 옆벽의 정점에서 상단 피벗을 계산하고 openFace로 높이를 축소/복원한다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
```

after:

```cpp
    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);
    /// [API] 선택 옆벽의 원래 중심을 기준으로 모든 축을 균등 축소/복원하며 완전 개방 시 숨긴다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
```
