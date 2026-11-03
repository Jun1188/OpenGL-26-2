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
        << "surface <객체> <면/all> | mount <객체> <관절> 면 삼각형 U V 거리 | anchor <객체> <관절> 면 XYZ\n"
        << "normal <객체> <면/all> XYZ 또는 auto/flip | winding <객체> <면/all> flip | warp <group> SX SY SZ\n"
        << "face-track <id> <객체> 면 시간 지연 once/loop/pingpong move/scale/turn ... | guide face-track\n"
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
        {"mount", {"modes/AssemblySurface.cpp", "std::string Assembly::mount(", "객체·면·삼각형 좌표 관절 부착과 프레임 추적"}},
        {"normal", {"modes/AssemblySurface.cpp", "if (command == \"normal\")", "조명 노멀·winding·면별 정점 데이터"}},
        {"geometry", {"modes/AssemblySurface.cpp", "void Assembly::model(", "사용자 Model 등록, 삼각형·정점·면 변환"}},
        {"warp", {"modes/AssemblySurface.cpp", "if (command == \"warp\")", "외곽 group의 비균일 배율과 반사"}},
        {"face-track", {"modes/AssemblyTracks.cpp", "bool Assembly::trackCommand(", "지연·반복·왕복 면 이동/축소/임의 축 회전"}},
        {"cpp-effect", {"modes/AssemblyRuntime.cpp", "} else if (b.effect == Effect::Custom)", "사용자 C++ 프레임 동작과 기준 SRT 합성"}},
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
    if (topic == "anchor" || topic == "unmount") topic = "mount";
    if (topic == "winding" || topic == "orient") topic = "normal";
    if (topic == "triangle" || topic == "triangle-add" || topic == "surface" || topic == "vertex" || topic == "face-srt" || topic == "face-mode" || topic == "facecolor" || topic == "outline" || topic == "line" || topic == "digit") topic = "geometry";
    if (topic == "effects" || topic == "render") topic = "input";
    if (topic.size() == 10 && topic.compare(0, 8, "practice") == 0) {
        const std::map<std::string, Location> practices{
            {"practice13", {"modes/Mode10Practice13.cpp", "bool Practice13::initScene()", "면 선택·무작위 선택 상태를 customCommand/Custom 동작으로 옮기기"}},
            {"practice14", {"modes/Mode11Practice14.cpp", "void Practice14::onUpdate(", "입력 상태·형상 교체·로컬 회전"}},
            {"practice15", {"modes/FaceCommon.cpp", "void FaceMode::applyFaceMotion()", "한 면의 복합 행렬을 하나의 FaceTrack Motion에 조립"}},
            {"practice16", {"modes/Mode13Practice16.cpp", "void Practice16::applyPyramid(", "순차 펼침과 다중 면 동기화"}},
            {"practice17", {"modes/Mode14Practice17.cpp", "void Practice17::onUpdate(", "교환·우회·사인 배율·조건 전이"}},
            {"practice18", {"modes/Mode15Practice18.cpp", "void Practice18::onUpdate(", "기울어진 궤도·달 중심·축과 반지름"}},
            {"practice19", {"modes/TankCommon.cpp", "void TankMode::onUpdate(", "탱크 부품·이동 상태·카메라 추적"}},
            {"practice20", {"modes/Mode17Practice20.cpp", "void Practice20::onUpdate(", "거리 기반 컨베이어·택배 간격·접선 자세"}},
            {"practice21", {"modes/Mode18Practice21.cpp", "void Practice21::onUpdate(", "드래그 방 회전·다중 상자/공 이탈 상태"}},
            {"practice22", {"modes/TankCommon.cpp", "bool TankMode::onRender()", "동일 상태의 3뷰 렌더와 카메라"}},
            {"practice23", {"modes/Mode20Practice23.cpp", "void Practice23::onUpdate(", "보행 충돌·장애물 난수 배치·벽 개방 상태"}}};
        if (const auto found = practices.find(topic); found != practices.end()) { show(found->second, out); return; }
    }
    if (topic == "truck-pattern") topic = "truck";
    if (topic == "tank-pattern") topic = "tank";
    if (topic == "person-pattern") topic = "person";
    if (topic == "group" || topic == "joint" || topic == "part") topic = "hierarchy";
    if (topic == "add" || topic == "parent" || topic == "color" || topic == "show") topic = "objects";
    if (topic == "spin" || topic == "orbit" || topic == "path" || topic == "linear" || topic == "pendulum" || topic == "remove") topic = "motion";
    if (topic == "wall") topic = "face";
    if (topic == "walk" || topic == "jump" || topic == "obstacle" || topic == "body" || topic == "release") topic = "physics";
    if (topic == "bind" || topic == "unbind" || topic == "drag") topic = "input";
    if (topic == "load") topic = "save";
    const auto found = locations.find(topic);
    if (found == locations.end()) {
        out << "지원 항목:";
        for (const auto& item : locations) out << ' ' << item.first;
        out << " practice13~23\n";
        return;
    }
    show(found->second, out);
}
} // namespace assembly
