# 전체 코드 before / after

작업 시작 기준: 7f69a6777b18d91d1d78b6d98717d0e2ae439dcb

각 변경 파일의 전체 원본과 최종 코드를 기록한다. 빌드·테스트는 실행하지 않았다.

## framework/detail/AssimpLoader.inl

### before

없음

### after

```cpp
#pragma once
#include "../dependencies/assimp/include/assimp/cimport.h"
#include "../dependencies/assimp/include/assimp/scene.h"
#include "../dependencies/assimp/include/assimp/postprocess.h"
#include <functional>
namespace graphics {
namespace detail {
struct AssimpApi {
    Module module;
    bool ready = false;
    decltype(&::aiImportFile) importFile = nullptr;
    decltype(&::aiReleaseImport) releaseImport = nullptr;
    decltype(&::aiGetErrorString) getError = nullptr;
    decltype(&::aiGetMaterialColor) getColor = nullptr;
    decltype(&::aiGetMaterialTexture) getTexture = nullptr;
    void load(const std::filesystem::path& root) {
        if(ready) return;
        module.open((root.empty()?frameworkDirectory():std::filesystem::absolute(root))
                    / "dependencies/assimp/bin/assimp-vc145-mt.dll");
        importFile=module.symbol<decltype(importFile)>("aiImportFile");
        releaseImport=module.symbol<decltype(releaseImport)>("aiReleaseImport");
        getError=module.symbol<decltype(getError)>("aiGetErrorString");
        getColor=module.symbol<decltype(getColor)>("aiGetMaterialColor");
        getTexture=module.symbol<decltype(getTexture)>("aiGetMaterialTexture");
        ready=true;
    }
};
inline AssimpApi& assimp() { static AssimpApi api; return api; }
inline glm::mat4 assimpMatrix(const aiMatrix4x4& a) {
    return glm::mat4(a.a1,a.b1,a.c1,a.d1, a.a2,a.b2,a.c2,a.d2,
                     a.a3,a.b3,a.c3,a.d3, a.a4,a.b4,a.c4,a.d4);
}
}
inline ModelResult AssetLoader::loadModel(const std::filesystem::path& relative,const LoadOptions& options) const {
        ModelResult result;
        if(!features_.models) {result.error="Assimp model mode is disabled.";return result;}
        if(!features_.textures&&!options.texturePath.empty()) {result.error="Texture mode is disabled.";return result;}
        try {
            const auto path=resolve(relative);
            auto& api=detail::assimp(); api.load(runtimeRoot_);
            const aiScene* scene=api.importFile(path.u8string().c_str(),aiProcess_Triangulate
                |aiProcess_JoinIdenticalVertices|aiProcess_GenSmoothNormals|aiProcess_ValidateDataStructure);
            if(!scene) throw std::runtime_error(api.getError());
            struct Release { detail::AssimpApi& api; const aiScene* scene; ~Release(){api.releaseImport(scene);} } release{api,scene};
            if(!scene->mRootNode) throw std::runtime_error("Model has no root node.");
            Model model; model.shape=GLShape::Imported; model.name=relative.u8string();
            std::map<unsigned int,Material> materials;
            std::map<unsigned int,unsigned int> uvChannels;
            for(unsigned int i=0;i<scene->mNumMaterials;++i) {
                Material material; aiColor4D color;
                if(api.getColor(scene->mMaterials[i],AI_MATKEY_BASE_COLOR,&color)==AI_SUCCESS
                    ||api.getColor(scene->mMaterials[i],AI_MATKEY_COLOR_DIFFUSE,&color)==AI_SUCCESS)
                    material.color={color.r,color.g,color.b};
                unsigned int uvChannel=0;
                if(features_.textures) {
                    if(!options.texturePath.empty()) material.texture=texture(options.texturePath,options.flipTextureRows);
                    else {
                        aiString texturePath;
                        aiReturn found=api.getTexture(scene->mMaterials[i],aiTextureType_BASE_COLOR,0,&texturePath,nullptr,&uvChannel,nullptr,nullptr,nullptr,nullptr);
                        if(found!=AI_SUCCESS) found=api.getTexture(scene->mMaterials[i],aiTextureType_DIFFUSE,0,&texturePath,nullptr,&uvChannel,nullptr,nullptr,nullptr,nullptr);
                        if(found==AI_SUCCESS) {
                            const aiTexture* embedded=scene->GetEmbeddedTexture(texturePath.C_Str());
                            if(embedded) {
                                TextureImage image;
                                if(embedded->mHeight==0) {
                                    const auto* bytes=reinterpret_cast<const std::uint8_t*>(embedded->pcData);
                                    std::vector<std::uint8_t> data(bytes,bytes+embedded->mWidth);
                                    image=detail::decodeImage({},&data,options.flipTextureRows);
                                } else {
                                    image.width=embedded->mWidth;image.height=embedded->mHeight;
                                    if(!image.width||!image.height||image.width>16384||image.height>16384)
                                        throw std::runtime_error("Invalid embedded texture size.");
                                    image.rgba.resize(std::size_t(image.width)*image.height*4);
                                    for(unsigned int y=0;y<image.height;++y) for(unsigned int x=0;x<image.width;++x) {
                                        const auto& pixel=embedded->pcData[std::size_t(y)*image.width+x];
                                        const auto dest=(std::size_t(options.flipTextureRows?image.height-1-y:y)*image.width+x)*4;
                                        image.rgba[dest]=pixel.r;image.rgba[dest+1]=pixel.g;image.rgba[dest+2]=pixel.b;image.rgba[dest+3]=pixel.a;
                                    }
                                }
                                material.texture=std::make_shared<const TextureImage>(std::move(image));
                            } else material.texture=texture(relative.parent_path()/std::filesystem::u8path(texturePath.C_Str()),options.flipTextureRows);
                        }
                    }
                    material.useTexture=bool(material.texture);
                }
                materials[i]=material;uvChannels[i]=uvChannel;
            }
            std::function<void(const aiNode*,const glm::mat4&)> visit;
            visit=[&](const aiNode* node,const glm::mat4& parent) {
                const auto matrix=parent*detail::assimpMatrix(node->mTransformation);
                const auto linear=glm::mat3(matrix);const auto determinant=glm::determinant(linear);
                const bool invertible=std::isfinite(determinant)&&std::abs(determinant)>1e-12f;
                const auto normalMatrix=invertible?glm::transpose(glm::inverse(linear)):glm::mat3(1);
                for(unsigned int n=0;n<node->mNumMeshes;++n) {
                    if(node->mMeshes[n]>=scene->mNumMeshes) throw std::runtime_error("Invalid node mesh index.");
                    const auto* source=scene->mMeshes[node->mMeshes[n]];
                    if(!source->HasPositions()) continue;
                    Mesh mesh; mesh.name=node->mName.C_Str();mesh.material=materials[source->mMaterialIndex];
                    const auto uvChannel=uvChannels[source->mMaterialIndex];
                    for(unsigned int v=0;v<source->mNumVertices;++v) {
                        const auto p=source->mVertices[v];mesh.localVertices.push_back(glm::vec3(matrix*glm::vec4(p.x,p.y,p.z,1)));
                        if(source->HasNormals()&&invertible) {
                            const auto normal=source->mNormals[v];const auto transformed=normalMatrix*glm::vec3(normal.x,normal.y,normal.z);
                            mesh.vertexNormals.push_back(glm::length(transformed)>1e-12f?glm::normalize(transformed):glm::vec3(0));
                        }
                        if(features_.textures&&source->HasTextureCoords(uvChannel)) {
                            const auto uv=source->mTextureCoords[uvChannel][v];mesh.texcoords.push_back({uv.x,uv.y});
                        }
                        if(source->HasVertexColors(0)) {const auto c=source->mColors[0][v];mesh.vertexColors.push_back({c.r,c.g,c.b});}
                    }
                    for(unsigned int f=0;f<source->mNumFaces;++f) {
                        const auto& face=source->mFaces[f];if(face.mNumIndices!=3) continue;
                        mesh.triangleIndices.insert(mesh.triangleIndices.end(),{face.mIndices[0],
                            face.mIndices[determinant<0?2:1],face.mIndices[determinant<0?1:2]});
                    }
                    if(mesh.triangleIndices.empty()) continue;
                    if(mesh.vertexNormals.empty()) recalculateNormals(mesh);
                    if(mesh.material.useTexture&&mesh.texcoords.empty()) throw std::runtime_error("Textured mesh is missing the requested UV channel.");
                    model.faces.push_back(std::move(mesh));
                }
                for(unsigned int i=0;i<node->mNumChildren;++i) visit(node->mChildren[i],matrix);
            };
            visit(scene->mRootNode,glm::mat4(1));
            return detail::finishLoad(std::move(model),options);
        } catch(const std::exception& e) {result.error=e.what();return result;}
    }
} // namespace graphics

```

## framework/detail/PrimitiveLoader.inl

### before

없음

### after

```cpp
#pragma once
#include <fstream>
#include <sstream>
#include <locale>
namespace graphics {
namespace detail {
inline std::size_t objIndex(const std::string& text,std::size_t count) {
    std::size_t consumed=0;
    const auto index=std::stoll(text,&consumed);
    if(consumed!=text.size() || index==0) throw std::runtime_error("Invalid OBJ index.");
    const auto resolved=index>0?index-1:static_cast<long long>(count)+index;
    if(resolved<0 || static_cast<unsigned long long>(resolved)>=count)
        throw std::runtime_error("OBJ index is out of range.");
    return std::size_t(resolved);
}
}
inline ModelResult AssetLoader::loadPrimitive(const std::filesystem::path& relative,const LoadOptions& options) const {
        try {
            if(!options.texturePath.empty()&&!features_.textures)
                throw std::runtime_error("Enable texture mode before specifying texturePath.");
            std::ifstream input(resolve(relative),std::ios::binary);
            if(!input) throw std::runtime_error("Cannot open primitive data.");
            input.imbue(std::locale::classic());
            std::vector<glm::vec3> positions,normals;
            std::vector<glm::vec2> uvs;
            Model model; model.shape=GLShape::Custom; model.name=relative.u8string();
            std::string line; std::size_t lineNumber=0;
            while(std::getline(input,line)) {
                ++lineNumber;
                if(lineNumber==1 && line.compare(0,3,"\xEF\xBB\xBF")==0) line.erase(0,3);
                line=line.substr(0,line.find('#'));
                std::istringstream row(line); row.imbue(std::locale::classic());
                std::string kind; row>>kind;
                if(kind=="v"||kind=="vn") {
                    glm::vec3 v; if(!(row>>v.x>>v.y>>v.z)||!detail::finiteVector(v))
                        throw std::runtime_error("Invalid vector at line "+std::to_string(lineNumber));
                    (kind=="v"?positions:normals).push_back(v);
                } else if(kind=="vt") {
                    glm::vec2 uv; if(!(row>>uv.x>>uv.y)||!std::isfinite(uv.x)||!std::isfinite(uv.y))
                        throw std::runtime_error("Invalid UV at line "+std::to_string(lineNumber));
                    uvs.push_back(uv);
                } else if(kind=="f") {
                    Mesh mesh; mesh.name="face_"+std::to_string(model.faces.size());
                    std::string corner; bool hasUV=false,hasNormal=false; std::size_t uvCount=0,normalCount=0;
                    while(row>>corner) {
                        const auto first=corner.find('/');
                        const auto second=first==std::string::npos?first:corner.find('/',first+1);
                        mesh.localVertices.push_back(positions.at(detail::objIndex(corner.substr(0,first),positions.size())));
                        glm::vec2 uv(0); glm::vec3 normal(0);
                        if(first!=std::string::npos) {
                            const auto part=corner.substr(first+1,second==std::string::npos?second:second-first-1);
                            if(!part.empty()) {uv=uvs.at(detail::objIndex(part,uvs.size()));hasUV=true;++uvCount;}
                        }
                        if(second!=std::string::npos) {
                            normal=normals.at(detail::objIndex(corner.substr(second+1),normals.size()));hasNormal=true;++normalCount;
                        }
                        mesh.texcoords.push_back(uv);mesh.vertexNormals.push_back(normal);
                    }
                    if((hasUV&&uvCount!=mesh.localVertices.size())||(hasNormal&&normalCount!=mesh.localVertices.size()))
                        throw std::runtime_error("Mixed OBJ corner attributes in one face.");
                    mesh.triangleIndices=detail::triangulate(mesh.localVertices);
                    if(!hasNormal) recalculateNormals(mesh);
                    if(!hasUV || !features_.textures) mesh.texcoords.clear();
                    const auto& indices=mesh.triangleIndices;
                    mesh.normal=glm::normalize(glm::cross(mesh.localVertices[indices[1]]-mesh.localVertices[indices[0]],
                                                          mesh.localVertices[indices[2]]-mesh.localVertices[indices[0]]));
                    model.faces.push_back(std::move(mesh));
                } else if(!kind.empty()&&kind!="g"&&kind!="o"&&kind!="s"&&kind!="mtllib"&&kind!="usemtl")
                    throw std::runtime_error("Unsupported primitive record at line "+std::to_string(lineNumber));
            }
            if(input.bad()) throw std::runtime_error("Primitive file read failed.");
            if(options.mergeCoplanarFaces) detail::mergeCoplanar(model);
            if(!options.texturePath.empty()) {
                auto image=texture(options.texturePath,options.flipTextureRows);
                for(auto& mesh:model.faces) {mesh.material.texture=image;mesh.material.useTexture=true;}
            }
            return detail::finishLoad(std::move(model),options);
        } catch(const std::exception& e) {ModelResult result;result.error=e.what();return result;}
    }

} // namespace graphics

```

## framework/detail/Renderer.inl

### before

