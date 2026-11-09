# 공개 API 한글 목록

현재 헤더 선언에서 생성했다. 빌드·테스트·브라우저 확인은 사용자가 수행한다.

### SceneActions::transform

```cpp
static MotionAction transform(Scene& scene, EntityId id, Transform3D value);
```

지정 값과 대상 ID를 기존 API에 전달하는 transform 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 8행](../framework/include/Actions.h)

### SceneActions::position

```cpp
static MotionAction position(Scene& scene, EntityId id, glm::vec3 value);
```

지정 값과 대상 ID를 기존 API에 전달하는 position 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 15행](../framework/include/Actions.h)

### SceneActions::scale

```cpp
static MotionAction scale(Scene& scene, EntityId id, glm::vec3 value);
```

지정 값과 대상 ID를 기존 API에 전달하는 scale 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 22행](../framework/include/Actions.h)

### SceneActions::rotation

```cpp
static MotionAction rotation(Scene& scene, EntityId id, glm::mat3 value);
```

지정 값과 대상 ID를 기존 API에 전달하는 rotation 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 34행](../framework/include/Actions.h)

### SceneActions::tint

```cpp
static MotionAction tint(Scene& scene, EntityId id, GlColor value);
```

지정 값과 대상 ID를 기존 API에 전달하는 tint 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 46행](../framework/include/Actions.h)

### SceneActions::pivot

```cpp
static MotionAction pivot(Scene& scene, EntityId id, glm::vec3 value);
```

지정 값과 대상 ID를 기존 API에 전달하는 pivot 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 53행](../framework/include/Actions.h)

### SceneActions::visible

```cpp
static MotionAction visible(Scene& scene, EntityId id, bool value);
```

지정 값과 대상 ID를 기존 API에 전달하는 visible 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 60행](../framework/include/Actions.h)

### SceneActions::faceVisible

```cpp
static MotionAction faceVisible(Scene& scene, EntityId id, std::size_t face, bool value);
```

지정 값과 대상 ID를 기존 API에 전달하는 faceVisible 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 67행](../framework/include/Actions.h)

### SceneActions::faceRotation

```cpp
static MotionAction faceRotation(Scene& scene, EntityId id, std::size_t face, FaceRotationConfig config);
```

지정 값과 대상 ID를 기존 API에 전달하는 faceRotation 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 74행](../framework/include/Actions.h)

### SceneActions::faceMode

```cpp
static MotionAction faceMode(Scene& scene, EntityId id, std::size_t face, FaceTransformMode mode, bool preserveWorld = true);
```

지정 값과 대상 ID를 기존 API에 전달하는 faceMode 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 81행](../framework/include/Actions.h)

### SceneActions::layer

```cpp
static MotionAction layer(Scene& scene, EntityId id, std::optional<LayerId> value);
```

지정 값과 대상 ID를 기존 API에 전달하는 layer 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 89행](../framework/include/Actions.h)

### SceneActions::motion

```cpp
static MotionAction motion(Scene& scene, LayerId layer, std::optional<MotionId> value);
```

지정 값과 대상 ID를 기존 API에 전달하는 motion 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 96행](../framework/include/Actions.h)

### SceneActions::parent

```cpp
static MotionAction parent(Scene& scene, EntityId id, std::optional<EntityId> value);
```

지정 값과 대상 ID를 기존 API에 전달하는 parent 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 103행](../framework/include/Actions.h)

### SceneActions::camera

```cpp
static MotionAction camera(Camera& camera, Camera value);
```

지정 값과 대상 ID를 기존 API에 전달하는 camera 의미 동작을 만든다. 참조 대상은 실행 동안 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 110행](../framework/include/Actions.h)

### HumanoidPose::seated

```cpp
static HumanoidPose seated();
```

허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 34행](../framework/include/Animation.h)

### AnimationSystem::createJointAnimation

```cpp
static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);
```

관절의 현재 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율만 허용한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 55행](../framework/include/Animation.h)

### AnimationSystem::poseHierarchy

```cpp
static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);
```

기준 SRT·피벗과 강체 모션을 합성해 모두 유효할 때 반영한다. 부모 계층과 균일 배율을 유지한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 59행](../framework/include/Animation.h)

### AnimationSystem::addHumanoid

```cpp
static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{}, std::optional<EntityId> parent = std::nullopt);
```

실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 63행](../framework/include/Animation.h)

### AnimationSystem::poseHumanoid

```cpp
static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);
```

관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 68행](../framework/include/Animation.h)

### AnimationSystem::addSwingWithRider

```cpp
static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{}, std::optional<EntityId> parent = std::nullopt);
```

고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 72행](../framework/include/Animation.h)

### Application::Application

```cpp
Application() = default;
```

초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 88행](../framework/include/Application.h)

### Application::~Application

```cpp
~Application();
```

실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 92행](../framework/include/Application.h)

### Application::Application

```cpp
Application(const Application&) = delete;
```

이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.

반환·실패: 호출 불가(복사 금지 선언).

[선언 96행](../framework/include/Application.h)

### Application::operator=

```cpp
Application& operator=(const Application&) = delete;
```

이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.

반환·실패: 호출 불가(복사 금지 선언).

[선언 100행](../framework/include/Application.h)

### Application::setInitHandler

```cpp
void setInitHandler(std::function<bool(Application&)> handler);
```

장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 104행](../framework/include/Application.h)

### Application::setUpdateHandler

```cpp
void setUpdateHandler(std::function<void(Application&, float)> handler);
```

매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 108행](../framework/include/Application.h)

### Application::setKeyHandler

```cpp
void setKeyHandler(std::function<void(Application&, int key, int action)> handler);
```

키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 112행](../framework/include/Application.h)

### Application::setModifiedKeyHandler

```cpp
void setModifiedKeyHandler(std::function<void(Application&, int key, int action, int mods)> handler);
```

키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 117행](../framework/include/Application.h)

### Application::setFocusHandler

```cpp
void setFocusHandler(std::function<void(Application&, bool focused)> handler);
```

창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 121행](../framework/include/Application.h)

### Application::setRenderHandler

```cpp
void setRenderHandler(std::function<bool(Application&)> handler);
```

사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 126행](../framework/include/Application.h)

### Application::setMouseHandler

```cpp
void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);
```

마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 130행](../framework/include/Application.h)

### Application::setCursorHandler

```cpp
void setCursorHandler(std::function<void(Application&, double x, double y)> handler);
```

창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 134행](../framework/include/Application.h)

### Application::pickAt

```cpp
std::optional<PickResult> pickAt(double windowX, double windowY) const;
```

창 좌표를 framebuffer 좌표로 보정하고 기본 카메라의 전체 viewport로 ray picking을 수행한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 138행](../framework/include/Application.h)

### Application::pickAtCursor

```cpp
std::optional<PickResult> pickAtCursor() const;
```

현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 142행](../framework/include/Application.h)

### Application::getAssets

```cpp
AssetLoader& getAssets();
```

