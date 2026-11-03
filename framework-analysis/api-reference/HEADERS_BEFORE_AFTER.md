# 헤더 변경 전후
공백·줄바꿈만 변경했다. 주석은 선언에 붙여 유지했다. 빌드·테스트·IDE hover 확인은 사용자에게 맡긴다.

## standard/Console.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::size_t selectedJoint_ = 0;
    int selectedAxis_ = 0;
    /// 선택한 관절의 기준 SRT와 피벗을 키 입력으로 편집한다.
    bool handleJointKey(int key);
    /// 현재 선택 관절과 조작 축을 콘솔에 출력한다.
    void printSelectedJoint() const;

    bool threeViews_ = false, trace_ = false, floorOpen_ = false;
    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();
    /// 회전하는 방 예제를 구성한다.
    bool initRoom();
    /// 선형보간·그네·사람 관절의 계층 애니메이션 예제를 구성한다.
    bool initAnimation();

    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);
    /// 반사 변환을 순서대로 변경한다.
    bool changeReflection();
    /// 크기 레이어를 삽입하거나 제거한다.
    bool toggleInsertedLayer();
    /// 조작 방법을 출력한다.
    void printHelp() const;

public:
```

### after
```cpp
    std::size_t selectedJoint_ = 0;
    int selectedAxis_ = 0;


    /// 선택한 관절의 기준 SRT와 피벗을 키 입력으로 편집한다.
    bool handleJointKey(int key);


    /// 현재 선택 관절과 조작 축을 콘솔에 출력한다.
    void printSelectedJoint() const;


    bool threeViews_ = false, trace_ = false, floorOpen_ = false;


    /// 거리 기반 격자 예제를 구성한다.
    bool initGrid();


    /// 회전하는 방 예제를 구성한다.
    bool initRoom();


    /// 선형보간·그네·사람 관절의 계층 애니메이션 예제를 구성한다.
    bool initAnimation();


    /// 선택한 경로로 이동 모션을 바꾼다.
    bool choosePath(int kind);


    /// 반사 변환을 순서대로 변경한다.
    bool changeReflection();


    /// 크기 레이어를 삽입하거나 제거한다.
    bool toggleInsertedLayer();


    /// 조작 방법을 출력한다.
    void printHelp() const;


public:
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();
    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action);
    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);
    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);
    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
```

### after
```cpp
    }


    /// 장면 객체와 초기 상태를 구성한다.
    bool initScene();


    /// 키 입력에 따라 동작을 변경한다.
    void onKey(int key, int action);


    /// 마우스 버튼 입력을 처리한다.
    void onMouse(const graphics::MouseEvent& event);


    /// 프레임 시간으로 상태와 장면을 갱신한다.
    void onUpdate(float dt);


    /// 현재 장면을 지정한 뷰로 렌더링한다.
    bool onRender();
```

## framework/detail/Internal.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
namespace graphics::detail {
std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction);
std::optional<glm::mat4> motionAround(const glm::vec3& center, const glm::mat4& matrix);
bool motionPlane(const glm::vec3& u, const glm::vec3& v);
bool positiveLengths(const glm::vec3& value);
std::uint64_t nextGeometryRevision();

void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color);

bool rigidRoom(const glm::mat4& matrix);
} // namespace graphics::detail
```

### after
```cpp
namespace graphics::detail {
std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction);


std::optional<glm::mat4> motionAround(const glm::vec3& center, const glm::mat4& matrix);


bool motionPlane(const glm::vec3& u, const glm::vec3& v);


bool positiveLengths(const glm::vec3& value);


std::uint64_t nextGeometryRevision();


void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color);


bool rigidRoom(const glm::mat4& matrix);
} // namespace graphics::detail
```

## framework/detail/Native.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
static_assert(sizeof(void*) == 8, "Bundled DLLs require x64.");

inline std::filesystem::path executableDirectory() {
    std::wstring path(32768, L'\0');
```

### after
```cpp
static_assert(sizeof(void*) == 8, "Bundled DLLs require x64.");


inline std::filesystem::path executableDirectory() {
    std::wstring path(32768, L'\0');
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return std::filesystem::path(path).parent_path();
}

inline std::filesystem::path relativePathInside(const std::filesystem::path& root,
```

### after
```cpp
    return std::filesystem::path(path).parent_path();
}


inline std::filesystem::path relativePathInside(const std::filesystem::path& root,
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline std::filesystem::path frameworkDirectory() {
    return executableDirectory() / "framework";
}

class Module {
```

### after
```cpp
}


inline std::filesystem::path frameworkDirectory() {
    return executableDirectory() / "framework";
}


class Module {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
public:
    Module() = default;
    Module(const Module&) = delete;

    Module& operator=(const Module&) = delete;

    ~Module() {
```

### after
```cpp
public:
    Module() = default;


    Module(const Module&) = delete;


    Module& operator=(const Module&) = delete;


    ~Module() {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
            FreeLibrary(handle_);
    }

    void open(const std::filesystem::path& path) {
```

### after
```cpp
            FreeLibrary(handle_);
    }


    void open(const std::filesystem::path& path) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        }
    }

    template <class T> T symbol(const char* name) const {
```

### after
```cpp
        }
    }


    template <class T> T symbol(const char* name) const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    decltype(&::glfwSwapBuffers) SwapBuffers = nullptr;

    void load(const std::filesystem::path& root = std::filesystem::path{}) {
        if (ready)
```

### after
```cpp
    decltype(&::glfwSwapBuffers) SwapBuffers = nullptr;


    void load(const std::filesystem::path& root = std::filesystem::path{}) {
        if (ready)
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return api;
}

struct GlApi {
```

### after
```cpp
    return api;
}


struct GlApi {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;

    template <class T> T resolve(const char* name) {
        PROC p = wglGetProc(name);
```

### after
```cpp
    PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;


    template <class T> T resolve(const char* name) {
        PROC p = wglGetProc(name);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        return reinterpret_cast<T>(p);
    }

    void load() {
```

### after
```cpp
        return reinterpret_cast<T>(p);
    }


    void load() {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline bool checkGlAt(const char* file, int line, PipelineStage stage, const char* api) {
    bool success = true;
```

### after
```cpp
}


inline bool checkGlAt(const char* file, int line, PipelineStage stage, const char* api) {
    bool success = true;
```

## framework/include/Actions.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
public:
    static MotionAction transform(Scene& scene, EntityId id, Transform3D value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setTransform(id, value); };
    }
    static MotionAction position(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setPosition(id, value); };
    }
    static MotionAction scale(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component) return false;
            auto transform = component->transform;
            transform.scale = value;
```

### after
```cpp
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
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction rotation(Scene& scene, EntityId id, glm::mat3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component) return false;
            auto transform = component->transform;
            transform.rotation = value;
```

### after
```cpp
        };
    }


    static MotionAction rotation(Scene& scene, EntityId id, glm::mat3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component)
                return false;
            auto transform = component->transform;
            transform.rotation = value;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction tint(Scene& scene, EntityId id, GlColor value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setTint(id, value); };
    }
    static MotionAction pivot(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
```

### after
```cpp
        };
    }


    static MotionAction tint(Scene& scene, EntityId id, GlColor value) {
        return [&scene, id, value](double, const MotionSample&) {
            return scene.setTint(id, value);
        };
    }


    static MotionAction pivot(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction visible(Scene& scene, EntityId id, bool value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setVisible(id, value); };
    }
    static MotionAction faceVisible(Scene& scene, EntityId id, std::size_t face, bool value) {
        return [&scene, id, face, value](double, const MotionSample&) { return scene.setFaceVisible(id, face, value); };
    }
    static MotionAction faceRotation(Scene& scene, EntityId id, std::size_t face, FaceRotationConfig config) {
        return [&scene, id, face, config](double, const MotionSample&) {
```

### after
```cpp
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
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction faceMode(Scene& scene, EntityId id, std::size_t face, FaceTransformMode mode,
                                 bool preserveWorld = true) {
```

### after
```cpp
        };
    }


    static MotionAction faceMode(Scene& scene, EntityId id, std::size_t face, FaceTransformMode mode,
                                 bool preserveWorld = true) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction layer(Scene& scene, EntityId id, std::optional<LayerId> value) {
        return [&scene, id, value](double, const MotionSample&) {
```

### after
```cpp
        };
    }


    static MotionAction layer(Scene& scene, EntityId id, std::optional<LayerId> value) {
        return [&scene, id, value](double, const MotionSample&) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction motion(Scene& scene, LayerId layer, std::optional<MotionId> value) {
        return [&scene, layer, value](double, const MotionSample&) { return scene.getMotions().bindMotion(layer, value); };
    }
    static MotionAction parent(Scene& scene, EntityId id, std::optional<EntityId> value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setParent(id, value); };
    }
    static MotionAction camera(Camera& camera, Camera value) {
        return [&camera, value](double, const MotionSample&) {
            if (!CameraSystem::validCamera(value)) return false;
            camera = value; return true;
        };
    }
```

### after
```cpp
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
```

## framework/include/Animation.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::array<glm::vec3, 2> hips{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> knees{glm::vec3(0), glm::vec3(0)};
    /// [API] 허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.
    static HumanoidPose seated();
```

### after
```cpp
    std::array<glm::vec3, 2> hips{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> knees{glm::vec3(0), glm::vec3(0)};


    /// [API] 허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.
    static HumanoidPose seated();
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 관절의 현재 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율만 허용한다.
    static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);
    /// [API] 기준 SRT·피벗과 강체 모션을 합성해 모두 유효할 때 반영한다. 부모 계층과 균일 배율을 유지한다.
    static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);
    /// [API] 실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.
    static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{},
                                                  std::optional<EntityId> parent = std::nullopt);
    /// [API] 관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.
    static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);
    /// [API] 고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.
    static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{},
```

### after
```cpp
    /// [API] 관절의 현재 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율만 허용한다.
    static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);


    /// [API] 기준 SRT·피벗과 강체 모션을 합성해 모두 유효할 때 반영한다. 부모 계층과 균일 배율을 유지한다.
    static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);


    /// [API] 실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.
    static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{},
                                                  std::optional<EntityId> parent = std::nullopt);


    /// [API] 관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.
    static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);


    /// [API] 고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.
    static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{},
```

## framework/include/Application.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    void* nativeContext_ = nullptr;

    bool ensureContext(const char* operation, const char* file, int line);
    Scene scene_;
    Camera camera_;
```

### after
```cpp
    void* nativeContext_ = nullptr;


    bool ensureContext(const char* operation, const char* file, int line);


    Scene scene_;
    Camera camera_;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::function<void(Application&, const MouseEvent&)> mouseHandler_;
    std::function<void(Application&, double, double)> cursorHandler_;

    template <class Handler> void dispatch(Handler&& handler) noexcept {
```

### after
```cpp
    std::function<void(Application&, const MouseEvent&)> mouseHandler_;
    std::function<void(Application&, double, double)> cursorHandler_;


    template <class Handler> void dispatch(Handler&& handler) noexcept {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
```

### after
```cpp
    }


    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::function<bool(Application&)> renderHandler_;

    bool initialize(const ApplicationConfig& config);

    void shutdown();

public:
    /// [API] 초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.
    Application() = default;
    /// [API] 실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.
    ~Application();
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application(const Application&) = delete;
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application& operator=(const Application&) = delete;
    /// [API] 장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.
    void setInitHandler(std::function<bool(Application&)> handler);
    /// [API] 매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.
    void setUpdateHandler(std::function<void(Application&, float)> handler);
    /// [API] 키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);
    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    /// [API] 키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.
    void setModifiedKeyHandler(std::function<void(Application&, int key, int action, int mods)> handler);
    /// [API] 창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);
    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    /// [API] 사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);
    /// [API] 마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.
    void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);
    /// [API] 창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.
    void setCursorHandler(std::function<void(Application&, double x, double y)> handler);
    /// [API] 창 좌표를 framebuffer 좌표로 보정하고 기본 카메라의 전체 viewport로 ray picking을 수행한다.
    std::optional<PickResult> pickAt(double windowX, double windowY) const;
    /// [API] 현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.
    std::optional<PickResult> pickAtCursor() const;
    /// [API] 실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.
    AssetLoader& getAssets(); // Available from initialization callback until run() ends.
    /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.
    int run(const ApplicationConfig& config = ApplicationConfig{});
    /// [API] 현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.
    bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {},
                           const std::filesystem::path& fragmentPath = {});
    /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
    void requestClose();
    /// [API] 기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.
    bool renderScene();
    /// [API] 현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.
    bool renderViews(const std::vector<RenderPass>& passes);
    /// [API] 현재 창의 실제 픽셀 크기를 반환한다. DPI 배율이 있으면 창 좌표 크기와 다를 수 있다.
    std::array<int, 2> getFramebufferSize() const;

    /// [API] 애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.
```

### after
```cpp
    std::function<bool(Application&)> renderHandler_;


    bool initialize(const ApplicationConfig& config);


    void shutdown();


public:
    /// [API] 초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.
    Application() = default;


    /// [API] 실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.
    ~Application();


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application(const Application&) = delete;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application& operator=(const Application&) = delete;


    /// [API] 장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.
    void setInitHandler(std::function<bool(Application&)> handler);


    /// [API] 매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.
    void setUpdateHandler(std::function<void(Application&, float)> handler);


    /// [API] 키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);


    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    /// [API] 키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.
    void setModifiedKeyHandler(std::function<void(Application&, int key, int action, int mods)> handler);


    /// [API] 창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);


    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    /// [API] 사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);


    /// [API] 마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.
    void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);


    /// [API] 창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.
    void setCursorHandler(std::function<void(Application&, double x, double y)> handler);


    /// [API] 창 좌표를 framebuffer 좌표로 보정하고 기본 카메라의 전체 viewport로 ray picking을 수행한다.
    std::optional<PickResult> pickAt(double windowX, double windowY) const;


    /// [API] 현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.
    std::optional<PickResult> pickAtCursor() const;


    /// [API] 실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.
    AssetLoader& getAssets(); // Available from initialization callback until run() ends.


    /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.
    int run(const ApplicationConfig& config = ApplicationConfig{});


    /// [API] 현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.
    bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {},
                           const std::filesystem::path& fragmentPath = {});


    /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
    void requestClose();


    /// [API] 기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.
    bool renderScene();


    /// [API] 현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.
    bool renderViews(const std::vector<RenderPass>& passes);


    /// [API] 현재 창의 실제 픽셀 크기를 반환한다. DPI 배율이 있으면 창 좌표 크기와 다를 수 있다.
    std::array<int, 2> getFramebufferSize() const;


    /// [API] 애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.
    Scene& getScene() {
```

### after
```cpp
    }


    /// [API] 애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.
    Scene& getScene() {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 기본 카메라의 수정 가능한 참조를 반환한다.
    Camera& getCamera() {
        return camera_;
    }

    /// [API] GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.
```

### after
```cpp
    }


    /// [API] 기본 카메라의 수정 가능한 참조를 반환한다.
    Camera& getCamera() {
        return camera_;
    }


    /// [API] GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.
```

