# OBJ 최종 실패 콘솔 안내 변경 전후

기준: 9b5ec9e0e950b30af1c4b0f86f9c62b46c3e969a 작업 트리. 같은 코드 쌍은 적용 경로를 모두 명시하고 본문을 한 번씩 전부 표시한다. before는 이번 후속 변경 직전, after는 저장된 최종 내용이다.

단일 단계: 공통 최종 실패 경로에서 서식·직접 파싱 안내 출력, 예제의 중복 출력 제거, API 문서와 배포·구조 분석 갱신. 빌드·실행 테스트는 사용자에게 맡긴다. 실습20 미추적 사용자 폴더는 로컬만 변경한다.

## standard/framework/detail/ObjLoader.inl

최종 실패 시 원인과 OBJ 서식·최소 예제·직접 파싱 방법을 출력한다. 성공한 재시도는 출력하지 않는다.

- `standard/framework/detail/ObjLoader.inl`
- `openGL_3dAssign_20/framework/detail/ObjLoader.inl`
- `openGL_3dAssign_19/framework/detail/ObjLoader.inl`
- `openGL_3dAssign_18/framework/detail/ObjLoader.inl`
- `openGL_3dAssign_17/framework/detail/ObjLoader.inl`
- `openGL_3dAssign_16/framework/detail/ObjLoader.inl`
- `openGL_3dAssign_15/framework/detail/ObjLoader.inl`
- `framework/detail/ObjLoader.inl`

### before

````text
#pragma once
#include <fstream>
#include <sstream>
#include <locale>
#include <iterator>

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
    } catch(const std::exception& e) { result.error=name+": "+e.what(); return result; }
}
} // namespace graphics

````

### after

````text
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

````

## standard/framework/docs/OBJ_LOADING.md

콘솔 출력 계약과 사용자 직접 확인 절차를 설명한다.

- `standard/framework/docs/OBJ_LOADING.md`
- `openGL_3dAssign_20/framework/docs/OBJ_LOADING.md`
- `openGL_3dAssign_19/framework/docs/OBJ_LOADING.md`
- `openGL_3dAssign_18/framework/docs/OBJ_LOADING.md`
- `openGL_3dAssign_17/framework/docs/OBJ_LOADING.md`
- `openGL_3dAssign_16/framework/docs/OBJ_LOADING.md`
- `openGL_3dAssign_15/framework/docs/OBJ_LOADING.md`
- `framework/docs/OBJ_LOADING.md`

### before

````text
# OBJ 텍스트 로드 API

Assimp와 DLL·헤더 종속성을 제거했다. C++17, Windows x64 API다. `loadObj`는 확장자가 `.obj` 또는 `.txt`인 OBJ 텍스트를 같은 방식으로 읽는다. 경로는 실행 파일 디렉터리를 기준으로 한 `projectRoot` 내부 상대경로다. 외부 경로는 거부한다.

```cpp
// Application 초기화 콜백 안에서 사용한다.
graphics::LoadOptions options;
options.pivot = graphics::PivotMode::Preserve;
options.outline = graphics::OutlineMode::FeatureEdges;
const auto result = app.getAssets().loadObj("data/cube.txt", options);
if (!result) {
    std::cerr << result.error << '\n';
    return false;
}
for (const auto& warning : result.warnings) std::cerr << warning << '\n';
return app.getScene().addModel(*result.model, graphics::Transform3D{},
    result.pivotMode, result.localBasis).has_value();
```

`<iostream>`를 포함한다. 파일 읽기는 OpenGL 컨텍스트 없이 `graphics::AssetLoader assets("."); assets.loadObj("data/cube.txt");`로도 호출할 수 있다. 이때 루트는 생성자에 전달한 경로다. `loadPrimitive`와 `loadModel`은 `loadObj`의 호환 함수다. `FeatureOptions.models`와 AssetLoader 생성자의 세 번째 런타임 경로 인자는 제거했다. OBJ 읽기는 항상 가능하며 텍스처만 `config.features.textures=true`로 켠다.

## 입력 → 메시 → 표시

1. 텍스트의 `v/vt/vn` 배열과 `f`의 각 코너를 읽는다. 위치·UV·법선 인덱스는 서로 독립적이다.
2. `f`를 삼각형으로 분할하고 원래 좌표를 보존한다. UV 경계와 법선 경계를 가진 코너를 별도 정점으로 유지한다. 누락된 법선만 면 안에서 계산한다.
3. `ModelResult`의 model·pivotMode·localBasis를 `Scene::addModel`로 전달한다.
4. Renderer가 기존 VAO/VBO/EBO 경로로 업로드한다. 로더는 GPU 자원을 만들지 않는다.

