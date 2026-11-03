#pragma once
#include "PracticeMode.h"
#include <functional>
#include <map>
#include <ostream>

namespace assembly {
using Tokens = std::vector<std::string>;
class Assembly;

struct FaceMount {
    graphics::EntityId mesh = 0;
    std::size_t face = 0, triangle = 0;
    glm::vec3 coordinate{0};
    bool barycentric = true;
};

struct Node {
    graphics::EntityId id = 0;
    std::optional<graphics::EntityId> mesh;
    graphics::Transform3D rest, initial;
    std::string kind;
    bool whole = false;
    std::optional<graphics::Bounds3> room;
    std::optional<graphics::JointRig> walker;
    std::string guideTopic;
    std::optional<FaceMount> mount;
    bool affineGroup = false;
};

enum class Effect { Motion, Face, FaceTrack, Wall, Body, Walk, Conveyor, Custom, Input };
inline bool faceEffect(Effect effect) { return effect == Effect::Face || effect == Effect::FaceTrack || effect == Effect::Wall; }
struct Behavior {
    std::string name, target, reference;
    Effect effect = Effect::Motion;
    graphics::Motion motion;
    double seconds = 0;
    bool playing = true, enabled = true, released = false;
    bool positionDriver = false, parentSpin = false;
    graphics::FaceRotationConfig face;
    std::size_t faceIndex = 0;
    glm::mat4 initialFaceMatrix{1};
    graphics::FaceAnimationSpace initialFaceSpace = graphics::FaceAnimationSpace::Local;
    bool initialFaceVisible = true;
    glm::mat4 faceTrackBase{1};
    float rate = 1, phase = 0, height = .455f;
    graphics::RoomBody body;
    glm::vec3 gravity{0, -9.8f, 0}, angularVelocity{0};
    bool elastic = false, releaseRequested = false;
    glm::mat3 releasedRotation{1};
    graphics::CharacterBody character;
    std::vector<std::string> obstacles;
    glm::mat4 previousRoom{1};
    std::function<bool(graphics::Scene&, const graphics::Transform3D&, double, float)> custom;
    std::function<bool(Assembly&, int, int, int)> key;
    std::function<bool(Assembly&, const graphics::MouseEvent&)> mouse;
    std::function<bool(Assembly&, double, double)> cursor;
    std::function<void(Assembly&, bool)> focus;
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
        std::string dragTarget;
        glm::vec3 dragAxis{0, 1, 0};
        float dragSensitivity = .35f;
        bool dragging = false;
        std::optional<double> dragX;
        bool paused = false, threeViews = false;
    } state_;
    graphics::Application& app_;
    bool replaying_ = false, faulted_ = false;
    std::uint64_t topologyRevision_ = 0;

    bool dispatch(const Tokens& args, std::ostream& out);
    bool motionCommand(const Tokens& args, std::ostream& out);
    bool objectCommand(const Tokens& args, std::ostream& out);
    bool hierarchyCommand(const Tokens& args, std::ostream& out);
    bool surfaceCommand(const Tokens& args, std::ostream& out);
    bool trackCommand(const Tokens& args, std::ostream& out);
    bool inputCommand(const Tokens& args, std::ostream& out);
    bool applyPoses(float dt);
    bool dispatchInput(const std::function<bool(const Behavior&)>& callback, std::ostream& out);
    void applyMounts();
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
    void group(const std::string& name, glm::vec3 position);
    std::string part(const std::string& kind, const std::string& parent, const std::string& localName,
                     glm::vec3 position, glm::vec3 size = glm::vec3(1));
    void srt(const std::string& target, glm::vec3 position, glm::vec3 degrees, float scale);
    void pivot(const std::string& target, glm::vec3 point, bool keep = true);
    glm::vec3 center(const std::string& target) const;
    void attach(Behavior behavior);
    void move(const std::string& target, glm::vec3 delta);
    void rotate(const std::string& target, glm::vec3 axis, float degrees);
    void setRest(const std::string& target, const graphics::Transform3D& rest);
    graphics::EntityId meshEntity(const std::string& target) const;
    void requireGeometryEditable(graphics::EntityId mesh) const;
    void requireFaceEditable(graphics::EntityId mesh, std::size_t face) const;
    void model(const std::string& name, const graphics::Model& model, glm::vec3 position = glm::vec3(0));
    std::string mount(const std::string& parent, const std::string& localName, FaceMount attachment);
    void bind(Binding binding);
    bool execute(const std::string& line, std::ostream& out);
    bool update(float dt, std::ostream& out);
    bool render();
    void onKey(int key, int action, int mods, std::ostream& out);
    void onMouse(const graphics::MouseEvent& event, std::ostream& out);
    void onFocus(bool focused);
    void onCursor(double x, double y, std::ostream& out);
    void guide(const std::string& topic, std::ostream& out) const;
    void help(std::ostream& out) const;
    void inspect(const std::string& target, std::ostream& out) const;
};

Tokens tokenize(const std::string& line);
std::string quote(const std::string& text);
double number(const std::string& text);
std::size_t indexNumber(const std::string& text);
glm::vec3 vector(const Tokens& args, std::size_t offset);
glm::vec3 axis(const std::string& text);
bool boolean(const std::string& text);
void require(bool success, const std::string& message);
bool rigid(const glm::mat4& matrix);
std::optional<bool> customCommand(Assembly& scene, const Tokens& args, std::ostream& out);
bool defaultRecipe(Assembly& scene, std::ostream& out);
} // namespace assembly
