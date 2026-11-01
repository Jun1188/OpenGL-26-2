# Unicode 초기화 오류와 VS 버전 안내

## 관찰과 원인

사용자 실습20 로그: No mapping for the Unicode character exists in the target multi-byte code page. Application::run catch에서 종료코드1. 소스 경로에 한글이 있다. /utf-8 /FC에서 __FILE__은 UTF-8 좁은 문자열이며 Renderer::initShader가 std::filesystem::path(__FILE__)로 네이티브 경로를 구성했다. 이 코드 페이지 변환은 명시적 UTF-8 디코딩이 아니다. 실제 실행으로 재현하지는 않았으나 초기화 순서와 예외에 맞는 결함을 확인했다.

## 수정

__FILE__을 std::filesystem::u8path로 변환한 후 내장 셰이더의 Shaders.h 위치를 만든다. 셰이더 초기화 전 Operation을 갱신해 WGL 연결 단계가 오류 경로로 남지 않게 한다. standard와 실습15~23의 동일 사본에 반영했다. 아래 실제 파일 전체의 before/after는 나머지 배포 사본에도 동일하게 적용했다.

## VS 호환성

사용자 추가 설명: VS 2022로 매핑됐다는 경고였고 정상 실행됐다. 이 경고는 이번 Unicode 실행 실패의 근거가 아니다. 정확한 경고 문구는 받지 않았다. 기존 .sln의 VisualStudioVersion=17은 VS 2022 호환 형식이며 최신 IDE 전용으로 변경하지 않는다. .slnx 대신 .sln을 열 수 있다. 명시한 PlatformToolset을 유지하고 자동 선택은 설치된 기본 도구 집합을 먼저 사용한다. 없으면 v143/v145를 확인한다. 설치 경로·VS 버전·선택 도구 집합을 빌드 출력에 표시하고 없는 도구 집합의 설치/수정 조치를 안내한다. .vcxproj.user나 로컬 VS 설정을 덮어쓰지 않았다.

## 사용자 확인

1. 한글 경로를 유지한 상태에서 각 실습의 .sln을 연다.
2. Debug/Release x64를 다시 빌드한 뒤 실행한다. 이전 EXE에는 수정이 반영되지 않는다.
3. 20의 창 생성과 b 동작, 21의 e/v, 22의 3뷰, 23의 보행을 확인한다.
4. 같은 예외가 남으면 새 Source/Operation 전체 로그를 확인한다.

빌드·실행·테스트·검증기·ZIP 생성은 수행하지 않았다. 소스 읽기와 사본 반영만 완료했다. 사용자 main.cpp 변경은 보존했다. 기존 SKILL의 호출부 추적·공통 원인 수정·UTF-8 경계 규칙을 적용했으며 전역 스킬 자체는 수정하지 않았다. 재발 방지 효과와 실제 실행 성공은 사용자 확인이 필요하다.

## framework/detail/Renderer.inl

### before

