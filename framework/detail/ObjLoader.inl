#pragma once
namespace graphics {
inline ModelResult AssetLoader::loadObj(const std::filesystem::path& relative,const LoadOptions& options) const {
    return loadPrimitive(relative,options);
}
} // namespace graphics
