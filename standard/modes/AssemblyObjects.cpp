#include "Assembly.h"
#include "AssemblyParts.h"
#include <cctype>

namespace assembly {
void Assembly::registerPerson(const std::string& name, const graphics::HumanoidRig& rig) {
    registerNode(name, rig.root, true, "humanoid");
    registerNode(name + ".pelvis", rig.pelvis, false);
    registerNode(name + ".torso", rig.torso, false, "joint", rig.torsoMesh);
    registerNode(name + ".neck", rig.neck, false, "joint", rig.head);
    registerNode(name + ".head", rig.head, false, "mesh", rig.head);
    const char* sides[] = {".left", ".right"};
    for (std::size_t i = 0; i < 2; ++i) {
        const std::string prefix = name + sides[i];
        registerNode(prefix + "Shoulder", rig.shoulders[i], false);
        registerNode(prefix + "Elbow", rig.elbows[i], false);
        registerNode(prefix + "Hand", rig.hands[i], false);
        registerNode(prefix + "Hip", rig.hips[i], false);
        registerNode(prefix + "Knee", rig.knees[i], false);
        registerNode(prefix + "Foot", rig.feet[i], false);
    }
}

void Assembly::add(const std::string& kind, const std::string& name, glm::vec3 position, glm::vec3 size) {
    require(!name.empty() && name.size() <= 64 && !state_.nodes.count(name), "사용하지 않은 객체 이름(1~64바이트)이 필요합니다.");
    for (unsigned char c : name)
        require(c > 32 && c != '"' && c != '#' && c != '\\', "이름에는 공백·따옴표·#·역슬래시를 쓰지 마세요.");
    require(name.front() != '.' && name.back() != '.' && name.find("..") == std::string::npos, "이름의 각 구간은 비어 있으면 안 됩니다.");
    require(state_.nodes.size() < 1000, "객체/부품 이름 1000개 한도입니다.");
    require(graphics::detail::finiteVector(position) && graphics::detail::finiteVector(size) && glm::all(glm::greaterThan(size, glm::vec3(0))), "위치와 양수 크기를 확인하세요.");
    const bool sized = kind == "box" || kind == "pyramid" || kind == "sphere" || kind == "cone" || kind == "tetra";
    require(sized || size == glm::vec3(1), "복합 객체는 크기 인자 대신 scale 명령을 사용하세요.");
    auto& scene = app_.getScene();
    const auto entity = [](std::optional<graphics::EntityId> id) {
        require(id.has_value(), "객체 생성 실패. 앞선 프레임워크 진단을 확인하세요.");
        return *id;
    };
    if (kind == "humanoid") {
        graphics::HumanoidConfig config;
        config.grid.frame = glm::translate(glm::mat4(1), position);
        const auto rig = graphics::AnimationSystem::addHumanoid(scene, config);
        require(rig.has_value(), "사람 생성 실패.");
        registerPerson(name, *rig);
        return;
    }
    if (kind == "swing") {
        graphics::SwingConfig config;
        config.pivot += position;
        const auto rig = graphics::AnimationSystem::addSwingWithRider(scene, config);
        require(rig.has_value(), "그네 생성 실패.");
        registerNode(name, rig->frame, true, kind);
        registerNode(name + ".hinge", rig->hinge, false);
        registerNode(name + ".seat", rig->seat, false, "mesh", rig->seat);
        registerPerson(name + ".rider", rig->person);
        for (std::size_t i = 0; i < rig->animation.size(); ++i) {
            Behavior b;
            b.name = name + (i == 0 ? ".swing" : ".sway");
            b.target = name + (i == 0 ? ".hinge" : ".rider.torso");
            b.motion = rig->animation[i].motion;
            attach(std::move(b));
        }
        return;
    }
    if (kind == "tank") {
        const auto rig = addTankParts(scene);
        graphics::Transform3D rest;
        rest.basis = position;
        require(scene.setTransform(rig.root, rest), "탱크 위치 설정 실패.");
        registerNode(name, rig.root, true, kind);
        node(name).guideTopic = "tank-preset";
        registerNode(name + ".central", rig.central, false);
        for (std::size_t i = 0; i < 2; ++i) {
            const auto prefix = name + (i == 0 ? ".left" : ".right");
            registerNode(prefix + "Lower", rig.lower[i], false, "mesh", rig.lower[i]);
            registerNode(prefix + "Upper", rig.upper[i], false);
            registerNode(prefix + "Gun", rig.guns[i], false);
            registerNode(prefix + "Pole", rig.poles[i], false);
        }
        return;
    }
    if (kind == "walker") {
        auto grid = graphics::HumanoidConfig{}.grid;
        grid.frame = glm::translate(glm::mat4(1), position);
        const auto rig = graphics::PatternSystem::addWalkingRig(scene, grid);
        require(rig.has_value(), "보행 리그 생성 실패.");
        registerNode(name, rig->root, true, kind);
        node(name).walker = *rig;
        registerNode(name + ".head", rig->head, false);
        for (std::size_t i = 0; i < 2; ++i) {
            const auto prefix = name + (i == 0 ? ".left" : ".right");
            registerNode(prefix + "Shoulder", rig->shoulders[i], false);
            registerNode(prefix + "Hip", rig->hips[i], false);
        }
        return;
    }
    const bool primitive = kind == "box" || kind == "pyramid" || kind == "sphere" || kind == "cone" || kind == "tetra";
    require(primitive || kind == "room" || kind == "grid" || kind == "joint" || kind == "conveyor" || kind == "planets",
            "지원 종류: box pyramid sphere cone tetra humanoid swing tank walker room grid joint conveyor planets");
    graphics::Transform3D rest;
    rest.basis = position;
    const auto root = entity(scene.addGroup(rest));
    registerNode(name, root, true, kind);
    if (primitive) {
        graphics::EntityId mesh;
        if (kind == "box")
            mesh = entity(scene.addCuboid(size, {0, 0, 0}));
        else if (kind == "tetra")
            mesh = entity(scene.addRightTetrahedron(size, {0, 0, 0}));
        else if (kind == "pyramid")
            mesh = entity(scene.addPyramid(size, {0, 0, 0}));
        else if (kind == "sphere") {
            require(size.x == size.y && size.x == size.z, "sphere의 크기는 세 축이 같은 지름입니다.");
            mesh = entity(scene.addSphere(size.x * .5f, {0, 0, 0}));
        } else {
            require(size.x == size.z, "cone의 X/Z 지름은 같아야 합니다.");
            mesh = entity(scene.addCone(size.x * .5f, size.y, {0, 0, 0}));
        }
        require(scene.setParent(mesh, root), "메시 부모 연결 실패.");
        node(name).mesh = mesh;
        registerNode(name + ".mesh", mesh, false, "mesh", mesh);
    } else if (kind == "room") {
        const graphics::Bounds3 bounds{{-2, 0, -2}, {2, 3, 2}};
        const auto mesh = entity(graphics::PatternSystem::addRoom(scene, bounds));
        require(scene.setParent(mesh, root), "방 부모 연결 실패.");
        node(name).mesh = mesh;
        node(name).room = bounds;
        registerNode(name + ".walls", mesh, false, "mesh", mesh);
    } else if (kind == "grid") {
        graphics::GridLayout grid;
        grid.origin = {-3, -.1f, -2};
        grid.extent = {6, .2f, 4};
        grid.cells = {6, 1, 4};
        const auto cells = addGridParts(scene, grid);
        for (std::size_t i = 0; i < cells.size(); ++i) {
            require(scene.setParent(cells[i], root), "격자 부모 연결 실패.");
            registerNode(name + ".cell" + std::to_string(i + 1), cells[i], false, "mesh", cells[i]);
        }
    } else if (kind == "conveyor") {
        for (const auto& spec : std::vector<std::pair<glm::vec3, glm::vec3>>{
            {{7.2f, .15f, 1.6f}, {-.8f, .925f, -2}}, {{1.6f, .15f, 5}, {3, .925f, 1.3f}}})
            entity(graphics::PatternSystem::addBox(scene, spec.first, spec.second, {.15f, .2f, .25f}, root));
        for (unsigned i = 0; i < 5; ++i) {
            const auto parcel = entity(scene.addGroup());
            require(scene.setParent(parcel, root), "상자 부모 연결 실패.");
            const auto mesh = entity(graphics::PatternSystem::addBox(scene, {.85f, .85f, .85f}, {0, 0, 0},
                {.3f + .12f * i, .7f, .3f}, parcel));
            const auto label = name + ".parcel" + std::to_string(i + 1);
            registerNode(label, parcel, true, "parcel", mesh);
            require(graphics::PatternSystem::addNumber(scene, i + 1, {-.1f, 0, .43f}, .3f, {1, 1, 1}, parcel),
                    "택배 번호 생성 실패.");
            Behavior b;
            b.name = label + ".belt"; b.target = label; b.effect = Effect::Conveyor;
            b.positionDriver = true; b.rate = 1.25f; b.phase = float(i) / 5;
            attach(std::move(b));
        }
    } else if (kind == "planets") {
        const auto sun = entity(scene.addSphere(.55f, {0, 0, 0}));
        require(scene.setParent(sun, root) && scene.setTint(sun, {1, .8f, .2f}), "태양 생성 실패.");
        registerNode(name + ".sun", sun, true, "sun", sun);
        for (unsigned i = 0; i < 3; ++i) {
            const auto planet = entity(scene.addGroup());
            const auto mesh = entity(scene.addSphere(.25f, {0, 0, 0}));
            require(scene.setParent(planet, root) && scene.setParent(mesh, planet), "행성 부모 연결 실패.");
            const auto label = name + ".planet" + std::to_string(i + 1);
            registerNode(label, planet, true, "planet", mesh);
            node(label).rest.basis = {2 + .8f * i, 0, 0};
            Behavior orbit;
            orbit.name = label + ".orbit"; orbit.target = label; orbit.positionDriver = true;
            graphics::RotationMotion config;
            config.space = graphics::TransformSpace::World; config.degreesPerSecond = 20 + 10 * i;
            require(orbit.motion.append(graphics::Motion::rotation(config)), "행성 공전 생성 실패.");
            attach(std::move(orbit));
            const auto moon = entity(scene.addGroup());
            const auto moonMesh = entity(scene.addSphere(.1f, {0, 0, 0}));
            require(scene.setParent(moon, planet) && scene.setParent(moonMesh, moon), "달 부모 연결 실패.");
            registerNode(label + ".moon", moon, true, "moon", moonMesh);
            node(label + ".moon").rest.basis = {.6f, 0, 0};
            Behavior lunar;
            lunar.name = label + ".moon.orbit"; lunar.target = label + ".moon"; lunar.positionDriver = true;
            config.degreesPerSecond = 70 + 30 * i;
            require(lunar.motion.append(graphics::Motion::rotation(config)), "달 공전 생성 실패.");
            attach(std::move(lunar));
        }
    }
}
} // namespace assembly
