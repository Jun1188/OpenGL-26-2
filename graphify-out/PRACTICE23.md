# 실습 23 수정 전후

2026-11-02. 변경 전은 작업 시작 HEAD 3897ee5, 변경 후는 최종 저장 소스다. 모든 수정 코드 블록을 포함한다. 문서와 생성 그래프 데이터는 아래 별도로 설명한다.

## openGL_3dAssign_23/Console.cpp

GL_LINES 윤곽선은 면 컬링으로 사라지지 않으므로 벽의 윤곽선 요청을 제거한다. Front 컬링은 유지한다.

변경 전:

```cpp
    const auto stage=graphics::PatternSystem::addRoom(scene,bounds_,false);
    if(!stage) return false;stage_=*stage;
    for(std::size_t face=0;face<4;++face) {
        if(!graphics::PatternSystem::openRoomWall(scene,stage_,face,0)
            || !scene.setOutline(stage_,face,graphics::OutlineMode::FeatureEdges)) return false;
    }
    graphics::GridLayout grid;grid.origin={-.5f,0,-.3f};grid.extent={1,1.8f,.6f};
    const auto rig=graphics::PatternSystem::addWalkingRig(scene,grid);if(!rig) return false;rig_=*rig;
```

변경 후:

```cpp
    const auto stage=graphics::PatternSystem::addRoom(scene,bounds_,false);
    if(!stage) return false;stage_=*stage;
    for(std::size_t face=0;face<4;++face) {
        if(!graphics::PatternSystem::openRoomWall(scene,stage_,face,0)) return false;
    }
    graphics::GridLayout grid;grid.origin={-.5f,0,-.3f};grid.extent={1,1.8f,.6f};
    const auto rig=graphics::PatternSystem::addWalkingRig(scene,grid);if(!rig) return false;rig_=*rig;
```

## openGL_3dAssign_23/framework/detail/Patterns.inl

상단 고정 Y축 축소를 벽 중심 기준 균등 축소로 바꾼다. 진행률 검사를 유지하고 완전 개방은 숨기며 역행렬 계산용 양수 배율을 유지한다.

변경 전:

```cpp
    const auto* mesh=scene.getMesh(room,face);
    if(!mesh || face>=4 || mesh->localVertices.empty())
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Scene,"openRoomWall","Select one of the four side walls of addRoom().");
    glm::vec3 pivot(0);float top=mesh->localVertices.front().y;
    for(const auto& vertex:mesh->localVertices) {pivot+=vertex;top=std::max(top,vertex.y);}
    pivot/=float(mesh->localVertices.size());pivot.y=top;
    return openFace(scene,room,face,progress,glm::vec3(0),pivot,true);
}
inline bool PatternSystem::swapModels(Scene& scene,EntityId first,EntityId second) {
    const auto* a=scene.getComponent(first);const auto* b=scene.getComponent(second);
```

변경 후:

```cpp
    const auto* mesh=scene.getMesh(room,face);
    if(!mesh || face>=4 || mesh->localVertices.empty())
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Scene,"openRoomWall","Select one of the four side walls of addRoom().");
    if(!std::isfinite(progress) || progress<0 || progress>1)
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Transform,"openRoomWall","Need progress in [0,1].");
    glm::vec3 pivot(0);
    for(const auto& vertex:mesh->localVertices) pivot+=vertex;
    pivot/=float(mesh->localVertices.size());
    // Fully open faces are hidden; retain a nonsingular matrix for normals/picking.
    const float factor=std::max(1e-3f,1-progress);
    const auto scale=TransformSystem::scaling(glm::vec3(factor),pivot);
    return scale && scene.setFaceLocalMatrix(room,face,*scale) && scene.setFaceVisible(room,face,progress<1);
}
inline bool PatternSystem::swapModels(Scene& scene,EntityId first,EntityId second) {
    const auto* a=scene.getComponent(first);const auto* b=scene.getComponent(second);
```

## openGL_3dAssign_23/framework/include/Patterns.h

실습 23 사본의 API 주석을 새 중심 기준 축소 동작과 맞춘다.

변경 전:

```cpp
                         const glm::vec3& displacement,const glm::vec3& topPivot,bool scaleOpen);
    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene,EntityId room,const Camera& camera);
    /// [API] 선택 옆벽의 정점에서 상단 피벗을 계산하고 openFace로 높이를 축소/복원한다.
    static bool openRoomWall(Scene& scene,EntityId room,std::size_t face,float progress);
    /// [API] 두 ID의 위치는 유지하고 모델을 교환한다.
    static bool swapModels(Scene& scene,EntityId first,EntityId second);
```

변경 후:

```cpp
                         const glm::vec3& displacement,const glm::vec3& topPivot,bool scaleOpen);
    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene,EntityId room,const Camera& camera);
    /// [API] 선택 옆벽의 정점 평균을 중심으로 균등 축소/복원한다. 진행률 1은 숨기고 0은 원래 크기로 복원한다.
    static bool openRoomWall(Scene& scene,EntityId room,std::size_t face,float progress);
    /// [API] 두 ID의 위치는 유지하고 모델을 교환한다.
    static bool swapModels(Scene& scene,EntityId first,EntityId second);
```

## 문서·그래프

- README.md: 상단 고정 높이 축소 설명을 중심 기준 균등 축소로 바꾸고, 윤곽선을 제거한 이유를 설명했다.
- USER_CHECKS.md: 중심 고정·가로세로 축소·완전 숨김·같은 벽 복원·진행 중 반전·컬링된 벽의 잔여 선을 사용자 확인 항목으로 명시했다.
- graphify-out/: 코드 64개 파일의 정적 구조를 갱신했다. 실행 프로젝트의 진입점은 PROJECTS.json, 공통·프로젝트별 콜백과 CPU → GPU 흐름은 EXECUTION_FLOW.md에 기록했다. 실습 23의 수정된 Patterns 사본을 별도 입력으로 포함했다.

## 사용자 빌드·테스트

1. openGL_3dAssign_23/openGL_assign_23.sln을 열어 Debug 또는 Release x64로 직접 빌드·실행한다.
2. y/Y로 카메라를 공전한다. 컬링된 가까운 벽의 윤곽선이 별도로 남지 않아야 한다.
3. O를 누른다. 선택한 벽의 중심은 고정되고 가로·세로가 같은 비율로 줄어 사라져야 한다.
4. 다시 O를 누른다. 같은 벽이 원래 위치와 크기로 복원되어야 한다. 복원 전 카메라를 옮겨도 같은 벽이 대상이다.
5. 축소·복원 도중 O를 누른다. 현재 크기에서 진행 방향이 바뀌어야 한다. I를 누르면 모든 벽이 초기 크기·가시성으로 돌아와야 한다.

외부에서 Front 컬링된 벽은 축소가 보이지 않을 수 있다. 선택된 벽의 내부 면이 보이는 시점에서도 확인한다. 기존 충돌 경계는 유지하므로 시각적으로 연 벽을 로봇이 통과하는 기능을 추가한 것은 아니다.

빌드·실행·테스트·검증기·그래프 무결성 진단은 실행하지 않았다. 정적 소스 검토와 그래프 생성은 화면 동작 성공의 증거가 아니다. 임시 스크립트·ZIP은 만들지 않았다.