| 처리 | 지원 범위 |
|---|---|
| 면 표기 | `v`, `v/vt`, `v//vn`, `v/vt/vn`; 양수 1기반·음수 상대 인덱스 |
| 정점 | xyz, xyzw(w로 나눔), xyz rgb 확장 |
| UV | u, uv, uv0; 텍스처 표시가 꺼져도 보존 |
| 텍스트 | UTF-8 BOM 유무, LF/CRLF/CR, 주석, 탭·공백, 역슬래시 줄 연결 |
| 면 | 삼각형, 평면 단순 볼록·오목 다각형. 원래 winding 보존 |
| 그룹 | o/g 이름 보존; o/g/usemtl/s 경계 밖으로 면을 병합하지 않음 |
| 일부 속성 누락 | 법선은 누락 코너만 생성; UV가 일부만 있으면 누락 코너를 (0,0)으로 채우고 경고 |
| 공면 병합 | 기본 true. 정점 색이 있으면 색 보존을 위해 병합을 생략하고 경고 |

## 실패 시 다른 읽기 경로

표준 읽기가 `ObjParseError`로 실패하면 파일 전체를 호환 읽기로 다시 파싱한다. 부분 모델은 반환하지 않는다. `allowObjFallback=false`로 재시도를 끌 수 있다.

- UTF-16 LE/BE BOM 텍스트를 UTF-8로 디코딩한다. 잘못된 surrogate·잘린 텍스트는 거부한다.
- 양수 인덱스는 파일 전체의 배열로 해석해 뒤에서 정의된 정점·UV·법선을 허용한다. 음수 인덱스는 해당 면을 만났을 때의 배열 길이를 유지한다.
- 연속 중복 코너와 첫 코너를 끝에 다시 쓴 표기를 제거한다. 위치·UV·법선 인덱스가 모두 같은 코너만 제거한다.

성공하면 `usedFallback=true`와 `warnings`에 재시도 원인·보정 내용을 담는다. 두 읽기가 실패하면 `error`에 파일 경로, 행 번호, 두 실패 원인을 담는다. 파일 열기·텍스처 오류는 기하 파싱을 재시도하지 않는다.

0·범위 밖 인덱스, 비정상 숫자, 퇴화면, 자기 교차, 비평면 다각형은 임의 삼각형 fan이나 면 누락으로 숨기지 않고 오류로 반환한다. 비평면 면은 내보내기 도구에서 먼저 삼각형으로 나눈다. 아주 얇은 면은 부동소수점 허용오차로 거부될 수 있다.

## 재질과 형식의 제한

MTL 파일 자동 읽기, `usemtl` 재질 적용, smoothing group 보간, 선·점·곡면, glTF·FBX·스켈레톤은 지원하지 않는다. `mtllib/usemtl`은 경계를 유지하고 경고한다. `s`는 경계를 유지하며 off/0 이외에는 미보간 경고를 반환한다. 선·점·곡면 등 지원하지 않는 레코드는 파일 전체를 실패시킨다.

텍스처는 `LoadOptions.texturePath` 또는 Scene 재질 API로 명시한다. texturePath가 있으면 텍스처 기능을 켜야 하며 모든 면에 UV가 있어야 한다. 이 제한은 결과 경고를 확인해 판단한다. OBJ 지원은 모든 OBJ 확장 기능 지원을 뜻하지 않는다.

## 사용자가 실행할 확인

빌드·실행·테스트는 에이전트가 수행하지 않았다. Visual Studio C++17·x64에서 `examples/assets.cpp`를 유일한 main으로 등록하고 Framework.props를 적용한다. 실행 파일 아래 `framework/examples/data`에 동봉 데이터를 복사한다.

| 입력 또는 조작 | 예상 결과 |
|---|---|
| `assets.exe obj framework/examples/data/cube.obj` | 큐브, 기본 공면 병합 시 6개 면·12개 삼각형 |
| pyramid.obj / tetrahedron.obj | 원본 data와 같은 형상 |
| corner_formats.obj | 네 표기 모두 로드; `mergeCoplanarFaces=false`면 4개 면·4개 삼각형 |
| forward_closed.obj | 1개 삼각형, usedFallback=true, 보정 경고 |
| 위 파일을 UTF-16 LE/BE BOM으로 저장한 사용자 사본 | UTF-16 디코딩 경고, 같은 기하 |
| allowObjFallback=false + forward_closed.obj | 실패하며 model 없음 |
| 코너 인덱스를 0 또는 999로 수정한 사용자 사본 | 실패하며 행 번호·원인 표시, 부분 모델 없음 |
| 정점 세 개를 일직선으로 만든 사용자 사본 | 퇴화면 오류 |
| 오목한 5각형 / 자기 교차 다각형 | 오목한 평면은 분할, 자기 교차는 오류 |
| 텍스처·클릭·이동·회전 | UV 표시, 기존 피킹·변환이 메시와 일치 |

회상 질문: UV 경계에서는 위치 인덱스가 같아도 왜 서로 다른 렌더링 정점이 필요한가?

````

### after

````text
# OBJ 텍스트 로드 API

