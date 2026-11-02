"""Author offline source-backed guides. Does not build, test or launch an app."""
from pathlib import Path
import re, json, html, hashlib, difflib
import os

ROOT = Path(__file__).resolve().parents[1]
def source_path(p):
    # Older guide inputs used inline implementations; current sources use .cpp.
    if p.endswith('.inl') and not (ROOT / p).is_file():
        candidate = str(Path(p).with_suffix('.cpp')).replace('\\', '/')
        if (ROOT / candidate).is_file():
            return candidate
    return p

def read(p): return (ROOT / source_path(p)).read_text(encoding='utf-8-sig')
def write(p, s):
    p = ROOT / p
    p.parent.mkdir(parents=True, exist_ok=True)
    if p.is_file() and p.read_bytes() == s.encode('utf-8-sig'):
        return
    p.write_text(s, encoding='utf-8-sig', newline='\n')
def esc(s): return html.escape(s)
def code(s): return '<pre><code>' + esc(s.strip()) + '</code></pre>'
def para(s): return '<p>' + esc(s) + '</p>'
def source(p):
    p = source_path(p)
    return '<details><summary>실제 저장소 소스 · '+esc(p)+'</summary>'+code(read(p))+'</details>'

def apis():
    result = []
    for p in sorted((ROOT/'framework/include').glob('*.h')):
        s = p.read_text(encoding='utf-8-sig')
        mask = re.sub(r'//[^\n]*|/\*[\s\S]*?\*/|"(?:\\.|[^"\\])*"', lambda m: ''.join('\n' if c=='\n' else ' ' for c in m.group()), s)
        spans = []
        for m in re.finditer(r'\b(?:class|struct)\s+(\w+)\s*\{', mask):
            start=m.end()-1; depth=1; end=start+1
            while end<len(mask) and depth:
                depth += (mask[end]=='{')-(mask[end]=='}'); end+=1
            spans.append((start,end,m.group(1)))
        for m in re.finditer(r'^[ \t]*///\s*\[API\]\s*(.+)\n',s,re.M):
            start=m.end(); i=start; parens=0
            while i<len(s):
                c=s[i]
                if c=='(': parens+=1
                elif c==')': parens-=1
                elif c in ';{' and parens==0: break
                i+=1
            signature=' '.join(s[start:i].strip().split())
            if '(' not in signature: continue
            prefix=signature.split('(',1)[0]
            nm=re.search(r'(operator\s+bool|operator\s*=|~?\w+)\s*$',prefix)
            if not nm: continue
            name=nm.group(1)
            if re.search(r'\w+::'+re.escape(name)+r'\s*$',prefix):continue
            enclosing=[v for v in spans if v[0]<start<v[1]]
            owner=min(enclosing,key=lambda v:v[1]-v[0])[2] if enclosing else ''
            result.append(dict(owner=owner,name=name,signature=signature,file=p.relative_to(ROOT).as_posix(),line=s[:start].count('\n')+1,description=m.group(1)))
            if name=='readTextFile': result[-1]['description']='파일을 문자열로 읽고 UTF-8 BOM을 제거한다. 파일을 열지 못하면 빈 문자열을 반환한다.'
    return result

GROUPS={}
def group(key,title,story,roles,code_text,after,files=()):
    GROUPS[key]=dict(title=title,story=story,roles=roles,code=code_text,after=after,files=files)

group('app','입력·시간·장면·렌더링을 연결하는 실제 main',
 '회전하는 상자를 만들고 Esc로 종료하는 상황이다. Application은 창과 루프를 소유하고, box는 Scene 안의 상자를 다시 찾는 번호다. 콜백은 등록할 때 실행되지 않는다. run이 초기화·이벤트·프레임 시점에 각각 실행한다.',
 'app은 run 종료까지 살아 있는 소유 객체다. box는 초기화 콜백에서 채우는 optional ID다. dt는 이번 프레임 경과 초이며 angle은 누적 각도다. camera와 options는 Application 내부 상태를 수정하는 참조다.',r'''
#include "framework/Framework.h"
#include <iostream>
int main() {
    graphics::Application app;
    std::optional<graphics::EntityId> box;
    float angle = 0;
    app.setInitHandler([&](graphics::Application& current) {
        box = current.getScene().addCuboid({1,1,1}, {0,0,0});
        auto& camera = current.getCamera(); camera.position = {4,3,6};
        current.getRenderOptions().depthTest = true;
        return box.has_value();
    });
    app.setKeyHandler([](graphics::Application& current,int key,int action) {
        if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) current.requestClose();
    });
    app.setModifiedKeyHandler([](graphics::Application& current,int key,int action,int mods) {
        if(key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) current.requestClose();
        std::cout << "수정키 비트=" << mods << '\n';
    });
    app.setFocusHandler([](graphics::Application&,bool focused) {
        std::cout << "포커스=" << focused << '\n';
    });
    app.setMouseHandler([](graphics::Application& current,const graphics::MouseEvent& e) {
        if(e.action!=GLFW_PRESS) return;
        if(auto hit=current.pickAt(e.windowX,e.windowY)) std::cout << hit->entityId << '\n';
    });
    app.setCursorHandler([](graphics::Application& current,double x,double y) {
        auto hit=current.pickAtCursor();
        (void)hit; (void)x; (void)y;
    });
    app.setUpdateHandler([&](graphics::Application& current,float dt) {
        angle += 45*dt;
        graphics::Transform3D pose;
        pose.rotation=glm::mat3(glm::rotate(glm::mat4(1),glm::radians(angle),glm::vec3(0,1,0)));
        if(!box || !current.getScene().setTransform(*box,pose)) current.requestClose();
    });
    app.setRenderHandler([](graphics::Application& current) {
        const auto size=current.getFramebufferSize();
        GLFWwindow* window=current.getWindow(); (void)window;
        if(size[0]<3 || size[1]<2) return true;
        return current.renderViews(graphics::PatternSystem::threeViews(
            current.getCamera(),size[0],size[1],current.getRenderOptions()));
        // 단일 뷰로 교체할 때는 return current.renderScene();
    });
    graphics::ApplicationConfig config;
    return app.run(config);
} // ~Application이 호출된다. 복사 생성/대입은 =delete이므로 호출할 수 없다.
''','수정키 콜백을 등록하면 일반 키 콜백 대신 호출된다. 이 예제의 Esc 처리는 두 콜백에 모두 있지만 실행되는 것은 수정키 콜백이다. update가 실패하면 종료를 요청하고, run은 GPU를 현재 컨텍스트에서 정리한다. getAssets의 사용 예는 자산 그룹에 따로 둔다.',('standard/main.cpp','framework/detail/Application.inl'))

group('scene','CPU 장면에 형상을 만들고 ID로 관리하기',
 '방 안에 상자·공·원뿔을 놓는 장면을 만든다. 생성 함수는 GPU를 직접 그리는 명령이 아니라 Scene에 CPU 데이터를 등록하는 명령이다. 생성 ID를 보관한 뒤 다음 프레임에서 위치와 표시 상태를 바꾼다.',
 'scene이 모델과 컴포넌트를 소유한다. id는 컨테이너 원소를 찾는 값이며 포인터가 아니다. component와 mesh 포인터는 비소유 조회 결과이므로 장면 변경을 가로질러 보관하지 않는다.',r'''
bool populate(graphics::Scene& scene) {
    auto box=scene.addCuboid({1,1,1},{0,.5f,0});
    auto ball=scene.addSphere(.4f,{2,.4f,0});
    auto cone=scene.addCone(.4f,1,{3,0,0});
    auto tetra=scene.addRightTetrahedron({1,1,1},{-2,0,0});
    auto pyramid=scene.addPyramid({1,1,1},{-3,0,0});
    auto model=graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube,{1,1,1});
    auto first=scene.push_entity(model);
    auto second=scene.addModel(model,{},graphics::PivotMode::Preserve);
    if(!box||!ball||!cone||!tetra||!pyramid||!first||!second) return false;
    if(!scene.setTint(*box,{1,.4f,.2f}) || !scene.setVisible(*ball,true)) return false;
    if(!scene.setSideLengths(*box,{1,2,1}) || !scene.setModel(*second,model)) return false;
    for(auto id:scene.getIds()) {
        const auto* component=scene.getComponent(id);
        if(component) { auto local=component->transform; (void)local; }
    }
    auto revision=scene.getGeometryRevision();
    const auto& cubes=scene.getCubeFaceIds();
    const auto& tetras=scene.getTetraFaceIds();
    const auto& pyramids=scene.getPyramidFaceIds();
    (void)revision; (void)cubes; (void)tetras; (void)pyramids;
    return true;
}
void rebuildScene() {
    graphics::Scene scene;
    if(!populate(scene)) return;
    scene.clear(); // 이전 ID는 버리고 다시 populate한다.
}
''','형상 변경 번호는 Renderer가 업로드 필요성을 판단하는 근거다. clear 뒤 ID가 재사용될 수 있으므로 예전 box 번호로 새 장면을 조작하면 안 된다.',('framework/detail/Scene.inl',))

group('transform','부모·피벗·면 변환을 실제 부품에 적용하기',
 '탱크 몸체에 포탑을 달고, 몸체 전체와 한 면을 따로 움직인다. 부모를 설정한 뒤 포탑 위치는 부모 좌표로 해석된다. 부모의 월드 행렬이 포탑 로컬 행렬 앞에 곱해진다.',
 'root는 회전 중심만 가진 그룹, body는 표시 메시다. pose.basis는 부모 공간 위치, localBasis는 모델 안의 피벗, rotation은 회전 기저, scale은 축별 배율이다. faceIndex는 엔티티 ID와 다른 면 번호다.',r'''
bool placeParts(graphics::Scene& scene) {
    auto root=scene.addGroup();
    auto body=scene.addCuboid({2,1,1},{0,0,0});
    if(!root||!body||!scene.setParent(*body,*root)) return false;
    graphics::Transform3D pose; pose.basis={0,.5f,0};
    if(!scene.setTransform(*body,pose) || !scene.setPosition(*body,{0,.6f,0})) return false;
    if(!scene.setLocalBasis(*body,graphics::PivotMode::Custom,{0,0,0})) return false;
    auto world=scene.worldMatrix(*body);
    auto faceWorld=scene.faceWorldMatrix(*body,0);
    if(!world||!faceWorld) return false;
    if(!scene.setFaceVisible(*body,0,true)
       ||!scene.setFaceCullMode(*body,0,graphics::CullMode::Back)) return false;
    graphics::Transform3D facePose; facePose.basis={0,.1f,0};
    if(!scene.setFaceTransform(*body,0,facePose,true)
       ||!scene.splitFace(*body,0)||!scene.setFaceSRT(*body,0,facePose)) return false;
    if(!scene.setFaceLocalMatrix(*body,0,glm::mat4(1))) return false;
    return scene.setParent(*body,std::nullopt);
}
''','setParent는 월드 자세를 자동 보존하지 않는다. 마지막 분리도 기존 로컬 값을 그대로 월드 값으로 사용한다. setFaceSRT와 setFaceLocalMatrix는 서로 다른 추가 변환 층이므로 사용 목적을 나눈다.',('openGL_3dAssign_22/Console.cpp','framework/detail/Scene.inl'))

group('mesh','메시·UV·재질을 편집한 뒤 Scene에 반영하기',
 '바닥 삼각형에 UV와 색을 지정한다. 등록 전에는 Mesh 값을 자유 함수로 가공하고, 등록 후에는 Scene setter를 사용한다. 조회한 메시를 복사해 편집한 뒤 다시 넣으면 GPU 변경 감지가 유지된다.',
 'vertices는 위치·법선·UV·색을 가진 CPU 정점 배열이다. indices는 이 배열을 참조하는 0 기반 번호다. meshIndex는 Model의 면/메시 배열 번호다. UV는 이미지 픽셀이 아니라 보통 0~1 텍스처 좌표다.',r'''
bool editTriangle(graphics::Scene& scene) {
    std::vector<graphics::Vertex> vertices(3);
    vertices[0].position={0,0,0}; vertices[1].position={1,0,0}; vertices[2].position={0,0,1};
    auto mesh=graphics::PlaneFigure::fromVertices(vertices,{0,1,2});
    graphics::UVOptions uv; uv.axis=1;
    if(!graphics::generateTexcoords(mesh,uv)
       ||!graphics::transformTexcoords(mesh,{2,2},{0,0},0)) return false;
    graphics::recalculateNormals(mesh);
    auto edges=graphics::meshOutline(mesh); (void)edges;
    graphics::Model model; model.meshes().push_back(mesh);
    const graphics::Model& view=model;
    auto count=view.meshes().size(); (void)count;
    auto id=scene.addModel(model,{},graphics::PivotMode::Preserve); if(!id) return false;
    if(!scene.setMesh(*id,0,mesh)) return false;
    if(!scene.setMeshVertices(*id,0,{{0,0,0},{1,0,0},{0,0,1}},{0,1,2})) return false;
    if(!scene.setVertexPosition(*id,0,1,{2,0,0})||!scene.setTriangleIndices(*id,0,{0,1,2})) return false;
    if(!scene.generateTexcoords(*id,0,uv)||!scene.transformTexcoords(*id,0,{1,1},{.1f,0},0)) return false;
    if(!scene.setTexcoords(*id,0,{{0,0},{1,0},{0,1}})
       ||!scene.setVertexNormals(*id,0,{{0,-1,0},{0,-1,0},{0,-1,0}})) return false;
    if(!scene.setFaceVertexColors(*id,0,{{1,0,0},{0,1,0},{0,0,1}})) return false;
    graphics::Material material; material.color={1,1,1};
    if(!scene.setMaterial(*id,0,material)
       ||!scene.setOutline(*id,0,graphics::OutlineMode::Explicit,{0,1,1,2,2,0})) return false;
    const auto* current=scene.getMesh(*id,0); return current!=nullptr;
}
''','정점 수를 바꾸면 UV·법선·색 배열도 같은 정점 수에 맞춰야 한다. Explicit 외곽선은 정점 번호의 쌍이다. 텍스처가 없는 위 사례는 재질 색만 사용한다.',('framework/include/Geometry.h','framework/detail/Scene.inl'))

