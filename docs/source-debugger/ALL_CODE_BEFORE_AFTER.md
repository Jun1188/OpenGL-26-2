# 전체 코드 변경 전후

기준 커밋 `01db4ff48ade5f8d8941110408852762cb0f3f64`에서 이번 저장된 소스까지의 모든 변경 블록이다.

이전 프로젝트의 이동은 old_standard의 파일을 원래 standard 경로와 비교한다. 내용이 같은 이동은 코드 변경으로 중복 표기하지 않는다. 자동 생성 문서·그래프·기존 SDK 복사본은 manifest에 기록하고 코드 블록 대조에서는 제외한다. 빌드·테스트·검증기·앱은 실행하지 않았다.

## OpenGL-26-2.sln

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````text
Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio Version 17
VisualStudioVersion = 17.0.31903.59
MinimumVisualStudioVersion = 10.0.40219.1
Project("{BC8A1FFA-BEE3-4634-8014-F334798102B3}") = "standard", "standard\standard.vcxproj", "{AD50DC3F-7731-4954-923F-907BB2A8277C}"
EndProject
Project("{BC8A1FFA-BEE3-4634-8014-F334798102B3}") = "debugger", "debugger\debugger.vcxproj", "{48D466D1-8954-4D75-A111-D8BAC65250A8}"
EndProject
Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|x64 = Debug|x64
		Release|x64 = Release|x64
	EndGlobalSection
	GlobalSection(ProjectConfigurationPlatforms) = postSolution
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Debug|x64.ActiveCfg = Debug|x64
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Debug|x64.Build.0 = Debug|x64
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Release|x64.ActiveCfg = Release|x64
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Release|x64.Build.0 = Release|x64
		{AD50DC3F-7731-4954-923F-907BB2A8277C}.Debug|x64.ActiveCfg = Debug|x64
		{AD50DC3F-7731-4954-923F-907BB2A8277C}.Debug|x64.Build.0 = Debug|x64
		{AD50DC3F-7731-4954-923F-907BB2A8277C}.Release|x64.ActiveCfg = Release|x64
		{AD50DC3F-7731-4954-923F-907BB2A8277C}.Release|x64.Build.0 = Release|x64
	EndGlobalSection
EndGlobal
````


## debugger/Commands.cpp

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
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
````


## debugger/SourceIndex.cpp

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "SourceIndex.h"
#include <algorithm>
#include <fstream>
#include <optional>
#include <regex>
#include <stdexcept>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace source_debugger {
namespace {
bool excluded(const fs::path& path) {
    const auto name = path.filename().u8string();
    return name == ".git" || name == ".vs" || name == "x64" || name == "Debug" || name == "Release" ||
        name == "dependencies" || name == "docs" || name == "old_standard" || name == "graphify-out" ||
        name == ".agents" || name == ".obj-work" || name == "dist" || name == "packages";
}
bool source(const fs::path& path) {
    const auto ext = path.extension().u8string();
    return ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".glsl" || ext == ".props" || ext == ".vcxproj";
}
}
void SourceIndex::project(const fs::path& input) {
    auto path = fs::canonical(input);
    if (fs::is_regular_file(path)) {
        if (path.extension() != ".vcxproj") throw std::runtime_error("프로젝트 폴더 또는 .vcxproj를 지정하세요.");
        path = path.parent_path();
    }
    if (!fs::is_directory(path)) throw std::runtime_error("프로젝트 폴더가 아닙니다.");
    root_ = path;
}
void SourceIndex::refresh() {
    if (root_.empty()) throw std::runtime_error("project \"프로젝트 폴더 또는 .vcxproj 경로\"를 먼저 입력하세요.");
    std::vector<SourceFile> next;
    std::vector<fs::path> paths;
    for (fs::recursive_directory_iterator it(root_), end; it != end; ++it) {
        const auto status = it->symlink_status();
        const auto attributes = GetFileAttributesW(it->path().c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) throw std::runtime_error("소스 경로 속성을 읽지 못했습니다.");
        if (fs::is_symlink(status) || (attributes & FILE_ATTRIBUTE_REPARSE_POINT) || excluded(it->path())) {
            it.disable_recursion_pending(); continue;
        }
        if (fs::is_regular_file(status) && source(it->path())) paths.push_back(it->path());
    }
    std::sort(paths.begin(), paths.end());
    for (const auto& path : paths) {
        if (fs::file_size(path) > 8 * 1024 * 1024) throw std::runtime_error("소스 파일이 8 MiB를 초과합니다: " + path.u8string());
        std::ifstream input(path, std::ios::binary);
        if (!input) throw std::runtime_error("소스를 열 수 없습니다: " + path.u8string());
        SourceFile file{path, {}};
        std::string line;
        while (std::getline(input, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (file.lines.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
            if (line.find('\0') != std::string::npos) throw std::runtime_error("UTF-8 텍스트 소스가 필요합니다: " + path.u8string());
            if (!line.empty() && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, line.data(), int(line.size()), nullptr, 0) == 0)
                throw std::runtime_error("UTF-8로 읽을 수 없는 소스입니다: " + path.u8string());
            file.lines.push_back(line);
        }
        if (input.bad()) throw std::runtime_error("소스 읽기 실패: " + path.u8string());
        next.push_back(std::move(file));
    }
    std::vector<Block> found;
    const std::regex marker(R"(^\s*//\s*@debug\s+(begin|end)\s+(object|layer|light|camera|render)\s+([A-Za-z_][A-Za-z_0-9]*)\s*$)");
    for (std::size_t f = 0; f < next.size(); ++f) {
        std::optional<Block> open;
        for (std::size_t i = 0; i < next[f].lines.size(); ++i) {
            std::smatch match;
            if (!std::regex_match(next[f].lines[i], match, marker)) continue;
            if (match[1] == "begin") {
                if (open) throw std::runtime_error("중첩 @debug 구획: " + next[f].path.u8string());
                open = Block{f, i, i, match[2], match[3]};
            } else {
                if (!open || open->kind != match[2] || open->name != match[3])
                    throw std::runtime_error("짝이 맞지 않는 @debug end: " + next[f].path.u8string());
                open->last = i; found.push_back(*open); open.reset();
            }
        }
        if (open) throw std::runtime_error("닫히지 않은 @debug 구획: " + next[f].path.u8string());
    }
    files = std::move(next); blocks = std::move(found);
}
const Block& SourceIndex::block(const std::string& kind, const std::string& name) const {
    const Block* result = nullptr;
    for (const auto& item : blocks) if (item.kind == kind && item.name == name) {
        if (result) throw std::runtime_error("같은 구획이 여러 파일에 있습니다. project로 대상 폴더를 좁히세요.");
        result = &item;
    }
    if (!result) throw std::runtime_error("해당 @debug 구획이 없습니다. list/find를 사용하세요.");
    return *result;
}
const SourceFile& SourceIndex::file(const fs::path& input) const {
    const auto path = fs::canonical(input.is_absolute() ? input : root_ / input);
    for (const auto& item : files) if (fs::equivalent(item.path, path)) return item;
    throw std::runtime_error("현재 프로젝트에 색인한 소스 파일이 아닙니다.");
}
void SourceIndex::print(const SourceFile& file, std::size_t first, std::size_t last, std::ostream& out) const {
    if (first == 0 || first > last || last > file.lines.size()) throw std::runtime_error("현재 소스의 행 범위를 벗어났습니다.");
    out << file.path.u8string() << ':' << first << '-' << last << '\n';
    for (std::size_t i = first; i <= last; ++i) out << i << " | " << file.lines[i - 1] << '\n';
}
void proposal(const SourceFile& file, std::size_t first, std::size_t last,
              const std::vector<std::string>& after, const std::string& meaning, std::ostream& out) {
    if (first == 0 || first > file.lines.size() + 1 || last > file.lines.size() || last + 1 < first)
        throw std::runtime_error("제안할 행 범위가 현재 파일과 맞지 않습니다.");
    out << "[수정 제안] " << file.path.u8string() << ':' << first;
    if (last >= first && last != first) out << '-' << last;
    if (last < first) out << " 앞에 삽입";
    out << "\n[위치·의미] " << meaning << "\n[before]\n";
    if (last < first) out << "(없음: 새 코드 삽입)\n";
    else for (auto i = first; i <= last; ++i) out << i << " | " << file.lines[i - 1] << '\n';
    out << "[after: 복사할 C++ 코드]\n";
    for (const auto& line : after) out << line << '\n';
    out << "[주변 코드]\n";
    if (first > 1) out << first - 1 << " | " << file.lines[first - 2] << '\n';
    const auto following = last < first ? first : last + 1;
    if (following <= file.lines.size()) out << following << " | " << file.lines[following - 1] << '\n';
    out << "파일은 사용자가 직접 수정합니다. 다음 명령에서 디스크 소스를 다시 읽습니다.\n";
}
} // namespace source_debugger
````


## debugger/SourceIndex.h

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#pragma once
#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

namespace source_debugger {
namespace fs = std::filesystem;
struct SourceFile { fs::path path; std::vector<std::string> lines; };
struct Block { std::size_t file, first, last; std::string kind, name; };
class SourceIndex {
    fs::path root_;
public:
    std::vector<SourceFile> files;
    std::vector<Block> blocks;
    void project(const fs::path& path);
    const fs::path& root() const { return root_; }
    void refresh();
    const Block& block(const std::string& kind, const std::string& name) const;
    const SourceFile& file(const fs::path& path) const;
    void print(const SourceFile& source, std::size_t first, std::size_t last, std::ostream& out) const;
};
void proposal(const SourceFile& file, std::size_t first, std::size_t last,
              const std::vector<std::string>& after, const std::string& meaning, std::ostream& out);
bool command(SourceIndex& sources, const std::string& line, std::ostream& out);
void help(std::ostream& out);
} // namespace source_debugger
````


## debugger/Toolchain.props

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <!-- Import after Microsoft.Cpp.Default.props and before Microsoft.Cpp.props. -->
  <PropertyGroup>
    <DebuggerToolsetRoot>$(VCTargetsPath)\Platforms\x64\PlatformToolsets</DebuggerToolsetRoot>
    <PlatformToolset Condition="'$(PlatformToolset)'=='' and '$(DefaultPlatformToolset)'!='' and Exists('$(DebuggerToolsetRoot)\$(DefaultPlatformToolset)\Toolset.props')">$(DefaultPlatformToolset)</PlatformToolset>
    <PlatformToolset Condition="'$(PlatformToolset)'=='' and Exists('$(DebuggerToolsetRoot)\v143\Toolset.props')">v143</PlatformToolset>
    <PlatformToolset Condition="'$(PlatformToolset)'=='' and Exists('$(DebuggerToolsetRoot)\v145\Toolset.props')">v145</PlatformToolset>
  </PropertyGroup>
  <Target Name="ExplainDebuggerToolchain" BeforeTargets="PrepareForBuild">
    <Message Importance="High" Text="Debugger toolchain: VS=$(VisualStudioVersion); Platform=$(Platform); Toolset=$(PlatformToolset); VCTargetsPath=$(VCTargetsPath)" />
    <Error Condition="'$(PlatformToolset)'=='' or !Exists('$(DebuggerToolsetRoot)\$(PlatformToolset)\Toolset.props')" Text="Debugger toolset unavailable: $(PlatformToolset). Expected $(DebuggerToolsetRoot)\$(PlatformToolset)\Toolset.props. VS 2022: install Desktop development with C++ and MSVC v143, then open the .sln (not .slnx on older VS). For a deliberate toolset override, install that toolset or change Platform Toolset in Project Properties." />
  </Target>
</Project>
````


## debugger/debugger.sln

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````text
Microsoft Visual Studio Solution File, Format Version 12.00
# Visual Studio Version 17
VisualStudioVersion = 17.0.31903.59
MinimumVisualStudioVersion = 10.0.40219.1
Project("{BC8A1FFA-BEE3-4634-8014-F334798102B3}") = "debugger", "debugger.vcxproj", "{48D466D1-8954-4D75-A111-D8BAC65250A8}"
EndProject
Global
	GlobalSection(SolutionConfigurationPlatforms) = preSolution
		Debug|x64 = Debug|x64
		Release|x64 = Release|x64
	EndGlobalSection
	GlobalSection(ProjectConfigurationPlatforms) = postSolution
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Debug|x64.ActiveCfg = Debug|x64
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Debug|x64.Build.0 = Debug|x64
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Release|x64.ActiveCfg = Release|x64
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Release|x64.Build.0 = Release|x64
	EndGlobalSection
EndGlobal
````


## debugger/debugger.vcxproj

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64"><Configuration>Debug</Configuration><Platform>x64</Platform></ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64"><Configuration>Release</Configuration><Platform>x64</Platform></ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{48D466D1-8954-4D75-A111-D8BAC65250A8}</ProjectGuid>
    <Keyword>Win32Proj</Keyword><ProjectName>debugger</ProjectName>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <Import Project="Toolchain.props" />
  <PropertyGroup Label="Configuration">
    <ConfigurationType>Application</ConfigurationType><CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries Condition="'$(Configuration)'=='Debug'">true</UseDebugLibraries>
    <UseDebugLibraries Condition="'$(Configuration)'=='Release'">false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <OutDir>$(ProjectDir)x64\$(Configuration)\</OutDir>
    <IntDir>$(ProjectDir)x64\$(Configuration)\obj\</IntDir>
    <TargetName>debugger</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile><LanguageStandard>stdcpp17</LanguageStandard><WarningLevel>Level4</WarningLevel><AdditionalOptions>/utf-8 /FC %(AdditionalOptions)</AdditionalOptions></ClCompile>
    <Link><SubSystem>Console</SubSystem><GenerateDebugInformation>true</GenerateDebugInformation></Link>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="main.cpp" /><ClCompile Include="SourceIndex.cpp" /><ClCompile Include="Commands.cpp" />
    <ClInclude Include="SourceIndex.h" /><None Include="README.md" />
  </ItemGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