Assimp와 DLL·헤더 종속성을 제거했다. C++17, Windows x64 API다. `loadObj`는 확장자가 `.obj` 또는 `.txt`인 OBJ 텍스트를 같은 방식으로 읽는다. 경로는 실행 파일 디렉터리를 기준으로 한 `projectRoot` 내부 상대경로다. 외부 경로는 거부한다.

```cpp
// Application 초기화 콜백 안에서 사용한다.
graphics::LoadOptions options;
options.pivot = graphics::PivotMode::Preserve;
options.outline = graphics::OutlineMode::FeatureEdges;
const auto result = app.getAssets().loadObj("data/cube.txt", options);
if (!result) {
    return false; // loadObj가 오류 원인과 서식 안내를 이미 출력했다.
}
for (const auto& warning : result.warnings) std::cerr << warning << '\n';
return app.getScene().addModel(*result.model, graphics::Transform3D{},
    result.pivotMode, result.localBasis).has_value();
```

`<iostream>`를 포함한다. 파일 읽기는 OpenGL 컨텍스트 없이 `graphics::AssetLoader assets("."); assets.loadObj("data/cube.txt");`로도 호출할 수 있다. 이때 루트는 생성자에 전달한 경로다. `loadPrimitive`와 `loadModel`은 `loadObj`의 호환 함수다. `FeatureOptions.models`와 AssetLoader 생성자의 세 번째 런타임 경로 인자는 제거했다. OBJ 읽기는 항상 가능하며 텍스처만 `config.features.textures=true`로 켠다.

## 입력 → 메시 → 표시

1. 텍스트의 `v/vt/vn` 배열과 `f`의 각 코너를 읽는다. 위치·UV·법선 인덱스는 서로 독립적이다.
2. `f`를 삼각형으로 분할하고 원래 좌표를 보존한다. UV 경계와 법선 경계를 가진 코너를 별도 정점으로 유지한다. 누락된 법선만 면 안에서 계산한다.
3. `ModelResult`의 model·pivotMode·localBasis를 `Scene::addModel`로 전달한다.
4. Renderer가 기존 VAO/VBO/EBO 경로로 업로드한다. 로더는 GPU 자원을 만들지 않는다.

| 처리 | 지원 범위 |
|---|---|
| 면 표기 | `v`, `v/vt`, `v//vn`, `v/vt/vn`; 양수 1기반·음수 상대 인덱스 |
| 정점 | xyz, xyzw(w로 나눔), xyz rgb 확장 |
| UV | u, uv, uv0; 텍스처 표시가 꺼져도 보존 |
| 텍스트 | UTF-8 BOM 유무, LF/CRLF/CR, 주석, 탭·공백, 역슬래시 줄 연결 |
| 면 | 삼각형, 평면 단순 볼록·오목 다각형. 원래 winding 보존 |
| 그룹 | o/g 이름 보존; o/g/usemtl/s 경계 밖으로 면을 병합하지 않음 |
| 일부 속성 누락 | 법선은 누락 코너만 생성; UV가 일부만 있으면 누락 코너를 (0,0)으로 채우고 경고 |
| 공면 병합 | 기본 true. 정점 색이 있으면 색 보존을 위해 병합을 생략하고 경고 |

## 실패 시 다른 읽기 경로

표준 읽기가 `ObjParseError`로 실패하면 파일 전체를 호환 읽기로 다시 파싱한다. 부분 모델은 반환하지 않는다. `allowObjFallback=false`로 재시도를 끌 수 있다.

- UTF-16 LE/BE BOM 텍스트를 UTF-8로 디코딩한다. 잘못된 surrogate·잘린 텍스트는 거부한다.
- 양수 인덱스는 파일 전체의 배열로 해석해 뒤에서 정의된 정점·UV·법선을 허용한다. 음수 인덱스는 해당 면을 만났을 때의 배열 길이를 유지한다.
- 연속 중복 코너와 첫 코너를 끝에 다시 쓴 표기를 제거한다. 위치·UV·법선 인덱스가 모두 같은 코너만 제거한다.

성공하면 `usedFallback=true`와 `warnings`에 재시도 원인·보정 내용을 담는다. 두 읽기가 실패하면 `error`에 파일 경로, 행 번호, 두 실패 원인을 담는다. 파일 열기·텍스처 오류는 기하 파싱을 재시도하지 않는다.

0·범위 밖 인덱스, 비정상 숫자, 퇴화면, 자기 교차, 비평면 다각형은 임의 삼각형 fan이나 면 누락으로 숨기지 않고 오류로 반환한다. 비평면 면은 내보내기 도구에서 먼저 삼각형으로 나눈다. 아주 얇은 면은 부동소수점 허용오차로 거부될 수 있다.

## 재질과 형식의 제한

