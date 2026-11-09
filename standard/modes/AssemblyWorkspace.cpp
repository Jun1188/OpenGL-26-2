#include "Assembly.h"
#include <algorithm>
#include <iomanip>
#include <sstream>

namespace assembly {
namespace {
std::string decimal(float value) {
    std::ostringstream out; out << std::setprecision(9) << value; return out.str();
}
Tokens pointTokens(glm::vec3 p) { return {decimal(p.x), decimal(p.y), decimal(p.z)}; }
bool affine(const glm::mat4& m) {
    return graphics::TransformSystem::validAffine(m) && std::abs(glm::determinant(glm::mat3(m))) > 1e-18f;
}
glm::mat4 matrixTokens(const Tokens& a, std::size_t start) {
    require(a.size() == start + 16, "행렬은 열 우선 16개 값입니다.");
    glm::mat4 m(1);
    for (int c=0;c<4;++c) for (int r=0;r<4;++r) m[c][r] = float(number(a[start+c*4+r]));
    require(affine(m), "역행렬이 있는 유한 아핀 행렬이 필요합니다.");
    return m;
}
}

Tokens Assembly::resolveCommand(const Tokens& input) const {
    auto a = input;
    if (a[0] == "add" && a.size() >= 2 && (a.size() <= 3 || a[2] == "at" || (a.size() > 3 && a[3] == "at"))) {
        std::string name = a[1]; unsigned suffix = 1;
        while (state_.nodes.count(name + std::to_string(suffix))) ++suffix;
        name += std::to_string(suffix);
        std::size_t at = 2;
        if (a.size() > 2 && a[2] != "at") { name = a[2]; at = 3; }
        glm::vec3 point = state_.cursor;
        if (a.size() > at) {
            require(a[at] == "at", "생성 위치 앞에 at을 사용하세요.");
            if (a.size() == at+2) require(a[at+1] == "cursor", "at cursor 또는 at X Y Z");
            else { require(a.size() == at+4,"at cursor 또는 at X Y Z"); point = vector(a,at+1); }
        }
        a = {"add",a[1],name}; const auto p=pointTokens(point); a.insert(a.end(),p.begin(),p.end());
    }
    if (a[0] == "parent" && a.size() == 2) a.insert(a.begin()+1, state_.selected);
    if ((a[0] == "inspect" || a[0] == "diagnose" || a[0] == "center") && a.size() == 1)
        a.push_back(state_.selected);
    if (a[0] == "pivot" && a.size() >= 2 && a[1] == "cursor") {
        require(!state_.selected.empty(), "먼저 물체를 클릭하세요.");
        const auto m = app_.getScene().worldMatrix(node(state_.selected).id);
        require(m && affine(*m), "피벗 좌표 변환 실패.");
        const auto p = pointTokens(glm::vec3(glm::inverse(*m) * glm::vec4(state_.cursor,1)));
        a = {"pivot", state_.selected}; a.insert(a.end(),p.begin(),p.end()); a.push_back("keep");
    } else if (a[0] == "pivot" && a.size() == 2 && a[1] == "center") {
        a.insert(a.begin()+1,state_.selected);
    }
    if (a[0] == "insert" && a.back() == "cursor") {
        a.pop_back(); const auto p = pointTokens(state_.cursor); a.insert(a.end(),p.begin(),p.end());
    }
    if (a[0] == "t" || a[0] == "r" || a[0] == "s" || a[0] == "layer" || a[0] == "local" || a[0] == "insert") {
        require(!state_.selected.empty(), "먼저 물체를 클릭하세요. select <이름>도 가능합니다.");
        Tokens scoped{"with",state_.selected,state_.selectedLayer.empty() ? "none" : state_.selectedLayer};
        scoped.insert(scoped.end(),a.begin(),a.end()); return scoped;
    }
    return a;
}

void Assembly::setWorld(const std::string& target, const glm::mat4& matrix) {
    auto& object = node(target);
    const auto* driver = positionDriver(target);
    require(!object.mount && (!driver || (driver->effect != Effect::Body && driver->effect != Effect::Walk && driver->effect != Effect::Custom)),
            "물리/사용자 제어 루트 또는 면 관절의 배치는 제어 함수를 통해 변경하세요.");
    auto& scene = app_.getScene();
    const auto current = scene.getComponent(object.id)->attachment;
    const auto animation = glm::inverse(object.attachment) * current;
    require(affine(animation) && scene.setWorldPlacement(object.id,matrix), "월드 배치 실패: 부모/배율을 확인하세요.");
    object.attachment = scene.getComponent(object.id)->attachment * glm::inverse(animation);
}

bool Assembly::workspaceCommand(const Tokens& a, std::ostream& out) {
    const auto& command = a[0];
    if (command == "with") {
        require(a.size() >= 4, "with <객체> <레이어/none> <명령...>");
        node(a[1]); state_.selected = a[1]; state_.selectedLayer = a[2] == "none" ? "" : a[2];
        return dispatch(Tokens(a.begin()+3,a.end()),out);
    }
    if (command == "attachment") {
        require(a.size() == 18,"attachment <객체> <열 우선 행렬16>");
        node(a[1]).attachment = matrixTokens(a,2); return true;
    }
    if (command == "edit") {
        require(a.size() == 2 && (a[1] == "object" || a[1] == "part" || a[1] == "face"),"edit object/part/face");
        state_.editMode = a[1]; state_.faceSelection.reset(); return true;
    }
    if (command == "cursor") {
        require(a.size() == 1 || a.size() == 4,"cursor [X Y Z]");
        if (a.size() == 4) state_.cursor = vector(a,1);
        out << "cursor [생성 기준점] - 월드 XYZ=" << state_.cursor.x << ',' << state_.cursor.y << ',' << state_.cursor.z << '\n';
        return true;
    }
    if (command == "plane") {
        require(a.size() == 3,"plane <xy/xz/yz> <해당 평면의 고정 좌표>");
        require(a[1] == "xy" || a[1] == "xz" || a[1] == "yz","평면은 xy/xz/yz입니다.");
        state_.planeNormal = a[1] == "xy" ? glm::vec3(0,0,1) : a[1] == "xz" ? glm::vec3(0,1,0) : glm::vec3(1,0,0);
        state_.planePoint = state_.planeNormal * float(number(a[2])); return true;
    }
    if (command == "snap") {
        require(a.size() == 2,"snap <간격. 0은 끄기>");
        const float value = float(number(a[1])); require(value >= 0 && value <= 1000,"간격은 0~1000입니다.");
        state_.snapSpacing = value; return true;
    }
    if (command == "t" || command == "r" || command == "s") {
        auto& scene = app_.getScene();
        const auto placement = scene.placementMatrix(node(state_.selected).id);
        require(placement.has_value(),"객체 배치가 없습니다.");
        auto matrix = *placement;
        const auto final = scene.worldMatrix(node(state_.selected).id);
        require(final.has_value(),"피벗 월드 변환 실패.");
        const auto origin = glm::vec3(*final * glm::vec4(node(state_.selected).rest.localBasis,1));
        if (command == "t") {
            require(a.size() == 4,"t X Y Z [월드 원점 배치] - XYZ: 절대 좌표");
            matrix[3] = glm::vec4(vector(a,1),1);
        } else {
            std::optional<glm::mat4> change;
            if (command == "r") {
                require(a.size() == 3,"r <축> <도> [월드 축 상대 회전]");
                change = graphics::TransformSystem::rotation({origin,axis(a[1])},number(a[2]));
            } else {
                require(a.size() == 2 || a.size() == 4,"s <균일값 또는 SX SY SZ> [월드 축 상대 배율]");
                const auto factors = a.size() == 2 ? glm::vec3(float(number(a[1]))) : vector(a,1);
                require(glm::all(glm::greaterThan(glm::abs(factors),glm::vec3(.0001f))),"배율은 0이 아니어야 합니다.");
                change = graphics::TransformSystem::scaling(factors,origin);
            }
            require(change.has_value(),"변환 행렬 생성 실패."); matrix = *change * matrix;
        }
        setWorld(state_.selected,matrix); return true;
    }
    if (command == "local") {
        require(a.size() >= 2,"local t/r/s ...");
        Tokens local{"layer"}; local.insert(local.end(),a.begin()+1,a.end()); return workspaceCommand(local,out);
    }
    if (command == "layer" || command == "insert") {
        auto& object = node(state_.selected);
        auto& layers = object.layers;
        const auto* driver = positionDriver(state_.selected);
        require(!driver || (driver->effect != Effect::Body && driver->effect != Effect::Walk && driver->effect != Effect::Custom),
                "적분 물리/사용자 제어 루트의 레이어는 제어 함수와 별도의 자식 객체에 작성하세요.");
        if (command == "insert") {
            require(a.size() >= 2,"insert spin/orbit/linear/pendulum/translate/rotate/scale ...");
            std::string name = "layer" + std::to_string(layers.size()+1);
            while (std::any_of(layers.begin(),layers.end(),[&](const LocalLayer& l){return l.name == name;})) name += "_";
            LocalLayer layer; layer.name = name;
            if (a[1] == "spin" || a[1] == "orbit") {
                require(a[1] == "spin" ? a.size() == 4 : (a.size() == 8 && a[4] == "center"),
                    "insert spin <축> <도/초> 또는 insert orbit <축> <도/초> center <월드 XYZ/cursor>");
                graphics::RotationMotion rotation;
                rotation.axis.direction = axis(a[2]); rotation.degreesPerSecond = number(a[3]);
                rotation.space = graphics::TransformSpace::Local;
                if (a[1] == "orbit") {
                    const auto frame = app_.getScene().worldMatrix(object.id);
                    require(frame && affine(*frame),"공전 중심 변환 실패.");
                    rotation.axis.point = glm::vec3(glm::inverse(*frame)*glm::vec4(vector(a,5),1));
                }
                require(layer.motion.append(graphics::Motion::rotation(rotation)),"회전 템플릿 생성 실패.");
            } else if (a[1] == "linear") {
                require(a.size() == 6,"insert linear DX DY DZ 초");
                graphics::LinearMotion motion; motion.to = vector(a,2); motion.durationSeconds = number(a[5]);
                motion.space = graphics::TransformSpace::Local;
                require(layer.motion.append(graphics::Motion::linear(motion)),"선형 이동 템플릿 생성 실패.");
            } else if (a[1] == "pendulum") {
                require(a.size() == 5,"insert pendulum <축> <진폭 도> <주기 초>");
                graphics::PendulumMotion motion; motion.axis.direction = axis(a[2]);
                motion.amplitudeDegrees = number(a[3]); motion.periodSeconds = number(a[4]);
                require(layer.motion.append(graphics::Motion::pendulum(motion)),"진자 템플릿 생성 실패.");
            } else {
                require(a[1] == "translate" || a[1] == "rotate" || a[1] == "scale","지원하지 않는 변환 템플릿입니다.");
                layers.push_back(layer); state_.selectedLayer = name;
                Tokens edit{"layer",a[1] == "translate" ? "t" : a[1] == "rotate" ? "r" : "s"};
                edit.insert(edit.end(),a.begin()+2,a.end()); return workspaceCommand(edit,out);
            }
            require(layers.size() < 127,"지역 레이어는 127개까지입니다.");
            layers.push_back(std::move(layer)); state_.selectedLayer = name; return true;
        }
        require(a.size() >= 2,"layer add/select/remove/before/t/r/s/reset/list ...");
        if (a[1] == "list") {
            for (const auto& l : layers) out << l.name << " [지역 레이어] - 앞 레이어 결과를 상속, 초=" << l.seconds << '\n';
            return true;
        }
        if (a[1] == "add") {
            require(a.size() == 3 && a[2] != "none" && layers.size() < 127,"layer add <이름>. none은 예약어입니다.");
            for (const auto& l:layers) require(l.name != a[2],"중복 레이어 이름입니다.");
            LocalLayer layer; layer.name = a[2]; layers.push_back(layer); state_.selectedLayer = a[2]; return true;
        }
        if (a[1] == "select") {
            require(a.size() == 3,"layer select <이름/none>");
            if (a[2] == "none") { state_.selectedLayer.clear(); return true; }
            require(std::any_of(layers.begin(),layers.end(),[&](const LocalLayer& l){return l.name == a[2];}),"없는 레이어입니다.");
            state_.selectedLayer = a[2]; return true;
        }
        auto it = std::find_if(layers.begin(),layers.end(),[&](const LocalLayer& l){return l.name == state_.selectedLayer;});
        require(it != layers.end(),"layer add/select로 지역 레이어를 지정하세요.");
        if (a[1] == "remove") {
            require(a.size() == 2,"layer remove"); layers.erase(it); state_.selectedLayer.clear(); return true;
        }
        if (a[1] == "before") {
            require(a.size() == 3 && a[2] != it->name,"layer before <다른 레이어 이름>");
            const auto target = std::find_if(layers.begin(),layers.end(),[&](const LocalLayer& l){return l.name == a[2];});
            require(target != layers.end(),"대상 레이어가 없습니다.");
            const auto old = std::size_t(it-layers.begin()), to = std::size_t(target-layers.begin());
            auto moved = *it; layers.erase(layers.begin()+old);
            layers.insert(layers.begin()+(to > old ? to-1 : to),std::move(moved)); return true;
        }
        if (a[1] == "reset") { require(a.size()==2,"layer reset"); it->matrix=glm::mat4(1); return true; }
        if (a[1] == "enable" || a[1] == "play") {
            require(a.size()==3,"layer enable/play on/off");
            (a[1] == "enable" ? it->enabled : it->playing) = boolean(a[2]); return true;
        }
        std::optional<glm::mat4> change;
        if (a[1] == "t") { require(a.size()==5,"layer t DX DY DZ"); change=graphics::TransformSystem::translation(vector(a,2)); }
        else if (a[1] == "r") { require(a.size()==4,"layer r <지역 축> <도>"); change=graphics::TransformSystem::rotation({glm::vec3(0),axis(a[2])},number(a[3])); }
        else if (a[1] == "s") {
            require(a.size()==3 || a.size()==5,"layer s <균일값 또는 SX SY SZ>");
            change=graphics::TransformSystem::scaling(a.size()==3 ? glm::vec3(float(number(a[2]))) : vector(a,2));
        } else require(false,"지원하지 않는 레이어 명령입니다.");
        require(change && affine(it->matrix * *change),"레이어 변환은 역행렬이 있는 아핀 행렬이어야 합니다.");
        it->matrix *= *change; return true;
    }
    return false;
}

void Assembly::editStep(float amount, std::ostream& out) {
    if (!editOperation_ || state_.selected.empty()) return;
    const std::string axisName(1,"xyz"[state_.axis]);
    std::ostringstream command;
    const bool local = !state_.selectedLayer.empty();
    if (editOperation_ == 'r') command << (local ? "layer r " : "r ") << axisName << ' ' << amount*5;
    else if (editOperation_ == 's') {
        glm::vec3 scale(1); scale[state_.axis] = std::pow(1.05f,amount);
        command << (local ? "layer s " : "s ") << scale.x << ' ' << scale.y << ' ' << scale.z;
    } else {
        glm::vec3 p(0); p[state_.axis]=amount*.1f;
        if (!local) {
            const auto base=app_.getScene().placementMatrix(node(state_.selected).id);
            if (!base) return;
            p += glm::vec3((*base)[3]);
        }
        command << (local ? "layer t " : "t ") << p.x << ' ' << p.y << ' ' << p.z;
    }
    execute(command.str(),out);
}
void Assembly::finishEdit(bool keep, std::ostream& out) {
    if (!editStart_) return;
    if (!keep) { state_ = *editStart_; applyPoses(0); }
    else state_.paused = editStart_->paused;
    editStart_.reset(); editOperation_=0; editCursor_.reset();
    out << (keep ? "Enter [변환 확정]" : "Esc [변환 취소]") << " - 인수 없음\n";
}
} // namespace assembly