````


## debugger/main.cpp

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "SourceIndex.h"
#include <iostream>
#include <exception>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

int wmain(int argc, wchar_t** argv) {
    SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8);
    source_debugger::SourceIndex sources;
    std::cout << "Source debugger: 실제 디스크 소스의 편집 위치와 코드를 안내합니다.\n";
    if (argc == 2) {
        try { sources.project(std::filesystem::path(argv[1])); sources.refresh(); std::cout << "대상: " << sources.root().u8string() << '\n'; }
        catch (const std::exception& error) { std::cerr << "대상 오류: " << error.what() << '\n'; }
    } else if (argc > 2) { std::cerr << "사용법: debugger.exe [\"프로젝트 폴더 또는 .vcxproj\"]\n"; return 1; }
    source_debugger::help(std::cout);
    std::string line;
    while (std::cout << "dbg> " << std::flush, std::getline(std::cin, line)) {
        try { if (!source_debugger::command(sources, line, std::cout)) break; }
        catch (const std::exception& error) { std::cerr << "명령 거절: " << error.what() << '\n'; }
    }
    return 0;
}
````


## framework/Framework.h

### 블록 1 · before L10 / after L10

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "include/SceneAuthoring.h"
````


## framework/detail/SceneAuthoring.cpp

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "../include/SceneAuthoring.h"
#include <stdexcept>

