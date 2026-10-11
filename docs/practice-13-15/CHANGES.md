# 실습 13·14·15 전체 before / after

- 시작 기준 커밋: `148ae23cbe6cc37d76a7dda0446923e2c319fd20`
- 빌드와 실행 테스트는 수행하지 않았다. 사용자가 직접 확인한다.
- 모든 수정 코드와 신규 자체 코드를 파일별 완전한 before/after로 수록한다.
- 동일한 내용의 사본은 경로를 모두 나열하고 after 전체를 한 번 수록한다.
- 기존 외부 의존성은 바이트 그대로 복사한다. 아래 별도 목록에 경로와 SHA-256을 수록한다.
- 리소스의 UTF-16/기존 인코딩은 UTF-8 BOM으로 변환하고 RC에 code_page(65001)을 지정했다.

## 변경 묶음 1: .gitignore

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `b7c0490f7950c160cc79d129b13a09ae7846c6c20d9453cf0c9d50f20ad3f1ba`

### .gitignore — before

````text
# Prerequisites
*.d

# Compiled Object files
*.slo
*.lo
*.o
*.obj

# Precompiled Headers
*.gch
*.pch

# Compiled Dynamic libraries
*.so
*.dylib
*.dll

# Fortran module files
*.mod
*.smod

# Compiled Static libraries
*.lai
*.la
*.a
*.lib

# Executables
*.exe
*.out
*.app

# VS File for Copliot, vs set, and else..
*.vs

**/x64/
*.aps
*.vcxproj.user
.vs/

# Local OpenGL dependency locations and downloaded packages
OpenGL.local.props
/opengl_libs/
/opengl_libs.zip

# Vendored OpenGL x64 runtime and import libraries (source dependencies)
!/dependencies/glew-2.3.1/lib/Release/x64/
!/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/dependencies/glew-2.3.1/bin/Release/x64/
!/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll

# Framework-local x64 dependency runtime and import libraries
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/lib/Release/x64/
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/bin/Release/x64/
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/openGL_3dAssign_13/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/openGL_3dAssign_13/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll
````

### after — 위 경로 전체의 최종 저장 내용

````text
# Prerequisites
*.d

# Compiled Object files
*.slo
*.lo
*.o
*.obj

# Precompiled Headers
*.gch
*.pch

# Compiled Dynamic libraries
*.so
*.dylib
*.dll

# Fortran module files
*.mod
*.smod

# Compiled Static libraries
*.lai
*.la
*.a
*.lib

# Executables
*.exe
*.out
*.app

# VS File for Copliot, vs set, and else..
*.vs

**/x64/
*.aps
*.vcxproj.user
.vs/

# Local OpenGL dependency locations and downloaded packages
OpenGL.local.props
/opengl_libs/
/opengl_libs.zip

# Vendored OpenGL x64 runtime and import libraries (source dependencies)
!/dependencies/glew-2.3.1/lib/Release/x64/
!/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/dependencies/glew-2.3.1/bin/Release/x64/
!/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll

# Framework-local x64 dependency runtime and import libraries
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/lib/Release/x64/
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/bin/Release/x64/
!/openGL_3dAssign_13/framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/openGL_3dAssign_13/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/openGL_3dAssign_13/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll

# Independent practice and reusable framework binary dependencies
!/framework/dependencies/glew-2.3.1/lib/Release/x64/
!/framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/framework/dependencies/glew-2.3.1/bin/Release/x64/
!/framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll
!/openGL_3dAssign_14/framework/dependencies/glew-2.3.1/lib/Release/x64/
!/openGL_3dAssign_14/framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/openGL_3dAssign_14/framework/dependencies/glew-2.3.1/bin/Release/x64/
!/openGL_3dAssign_14/framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/openGL_3dAssign_14/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/openGL_3dAssign_14/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll
!/openGL_3dAssign_15/framework/dependencies/glew-2.3.1/lib/Release/x64/
!/openGL_3dAssign_15/framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib
!/openGL_3dAssign_15/framework/dependencies/glew-2.3.1/bin/Release/x64/
!/openGL_3dAssign_15/framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll
!/openGL_3dAssign_15/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib
!/openGL_3dAssign_15/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll
````

## 변경 묶음 2: Console.cpp

변경 이유: 과제별 입력·상태·변환을 구현한다.

최종 파일 SHA-256: `390da62e5dd34e8bc2c64fbb395b630ad0a1cd5b81e07b79368dc4f06d088155`

### openGL_3dAssign_13/Console.cpp — before

````cpp
#include "Console.h"

#include <utility>

bool Console::initScene() {
    // 기본 입력 값. 길이·월드 위치·회전을 각각 변경할 수 있다.
    const glm::vec3 cuboidSideLengths{ 1.0f, 1.0f, 1.0f };
    const glm::vec3 cuboidWorldPosition{ -0.9f, 0.0f, 0.0f };
    const glm::vec3 cuboidRotationDegrees{ 20.0f, -25.0f, 0.0f };
    const glm::vec3 tetrahedronEdgeLengths{ 1.0f, 1.0f, 1.0f };
    const glm::vec3 tetrahedronWorldPosition{ 0.9f, 0.0f, 0.0f };
    const glm::vec3 tetrahedronRotationDegrees{ 0.0f, 55.0f, 0.0f };

    graphics::Scene scene;
    const auto cuboid = scene.addCuboid(cuboidSideLengths, cuboidWorldPosition);
    const auto tetrahedron = scene.addRightTetrahedron(
        tetrahedronEdgeLengths, tetrahedronWorldPosition);
    if (!cuboid || !tetrahedron) {
        return false;
    }

    graphics::Transform3D cuboidTransform;
    cuboidTransform.basis = cuboidWorldPosition;
    cuboidTransform.rotationDegrees = cuboidRotationDegrees;
    graphics::Transform3D tetrahedronTransform;
    tetrahedronTransform.basis = tetrahedronWorldPosition;
    tetrahedronTransform.rotationDegrees = tetrahedronRotationDegrees;
    if (!scene.setTransform(*cuboid, cuboidTransform)
        || !scene.setTransform(*tetrahedron, tetrahedronTransform)) {
        return false;
    }

    const glm::vec3 basisOrigin{ 0.0f };
    const glm::mat3 basisVectors{ 1.0f };
    const glm::vec3 worldAxisLengths{ 1.4f };
    const glm::vec3 localAxisLengths{ 0.9f };
    const glm::vec3 vectorOrigin{ 0.0f, -0.9f, 0.0f };
    const glm::vec3 vectorValue{ 0.8f, 0.5f, 0.6f };
    const graphics::GlColor vectorColor{ 1.0f, 0.85f, 0.2f };
    if (!scene.addBasis(basisOrigin, basisVectors, worldAxisLengths)
        || !scene.addBasis(basisOrigin, basisVectors, localAxisLengths, *cuboid)
        || !scene.addBasis(basisOrigin, basisVectors, localAxisLengths, *tetrahedron)
        || !scene.addVector(vectorOrigin, vectorValue, vectorColor)) {
        return false;
    }
    application_.getScene() = std::move(scene);
    cuboidId_ = cuboid;
    tetrahedronId_ = tetrahedron;
    return true;
}
````

### after — 위 경로 전체의 최종 저장 내용

````cpp
#include "Console.h"
#include <glfw3.h>
#include <iostream>

bool Console::initScene() {
    graphics::Scene& scene = application_.getScene();
    scene.clear();
    const auto cube = scene.addCuboid(glm::vec3(0.8f), glm::vec3(0.0f));
    const auto pyramid = scene.addPyramid(glm::vec3(0.8f), glm::vec3(0.0f));
    if (!cube || !pyramid) {
        return false;
    }
    cuboidId_ = *cube;
    pyramidId_ = *pyramid;
    graphics::Transform3D initial;
    initial.rotationDegrees = { 30.0f, 30.0f, 0.0f };
    if (!scene.setTransform(cuboidId_, initial) || !scene.setTransform(pyramidId_, initial)) {
        return false;
    }
    // 축은 객체에 연결하지 않는다. 원점부터 양·음 방향으로 각각 1만큼 표시한다.
    if (!scene.addLine({ -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.2f, 0.2f })
        || !scene.addLine({ 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.2f, 1.0f, 0.2f })
        || !scene.addLine({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.2f, 0.4f, 1.0f })) {
        return false;
    }
    auto& camera = application_.getCamera();
    camera.position = { 1.8f, 1.2f, 3.0f };
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    selectFaces(cuboidId_, { 0, 1, 2, 3, 4, 5 });
    std::cout << "Practice 13: each selection replaces the previous faces.\n"
              << "1 front(red), 2 back(green), 3 left(blue), 4 right(yellow)\n"
              << "5 top(cyan), 6 bottom(magenta)\n"
              << "7/8/9/0 pyramid: front(red)/right(blue)/back(green)/left(yellow)\n"
              << "C: two distinct cube faces, T: pyramid base + one side\n"
              << "Axes: X red, Y green, Z blue. Esc: quit\n";
    return true;
}

void Console::selectFaces(graphics::EntityId id, std::initializer_list<std::size_t> faces) {
    auto& scene = application_.getScene();
    for (auto entity : scene.getIds()) {
        const auto* component = scene.getComponent(entity);
        for (std::size_t i = 0; i < component->polyhedron.faces.size(); ++i) {
            scene.setFaceVisible(entity, i, false);
        }
    }
    for (std::size_t face : faces) {
        scene.setFaceVisible(id, face, true);
    }
}

void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return; // 키 반복으로 무작위 선택을 여러 번 바꾸지 않는다.
    }
    if (key == GLFW_KEY_ESCAPE) {
        application_.requestClose();
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        selectFaces(cuboidId_, { static_cast<std::size_t>(key - GLFW_KEY_1) });
    } else if (key >= GLFW_KEY_7 && key <= GLFW_KEY_9) {
        selectFaces(pyramidId_, { static_cast<std::size_t>(key - GLFW_KEY_7) });
    } else if (key == GLFW_KEY_0) {
        selectFaces(pyramidId_, { 3 });
    } else if (key == GLFW_KEY_C) {
        const std::size_t first = std::uniform_int_distribution<int>(0, 5)(random_);
        std::size_t second = std::uniform_int_distribution<int>(0, 4)(random_);
        if (second >= first) {
            ++second; // 첫 면을 제외한 다섯 면 중 하나다.
        }
        selectFaces(cuboidId_, { first, second });
    } else if (key == GLFW_KEY_T) {
        const std::size_t side = std::uniform_int_distribution<int>(0, 3)(random_);
        selectFaces(pyramidId_, { 4, side });
    }
}
````

## 변경 묶음 3: Console.h

변경 이유: 과제별 입력·상태·변환을 구현한다.

최종 파일 SHA-256: `d50f8cea24ac7526a4f39a1e7ce332cdd7d0cbb47a3297e5d147a0ff0f1fc287`

### openGL_3dAssign_13/Console.h — before

````cpp
#pragma once

#include "framework/include/Application.h"
#include <optional>

// 과제 예제 전용 상태. framework를 사용하는 다른 프로젝트에는 필요 없다.
class Console {
private:
    graphics::Application& application_;
    std::optional<graphics::EntityId> cuboidId_;
    std::optional<graphics::EntityId> tetrahedronId_;

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    graphics::Scene& getScene() { return application_.getScene(); }
    graphics::Camera& getCamera() { return application_.getCamera(); }
    std::optional<graphics::EntityId> getCuboidId() const { return cuboidId_; }
    std::optional<graphics::EntityId> getTetrahedronId() const { return tetrahedronId_; }
};
````

### after — 위 경로 전체의 최종 저장 내용

````cpp
#pragma once

#include "framework/include/Application.h"
#include <initializer_list>
#include <random>

// 실습 13: 표시할 면과 난수 상태만 관리한다.
class Console {
private:
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    std::mt19937 random_{ std::random_device{}() };

    void selectFaces(graphics::EntityId id, std::initializer_list<std::size_t> faces);

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    void onKey(int key, int action);
};
````

## 변경 묶음 4: main.cpp

변경 이유: 과제별 입력·상태·변환을 구현한다.

최종 파일 SHA-256: `aa09fd90e2dddbdef767209c4b05604248df6d6ee3a32b91b35bc2227d7724de`

### openGL_3dAssign_13/main.cpp — before

````cpp
#include <glew.h>
#include <glfw3.h>

#include "Console.h"

#include <algorithm>
#include <array>
#include <filesystem>
#include <iostream>
#include <initializer_list>

namespace {

constexpr const char* WINDOW_TITLE = "OpenGL Practice 13";

struct Controls {
    enum class Target { Cuboid, Tetrahedron, Camera };
    Target target = Target::Cuboid;
    int sizeAxis = 0;
    float moveSpeed = 1.0f;
    float resizeSpeed = 0.6f;
    std::array<bool, GLFW_KEY_LAST + 1> pressed{};
};

// 이 두 함수는 다른 .cpp로 옮겨도 같은 콜백 API로 등록할 수 있다.
void onKey(graphics::Application& app, int key, int action, void* userData) {
    Controls& controls = *static_cast<Controls*>(userData);
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            controls.pressed[key] = true;
        } else if (action == GLFW_RELEASE) {
            controls.pressed[key] = false;
        }
    }
    if (action != GLFW_PRESS) {
        return;
    }
    if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) {
        app.requestClose();
    }
    switch (key) {
    case GLFW_KEY_1: controls.target = Controls::Target::Cuboid; break;
    case GLFW_KEY_2: controls.target = Controls::Target::Tetrahedron; break;
    case GLFW_KEY_3: controls.target = Controls::Target::Camera; break;
    case GLFW_KEY_X: controls.sizeAxis = 0; break;
    case GLFW_KEY_Y: controls.sizeAxis = 1; break;
    case GLFW_KEY_Z: controls.sizeAxis = 2; break;
    default: break;
    }
}

void onUpdate(Console& console, float deltaTime, void* userData) {
    const Controls& controls = *static_cast<Controls*>(userData);
    const auto& key = controls.pressed;
    glm::vec3 direction{
        static_cast<float>(key[GLFW_KEY_RIGHT]) - static_cast<float>(key[GLFW_KEY_LEFT]),
        static_cast<float>(key[GLFW_KEY_UP]) - static_cast<float>(key[GLFW_KEY_DOWN]),
        static_cast<float>(key[GLFW_KEY_PAGE_UP]) - static_cast<float>(key[GLFW_KEY_PAGE_DOWN])
    };
    const bool moving = glm::dot(direction, direction) > 0.0f;
    const glm::vec3 movement = moving
        ? glm::normalize(direction) * controls.moveSpeed * deltaTime : glm::vec3(0.0f);

    if (controls.target == Controls::Target::Camera) {
        // 카메라만 이동하고 target은 유지해 입체를 다른 방향에서 볼 수 있다.
        if (moving) {
            graphics::Camera& camera = console.getCamera();
            graphics::Camera candidate = camera;
            candidate.position += movement;
            if (graphics::CameraSystem::validCamera(candidate)) {
                camera = candidate;
            }
        }
        return;
    }

    const auto id = controls.target == Controls::Target::Cuboid
        ? console.getCuboidId() : console.getTetrahedronId();
    if (!id) {
        return;
    }
    graphics::Scene& scene = console.getScene();
    const graphics::Component_Poly* component = scene.getComponent(*id);
    if (component == nullptr) {
        return;
    }
    if (moving) {
        scene.setPosition(*id, component->transform.basis + movement);
    }

    const float sizeDirection =
        static_cast<float>(key[GLFW_KEY_EQUAL] || key[GLFW_KEY_KP_ADD])
        - static_cast<float>(key[GLFW_KEY_MINUS] || key[GLFW_KEY_KP_SUBTRACT]);
    if (sizeDirection != 0.0f) {
        glm::vec3 lengths = component->sideLengths;
        const int axis = controls.sizeAxis;
        lengths[axis] = std::max(0.05f,
            lengths[axis] + sizeDirection * controls.resizeSpeed * deltaTime);
        if (lengths[axis] != component->sideLengths[axis]) {
            scene.setSideLengths(*id, lengths);
        }
    }
}

std::filesystem::path findShaderDirectory() {
    std::filesystem::path directory = std::filesystem::current_path();
    for (int depth = 0; depth < 6; ++depth) {
        const auto local = directory / "framework" / "shaders";
        const auto project = directory / "openGL_3dAssign_13" / "framework" / "shaders";
        for (const auto& candidate : { local, project }) {
            if (std::filesystem::exists(candidate / "VertexShader.glsl")
                && std::filesystem::exists(candidate / "FragmentShader.glsl")) {
                return candidate;
            }
        }
        if (directory == directory.parent_path()) {
            break;
        }
        directory = directory.parent_path();
    }
    return {};
}

} // namespace

int main() {
    graphics::Application application;
    Console example(application);
    Controls controls;
    graphics::ApplicationConfig config;
    config.title = WINDOW_TITLE;
    config.shaderDirectory = findShaderDirectory();
    if (config.shaderDirectory.empty()) {
        std::cerr << "framework/shaders was not found.\n";
        return -1;
    }

    // 장면과 키 명세는 사용자 코드에서 등록한다.
    application.setInitHandler([&](graphics::Application&) {
        return example.initScene();
    });
    application.setKeyHandler([&](graphics::Application& app, int key, int action) {
        onKey(app, key, action, &controls);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        onUpdate(example, deltaTime, &controls);
    });
    application.setFocusHandler([&](graphics::Application&, bool focused) {
        if (!focused) {
            controls.pressed.fill(false);
        }
    });
    std::cout << "1: cuboid, 2: right tetrahedron, 3: camera\n"
              << "Arrows: world X/Y, PageUp/PageDown: world Z\n"
              << "X/Y/Z: resize axis, +/-: resize, Esc/Q: quit\n";
    return application.run(config);
}
````

### after — 위 경로 전체의 최종 저장 내용

````cpp
#include "Console.h"
#include <iostream>

int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 13";
    config.shaderDirectory = graphics::Application::findShaderDirectory();
    if (config.shaderDirectory.empty()) {
        std::cerr << "Shaders were not found beside the executable or in framework/shaders.\n";
        return -1;
    }
    application.setInitHandler([&](graphics::Application&) {
        return practice.initScene();
    });
    application.setKeyHandler([&](graphics::Application&, int key, int action) {
        practice.onKey(key, action);
    });
    return application.run(config);
}
````

## 변경 묶음 5: openGL_assign_789.rc

변경 이유: 내용을 보존하며 UTF-8 BOM 인코딩과 리소스 코드 페이지를 통일한다.

최종 파일 SHA-256: `2199571ee6b1d1351bafca9c7c5bfaaf62329b6479974bd32cc33195a126e09b`

### openGL_3dAssign_13/openGL_assign_789.rc — before

````rc
// Microsoft Visual C++에서 생성한 리소스 스크립트입니다.
//

#include "resource.h"

#define APSTUDIO_READONLY_SYMBOLS
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE 2 리소스에서 생성되었습니다.
//
#include "winres.h"

/////////////////////////////////////////////////////////////////////////////
#undef APSTUDIO_READONLY_SYMBOLS

/////////////////////////////////////////////////////////////////////////////
// 한국어(대한민국) 리소스

#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_ENU)
LANGUAGE 18, 1

#ifdef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE
//

1 TEXTINCLUDE
BEGIN
    "resource.h\0"
END

2 TEXTINCLUDE  
BEGIN
    "#include ""winres.h""\r\n"
    "\0"
END

3 TEXTINCLUDE  
BEGIN
    "\r\n"
    "\0"
END

#endif    // APSTUDIO_INVOKED

#endif    // 한국어(대한민국) 리소스
/////////////////////////////////////////////////////////////////////////////



#ifndef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE 3 리소스에서 생성되었습니다.
//


/////////////////////////////////////////////////////////////////////////////
#endif    // APSTUDIO_INVOKED가 아님
````

### openGL_3dAssign_14/openGL_assign_789.rc — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/openGL_assign_789.rc — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````rc
#pragma code_page(65001)
// Microsoft Visual C++에서 생성한 리소스 스크립트입니다.
//

#include "resource.h"

#define APSTUDIO_READONLY_SYMBOLS
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE 2 리소스에서 생성되었습니다.
//
#include "winres.h"

/////////////////////////////////////////////////////////////////////////////
#undef APSTUDIO_READONLY_SYMBOLS

/////////////////////////////////////////////////////////////////////////////
// 한국어(대한민국) 리소스

#if !defined(AFX_RESOURCE_DLL) || defined(AFX_TARG_ENU)
LANGUAGE 18, 1

#ifdef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE
//

1 TEXTINCLUDE
BEGIN
    "resource.h\0"
END

2 TEXTINCLUDE
BEGIN
    "#include ""winres.h""\r\n"
    "\0"
END

3 TEXTINCLUDE
BEGIN
    "\r\n"
    "\0"
END

#endif    // APSTUDIO_INVOKED

#endif    // 한국어(대한민국) 리소스
/////////////////////////////////////////////////////////////////////////////



#ifndef APSTUDIO_INVOKED
/////////////////////////////////////////////////////////////////////////////
//
// TEXTINCLUDE 3 리소스에서 생성되었습니다.
//


/////////////////////////////////////////////////////////////////////////////
#endif    // APSTUDIO_INVOKED가 아님
````

## 변경 묶음 6: resource.h

변경 이유: 내용을 보존하며 UTF-8 BOM 인코딩과 리소스 코드 페이지를 통일한다.

최종 파일 SHA-256: `c64c1451b98902bb536bf8beb1e2ae2c8c16ca19e70cdcb002ac80d4e938c350`

### openGL_3dAssign_13/resource.h — before

````cpp
//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by openGL_assign_789.rc

// 다음은 새 개체에 사용할 기본값입니다.
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
````

### openGL_3dAssign_14/resource.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/resource.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by openGL_assign_789.rc

// 다음은 새 개체에 사용할 기본값입니다.
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        101
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1001
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif
````

## 변경 묶음 7: Framework.props

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `93260ed4dedba2327185029aacc14e396a04ceb553823edfa12aa9da8a0a786d`

### openGL_3dAssign_13/framework/Framework.props — before

````xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <GraphicsFrameworkDirectory>$(MSBuildThisFileDirectory)</GraphicsFrameworkDirectory>
    <GraphicsFrameworkGlewRoot>$(GraphicsFrameworkDirectory)dependencies\glew-2.3.1\</GraphicsFrameworkGlewRoot>
    <GraphicsFrameworkGlfwRoot>$(GraphicsFrameworkDirectory)dependencies\glfw-3.4.bin.WIN64\</GraphicsFrameworkGlfwRoot>
    <GraphicsFrameworkGlmRoot>$(GraphicsFrameworkDirectory)dependencies\glm\</GraphicsFrameworkGlmRoot>
    <LocalDebuggerWorkingDirectory>$(ProjectDir)</LocalDebuggerWorkingDirectory>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <AdditionalIncludeDirectories>$(GraphicsFrameworkDirectory)include;$(GraphicsFrameworkGlewRoot)include;$(GraphicsFrameworkGlewRoot)include\GL;$(GraphicsFrameworkGlfwRoot)include;$(GraphicsFrameworkGlfwRoot)include\GLFW;$(GraphicsFrameworkGlmRoot);%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>GLFW_DLL;GLFW_INCLUDE_NONE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebugDLL</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreadedDLL</RuntimeLibrary>
    </ClCompile>
    <Link>
      <AdditionalLibraryDirectories>$(GraphicsFrameworkGlewRoot)lib\Release\x64;$(GraphicsFrameworkGlfwRoot)lib-vc2022;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <AdditionalDependencies>opengl32.lib;glew32.lib;glfw3dll.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>
  <Target Name="CheckGraphicsFrameworkDependencies" BeforeTargets="PrepareForBuild">
    <Error Condition="'$(Platform)'!='x64'" Text="The bundled framework libraries support x64. Select Debug|x64 or Release|x64." />
    <ItemGroup>
      <GraphicsFrameworkRequiredFile Include="$(GraphicsFrameworkGlewRoot)include\GL\glew.h;$(GraphicsFrameworkGlewRoot)lib\Release\x64\glew32.lib;$(GraphicsFrameworkGlewRoot)bin\Release\x64\glew32.dll;$(GraphicsFrameworkGlfwRoot)include\GLFW\glfw3.h;$(GraphicsFrameworkGlfwRoot)lib-vc2022\glfw3dll.lib;$(GraphicsFrameworkGlfwRoot)lib-vc2022\glfw3.dll;$(GraphicsFrameworkGlmRoot)glm\glm.hpp" />
    </ItemGroup>
    <Error Condition="!Exists('%(GraphicsFrameworkRequiredFile.Identity)')" Text="Missing framework dependency: %(GraphicsFrameworkRequiredFile.Identity). Copy the complete framework folder including dependencies." />
  </Target>
  <Target Name="CopyGraphicsFrameworkRuntimeDlls" AfterTargets="Build">
    <ItemGroup>
      <GraphicsFrameworkRuntimeDll Include="$(GraphicsFrameworkGlewRoot)bin\Release\x64\glew32.dll;$(GraphicsFrameworkGlfwRoot)lib-vc2022\glfw3.dll" />
    </ItemGroup>
    <Copy SourceFiles="@(GraphicsFrameworkRuntimeDll)" DestinationFolder="$(TargetDir)" SkipUnchangedFiles="true" />
  </Target>
</Project>
````

### openGL_3dAssign_14/framework/Framework.props — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/framework/Framework.props — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### framework/Framework.props — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <PropertyGroup>
    <GraphicsFrameworkDirectory>$(MSBuildThisFileDirectory)</GraphicsFrameworkDirectory>
    <GraphicsFrameworkGlewRoot>$(GraphicsFrameworkDirectory)dependencies\glew-2.3.1\</GraphicsFrameworkGlewRoot>
    <GraphicsFrameworkGlfwRoot>$(GraphicsFrameworkDirectory)dependencies\glfw-3.4.bin.WIN64\</GraphicsFrameworkGlfwRoot>
    <GraphicsFrameworkGlmRoot>$(GraphicsFrameworkDirectory)dependencies\glm\</GraphicsFrameworkGlmRoot>
    <LocalDebuggerWorkingDirectory>$(ProjectDir)</LocalDebuggerWorkingDirectory>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <AdditionalIncludeDirectories>$(GraphicsFrameworkDirectory)include;$(GraphicsFrameworkGlewRoot)include;$(GraphicsFrameworkGlewRoot)include\GL;$(GraphicsFrameworkGlfwRoot)include;$(GraphicsFrameworkGlfwRoot)include\GLFW;$(GraphicsFrameworkGlmRoot);%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>GLFW_DLL;GLFW_INCLUDE_NONE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <AdditionalOptions>/utf-8 %(AdditionalOptions)</AdditionalOptions>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebugDLL</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreadedDLL</RuntimeLibrary>
    </ClCompile>
    <Link>
      <AdditionalLibraryDirectories>$(GraphicsFrameworkGlewRoot)lib\Release\x64;$(GraphicsFrameworkGlfwRoot)lib-vc2022;%(AdditionalLibraryDirectories)</AdditionalLibraryDirectories>
      <AdditionalDependencies>opengl32.lib;glew32.lib;glfw3dll.lib;%(AdditionalDependencies)</AdditionalDependencies>
    </Link>
  </ItemDefinitionGroup>
  <Target Name="CheckGraphicsFrameworkDependencies" BeforeTargets="PrepareForBuild">
    <Error Condition="'$(Platform)'!='x64'" Text="The bundled framework libraries support x64. Select Debug|x64 or Release|x64." />
    <ItemGroup>
      <GraphicsFrameworkRequiredFile Include="$(GraphicsFrameworkGlewRoot)include\GL\glew.h;$(GraphicsFrameworkGlewRoot)lib\Release\x64\glew32.lib;$(GraphicsFrameworkGlewRoot)bin\Release\x64\glew32.dll;$(GraphicsFrameworkGlfwRoot)include\GLFW\glfw3.h;$(GraphicsFrameworkGlfwRoot)lib-vc2022\glfw3dll.lib;$(GraphicsFrameworkGlfwRoot)lib-vc2022\glfw3.dll;$(GraphicsFrameworkGlmRoot)glm\glm.hpp" />
    </ItemGroup>
    <Error Condition="!Exists('%(GraphicsFrameworkRequiredFile.Identity)')" Text="Missing framework dependency: %(GraphicsFrameworkRequiredFile.Identity). Copy the complete framework folder including dependencies." />
  </Target>
  <Target Name="CopyGraphicsFrameworkRuntimeDlls" AfterTargets="Build">
    <ItemGroup>
      <GraphicsFrameworkRuntimeDll Include="$(GraphicsFrameworkGlewRoot)bin\Release\x64\glew32.dll;$(GraphicsFrameworkGlfwRoot)lib-vc2022\glfw3.dll" />
    </ItemGroup>
    <Copy SourceFiles="@(GraphicsFrameworkRuntimeDll)" DestinationFolder="$(TargetDir)" SkipUnchangedFiles="true" />
  </Target>
  <Target Name="CopyGraphicsFrameworkShaders" AfterTargets="Build">
    <ItemGroup>
      <GraphicsFrameworkShader Include="$(GraphicsFrameworkDirectory)shaders\VertexShader.glsl;$(GraphicsFrameworkDirectory)shaders\FragmentShader.glsl" />
    </ItemGroup>
    <Copy SourceFiles="@(GraphicsFrameworkShader)" DestinationFolder="$(TargetDir)shaders\" SkipUnchangedFiles="true" />
  </Target>
</Project>
````

## 변경 묶음 8: README.md

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `54b960b8c109e75cb690fa18a793920a6bdf5d4e6e865000a7dd590d244cb1e9`

### openGL_3dAssign_13/framework/README.md — before

````markdown
# Graphics framework 사용법

이 폴더는 소스 형태로 재사용하는 라이브러리다. 과제의 main.cpp, Console.h,
Console.cpp, Common.h는 필요 없다. 미리 빌드된 .lib나 헤더만으로 동작하는
라이브러리는 아니다. 변경 코드는 UTF-8 BOM이다.

## 다른 프로젝트에 가져오기

1. framework 폴더를 복사한다.
2. include/Application.h 또는 include/Graphics.h를 include한다.
3. src/Graphics.cpp와 src/Application.cpp를 프로젝트의 컴파일 항목에 추가한다.
   .cpp 파일 자체를 #include하지 않는다.
4. C++17 이상과 x64 구성을 선택한다. assign13의 기존 언어 표준은 유지한다.
   dependencies에는 GLEW 2.3.1, GLFW 3.4, GLM 1.0.3과 라이선스가 포함된다.
5. .vcxproj에서 Microsoft.Cpp.targets 직전에 Framework.props를 가져온다.
   프로젝트 폴더 바로 아래에 framework를 복사한 경우 다음과 같이 작성한다.

   <Import Project="framework\Framework.props" />
   <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />

   Framework.props는 자신의 위치인 $(MSBuildThisFileDirectory)를 기준으로
   include와 LIB 경로를 지정한다. 저장소 루트의 OpenGL.props 또는
   컴퓨터별 OpenGL.local.props 설정은 필요하지 않다.
   기존 속성 시트를 대체하며 OpenGL.props와 중복해서 가져오지 않는다.
   GLFW_DLL, GLFW_INCLUDE_NONE, /utf-8, Debug /MDd 및 Release /MD를 적용한다.
   GLEW_STATIC은 정의하지 않는다. opengl32.lib, glew32.lib, glfw3dll.lib를 링크한다.
   빌드 후 glew32.dll과 glfw3.dll을 $(TargetDir)에 자동 복사한다.
   함께 제공한 배포본은 x64 전용이며 Win32를 선택하면 오류를 표시한다.
6. ApplicationConfig::shaderDirectory에 shaders 폴더 경로를 지정한다.
   상대 경로는 프로세스의 현재 작업 폴더 기준이다. Visual Studio 디버깅 시
   Framework.props는 $(ProjectDir)를 작업 폴더로 지정한다.
   터미널에서 실행할 때도 프로젝트 폴더에서 시작하거나 셰이더 절대경로를 지정한다.
   셰이더는 자동 복사하지 않으므로 framework/shaders 폴더를 함께 가져간다.

Renderer만 사용할 때는 Graphics.cpp만 컴파일해도 된다.
이 경우 GLFW는 필수가 아니다. 호출자가 OpenGL 3.3 컨텍스트와 GLEW 초기화를
제공하고 컨텍스트 종료 전에 Renderer::destroy()를 호출한다.

## 사용자 main 예시

아래 코드는 문서 예제다. 빌드와 실행은 수행하지 않았다.

```cpp
#include "framework/include/Application.h"
#include <glfw3.h>

bool createScene(graphics::Application& app) {
    const glm::vec3 lengths{ 1.0f, 2.0f, 0.5f };
    const glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    return app.getScene().addCuboid(lengths, position).has_value();
}

void onKey(graphics::Application& app, int key, int action) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app.requestClose();
    }
}

int main() {
    graphics::Application app;
    graphics::ApplicationConfig config;
    config.title = "My graphics program";
    config.shaderDirectory = "framework/shaders";
    app.setInitHandler(createScene);
    app.setKeyHandler(onKey);
    return app.run(config);
}
```

