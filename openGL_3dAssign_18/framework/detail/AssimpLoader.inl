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