```text
#pragma once
#include "Shaders.h"
#include "../dependencies/glm/glm/gtc/type_ptr.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace graphics {
inline std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream input(path,std::ios::binary);
    if(!input) return {};
    std::ostringstream buffer;buffer<<input.rdbuf();auto text=buffer.str();
    if(text.compare(0,3,"\xEF\xBB\xBF")==0) text.erase(0,3);
    return text;
}
namespace detail {
inline GLuint compileShader(GLenum kind,const std::string& sourceText,const std::filesystem::path& sourcePath) {
    Diagnostics::trace(PipelineStage::Shader, "compileShader");
    auto& api=gl();const auto shader=api.CreateShader(kind);
    if(!shader) { Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, "glCreateShader", "Shader allocation failed."); return 0; }
    const char* text=sourceText.c_str();api.ShaderSource(shader,1,&text,nullptr);api.CompileShader(shader);
    GLint success=0;api.GetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success) {
        GLint length=0;api.GetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
        std::string log(std::max(length,1), '\0');api.GetShaderInfoLog(shader,GLsizei(log.size()),nullptr,log.data());
        Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, kind==GL_VERTEX_SHADER?"vertex shader":"fragment shader", "GLSL file="+sourcePath.u8string()+"\n"+log,
            "Read the compiler line number; check GLSL version, declarations and shader inputs/outputs.");
        api.DeleteShader(shader);return 0;
    }
    return shader;
}
struct GpuVertex { float position[3], color[3], uv[2], normal[3]; };
inline GLuint uploadTexture(const TextureImage& image,bool repeat) {
    auto& api=gl();GLuint id=0;api.GenTextures(1,&id);
    if(!id){Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Asset,"glGenTextures","Texture allocation failed.");return 0;}
    api.ActiveTexture(GL_TEXTURE0);api.BindTexture(GL_TEXTURE_2D,id);
    GLint previousAlignment=4;api.GetIntegerv(GL_UNPACK_ALIGNMENT,&previousAlignment);
    api.PixelStorei(GL_UNPACK_ALIGNMENT,1);
    api.TexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,GLsizei(image.width),GLsizei(image.height),0,GL_RGBA,GL_UNSIGNED_BYTE,image.rgba.data());
    api.PixelStorei(GL_UNPACK_ALIGNMENT,previousAlignment);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
    if(!checkGlAt(__FILE__,__LINE__,PipelineStage::Asset, "uploadTexture")) {api.DeleteTextures(1,&id);return 0;}
    return id;
}
} // namespace detail

inline bool Renderer::initShader(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    auto& gl=detail::gl();
    const auto vertexText=vertexPath.empty()?std::string(detail::vertexShader):readTextFile(vertexPath);
    const auto fragmentText=fragmentPath.empty()?std::string(detail::fragmentShader):readTextFile(fragmentPath);
    if(vertexText.empty()||fragmentText.empty()) return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, "Renderer::initShader",
        "Shader source is empty or unreadable: vertex=" + vertexPath.u8string() + "; fragment=" + fragmentPath.u8string(),
        "Use embedded shaders (empty shaderDirectory), or copy both GLSL files to the configured project-relative directory.");
    const auto vertex=detail::compileShader(GL_VERTEX_SHADER,vertexText,vertexPath.empty()?std::filesystem::path(__FILE__).parent_path()/"Shaders.h":vertexPath);if(!vertex)return false;
    const auto fragment=detail::compileShader(GL_FRAGMENT_SHADER,fragmentText,fragmentPath.empty()?std::filesystem::path(__FILE__).parent_path()/"Shaders.h":fragmentPath);
    if(!fragment){gl.DeleteShader(vertex);return false;}
    shaderProgramID_=gl.CreateProgram();
    if(!shaderProgramID_){gl.DeleteShader(vertex);gl.DeleteShader(fragment);return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader,"glCreateProgram","Program allocation failed.");}
    gl.AttachShader(shaderProgramID_,vertex);gl.AttachShader(shaderProgramID_,fragment);gl.LinkProgram(shaderProgramID_);
    gl.DeleteShader(vertex);gl.DeleteShader(fragment);
    GLint success=0;gl.GetProgramiv(shaderProgramID_,GL_LINK_STATUS,&success);
    if(!success) {
        GLint size=0;gl.GetProgramiv(shaderProgramID_,GL_INFO_LOG_LENGTH,&size);
        std::string log(std::max(size,1),'\0');gl.GetProgramInfoLog(shaderProgramID_,GLsizei(log.size()),nullptr,log.data());
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, "glLinkProgram", log, "Match vertex outputs and fragment inputs; inspect the linker log.");
    }
    mvpLocation_=gl.GetUniformLocation(shaderProgramID_,"uMVP");
    modelLocation_=gl.GetUniformLocation(shaderProgramID_,"uModel");
    normalLocation_=gl.GetUniformLocation(shaderProgramID_,"uNormalMatrix");
    textureLocation_=gl.GetUniformLocation(shaderProgramID_,"uTexture");
    hasTextureLocation_=gl.GetUniformLocation(shaderProgramID_,"uHasTexture");
    lightingLocation_=gl.GetUniformLocation(shaderProgramID_,"uLighting");
    lightDirectionLocation_=gl.GetUniformLocation(shaderProgramID_,"uLightDirection");
    lightColorLocation_=gl.GetUniformLocation(shaderProgramID_,"uLightColor");
    overrideLocation_=gl.GetUniformLocation(shaderProgramID_,"uOverrideColor");
    outlineColorLocation_=gl.GetUniformLocation(shaderProgramID_,"uOutlineColor");
    tintLocation_=gl.GetUniformLocation(shaderProgramID_,"uTint");
    return mvpLocation_>=0 || Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, "glGetUniformLocation", "Required uMVP uniform is missing or optimized away.");
}
inline bool Renderer::initialize(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    detail::gl().load();destroy();
    if(!initShader(vertexPath,fragmentPath)){destroy();return false;}
    auto& gl=detail::gl();gl.GenVertexArrays(1,&vao_);gl.GenBuffers(1,&vbo_);gl.GenBuffers(1,&ebo_);
    if(!vao_||!vbo_||!ebo_){destroy();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput, "Renderer::initialize", "VAO/VBO/EBO allocation failed.");}
    gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    using V=detail::GpuVertex;
    gl.VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,position)));
    gl.VertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,color)));
    gl.VertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,uv)));
    gl.VertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,normal)));
    for(GLuint i=0;i<4;++i)gl.EnableVertexAttribArray(i);
    gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);return detail::checkGlAt(__FILE__,__LINE__,PipelineStage::VertexInput, "Renderer::initialize vertex layout");
}
inline bool Renderer::upload(const Scene& scene,bool textureMode) {
    Diagnostics::trace(PipelineStage::VertexInput, "Renderer::upload");
    std::vector<detail::GpuVertex> vertices;
    std::vector<GLuint> indices;std::vector<DrawRange> ranges;
    TextureMap stagedTextures;auto& gl=detail::gl();
    auto cleanup=[&](){for(const auto& item:stagedTextures)gl.DeleteTextures(1,&item.second);};
    try {
        for(auto id:scene.getIds()) {
            const auto* c=scene.getComponent(id);if(c&&c->transformOnly)continue;
            if(!c||!DiagramSystem::validPolyhedron(c->polyhedron)){cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput, "Renderer::upload", "Missing component or invalid mesh, indices, colors, UVs or normals.", "Inspect the entity's mesh data before GPU upload.", id);}
            for(std::size_t faceIndex=0;faceIndex<c->polyhedron.faces.size();++faceIndex) {
                const auto& face=c->polyhedron.faces[faceIndex];
                if(vertices.size()+face.localVertices.size()>std::numeric_limits<GLuint>::max()) {cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::upload","Vertex count exceeds the GPU index range.");}
                const auto first=GLuint(vertices.size());
                Mesh normals=face;if(normals.vertexNormals.empty())recalculateNormals(normals);
                for(std::size_t i=0;i<face.localVertices.size();++i) {
                    const auto p=face.localVertices[i];const auto n=normals.vertexNormals[i];
                    auto color=face.vertexColors.empty()?face.color:face.vertexColors[i];
                    color.red*=face.material.color.red;color.green*=face.material.color.green;color.blue*=face.material.color.blue;
                    const auto uv=face.texcoords.empty()?glm::vec2(0):face.texcoords[i];
                    vertices.push_back({{p.x,p.y,p.z},{color.red,color.green,color.blue},{uv.x,uv.y},{n.x,n.y,n.z}});
                }
                const auto triangles=detail::triangles(face);
                const auto outline=meshOutline(face,face.outline==OutlineMode::None?OutlineMode::FeatureEdges:face.outline);
                if(triangles.size()>std::numeric_limits<GLsizei>::max()||outline.size()>std::numeric_limits<GLsizei>::max()){cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::upload","Draw count exceeds GLsizei range.");}
                DrawRange range;range.entityId=id;range.faceIndex=faceIndex;range.firstIndex=indices.size();range.count=GLsizei(triangles.size());
                for(auto index:triangles)indices.push_back(first+index);
                range.outlineFirstIndex=indices.size();range.outlineCount=GLsizei(outline.size());
                for(auto index:outline)indices.push_back(first+index);
                ranges.push_back(range);
                if(textureMode&&face.material.useTexture&&stagedTextures.find(face.material.texture)==stagedTextures.end()) {
                    if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Asset, "before uploadTexture")){cleanup();return false;}
                    const auto texture=detail::uploadTexture(*face.material.texture,face.material.repeatTexture);
                    if(!texture){cleanup();return false;}
                    try{stagedTextures.emplace(face.material.texture,texture);}catch(...){gl.DeleteTextures(1,&texture);throw;}
                }
            }
        }
        for(std::size_t lineIndex=0;lineIndex<scene.getLines().size();++lineIndex) {
            const auto& line=scene.getLines()[lineIndex];
            if(vertices.size()>std::numeric_limits<GLuint>::max()-2){cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::upload lines","Vertex count exceeds the GPU index range.");}
            const auto first=GLuint(vertices.size());
            for(auto p:{line.start,line.end})vertices.push_back({{p.x,p.y,p.z},{line.color.red,line.color.green,line.color.blue},{0,0},{0,0,0}});
            DrawRange range;range.lineId=lineIndex;range.firstIndex=indices.size();range.count=2;
            ranges.push_back(range);indices.insert(indices.end(),{first,first+1});
        }
        if(vertices.size()>std::size_t(std::numeric_limits<GLsizeiptr>::max())/sizeof(detail::GpuVertex)
            ||indices.size()>std::size_t(std::numeric_limits<GLsizeiptr>::max())/sizeof(GLuint)){cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::upload","Buffer byte size exceeds GLsizeiptr range.");}
        if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::VertexInput, "before buffer upload")){cleanup();return false;}
        gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);
        gl.BufferData(GL_ARRAY_BUFFER,GLsizeiptr(vertices.size()*sizeof(detail::GpuVertex)),vertices.empty()?nullptr:vertices.data(),GL_STATIC_DRAW);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
        gl.BufferData(GL_ELEMENT_ARRAY_BUFFER,GLsizeiptr(indices.size()*sizeof(GLuint)),indices.empty()?nullptr:indices.data(),GL_STATIC_DRAW);
        gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);
        if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::VertexInput, "glBufferData")){cleanup();return false;}
        for(const auto& item:textures_)gl.DeleteTextures(1,&item.second);
        textures_=std::move(stagedTextures);drawRanges_=std::move(ranges);return true;
    } catch(...) {cleanup();throw;}
}
inline bool Renderer::draw(const Scene& scene,const glm::mat4& view,const glm::mat4& projection,
                           const RenderOptions& options) const {
    Diagnostics::trace(PipelineStage::Transform, "Renderer::draw");
    if(!detail::finiteMatrix(view)||!detail::finiteMatrix(projection))return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Transform, "Renderer::draw", "View or projection matrix contains NaN/Inf.");
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
        }
        if(!detail::finiteMatrix(matrix)||!detail::finiteMatrix(projection*view*matrix))return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Transform, "Renderer::draw", "World or MVP matrix contains NaN/Inf.", "Check parent transforms, scales and motion samples.", range.entityId);
    }
    auto& gl=detail::gl();gl.UseProgram(shaderProgramID_);gl.BindVertexArray(vao_);
    auto cull=[&](const Mesh* face,const glm::mat4& model) {
        const auto mode=face && face->cull!=CullMode::Inherit?face->cull:options.cull;
        gl.FrontFace(glm::determinant(glm::mat3(model))<0?GL_CW:GL_CCW);
        if(!face || mode==CullMode::None || mode==CullMode::Inherit) gl.Disable(GL_CULL_FACE);
        else {gl.Enable(GL_CULL_FACE);gl.CullFace(mode==CullMode::Front?GL_FRONT:GL_BACK);}
    };
    gl.ActiveTexture(GL_TEXTURE0);gl.Uniform1i(textureLocation_,0);
    gl.Uniform3f(lightDirectionLocation_,options.lightDirection.x,options.lightDirection.y,options.lightDirection.z);
    gl.Uniform3f(lightColorLocation_,options.lightColor.red,options.lightColor.green,options.lightColor.blue);
    gl.Uniform3f(outlineColorLocation_,options.outlineColor.red,options.outlineColor.green,options.outlineColor.blue);
    if(options.wireframe && options.depthTest) {
        // All surfaces write depth first, so a later object's front face hides earlier back edges.
        gl.ColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
        for(const auto& range:drawRanges_) {
            if(range.lineId)continue;
            const auto* c=scene.getComponent(range.entityId);
            const auto& face=c->polyhedron.faces[range.faceIndex];
            if(!c->visible||!face.visible)continue;
            const auto model=*scene.faceWorldMatrix(range.entityId,range.faceIndex);
            cull(&face,model);
            const auto matrix=projection*view*model;
            gl.UniformMatrix4fv(mvpLocation_,1,GL_FALSE,glm::value_ptr(matrix));
            gl.DrawElements(GL_TRIANGLES,range.count,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.firstIndex*sizeof(GLuint)));
        }
        gl.ColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    }
    for(const auto& range:drawRanges_) {
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;model=*world;
        }
        const auto mvp=projection*view*model;
        cull(face,model);
        const float determinant=glm::determinant(glm::mat3(model));
        const auto normalMatrix=std::isfinite(determinant)&&std::abs(determinant)>1e-12f
            ?glm::transpose(glm::inverse(glm::mat3(model))):glm::mat3(1);
        gl.UniformMatrix4fv(mvpLocation_,1,GL_FALSE,glm::value_ptr(mvp));
        gl.UniformMatrix4fv(modelLocation_,1,GL_FALSE,glm::value_ptr(model));
        gl.UniformMatrix3fv(normalLocation_,1,GL_FALSE,glm::value_ptr(normalMatrix));
        gl.Uniform3f(tintLocation_,tint.red,tint.green,tint.blue);
        gl.Uniform1i(lightingLocation_,face&&options.lighting);gl.Uniform1i(overrideLocation_,0);
        const bool textured=face&&options.textures&&face->material.useTexture;
        gl.Uniform1i(hasTextureLocation_,textured);
        if(textured){gl.BindTexture(GL_TEXTURE_2D,textures_.at(face->material.texture));
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);}
        else gl.BindTexture(GL_TEXTURE_2D,0);
        if(!face||!options.wireframe)
        {
            gl.DrawElements(face?GL_TRIANGLES:GL_LINES,range.count,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.firstIndex*sizeof(GLuint)));
            if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Raster, "glDrawElements")) {
                gl.BindVertexArray(0);gl.UseProgram(0);return false;
            }
        }
        if(face&&(options.wireframe||face->outline!=OutlineMode::None)) {
            gl.Uniform1i(hasTextureLocation_,0);gl.Uniform1i(lightingLocation_,0);gl.Uniform1i(overrideLocation_,options.wireframe?0:1);
            gl.DepthFunc(GL_LEQUAL);gl.DepthMask(GL_FALSE);
            gl.DrawElements(GL_LINES,range.outlineCount,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.outlineFirstIndex*sizeof(GLuint)));
            gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);
        }
    }
    gl.Uniform1i(overrideLocation_,0);gl.BindTexture(GL_TEXTURE_2D,0);gl.BindVertexArray(0);gl.UseProgram(0);return detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Raster, "Renderer::draw outlines and cleanup");
}
inline bool Renderer::renderPass(const Scene& scene,const RenderPass& pass,int width,int height) {
    Diagnostics::trace(PipelineStage::Framebuffer,"Renderer::renderPass");
    const auto& camera=pass.camera;const auto& options=pass.options;const auto& viewport=pass.viewport;
    if(!shaderProgramID_||!vao_)return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::renderPass","Renderer is not initialized.");
    if(width<=0||height<=0)return true;
    if(viewport.x<0 || viewport.y<0 || viewport.x>width || viewport.y>height || viewport.width<0 || viewport.height<0
        || viewport.width>width-viewport.x || viewport.height>height-viewport.y)
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Renderer::renderPass","Viewport lies outside the framebuffer.");
    if(viewport.width==0 || viewport.height==0) return true;
    if(!CameraSystem::validCamera(camera)||!detail::finiteVector(options.lightDirection)
        ||glm::length(options.lightDirection)<1e-12f||!detail::validColor(options.lightColor)
        ||!detail::validColor(options.outlineColor)
        ||!std::isfinite(options.background.r)||!std::isfinite(options.background.g)
        ||!std::isfinite(options.background.b)||!std::isfinite(options.background.a)
        ||(options.cull!=CullMode::Inherit && options.cull!=CullMode::None && options.cull!=CullMode::Back && options.cull!=CullMode::Front))
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Transform,"Renderer::renderPass","Invalid camera, light, color or culling mode.","Check near/far planes, FOV, target/up directions, positive orthographic extent and finite render options.");
    if(uploadedScene_!=&scene||uploadedRevision_!=scene.getGeometryRevision()||uploadedTextures_!=options.textures) {
        if(!upload(scene,options.textures))return false;
        uploadedScene_=&scene;uploadedRevision_=scene.getGeometryRevision();uploadedTextures_=options.textures;
    }
    auto& gl=detail::gl();if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Framebuffer, "before Renderer::render"))return false;
    gl.Viewport(viewport.x,viewport.y,viewport.width,viewport.height);gl.LineWidth(1);
    if(options.depthTest)gl.Enable(GL_DEPTH_TEST);else gl.Disable(GL_DEPTH_TEST);
    gl.Disable(GL_CULL_FACE);gl.Disable(GL_BLEND);gl.Enable(GL_SCISSOR_TEST);
    gl.Scissor(viewport.x,viewport.y,viewport.width,viewport.height);
    gl.PolygonMode(GL_FRONT_AND_BACK,GL_FILL);gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);gl.ColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    gl.ClearColor(options.background.r,options.background.g,options.background.b,options.background.a);
    const GLbitfield mask=(pass.clearColor?GL_COLOR_BUFFER_BIT:0)|(pass.clearDepth?GL_DEPTH_BUFFER_BIT:0);
    if(mask) gl.Clear(mask);
    if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"viewport/scissor/clear")) {gl.Disable(GL_SCISSOR_TEST);return false;}
    const bool success=draw(scene,CameraSystem::viewMatrix(camera),CameraSystem::projectionMatrix(camera,float(viewport.width)/viewport.height),options);
    gl.Disable(GL_SCISSOR_TEST);gl.Disable(GL_CULL_FACE);gl.FrontFace(GL_CCW);
    return success;
}
inline bool Renderer::render(const Scene& scene,const Camera& camera,int width,int height,const RenderOptions& options) {
    RenderPass pass;pass.camera=camera;pass.options=options;pass.viewport={0,0,width,height};
    return renderPass(scene,pass,width,height);
}
inline bool Renderer::renderViews(const Scene& scene,const std::vector<RenderPass>& passes,int width,int height) {
    if(width<=0 || height<=0) return true;
    if(passes.empty()) return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Renderer::renderViews","Need at least one render pass.");
    if(!shaderProgramID_ || !vao_) return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::renderViews","Renderer is not initialized.");
    auto& gl=detail::gl();
    gl.Disable(GL_SCISSOR_TEST);gl.Viewport(0,0,width,height);gl.DepthMask(GL_TRUE);gl.ColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    gl.ClearColor(.08f,.08f,.1f,1);gl.Clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Renderer::renderViews full-frame clear")) return false;
    for(const auto& pass:passes) if(!renderPass(scene,pass,width,height)) return false;
    return true;
}
inline void Renderer::destroy() {
    auto& gl=detail::gl();
    if(ebo_)gl.DeleteBuffers(1,&ebo_);if(vbo_)gl.DeleteBuffers(1,&vbo_);if(vao_)gl.DeleteVertexArrays(1,&vao_);
    if(shaderProgramID_)gl.DeleteProgram(shaderProgramID_);
    for(const auto& item:textures_)gl.DeleteTextures(1,&item.second);
    textures_.clear();drawRanges_.clear();ebo_=vbo_=vao_=shaderProgramID_=0;
    uploadedScene_=nullptr;uploadedRevision_=std::numeric_limits<std::uint64_t>::max();uploadedTextures_=false;
}
} // namespace graphics

```