```cpp
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
    auto& api=gl();const auto shader=api.CreateShader(kind);if(!shader)return 0;
    const char* text=sourceText.c_str();api.ShaderSource(shader,1,&text,nullptr);api.CompileShader(shader);
    GLint success=0;api.GetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success) {
        GLint length=0;api.GetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
        std::string log(std::max(length,1), '\0');api.GetShaderInfoLog(shader,GLsizei(log.size()),nullptr,log.data());
        std::cerr<<"Shader compile failed: "<<log<<'\n';api.DeleteShader(shader);return 0;
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
    if(api.GetError()!=GL_NO_ERROR) {api.DeleteTextures(1,&id);return 0;}
    return id;
}
} // namespace detail

inline bool Renderer::initShader(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    auto& gl=detail::gl();
    const auto vertexText=vertexPath.empty()?std::string(detail::vertexShader):readTextFile(vertexPath);
    const auto fragmentText=fragmentPath.empty()?std::string(detail::fragmentShader):readTextFile(fragmentPath);
    if(vertexText.empty()||fragmentText.empty()) return false;
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
        std::cerr<<"Shader link failed: "<<log<<'\n';return false;
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
    return mvpLocation_>=0;
}
inline bool Renderer::initialize(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    detail::gl().load();destroy();
    if(!initShader(vertexPath,fragmentPath)){destroy();return false;}
    auto& gl=detail::gl();gl.GenVertexArrays(1,&vao_);gl.GenBuffers(1,&vbo_);gl.GenBuffers(1,&ebo_);
    if(!vao_||!vbo_||!ebo_){destroy();return false;}
    gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    using V=detail::GpuVertex;
    gl.VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,position)));
    gl.VertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,color)));
    gl.VertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,uv)));
    gl.VertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,normal)));
    for(GLuint i=0;i<4;++i)gl.EnableVertexAttribArray(i);
    gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);return true;
}
inline bool Renderer::upload(const Scene& scene,bool textureMode) {
    std::vector<detail::GpuVertex> vertices;
    std::vector<GLuint> indices;std::vector<DrawRange> ranges;
    TextureMap stagedTextures;auto& gl=detail::gl();
    auto cleanup=[&](){for(const auto& item:stagedTextures)gl.DeleteTextures(1,&item.second);};
    try {
        for(auto id:scene.getIds()) {
            const auto* c=scene.getComponent(id);if(!c||!DiagramSystem::validPolyhedron(c->polyhedron)){cleanup();return false;}
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
                    while(gl.GetError()!=GL_NO_ERROR){}
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
        while(gl.GetError()!=GL_NO_ERROR){}
        gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);
        gl.BufferData(GL_ARRAY_BUFFER,GLsizeiptr(vertices.size()*sizeof(detail::GpuVertex)),vertices.empty()?nullptr:vertices.data(),GL_STATIC_DRAW);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
        gl.BufferData(GL_ELEMENT_ARRAY_BUFFER,GLsizeiptr(indices.size()*sizeof(GLuint)),indices.empty()?nullptr:indices.data(),GL_STATIC_DRAW);
        gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);
        if(gl.GetError()!=GL_NO_ERROR){cleanup();return false;}
        for(const auto& item:textures_)gl.DeleteTextures(1,&item.second);
        textures_=std::move(stagedTextures);drawRanges_=std::move(ranges);return true;
    } catch(...) {cleanup();throw;}
}
inline bool Renderer::draw(const Scene& scene,const glm::mat4& view,const glm::mat4& projection,
                           const RenderOptions& options) const {
    if(!detail::finiteMatrix(view)||!detail::finiteMatrix(projection))return false;
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) matrix=DiagramSystem::modelMatrix(scene.getComponent(*line.owner)->transform);
        } else {
            const auto* c=scene.getComponent(range.entityId);
            matrix=DiagramSystem::faceModelMatrix(*c,c->polyhedron.faces[range.faceIndex]);
        }
        if(!detail::finiteMatrix(matrix)||!detail::finiteMatrix(projection*view*matrix))return false;
    }
    auto& gl=detail::gl();gl.UseProgram(shaderProgramID_);gl.BindVertexArray(vao_);
    gl.ActiveTexture(GL_TEXTURE0);gl.Uniform1i(textureLocation_,0);
    gl.Uniform3f(lightDirectionLocation_,options.lightDirection.x,options.lightDirection.y,options.lightDirection.z);
    gl.Uniform3f(lightColorLocation_,options.lightColor.red,options.lightColor.green,options.lightColor.blue);
    gl.Uniform3f(outlineColorLocation_,options.outlineColor.red,options.outlineColor.green,options.outlineColor.blue);
    for(const auto& range:drawRanges_) {
        glm::mat4 model(1);const Mesh* face=nullptr;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;model=DiagramSystem::modelMatrix(owner->transform);}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;
            model=DiagramSystem::faceModelMatrix(*c,*face);
        }
        const auto mvp=projection*view*model;
        const float determinant=glm::determinant(glm::mat3(model));
        const auto normalMatrix=std::isfinite(determinant)&&std::abs(determinant)>1e-12f
            ?glm::transpose(glm::inverse(glm::mat3(model))):glm::mat3(1);
        gl.UniformMatrix4fv(mvpLocation_,1,GL_FALSE,glm::value_ptr(mvp));
        gl.UniformMatrix4fv(modelLocation_,1,GL_FALSE,glm::value_ptr(model));
        gl.UniformMatrix3fv(normalLocation_,1,GL_FALSE,glm::value_ptr(normalMatrix));
        gl.Uniform1i(lightingLocation_,face&&options.lighting);gl.Uniform1i(overrideLocation_,0);
        const bool textured=face&&options.textures&&face->material.useTexture;
        gl.Uniform1i(hasTextureLocation_,textured);
        if(textured){gl.BindTexture(GL_TEXTURE_2D,textures_.at(face->material.texture));
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);}
        else gl.BindTexture(GL_TEXTURE_2D,0);
        if(!face||!options.wireframe)
            gl.DrawElements(face?GL_TRIANGLES:GL_LINES,range.count,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.firstIndex*sizeof(GLuint)));
        if(face&&(options.wireframe||face->outline!=OutlineMode::None)) {
            gl.Uniform1i(hasTextureLocation_,0);gl.Uniform1i(lightingLocation_,0);gl.Uniform1i(overrideLocation_,1);
            gl.DepthFunc(GL_LEQUAL);gl.DepthMask(GL_FALSE);
            gl.DrawElements(GL_LINES,range.outlineCount,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.outlineFirstIndex*sizeof(GLuint)));
            gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);
        }
    }
    gl.Uniform1i(overrideLocation_,0);gl.BindTexture(GL_TEXTURE_2D,0);gl.BindVertexArray(0);gl.UseProgram(0);return gl.GetError()==GL_NO_ERROR;
}
inline bool Renderer::render(const Scene& scene,const Camera& camera,int width,int height,const RenderOptions& options) {
    if(!shaderProgramID_||!vao_)return false;if(width<=0||height<=0)return true;
    if(!CameraSystem::validCamera(camera)||!detail::finiteVector(options.lightDirection)
        ||glm::length(options.lightDirection)<1e-12f||!detail::validColor(options.lightColor)
        ||!detail::validColor(options.outlineColor)
        ||!std::isfinite(options.background.r)||!std::isfinite(options.background.g)
        ||!std::isfinite(options.background.b)||!std::isfinite(options.background.a))return false;
    if(uploadedScene_!=&scene||uploadedRevision_!=scene.getGeometryRevision()||uploadedTextures_!=options.textures) {
        if(!upload(scene,options.textures))return false;
        uploadedScene_=&scene;uploadedRevision_=scene.getGeometryRevision();uploadedTextures_=options.textures;
    }
    auto& gl=detail::gl();while(gl.GetError()!=GL_NO_ERROR){}
    gl.Viewport(0,0,width,height);gl.LineWidth(1);
    if(options.depthTest)gl.Enable(GL_DEPTH_TEST);else gl.Disable(GL_DEPTH_TEST);
    gl.Disable(GL_CULL_FACE);gl.Disable(GL_BLEND);gl.Disable(GL_SCISSOR_TEST);
    gl.PolygonMode(GL_FRONT_AND_BACK,GL_FILL);gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);
    gl.ClearColor(options.background.r,options.background.g,options.background.b,options.background.a);
    gl.Clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    return draw(scene,CameraSystem::viewMatrix(camera),CameraSystem::projectionMatrix(camera,float(width)/height),options);
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

```cpp
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
    auto& api=gl();const auto shader=api.CreateShader(kind);if(!shader)return 0;
    const char* text=sourceText.c_str();api.ShaderSource(shader,1,&text,nullptr);api.CompileShader(shader);
    GLint success=0;api.GetShaderiv(shader,GL_COMPILE_STATUS,&success);
    if(!success) {
        GLint length=0;api.GetShaderiv(shader,GL_INFO_LOG_LENGTH,&length);
        std::string log(std::max(length,1), '\0');api.GetShaderInfoLog(shader,GLsizei(log.size()),nullptr,log.data());
        std::cerr<<"Shader compile failed: "<<log<<'\n';api.DeleteShader(shader);return 0;
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
    if(api.GetError()!=GL_NO_ERROR) {api.DeleteTextures(1,&id);return 0;}
    return id;
}
} // namespace detail