실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 146행](../framework/include/Application.h)

### Application::run

```cpp
int run(const ApplicationConfig& config = ApplicationConfig{});
```

초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 150행](../framework/include/Application.h)

### Application::configureRenderer

```cpp
bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {}, const std::filesystem::path& fragmentPath = {});
```

현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 154행](../framework/include/Application.h)

### Application::requestClose

```cpp
void requestClose();
```

창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 159행](../framework/include/Application.h)

### Application::renderScene

```cpp
bool renderScene();
```

기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 163행](../framework/include/Application.h)

### Application::renderViews

```cpp
bool renderViews(const std::vector<RenderPass>& passes);
```

현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 167행](../framework/include/Application.h)

### Application::getFramebufferSize

```cpp
std::array<int, 2> getFramebufferSize() const;
```

현재 창의 실제 픽셀 크기를 반환한다. DPI 배율이 있으면 창 좌표 크기와 다를 수 있다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 171행](../framework/include/Application.h)

### Application::getRenderOptions

```cpp
RenderOptions& getRenderOptions();
```

애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 175행](../framework/include/Application.h)

### Application::getScene

```cpp
Scene& getScene();
```

애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 181행](../framework/include/Application.h)

### Application::getCamera

```cpp
Camera& getCamera();
```

기본 카메라의 수정 가능한 참조를 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 187행](../framework/include/Application.h)

### Application::getWindow

```cpp
GLFWwindow* getWindow() const;
```

GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.

반환·실패: 비소유 포인터다. nullptr과 원본 객체 수명·컨테이너 변경을 확인한다.

[선언 193행](../framework/include/Application.h)

### CuboidMode::setConfig

```cpp
bool setConfig(const CuboidModeConfig& config);
```

초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 58행](../framework/include/CuboidMode.h)

### CuboidMode::getConfig

```cpp
const CuboidModeConfig& getConfig() const;
```

초기 설정의 읽기 전용 참조를 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 61행](../framework/include/CuboidMode.h)

### CuboidMode::setCuboid

```cpp
bool setCuboid(const CuboidConfig& config);
```

같은 이름의 육면체 초기 설정을 교체하거나 새로 등록한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 67행](../framework/include/CuboidMode.h)

### CuboidMode::bindKey

```cpp
bool bindKey(const CuboidKeyBinding& binding);
```

같은 key/action/mods 바인딩을 교체하거나 추가한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 71행](../framework/include/CuboidMode.h)

### CuboidMode::unbindKey

```cpp
bool unbindKey(int key, int action = GLFW_PRESS, int mods = 0);
```

같은 key/action/mods 바인딩을 해제한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 74행](../framework/include/CuboidMode.h)

### CuboidMode::getBindings

```cpp
const std::vector<CuboidKeyBinding>& getBindings() const;
```

키 바인딩 목록의 읽기 전용 참조를 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 77행](../framework/include/CuboidMode.h)

### CuboidMode::setSetupHandler

```cpp
void setSetupHandler(CuboidHandler handler);
```

새 장면 구성 후 실행할 설정 핸들러를 교체한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 82행](../framework/include/CuboidMode.h)

### CuboidMode::setUpdateHandler

```cpp
void setUpdateHandler(std::function<bool(Application&, CuboidMode&, float)> handler);
```

프레임별 응용 갱신 핸들러를 교체한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 87행](../framework/include/CuboidMode.h)

### CuboidMode::apply

```cpp
bool apply(Application& app);
```

별도 CPU Scene에 육면체와 모션을 구성하고 성공하면 장면 전체를 교체한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 93행](../framework/include/CuboidMode.h)

### CuboidMode::update

```cpp
bool update(Application& app, float dt);
```

등록된 모드 프레임 핸들러를 호출한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 96행](../framework/include/CuboidMode.h)

### CuboidMode::dispatch

```cpp
bool dispatch(Application& app, int key, int action, int mods);
```

일치하는 키 바인딩을 실행한다. 없는 키는 정상 처리한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 99행](../framework/include/CuboidMode.h)

### CuboidMode::entity

```cpp
std::optional<EntityId> entity(const std::string& name) const;
```

이름에 대응하는 마지막 생성 엔티티 ID를 조회한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 102행](../framework/include/CuboidMode.h)

### CuboidMode::layer

```cpp
std::optional<LayerId> layer(const std::string& name) const;
```

이름에 대응하는 초기 생성 레이어 ID를 조회한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 105행](../framework/include/CuboidMode.h)

### CuboidMode::motion

```cpp
std::optional<MotionId> motion(const std::string& name) const;
```

이름에 대응하는 초기 생성 모션 ID를 조회한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 108행](../framework/include/CuboidMode.h)

### CuboidMode::action

```cpp
static CuboidHandler action(std::string name, CuboidCommand command);
```

실행 시 이름을 해석하므로 초기 설정 재적용 뒤에도 같은 바인딩을 사용한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 112행](../framework/include/CuboidMode.h)

### bindCuboidMode

```cpp
bool bindCuboidMode(Application& app, std::shared_ptr<CuboidMode> mode);
```

육면체 모드의 init/update/수정키 콜백을 설치한다. 기존 해당 핸들러를 교체한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 115행](../framework/include/CuboidMode.h)

### Diagnostics::context

```cpp
static Diagnostic& context();
```

진단에 첨부할 현재 파일·줄·호출 경로 문맥의 참조를 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 40행](../framework/include/Diagnostics.h)

### Diagnostics::locate

```cpp
static void locate(const char* file, int line, const char* operation);
```

다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 48행](../framework/include/Diagnostics.h)

### Diagnostics::failAt

```cpp
static bool failAt(const char* file, int line, PipelineStage stage, const char* api, const std::string& cause, const std::string& action = "소스 위치와 해당 API의 입력 조건을 확인하세요.", std::optional<std::uint64_t> object = std::nullopt);
```

주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 56행](../framework/include/Diagnostics.h)

### Diagnostics::stageName

```cpp
static const char* stageName(PipelineStage stage);
```

파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.

반환·실패: 비소유 포인터다. nullptr과 원본 객체 수명·컨테이너 변경을 확인한다.

[선언 75행](../framework/include/Diagnostics.h)

### Diagnostics::configure

```cpp
static void configure(DiagnosticOptions value);
```

오류 출력과 단계 추적 출력의 사용 여부를 설정한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 104행](../framework/include/Diagnostics.h)

### Diagnostics::lastError

```cpp
static const std::optional<Diagnostic>& lastError();
```

마지막으로 기록한 오류의 optional 참조를 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 110행](../framework/include/Diagnostics.h)

### Diagnostics::clear

```cpp
static void clear();
```

마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 116행](../framework/include/Diagnostics.h)

### Diagnostics::trace

```cpp
static void trace(PipelineStage stage, const char* api);
```

추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 122행](../framework/include/Diagnostics.h)

### Diagnostics::fail

