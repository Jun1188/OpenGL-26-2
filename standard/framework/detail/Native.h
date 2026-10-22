#pragma once
#ifndef _WIN32
#error This bundled framework targets Windows x64.
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "../dependencies/glew-2.3.1/include/GL/glew.h"
#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#include "../dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#include <filesystem>
#include <string>
#include <stdexcept>
#include <vector>

namespace graphics::detail {
static_assert(sizeof(void*) == 8, "Bundled DLLs require x64.");
inline std::filesystem::path executableDirectory() {
    std::wstring path(32768, L'\0');
    const DWORD size = GetModuleFileNameW(nullptr, path.data(), DWORD(path.size()));
    if (!size || size >= path.size()) throw std::runtime_error("Cannot locate the executable directory.");
    path.resize(size);
    return std::filesystem::path(path).parent_path();
}
inline std::filesystem::path relativePathInside(const std::filesystem::path& root,
                                                const std::filesystem::path& relative) {
    if(relative.empty()||relative.is_absolute()||relative.has_root_name()||relative.has_root_directory())
        throw std::runtime_error("Use a relative path inside the project.");
    const auto base=std::filesystem::weakly_canonical(root);
    const auto full=std::filesystem::weakly_canonical(base/relative);
    auto a=base.begin(),b=full.begin();
    for(;a!=base.end();++a,++b)if(b==full.end()||*a!=*b)
        throw std::runtime_error("Path leaves the project directory.");
    return full;
}
inline std::filesystem::path frameworkDirectory() {
    return executableDirectory()/"framework";
}
class Module {
    HMODULE handle_ = nullptr;
public:
    Module() = default;
    Module(const Module&) = delete;
    Module& operator=(const Module&) = delete;
    ~Module() { if (handle_) FreeLibrary(handle_); }
    void open(const std::filesystem::path& path) {
        if (handle_) return;
        handle_ = LoadLibraryExW(path.c_str(), nullptr, LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR | LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
        if (!handle_) throw std::runtime_error("Cannot load bundled runtime DLL (Win32 error " + std::to_string(GetLastError()) + ").");
    }
    template<class T> T symbol(const char* name) const {
        const auto address = ::GetProcAddress(handle_, name);
        if (!address) throw std::runtime_error(std::string("Missing runtime export: ") + name);
        return reinterpret_cast<T>(address);
    }
};
struct GlfwApi {
    Module module;
    bool ready = false;
    decltype(&::glfwInit) Init = nullptr;
    decltype(&::glfwTerminate) Terminate = nullptr;
    decltype(&::glfwWindowHint) WindowHint = nullptr;
    decltype(&::glfwCreateWindow) createWindow = nullptr;
    decltype(&::glfwDestroyWindow) DestroyWindow = nullptr;
    decltype(&::glfwMakeContextCurrent) MakeContextCurrent = nullptr;
    decltype(&::glfwSwapInterval) SwapInterval = nullptr;
    decltype(&::glfwSetInputMode) SetInputMode = nullptr;
    decltype(&::glfwSetWindowUserPointer) SetWindowUserPointer = nullptr;
    decltype(&::glfwGetWindowUserPointer) GetWindowUserPointer = nullptr;
    decltype(&::glfwSetKeyCallback) SetKeyCallback = nullptr;
    decltype(&::glfwSetWindowFocusCallback) SetWindowFocusCallback = nullptr;
    decltype(&::glfwSetMouseButtonCallback) SetMouseButtonCallback = nullptr;
    decltype(&::glfwSetCursorPosCallback) SetCursorPosCallback = nullptr;
    decltype(&::glfwGetCursorPos) GetCursorPos = nullptr;
    decltype(&::glfwGetWindowSize) GetWindowSize = nullptr;
    decltype(&::glfwGetFramebufferSize) GetFramebufferSize = nullptr;
    decltype(&::glfwWindowShouldClose) WindowShouldClose = nullptr;
    decltype(&::glfwSetWindowShouldClose) SetWindowShouldClose = nullptr;
    decltype(&::glfwGetTime) GetTime = nullptr;
    decltype(&::glfwPollEvents) PollEvents = nullptr;
    decltype(&::glfwWaitEvents) WaitEvents = nullptr;
    decltype(&::glfwSwapBuffers) SwapBuffers = nullptr;
    void load(const std::filesystem::path& root = std::filesystem::path{}) {
        if (ready) return;
        module.open((root.empty() ? frameworkDirectory() : std::filesystem::absolute(root)) / "dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll");
        Init = module.symbol<decltype(Init)>("glfwInit");
        Terminate = module.symbol<decltype(Terminate)>("glfwTerminate");
        WindowHint = module.symbol<decltype(WindowHint)>("glfwWindowHint");
        createWindow = module.symbol<decltype(createWindow)>("glfwCreateWindow");
        DestroyWindow = module.symbol<decltype(DestroyWindow)>("glfwDestroyWindow");
        MakeContextCurrent = module.symbol<decltype(MakeContextCurrent)>("glfwMakeContextCurrent");
        SwapInterval = module.symbol<decltype(SwapInterval)>("glfwSwapInterval");
        SetInputMode = module.symbol<decltype(SetInputMode)>("glfwSetInputMode");
        SetWindowUserPointer = module.symbol<decltype(SetWindowUserPointer)>("glfwSetWindowUserPointer");
        GetWindowUserPointer = module.symbol<decltype(GetWindowUserPointer)>("glfwGetWindowUserPointer");
        SetKeyCallback = module.symbol<decltype(SetKeyCallback)>("glfwSetKeyCallback");
        SetWindowFocusCallback = module.symbol<decltype(SetWindowFocusCallback)>("glfwSetWindowFocusCallback");
        SetMouseButtonCallback = module.symbol<decltype(SetMouseButtonCallback)>("glfwSetMouseButtonCallback");
        SetCursorPosCallback = module.symbol<decltype(SetCursorPosCallback)>("glfwSetCursorPosCallback");
        GetCursorPos = module.symbol<decltype(GetCursorPos)>("glfwGetCursorPos");
        GetWindowSize = module.symbol<decltype(GetWindowSize)>("glfwGetWindowSize");
        GetFramebufferSize = module.symbol<decltype(GetFramebufferSize)>("glfwGetFramebufferSize");
        WindowShouldClose = module.symbol<decltype(WindowShouldClose)>("glfwWindowShouldClose");
        SetWindowShouldClose = module.symbol<decltype(SetWindowShouldClose)>("glfwSetWindowShouldClose");
        GetTime = module.symbol<decltype(GetTime)>("glfwGetTime");
        PollEvents = module.symbol<decltype(PollEvents)>("glfwPollEvents");
        WaitEvents = module.symbol<decltype(WaitEvents)>("glfwWaitEvents");
        SwapBuffers = module.symbol<decltype(SwapBuffers)>("glfwSwapBuffers");
        ready = true;
    }
};
inline GlfwApi& glfw() { static GlfwApi api; return api; }

struct GlApi {
    Module module;
    using WglGetProc = PROC (WINAPI*)(LPCSTR);
    WglGetProc wglGetProc = nullptr;
    decltype(&::glClearColor) ClearColor = nullptr;
    decltype(&::glClear) Clear = nullptr;
    decltype(&::glEnable) Enable = nullptr;
    decltype(&::glDisable) Disable = nullptr;
    decltype(&::glDepthMask) DepthMask = nullptr;
    decltype(&::glColorMask) ColorMask = nullptr;
    decltype(&::glDepthFunc) DepthFunc = nullptr;
    decltype(&::glViewport) Viewport = nullptr;
    decltype(&::glPolygonMode) PolygonMode = nullptr;
    decltype(&::glDrawElements) DrawElements = nullptr;
    decltype(&::glGenTextures) GenTextures = nullptr;
    decltype(&::glBindTexture) BindTexture = nullptr;
    decltype(&::glTexParameteri) TexParameteri = nullptr;
    decltype(&::glTexImage2D) TexImage2D = nullptr;
    decltype(&::glPixelStorei) PixelStorei = nullptr;
    decltype(&::glDeleteTextures) DeleteTextures = nullptr;
    decltype(&::glGetIntegerv) GetIntegerv = nullptr;
    decltype(&::glGetError) GetError = nullptr;
    decltype(&::glLineWidth) LineWidth = nullptr;
    PFNGLGENVERTEXARRAYSPROC GenVertexArrays = nullptr;
    PFNGLBINDVERTEXARRAYPROC BindVertexArray = nullptr;
    PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays = nullptr;
    PFNGLGENBUFFERSPROC GenBuffers = nullptr;
    PFNGLBINDBUFFERPROC BindBuffer = nullptr;
    PFNGLBUFFERDATAPROC BufferData = nullptr;
    PFNGLDELETEBUFFERSPROC DeleteBuffers = nullptr;
    PFNGLVERTEXATTRIBPOINTERPROC VertexAttribPointer = nullptr;
    PFNGLENABLEVERTEXATTRIBARRAYPROC EnableVertexAttribArray = nullptr;
    PFNGLCREATESHADERPROC CreateShader = nullptr;
    PFNGLSHADERSOURCEPROC ShaderSource = nullptr;
    PFNGLCOMPILESHADERPROC CompileShader = nullptr;
    PFNGLGETSHADERIVPROC GetShaderiv = nullptr;
    PFNGLGETSHADERINFOLOGPROC GetShaderInfoLog = nullptr;
    PFNGLDELETESHADERPROC DeleteShader = nullptr;
    PFNGLCREATEPROGRAMPROC CreateProgram = nullptr;
    PFNGLATTACHSHADERPROC AttachShader = nullptr;
    PFNGLLINKPROGRAMPROC LinkProgram = nullptr;
    PFNGLGETPROGRAMIVPROC GetProgramiv = nullptr;
    PFNGLGETPROGRAMINFOLOGPROC GetProgramInfoLog = nullptr;
    PFNGLDETACHSHADERPROC DetachShader = nullptr;
    PFNGLDELETEPROGRAMPROC DeleteProgram = nullptr;
    PFNGLGETUNIFORMLOCATIONPROC GetUniformLocation = nullptr;
    PFNGLUSEPROGRAMPROC UseProgram = nullptr;
    PFNGLUNIFORMMATRIX4FVPROC UniformMatrix4fv = nullptr;
    PFNGLUNIFORMMATRIX3FVPROC UniformMatrix3fv = nullptr;
    PFNGLUNIFORM3FPROC Uniform3f = nullptr;
    PFNGLUNIFORM1IPROC Uniform1i = nullptr;
    PFNGLACTIVETEXTUREPROC ActiveTexture = nullptr;
    template<class T> T resolve(const char* name) {
        PROC p = wglGetProc(name);
        if (!p || p == reinterpret_cast<PROC>(1) || p == reinterpret_cast<PROC>(2)
            || p == reinterpret_cast<PROC>(3) || p == reinterpret_cast<PROC>(-1))
            p = module.symbol<PROC>(name);
        return reinterpret_cast<T>(p);
    }
    void load() {
        std::wstring directory(32768, L'\0');
        const UINT size = GetSystemDirectoryW(directory.data(), UINT(directory.size()));
        if (!size || size >= directory.size()) throw std::runtime_error("Cannot find Windows system directory.");
        directory.resize(size);
        module.open(std::filesystem::path(directory) / "opengl32.dll");
        wglGetProc = module.symbol<WglGetProc>("wglGetProcAddress");
        ClearColor = resolve<decltype(ClearColor)>("glClearColor");
        Clear = resolve<decltype(Clear)>("glClear");
        Enable = resolve<decltype(Enable)>("glEnable");
        Disable = resolve<decltype(Disable)>("glDisable");
        DepthMask = resolve<decltype(DepthMask)>("glDepthMask");
        ColorMask = resolve<decltype(ColorMask)>("glColorMask");
        DepthFunc = resolve<decltype(DepthFunc)>("glDepthFunc");
        Viewport = resolve<decltype(Viewport)>("glViewport");
        PolygonMode = resolve<decltype(PolygonMode)>("glPolygonMode");
        DrawElements = resolve<decltype(DrawElements)>("glDrawElements");
        GenTextures = resolve<decltype(GenTextures)>("glGenTextures");
        BindTexture = resolve<decltype(BindTexture)>("glBindTexture");
        TexParameteri = resolve<decltype(TexParameteri)>("glTexParameteri");
        TexImage2D = resolve<decltype(TexImage2D)>("glTexImage2D");
        PixelStorei = resolve<decltype(PixelStorei)>("glPixelStorei");
        DeleteTextures = resolve<decltype(DeleteTextures)>("glDeleteTextures");
        GetIntegerv = resolve<decltype(GetIntegerv)>("glGetIntegerv");
        GetError = resolve<decltype(GetError)>("glGetError");
        LineWidth = resolve<decltype(LineWidth)>("glLineWidth");
        GenVertexArrays = resolve<decltype(GenVertexArrays)>("glGenVertexArrays");
        BindVertexArray = resolve<decltype(BindVertexArray)>("glBindVertexArray");
        DeleteVertexArrays = resolve<decltype(DeleteVertexArrays)>("glDeleteVertexArrays");
        GenBuffers = resolve<decltype(GenBuffers)>("glGenBuffers");
        BindBuffer = resolve<decltype(BindBuffer)>("glBindBuffer");
        BufferData = resolve<decltype(BufferData)>("glBufferData");
        DeleteBuffers = resolve<decltype(DeleteBuffers)>("glDeleteBuffers");
        VertexAttribPointer = resolve<decltype(VertexAttribPointer)>("glVertexAttribPointer");
        EnableVertexAttribArray = resolve<decltype(EnableVertexAttribArray)>("glEnableVertexAttribArray");
        CreateShader = resolve<decltype(CreateShader)>("glCreateShader");
        ShaderSource = resolve<decltype(ShaderSource)>("glShaderSource");
        CompileShader = resolve<decltype(CompileShader)>("glCompileShader");
        GetShaderiv = resolve<decltype(GetShaderiv)>("glGetShaderiv");
        GetShaderInfoLog = resolve<decltype(GetShaderInfoLog)>("glGetShaderInfoLog");
        DeleteShader = resolve<decltype(DeleteShader)>("glDeleteShader");
        CreateProgram = resolve<decltype(CreateProgram)>("glCreateProgram");
        AttachShader = resolve<decltype(AttachShader)>("glAttachShader");
        LinkProgram = resolve<decltype(LinkProgram)>("glLinkProgram");
        GetProgramiv = resolve<decltype(GetProgramiv)>("glGetProgramiv");
        GetProgramInfoLog = resolve<decltype(GetProgramInfoLog)>("glGetProgramInfoLog");
        DetachShader = resolve<decltype(DetachShader)>("glDetachShader");
        DeleteProgram = resolve<decltype(DeleteProgram)>("glDeleteProgram");
        GetUniformLocation = resolve<decltype(GetUniformLocation)>("glGetUniformLocation");
        UseProgram = resolve<decltype(UseProgram)>("glUseProgram");
        UniformMatrix4fv = resolve<decltype(UniformMatrix4fv)>("glUniformMatrix4fv");
        UniformMatrix3fv = resolve<decltype(UniformMatrix3fv)>("glUniformMatrix3fv");
        Uniform3f = resolve<decltype(Uniform3f)>("glUniform3f");
        Uniform1i = resolve<decltype(Uniform1i)>("glUniform1i");
        ActiveTexture = resolve<decltype(ActiveTexture)>("glActiveTexture");
    }
};
inline GlApi& gl() { static GlApi api; return api; }
} // namespace graphics::detail
