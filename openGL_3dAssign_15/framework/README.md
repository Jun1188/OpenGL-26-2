# 재사용 가능한 OpenGL 프레임워크

Windows x64, C++17, OpenGL 3.3. framework 폴더 전체를 복사하고 `#include "framework/Framework.h"`를 사용한다. Framework.props는 include 경로와 실행 DLL·자산 배치를 제공한다. 프레임워크 구현 .cpp 등록과 외부 SDK 설정은 필요 없다.

## 변환

Transform3D는 basis(부모 좌표의 위치), localBasis(로컬 피벗), rotation(항등값 mat3), scale을 가진다. 행렬은 `T(basis) * R * S * T(-localBasis)`다. 쿼터니언·Euler 병행 표현은 없다. 로컬 자전은 rotation의 오른쪽에 회전행렬을 곱한다. 공전·부품 연결은 addGroup/setParent를 사용한다. 연결·해제는 로컬 값을 유지하며 월드 위치는 달라질 수 있다. 순환 계층은 거부한다.

초기 표시 기울기는 Camera의 position/target/up으로 지정한다. 모델 정점·로컬 기저·입력 회전에는 초기 표시 각도를 넣지 않는다. 렌더링·법선·선·피킹은 같은 계층 행렬을 사용한다.

## 공통 API

- Scene: addGroup, addCuboid, addPyramid, addRightTetrahedron, addSphere, addCone, addModel.
- DiagramSystem: makePolyhedron, makeSphere, makeCone. 구는 중심 원점, 원뿔은 Y축 중심이다. 크기는 양수, 구 slices 3~512/stacks 2~256, 원뿔 slices 3~512다. 잘못된 값은 빈 모델/빈 optional을 반환한다.
- setTransform/setPosition/setParent와 worldMatrix/faceWorldMatrix: 로컬·월드 변환.
- splitFace/setFaceSRT/setFaceLocalMatrix: 부모를 따르는 면 피벗과 추가 변환.
- setModel/setMesh/setMeshVertices/setVertexPosition/setTriangleIndices: 형상 편집.
- setTint: 자체 색상 배율. 자식과 선에는 전파하지 않으며 GPU 정점 버퍼를 다시 업로드하지 않는다.
- setMaterial/setTexcoords/generateTexcoords/transformTexcoords/setOutline: 재질·UV·경계선.
- addLine/addBasis: 그룹을 owner로 지정하면 선도 계층 변환을 따른다.
- addGroup은 보이지 않는 변환 노드다. 일반 addModel/push_entity는 빈 모델을 거부한다. setModel로 그룹에 유효한 모델을 지정하면 일반 엔티티가 된다. 그룹은 BottomCenter 피벗을 사용할 수 없다.

AssetLoader::loadPrimitive는 도형 텍스트·OBJ 부분집합, loadModel은 Assimp 정적 모델을 읽는다. models/textures는 ApplicationConfig.features에서 명시적으로 켠다. 뼈대 애니메이션은 제공하지 않는다. UV는 Planar/Box/Cylindrical/Spherical 투영을 지원한다.

와이어는 정점 색상과 tint를 유지한다. depthTest가 켜지면 전체 면 깊이를 먼저 기록하여 가려진 와이어를 제거한다. 솔리드 경계선은 outlineColor를 사용한다.

## 상대경로와 배포

ApplicationConfig.projectRoot 기본값 `.`은 실행 파일 폴더다. runtimeDirectory 기본값 `framework`와 선택적 shaderDirectory는 projectRoot 내부 상대경로다. 자산 인자는 projectRoot 내부 상대경로여야 한다. 절대경로·루트 이탈은 거부한다. 기본 셰이더는 내장되어 있다.

Framework.props는 DLL을 실행 폴더의 framework/dependencies에, 선택적 셰이더를 framework/shaders에 복사한다. data/models/textures는 실행 폴더의 같은 상대경로에 복사한다. 다른 작업 폴더에서 실행해도 소스 트리 탐색을 하지 않는다. 직접 배포할 때도 이 배치를 유지한다.