### after

```text
#pragma once
#include "Shaders.h"
#include "../dependencies/glm/glm/gtc/type_ptr.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace graphics {
inline std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream input(path,std::ios::binary);
    if(!input) return {};
    std::ostringstream buffer;buffer<<input.rdbuf();auto text=buffer.str();
    if(text.compare(0,3,"\xEF\xBB\xBF")==0) text.erase(0,3);
    return text;
}
namespace detail {
inline GLuint compileShader(GLenum kind,const std::string& sourceText,const std::filesystem::path& sourcePath) {
    Diagnostics::trace(PipelineStage::Shader, "compileShader");
    auto& api=gl();const auto shader=api.CreateShader(kind);
    if(!shader) { Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, "glCreateShader", "Shader allocation failed."); return 0; }
    const char* text=sourceText.c_str();api.ShaderSource(shader,1,&text,nullptr);api.CompileShader(shader);
    GLint success=0;api.GetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success) {
        GLint length=0;api.GetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
        std::string log(std::max(length,1), '\0');api.GetShaderInfoLog(shader,GLsizei(log.size()),nullptr,log.data());
        Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, kind==GL_VERTEX_SHADER?"vertex shader":"fragment shader", "GLSL file="+sourcePath.u8string()+"\n"+log,
            "Read the compiler line number; check GLSL version, declarations and shader inputs/outputs.");
        api.DeleteShader(shader);return 0;
    }
    return shader;
}
struct GpuVertex { float position[3], color[3], uv[2], normal[3]; };
inline GLuint uploadTexture(const TextureImage& image,bool repeat) {
    auto& api=gl();GLuint id=0;api.GenTextures(1,&id);
    if(!id){Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Asset,"glGenTextures","Texture allocation failed.");return 0;}
    api.ActiveTexture(GL_TEXTURE0);api.BindTexture(GL_TEXTURE_2D,id);
    GLint previousAlignment=4;api.GetIntegerv(GL_UNPACK_ALIGNMENT,&previousAlignment);
    api.PixelStorei(GL_UNPACK_ALIGNMENT,1);
    api.TexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,GLsizei(image.width),GLsizei(image.height),0,GL_RGBA,GL_UNSIGNED_BYTE,image.rgba.data());
    api.PixelStorei(GL_UNPACK_ALIGNMENT,previousAlignment);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
    if(!checkGlAt(__FILE__,__LINE__,PipelineStage::Asset, "uploadTexture")) {api.DeleteTextures(1,&id);return 0;}
    return id;
}
} // namespace detail

inline bool Renderer::initShader(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    auto& gl=detail::gl();
    const auto vertexText=vertexPath.empty()?std::string(detail::vertexShader):readTextFile(vertexPath);
    const auto fragmentText=fragmentPath.empty()?std::string(detail::fragmentShader):readTextFile(fragmentPath);
    if(vertexText.empty()||fragmentText.empty()) return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, "Renderer::initShader",
        "Shader source is empty or unreadable: vertex=" + vertexPath.u8string() + "; fragment=" + fragmentPath.u8string(),
        "Use embedded shaders (empty shaderDirectory), or copy both GLSL files to the configured project-relative directory.");
    // /utf-8 /FC encodes __FILE__ as UTF-8, not the Windows ANSI code page.
    const auto embeddedSource=std::filesystem::u8path(__FILE__).parent_path()/"Shaders.h";
    const auto vertex=detail::compileShader(GL_VERTEX_SHADER,vertexText,vertexPath.empty()?embeddedSource:vertexPath);if(!vertex)return false;
    const auto fragment=detail::compileShader(GL_FRAGMENT_SHADER,fragmentText,fragmentPath.empty()?embeddedSource:fragmentPath);
    if(!fragment){gl.DeleteShader(vertex);return false;}
    shaderProgramID_=gl.CreateProgram();
    if(!shaderProgramID_){gl.DeleteShader(vertex);gl.DeleteShader(fragment);return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader,"glCreateProgram","Program allocation failed.");}
    gl.AttachShader(shaderProgramID_,vertex);gl.AttachShader(shaderProgramID_,fragment);gl.LinkProgram(shaderProgramID_);
    gl.DeleteShader(vertex);gl.DeleteShader(fragment);
    GLint success=0;gl.GetProgramiv(shaderProgramID_,GL_LINK_STATUS,&success);
    if(!success) {
        GLint size=0;gl.GetProgramiv(shaderProgramID_,GL_INFO_LOG_LENGTH,&size);
        std::string log(std::max(size,1),'\0');gl.GetProgramInfoLog(shaderProgramID_,GLsizei(log.size()),nullptr,log.data());
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, "glLinkProgram", log, "Match vertex outputs and fragment inputs; inspect the linker log.");
    }
    mvpLocation_=gl.GetUniformLocation(shaderProgramID_,"uMVP");
    modelLocation_=gl.GetUniformLocation(shaderProgramID_,"uModel");
    normalLocation_=gl.GetUniformLocation(shaderProgramID_,"uNormalMatrix");
    textureLocation_=gl.GetUniformLocation(shaderProgramID_,"uTexture");
    hasTextureLocation_=gl.GetUniformLocation(shaderProgramID_,"uHasTexture");
    lightingLocation_=gl.GetUniformLocation(shaderProgramID_,"uLighting");
    lightDirectionLocation_=gl.GetUniformLocation(shaderProgramID_,"uLightDirection");
    lightColorLocation_=gl.GetUniformLocation(shaderProgramID_,"uLightColor");
    overrideLocation_=gl.GetUniformLocation(shaderProgramID_,"uOverrideColor");
    outlineColorLocation_=gl.GetUniformLocation(shaderProgramID_,"uOutlineColor");
    tintLocation_=gl.GetUniformLocation(shaderProgramID_,"uTint");
    return mvpLocation_>=0 || Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Shader, "glGetUniformLocation", "Required uMVP uniform is missing or optimized away.");
}
inline bool Renderer::initialize(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    detail::gl().load();destroy();
    if(!initShader(vertexPath,fragmentPath)){destroy();return false;}
    auto& gl=detail::gl();gl.GenVertexArrays(1,&vao_);gl.GenBuffers(1,&vbo_);gl.GenBuffers(1,&ebo_);
    if(!vao_||!vbo_||!ebo_){destroy();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput, "Renderer::initialize", "VAO/VBO/EBO allocation failed.");}
    gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    using V=detail::GpuVertex;
    gl.VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,position)));
    gl.VertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,color)));
    gl.VertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,uv)));
    gl.VertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,normal)));
    for(GLuint i=0;i<4;++i)gl.EnableVertexAttribArray(i);
    gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);return detail::checkGlAt(__FILE__,__LINE__,PipelineStage::VertexInput, "Renderer::initialize vertex layout");
}
inline bool Renderer::upload(const Scene& scene,bool textureMode) {
    Diagnostics::trace(PipelineStage::VertexInput, "Renderer::upload");
    std::vector<detail::GpuVertex> vertices;
    std::vector<GLuint> indices;std::vector<DrawRange> ranges;
    TextureMap stagedTextures;auto& gl=detail::gl();
    auto cleanup=[&](){for(const auto& item:stagedTextures)gl.DeleteTextures(1,&item.second);};
    try {
        for(auto id:scene.getIds()) {
            const auto* c=scene.getComponent(id);if(c&&c->transformOnly)continue;
            if(!c||!DiagramSystem::validPolyhedron(c->polyhedron)){cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput, "Renderer::upload", "Missing component or invalid mesh, indices, colors, UVs or normals.", "Inspect the entity's mesh data before GPU upload.", id);}
            for(std::size_t faceIndex=0;faceIndex<c->polyhedron.faces.size();++faceIndex) {
                const auto& face=c->polyhedron.faces[faceIndex];
                if(vertices.size()+face.localVertices.size()>std::numeric_limits<GLuint>::max()) {cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::upload","Vertex count exceeds the GPU index range.");}
                const auto first=GLuint(vertices.size());
                Mesh normals=face;if(normals.vertexNormals.empty())recalculateNormals(normals);
                for(std::size_t i=0;i<face.localVertices.size();++i) {
                    const auto p=face.localVertices[i];const auto n=normals.vertexNormals[i];
                    auto color=face.vertexColors.empty()?face.color:face.vertexColors[i];
                    color.red*=face.material.color.red;color.green*=face.material.color.green;color.blue*=face.material.color.blue;
                    const auto uv=face.texcoords.empty()?glm::vec2(0):face.texcoords[i];
                    vertices.push_back({{p.x,p.y,p.z},{color.red,color.green,color.blue},{uv.x,uv.y},{n.x,n.y,n.z}});
                }
                const auto triangles=detail::triangles(face);
                const auto outline=meshOutline(face,face.outline==OutlineMode::None?OutlineMode::FeatureEdges:face.outline);
                if(triangles.size()>std::numeric_limits<GLsizei>::max()||outline.size()>std::numeric_limits<GLsizei>::max()){cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::upload","Draw count exceeds GLsizei range.");}
                DrawRange range;range.entityId=id;range.faceIndex=faceIndex;range.firstIndex=indices.size();range.count=GLsizei(triangles.size());
                for(auto index:triangles)indices.push_back(first+index);
                range.outlineFirstIndex=indices.size();range.outlineCount=GLsizei(outline.size());
                for(auto index:outline)indices.push_back(first+index);
                ranges.push_back(range);
                if(textureMode&&face.material.useTexture&&stagedTextures.find(face.material.texture)==stagedTextures.end()) {
                    if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Asset, "before uploadTexture")){cleanup();return false;}
                    const auto texture=detail::uploadTexture(*face.material.texture,face.material.repeatTexture);
                    if(!texture){cleanup();return false;}
                    try{stagedTextures.emplace(face.material.texture,texture);}catch(...){gl.DeleteTextures(1,&texture);throw;}
                }
            }
        }
        for(std::size_t lineIndex=0;lineIndex<scene.getLines().size();++lineIndex) {
            const auto& line=scene.getLines()[lineIndex];
            if(vertices.size()>std::numeric_limits<GLuint>::max()-2){cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::upload lines","Vertex count exceeds the GPU index range.");}
            const auto first=GLuint(vertices.size());
            for(auto p:{line.start,line.end})vertices.push_back({{p.x,p.y,p.z},{line.color.red,line.color.green,line.color.blue},{0,0},{0,0,0}});
            DrawRange range;range.lineId=lineIndex;range.firstIndex=indices.size();range.count=2;
            ranges.push_back(range);indices.insert(indices.end(),{first,first+1});
        }
        if(vertices.size()>std::size_t(std::numeric_limits<GLsizeiptr>::max())/sizeof(detail::GpuVertex)
            ||indices.size()>std::size_t(std::numeric_limits<GLsizeiptr>::max())/sizeof(GLuint)){cleanup();return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::upload","Buffer byte size exceeds GLsizeiptr range.");}
        if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::VertexInput, "before buffer upload")){cleanup();return false;}
        gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);
        gl.BufferData(GL_ARRAY_BUFFER,GLsizeiptr(vertices.size()*sizeof(detail::GpuVertex)),vertices.empty()?nullptr:vertices.data(),GL_STATIC_DRAW);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
        gl.BufferData(GL_ELEMENT_ARRAY_BUFFER,GLsizeiptr(indices.size()*sizeof(GLuint)),indices.empty()?nullptr:indices.data(),GL_STATIC_DRAW);
        gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);
        if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::VertexInput, "glBufferData")){cleanup();return false;}
        for(const auto& item:textures_)gl.DeleteTextures(1,&item.second);
        textures_=std::move(stagedTextures);drawRanges_=std::move(ranges);return true;
    } catch(...) {cleanup();throw;}
}
inline bool Renderer::draw(const Scene& scene,const glm::mat4& view,const glm::mat4& projection,
                           const RenderOptions& options) const {
    Diagnostics::trace(PipelineStage::Transform, "Renderer::draw");
    if(!detail::finiteMatrix(view)||!detail::finiteMatrix(projection))return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Transform, "Renderer::draw", "View or projection matrix contains NaN/Inf.");
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
        }
        if(!detail::finiteMatrix(matrix)||!detail::finiteMatrix(projection*view*matrix))return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Transform, "Renderer::draw", "World or MVP matrix contains NaN/Inf.", "Check parent transforms, scales and motion samples.", range.entityId);
    }
    auto& gl=detail::gl();gl.UseProgram(shaderProgramID_);gl.BindVertexArray(vao_);
    auto cull=[&](const Mesh* face,const glm::mat4& model) {
        const auto mode=face && face->cull!=CullMode::Inherit?face->cull:options.cull;
        gl.FrontFace(glm::determinant(glm::mat3(model))<0?GL_CW:GL_CCW);
        if(!face || mode==CullMode::None || mode==CullMode::Inherit) gl.Disable(GL_CULL_FACE);
        else {gl.Enable(GL_CULL_FACE);gl.CullFace(mode==CullMode::Front?GL_FRONT:GL_BACK);}
    };
    gl.ActiveTexture(GL_TEXTURE0);gl.Uniform1i(textureLocation_,0);
    gl.Uniform3f(lightDirectionLocation_,options.lightDirection.x,options.lightDirection.y,options.lightDirection.z);
    gl.Uniform3f(lightColorLocation_,options.lightColor.red,options.lightColor.green,options.lightColor.blue);
    gl.Uniform3f(outlineColorLocation_,options.outlineColor.red,options.outlineColor.green,options.outlineColor.blue);
    if(options.wireframe && options.depthTest) {
        // All surfaces write depth first, so a later object's front face hides earlier back edges.
        gl.ColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
        for(const auto& range:drawRanges_) {
            if(range.lineId)continue;
            const auto* c=scene.getComponent(range.entityId);
            const auto& face=c->polyhedron.faces[range.faceIndex];
            if(!c->visible||!face.visible)continue;
            const auto model=*scene.faceWorldMatrix(range.entityId,range.faceIndex);
            cull(&face,model);
            const auto matrix=projection*view*model;
            gl.UniformMatrix4fv(mvpLocation_,1,GL_FALSE,glm::value_ptr(matrix));
            gl.DrawElements(GL_TRIANGLES,range.count,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.firstIndex*sizeof(GLuint)));
        }
        gl.ColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    }
    for(const auto& range:drawRanges_) {
        glm::mat4 model(1);const Mesh* face=nullptr;GlColor tint;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;}
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;model=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;tint=c->tint;
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;model=*world;
        }
        const auto mvp=projection*view*model;
        cull(face,model);
        const float determinant=glm::determinant(glm::mat3(model));
        const auto normalMatrix=std::isfinite(determinant)&&std::abs(determinant)>1e-12f
            ?glm::transpose(glm::inverse(glm::mat3(model))):glm::mat3(1);
        gl.UniformMatrix4fv(mvpLocation_,1,GL_FALSE,glm::value_ptr(mvp));
        gl.UniformMatrix4fv(modelLocation_,1,GL_FALSE,glm::value_ptr(model));
        gl.UniformMatrix3fv(normalLocation_,1,GL_FALSE,glm::value_ptr(normalMatrix));
        gl.Uniform3f(tintLocation_,tint.red,tint.green,tint.blue);
        gl.Uniform1i(lightingLocation_,face&&options.lighting);gl.Uniform1i(overrideLocation_,0);
        const bool textured=face&&options.textures&&face->material.useTexture;
        gl.Uniform1i(hasTextureLocation_,textured);
        if(textured){gl.BindTexture(GL_TEXTURE_2D,textures_.at(face->material.texture));
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);}
        else gl.BindTexture(GL_TEXTURE_2D,0);
        if(!face||!options.wireframe)
        {
            gl.DrawElements(face?GL_TRIANGLES:GL_LINES,range.count,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.firstIndex*sizeof(GLuint)));
            if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Raster, "glDrawElements")) {
                gl.BindVertexArray(0);gl.UseProgram(0);return false;
            }
        }
        if(face&&(options.wireframe||face->outline!=OutlineMode::None)) {
            gl.Uniform1i(hasTextureLocation_,0);gl.Uniform1i(lightingLocation_,0);gl.Uniform1i(overrideLocation_,options.wireframe?0:1);
            gl.DepthFunc(GL_LEQUAL);gl.DepthMask(GL_FALSE);
            gl.DrawElements(GL_LINES,range.outlineCount,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.outlineFirstIndex*sizeof(GLuint)));
            gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);
        }
    }
    gl.Uniform1i(overrideLocation_,0);gl.BindTexture(GL_TEXTURE_2D,0);gl.BindVertexArray(0);gl.UseProgram(0);return detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Raster, "Renderer::draw outlines and cleanup");
}
inline bool Renderer::renderPass(const Scene& scene,const RenderPass& pass,int width,int height) {
    Diagnostics::trace(PipelineStage::Framebuffer,"Renderer::renderPass");
    const auto& camera=pass.camera;const auto& options=pass.options;const auto& viewport=pass.viewport;
    if(!shaderProgramID_||!vao_)return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::renderPass","Renderer is not initialized.");
    if(width<=0||height<=0)return true;
    if(viewport.x<0 || viewport.y<0 || viewport.x>width || viewport.y>height || viewport.width<0 || viewport.height<0
        || viewport.width>width-viewport.x || viewport.height>height-viewport.y)
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Renderer::renderPass","Viewport lies outside the framebuffer.");
    if(viewport.width==0 || viewport.height==0) return true;
    if(!CameraSystem::validCamera(camera)||!detail::finiteVector(options.lightDirection)
        ||glm::length(options.lightDirection)<1e-12f||!detail::validColor(options.lightColor)
        ||!detail::validColor(options.outlineColor)
        ||!std::isfinite(options.background.r)||!std::isfinite(options.background.g)
        ||!std::isfinite(options.background.b)||!std::isfinite(options.background.a)
        ||(options.cull!=CullMode::Inherit && options.cull!=CullMode::None && options.cull!=CullMode::Back && options.cull!=CullMode::Front))
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Transform,"Renderer::renderPass","Invalid camera, light, color or culling mode.","Check near/far planes, FOV, target/up directions, positive orthographic extent and finite render options.");
    if(uploadedScene_!=&scene||uploadedRevision_!=scene.getGeometryRevision()||uploadedTextures_!=options.textures) {
        if(!upload(scene,options.textures))return false;
        uploadedScene_=&scene;uploadedRevision_=scene.getGeometryRevision();uploadedTextures_=options.textures;
    }
    auto& gl=detail::gl();if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Framebuffer, "before Renderer::render"))return false;
    gl.Viewport(viewport.x,viewport.y,viewport.width,viewport.height);gl.LineWidth(1);
    if(options.depthTest)gl.Enable(GL_DEPTH_TEST);else gl.Disable(GL_DEPTH_TEST);
    gl.Disable(GL_CULL_FACE);gl.Disable(GL_BLEND);gl.Enable(GL_SCISSOR_TEST);
    gl.Scissor(viewport.x,viewport.y,viewport.width,viewport.height);
    gl.PolygonMode(GL_FRONT_AND_BACK,GL_FILL);gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);gl.ColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    gl.ClearColor(options.background.r,options.background.g,options.background.b,options.background.a);
    const GLbitfield mask=(pass.clearColor?GL_COLOR_BUFFER_BIT:0)|(pass.clearDepth?GL_DEPTH_BUFFER_BIT:0);
    if(mask) gl.Clear(mask);
    if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"viewport/scissor/clear")) {gl.Disable(GL_SCISSOR_TEST);return false;}
    const bool success=draw(scene,CameraSystem::viewMatrix(camera),CameraSystem::projectionMatrix(camera,float(viewport.width)/viewport.height),options);
    gl.Disable(GL_SCISSOR_TEST);gl.Disable(GL_CULL_FACE);gl.FrontFace(GL_CCW);
    return success;
}
inline bool Renderer::render(const Scene& scene,const Camera& camera,int width,int height,const RenderOptions& options) {
    RenderPass pass;pass.camera=camera;pass.options=options;pass.viewport={0,0,width,height};
    return renderPass(scene,pass,width,height);
}
inline bool Renderer::renderViews(const Scene& scene,const std::vector<RenderPass>& passes,int width,int height) {
    if(width<=0 || height<=0) return true;
    if(passes.empty()) return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Renderer::renderViews","Need at least one render pass.");
    if(!shaderProgramID_ || !vao_) return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::VertexInput,"Renderer::renderViews","Renderer is not initialized.");
    auto& gl=detail::gl();
    gl.Disable(GL_SCISSOR_TEST);gl.Viewport(0,0,width,height);gl.DepthMask(GL_TRUE);gl.ColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    gl.ClearColor(.08f,.08f,.1f,1);gl.Clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(!detail::checkGlAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Renderer::renderViews full-frame clear")) return false;
    for(const auto& pass:passes) if(!renderPass(scene,pass,width,height)) return false;
    return true;
}
inline void Renderer::destroy() {
    auto& gl=detail::gl();
    if(ebo_)gl.DeleteBuffers(1,&ebo_);if(vbo_)gl.DeleteBuffers(1,&vbo_);if(vao_)gl.DeleteVertexArrays(1,&vao_);
    if(shaderProgramID_)gl.DeleteProgram(shaderProgramID_);
    for(const auto& item:textures_)gl.DeleteTextures(1,&item.second);
    textures_.clear();drawRanges_.clear();ebo_=vbo_=vao_=shaderProgramID_=0;
    uploadedScene_=nullptr;uploadedRevision_=std::numeric_limits<std::uint64_t>::max();uploadedTextures_=false;
}
} // namespace graphics

```