MTL 파일 자동 읽기, `usemtl` 재질 적용, smoothing group 보간, 선·점·곡면, glTF·FBX·스켈레톤은 지원하지 않는다. `mtllib/usemtl`은 경계를 유지하고 경고한다. `s`는 경계를 유지하며 off/0 이외에는 미보간 경고를 반환한다. 선·점·곡면 등 지원하지 않는 레코드는 파일 전체를 실패시킨다.

텍스처는 `LoadOptions.texturePath` 또는 Scene 재질 API로 명시한다. texturePath가 있으면 텍스처 기능을 켜야 하며 모든 면에 UV가 있어야 한다. 이 제한은 결과 경고를 확인해 판단한다. OBJ 지원은 모든 OBJ 확장 기능 지원을 뜻하지 않는다.

## 사용자가 실행할 확인

빌드·실행·테스트는 에이전트가 수행하지 않았다. Visual Studio C++17·x64에서 `examples/assets.cpp`를 유일한 main으로 등록하고 Framework.props를 적용한다. 실행 파일 아래 `framework/examples/data`에 동봉 데이터를 복사한다.

| 입력 또는 조작 | 예상 결과 |
|---|---|
| `assets.exe obj framework/examples/data/cube.obj` | 큐브, 기본 공면 병합 시 6개 면·12개 삼각형 |
| pyramid.obj / tetrahedron.obj | 원본 data와 같은 형상 |
| corner_formats.obj | 네 표기 모두 로드; `mergeCoplanarFaces=false`면 4개 면·4개 삼각형 |
| forward_closed.obj | 1개 삼각형, usedFallback=true, 보정 경고 |
| 위 파일을 UTF-16 LE/BE BOM으로 저장한 사용자 사본 | UTF-16 디코딩 경고, 같은 기하 |
| allowObjFallback=false + forward_closed.obj | 실패하며 model 없음 |
| 코너 인덱스를 0 또는 999로 수정한 사용자 사본 | 실패하며 행 번호·원인 표시, 부분 모델 없음 |
| 정점 세 개를 일직선으로 만든 사용자 사본 | 퇴화면 오류 |
| 오목한 5각형 / 자기 교차 다각형 | 오목한 평면은 분할, 자기 교차는 오류 |
| 텍스처·클릭·이동·회전 | UV 표시, 기존 피킹·변환이 메시와 일치 |

회상 질문: UV 경계에서는 위치 인덱스가 같아도 왜 서로 다른 렌더링 정점이 필요한가?

## 최종 실패 시 콘솔 안내

표준 읽기와 허용된 호환 읽기가 모두 실패하면 loadObj가 std::cerr에 파일 경로·오류 원인, 지원하는 v/vt/vn/f 서식·코너 표기·인덱스 규칙, 최소 삼각형 예제를 출력한다. allowObjFallback=false이면 표준 읽기의 최종 실패에서 같은 안내를 출력한다. loadPrimitive/loadModel도 공통 경로를 사용한다. 호환 읽기로 성공한 경우에는 이 실패 안내를 출력하지 않는다.

파일 열기·경로·텍스처 오류도 최종 로드 실패이므로 원인을 먼저 확인하라고 출력한다. 이 경우에는 파서부터 수정하지 않는다. 그래도 서식이 맞지 않으면 원본 텍스트를 확인하고 해당 서식의 파서를 직접 작성한다. 파서는 graphics::Mesh.localVertices와 triangleIndices를 채우고 graphics::recalculateNormals(mesh)를 호출한 뒤 graphics::Model.faces에 넣는다. Scene::addModel로 모델을 등록한다. UV·재질이 필요하면 같은 정점 개수에 맞춰 추가한다. 이 안내는 파서를 대신 실행하지 않는다.

ModelResult.error 반환은 유지한다. 콘솔 출력이 필요하므로 이제 로더에 실패 시 출력 부작용이 있다. 호출자는 같은 error를 다시 출력하면 중복된다. 동봉 assets 예제는 중복 출력을 제거했다. 정상 로드와 성공한 호환 읽기의 warnings 반환은 기존과 같다.

사용자가 직접 확인: 없는 경로 또는 f 0 2 3인 사본을 로드하면 최종 실패 안내 한 묶음과 model 없음이 예상된다. forward_closed.obj는 재시도로 성공하므로 최종 실패 안내가 없어야 한다. allowObjFallback=false로 같은 파일을 읽으면 실패 안내가 예상된다. assets 예제에서 오류 원인이 두 번 출력되지 않는지 확인한다. 빌드·실행·테스트는 에이전트가 수행하지 않았다.

````

## standard/framework/examples/assets.cpp

동일한 오류의 중복 콘솔 출력을 제거한다.

- `standard/framework/examples/assets.cpp`
- `openGL_3dAssign_20/framework/examples/assets.cpp`
- `openGL_3dAssign_19/framework/examples/assets.cpp`
- `openGL_3dAssign_18/framework/examples/assets.cpp`
- `openGL_3dAssign_17/framework/examples/assets.cpp`
- `openGL_3dAssign_16/framework/examples/assets.cpp`
- `openGL_3dAssign_15/framework/examples/assets.cpp`
- `framework/examples/assets.cpp`