group('lines','객체를 따라가는 선과 기저 표시',
 '회전하는 로봇에 X/Y/Z 기저를 붙여 어느 방향을 보는지 확인한다. 선의 owner가 로봇 ID이면 끝점이 로봇 로컬 좌표로 해석되어 함께 움직인다.',
 'line은 선 ID이고 owner는 엔티티 ID다. basis의 각 열은 X/Y/Z 방향이며 axisLengths는 표시 길이이다. 방향 벡터를 자동 정규화하지 않는다.',r'''
bool showAxes(graphics::Scene& scene,graphics::EntityId owner) {
    auto line=scene.addLine({0,0,0},{1,0,0},{1,0,0},owner);
    auto vector=scene.addVector({0,0,0},{0,1,0},{0,1,0},owner);
    auto axes=scene.addBasis({0,0,0},glm::mat3(1),{1,1,1},owner);
    if(!line||!vector||!axes||!scene.setVector(*line,{0,0,0},{2,0,0})) return false;
    auto world=scene.lineWorldMatrix(*line);
    const auto& lines=scene.getLines(); (void)lines;
    return world.has_value();
}
''','기저선은 실제 메시 표면을 수정하지 않는 관찰 도구다. 선과 모델은 서로 다른 ID 배열에 보관된다.',('framework/examples/basic.cpp',))

group('math','변환 계산과 경로 생성',
 '공이 원 경로를 따라 이동하고 한 축을 중심으로 도는 상황이다. TransformSystem은 행렬을 계산하고 PathSystem은 진행률을 위치로 바꾸는 함수를 만든다. 이 계산만으로 Scene이 변경되지는 않는다.',
 'axis.point는 회전축 위의 한 점, direction은 축 방향이다. u는 시간 자체가 아니라 경로 진행률이다. smoothStep은 u를 느리게 시작·끝나는 진행률로 바꾼다.',r'''
void calculateTransforms() {
    graphics::AxisLine axis{{0,0,0},{0,1,0}};
    auto t=graphics::TransformSystem::translation({1,0,0});
    auto s=graphics::TransformSystem::scaling({2,1,1},{0,0,0});
    auto r=graphics::TransformSystem::rotation(axis,90);
    auto point=graphics::TransformSystem::reflectPoint({0,0,0});
    auto line=graphics::TransformSystem::reflectLine(axis);
    auto plane=graphics::TransformSystem::reflectPlane({0,0,0},{1,0,0});
    if(!t||!s||!r||!point||!line||!plane) return;
    bool affine=graphics::TransformSystem::validAffine(*t * *r * *s); (void)affine;
    auto circle=graphics::PathSystem::circle({0,0,0},2);
    auto ellipse=graphics::PathSystem::ellipse({0,0,0},{2,0,0},{0,0,1});
    auto polyline=graphics::PathSystem::polyline({{0,0,0},{1,0,0},{1,0,1}});
    auto polygon=graphics::PathSystem::polygon({{0,0,0},{1,0,0},{0,0,1}});
    auto rectangle=graphics::PathSystem::rectangle({0,0,0},{1,0,0},{0,0,1});
    if(!circle||!ellipse||!polyline||!polygon||!rectangle) return;
    auto u=graphics::PathSystem::linear(.25);
    auto eased=graphics::PathSystem::smoothStep(u);
    glm::vec3 position=circle(eased); (void)position;
}
''','optional 행렬이 비어 있거나 경로 std::function이 비면 입력이 유효하지 않다. 0 길이 축이나 0 반지름을 넘긴 실패를 identity 행렬 성공으로 간주하지 않는다.',('framework/detail/Transform.inl',))

group('motion','한 물체의 모션 구성·편집·샘플링',
 '상자가 2초 이동한 뒤 1초 회전하는 모션을 편집한다. append는 단계를 배열에 더할 뿐 시작 시간을 뒤로 밀지 않는다. 순차 실행은 then을 사용한다.',
 'Motion은 단계들의 값 묶음이다. MotionStep의 콜백이 받는 초는 해당 단계가 시작된 이후의 시간이다. sample(2.5)는 dt=.5가 아니라 모션 시작 후 2.5초의 자세를 요구한다.',r'''
std::optional<graphics::Motion> makeMotion() {
    graphics::Motion motion;
    graphics::LinearMotion move; move.to={2,0,0}; move.durationSeconds=2;
    if(!motion.append(graphics::Motion::linear(move))) return {};
    graphics::RotationMotion spin; spin.degreesPerSecond=90;
    auto rotate=graphics::Motion::rotation(spin); rotate.durationSeconds=1;
    if(!motion.then(rotate)) return {};
    auto fixed=graphics::Motion::fixed(glm::mat4(1));
    if(!motion.insert(0,fixed)||!motion.replace(0,fixed)||!motion.erase(0)) return {};
    const auto& steps=motion.getSteps(); (void)steps;
    auto pose=motion.sample(2.5); if(!pose) return {};
    graphics::PathMotion path; path.path=graphics::PathSystem::circle({0,0,0},1);
    graphics::Motion looping;
    if(!looping.append(graphics::Motion::followPath(path))) return {};
    graphics::PendulumMotion sway; sway.periodSeconds=3;
    graphics::Motion pendulum;
    if(!pendulum.append(graphics::Motion::pendulum(sway))) return {};
    return motion;
}
''','유한 종료 시간이 없는 루프 단계 뒤에는 then을 사용할 수 없다. Once 단계는 종료 자세를 유지한다. 진자는 중력 적분이 아니라 지정 주기의 sin 각도 함수다.',('framework/detail/Motion.inl',))

group('layers','모션 시계와 레이어를 연결·편집·해제하기',
 '두 물체에 같은 경로 시계를 연결하되 한 분기만 반사시키는 경우다. MotionId는 시간 상태를 찾고 LayerId는 변환 합성 경로를 찾는다. 같은 모션을 공유하면 위상도 공유한다.',
 'system은 Scene 안의 MotionSystem이다. rootLayer는 삭제할 수 없는 0번 루트다. parent는 변환 트리 부모이며 motion의 World/Local 기여는 합성 위치를 나눈다.',r'''
bool connectMotion(graphics::Scene& scene,graphics::EntityId entity) {
    auto& system=scene.getMotions();
    const auto& constSystem=static_cast<const graphics::Scene&>(scene).getMotions(); (void)constSystem;
    graphics::Motion motion;
    graphics::RotationMotion rotation;
    if(!motion.append(graphics::Motion::rotation(rotation))) return false;
    auto id=system.createMotion(motion); if(!id) return false;
    if(!system.setMotion(*id,motion)||!system.seekMotion(*id,1)
       ||!system.setMotionPlaying(*id,false)||!system.setMotionEnabled(*id,true)) return false;
    const auto* state=system.getMotion(*id); if(!state) return false;
    auto layer=system.pushLayer(graphics::MotionSystem::rootLayer()); if(!layer) return false;
    graphics::LayerConfig config; config.motion=*id;
    if(!system.setLayer(*layer,config)||!system.bindMotion(*layer,*id)) return false;
    auto inserted=system.insertLayer(*layer); if(!inserted) return false;
    if(!system.reparentLayer(*layer,graphics::MotionSystem::rootLayer())
       ||!system.removeLayer(*inserted)) return false;
    const auto* layerState=system.getLayer(*layer); (void)layerState;
    auto matrices=system.layerMatrices(*layer); if(!matrices) return false;
    if(!scene.bindEntity(entity,*layer)) return false;
    auto binding=scene.getEntityLayer(entity); (void)binding;
    if(!system.update(.016)||!scene.updateMotions(.016)) return false;
    // 위 두 호출은 수동 진행의 시연이다. Application 루프에서는 자동 진행하므로 호출하지 않는다.
    if(!scene.unbindEntity(entity)||!system.bindMotion(*layer,std::nullopt)) return false;
    if(!system.popLayer(*layer)||!system.removeMotion(*id)) return false;
    auto spin=system.createSpin(rotation);
    auto orbit=system.createOrbit(rotation);
    auto combined=system.createOrbitAndSpin(rotation,rotation);
    graphics::PathMotion path; path.path=graphics::PathSystem::circle({0,0,0},1);
    auto moving=system.createPathAndSpin(path,rotation);
    if(!spin||!orbit||!combined||!moving) return false;
    system.clear(); // 기존 레이어 연결을 사용하지 않는 별도 예제 종료 지점
    return true;
}
''','playing=false는 시간 정지, enabled=false는 변환 기여 제거다. popLayer는 자식 없는 레이어만 제거한다. removeLayer는 자식을 부모 쪽으로 승격한다. system.clear 전에 Scene 엔티티 바인딩을 해제해야 한다.',('standard/Console.cpp','framework/examples/motion.cpp'))

group('geometry','CPU 모델의 생성·형상 계약과 행렬 읽기',
 '직육면체와 구·원뿔을 Scene에 등록하기 전에 형상과 변환을 확인한다. DiagramSystem은 GPU 자원 소유자가 아니라 CPU 모델 생성·계산 도구다.',
 'model.faces는 여러 면/메시다. transform은 SRT와 모델 피벗을 포함한다. modelMatrix는 T(basis)·R·S·T(-localBasis)를 계산한다.',r'''
void inspectModel(const graphics::Component_Poly& component) {
    auto cube=graphics::DiagramSystem::makePolyhedron(graphics::GLShape::Cube,{1,1,1});
    auto sphere=graphics::DiagramSystem::makeSphere(.5f);
    auto cone=graphics::DiagramSystem::makeCone(.5f,1);
    bool valid=graphics::DiagramSystem::validPolyhedron(cube);
    graphics::Transform3D transform;
    valid=graphics::DiagramSystem::validTransform(transform)&&valid;
    auto matrix=graphics::DiagramSystem::modelMatrix(transform);
    if(!component.polyhedron.faces.empty()) {
        auto face=graphics::DiagramSystem::faceModelMatrix(component,component.polyhedron.faces[0]);
        (void)face;
    }
    (void)sphere;(void)cone;(void)valid;(void)matrix;
}
''','validPolyhedron은 면 구성의 유효성을, validTransform은 회전 기저와 수치의 유효성을 다룬다. 둘 중 하나의 성공이 다른 하나의 성공을 보장하지 않는다.',('framework/detail/Scene.inl',))

group('render','카메라와 직접 Renderer 사용',
 'Application 대신 자체 창을 소유하는 호스트가 Scene을 직접 그리는 상황이다. 아래 함수는 이미 OpenGL 3.3 컨텍스트가 현재인 동안만 호출한다. 일반 실습에서는 Application::renderScene/renderViews가 이 책임을 맡는다.',
 'camera.position은 시점, target은 바라보는 점, near/far는 보이는 깊이 범위다. RenderPass는 카메라·픽셀 사각형·옵션·clear 정책을 한 번의 드로우 묶음으로 전달한다.',r'''
bool renderInCurrentContext(const graphics::Scene& scene,int width,int height) {
    graphics::Camera camera; camera.position={4,3,6};
    if(!graphics::CameraSystem::validCamera(camera)||width<1||height<1) return false;
    auto view=graphics::CameraSystem::viewMatrix(camera);
    auto projection=graphics::CameraSystem::projectionMatrix(camera,float(width)/height);
    (void)view;(void)projection;
    graphics::Renderer renderer;
    if(!renderer.initialize()) return false;
    graphics::RenderOptions options; options.depthTest=true;
    bool ok=renderer.render(scene,camera,width,height,options);
    graphics::RenderPass pass; pass.camera=camera;pass.viewport={0,0,width,height};
    ok=renderer.renderPass(scene,pass,width,height)&&ok;
    ok=renderer.renderViews(scene,{pass},width,height)&&ok;
    renderer.destroy(); // 현재 컨텍스트가 유효한 정상 종료 경로
    return ok;
}
void onContextLost(graphics::Renderer& renderer) {
    renderer.abandon(); // GL 호출이 불가능한 별도 실패 경로: GPU delete를 호출하지 않는다.
}
// Renderer 복사 생성/대입은 =delete. 동일 GPU 핸들을 두 객체가 소유할 수 없다.
''','세 렌더 호출은 API별 사용 예를 순서대로 보여준다. 실제 한 프레임에서는 의도한 방식 하나를 선택한다. destroy와 abandon은 같은 종료 지점에서 연속 호출하는 API가 아니다.',('framework/detail/Renderer.inl','framework/detail/Shaders.h'))

group('pick','마우스 픽셀을 객체 선택과 배치 위치로 바꾸기',
 '왼쪽 클릭으로 선택한 상자의 면을 찾고, 빈 바닥 클릭은 Y=0 평면 위치로 바꾼다. 창 좌표와 framebuffer 픽셀은 DPI 배율 때문에 다를 수 있으므로 좌표계를 맞춘다.',
 'viewport는 왼쪽 위 원점의 픽셀 사각형이다. ray는 near plane에서 시작하며 direction은 정규화된다. hit에는 객체·면·삼각형 번호와 월드/로컬 교차점·UV가 들어 있다.',r'''
void selectAt(const graphics::Scene& scene,const graphics::Camera& camera,double x,double y) {
    graphics::PickViewport viewport{0,0,1100,800};
    auto ray=graphics::screenRay(camera,x,y,viewport);
    if(!ray) return;
    auto first=graphics::pick(scene,*ray);
    auto second=graphics::pick(scene,camera,x,y,viewport);
    auto floor=graphics::PatternSystem::screenOnPlane(camera,x,y,viewport,{0,0,0},{0,1,0});
    if(first) { auto id=first->entityId; auto face=first->faceIndex; (void)id;(void)face; }
    if(floor) { glm::vec3 placement=*floor; (void)placement; }
    (void)second;
}
''','화면 밖 좌표, 평행한 평면, 또는 물체를 맞히지 않은 클릭은 빈 optional이 정상 결과다. 빈 결과를 시스템 오류로 처리하지 않는다.',('framework/include/Picking.h','framework/examples/basic.cpp'))

