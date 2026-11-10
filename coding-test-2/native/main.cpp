#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#define GLFW_INCLUDE_NONE
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

// No project Framework.h: context, GPU buffers, GLSL and uniforms are written here.
constexpr double pi = 3.14159265358979323846;
struct Vertex { glm::vec3 position, normal; };
struct Mesh { GLuint vao = 0, vbo = 0; GLsizei count = 0; GLenum mode = GL_TRIANGLES; };
// @debug hook state
struct State {
    double lightDegrees = 56.309932, objectDegrees = 0;
    float radius = 3.605551f, lightHeight = 4, gain = 1;
    int lightDirection = 0, cameraDirection = 0, objectDirection = 0, shape = 0, color = 0, studyView = 0;
    bool lighting = true, paused = false;
    glm::vec3 camera{6, 4, 8}, target{0}, position{0}, scale{2, 1, .5f}, pivot{0};
    std::array<bool, 4> move{};
};
int toggleDirection(int current, int desired) { return current == desired ? 0 : desired; }
std::filesystem::path executableDirectory() {
    std::vector<wchar_t> buffer(32768);
    const DWORD size = GetModuleFileNameW(nullptr, buffer.data(), DWORD(buffer.size()));
    if (!size || size == buffer.size()) throw std::runtime_error("Executable path unavailable.");
    return std::filesystem::path(std::wstring(buffer.data(), size)).parent_path();
}
std::string readText(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);
    if (!input) throw std::runtime_error("Cannot read shader: " + path.u8string());
    std::ostringstream out; out << input.rdbuf(); auto text = out.str();
    if (text.compare(0, 3, "\xEF\xBB\xBF") == 0) text.erase(0, 3);
    return text;
}
GLuint compile(GLenum kind, const std::filesystem::path& path) {
    const auto source = readText(path); const char* data = source.c_str();
    const auto shader = glCreateShader(kind);
    if (!shader) throw std::runtime_error("glCreateShader failed.");
    glShaderSource(shader, 1, &data, nullptr); glCompileShader(shader);
    GLint ok = 0; glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        GLint size = 0; glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &size);
        std::string log(std::size_t(std::max(size, 1)), '\0');
        glGetShaderInfoLog(shader, GLsizei(log.size()), nullptr, log.data()); glDeleteShader(shader);
        throw std::runtime_error(path.u8string() + "\n" + log);
    }
    return shader;
}
GLuint program(const std::filesystem::path& directory) {
    GLuint v = 0, f = 0, p = 0;
    try {
        v = compile(GL_VERTEX_SHADER, directory / "VertexShader.glsl");
        f = compile(GL_FRAGMENT_SHADER, directory / "FragmentShader.glsl");
        p = glCreateProgram(); if (!p) throw std::runtime_error("glCreateProgram failed.");
        glAttachShader(p, v); glAttachShader(p, f); glLinkProgram(p);
        GLint ok = 0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
        if (!ok) {
            GLint size = 0; glGetProgramiv(p, GL_INFO_LOG_LENGTH, &size);
            std::string log(std::size_t(std::max(size, 1)), '\0');
            glGetProgramInfoLog(p, GLsizei(log.size()), nullptr, log.data());
            throw std::runtime_error("GLSL link failed:\n" + log);
        }
        glDeleteShader(v); glDeleteShader(f); return p;
    } catch (...) { if (v) glDeleteShader(v); if (f) glDeleteShader(f); if (p) glDeleteProgram(p); throw; }
}
void triangle(std::vector<Vertex>& vertices, glm::vec3 a, glm::vec3 b, glm::vec3 c) {
    const auto normal = glm::normalize(glm::cross(b - a, c - a));
    for (const auto p : {a, b, c}) vertices.push_back({p, normal});
}
std::vector<Vertex> cube() {
    const glm::vec3 p[] = {{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f},{.5f,.5f,-.5f},{-.5f,.5f,-.5f},
                          {-.5f,-.5f,.5f},{.5f,-.5f,.5f},{.5f,.5f,.5f},{-.5f,.5f,.5f}};
    const int faces[][4] = {{0,3,2,1},{4,5,6,7},{0,4,7,3},{1,2,6,5},{3,7,6,2},{0,1,5,4}};
    std::vector<Vertex> result;
    for (const auto& f : faces) { triangle(result,p[f[0]],p[f[1]],p[f[2]]); triangle(result,p[f[0]],p[f[2]],p[f[3]]); }
    return result;
}
std::vector<Vertex> pyramid() {
    const glm::vec3 a{-.5f,-.5f,-.5f}, b{.5f,-.5f,-.5f}, c{.5f,-.5f,.5f}, d{-.5f,-.5f,.5f}, top{0,.5f,0};
    std::vector<Vertex> result;
    triangle(result,b,a,top); triangle(result,c,b,top); triangle(result,d,c,top); triangle(result,a,d,top);
    triangle(result,a,c,d); triangle(result,a,b,c); return result;
}
std::vector<Vertex> sphere() {
    std::vector<Vertex> result;
    const auto point = [](int y, int x) {
        const double latitude = -pi / 2 + pi * y / 16, longitude = 2 * pi * x / 24;
        return glm::vec3(float(std::cos(latitude)*std::cos(longitude)),float(std::sin(latitude)),float(std::cos(latitude)*std::sin(longitude)));
    };
    const auto append = [&](glm::vec3 a, glm::vec3 b, glm::vec3 c) {
        for (const auto p : {a,b,c}) result.push_back({p*.5f,p});
    };
    for (int y = 0; y < 16; ++y) for (int x = 0; x < 24; ++x) {
        const auto a=point(y,x), b=point(y,x+1), c=point(y+1,x+1), d=point(y+1,x);
        if (y > 0) append(a,c,b);
        if (y < 15) append(a,d,c);
    }
    return result;
}
Mesh upload(const std::vector<Vertex>& vertices, GLenum mode = GL_TRIANGLES) {
    Mesh mesh; mesh.mode=mode; mesh.count=GLsizei(vertices.size());
    glGenVertexArrays(1,&mesh.vao); glGenBuffers(1,&mesh.vbo); glBindVertexArray(mesh.vao);
    glBindBuffer(GL_ARRAY_BUFFER,mesh.vbo); glBufferData(GL_ARRAY_BUFFER,GLsizeiptr(vertices.size()*sizeof(Vertex)),vertices.data(),GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex,position)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),reinterpret_cast<void*>(offsetof(Vertex,normal)));
    glEnableVertexAttribArray(1); glBindVertexArray(0); return mesh;
}
GLint location(GLuint p, const char* name) {
    const GLint value=glGetUniformLocation(p,name);
    if(value<0) throw std::runtime_error(std::string("Uniform missing or optimized out: ")+name);
    return value;
}
void matrix(GLuint p,const char* name,const glm::mat4& m) { glUniformMatrix4fv(location(p,name),1,GL_FALSE,glm::value_ptr(m)); }
void vector(GLuint p,const char* name,glm::vec3 v) { glUniform3fv(location(p,name),1,glm::value_ptr(v)); }
void scalar(GLuint p,const char* name,float v) { glUniform1f(location(p,name),v); }
void integer(GLuint p,const char* name,int v) { glUniform1i(location(p,name),v); }
void draw(GLuint p,const Mesh& mesh,const glm::mat4& model,glm::vec3 color,bool unlit=false) {
    matrix(p,"model",model);
    const auto normal=glm::transpose(glm::inverse(glm::mat3(model)));
    glUniformMatrix3fv(location(p,"normalMatrix"),1,GL_FALSE,glm::value_ptr(normal));
    vector(p,"objectColor",color); integer(p,"unlit",unlit ? 1 : 0);
    glBindVertexArray(mesh.vao); glDrawArrays(mesh.mode,0,mesh.count);
}