## framework/detail/Application.inl

### before

```text
#pragma once
#include <algorithm>
#include <iostream>
#include <utility>
namespace graphics::detail {
inline bool& applicationActive() { static bool active=false;return active; }
inline unsigned& contextErrorSerial() { static unsigned value=0;return value; }
}
namespace graphics {

inline Application::~Application() {
    shutdown();
}

inline void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

inline void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

inline void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

inline void Application::setModifiedKeyHandler(
    std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

inline void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

inline void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

inline bool Application::ensureContext(const char* operation,const char* file,int line) {
    Diagnostics::locate(file,line,operation);
    if(contextFailed_) return false;
    if(!window_ || GetCurrentThreadId()!=contextThread_) {
        contextFailed_=true;
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime,"Application::ensureContext","Missing window or context used from a different thread.","Keep window creation, rendering and shutdown on the owning thread; inspect Application::run and the custom render handler.");
    }
    using CurrentProc=void* (WINAPI*)();
    const auto module=GetModuleHandleW(L"opengl32.dll");
    const auto current=module?reinterpret_cast<CurrentProc>(::GetProcAddress(module,"wglGetCurrentContext")):nullptr;
    if(nativeContext_ && detail::glfw().GetCurrentContext()==window_ && current && current()==nativeContext_) return true;
    const auto serial=detail::contextErrorSerial();
    detail::glfw().MakeContextCurrent(window_);
    if(serial!=detail::contextErrorSerial() || detail::glfw().GetCurrentContext()!=window_ || !current || !current()) {
        contextFailed_=true;
        return Diagnostics::fail(PipelineStage::Runtime,"glfwMakeContextCurrent","WGL did not attach the requested context; rendering stopped before GL commands.","Source is the failed bind call below. Check window lifetime, render-thread ownership and external handlers that detach/destroy the context. If those are valid, inspect the GPU driver/session; this log alone cannot prove a driver cause.");
    }
    nativeContext_=current();
    return true;
}

inline bool Application::initialize(const ApplicationConfig& config) {
    contextThread_=GetCurrentThreadId();contextFailed_=false;nativeContext_=nullptr;
    Diagnostics::trace(PipelineStage::Runtime, "Application::initialize");
    const auto root=detail::relativePathInside(detail::executableDirectory(),config.projectRoot);
    const auto runtime=detail::relativePathInside(root,config.runtimeDirectory);
    detail::glfw().load(runtime);
    detail::glfw().SetErrorCallback([](int code, const char* message) {
        ++detail::contextErrorSerial();
        Diagnostics::fail(PipelineStage::Runtime, "GLFW/WGL", std::to_string(code) + ": " + (message ? message : "unknown error"),
            "Context binding failed at Source/Operation. Inspect Application context lifetime/thread first; the OS/driver reason is in the original message. Render passes do not need separate WGL contexts.");
    });
    if (config.width <= 0 || config.height <= 0)
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "Application::initialize", "Window width and height must be positive.");
    if (!detail::glfw().Init())
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "glfwInit", "GLFW initialization failed; see the GLFW error above.");
    glfwInitialized_ = true;
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    detail::glfw().WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    detail::glfw().WindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    detail::glfw().WindowHint(GLFW_DEPTH_BITS, 24);
    window_ = detail::glfw().createWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "glfwCreateWindow", "Could not create an OpenGL 3.3 core window.", "Check the GLFW error above and update the graphics driver.");
    }
    if(!ensureContext("initialize -> attach new window",__FILE__,__LINE__)) return false;
    detail::glfw().SwapInterval(config.verticalSync ? 1 : 0);
    const auto vertexPath=config.shaderDirectory.empty()?std::filesystem::path{}:detail::relativePathInside(root,config.shaderDirectory)/"VertexShader.glsl";
    const auto fragmentPath=config.shaderDirectory.empty()?std::filesystem::path{}:detail::relativePathInside(root,config.shaderDirectory)/"FragmentShader.glsl";
    if(!renderer_.initialize(vertexPath,fragmentPath)) return false;
    assets_.emplace(root,config.features);
    renderOptions_.textures=config.features.textures;

    detail::glfw().SetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    detail::glfw().SetWindowUserPointer(window_, this);
    detail::glfw().SetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&] {
            if(app.modifiedKeyHandler_)app.modifiedKeyHandler_(app,key,action,mods);
            else if(app.keyHandler_)app.keyHandler_(app,key,action);
        });
    });
    detail::glfw().SetWindowFocusCallback(window_, [](GLFWwindow* window,int focused) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&]{if(app.focusHandler_)app.focusHandler_(app,focused==GLFW_TRUE);});
    });
    detail::glfw().SetMouseButtonCallback(window_, [](GLFWwindow* window,int button,int action,int mods) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&] {
            if(!app.mouseHandler_)return;
            MouseEvent event;event.button=button;event.action=action;event.mods=mods;
            detail::glfw().GetCursorPos(window,&event.windowX,&event.windowY);
            int ww=0,wh=0,fw=0,fh=0;
            detail::glfw().GetWindowSize(window,&ww,&wh);detail::glfw().GetFramebufferSize(window,&fw,&fh);
            if(ww<=0||wh<=0||fw<=0||fh<=0)return;
            event.framebufferX=event.windowX*fw/ww;event.framebufferY=event.windowY*fh/wh;
            app.mouseHandler_(app,event);
        });
    });
    detail::glfw().SetCursorPosCallback(window_, [](GLFWwindow* window,double x,double y) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&]{if(app.cursorHandler_)app.cursorHandler_(app,x,y);});
    });
    return true;
}

inline void Application::shutdown() {
    assets_.reset();
    if (window_ != nullptr) {
        if(!contextFailed_ && ensureContext("shutdown -> GPU cleanup",__FILE__,__LINE__)) renderer_.destroy();
        else renderer_.abandon();
        detail::glfw().SetWindowUserPointer(window_, nullptr);
        detail::glfw().DestroyWindow(window_);
        window_ = nullptr;
    }
    if (glfwInitialized_) {
        detail::glfw().Terminate();
        glfwInitialized_ = false;
    }
}

inline void Application::requestClose() {
    if (window_ != nullptr) {
        detail::glfw().SetWindowShouldClose(window_, GLFW_TRUE);
    }
}

inline bool Application::renderScene() {
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Application::renderScene","No active window/context.");
    }
    if(!ensureContext("renderScene -> single pass",__FILE__,__LINE__)) return false;
    int width = 0;
    int height = 0;
    detail::glfw().GetFramebufferSize(window_, &width, &height);
    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

inline std::array<int,2> Application::getFramebufferSize() const {
    std::array<int,2> size{};
    if(window_) detail::glfw().GetFramebufferSize(window_,&size[0],&size[1]);
    return size;
}
inline bool Application::renderViews(const std::vector<RenderPass>& passes) {
    if(!window_) return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Application::renderViews","No active window/context.");
    if(!ensureContext("renderViews -> shared context for all viewports",__FILE__,__LINE__)) return false;
    const auto size=getFramebufferSize();
    return renderer_.renderViews(scene_,passes,size[0],size[1]);
}

inline int Application::run(const ApplicationConfig& config) {
    Diagnostics::configure(config.diagnostics);
    Diagnostics::clear();
    if (running_) {
        Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime,"Application::run","Recursive run() is not allowed.");
        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }
    if(detail::applicationActive()) {Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime,"Application::run","Only one active Application is supported.");return -1;}
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    detail::applicationActive()=true;
    callbackError_=nullptr;
    running_ = true;
    int exitCode = 0;
    try {
        if (!initialize(config)) {
            std::cerr << "Application initialization failed.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Scene,"Application::initHandler","User initialization returned false.","Read the preceding API error; inspect the scene creation callback.");
            std::cerr << "User scene initialization failed.\n";
            exitCode = -1;
        } else {
            double previousTime = detail::glfw().GetTime();
            while (!detail::glfw().WindowShouldClose(window_)) {
                Diagnostics::locate(__FILE__,__LINE__,"run -> glfwPollEvents/input callbacks");
                detail::glfw().PollEvents();
                if(callbackError_)std::rethrow_exception(callbackError_);
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                int width = 0;
                int height = 0;
                detail::glfw().GetFramebufferSize(window_, &width, &height);
                if (width <= 0 || height <= 0) {
                    detail::glfw().WaitEvents();
                    if(callbackError_)std::rethrow_exception(callbackError_);
                    previousTime = detail::glfw().GetTime();
                    continue;
                }
                const double currentTime = detail::glfw().GetTime();
                const float deltaTime = static_cast<float>(
                    std::clamp(currentTime - previousTime, 0.0, 0.05));
                previousTime = currentTime;
                if (updateHandler_) {
                    Diagnostics::trace(PipelineStage::Input, "Application::updateHandler");
                    updateHandler_(*this, deltaTime);
                }
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                if(!ensureContext("run -> render callback",__FILE__,__LINE__)) {exitCode=-1;break;}
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
                if (!rendered) {
                    std::cerr << "Rendering failed.\n";
                    exitCode = -1;
                    break;
                }
                Diagnostics::locate(__FILE__,__LINE__,"run -> glfwSwapBuffers");
                const auto swapSerial=detail::contextErrorSerial();
                detail::glfw().SwapBuffers(window_);
                if(swapSerial!=detail::contextErrorSerial()) {contextFailed_=true;exitCode=-1;break;}
            }
        }
    } catch (const std::exception& error) {
        Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "Application::run", error.what(), "Read the preceding stage/API error; user callback exceptions are rethrown after cleanup.");
        shutdown();
        running_ = false;
        detail::applicationActive()=false;
        throw;
    } catch (...) {
        Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "Application::run", "Non-standard exception in a callback or runtime operation.");
        shutdown();
        running_ = false;
        detail::applicationActive()=false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    detail::applicationActive()=false;
    return exitCode;
}

inline void Application::setMouseHandler(std::function<void(Application&,const MouseEvent&)> handler) {
    mouseHandler_=std::move(handler);
}
inline void Application::setCursorHandler(std::function<void(Application&,double,double)> handler) {
    cursorHandler_=std::move(handler);
}
inline std::optional<PickResult> Application::pickAt(double x,double y) const {
    if(!window_)return {};
    int ww=0,wh=0,fw=0,fh=0;
    detail::glfw().GetWindowSize(window_,&ww,&wh);detail::glfw().GetFramebufferSize(window_,&fw,&fh);
    if(ww<=0||wh<=0||fw<=0||fh<=0)return {};
    return graphics::pick(scene_,camera_,x*fw/ww,y*fh/wh,{0,0,double(fw),double(fh)});
}
inline std::optional<PickResult> Application::pickAtCursor() const {
    if(!window_)return {};
    double x=0,y=0;detail::glfw().GetCursorPos(window_,&x,&y);return pickAt(x,y);
}
inline AssetLoader& Application::getAssets() {
    if(!assets_)throw std::logic_error("AssetLoader is available during run callbacks only.");
    return *assets_;
}
} // namespace graphics

```