## framework/include/CuboidMode.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.
    bool setConfig(const CuboidModeConfig& config);
    const CuboidModeConfig& getConfig() const { return config_; }
    /// [API] 같은 이름의 육면체 초기 설정을 교체하거나 새로 등록한다.
    bool setCuboid(const CuboidConfig& config);
    /// [API] 같은 key/action/mods 바인딩을 교체하거나 추가한다.
    bool bindKey(const CuboidKeyBinding& binding);
    bool unbindKey(int key, int action = GLFW_PRESS, int mods = 0);
    const std::vector<CuboidKeyBinding>& getBindings() const { return bindings_; }
    void setSetupHandler(CuboidHandler handler) { setup_ = std::move(handler); }
    void setUpdateHandler(std::function<bool(Application&, CuboidMode&, float)> handler) { update_ = std::move(handler); }
    /// [API] 별도 CPU Scene에 육면체와 모션을 구성하고 성공하면 장면 전체를 교체한다.
    bool apply(Application& app);
    bool update(Application& app, float dt);
    bool dispatch(Application& app, int key, int action, int mods);
    std::optional<EntityId> entity(const std::string& name) const;
    std::optional<LayerId> layer(const std::string& name) const;
    std::optional<MotionId> motion(const std::string& name) const;
    /// [API] 실행 시 이름을 해석하므로 초기 설정 재적용 뒤에도 같은 바인딩을 사용한다.
    static CuboidHandler action(std::string name, CuboidCommand command);
```

### after
```cpp
    /// [API] 초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.
    bool setConfig(const CuboidModeConfig& config);


    const CuboidModeConfig& getConfig() const {
        return config_;
    }


    /// [API] 같은 이름의 육면체 초기 설정을 교체하거나 새로 등록한다.
    bool setCuboid(const CuboidConfig& config);


    /// [API] 같은 key/action/mods 바인딩을 교체하거나 추가한다.
    bool bindKey(const CuboidKeyBinding& binding);


    bool unbindKey(int key, int action = GLFW_PRESS, int mods = 0);


    const std::vector<CuboidKeyBinding>& getBindings() const {
        return bindings_;
    }


    void setSetupHandler(CuboidHandler handler) {
        setup_ = std::move(handler);
    }


    void setUpdateHandler(std::function<bool(Application&, CuboidMode&, float)> handler) {
        update_ = std::move(handler);
    }


    /// [API] 별도 CPU Scene에 육면체와 모션을 구성하고 성공하면 장면 전체를 교체한다.
    bool apply(Application& app);


    bool update(Application& app, float dt);


    bool dispatch(Application& app, int key, int action, int mods);


    std::optional<EntityId> entity(const std::string& name) const;


    std::optional<LayerId> layer(const std::string& name) const;


    std::optional<MotionId> motion(const std::string& name) const;


    /// [API] 실행 시 이름을 해석하므로 초기 설정 재적용 뒤에도 같은 바인딩을 사용한다.
    static CuboidHandler action(std::string name, CuboidCommand command);
```

## framework/include/Diagnostics.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    static std::optional<Diagnostic>& issue() {
        static std::optional<Diagnostic> value;
```

### after
```cpp
    }


    static std::optional<Diagnostic>& issue() {
        static std::optional<Diagnostic> value;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        return value;
    }

public:
```

### after
```cpp
        return value;
    }


public:
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.
    static void locate(const char* file, int line, const char* operation) {
```

### after
```cpp
    }


    /// [API] 다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.
    static void locate(const char* file, int line, const char* operation) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        context().operation = operation;
    }

    /// [API] 주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.
```

### after
```cpp
        context().operation = operation;
    }


    /// [API] 주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.
    static const char* stageName(PipelineStage stage) {
        switch (stage) {
        case PipelineStage::Runtime:
            return "runtime";
        case PipelineStage::Input:
```

### after
```cpp
    }


    /// [API] 파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.
    static const char* stageName(PipelineStage stage) {
        switch (stage) {
            case PipelineStage::Runtime:
            return "runtime";
        case PipelineStage::Input:
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 오류 출력과 단계 추적 출력의 사용 여부를 설정한다.
    static void configure(DiagnosticOptions value) {
        options() = value;
    }

    /// [API] 마지막으로 기록한 오류의 optional 참조를 반환한다.
```

### after
```cpp
    }


    /// [API] 오류 출력과 단계 추적 출력의 사용 여부를 설정한다.
    static void configure(DiagnosticOptions value) {
        options() = value;
    }


    /// [API] 마지막으로 기록한 오류의 optional 참조를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.
    static void clear() {
        issue().reset();
    }

    /// [API] 추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.
```

### after
```cpp
    }


    /// [API] 마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.
    static void clear() {
        issue().reset();
    }


    /// [API] 추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
            std::clog << "[OpenGL][" << stageName(stage) << "][" << api << "] 시작\n";
    }

    /// [API] 현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.
```

### after
```cpp
            std::clog << "[OpenGL][" << stageName(stage) << "][" << api << "] 시작\n";
    }


    /// [API] 현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.
```

## framework/include/Geometry.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

/// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
inline bool generateTexcoords(Mesh& mesh, const UVOptions& options = UVOptions{}) {
```

### after
```cpp
}


/// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
inline bool generateTexcoords(Mesh& mesh, const UVOptions& options = UVOptions{}) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp

    switch (options.mapping) {
    case UVMapping::Planar:
    case UVMapping::Box:
    case UVMapping::Cylindrical:
```

### after
```cpp

    switch (options.mapping) {
        case UVMapping::Planar:
    case UVMapping::Box:
    case UVMapping::Cylindrical:
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

/// [API] 메시의 삼각형 기하로 정점 법선을 다시 계산한다.
inline void recalculateNormals(Mesh& mesh) {
```

### after
```cpp
}


/// [API] 메시의 삼각형 기하로 정점 법선을 다시 계산한다.
inline void recalculateNormals(Mesh& mesh) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }
}

// Weld equal positions for edge topology, while preserving separate UV/normal vertices.
```

### after
```cpp
    }
}


// Weld equal positions for edge topology, while preserving separate UV/normal vertices.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return result;
}

namespace detail {
```

### after
```cpp
    return result;
}


namespace detail {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first = std::uint32_t(destination.localVertices.size());
```

### after
```cpp
}


inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first = std::uint32_t(destination.localVertices.size());
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    destination.shape = GLFace::Mesh;
}

inline bool shareEdge(const Mesh& a, const Mesh& b) {
```

### after
```cpp
    destination.shape = GLFace::Mesh;
}


inline bool shareEdge(const Mesh& a, const Mesh& b) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return false;
}

inline void mergeCoplanar(Model& model) {
```

### after
```cpp
    return false;
}


inline void mergeCoplanar(Model& model) {
```

## framework/include/Graphics.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::map<EntityId, LayerId> entityLayers_;

    void rebuildFaceIds();

public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();
    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});
    /// [API] 반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32,
                                      unsigned stacks = 16);
    /// [API] 반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);
    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);
    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});
    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);
    /// [API] 세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);
    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);
    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);
    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);
    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);
    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);
    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);
    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);
    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.
    bool setParent(EntityId id, std::optional<EntityId> parent);
    /// [API] 부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    /// [API] 엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;

    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
```

### after
```cpp
    std::map<EntityId, LayerId> entityLayers_;


    void rebuildFaceIds();


public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();


    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});


    /// [API] 반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32,
                                      unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);


    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);


    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});


    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);


    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);


    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);


    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);


    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);


    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);


    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);


    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);


    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.
    bool setParent(EntityId id, std::optional<EntityId> parent);


    /// [API] 부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;


    /// [API] 엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;


    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const MotionSystem& getMotions() const {
```

### after
```cpp
    }


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const MotionSystem& getMotions() const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);
    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);
    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;

    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
```

### after
```cpp
    }


    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);


    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);


    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;


    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);
    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);

    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
```

### after
```cpp
    }


    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);


    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);


    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));
    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);
    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);
    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;
    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);
    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);
    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);
    /// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});
    /// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                            float rotationDegrees = 0);
    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);
    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);
    /// [API] 메시의 색상·텍스처 등 재질 데이터를 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);
    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});
    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));
    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);
    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);
    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);

    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);
    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
```

### after
```cpp
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);


    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);


    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;


    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);


    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);


    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);


    /// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});


    /// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                            float rotationDegrees = 0);


    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);


    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);


    /// [API] 메시의 색상·텍스처 등 재질 데이터를 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);


    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});


    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);


    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);


    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);


    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);


    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);
    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);
    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.

    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
```

### after
```cpp
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);


    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.


    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;

    /// [API] 직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.
```

### after
```cpp
    }


    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;


    /// [API] 직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 사면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getTetraFaceIds() const {
        return tetraFaceId_;
    }

    /// [API] 피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.
```

### after
```cpp
    }


    /// [API] 사면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getTetraFaceIds() const {
        return tetraFaceId_;
    }


    /// [API] 피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const {
        return lines_;
    }

    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
```

### after
```cpp
    }


    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const {
        return lines_;
    }


    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));
    /// [API] 반지름·분할 수로 CPU 구 메시 모델을 생성한다.
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);
    /// [API] 반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.
    static Model makeCone(float radius, float height, unsigned slices = 32);
    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);
    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);
    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);
    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
```

### after
```cpp
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));


    /// [API] 반지름·분할 수로 CPU 구 메시 모델을 생성한다.
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.
    static Model makeCone(float radius, float height, unsigned slices = 32);


    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);


    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);


    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);


    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);
    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);
    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
```

### after
```cpp
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);


    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);


    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);

struct RenderOptions {
```

### after
```cpp
/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);


struct RenderOptions {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;
    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    bool upload(const Scene& scene, bool textures);
    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;

public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer(const Renderer&) = delete;
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer& operator=(const Renderer&) = delete;
    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
```

### after
```cpp
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;


    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);


    bool upload(const Scene& scene, bool textures);


    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;


public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer(const Renderer&) = delete;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer& operator=(const Renderer&) = delete;


    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});
    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});
    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);
    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);
    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy(); // Call while the owning context is current, before it is destroyed.

    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.
```

### after
```cpp
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});


    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});


    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);


    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);


    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy(); // Call while the owning context is current, before it is destroyed.


    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.
```

## framework/include/GridLayout.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    glm::mat4 frame{1};

    /// [API] origin·extent·셀 개수와 frame의 유한성·양수 크기·가역성을 검사한다.
    bool valid() const {
```

### after
```cpp
    glm::mat4 frame{1};


    /// [API] origin·extent·셀 개수와 frame의 유한성·양수 크기·가역성을 검사한다.
    bool valid() const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
               std::isfinite(determinant) && std::abs(determinant) > 1e-8f;
    }

    /// [API] [0,1] 비율을 origin+extent*ratio로 바꾼 뒤 frame으로 월드 위치를 계산한다.
```

### after
```cpp
               std::isfinite(determinant) && std::abs(determinant) > 1e-8f;
    }


    /// [API] [0,1] 비율을 origin+extent*ratio로 바꾼 뒤 frame으로 월드 위치를 계산한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 0부터 시작하는 셀 인덱스를 해당 셀 중심의 월드 위치로 변환한다.
    std::optional<glm::vec3> cellCenter(const glm::ivec3& index) const {
```

### after
```cpp
    }


    /// [API] 0부터 시작하는 셀 인덱스를 해당 셀 중심의 월드 위치로 변환한다.
    std::optional<glm::vec3> cellCenter(const glm::ivec3& index) const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        return position((glm::vec3(index) + glm::vec3(.5f)) / glm::vec3(cells));
    }

    /// [API] 양수 비율과 extent를 곱해 그리드 로컬 크기를 반환한다. frame 회전/배율은 적용하지 않는다.
```

### after
```cpp
        return position((glm::vec3(index) + glm::vec3(.5f)) / glm::vec3(cells));
    }


    /// [API] 양수 비율과 extent를 곱해 그리드 로컬 크기를 반환한다. frame 회전/배율은 적용하지 않는다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        }

        return result; // Local dimensions. frame orientation is kept separate.
    }

    /// [API] 비율 위치를 월드로 계산한 뒤 parentWorld의 역행렬로 부모 로컬 위치를 반환한다.