운영체제의 Windows SDK·OpenGL 드라이버·VC 런타임은 별도 실행 환경이다. 기존 실습용 LegacyOpenGL.props는 내부 GLEW/GLFW import library를 사용한다.

## 수명과 오류

Application은 한 스레드에서 실행 루프와 컨텍스트를 소유한다. getAssets는 run 콜백 동안 유효하다. 콜백 캡처 객체는 run 종료까지 유지한다. 예외는 C 콜백 밖으로 직접 전달하지 않고 정리 후 다시 전달한다. Renderer 직접 사용 시 current 컨텍스트에서 initialize/render/destroy를 호출하고 컨텍스트를 마지막에 해제한다.

setter의 false와 로더의 error를 확인한다. clear 뒤에는 ID가 재사용되므로 보관한 ID를 폐기한다. getComponent/getMesh 포인터는 장면 변경 뒤 다시 조회한다.

[응용 패턴](docs/APPLICATION_PATTERNS.md), [사용자 확인](docs/USER_CHECKS.md). 빌드·테스트는 사용자가 수행한다. 이전 변경 보고는 저장소의 framework-analysis/archive에 보관한다.

## 13. 변환·Motion·레이어 트리 (2026-10-22)

이 절은 공통 프레임워크에 추가한 API를 설명한다. 기존 실습의 Console 애니메이션은 유지한다. 새 API를 사용하지 않으면 Motion과 레이어는 항등변환이다. 카메라는 Scene 객체와 독립적이며 루트 레이어로 이동하지 않는다. 이 절과 예제는 빌드·실행하지 않았다.

### 13.1 구성과 프레임 순서

- `include/Transform.h`, `detail/Transform.inl`: 반사·회전·이동·크기 행렬 및 경로 함수.
- `include/Motion.h`, `detail/Motion.inl`: 시간에 따른 행렬 합성, 동작 식별자, 레이어 트리.
- `Scene::getMotions()`: 해당 장면의 MotionSystem 참조. `Framework.h` 또는 `Graphics.h`로 공개된다.
- `examples/motion.cpp`: 경로 선택, 공유 동작, 반사, 레이어 삽입·삭제, 클릭 피킹 예제. 다른 예제와 main이 중복되므로 하나만 프로젝트에 등록한다.

현재 프레임 순서는 `PollEvents -> 사용자 update 콜백 -> 종료 확인 -> Scene::updateMotions(dt) -> render 콜백/기본 렌더 -> SwapBuffers`다. Application::run 안에서는 updateMotions를 다시 호출하지 않는다. 콜백에서 만든 동작은 그 프레임의 dt만큼 진행된 뒤 렌더된다. run 밖에서 Scene과 Renderer를 직접 사용하면 호출자가 한 프레임에 한 번 updateMotions를 호출한다. 기존 dt의 0~0.05초 제한과 최소화 대기 처리는 유지한다. 실제 벽시계가 아니라 전달받은 dt의 누적 시간이다.

### 13.2 기본 행렬과 반사

```cpp
const auto rotate = graphics::TransformSystem::rotation(
    graphics::AxisLine{{1, 0, 0}, {0, 1, 0}}, 30.0);
const auto mirror = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
const auto pointSymmetry = graphics::TransformSystem::reflectPoint({1, 0, 0});
const auto lineSymmetry = graphics::TransformSystem::reflectLine(
    graphics::AxisLine{{0, 0, 0}, {0, 1, 0}});
```

반사는 회전 API와 분리한다. rotation은 축 위의 한 점과 축 방향을 받으며 양의 각도는 오른손 방향이다. 축 방향을 반대로 지정하면 같은 양의 각도도 반대 방향이다. 각도 단위는 도, 속도는 도/초다.