namespace graphics {
namespace {
void requireAuthoring(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
bool finite(const glm::vec3& value) {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}
bool nonsingular(const glm::mat4& matrix) {
    const auto determinant = glm::determinant(glm::mat3(matrix));
    return TransformSystem::validAffine(matrix) && std::isfinite(determinant) && std::abs(determinant) > 1e-18f;
}
glm::mat4 rotation(const glm::vec3& degrees) {
    requireAuthoring(finite(degrees), "SceneAuthoring: rotation must be finite.");
    glm::mat4 result(1);
    result = glm::rotate(result, glm::radians(degrees.z), glm::vec3(0, 0, 1));
    result = glm::rotate(result, glm::radians(degrees.y), glm::vec3(0, 1, 0));
    return glm::rotate(result, glm::radians(degrees.x), glm::vec3(1, 0, 0));
}
glm::mat4 layerMatrix(const LayerSetup& setup, double seconds) {
    requireAuthoring(finite(setup.translation) && finite(setup.scale) && finite(setup.pivot) &&
        finite(setup.spinAxis) && std::isfinite(setup.spinDegreesPerSecond), "SceneAuthoring: invalid layer values.");
    glm::mat4 spin(1);
    if (setup.spinDegreesPerSecond != 0) {
        requireAuthoring(glm::length(setup.spinAxis) > 1e-6f, "SceneAuthoring: spin axis is zero.");
        const double angle = setup.spinDegreesPerSecond * seconds;
        requireAuthoring(std::isfinite(angle), "SceneAuthoring: spin time overflow.");
        spin = glm::rotate(glm::mat4(1), float(glm::radians(std::remainder(angle, 360.0))), glm::normalize(setup.spinAxis));
    }
    const auto result = glm::translate(glm::mat4(1), setup.translation + setup.pivot) *
        rotation(setup.rotationDegrees) * spin * glm::scale(glm::mat4(1), setup.scale) *
        glm::translate(glm::mat4(1), -setup.pivot);
    requireAuthoring(nonsingular(result),
        "SceneAuthoring: layer scale must be invertible.");
    return result;
}
bool validColor(GlColor color) {
    return std::isfinite(color.red) && std::isfinite(color.green) && std::isfinite(color.blue) &&
        color.red >= 0 && color.green >= 0 && color.blue >= 0;
}
}

void SceneAuthoring::clear() {
    app_.getScene().clear(); objects_.clear(); layers_.clear(); lights_.clear();
    camera_.reset(); cameraParent_.clear(); app_.getRenderOptions().pointLights.clear();
}
EntityId SceneAuthoring::object(const std::string& name) const {
    const auto found = objects_.find(name);
    if (found == objects_.end()) throw std::runtime_error("SceneAuthoring: missing object: " + name);
    return found->second;
}
std::string SceneAuthoring::name(EntityId id) const {
    for (const auto& item : objects_) if (item.second == id) return item.first;
    return {};
}
EntityId SceneAuthoring::addObject(const std::string& name, const ObjectSetup& setup, const std::string& parent) {
    requireAuthoring(!name.empty() && !objects_.count(name), "SceneAuthoring: duplicate/empty object name.");
    requireAuthoring(setup.shape == GLShape::Cube || setup.shape == GLShape::Tetra || setup.shape == GLShape::Pyramid,
        "SceneAuthoring: use native Scene APIs for other model types.");
    requireAuthoring(finite(setup.size) && glm::all(glm::greaterThan(setup.size, glm::vec3(0))) &&
        finite(setup.position) && finite(setup.pivot) && finite(setup.scale), "SceneAuthoring: invalid object values.");
    const auto parentId = parent.empty() ? std::optional<EntityId>{} : std::optional<EntityId>{object(parent)};
    Transform3D pose;
    pose.basis = setup.position; pose.rotation = glm::mat3(rotation(setup.rotationDegrees));
    pose.scale = setup.scale; pose.localBasis = setup.pivot;
    const auto world = DiagramSystem::modelMatrix(pose);
    requireAuthoring(nonsingular(world),
        "SceneAuthoring: object scale must be invertible.");
    auto model = DiagramSystem::makePolyhedron(setup.shape, setup.size);
    for (auto& face : model.faces) face.material = setup.material;
    auto saved = app_.getScene();
    try {
        const auto id = app_.getScene().push_entity(model);
        requireAuthoring(id.has_value(), "SceneAuthoring: object creation failed.");
        requireAuthoring(app_.getScene().setParent(*id, parentId) && app_.getScene().setWorldPlacement(*id, world),
            "SceneAuthoring: parent/world placement failed.");
        objects_.emplace(name, *id);
        return *id;
    } catch (...) { app_.getScene() = std::move(saved); throw; }
}
void SceneAuthoring::addLayer(const std::string& objectName, const std::string& name, const LayerSetup& setup) {
    const auto id = object(objectName);
    requireAuthoring(!name.empty(), "SceneAuthoring: empty layer name.");
    std::size_t count = 0;
    for (const auto& layer : layers_) if (layer.object == objectName) {
        requireAuthoring(layer.name != name, "SceneAuthoring: duplicate object-local layer name."); ++count;
    }
    requireAuthoring(count < 128, "SceneAuthoring: at most 128 layers per object.");
    std::vector<glm::mat4> initial;
    for (const auto& layer : layers_) if (layer.object == objectName) initial.push_back(layerMatrix(layer.setup, 0));
    initial.push_back(layerMatrix(setup, 0));
    // Materialize initial layers before a later child computes its world-authored attachment.
    requireAuthoring(app_.getScene().setLocalLayers(id, initial), "SceneAuthoring: invalid initial local layers.");
    layers_.push_back({objectName, name, setup});
}
void SceneAuthoring::addLight(const std::string& name, const PointLight& light, const std::string& parent) {
    requireAuthoring(!name.empty() && lights_.size() < 8, "SceneAuthoring: empty name or more than eight point lights.");
    for (const auto& item : lights_) requireAuthoring(item.name != name, "SceneAuthoring: duplicate light name.");
    if (!parent.empty()) object(parent);
    requireAuthoring(finite(light.position) && validColor(light.color) && std::isfinite(light.intensity) &&
        light.intensity >= 0 && std::isfinite(light.attenuation) && light.attenuation >= 0, "SceneAuthoring: invalid point light.");
    lights_.push_back({name, parent, light});
}
void SceneAuthoring::setCamera(const Camera& camera, const std::string& parent) {
    if (!parent.empty()) object(parent);
    requireAuthoring(CameraSystem::validCamera(camera), "SceneAuthoring: invalid camera.");
    camera_ = camera; cameraParent_ = parent;
}
void SceneAuthoring::update(double seconds) {
    requireAuthoring(std::isfinite(seconds) && seconds >= 0, "SceneAuthoring: time must be finite and nonnegative.");
    std::map<EntityId, std::vector<glm::mat4>> matrices;
    for (const auto& layer : layers_) matrices[object(layer.object)].push_back(layerMatrix(layer.setup, seconds));
    for (const auto& item : matrices)
        requireAuthoring(app_.getScene().setLocalLayers(item.first, item.second), "SceneAuthoring: invalid composed local layers.");
    std::vector<PointLight> lights;
    for (const auto& item : lights_) {
        auto light = item.value;
        if (!item.parent.empty()) {
            const auto frame = app_.getScene().worldMatrix(object(item.parent));
            requireAuthoring(frame.has_value(), "SceneAuthoring: invalid light frame.");
            light.position = glm::vec3(*frame * glm::vec4(light.position, 1));
        }
        requireAuthoring(finite(light.position), "SceneAuthoring: light position overflow.");
        lights.push_back(light);
    }
    app_.getRenderOptions().pointLights = std::move(lights);
    if (camera_) {
        if (cameraParent_.empty()) app_.getCamera() = *camera_;
        else {
            const auto frame = app_.getScene().worldMatrix(object(cameraParent_));
            requireAuthoring(frame.has_value(), "SceneAuthoring: invalid camera frame.");
            const auto camera = CameraSystem::attached(*camera_, *frame);
            requireAuthoring(camera.has_value(), "SceneAuthoring: degenerate camera frame.");
            app_.getCamera() = *camera;
        }
    }
}
} // namespace graphics
````


## framework/examples/source_authoring.cpp

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "../Framework.h"
#include <iostream>
#include <exception>

// Compile as an independent entrypoint with this folder's Framework.props.
int main() try {
    graphics::Application app;
    graphics::SceneAuthoring scene(app);
    double seconds = 0;
    app.setInitHandler([&](graphics::Application&) {
        scene.clear();
        graphics::ObjectSetup object;
        object.position = {0.f, 0.f, 0.f};
        object.scale = {2.f, 1.f, 0.5f};
        object.material.color = {0.3f, 0.6f, 0.9f};
        object.material.specularStrength = 0.7f;
        object.material.shininess = 64;
        scene.addObject("sample", object);
        graphics::LayerSetup spin;
        spin.spinDegreesPerSecond = 30;
        scene.addLayer("sample", "spin", spin);
        graphics::PointLight key;
        key.position = {3.f, 4.f, 5.f};
        key.intensity = 2;
        scene.addLight("key", key);
        graphics::Camera camera;
        scene.setCamera(camera);
        auto& render = app.getRenderOptions();
        render.lighting = true;
        render.directionalIntensity = 0;
        render.ambientColor = {0.15f, 0.15f, 0.15f};
        scene.update(0);
        return true;
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) { seconds += dt; scene.update(seconds); });
    graphics::ApplicationConfig config;
    config.title = "Native source authoring example";
    return app.run(config);
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n'; return 1;
}
````


## framework/include/SceneAuthoring.h

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#pragma once
#include "Application.h"

namespace graphics {
// Direct C++ authoring data. No command parser or assembly file is required.
struct ObjectSetup {
    GLShape shape = GLShape::Cube;
    glm::vec3 size{1}, position{0}, rotationDegrees{0}, scale{1}, pivot{0};
    Material material;
};

struct LayerSetup {
    glm::vec3 translation{0}, rotationDegrees{0}, scale{1}, pivot{0};
    glm::vec3 spinAxis{0, 1, 0};
    double spinDegreesPerSecond = 0;
};

class SceneAuthoring {
    struct Layer { std::string object, name; LayerSetup setup; };
    struct Light { std::string name, parent; PointLight value; };
    Application& app_;
    std::map<std::string, EntityId> objects_;
    std::vector<Layer> layers_;
    std::vector<Light> lights_;
    std::optional<Camera> camera_;
    std::string cameraParent_;
public:
    explicit SceneAuthoring(Application& app) : app_(app) {}
    /// [API] Clear Scene and authored handles together; call before rebuilding the scene.
    void clear();
    /// [API] Create a cube/tetrahedron/pyramid at a world-authored pose; parent must already exist.
    EntityId addObject(const std::string& name, const ObjectSetup& setup, const std::string& parent = "");
    /// [API] Append an object-local layer. Later layers inherit all earlier layers.
    void addLayer(const std::string& object, const std::string& name, const LayerSetup& setup);
    /// [API] Register a point light, optionally expressed in a named object's local frame.
    void addLight(const std::string& name, const PointLight& light, const std::string& parent = "");
    /// [API] Select one camera, optionally expressed in a named object's local frame.
    void setCamera(const Camera& camera, const std::string& parent = "");
    /// [API] Evaluate local layers, attached lights and camera at absolute seconds; throws on invalid input.
    void update(double seconds);
    /// [API] Resolve a source-authored object name; throws if the name is absent.
    EntityId object(const std::string& name) const;
    /// [API] Resolve a picked object ID to its source-authored name, or return an empty string.
    std::string name(EntityId id) const;
};
} // namespace graphics
````


## standard/Console.cpp

### 블록 1 · before L2 / after L2

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "SceneSetup.h"
````


### 블록 2 · before L5 / after L6

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
    if (!assembly::defaultRecipe(workspace_, std::cout)) return false;
    workspace_.help(std::cout);
    input_.prompt();
````

**after**

````cpp
    scene_.clear();
    configureScene(scene_, app_.getRenderOptions());
    scene_.update(0);
    std::cout << "Standard: SceneSetup.cpp를 직접 편집하세요.\n"
                 "별도 debugger 프로젝트가 현재 소스의 경로/행/수정 전후 코드를 안내합니다.\n"
                 "클릭: 객체 이름 확인 | Space: 시간 정지 | Home: 시간 초기화 | Esc: 종료\n";
````


### 블록 3 · before L10 / after L14

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
void Console::onKey(int key, int action, int mods) {
    if (action == GLFW_PRESS && key == GLFW_KEY_F12) {
        workspace_.help(std::cout);
        return;
    }
    if (action == GLFW_PRESS && key == GLFW_KEY_F5 && (mods & GLFW_MOD_CONTROL)) {
        workspace_.execute("reload", std::cout);
        return;
    }
    workspace_.onKey(key, action, mods, std::cout);
````

**after**

````cpp
void Console::onKey(int key, int action, int) {
    if (action != GLFW_PRESS) return;
    if (key == GLFW_KEY_ESCAPE) app_.requestClose();
    if (key == GLFW_KEY_SPACE) paused_ = !paused_;
    if (key == GLFW_KEY_HOME) { seconds_ = 0; scene_.update(0); }
````


### 블록 4 · before L21 / after L20

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
void Console::onMouse(const graphics::MouseEvent& event) { workspace_.onMouse(event, std::cout); }
void Console::onCursor(double x, double y) { workspace_.onCursor(x, y, std::cout); }
void Console::onFocus(bool focused) { workspace_.onFocus(focused); }
````

**after**

````cpp
void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) return;
    const auto hit = app_.pickAt(event.windowX, event.windowY);
    if (!hit) { std::cout << "선택: 없음\n"; return; }
    const auto name = scene_.name(hit->entityId);
    if (!name.empty()) std::cout << "선택: " << name << " | debugger 명령: show object " << name << '\n';
}
````


### 블록 5 · before L25 / after L28

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
    for (unsigned i = 0; i < 8; ++i) {
        const auto line = input_.poll();
        if (!line) break;
        if (*line == "quit") { app_.requestClose(); return; }
        workspace_.execute(*line, std::cout);
        input_.prompt();
    }
    workspace_.update(dt, std::cout);
````

**after**

````cpp
    if (!paused_) seconds_ += dt;
    scene_.update(seconds_);
````


### 블록 6 · before L34 / after L31

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
bool Console::onRender() { return workspace_.render(); }
````

**after**

````cpp
bool Console::onRender() { return app_.renderScene(); }
````


## standard/Console.h

### 블록 1 · before L2 / after L2

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
#include "modes/Assembly.h"
#include "modes/AssemblyConsole.h"
````

**after**

````cpp
#include "framework/Framework.h"
````


### 블록 2 · before L5 / after L4

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
// Standard owns the editor; reference practices do not participate in its runtime.
````

**after: 없음**



### 블록 3 · before L8 / after L6

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
    assembly::Assembly workspace_;
    assembly::ConsoleInput input_;
````

**after**

````cpp
    graphics::SceneAuthoring scene_;
    double seconds_ = 0;
    bool paused_ = false;
````


### 블록 4 · before L11 / after L10

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
    explicit Console(graphics::Application& app) : app_(app), workspace_(app) {}
````

**after**

````cpp
    explicit Console(graphics::Application& app) : app_(app), scene_(app) {}
````


### 블록 5 · before L15 / after L14

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
    void onCursor(double x, double y);
    void onFocus(bool focused);
````

**after: 없음**



## standard/SceneSetup.cpp

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "SceneSetup.h"

void configureScene(graphics::SceneAuthoring& scene, graphics::RenderOptions& options) {
    // @debug begin render main
    auto& cfg_render = options;
    cfg_render.lighting = true;
    cfg_render.textures = false;
    cfg_render.ambientColor = {0.15f, 0.15f, 0.15f};
    cfg_render.lightDirection = {1.f, 1.f, 1.f};
    cfg_render.lightColor = {1.f, 1.f, 1.f};
    cfg_render.directionalIntensity = 0.f;
    cfg_render.background = {0.08f, 0.10f, 0.14f, 1.f};
    // @debug end render main

    // @debug begin object base
    graphics::ObjectSetup cfg_base;
    cfg_base.shape = graphics::GLShape::Cube;
    cfg_base.size = {2.f, 0.5f, 2.f};
    cfg_base.position = {0.f, 0.f, 0.f};
    cfg_base.rotationDegrees = {0.f, 0.f, 0.f};
    cfg_base.scale = {1.f, 1.f, 1.f};
    cfg_base.pivot = {0.f, 0.f, 0.f};
    cfg_base.material.color = {0.3f, 0.5f, 0.8f};
    cfg_base.material.ambientStrength = 0.25f;
    cfg_base.material.diffuseStrength = 1.f;
    cfg_base.material.specularStrength = 0.4f;
    cfg_base.material.shininess = 32.f;
    cfg_base.material.unlit = false;
    scene.addObject("base", cfg_base);
    // @debug end object base

    // @debug begin object panel
    graphics::ObjectSetup cfg_panel;
    cfg_panel.shape = graphics::GLShape::Cube;
    cfg_panel.size = {2.f, 1.f, 0.2f};
    cfg_panel.position = {0.f, 1.f, 0.f};
    cfg_panel.rotationDegrees = {0.f, 0.f, 0.f};
    cfg_panel.scale = {1.f, 1.f, 1.f};
    cfg_panel.pivot = {0.f, 0.f, 0.f};
    cfg_panel.material.color = {0.9f, 0.5f, 0.2f};
    cfg_panel.material.ambientStrength = 0.25f;
    cfg_panel.material.diffuseStrength = 1.f;
    cfg_panel.material.specularStrength = 0.6f;
    cfg_panel.material.shininess = 64.f;
    cfg_panel.material.unlit = false;
    scene.addObject("panel", cfg_panel, "base");
    // @debug end object panel

    // @debug begin layer hinge
    graphics::LayerSetup cfg_hinge;
    cfg_hinge.translation = {0.f, 0.f, 0.f};
    cfg_hinge.rotationDegrees = {0.f, 0.f, 0.f};
    cfg_hinge.scale = {1.f, 1.f, 1.f};
    cfg_hinge.pivot = {-1.f, 0.f, 0.f};
    cfg_hinge.spinAxis = {0.f, 1.f, 0.f};
    cfg_hinge.spinDegreesPerSecond = 0;
    scene.addLayer("panel", "hinge", cfg_hinge);
    // @debug end layer hinge

    // @debug begin light key
    graphics::PointLight cfg_key;
    cfg_key.position = {2.f, 4.f, 3.f};
    cfg_key.color = {1.f, 0.95f, 0.85f};
    cfg_key.intensity = 1.5f;
    cfg_key.attenuation = 0.02f;
    scene.addLight("key", cfg_key);
    // @debug end light key

    // @debug begin camera main
    graphics::Camera cfg_main;
    cfg_main.position = {6.f, 4.f, 8.f};
    cfg_main.target = {0.f, 0.5f, 0.f};
    cfg_main.up = {0.f, 1.f, 0.f};
    cfg_main.fieldOfViewDegrees = 45.f;
    cfg_main.nearPlane = 0.1f;
    cfg_main.farPlane = 100.f;
    cfg_main.orthographic = false;
    cfg_main.orthographicHalfExtent = 4.f;
    scene.setCamera(cfg_main);
    // @debug end camera main

    // @debug insert
}
````


## standard/SceneSetup.h

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#pragma once
#include "framework/Framework.h"

// Edit the native C++ blocks in SceneSetup.cpp, then rebuild Standard yourself.
void configureScene(graphics::SceneAuthoring& scene, graphics::RenderOptions& options);
````


## standard/examples/SourceExamples.h

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#pragma once
#include "../framework/Framework.h"

// Native C++ references. Call one function from configureScene after scene.clear().
namespace source_examples {
inline void rotatingDisplay(graphics::SceneAuthoring& scene, graphics::RenderOptions& options) {
    graphics::ObjectSetup support;
    support.size = {0.4f, 3.f, 0.4f};
    support.position = {1.f, 0.f, 0.f};
    support.material.color = {0.2f, 0.4f, 0.7f};
    scene.addObject("support", support);

    graphics::ObjectSetup display;
    display.size = {2.f, 1.f, 0.2f};
    display.position = {2.f, 1.f, 0.f}; // World-authored, even with a parent.
    display.scale = {1.5f, 1.f, 0.5f};
    display.material.color = {0.9f, 0.6f, 0.2f};
    scene.addObject("display", display, "support");

    graphics::LayerSetup parentTurn;
    parentTurn.spinDegreesPerSecond = 20;
    scene.addLayer("support", "turn", parentTurn);
    graphics::LayerSetup hinge;
    hinge.pivot = {-1.f, 0.f, 0.f};
    hinge.spinAxis = {0.f, 0.f, 1.f};
    hinge.spinDegreesPerSecond = 45;
    scene.addLayer("display", "hinge", hinge);
    graphics::LayerSetup shift;
    shift.translation = {0.5f, 0.f, 0.f};
    scene.addLayer("display", "shift", shift);

    options.lighting = true;
    options.ambientColor = {0.2f, 0.2f, 0.2f};
    options.directionalIntensity = 0;
    graphics::PointLight key;
    key.position = {3.f, 5.f, 6.f};
    key.intensity = 2;
    scene.addLight("key", key);
    graphics::Camera camera;
    camera.position = {8.f, 5.f, 10.f};
    camera.target = {1.f, 0.f, 0.f};
    scene.setCamera(camera);
}

inline void attachedLightAndCamera(graphics::SceneAuthoring& scene, graphics::RenderOptions& options) {
    graphics::ObjectSetup carrier;
    carrier.position = {0.f, 0.f, 0.f};
    carrier.size = {1.f, 0.2f, 1.f};
    scene.addObject("carrier", carrier);
    graphics::LayerSetup turn;
    turn.spinDegreesPerSecond = 25;
    scene.addLayer("carrier", "orbit", turn);

    graphics::ObjectSetup sculpture;
    sculpture.shape = graphics::GLShape::Pyramid;
    sculpture.position = {0.f, 0.8f, 0.f};
    sculpture.material.color = {0.5f, 0.7f, 0.9f};
    sculpture.material.specularStrength = 0.8f;
    sculpture.material.shininess = 128;
    scene.addObject("sculpture", sculpture);

    options.lighting = true;
    options.ambientColor = {0.1f, 0.1f, 0.1f};
    options.directionalIntensity = 0;
    graphics::PointLight lamp;
    lamp.position = {3.f, 3.f, 0.f}; // Carrier-local position.
    lamp.color = {1.f, 0.8f, 0.5f};
    lamp.intensity = 2;
    scene.addLight("lamp", lamp, "carrier");
    graphics::Camera camera;
    camera.position = {6.f, 4.f, 0.f}; // Position, target and up are carrier-local.
    camera.target = {0.f, 0.8f, 0.f};
    scene.setCamera(camera, "carrier");
}
} // namespace source_examples
````


## standard/framework/Framework.h

### 블록 1 · before L10 / after L10

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "include/SceneAuthoring.h"
````


## standard/framework/Framework.props

### 블록 1 · before L10 / after L10

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````xml
    <ClCompile Include="$(GraphicsFrameworkDirectory)detail\Dynamics.cpp" />
````


### 블록 2 · before L16 / after L17

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````xml
    <ClCompile Include="$(GraphicsFrameworkDirectory)detail\SceneAuthoring.cpp" />
````


## standard/framework/detail/SceneAuthoring.cpp

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "../include/SceneAuthoring.h"
#include <stdexcept>

namespace graphics {
namespace {
void requireAuthoring(bool ok, const char* message) {
    if (!ok) throw std::runtime_error(message);
}
bool finite(const glm::vec3& value) {
    return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
}
bool nonsingular(const glm::mat4& matrix) {
    const auto determinant = glm::determinant(glm::mat3(matrix));
    return TransformSystem::validAffine(matrix) && std::isfinite(determinant) && std::abs(determinant) > 1e-18f;
}
glm::mat4 rotation(const glm::vec3& degrees) {
    requireAuthoring(finite(degrees), "SceneAuthoring: rotation must be finite.");
    glm::mat4 result(1);
    result = glm::rotate(result, glm::radians(degrees.z), glm::vec3(0, 0, 1));
    result = glm::rotate(result, glm::radians(degrees.y), glm::vec3(0, 1, 0));
    return glm::rotate(result, glm::radians(degrees.x), glm::vec3(1, 0, 0));
}
glm::mat4 layerMatrix(const LayerSetup& setup, double seconds) {
    requireAuthoring(finite(setup.translation) && finite(setup.scale) && finite(setup.pivot) &&
        finite(setup.spinAxis) && std::isfinite(setup.spinDegreesPerSecond), "SceneAuthoring: invalid layer values.");
    glm::mat4 spin(1);
    if (setup.spinDegreesPerSecond != 0) {
        requireAuthoring(glm::length(setup.spinAxis) > 1e-6f, "SceneAuthoring: spin axis is zero.");
        const double angle = setup.spinDegreesPerSecond * seconds;
        requireAuthoring(std::isfinite(angle), "SceneAuthoring: spin time overflow.");
        spin = glm::rotate(glm::mat4(1), float(glm::radians(std::remainder(angle, 360.0))), glm::normalize(setup.spinAxis));
    }
    const auto result = glm::translate(glm::mat4(1), setup.translation + setup.pivot) *
        rotation(setup.rotationDegrees) * spin * glm::scale(glm::mat4(1), setup.scale) *
        glm::translate(glm::mat4(1), -setup.pivot);
    requireAuthoring(nonsingular(result),
        "SceneAuthoring: layer scale must be invertible.");
    return result;
}
bool validColor(GlColor color) {
    return std::isfinite(color.red) && std::isfinite(color.green) && std::isfinite(color.blue) &&
        color.red >= 0 && color.green >= 0 && color.blue >= 0;
}
}

void SceneAuthoring::clear() {
    app_.getScene().clear(); objects_.clear(); layers_.clear(); lights_.clear();
    camera_.reset(); cameraParent_.clear(); app_.getRenderOptions().pointLights.clear();
}
EntityId SceneAuthoring::object(const std::string& name) const {
    const auto found = objects_.find(name);
    if (found == objects_.end()) throw std::runtime_error("SceneAuthoring: missing object: " + name);
    return found->second;
}
std::string SceneAuthoring::name(EntityId id) const {
    for (const auto& item : objects_) if (item.second == id) return item.first;
    return {};
}
EntityId SceneAuthoring::addObject(const std::string& name, const ObjectSetup& setup, const std::string& parent) {
    requireAuthoring(!name.empty() && !objects_.count(name), "SceneAuthoring: duplicate/empty object name.");
    requireAuthoring(setup.shape == GLShape::Cube || setup.shape == GLShape::Tetra || setup.shape == GLShape::Pyramid,
        "SceneAuthoring: use native Scene APIs for other model types.");
    requireAuthoring(finite(setup.size) && glm::all(glm::greaterThan(setup.size, glm::vec3(0))) &&
        finite(setup.position) && finite(setup.pivot) && finite(setup.scale), "SceneAuthoring: invalid object values.");
    const auto parentId = parent.empty() ? std::optional<EntityId>{} : std::optional<EntityId>{object(parent)};
    Transform3D pose;
    pose.basis = setup.position; pose.rotation = glm::mat3(rotation(setup.rotationDegrees));
    pose.scale = setup.scale; pose.localBasis = setup.pivot;
    const auto world = DiagramSystem::modelMatrix(pose);
    requireAuthoring(nonsingular(world),
        "SceneAuthoring: object scale must be invertible.");
    auto model = DiagramSystem::makePolyhedron(setup.shape, setup.size);
    for (auto& face : model.faces) face.material = setup.material;
    auto saved = app_.getScene();
    try {
        const auto id = app_.getScene().push_entity(model);
        requireAuthoring(id.has_value(), "SceneAuthoring: object creation failed.");
        requireAuthoring(app_.getScene().setParent(*id, parentId) && app_.getScene().setWorldPlacement(*id, world),
            "SceneAuthoring: parent/world placement failed.");
        objects_.emplace(name, *id);
        return *id;
    } catch (...) { app_.getScene() = std::move(saved); throw; }
}
void SceneAuthoring::addLayer(const std::string& objectName, const std::string& name, const LayerSetup& setup) {
    const auto id = object(objectName);
    requireAuthoring(!name.empty(), "SceneAuthoring: empty layer name.");
    std::size_t count = 0;
    for (const auto& layer : layers_) if (layer.object == objectName) {
        requireAuthoring(layer.name != name, "SceneAuthoring: duplicate object-local layer name."); ++count;
    }
    requireAuthoring(count < 128, "SceneAuthoring: at most 128 layers per object.");
    std::vector<glm::mat4> initial;
    for (const auto& layer : layers_) if (layer.object == objectName) initial.push_back(layerMatrix(layer.setup, 0));
    initial.push_back(layerMatrix(setup, 0));
    // Materialize initial layers before a later child computes its world-authored attachment.
    requireAuthoring(app_.getScene().setLocalLayers(id, initial), "SceneAuthoring: invalid initial local layers.");
    layers_.push_back({objectName, name, setup});
}
void SceneAuthoring::addLight(const std::string& name, const PointLight& light, const std::string& parent) {
    requireAuthoring(!name.empty() && lights_.size() < 8, "SceneAuthoring: empty name or more than eight point lights.");
    for (const auto& item : lights_) requireAuthoring(item.name != name, "SceneAuthoring: duplicate light name.");
    if (!parent.empty()) object(parent);
    requireAuthoring(finite(light.position) && validColor(light.color) && std::isfinite(light.intensity) &&
        light.intensity >= 0 && std::isfinite(light.attenuation) && light.attenuation >= 0, "SceneAuthoring: invalid point light.");
    lights_.push_back({name, parent, light});
}
void SceneAuthoring::setCamera(const Camera& camera, const std::string& parent) {
    if (!parent.empty()) object(parent);
    requireAuthoring(CameraSystem::validCamera(camera), "SceneAuthoring: invalid camera.");
    camera_ = camera; cameraParent_ = parent;
}
void SceneAuthoring::update(double seconds) {
    requireAuthoring(std::isfinite(seconds) && seconds >= 0, "SceneAuthoring: time must be finite and nonnegative.");
    std::map<EntityId, std::vector<glm::mat4>> matrices;
    for (const auto& layer : layers_) matrices[object(layer.object)].push_back(layerMatrix(layer.setup, seconds));
    for (const auto& item : matrices)
        requireAuthoring(app_.getScene().setLocalLayers(item.first, item.second), "SceneAuthoring: invalid composed local layers.");
    std::vector<PointLight> lights;
    for (const auto& item : lights_) {
        auto light = item.value;
        if (!item.parent.empty()) {
            const auto frame = app_.getScene().worldMatrix(object(item.parent));
            requireAuthoring(frame.has_value(), "SceneAuthoring: invalid light frame.");
            light.position = glm::vec3(*frame * glm::vec4(light.position, 1));
        }
        requireAuthoring(finite(light.position), "SceneAuthoring: light position overflow.");
        lights.push_back(light);
    }
    app_.getRenderOptions().pointLights = std::move(lights);
    if (camera_) {
        if (cameraParent_.empty()) app_.getCamera() = *camera_;
        else {
            const auto frame = app_.getScene().worldMatrix(object(cameraParent_));
            requireAuthoring(frame.has_value(), "SceneAuthoring: invalid camera frame.");
            const auto camera = CameraSystem::attached(*camera_, *frame);
            requireAuthoring(camera.has_value(), "SceneAuthoring: degenerate camera frame.");
            app_.getCamera() = *camera;
        }
    }
}
} // namespace graphics
````


## standard/framework/examples/source_authoring.cpp

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#include "../Framework.h"
#include <iostream>
#include <exception>

// Compile as an independent entrypoint with this folder's Framework.props.
int main() try {
    graphics::Application app;
    graphics::SceneAuthoring scene(app);
    double seconds = 0;
    app.setInitHandler([&](graphics::Application&) {
        scene.clear();
        graphics::ObjectSetup object;
        object.position = {0.f, 0.f, 0.f};
        object.scale = {2.f, 1.f, 0.5f};
        object.material.color = {0.3f, 0.6f, 0.9f};
        object.material.specularStrength = 0.7f;
        object.material.shininess = 64;
        scene.addObject("sample", object);
        graphics::LayerSetup spin;
        spin.spinDegreesPerSecond = 30;
        scene.addLayer("sample", "spin", spin);
        graphics::PointLight key;
        key.position = {3.f, 4.f, 5.f};
        key.intensity = 2;
        scene.addLight("key", key);
        graphics::Camera camera;
        scene.setCamera(camera);
        auto& render = app.getRenderOptions();
        render.lighting = true;
        render.directionalIntensity = 0;
        render.ambientColor = {0.15f, 0.15f, 0.15f};
        scene.update(0);
        return true;
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) { seconds += dt; scene.update(seconds); });
    graphics::ApplicationConfig config;
    config.title = "Native source authoring example";
    return app.run(config);
} catch (const std::exception& error) {
    std::cerr << error.what() << '\n'; return 1;
}
````


## standard/framework/include/SceneAuthoring.h

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
#pragma once
#include "Application.h"

namespace graphics {
// Direct C++ authoring data. No command parser or assembly file is required.
struct ObjectSetup {
    GLShape shape = GLShape::Cube;
    glm::vec3 size{1}, position{0}, rotationDegrees{0}, scale{1}, pivot{0};
    Material material;
};

struct LayerSetup {
    glm::vec3 translation{0}, rotationDegrees{0}, scale{1}, pivot{0};
    glm::vec3 spinAxis{0, 1, 0};
    double spinDegreesPerSecond = 0;
};

class SceneAuthoring {
    struct Layer { std::string object, name; LayerSetup setup; };
    struct Light { std::string name, parent; PointLight value; };
    Application& app_;
    std::map<std::string, EntityId> objects_;
    std::vector<Layer> layers_;
    std::vector<Light> lights_;
    std::optional<Camera> camera_;
    std::string cameraParent_;
public:
    explicit SceneAuthoring(Application& app) : app_(app) {}
    /// [API] Clear Scene and authored handles together; call before rebuilding the scene.
    void clear();
    /// [API] Create a cube/tetrahedron/pyramid at a world-authored pose; parent must already exist.
    EntityId addObject(const std::string& name, const ObjectSetup& setup, const std::string& parent = "");
    /// [API] Append an object-local layer. Later layers inherit all earlier layers.
    void addLayer(const std::string& object, const std::string& name, const LayerSetup& setup);
    /// [API] Register a point light, optionally expressed in a named object's local frame.
    void addLight(const std::string& name, const PointLight& light, const std::string& parent = "");
    /// [API] Select one camera, optionally expressed in a named object's local frame.
    void setCamera(const Camera& camera, const std::string& parent = "");
    /// [API] Evaluate local layers, attached lights and camera at absolute seconds; throws on invalid input.
    void update(double seconds);
    /// [API] Resolve a source-authored object name; throws if the name is absent.
    EntityId object(const std::string& name) const;
    /// [API] Resolve a picked object ID to its source-authored name, or return an empty string.
    std::string name(EntityId id) const;
};
} // namespace graphics
````


## standard/main.cpp

### 블록 1 · before L5 / after L5

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
/// 콜백을 등록하고 애플리케이션을 실행한다.
````

**after: 없음**



### 블록 2 · before L7 / after L6

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````cpp
    SetConsoleOutputCP(CP_UTF8);
````


### 블록 3 · before L9 / after L9

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
        sample.onKey(key, action, mods);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });
    app.setCursorHandler([&](graphics::Application&, double x, double y) {
        sample.onCursor(x, y);
    });
    app.setFocusHandler([&](graphics::Application&, bool focused) {
        sample.onFocus(focused);
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) {
        sample.onUpdate(dt);
    });
    app.setRenderHandler([&](graphics::Application&) {
        return sample.onRender();
    });