```cpp
static bool fail( PipelineStage stage, const char* api, const std::string& cause, const std::string& action = "framework/docs/API_KO.md에서 해당 API의 입력 조건과 반환값을 확인하세요.", std::optional<std::uint64_t> object = std::nullopt);
```

현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 129행](../framework/include/Diagnostics.h)

### DynamicsSystem::validFrame

```cpp
static bool validFrame(const ReferenceFrame& frame);
```

기준 프레임의 정규직교 회전과 유한한 가속도·각속도·각가속도를 확인한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 19행](../framework/include/Dynamics.h)

### DynamicsSystem::effectiveAcceleration

```cpp
static std::optional<glm::vec3> effectiveAcceleration(const ReferenceFrame& frame, glm::vec3 localPosition, glm::vec3 localVelocity, glm::vec3 gravityWorld = {0,-9.8f,0});
```

월드 중력과 이동·회전 프레임의 관성항을 합성해 지역 가속도를 계산한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 21행](../framework/include/Dynamics.h)

### DynamicsSystem::step

```cpp
static bool step(ParticleState& particle, const ReferenceFrame& frame, float dt, glm::vec3 gravityWorld = {0,-9.8f,0}, const ForceFunction& force = {}, double startTime = 0);
```

지역 힘 콜백을 질량으로 나누고 점질량을 적분한다. 비유한 결과는 상태에 반영하지 않는다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 24행](../framework/include/Dynamics.h)

### DynamicsSystem::holdingForce

```cpp
static std::optional<glm::vec3> holdingForce(const ReferenceFrame& frame, glm::vec3 localPosition, float mass, glm::vec3 gravityWorld = {0,-9.8f,0});
```

질점을 기준 프레임의 해당 위치에 정지 구속하는 데 필요한 지역 힘을 계산한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 27행](../framework/include/Dynamics.h)

### DynamicsSystem::normalLoad

```cpp
static std::optional<float> normalLoad(const ReferenceFrame& frame, glm::vec3 localPosition, float mass, glm::vec3 normal = {0,1,0}, glm::vec3 gravityWorld = {0,-9.8f,0});
```

정지 구속력을 주어진 법선에 투영한 압축력을 반환한다. 실제 접촉 충돌 계산은 아니다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 30행](../framework/include/Dynamics.h)

### DynamicsSystem::spring

```cpp
static ForceFunction spring(glm::vec3 anchor, float stiffness, float damping);
```

앵커·강성·감쇠로 영자연길이 선형 스프링의 지역 힘 콜백을 만든다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 33행](../framework/include/Dynamics.h)

### transformTexcoords

```cpp
inline bool transformTexcoords(Mesh& mesh, const glm::vec2& scale, const glm::vec2& offset, float rotationDegrees = 0);
```

메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 20행](../framework/include/Geometry.h)

### generateTexcoords

```cpp
inline bool generateTexcoords(Mesh& mesh, const UVOptions& options = UVOptions{});
```

평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 45행](../framework/include/Geometry.h)

### recalculateNormals

```cpp
inline void recalculateNormals(Mesh& mesh);
```

메시의 삼각형 기하로 정점 법선을 다시 계산한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 181행](../framework/include/Geometry.h)

### meshOutline

```cpp
inline std::vector<std::uint32_t> meshOutline(const Mesh& mesh, OutlineMode mode = OutlineMode::FeatureEdges);
```

메시의 윤곽 모드에 따라 선으로 그릴 정점 인덱스 쌍 목록을 계산한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 206행](../framework/include/Geometry.h)

### Scene::Scene

```cpp
Scene();
```

빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 38행](../framework/include/Graphics.h)

### Scene::addGroup

```cpp
std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});
```

메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 42행](../framework/include/Graphics.h)

### Scene::addSphere

```cpp
std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32, unsigned stacks = 16);
```

반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 46행](../framework/include/Graphics.h)

### Scene::addCone

```cpp
std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);
```

반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 51행](../framework/include/Graphics.h)

### Scene::setTint

```cpp
bool setTint(EntityId id, GlColor color);
```

엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 55행](../framework/include/Graphics.h)

### Scene::push_entity

```cpp
std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});
```

주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 59행](../framework/include/Graphics.h)

### Scene::addCuboid

```cpp
std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);
```

중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 63행](../framework/include/Graphics.h)

### Scene::addRightTetrahedron

```cpp
std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);
```

세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 67행](../framework/include/Graphics.h)

### Scene::addPyramid

```cpp
std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);
```

가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 71행](../framework/include/Graphics.h)

### Scene::setFaceVisible

```cpp
bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);
```

해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 75행](../framework/include/Graphics.h)

### Scene::setFaceFrame

```cpp
bool setFaceFrame(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);
```

형상 재업로드 없이 면 앞쪽의 부모 프레임을 설정한다. 렌더·피킹·법선 변환에 함께 반영한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 78행](../framework/include/Graphics.h)

### Scene::setFaceCullMode

```cpp
bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);
```

해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 82행](../framework/include/Graphics.h)

### Scene::setFaceLocalMatrix

```cpp
bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix, FaceAnimationSpace space = FaceAnimationSpace::Local);
```

해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 86행](../framework/include/Graphics.h)

### Scene::setFaceVertexColors

```cpp
bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);
```

면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 91행](../framework/include/Graphics.h)

### Scene::setPosition

```cpp
bool setPosition(EntityId id, const glm::vec3& worldPosition);
```

basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 95행](../framework/include/Graphics.h)

### Scene::setSideLengths

```cpp
bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
```

기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 99행](../framework/include/Graphics.h)

### Scene::setTransform

```cpp
bool setTransform(EntityId id, const Transform3D& transform);
```

기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 103행](../framework/include/Graphics.h)

### Scene::setParent

```cpp
bool setParent(EntityId id, std::optional<EntityId> parent);
```

부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 107행](../framework/include/Graphics.h)

### Scene::placementMatrix

```cpp
std::optional<glm::mat4> placementMatrix(EntityId id) const;
```

Full affine placement before this object's local layers; parent's final layers are inherited.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 110행](../framework/include/Graphics.h)

### Scene::setWorldPlacement

```cpp
bool setWorldPlacement(EntityId id, const glm::mat4& world);
```

Sets world-authored placement without decomposing a potentially sheared matrix.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 112행](../framework/include/Graphics.h)

### Scene::setParentPreserveWorld

```cpp
bool setParentPreserveWorld(EntityId id, std::optional<EntityId> parent);
```

Changes parent while preserving placement and this object's local layer list.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 114행](../framework/include/Graphics.h)

### Scene::setAttachment

```cpp
bool setAttachment(EntityId id, const glm::mat4& matrix);
```

Sets the parent-relative affine attachment, before base SRT.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 116행](../framework/include/Graphics.h)

### Scene::setLocalLayers

```cpp
bool setLocalLayers(EntityId id, const std::vector<glm::mat4>& layers);
```

Replaces ordered local layers atomically. Each layer inherits all preceding layers.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 118행](../framework/include/Graphics.h)