createScene, onKey, onUpdate는 사용자가 작성한 다른 .h/.cpp에 선언하고
정의해도 된다. main은 해당 헤더를 include하고 콜백을 등록한다.
사용자 상태는 람다로 캡처할 수 있다. 캡처한 객체는 run()이 끝날 때까지 유지한다.

## API와 실행 순서

- ApplicationConfig: 창 크기, 제목, 셰이더 경로, 수직 동기화를 지정한다.
- setInitHandler: 컨텍스트와 렌더러 초기화 후 한 번 호출한다. bool을 반환한다.
- setKeyHandler: GLFW의 key와 action을 전달한다. 종료 키는 사용자가 지정한다.
- setFocusHandler: 창의 포커스 변경을 bool로 전달한다.
- setUpdateHandler: 프레임마다 deltaTime(초)을 전달한다. 최대 0.05초로 제한한다.
- setRenderHandler: 등록하지 않으면 기본 Scene/Camera를 렌더링한다.
  등록하면 기본 렌더링을 대체한다. 필요하면 콜백에서 renderScene()을 호출한다.
  bool 반환이 false면 run()이 -1로 끝난다.
- getScene/getCamera: 사용자 코드에서 장면과 카메라를 변경한다.
- requestClose: 반복 루프 종료를 요청한다.
- run: 창 생성 → 초기화 콜백 → 이벤트/업데이트/렌더링/화면 교체 → 자원 해제.
  초기화 실패도 자원을 해제하고 -1을 반환한다. 중첩 run 호출은 허용하지 않는다.
  최소화 상태에서는 업데이트와 렌더링을 멈추고 이벤트를 기다린다.

Application은 한 스레드에서 하나씩 사용하며 GLFW 초기화/종료를 소유한다.
다른 GLFW 프로그램과 컨텍스트를 공유해야 하면 Application 대신 Renderer를 쓴다.
GLFW window user pointer와 key/focus callback은 Application이 관리한다.
getWindow()로 이를 덮어쓰지 않는다. 콜백 실행 중 콜백 자체를 교체하지 않는다.
초기화 콜백에서 사용자가 장면을 구성하기 전에는 Scene이 비어 있다.
run을 다시 호출하면 기존 Scene/Camera 데이터는 유지되고 GPU 자원은 다시 생성된다.

CPU Scene 데이터 → VAO/VBO/EBO → CPU의 P*V*M 계산 → uMVP 전달 →
vertex shader → 삼각형/선 → 깊이 검사 → 화면 순서다.
GLSL은 layout 0의 vec3 위치와 layout 1의 vec3 색상, mat4 uMVP를 사용한다.

Scene은 직육면체, 세 모서리가 직교하는 직사면체, Line, vec3 벡터, basis를
제공한다. 현재 용량은 MAX_ENTITIES=50, MAX_LINES=500이다.
도형의 worldPosition은 로컬 무게중심을 옮긴 위치다.
addBasis의 mat3 열이 기저 벡터이며, owner를 주면 객체 변환을 따른다.

## 사용자가 수행할 확인

빌드와 테스트는 실행하지 않았다. Visual Studio에서 Debug|x64 및 Release|x64를
직접 빌드한다. 기존 예제에서 도형/선 표시, 이동/크기 변경, 카메라 이동,
창 크기 변경, 최소화/복원, 포커스 변경, Esc/Q 종료를 확인한다.
다른 프로젝트에서는 Console과 기존 main을 제외하고 위 main 예시로 연결한다.
셰이더 경로 오류 시 초기화 실패, init callback의 false 반환 시 정상 정리를 확인한다.

전체 저장소의 graphify 그래프 생성/갱신은 이번 승인 범위에 포함되지 않아
수행하지 않았다. 위 흐름은 소스 정적 확인에 따른 설명이다.

## 의존성 포함 변경

CHANGES-DEPENDENCIES.md는 이번 작업 시작 시점과 최종 설정·사용법을 모두 대조한다.
DEPENDENCIES.patch는 추가한 의존성 전체와 LIB/DLL의 Git binary patch를 포함한다.
제3자 텍스트는 UTF-8 BOM만 적용했으며 내용은 원본과 같다. 바이너리는 그대로 복사했다.

## 상대경로와 DLL 확인은 사용자가 수행

1. Visual Studio에서 openGL_assign_13.slnx를 열고 Debug|x64와 Release|x64를 빌드한다.
2. 각 출력 폴더의 실행 파일 옆에 glew32.dll과 glfw3.dll이 있는지 확인한다.
3. F5로 실행하고 도형·선·카메라 및 기존 입력 동작을 확인한다.
4. framework 폴더를 다른 x64 프로젝트에 복사한다. 두 .cpp를 등록하고
   Framework.props를 가져온 뒤 사용자 main으로 빌드·실행한다.
5. 프로젝트 속성에서 include/LIB 경로가 복사한 framework 내부를 가리키는지 확인한다.

위 빌드 및 실행 테스트는 이 작업에서 수행하지 않았다.
````

### openGL_3dAssign_14/framework/README.md — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/framework/README.md — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### framework/README.md — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````markdown
# Graphics framework 사용법

이 폴더는 소스 형태로 재사용하는 라이브러리다. 과제의 main.cpp, Console.h,
Console.cpp, Common.h는 필요 없다. 미리 빌드된 .lib나 헤더만으로 동작하는
라이브러리는 아니다. 변경 코드는 UTF-8 BOM이다.

## 다른 프로젝트에 가져오기

1. framework 폴더를 복사한다.
2. include/Application.h 또는 include/Graphics.h를 include한다.
3. src/Graphics.cpp와 src/Application.cpp를 프로젝트의 컴파일 항목에 추가한다.
   .cpp 파일 자체를 #include하지 않는다.
4. C++17 이상과 x64 구성을 선택한다. assign13의 기존 언어 표준은 유지한다.
   dependencies에는 GLEW 2.3.1, GLFW 3.4, GLM 1.0.3과 라이선스가 포함된다.
5. .vcxproj에서 Microsoft.Cpp.targets 직전에 Framework.props를 가져온다.
   프로젝트 폴더 바로 아래에 framework를 복사한 경우 다음과 같이 작성한다.

   <Import Project="framework\Framework.props" />
   <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />

   Framework.props는 자신의 위치인 $(MSBuildThisFileDirectory)를 기준으로
   include와 LIB 경로를 지정한다. 저장소 루트의 OpenGL.props 또는
   컴퓨터별 OpenGL.local.props 설정은 필요하지 않다.
   기존 속성 시트를 대체하며 OpenGL.props와 중복해서 가져오지 않는다.
   GLFW_DLL, GLFW_INCLUDE_NONE, /utf-8, Debug /MDd 및 Release /MD를 적용한다.
   GLEW_STATIC은 정의하지 않는다. opengl32.lib, glew32.lib, glfw3dll.lib를 링크한다.
   빌드 후 glew32.dll과 glfw3.dll을 $(TargetDir)에 자동 복사한다.
   함께 제공한 배포본은 x64 전용이며 Win32를 선택하면 오류를 표시한다.
6. ApplicationConfig::shaderDirectory에 shaders 폴더 경로를 지정한다.
   상대 경로는 프로세스의 현재 작업 폴더 기준이다. Visual Studio 디버깅 시
   Framework.props는 $(ProjectDir)를 작업 폴더로 지정한다.
   터미널에서 실행할 때도 프로젝트 폴더에서 시작하거나 셰이더 절대경로를 지정한다.
   셰이더는 빌드 후 실행 파일 옆의 shaders 폴더에 자동 복사한다.
   Application::findShaderDirectory()는 실행 파일 옆을 먼저 확인하고,
   현재 작업 폴더와 상위 폴더의 framework/shaders를 확인한다.

Renderer만 사용할 때는 Graphics.cpp만 컴파일해도 된다.
이 경우 GLFW는 필수가 아니다. 호출자가 OpenGL 3.3 컨텍스트와 GLEW 초기화를
제공하고 컨텍스트 종료 전에 Renderer::destroy()를 호출한다.

## 사용자 main 예시

아래 코드는 문서 예제다. 빌드와 실행은 수행하지 않았다.

```cpp
#include "framework/include/Application.h"
#include <glfw3.h>

bool createScene(graphics::Application& app) {
    const glm::vec3 lengths{ 1.0f, 2.0f, 0.5f };
    const glm::vec3 position{ 0.0f, 0.0f, 0.0f };
    return app.getScene().addCuboid(lengths, position).has_value();
}

void onKey(graphics::Application& app, int key, int action) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        app.requestClose();
    }
}

int main() {
    graphics::Application app;
    graphics::ApplicationConfig config;
    config.title = "My graphics program";
    config.shaderDirectory = "framework/shaders";
    app.setInitHandler(createScene);
    app.setKeyHandler(onKey);
    return app.run(config);
}
```

createScene, onKey, onUpdate는 사용자가 작성한 다른 .h/.cpp에 선언하고
정의해도 된다. main은 해당 헤더를 include하고 콜백을 등록한다.
사용자 상태는 람다로 캡처할 수 있다. 캡처한 객체는 run()이 끝날 때까지 유지한다.

## API와 실행 순서

- ApplicationConfig: 창 크기, 제목, 셰이더 경로, 수직 동기화를 지정한다.
- setInitHandler: 컨텍스트와 렌더러 초기화 후 한 번 호출한다. bool을 반환한다.
- setKeyHandler: GLFW의 key와 action을 전달한다. 종료 키는 사용자가 지정한다.
- setFocusHandler: 창의 포커스 변경을 bool로 전달한다.
- setUpdateHandler: 프레임마다 deltaTime(초)을 전달한다. 최대 0.05초로 제한한다.
- setRenderHandler: 등록하지 않으면 기본 Scene/Camera를 렌더링한다.
  등록하면 기본 렌더링을 대체한다. 필요하면 콜백에서 renderScene()을 호출한다.
  bool 반환이 false면 run()이 -1로 끝난다.
- getScene/getCamera: 사용자 코드에서 장면과 카메라를 변경한다.
- requestClose: 반복 루프 종료를 요청한다.
- run: 창 생성 → 초기화 콜백 → 이벤트/업데이트/렌더링/화면 교체 → 자원 해제.
  초기화 실패도 자원을 해제하고 -1을 반환한다. 중첩 run 호출은 허용하지 않는다.
  최소화 상태에서는 업데이트와 렌더링을 멈추고 이벤트를 기다린다.

Application은 한 스레드에서 하나씩 사용하며 GLFW 초기화/종료를 소유한다.
다른 GLFW 프로그램과 컨텍스트를 공유해야 하면 Application 대신 Renderer를 쓴다.
GLFW window user pointer와 key/focus callback은 Application이 관리한다.
getWindow()로 이를 덮어쓰지 않는다. 콜백 실행 중 콜백 자체를 교체하지 않는다.
초기화 콜백에서 사용자가 장면을 구성하기 전에는 Scene이 비어 있다.
run을 다시 호출하면 기존 Scene/Camera 데이터는 유지되고 GPU 자원은 다시 생성된다.

CPU Scene 데이터 → VAO/VBO/EBO → CPU의 P*V*M 계산 → uMVP 전달 →
vertex shader → 삼각형/선 → 깊이 검사 → 화면 순서다.
GLSL은 layout 0의 vec3 위치와 layout 1의 vec3 색상, mat4 uMVP를 사용한다.

Scene은 직육면체, 세 모서리가 직교하는 직사면체, Line, vec3 벡터, basis를
제공한다. 현재 용량은 MAX_ENTITIES=50, MAX_LINES=500이다.
도형의 worldPosition은 로컬 무게중심을 옮긴 위치다.
addBasis의 mat3 열이 기저 벡터이며, owner를 주면 객체 변환을 따른다.

## 사용자가 수행할 확인

빌드와 테스트는 실행하지 않았다. Visual Studio에서 Debug|x64 및 Release|x64를
직접 빌드한다. 기존 예제에서 도형/선 표시, 이동/크기 변경, 카메라 이동,
창 크기 변경, 최소화/복원, 포커스 변경, Esc/Q 종료를 확인한다.
다른 프로젝트에서는 Console과 기존 main을 제외하고 위 main 예시로 연결한다.
셰이더 경로 오류 시 초기화 실패, init callback의 false 반환 시 정상 정리를 확인한다.

전체 저장소의 graphify 그래프 생성/갱신은 이번 승인 범위에 포함되지 않아
수행하지 않았다. 위 흐름은 소스 정적 확인에 따른 설명이다.

## 의존성 포함 변경

CHANGES-DEPENDENCIES.md는 이번 작업 시작 시점과 최종 설정·사용법을 모두 대조한다.
DEPENDENCIES.patch는 추가한 의존성 전체와 LIB/DLL의 Git binary patch를 포함한다.
제3자 텍스트는 UTF-8 BOM만 적용했으며 내용은 원본과 같다. 바이너리는 그대로 복사했다.

## 상대경로와 DLL 확인은 사용자가 수행

1. Visual Studio에서 openGL_assign_13.slnx를 열고 Debug|x64와 Release|x64를 빌드한다.
2. 각 출력 폴더의 실행 파일 옆에 glew32.dll과 glfw3.dll이 있는지 확인한다.
3. F5로 실행하고 도형·선·카메라 및 기존 입력 동작을 확인한다.
4. framework 폴더를 다른 x64 프로젝트에 복사한다. 두 .cpp를 등록하고
   Framework.props를 가져온 뒤 사용자 main으로 빌드·실행한다.
5. 프로젝트 속성에서 include/LIB 경로가 복사한 framework 내부를 가리키는지 확인한다.

위 빌드 및 실행 테스트는 이 작업에서 수행하지 않았다.

## 실습 13·14·15 공용 확장

- addPyramid: 바닥 X폭, Y높이, 바닥 Z폭. 면 순서는 앞·오른쪽·뒤·왼쪽·바닥이다.
- addCuboid: 기존 앞·뒤·왼쪽·오른쪽·위·아래 면 순서를 유지한다.
- addRightTetrahedron: 기존 직사면체 API를 유지한다. 사각뿔과 별도 형상이다.
- setFaceVisible: draw 단계에서 표시 여부를 읽으며 geometry revision은 바꾸지 않는다.
- setFaceLocalMatrix: 일반 면의 Model은 객체 Model × localAnimationMatrix이다.
  독립 면은 기존 월드 transform만 사용한다. 행렬은 유한한 값이어야 한다.
- setFaceVertexColors: 로컬 꼭짓점 수와 같은 색상 배열을 지정한다.
  빈 배열은 단색으로 복원한다. 색상 변경은 geometry revision을 갱신한다.
- Transform3D::orientation: 정규화한 쿼터니언을 Euler 회전 다음, scale 전에 적용한다.
  기본 단위 쿼터니언은 기존 Euler 코드의 결과를 유지한다.
- Camera::orthographic: 짧은 화면 축의 범위는 ±orthographicHalfExtent이다.
  긴 축은 aspect에 맞춰 넓혀 화면 비율을 유지한다. 기본값은 기존 원근 투영이다.
- getRenderOptions(): depthTest와 wireframe을 설정한다. 기본은 깊이 켜짐·솔리드이다.
  와이어는 GL_LINE_LOOP로 면 테두리를 그리며 삼각분할 대각선은 제외한다.
- setModifiedKeyHandler: key/action/mods를 전달한다. 설정하면 기존 keyHandler를 대체한다.
  GLFW_LOCK_KEY_MODS를 켜므로 Shift와 Caps Lock을 함께 판별할 수 있다.

입력 → 장면 상태 → 면 행렬 → 객체 Model → View/Projection → uMVP → 화면.
형상·색상 변경만 GPU 버퍼를 다시 올린다. 면 표시·변환은 draw 때 읽는다.
빌드와 실행 테스트는 사용자에게 맡긴다. 정적 검토만으로 실행 성공을 보장하지 않는다.
````

## 변경 묶음 9: Application.h

변경 이유: 루프 소유권을 유지하며 수정키 전달, 렌더 옵션, 실행 파일 기준 셰이더 탐색을 연결한다.

최종 파일 SHA-256: `f8111ad06e4028525f0351eb7b9331b9c6b769832a26ceece6f040175cbfb39a`

### openGL_3dAssign_13/framework/include/Application.h — before

````cpp
#pragma once

#include "Graphics.h"
#include <functional>

struct GLFWwindow;

namespace graphics {

struct ApplicationConfig {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    std::string title = "OpenGL";
    std::filesystem::path shaderDirectory = "framework/shaders";
    bool verticalSync = true;
};

// 콜백에 캡처한 객체는 run()이 끝날 때까지 유지한다.
// 한 스레드에서 Application 하나를 사용한다. GLFW 초기화와 종료를 소유한다.
class Application {
private:
    GLFWwindow* window_ = nullptr;
    bool glfwInitialized_ = false;
    bool running_ = false;
    Scene scene_;
    Camera camera_;
    Renderer renderer_;
    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
    std::function<void(Application&, int, int)> keyHandler_;
    std::function<void(Application&, bool)> focusHandler_;
    std::function<bool(Application&)> renderHandler_;

    bool initialize(const ApplicationConfig& config);
    void shutdown();

public:
    Application() = default;
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void setInitHandler(std::function<bool(Application&)> handler);
    void setUpdateHandler(std::function<void(Application&, float)> handler);
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);
    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);
    int run(const ApplicationConfig& config = {});
    void requestClose();
    bool renderScene();

    Scene& getScene() { return scene_; }
    Camera& getCamera() { return camera_; }
    GLFWwindow* getWindow() const { return window_; }
};

} // namespace graphics
````

### openGL_3dAssign_14/framework/include/Application.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/framework/include/Application.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### framework/include/Application.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#pragma once

#include "Graphics.h"
#include <functional>

struct GLFWwindow;

namespace graphics {

struct ApplicationConfig {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    std::string title = "OpenGL";
    std::filesystem::path shaderDirectory = "framework/shaders";
    bool verticalSync = true;
};

// 콜백에 캡처한 객체는 run()이 끝날 때까지 유지한다.
// 한 스레드에서 Application 하나를 사용한다. GLFW 초기화와 종료를 소유한다.
class Application {
private:
    GLFWwindow* window_ = nullptr;
    bool glfwInitialized_ = false;
    bool running_ = false;
    Scene scene_;
    Camera camera_;
    Renderer renderer_;
    RenderOptions renderOptions_;
    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
    std::function<void(Application&, int, int)> keyHandler_;
    std::function<void(Application&, int, int, int)> modifiedKeyHandler_;
    std::function<void(Application&, bool)> focusHandler_;
    std::function<bool(Application&)> renderHandler_;

    bool initialize(const ApplicationConfig& config);
    void shutdown();

public:
    Application() = default;
    ~Application();
    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void setInitHandler(std::function<bool(Application&)> handler);
    void setUpdateHandler(std::function<void(Application&, float)> handler);
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);
    // 등록된 경우 기존 keyHandler 대신 key/action/mods를 전달한다.
    void setModifiedKeyHandler(
        std::function<void(Application&, int key, int action, int mods)> handler);
    static std::filesystem::path findShaderDirectory();
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);
    // 없으면 renderScene()을 호출한다. 사용자 콜백은 필요할 때 직접 renderScene()을 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);
    int run(const ApplicationConfig& config = {});
    void requestClose();
    bool renderScene();

    RenderOptions& getRenderOptions() { return renderOptions_; }
    Scene& getScene() { return scene_; }
    Camera& getCamera() { return camera_; }
    GLFWwindow* getWindow() const { return window_; }
};

} // namespace graphics
````

## 변경 묶음 10: Graphics.h

변경 이유: 사각뿔, 꼭짓점 색상, 면 표시·변환, 렌더 옵션을 지원한다. 호환 헤더 사본은 기존 계약을 유지한다.

최종 파일 SHA-256: `7012eb07d3ea194f398db3da02a3ef1941ebe47715883e330d2939cdbcd2f70f`

### openGL_3dAssign_13/framework/include/Graphics.h — before

````cpp
#pragma once

#include <glew.h>
#include <glm/glm.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_ENTITIES = 50;
constexpr std::size_t MAX_LINES = 500;

using EntityId = std::size_t;
using LineId = std::size_t;

struct GlColor {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
};

enum class GLShape { None, Cube, Tetra }; // Cube는 직육면체, Tetra는 직사면체.
enum class GLFace { None, Rect, Triangle };

struct Transform3D {
    glm::vec3 basis{ 0.0f }; // 월드 위치. 방향 기저는 addBasis의 mat3로 구분한다.
    glm::vec3 localBasis{ 0.0f };
    glm::vec3 rotationDegrees{ 0.0f };
    glm::vec3 scale{ 1.0f };
};

struct PlaneFigure {
    GLFace shape = GLFace::None;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{ 0.0f }; // 로컬 법선 데이터. 조명은 현재 사용하지 않는다.
    bool independence = false;
    Transform3D transform;   // 독립 면일 때의 월드 변환.
};

struct Polyhedron {
    GLShape shape = GLShape::None;
    std::vector<PlaneFigure> faces;
};

struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{ 1.0f }; // scale과 구분한 실제 로컬 모서리 길이.
};

struct FaceId {
    EntityId entityId = 0;
    std::size_t faceIndex = 0;
};

struct Line3D {
    glm::vec3 start{ 0.0f };
    glm::vec3 end{ 0.0f };
    GlColor color;
    std::optional<EntityId> owner; // 없으면 월드 좌표, 있으면 객체의 로컬 좌표.
};

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;

public:
    Scene();
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = {});
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths,
                                     const glm::vec3& worldPosition);
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths,
                                               const glm::vec3& worldPosition);
    bool setPosition(EntityId id, const glm::vec3& worldPosition);
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
    bool setTransform(EntityId id, const Transform3D& transform);
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end,
                                 GlColor color, std::optional<EntityId> owner = {});
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector,
                                   GlColor color, std::optional<EntityId> owner = {});
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    std::optional<std::array<LineId, 3>> addBasis(
        const glm::vec3& origin, const glm::mat3& basisVectors = glm::mat3(1.0f),
        const glm::vec3& axisLengths = glm::vec3(1.0f),
        std::optional<EntityId> owner = {});
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.

    const std::vector<EntityId>& getIds() const { return polyId_; }
    const Component_Poly* getComponent(EntityId id) const;
    const std::vector<FaceId>& getCubeFaceIds() const { return cubeFaceId_; }
    const std::vector<FaceId>& getTetraFaceIds() const { return tetraFaceId_; }
    const std::vector<Line3D>& getLines() const { return lines_; }
    std::uint64_t getGeometryRevision() const { return geometryRevision_; }
};

class DiagramSystem {
public:
    static Polyhedron makePolyhedron(GLShape shape,
                                     const glm::vec3& sideLengths = glm::vec3(1.0f));
    static bool validPolyhedron(const Polyhedron& polyhedron);
    static bool validTransform(const Transform3D& transform);
    static glm::mat4 modelMatrix(const Transform3D& transform);
    static glm::mat4 faceModelMatrix(const Component_Poly& component,
                                    const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{ 2.8f, 2.0f, 5.0f };
    glm::vec3 target{ 0.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
};

class CameraSystem {
public:
    static bool validCamera(const Camera& camera);
    static glm::mat4 viewMatrix(const Camera& camera);
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

std::string readTextFile(const std::filesystem::path& path);

class Renderer {
private:
    struct DrawRange {
        GLenum mode = GL_TRIANGLES;
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0;
        GLsizei count = 0;
    };

    GLuint shaderProgramID_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_[2] = { 0, 0 };
    GLuint ebo_ = 0;
    GLint mvpLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();

    bool initShader(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    bool initBuffer();
    bool upload(const Scene& scene);
    bool draw(const Scene& scene, const glm::mat4& view,
              const glm::mat4& projection) const;

public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // 호출자가 유효한 OpenGL 3.3 컨텍스트를 current로 설정한 뒤 호출한다.
    bool initialize(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    // viewport/depth/clear/program/VAO를 설정한다. 컨텍스트와 윈도우는 소유하지 않는다.
    bool render(const Scene& scene, const Camera& camera, int width, int height);
    void destroy(); // 컨텍스트 종료 전에 호출한다. 여러 번 호출해도 된다.
};

} // namespace graphics
````

### openGL_3dAssign_14/framework/include/Graphics.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/framework/include/Graphics.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### framework/include/Graphics.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#pragma once

#include <glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
constexpr std::size_t MAX_ENTITIES = 50;
constexpr std::size_t MAX_LINES = 500;

using EntityId = std::size_t;
using LineId = std::size_t;

struct GlColor {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
};

enum class GLShape { None, Cube, Tetra, Pyramid }; // Cube는 직육면체, Tetra는 직사면체.
enum class GLFace { None, Rect, Triangle };

struct Transform3D {
    glm::vec3 basis{ 0.0f }; // 월드 위치. 방향 기저는 addBasis의 mat3로 구분한다.
    glm::vec3 localBasis{ 0.0f };
    glm::vec3 rotationDegrees{ 0.0f };
    glm::vec3 scale{ 1.0f };
    glm::quat orientation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Euler 회전 다음에 적용한다.
};

struct PlaneFigure {
    GLFace shape = GLFace::None;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{ 0.0f }; // 로컬 법선 데이터. 조명은 현재 사용하지 않는다.
    bool independence = false;
    Transform3D transform;   // 독립 면일 때의 월드 변환.
    bool visible = true;
    glm::mat4 localAnimationMatrix{ 1.0f };
    std::vector<GlColor> vertexColors; // 비어 있으면 면의 단색을 사용한다.
};

struct Polyhedron {
    GLShape shape = GLShape::None;
    std::vector<PlaneFigure> faces;
};

struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{ 1.0f }; // scale과 구분한 실제 로컬 모서리 길이.
};

struct FaceId {
    EntityId entityId = 0;
    std::size_t faceIndex = 0;
};

struct Line3D {
    glm::vec3 start{ 0.0f };
    glm::vec3 end{ 0.0f };
    GlColor color;
    std::optional<EntityId> owner; // 없으면 월드 좌표, 있으면 객체의 로컬 좌표.
};

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;

public:
    Scene();
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = {});
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths,
                                     const glm::vec3& worldPosition);
    std::optional<EntityId> addRightTetrahedron(const glm::vec3& edgeLengths,
                                               const glm::vec3& worldPosition);
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths,
                                      const glm::vec3& worldPosition);
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex,
                             const std::vector<GlColor>& colors);
    bool setPosition(EntityId id, const glm::vec3& worldPosition);
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);
    bool setTransform(EntityId id, const Transform3D& transform);
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end,
                                 GlColor color, std::optional<EntityId> owner = {});
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector,
                                   GlColor color, std::optional<EntityId> owner = {});
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    std::optional<std::array<LineId, 3>> addBasis(
        const glm::vec3& origin, const glm::mat3& basisVectors = glm::mat3(1.0f),
        const glm::vec3& axisLengths = glm::vec3(1.0f),
        std::optional<EntityId> owner = {});
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);
    void clear(); // ID는 clear 이후 재사용된다. 기존 ID를 계속 사용하지 않는다.

    const std::vector<EntityId>& getIds() const { return polyId_; }
    const Component_Poly* getComponent(EntityId id) const;
    const std::vector<FaceId>& getCubeFaceIds() const { return cubeFaceId_; }
    const std::vector<FaceId>& getTetraFaceIds() const { return tetraFaceId_; }
    const std::vector<FaceId>& getPyramidFaceIds() const { return pyramidFaceId_; }
    const std::vector<Line3D>& getLines() const { return lines_; }
    std::uint64_t getGeometryRevision() const { return geometryRevision_; }
};

class DiagramSystem {
public:
    static Polyhedron makePolyhedron(GLShape shape,
                                     const glm::vec3& sideLengths = glm::vec3(1.0f));
    static bool validPolyhedron(const Polyhedron& polyhedron);
    static bool validTransform(const Transform3D& transform);
    static glm::mat4 modelMatrix(const Transform3D& transform);
    static glm::mat4 faceModelMatrix(const Component_Poly& component,
                                    const PlaneFigure& face);
};

struct Camera {
    glm::vec3 position{ 2.8f, 2.0f, 5.0f };
    glm::vec3 target{ 0.0f };
    glm::vec3 up{ 0.0f, 1.0f, 0.0f };
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

class CameraSystem {
public:
    static bool validCamera(const Camera& camera);
    static glm::mat4 viewMatrix(const Camera& camera);
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

std::string readTextFile(const std::filesystem::path& path);

struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
};

class Renderer {
private:
    struct DrawRange {
        GLenum mode = GL_TRIANGLES;
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0;
        GLsizei count = 0;
        std::size_t outlineFirstIndex = 0;
        GLsizei outlineCount = 0;
    };

    GLuint shaderProgramID_ = 0;
    GLuint vao_ = 0;
    GLuint vbo_[2] = { 0, 0 };
    GLuint ebo_ = 0;
    GLint mvpLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();

    bool initShader(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    bool initBuffer();
    bool upload(const Scene& scene);
    bool draw(const Scene& scene, const glm::mat4& view,
              const glm::mat4& projection, const RenderOptions& options) const;

public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    // 호출자가 유효한 OpenGL 3.3 컨텍스트를 current로 설정한 뒤 호출한다.
    bool initialize(const std::filesystem::path& vertexPath,
                    const std::filesystem::path& fragmentPath);
    // viewport/depth/clear/program/VAO를 설정한다. 컨텍스트와 윈도우는 소유하지 않는다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = {});
    void destroy(); // 컨텍스트 종료 전에 호출한다. 여러 번 호출해도 된다.
};

} // namespace graphics
````

## 변경 묶음 11: Application.cpp

변경 이유: 루프 소유권을 유지하며 수정키 전달, 렌더 옵션, 실행 파일 기준 셰이더 탐색을 연결한다.

최종 파일 SHA-256: `d2c5275067b6352e5fedd7f58649a2d82a2158fa928231319f2e1cec5cd610f4`

### openGL_3dAssign_13/framework/src/Application.cpp — before

````cpp
#include "../include/Application.h"
#include <glfw3.h>

#include <algorithm>
#include <iostream>
#include <utility>

namespace graphics {

Application::~Application() {
    shutdown();
}

void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::initialize(const ApplicationConfig& config) {
    if (config.width <= 0 || config.height <= 0 || !glfwInit()) {
        return false;
    }
    glfwInitialized_ = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    window_ = glfwCreateWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(config.verticalSync ? 1 : 0);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return false;
    }
    if (!renderer_.initialize(config.shaderDirectory / "VertexShader.glsl",
                              config.shaderDirectory / "FragmentShader.glsl")) {
        return false;
    }

    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app.keyHandler_) {
            app.keyHandler_(app, key, action);
        }
    });
    glfwSetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app.focusHandler_) {
            app.focusHandler_(app, focused == GLFW_TRUE);
        }
    });
    return true;
}

void Application::shutdown() {
    if (window_ != nullptr) {
        glfwMakeContextCurrent(window_);
        renderer_.destroy();
        glfwSetWindowUserPointer(window_, nullptr);
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    if (glfwInitialized_) {
        glfwTerminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window_);
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    return renderer_.render(scene_, camera_, width, height);
}

int Application::run(const ApplicationConfig& config) {
    if (running_) {
        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }
    running_ = true;
    int exitCode = 0;
    try {
        if (!initialize(config)) {
            std::cerr << "Application initialization failed.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            std::cerr << "User scene initialization failed.\n";
            exitCode = -1;
        } else {
            double previousTime = glfwGetTime();
            while (!glfwWindowShouldClose(window_)) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window_)) {
                    break;
                }
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(window_, &width, &height);
                if (width <= 0 || height <= 0) {
                    glfwWaitEvents();
                    previousTime = glfwGetTime();
                    continue;
                }
                const double currentTime = glfwGetTime();
                const float deltaTime = static_cast<float>(
                    std::clamp(currentTime - previousTime, 0.0, 0.05));
                previousTime = currentTime;
                if (updateHandler_) {
                    updateHandler_(*this, deltaTime);
                }
                if (glfwWindowShouldClose(window_)) {
                    break;
                }
                glfwMakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
                if (!rendered) {
                    std::cerr << "Rendering failed.\n";
                    exitCode = -1;
                    break;
                }
                glfwSwapBuffers(window_);
            }
        }
    } catch (...) {
        shutdown();
        running_ = false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    return exitCode;
}

} // namespace graphics
````

### openGL_3dAssign_14/framework/src/Application.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/framework/src/Application.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### framework/src/Application.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "../include/Application.h"
#include <glfw3.h>

#include <algorithm>
#include <iostream>
#include <utility>
#ifdef _WIN32
#include <windows.h>
#endif