- `translation(offset)`: 이동 행렬.
- `scaling(factors, center)`: 지정 중심의 크기 변경. 음수와 0 배율도 허용한다.
- `rotation(axis, degrees)`: 지정 직선 주위의 회전.
- `reflectPlane(point, normal)`: 평면 양쪽의 거리를 뒤집는 평면 반사. determinant는 -1이다.
- `reflectPoint(center)`: 중심을 기준으로 세 좌표를 반전하는 점대칭. 3D에서는 determinant가 -1이다.
- `reflectLine(axis)`: 직선 방향 성분은 보존하고 수직 성분 둘을 뒤집는다. 3D에서 같은 직선 주위의 180도 회전과 수학적으로 같지만 별도 대칭 API로 제공한다.

기본식은 `T(center) * A * T(-center)`다. 평면 반사에서 A는 I-2nnᵀ, 직선 대칭에서는 2nnᵀ-I다. 방향 벡터는 내부에서 정규화한다. 영벡터·NaN·무한대나 계산 결과가 유효하지 않으면 optional이 비어 있다. 행렬은 유한한 아핀 행렬이어야 하며 투영 행렬은 허용하지 않는다.

기존 Transform3D.rotation은 순수 회전 조건을 유지한다. 반사 행렬을 이 필드에 억지로 넣지 않고 LayerConfig.transform 또는 Motion::fixed에 넣는다. Renderer는 면 컬링을 끄고 역전치 법선을 사용하며, 피킹은 양면 교차 검사와 같은 faceWorldMatrix를 사용한다. 0 배율로 붕괴한 면과 특이 행렬의 조명은 일반적인 입체 표면 결과를 보장하지 않는다.

### 13.3 경로 함수와 시간 보간

```cpp
graphics::PathMotion path;
path.path = graphics::PathSystem::ellipse({0, 0, 0}, {2, 0, 0}, {0, 0, 1});
path.interpolation = graphics::PathSystem::smoothStep;
path.durationSeconds = 4;
path.playback = graphics::Playback::Loop;
graphics::Motion motion;
if (!motion.append(graphics::Motion::followPath(path))) return false;
const auto id = scene.getMotions().createMotion(motion);
```

`PathFunction`은 `glm::vec3(double u)`, `InterpolationFunction`은 `double(double progress)` 형태의 std::function이다. 내부 흐름은 다음과 같다.

```text
q = phase + 경과초 / durationSeconds * rate
progress = Once의 clamp / Loop의 [0,1) 순환 / PingPong의 0↔1 왕복
u = interpolation(progress)
offset = path(u)
relativeToStart이면 offset -= path(interpolation(phase))
결과 = T(offset)
```

path의 반환값은 객체의 기존 위치를 직접 대입하는 절대 월드 좌표가 아니라 추가 이동량이다. 기존 배치가 원점이면 경로 좌표와 결과 위치가 일치한다. relativeToStart=true는 시작 위치를 보존하는 상대 이동에 사용한다. World/Local 공간에 따라 이 이동을 배치 앞/뒤에 적용한다. 객체가 경로 접선 방향을 자동으로 바라보지는 않는다. 방향 변화가 필요하면 별도 회전 Step 또는 사용자 행렬 함수를 합성한다.

| 설정 | 계약 |
|---|---|
| durationSeconds | 양의 유한값. rate=1일 때 경로 한 번의 진행 시간 |
| rate | 0이 아닌 유한값. 부호로 진행 방향 지정 |
| phase | 0~1의 시작 진행률. Once를 역방향으로 시작하려면 phase=1, rate<0 |
| playback | Once는 끝에서 유지, Loop는 순환, PingPong은 왕복 |
| relativeToStart | 시작 경로 위치를 차감할지 여부. 기본 false |
| space | World 또는 Local. 기본 World |

순환에서 열린 경로는 끝과 시작 사이에 점프한다. 보간 함수는 원래 시간 진행률 0~1을 받는다. 반환 u는 제한하지 않으므로 사용자 경로는 필요한 구간 밖 값도 처리해야 한다. 내장 polyline은 u를 0~1로 제한하고, ellipse는 주기 함수로 평가한다. 함수는 유한한 결과를 반환해야 한다.