### before

````text
#include "../Framework.h"
#include <iostream>

// assets.exe obj framework/examples/data/cube.obj
// assets.exe obj models/example.obj textures/override.png
// The old primitive/model mode names remain aliases of obj.
int main(int argc,char** argv) {
    if(argc<3){std::cerr<<"Use: assets.exe obj|primitive|model relative-path [relative-texture-path]\n";return 1;}
    const std::string mode=argv[1];
    if(mode!="obj"&&mode!="primitive"&&mode!="model"){std::cerr<<"Unknown mode\n";return 1;}
    const auto path=std::filesystem::u8path(argv[2]);
    const auto texturePath=argc>3?std::filesystem::u8path(argv[3]):std::filesystem::path{};
    graphics::ApplicationConfig config;
    config.projectRoot=".";
    config.features.textures=!texturePath.empty();
    graphics::Application app;
    app.setInitHandler([&](graphics::Application& app) {
        graphics::LoadOptions options;
        options.outline=graphics::OutlineMode::FeatureEdges;
        options.texturePath=texturePath;
        const auto loaded=app.getAssets().loadObj(path,options);
        if(!loaded){std::cerr<<loaded.error<<'\n';return false;}
        for(const auto& warning:loaded.warnings) std::cerr<<warning<<'\n';
        return app.getScene().addModel(*loaded.model,graphics::Transform3D{},
                                      loaded.pivotMode,loaded.localBasis).has_value();
    });
    app.setMouseHandler([](graphics::Application& app,const graphics::MouseEvent& event) {
        if(event.button==GLFW_MOUSE_BUTTON_LEFT&&event.action==GLFW_PRESS)
            if(const auto hit=app.pickAtCursor())
                std::cout<<"entity="<<hit->entityId<<" mesh="<<hit->faceIndex<<" triangle="<<hit->triangleIndex<<'\n';
    });
    app.setKeyHandler([](graphics::Application& app,int key,int action){
        if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)app.requestClose();
    });
    return app.run(config);
}

````

### after

````text
#include "../Framework.h"
#include <iostream>

// assets.exe obj framework/examples/data/cube.obj
// assets.exe obj models/example.obj textures/override.png
// The old primitive/model mode names remain aliases of obj.
int main(int argc,char** argv) {
    if(argc<3){std::cerr<<"Use: assets.exe obj|primitive|model relative-path [relative-texture-path]\n";return 1;}
    const std::string mode=argv[1];
    if(mode!="obj"&&mode!="primitive"&&mode!="model"){std::cerr<<"Unknown mode\n";return 1;}
    const auto path=std::filesystem::u8path(argv[2]);
    const auto texturePath=argc>3?std::filesystem::u8path(argv[3]):std::filesystem::path{};
    graphics::ApplicationConfig config;
    config.projectRoot=".";
    config.features.textures=!texturePath.empty();
    graphics::Application app;
    app.setInitHandler([&](graphics::Application& app) {
        graphics::LoadOptions options;
        options.outline=graphics::OutlineMode::FeatureEdges;
        options.texturePath=texturePath;
        const auto loaded=app.getAssets().loadObj(path,options);
        if(!loaded) return false; // loadObj already printed the error and format guidance.
        for(const auto& warning:loaded.warnings) std::cerr<<warning<<'\n';
        return app.getScene().addModel(*loaded.model,graphics::Transform3D{},
                                      loaded.pivotMode,loaded.localBasis).has_value();
    });
    app.setMouseHandler([](graphics::Application& app,const graphics::MouseEvent& event) {
        if(event.button==GLFW_MOUSE_BUTTON_LEFT&&event.action==GLFW_PRESS)
            if(const auto hit=app.pickAtCursor())
                std::cout<<"entity="<<hit->entityId<<" mesh="<<hit->faceIndex<<" triangle="<<hit->triangleIndex<<'\n';
    });
    app.setKeyHandler([](graphics::Application& app,int key,int action){
        if(key==GLFW_KEY_ESCAPE&&action==GLFW_PRESS)app.requestClose();
    });
    return app.run(config);
}

````


## docs/obj-loader/ARCHITECTURE.md

실제 소스 행 번호와 그래프의 미해결 간선 수, 실패 콘솔 출력 경계를 갱신한다.

### before

````text
# 현재 실행 구조와 OBJ 변경

2026-10-22 작업 트리의 소스·graphify AST·함수 정의·콜백 등록 앵커를 대조한 정적 설명이다. 빌드·실행 결과가 아니다. 루트 framework와 샘플별 framework의 변경 대상 코드는 같고, 프로젝트 설명 문서는 기존 차이를 유지했다. 실습 20은 사용자 미추적 폴더라 로컬 반영만 했다.

## 독립 실행 파일