### Scene::worldMatrix

```cpp
std::optional<glm::mat4> worldMatrix(EntityId id) const;
```

부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 123행](../framework/include/Graphics.h)

### Scene::faceWorldMatrix

```cpp
std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
```

엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 127행](../framework/include/Graphics.h)

### Scene::lineWorldMatrix

```cpp
std::optional<glm::mat4> lineWorldMatrix(LineId id) const;
```

선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 131행](../framework/include/Graphics.h)

### Scene::getMotions

```cpp
MotionSystem& getMotions();
```

Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 135행](../framework/include/Graphics.h)

### Scene::getMotions

```cpp
const MotionSystem& getMotions() const;
```

Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 141행](../framework/include/Graphics.h)

### Scene::bindEntity

```cpp
bool bindEntity(EntityId id, LayerId layer);
```

엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 147행](../framework/include/Graphics.h)

### Scene::unbindEntity

```cpp
bool unbindEntity(EntityId id);
```

엔티티와 모션 레이어의 연결을 해제한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 151행](../framework/include/Graphics.h)

### Scene::getEntityLayer

```cpp
std::optional<LayerId> getEntityLayer(EntityId id) const;
```

엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 155행](../framework/include/Graphics.h)

### Scene::updateMotions

```cpp
bool updateMotions(double deltaSeconds);
```

Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 159행](../framework/include/Graphics.h)

### Scene::setFaceTransform

```cpp
bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);
```

면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 165행](../framework/include/Graphics.h)

### Scene::setFaceTransformMode

```cpp
bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);
```

면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 169행](../framework/include/Graphics.h)

### Scene::addModel

```cpp
std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{}, PivotMode pivot = PivotMode::BottomCenter, const glm::vec3& customPivot = glm::vec3(0));
```

CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 173행](../framework/include/Graphics.h)

### Scene::setModel

```cpp
bool setModel(EntityId id, const Model& model);
```

기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 179행](../framework/include/Graphics.h)

### Scene::setMesh

```cpp
bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);
```

기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 183행](../framework/include/Graphics.h)

### Scene::getMesh

```cpp
const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;
```

지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.

반환·실패: 비소유 포인터다. nullptr과 원본 객체 수명·컨테이너 변경을 확인한다.

[선언 187행](../framework/include/Graphics.h)

### Scene::setMeshVertices

```cpp
bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& triangles);
```

메시의 정점과 삼각형 인덱스를 함께 교체한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 191행](../framework/include/Graphics.h)

### Scene::setVertexPosition

```cpp
bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);
```

메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 196행](../framework/include/Graphics.h)

### Scene::setTriangleIndices

```cpp
bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);
```

메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 200행](../framework/include/Graphics.h)

### Scene::generateTexcoords

```cpp
bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});
```

평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 204행](../framework/include/Graphics.h)

### Scene::transformTexcoords

```cpp
bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset, float rotationDegrees = 0);
```

메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 208행](../framework/include/Graphics.h)

### Scene::setTexcoords

```cpp
bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);
```

메시의 정점별 UV 배열을 교체한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 213행](../framework/include/Graphics.h)

### Scene::setVertexNormals

```cpp
bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);
```

메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 217행](../framework/include/Graphics.h)

### Scene::setMaterial

```cpp
bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);
```

메시의 색상·텍스처 등 재질 데이터를 교체한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 221행](../framework/include/Graphics.h)

### Scene::setOutline

```cpp
bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode, const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});
```

윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 225행](../framework/include/Graphics.h)

### Scene::setLocalBasis

```cpp
bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));
```

피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 230행](../framework/include/Graphics.h)

### Scene::setVisible

```cpp
bool setVisible(EntityId id, bool visible);
```

엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 234행](../framework/include/Graphics.h)

### Scene::splitFace

```cpp
bool splitFace(EntityId id, std::size_t faceIndex);
```

지정 면을 독립 변환 가능한 면으로 분리한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 238행](../framework/include/Graphics.h)

### Scene::setFaceSRT

```cpp
bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);
```

면의 크기·회전·이동 변환을 설정한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 242행](../framework/include/Graphics.h)

### Scene::addLine

```cpp
std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color, std::optional<EntityId> owner = std::nullopt);
```

두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 246행](../framework/include/Graphics.h)

### Scene::addVector

```cpp
std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color, std::optional<EntityId> owner = std::nullopt);
```

시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 251행](../framework/include/Graphics.h)

### Scene::addBasis

```cpp
std::optional<std::array<LineId, 3>> addBasis(const glm::vec3& origin, const glm::mat3& basisVectors = glm::mat3(1.0f), const glm::vec3& axisLengths = glm::vec3(1.0f), std::optional<EntityId> owner = std::nullopt);
```

기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 257행](../framework/include/Graphics.h)

### Scene::setVector

```cpp
bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);
```

기존 선을 시작점과 방향/길이 벡터로 갱신한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 264행](../framework/include/Graphics.h)

### Scene::clear

```cpp
void clear();
```

Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 268행](../framework/include/Graphics.h)

### Scene::getIds

```cpp
const std::vector<EntityId>& getIds() const;
```

현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 272행](../framework/include/Graphics.h)

### Scene::getComponent

```cpp
const Component_Poly* getComponent(EntityId id) const;
```

엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.

반환·실패: 비소유 포인터다. nullptr과 원본 객체 수명·컨테이너 변경을 확인한다.

[선언 278행](../framework/include/Graphics.h)

### Scene::getCubeFaceIds

```cpp
const std::vector<FaceId>& getCubeFaceIds() const;
```

직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 282행](../framework/include/Graphics.h)

### Scene::getTetraFaceIds

```cpp
const std::vector<FaceId>& getTetraFaceIds() const;
```

사면체 면 ID 목록을 읽기 전용 참조로 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 288행](../framework/include/Graphics.h)

### Scene::getPyramidFaceIds

```cpp
const std::vector<FaceId>& getPyramidFaceIds() const;
```

피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 294행](../framework/include/Graphics.h)

### Scene::getLines

```cpp
const std::vector<Line3D>& getLines() const;
```

Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 300행](../framework/include/Graphics.h)

### Scene::getGeometryRevision

```cpp
std::uint64_t getGeometryRevision() const;
```

렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 306행](../framework/include/Graphics.h)

### DiagramSystem::makePolyhedron

```cpp
static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));
```

형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 314행](../framework/include/Graphics.h)

### DiagramSystem::makeSphere

```cpp
static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);
```

반지름·분할 수로 CPU 구 메시 모델을 생성한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 318행](../framework/include/Graphics.h)

### DiagramSystem::makeCone

```cpp
static Model makeCone(float radius, float height, unsigned slices = 32);
```

반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 322행](../framework/include/Graphics.h)

### DiagramSystem::validPolyhedron

```cpp
static bool validPolyhedron(const Polyhedron& polyhedron);
```

다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 326행](../framework/include/Graphics.h)

### DiagramSystem::validTransform

