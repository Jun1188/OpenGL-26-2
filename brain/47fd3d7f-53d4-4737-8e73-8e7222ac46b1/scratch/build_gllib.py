import pathlib
import re

root = pathlib.Path(r"C:\Users\변준영\바탕 화면\college\OpenGL\OpenGL-26-2")
fw = root / "openGL_3dAssign_28" / "framework"
out_dir = root / "test2"
out_dir.mkdir(parents=True, exist_ok=True)

header_files = [
    fw / "include" / "Diagnostics.h",
    fw / "include" / "Model.h",
    fw / "include" / "Transform.h",
    fw / "include" / "Motion.h",
    fw / "include" / "Geometry.h",
    fw / "include" / "Texture.h",
    fw / "include" / "ModelLoader.h",
    fw / "include" / "RenderConfig.h",
    fw / "include" / "Graphics.h",
    fw / "include" / "Picking.h",
    fw / "include" / "Application.h",
    fw / "include" / "Dynamics.h",
    fw / "detail" / "Shaders.h",
    fw / "detail" / "Internal.h",
]

cpp_files = [
    fw / "detail" / "Transform.cpp",
    fw / "detail" / "Dynamics.cpp",
    fw / "detail" / "ObjLoader.cpp",
    fw / "detail" / "Motion.cpp",
    fw / "detail" / "Scene.cpp",
    fw / "detail" / "Renderer.cpp",
    fw / "detail" / "Application.cpp",
]

preamble = """#pragma once
// OpenGL Practice 28 Framework - Single-File Compressed Edition (normal dependencies)
// Dependencies: GLEW, GLFW, GLM, Windows SDK (opengl32.lib, glew32.lib, glfw3dll.lib, ole32.lib, windowscodecs.lib)
// C++17, Windows x64, /utf-8

#ifndef _WIN32
#error This framework targets Windows x64.
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <GL/glew.h>
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <wincodec.h>
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "windowscodecs.lib")

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <numeric>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

static_assert(sizeof(void*) == 8, "x64 build required.");
"""

native_replacement = """
namespace graphics::detail {
inline std::filesystem::path executableDirectory() {
    std::wstring path(32768, L'\\0');
    const DWORD size = GetModuleFileNameW(nullptr, path.data(), DWORD(path.size()));
    if (!size || size >= path.size())
        throw std::runtime_error("Cannot locate the executable directory.");
    path.resize(size);
    return std::filesystem::path(path).parent_path();
}

inline std::filesystem::path relativePathInside(const std::filesystem::path& root,
                                                const std::filesystem::path& relative) {
    if (relative.empty() || relative.is_absolute() || relative.has_root_name() || relative.has_root_directory())
        throw std::runtime_error("Use a relative path inside the project.");
    const auto base = std::filesystem::weakly_canonical(root);
    const auto full = std::filesystem::weakly_canonical(base / relative);
    auto a = base.begin(), b = full.begin();
    for (; a != base.end(); ++a, ++b)
        if (b == full.end() || *a != *b)
            throw std::runtime_error("Path leaves the project directory.");
    return full;
}

inline bool checkGlAt(const char* file, int line, PipelineStage stage, const char* api) {
    bool success = true;
    for (GLenum code = glGetError(); code != GL_NO_ERROR; code = glGetError()) {
        const char* cause =
            code == GL_INVALID_ENUM                    ? "GL_INVALID_ENUM: unsupported enum"
            : code == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE: value outside allowed range"
            : code == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION: incompatible binding, state or program"
            : code == GL_OUT_OF_MEMORY                 ? "GL_OUT_OF_MEMORY: GPU allocation failed"
            : code == GL_INVALID_FRAMEBUFFER_OPERATION ? "GL_INVALID_FRAMEBUFFER_OPERATION: incomplete framebuffer"
                                                       : "OpenGL error";
        Diagnostics::failAt(
            file, line, stage, api, std::string(cause) + " (" + std::to_string(code) + ")",
            "Inspect this stage's bindings, buffer sizes, vertex layout, shader log and current context. A GL error does not identify a single root cause.");
        success = false;
    }
    return success;
}
} // namespace graphics::detail
"""

def strip_includes_and_pragmas(text: str) -> str:
    out = []
    for line in text.splitlines():
        s = line.strip()
        if s == "#pragma once":
            continue
        if s.startswith("#include "):
            continue
        if s.startswith("#pragma comment(lib,"):
            continue
        out.append(line)
    return "\n".join(out)

