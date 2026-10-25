#pragma once
#include <fstream>
#include <sstream>
#include <locale>
#include <iterator>
#include <iostream>

namespace graphics::detail {
struct ObjParseError : std::runtime_error { using std::runtime_error::runtime_error; };
struct ObjCorner {
    long long position=0, uv=0, normal=0;
    bool operator==(const ObjCorner& b) const {
        return position==b.position && uv==b.uv && normal==b.normal;
    }
};
struct ObjFace {
    std::vector<ObjCorner> corners;
    std::size_t positions=0, uvs=0, normals=0, line=0, batch=0;
    std::string name;
};
inline void objWarning(std::vector<std::string>& warnings,const std::string& message) {
    if(std::find(warnings.begin(),warnings.end(),message)==warnings.end()) warnings.push_back(message);
}
inline long long objInteger(const std::string& text) {
    const auto start=(!text.empty() && (text[0]=='-' || text[0]=='+'))?1u:0u;
    if(start==text.size() || text.find_first_not_of("0123456789",start)!=std::string::npos)
        throw ObjParseError("Invalid OBJ index: "+text);
    try {
        const auto value=std::stoll(text);
        if(value==0) throw ObjParseError("OBJ indices start at 1; zero is invalid.");
        return value;
    } catch(const std::out_of_range&) { throw ObjParseError("OBJ index is too large."); }
}
inline ObjCorner objCorner(const std::string& text) {
    ObjCorner result;
    const auto a=text.find('/'), b=a==std::string::npos?a:text.find('/',a+1);
    if(b!=std::string::npos && text.find('/',b+1)!=std::string::npos)
        throw ObjParseError("Too many fields in OBJ corner: "+text);
    result.position=objInteger(text.substr(0,a));
    if(a!=std::string::npos) {
        const auto part=text.substr(a+1,b==std::string::npos?b:b-a-1);
        if(!part.empty()) result.uv=objInteger(part);
        else if(b==std::string::npos) throw ObjParseError("Missing UV after '/'.");
    }
    if(b!=std::string::npos) {
        if(b+1==text.size()) throw ObjParseError("Missing normal after '//'.");
        result.normal=objInteger(text.substr(b+1));
    }
    return result;
}
inline std::size_t objResolve(long long value,std::size_t atFace,std::size_t total,bool compatible) {
    // Negative references use counts at the face, even during the second pass.
    if(value<0) {
        if(value < -static_cast<long long>(atFace)) throw ObjParseError("Negative OBJ index is out of range.");
        return std::size_t(static_cast<long long>(atFace)+value);
    }
    if(static_cast<unsigned long long>(value)>(compatible?total:atFace))
        throw ObjParseError("Positive OBJ index is out of range or references a later record.");
    return std::size_t(value-1);
}
inline std::vector<float> objNumbers(std::istringstream& row) {
    std::vector<float> result;
    std::string token;
    while(row>>token) {
        std::istringstream number(token); number.imbue(std::locale::classic());
        double value=0; char extra=0;
        if(!(number>>value) || (number>>extra) || !std::isfinite(value)
            || std::abs(value)>std::numeric_limits<float>::max())
            throw ObjParseError("Invalid finite OBJ number: "+token);
        result.push_back(float(value));
    }
    return result;
}
inline std::string objText(const std::string& bytes,bool compatible,std::vector<std::string>& warnings) {
    std::string text=bytes;
    if(bytes.size()>=2 && ((static_cast<unsigned char>(bytes[0])==0xff && static_cast<unsigned char>(bytes[1])==0xfe)
        || (static_cast<unsigned char>(bytes[0])==0xfe && static_cast<unsigned char>(bytes[1])==0xff))) {
        if(!compatible) throw ObjParseError("UTF-16 BOM requires the compatible text reader.");
        if(bytes.size()%2) throw ObjParseError("Truncated UTF-16 text.");
        const bool little=static_cast<unsigned char>(bytes[0])==0xff;
        auto unit=[&](std::size_t i) {
            const auto a=static_cast<unsigned char>(bytes[i]),b=static_cast<unsigned char>(bytes[i+1]);
            return unsigned(little?(a|(b<<8)):((a<<8)|b));
        };
        text.clear();
        for(std::size_t i=2;i<bytes.size();i+=2) {
            unsigned c=unit(i);
            if(c>=0xd800 && c<=0xdbff) {
                if(i+3>=bytes.size()) throw ObjParseError("Truncated UTF-16 surrogate pair.");
                const auto low=unit(i+2);
                if(low<0xdc00 || low>0xdfff) throw ObjParseError("Invalid UTF-16 surrogate pair.");
                c=0x10000+((c-0xd800)<<10)+(low-0xdc00); i+=2;
            } else if(c>=0xdc00 && c<=0xdfff) throw ObjParseError("Unpaired UTF-16 surrogate.");
            if(c<0x80) text+=char(c);
            else if(c<0x800) { text+=char(0xc0|(c>>6)); text+=char(0x80|(c&63)); }
            else if(c<0x10000) { text+=char(0xe0|(c>>12)); text+=char(0x80|((c>>6)&63)); text+=char(0x80|(c&63)); }
            else { text+=char(0xf0|(c>>18)); text+=char(0x80|((c>>12)&63)); text+=char(0x80|((c>>6)&63)); text+=char(0x80|(c&63)); }
        }
        objWarning(warnings,"Decoded UTF-16 BOM text as UTF-8.");
    }
    if(text.compare(0,3,"\xEF\xBB\xBF")==0) text.erase(0,3);
    if(text.find('\0')!=std::string::npos) throw ObjParseError("NUL byte in OBJ text; unsupported encoding or binary file.");
    std::string lines;
    for(std::size_t i=0;i<text.size();++i) {
        if(text[i]=='\r') { lines+='\n'; if(i+1<text.size() && text[i+1]=='\n') ++i; }
        else lines+=text[i];
    }
    return lines;
}
inline Model parseObj(const std::string& bytes,const std::string& name,const LoadOptions& options,
                      bool compatible,std::vector<std::string>& warnings) {
    std::istringstream input(objText(bytes,compatible,warnings));
    std::vector<glm::vec3> positions,normals;
    std::vector<glm::vec2> uvs;
    std::vector<GlColor> colors;
    std::vector<ObjFace> faces;
    bool hasColors=false;
    std::string physical,line,group="face";
    std::size_t lineNumber=0,start=0,batch=0;
    while(std::getline(input,physical)) {
        ++lineNumber;
        if(line.empty()) start=lineNumber;
        physical=physical.substr(0,physical.find('#'));
        const auto end=physical.find_last_not_of(" \t");
        const bool continued=end!=std::string::npos && physical[end]=='\\';
        line+=continued?physical.substr(0,end):physical;
        if(continued) { line+=' '; continue; }
        std::istringstream row(line); row.imbue(std::locale::classic());
        std::string kind; row>>kind;
        try {
            if(kind=="v") {
                const auto values=objNumbers(row);
                if(values.size()!=3 && values.size()!=4 && values.size()!=6)
                    throw ObjParseError("Expected v x y z [w] or v x y z r g b.");
                glm::vec3 p(values[0],values[1],values[2]);
                if(values.size()==4) {
                    if(values[3]==0) throw ObjParseError("Homogeneous vertex has zero w.");
                    p/=values[3];
                }
                if(!finiteVector(p)) throw ObjParseError("Vertex overflows after division by w.");
                positions.push_back(p);
                colors.push_back(values.size()==6?GlColor{values[3],values[4],values[5]}:GlColor{});
                hasColors=hasColors || values.size()==6;
            } else if(kind=="vt") {
                const auto values=objNumbers(row);
                if(values.empty() || values.size()>3 || (values.size()==3 && values[2]!=0))
                    throw ObjParseError("Expected vt u [v [0]]; volume texture coordinates are unsupported.");
                uvs.push_back({values[0],values.size()>1?values[1]:0});
            } else if(kind=="vn") {
                const auto values=objNumbers(row);
                if(values.size()!=3) throw ObjParseError("Expected vn x y z.");
                const glm::dvec3 normal(values[0],values[1],values[2]);
                if(glm::length(normal)==0) throw ObjParseError("Zero OBJ normal.");
                normals.push_back(glm::vec3(glm::normalize(normal)));
            } else if(kind=="f") {
                ObjFace face; face.positions=positions.size(); face.uvs=uvs.size(); face.normals=normals.size();
                face.line=start; face.batch=batch; face.name=group;
                std::string token;
                while(row>>token) face.corners.push_back(objCorner(token));
                if(face.corners.size()<3) throw ObjParseError("Face has fewer than three corners.");
                faces.push_back(std::move(face));
            } else if(kind=="o" || kind=="g") {
                std::getline(row,group); const auto first=group.find_first_not_of(" \t");
                group=first==std::string::npos?"face":group.substr(first); ++batch;
            } else if(kind=="mtllib" || kind=="usemtl") {
                ++batch;
                objWarning(warnings,"MTL materials are not loaded; set LoadOptions.texturePath or a Scene material explicitly.");
            } else if(kind=="s") {
                ++batch;
                std::string smoothing; row>>smoothing;
                if(smoothing!="off" && smoothing!="0")
                    objWarning(warnings,"Smoothing groups are not interpolated; supplied vn values are preserved and missing normals are generated per face.");
            } else if(!kind.empty()) throw ObjParseError("Unsupported OBJ record: "+kind);
        } catch(const ObjParseError& e) { throw ObjParseError("line "+std::to_string(start)+": "+e.what()); }
        line.clear();
    }
    if(!line.empty()) throw ObjParseError("line "+std::to_string(start)+": unterminated line continuation.");
    Model model; model.shape=GLShape::Custom; model.name=name;
    Model segment; std::size_t activeBatch=0;
    auto flush=[&] {
        if(options.mergeCoplanarFaces && !hasColors) mergeCoplanar(segment);
        for(auto& face:segment.faces) model.faces.push_back(std::move(face));
        segment.faces.clear();
    };
    for(auto& face:faces) {
        try {
            if(face.batch!=activeBatch) { flush(); activeBatch=face.batch; }
            if(compatible) {
                const auto old=face.corners.size();
                face.corners.erase(std::unique(face.corners.begin(),face.corners.end()),face.corners.end());
                if(face.corners.size()>1 && face.corners.front()==face.corners.back()) face.corners.pop_back();
                if(face.corners.size()!=old) objWarning(warnings,"Removed repeated identical OBJ corners at line "+std::to_string(face.line)+".");
            }
            Mesh mesh; mesh.name=face.name+"_"+std::to_string(face.line);
            std::size_t uvCount=0;
            bool forward=false;
            for(const auto& corner:face.corners) {
                const auto p=objResolve(corner.position,face.positions,positions.size(),compatible);
                mesh.localVertices.push_back(positions[p]);
                if(hasColors) mesh.vertexColors.push_back(colors[p]);
                forward=forward || (corner.position>0 && static_cast<unsigned long long>(corner.position)>face.positions);
                glm::vec2 uv(0);
                if(corner.uv) {
                    uv=uvs[objResolve(corner.uv,face.uvs,uvs.size(),compatible)]; ++uvCount;
                    forward=forward || (corner.uv>0 && static_cast<unsigned long long>(corner.uv)>face.uvs);
                }
                mesh.texcoords.push_back(uv);
            }
            // Normalize only the triangulation workspace; retain original coordinates.
            if(mesh.localVertices.size()<3) throw ObjParseError("Face has fewer than three distinct corners.");
            glm::dvec3 low(mesh.localVertices[0]),high(low);
            for(const auto& p:mesh.localVertices) { low=glm::min(low,glm::dvec3(p)); high=glm::max(high,glm::dvec3(p)); }
            const auto size=high-low; const double scale=std::max({size.x,size.y,size.z});
            if(scale==0) throw ObjParseError("Degenerate polygon.");
            Mesh workspace;
            for(const auto& p:mesh.localVertices) workspace.localVertices.push_back(glm::vec3((glm::dvec3(p)-low)/scale));
            if(workspace.localVertices.size()>3) {
                glm::dvec3 plane(0);
                const auto count=workspace.localVertices.size();
                for(std::size_t i=0;i<count;++i) {
                    const auto a=glm::dvec3(workspace.localVertices[i]);
                    const auto b=glm::dvec3(workspace.localVertices[(i+1)%count]);
                    plane+=glm::cross(a,b);
                }
                if(glm::length(plane)<1e-12) throw ObjParseError("Degenerate polygon plane.");
                plane=glm::normalize(plane);
                const auto absolute=glm::abs(plane);
                const int axis=absolute.x>absolute.y?(absolute.x>absolute.z?0:2):(absolute.y>absolute.z?1:2);
                std::vector<glm::dvec2> projected;
                for(const auto& p:workspace.localVertices) {
                    if(std::abs(glm::dot(plane,glm::dvec3(p-workspace.localVertices[0])))>1e-5)
                        throw ObjParseError("Non-planar OBJ polygon; export triangulated faces.");
                    projected.push_back(axis==0?glm::dvec2(p.y,p.z):axis==1?glm::dvec2(p.x,p.z):glm::dvec2(p.x,p.y));
                }
                auto cross=[](glm::dvec2 a,glm::dvec2 b,glm::dvec2 c) {
                    return (b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x);
                };
                auto on=[](glm::dvec2 a,glm::dvec2 b,glm::dvec2 p) {
                    return p.x>=std::min(a.x,b.x)-1e-14 && p.x<=std::max(a.x,b.x)+1e-14
                        && p.y>=std::min(a.y,b.y)-1e-14 && p.y<=std::max(a.y,b.y)+1e-14;
                };
                for(std::size_t i=0;i<count;++i) for(std::size_t j=i+1;j<count;++j) {
                    if((i+1)%count==j || (j+1)%count==i) continue;
                    const auto a=projected[i],b=projected[(i+1)%count],c=projected[j],d=projected[(j+1)%count];
                    const auto abC=cross(a,b,c),abD=cross(a,b,d),cdA=cross(c,d,a),cdB=cross(c,d,b);
                    const bool crossing=((abC>1e-14 && abD< -1e-14)||(abC< -1e-14 && abD>1e-14))
                        && ((cdA>1e-14 && cdB< -1e-14)||(cdA< -1e-14 && cdB>1e-14));
                    if(crossing || (std::abs(abC)<=1e-14 && on(a,b,c)) || (std::abs(abD)<=1e-14 && on(a,b,d))
                        || (std::abs(cdA)<=1e-14 && on(c,d,a)) || (std::abs(cdB)<=1e-14 && on(c,d,b)))
                        throw ObjParseError("Self-intersection or repeated corner in OBJ polygon.");
                }
            }
            try { mesh.triangleIndices=triangulate(workspace.localVertices); }
            catch(const std::runtime_error& e) { throw ObjParseError(e.what()); }
            workspace.triangleIndices=mesh.triangleIndices;
            recalculateNormals(workspace);
            mesh.vertexNormals=std::move(workspace.vertexNormals);
            for(std::size_t i=0;i<face.corners.size();++i) if(face.corners[i].normal) {
                const auto value=face.corners[i].normal;
                mesh.vertexNormals[i]=normals[objResolve(value,face.normals,normals.size(),compatible)];
                forward=forward || (value>0 && static_cast<unsigned long long>(value)>face.normals);
            }
            if(forward) objWarning(warnings,"Resolved positive forward references at line "+std::to_string(face.line)+".");
            if(uvCount==0) mesh.texcoords.clear();
            else if(uvCount!=face.corners.size()) objWarning(warnings,"Filled missing UV corners with (0,0) at line "+std::to_string(face.line)+".");
            const auto& indices=mesh.triangleIndices;
            mesh.normal=glm::normalize(glm::cross(workspace.localVertices[indices[1]]-workspace.localVertices[indices[0]],
                                                  workspace.localVertices[indices[2]]-workspace.localVertices[indices[0]]));
            if(!validMesh(mesh)) throw ObjParseError("Invalid or degenerate OBJ mesh.");
            segment.faces.push_back(std::move(mesh));
        } catch(const ObjParseError& e) { throw ObjParseError("line "+std::to_string(face.line)+": "+e.what()); }
    }
    flush();
    if(options.mergeCoplanarFaces && hasColors) objWarning(warnings,"Coplanar merging is skipped to preserve vertex colors.");
    if(model.faces.empty()) throw ObjParseError("OBJ contains no polygon faces.");
    return model;
}
} // namespace graphics::detail