| 프로젝트 | main부터 종료까지 |
|---|---|
| standard | main → Console sample 구성 → init/key/mouse 등록 → Application::run. initScene에서 객체·Motion·레이어 구성, onKey에서 상태 변경, onMouse에서 피킹. 공통 run 루프와 shutdown 사용 |
| openGL_sample0 | main → GLFW/GLEW와 셰이더 생성 → loadMeshVertices → MeshParser::parse / MeshValidator::validate → VAO/VBO와 카메라·입력 루프 → glDeleteBuffers/VertexArrays/Program → glfwTerminate. 프레임워크 AssetLoader와 별도 구현 |
| 실습 15 | main → Console practice → init/key/update 등록 → run. initScene/resetAll → beginMotion → onUpdate → applyFaceMotion/setFaceLocalMatrix와 applyObjectTransform |
| 실습 16 | 15와 같은 공통 경로에 beginPyramid/updatePyramid/applyPyramid 추가. 피벗·진행률을 면 행렬로 전달 |
| 실습 17 | main의 init/modified-key/update/focus 등록 → run. reset이 그룹·모델 구성, onKey/onUpdate가 상태 계산, apply가 부모와 자식 변환 반영 |
| 실습 18 | init/modified-key/update/focus 등록 → run. initScene에서 천체·궤도 구성, onUpdate → apply에서 선·천체 변환 반영 |
| 실습 19 | init/modified-key/update/focus 등록 → run. reset/box가 부품 계층 생성, onKey/onUpdate → apply가 탱크 부품·카메라 상태 반영 |
| 실습 20 | 별도 main과 별도 Console. 현재 initScene/reset/box/apply/onKey/onUpdate/onFocus는 탱크 예제 구조를 사용하며, main의 창 제목은 Practice 20이다. 다른 실행 파일과 상태를 공유하지 않음 |
| examples/basic.cpp | 별도 main → Application과 초기 장면·입력 콜백 등록 → 공통 run/shutdown |
| examples/assets.cpp | 별도 main → 인자·텍스처 옵션 → init 콜백에서 loadObj, 경고·오류 출력, addModel → 마우스 피킹·Esc 콜백 → 공통 run/shutdown |
| examples/motion.cpp | 별도 main → Motion·레이어 초기화 콜백 → 공통 run과 Scene의 Motion 갱신 → shutdown |

현재 8개 vcxproj는 각각 독립 실행 프로젝트다. 예제 3개는 별도로 main 하나씩 등록한다. 그래프의 PROJECTS.json은 실제 컴파일 소스와 진입점 위치를 기록한다. 과거 기록의 워밍업·2D·실습 13/14는 현재 작업 트리에서 확인되지 않는다. 서로 다른 main의 호출 흐름을 합치지 않았다.

## 공통 프레임 루프

Framework.h가 Application.h와 include 계층 및 detail inline 구현을 포함한다. Application::run → initialize에서 경로 해석 → GLFW DLL → 창·OpenGL 컨텍스트 → Renderer::initialize → 셰이더·VAO/VBO/EBO → AssetLoader 생성 → GLFW 콜백 등록 → initHandler 순으로 준비한다. 콜백 등록은 본문 실행이 아니다.

PollEvents에서 입력 콜백이 실행된다. 유효한 프레임버퍼에서는 updateHandler → Scene::updateMotions → renderHandler 또는 renderScene → SwapBuffers로 진행한다. 최소화 상태는 WaitEvents를 사용한다. 이벤트 예외는 dispatch가 보관하고 run이 자원을 정리한 뒤 전달한다. 종료 시 shutdown이 AssetLoader, GPU 자원, 창, GLFW를 정리한다.

## OBJ와 CPU → GPU

입력 상대경로 → AssetLoader::resolve의 루트 경계 → 파일 바이트 → objText → parseObj 표준 해석 → ObjParseError일 때 허용된 호환 해석 → 독립 코너 인덱스와 누락 속성 처리 → 정규화된 작업 공간에서 다각형 분할 → 원래 좌표의 Model/Mesh → finishLoad의 검사·피벗 → ModelResult.

호출자는 성공을 확인하고 Scene::addModel에 모델과 pivotMode/localBasis를 전달한다. Scene 변경의 geometryRevision에 따라 Renderer::upload가 GpuVertex(position/color/UV/normal)를 VAO/VBO/EBO에 전달한다. loadObj 자체는 OpenGL 핸들을 생성하지 않는다. UV는 텍스처 표시 설정과 독립적으로 보존한다. texturePath가 있을 때에만 텍스처 기능과 면별 UV를 요구한다.

