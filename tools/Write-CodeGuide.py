"""Author source-backed Korean HTML lessons from the six graphify extractions. No builds/tests/browser execution."""
from pathlib import Path
import html
import json
import os
import re
import subprocess
from urllib.parse import quote

ROOT=Path(__file__).resolve().parent.parent
OUT=ROOT/'docs/code-guide'
SCOPES=['framework','standard','20','21','22','23']
TITLES={'framework':'공통 프레임워크','standard':'standard','20':'실습 20 · 컨베이어','21':'실습 21 · 회전 공간','22':'실습 22 · 세 뷰포트','23':'실습 23 · 로봇'}
PARTS={'pipeline':'1. 렌더링·호출 흐름','ownership':'2. 추상화·소유 구조','symbols':'3. 객체·함수 설명','notes':'4. 계산·명암·주의점'}
CSS='''body{margin:0;background:#f4f5f7;color:#202934;font:16px/1.8 system-ui,"Malgun Gothic",sans-serif}main{max-width:1120px;margin:auto;padding:32px}nav{padding:12px;background:#fff;border:1px solid #ccd4df;position:sticky;top:0;z-index:2}a{color:#1454a0;text-underline-offset:3px}h1,h2,h3{line-height:1.4}h2{margin-top:48px;border-bottom:2px solid #a7bdd4;padding-bottom:8px}h3{margin-top:28px}pre{white-space:pre-wrap;overflow-wrap:anywhere;background:#152333;color:#e8f2fc;padding:18px;font:14px/1.7 Consolas,monospace;border-radius:8px}code{font-family:Consolas,monospace}table{border-collapse:collapse;width:100%;background:white;margin:20px 0}td,th{border:1px solid #ccd4df;padding:10px;text-align:left;vertical-align:top}th{background:#e6edf5}aside,.box{background:white;border-left:4px solid #3377ac;padding:18px;margin:20px 0}.warn{border-color:#b46a13;background:#fff5e5}details{background:white;border:1px solid #d4dce6;margin:14px 0;padding:14px}summary{cursor:pointer;font-weight:650}.flow{display:flex;flex-wrap:wrap;gap:10px;align-items:center}.node{display:inline-block;border:1px solid #829ebc;border-radius:7px;background:white;padding:12px}.line{display:block;scroll-margin-top:100px;line-height:1.6}.line:target{background:#485b21}.small{font-size:14px;color:#516171}input[type=search]{padding:12px;width:90%;font:inherit}.cards{display:grid;grid-template-columns:repeat(auto-fit,minmax(260px,1fr));gap:15px}svg{max-width:100%;height:auto}label{display:inline-block;margin:8px 15px 8px 0}footer{margin-top:48px;border-top:1px solid #ccd4df;padding:20px}.active{background:#dbeedb;border-color:#438243}.tag{font-size:13px;background:#e6edf5;padding:3px 8px}'''
JS='''document.querySelectorAll('input[data-search]').forEach(x=>x.addEventListener('input',()=>{let q=x.value.toLowerCase();document.querySelectorAll('[data-symbol]').forEach(d=>d.hidden=!d.dataset.symbol.toLowerCase().includes(q));}));function reveal(){const e=document.getElementById(decodeURIComponent(location.hash.slice(1)));if(e){let d=e.closest('details');if(d)d.open=true;e.scrollIntoView();}}addEventListener('hashchange',reveal);if(location.hash)reveal();'''
def esc(s):return html.escape(str(s))
def write(p,s):p.parent.mkdir(parents=True,exist_ok=True);p.write_text(s,encoding='utf-8-sig',newline='\n')
def code(s):return '<pre><code>'+esc(s)+'</code></pre>'
def table(head,rows):return '<table><thead><tr>'+''.join('<th>'+esc(x)+'</th>' for x in head)+'</tr></thead><tbody>'+''.join('<tr>'+''.join('<td>'+str(x)+'</td>' for x in row)+'</tr>' for row in rows)+'</tbody></table>'
def flow(items):return '<div class="flow">'+'<b>→</b>'.join('<span class="node">'+esc(x)+'</span>' for x in items)+'</div>'
def para(s):return '<p>'+s+'</p>'
def section(title,s):return '<h2>'+title+'</h2>'+s
def source_id(file,line):return re.sub(r'[^a-zA-Z0-9]','_',file)+'_L'+str(line)
def source_link(file,line,label=None,sub=True):return '<a href="'+('../' if sub else '')+'sources.html#'+source_id(file,line)+'">'+esc(label or f'{file}:{line}')+'</a>'
def locate(file,symbol):
    text=(ROOT/file).read_text(encoding='utf-8-sig');pos=text.find(symbol)
    return text[:max(0,pos)].count('\n')+1
def ref(file,symbol,label=None):return source_link(file,locate(file,symbol),label or symbol)
def page(path,title,body,scope=None,extra=''):
    up='../' if scope else ''
    nav='<a href="'+up+'index.html">목차</a> · <a href="'+up+'exam.html">시험 행동 그래프</a>'
    if scope:nav+=' · '+' · '.join('<a href="'+p+'.html">'+v+'</a>' for p,v in PARTS.items())+' · <a href="graphify-out/graph.html">graphify 그래프</a>'
    footer='소스와 graphify AST/정의 앵커를 대조한 학습 자료다. 실행 순서 설명은 실제 콜백 코드로 보완했다. 그래프의 동명 심볼·함수 포인터 간선은 확정 호출로 해석하지 않는다. 빌드·실행·테스트·검증기·브라우저 시각 확인은 수행하지 않았고 사용자에게 맡긴다. ZIP을 만들지 않았다.'
    write(path,'<!doctype html><html lang="ko"><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1"><title>'+esc(title)+'</title><style>'+CSS+'</style></head><body><main><nav>'+nav+'</nav><h1>'+esc(title)+'</h1>'+body+'<footer>'+footer+'</footer></main><script>'+JS+extra+'</script></body></html>')