```cpp
static bool validTransform(const Transform3D& transform);
```

변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 330행](../framework/include/Graphics.h)

### DiagramSystem::modelMatrix

```cpp
static glm::mat4 modelMatrix(const Transform3D& transform);
```

T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 334행](../framework/include/Graphics.h)

### DiagramSystem::faceModelMatrix

```cpp
static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
```

컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 338행](../framework/include/Graphics.h)

### CameraSystem::validCamera

```cpp
static bool validCamera(const Camera& camera);
```

시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 355행](../framework/include/Graphics.h)

### CameraSystem::viewMatrix

```cpp
static glm::mat4 viewMatrix(const Camera& camera);
```

카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 359행](../framework/include/Graphics.h)

### CameraSystem::projectionMatrix

```cpp
static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
```

카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 363행](../framework/include/Graphics.h)

### readTextFile

```cpp
std::string readTextFile(const std::filesystem::path& path);
```

지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 367행](../framework/include/Graphics.h)

### Renderer::Renderer

```cpp
Renderer() = default;
```

GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 439행](../framework/include/Graphics.h)

### Renderer::Renderer

```cpp
Renderer(const Renderer&) = delete;
```

이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.

반환·실패: 호출 불가(복사 금지 선언).

[선언 443행](../framework/include/Graphics.h)

### Renderer::operator=

```cpp
Renderer& operator=(const Renderer&) = delete;
```

이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.

반환·실패: 호출 불가(복사 금지 선언).

[선언 447행](../framework/include/Graphics.h)

### Renderer::initialize

```cpp
bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{}, const std::filesystem::path& fragmentPath = std::filesystem::path{}, const RenderConfig& config = RenderConfig{});
```

현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 452행](../framework/include/Graphics.h)

### Renderer::render

```cpp
bool render(const Scene& scene, const Camera& camera, int width, int height, const RenderOptions& options = RenderOptions{});
```

Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 458행](../framework/include/Graphics.h)

### Renderer::renderPass

```cpp
bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);
```

viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 463행](../framework/include/Graphics.h)

### Renderer::renderViews

```cpp
bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);
```

프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 467행](../framework/include/Graphics.h)

### Renderer::destroy

```cpp
void destroy();
```

GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 471행](../framework/include/Graphics.h)

### Renderer::abandon

```cpp
void abandon();
```

컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 475행](../framework/include/Graphics.h)

### GridLayout::valid

```cpp
bool valid() const;
```

origin·extent·셀 개수와 frame의 유한성·양수 크기·가역성을 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 13행](../framework/include/GridLayout.h)

### GridLayout::position

```cpp
std::optional<glm::vec3> position(const glm::vec3& ratio) const;
```

[0,1] 비율을 origin+extent*ratio로 바꾼 뒤 frame으로 월드 위치를 계산한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 23행](../framework/include/GridLayout.h)

### GridLayout::cellCenter

```cpp
std::optional<glm::vec3> cellCenter(const glm::ivec3& index) const;
```

0부터 시작하는 셀 인덱스를 해당 셀 중심의 월드 위치로 변환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 46행](../framework/include/GridLayout.h)

### GridLayout::relativeSize

```cpp
std::optional<glm::vec3> relativeSize(const glm::vec3& ratio) const;
```

양수 비율과 extent를 곱해 그리드 로컬 크기를 반환한다. frame 회전/배율은 적용하지 않는다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 60행](../framework/include/GridLayout.h)

### GridLayout::positionInParent

```cpp
std::optional<glm::vec3> positionInParent(const glm::vec3& ratio, const glm::mat4& parentWorld) const;
```

비율 위치를 월드로 계산한 뒤 parentWorld의 역행렬로 부모 로컬 위치를 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 83행](../framework/include/GridLayout.h)

### GridLayout::cellAt

```cpp
std::optional<glm::ivec3> cellAt(const glm::vec3& world) const;
```

월드 점을 frame 역변환으로 그리드 셀 인덱스에 대응시킨다. 그리드 밖이면 빈 결과다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 109행](../framework/include/GridLayout.h)

### TextureImage::valid

```cpp
bool valid() const;
```

이미지 너비·높이와 RGBA 바이트 수가 맞는지 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 54행](../framework/include/Model.h)

### PlaneFigure::fromVertices

```cpp
static PlaneFigure fromVertices(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& triangles);
```

정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 99행](../framework/include/Model.h)

### Polyhedron::meshes

```cpp
std::vector<Mesh>& meshes();
```

모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 124행](../framework/include/Model.h)

### Polyhedron::meshes

```cpp
const std::vector<Mesh>& meshes() const;
```

모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 130행](../framework/include/Model.h)

### ModelResult::operator bool

```cpp
explicit operator bool() const;
```

모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 32행](../framework/include/ModelLoader.h)

### TextureResult::operator bool

```cpp
explicit operator bool() const;
```

모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 43행](../framework/include/ModelLoader.h)

### AssetLoader::AssetLoader

```cpp
explicit AssetLoader(std::filesystem::path projectRoot = detail::executableDirectory(), FeatureOptions features = FeatureOptions{}) : projectRoot_(std::filesystem::absolute(projectRoot)), features_(features);
```

프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 121행](../framework/include/ModelLoader.h)

### AssetLoader::features

```cpp
const FeatureOptions& features() const;
```

로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 128행](../framework/include/ModelLoader.h)

### AssetLoader::loadTexture

```cpp
TextureResult loadTexture(const std::filesystem::path& relative, bool flipRows = true) const;
```

프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 134행](../framework/include/ModelLoader.h)

### AssetLoader::loadObj

```cpp
ModelResult loadObj(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
```

프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 144행](../framework/include/ModelLoader.h)

### AssetLoader::loadPrimitive

```cpp
ModelResult loadPrimitive(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
```

기본 도형 OBJ를 읽는 호환 진입점이다. OBJ 로더로 처리를 위임한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 148행](../framework/include/ModelLoader.h)

### AssetLoader::loadModel

```cpp
ModelResult loadModel(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
```

모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 152행](../framework/include/ModelLoader.h)

### Motion::append

```cpp
bool append(const MotionStep& step);
```

모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 75행](../framework/include/Motion.h)

### Motion::insert

```cpp
bool insert(std::size_t index, const MotionStep& step);
```

주어진 인덱스에 모션 단계를 삽입한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 79행](../framework/include/Motion.h)

### Motion::insertAction

```cpp
bool insertAction(std::size_t index, MotionAction action, double startSeconds = 0, std::optional<double> durationSeconds = std::nullopt);
```

변환 사이의 인덱스에 의미 동작 또는 람다·함수 포인터를 삽입한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 83행](../framework/include/Motion.h)

### Motion::replace

```cpp
bool replace(std::size_t index, const MotionStep& step);
```

주어진 인덱스의 모션 단계를 새 단계로 교체한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 88행](../framework/include/Motion.h)

### Motion::erase

```cpp
bool erase(std::size_t index);
```

