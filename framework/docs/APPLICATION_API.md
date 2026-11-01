# 응용 API: 입력에서 화면까지

진입점은 `framework/Framework.h`다. C++17, Windows x64, OpenGL 3.3 core 기준이다. 빌드·실행·테스트는 사용자가 수행한다. 실습 23 구현은 보류다. 관절·캐릭터 API만 공동 개발을 위해 준비했다.

## 진단

`ApplicationConfig::diagnostics={true,false}`는 오류 출력 켜기, 단계 추적 끄기다. 실행 중 `Diagnostics::configure({true,true})`로 추적을 켠다. `lastError()`는 마지막 오류의 단계·API·원인·조치·객체 ID를 반환한다. `clear()`로 지운다.

Runtime → Input/Scene/Motion → VertexInput → Shader/Transform → Raster → Framebuffer를 구분한다. CPU 입력 거부, DLL 경로/Win32 오류, GLFW 오류 콜백, 셰이더 컴파일·링크 로그, GPU 업로드·드로우·뷰포트 오류를 출력한다. 기존 bool/optional 계약을 유지한다. `Rejected condition`은 실제 거부 조건식이다. Scene::clear 이후 ID는 다시 받아야 한다.

유효한 GL 호출도 잘못된 카메라·클리핑·가림으로 보이지 않을 수 있다. GPU 내부 단계의 결과를 직접 읽는 기능은 아니다. 드로우 오류는 앞선 바인딩·uniform 오류도 포함할 수 있다. 메시지를 단일 원인의 확정으로 해석하지 않는다. 피킹 빗나감·그리드 밖 조회·공중 점프 거절은 정상적인 미일치다.

## GridLayout

origin은 로컬 최소 모서리, extent는 양수 전체 크기, cells는 양수 셀 개수, frame은 로컬→월드의 가역 affine 행렬이다.

```cpp
graphics::GridLayout grid;
grid.origin={-5,0,-5};grid.extent={10,4,10};grid.cells={10,4,10};
auto p=grid.position({.5f,0,.5f}); // 월드 바닥 중앙 (0,0,0)
auto c=grid.cellCenter({2,0,3});   // (-2.5,.5,-1.5)
auto s=grid.relativeSize({.1f,.25f,.1f}); // 로컬 크기 (1,1,1)
auto q=grid.positionInParent({.5f,0,.5f},parentWorld);
auto i=grid.cellAt(worldPoint);
```

position 비율은 [0,1], relativeSize 비율은 양수다. 크기는 로컬 길이이며 frame의 회전/배율을 포함하지 않는다. cellAt은 최대 경계를 마지막 셀로 처리하고 영역 밖은 빈 optional이다. 부모가 있는 Scene 객체의 setPosition에는 부모 로컬 위치를 전달한다.

## 도형·입력·조립

| 목적 | API·계약 |
|---|---|
| 중심 기준 상자 | `PatternSystem::addBox(scene,size,position,color,parent)` |
| 평면 도형·바닥·벨트 | `addPolygon(scene,points,color,parent)`; 동일 평면의 볼록 다각형, 순서대로 전달, 삼각형 fan |
| 내부 공간 | `addRoom(scene,bounds,frontOpen)`; 각 면 색·안쪽 법선·앞면 컬링. face 0 앞(+Z), 1 뒤, 2 왼쪽, 3 오른쪽, 4 위, 5 바닥 |
| 관절 | `addJoint(scene,parent,anchor)` + `rotateJoint(scene,joint,axis,degrees)`; 연결 위치와 표시 크기 분리 |
| 숫자 | `addNumber(scene,digit,position,height,color,parent)`; 0~9 선분, 부모 로컬 XY 평면, +Z 쪽에서 읽음 |
| 면 접기 | `foldFace(scene,id,face,hinge,degrees)`; 원본 로컬 좌표의 피벗 회전 |
| 문 | `openFace(scene,id,face,progress,displacement,topPivot,scaleOpen)`; 진행률 [0,1], 이동 또는 상단 기준 Y축 축소. 완전 축소는 숨김. 충돌 개방 상태는 앱에서 함께 변경 |
| 모양 교환 | `swapModels(scene,first,second)`; 위치·ID 유지, 모델 교환 |
| 셀 경로 | `snakeCells(cells)`; Y=1 XZ 그리드, 최대 100만 셀 |
| 높이 일치 | `aligned(first,second,tolerance)`; 정지·적층 상태는 앱에서 결정 |
| 드래그 | `pick` + `screenOnPlane(camera,x,y,viewport,planePoint,normal)` + `setPosition` |
| 스냅 | `snap(point,spacing)`; 양수 간격. 실패 시 원점이 아닌 원래 점을 반환하고 진단 |