OBJECTS={
'Application':'Scene·Camera·Renderer·RenderOptions·AssetLoader와 콜백을 값/optional로 보관한다. GLFW 창과 초기화·종료를 책임진다. main의 Console을 소유하지 않는다.',
'Scene':'CPU의 모델 컴포넌트·ID 목록·선·모션 시스템을 보관한다. 부모는 ID 관계다. Scene이 GPU 버퍼를 직접 소유하지 않는다.',
'Renderer':'GL 프로그램·VAO/VBO/EBO·GPU 텍스처와 draw range 캐시를 소유한다. GPU 삭제는 유효한 컨텍스트에서 해야 한다.',
'Camera':'position은 눈 위치, target은 주목점이다. target 자체가 시선 벡터는 아니다. up·투영·클리핑 값을 갖는 데이터다.',
'Transform3D':'basis 위치, localBasis 피벗, rotation 회전 기저, scale 배율을 분리한다. T(basis) R S T(-localBasis)로 변환한다.',
'MotionSystem':'MotionId별 시간·샘플과 LayerId별 부모/자식 설정을 보관한다. Application 루프가 프레임당 한 번 갱신한다.',
'Motion':'MotionStep 배열이다. 정해진 시간의 parent/local 변환을 샘플링한다. 충돌·키 상태를 소유하지 않는다.',
'GridLayout':'origin/extent/cells/frame으로 상대 비율·셀 중심과 좌표계 변환을 계산한다. Scene이나 표시 객체를 소유하지 않는다.',
'RoomBody':'방 안의 localPosition/localVelocity와 이탈 후 worldPosition/worldVelocity를 분리한다. released가 계산 좌표계를 결정한다.',
'CharacterBody':'발 위치·진행 방향·속도·XZ 반폭·높이·수직 속도·접지·실제 이동 거리를 보관한다. 표시 관절 데이터와 다르다.',
'JointRig':'root/head/shoulder/hip의 EntityId만 보관한다. 모델 메모리와 관절 컴포넌트는 Scene이 소유한다.',
'DistancePath':'꺾은선 점과 누적 거리를 소유한다. 시계는 별도 AnimationClock에 있으며 sample은 거리에서 자세를 얻는다.',
'AnimationClock':'seconds와 playing을 갖는다. 시간 정지와 경로의 위치 샘플을 분리한다.',
'PatternSystem':'도형 조립·관절·카메라·응용 계산을 묶은 정적 함수 모음이다. 영속 Scene 상태를 따로 소유하지 않는다.',
'CollisionSystem':'반사·미끄럼·낙하·캐릭터 충돌과 비겹침 난수 배치를 계산한다. 물리 엔진 전체가 아닌 교육용 대리 모델이다.',
'Bounds3':'축 정렬 최소/최대 경계다. 크기가 양수인지 확인하고 포함·겹침을 계산한다.',
'RenderPass':'카메라·픽셀 viewport·렌더 옵션과 clear 정책을 묶는 값 데이터다. 별도 WGL 컨텍스트가 아니다.',
'RenderOptions':'깊이 검사·와이어프레임·텍스처·방향광·배경·컬링의 기본 정책이다. 그림자 맵 설정은 없다.',
'PlaneFigure':'정점·인덱스·법선·색·UV·재질·면 로컬 변환·컬링/가시성을 가진 메시다. Mesh는 이 타입의 별칭이다.',
'Polyhedron':'면/메시 배열을 소유한다. Model은 별칭이며 EntityId와 다른 개념이다.',
'Component_Poly':'모델·변환·부모 ID·가시성·tint를 묶어 Scene에 저장하는 실제 컴포넌트다.',
'Diagnostics':'오류 옵션과 마지막 오류·위치/Operation 문맥을 저장한다. GPU 내부 모든 단계의 결과를 읽는 디버거가 아니다.',
'Module':'HMODULE을 보관하고 DLL 해제를 책임진다. 복사를 금지한다. 함수 주소를 읽어 API 포인터에 연결한다.',
'AssetLoader':'프로젝트 루트·옵션·이미지 캐시를 보관하며 상대경로 경계를 검사한다. 로더 결과가 Scene 등록을 대신하지 않는다.',
'Console':'main이 생성하고 Application 참조를 빌려 쓴다. 실습별 입력·상태와 Scene ID를 관리한다. run 종료 전까지 살아 있어야 한다.'}
PURPOSE={
'main':'Application과 응용 객체를 만든 뒤 콜백을 등록하고 run으로 제어권을 넘긴다. 등록과 콜백 실행 시점을 구분한다.',
'initScene':'CPU Scene과 실습 상태·카메라·렌더 옵션을 초기화한다. 실패하면 초기화 콜백 결과를 통해 루프 진입을 막는다.',
'onKey':'키 action/mods를 해석해 응용 상태를 바꾼다. press-only 토글과 repeat 이동/속도 명령을 구분한다.',
'onUpdate':'한 프레임의 dt로 논리 상태를 갱신하고 계산이 끝난 변환을 Scene에 반영한다. Application의 자동 Motion 갱신을 중복 호출하지 않는다.',
'onRender':'표시할 카메라/viewport를 구성하고 같은 Scene을 렌더링한다. Scene의 시간을 다시 진행시키지 않는다.',
'onFocus':'포커스 상실 시 지속 키·드래그 상태를 해제하거나 시뮬레이션 갱신을 제한한다.',
'onMouse':'마우스 버튼 이벤트에서 선택·드래그 시작/종료를 처리한다. 창 좌표와 framebuffer 좌표의 차이를 확인한다.',
'onCursor':'이전 마우스 좌표와의 차분을 계산해 드래그 회전을 누적한다. 절대 x를 매 프레임 더하지 않는다.',
'ensureContext':'생성 스레드·GLFW current window·실제 WGL context를 확인한다. 이미 연결된 컨텍스트를 매 pass마다 다시 연결하지 않는다.',
'run':'초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정상/예외 종료에서 정리한다.',
'initialize':'현재 책임의 네이티브/GPU 자원을 만든다. Application에서는 창·컨텍스트, Renderer에서는 셰이더·버퍼를 초기화한다.',
'shutdown':'Scene 사용이 끝난 뒤 유효한 컨텍스트에서 GPU 자원을 해제하고 창·GLFW를 종료한다.',
'destroy':'GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.',
'abandon':'컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다. 창 파괴가 컨텍스트 자원을 회수한다.',
'renderPass':'viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.',
'renderViews':'프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.',
'upload':'CPU 메시를 GpuVertex와 인덱스 배열로 변환해 VBO/EBO에 보낸다. geometry revision으로 재업로드 필요성을 판단한다.',
'draw':'각 면/선의 월드 행렬과 MVP/법선 uniform을 계산하고 DrawElements를 실행한다. 면별 컬링이 기본 렌더 정책을 덮어쓸 수 있다.',
'compileShader':'GLSL 소스를 컴파일하고 실패 로그와 실제 GLSL 파일 경로를 출력한다.',
'initShader':'내장/외부 GLSL을 고르고 vertex/fragment 컴파일·링크와 uniform 위치 조회를 수행한다. UTF-8 __FILE__은 u8path로 변환한다.',
'setPosition':'basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 worldPosition이라는 매개변수 이름과 달리 부모 좌표다.',
'worldMatrix':'부모 체인과 모션 레이어를 합성한다. parent/prefix/local/suffix 순서를 실제 소스와 함께 읽는다.',
'faceWorldMatrix':'엔티티의 월드 행렬 뒤에 face SRT와 localAnimationMatrix를 합성한다.',
'setTransform':'기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.',
'setParent':'부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존 reparent 기능은 아니다.',
'addBox':'양수 크기의 중심 기준 모델을 만들고 부모 로컬 위치·색·부모 관계를 설정한다.',
'addRoom':'여섯 면의 방을 만들고 바깥 winding에 Front cull, 안쪽 조명 법선을 적용한다. face 0..3은 옆벽, 4 천장, 5 바닥이다.',
'addJoint':'부모에 부착된 transform-only 그룹을 만든다. 표시 메시 크기와 회전 연결 위치를 분리한다.',
'rotateJoint':'연결 위치와 크기를 유지하면서 회전 기저를 변경한다.',
'conveyorCornerPose':'직선 → 안쪽 코너 중심 원호 → 직선의 거리에서 위치·접선·yaw를 계산한다. 끝에서 처음으로 순환한다.',
'follow':'DistancePath 샘플을 엔티티 위치와 +Z 기준 Y 회전에 반영한다.',
'cameraFacingWall':'시선 target-position과 옆벽의 월드 바깥 법선 내적이 가장 작은 면을 선택한다. 수직 시선에는 옆벽이 없을 수 있다.',
'openRoomWall':'선택 옆벽의 정점에서 상단 피벗을 계산하고 openFace로 높이를 축소/복원한다.',
'openFace':'진행률 [0,1]로 이동 또는 상단 피벗 Y축 축소를 적용한다. 완전 개방은 숨기고 0 배율 특이행렬을 피한다.',
'stepRoomBody':'방 내부에서는 로컬 반사/중력을 계산하고 released 뒤에는 월드 중력으로 적분한다.',
'releaseBody':'개방 시 내부 물체의 로컬 상태를 월드 위치·속도와 회전으로 옮기고 released로 고정한다.',
'stepCharacter':'수평 이동·장애물/경계 반전·중력·윗면 착지·지지면 이탈을 하위 단계로 계산한다.',
'randomFreeBox':'제한 횟수 안에서 occupied와 겹치지 않는 배치를 찾는다. 난수 엔진의 시드·수명은 호출자가 관리한다.',
'jump':'접지 상태에서만 위쪽 속도를 설정한다. 공중 입력 거절은 정상적인 false다.',
'addWalkingRig':'그리드 비율로 머리·코·몸통·어깨·엉덩이·팔·다리 계층을 조립하고 ID 묶음을 반환한다.',
'poseWalkingRig':'루트 이동/yaw와 실제 이동 거리 기반 팔다리 반대 위상을 적용한다. 속도는 보행 진폭에 반영한다.',
'threeViews':'원근·XZ 평면도·XY 정면도 카메라와 픽셀 영역을 구성한다. 상단 뷰의 up은 -Z다.',
'moveCamera':'시점과 주목점을 동일하게 이동해 시선 방향을 보존한다.',
'yawCamera':'시점은 고정하고 카메라 로컬 up축을 기준으로 시선을 회전한다.',
'orbitCamera':'월드 축에 대해 시점·주목점·up을 함께 변환한다. 축을 target에 두면 target은 고정된다.',
'zoomCamera':'원근은 target까지 거리, 직각은 orthographicHalfExtent를 배율로 바꾼다.',
'addNumber':'0~9를 선분으로 구성한다. 부모 로컬 XY 평면의 표시이며 일반 폰트/텍스트 렌더러는 아니다.',
'sample':'현재 시간 또는 거리를 읽어 위치/변환 결과를 얻는다. 샘플과 영속 상태 변경을 구분한다.',
'update':'시간 또는 프레임 상태를 진행한다. MotionSystem은 임시 샘플을 모은 뒤 성공한 경우 반영한다.',
'advance':'playing일 때만 누적 시간을 늘린다. dt와 누적 값의 유한성·범위를 검사한다.',
'foldFace':'지정 회전축을 중심으로 면 로컬 애니메이션 행렬을 계산한다.',
'screenOnPlane':'화면 픽셀을 ray로 바꾸어 평면 교차 위치를 얻는다. 평행/뒤쪽 교차는 빈 결과다.',
'snap':'좌표를 양수 간격으로 반올림한다. 실패 시 입력 점을 그대로 유지한다.',
'swapModels':'두 ID의 위치는 유지하고 모델을 교환한다.',
'reset':'응용 시간·ID·계층·카메라·입력과 모드를 초기 상태로 복구한다.',
'placeMovingObjects':'상자와 줄무늬가 공유하는 거리 시계에 offset을 더해 각 최종 자세를 계산한다.',
'syncScene':'물리/논리 상태에서 결정한 월드 위치·회전을 표시 Scene에 반영한다.',
'require':'실패한 bool/optional 검사 결과를 예외로 바꾸어 상위 초기화/루프에서 원인을 보고하게 한다.'}