주어진 인덱스의 모션 단계를 삭제한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 92행](../framework/include/Motion.h)

### Motion::then

```cpp
bool then(MotionStep step);
```

모든 앞 단계의 유한 종료 시각을 구한 뒤 새 단계의 시작을 그 뒤로 이동해 순차 실행을 만든다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 96행](../framework/include/Motion.h)

### Motion::getSteps

```cpp
const std::vector<MotionStep>& getSteps() const;
```

등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.

반환·실패: 비소유 참조다. 반환한 Scene/배열/상태의 수명 동안만 사용한다.

[선언 100행](../framework/include/Motion.h)

### Motion::sample

```cpp
std::optional<MotionSample> sample(double seconds) const;
```

절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다. 의미 동작은 실행하지 않는다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 106행](../framework/include/Motion.h)

### Motion::execute

```cpp
std::optional<MotionSample> execute(double seconds) const;
```

변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 110행](../framework/include/Motion.h)

### Motion::action

```cpp
static MotionStep action(MotionAction action, double startSeconds = 0, std::optional<double> durationSeconds = std::nullopt);
```

기존 append/insert/then/replace와 조립하는 비변환 동작 단계를 만든다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 114행](../framework/include/Motion.h)

### Motion::fixed

```cpp
static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);
```

고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 119행](../framework/include/Motion.h)

### Motion::rotation

```cpp
static MotionStep rotation(const RotationMotion& config);
```

축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 123행](../framework/include/Motion.h)

### Motion::followPath

```cpp
static MotionStep followPath(const PathMotion& config);
```

경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 127행](../framework/include/Motion.h)

### Motion::linear

```cpp
static MotionStep linear(const LinearMotion& config);
```

시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 131행](../framework/include/Motion.h)

### Motion::pendulum

```cpp
static MotionStep pendulum(const PendulumMotion& config);
```

지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 135행](../framework/include/Motion.h)

### MotionSystem::rootLayer

```cpp
static constexpr LayerId rootLayer();
```

삭제할 수 없는 루트 레이어의 ID 0을 반환한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 171행](../framework/include/Motion.h)

### MotionSystem::createMotion

```cpp
std::optional<MotionId> createMotion(const Motion& motion);
```

모션을 시스템에 등록하고 새 MotionId를 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 177행](../framework/include/Motion.h)

### MotionSystem::createMotion

```cpp
std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);
```

의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 181행](../framework/include/Motion.h)

### MotionSystem::setMotion

```cpp
bool setMotion(MotionId id, const Motion& motion);
```

기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 185행](../framework/include/Motion.h)

### MotionSystem::getMotion

```cpp
const MotionState* getMotion(MotionId id) const;
```

모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.

반환·실패: 비소유 포인터다. nullptr과 원본 객체 수명·컨테이너 변경을 확인한다.

[선언 189행](../framework/include/Motion.h)

### MotionSystem::setMotionPlaying

```cpp
bool setMotionPlaying(MotionId id, bool playing);
```

모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 193행](../framework/include/Motion.h)

### MotionSystem::setMotionEnabled

```cpp
bool setMotionEnabled(MotionId id, bool enabled);
```

모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 197행](../framework/include/Motion.h)

### MotionSystem::seekMotion

```cpp
bool seekMotion(MotionId id, double seconds);
```

지정 시각을 샘플링하고 유효할 때 모션의 시간과 샘플을 함께 바꾼다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 201행](../framework/include/Motion.h)

### MotionSystem::removeMotion

```cpp
bool removeMotion(MotionId id);
```

모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 205행](../framework/include/Motion.h)

### MotionSystem::update

```cpp
bool update(double deltaSeconds);
```

모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 209행](../framework/include/Motion.h)

### MotionSystem::clear

```cpp
void clear();
```

모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 213행](../framework/include/Motion.h)

### MotionSystem::pushLayer

```cpp
std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
```

유효한 부모 아래 새 자식 레이어를 추가한다. 복사한 임시 맵에 반영한 뒤 swap하여 상태를 확정한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 217행](../framework/include/Motion.h)

### MotionSystem::insertLayer

```cpp
std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
```

기존 child와 그 parent 사이에 새 레이어를 끼운다. root 위 삽입은 허용하지 않는다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 221행](../framework/include/Motion.h)

### MotionSystem::popLayer

```cpp
std::optional<LayerId> popLayer(LayerId top);
```

자식이 없는 비root leaf만 제거하고 그 부모 ID를 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 225행](../framework/include/Motion.h)

### MotionSystem::removeLayer

```cpp
bool removeLayer(LayerId id);
```

root 이외 레이어를 제거하며 자식들을 부모의 해당 위치로 승격한다. 월드 자세 보존을 자동 약속하지 않는다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 229행](../framework/include/Motion.h)

### MotionSystem::reparentLayer

```cpp
bool reparentLayer(LayerId id, LayerId parent);
```

레이어의 부모를 바꾸며 자신의 자손으로 연결되는 순환을 거부한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 233행](../framework/include/Motion.h)

### MotionSystem::setLayer

```cpp
bool setLayer(LayerId id, const LayerConfig& config);
```

기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 237행](../framework/include/Motion.h)

### MotionSystem::getLayer

```cpp
const LayerState* getLayer(LayerId id) const;
```

레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.

반환·실패: 비소유 포인터다. nullptr과 원본 객체 수명·컨테이너 변경을 확인한다.

[선언 241행](../framework/include/Motion.h)

### MotionSystem::bindMotion

```cpp
bool bindMotion(LayerId layer, std::optional<MotionId> motion);
```

기존 레이어에 모션 ID를 연결하거나 해제한다. 모션과 레이어가 존재하는지 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 245행](../framework/include/Motion.h)

### MotionSystem::layerMatrices

```cpp
std::optional<MotionSample> layerMatrices(LayerId leaf) const;
```

leaf까지의 계층을 따라 transform과 motion 샘플을 World/Local 공간에 맞게 parent/local 행렬로 합성한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 249행](../framework/include/Motion.h)

### MotionSystem::createSpin

```cpp
std::optional<MotionId> createSpin(RotationMotion config);
```

로컬 공간 회전 모션을 등록하여 자전에 사용한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 253행](../framework/include/Motion.h)

### MotionSystem::createOrbit

```cpp
std::optional<MotionId> createOrbit(RotationMotion config);
```

월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 257행](../framework/include/Motion.h)

### Bounds3::valid

```cpp
bool valid() const;
```

해당 데이터가 계산에 필요한 유효 범위와 유한성 조건을 만족하는지 반환한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 12행](../framework/include/Patterns.h)

### Bounds3::contains

```cpp
bool contains(const glm::vec3& point, const glm::vec3& margin = glm::vec3(0)) const;
```

점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 16행](../framework/include/Patterns.h)

### Bounds3::overlaps

```cpp
bool overlaps(const Bounds3& other) const;
```

두 축 정렬 경계 상자가 겹치는지 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 20행](../framework/include/Patterns.h)

### DistancePath::setPoints