inline bool Renderer::initShader(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    auto& gl=detail::gl();
    const auto vertexText=vertexPath.empty()?std::string(detail::vertexShader):readTextFile(vertexPath);
    const auto fragmentText=fragmentPath.empty()?std::string(detail::fragmentShader):readTextFile(fragmentPath);
    if(vertexText.empty()||fragmentText.empty()) return false;
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
        std::cerr<<"Shader link failed: "<<log<<'\n';return false;
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
    return mvpLocation_>=0;
}
inline bool Renderer::initialize(const std::filesystem::path& vertexPath,const std::filesystem::path& fragmentPath) {
    detail::gl().load();destroy();
    if(!initShader(vertexPath,fragmentPath)){destroy();return false;}
    auto& gl=detail::gl();gl.GenVertexArrays(1,&vao_);gl.GenBuffers(1,&vbo_);gl.GenBuffers(1,&ebo_);
    if(!vao_||!vbo_||!ebo_){destroy();return false;}
    gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
    using V=detail::GpuVertex;
    gl.VertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,position)));
    gl.VertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,color)));
    gl.VertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,uv)));
    gl.VertexAttribPointer(3,3,GL_FLOAT,GL_FALSE,sizeof(V),reinterpret_cast<void*>(offsetof(V,normal)));
    for(GLuint i=0;i<4;++i)gl.EnableVertexAttribArray(i);
    gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);return true;
}
inline bool Renderer::upload(const Scene& scene,bool textureMode) {
    std::vector<detail::GpuVertex> vertices;
    std::vector<GLuint> indices;std::vector<DrawRange> ranges;
    TextureMap stagedTextures;auto& gl=detail::gl();
    auto cleanup=[&](){for(const auto& item:stagedTextures)gl.DeleteTextures(1,&item.second);};
    try {
        for(auto id:scene.getIds()) {
            const auto* c=scene.getComponent(id);if(!c||!DiagramSystem::validPolyhedron(c->polyhedron)){cleanup();return false;}
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
                    while(gl.GetError()!=GL_NO_ERROR){}
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
        while(gl.GetError()!=GL_NO_ERROR){}
        gl.BindVertexArray(vao_);gl.BindBuffer(GL_ARRAY_BUFFER,vbo_);
        gl.BufferData(GL_ARRAY_BUFFER,GLsizeiptr(vertices.size()*sizeof(detail::GpuVertex)),vertices.empty()?nullptr:vertices.data(),GL_STATIC_DRAW);
        gl.BindBuffer(GL_ELEMENT_ARRAY_BUFFER,ebo_);
        gl.BufferData(GL_ELEMENT_ARRAY_BUFFER,GLsizeiptr(indices.size()*sizeof(GLuint)),indices.empty()?nullptr:indices.data(),GL_STATIC_DRAW);
        gl.BindVertexArray(0);gl.BindBuffer(GL_ARRAY_BUFFER,0);
        if(gl.GetError()!=GL_NO_ERROR){cleanup();return false;}
        for(const auto& item:textures_)gl.DeleteTextures(1,&item.second);
        textures_=std::move(stagedTextures);drawRanges_=std::move(ranges);return true;
    } catch(...) {cleanup();throw;}
}
inline bool Renderer::draw(const Scene& scene,const glm::mat4& view,const glm::mat4& projection,
                           const RenderOptions& options) const {
    if(!detail::finiteMatrix(view)||!detail::finiteMatrix(projection))return false;
    for(const auto& range:drawRanges_) {
        glm::mat4 matrix(1);
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner) {const auto world=scene.worldMatrix(*line.owner);if(!world)return false;matrix=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;matrix=*world;
        }
        if(!detail::finiteMatrix(matrix)||!detail::finiteMatrix(projection*view*matrix))return false;
    }
    auto& gl=detail::gl();gl.UseProgram(shaderProgramID_);gl.BindVertexArray(vao_);
    gl.ActiveTexture(GL_TEXTURE0);gl.Uniform1i(textureLocation_,0);
    gl.Uniform3f(lightDirectionLocation_,options.lightDirection.x,options.lightDirection.y,options.lightDirection.z);
    gl.Uniform3f(lightColorLocation_,options.lightColor.red,options.lightColor.green,options.lightColor.blue);
    gl.Uniform3f(outlineColorLocation_,options.outlineColor.red,options.outlineColor.green,options.outlineColor.blue);
    for(const auto& range:drawRanges_) {
        glm::mat4 model(1);const Mesh* face=nullptr;
        if(range.lineId) {
            const auto& line=scene.getLines()[*range.lineId];
            if(line.owner){const auto* owner=scene.getComponent(*line.owner);if(!owner||!owner->visible)continue;const auto world=scene.worldMatrix(*line.owner);if(!world)return false;model=*world;}
        } else {
            const auto* c=scene.getComponent(range.entityId);if(!c||!c->visible)continue;
            face=&c->polyhedron.faces[range.faceIndex];if(!face->visible)continue;
            const auto world=scene.faceWorldMatrix(range.entityId,range.faceIndex);if(!world)return false;model=*world;
        }
        const auto mvp=projection*view*model;
        const float determinant=glm::determinant(glm::mat3(model));
        const auto normalMatrix=std::isfinite(determinant)&&std::abs(determinant)>1e-12f
            ?glm::transpose(glm::inverse(glm::mat3(model))):glm::mat3(1);
        gl.UniformMatrix4fv(mvpLocation_,1,GL_FALSE,glm::value_ptr(mvp));
        gl.UniformMatrix4fv(modelLocation_,1,GL_FALSE,glm::value_ptr(model));
        gl.UniformMatrix3fv(normalLocation_,1,GL_FALSE,glm::value_ptr(normalMatrix));
        gl.Uniform1i(lightingLocation_,face&&options.lighting);gl.Uniform1i(overrideLocation_,0);
        const bool textured=face&&options.textures&&face->material.useTexture;
        gl.Uniform1i(hasTextureLocation_,textured);
        if(textured){gl.BindTexture(GL_TEXTURE_2D,textures_.at(face->material.texture));
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);
            gl.TexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,face->material.repeatTexture?GL_REPEAT:GL_CLAMP_TO_EDGE);}
        else gl.BindTexture(GL_TEXTURE_2D,0);
        if(!face||!options.wireframe)
            gl.DrawElements(face?GL_TRIANGLES:GL_LINES,range.count,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.firstIndex*sizeof(GLuint)));
        if(face&&(options.wireframe||face->outline!=OutlineMode::None)) {
            gl.Uniform1i(hasTextureLocation_,0);gl.Uniform1i(lightingLocation_,0);gl.Uniform1i(overrideLocation_,1);
            gl.DepthFunc(GL_LEQUAL);gl.DepthMask(GL_FALSE);
            gl.DrawElements(GL_LINES,range.outlineCount,GL_UNSIGNED_INT,
                reinterpret_cast<const void*>(range.outlineFirstIndex*sizeof(GLuint)));
            gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);
        }
    }
    gl.Uniform1i(overrideLocation_,0);gl.BindTexture(GL_TEXTURE_2D,0);gl.BindVertexArray(0);gl.UseProgram(0);return gl.GetError()==GL_NO_ERROR;
}
inline bool Renderer::render(const Scene& scene,const Camera& camera,int width,int height,const RenderOptions& options) {
    if(!shaderProgramID_||!vao_)return false;if(width<=0||height<=0)return true;
    if(!CameraSystem::validCamera(camera)||!detail::finiteVector(options.lightDirection)
        ||glm::length(options.lightDirection)<1e-12f||!detail::validColor(options.lightColor)
        ||!detail::validColor(options.outlineColor)
        ||!std::isfinite(options.background.r)||!std::isfinite(options.background.g)
        ||!std::isfinite(options.background.b)||!std::isfinite(options.background.a))return false;
    if(uploadedScene_!=&scene||uploadedRevision_!=scene.getGeometryRevision()||uploadedTextures_!=options.textures) {
        if(!upload(scene,options.textures))return false;
        uploadedScene_=&scene;uploadedRevision_=scene.getGeometryRevision();uploadedTextures_=options.textures;
    }
    auto& gl=detail::gl();while(gl.GetError()!=GL_NO_ERROR){}
    gl.Viewport(0,0,width,height);gl.LineWidth(1);
    if(options.depthTest)gl.Enable(GL_DEPTH_TEST);else gl.Disable(GL_DEPTH_TEST);
    gl.Disable(GL_CULL_FACE);gl.Disable(GL_BLEND);gl.Disable(GL_SCISSOR_TEST);
    gl.PolygonMode(GL_FRONT_AND_BACK,GL_FILL);gl.DepthFunc(GL_LESS);gl.DepthMask(GL_TRUE);
    gl.ClearColor(options.background.r,options.background.g,options.background.b,options.background.a);
    gl.Clear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    return draw(scene,CameraSystem::viewMatrix(camera),CameraSystem::projectionMatrix(camera,float(width)/height),options);
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

## framework/detail/Scene.inl

### before

```cpp
#pragma once
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace graphics {
namespace detail {

inline bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f
        && value.y > 0.0f && value.z > 0.0f;
}

inline std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;
    return ++revision;
}

inline void appendFace(Polyhedron& polyhedron, GLFace shape,
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

} // namespace detail
using detail::finiteVector;
using detail::finiteMatrix;
using detail::validColor;
using detail::positiveLengths;
using detail::nextGeometryRevision;
using detail::appendFace;

inline Scene::Scene() {
    geometryRevision_ = nextGeometryRevision();
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
    pyramidFaceId_.reserve(MAX_ENTITIES * 5);
    lines_.reserve(MAX_LINES);
}

inline std::optional<EntityId> Scene::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (!DiagramSystem::validPolyhedron(polyhedron)
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
    if (!finiteVector(maximum - minimum)) return {};
    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform, maximum - minimum });
    polyId_.push_back(id);
    std::vector<FaceId>* faceIds = nullptr;
    if (polyhedron.shape == GLShape::Cube) faceIds = &cubeFaceId_;
    if (polyhedron.shape == GLShape::Tetra) faceIds = &tetraFaceId_;
    if (polyhedron.shape == GLShape::Pyramid) faceIds = &pyramidFaceId_;
    if (faceIds) for (std::size_t i = 0; i < polyhedron.faces.size(); ++i)
        faceIds->push_back({id, i});
    geometryRevision_ = nextGeometryRevision();
    return id;
}

inline std::optional<EntityId> Scene::addCuboid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths),
                       transform);
}

inline std::optional<EntityId> Scene::addRightTetrahedron(
    const glm::vec3& edgeLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths),
                       transform);
}

inline std::optional<EntityId> Scene::addPyramid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths),
                       transform);
}

inline bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;
    return true;
}

inline bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex,
                               const glm::mat4& matrix) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !finiteMatrix(matrix)) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    return true;
}

inline bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex,
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

inline const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

inline bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return false;
    }
    auto candidate = components_[id].transform;
    candidate.basis = worldPosition;
    return setTransform(id, candidate);
}

inline bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if(id>=components_.size()||!positiveLengths(sideLengths))return false;
    const auto& c=components_[id];
    auto replacement=c.polyhedron;
    const auto box=detail::bounds(replacement);const auto oldSize=box.second-box.first;
    if(!positiveLengths(oldSize))return false;
    const auto ratio=sideLengths/oldSize;
    if(!finiteVector(ratio))return false;
    // Preserve topology/materials. Resize mesh coordinates around the model pivot.
    for(auto& face:replacement.faces) {
        for(auto& vertex:face.localVertices)vertex=c.transform.localBasis+(vertex-c.transform.localBasis)*ratio;
        face.transform.localBasis=c.transform.localBasis+(face.transform.localBasis-c.transform.localBasis)*ratio;
        recalculateNormals(face);
    }
    return setModel(id,replacement);
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true;
}

inline bool Scene::setFaceTransform(
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

inline std::optional<LineId> Scene::addLine(
    const glm::vec3& start, const glm::vec3& end,
    GlColor color, std::optional<EntityId> owner
) {
    if (!finiteVector(start) || !finiteVector(end)
        || !validColor(color) || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    const LineId id = lines_.size();
    lines_.push_back({ start, end, color, owner });
    geometryRevision_ = nextGeometryRevision();
    return id;
}

inline std::optional<LineId> Scene::addVector(
    const glm::vec3& origin, const glm::vec3& vector,
    GlColor color, std::optional<EntityId> owner
) {
    return addLine(origin, origin + vector, color, owner);
}

inline std::optional<std::array<LineId, 3>> Scene::addBasis(
    const glm::vec3& origin, const glm::mat3& basisVectors,
    const glm::vec3& axisLengths, std::optional<EntityId> owner
) {
    if (!finiteVector(origin)
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

inline bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin)
        || !finiteVector(origin + vector)) {
        return false;
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

inline void Scene::clear() {
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

inline bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale)
        && std::isfinite(transform.orientation.w)
        && std::isfinite(transform.orientation.x)
        && std::isfinite(transform.orientation.y)
        && std::isfinite(transform.orientation.z)
        && std::isfinite(glm::dot(transform.orientation, transform.orientation))
        && glm::dot(transform.orientation, transform.orientation) > 0.000001f
        && finiteMatrix(detail::transformMatrix(transform));
}

inline bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    if (polyhedron.faces.empty()) return false;
    for (const auto& face : polyhedron.faces)
        if (!detail::validMesh(face) || !validTransform(face.transform)) return false;
    return true;
}

inline Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
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

inline glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    return detail::transformMatrix(transform);
}

inline glm::mat4 DiagramSystem::faceModelMatrix(const Component_Poly& component,
                                         const PlaneFigure& face) {
    return detail::faceMatrix(component, face);
}

inline bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);
    return finiteVector(camera.position) && finiteVector(camera.target)
        && finiteVector(camera.up) && finiteVector(direction) && finiteVector(right)
        && std::isfinite(glm::dot(direction, direction))
        && std::isfinite(glm::dot(right, right))
        && glm::dot(direction, direction) > 0.000001f
        && glm::dot(right, right) > 0.000001f
        && std::isfinite(camera.fieldOfViewDegrees)
        && camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f
        && std::isfinite(camera.nearPlane) && std::isfinite(camera.farPlane)
        && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane
        && std::isfinite(camera.orthographicHalfExtent)
        && camera.orthographicHalfExtent > 0.0f;
}

inline glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

inline glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                          camera.nearPlane, camera.farPlane);
    }
    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect,
                            camera.nearPlane, camera.farPlane);
}


inline void Scene::rebuildFaceIds() {
    cubeFaceId_.clear(); tetraFaceId_.clear(); pyramidFaceId_.clear();
    for (EntityId id : polyId_) {
        const auto& model = components_[id].polyhedron;
        auto* ids = model.shape == GLShape::Cube ? &cubeFaceId_
            : model.shape == GLShape::Tetra ? &tetraFaceId_
            : model.shape == GLShape::Pyramid ? &pyramidFaceId_ : nullptr;
        if(ids) for(std::size_t f=0; f<model.faces.size(); ++f) ids->push_back({id,f});
    }
}
inline std::optional<EntityId> Scene::addModel(const Model& model, const Transform3D& transform,
                                             PivotMode pivot, const glm::vec3& customPivot) {
    if(!DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot)) return {};
    auto initial=transform;
    if(pivot==PivotMode::BottomCenter) initial.localBasis=detail::bottomCenter(model);
    else if(pivot==PivotMode::Custom) initial.localBasis=customPivot;
    auto id=push_entity(model,initial);
    if(id) components_[*id].pivotMode=pivot;
    return id;
}
inline bool Scene::setModel(EntityId id, const Model& model) {
    if(id>=components_.size() || !DiagramSystem::validPolyhedron(model)) return false;
    const auto box=detail::bounds(model);
    if(!detail::finiteVector(box.second-box.first)) return false;
    auto replacement=components_[id];
    replacement.polyhedron=model;replacement.sideLengths=box.second-box.first;
    if(replacement.pivotMode==PivotMode::BottomCenter)
        replacement.transform.localBasis=detail::bottomCenter(model);
    components_[id]=std::move(replacement);
    rebuildFaceIds();geometryRevision_=nextGeometryRevision();return true;
}
inline const Mesh* Scene::getMesh(EntityId id, std::size_t meshIndex) const {
    const auto* c=getComponent(id);
    return c && meshIndex<c->polyhedron.faces.size()?&c->polyhedron.faces[meshIndex]:nullptr;
}
inline bool Scene::setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh) {
    if(!getMesh(id,meshIndex)) return false;
    Model replacement=components_[id].polyhedron;
    replacement.faces[meshIndex]=mesh;
    return setModel(id,replacement);
}
inline bool Scene::setMeshVertices(EntityId id,std::size_t meshIndex,
                                   const std::vector<Vertex>& vertices,
                                   const std::vector<std::uint32_t>& triangles) {
    const auto* original=getMesh(id,meshIndex);if(!original)return false;
    auto replacement=*original;
    const auto geometry=Mesh::fromVertices(vertices,triangles);
    replacement.localVertices=geometry.localVertices;replacement.vertexNormals=geometry.vertexNormals;
    replacement.texcoords=geometry.texcoords;replacement.vertexColors=geometry.vertexColors;
    replacement.triangleIndices=triangles;replacement.shape=GLFace::Mesh;
    replacement.outlineIndices.clear();
    if(replacement.outline==OutlineMode::Explicit) replacement.outline=OutlineMode::None;
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setVertexPosition(EntityId id,std::size_t meshIndex,std::size_t vertexIndex,
                                     const glm::vec3& position) {
    const auto* mesh=getMesh(id,meshIndex);
    if(!mesh||vertexIndex>=mesh->localVertices.size()||!detail::finiteVector(position))return false;
    auto replacement=*mesh;replacement.localVertices[vertexIndex]=position;
    recalculateNormals(replacement);
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setTriangleIndices(EntityId id,std::size_t meshIndex,
                                      const std::vector<std::uint32_t>& indices) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.shape=GLFace::Mesh;replacement.triangleIndices=indices;
    if(!detail::validMesh(replacement))return false;
    recalculateNormals(replacement);return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setTexcoords(EntityId id,std::size_t meshIndex,const std::vector<glm::vec2>& uvs) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.texcoords=uvs;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setVertexNormals(EntityId id,std::size_t meshIndex,const std::vector<glm::vec3>& normals) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.vertexNormals=normals;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setMaterial(EntityId id,std::size_t meshIndex,const Material& material) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.material=material;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setOutline(EntityId id,std::size_t meshIndex,OutlineMode mode,
                              const std::vector<std::uint32_t>& explicitEdges) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.outline=mode;replacement.outlineIndices=explicitEdges;
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setLocalBasis(EntityId id,PivotMode mode,const glm::vec3& customPivot) {
    if(id>=components_.size()||!detail::finiteVector(customPivot))return false;
    auto& c=components_[id];
    if(mode==PivotMode::BottomCenter)c.transform.localBasis=detail::bottomCenter(c.polyhedron);
    else if(mode==PivotMode::Custom)c.transform.localBasis=customPivot;
    c.pivotMode=mode;return true;
}
inline bool Scene::setVisible(EntityId id,bool visible) {
    if(id>=components_.size())return false;
    components_[id].visible=visible;return true;
}
inline bool Scene::splitFace(EntityId id,std::size_t faceIndex) {
    if(!getMesh(id,faceIndex))return false;
    // The same face remains under its parent; vertices and indices are already face-local storage.
    auto& face=components_[id].polyhedron.faces[faceIndex];
    if(!face.independence) {face.transform=Transform3D{};face.independence=true;}
    return true;
}
inline bool Scene::setFaceSRT(EntityId id,std::size_t faceIndex,const Transform3D& transform) {
    return setFaceTransform(id,faceIndex,transform,true);
}
} // namespace graphics

```

### after

```cpp
#pragma once
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <sstream>
#include <utility>

namespace graphics {
namespace detail {

inline bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f
        && value.y > 0.0f && value.z > 0.0f;
}

inline std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;
    return ++revision;
}

inline void appendFace(Polyhedron& polyhedron, GLFace shape,
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

} // namespace detail
using detail::finiteVector;
using detail::finiteMatrix;
using detail::validColor;
using detail::positiveLengths;
using detail::nextGeometryRevision;
using detail::appendFace;

inline Scene::Scene() {
    geometryRevision_ = nextGeometryRevision();
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    tetraFaceId_.reserve(MAX_ENTITIES * 4);
    cubeFaceId_.reserve(MAX_ENTITIES * 6);
    pyramidFaceId_.reserve(MAX_ENTITIES * 5);
    lines_.reserve(MAX_LINES);
}

inline std::optional<EntityId> Scene::push_entity(
    const Polyhedron& polyhedron, const Transform3D& transform
) {
    if (!DiagramSystem::validPolyhedron(polyhedron)
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
    if (!finiteVector(maximum - minimum)) return {};
    const EntityId id = components_.size();
    components_.push_back({ polyhedron, transform, maximum - minimum });
    polyId_.push_back(id);
    std::vector<FaceId>* faceIds = nullptr;
    if (polyhedron.shape == GLShape::Cube) faceIds = &cubeFaceId_;
    if (polyhedron.shape == GLShape::Tetra) faceIds = &tetraFaceId_;
    if (polyhedron.shape == GLShape::Pyramid) faceIds = &pyramidFaceId_;
    if (faceIds) for (std::size_t i = 0; i < polyhedron.faces.size(); ++i)
        faceIds->push_back({id, i});
    geometryRevision_ = nextGeometryRevision();
    return id;
}

inline std::optional<EntityId> Scene::addCuboid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths),
                       transform);
}

inline std::optional<EntityId> Scene::addRightTetrahedron(
    const glm::vec3& edgeLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Tetra, edgeLengths),
                       transform);
}

inline std::optional<EntityId> Scene::addPyramid(
    const glm::vec3& sideLengths, const glm::vec3& worldPosition
) {
    Transform3D transform;
    transform.basis = worldPosition;
    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths),
                       transform);
}

inline bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;
    return true;
}

inline bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex,
                               const glm::mat4& matrix) {
    if (id >= components_.size()
        || faceIndex >= components_[id].polyhedron.faces.size()
        || !finiteMatrix(matrix)) {
        return false;
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    return true;
}

inline bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex,
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

inline const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

inline bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    // For a parented entity this position is in parent coordinates.
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return false;
    }
    auto candidate = components_[id].transform;
    candidate.basis = worldPosition;
    return setTransform(id, candidate);
}

inline bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if(id>=components_.size()||!positiveLengths(sideLengths))return false;
    const auto& c=components_[id];
    auto replacement=c.polyhedron;
    const auto box=detail::bounds(replacement);const auto oldSize=box.second-box.first;
    if(!positiveLengths(oldSize))return false;
    const auto ratio=sideLengths/oldSize;
    if(!finiteVector(ratio))return false;
    // Preserve topology/materials. Resize mesh coordinates around the model pivot.
    for(auto& face:replacement.faces) {
        for(auto& vertex:face.localVertices)vertex=c.transform.localBasis+(vertex-c.transform.localBasis)*ratio;
        face.transform.localBasis=c.transform.localBasis+(face.transform.localBasis-c.transform.localBasis)*ratio;
        recalculateNormals(face);
    }
    return setModel(id,replacement);
}

inline bool Scene::setParent(EntityId id, std::optional<EntityId> parent) {
    if (id >= components_.size()) return false;
    auto ancestor = parent;
    std::size_t remaining = components_.size();
    while (ancestor) {
        if (*ancestor >= components_.size() || *ancestor == id || remaining-- == 0) return false;
        ancestor = components_[*ancestor].parent;
    }
    components_[id].parent = parent;
    return true;
}
inline std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    glm::mat4 result(1);
    std::optional<EntityId> current = id;
    std::size_t remaining = components_.size();
    while (current) {
        if (*current >= components_.size() || remaining-- == 0) return {};
        const auto& component = components_[*current];
        result = detail::transformMatrix(component.transform) * result;
        if (!detail::finiteMatrix(result)) return {};
        current = component.parent;
    }
    return result;
}
inline std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) return {};
    const auto& component = components_[id];
    auto result = DiagramSystem::faceModelMatrix(component, component.polyhedron.faces[faceIndex]);
    if (component.parent) {
        const auto parent = worldMatrix(*component.parent);
        if (!parent) return {};
        result = *parent * result;
    }
    return detail::finiteMatrix(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

inline bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return false;
    }
    components_[id].transform = transform;
    return true;
}

inline bool Scene::setFaceTransform(
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

inline std::optional<LineId> Scene::addLine(
    const glm::vec3& start, const glm::vec3& end,
    GlColor color, std::optional<EntityId> owner
) {
    if (!finiteVector(start) || !finiteVector(end)
        || !validColor(color) || (owner && getComponent(*owner) == nullptr)) {
        return std::nullopt;
    }
    const LineId id = lines_.size();
    lines_.push_back({ start, end, color, owner });
    geometryRevision_ = nextGeometryRevision();
    return id;
}

inline std::optional<LineId> Scene::addVector(
    const glm::vec3& origin, const glm::vec3& vector,
    GlColor color, std::optional<EntityId> owner
) {
    return addLine(origin, origin + vector, color, owner);
}

inline std::optional<std::array<LineId, 3>> Scene::addBasis(
    const glm::vec3& origin, const glm::mat3& basisVectors,
    const glm::vec3& axisLengths, std::optional<EntityId> owner
) {
    if (!finiteVector(origin)
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

inline bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin)
        || !finiteVector(origin + vector)) {
        return false;
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();
    return true;
}

inline void Scene::clear() {
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

inline bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis)
        && finiteVector(transform.rotationDegrees) && finiteVector(transform.scale)
        && std::isfinite(transform.orientation.w)
        && std::isfinite(transform.orientation.x)
        && std::isfinite(transform.orientation.y)
        && std::isfinite(transform.orientation.z)
        && std::isfinite(glm::dot(transform.orientation, transform.orientation))
        && glm::dot(transform.orientation, transform.orientation) > 0.000001f
        && finiteMatrix(detail::transformMatrix(transform));
}

inline bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    if (polyhedron.faces.empty()) return false;
    for (const auto& face : polyhedron.faces)
        if (!detail::validMesh(face) || !validTransform(face.transform)) return false;
    return true;
}

inline Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
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

inline glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    return detail::transformMatrix(transform);
}

inline glm::mat4 DiagramSystem::faceModelMatrix(const Component_Poly& component,
                                         const PlaneFigure& face) {
    return detail::faceMatrix(component, face);
}

inline bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);
    return finiteVector(camera.position) && finiteVector(camera.target)
        && finiteVector(camera.up) && finiteVector(direction) && finiteVector(right)
        && std::isfinite(glm::dot(direction, direction))
        && std::isfinite(glm::dot(right, right))
        && glm::dot(direction, direction) > 0.000001f
        && glm::dot(right, right) > 0.000001f
        && std::isfinite(camera.fieldOfViewDegrees)
        && camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f
        && std::isfinite(camera.nearPlane) && std::isfinite(camera.farPlane)
        && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane
        && std::isfinite(camera.orthographicHalfExtent)
        && camera.orthographicHalfExtent > 0.0f;
}

inline glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

inline glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);
        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                          camera.nearPlane, camera.farPlane);
    }
    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect,
                            camera.nearPlane, camera.farPlane);
}


inline void Scene::rebuildFaceIds() {
    cubeFaceId_.clear(); tetraFaceId_.clear(); pyramidFaceId_.clear();
    for (EntityId id : polyId_) {
        const auto& model = components_[id].polyhedron;
        auto* ids = model.shape == GLShape::Cube ? &cubeFaceId_
            : model.shape == GLShape::Tetra ? &tetraFaceId_
            : model.shape == GLShape::Pyramid ? &pyramidFaceId_ : nullptr;
        if(ids) for(std::size_t f=0; f<model.faces.size(); ++f) ids->push_back({id,f});
    }
}
inline std::optional<EntityId> Scene::addModel(const Model& model, const Transform3D& transform,
                                             PivotMode pivot, const glm::vec3& customPivot) {
    if(!DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot)) return {};
    auto initial=transform;
    if(pivot==PivotMode::BottomCenter) initial.localBasis=detail::bottomCenter(model);
    else if(pivot==PivotMode::Custom) initial.localBasis=customPivot;
    auto id=push_entity(model,initial);
    if(id) components_[*id].pivotMode=pivot;
    return id;
}
inline bool Scene::setModel(EntityId id, const Model& model) {
    if(id>=components_.size() || !DiagramSystem::validPolyhedron(model)) return false;
    const auto box=detail::bounds(model);
    if(!detail::finiteVector(box.second-box.first)) return false;
    auto replacement=components_[id];
    replacement.polyhedron=model;replacement.sideLengths=box.second-box.first;
    if(replacement.pivotMode==PivotMode::BottomCenter)
        replacement.transform.localBasis=detail::bottomCenter(model);
    components_[id]=std::move(replacement);
    rebuildFaceIds();geometryRevision_=nextGeometryRevision();return true;
}
inline const Mesh* Scene::getMesh(EntityId id, std::size_t meshIndex) const {
    const auto* c=getComponent(id);
    return c && meshIndex<c->polyhedron.faces.size()?&c->polyhedron.faces[meshIndex]:nullptr;
}
inline bool Scene::setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh) {
    if(!getMesh(id,meshIndex)) return false;
    Model replacement=components_[id].polyhedron;
    replacement.faces[meshIndex]=mesh;
    return setModel(id,replacement);
}
inline bool Scene::setMeshVertices(EntityId id,std::size_t meshIndex,
                                   const std::vector<Vertex>& vertices,
                                   const std::vector<std::uint32_t>& triangles) {
    const auto* original=getMesh(id,meshIndex);if(!original)return false;
    auto replacement=*original;
    const auto geometry=Mesh::fromVertices(vertices,triangles);
    replacement.localVertices=geometry.localVertices;replacement.vertexNormals=geometry.vertexNormals;
    replacement.texcoords=geometry.texcoords;replacement.vertexColors=geometry.vertexColors;
    replacement.triangleIndices=triangles;replacement.shape=GLFace::Mesh;
    replacement.outlineIndices.clear();
    if(replacement.outline==OutlineMode::Explicit) replacement.outline=OutlineMode::None;
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setVertexPosition(EntityId id,std::size_t meshIndex,std::size_t vertexIndex,
                                     const glm::vec3& position) {
    const auto* mesh=getMesh(id,meshIndex);
    if(!mesh||vertexIndex>=mesh->localVertices.size()||!detail::finiteVector(position))return false;
    auto replacement=*mesh;replacement.localVertices[vertexIndex]=position;
    recalculateNormals(replacement);
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setTriangleIndices(EntityId id,std::size_t meshIndex,
                                      const std::vector<std::uint32_t>& indices) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.shape=GLFace::Mesh;replacement.triangleIndices=indices;
    if(!detail::validMesh(replacement))return false;
    recalculateNormals(replacement);return setMesh(id,meshIndex,replacement);
}
inline bool Scene::generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options) {
    const auto* mesh=getMesh(id,meshIndex); if(!mesh)return false;
    auto replacement=*mesh;
    return graphics::generateTexcoords(replacement,options) && setMesh(id,meshIndex,replacement);
}
inline bool Scene::transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale,
                                     const glm::vec2& offset, float rotationDegrees) {
    const auto* mesh=getMesh(id,meshIndex); if(!mesh)return false;
    auto replacement=*mesh;
    return graphics::transformTexcoords(replacement,scale,offset,rotationDegrees) && setMesh(id,meshIndex,replacement);
}
inline bool Scene::setTexcoords(EntityId id,std::size_t meshIndex,const std::vector<glm::vec2>& uvs) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.texcoords=uvs;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setVertexNormals(EntityId id,std::size_t meshIndex,const std::vector<glm::vec3>& normals) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.vertexNormals=normals;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setMaterial(EntityId id,std::size_t meshIndex,const Material& material) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.material=material;return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setOutline(EntityId id,std::size_t meshIndex,OutlineMode mode,
                              const std::vector<std::uint32_t>& explicitEdges) {
    const auto* mesh=getMesh(id,meshIndex);if(!mesh)return false;
    auto replacement=*mesh;replacement.outline=mode;replacement.outlineIndices=explicitEdges;
    return setMesh(id,meshIndex,replacement);
}
inline bool Scene::setLocalBasis(EntityId id,PivotMode mode,const glm::vec3& customPivot) {
    if(id>=components_.size()||!detail::finiteVector(customPivot))return false;
    auto& c=components_[id];
    if(mode==PivotMode::BottomCenter)c.transform.localBasis=detail::bottomCenter(c.polyhedron);
    else if(mode==PivotMode::Custom)c.transform.localBasis=customPivot;
    c.pivotMode=mode;return true;
}
inline bool Scene::setVisible(EntityId id,bool visible) {
    if(id>=components_.size())return false;
    components_[id].visible=visible;return true;
}
inline bool Scene::splitFace(EntityId id,std::size_t faceIndex) {
    if(!getMesh(id,faceIndex))return false;
    // The same face remains under its parent; vertices and indices are already face-local storage.
    auto& face=components_[id].polyhedron.faces[faceIndex];
    if(!face.independence) {face.transform=Transform3D{};face.independence=true;}
    return true;
}
inline bool Scene::setFaceSRT(EntityId id,std::size_t faceIndex,const Transform3D& transform) {
    return setFaceTransform(id,faceIndex,transform,true);
}
} // namespace graphics

```

## framework/examples/hierarchy.cpp

### before

없음

### after

```cpp
#include "../Framework.h"
#include <array>
#include <iostream>

int main() {
    graphics::Application app;
    std::array<graphics::EntityId,3> bodies{};
    app.setInitHandler([&](graphics::Application& app) {
        auto& scene=app.getScene();
        const auto sun=scene.addCuboid({.8f,.8f,.8f},{0,0,0});
        const auto earth=scene.addCuboid({.4f,.4f,.4f},{2,0,0});
        const auto moon=scene.addCuboid({.2f,.2f,.2f},{.8f,0,0});
        if(!sun||!earth||!moon)return false;
        bodies={*sun,*earth,*moon};
        if(!scene.setParent(*earth,*sun)||!scene.setParent(*moon,*earth))return false;
        auto image=std::make_shared<graphics::TextureImage>();
        image->width=2;image->height=2;
        image->rgba={255,255,255,255, 20,20,20,255,
                     20,20,20,255, 255,255,255,255};
        graphics::Material material;material.texture=image;material.useTexture=true;
        graphics::UVOptions uv;uv.mapping=graphics::UVMapping::Box;uv.scale={2,2};
        for(auto id:bodies) {
            const auto count=scene.getComponent(id)->polyhedron.faces.size();
            for(std::size_t mesh=0;mesh<count;++mesh) {
                if(!scene.generateTexcoords(id,mesh,uv)||!scene.setMaterial(id,mesh,material))return false;
            }
        }
        app.getCamera().position={0,4,8};
        app.getCamera().target={0,0,0};
        return true;
    });
    app.setUpdateHandler([&](graphics::Application& app,float dt) {
        auto& scene=app.getScene();
        for(std::size_t i=0;i<2;++i) {
            auto transform=scene.getComponent(bodies[i])->transform;
            transform.rotationDegrees.y=std::fmod(transform.rotationDegrees.y+dt*(i?80.0f:20.0f),360.0f);
            if(!scene.setTransform(bodies[i],transform))app.requestClose();
        }
    });
    app.setMouseHandler([](graphics::Application& app,const graphics::MouseEvent& event) {
        if(event.button!=GLFW_MOUSE_BUTTON_LEFT||event.action!=GLFW_PRESS)return;
        if(const auto hit=app.pickAt(event.windowX,event.windowY))
            std::cout<<"entity="<<hit->entityId<<" UV="<<hit->uv.x<<","<<hit->uv.y<<'\n';
    });
    app.setKeyHandler([](graphics::Application& app,int key,int action) {
        if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)app.requestClose();
    });
    graphics::ApplicationConfig config;config.features.textures=true;
    config.title="Hierarchy and UV mapping";
    return app.run(config);
}

```

## framework/include/Geometry.h

### before

```cpp
#pragma once
#include "Model.h"
#include <algorithm>
#include <map>
#include <numeric>
#include <stdexcept>

namespace graphics {
inline void recalculateNormals(Mesh& mesh) {
    mesh.vertexNormals.assign(mesh.localVertices.size(), glm::vec3(0));
    const auto indices = detail::triangles(mesh);
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i+1], c = indices[i+2];
        const auto n = glm::cross(mesh.localVertices[b] - mesh.localVertices[a],
                                  mesh.localVertices[c] - mesh.localVertices[a]);
        mesh.vertexNormals[a] += n; mesh.vertexNormals[b] += n; mesh.vertexNormals[c] += n;
    }
    for (auto& n : mesh.vertexNormals) {
        const float size = glm::length(n);
        n = size > 1e-12f ? n / size : glm::vec3(0);
    }
}

// Weld equal positions for edge topology, while preserving separate UV/normal vertices.
inline std::vector<std::uint32_t> meshOutline(const Mesh& mesh,
                                             OutlineMode mode = OutlineMode::FeatureEdges) {
    if (mode == OutlineMode::None) return {};
    if (mode == OutlineMode::Explicit) return mesh.outlineIndices;
    using Position = std::array<float, 3>;
    std::map<Position, std::uint32_t> points;
    std::vector<std::uint32_t> welded;
    for (std::uint32_t i = 0; i < mesh.localVertices.size(); ++i) {
        const auto& p = mesh.localVertices[i];
        const auto result = points.emplace(Position{p.x, p.y, p.z}, i);
        welded.push_back(result.first->second);
    }
    struct Edge { std::uint32_t a = 0, b = 0; std::vector<glm::vec3> normals; };
    std::map<std::pair<std::uint32_t, std::uint32_t>, Edge> edges;
    const auto indices = detail::triangles(mesh);
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i+1], c = indices[i+2];
        auto normal = glm::cross(mesh.localVertices[b] - mesh.localVertices[a],
                                 mesh.localVertices[c] - mesh.localVertices[a]);
        const auto length = glm::length(normal);
        if (length <= 1e-12f) continue;
        normal /= length;
        const std::array<std::uint32_t, 3> corners{a,b,c};
        for (int e = 0; e < 3; ++e) {
            auto first = welded[corners[e]], second = welded[corners[(e+1)%3]];
            if (first == second) continue;
            if (first > second) std::swap(first, second);
            auto& edge = edges[{first, second}];
            edge.a = first; edge.b = second; edge.normals.push_back(normal);
        }
    }
    const float threshold = std::cos(glm::radians(mesh.creaseAngleDegrees));
    std::vector<std::uint32_t> result;
    for (const auto& item : edges) {
        const auto& edge = item.second;
        const bool show = mode == OutlineMode::AllTriangleEdges || edge.normals.size() != 2
            || glm::dot(edge.normals[0], edge.normals[1]) < threshold;
        if (show) { result.push_back(edge.a); result.push_back(edge.b); }
    }
    return result;
}

namespace detail {
// Ear clipping handles planar simple concave polygons, with either winding.
inline std::vector<std::uint32_t> triangulate(const std::vector<glm::vec3>& points) {
    if (points.size() < 3) throw std::runtime_error("Face has fewer than three corners.");
    glm::vec3 normal(0);
    for (std::size_t i=0; i<points.size(); ++i) {
        const auto& a=points[i]; const auto& b=points[(i+1)%points.size()];
        normal += glm::vec3((a.y-b.y)*(a.z+b.z), (a.z-b.z)*(a.x+b.x), (a.x-b.x)*(a.y+b.y));
    }
    if (glm::length(normal) < 1e-12f) throw std::runtime_error("Degenerate polygon.");
    const auto absNormal=glm::abs(normal);
    const int axis=absNormal.x>absNormal.y ? (absNormal.x>absNormal.z?0:2) : (absNormal.y>absNormal.z?1:2);
    std::vector<glm::dvec2> projected;
    for (const auto& p : points) projected.push_back(axis==0 ? glm::dvec2(p.y,p.z)
        : axis==1 ? glm::dvec2(p.x,p.z) : glm::dvec2(p.x,p.y));
    double area=0;
    for(std::size_t i=0;i<projected.size();++i) {
        const auto& a=projected[i]; const auto& b=projected[(i+1)%projected.size()];
        area+=a.x*b.y-b.x*a.y;
    }
    if (std::abs(area)<1e-14) throw std::runtime_error("Degenerate polygon projection.");
    const double direction=area>0?1:-1;
    auto cross=[](glm::dvec2 a,glm::dvec2 b,glm::dvec2 c) {return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);};
    std::vector<std::uint32_t> polygon(points.size()), result;
    std::iota(polygon.begin(),polygon.end(),0);
    while(polygon.size()>3) {
        bool clipped=false;
        for(std::size_t i=0;i<polygon.size();++i) {
            const auto a=polygon[(i+polygon.size()-1)%polygon.size()],b=polygon[i],c=polygon[(i+1)%polygon.size()];
            if(direction*cross(projected[a],projected[b],projected[c])<=1e-14) continue;
            bool inside=false;
            for(auto p:polygon) if(p!=a&&p!=b&&p!=c
                && direction*cross(projected[a],projected[b],projected[p])>=-1e-14
                && direction*cross(projected[b],projected[c],projected[p])>=-1e-14
                && direction*cross(projected[c],projected[a],projected[p])>=-1e-14) {inside=true;break;}
            if(inside) continue;
            result.insert(result.end(),{a,b,c}); polygon.erase(polygon.begin()+i); clipped=true; break;
        }
        if(!clipped) throw std::runtime_error("Cannot triangulate polygon; check repeated corners or self-intersections.");
    }
    result.insert(result.end(),polygon.begin(),polygon.end());
    return result;
}
inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first=std::uint32_t(destination.localVertices.size());
    destination.localVertices.insert(destination.localVertices.end(),source.localVertices.begin(),source.localVertices.end());
    destination.vertexNormals.insert(destination.vertexNormals.end(),source.vertexNormals.begin(),source.vertexNormals.end());
    destination.texcoords.insert(destination.texcoords.end(),source.texcoords.begin(),source.texcoords.end());
    for(auto i:triangles(source)) destination.triangleIndices.push_back(first+i);
    destination.shape=GLFace::Mesh;
}
inline bool shareEdge(const Mesh& a,const Mesh& b) {
    const auto ae=meshOutline(a),be=meshOutline(b);
    for(std::size_t i=0;i<ae.size();i+=2) for(std::size_t j=0;j<be.size();j+=2) {
        const auto p=a.localVertices[ae[i]],q=a.localVertices[ae[i+1]],r=b.localVertices[be[j]],s=b.localVertices[be[j+1]];
        if((p==r&&q==s)||(p==s&&q==r)) return true;
    }
    return false;
}
inline void mergeCoplanar(Model& model) {
    for(std::size_t i=0;i<model.faces.size();++i) {
        auto& face=model.faces[i];
        if(glm::length(face.normal)<1e-12f) continue;
        for(std::size_t j=i+1;j<model.faces.size();) {
            const auto& other=model.faces[j];
            const bool samePlane=glm::dot(face.normal,other.normal)>1-1e-6f
                && std::abs(glm::dot(face.normal,other.localVertices[0]-face.localVertices[0]))<1e-6f;
            if(samePlane && face.texcoords.empty()==other.texcoords.empty() && shareEdge(face,other)) {
                appendMesh(face,other); model.faces.erase(model.faces.begin()+j); j=i+1;
            } else ++j;
        }
    }
}
} // namespace detail
} // namespace graphics

```

### after

```cpp
#pragma once
#include "Model.h"
#include <algorithm>
#include <map>
#include <numeric>
#include <stdexcept>

namespace graphics {
enum class UVMapping { Planar, Box, Cylindrical, Spherical };
struct UVOptions {
    UVMapping mapping = UVMapping::Planar;
    int axis = 2; // Planar projection drops X=0, Y=1, or Z=2.
    glm::vec2 scale{1};
    glm::vec2 offset{0};
    float rotationDegrees = 0;
};

inline bool transformTexcoords(Mesh& mesh, const glm::vec2& scale,
                              const glm::vec2& offset, float rotationDegrees = 0) {
    if (mesh.texcoords.size() != mesh.localVertices.size() || mesh.texcoords.empty()
        || !std::isfinite(scale.x) || !std::isfinite(scale.y)
        || !std::isfinite(offset.x) || !std::isfinite(offset.y)
        || !std::isfinite(rotationDegrees)) return false;
    auto uvs = mesh.texcoords;
    const double angle = glm::radians(double(rotationDegrees));
    const double c = std::cos(angle), s = std::sin(angle);
    for (auto& uv : uvs) {
        const auto p = glm::dvec2(uv) * glm::dvec2(scale);
        uv = glm::vec2(c*p.x-s*p.y, s*p.x+c*p.y) + offset;
        if (!std::isfinite(uv.x) || !std::isfinite(uv.y)) return false;
    }
    mesh.texcoords = std::move(uvs);
    return true;
}

inline bool generateTexcoords(Mesh& mesh, const UVOptions& options = UVOptions{}) {
    if (!detail::validMesh(mesh) || options.axis < 0 || options.axis > 2) return false;
    switch (options.mapping) {
    case UVMapping::Planar: case UVMapping::Box:
    case UVMapping::Cylindrical: case UVMapping::Spherical: break;
    default: return false;
    }
    Mesh result = mesh;
    glm::dvec3 low(mesh.localVertices[0]), high(low);
    for (const auto& p : mesh.localVertices) {
        low = glm::min(low, glm::dvec3(p)); high = glm::max(high, glm::dvec3(p));
    }
    const auto size = high-low, center = (low+high)*0.5;
    const double pi = std::acos(-1.0);
    auto project = [&](std::uint32_t index, int axis) {
        const auto p = glm::dvec3(mesh.localVertices[index]);
        if (options.mapping == UVMapping::Planar || options.mapping == UVMapping::Box) {
            const int u = axis == 0 ? 2 : 0, v = axis == 1 ? 2 : 1;
            return glm::vec2(size[u] > 0 ? (p[u]-low[u])/size[u] : 0.5,
                             size[v] > 0 ? (p[v]-low[v])/size[v] : 0.5);
        }
        const auto d = p-center;
        const double radius = glm::length(d);
        return glm::vec2(std::atan2(d.z,d.x)/(2*pi)+0.5,
            options.mapping == UVMapping::Spherical
                ? (radius > 0 ? std::asin(std::clamp(d.y/radius,-1.0,1.0))/pi+0.5 : 0.5)
                : (size.y > 0 ? (p.y-low.y)/size.y : 0.5));
    };
    if (options.mapping == UVMapping::Planar) {
        result.texcoords.clear();
        for (std::uint32_t i=0;i<mesh.localVertices.size();++i) result.texcoords.push_back(project(i,options.axis));
    } else {
        const auto indices = detail::triangles(mesh);
        if (indices.size() > std::numeric_limits<std::uint32_t>::max()) return false;
        result.localVertices.clear(); result.vertexNormals.clear(); result.vertexColors.clear();
        result.texcoords.clear(); result.triangleIndices.clear(); result.outlineIndices.clear();
        result.shape = GLFace::Mesh;
        std::vector<std::uint32_t> first(mesh.localVertices.size(),std::numeric_limits<std::uint32_t>::max());
        for (std::size_t t=0;t<indices.size();t+=3) {
            const auto a=glm::dvec3(mesh.localVertices[indices[t]]);
            const auto b=glm::dvec3(mesh.localVertices[indices[t+1]]);
            const auto d=glm::dvec3(mesh.localVertices[indices[t+2]]);
            const auto normal=glm::abs(glm::cross(b-a,d-a));
            const int axis=normal.x>normal.y ? (normal.x>normal.z?0:2) : (normal.y>normal.z?1:2);
            std::array<glm::vec2,3> uv{project(indices[t],axis),project(indices[t+1],axis),project(indices[t+2],axis)};
            if (options.mapping != UVMapping::Box) {
                // Pole U is taken from neighboring corners, where longitude is defined.
                for (int k=0;k<3;++k) {
                    const auto p=glm::dvec3(mesh.localVertices[indices[t+k]])-center;
                    if (p.x==0 && p.z==0) {
                        const auto u=uv[(k+1)%3].x, v=uv[(k+2)%3].x;
                        uv[k].x=(u+v+(std::abs(u-v)>0.5f?1.0f:0.0f))*0.5f;
                    }
                }
                const float lo=std::min({uv[0].x,uv[1].x,uv[2].x});
                const float hi=std::max({uv[0].x,uv[1].x,uv[2].x});
                if (hi-lo>0.5f) for (auto& p:uv) if (p.x<0.5f) p.x+=1;
            }
            for (int k=0;k<3;++k) {
                const auto source=indices[t+k], target=std::uint32_t(result.localVertices.size());
                if(first[source]==std::numeric_limits<std::uint32_t>::max()) first[source]=target;
                result.localVertices.push_back(mesh.localVertices[source]);
                if(!mesh.vertexNormals.empty()) result.vertexNormals.push_back(mesh.vertexNormals[source]);
                if(!mesh.vertexColors.empty()) result.vertexColors.push_back(mesh.vertexColors[source]);
                result.texcoords.push_back(uv[k]); result.triangleIndices.push_back(target);
            }
        }
        for(auto index:mesh.outlineIndices) {
            if(first[index]==std::numeric_limits<std::uint32_t>::max()) {
                first[index]=std::uint32_t(result.localVertices.size());
                result.localVertices.push_back(mesh.localVertices[index]);
                if(!mesh.vertexNormals.empty()) result.vertexNormals.push_back(mesh.vertexNormals[index]);
                if(!mesh.vertexColors.empty()) result.vertexColors.push_back(mesh.vertexColors[index]);
                result.texcoords.push_back(project(index,options.axis));
            }
            result.outlineIndices.push_back(first[index]);
        }
    }
    if (!transformTexcoords(result,options.scale,options.offset,options.rotationDegrees)
        || !detail::validMesh(result)) return false;
    mesh=std::move(result); return true;
}

inline void recalculateNormals(Mesh& mesh) {
    mesh.vertexNormals.assign(mesh.localVertices.size(), glm::vec3(0));
    const auto indices = detail::triangles(mesh);
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i+1], c = indices[i+2];
        const auto n = glm::cross(mesh.localVertices[b] - mesh.localVertices[a],
                                  mesh.localVertices[c] - mesh.localVertices[a]);
        mesh.vertexNormals[a] += n; mesh.vertexNormals[b] += n; mesh.vertexNormals[c] += n;
    }
    for (auto& n : mesh.vertexNormals) {
        const float size = glm::length(n);
        n = size > 1e-12f ? n / size : glm::vec3(0);
    }
}

// Weld equal positions for edge topology, while preserving separate UV/normal vertices.
inline std::vector<std::uint32_t> meshOutline(const Mesh& mesh,
                                             OutlineMode mode = OutlineMode::FeatureEdges) {
    if (mode == OutlineMode::None) return {};
    if (mode == OutlineMode::Explicit) return mesh.outlineIndices;
    using Position = std::array<float, 3>;
    std::map<Position, std::uint32_t> points;
    std::vector<std::uint32_t> welded;
    for (std::uint32_t i = 0; i < mesh.localVertices.size(); ++i) {
        const auto& p = mesh.localVertices[i];
        const auto result = points.emplace(Position{p.x, p.y, p.z}, i);
        welded.push_back(result.first->second);
    }
    struct Edge { std::uint32_t a = 0, b = 0; std::vector<glm::vec3> normals; };
    std::map<std::pair<std::uint32_t, std::uint32_t>, Edge> edges;
    const auto indices = detail::triangles(mesh);
    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i+1], c = indices[i+2];
        auto normal = glm::cross(mesh.localVertices[b] - mesh.localVertices[a],
                                 mesh.localVertices[c] - mesh.localVertices[a]);
        const auto length = glm::length(normal);
        if (length <= 1e-12f) continue;
        normal /= length;
        const std::array<std::uint32_t, 3> corners{a,b,c};
        for (int e = 0; e < 3; ++e) {
            auto first = welded[corners[e]], second = welded[corners[(e+1)%3]];
            if (first == second) continue;
            if (first > second) std::swap(first, second);
            auto& edge = edges[{first, second}];
            edge.a = first; edge.b = second; edge.normals.push_back(normal);
        }
    }
    const float threshold = std::cos(glm::radians(mesh.creaseAngleDegrees));
    std::vector<std::uint32_t> result;
    for (const auto& item : edges) {
        const auto& edge = item.second;
        const bool show = mode == OutlineMode::AllTriangleEdges || edge.normals.size() != 2
            || glm::dot(edge.normals[0], edge.normals[1]) < threshold;
        if (show) { result.push_back(edge.a); result.push_back(edge.b); }
    }
    return result;
}

namespace detail {
// Ear clipping handles planar simple concave polygons, with either winding.
inline std::vector<std::uint32_t> triangulate(const std::vector<glm::vec3>& points) {
    if (points.size() < 3) throw std::runtime_error("Face has fewer than three corners.");
    glm::vec3 normal(0);
    for (std::size_t i=0; i<points.size(); ++i) {
        const auto& a=points[i]; const auto& b=points[(i+1)%points.size()];
        normal += glm::vec3((a.y-b.y)*(a.z+b.z), (a.z-b.z)*(a.x+b.x), (a.x-b.x)*(a.y+b.y));
    }
    if (glm::length(normal) < 1e-12f) throw std::runtime_error("Degenerate polygon.");
    const auto absNormal=glm::abs(normal);
    const int axis=absNormal.x>absNormal.y ? (absNormal.x>absNormal.z?0:2) : (absNormal.y>absNormal.z?1:2);
    std::vector<glm::dvec2> projected;
    for (const auto& p : points) projected.push_back(axis==0 ? glm::dvec2(p.y,p.z)
        : axis==1 ? glm::dvec2(p.x,p.z) : glm::dvec2(p.x,p.y));
    double area=0;
    for(std::size_t i=0;i<projected.size();++i) {
        const auto& a=projected[i]; const auto& b=projected[(i+1)%projected.size()];
        area+=a.x*b.y-b.x*a.y;
    }
    if (std::abs(area)<1e-14) throw std::runtime_error("Degenerate polygon projection.");
    const double direction=area>0?1:-1;
    auto cross=[](glm::dvec2 a,glm::dvec2 b,glm::dvec2 c) {return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);};
    std::vector<std::uint32_t> polygon(points.size()), result;
    std::iota(polygon.begin(),polygon.end(),0);
    while(polygon.size()>3) {
        bool clipped=false;
        for(std::size_t i=0;i<polygon.size();++i) {
            const auto a=polygon[(i+polygon.size()-1)%polygon.size()],b=polygon[i],c=polygon[(i+1)%polygon.size()];
            if(direction*cross(projected[a],projected[b],projected[c])<=1e-14) continue;
            bool inside=false;
            for(auto p:polygon) if(p!=a&&p!=b&&p!=c
                && direction*cross(projected[a],projected[b],projected[p])>=-1e-14
                && direction*cross(projected[b],projected[c],projected[p])>=-1e-14
                && direction*cross(projected[c],projected[a],projected[p])>=-1e-14) {inside=true;break;}
            if(inside) continue;
            result.insert(result.end(),{a,b,c}); polygon.erase(polygon.begin()+i); clipped=true; break;
        }
        if(!clipped) throw std::runtime_error("Cannot triangulate polygon; check repeated corners or self-intersections.");
    }
    result.insert(result.end(),polygon.begin(),polygon.end());
    return result;
}
inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first=std::uint32_t(destination.localVertices.size());
    destination.localVertices.insert(destination.localVertices.end(),source.localVertices.begin(),source.localVertices.end());
    destination.vertexNormals.insert(destination.vertexNormals.end(),source.vertexNormals.begin(),source.vertexNormals.end());
    destination.texcoords.insert(destination.texcoords.end(),source.texcoords.begin(),source.texcoords.end());
    for(auto i:triangles(source)) destination.triangleIndices.push_back(first+i);
    destination.shape=GLFace::Mesh;
}
inline bool shareEdge(const Mesh& a,const Mesh& b) {
    const auto ae=meshOutline(a),be=meshOutline(b);
    for(std::size_t i=0;i<ae.size();i+=2) for(std::size_t j=0;j<be.size();j+=2) {
        const auto p=a.localVertices[ae[i]],q=a.localVertices[ae[i+1]],r=b.localVertices[be[j]],s=b.localVertices[be[j+1]];
        if((p==r&&q==s)||(p==s&&q==r)) return true;
    }
    return false;
}
inline void mergeCoplanar(Model& model) {
    for(std::size_t i=0;i<model.faces.size();++i) {
        auto& face=model.faces[i];
        if(glm::length(face.normal)<1e-12f) continue;
        for(std::size_t j=i+1;j<model.faces.size();) {
            const auto& other=model.faces[j];
            const bool samePlane=glm::dot(face.normal,other.normal)>1-1e-6f
                && std::abs(glm::dot(face.normal,other.localVertices[0]-face.localVertices[0]))<1e-6f;
            if(samePlane && face.texcoords.empty()==other.texcoords.empty() && shareEdge(face,other)) {
                appendMesh(face,other); model.faces.erase(model.faces.begin()+j); j=i+1;
            } else ++j;
        }
    }
}
} // namespace detail
} // namespace graphics

```

## framework/include/Graphics.h

### before

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
    Scene();
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = Transform3D{});
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

    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));
    bool setModel(EntityId id, const Model& model);
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;
    bool setMeshVertices(EntityId id, std::size_t meshIndex,
                         const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);
    bool setVertexPosition(EntityId id, std::size_t meshIndex,
                           std::size_t vertexIndex, const glm::vec3& position);
    bool setTriangleIndices(EntityId id, std::size_t meshIndex,
                            const std::vector<std::uint32_t>& indices);
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));
    bool setVisible(EntityId id, bool visible);
    bool splitFace(EntityId id, std::size_t faceIndex);
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);

    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end,
                                 GlColor color, std::optional<EntityId> owner = std::nullopt);
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector,
                                   GlColor color, std::optional<EntityId> owner = std::nullopt);
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    std::optional<std::array<LineId, 3>> addBasis(
        const glm::vec3& origin, const glm::mat3& basisVectors = glm::mat3(1.0f),
        const glm::vec3& axisLengths = glm::vec3(1.0f),
        std::optional<EntityId> owner = std::nullopt);
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
    bool textures = false; // Explicit opt-in; no texture GPU work when false.
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
};