TARGET={
'20':{'flow':['키 b/줌/공전','공유 clock.seconds','거리 + 객체 offset','직선/원호/직선 Pose','Scene.setTransform','Renderer·MVP'],
 'state':'Console이 clock_, 상자5개 ID, 줄무늬30개 ID와 카메라 지속 키를 보관한다. 번호는 상자 자식 joint/line이라 부모 변환을 따라간다.',
 'math':'코너 C=(2.2,1,-1.2), r=.8. 원호 구간의 θ=(d-L직선)/r. p=(Cx+r sinθ,1,Cz-r cosθ), tangent=(cosθ,0,sinθ), yaw=atan2(tx,tz). 0≤θ≤π/2라 +X 진행에서 +Z 진행으로 연속 회전한다. 총길이는 약 11.656637, 속도1.25이면 순환은 약9.325초다.',
 'trace':'b → playing=true → dt=.02 → 거리+.025 → i번째 offset=i*L/5 → 코너 구간이면 θ 증가 → 위치와 yaw를 함께 반영. b 재입력으로 시간이 멈추면 위치·yaw·간격도 유지된다.',
 'pitfall':'상자만 시간을 갱신하거나 줄무늬에 다른 속도를 주면 벨트와 어긋난다. 코너는 회전만 보간하는 것이 아니라 피벗 주위 위치도 회전해야 한다. L자로 오목한 전체 바닥을 단일 triangle fan으로 만들지 않는다.'},
'21':{'flow':['키/마우스 dx','Yspin × Ztilt 방 자세','로컬 RoomBody','e: 모든 물체 release','월드 중력 적분','syncScene → 표시'],
 'state':'Console은 방 ID·방 경계·회전/드래그·BodyObject 배열을 갖는다. 각 BodyObject는 표시 ID와 RoomBody 및 이탈 회전을 연결한다. 방 표시와 공/상자는 각각 root 객체다.',
 'math':'공은 중심 허용 범위 [min+radius,max-radius]에서 반사한다. 상자의 로컬 중력은 Rᵀg다. 미이탈 물체의 월드 위치는 R*p_local. e를 누르면 p_world=R*p_local, v_world=R*v_local로 옮긴 뒤 released=true. 이후 v+=g*dt, p+=v*dt.',
 'trace':'v → 30도/초 Y자전 토글 → 방은 Front cull 유지. e → 바닥 face5 숨김 → 배열의 모든 미이탈 공·상자 release → 다음 update에서 모두 월드 중력 낙하. 바닥을 닫아도 이미 이탈한 물체는 계속 떨어진다.',
 'pitfall':'F는 e의 호환 별칭이다. 시각적으로 열린 앞면과 물리 반사벽은 다른 정책이다. 방 축 정렬 상자·교육용 미끄럼이며 공끼리 충돌·마찰·정밀 강체 토크는 없다.'},
'22':{'flow':['방향키/부품 토글','탱크 상태 한 번 갱신','관절 transform 반영','현재 framebuffer 크기','threeViews','같은 Scene 3 pass'],
 'state':'탱크 root→central→upper→gun/pole 계층과 ground를 Scene ID로 조립한다. Console은 이동·회전·교환 시간과 입력을 보관하며 렌더 패스는 그 상태를 읽는다.',
 'math':'각 pass마다 MVP=P_i*V_i*M. 원근은 왼쪽 2/3, XZ 평면도는 오른쪽 위, XY 정면도는 오른쪽 아래. aspect=각 viewport.width/height. scissor가 clear 범위를 제한하고 깊이 버퍼 간섭을 막는다.',
 'trace':'오른쪽 방향키 → position.x += speed*dt 한 번 → root 이동 → 원근/평면도/정면도가 같은 M을 각각 다른 P,V로 읽는다. renderViews 안에서 onUpdate를 다시 호출하지 않는다.',
 'pitfall':'glViewport만 바꾸면 glClear가 영역 밖까지 지울 수 있다. 3뷰에 WGL 컨텍스트 세 개를 만들 필요가 없다. 포신은 몸체 중심, 깃대는 밑점 피벗을 유지한다.'},
'23':{'flow':['WASD/속도/J/O/I','CharacterBody·난수 배치','충돌/반전/착지','보행 루트·관절','벽별 개방 진행률','Front cull + 렌더'],
 'state':'Console은 CharacterBody, JointRig ID, 장애물 Bounds3 3개, 벽별 open/doors와 유지되는 mt19937 엔진을 소유한다. 실제 모델·부모 관계는 Scene에 있다.',
 'math':'f=normalize(target-position). 각 옆벽의 바깥 법선 n을 월드로 옮겨 dot(n,f)가 가장 작은 면을 선택한다. 상단 피벗 C에 T(C)*S(1,1-progress,1)*T(-C)를 적용한다. 보행은 이동 거리의 sin 위상으로 좌우를 반대로, 팔은 같은 쪽 다리와 반대로 흔든다.',
 'trace':'I → 기존 난수 엔진의 다음 수열로 장애물3개를 다시 배치. 카메라를 옆으로 공전 → O → 그 순간의 벽 ID 저장 → 해당 벽의 개방 목표 토글 → 1초 동안 축소/복원. 카메라를 돌려도 진행 중 대상이 바뀌지 않는다.',
 'pitfall':'시드2301을 initScene에서 매번 주면 같은 배치가 반복된다. 바닥/천장은 O 선택에서 제외한다. Front cull로 이미 가려진 가까운 벽은 외부 시점에서 축소가 눈에 잘 보이지 않을 수 있다. 문 선택과 컬링은 다른 처리다.'},
'standard':{'flow':['0 기존 Motion / 7 Grid / 8 Room','CPU Scene 구성','입력·업데이트','자동 Motion 갱신','V: 1뷰/3뷰','피킹·D 단계 진단'],
 'state':'Console은 모드, MotionId/LayerId, 거리 시계와 RoomBody를 갖는다. Application은 그 Console을 소유하지 않고 등록된 람다가 참조로 접근한다.',
 'math':'GridLayout p_world=frame*(origin+extent*ratio). cells=(6,1,4)일 때 cellCenter는 (index+.5)/cells 비율이다. 기존 반사 레이어는 음수 determinant가 가능하므로 winding 보정과 노멀 계산을 따로 본다.',
 'trace':'7 → scene.clear로 ID 재발급 → Grid 셀을 addBox로 구성 → Clock+DistancePath로 상자 이동. V → 같은 Scene 3뷰. 8 → RoomBody 예제. 0 → 기존 모션/레이어 초기화.',
 'pitfall':'standard는 새 실습의 요구사항을 자동 구현해 주는 프로그램이 아니다. Console의 상태와 입력을 먼저 줄이고 필요한 API만 가져온다. V 모드 피킹은 클릭한 viewport의 카메라를 사용해야 한다.'},
'framework':{'flow':['Application: 입력·수명','Scene/Motion: CPU 상태','Renderer.upload: VBO/EBO/VAO','Vertex shader: MVP','클리핑·래스터화','Fragment shader·depth·swap'],
 'state':'Application은 Scene/Camera/Renderer/옵션을 소유한다. Scene은 CPU 컴포넌트와 ID 관계, Renderer는 GPU 핸들·텍스처 캐시, Module은 DLL 핸들을 소유한다.',
 'math':'local=T(basis)*R*S*T(-localBasis), world=부모 행렬과 레이어 합성, clip=P*V*M*p. 법선은 비균일 배율에서 mat3(M)의 역전치로 변환한다. 렌더러는 geometry revision이 변할 때만 정점 배열을 다시 업로드한다.',
 'trace':'main에서 등록한 init 람다는 run 초기화 뒤 실행한다. 매 루프 PollEvents → updateHandler → Scene.updateMotions → renderHandler 또는 renderScene → SwapBuffers. 최소화 시 WaitEvents와 시간 기준 갱신, 종료 시 GPU→window→GLFW 순서다.',
 'pitfall':'GL 함수 포인터·콜백은 정적 그래프가 실행 순서를 완전히 복원하지 못한다. __FILE__은 /utf-8에서 u8path로 변환한다. WGL current 확인 실패 후 GL 명령이나 삭제를 계속 호출하지 않는다.'}}