```

### after
```cpp
        }

        return result;
        // Local dimensions. frame orientation is kept separate.
    }


    /// [API] 비율 위치를 월드로 계산한 뒤 parentWorld의 역행렬로 부모 로컬 위치를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 월드 점을 frame 역변환으로 그리드 셀 인덱스에 대응시킨다. 그리드 밖이면 빈 결과다.
    std::optional<glm::ivec3> cellAt(const glm::vec3& world) const {
```

### after
```cpp
    }


    /// [API] 월드 점을 frame 역변환으로 그리드 셀 인덱스에 대응시킨다. 그리드 밖이면 빈 결과다.
    std::optional<glm::ivec3> cellAt(const glm::vec3& world) const {
```

## framework/include/Model.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::vector<std::uint8_t> rgba;

    // Row zero maps to UV v=0. File decoding flips top-down images by default.
    /// [API] 이미지 너비·높이와 RGBA 바이트 수가 맞는지 검사한다.
```

### after
```cpp
    std::vector<std::uint8_t> rgba;


    // Row zero maps to UV v=0. File decoding flips top-down images by default.
    /// [API] 이미지 너비·높이와 RGBA 바이트 수가 맞는지 검사한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    FaceAnimationSpace animationSpace = FaceAnimationSpace::Local;

    /// [API] 정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& triangles) {
```

### after
```cpp
    FaceAnimationSpace animationSpace = FaceAnimationSpace::Local;


    /// [API] 정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& triangles) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::string name;

    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    std::vector<Mesh>& meshes() {
        return faces;
    }

    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
```

### after
```cpp
    std::string name;


    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    std::vector<Mesh>& meshes() {
        return faces;
    }


    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c)
```

### after
```cpp
}


inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c)
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}

inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
```

### after
```cpp
}


inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}


inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return {};
}

inline bool validMesh(const Mesh& m) {
```

### after
```cpp
    return {};
}


inline bool validMesh(const Mesh& m) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());
```

### after
```cpp
}


inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);
```

### after
```cpp
}


inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return {box.first.x * .5f + box.second.x * .5f, box.first.y, box.first.z * .5f + box.second.z * .5f};
}

inline glm::mat4 transformMatrix(const Transform3D& t) {
```

### after
```cpp
    return {box.first.x * .5f + box.second.x * .5f, box.first.y, box.first.z * .5f + box.second.z * .5f};
}


inline glm::mat4 transformMatrix(const Transform3D& t) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline glm::mat4 faceSrtMatrix(const Mesh& face) {
    return face.independence
```

### after
```cpp
}


inline glm::mat4 faceSrtMatrix(const Mesh& face) {
    return face.independence
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline glm::mat4 faceLocalMatrix(const Mesh& face) {
    const auto child = faceSrtMatrix(face);
```

### after
```cpp
}


inline glm::mat4 faceLocalMatrix(const Mesh& face) {
    const auto child = faceSrtMatrix(face);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        ? face.localAnimationMatrix * child : child * face.localAnimationMatrix;
}

inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
```

### after
```cpp
        ? face.localAnimationMatrix * child : child * face.localAnimationMatrix;
}


inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
```

## framework/include/ModelLoader.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    bool usedFallback = false;

    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
    explicit operator bool() const {
```

### after
```cpp
    bool usedFallback = false;


    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
    explicit operator bool() const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::shared_ptr<const TextureImage> image;
    std::string error;

    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
```

### after
```cpp
    std::shared_ptr<const TextureImage> image;
    std::string error;


    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    mutable std::map<std::filesystem::path, std::weak_ptr<const TextureImage>> imageCache_;

    std::filesystem::path resolve(const std::filesystem::path& relative) const {
        if (relative.empty() || relative.is_absolute() || relative.has_root_name())
```

### after
```cpp
    mutable std::map<std::filesystem::path, std::weak_ptr<const TextureImage>> imageCache_;


    std::filesystem::path resolve(const std::filesystem::path& relative) const {
        if (relative.empty() || relative.is_absolute() || relative.has_root_name())
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        return full;
    }

    std::shared_ptr<const TextureImage> texture(const std::filesystem::path& relative, bool flip) const {
```

### after
```cpp
        return full;
    }


    std::shared_ptr<const TextureImage> texture(const std::filesystem::path& relative, bool flip) const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

public:
    /// [API] 프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.
```

### after
```cpp
    }


public:
    /// [API] 프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.
    const FeatureOptions& features() const {
        return features_;
    }

    /// [API] 프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.
```

### after
```cpp
    }


    /// [API] 로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.
    const FeatureOptions& features() const {
        return features_;
    }


    /// [API] 프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.
    ModelResult loadObj(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
    /// [API] 기본 도형 OBJ를 읽는 호환 진입점이다. OBJ 로더로 처리를 위임한다.
    ModelResult loadPrimitive(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
    /// [API] 모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.
    ModelResult loadModel(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
```

### after
```cpp
    }


    /// [API] 프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.
    ModelResult loadObj(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;


    /// [API] 기본 도형 OBJ를 읽는 호환 진입점이다. OBJ 로더로 처리를 위임한다.
    ModelResult loadPrimitive(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;


    /// [API] 모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.
    ModelResult loadModel(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
```

## framework/include/Motion.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
    std::optional<MotionSample> evaluate(double seconds, bool actions) const;

public:
    /// [API] 모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.
    bool append(const MotionStep& step);
    /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
    bool insert(std::size_t index, const MotionStep& step);
    /// [API] 변환 사이의 인덱스에 의미 동작 또는 람다·함수 포인터를 삽입한다.
    bool insertAction(std::size_t index, MotionAction action, double startSeconds = 0,
                      std::optional<double> durationSeconds = std::nullopt);
    /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
    bool replace(std::size_t index, const MotionStep& step);
    /// [API] 주어진 인덱스의 모션 단계를 삭제한다.
    bool erase(std::size_t index);
    /// [API] 모든 앞 단계의 유한 종료 시각을 구한 뒤 새 단계의 시작을 그 뒤로 이동해 순차 실행을 만든다.
    bool then(MotionStep step); // Requires all previous steps to have finite durations.

    /// [API] 등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.
```

### after
```cpp
class Motion {
    std::vector<MotionStep> steps_;


    static bool validStep(const MotionStep& step);


    std::optional<MotionSample> evaluate(double seconds, bool actions) const;


public:
    /// [API] 모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.
    bool append(const MotionStep& step);


    /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
    bool insert(std::size_t index, const MotionStep& step);


    /// [API] 변환 사이의 인덱스에 의미 동작 또는 람다·함수 포인터를 삽입한다.
    bool insertAction(std::size_t index, MotionAction action, double startSeconds = 0,
                      std::optional<double> durationSeconds = std::nullopt);


    /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
    bool replace(std::size_t index, const MotionStep& step);


    /// [API] 주어진 인덱스의 모션 단계를 삭제한다.
    bool erase(std::size_t index);


    /// [API] 모든 앞 단계의 유한 종료 시각을 구한 뒤 새 단계의 시작을 그 뒤로 이동해 순차 실행을 만든다.
    bool then(MotionStep step); // Requires all previous steps to have finite durations.


    /// [API] 등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다. 의미 동작은 실행하지 않는다.
    std::optional<MotionSample> sample(double seconds) const;
    /// [API] 변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.
    std::optional<MotionSample> execute(double seconds) const;
    /// [API] 기존 append/insert/then/replace와 조립하는 비변환 동작 단계를 만든다.
    static MotionStep action(MotionAction action, double startSeconds = 0,
                             std::optional<double> durationSeconds = std::nullopt);
    /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
    static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);
    /// [API] 축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.
    static MotionStep rotation(const RotationMotion& config);
    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);
    /// [API] 시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.
    static MotionStep linear(const LinearMotion& config);
    /// [API] 지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.
    static MotionStep pendulum(const PendulumMotion& config);
```

### after
```cpp
    }


    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다. 의미 동작은 실행하지 않는다.
    std::optional<MotionSample> sample(double seconds) const;


    /// [API] 변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.
    std::optional<MotionSample> execute(double seconds) const;


    /// [API] 기존 append/insert/then/replace와 조립하는 비변환 동작 단계를 만든다.
    static MotionStep action(MotionAction action, double startSeconds = 0,
                             std::optional<double> durationSeconds = std::nullopt);


    /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
    static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);


    /// [API] 축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.
    static MotionStep rotation(const RotationMotion& config);


    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);


    /// [API] 시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.
    static MotionStep linear(const LinearMotion& config);


    /// [API] 지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.
    static MotionStep pendulum(const PendulumMotion& config);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;

public:
```

### after
```cpp
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;


    bool validLayer(const LayerConfig& config) const;


public:
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
    std::optional<MotionId> createMotion(const Motion& motion);
    /// [API] 의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.
    std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);
    /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
    bool setMotion(MotionId id, const Motion& motion);
    /// [API] 모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const MotionState* getMotion(MotionId id) const;
    /// [API] 모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.
    bool setMotionPlaying(MotionId id, bool playing);
    /// [API] 모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.
    bool setMotionEnabled(MotionId id, bool enabled);
    /// [API] 지정 시각을 샘플링하고 유효할 때 모션의 시간과 샘플을 함께 바꾼다.
    bool seekMotion(MotionId id, double seconds);
    /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
    bool removeMotion(MotionId id);
    /// [API] 모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.
    bool update(double deltaSeconds);
    /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
    void clear(); // Identifiers are never reused by this system, including after clear.
    /// [API] 유효한 부모 아래 새 자식 레이어를 추가한다. 복사한 임시 맵에 반영한 뒤 swap하여 상태를 확정한다.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    /// [API] 기존 child와 그 parent 사이에 새 레이어를 끼운다. root 위 삽입은 허용하지 않는다.
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    /// [API] 자식이 없는 비root leaf만 제거하고 그 부모 ID를 반환한다.
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    /// [API] root 이외 레이어를 제거하며 자식들을 부모의 해당 위치로 승격한다. 월드 자세 보존을 자동 약속하지 않는다.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    /// [API] 레이어의 부모를 바꾸며 자신의 자손으로 연결되는 순환을 거부한다.
    bool reparentLayer(LayerId id, LayerId parent);
    /// [API] 기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.
    bool setLayer(LayerId id, const LayerConfig& config);
    /// [API] 레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const LayerState* getLayer(LayerId id) const;
    /// [API] 기존 레이어에 모션 ID를 연결하거나 해제한다. 모션과 레이어가 존재하는지 검사한다.
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    /// [API] leaf까지의 계층을 따라 transform과 motion 샘플을 World/Local 공간에 맞게 parent/local 행렬로 합성한다.
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    /// [API] 로컬 공간 회전 모션을 등록하여 자전에 사용한다.
    std::optional<MotionId> createSpin(RotationMotion config);
    /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
    std::optional<MotionId> createOrbit(RotationMotion config);
```

### after
```cpp
    }


    /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
    std::optional<MotionId> createMotion(const Motion& motion);


    /// [API] 의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.
    std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);


    /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
    bool setMotion(MotionId id, const Motion& motion);


    /// [API] 모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const MotionState* getMotion(MotionId id) const;


    /// [API] 모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.
    bool setMotionPlaying(MotionId id, bool playing);


    /// [API] 모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.
    bool setMotionEnabled(MotionId id, bool enabled);


    /// [API] 지정 시각을 샘플링하고 유효할 때 모션의 시간과 샘플을 함께 바꾼다.
    bool seekMotion(MotionId id, double seconds);


    /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
    bool removeMotion(MotionId id);


    /// [API] 모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.
    bool update(double deltaSeconds);


    /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
    void clear(); // Identifiers are never reused by this system, including after clear.


    /// [API] 유효한 부모 아래 새 자식 레이어를 추가한다. 복사한 임시 맵에 반영한 뒤 swap하여 상태를 확정한다.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});


    /// [API] 기존 child와 그 parent 사이에 새 레이어를 끼운다. root 위 삽입은 허용하지 않는다.
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});


    /// [API] 자식이 없는 비root leaf만 제거하고 그 부모 ID를 반환한다.
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.


    /// [API] root 이외 레이어를 제거하며 자식들을 부모의 해당 위치로 승격한다. 월드 자세 보존을 자동 약속하지 않는다.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.


    /// [API] 레이어의 부모를 바꾸며 자신의 자손으로 연결되는 순환을 거부한다.
    bool reparentLayer(LayerId id, LayerId parent);


    /// [API] 기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.
    bool setLayer(LayerId id, const LayerConfig& config);


    /// [API] 레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const LayerState* getLayer(LayerId id) const;


    /// [API] 기존 레이어에 모션 ID를 연결하거나 해제한다. 모션과 레이어가 존재하는지 검사한다.
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);


    /// [API] leaf까지의 계층을 따라 transform과 motion 샘플을 World/Local 공간에 맞게 parent/local 행렬로 합성한다.
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;


    /// [API] 로컬 공간 회전 모션을 등록하여 자전에 사용한다.
    std::optional<MotionId> createSpin(RotationMotion config);


    /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
    std::optional<MotionId> createOrbit(RotationMotion config);
```

## framework/include/Patterns.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
struct Bounds3 {
    glm::vec3 minimum{-1}, maximum{1};
    /// [API] 해당 데이터가 계산에 필요한 유효 범위와 유한성 조건을 만족하는지 반환한다.
    bool valid() const;
    /// [API] 점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.
    bool contains(const glm::vec3& point, const glm::vec3& margin = glm::vec3(0)) const;
    /// [API] 두 축 정렬 경계 상자가 겹치는지 검사한다.
    bool overlaps(const Bounds3& other) const;
```

### after
```cpp
struct Bounds3 {
    glm::vec3 minimum{-1}, maximum{1};


    /// [API] 해당 데이터가 계산에 필요한 유효 범위와 유한성 조건을 만족하는지 반환한다.
    bool valid() const;


    /// [API] 점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.
    bool contains(const glm::vec3& point, const glm::vec3& margin = glm::vec3(0)) const;


    /// [API] 두 축 정렬 경계 상자가 겹치는지 검사한다.
    bool overlaps(const Bounds3& other) const;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    bool setPoints(const std::vector<glm::vec3>& points, bool closed = false);

    /// [API] 경로 전체 누적 길이를 반환한다. 경로가 비어 있으면 0이다.
    double length() const {
        return distances_.empty() ? 0 : distances_.back();
    }

    /// [API] 누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.
```

### after
```cpp
    bool setPoints(const std::vector<glm::vec3>& points, bool closed = false);


    /// [API] 경로 전체 누적 길이를 반환한다. 경로가 비어 있으면 0이다.
    double length() const {
        return distances_.empty() ? 0 : distances_.back();
    }


    /// [API] 누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    double seconds = 0;
    bool playing = true;
    /// [API] playing일 때만 누적 시간을 늘린다. dt와 누적 값의 유한성·범위를 검사한다.
    bool advance(double dt);

    /// [API] 누적 시간을 0으로 되돌리고 재생 상태를 켠다.
```

### after
```cpp
    double seconds = 0;
    bool playing = true;


    /// [API] playing일 때만 누적 시간을 늘린다. dt와 누적 값의 유한성·범위를 검사한다.
    bool advance(double dt);


    /// [API] 누적 시간을 0으로 되돌리고 재생 상태를 켠다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    glm::vec3 worldPosition{0}, worldVelocity{0};
    bool released = false;

    /// [API] 이탈 전에는 roomWorld로 로컬 중심을 월드로 변환하고 이탈 후에는 worldPosition을 반환한다.
```

### after
```cpp
    glm::vec3 worldPosition{0}, worldVelocity{0};
    bool released = false;


    /// [API] 이탈 전에는 roomWorld로 로컬 중심을 월드로 변환하고 이탈 후에는 worldPosition을 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
                             bool elastic, const glm::vec3& gravity = glm::vec3(0, -9.8f, 0),
                             const glm::vec3& angularVelocity = glm::vec3(0));
    /// [API] 접지 상태에서만 위쪽 속도를 설정한다. 공중 입력 거절은 정상적인 false다.
    static bool jump(CharacterBody& body, float speed);
    /// [API] 수평 이동·장애물/경계 반전·중력·윗면 착지·지지면 이탈을 하위 단계로 계산한다.
    static std::optional<CharacterStep> stepCharacter(CharacterBody& body, const Bounds3& stage,
                                                      const std::vector<Bounds3>& obstacles, float dt,
                                                      float gravity = 9.8f);
    /// [API] 제한 횟수 안에서 occupied와 겹치지 않는 배치를 찾는다. 난수 엔진의 시드·수명은 호출자가 관리한다.
    static std::optional<Bounds3> randomFreeBox(const Bounds3& area, const glm::vec3& size,
```

### after
```cpp
                             bool elastic, const glm::vec3& gravity = glm::vec3(0, -9.8f, 0),
                             const glm::vec3& angularVelocity = glm::vec3(0));


    /// [API] 접지 상태에서만 위쪽 속도를 설정한다. 공중 입력 거절은 정상적인 false다.
    static bool jump(CharacterBody& body, float speed);


    /// [API] 수평 이동·장애물/경계 반전·중력·윗면 착지·지지면 이탈을 하위 단계로 계산한다.
    static std::optional<CharacterStep> stepCharacter(CharacterBody& body, const Bounds3& stage,
                                                      const std::vector<Bounds3>& obstacles, float dt,
                                                      float gravity = 9.8f);


    /// [API] 제한 횟수 안에서 occupied와 겹치지 않는 배치를 찾는다. 난수 엔진의 시드·수명은 호출자가 관리한다.
    static std::optional<Bounds3> randomFreeBox(const Bounds3& area, const glm::vec3& size,
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    static std::optional<PathPose> conveyorCornerPose(double distance, const glm::vec3& start, const glm::vec3& pivot,
                                                      float radius, float endZ);
    /// [API] 양수 크기의 중심 기준 모델을 만들고 부모 로컬 위치·색·부모 관계를 설정한다.
    static std::optional<EntityId> addBox(Scene& scene, const glm::vec3& size, const glm::vec3& position, GlColor color,
                                          std::optional<EntityId> parent = std::nullopt);
    /// [API] 점 목록으로 단일 다각형 면을 만들고 색·부모 관계와 함께 Scene에 등록한다. 오목 다각형의 자동 분할을 가정하지 않는다.
    static std::optional<EntityId> addPolygon(Scene& scene, const std::vector<glm::vec3>& points, GlColor color,
                                              std::optional<EntityId> parent = std::nullopt);
    /// [API] 여섯 면의 방을 만들고 바깥 winding에 Front cull, 안쪽 조명 법선을 적용한다. face 0..3은 옆벽, 4 천장, 5 바닥이다.
    static std::optional<EntityId> addRoom(Scene& scene, const Bounds3& bounds, bool frontOpen = true);
    /// [API] 부모에 부착된 transform-only 그룹을 만든다. 표시 메시 크기와 회전 연결 위치를 분리한다.
    static std::optional<EntityId> addJoint(Scene& scene, EntityId parent, const glm::vec3& anchor);
    /// [API] 연결 위치와 크기를 유지하면서 회전 기저를 변경한다.
    static bool rotateJoint(Scene& scene, EntityId joint, const glm::vec3& axis, float degrees);
    /// [API] DistancePath 샘플을 엔티티 위치와 +Z 기준 Y 회전에 반영한다.
    static bool follow(Scene& scene, EntityId entity, const DistancePath& path, double distance,
                       const glm::vec3& offset = glm::vec3(0), bool orient = true);
    /// [API] 0~9를 선분으로 구성한다. 부모 로컬 XY 평면의 표시이며 일반 폰트/텍스트 렌더러는 아니다.
    static bool addNumber(Scene& scene, unsigned digit, const glm::vec3& position, float height, GlColor color,
                          std::optional<EntityId> parent = std::nullopt);
    /// [API] 지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.
    static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);
    /// [API] 원래 모서리 힌지·면 중심 자전·원본 모델 중심 공전을 구분하여 면 회전 행렬을 설정한다.
    static bool rotateFace(Scene& scene, EntityId entity, std::size_t face, const FaceRotationConfig& config);
    /// [API] 진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.
    static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement,
                         const glm::vec3& topPivot, bool scaleOpen);
    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);
    /// [API] 선택 옆벽의 원래 중심을 기준으로 모든 축을 균등 축소/복원하며 완전 개방 시 숨긴다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
    /// [API] 두 값의 차이가 허용 오차 이하인지 검사한다.
    static bool aligned(float first, float second, float tolerance);
    /// [API] 좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.
    static glm::vec3 snap(const glm::vec3& point, float spacing);
    /// [API] 화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.
    static std::optional<glm::vec3> screenOnPlane(const Camera& camera, double x, double y,
                                                  const PickViewport& viewport, const glm::vec3& planePoint,
                                                  const glm::vec3& normal);
    /// [API] 시점과 주목점을 동일하게 이동해 시선 방향을 보존한다.
    static bool moveCamera(Camera& camera, const glm::vec3& offset);
    /// [API] 시점은 고정하고 카메라 로컬 up축을 기준으로 시선을 회전한다.
    static bool yawCamera(Camera& camera, float degrees);
    /// [API] 월드 축에 대해 시점·주목점·up을 함께 변환한다. 축을 target에 두면 target은 고정된다.
    static bool orbitCamera(Camera& camera, const AxisLine& axis, float degrees);
    /// [API] 원근은 target까지 거리, 직각은 orthographicHalfExtent를 배율로 바꾼다.
    static bool zoomCamera(Camera& camera, float factor);
    /// [API] 원근·XZ 평면도·XY 정면도 카메라와 픽셀 영역을 구성한다. 상단 뷰의 up은 -Z다.
    static std::vector<RenderPass> threeViews(const Camera& perspective, int width, int height,
                                              const RenderOptions& options = RenderOptions{});
    /// [API] 그리드 비율로 머리·코·몸통·어깨·엉덩이·팔·다리 계층을 조립하고 ID 묶음을 반환한다.
    static std::optional<JointRig> addWalkingRig(Scene& scene, const GridLayout& bodyGrid);
    /// [API] 루트 이동/yaw와 실제 이동 거리 기반 팔다리 반대 위상을 적용한다. 속도는 보행 진폭에 반영한다.
    static bool poseWalkingRig(Scene& scene, const JointRig& rig, const glm::vec3& feet, float yawDegrees,
```

### after
```cpp
    static std::optional<PathPose> conveyorCornerPose(double distance, const glm::vec3& start, const glm::vec3& pivot,
                                                      float radius, float endZ);


    /// [API] 양수 크기의 중심 기준 모델을 만들고 부모 로컬 위치·색·부모 관계를 설정한다.
    static std::optional<EntityId> addBox(Scene& scene, const glm::vec3& size, const glm::vec3& position, GlColor color,
                                          std::optional<EntityId> parent = std::nullopt);


    /// [API] 점 목록으로 단일 다각형 면을 만들고 색·부모 관계와 함께 Scene에 등록한다. 오목 다각형의 자동 분할을 가정하지 않는다.
    static std::optional<EntityId> addPolygon(Scene& scene, const std::vector<glm::vec3>& points, GlColor color,
                                              std::optional<EntityId> parent = std::nullopt);


    /// [API] 여섯 면의 방을 만들고 바깥 winding에 Front cull, 안쪽 조명 법선을 적용한다. face 0..3은 옆벽, 4 천장, 5 바닥이다.
    static std::optional<EntityId> addRoom(Scene& scene, const Bounds3& bounds, bool frontOpen = true);


    /// [API] 부모에 부착된 transform-only 그룹을 만든다. 표시 메시 크기와 회전 연결 위치를 분리한다.
    static std::optional<EntityId> addJoint(Scene& scene, EntityId parent, const glm::vec3& anchor);


    /// [API] 연결 위치와 크기를 유지하면서 회전 기저를 변경한다.
    static bool rotateJoint(Scene& scene, EntityId joint, const glm::vec3& axis, float degrees);


    /// [API] DistancePath 샘플을 엔티티 위치와 +Z 기준 Y 회전에 반영한다.
    static bool follow(Scene& scene, EntityId entity, const DistancePath& path, double distance,
                       const glm::vec3& offset = glm::vec3(0), bool orient = true);


    /// [API] 0~9를 선분으로 구성한다. 부모 로컬 XY 평면의 표시이며 일반 폰트/텍스트 렌더러는 아니다.
    static bool addNumber(Scene& scene, unsigned digit, const glm::vec3& position, float height, GlColor color,
                          std::optional<EntityId> parent = std::nullopt);


    /// [API] 지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.
    static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);


    /// [API] 원래 모서리 힌지·면 중심 자전·원본 모델 중심 공전을 구분하여 면 회전 행렬을 설정한다.
    static bool rotateFace(Scene& scene, EntityId entity, std::size_t face, const FaceRotationConfig& config);


    /// [API] 진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.
    static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement,
                         const glm::vec3& topPivot, bool scaleOpen);


    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);


    /// [API] 선택 옆벽의 원래 중심을 기준으로 모든 축을 균등 축소/복원하며 완전 개방 시 숨긴다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);


    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);


    /// [API] 두 값의 차이가 허용 오차 이하인지 검사한다.
    static bool aligned(float first, float second, float tolerance);


    /// [API] 좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.
    static glm::vec3 snap(const glm::vec3& point, float spacing);


    /// [API] 화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.
    static std::optional<glm::vec3> screenOnPlane(const Camera& camera, double x, double y,
                                                  const PickViewport& viewport, const glm::vec3& planePoint,
                                                  const glm::vec3& normal);


    /// [API] 시점과 주목점을 동일하게 이동해 시선 방향을 보존한다.
    static bool moveCamera(Camera& camera, const glm::vec3& offset);


    /// [API] 시점은 고정하고 카메라 로컬 up축을 기준으로 시선을 회전한다.
    static bool yawCamera(Camera& camera, float degrees);


    /// [API] 월드 축에 대해 시점·주목점·up을 함께 변환한다. 축을 target에 두면 target은 고정된다.
    static bool orbitCamera(Camera& camera, const AxisLine& axis, float degrees);


    /// [API] 원근은 target까지 거리, 직각은 orthographicHalfExtent를 배율로 바꾼다.
    static bool zoomCamera(Camera& camera, float factor);


    /// [API] 원근·XZ 평면도·XY 정면도 카메라와 픽셀 영역을 구성한다. 상단 뷰의 up은 -Z다.
    static std::vector<RenderPass> threeViews(const Camera& perspective, int width, int height,
                                              const RenderOptions& options = RenderOptions{});


    /// [API] 그리드 비율로 머리·코·몸통·어깨·엉덩이·팔·다리 계층을 조립하고 ID 묶음을 반환한다.
    static std::optional<JointRig> addWalkingRig(Scene& scene, const GridLayout& bodyGrid);


    /// [API] 루트 이동/yaw와 실제 이동 거리 기반 팔다리 반대 위상을 적용한다. 속도는 보행 진폭에 반영한다.
    static bool poseWalkingRig(Scene& scene, const JointRig& rig, const glm::vec3& feet, float yawDegrees,
```

## framework/include/Picking.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    double x = 0, y = 0, width = 0, height = 0;
}; // top-left framebuffer pixels

/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
```

### after
```cpp
    double x = 0, y = 0, width = 0, height = 0;
}; // top-left framebuffer pixels


/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return Ray{glm::vec3(nearPoint), direction / length, length};
}

/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
```

### after
```cpp
    return Ray{glm::vec3(nearPoint), direction / length, length};
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
```

### after
```cpp
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
```

## framework/include/RenderConfig.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    glm::mat4 model, view, projection;
    bool depthOnly = false;
    /// [API] 현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.
    bool setUniform(const std::string& name, float value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !std::isfinite(value)) return false;
        detail::gl().Uniform1f(location, value); return true;
    }
    /// [API] 현재 program의 추가 int/bool/sampler uniform을 설정한다.
    bool setUniform(const std::string& name, int value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0) return false;
        detail::gl().Uniform1i(location, value); return true;
    }
    /// [API] 현재 program의 추가 vec3 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::vec3& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteVector(value)) return false;
        detail::gl().Uniform3f(location, value.x, value.y, value.z); return true;
    }
    /// [API] 현재 program의 추가 mat4 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::mat4& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteMatrix(value)) return false;
        detail::gl().UniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]); return true;
    }
};
```

### after
```cpp
    glm::mat4 model, view, projection;
    bool depthOnly = false;


    /// [API] 현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.
    bool setUniform(const std::string& name, float value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !std::isfinite(value))
            return false;
        detail::gl().Uniform1f(location, value);
        return true;
    }


    /// [API] 현재 program의 추가 int/bool/sampler uniform을 설정한다.
    bool setUniform(const std::string& name, int value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0)
            return false;
        detail::gl().Uniform1i(location, value);
        return true;
    }


    /// [API] 현재 program의 추가 vec3 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::vec3& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteVector(value))
            return false;
        detail::gl().Uniform3f(location, value.x, value.y, value.z);
        return true;
    }


    /// [API] 현재 program의 추가 mat4 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::mat4& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteMatrix(value))
            return false;
        detail::gl().UniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
        return true;
    }
};
```

## framework/include/Texture.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    T* value = nullptr;

    ~ComOwner() {
        if (value)
```