class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    using TextureMap = std::map<std::shared_ptr<const TextureImage>, GLuint,
                               std::owner_less<std::shared_ptr<const TextureImage>>>;
    TextureMap textures_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;
    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    bool upload(const Scene& scene, bool textures);
    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;
public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{}, const std::filesystem::path& fragmentPath = std::filesystem::path{});
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});
    void destroy(); // Call while the owning context is current, before it is destroyed.
};
} // namespace graphics
#include "../detail/Scene.inl"
#include "../detail/Renderer.inl"

```

### after

```cpp
#pragma once

#include "Geometry.h"
#include "../detail/Native.h"
#include <map>

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace graphics {

class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<FaceId> cubeFaceId_;
    std::vector<FaceId> tetraFaceId_;
    std::vector<FaceId> pyramidFaceId_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    void rebuildFaceIds();

public:
    Scene();
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron,
                                       const Transform3D& transform = Transform3D{});
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
    bool setParent(EntityId id, std::optional<EntityId> parent);
    std::optional<glm::mat4> worldMatrix(EntityId id) const;
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;
    bool setFaceTransform(EntityId id, std::size_t faceIndex,
                          const Transform3D& transform, bool independence);

    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));
    bool setModel(EntityId id, const Model& model);
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;
    bool setMeshVertices(EntityId id, std::size_t meshIndex,
                         const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);
    bool setVertexPosition(EntityId id, std::size_t meshIndex,
                           std::size_t vertexIndex, const glm::vec3& position);
    bool setTriangleIndices(EntityId id, std::size_t meshIndex,
                            const std::vector<std::uint32_t>& indices);
    bool generateTexcoords(EntityId id, std::size_t meshIndex, const UVOptions& options = UVOptions{});
    bool transformTexcoords(EntityId id, std::size_t meshIndex, const glm::vec2& scale,
                            const glm::vec2& offset, float rotationDegrees = 0);
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));
    bool setVisible(EntityId id, bool visible);
    bool splitFace(EntityId id, std::size_t faceIndex);
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);

    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end,
                                 GlColor color, std::optional<EntityId> owner = std::nullopt);
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector,
                                   GlColor color, std::optional<EntityId> owner = std::nullopt);
    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    std::optional<std::array<LineId, 3>> addBasis(
        const glm::vec3& origin, const glm::mat3& basisVectors = glm::mat3(1.0f),
        const glm::vec3& axisLengths = glm::vec3(1.0f),
        std::optional<EntityId> owner = std::nullopt);
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
    bool textures = false; // Explicit opt-in; no texture GPU work when false.
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
};