PathSystem::ellipse(center, axisU, axisV)의 두 벡터는 반지름과 평면을 함께 정한다. 서로 평행하거나 길이가 0인 벡터는 거부한다. circle(center, radius, normal)은 법선에 수직인 원을 만든다. rectangle의 halfU/halfV는 수직인 반변 벡터다. polygon은 입력 순서대로 닫힌 경계를, polyline(points, closed=false)은 입력 순서대로 선분을 만든다. 연속 중복점·길이 0 선분은 거부한다. 닫힌 polygon은 마지막 점을 다시 적어도 된다.

polyline/polygon/rectangle은 선분 길이 누적으로 매개변수를 배분하므로 linear 보간에서 실제 이동 속도가 일정하다. 꼭짓점에서는 방향이 즉시 바뀐다. ellipse와 임의 사용자 경로는 매개변수 속도가 일정해도 실제 속도가 일정하지 않을 수 있다. smoothStep을 적용하면 선분 경로도 가감속한다. 일반 영역의 경계·내부 궤적은 사용자의 path 함수가 결정하며 영역만으로 경로를 자동 추론하지 않는다.

### 13.4 Motion 합성과 편의 API

Motion은 MotionStep의 목록이다. append/insert(index, step)/replace(index, step)/erase(index)로 편집하고 getSteps로 읽는다. 변경한 Motion을 setMotion(id, motion)에 다시 전달한다. 인덱스는 삽입·삭제 후 달라진다.

각 Step은 transform(elapsedSeconds), space, startSeconds, 선택적 durationSeconds, enabled를 가진다. 시작 전에는 기여하지 않고, 종료 시각 이후에는 마지막 행렬을 유지한다. 사용자 transform 콜백은 아핀 행렬을 반환한다. 같은 공간 안에서는 API로 추가한 순서대로 적용한다. 예를 들어 A 다음 B는 B*A다. World와 Local은 별도 목록으로 합성하므로 서로 다른 공간의 추가 순서로 두 공간을 교차 배치하지 않는다. 공간을 넘는 계층은 레이어로 표현한다.

```cpp
graphics::Motion combo;
graphics::RotationMotion orbit;
orbit.degreesPerSecond = -20;
orbit.axis = {{0, 0, 0}, {0, 1, 0}};
orbit.space = graphics::TransformSpace::World;
if (!combo.append(graphics::Motion::rotation(orbit))) return false;
const auto reflect = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
if (!reflect || !combo.append(graphics::Motion::fixed(*reflect))) return false;
```

append로 넣은 Step들은 각자의 startSeconds 기준으로 동시에 평가한다. then(step)은 앞선 모든 Step의 종료 시각 뒤에 배치한다. 앞선 Step 중 무기한 회전·반복 경로가 있으면 false다. 시간 순서 실행은 이전 변환을 삭제하거나 대체하지 않고 종료 행렬을 유지하며 다음 변환을 합성한다.

```cpp
graphics::Motion sequential;
graphics::MotionStep x;
x.durationSeconds = 1;
x.transform = [](double t) { return glm::translate(glm::mat4(1), glm::vec3(float(t), 0, 0)); };
graphics::MotionStep y;
y.durationSeconds = 1;
y.transform = [](double t) { return glm::translate(glm::mat4(1), glm::vec3(0, float(t), 0)); };
if (!sequential.append(x) || !sequential.then(y)) return false;
// t=0.5: (0.5,0,0), t=1.5: (1,0.5,0), t>=2: (1,1,0)의 추가 이동.
```

MotionSystem의 편의 생성 API는 기본 Step 합성을 재사용한다.

| API | 생성 결과 |
|---|---|
| createMotion(motion) | 사용자 조합 |
| createSpin(rotation) | space를 Local로 지정한 자전 |
| createOrbit(rotation) | space를 World로 지정한 공전 |
| createOrbitAndSpin(orbit, spin) | World 공전과 Local 자전의 독립 축·각속도 |
| createPathAndSpin(path, spin) | 사용자 경로의 이동과 Local 자전 |

createSpin의 기본 축은 모델 원점을 지나는 Y축이다. 모델의 localBasis를 중심으로 자전하려면 axis.point에도 같은 모델 좌표를 지정한다. createOrbit은 기존 모델 배치 전체를 지정 월드 직선 주위로 회전시킨다.