namespace graphics {

Application::~Application() {
    shutdown();
}

void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

void Application::setModifiedKeyHandler(
    std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

std::filesystem::path Application::findShaderDirectory() {
    const auto containsShaders = [](const std::filesystem::path& path) {
        return std::filesystem::is_regular_file(path / "VertexShader.glsl")
            && std::filesystem::is_regular_file(path / "FragmentShader.glsl");
    };
#ifdef _WIN32
    // 실행 파일 옆의 shaders를 우선한다. 작업 폴더와 프로젝트 이름에 의존하지 않는다.
    std::wstring executable(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(),
                                           static_cast<DWORD>(executable.size()));
    if (length > 0 && length < executable.size()) {
        executable.resize(length);
        const auto candidate = std::filesystem::path(executable).parent_path() / "shaders";
        if (containsShaders(candidate)) {
            return candidate;
        }
    }
#endif
    auto directory = std::filesystem::current_path();
    for (;;) {
        const auto candidate = directory / "framework" / "shaders";
        if (containsShaders(candidate)) {
            return candidate;
        }
        if (directory == directory.parent_path()) {
            return {};
        }
        directory = directory.parent_path();
    }
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::initialize(const ApplicationConfig& config) {
    if (config.width <= 0 || config.height <= 0 || !glfwInit()) {
        return false;
    }
    glfwInitialized_ = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    window_ = glfwCreateWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window_);
    glfwSwapInterval(config.verticalSync ? 1 : 0);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return false;
    }
    if (!renderer_.initialize(config.shaderDirectory / "VertexShader.glsl",
                              config.shaderDirectory / "FragmentShader.glsl")) {
        return false;
    }

    glfwSetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app.modifiedKeyHandler_) {
            app.modifiedKeyHandler_(app, key, action, mods);
        } else if (app.keyHandler_) {
            app.keyHandler_(app, key, action);
        }
    });
    glfwSetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));
        if (app.focusHandler_) {
            app.focusHandler_(app, focused == GLFW_TRUE);
        }
    });
    return true;
}

void Application::shutdown() {
    if (window_ != nullptr) {
        glfwMakeContextCurrent(window_);
        renderer_.destroy();
        glfwSetWindowUserPointer(window_, nullptr);
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }
    if (glfwInitialized_) {
        glfwTerminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return false;
    }
    glfwMakeContextCurrent(window_);
    int width = 0;
    int height = 0;
    glfwGetFramebufferSize(window_, &width, &height);
    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

int Application::run(const ApplicationConfig& config) {
    if (running_) {
        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }
    running_ = true;
    int exitCode = 0;
    try {
        if (!initialize(config)) {
            std::cerr << "Application initialization failed.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            std::cerr << "User scene initialization failed.\n";
            exitCode = -1;
        } else {
            double previousTime = glfwGetTime();
            while (!glfwWindowShouldClose(window_)) {
                glfwPollEvents();
                if (glfwWindowShouldClose(window_)) {
                    break;
                }
                int width = 0;
                int height = 0;
                glfwGetFramebufferSize(window_, &width, &height);
                if (width <= 0 || height <= 0) {
                    glfwWaitEvents();
                    previousTime = glfwGetTime();
                    continue;
                }
                const double currentTime = glfwGetTime();
                const float deltaTime = static_cast<float>(
                    std::clamp(currentTime - previousTime, 0.0, 0.05));
                previousTime = currentTime;
                if (updateHandler_) {
                    updateHandler_(*this, deltaTime);
                }
                if (glfwWindowShouldClose(window_)) {
                    break;
                }
                glfwMakeContextCurrent(window_);
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
                if (!rendered) {
                    std::cerr << "Rendering failed.\n";
                    exitCode = -1;
                    break;
                }
                glfwSwapBuffers(window_);
            }
        }
    } catch (...) {
        shutdown();
        running_ = false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    return exitCode;
}

} // namespace graphics
````

## 변경 묶음 12: Graphics.cpp

변경 이유: 사각뿔, 꼭짓점 색상, 면 표시·변환, 렌더 옵션을 지원한다. 호환 헤더 사본은 기존 계약을 유지한다.

최종 파일 SHA-256: `710d3fb2abaeaec90856bf7a0e49480ca9faddce18b4edfe5d8b0b2ce678ea79`

### openGL_3dAssign_13/framework/src/Graphics.cpp — before

````cpp
#include "../include/Graphics.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace graphics {
namespace {

bool finiteVector(const glm::vec3& value) {
    return std::isfinite(value.x) && std::isfinite(value.y)
        && std::isfinite(value.z);
}

bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f
        && value.y > 0.0f && value.z > 0.0f;
}

bool validColor(GlColor color) {
    return std::isfinite(color.red) && std::isfinite(color.green)
        && std::isfinite(color.blue);
}

std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;
    return ++revision;
}

void appendFace(Polyhedron& polyhedron, GLFace shape,
                std::initializer_list<glm::vec3> vertices, GlColor color) {
    PlaneFigure face;
    face.shape = shape;
    face.localVertices = vertices;
    face.color = color;
    face.normal = glm::normalize(glm::cross(
        face.localVertices[1] - face.localVertices[0],
        face.localVertices[2] - face.localVertices[0]));
    polyhedron.faces.push_back(face);
}

} // namespace

Scene::Scene() {
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
    lines_.reserve(MAX_LINES);
}

std::optional<EntityId> Scene::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (components_.size() >= MAX_ENTITIES
        || !DiagramSystem::validPolyhedron(polyhedron)
        || !DiagramSystem::validTransform(transform)) {
        return std::nullopt;
    }
    glm::vec3 minimum = polyhedron.faces[0].localVertices[0];
    glm::vec3 maximum = minimum;
    for (const PlaneFigure& face : polyhedron.faces) {
        for (const glm::vec3& vertex : face.localVertices) {
            minimum = glm::min(minimum, vertex);
            maximum = glm::max(maximum, vertex);
        }
    }
    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform, maximum - minimum });
    polyId_.push_back(id);
    std::vector<FaceId>& faceIds = polyhedron.shape == GLShape::Cube
        ? cubeFaceId_ : tetraFaceId_;
    for (std::size_t i = 0; i < polyhedron.faces.size(); ++i) {
        faceIds.push_back({ id, i });
    }
    geometryRevision_ = nextGeometryRevision();
    return id;
}

std::optional<EntityId> Scene::addCuboid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths),
                       transform);
}

std::optional<EntityId> Scene::addRightTetrahedron(
    const glm::vec3& edgeLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths),
                       transform);
}

const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return false;
    }
    components_[id].transform.basis = worldPosition;
    return true;
}

bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if (id >= components_.size() || !positiveLengths(sideLengths)) {
        return false;
    }
    Component_Poly& component = components_[id];
    Polyhedron replacement = DiagramSystem::makePolyhedron(
        component.polyhedron.shape, sideLengths);
    if (!DiagramSystem::validPolyhedron(replacement)) {
        return false;
    }
    for (std::size_t i = 0; i < replacement.faces.size(); ++i) {
        replacement.faces[i].color = component.polyhedron.faces[i].color;
        replacement.faces[i].independence = component.polyhedron.faces[i].independence;
        replacement.faces[i].transform = component.polyhedron.faces[i].transform;
    }
    component.polyhedron = std::move(replacement);
    component.sideLengths = sideLengths;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true;
}

bool Scene::setFaceTransform(
    EntityId id, std::size_t faceIndex,
    const Transform3D& transform, bool independence
) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    face.transform = transform;
    face.independence = independence;
    return true;
}

std::optional<LineId> Scene::addLine(
    const glm::vec3& start, const glm::vec3& end,
    GlColor color, std::optional<EntityId> owner
) {
    if (lines_.size() >= MAX_LINES || !finiteVector(start) || !finiteVector(end)
        || !validColor(color) || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    const LineId id = lines_.size();
    lines_.push_back({ start, end, color, owner });
    geometryRevision_ = nextGeometryRevision();
    return id;
}

std::optional<LineId> Scene::addVector(
    const glm::vec3& origin, const glm::vec3& vector,
    GlColor color, std::optional<EntityId> owner
) {
    return addLine(origin, origin + vector, color, owner);
}

std::optional<std::array<LineId, 3>> Scene::addBasis(
    const glm::vec3& origin, const glm::mat3& basisVectors,
    const glm::vec3& axisLengths, std::optional<EntityId> owner
) {
    if (lines_.size() > MAX_LINES - 3 || !finiteVector(origin)
        || !positiveLengths(axisLengths)
        || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    for (int i = 0; i < 3; ++i) {
        if (!finiteVector(basisVectors[i])
            || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f
            || !finiteVector(origin + basisVectors[i] * axisLengths[i])) {
            return std::nullopt;
        }
    }
    // 사전 검증 후 세 선을 함께 등록한다. 실패 입력은 장면을 변경하지 않는다.
    const std::array<GlColor, 3> colors{
        GlColor{ 1.0f, 0.2f, 0.2f },
        GlColor{ 0.2f, 1.0f, 0.2f },
        GlColor{ 0.2f, 0.4f, 1.0f }
    };
    std::array<LineId, 3> ids{};
    for (int i = 0; i < 3; ++i) {
        ids[i] = lines_.size();
        lines_.push_back({
            origin, origin + basisVectors[i] * axisLengths[i], colors[i], owner });
    }
    geometryRevision_ = nextGeometryRevision();
    return ids;
}

bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin)
        || !finiteVector(origin + vector)) {
        return false;
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    cubeFaceId_.clear();
    tetraFaceId_.clear();
    lines_.clear();
    geometryRevision_ = nextGeometryRevision();
}

bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale);
}

bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    const bool cube = polyhedron.shape == GLShape::Cube;
    if ((!cube && polyhedron.shape != GLShape::Tetra)
        || polyhedron.faces.size() != (cube ? 6u : 4u)) {
        return false;
    }
    for (const PlaneFigure& face : polyhedron.faces) {
        if (face.shape != (cube ? GLFace::Rect : GLFace::Triangle)
            || face.localVertices.size() != (cube ? 4u : 3u)
            || !finiteVector(face.normal) || !validColor(face.color)
            || !validTransform(face.transform)) {
            return false;
        }
        for (const glm::vec3& vertex : face.localVertices) {
            if (!finiteVector(vertex)) {
                return false;
            }
        }
    }
    return true;
}

Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
    Polyhedron polyhedron;
    if (!positiveLengths(sideLengths)) {
        return polyhedron;
    }
    polyhedron.shape = shape;
    if (shape == GLShape::Cube) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        polyhedron.faces.reserve(6);
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, z }, { x, -y, z }, { x, y, z }, { -x, y, z } },
            { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, -z }, { -x, -y, -z }, { -x, y, -z }, { x, y, -z } },
            { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { -x, -y, z }, { -x, y, z }, { -x, y, -z } },
            { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, z }, { x, -y, -z }, { x, y, -z }, { x, y, z } },
            { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, y, z }, { x, y, z }, { x, y, -z }, { -x, y, -z } },
            { .3f, .8f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { x, -y, -z }, { x, -y, z }, { -x, -y, z } },
            { .8f, .3f, .9f });
    } else if (shape == GLShape::Tetra) {
        // 원점에서 직교하는 세 모서리를 만들고, 무게중심을 로컬 원점으로 옮긴다.
        const glm::vec3 center = sideLengths * 0.25f;
        const glm::vec3 origin = -center;
        const glm::vec3 x = glm::vec3(sideLengths.x, 0.0f, 0.0f) - center;
        const glm::vec3 y = glm::vec3(0.0f, sideLengths.y, 0.0f) - center;
        const glm::vec3 z = glm::vec3(0.0f, 0.0f, sideLengths.z) - center;
        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle,
            { origin, y, x }, { .9f, .4f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, x, z }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, z, y }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { x, y, z }, { .9f, .8f, .3f });
    }
    return polyhedron;
}

glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    // 열벡터 기준: localBasis 이동 -> S -> Rz -> Ry -> Rx -> 월드 위치.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.basis);
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, transform.scale);
    return glm::translate(model, -transform.localBasis);
}

glm::mat4 DiagramSystem::faceModelMatrix(
    const Component_Poly& component, const PlaneFigure& face
) {
    return modelMatrix(face.independence ? face.transform : component.transform);
}

bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);
    return finiteVector(camera.position) && finiteVector(camera.target)
        && finiteVector(camera.up) && finiteVector(direction) && finiteVector(right)
        && glm::dot(direction, direction) > 0.000001f
        && glm::dot(right, right) > 0.000001f
        && std::isfinite(camera.fieldOfViewDegrees)
        && camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f
        && std::isfinite(camera.nearPlane) && std::isfinite(camera.farPlane)
        && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane;
}

glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect,
                            camera.nearPlane, camera.farPlane);
}

namespace {

GLuint compileShader(GLenum type, const std::filesystem::path& path) {
    const std::string sourceText = readTextFile(path);
    if (sourceText.empty()) {
        return 0;
    }
    const char* source = sourceText.c_str();
    const GLuint shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), nullptr, log.data());
        std::cerr << "Shader compile failed: " << path << '\n' << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

} // namespace

std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open shader file: " << path << '\n';
        return {};
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();
    if (text.size() >= 3 && static_cast<unsigned char>(text[0]) == 0xEF
        && static_cast<unsigned char>(text[1]) == 0xBB
        && static_cast<unsigned char>(text[2]) == 0xBF) {
        text.erase(0, 3); // 저장 파일은 BOM을 유지하고 GLSL 입력에서만 제거한다.
    }
    return text;
}

bool Renderer::initShader(const std::filesystem::path& vertexPath,
                          const std::filesystem::path& fragmentPath) {
    destroy();
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
    if (vertexShader == 0) {
        return false;
    }
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    shaderProgramID_ = glCreateProgram();
    if (shaderProgramID_ == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    glAttachShader(shaderProgramID_, vertexShader);
    glAttachShader(shaderProgramID_, fragmentShader);
    glLinkProgram(shaderProgramID_);

    GLint linked = GL_FALSE;
    glGetProgramiv(shaderProgramID_, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(shaderProgramID_, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetProgramInfoLog(shaderProgramID_, static_cast<GLsizei>(log.size()),
                            nullptr, log.data());
        std::cerr << "Shader link failed:\n" << log << '\n';
    }
    glDetachShader(shaderProgramID_, vertexShader);
    glDetachShader(shaderProgramID_, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (linked != GL_TRUE) {
        destroy();
        return false;
    }

    mvpLocation_ = glGetUniformLocation(shaderProgramID_, "uMVP");
    if (mvpLocation_ < 0) {
        std::cerr << "Shader uniform uMVP is missing.\n";
        destroy();
        return false;
    }
    return true;
}

bool Renderer::initBuffer() {
    if (shaderProgramID_ == 0 || vao_ != 0) {
        return false;
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(2, vbo_);
    glGenBuffers(1, &ebo_);
    if (vao_ == 0 || vbo_[0] == 0 || vbo_[1] == 0 || ebo_ == 0) {
        destroy();
        return false;
    }

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBindVertexArray(0); // EBO 연결은 VAO의 상태로 유지한다.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool Renderer::upload(const Scene& scene) {
    if (vao_ == 0) {
        return false;
    }
    std::vector<GLfloat> positions;
    std::vector<GLfloat> colors;
    std::vector<GLuint> indices;
    std::vector<DrawRange> ranges;
    for (EntityId id : scene.getIds()) {
        const Component_Poly* component = scene.getComponent(id);
        if (component == nullptr
            || !DiagramSystem::validPolyhedron(component->polyhedron)) {
            return false;
        }
        const auto& faces = component->polyhedron.faces;
        for (std::size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex) {
            const PlaneFigure& face = faces[faceIndex];
            const GLuint firstVertex = static_cast<GLuint>(positions.size() / 3);
            const std::size_t firstIndex = indices.size();
            for (const glm::vec3& vertex : face.localVertices) {
                positions.insert(positions.end(), { vertex.x, vertex.y, vertex.z });
                colors.insert(colors.end(),
                    { face.color.red, face.color.green, face.color.blue });
            }
            indices.insert(indices.end(),
                { firstVertex, firstVertex + 1, firstVertex + 2 });
            if (face.shape == GLFace::Rect) {
                indices.insert(indices.end(),
                    { firstVertex, firstVertex + 2, firstVertex + 3 });
            }
            ranges.push_back({ GL_TRIANGLES, id, faceIndex, std::nullopt,
                firstIndex, static_cast<GLsizei>(indices.size() - firstIndex) });
        }
    }
    const auto& lines = scene.getLines();
    for (LineId id = 0; id < lines.size(); ++id) {
        const Line3D& line = lines[id];
        const GLuint firstVertex = static_cast<GLuint>(positions.size() / 3);
        const std::size_t firstIndex = indices.size();
        positions.insert(positions.end(),
            { line.start.x, line.start.y, line.start.z,
              line.end.x, line.end.y, line.end.z });
        for (int i = 0; i < 2; ++i) {
            colors.insert(colors.end(),
                { line.color.red, line.color.green, line.color.blue });
        }
        indices.insert(indices.end(), { firstVertex, firstVertex + 1 });
        ranges.push_back({ GL_LINES, 0, 0, id, firstIndex, 2 });
    }
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(positions.size() * sizeof(GLfloat)),
        positions.empty() ? nullptr : positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(colors.size() * sizeof(GLfloat)),
        colors.empty() ? nullptr : colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)),
        indices.empty() ? nullptr : indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    drawRanges_ = std::move(ranges);
    return true;
}

bool Renderer::draw(const Scene& scene, const glm::mat4& view,
                    const glm::mat4& projection) const {
    if (shaderProgramID_ == 0 || vao_ == 0 || mvpLocation_ < 0) {
        return false;
    }
    // 사용되는 ID를 먼저 검사한다. 잘못된 데이터로 일부만 그리지 않는다.
    for (const DrawRange& range : drawRanges_) {
        if (range.lineId) {
            if (*range.lineId >= scene.getLines().size()) {
                return false;
            }
            const auto owner = scene.getLines()[*range.lineId].owner;
            if (owner && scene.getComponent(*owner) == nullptr) {
                return false;
            }
        } else {
            const Component_Poly* component = scene.getComponent(range.entityId);
            if (component == nullptr
                || range.faceIndex >= component->polyhedron.faces.size()) {
                return false;
            }
        }
    }
    glUseProgram(shaderProgramID_);
    glBindVertexArray(vao_);
    for (const DrawRange& range : drawRanges_) {
        glm::mat4 model(1.0f);
        if (range.lineId) {
            const auto owner = scene.getLines()[*range.lineId].owner;
            if (owner) {
                model = DiagramSystem::modelMatrix(scene.getComponent(*owner)->transform);
            }
        } else {
            const Component_Poly* component = scene.getComponent(range.entityId);
            model = DiagramSystem::faceModelMatrix(
                *component, component->polyhedron.faces[range.faceIndex]);
        }
        const glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(mvp));
        const void* offset = reinterpret_cast<const void*>(
            range.firstIndex * sizeof(GLuint));
        glDrawElements(range.mode, range.count, GL_UNSIGNED_INT, offset);
    }
    glBindVertexArray(0);
    glUseProgram(0);
    return true;
}

bool Renderer::initialize(const std::filesystem::path& vertexPath,
                          const std::filesystem::path& fragmentPath) {
    if (!initShader(vertexPath, fragmentPath) || !initBuffer()) {
        destroy();
        return false;
    }
    return true;
}

bool Renderer::render(const Scene& scene, const Camera& camera, int width, int height) {
    if (shaderProgramID_ == 0 || vao_ == 0) {
        return false;
    }
    if (width <= 0 || height <= 0) {
        return true; // 최소화 중에는 투영과 드로우를 건너뛴다.
    }
    if (!CameraSystem::validCamera(camera)) {
        return false;
    }
    if (uploadedScene_ != &scene || uploadedRevision_ != scene.getGeometryRevision()) {
        if (!upload(scene)) {
            return false;
        }
        uploadedScene_ = &scene;
        uploadedRevision_ = scene.getGeometryRevision();
    }
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glClearColor(0.12f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    return draw(scene, CameraSystem::viewMatrix(camera),
                CameraSystem::projectionMatrix(camera, aspect));
}

void Renderer::destroy() {
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    if (vbo_[0] != 0 || vbo_[1] != 0) {
        glDeleteBuffers(2, vbo_);
        vbo_[0] = vbo_[1] = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (shaderProgramID_ != 0) {
        glDeleteProgram(shaderProgramID_);
        shaderProgramID_ = 0;
    }
    mvpLocation_ = -1;
    drawRanges_.clear();
    uploadedScene_ = nullptr;
    uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
}

} // namespace graphics
````

### openGL_3dAssign_14/framework/src/Graphics.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/framework/src/Graphics.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### framework/src/Graphics.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#include "../include/Graphics.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace graphics {
namespace {

bool finiteVector(const glm::vec3& value) {
    return std::isfinite(value.x) && std::isfinite(value.y)
        && std::isfinite(value.z);
}

bool finiteMatrix(const glm::mat4& matrix) {
    for (int column = 0; column < 4; ++column) {
        for (int row = 0; row < 4; ++row) {
            if (!std::isfinite(matrix[column][row])) {
                return false;
            }
        }
    }
    return true;
}

bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f
        && value.y > 0.0f && value.z > 0.0f;
}

bool validColor(GlColor color) {
    return std::isfinite(color.red) && std::isfinite(color.green)
        && std::isfinite(color.blue);
}

std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;
    return ++revision;
}

void appendFace(Polyhedron& polyhedron, GLFace shape,
                std::initializer_list<glm::vec3> vertices, GlColor color) {
    PlaneFigure face;
    face.shape = shape;
    face.localVertices = vertices;
    face.color = color;
    face.normal = glm::normalize(glm::cross(
        face.localVertices[1] - face.localVertices[0],
        face.localVertices[2] - face.localVertices[0]));
    polyhedron.faces.push_back(face);
}

} // namespace

Scene::Scene() {
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
    pyramidFaceId_.reserve(MAX_ENTITIES * 5);
    lines_.reserve(MAX_LINES);
}

std::optional<EntityId> Scene::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (components_.size() >= MAX_ENTITIES
        || !DiagramSystem::validPolyhedron(polyhedron)
        || !DiagramSystem::validTransform(transform)) {
        return std::nullopt;
    }
    glm::vec3 minimum = polyhedron.faces[0].localVertices[0];
    glm::vec3 maximum = minimum;
    for (const PlaneFigure& face : polyhedron.faces) {
        for (const glm::vec3& vertex : face.localVertices) {
            minimum = glm::min(minimum, vertex);
            maximum = glm::max(maximum, vertex);
        }
    }
    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform, maximum - minimum });
    polyId_.push_back(id);
    std::vector<FaceId>& faceIds = polyhedron.shape == GLShape::Cube
        ? cubeFaceId_ : (polyhedron.shape == GLShape::Pyramid
            ? pyramidFaceId_ : tetraFaceId_);
    for (std::size_t i = 0; i < polyhedron.faces.size(); ++i) {
        faceIds.push_back({ id, i });
    }
    geometryRevision_ = nextGeometryRevision();
    return id;
}

std::optional<EntityId> Scene::addCuboid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths),
                       transform);
}

std::optional<EntityId> Scene::addRightTetrahedron(
    const glm::vec3& edgeLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths),
                       transform);
}

std::optional<EntityId> Scene::addPyramid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths),
                       transform);
}

bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;
    return true;
}

bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex,
                               const glm::mat4& matrix) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !finiteMatrix(matrix)) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    return true;
}

bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex,
                                const std::vector<GlColor>& colors) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    if (!colors.empty() && colors.size() != face.localVertices.size()) {
        return false;
    }
    for (GlColor color : colors) {
        if (!validColor(color)) {
            return false;
        }
    }
    face.vertexColors = colors;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return false;
    }
    components_[id].transform.basis = worldPosition;
    return true;
}

bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if (id >= components_.size() || !positiveLengths(sideLengths)) {
        return false;
    }
    Component_Poly& component = components_[id];
    Polyhedron replacement = DiagramSystem::makePolyhedron(
        component.polyhedron.shape, sideLengths);
    if (!DiagramSystem::validPolyhedron(replacement)) {
        return false;
    }
    for (std::size_t i = 0; i < replacement.faces.size(); ++i) {
        replacement.faces[i].color = component.polyhedron.faces[i].color;
        replacement.faces[i].independence = component.polyhedron.faces[i].independence;
        replacement.faces[i].transform = component.polyhedron.faces[i].transform;
        replacement.faces[i].visible = component.polyhedron.faces[i].visible;
        replacement.faces[i].localAnimationMatrix =
            component.polyhedron.faces[i].localAnimationMatrix;
        replacement.faces[i].vertexColors = component.polyhedron.faces[i].vertexColors;
    }
    component.polyhedron = std::move(replacement);
    component.sideLengths = sideLengths;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true;
}

bool Scene::setFaceTransform(
    EntityId id, std::size_t faceIndex,
    const Transform3D& transform, bool independence
) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];
    face.transform = transform;
    face.independence = independence;
    return true;
}

std::optional<LineId> Scene::addLine(
    const glm::vec3& start, const glm::vec3& end,
    GlColor color, std::optional<EntityId> owner
) {
    if (lines_.size() >= MAX_LINES || !finiteVector(start) || !finiteVector(end)
        || !validColor(color) || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    const LineId id = lines_.size();
    lines_.push_back({ start, end, color, owner });
    geometryRevision_ = nextGeometryRevision();
    return id;
}

std::optional<LineId> Scene::addVector(
    const glm::vec3& origin, const glm::vec3& vector,
    GlColor color, std::optional<EntityId> owner
) {
    return addLine(origin, origin + vector, color, owner);
}

std::optional<std::array<LineId, 3>> Scene::addBasis(
    const glm::vec3& origin, const glm::mat3& basisVectors,
    const glm::vec3& axisLengths, std::optional<EntityId> owner
) {
    if (lines_.size() > MAX_LINES - 3 || !finiteVector(origin)
        || !positiveLengths(axisLengths)
        || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    for (int i = 0; i < 3; ++i) {
        if (!finiteVector(basisVectors[i])
            || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f
            || !finiteVector(origin + basisVectors[i] * axisLengths[i])) {
            return std::nullopt;
        }
    }
    // 사전 검증 후 세 선을 함께 등록한다. 실패 입력은 장면을 변경하지 않는다.
    const std::array<GlColor, 3> colors{
        GlColor{ 1.0f, 0.2f, 0.2f },
        GlColor{ 0.2f, 1.0f, 0.2f },
        GlColor{ 0.2f, 0.4f, 1.0f }
    };
    std::array<LineId, 3> ids{};
    for (int i = 0; i < 3; ++i) {
        ids[i] = lines_.size();
        lines_.push_back({
            origin, origin + basisVectors[i] * axisLengths[i], colors[i], owner });
    }
    geometryRevision_ = nextGeometryRevision();
    return ids;
}

bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin)
        || !finiteVector(origin + vector)) {
        return false;
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

void Scene::clear() {
    if (components_.empty() && lines_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    cubeFaceId_.clear();
    tetraFaceId_.clear();
    pyramidFaceId_.clear();
    lines_.clear();
    geometryRevision_ = nextGeometryRevision();
}

bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale)
        && std::isfinite(transform.orientation.w)
        && std::isfinite(transform.orientation.x)
        && std::isfinite(transform.orientation.y)
        && std::isfinite(transform.orientation.z)
        && std::isfinite(glm::dot(transform.orientation, transform.orientation))
        && glm::dot(transform.orientation, transform.orientation) > 0.000001f;
}

bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    std::size_t expectedFaces = 0;
    switch (polyhedron.shape) {
    case GLShape::Cube: expectedFaces = 6; break;
    case GLShape::Tetra: expectedFaces = 4; break;
    case GLShape::Pyramid: expectedFaces = 5; break;
    default: return false;
    }
    if (polyhedron.faces.size() != expectedFaces) {
        return false;
    }
    for (std::size_t i = 0; i < expectedFaces; ++i) {
        const PlaneFigure& face = polyhedron.faces[i];
        const bool rectangle = polyhedron.shape == GLShape::Cube
            || (polyhedron.shape == GLShape::Pyramid && i == 4);
        if (face.shape != (rectangle ? GLFace::Rect : GLFace::Triangle)
            || face.localVertices.size() != (rectangle ? 4u : 3u)
            || !finiteVector(face.normal) || !validColor(face.color)
            || !validTransform(face.transform) || !finiteMatrix(face.localAnimationMatrix)
            || (!face.vertexColors.empty()
                && face.vertexColors.size() != face.localVertices.size())) {
            return false;
        }
        for (const glm::vec3& vertex : face.localVertices) {
            if (!finiteVector(vertex)) {
                return false;
            }
        }
        for (GlColor color : face.vertexColors) {
            if (!validColor(color)) {
                return false;
            }
        }
    }
    return true;
}

Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
    Polyhedron polyhedron;
    if (!positiveLengths(sideLengths)) {
        return polyhedron;
    }
    polyhedron.shape = shape;
    if (shape == GLShape::Cube) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        polyhedron.faces.reserve(6);
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, z }, { x, -y, z }, { x, y, z }, { -x, y, z } },
            { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, -z }, { -x, -y, -z }, { -x, y, -z }, { x, y, -z } },
            { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { -x, -y, z }, { -x, y, z }, { -x, y, -z } },
            { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { x, -y, z }, { x, -y, -z }, { x, y, -z }, { x, y, z } },
            { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, y, z }, { x, y, z }, { x, y, -z }, { -x, y, -z } },
            { .3f, .8f, .9f });
        appendFace(polyhedron, GLFace::Rect,
            { { -x, -y, -z }, { x, -y, -z }, { x, -y, z }, { -x, -y, z } },
            { .8f, .3f, .9f });
    } else if (shape == GLShape::Pyramid) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        const glm::vec3 frontLeft{ -x, -y, z };
        const glm::vec3 frontRight{ x, -y, z };
        const glm::vec3 backRight{ x, -y, -z };
        const glm::vec3 backLeft{ -x, -y, -z };
        const glm::vec3 apex{ 0.0f, y, 0.0f };
        polyhedron.faces.reserve(5);
        // 앞, 오른쪽, 뒤, 왼쪽, 바닥. 삼각형과 사각형은 바깥쪽 winding이다.
        appendFace(polyhedron, GLFace::Triangle,
            { frontLeft, frontRight, apex }, { .9f, .3f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { frontRight, backRight, apex }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { backRight, backLeft, apex }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { backLeft, frontLeft, apex }, { .9f, .8f, .3f });
        appendFace(polyhedron, GLFace::Rect,
            { backLeft, backRight, frontRight, frontLeft }, { .8f, .3f, .9f });
    } else if (shape == GLShape::Tetra) {
        // 원점에서 직교하는 세 모서리를 만들고, 무게중심을 로컬 원점으로 옮긴다.
        const glm::vec3 center = sideLengths * 0.25f;
        const glm::vec3 origin = -center;
        const glm::vec3 x = glm::vec3(sideLengths.x, 0.0f, 0.0f) - center;
        const glm::vec3 y = glm::vec3(0.0f, sideLengths.y, 0.0f) - center;
        const glm::vec3 z = glm::vec3(0.0f, 0.0f, sideLengths.z) - center;
        polyhedron.faces.reserve(4);
        appendFace(polyhedron, GLFace::Triangle,
            { origin, y, x }, { .9f, .4f, .3f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, x, z }, { .3f, .8f, .4f });
        appendFace(polyhedron, GLFace::Triangle,
            { origin, z, y }, { .3f, .5f, .9f });
        appendFace(polyhedron, GLFace::Triangle,
            { x, y, z }, { .9f, .8f, .3f });
    }
    return polyhedron;
}

glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    // 열벡터 기준: localBasis 이동 -> S -> Rz -> Ry -> Rx -> 월드 위치.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), transform.basis);
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.x),
                        glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.y),
                        glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(transform.rotationDegrees.z),
                        glm::vec3(0.0f, 0.0f, 1.0f));
    model *= glm::mat4_cast(glm::normalize(transform.orientation));
    model = glm::scale(model, transform.scale);
    return glm::translate(model, -transform.localBasis);
}

glm::mat4 DiagramSystem::faceModelMatrix(
    const Component_Poly& component, const PlaneFigure& face
) {
    // 기존 독립 면은 월드 transform만 사용한다. 일반 면은 객체를 따라간다.
    return face.independence ? modelMatrix(face.transform)
        : modelMatrix(component.transform) * face.localAnimationMatrix;
}

bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);
    return finiteVector(camera.position) && finiteVector(camera.target)
        && finiteVector(camera.up) && finiteVector(direction) && finiteVector(right)
        && glm::dot(direction, direction) > 0.000001f
        && glm::dot(right, right) > 0.000001f
        && std::isfinite(camera.fieldOfViewDegrees)
        && camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f
        && std::isfinite(camera.nearPlane) && std::isfinite(camera.farPlane)
        && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane
        && std::isfinite(camera.orthographicHalfExtent)
        && camera.orthographicHalfExtent > 0.0f;
}

glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                          camera.nearPlane, camera.farPlane);
    }
    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect,
                            camera.nearPlane, camera.farPlane);
}

namespace {

GLuint compileShader(GLenum type, const std::filesystem::path& path) {
    const std::string sourceText = readTextFile(path);
    if (sourceText.empty()) {
        return 0;
    }
    const char* source = sourceText.c_str();
    const GLuint shader = glCreateShader(type);
    if (shader == 0) {
        return 0;
    }
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint compiled = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
    if (compiled != GL_TRUE) {
        GLint length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetShaderInfoLog(shader, static_cast<GLsizei>(log.size()), nullptr, log.data());
        std::cerr << "Shader compile failed: " << path << '\n' << log << '\n';
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

} // namespace

std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Cannot open shader file: " << path << '\n';
        return {};
    }
    std::ostringstream buffer;
    buffer << file.rdbuf();
    std::string text = buffer.str();
    if (text.size() >= 3 && static_cast<unsigned char>(text[0]) == 0xEF
        && static_cast<unsigned char>(text[1]) == 0xBB
        && static_cast<unsigned char>(text[2]) == 0xBF) {
        text.erase(0, 3); // 저장 파일은 BOM을 유지하고 GLSL 입력에서만 제거한다.
    }
    return text;
}

