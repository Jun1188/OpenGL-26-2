#include "Assembly.h"
#include "GuideRecipes.h"
#include <algorithm>
#include <fstream>

namespace assembly {
namespace {
std::filesystem::path projectRoot() {
    std::wstring binary(32768, L'\0');
    const DWORD count = GetModuleFileNameW(nullptr, binary.data(), DWORD(binary.size()));
    if (count > 0 && count < binary.size()) {
        binary.resize(count);
        auto path = std::filesystem::path(binary).parent_path();
        for (int i = 0; i < 6 && !path.empty(); ++i, path = path.parent_path())
            if (std::filesystem::is_regular_file(path / "modes/Assembly.h")) return path;
    }
    const auto source = std::filesystem::u8path(__FILE__).parent_path().parent_path();
    if (std::filesystem::is_regular_file(source / "modes/Assembly.h")) return source;
    return {};
}

void showSite(const GuideSite& site, std::ostream& out) {
    const auto root = projectRoot();
    const auto path = root.empty() ? std::filesystem::u8path(site.file) : root / site.file;
    std::ifstream input(path, std::ios::binary);
    if (!input) { out << path.u8string() << " : 소스가 없어 현재 줄 확인 불가. 표식=" << site.marker << '\n'; return; }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(input, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (lines.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
        lines.push_back(line);
    }
    out << "편집 위치: " << site.purpose << '\n';
    std::size_t matches = 0;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        if (lines[i].find(site.marker) == std::string::npos) continue;
        ++matches;
        if (matches > 4) continue;
        out << path.u8string() << ':' << i + 1 << '\n';
        const auto begin = i > 1 ? i - 2 : 0;
        const auto end = std::min(lines.size(), i + 4);
        for (auto n = begin; n < end; ++n) out << (n == i ? " > " : "   ") << n + 1 << " | " << lines[n] << '\n';
    }
    if (!matches) out << path.u8string() << " : 표식이 바뀌었습니다. 현재 줄을 추정하지 않습니다. 검색어=" << site.marker << '\n';
    if (matches > 1) out << "동일 표식 " << matches << "곳. 함수/부모 문맥을 비교하세요(최대4곳 표시).\n";
}
void showRecipe(const GuideRecipe& recipe, std::ostream& out) {
    out << "guide " << recipe.topic << ": " << recipe.goal << '\n';
    for (const auto& site : recipe.sites) showSite(site, out);
    out << "\n변경 예시 before (작성 기준값; 현재 코드는 위 출력 우선):\n" << recipe.before
        << "\nafter 예시:\n" << recipe.after << "\n변경 이유/단위/제약:\n" << recipe.reason
        << "\n디버깅-수정 순서:\n" << recipe.debug << "\n예상 관찰:\n" << recipe.expected << '\n';
    const auto root = projectRoot();
    out << "참고: " << (root.empty() ? std::filesystem::u8path(recipe.document) : root / recipe.document).u8string() << '\n'
        << "예시는 자동 패치가 아닙니다. 명령 예시는 콘솔/.assembly에서 실행합니다. C++ 예시는 표시한 함수/삽입 문맥에 맞춰 편집합니다.\nC++ 수정 후 빌드·실행 확인은 사용자가 수행합니다.\n";
}
}

void Assembly::help(std::ostream& out) const {
    out << "0번 조립 콘솔. 명세: docs/assembly/SPEC.md, 사용법: docs/assembly/USER_GUIDE.md\n"
        << "build tank/truck/person <이름> : C++ 예제 생성. 확장은 guide tank/truck/person으로 직접 편집\n"
        << "surface <객체> <면/all> | mount <객체> <관절> 면 삼각형 U V 거리 | anchor <객체> <관절> 면 XYZ\n"
        << "normal <객체> <면/all> XYZ 또는 auto/flip | winding <객체> <면/all> flip | warp <group> SX SY SZ\n"
        << "face-track <id> <객체> 면 시간 지연 once/loop/pingpong move/scale/turn ... | guide face-track\n"
        << "feature add <이름> 1~22 | feature parts <이름> | feature port <원본> <nativeID> <이름> | feature snapshot ...\n"
        << "feature focus/camera <이름/none> | feature play/enable <이름> on/off | Ctrl+F5 / hotload <파일> / reload\n"
        << "diagnose <객체> | guide <기능> : 현재 줄·변경 전후 예·이유·디버깅 단계\n"
        << "group <이름> X Y Z | part box <부모> <부품> 위치3 크기3 | joint <부모> <관절> 위치3\n"
        << "srt <그룹> 위치3 회전도3 균일배율 | pivot <그룹> center 또는 위치3 [keep/raw]\n"
        << "add <종류> <이름> X Y Z [크기3] | list | inspect <이름> | select <이름>\n"
        << "move/position <이름> X Y Z | rotate <이름> x/y/z 도 | scale <이름> 값 | parent <자식> <부모/none>\n"
        << "spin <id> <대상> 축 도/초 | path <id> <대상> 1~6 초 | pendulum <id> <대상> 축 진폭 초\n"
        << "linear <id> <대상> DX DY DZ 초 [once/loop/pingpong] | orbit <id> <대상> 축 도/초 중심3\n"
        << "face <id> <대상> 면 spin/orbit 축 도/초 또는 face <id> <대상> 면 hinge 모서리 도/초 | wall <id> <방> 면0~3 초\n"
        << "physics <id> <상자> <방> | walk <id> <walker> <방> 속도 | jump <보행id> 속도\n"
        << "bind RIGHT hold \"move hero 1 0 0\" | bind J press \"jump walking 5.5\" | unbind RIGHT\n"
        << "play/enable <id> on/off | seek <id> 초 | remove <id> | pause on/off | view 1/3\n"
        << "camera ... | render wire/lighting/depth on/off | save/load \"파일.assembly\" | clear\n"
        << "guide mount/normal/warp/face-track/cpp-effect/practice13~23 또는 guide <객체이름>, guide feature:<기능>\n"
        << "창: 클릭 선택, Tab/Backspace 부품, XYZ 축, 방향키 편집, Space 정지, Home 기본 레시피 복원\n";
}


void Assembly::guide(const std::string& requested, std::ostream& out) const {
    const bool feature = requested.compare(0, 8, "feature:") == 0;
    std::string topic = feature ? requested.substr(8) : requested;
    if (const auto found = state_.nodes.find(topic); !feature && found != state_.nodes.end()) {
        inspect(topic, out);
        std::optional<graphics::EntityId> id = found->second.id;
        topic = "objects";
        while (id) {
            bool matched = false;
            for (const auto& item : state_.nodes) if (item.second.id == *id && item.second.whole) {
                topic = !item.second.guideTopic.empty() ? item.second.guideTopic :
                    findGuideRecipe(item.second.kind) ? item.second.kind : "objects";
                matched = true; break;
            }
            if (matched) break;
            const auto* component = app_.getScene().getComponent(*id);
            id = component ? component->parent : std::nullopt;
        }
    }
    if (const auto* recipe = findGuideRecipe(topic)) { showRecipe(*recipe, out); return; }
    out << "지원 항목:";
    for (const auto& recipe : guideRecipes()) out << ' ' << recipe.topic;
    out << "\n객체 이름과 충돌하면 guide feature:<기능>을 사용하세요.\n";
}
} // namespace assembly
