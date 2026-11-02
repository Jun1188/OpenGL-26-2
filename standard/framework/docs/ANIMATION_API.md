# 그네·사람형·계층 SRT 빠른 참조

[통합 응용패턴과 실제 코드](IMPLEMENTATION_PATTERNS.html#pattern-swing) · [공통 구현·전체 API](API_REFERENCE.html)

사람은 seat의 자식이며 엉덩이 -90°, 무릎 +90°의 앉은 자세다. 허벅지 아래쪽을 좌석 윗면에 맞춘다. frame의 고정 지지대는 hinge의 진자 회전에 포함하지 않는다. torso는 독립 상체 트랙을 가진다.

계층은 frame → hinge → seat → person.root → pelvis → torso/hips다. 목·양어깨·팔꿈치·손·양엉덩이·무릎·발은 각각 편집 가능한 ID다. 머리·코와 팔다리 메시 ID는 관절 ID와 구분한다.

JointAnimation.rest는 초기 자세다. poseHierarchy는 sample.parent × rest × sample.local을 매 프레임 계산한다. 샘플은 강체 변환만, rest는 양수 균일 배율·이동·회전·피벗을 허용한다. 비균일 배율·shear는 거부한다. World 기여는 이 API에서 관절 부모 공간이다.

| 입력 | 실제 동작 |
|---|---|
| 0 / 7 / 8 / 9 | Motion / 격자·거리 / 회전 방 / 그네·사람형·선형보간 |
| 1~6 (Motion) | 타원·사각형·다각형·사용자 3D 경로·원·왕복 꺾은선 |
| Space | 현재 모드의 시간 정지·재개 |
| S / G / M / I (Motion) | 자전 기여 / 전체 공전 기여 / 반사 종류 / 레이어 삽입·삭제 |
| V / D / F | 3뷰 / 단계 로그 / 방 모드 바닥 개방 |
| Tab / Backspace (9번) | 다음 / 이전 관절·프레임 선택 |
| X / Y / Z (9번) | 편집 축 선택 |
| ← / → (9번) | 선택 관절 로컬 회전 -5° / +5° |
| ↑ / ↓ (9번) | 부모 좌표 basis 이동 +.05 / -.05 |
| PageUp / PageDown (9번) | 모델 좌표 localBasis 피벗 +.05 / -.05 |
| + 또는 = / - (9번) | 양수 균일 배율 확대 / 축소 (.1~3) |
| R (9번) | 선택 관절의 초기 기준 SRT·피벗 복원 |
| 9 재입력 | 장면·트랙·모든 편집값 전체 재생성 |
| 왼쪽 클릭 / H / Esc | 객체·면 선택 / 도움말 / 종료 |

```cpp
struct SwingExample {
    graphics::AnimationClock clock;
    std::vector<graphics::JointAnimation> animation;
    bool initialize(graphics::Scene& scene) {
        graphics::HumanoidConfig personConfig;
        personConfig.grid.frame=glm::translate(glm::mat4(1),glm::vec3(-2,0,0));
        auto person=graphics::AnimationSystem::addHumanoid(scene,personConfig); if(!person) return false;
        auto pose=graphics::HumanoidPose::seated();
        if(!graphics::AnimationSystem::poseHumanoid(scene,*person,pose)) return false;
        graphics::SwingConfig config;config.pivot={1,3,0};config.pendulum.periodSeconds=3;
        auto swing=graphics::AnimationSystem::addSwingWithRider(scene,config); if(!swing) return false;
        animation=swing->animation;
        graphics::PendulumMotion elbow;elbow.amplitudeDegrees=10;
        graphics::Motion motion;if(!motion.append(graphics::Motion::pendulum(elbow))) return false;
        auto track=graphics::AnimationSystem::createJointAnimation(scene,swing->person.elbows[0],motion);
        if(!track) return false;
        animation.push_back(*track);clock.reset();return true;
    }
    bool update(graphics::Scene& scene,float dt) {
        return clock.advance(dt)&&graphics::AnimationSystem::poseHierarchy(scene,animation,clock.seconds);
    }
};
```

initialize는 initHandler에서, update는 updateHandler에서 호출한다. 샘플은 강체 회전·이동이며 기준 rest는 양수 균일 배율과 피벗 편집을 허용한다. 비균일 배율·shear는 거부한다. standard 9번은 35개 관절/프레임을 선택해 rest를 편집하고, 매 프레임 같은 rest에서 계산하므로 입력이 덮어써지지 않는다. 전체 연결 코드는 아래에 있다.

Scene::clear 후 rig와 트랙을 다시 만든다. parent 변경은 월드 자세를 자동 보존하지 않는다.