bool Renderer::initShader(const std::filesystem::path& vertexPath,
                          const std::filesystem::path& fragmentPath) {
    destroy();
    const GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vertexPath);
    if (vertexShader == 0) {
        return false;
    }
    const GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentPath);
    if (fragmentShader == 0) {
        glDeleteShader(vertexShader);
        return false;
    }

    shaderProgramID_ = glCreateProgram();
    if (shaderProgramID_ == 0) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }
    glAttachShader(shaderProgramID_, vertexShader);
    glAttachShader(shaderProgramID_, fragmentShader);
    glLinkProgram(shaderProgramID_);

    GLint linked = GL_FALSE;
    glGetProgramiv(shaderProgramID_, GL_LINK_STATUS, &linked);
    if (linked != GL_TRUE) {
        GLint length = 0;
        glGetProgramiv(shaderProgramID_, GL_INFO_LOG_LENGTH, &length);
        std::string log(length > 0 ? length : 1, '\0');
        glGetProgramInfoLog(shaderProgramID_, static_cast<GLsizei>(log.size()),
                            nullptr, log.data());
        std::cerr << "Shader link failed:\n" << log << '\n';
    }
    glDetachShader(shaderProgramID_, vertexShader);
    glDetachShader(shaderProgramID_, fragmentShader);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    if (linked != GL_TRUE) {
        destroy();
        return false;
    }

    mvpLocation_ = glGetUniformLocation(shaderProgramID_, "uMVP");
    if (mvpLocation_ < 0) {
        std::cerr << "Shader uniform uMVP is missing.\n";
        destroy();
        return false;
    }
    return true;
}

bool Renderer::initBuffer() {
    if (shaderProgramID_ == 0 || vao_ != 0) {
        return false;
    }

    glGenVertexArrays(1, &vao_);
    glGenBuffers(2, vbo_);
    glGenBuffers(1, &ebo_);
    if (vao_ == 0 || vbo_[0] == 0 || vbo_[1] == 0 || ebo_ == 0) {
        destroy();
        return false;
    }

    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBindVertexArray(0); // EBO 연결은 VAO의 상태로 유지한다.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

bool Renderer::upload(const Scene& scene) {
    if (vao_ == 0) {
        return false;
    }
    std::vector<GLfloat> positions;
    std::vector<GLfloat> colors;
    std::vector<GLuint> indices;
    std::vector<DrawRange> ranges;
    for (EntityId id : scene.getIds()) {
        const Component_Poly* component = scene.getComponent(id);
        if (component == nullptr
            || !DiagramSystem::validPolyhedron(component->polyhedron)) {
            return false;
        }
        const auto& faces = component->polyhedron.faces;
        for (std::size_t faceIndex = 0; faceIndex < faces.size(); ++faceIndex) {
            const PlaneFigure& face = faces[faceIndex];
            const GLuint firstVertex = static_cast<GLuint>(positions.size() / 3);
            const std::size_t firstIndex = indices.size();
            for (std::size_t i = 0; i < face.localVertices.size(); ++i) {
                const glm::vec3& vertex = face.localVertices[i];
                const GlColor color = face.vertexColors.empty() ? face.color : face.vertexColors[i];
                positions.insert(positions.end(), { vertex.x, vertex.y, vertex.z });
                colors.insert(colors.end(), { color.red, color.green, color.blue });
            }
            indices.insert(indices.end(),
                { firstVertex, firstVertex + 1, firstVertex + 2 });
            if (face.shape == GLFace::Rect) {
                indices.insert(indices.end(),
                    { firstVertex, firstVertex + 2, firstVertex + 3 });
            }
            const GLsizei triangleCount = static_cast<GLsizei>(indices.size() - firstIndex);
            const std::size_t outlineFirstIndex = indices.size();
            for (std::size_t i = 0; i < face.localVertices.size(); ++i) {
                indices.push_back(firstVertex + static_cast<GLuint>(i));
            }
            ranges.push_back({ GL_TRIANGLES, id, faceIndex, std::nullopt,
                firstIndex, triangleCount, outlineFirstIndex,
                static_cast<GLsizei>(face.localVertices.size()) });
        }
    }
    const auto& lines = scene.getLines();
    for (LineId id = 0; id < lines.size(); ++id) {
        const Line3D& line = lines[id];
        const GLuint firstVertex = static_cast<GLuint>(positions.size() / 3);
        const std::size_t firstIndex = indices.size();
        positions.insert(positions.end(),
            { line.start.x, line.start.y, line.start.z,
              line.end.x, line.end.y, line.end.z });
        for (int i = 0; i < 2; ++i) {
            colors.insert(colors.end(),
                { line.color.red, line.color.green, line.color.blue });
        }
        indices.insert(indices.end(), { firstVertex, firstVertex + 1 });
        ranges.push_back({ GL_LINES, 0, 0, id, firstIndex, 2 });
    }
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[0]);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(positions.size() * sizeof(GLfloat)),
        positions.empty() ? nullptr : positions.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_[1]);
    glBufferData(GL_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(colors.size() * sizeof(GLfloat)),
        colors.empty() ? nullptr : colors.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        static_cast<GLsizeiptr>(indices.size() * sizeof(GLuint)),
        indices.empty() ? nullptr : indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    drawRanges_ = std::move(ranges);
    return true;
}

bool Renderer::draw(const Scene& scene, const glm::mat4& view,
                    const glm::mat4& projection, const RenderOptions& options) const {
    if (shaderProgramID_ == 0 || vao_ == 0 || mvpLocation_ < 0) {
        return false;
    }
    // 사용되는 ID를 먼저 검사한다. 잘못된 데이터로 일부만 그리지 않는다.
    for (const DrawRange& range : drawRanges_) {
        if (range.lineId) {
            if (*range.lineId >= scene.getLines().size()) {
                return false;
            }
            const auto owner = scene.getLines()[*range.lineId].owner;
            if (owner && scene.getComponent(*owner) == nullptr) {
                return false;
            }
        } else {
            const Component_Poly* component = scene.getComponent(range.entityId);
            if (component == nullptr
                || range.faceIndex >= component->polyhedron.faces.size()) {
                return false;
            }
        }
    }
    glUseProgram(shaderProgramID_);
    glBindVertexArray(vao_);
    for (const DrawRange& range : drawRanges_) {
        glm::mat4 model(1.0f);
        if (range.lineId) {
            const auto owner = scene.getLines()[*range.lineId].owner;
            if (owner) {
                model = DiagramSystem::modelMatrix(scene.getComponent(*owner)->transform);
            }
        } else {
            const Component_Poly* component = scene.getComponent(range.entityId);
            const PlaneFigure& face = component->polyhedron.faces[range.faceIndex];
            if (!face.visible) {
                continue;
            }
            model = DiagramSystem::faceModelMatrix(*component, face);
        }
        const glm::mat4 mvp = projection * view * model;
        glUniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(mvp));
        const bool outline = options.wireframe && !range.lineId.has_value();
        const std::size_t firstIndex = outline ? range.outlineFirstIndex : range.firstIndex;
        const void* offset = reinterpret_cast<const void*>(firstIndex * sizeof(GLuint));
        glDrawElements(outline ? GL_LINE_LOOP : range.mode,
                       outline ? range.outlineCount : range.count, GL_UNSIGNED_INT, offset);
    }
    glBindVertexArray(0);
    glUseProgram(0);
    return true;
}

bool Renderer::initialize(const std::filesystem::path& vertexPath,
                          const std::filesystem::path& fragmentPath) {
    if (!initShader(vertexPath, fragmentPath) || !initBuffer()) {
        destroy();
        return false;
    }
    return true;
}

bool Renderer::render(const Scene& scene, const Camera& camera, int width, int height,
                      const RenderOptions& options) {
    if (shaderProgramID_ == 0 || vao_ == 0) {
        return false;
    }
    if (width <= 0 || height <= 0) {
        return true; // 최소화 중에는 투영과 드로우를 건너뛴다.
    }
    if (!CameraSystem::validCamera(camera)) {
        return false;
    }
    if (uploadedScene_ != &scene || uploadedRevision_ != scene.getGeometryRevision()) {
        if (!upload(scene)) {
            return false;
        }
        uploadedScene_ = &scene;
        uploadedRevision_ = scene.getGeometryRevision();
    }
    glViewport(0, 0, width, height);
    if (options.depthTest) {
        glEnable(GL_DEPTH_TEST);
    } else {
        glDisable(GL_DEPTH_TEST);
    }
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LESS);
    glClearColor(0.12f, 0.12f, 0.16f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    const float aspect = static_cast<float>(width) / static_cast<float>(height);
    return draw(scene, CameraSystem::viewMatrix(camera),
                CameraSystem::projectionMatrix(camera, aspect), options);
}

void Renderer::destroy() {
    if (ebo_ != 0) {
        glDeleteBuffers(1, &ebo_);
        ebo_ = 0;
    }
    if (vbo_[0] != 0 || vbo_[1] != 0) {
        glDeleteBuffers(2, vbo_);
        vbo_[0] = vbo_[1] = 0;
    }
    if (vao_ != 0) {
        glDeleteVertexArrays(1, &vao_);
        vao_ = 0;
    }
    if (shaderProgramID_ != 0) {
        glDeleteProgram(shaderProgramID_);
        shaderProgramID_ = 0;
    }
    mvpLocation_ = -1;
    drawRanges_.clear();
    uploadedScene_ = nullptr;
    uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
}

} // namespace graphics
````

## 변경 묶음 13: Common.h

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `60e58979f632f0ddd1d978516bd1fa121389a3941b9094c48ec62a313e146e6e`

### openGL_3dAssign_14/Common.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/Common.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#pragma once

// 기존 과제 코드의 include와 이름을 유지하는 호환 헤더.
// 새 코드는 Graphics.h와 graphics 네임스페이스를 사용한다.
#include "Graphics.h"

using graphics::WINDOW_WIDTH;
using graphics::WINDOW_HEIGHT;
using graphics::MAX_ENTITIES;
using graphics::EntityId;
using graphics::GlColor;
using graphics::GLShape;
using graphics::GLFace;
using graphics::Transform3D;
using graphics::PlaneFigure;
using graphics::Polyhedron;
using graphics::Component_Poly;
using graphics::FaceId;
using graphics::DiagramSystem;
using graphics::Camera;
using graphics::CameraSystem;
using graphics::readTextFile;
using Entity = graphics::Scene;
using ShaderHandler = graphics::Renderer;
````

## 변경 묶음 14: Console.cpp

변경 이유: 과제별 입력·상태·변환을 구현한다.

최종 파일 SHA-256: `1bef94b5c3c637362d75c8a0984dcea77554278b3adbf7f8bcb115de5a2ba0dc`

### openGL_3dAssign_14/Console.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#include "Console.h"
#include <glm/gtc/quaternion.hpp>
#include <iostream>

namespace {

// 부호 조합은 육면체 8개 꼭짓점을 구분한다. 사각뿔은 바닥 4개와 꼭짓점 색이 다르다.
void assignVertexColors(graphics::Polyhedron& shape) {
    const std::array<graphics::GlColor, 8> palette{
        graphics::GlColor{ 1.0f, 0.2f, 0.2f }, { 0.2f, 1.0f, 0.2f },
        { 0.2f, 0.4f, 1.0f }, { 1.0f, 1.0f, 0.2f },
        { 1.0f, 0.2f, 1.0f }, { 0.2f, 1.0f, 1.0f },
        { 1.0f, 0.6f, 0.2f }, { 0.7f, 0.3f, 1.0f }
    };
    for (auto& face : shape.faces) {
        for (const auto& vertex : face.localVertices) {
            const int index = (vertex.x > 0.0f ? 1 : 0)
                | (vertex.y > 0.0f ? 2 : 0) | (vertex.z > 0.0f ? 4 : 0);
            face.vertexColors.push_back(palette[index]);
        }
    }
}

} // namespace

bool Console::initScene() {
    auto& scene = application_.getScene();
    scene.clear();
    auto cube = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube, glm::vec3(0.8f));
    auto pyramid = graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Pyramid, glm::vec3(0.8f));
    assignVertexColors(cube);
    assignVertexColors(pyramid);
    const auto cubeId = scene.push_entity(cube);
    const auto pyramidId = scene.push_entity(pyramid);
    if (!cubeId || !pyramidId) {
        return false;
    }
    cuboidId_ = *cubeId;
    pyramidId_ = *pyramidId;
    if (!scene.addLine({ -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.2f, 0.2f })
        || !scene.addLine({ 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.2f, 1.0f, 0.2f })
        || !scene.addLine({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.2f, 0.4f, 1.0f })) {
        return false;
    }
    auto& camera = application_.getCamera();
    camera.position = { 1.8f, 1.2f, 3.0f };
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetMotion();
    selectShape(false);
    std::cout << "Practice 14: C cube, P pyramid, H depth test\n"
              << "w wireframe / W solid (Shift or Caps Lock)\n"
              << "x/X: local X positive/negative, y/Y: local Y positive/negative\n"
              << "Arrows: world X/Y translation, S: reset motion, Esc: quit\n";
    return true;
}

void Console::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;
    auto& scene = application_.getScene();
    for (auto id : scene.getIds()) {
        const bool visible = id == (pyramidSelected_ ? pyramidId_ : cuboidId_);
        for (std::size_t i = 0; i < scene.getComponent(id)->polyhedron.faces.size(); ++i) {
            scene.setFaceVisible(id, i, visible);
        }
    }
    applyTransform();
}

void Console::resetMotion() {
    position_ = glm::vec3(0.0f);
    orientation_ = glm::angleAxis(glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f))
        * glm::angleAxis(glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    angularDirection_ = glm::vec2(0.0f);
    pressed_.fill(false);
    applyTransform();
}

void Console::applyTransform() {
    graphics::Transform3D transform;
    transform.basis = position_;
    transform.orientation = orientation_;
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void Console::onKey(int key, int action, int mods) {
    if (key >= 0 && key <= GLFW_KEY_LAST) {
        if (action == GLFW_PRESS) {
            pressed_[key] = true;
        } else if (action == GLFW_RELEASE) {
            pressed_[key] = false;
        }
    }
    if (action != GLFW_PRESS) {
        return;
    }
    const bool uppercase = ((mods & GLFW_MOD_SHIFT) != 0) != ((mods & GLFW_MOD_CAPS_LOCK) != 0);
    auto& options = application_.getRenderOptions();
    switch (key) {
    case GLFW_KEY_ESCAPE: application_.requestClose(); break;
    case GLFW_KEY_C: selectShape(false); break;
    case GLFW_KEY_P: selectShape(true); break;
    case GLFW_KEY_H: options.depthTest = !options.depthTest; break;
    case GLFW_KEY_W: options.wireframe = !uppercase; break;
    case GLFW_KEY_X: angularDirection_.x = uppercase ? -1.0f : 1.0f; break;
    case GLFW_KEY_Y: angularDirection_.y = uppercase ? -1.0f : 1.0f; break;
    case GLFW_KEY_S: resetMotion(); break;
    default: break;
    }
}

void Console::onUpdate(float deltaTime) {
    const glm::vec2 movement{
        static_cast<float>(pressed_[GLFW_KEY_RIGHT]) - static_cast<float>(pressed_[GLFW_KEY_LEFT]),
        static_cast<float>(pressed_[GLFW_KEY_UP]) - static_cast<float>(pressed_[GLFW_KEY_DOWN])
    };
    if (glm::dot(movement, movement) > 0.0f) {
        position_ += glm::vec3(glm::normalize(movement) * (0.5f * deltaTime), 0.0f);
    }
    // 오른쪽 곱은 현재 객체의 로컬 축 회전이다. 위치는 별도로 보존한다.
    const float angle = glm::radians(45.0f * deltaTime);
    orientation_ = glm::normalize(orientation_
        * glm::angleAxis(angle * angularDirection_.x, glm::vec3(1.0f, 0.0f, 0.0f))
        * glm::angleAxis(angle * angularDirection_.y, glm::vec3(0.0f, 1.0f, 0.0f)));
    applyTransform();
}

void Console::onFocus(bool focused) {
    if (!focused) {
        pressed_.fill(false); // 포커스 밖에서 놓친 release가 이동을 계속시키지 않는다.
    }
}
````

## 변경 묶음 15: Console.h

변경 이유: 과제별 입력·상태·변환을 구현한다.

최종 파일 SHA-256: `f74077e724853d5c67131202bc59d3186cb41686667f86bb8a93610fd171de73`

### openGL_3dAssign_14/Console.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#pragma once

#include "framework/include/Application.h"
#include <glfw3.h>
#include <array>

// 실습 14: 도형 선택, 연속 입력, 위치와 쿼터니언 회전을 관리한다.
class Console {
private:
    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    std::array<bool, GLFW_KEY_LAST + 1> pressed_{};
    glm::vec3 position_{ 0.0f };
    glm::quat orientation_{ 1.0f, 0.0f, 0.0f, 0.0f };
    glm::vec2 angularDirection_{ 0.0f };

    void selectShape(bool pyramid);
    void resetMotion();
    void applyTransform();

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    void onKey(int key, int action, int mods);
    void onUpdate(float deltaTime);
    void onFocus(bool focused);
};
````

## 변경 묶음 16: Graphics.h

변경 이유: 사각뿔, 꼭짓점 색상, 면 표시·변환, 렌더 옵션을 지원한다. 호환 헤더 사본은 기존 계약을 유지한다.

최종 파일 SHA-256: `1fec04909877800619185d81abe6993a4b7569942d5957775c87d92a5262e605`

### openGL_3dAssign_14/Graphics.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/Graphics.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#pragma once

// 이전 include 경로를 유지하는 호환 헤더.
#include "framework/include/Graphics.h"
````

## 변경 묶음 17: main.cpp

변경 이유: 과제별 입력·상태·변환을 구현한다.

최종 파일 SHA-256: `9e44dac6b072411db184537280b7b49760436707fe59374928f13c9f6825e3b3`

### openGL_3dAssign_14/main.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#include "Console.h"
#include <iostream>

int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 14";
    config.shaderDirectory = graphics::Application::findShaderDirectory();
    if (config.shaderDirectory.empty()) {
        std::cerr << "Shaders were not found beside the executable or in framework/shaders.\n";
        return -1;
    }
    application.setInitHandler([&](graphics::Application&) {
        return practice.initScene();
    });
    application.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
        practice.onKey(key, action, mods);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        practice.onUpdate(deltaTime);
    });
    application.setFocusHandler([&](graphics::Application&, bool focused) {
        practice.onFocus(focused);
    });
    return application.run(config);
}
````

## 변경 묶음 18: openGL_assign_14.slnx

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `b45e575a19a3fa788594fea0f1f5b185b3cb8545c2e071dceda77b2978197164`

### openGL_3dAssign_14/openGL_assign_14.slnx — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````xml
<Solution>
  <Configurations>
    <Platform Name="x64" />
    <Platform Name="x86" />
  </Configurations>
  <Project Path="openGL_sample2.vcxproj" Id="28b28099-fea2-4804-9635-0eccde411a97" />
</Solution>
````

## 변경 묶음 19: openGL_sample2.vcxproj

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `540a956fefd043a9d27cc5b5ded49ae47cc07766f193a006f6d95a6ae8f4dca3`

### openGL_3dAssign_14/openGL_sample2.vcxproj — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>18.0</VCProjectVersion>
    <Keyword>Win32Proj</Keyword>
    <ProjectGuid>{28b28099-fea2-4804-9635-0eccde411a97}</ProjectGuid>
    <RootNamespace>openGL_assign_14</RootNamespace>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
    <ProjectName>openGL_assign_14</ProjectName>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpplatest</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="framework\src\Graphics.cpp" />
    <ClCompile Include="framework\src\Application.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="main.cpp" />
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="framework\include\Graphics.h" />
    <ClInclude Include="framework\include\Application.h" />
    <ClInclude Include="Common.h" />
    <ClInclude Include="Graphics.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="resource.h" />
  </ItemGroup>
  <ItemGroup>
    <None Include="framework\shaders\FragmentShader.glsl" />
    <None Include="packages.config" />
    <None Include="framework\Framework.props" />
    <None Include="framework\shaders\VertexShader.glsl" />
  </ItemGroup>
  <ItemGroup>
    <ResourceCompile Include="openGL_assign_789.rc" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets" />
</Project>
````

## 변경 묶음 20: openGL_sample2.vcxproj.filters

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `7e70de1ae122e8a50c11135bc22aa1e5e047c624fdf70b31967ecf19749f318e`

### openGL_3dAssign_14/openGL_sample2.vcxproj.filters — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/openGL_sample2.vcxproj.filters — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````xml
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <Filter Include="소스 파일">
      <UniqueIdentifier>{4FC737F1-C7A5-4376-A066-2A32D752A2FF}</UniqueIdentifier>
      <Extensions>cpp;c;cc;cxx;c++;cppm;ixx;def;odl;idl;hpj;bat;asm;asmx</Extensions>
    </Filter>
    <Filter Include="헤더 파일">
      <UniqueIdentifier>{93995380-89BD-4b04-88EB-625FBE52EBFB}</UniqueIdentifier>
      <Extensions>h;hh;hpp;hxx;h++;hm;inl;inc;ipp;xsd</Extensions>
    </Filter>
    <Filter Include="리소스 파일">
      <UniqueIdentifier>{67DA6AB6-F800-4c08-8B7A-83BB121AAD01}</UniqueIdentifier>
      <Extensions>rc;ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe;resx;tiff;tif;png;wav;mfcribbon-ms</Extensions>
    </Filter>
    <Filter Include="쉐이더 파일">
      <UniqueIdentifier>{c0045b40-ad4a-473e-be65-6546fceae441}</UniqueIdentifier>
    </Filter>
  </ItemGroup>
  <ItemGroup>
    <ClCompile Include="framework\src\Graphics.cpp">
      <Filter>소스 파일</Filter>
    </ClCompile>
    <ClCompile Include="framework\src\Application.cpp" />
    <ClCompile Include="Console.cpp">
      <Filter>소스 파일</Filter>
    </ClCompile>
    <ClCompile Include="main.cpp">
      <Filter>소스 파일</Filter>
    </ClCompile>
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="Graphics.h">
      <Filter>헤더 파일</Filter>
    </ClInclude>
    <ClInclude Include="framework\include\Graphics.h" />
    <ClInclude Include="framework\include\Application.h" />
    <ClInclude Include="Common.h">
      <Filter>헤더 파일</Filter>
    </ClInclude>
    <ClInclude Include="Console.h">
      <Filter>헤더 파일</Filter>
    </ClInclude>
    <ClInclude Include="resource.h">
      <Filter>헤더 파일</Filter>
    </ClInclude>
  </ItemGroup>
  <ItemGroup>
    <None Include="packages.config" />
    <None Include="framework\shaders\VertexShader.glsl">
      <Filter>쉐이더 파일</Filter>
    </None>
    <None Include="framework\shaders\FragmentShader.glsl">
      <Filter>쉐이더 파일</Filter>
    </None>
  </ItemGroup>
  <ItemGroup>
    <ResourceCompile Include="openGL_assign_789.rc">
      <Filter>리소스 파일</Filter>
    </ResourceCompile>
  </ItemGroup>
</Project>
````

## 변경 묶음 21: packages.config

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `abb02f5a3fca83e66d3e7fde3fe7e36159376f25db40c5fc9aa0309411a842e4`

### openGL_3dAssign_14/packages.config — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/packages.config — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````xml
<?xml version="1.0" encoding="utf-8"?>
<packages>
  <package id="freeglut.3.0.0.v140" version="1.0.2" targetFramework="native" />
</packages>
````

## 변경 묶음 22: FragmentShader.glsl

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `058f54b9e205d74a24267ce1150137f592c8560c95a484906ba26c6f5b8b5185`

### openGL_3dAssign_14/framework/shaders/FragmentShader.glsl — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/framework/shaders/FragmentShader.glsl — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### framework/shaders/FragmentShader.glsl — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````glsl
#version 330 core

in vec3 outColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(outColor, 1.0);
}
````

## 변경 묶음 23: VertexShader.glsl

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `0b909595c40ac1bee3a9cd27ad337ff10104de13f6b4970e7ec8dc46fa33ba45`

### openGL_3dAssign_14/framework/shaders/VertexShader.glsl — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### openGL_3dAssign_15/framework/shaders/VertexShader.glsl — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### framework/shaders/VertexShader.glsl — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````glsl
#version 330 core

layout(location = 0) in vec3 vPos;
layout(location = 1) in vec3 vColor;

uniform mat4 uMVP;
out vec3 outColor;

void main()
{
    gl_Position = uMVP * vec4(vPos, 1.0);
    outColor = vColor;
}
````

## 변경 묶음 24: Console.cpp

변경 이유: 과제별 입력·상태·변환을 구현한다.

최종 파일 SHA-256: `375f150af919273b0f85a612606e4a90c8b465acded298c156f33cff99049ece`

### openGL_3dAssign_15/Console.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#include "Console.h"
#include <glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>

namespace {

constexpr float HALF_SIZE = 0.4f;
constexpr float ROTATION_SPEED = 45.0f;
const glm::vec3 X_AXIS{ 1.0f, 0.0f, 0.0f };
const glm::vec3 Y_AXIS{ 0.0f, 1.0f, 0.0f };
const glm::vec3 Z_AXIS{ 0.0f, 0.0f, 1.0f };

glm::mat4 rotateAround(const glm::vec3& pivot, const glm::vec3& axis, float degrees) {
    return glm::translate(glm::mat4(1.0f), pivot)
        * glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axis)
        * glm::translate(glm::mat4(1.0f), -pivot);
}

glm::mat4 scaleAround(const glm::vec3& pivot, float scale) {
    return glm::translate(glm::mat4(1.0f), pivot)
        * glm::scale(glm::mat4(1.0f), glm::vec3(scale))
        * glm::translate(glm::mat4(1.0f), -pivot);
}

} // namespace

bool Console::initScene() {
    auto& scene = application_.getScene();
    scene.clear();
    const auto cube = scene.addCuboid(glm::vec3(HALF_SIZE * 2.0f), glm::vec3(0.0f));
    const auto pyramid = scene.addPyramid(glm::vec3(HALF_SIZE * 2.0f), glm::vec3(0.0f));
    if (!cube || !pyramid) {
        return false;
    }
    cuboidId_ = *cube;
    pyramidId_ = *pyramid;
    if (!scene.addLine({ -1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.2f, 0.2f })
        || !scene.addLine({ 0.0f, -1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.2f, 1.0f, 0.2f })
        || !scene.addLine({ 0.0f, 0.0f, -1.0f }, { 0.0f, 0.0f, 1.0f }, { 0.2f, 0.4f, 1.0f })) {
        return false;
    }
    auto& camera = application_.getCamera();
    camera.position = { 1.8f, 1.2f, 3.0f };
    camera.orthographic = true;
    camera.orthographicHalfExtent = 1.0f;
    resetAll();
    std::cout << "Practice 15: H depth, P cube/pyramid, Y spin/pause, R reset, Q/Esc quit\n"
              << "Cube only: T top Z rotation, F front open/close, S right-face normal rotation\n"
              << "B back shrink/restore, 1 adjacent pair hinge 360, 2 adjacent pair swap 90\n"
              << "4 four side faces orbit Y, 5 top/bottom/front/back orbit X\n"
              << "Only one face motion is active. Object spin can run with it.\n"
              << "T/S/4/5 repeat to pause/resume. F/B repeat to reverse.\n";
    return true;
}

void Console::resetFaces() {
    auto& scene = application_.getScene();
    for (auto id : scene.getIds()) {
        const bool visible = id == (pyramidSelected_ ? pyramidId_ : cuboidId_);
        for (std::size_t i = 0; i < scene.getComponent(id)->polyhedron.faces.size(); ++i) {
            scene.setFaceLocalMatrix(id, i, glm::mat4(1.0f));
            scene.setFaceVisible(id, i, visible);
        }
    }
}

void Console::selectShape(bool pyramid) {
    pyramidSelected_ = pyramid;
    motion_ = FaceMotion{};
    resetFaces();
    applyObjectTransform();
}

void Console::resetAll() {
    spinning_ = false;
    spinDegrees_ = 0.0f;
    application_.getRenderOptions() = graphics::RenderOptions{};
    selectShape(false);
}

void Console::applyObjectTransform() {
    graphics::Transform3D transform;
    transform.rotationDegrees = { 30.0f, 30.0f, 0.0f };
    // 초기 기울기 다음에 로컬 Y축 자전을 적용한다.
    transform.orientation = glm::angleAxis(glm::radians(spinDegrees_), Y_AXIS);
    application_.getScene().setTransform(pyramidSelected_ ? pyramidId_ : cuboidId_, transform);
}

void Console::beginMotion(Motion kind) {
    if (pyramidSelected_) {
        return; // 사각뿔에서는 육면체 면 명령을 무시한다.
    }
    if (motion_.kind == kind) {
        if (kind == Motion::Front || kind == Motion::Back) {
            motion_.direction = -motion_.direction;
            motion_.running = true;
            return;
        }
        if (kind != Motion::Hinge && kind != Motion::Swap) {
            motion_.running = !motion_.running;
            return;
        }
    }
    // 새 동작은 이전 동작의 면 행렬·숨김을 모두 초기화한다.
    resetFaces();
    motion_ = FaceMotion{};
    motion_.kind = kind;
    motion_.running = true;
    if (kind == Motion::Hinge || kind == Motion::Swap) {
        const std::array<std::array<std::size_t, 2>, 4> pairs{
            std::array<std::size_t, 2>{ 0, 3 }, { 3, 1 }, { 1, 2 }, { 2, 0 }
        };
        const std::array<glm::vec3, 4> corners{
            glm::vec3{ HALF_SIZE, 0.0f, HALF_SIZE }, { HALF_SIZE, 0.0f, -HALF_SIZE },
            { -HALF_SIZE, 0.0f, -HALF_SIZE }, { -HALF_SIZE, 0.0f, HALF_SIZE }
        };
        const int index = std::uniform_int_distribution<int>(0, 3)(random_);
        motion_.pair = pairs[index];
        motion_.hinge = corners[index];
        std::cout << "Selected cube faces (1-based): " << motion_.pair[0] + 1
                  << ", " << motion_.pair[1] + 1 << '\n';
    }
    applyFaceMotion();
}

void Console::applyFaceMotion() {
    auto& scene = application_.getScene();
    const auto setMatrix = [&](std::size_t face, const glm::mat4& matrix) {
        scene.setFaceLocalMatrix(cuboidId_, face, matrix);
    };
    const float progress = motion_.progress;
    switch (motion_.kind) {
    case Motion::Top:
        setMatrix(4, rotateAround({ 0.0f, HALF_SIZE, 0.0f }, Z_AXIS, progress));
        break;
    case Motion::Front:
        // 음의 X 회전으로 앞면이 바깥쪽으로 열린다. 위쪽 모서리는 고정된다.
        setMatrix(0, rotateAround({ 0.0f, HALF_SIZE, HALF_SIZE }, X_AXIS, -90.0f * progress));
        break;
    case Motion::Side:
        setMatrix(3, rotateAround({ HALF_SIZE, 0.0f, 0.0f }, X_AXIS, progress));
        break;
    case Motion::Back:
        setMatrix(1, scaleAround({ 0.0f, 0.0f, -HALF_SIZE }, 1.0f - progress));
        scene.setFaceVisible(cuboidId_, 1, progress < 1.0f);
        break;
    case Motion::Hinge: {
        const auto matrix = progress == 360.0f ? glm::mat4(1.0f)
            : rotateAround(motion_.hinge, Y_AXIS, progress);
        setMatrix(motion_.pair[0], matrix);
        setMatrix(motion_.pair[1], matrix);
        break;
    }
    case Motion::Swap:
        // 순서는 앞→오른쪽→뒤→왼쪽이다. +90/-90이면 두 중심과 면이 서로 교체된다.
        setMatrix(motion_.pair[0], rotateAround(glm::vec3(0.0f), Y_AXIS, progress));
        setMatrix(motion_.pair[1], rotateAround(glm::vec3(0.0f), Y_AXIS, -progress));
        break;
    case Motion::OrbitY:
        for (std::size_t face : { 0u, 1u, 2u, 3u }) {
            setMatrix(face, rotateAround(glm::vec3(0.0f), Y_AXIS, progress));
        }
        break;
    case Motion::OrbitX:
        for (std::size_t face : { 4u, 5u, 0u, 1u }) {
            setMatrix(face, rotateAround(glm::vec3(0.0f), X_AXIS, progress));
        }
        break;
    default: break;
    }
}