class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint textureLocation_ = -1, hasTextureLocation_ = -1, lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1;
    std::vector<DrawRange> drawRanges_;
    using TextureMap = std::map<std::shared_ptr<const TextureImage>, GLuint,
                               std::owner_less<std::shared_ptr<const TextureImage>>>;
    TextureMap textures_;
    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
    bool uploadedTextures_ = false;
    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    bool upload(const Scene& scene, bool textures);
    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;
public:
    Renderer() = default;
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{}, const std::filesystem::path& fragmentPath = std::filesystem::path{});
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});
    void destroy(); // Call while the owning context is current, before it is destroyed.
};
} // namespace graphics
#include "../detail/Scene.inl"
#include "../detail/Renderer.inl"

```

## framework/include/Model.h

### before

```cpp
#pragma once

#include "../dependencies/glm/glm/glm.hpp"
#include "../dependencies/glm/glm/gtc/quaternion.hpp"
#include "../dependencies/glm/glm/gtc/matrix_transform.hpp"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <utility>

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
// Initial allocation hints, not scene capacity limits.
constexpr std::size_t MAX_ENTITIES = 50;
constexpr std::size_t MAX_LINES = 500;
using EntityId = std::size_t;
using LineId = std::size_t;

struct GlColor { float red = 1.0f, green = 1.0f, blue = 1.0f; };
enum class GLShape { None, Cube, Tetra, Pyramid, Custom, Imported };
enum class GLFace { None, Rect, Triangle, Mesh };
enum class PivotMode { Preserve, BottomCenter, Custom };
enum class OutlineMode { None, FeatureEdges, AllTriangleEdges, Explicit };