### after

```text
#pragma once
#include <algorithm>
#include <iostream>
#include <utility>
namespace graphics::detail {
inline bool& applicationActive() { static bool active=false;return active; }
inline unsigned& contextErrorSerial() { static unsigned value=0;return value; }
}
namespace graphics {

inline Application::~Application() {
    shutdown();
}

inline void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

inline void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

inline void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

inline void Application::setModifiedKeyHandler(
    std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

inline void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

inline void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

inline bool Application::ensureContext(const char* operation,const char* file,int line) {
    Diagnostics::locate(file,line,operation);
    if(contextFailed_) return false;
    if(!window_ || GetCurrentThreadId()!=contextThread_) {
        contextFailed_=true;
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime,"Application::ensureContext","Missing window or context used from a different thread.","Keep window creation, rendering and shutdown on the owning thread; inspect Application::run and the custom render handler.");
    }
    using CurrentProc=void* (WINAPI*)();
    const auto module=GetModuleHandleW(L"opengl32.dll");
    const auto current=module?reinterpret_cast<CurrentProc>(::GetProcAddress(module,"wglGetCurrentContext")):nullptr;
    if(nativeContext_ && detail::glfw().GetCurrentContext()==window_ && current && current()==nativeContext_) return true;
    const auto serial=detail::contextErrorSerial();
    detail::glfw().MakeContextCurrent(window_);
    if(serial!=detail::contextErrorSerial() || detail::glfw().GetCurrentContext()!=window_ || !current || !current()) {
        contextFailed_=true;
        return Diagnostics::fail(PipelineStage::Runtime,"glfwMakeContextCurrent","WGL did not attach the requested context; rendering stopped before GL commands.","Source is the failed bind call below. Check window lifetime, render-thread ownership and external handlers that detach/destroy the context. If those are valid, inspect the GPU driver/session; this log alone cannot prove a driver cause.");
    }
    nativeContext_=current();
    return true;
}

inline bool Application::initialize(const ApplicationConfig& config) {
    contextThread_=GetCurrentThreadId();contextFailed_=false;nativeContext_=nullptr;
    Diagnostics::trace(PipelineStage::Runtime, "Application::initialize");
    const auto root=detail::relativePathInside(detail::executableDirectory(),config.projectRoot);
    const auto runtime=detail::relativePathInside(root,config.runtimeDirectory);
    detail::glfw().load(runtime);
    detail::glfw().SetErrorCallback([](int code, const char* message) {
        ++detail::contextErrorSerial();
        Diagnostics::fail(PipelineStage::Runtime, "GLFW/WGL", std::to_string(code) + ": " + (message ? message : "unknown error"),
            "Context binding failed at Source/Operation. Inspect Application context lifetime/thread first; the OS/driver reason is in the original message. Render passes do not need separate WGL contexts.");
    });
    if (config.width <= 0 || config.height <= 0)
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "Application::initialize", "Window width and height must be positive.");
    if (!detail::glfw().Init())
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "glfwInit", "GLFW initialization failed; see the GLFW error above.");
    glfwInitialized_ = true;
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    detail::glfw().WindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    detail::glfw().WindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    detail::glfw().WindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    detail::glfw().WindowHint(GLFW_DEPTH_BITS, 24);
    window_ = detail::glfw().createWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "glfwCreateWindow", "Could not create an OpenGL 3.3 core window.", "Check the GLFW error above and update the graphics driver.");
    }
    if(!ensureContext("initialize -> attach new window",__FILE__,__LINE__)) return false;
    detail::glfw().SwapInterval(config.verticalSync ? 1 : 0);
    const auto vertexPath=config.shaderDirectory.empty()?std::filesystem::path{}:detail::relativePathInside(root,config.shaderDirectory)/"VertexShader.glsl";
    const auto fragmentPath=config.shaderDirectory.empty()?std::filesystem::path{}:detail::relativePathInside(root,config.shaderDirectory)/"FragmentShader.glsl";
    Diagnostics::locate(__FILE__,__LINE__,"initialize -> Renderer::initialize -> shader source/path setup");
    if(!renderer_.initialize(vertexPath,fragmentPath)) return false;
    assets_.emplace(root,config.features);
    renderOptions_.textures=config.features.textures;

    detail::glfw().SetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    detail::glfw().SetWindowUserPointer(window_, this);
    detail::glfw().SetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&] {
            if(app.modifiedKeyHandler_)app.modifiedKeyHandler_(app,key,action,mods);
            else if(app.keyHandler_)app.keyHandler_(app,key,action);
        });
    });
    detail::glfw().SetWindowFocusCallback(window_, [](GLFWwindow* window,int focused) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&]{if(app.focusHandler_)app.focusHandler_(app,focused==GLFW_TRUE);});
    });
    detail::glfw().SetMouseButtonCallback(window_, [](GLFWwindow* window,int button,int action,int mods) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&] {
            if(!app.mouseHandler_)return;
            MouseEvent event;event.button=button;event.action=action;event.mods=mods;
            detail::glfw().GetCursorPos(window,&event.windowX,&event.windowY);
            int ww=0,wh=0,fw=0,fh=0;
            detail::glfw().GetWindowSize(window,&ww,&wh);detail::glfw().GetFramebufferSize(window,&fw,&fh);
            if(ww<=0||wh<=0||fw<=0||fh<=0)return;
            event.framebufferX=event.windowX*fw/ww;event.framebufferY=event.windowY*fh/wh;
            app.mouseHandler_(app,event);
        });
    });
    detail::glfw().SetCursorPosCallback(window_, [](GLFWwindow* window,double x,double y) {
        auto& app=*static_cast<Application*>(detail::glfw().GetWindowUserPointer(window));
        app.dispatch([&]{if(app.cursorHandler_)app.cursorHandler_(app,x,y);});
    });
    return true;
}

inline void Application::shutdown() {
    assets_.reset();
    if (window_ != nullptr) {
        if(!contextFailed_ && ensureContext("shutdown -> GPU cleanup",__FILE__,__LINE__)) renderer_.destroy();
        else renderer_.abandon();
        detail::glfw().SetWindowUserPointer(window_, nullptr);
        detail::glfw().DestroyWindow(window_);
        window_ = nullptr;
    }
    if (glfwInitialized_) {
        detail::glfw().Terminate();
        glfwInitialized_ = false;
    }
}

inline void Application::requestClose() {
    if (window_ != nullptr) {
        detail::glfw().SetWindowShouldClose(window_, GLFW_TRUE);
    }
}

inline bool Application::renderScene() {
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Application::renderScene","No active window/context.");
    }
    if(!ensureContext("renderScene -> single pass",__FILE__,__LINE__)) return false;
    int width = 0;
    int height = 0;
    detail::glfw().GetFramebufferSize(window_, &width, &height);
    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

inline std::array<int,2> Application::getFramebufferSize() const {
    std::array<int,2> size{};
    if(window_) detail::glfw().GetFramebufferSize(window_,&size[0],&size[1]);
    return size;
}
inline bool Application::renderViews(const std::vector<RenderPass>& passes) {
    if(!window_) return Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Framebuffer,"Application::renderViews","No active window/context.");
    if(!ensureContext("renderViews -> shared context for all viewports",__FILE__,__LINE__)) return false;
    const auto size=getFramebufferSize();
    return renderer_.renderViews(scene_,passes,size[0],size[1]);
}

inline int Application::run(const ApplicationConfig& config) {
    Diagnostics::configure(config.diagnostics);
    Diagnostics::clear();
    if (running_) {
        Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime,"Application::run","Recursive run() is not allowed.");
        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }
    if(detail::applicationActive()) {Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime,"Application::run","Only one active Application is supported.");return -1;}
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    detail::applicationActive()=true;
    callbackError_=nullptr;
    running_ = true;
    int exitCode = 0;
    try {
        if (!initialize(config)) {
            std::cerr << "Application initialization failed.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Scene,"Application::initHandler","User initialization returned false.","Read the preceding API error; inspect the scene creation callback.");
            std::cerr << "User scene initialization failed.\n";
            exitCode = -1;
        } else {
            double previousTime = detail::glfw().GetTime();
            while (!detail::glfw().WindowShouldClose(window_)) {
                Diagnostics::locate(__FILE__,__LINE__,"run -> glfwPollEvents/input callbacks");
                detail::glfw().PollEvents();
                if(callbackError_)std::rethrow_exception(callbackError_);
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                int width = 0;
                int height = 0;
                detail::glfw().GetFramebufferSize(window_, &width, &height);
                if (width <= 0 || height <= 0) {
                    detail::glfw().WaitEvents();
                    if(callbackError_)std::rethrow_exception(callbackError_);
                    previousTime = detail::glfw().GetTime();
                    continue;
                }
                const double currentTime = detail::glfw().GetTime();
                const float deltaTime = static_cast<float>(
                    std::clamp(currentTime - previousTime, 0.0, 0.05));
                previousTime = currentTime;
                if (updateHandler_) {
                    Diagnostics::trace(PipelineStage::Input, "Application::updateHandler");
                    updateHandler_(*this, deltaTime);
                }
                if (detail::glfw().WindowShouldClose(window_)) {
                    break;
                }
                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "Motion update failed.\n";
                    exitCode = -1;
                    break;
                }
                if(!ensureContext("run -> render callback",__FILE__,__LINE__)) {exitCode=-1;break;}
                const bool rendered = renderHandler_
                    ? renderHandler_(*this) : renderScene();
                if (!rendered) {
                    std::cerr << "Rendering failed.\n";
                    exitCode = -1;
                    break;
                }
                Diagnostics::locate(__FILE__,__LINE__,"run -> glfwSwapBuffers");
                const auto swapSerial=detail::contextErrorSerial();
                detail::glfw().SwapBuffers(window_);
                if(swapSerial!=detail::contextErrorSerial()) {contextFailed_=true;exitCode=-1;break;}
            }
        }
    } catch (const std::exception& error) {
        Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "Application::run", error.what(), "Read the preceding stage/API error; user callback exceptions are rethrown after cleanup.");
        shutdown();
        running_ = false;
        detail::applicationActive()=false;
        throw;
    } catch (...) {
        Diagnostics::failAt(__FILE__,__LINE__,PipelineStage::Runtime, "Application::run", "Non-standard exception in a callback or runtime operation.");
        shutdown();
        running_ = false;
        detail::applicationActive()=false;
        throw; // 사용자 예외를 숨기지 않고 컨텍스트 자원부터 정리한다.
    }
    shutdown();
    running_ = false;
    detail::applicationActive()=false;
    return exitCode;
}

inline void Application::setMouseHandler(std::function<void(Application&,const MouseEvent&)> handler) {
    mouseHandler_=std::move(handler);
}
inline void Application::setCursorHandler(std::function<void(Application&,double,double)> handler) {
    cursorHandler_=std::move(handler);
}
inline std::optional<PickResult> Application::pickAt(double x,double y) const {
    if(!window_)return {};
    int ww=0,wh=0,fw=0,fh=0;
    detail::glfw().GetWindowSize(window_,&ww,&wh);detail::glfw().GetFramebufferSize(window_,&fw,&fh);
    if(ww<=0||wh<=0||fw<=0||fh<=0)return {};
    return graphics::pick(scene_,camera_,x*fw/ww,y*fh/wh,{0,0,double(fw),double(fh)});
}
inline std::optional<PickResult> Application::pickAtCursor() const {
    if(!window_)return {};
    double x=0,y=0;detail::glfw().GetCursorPos(window_,&x,&y);return pickAt(x,y);
}
inline AssetLoader& Application::getAssets() {
    if(!assets_)throw std::logic_error("AssetLoader is available during run callbacks only.");
    return *assets_;
}
} // namespace graphics

```