void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }
    switch (key) {
    case GLFW_KEY_Q:
    case GLFW_KEY_ESCAPE: application_.requestClose(); break;
    case GLFW_KEY_H: {
        auto& options = application_.getRenderOptions();
        options.depthTest = !options.depthTest;
        break;
    }
    case GLFW_KEY_P: selectShape(!pyramidSelected_); break;
    case GLFW_KEY_Y: spinning_ = !spinning_; break;
    case GLFW_KEY_R: resetAll(); break;
    case GLFW_KEY_T: beginMotion(Motion::Top); break;
    case GLFW_KEY_F: beginMotion(Motion::Front); break;
    case GLFW_KEY_S: beginMotion(Motion::Side); break;
    case GLFW_KEY_B: beginMotion(Motion::Back); break;
    case GLFW_KEY_1: beginMotion(Motion::Hinge); break;
    case GLFW_KEY_2: beginMotion(Motion::Swap); break;
    case GLFW_KEY_4: beginMotion(Motion::OrbitY); break;
    case GLFW_KEY_5: beginMotion(Motion::OrbitX); break;
    default: break;
    }
}

void Console::onUpdate(float deltaTime) {
    if (spinning_) {
        spinDegrees_ = std::fmod(spinDegrees_ + ROTATION_SPEED * deltaTime, 360.0f);
        applyObjectTransform();
    }
    if (!motion_.running || pyramidSelected_) {
        return;
    }
    if (motion_.kind == Motion::Front || motion_.kind == Motion::Back) {
        motion_.progress = std::clamp(motion_.progress + motion_.direction * deltaTime, 0.0f, 1.0f);
        if (motion_.progress == 0.0f || motion_.progress == 1.0f) {
            motion_.running = false;
        }
    } else if (motion_.kind == Motion::Hinge || motion_.kind == Motion::Swap) {
        const float end = motion_.kind == Motion::Hinge ? 360.0f : 90.0f;
        motion_.progress = std::min(end, motion_.progress + ROTATION_SPEED * deltaTime);
        if (motion_.progress == end) {
            motion_.running = false;
        }
    } else {
        motion_.progress = std::fmod(motion_.progress + ROTATION_SPEED * deltaTime, 360.0f);
    }
    applyFaceMotion();
}
````

## 변경 묶음 25: Console.h

변경 이유: 과제별 입력·상태·변환을 구현한다.

최종 파일 SHA-256: `d096cf597c866561e93379cc265b9ba3945bb3585431367663cf030391f31646`

### openGL_3dAssign_15/Console.h — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#pragma once

#include "framework/include/Application.h"
#include <array>
#include <random>

// 실습 15: 전체 자전과 하나의 면 동작을 분리한다.
class Console {
private:
    enum class Motion { None, Top, Front, Side, Back, Hinge, Swap, OrbitY, OrbitX };
    struct FaceMotion {
        Motion kind = Motion::None;
        bool running = false;
        float progress = 0.0f; // 회전은 도, 열기·축소는 0~1이다.
        float direction = 1.0f;
        std::array<std::size_t, 2> pair{ 0, 3 };
        glm::vec3 hinge{ 0.4f, 0.0f, 0.4f };
    };

    graphics::Application& application_;
    graphics::EntityId cuboidId_ = 0;
    graphics::EntityId pyramidId_ = 0;
    bool pyramidSelected_ = false;
    bool spinning_ = false;
    float spinDegrees_ = 0.0f;
    FaceMotion motion_;
    std::mt19937 random_{ std::random_device{}() };

    void selectShape(bool pyramid);
    void resetFaces();
    void resetAll();
    void beginMotion(Motion kind);
    void applyObjectTransform();
    void applyFaceMotion();

public:
    explicit Console(graphics::Application& application) : application_(application) {}
    bool initScene();
    void onKey(int key, int action);
    void onUpdate(float deltaTime);
};
````

## 변경 묶음 26: main.cpp

변경 이유: 과제별 입력·상태·변환을 구현한다.

최종 파일 SHA-256: `ca58426f062c919a5a09893e5b6e825141439bb2f5cfdc17521fcf50cab9e4de`

### openGL_3dAssign_15/main.cpp — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````cpp
#include "Console.h"
#include <iostream>

int main() {
    graphics::Application application;
    Console practice(application);
    graphics::ApplicationConfig config;
    config.title = "OpenGL Practice 15";
    config.shaderDirectory = graphics::Application::findShaderDirectory();
    if (config.shaderDirectory.empty()) {
        std::cerr << "Shaders were not found beside the executable or in framework/shaders.\n";
        return -1;
    }
    application.setInitHandler([&](graphics::Application&) {
        return practice.initScene();
    });
    application.setKeyHandler([&](graphics::Application&, int key, int action) {
        practice.onKey(key, action);
    });
    application.setUpdateHandler([&](graphics::Application&, float deltaTime) {
        practice.onUpdate(deltaTime);
    });
    return application.run(config);
}
````

## 변경 묶음 27: openGL_assign_15.slnx

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `2301f1b73521a354397d22d8f135f9bc1c4db244873c73f6ef26624c08ae852f`

### openGL_3dAssign_15/openGL_assign_15.slnx — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````xml
<Solution>
  <Configurations>
    <Platform Name="x64" />
    <Platform Name="x86" />
  </Configurations>
  <Project Path="openGL_sample2.vcxproj" Id="e4f924ff-a6ee-4e7f-b746-87e83cc809e3" />
</Solution>
````

## 변경 묶음 28: openGL_sample2.vcxproj

변경 이유: 공용 API, 프로젝트 연결 또는 독립 사본을 구성한다.

최종 파일 SHA-256: `78373f44136f8aea580c518967c14e4127b7dab9837eaf967a78606398ceeaed`

### openGL_3dAssign_15/openGL_sample2.vcxproj — before

없음. 이번 작업에서 새로 만든 사본 또는 파일이다.

### after — 위 경로 전체의 최종 저장 내용

````xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|Win32">
      <Configuration>Debug</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|Win32">
      <Configuration>Release</Configuration>
      <Platform>Win32</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <VCProjectVersion>18.0</VCProjectVersion>
    <Keyword>Win32Proj</Keyword>
    <ProjectGuid>{e4f924ff-a6ee-4e7f-b746-87e83cc809e3}</ProjectGuid>
    <RootNamespace>openGL_assign_15</RootNamespace>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
    <ProjectName>openGL_assign_15</ProjectName>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>true</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'" Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <UseDebugLibraries>false</UseDebugLibraries>
    <PlatformToolset>v145</PlatformToolset>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <CharacterSet>Unicode</CharacterSet>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <ImportGroup Label="ExtensionSettings">
  </ImportGroup>
  <ImportGroup Label="Shared">
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Import Project="$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props" Condition="exists('$(UserRootDir)\Microsoft.Cpp.$(Platform).user.props')" Label="LocalAppDataPlatform" />
  </ImportGroup>
  <PropertyGroup Label="UserMacros" />
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>WIN32;NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>_DEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpplatest</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemDefinitionGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <ClCompile>
      <WarningLevel>Level3</WarningLevel>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <SDLCheck>true</SDLCheck>
      <PreprocessorDefinitions>NDEBUG;_CONSOLE;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <ConformanceMode>true</ConformanceMode>
      <LanguageStandard>stdcpp20</LanguageStandard>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
    <Manifest>
      <EnableSegmentHeap>true</EnableSegmentHeap>
    </Manifest>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="framework\src\Graphics.cpp" />
    <ClCompile Include="framework\src\Application.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClCompile Include="main.cpp" />
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="framework\include\Graphics.h" />
    <ClInclude Include="framework\include\Application.h" />
    <ClInclude Include="Common.h" />
    <ClInclude Include="Graphics.h" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="resource.h" />
  </ItemGroup>
  <ItemGroup>
    <None Include="framework\shaders\FragmentShader.glsl" />
    <None Include="packages.config" />
    <None Include="framework\Framework.props" />
    <None Include="framework\shaders\VertexShader.glsl" />
  </ItemGroup>
  <ItemGroup>
    <ResourceCompile Include="openGL_assign_789.rc" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
  <ImportGroup Label="ExtensionTargets" />
</Project>
````

## 외부 의존성과 과거 문서 사본

이 목록의 파일은 기존 실습 13에서 그대로 복사했다. 외부 소스와 라이선스를 수정하지 않았다.
바이너리 파일에는 텍스트 before/after를 적용하지 않는다. 모든 사본의 내용을 해시로 식별한다.