def transform_gl_glfw(text: str, filename: str) -> str:
    # Replace detail::gl().X -> glX, gl().X -> glX
    text = re.sub(r'\b(?:detail::)?gl\(\)\.', 'gl', text)
    # Remove `auto& api = gl();` / `auto& gl = detail::gl();`
    text = re.sub(r'^\s*auto&\s+(?:api|gl)\s*=\s*(?:detail::)?gl\(\);\s*\n', '', text, flags=re.MULTILINE)
    # Replace `api.X` and `gl.X` (where X is uppercase GL function suffix) -> `glX`
    text = re.sub(r'\b(?:api|gl)\.([A-Z][A-Za-z0-9_]*)', r'gl\1', text)
    # Remove `detail::gl().load();` or `gl.load();` or `glload();`
    text = re.sub(r'^\s*(?:detail::)?glload\(\);\s*\n', '', text, flags=re.MULTILINE)

    # Replace detail::glfw().createWindow -> glfwCreateWindow
    text = text.replace('detail::glfw().createWindow', 'glfwCreateWindow')
    # Replace detail::glfw().X -> glfwX
    text = re.sub(r'\bdetail::glfw\(\)\.([A-Z][A-Za-z0-9_]*)', r'glfw\1', text)
    # Remove `detail::glfw().load(runtime);` or `glfwload(runtime);`
    text = re.sub(r'^\s*glfwload\([^)]*\);\s*\n', '', text, flags=re.MULTILINE)

    if filename == "Application.cpp":
        # Remove unused `runtime` variable in Application::initialize
        text = re.sub(r'^\s*const auto runtime = detail::relativePathInside\(root, config\.runtimeDirectory\);\s*\n', '', text, flags=re.MULTILINE)
        # Add glewInit() right after ensureContext("initialize -> attach new window", ...)
        target = '    if (!ensureContext("initialize -> attach new window", __FILE__, __LINE__))\n        return false;'
        replacement = target + '\n\n    glewExperimental = GL_TRUE;\n    if (glewInit() != GLEW_OK)\n        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glewInit", "GLEW 초기화 실패.");'
        assert target in text, "Could not find ensureContext target in Application.cpp"
        text = text.replace(target, replacement, 1)

    return text

