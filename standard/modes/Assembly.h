#pragma once
#include "PracticeMode.h"
#include <map>
#include <ostream>

namespace assembly {
using Tokens = std::vector<std::string>;

struct Node {
    graphics::EntityId id = 0;
    std::optional<graphics::EntityId> mesh;
    graphics::Transform3D rest, initial;
    std::string kind;
    bool whole = false;
    std::optional<graphics::Bounds3> room;
    std::optional<graphics::JointRig> walker;
};

enum class Effect { Motion, Face, Wall, Body, Walk, Conveyor };
struct Behavior {
    std::string name, target, reference;
    Effect effect = Effect::Motion;
    graphics::Motion motion;
    double seconds = 0;
    bool playing = true, enabled = true;
    bool positionDriver = false;
    graphics::FaceRotationConfig face;
    std::size_t faceIndex = 0;
    float rate = 1, phase = 0, height = .455f;
    graphics::RoomBody body;
    glm::mat3 releasedRotation{1};
    graphics::CharacterBody character;
    std::vector<std::string> obstacles;
    glm::mat4 previousRoom{1};
};

struct Binding {
    int key = 0, mods = 0;
    bool hold = false, down = false;
    std::string command;
    Tokens tokens;
};

class Assembly {
    struct State {
        std::map<std::string, Node> nodes;
        std::vector<Behavior> behaviors;
        std::vector<Binding> bindings;
        std::vector<std::string> recipe;
        std::string selected;
        int axis = 0;
        bool paused = false, threeViews = false;
    } state_;
    graphics::Application& app_;
    bool replaying_ = false, faulted_ = false;

    bool dispatch(const Tokens& args, std::ostream& out);
    bool motionCommand(const Tokens& args, std::ostream& out);
    bool objectCommand(const Tokens& args, std::ostream& out);
    bool inputCommand(const Tokens& args, std::ostream& out);
    bool applyPoses(float dt);
    void registerNode(const std::string& name, graphics::EntityId id, bool whole,
                      const std::string& kind = "part", std::optional<graphics::EntityId> mesh = std::nullopt);
    void registerPerson(const std::string& prefix, const graphics::HumanoidRig& rig);
    Behavior* positionDriver(const std::string& target);
    bool save(const std::string& path, std::ostream& out) const;
    bool load(const std::string& path, std::ostream& out);
public:
    explicit Assembly(graphics::Application& app) : app_(app) {}
    graphics::Application& application() { return app_; }
    const std::map<std::string, Node>& nodes() const { return state_.nodes; }
    const std::vector<Behavior>& behaviors() const { return state_.behaviors; }
    Node& node(const std::string& name);
    const Node& node(const std::string& name) const;
    void clear();
    void add(const std::string& kind, const std::string& name, glm::vec3 position,
             glm::vec3 size = glm::vec3(1));
    void attach(Behavior behavior);
    void move(const std::string& target, glm::vec3 delta);
    void rotate(const std::string& target, glm::vec3 axis, float degrees);
    void setRest(const std::string& target, const graphics::Transform3D& rest);
    void bind(Binding binding);
    bool execute(const std::string& line, std::ostream& out);
    bool update(float dt, std::ostream& out);
    bool render();
    void onKey(int key, int action, int mods, std::ostream& out);
    void onMouse(const graphics::MouseEvent& event, std::ostream& out);
    void onFocus(bool focused);
    void guide(const std::string& topic, std::ostream& out) const;
    void help(std::ostream& out) const;
    void inspect(const std::string& target, std::ostream& out) const;
};

Tokens tokenize(const std::string& line);
std::string quote(const std::string& text);
double number(const std::string& text);
glm::vec3 vector(const Tokens& args, std::size_t offset);
glm::vec3 axis(const std::string& text);
bool boolean(const std::string& text);
void require(bool success, const std::string& message);
bool rigid(const glm::mat4& matrix);
std::optional<bool> customCommand(Assembly& scene, const Tokens& args, std::ostream& out);
bool defaultRecipe(Assembly& scene, std::ostream& out);
} // namespace assembly
