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
    decltype(&::aiImportFile) importFile = nullptr;
    decltype(&::aiReleaseImport) releaseImport = nullptr;
    decltype(&::aiGetErrorString) getError = nullptr;
    decltype(&::aiGetMaterialColor) getColor = nullptr;
    decltype(&::aiGetMaterialTexture) getTexture = nullptr;
    void load(const std::filesystem::path& root) {
        if(importFile) return;
        module.open((root.empty()?frameworkDirectory():std::filesystem::absolute(root))
                    / "dependencies/assimp/bin/assimp-vc145-mt.dll");
        importFile=module.symbol<decltype(importFile)>("aiImportFile");
        releaseImport=module.symbol<decltype(releaseImport)>("aiReleaseImport");
        getError=module.symbol<decltype(getError)>("aiGetErrorString");
        getColor=module.symbol<decltype(getColor)>("aiGetMaterialColor");
        getTexture=module.symbol<decltype(getTexture)>("aiGetMaterialTexture");
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
                         FeatureOptions features={},std::filesystem::path runtimeRoot={})
        : projectRoot_(std::filesystem::absolute(projectRoot)),runtimeRoot_(std::move(runtimeRoot)),features_(features) {}
    const FeatureOptions& features() const { return features_; }
    TextureResult loadTexture(const std::filesystem::path& relative,bool flipRows=true) const {
        try { return {texture(relative,flipRows),{}}; }
        catch(const std::exception& e) { return {nullptr,e.what()}; }
    }
    ModelResult loadPrimitive(const std::filesystem::path& relative,const LoadOptions& options={}) const {
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
    ModelResult loadModel(const std::filesystem::path& relative,const LoadOptions& options={}) const {
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