guide_comment = r"""
/*
================================================================================
  OpenGL Framework & Practice 28 기본 가이드 (docs HTML + 실습 28 요약)
================================================================================

1. 의존성 및 빌드 구성 (normal 기준)
--------------------------------------------------------------------------------
- 포함 헤더/라이브러리:
  * GLEW (glew32.lib / glew32.dll), GLFW (glfw3dll.lib / glfw3.dll), GLM
  * Windows SDK: opengl32.lib, ole32.lib, windowscodecs.lib
- 프로젝트 설정:
  * C++17 (/std:c++17), x64, /utf-8, 전처리기 GLFW_DLL;GLFW_INCLUDE_NONE
  * main.cpp에서 `#include "gllib.cpp"`로 단일 번역 단위(TU)로 포함하여 사용
  * (.vcxproj에서 gllib.cpp는 ClCompile이 아닌 None/ClInclude로 두어 중복 컴파일 방지)

2. 기본 실행 골격 (docs/code-guide/api.html · exam.html)
--------------------------------------------------------------------------------
  graphics::Application app;
  Console console(app); // run() 종료까지 수명 유지
  app.setInitHandler([&](graphics::Application&) { return console.initScene(); });
  app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
      console.onKey(key, action, mods);
  });
  app.setUpdateHandler([&](graphics::Application&, float dt) { console.onUpdate(dt); });
  app.setFocusHandler([&](graphics::Application&, bool focused) { console.onFocus(focused); });
  graphics::ApplicationConfig config;
  config.title = "Title";
  return app.run(config);

- run() 내부 순서:
  1) initialize(): GLFW/OpenGL 3.3 Core 창 생성 -> 컨텍스트 바인딩 -> glewInit()
                   -> 내장/외부 GLSL 셰이더 및 VAO/VBO/EBO 초기화
  2) initHandler() 1회 호출 (false 반환 시 종료)
  3) 매 프레임 루프:
     glfwPollEvents() -> 최소화 시(fb <= 0) glfwWaitEvents() 대기
     -> updateHandler(dt) (dt는 0~0.05초로 클램프)
     -> scene_.updateMotions(dt)
     -> renderHandler() 또는 기본 renderScene()
     -> glfwSwapBuffers()

3. 요구사항 -> 행동 -> API 호출 매핑 (docs/code-guide/api.html · exam.html)
--------------------------------------------------------------------------------
- 정적 장면 & 계층/관절:
  * Scene::addGroup(), addCuboid(), addSphere(), addCone(), addModel()
  * DiagramSystem::makePolyhedron(GLShape::Cube, size)
  * Scene::setParent(child, parent)로 트리 계층 구성 (순환 금지)
  * Transform3D: T(basis) * R(rotation) * S(scale) * T(-localBasis)
    -> 메시 배율(scale)과 관절 피벗/위치를 분리해야 비례 왜곡을 막는다.
- 키 입력 & 상태 관리:
  * 단발 토글: action == GLFW_PRESS 일 때만 상태 반전 (GLFW_REPEAT 무시)
  * 지속 이동: PRESS/RELEASE 시 pressed_[key] 갱신 -> onUpdate(dt)에서 dt 곱해 반영
  * onFocus(false)에서 pressed_ 등 지속 입력 상태를 모두 초기화
  * Scene::clear() 호출 후에는 기존 EntityId를 재사용하지 말고 새로 저장
- 모션 & 경로:
  * Motion::fixed / rotation / followPath / linear / pendulum / then / action
  * MotionSystem: createSpin(로컬 자전), createOrbit(월드 공전), pushLayer, bindEntity
- 다중 카메라 & 뷰포트:
  * Application::getFramebufferSize()로 실제 픽셀 크기 확인
  * RenderPass 목록 구성 후 Application::renderViews(passes) 호출
  * 업데이트(시간/충돌)는 프레임당 1회만 수행하고 렌더만 패스별로 분할
- 조명·명암·컬링:
  * RenderOptions: lighting, directionalIntensity, ambientColor, pointLights(최대 8), cull
  * Material: ambientStrength, diffuseStrength, specularStrength, shininess, unlit
  * 내장 셰이더는 방향광+점광원 Phong 표면 명암을 계산하며 cast shadow는 미지원
- 오류 진단 & 역추적:
  * ApplicationConfig::diagnostics = { .errors = true, .trace = true }
  * Diagnostics::lastError() 또는 Diagnostics::failAt(__FILE__, __LINE__, ...) 확인

4. 실습 28 아키텍처 및 기본 조작 요약 (openGL_3dAssign_28)
--------------------------------------------------------------------------------
- 4계층 구조:
  App(main + Console) -> Application(창/루프/콜백)
  -> Scene / MotionSystem / TransformSystem / Picking / AssetLoader
  -> Renderer(GPU VAO/VBO/EBO/Shader/Texture)
- 실습 28 키 조작 (테스트용 main.cpp 기준):
  * 방향키      : 탱크 XZ 평면 이동 (X/Z ±3.5 제한)
  * h           : 하부 궤도 높이 진동 토글
  * t           : 중앙 몸체 Y축 자전 토글
  * l           : 상부 두 블록 반원 경로 교환 / 완료 후 재입력 시 복귀
  * g           : 포신 반대 방향 Y축 회전 토글
  * p           : 깃대 반대 방향 X축 회전(±60도) 토글
  * w/a/s/d     : 카메라 수평 이동 (시선 기준 앞/뒤/좌/우)
  * x/X, z/Z    : 카메라 X / Z 축 평행 이동
  * r / R       : 원점 기준 카메라 Y축 공전 토글
  * y / Y       : 점광원 공전 토글
  * m           : 조명(lighting) On/Off
  * c           : 점광원 색상 순환 (흰/빨/초/파)
  * o           : 바닥·탱크 팔레트 색상 순환
  * q / Esc     : 종료
================================================================================
*/
"""

parts = [preamble]

for p in header_files:
    raw = p.read_text(encoding="utf-8-sig")
    cleaned = strip_includes_and_pragmas(raw)
    cleaned = transform_gl_glfw(cleaned, p.name)
    parts.append(f"// --- {p.name} ---\n{cleaned}\n")
    if p.name == "ModelLoader.h":
        parts.append(native_replacement)

for p in cpp_files:
    raw = p.read_text(encoding="utf-8-sig")
    cleaned = strip_includes_and_pragmas(raw)
    cleaned = transform_gl_glfw(cleaned, p.name)
    parts.append(f"// --- {p.name} ---\n{cleaned}\n")

parts.append(guide_comment.lstrip("\n"))

combined = "\n".join(parts)
# Collapse 3+ consecutive newlines to 2 newlines for clean compression
combined = re.sub(r'\n{3,}', '\n\n', combined)

out_file = out_dir / "gllib.cpp"
out_file.write_text(combined, encoding="utf-8-sig")
print(f"Wrote {out_file} ({len(combined.splitlines())} lines, {len(combined)} bytes)")