### 13.5 식별자와 재생 상태

- createMotion은 optional<MotionId>를 반환한다. ID는 MotionSystem 인스턴스에 속한다.
- getMotion은 const MotionState*를 반환한다. motion, timeSeconds, playing, enabled, sampled를 조회한다.
- setMotion은 설정을 교체하되 현재 시각·재생·활성 상태를 유지한다. 처음부터 시작하려면 seekMotion(id, 0)을 호출한다.
- setMotionPlaying(false)는 시계를 멈추고 마지막 변환을 유지한다. true는 그 시각부터 재개한다.
- setMotionEnabled(false)는 변환의 기여만 항등행렬로 바꾼다. playing=true이면 보이지 않는 동안에도 시간은 흐른다. 다시 활성화하면 현재 시각의 변환이 나타난다.
- seekMotion은 0 이상의 유한한 시각으로 이동하고 즉시 샘플을 바꾼다.
- removeMotion은 관련 레이어에서 그 동작 바인딩도 해제한다. 레이어의 고정 transform은 유지한다.
- update는 재생 중인 각 Motion을 한 번 평가한다. 같은 ID를 여러 레이어가 공유해도 시계가 여러 번 진행하지 않는다.
- Motion·Layer ID는 삭제나 clear 후에도 재사용하지 않는다. rootLayer()의 0만 장면 전체 루트로 유지한다. EntityId는 기존처럼 Scene::clear 후 재사용되므로 새로 받는다.

생성·설정·seek는 해당 시각의 콜백을 실행할 수 있다. bool/optional 실패는 식별자·매개변수 또는 행렬이 유효하지 않다는 뜻이다. 사용자 콜백 예외와 내장 경로의 비유한 결과 예외는 호출자에게 전파된다. Application::run은 기존 예외 정리 경로로 GPU·창 자원을 정리하고 재전파한다. update는 모든 샘플을 준비한 뒤 시계와 캐시를 반영하므로 중간 실패 시 일부 Motion만 시간이 진행하지 않는다. 콜백의 외부 부작용까지 되돌리지는 않는다.

콜백은 결정적 계산 함수로 사용하고, 평가 중 Scene·MotionSystem을 수정하거나 재진입하지 않는다. 캡처한 객체의 수명은 동작이 삭제될 때까지 유지한다. getMotion/getLayer/getComponent의 포인터를 수정 API 호출 전후로 장기 보관하지 않는다. API는 기존 Scene처럼 단일 렌더링 스레드에서 사용한다.

### 13.6 레이어 트리와 객체 바인딩

```cpp
auto& motions = scene.getMotions();
graphics::RotationMotion spin;
const auto spinId = motions.createSpin(spin);
const auto groupLayer = motions.pushLayer(graphics::MotionSystem::rootLayer());
if (!spinId || !groupLayer || !motions.bindMotion(*groupLayer, *spinId)) return false;
if (!scene.bindEntity(firstEntity, *groupLayer) || !scene.bindEntity(secondEntity, *groupLayer)) return false;
```

이 코드는 같은 자전 시각·파라미터를 공유한다. 각 객체에 직접 바인딩하면 각 객체의 모델 좌표에서 자전한다. 객체들의 공통 중심을 기준으로 함께 돌리려면 World 공전을 사용하거나, Scene::addGroup/setParent로 조립한 부모 객체에 Local 자전 레이어를 바인딩한다.

레이어는 root -> branch -> leaf 형태의 트리다. 각 루트-말단 경로가 하나의 변환 스택이며 형제 가지는 서로 영향을 주지 않는다. root의 고정 변환과 Motion은 미바인딩 객체·소유자 없는 선을 포함한 전체 Scene에 한 번 적용한다. LayerConfig는 transform, space, 선택적 motion, enabled를 가진다. 고정 transform과 Motion이 함께 있으면 같은 공간에서 고정 변환 다음 Motion을 적용한다. enabled=false는 그 레이어의 기여만 생략하며 자식과 공유 Motion 시계는 유지한다.