````

**after**

````cpp
    app.setInitHandler([&](graphics::Application&) { return sample.initScene(); });
    app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) { sample.onKey(key, action, mods); });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) { sample.onMouse(event); });
    app.setUpdateHandler([&](graphics::Application&, float dt) { sample.onUpdate(dt); });
    app.setRenderHandler([&](graphics::Application&) { return sample.onRender(); });
````


### 블록 4 · before L33 / after L15

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp

    config.title = "Standard - Scene workspace (mode 0)";
````

**after**

````cpp
    config.title = "Standard - Direct C++ source";
````


### 블록 5 · before L38 / after L18

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
    std::cerr << "standard 실행 실패: " << error.what() << '\n';
````

**after**

````cpp
    std::cerr << "Standard 실행 실패: " << error.what() << '\n';
````


### 블록 6 · before L41 / after L21

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````cpp
    std::cerr << "standard 실행 실패: 앞선 프레임워크 진단을 확인하세요.\n";
````

**after**

````cpp
    std::cerr << "Standard 실행 실패: 프레임워크 진단을 확인하세요.\n";
````


## standard/standard.vcxproj

### 블록 1 · before L48 / after L48

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````xml
    <ClCompile Include="modes\Assembly.cpp" />
    <ClCompile Include="modes\AssemblyCommands.cpp" />
    <ClCompile Include="modes\AssemblyConsole.cpp" />
    <ClCompile Include="modes\AssemblyDiagnostics.cpp" />
    <ClCompile Include="modes\AssemblyDynamics.cpp" />
    <ClCompile Include="modes\AssemblyExamples.cpp" />
    <ClCompile Include="modes\AssemblyExtensions.cpp" />
    <ClCompile Include="modes\AssemblyGuide.cpp" />
    <ClCompile Include="modes\AssemblyHierarchy.cpp" />
    <ClCompile Include="modes\AssemblyInput.cpp" />
    <ClCompile Include="modes\AssemblyMotion.cpp" />
    <ClCompile Include="modes\AssemblyObjects.cpp" />
    <ClCompile Include="modes\AssemblyParts.cpp" />
    <ClCompile Include="modes\AssemblyPaths.cpp" />
    <ClCompile Include="modes\AssemblyPersistence.cpp" />
    <ClCompile Include="modes\AssemblyRuntime.cpp" />
    <ClCompile Include="modes\AssemblySceneTools.cpp" />
    <ClCompile Include="modes\AssemblySurface.cpp" />
    <ClCompile Include="modes\AssemblyTracks.cpp" />
    <ClCompile Include="modes\AssemblyWorkspace.cpp" />
    <ClCompile Include="modes\Mode0Recipe.cpp" />
    <ClInclude Include="modes\Assembly*.h" />
    <None Include="docs\reference\modes\*.md" />
````

**after**

````xml
    <ClCompile Include="SceneSetup.cpp" />
````


### 블록 2 · before L72 / after L50

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````xml
    <ClInclude Include="SceneSetup.h" />
    <ClInclude Include="examples\SourceExamples.h" />
````


### 블록 3 · before L73 / after L53

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````xml
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Animation.h" />
    <ClInclude Include="framework\include\Transform.h" />
    <ClInclude Include="framework\include\Patterns.h" />
    <ClInclude Include="framework\include\GridLayout.h" />
    <ClInclude Include="framework\include\Diagnostics.h" />