PURPOSE.update({
 'roomWorld':'Y축 자전과 Z축 기울기를 곱해 방 로컬→월드 행렬을 만든다. 물리와 표시에서 같은 행렬을 공유한다.',
 'addBall':'최대5개 제한과 반지름·기존 물체 경계를 검사하여 공을 생성한다. 바닥 개방 중 생성한 공은 즉시 월드 낙하로 전환한다.',
 'printHelp':'현재 응용의 키와 입력 지속 정책을 콘솔에 출력한다. 안내 문자열과 onKey 분기가 일치하는지 함께 본다.',
 'zoom':'현재 투영 모드의 거리/extent를 유효 범위로 제한하고 공통 zoomCamera를 호출한다.',
 'resetState':'20의 시간·입력·카메라를 초기화하고 전체 이동 객체를 같은 거리 시계로 다시 배치한다.',
 'box':'공통 addBox로 탱크 표시 부품을 만들고 색상 변경용 ID 목록에 넣는다.',
 'recolor':'현재 colorVersion과 팔레트로 바닥/탱크 부품의 tint를 갱신한다. 형상이나 관절 위치는 바꾸지 않는다.',
 'apply':'탱크 논리 시간·각도·교환 진행률에서 중앙/하부/상부/포신/깃대의 최종 변환을 계산한다.',
 'choosePath':'사용자가 고른 경로/보간/재생 설정으로 기존 경로 모션을 교체하고 시각을0으로 되돌린다.',
 'changeReflection':'반사 모드를 순환하고 해당 레이어의 고정 변환을 교체한다. 시간 진행과 반사 기여를 분리한다.',
 'toggleInsertedLayer':'첫 객체의 자전 레이어 위에 로컬1.5배 크기 레이어를 삽입하거나 제거한다.',
 'initGrid':'기존 Scene을 비우고 Grid 셀 바닥과 거리 기반 번호 상자 예제로 바꾼다.',
 'initRoom':'기존 Scene을 비우고 기울어지는 방·상자·바닥 개방 예제를 만든다.',
 'pushLayer':'유효한 부모 아래 새 자식 레이어를 추가한다. 복사한 임시 맵에 반영한 뒤 swap하여 상태를 확정한다.',
 'insertLayer':'기존 child와 그 parent 사이에 새 레이어를 끼운다. root 위 삽입은 허용하지 않는다.',
 'removeLayer':'root 이외 레이어를 제거하며 자식들을 부모의 해당 위치로 승격한다. 월드 자세 보존을 자동 약속하지 않는다.',
 'popLayer':'자식이 없는 비root leaf만 제거하고 그 부모 ID를 반환한다.',
 'reparentLayer':'레이어의 부모를 바꾸며 자신의 자손으로 연결되는 순환을 거부한다.',
 'layerMatrices':'leaf까지의 계층을 따라 transform과 motion 샘플을 World/Local 공간에 맞게 parent/local 행렬로 합성한다.',
 'then':'모든 앞 단계의 유한 종료 시각을 구한 뒤 새 단계의 시작을 그 뒤로 이동해 순차 실행을 만든다.',
 'seekMotion':'지정 시각을 샘플링하고 유효할 때 모션의 시간과 샘플을 함께 바꾼다.',
 'bindMotion':'기존 레이어에 모션 ID를 연결하거나 해제한다. 모션과 레이어가 존재하는지 검사한다.',
 'setSideLengths':'기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. 모델 수정이므로 geometry revision이 바뀐다.',
 'setLocalBasis':'피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.',
 'getFramebufferSize':'현재 창의 실제 픽셀 크기를 반환한다. DPI 배율이 있으면 창 좌표 크기와 다를 수 있다.',
 'pickAt':'창 좌표를 framebuffer 좌표로 보정하고 기본 카메라의 전체 viewport로 ray picking을 수행한다.',
 'checkGlAt':'GL 오류 큐를 읽고 검사 호출 위치·단계·API와 조치를 출력한다. 드라이버 오류가 단일 원인을 확정하지는 않는다.'})

def application_file(scope):return ('standard' if scope=='standard' else 'openGL_3dAssign_'+scope)+'/Console.cpp'
def main_file(scope):return ('standard' if scope=='standard' else 'openGL_3dAssign_'+scope)+'/main.cpp'
def graph_data(scope):
    folder=OUT/scope/'graphify-out'
    return json.loads((folder/'graph.json').read_text(encoding='utf-8-sig')),json.loads((folder/'EXTRACTION.json').read_text(encoding='utf-8-sig'))
def canon(scope,file):return 'framework/'+file if scope=='framework' else file
def block(file,line):
    text=(ROOT/file).read_text(encoding='utf-8-sig');lines=text.splitlines();start=max(0,line-1);fragment='\n'.join(lines[start:])
    # Locate C++ bodies without counting ordinary quoted strings or comments as braces.
    masked=re.sub(r'"(?:\\.|[^"\\])*"|\'(?:\\.|[^\'\\])*\'|//[^\n]*|/\*[\s\S]*?\*/',lambda m:''.join('\n' if c=='\n' else ' ' for c in m[0]),fragment)
    brace=masked.find('{');semi=masked.find(';')
    if brace<0 or (semi>=0 and semi<brace):return fragment[:semi+1] if semi>=0 else lines[start]
    depth=0
    for i in range(brace,len(masked)):
        if masked[i]=='{':depth+=1
        elif masked[i]=='}':
            depth-=1
            if depth==0:return fragment[:i+1]
    return '\n'.join(lines[start:start+15])
def purpose(name,is_class,file):
    leaf=name.split('::')[-1]
    if is_class:return OBJECTS.get(leaf,'값·자원·상태를 묶는 타입이다. 아래 멤버 선언과 초기값을 보고 소유 데이터와 참조/ID를 구분한다. 관련 함수는 같은 파일/타입의 항목을 함께 읽는다.')
    if leaf in PURPOSE:return PURPOSE[leaf]
    if leaf.startswith('get'):return '보관된 상태의 접근점이다. 반환 선언이 값/참조/포인터/optional 중 무엇인지 확인한다. 읽기 접근이 자원의 소유권 이전을 뜻하지는 않는다.'
    if leaf.startswith('valid'):return '호출 전에 필요한 값·범위·행렬 조건을 검사한다. 아래 조건식의 AND/OR와 유한성·인덱스 검사를 순서대로 읽는다.'
    if leaf.startswith('set'):return '기존 상태를 갱신하는 API다. 아래 인자와 거부 조건을 통과한 뒤 어떤 멤버/위임 setter가 바뀌는지 확인한다. 표시 변환과 형상 재업로드는 구분한다.'
    if leaf.startswith(('add','create','push','insert')):return '새 도형·모션·레이어·구성요소를 만든다. 아래 반환 ID/optional과 실제 저장 컨테이너, 실패 분기를 연결해 읽는다. ID를 객체 소유 포인터로 해석하지 않는다.'
    if leaf.startswith(('remove','erase','pop','clear','unbind')):return '기존 항목 또는 연결을 해제한다. 자식 승격·루트 예외·ID 재사용 여부는 아래 실제 분기와 호출을 확인한다.'
    if leaf.startswith(('load','decode','read','resolve')):return '파일·DLL·자원 입력을 읽거나 경로를 해석한다. 프로젝트 상대경로와 UTF-8/Windows wide 경로 경계, 예외/결과 반환을 함께 확인한다.'
    if 'Matrix' in leaf or leaf in ['translation','rotation','scaling','reflectPoint','reflectLine','reflectPlane']:return '좌표/기저에서 변환 행렬을 계산한다. 아래 식에서 피벗·좌표계·곱셈 순서를 확인하고 불가능한 입력의 반환을 구분한다.'
    if leaf in ['ellipse','circle','rectangle','polygon','polyline','linear','smoothStep']:return '경로 또는 보간 규칙을 구성한다. 샘플 인자가 시간인지 정규화 진행률인지 확인하고 경로 생성과 매 프레임 평가를 구분한다.'
    return '아래 실제 정의에서 인자 → 조건 분기 → 상태 변경/하위 호출 → 반환을 따라 읽는다. 인접 호출 관계는 graphify의 정적 후보이며 런타임 실행을 보장하지 않는다.'