addPolygon은 오목 다각형·구멍을 삼각분할하지 않는다. 숫자 선분은 폰트 렌더러가 아니다. 생성 도중 메모리 예외의 장면 롤백은 제공하지 않는다. 피킹은 위쪽 원점 framebuffer 좌표이므로 창 좌표에 DPI 배율을 적용한다.

## 거리·모션

DistancePath::setPoints(points,false)는 열린 꺾은선이다. length()는 전체 거리다. sample(distance,Playback::Loop)는 위치·단위 접선·XZ Y 회전각을 반환한다. Once는 끝에 고정하고 PingPong은 왕복한다. Loop는 끝에서 시작으로 순간 이동하며 복귀 선분을 추가하지 않는다.

```cpp
graphics::DistancePath path;
path.setPoints({{-3,1,0},{2,1,0},{2,1,3}});
graphics::AnimationClock clock;
// onUpdate에서 공통 시간을 한 번만 갱신
clock.advance(dt);
graphics::PatternSystem::follow(scene,id,path,clock.seconds*speed+offsetDistance);
```

follow는 기존 크기·피벗을 유지하고 위치 및 선택적으로 Y 회전을 적용한다. XZ 경로·정면 +Z 모델 기준이다. offset 인자는 위치 오프셋이다. AnimationClock::playing=false면 시간이 유지된다. 상자와 줄무늬에 동일 시간과 서로 다른 거리 offset을 사용한다. 기존 Motion/PathMotion/레이어 API를 계속 재사용한다. Application::run이 Scene 모션을 자동 갱신하므로 updateMotions를 중복 호출하지 않는다.

## 충돌·낙하

Bounds3는 축 정렬 최소·최대 경계다. contains(point,margin)은 반크기를 포함한 내부 판정, overlaps는 양의 부피 겹침이다. 맞닿기만 한 상자는 겹침이 아니다.

`CollisionSystem::stepRoomBody(body,room,roomWorld,dt,floorOpen,elastic,gravity,angularVelocity)`:

- roomWorld는 회전·이동만 허용한다. dt는 [0,.25], halfExtent는 양수다.
- elastic=true: 방 안에서는 중력 없는 완전 탄성 반사. 반크기를 포함한 중심 경계에 축별 거울 접기를 적용한다.
- elastic=false: 월드 중력을 방 로컬로 바꾸어 최대 1/120초 간격으로 적분하고 벽 침투를 보정한다.
- 열린 바닥 아래로 나가면 released=true가 되고 월드 낙하로 전환한다. 회전 방의 각속도는 이탈 속도에 반영한다. 바닥을 닫아도 자동 복귀하지 않는다.
- body.position(roomWorld)가 표시할 월드 위치다. 미이탈 상자의 표시 회전은 roomWorld의 회전이다.
- 방 축에 정렬된 상자의 교육용 운동이다. 회전 방의 코리올리/원심 가속도·마찰·강체 토크·공끼리 충돌은 구현하지 않는다. 이탈 후 착지는 이 API의 범위 밖이다.

jump(CharacterBody&,speed)는 접지 중에만 시작한다. stepCharacter(body,stage,obstacles,dt,gravity)는 수평 장애물·가장자리 반전, 중력, 장애물 윗면 착지·지지면 이탈을 계산하고 CharacterStep을 반환한다. feet는 발 위치, halfWidth는 XZ 반폭이다. 폭·속도에 따라 하위 단계를 늘리며 4096단계 초과 시 상태를 보존하고 실패한다. 회전 팔다리의 정밀 충돌은 아니다.

randomFreeBox(area,size,occupied,rng,attempts)는 제한 횟수 안에서 비겹침 배치를 찾는다. 바닥 배치는 area의 Y 두께를 size.y와 같게 준다. 실패 optional을 처리한다.

addWalkingRig(scene,bodyGrid)는 머리·코·몸통·팔다리 조립 예제다. bodyGrid.origin=(-폭/2,0,-깊이/2)로 중앙·발을 기준으로 둔다. poseWalkingRig(scene,rig,feet,yawDegrees,walkDistance,speed)는 이동 거리 기반 반대 위상 보행과 속도별 진폭을 반영한다. 두 API는 공통 기능이며 실습 23 구현은 보류다.

## 카메라·렌더 패스