````

**after: 없음**



### 블록 4 · before L82 / after L56

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````xml
  <ItemGroup>
    <AssemblyRecipe Include="examples\assembly\**\*.assembly" />
  </ItemGroup>
  <Target Name="CopyAssemblyLearningFiles" AfterTargets="Build">
    <Copy SourceFiles="@(AssemblyRecipe)" DestinationFiles="@(AssemblyRecipe->'$(TargetDir)examples\assembly\%(RecursiveDir)%(Filename)%(Extension)')" SkipUnchangedFiles="true" />
  </Target>
````

**after: 없음**



## tools/Update-StandardDocs.py

### 블록 1 · before L129 / after L129

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````python
    old = (STANDARD / 'DOCUMENT_INDEX.md').read_text(encoding='utf-8-sig')
````

**after**

````python
    index = STANDARD / 'DOCUMENT_INDEX.md'
    old = index.read_text(encoding='utf-8-sig') if index.exists() else '# Standard 문서 목차\n\n'
````


### 블록 2 · before L132 / after L133

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````python
    lines = ['[브라우저 목차](index.html) · [변환 템플릿](docs/TRANSFORM_TEMPLATES.html) · [README](README.md)', '',
````

**after**

````python
    lines = ['[브라우저 목차](index.html) · [C++ 예제](docs/EXAMPLES.md) · [README](README.md)', '',
````


### 블록 3 · before L202 / after L203

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````python
    for path in sorted((VIEWER_TEMPLATES / 'vendor').glob('*')):
        if path.is_file(): write(STANDARD / 'docs/viewer/vendor' / path.name, path.read_text(encoding='utf-8-sig'))
````


### 블록 4 · before L208 / after L211

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````python
    runpy.run_path(str(ROOT / 'tools/Write-WorkspaceDocs.py'))['refresh_documents']()
    refresh_api()
````

**after**

````python
    runpy.run_path(str(ROOT / 'tools/Write-SourceDebuggerDocs.py'))['refresh_documents']()
````


## tools/Update-StandardModesGraph.py

### 블록 1 · before L63 / after L63

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````python
    for name in ('basic', 'assets', 'motion'):
````

**after**

````python
    for name in ('basic', 'assets', 'motion', 'source_authoring'):
````


### 블록 2 · before L71 / after L71

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````python
                'x64', 'Debug', 'Release', '.obj-work', 'dist', 'tests'}
````

**after**

````python
                'x64', 'Debug', 'Release', '.obj-work', 'dist', 'tests', 'old_standard'}
````


### 블록 3 · before L82 / after L82

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````python
              ROOT / 'standard/framework/include/Graphics.h', ROOT / 'standard/framework/detail/Scene.cpp']
````

**after**

````python
              ROOT / 'standard/framework/include/Graphics.h', ROOT / 'standard/framework/detail/Scene.cpp',
              ROOT / 'standard/framework/include/SceneAuthoring.h', ROOT / 'standard/framework/detail/SceneAuthoring.cpp']
````


## tools/Write-SourceDebuggerDocs.py

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````python
"""Write native C++ authoring and source-debugger documentation. No build, tests or app run."""
from pathlib import Path
import html
import json
import re

ROOT = Path(__file__).resolve().parents[1]
REFERENCE = 'https://github.com/Jun1188/ComputerGraphics/tree/f42b28ab2cba2fb57f37bb67bb3738e27889765a'


def write(relative, text):
    path = ROOT / relative
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text.strip() + '\n', encoding='utf-8-sig')


DEBUGGER = r'''# Source debugger: 실제 C++ 소스 편집 안내

원하는 변화를 명령으로 입력하면 현재 디스크 소스의 절대 경로, 1부터 시작하는 행 번호, 수정 전 코드, 복사할 수정 후 코드를 출력한다. 사용자가 코드를 직접 편집하고 빌드한다. 프로그램은 대상 파일을 쓰지 않는다.

## 시작

`debugger/debugger.sln`에서 debugger를 빌드한다. 저장소 루트 `OpenGL-26-2.sln`에는 Standard와 debugger가 별도 프로젝트로 들어 있다. 각 프로젝트의 단독 솔루션도 사용할 수 있다. debugger는 OpenGL·GLFW·GLEW·Standard 실행 파일에 의존하지 않는 Windows C++17 콘솔 프로그램이다.

실행 시 프로젝트 폴더 또는 `.vcxproj` 경로를 인수 하나로 전달하거나 아래처럼 입력한다. 예제 경로는 자기 복사 위치로 바꾼다.

```text
project "C:/Work/OpenGL-26-2/standard/standard.vcxproj"
list
show object panel
object panel position 2 1 0
object panel scale 2 1 0.5
layer hinge pivot -1 0 0
layer hinge spin 30
light key position 2 4 1
object panel specular 0.8
camera main fov 55
render main ambient 0.2 0.2 0.2
```

위 명령은 수정 제안만 출력한다. `.assembly`를 만들거나 장면을 실시간 변경하지 않는다. 파일을 저장한 다음 같은 명령을 다시 입력하면 **변경한 소스의 행과 기존 값**을 다시 읽는다. IDE의 저장하지 않은 버퍼는 읽을 수 없다. 변경 감시는 명령을 처리하는 시점에 수행하며, 창을 띄워 계속 감시하는 기능은 아니다.

## 출력 계약

`[수정 제안] 실제 절대 경로:현재 행`, `[위치·의미] 구획/속성/좌표계/단위`, `[before] 디스크에서 읽은 코드와 행 번호`, `[after] 복사 가능한 C++ 코드`, `[주변 코드]`를 출력한다. 신규 코드에서는 실제 삽입 지점과 전체 등록 코드를 보여주고 before는 없다고 표시한다. 행 번호 예시를 고정 값으로 제공하지 않는다.

## 명령과 인수

| 명령 | 결과와 인수 의미 |
|---|---|
| `object 이름 position/rotation/size/scale/pivot X Y Z` | 객체의 월드 배치, Euler XYZ(도), 모델 크기, 비균등 배율, 모델 지역 피벗 대입문 제안 |
| `object 이름 color R G B` | 재질 RGB, 각 값 0~1 |
| `object 이름 ambient/diffuse/specular/shininess 값` | Ka/Kd/Ks 또는 정반사 지수 |
| `object 이름 unlit on/off` | 재질 조명 계산 생략 |
| `object 이름 shape box/tetra/pyramid` | 기본 형상 변경 |
| `layer 이름 translation/rotation/scale/pivot/axis X Y Z` | 선행 레이어 기준 지역 변환 또는 회전축 |
| `layer 이름 spin 도/초` | 절대 시간에서 평가하는 지역 회전 속도 |
| `light 이름 position X Y Z`, `light 이름 color R G B` | 점광원 위치/색. parent가 있으면 위치는 부모 지역 좌표 |
| `light 이름 intensity/attenuation 값` | 강도/거리 제곱 감쇠 계수, 0 이상 |
| `camera 이름 position/target/up X Y Z` | 시점/주목점/up. parent가 있으면 부모 지역 좌표 |
| `camera 이름 fov/near/far/extent 값` | 수직 화각/클립 거리/직교 투영 짧은 축 반길이 |
| `camera 이름 ortho on/off` | 직교 투영 또는 원근 투영 |
| `render main lighting/textures on/off` | 렌더 기능 사용 |
| `render main ambient/sun-color R G B` | 독립 주변광/방향광 색 |
| `render main sun-direction X Y Z`, `render main sun-intensity 값` | 표면에서 광원을 향하는 방향/방향광 강도 |
| `render main background R G B A` | 배경 RGBA |
| `object/light/camera 이름 parent 부모/none` | 등록 호출의 부모 인수 변경. 같은 파일의 앞 구획에 부모가 있어야 함 |
| `add object 이름 box/tetra/pyramid [부모]` | 객체 선언·모든 편집 속성·등록 호출의 전체 삽입안 |
| `add layer 이름 객체` | 지역 레이어 선언·속성·등록 호출 삽입안 |
| `add light 이름 [부모]`, `add camera 이름 [부모]` | 점광원 또는 활성 카메라 삽입안 |
| `find "문자열"` | 현재 소스에서 실제 파일·행·해당 줄 검색 |
| `code "파일" 시작행 끝행` | 실제 소스 행 범위 표시 |
| `replace "파일" 시작행 끝행 "코드"` | 임의 C++/GLSL/프로젝트 파일의 정확한 수정 전후 제안 |
| `insert "파일" 앞행 "코드"` | 해당 행 앞의 삽입안. 마지막 행+1은 파일 끝 |
| `help`, `quit` | 도움말, 종료 |

`replace/insert`의 코드 인수에서 `\n`은 줄바꿈으로 해석한다. 코드 안의 큰따옴표는 `\"`로 입력한다. Windows 폴더 경로는 따옴표로 감싸고 닫는 따옴표 앞의 불필요한 끝 역슬래시는 생략한다.

프레임워크 자체를 수정할 위치도 찾을 수 있다.

```text
project "C:/Work/OpenGL-26-2/framework"
find "SceneAuthoring::update"
find "uMaterialSpecular"
```

검색 결과에서 현재 행을 확인하고 `code` 또는 `replace`로 범위를 지정한다. `find` 결과에는 헤더 선언·사용부 등 여러 후보가 나올 수 있으며, 첫 결과를 자동 정답으로 선택하지 않는다.

## 구획 표시와 지원 경계

```cpp
// @debug begin light key
graphics::PointLight cfg_key;
cfg_key.position = {2.f, 4.f, 3.f};
cfg_key.color = {1.f, 1.f, 1.f};
cfg_key.intensity = 1.f;
cfg_key.attenuation = 0.02f;
scene.addLight("key", cfg_key);
// @debug end light key
```

이 표시는 C++ 주석이다. 실행·빌드에는 필요하지 않으며 디버거의 위치 기준으로만 사용한다. 종류+이름 구획은 선택한 프로젝트 안에서 유일해야 한다. 구획 이름은 ASCII 식별자를 쓴다. 속성 대입은 한 줄에 한 문장으로 작성한다. 프로그램은 현재 변수 이름을 대입문에서 읽는다. 구획이 중복되거나 대입이 여러 개면 추정하지 않고 거절한다. 구획이 없는 일반 실습은 `find/code/replace/insert`를 사용할 수 있다.

새 코드 삽입에는 `configureScene(graphics::SceneAuthoring& scene, graphics::RenderOptions& options)` 안의 독립된 `// @debug insert` 한 줄을 사용한다. 부모와 대상 객체를 먼저 등록한다. 새 코드를 수동으로 반영한 뒤 다음 명령을 입력해야 새 구획을 찾을 수 있다.

색인은 선택한 폴더의 `.cpp/.h/.hpp/.glsl/.props/.vcxproj` 텍스트를 읽는다. `.vcxproj` 입력은 해당 파일의 상위 폴더를 선택한다. MSBuild의 조건·매크로·실제 컴파일 목록을 평가하는 기능은 아니다. `dependencies`, `docs`, `old_standard`, 빌드/IDE/그래프 폴더와 링크·재분석 지점은 순회하지 않는다. 파일은 UTF-8(BOM 허용), 개별 8 MiB 이하여야 한다. 다른 인코딩이나 읽기 오류는 경로와 함께 거절한다.

이 프로그램은 지원 문법과 실제 텍스트를 연결하는 편집 보조 도구다. 자연어 해석기, C++ AST 분석기, 중단점/메모리 디버거 또는 컴파일러는 아니다. 임의 함수·매크로·여러 줄 초기화의 의미는 추론하지 않는다. 카메라 시선·near/far 등 속성 사이의 관계, 부모 실행 순서, 모델별 조건은 사용자가 소스와 실행으로 확인한다. 숫자의 기본 범위 검사만으로 컴파일·화면 성공을 보장하지 않는다.
'''