| API | 효과와 제한 |
|---|---|
| pushLayer(parent, config) | 부모의 마지막 자식으로 추가. 반환 ID를 다음 push의 부모로 쓰면 stack push |
| insertLayer(child, config) | 기존 child와 부모 사이에 삽입. child ID·객체 바인딩 보존 |
| popLayer(top) | 자식 없는 말단만 제거하고 부모 ID 반환. 루트·분기는 거부 |
| removeLayer(id) | 해당 노드만 제거. 자식은 기존 형제 위치에 부모로 승격. 루트 삭제 거부 |
| reparentLayer(id, parent) | 가지를 새 부모의 마지막 자식으로 이동. 자기 자신·자손 밑 연결 거부 |
| setLayer(id, config) | 고정 변환·공간·동작·활성 상태 수정. 트리 연결 보존 |
| getLayer(id) | const LayerState*로 설정·부모·자식 조회 |
| bindMotion(layer, id/nullopt) | Motion 연결/해제. 레이어당 하나이며 여러 Step은 Motion 안에서 합성 |
| Scene::bindEntity(entity, layer) | 객체당 말단 하나 연결. 기존 바인딩 교체 |
| Scene::unbindEntity(entity) | 직접 바인딩 해제. 부모의 바인딩 또는 루트로 복귀 |
| Scene::getEntityLayer(entity) | 직접 바인딩된 유효 ID만 반환. 상속된 ID는 반환하지 않음 |

같은 Scene 객체의 서로 다른 자식들을 독립시키려면 각자 다른 말단에 연결한다. 객체에 직접 연결이 없으면 가장 가까운 객체 조상의 바인딩을 사용한다. 직접 연결이 있으면 상속된 가지를 대체한다. 부모·자식에서 바인딩을 중복 누적하지 않는다. 부모와 자식의 효과를 모두 원하면 자식의 레이어 가지가 원하는 상위 레이어를 포함하도록 구성한다. 객체 부모 트리와 레이어 트리 사이에 포인터나 같은 모양을 강제하지 않는다.

삭제된 레이어의 직접 바인딩은 조회 시 무효 처리한다. 해당 객체는 가까운 객체 조상 바인딩 또는 루트로 복귀한다. 승격된 자식 레이어의 바인딩은 보존한다. reparent/remove는 월드 자세를 보존하는 역행렬 보정을 하지 않으므로 새 합성 결과로 바뀐다.

최종 행렬은 다음과 같다.

```text
P = 루트의 전역 변환 * 가지의 World 변환들
L = 가지의 Local 변환들
A = 객체 부모 계층의 기존 SRT 중 바인딩된 객체까지
D = 바인딩된 객체 아래 자손의 기존 SRT
M_entity = P * A * L * D
M_face = M_entity * 면의 기존 추가 SRT * localAnimationMatrix
```

직접 바인딩된 객체는 D=I다. 상속이면 Local 변환은 바인딩한 조상 위치에 들어가므로 그 그룹 전체가 함께 움직인다. 가지 내부는 루트에서 말단 순서로 행렬을 오른쪽에 쌓는다. 루트의 Local 기여도 장면 전체 변환으로 처리한다. World는 객체의 Scene 부모 좌표가 아니라 기존 객체 배치 전체의 바깥쪽, 즉 월드 측 공간이다.

소유자가 있는 선은 owner의 M_entity를, 소유자가 없는 선은 루트의 전역 행렬만 사용한다. 색·표시 여부·카메라·메시 정점은 이 레이어에서 수정하지 않는다. 월드 행렬을 조회할 때 객체·레이어의 깊이에 따라 CPU 순회와 임시 저장이 필요하다. 성능 측정은 하지 않았다.

실행 확인 절차는 [USER_CHECKS.md](docs/USER_CHECKS.md)의 Motion 절, 조립 예시는 [APPLICATION_PATTERNS.md](docs/APPLICATION_PATTERNS.md)의 추가 절을 참고한다.