moveCamera(camera,offset)는 시점·주목점을 함께 이동한다. yawCamera는 현재 시점·로컬 up 기준 자전, orbitCamera(camera,axis,degrees)는 지정 월드 축의 공전이다. zoomCamera(camera,factor)는 원근에서 거리, 직각에서 orthographicHalfExtent를 바꾼다. factor<1은 확대다. 변경 후보가 유효할 때만 반영한다.

RenderPass에는 camera/viewport/options/clearColor/clearDepth가 있다. viewport는 아래쪽 원점 framebuffer 픽셀이다. 패스 크기로 aspect를 계산하고 scissor 영역 안에서만 버퍼를 지운다.

```cpp
const auto size=app.getFramebufferSize();
if(size[0]>=3 && size[1]>=2)
    return app.renderViews(graphics::PatternSystem::threeViews(
        app.getCamera(),size[0],size[1],app.getRenderOptions()));
return true;
```

threeViews는 왼쪽 원근, 오른쪽 위 XZ 평면도(up=-Z), 오른쪽 아래 XY 정면도다. 업데이트는 한 번, 렌더만 세 번 한다. 임의 RenderPass 배열도 지원한다. 분할 피킹은 해당 카메라와 위쪽 원점 PickViewport를 사용한다.

RenderOptions::cull은 패스 기본 정책, Mesh::cull 또는 Scene::setFaceCullMode는 면별 정책이다. None/Back/Front/Inherit를 지원하고 음수 배율의 winding을 보정한다. 실내벽은 Front, 일반 물체는 기본 None 또는 Back이다.

## 실습 주요 패턴 대응

| 실습 | 응용 API 조합 |
|---|---|
| 1~6 | addPolygon/addBox, setTint/setModel, translation/scaling/rotation, Motion 시간 기반 이동 |
| 7~9 | pick/screenOnPlane, outline/가시성, PathMotion, 반사·스냅 |
| 10 | 다각형 생성 → pick/드래그 → snap → bounds 판정 → 완성 상태 고정 |
| 11 | GridLayout → snakeCells → 셀 경로 → overlaps → swapModels |
| 12 | PingPong → aligned → Motion::then/Once → GridLayout 적층 |
| 13~14 | setFaceVisible/setFaceVertexColors, setTransform, createSpin |
| 15~16 | foldFace, 피벗 scaling, addJoint, Motion::then 순차 전개 |
| 17 | createSpin/createOrbit/createOrbitAndSpin/createPathAndSpin, setSideLengths |
| 18 | addJoint/setParent, 타원 경로, orbit/spin 레이어, addLine 궤도 |
| 19 | addBox/addJoint, rotateJoint, Motion/PathMotion, 카메라 조작 |
| 20 | DistancePath/AnimationClock/follow, addPolygon/addBox/addNumber |
| 21 | addRoom/addSphere, RoomBody/stepRoomBody, openFace/setFaceVisible |
| 22 | 기존 탱크 Scene + threeViews/renderViews |
| 23 준비 | GridLayout/JointRig/CharacterBody/jump/stepCharacter/randomFreeBox/openFace |

키·상태 전환·게임 종료·완료 판정은 앱이 소유한다. 옛 실습 전체의 소스 이식과 실습 23 구현은 하지 않았다. 기존 APPLICATION_PATTERNS.md는 유지한다.

## 사용자 확인

standard.sln에서 Debug/Release x64를 각각 빌드한다. 0 기존 모션, 7 Grid, 8 실내 예제, V 3뷰, D 추적 로그, F 바닥 개방을 확인한다. 별도 복사본에서 잘못된 EntityId·카메라 up·셰이더 문법을 넣어 오류 단계와 API를 확인한다. 리사이즈·최소화·DPI·분할 피킹·종료와 다른 경로/PC에서의 DLL 배치를 확인한다.

에이전트는 빌드·실행·테스트·검증기를 실행하지 않았다. 소스·문서를 읽은 상태이며 실제 화면과 호환성은 미확인이다.

## 런타임 동봉

GLFW DLL의 정적 import table에서 VCRUNTIME140.dll 의존성을 확인했다. 정식 VS Redist의 x64 VCRUNTIME140.dll과 원본 Microsoft 라이선스를 dependencies/glfw-3.4.bin.WIN64 아래에 동봉했다. DLL은 GLFW 바로 옆 lib-vc2022 폴더에 배치된다. Windows 10/11 x64의 UCRT·시스템 DLL·OpenGL 드라이버는 OS에서 제공한다. 설치·실행 확인은 사용자 담당이다.