def refresh_documents():
    write('debugger/README.md', DEBUGGER)
    write('standard/docs/SOURCE_DEBUGGER.md', DEBUGGER)
    examples = (ROOT / 'standard/examples/SourceExamples.h').read_text(encoding='utf-8-sig')
    write('standard/docs/EXAMPLES.md', '''# C++ 변환·조명·카메라 예제

원문은 `examples/SourceExamples.h`다. 외부 assembly 파일이나 명령 문자열을 실행하지 않는 C++ 함수 두 개를 제공한다. 현재 Standard의 기본 장면 대신 확인하려면 `SceneSetup.cpp`에서 이 헤더를 include하고 configureScene 본문을 아래 호출 중 하나로 직접 바꾼 뒤 사용자가 재빌드한다.

```cpp
#include "examples/SourceExamples.h"
// configureScene 본문에서 하나를 선택한다.
source_examples::rotatingDisplay(scene, options);
// 또는 source_examples::attachedLightAndCamera(scene, options);
```

`rotatingDisplay`는 회전 지지대와 표시판이다. 객체 부모 상속, 비균등 배율, 왼쪽 끝 피벗 회전, 후행 지역 이동을 조합한다. `attachedLightAndCamera`는 회전하는 운반대에 광원과 카메라를 붙여 고정 조형물을 관찰한다. 원래 실습의 사물 이름만 바꾼 재출제 예측이 아니라 연산·시간·기준 프레임을 조합하는 예다.

둘 다 시간0 등록 후 `SceneAuthoring::update(seconds)`로 갱신한다. 부모의 등록된 지역 레이어는 0초에서 먼저 반영되어 나중에 등록하는 자식이 지정 월드 배치에서 시작한다. 등록 함수는 장면 초기화 단계에서 사용한다. 런타임에서 레이어를 추가하면 기존 지역 레이어가 우선 0초로 초기화되므로 다음 update까지의 중간 상태를 별도 동작으로 사용하지 않는다.

이 헤더는 참조 함수이며 독립 main을 추가하지 않는다. 구획 주석을 쓰는 기본 SceneSetup.cpp는 구조화 debugger 명령 대상이다. 예제 함수로 교체한 뒤에는 find/code/replace/insert를 사용하거나 편집할 코드에 유일한 @debug 구획을 직접 추가한다. 예제의 실행 결과는 아직 확인하지 않았다.

프레임워크 자체의 별도 main 예제는 `framework/examples/source_authoring.cpp`다. 이를 별도 프로젝트의 진입점으로 선택하고 그 폴더의 Framework.props를 import한다. Standard의 main과 동시에 컴파일하지 않는다.

## 전체 예제 코드

```cpp
''' + examples.rstrip() + '\n```')
    write('standard/README.md', '''# Standard: C++ 직접 작성 프로젝트

`SceneSetup.cpp`에 객체·변환·조명·재질·카메라를 C++ 코드로 작성한다. 수정 후 사용자가 빌드·실행한다.

[전체 문서](index.html) · [작성 흐름](docs/DIRECT_CPP_WORKFLOW.md) · [두 물체 튜토리얼](docs/WORKSPACE_TUTORIAL.md) · [디버거 명령](docs/SOURCE_DEBUGGER.md) · [조명](docs/LIGHTING_AND_MATERIALS.md) · [사용자 확인](USER_CHECKS.md)

- `standard.sln`: 현재 직접 작성 프로젝트. 루트 솔루션에는 별도 debugger 프로젝트도 있다.
- `SceneSetup.cpp`: 사용자가 직접 고치는 장면 설정. 문자열 명령·외부 assembly 파일을 실행하지 않는다.
- `Console.cpp`: 초기화·마우스 객체 이름 확인·시간 갱신·렌더 콜백.
- `framework/`: 이 폴더 안의 배포 사본. 상위·형제 폴더 없이 빌드/실행에 사용한다.
- `old_standard/`: 저장소의 형제 폴더에 보존한 이전 명령·레시피 버전. 그 폴더의 `standard.sln`으로 별도 연다.

클릭은 선택한 객체 이름과 `show object 이름` 안내만 출력한다. Space는 시간 정지/재개, Home은 시간 초기화, Esc는 종료다. 객체 속성은 코드에서 편집한다. 물체 전체 선택이 기본이며 면 편집 명령 모드는 현재 Standard 실행에 포함하지 않는다. 필요하면 기존 면 변환 C++ API를 직접 사용한다.

`debugger`에 원하는 변경을 입력하면 현재 소스의 파일·행·수정 전후 코드가 출력된다. 소스 저장, 재빌드, 화면 확인은 사용자가 수행한다. 빌드·테스트·앱 실행은 에이전트가 수행하지 않았다.
''')
    write('standard/docs/DIRECT_CPP_WORKFLOW.md', '''# C++ 직접 작성과 실행 흐름

작업의 원본은 `SceneSetup.cpp`이다. 명령·레시피가 C++ 원본을 대신하지 않는다.

## 입력 → 상태/변환 → 출력

1. 사용자가 `ObjectSetup`, `LayerSetup`, `PointLight`, `Camera`, `RenderOptions` 값을 직접 편집한다.
2. `main`은 Application 콜백을 한 번 등록한다. `run` 초기화 후 `Console::initScene`이 `configureScene`을 호출한다.
3. `SceneAuthoring::addObject`는 월드 배치를 부모 기준 attachment에 저장한다. 비균등 배율 결과를 SRT로 다시 분해하지 않는다.
4. `Console::onUpdate`가 절대 시간을 누적하고 `SceneAuthoring::update`가 지역 레이어와 부모를 따르는 광원·카메라를 평가한다. Application의 기존 Scene 모션 갱신은 그 뒤 한 번 실행된다.
5. `Console::onRender` → `Application::renderScene` → Renderer가 모델·정규행렬·광원·재질을 GPU에 전달한다. Scene의 월드/면 행렬은 렌더와 피킹이 함께 사용한다.
6. 종료 시 Application이 GPU·창·GLFW 수명을 정리한다. debugger는 이 실행 흐름에 참여하지 않는다.

## 편집 위치

| 바꿀 내용 | 작성 위치 / API |
|---|---|
| 객체 생성·위치·회전·비균등 배율·모델 피벗·재질 | `SceneSetup.cpp` object 구획 / `ObjectSetup`, `addObject` |
| 부모 연결 | `scene.addObject("자식", 설정, "부모")`, 부모를 먼저 등록 |
| 지역 레이어와 지속 회전 | layer 구획 / `LayerSetup`, `addLayer` |
| 독립 주변광·방향광·배경 | render 구획 / `RenderOptions` |
| 점광원·부모 부착 | light 구획 / `PointLight`, `addLight` |
| 투영·시점·부모 부착 | camera 구획 / `Camera`, `setCamera` |
| 특정 기믹의 상태·힘·입력 법칙 | 독립 컨트롤러 또는 `Console.cpp` 콜백에서 기존 공통 API 호출 |

새 사물마다 프레임워크 enum이나 명령 파서를 늘리지 않는다. 기본 형상 도우미는 box/tetra/pyramid를 지원한다. 구·원뿔·모델·면·경로·힘 등은 기존 Scene/PatternSystem/Motion/Dynamics API로 직접 연결할 수 있다. 이들 모두를 디버거의 구조화 명령으로 변환한다고 주장하지 않는다. 그런 소스는 `find/code/replace/insert`로 위치와 코드를 안내받는다.

`SceneAuthoring::update`가 자신에게 등록한 객체의 지역 레이어, 점광원 목록, 등록 카메라를 관리한다. 같은 상태에 별도 컨트롤러를 동시에 쓰면 덮어쓸 수 있다. 네이티브 모션은 별도 객체에 연결하거나 해당 상태의 소유자를 명확히 정한다. `clear` 후 이전 EntityId는 재사용하지 않는다.

각 프레임의 잘못된 입력은 예외로 Application 종료 경로에 전달한다. `addObject` 생성 실패는 생성 전 Scene으로 복원한다. 다른 등록 함수는 값을 확인한 뒤 목록에 추가한다. `update`는 여러 객체에 대한 트랜잭션이 아니며 중간 실패 시 계속 진행하지 않는다.

## 배포 경계

Standard는 자체 framework·셰이더·헤더·라이브러리·실행 DLL을 가진다. debugger는 자체 소스와 Toolchain.props만 사용한다. 합본 솔루션의 프로젝트 경로는 편의용이며 각 단독 솔루션의 빌드/실행 의존성은 아니다. 실제 소스 위치는 debugger 실행 인수나 project 명령으로 선택한다. 폴더를 다른 위치로 복사한 뒤 빌드·실행하는 확인은 사용자가 수행한다.
''')
    write('standard/docs/TRANSFORM_REFERENCE.md', '''# 객체 월드 배치와 지역 레이어

`ObjectSetup`은 `size`로 모델을 만든 뒤 다음 월드 배치를 지정한다.

```text
D = T(position) × Rz × Ry × Rx × S(scale) × T(-pivot)
attachment = inverse(parentWorld) × D
W = parentWorld × attachment × L1 × L2 × ...
```

기본 SRT는 단위행렬이며 attachment에 배치가 들어간다. 부모는 자식 등록 전에 존재해야 한다. 연결 순간에는 지정한 월드 배치를 유지하고 이후 부모 움직임을 상속한다. `size`는 모델 형상 크기, `scale`은 변환 배율이므로 지역 레이어의 이동에도 scale의 영향이 남는다.

각 `LayerSetup`의 행렬은 다음과 같다.

```text
L = T(translation + pivot) × Rz × Ry × Rx × Rspin(seconds) × S(scale) × T(-pivot)
```

translation과 pivot은 선행 레이어의 지역 좌표다. spin은 초당 도 단위이며 `spinAxis`를 정규화한다. 회전 순서를 명시하고 절대 시간에서 계산하므로 프레임별 각도 누적을 사용하지 않는다. 객체 pivot은 `T(-pivot)`으로 모델 원점을 재지정한다. 지역 레이어 pivot은 `T(pivot)...T(-pivot)`으로 해당 점 주위 변환을 수행한다. 피벗 숫자만 수정할 때 기존 화면 배치를 자동 보존하지 않는다.

비균등 배율과 회전을 계층으로 합성한 전단을 행렬 그대로 유지한다. 0 배율, 유한하지 않은 값, 특이 변환은 거절한다. `addLayer` 등록 순서가 의존 순서다. 특정 사물·시험 문제에 고정된 그룹이 아니다.

부모가 있는 점광원의 position과 카메라 position/target/up은 부모 지역 좌표다. 점광원은 점을 월드로 변환한다. 카메라는 `CameraSystem::attached`로 시선/up을 직교화한다. 광원 색·강도·거리 감쇠는 형상 배율과 별개다.
''')
    write('standard/docs/WORKSPACE_TUTORIAL.md', '''# 두 물체를 C++ 코드로 작성하기

디버거 명령으로 수정안을 받고 사용자가 소스를 편집하는 순서다. 아래 명령 자체는 Standard 장면을 변경하지 않는다.

## 1. 부모와 자식의 월드 배치

기본 `SceneSetup.cpp`는 base를 `(0,0,0)`, panel을 `(0,1,0)`에 생성하고 panel을 base에 연결한다. 두 값은 등록 순간의 월드 기준 배치다.

```text
show object base
show object panel
object panel position 3 1 0
```

출력한 실제 파일·행의 after를 직접 반영하고 저장한다. panel 등록 시 base의 현재 월드 행렬 역행렬로 attachment를 계산한다. 초기 base 위치를 바꾸어 재빌드해도 panel은 자신이 지정한 월드 배치에서 시작한다. 이후 실행 중 base가 변하면 panel은 그 변화를 상속한다.

## 2. 비균등 배율과 피벗

```text
object panel scale 2 1 0.5
layer hinge pivot -1 0 0
layer hinge rotation 0 90 0
```

각 명령의 before가 현재 소스와 같은지 확인한 뒤 수동으로 반영한다. panel의 형상 크기는 기본 `(2,1,0.2)`다. object scale과 hinge 지역 회전은 서로 다른 행렬이다. 비균등 배율 뒤의 지역 회전은 전단을 포함할 수 있다. 객체 자체의 단순 월드 Y 회전을 원하면 layer 회전 대신 object rotation을 편집한다.

## 3. 시간 회전과 지역 이동 레이어

```text
layer hinge rotation 0 0 0
layer hinge spin 30
add layer shift panel
```

shift 전체 코드를 실제 삽입 지점에 반영한 뒤 다음 명령을 입력한다.

```text
layer shift translation 1 0 0
```

힌지 회전을 shift가 상속하므로 이동 방향은 회전한다. shift를 먼저 적용하려면 C++의 두 `scene.addLayer` 호출 순서를 바꾼다. 피벗·배율·이동의 순서를 눈으로 예측하고 사용자가 빌드 후 관찰한다. Space는 시간 정지, Home은 0초 복귀다.

## 4. 조명과 카메라

```text
light key position 2 4 1
light key intensity 2
object panel specular 0.8
object panel shininess 128
camera main position 8 5 10
camera main target 0 1 0
```

부모에 광원을 붙이려면 `light key parent base`의 수정안을 반영한다. 이후 position은 부모 지역 좌표다. 재빌드 후 광원 방향, 표면 법선, 카메라 위치가 정반사에 주는 영향을 각각 관찰한다. 이 문서의 화면 결과는 에이전트가 실행해 확인하지 않았다.

회상 질문: 같은 지역 X 이동 1이 object scale과 앞 레이어의 회전 뒤에서 어떤 월드 방향·거리로 나타나는가?
''')
    write('standard/docs/LIGHTING_AND_MATERIALS.md', f'''# 조명·재질·텍스처를 코드에서 편집하기

추가 개념의 참고 저장소는 [Jun1188/ComputerGraphics]({REFERENCE})다. 이번 대응은 확인한 커밋 `f42b28a`를 기준으로 기록한다. 향후 기능은 해당 주제의 실제 소스를 다시 확인한 뒤 추가한다.

## 확인한 원본과 대응

| 참고 소스 | 확인한 개념 | 현재 C++ 작성 위치 |
|---|---|---|
| [조명 fragment.glsl]({REFERENCE}/컴그_조명까지/컴그17/fragment.glsl) | 주변광·확산광·reflect 기반 정반사, 광원/시점/물체 색, shininess | RenderOptions, PointLight, ObjectSetup.material |
| [조명 ComputerGraphics17.cpp]({REFERENCE}/컴그_조명까지/컴그17/ComputerGraphics17.cpp) | CPU에서 광원 위치·색 uniform 전달 | Renderer의 기존 uniform 전달, SceneSetup.cpp의 light 구획 |
| [텍스처 vertex.glsl]({REFERENCE}/컴그_29번_텍스처매핑/컴그17/vertex.glsl) | 모델 역전치 법선 변환과 UV 전달 | 기존 uNormalMatrix, TexCoord 계약 |
| [텍스처 fragment.glsl]({REFERENCE}/컴그_29번_텍스처매핑/컴그17/fragment.glsl) | 텍스처 샘플과 조명 결과 결합 | Material.texture/useTexture와 RenderOptions.textures |

원본을 그대로 복제하지 않는다. 확인한 조명 버전은 vertex의 FragPos가 vec3인데 fragment에서는 vec4이며, 그 vertex는 법선을 모델 공간 그대로 전달한다. 텍스처 버전은 FragPos가 vec3으로 일치하고 역전치 법선 변환을 사용한다. 현재 프레임워크는 기존 CPU 정규행렬 전달을 유지한다. 두 원본의 정상 실행 여부는 확인하지 않았다.

## 직접 편집할 값

- `render main`: lighting, 독립 ambientColor, 방향광 lightDirection/lightColor/directionalIntensity, 배경. 기본 예제는 방향광 강도 0으로 점광원의 효과를 구분한다.
- `light key`: position, color, intensity, attenuation. 기본 셰이더는 점광원 최대 8개이며 감쇠는 `1 / (1 + attenuation × distance²)`다.
- `object panel`: material.color, ambientStrength, diffuseStrength, specularStrength, shininess, unlit. unlit는 조명을 생략한다.
- `camera main`: position이 시점 벡터와 정반사 방향 계산에 반영된다. FOV·near/far·직교 투영은 별도다.

현재 셰이더는 ambientColor와 재질 Ka를 곱한 독립 주변광에 기존 방향광의 주변광·확산·정반사 항을 더한다. 방향광에는 기존 0.75 확산 계수가 있고 점광원에는 거리 감쇠를 적용한다. 이 기존 식을 참고 저장소의 수치와 완전히 동일하다고 설명하지 않는다. 정확한 식은 `framework/detail/Shaders.h`와 `framework/shaders/FragmentShader.glsl`을 따른다.

## 텍스처의 직접 코드 경로

파일 로딩은 Application의 AssetLoader와 기존 모델 API를 사용한다. 디버거의 `render main textures on`만으로 이미지나 UV가 생성되지 않는다. Material.texture에 유효한 TextureImage를 연결하고 useTexture를 켜며, 메시 UV와 RenderOptions.textures를 함께 설정해야 한다. 현재 구조화 명령은 이미지 로딩 코드 전체를 생성하지 않는다. `find/code/replace/insert`와 기존 모델 예제를 이용한다.

그림자·스포트라이트·PBR·접촉/강체 솔버는 이번에 구현한 기능이 아니다. 앞으로 추가할 때 실제 참고 파일, CPU 상태, 셰이더 입력, 직접 편집 위치, 사용자 관찰 절차를 함께 문서화한다.
''')
    write('standard/USER_CHECKS.md', '''# 사용자가 수행할 빌드·테스트·화면 확인

에이전트는 소스 작성·소스 열람·문서/구조 자료 생성만 수행했다. 아래 항목은 실행하지 않았다.

1. 루트 `OpenGL-26-2.sln`을 Visual Studio에서 열고 Standard와 debugger를 각각 빌드한다. Standard 컴파일 목록에 Assembly*.cpp가 없고 SceneSetup.cpp와 framework/detail/SceneAuthoring.cpp가 있는지 확인한다.
2. debugger를 실행하고 `project "자신의 경로/standard/standard.vcxproj"`, `list`, `show light key`, `light key position 5 6 7`을 입력한다. 절대 경로·실제 행·before의 기존 position·after `{5.f, 6.f, 7.f}`를 소스와 대조한다. 생성 숫자는 유효한 동등 float 표기로 나올 수 있다.
3. 소스 구획 앞에 빈 줄 두 개를 넣어 저장한 뒤 같은 명령을 반복한다. 안내 행은 두 줄 이동해야 한다. position 기존 값을 수동 수정해 저장한 뒤 before도 새 값으로 바뀌는지 확인한다. 대상 소스가 프로그램에 의해 저장되지 않는지도 확인한다.
4. `add object gate box base`의 전체 after를 안내 위치에 반영한다. 저장 후 `show object gate`, `object gate position 3 1 0`이 새 구획과 실제 변수를 찾는지 확인한다.
5. `layer hinge spin 30`을 수동 반영하고 Standard를 재빌드·실행한다. 부모/지역 레이어의 회전을 관찰한다. Space 정지·Home 시간 초기화·클릭의 객체 이름 출력을 확인한다.
6. `light key parent base`와 조명 위치·강도·재질 Ks/N 수정안을 수동 반영한다. 재빌드해 광원과 재질의 효과를 각각 관찰한다. 카메라 변경 시 position/target/up과 near/far가 함께 유효한지 확인한다.
7. 잘못된 숫자, 0 배율, 0 회전축, 없는 구획, 중복 구획, 여러 대입문, 존재하지 않는 파일/행을 입력한다. 잘못된 위치를 성공으로 안내하지 않고 오류를 표시해야 한다. 임시로 바꾼 사용자 소스는 직접 복원한다.
8. `find "SceneAuthoring::update"`를 framework 대상으로 실행하고 실제 결과를 `code`로 읽는다. 구획이 없는 일반 실습에도 find/code/replace/insert의 before가 실제 파일과 같은지 확인한다. 이 명령은 MSBuild 활성 분기나 C++ 의미를 판정하지 않는다.
9. 한글·공백 경로에서 UTF-8 BOM 파일을 읽고 EOF/quit로 종료한다. 링크·재분석 지점 아래로 순회하지 않는지 확인한다.
10. standard 폴더와 debugger 폴더를 각각 다른 위치로 복사해 단독 솔루션 빌드·실행을 확인한다. Standard의 framework 배포 사본과 DLL/셰이더 복사 결과를 확인한다.

`old_standard`는 이전 프로젝트를 보존한 폴더다. 기존 standard 내부에 x64/.vs가 없었으므로 이 이동에서 삭제된 용량은 0바이트다. 저장소 루트의 .vs, 다른 실습의 x64, 사용자 docs.zip·PDF·기존 Renderer.cpp 변경은 정리 범위에서 제외했다.
''')
    api = '''# SceneAuthoring: C++ 직접 작성 API

원본은 `include/SceneAuthoring.h`, 구현은 `detail/SceneAuthoring.cpp`다. Standard에 동일한 배포 소스를 포함한다.

| API | 계약 |
|---|---|
| ObjectSetup / addObject | box/tetra/pyramid, 모델 크기, 월드 배치·비균등 배율·피벗·재질. 부모를 먼저 등록 |
| LayerSetup / addLayer | 객체 소유 지역 레이어, T(p+pivot) Rz Ry Rx Rspin S T(-pivot), 등록 순서 상속 |
| PointLight / addLight | 최대8 점광원. 선택 부모가 있으면 지역 위치에서 월드로 변환 |
| Camera / setCamera | 하나의 활성 카메라. 선택 부모에 부착, 시선/up 직교화 |
| update(seconds) | 절대 시간으로 레이어·광원·카메라 평가. 프레임 dt를 직접 전달하지 않음 |
| object(name) / name(id) | 이름↔Scene 엔티티 ID 연결. 없는 이름은 예외, 없는 ID는 빈 이름 |
| clear | Scene과 작성 핸들을 함께 초기화. 이전 ID를 재사용하지 않음 |

직접 C++로 설정하고 사용자 콜백에서 갱신한다. 소스 편집용 주석은 debugger만 읽으며 런타임 의존성이 아니다. 새로운 사물의 상태·법칙은 독립 컨트롤러에서 기존 변환·Motion·Dynamics API로 연결한다. 재질과 셰이더의 기존 광원 계산을 재사용하며 이번 작업은 새로운 그림자·강체 솔버를 추가하지 않는다.

자세한 작성 순서와 확인 절차는 Standard의 DIRECT_CPP_WORKFLOW.md, SOURCE_DEBUGGER.md, LIGHTING_AND_MATERIALS.md를 참조한다. 소스·문서 반영과 컴파일·실행 성공은 구분한다. 빌드·테스트는 사용자가 수행한다.
'''
    for base in ('framework', 'standard/framework'):
        write(base + '/docs/SOURCE_AUTHORING.md', api)
        write(base + '/README.md', '''# Modern OpenGL C++ framework

이 폴더 전체를 복사하고 Framework.props를 프로젝트에서 import한다. C++17·Windows x64·Visual Studio C++ 도구와 Windows SDK가 필요하다. 헤더·라이브러리·런타임 DLL·셰이더·라이선스는 폴더 안의 배포 사본을 사용한다.

현재 작성 방식은 [SceneAuthoring C++ API](docs/SOURCE_AUTHORING.md)다. ObjectSetup·LayerSetup·PointLight·Camera 값을 소스에서 직접 작성한다. 프레임워크에 명령 파서나 assembly 파일 실행 의존성을 추가하지 않는다. 낮은 수준의 Scene·TransformSystem·PatternSystem·Motion·Dynamics API도 그대로 직접 사용할 수 있다.

[기존 공통 API 레퍼런스](docs/API_REFERENCE.html)는 계속 제공한다. 과거 Standard 명령·레시피·모드 설명은 이전 버전 자료이며 저장소의 old_standard에 보존한다. 현재 Standard의 작성 흐름은 SceneSetup.cpp와 새 문서를 따른다. 기존 명령 예제가 새 Standard에서 실행된다고 간주하지 않는다.

조명은 기존 방향광·점광원·독립 주변광·재질 Ka/Kd/Ks/shininess와 법선 변환을 사용한다. 추가 개념은 사용자가 지정한 ComputerGraphics 저장소의 해당 소스를 확인한 뒤 반영한다. 빌드·실행·화면 확인은 사용자가 수행한다.
''')
        page = ROOT / base / 'docs/API_REFERENCE.html'
        if page.exists():
            text = page.read_text(encoding='utf-8-sig')
            text = re.sub(r'<!-- current-assembly-api:start -->.*?<!-- current-assembly-api:end -->', '', text, flags=re.S)
            text = re.sub(r'<!-- source-authoring:start -->.*?<!-- source-authoring:end -->', '', text, flags=re.S)
            section = '<!-- source-authoring:start --><section><h2>C++ 직접 작성 API</h2><p>현재 Standard는 SceneSetup.cpp를 직접 편집한다. 과거 명령·레시피 설명은 old_standard 자료다.</p><p><a href="SOURCE_AUTHORING.md">계약과 작성 흐름</a></p><pre><code>'
            section += html.escape((ROOT / base / 'include/SceneAuthoring.h').read_text(encoding='utf-8-sig')) + '</code></pre></section><!-- source-authoring:end -->'
            text = re.sub(r'(<body\b[^>]*>)', lambda m: m[0] + section, text, count=1)
            write(base + '/docs/API_REFERENCE.html', text)
    catalog = {'schemaVersion': 1, 'id': 'standard-document-catalog', 'source': 'tools/Write-SourceDebuggerDocs.py',
        'uncategorized': {'id': 'other', 'title': '공통 API·기존 참고 자료', 'description': '현재 작성 흐름은 시작하기 문서를 우선한다. 과거 명령 예제는 old_standard용이다.'},
        'categories': [{'id': 'start', 'title': 'C++ 직접 작성과 편집 안내', 'description': '수동 소스 편집 → 사용자 빌드·실행',
            'documents': [{'path': path, 'title': title, 'featured': True} for path, title in [
                ('README.md', 'Standard 시작'), ('docs/DIRECT_CPP_WORKFLOW.md', '작성 위치와 실행 흐름'),
                ('docs/SOURCE_DEBUGGER.md', 'debugger 명령과 실제 소스 위치'), ('docs/WORKSPACE_TUTORIAL.md', '두 물체 C++ 튜토리얼'),
                ('docs/TRANSFORM_REFERENCE.md', '월드 배치·지역 레이어'), ('docs/EXAMPLES.md', 'C++ 예제 전체 코드'), ('docs/LIGHTING_AND_MATERIALS.md', '조명·재질·참고 소스'),
                ('framework/docs/SOURCE_AUTHORING.md', '공통 작성 API'), ('USER_CHECKS.md', '사용자 확인 절차')]]}]}
    write('standard/docs/DOCUMENT_CATALOG.json', json.dumps(catalog, ensure_ascii=False, indent=2))


