#include "Assembly.h"
#include <fstream>

namespace assembly {
namespace {
struct Location { const char* file; const char* marker; const char* edit; };

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

void show(const Location& location, std::ostream& out) {
    const auto root = projectRoot();
    const auto path = root.empty() ? std::filesystem::u8path(location.file) : root / location.file;
    out << "문서: " << (root.empty() ? std::filesystem::path("docs/assembly/CODE_GUIDE.md") : root / "docs/assembly/CODE_GUIDE.md").u8string() << '\n';
    std::ifstream source(path, std::ios::binary);
    std::string line;
    std::size_t number = 0;
    bool found = false;
    while (std::getline(source, line)) {
        ++number;
        if (line.find(location.marker) != std::string::npos) {
            out << path.u8string() << ':' << number << "\n  " << line << '\n';
            found = true;
            break;
        }
    }
    if (!found) out << path.u8string() << " : 소스/표식을 찾지 못해 현재 줄 번호를 확인할 수 없습니다.\n";
    out << "응용 조합: docs/assembly/PATTERN_COMBINATIONS.md\n";
    out << "수정 항목: " << location.edit << "\n레시피는 다시 load하고, C++ 수정 후 빌드·실행은 사용자가 수행하세요.\n";
}
}

void Assembly::help(std::ostream& out) const {
    out << "0번 조립 콘솔. 명세: docs/assembly/SPEC.md, 사용법: docs/assembly/USER_GUIDE.md\n"
        << "build tank/truck/person <이름> : C++ 예제 생성. 확장은 guide tank/truck/person으로 직접 편집\n"
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
        << "guide objects/motion/face/physics/input/camera/recipe/custom 또는 guide <객체이름>\n"
        << "창: 클릭 선택, Tab/Backspace 부품, XYZ 축, 방향키 편집, Space 정지, Home 기본 레시피 복원\n";
}

void Assembly::guide(const std::string& requested, std::ostream& out) const {
    std::string topic = requested;
    if (const auto found = state_.nodes.find(topic); found != state_.nodes.end()) {
        inspect(topic, out);
        std::optional<graphics::EntityId> id = found->second.id;
        topic = "objects";
        while (id) {
            bool matched = false;
            for (const auto& item : state_.nodes) {
                if (item.second.id == *id && item.second.whole) {
                    const auto& kind = item.second.kind;
                    if (!item.second.guideTopic.empty()) { topic = item.second.guideTopic; matched = true; break; }
                    if (kind == "humanoid" || kind == "swing" || kind == "tank" || kind == "walker" || kind == "grid" || kind == "room" || kind == "group" || kind == "joint")
                        topic = kind;
                    matched = true;
                    break;
                }
            }
            if (matched) break;
            const auto* component = app_.getScene().getComponent(*id);
            id = component ? component->parent : std::nullopt;
        }
    }
    const std::map<std::string, Location> locations{
        {"objects", {"modes/AssemblyObjects.cpp", "void Assembly::add(", "객체 종류 분기와 크기·색·부품 등록"}},
        {"humanoid", {"framework/detail/Animation.cpp", "std::optional<HumanoidRig> AnimationSystem::addHumanoid", "사람 비율·관절 연결점·머리/팔/다리 생성"}},
        {"swing", {"framework/detail/Animation.cpp", "std::optional<SwingRig> AnimationSystem::addSwingWithRider", "그네 지지대·줄·좌석·탑승자 연결"}},
        {"tank", {"modes/AssemblyExamples.cpp", "void buildTankExample(", "육면체 기반 차체·궤도·포탑·포신의 계층과 동작"}},
        {"tank-preset", {"modes/AssemblyParts.cpp", "TankParts addTankParts(", "기존 실습19·22와 공유하는 탱크 부품"}},
        {"walker", {"framework/detail/Patterns.cpp", "std::optional<JointRig> PatternSystem::addWalkingRig", "보행 리그의 몸체 비율·관절 연결점"}},
        {"grid", {"modes/AssemblyParts.cpp", "std::vector<graphics::EntityId> addGridParts(", "셀 배치·크기 비율·교대 색"}},
        {"room", {"modes/AssemblyObjects.cpp", "} else if (kind == \"room\")", "방 경계와 벽 메시 생성"}},
        {"hierarchy", {"modes/AssemblyHierarchy.cpp", "void Assembly::group(", "group/part/joint 조립 API와 최상위 변환"}},
        {"srt", {"modes/AssemblyHierarchy.cpp", "void Assembly::srt(", "최상위 위치·회전·균일배율을 한 번에 설정"}},
        {"pivot", {"modes/AssemblyHierarchy.cpp", "void Assembly::pivot(", "중심점 변경과 기준 배치 보존"}},
        {"center", {"modes/AssemblyHierarchy.cpp", "glm::vec3 Assembly::center(", "하위 메시 전체의 로컬 AABB 중심 계산"}},
        {"patterns", {"docs/assembly/PATTERN_COMBINATIONS.md", "# 기능 조합 패턴", "탱크·트럭·사람의 계층 조립과 C++ 수정 위치"}},
        {"truck", {"modes/AssemblyExamples.cpp", "void buildTruckExample(", "차체·바퀴·컨테이너·문 관절 조합"}},
        {"person", {"modes/AssemblyExamples.cpp", "void buildPersonExample(", "육면체·관절로 몸체·팔·다리 조합"}},
        {"motion", {"modes/AssemblyMotion.cpp", "bool Assembly::motionCommand(", "경로·회전·진자·면 동작 설정"}},
        {"face", {"modes/AssemblyMotion.cpp", "} else if (command == \"face\"", "힌지·자전·공전 종류, 면/모서리, 속도"}},
        {"physics", {"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Body)", "중력·충돌 경계·이탈/보행 계산"}},
        {"input", {"modes/AssemblyInput.cpp", "bool Assembly::inputCommand(", "키 이름·수정키·press/hold 문법과 동작"}},
        {"camera", {"modes/AssemblyInput.cpp", "if (a[1] == \"pose\")", "카메라 이동·투영·시야각과 줌"}},
        {"recipe", {"modes/Mode0Recipe.cpp", "USER_RECIPE_BEGIN", "기본 조립 명령 문자열을 추가·수정"}},
        {"custom", {"modes/AssemblyExtensions.cpp", "USER_COMMAND_BEGIN", "새 명령 분기와 typed Assembly API 호출"}},
        {"runtime", {"modes/AssemblyRuntime.cpp", "bool Assembly::applyPoses(", "프레임별 합성 순서와 기능 업데이트"}},
        {"save", {"modes/AssemblyPersistence.cpp", "bool Assembly::save(", "레시피 기록과 기준 SRT 저장 형식"}}};
    if (topic == "tank-pattern") topic = "tank";
    if (topic == "person-pattern") topic = "person";
    if (topic == "group" || topic == "joint" || topic == "part") topic = "hierarchy";
    if (topic == "add" || topic == "parent" || topic == "color" || topic == "show") topic = "objects";
    if (topic == "spin" || topic == "orbit" || topic == "path" || topic == "linear" || topic == "pendulum" || topic == "remove") topic = "motion";
    if (topic == "wall") topic = "face";
    if (topic == "walk" || topic == "jump" || topic == "obstacle") topic = "physics";
    if (topic == "bind" || topic == "unbind") topic = "input";
    if (topic == "load") topic = "save";
    const auto found = locations.find(topic);
    if (found == locations.end()) {
        out << "지원 항목: objects humanoid swing tank walker grid room hierarchy patterns truck person motion face physics input camera recipe custom runtime save\n";
        return;
    }
    show(found->second, out);
}
} // namespace assembly
