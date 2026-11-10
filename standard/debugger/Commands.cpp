#include "SourceIndex.h"
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <limits>
#include <regex>
#include <sstream>
#include <stdexcept>

namespace source_debugger {
namespace {
using Words = std::vector<std::string>;
void require(bool ok, const std::string& message) { if (!ok) throw std::runtime_error(message); }
Words words(const std::string& line) {
    Words result; std::string current; bool quoted = false, started = false;
    require(line.size() <= 16384, "명령은 16384바이트 이하로 입력하세요.");
    for (std::size_t i = 0; i < line.size(); ++i) {
        const char c = line[i];
        if (c == '"') { quoted = !quoted; started = true; }
        else if (quoted && c == '\\' && i + 1 < line.size() && line[i + 1] == '"') { current += '"'; ++i; }
        else if (!quoted && (c == ' ' || c == '\t')) {
            if (started) { result.push_back(current); current.clear(); started = false; }
        } else { current += c; started = true; }
    }
    require(!quoted, "큰따옴표를 닫으세요.");
    if (started) result.push_back(current);
    return result;
}
std::size_t index(const std::string& text) {
    require(!text.empty() && text.find_first_not_of("0123456789") == std::string::npos, "행 번호는 양의 정수입니다.");
    std::size_t used = 0; const auto n = std::stoull(text, &used);
    require(used == text.size() && n > 0 && n <= std::numeric_limits<std::size_t>::max(), "유효하지 않은 행 번호입니다.");
    return static_cast<std::size_t>(n);
}
bool identifier(const std::string& name) {
    return std::regex_match(name, std::regex("[A-Za-z_][A-Za-z_0-9]{0,63}"));
}
std::string number(const std::string& text, double low, double high, bool nonzero = false) {
    std::size_t used = 0; const double value = std::stod(text, &used);
    require(used == text.size() && std::isfinite(value) && value >= low && value <= high && (!nonzero || value != 0),
        "값 범위 또는 유한성 조건을 확인하세요.");
    const auto converted = static_cast<float>(value);
    require(std::isfinite(converted) && (!nonzero || converted != 0), "float 변환 후 값이 유효하지 않습니다.");
    std::ostringstream out; out << std::setprecision(9) << converted;
    auto result = out.str(); if (result.find_first_of(".eE") == std::string::npos) result += '.';
    return result + 'f';
}
struct Field { std::string member, meaning; int count; double low, high; bool nonzero = false; };
Field field(const std::string& kind, const std::string& property) {
    const double max = std::numeric_limits<float>::max();
    if (kind == "object") {
        if (property == "position") return {property, "객체 월드 배치 XYZ", 3, -max, max};
        if (property == "rotation") return {"rotationDegrees", "월드 기준 Euler XYZ(도), 행렬 순서 Rz Ry Rx", 3, -max, max};
        if (property == "size") return {property, "배율 적용 전 모델 크기 XYZ, 각 값 > 0", 3, 0, max, true};
        if (property == "scale") return {property, "객체 축별 배율 XYZ, 0 제외", 3, -max, max, true};
        if (property == "pivot") return {property, "모델 지역 피벗 XYZ; 위치를 자동 보존하지 않음", 3, -max, max};
        if (property == "color") return {"material.color", "물체 RGB 0~1", 3, 0, 1};
        if (property == "ambient") return {"material.ambientStrength", "주변광 재질 계수 Ka", 1, 0, 1};
        if (property == "diffuse") return {"material.diffuseStrength", "확산광 재질 계수 Kd", 1, 0, 1};
        if (property == "specular") return {"material.specularStrength", "정반사 재질 계수 Ks", 1, 0, 1};
        if (property == "shininess") return {"material.shininess", "정반사 지수, 양수", 1, 0, max, true};
        if (property == "unlit") return {"material.unlit", "재질 조명 계산 생략 on/off", 0, 0, 0};
        if (property == "shape") return {property, "형상 box/tetra/pyramid", -1, 0, 0};
    }
    if (kind == "layer") {
        if (property == "translation" || property == "pivot") return {property, "선행 레이어 기준 지역 XYZ", 3, -max, max};
        if (property == "rotation") return {"rotationDegrees", "지역 Euler XYZ(도), 행렬 순서 Rz Ry Rx", 3, -max, max};
        if (property == "scale") return {property, "지역 축별 배율 XYZ, 0 제외", 3, -max, max, true};
        if (property == "axis") return {"spinAxis", "지역 회전축 XYZ, 영벡터 제외", 3, -max, max};
        if (property == "spin") return {"spinDegreesPerSecond", "초당 회전각(도/초), 0은 정지", 1, -max, max};
    }
    if (kind == "light") {
        if (property == "position") return {property, "광원 XYZ; parent가 있으면 부모 지역 좌표", 3, -max, max};
        if (property == "color") return {property, "광원 RGB 0~1", 3, 0, 1};
        if (property == "intensity" || property == "attenuation") return {property, "광원 강도 / 거리 제곱 감쇠 계수, 0 이상", 1, 0, max};
    }
    if (kind == "camera") {
        if (property == "position" || property == "target" || property == "up") return {property, "카메라 XYZ; parent가 있으면 부모 지역 좌표", 3, -max, max};
        if (property == "fov") return {"fieldOfViewDegrees", "수직 화각(도), 0 < FOV < 180", 1, 0.001, 179.999};
        if (property == "near") return {"nearPlane", "근평면 거리, far보다 작은 양수", 1, 0, max, true};
        if (property == "far") return {"farPlane", "원평면 거리, near보다 큰 양수", 1, 0, max, true};
        if (property == "extent") return {"orthographicHalfExtent", "직교 투영 짧은 화면 축의 반길이", 1, 0, max, true};
        if (property == "ortho") return {"orthographic", "직교 투영 on / 원근 투영 off", 0, 0, 0};
    }
    if (kind == "render") {
        if (property == "lighting" || property == "textures") return {property, "조명 / 텍스처 사용 on/off", 0, 0, 0};
        if (property == "ambient") return {"ambientColor", "독립 주변광 RGB", 3, 0, 1};
        if (property == "sun-direction") return {"lightDirection", "표면에서 방향광을 향하는 월드 방향 XYZ", 3, -max, max};
        if (property == "sun-color") return {"lightColor", "방향광 RGB", 3, 0, 1};
        if (property == "sun-intensity") return {"directionalIntensity", "방향광 강도, 0이면 끄기", 1, 0, max};
        if (property == "background") return {property, "배경 RGBA 0~1", 4, 0, 1};
    }
    throw std::runtime_error("지원하지 않는 속성입니다. help에서 명령 목록을 확인하세요.");
}
std::string escaped(const std::string& value) {
    std::string result; for (const auto c : value) { if (c == '.') result += '\\'; result += c; } return result;
}
void set(SourceIndex& sources, const Words& a, std::ostream& out) {
    require(a.size() >= 4, "<object/layer/light/camera/render> 이름 속성 값...");
    const auto& block = sources.block(a[0], a[1]); const auto& file = sources.files[block.file];
    const auto spec = field(a[0], a[2]);
    require(a.size() == std::size_t(3 + (spec.count <= 0 ? 1 : spec.count)), "속성의 인수 개수가 맞지 않습니다.");
    std::string value;
    if (spec.count == 0) {
        require(a[3] == "on" || a[3] == "off", "on 또는 off를 입력하세요."); value = a[3] == "on" ? "true" : "false";
    } else if (spec.count == -1) {
        require(a[3] == "box" || a[3] == "tetra" || a[3] == "pyramid", "box/tetra/pyramid만 지원합니다.");
        value = "graphics::GLShape::" + std::string(a[3] == "box" ? "Cube" : a[3] == "tetra" ? "Tetra" : "Pyramid");
    } else {
        if (spec.count > 1) value = "{";
        bool any = false;
        for (int i = 0; i < spec.count; ++i) {
            if (i) value += ", "; value += number(a[std::size_t(i) + 3], spec.low, spec.high, spec.nonzero);
            any = any || std::stod(a[std::size_t(i) + 3]) != 0;
        }
        if (a[2] == "axis" || a[2] == "sun-direction" || (a[0] == "camera" && a[2] == "up")) require(any, "방향 벡터는 0이 될 수 없습니다.");
        if (spec.count > 1) value += '}';
    }
    const std::regex assignment("^(\\s*)([A-Za-z_][A-Za-z_0-9]*)\\." + escaped(spec.member) + "\\s*=[^;]*;\\s*(?://.*)?$");
    std::size_t line = 0; std::string replacement; bool found = false;
    for (auto i = block.first + 1; i < block.last; ++i) {
        std::smatch match;
        if (!std::regex_match(file.lines[i], match, assignment)) continue;
        require(!found, "같은 속성의 대입이 여러 번 있습니다. code/replace로 직접 범위를 지정하세요.");
        found = true; line = i;
        replacement = match[1].str() + match[2].str() + '.' + spec.member + " = " + value + ';';
    }
    require(found, "단일 행 속성 대입을 찾지 못했습니다. show/find 후 insert/replace로 수정안을 작성하세요.");
    std::string meaning = a[0] + ' ' + a[1] + " / " + a[2] + " — " + spec.meaning;
    if (a[0] == "camera") meaning += "; 위치/시선/up/near/far의 상호 유효성은 사용자 빌드·실행에서 확인";
    proposal(file, line + 1, line + 1, {replacement}, meaning, out);
}
Words splitCode(std::string text) {
    std::string decoded;
    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\\' && i + 1 < text.size() && text[i + 1] == 'n') { decoded += '\n'; ++i; }
        else decoded += text[i];
    }
    Words lines; std::istringstream input(decoded); std::string line;
    while (std::getline(input, line)) lines.push_back(line);
    return lines;
}
void add(SourceIndex& sources, const Words& a, std::ostream& out) {
    require(a.size() >= 3 && identifier(a[2]), "add object/layer/light/camera 이름 ...; 이름은 ASCII 식별자입니다.");
    const auto& kind = a[1]; const auto& name = a[2];
    for (const auto& block : sources.blocks) require(block.kind != kind || block.name != name, "같은 구획이 이미 있습니다.");
    const auto var = "cfg_" + kind + '_' + name;
    Words code{"    // @debug begin " + kind + ' ' + name};
    auto line = [&](const std::string& member, const std::string& value) { code.push_back("    " + var + '.' + member + " = " + value + ';'); };
    std::string call;
    if (kind == "object") {
        require(a.size() == 4 || a.size() == 5, "add object 이름 box/tetra/pyramid [부모]");
        require(a[3] == "box" || a[3] == "tetra" || a[3] == "pyramid", "box/tetra/pyramid만 지원합니다.");
        code.push_back("    graphics::ObjectSetup " + var + ';');
        line("shape", "graphics::GLShape::" + std::string(a[3] == "box" ? "Cube" : a[3] == "tetra" ? "Tetra" : "Pyramid"));
        line("size", "{1.f, 1.f, 1.f}"); line("position", "{0.f, 0.f, 0.f}"); line("rotationDegrees", "{0.f, 0.f, 0.f}");
        line("scale", "{1.f, 1.f, 1.f}"); line("pivot", "{0.f, 0.f, 0.f}");
        line("material.color", "{1.f, 1.f, 1.f}"); line("material.ambientStrength", "0.25f");
        line("material.diffuseStrength", "1.f"); line("material.specularStrength", "0.4f");
        line("material.shininess", "32.f"); line("material.unlit", "false");
        call = "scene.addObject(\"" + name + "\", " + var;
    } else if (kind == "layer") {
        require(a.size() == 4, "add layer 레이어이름 객체이름"); sources.block("object", a[3]);
        code.push_back("    graphics::LayerSetup " + var + ';');
        line("translation", "{0.f, 0.f, 0.f}"); line("rotationDegrees", "{0.f, 0.f, 0.f}");
        line("scale", "{1.f, 1.f, 1.f}"); line("pivot", "{0.f, 0.f, 0.f}");
        line("spinAxis", "{0.f, 1.f, 0.f}"); line("spinDegreesPerSecond", "0.f");
        call = "scene.addLayer(\"" + a[3] + "\", \"" + name + "\", " + var;
    } else if (kind == "light") {
        require(a.size() == 3 || a.size() == 4, "add light 이름 [부모]");
        code.push_back("    graphics::PointLight " + var + ';');
        line("position", "{2.f, 4.f, 3.f}"); line("color", "{1.f, 1.f, 1.f}");
        line("intensity", "1.f"); line("attenuation", "0.02f");
        call = "scene.addLight(\"" + name + "\", " + var;
    } else if (kind == "camera") {
        require(a.size() == 3 || a.size() == 4, "add camera 이름 [부모]");
        for (const auto& block : sources.blocks) require(block.kind != "camera", "카메라는 한 개의 활성 구획을 사용합니다. 기존 camera를 수정하세요.");
        code.push_back("    graphics::Camera " + var + ';');
        line("position", "{6.f, 4.f, 8.f}"); line("target", "{0.f, 0.f, 0.f}"); line("up", "{0.f, 1.f, 0.f}");
        line("fieldOfViewDegrees", "45.f"); line("nearPlane", "0.1f"); line("farPlane", "100.f");
        line("orthographic", "false"); line("orthographicHalfExtent", "4.f");
        call = "scene.setCamera(" + var;
    } else throw std::runtime_error("add는 object/layer/light/camera를 지원합니다.");
    const bool parent = (kind == "object" && a.size() == 5) || ((kind == "light" || kind == "camera") && a.size() == 4);
    if (parent) { sources.block("object", a.back()); call += ", \"" + a.back() + "\""; }
    code.push_back("    " + call + ");"); code.push_back("    // @debug end " + kind + ' ' + name); code.push_back("");
    const SourceFile* file = nullptr; std::size_t at = 0;
    const std::regex insertion(R"(^\s*//\s*@debug\s+insert\s*$)");
    for (const auto& candidate : sources.files) for (std::size_t i = 0; i < candidate.lines.size(); ++i)
        if (std::regex_match(candidate.lines[i], insertion)) { require(!file, "삽입 지점이 여러 개입니다. project 범위를 좁히세요."); file = &candidate; at = i; }
    require(file != nullptr, "@debug insert 지점이 없습니다. configureScene 작성 함수의 닫는 괄호 앞에 추가하세요.");
    proposal(*file, at + 1, at, code, "configureScene(scene, options) 안에 새 " + kind + " 등록; 부모는 먼저 등록해야 함", out);
    if (kind == "light") out << "조명 사용: render main lighting on. 기본 셰이더 점광원은 최대 8개입니다.\n";
}
void parent(SourceIndex& sources, const Words& a, std::ostream& out) {
    require(a.size() == 4, "object/light/camera 이름 parent 부모이름/none");
    const auto& block = sources.block(a[0], a[1]); const auto& file = sources.files[block.file];
    require(a[0] == "object" || a[0] == "light" || a[0] == "camera", "parent는 object/light/camera에만 적용합니다.");
    if (a[3] != "none") {
        const auto& owner = sources.block("object", a[3]);
        require(a[0] != "object" || a[1] != a[3], "자기 자신을 부모로 지정할 수 없습니다.");
        require(owner.file == block.file && owner.last < block.first, "같은 작성 함수에서 부모 객체를 먼저 등록하세요.");
    }
    const std::string method = a[0] == "object" ? "addObject" : a[0] == "light" ? "addLight" : "setCamera";
    const std::string args = a[0] == "camera" ? "" : "\"" + a[1] + "\",\\s*";
    const std::regex call("^(\\s*)([A-Za-z_][A-Za-z_0-9]*)\\." + method + "\\(" + args + "([A-Za-z_][A-Za-z_0-9]*)(?:,\\s*\"[^\"]*\")?\\);\\s*$");
    bool found = false; std::size_t at = 0; std::string after;
    for (auto i = block.first + 1; i < block.last; ++i) {
        std::smatch match; if (!std::regex_match(file.lines[i], match, call)) continue;
        require(!found, "등록 호출이 여러 개입니다. replace로 직접 범위를 지정하세요."); found = true; at = i;
        after = match[1].str() + match[2].str() + '.' + method + '(';
        if (a[0] != "camera") after += '"' + a[1] + "\", ";
        after += match[3].str(); if (a[3] != "none") after += ", \"" + a[3] + '"'; after += ");";
    }
    require(found, "단일 행 등록 호출을 찾지 못했습니다. show/find를 사용하세요.");
    proposal(file, at + 1, at + 1, {after}, a[0] == "object" ? "객체 배치는 월드 좌표로 지정하고 이후 부모 변환을 상속" : "광원/카메라의 좌표를 부모 지역 좌표로 해석", out);
}
}

