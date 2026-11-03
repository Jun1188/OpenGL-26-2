"""시험 요구사항 → 호출 순서 → 실습 예시 자료를 생성한다. 실행 검증은 하지 않는다."""
from pathlib import Path
import importlib.util
import json
ROOT=Path(__file__).resolve().parent.parent
spec=importlib.util.spec_from_file_location('guide',ROOT/'tools/Write-CodeGuide.py')
g=importlib.util.module_from_spec(spec);spec.loader.exec_module(g)
RECIPES=[
('input','키 입력·토글·초기화','한 번 누르는 명령과 누르는 동안의 입력을 구분하고, reset이 복구할 상태 목록을 먼저 적는다.',
 ['PRESS/REPEAT/RELEASE 구분','입력 의도를 상태에 저장','update에서 dt 적용','reset에서 상태와 ID 복원'],
 '''// onKey의 토글 예:
if(action==GLFW_PRESS && key==GLFW_KEY_SPACE) playing_=!playing_;
// 지속 이동은 PRESS/RELEASE로 누름 상태를 저장한다.
if(key==GLFW_KEY_LEFT && action!=GLFW_REPEAT)
    leftHeld_=(action==GLFW_PRESS);
// onUpdate에서만 시간 반영:
if(leftHeld_) position_.x-=speed_*dt;''',
 '예제의 멤버 변수는 응용이 선언해야 한다. REPEAT마다 토글하면 한 번 길게 눌렀을 때 상태가 계속 뒤집힌다. Scene.clear 후 예전 ID를 재사용하지 않는다.',
 ['20/pipeline.html','21/pipeline.html']),
('boot','창·기본 루프','모든 문제의 출발점이다. standard의 main과 콜백 연결을 유지하고 데모의 상태와 입력을 필요한 만큼 줄인다.',
 ['Application 생성','Console 수명 유지','init/key/update 등록','run(config)'],
 '''graphics::Application app;
Console console(app); // run 종료까지 유지
app.setInitHandler([&](graphics::Application&) { return console.initScene(); });
app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
    console.onKey(key, action, mods);
});
app.setUpdateHandler([&](graphics::Application&, float dt) { console.onUpdate(dt); });
return app.run();''',
 '람다 등록은 실행이 아니다. run이 창/컨텍스트를 준비한 뒤 init을 호출한다. 기본 렌더를 쓰면 render 콜백을 추가할 필요가 없다.',
 ['standard/pipeline.html','framework/pipeline.html']),
('scene','비율 배치·정적 형상','먼저 대표 객체 한 개의 위치와 방향을 맞춘다. 비율은 GridLayout, 표시 ID와 부모 관계는 Scene이 담당한다.',
 ['GridLayout.valid','position / relativeSize','결과 검사','PatternSystem.addBox'],
 '''graphics::GridLayout grid;
grid.origin={-5,0,-5}; grid.extent={10,4,10}; grid.cells={5,1,5};
const auto p=grid.position({.5f,.25f,.5f}); // 월드 (0,1,0)
const auto size=grid.relativeSize({.1f,.25f,.1f}); // 로컬 크기 (1,1,1)
if(!p || !size) return false;
const auto box=graphics::PatternSystem::addBox(app_.getScene(),*size,*p,{1,0,0});
if(!box) return false;''',
 'frame이 항등이고 부모가 없는 예다. 부모가 있으면 positionInParent(ratio,parentWorld)를 쓴다. relativeSize는 frame을 적용하지 않는다. cellCenter는 정수 셀 인덱스를 받는다.',
 ['standard/notes.html','23/ownership.html']),
('hierarchy','관절·자전·공전','부품 회전은 관절 그룹의 피벗 위치를 정한 뒤 추가한다. 메시 크기를 관절 변환에 섞지 않는다.',
 ['Scene.addGroup 루트','addJoint(anchor)','addBox(parent=joint)','rotateJoint'],
 '''auto& scene=app_.getScene();
const auto root=scene.addGroup(); if(!root) return false;
const auto joint=graphics::PatternSystem::addJoint(scene,*root,{0,1,0});
if(!joint) return false;
const auto arm=graphics::PatternSystem::addBox(scene,{.2f,1,.2f},{0,-.5f,0},{1,1,0},*joint);
if(!arm) return false;
if(!graphics::PatternSystem::rotateJoint(scene,*joint,{0,0,1},30)) return false;''',
 'anchor와 자식 위치는 부모 로컬 좌표다. 임의 축의 공전 행렬은 TransformSystem::rotation(AxisLine,degrees), 자전/공전 모션은 createSpin/createOrbit로 공간을 구분한다.',
 ['22/ownership.html','23/ownership.html']),
('motion','일정 간격·코너·일시 정지','시간은 하나만 진행하고 객체마다 거리 offset을 더한다. 순간 방향 전환 대신 경로의 위치와 접선/yaw를 함께 계산한다.',
 ['AnimationClock.advance(dt) 한 번','distance=speed*seconds+offset','conveyorCornerPose','Scene.setTransform'],
 '''// clock_, boxId_는 init에서 준비한 상태다.
if(!clock_.advance(dt)) return; // 실패 처리 정책은 호출부에서 결정
const auto pose=graphics::PatternSystem::conveyorCornerPose(
    1.25*clock_.seconds+offset,{ -4,1,-2 },{ 2.2f,1,-1.2f },.8f,3.8f);
if(!pose) return;
graphics::Transform3D transform;
transform.basis=pose->position;
transform.rotation=glm::mat3(glm::rotate(glm::mat4(1),glm::radians(pose->yawDegrees),glm::vec3(0,1,0)));
if(!app_.getScene().setTransform(boxId_,transform)) return;''',
 '고정된 이 코너 API는 +X 직선→안쪽 피벗의 90도 원호→+Z 직선이다. 다른 경로는 DistancePath.setPoints/sample 또는 PathSystem+Motion을 선택한다. Application 모션 갱신을 직접 중복 호출하지 않는다.',
 ['20/notes.html','framework/ownership.html']),
('collision','충돌·점프·보행','발 위치·반폭·높이·수직 속도·접지를 먼저 정한다. 충돌 해결 전 후보 위치를 Scene에 먼저 쓰지 않는다.',
 ['CharacterBody 초기값','randomFreeBox로 비겹침 배치','PRESS에서 jump','update에서 stepCharacter','poseWalkingRig'],
 '''// body_, bounds_, obstacles_, rig_는 init에서 준비한다.
// 키 PRESS 분기:
graphics::CollisionSystem::jump(body_,5.5f); // 공중이면 정상적으로 false
// 프레임 갱신 분기:
const auto step=graphics::CollisionSystem::stepCharacter(body_,bounds_,obstacles_,dt);
if(!step) return;
const float yaw=glm::degrees(std::atan2(body_.direction.x,body_.direction.z));
if(!graphics::PatternSystem::poseWalkingRig(app_.getScene(),rig_,body_.feet,yaw,
                                          body_.walkDistance,body_.speed)) return;''',
 '두 분기는 같은 곳에 연달아 넣지 않는다. 점프 명령은 입력 콜백, 적분과 표시 갱신은 update에 둔다. random_ 엔진은 Console 멤버로 유지하며 reset마다 같은 시드를 다시 주지 않는다.',
 ['23/pipeline.html','23/notes.html']),
('room','기울어진 방·바닥 개방·낙하','방 내부 계산과 이탈 후 계산을 좌표계로 나눈다. 방이 회전할 때도 중력은 월드 아래 방향이다.',
 ['RoomBody 로컬 상태','roomWorld 계산','e에서 바닥 숨김 + 모든 물체 좌표 전환','stepRoomBody','position(roomWorld)로 표시'],
 '''// e의 PRESS 분기에서 각 미이탈 물체에 수행:
body.worldPosition=glm::vec3(roomWorld*glm::vec4(body.localPosition,1));
body.worldVelocity=glm::vec3(roomWorld*glm::vec4(body.localVelocity,0));
body.released=true;
// 프레임 갱신:
if(!graphics::CollisionSystem::stepRoomBody(body,localBounds,roomWorld,dt,
                                          floorOpen,elastic)) return;
const glm::vec3 worldPosition=body.position(roomWorld);''',
 '이 코드는 좌표 전환의 핵심이다. 실습21의 Console::releaseBody는 이탈 당시 방의 회전 자세도 저장한다. 강제 이탈에서는 방의 각속도에 따른 추가 접선 속도를 넣지 않는다. releaseBody는 공통 공개 API가 아니라 실습21 보조 함수다. 표시만 바닥을 숨기면 내부 물체가 자동 낙하하지 않는다.',
 ['21/pipeline.html','21/notes.html']),
('views','다중 카메라·뷰포트','한 Scene과 한 시간 갱신을 유지한 뒤 렌더만 나눈다. 실제 프레임버퍼 픽셀 크기를 사용한다.',
 ['getFramebufferSize','threeViews(camera,width,height,options)','Application.renderViews'],
 '''const auto size=app_.getFramebufferSize();
if(size[0]<=0 || size[1]<=0) return true; // 최소화
const auto passes=graphics::PatternSystem::threeViews(
    app_.getCamera(),size[0],size[1],app_.getRenderOptions());
if(passes.size()!=3) return false;
return app_.renderViews(passes);''',
 '사용자 render 콜백에서 실행할 예다. RenderPass viewport는 왼쪽 아래 기준이며 Picking viewport는 왼쪽 위 기준이다. 다중뷰 피킹은 클릭한 pass의 카메라와 영역을 사용한다.',
 ['22/pipeline.html','22/notes.html']),
('lighting','앞면 제거·벽 열기·표면 명암','면 제거, 벽 가시성, 벽 높이 변형, 조명 법선을 서로 구분한다.',
 ['addRoom(frontOpen=false)','Front cull','첫 O에서 cameraFacingWall','activeWall 저장','다음 O는 같은 벽 목표 반전','openRoomWall(progress)'],
 '''// 초기화: 네 옆벽을 처음부터 모두 만든다.
const auto room=graphics::PatternSystem::addRoom(scene,bounds,false);
if(!room) return false;
// 처음 O에서만 선택하고 optional< size_t > activeWall_에 저장한다.
activeWall_=graphics::PatternSystem::cameraFacingWall(scene,*room,camera);
// 이후 O는 새 벽을 찾지 않고 저장한 벽의 열기/복원 목표만 반전한다.
// update에서 progress를 [0,1]로 이동시킨 뒤:
if(activeWall_ && !graphics::PatternSystem::openRoomWall(scene,*room,*activeWall_,progress))
    return false;''',
 '위 코드는 역할별 발췌다. 전체 상태 전이는 23의 onKey/onUpdate를 본다. f=normalize(target-position), 바깥 법선 n과 dot(n,f)가 가장 큰 양수인 맞은편 벽을 선택한다. 초기 카메라에서는 -Z 벽이다. 완전 복원 후에만 activeWall을 해제한다. 모든 실습의 현재 lighting은 표면 명암이며 투영 그림자는 없다.',
 ['23/notes.html','21/notes.html']),
('finish','최초 오류에서 역추적','증상만 보고 여러 기능을 동시에 고치지 않는다. 첫 진단의 단계·API·파일·줄·호출 경로를 따라 입력을 확인한다.',
 ['DiagnosticOptions.trace=true','최초 오류 저장','Source 위치의 호출 인자','선행 반환 실패','수정 후 사용자 확인'],
 '''graphics::ApplicationConfig config;
config.diagnostics.errors=true;
config.diagnostics.trace=true;
// 사용자 정의 bool API도 위치와 조치를 기록할 수 있다.
if(!condition) return graphics::Diagnostics::failAt(__FILE__,__LINE__,
    graphics::PipelineStage::Transform,"사용자변환","크기 입력이 유효하지 않습니다.",
    "크기 세 축이 유한한 양수인지 확인하세요.");''',
 '드라이버/표준 라이브러리의 예외 원문은 원래 언어로 남을 수 있다. catch 위치는 결함 발생 위치와 같다고 단정하지 않는다. 현재 자료 생성 과정에서 빌드·테스트·앱 실행은 하지 않았다.',
 ['framework/notes.html','framework/pipeline.html']),
]
def main():
    order_keys=['boot','scene','hierarchy','input','motion','collision','room','views','lighting','finish']
    recipes=sorted(RECIPES,key=lambda r:order_keys.index(r[0]))
    body=g.para('시험 문제의 동사를 행동으로 바꾼 뒤 이 표에서 호출 순서를 고른다. 예제는 현재 공개 시그니처에 맞춘 발췌이며 독립된 완성 프로그램이 아니다. 먼저 <a href="exam.html">공통 행동 그래프</a>를 읽는다. 모든 함수는 <a href="api-reference.html">전체 API 검색</a>과 <a href="../../framework/docs/API_KO.md">API_KO.md</a>에서 찾는다.')
    body+=g.section('요구사항 → 행동 → API',g.table(['요구','행동 카드','호출 순서'],[[title,'<a href="#'+key+'">'+title+'</a>',g.esc(' → '.join(order))] for key,title,intro,order,snippet,caution,examples in recipes]))
    for key,title,intro,order,snippet,caution,examples in recipes:
        body+='<section id="'+key+'"><h2>'+title+'</h2>'+g.para(intro)+g.flow(order)+g.code(snippet)+'<aside>'+caution+'</aside>'+g.para('적용 예시: '+' · '.join('<a href="'+p+'">'+g.esc(p.split('/')[0])+'의 '+g.PARTS.get(Path(p).stem,Path(p).stem)+'</a>' for p in examples))+'</section>'
    body+=g.section('시험에서 섞을 때의 판단',g.table(['혼합 요구','공유할 상태','추가할 계산','추가하면 안 되는 중복'],[
      ['점프 로봇 + 세 시점','CharacterBody 하나와 Scene 하나','렌더 pass만 세 개','pass마다 중력·보행 시간 갱신'],
      ['회전 방 + 컨베이어','방 로컬 경로와 공통 거리','방 월드 행렬 합성, 이탈 시 좌표 전환','방 자식에 월드 위치를 다시 입력'],
      ['벽 개방 + 랜덤 장애물','단일 activeWall과 유지되는 난수 엔진','개방 진행률, 비겹침 배치','O마다 다른 벽 선택, reset마다 같은 시드'],
      ['자전 부품 + 공전 이동','루트/관절/표시 계층','각 책임의 축과 피벗','메시 배율을 관절 위치에 중복 적용']]))
    g.page(g.OUT/'api.html','코딩테스트 · 요구사항별 API 호출 순서',body)
    records=json.loads((ROOT/'framework/docs/API_KO.json').read_text(encoding='utf-8-sig'))
    body=g.para('공개 함수·메서드의 모든 선언을 포함한다. 오버로드·const 구분·복사 금지 선언은 개별 항목이다. 클래스 외부 멤버 정의는 클래스 내부 선언과 중복 수록하지 않는다. 비공개/detail/SDK는 제외한다. <a href="api.html">먼저 호출 순서 보기</a> · <a href="../../framework/docs/API_KO.md">전체 Markdown</a>')
    body+='<input type="search" data-search placeholder="함수·타입·한글 기능 검색" aria-label="API 검색">'
    for i,r in enumerate(records):
        full=(r['owner']+'::' if r['owner'] else '')+r['name']
        body+='<details data-symbol="'+g.esc(full+' '+r['description'])+'"><summary>'+g.esc(full)+'</summary>'+g.code(r['signature']+';')+g.para(g.esc(r['description']))+g.para('반환·수명: '+g.esc(r['result']))+g.para(g.source_link(r['file'],r['line'],sub=False))+'</details>'
    g.page(g.OUT/'api-reference.html','프레임워크 전체 공개 API · 한글 검색',body)
    print('시험 API 호출 안내·전체 API 검색 HTML 생성')
if __name__=='__main__':main()