### after
```cpp
    T* value = nullptr;


    ~ComOwner() {
        if (value)
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    ComOwner() = default;
    ComOwner(const ComOwner&) = delete;

    ComOwner& operator=(const ComOwner&) = delete;
};

inline void checkCom(HRESULT result) {
```

### after
```cpp
    }


    ComOwner() = default;


    ComOwner(const ComOwner&) = delete;


    ComOwner& operator=(const ComOwner&) = delete;
};


inline void checkCom(HRESULT result) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

struct ComApartment {
    HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    ComApartment() {
```

### after
```cpp
}


struct ComApartment {
    HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);


    ComApartment() {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
            checkCom(result);
    }

    ~ComApartment() {
```

### after
```cpp
            checkCom(result);
    }


    ~ComApartment() {
```

## framework/include/Transform.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.
    static bool validAffine(const glm::mat4& matrix);
    /// [API] 이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    /// [API] 중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.
    static std::optional<glm::mat4> scaling(const glm::vec3& factors, const glm::vec3& center = glm::vec3(0));
    /// [API] 직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    /// [API] 주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    /// [API] 주어진 직선을 기준으로 선 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    /// [API] 평면의 점과 법선으로 평면 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point, const glm::vec3& normal);
```

### after
```cpp
    /// [API] 행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.
    static bool validAffine(const glm::mat4& matrix);


    /// [API] 이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.
    static std::optional<glm::mat4> translation(const glm::vec3& offset);


    /// [API] 중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.
    static std::optional<glm::mat4> scaling(const glm::vec3& factors, const glm::vec3& center = glm::vec3(0));


    /// [API] 직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);


    /// [API] 주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);


    /// [API] 주어진 직선을 기준으로 선 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);


    /// [API] 평면의 점과 법선으로 평면 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point, const glm::vec3& normal);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU, const glm::vec3& axisV);
    /// [API] 중심·반지름·평면 법선으로 원 경로 함수를 만든다.
    static PathFunction circle(const glm::vec3& center, float radius, const glm::vec3& normal = glm::vec3(0, 1, 0));
    /// [API] 점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    /// [API] 마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.
    static PathFunction polygon(std::vector<glm::vec3> points);
    /// [API] 중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU, const glm::vec3& halfV);
    /// [API] 입력 진행률을 선형 보간 진행률로 반환한다.
    static double linear(double progress);
    /// [API] 진행률에 3t²-2t³ 완화 곡선을 적용한다.
    static double smoothStep(double progress);
```

### after
```cpp
    /// [API] 중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU, const glm::vec3& axisV);


    /// [API] 중심·반지름·평면 법선으로 원 경로 함수를 만든다.
    static PathFunction circle(const glm::vec3& center, float radius, const glm::vec3& normal = glm::vec3(0, 1, 0));


    /// [API] 점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);


    /// [API] 마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.
    static PathFunction polygon(std::vector<glm::vec3> points);


    /// [API] 중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU, const glm::vec3& halfV);


    /// [API] 입력 진행률을 선형 보간 진행률로 반환한다.
    static double linear(double progress);


    /// [API] 진행률에 3t²-2t³ 완화 곡선을 적용한다.
    static double smoothStep(double progress);
```

## standard/framework/detail/Internal.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
namespace graphics::detail {
std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction);
std::optional<glm::mat4> motionAround(const glm::vec3& center, const glm::mat4& matrix);
bool motionPlane(const glm::vec3& u, const glm::vec3& v);
bool positiveLengths(const glm::vec3& value);
std::uint64_t nextGeometryRevision();

void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color);

bool rigidRoom(const glm::mat4& matrix);
} // namespace graphics::detail
```

### after
```cpp
namespace graphics::detail {
std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction);


std::optional<glm::mat4> motionAround(const glm::vec3& center, const glm::mat4& matrix);


bool motionPlane(const glm::vec3& u, const glm::vec3& v);


bool positiveLengths(const glm::vec3& value);


std::uint64_t nextGeometryRevision();


void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color);


bool rigidRoom(const glm::mat4& matrix);
} // namespace graphics::detail
```

## standard/framework/detail/Native.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
static_assert(sizeof(void*) == 8, "Bundled DLLs require x64.");

