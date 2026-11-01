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
inline GLuint compileShader(GLenum kind,const std::string& sourceText) {
    Diagnostics::trace(PipelineStage::Shader, "compileShader");
    auto& api=gl();const auto shader=api.CreateShader(kind);
    if(!shader) { Diagnostics::fail(PipelineStage::Shader, "glCreateShader", "Shader allocation failed."); return 0; }
    const char* text=sourceText.c_str();api.ShaderSource(shader,1,&text,nullptr);api.CompileShader(shader);
    GLint success=0;api.GetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success) {
        GLint length=0;api.GetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
        std::string log(std::max(length,1), '\0');api.GetShaderInfoLog(shader,GLsizei(log.size()),nullptr,log.data());
        Diagnostics::fail(PipelineStage::Shader, kind==GL_VERTEX_SHADER?"vertex shader":"fragment shader", log,
            "Read the compiler line number; check GLSL version, declarations and shader inputs/outputs.");
        api.DeleteShader(shader);return 0;
    }
    return shader;
}
struct GpuVertex { float position[3], color[3], uv[2], normal[3]; };
inline GLuint uploadTexture(const TextureImage& image,bool repeat) {
    auto& api=gl();GLuint id=0;api.GenTextures(1,&id);if(!id)return 0;
    api.ActiveTexture(GL_TEXTURE0);api.BindTexture(GL_TEXTURE_2D,id);
    GLint previousAlignment=4;api.GetIntegerv(GL_UNPACK_ALIGNMENT,&previousAlignment);
    api.PixelStorei(GL_UNPACK_ALIGNMENT,1);
    api.TexImage2D(GL_TEXTURE_2D,0,GL_RGBA8,GLsizei(image.width),GLsizei(image.height),0,GL_RGBA,GL_UNSIGNED_BYTE,image.rgba.data());
    api.PixelStorei(GL_UNPACK_ALIGNMENT,previousAlignment);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
    api.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,repeat?GL_REPEAT:GL_CLAMP_TO_EDGE);
    if(!checkGl(PipelineStage::Asset, "uploadTexture")) {api.DeleteTextures(1,&id);return 0;}
    return id;
}
} // namespace detail