## framework/Toolchain.props

### before

```text
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <!-- Import after Microsoft.Cpp.Default.props and before Microsoft.Cpp.props. -->
  <PropertyGroup>
    <PlatformToolset Condition="'$(PlatformToolset)'=='' and Exists('$(VCTargetsPath)\Platforms\x64\PlatformToolsets\v145\Toolset.props')">v145</PlatformToolset>
    <PlatformToolset Condition="'$(PlatformToolset)'=='' and Exists('$(VCTargetsPath)\Platforms\x64\PlatformToolsets\v143\Toolset.props')">v143</PlatformToolset>
  </PropertyGroup>
</Project>

```

### after

```text
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <!-- Import after Microsoft.Cpp.Default.props and before Microsoft.Cpp.props. -->
  <PropertyGroup>
    <GraphicsToolsetRoot>$(VCTargetsPath)\Platforms\x64\PlatformToolsets</GraphicsToolsetRoot>
    <PlatformToolset Condition="'$(PlatformToolset)'=='' and '$(DefaultPlatformToolset)'!='' and Exists('$(GraphicsToolsetRoot)\$(DefaultPlatformToolset)\Toolset.props')">$(DefaultPlatformToolset)</PlatformToolset>
    <PlatformToolset Condition="'$(PlatformToolset)'=='' and Exists('$(GraphicsToolsetRoot)\v143\Toolset.props')">v143</PlatformToolset>
    <PlatformToolset Condition="'$(PlatformToolset)'=='' and Exists('$(GraphicsToolsetRoot)\v145\Toolset.props')">v145</PlatformToolset>
  </PropertyGroup>
  <Target Name="ExplainGraphicsToolchain" BeforeTargets="PrepareForBuild">
    <Message Importance="High" Text="OpenGL toolchain: VS=$(VisualStudioVersion); Platform=$(Platform); Toolset=$(PlatformToolset); VCTargetsPath=$(VCTargetsPath)" />
    <Error Condition="'$(PlatformToolset)'=='' or !Exists('$(GraphicsToolsetRoot)\$(PlatformToolset)\Toolset.props')" Text="OpenGL toolset unavailable: $(PlatformToolset). Expected $(GraphicsToolsetRoot)\$(PlatformToolset)\Toolset.props. VS 2022: install Desktop development with C++ and MSVC v143, then open the .sln (not .slnx on older VS). For a deliberate toolset override, install that toolset or change Platform Toolset in Project Properties." />
  </Target>
</Project>

```