inline std::filesystem::path executableDirectory() {
    std::wstring path(32768, L'\0');
```

### after
```cpp
static_assert(sizeof(void*) == 8, "Bundled DLLs require x64.");


inline std::filesystem::path executableDirectory() {
    std::wstring path(32768, L'\0');
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return std::filesystem::path(path).parent_path();
}

inline std::filesystem::path relativePathInside(const std::filesystem::path& root,
```

### after
```cpp
    return std::filesystem::path(path).parent_path();
}


inline std::filesystem::path relativePathInside(const std::filesystem::path& root,
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline std::filesystem::path frameworkDirectory() {
    return executableDirectory() / "framework";
}

class Module {
```

### after
```cpp
}


inline std::filesystem::path frameworkDirectory() {
    return executableDirectory() / "framework";
}


class Module {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
public:
    Module() = default;
    Module(const Module&) = delete;

    Module& operator=(const Module&) = delete;

    ~Module() {
```

### after
```cpp
public:
    Module() = default;


    Module(const Module&) = delete;


    Module& operator=(const Module&) = delete;


    ~Module() {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
            FreeLibrary(handle_);
    }

    void open(const std::filesystem::path& path) {
```

### after
```cpp
            FreeLibrary(handle_);
    }


    void open(const std::filesystem::path& path) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        }
    }

    template <class T> T symbol(const char* name) const {
```

### after
```cpp
        }
    }


    template <class T> T symbol(const char* name) const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    decltype(&::glfwSwapBuffers) SwapBuffers = nullptr;

    void load(const std::filesystem::path& root = std::filesystem::path{}) {
        if (ready)
```

### after
```cpp
    decltype(&::glfwSwapBuffers) SwapBuffers = nullptr;


    void load(const std::filesystem::path& root = std::filesystem::path{}) {
        if (ready)
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return api;
}

struct GlApi {
```

### after
```cpp
    return api;
}


struct GlApi {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;

    template <class T> T resolve(const char* name) {
        PROC p = wglGetProc(name);
```

### after
```cpp
    PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;


    template <class T> T resolve(const char* name) {
        PROC p = wglGetProc(name);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        return reinterpret_cast<T>(p);
    }

    void load() {
```

### after
```cpp
        return reinterpret_cast<T>(p);
    }


    void load() {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline bool checkGlAt(const char* file, int line, PipelineStage stage, const char* api) {
    bool success = true;
```

### after
```cpp
}


inline bool checkGlAt(const char* file, int line, PipelineStage stage, const char* api) {
    bool success = true;
```

## standard/framework/include/Actions.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
public:
    static MotionAction transform(Scene& scene, EntityId id, Transform3D value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setTransform(id, value); };
    }
    static MotionAction position(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setPosition(id, value); };
    }
    static MotionAction scale(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component) return false;
            auto transform = component->transform;
            transform.scale = value;
```

### after
```cpp
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
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction rotation(Scene& scene, EntityId id, glm::mat3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component) return false;
            auto transform = component->transform;
            transform.rotation = value;
```

### after
```cpp
        };
    }


    static MotionAction rotation(Scene& scene, EntityId id, glm::mat3 value) {
        return [&scene, id, value](double, const MotionSample&) {
            const auto* component = scene.getComponent(id);
            if (!component)
                return false;
            auto transform = component->transform;
            transform.rotation = value;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction tint(Scene& scene, EntityId id, GlColor value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setTint(id, value); };
    }
    static MotionAction pivot(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
```

### after
```cpp
        };
    }


    static MotionAction tint(Scene& scene, EntityId id, GlColor value) {
        return [&scene, id, value](double, const MotionSample&) {
            return scene.setTint(id, value);
        };
    }


    static MotionAction pivot(Scene& scene, EntityId id, glm::vec3 value) {
        return [&scene, id, value](double, const MotionSample&) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction visible(Scene& scene, EntityId id, bool value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setVisible(id, value); };
    }
    static MotionAction faceVisible(Scene& scene, EntityId id, std::size_t face, bool value) {
        return [&scene, id, face, value](double, const MotionSample&) { return scene.setFaceVisible(id, face, value); };
    }
    static MotionAction faceRotation(Scene& scene, EntityId id, std::size_t face, FaceRotationConfig config) {
        return [&scene, id, face, config](double, const MotionSample&) {
```

### after
```cpp
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
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction faceMode(Scene& scene, EntityId id, std::size_t face, FaceTransformMode mode,
                                 bool preserveWorld = true) {
```

### after
```cpp
        };
    }


    static MotionAction faceMode(Scene& scene, EntityId id, std::size_t face, FaceTransformMode mode,
                                 bool preserveWorld = true) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction layer(Scene& scene, EntityId id, std::optional<LayerId> value) {
        return [&scene, id, value](double, const MotionSample&) {
```

### after
```cpp
        };
    }


    static MotionAction layer(Scene& scene, EntityId id, std::optional<LayerId> value) {
        return [&scene, id, value](double, const MotionSample&) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        };
    }
    static MotionAction motion(Scene& scene, LayerId layer, std::optional<MotionId> value) {
        return [&scene, layer, value](double, const MotionSample&) { return scene.getMotions().bindMotion(layer, value); };
    }
    static MotionAction parent(Scene& scene, EntityId id, std::optional<EntityId> value) {
        return [&scene, id, value](double, const MotionSample&) { return scene.setParent(id, value); };
    }
    static MotionAction camera(Camera& camera, Camera value) {
        return [&camera, value](double, const MotionSample&) {
            if (!CameraSystem::validCamera(value)) return false;
            camera = value; return true;
        };
    }
```

### after
```cpp
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
```

## standard/framework/include/Animation.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::array<glm::vec3, 2> hips{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> knees{glm::vec3(0), glm::vec3(0)};
    /// [API] 허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.
    static HumanoidPose seated();
```

### after
```cpp
    std::array<glm::vec3, 2> hips{glm::vec3(0), glm::vec3(0)};
    std::array<glm::vec3, 2> knees{glm::vec3(0), glm::vec3(0)};


    /// [API] 허벅지를 앞으로 뻗고 무릎을 굽힌 앉은 자세를 반환한다. 회전 단위는 도다.
    static HumanoidPose seated();
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 관절의 현재 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율만 허용한다.
    static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);
    /// [API] 기준 SRT·피벗과 강체 모션을 합성해 모두 유효할 때 반영한다. 부모 계층과 균일 배율을 유지한다.
    static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);
    /// [API] 실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.
    static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{},
                                                  std::optional<EntityId> parent = std::nullopt);
    /// [API] 관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.
    static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);
    /// [API] 고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.
    static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{},
```

### after
```cpp
    /// [API] 관절의 현재 SRT와 강체 모션을 보관한다. 기준 배율은 양수 균일 배율만 허용한다.
    static std::optional<JointAnimation> createJointAnimation(const Scene& scene, EntityId joint, const Motion& motion);


    /// [API] 기준 SRT·피벗과 강체 모션을 합성해 모두 유효할 때 반영한다. 부모 계층과 균일 배율을 유지한다.
    static bool poseHierarchy(Scene& scene, const std::vector<JointAnimation>& animation, double seconds);


    /// [API] 실습 23 비율의 사람형을 만든다. 부모가 있으면 grid.frame은 부모 로컬 변환이다. 실패 시 Scene을 보존한다.
    static std::optional<HumanoidRig> addHumanoid(Scene& scene, const HumanoidConfig& config = HumanoidConfig{},
                                                  std::optional<EntityId> parent = std::nullopt);


    /// [API] 관절 위치를 유지하며 XYZ 순서의 로컬 Euler 회전을 적용한다. 오류가 있으면 전체 자세를 보존한다.
    static bool poseHumanoid(Scene& scene, const HumanoidRig& rig, const HumanoidPose& pose);


    /// [API] 고정 프레임→진자 관절→좌석→앉은 사람 계층과 진자·상체 모션을 만든다. 실패 시 Scene을 보존한다.
    static std::optional<SwingRig> addSwingWithRider(Scene& scene, const SwingConfig& config = SwingConfig{},
```

## standard/framework/include/Application.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    void* nativeContext_ = nullptr;

    bool ensureContext(const char* operation, const char* file, int line);
    Scene scene_;
    Camera camera_;
```

### after
```cpp
    void* nativeContext_ = nullptr;


    bool ensureContext(const char* operation, const char* file, int line);


    Scene scene_;
    Camera camera_;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::function<void(Application&, const MouseEvent&)> mouseHandler_;
    std::function<void(Application&, double, double)> cursorHandler_;

    template <class Handler> void dispatch(Handler&& handler) noexcept {
```

### after
```cpp
    std::function<void(Application&, const MouseEvent&)> mouseHandler_;
    std::function<void(Application&, double, double)> cursorHandler_;


    template <class Handler> void dispatch(Handler&& handler) noexcept {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
```

### after
```cpp
    }


    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::function<bool(Application&)> renderHandler_;

    bool initialize(const ApplicationConfig& config);

    void shutdown();

public:
    /// [API] 초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.
    Application() = default;
    /// [API] 실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.
    ~Application();
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application(const Application&) = delete;
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application& operator=(const Application&) = delete;
    /// [API] 장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.
    void setInitHandler(std::function<bool(Application&)> handler);
    /// [API] 매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.
    void setUpdateHandler(std::function<void(Application&, float)> handler);
    /// [API] 키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);
    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    /// [API] 키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.
    void setModifiedKeyHandler(std::function<void(Application&, int key, int action, int mods)> handler);
    /// [API] 창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);
    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    /// [API] 사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);
    /// [API] 마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.
    void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);
    /// [API] 창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.
    void setCursorHandler(std::function<void(Application&, double x, double y)> handler);
    /// [API] 창 좌표를 framebuffer 좌표로 보정하고 기본 카메라의 전체 viewport로 ray picking을 수행한다.
    std::optional<PickResult> pickAt(double windowX, double windowY) const;
    /// [API] 현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.
    std::optional<PickResult> pickAtCursor() const;
    /// [API] 실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.
    AssetLoader& getAssets(); // Available from initialization callback until run() ends.
    /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.
    int run(const ApplicationConfig& config = ApplicationConfig{});
    /// [API] 현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.
    bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {},
                           const std::filesystem::path& fragmentPath = {});
    /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
    void requestClose();
    /// [API] 기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.
    bool renderScene();
    /// [API] 현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.
    bool renderViews(const std::vector<RenderPass>& passes);
    /// [API] 현재 창의 실제 픽셀 크기를 반환한다. DPI 배율이 있으면 창 좌표 크기와 다를 수 있다.
    std::array<int, 2> getFramebufferSize() const;

    /// [API] 애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.
```

### after
```cpp
    std::function<bool(Application&)> renderHandler_;


    bool initialize(const ApplicationConfig& config);


    void shutdown();


public:
    /// [API] 초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.
    Application() = default;


    /// [API] 실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.
    ~Application();


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application(const Application&) = delete;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Application& operator=(const Application&) = delete;


    /// [API] 장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 실행을 종료한다.
    void setInitHandler(std::function<bool(Application&)> handler);


    /// [API] 매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.
    void setUpdateHandler(std::function<void(Application&, float)> handler);


    /// [API] 키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);


    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    /// [API] 키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.
    void setModifiedKeyHandler(std::function<void(Application&, int key, int action, int mods)> handler);


    /// [API] 창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);


    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    /// [API] 사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);


    /// [API] 마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.
    void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);


    /// [API] 창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.
    void setCursorHandler(std::function<void(Application&, double x, double y)> handler);


    /// [API] 창 좌표를 framebuffer 좌표로 보정하고 기본 카메라의 전체 viewport로 ray picking을 수행한다.
    std::optional<PickResult> pickAt(double windowX, double windowY) const;


    /// [API] 현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.
    std::optional<PickResult> pickAtCursor() const;


    /// [API] 실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.
    AssetLoader& getAssets(); // Available from initialization callback until run() ends.


    /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.
    int run(const ApplicationConfig& config = ApplicationConfig{});


    /// [API] 현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.
    bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {},
                           const std::filesystem::path& fragmentPath = {});


    /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
    void requestClose();


    /// [API] 기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.
    bool renderScene();


    /// [API] 현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.
    bool renderViews(const std::vector<RenderPass>& passes);


    /// [API] 현재 창의 실제 픽셀 크기를 반환한다. DPI 배율이 있으면 창 좌표 크기와 다를 수 있다.
    std::array<int, 2> getFramebufferSize() const;


    /// [API] 애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.
    Scene& getScene() {
```

### after
```cpp
    }


    /// [API] 애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.
    Scene& getScene() {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 기본 카메라의 수정 가능한 참조를 반환한다.
    Camera& getCamera() {
        return camera_;
    }

    /// [API] GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.
```

### after
```cpp
    }


    /// [API] 기본 카메라의 수정 가능한 참조를 반환한다.
    Camera& getCamera() {
        return camera_;
    }


    /// [API] GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.
```

## standard/framework/include/CuboidMode.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.
    bool setConfig(const CuboidModeConfig& config);
    const CuboidModeConfig& getConfig() const { return config_; }
    /// [API] 같은 이름의 육면체 초기 설정을 교체하거나 새로 등록한다.
    bool setCuboid(const CuboidConfig& config);
    /// [API] 같은 key/action/mods 바인딩을 교체하거나 추가한다.
    bool bindKey(const CuboidKeyBinding& binding);
    bool unbindKey(int key, int action = GLFW_PRESS, int mods = 0);
    const std::vector<CuboidKeyBinding>& getBindings() const { return bindings_; }
    void setSetupHandler(CuboidHandler handler) { setup_ = std::move(handler); }
    void setUpdateHandler(std::function<bool(Application&, CuboidMode&, float)> handler) { update_ = std::move(handler); }
    /// [API] 별도 CPU Scene에 육면체와 모션을 구성하고 성공하면 장면 전체를 교체한다.
    bool apply(Application& app);
    bool update(Application& app, float dt);
    bool dispatch(Application& app, int key, int action, int mods);
    std::optional<EntityId> entity(const std::string& name) const;
    std::optional<LayerId> layer(const std::string& name) const;
    std::optional<MotionId> motion(const std::string& name) const;
    /// [API] 실행 시 이름을 해석하므로 초기 설정 재적용 뒤에도 같은 바인딩을 사용한다.
    static CuboidHandler action(std::string name, CuboidCommand command);
```

### after
```cpp
    /// [API] 초기 설정만 교체한다. 현재 Scene은 apply를 호출할 때 갱신된다.
    bool setConfig(const CuboidModeConfig& config);


    const CuboidModeConfig& getConfig() const {
        return config_;
    }


    /// [API] 같은 이름의 육면체 초기 설정을 교체하거나 새로 등록한다.
    bool setCuboid(const CuboidConfig& config);


    /// [API] 같은 key/action/mods 바인딩을 교체하거나 추가한다.
    bool bindKey(const CuboidKeyBinding& binding);


    bool unbindKey(int key, int action = GLFW_PRESS, int mods = 0);


    const std::vector<CuboidKeyBinding>& getBindings() const {
        return bindings_;
    }


    void setSetupHandler(CuboidHandler handler) {
        setup_ = std::move(handler);
    }


    void setUpdateHandler(std::function<bool(Application&, CuboidMode&, float)> handler) {
        update_ = std::move(handler);
    }


    /// [API] 별도 CPU Scene에 육면체와 모션을 구성하고 성공하면 장면 전체를 교체한다.
    bool apply(Application& app);


    bool update(Application& app, float dt);


    bool dispatch(Application& app, int key, int action, int mods);


    std::optional<EntityId> entity(const std::string& name) const;


    std::optional<LayerId> layer(const std::string& name) const;


    std::optional<MotionId> motion(const std::string& name) const;


    /// [API] 실행 시 이름을 해석하므로 초기 설정 재적용 뒤에도 같은 바인딩을 사용한다.
    static CuboidHandler action(std::string name, CuboidCommand command);
```

## standard/framework/include/Diagnostics.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    static std::optional<Diagnostic>& issue() {
        static std::optional<Diagnostic> value;
```

### after
```cpp
    }


    static std::optional<Diagnostic>& issue() {
        static std::optional<Diagnostic> value;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        return value;
    }

public:
```

### after
```cpp
        return value;
    }


public:
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.
    static void locate(const char* file, int line, const char* operation) {
```

### after
```cpp
    }


    /// [API] 다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.
    static void locate(const char* file, int line, const char* operation) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        context().operation = operation;
    }

    /// [API] 주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.
```

### after
```cpp
        context().operation = operation;
    }


    /// [API] 주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.
    static const char* stageName(PipelineStage stage) {
        switch (stage) {
        case PipelineStage::Runtime:
            return "runtime";
        case PipelineStage::Input:
```

### after
```cpp
    }


    /// [API] 파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.
    static const char* stageName(PipelineStage stage) {
        switch (stage) {
            case PipelineStage::Runtime:
            return "runtime";
        case PipelineStage::Input:
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 오류 출력과 단계 추적 출력의 사용 여부를 설정한다.
    static void configure(DiagnosticOptions value) {
        options() = value;
    }

    /// [API] 마지막으로 기록한 오류의 optional 참조를 반환한다.
```

### after
```cpp
    }


    /// [API] 오류 출력과 단계 추적 출력의 사용 여부를 설정한다.
    static void configure(DiagnosticOptions value) {
        options() = value;
    }


    /// [API] 마지막으로 기록한 오류의 optional 참조를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.
    static void clear() {
        issue().reset();
    }

    /// [API] 추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.
```

### after
```cpp
    }


    /// [API] 마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.
    static void clear() {
        issue().reset();
    }


    /// [API] 추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
            std::clog << "[OpenGL][" << stageName(stage) << "][" << api << "] 시작\n";
    }

    /// [API] 현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.
```

### after
```cpp
            std::clog << "[OpenGL][" << stageName(stage) << "][" << api << "] 시작\n";
    }


    /// [API] 현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.
```

## standard/framework/include/Geometry.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

/// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
inline bool generateTexcoords(Mesh& mesh, const UVOptions& options = UVOptions{}) {
```

### after
```cpp
}


/// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
inline bool generateTexcoords(Mesh& mesh, const UVOptions& options = UVOptions{}) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp

    switch (options.mapping) {
    case UVMapping::Planar:
    case UVMapping::Box:
    case UVMapping::Cylindrical:
```

### after
```cpp

    switch (options.mapping) {
        case UVMapping::Planar:
    case UVMapping::Box:
    case UVMapping::Cylindrical:
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

/// [API] 메시의 삼각형 기하로 정점 법선을 다시 계산한다.
inline void recalculateNormals(Mesh& mesh) {
```

### after
```cpp
}


/// [API] 메시의 삼각형 기하로 정점 법선을 다시 계산한다.
inline void recalculateNormals(Mesh& mesh) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }
}

// Weld equal positions for edge topology, while preserving separate UV/normal vertices.
```

### after
```cpp
    }
}


// Weld equal positions for edge topology, while preserving separate UV/normal vertices.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return result;
}