group('assets','OBJ·이미지 로딩을 실제 초기화 시점에 연결하기',
 '외부 모델과 한 장의 텍스처를 불러온다. 상대경로는 실행 파일 기준 projectRoot 안에서 해석된다. 모델 파일에 담긴 피벗 결과를 addModel에 그대로 전달해야 발 기준 배치가 유지된다.',
 'loaded는 CPU 모델과 피벗·경고·오류를 묶은 ModelResult다. texture.image는 공유 이미지다. features.textures는 로딩 기능, RenderOptions.textures는 그리기 기능을 켠다.',r'''
bool loadAssets(graphics::Application& app) { // initHandler 안에서 호출
    auto& loader=app.getAssets();
    const auto& features=loader.features(); (void)features;
    graphics::LoadOptions options; options.texturePath="textures/robot.png";
    auto loaded=loader.loadObj("models/robot.obj",options);
    if(!loaded) { std::cerr<<loaded.error; return false; }
    auto texture=loader.loadTexture("textures/robot.png");
    if(!texture) { std::cerr<<texture.error; return false; }
    if(!texture.image->valid()) return false;
    auto id=app.getScene().addModel(*loaded.model,{},loaded.pivotMode,loaded.localBasis);
    app.getRenderOptions().textures=true;
    return id.has_value();
}
void standaloneLoader() {
    graphics::FeatureOptions features; features.textures=true;
    graphics::AssetLoader loader(".",features);
    auto a=loader.loadPrimitive("models/cube.obj");
    auto b=loader.loadModel("models/robot.obj");
    graphics::TextureImage pixel; pixel.width=1;pixel.height=1;pixel.rgba={255,255,255,255};
    bool valid=pixel.valid(); (void)a;(void)b;(void)valid;
}
// main에서 config.features.textures=true를 설정한 뒤 app.run(config)를 호출한다.
''','이 경로는 사용자가 준비해야 하는 사례 자산이다. loadPrimitive/loadModel도 현재는 OBJ 로더를 호출한다. FBX/glTF 로딩을 의미하지 않는다. 이미지 기능을 끄면 해당 로드는 error를 돌려준다.',('framework/examples/assets.cpp','framework/include/ModelLoader.h'))

group('diagnostics','실패한 작업의 위치·원인·조치 남기기',
 '장면 조립 실패가 어느 함수에서 발생했는지 찾는 상황이다. bool만 출력하지 않고 소스 위치와 객체 ID를 같이 기록하면 잘못된 값이 들어온 경로를 추적할 수 있다.',
 'context는 다음 오류에 붙는 문맥이다. lastError는 마지막 오류 기록이며 clear는 설정을 끄는 동작이 아니다. trace 옵션은 단계 진입 로그의 출력 여부다.',r'''
void explainFailure() {
    graphics::Diagnostics::configure({true,true});
    graphics::Diagnostics::locate(__FILE__,__LINE__,"robot initialization");
    auto operation=graphics::Diagnostics::context().operation;
    graphics::Diagnostics::trace(graphics::PipelineStage::Scene,"attach head");
    // 아래는 의도적으로 실패 기록을 만드는 진단 사용 사례다.
    bool accepted=graphics::Diagnostics::failAt(__FILE__,__LINE__,graphics::PipelineStage::Scene,
        "attach head","부모 ID가 없음","부모를 먼저 생성",42);
    accepted=graphics::Diagnostics::fail(graphics::PipelineStage::Motion,"walk","주기가 0","양수 주기 지정");
    const auto& last=graphics::Diagnostics::lastError();
    if(last) std::cerr<<graphics::Diagnostics::stageName(last->stage)<<": "<<last->cause;
    graphics::Diagnostics::clear();
    (void)operation;(void)accepted;
}
''','fail/failAt은 false를 반환하므로 호출 함수의 실패 반환으로 연결할 수 있다. trace가 출력되었다는 사실은 GPU 결과가 맞다는 증거가 아니다.',('framework/include/Diagnostics.h',))

group('grid','격자 비율·셀 번호로 반복 배치하기',
 '가로 10칸 바닥에 장애물을 배치한다. 셀 번호는 이산 인덱스이고 비율은 전체 영역을 0~1로 표현하는 연속 값이다. 두 입력의 의미를 섞지 않는다.',
 'origin은 최소 모서리, extent는 전체 크기, cells는 분할 수다. frame은 격자 좌표를 월드로 바꾼다. parentWorld는 배치할 부모의 월드 행렬이다.',r'''
bool placeOnGrid(graphics::Scene& scene) {
    graphics::GridLayout grid; grid.origin={-5,0,-5}; grid.extent={10,2,10};grid.cells={10,2,10};
    if(!grid.valid()) return false;
    auto middle=grid.position({.5f,0,.5f});
    auto cell=grid.cellCenter({2,0,3});
    auto size=grid.relativeSize({.1f,.25f,.1f});
    auto local=grid.positionInParent({.5f,0,.5f},glm::mat4(1));
    if(!middle||!cell||!size||!local) return false;
    auto index=grid.cellAt(*cell); if(!index) return false;
    auto order=graphics::PatternSystem::snakeCells({10,1,10}); (void)order;
    auto snapped=graphics::PatternSystem::snap(*cell,1);
    bool aligned=graphics::PatternSystem::aligned(snapped.x,cell->x,.01f); (void)aligned;
    return graphics::PatternSystem::addBox(scene,*size,*cell,{.6f,.7f,.8f}).has_value();
}
''','cellCenter({2,0,3})는 (-2.5,.5,-1.5)다. relativeSize는 로컬 크기를 반환하므로 frame의 비균일 배율을 이미 적용한 월드 크기로 착각하지 않는다.',('standard/Console.cpp','framework/include/GridLayout.h'))

group('distance','하나의 거리 시계로 컨베이어 물체 배치하기',
 '같은 컨베이어 위의 상자 다섯 개가 같은 속도로 움직이면서 간격을 유지한다. 각 상자의 시간을 따로 진행하지 않고 공통 누적 시간에 거리 간격을 더한다.',
 'clock.seconds는 재생 중인 누적 초, path.length는 전체 길이, distance는 속도×시간이다. PathPose는 위치뿐 아니라 접선과 yaw를 함께 담아 코너의 진행 방향을 알려준다.',r'''
bool prepareBelt(graphics::AnimationClock& clock,graphics::DistancePath& path) {
    clock.reset();
    return path.setPoints({{-2,0,0},{2,0,0},{2,0,3}},false);
}
bool placeBelt(graphics::Scene& scene,graphics::EntityId entity,
               graphics::AnimationClock& clock,const graphics::DistancePath& path,double dt) {
    if(!clock.advance(dt)) return false;
    double distance=clock.seconds+path.length()/5;
    auto pose=path.sample(distance,graphics::Playback::Loop); if(!pose) return false;
    if(!graphics::PatternSystem::follow(scene,entity,path,distance)) return false;
    auto corner=graphics::PatternSystem::conveyorCornerPose(1,{-2,1,-2},{1,1,-1},1,3);
    return corner.has_value();
}
''','clock과 path는 Console 멤버로 보관하고 prepareBelt는 초기화에 한 번 호출한다. 실제 실습 20의 전체 코드는 아래 소스에서 상자·줄무늬가 시계를 공유하는 부분을 보여준다.',('openGL_3dAssign_20/Console.h','openGL_3dAssign_20/Console.cpp'))

group('room','회전하는 방의 로컬 운동과 이탈 후 월드 낙하',
 '방 안의 상자는 방과 함께 회전하지만 바닥을 열고 이탈하면 월드 중력을 받는다. released가 좌표계 전환 여부를 결정한다.',
 'Bounds3는 축 정렬 최소·최대 경계다. localPosition은 방 좌표의 중심, worldPosition은 이탈 후 월드 중심이다. roomWorld는 방의 현재 강체 변환이다.',r'''
bool moveInsideRoom(graphics::Scene& scene) {
    graphics::Bounds3 bounds{{-2,-1,-2},{2,2,2}};
    if(!bounds.valid()||!bounds.contains({0,0,0},{.2f,.2f,.2f})) return false;
    graphics::Bounds3 other{{-.5f,-.5f,-.5f},{.5f,.5f,.5f}};
    bool overlap=bounds.overlaps(other); (void)overlap;
    auto room=graphics::PatternSystem::addRoom(scene,bounds,true);
    auto box=graphics::PatternSystem::addBox(scene,{.4f,.4f,.4f},{0,0,0},{1,.6f,.2f});
    if(!room||!box) return false;
    graphics::RoomBody body; body.halfExtent={.2f,.2f,.2f};
    auto roomWorld=glm::rotate(glm::mat4(1),.2f,glm::vec3(0,0,1));
    if(!graphics::CollisionSystem::stepRoomBody(body,bounds,roomWorld,.016f,false,false)) return false;
    return scene.setPosition(*box,body.position(roomWorld));
}
''','상자 표시를 방의 자식으로 둔 채 계산된 월드 위치를 다시 넣으면 방 회전이 두 번 적용될 수 있다. 예제 상자는 부모 없는 객체다. 실제 시간 진행에서는 body를 프레임 간 보관한다.',('openGL_3dAssign_21/Console.h','openGL_3dAssign_21/Console.cpp'))

group('character','충돌 상태를 계산한 뒤 보행 관절에 반영하기',
 '로봇이 장애물을 피하며 걷다가 점프해 상자 위에 착지한다. 충돌 계산용 CharacterBody와 표시용 JointRig를 분리한다. 메시를 회전시키는 것만으로 충돌 높이가 바뀌지는 않는다.',
 'feet는 발의 월드 위치, direction은 수평 진행 방향, verticalSpeed는 점프 속도다. walkDistance는 실제 이동한 거리이며 보행 위상을 만든다. occupied는 생성할 장애물이 피해야 하는 경계 목록이다.',r'''
bool walkOneFrame(graphics::Scene& scene,graphics::CharacterBody& body,
                  const graphics::JointRig& rig,const std::vector<graphics::Bounds3>& obstacles,float dt) {
    graphics::Bounds3 stage{{-5,0,-5},{5,4,5}};
    auto result=graphics::CollisionSystem::stepCharacter(body,stage,obstacles,dt);
    if(!result) return false;
    float yaw=glm::degrees(std::atan2(body.direction.x,body.direction.z));
    return graphics::PatternSystem::poseWalkingRig(scene,rig,body.feet,yaw,body.walkDistance,body.speed);
}
void requestJump(graphics::CharacterBody& body) {
    bool accepted=graphics::CollisionSystem::jump(body,4); (void)accepted;
}
bool createWalker(graphics::Scene& scene) {
    graphics::GridLayout grid;grid.origin={-.5f,0,-.3f};grid.extent={1,1.8f,.6f};
    auto rig=graphics::PatternSystem::addWalkingRig(scene,grid); if(!rig) return false;
    std::mt19937 random(42); std::vector<graphics::Bounds3> occupied;
    auto obstacle=graphics::CollisionSystem::randomFreeBox({{-4,0,-4},{4,2,4}},{1,1,1},occupied,random);
    return obstacle.has_value();
}
''','공중 jump의 false는 정상적인 입력 거절이다. stepCharacter의 빈 optional은 계산 입력 오류다. randomFreeBox도 제한 횟수 안에서 배치하지 못하면 실패하므로 무한 재추첨하지 않는다.',('openGL_3dAssign_23/Console.h','openGL_3dAssign_23/Console.cpp'))

group('parts','부품·면·카메라 조작을 합성하기',
 '몸체의 자식 관절을 만들고 문을 열거나 카메라를 돌린다. 이 함수들은 응용의 키 입력이나 누적 진행률을 Scene·Camera 상태로 바꾸는 작은 재사용 도구다.',
 'anchor는 부모 공간의 관절 위치다. progress는 0~1 개방률이며 dt가 아니다. cameraFacingWall의 결과는 선택한 옆벽 번호다. selected가 비면 벽을 임의로 열지 않는다.',r'''
bool editParts(graphics::Scene& scene,graphics::EntityId body,graphics::EntityId room,graphics::Camera& camera) {
    auto joint=graphics::PatternSystem::addJoint(scene,body,{0,.5f,0});
    if(!joint||!graphics::PatternSystem::rotateJoint(scene,*joint,{1,0,0},30)) return false;
    auto polygon=graphics::PatternSystem::addPolygon(scene,{{0,0,0},{1,0,0},{0,1,0}},{1,1,0},body);
    if(!polygon||!graphics::PatternSystem::addNumber(scene,3,{0,1,0},.2f,{1,1,1},body)) return false;
    if(!graphics::PatternSystem::foldFace(scene,body,0,{{0,.5f,0},{1,0,0}},45)) return false;
    if(!graphics::PatternSystem::openFace(scene,body,0,.5f,{0,1,0},{0,.5f,0},true)) return false;
    auto selected=graphics::PatternSystem::cameraFacingWall(scene,room,camera);
    if(selected&&!graphics::PatternSystem::openRoomWall(scene,room,*selected,.5f)) return false;
    auto other=scene.addCuboid({1,1,1},{2,0,0}); if(!other) return false;
    if(!graphics::PatternSystem::swapModels(scene,body,*other)) return false;
    if(!graphics::PatternSystem::moveCamera(camera,{.2f,0,0})
       ||!graphics::PatternSystem::yawCamera(camera,5)
       ||!graphics::PatternSystem::orbitCamera(camera,{{0,0,0},{0,1,0}},5)
       ||!graphics::PatternSystem::zoomCamera(camera,.9f)) return false;
    auto views=graphics::PatternSystem::threeViews(camera,1100,800); return !views.empty();
}
''','이 단편의 각 조작은 별도의 입력 명령 사례다. 실제 앱에서는 같은 면을 foldFace와 openFace가 동시에 덮어쓰지 않게 모드를 정한다. 벽 표시 개방은 충돌 경계 제거와 별개다.',('openGL_3dAssign_22/Console.cpp','openGL_3dAssign_23/Console.cpp'))

group('animation','편집 가능한 사람형과 그네를 같은 관절 시스템으로 갱신하기',
 '그네 좌석에 앉은 사람을 만들고 상체·팔꿈치를 따로 움직인다. 사람 루트를 좌석의 자식으로 연결하므로 그네 각도를 사람의 모든 관절에 다시 수동 계산할 필요가 없다.',
 'HumanoidRig는 관절과 표시 메시의 ID 묶음이다. JointAnimation.rest는 생성 시 캡처한 기준 자세이며, 매 프레임 이 자세에서 다시 계산해 회전 누적 오차를 피한다. animation은 매 프레임 샘플링할 트랙 목록이다.',r'''
struct SwingExample {
    graphics::AnimationClock clock;
    std::vector<graphics::JointAnimation> animation;
    bool initialize(graphics::Scene& scene) {
        graphics::HumanoidConfig personConfig;
        personConfig.grid.frame=glm::translate(glm::mat4(1),glm::vec3(-2,0,0));
        auto person=graphics::AnimationSystem::addHumanoid(scene,personConfig); if(!person) return false;
        auto pose=graphics::HumanoidPose::seated();
        if(!graphics::AnimationSystem::poseHumanoid(scene,*person,pose)) return false;
        graphics::SwingConfig config;config.pivot={1,3,0};config.pendulum.periodSeconds=3;
        auto swing=graphics::AnimationSystem::addSwingWithRider(scene,config); if(!swing) return false;
        animation=swing->animation;
        graphics::PendulumMotion elbow;elbow.amplitudeDegrees=10;
        graphics::Motion motion;if(!motion.append(graphics::Motion::pendulum(elbow))) return false;
        auto track=graphics::AnimationSystem::createJointAnimation(scene,swing->person.elbows[0],motion);
        if(!track) return false;
        animation.push_back(*track);clock.reset();return true;
    }
    bool update(graphics::Scene& scene,float dt) {
        return clock.advance(dt)&&graphics::AnimationSystem::poseHierarchy(scene,animation,clock.seconds);
    }
};
''','initialize는 initHandler에서, update는 updateHandler에서 호출한다. 샘플은 강체 회전·이동이며 기준 rest는 양수 균일 배율과 피벗 편집을 허용한다. 비균일 배율·shear는 거부한다. standard 9번은 35개 관절/프레임을 선택해 rest를 편집하고, 매 프레임 같은 rest에서 계산하므로 입력이 덮어써지지 않는다. 전체 연결 코드는 아래에 있다.',('standard/Console.h','standard/Console.cpp','standard/main.cpp','framework/detail/Animation.inl'))