inline bool Renderer::initShader(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    auto& gl=detail::gl();
    const auto vertexText=vertexPath.empty()?std::string(detail::vertexShader):readTextFile(vertexPath);
    const auto fragmentText=fragmentPath.empty()?std::string(detail::fragmentShader):readTextFile(fragmentPath);
    if(vertexText.empty()||fragmentText.empty()) return Diagnostics::fail(PipelineStage::Shader, "Renderer::initShader",
        "Shader source is empty or unreadable: vertex=" + vertexPath.u8string() + "; fragment=" + fragmentPath.u8string(),
        "Use embedded shaders (empty shaderDirectory), or copy both GLSL files to the configured project-relative directory.");
    const auto vertex=detail::compileShader(GL_VERTEX_SHADER,vertexText);if(!vertex)return false;
    const auto fragment=detail::compileShader(GL_FRAGMENT_SHADER,fragmentText);
    if(!fragment){gl.DeleteShader(vertex);return false;}
    shaderProgramID_=gl.CreateProgram();
    if(!shaderProgramID_){gl.DeleteShader(vertex);gl.DeleteShader(fragment);return false;}
    gl.AttachShader(shaderProgramID_,vertex);gl.AttachShader(shaderProgramID_,fragment);gl.LinkProgram(shaderProgramID_);
    gl.DeleteShader(vertex);gl.DeleteShader(fragment);
    GLint success=0;gl.GetProgramiv(shaderProgramID_,GL_LINK_STATUS,&success);
    if(!success) {
        GLint size=0;gl.GetProgramiv(shaderProgramID_,GL_INFO_LOG_LENGTH,&size);
        std::string log(std::max(size,1),'\0');gl.GetProgramInfoLog(shaderProgramID_,GLsizei(log.size()),nullptr,log.data());
        return Diagnostics::fail(PipelineStage::Shader, "glLinkProgram", log, "Match vertex outputs and fragment inputs; inspect the linker log.");
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
    return mvpLocation_>=0 || Diagnostics::fail(PipelineStage::Shader, "glGetUniformLocation", "Required uMVP uniform is missing or optimized away.");
}
inline bool Renderer::initialize(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    detail::gl().load();destroy();
    if(!initShader(vertexPath,fragmentPath)){destroy();return false;}
    auto& gl=detail::gl();gl.GenVertexArrays(1,&vao_);gl.GenBuffers(1,&vbo_);gl.GenBuffers(1,&ebo_);
    if(!vao_||!vbo_||!ebo_){destroy();return Diagnostics::fail(PipelineStage::VertexInput, "Renderer::initialize", "VAO/VBO/EBO allocation failed.");}
    gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    using V=detail::GpuVertex;
    gl.VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,position)));
    gl.VertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,color)));
    gl.VertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,uv)));
    gl.VertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,normal)));
    for(GLuint i=0;i<4;++i)gl.EnableVertexAttribArray(i);
    gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);return detail::checkGl(PipelineStage::VertexInput, "Renderer::initialize vertex layout");
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
            if(!c||!DiagramSystem::validPolyhedron(c->polyhedron)){cleanup();return Diagnostics::fail(PipelineStage::VertexInput, "Renderer::upload", "Missing component or invalid mesh, indices, colors, UVs or normals.", "Inspect the entity's mesh data before GPU upload.", id);}
            for(std::size_t faceIndex=0;faceIndex<c->polyhedron.faces.size();++faceIndex) {
                const auto& face=c->polyhedron.faces[faceIndex];
                if(vertices.size()+face.localVertices.size()>std::numeric_limits<GLuint>::max()) {cleanup();return false;}
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
                if(triangles.size()>std::numeric_limits<GLsizei>::max()||outline.size()>std::numeric_limits<GLsizei>::max()){cleanup();return false;}
                DrawRange range;range.entityId=id;range.faceIndex=faceIndex;range.firstIndex=indices.size();range.count=GLsizei(triangles.size());
                for(auto index:triangles)indices.push_back(first+index);
                range.outlineFirstIndex=indices.size();range.outlineCount=GLsizei(outline.size());
                for(auto index:outline)indices.push_back(first+index);
                ranges.push_back(range);
                if(textureMode&&face.material.useTexture&&stagedTextures.find(face.material.texture)==stagedTextures.end()) {
                    if(!detail::checkGl(PipelineStage::Asset, "before uploadTexture")){cleanup();return false;}
                    const auto texture=detail::uploadTexture(*face.material.texture,face.material.repeatTexture);
                    if(!texture){cleanup();return false;}
                    try{stagedTextures.emplace(face.material.texture,texture);}catch(...){gl.DeleteTextures(1,&texture);throw;}
                }
            }
        }
        for(std::size_t lineIndex=0;lineIndex<scene.getLines().size();++lineIndex) {
            const auto& line=scene.getLines()[lineIndex];
            if(vertices.size()>std::numeric_limits<GLuint>::max()-2){cleanup();return false;}
            const auto first=GLuint(vertices.size());
            for(auto p:{line.start,line.end})vertices.push_back({{p.x,p.y,p.z},{line.color.red,line.color.green,line.color.blue},{0,0},{0,0,0}});
            DrawRange range;range.lineId=lineIndex;range.firstIndex=indices.size();range.count=2;
            ranges.push_back(range);indices.insert(indices.end(),{first,first+1});
        }
        if(vertices.size()>std::size_t(std::numeric_limits<GLsizeiptr>::max())/sizeof(detail::GpuVertex)
            ||indices.size()>std::size_t(std::numeric_limits<GLsizeiptr>::max())/sizeof(GLuint)){cleanup();return false;}
        if(!detail::checkGl(PipelineStage::VertexInput, "before buffer upload")){cleanup();return false;}
        gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);
        gl.BufferData(GL_ARRAY_BUFFER,GLsizeiptr(vertices.size()*sizeof(detail::GpuVertex)),vertices.empty()?nullptr:vertices.data(),GL_STATIC_DRAW);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
        gl.BufferData(GL_ELEMENT_ARRAY_BUFFER,GLsizeiptr(indices.size()*sizeof(GLuint)),indices.empty()?nullptr:indices.data(),GL_STATIC_DRAW);
        gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);
        if(!detail::checkGl(PipelineStage::VertexInput, "glBufferData")){cleanup();return false;}
        for(const auto& item:textures_)gl.DeleteTextures(1,&item.second);
        textures_=std::move(stagedTextures);drawRanges_=std::move(ranges);return true;
    } catch(...) {cleanup();throw;}
}
inline bool Renderer::draw(const Scene& scene,const glm::mat4& view,const glm::mat4& projection,
                           const RenderOptions& options) const {
    Diagnostics::trace(PipelineStage::Transform, "Renderer::draw");
    if(!detail::finiteMatrix(view)||!detail::finiteMatrix(projection))return Diagnostics::fail(PipelineStage::Transform, "Renderer::draw", "View or projection matrix contains NaN/Inf.");
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto world=scene.lineWorldMatrix(*range.lineId);if(!world)return false;matrix=*world;
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
        }
        if(!detail::finiteMatrix(matrix)||!detail::finiteMatrix(projection*view*matrix))return Diagnostics::fail(PipelineStage::Transform, "Renderer::draw", "World or MVP matrix contains NaN/Inf.", "Check parent transforms, scales and motion samples.", range.entityId);
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
            if(!detail::checkGl(PipelineStage::Raster, "glDrawElements")) {
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
    gl.Uniform1i(overrideLocation_,0);gl.BindTexture(GL_TEXTURE_2D,0);gl.BindVertexArray(0);gl.UseProgram(0);return detail::checkGl(PipelineStage::Raster, "Renderer::draw outlines and cleanup");
}
inline bool Renderer::renderPass(const Scene& scene,const RenderPass& pass,int width,int height) {
    Diagnostics::trace(PipelineStage::Framebuffer,"Renderer::renderPass");
    const auto& camera=pass.camera;const auto& options=pass.options;const auto& viewport=pass.viewport;
    if(!shaderProgramID_||!vao_)return Diagnostics::fail(PipelineStage::VertexInput,"Renderer::renderPass","Renderer is not initialized.");
    if(width<=0||height<=0)return true;
    if(viewport.x<0 || viewport.y<0 || viewport.x>width || viewport.y>height || viewport.width<0 || viewport.height<0
        || viewport.width>width-viewport.x || viewport.height>height-viewport.y)
        return Diagnostics::fail(PipelineStage::Framebuffer,"Renderer::renderPass","Viewport lies outside the framebuffer.");
    if(viewport.width==0 || viewport.height==0) return true;
    if(!CameraSystem::validCamera(camera)||!detail::finiteVector(options.lightDirection)
        ||glm::length(options.lightDirection)<1e-12f||!detail::validColor(options.lightColor)
        ||!detail::validColor(options.outlineColor)
        ||!std::isfinite(options.background.r)||!std::isfinite(options.background.g)
        ||!std::isfinite(options.background.b)||!std::isfinite(options.background.a)
        ||(options.cull!=CullMode::Inherit && options.cull!=CullMode::None && options.cull!=CullMode::Back && options.cull!=CullMode::Front))
        return Diagnostics::fail(PipelineStage::Transform,"Renderer::renderPass","Invalid camera, light, color or culling mode.","Check near/far planes, FOV, target/up directions, positive orthographic extent and finite render options.");
    if(uploadedScene_!=&scene||uploadedRevision_!=scene.getGeometryRevision()||uploadedTextures_!=options.textures) {
        if(!upload(scene,options.textures))return false;
        uploadedScene_=&scene;uploadedRevision_=scene.getGeometryRevision();uploadedTextures_=options.textures;
    }
    auto& gl=detail::gl();if(!detail::checkGl(PipelineStage::Framebuffer, "before Renderer::render"))return false;
    gl.Viewport(viewport.x,viewport.y,viewport.width,viewport.height);gl.LineWidth(1);
    if(options.depthTest)gl.Enable(GL_DEPTH_TEST);else gl.Disable(GL_DEPTH_TEST);
    gl.Disable(GL_CULL_FACE);gl.Disable(GL_BLEND);gl.Enable(GL_SCISSOR_TEST);
    gl.Scissor(viewport.x,viewport.y,viewport.width,viewport.height);
    gl.PolygonMode(GL_FRONT_AND_BACK,GL_FILL);gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);gl.ColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    gl.ClearColor(options.background.r,options.background.g,options.background.b,options.background.a);
    const GLbitfield mask=(pass.clearColor?GL_COLOR_BUFFER_BIT:0)|(pass.clearDepth?GL_DEPTH_BUFFER_BIT:0);
    if(mask) gl.Clear(mask);
    if(!detail::checkGl(PipelineStage::Framebuffer,"viewport/scissor/clear")) {gl.Disable(GL_SCISSOR_TEST);return false;}
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
    if(passes.empty()) return Diagnostics::fail(PipelineStage::Framebuffer,"Renderer::renderViews","Need at least one render pass.");
    if(!shaderProgramID_ || !vao_) return Diagnostics::fail(PipelineStage::VertexInput,"Renderer::renderViews","Renderer is not initialized.");
    auto& gl=detail::gl();
    gl.Disable(GL_SCISSOR_TEST);gl.Viewport(0,0,width,height);gl.DepthMask(GL_TRUE);gl.ColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
    gl.ClearColor(.08f,.08f,.1f,1);gl.Clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(!detail::checkGl(PipelineStage::Framebuffer,"Renderer::renderViews full-frame clear")) return false;
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
