#include "../Framework.h"
#include "Internal.h"
namespace graphics {
ModelResult AssetLoader::loadPrimitive(const std::filesystem::path& relative,const LoadOptions& options) const {
    return loadObj(relative,options);
}
} // namespace graphics