group('text','셰이더 파일의 원문을 읽기',
 '외부 셰이더를 직접 관리하는 도구에서 파일 내용을 읽는다. readTextFile은 문자열을 읽을 뿐 셰이더 컴파일·링크를 수행하지 않는다.',
 'path는 읽을 파일 경로이고 contents는 CPU 문자열이다. 구현은 파일을 열지 못하면 빈 문자열을 반환한다. 따라서 빈 파일과 열기 실패는 이 반환값만으로 구분할 수 없다.',r'''
void showShaderText(const std::filesystem::path& path) {
    auto contents=graphics::readTextFile(path);
    if(contents.empty()) { std::cerr<<"비어 있거나 읽을 수 없는 셰이더 파일"; return; }
    std::cout<<contents;
}
''','GPU에서 사용하려면 별도의 셰이더 생성·컴파일·링크 단계가 필요하다. 함수는 파일 앞의 UTF-8 BOM을 제거한다. 기본 Application은 경로가 비어 있으면 내장 셰이더를 사용한다.',('framework/detail/Renderer.inl',))

group('jointedit','선택한 관절의 SRT·피벗을 애니메이션과 함께 편집하기',
 '그네를 멈추고 왼쪽 팔꿈치만 돌린 뒤 다시 재생한다. Scene 변환을 직접 덮어쓰면 다음 poseHierarchy가 원래 rest로 되돌리므로, 입력은 선택한 트랙의 rest를 수정한다.',
 'selectedTrack은 animation 배열 인덱스이며 EntityId와 다르다. axis는 0/1/2의 X/Y/Z다. basis는 부모 공간 이동, localBasis는 모델 피벗, scale은 양수 균일 배율이다. rest.rotation에 오른쪽으로 회전을 곱하면 선택 관절의 로컬 축을 사용한다.',r'''
bool rotateSelected(graphics::Scene& scene,std::vector<graphics::JointAnimation>& animation,
                    std::size_t selectedTrack,int axis,double seconds) {
    if(selectedTrack>=animation.size()||axis<0||axis>2) return false;
    auto& track=animation[selectedTrack];
    const auto saved=track.rest;
    glm::vec3 direction(0);direction[axis]=1;
    track.rest.rotation=track.rest.rotation*glm::mat3(
        glm::rotate(glm::mat4(1),glm::radians(5.0f),direction));
    if(!graphics::AnimationSystem::poseHierarchy(scene,animation,seconds)) {
        track.rest=saved;return false;
    }
    return true;
}
// 이동 입력: track.rest.basis[axis] += .05f;
// 피벗 입력: track.rest.localBasis[axis] += .05f;
// 배율 입력: track.rest.scale = glm::vec3(1.1f * track.rest.scale.x);
// 실제 키 선택·배율 제한·복원은 아래 standard::Console 소스를 따른다.
''','시계를 정지해도 seconds는 유지되므로 같은 시각에서 편집 결과를 즉시 볼 수 있다. 입력 후 재생을 재개해도 rest가 남는다. standard의 handleJointKey는 실패 시 rest를 복원하고, R은 선택 관절의 최초 rest를 복원한다.',('standard/Console.h','standard/Console.cpp'))

def choose(a):
    o,n=a['owner'],a['name']
    if o=='Application': return 'assets' if n=='getAssets' else 'app'
    if o=='Scene':
        if n in 'setFaceVisible setFaceCullMode setFaceLocalMatrix setPosition setTransform setParent worldMatrix faceWorldMatrix setFaceTransform setLocalBasis splitFace setFaceSRT addGroup'.split():return 'transform'
        if n in 'setMesh getMesh setMeshVertices setVertexPosition setTriangleIndices generateTexcoords transformTexcoords setTexcoords setVertexNormals setMaterial setOutline setFaceVertexColors'.split():return 'mesh'
        if n in 'addLine addVector addBasis setVector getLines lineWorldMatrix'.split():return 'lines'
        if n in 'getMotions bindEntity unbindEntity getEntityLayer updateMotions'.split():return 'layers'
        return 'scene'
    if o in ('DiagramSystem',):return 'geometry'
    if o in ('CameraSystem','Renderer'):return 'render'
    if o in ('AssetLoader','ModelResult','TextureResult','TextureImage'):return 'assets'
    if o in ('PlaneFigure','Polyhedron'):return 'mesh'
    if o=='Motion':return 'motion'
    if o=='MotionSystem':return 'layers'
    if o=='Diagnostics':return 'diagnostics'
    if o=='GridLayout':return 'grid'
    if o in ('DistancePath','AnimationClock'):return 'distance'
    if o in ('Bounds3','RoomBody'):return 'room'
    if o=='CollisionSystem':return 'room' if n=='stepRoomBody' else 'character'
    if o in ('AnimationSystem','HumanoidPose'):return 'animation'
    if o=='TransformSystem' or o=='PathSystem':return 'math'
    if o=='PatternSystem':
        if n in ('snakeCells','aligned','snap'):return 'grid'
        if n in ('follow','conveyorCornerPose'):return 'distance'
        if n=='addRoom':return 'room'
        if n=='addBox':return 'grid'
        if n in ('addWalkingRig','poseWalkingRig'):return 'character'
        if n=='screenOnPlane':return 'pick'
        return 'parts'
    if not o:
        if n=='readTextFile':return 'text'
        if n in ('screenRay','pick'):return 'pick'
        return 'mesh'
    raise ValueError((o,n))

CSS='''*{box-sizing:border-box}body{margin:0;color:#1d2c40;background:#f4f6fa;font:16px/1.85 "Malgun Gothic",sans-serif}main{max-width:1160px;margin:auto;padding:40px 28px}h1{font-size:32px}h2{margin-top:48px;border-bottom:2px solid #c8d6e7;padding-bottom:10px}h3{margin-top:30px}a{color:#215b99}nav{display:flex;gap:14px;flex-wrap:wrap}section,article{scroll-margin-top:20px}pre{background:#17253a;color:#edf4ff;padding:20px;border-radius:10px;white-space:pre-wrap;overflow-wrap:anywhere;font:14px/1.65 Consolas,"Malgun Gothic",monospace}details{background:white;border:1px solid #ccd8e5;margin:16px 0;padding:12px 18px;border-radius:10px}summary{cursor:pointer;font-weight:bold}table{border-collapse:collapse;width:100%;background:white}td,th{border:1px solid #ccd8e5;text-align:left;padding:10px;vertical-align:top}.note{background:#e7effa;padding:16px 20px;border-left:4px solid #477bb9}.flow{overflow:auto;margin:18px 0}svg{display:block;max-width:100%;min-width:680px;height:auto}.badge{font-size:13px;color:#4e637e}input{font:inherit;width:100%;padding:12px;border:1px solid #99adc5;border-radius:6px}article{padding:12px 20px;margin:16px 0;background:white;border-radius:10px}.roles{color:#325578}@media print{body{background:white}main{padding:0}details{break-inside:avoid}pre{color:black;background:#f2f2f2}nav,input{display:none}svg{min-width:0}}'''
def page(title,body):
    return '<!doctype html><html lang="ko"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>'+esc(title)+'</title><style>'+CSS+'</style></head><body><main><h1>'+esc(title)+'</h1><nav><a href="API_REFERENCE.html">공통 구현·전체 API 사례</a><a href="IMPLEMENTATION_PATTERNS.html">응용패턴·그네 통합</a></nav>'+body+'<footer><p>2026-11-02 소스 기준. 작성 예제·소스 발췌는 빌드·테스트·브라우저 시각 확인을 수행하지 않았다. 사용자가 실행하여 확인한다.</p></footer></main></body></html>'

def flow(steps,ident):
    # A self-contained SVG works offline; no Mermaid renderer or CDN is required.
    w=1020; h=100+len(steps)*104; parts=[f'<div class="flow"><svg viewBox="0 0 {w} {h}" role="img" aria-label="{esc(ident)} 흐름도"><title>{esc(ident)} 흐름도</title><defs><marker id="{ident}-arrow" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto"><path d="M0 0 L10 5 L0 10Z" fill="#4d688a"/></marker></defs>']
    for i,label in enumerate(steps):
        y=30+i*104
        if i:parts.append(f'<path d="M510 {y-32} V{y}" stroke="#4d688a" stroke-width="2" marker-end="url(#{ident}-arrow)"/>')
        parts.append(f'<rect x="70" y="{y}" width="880" height="72" rx="10" fill="#e8f0fb" stroke="#6a8ebc"/><text x="510" y="{y+44}" text-anchor="middle" font-size="20" font-family="Malgun Gothic,sans-serif" fill="#1d2c40">{esc(label)}</text>')
    return ''.join(parts)+'</svg></div>'

def case(key,source_files=True):
    g=GROUPS[key]
    body='<section id="case-'+key+'"><h2>'+esc(g['title'])+'</h2>'+para(g['story'])+'<p class="roles"><strong>변수·객체 역할.</strong> '+esc(g['roles'])+'</p><p class="badge">새로 작성한 사용 사례 · 단편별 전제는 본문 참조 · 빌드하지 않음</p>'+code(g['code'])+para(g['after'])
    if source_files:
        body+=''.join(source(p) for p in g['files'])
    return body+'</section>'