Renderer::draw는 faceWorldMatrix/worldMatrix 및 MotionSystem::layerMatrices를 사용해 모델 변환과 uMVP/uNormalMatrix/uTint를 설정한다. 로컬 정점 → 모델·부모·면 변환 → view/projection → clip/NDC → viewport → 래스터화 → 프래그먼트 셰이더·깊이 처리 → 화면으로 진행한다. pick/screenRay도 같은 면 월드 변환을 사용한다. 내장 셰이더는 detail/Shaders.h, 선택 외부 셰이더는 shaders/*.glsl에 있다.

## 정의와 호출 위치

| 역할 | 소스 위치 |
|---|---|
| OBJ API 선언 | `framework/include/ModelLoader.h:81` |
| OBJ API 본문 | `framework/detail/ObjLoader.inl:280` |
| OBJ 파싱 | `framework/detail/ObjLoader.inl:109` |
| 문자 인코딩 | `framework/detail/ObjLoader.inl:73` |
| 음수/전방 참조 | `framework/detail/ObjLoader.inl:50` |
| 기존 primitive 호출 | `framework/detail/PrimitiveLoader.inl:3` |
| 기존 model 호출 | `framework/include/ModelLoader.h:90` |
| 소비 예제 | `framework/examples/assets.cpp:21` |
| 실행 초기화 | `framework/detail/Application.inl:39` |
| Scene 등록 | `framework/detail/Scene.inl:568` |

## 그래프 증거와 한계

프레임워크 그래프는 22개 소스, 1,010개 노드·1,524개 간선이다. 전체 그래프는 49개 소스, 1,510개 노드·2,341개 간선이다. SDK·중복 프레임워크·빌드 산출물·임시 백업·배포 ZIP은 제외했다. framework-analysis/graphify-out와 graphify-out에 JSON·HTML·보고서·소스 해시를 갱신했다.

AST의 오버로드·외부 타입·std::function·GLFW 동적 심벌 해석은 완전하지 않다. 프레임워크에는 endpoint 미해결 간선 49개, 전체에는 101개가 있으며 GRAPH_HEALTH.md에 남겼다. 다른 프로그램의 같은 함수 이름으로 연결된 간선은 확정 호출로 보지 않는다. 위의 직접 호출·등록 위치는 소스와 대조했고, 드라이버·DLL 내부와 실제 호출 시점의 실행 결과는 확인하지 않았다.

````

### after

````text
# 현재 실행 구조와 OBJ 변경

2026-10-22 작업 트리의 소스·graphify AST·함수 정의·콜백 등록 앵커를 대조한 정적 설명이다. 빌드·실행 결과가 아니다. 루트 framework와 샘플별 framework의 변경 대상 코드는 같고, 프로젝트 설명 문서는 기존 차이를 유지했다. 실습 20은 사용자 미추적 폴더라 로컬 반영만 했다.

## 독립 실행 파일

| 프로젝트 | main부터 종료까지 |
|---|---|
| standard | main → Console sample 구성 → init/key/mouse 등록 → Application::run. initScene에서 객체·Motion·레이어 구성, onKey에서 상태 변경, onMouse에서 피킹. 공통 run 루프와 shutdown 사용 |
| openGL_sample0 | main → GLFW/GLEW와 셰이더 생성 → loadMeshVertices → MeshParser::parse / MeshValidator::validate → VAO/VBO와 카메라·입력 루프 → glDeleteBuffers/VertexArrays/Program → glfwTerminate. 프레임워크 AssetLoader와 별도 구현 |
| 실습 15 | main → Console practice → init/key/update 등록 → run. initScene/resetAll → beginMotion → onUpdate → applyFaceMotion/setFaceLocalMatrix와 applyObjectTransform |
| 실습 16 | 15와 같은 공통 경로에 beginPyramid/updatePyramid/applyPyramid 추가. 피벗·진행률을 면 행렬로 전달 |
| 실습 17 | main의 init/modified-key/update/focus 등록 → run. reset이 그룹·모델 구성, onKey/onUpdate가 상태 계산, apply가 부모와 자식 변환 반영 |
| 실습 18 | init/modified-key/update/focus 등록 → run. initScene에서 천체·궤도 구성, onUpdate → apply에서 선·천체 변환 반영 |
| 실습 19 | init/modified-key/update/focus 등록 → run. reset/box가 부품 계층 생성, onKey/onUpdate → apply가 탱크 부품·카메라 상태 반영 |
| 실습 20 | 별도 main과 별도 Console. 현재 initScene/reset/box/apply/onKey/onUpdate/onFocus는 탱크 예제 구조를 사용하며, main의 창 제목은 Practice 20이다. 다른 실행 파일과 상태를 공유하지 않음 |
| examples/basic.cpp | 별도 main → Application과 초기 장면·입력 콜백 등록 → 공통 run/shutdown |
| examples/assets.cpp | 별도 main → 인자·텍스처 옵션 → init 콜백에서 loadObj, 경고·오류 출력, addModel → 마우스 피킹·Esc 콜백 → 공통 run/shutdown |
| examples/motion.cpp | 별도 main → Motion·레이어 초기화 콜백 → 공통 run과 Scene의 Motion 갱신 → shutdown |

현재 8개 vcxproj는 각각 독립 실행 프로젝트다. 예제 3개는 별도로 main 하나씩 등록한다. 그래프의 PROJECTS.json은 실제 컴파일 소스와 진입점 위치를 기록한다. 과거 기록의 워밍업·2D·실습 13/14는 현재 작업 트리에서 확인되지 않는다. 서로 다른 main의 호출 흐름을 합치지 않았다.

## 공통 프레임 루프

Framework.h가 Application.h와 include 계층 및 detail inline 구현을 포함한다. Application::run → initialize에서 경로 해석 → GLFW DLL → 창·OpenGL 컨텍스트 → Renderer::initialize → 셰이더·VAO/VBO/EBO → AssetLoader 생성 → GLFW 콜백 등록 → initHandler 순으로 준비한다. 콜백 등록은 본문 실행이 아니다.

PollEvents에서 입력 콜백이 실행된다. 유효한 프레임버퍼에서는 updateHandler → Scene::updateMotions → renderHandler 또는 renderScene → SwapBuffers로 진행한다. 최소화 상태는 WaitEvents를 사용한다. 이벤트 예외는 dispatch가 보관하고 run이 자원을 정리한 뒤 전달한다. 종료 시 shutdown이 AssetLoader, GPU 자원, 창, GLFW를 정리한다.

## OBJ와 CPU → GPU

입력 상대경로 → AssetLoader::resolve의 루트 경계 → 파일 바이트 → objText → parseObj 표준 해석 → ObjParseError일 때 허용된 호환 해석 → 독립 코너 인덱스와 누락 속성 처리 → 정규화된 작업 공간에서 다각형 분할 → 원래 좌표의 Model/Mesh → finishLoad의 검사·피벗 → ModelResult.

호출자는 성공을 확인하고 Scene::addModel에 모델과 pivotMode/localBasis를 전달한다. Scene 변경의 geometryRevision에 따라 Renderer::upload가 GpuVertex(position/color/UV/normal)를 VAO/VBO/EBO에 전달한다. loadObj 자체는 OpenGL 핸들을 생성하지 않는다. UV는 텍스처 표시 설정과 독립적으로 보존한다. texturePath가 있을 때에만 텍스처 기능과 면별 UV를 요구한다.

Renderer::draw는 faceWorldMatrix/worldMatrix 및 MotionSystem::layerMatrices를 사용해 모델 변환과 uMVP/uNormalMatrix/uTint를 설정한다. 로컬 정점 → 모델·부모·면 변환 → view/projection → clip/NDC → viewport → 래스터화 → 프래그먼트 셰이더·깊이 처리 → 화면으로 진행한다. pick/screenRay도 같은 면 월드 변환을 사용한다. 내장 셰이더는 detail/Shaders.h, 선택 외부 셰이더는 shaders/*.glsl에 있다.

## 정의와 호출 위치

| 역할 | 소스 위치 |
|---|---|
| OBJ API 선언 | `framework/include/ModelLoader.h:81` |
| OBJ API 본문 | `framework/detail/ObjLoader.inl:281` |
| OBJ 파싱 | `framework/detail/ObjLoader.inl:110` |
| 문자 인코딩 | `framework/detail/ObjLoader.inl:74` |
| 음수/전방 참조 | `framework/detail/ObjLoader.inl:51` |
| 기존 primitive 호출 | `framework/detail/PrimitiveLoader.inl:3` |
| 기존 model 호출 | `framework/include/ModelLoader.h:90` |
| 소비 예제 | `framework/examples/assets.cpp:21` |
| 실행 초기화 | `framework/detail/Application.inl:39` |
| Scene 등록 | `framework/detail/Scene.inl:568` |

## 그래프 증거와 한계

프레임워크 그래프는 22개 소스, 1,010개 노드·1,524개 간선이다. 전체 그래프는 49개 소스, 1,510개 노드·2,341개 간선이다. SDK·중복 프레임워크·빌드 산출물·임시 백업·배포 ZIP은 제외했다. framework-analysis/graphify-out와 graphify-out에 JSON·HTML·보고서·소스 해시를 갱신했다.

AST의 오버로드·외부 타입·std::function·GLFW 동적 심벌 해석은 완전하지 않다. 프레임워크에는 endpoint 미해결 간선 50개, 전체에는 102개가 있으며 GRAPH_HEALTH.md에 남겼다. 다른 프로그램의 같은 함수 이름으로 연결된 간선은 확정 호출로 보지 않는다. 위의 직접 호출·등록 위치는 소스와 대조했고, 드라이버·DLL 내부와 실제 호출 시점의 실행 결과는 확인하지 않았다.

최종 OBJ 로드 실패는 loadObj의 공통 catch 경계에서 std::cerr로 경로·원인·서식·최소 예제·직접 파싱 방법을 출력한다. 성공한 호환 읽기는 실패 안내를 출력하지 않는다. ModelResult.error 반환은 유지하고 assets 예제는 중복 출력을 생략한다.

````