def lighting(scope):
    enabled='실습 '+scope+'은 Console 초기화에서 lighting=true를 켠다.' if scope in {'20','21','22','23'} else ('standard는 현재 Console에서 lighting을 켜지 않아 기본 false다. 색 차이·깊이 가림을 조명 그림자로 해석하지 않는다.' if scope=='standard' else 'RenderOptions.lighting의 기본값은 false다. 호출한 응용이 true로 켠 경우에만 방향광 명암이 적용된다.')
    return section('그림자처럼 보이는 부분의 실제 계산',para(enabled)+
      para('현재 기본 GLSL에는 shadow map·light-space depth 비교·투영 그림자 계산이 없다. 광원 방향과 표면 법선으로 <b>표면 명암(shading)</b>을 계산한다. 물체가 다른 표면에 드리우는 <b>투영 그림자(cast shadow)</b>와 다르다.')+
      table(['처리','이 코드의 동작','구분 기준'],[
       ['방향광 명암','lighting=true인 삼각형 면에서 N·L을 사용한다.','빛을 등진 면 자체가 어두워진다. 바닥의 다른 위치에 그림자를 만들지 않는다.'],
       ['은면 제거','depthTest, GL_LESS와 깊이 버퍼로 가까운 조각을 남긴다.','관찰 카메라 기준 가림이다. 광원 기준 가시성 검사는 아니다.'],
       ['Front cull','방의 카메라 쪽 바깥 앞면 삼각형을 버려 내부를 보이게 한다.','삼각형 winding을 이용한 면 제거다. 조명·그림자와 별개다.'],
       ['와이어프레임 depth prepass','선 뒤의 면을 깊이에 먼저 기록해 뒷선을 가린다.','빛이 만든 그림자 패스가 아니다.'],
       ['실제 그림자','현재 구현 없음.','추가하려면 광원 관점 깊이 맵/비교, bias·자원·추가 pass 설계가 필요하다. 이번 작업에 구현하지 않았다.']])+
      code('N = normalize(uNormalMatrix * localNormal)\nL = normalize(uLightDirection)\nbrightness = 0.25 + 0.75 * max(dot(N,L), 0)\ncolor.rgb *= brightness * uLightColor\nN·L=1 → 1.0, N·L=0 → 0.25, N·L=-1 → 0.25')+
      para('0.25는 고정 주변광처럼 더한 밝기다. 물리 기반 간접광 계산은 아니다. 면 안쪽을 그리는 방은 vertex normal을 반대로 만들어 조명 방향도 내부 표면에 맞춘다. 선은 조명 적용을 끈다. 텍스처는 옵션을 켠 경우 색에 곱한다.')+
      ref('framework/detail/Shaders.h','brightness' if 'brightness' in (ROOT/'framework/detail/Shaders.h').read_text(encoding='utf-8-sig') else '0.25','실제 fragment shader')+' · '+ref('framework/detail/Renderer.inl','inline bool Renderer::draw','draw 상태/법선 uniform')+
      '<details><summary>실제 내장 셰이더 전체</summary>'+code((ROOT/'framework/detail/Shaders.h').read_text(encoding='utf-8-sig'))+'</details>')

def pipeline(scope):
    info=TARGET[scope]
    body=para('대상: '+TITLES[scope]+'. 공통 프레임워크는 정규 원본을 분석하고 프로젝트 안의 SDK·프레임워크 중복 사본은 그래프에서 제외했다. 파이프라인은 GPU 흐름, 콜백은 CPU 실행 시점이다.')
    body+=section('이 대상의 입력 → 상태 → 출력',flow(info['flow'])+para(info['trace']))
    if scope!='framework':body+=para(ref(main_file(scope),'int main','이 프로젝트의 main')+' → '+ref(application_file(scope),'Console::initScene','initScene')+' → '+ref(application_file(scope),'Console::onUpdate','onUpdate'))
    body+=section('공통 실행 순서: 등록과 실행을 분리',flow(['main: 객체 생성','set...Handler: 람다 저장','run: 창/컨텍스트/셰이더','initHandler: Scene 생성','PollEvents → 입력 콜백','update → 자동 Motion → render → swap'])+
      para('main의 set...Handler 호출은 함수를 등록한다. 람다 본문은 run이 나중에 실행한다. initHandler는 초기화 뒤 한 번, updateHandler는 프레임마다, renderHandler가 없으면 renderScene이 실행된다. dt는 [0,.05]로 제한하고 최소화에서는 이벤트 대기로 진행을 멈춘다.'))
    body+=section('한 정점이 화면으로 가는 단계',table(['단계','실제 입력/계산/출력','코드'],[
      ['CPU 장면','Mesh 정점·색·UV·법선·삼각형 인덱스를 보관한다.',ref('framework/include/Model.h','struct PlaneFigure')],
      ['업로드·정점 입력','GpuVertex 배열 → VBO, 인덱스 → EBO, VAO attribute 0/1/2/3. geometry revision이 변하면 다시 올린다.',ref('framework/detail/Renderer.inl','inline bool Renderer::upload')],
      ['모델/뷰/투영','Scene 계층 월드 M, 카메라 V, viewport별 P → uMVP=P*V*M.',ref('framework/detail/Renderer.inl','inline bool Renderer::draw')],
      ['정점 셰이더','gl_Position=uMVP*vec4(vPos,1), 색/UV/변환 법선을 다음 단계로 보낸다.',ref('framework/detail/Shaders.h','layout(location=0)')],
      ['조립·클리핑·래스터화','GL_TRIANGLES/GL_LINES를 조립하고 클립 공간을 자른 뒤 원근 나눗셈·viewport·래스터화를 진행한다. CPU의 별도 C++ 함수 호출이 아니다.',ref('framework/detail/Renderer.inl','gl.DrawElements')],
      ['프래그먼트·깊이','색*tint*선택 텍스처*선택 방향광. 깊이 검사는 카메라 가림을 결정한다.',ref('framework/detail/Shaders.h','inline constexpr char fragmentShader')],
      ['프레임 표시','scissor/clear가 pass 영역을 제어한다. 한 창/컨텍스트에서 swap한다.',ref('framework/detail/Application.inl','SwapBuffers(window_)')]]))
    body+=section('이 대상의 계산 예시',para(info['math']))
    body+=section('컨텍스트·자원 정리',para('Application이 창/컨텍스트 수명을 가진다. ensureContext는 생성 스레드·GLFW current·실제 WGL context를 확인한다. 실패하면 GL 작업을 계속하지 않는다. 유효할 때 GPU destroy, 실패 상태에서는 abandon 후 창 파괴로 회수한다. 각 viewport는 별도 컨텍스트가 아니다.')+ref('framework/detail/Application.inl','Application::ensureContext'))
    return body