def runtime():
    sections=[
      ('loop','렌더링 루프: 등록한 함수는 언제 실행되는가?',
       '상자를 회전시킬 때 setUpdateHandler를 main에서 호출해도 즉시 상자가 돌지 않는다. 이 함수는 std::function 멤버에 람다를 저장한다. run이 창과 Renderer를 초기화한 뒤 initHandler를 한 번 실행하고, 이후 매 프레임 updateHandler를 호출한다. 따라서 초기화 콜백에서 box ID를 만들고 update 콜백에서 같은 ID를 사용하는 수명 관계가 성립한다.',
       'previousTime은 직전 유효 프레임의 시간 기준이고 currentTime은 이번 GLFW 시간이다. 두 값의 차이를 [0,.05]초로 제한한 deltaTime은 최소화 복귀 때 갑자기 큰 이동이 일어나는 것을 줄인다. 최소화에서는 WaitEvents로 기다린 뒤 previousTime을 다시 설정한다. PollEvents는 이벤트가 있을 때 입력 콜백을 실행하며, set...Handler 등록 순서대로 콜백을 모두 실행하는 구조가 아니다.',
       ['main: Application·응용 상태 생성 → 콜백 저장','run: 컨텍스트·Renderer 초기화 → initHandler 한 번','반복: PollEvents → 종료·최소화 처리','dt 제한 → updateHandler → Scene.updateMotions','사용자 renderHandler 또는 기본 renderScene → SwapBuffers','종료·실패: shutdown → GPU·창·GLFW 정리'],['framework/detail/Application.inl']),
      ('ownership','수명·소유권: ID와 포인터를 왜 구분하는가?',
       'Console이 app.getScene()으로 받은 참조는 Application 내부 Scene을 빌려 쓰는 것이다. Console이 상자를 소유하는 것이 아니다. addCuboid의 반환 ID는 다음 프레임에 그 컴포넌트를 찾기 위한 번호다. getComponent가 반환하는 포인터는 조회 시점의 컨테이너 원소를 가리키므로 장면 대입·재구성 후에는 다시 조회한다.',
       '예를 들어 addSwingWithRider는 임시 Scene에서 사람·좌석·관절을 모두 만든 뒤 성공할 때만 원래 Scene을 교체한다. 기존 EntityId 관계는 복사되어 유지되지만 기존 컴포넌트 포인터는 무효화될 수 있다. Scene::clear는 엔티티 ID를 재사용하므로 저장한 rig와 트랙도 다시 만든다. Application/Renderer의 복사 금지는 창과 GPU 핸들을 중복 소유하는 일을 막는다.',
       ['main 소유: Application + Console','Application 소유: Scene·Camera·Renderer·콜백','Console 보관: 키 상태·누적 시간·EntityId','Scene 소유: CPU 모델·부모 관계·MotionSystem','Renderer 소유: program·VAO/VBO/EBO·GPU texture'],['framework/include/Application.h','framework/include/Graphics.h']),
      ('matrix','계층 변환: 좌석 위 사람의 정점은 어떻게 움직이는가?',
       'seat의 자식인 person.root의 basis는 좌석 좌표의 위치다. 사람 머리 정점에는 사람 로컬 변환뿐 아니라 좌석·진자 관절·고정 프레임의 변환도 차례로 적용된다. 모델 로컬 행렬은 T(basis)·R·S·T(-localBasis)이다. localBasis는 회전·축소의 기준점이며 화면의 위치와 구분한다.',
       'Scene::worldMatrix는 부모 체인을 수집하고 기존 모션 레이어의 parent/local 기여를 연결 위치 앞뒤에 합성한다. 새 poseHierarchy는 각 관절의 sample.parent * rest * sample.local을 계산해 Scene 로컬 변환에 반영한다. 이 함수에서 World 기여는 관절 부모 공간이다. 같은 관절을 모션 레이어와 update 코드에서 동시에 조작하지 않게 책임을 정한다. 매 프레임 이전 회전에 또 회전을 곱하지 않고 rest에서 다시 샘플링하는 것이 핵심이다.',
       ['누적 seconds → Motion.sample','관절 로컬 = sample.parent × rest × sample.local','Scene: 부모 체인 + 연결 레이어 합성 → 모델 M','카메라 V · 투영 P → 클립 좌표 P×V×M×정점'],['framework/detail/Animation.inl','framework/detail/Scene.inl']),
      ('upload','GPU 업로드: transform 변경과 정점 변경의 비용 차이',
       '상자가 이동할 때 setTransform은 CPU 컴포넌트의 변환을 바꾼다. 같은 상자의 정점 배열을 매 프레임 다시 만들 필요는 없다. 반면 setMeshVertices나 setModel로 형상을 바꾸면 geometry revision이 변하고 Renderer가 다음 렌더링에서 업로드를 다시 준비한다. 서로 다른 Scene을 같은 revision 숫자만으로 혼동하지 않도록 Scene 포인터도 비교한다.',
       'upload는 CPU Mesh를 GPU 정점 배열과 인덱스 배열로 펼친다. VAO는 attribute 형식과 버퍼 연결 상태를 보관하고, VBO는 정점 바이트를, EBO는 삼각형·외곽선 인덱스를 보관한다. DrawRange는 한 엔티티의 한 면이 어느 인덱스 구간을 그릴지 기록한다. 프로그램을 만들었다는 사실과 어떤 VAO/EBO가 현재 바인딩되어 있는지는 별개의 상태다.',
       ['Scene·geometry revision·텍스처 모드 확인','형상 변경 시 CPU Mesh → GPU 정점·인덱스 배열','VBO/EBO 업로드 → VAO attribute 구성','면별 DrawRange 저장 → draw에서 구간 사용'],['framework/detail/Renderer.inl']),
      ('shader','드로우·셰이더: 행렬과 법선은 어디서 소비되는가?',
       'Renderer::draw는 객체별 M과 카메라 V/P로 uMVP를 계산한다. 정점 셰이더는 로컬 정점에 uMVP를 곱해 클립 좌표를 출력한다. 정점의 위치가 바뀌었다는 것과 픽셀이 화면에 보인다는 것은 다르다. 이후 조립·클리핑·원근 나눗셈·뷰포트·래스터화를 통과해야 프래그먼트 셰이더가 실행될 수 있다.',
       '비균일 배율이 있는 모델의 법선은 위치와 같은 4×4 곱셈만으로 변환하면 면에 수직인 성질이 깨질 수 있다. draw에서 준비한 normal matrix를 정점 셰이더가 적용한다. 프래그먼트 셰이더는 색·선택적 텍스처·방향광 N·L 명암을 계산한다. 깊이 검사는 카메라 기준 가림을 해결한다. 이 기본 셰이더는 다른 물체가 바닥에 드리우는 shadow map을 계산하지 않는다.',
       ['CPU: model/view/projection·normal matrix uniform','정점 셰이더: clip = P×V×M×local','조립 → 클리핑 → 원근 나눗셈 → 뷰포트 → 래스터화','프래그먼트 색·텍스처·조명 → 깊이·블렌딩 → 프레임버퍼'],['framework/detail/Shaders.h','framework/detail/Renderer.inl']),
      ('passes','다중 뷰: 왜 상태를 한 번만 갱신하는가?',
       '실습 22와 standard의 세 뷰는 같은 Scene을 원근·평면·정면 카메라로 반복해 그린다. 한 프레임에서 update를 세 번 실행하면 뷰마다 다른 시간의 자세가 보이므로, 시간 진행은 run의 update 단계에서 한 번 수행하고 렌더 패스는 읽기만 한다.',
       'RenderPass.viewport는 framebuffer의 왼쪽 아래 원점 픽셀이다. Picking의 PickViewport는 왼쪽 위 원점이므로 마우스 좌표를 대응할 때 Y 방향을 바꿔야 한다. 렌더 패스의 viewport/scissor와 clear 플래그는 어떤 픽셀을 그리거나 지울지 정한다. 같은 창의 한 컨텍스트에서 그리며, 뷰마다 독립 창이나 모션 시계를 만들지 않는다.',
       ['프레임 update: 최종 Scene 자세 결정','threeViews: 카메라·픽셀 영역·옵션 구성','Renderer.renderViews: 패스별 viewport/scissor/clear','같은 Scene을 카메라마다 draw → 마지막에 SwapBuffers'],['standard/Console.cpp','framework/detail/Renderer.inl']),
      ('cleanup','종료와 실패: 컨텍스트가 살아 있을 때 정리하기',
       'GPU 버퍼 삭제는 해당 OpenGL 컨텍스트가 현재인 동안 수행해야 한다. 창부터 파괴한 뒤 glDeleteBuffers를 호출하는 순서는 수명 계약을 어긴다. Application::shutdown은 유효 컨텍스트에서 Renderer::destroy를 수행하고 창과 GLFW를 정리한다.',
       '컨텍스트가 실패해 더 이상 안전하게 GL 호출을 할 수 없다면 abandon으로 CPU 쪽 핸들·캐시를 비우고 네이티브 창 파괴 경로에 회수를 맡긴다. abandon은 정상 destroy의 빠른 대체물이 아니다. 입력 콜백의 예외도 저장한 뒤 run의 정리 경로에서 처리하므로 예외가 났다고 소유 자원을 그대로 남기지 않는다.',
       ['정상: 현재 컨텍스트 확인 → Renderer.destroy','실패: GL 호출 불가 → Renderer.abandon','창 파괴 → GLFW 종료 → running 상태 해제'],['framework/detail/Application.inl'])]
    body='<section id="runtime"><h2>공통 구현: 입력에서 화면·종료까지</h2><p>아래는 실제 구현을 읽기 위한 설명이다. private 함수는 공개 API 목록과 구분하되 호출 순서와 책임을 상세히 연결한다.</p></section>'
    for ident,title,a,b,steps,files in sections:
        body+=f'<section id="runtime-{ident}"><h2>{esc(title)}</h2>'+para(a)+para(b)+flow(steps,'runtime-'+ident)+''.join(source(p) for p in files)+'</section>'
    return body

PATTERNS=[
 ('bootstrap','콜백으로 응용 상태 연결','app',['상태와 ID 보관','init에서 Scene 생성','입력으로 상태 변경','update로 변환 반영','렌더·종료는 Application에 위임']),
 ('spin-orbit','자전과 공전을 분리','layers',['공전축·자전축 지정','createOrbitAndSpin으로 모션 등록','레이어에 모션 연결','객체 바인딩 → 자동 시간 진행']),
 ('path','경로 전환·왕복','math',['PathSystem으로 경로 생성','PathMotion의 기간·재생 정책 지정','Motion.followPath 구성','MotionSystem.setMotion → seekMotion(0)']),
 ('sequence','순차 동작과 끝 자세 유지','motion',['유한 duration의 단계 생성','append로 첫 단계 등록','then으로 다음 시작 시각 배치','sample이 종료 자세를 유지']),
 ('shared','모션 공유와 대칭 분기','layers',['공통 MotionId 생성','정상·반사 레이어 분기','동일 MotionId 연결','같은 시각을 서로 다른 변환으로 표시']),
 ('pause','시간 정지와 변환 기여 끄기','layers',['정지 대상 MotionId 선택','playing은 시간 진행 제어','enabled는 변환 기여 제어','재개 시 유지된 시간에서 샘플링']),
 ('insert','바인딩을 유지한 레이어 삽입','layers',['바인딩된 말단 LayerId 보관','insertLayer로 부모 사이에 삽입','추가 배율·반사 설정','removeLayer로 자식을 부모 쪽에 승격']),
 ('parts','부품 계층과 소유자 기저선','transform',['몸체 그룹·부품 생성','setParent로 부모 관계 설정','부모 로컬 anchor 지정','부모 변환 → 모든 자식에 전달']),
 ('hinge','면 힌지와 추가 로컬 행렬','parts',['대상 면·힌지 축 지정','진행 각도 계산','foldFace로 추가 로컬 행렬 설정','면 월드 행렬로 렌더링']),
 ('pick','클릭 선택과 바닥 배치','pick',['창 좌표를 framebuffer 좌표로 대응','screenRay로 역투영','pick 또는 screenOnPlane','hit ID·월드 점으로 선택·배치']),
 ('mesh','메시·UV·재질 편집','mesh',['등록 전 Mesh 생성·UV 가공','Scene에 모델 등록','Scene setter로 변경 알림','다음 render에서 revision에 따라 업로드']),
 ('assets','선택적 OBJ·텍스처','assets',['config.features.textures 지정','init 안에서 getAssets','loadObj·loadTexture 결과 확인','피벗을 유지해 addModel','RenderOptions.textures로 표시']),
 ('linear','두 위치 사이 선형보간','motion',['from·to·편도 시간 지정','Playback 선택','Motion.linear로 단계 생성','MotionSystem 또는 관절 트랙에서 시간 샘플링']),
 ('pendulum','지정 주기의 진자 운동','motion',['축·진폭·주기·위상 설정','Motion.pendulum 생성','초 단위 절대 시간 샘플링','sin 각도를 로컬 회전에 반영']),
 ('grid','격자 비율·셀·지그재그 배치','grid',['origin·extent·cells 설정','셀 중심·로컬 크기 계산','snakeCells로 방문 순서 구성','Scene에 반복 배치']),
 ('conveyor','거리 기반 컨베이어와 코너 방향','distance',['공통 AnimationClock 진행','속도×시간 + 물체 간 거리 간격','DistancePath 또는 원호 pose 샘플링','위치·접선 yaw를 함께 반영']),
 ('room','회전하는 방과 이탈 후 낙하','room',['방 로컬 body 상태 보관','회전 공간에서 반사·중력 계산','바닥 개방 시 월드 상태로 전환','월드 낙하 → 표시 객체 동기화']),
 ('walker','보행·점프·장애물 착지','character',['CharacterBody와 JointRig 분리','입력으로 방향·점프 속도 지정','stepCharacter로 충돌·착지 계산','실제 거리로 poseWalkingRig']),
 ('wall','카메라 방향의 벽 개방','parts',['cameraFacingWall로 대상 벽 선택','선택 번호를 애니메이션 동안 보관','0~1 진행률로 openRoomWall','같은 면에 복원 적용']),
 ('humanoid','편집 가능한 사람형·관절 트랙','animation',['addHumanoid로 분할 관절 생성','poseHumanoid로 기준 자세 편집','createJointAnimation으로 rest 캡처','poseHierarchy로 관절별 모션 반영']),
 ('jointedit','관절 선택·SRT·피벗 편집','jointedit',['Tab으로 트랙 선택 → X/Y/Z 축 선택','입력으로 선택 track.rest의 SRT·피벗 편집','같은 seconds로 poseHierarchy → 실패 시 rest 복원','다음 프레임에도 편집된 rest에서 재생']),
 ('swing','그네(진자) → 좌석 → 사람','animation',['addSwingWithRider 설정','고정 frame → hinge → seat → person.root','앉은 자세와 그네·상체 트랙 구성','update: 누적 시간으로 poseHierarchy','부모 변환이 사람의 전체 관절에 전달']),
 ('views','여러 카메라로 같은 Scene 표시','parts',['update에서 Scene 상태 한 번 갱신','threeViews로 카메라·픽셀 영역 구성','Application.renderViews 호출','패스마다 같은 시간의 장면 표시'])]

def main():
    entries=apis()
    intro=para('함수 이름을 외우는 대신 실제 상황에서 무엇을 저장하고 무엇을 바꾸는지 읽는다. 공개 함수·오버로드·생성자·소멸자·삭제된 복사 연산을 현재 framework/include 헤더에서 수록했다. graphics::detail 및 private 구현은 공개 호출 대상으로 나열하지 않고 공통 구현 설명에서 다룬다.')
    intro+='<p class="note">모든 코드는 현재 선언·구현에 근거한 문서다. 새 사용 사례는 함께 쓸 API를 묶은 함수 단편이며, 독립 main인 예제는 별도로 표시한다. 실행 성공을 확인한 코드라는 뜻은 아니다. 외부 자산 경로는 사용자가 준비한다.</p>'
    body=intro+runtime()+'<section id="cases"><h2>함께 사용하는 API의 실제 사례 코드</h2>'+para('기본 포함은 framework/Framework.h다. 출력 사례는 iostream, 예외 처리는 stdexcept를 추가한다. 아래 독립 함수들은 graphics 이름을 완전히 적었고, 입력 Scene/ID/컨텍스트의 전제는 각 사례에 명시한다.')+'</section>'
    body+=''.join(case(k) for k in GROUPS)
    body+='<section id="api"><h2>전체 공개 API → 사례 연결</h2>'+para(f'현재 헤더에서 수록한 선언 항목은 {len(entries)}개다. 각 오버로드는 선언을 따로 표시하고 같은 사례에서 함께 설명한다. 삭제된 복사 연산은 호출 예제를 만드는 대신 금지 이유를 설명한다.')+'<label for="api-search">함수·클래스·설명 검색</label><input id="api-search" placeholder="예: setParent, Motion, 좌석"></section>'
    inventory=[];missing=[]
    for idx,a in enumerate(entries):
        key=choose(a);g=GROUPS[key];name=a['name'];qualified=(a['owner']+'::' if a['owner'] else '')+name
        lines=[line.strip() for line in g['code'].splitlines() if re.search(r'\b'+re.escape(name)+r'\s*\(',line)]
        deleted='= delete' in a['signature']
        if name.startswith('~') or deleted: lines=['// 소멸은 스코프 종료로 발생하며, =delete 복사 연산은 호출할 수 없다.']
        elif name=='operator bool':lines=['if (!loaded) { std::cerr << loaded.error; return false; }' if a['owner']=='ModelResult' else 'if (!texture) { std::cerr << texture.error; return false; }']
        elif name==a['owner']:
            lines=[l.strip() for l in g['code'].splitlines() if re.search(r'\b'+re.escape(name)+r'\s+\w+',l)]
        if not lines:missing.append(qualified)
        body+=f'<article class="api-entry" id="api-{idx}"><h3>{esc(qualified)}</h3>'+code(a['signature'])+para(a['description'])
        body+=para('사례 맥락: '+g['story'])+'<p><a href="#case-'+key+'">'+esc(g['title'])+' — 입력·관련 변수·전체 호출 묶음</a></p>'
        body+=code('\n'.join(lines)) if lines else para('이 함수의 호출 위치는 아래 연결된 전체 사례와 원본 선언에서 읽는다.')
        body+=para('호출 뒤 확인할 점: '+g['after'])+f'<p class="badge">선언 위치: {esc(a["file"])}:{a["line"]} · 반환형·기본값·const 여부는 위 선언을 따른다.</p></article>'
        inventory.append(dict(**a,case=key,example_lines=lines,callable=not deleted))
    body+='''<script>document.getElementById('api-search').addEventListener('input',e=>{const q=e.target.value.toLowerCase();document.querySelectorAll('.api-entry').forEach(n=>n.hidden=!n.textContent.toLowerCase().includes(q));});</script>'''
    api_html=page('공통 구현과 전체 API 사례 코드',body)
    patterns=intro+para('응용패턴은 목적 → 변수 역할 → 흐름도 → 응용 코드 → 예상 결과 순으로 짧게 읽는다. 반복되는 렌더링 루프·GPU 자원 구현은 API 문서의 공통 구현 절에서 자세히 설명한다.')
    patterns+='<nav>'+''.join(f'<a href="#pattern-{p[0]}">{esc(p[1])}</a>' for p in PATTERNS)+'</nav>'
    for ident,title,key,steps in PATTERNS:
        g=GROUPS[key]
        patterns+=f'<section id="pattern-{ident}"><h2>{esc(title)}</h2>'+para(g['story'])+para('주요 상태: '+g['roles'])+flow(steps,'pattern-'+ident)
        patterns+='<h3>실제 응용 코드 묶음</h3><p class="badge">새로 작성한 사용 사례. 같은 API를 쓰는 기존 실행 소스도 아래에 전체 첨부한다.</p>'+code(g['code'])+para(g['after'])
        patterns+=''.join(source(p) for p in g['files'])+f'<p><a href="API_REFERENCE.html#case-{key}">함수·반환값·실패 조건과 함께 읽기</a></p></section>'
    patterns+='<section id="sample"><h2>standard 전체 연결: main → Console 상태 → 콜백 구현</h2>'+para('9번이 그네·사람형·선형보간의 실제 실행 경로다. clock과 animation_은 Console 멤버이며 초기화 때 만들고 프레임 update에서 재사용한다. 아래 세 파일은 서로 연결되는 실제 소스다.')+source('standard/main.cpp')+source('standard/Console.h')+source('standard/Console.cpp')+'</section>'
    patterns_html=page('응용패턴 통합: 흐름도와 실제 코드',patterns)
    for directory in ('framework/docs','standard/docs'):
        write(directory+'/API_REFERENCE.html',api_html)
        write(directory+'/IMPLEMENTATION_PATTERNS.html',patterns_html)
        write(directory+'/API_CASE_MAP.json',json.dumps(inventory,ensure_ascii=False,indent=2)+'\n')
    print('Authored declarations:',len(entries),'case groups:',len(GROUPS),'patterns:',len(PATTERNS))
    print('Declarations needing explicit call text:',missing)
    write('framework-analysis/animation-api/API_CASE_INVENTORY.md','# 문서 작성 범위\n\n'+f'- 공개 선언: {len(entries)}개 (오버로드·수명·삭제 연산 포함)\n- 사례 묶음: {len(GROUPS)}개\n- 응용패턴과 SVG 흐름도: {len(PATTERNS)}개\n\n기준은 framework/include의 공개 API 주석이 붙은 선언이다. API_CASE_MAP.json은 선언별 사례 위치와 호출 코드를 연결한다. 외부 라이브러리·private·graphics::detail 함수는 공개 API에서 제외한다. API_KO.json과 API_CASE_MAP.json은 같은 최신 선언·사례 정보를 제공한다. 빌드·테스트·브라우저 확인은 수행하지 않았다.\n')