void help(std::ostream& out) {
    out << R"(project "폴더 또는 .vcxproj" [대상 지정] - 개인 절대 경로를 코드에 넣지 않음
list [현재 소스 구획 목록] - 매 명령에서 디스크 파일을 다시 읽음
show 종류 이름 [구획 전체 코드] - 종류 object/layer/light/camera/render
object 이름 position/rotation/scale/size/pivot X Y Z [수정안] - 월드 배치 / 모델 크기·피벗
object 이름 color R G B [수정안] - 재질 RGB 0~1
object 이름 ambient/diffuse/specular/shininess 값 [수정안] - Ka/Kd/Ks/정반사 지수
object 이름 unlit on/off [수정안] - 조명 계산 생략
object 이름 shape box/tetra/pyramid [수정안] - 기본 형상
layer 이름 translation/rotation/scale/pivot/axis X Y Z [수정안] - 선행 레이어 기준 지역 변환
layer 이름 spin 도/초 [수정안] - 시간에 따른 지역 회전
light 이름 position X Y Z / color R G B [수정안] - 부모가 있으면 지역 위치
light 이름 intensity/attenuation 값 [수정안] - 강도/거리 제곱 감쇠
camera 이름 position/target/up X Y Z [수정안] - 시점/주목점/up
camera 이름 fov/near/far/extent 값 / ortho on/off [수정안] - 투영 설정
render main lighting/textures on/off [수정안] - 렌더 옵션
render main ambient/sun-color R G B / sun-direction X Y Z / sun-intensity 값 [수정안]
render main background R G B A [수정안] - 배경색
object/light/camera 이름 parent 부모이름/none [등록 코드 수정안] - 부모는 먼저 등록
add object 이름 box/tetra/pyramid [부모] [전체 삽입 코드]
add layer 이름 객체 [전체 삽입 코드] - 이후 레이어가 앞 레이어를 상속
add light 이름 [부모] / add camera 이름 [부모] [전체 삽입 코드]
find "문자열" [실제 파일·행·코드] - 일반 C++/GLSL/props에도 사용
code "파일" 시작행 끝행 [현재 코드] - 프로젝트 내부의 색인된 파일
replace "파일" 시작행 끝행 "코드" [정확한 before/after] - 코드 안의 \n은 줄바꿈
insert "파일" 앞행 "코드" [삽입안] - 마지막 행+1도 가능
help / quit [도움말/종료]
소스 저장·컴파일·실행은 사용자가 수행합니다. 함수의 동적 실행이나 임의 C++ 의미 분석은 하지 않습니다.
)";
}
bool command(SourceIndex& sources, const std::string& line, std::ostream& out) {
    const auto a = words(line); if (a.empty()) return true;
    if (a[0] == "quit") { require(a.size() == 1, "quit"); return false; }
    if (a[0] == "help") { require(a.size() == 1, "help"); help(out); return true; }
    if (a[0] == "project") { require(a.size() == 2, "project \"폴더 또는 .vcxproj\""); sources.project(fs::u8path(a[1])); }
    sources.refresh();
    if (a[0] == "project") { out << "대상: " << sources.root().u8string() << " | 소스 " << sources.files.size() << "개\n"; return true; }
    if (a[0] == "list") {
        require(a.size() == 1, "list");
        for (const auto& b : sources.blocks) out << b.kind << ' ' << b.name << " | " << sources.files[b.file].path.u8string() << ':' << b.first + 1 << '-' << b.last + 1 << '\n';
        if (sources.blocks.empty()) out << "@debug 구획이 없습니다. find/code/replace/insert로 일반 소스에 사용할 수 있습니다.\n";
    } else if (a[0] == "show") {
        require(a.size() == 3, "show 종류 이름"); const auto& b = sources.block(a[1], a[2]); sources.print(sources.files[b.file], b.first + 1, b.last + 1, out);
    } else if (a[0] == "find") {
        require(a.size() == 2 && !a[1].empty(), "find \"문자열\""); std::size_t count = 0;
        for (const auto& f : sources.files) for (std::size_t i = 0; i < f.lines.size(); ++i)
            if (f.lines[i].find(a[1]) != std::string::npos) { out << f.path.u8string() << ':' << i + 1 << " | " << f.lines[i] << '\n'; ++count; }
        out << "검색 결과 " << count << "개.\n";
    } else if (a[0] == "code") {
        require(a.size() == 4, "code 파일 시작행 끝행"); sources.print(sources.file(fs::u8path(a[1])), index(a[2]), index(a[3]), out);
    } else if (a[0] == "replace" || a[0] == "insert") {
        const bool replace = a[0] == "replace"; require(a.size() == (replace ? 5u : 4u), "replace 파일 시작행 끝행 코드 / insert 파일 앞행 코드");
        const auto first = index(a[2]); const auto last = replace ? index(a[3]) : first - 1;
        require(!replace || first <= last, "시작행은 끝행 이하여야 합니다.");
        proposal(sources.file(fs::u8path(a[1])), first, last, splitCode(a.back()), "일반 소스 편집 제안; 문법·의미는 사용자 확인", out);
    } else if (a[0] == "add") add(sources, a, out);
    else if (a.size() >= 3 && a[2] == "parent") parent(sources, a, out);
    else if (a[0] == "object" || a[0] == "layer" || a[0] == "light" || a[0] == "camera" || a[0] == "render") set(sources, a, out);
    else throw std::runtime_error("지원하지 않는 명령입니다. help를 입력하세요.");
    return true;
}
} // namespace source_debugger