```cpp
bool setPoints(const std::vector<glm::vec3>& points, bool closed = false);
```

연속 점들의 누적 거리 경로를 구성한다. closed면 마지막에서 처음으로 연결한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 34행](../framework/include/Patterns.h)

### DistancePath::length

```cpp
double length() const;
```

경로 전체 누적 길이를 반환한다. 경로가 비어 있으면 0이다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 38행](../framework/include/Patterns.h)

### DistancePath::sample

```cpp
std::optional<PathPose> sample(double distance, Playback playback = Playback::Loop) const;
```

누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 44행](../framework/include/Patterns.h)

### AnimationClock::advance

```cpp
bool advance(double dt);
```

playing일 때만 누적 시간을 늘린다. dt와 누적 값의 유한성·범위를 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 53행](../framework/include/Patterns.h)

### AnimationClock::reset

```cpp
void reset();
```

누적 시간을 0으로 되돌리고 재생 상태를 켠다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 57행](../framework/include/Patterns.h)

### RoomBody::position

```cpp
glm::vec3 position(const glm::mat4& roomWorld) const;
```

이탈 전에는 roomWorld로 로컬 중심을 월드로 변환하고 이탈 후에는 worldPosition을 반환한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 70행](../framework/include/Patterns.h)

### CollisionSystem::stepRoomBody

```cpp
static bool stepRoomBody(RoomBody& body, const Bounds3& room, const glm::mat4& roomWorld, float dt, bool floorOpen, bool elastic, const glm::vec3& gravity = glm::vec3(0, -9.8f, 0), const glm::vec3& angularVelocity = glm::vec3(0));
```

방 내부에서는 로컬 반사/중력을 계산하고 released 뒤에는 월드 중력으로 적분한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 91행](../framework/include/Patterns.h)

### CollisionSystem::jump

```cpp
static bool jump(CharacterBody& body, float speed);
```

접지 상태에서만 위쪽 속도를 설정한다. 공중 입력 거절은 정상적인 false다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 97행](../framework/include/Patterns.h)

### CollisionSystem::stepCharacter

```cpp
static std::optional<CharacterStep> stepCharacter(CharacterBody& body, const Bounds3& stage, const std::vector<Bounds3>& obstacles, float dt, float gravity = 9.8f);
```

수평 이동·장애물/경계 반전·중력·윗면 착지·지지면 이탈을 하위 단계로 계산한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 101행](../framework/include/Patterns.h)

### CollisionSystem::randomFreeBox

```cpp
static std::optional<Bounds3> randomFreeBox(const Bounds3& area, const glm::vec3& size, const std::vector<Bounds3>& occupied, std::mt19937& random, unsigned attempts = 128);
```

제한 횟수 안에서 occupied와 겹치지 않는 배치를 찾는다. 난수 엔진의 시드·수명은 호출자가 관리한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 107행](../framework/include/Patterns.h)

### PatternSystem::conveyorCornerPose

```cpp
static std::optional<PathPose> conveyorCornerPose(double distance, const glm::vec3& start, const glm::vec3& pivot, float radius, float endZ);
```

직선 → 안쪽 코너 중심 원호 → 직선의 거리에서 위치·접선·yaw를 계산한다. 끝에서 처음으로 순환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 128행](../framework/include/Patterns.h)

### PatternSystem::addBox

```cpp
static std::optional<EntityId> addBox(Scene& scene, const glm::vec3& size, const glm::vec3& position, GlColor color, std::optional<EntityId> parent = std::nullopt);
```

양수 크기의 중심 기준 모델을 만들고 부모 로컬 위치·색·부모 관계를 설정한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 133행](../framework/include/Patterns.h)

### PatternSystem::addPolygon

```cpp
static std::optional<EntityId> addPolygon(Scene& scene, const std::vector<glm::vec3>& points, GlColor color, std::optional<EntityId> parent = std::nullopt);
```

점 목록으로 단일 다각형 면을 만들고 색·부모 관계와 함께 Scene에 등록한다. 오목 다각형의 자동 분할을 가정하지 않는다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 138행](../framework/include/Patterns.h)

### PatternSystem::addRoom

```cpp
static std::optional<EntityId> addRoom(Scene& scene, const Bounds3& bounds, bool frontOpen = true);
```

여섯 면의 방을 만들고 바깥 winding에 Front cull, 안쪽 조명 법선을 적용한다. face 0..3은 옆벽, 4 천장, 5 바닥이다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 143행](../framework/include/Patterns.h)

### PatternSystem::addJoint

```cpp
static std::optional<EntityId> addJoint(Scene& scene, EntityId parent, const glm::vec3& anchor);
```

부모에 부착된 transform-only 그룹을 만든다. 표시 메시 크기와 회전 연결 위치를 분리한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 147행](../framework/include/Patterns.h)

### PatternSystem::rotateJoint

```cpp
static bool rotateJoint(Scene& scene, EntityId joint, const glm::vec3& axis, float degrees);
```

연결 위치와 크기를 유지하면서 회전 기저를 변경한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 151행](../framework/include/Patterns.h)

### PatternSystem::follow

```cpp
static bool follow(Scene& scene, EntityId entity, const DistancePath& path, double distance, const glm::vec3& offset = glm::vec3(0), bool orient = true);
```

DistancePath 샘플을 엔티티 위치와 +Z 기준 Y 회전에 반영한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 155행](../framework/include/Patterns.h)

### PatternSystem::addNumber

```cpp
static bool addNumber(Scene& scene, unsigned digit, const glm::vec3& position, float height, GlColor color, std::optional<EntityId> parent = std::nullopt);
```

0~9를 선분으로 구성한다. 부모 로컬 XY 평면의 표시이며 일반 폰트/텍스트 렌더러는 아니다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 160행](../framework/include/Patterns.h)

### PatternSystem::foldFace

```cpp
static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);
```

지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 165행](../framework/include/Patterns.h)

### PatternSystem::rotateFace

```cpp
static bool rotateFace(Scene& scene, EntityId entity, std::size_t face, const FaceRotationConfig& config);
```

원래 모서리 힌지·면 중심 자전·원본 모델 중심 공전을 구분하여 면 회전 행렬을 설정한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 169행](../framework/include/Patterns.h)

### PatternSystem::openFace

```cpp
static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement, const glm::vec3& topPivot, bool scaleOpen);
```

진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 173행](../framework/include/Patterns.h)

### PatternSystem::cameraFacingWall

```cpp
static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);
```

시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 178행](../framework/include/Patterns.h)

### PatternSystem::openRoomWall

```cpp
static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
```

선택 옆벽의 원래 중심을 기준으로 모든 축을 균등 축소/복원하며 완전 개방 시 숨긴다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 182행](../framework/include/Patterns.h)

### PatternSystem::snakeCells

```cpp
static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
```

셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 186행](../framework/include/Patterns.h)

### PatternSystem::aligned

```cpp
static bool aligned(float first, float second, float tolerance);
```