KEYS='''| 입력 | 실제 동작 |
|---|---|
| 0 / 7 / 8 / 9 | Motion / 격자·거리 / 회전 방 / 그네·사람형·선형보간 |
| 1~6 (Motion) | 타원·사각형·다각형·사용자 3D 경로·원·왕복 꺾은선 |
| Space | 현재 모드의 시간 정지·재개 |
| S / G / M / I (Motion) | 자전 기여 / 전체 공전 기여 / 반사 종류 / 레이어 삽입·삭제 |
| V / D / F | 3뷰 / 단계 로그 / 방 모드 바닥 개방 |
| Tab / Backspace (9번) | 다음 / 이전 관절·프레임 선택 |
| X / Y / Z (9번) | 편집 축 선택 |
| ← / → (9번) | 선택 관절 로컬 회전 -5° / +5° |
| ↑ / ↓ (9번) | 부모 좌표 basis 이동 +.05 / -.05 |
| PageUp / PageDown (9번) | 모델 좌표 localBasis 피벗 +.05 / -.05 |
| + 또는 = / - (9번) | 양수 균일 배율 확대 / 축소 (.1~3) |
| R (9번) | 선택 관절의 초기 기준 SRT·피벗 복원 |
| 9 재입력 | 장면·트랙·모든 편집값 전체 재생성 |
| 왼쪽 클릭 / H / Esc | 객체·면 선택 / 도움말 / 종료 |
'''