namespace detail {
```

### after
```cpp
    return result;
}


namespace detail {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first = std::uint32_t(destination.localVertices.size());
```

### after
```cpp
}


inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first = std::uint32_t(destination.localVertices.size());
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    destination.shape = GLFace::Mesh;
}

inline bool shareEdge(const Mesh& a, const Mesh& b) {
```

### after
```cpp
    destination.shape = GLFace::Mesh;
}


inline bool shareEdge(const Mesh& a, const Mesh& b) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return false;
}

inline void mergeCoplanar(Model& model) {
```

### after
```cpp
    return false;
}


inline void mergeCoplanar(Model& model) {
```

## standard/framework/include/Graphics.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::map<EntityId, LayerId> entityLayers_;

    void rebuildFaceIds();

public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();
    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});
    /// [API] 반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32,
                                      unsigned stacks = 16);
    /// [API] 반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);
    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);
    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});
    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);
    /// [API] 세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);
    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);
    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);
    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);
    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);
    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);
    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);
    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);
    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.
    bool setParent(EntityId id, std::optional<EntityId> parent);
    /// [API] 부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    /// [API] 엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;

    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
```

### after
```cpp
    std::map<EntityId, LayerId> entityLayers_;


    void rebuildFaceIds();


public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();


    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});


    /// [API] 반지름과 분할 수로 구 모델을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addSphere(float radius, const glm::vec3& position, unsigned slices = 32,
                                      unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 원뿔을 생성하고 지정 위치에 등록한다.
    std::optional<EntityId> addCone(float radius, float height, const glm::vec3& position, unsigned slices = 32);


    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);


    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});


    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 세 축의 모서리 길이로 직각 사면체를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths, const glm::vec3& worldPosition);


    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);


    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);


    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);


    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);


    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);


    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);


    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);


    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);


    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.
    bool setParent(EntityId id, std::optional<EntityId> parent);


    /// [API] 부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;


    /// [API] 엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;


    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const MotionSystem& getMotions() const {
```

### after
```cpp
    }


    /// [API] Scene이 소유한 MotionSystem의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    const MotionSystem& getMotions() const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);
    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);
    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;

    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
```

### after
```cpp
    }


    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);


    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);


    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;


    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);
    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);

    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
```

### after
```cpp
    }


    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);


    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);


    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));
    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);
    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);
    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;
    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);
    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);
    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);
    /// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});
    /// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                            float rotationDegrees = 0);
    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);
    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);
    /// [API] 메시의 색상·텍스처 등 재질 데이터를 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);
    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});
    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));
    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);
    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);
    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);

    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);
    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
```

### after
```cpp
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);


    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);


    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;


    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);


    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);


    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);


    /// [API] 평면·박스·원통·구 투영 옵션으로 메시 UV를 생성한다.
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});


    /// [API] 메시 UV에 배율→회전→이동을 적용한다. 회전 입력 단위는 도다.
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale, const glm::vec2& offset,
                            float rotationDegrees = 0);


    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);


    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);


    /// [API] 메시의 색상·텍스처 등 재질 데이터를 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);


    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});


    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));


    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);


    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);


    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);


    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);


    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);
    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);
    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.

    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
```

### after
```cpp
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);


    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);


    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.


    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;

    /// [API] 직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.
```

### after
```cpp
    }


    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;


    /// [API] 직육면체 면 ID 목록을 읽기 전용 참조로 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 사면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getTetraFaceIds() const {
        return tetraFaceId_;
    }

    /// [API] 피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.
```

### after
```cpp
    }


    /// [API] 사면체 면 ID 목록을 읽기 전용 참조로 반환한다.
    const std::vector<FaceId>& getTetraFaceIds() const {
        return tetraFaceId_;
    }


    /// [API] 피라미드 면 ID 목록을 읽기 전용 참조로 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const {
        return lines_;
    }

    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
```

### after
```cpp
    }


    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const {
        return lines_;
    }


    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));
    /// [API] 반지름·분할 수로 CPU 구 메시 모델을 생성한다.
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);
    /// [API] 반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.
    static Model makeCone(float radius, float height, unsigned slices = 32);
    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);
    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);
    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);
    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
```

### after
```cpp
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));


    /// [API] 반지름·분할 수로 CPU 구 메시 모델을 생성한다.
    static Model makeSphere(float radius, unsigned slices = 32, unsigned stacks = 16);


    /// [API] 반지름·높이·분할 수로 CPU 원뿔 모델을 생성한다.
    static Model makeCone(float radius, float height, unsigned slices = 32);


    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);


    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);


    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);


    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);
    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);
    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
```

### after
```cpp
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);


    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);


    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);

struct RenderOptions {
```

### after
```cpp
/// [API] 지정 경로의 파일을 문자열로 읽는다. 읽기에 실패하면 예외를 전달한다.
std::string readTextFile(const std::filesystem::path& path);


struct RenderOptions {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;
    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    bool upload(const Scene& scene, bool textures);
    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;

public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer(const Renderer&) = delete;
    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer& operator=(const Renderer&) = delete;
    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
```

### after
```cpp
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;


    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);


    bool upload(const Scene& scene, bool textures);


    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;


public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer(const Renderer&) = delete;


    /// [API] 이 자원 소유 타입의 복사 생성/대입을 금지한다. 창·GPU 핸들을 중복 소유하지 않게 한다.
    Renderer& operator=(const Renderer&) = delete;


    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});
    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});
    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);
    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);
    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy(); // Call while the owning context is current, before it is destroyed.

    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.
```

### after
```cpp
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});


    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});


    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);


    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);


    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy(); // Call while the owning context is current, before it is destroyed.


    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.
```

## standard/framework/include/GridLayout.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    glm::mat4 frame{1};

    /// [API] origin·extent·셀 개수와 frame의 유한성·양수 크기·가역성을 검사한다.
    bool valid() const {
```

### after
```cpp
    glm::mat4 frame{1};


    /// [API] origin·extent·셀 개수와 frame의 유한성·양수 크기·가역성을 검사한다.
    bool valid() const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
               std::isfinite(determinant) && std::abs(determinant) > 1e-8f;
    }

    /// [API] [0,1] 비율을 origin+extent*ratio로 바꾼 뒤 frame으로 월드 위치를 계산한다.
```

### after
```cpp
               std::isfinite(determinant) && std::abs(determinant) > 1e-8f;
    }


    /// [API] [0,1] 비율을 origin+extent*ratio로 바꾼 뒤 frame으로 월드 위치를 계산한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 0부터 시작하는 셀 인덱스를 해당 셀 중심의 월드 위치로 변환한다.
    std::optional<glm::vec3> cellCenter(const glm::ivec3& index) const {
```

### after
```cpp
    }


    /// [API] 0부터 시작하는 셀 인덱스를 해당 셀 중심의 월드 위치로 변환한다.
    std::optional<glm::vec3> cellCenter(const glm::ivec3& index) const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        return position((glm::vec3(index) + glm::vec3(.5f)) / glm::vec3(cells));
    }

    /// [API] 양수 비율과 extent를 곱해 그리드 로컬 크기를 반환한다. frame 회전/배율은 적용하지 않는다.
```

### after
```cpp
        return position((glm::vec3(index) + glm::vec3(.5f)) / glm::vec3(cells));
    }


    /// [API] 양수 비율과 extent를 곱해 그리드 로컬 크기를 반환한다. frame 회전/배율은 적용하지 않는다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        }

        return result; // Local dimensions. frame orientation is kept separate.
    }

    /// [API] 비율 위치를 월드로 계산한 뒤 parentWorld의 역행렬로 부모 로컬 위치를 반환한다.