struct Transform3D {
    glm::vec3 basis{0.0f};
    glm::vec3 localBasis{0.0f};
    glm::vec3 rotationDegrees{0.0f};
    glm::vec3 scale{1.0f};
    glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
};

struct TextureImage {
    std::uint32_t width = 0, height = 0;
    std::vector<std::uint8_t> rgba;
    // Row zero maps to UV v=0. File decoding flips top-down images by default.
    bool valid() const {
        return width && height && width <= 16384 && height <= 16384
            && rgba.size() == std::size_t(width) * height * 4;
    }
};

struct Material {
    GlColor color;
    std::shared_ptr<const TextureImage> texture;
    bool useTexture = false;
    bool repeatTexture = true;
};

struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f};
    glm::vec2 uv{0.0f};
    GlColor color;
};

struct PlaneFigure {
    GLFace shape = GLFace::Mesh;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{0.0f};
    // Compatibility flag: true enables child-local SRT, never world detachment.
    bool independence = false;
    Transform3D transform;
    bool visible = true;
    glm::mat4 localAnimationMatrix{1.0f};
    std::vector<GlColor> vertexColors;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> texcoords;
    std::vector<std::uint32_t> triangleIndices;
    std::vector<std::uint32_t> outlineIndices; // GL_LINES pairs, mesh-local indices.
    Material material;
    OutlineMode outline = OutlineMode::None;
    float creaseAngleDegrees = 1.0f;
    std::string name;
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices,
                                   const std::vector<std::uint32_t>& triangles) {
        PlaneFigure mesh;
        mesh.triangleIndices = triangles;
        for (const auto& vertex : vertices) {
            mesh.localVertices.push_back(vertex.position);
            mesh.vertexNormals.push_back(vertex.normal);
            mesh.texcoords.push_back(vertex.uv);
            mesh.vertexColors.push_back(vertex.color);
        }
        return mesh;
    }
};
using Mesh = PlaneFigure;

struct Polyhedron {
    GLShape shape = GLShape::Custom;
    std::vector<PlaneFigure> faces;
    std::string name;
    std::vector<Mesh>& meshes() { return faces; }
    const std::vector<Mesh>& meshes() const { return faces; }
};
using Model = Polyhedron;

struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{1.0f};
    bool visible = true;
    PivotMode pivotMode = PivotMode::Preserve;
};
struct FaceId { EntityId entityId = 0; std::size_t faceIndex = 0; };
struct Line3D {
    glm::vec3 start{0.0f}, end{0.0f};
    GlColor color;
    std::optional<EntityId> owner;
};

namespace detail {
inline bool finiteVector(const glm::vec3& v) {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}
inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c) for (int r = 0; r < 4; ++r)
        if (!std::isfinite(m[c][r])) return false;
    return true;
}
inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}
inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
    if (!mesh.triangleIndices.empty()) return mesh.triangleIndices;
    if (mesh.shape == GLFace::Triangle) return {0, 1, 2};
    if (mesh.shape == GLFace::Rect) return {0, 1, 2, 0, 2, 3};
    return {};
}
inline bool validMesh(const Mesh& m) {
    if (m.localVertices.size() < 3 || !validColor(m.color)
        || !validColor(m.material.color) || !finiteVector(m.normal)
        || !finiteMatrix(m.localAnimationMatrix)
        || !std::isfinite(m.creaseAngleDegrees) || m.creaseAngleDegrees < 0
        || m.creaseAngleDegrees > 180) return false;
    const auto count = m.localVertices.size();
    if (count > std::numeric_limits<std::uint32_t>::max()) return false;
    if ((!m.vertexColors.empty() && m.vertexColors.size() != count)
        || (!m.vertexNormals.empty() && m.vertexNormals.size() != count)
        || (!m.texcoords.empty() && m.texcoords.size() != count)
        || (m.material.texture && !m.material.texture->valid())
        || (m.material.useTexture && (!m.material.texture || m.texcoords.empty()))) return false;
    for (const auto& v : m.localVertices) if (!finiteVector(v)) return false;
    for (const auto& v : m.vertexNormals) if (!finiteVector(v)) return false;
    for (const auto& v : m.texcoords) if (!std::isfinite(v.x) || !std::isfinite(v.y)) return false;
    for (auto c : m.vertexColors) if (!validColor(c)) return false;
    const auto indices = triangles(m);
    if (indices.empty() || indices.size() % 3 || m.outlineIndices.size() % 2) return false;
    for (auto i : indices) if (i >= count) return false;
    for (auto i : m.outlineIndices) if (i >= count) return false;
    for (std::size_t i=0;i<indices.size();i+=3) {
        const auto a=glm::dvec3(m.localVertices[indices[i]]);
        const auto b=glm::dvec3(m.localVertices[indices[i+1]]);
        const auto c=glm::dvec3(m.localVertices[indices[i+2]]);
        const auto normal=glm::cross(b-a,c-a);
        const double area=glm::dot(normal,normal);
        if(!std::isfinite(area)||area<=0) return false;
    }
    return true;
}
inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());
    for (const auto& face : model.faces) for (const auto& p : face.localVertices) {
        low = glm::min(low, p); high = glm::max(high, p);
    }
    return {low, high};
}
inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);
    return {box.first.x * .5f + box.second.x * .5f, box.first.y,
            box.first.z * .5f + box.second.z * .5f};
}
inline glm::mat4 transformMatrix(const Transform3D& t) {
    glm::mat4 m = glm::translate(glm::mat4(1), t.basis);
    m = glm::rotate(m, glm::radians(t.rotationDegrees.x), {1, 0, 0});
    m = glm::rotate(m, glm::radians(t.rotationDegrees.y), {0, 1, 0});
    m = glm::rotate(m, glm::radians(t.rotationDegrees.z), {0, 0, 1});
    m *= glm::mat4_cast(glm::normalize(t.orientation));
    return glm::translate(glm::scale(m, t.scale), -t.localBasis);
}
inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
    glm::mat4 child(1);
    if (face.independence) {
        // Rotate/scale around a face pivot without moving an identity child.
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * transformMatrix(face.transform);
    }
    return transformMatrix(c.transform) * child * face.localAnimationMatrix;
}
} // namespace detail
} // namespace graphics

```

### after

```cpp
#pragma once

#include "../dependencies/glm/glm/glm.hpp"
#include "../dependencies/glm/glm/gtc/quaternion.hpp"
#include "../dependencies/glm/glm/gtc/matrix_transform.hpp"
#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <utility>

namespace graphics {

constexpr int WINDOW_WIDTH = 800;
constexpr int WINDOW_HEIGHT = 600;
// Initial allocation hints, not scene capacity limits.
constexpr std::size_t MAX_ENTITIES = 50;
constexpr std::size_t MAX_LINES = 500;
using EntityId = std::size_t;
using LineId = std::size_t;

struct GlColor { float red = 1.0f, green = 1.0f, blue = 1.0f; };
enum class GLShape { None, Cube, Tetra, Pyramid, Custom, Imported };
enum class GLFace { None, Rect, Triangle, Mesh };
enum class PivotMode { Preserve, BottomCenter, Custom };
enum class OutlineMode { None, FeatureEdges, AllTriangleEdges, Explicit };

struct Transform3D {
    glm::vec3 basis{0.0f};
    glm::vec3 localBasis{0.0f};
    glm::vec3 rotationDegrees{0.0f};
    glm::vec3 scale{1.0f};
    glm::quat orientation{1.0f, 0.0f, 0.0f, 0.0f};
};

struct TextureImage {
    std::uint32_t width = 0, height = 0;
    std::vector<std::uint8_t> rgba;
    // Row zero maps to UV v=0. File decoding flips top-down images by default.
    bool valid() const {
        return width && height && width <= 16384 && height <= 16384
            && rgba.size() == std::size_t(width) * height * 4;
    }
};

struct Material {
    GlColor color;
    std::shared_ptr<const TextureImage> texture;
    bool useTexture = false;
    bool repeatTexture = true;
};

struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f};
    glm::vec2 uv{0.0f};
    GlColor color;
};

struct PlaneFigure {
    GLFace shape = GLFace::Mesh;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{0.0f};
    // Compatibility flag: true enables child-local SRT, never world detachment.
    bool independence = false;
    Transform3D transform;
    bool visible = true;
    glm::mat4 localAnimationMatrix{1.0f};
    std::vector<GlColor> vertexColors;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> texcoords;
    std::vector<std::uint32_t> triangleIndices;
    std::vector<std::uint32_t> outlineIndices; // GL_LINES pairs, mesh-local indices.
    Material material;
    OutlineMode outline = OutlineMode::None;
    float creaseAngleDegrees = 1.0f;
    std::string name;
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices,
                                   const std::vector<std::uint32_t>& triangles) {
        PlaneFigure mesh;
        mesh.triangleIndices = triangles;
        for (const auto& vertex : vertices) {
            mesh.localVertices.push_back(vertex.position);
            mesh.vertexNormals.push_back(vertex.normal);
            mesh.texcoords.push_back(vertex.uv);
            mesh.vertexColors.push_back(vertex.color);
        }
        return mesh;
    }
};
using Mesh = PlaneFigure;

struct Polyhedron {
    GLShape shape = GLShape::Custom;
    std::vector<PlaneFigure> faces;
    std::string name;
    std::vector<Mesh>& meshes() { return faces; }
    const std::vector<Mesh>& meshes() const { return faces; }
};
using Model = Polyhedron;

struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{1.0f};
    bool visible = true;
    PivotMode pivotMode = PivotMode::Preserve;
    std::optional<EntityId> parent; // Local SRT is relative to this entity.
};
struct FaceId { EntityId entityId = 0; std::size_t faceIndex = 0; };
struct Line3D {
    glm::vec3 start{0.0f}, end{0.0f};
    GlColor color;
    std::optional<EntityId> owner;
};

namespace detail {
inline bool finiteVector(const glm::vec3& v) {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}
inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c) for (int r = 0; r < 4; ++r)
        if (!std::isfinite(m[c][r])) return false;
    return true;
}
inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}
inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
    if (!mesh.triangleIndices.empty()) return mesh.triangleIndices;
    if (mesh.shape == GLFace::Triangle) return {0, 1, 2};
    if (mesh.shape == GLFace::Rect) return {0, 1, 2, 0, 2, 3};
    return {};
}
inline bool validMesh(const Mesh& m) {
    if (m.localVertices.size() < 3 || !validColor(m.color)
        || !validColor(m.material.color) || !finiteVector(m.normal)
        || !finiteMatrix(m.localAnimationMatrix)
        || !std::isfinite(m.creaseAngleDegrees) || m.creaseAngleDegrees < 0
        || m.creaseAngleDegrees > 180) return false;
    const auto count = m.localVertices.size();
    if (count > std::numeric_limits<std::uint32_t>::max()) return false;
    if ((!m.vertexColors.empty() && m.vertexColors.size() != count)
        || (!m.vertexNormals.empty() && m.vertexNormals.size() != count)
        || (!m.texcoords.empty() && m.texcoords.size() != count)
        || (m.material.texture && !m.material.texture->valid())
        || (m.material.useTexture && (!m.material.texture || m.texcoords.empty()))) return false;
    for (const auto& v : m.localVertices) if (!finiteVector(v)) return false;
    for (const auto& v : m.vertexNormals) if (!finiteVector(v)) return false;
    for (const auto& v : m.texcoords) if (!std::isfinite(v.x) || !std::isfinite(v.y)) return false;
    for (auto c : m.vertexColors) if (!validColor(c)) return false;
    const auto indices = triangles(m);
    if (indices.empty() || indices.size() % 3 || m.outlineIndices.size() % 2) return false;
    for (auto i : indices) if (i >= count) return false;
    for (auto i : m.outlineIndices) if (i >= count) return false;
    for (std::size_t i=0;i<indices.size();i+=3) {
        const auto a=glm::dvec3(m.localVertices[indices[i]]);
        const auto b=glm::dvec3(m.localVertices[indices[i+1]]);
        const auto c=glm::dvec3(m.localVertices[indices[i+2]]);
        const auto normal=glm::cross(b-a,c-a);
        const double area=glm::dot(normal,normal);
        if(!std::isfinite(area)||area<=0) return false;
    }
    return true;
}
inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());
    for (const auto& face : model.faces) for (const auto& p : face.localVertices) {
        low = glm::min(low, p); high = glm::max(high, p);
    }
    return {low, high};
}
inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);
    return {box.first.x * .5f + box.second.x * .5f, box.first.y,
            box.first.z * .5f + box.second.z * .5f};
}
inline glm::mat4 transformMatrix(const Transform3D& t) {
    glm::mat4 m = glm::translate(glm::mat4(1), t.basis);
    m = glm::rotate(m, glm::radians(t.rotationDegrees.x), {1, 0, 0});
    m = glm::rotate(m, glm::radians(t.rotationDegrees.y), {0, 1, 0});
    m = glm::rotate(m, glm::radians(t.rotationDegrees.z), {0, 0, 1});
    m *= glm::mat4_cast(glm::normalize(t.orientation));
    return glm::translate(glm::scale(m, t.scale), -t.localBasis);
}
inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
    glm::mat4 child(1);
    if (face.independence) {
        // Rotate/scale around a face pivot without moving an identity child.
        child = glm::translate(glm::mat4(1), face.transform.localBasis)
              * transformMatrix(face.transform);
    }
    return transformMatrix(c.transform) * child * face.localAnimationMatrix;
}
} // namespace detail
} // namespace graphics

```

## framework/include/ModelLoader.h

### before

```cpp
#pragma once
#include "Geometry.h"
#include "Texture.h"
#include "../dependencies/assimp/include/assimp/cimport.h"
#include "../dependencies/assimp/include/assimp/scene.h"
#include "../dependencies/assimp/include/assimp/postprocess.h"
#include <fstream>
#include <sstream>
#include <functional>
#include <locale>