if __name__ == '__main__':
    refresh_documents()
````


## tools/Write-SourceDebuggerReport.py

### 블록 1 · before L1 / after L1

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before: 없음**


**after**

````python
"""Generate complete source before/after and architecture reports, without builds or tests."""
from pathlib import Path
import collections
import difflib
import hashlib
import json
import re
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BASE = '01db4ff48ade5f8d8941110408852762cb0f3f64'
OUT = ROOT / 'docs/source-debugger'
GRAPH = ROOT / 'graphify-out/source-debugger'
OWNED = ('framework/', 'standard/', 'old_standard/', 'debugger/', 'docs/source-debugger/',
         'graphify-out/source-debugger/', 'tools/templates/standard-viewer/')
TOOLS = {'tools/Write-SourceDebuggerDocs.py', 'tools/Write-SourceDebuggerReport.py',
         'tools/Update-StandardDocs.py', 'tools/Update-StandardModesGraph.py', 'OpenGL-26-2.sln'}


def git(*args):
    return subprocess.check_output(['git', '-c', 'core.quotePath=false', *args], cwd=ROOT)


def write(path, text):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text.rstrip() + '\n', encoding='utf-8-sig')


def code(text, language):
    fence = '`' * max(4, max((len(x) for x in re.findall(r'`+', text)), default=0) + 1)
    return fence + language + '\n' + text.rstrip('\n') + '\n' + fence + '\n'


