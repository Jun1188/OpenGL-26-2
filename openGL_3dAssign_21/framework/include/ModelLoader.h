#pragma once
#include "Geometry.h"
#include "Texture.h"

namespace graphics {
struct FeatureOptions {
    bool textures = false;
};

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

    explicit operator bool() const {
        return model.has_value();
    }
};

struct TextureResult {
    std::shared_ptr<const TextureImage> image;
    std::string error;

    explicit operator bool() const {
        return bool(image);
    }
};

namespace detail {
inline ModelResult finishLoad(Model model, const LoadOptions& options) {
    if (model.faces.empty())
        throw std::runtime_error("Model contains no triangle meshes.");

    for (auto& face : model.faces) {
        face.outline = options.outline;

        if (!validMesh(face))
            throw std::runtime_error("Invalid model mesh data.");
    }

    ModelResult result;

    if (options.pivot == PivotMode::BottomCenter)
        result.localBasis = bottomCenter(model);
    else if (options.pivot == PivotMode::Custom)
        result.localBasis = options.customPivot;

    if (!finiteVector(result.localBasis))
        throw std::runtime_error("Invalid local basis.");
    result.pivotMode = options.pivot;
    result.model = std::move(model);

    return result;
}
} // namespace detail

class AssetLoader {
    std::filesystem::path projectRoot_;

    FeatureOptions features_;

    mutable std::map<std::filesystem::path, std::weak_ptr<const TextureImage>> imageCache_;

    std::filesystem::path resolve(const std::filesystem::path& relative) const {
        if (relative.empty() || relative.is_absolute() || relative.has_root_name())
            throw std::runtime_error("Asset path must be project-relative.");

        const auto root = std::filesystem::weakly_canonical(projectRoot_);
        const auto full = std::filesystem::weakly_canonical(root / relative);
        auto a = root.begin(), b = full.begin();

        for (; a != root.end(); ++a, ++b)
            if (b == full.end() || *a != *b)
                throw std::runtime_error("Asset path is outside projectRoot.");

        return full;
    }

    std::shared_ptr<const TextureImage> texture(const std::filesystem::path& relative, bool flip) const {
        if (!features_.textures)
            throw std::runtime_error("Texture mode is disabled.");

        const auto full = resolve(relative);
        // The row convention is part of the cache key.
        const auto key = full / (flip ? "#flipped" : "#native");

        if (auto cached = imageCache_[key].lock())
            return cached;

        auto image = std::make_shared<const TextureImage>(detail::decodeImage(full, nullptr, flip));

        imageCache_[key] = image;

        return image;
    }

public:
    explicit AssetLoader(std::filesystem::path projectRoot = detail::executableDirectory(),
                         FeatureOptions features = FeatureOptions{})
        : projectRoot_(std::filesystem::absolute(projectRoot)), features_(features) {
    }

    const FeatureOptions& features() const {
        return features_;
    }

    TextureResult loadTexture(const std::filesystem::path& relative, bool flipRows = true) const {
        try {
            return {texture(relative, flipRows), {}};
        } catch (const std::exception& e) {
            return {nullptr, e.what()};
        }
    }

    ModelResult loadObj(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
    ModelResult loadPrimitive(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
    ModelResult loadModel(const std::filesystem::path& relative,
                          const LoadOptions& options = LoadOptions{}) const; // loadObj() 호환 진입점. OBJ만 지원.
};
} // namespace graphics

namespace graphics {
inline ModelResult AssetLoader::loadModel(const std::filesystem::path& relative,
                                          const LoadOptions& options) const { // loadObj() 호환 진입점. OBJ만 지원.
    return loadObj(relative, options);
}
} // namespace graphics