```

### after
```cpp
        }

        return result;
        // Local dimensions. frame orientation is kept separate.
    }


    /// [API] 비율 위치를 월드로 계산한 뒤 parentWorld의 역행렬로 부모 로컬 위치를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 월드 점을 frame 역변환으로 그리드 셀 인덱스에 대응시킨다. 그리드 밖이면 빈 결과다.
    std::optional<glm::ivec3> cellAt(const glm::vec3& world) const {
```

### after
```cpp
    }


    /// [API] 월드 점을 frame 역변환으로 그리드 셀 인덱스에 대응시킨다. 그리드 밖이면 빈 결과다.
    std::optional<glm::ivec3> cellAt(const glm::vec3& world) const {
```

## standard/framework/include/Model.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::vector<std::uint8_t> rgba;

    // Row zero maps to UV v=0. File decoding flips top-down images by default.
    /// [API] 이미지 너비·높이와 RGBA 바이트 수가 맞는지 검사한다.
```

### after
```cpp
    std::vector<std::uint8_t> rgba;


    // Row zero maps to UV v=0. File decoding flips top-down images by default.
    /// [API] 이미지 너비·높이와 RGBA 바이트 수가 맞는지 검사한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    FaceAnimationSpace animationSpace = FaceAnimationSpace::Local;

    /// [API] 정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& triangles) {
```

### after
```cpp
    FaceAnimationSpace animationSpace = FaceAnimationSpace::Local;


    /// [API] 정점 목록에서 하나의 면 데이터를 만든다. 모델/Scene 등록은 수행하지 않는다.
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& triangles) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::string name;

    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    std::vector<Mesh>& meshes() {
        return faces;
    }

    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
```

### after
```cpp
    std::string name;


    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
    std::vector<Mesh>& meshes() {
        return faces;
    }


    /// [API] 모델이 보관한 면/메시 배열의 참조를 반환한다. const 오버로드는 읽기 전용이다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c)
```

### after
```cpp
}


inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c)
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}

inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
```

### after
```cpp
}


inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}


inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return {};
}

inline bool validMesh(const Mesh& m) {
```

### after
```cpp
    return {};
}


inline bool validMesh(const Mesh& m) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());
```

### after
```cpp
}


inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);
```

### after
```cpp
}


inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return {box.first.x * .5f + box.second.x * .5f, box.first.y, box.first.z * .5f + box.second.z * .5f};
}

inline glm::mat4 transformMatrix(const Transform3D& t) {
```

### after
```cpp
    return {box.first.x * .5f + box.second.x * .5f, box.first.y, box.first.z * .5f + box.second.z * .5f};
}


inline glm::mat4 transformMatrix(const Transform3D& t) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline glm::mat4 faceSrtMatrix(const Mesh& face) {
    return face.independence
```

### after
```cpp
}


inline glm::mat4 faceSrtMatrix(const Mesh& face) {
    return face.independence
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

inline glm::mat4 faceLocalMatrix(const Mesh& face) {
    const auto child = faceSrtMatrix(face);
```

### after
```cpp
}


inline glm::mat4 faceLocalMatrix(const Mesh& face) {
    const auto child = faceSrtMatrix(face);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        ? face.localAnimationMatrix * child : child * face.localAnimationMatrix;
}

inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
```

### after
```cpp
        ? face.localAnimationMatrix * child : child * face.localAnimationMatrix;
}


inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
```

## standard/framework/include/ModelLoader.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    bool usedFallback = false;

    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
    explicit operator bool() const {
```

### after
```cpp
    bool usedFallback = false;


    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
    explicit operator bool() const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::shared_ptr<const TextureImage> image;
    std::string error;

    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
```

### after
```cpp
    std::shared_ptr<const TextureImage> image;
    std::string error;


    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    mutable std::map<std::filesystem::path, std::weak_ptr<const TextureImage>> imageCache_;

    std::filesystem::path resolve(const std::filesystem::path& relative) const {
        if (relative.empty() || relative.is_absolute() || relative.has_root_name())
```

### after
```cpp
    mutable std::map<std::filesystem::path, std::weak_ptr<const TextureImage>> imageCache_;


    std::filesystem::path resolve(const std::filesystem::path& relative) const {
        if (relative.empty() || relative.is_absolute() || relative.has_root_name())
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
        return full;
    }

    std::shared_ptr<const TextureImage> texture(const std::filesystem::path& relative, bool flip) const {
```

### after
```cpp
        return full;
    }


    std::shared_ptr<const TextureImage> texture(const std::filesystem::path& relative, bool flip) const {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

public:
    /// [API] 프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.
```

### after
```cpp
    }


public:
    /// [API] 프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.
    const FeatureOptions& features() const {
        return features_;
    }

    /// [API] 프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.
```

### after
```cpp
    }


    /// [API] 로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.
    const FeatureOptions& features() const {
        return features_;
    }


    /// [API] 프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.
    ModelResult loadObj(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
    /// [API] 기본 도형 OBJ를 읽는 호환 진입점이다. OBJ 로더로 처리를 위임한다.
    ModelResult loadPrimitive(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
    /// [API] 모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.
    ModelResult loadModel(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
```

### after
```cpp
    }


    /// [API] 프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.
    ModelResult loadObj(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;


    /// [API] 기본 도형 OBJ를 읽는 호환 진입점이다. OBJ 로더로 처리를 위임한다.
    ModelResult loadPrimitive(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;


    /// [API] 모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.
    ModelResult loadModel(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
```

## standard/framework/include/Motion.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
class Motion {
    std::vector<MotionStep> steps_;
    static bool validStep(const MotionStep& step);
    std::optional<MotionSample> evaluate(double seconds, bool actions) const;

public:
    /// [API] 모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.
    bool append(const MotionStep& step);
    /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
    bool insert(std::size_t index, const MotionStep& step);
    /// [API] 변환 사이의 인덱스에 의미 동작 또는 람다·함수 포인터를 삽입한다.
    bool insertAction(std::size_t index, MotionAction action, double startSeconds = 0,
                      std::optional<double> durationSeconds = std::nullopt);
    /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
    bool replace(std::size_t index, const MotionStep& step);
    /// [API] 주어진 인덱스의 모션 단계를 삭제한다.
    bool erase(std::size_t index);
    /// [API] 모든 앞 단계의 유한 종료 시각을 구한 뒤 새 단계의 시작을 그 뒤로 이동해 순차 실행을 만든다.
    bool then(MotionStep step); // Requires all previous steps to have finite durations.

    /// [API] 등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.
```

### after
```cpp
class Motion {
    std::vector<MotionStep> steps_;


    static bool validStep(const MotionStep& step);


    std::optional<MotionSample> evaluate(double seconds, bool actions) const;


public:
    /// [API] 모션 단계 배열의 끝에 유효한 단계를 추가한다. 자동 순차 배치는 then을 사용한다.
    bool append(const MotionStep& step);


    /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
    bool insert(std::size_t index, const MotionStep& step);


    /// [API] 변환 사이의 인덱스에 의미 동작 또는 람다·함수 포인터를 삽입한다.
    bool insertAction(std::size_t index, MotionAction action, double startSeconds = 0,
                      std::optional<double> durationSeconds = std::nullopt);


    /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
    bool replace(std::size_t index, const MotionStep& step);


    /// [API] 주어진 인덱스의 모션 단계를 삭제한다.
    bool erase(std::size_t index);


    /// [API] 모든 앞 단계의 유한 종료 시각을 구한 뒤 새 단계의 시작을 그 뒤로 이동해 순차 실행을 만든다.
    bool then(MotionStep step); // Requires all previous steps to have finite durations.


    /// [API] 등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다. 의미 동작은 실행하지 않는다.
    std::optional<MotionSample> sample(double seconds) const;
    /// [API] 변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.
    std::optional<MotionSample> execute(double seconds) const;
    /// [API] 기존 append/insert/then/replace와 조립하는 비변환 동작 단계를 만든다.
    static MotionStep action(MotionAction action, double startSeconds = 0,
                             std::optional<double> durationSeconds = std::nullopt);
    /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
    static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);
    /// [API] 축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.
    static MotionStep rotation(const RotationMotion& config);
    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);
    /// [API] 시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.
    static MotionStep linear(const LinearMotion& config);
    /// [API] 지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.
    static MotionStep pendulum(const PendulumMotion& config);
```

### after
```cpp
    }


    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다. 의미 동작은 실행하지 않는다.
    std::optional<MotionSample> sample(double seconds) const;


    /// [API] 변환을 평가하며 동작을 순서대로 실행한다. 실패한 외부 부수 효과는 복구하지 않는다.
    std::optional<MotionSample> execute(double seconds) const;


    /// [API] 기존 append/insert/then/replace와 조립하는 비변환 동작 단계를 만든다.
    static MotionStep action(MotionAction action, double startSeconds = 0,
                             std::optional<double> durationSeconds = std::nullopt);


    /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
    static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);


    /// [API] 축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.
    static MotionStep rotation(const RotationMotion& config);


    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);


    /// [API] 시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.
    static MotionStep linear(const LinearMotion& config);


    /// [API] 지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.
    static MotionStep pendulum(const PendulumMotion& config);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;
    bool validLayer(const LayerConfig& config) const;

public:
```

### after
```cpp
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;


    bool validLayer(const LayerConfig& config) const;


public:
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
    std::optional<MotionId> createMotion(const Motion& motion);
    /// [API] 의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.
    std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);
    /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
    bool setMotion(MotionId id, const Motion& motion);
    /// [API] 모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const MotionState* getMotion(MotionId id) const;
    /// [API] 모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.
    bool setMotionPlaying(MotionId id, bool playing);
    /// [API] 모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.
    bool setMotionEnabled(MotionId id, bool enabled);
    /// [API] 지정 시각을 샘플링하고 유효할 때 모션의 시간과 샘플을 함께 바꾼다.
    bool seekMotion(MotionId id, double seconds);
    /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
    bool removeMotion(MotionId id);
    /// [API] 모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.
    bool update(double deltaSeconds);
    /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
    void clear(); // Identifiers are never reused by this system, including after clear.
    /// [API] 유효한 부모 아래 새 자식 레이어를 추가한다. 복사한 임시 맵에 반영한 뒤 swap하여 상태를 확정한다.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});
    /// [API] 기존 child와 그 parent 사이에 새 레이어를 끼운다. root 위 삽입은 허용하지 않는다.
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});
    /// [API] 자식이 없는 비root leaf만 제거하고 그 부모 ID를 반환한다.
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.
    /// [API] root 이외 레이어를 제거하며 자식들을 부모의 해당 위치로 승격한다. 월드 자세 보존을 자동 약속하지 않는다.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.
    /// [API] 레이어의 부모를 바꾸며 자신의 자손으로 연결되는 순환을 거부한다.
    bool reparentLayer(LayerId id, LayerId parent);
    /// [API] 기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.
    bool setLayer(LayerId id, const LayerConfig& config);
    /// [API] 레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const LayerState* getLayer(LayerId id) const;
    /// [API] 기존 레이어에 모션 ID를 연결하거나 해제한다. 모션과 레이어가 존재하는지 검사한다.
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);
    /// [API] leaf까지의 계층을 따라 transform과 motion 샘플을 World/Local 공간에 맞게 parent/local 행렬로 합성한다.
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;
    /// [API] 로컬 공간 회전 모션을 등록하여 자전에 사용한다.
    std::optional<MotionId> createSpin(RotationMotion config);
    /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
    std::optional<MotionId> createOrbit(RotationMotion config);
```

### after
```cpp
    }


    /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
    std::optional<MotionId> createMotion(const Motion& motion);


    /// [API] 의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.
    std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);


    /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
    bool setMotion(MotionId id, const Motion& motion);


    /// [API] 모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const MotionState* getMotion(MotionId id) const;


    /// [API] 모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.
    bool setMotionPlaying(MotionId id, bool playing);


    /// [API] 모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.
    bool setMotionEnabled(MotionId id, bool enabled);


    /// [API] 지정 시각을 샘플링하고 유효할 때 모션의 시간과 샘플을 함께 바꾼다.
    bool seekMotion(MotionId id, double seconds);


    /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
    bool removeMotion(MotionId id);


    /// [API] 모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.
    bool update(double deltaSeconds);


    /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
    void clear(); // Identifiers are never reused by this system, including after clear.


    /// [API] 유효한 부모 아래 새 자식 레이어를 추가한다. 복사한 임시 맵에 반영한 뒤 swap하여 상태를 확정한다.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});


    /// [API] 기존 child와 그 parent 사이에 새 레이어를 끼운다. root 위 삽입은 허용하지 않는다.
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});


    /// [API] 자식이 없는 비root leaf만 제거하고 그 부모 ID를 반환한다.
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.


    /// [API] root 이외 레이어를 제거하며 자식들을 부모의 해당 위치로 승격한다. 월드 자세 보존을 자동 약속하지 않는다.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.


    /// [API] 레이어의 부모를 바꾸며 자신의 자손으로 연결되는 순환을 거부한다.
    bool reparentLayer(LayerId id, LayerId parent);


    /// [API] 기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.
    bool setLayer(LayerId id, const LayerConfig& config);


    /// [API] 레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const LayerState* getLayer(LayerId id) const;


    /// [API] 기존 레이어에 모션 ID를 연결하거나 해제한다. 모션과 레이어가 존재하는지 검사한다.
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);


    /// [API] leaf까지의 계층을 따라 transform과 motion 샘플을 World/Local 공간에 맞게 parent/local 행렬로 합성한다.
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;


    /// [API] 로컬 공간 회전 모션을 등록하여 자전에 사용한다.
    std::optional<MotionId> createSpin(RotationMotion config);


    /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
    std::optional<MotionId> createOrbit(RotationMotion config);
```

## standard/framework/include/Patterns.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
struct Bounds3 {
    glm::vec3 minimum{-1}, maximum{1};
    /// [API] 해당 데이터가 계산에 필요한 유효 범위와 유한성 조건을 만족하는지 반환한다.
    bool valid() const;
    /// [API] 점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.
    bool contains(const glm::vec3& point, const glm::vec3& margin = glm::vec3(0)) const;
    /// [API] 두 축 정렬 경계 상자가 겹치는지 검사한다.
    bool overlaps(const Bounds3& other) const;
```

### after
```cpp
struct Bounds3 {
    glm::vec3 minimum{-1}, maximum{1};


    /// [API] 해당 데이터가 계산에 필요한 유효 범위와 유한성 조건을 만족하는지 반환한다.
    bool valid() const;


    /// [API] 점이 margin만큼 안쪽으로 줄인 Bounds3 안에 있는지 검사한다.
    bool contains(const glm::vec3& point, const glm::vec3& margin = glm::vec3(0)) const;


    /// [API] 두 축 정렬 경계 상자가 겹치는지 검사한다.
    bool overlaps(const Bounds3& other) const;
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    bool setPoints(const std::vector<glm::vec3>& points, bool closed = false);

    /// [API] 경로 전체 누적 길이를 반환한다. 경로가 비어 있으면 0이다.
    double length() const {
        return distances_.empty() ? 0 : distances_.back();
    }

    /// [API] 누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.
```

### after
```cpp
    bool setPoints(const std::vector<glm::vec3>& points, bool closed = false);


    /// [API] 경로 전체 누적 길이를 반환한다. 경로가 비어 있으면 0이다.
    double length() const {
        return distances_.empty() ? 0 : distances_.back();
    }


    /// [API] 누적 거리와 Once/Loop/PingPong 정책에서 위치·접선·yaw를 샘플링한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    double seconds = 0;
    bool playing = true;
    /// [API] playing일 때만 누적 시간을 늘린다. dt와 누적 값의 유한성·범위를 검사한다.
    bool advance(double dt);

    /// [API] 누적 시간을 0으로 되돌리고 재생 상태를 켠다.
```

### after
```cpp
    double seconds = 0;
    bool playing = true;


    /// [API] playing일 때만 누적 시간을 늘린다. dt와 누적 값의 유한성·범위를 검사한다.
    bool advance(double dt);


    /// [API] 누적 시간을 0으로 되돌리고 재생 상태를 켠다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    glm::vec3 worldPosition{0}, worldVelocity{0};
    bool released = false;

    /// [API] 이탈 전에는 roomWorld로 로컬 중심을 월드로 변환하고 이탈 후에는 worldPosition을 반환한다.
```

### after
```cpp
    glm::vec3 worldPosition{0}, worldVelocity{0};
    bool released = false;


    /// [API] 이탈 전에는 roomWorld로 로컬 중심을 월드로 변환하고 이탈 후에는 worldPosition을 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
                             bool elastic, const glm::vec3& gravity = glm::vec3(0, -9.8f, 0),
                             const glm::vec3& angularVelocity = glm::vec3(0));
    /// [API] 접지 상태에서만 위쪽 속도를 설정한다. 공중 입력 거절은 정상적인 false다.
    static bool jump(CharacterBody& body, float speed);
    /// [API] 수평 이동·장애물/경계 반전·중력·윗면 착지·지지면 이탈을 하위 단계로 계산한다.
    static std::optional<CharacterStep> stepCharacter(CharacterBody& body, const Bounds3& stage,
                                                      const std::vector<Bounds3>& obstacles, float dt,
                                                      float gravity = 9.8f);
    /// [API] 제한 횟수 안에서 occupied와 겹치지 않는 배치를 찾는다. 난수 엔진의 시드·수명은 호출자가 관리한다.
    static std::optional<Bounds3> randomFreeBox(const Bounds3& area, const glm::vec3& size,
```

### after
```cpp
                             bool elastic, const glm::vec3& gravity = glm::vec3(0, -9.8f, 0),
                             const glm::vec3& angularVelocity = glm::vec3(0));


    /// [API] 접지 상태에서만 위쪽 속도를 설정한다. 공중 입력 거절은 정상적인 false다.
    static bool jump(CharacterBody& body, float speed);


    /// [API] 수평 이동·장애물/경계 반전·중력·윗면 착지·지지면 이탈을 하위 단계로 계산한다.
    static std::optional<CharacterStep> stepCharacter(CharacterBody& body, const Bounds3& stage,
                                                      const std::vector<Bounds3>& obstacles, float dt,
                                                      float gravity = 9.8f);


    /// [API] 제한 횟수 안에서 occupied와 겹치지 않는 배치를 찾는다. 난수 엔진의 시드·수명은 호출자가 관리한다.
    static std::optional<Bounds3> randomFreeBox(const Bounds3& area, const glm::vec3& size,
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    static std::optional<PathPose> conveyorCornerPose(double distance, const glm::vec3& start, const glm::vec3& pivot,
                                                      float radius, float endZ);
    /// [API] 양수 크기의 중심 기준 모델을 만들고 부모 로컬 위치·색·부모 관계를 설정한다.
    static std::optional<EntityId> addBox(Scene& scene, const glm::vec3& size, const glm::vec3& position, GlColor color,
                                          std::optional<EntityId> parent = std::nullopt);
    /// [API] 점 목록으로 단일 다각형 면을 만들고 색·부모 관계와 함께 Scene에 등록한다. 오목 다각형의 자동 분할을 가정하지 않는다.
    static std::optional<EntityId> addPolygon(Scene& scene, const std::vector<glm::vec3>& points, GlColor color,
                                              std::optional<EntityId> parent = std::nullopt);
    /// [API] 여섯 면의 방을 만들고 바깥 winding에 Front cull, 안쪽 조명 법선을 적용한다. face 0..3은 옆벽, 4 천장, 5 바닥이다.
    static std::optional<EntityId> addRoom(Scene& scene, const Bounds3& bounds, bool frontOpen = true);
    /// [API] 부모에 부착된 transform-only 그룹을 만든다. 표시 메시 크기와 회전 연결 위치를 분리한다.
    static std::optional<EntityId> addJoint(Scene& scene, EntityId parent, const glm::vec3& anchor);
    /// [API] 연결 위치와 크기를 유지하면서 회전 기저를 변경한다.
    static bool rotateJoint(Scene& scene, EntityId joint, const glm::vec3& axis, float degrees);
    /// [API] DistancePath 샘플을 엔티티 위치와 +Z 기준 Y 회전에 반영한다.
    static bool follow(Scene& scene, EntityId entity, const DistancePath& path, double distance,
                       const glm::vec3& offset = glm::vec3(0), bool orient = true);
    /// [API] 0~9를 선분으로 구성한다. 부모 로컬 XY 평면의 표시이며 일반 폰트/텍스트 렌더러는 아니다.
    static bool addNumber(Scene& scene, unsigned digit, const glm::vec3& position, float height, GlColor color,
                          std::optional<EntityId> parent = std::nullopt);
    /// [API] 지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.
    static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);
    /// [API] 원래 모서리 힌지·면 중심 자전·원본 모델 중심 공전을 구분하여 면 회전 행렬을 설정한다.
    static bool rotateFace(Scene& scene, EntityId entity, std::size_t face, const FaceRotationConfig& config);
    /// [API] 진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.
    static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement,
                         const glm::vec3& topPivot, bool scaleOpen);
    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);
    /// [API] 선택 옆벽의 원래 중심을 기준으로 모든 축을 균등 축소/복원하며 완전 개방 시 숨긴다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);
    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);
    /// [API] 두 값의 차이가 허용 오차 이하인지 검사한다.
    static bool aligned(float first, float second, float tolerance);
    /// [API] 좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.
    static glm::vec3 snap(const glm::vec3& point, float spacing);
    /// [API] 화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.
    static std::optional<glm::vec3> screenOnPlane(const Camera& camera, double x, double y,
                                                  const PickViewport& viewport, const glm::vec3& planePoint,
                                                  const glm::vec3& normal);
    /// [API] 시점과 주목점을 동일하게 이동해 시선 방향을 보존한다.
    static bool moveCamera(Camera& camera, const glm::vec3& offset);
    /// [API] 시점은 고정하고 카메라 로컬 up축을 기준으로 시선을 회전한다.
    static bool yawCamera(Camera& camera, float degrees);
    /// [API] 월드 축에 대해 시점·주목점·up을 함께 변환한다. 축을 target에 두면 target은 고정된다.
    static bool orbitCamera(Camera& camera, const AxisLine& axis, float degrees);
    /// [API] 원근은 target까지 거리, 직각은 orthographicHalfExtent를 배율로 바꾼다.
    static bool zoomCamera(Camera& camera, float factor);
    /// [API] 원근·XZ 평면도·XY 정면도 카메라와 픽셀 영역을 구성한다. 상단 뷰의 up은 -Z다.
    static std::vector<RenderPass> threeViews(const Camera& perspective, int width, int height,
                                              const RenderOptions& options = RenderOptions{});
    /// [API] 그리드 비율로 머리·코·몸통·어깨·엉덩이·팔·다리 계층을 조립하고 ID 묶음을 반환한다.
    static std::optional<JointRig> addWalkingRig(Scene& scene, const GridLayout& bodyGrid);
    /// [API] 루트 이동/yaw와 실제 이동 거리 기반 팔다리 반대 위상을 적용한다. 속도는 보행 진폭에 반영한다.
    static bool poseWalkingRig(Scene& scene, const JointRig& rig, const glm::vec3& feet, float yawDegrees,
```

### after
```cpp
    static std::optional<PathPose> conveyorCornerPose(double distance, const glm::vec3& start, const glm::vec3& pivot,
                                                      float radius, float endZ);


    /// [API] 양수 크기의 중심 기준 모델을 만들고 부모 로컬 위치·색·부모 관계를 설정한다.
    static std::optional<EntityId> addBox(Scene& scene, const glm::vec3& size, const glm::vec3& position, GlColor color,
                                          std::optional<EntityId> parent = std::nullopt);


    /// [API] 점 목록으로 단일 다각형 면을 만들고 색·부모 관계와 함께 Scene에 등록한다. 오목 다각형의 자동 분할을 가정하지 않는다.
    static std::optional<EntityId> addPolygon(Scene& scene, const std::vector<glm::vec3>& points, GlColor color,
                                              std::optional<EntityId> parent = std::nullopt);


    /// [API] 여섯 면의 방을 만들고 바깥 winding에 Front cull, 안쪽 조명 법선을 적용한다. face 0..3은 옆벽, 4 천장, 5 바닥이다.
    static std::optional<EntityId> addRoom(Scene& scene, const Bounds3& bounds, bool frontOpen = true);


    /// [API] 부모에 부착된 transform-only 그룹을 만든다. 표시 메시 크기와 회전 연결 위치를 분리한다.
    static std::optional<EntityId> addJoint(Scene& scene, EntityId parent, const glm::vec3& anchor);


    /// [API] 연결 위치와 크기를 유지하면서 회전 기저를 변경한다.
    static bool rotateJoint(Scene& scene, EntityId joint, const glm::vec3& axis, float degrees);


    /// [API] DistancePath 샘플을 엔티티 위치와 +Z 기준 Y 회전에 반영한다.
    static bool follow(Scene& scene, EntityId entity, const DistancePath& path, double distance,
                       const glm::vec3& offset = glm::vec3(0), bool orient = true);


    /// [API] 0~9를 선분으로 구성한다. 부모 로컬 XY 평면의 표시이며 일반 폰트/텍스트 렌더러는 아니다.
    static bool addNumber(Scene& scene, unsigned digit, const glm::vec3& position, float height, GlColor color,
                          std::optional<EntityId> parent = std::nullopt);


    /// [API] 지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.
    static bool foldFace(Scene& scene, EntityId entity, std::size_t face, const AxisLine& hinge, float degrees);


    /// [API] 원래 모서리 힌지·면 중심 자전·원본 모델 중심 공전을 구분하여 면 회전 행렬을 설정한다.
    static bool rotateFace(Scene& scene, EntityId entity, std::size_t face, const FaceRotationConfig& config);


    /// [API] 진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.
    static bool openFace(Scene& scene, EntityId entity, std::size_t face, float progress, const glm::vec3& displacement,
                         const glm::vec3& topPivot, bool scaleOpen);


    /// [API] 시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 큰 양수인 면을 선택한다. 외부 카메라에서 보이는 맞은편 내부 벽이며 수직 시선에는 선택이 없을 수 있다.
    static std::optional<std::size_t> cameraFacingWall(const Scene& scene, EntityId room, const Camera& camera);


    /// [API] 선택 옆벽의 원래 중심을 기준으로 모든 축을 균등 축소/복원하며 완전 개방 시 숨긴다.
    static bool openRoomWall(Scene& scene, EntityId room, std::size_t face, float progress);


    /// [API] 셀 격자를 지그재그 순서로 방문하는 정수 인덱스 목록을 만든다.
    static std::vector<glm::ivec3> snakeCells(const glm::ivec3& cells);


    /// [API] 두 값의 차이가 허용 오차 이하인지 검사한다.
    static bool aligned(float first, float second, float tolerance);


    /// [API] 좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.
    static glm::vec3 snap(const glm::vec3& point, float spacing);


    /// [API] 화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.
    static std::optional<glm::vec3> screenOnPlane(const Camera& camera, double x, double y,
                                                  const PickViewport& viewport, const glm::vec3& planePoint,
                                                  const glm::vec3& normal);


    /// [API] 시점과 주목점을 동일하게 이동해 시선 방향을 보존한다.
    static bool moveCamera(Camera& camera, const glm::vec3& offset);


    /// [API] 시점은 고정하고 카메라 로컬 up축을 기준으로 시선을 회전한다.
    static bool yawCamera(Camera& camera, float degrees);


    /// [API] 월드 축에 대해 시점·주목점·up을 함께 변환한다. 축을 target에 두면 target은 고정된다.
    static bool orbitCamera(Camera& camera, const AxisLine& axis, float degrees);


    /// [API] 원근은 target까지 거리, 직각은 orthographicHalfExtent를 배율로 바꾼다.
    static bool zoomCamera(Camera& camera, float factor);


    /// [API] 원근·XZ 평면도·XY 정면도 카메라와 픽셀 영역을 구성한다. 상단 뷰의 up은 -Z다.
    static std::vector<RenderPass> threeViews(const Camera& perspective, int width, int height,
                                              const RenderOptions& options = RenderOptions{});


    /// [API] 그리드 비율로 머리·코·몸통·어깨·엉덩이·팔·다리 계층을 조립하고 ID 묶음을 반환한다.
    static std::optional<JointRig> addWalkingRig(Scene& scene, const GridLayout& bodyGrid);


    /// [API] 루트 이동/yaw와 실제 이동 거리 기반 팔다리 반대 위상을 적용한다. 속도는 보행 진폭에 반영한다.
    static bool poseWalkingRig(Scene& scene, const JointRig& rig, const glm::vec3& feet, float yawDegrees,
```

## standard/framework/include/Picking.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    double x = 0, y = 0, width = 0, height = 0;
}; // top-left framebuffer pixels

/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
```

### after
```cpp
    double x = 0, y = 0, width = 0, height = 0;
}; // top-left framebuffer pixels


/// [API] 왼쪽 위 기준 프레임버퍼 픽셀을 카메라 ray로 역투영한다. 영역 밖/잘못된 투영이면 빈 결과다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    return Ray{glm::vec3(nearPoint), direction / length, length};
}

/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
```

### after
```cpp
    return Ray{glm::vec3(nearPoint), direction / length, length};
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
```

### after
```cpp
}


/// [API] ray 또는 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
```

## standard/framework/include/RenderConfig.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    glm::mat4 model, view, projection;
    bool depthOnly = false;
    /// [API] 현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.
    bool setUniform(const std::string& name, float value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !std::isfinite(value)) return false;
        detail::gl().Uniform1f(location, value); return true;
    }
    /// [API] 현재 program의 추가 int/bool/sampler uniform을 설정한다.
    bool setUniform(const std::string& name, int value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0) return false;
        detail::gl().Uniform1i(location, value); return true;
    }
    /// [API] 현재 program의 추가 vec3 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::vec3& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteVector(value)) return false;
        detail::gl().Uniform3f(location, value.x, value.y, value.z); return true;
    }
    /// [API] 현재 program의 추가 mat4 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::mat4& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteMatrix(value)) return false;
        detail::gl().UniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]); return true;
    }
};
```

### after
```cpp
    glm::mat4 model, view, projection;
    bool depthOnly = false;


    /// [API] 현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.
    bool setUniform(const std::string& name, float value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !std::isfinite(value))
            return false;
        detail::gl().Uniform1f(location, value);
        return true;
    }


    /// [API] 현재 program의 추가 int/bool/sampler uniform을 설정한다.
    bool setUniform(const std::string& name, int value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0)
            return false;
        detail::gl().Uniform1i(location, value);
        return true;
    }


    /// [API] 현재 program의 추가 vec3 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::vec3& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteVector(value))
            return false;
        detail::gl().Uniform3f(location, value.x, value.y, value.z);
        return true;
    }


    /// [API] 현재 program의 추가 mat4 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::mat4& value) const {
        const auto location = detail::gl().GetUniformLocation(program, name.c_str());
        if (location < 0 || !detail::finiteMatrix(value))
            return false;
        detail::gl().UniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
        return true;
    }
};
```

## standard/framework/include/Texture.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    T* value = nullptr;

    ~ComOwner() {
        if (value)
```