def refresh_standard():
    entries=json.loads(read('framework/docs/API_CASE_MAP.json'))
    api_md='# 공개 API와 실제 사용 사례\n\n현재 헤더 기준의 선언·오버로드를 모두 수록한다. 기본 include는 framework/Framework.h다. 문서 예제는 빌드하지 않았다.\n\n[공통 구현·사례 HTML](API_REFERENCE.html) · [응용패턴 HTML](IMPLEMENTATION_PATTERNS.html)\n\n'
    for key,g in GROUPS.items():
        api_md+='## '+g['title']+'\n\n'+g['story']+'\n\n'+g['roles']+'\n\n```cpp\n'+g['code'].strip()+'\n```\n\n'+g['after']+'\n\n'
        for a in entries:
            if a['case']==key:api_md+='- `'+(a['owner']+'::' if a['owner'] else '')+a['signature']+'` — '+a['description']+'\n'
        api_md+='\n'
    pattern_md='# 응용패턴·그네 통합 설명\n\n흐름도·실제 사용 코드·저장소 소스는 [통합 HTML](IMPLEMENTATION_PATTERNS.html)을 연다. 이 목록은 현재 API로 구현하는 순서를 요약한다.\n\n'
    for ident,title,key,steps in PATTERNS:
        g=GROUPS[key];pattern_md+='## '+title+'\n\n'+g['story']+'\n\n'+' → '.join(steps)+'\n\n```cpp\n'+g['code'].strip()+'\n```\n\n'+g['after']+'\n\n'
    animation_md='# 그네·사람형·계층 SRT 빠른 참조\n\n[통합 응용패턴과 실제 코드](IMPLEMENTATION_PATTERNS.html#pattern-swing) · [공통 구현·전체 API](API_REFERENCE.html)\n\n'
    animation_md+='사람은 seat의 자식이며 엉덩이 -90°, 무릎 +90°의 앉은 자세다. 허벅지 아래쪽을 좌석 윗면에 맞춘다. frame의 고정 지지대는 hinge의 진자 회전에 포함하지 않는다. torso는 독립 상체 트랙을 가진다.\n\n'
    animation_md+='계층은 frame → hinge → seat → person.root → pelvis → torso/hips다. 목·양어깨·팔꿈치·손·양엉덩이·무릎·발은 각각 편집 가능한 ID다. 머리·코와 팔다리 메시 ID는 관절 ID와 구분한다.\n\n'
    animation_md+='JointAnimation.rest는 초기 자세다. poseHierarchy는 sample.parent × rest × sample.local을 매 프레임 계산한다. 샘플은 강체 변환만, rest는 양수 균일 배율·이동·회전·피벗을 허용한다. 비균일 배율·shear는 거부한다. World 기여는 이 API에서 관절 부모 공간이다.\n\n'+KEYS+'\n'
    animation_md+='```cpp\n'+GROUPS['animation']['code'].strip()+'\n```\n\n'+GROUPS['animation']['after']+'\n\nScene::clear 후 rig와 트랙을 다시 만든다. parent 변경은 월드 자세를 자동 보존하지 않는다.\n'
    for directory in ('framework/docs','standard/docs','standard/framework/docs'):
        write(directory+'/API_KO.md',api_md)
        write(directory+'/API_KO.json',json.dumps(entries,ensure_ascii=False,indent=2)+'\n')
        write(directory+'/APPLICATION_PATTERNS.md',pattern_md)
        write(directory+'/APPLICATION_API.md','# 응용 API 읽는 순서\n\n1. [전체 API와 실제 사례](API_REFERENCE.html): 렌더링 루프·계층 행렬·GPU 업로드·자원 수명은 상세히 읽는다.\n2. [응용패턴 통합](IMPLEMENTATION_PATTERNS.html): 패턴별 흐름도와 응용 코드를 간결하게 읽는다.\n3. [그네·사람형 SRT](ANIMATION_API.md): 착석과 관절 조작을 확인한다.\n\n현재 Scene은 다중 뷰·면 컬링·관절·충돌·보행 API를 제공하며 실습 23은 구현되어 있다. 투영 그림자·FBX/glTF·MTL 자동 로드는 제공하지 않는다.\n')
        write(directory+'/ANIMATION_API.md',animation_md)
    for name in ('API_REFERENCE.html','IMPLEMENTATION_PATTERNS.html','API_CASE_MAP.json'):
        write('standard/framework/docs/'+name,read('framework/docs/'+name))
    write('standard/framework/docs/OBJ_LOADING.md','# 현재 배포본: 2026-11-02\n\n아래 OBJ·텍스처 계약은 현재 배포 헤더와 함께 읽는다. [전체 API 사례](API_REFERENCE.html#case-assets)와 [응용패턴](IMPLEMENTATION_PATTERNS.html#pattern-assets)에서 초기화 연결 코드를 제공한다. 빌드·실행은 사용자에게 맡긴다.\n\n'+read('framework/docs/OBJ_LOADING.md'))
    checks='''# 사용자 빌드·실행 확인 절차

이 문서는 예상 결과다. 에이전트는 빌드·테스트·앱 실행·브라우저 확인을 수행하지 않았다.

## 프로젝트와 시작

1. standard.sln을 연다. .slnx를 지원하는 VS에서는 standard.slnx도 사용할 수 있다.
2. C++ 데스크톱 개발 도구와 Windows SDK를 준비한다. x64 Debug/Release, C++17을 선택한다. Toolchain.props는 기본 도구 집합을 우선하고 비어 있으면 설치된 v143/v145를 선택한다.
3. 사용자가 빌드한다. /utf-8, /MTd 또는 /MT, 실행 파일 옆 framework/dependencies의 x64 GLFW·VCRUNTIME140 DLL과 라이선스를 확인한다.
4. F5 실행 후 한글 도움말과 기본 Motion 장면을 확인한다.

## 모드별 예상 결과

- 0: 두 상자가 같은 경로·자전 시계를 공유한다. 1~6 경로, S/G 기여, M 반사, I 레이어 삽입을 확인한다.
- 7: 격자에 배치된 상자가 거리 경로를 따라 움직이고 번호 표시가 상자를 따라간다.
- 8: 방이 기울고 상자가 미끄러진다. F로 바닥을 열면 월드 낙하로 전환한다.
- 9: 사람은 좌석 위에 앉아 있다. 상체는 별도 모션을 가지며 줄·좌석·사람 전체가 hinge를 따라 움직인다. 지지대는 흔들리지 않는다. 노란 상자는 4초 편도, 8초 왕복 선형보간이다.
- 9 → Space: 시간을 정지한다. Tab/Backspace로 35개 선택 항목을 이동하며 콘솔 이름을 확인한다. rider hip/knee가 굽혀진 상태여야 한다.
- rider elbow 선택 → X → 방향키: 아래팔·손이 따라가고 몸통 전체는 움직이지 않아야 한다.
- swing hinge 선택 → 회전: 줄·좌석·사람 전체가 함께 움직여야 한다.
- torso 선택 → 이동·회전: 머리·팔에 전달되고 골반·다리는 따라가지 않아야 한다.
- PageUp/Down 뒤 회전: 피벗 변화에 따라 회전 중심이 달라져야 한다. +/-는 전체 자식에 균일 배율을 전달한다. R은 선택 관절, 9 재입력은 전체를 복원한다.
- Space로 재개: 편집한 rest가 다음 프레임에도 유지되어야 한다.
- V: 같은 자세를 세 뷰에서 그린다. 마우스 피킹·리사이즈·DPI·최소화 복귀·Esc/창 닫기를 확인한다.

## 실패와 경계

기간 0, 0축, 중복 관절 트랙, 비균일 rest 배율은 거부되어야 한다. poseHierarchy의 마지막 트랙이 잘못되어 앞 관절만 먼저 갱신되어서는 안 된다. Scene::clear 후 저장 ID를 재사용하지 않는다. jump의 false나 pick의 빈 결과는 정상 미일치일 수 있다.

## 독립 폴더

standard 전체를 다른 경로로 복사하고 상위 공용 framework 없이 열어 본다. 다른 PC 실행·컴파일·문서 표시 결과는 사용자가 확인한다. 외부 자산 사례의 models/robot.obj·textures/robot.png는 제공된 기본 실행에 필요하지 않으며 사용자가 준비한다. 용량 기준은 SIZE_REPORT.md에서 실제 폴더 합계를 확인한다.
'''
    write('standard/USER_CHECKS.md',checks)
    write('standard/framework/docs/USER_CHECKS.md',checks.replace('SIZE_REPORT.md','../../SIZE_REPORT.md').replace('standard.sln','../../standard.sln').replace('standard.slnx','../../standard.slnx'))
    readme='''# standard: Motion·격자·방·그네·관절 SRT 샘플

현재 기준: 2026-11-02. Windows x64, C++17, OpenGL 3.3 Core를 사용한다. 빌드·실행은 사용자가 수행한다.

## 문서 선택

- [모든 문서 목차](DOCUMENT_INDEX.md)
- [공통 구현과 전체 API 실제 사례](docs/API_REFERENCE.html)
- [응용패턴·그네·패턴별 흐름도·실제 코드](docs/IMPLEMENTATION_PATTERNS.html)
- [시험 현장 구현 순서](docs/EXAM_IMPLEMENTATION_ORDER.html)
- [직접 실행 확인](USER_CHECKS.md), [독립 배포](PORTABLE.md), [변경 전후](BEFORE_AFTER.md), [폴더 용량](SIZE_REPORT.md)

## 시작과 조작

standard.sln 또는 지원되는 VS의 standard.slnx를 연다. x64 Debug/Release를 선택한다. 프로젝트는 framework/Toolchain.props와 Framework.props를 포함하며 특정 v145를 강제하지 않는다. DLL·라이선스를 동봉한 폴더 구조를 유지한다.

'''+KEYS+'''
## 상태와 실행 흐름

main → Application/Console 생성 → 콜백 저장 → run의 초기화 → initScene.
프레임은 PollEvents → onUpdate(dt) → Scene.updateMotions(dt) → onRender → SwapBuffers다. onUpdate는 7/8/9 모드의 시계·물리·관절 상태를 갱신하고, 공통 MotionSystem은 Application이 자동 진행한다. 렌더 패스에서 시간을 다시 진행하지 않는다.

9번의 사람은 좌석에 붙어 앉은 자세다. 관절 편집값은 animation_의 rest에 저장되므로 재생 중에도 덮어써지지 않는다. 회전은 로컬 축, 이동은 부모 공간, 피벗은 모델 공간이다. 배율은 .1~3 범위의 균일 배율이다. 비균일 배율과 shear를 지원한다고 가정하지 않는다.

기본 실행은 외부 모델·텍스처를 요구하지 않는다. 문서의 별도 자산 예제는 사용자가 파일을 준비한다. 제3자 SDK·라이선스는 원본을 유지한다. 새로 수정한 소스·문서는 UTF-8 BOM이다.
'''
    write('standard/README.md',readme)
    write('standard/PORTABLE.md','''# 독립 배포와 환경

현재 기준: 2026-11-02. 새 ZIP을 만들거나 앱을 실행하지 않았다.

standard 폴더 전체가 프로젝트 단위다. standard.sln을 우선 사용하고 최신 VS에서는 .slnx도 사용한다. C++ 데스크톱 도구·Windows SDK·OpenGL 3.3 드라이버가 필요하다. Toolchain.props는 설치된 기본 도구 집합을 우선하고 필요 시 v143/v145를 선택한다. x64만 지원하며 C++17, /utf-8, /MTd 또는 /MT를 적용한다.

Framework.props는 include 경로와 런타임 배치를 정의한다. 실행 파일 옆 framework/dependencies 내부 경로의 GLFW DLL, VCRUNTIME140 DLL, 원본 라이선스를 유지한다. 외부 PC 절대경로·NuGet·vcpkg 설치 없이 동봉된 소스를 사용한다는 구성 의도이며, 다른 PC에서의 실제 빌드·실행 성공은 확인하지 않았다.

기본 모드 0/7/8/9와 관절 조작은 [README](README.md), 현장 구현 순서는 [시험 문서](docs/EXAM_IMPLEMENTATION_ORDER.html), 사용자 확인은 [USER_CHECKS](USER_CHECKS.md)를 따른다. 9번은 착석 사람·그네·개별 관절 SRT 편집을 포함한다. 실습 23 보류라는 과거 문구는 현재 상태가 아니다.

이 작업의 확인 기준은 압축 전 standard 폴더 전체 30 MB다. 과거 문서의 ZIP 3,000,000바이트 제한과 다른 기준이다. 실제 합계와 MB/MiB는 [SIZE_REPORT](SIZE_REPORT.md)를 따른다. 빌드 산출물·IDE 캐시가 나중에 생기면 폴더 크기도 바뀐다. 파일을 삭제하여 용량을 줄이는 작업은 하지 않았다.
''')
    write('standard/framework/README.md','''# 동봉 프레임워크 사용 안내

2026-11-02 기준 Windows x64 / C++17 / OpenGL 3.3 Core 배포본이다. 진입점 Framework.h는 Application·Patterns·Animation 공개 API를 포함한다.

1. [공통 구현·전체 API 사례](docs/API_REFERENCE.html): 콜백·렌더링 루프·행렬·CPU/GPU 업로드·자원 수명을 상세히 읽는다.
2. [응용패턴과 그네](docs/IMPLEMENTATION_PATTERNS.html): 흐름도·실제 응용 코드·현재 standard 원본을 함께 읽는다.
3. [공개 선언 색인](docs/API_KO.md), [착석·관절 SRT](docs/ANIMATION_API.md), [OBJ·텍스처](docs/OBJ_LOADING.md).
4. [시험 구현 순서](docs/EXAM_IMPLEMENTATION_ORDER.html), [사용자 확인](docs/USER_CHECKS.md).

Application이 Scene·Renderer와 창 수명을 소유한다. Scene의 ID와 비소유 포인터를 구분한다. Scene.clear 후 ID·rig·트랙을 다시 만든다. 새 관절 API의 모션 샘플은 강체이며 rest는 양수 균일 배율을 허용한다. 각 축의 비균일 배율은 자식 메시 크기 편집으로 분리한다.

텍스처 기능은 기본 꺼져 있다. OBJ만 지원하며 FBX/glTF·MTL 자동 로드·투영 그림자를 제공하지 않는다. 외부 의존성·라이선스는 원본을 유지한다. 소스·문서 작성과 실제 빌드·렌더링 확인은 구분하며 실행은 사용자에게 맡긴다.
''')
    b=json.loads(read('.obj-work/guides7/code-before.json'))
    before_after='# 현재 standard 변경 전후\n\n2026-11-02. 과거 최초 생성 기록 대신 현재 착석·관절 조작 변경을 보여준다. 빌드·테스트는 수행하지 않았다. 문서의 일반 구현·API 사례는 docs/API_REFERENCE.html, 응용 사례는 docs/IMPLEMENTATION_PATTERNS.html을 따른다.\n\n'
    for rel in ('standard/Console.h','standard/Console.cpp','standard/framework/include/Animation.h','standard/framework/detail/Animation.inl'):
        before_after+='## '+rel+'\n\n### before\n```cpp\n'+b[rel].rstrip()+'\n```\n\n### after\n```cpp\n'+read(rel).rstrip()+'\n```\n\n'
    write('standard/BEFORE_AFTER.md',before_after)
    exam=para('기존 시험 순서 문서는 저장소 docs/code-guide/exam.html에 존재한다. 이 문서는 그 순서를 현재 standard의 착석·관절 SRT 조작까지 연결해 독립 폴더 안에서 읽을 수 있도록 정리한 최신판이다. 시험의 실제 제한 시간과 허용 자료는 문제 지시를 따른다.')
    steps=[
      ('요구사항을 상태와 함수로 바꾸기','사람이 앉는다, 그네가 흔들린다, 관절을 선택한다, 세 뷰로 본다 같은 동사를 적는다. 각 동사를 좌석 부모 관계, hinge 모션, 선택 인덱스, RenderPass에 연결한다. 출력 조건과 입력 키를 먼저 표로 만든다.'),
      ('프로젝트·main 골격 준비','standard.sln, x64, Framework.h 포함을 확인한다. main은 Application과 응용 상태를 만들고 init/update/key/render 콜백을 저장한 뒤 run을 한 번 호출한다. 예제 main을 여러 개 프로젝트에 등록하지 않는다.'),
      ('정적 장면부터 조립','바닥·고정 프레임·좌석·앉은 사람을 먼저 만든다. 부모는 frame→hinge→seat→person.root이며 지지대는 frame의 자식이다. 관절 위치·바라보는 +Z 방향·착석 높이를 애니메이션 없이 확인할 준비를 한다.'),
      ('각 관절 기준 SRT·피벗 결정','HumanoidPose::seated로 엉덩이·무릎을 굽힌 뒤 createJointAnimation으로 rest를 캡처한다. basis는 부모 공간 위치, localBasis는 모델 피벗이다. 관절 선택 목록은 EntityId 또는 트랙 인덱스를 보관하고 포인터는 보관하지 않는다.'),
      ('입력 명령 연결','PRESS에서 모드·선택 관절·축을 바꾼다. 방향키·PageUp/Down·+/-는 선택 트랙의 rest만 수정한다. Space는 시간을 정지하고 R은 선택 rest를 복원한다. 애니메이션 함수와 입력이 같은 Scene 변환을 덮어쓰지 않게 한다.'),
      ('누적 시간과 모션 추가','AnimationClock.advance(dt)로 seconds를 누적하고 poseHierarchy(scene,animation,seconds)를 호출한다. dt를 누적 절대 시간 대신 넘기지 않는다. 선형보간은 편도 시간·재생 방식, 진자는 진폭·주기를 지정한다. Application이 자동 진행하는 MotionSystem을 따로 중복 update하지 않는다.'),
      ('충돌·추가 패턴은 필요한 경우만 결합','걷기·점프가 요구되면 CharacterBody 상태→stepCharacter→표시 관절 순서로 넣는다. 좌석에 앉은 사람에게 바닥 보행 상태를 그대로 적용하지 않는다. 벽 표시 개방과 실제 충돌 통과를 구분한다.'),
      ('렌더링·다중 뷰·피킹 연결','기본 renderScene 또는 사용자 renderViews를 택한다. update는 프레임당 한 번, 렌더는 여러 카메라로 반복한다. framebuffer의 크기·viewport 원점·DPI 좌표를 맞춘다.'),
      ('복원·오류·종료 처리','각 optional/bool 결과를 확인한다. 중복 관절 트랙·0 주기·비균일 rest 배율은 거부한다. 초기화에서 모든 ID·모드·시계를 다시 구성한다. 종료는 requestClose를 사용하고 GPU 해제를 컨텍스트 수명 안에서 처리한다.'),
      ('사용자가 최종 실행 확인','사용자가 빌드·실행한다. 그네를 멈추고 엉덩이·무릎 착석, 선택 팔꿈치의 자식 전달, hinge의 전체 전달, 지지대 고정, 피벗 이동 후 회전 중심, Space 재개 뒤 편집 유지, 리셋, 세 뷰 일치를 확인한다. 이 작성 작업에서는 실행하지 않았다.')]
    exam+=flow([t for t,_ in steps],'exam-order')
    for i,(title,description) in enumerate(steps,1):exam+=f'<h2>{i}. {esc(title)}</h2>'+para(description)
    exam+='<h2>착석·관절 구현 출발 코드</h2>'+code(GROUPS['animation']['code'])+'<h2>실제 main 연결</h2>'+code(read('standard/main.cpp'))
    exam+='<h2>증상으로 되돌아갈 위치</h2><table><tr><th>증상</th><th>되돌아갈 단계</th></tr><tr><td>사람이 좌석을 따라가지 않음</td><td>3: seat 부모 관계·월드/로컬 위치</td></tr><tr><td>조작한 관절이 다음 프레임 복귀</td><td>4~6: Scene 직접 변경 대신 track.rest 편집</td></tr><tr><td>관절의 중심이 이상함</td><td>4: basis·localBasis·회전 순서</td></tr><tr><td>뷰마다 자세가 다름</td><td>6·8: 렌더 안의 중복 시간 진행</td></tr><tr><td>상자 위치 변경마다 정점을 재생성</td><td>8: transform과 geometry revision 구분</td></tr></table>'
    for directory in ('standard/docs','standard/framework/docs','framework/docs'):
        write(directory+'/EXAM_IMPLEMENTATION_ORDER.html',page('시험 현장 구현 순서: 정적 조립에서 관절 제어까지',exam))
    index='# standard 문서 목차\n\n2026-11-02 현재 소스 기준. 저장소 밖 링크 없이 standard 폴더 안에서 읽는다. 의존성 라이선스와 예제 데이터는 문서 개편 대상에서 제외하고 원본을 유지한다.\n\n'
    index+='- README.md: 모드·키·실행 흐름\n- PORTABLE.md: 도구 집합·런타임·독립 폴더\n- USER_CHECKS.md: 사용자가 수행할 확인\n- BEFORE_AFTER.md: 최신 착석·관절 제어의 전체 전후 코드\n- SIZE_REPORT.md: 모든 파일의 실제 크기 합계\n\n'
    for p in sorted((ROOT/'standard').rglob('*')):
        if p.is_file() and p.suffix.lower() in ('.md','.html','.json') and not any(x in p.parts for x in ('dependencies','x64','.vs')):
            rel=p.relative_to(ROOT/'standard').as_posix();index+=f'- [{rel}]({rel})\n'
    write('standard/DOCUMENT_INDEX.md',index)

def refresh_size_report():
    # Re-read logical byte sizes after all authored documents are present, including this report.
    for _ in range(4):
        total=0; by={};count=0
        for d,dirs,names in os.walk(ROOT/'standard',followlinks=False):
            dirs[:]=[x for x in dirs if not (Path(d)/x).is_symlink()]
            for name in names:
                p=Path(d)/name
                if p.is_symlink():continue
                size=p.stat().st_size;total+=size;count+=1;k=p.relative_to(ROOT/'standard').parts[0];by[k]=by.get(k,0)+size
        report='# standard 전체 폴더 용량\n\n최종 문서 작성 후의 논리 파일 크기를 합산했다. SDK·DLL·문서·프로젝트·현재 존재하는 산출물을 포함하며 파일을 제외해 한도에 맞추지 않았다. 심볼릭 링크를 따라 다른 폴더를 중복 합산하지 않는다. 디스크 할당 크기·압축 ZIP 크기와 다르다.\n\n'
        report+=f'- 파일 수: {count:,}\n- 전체: **{total:,}바이트**\n- MB (1 MB=1,000,000바이트): **{total/1e6:.6f} MB**\n- MiB (1 MiB=1,048,576바이트): **{total/1048576:.6f} MiB**\n- 30 MB 초과 여부: **{"초과" if total>30000000 else "초과하지 않음"}**\n- 30 MiB 초과 여부: **{"초과" if total>31457280 else "초과하지 않음"}**\n\n'
        report+='| 최상위 항목 | 바이트 |\n|---|---:|\n'+''.join(f'| {k} | {v:,} |\n' for k,v in sorted(by.items(),key=lambda t:-t[1]))
        report+='\n빌드·앱 실행은 하지 않았다. 사용자가 나중에 빌드해 x64/.vs 등이 추가되면 합계가 바뀐다. 삭제로 회수한 용량은 0바이트다.\n'
        write('standard/SIZE_REPORT.md',report)
    print(f'Standard total: {total:,} bytes; {total/1e6:.6f} MB; 30 MB exceeded: {total>30000000}')

def graph_svg(ident, title, nodes, edges, width, height):
    """Author an offline diagram with explicit branch and return paths."""
    marker = ident + '-arrow'
    parts = [f'<figure><div class="flow"><svg viewBox="0 0 {width} {height}" role="img" aria-label="{esc(title)}"><title>{esc(title)}</title><defs><marker id="{marker}" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto"><path d="M0 0 L10 5 L0 10Z" fill="#4d688a"/></marker></defs>']
    for path, label, x, y in edges:
        parts.append(f'<path d="{path}" fill="none" stroke="#4d688a" stroke-width="2" marker-end="url(#{marker})"/>')
        if label:
            parts.append(f'<text x="{x}" y="{y}" font-size="16" fill="#263f5e">{esc(label)}</text>')
    for x, y, w, h, decision, lines in nodes:
        if decision:
            parts.append(f'<path d="M{x+w/2} {y} L{x+w} {y+h/2} L{x+w/2} {y+h} L{x} {y+h/2} Z" fill="#fff1cd" stroke="#9a7a31"/>')
        else:
            parts.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" rx="10" fill="#e8f0fb" stroke="#6a8ebc"/>')
        for i, line in enumerate(lines):
            text_y = y + h/2 + (i-(len(lines)-1)/2)*23 + 6
            parts.append(f'<text x="{x+w/2}" y="{text_y}" text-anchor="middle" font-size="17" fill="#1d2c40">{esc(line)}</text>')
    return ''.join(parts) + '</svg></div><figcaption>' + esc(title) + '</figcaption></figure>'