// @debug hook input
void key(GLFWwindow* window,int key,int,int action,int mods) {
    auto& state=*static_cast<State*>(glfwGetWindowUserPointer(window));
    const int keys[]={GLFW_KEY_W,GLFW_KEY_A,GLFW_KEY_S,GLFW_KEY_D};
    for(std::size_t i=0;i<state.move.size();++i) if(key==keys[i]) state.move[i]=action!=GLFW_RELEASE;
    if(action!=GLFW_PRESS) return;
    const int direction=mods&GLFW_MOD_SHIFT ? -1 : 1;
    if(key==GLFW_KEY_ESCAPE || key==GLFW_KEY_Q) glfwSetWindowShouldClose(window,GLFW_TRUE);
    if(key==GLFW_KEY_M) state.lighting=!state.lighting;
    if(key==GLFW_KEY_C) state.color=(state.color+1)%3;
    if(key==GLFW_KEY_N) state.shape=(state.shape+1)%3;
    if(key==GLFW_KEY_Y) state.lightDirection=toggleDirection(state.lightDirection,direction);
    if(key==GLFW_KEY_R) state.cameraDirection=toggleDirection(state.cameraDirection,direction);
    if(key==GLFW_KEY_P) state.objectDirection=state.objectDirection ? 0 : 1;
    if(key==GLFW_KEY_Z) state.radius=std::max(.25f,state.radius+(mods&GLFW_MOD_SHIFT ? .25f : -.25f));
    if(key==GLFW_KEY_F2) state.studyView=(state.studyView+1)%4;
    if(key==GLFW_KEY_G) state.gain=state.gain==1 ? 1.5f : 1;
    if(key==GLFW_KEY_SPACE) state.paused=!state.paused;
    if(key==GLFW_KEY_HOME) state=State{};
}
// @debug hook update
void update(State& state,float dt) {
    if(state.paused) return;
    state.lightDegrees=std::remainder(state.lightDegrees+state.lightDirection*45*dt,360.0);
    state.objectDegrees=std::remainder(state.objectDegrees+state.objectDirection*35*dt,360.0);
    auto move=glm::vec3(int(state.move[3])-int(state.move[1]),0,int(state.move[2])-int(state.move[0]));
    if(glm::length(move)>0) state.camera+=glm::normalize(move)*3.f*dt;
    const auto orbit=glm::rotate(glm::mat4(1),glm::radians(state.cameraDirection*35.f*dt),glm::vec3(0,1,0));
    state.camera=state.target+glm::vec3(orbit*glm::vec4(state.camera-state.target,0));
}
int main() {
    SetConsoleOutputCP(CP_UTF8);
    GLFWwindow* window=nullptr; bool initialized=false, gpu=false; GLuint shader=0;
    std::vector<Mesh> meshes; int result=0;
    try {
        if(!glfwInit()) throw std::runtime_error("glfwInit failed."); initialized=true;
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3); glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
        glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
        window=glfwCreateWindow(1100,760,"Native OpenGL - Coding Test 2",nullptr,nullptr);
        if(!window) throw std::runtime_error("OpenGL 3.3 core window creation failed.");
        glfwMakeContextCurrent(window); glewExperimental=GL_TRUE;
        if(glewInit()!=GLEW_OK) throw std::runtime_error("glewInit failed."); gpu=true;
        while(glGetError()!=GL_NO_ERROR) {} // GLEW core-context initialization can leave GL_INVALID_ENUM.
        glfwSwapInterval(1); glEnable(GL_DEPTH_TEST);
        State state; glfwSetWindowUserPointer(window,&state); glfwSetKeyCallback(window,key);
        glfwSetWindowFocusCallback(window,[](GLFWwindow* w,int focused) {
            if(!focused) static_cast<State*>(glfwGetWindowUserPointer(w))->move.fill(false);
        });
        // @debug hook shader
        shader=program(executableDirectory()/"shaders");
        meshes.push_back(upload(cube())); meshes.push_back(upload(pyramid())); meshes.push_back(upload(sphere()));
        std::vector<Vertex> circle(64);
        meshes.push_back(upload(circle,GL_LINE_LOOP));
        float previousRadius=-1,previousHeight=0;
        double previous=glfwGetTime();
        std::cout<<"Native: n box/pyramid/sphere | m light | c color | y/Y light orbit | z/Z radius\n"
                    "w/a/s/d camera XZ | r/R camera orbit | p object spin | F2 shader view | g gain\n"
                    "Space pause | Home reset | q/Esc quit. Edit main.cpp and shaders directly.\n";
        while(!glfwWindowShouldClose(window)) {
            glfwPollEvents(); const double now=glfwGetTime();
            const float dt=float(std::clamp(now-previous,0.0,.05)); previous=now;
            update(state,dt);
            int width=0,height=0; glfwGetFramebufferSize(window,&width,&height);
            if(width<=0 || height<=0) { glfwWaitEventsTimeout(.05); continue; }
            glViewport(0,0,width,height); glClearColor(.08f,.10f,.14f,1); glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
            glUseProgram(shader);
            const auto view=glm::lookAt(state.camera,state.target,glm::vec3(0,1,0));
            const auto projection=glm::perspective(glm::radians(45.f),float(width)/height,.1f,100.f);
            matrix(shader,"view",view); matrix(shader,"projection",projection);
            const auto point=[&](double degrees) { const double a=degrees*pi/180;
                return glm::vec3(state.radius*float(std::cos(a)),state.lightHeight,state.radius*float(std::sin(a))); };
            const glm::vec3 colors[]={{1,1,1},{1,.3f,.2f},{.2f,.5f,1}};
            // @debug hook uniform
            vector(shader,"lightPosition",point(state.lightDegrees)); vector(shader,"lightColor",colors[state.color]);
            vector(shader,"viewPosition",state.camera); integer(shader,"lighting",state.lighting ? 1 : 0);
            integer(shader,"studyView",state.studyView); scalar(shader,"Ka",.25f); scalar(shader,"Kd",1);
            scalar(shader,"Ks",.8f); scalar(shader,"shininess",64); scalar(shader,"attenuation",.02f); scalar(shader,"gain",state.gain);
            // Model-local pivot: world = T(position) * R * S * T(-pivot).
            auto model=glm::rotate(glm::translate(glm::mat4(1),state.position),glm::radians(float(state.objectDegrees)),glm::vec3(0,1,0));
            model=glm::scale(model,state.scale)*glm::translate(glm::mat4(1),-state.pivot);
            draw(shader,meshes[std::size_t(state.shape)],model,{.3f,.7f,.9f});
            draw(shader,meshes[2],glm::translate(glm::mat4(1),glm::vec3(-2,0,0)),{.9f,.4f,.2f});
            draw(shader,meshes[2],glm::translate(glm::mat4(1),glm::vec3(2.5f,0,0)),{.3f,.9f,.4f});
            draw(shader,meshes[0],glm::scale(glm::translate(glm::mat4(1),glm::vec3(0,-1.2f,0)),glm::vec3(10,.2f,10)),{.25f,.3f,.3f});
            draw(shader,meshes[0],glm::scale(glm::translate(glm::mat4(1),point(state.lightDegrees)),glm::vec3(.15f)),colors[state.color],true);
            if(previousRadius!=state.radius || previousHeight!=state.lightHeight) {
                for(std::size_t i=0;i<circle.size();++i) circle[i]={point(360.0*i/circle.size()),{0,1,0}};
                glBindBuffer(GL_ARRAY_BUFFER,meshes[3].vbo);
                glBufferSubData(GL_ARRAY_BUFFER,0,GLsizeiptr(circle.size()*sizeof(Vertex)),circle.data());
                previousRadius=state.radius; previousHeight=state.lightHeight;
            }
            draw(shader,meshes[3],glm::mat4(1),{.35f,.4f,.45f},true);
            glfwSwapBuffers(window);
        }
    } catch(const std::exception& error) { std::cerr<<error.what()<<'\n'; result=1; }
    if(gpu) {
        for(const auto& mesh:meshes) { glDeleteBuffers(1,&mesh.vbo); glDeleteVertexArrays(1,&mesh.vao); }
        if(shader) glDeleteProgram(shader);
    }
    if(window) glfwDestroyWindow(window);
    if(initialized) glfwTerminate();
    return result;
}