namespace graphics {
struct FeatureOptions { bool models = false; bool textures = false; };
struct LoadOptions {
    PivotMode pivot = PivotMode::BottomCenter;
    glm::vec3 customPivot{0};
    OutlineMode outline = OutlineMode::None;
    bool mergeCoplanarFaces = true; // primitive text only
    bool flipTextureRows = true;
    std::filesystem::path texturePath; // project-relative explicit override
};
struct ModelResult {
    std::optional<Model> model;
    glm::vec3 localBasis{0};
    PivotMode pivotMode = PivotMode::Preserve;
    std::string error;
    explicit operator bool() const { return model.has_value(); }
};
struct TextureResult {
    std::shared_ptr<const TextureImage> image;
    std::string error;
    explicit operator bool() const { return bool(image); }
};

namespace detail {
struct AssimpApi {
    Module module;
    bool ready = false;
    decltype(&::aiImportFile) importFile = nullptr;
    decltype(&::aiReleaseImport) releaseImport = nullptr;
    decltype(&::aiGetErrorString) getError = nullptr;
    decltype(&::aiGetMaterialColor) getColor = nullptr;
    decltype(&::aiGetMaterialTexture) getTexture = nullptr;
    void load(const std::filesystem::path& root) {
        if(ready) return;
        module.open((root.empty()?frameworkDirectory():std::filesystem::absolute(root))
                    / "dependencies/assimp/bin/assimp-vc145-mt.dll");
        importFile=module.symbol<decltype(importFile)>("aiImportFile");
        releaseImport=module.symbol<decltype(releaseImport)>("aiReleaseImport");
        getError=module.symbol<decltype(getError)>("aiGetErrorString");
        getColor=module.symbol<decltype(getColor)>("aiGetMaterialColor");
        getTexture=module.symbol<decltype(getTexture)>("aiGetMaterialTexture");
        ready=true;
    }
};
inline AssimpApi& assimp() { static AssimpApi api; return api; }
inline glm::mat4 assimpMatrix(const aiMatrix4x4& a) {
    return glm::mat4(a.a1,a.b1,a.c1,a.d1, a.a2,a.b2,a.c2,a.d2,
                     a.a3,a.b3,a.c3,a.d3, a.a4,a.b4,a.c4,a.d4);
}
inline std::size_t objIndex(const std::string& text,std::size_t count) {
    std::size_t consumed=0;
    const auto index=std::stoll(text,&consumed);
    if(consumed!=text.size() || index==0) throw std::runtime_error("Invalid OBJ index.");
    const auto resolved=index>0?index-1:static_cast<long long>(count)+index;
    if(resolved<0 || static_cast<unsigned long long>(resolved)>=count)
        throw std::runtime_error("OBJ index is out of range.");
    return std::size_t(resolved);
}
inline ModelResult finishLoad(Model model,const LoadOptions& options) {
    if(model.faces.empty()) throw std::runtime_error("Model contains no triangle meshes.");
    for(auto& face:model.faces) {
        face.outline=options.outline;
        if(!validMesh(face)) throw std::runtime_error("Invalid model mesh data.");
    }
    ModelResult result;
    if(options.pivot==PivotMode::BottomCenter) result.localBasis=bottomCenter(model);
    else if(options.pivot==PivotMode::Custom) result.localBasis=options.customPivot;
    if(!finiteVector(result.localBasis)) throw std::runtime_error("Invalid local basis.");
    result.pivotMode=options.pivot;
    result.model=std::move(model);
    return result;
}
} // namespace detail

class AssetLoader {
    std::filesystem::path projectRoot_,runtimeRoot_;
    FeatureOptions features_;
    mutable std::map<std::filesystem::path,std::weak_ptr<const TextureImage>> imageCache_;
    std::filesystem::path resolve(const std::filesystem::path& relative) const {
        if(relative.empty() || relative.is_absolute() || relative.has_root_name())
            throw std::runtime_error("Asset path must be project-relative.");
        const auto root=std::filesystem::weakly_canonical(projectRoot_);
        const auto full=std::filesystem::weakly_canonical(root/relative);
        auto a=root.begin(), b=full.begin();
        for(;a!=root.end();++a,++b) if(b==full.end() || *a!=*b)
            throw std::runtime_error("Asset path is outside projectRoot.");
        return full;
    }
    std::shared_ptr<const TextureImage> texture(const std::filesystem::path& relative,bool flip) const {
        if(!features_.textures) throw std::runtime_error("Texture mode is disabled.");
        const auto full=resolve(relative);
        // The row convention is part of the cache key.
        const auto key=full / (flip?"#flipped":"#native");
        if(auto cached=imageCache_[key].lock()) return cached;
        auto image=std::make_shared<const TextureImage>(detail::decodeImage(full,nullptr,flip));
        imageCache_[key]=image;
        return image;
    }
public:
    explicit AssetLoader(std::filesystem::path projectRoot=std::filesystem::current_path(),
                         FeatureOptions features=FeatureOptions{},std::filesystem::path runtimeRoot=std::filesystem::path{})
        : projectRoot_(std::filesystem::absolute(projectRoot)),runtimeRoot_(std::move(runtimeRoot)),features_(features) {}
    const FeatureOptions& features() const { return features_; }
    TextureResult loadTexture(const std::filesystem::path& relative,bool flipRows=true) const {
        try { return {texture(relative,flipRows),{}}; }
        catch(const std::exception& e) { return {nullptr,e.what()}; }
    }
    ModelResult loadPrimitive(const std::filesystem::path& relative,const LoadOptions& options=LoadOptions{}) const {
        try {
            if(!options.texturePath.empty()&&!features_.textures)
                throw std::runtime_error("Enable texture mode before specifying texturePath.");
            std::ifstream input(resolve(relative),std::ios::binary);
            if(!input) throw std::runtime_error("Cannot open primitive data.");
            input.imbue(std::locale::classic());
            std::vector<glm::vec3> positions,normals;
            std::vector<glm::vec2> uvs;
            Model model; model.shape=GLShape::Custom; model.name=relative.u8string();
            std::string line; std::size_t lineNumber=0;
            while(std::getline(input,line)) {
                ++lineNumber;
                if(lineNumber==1 && line.compare(0,3,"\xEF\xBB\xBF")==0) line.erase(0,3);
                line=line.substr(0,line.find('#'));
                std::istringstream row(line); row.imbue(std::locale::classic());
                std::string kind; row>>kind;
                if(kind=="v"||kind=="vn") {
                    glm::vec3 v; if(!(row>>v.x>>v.y>>v.z)||!detail::finiteVector(v))
                        throw std::runtime_error("Invalid vector at line "+std::to_string(lineNumber));
                    (kind=="v"?positions:normals).push_back(v);
                } else if(kind=="vt") {
                    glm::vec2 uv; if(!(row>>uv.x>>uv.y)||!std::isfinite(uv.x)||!std::isfinite(uv.y))
                        throw std::runtime_error("Invalid UV at line "+std::to_string(lineNumber));
                    uvs.push_back(uv);
                } else if(kind=="f") {
                    Mesh mesh; mesh.name="face_"+std::to_string(model.faces.size());
                    std::string corner; bool hasUV=false,hasNormal=false; std::size_t uvCount=0,normalCount=0;
                    while(row>>corner) {
                        const auto first=corner.find('/');
                        const auto second=first==std::string::npos?first:corner.find('/',first+1);
                        mesh.localVertices.push_back(positions.at(detail::objIndex(corner.substr(0,first),positions.size())));
                        glm::vec2 uv(0); glm::vec3 normal(0);
                        if(first!=std::string::npos) {
                            const auto part=corner.substr(first+1,second==std::string::npos?second:second-first-1);
                            if(!part.empty()) {uv=uvs.at(detail::objIndex(part,uvs.size()));hasUV=true;++uvCount;}
                        }
                        if(second!=std::string::npos) {
                            normal=normals.at(detail::objIndex(corner.substr(second+1),normals.size()));hasNormal=true;++normalCount;
                        }
                        mesh.texcoords.push_back(uv);mesh.vertexNormals.push_back(normal);
                    }
                    if((hasUV&&uvCount!=mesh.localVertices.size())||(hasNormal&&normalCount!=mesh.localVertices.size()))
                        throw std::runtime_error("Mixed OBJ corner attributes in one face.");
                    mesh.triangleIndices=detail::triangulate(mesh.localVertices);
                    if(!hasNormal) recalculateNormals(mesh);
                    if(!hasUV || !features_.textures) mesh.texcoords.clear();
                    const auto& indices=mesh.triangleIndices;
                    mesh.normal=glm::normalize(glm::cross(mesh.localVertices[indices[1]]-mesh.localVertices[indices[0]],
                                                          mesh.localVertices[indices[2]]-mesh.localVertices[indices[0]]));
                    model.faces.push_back(std::move(mesh));
                } else if(!kind.empty()&&kind!="g"&&kind!="o"&&kind!="s"&&kind!="mtllib"&&kind!="usemtl")
                    throw std::runtime_error("Unsupported primitive record at line "+std::to_string(lineNumber));
            }
            if(input.bad()) throw std::runtime_error("Primitive file read failed.");
            if(options.mergeCoplanarFaces) detail::mergeCoplanar(model);
            if(!options.texturePath.empty()) {
                auto image=texture(options.texturePath,options.flipTextureRows);
                for(auto& mesh:model.faces) {mesh.material.texture=image;mesh.material.useTexture=true;}
            }
            return detail::finishLoad(std::move(model),options);
        } catch(const std::exception& e) {ModelResult result;result.error=e.what();return result;}
    }
    ModelResult loadModel(const std::filesystem::path& relative,const LoadOptions& options=LoadOptions{}) const {
        ModelResult result;
        if(!features_.models) {result.error="Assimp model mode is disabled.";return result;}
        if(!features_.textures&&!options.texturePath.empty()) {result.error="Texture mode is disabled.";return result;}
        try {
            const auto path=resolve(relative);
            auto& api=detail::assimp(); api.load(runtimeRoot_);
            const aiScene* scene=api.importFile(path.u8string().c_str(),aiProcess_Triangulate
                |aiProcess_JoinIdenticalVertices|aiProcess_GenSmoothNormals|aiProcess_ValidateDataStructure);
            if(!scene) throw std::runtime_error(api.getError());
            struct Release { detail::AssimpApi& api; const aiScene* scene; ~Release(){api.releaseImport(scene);} } release{api,scene};
            if(!scene->mRootNode) throw std::runtime_error("Model has no root node.");
            Model model; model.shape=GLShape::Imported; model.name=relative.u8string();
            std::map<unsigned int,Material> materials;
            std::map<unsigned int,unsigned int> uvChannels;
            for(unsigned int i=0;i<scene->mNumMaterials;++i) {
                Material material; aiColor4D color;
                if(api.getColor(scene->mMaterials[i],AI_MATKEY_BASE_COLOR,&color)==AI_SUCCESS
                    ||api.getColor(scene->mMaterials[i],AI_MATKEY_COLOR_DIFFUSE,&color)==AI_SUCCESS)
                    material.color={color.r,color.g,color.b};
                unsigned int uvChannel=0;
                if(features_.textures) {
                    if(!options.texturePath.empty()) material.texture=texture(options.texturePath,options.flipTextureRows);
                    else {
                        aiString texturePath;
                        aiReturn found=api.getTexture(scene->mMaterials[i],aiTextureType_BASE_COLOR,0,&texturePath,nullptr,&uvChannel,nullptr,nullptr,nullptr,nullptr);
                        if(found!=AI_SUCCESS) found=api.getTexture(scene->mMaterials[i],aiTextureType_DIFFUSE,0,&texturePath,nullptr,&uvChannel,nullptr,nullptr,nullptr,nullptr);
                        if(found==AI_SUCCESS) {
                            const aiTexture* embedded=scene->GetEmbeddedTexture(texturePath.C_Str());
                            if(embedded) {
                                TextureImage image;
                                if(embedded->mHeight==0) {
                                    const auto* bytes=reinterpret_cast<const std::uint8_t*>(embedded->pcData);
                                    std::vector<std::uint8_t> data(bytes,bytes+embedded->mWidth);
                                    image=detail::decodeImage({},&data,options.flipTextureRows);
                                } else {
                                    image.width=embedded->mWidth;image.height=embedded->mHeight;
                                    if(!image.width||!image.height||image.width>16384||image.height>16384)
                                        throw std::runtime_error("Invalid embedded texture size.");
                                    image.rgba.resize(std::size_t(image.width)*image.height*4);
                                    for(unsigned int y=0;y<image.height;++y) for(unsigned int x=0;x<image.width;++x) {
                                        const auto& pixel=embedded->pcData[std::size_t(y)*image.width+x];
                                        const auto dest=(std::size_t(options.flipTextureRows?image.height-1-y:y)*image.width+x)*4;
                                        image.rgba[dest]=pixel.r;image.rgba[dest+1]=pixel.g;image.rgba[dest+2]=pixel.b;image.rgba[dest+3]=pixel.a;
                                    }
                                }
                                material.texture=std::make_shared<const TextureImage>(std::move(image));
                            } else material.texture=texture(relative.parent_path()/std::filesystem::u8path(texturePath.C_Str()),options.flipTextureRows);
                        }
                    }
                    material.useTexture=bool(material.texture);
                }
                materials[i]=material;uvChannels[i]=uvChannel;
            }
            std::function<void(const aiNode*,const glm::mat4&)> visit;
            visit=[&](const aiNode* node,const glm::mat4& parent) {
                const auto matrix=parent*detail::assimpMatrix(node->mTransformation);
                const auto linear=glm::mat3(matrix);const auto determinant=glm::determinant(linear);
                const bool invertible=std::isfinite(determinant)&&std::abs(determinant)>1e-12f;
                const auto normalMatrix=invertible?glm::transpose(glm::inverse(linear)):glm::mat3(1);
                for(unsigned int n=0;n<node->mNumMeshes;++n) {
                    if(node->mMeshes[n]>=scene->mNumMeshes) throw std::runtime_error("Invalid node mesh index.");
                    const auto* source=scene->mMeshes[node->mMeshes[n]];
                    if(!source->HasPositions()) continue;
                    Mesh mesh; mesh.name=node->mName.C_Str();mesh.material=materials[source->mMaterialIndex];
                    const auto uvChannel=uvChannels[source->mMaterialIndex];
                    for(unsigned int v=0;v<source->mNumVertices;++v) {
                        const auto p=source->mVertices[v];mesh.localVertices.push_back(glm::vec3(matrix*glm::vec4(p.x,p.y,p.z,1)));
                        if(source->HasNormals()&&invertible) {
                            const auto normal=source->mNormals[v];const auto transformed=normalMatrix*glm::vec3(normal.x,normal.y,normal.z);
                            mesh.vertexNormals.push_back(glm::length(transformed)>1e-12f?glm::normalize(transformed):glm::vec3(0));
                        }
                        if(features_.textures&&source->HasTextureCoords(uvChannel)) {
                            const auto uv=source->mTextureCoords[uvChannel][v];mesh.texcoords.push_back({uv.x,uv.y});
                        }
                        if(source->HasVertexColors(0)) {const auto c=source->mColors[0][v];mesh.vertexColors.push_back({c.r,c.g,c.b});}
                    }
                    for(unsigned int f=0;f<source->mNumFaces;++f) {
                        const auto& face=source->mFaces[f];if(face.mNumIndices!=3) continue;
                        mesh.triangleIndices.insert(mesh.triangleIndices.end(),{face.mIndices[0],
                            face.mIndices[determinant<0?2:1],face.mIndices[determinant<0?1:2]});
                    }
                    if(mesh.triangleIndices.empty()) continue;
                    if(mesh.vertexNormals.empty()) recalculateNormals(mesh);
                    if(mesh.material.useTexture&&mesh.texcoords.empty()) throw std::runtime_error("Textured mesh is missing the requested UV channel.");
                    model.faces.push_back(std::move(mesh));
                }
                for(unsigned int i=0;i<node->mNumChildren;++i) visit(node->mChildren[i],matrix);
            };
            visit(scene->mRootNode,glm::mat4(1));
            return detail::finishLoad(std::move(model),options);
        } catch(const std::exception& e) {result.error=e.what();return result;}
    }
};
} // namespace graphics

```

### after

```cpp
#pragma once
#include "Geometry.h"
#include "Texture.h"

namespace graphics {
struct FeatureOptions { bool models = false; bool textures = false; };
struct LoadOptions {
    PivotMode pivot = PivotMode::BottomCenter;
    glm::vec3 customPivot{0};
    OutlineMode outline = OutlineMode::None;
    bool mergeCoplanarFaces = true; // primitive text only
    bool flipTextureRows = true;
    std::filesystem::path texturePath; // project-relative explicit override
};
struct ModelResult {
    std::optional<Model> model;
    glm::vec3 localBasis{0};
    PivotMode pivotMode = PivotMode::Preserve;
    std::string error;
    explicit operator bool() const { return model.has_value(); }
};
struct TextureResult {
    std::shared_ptr<const TextureImage> image;
    std::string error;
    explicit operator bool() const { return bool(image); }
};

namespace detail {
inline ModelResult finishLoad(Model model,const LoadOptions& options) {
    if(model.faces.empty()) throw std::runtime_error("Model contains no triangle meshes.");
    for(auto& face:model.faces) {
        face.outline=options.outline;
        if(!validMesh(face)) throw std::runtime_error("Invalid model mesh data.");
    }
    ModelResult result;
    if(options.pivot==PivotMode::BottomCenter) result.localBasis=bottomCenter(model);
    else if(options.pivot==PivotMode::Custom) result.localBasis=options.customPivot;
    if(!finiteVector(result.localBasis)) throw std::runtime_error("Invalid local basis.");
    result.pivotMode=options.pivot;
    result.model=std::move(model);
    return result;
}
} // namespace detail

class AssetLoader {
    std::filesystem::path projectRoot_,runtimeRoot_;
    FeatureOptions features_;
    mutable std::map<std::filesystem::path,std::weak_ptr<const TextureImage>> imageCache_;
    std::filesystem::path resolve(const std::filesystem::path& relative) const {
        if(relative.empty() || relative.is_absolute() || relative.has_root_name())
            throw std::runtime_error("Asset path must be project-relative.");
        const auto root=std::filesystem::weakly_canonical(projectRoot_);
        const auto full=std::filesystem::weakly_canonical(root/relative);
        auto a=root.begin(), b=full.begin();
        for(;a!=root.end();++a,++b) if(b==full.end() || *a!=*b)
            throw std::runtime_error("Asset path is outside projectRoot.");
        return full;
    }
    std::shared_ptr<const TextureImage> texture(const std::filesystem::path& relative,bool flip) const {
        if(!features_.textures) throw std::runtime_error("Texture mode is disabled.");
        const auto full=resolve(relative);
        // The row convention is part of the cache key.
        const auto key=full / (flip?"#flipped":"#native");
        if(auto cached=imageCache_[key].lock()) return cached;
        auto image=std::make_shared<const TextureImage>(detail::decodeImage(full,nullptr,flip));
        imageCache_[key]=image;
        return image;
    }
public:
    explicit AssetLoader(std::filesystem::path projectRoot=std::filesystem::current_path(),
                         FeatureOptions features=FeatureOptions{},std::filesystem::path runtimeRoot=std::filesystem::path{})
        : projectRoot_(std::filesystem::absolute(projectRoot)),runtimeRoot_(std::move(runtimeRoot)),features_(features) {}
    const FeatureOptions& features() const { return features_; }
    TextureResult loadTexture(const std::filesystem::path& relative,bool flipRows=true) const {
        try { return {texture(relative,flipRows),{}}; }
        catch(const std::exception& e) { return {nullptr,e.what()}; }
    }
    ModelResult loadPrimitive(const std::filesystem::path& relative,const LoadOptions& options=LoadOptions{}) const;
    ModelResult loadModel(const std::filesystem::path& relative,const LoadOptions& options=LoadOptions{}) const;
};
} // namespace graphics

#include "../detail/PrimitiveLoader.inl"
#include "../detail/AssimpLoader.inl"