def ownership(scope):
    info=TARGET[scope]
    layouts={
      '20':'Console: clock_ + boxes_[5] + stripes_[30]\nScene: 바닥/벨트/상자/줄무늬\n상자 Entity → 번호 joint(4방향) → 숫자 line\n상자와 줄무늬는 같은 거리 시계를 읽음',
      '21':'Console: roomBounds_/roomAngle_/roomSpinAngle_/floorOpen_/objects_\nScene root: room 표시 + 각 box/sphere 표시 (부모 회전 중복 없음)\nBodyObject: entity ID ↔ RoomBody(local 또는 released world) + releasedRotation\ne: 모든 body release / v: 방 Y자전만 토글',
      '22':'root_ (탱크 XZ 이동)\n  ├ lower_[2] (높이와 접지)\n  └ central_ (높이 + Y자전)\n      └ upper_[2] (반원 교환)\n          ├ guns_[2] (몸체 중심 yaw) → 포신 표시\n          └ poles_[2] (밑점 pitch) → 깃대 표시\nground는 탱크 루트 밖',
      '23':'Console: CharacterBody + JointRig + obstacles_[3] + random_ + doors_[4]/open_[4]\nScene: stage와 obstacle 모델, robot root\nrobot root (feet + yaw)\n  ├ torso 표시\n  ├ head joint → head/nose 표시\n  ├ shoulders[2] → arm 표시\n  └ hips[2] → leg 표시\n충돌 대리 상자는 root의 논리 상태이며 개별 팔다리 메시와 다름',
      'standard':'main의 Console → demo_ 모드\n0: MotionId/LayerId → Scene entity binding\n7: AnimationClock/DistancePath → 번호 상자\n8: RoomBody → room/cube ID\nV: 동일 Scene을 다른 pass로 읽기 (상태 복제 없음)',
      'framework':'Application\n  ├ Scene → Component_Poly/Model/Face + Line3D + MotionSystem\n  ├ Camera + RenderOptions\n  ├ Renderer → VAO/VBO/EBO/program + texture cache\n  ├ optional AssetLoader → weak image cache\n  └ callback std::function → 외부 Console 참조\nModule → HMODULE / GLFWwindow* → 네이티브 창 핸들'}
    return (section('이 대상의 응용 상태',para(info['state'])+code(layouts[scope]))+section('소유 관계와 의존 방향',para('아래 표는 실습의 Console 사용 패턴이다. framework/examples의 독립 예제는 Console 없이 main이 응용 상태를 소유할 수도 있다.')+flow(['main: Console 생명주기','Application& 빌림','Scene ID로 상태 반영','Renderer는 Scene을 읽음'])+
      table(['주체','소유하는 것','소유하지 않는 것/주의'],[
       ['main','Application과 Console 지역 객체','Console을 캡처한 참조는 run이 끝날 때까지 유효해야 한다.'],
       ['Application','Scene, Camera, Renderer, 옵션, optional AssetLoader, std::function 콜백','Console은 바깥 객체다. GLFWwindow*는 네이티브 창의 관리 핸들이다.'],
       ['Console','키/모드/시간/충돌 상태와 각 EntityId','EntityId는 포인터가 아니다. 모델과 GPU 핸들을 직접 소유하지 않는다.'],
       ['Scene','vector의 Component_Poly/모델, 선, MotionSystem','parent는 optional ID 관계다. Scene::clear 뒤 기존 ID 사용 금지.'],
       ['Renderer','GLuint 프로그램/VAO/VBO/EBO, GPU texture 캐시','CPU Mesh 자체나 카메라 입력을 결정하지 않는다. GPU 핸들 복사로 수명이 복제되지 않는다.'],
       ['AssetLoader / Texture','경로·약한 이미지 캐시, shared_ptr<const TextureImage>로 이미지 공유','weak_ptr은 이미지 수명을 연장하지 않는다. Scene 등록과 로딩은 별도다.'],
       ['Module','HMODULE과 함수 export 주소 조회','DLL/컨텍스트 수명보다 함수 포인터를 오래 쓰면 안 된다.']]))+
      section('추상화별로 바꿀 코드',table(['요구 변화','주로 바꿀 계층','예시'],[
       ['키/모드/초기 배치/게임 규칙','Console','b/e/v/o, 속도, 장애물 개수, 완료 상태'],
       ['재사용 가능한 거리·충돌·관절·카메라 계산','PatternSystem/CollisionSystem/GridLayout','코너 원호, cameraFacingWall, jump와 착지'],
       ['형상과 계층의 저장/유효성','Scene/Model','setParent, setTransform, Face cull/visibility'],
       ['픽셀 영역·GPU 자원·드로우 상태','Renderer/RenderPass','3뷰 scissor/clear, 노멀 uniform, EBO offset'],
       ['명암식·표면 출력','GLSL','방향광. 투영 그림자는 현재 범위 밖'],
       ['VS/SDK/DLL·문자 인코딩','props/Native/Application','u8path, x64, WGL 컨텍스트, runtime 배치']]))+
      section('합성에서 흔한 소유·상태 오류',para('표시 객체를 방의 자식으로 둔 뒤 물리에서 계산한 월드 위치를 다시 넣으면 회전/이동이 중복될 수 있다. Motion이 움직이는 위치를 Console에서 또 덮어쓰지 않는다. 물리 상태 → 최종 위치를 결정하는 책임을 한곳에 둔다. 새 프레임마다 모델을 재생성하는 것과 transform만 갱신하는 것은 비용과 수명이 다르다.')+para(info['pitfall'])))

def symbols(scope,extraction,all_sources):
    nodes=extraction['nodes'];by_key={}
    for n in nodes:
        if not n.get('_callable') or not n.get('source_file') or not n.get('source_location'):continue
        file=canon(scope,n['source_file']).replace('\\','/')
        if not (ROOT/file).is_file() or file.endswith('Shaders.h'):continue
        match=re.search(r'\d+',str(n['source_location']))
        if not match:continue
        line=int(match[0]);name=n.get('label','');is_class=bool(n.get('_callable_class'))
        key=(file,name,is_class)
        if key not in by_key or n.get('_origin')=='source_definition_review':by_key[key]=(line,n)
    entries=sorted(by_key.items(),key=lambda x:(x[0][0],x[1][0],x[0][1]))
    body=para('graphify가 소스 위치를 가진 타입·함수로 추출한 항목을 파일/심볼별로 묶었다. 외부 라이브러리의 정의 없는 타입은 설명 대상으로 꾸며 넣지 않았다. 아래 정의·선언과 실제 구현을 구분하며, 호출 후보는 실행을 보장하지 않는다. 공통 API도 이 대상의 그래프에 포함되어 있다.')
    body+='<p><input type="search" data-search placeholder="심볼·파일 검색: onUpdate, Renderer, cameraFacingWall …" aria-label="객체 함수 검색"></p>'
    body+=para(f'위치가 확인되는 타입/함수 항목 {len(entries)}개. '+('<a href="../framework/symbols.html">공통 프레임워크 카탈로그</a>' if scope!='framework' else '각 소스 링크에서 전체 파일도 읽을 수 있다.'))
    raw_out={}
    for e in extraction['edges']:raw_out.setdefault(e.get('source'),[]).append(e)
    node_map={n['id']:n for n in nodes}
    for (file,name,is_class),(line,n) in entries:
        all_sources.add(file);snippet=block(file,line)
        signature=snippet.split('{',1)[0].strip()
        members=sorted(set(re.findall(r'\b\w+_\b',snippet)))
        returns=[' '.join(x.split()) for x in re.findall(r'\breturn\s+([^;]+);',snippet)]
        conditions=[' '.join(x.split()) for x in re.findall(r'\bif\s*\(([^\n]+)',snippet)]
        candidates=[]
        for e in raw_out.get(n['id'],[]):
            t=node_map.get(e.get('target'))
            if t and 'call' in e.get('relation',''):
                candidates.append(esc(t.get('label',''))+' <span class="tag">'+esc(e.get('confidence','EXTRACTED'))+'</span>')
        body+='<details data-symbol="'+esc(file+' '+name)+'"><summary>'+esc(name)+' · '+('타입' if is_class else '함수/선언')+'</summary>'
        body+=para(purpose(name,is_class,file))+para(source_link(file,line))
        body+=table(['읽을 단위','현재 소스에서 확인할 내용'],[
            ['인자·반환 선언',esc(signature)],['관련 상태 멤버',esc(', '.join(members)) or '소스 본문에 _ 접미 멤버 접근이 없음. 매개변수와 지역값을 확인한다.'],
            ['분기/거부 조건 발췌',esc('\n'.join(conditions[:4])) or '해당 발췌에 if가 없음. 위임 호출의 검사도 확인한다.'],
            ['반환식 발췌',esc('\n'.join(returns[:4])) or 'void 또는 타입 선언. 상태 변경·예외·자원 생성/해제를 확인한다.'],
            ['그래프의 직접 호출 후보',' · '.join(candidates[:12]) or '이 앵커의 직접 호출 간선이 없음. 아래 본문과 pipeline의 콜백 흐름을 확인한다.']])
        body+='<h3>정의/선언 원문</h3>'+code(snippet)+'</details>'
    return body