두 값의 차이가 허용 오차 이하인지 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 190행](../framework/include/Patterns.h)

### PatternSystem::snap

```cpp
static glm::vec3 snap(const glm::vec3& point, float spacing);
```

좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 194행](../framework/include/Patterns.h)

### PatternSystem::screenOnPlane

```cpp
static std::optional<glm::vec3> screenOnPlane(const Camera& camera, double x, double y, const PickViewport& viewport, const glm::vec3& planePoint, const glm::vec3& normal);
```

화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 198행](../framework/include/Patterns.h)

### PatternSystem::moveCamera

```cpp
static bool moveCamera(Camera& camera, const glm::vec3& offset);
```

시점과 주목점을 동일하게 이동해 시선 방향을 보존한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 204행](../framework/include/Patterns.h)

### PatternSystem::yawCamera

```cpp
static bool yawCamera(Camera& camera, float degrees);
```

시점은 고정하고 카메라 로컬 up축을 기준으로 시선을 회전한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 208행](../framework/include/Patterns.h)

### PatternSystem::orbitCamera

```cpp
static bool orbitCamera(Camera& camera, const AxisLine& axis, float degrees);
```

월드 축에 대해 시점·주목점·up을 함께 변환한다. 축을 target에 두면 target은 고정된다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 212행](../framework/include/Patterns.h)

### PatternSystem::zoomCamera

```cpp
static bool zoomCamera(Camera& camera, float factor);
```

원근은 target까지 거리, 직각은 orthographicHalfExtent를 배율로 바꾼다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 216행](../framework/include/Patterns.h)

### PatternSystem::threeViews

```cpp
static std::vector<RenderPass> threeViews(const Camera& perspective, int width, int height, const RenderOptions& options = RenderOptions{});
```

원근·XZ 평면도·XY 정면도 카메라와 픽셀 영역을 구성한다. 상단 뷰의 up은 -Z다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 220행](../framework/include/Patterns.h)

### PatternSystem::addWalkingRig

```cpp
static std::optional<JointRig> addWalkingRig(Scene& scene, const GridLayout& bodyGrid);
```

그리드 비율로 머리·코·몸통·어깨·엉덩이·팔·다리 계층을 조립하고 ID 묶음을 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 225행](../framework/include/Patterns.h)

### PatternSystem::poseWalkingRig

```cpp
static bool poseWalkingRig(Scene& scene, const JointRig& rig, const glm::vec3& feet, float yawDegrees, double walkDistance, float speed);
```

루트 이동/yaw와 실제 이동 거리 기반 팔다리 반대 위상을 적용한다. 속도는 보행 진폭에 반영한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 229행](../framework/include/Patterns.h)

### screenRay

```cpp
inline std::optional<Ray> screenRay(const Camera& camera, double x, double y, const PickViewport& viewport);
```

왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 25행](../framework/include/Picking.h)

### pick

```cpp
inline std::optional<PickResult> pick(const Scene& scene, const Ray& input, CullMode inheritedCull = CullMode::None);
```

ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 61행](../framework/include/Picking.h)

### pick

```cpp
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y, const PickViewport& viewport, CullMode inheritedCull = CullMode::None);
```

ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 165행](../framework/include/Picking.h)

### DrawUniformContext::setUniform

```cpp
bool setUniform(const std::string& name, float value) const;
```

현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 52행](../framework/include/RenderConfig.h)

### DrawUniformContext::setUniform

```cpp
bool setUniform(const std::string& name, int value) const;
```

현재 program의 추가 int/bool/sampler uniform을 설정한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 62행](../framework/include/RenderConfig.h)

### DrawUniformContext::setUniform

```cpp
bool setUniform(const std::string& name, const glm::vec3& value) const;
```

현재 program의 추가 vec3 uniform을 설정한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 72행](../framework/include/RenderConfig.h)

### DrawUniformContext::setUniform

```cpp
bool setUniform(const std::string& name, const glm::mat4& value) const;
```

현재 program의 추가 mat4 uniform을 설정한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 82행](../framework/include/RenderConfig.h)

### TransformSystem::validAffine

```cpp
static bool validAffine(const glm::mat4& matrix);
```

행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.

반환·실패: true/false로 결과를 판단한다. valid/contains/overlaps 등 판정 함수의 false는 오류와 구분한다.

[선언 19행](../framework/include/Transform.h)

### TransformSystem::translation

```cpp
static std::optional<glm::mat4> translation(const glm::vec3& offset);
```

이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 23행](../framework/include/Transform.h)

### TransformSystem::scaling

```cpp
static std::optional<glm::mat4> scaling(const glm::vec3& factors, const glm::vec3& center = glm::vec3(0));
```

중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 27행](../framework/include/Transform.h)

### TransformSystem::rotation

```cpp
static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
```

직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 31행](../framework/include/Transform.h)

### TransformSystem::reflectPoint

```cpp
static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
```

주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 35행](../framework/include/Transform.h)

### TransformSystem::reflectLine

```cpp
static std::optional<glm::mat4> reflectLine(const AxisLine& line);
```

주어진 직선을 기준으로 선 반사 행렬을 만든다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 39행](../framework/include/Transform.h)

### TransformSystem::reflectPlane

```cpp
static std::optional<glm::mat4> reflectPlane(const glm::vec3& point, const glm::vec3& normal);
```

평면의 점과 법선으로 평면 반사 행렬을 만든다.

반환·실패: 값이 있으면 사용한다. 빈 optional은 실패 또는 선택/교차 결과 없음이다. 정상적인 빈 결과도 있으므로 호출부 조건을 확인한다.

[선언 43행](../framework/include/Transform.h)

### PathSystem::ellipse

```cpp
static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU, const glm::vec3& axisV);
```

중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 50행](../framework/include/Transform.h)

### PathSystem::circle

```cpp
static PathFunction circle(const glm::vec3& center, float radius, const glm::vec3& normal = glm::vec3(0, 1, 0));
```

중심·반지름·평면 법선으로 원 경로 함수를 만든다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 54행](../framework/include/Transform.h)

### PathSystem::polyline

```cpp
static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
```

점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 58행](../framework/include/Transform.h)

### PathSystem::polygon

```cpp
static PathFunction polygon(std::vector<glm::vec3> points);
```

마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 62행](../framework/include/Transform.h)

### PathSystem::rectangle

```cpp
static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU, const glm::vec3& halfV);
```

중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 66행](../framework/include/Transform.h)

### PathSystem::linear

```cpp
static double linear(double progress);
```

입력 진행률을 선형 보간 진행률로 반환한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 70행](../framework/include/Transform.h)

### PathSystem::smoothStep

```cpp
static double smoothStep(double progress);
```

진행률에 3t²-2t³ 완화 곡선을 적용한다.

반환·실패: 반환형과 위 기능 설명을 따른다. 이 호출만으로 Scene 표시나 시간 진행이 자동 발생한다고 가정하지 않는다.

[선언 74행](../framework/include/Transform.h)