```

## framework/include/Picking.h

### before

```cpp
#pragma once
#include "Graphics.h"

namespace graphics {
struct Ray {
    glm::vec3 origin{0}, direction{0,0,-1};
    float maxDistance = std::numeric_limits<float>::infinity();
};
struct PickResult {
    EntityId entityId = 0;
    std::size_t faceIndex = 0, triangleIndex = 0;
    glm::vec3 worldPoint{0}, localPoint{0}, barycentric{0};
    glm::vec2 uv{0};
    bool hasUV = false;
    float distance = 0; // Distance from ray origin (near plane for a screen ray).
};
struct PickViewport { double x = 0, y = 0, width = 0, height = 0; }; // top-left framebuffer pixels

inline std::optional<Ray> screenRay(const Camera& camera,double x,double y,const PickViewport& viewport) {
    if(!CameraSystem::validCamera(camera)||!std::isfinite(x)||!std::isfinite(y)
        ||!std::isfinite(viewport.x)||!std::isfinite(viewport.y)
        ||!std::isfinite(viewport.width)||!std::isfinite(viewport.height)
        ||viewport.width<=0||viewport.height<=0||x<viewport.x||y<viewport.y
        ||x>=viewport.x+viewport.width||y>=viewport.y+viewport.height)return {};
    const auto pv=CameraSystem::projectionMatrix(camera,float(viewport.width/viewport.height))*CameraSystem::viewMatrix(camera);
    const auto determinant=glm::determinant(pv);
    if(!std::isfinite(determinant)||std::abs(determinant)<1e-12f)return {};
    const auto inverse=glm::inverse(pv);
    const float nx=float(2*(x-viewport.x)/viewport.width-1),ny=float(1-2*(y-viewport.y)/viewport.height);
    auto nearPoint=inverse*glm::vec4(nx,ny,-1,1),farPoint=inverse*glm::vec4(nx,ny,1,1);
    if(std::abs(nearPoint.w)<1e-12f||std::abs(farPoint.w)<1e-12f)return {};
    nearPoint/=nearPoint.w;farPoint/=farPoint.w;
    const auto direction=glm::vec3(farPoint-nearPoint);const auto length=glm::length(direction);
    if(!detail::finiteVector(glm::vec3(nearPoint))||!detail::finiteVector(direction)||!std::isfinite(length)||length<=0)return {};
    return Ray{glm::vec3(nearPoint),direction/length,length};
}
inline std::optional<PickResult> pick(const Scene& scene,const Ray& input) {
    if(!detail::finiteVector(input.origin)||!detail::finiteVector(input.direction)
        ||!std::isfinite(glm::length(input.direction))||glm::length(input.direction)<1e-12f||std::isnan(input.maxDistance)||input.maxDistance<0)return {};
    const auto direction=glm::dvec3(glm::normalize(input.direction));
    const auto origin=glm::dvec3(input.origin);
    double nearest=input.maxDistance;std::optional<PickResult> result;
    for(auto id:scene.getIds()) {
        const auto* component=scene.getComponent(id);if(!component||!component->visible)continue;
        for(std::size_t f=0;f<component->polyhedron.faces.size();++f) {
            const auto& face=component->polyhedron.faces[f];if(!face.visible)continue;
            const auto matrix=DiagramSystem::faceModelMatrix(*component,face);
            const auto indices=detail::triangles(face);
            for(std::size_t t=0;t<indices.size();t+=3) {
                const auto ia=indices[t],ib=indices[t+1],ic=indices[t+2];
                const auto a4=matrix*glm::vec4(face.localVertices[ia],1),b4=matrix*glm::vec4(face.localVertices[ib],1),c4=matrix*glm::vec4(face.localVertices[ic],1);
                if(std::abs(a4.w)<1e-12f||std::abs(b4.w)<1e-12f||std::abs(c4.w)<1e-12f)continue;
                const auto a=glm::dvec3(a4/a4.w),b=glm::dvec3(b4/b4.w),c=glm::dvec3(c4/c4.w);
                const auto edge1=b-a,edge2=c-a,p=glm::cross(direction,edge2);
                const auto determinant=glm::dot(edge1,p);
                const auto tolerance=1e-12*glm::length(edge1)*glm::length(edge2);
                if(!std::isfinite(determinant)||std::abs(determinant)<=tolerance)continue;
                const auto relative=origin-a;const auto u=glm::dot(relative,p)/determinant;
                if(u<0||u>1)continue;
                const auto q=glm::cross(relative,edge1);const auto v=glm::dot(direction,q)/determinant;
                if(v<0||u+v>1)continue;
                const auto distance=glm::dot(edge2,q)/determinant;
                if(!std::isfinite(distance)||distance<0||distance>nearest)continue;
                const glm::dvec3 weights((1-u-v)/a4.w,u/b4.w,v/c4.w);
                const auto sum=weights.x+weights.y+weights.z;if(std::abs(sum)<1e-12)continue;
                PickResult hit;hit.entityId=id;hit.faceIndex=f;hit.triangleIndex=t/3;
                hit.distance=float(distance);hit.worldPoint=glm::vec3(origin+direction*distance);
                hit.barycentric=glm::vec3(weights/sum);
                hit.localPoint=face.localVertices[ia]*hit.barycentric.x+face.localVertices[ib]*hit.barycentric.y+face.localVertices[ic]*hit.barycentric.z;
                hit.hasUV=!face.texcoords.empty();
                if(hit.hasUV)hit.uv=face.texcoords[ia]*hit.barycentric.x+face.texcoords[ib]*hit.barycentric.y+face.texcoords[ic]*hit.barycentric.z;
                nearest=distance;result=hit;
            }
        }
    }
    return result;
}
inline std::optional<PickResult> pick(const Scene& scene,const Camera& camera,double x,double y,
                                     const PickViewport& viewport) {
    const auto ray=screenRay(camera,x,y,viewport);return ray?pick(scene,*ray):std::nullopt;
}
} // namespace graphics

```

### after

```cpp
#pragma once
#include "Graphics.h"

namespace graphics {
struct Ray {
    glm::vec3 origin{0}, direction{0,0,-1};
    float maxDistance = std::numeric_limits<float>::infinity();
};
struct PickResult {
    EntityId entityId = 0;
    std::size_t faceIndex = 0, triangleIndex = 0;
    glm::vec3 worldPoint{0}, localPoint{0}, barycentric{0};
    glm::vec2 uv{0};
    bool hasUV = false;
    float distance = 0; // Distance from ray origin (near plane for a screen ray).
};
struct PickViewport { double x = 0, y = 0, width = 0, height = 0; }; // top-left framebuffer pixels

inline std::optional<Ray> screenRay(const Camera& camera,double x,double y,const PickViewport& viewport) {
    if(!CameraSystem::validCamera(camera)||!std::isfinite(x)||!std::isfinite(y)
        ||!std::isfinite(viewport.x)||!std::isfinite(viewport.y)
        ||!std::isfinite(viewport.width)||!std::isfinite(viewport.height)
        ||viewport.width<=0||viewport.height<=0||x<viewport.x||y<viewport.y
        ||x>=viewport.x+viewport.width||y>=viewport.y+viewport.height)return {};
    const auto pv=CameraSystem::projectionMatrix(camera,float(viewport.width/viewport.height))*CameraSystem::viewMatrix(camera);
    const auto determinant=glm::determinant(pv);
    if(!std::isfinite(determinant)||std::abs(determinant)<1e-12f)return {};
    const auto inverse=glm::inverse(pv);
    const float nx=float(2*(x-viewport.x)/viewport.width-1),ny=float(1-2*(y-viewport.y)/viewport.height);
    auto nearPoint=inverse*glm::vec4(nx,ny,-1,1),farPoint=inverse*glm::vec4(nx,ny,1,1);
    if(std::abs(nearPoint.w)<1e-12f||std::abs(farPoint.w)<1e-12f)return {};
    nearPoint/=nearPoint.w;farPoint/=farPoint.w;
    const auto direction=glm::vec3(farPoint-nearPoint);const auto length=glm::length(direction);
    if(!detail::finiteVector(glm::vec3(nearPoint))||!detail::finiteVector(direction)||!std::isfinite(length)||length<=0)return {};
    return Ray{glm::vec3(nearPoint),direction/length,length};
}
inline std::optional<PickResult> pick(const Scene& scene,const Ray& input) {
    if(!detail::finiteVector(input.origin)||!detail::finiteVector(input.direction)
        ||!std::isfinite(glm::length(input.direction))||glm::length(input.direction)<1e-12f||std::isnan(input.maxDistance)||input.maxDistance<0)return {};
    const auto direction=glm::dvec3(glm::normalize(input.direction));
    const auto origin=glm::dvec3(input.origin);
    double nearest=input.maxDistance;std::optional<PickResult> result;
    for(auto id:scene.getIds()) {
        const auto* component=scene.getComponent(id);if(!component||!component->visible)continue;
        for(std::size_t f=0;f<component->polyhedron.faces.size();++f) {
            const auto& face=component->polyhedron.faces[f];if(!face.visible)continue;
            const auto world=scene.faceWorldMatrix(id,f);if(!world)continue;
            const auto matrix=*world;
            const auto indices=detail::triangles(face);
            for(std::size_t t=0;t<indices.size();t+=3) {
                const auto ia=indices[t],ib=indices[t+1],ic=indices[t+2];
                const auto a4=matrix*glm::vec4(face.localVertices[ia],1),b4=matrix*glm::vec4(face.localVertices[ib],1),c4=matrix*glm::vec4(face.localVertices[ic],1);
                if(std::abs(a4.w)<1e-12f||std::abs(b4.w)<1e-12f||std::abs(c4.w)<1e-12f)continue;
                const auto a=glm::dvec3(a4/a4.w),b=glm::dvec3(b4/b4.w),c=glm::dvec3(c4/c4.w);
                const auto edge1=b-a,edge2=c-a,p=glm::cross(direction,edge2);
                const auto determinant=glm::dot(edge1,p);
                const auto tolerance=1e-12*glm::length(edge1)*glm::length(edge2);
                if(!std::isfinite(determinant)||std::abs(determinant)<=tolerance)continue;
                const auto relative=origin-a;const auto u=glm::dot(relative,p)/determinant;
                if(u<0||u>1)continue;
                const auto q=glm::cross(relative,edge1);const auto v=glm::dot(direction,q)/determinant;
                if(v<0||u+v>1)continue;
                const auto distance=glm::dot(edge2,q)/determinant;
                if(!std::isfinite(distance)||distance<0||distance>nearest)continue;
                const glm::dvec3 weights((1-u-v)/a4.w,u/b4.w,v/c4.w);
                const auto sum=weights.x+weights.y+weights.z;if(std::abs(sum)<1e-12)continue;
                PickResult hit;hit.entityId=id;hit.faceIndex=f;hit.triangleIndex=t/3;
                hit.distance=float(distance);hit.worldPoint=glm::vec3(origin+direction*distance);
                hit.barycentric=glm::vec3(weights/sum);
                hit.localPoint=face.localVertices[ia]*hit.barycentric.x+face.localVertices[ib]*hit.barycentric.y+face.localVertices[ic]*hit.barycentric.z;
                hit.hasUV=!face.texcoords.empty();
                if(hit.hasUV)hit.uv=face.texcoords[ia]*hit.barycentric.x+face.texcoords[ib]*hit.barycentric.y+face.texcoords[ic]*hit.barycentric.z;
                nearest=distance;result=hit;
            }
        }
    }
    return result;
}
inline std::optional<PickResult> pick(const Scene& scene,const Camera& camera,double x,double y,
                                     const PickViewport& viewport) {
    const auto ray=screenRay(camera,x,y,viewport);return ray?pick(scene,*ray):std::nullopt;
}
} // namespace graphics

```

## framework-analysis/update_graph.py

### before

없음

### after

```python
from pathlib import Path
import json
import hashlib
import subprocess
import graphify.extract as extraction_api
from graphify.detect import detect, save_manifest
from graphify.build import build_from_json
from graphify.cluster import cluster, score_all
from graphify.analyze import god_nodes, surprising_connections, suggest_questions
from graphify.report import generate
from graphify.export import to_json, to_html
from graphify.diagnostics import diagnose_extraction, format_diagnostic_report

base = Path(__file__).resolve().parent
root = base.parent / 'framework'
output = base / 'graphify-out'
extraction_api._DISPATCH['.inl'] = extraction_api.extract_cpp
files = sorted(p for p in root.rglob('*') if p.is_file()
               and p.suffix in {'.h', '.cpp', '.inl'} and 'dependencies' not in p.parts)
detection = detect(root, extra_excludes=['dependencies/**', 'docs/**', '*.md', 'shaders/**'], cache_root=base)
detection['files'] = {'code': [str(p) for p in files], 'document': [], 'paper': [], 'image': [], 'video': []}
detection['total_files'] = len(files)
detection['total_words'] = sum(len(p.read_text(encoding='utf-8-sig').split()) for p in files)
extraction = extraction_api.extract(files, cache_root=base, root=root)
graph = build_from_json(extraction, root=str(root), directed=True)
old = json.loads((output / 'graph.json').read_text(encoding='utf-8-sig'))
if not graph.number_of_nodes() or graph.number_of_nodes() < len(old['nodes']):
    raise RuntimeError('Empty or smaller graph: existing graph preserved')
communities = cluster(graph)
cohesion = score_all(graph, communities)
names = {'Native.h': '네이티브 함수 연결', 'Geometry.h': 'UV와 기하 계산',
         'Scene.inl': '장면과 계층 변환', 'Renderer.inl': 'GPU 렌더링',
         'Picking.h': '교점과 UV 피킹', 'PrimitiveLoader.inl': '기본 도형 파서',
         'AssimpLoader.inl': '일반 모델 로딩', 'Application.inl': '앱 실행과 입력',
         'Model.h': '모델과 재질 데이터', 'Graphics.h': '장면 공개 API',
         'Texture.h': '이미지 디코딩', 'Shaders.h': '내장 셰이더'}
labels = {}
for cid, members in communities.items():
    counts = {}
    for node in members:
        source = Path(graph.nodes[node].get('source_file', '')).name
        counts[source] = counts.get(source, 0) + 1
    source = max(counts, key=counts.get) if counts else 'Framework'
    labels[cid] = names.get(source, source + ' 구조')
gods = god_nodes(graph)
surprises = surprising_connections(graph, communities)
questions = suggest_questions(graph, communities, labels)
staged = output / 'graph.generated.json'
if not to_json(graph, communities, str(staged), force=True, community_labels=labels):
    raise RuntimeError('Graph export failed')
staged.write_text(staged.read_text(encoding='utf-8-sig'), encoding='utf-8-sig')
staged.replace(output / 'graph.json')

def dump(name, value):
    (output / name).write_text(json.dumps(value, ensure_ascii=False, indent=2), encoding='utf-8-sig')

dump('EXTRACTION.json', extraction)
dump('DETECTION.json', detection)
dump('COMMUNITIES.json', {str(k): v for k, v in labels.items()})
dump('ANALYSIS.json', {'communities': {str(k): v for k, v in communities.items()},
                     'cohesion': {str(k): v for k, v in cohesion.items()},
                     'gods': gods, 'surprises': surprises, 'questions': questions})
report = generate(graph, communities, cohesion, labels, gods, surprises, detection,
                  {'input': 0, 'output': 0}, str(root), suggested_questions=questions)
(output / 'GRAPH_REPORT.md').write_text(report, encoding='utf-8-sig')
health = diagnose_extraction(extraction, directed=True, root=str(root))
(output / 'GRAPH_HEALTH.md').write_text(format_diagnostic_report(health), encoding='utf-8-sig')
if not to_html(graph, communities, str(output / 'graph.html'), community_labels=labels):
    raise RuntimeError('HTML export incomplete')
html = output / 'graph.html'
html.write_text(html.read_text(encoding='utf-8-sig'), encoding='utf-8-sig')
save_manifest({'code': [str(p) for p in files]}, str(output / 'manifest.json'),
              kind='ast', root=root, scan_corpus=[str(p) for p in files])
manifest = output / 'manifest.json'
manifest.write_text(manifest.read_text(encoding='utf-8-sig'), encoding='utf-8-sig')
commit = subprocess.check_output(['git', 'rev-parse', 'HEAD'], cwd=base, text=True).strip()
dump('SOURCE_MANIFEST.json', {'source_root': str(root), 'output_root': str(output),
                            'source_commit': commit, 'includes_uncommitted_step4': True,
                            'files': [{'path': p.relative_to(root).as_posix(),
                                       'sha256': hashlib.sha256(p.read_bytes()).hexdigest()} for p in files]})
dump('BENCHMARK.json', {'status': 'not_run', 'reason': 'User delegates tests and benchmarks; previous estimate is obsolete.'})
dump('cost.json', {'run': '2026-10-12', 'mode': 'AST only', 'input_tokens': 0,
                   'output_tokens': 0, 'external_api_calls': 0})
print(json.dumps({'files': len(files), 'nodes': graph.number_of_nodes(),
                  'edges': graph.number_of_edges(), 'communities': len(communities)}))
print(format_diagnostic_report(health))

```

