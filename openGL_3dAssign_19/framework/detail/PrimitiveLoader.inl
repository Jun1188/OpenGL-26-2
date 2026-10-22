#pragma once
namespace graphics {
inline ModelResult AssetLoader::loadPrimitive(const std::filesystem::path& relative,const LoadOptions& options) const {
    return loadObj(relative,options);
}
} // namespace graphics