namespace graphics {
inline ModelResult AssetLoader::loadObj(const std::filesystem::path& relative,const LoadOptions& options) const {
    ModelResult result;
    const auto utf8=relative.u8string();
    const std::string name(utf8.begin(),utf8.end());
    try {
        if(!options.texturePath.empty() && !features_.textures)
            throw std::runtime_error("Enable texture mode before specifying texturePath.");
        std::ifstream input(resolve(relative),std::ios::binary);
        if(!input) throw std::runtime_error("Cannot open OBJ text.");
        const std::string bytes((std::istreambuf_iterator<char>(input)),std::istreambuf_iterator<char>());
        if(input.bad()) throw std::runtime_error("OBJ file read failed.");
        Model model;
        try { model=detail::parseObj(bytes,name,options,false,result.warnings); }
        catch(const detail::ObjParseError& first) {
            if(!options.allowObjFallback) throw;
            result.warnings.clear();
            try { model=detail::parseObj(bytes,name,options,true,result.warnings); }
            catch(const detail::ObjParseError& second) {
                throw std::runtime_error(std::string("Standard reader: ")+first.what()+"; compatible reader: "+second.what());
            }
            result.usedFallback=true;
            detail::objWarning(result.warnings,std::string("Compatible OBJ reader used after: ")+first.what());
        }
        if(!options.texturePath.empty()) {
            auto image=texture(options.texturePath,options.flipTextureRows);
            for(auto& mesh:model.faces) {
                if(mesh.texcoords.empty()) throw std::runtime_error("Explicit texture requires UV coordinates on every face.");
                mesh.material.texture=image; mesh.material.useTexture=true;
            }
        }
        auto finished=detail::finishLoad(std::move(model),options);
        finished.warnings=std::move(result.warnings); finished.usedFallback=result.usedFallback;
        return finished;
    } catch(const std::exception& e) {
        result.error=name+": "+e.what();
        std::cerr << "[OBJ load failed] " << result.error << '\n'
            << "Check the file path and the reported cause first, including texture settings.\n"
            << "Supported OBJ text format (.obj or .txt), one record per line:\n"
            << "  v x y z       : vertex position\n"
            << "  vt u [v]      : optional texture coordinate\n"
            << "  vn x y z      : optional nonzero normal\n"
            << "  f 1 2 3       : face using 1-based vertex indices\n"
            << "  Corner forms: v, v/vt, v//vn, v/vt/vn; negative relative indices are allowed.\n"
            << "Minimal triangle example:\n"
            << "  v 0 0 0\n  v 1 0 0\n  v 0 1 0\n  f 1 2 3\n"
            << "If parsing still fails, inspect the model's source format and write a parser for it.\n"
            << "Fill graphics::Mesh.localVertices and triangleIndices, then call\n"
            << "graphics::recalculateNormals(mesh), add the mesh to graphics::Model.faces,\n"
            << "and pass that model to Scene::addModel. See framework/docs/OBJ_LOADING.md.\n";
        return result;
    }
}
} // namespace graphics
