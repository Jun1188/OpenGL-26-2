#pragma once
#include "Geometry.h"
#include "Texture.h"

namespace graphics {
struct FeatureOptions { bool textures = false; };
struct LoadOptions {
    PivotMode pivot = PivotMode::BottomCenter;
    glm::vec3 customPivot{0};
    OutlineMode outline = OutlineMode::None;
    bool mergeCoplanarFaces = true; // within each OBJ object/group/material/smoothing segment
    bool flipTextureRows = true;
    std::filesystem::path texturePath; // project-relative explicit override
    bool allowObjFallback = true;
};
struct ModelResult {
    std::optional<Model> model;
    glm::vec3 localBasis{0};
    PivotMode pivotMode = PivotMode::Preserve;
    std::string error;
    std::vector<std::string> warnings;
    bool usedFallback = false;
    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
    explicit operator bool() const { return model.has_value(); }
};
struct TextureResult {
    std::shared_ptr<const TextureImage> image;
    std::string error;
    /// [API] 모델 또는 이미지 결과가 있는지 성공 여부로 변환한다. 실패 시 error 문자열을 확인한다.
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
    std::filesystem::path projectRoot_;
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
    /// [API] 프로젝트 루트와 기능 옵션을 보관하고 상대경로 자산 로딩을 준비한다.
    explicit AssetLoader(std::filesystem::path projectRoot=detail::executableDirectory(),
                         FeatureOptions features=FeatureOptions{})
        : projectRoot_(std::filesystem::absolute(projectRoot)),features_(features) {}
    /// [API] 로더의 텍스처 등 기능 사용 옵션을 읽기 전용 참조로 반환한다.
    const FeatureOptions& features() const { return features_; }
    /// [API] 프로젝트 상대경로의 이미지를 읽어 TextureResult로 반환한다. 실패 원인은 error에 담는다.
    TextureResult loadTexture(const std::filesystem::path& relative,bool flipRows=true) const {
        try { return {texture(relative,flipRows),{}}; }
        catch(const std::exception& e) { return {nullptr,e.what()}; }
    }
    /// [API] 프로젝트 상대경로의 OBJ를 직접 파싱하여 모델·피벗·경고·오류 결과를 반환한다.
    ModelResult loadObj(const std::filesystem::path& relative,const LoadOptions& options=LoadOptions{}) const;
    /// [API] 기본 도형 OBJ를 읽는 호환 진입점이다. OBJ 로더로 처리를 위임한다.
    ModelResult loadPrimitive(const std::filesystem::path& relative,const LoadOptions& options=LoadOptions{}) const;
    /// [API] 모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.
    ModelResult loadModel(const std::filesystem::path& relative,const LoadOptions& options=LoadOptions{}) const;
};
} // namespace graphics

#include "../detail/PrimitiveLoader.inl"
#include "../detail/ObjLoader.inl"
namespace graphics {
/// [API] 모델 로딩 호환 진입점이다. 현재 구현은 loadObj로 위임하며 다른 형식 변환은 하지 않는다.
inline ModelResult AssetLoader::loadModel(const std::filesystem::path& relative,const LoadOptions& options) const {
    return loadObj(relative,options);
}
} // namespace graphics