| 원본의 상대 경로 | 사본 위치 | SHA-256 |
|---|---|---|
| CHANGES-DEPENDENCIES.md | openGL_3dAssign_14/framework/CHANGES-DEPENDENCIES.md<br>openGL_3dAssign_15/framework/CHANGES-DEPENDENCIES.md<br>framework/CHANGES-DEPENDENCIES.md | ddb99d22347632afc2a340db7287b2a27749b1a28bc49c197d1af7e5133aa8fc |
| CHANGES.md | openGL_3dAssign_14/framework/CHANGES.md<br>openGL_3dAssign_15/framework/CHANGES.md<br>framework/CHANGES.md | a7a12c803a1b7ca8b3051aad1cd4b5a793a1e14ca262928b9f2e99dc9a1b19c1 |
| dependencies/glew-2.3.1/bin/Release/x64/glew32.dll | openGL_3dAssign_14/framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll<br>openGL_3dAssign_15/framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll<br>framework/dependencies/glew-2.3.1/bin/Release/x64/glew32.dll | a4428b3283ded13b701797d433343eb923d29efe198d27d89e1f3f64f6516427 |
| dependencies/glew-2.3.1/include/GL/eglew.h | openGL_3dAssign_14/framework/dependencies/glew-2.3.1/include/GL/eglew.h<br>openGL_3dAssign_15/framework/dependencies/glew-2.3.1/include/GL/eglew.h<br>framework/dependencies/glew-2.3.1/include/GL/eglew.h | feb986fe31fb9a48fef1b39f6042be2602daedb97a6f8b9125bc3596c08e41db |
| dependencies/glew-2.3.1/include/GL/glew.h | openGL_3dAssign_14/framework/dependencies/glew-2.3.1/include/GL/glew.h<br>openGL_3dAssign_15/framework/dependencies/glew-2.3.1/include/GL/glew.h<br>framework/dependencies/glew-2.3.1/include/GL/glew.h | 7e8851309fb6ff9a01ec6317ef38d0534d29eaae80ee2e9cbe1050c8f6a5c825 |
| dependencies/glew-2.3.1/include/GL/glxew.h | openGL_3dAssign_14/framework/dependencies/glew-2.3.1/include/GL/glxew.h<br>openGL_3dAssign_15/framework/dependencies/glew-2.3.1/include/GL/glxew.h<br>framework/dependencies/glew-2.3.1/include/GL/glxew.h | 313e9191173fe3bc2dfc979c35668d542d44a3185e3119414ca65093e2ab3779 |
| dependencies/glew-2.3.1/include/GL/wglew.h | openGL_3dAssign_14/framework/dependencies/glew-2.3.1/include/GL/wglew.h<br>openGL_3dAssign_15/framework/dependencies/glew-2.3.1/include/GL/wglew.h<br>framework/dependencies/glew-2.3.1/include/GL/wglew.h | 0ca21745247a7903204959b1d96f944c3873cac9c6d1d03ed74d9273d312fda2 |
| dependencies/glew-2.3.1/lib/Release/x64/glew32.lib | openGL_3dAssign_14/framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib<br>openGL_3dAssign_15/framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib<br>framework/dependencies/glew-2.3.1/lib/Release/x64/glew32.lib | b8c3e33e1c08beba9c9c60e7a1c795bdb1cd3982db9eb6332f98ed972dcc5f65 |
| dependencies/glew-2.3.1/LICENSE.txt | openGL_3dAssign_14/framework/dependencies/glew-2.3.1/LICENSE.txt<br>openGL_3dAssign_15/framework/dependencies/glew-2.3.1/LICENSE.txt<br>framework/dependencies/glew-2.3.1/LICENSE.txt | 38ee953cbae3c9d8b26308bdf92a1d0d6b79629165b58946e92df2f88001f16f |
| dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h | openGL_3dAssign_14/framework/dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h<br>openGL_3dAssign_15/framework/dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h<br>framework/dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h | 182f851c480be6f79b27b064171e7f43572d3fe195c364dc2932277f98da9d04 |
| dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3native.h | openGL_3dAssign_14/framework/dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3native.h<br>openGL_3dAssign_15/framework/dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3native.h<br>framework/dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3native.h | b66cb1327a98a5a8ecede8eefe48eca3b702b09c7fc0bec04ad0bf86c7bd81ee |
| dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll | openGL_3dAssign_14/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll<br>openGL_3dAssign_15/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll<br>framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll | 4429adff46d4d1038bb5836cecc59661eef7126e4941de6fdfde8855e7c14bb1 |
| dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib | openGL_3dAssign_14/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib<br>openGL_3dAssign_15/framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib<br>framework/dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib | 3b1e0f2e4e9f443838a0917b7c58aedc4af2c25823a811254a75c5b1ad62e0d3 |
| dependencies/glfw-3.4.bin.WIN64/LICENSE.md | openGL_3dAssign_14/framework/dependencies/glfw-3.4.bin.WIN64/LICENSE.md<br>openGL_3dAssign_15/framework/dependencies/glfw-3.4.bin.WIN64/LICENSE.md<br>framework/dependencies/glfw-3.4.bin.WIN64/LICENSE.md | 6d091ca4f2cb3e9c53d83112ebb63774ecd40c64b468003b348bf0432386f5a6 |
| dependencies/glm/copying.txt | openGL_3dAssign_14/framework/dependencies/glm/copying.txt<br>openGL_3dAssign_15/framework/dependencies/glm/copying.txt<br>framework/dependencies/glm/copying.txt | 142911fd27af238e59643eacefa39d8412c3f9d3bf2026c2c1e2a0bd00d4b4d2 |
| dependencies/glm/glm/CMakeLists.txt | openGL_3dAssign_14/framework/dependencies/glm/glm/CMakeLists.txt<br>openGL_3dAssign_15/framework/dependencies/glm/glm/CMakeLists.txt<br>framework/dependencies/glm/glm/CMakeLists.txt | 0c45721afef81c3526612a8ecfe045eef3da48d09ae15ac61f38ba02233ed219 |
| dependencies/glm/glm/common.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/common.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/common.hpp<br>framework/dependencies/glm/glm/common.hpp | 48d0b170fc8d4bfe81c5aea565e874613f989defee4f3d69e8c253a8f36ee737 |
| dependencies/glm/glm/detail/_features.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/_features.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/_features.hpp<br>framework/dependencies/glm/glm/detail/_features.hpp | 50f94f2f93f598cc719388a69d9532b0986a1e47cadbcfb3b424ad5f6275d304 |
| dependencies/glm/glm/detail/_fixes.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/_fixes.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/_fixes.hpp<br>framework/dependencies/glm/glm/detail/_fixes.hpp | 56f2af1c102984685206613ebb40335dbce3c14e645de4295ecc8d6043dd1786 |
| dependencies/glm/glm/detail/_noise.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/_noise.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/_noise.hpp<br>framework/dependencies/glm/glm/detail/_noise.hpp | cec1a5818e6e57c8006c5f796239ff130b033f49a0eccacc24e016f472adc424 |
| dependencies/glm/glm/detail/_swizzle.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/_swizzle.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/_swizzle.hpp<br>framework/dependencies/glm/glm/detail/_swizzle.hpp | d898ea2c0c7497438abe4c8ad8ff1c9fa01035433b6a87ce7bf0efcac3d1b7fa |
| dependencies/glm/glm/detail/_swizzle_func.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/_swizzle_func.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/_swizzle_func.hpp<br>framework/dependencies/glm/glm/detail/_swizzle_func.hpp | fbc8edbbaac0ac999a759f2ce5fca903230b9535e8f331004982de9c683f45c9 |
| dependencies/glm/glm/detail/_vectorize.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/_vectorize.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/_vectorize.hpp<br>framework/dependencies/glm/glm/detail/_vectorize.hpp | d793730202775daaec85cdae89cb89c8f9c35954f8621fbb8bdaf8f1fe31701d |
| dependencies/glm/glm/detail/compute_common.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/compute_common.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/compute_common.hpp<br>framework/dependencies/glm/glm/detail/compute_common.hpp | e19139d3505101a73dd8f1a0a6252a01bb0ce6f6afa85bfb3ef367ebb48e5efd |
| dependencies/glm/glm/detail/compute_vector_decl.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/compute_vector_decl.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/compute_vector_decl.hpp<br>framework/dependencies/glm/glm/detail/compute_vector_decl.hpp | 53bc95c34ca4f33e022b2f9fca43e97ca8decceb7fea7d3cc6d9ce9b385036a5 |
| dependencies/glm/glm/detail/compute_vector_relational.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/compute_vector_relational.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/compute_vector_relational.hpp<br>framework/dependencies/glm/glm/detail/compute_vector_relational.hpp | 81cb9d8c12f083227a0ed9a94a4147a4b4b5f0c3be809b978571df4b7f86649c |
| dependencies/glm/glm/detail/func_common.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_common.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_common.inl<br>framework/dependencies/glm/glm/detail/func_common.inl | 886b8f4cac597a6796bc65c50d7d43d474f850c397f7fbb84b4532314d740c54 |
| dependencies/glm/glm/detail/func_common_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_common_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_common_simd.inl<br>framework/dependencies/glm/glm/detail/func_common_simd.inl | 9d12159e26d03440b55cebcc0d6101eb30522d0e6df486b7e3788a5adb188cbe |
| dependencies/glm/glm/detail/func_exponential.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_exponential.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_exponential.inl<br>framework/dependencies/glm/glm/detail/func_exponential.inl | d0050a125dc20f0aef0d721b0119ced6af24430e6ed08af5420f1c773eb18221 |
| dependencies/glm/glm/detail/func_exponential_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_exponential_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_exponential_simd.inl<br>framework/dependencies/glm/glm/detail/func_exponential_simd.inl | 1bf6a6d54c8f7a244e3378ac4e46c598ef332811b7dd4b570b47332c6e89edc0 |
| dependencies/glm/glm/detail/func_geometric.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_geometric.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_geometric.inl<br>framework/dependencies/glm/glm/detail/func_geometric.inl | c671e4733da6bb1ea22b6e76eea7f20a737357ba9f0b01a2ac2c567998b8652d |
| dependencies/glm/glm/detail/func_geometric_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_geometric_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_geometric_simd.inl<br>framework/dependencies/glm/glm/detail/func_geometric_simd.inl | 1991e7ef10f8a3711b4c85da87448eeb35db8f53b6c2daa1303394623285ee12 |
| dependencies/glm/glm/detail/func_integer.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_integer.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_integer.inl<br>framework/dependencies/glm/glm/detail/func_integer.inl | 5de79b2fbf67a6a32d51cf37510c34ef09e683279ecdd3c2d4b0fa6d7b89f574 |
| dependencies/glm/glm/detail/func_integer_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_integer_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_integer_simd.inl<br>framework/dependencies/glm/glm/detail/func_integer_simd.inl | baefdb29125860e443e7a534b8d01bd4642dfa5a9875572d063d0df9fa3721ad |
| dependencies/glm/glm/detail/func_matrix.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_matrix.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_matrix.inl<br>framework/dependencies/glm/glm/detail/func_matrix.inl | dec91438d926e37422690dc2261b3c19bda5d1b67dbbf9dbb9c3f3aa3bbbe6ad |
| dependencies/glm/glm/detail/func_matrix_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_matrix_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_matrix_simd.inl<br>framework/dependencies/glm/glm/detail/func_matrix_simd.inl | ea51aa2c30277a084c87673e6e24dfec7d9e96064a341e97431f45cc4d1647a0 |
| dependencies/glm/glm/detail/func_packing.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_packing.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_packing.inl<br>framework/dependencies/glm/glm/detail/func_packing.inl | c35a936c4cb68682c137df454b0d88163f3f3d012cea40c8c3639ae6fec1830a |
| dependencies/glm/glm/detail/func_packing_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_packing_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_packing_simd.inl<br>framework/dependencies/glm/glm/detail/func_packing_simd.inl | d59a7ff19ca90848710933ada5df8da312439977051b3ddc566aa9bd9903dc12 |
| dependencies/glm/glm/detail/func_trigonometric.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_trigonometric.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_trigonometric.inl<br>framework/dependencies/glm/glm/detail/func_trigonometric.inl | 8dce04a74b72b88d312ee9f3025e0ccae235beb1bf5835d16fc7284cd2ee07f1 |
| dependencies/glm/glm/detail/func_trigonometric_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_trigonometric_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_trigonometric_simd.inl<br>framework/dependencies/glm/glm/detail/func_trigonometric_simd.inl | f1945cd6c19e56b3c1c78943ef5ec18116907a4ca1efc40a57d48ab1db7adfc5 |
| dependencies/glm/glm/detail/func_vector_relational.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_vector_relational.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_vector_relational.inl<br>framework/dependencies/glm/glm/detail/func_vector_relational.inl | d7e625f03f3949e06b34fb2075584b8e8b7dae0b4b345a19abd8bc2a27a95165 |
| dependencies/glm/glm/detail/func_vector_relational_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/func_vector_relational_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/func_vector_relational_simd.inl<br>framework/dependencies/glm/glm/detail/func_vector_relational_simd.inl | d59a7ff19ca90848710933ada5df8da312439977051b3ddc566aa9bd9903dc12 |
| dependencies/glm/glm/detail/glm.cpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/glm.cpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/glm.cpp<br>framework/dependencies/glm/glm/detail/glm.cpp | 66a3757ef93bcbbd125be1d5b9cc74b155483fc70679ced1cddb211aed1dd665 |
| dependencies/glm/glm/detail/qualifier.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/qualifier.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/qualifier.hpp<br>framework/dependencies/glm/glm/detail/qualifier.hpp | b600bebe59852af7922430378c64741255c08fd723f89ce446bf0cec3fa62cc3 |
| dependencies/glm/glm/detail/setup.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/setup.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/setup.hpp<br>framework/dependencies/glm/glm/detail/setup.hpp | 5cd3c1910234033904cf05e7c26f13bdc15e962d42104ad5af18563b2a9a6990 |
| dependencies/glm/glm/detail/type_float.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_float.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_float.hpp<br>framework/dependencies/glm/glm/detail/type_float.hpp | 9bc3296ba7ee8abc736bc1702cd235e63a0073088e282b09de1be8936a19940e |
| dependencies/glm/glm/detail/type_half.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_half.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_half.hpp<br>framework/dependencies/glm/glm/detail/type_half.hpp | 3c9a5ee63e531d99b1d40d3a85282f96c43f21901565328b78156a714381028b |
| dependencies/glm/glm/detail/type_half.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_half.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_half.inl<br>framework/dependencies/glm/glm/detail/type_half.inl | 341729a03107c010eab6f60da3a9aac7836854790d1c4547a865c56817ab6221 |
| dependencies/glm/glm/detail/type_mat2x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat2x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat2x2.hpp<br>framework/dependencies/glm/glm/detail/type_mat2x2.hpp | 937aa8268f8018992edfaf8c8119afbe7bd44ff2ab59e2a75e35c767af49bf87 |
| dependencies/glm/glm/detail/type_mat2x2.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat2x2.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat2x2.inl<br>framework/dependencies/glm/glm/detail/type_mat2x2.inl | 07a6b9b13a9a4841f2aabaf348472a70491864d412210aa3b12fa3553560deb1 |
| dependencies/glm/glm/detail/type_mat2x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat2x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat2x3.hpp<br>framework/dependencies/glm/glm/detail/type_mat2x3.hpp | 6a80157ac17058876986ea8f555262f391caba30ffea29f23cf85705c234d9d9 |
| dependencies/glm/glm/detail/type_mat2x3.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat2x3.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat2x3.inl<br>framework/dependencies/glm/glm/detail/type_mat2x3.inl | 628403a6c9604dface15711fea9692b0459430a7685ceedd38dfadd68ccc3725 |
| dependencies/glm/glm/detail/type_mat2x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat2x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat2x4.hpp<br>framework/dependencies/glm/glm/detail/type_mat2x4.hpp | 38f2a429bba596470183865a6c6f045d013ad5fd3dbedb6bec27dc8f96c2c890 |
| dependencies/glm/glm/detail/type_mat2x4.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat2x4.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat2x4.inl<br>framework/dependencies/glm/glm/detail/type_mat2x4.inl | 071416302f5c0da02599217dfa618919fbce8b5323161ac7876261bfa87d337f |
| dependencies/glm/glm/detail/type_mat3x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat3x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat3x2.hpp<br>framework/dependencies/glm/glm/detail/type_mat3x2.hpp | d96770fa59e0d341a8811bdc4312d5594f13c3c95f96fd5dd9d93d6991418c51 |
| dependencies/glm/glm/detail/type_mat3x2.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat3x2.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat3x2.inl<br>framework/dependencies/glm/glm/detail/type_mat3x2.inl | 245d79c7b00f355a3840f8b78e658717e2d56bd644fa5f9fd1806be8e63d4105 |
| dependencies/glm/glm/detail/type_mat3x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat3x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat3x3.hpp<br>framework/dependencies/glm/glm/detail/type_mat3x3.hpp | 01d5dc18cfa048cd25f40bcc78e4e32b082ce42b949d6d5b9985ee8fef1d8807 |
| dependencies/glm/glm/detail/type_mat3x3.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat3x3.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat3x3.inl<br>framework/dependencies/glm/glm/detail/type_mat3x3.inl | cabeca31c8af7b4f25558204ea17be9ac3502a077d059f5a153df94365508615 |
| dependencies/glm/glm/detail/type_mat3x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat3x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat3x4.hpp<br>framework/dependencies/glm/glm/detail/type_mat3x4.hpp | 3799b956c1194ae3a37d10e22c4d37e5f7952e39fcb4697e6f1cddd55fc9ce84 |
| dependencies/glm/glm/detail/type_mat3x4.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat3x4.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat3x4.inl<br>framework/dependencies/glm/glm/detail/type_mat3x4.inl | a0dcf42743e0382a2628124878aaaa7c8a981c3e1f3d0e0dae924ec4cfe574aa |
| dependencies/glm/glm/detail/type_mat4x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat4x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat4x2.hpp<br>framework/dependencies/glm/glm/detail/type_mat4x2.hpp | 077207ac339552caafcb335e64b9ed57bc8238088744c6d4de48218c0943946e |
| dependencies/glm/glm/detail/type_mat4x2.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat4x2.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat4x2.inl<br>framework/dependencies/glm/glm/detail/type_mat4x2.inl | d2fd9318adebab558d2c65c7e188f1e2c3a40b605aced78c8cce7782dd04535f |
| dependencies/glm/glm/detail/type_mat4x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat4x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat4x3.hpp<br>framework/dependencies/glm/glm/detail/type_mat4x3.hpp | 5d7c18e8f5dc5a3e7ccca5d0e7f46ca46a6ceb29604a866f351ba9594312e1be |
| dependencies/glm/glm/detail/type_mat4x3.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat4x3.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat4x3.inl<br>framework/dependencies/glm/glm/detail/type_mat4x3.inl | 77397c65708f55f0ad233f6070627a5565a01549c309a514dd76e2ec05df874a |
| dependencies/glm/glm/detail/type_mat4x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat4x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat4x4.hpp<br>framework/dependencies/glm/glm/detail/type_mat4x4.hpp | 642ac8fe4f5571080746fcded2800817dc5bd4314755a3596a1cbe5b3b62c4e2 |
| dependencies/glm/glm/detail/type_mat4x4.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat4x4.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat4x4.inl<br>framework/dependencies/glm/glm/detail/type_mat4x4.inl | e92d519215e7cd4fb739f72973747e852ae81110b4649a271d8d08e169bc792b |
| dependencies/glm/glm/detail/type_mat4x4_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_mat4x4_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_mat4x4_simd.inl<br>framework/dependencies/glm/glm/detail/type_mat4x4_simd.inl | bcc356a3c9fedb9bd177fc15d5b6ca0cd19a0571f5b0b568894ba8f51ed940a1 |
| dependencies/glm/glm/detail/type_quat.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_quat.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_quat.hpp<br>framework/dependencies/glm/glm/detail/type_quat.hpp | 61083f3084e17197cb59e8793543d537fe1a30667dd9573803d6f36419429cdd |
| dependencies/glm/glm/detail/type_quat.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_quat.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_quat.inl<br>framework/dependencies/glm/glm/detail/type_quat.inl | 97c89e7d114283aeec9ed274421ac0ac14961ca91a8a0330aadc452db6994a00 |
| dependencies/glm/glm/detail/type_quat_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_quat_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_quat_simd.inl<br>framework/dependencies/glm/glm/detail/type_quat_simd.inl | 4060eeb8bca977e059493c7b684d564965f9920a645bdce4e975d55ed699a74d |
| dependencies/glm/glm/detail/type_vec1.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_vec1.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_vec1.hpp<br>framework/dependencies/glm/glm/detail/type_vec1.hpp | 749ad682d740317de0cee106346f650d1317d770903839019d2192f31f6552f6 |
| dependencies/glm/glm/detail/type_vec1.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_vec1.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_vec1.inl<br>framework/dependencies/glm/glm/detail/type_vec1.inl | 086c38597bea0f5ecf8b5f1feb7303bd076c4d8e8557a6b77abb2cce22f630a9 |
| dependencies/glm/glm/detail/type_vec2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_vec2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_vec2.hpp<br>framework/dependencies/glm/glm/detail/type_vec2.hpp | 343f6dbc68c89fbfa820fa1f6cbaaded2b917155c10f0bef1e221368fa33ea0d |
| dependencies/glm/glm/detail/type_vec2.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_vec2.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_vec2.inl<br>framework/dependencies/glm/glm/detail/type_vec2.inl | 38234ca126a6b52baec79accb5bb6bae91ea5e7ef737ed8dd378cccea01b3f20 |
| dependencies/glm/glm/detail/type_vec3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_vec3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_vec3.hpp<br>framework/dependencies/glm/glm/detail/type_vec3.hpp | c87dede89a8dbb39688d580b8cfcbda18abe73e8b2e07119b2f4168c5d9827b2 |
| dependencies/glm/glm/detail/type_vec3.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_vec3.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_vec3.inl<br>framework/dependencies/glm/glm/detail/type_vec3.inl | 0e1033167c913bf20c8254ef32fc26bf74becd7f7f343304a3b1d2df29b737e3 |
| dependencies/glm/glm/detail/type_vec4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_vec4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_vec4.hpp<br>framework/dependencies/glm/glm/detail/type_vec4.hpp | 6804a80268f0de2cfeb8b34c569d7b2a5c9d20afb9abcafe81bdb2670da165bd |
| dependencies/glm/glm/detail/type_vec4.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_vec4.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_vec4.inl<br>framework/dependencies/glm/glm/detail/type_vec4.inl | 6f1b350f32078a5d08208a129b633fef24f3d784c68555dd71f3d38a4292bc4a |
| dependencies/glm/glm/detail/type_vec_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/detail/type_vec_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/detail/type_vec_simd.inl<br>framework/dependencies/glm/glm/detail/type_vec_simd.inl | 269e9e8a5862599c08089c9adcb93d060dd23a0cb4502d157af447991957dec1 |
| dependencies/glm/glm/exponential.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/exponential.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/exponential.hpp<br>framework/dependencies/glm/glm/exponential.hpp | f99fe4662b828fb40b0d82dbaeea5745f6d85ee7358af1f0fa00dde05de70d40 |
| dependencies/glm/glm/ext/_matrix_vectorize.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/_matrix_vectorize.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/_matrix_vectorize.hpp<br>framework/dependencies/glm/glm/ext/_matrix_vectorize.hpp | d47f45c0772ae134d7b125c3b8fbfb1f736e278774fc06542b72ee75ff62cf28 |
| dependencies/glm/glm/ext/matrix_clip_space.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_clip_space.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_clip_space.hpp<br>framework/dependencies/glm/glm/ext/matrix_clip_space.hpp | 4e304202722baac68c98f76461b6eddc7cc908c5b494b8c6d043d49a4b822368 |
| dependencies/glm/glm/ext/matrix_clip_space.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_clip_space.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_clip_space.inl<br>framework/dependencies/glm/glm/ext/matrix_clip_space.inl | 7fa1c76868e9bebe00802ab29492d6c4d2f1977ef9ef7a491e8c9883d611b6e8 |
| dependencies/glm/glm/ext/matrix_common.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_common.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_common.hpp<br>framework/dependencies/glm/glm/ext/matrix_common.hpp | ff738dde368720033d72ec69c8eaac5edac4b117af39eba265762f6abe5e7bb0 |
| dependencies/glm/glm/ext/matrix_common.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_common.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_common.inl<br>framework/dependencies/glm/glm/ext/matrix_common.inl | 83989be3cc04bd8bf58c8dffbf50059ff89c919a35a5d901d7b1136786fee04d |
| dependencies/glm/glm/ext/matrix_double2x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double2x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double2x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_double2x2.hpp | 8988aae2320d0ef8f0241545eb13e7a5648f434f5361a7e0a9bb35965a41701e |
| dependencies/glm/glm/ext/matrix_double2x2_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double2x2_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double2x2_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_double2x2_precision.hpp | d30ec80b9397ae666807c3ddc63a624de95ec7bd6a3d638cba4abdc36cea3d9e |
| dependencies/glm/glm/ext/matrix_double2x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double2x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double2x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_double2x3.hpp | 0249cdd96d25361f1f2e79de3f3d2a68e5e835b2fbbfcc7295fb0ebe3d94f618 |
| dependencies/glm/glm/ext/matrix_double2x3_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double2x3_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double2x3_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_double2x3_precision.hpp | 984659c78e14736235014216bf7a6f921e31cc4e674cebbcc42e8c024c63813c |
| dependencies/glm/glm/ext/matrix_double2x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double2x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double2x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_double2x4.hpp | 399e591e95a51c0ff287a76270035fe0038ccc93b6288a43568391f9e8c35272 |
| dependencies/glm/glm/ext/matrix_double2x4_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double2x4_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double2x4_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_double2x4_precision.hpp | 734d6eb388aa0360209173b481af00af3e8cff8cc0db17bc5f4ff647baccacdb |
| dependencies/glm/glm/ext/matrix_double3x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double3x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double3x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_double3x2.hpp | 3631794438a7042c6a849c0fa0b85ab0eb6dc36a2eaff23c31c30031d6f9c742 |
| dependencies/glm/glm/ext/matrix_double3x2_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double3x2_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double3x2_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_double3x2_precision.hpp | 18bff31b2d832d1f685f738446779f69cede287ec01375af434976586b3233c3 |
| dependencies/glm/glm/ext/matrix_double3x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double3x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double3x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_double3x3.hpp | 524d18afb1f977ad4da6a1a3cf7d4a7a408878e04a5c072740804bfb1aa1af0c |
| dependencies/glm/glm/ext/matrix_double3x3_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double3x3_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double3x3_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_double3x3_precision.hpp | 52ad25d2c39fa2d1829a024cf1285e4dbc4af0e20636563e13e2491aa484ce72 |
| dependencies/glm/glm/ext/matrix_double3x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double3x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double3x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_double3x4.hpp | d310d2f564f69576a8492841be71815bd90f3dead089d448227dd7f925e821ff |
| dependencies/glm/glm/ext/matrix_double3x4_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double3x4_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double3x4_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_double3x4_precision.hpp | 7c2de092da7dddc03c5c40dbea83d0b9c4ba2b394eb5e3514da691ef35f39d6b |
| dependencies/glm/glm/ext/matrix_double4x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double4x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double4x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_double4x2.hpp | ffc3fe35dd8e8ab655ccb826621e769cc9dd5c138ef251f6ed33c45e4b9fb435 |
| dependencies/glm/glm/ext/matrix_double4x2_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double4x2_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double4x2_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_double4x2_precision.hpp | 8a732fe4f2f8820a688e5c6b36c61678565d432139ef610ad193531863437e2a |
| dependencies/glm/glm/ext/matrix_double4x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double4x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double4x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_double4x3.hpp | 12a9e5cbfba247e7c719e58f4d6d2e37322a245606e294935c352f4c244b02d6 |
| dependencies/glm/glm/ext/matrix_double4x3_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double4x3_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double4x3_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_double4x3_precision.hpp | c02846c09ab692fd77dc82c9b929b2f7a2817a6860b98035f0c376c82528a2ea |
| dependencies/glm/glm/ext/matrix_double4x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double4x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double4x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_double4x4.hpp | 35f2047fe26f495443c74218fbdc1b404a9bdb14cbd876113929377794255864 |
| dependencies/glm/glm/ext/matrix_double4x4_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_double4x4_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_double4x4_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_double4x4_precision.hpp | 51d5680c95c49f96c04b37829bfff215dd626b3eba495f2efb64eaab3e0435f7 |
| dependencies/glm/glm/ext/matrix_float2x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float2x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float2x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_float2x2.hpp | 841936453083c65220e826de9eeca359f4519eefc7769671cc99906c9a8aede8 |
| dependencies/glm/glm/ext/matrix_float2x2_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float2x2_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float2x2_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_float2x2_precision.hpp | 6ca2a15c6e71ad30b64bc2bc4546858d12c2305a60424eeffc35e906cc3fd4ac |
| dependencies/glm/glm/ext/matrix_float2x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float2x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float2x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_float2x3.hpp | 6dae07ddd823bea70f59c1f7b74cabd44b8796e05bc80ae5f9f4a54fd36561c6 |
| dependencies/glm/glm/ext/matrix_float2x3_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float2x3_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float2x3_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_float2x3_precision.hpp | c3947ce6c001ed94bbf124d11f403c39825d7dd51f4963343f8a04da1f0b655d |
| dependencies/glm/glm/ext/matrix_float2x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float2x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float2x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_float2x4.hpp | 9dd80a13352ae4f12e29c22bef63bd09803bfa4f1f5f4843c4ec87a8d585f5fd |
| dependencies/glm/glm/ext/matrix_float2x4_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float2x4_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float2x4_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_float2x4_precision.hpp | 587470650fad9c426a74fae76e12ca9b339e1246f1604a7ca96c7c32b3d3d362 |
| dependencies/glm/glm/ext/matrix_float3x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float3x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float3x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_float3x2.hpp | 6f9ed2d1b89187032d12da29c3953831293e92e4c3f8879457cde25bda5ce108 |
| dependencies/glm/glm/ext/matrix_float3x2_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float3x2_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float3x2_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_float3x2_precision.hpp | f54ebc067b1528c4439bd56244a17e9f57ed90f6d98b97b47148f4a19be430a6 |
| dependencies/glm/glm/ext/matrix_float3x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float3x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float3x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_float3x3.hpp | cdf6291801c42f465ba0fcfd07bddd5235be87fb919a4ea6250c11cda22422f8 |
| dependencies/glm/glm/ext/matrix_float3x3_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float3x3_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float3x3_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_float3x3_precision.hpp | ee880ab0a3c60ad13d1493dcd75b0d26899a76277b27d9592f0b3e858924cb24 |
| dependencies/glm/glm/ext/matrix_float3x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float3x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float3x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_float3x4.hpp | 4ac3173d621582453abab434303a475b6dd5251d1ca6ae8810cdf00345ac41e3 |
| dependencies/glm/glm/ext/matrix_float3x4_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float3x4_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float3x4_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_float3x4_precision.hpp | 5b8f8a5eff4c138486d7ea4b0f447a47e5d693e9f7781082401e58029403dccc |
| dependencies/glm/glm/ext/matrix_float4x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float4x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float4x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_float4x2.hpp | 6c0c92f24889e80251b6db6ffaf3efcefddab75d34355356db349322f2ba1ac7 |
| dependencies/glm/glm/ext/matrix_float4x2_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float4x2_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float4x2_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_float4x2_precision.hpp | bb2499e042f10111a2cb82cd5eb7c5b124e2883310159b32d12963f5669af3c0 |
| dependencies/glm/glm/ext/matrix_float4x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float4x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float4x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_float4x3.hpp | f312a05ce56f83f4ae030fc05b00bc6be2755b880a20340ba9dbb77cd0855921 |
| dependencies/glm/glm/ext/matrix_float4x3_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float4x3_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float4x3_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_float4x3_precision.hpp | ec7be67780fe92c5226137ab2efa38042abf341cee1ba1e506bd9a214055f10a |
| dependencies/glm/glm/ext/matrix_float4x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float4x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float4x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_float4x4.hpp | df618841315d9cb6027d9b49537206719e9323332ad31273254c48b348b8527c |
| dependencies/glm/glm/ext/matrix_float4x4_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_float4x4_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_float4x4_precision.hpp<br>framework/dependencies/glm/glm/ext/matrix_float4x4_precision.hpp | 944dfc764179cf8d9faced4215b75d603d875f837be4ac7a6aa356233abfe842 |
| dependencies/glm/glm/ext/matrix_int2x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int2x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int2x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_int2x2.hpp | a620147e0f9124b3038b8b01c03805cb1b5757f812e03eae07fda3873a2b8978 |
| dependencies/glm/glm/ext/matrix_int2x2_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int2x2_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int2x2_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_int2x2_sized.hpp | bfc3abaca841ccb6cfde266fbf9a774b274bb6b435ac68611d5c9c8518cd94d7 |
| dependencies/glm/glm/ext/matrix_int2x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int2x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int2x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_int2x3.hpp | 4d74abaeb3d02348a1ab42af58c4656a21e9b796c725ec2807dc710af5594bc4 |
| dependencies/glm/glm/ext/matrix_int2x3_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int2x3_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int2x3_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_int2x3_sized.hpp | f70854e696b1b2a95e1125499140ca8867d7da2f8fd3564874d780337bcd1a40 |
| dependencies/glm/glm/ext/matrix_int2x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int2x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int2x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_int2x4.hpp | f667fb414097e7ddf2fd7f722056e3e21afb8d21f20ab75898e820f5b16a56be |
| dependencies/glm/glm/ext/matrix_int2x4_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int2x4_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int2x4_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_int2x4_sized.hpp | e28652fd0fd8c49eb565a07885f277daa494b6e978526d7f2d65cae49656c91e |
| dependencies/glm/glm/ext/matrix_int3x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int3x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int3x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_int3x2.hpp | bff626c579914f8f1af96762eac40ce549652ad158f978c02eaba602fe6c04b2 |
| dependencies/glm/glm/ext/matrix_int3x2_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int3x2_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int3x2_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_int3x2_sized.hpp | 2013364a7a1dc87bf9dfff9cfe9a640a77a24d81d253040db54c1aa1b0643ffd |
| dependencies/glm/glm/ext/matrix_int3x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int3x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int3x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_int3x3.hpp | 87d02d438d5c506d2632fa80b62957447c41990b4600396ea7fa6c6075c4a17f |
| dependencies/glm/glm/ext/matrix_int3x3_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int3x3_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int3x3_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_int3x3_sized.hpp | d5505b8f0e34a00057c77024699adbae5f17efaafe434bc52552c12daed9e3a5 |
| dependencies/glm/glm/ext/matrix_int3x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int3x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int3x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_int3x4.hpp | c9e8e6a204e085dd4e8c4ba7885562e6cfcdb9eda5641d004530902aa58b7982 |
| dependencies/glm/glm/ext/matrix_int3x4_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int3x4_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int3x4_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_int3x4_sized.hpp | f56444a1efb44b45cd6ad44f9ee2479b05d7a42a5613dda316043d5b9bbe31f2 |
| dependencies/glm/glm/ext/matrix_int4x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int4x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int4x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_int4x2.hpp | 30b615dddcff99eeb9afc34740610d751c196a847fe146c77694b3d4c65fb64c |
| dependencies/glm/glm/ext/matrix_int4x2_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int4x2_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int4x2_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_int4x2_sized.hpp | 8544d801d4d4f4632dace8eebbf224836cd9ea0c9f00bd398a02d17d48eac2d1 |
| dependencies/glm/glm/ext/matrix_int4x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int4x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int4x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_int4x3.hpp | 7b2108328b78bac9a9f919660918b81cff4f6e04ccc8c2504ad1e0dba6d7ce7b |
| dependencies/glm/glm/ext/matrix_int4x3_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int4x3_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int4x3_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_int4x3_sized.hpp | a41fce94c7a9df4d128826ee84531ae987c4be9b7e75ca1e3ee5400e4d4045dd |
| dependencies/glm/glm/ext/matrix_int4x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int4x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int4x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_int4x4.hpp | 20619f8dc7b01a378fd1a84f949b64286ee658fc9bc1bd410c3d9ff8e094a45a |
| dependencies/glm/glm/ext/matrix_int4x4_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_int4x4_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_int4x4_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_int4x4_sized.hpp | 340140a14b834fee2fd8ea22bf96e26fa1901522ecaf719807e6feeab4d4deca |
| dependencies/glm/glm/ext/matrix_integer.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_integer.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_integer.hpp<br>framework/dependencies/glm/glm/ext/matrix_integer.hpp | 9ef29c5f2dff02aadd85a12bd008102e71c96df013cbccae536c1ef2a02e0859 |
| dependencies/glm/glm/ext/matrix_integer.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_integer.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_integer.inl<br>framework/dependencies/glm/glm/ext/matrix_integer.inl | ed5e4698b532e03e9b3a149bd256c30f26ba9e5464a1057f8cdc394660578b01 |
| dependencies/glm/glm/ext/matrix_projection.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_projection.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_projection.hpp<br>framework/dependencies/glm/glm/ext/matrix_projection.hpp | 5da44fb43797d760397f0b0c282e982adf1f070b655ef380ac0f7223959e1203 |
| dependencies/glm/glm/ext/matrix_projection.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_projection.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_projection.inl<br>framework/dependencies/glm/glm/ext/matrix_projection.inl | bb0a942e7219ae7083ba196bb6a95cb3fe096162a332f5489a715508dfd8e0a1 |
| dependencies/glm/glm/ext/matrix_relational.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_relational.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_relational.hpp<br>framework/dependencies/glm/glm/ext/matrix_relational.hpp | 5d157ffea4189ebc5a7d7f9d1d4efe426d76ecfbb1df1ccd7216e6639356721c |
| dependencies/glm/glm/ext/matrix_relational.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_relational.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_relational.inl<br>framework/dependencies/glm/glm/ext/matrix_relational.inl | 9dc556c33e7f6eeee4cac3f72af288f9642ebb0a515f84cf2785b0b7e0e88d82 |
| dependencies/glm/glm/ext/matrix_transform.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_transform.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_transform.hpp<br>framework/dependencies/glm/glm/ext/matrix_transform.hpp | e95ec6ad6d6e403fc3d87fb5c769ad78fd5bfdf3f440fde8165f7a5578534595 |
| dependencies/glm/glm/ext/matrix_transform.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_transform.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_transform.inl<br>framework/dependencies/glm/glm/ext/matrix_transform.inl | 7a90941a5079340f903f8b6c19ac3d8dbc4e5ab82f4feb18e9d3d495a8c2316c |
| dependencies/glm/glm/ext/matrix_uint2x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint2x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint2x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint2x2.hpp | 872ebf8d37765e29fc048ea24c1ec087ce90ecf52075fe00d513e6e421ad1992 |
| dependencies/glm/glm/ext/matrix_uint2x2_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint2x2_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint2x2_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint2x2_sized.hpp | 54704646ed5e103a86055c3c503690ea2f7c22fcf56f09326152946973efc9de |
| dependencies/glm/glm/ext/matrix_uint2x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint2x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint2x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint2x3.hpp | 2750219cf54b2f06983af4b7d2bd7f7020bddf61291fcbe89a56cd51b42ed5ae |
| dependencies/glm/glm/ext/matrix_uint2x3_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint2x3_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint2x3_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint2x3_sized.hpp | 637ac63599d715801dedfd0f3dc6f78f8c18569792ae5525719725690ac7e512 |
| dependencies/glm/glm/ext/matrix_uint2x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint2x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint2x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint2x4.hpp | 5866045e6c781649c96760a8fd6f615941f8be34ba89ae544fd2904e6e7ecaa5 |
| dependencies/glm/glm/ext/matrix_uint2x4_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint2x4_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint2x4_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint2x4_sized.hpp | 204c9a207e17b416cab76d46727edf57432a38f54e49b1987d8385128b8b1610 |
| dependencies/glm/glm/ext/matrix_uint3x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint3x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint3x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint3x2.hpp | 2bacd8d27fde66217ba7d7e249f4fa0f45a96e00fa8928fd864b7b6923e304ad |
| dependencies/glm/glm/ext/matrix_uint3x2_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint3x2_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint3x2_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint3x2_sized.hpp | 4d73e62033ac02d65b16343384dbcf6b338bd0be167d07e17b16587e2e6e8a10 |
| dependencies/glm/glm/ext/matrix_uint3x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint3x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint3x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint3x3.hpp | 5cf424210dddc3c92608df44c68d6faf7c8c9cf4b76d1051f90654ebc4d3a926 |
| dependencies/glm/glm/ext/matrix_uint3x3_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint3x3_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint3x3_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint3x3_sized.hpp | 68d0dd82b154ed3ccb1e2b7a9fddaa40ec748c1c18bbb6546f15f97d324c8a73 |
| dependencies/glm/glm/ext/matrix_uint3x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint3x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint3x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint3x4.hpp | 090fab8a63aeee49aba46a04bbbce100b6e48a0dbbbf535554ff6d9e77a9f6db |
| dependencies/glm/glm/ext/matrix_uint3x4_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint3x4_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint3x4_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint3x4_sized.hpp | 4c2b34a932e0ff1c7404b9b44a10829fd0b51d4b785594b1acb6d4d5a35ecd85 |
| dependencies/glm/glm/ext/matrix_uint4x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint4x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint4x2.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint4x2.hpp | aa2171132e5da762562ac2f25c2e1969e966c0a1df380886d7bca8623363b728 |
| dependencies/glm/glm/ext/matrix_uint4x2_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint4x2_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint4x2_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint4x2_sized.hpp | 1884239d1ce81d55fc84663caab98bd4a4142c636b0e605775599b419004768d |
| dependencies/glm/glm/ext/matrix_uint4x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint4x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint4x3.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint4x3.hpp | 1b73e12c2c03f5f15e25dd9894a2e0b9bcc8006bfb202a872df53dcdc3a95bb5 |
| dependencies/glm/glm/ext/matrix_uint4x3_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint4x3_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint4x3_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint4x3_sized.hpp | 4f7d80da919973d9b7837108cd1e1edfac1829e60895cb5687e0554dee33e8a9 |
| dependencies/glm/glm/ext/matrix_uint4x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint4x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint4x4.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint4x4.hpp | f6123f7e7ec862b2f276212ce1cb45cdd9bee37ba0e8c2db2ea90f3739ba5e93 |
| dependencies/glm/glm/ext/matrix_uint4x4_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/matrix_uint4x4_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/matrix_uint4x4_sized.hpp<br>framework/dependencies/glm/glm/ext/matrix_uint4x4_sized.hpp | ffe916f61d78dec350673e9e97506df77395f5bc69905901daacce05efc6e1bb |
| dependencies/glm/glm/ext/quaternion_common.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_common.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_common.hpp<br>framework/dependencies/glm/glm/ext/quaternion_common.hpp | 1fb9fc6207c850b593c8a51a776584cd08f1727157f7790b935d3f9a0c898d61 |
| dependencies/glm/glm/ext/quaternion_common.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_common.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_common.inl<br>framework/dependencies/glm/glm/ext/quaternion_common.inl | cbcf7fe43a66ca691f044d7a7249db5dd7e76d4d3b1dd671b60758b2db1f1915 |
| dependencies/glm/glm/ext/quaternion_common_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_common_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_common_simd.inl<br>framework/dependencies/glm/glm/ext/quaternion_common_simd.inl | 16f63f092b0f80d594d28840cc9c3bae7720323606518d5a5eaa19f541284e5c |
| dependencies/glm/glm/ext/quaternion_double.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_double.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_double.hpp<br>framework/dependencies/glm/glm/ext/quaternion_double.hpp | 12954acc5a09e387c182961c1b1377bc881aba7458933f9b5fbae7cc2c28e8ca |
| dependencies/glm/glm/ext/quaternion_double_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_double_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_double_precision.hpp<br>framework/dependencies/glm/glm/ext/quaternion_double_precision.hpp | 24f6e7732fd93f9fa87d8cd8326e2f687758cd1c55cbf598b5b6ed975be58b6b |
| dependencies/glm/glm/ext/quaternion_exponential.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_exponential.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_exponential.hpp<br>framework/dependencies/glm/glm/ext/quaternion_exponential.hpp | eadb3c5cc99be4ccafc32de336c117af139d472651364a94adebf04480a7d5cb |
| dependencies/glm/glm/ext/quaternion_exponential.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_exponential.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_exponential.inl<br>framework/dependencies/glm/glm/ext/quaternion_exponential.inl | 5f156104bdcb6611db7343f3c570fbbef65a18d32cc1747cd2b31a19533bb8a9 |
| dependencies/glm/glm/ext/quaternion_float.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_float.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_float.hpp<br>framework/dependencies/glm/glm/ext/quaternion_float.hpp | 4da67234432fb03d656e0d98ca625245a344d314bc1132d85d40fd7b39f2f7ca |
| dependencies/glm/glm/ext/quaternion_float_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_float_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_float_precision.hpp<br>framework/dependencies/glm/glm/ext/quaternion_float_precision.hpp | 3b923827b8c728df9d901f05f7c6a573122a9e8f047727f174ed0a97dbccf7ac |
| dependencies/glm/glm/ext/quaternion_geometric.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_geometric.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_geometric.hpp<br>framework/dependencies/glm/glm/ext/quaternion_geometric.hpp | d5c3823f9e3fbc079f4abffa1e099f2f20dd61e6fd1d7bd347ce13185a754f4a |
| dependencies/glm/glm/ext/quaternion_geometric.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_geometric.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_geometric.inl<br>framework/dependencies/glm/glm/ext/quaternion_geometric.inl | e2be0c800cd2a635c26e66747556990628b132336aa2e2769672ba2558a5dbe2 |
| dependencies/glm/glm/ext/quaternion_relational.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_relational.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_relational.hpp<br>framework/dependencies/glm/glm/ext/quaternion_relational.hpp | d60b00fa64320affda373f34e8f76801414a120ad6371d190e2f89f3b3c14dcf |
| dependencies/glm/glm/ext/quaternion_relational.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_relational.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_relational.inl<br>framework/dependencies/glm/glm/ext/quaternion_relational.inl | 6c2ec05dd24cdf1a566c973b41aeb9d9931ffc02f849e515acd40c28a972c7a8 |
| dependencies/glm/glm/ext/quaternion_transform.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_transform.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_transform.hpp<br>framework/dependencies/glm/glm/ext/quaternion_transform.hpp | 5ea9afad1a212cad00de121a6c50ccd4e4b37546b68439ba436fdc09b274fbda |
| dependencies/glm/glm/ext/quaternion_transform.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_transform.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_transform.inl<br>framework/dependencies/glm/glm/ext/quaternion_transform.inl | 56b6eabbd2b08fd1bd5400e2c7455ec77ae927376801db92b4f2c6b3fc3ed99e |
| dependencies/glm/glm/ext/quaternion_trigonometric.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_trigonometric.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_trigonometric.hpp<br>framework/dependencies/glm/glm/ext/quaternion_trigonometric.hpp | 4ad10d61b77f92f1347aac5855b7bc76111220132440f80f8e84435d25976f66 |
| dependencies/glm/glm/ext/quaternion_trigonometric.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/quaternion_trigonometric.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/quaternion_trigonometric.inl<br>framework/dependencies/glm/glm/ext/quaternion_trigonometric.inl | c2e8992300392f5c63cb107ec04b1a073df8f6aa1a43cab6dd809740dcdb5b95 |
| dependencies/glm/glm/ext/scalar_common.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_common.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_common.hpp<br>framework/dependencies/glm/glm/ext/scalar_common.hpp | 89e6c98ad49b61cae2d70c841a110f5c30d2ee802983a070d4e8be2bf070aab2 |
| dependencies/glm/glm/ext/scalar_common.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_common.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_common.inl<br>framework/dependencies/glm/glm/ext/scalar_common.inl | 8202367aa7c4f20b48fa0281e77b28424d55966f3a78f08e1663a6a3435c8be3 |
| dependencies/glm/glm/ext/scalar_constants.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_constants.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_constants.hpp<br>framework/dependencies/glm/glm/ext/scalar_constants.hpp | bd1cea6ec26b60549f6a982e4178e53a93b8b0659c258167e4022b50b64bb83a |
| dependencies/glm/glm/ext/scalar_constants.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_constants.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_constants.inl<br>framework/dependencies/glm/glm/ext/scalar_constants.inl | 244633feae7f575b4c96c6e245b7cd1aa94a29c0911fd6c99b54de29295a09be |
| dependencies/glm/glm/ext/scalar_int_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_int_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_int_sized.hpp<br>framework/dependencies/glm/glm/ext/scalar_int_sized.hpp | fa4d9e88289df1c89fe98bca956489d3f55c1aadc20b3b78f3e67add678c741c |
| dependencies/glm/glm/ext/scalar_integer.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_integer.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_integer.hpp<br>framework/dependencies/glm/glm/ext/scalar_integer.hpp | 79ea6f569f61dd5e8dd5b5452be1b4c9261afc86d92247ff1632423617c355f9 |
| dependencies/glm/glm/ext/scalar_integer.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_integer.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_integer.inl<br>framework/dependencies/glm/glm/ext/scalar_integer.inl | 2888ba2a348ba38e3d52e5f5c59de47ea0a20e7c2acd281f3b2f80b02f2fbdb1 |
| dependencies/glm/glm/ext/scalar_packing.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_packing.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_packing.hpp<br>framework/dependencies/glm/glm/ext/scalar_packing.hpp | 0872250cfc4ea74705ee974d6fded5acfd71fdc1796fd418c9a5691195fa6c70 |
| dependencies/glm/glm/ext/scalar_packing.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_packing.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_packing.inl<br>framework/dependencies/glm/glm/ext/scalar_packing.inl | f1945cd6c19e56b3c1c78943ef5ec18116907a4ca1efc40a57d48ab1db7adfc5 |
| dependencies/glm/glm/ext/scalar_reciprocal.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_reciprocal.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_reciprocal.hpp<br>framework/dependencies/glm/glm/ext/scalar_reciprocal.hpp | a71422a9b7349fc3d66ae49153fdac2233b451f7c925fd4e85c765388ef47a4c |
| dependencies/glm/glm/ext/scalar_reciprocal.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_reciprocal.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_reciprocal.inl<br>framework/dependencies/glm/glm/ext/scalar_reciprocal.inl | 2c8569ac70e4848ab43250007489111e131169a6e32de8db0c69de5ed4e67177 |
| dependencies/glm/glm/ext/scalar_relational.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_relational.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_relational.hpp<br>framework/dependencies/glm/glm/ext/scalar_relational.hpp | 8438969d52284e5e74e44b630129a4ab28636ca95124e9e5f52cd7496363f896 |
| dependencies/glm/glm/ext/scalar_relational.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_relational.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_relational.inl<br>framework/dependencies/glm/glm/ext/scalar_relational.inl | 7f2bbbfeefac15b3aa431cf12c924c36c362d4a5390e3bcbc915188efd071b54 |
| dependencies/glm/glm/ext/scalar_uint_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_uint_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_uint_sized.hpp<br>framework/dependencies/glm/glm/ext/scalar_uint_sized.hpp | 40be3227bcbae6e41e6de317f1455a76a2d298def19a99209a8d6670f3fa993a |
| dependencies/glm/glm/ext/scalar_ulp.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_ulp.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_ulp.hpp<br>framework/dependencies/glm/glm/ext/scalar_ulp.hpp | f06023e60c8228be85e97b9324619fa451b4737e7d9f01d78d9e96cb7f43c79f |
| dependencies/glm/glm/ext/scalar_ulp.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/scalar_ulp.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/scalar_ulp.inl<br>framework/dependencies/glm/glm/ext/scalar_ulp.inl | 6df0296f088293af2876483584645085aeb84a444fc425c92b540142f7a4c4b6 |
| dependencies/glm/glm/ext/vector_bool1.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_bool1.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_bool1.hpp<br>framework/dependencies/glm/glm/ext/vector_bool1.hpp | 4e66b1635bebb66ad36e9666f01fa79fb2fbb66e0df77d53c95eee00deac9a10 |
| dependencies/glm/glm/ext/vector_bool1_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_bool1_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_bool1_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_bool1_precision.hpp | 6d648e99f532b92099f44b5a7059361080b312e878c7b425de1c8502d7e323f6 |
| dependencies/glm/glm/ext/vector_bool2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_bool2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_bool2.hpp<br>framework/dependencies/glm/glm/ext/vector_bool2.hpp | 4aa38230cb6e0d7da105e2f89567cce9960af50bdd072e60e8ba52c88ff86305 |
| dependencies/glm/glm/ext/vector_bool2_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_bool2_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_bool2_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_bool2_precision.hpp | 028f7315009d69facc70cf14d2582a9005239df2b0c22d13ee8bcb1d2680581c |
| dependencies/glm/glm/ext/vector_bool3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_bool3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_bool3.hpp<br>framework/dependencies/glm/glm/ext/vector_bool3.hpp | c693f619905f83ee8e16aa776ae86c8c737039ab05292a9430fe1dde74a5a140 |
| dependencies/glm/glm/ext/vector_bool3_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_bool3_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_bool3_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_bool3_precision.hpp | 2f02eb6ce170ab9a17e2f52af4c2bf3ae4ab2640f34f85b28223c6bf7e331ea9 |
| dependencies/glm/glm/ext/vector_bool4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_bool4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_bool4.hpp<br>framework/dependencies/glm/glm/ext/vector_bool4.hpp | 4a64578845386c0c91698ab8b5f4b5d09baea94ab3a53e2b869da41f4afb4dee |
| dependencies/glm/glm/ext/vector_bool4_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_bool4_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_bool4_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_bool4_precision.hpp | 614c4e94957edd22503e557687d56bbf4490029deff6a5a3933e95fd1019baee |
| dependencies/glm/glm/ext/vector_common.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_common.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_common.hpp<br>framework/dependencies/glm/glm/ext/vector_common.hpp | c1a6eb0bdbf3609ce2f3481449bbcdea43bc8f91af28470f21d716a12e3d7fe8 |
| dependencies/glm/glm/ext/vector_common.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_common.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_common.inl<br>framework/dependencies/glm/glm/ext/vector_common.inl | 52af3d66fda5958946e046dad3ee1cb52dfb6f84e5c36a8720f155f14bc4236b |
| dependencies/glm/glm/ext/vector_double1.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_double1.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_double1.hpp<br>framework/dependencies/glm/glm/ext/vector_double1.hpp | c10b120ee00c980d924f2ab5979c1368033369fffe103464297499b8120ee102 |
| dependencies/glm/glm/ext/vector_double1_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_double1_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_double1_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_double1_precision.hpp | 8f8246a4b9b0bb1f76e59b6e14b021cbdace56304710ae37d735359c3ae367f8 |
| dependencies/glm/glm/ext/vector_double2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_double2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_double2.hpp<br>framework/dependencies/glm/glm/ext/vector_double2.hpp | baf54247150a183acb161a4c905b01736e6f01313b438a4725d8d72da2c29e67 |
| dependencies/glm/glm/ext/vector_double2_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_double2_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_double2_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_double2_precision.hpp | 12370946f9ab21ec2cce2d9f226d74aa515c0b9e48b567b7daea370f6c4223fc |
| dependencies/glm/glm/ext/vector_double3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_double3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_double3.hpp<br>framework/dependencies/glm/glm/ext/vector_double3.hpp | e3299c9b32f98d3a8a63686101b18e25292f6a157f557e23e386472d69a71c29 |
| dependencies/glm/glm/ext/vector_double3_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_double3_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_double3_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_double3_precision.hpp | 1355cf513265fa8717c219fc6f3dfd22bed2546b1a201b0b85a03af5509b97ca |
| dependencies/glm/glm/ext/vector_double4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_double4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_double4.hpp<br>framework/dependencies/glm/glm/ext/vector_double4.hpp | 49ff2cb0b45e88f975cdcdfe8622b1663a4024a1d50cb1167a3c8646d18f204d |
| dependencies/glm/glm/ext/vector_double4_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_double4_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_double4_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_double4_precision.hpp | e943fba185e8b191b691223cd8bec4b92d2cbe909dc1d2f4f838da2565163735 |
| dependencies/glm/glm/ext/vector_float1.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_float1.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_float1.hpp<br>framework/dependencies/glm/glm/ext/vector_float1.hpp | 6f4c821805c967856ada823c555efaa9b8e2fe037a4f60691b5ec922fc9109bb |
| dependencies/glm/glm/ext/vector_float1_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_float1_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_float1_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_float1_precision.hpp | 1c7b780b9895f72caab184f21498626f56f4776d87b2279945b923c915bddba2 |
| dependencies/glm/glm/ext/vector_float2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_float2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_float2.hpp<br>framework/dependencies/glm/glm/ext/vector_float2.hpp | 5c3746bf09b27d0e4444b17e0031b7f375a67b001ddda19134e08bc97e75e690 |
| dependencies/glm/glm/ext/vector_float2_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_float2_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_float2_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_float2_precision.hpp | a5dafa12f4b870c072c538e8145093f97d4b2f5f744ce93836cac2b3ac428774 |
| dependencies/glm/glm/ext/vector_float3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_float3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_float3.hpp<br>framework/dependencies/glm/glm/ext/vector_float3.hpp | d5fb00ea964bafd7a52cc491fa7d968bd58cbdbdb6065e1663879b56f4200a85 |
| dependencies/glm/glm/ext/vector_float3_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_float3_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_float3_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_float3_precision.hpp | 048bfcd179ef54535fea651f4813c826812b02ddc77449da18b361a511745778 |
| dependencies/glm/glm/ext/vector_float4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_float4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_float4.hpp<br>framework/dependencies/glm/glm/ext/vector_float4.hpp | 3a8ef67ddcc7dc30ddde9a0609d6954ed025db1ad578f754c399e3ab98a97359 |
| dependencies/glm/glm/ext/vector_float4_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_float4_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_float4_precision.hpp<br>framework/dependencies/glm/glm/ext/vector_float4_precision.hpp | e3d2401739a80b5ab05244bea13f3f8f0d69db54c293b6d3726a55234742f903 |
| dependencies/glm/glm/ext/vector_int1.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_int1.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_int1.hpp<br>framework/dependencies/glm/glm/ext/vector_int1.hpp | b9b927b0f6a2fb9cb3f87df7e986e2e3e125850b302417e4b97aec450be40d73 |
| dependencies/glm/glm/ext/vector_int1_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_int1_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_int1_sized.hpp<br>framework/dependencies/glm/glm/ext/vector_int1_sized.hpp | fcfe212a8eeac8b26d6a89fe7f22cd7a8fb4d7edf0b4cd886583a86afc489811 |
| dependencies/glm/glm/ext/vector_int2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_int2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_int2.hpp<br>framework/dependencies/glm/glm/ext/vector_int2.hpp | 672675ee53c6b56e6512db4f7094349555ae05a38be135c08360255ed555d578 |
| dependencies/glm/glm/ext/vector_int2_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_int2_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_int2_sized.hpp<br>framework/dependencies/glm/glm/ext/vector_int2_sized.hpp | 4c5188a656f0f38d76d98d7986c5ec305d3f9f3d921cac8baa756123746e26e3 |
| dependencies/glm/glm/ext/vector_int3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_int3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_int3.hpp<br>framework/dependencies/glm/glm/ext/vector_int3.hpp | 2565ece98d1f17928c353935f0bcf480b4be9db70720b1f13e0a54a022ae83c5 |
| dependencies/glm/glm/ext/vector_int3_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_int3_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_int3_sized.hpp<br>framework/dependencies/glm/glm/ext/vector_int3_sized.hpp | 19b06a4e05dd0883762048c5e5f56b8f3e51471aba0ec4f90b9fbbac593fb787 |
| dependencies/glm/glm/ext/vector_int4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_int4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_int4.hpp<br>framework/dependencies/glm/glm/ext/vector_int4.hpp | 0dcf10a6840a6c2652c9ba79df912d2921f0b7f4fe0f037c8aed4e71bc6e2502 |
| dependencies/glm/glm/ext/vector_int4_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_int4_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_int4_sized.hpp<br>framework/dependencies/glm/glm/ext/vector_int4_sized.hpp | b66d6d6a64d5fe38ad6f7a4ef78f083e8881c48489324e2998a8816b8c093cb3 |
| dependencies/glm/glm/ext/vector_integer.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_integer.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_integer.hpp<br>framework/dependencies/glm/glm/ext/vector_integer.hpp | d13628dd89d1bfb51352a418fe9b50531eec9e55b63fda77811301afad3913da |
| dependencies/glm/glm/ext/vector_integer.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_integer.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_integer.inl<br>framework/dependencies/glm/glm/ext/vector_integer.inl | ccd91c197259300d89eb7698d5709b3046db72bb6a2092ab0b4cd2222c92ae14 |
| dependencies/glm/glm/ext/vector_packing.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_packing.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_packing.hpp<br>framework/dependencies/glm/glm/ext/vector_packing.hpp | 4d090c95c9daea82f9aec67f70ac2f25f59d704fdd86df869f072c408748212f |
| dependencies/glm/glm/ext/vector_packing.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_packing.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_packing.inl<br>framework/dependencies/glm/glm/ext/vector_packing.inl | f1945cd6c19e56b3c1c78943ef5ec18116907a4ca1efc40a57d48ab1db7adfc5 |
| dependencies/glm/glm/ext/vector_reciprocal.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_reciprocal.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_reciprocal.hpp<br>framework/dependencies/glm/glm/ext/vector_reciprocal.hpp | a22d5156397e7bebaa60920674e90df6027694afbfd9f4d270acfe92f683d51d |
| dependencies/glm/glm/ext/vector_reciprocal.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_reciprocal.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_reciprocal.inl<br>framework/dependencies/glm/glm/ext/vector_reciprocal.inl | 190710a5e39e8566b4b8dd98ca802bd28dd05c476f90758d0dd9572075e3e2dc |
| dependencies/glm/glm/ext/vector_relational.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_relational.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_relational.hpp<br>framework/dependencies/glm/glm/ext/vector_relational.hpp | 4c5de772115a0474718b8f86542c1a561cbf1df67fd4802fc70f947dbd51440f |
| dependencies/glm/glm/ext/vector_relational.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_relational.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_relational.inl<br>framework/dependencies/glm/glm/ext/vector_relational.inl | c360805dbd2c4de9fcb1ee2aa7b7af16d379fbca5ac615298ea7d3cdf3bc6179 |
| dependencies/glm/glm/ext/vector_uint1.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_uint1.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_uint1.hpp<br>framework/dependencies/glm/glm/ext/vector_uint1.hpp | 88dcd41f191dae33cc29a28137a74b23cd85775f4fbba34bcdb4326e17bd75cb |
| dependencies/glm/glm/ext/vector_uint1_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_uint1_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_uint1_sized.hpp<br>framework/dependencies/glm/glm/ext/vector_uint1_sized.hpp | cd9eaf3225ec83c15a1e07777dc2eafbd8cc58749d018717b9d3718b80b540b2 |
| dependencies/glm/glm/ext/vector_uint2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_uint2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_uint2.hpp<br>framework/dependencies/glm/glm/ext/vector_uint2.hpp | fe84b2317032f3e0f2073c5ad123f8fe0ed13e3990d4dec6046317765d825370 |
| dependencies/glm/glm/ext/vector_uint2_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_uint2_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_uint2_sized.hpp<br>framework/dependencies/glm/glm/ext/vector_uint2_sized.hpp | fa7c2a827e3daa1b755d0013b0469d9f471f5909aa1773b00d102c06b04535b8 |
| dependencies/glm/glm/ext/vector_uint3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_uint3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_uint3.hpp<br>framework/dependencies/glm/glm/ext/vector_uint3.hpp | 1a73a9736b1613b322d11f19d94e48eaf23c0ae35d79a81dd5e321890255381f |
| dependencies/glm/glm/ext/vector_uint3_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_uint3_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_uint3_sized.hpp<br>framework/dependencies/glm/glm/ext/vector_uint3_sized.hpp | ada490ff5df88c558a6eeab144ca276336b1375206e56a03dade776dd63c4dd8 |
| dependencies/glm/glm/ext/vector_uint4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_uint4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_uint4.hpp<br>framework/dependencies/glm/glm/ext/vector_uint4.hpp | dd08786412313d539b83113ccb5c9081942abd8ab2969dc8e1d8b1cc472308e2 |
| dependencies/glm/glm/ext/vector_uint4_sized.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_uint4_sized.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_uint4_sized.hpp<br>framework/dependencies/glm/glm/ext/vector_uint4_sized.hpp | 056290350fee7cf0f122056e4ec365ef36c3d67643b098a4c68f178a9a7a1116 |
| dependencies/glm/glm/ext/vector_ulp.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_ulp.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_ulp.hpp<br>framework/dependencies/glm/glm/ext/vector_ulp.hpp | 4fdbec97530a16028e33f381f00a52bb448e1613387ca882787dd5ce433c871e |
| dependencies/glm/glm/ext/vector_ulp.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/ext/vector_ulp.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext/vector_ulp.inl<br>framework/dependencies/glm/glm/ext/vector_ulp.inl | 2625eb167985054bcaed6670d5054535d34b545c2c0655ce7220d751f864ea64 |
| dependencies/glm/glm/ext.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/ext.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/ext.hpp<br>framework/dependencies/glm/glm/ext.hpp | 9c57ebe0b9636d216ed395d26b5aa38d6ae35aeb9d2be7dbf18af9fd161ea876 |
| dependencies/glm/glm/fwd.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/fwd.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/fwd.hpp<br>framework/dependencies/glm/glm/fwd.hpp | e0a9c3a31d3f1000758e7b6a5a98e3eee07d32e20ba2ec47653d9f15ebf63631 |
| dependencies/glm/glm/geometric.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/geometric.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/geometric.hpp<br>framework/dependencies/glm/glm/geometric.hpp | e60e39fb5d98cbca4135f902461ff76e0eb059de9355684f4d10623cbecbf523 |
| dependencies/glm/glm/glm.cppm | openGL_3dAssign_14/framework/dependencies/glm/glm/glm.cppm<br>openGL_3dAssign_15/framework/dependencies/glm/glm/glm.cppm<br>framework/dependencies/glm/glm/glm.cppm | 512a3eb2b218f405705da07fd244f027efc816104d8caa197a94a4da3de345ab |
| dependencies/glm/glm/glm.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/glm.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/glm.hpp<br>framework/dependencies/glm/glm/glm.hpp | 9bb34dc8dba5e23e74c50943f804873532919b161fecc3e1a9247f9890d498d9 |
| dependencies/glm/glm/gtc/bitfield.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/bitfield.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/bitfield.hpp<br>framework/dependencies/glm/glm/gtc/bitfield.hpp | 944b295cf9c0cf45d3f2baa98680c6ebddb135e7f7ced239d64b71c23d0a7be8 |
| dependencies/glm/glm/gtc/bitfield.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/bitfield.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/bitfield.inl<br>framework/dependencies/glm/glm/gtc/bitfield.inl | 17c33ad1b4459cf024c73a30fca0e48beb020bca6eaeb647967f8f511ec92e37 |
| dependencies/glm/glm/gtc/color_space.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/color_space.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/color_space.hpp<br>framework/dependencies/glm/glm/gtc/color_space.hpp | e125a247aa5165ef8b47ad9f2db5429f28a61d80c96edaf8a73b96995362a987 |
| dependencies/glm/glm/gtc/color_space.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/color_space.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/color_space.inl<br>framework/dependencies/glm/glm/gtc/color_space.inl | 66211e68c6bc090bd971efbd20eae1c0bf9e00eaf53b6d8175a91c8050eaacd6 |
| dependencies/glm/glm/gtc/constants.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/constants.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/constants.hpp<br>framework/dependencies/glm/glm/gtc/constants.hpp | ef2a4934378a306f57d0fc8daf00e210e3ebc6fcda47d4c2a60714062ea82f23 |
| dependencies/glm/glm/gtc/constants.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/constants.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/constants.inl<br>framework/dependencies/glm/glm/gtc/constants.inl | b6da0725b14830815f4839f8bbcb425d8695be173e058fadc56f74907610ebec |
| dependencies/glm/glm/gtc/epsilon.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/epsilon.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/epsilon.hpp<br>framework/dependencies/glm/glm/gtc/epsilon.hpp | f258d822295ba7e5c3a55da3da06ab59af8805567f0fd3e1dfb33139734e8477 |
| dependencies/glm/glm/gtc/epsilon.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/epsilon.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/epsilon.inl<br>framework/dependencies/glm/glm/gtc/epsilon.inl | a7c2f8b051b925998af2d11c445d994a84e2b1abb292ee2032cde41864726fef |
| dependencies/glm/glm/gtc/integer.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/integer.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/integer.hpp<br>framework/dependencies/glm/glm/gtc/integer.hpp | 17f146d81d03887d11c753eb580db2d29593d633e488c49f44142e8b42b4ddac |
| dependencies/glm/glm/gtc/integer.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/integer.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/integer.inl<br>framework/dependencies/glm/glm/gtc/integer.inl | ece2e3c9e1ecbf754369c244503a25f4e772299d08ac98dc6692e33a2d24759f |
| dependencies/glm/glm/gtc/matrix_access.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/matrix_access.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/matrix_access.hpp<br>framework/dependencies/glm/glm/gtc/matrix_access.hpp | 87c20ed9ae8eaf7d993febf6783fb4d04011667f78636d5eeed15ba0b9d47507 |
| dependencies/glm/glm/gtc/matrix_access.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/matrix_access.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/matrix_access.inl<br>framework/dependencies/glm/glm/gtc/matrix_access.inl | 6bcb64d5245584ebfabf06fc5166c9ffd45b1f832f13077b09d6f5e8607711c4 |
| dependencies/glm/glm/gtc/matrix_integer.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/matrix_integer.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/matrix_integer.hpp<br>framework/dependencies/glm/glm/gtc/matrix_integer.hpp | ccfb7d9ae7a65b0eca54af01ed5d079f0e8d75788c4db56b2d81499e4f47f6df |
| dependencies/glm/glm/gtc/matrix_inverse.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/matrix_inverse.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/matrix_inverse.hpp<br>framework/dependencies/glm/glm/gtc/matrix_inverse.hpp | 3272b7c4c6abe6d69da51c439a68ad35fbaa3e6d1af1cb85aa35cfaa08b9f493 |
| dependencies/glm/glm/gtc/matrix_inverse.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/matrix_inverse.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/matrix_inverse.inl<br>framework/dependencies/glm/glm/gtc/matrix_inverse.inl | 5d70d3da6d92bb829acad9351ef7022b780ada7c7981e0b6c719c4dff1c4db33 |
| dependencies/glm/glm/gtc/matrix_transform.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/matrix_transform.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/matrix_transform.hpp<br>framework/dependencies/glm/glm/gtc/matrix_transform.hpp | 0aef74b23433b727dfca0bbe35f3bb9bb14f313346ba1c5c4f971d825d580a5d |
| dependencies/glm/glm/gtc/matrix_transform.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/matrix_transform.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/matrix_transform.inl<br>framework/dependencies/glm/glm/gtc/matrix_transform.inl | 30242dca1319f41aa6286c4cf0374ec6af0a5d6169fae9cfa125dd715f0eb044 |
| dependencies/glm/glm/gtc/noise.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/noise.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/noise.hpp<br>framework/dependencies/glm/glm/gtc/noise.hpp | c8a2e2628315549cc8c62e1235cfb05cb52d242b188d9f99e7b17670de25febd |
| dependencies/glm/glm/gtc/noise.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/noise.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/noise.inl<br>framework/dependencies/glm/glm/gtc/noise.inl | 0099a2804df9b679a8c7dd21c1d7508da108a2fca8d9d42b6c734a9452eba5b2 |
| dependencies/glm/glm/gtc/packing.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/packing.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/packing.hpp<br>framework/dependencies/glm/glm/gtc/packing.hpp | 7f68657057f311c373eb43237a7d646e97d3b2ce121630f4c9056ec3fc720534 |
| dependencies/glm/glm/gtc/packing.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/packing.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/packing.inl<br>framework/dependencies/glm/glm/gtc/packing.inl | 3e48c8c98836323d2d95d2a6e2fe16864dbfaefda6ae95da74d438549ec49851 |
| dependencies/glm/glm/gtc/quaternion.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/quaternion.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/quaternion.hpp<br>framework/dependencies/glm/glm/gtc/quaternion.hpp | 2dbc2c30ad5c24201aadb186f9e757cfc89006bb4b765740a1ab5ae5bce773b2 |
| dependencies/glm/glm/gtc/quaternion.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/quaternion.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/quaternion.inl<br>framework/dependencies/glm/glm/gtc/quaternion.inl | ff39757356667069995e28ed0e2793ee649b2cbe2f5e1b67cdaa7706cd94c2d1 |
| dependencies/glm/glm/gtc/quaternion_simd.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/quaternion_simd.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/quaternion_simd.inl<br>framework/dependencies/glm/glm/gtc/quaternion_simd.inl | f1945cd6c19e56b3c1c78943ef5ec18116907a4ca1efc40a57d48ab1db7adfc5 |
| dependencies/glm/glm/gtc/random.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/random.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/random.hpp<br>framework/dependencies/glm/glm/gtc/random.hpp | ca608d4c85ad790b64d60d33d441c95f42eb266f9eb0246f096d145c192aa8b8 |
| dependencies/glm/glm/gtc/random.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/random.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/random.inl<br>framework/dependencies/glm/glm/gtc/random.inl | a540bbc165dbb6ab2d26469d924f157a3c407189ffc7cab2b1b20ac4bebcfcbd |
| dependencies/glm/glm/gtc/reciprocal.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/reciprocal.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/reciprocal.hpp<br>framework/dependencies/glm/glm/gtc/reciprocal.hpp | 7ec8b0a4147138782021371633afc5382a551dce3d8f2db4a43687c5ac291e5a |
| dependencies/glm/glm/gtc/round.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/round.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/round.hpp<br>framework/dependencies/glm/glm/gtc/round.hpp | 1cc9d5b26c98b84e6353b6de414ead7e5df611573c63afc3c61dfa1e105c289f |
| dependencies/glm/glm/gtc/round.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/round.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/round.inl<br>framework/dependencies/glm/glm/gtc/round.inl | 13984660fb15b5bb1c5781e99f16ae48bfdecb8346c30044e31b5ae34cae35bc |
| dependencies/glm/glm/gtc/type_aligned.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/type_aligned.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/type_aligned.hpp<br>framework/dependencies/glm/glm/gtc/type_aligned.hpp | 58fb1b4309a4e39f2eddbc71f054e85de6d6956e5edd1e424ae4ef815d7c6c97 |
| dependencies/glm/glm/gtc/type_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/type_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/type_precision.hpp<br>framework/dependencies/glm/glm/gtc/type_precision.hpp | d594ee0cd495ff3374df469370b0203c401743723e82803dd02c59e7165817ea |
| dependencies/glm/glm/gtc/type_precision.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/type_precision.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/type_precision.inl<br>framework/dependencies/glm/glm/gtc/type_precision.inl | 05bbecd4bec385a010755790ec01745fb8efa074056747c6ebdeb6694fbd87a4 |
| dependencies/glm/glm/gtc/type_ptr.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/type_ptr.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/type_ptr.hpp<br>framework/dependencies/glm/glm/gtc/type_ptr.hpp | 0b8fdea75e9840608eff08a07215d25da2e282281122c922b35de283a43b2cdf |
| dependencies/glm/glm/gtc/type_ptr.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/type_ptr.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/type_ptr.inl<br>framework/dependencies/glm/glm/gtc/type_ptr.inl | 54c5fd6072d79c81acd6f1f7c8bf70d699cef15cc25b28b47176354b89506d9d |
| dependencies/glm/glm/gtc/ulp.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/ulp.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/ulp.hpp<br>framework/dependencies/glm/glm/gtc/ulp.hpp | 7114ff35d57f68928875c61b3a95b5ff07fa44ddf0508a0c82829c484e0df4ee |
| dependencies/glm/glm/gtc/ulp.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/ulp.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/ulp.inl<br>framework/dependencies/glm/glm/gtc/ulp.inl | 5639d486c78c7a28ad5c536350f372420824802b41fba04644868e00a5e6be22 |
| dependencies/glm/glm/gtc/vec1.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtc/vec1.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtc/vec1.hpp<br>framework/dependencies/glm/glm/gtc/vec1.hpp | d7c73593b3b73776a984544d35bbcc52825361b2d6b2a07ed93ad032e06a030b |
| dependencies/glm/glm/gtx/associated_min_max.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/associated_min_max.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/associated_min_max.hpp<br>framework/dependencies/glm/glm/gtx/associated_min_max.hpp | f088e363eb2550cf00d21e95246e08e08f602d209c0b7772f631490ab78f86a8 |
| dependencies/glm/glm/gtx/associated_min_max.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/associated_min_max.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/associated_min_max.inl<br>framework/dependencies/glm/glm/gtx/associated_min_max.inl | 37ed87051be23099e0b4959cc0ef65c037b2cafb1769a5c161958657f372621a |
| dependencies/glm/glm/gtx/bit.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/bit.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/bit.hpp<br>framework/dependencies/glm/glm/gtx/bit.hpp | 5a31a4322679ce43c0314a90feb2287223e1f0022f7f51b4dc75cb5c25893ab1 |
| dependencies/glm/glm/gtx/bit.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/bit.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/bit.inl<br>framework/dependencies/glm/glm/gtx/bit.inl | acfc45a2147ef4c6e4b5f98bb8a1a74b8199ddb0e23ecd334fa784e0359cd23a |
| dependencies/glm/glm/gtx/closest_point.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/closest_point.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/closest_point.hpp<br>framework/dependencies/glm/glm/gtx/closest_point.hpp | 6c49123b2b9b6a75be8eeec3888f6fd90f2ff0cc933cd4aee132662ed8560077 |
| dependencies/glm/glm/gtx/closest_point.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/closest_point.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/closest_point.inl<br>framework/dependencies/glm/glm/gtx/closest_point.inl | 9ba0da204fb39ed0ffbcab1737d84f7b692029ae26b6bc9ddcc7ffcbfbe0673c |
| dependencies/glm/glm/gtx/color_encoding.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/color_encoding.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/color_encoding.hpp<br>framework/dependencies/glm/glm/gtx/color_encoding.hpp | 626676411a4141e69019ddaecaaccbc80dcb298110b4f5ea4870901f57358c88 |
| dependencies/glm/glm/gtx/color_encoding.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/color_encoding.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/color_encoding.inl<br>framework/dependencies/glm/glm/gtx/color_encoding.inl | 33ad9c49d55427886d86b4030c4f200f568bedda94e64a5259a7b8db2d51ff65 |
| dependencies/glm/glm/gtx/color_space.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/color_space.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/color_space.hpp<br>framework/dependencies/glm/glm/gtx/color_space.hpp | 3710628a54217adab3d4d6a522dcacbc4b8b468e4b6a4ce5e2dacae026dcc549 |
| dependencies/glm/glm/gtx/color_space.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/color_space.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/color_space.inl<br>framework/dependencies/glm/glm/gtx/color_space.inl | f47e6e2ca4c7c42c0f591700ffb4bd5ad468ba36aa0711ab3887c304324b244c |
| dependencies/glm/glm/gtx/color_space_YCoCg.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/color_space_YCoCg.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/color_space_YCoCg.hpp<br>framework/dependencies/glm/glm/gtx/color_space_YCoCg.hpp | 9a13bc9dc9af09f1197657937520bdfe8e1b82842e2c1bd16c3afeb79ae8600c |
| dependencies/glm/glm/gtx/color_space_YCoCg.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/color_space_YCoCg.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/color_space_YCoCg.inl<br>framework/dependencies/glm/glm/gtx/color_space_YCoCg.inl | a2eb82983d935ac152e4b8419be2bc571f57bcf5d419d062adbddcfa2de60f73 |
| dependencies/glm/glm/gtx/common.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/common.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/common.hpp<br>framework/dependencies/glm/glm/gtx/common.hpp | b796116b0a017700c6d2b8ea35f1ae5f7d050c8f2fb195ce6c1ce85b9ed2eb0d |
| dependencies/glm/glm/gtx/common.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/common.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/common.inl<br>framework/dependencies/glm/glm/gtx/common.inl | e4bc51659970309172115be4ff6eaa298a1a1cfc4987efb04a8cfd002736b603 |
| dependencies/glm/glm/gtx/compatibility.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/compatibility.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/compatibility.hpp<br>framework/dependencies/glm/glm/gtx/compatibility.hpp | 472c95fbcbd2ba8c68a323204c3887cd0a990a24955c7f0a8efc2fa5e833b7c5 |
| dependencies/glm/glm/gtx/compatibility.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/compatibility.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/compatibility.inl<br>framework/dependencies/glm/glm/gtx/compatibility.inl | b3545942c4d90cc1ec0d92b5b8c4593b71c9230cd1104a43fdda923b25491912 |
| dependencies/glm/glm/gtx/component_wise.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/component_wise.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/component_wise.hpp<br>framework/dependencies/glm/glm/gtx/component_wise.hpp | 93da54d0d08054c6d4f72d1f607f6f4c1f1ecec9c62e9003bfd30b0e8acc434e |
| dependencies/glm/glm/gtx/component_wise.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/component_wise.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/component_wise.inl<br>framework/dependencies/glm/glm/gtx/component_wise.inl | 17fadf08e210c7b51b33cf159bf6d93a40ad0cb664800fdf06aa139a83896760 |
| dependencies/glm/glm/gtx/dual_quaternion.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/dual_quaternion.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/dual_quaternion.hpp<br>framework/dependencies/glm/glm/gtx/dual_quaternion.hpp | 69413d31fefcfcc07230bb75129965518ea8dbd50637a7ef71e85024ce354980 |
| dependencies/glm/glm/gtx/dual_quaternion.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/dual_quaternion.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/dual_quaternion.inl<br>framework/dependencies/glm/glm/gtx/dual_quaternion.inl | 4fe3726f9ddc7c4b93e0e988fae986aaf519b91ae2e48cc41ec85e812f25f536 |
| dependencies/glm/glm/gtx/easing.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/easing.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/easing.hpp<br>framework/dependencies/glm/glm/gtx/easing.hpp | c95a97d9492b48807c0f3f9669f91faa67d715ad7005df341fe930eca88bab0e |
| dependencies/glm/glm/gtx/easing.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/easing.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/easing.inl<br>framework/dependencies/glm/glm/gtx/easing.inl | f1cc823b653328a374aadce461511436b5bdf5f236c6adbf12d2d9b90d5121a6 |
| dependencies/glm/glm/gtx/euler_angles.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/euler_angles.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/euler_angles.hpp<br>framework/dependencies/glm/glm/gtx/euler_angles.hpp | 7b0a85c5f4bd0aac4197ba6f48ea079c1017888aaba59b21a6aaa515c596e354 |
| dependencies/glm/glm/gtx/euler_angles.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/euler_angles.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/euler_angles.inl<br>framework/dependencies/glm/glm/gtx/euler_angles.inl | 2f383b23679461c9a39b494d07f2c1daf6db7c3d9560e4b8c04edf370b45b5a8 |
| dependencies/glm/glm/gtx/extend.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/extend.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/extend.hpp<br>framework/dependencies/glm/glm/gtx/extend.hpp | 6974d3d820af31ceceea75f84b776051be7764365be8625b889c58593ccfaf31 |
| dependencies/glm/glm/gtx/extend.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/extend.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/extend.inl<br>framework/dependencies/glm/glm/gtx/extend.inl | 68c770692b484a84dd8be8c6713cbd0b9a9c76e18eb6c0ddd8f28ddee04d72e3 |
| dependencies/glm/glm/gtx/extended_min_max.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/extended_min_max.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/extended_min_max.hpp<br>framework/dependencies/glm/glm/gtx/extended_min_max.hpp | bbe9704c6bc9052874340b04174f45ea35c941594c5618649c55f2960b9ab0bd |
| dependencies/glm/glm/gtx/extended_min_max.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/extended_min_max.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/extended_min_max.inl<br>framework/dependencies/glm/glm/gtx/extended_min_max.inl | 99807758d5be8ca9f27a6dbf8de28bd78fb552bf99430da5e04d7089fe7de9d6 |
| dependencies/glm/glm/gtx/exterior_product.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/exterior_product.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/exterior_product.hpp<br>framework/dependencies/glm/glm/gtx/exterior_product.hpp | 210823a450ece948381ac71c8ce86613e0ec2012e6749f43ea0629b64c8c6cfa |
| dependencies/glm/glm/gtx/exterior_product.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/exterior_product.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/exterior_product.inl<br>framework/dependencies/glm/glm/gtx/exterior_product.inl | be51fb66433653dfb91b1dc6511504235e368496bde0653256b6fd3606645527 |
| dependencies/glm/glm/gtx/fast_exponential.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/fast_exponential.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/fast_exponential.hpp<br>framework/dependencies/glm/glm/gtx/fast_exponential.hpp | ba3f1c8a479162e925425ed52a1916000e5fda618175038772b82b067db8cec9 |
| dependencies/glm/glm/gtx/fast_exponential.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/fast_exponential.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/fast_exponential.inl<br>framework/dependencies/glm/glm/gtx/fast_exponential.inl | 65f8afa2ce0c65feefa57857b74c204ec1d98e0f7dc1e095bd2627542f62a9de |
| dependencies/glm/glm/gtx/fast_square_root.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/fast_square_root.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/fast_square_root.hpp<br>framework/dependencies/glm/glm/gtx/fast_square_root.hpp | e512ee5439d68d182ca3497bcc887cd51f4b11d6297f5c1425d6101e8a651055 |
| dependencies/glm/glm/gtx/fast_square_root.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/fast_square_root.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/fast_square_root.inl<br>framework/dependencies/glm/glm/gtx/fast_square_root.inl | 770303c3ba5af5bee51a1f842fa76a7cb8428feb6099fc73a1bc32e09f4591b7 |
| dependencies/glm/glm/gtx/fast_trigonometry.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/fast_trigonometry.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/fast_trigonometry.hpp<br>framework/dependencies/glm/glm/gtx/fast_trigonometry.hpp | f976e42c761d7423d86b9150d1b340f47ce96abe3d1eb4b1089d53097e1258ef |
| dependencies/glm/glm/gtx/fast_trigonometry.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/fast_trigonometry.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/fast_trigonometry.inl<br>framework/dependencies/glm/glm/gtx/fast_trigonometry.inl | a8bf654cd093c92c6214177ea5ce9349f4b3940efb785ac597fc61d6fdb73b59 |
| dependencies/glm/glm/gtx/float_normalize.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/float_normalize.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/float_normalize.inl<br>framework/dependencies/glm/glm/gtx/float_normalize.inl | e6039466bf2eb2af27c2415cb94990ae110666571950d6cdc56754d3b2478ce8 |
| dependencies/glm/glm/gtx/functions.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/functions.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/functions.hpp<br>framework/dependencies/glm/glm/gtx/functions.hpp | af7708c25d6388432b0b8149ebb0a1a6d6b095ac6775366374344d8a689c8083 |
| dependencies/glm/glm/gtx/functions.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/functions.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/functions.inl<br>framework/dependencies/glm/glm/gtx/functions.inl | 296374bc1285d535f75f50a2ae8393f9b79a42ebf59ffd7f63178b4433faff5a |
| dependencies/glm/glm/gtx/gradient_paint.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/gradient_paint.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/gradient_paint.hpp<br>framework/dependencies/glm/glm/gtx/gradient_paint.hpp | 6d9cf170682fe4582a025be1c194c63d6a18cb1447893200e097c92e3964314b |
| dependencies/glm/glm/gtx/gradient_paint.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/gradient_paint.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/gradient_paint.inl<br>framework/dependencies/glm/glm/gtx/gradient_paint.inl | 2fc2cfc87630fb2cf6b0df649a2c32642d08af783393b4d0f2e889eb135f6659 |
| dependencies/glm/glm/gtx/handed_coordinate_space.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/handed_coordinate_space.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/handed_coordinate_space.hpp<br>framework/dependencies/glm/glm/gtx/handed_coordinate_space.hpp | 1cb8639ba63ab373d6be2948ec9af01d8b242e0ab8a4d28e2287e35abdc0bb93 |
| dependencies/glm/glm/gtx/handed_coordinate_space.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/handed_coordinate_space.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/handed_coordinate_space.inl<br>framework/dependencies/glm/glm/gtx/handed_coordinate_space.inl | 6a780257eb98ed85aba8c5f3fee5f290e7587c139a67d0d9dfb45940266dca8e |
| dependencies/glm/glm/gtx/hash.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/hash.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/hash.hpp<br>framework/dependencies/glm/glm/gtx/hash.hpp | 93768ca2d74e33e6e6cdd24ccc018bc78152f3e6cd52e984c9da9cbe2111adb0 |
| dependencies/glm/glm/gtx/hash.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/hash.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/hash.inl<br>framework/dependencies/glm/glm/gtx/hash.inl | 53ac1b58aadf3268045c556a3dc5d54a0b4832b9b9285df92e64c84bc14fd819 |
| dependencies/glm/glm/gtx/integer.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/integer.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/integer.hpp<br>framework/dependencies/glm/glm/gtx/integer.hpp | 132b453d75983b2d73b1482afc56bbae42ee2360581b50a8b3684fe0d3f03d8d |
| dependencies/glm/glm/gtx/integer.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/integer.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/integer.inl<br>framework/dependencies/glm/glm/gtx/integer.inl | 6b84077a7c0b76fee111e048c4c96b2e4787a373281fd3d37c3102ded6cef972 |
| dependencies/glm/glm/gtx/intersect.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/intersect.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/intersect.hpp<br>framework/dependencies/glm/glm/gtx/intersect.hpp | 210df0efe55c852db4bf44b6f67dde5d93e399ef55b9bb84b2e30495bba1c83b |
| dependencies/glm/glm/gtx/intersect.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/intersect.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/intersect.inl<br>framework/dependencies/glm/glm/gtx/intersect.inl | 209bd241af8d28aa59435ba0bae077f8fa18353d44a394dad11a3557b1d2f79b |
| dependencies/glm/glm/gtx/io.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/io.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/io.hpp<br>framework/dependencies/glm/glm/gtx/io.hpp | db2105d6ab4837426a889d373f4b772a50272fd646b579010c45a0669ed09561 |
| dependencies/glm/glm/gtx/io.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/io.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/io.inl<br>framework/dependencies/glm/glm/gtx/io.inl | 22f57ca9b45a1bc13b6827efc13bbf2cadbeb3ddb069cb2c58027d16fa704d47 |
| dependencies/glm/glm/gtx/iteration.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/iteration.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/iteration.hpp<br>framework/dependencies/glm/glm/gtx/iteration.hpp | 9a7083287f735879b191e556003ff7fa80f62ed36f3ccedec9cce718e546f641 |
| dependencies/glm/glm/gtx/iteration.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/iteration.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/iteration.inl<br>framework/dependencies/glm/glm/gtx/iteration.inl | 026b3c703599c6cc708aec0d093c2fb94a37ecd76baca6433ede72bd12fd2636 |
| dependencies/glm/glm/gtx/log_base.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/log_base.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/log_base.hpp<br>framework/dependencies/glm/glm/gtx/log_base.hpp | 0cde266163798e0baf1789561b7f8c813dca7bb68a5e64152f8846282ae30473 |
| dependencies/glm/glm/gtx/log_base.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/log_base.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/log_base.inl<br>framework/dependencies/glm/glm/gtx/log_base.inl | 85699c013bef828a340a0a5898f1d2a8fe15f7396d75fd89268c4d81fcfbdc4d |
| dependencies/glm/glm/gtx/matrix_cross_product.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_cross_product.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_cross_product.hpp<br>framework/dependencies/glm/glm/gtx/matrix_cross_product.hpp | c5556a345945603fe37154dbfb4e087987fccc8333512421da1a3ddf09f9b337 |
| dependencies/glm/glm/gtx/matrix_cross_product.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_cross_product.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_cross_product.inl<br>framework/dependencies/glm/glm/gtx/matrix_cross_product.inl | 06ba79f1135fbf5d66fba4179904df4a510c405f0c41431ace5b0d76102003d4 |
| dependencies/glm/glm/gtx/matrix_decompose.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_decompose.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_decompose.hpp<br>framework/dependencies/glm/glm/gtx/matrix_decompose.hpp | acbc77276935d672809bc08aa51be6a2c55df6df09bcfbd9e75a383e68d71079 |
| dependencies/glm/glm/gtx/matrix_decompose.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_decompose.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_decompose.inl<br>framework/dependencies/glm/glm/gtx/matrix_decompose.inl | 3555fa1ce91cb62cbb4526a8cfdaa5f820d93830b8fe2f64967ac1d4ce15205a |
| dependencies/glm/glm/gtx/matrix_factorisation.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_factorisation.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_factorisation.hpp<br>framework/dependencies/glm/glm/gtx/matrix_factorisation.hpp | 3c616f1f712fc4865c79d530afc0e75d1f30b0e90feedccf3939c4072fd66f19 |
| dependencies/glm/glm/gtx/matrix_factorisation.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_factorisation.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_factorisation.inl<br>framework/dependencies/glm/glm/gtx/matrix_factorisation.inl | 23715c719b138b302710e08f37355e35a8f9838ffca251c89b7fcb2bcd6e4d66 |
| dependencies/glm/glm/gtx/matrix_interpolation.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_interpolation.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_interpolation.hpp<br>framework/dependencies/glm/glm/gtx/matrix_interpolation.hpp | 3c522c8d8b89c7941e0133d90a5bcf6f4812233bdfff636da7d688dc94049ea5 |
| dependencies/glm/glm/gtx/matrix_interpolation.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_interpolation.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_interpolation.inl<br>framework/dependencies/glm/glm/gtx/matrix_interpolation.inl | 973e2cf1232a17e778c3e6164bbc9aa7f2e4192ac6eca478de47acf5e60027c6 |
| dependencies/glm/glm/gtx/matrix_major_storage.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_major_storage.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_major_storage.hpp<br>framework/dependencies/glm/glm/gtx/matrix_major_storage.hpp | 7e364e1bf00b678ba97ef2edc7821d68b63e1b737de097ae3f60a59f19096dbe |
| dependencies/glm/glm/gtx/matrix_major_storage.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_major_storage.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_major_storage.inl<br>framework/dependencies/glm/glm/gtx/matrix_major_storage.inl | 77225638a968bcef1e7586a3a615cb0aaa94ee1d9e691cb29001313d8575d22e |
| dependencies/glm/glm/gtx/matrix_operation.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_operation.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_operation.hpp<br>framework/dependencies/glm/glm/gtx/matrix_operation.hpp | 2351cffb5895d22e5bc3dd63fb926e43b2eed0e274cee49f40cfbda06c6b210f |
| dependencies/glm/glm/gtx/matrix_operation.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_operation.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_operation.inl<br>framework/dependencies/glm/glm/gtx/matrix_operation.inl | 1de136720f3381058c8f7f41fbbe4f0b5baf0da69ec19d278710433540d89a5c |
| dependencies/glm/glm/gtx/matrix_query.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_query.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_query.hpp<br>framework/dependencies/glm/glm/gtx/matrix_query.hpp | 8411d1a5afa033f870479a4701551bacc7834111bd56e3700cb19368d950f1bf |
| dependencies/glm/glm/gtx/matrix_query.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_query.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_query.inl<br>framework/dependencies/glm/glm/gtx/matrix_query.inl | 0a9b95484c088209d7daf1be296524cc4562d7222036fc258a1b3093c03bfe68 |
| dependencies/glm/glm/gtx/matrix_transform_2d.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_transform_2d.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_transform_2d.hpp<br>framework/dependencies/glm/glm/gtx/matrix_transform_2d.hpp | 187560ddc197b08fc5078cfc21e16df3bcc5ea6db00e2b544e5b8f4990e0c109 |
| dependencies/glm/glm/gtx/matrix_transform_2d.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/matrix_transform_2d.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/matrix_transform_2d.inl<br>framework/dependencies/glm/glm/gtx/matrix_transform_2d.inl | 50c0565a770f1ebf81c0504f80e77850cbdc78a28dc82ac358872137406c28bf |
| dependencies/glm/glm/gtx/mixed_product.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/mixed_product.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/mixed_product.hpp<br>framework/dependencies/glm/glm/gtx/mixed_product.hpp | c66320a7c31600053022637ea2e2ee1f8e3921d8beeb2f0a12ef3cba22cbe287 |
| dependencies/glm/glm/gtx/mixed_product.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/mixed_product.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/mixed_product.inl<br>framework/dependencies/glm/glm/gtx/mixed_product.inl | d6b74359ecef2ae9cd19ba8a34faf27dd522a33c9cad5bcecddb6614c0015951 |
| dependencies/glm/glm/gtx/norm.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/norm.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/norm.hpp<br>framework/dependencies/glm/glm/gtx/norm.hpp | 7e0660877037c349847eaaed4a9984fe7d01dd5299f1c33a4478057ba7868a1c |
| dependencies/glm/glm/gtx/norm.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/norm.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/norm.inl<br>framework/dependencies/glm/glm/gtx/norm.inl | 800c6a55c387e470e74eeeb1ec4eaa7aa46c9227fc9454baa914f44277513017 |
| dependencies/glm/glm/gtx/normal.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/normal.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/normal.hpp<br>framework/dependencies/glm/glm/gtx/normal.hpp | 787004d4175773d777a524ccbc254f8d6d63ec1ed29eee658b3ab22833868742 |
| dependencies/glm/glm/gtx/normal.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/normal.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/normal.inl<br>framework/dependencies/glm/glm/gtx/normal.inl | d17d6e052713b4ecdf1b7129dd5691ecb3243e9789b6b4b80f9b5406b2cdf3a0 |
| dependencies/glm/glm/gtx/normalize_dot.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/normalize_dot.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/normalize_dot.hpp<br>framework/dependencies/glm/glm/gtx/normalize_dot.hpp | 970aea4ff39b03a1c7722466f6d80adb6cdf6a2e9461870939f291a39989b08e |
| dependencies/glm/glm/gtx/normalize_dot.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/normalize_dot.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/normalize_dot.inl<br>framework/dependencies/glm/glm/gtx/normalize_dot.inl | badae260727e9176e4f624fd7e4716d6ff2c9a7d3ad463db86aa135e9545ec20 |
| dependencies/glm/glm/gtx/number_precision.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/number_precision.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/number_precision.hpp<br>framework/dependencies/glm/glm/gtx/number_precision.hpp | fb1e65e6042e84e72e55217460b2dd9dbad2f3dbdfbe39cbf2f053a7d3542d56 |
| dependencies/glm/glm/gtx/optimum_pow.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/optimum_pow.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/optimum_pow.hpp<br>framework/dependencies/glm/glm/gtx/optimum_pow.hpp | 7beabee46c33ae1922b15e2d913c4dc85d26e7841fc32391fe1fac02e0a7a38d |
| dependencies/glm/glm/gtx/optimum_pow.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/optimum_pow.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/optimum_pow.inl<br>framework/dependencies/glm/glm/gtx/optimum_pow.inl | 24e9df52744266f60fb1b850be276450af597dc62d62284f598c7d53a667899f |
| dependencies/glm/glm/gtx/orthonormalize.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/orthonormalize.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/orthonormalize.hpp<br>framework/dependencies/glm/glm/gtx/orthonormalize.hpp | 3009c895ea6c56d503465ba8132894a1816f1a3f3de636ac30115b8a06fca0b8 |
| dependencies/glm/glm/gtx/orthonormalize.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/orthonormalize.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/orthonormalize.inl<br>framework/dependencies/glm/glm/gtx/orthonormalize.inl | 0597119fefbb323ae82e4797298e7af9dc514600436d231eb7cd1eb510d7e0df |
| dependencies/glm/glm/gtx/pca.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/pca.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/pca.hpp<br>framework/dependencies/glm/glm/gtx/pca.hpp | 4f63838d8ceaa483f0562ab82543c01d79c88e50dbbd6c63109f7e0615e8a66d |
| dependencies/glm/glm/gtx/pca.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/pca.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/pca.inl<br>framework/dependencies/glm/glm/gtx/pca.inl | 25f4438fd03a61ff119157ca4a26372b14e571ed6ef15c65e363c0a5de415912 |
| dependencies/glm/glm/gtx/perpendicular.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/perpendicular.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/perpendicular.hpp<br>framework/dependencies/glm/glm/gtx/perpendicular.hpp | 17c2693ffb9238e2407c07af1571b9d3d52d0b5c2d71450c5affb0ca4d93374a |
| dependencies/glm/glm/gtx/perpendicular.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/perpendicular.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/perpendicular.inl<br>framework/dependencies/glm/glm/gtx/perpendicular.inl | 115195d90827188d09925a1608889d634d58922839978fba6d66af42cd605e49 |
| dependencies/glm/glm/gtx/polar_coordinates.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/polar_coordinates.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/polar_coordinates.hpp<br>framework/dependencies/glm/glm/gtx/polar_coordinates.hpp | 4a56db9bd4e6ddf008ee5ea07a7df64e8a22169886c3fb093c763fdc38fd94c3 |
| dependencies/glm/glm/gtx/polar_coordinates.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/polar_coordinates.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/polar_coordinates.inl<br>framework/dependencies/glm/glm/gtx/polar_coordinates.inl | 34fcbdf27e27caf0d6b76bd66a6296443e85fba396c8b443f5ee70fbd64123dc |
| dependencies/glm/glm/gtx/projection.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/projection.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/projection.hpp<br>framework/dependencies/glm/glm/gtx/projection.hpp | e012b18348c77a59d681d9025bba9d2b2b9194cbda94fd5e4a7c20357eab24ab |
| dependencies/glm/glm/gtx/projection.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/projection.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/projection.inl<br>framework/dependencies/glm/glm/gtx/projection.inl | ed7608b9ea6b218f5e10cf379d6169169b12f9c5b1a6f37bd679c59363f6a034 |
| dependencies/glm/glm/gtx/quaternion.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/quaternion.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/quaternion.hpp<br>framework/dependencies/glm/glm/gtx/quaternion.hpp | a7a914bc351184d766cb821b3e8d5295939453d4c44fc924af5770d22d763b60 |
| dependencies/glm/glm/gtx/quaternion.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/quaternion.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/quaternion.inl<br>framework/dependencies/glm/glm/gtx/quaternion.inl | 6d9655c893a990a31b6529983cccfa7696dc08606d2e34448ceed0e1516ca712 |
| dependencies/glm/glm/gtx/range.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/range.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/range.hpp<br>framework/dependencies/glm/glm/gtx/range.hpp | c1b35ae8855c29a7a8b94ae92b0d6c576accde9234738381c426d761f9500266 |
| dependencies/glm/glm/gtx/raw_data.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/raw_data.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/raw_data.hpp<br>framework/dependencies/glm/glm/gtx/raw_data.hpp | b5ffe9a52fc8e5e7848d38c68787d01d2d78c6f968fa97b4815301af0d47fd2a |
| dependencies/glm/glm/gtx/raw_data.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/raw_data.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/raw_data.inl<br>framework/dependencies/glm/glm/gtx/raw_data.inl | 835ba49219f27f24849da440a3c10ca54e5ed30cdb112c95370518646a44637c |
| dependencies/glm/glm/gtx/rotate_normalized_axis.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/rotate_normalized_axis.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/rotate_normalized_axis.hpp<br>framework/dependencies/glm/glm/gtx/rotate_normalized_axis.hpp | 4301eb490af85e6ec3e7376f9e269a43ef87342e8b3d1abfb0f08eeae4819313 |
| dependencies/glm/glm/gtx/rotate_normalized_axis.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/rotate_normalized_axis.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/rotate_normalized_axis.inl<br>framework/dependencies/glm/glm/gtx/rotate_normalized_axis.inl | b5955df01c4fe9ea7bf275edfd2e78e1dcf49a50411a3ebd707ea57077bcdbb2 |
| dependencies/glm/glm/gtx/rotate_vector.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/rotate_vector.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/rotate_vector.hpp<br>framework/dependencies/glm/glm/gtx/rotate_vector.hpp | 285a668f8c6d349c727e07ca76578783c1f09ec2bed18431c73f4107c99e3d3e |
| dependencies/glm/glm/gtx/rotate_vector.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/rotate_vector.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/rotate_vector.inl<br>framework/dependencies/glm/glm/gtx/rotate_vector.inl | e4175c3985443ce1bd5034e05c7f740ba0b2c450b3b5ba246bbdddf8faf2b234 |
| dependencies/glm/glm/gtx/scalar_multiplication.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/scalar_multiplication.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/scalar_multiplication.hpp<br>framework/dependencies/glm/glm/gtx/scalar_multiplication.hpp | 08ef0279871de09c13b8dca131b442a483284714955b48b1829945a0a82a6562 |
| dependencies/glm/glm/gtx/scalar_relational.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/scalar_relational.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/scalar_relational.hpp<br>framework/dependencies/glm/glm/gtx/scalar_relational.hpp | 90a1e3eecc2fd3d7da187033827de81e44a0b5d8bf1e0fb88eac7c5e4c97687f |
| dependencies/glm/glm/gtx/scalar_relational.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/scalar_relational.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/scalar_relational.inl<br>framework/dependencies/glm/glm/gtx/scalar_relational.inl | e8f934703017a9224f1d2b477a3c9cb87aaedbb212d9570906032f74c7f6d229 |
| dependencies/glm/glm/gtx/spline.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/spline.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/spline.hpp<br>framework/dependencies/glm/glm/gtx/spline.hpp | 270afc994bad59961a0e6fedac2300efde95810a5083c4f67fd4263309750be5 |
| dependencies/glm/glm/gtx/spline.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/spline.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/spline.inl<br>framework/dependencies/glm/glm/gtx/spline.inl | ce468401be713b6c732d3ab58ad5837ad4eba9398526481c5bcf8346f7e4bd8f |
| dependencies/glm/glm/gtx/std_based_type.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/std_based_type.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/std_based_type.hpp<br>framework/dependencies/glm/glm/gtx/std_based_type.hpp | 9ef6192990d67cfea5a894fdbba9ae0dcf0b8ffe4f70ef1cdc2f8eaab80d8bc8 |
| dependencies/glm/glm/gtx/std_based_type.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/std_based_type.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/std_based_type.inl<br>framework/dependencies/glm/glm/gtx/std_based_type.inl | e9132bf571a9a80d04e06ba1b92401b89177e439a6c306a694aab44125bf609c |
| dependencies/glm/glm/gtx/string_cast.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/string_cast.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/string_cast.hpp<br>framework/dependencies/glm/glm/gtx/string_cast.hpp | 4614291c958594c28f437866db2669a8dc62e09b49d7e7da8c6566b1fa79dd37 |
| dependencies/glm/glm/gtx/string_cast.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/string_cast.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/string_cast.inl<br>framework/dependencies/glm/glm/gtx/string_cast.inl | 5e0d0c154bf6f4c3537e80668a09f962443656e681a560469e507e6eb31c268b |
| dependencies/glm/glm/gtx/structured_bindings.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/structured_bindings.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/structured_bindings.hpp<br>framework/dependencies/glm/glm/gtx/structured_bindings.hpp | ed9db45581daafbff7c5a899ee006dcc9e7c78f1081e8d01a865066390890a69 |
| dependencies/glm/glm/gtx/structured_bindings.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/structured_bindings.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/structured_bindings.inl<br>framework/dependencies/glm/glm/gtx/structured_bindings.inl | 715f7995372042e97a7f4db66ca3a10a6c023a3e58cd6664a28c252a58fe6b02 |
| dependencies/glm/glm/gtx/texture.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/texture.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/texture.hpp<br>framework/dependencies/glm/glm/gtx/texture.hpp | 43b56639e3bc43456d5af6a3230d96d5e368d2d9c920691eb2ec6e5cb3f9c6e0 |
| dependencies/glm/glm/gtx/texture.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/texture.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/texture.inl<br>framework/dependencies/glm/glm/gtx/texture.inl | e00a18eacb9cb158de1cd84c2ff8a08e27afa7f03a13475f35ee06c5f7362ba4 |
| dependencies/glm/glm/gtx/transform.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/transform.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/transform.hpp<br>framework/dependencies/glm/glm/gtx/transform.hpp | eeba2399bd1a0a7c57aefe8be663b9e9a338fc3fa3ddb14b88dc4817c4f7c839 |
| dependencies/glm/glm/gtx/transform.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/transform.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/transform.inl<br>framework/dependencies/glm/glm/gtx/transform.inl | a92ca1ffb9cc998723289f19841d54a49bea0c71a04f875e4736223261be029b |
| dependencies/glm/glm/gtx/transform2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/transform2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/transform2.hpp<br>framework/dependencies/glm/glm/gtx/transform2.hpp | 1e5cb9960ae28e2e5bfc5fd3c07c48b3b9d810542699143f5936fa270b44240d |
| dependencies/glm/glm/gtx/transform2.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/transform2.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/transform2.inl<br>framework/dependencies/glm/glm/gtx/transform2.inl | b2fe1bab72eac347f1d1fdf0eb3c675b0adeb1d84bf25e337a58316e671a9000 |
| dependencies/glm/glm/gtx/type_aligned.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/type_aligned.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/type_aligned.hpp<br>framework/dependencies/glm/glm/gtx/type_aligned.hpp | e7127320c8d967a787c3298e0399ce65bfcdef09a21174b578b7afd5ecb82873 |
| dependencies/glm/glm/gtx/type_aligned.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/type_aligned.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/type_aligned.inl<br>framework/dependencies/glm/glm/gtx/type_aligned.inl | 8a13d7cc7e03e83781ba18b8faa4c377a752e2d084b07a3717529ff975eb872d |
| dependencies/glm/glm/gtx/type_trait.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/type_trait.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/type_trait.hpp<br>framework/dependencies/glm/glm/gtx/type_trait.hpp | 93dabdba43fb44d3c622b77070c3cf58676cd611a89f1daee2ea683d3b5b14d8 |
| dependencies/glm/glm/gtx/type_trait.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/type_trait.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/type_trait.inl<br>framework/dependencies/glm/glm/gtx/type_trait.inl | a6ddfc7bd958897acf7c64795be4cf126b563517a59030c858e204f298cfed81 |
| dependencies/glm/glm/gtx/vec_swizzle.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/vec_swizzle.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/vec_swizzle.hpp<br>framework/dependencies/glm/glm/gtx/vec_swizzle.hpp | 5299aa5e5cb96eb52f0ce49fd572cdf91da86241d3e6a6ff8e5b64e07e1fb7c6 |
| dependencies/glm/glm/gtx/vector_angle.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/vector_angle.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/vector_angle.hpp<br>framework/dependencies/glm/glm/gtx/vector_angle.hpp | abd1ec2d18a29f8c236e3979ab6bfd3dd4540cca6edece04a859ebd427296021 |
| dependencies/glm/glm/gtx/vector_angle.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/vector_angle.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/vector_angle.inl<br>framework/dependencies/glm/glm/gtx/vector_angle.inl | e75e5a6bb98cbbee1585ad66ded2bb87a38028697cb3808efade4fad25e5a2da |
| dependencies/glm/glm/gtx/vector_query.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/vector_query.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/vector_query.hpp<br>framework/dependencies/glm/glm/gtx/vector_query.hpp | f63e1a4a6b020b05979d265cc656ae949d2b84b01306b96bc568b1c7682e99a1 |
| dependencies/glm/glm/gtx/vector_query.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/vector_query.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/vector_query.inl<br>framework/dependencies/glm/glm/gtx/vector_query.inl | 744768db68d23dedbc1ab33d4ca60764b276cfb5c30def3deaa2033fa789865a |
| dependencies/glm/glm/gtx/wrap.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/wrap.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/wrap.hpp<br>framework/dependencies/glm/glm/gtx/wrap.hpp | 63ceadb7338652f8db8c487e712e2b2143e5cb534d3274ac1099bb404ed7451d |
| dependencies/glm/glm/gtx/wrap.inl | openGL_3dAssign_14/framework/dependencies/glm/glm/gtx/wrap.inl<br>openGL_3dAssign_15/framework/dependencies/glm/glm/gtx/wrap.inl<br>framework/dependencies/glm/glm/gtx/wrap.inl | 97cde43039625b06f13c4df01a828474b9cf5b6c2fe61c6d63f5e6fcbeaf86d8 |
| dependencies/glm/glm/integer.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/integer.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/integer.hpp<br>framework/dependencies/glm/glm/integer.hpp | fbb3ca5c518502bbba672ca1006a2e299262b5814e24572e50cb592086641594 |
| dependencies/glm/glm/mat2x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/mat2x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/mat2x2.hpp<br>framework/dependencies/glm/glm/mat2x2.hpp | ad69b7477122d91a9c5f5a4655c2e66ff348c8143c9c97511517e04e45020e44 |
| dependencies/glm/glm/mat2x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/mat2x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/mat2x3.hpp<br>framework/dependencies/glm/glm/mat2x3.hpp | 99f0c981ddac7432e3b7637a6d1d63b3345593a190feb670059c7dacbf2f8053 |
| dependencies/glm/glm/mat2x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/mat2x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/mat2x4.hpp<br>framework/dependencies/glm/glm/mat2x4.hpp | d9fe8b645b3f76186da7706e36750622b25dacc11aadc0171e00b7e8bd3e6d4a |
| dependencies/glm/glm/mat3x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/mat3x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/mat3x2.hpp<br>framework/dependencies/glm/glm/mat3x2.hpp | d55d1216d9701c51feb2d74ed62971d0000548eca60132812edaec6ed2aa589b |
| dependencies/glm/glm/mat3x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/mat3x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/mat3x3.hpp<br>framework/dependencies/glm/glm/mat3x3.hpp | 94d0e032b5458aeeea0233692b0181f820a4b498ca6c0d5ebe38276afb0be9bd |
| dependencies/glm/glm/mat3x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/mat3x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/mat3x4.hpp<br>framework/dependencies/glm/glm/mat3x4.hpp | 28f6826b1cded1ee9eacfa26037775ec377a4157c80534664a2c443a32181978 |
| dependencies/glm/glm/mat4x2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/mat4x2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/mat4x2.hpp<br>framework/dependencies/glm/glm/mat4x2.hpp | 4f041500e15c4690a19d266394d4585fc9962f99907269ad00cecbbee3598c04 |
| dependencies/glm/glm/mat4x3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/mat4x3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/mat4x3.hpp<br>framework/dependencies/glm/glm/mat4x3.hpp | 4681e9ebf357d4aca98394a8e056b58a26a575792054b7bf4d2694a1ec020c6f |
| dependencies/glm/glm/mat4x4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/mat4x4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/mat4x4.hpp<br>framework/dependencies/glm/glm/mat4x4.hpp | 367b21fe6b9052a4dcb3e7f146038fa547384755f58c1c5784a54243b4c014a2 |
| dependencies/glm/glm/matrix.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/matrix.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/matrix.hpp<br>framework/dependencies/glm/glm/matrix.hpp | ca9f427d40a824528d3f4d320dab552c4c4a1034e0b8786f7eede43a34f86e72 |
| dependencies/glm/glm/packing.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/packing.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/packing.hpp<br>framework/dependencies/glm/glm/packing.hpp | a2dcdc7769b43f3d1d3e5c7b959beab1898797df83ffb750d85e20c4059b72a4 |
| dependencies/glm/glm/simd/common.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/common.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/common.h<br>framework/dependencies/glm/glm/simd/common.h | 1cfbea079bf62ac76cf7aeb2532a6162d388088807dbdf876ef64053b298929e |
| dependencies/glm/glm/simd/exponential.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/exponential.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/exponential.h<br>framework/dependencies/glm/glm/simd/exponential.h | b04a68b1fcdf87200873db2593ab50681184820997949de8e75fd0daadbc1fe8 |
| dependencies/glm/glm/simd/geometric.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/geometric.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/geometric.h<br>framework/dependencies/glm/glm/simd/geometric.h | 9c740b6f17cfd2092171353381b17b7622c1d2121ee43395d6ed8a6b8954744d |
| dependencies/glm/glm/simd/integer.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/integer.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/integer.h<br>framework/dependencies/glm/glm/simd/integer.h | 7361a721e51303b4f89379ad0c752ed90f85332eb2e1c75748abe461916779b6 |
| dependencies/glm/glm/simd/matrix.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/matrix.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/matrix.h<br>framework/dependencies/glm/glm/simd/matrix.h | 1a41850417c7daa54a0b7a283e0cab5508f6b3097e7b7936b6bc4be0738467e1 |
| dependencies/glm/glm/simd/neon.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/neon.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/neon.h<br>framework/dependencies/glm/glm/simd/neon.h | 247d54d342596361f6e37f3b2b42594aa2a0616b9e02611478a1e04f58398b18 |
| dependencies/glm/glm/simd/packing.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/packing.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/packing.h<br>framework/dependencies/glm/glm/simd/packing.h | 99efb5a0b5c2ac5d34782de883dbb92d613f1f149fabfb3a3e32e703c2f016ae |
| dependencies/glm/glm/simd/platform.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/platform.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/platform.h<br>framework/dependencies/glm/glm/simd/platform.h | f6e3a790cecb95c839aeb6e904d8bf4c865755c6976c2a2b71739e4c087b5593 |
| dependencies/glm/glm/simd/trigonometric.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/trigonometric.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/trigonometric.h<br>framework/dependencies/glm/glm/simd/trigonometric.h | 1b9492b7c7fb284dcf8a86bf25fa379c84001c38a5893e3b37b7834c3d4b6b3e |
| dependencies/glm/glm/simd/vector_relational.h | openGL_3dAssign_14/framework/dependencies/glm/glm/simd/vector_relational.h<br>openGL_3dAssign_15/framework/dependencies/glm/glm/simd/vector_relational.h<br>framework/dependencies/glm/glm/simd/vector_relational.h | 50298b14982ff78b6c1cb88d15c8549bf7ee11d2baf5a77d3787ac0deed3b001 |
| dependencies/glm/glm/trigonometric.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/trigonometric.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/trigonometric.hpp<br>framework/dependencies/glm/glm/trigonometric.hpp | 15f7b9d814d244d0f1351329bc7cd3c853a03c95cb350197d9b203ecce463f71 |
| dependencies/glm/glm/vec2.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/vec2.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/vec2.hpp<br>framework/dependencies/glm/glm/vec2.hpp | 993c05b83e034d8bc451f25749d0d5a1ef003ed614cf7268693dbb43cdfef567 |
| dependencies/glm/glm/vec3.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/vec3.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/vec3.hpp<br>framework/dependencies/glm/glm/vec3.hpp | be9ca0adf100538cf4ff62cb4cbd2d8504cb410baf9f28e9ea748c57c461a14d |
| dependencies/glm/glm/vec4.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/vec4.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/vec4.hpp<br>framework/dependencies/glm/glm/vec4.hpp | 8ce1f9cd962932a8fefa61fa84a5972aebb9721cbd07e74106ab2349c4c83ba2 |
| dependencies/glm/glm/vector_relational.hpp | openGL_3dAssign_14/framework/dependencies/glm/glm/vector_relational.hpp<br>openGL_3dAssign_15/framework/dependencies/glm/glm/vector_relational.hpp<br>framework/dependencies/glm/glm/vector_relational.hpp | 7faf05e55d5346c10189abc5af01a68b00c240824093742a7363f514360b880c |
| DEPENDENCIES.patch | openGL_3dAssign_14/framework/DEPENDENCIES.patch<br>openGL_3dAssign_15/framework/DEPENDENCIES.patch<br>framework/DEPENDENCIES.patch | 9e0f7c1e58a28bf45432b462c872f35f5755e6ace92ddee8519375a263d8577a |