def before(name):
    name = 'standard/' + name[len('old_standard/'):] if name.startswith('old_standard/') else name
    p = subprocess.run(['git', 'show', BASE + ':' + name], cwd=ROOT, capture_output=True)
    return p.stdout.decode('utf-8-sig').replace('\r\n', '\n') if p.returncode == 0 else ''


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    names = set(git('diff', '--no-renames', '--name-only', BASE, '--').decode('utf-8').splitlines())
    names.update(git('ls-files', '--others', '--exclude-standard').decode('utf-8').splitlines())
    names = sorted(n for n in names if n in TOOLS or n.startswith(OWNED))
    manifest = []
    report = ['# 전체 코드 변경 전후', '', '기준 커밋 `' + BASE + '`에서 이번 저장된 소스까지의 모든 변경 블록이다.', '',
              '이전 프로젝트의 이동은 old_standard의 파일을 원래 standard 경로와 비교한다. 내용이 같은 이동은 코드 변경으로 중복 표기하지 않는다. '
              '자동 생성 문서·그래프·기존 SDK 복사본은 manifest에 기록하고 코드 블록 대조에서는 제외한다. 빌드·테스트·검증기·앱은 실행하지 않았다.', '']
    kinds = {'.cpp': 'cpp', '.h': 'cpp', '.hpp': 'cpp', '.glsl': 'glsl', '.py': 'python',
             '.props': 'xml', '.vcxproj': 'xml', '.sln': 'text'}
    blocks = 0
    for name in names:
        path = ROOT / name
        if path == OUT / 'ALL_CODE_BEFORE_AFTER.md' or path == OUT / 'CHANGE_MANIFEST.json': continue
        data = path.read_bytes() if path.is_file() else b''
        moved = name.startswith('old_standard/')
        if not path.exists() and name.startswith('standard/'):
            moved = (ROOT / 'old_standard' / name[len('standard/'):]).is_file()
        manifest.append({'path': name, 'exists': path.is_file(), 'bytes': len(data),
                         'sha256': hashlib.sha256(data).hexdigest() if path.is_file() else None,
                         'preserved_project_relocation': moved})
        language = kinds.get(path.suffix)
        if name.startswith('tools/templates/') and path.suffix in {'.html', '.js', '.css'}: language = path.suffix[1:]
        if not language or '/dependencies/' in name or '/vendor/' in name: continue
        if not path.is_file() and moved: continue
        left = before(name).splitlines(keepends=True)
        right = data.decode('utf-8-sig').replace('\r\n', '\n').splitlines(keepends=True) if data else []
        groups = list(difflib.SequenceMatcher(None, left, right, autojunk=False).get_grouped_opcodes(0))
        if not groups: continue
        report += ['## ' + name, '']
        for n, group in enumerate(groups, 1):
            first, last = group[0], group[-1]
            i, j, a, b = first[1], last[2], first[3], last[4]
            old, new = ''.join(left[i:j]), ''.join(right[a:b])
            blocks += 1
            report += [f'### 블록 {n} · before L{i+1} / after L{a+1}', '',
                       '변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. '
                       '함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.', '',
                       '**before**' if old else '**before: 없음**', '', code(old, language) if old else '',
                       '**after**' if new else '**after: 없음**', '', code(new, language) if new else '', '']
    write(OUT / 'ALL_CODE_BEFORE_AFTER.md', '\n'.join(report))
    write(OUT / 'CHANGE_MANIFEST.json', json.dumps({'baseline': BASE, 'changed_code_blocks': blocks,
        'source_state': 'Saved source during final documentation stage; hashes include current generated artifacts.',
        'excluded_preexisting_user_items': ['openGL_3dAssign_26/framework/detail/Renderer.cpp', 'docs.zip',
            'lecture_pdf/homework/*', 'tools/__pycache__/*'], 'files': manifest}, ensure_ascii=False, indent=2))

    projects = json.loads((GRAPH / 'PROJECTS.json').read_text(encoding='utf-8-sig'))
    graph = json.loads((GRAPH / 'graph.json').read_text(encoding='utf-8-sig'))
    source = json.loads((GRAPH / 'SOURCE_MANIFEST.json').read_text(encoding='utf-8-sig'))
    nodes = {n['id']: n for n in graph['nodes']}
    links = graph.get('links', graph.get('edges', []))
    degree = collections.Counter()
    for link in links:
        degree[link['source']] += 1; degree[link['target']] += 1
    lines = ['# 소스 구조와 실행 프로젝트', '',
        f"graphify AST: {source['nodes']} nodes / {source['edges']} edges / {len(source['files'])} source files.", '',
        '코드 전용 AST 추출이며 의미 분석 API 토큰 사용은 0이다. 코드 관계를 소스와 함께 읽는 자료다. '
        '빌드·테스트·그래프 진단기·인코딩 검증기·앱·브라우저를 실행하지 않았다. 동적 콜백의 실제 실행이나 화면을 입증하지 않는다.', '',
        '## 현재 Standard', '',
        '1. main이 Console과 Application을 만들고 초기화·키·마우스·업데이트·렌더 콜백을 등록한다. 등록은 실행이 아니다.',
        '2. Application::run 초기화 후 Console::initScene → SceneAuthoring::clear → configureScene → addObject/addLayer/addLight/setCamera → update(0).',
        '3. 입력은 Application의 이벤트 전달 뒤 Console::onKey/onMouse로 들어온다. 클릭은 pickAt의 결과 EntityId를 이름으로 바꾸어 출력한다. 명령 파서와 assembly 런타임은 없다.',
        '4. Console::onUpdate가 절대 시간을 계산해 SceneAuthoring::update를 호출한다. 지역 레이어를 평가한 뒤 Scene::worldMatrix로 부착 광원·카메라를 평가한다.',
        '5. Application의 기존 Scene 모션 갱신 후 Console::onRender → Application::renderScene → Renderer의 CPU 정점/인덱스 업로드와 VAO/VBO/EBO·uniform·draw 호출로 이어진다.',
        '6. Shaders.h 또는 외부 VertexShader/FragmentShader가 모델/정규행렬·조명·재질·텍스처를 처리한다. 프레임 종료 뒤 run이 GPU·창·GLFW를 정리한다.', '',
        '## 별도 debugger', '',
        '1. wmain은 UTF-8 콘솔을 설정하고 인수 또는 project 명령으로 대상 폴더를 고른다. 독립 실행 프로그램이며 Standard의 메모리/렌더러를 제어하지 않는다.',
        '2. command는 문자열을 토큰화하고 SourceIndex::refresh로 실제 디스크 소스를 다시 읽는다. 시작/끝 구획과 현행 행 번호를 만든다.',
        '3. set/parent는 유일한 현재 대입/등록 줄을 찾고, add는 실제 @debug insert 앞의 전체 C++ 삽입안을 만든다. find/code는 일반 소스 위치를 제공한다.',
        '4. proposal은 실제 before와 생성한 after, 경로·행·주변 코드를 stdout에 출력한다. 대상 파일 저장·프로세스 실행·컴파일 호출은 없다.',
        '5. EOF 또는 quit로 끝난다. Windows 파일 경로/콘솔 API와 C++ 표준 기능만 사용하며 OpenGL SDK 링크는 없다.', '',
        '## 전체 독립 프로젝트와 진입점', '',
        '각 행은 별도 실행 대상이다. 같은 이름의 Console/main을 하나의 호출 흐름으로 합치지 않는다. '
        'old_standard는 보존한 과거 프로젝트로 등록만 기록하고 현재 AST 추출에서는 제외한다. 프레임워크 예제의 main도 동시에 컴파일하지 않는다.', '',
        '| 프로젝트/독립 예제 | 확인한 진입점 |', '|---|---|']
    for project in projects:
        name = project.get('project', project.get('file', 'example'))
        if project.get('available') is False:
            entries = '작업 전 부재, 이전 기록만 존재'
        else:
            entries = '; '.join(f"{e['file']}:{e.get('line', '?')} {e.get('name', 'main')}" for e in project.get('entrypoints', []))
            if not entries and project.get('standalone_example'): entries = project['file'] + ' main'
            if not entries: entries = '프로젝트 파일/소스의 조건부 진입점을 별도 확인'
        lines.append('| ' + name + ' | ' + entries + ' |')
    lines += ['', '## God Nodes', '', 'AST 연결 차수다. 이름 중복과 동적 호출을 고려해 실행 순서로 해석하지 않는다.', '',
              '| 노드 | 연결 차수 | 소스 |', '|---|---:|---|']
    for node, count in degree.most_common(8):
        item = nodes.get(node, {})
        lines.append('| ' + str(item.get('label', node)).replace('|', '/') + ' | ' + str(count) + ' | ' + str(item.get('source_file', '')) + ' |')
    lines += ['', '## Surprising Connections', '',
        '- 렌더와 피킹은 Scene의 월드/면 행렬을 공유한다. 새 작성 API는 그 앞의 attachment와 지역 레이어만 구성한다. 별도 피킹 좌표 계산기를 추가하지 않았다.',
        '- 광원·카메라 부착은 기존 Scene 월드 행렬과 CameraSystem::attached를 재사용한다. C++ 구획 주석은 GPU나 실행 상태에 참여하지 않는다.',
        '- debugger는 프레임워크를 링크하지 않지만 생성 코드가 실제 공개 API를 호출한다. SourceIndex는 컴파일러의 활성 분기나 AST 의미를 판정하지 않는다.', '',
        '## Suggested Questions', '',
        '- 객체의 월드 배치를 지정한 뒤 부모의 지역 레이어가 회전하면 자식과 광원은 어떤 행렬을 상속하는가?',
        '- source 파일에 빈 줄을 넣었을 때 debugger의 before와 행 번호는 어디서 다시 계산되는가?', '',
        '## 산출물과 미확인 범위', '',
        'graph.json/graph.html/EXTRACTION.json/PROJECTS.json/SOURCE_MANIFEST.json을 함께 보존한다. '
        '빌드·런타임·화면·폴더 분리 실행 확인은 standard/USER_CHECKS.md에 따라 사용자가 수행한다.']
    write(OUT / 'ARCHITECTURE.md', '\n'.join(lines))
    write(GRAPH / 'GRAPH_REPORT.md', '\n'.join(lines))
    write(OUT / 'PLAN.md', '''# 직접 C++ 작성과 source debugger 개편

- 날짜: 2026-11-10
- 승인 범위: 프레임워크 원본과 Standard 직접 작성 전환, 별도 debugger, 실제 소스의 경로·행·before/after 안내, 조명·재질·카메라와 문서·튜토리얼·예제 개편.
- 추가 승인: 기존 Standard를 old_standard로 이동하고 그 안의 x64/.vs만 삭제. 두 폴더가 없어 실제 삭제·회수 용량은 0바이트.
- 기존 사용자 Renderer.cpp 변경, docs.zip, PDF, 기존 pycache는 수정·삭제·커밋하지 않음.

| 단계 | 커밋 | 내용 |
|---|---|---|
| 1 | b6daa40 | SceneAuthoring 원본/배포 사본, 직접 C++ 작성 API |
| 2 | 14fefc4 | old_standard 보존, 새 Standard의 SceneSetup·콜백·컴파일 목록 |
| 3 | d0ac31a | debugger, 실제 소스 색인·수정안·합본/단독 솔루션 |
| 4 | 50d73c5 | 튜토리얼·예제·조명 문서·목차·생성기, 초기 부모 레이어 적용 |
| 5 | 최종 구조/보고 커밋 | graphify AST, 프로젝트 진입점, 전체 before/after·manifest |

문서 생성: python -B tools/Update-StandardDocs.py. 코드 작성·문서 생성·구조 자료 생성은 수행했고, 컴파일·테스트·검증기·앱 실행·화면 확인은 수행하지 않았다.

새 debugger는 지원 속성과 구획 표시를 사용한다. 일반 C++/GLSL은 find/code/replace/insert로 실제 행의 수정안을 안내한다. 임의 자연어·C++ 의미 분석·소스 자동 저장·자동 컴파일은 구현하지 않았다. 추가 개념은 지정된 Jun1188/ComputerGraphics의 해당 실제 소스를 참고한다.

별도 삭제할 임시파일을 만들지 않았다. graphify-out/source-debugger의 추출/캐시는 구조 자료와 재생성 근거로 보존한다. 삭제 후보로 사용자 자료를 임의 분류하지 않는다.
''')
    print(f'Generated complete before/after ({blocks} blocks), source manifest and architecture reports. No builds/tests run.')


if __name__ == '__main__':
    main()
````


## tools/templates/standard-viewer/index.html

### 블록 1 · before L13 / after L13

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````html
  <a href="docs/TRANSFORM_TEMPLATES.html">변환 템플릿</a>
  <a href="docs/API_REFERENCE.html">API</a>
````

**after**

````html
  <a href="document.html?doc=docs/SOURCE_DEBUGGER.md">소스 디버거</a>
  <a href="framework/docs/API_REFERENCE.html">API</a>
````


### 블록 2 · before L20 / after L20

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````html
  <p class="lead">0번 작업공간의 짧은 명령, 월드 배치·객체 상속·지역 레이어, 조명·카메라·힘 응용과 실습 소스 참조를 모았습니다. Markdown 문서는 인터넷 없이 이 창에서 바로 미리보기로 열립니다.</p>
````

**after**

````html
  <p class="lead">C++ 직접 작성, 실제 소스의 편집 위치를 안내하는 debugger, 월드 배치·지역 레이어·조명·카메라 예제를 모았습니다. Markdown 문서는 인터넷 없이 이 창에서 읽을 수 있습니다.</p>
````


### 블록 3 · before L23 / after L23

변경 이유: PLAN.md의 해당 단계에 따라 직접 C++ 작성, 실제 소스 편집 안내, 문서/프로젝트 구성 또는 구조 보고를 반영한다. 함수/클래스 이름과 호출부는 아래 실제 변경 코드 및 ARCHITECTURE.md에서 확인한다.

**before**

````html
    <input id="search" type="search" placeholder="제목·설명·경로로 검색 (예: 탱크, face-track, P19)" aria-label="문서 검색">
````

**after**

````html
    <input id="search" type="search" placeholder="제목·설명·경로로 검색 (예: 소스, 조명, 레이어)" aria-label="문서 검색">
````