### after
```cpp
    T* value = nullptr;


    ~ComOwner() {
        if (value)
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    }

    ComOwner() = default;
    ComOwner(const ComOwner&) = delete;

    ComOwner& operator=(const ComOwner&) = delete;
};

inline void checkCom(HRESULT result) {
```

### after
```cpp
    }


    ComOwner() = default;


    ComOwner(const ComOwner&) = delete;


    ComOwner& operator=(const ComOwner&) = delete;
};


inline void checkCom(HRESULT result) {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
}

struct ComApartment {
    HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);

    ComApartment() {
```

### after
```cpp
}


struct ComApartment {
    HRESULT result = CoInitializeEx(nullptr, COINIT_MULTITHREADED);


    ComApartment() {
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
            checkCom(result);
    }

    ~ComApartment() {
```

### after
```cpp
            checkCom(result);
    }


    ~ComApartment() {
```

## standard/framework/include/Transform.h

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.
    static bool validAffine(const glm::mat4& matrix);
    /// [API] 이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.
    static std::optional<glm::mat4> translation(const glm::vec3& offset);
    /// [API] 중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.
    static std::optional<glm::mat4> scaling(const glm::vec3& factors, const glm::vec3& center = glm::vec3(0));
    /// [API] 직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);
    /// [API] 주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);
    /// [API] 주어진 직선을 기준으로 선 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);
    /// [API] 평면의 점과 법선으로 평면 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point, const glm::vec3& normal);
```

### after
```cpp
    /// [API] 행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.
    static bool validAffine(const glm::mat4& matrix);


    /// [API] 이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.
    static std::optional<glm::mat4> translation(const glm::vec3& offset);


    /// [API] 중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.
    static std::optional<glm::mat4> scaling(const glm::vec3& factors, const glm::vec3& center = glm::vec3(0));


    /// [API] 직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);


    /// [API] 주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);


    /// [API] 주어진 직선을 기준으로 선 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);


    /// [API] 평면의 점과 법선으로 평면 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point, const glm::vec3& normal);
```

변경 이유: API 단위 간격·문장 줄바꿈. 동작 차이: 없음(공백만 수정).

### before
```cpp
    /// [API] 중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU, const glm::vec3& axisV);
    /// [API] 중심·반지름·평면 법선으로 원 경로 함수를 만든다.
    static PathFunction circle(const glm::vec3& center, float radius, const glm::vec3& normal = glm::vec3(0, 1, 0));
    /// [API] 점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);
    /// [API] 마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.
    static PathFunction polygon(std::vector<glm::vec3> points);
    /// [API] 중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU, const glm::vec3& halfV);
    /// [API] 입력 진행률을 선형 보간 진행률로 반환한다.
    static double linear(double progress);
    /// [API] 진행률에 3t²-2t³ 완화 곡선을 적용한다.
    static double smoothStep(double progress);
```

### after
```cpp
    /// [API] 중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU, const glm::vec3& axisV);


    /// [API] 중심·반지름·평면 법선으로 원 경로 함수를 만든다.
    static PathFunction circle(const glm::vec3& center, float radius, const glm::vec3& normal = glm::vec3(0, 1, 0));


    /// [API] 점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);


    /// [API] 마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.
    static PathFunction polygon(std::vector<glm::vec3> points);


    /// [API] 중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU, const glm::vec3& halfV);


    /// [API] 입력 진행률을 선형 보간 진행률로 반환한다.
    static double linear(double progress);


    /// [API] 진행률에 3t²-2t³ 완화 곡선을 적용한다.
    static double smoothStep(double progress);
```