## UTF-8 BOM 통일: 실습 13 솔루션

### openGL_3dAssign_13/openGL_assign_13.slnx — before

````xml
<Solution>
  <Configurations>
    <Platform Name="x64" />
    <Platform Name="x86" />
  </Configurations>
  <Project Path="openGL_sample2.vcxproj" Id="3f7bde5d-cd40-4419-9023-66456a3a28eb" />
</Solution>
````

### after

````xml
<Solution>
  <Configurations>
    <Platform Name="x64" />
    <Platform Name="x86" />
  </Configurations>
  <Project Path="openGL_sample2.vcxproj" Id="3f7bde5d-cd40-4419-9023-66456a3a28eb" />
</Solution>
````

내용은 동일하며 저장 인코딩에 UTF-8 BOM을 추가했다.

## 후속 변경: 사용하지 않는 Common.h 제거

사용자 요청에 따라 실습 13·14·15의 호환 헤더와 프로젝트 등록을 제거했다. 빌드와 실행 테스트는 사용자가 수행한다.

### openGL_3dAssign_13\Common.h
Before:
~~~cpp
#pragma once

// 기존 과제 코드의 include와 이름을 유지하는 호환 헤더.
// 새 코드는 Graphics.h와 graphics 네임스페이스를 사용한다.
#include "Graphics.h"