def loop_chart(ident):
    nodes = [
        (410,20,380,65,False,['main: 콜백 등록','등록 시 본문은 실행하지 않음']),
        (410,120,380,65,False,['run: 창·Renderer 초기화','initHandler → initScene 1회']),
        (410,230,380,90,True,['종료 요청 없이 계속 실행?']),
        (850,240,300,70,False,['shutdown → return','실패·예외도 자원 정리']),
        (410,360,380,65,False,['PollEvents → 입력 콜백','닫기 요청·콜백 오류 재확인']),
        (410,470,380,90,True,['framebuffer 크기가 양수?']),
        (30,480,270,70,False,['WaitEvents','previousTime 재설정']),
        (410,610,380,65,False,['dt = clamp(경과 초, 0, .05)','updateHandler → onUpdate']),
        (410,720,380,65,False,['Scene::updateMotions(dt)','현재 GL 컨텍스트 확인']),
        (410,830,380,90,True,['renderHandler 등록됨?']),
        (30,840,270,65,False,['사용자 renderHandler','예: Console::onRender']),
        (850,840,300,65,False,['기본 renderScene','한 Scene·기본 카메라']),
        (410,1000,380,65,False,['성공한 프레임: SwapBuffers','다음 반복으로 돌아감']),
    ]
    edges = [('M600 85 V120','',0,0),('M600 185 V230','',0,0),
             ('M790 275 H850','아니오',797,263),('M600 320 V360','예',612,344),
             ('M600 425 V470','',0,0),('M410 515 H300','아니오',320,501),
             ('M30 515 H15 V275 H410','다음 반복',24,260),
             ('M600 560 V610','예',612,587),('M600 675 V720','',0,0),
             ('M600 785 V830','',0,0),('M410 875 H300','예',350,859),
             ('M790 875 H850','아니오',797,859),
             ('M165 905 V960 H600 V1000','',0,0),('M1000 905 V960 H600','',0,0),
             ('M600 1065 V1100 H350 V210 H600 V230','반복',363,1090)]
    return graph_svg(ident, '프레임 루프: 조건 분기와 반복 복귀', nodes, edges, 1200, 1130)

def hierarchy_chart(ident):
    nodes = [(60,20,270,65,False,['엔티티 루트 L0']),
             (60,150,270,65,False,['조상 L1: prefix에 포함']),
             (360,150,250,65,False,['다른 자식','대상 체인에는 미포함']),
             (60,280,270,65,False,['연결 엔티티 La: anchor']),
             (60,410,270,65,False,['자손 Lt: suffix에 포함']),
             (60,540,270,65,False,['대상 면: faceChild × F']),
             (710,20,320,65,False,['모션 rootLayer']),
             (710,150,320,65,False,['상위 모션 레이어']),
             (710,280,320,65,False,['연결할 말단 LayerId']),
             (690,410,360,65,False,['layerMatrices 결과','motion.parent / motion.local']),
             (390,540,660,65,False,['entityWorld = parent × prefix × local × suffix','Mface = entityWorld × faceChild × F'])]
    edges = [('M195 85 V150','부모 → 자식',205,125),
             ('M195 85 V115 H485 V150','분기',385,106),
             ('M195 215 V280','',0,0),('M195 345 V410','',0,0),
             ('M195 475 V540','면 변환',205,513),
             ('M870 85 V150','레이어 부모',880,125),('M870 215 V280','',0,0),
             ('M710 312 H330','bindEntity: 부모 관계와 별개',365,298),
             ('M870 345 V410','World / Local 합성',878,381),
             ('M870 475 V540','',0,0)]
    return graph_svg(ident, '엔티티 계층과 모션 레이어: 연결 위치와 행렬 전달', nodes, edges, 1100, 640)

def swing_chart(ident):
    nodes = [(410,20,300,65,False,['frame: 고정 기준']),
             (40,150,270,65,False,['고정 지지대','hinge의 형제가 됨']),
             (410,150,300,65,False,['hinge: 진자 회전']),
             (40,280,270,65,False,['줄: hinge를 따라감']),
             (410,280,300,65,False,['seat: hinge를 따라감']),
             (410,410,300,65,False,['person.root: 좌석의 자식']),
             (410,540,300,65,False,['pelvis: 골반 기준']),
             (100,680,330,65,False,['torso → 목·머리·어깨·팔','상체 편집은 이 하위로 전달']),
             (680,680,330,65,False,['양쪽 hips → 무릎·발','하체는 골반 아래에서 분기'])]
    edges = [('M560 85 V115 H175 V150','',0,0),('M560 85 V150','',0,0),
             ('M560 215 V245 H175 V280','',0,0),('M560 215 V280','',0,0),
             ('M560 345 V410','부모 변환 전달',573,385),('M560 475 V540','',0,0),
             ('M560 605 V640 H265 V680','',0,0),('M560 605 V640 H845 V680','',0,0)]
    return graph_svg(ident, '그네·착석 사람: 부모에서 자식으로 전달되는 변환', nodes, edges, 1100, 790)

def cube_diagrams():
    return {
        'cube-exam': flow(['CPU: 입력·시간·Scene 상태 결정','GPU 버퍼·프로그램·uniform 바인딩','정점: M → V → P → 클립 좌표','클리핑 → 원근 나눗셈 → viewport','래스터화 → 프래그먼트 → 깊이/블렌딩','framebuffer → SwapBuffers'], 'cube-pipeline-chart'),
        'cube-m': flow(['모델 정점 v','T(-localBasis): 피벗을 원점으로','S: 배율 → R: 회전','T(basis): 부모 공간에 배치','부모·모션·면 합성 → 최종 M'], 'cube-m-chart'),
        'cube-v': flow(['월드 좌표 M × v','position / target / up','viewMatrix → lookAt → V','뷰 좌표 V × M × v'], 'cube-v-chart'),
        'cube-p': flow(['카메라 투영 값·viewport aspect','원근 perspective / 직교 ortho','P × viewPosition → clip','클리핑 후 w로 나눠 NDC'], 'cube-p-chart'),
        'cube-binding': flow(['초기화: VAO → VBO/EBO → attribute 설정','형상 변경: CPU 메시 → GPU 버퍼 업로드','draw: UseProgram → BindVertexArray','면별 MVP·법선·텍스처 uniform','DrawElements: EBO 구간의 인덱스 사용'], 'cube-bind-chart'),
        'cube-hierarchy': hierarchy_chart('cube-hierarchy-chart'),
        'cube-face': flow(['상자 entityWorld 계산','independence이면 faceChild 합성; 아니면 I','localAnimationMatrix: T(c) × R × T(-c)','Mface = entityWorld × faceChild × localAnimation','P × V × Mface × 정점 → 한 면만 열기'], 'cube-face-chart'),
        'cube-after': flow(['gl_Position: 클립 좌표','프리미티브 조립·클리핑','원근 나눗셈: clip.xyz / clip.w','NDC → viewport → 화면 좌표','래스터화·속성 보간 → 프래그먼트','깊이·블렌딩 → framebuffer'], 'cube-after-chart'),
        'cube-order': flow(['요구를 M/V/P·부모·면·바인딩으로 분류','정지한 육면체 한 개 생성','M·피벗 → 부모 계층 → 면 변환','입력 상태 → 프레임당 update 한 번','V/P·viewport → 렌더링 연결','사용자가 빌드·실행·리셋 확인'], 'cube-order-chart'),
    }

def refresh_doc_flowcharts():
    targets = {'runtime-loop': loop_chart, 'case-app': loop_chart,
               'runtime-matrix': swing_chart, 'case-animation': swing_chart,
               'case-transform': hierarchy_chart, 'case-layers': hierarchy_chart,
               'pattern-parts': hierarchy_chart, 'pattern-swing': swing_chart}
    assets = {'frame-loop': loop_chart('standalone-loop'),
              'entity-layers': hierarchy_chart('standalone-hierarchy'),
              'swing-hierarchy': swing_chart('standalone-swing'),
              'render-pipeline': cube_diagrams()['cube-exam']}
    markdown = {'API_KO.md': ['frame-loop', 'entity-layers', 'render-pipeline'],
                'APPLICATION_PATTERNS.md': ['frame-loop', 'entity-layers', 'swing-hierarchy'],
                'ANIMATION_API.md': ['frame-loop', 'swing-hierarchy']}
    labels = {'frame-loop': '프레임 루프·조건 분기', 'entity-layers': '엔티티 계층·모션 레이어',
              'swing-hierarchy': '그네·착석 사람 계층', 'render-pipeline': '렌더링 파이프라인'}
    for directory in ('framework/docs', 'standard/docs', 'standard/framework/docs'):
        for name, diagram in assets.items():
            svg = re.search(r'<svg\b[\s\S]*?</svg>', diagram).group(0)
            svg = svg.replace('<svg ', '<svg xmlns="http://www.w3.org/2000/svg" font-family="Malgun Gothic, sans-serif" ', 1)
            write(directory + '/flowcharts/' + name + '.svg', svg + '\n')
        for name, images in markdown.items():
            begin, end = '<!-- doc-flowcharts:start -->', '<!-- doc-flowcharts:end -->'
            addition = begin + '\n\n' + '\n\n'.join(
                '![이미지: ' + labels[key] + '](flowcharts/' + key + '.svg)' for key in images)
            addition += '\n\n' + end + '\n\n'
            path = directory + '/' + name
            current = re.sub(re.escape(begin) + r'[\s\S]*?' + re.escape(end) + r'\s*', '', read(path))
            current = current.replace('\n\n', '\n\n' + addition, 1)
            write(path, current)
        for name in ('API_REFERENCE.html', 'IMPLEMENTATION_PATTERNS.html'):
            path = directory + '/' + name
            current = read(path)
            for section, author in targets.items():
                begin, end = '<!-- chart-' + section + ':start -->', '<!-- chart-' + section + ':end -->'
                current = re.sub(re.escape(begin) + r'[\s\S]*?' + re.escape(end), '', current)
                pattern = r'(<section id="' + section + r'">\s*<h2>[\s\S]*?</h2>)'
                addition = begin + author('doc-' + section) + end
                current = re.sub(pattern, lambda m: m.group(0) + addition, current, count=1)
            write(path, current)
    index = read('standard/DOCUMENT_INDEX.md')
    begin, end = '<!-- flowchart-index:start -->', '<!-- flowchart-index:end -->'
    index = re.sub(re.escape(begin) + r'[\s\S]*?' + re.escape(end) + r'\s*', '', index)
    addition = begin + '\n\n' + '\n'.join(
        '- [' + labels[key] + '](docs/flowcharts/' + key + '.svg)' for key in assets)
    index = index.replace('\n\n', '\n\n' + addition + '\n\n' + end + '\n\n', 1)
    write('standard/DOCUMENT_INDEX.md', index)

def refresh_cube_docs():
    """Add the approved cube material without refreshing unrelated source snapshots."""
    body = read('tools/templates/cube_pipeline_body.html')
    for section, diagram in cube_diagrams().items():
        pattern = r'(<section id="' + section + r'">\s*<h2>[\s\S]*?</h2>)'
        body = re.sub(pattern, lambda m: m.group(0) + diagram, body, count=1)
    loop = '<section id="cube-loop"><h2>프레임 반복과 콜백 실행 시점</h2>' + loop_chart('cube-loop-chart')
    loop += '<p>화살표가 조건 검사로 돌아가면 다음 프레임이다. 최소화 상태는 WaitEvents 뒤 시간 기준을 재설정한다. 입력·update 뒤의 종료 요청은 반복을 중단한다. 모션·컨텍스트·렌더·swap 실패와 콜백 예외도 shutdown 경로로 간다. update는 프레임당 한 번이며 여러 카메라 렌더 패스에서 반복하지 않는다.</p></section>'
    body += loop
    begin, end = '<!-- cube-exam:start -->', '<!-- cube-exam:end -->'
    sections = {m.group(1): m.group(0) for m in re.finditer(
        r'<section id="(cube-[^"]+)">[\s\S]*?</section>', body)}
    selection = {
        'API_REFERENCE.html': list(sections),
        'IMPLEMENTATION_PATTERNS.html': ['cube-exam', 'cube-loop', 'cube-hierarchy', 'cube-face'],
        'EXAM_IMPLEMENTATION_ORDER.html': ['cube-exam', 'cube-loop', 'cube-order'],
    }
    for directory in ('framework/docs', 'standard/docs', 'standard/framework/docs'):
        for name, ids in selection.items():
            addition = ''.join(sections[key] for key in ids)
            if name != 'API_REFERENCE.html':
                addition = re.sub(
                    r'href="#(cube-[^"]+)"',
                    lambda m: m.group(0) if m.group(1) in ids else
                    'href="API_REFERENCE.html#' + m.group(1) + '"', addition)
                addition += '<p><a href="API_REFERENCE.html#cube-binding">바인딩·M/V/P·전체 변환 설명</a></p>'
            if name == 'EXAM_IMPLEMENTATION_ORDER.html':
                addition += '<h2>추가 응용: 기존 그네·사람형 구현 순서</h2><p>아래 착석·관절 예제는 추가 응용 자료다. 육면체 코딩테스트의 확정 요구사항이 아니다.</p>'
            addition = begin + addition + end
            path = directory + '/' + name
            current = read(path)
            current = re.sub(re.escape(begin) + r'[\s\S]*?' + re.escape(end), '', current)
            # Insert before the previous material so the cube is the exam baseline.
            current = current.replace('</nav>', '</nav>' + addition, 1)
            if name == 'EXAM_IMPLEMENTATION_ORDER.html':
                current = current.replace('시험 현장 구현 순서: 정적 조립에서 관절 제어까지',
                                          '시험 현장 구현 순서: 육면체·렌더링·계층 변환')
            write(path, current)

if __name__=='__main__':
    import argparse
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cube-only', action='store_true',
                        help='Update only cube sections; retain existing guide snapshots.')
    args = parser.parse_args()
    if not args.cube_only:
        main()
        refresh_standard()
    refresh_cube_docs()
    refresh_doc_flowcharts()
    refresh_size_report()