def notes(scope):
    info=TARGET[scope]
    body=section('계산을 코드와 연결',para(info['math'])+code(info['trace']))+lighting(scope)
    if scope=='23':body+=section('카메라 at·벽 선택·컬링을 분리',para('at을 target이라는 점으로 해석한다. 시선 벡터는 f=normalize(target-position)이다. 옆벽의 바깥 법선과 f의 내적이 가장 작은 면이 카메라를 향하는 벽이다. 예: position=(8,5,11), target=(0,1,0)이면 +Z 벽이 선택된다. +X 쪽으로 카메라를 옮기면 +X 벽이 선택될 수 있다. 동률은 작은 face ID를 유지한다. 완전 수직 시선은 선택하지 않는다.')+para('선택 대상은 o를 누를 때 고정한다. 벽별 개방 목표와 진행률을 따로 보관하므로 카메라 이동으로 애니메이션 대상이 튀지 않는다. 축소에 변형된 노멀 대신 원래 기하의 바깥 노멀을 사용한다. Front cull은 삼각형 winding으로 처리하므로 문 선택 계산과 별개다.'))
    body+=section('실패 증상 → 먼저 볼 위치',table(['증상','점검 순서'],[
       ['콘솔만 뜨고 종료','첫 ERROR의 stage/Source/Operation → UTF-8 경로·DLL → 컨텍스트 → 셰이더 순서. 예외 catch 줄을 실제 결함 줄과 혼동하지 않는다.'],
       ['객체가 안 보임','정점/인덱스 → 모델/부모 변환 → 카메라/클립 → winding/culling → depth → shader/tint 순서로 좁힌다.'],
       ['리셋 후 다른 객체가 움직임','Scene::clear 이후 ID 재발급, 캐시된 ID/포인터·입력 상태가 남았는지 확인한다.'],
       ['시간·속도가 두 배','onUpdate와 Scene.updateMotions의 중복 갱신 여부, pass마다 시간 갱신 여부를 확인한다.'],
       ['어두운 면이 그림자처럼 보임','uLighting·N·L과 normal matrix를 먼저 읽는다. 현재 그림자 맵이 있다고 추정하지 않는다.']]))
    body+=section('사용자에게 맡기는 최소 확인',para(info['pitfall'])+para('해당 .sln의 Debug/Release x64 빌드·실행, 모든 키·리셋·포커스·최소화·리사이즈·DPI를 확인한다. 에이전트는 수행하지 않았다. HTML 검색/탐색과 그래프 렌더도 브라우저에서 직접 확인해야 한다.'))
    return body

def sources(files):
    body=para('현재 작성 자료가 참조하는 정규 원본이다. SDK와 프로젝트별 framework 중복 사본은 제외했다. 각 줄의 링크는 HTML 안의 실제 원문 줄을 가리킨다.')
    for file in sorted(files):
        lines=(ROOT/file).read_text(encoding='utf-8-sig').splitlines()
        body+='<details><summary>'+esc(file)+'</summary><pre>'+''.join('<span class="line" id="'+source_id(file,i)+'">'+str(i).rjust(4)+' '+esc(text)+'</span>' for i,text in enumerate(lines,1))+'</pre></details>'
    page(OUT/'sources.html','전체 소스 위치 보기',body)

def exam():
    nodes=[('read','0. 문제를 계약으로 분해','필수 출력·키·객체·좌표계·시간·충돌·카메라를 표로 적는다. 요구되지 않은 기능은 뒤로 둔다.'),
     ('boot','1. 최소 창·종료 경로','허용된 standard에서 출발한다. main→init/update/input/render 등록과 종료를 남기고 데모 상태를 줄인다.'),
     ('scene','2. 정적 장면','바닥→대표 객체1개→좌표축/정면표식. 초기 카메라와 투영을 결정한다.'),
     ('hierarchy','3. 계층·피벗','부품인가? 루트→관절 그룹→표시 메시. 연결 위치와 메시 배율을 분리한다.'),
     ('input','4. 입력과 상태','press 토글/repeat/누르는 동안 이동을 구분한다. reset이 되돌릴 상태를 함께 정의한다.'),
     ('motion','5. 단일 시간·경로','시간 주인은 한곳. path distance·offset·공전/자전·순차 모션을 구분한다.'),
     ('collision','6. 충돌·접지','계산용 후보 위치→반크기 경계→충돌 해결→최종 Scene 변환. 상승/하강/착지를 구분한다.'),
     ('views','7. 카메라·뷰 분할','기본1뷰를 만든 뒤 view/pass를 추가. 업데이트1회, 카메라별 렌더 N회. viewport별 aspect/clear.'),
     ('lighting','8. 표현과 명암','색·선·방향광·컬링을 요구대로 적용. 그림자가 진짜 필수인지 표면 명암인지 구분한다.'),
     ('finish','9. 요구사항 대조','필수 키·초기 상태·정지/재개·reset·경계·최소화를 사용자가 확인. 미완료 기능을 숨기지 않는다.')]
    body=para('실습을 그대로 외우는 자료가 아니라 혼합·변형 요구를 구현 행동으로 바꾸는 지도다. 실제 시험 시간·표준 코드 사용 허용 여부는 제공되지 않았다. 아래 순서는 권장 학습 전략이며 시험 규정의 사실이 아니다. standard를 사용할 수 없다면 같은 책임을 직접 구현하는 순서로 해석한다.')
    body+=section('시험장에서 먼저 적을 요구사항 카드',table(['분류','문제에서 뽑을 것','예'],[
      ['출력','몇 개·어떤 형상·어느 위치','상자5/방/로봇/장애물3'],['변환','이동·자전·공전·피벗·크기','안쪽 코너 중심 회전, 어깨 연결점'],
      ['입력','키/마우스와 동작 지속 방식','한 번 누르면 계속/누르는 동안/재입력 정지'],['좌표계','모델/부모/월드/카메라','방 안 물리와 이탈 후 월드 중력'],
      ['시간','한 번/반복/왕복/순차','속도·거리·위상·완료 상태'],['판정','경계·충돌·성공·착지','중심이 아니라 반크기, 하강할 때만 착지'],
      ['시점·표현','원근/직각/뷰포트/컬링/명암','3뷰, Front cull, 방향광']]))
    body+=section('행동 그래프',flow(['요구 계약','최소 창','정적 Scene','입력/상태','필요 기능 분기','최종 변환','기본 렌더','추가 뷰/표현','대조'])+
      '<div class="box">아래 항목을 고르면 우선 구현 순서가 좁혀진다. JavaScript 없이도 모든 행동 카드와 표를 읽을 수 있다.<br>'+''.join('<label><input type="checkbox" class="requirement" value="'+key+'"> '+label+'</label>' for key,label in [('hierarchy','부품/관절'),('motion','경로/애니메이션'),('collision','충돌/점프/낙하'),('views','다중 뷰'),('lighting','명암/컬링')])+'<p id="plan">전체 행동 카드를 확인한다.</p></div>')
    # SVG is a literal action graph, not a screenshot or claimed runtime result.
    body+='<svg viewBox="0 0 1000 480" role="img" aria-label="요구사항에 따른 구현 행동 분기 그래프"><defs><marker id="arrow" markerWidth="8" markerHeight="8" refX="7" refY="4" orient="auto"><path d="M0,0 L8,4 L0,8" fill="#3a5978"/></marker></defs>'
    positions={'read':(20,30),'boot':(270,30),'scene':(520,30),'input':(770,30),'hierarchy':(70,200),'motion':(310,200),'collision':(550,200),'views':(790,200),'lighting':(310,370),'finish':(600,370)}
    edges=[('read','boot'),('boot','scene'),('scene','input'),('input','hierarchy'),('input','motion'),('input','collision'),('input','views'),('hierarchy','lighting'),('motion','lighting'),('collision','lighting'),('views','finish'),('lighting','finish')]
    for a,b in edges:
        x,y=positions[a];u,v=positions[b];body+=f'<path d="M{x+90},{y+55} L{u+90},{v}" stroke="#3a5978" fill="none" marker-end="url(#arrow)"/>'
    for key,title,_ in nodes:
        x,y=positions[key];body+=f'<a href="#{key}"><rect x="{x}" y="{y}" width="190" height="55" rx="8" fill="#e3edf7" stroke="#577694"/><text x="{x+10}" y="{y+32}" font-size="15">{esc(title)}</text></a>'
    body+='</svg>'
    for key,title,description in nodes:body+='<section id="'+key+'" class="box action"><h3>'+title+'</h3><p>'+description+'</p></section>'
    body+=section('요구에 따른 분기와 재사용할 API',table(['문제 문장','먼저 할 일','이 저장소에서 연결할 것'],[
      ['부품이 연결된 채 회전','관절 위치를 정적 장면에서 먼저 맞춘다. 그 다음 각도만 변화.','addJoint/rotateJoint → 22·23'],
      ['끝에서 처음, 같은 간격','총 경로 길이와 공통 거리 시계. 개별 offset을 더한다.','conveyorCornerPose/DistancePath/AnimationClock → 20'],
      ['기울어진 방/바닥 개방','내부 좌표계와 월드 좌표계 전환, released 상태부터 설계.','RoomBody/stepRoomBody/releaseBody → 21'],
      ['장애물/점프/착지','발 위치·반폭·수직속도·접지를 정의. 충돌 후 표시.','CharacterBody/jump/stepCharacter → 23'],
      ['같은 장면 여러 시점','단일 Scene과 한 update를 유지. 렌더만 분할.','RenderPass/threeViews/renderViews → 22'],
      ['카메라 쪽 벽 열기','target-position과 바깥 법선의 내적, 키 입력 시 선택 고정.','cameraFacingWall/openRoomWall → 23'],
      ['랜덤 장애물','시드는 실행 시, 엔진 수명은 리셋보다 길게. 제한 재추첨·비겹침.','randomFreeBox + mt19937 → 23'],
      ['그림자처럼 어둡게','요구가 방향광 명암인지 투영 그림자인지 구분한다.','RenderOptions.lighting / Shaders.h. cast shadow는 현재 없음.']]))
    body+=section('혼합·변형 문제 세 가지를 읽는 방법',
      '<h3>A. 점프하는 로봇을 세 카메라로 보여 주기</h3>'+flow(['23식 정적 로봇/장애물','발·충돌·접지','관절 보행','22식 pass 추가'])+para('충돌 상태를 카메라마다 따로 만들지 않는다. 세 화면이 동일한 feet와 관절을 읽게 한다.')+
      '<h3>B. 기울어지는 방 안의 컨베이어</h3>'+flow(['방 로컬 축','20식 거리 경로','방→월드 표시','개방 시 월드 낙하'])+para('경로 위치를 방 로컬로 계산했으면 방 회전은 한 번만 합성한다. 이탈 후에도 방 자식으로 남겨 다시 회전시키지 않는다.')+
      '<h3>C. 카메라 쪽 문을 열고 무작위 객체 생성</h3>'+flow(['카메라/방 법선','선택 벽 ID','개방 진행률','기존 객체와 비겹침 배치'])+para('문 선택·면 컬링·충돌 통과 판정은 세 책임이다. 문제에서 통과를 요구하면 표시만 축소하지 말고 해당 충돌 경계도 바꾼다. 현재23은 경계에서 돌아서는 규칙을 유지한다.'))
    body+=section('코딩 시작용 책임 골격',code('State: 입력/모드/시간/논리 위치/충돌체/Scene ID\ninitScene(): 정적 배치 + 관절 + 초기 camera + 초기 state\nonKey(): 입력을 state 명령으로 변환 (계산/렌더를 모두 넣지 않음)\nonUpdate(dt):\n    입력 의도 → 후보 위치/각도\n    경로·충돌·접지 해결\n    논리 상태 확정\n    Scene에 최종 transform 반영\nonRender(): 같은 Scene을 필요한 카메라별로 출력\nreset(): state와 ID를 함께 복구'))
    body+=section('막혔을 때 돌아갈 지점',table(['증상','돌아갈 행동'],[
      ['창도 안 뜸','1: DLL/경로/컨텍스트/셰이더 최초 오류. 객체 움직임 코드를 먼저 바꾸지 않는다.'],
      ['정적 객체 위치부터 틀림','2·3: 부모 좌표·피벗·T R S 순서. 애니메이션을 끄고 기준 자세를 본다.'],
      ['입력 한 번에 계속 토글','4: PRESS/REPEAT/RELEASE 구분.'],
      ['움직임이 카메라마다 달라짐','5·7: 시간 갱신이 render 안에 들어갔는지 확인.'],
      ['벽 통과/점프 순간 텔레포트','6: 반크기·직전/다음 높이·하강 착지·이동 하위 단계.'],
      ['면이 너무 어둡거나 사라짐','8: 명암 법선과 winding/컬링을 각각 확인.']]))
    body+=section('끝나기 전에 확인할 질문',para('요구의 각 동사를 하나의 상태와 함수로 연결했는가? reset이 모든 상태를 되돌리는가? 같은 좌표를 두 시스템이 덮어쓰는가? 광원과 카메라를 혼동했는가? 아직 구현하지 않은 것을 구현했다고 적지는 않았는가? 시험 시간 배분은 실제 제한 시간을 확인한 뒤 정한다.'))
    extra="""document.querySelectorAll('.requirement').forEach(e=>e.addEventListener('change',()=>{let chosen=[...document.querySelectorAll('.requirement:checked')].map(x=>x.value);let order=['read','boot','scene','hierarchy','input','motion','collision','views','lighting','finish'].filter(x=>['read','boot','scene','input','finish'].includes(x)||chosen.includes(x));document.getElementById('plan').textContent='권장 행동: '+order.map(x=>document.querySelector('#'+x+' h3').textContent).join(' → ');document.querySelectorAll('.action').forEach(x=>x.classList.toggle('active',order.includes(x.id)));}));"""
    page(OUT/'exam.html','중간고사 코딩테스트 · 요구사항에서 행동으로',body,extra=extra)