using graphics::WINDOW_WIDTH;
using graphics::WINDOW_HEIGHT;
using graphics::MAX_ENTITIES;
using graphics::EntityId;
using graphics::GlColor;
using graphics::GLShape;
using graphics::GLFace;
using graphics::Transform3D;
using graphics::PlaneFigure;
using graphics::Polyhedron;
using graphics::Component_Poly;
using graphics::FaceId;
using graphics::DiagramSystem;
using graphics::Camera;
using graphics::CameraSystem;
using graphics::readTextFile;
using Entity = graphics::Scene;
using ShaderHandler = graphics::Renderer;

~~~
After: 파일 삭제.

openGL_3dAssign_13\openGL_sample2.vcxproj Before:
~~~xml
    <ClInclude Include="Common.h" />
~~~
After: 위 등록 블록 삭제.

openGL_3dAssign_13\openGL_sample2.vcxproj.filters Before:
~~~xml
    <ClInclude Include="Common.h">
      <Filter>헤더 파일</Filter>
    </ClInclude>
~~~
After: 위 등록 블록 삭제.

### openGL_3dAssign_14\Common.h
Before:
~~~cpp
#pragma once

// 기존 과제 코드의 include와 이름을 유지하는 호환 헤더.
// 새 코드는 Graphics.h와 graphics 네임스페이스를 사용한다.
#include "Graphics.h"

using graphics::WINDOW_WIDTH;
using graphics::WINDOW_HEIGHT;
using graphics::MAX_ENTITIES;
using graphics::EntityId;
using graphics::GlColor;
using graphics::GLShape;
using graphics::GLFace;
using graphics::Transform3D;
using graphics::PlaneFigure;
using graphics::Polyhedron;
using graphics::Component_Poly;
using graphics::FaceId;
using graphics::DiagramSystem;
using graphics::Camera;
using graphics::CameraSystem;
using graphics::readTextFile;
using Entity = graphics::Scene;
using ShaderHandler = graphics::Renderer;

~~~
After: 파일 삭제.

openGL_3dAssign_14\openGL_sample2.vcxproj Before:
~~~xml
    <ClInclude Include="Common.h" />
~~~
After: 위 등록 블록 삭제.

openGL_3dAssign_14\openGL_sample2.vcxproj.filters Before:
~~~xml
    <ClInclude Include="Common.h">
      <Filter>헤더 파일</Filter>
    </ClInclude>
~~~
After: 위 등록 블록 삭제.

### openGL_3dAssign_15\Common.h
Before:
~~~cpp
#pragma once

// 기존 과제 코드의 include와 이름을 유지하는 호환 헤더.
// 새 코드는 Graphics.h와 graphics 네임스페이스를 사용한다.
#include "Graphics.h"

using graphics::WINDOW_WIDTH;
using graphics::WINDOW_HEIGHT;
using graphics::MAX_ENTITIES;
using graphics::EntityId;
using graphics::GlColor;
using graphics::GLShape;
using graphics::GLFace;
using graphics::Transform3D;
using graphics::PlaneFigure;
using graphics::Polyhedron;
using graphics::Component_Poly;
using graphics::FaceId;
using graphics::DiagramSystem;
using graphics::Camera;
using graphics::CameraSystem;
using graphics::readTextFile;
using Entity = graphics::Scene;
using ShaderHandler = graphics::Renderer;

~~~
After: 파일 삭제.

openGL_3dAssign_15\openGL_sample2.vcxproj Before:
~~~xml
    <ClInclude Include="Common.h" />
~~~
After: 위 등록 블록 삭제.

openGL_3dAssign_15\openGL_sample2.vcxproj.filters Before:
~~~xml
    <ClInclude Include="Common.h">
      <Filter>헤더 파일</Filter>
    </ClInclude>
~~~
After: 위 등록 블록 삭제.
