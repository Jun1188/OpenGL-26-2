#include "../Framework.h"
#include "Internal.h"
#include <fstream>
#include <sstream>
#include <locale>
namespace graphics {
namespace detail {
std::size_t objIndex(const std::string& text,std::size_t count) {
    std::size_t consumed=0;
    const auto index=std::stoll(text,&consumed);
    if(consumed!=text.size() || index==0) throw std::runtime_error("Invalid OBJ index.");
    const auto resolved=index>0?index-1:static_cast<long long>(count)+index;
    if(resolved<0 || static_cast<unsigned long long>(resolved)>=count)
        throw std::runtime_error("OBJ index is out of range.");
    return std::size_t(resolved);
}
}
ModelResult AssetLoader::loadPrimitive(const std::filesystem::path& relative,const LoadOptions& options) const {
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