def main():
    all_sources=set();stats=[]
    for scope in SCOPES:
        graph,extraction=graph_data(scope)
        stats.append((scope,len(graph['nodes']),len(graph.get('links',graph.get('edges',[])))))
        for p in (OUT/scope/'graphify-out/SOURCE_HASHES.json',):
            all_sources.update(canon(scope,k).replace('\\','/') for k in json.loads(p.read_text(encoding='utf-8-sig')))
        page(OUT/scope/'pipeline.html',TITLES[scope]+' · '+PARTS['pipeline'],pipeline(scope),scope)
        page(OUT/scope/'ownership.html',TITLES[scope]+' · '+PARTS['ownership'],ownership(scope),scope)
        page(OUT/scope/'symbols.html',TITLES[scope]+' · '+PARTS['symbols'],symbols(scope,extraction,all_sources),scope)
        page(OUT/scope/'notes.html',TITLES[scope]+' · '+PARTS['notes'],notes(scope),scope)
    sources(all_sources);exam()
    body=para('각 대상의 3개 핵심 부분과 보충 설명을 별도 HTML로 나눴다. 코드·그래프의 현재 source location을 연결한다. 공통 framework를 정규 원본으로 읽고 SDK/복사본을 중복 분석하지 않는다.')
    body+='<div class="cards">'
    for scope,n,e in stats:
        body+='<section class="box"><h2>'+TITLES[scope]+'</h2>'+''.join('<p><a href="'+scope+'/'+key+'.html">'+value+'</a></p>' for key,value in PARTS.items())+'<p><a href="'+scope+'/graphify-out/graph.html">독립 graphify 그래프</a> · '+str(n)+'노드/'+str(e)+'간선</p></section>'
    body+='</div>'+section('시험장에서 무엇부터 할까?',para('<a href="exam.html">중간고사 코딩테스트 행동 그래프</a> — 요구사항 카드, 우선 행동 분기, 혼합·변형 예시, 책임 골격과 증상별 복귀 지점. 실습별 복사 순서가 아닌 일반 구현 전략이다.'))
    body+=section('그림자·명암',para('각 notes 페이지에서 방향광 명암/깊이 가림/Front culling/투영 그림자를 구분한다. 현재 셰이더에는 cast shadow 기법이 없다.'))
    body+=section('근거와 한계',para('code-only graphify AST + source-definition anchors, 외부 LLM/API 호출 0. 기존 query CLI는 UTF-8 BOM 입력을 읽지 못해 직접 UTF-8-SIG JSON 로딩으로 근거를 읽었다. 새 6개 scope 그래프를 각각 생성했다. 생성 그래프의 dangling endpoint 및 동일 노드쌍 간선 축약은 GRAPH_HEALTH.md에 남아 있으므로 완전한 동적 호출 그래프로 해석하지 않는다.')+
       table(['대상','근거/건강 보고서'],[[TITLES[s],'<a href="'+s+'/graphify-out/GRAPH_REPORT.md">GRAPH_REPORT</a> · <a href="'+s+'/graphify-out/GRAPH_HEALTH.md">GRAPH_HEALTH</a> · <a href="'+s+'/graphify-out/SOURCE_MANIFEST.json">소스 해시</a>'] for s in SCOPES]))
    body+=para('<a href="sources.html">전체 원문 줄 보기</a> · <a href="../../openGL_3dAssign_23/README.md">23 변경 설명</a>')
    page(OUT/'index.html','OpenGL 코드 학습 · 20~23 / framework / standard',body)
    write(OUT/'GUIDE_MANIFEST.json',json.dumps({'source_commit':subprocess.check_output(['git','rev-parse','HEAD'],cwd=ROOT,text=True).strip(),'scopes':stats,'html_lessons':27,'no_build_test_browser_execution':True,'query_cli':'BOM read failure; source-backed JSON fallback','graph_cost':'code-only; external LLM input/output tokens 0'},ensure_ascii=False,indent=2)+'\n')
    print('Authored 24 subject lessons + exam + sources + index = 27 HTML files, with six graphify outputs. No builds/tests/browser execution.')

if __name__=='__main__':main()
