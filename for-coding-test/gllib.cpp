// ================================================================================
// OpenGL 3.3 시험용 프레임워크 — 단일 C++ 소스
// ================================================================================
// 의존성 : GLEW 헤더(이 파일에 필요한 함수 로더 내장), GLFW 헤더·glfw3dll.lib·glfw3.dll, GLM 헤더,
//          Windows SDK (opengl32.lib)
// 환경   : C++17 이상, Windows x64, OpenGL 3.3 Core, UTF-8 BOM
//
// [파일 구조 — 전방 선언 / 후방 정의 분리]
//   1부. 헤더 포함 · 자동 링크 · 플랫폼 가드
//   2부. 전방 선언부 (Forward Declarations) — 모든 객체·열거형·함수의 역할 주석(VS 호버 지원)
//   3부. 인터페이스 선언부 (Type & Class Declarations) — 멤버·API 시그니처만 선언
//   4부. 후방 정의부 (Implementations) — 메서드·로더·모션·장면·렌더러·앱 구현
//   5부. 시험장 가이드 · 기본 도형 OBJ 목록 · lecture_pdf 빠른 참고 색인
//   6부. 시험 코드 (#ifndef GLLIB_NO_MAIN) — Console 및 runExam()
// 자전·공전·계층 함수는 EXAM_TRANSFORMS, 호출 예제는 EXAM_TRANSFORM_PATTERNS를 검색한다.
// 준비 PC 설치: EXAM_INSTALL. 사전 학습용 추가 PDF: EXAM_PDF_REVIEW. 현장 SDK 경로 확인: EXAM_OFFLINE.
/*
EXAM_INSTALL — 시험장 초기 의존성 설치 (PowerShell 5.1 / 7)
gllib.cpp가 있는 프로젝트 폴더에서 아래 & {부터 마지막 }까지 복사해 실행한다.
인터넷이 열려 있을 때 설치를 마치고 Debug | x64에서 기본 장면을 확인한다.

& {
    $ErrorActionPreference = 'Stop'
    $ProgressPreference = 'SilentlyContinue'
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    if (!(Test-Path -LiteralPath 'gllib.cpp' -PathType Leaf)) {
        throw 'gllib.cpp가 있는 프로젝트 폴더에서 실행하세요. (VS: 프로젝트 우클릭 -> 터미널에서 열기)'
    }

    $req = @(
        'dependencies\glew-2.3.1\include\GL\glew.h',
        'dependencies\glfw-3.4.bin.WIN64\include\GLFW\glfw3.h',
        'dependencies\glfw-3.4.bin.WIN64\lib-vc2022\glfw3dll.lib',
        'dependencies\glm\glm\glm.hpp',
        'dependencies\dll\glfw3.dll'
    )
    if (@($req | Where-Object { !(Test-Path -LiteralPath $_ -PathType Leaf) }).Count -gt 0) {
        $null = New-Item -ItemType Directory -Force -Path 'dependencies\dll'
        $tmp = Join-Path $env:TEMP ('gl_deps_' + [guid]::NewGuid().ToString('N'))
        $null = New-Item -ItemType Directory -Force -Path $tmp
        try {
            $packages = [ordered]@{
                'glew' = 'https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1-win32.zip'
                'glfw' = 'https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip'
                'glm'  = 'https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip'
            }
            foreach ($name in $packages.Keys) {
                $zip = Join-Path $tmp "$name.zip"
                Invoke-WebRequest -UseBasicParsing -Uri $packages[$name] -OutFile $zip
                if ($name -eq 'glm') {
                    $stage = Join-Path $tmp 'glm'
                    Expand-Archive -LiteralPath $zip -DestinationPath $stage -Force
                    $glmHpp = Get-ChildItem -LiteralPath $stage -Recurse -File -Filter 'glm.hpp' |
                        Where-Object { $_.Directory.Name -eq 'glm' } | Select-Object -First 1
                    if (!$glmHpp) { throw 'GLM 압축에서 glm/glm.hpp를 찾지 못했습니다.' }
                    if (Test-Path -LiteralPath 'dependencies\glm') { Remove-Item -LiteralPath 'dependencies\glm' -Recurse -Force }
                    $null = New-Item -ItemType Directory -Force -Path 'dependencies\glm'
                    Copy-Item -LiteralPath $glmHpp.Directory.FullName -Destination 'dependencies\glm\glm' -Recurse -Force
                } else {
                    Expand-Archive -LiteralPath $zip -DestinationPath 'dependencies' -Force
                }
            }
            Copy-Item -LiteralPath 'dependencies\glfw-3.4.bin.WIN64\lib-vc2022\glfw3.dll' -Destination 'dependencies\dll\glfw3.dll' -Force
        } finally {
            if (Test-Path -LiteralPath $tmp) { Remove-Item -LiteralPath $tmp -Recurse -Force }
        }
    }

    foreach ($outDir in 'x64\Debug', 'x64\Release') {
        $null = New-Item -ItemType Directory -Force -Path $outDir
        Copy-Item -LiteralPath 'dependencies\dll\glfw3.dll' -Destination (Join-Path $outDir 'glfw3.dll') -Force
    }
    Write-Host '[완료] dependencies\ 설치 및 x64\Debug, x64\Release에 glfw3.dll 복사 완료.'
    Write-Host '출력 폴더가 다른 경우: Copy-Item dependencies\dll\glfw3.dll <EXE폴더>\ -Force'
}

EXAM_DLL_COPY — 빌드 후 GLFW DLL 자동 복사
솔루션 탐색기에서 프로젝트 우클릭 → 속성. 구성은 모든 구성, 플랫폼은 x64로 선택한다.
구성 속성 → 빌드 이벤트 → 빌드 후 이벤트 → 명령줄에 다음 한 줄을 넣고 적용한다.
    copy /Y "$(ProjectDir)dependencies\dll\glfw3.dll" "$(TargetDir)glfw3.dll"
원본 DLL은 EXAM_INSTALL로 준비한다. $(TargetDir)은 현재 구성의 실제 EXE 폴더다.
설정을 저장한 뒤 다시 빌드하고 실제 EXE 옆에 glfw3.dll이 생겼는지 확인한다.

Visual Studio 주석 맞춤법 표시 끄기
.editorconfig 없이 gllib.cpp를 연 경우, 편집 → 고급 → 맞춤법 검사 전환(Toggle Spell Checker)을 선택한다.
*/

// EXAM_ENTRY — 프레임워크 선언보다 먼저 보이는 C++ 진입점
#ifndef GLLIB_NO_MAIN
static int runExam();

int main() {
    return runExam();
}
#endif

/*
[시험장 실행 시나리오] 새 x64 콘솔 프로젝트 → 첫 화면 → 접이식 검사판
아래 줄 번호는 현재 파일 기준이다. 코드 삽입으로 밀리면 괄호 안의 이름을 검색한다.

01. 반입 파일 배치 · 시험장 빠른 세팅
    gllib.cpp와 obj 폴더를 시험 프로젝트 폴더에 둔다. lecture_pdf는 열람용이다.
    빈 C++ 콘솔 프로젝트, x64, /std:c++17 이상, /utf-8 권장. PCH 사용 안 함.
    gllib.cpp를 소스에 추가하고 템플릿 main()의 .cpp는 빌드에서 제외한다.

02. 의존성 연결 · GLEW 178행, GLFW 194행, GLM 206행
    (인터넷 가능 시) EXAM_INSTALL을 프로젝트 폴더에서 1회 실행하면 dependencies\와 DLL이 자동 배치된다.
    (오프라인/현장 SDK) 추가 포함 디렉터리에 헤더 상위 폴더 3개, 추가 라이브러리 디렉터리에 x64 glfw3dll.lib 폴더,
    실행 파일 옆(또는 DLL 검색 경로)에 같은 배포본 x64 glfw3.dll을 둔다.
    부가 설정과 오류별 조치는 아래 시험 안내, 현장 SDK 폴더 조회는 EXAM_OFFLINE.

03. 첫 화면 확인 · EXAM_START, main
    내장 GLSL 상태로 실행하면 바닥과 탱크가 나온다. Q 또는 Esc를 눌러 종료한다.
    도형을 만드는 곳은 EXAM_START, 크기와 배치 상수는 EXAM_LAYOUT를 검색한다.

04. 멤버 추가 · class Console의 public: 앞
    아래 세 줄을 멤버에 추가한다.
        std::optional<graphics::EntityId> panel_;
        float panelAngle_ = 0.0f;
        bool panelOpen_ = false;

05. 장면 작성 · EXAM_START
    카메라와 기본 조명 설정은 유지한다. 함수 끝의 'auto& scene'부터 return까지 아래로 교체한다.
        auto& scene = app_.getScene();
        const auto floor = scene.addCuboid({10.0f, 0.2f, 10.0f}, {0.0f, -0.1f, 0.0f});
        panel_ = scene.addCuboid({1.6f, 1.2f, 0.08f}, {0.0f, 0.0f, 0.0f});
        if (!floor || !panel_)
            return false;
        onUpdate(0.0f);
        return scene.setTint(*floor, {0.45f, 0.45f, 0.45f}) &&
               scene.setTint(*panel_, {0.35f, 0.7f, 0.95f});

06. 입력 작성 · EXAM_INPUT
    GLFW_PRESS 필터 뒤, Q/Esc 처리 앞에 추가한다.
        if (key == GLFW_KEY_SPACE)
            panelOpen_ = !panelOpen_;
        if (key == GLFW_KEY_R) {
            panelOpen_ = false;
            panelAngle_ = 0.0f;
        }
    키를 누르고 있는 동작은 PRESS/RELEASE를 필터 앞에서 처리한다. 포커스 해제는 onFocus().

07. 변환 작성 · EXAM_UPDATE
    onUpdate의 본문을 아래로 교체한다. 판의 로컬 상단 y=0.6이 월드 y=1.65의 경첩에 고정된다.
        if (!panel_)
            return;
        const float target = panelOpen_ ? -90.0f : 0.0f;
        const float step = 120.0f * dt;
        panelAngle_ += std::clamp(target - panelAngle_, -step, step);
        graphics::Transform3D t;
        t.basis = {0.0f, 1.65f, 0.0f};
        t.localBasis = {0.0f, 0.6f, 0.0f};
        t.rotation = glm::mat3(glm::rotate(glm::mat4(1.0f), glm::radians(panelAngle_),
                                         glm::vec3(1.0f, 0.0f, 0.0f)));
        app_.getScene().setTransform(*panel_, t);

08. 동작 확인 · EXAM_UPDATE, Application::run
    첫 화면에서 판은 수직이다. Space로 수평까지 펴고, 움직이는 중 다시 누르면 현재 각도에서 방향을 바꾼다.
    경첩이 고정되는지 보고 R로 수직 복귀, Q/Esc로 종료한다. 새 문제에서는 04~07의 상태·변환만 교체한다.

09. GLSL을 요구할 때 · EXAM_GLSL, EXAM_RUN의 ApplicationConfig
    주석의 최소 셰이더 쌍을 사용한다. 외부 파일이면 config.shaderDirectory = "shaders"; 를 설정하고
    프로젝트 폴더(또는 EXE 폴더) 아래 shaders/에 VertexShader.glsl과 FragmentShader.glsl을 둔다.
*/

#ifndef _WIN32
#error This framework targets Windows x64.
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#ifndef GLEW_STATIC
#define GLEW_STATIC
#endif
#if __has_include(<GL/glew.h>)
#include <GL/glew.h>
#elif __has_include("dependencies/glew-2.3.1/include/GL/glew.h")
#include "dependencies/glew-2.3.1/include/GL/glew.h"
#elif __has_include("../dependencies/glew-2.3.1/include/GL/glew.h")
#include "../dependencies/glew-2.3.1/include/GL/glew.h"
#else
#error "GLEW header not found. Place dependencies/ next to gllib.cpp or configure AdditionalIncludeDirectories."
#endif

#ifndef GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_NONE
#endif
#ifndef GLFW_DLL
#define GLFW_DLL
#endif
#if __has_include(<GLFW/glfw3.h>)
#include <GLFW/glfw3.h>
#pragma comment(lib, "glfw3dll.lib")
#elif __has_include("dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h")
#include "dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#pragma comment(lib, "dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib")
#elif __has_include("../dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h")
#include "../dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h"
#pragma comment(lib, "../dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib")
#else
#error "GLFW header not found. Place dependencies/ next to gllib.cpp or configure AdditionalIncludeDirectories."
#endif
#if __has_include(<glm/glm.hpp>)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#elif __has_include("dependencies/glm/glm/glm.hpp")
#include "dependencies/glm/glm/glm.hpp"
#include "dependencies/glm/glm/gtc/matrix_transform.hpp"
#include "dependencies/glm/glm/gtc/type_ptr.hpp"
#elif __has_include("../dependencies/glm/glm/glm.hpp")
#include "../dependencies/glm/glm/glm.hpp"
#include "../dependencies/glm/glm/gtc/matrix_transform.hpp"
#include "../dependencies/glm/glm/gtc/type_ptr.hpp"
#else
#error "GLM header not found. Place dependencies/ next to gllib.cpp or configure AdditionalIncludeDirectories."
#endif

#pragma comment(lib, "opengl32.lib")

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

static_assert(sizeof(void*) == 8, "x64 build required.");

// ================================================================================
// [2부] 전방 선언부 (Forward Declarations)
// --------------------------------------------------------------------------------
// Visual Studio에서 각 타입·객체 이름 위에 마우스 커서를 올리면(Quick Info)
// 해당 객체가 프레임워크에서 맡는 역할을 바로 확인할 수 있습니다.
// ================================================================================

struct GLFWwindow; ///< GLFW 네이티브 창 및 OpenGL 컨텍스트 핸들(전방 선언)

namespace graphics {

// --- 2-1. 공통 상수 및 식별자 별칭 ------------------------------------------------
constexpr int WINDOW_WIDTH = 800;       ///< 기본 애플리케이션 창 너비(픽셀)
constexpr int WINDOW_HEIGHT = 600;      ///< 기본 애플리케이션 창 높이(픽셀)
constexpr std::size_t MAX_ENTITIES = 50;///< 초기 엔티티 예약 힌트(장면 상한 아님)
constexpr std::size_t MAX_LINES = 500;  ///< 초기 선분 예약 힌트(장면 상한 아님)

using EntityId = std::size_t;           ///< Scene에 등록된 3D 엔티티(다면체·그룹) 고유 ID
using LineId = std::size_t;             ///< Scene에 등록된 3D 선분(축·벡터·가이드선) 고유 ID
using MotionId = std::uint64_t;         ///< MotionSystem에 등록된 모션 타임라인 고유 ID
using LayerId = std::uint64_t;          ///< 계층 변환 합성을 위한 모션 레이어 고유 ID (루트 = 0)

// --- 2-2. 진단 (Diagnostics) -----------------------------------------------------
enum class PipelineStage;               ///< 오류 기록을 분류하는 렌더링 파이프라인 단계 열거형
struct DiagnosticOptions;               ///< 진단 오류 출력 옵션
struct Diagnostic;                      ///< 발생한 오류의 단계·API·원인·조치·소스 파일/줄 기록 구조체
class Diagnostics;                      ///< 전역 진단 문맥·오류 기록·콘솔 출력 정적 유틸리티

// --- 2-3. 모델 · 형상 · 재질 (Model & Material) -----------------------------------
enum class GLShape;                     ///< 기본 도형 종류 (None, Cube, Pyramid, Custom, Imported)
enum class GLFace;                      ///< 면 단위 기하 형식 (None, Rect, Triangle, Mesh)
enum class PivotMode;                   ///< 모델 로컬 기준점(피벗) 결정 정책 (Preserve, BottomCenter, Custom)
enum class OutlineMode;                 ///< 외곽선 추출 방식 (None, FeatureEdges, AllTriangleEdges, Explicit)
enum class CullMode;                    ///< 면 컬링 정책 (Inherit, None, Back, Front)
enum class FaceTransformMode;           ///< 면 변환 기준 공간 (Hierarchy 부모 종속 / Independent 월드 독립)
enum class FaceAnimationSpace;          ///< 면 추가 애니메이션 행렬 적용 공간 (Local / Model)

struct GlColor;                         ///< 0~1 범위의 RGB 색상(red, green, blue) 구조체
struct Transform3D;                     ///< 이동(basis)·피벗(localBasis)·회전(rotation)·배율(scale) SRT 변환
struct Material;                        ///< 색상·Phong 조명 계수(ambient/diffuse/specular/shininess/unlit) 재질
struct Vertex;                          ///< 위치·법선·UV·색상을 하나로 묶은 정점 입력 구조체
struct PlaneFigure;                     ///< 단일 면/서브메시(정점·삼각형 인덱스·법선·UV·재질·면 변환) 데이터
using Mesh = PlaneFigure;               ///< PlaneFigure의 별칭 (일반 삼각형 메시 단위)
struct Polyhedron;                      ///< 여러 면(Mesh)을 묶어 보관하는 3D 다면체/모델 컨테이너
using Model = Polyhedron;               ///< Polyhedron의 별칭 (생성 또는 .obj 로드 모델)
struct Component_Poly;                  ///< Scene 내부 엔티티 1개의 모델·변환·부모·색조·로컬 레이어 상태
struct FaceId;                          ///< 특정 엔티티(entityId)와 내부 면 인덱스(faceIndex) 식별 쌍
struct Line3D;                          ///< 시작점·끝점·색상·소유 엔티티로 정의되는 3D 선분 데이터

// --- 2-4. 변환 · 경로 (Transform & Path) ------------------------------------------
struct AxisLine;                        ///< 기준점(point)과 축 방향(direction)으로 정의하는 3D 직선
using PathFunction = std::function<glm::vec3(double)>;   ///< 진행률 t(0~1)를 3D 좌표로 사상하는 경로 함수
using InterpolationFunction = std::function<double(double)>; ///< 선형 진행률을 완화/보간 진행률로 변환하는 함수
class TransformSystem;                  ///< 이동·배율·임의 축 회전·점/선/평면 반사 아핀 행렬 생성기
class PathSystem;                       ///< 타원·원·꺾은선·다각형·직사각형 경로 및 보간 함수 생성기

// --- 2-5. 모션 · 애니메이션 레이어 (Motion & Animation) ----------------------------
enum class TransformSpace;              ///< 모션 행렬 합성 기준 공간 (World 공전 / Local 자전)
enum class Playback;                    ///< 모션 재생 모드 (Once 1회 / Loop 반복 / PingPong 왕복)
struct MotionSample;                    ///< 특정 시각에 평가된 부모(World)·로컬(Local) 변환 행렬 쌍
struct RotationMotion;                  ///< 축·초당 각속도·초기각·공간으로 정의하는 연속 회전 설정
struct PathMotion;                      ///< 경로·보간·주기·위상·재생 방식으로 정의하는 경로 이동 설정
struct LinearMotion;                    ///< 시작점→끝점 선형 이동 애니메이션 설정
struct PendulumMotion;                  ///< 사인파 진자 왕복 회전 애니메이션 설정
struct MotionStep;                      ///< 모션 타임라인의 단일 변환 또는 동작 단계 항목
class Motion;                           ///< 여러 변환 단계를 합성하는 시간 기반 모션 타임라인
struct MotionState;                     ///< 등록된 모션의 현재 시간·재생·활성 여부 및 마지막 샘플 상태
struct LayerConfig;                     ///< 모션 레이어의 고정 변환·합성 공간·연결 모션·활성 설정
struct LayerState;                      ///< 모션 레이어 설정과 부모/자식 트리 연결 정보
class MotionSystem;                     ///< 모션 시간 진행과 계층적 모션 레이어 행렬 합성을 관리하는 시스템

// --- 2-7. 자산 · OBJ 로더 (Assets & OBJ Loader) -----------------------------------
struct LoadOptions;                     ///< .obj 로드 시 피벗·윤곽선·동일평면 병합 옵션
struct ModelResult;                     ///< .obj 로드 결과(성공 모델·계산된 피벗·경고·오류 메시지)
class AssetLoader;                      ///< 프로젝트 상대경로의 .obj 모델을 로드하는 로더

// --- 2-8. 렌더 설정 · 장면 · 카메라 · 조명 · 렌더러 (Scene & Renderer) --------------
struct RenderVertex;                    ///< GPU 업로드 전 CPU 표준 정점 레이아웃(위치·색·UV·법선)
struct VertexAttribute;                 ///< VAO 정점 속성 1개의 location·성분 수·타입·오프셋 기술자
struct VertexLayout;                    ///< VBO 정점 stride와 VertexAttribute 목록 정의
struct UniformBindings;                 ///< 셰이더 내장 uniform 변수 이름 매핑 테이블
struct DrawUniformContext;              ///< 커스텀 uniform 콜백에 전달되는 현재 드로우 상태 및 uniform 설정 도구
struct RenderConfig;                    ///< 커스텀 정점 패킹·VAO 레이아웃·uniform 바인딩 확장 설정
class Scene;                            ///< 3D 엔티티·선분·부모 계층·재질·모션 연결을 총괄하는 CPU 장면 관리자
class DiagramSystem;                    ///< 육면체·사각뿔 CPU 모델 생성 및 유효성 검사기
struct Camera;                          ///< 시점·주목점·up·원근/직각 투영 설정을 보관하는 카메라 구조체
class CameraSystem;                     ///< 카메라 유효성 검사·아핀 프레임 부착·View/Projection 행렬 계산기
struct PointLight;                      ///< 월드 위치·색상·강도·거리 감쇠를 갖는 점광원 구조체
struct RenderOptions;                   ///< 깊이·와이어프레임·조명·배경색·컬링·점광원 목록 등 프레임 렌더 옵션
struct Viewport;                        ///< 프레임버퍼 좌하단 기준 뷰포트 영역(x, y, width, height)
struct RenderPass;                      ///< 다중 시점 분할 렌더링을 위한 카메라·뷰포트·옵션·클리어 설정 1패스
class Renderer;                         ///< OpenGL 3.3 셰이더·VAO/VBO/EBO 업로드 및 드로우 실행기

// --- 2-9. 피킹 (Ray Picking) ------------------------------------------------------
struct Ray;                             ///< 시작점·단위 방향·최대 거리로 정의되는 3D 광선
struct PickResult;                      ///< 광선 교차 판정 결과(엔티티·면·삼각형·월드/로컬 교점·UV·거리)
struct PickViewport;                    ///< 화면 좌상단 기준 피킹 영역 사각형

// --- 2-10. 애플리케이션 (Application) ---------------------------------------------
struct ApplicationConfig;               ///< 창 크기·제목·셰이더/프로젝트 경로·VSync·진단·렌더 확장 설정
struct MouseEvent;                      ///< 마우스 버튼·액션·수정키·창/프레임버퍼 좌표 이벤트 구조체
class Application;                      ///< GLFW 창·GL 컨텍스트·입력 콜백·메인 루프·Scene/Renderer/OBJ 로더 총괄

// --- 2-12. 공개 자유 함수 전방 선언 (Free Functions) -------------------------------
inline void recalculateNormals(Mesh& mesh); ///< 삼각형 기하로부터 메시의 정점 법선을 재계산
inline std::vector<std::uint32_t> meshOutline(const Mesh& mesh, OutlineMode mode); ///< 외곽선 인덱스 쌍 추출
std::string readTextFile(const std::filesystem::path& path); ///< 텍스트 파일 전체를 UTF-8 BOM 제거 후 문자열로 읽기
inline std::optional<Ray> screenRay(const Camera& camera, double x, double y,
                                    const PickViewport& viewport); ///< 화면 픽셀 좌표를 3D 카메라 광선으로 역투영
inline std::optional<PickResult> pick(const Scene& scene, const Ray& input,
                                      CullMode inheritedCull); ///< 3D 광선과 장면 삼각형의 최근접 교차 판정
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
                                      const PickViewport& viewport, CullMode inheritedCull); ///< 화면 좌표 피킹

// --- 2-13. 내부 구현 보조 전방 선언 (graphics::detail) -----------------------------
namespace detail {
struct ObjParseError;                   ///< .obj 텍스트 파싱 오류 전달용 내부 예외 타입
struct ObjCorner;                       ///< .obj 면 정점 1개(v/vt/vn 인덱스 조합) 내부 구조체
struct ObjFace;                         ///< .obj 파일의 f 레코드 1줄을 보관하는 내부 구조체

inline bool finiteVector(const glm::vec3& v); ///< 3D 벡터의 모든 성분이 유한한지 검사
inline bool finiteMatrix(const glm::mat4& m); ///< 4x4 행렬의 모든 원소가 유한한지 검사
inline bool validColor(GlColor c);            ///< RGB 색상 성분이 유한한지 검사
inline std::vector<std::uint32_t> triangles(const Mesh& mesh); ///< 메시의 삼각형 인덱스 배열 반환
inline bool validMesh(const Mesh& m);         ///< 메시 정점·법선·UV·재질·삼각형 유효성 검사
inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model); ///< 모델의 축 정렬 경계 상자(min, max) 계산
inline glm::vec3 bottomCenter(const Model& model); ///< 모델 경계 상자의 바닥 중심 좌표 계산
inline glm::mat4 transformMatrix(const Transform3D& t); ///< T(basis)*R*S*T(-localBasis) 행렬 계산
inline glm::mat4 faceSrtMatrix(const Mesh& face); ///< 독립 면의 로컬 SRT 행렬 계산
inline glm::mat4 faceLocalMatrix(const Mesh& face); ///< 면의 SRT와 localAnimationMatrix 합성
inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face); ///< 엔티티와 면의 최종 모델 행렬 합성

inline std::vector<std::uint32_t> triangulate(const std::vector<glm::vec3>& points); ///< 평면 단순 다각형 귀 자르기 삼각화
inline void appendMesh(Mesh& destination, const Mesh& source); ///< source 메시를 destination 끝에 병합
inline bool shareEdge(const Mesh& a, const Mesh& b); ///< 두 메시가 공유 모서리를 가지는지 판정
inline void mergeCoplanar(Model& model); ///< 동일 평면 인접 면들을 하나의 메시로 병합

inline void checkCom(HRESULT result); ///< COM HRESULT 실패 시 std::runtime_error 예외 발생

inline std::filesystem::path executableDirectory(); ///< 실행 파일(.exe)이 위치한 디렉터리 경로 반환
inline std::filesystem::path sourceDirectory();     ///< gllib.cpp 소스 파일이 위치한 디렉터리 경로 반환
inline ModelResult finishLoad(Model model, const LoadOptions& options); ///< 로드된 모델의 유효성·피벗 확정
inline std::filesystem::path relativePathInside(const std::filesystem::path& root,
                                                const std::filesystem::path& relative); ///< root 내부 상대경로 안전 해석
inline std::filesystem::path resolveProjectRelative(const std::filesystem::path& primaryRoot,
                                                    const std::filesystem::path& relative); ///< 상대경로를 실행·작업·소스·상위 폴더 순으로 안전 탐색
inline bool checkGlAt(const char* file, int line, PipelineStage stage, const char* api); ///< OpenGL 오류 큐 점검·보고

std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction); ///< 영벡터가 아닌 방향 벡터를 단위화
std::optional<glm::mat4> motionAround(const glm::vec3& center, const glm::mat4& matrix); ///< 기준점 주위 변환 T(c)*M*T(-c)
bool motionPlane(const glm::vec3& u, const glm::vec3& v); ///< 두 벡터가 비퇴화 평면을 이루는지 검사
bool positiveLengths(const glm::vec3& value); ///< 세 변 길이가 모두 양수·유한한지 검사
std::uint64_t nextGeometryRevision(); ///< GPU 재업로드 판별용 단조 증가 형상 리비전 번호 발급
void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color); ///< 기본 도형 면 추가
bool& applicationActive(); ///< 단일 Application 동시 실행 가드 플래그 참조
unsigned& contextErrorSerial(); ///< GLFW/WGL 오류 콜백 누적 카운터 참조
} // namespace detail

} // namespace graphics

// ================================================================================
// [3부] 타입 · 클래스 인터페이스 선언부 (Type & Class Declarations)
// --------------------------------------------------------------------------------
// 모든 열거형·구조체·클래스의 멤버와 공개 API 시그니처만 선언합니다.
// 실제 구현 본문은 아래 [4부] 후방 정의부에 분리되어 있습니다.
// ================================================================================

namespace graphics {

// --- 3-1. Diagnostics 선언 -------------------------------------------------------

/// [역할] 오류 기록을 분류하는 렌더링 파이프라인 단계 열거형.
enum class PipelineStage { Runtime, Input, Scene, Motion, VertexInput, Shader, Transform, Raster, Framebuffer, Asset };

/// [역할] 진단 오류 출력 여부를 담는 옵션 구조체.
struct DiagnosticOptions {
    bool errors = true; ///< 오류 발생 시 std::cerr에 원인·조치·소스 위치 출력 여부
};

/// [역할] 발생한 파이프라인 오류 1건의 단계·API·원인·조치·소스 위치 기록 구조체.
struct Diagnostic {
    PipelineStage stage = PipelineStage::Runtime;
    std::string api, cause, action;
    std::optional<std::uint64_t> object;
    std::string file, operation;
    int line = 0;
};

/// [역할] 전역 오류 기록·콘솔 출력·호출 위치를 기록하는 정적 진단 유틸리티.
class Diagnostics {
    static DiagnosticOptions& options();
    static std::optional<Diagnostic>& issue();

public:
    /// [API] 진단에 첨부할 현재 파일·줄·호출 경로 문맥의 참조를 반환한다.
    static Diagnostic& context();

    /// [API] 다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.
    static void locate(const char* file, int line, const char* operation);

    /// [API] 주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.
    static bool failAt(const char* file, int line, PipelineStage stage, const char* api, const std::string& cause,
                       const std::string& action = "소스 위치와 해당 API의 입력 조건을 확인하세요.",
                       std::optional<std::uint64_t> object = std::nullopt);

    /// [API] 오류 출력 여부를 설정한다.
    static void configure(DiagnosticOptions value);

    /// [API] 마지막으로 기록한 오류의 optional 참조를 반환한다.
    static const std::optional<Diagnostic>& lastError();

    /// [API] 마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.
    static void clear();

    /// [API] 현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.
    static bool fail(
        PipelineStage stage, const char* api, const std::string& cause,
        const std::string& action = "gllib.cpp 하단 가이드와 해당 API의 입력 조건·반환값을 확인하세요.",
        std::optional<std::uint64_t> object = std::nullopt);
};

// --- 3-2. Model · Material 선언 --------------------------------------------------

/// [역할] 0~1 범위의 RGB 색상(red, green, blue)을 표현하는 구조체.
struct GlColor {
    float red = 1.0f, green = 1.0f, blue = 1.0f;
};

/// [역할] 기본 다면체 형상 종류 열거형.
enum class GLShape { None, Cube, Pyramid, Custom, Imported };
/// [역할] 면 단위 기하 표현 형식 열거형.
enum class GLFace { None, Rect, Triangle, Mesh };
/// [역할] 모델의 로컬 피벗(localBasis) 자동 계산 정책 열거형.
enum class PivotMode { Preserve, BottomCenter, Custom };
/// [역할] 메시 윤곽선(GL_LINES) 추출 정책 열거형.
enum class OutlineMode { None, FeatureEdges, AllTriangleEdges, Explicit };
/// [역할] 면 컬링(앞면/뒷면 제거) 정책 열거형.
enum class CullMode { Inherit, None, Back, Front };
/// [역할] 면 변환이 부모 계층을 따를지 월드에 독립할지 정하는 정책 열거형.
enum class FaceTransformMode { Hierarchy, Independent };
/// [역할] 면의 localAnimationMatrix 적용 순서를 결정하는 공간 열거형.
enum class FaceAnimationSpace { Local, Model };

/// [역할] 이동(basis)·피벗(localBasis)·회전(rotation)·배율(scale)을 담는 3D SRT 변환 구조체.
/// 최종 행렬 공식: T(basis) * R(rotation) * S(scale) * T(-localBasis)
/// 오른쪽부터 적용한다. localBasis를 원점으로 옮겨 회전·배율을 적용한 뒤 basis에 배치한다.
struct Transform3D {
    glm::vec3 basis{0.0f};       ///< 부모(또는 월드) 좌표계 기준 배치 위치
    glm::vec3 localBasis{0.0f};  ///< 회전·배율의 중심이 되는 모델 로컬 피벗 좌표
    glm::mat3 rotation{1.0f};    ///< 3x3 직교 회전 행렬
    glm::vec3 scale{1.0f};       ///< 로컬 X/Y/Z 축 배율
};

/// [역할] 색상·Phong 조명 계수(ambient/diffuse/specular/shininess/unlit)를 보관하는 재질 구조체.
struct Material {
    GlColor color;
    float ambientStrength = .25f, diffuseStrength = 1;
    float specularStrength = 0, shininess = 32;
    bool unlit = false;
};

/// [역할] 정점 위치·법선·UV·색상을 하나로 묶어 메시 생성에 넘기는 정점 구조체.
struct Vertex {
    glm::vec3 position{0.0f};
    glm::vec3 normal{0.0f};
    glm::vec2 uv{0.0f};
    GlColor color;
};

/// [역할] 단일 면 또는 서브메시의 정점·인덱스·법선·UV·재질·개별 면 변환을 보관하는 구조체(별칭: Mesh).
struct PlaneFigure {
    GLFace shape = GLFace::Mesh;
    std::vector<glm::vec3> localVertices;
    GlColor color;
    glm::vec3 normal{0.0f};
    // Compatibility flag: true enables child-local SRT, never world detachment.
    bool independence = false;
    Transform3D transform;
    bool visible = true;
    glm::mat4 localAnimationMatrix{1.0f};
    std::vector<GlColor> vertexColors;
    std::vector<glm::vec3> vertexNormals;
    std::vector<glm::vec2> texcoords;
    std::vector<std::uint32_t> triangleIndices;
    std::vector<std::uint32_t> outlineIndices; // GL_LINES pairs, mesh-local indices.
    Material material;
    OutlineMode outline = OutlineMode::None;
    float creaseAngleDegrees = 1.0f;
    std::string name;
    CullMode cull = CullMode::Inherit;
    FaceTransformMode transformMode = FaceTransformMode::Hierarchy;
    glm::mat4 transformFrame{1.0f}; // Parent-local correction, or a frozen world frame.
    FaceAnimationSpace animationSpace = FaceAnimationSpace::Local;

    /// [API] 정점 목록과 삼각형 인덱스에서 하나의 면(Mesh) 데이터를 만든다. Scene 등록은 별도 수행.
    static PlaneFigure fromVertices(const std::vector<Vertex>& vertices, const std::vector<std::uint32_t>& triangles);
};

/// [역할] 여러 면(PlaneFigure/Mesh)으로 구성된 3D 다면체 및 모델 컨테이너(별칭: Model).
struct Polyhedron {
    GLShape shape = GLShape::Custom;
    std::vector<PlaneFigure> faces;
    std::string name;

    /// [API] 모델이 보관한 면/메시 배열의 수정 가능한 참조를 반환한다.
    std::vector<Mesh>& meshes();

    /// [API] 모델이 보관한 면/메시 배열의 읽기 전용 참조를 반환한다.
    const std::vector<Mesh>& meshes() const;
};

/// [역할] Scene에 등록된 엔티티 1개의 모델·변환·부모 관계·색조·로컬 레이어를 보관하는 컴포넌트.
struct Component_Poly {
    Polyhedron polyhedron;
    Transform3D transform;
    glm::vec3 sideLengths{1.0f};
    bool visible = true;
    PivotMode pivotMode = PivotMode::Preserve;
    std::optional<EntityId> parent;     // Local SRT is relative to this entity.
    GlColor tint;                       // Per-entity multiplier; does not propagate to children.
    bool transformOnly = false;
    glm::mat4 attachment{1.0f};         // Object-owned affine placement, including inherited shear.
    std::vector<glm::mat4> localLayers; // Ordered, object-local transformations.
};

/// [역할] 특정 엔티티 ID와 그 안의 면 인덱스를 쌍으로 가리키는 식별 구조체.
struct FaceId {
    EntityId entityId = 0;
    std::size_t faceIndex = 0;
};

/// [역할] 두 끝점과 색상, 선택적 소유 엔티티로 정의되는 3D 선분(축·벡터·궤도 표시용).
struct Line3D {
    glm::vec3 start{0.0f}, end{0.0f};
    GlColor color;
    std::optional<EntityId> owner;
};

// --- 3-3. Transform · Path 선언 --------------------------------------------------

/// [역할] 기준점과 방향 벡터로 표현하는 3D 회전·반사 축 직선.
struct AxisLine {
    glm::vec3 point{0.0f};
    glm::vec3 direction{0.0f, 1.0f, 0.0f};
};

/// [역할] 이동·배율·임의 축 회전·점/선/평면 반사 아핀 변환 행렬을 생성하는 유틸리티 클래스.
class TransformSystem {
public:
    /// [API] 행렬 원소가 유한하고 마지막 행이 아핀 변환 형식인지 검사한다.
    static bool validAffine(const glm::mat4& matrix);

    /// [API] 이동 벡터로 아핀 이동 행렬을 만든다. 유효하지 않으면 빈 optional이다.
    static std::optional<glm::mat4> translation(const glm::vec3& offset);

    /// [API] 중심점 주위의 축별 배율 행렬 T(C)·S·T(-C)를 만든다.
    static std::optional<glm::mat4> scaling(const glm::vec3& factors, const glm::vec3& center = glm::vec3(0));

    /// [API] 직선 위 점·축 방향과 도 단위 각도로 축 주위 회전 행렬을 만든다.
    static std::optional<glm::mat4> rotation(const AxisLine& axis, double degrees);

    /// [API] 주어진 점을 중심으로 세 축을 반전하는 점 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPoint(const glm::vec3& center);

    /// [API] 주어진 직선을 기준으로 선 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectLine(const AxisLine& line);

    /// [API] 평면의 점과 법선으로 평면 반사 행렬을 만든다.
    static std::optional<glm::mat4> reflectPlane(const glm::vec3& point, const glm::vec3& normal);
};

/// [역할] 타원·원·꺾은선·다각형·직사각형 경로 함수와 보간 함수를 생성하는 유틸리티 클래스.
class PathSystem {
public:
    // axisU/axisV include the ellipse radii and define its plane.
    /// [API] 중심과 두 반축 벡터로 매개변수 타원 경로 함수를 만든다.
    static PathFunction ellipse(const glm::vec3& center, const glm::vec3& axisU, const glm::vec3& axisV);

    /// [API] 중심·반지름·평면 법선으로 원 경로 함수를 만든다.
    static PathFunction circle(const glm::vec3& center, float radius, const glm::vec3& normal = glm::vec3(0, 1, 0));

    /// [API] 점 목록을 따라가는 꺾은선 경로 함수를 만든다. closed로 순환 여부를 정한다.
    static PathFunction polyline(std::vector<glm::vec3> points, bool closed = false);

    /// [API] 마지막 점과 첫 점이 연결되는 닫힌 꺾은선 경로 함수를 만든다.
    static PathFunction polygon(std::vector<glm::vec3> points);

    /// [API] 중심과 두 반변 벡터로 직사각형 테두리 경로 함수를 만든다.
    static PathFunction rectangle(const glm::vec3& center, const glm::vec3& halfU, const glm::vec3& halfV);

    /// [API] 입력 진행률을 선형 보간 진행률로 반환한다.
    static double linear(double progress);

    /// [API] 진행률에 3t²-2t³ 완화 곡선을 적용한다.
    static double smoothStep(double progress);
};

// --- 3-4. Motion · MotionSystem 선언 ---------------------------------------------

/// [역할] 모션 변환을 적용할 공간 기준(World 공전 vs Local 자전).
enum class TransformSpace { World, Local };
/// [역할] 경로·선형 모션의 재생 반복 정책(Once 1회 / Loop 반복 / PingPong 왕복).
enum class Playback { Once, Loop, PingPong };

/// [역할] 특정 시각의 모션 평가 결과인 월드(parent) 및 로컬(local) 행렬 쌍.
struct MotionSample {
    glm::mat4 parent{1.0f};
    glm::mat4 local{1.0f};
};

/// [역할] 축·각속도·초기각·공간으로 정의하는 연속 회전 모션 설정.
struct RotationMotion {
    AxisLine axis;
    double degreesPerSecond = 45;
    double initialDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};

/// [역할] 경로 함수·보간·주기·재생 방식으로 정의하는 경로 이동 모션 설정.
struct PathMotion {
    PathFunction path;
    InterpolationFunction interpolation = PathSystem::linear;
    double durationSeconds = 1;
    double rate = 1;
    double phase = 0;
    Playback playback = Playback::Loop;
    bool relativeToStart = false;
    TransformSpace space = TransformSpace::World;
};

/// [역할] 시작점과 끝점 사이를 지정 시간 동안 선형 이동하는 모션 설정.
struct LinearMotion {
    glm::vec3 from{0}, to{1, 0, 0};
    double durationSeconds = 1;
    Playback playback = Playback::Once;
    TransformSpace space = TransformSpace::Local;
};

/// [역할] 지정 축 주위를 사인파로 왕복 회전하는 진자 모션 설정.
struct PendulumMotion {
    AxisLine axis{{0, 0, 0}, {1, 0, 0}};
    double amplitudeDegrees = 25;
    double periodSeconds = 3;
    double phaseDegrees = 0;
    double offsetDegrees = 0;
    TransformSpace space = TransformSpace::Local;
};

/// [역할] 모션 타임라인의 단일 변환 단계.
struct MotionStep {
    // The callback receives seconds since this step started, not frame delta time.
    std::function<glm::mat4(double)> transform;
    TransformSpace space = TransformSpace::World;
    double startSeconds = 0;
    std::optional<double> durationSeconds;
    bool enabled = true;
};

/// [역할] 시간별 MotionStep의 변환 행렬을 합성하는 모션 타임라인.
class Motion {
    std::vector<MotionStep> steps_;

    static bool validStep(const MotionStep& step);

public:
    /// [API] 모션 단계 배열의 끝에 유효한 변환 단계를 추가한다.
    bool append(const MotionStep& step);

    /// [API] 주어진 인덱스에 모션 단계를 삽입한다.
    bool insert(std::size_t index, const MotionStep& step);

    /// [API] 주어진 인덱스의 모션 단계를 새 단계로 교체한다.
    bool replace(std::size_t index, const MotionStep& step);

    /// [API] 주어진 인덱스의 모션 단계를 삭제한다.
    bool erase(std::size_t index);

    /// [API] 등록된 모션 단계 배열을 읽기 전용 참조로 반환한다.
    const std::vector<MotionStep>& getSteps() const;

    /// [API] 절대 시간(초)에서 활성 단계의 월드/로컬 행렬을 합성한다.
    std::optional<MotionSample> sample(double seconds) const;

    /// [API] 고정 행렬을 반환하는 모션 단계를 만든다. 합성 공간을 함께 지정한다.
    static MotionStep fixed(const glm::mat4& matrix, TransformSpace space = TransformSpace::World);

    /// [API] 축·초당 각속도·초기 각도·합성 공간으로 회전 모션 단계를 만든다.
    static MotionStep rotation(const RotationMotion& config);

    /// [API] 경로·보간·재생 설정으로 시간에 따른 위치 이동 모션 단계를 만든다.
    static MotionStep followPath(const PathMotion& config);

    /// [API] 시작·끝 위치를 지정 시간 동안 선형보간한다. Once·Loop·PingPong을 지원한다.
    static MotionStep linear(const LinearMotion& config);

    /// [API] 지정 축에서 offset + amplitude*sin(위상)의 도 단위 회전을 반복한다. 물리 적분은 아니다.
    static MotionStep pendulum(const PendulumMotion& config);
};

/// [역할] MotionSystem에 등록된 모션 1개의 시간·재생·활성 여부·최근 샘플 상태.
struct MotionState {
    Motion motion;
    double timeSeconds = 0;
    bool playing = true;
    bool enabled = true;
    MotionSample sampled;
};

/// [역할] 모션 레이어 1개의 고정 행렬·공간·연결 모션·활성 설정.
struct LayerConfig {
    glm::mat4 transform{1.0f};
    TransformSpace space = TransformSpace::World;
    std::optional<MotionId> motion;
    bool enabled = true;
};

/// [역할] 모션 레이어 설정과 부모/자식 트리 연결 정보를 보관하는 노드 상태.
struct LayerState {
    LayerConfig config;
    std::optional<LayerId> parent;
    std::vector<LayerId> children;
};

/// [역할] 등록된 모션의 시간 진행과 계층 레이어 트리 행렬 합성을 관리하는 애니메이션 시스템.
class MotionSystem {
    std::map<MotionId, MotionState> motions_;
    MotionId nextMotion_ = 1;
    std::map<LayerId, LayerState> layers_{{0, LayerState{}}};
    LayerId nextLayer_ = 1;

    bool validLayer(const LayerConfig& config) const;

public:
    /// [API] 삭제할 수 없는 루트 레이어의 ID 0을 반환한다.
    static constexpr LayerId rootLayer();

    /// [API] 모션을 시스템에 등록하고 새 MotionId를 반환한다.
    std::optional<MotionId> createMotion(const Motion& motion);

    /// [API] 의미 단계 목록을 검사·조립하여 하나의 모션으로 등록한다.
    std::optional<MotionId> createMotion(std::initializer_list<MotionStep> steps);

    /// [API] 기존 MotionId의 모션 데이터를 교체하고 현재 시각의 샘플을 갱신한다.
    bool setMotion(MotionId id, const Motion& motion);

    /// [API] 모션 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const MotionState* getMotion(MotionId id) const;

    /// [API] 모션의 시간 진행을 정지하거나 재개한다. 모션 기여 자체의 사용 여부와 구분한다.
    bool setMotionPlaying(MotionId id, bool playing);

    /// [API] 모션 샘플의 기여 여부를 설정한다. playing과 별도 상태다.
    bool setMotionEnabled(MotionId id, bool enabled);

    /// [API] 지정 시각을 샘플링하고 유효할 때 모션의 시간과 샘플을 함께 바꾼다.
    bool seekMotion(MotionId id, double seconds);

    /// [API] 모션을 제거하고 연결된 레이어의 해당 모션 연결을 해제한다.
    bool removeMotion(MotionId id);

    /// [API] 모든 샘플 성공 후 시간·샘플을 확정하고 활성 모션의 동작을 실행한다. 동작 실패는 외부 상태를 복구하지 않는다.
    bool update(double deltaSeconds);

    /// [API] 모션과 레이어를 비우고 루트 레이어를 복구한다. 다음 ID 카운터는 되돌리지 않는다.
    void clear(); // Identifiers are never reused by this system, including after clear.

    /// [API] 유효한 부모 아래 새 자식 레이어를 추가한다. 복사한 임시 맵에 반영한 뒤 swap하여 상태를 확정한다.
    std::optional<LayerId> pushLayer(LayerId parent, const LayerConfig& config = LayerConfig{});

    /// [API] 기존 child와 그 parent 사이에 새 레이어를 끼운다. root 위 삽입은 허용하지 않는다.
    std::optional<LayerId> insertLayer(LayerId child, const LayerConfig& config = LayerConfig{});

    /// [API] 자식이 없는 비root leaf만 제거하고 그 부모 ID를 반환한다.
    std::optional<LayerId> popLayer(LayerId top); // Only removes a leaf; returns its parent.

    /// [API] root 이외 레이어를 제거하며 자식들을 부모의 해당 위치로 승격한다. 월드 자세 보존을 자동 약속하지 않는다.
    bool removeLayer(LayerId id); // Promotes children in place; root cannot be removed.

    /// [API] 레이어의 부모를 바꾸며 자신의 자손으로 연결되는 순환을 거부한다.
    bool reparentLayer(LayerId id, LayerId parent);

    /// [API] 기존 레이어의 고정 변환·합성 공간·모션 연결·활성 상태를 바꾼다.
    bool setLayer(LayerId id, const LayerConfig& config);

    /// [API] 레이어 상태의 읽기 전용 비소유 포인터를 반환한다. 없는 ID면 nullptr이다.
    const LayerState* getLayer(LayerId id) const;

    /// [API] 기존 레이어에 모션 ID를 연결하거나 해제한다. 모션과 레이어가 존재하는지 검사한다.
    bool bindMotion(LayerId layer, std::optional<MotionId> motion);

    /// [API] leaf까지의 계층을 따라 transform과 motion 샘플을 World/Local 공간에 맞게 parent/local 행렬로 합성한다.
    std::optional<MotionSample> layerMatrices(LayerId leaf) const;

    /// [API] 로컬 공간 회전 모션을 등록하여 자전에 사용한다.
    std::optional<MotionId> createSpin(RotationMotion config);

    /// [API] 월드 공간 회전 모션을 등록하여 축 주위 공전에 사용한다.
    std::optional<MotionId> createOrbit(RotationMotion config);
};

// --- 3-5. Geometry 선언 -----------------------------------------------------

/// [API] 메시의 윤곽 모드에 따라 선으로 그릴 정점 인덱스 쌍 목록을 계산한다.
inline std::vector<std::uint32_t> meshOutline(const Mesh& mesh, OutlineMode mode = OutlineMode::FeatureEdges);

// --- 3-7. AssetLoader · OBJ 로더 선언 --------------------------------------------

/// [역할] .obj 모델 로드 시 피벗 정책·외곽선·동일평면 병합을 지정하는 옵션.
struct LoadOptions {
    PivotMode pivot = PivotMode::BottomCenter;
    glm::vec3 customPivot{0};
    OutlineMode outline = OutlineMode::None;
    bool mergeCoplanarFaces = true; // within each OBJ object/group/material/smoothing segment

    bool allowObjFallback = true;
};

/// [역할] .obj 모델 로드 결과(성공 시 Model·피벗·경고, 실패 시 error 메시지)를 담는 반환 구조체.
struct ModelResult {
    std::optional<Model> model;
    glm::vec3 localBasis{0};
    PivotMode pivotMode = PivotMode::Preserve;
    std::string error;
    std::vector<std::string> warnings;
    bool usedFallback = false;

    /// [API] 모델 로드 성공 여부를 bool로 변환한다. 실패 시 error 문자열을 확인한다.
    explicit operator bool() const;
};

/// [역할] 프로젝트 상대경로의 .obj 3D 모델을 탐색하고 파싱하는 자산 로더.
class AssetLoader {
    std::filesystem::path projectRoot_;

    std::filesystem::path resolve(const std::filesystem::path& relative) const;

public:
    /// [API] 프로젝트 루트를 보관하고 상대경로 OBJ 로딩을 준비한다.
    explicit AssetLoader(std::filesystem::path projectRoot = detail::executableDirectory());

    /// [API] 프로젝트 상대경로의 .obj 파일을 파싱하여 모델·피벗·경고·오류 결과를 반환한다.
    ModelResult loadObj(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{}) const;
};

// --- 3-8. RenderConfig 선언 ------------------------------------------------------

/// [역할] GPU 업로드 전 CPU에서 표준화되는 정점 1개의 패킹 입력 구조체(위치·색·UV·법선).
struct RenderVertex {
    float position[3], color[3], uv[2], normal[3];
};

/// [역할] VAO의 단일 정점 속성(location·성분 수·GL 타입·정규화·바이트 오프셋) 기술자.
struct VertexAttribute {
    GLuint location = 0;
    GLint components = 3;
    GLenum type = GL_FLOAT;
    bool normalized = false;
    std::size_t offset = 0;
    bool integer = false; // ivec/uvec inputs use VertexAttribIPointer.
};

/// [역할] VBO 정점 간격(stride)과 정점 속성 배열을 정의하는 정점 입력 레이아웃.
struct VertexLayout {
    std::size_t stride = sizeof(RenderVertex);
    std::vector<VertexAttribute> attributes{
        {0, 3, GL_FLOAT, false, offsetof(RenderVertex, position)},
        {1, 3, GL_FLOAT, false, offsetof(RenderVertex, color)},
        {2, 2, GL_FLOAT, false, offsetof(RenderVertex, uv)},
        {3, 3, GL_FLOAT, false, offsetof(RenderVertex, normal)}};
};

/// [역할] 기본/외부 GLSL 셰이더의 내장 uniform 변수 이름 매핑 테이블.
struct UniformBindings {
    std::string mvp = "uMVP", model = "uModel", normal = "uNormalMatrix";
    std::string lighting = "uLighting";
    std::string lightDirection = "uLightDirection", lightColor = "uLightColor";
    std::string overrideColor = "uOverrideColor", outlineColor = "uOutlineColor", tint = "uTint";
    std::string pointLightCount = "uPointLightCount", pointLights = "uPointLights";
    std::string directionalIntensity = "uDirectionalIntensity";
    std::string viewPosition = "uViewPosition", ambientColor = "uAmbientColor";
    std::string materialAmbient = "uMaterialAmbient", materialDiffuse = "uMaterialDiffuse";
    std::string materialSpecular = "uMaterialSpecular", shininess = "uShininess";
};

/// [역할] 사용자 정의 bindUniforms 콜백에 전달되는 현재 드로우 문맥 및 uniform 설정 헬퍼.
struct DrawUniformContext {
    GLuint program;
    const Scene& scene;
    std::optional<EntityId> entity;
    std::optional<LineId> line;
    std::size_t face;
    glm::mat4 model, view, projection;
    bool depthOnly = false;

    /// [API] 현재 program의 추가 float uniform을 설정한다. 없는 이름이나 비유한 값은 거부한다.
    bool setUniform(const std::string& name, float value) const;

    /// [API] 현재 program의 추가 int/bool/sampler uniform을 설정한다.
    bool setUniform(const std::string& name, int value) const;

    /// [API] 현재 program의 추가 vec3 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::vec3& value) const;

    /// [API] 현재 program의 추가 mat4 uniform을 설정한다.
    bool setUniform(const std::string& name, const glm::mat4& value) const;
};

/// [역할] 커스텀 정점 패킹·VAO 레이아웃·추가 uniform 바인딩 콜백을 묶은 렌더 확장 설정.
struct RenderConfig {
    VertexLayout vertexLayout;
    UniformBindings uniforms;
    // Must output exactly input.size()*stride bytes, preserving vertex order/count.
    std::function<bool(const std::vector<RenderVertex>&, std::vector<std::uint8_t>&)> packVertices;
    // Runs with program bound, after built-in uniforms, before each range/pass.
    std::function<bool(const DrawUniformContext&)> bindUniforms;
};

// --- 3-9. Scene · DiagramSystem · Camera · Lighting · Renderer 선언 ---------------

/// [역할] 3D 엔티티·선분·부모 계층·재질·면 변환·모션 연결을 보관하는 CPU 장면 관리자.
class Scene {
private:
    std::vector<EntityId> polyId_;
    std::vector<Component_Poly> components_;
    std::vector<Line3D> lines_;
    std::uint64_t geometryRevision_ = 0;
    MotionSystem motions_;
    std::map<EntityId, LayerId> entityLayers_;

public:
    /// [API] 빈 CPU 장면과 ID·형상 변경 상태를 초기화한다.
    Scene();

    /// [API] 메시 없는 변환 전용 엔티티를 생성하여 부모·관절 노드로 사용한다.
    std::optional<EntityId> addGroup(const Transform3D& transform = Transform3D{});

    /// [API] 엔티티 전체에 곱할 색상 계수를 설정한다. 정점 자체의 색 배열과 구분한다.
    bool setTint(EntityId id, GlColor color);

    /// [API] 주어진 다면체와 변환을 Scene에 등록하고 새 EntityId를 반환한다.
    std::optional<EntityId> push_entity(const Polyhedron& polyhedron, const Transform3D& transform = Transform3D{});

    /// [API] 중심 기준 직육면체를 변 길이와 위치로 생성하여 Scene에 등록한다.
    std::optional<EntityId> addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);

    /// [API] 가로·높이·깊이에 맞는 피라미드를 생성하여 Scene에 등록한다.
    std::optional<EntityId> addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition);

    /// [API] 해당 엔티티의 한 면을 표시하거나 숨긴다. 컬링 조건과 별개의 가시성 상태다.
    bool setFaceVisible(EntityId id, std::size_t faceIndex, bool visible);

    /// [API] 형상 재업로드 없이 면 앞쪽의 부모 프레임을 설정한다. 렌더·피킹·법선 변환에 함께 반영한다.
    bool setFaceFrame(EntityId id, std::size_t faceIndex, const glm::mat4& matrix);

    /// [API] 해당 면의 앞면/뒷면 제거 정책을 설정한다. 조명 법선과 삼각형 winding을 구분한다.
    bool setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode);

    /// [API] 해당 면에 적용할 추가 로컬 애니메이션 행렬을 설정한다.
    bool setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix,
                            FaceAnimationSpace space = FaceAnimationSpace::Local);

    /// [API] 면의 정점별 색 배열을 교체한다. 정점 수와 색 배열 길이를 맞춰야 한다.
    bool setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors);

    /// [API] basis를 바꾸고 setTransform으로 유효성을 확인한다. 부모가 있으면 부모 좌표 기준이다.
    bool setPosition(EntityId id, const glm::vec3& worldPosition);

    /// [API] 기존 모델의 bounds와 새 크기 비율로 정점·면 피벗을 재계산한다. geometry revision이 바뀐다.
    bool setSideLengths(EntityId id, const glm::vec3& sideLengths);

    /// [API] 기존 엔티티의 최종 SRT를 바꾼다. 형상 정점 재생성과 GPU 재업로드는 별개의 작업이다.
    bool setTransform(EntityId id, const Transform3D& transform);

    /// [API] 부모 ID가 존재하고 순환이 없는지 확인한 뒤 계층 관계를 바꾼다. 월드 자세 보존은 setParentPreserveWorld 사용.
    /// 로컬 SRT는 유지된다. 새 부모 좌표계에서 해석되므로 월드 위치와 자세가 달라질 수 있다.
    bool setParent(EntityId id, std::optional<EntityId> parent);

    /// [API] 객체의 로컬 레이어 적용 전 전체 아핀 배치 행렬을 반환한다.
    std::optional<glm::mat4> placementMatrix(EntityId id) const;

    /// [API] 전단(shear)을 포함한 월드 기준 아핀 배치를 분해 없이 직접 설정한다.
    bool setWorldPlacement(EntityId id, const glm::mat4& world);

    /// [API] 현재 월드 배치와 로컬 레이어 목록을 유지한 채 부모 엔티티를 변경한다.
    bool setParentPreserveWorld(EntityId id, std::optional<EntityId> parent);

    /// [API] 기본 SRT 이전에 적용되는 부모 기준 아핀 부착(attachment) 행렬을 설정한다.
    bool setAttachment(EntityId id, const glm::mat4& matrix);

    /// [API] 객체 전용 순차 로컬 레이어 행렬 목록을 원자적으로 교체한다.
    bool setLocalLayers(EntityId id, const std::vector<glm::mat4>& layers);

    /// [API] 부모 체인과 모션 레이어를 합성한 엔티티의 최종 월드 행렬을 반환한다.
    std::optional<glm::mat4> worldMatrix(EntityId id) const;

    /// [API] 엔티티의 월드 행렬 뒤에 면 SRT와 localAnimationMatrix를 합성한 최종 면 월드 행렬을 반환한다.
    std::optional<glm::mat4> faceWorldMatrix(EntityId id, std::size_t faceIndex) const;

    /// [API] 선의 owner 엔티티를 따라 로컬 끝점을 월드로 옮길 행렬을 반환한다.
    std::optional<glm::mat4> lineWorldMatrix(LineId id) const;

    /// [API] Scene이 소유한 MotionSystem의 수정 가능한 참조를 반환한다.
    MotionSystem& getMotions();

    /// [API] Scene이 소유한 MotionSystem의 읽기 전용 참조를 반환한다.
    const MotionSystem& getMotions() const;

    /// [API] 엔티티를 기존 모션 레이어에 연결하여 월드 행렬 합성에 반영한다.
    bool bindEntity(EntityId id, LayerId layer);

    /// [API] 엔티티와 모션 레이어의 연결을 해제한다.
    bool unbindEntity(EntityId id);

    /// [API] 엔티티에 연결된 레이어 ID를 반환한다. 연결이 없으면 빈 optional이다.
    std::optional<LayerId> getEntityLayer(EntityId id) const;

    /// [API] Scene의 모션 시스템에 dt(초)를 전달한다. Application 루프에서는 자동 호출된다.
    bool updateMotions(double deltaSeconds);

    /// [API] 면 변환과 독립 변환 여부를 함께 설정한다. 부모 모델 변환과의 합성 정책을 바꾼다.
    bool setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence);

    /// [API] 면이 모델·계층을 따르거나 월드에 독립하도록 설정한다. 기본값은 전환 시 현재 자세 유지다.
    bool setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld = true);

    /// [API] CPU 모델을 Scene에 등록한다. 피벗 모드와 사용자 기준점으로 localBasis를 결정한다.
    std::optional<EntityId> addModel(const Model& model, const Transform3D& transform = Transform3D{},
                                     PivotMode pivot = PivotMode::BottomCenter,
                                     const glm::vec3& customPivot = glm::vec3(0));

    /// [API] 기존 엔티티의 모델을 교체한다. 형상 revision을 갱신한다.
    bool setModel(EntityId id, const Model& model);

    /// [API] 기존 모델의 지정 메시를 교체한다. 정점·인덱스 등 메시 유효성을 검사한다.
    bool setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh);

    /// [API] 지정 메시의 읽기 전용 비소유 포인터를 반환한다. 잘못된 ID/인덱스면 nullptr이다.
    const Mesh* getMesh(EntityId id, std::size_t meshIndex) const;

    /// [API] 메시의 정점과 삼각형 인덱스를 함께 교체한다.
    bool setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                         const std::vector<std::uint32_t>& triangles);

    /// [API] 메시의 지정 정점 위치를 수정하고 형상 변경을 알린다.
    bool setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position);

    /// [API] 메시의 삼각형 인덱스 배열을 교체한다. 범위와 세 개 단위 구성을 검사한다.
    bool setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices);

    /// [API] 메시의 정점별 UV 배열을 교체한다.
    bool setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs);

    /// [API] 메시의 정점별 조명 법선 배열을 교체한다. 컬링 winding은 바뀌지 않는다.
    bool setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals);

    /// [API] 메시의 색상과 조명 재질을 교체한다.
    bool setMaterial(EntityId id, std::size_t meshIndex, const Material& material);

    /// [API] 윤곽선 생성 방식과 선택적 명시 인덱스를 설정한다.
    bool setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                    const std::vector<std::uint32_t>& explicitEdges = std::vector<std::uint32_t>{});

    /// [API] 피벗 정책과 로컬 기준점을 설정한다. 위치 이동과 메시 자체 크기 변경을 구분한다.
    bool setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot = glm::vec3(0));

    /// [API] 엔티티의 표시 여부를 바꾼다. 엔티티나 충돌 데이터를 삭제하지 않는다.
    bool setVisible(EntityId id, bool visible);

    /// [API] 지정 면을 독립 변환 가능한 면으로 분리한다.
    bool splitFace(EntityId id, std::size_t faceIndex);

    /// [API] 면의 크기·회전·이동 변환을 설정한다.
    bool setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform);

    /// [API] 두 끝점의 선을 추가한다. owner가 있으면 해당 엔티티 변환을 따른다.
    std::optional<LineId> addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                  std::optional<EntityId> owner = std::nullopt);

    /// [API] 시작점과 방향/길이 벡터를 선의 두 끝점으로 변환하여 추가한다.
    std::optional<LineId> addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                    std::optional<EntityId> owner = std::nullopt);

    // mat3의 열이 X/Y/Z 기저 벡터다. 벡터를 정규화하지 않고 그대로 사용한다.
    /// [API] 기저 행렬의 열 벡터로 X/Y/Z 선 세 개를 추가하고 LineId 묶음을 반환한다.
    std::optional<std::array<LineId, 3>> addBasis(const glm::vec3& origin,
                                                  const glm::mat3& basisVectors = glm::mat3(1.0f),
                                                  const glm::vec3& axisLengths = glm::vec3(1.0f),
                                                  std::optional<EntityId> owner = std::nullopt);

    /// [API] 기존 선을 시작점과 방향/길이 벡터로 갱신한다.
    bool setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector);

    /// [API] Scene의 모델·선·연결·모션을 초기화한다. 엔티티 ID는 재사용되므로 저장한 ID를 갱신한다.
    void clear();

    /// [API] 현재 Scene의 엔티티 ID 배열을 읽기 전용 참조로 반환한다.
    const std::vector<EntityId>& getIds() const;

    /// [API] 엔티티 컴포넌트의 비소유 포인터를 반환한다. 유효하지 않은 ID면 nullptr이다.
    const Component_Poly* getComponent(EntityId id) const;

    /// [API] Scene의 선 데이터 배열을 읽기 전용 참조로 반환한다.
    const std::vector<Line3D>& getLines() const;

    /// [API] 렌더러가 GPU 재업로드 필요성을 비교하는 형상 변경 번호를 반환한다.
    std::uint64_t getGeometryRevision() const;
};

/// [역할] 육면체·사각뿔 CPU 모델 생성 및 모델/변환 유효성 검사를 제공하는 정적 유틸리티.
class DiagramSystem {
public:
    /// [API] 형상 종류와 변 길이로 CPU 다면체를 만든다. Scene 등록은 별도로 수행한다.
    static Polyhedron makePolyhedron(GLShape shape, const glm::vec3& sideLengths = glm::vec3(1.0f));

    /// [API] 다면체의 면·정점·인덱스 등 렌더 입력 조건을 검사한다.
    static bool validPolyhedron(const Polyhedron& polyhedron);

    /// [API] 변환의 유한성·회전 기저·배율 등 허용 조건을 검사한다.
    static bool validTransform(const Transform3D& transform);

    /// [API] T(basis)·R·S·T(-localBasis) 순서로 로컬 모델 행렬을 계산한다.
    static glm::mat4 modelMatrix(const Transform3D& transform);

    /// [API] 컴포넌트와 면의 변환·독립성 정책에 따라 면 모델 행렬을 계산한다.
    static glm::mat4 faceModelMatrix(const Component_Poly& component, const PlaneFigure& face);
};

/// [역할] 카메라 위치·주목점·상향 벡터·시야각·클립 평면·투영 모드를 보관하는 시점 구조체.
struct Camera {
    glm::vec3 position{2.8f, 2.0f, 5.0f};
    glm::vec3 target{0.0f};
    glm::vec3 up{0.0f, 1.0f, 0.0f};
    float fieldOfViewDegrees = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;
    bool orthographic = false;
    float orthographicHalfExtent = 1.0f; // 짧은 화면 축의 절반 길이.
};

/// [역할] 카메라 유효성 검사·아핀 프레임 부착·View 및 Projection 행렬 계산기.
class CameraSystem {
public:
    /// [API] 시점·주목점·up·투영/클립 범위가 유효한 카메라인지 검사한다.
    static bool validCamera(const Camera& camera);

    /// [API] 지역 카메라를 아핀 프레임에 부착한다. 시선/up을 직교화하며 퇴화하면 실패한다.
    static std::optional<Camera> attached(const Camera& local, const glm::mat4& frame);

    /// [API] 카메라 시점·주목점·up으로 월드→뷰 행렬을 계산한다.
    static glm::mat4 viewMatrix(const Camera& camera);

    /// [API] 카메라의 원근/직각 모드와 주어진 aspect로 투영 행렬을 계산한다.
    static glm::mat4 projectionMatrix(const Camera& camera, float aspect);
};

/// [역할] 월드 공간 점광원 1개의 위치·색상·강도·거리 감쇠 계수를 담는 구조체(최대 8개 지원).
struct PointLight {
    glm::vec3 position{0};
    GlColor color{1, 1, 1};
    float intensity = 1, attenuation = .02f;
};

/// [역할] 프레임 렌더링 시 적용할 깊이·와이어프레임·조명·배경색·컬링·점광원 옵션.
struct RenderOptions {
    bool depthTest = true;
    bool wireframe = false;
    bool lighting = false;
    glm::vec3 lightDirection{1, 1, 1};
    GlColor lightColor;
    GlColor outlineColor{0.05f, 0.05f, 0.05f};
    glm::vec4 background{0.12f, 0.12f, 0.16f, 1};
    CullMode cull = CullMode::None;      // Append to preserve existing aggregate initializers.
    float directionalIntensity = 1;      // 방향광 강도 (0 ~ 1)
    std::vector<PointLight> pointLights; // 내장 셰이더에서 최대 8개 월드 점광원 지원
    GlColor ambientColor{0, 0, 0};       // 독립 환경광 색상
};

/// [역할] 프레임버퍼 좌하단 기준 사각 영역(x, y, width, height)을 나타내는 뷰포트 구조체.
struct Viewport {
    int x = 0, y = 0, width = 0, height = 0;
};

/// [역할] 다중 뷰포트/다중 카메라 렌더링의 1개 패스(카메라·뷰포트·렌더 옵션·클리어 여부).
struct RenderPass {
    Camera camera;
    Viewport viewport;
    RenderOptions options;
    bool clearColor = true, clearDepth = true;
};

/// [역할] OpenGL 3.3 셰이더 프로그램·VAO/VBO/EBO 업로드 및 Scene 드로우를 실행하는 GPU 렌더러.
class Renderer {
    struct DrawRange {
        EntityId entityId = 0;
        std::size_t faceIndex = 0;
        std::optional<LineId> lineId;
        std::size_t firstIndex = 0, outlineFirstIndex = 0;
        GLsizei count = 0, outlineCount = 0;
    };

    RenderConfig config_;
    GLuint shaderProgramID_ = 0, vao_ = 0, vbo_ = 0, ebo_ = 0;
    GLint mvpLocation_ = -1, modelLocation_ = -1, normalLocation_ = -1;
    GLint lightingLocation_ = -1;
    GLint lightDirectionLocation_ = -1, lightColorLocation_ = -1;
    GLint overrideLocation_ = -1, outlineColorLocation_ = -1, tintLocation_ = -1;
    std::vector<DrawRange> drawRanges_;

    const Scene* uploadedScene_ = nullptr;
    std::uint64_t uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();

    bool initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    bool upload(const Scene& scene);
    bool draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
              const RenderOptions& options) const;

public:
    /// [API] GPU 핸들을 0과 미업로드 상태로 준비한다. 실제 GL 생성은 initialize에서 수행한다.
    Renderer() = default;

    /// [API] 자원 소유 타입이므로 복사 생성을 금지한다.
    Renderer(const Renderer&) = delete;

    /// [API] 자원 소유 타입이므로 복사 대입을 금지한다.
    Renderer& operator=(const Renderer&) = delete;

    // Requires a current OpenGL 3.3 context. Empty paths select embedded shaders.
    /// [API] 현재 OpenGL 3.3 컨텍스트에서 셰이더·프로그램·버퍼를 생성한다. 빈 경로면 내장 GLSL을 사용한다.
    bool initialize(const std::filesystem::path& vertexPath = std::filesystem::path{},
                    const std::filesystem::path& fragmentPath = std::filesystem::path{},
                    const RenderConfig& config = RenderConfig{});

    /// [API] Scene을 카메라와 전체 화면 크기로 렌더링한다. 유효한 현재 GL 컨텍스트가 필요하다.
    bool render(const Scene& scene, const Camera& camera, int width, int height,
                const RenderOptions& options = RenderOptions{});

    /// [API] viewport·scissor·깊이·컬링·clear를 설정하고 pass 크기로 aspect를 계산해 draw로 전달한다.
    bool renderPass(const Scene& scene, const RenderPass& pass, int framebufferWidth, int framebufferHeight);

    /// [API] 프레임 전체를 초기화한 뒤 여러 pass를 순서대로 그린다. 한 Scene과 한 WGL 컨텍스트를 공유한다.
    bool renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height);

    /// [API] GPU 핸들을 삭제하고 캐시를 비운다. 컨텍스트가 유효해야 한다.
    void destroy();

    /// [API] 컨텍스트 실패 후 GL 삭제 호출을 피하고 CPU 핸들/캐시를 비운다.
    void abandon();
};

// --- 3-10. Picking 선언 ----------------------------------------------------------

/// [역할] 시작점·단위 방향·최대 거리로 정의되는 3D 레이캐스트 광선.
struct Ray {
    glm::vec3 origin{0}, direction{0, 0, -1};
    float maxDistance = std::numeric_limits<float>::infinity();
};

/// [역할] 레이 피킹 교차 결과(맞은 엔티티·면·삼각형·월드/로컬 교점·무게중심·UV·거리).
struct PickResult {
    EntityId entityId = 0;
    std::size_t faceIndex = 0, triangleIndex = 0;
    glm::vec3 worldPoint{0}, localPoint{0}, barycentric{0};
    glm::vec2 uv{0};
    bool hasUV = false;
    float distance = 0; // Distance from ray origin (near plane for a screen ray).
};

/// [역할] 화면 좌상단 기준 프레임버퍼 픽셀 영역(x, y, width, height).
struct PickViewport {
    double x = 0, y = 0, width = 0, height = 0;
};

/// [API] 광선의 월드 교점을 구한다. 평행하거나, 광선 뒤 또는 최대 거리 밖이면 빈 결과를 반환한다.
inline std::optional<glm::vec3> intersectRayPlane(
    const Ray& ray, const glm::vec3& planePoint, const glm::vec3& planeNormal);

/// [API] 광선으로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Ray& input, CullMode inheritedCull = CullMode::None);

/// [API] 화면 좌표로 보이는 삼각형의 최근접 교차를 찾아 객체·면·위치·UV를 반환한다.
inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
                                      const PickViewport& viewport, CullMode inheritedCull = CullMode::None);

// --- 3-11. Application 선언 ------------------------------------------------------

/// [역할] 창 크기·제목·셰이더/프로젝트 경로·VSync·진단·렌더 확장 설정을 묶은 실행 구성 구조체.
struct ApplicationConfig {
    int width = WINDOW_WIDTH;
    int height = WINDOW_HEIGHT;
    std::string title = "OpenGL";

    std::filesystem::path shaderDirectory;                // Empty selects embedded shaders.
    std::filesystem::path projectRoot = ".";              // Relative to the executable.
    std::filesystem::path runtimeDirectory = "framework"; // Relative to projectRoot.

    bool verticalSync = true;
    DiagnosticOptions diagnostics;
    RenderConfig renderConfig;
};

/// [역할] 마우스 버튼 입력(버튼·액션·수정키·창/프레임버퍼 좌표)을 전달하는 이벤트 구조체.
struct MouseEvent {
    int button = 0, action = 0, mods = 0;
    double windowX = 0, windowY = 0, framebufferX = 0, framebufferY = 0;
};

/// [역할] GLFW 창·OpenGL 3.3 컨텍스트·입력 콜백·메인 루프·Scene·Renderer·OBJ 로더를 총괄하는 애플리케이션.
/// 콜백에 캡처한 객체는 run()이 끝날 때까지 유지하며, 한 스레드에서 하나만 사용한다.
class Application {
private:
    GLFWwindow* window_ = nullptr;
    bool glfwInitialized_ = false;
    bool running_ = false;
    DWORD contextThread_ = 0;
    bool contextFailed_ = false;
    void* nativeContext_ = nullptr;

    Scene scene_;
    Camera camera_;
    Renderer renderer_;
    RenderOptions renderOptions_;
    std::optional<AssetLoader> assets_;
    std::exception_ptr callbackError_;
    std::function<void(Application&, const MouseEvent&)> mouseHandler_;
    std::function<void(Application&, double, double)> cursorHandler_;
    std::function<bool(Application&)> initHandler_;
    std::function<void(Application&, float)> updateHandler_;
    std::function<void(Application&, int, int)> keyHandler_;
    std::function<void(Application&, int, int, int)> modifiedKeyHandler_;
    std::function<void(Application&, bool)> focusHandler_;
    std::function<bool(Application&)> renderHandler_;

    bool ensureContext(const char* operation, const char* file, int line);
    template <class Handler> void dispatch(Handler&& handler) noexcept;
    bool initialize(const ApplicationConfig& config);
    void shutdown();

public:
    /// [API] 초기 창 포인터와 콜백·CPU 장면 상태를 준비한다. 실제 창은 run에서 생성한다.
    Application() = default;

    /// [API] 실행 자원을 정리한다. GPU 해제는 창의 컨텍스트가 유효한 동안 수행한다.
    ~Application();

    /// [API] 자원 소유 타입이므로 복사 생성을 금지한다.
    Application(const Application&) = delete;

    /// [API] 자원 소유 타입이므로 복사 대입을 금지한다.
    Application& operator=(const Application&) = delete;

    /// [API] 장면 초기화 콜백을 등록한다. run의 창·렌더러 초기화 후 한 번 호출하며 false면 종료한다.
    void setInitHandler(std::function<bool(Application&)> handler);

    /// [API] 매 프레임 dt(초)를 받는 갱신 콜백을 등록한다. 렌더 패스마다 중복 갱신하지 않는다.
    void setUpdateHandler(std::function<void(Application&, float)> handler);

    /// [API] 키와 action을 받는 콜백을 등록한다. 수정키 콜백이 있으면 그 콜백을 우선한다.
    void setKeyHandler(std::function<void(Application&, int key, int action)> handler);

    /// [API] 키·action·mods를 받는 콜백을 등록하여 Shift 등 수정키를 처리한다.
    void setModifiedKeyHandler(std::function<void(Application&, int key, int action, int mods)> handler);

    /// [API] 창 포커스 변경 콜백을 등록한다. 지속 입력 상태 해제 등에 사용한다.
    void setFocusHandler(std::function<void(Application&, bool focused)> handler);

    /// [API] 사용자 렌더 콜백을 등록한다. 등록하면 기본 renderScene 대신 호출한다.
    void setRenderHandler(std::function<bool(Application&)> handler);

    /// [API] 마우스 버튼과 창/프레임버퍼 좌표를 받는 콜백을 등록한다.
    void setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler);

    /// [API] 창 좌표의 커서 이동 콜백을 등록한다. 드래그에는 이전 좌표와의 차이를 사용한다.
    void setCursorHandler(std::function<void(Application&, double x, double y)> handler);

    /// [API] 창 좌표를 기본 카메라의 월드 광선으로 변환한다. 전체 프레임버퍼를 사용한다.
    std::optional<Ray> screenRayAt(double windowX, double windowY) const;

    /// [API] 창 좌표를 framebuffer 좌표로 보정하고 기본 카메라의 전체 viewport로 ray picking을 수행한다.
    std::optional<PickResult> pickAt(double windowX, double windowY) const;

    /// [API] 현재 커서 위치를 읽어 기본 카메라의 장면 피킹 결과를 반환한다.
    std::optional<PickResult> pickAtCursor() const;

    /// [API] 실행 중 사용하는 AssetLoader 참조를 반환한다. run 콜백 밖에서는 예외를 발생시킨다.
    AssetLoader& getAssets();

    /// [API] 프로젝트 상대경로(예: "obj/cuboid.obj")의 .obj 파일을 파싱해 ModelResult로 반환한다.
    ModelResult loadObj(const std::filesystem::path& relative, const LoadOptions& options = LoadOptions{});

    /// [API] .obj 파일을 로드해 지정 Transform3D로 Scene에 등록하고 EntityId를 반환한다.
    std::optional<EntityId> addObj(const std::filesystem::path& relative,
                                   const Transform3D& transform = Transform3D{},
                                   const LoadOptions& options = LoadOptions{});

    /// [API] .obj 파일을 로드해 지정 월드/부모 위치(basis)에 Scene 엔티티로 간편 등록한다.
    std::optional<EntityId> addObjAt(const std::filesystem::path& relative,
                                     const glm::vec3& position,
                                     const LoadOptions& options = LoadOptions{});

    /// [API] 초기화 → 이벤트 → 사용자 update → Scene 모션 → 렌더 → swap을 소유하고 정리한다.
    int run(const ApplicationConfig& config = ApplicationConfig{});

    /// [API] 현재 컨텍스트에서 셰이더·정점 입력을 재생성한다. 실패하면 종료를 요청한다.
    bool configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath = {},
                           const std::filesystem::path& fragmentPath = {});

    /// [API] 창 닫기 플래그를 설정하여 이벤트 루프의 정상 종료를 요청한다.
    void requestClose();

    /// [API] 기본 카메라와 현재 프레임버퍼 크기로 Scene을 한 번 렌더링한다.
    bool renderScene();

    /// [API] 현재 창의 한 컨텍스트에서 여러 카메라 패스를 렌더링한다. 시간은 진행시키지 않는다.
    bool renderViews(const std::vector<RenderPass>& passes);

    /// [API] 현재 창의 실제 픽셀 크기를 반환한다. DPI 배율이 있으면 창 좌표 크기와 다를 수 있다.
    std::array<int, 2> getFramebufferSize() const;

    /// [API] 애플리케이션의 렌더 옵션을 수정할 수 있는 참조를 반환한다.
    RenderOptions& getRenderOptions();

    /// [API] 애플리케이션이 소유한 CPU Scene의 수정 가능한 참조를 반환한다.
    Scene& getScene();

    /// [API] 기본 카메라의 수정 가능한 참조를 반환한다.
    Camera& getCamera();

    /// [API] GLFW 창의 비소유 포인터를 반환한다. 창 수명 밖에서는 사용할 수 없다.
    GLFWwindow* getWindow() const;
};

} // namespace graphics

// ================================================================================
// [4부] 후방 정의부 (Definitions & Implementations)
// --------------------------------------------------------------------------------
// [3부]에서 선언한 모든 클래스 메서드·자유 함수·내부 유틸리티의 구현 본문입니다.
// ================================================================================

// --- Diagnostics.cpp (inline implementations) ---

namespace graphics {

inline DiagnosticOptions& Diagnostics::options() {
    static DiagnosticOptions value;
    return value;
}

inline std::optional<Diagnostic>& Diagnostics::issue() {
    static std::optional<Diagnostic> value;
    return value;
}

inline Diagnostic& Diagnostics::context() {
    static Diagnostic value;
    return value;
}

inline void Diagnostics::locate(const char* file, int line, const char* operation) {
    context().file = file;
    context().line = line;
    context().operation = operation;
}

inline bool Diagnostics::failAt(const char* file, int line, PipelineStage stage, const char* api,
                                const std::string& cause, const std::string& action,
                                std::optional<std::uint64_t> object) {
    const auto saved = context();

    locate(file, line, api);

    if (!saved.operation.empty())
        context().operation = saved.operation + " -> " + api;

    const bool result = fail(stage, api, cause, action, object);

    context() = saved;

    return result;
}

inline void Diagnostics::configure(DiagnosticOptions value) {
    options() = value;
}

inline const std::optional<Diagnostic>& Diagnostics::lastError() {
    return issue();
}

inline void Diagnostics::clear() {
    issue().reset();
}

inline bool Diagnostics::fail(PipelineStage stage, const char* api, const std::string& cause,
                              const std::string& action, std::optional<std::uint64_t> object) {
    issue() = Diagnostic{stage, api, cause, action, object};
    issue()->file = context().file;
    issue()->line = context().line;
    issue()->operation = context().operation;

    if (options().errors) {
        std::cerr << "[OpenGL][" << api << "] 오류";

        if (object)
            std::cerr << " 객체=" << *object;
        std::cerr << ": " << cause << "\n  조치: " << action << '\n';
        std::cerr << "  소스: " << context().file << ':' << context().line
                  << "\n  호출 경로: " << context().operation << '\n';
    }

    return false;
}

// --- Model & Core Helpers (inline implementations) ---

inline PlaneFigure PlaneFigure::fromVertices(const std::vector<Vertex>& vertices,
                                             const std::vector<std::uint32_t>& triangles) {
    PlaneFigure mesh;

    mesh.triangleIndices = triangles;

    for (const auto& vertex : vertices) {
        mesh.localVertices.push_back(vertex.position);
        mesh.vertexNormals.push_back(vertex.normal);
        mesh.texcoords.push_back(vertex.uv);
        mesh.vertexColors.push_back(vertex.color);
    }

    return mesh;
}

inline std::vector<Mesh>& Polyhedron::meshes() {
    return faces;
}

inline const std::vector<Mesh>& Polyhedron::meshes() const {
    return faces;
}

inline const std::vector<MotionStep>& Motion::getSteps() const {
    return steps_;
}

inline constexpr LayerId MotionSystem::rootLayer() {
    return 0;
}

inline ModelResult::operator bool() const {
    return model.has_value();
}

inline MotionSystem& Scene::getMotions() {
    return motions_;
}

inline const MotionSystem& Scene::getMotions() const {
    return motions_;
}

inline bool Scene::updateMotions(double deltaSeconds) {
    return motions_.update(deltaSeconds);
}

inline const std::vector<EntityId>& Scene::getIds() const {
    return polyId_;
}

inline const std::vector<Line3D>& Scene::getLines() const {
    return lines_;
}

inline std::uint64_t Scene::getGeometryRevision() const {
    return geometryRevision_;
}

inline void Renderer::abandon() {
    shaderProgramID_ = vao_ = vbo_ = ebo_ = 0;
    drawRanges_.clear();
    uploadedScene_ = nullptr;
    uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
}

template <class Handler> void Application::dispatch(Handler&& handler) noexcept {
    if (callbackError_)
        return;

    try {
        handler();
    } catch (...) {
        callbackError_ = std::current_exception();
        requestClose();
    }
}

inline RenderOptions& Application::getRenderOptions() {
    return renderOptions_;
}

inline Scene& Application::getScene() {
    return scene_;
}

inline Camera& Application::getCamera() {
    return camera_;
}

inline GLFWwindow* Application::getWindow() const {
    return window_;
}

namespace detail {

inline bool finiteVector(const glm::vec3& v) {
    return std::isfinite(v.x) && std::isfinite(v.y) && std::isfinite(v.z);
}

inline bool finiteMatrix(const glm::mat4& m) {
    for (int c = 0; c < 4; ++c)
        for (int r = 0; r < 4; ++r)
            if (!std::isfinite(m[c][r]))
                return false;

    return true;
}

inline bool validColor(GlColor c) {
    return std::isfinite(c.red) && std::isfinite(c.green) && std::isfinite(c.blue);
}

inline std::vector<std::uint32_t> triangles(const Mesh& mesh) {
    if (!mesh.triangleIndices.empty())
        return mesh.triangleIndices;

    if (mesh.shape == GLFace::Triangle)
        return {0, 1, 2};

    if (mesh.shape == GLFace::Rect)
        return {0, 1, 2, 0, 2, 3};

    return {};
}

inline bool validMesh(const Mesh& m) {
    if (m.cull != CullMode::Inherit && m.cull != CullMode::None && m.cull != CullMode::Back &&
        m.cull != CullMode::Front)
        return false;

    if ((m.transformMode != FaceTransformMode::Hierarchy && m.transformMode != FaceTransformMode::Independent) ||
        !finiteMatrix(m.transformFrame) ||
        (m.animationSpace != FaceAnimationSpace::Local && m.animationSpace != FaceAnimationSpace::Model))
        return false;

    if (m.localVertices.size() < 3 || !validColor(m.color) || !validColor(m.material.color) ||
        !finiteVector(m.normal) || !finiteMatrix(m.localAnimationMatrix) || !std::isfinite(m.creaseAngleDegrees) ||
        m.creaseAngleDegrees < 0 || m.creaseAngleDegrees > 180)
        return false;

    const auto& material = m.material;
    for (float value : {material.ambientStrength, material.diffuseStrength, material.specularStrength})
        if (!std::isfinite(value) || value < 0 || value > 1)
            return false;
    if (!std::isfinite(material.shininess) || material.shininess < 1 || material.shininess > 1024)
        return false;

    const auto count = m.localVertices.size();

    if (count > std::numeric_limits<std::uint32_t>::max())
        return false;

    if ((!m.vertexColors.empty() && m.vertexColors.size() != count) ||
        (!m.vertexNormals.empty() && m.vertexNormals.size() != count) ||
        (!m.texcoords.empty() && m.texcoords.size() != count))
        return false;

    for (const auto& v : m.localVertices)
        if (!finiteVector(v))
            return false;

    for (const auto& v : m.vertexNormals)
        if (!finiteVector(v))
            return false;

    for (const auto& v : m.texcoords)
        if (!std::isfinite(v.x) || !std::isfinite(v.y))
            return false;

    for (auto c : m.vertexColors)
        if (!validColor(c))
            return false;

    const auto indices = triangles(m);

    if (indices.empty() || indices.size() % 3 || m.outlineIndices.size() % 2)
        return false;

    for (auto i : indices)
        if (i >= count)
            return false;

    for (auto i : m.outlineIndices)
        if (i >= count)
            return false;

    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = glm::dvec3(m.localVertices[indices[i]]);
        const auto b = glm::dvec3(m.localVertices[indices[i + 1]]);
        const auto c = glm::dvec3(m.localVertices[indices[i + 2]]);
        const auto normal = glm::cross(b - a, c - a);
        const double area = glm::dot(normal, normal);

        if (!std::isfinite(area) || area <= 0)
            return false;
    }

    return true;
}

inline std::pair<glm::vec3, glm::vec3> bounds(const Model& model) {
    glm::vec3 low(std::numeric_limits<float>::max()), high(-std::numeric_limits<float>::max());

    for (const auto& face : model.faces)
        for (const auto& p : face.localVertices) {
            low = glm::min(low, p);
            high = glm::max(high, p);
        }

    return {low, high};
}

inline glm::vec3 bottomCenter(const Model& model) {
    const auto box = bounds(model);

    return {box.first.x * .5f + box.second.x * .5f, box.first.y, box.first.z * .5f + box.second.z * .5f};
}

inline glm::mat4 transformMatrix(const Transform3D& t) {
    glm::mat4 m = glm::translate(glm::mat4(1), t.basis);

    m *= glm::mat4(t.rotation);

    return glm::translate(glm::scale(m, t.scale), -t.localBasis);
}

inline glm::mat4 faceSrtMatrix(const Mesh& face) {
    return face.independence
        ? glm::translate(glm::mat4(1), face.transform.localBasis) * transformMatrix(face.transform)
        : glm::mat4(1);
}

inline glm::mat4 faceLocalMatrix(const Mesh& face) {
    const auto child = faceSrtMatrix(face);
    return face.animationSpace == FaceAnimationSpace::Model
        ? face.localAnimationMatrix * child : child * face.localAnimationMatrix;
}

inline glm::mat4 faceMatrix(const Component_Poly& c, const Mesh& face) {
    const auto parent = face.transformMode == FaceTransformMode::Hierarchy ? transformMatrix(c.transform) : glm::mat4(1);
    return parent * face.transformFrame * faceLocalMatrix(face);
}

} // namespace detail

// --- Geometry (inline implementations) ---

inline void recalculateNormals(Mesh& mesh) {
    mesh.vertexNormals.assign(mesh.localVertices.size(), glm::vec3(0));

    const auto indices = detail::triangles(mesh);

    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i + 1], c = indices[i + 2];
        const auto n =
            glm::cross(mesh.localVertices[b] - mesh.localVertices[a], mesh.localVertices[c] - mesh.localVertices[a]);

        mesh.vertexNormals[a] += n;
        mesh.vertexNormals[b] += n;
        mesh.vertexNormals[c] += n;
    }

    for (auto& n : mesh.vertexNormals) {
        const float size = glm::length(n);

        n = size > 1e-12f ? n / size : glm::vec3(0);
    }
}

inline std::vector<std::uint32_t> meshOutline(const Mesh& mesh, OutlineMode mode) {
    if (mode == OutlineMode::None)
        return {};

    if (mode == OutlineMode::Explicit)
        return mesh.outlineIndices;
    using Position = std::array<float, 3>;

    std::map<Position, std::uint32_t> points;
    std::vector<std::uint32_t> welded;

    for (std::uint32_t i = 0; i < mesh.localVertices.size(); ++i) {
        const auto& p = mesh.localVertices[i];
        const auto result = points.emplace(Position{p.x, p.y, p.z}, i);

        welded.push_back(result.first->second);
    }

    struct Edge {
        std::uint32_t a = 0, b = 0;
        std::vector<glm::vec3> normals;
    };

    std::map<std::pair<std::uint32_t, std::uint32_t>, Edge> edges;
    const auto indices = detail::triangles(mesh);

    for (std::size_t i = 0; i < indices.size(); i += 3) {
        const auto a = indices[i], b = indices[i + 1], c = indices[i + 2];
        auto normal =
            glm::cross(mesh.localVertices[b] - mesh.localVertices[a], mesh.localVertices[c] - mesh.localVertices[a]);
        const auto length = glm::length(normal);

        if (length <= 1e-12f)
            continue;
        normal /= length;

        const std::array<std::uint32_t, 3> corners{a, b, c};

        for (int e = 0; e < 3; ++e) {
            auto first = welded[corners[e]], second = welded[corners[(e + 1) % 3]];

            if (first == second)
                continue;

            if (first > second)
                std::swap(first, second);

            auto& edge = edges[{first, second}];

            edge.a = first;
            edge.b = second;
            edge.normals.push_back(normal);
        }
    }

    const float threshold = std::cos(glm::radians(mesh.creaseAngleDegrees));
    std::vector<std::uint32_t> result;

    for (const auto& item : edges) {
        const auto& edge = item.second;
        const bool show = mode == OutlineMode::AllTriangleEdges || edge.normals.size() != 2 ||
                          glm::dot(edge.normals[0], edge.normals[1]) < threshold;

        if (show) {
            result.push_back(edge.a);
            result.push_back(edge.b);
        }
    }

    return result;
}

namespace detail {

inline std::vector<std::uint32_t> triangulate(const std::vector<glm::vec3>& points) {
    if (points.size() < 3)
        throw std::runtime_error("Face has fewer than three corners.");

    glm::vec3 normal(0);

    for (std::size_t i = 0; i < points.size(); ++i) {
        const auto& a = points[i];
        const auto& b = points[(i + 1) % points.size()];

        normal += glm::vec3((a.y - b.y) * (a.z + b.z), (a.z - b.z) * (a.x + b.x), (a.x - b.x) * (a.y + b.y));
    }

    if (glm::length(normal) < 1e-12f)
        throw std::runtime_error("Degenerate polygon.");

    const auto absNormal = glm::abs(normal);
    const int axis =
        absNormal.x > absNormal.y ? (absNormal.x > absNormal.z ? 0 : 2) : (absNormal.y > absNormal.z ? 1 : 2);
    std::vector<glm::dvec2> projected;

    for (const auto& p : points)
        projected.push_back(axis == 0 ? glm::dvec2(p.y, p.z) : axis == 1 ? glm::dvec2(p.x, p.z) : glm::dvec2(p.x, p.y));

    double area = 0;

    for (std::size_t i = 0; i < projected.size(); ++i) {
        const auto& a = projected[i];
        const auto& b = projected[(i + 1) % projected.size()];

        area += a.x * b.y - b.x * a.y;
    }

    if (std::abs(area) < 1e-14)
        throw std::runtime_error("Degenerate polygon projection.");

    const double direction = area > 0 ? 1 : -1;
    auto cross = [](glm::dvec2 a, glm::dvec2 b, glm::dvec2 c) {
        return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    };

    std::vector<std::uint32_t> polygon(points.size()), result;

    std::iota(polygon.begin(), polygon.end(), 0);

    while (polygon.size() > 3) {
        bool clipped = false;

        for (std::size_t i = 0; i < polygon.size(); ++i) {
            const auto a = polygon[(i + polygon.size() - 1) % polygon.size()], b = polygon[i],
                       c = polygon[(i + 1) % polygon.size()];

            if (direction * cross(projected[a], projected[b], projected[c]) <= 1e-14)
                continue;

            bool inside = false;

            for (auto p : polygon)
                if (p != a && p != b && p != c &&
                    direction * cross(projected[a], projected[b], projected[p]) >= -1e-14 &&
                    direction * cross(projected[b], projected[c], projected[p]) >= -1e-14 &&
                    direction * cross(projected[c], projected[a], projected[p]) >= -1e-14) {
                    inside = true;
                    break;
                }

            if (inside)
                continue;
            result.insert(result.end(), {a, b, c});
            polygon.erase(polygon.begin() + i);
            clipped = true;
            break;
        }

        if (!clipped)
            throw std::runtime_error("Cannot triangulate polygon; check repeated corners or self-intersections.");
    }
    result.insert(result.end(), polygon.begin(), polygon.end());

    return result;
}

inline void appendMesh(Mesh& destination, const Mesh& source) {
    const auto first = std::uint32_t(destination.localVertices.size());

    destination.localVertices.insert(destination.localVertices.end(), source.localVertices.begin(),
                                     source.localVertices.end());
    destination.vertexNormals.insert(destination.vertexNormals.end(), source.vertexNormals.begin(),
                                     source.vertexNormals.end());
    destination.texcoords.insert(destination.texcoords.end(), source.texcoords.begin(), source.texcoords.end());

    for (auto i : triangles(source))
        destination.triangleIndices.push_back(first + i);
    destination.shape = GLFace::Mesh;
}

inline bool shareEdge(const Mesh& a, const Mesh& b) {
    const auto ae = meshOutline(a), be = meshOutline(b);

    for (std::size_t i = 0; i < ae.size(); i += 2)
        for (std::size_t j = 0; j < be.size(); j += 2) {
            const auto p = a.localVertices[ae[i]], q = a.localVertices[ae[i + 1]], r = b.localVertices[be[j]],
                       s = b.localVertices[be[j + 1]];

            if ((p == r && q == s) || (p == s && q == r))
                return true;
        }

    return false;
}

inline void mergeCoplanar(Model& model) {
    for (std::size_t i = 0; i < model.faces.size(); ++i) {
        auto& face = model.faces[i];

        if (glm::length(face.normal) < 1e-12f)
            continue;

        for (std::size_t j = i + 1; j < model.faces.size();) {
            const auto& other = model.faces[j];
            const bool samePlane =
                glm::dot(face.normal, other.normal) > 1 - 1e-6f &&
                std::abs(glm::dot(face.normal, other.localVertices[0] - face.localVertices[0])) < 1e-6f;

            if (samePlane && face.texcoords.empty() == other.texcoords.empty() && shareEdge(face, other)) {
                appendMesh(face, other);
                model.faces.erase(model.faces.begin() + j);
                j = i + 1;
            } else
                ++j;
        }
    }
}

// --- Path · Runtime DLL · GL Error Helpers ---

inline std::filesystem::path executableDirectory() {
    std::wstring path(32768, L'\0');
    const DWORD size = GetModuleFileNameW(nullptr, path.data(), DWORD(path.size()));
    if (!size || size >= path.size())
        throw std::runtime_error("Cannot locate the executable directory.");
    path.resize(size);
    return std::filesystem::path(path).parent_path();
}

inline std::filesystem::path sourceDirectory() {
    try {
#if defined(_MSVC_EXECUTION_CHARACTER_SET) && _MSVC_EXECUTION_CHARACTER_SET == 65001
#if defined(__cpp_char8_t)
        return std::filesystem::path(reinterpret_cast<const char8_t*>(__FILE__)).parent_path();
#else
        return std::filesystem::u8path(__FILE__).parent_path();
#endif
#else
        return std::filesystem::path(__FILE__).parent_path();
#endif
    } catch (...) {
        return {};
    }
}

inline ModelResult finishLoad(Model model, const LoadOptions& options) {
    if (model.faces.empty())
        throw std::runtime_error("Model contains no triangle meshes.");

    for (auto& face : model.faces) {
        face.outline = options.outline;

        if (!validMesh(face))
            throw std::runtime_error("Invalid model mesh data.");
    }

    ModelResult result;

    if (options.pivot == PivotMode::BottomCenter)
        result.localBasis = bottomCenter(model);
    else if (options.pivot == PivotMode::Custom)
        result.localBasis = options.customPivot;

    if (!finiteVector(result.localBasis))
        throw std::runtime_error("Invalid local basis.");
    result.pivotMode = options.pivot;
    result.model = std::move(model);

    return result;
}

inline std::filesystem::path relativePathInside(const std::filesystem::path& root,
                                                const std::filesystem::path& relative) {
    if (relative.empty() || relative.is_absolute() || relative.has_root_name() || relative.has_root_directory())
        throw std::runtime_error("Use a relative path inside the project.");
    const auto base = std::filesystem::weakly_canonical(root);
    const auto full = std::filesystem::weakly_canonical(base / relative);
    auto a = base.begin(), b = full.begin();
    for (; a != base.end(); ++a, ++b)
        if (b == full.end() || *a != *b)
            throw std::runtime_error("Path leaves the project directory.");
    return full;
}

inline std::filesystem::path resolveProjectRelative(const std::filesystem::path& primaryRoot,
                                                    const std::filesystem::path& relative) {
    if (relative.empty() || relative.is_absolute() || relative.has_root_name() || relative.has_root_directory())
        throw std::runtime_error("Asset path must be project-relative.");

    auto tryInside = [&](const std::filesystem::path& base) -> std::optional<std::filesystem::path> {
        std::error_code ec;
        const auto root = std::filesystem::weakly_canonical(base, ec);
        if (ec)
            return std::nullopt;
        const auto full = std::filesystem::weakly_canonical(root / relative, ec);
        if (ec)
            return std::nullopt;
        auto a = root.begin(), b = full.begin();
        for (; a != root.end(); ++a, ++b)
            if (b == full.end() || *a != *b)
                return std::nullopt;
        return full;
    };

    const auto primary = tryInside(primaryRoot);
    if (!primary)
        throw std::runtime_error("Asset path is outside projectRoot.");

    std::error_code ec;
    if (std::filesystem::exists(*primary, ec) && !ec)
        return *primary;

    // Visual Studio 기본 실행(x64/Debug/*.exe) 시 프로젝트 폴더(obj/, shaders/ 포함)도 자동 탐색한다.
    std::vector<std::filesystem::path> fallbacks;
    const auto cwd = std::filesystem::current_path(ec);
    if (!ec && !cwd.empty())
        fallbacks.push_back(cwd);

    const auto srcDir = sourceDirectory();
    if (!srcDir.empty())
        fallbacks.push_back(srcDir);

    if (primaryRoot.has_parent_path()) {
        const auto parent = primaryRoot.parent_path();
        fallbacks.push_back(parent);
        if (parent.has_parent_path())
            fallbacks.push_back(parent.parent_path());
    }

    for (const auto& root : fallbacks) {
        const auto candidate = tryInside(root);
        ec.clear();
        if (candidate && std::filesystem::exists(*candidate, ec) && !ec)
            return *candidate;
    }

    return *primary;
}

inline bool checkGlAt(const char* file, int line, PipelineStage stage, const char* api) {
    bool success = true;
    for (GLenum code = glGetError(); code != GL_NO_ERROR; code = glGetError()) {
        const char* cause =
            code == GL_INVALID_ENUM                    ? "GL_INVALID_ENUM: unsupported enum"
            : code == GL_INVALID_VALUE                 ? "GL_INVALID_VALUE: value outside allowed range"
            : code == GL_INVALID_OPERATION             ? "GL_INVALID_OPERATION: incompatible binding, state or program"
            : code == GL_OUT_OF_MEMORY                 ? "GL_OUT_OF_MEMORY: GPU allocation failed"
            : code == GL_INVALID_FRAMEBUFFER_OPERATION ? "GL_INVALID_FRAMEBUFFER_OPERATION: incomplete framebuffer"
                                                       : "OpenGL error";
        Diagnostics::failAt(
            file, line, stage, api, std::string(cause) + " (" + std::to_string(code) + ")",
            "Inspect this stage's bindings, buffer sizes, vertex layout, shader log and current context. A GL error does not identify a single root cause.");
        success = false;
    }
    return success;
}

} // namespace detail

// --- AssetLoader (inline methods) ---

inline AssetLoader::AssetLoader(std::filesystem::path projectRoot)
    : projectRoot_(std::filesystem::absolute(projectRoot)) {}

inline std::filesystem::path AssetLoader::resolve(const std::filesystem::path& relative) const {
    return detail::resolveProjectRelative(projectRoot_, relative);
}

// --- RenderConfig (DrawUniformContext inline methods) ---

inline bool DrawUniformContext::setUniform(const std::string& name, float value) const {
    const auto location = glGetUniformLocation(program, name.c_str());
    if (location < 0 || !std::isfinite(value))
        return false;
    glUniform1f(location, value);
    return true;
}

inline bool DrawUniformContext::setUniform(const std::string& name, int value) const {
    const auto location = glGetUniformLocation(program, name.c_str());
    if (location < 0)
        return false;
    glUniform1i(location, value);
    return true;
}

inline bool DrawUniformContext::setUniform(const std::string& name, const glm::vec3& value) const {
    const auto location = glGetUniformLocation(program, name.c_str());
    if (location < 0 || !detail::finiteVector(value))
        return false;
    glUniform3f(location, value.x, value.y, value.z);
    return true;
}

inline bool DrawUniformContext::setUniform(const std::string& name, const glm::mat4& value) const {
    const auto location = glGetUniformLocation(program, name.c_str());
    if (location < 0 || !detail::finiteMatrix(value))
        return false;
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
    return true;
}

// --- Picking (inline implementations) ---

inline std::optional<Ray> screenRay(const Camera& camera, double x, double y, const PickViewport& viewport) {
    if (!CameraSystem::validCamera(camera) || !std::isfinite(x) || !std::isfinite(y) || !std::isfinite(viewport.x) ||
        !std::isfinite(viewport.y) || !std::isfinite(viewport.width) || !std::isfinite(viewport.height) ||
        viewport.width <= 0 || viewport.height <= 0 || x < viewport.x || y < viewport.y ||
        x >= viewport.x + viewport.width || y >= viewport.y + viewport.height)
        return {};

    const auto pv = CameraSystem::projectionMatrix(camera, float(viewport.width / viewport.height)) *
                    CameraSystem::viewMatrix(camera);
    const auto determinant = glm::determinant(pv);

    if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
        return {};

    const auto inverse = glm::inverse(pv);
    const float nx = float(2 * (x - viewport.x) / viewport.width - 1),
                ny = float(1 - 2 * (y - viewport.y) / viewport.height);
    auto nearPoint = inverse * glm::vec4(nx, ny, -1, 1), farPoint = inverse * glm::vec4(nx, ny, 1, 1);

    if (std::abs(nearPoint.w) < 1e-12f || std::abs(farPoint.w) < 1e-12f)
        return {};
    nearPoint /= nearPoint.w;
    farPoint /= farPoint.w;

    const auto direction = glm::vec3(farPoint - nearPoint);
    const auto length = glm::length(direction);

    if (!detail::finiteVector(glm::vec3(nearPoint)) || !detail::finiteVector(direction) || !std::isfinite(length) ||
        length <= 0)
        return {};

    return Ray{glm::vec3(nearPoint), direction / length, length};
}

inline std::optional<glm::vec3> intersectRayPlane(
    const Ray& ray, const glm::vec3& planePoint, const glm::vec3& planeNormal) {
    if (!detail::finiteVector(ray.origin) || !detail::finiteVector(planePoint) ||
        std::isnan(ray.maxDistance) || ray.maxDistance < 0.0f)
        return {};

    const auto direction = detail::motionUnitVector(ray.direction);
    const auto normal = detail::motionUnitVector(planeNormal);
    if (!direction || !normal)
        return {};

    const float denominator = glm::dot(*direction, *normal);
    if (std::abs(denominator) <= 1e-6f)
        return {};

    const float distance = glm::dot(planePoint - ray.origin, *normal) / denominator;
    if (!std::isfinite(distance) || distance < 0.0f || distance > ray.maxDistance)
        return {};

    const auto point = ray.origin + distance * *direction;
    return detail::finiteVector(point) ? std::optional<glm::vec3>{point} : std::nullopt;
}

struct Aabb {
    glm::vec3 minimum;
    glm::vec3 maximum;
};

/// [API] 그룹과 하위 엔티티의 면 정점을 포함하는 월드 AABB를 계산한다.
inline std::optional<Aabb> groupWorldAabb(const Scene& scene, EntityId root, bool includeDescendants = true) {
    if (!scene.getComponent(root))
        return std::nullopt;

    std::optional<Aabb> bounds;
    for (EntityId id : scene.getIds()) {
        if (!includeDescendants && id != root)
            continue;

        const auto* component = scene.getComponent(id);
        if (!component || component->polyhedron.faces.empty())
            continue;

        const auto* ancestor = component;
        EntityId ancestorId = id;
        while (ancestor && ancestorId != root && ancestor->parent) {
            ancestorId = *ancestor->parent;
            ancestor = scene.getComponent(ancestorId);
        }
        if (!ancestor || ancestorId != root)
            continue;

        for (std::size_t faceIndex = 0; faceIndex < component->polyhedron.faces.size(); ++faceIndex) {
            const auto matrix = scene.faceWorldMatrix(id, faceIndex);
            if (!matrix)
                return std::nullopt;

            for (const auto& vertex : component->polyhedron.faces[faceIndex].localVertices) {
                const glm::vec3 point(*matrix * glm::vec4(vertex, 1.0f));
                if (!detail::finiteVector(point))
                    return std::nullopt;
                if (!bounds)
                    bounds = Aabb{point, point};
                else {
                    bounds->minimum = glm::min(bounds->minimum, point);
                    bounds->maximum = glm::max(bounds->maximum, point);
                }
            }
        }
    }
    return bounds;
}

/// [API] 두 월드 AABB가 겹치거나 맞닿으면 true를 반환한다.
inline bool overlaps(const Aabb& a, const Aabb& b) {
    return glm::all(glm::lessThanEqual(a.minimum, b.maximum)) &&
           glm::all(glm::lessThanEqual(b.minimum, a.maximum));
}

inline std::optional<PickResult> pick(const Scene& scene, const Ray& input, CullMode inheritedCull) {
    if (!detail::finiteVector(input.origin) || !detail::finiteVector(input.direction) ||
        !std::isfinite(glm::length(input.direction)) || glm::length(input.direction) < 1e-12f ||
        std::isnan(input.maxDistance) || input.maxDistance < 0)
        return {};

    const auto direction = glm::dvec3(glm::normalize(input.direction));
    const auto origin = glm::dvec3(input.origin);
    double nearest = input.maxDistance;
    std::optional<PickResult> result;

    for (auto id : scene.getIds()) {
        const auto* component = scene.getComponent(id);

        if (!component || !component->visible)
            continue;

        for (std::size_t f = 0; f < component->polyhedron.faces.size(); ++f) {
            const auto& face = component->polyhedron.faces[f];

            if (!face.visible)
                continue;

            const auto world = scene.faceWorldMatrix(id, f);

            if (!world)
                continue;

            const auto matrix = *world;
            const auto cull = face.cull == CullMode::Inherit ? inheritedCull : face.cull;
            const bool mirrored = glm::determinant(glm::mat3(matrix)) < 0;
            const auto indices = detail::triangles(face);

            for (std::size_t t = 0; t < indices.size(); t += 3) {
                const auto ia = indices[t], ib = indices[t + 1], ic = indices[t + 2];
                const auto a4 = matrix * glm::vec4(face.localVertices[ia], 1),
                           b4 = matrix * glm::vec4(face.localVertices[ib], 1),
                           c4 = matrix * glm::vec4(face.localVertices[ic], 1);

                if (std::abs(a4.w) < 1e-12f || std::abs(b4.w) < 1e-12f || std::abs(c4.w) < 1e-12f)
                    continue;

                const auto a = glm::dvec3(a4 / a4.w), b = glm::dvec3(b4 / b4.w), c = glm::dvec3(c4 / c4.w);
                const auto edge1 = b - a, edge2 = c - a, p = glm::cross(direction, edge2);
                const auto determinant = glm::dot(edge1, p);
                const auto tolerance = 1e-12 * glm::length(edge1) * glm::length(edge2);

                if (!std::isfinite(determinant) || std::abs(determinant) <= tolerance)
                    continue;

                const bool frontFacing = (determinant > 0) != mirrored;
                if ((cull == CullMode::Front && frontFacing) || (cull == CullMode::Back && !frontFacing))
                    continue;

                const auto relative = origin - a;
                const auto u = glm::dot(relative, p) / determinant;

                if (u < 0 || u > 1)
                    continue;

                const auto q = glm::cross(relative, edge1);
                const auto v = glm::dot(direction, q) / determinant;

                if (v < 0 || u + v > 1)
                    continue;

                const auto distance = glm::dot(edge2, q) / determinant;

                if (!std::isfinite(distance) || distance < 0 || distance > nearest)
                    continue;

                const glm::dvec3 weights((1 - u - v) / a4.w, u / b4.w, v / c4.w);
                const auto sum = weights.x + weights.y + weights.z;

                if (std::abs(sum) < 1e-12)
                    continue;

                PickResult hit;

                hit.entityId = id;
                hit.faceIndex = f;
                hit.triangleIndex = t / 3;
                hit.distance = float(distance);
                hit.worldPoint = glm::vec3(origin + direction * distance);
                hit.barycentric = glm::vec3(weights / sum);
                hit.localPoint = face.localVertices[ia] * hit.barycentric.x +
                                 face.localVertices[ib] * hit.barycentric.y +
                                 face.localVertices[ic] * hit.barycentric.z;
                hit.hasUV = !face.texcoords.empty();

                if (hit.hasUV)
                    hit.uv = face.texcoords[ia] * hit.barycentric.x + face.texcoords[ib] * hit.barycentric.y +
                             face.texcoords[ic] * hit.barycentric.z;
                nearest = distance;
                result = hit;
            }
        }
    }

    return result;
}

inline std::optional<PickResult> pick(const Scene& scene, const Camera& camera, double x, double y,
                                      const PickViewport& viewport, CullMode inheritedCull) {
    const auto ray = screenRay(camera, x, y, viewport);

    return ray ? pick(scene, *ray, inheritedCull) : std::nullopt;
}

} // namespace graphics

// --- Embedded GLEW Runtime Loader ---

extern "C" {
GLboolean glewExperimental = GL_FALSE;
PFNGLATTACHSHADERPROC __glewAttachShader = nullptr;
PFNGLBINDBUFFERPROC __glewBindBuffer = nullptr;
PFNGLBINDVERTEXARRAYPROC __glewBindVertexArray = nullptr;
PFNGLBUFFERDATAPROC __glewBufferData = nullptr;
PFNGLCOMPILESHADERPROC __glewCompileShader = nullptr;
PFNGLCREATEPROGRAMPROC __glewCreateProgram = nullptr;
PFNGLCREATESHADERPROC __glewCreateShader = nullptr;
PFNGLDELETEBUFFERSPROC __glewDeleteBuffers = nullptr;
PFNGLDELETEPROGRAMPROC __glewDeleteProgram = nullptr;
PFNGLDELETESHADERPROC __glewDeleteShader = nullptr;
PFNGLDELETEVERTEXARRAYSPROC __glewDeleteVertexArrays = nullptr;
PFNGLENABLEVERTEXATTRIBARRAYPROC __glewEnableVertexAttribArray = nullptr;
PFNGLGENBUFFERSPROC __glewGenBuffers = nullptr;
PFNGLGENVERTEXARRAYSPROC __glewGenVertexArrays = nullptr;
PFNGLGETPROGRAMINFOLOGPROC __glewGetProgramInfoLog = nullptr;
PFNGLGETPROGRAMIVPROC __glewGetProgramiv = nullptr;
PFNGLGETSHADERINFOLOGPROC __glewGetShaderInfoLog = nullptr;
PFNGLGETSHADERIVPROC __glewGetShaderiv = nullptr;
PFNGLGETUNIFORMLOCATIONPROC __glewGetUniformLocation = nullptr;
PFNGLLINKPROGRAMPROC __glewLinkProgram = nullptr;
PFNGLSHADERSOURCEPROC __glewShaderSource = nullptr;
PFNGLUNIFORM1FPROC __glewUniform1f = nullptr;
PFNGLUNIFORM1IPROC __glewUniform1i = nullptr;
PFNGLUNIFORM3FPROC __glewUniform3f = nullptr;
PFNGLUNIFORMMATRIX3FVPROC __glewUniformMatrix3fv = nullptr;
PFNGLUNIFORMMATRIX4FVPROC __glewUniformMatrix4fv = nullptr;
PFNGLUSEPROGRAMPROC __glewUseProgram = nullptr;
PFNGLVERTEXATTRIBIPOINTERPROC __glewVertexAttribIPointer = nullptr;
PFNGLVERTEXATTRIBPOINTERPROC __glewVertexAttribPointer = nullptr;

GLenum GLEWAPIENTRY glewInit(void) {
    struct Entry {
        void** target;
        const char* name;
    };
    const Entry entries[] = {
        {reinterpret_cast<void**>(&__glewAttachShader), "glAttachShader"},
        {reinterpret_cast<void**>(&__glewBindBuffer), "glBindBuffer"},
        {reinterpret_cast<void**>(&__glewBindVertexArray), "glBindVertexArray"},
        {reinterpret_cast<void**>(&__glewBufferData), "glBufferData"},
        {reinterpret_cast<void**>(&__glewCompileShader), "glCompileShader"},
        {reinterpret_cast<void**>(&__glewCreateProgram), "glCreateProgram"},
        {reinterpret_cast<void**>(&__glewCreateShader), "glCreateShader"},
        {reinterpret_cast<void**>(&__glewDeleteBuffers), "glDeleteBuffers"},
        {reinterpret_cast<void**>(&__glewDeleteProgram), "glDeleteProgram"},
        {reinterpret_cast<void**>(&__glewDeleteShader), "glDeleteShader"},
        {reinterpret_cast<void**>(&__glewDeleteVertexArrays), "glDeleteVertexArrays"},
        {reinterpret_cast<void**>(&__glewEnableVertexAttribArray), "glEnableVertexAttribArray"},
        {reinterpret_cast<void**>(&__glewGenBuffers), "glGenBuffers"},
        {reinterpret_cast<void**>(&__glewGenVertexArrays), "glGenVertexArrays"},
        {reinterpret_cast<void**>(&__glewGetProgramInfoLog), "glGetProgramInfoLog"},
        {reinterpret_cast<void**>(&__glewGetProgramiv), "glGetProgramiv"},
        {reinterpret_cast<void**>(&__glewGetShaderInfoLog), "glGetShaderInfoLog"},
        {reinterpret_cast<void**>(&__glewGetShaderiv), "glGetShaderiv"},
        {reinterpret_cast<void**>(&__glewGetUniformLocation), "glGetUniformLocation"},
        {reinterpret_cast<void**>(&__glewLinkProgram), "glLinkProgram"},
        {reinterpret_cast<void**>(&__glewShaderSource), "glShaderSource"},
        {reinterpret_cast<void**>(&__glewUniform1f), "glUniform1f"},
        {reinterpret_cast<void**>(&__glewUniform1i), "glUniform1i"},
        {reinterpret_cast<void**>(&__glewUniform3f), "glUniform3f"},
        {reinterpret_cast<void**>(&__glewUniformMatrix3fv), "glUniformMatrix3fv"},
        {reinterpret_cast<void**>(&__glewUniformMatrix4fv), "glUniformMatrix4fv"},
        {reinterpret_cast<void**>(&__glewUseProgram), "glUseProgram"},
        {reinterpret_cast<void**>(&__glewVertexAttribIPointer), "glVertexAttribIPointer"},
        {reinterpret_cast<void**>(&__glewVertexAttribPointer), "glVertexAttribPointer"},
    };
    for (const auto& entry : entries) {
        void* proc = reinterpret_cast<void*>(glfwGetProcAddress(entry.name));
        if (!proc) {
            proc = reinterpret_cast<void*>(wglGetProcAddress(entry.name));
            const auto code = reinterpret_cast<std::uintptr_t>(proc);
            if (code == 0 || code == 1 || code == 2 || code == 3 || code == static_cast<std::uintptr_t>(-1))
                proc = nullptr;
        }
        *entry.target = proc;
        if (!proc)
            return GLEW_ERROR_NO_GL_VERSION;
    }
    return GLEW_OK;
}
} // extern "C"

// --- Transform.cpp ---

namespace graphics {
namespace detail {
std::optional<glm::vec3> motionUnitVector(const glm::vec3& direction) {
    if (!finiteVector(direction))
        return {};

    const double length = glm::length(glm::dvec3(direction));

    if (!std::isfinite(length) || length <= 1e-12)
        return {};

    return glm::vec3(glm::dvec3(direction) / length);
}

std::optional<glm::mat4> motionAround(const glm::vec3& center, const glm::mat4& matrix) {
    if (!finiteVector(center))
        return {};

    const auto result = glm::translate(glm::mat4(1), center) * matrix * glm::translate(glm::mat4(1), -center);

    return TransformSystem::validAffine(result) ? std::optional<glm::mat4>(result) : std::nullopt;
}

bool motionPlane(const glm::vec3& u, const glm::vec3& v) {
    const auto a = motionUnitVector(u), b = motionUnitVector(v);

    return a && b && glm::length(glm::cross(*a, *b)) > 1e-6f;
}
} // namespace detail

bool TransformSystem::validAffine(const glm::mat4& m) {
    return detail::finiteMatrix(m) && m[0][3] == 0 && m[1][3] == 0 && m[2][3] == 0 && m[3][3] == 1;
}

std::optional<glm::mat4> TransformSystem::translation(const glm::vec3& offset) {
    if (!detail::finiteVector(offset))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::translation",
                                "Rejected condition: !detail::finiteVector(offset)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return glm::translate(glm::mat4(1), offset);
}

std::optional<glm::mat4> TransformSystem::scaling(const glm::vec3& factors, const glm::vec3& center) {
    if (!detail::finiteVector(factors))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::scaling",
                                "Rejected condition: !detail::finiteVector(factors)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return detail::motionAround(center, glm::scale(glm::mat4(1), factors));
}

std::optional<glm::mat4> TransformSystem::rotation(const AxisLine& axis, double degrees) {
    const auto unit = detail::motionUnitVector(axis.direction);

    if (!unit || !std::isfinite(degrees))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::rotation",
                                "Rejected condition: !unit || !std::isfinite(degrees)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const float radians = static_cast<float>(std::remainder(degrees, 360.0) * 3.14159265358979323846 / 180.0);

    return detail::motionAround(axis.point, glm::rotate(glm::mat4(1), radians, *unit));
}

std::optional<glm::mat4> TransformSystem::reflectPoint(const glm::vec3& center) {
    return scaling(glm::vec3(-1), center);
}

std::optional<glm::mat4> TransformSystem::reflectLine(const AxisLine& line) {
    const auto unit = detail::motionUnitVector(line.direction);

    if (!unit)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::reflectLine",
                                "Rejected condition: !unit",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    glm::mat3 matrix(0);

    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            matrix[c][r] = 2 * (*unit)[c] * (*unit)[r] - (c == r ? 1.0f : 0.0f);

    return detail::motionAround(line.point, glm::mat4(matrix));
}

std::optional<glm::mat4> TransformSystem::reflectPlane(const glm::vec3& point, const glm::vec3& normal) {
    const auto unit = detail::motionUnitVector(normal);

    if (!unit)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "TransformSystem::reflectPlane",
                                "Rejected condition: !unit",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    glm::mat3 matrix(1);

    for (int c = 0; c < 3; ++c)
        for (int r = 0; r < 3; ++r)
            matrix[c][r] -= 2 * (*unit)[c] * (*unit)[r];

    return detail::motionAround(point, glm::mat4(matrix));
}

PathFunction PathSystem::ellipse(const glm::vec3& center, const glm::vec3& u, const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::ellipse",
                                "Rejected condition: !detail::finiteVector(center) || !detail::motionPlane(u, v)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    return [center, u, v](double progress) {
        const double angle = progress * 6.28318530717958647692;

        return center + u * static_cast<float>(std::cos(angle)) + v * static_cast<float>(std::sin(angle));
    };
}

PathFunction PathSystem::circle(const glm::vec3& center, float radius, const glm::vec3& normal) {
    const auto n = detail::motionUnitVector(normal);

    if (!n || !std::isfinite(radius) || radius <= 0)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::circle",
                                "Rejected condition: !n || !std::isfinite(radius) || radius <= 0",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    const auto reference = std::abs(n->x) < .9f ? glm::vec3(1, 0, 0) : glm::vec3(0, 0, 1);
    const auto u = glm::normalize(glm::cross(reference, *n));

    return ellipse(center, radius * u, radius * glm::cross(*n, u));
}

PathFunction PathSystem::polyline(std::vector<glm::vec3> points, bool closed) {
    if (points.size() < 2)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::polyline",
                                "Rejected condition: points.size() < 2",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    for (const auto& p : points)
        if (!detail::finiteVector(p))
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Transform, "PathSystem::polyline",
                        "Rejected condition: !detail::finiteVector(p)",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    PathFunction{});

    if (closed) {
        if (glm::length(glm::dvec3(points.back()) - glm::dvec3(points.front())) > 1e-12)
            points.push_back(points.front());

        if (points.size() < 4)
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Transform, "PathSystem::polyline",
                        "Rejected condition: points.size() < 4",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    PathFunction{});
    }

    std::vector<double> distances{0};

    for (std::size_t i = 1; i < points.size(); ++i) {
        const double length = glm::length(glm::dvec3(points[i]) - glm::dvec3(points[i - 1]));

        if (!std::isfinite(length) || length <= 1e-12)
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Transform, "PathSystem::polyline",
                        "Rejected condition: !std::isfinite(length) || length <= 1e-12",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    PathFunction{});
        distances.push_back(distances.back() + length);
    }

    return [points = std::move(points), distances = std::move(distances)](double progress) {
        const double distance = std::clamp(progress, 0.0, 1.0) * distances.back();
        auto end = std::upper_bound(distances.begin(), distances.end(), distance);
        const std::size_t i = end == distances.end() ? distances.size() - 1 : std::size_t(end - distances.begin());
        const double weight = (distance - distances[i - 1]) / (distances[i] - distances[i - 1]);

        return glm::vec3(glm::dvec3(points[i - 1]) * (1 - weight) + glm::dvec3(points[i]) * weight);
    };
}

PathFunction PathSystem::polygon(std::vector<glm::vec3> points) {
    return polyline(std::move(points), true);
}

PathFunction PathSystem::rectangle(const glm::vec3& center, const glm::vec3& u, const glm::vec3& v) {
    if (!detail::finiteVector(center) || !detail::motionPlane(u, v))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::rectangle",
                                "Rejected condition: !detail::finiteVector(center) || !detail::motionPlane(u, v)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    const auto a = detail::motionUnitVector(u), b = detail::motionUnitVector(v);

    if (std::abs(glm::dot(*a, *b)) > 1e-5f)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "PathSystem::rectangle",
                                "Rejected condition: std::abs(glm::dot(*a, *b)) > 1e-5f",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            PathFunction{});

    return polygon({center - u - v, center + u - v, center + u + v, center - u + v});
}

double PathSystem::linear(double progress) {
    return progress;
}

double PathSystem::smoothStep(double progress) {
    return progress * progress * (3 - 2 * progress);
}
} // namespace graphics

// --- ObjLoader.cpp ---

namespace graphics::detail {
struct ObjParseError : std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct ObjCorner {
    long long position = 0, uv = 0, normal = 0;

    bool operator==(const ObjCorner& b) const {
        return position == b.position && uv == b.uv && normal == b.normal;
    }
};

struct ObjFace {
    std::vector<ObjCorner> corners;
    std::size_t positions = 0, uvs = 0, normals = 0, line = 0, batch = 0;
    std::string name;
};

void objWarning(std::vector<std::string>& warnings, const std::string& message) {
    if (std::find(warnings.begin(), warnings.end(), message) == warnings.end())
        warnings.push_back(message);
}

long long objInteger(const std::string& text) {
    const auto start = (!text.empty() && (text[0] == '-' || text[0] == '+')) ? 1u : 0u;

    if (start == text.size() || text.find_first_not_of("0123456789", start) != std::string::npos)
        throw ObjParseError("Invalid OBJ index: " + text);

    try {
        const auto value = std::stoll(text);

        if (value == 0)
            throw ObjParseError("OBJ indices start at 1; zero is invalid.");

        return value;
    } catch (const std::out_of_range&) {
        throw ObjParseError("OBJ index is too large.");
    }
}

ObjCorner objCorner(const std::string& text) {
    ObjCorner result;
    const auto a = text.find('/'), b = a == std::string::npos ? a : text.find('/', a + 1);

    if (b != std::string::npos && text.find('/', b + 1) != std::string::npos)
        throw ObjParseError("Too many fields in OBJ corner: " + text);
    result.position = objInteger(text.substr(0, a));

    if (a != std::string::npos) {
        const auto part = text.substr(a + 1, b == std::string::npos ? b : b - a - 1);

        if (!part.empty())
            result.uv = objInteger(part);
        else if (b == std::string::npos)
            throw ObjParseError("Missing UV after '/'.");
    }

    if (b != std::string::npos) {
        if (b + 1 == text.size())
            throw ObjParseError("Missing normal after '//'.");
        result.normal = objInteger(text.substr(b + 1));
    }

    return result;
}

std::size_t objResolve(long long value, std::size_t atFace, std::size_t total, bool compatible) {
    // Negative references use counts at the face, even during the second pass.
    if (value < 0) {
        if (value < -static_cast<long long>(atFace))
            throw ObjParseError("Negative OBJ index is out of range.");

        return std::size_t(static_cast<long long>(atFace) + value);
    }

    if (static_cast<unsigned long long>(value) > (compatible ? total : atFace))
        throw ObjParseError("Positive OBJ index is out of range or references a later record.");

    return std::size_t(value - 1);
}

std::vector<float> objNumbers(std::istringstream& row) {
    std::vector<float> result;
    std::string token;

    while (row >> token) {
        std::istringstream number(token);

        number.imbue(std::locale::classic());

        double value = 0;
        char extra = 0;

        if (!(number >> value) || (number >> extra) || !std::isfinite(value) ||
            std::abs(value) > std::numeric_limits<float>::max())
            throw ObjParseError("Invalid finite OBJ number: " + token);
        result.push_back(float(value));
    }

    return result;
}

std::string objText(const std::string& bytes, bool compatible, std::vector<std::string>& warnings) {
    std::string text = bytes;

    if (bytes.size() >= 2 &&
        ((static_cast<unsigned char>(bytes[0]) == 0xff && static_cast<unsigned char>(bytes[1]) == 0xfe) ||
         (static_cast<unsigned char>(bytes[0]) == 0xfe && static_cast<unsigned char>(bytes[1]) == 0xff))) {
        if (!compatible)
            throw ObjParseError("UTF-16 BOM requires the compatible text reader.");

        if (bytes.size() % 2)
            throw ObjParseError("Truncated UTF-16 text.");

        const bool little = static_cast<unsigned char>(bytes[0]) == 0xff;
        auto unit = [&](std::size_t i) {
            const auto a = static_cast<unsigned char>(bytes[i]), b = static_cast<unsigned char>(bytes[i + 1]);

            return unsigned(little ? (a | (b << 8)) : ((a << 8) | b));
        };
        text.clear();

        for (std::size_t i = 2; i < bytes.size(); i += 2) {
            unsigned c = unit(i);

            if (c >= 0xd800 && c <= 0xdbff) {
                if (i + 3 >= bytes.size())
                    throw ObjParseError("Truncated UTF-16 surrogate pair.");

                const auto low = unit(i + 2);

                if (low < 0xdc00 || low > 0xdfff)
                    throw ObjParseError("Invalid UTF-16 surrogate pair.");
                c = 0x10000 + ((c - 0xd800) << 10) + (low - 0xdc00);
                i += 2;
            } else if (c >= 0xdc00 && c <= 0xdfff)
                throw ObjParseError("Unpaired UTF-16 surrogate.");

            if (c < 0x80)
                text += char(c);
            else if (c < 0x800) {
                text += char(0xc0 | (c >> 6));
                text += char(0x80 | (c & 63));
            } else if (c < 0x10000) {
                text += char(0xe0 | (c >> 12));
                text += char(0x80 | ((c >> 6) & 63));
                text += char(0x80 | (c & 63));
            } else {
                text += char(0xf0 | (c >> 18));
                text += char(0x80 | ((c >> 12) & 63));
                text += char(0x80 | ((c >> 6) & 63));
                text += char(0x80 | (c & 63));
            }
        }
        objWarning(warnings, "Decoded UTF-16 BOM text as UTF-8.");
    }

    if (text.compare(0, 3, "\xEF\xBB\xBF") == 0)
        text.erase(0, 3);

    if (text.find('\0') != std::string::npos)
        throw ObjParseError("NUL byte in OBJ text; unsupported encoding or binary file.");

    std::string lines;

    for (std::size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\r') {
            lines += '\n';

            if (i + 1 < text.size() && text[i + 1] == '\n')
                ++i;
        } else
            lines += text[i];
    }

    return lines;
}

Model parseObj(const std::string& bytes, const std::string& name, const LoadOptions& options, bool compatible,
               std::vector<std::string>& warnings) {
    std::istringstream input(objText(bytes, compatible, warnings));
    std::vector<glm::vec3> positions, normals;
    std::vector<glm::vec2> uvs;
    std::vector<GlColor> colors;
    std::vector<ObjFace> faces;
    bool hasColors = false;
    std::string physical, line, group = "face";
    std::size_t lineNumber = 0, start = 0, batch = 0;

    while (std::getline(input, physical)) {
        ++lineNumber;

        if (line.empty())
            start = lineNumber;
        physical = physical.substr(0, physical.find('#'));

        const auto end = physical.find_last_not_of(" \t");
        const bool continued = end != std::string::npos && physical[end] == '\\';

        line += continued ? physical.substr(0, end) : physical;

        if (continued) {
            line += ' ';
            continue;
        }

        std::istringstream row(line);

        row.imbue(std::locale::classic());

        std::string kind;

        row >> kind;

        try {
            if (kind == "v") {
                const auto values = objNumbers(row);

                if (values.size() != 3 && values.size() != 4 && values.size() != 6)
                    throw ObjParseError("Expected v x y z [w] or v x y z r g b.");

                glm::vec3 p(values[0], values[1], values[2]);

                if (values.size() == 4) {
                    if (values[3] == 0)
                        throw ObjParseError("Homogeneous vertex has zero w.");
                    p /= values[3];
                }

                if (!finiteVector(p))
                    throw ObjParseError("Vertex overflows after division by w.");
                positions.push_back(p);
                colors.push_back(values.size() == 6 ? GlColor{values[3], values[4], values[5]} : GlColor{});
                hasColors = hasColors || values.size() == 6;
            } else if (kind == "vt") {
                const auto values = objNumbers(row);

                if (values.empty() || values.size() > 3 || (values.size() == 3 && values[2] != 0))
                    throw ObjParseError("Expected vt u [v [0]]; volume texture coordinates are unsupported.");
                uvs.push_back({values[0], values.size() > 1 ? values[1] : 0});
            } else if (kind == "vn") {
                const auto values = objNumbers(row);

                if (values.size() != 3)
                    throw ObjParseError("Expected vn x y z.");

                const glm::dvec3 normal(values[0], values[1], values[2]);

                if (glm::length(normal) == 0)
                    throw ObjParseError("Zero OBJ normal.");
                normals.push_back(glm::vec3(glm::normalize(normal)));
            } else if (kind == "f") {
                ObjFace face;

                face.positions = positions.size();
                face.uvs = uvs.size();
                face.normals = normals.size();
                face.line = start;
                face.batch = batch;
                face.name = group;

                std::string token;

                while (row >> token)
                    face.corners.push_back(objCorner(token));

                if (face.corners.size() < 3)
                    throw ObjParseError("Face has fewer than three corners.");
                faces.push_back(std::move(face));
            } else if (kind == "o" || kind == "g") {
                std::getline(row, group);

                const auto first = group.find_first_not_of(" \t");

                group = first == std::string::npos ? "face" : group.substr(first);
                ++batch;
            } else if (kind == "mtllib" || kind == "usemtl") {
                ++batch;
                objWarning(warnings,
                           "MTL materials are not loaded; use Scene material settings if needed.");
            } else if (kind == "s") {
                ++batch;

                std::string smoothing;

                row >> smoothing;

                if (smoothing != "off" && smoothing != "0")
                    objWarning(
                        warnings,
                        "Smoothing groups are not interpolated; supplied vn values are preserved and missing normals are generated per face.");
            } else if (!kind.empty())
                throw ObjParseError("Unsupported OBJ record: " + kind);
        } catch (const ObjParseError& e) {
            throw ObjParseError("line " + std::to_string(start) + ": " + e.what());
        }
        line.clear();
    }

    if (!line.empty())
        throw ObjParseError("line " + std::to_string(start) + ": unterminated line continuation.");

    Model model;

    model.shape = GLShape::Custom;
    model.name = name;

    Model segment;
    std::size_t activeBatch = 0;
    auto flush = [&] {
        if (options.mergeCoplanarFaces && !hasColors)
            mergeCoplanar(segment);

        for (auto& face : segment.faces)
            model.faces.push_back(std::move(face));
        segment.faces.clear();
    };

    for (auto& face : faces) {
        try {
            if (face.batch != activeBatch) {
                flush();
                activeBatch = face.batch;
            }

            if (compatible) {
                const auto old = face.corners.size();

                face.corners.erase(std::unique(face.corners.begin(), face.corners.end()), face.corners.end());

                if (face.corners.size() > 1 && face.corners.front() == face.corners.back())
                    face.corners.pop_back();

                if (face.corners.size() != old)
                    objWarning(warnings,
                               "Removed repeated identical OBJ corners at line " + std::to_string(face.line) + ".");
            }

            Mesh mesh;

            mesh.name = face.name + "_" + std::to_string(face.line);

            std::size_t uvCount = 0;
            bool forward = false;

            for (const auto& corner : face.corners) {
                const auto p = objResolve(corner.position, face.positions, positions.size(), compatible);

                mesh.localVertices.push_back(positions[p]);

                if (hasColors)
                    mesh.vertexColors.push_back(colors[p]);
                forward = forward ||
                          (corner.position > 0 && static_cast<unsigned long long>(corner.position) > face.positions);

                glm::vec2 uv(0);

                if (corner.uv) {
                    uv = uvs[objResolve(corner.uv, face.uvs, uvs.size(), compatible)];
                    ++uvCount;
                    forward = forward || (corner.uv > 0 && static_cast<unsigned long long>(corner.uv) > face.uvs);
                }
                mesh.texcoords.push_back(uv);
            }
            // Normalize only the triangulation workspace; retain original coordinates.
            if (mesh.localVertices.size() < 3)
                throw ObjParseError("Face has fewer than three distinct corners.");

            glm::dvec3 low(mesh.localVertices[0]), high(low);

            for (const auto& p : mesh.localVertices) {
                low = glm::min(low, glm::dvec3(p));
                high = glm::max(high, glm::dvec3(p));
            }

            const auto size = high - low;
            const double scale = std::max({size.x, size.y, size.z});

            if (scale == 0)
                throw ObjParseError("Degenerate polygon.");

            Mesh workspace;

            for (const auto& p : mesh.localVertices)
                workspace.localVertices.push_back(glm::vec3((glm::dvec3(p) - low) / scale));

            if (workspace.localVertices.size() > 3) {
                glm::dvec3 plane(0);
                const auto count = workspace.localVertices.size();

                for (std::size_t i = 0; i < count; ++i) {
                    const auto a = glm::dvec3(workspace.localVertices[i]);
                    const auto b = glm::dvec3(workspace.localVertices[(i + 1) % count]);

                    plane += glm::cross(a, b);
                }

                if (glm::length(plane) < 1e-12)
                    throw ObjParseError("Degenerate polygon plane.");
                plane = glm::normalize(plane);

                const auto absolute = glm::abs(plane);
                const int axis =
                    absolute.x > absolute.y ? (absolute.x > absolute.z ? 0 : 2) : (absolute.y > absolute.z ? 1 : 2);
                std::vector<glm::dvec2> projected;

                for (const auto& p : workspace.localVertices) {
                    if (std::abs(glm::dot(plane, glm::dvec3(p - workspace.localVertices[0]))) > 1e-5)
                        throw ObjParseError("Non-planar OBJ polygon; export triangulated faces.");
                    projected.push_back(axis == 0   ? glm::dvec2(p.y, p.z)
                                        : axis == 1 ? glm::dvec2(p.x, p.z)
                                                    : glm::dvec2(p.x, p.y));
                }

                auto cross = [](glm::dvec2 a, glm::dvec2 b, glm::dvec2 c) {
                    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
                };

                auto on = [](glm::dvec2 a, glm::dvec2 b, glm::dvec2 p) {
                    return p.x >= std::min(a.x, b.x) - 1e-14 && p.x <= std::max(a.x, b.x) + 1e-14 &&
                           p.y >= std::min(a.y, b.y) - 1e-14 && p.y <= std::max(a.y, b.y) + 1e-14;
                };

                for (std::size_t i = 0; i < count; ++i)
                    for (std::size_t j = i + 1; j < count; ++j) {
                        if ((i + 1) % count == j || (j + 1) % count == i)
                            continue;

                        const auto a = projected[i], b = projected[(i + 1) % count], c = projected[j],
                                   d = projected[(j + 1) % count];
                        const auto abC = cross(a, b, c), abD = cross(a, b, d), cdA = cross(c, d, a),
                                   cdB = cross(c, d, b);
                        const bool crossing = ((abC > 1e-14 && abD < -1e-14) || (abC < -1e-14 && abD > 1e-14)) &&
                                              ((cdA > 1e-14 && cdB < -1e-14) || (cdA < -1e-14 && cdB > 1e-14));

                        if (crossing || (std::abs(abC) <= 1e-14 && on(a, b, c)) ||
                            (std::abs(abD) <= 1e-14 && on(a, b, d)) || (std::abs(cdA) <= 1e-14 && on(c, d, a)) ||
                            (std::abs(cdB) <= 1e-14 && on(c, d, b)))
                            throw ObjParseError("Self-intersection or repeated corner in OBJ polygon.");
                    }
            }

            try {
                mesh.triangleIndices = triangulate(workspace.localVertices);
            } catch (const std::runtime_error& e) {
                throw ObjParseError(e.what());
            }
            workspace.triangleIndices = mesh.triangleIndices;
            recalculateNormals(workspace);
            mesh.vertexNormals = std::move(workspace.vertexNormals);

            for (std::size_t i = 0; i < face.corners.size(); ++i)
                if (face.corners[i].normal) {
                    const auto value = face.corners[i].normal;

                    mesh.vertexNormals[i] = normals[objResolve(value, face.normals, normals.size(), compatible)];
                    forward = forward || (value > 0 && static_cast<unsigned long long>(value) > face.normals);
                }

            if (forward)
                objWarning(warnings, "Resolved positive forward references at line " + std::to_string(face.line) + ".");

            if (uvCount == 0)
                mesh.texcoords.clear();
            else if (uvCount != face.corners.size())
                objWarning(warnings, "Filled missing UV corners with (0,0) at line " + std::to_string(face.line) + ".");

            const auto& indices = mesh.triangleIndices;

            mesh.normal =
                glm::normalize(glm::cross(workspace.localVertices[indices[1]] - workspace.localVertices[indices[0]],
                                          workspace.localVertices[indices[2]] - workspace.localVertices[indices[0]]));

            if (!validMesh(mesh))
                throw ObjParseError("Invalid or degenerate OBJ mesh.");
            segment.faces.push_back(std::move(mesh));
        } catch (const ObjParseError& e) {
            throw ObjParseError("line " + std::to_string(face.line) + ": " + e.what());
        }
    }
    flush();

    if (options.mergeCoplanarFaces && hasColors)
        objWarning(warnings, "Coplanar merging is skipped to preserve vertex colors.");

    if (model.faces.empty())
        throw ObjParseError("OBJ contains no polygon faces.");

    return model;
}
} // namespace graphics::detail

namespace graphics {
ModelResult AssetLoader::loadObj(const std::filesystem::path& relative, const LoadOptions& options) const {
    ModelResult result;
    const auto utf8 = relative.u8string();
    const std::string name(utf8.begin(), utf8.end());

    try {
        std::ifstream input(resolve(relative), std::ios::binary);

        if (!input)
            throw std::runtime_error("Cannot open OBJ text.");

        const std::string bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

        if (input.bad())
            throw std::runtime_error("OBJ file read failed.");

        Model model;

        try {
            model = detail::parseObj(bytes, name, options, false, result.warnings);
        } catch (const detail::ObjParseError& first) {
            if (!options.allowObjFallback)
                throw;
            result.warnings.clear();

            try {
                model = detail::parseObj(bytes, name, options, true, result.warnings);
            } catch (const detail::ObjParseError& second) {
                throw std::runtime_error(std::string("Standard reader: ") + first.what() +
                                         "; compatible reader: " + second.what());
            }
            result.usedFallback = true;
            detail::objWarning(result.warnings, std::string("Compatible OBJ reader used after: ") + first.what());
        }

        auto finished = detail::finishLoad(std::move(model), options);

        finished.warnings = std::move(result.warnings);
        finished.usedFallback = result.usedFallback;

        return finished;
    } catch (const std::exception& e) {
        result.error = name + ": " + e.what();
        std::cerr << "[OBJ load failed] " << result.error << '\n'
                  << "Check the file path and the reported cause first.\n"
                  << "Supported OBJ text format (.obj or .txt), one record per line:\n"
                  << "  v x y z       : vertex position\n"
                  << "  vt u [v]      : optional texture coordinate\n"
                  << "  vn x y z      : optional nonzero normal\n"
                  << "  f 1 2 3       : face using 1-based vertex indices\n"
                  << "  Corner forms: v, v/vt, v//vn, v/vt/vn; negative relative indices are allowed.\n"
                  << "Minimal triangle example:\n"
                  << "  v 0 0 0\n  v 1 0 0\n  v 0 1 0\n  f 1 2 3\n"
                  << "If parsing still fails, inspect the model's source format and write a parser for it.\n"
                  << "Fill graphics::Mesh.localVertices and triangleIndices, then call\n"
                  << "graphics::recalculateNormals(mesh), add the mesh to graphics::Model.faces,\n"
                  << "and pass that model to Scene::addModel. See framework/docs/OBJ_LOADING.md.\n";

        return result;
    }
}
} // namespace graphics

// --- Motion.cpp ---

namespace graphics {
bool Motion::validStep(const MotionStep& step) {
    return step.transform && std::isfinite(step.startSeconds) && step.startSeconds >= 0 &&
           (!step.durationSeconds || (std::isfinite(*step.durationSeconds) && *step.durationSeconds >= 0 &&
                                      std::isfinite(step.startSeconds + *step.durationSeconds)));
}

bool Motion::append(const MotionStep& step) {
    return insert(steps_.size(), step);
}

bool Motion::insert(std::size_t index, const MotionStep& step) {
    if (index > steps_.size() || !validStep(step))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "Motion::insert",
            "Rejected condition: index > steps_.size() || !validStep(step)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
    steps_.insert(steps_.begin() + index, step);

    return true;
}

bool Motion::replace(std::size_t index, const MotionStep& step) {
    if (index >= steps_.size() || !validStep(step))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "Motion::replace",
            "Rejected condition: index >= steps_.size() || !validStep(step)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
    steps_[index] = step;

    return true;
}

bool Motion::erase(std::size_t index) {
    if (index >= steps_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "Motion::erase", "Rejected condition: index >= steps_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
    steps_.erase(steps_.begin() + index);

    return true;
}

std::optional<MotionSample> Motion::sample(double seconds) const {
    if (!std::isfinite(seconds) || seconds < 0)
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "Motion::sample",
                                "Rejected condition: !std::isfinite(seconds) || seconds < 0",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    MotionSample result;

    for (const auto& step : steps_) {
        if (!step.enabled || seconds < step.startSeconds)
            continue;

        double time = seconds - step.startSeconds;

        if (step.durationSeconds)
            time = std::min(time, *step.durationSeconds);

        const auto matrix = step.transform(time);

        if (!TransformSystem::validAffine(matrix))
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Motion, "Motion::sample",
                        "Rejected condition: !TransformSystem::validAffine(matrix)",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    std::nullopt);

        auto& target = step.space == TransformSpace::World ? result.parent : result.local;

        target = matrix * target; // Within each space, API order is application order.

        if (!TransformSystem::validAffine(target))
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Motion, "Motion::sample",
                        "Rejected condition: !TransformSystem::validAffine(target)",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    std::nullopt);
    }

    return result;
}

MotionStep Motion::fixed(const glm::mat4& matrix, TransformSpace space) {
    MotionStep step;

    if (TransformSystem::validAffine(matrix))
        step.transform = [matrix](double) {
            return matrix;
        };
    step.space = space;
    step.durationSeconds = 0;

    return step;
}

MotionStep Motion::rotation(const RotationMotion& config) {
    MotionStep step;

    if (!std::isfinite(config.degreesPerSecond) || !std::isfinite(config.initialDegrees) ||
        !TransformSystem::rotation(config.axis, config.initialDegrees))
        return step;
    step.space = config.space;
    step.transform = [config](double seconds) {
        const auto matrix =
            TransformSystem::rotation(config.axis, config.initialDegrees + config.degreesPerSecond * seconds);

        if (!matrix)
            throw std::domain_error("Rotation motion produced an invalid matrix.");

        return *matrix;
    };

    return step;
}

MotionStep Motion::followPath(const PathMotion& config) {
    MotionStep step;

    if (!config.path || !config.interpolation || !std::isfinite(config.durationSeconds) ||
        config.durationSeconds <= 0 || !std::isfinite(config.rate) || config.rate == 0 ||
        !std::isfinite(config.phase) || config.phase < 0 || config.phase > 1)
        return step;
    step.space = config.space;

    if (config.playback == Playback::Once) {
        step.durationSeconds =
            (config.rate > 0 ? 1 - config.phase : config.phase) * config.durationSeconds / std::abs(config.rate);

        if (!std::isfinite(*step.durationSeconds))
            return MotionStep{};
    }
    step.transform = [config](double seconds) {
        double progress = config.phase + seconds / config.durationSeconds * config.rate;

        if (!std::isfinite(progress))
            throw std::domain_error("Path time overflow.");

        if (config.playback == Playback::Once)
            progress = std::clamp(progress, 0.0, 1.0);
        else if (config.playback == Playback::Loop)
            progress -= std::floor(progress);
        else {
            progress -= std::floor(progress / 2) * 2;

            if (progress > 1)
                progress = 2 - progress;
        }

        const double u = config.interpolation(progress);

        if (!std::isfinite(u))
            throw std::domain_error("Interpolation returned non-finite progress.");

        glm::vec3 offset = config.path(u);

        if (config.relativeToStart) {
            const double initial = config.interpolation(config.phase);

            if (!std::isfinite(initial))
                throw std::domain_error("Invalid initial interpolation.");
            offset -= config.path(initial);
        }

        const auto matrix = TransformSystem::translation(offset);

        if (!matrix)
            throw std::domain_error("Path returned a non-finite position.");

        return *matrix;
    };

    return step;
}

MotionStep Motion::linear(const LinearMotion& config) {
    if (!detail::finiteVector(config.from) || !detail::finiteVector(config.to) ||
        (config.playback != Playback::Once && config.playback != Playback::Loop &&
         config.playback != Playback::PingPong) ||
        (config.space != TransformSpace::World && config.space != TransformSpace::Local))
        return {};

    PathMotion path;

    path.path = [from = glm::dvec3(config.from), to = glm::dvec3(config.to)](double u) {
        return glm::vec3((1 - u) * from + u * to);
    };
    path.durationSeconds = config.durationSeconds;
    path.playback = config.playback;
    path.space = config.space;

    return followPath(path);
}

MotionStep Motion::pendulum(const PendulumMotion& config) {
    MotionStep step;

    if (!std::isfinite(config.amplitudeDegrees) || config.amplitudeDegrees < 0 ||
        !std::isfinite(config.periodSeconds) || config.periodSeconds <= 0 || !std::isfinite(config.phaseDegrees) ||
        !std::isfinite(config.offsetDegrees) ||
        !std::isfinite(std::abs(config.offsetDegrees) + config.amplitudeDegrees) ||
        (config.space != TransformSpace::World && config.space != TransformSpace::Local) ||
        !TransformSystem::rotation(config.axis, config.offsetDegrees))
        return step;
    step.space = config.space;
    step.transform = [config](double seconds) {
        constexpr double pi = 3.14159265358979323846;
        const double phase = std::remainder(seconds, config.periodSeconds) / config.periodSeconds * 2 * pi +
                             std::remainder(config.phaseDegrees, 360.0) * pi / 180;
        const auto matrix =
            TransformSystem::rotation(config.axis, config.offsetDegrees + config.amplitudeDegrees * std::sin(phase));

        if (!matrix)
            throw std::domain_error("Pendulum motion produced an invalid matrix.");

        return *matrix;
    };

    return step;
}

std::optional<MotionId> MotionSystem::createMotion(const Motion& motion) {
    const auto sample = motion.sample(0);

    if (!sample || nextMotion_ == std::numeric_limits<MotionId>::max())
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::createMotion",
                                "Rejected condition: !sample || nextMotion_ == std::numeric_limits<MotionId>::max()",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const auto id = nextMotion_;
    MotionState state;

    state.motion = motion;
    state.sampled = *sample;
    motions_.emplace(id, std::move(state));
    ++nextMotion_;

    return id;
}

const MotionState* MotionSystem::getMotion(MotionId id) const {
    const auto found = motions_.find(id);

    return found == motions_.end() ? nullptr : &found->second;
}

bool MotionSystem::setMotion(MotionId id, const Motion& motion) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setMotion",
            "Rejected condition: found == motions_.end()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto sample = motion.sample(found->second.timeSeconds);

    if (!sample)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setMotion", "Rejected condition: !sample",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.motion = motion;
    found->second.sampled = *sample;

    return true;
}

bool MotionSystem::setMotionPlaying(MotionId id, bool playing) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setMotionPlaying",
            "Rejected condition: found == motions_.end()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.playing = playing;

    return true;
}

bool MotionSystem::setMotionEnabled(MotionId id, bool enabled) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setMotionEnabled",
            "Rejected condition: found == motions_.end()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.enabled = enabled;

    return true;
}

bool MotionSystem::seekMotion(MotionId id, double seconds) {
    const auto found = motions_.find(id);

    if (found == motions_.end())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::seekMotion",
            "Rejected condition: found == motions_.end()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto sample = found->second.motion.sample(seconds);

    if (!sample)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::seekMotion", "Rejected condition: !sample",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.timeSeconds = seconds;
    found->second.sampled = *sample;

    return true;
}

bool MotionSystem::removeMotion(MotionId id) {
    if (!motions_.erase(id))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::removeMotion",
            "Rejected condition: !motions_.erase(id)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    for (auto& layer : layers_)
        if (layer.second.config.motion == id)
            layer.second.config.motion.reset();

    return true;
}

bool MotionSystem::update(double deltaSeconds) {
    if (!std::isfinite(deltaSeconds) || deltaSeconds < 0)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::update",
            "Rejected condition: !std::isfinite(deltaSeconds) || deltaSeconds < 0",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");

    struct Pending {
        MotionId id;
        double time;
        MotionSample sample;
    };

    std::vector<Pending> pending;

    pending.reserve(motions_.size());

    for (const auto& entry : motions_) {
        const auto& state = entry.second;

        if (!state.playing)
            continue;

        const double time = state.timeSeconds + deltaSeconds;
        const auto sample = state.motion.sample(time);

        if (!sample)
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::update", "Rejected condition: !sample",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");
        pending.push_back({entry.first, time, *sample});
    }

    for (const auto& item : pending) {
        auto& state = motions_.at(item.id);

        state.timeSeconds = item.time;
        state.sampled = item.sample;
    }

    return true;
}

void MotionSystem::clear() {
    std::map<LayerId, LayerState> emptyLayers{{rootLayer(), LayerState{}}};

    motions_.clear();
    layers_.swap(emptyLayers);
}

bool MotionSystem::validLayer(const LayerConfig& config) const {
    return TransformSystem::validAffine(config.transform) && (!config.motion || getMotion(*config.motion));
}

const LayerState* MotionSystem::getLayer(LayerId id) const {
    const auto found = layers_.find(id);

    return found == layers_.end() ? nullptr : &found->second;
}

std::optional<LayerId> MotionSystem::pushLayer(LayerId parent, const LayerConfig& config) {
    if (!getLayer(parent) || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max())
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::pushLayer",
                "Rejected condition: !getLayer(parent) || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max()",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    auto staged = layers_;
    const auto id = nextLayer_;

    staged.emplace(id, LayerState{config, parent, {}});
    staged.at(parent).children.push_back(id);
    layers_.swap(staged);
    ++nextLayer_;

    return id;
}

std::optional<LayerId> MotionSystem::insertLayer(LayerId child, const LayerConfig& config) {
    const auto current = getLayer(child);

    if (!current || !current->parent || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max())
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::insertLayer",
                "Rejected condition: !current || !current->parent || !validLayer(config) || nextLayer_ == std::numeric_limits<LayerId>::max()",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const auto parent = *current->parent;
    const auto id = nextLayer_;
    auto staged = layers_;

    staged.emplace(id, LayerState{config, parent, {child}});

    auto& siblings = staged.at(parent).children;

    std::replace(siblings.begin(), siblings.end(), child, id);
    staged.at(child).parent = id;
    layers_.swap(staged);
    ++nextLayer_;

    return id;
}

bool MotionSystem::removeLayer(LayerId id) {
    const auto current = getLayer(id);

    if (!current || !current->parent)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::removeLayer",
            "Rejected condition: !current || !current->parent",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto staged = layers_;
    const auto parent = *current->parent;
    auto& siblings = staged.at(parent).children;
    const auto position = std::find(siblings.begin(), siblings.end(), id);
    const auto index = std::size_t(position - siblings.begin());

    siblings.erase(position);
    siblings.insert(siblings.begin() + index, current->children.begin(), current->children.end());

    for (auto child : current->children)
        staged.at(child).parent = parent;
    staged.erase(id);
    layers_.swap(staged);

    return true;
}

std::optional<LayerId> MotionSystem::popLayer(LayerId top) {
    const auto current = getLayer(top);

    if (!current || !current->parent || !current->children.empty())
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::popLayer",
                                "Rejected condition: !current || !current->parent || !current->children.empty()",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const auto parent = *current->parent;

    if (!removeLayer(top))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::popLayer",
                                "Rejected condition: !removeLayer(top)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    return parent;
}

bool MotionSystem::reparentLayer(LayerId id, LayerId parent) {
    const auto current = getLayer(id);

    if (!current || !current->parent || !getLayer(parent))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::reparentLayer",
            "Rejected condition: !current || !current->parent || !getLayer(parent)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    if (current->parent == parent)
        return true;

    auto ancestor = std::optional<LayerId>(parent);

    while (ancestor) {
        if (*ancestor == id)
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::reparentLayer",
                "Rejected condition: *ancestor == id",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
        ancestor = layers_.at(*ancestor).parent;
    }

    auto staged = layers_;
    auto& old = staged.at(*current->parent).children;

    old.erase(std::find(old.begin(), old.end(), id));
    staged.at(parent).children.push_back(id);
    staged.at(id).parent = parent;
    layers_.swap(staged);

    return true;
}

bool MotionSystem::setLayer(LayerId id, const LayerConfig& config) {
    const auto found = layers_.find(id);

    if (found == layers_.end() || !validLayer(config))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::setLayer",
            "Rejected condition: found == layers_.end() || !validLayer(config)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    found->second.config = config;

    return true;
}

bool MotionSystem::bindMotion(LayerId layer, std::optional<MotionId> motion) {
    const auto current = getLayer(layer);

    if (!current || (motion && !getMotion(*motion)))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::bindMotion",
            "Rejected condition: !current || (motion && !getMotion(*motion))",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.");

    auto config = current->config;

    config.motion = motion;

    return setLayer(layer, config);
}

std::optional<MotionSample> MotionSystem::layerMatrices(LayerId leaf) const {
    std::vector<LayerId> chain;
    auto current = std::optional<LayerId>(leaf);

    while (current) {
        const auto layer = getLayer(*current);

        if (!layer || chain.size() >= layers_.size())
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::layerMatrices",
                        "Rejected condition: !layer || chain.size() >= layers_.size()",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                    std::nullopt);
        chain.push_back(*current);
        current = layer->parent;
    }

    MotionSample result;

    for (auto i = chain.rbegin(); i != chain.rend(); ++i) {
        const auto& config = layers_.at(*i).config;

        if (!config.enabled)
            continue;

        MotionSample own;

        if (config.space == TransformSpace::World)
            own.parent = config.transform;
        else
            own.local = config.transform;

        if (config.motion) {
            const auto state = getMotion(*config.motion);

            if (!state)
                return (Diagnostics::failAt(
                            __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::layerMatrices",
                            "Rejected condition: !state",
                            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                        std::nullopt);

            if (state->enabled) {
                own.parent = state->sampled.parent * own.parent;
                own.local = state->sampled.local * own.local;
            }
        }

        if (*i == rootLayer())
            result.parent = own.parent * own.local;
        else {
            result.parent *= own.parent;
            result.local *= own.local;
        }

        if (!TransformSystem::validAffine(result.parent) || !TransformSystem::validAffine(result.local))
            return (
                Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Motion, "MotionSystem::layerMatrices",
                    "Rejected condition: !TransformSystem::validAffine(result.parent) || !TransformSystem::validAffine(result.local)",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                std::nullopt);
    }

    return result;
}

std::optional<MotionId> MotionSystem::createMotion(std::initializer_list<MotionStep> steps) {
    Motion motion;
    for (const auto& step : steps)
        if (!motion.append(step)) return std::nullopt;
    return createMotion(motion);
}

std::optional<MotionId> MotionSystem::createSpin(RotationMotion config) {
    config.space = TransformSpace::Local;
    return createMotion({Motion::rotation(config)});
}

std::optional<MotionId> MotionSystem::createOrbit(RotationMotion config) {
    config.space = TransformSpace::World;
    return createMotion({Motion::rotation(config)});
}
} // namespace graphics

// --- Scene.cpp ---

namespace graphics {
namespace detail {

bool positiveLengths(const glm::vec3& value) {
    return finiteVector(value) && value.x > 0.0f && value.y > 0.0f && value.z > 0.0f;
}

std::uint64_t nextGeometryRevision() {
    // 장면 복사·교체 시에도 다른 형상에 같은 revision을 부여하지 않는다.
    // Scene과 Renderer는 한 렌더링 스레드에서 사용한다.
    static std::uint64_t revision = 0;

    return ++revision;
}

void appendFace(Polyhedron& polyhedron, GLFace shape, std::initializer_list<glm::vec3> vertices, GlColor color) {
    PlaneFigure face;

    face.shape = shape;
    face.localVertices = vertices;
    face.color = color;
    face.normal = glm::normalize(
        glm::cross(face.localVertices[1] - face.localVertices[0], face.localVertices[2] - face.localVertices[0]));
    polyhedron.faces.push_back(face);
}

} // namespace detail

using detail::finiteVector;
using detail::finiteMatrix;
using detail::validColor;
using detail::positiveLengths;
using detail::nextGeometryRevision;
using detail::appendFace;

Scene::Scene() {
    geometryRevision_ = nextGeometryRevision();
    polyId_.reserve(MAX_ENTITIES);
    components_.reserve(MAX_ENTITIES);
    lines_.reserve(MAX_LINES);
}

std::optional<EntityId> Scene::addGroup(const Transform3D& transform) {
    if (!DiagramSystem::validTransform(transform))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Scene::addGroup",
                                "Rejected condition: !DiagramSystem::validTransform(transform)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const auto id = components_.size();
    Component_Poly group;

    group.polyhedron.shape = GLShape::None;
    group.transform = transform;
    group.sideLengths = glm::vec3(0);
    group.transformOnly = true;
    components_.push_back(group);
    polyId_.push_back(id);
    geometryRevision_ = nextGeometryRevision();

    return id;
}

bool Scene::setTint(EntityId id, GlColor color) {
    if (id >= components_.size() || !validColor(color))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTint",
            "Rejected condition: id>=components_.size() || !validColor(color)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    components_[id].tint = color;

    return true;
}

std::optional<EntityId> Scene::push_entity(const Polyhedron& polyhedron, const Transform3D& transform) {
    if (!DiagramSystem::validPolyhedron(polyhedron) || !DiagramSystem::validTransform(transform)) {
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::push_entity",
                "Rejected condition: !DiagramSystem::validPolyhedron(polyhedron) || !DiagramSystem::validTransform(transform)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);
    }

    glm::vec3 minimum = polyhedron.faces[0].localVertices[0];
    glm::vec3 maximum = minimum;

    for (const PlaneFigure& face : polyhedron.faces) {
        for (const glm::vec3& vertex : face.localVertices) {
            minimum = glm::min(minimum, vertex);
            maximum = glm::max(maximum, vertex);
        }
    }

    if (!finiteVector(maximum - minimum))
        return (
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Scene::push_entity",
                                "Rejected condition: !finiteVector(maximum - minimum)",
                                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);

    const EntityId id = components_.size();

    components_.push_back({polyhedron, transform, maximum - minimum});
    polyId_.push_back(id);
    geometryRevision_ = nextGeometryRevision();

    return id;
}

std::optional<EntityId> Scene::addCuboid(const glm::vec3& sideLengths, const glm::vec3& worldPosition) {
    Transform3D transform;

    transform.basis = worldPosition;

    return push_entity(DiagramSystem::makePolyhedron(GLShape::Cube, sideLengths), transform);
}

std::optional<EntityId> Scene::addPyramid(const glm::vec3& sideLengths, const glm::vec3& worldPosition) {
    Transform3D transform;

    transform.basis = worldPosition;

    return push_entity(DiagramSystem::makePolyhedron(GLShape::Pyramid, sideLengths), transform);
}

bool Scene::setFaceFrame(EntityId id, std::size_t faceIndex, const glm::mat4& matrix) {
    if (!getMesh(id, faceIndex) || !TransformSystem::validAffine(matrix))
        return false;
    components_[id].polyhedron.faces[faceIndex].transformFrame = matrix;
    return true;
}

bool Scene::setFaceVisible(EntityId id, std::size_t faceIndex, bool visible) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVisible",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    components_[id].polyhedron.faces[faceIndex].visible = visible;

    return true;
}

bool Scene::setFaceCullMode(EntityId id, std::size_t faceIndex, CullMode mode) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() ||
        (mode != CullMode::Inherit && mode != CullMode::None && mode != CullMode::Back && mode != CullMode::Front))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Raster, "Scene::setFaceCullMode",
                                   "Invalid entity, face index or culling mode.",
                                   "Check the model's face list and CullMode enum.", id);
    components_[id].polyhedron.faces[faceIndex].cull = mode;

    return true;
}

bool Scene::setFaceLocalMatrix(EntityId id, std::size_t faceIndex, const glm::mat4& matrix, FaceAnimationSpace space) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix) ||
        (space != FaceAnimationSpace::Local && space != FaceAnimationSpace::Model)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceLocalMatrix",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !finiteMatrix(matrix)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    components_[id].polyhedron.faces[faceIndex].localAnimationMatrix = matrix;
    components_[id].polyhedron.faces[faceIndex].animationSpace = space;

    return true;
}

bool Scene::setFaceVertexColors(EntityId id, std::size_t faceIndex, const std::vector<GlColor>& colors) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVertexColors",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];

    if (!colors.empty() && colors.size() != face.localVertices.size()) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVertexColors",
            "Rejected condition: !colors.empty() && colors.size() != face.localVertices.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    for (GlColor color : colors) {
        if (!validColor(color)) {
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceVertexColors",
                "Rejected condition: !validColor(color)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
        }
    }
    face.vertexColors = colors;
    geometryRevision_ = nextGeometryRevision();

    return true;
}

const Component_Poly* Scene::getComponent(EntityId id) const {
    return id < components_.size() ? &components_[id] : nullptr;
}

bool Scene::setPosition(EntityId id, const glm::vec3& worldPosition) {
    // For a parented entity this position is in parent coordinates.
    if (id >= components_.size() || !finiteVector(worldPosition)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setPosition",
            "Rejected condition: id >= components_.size() || !finiteVector(worldPosition)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    auto candidate = components_[id].transform;

    candidate.basis = worldPosition;

    return setTransform(id, candidate);
}

bool Scene::setSideLengths(EntityId id, const glm::vec3& sideLengths) {
    if (id >= components_.size() || !positiveLengths(sideLengths))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths",
            "Rejected condition: id>=components_.size()||!positiveLengths(sideLengths)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto& c = components_[id];

    if (c.transformOnly)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths", "Rejected condition: c.transformOnly",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = c.polyhedron;
    const auto box = detail::bounds(replacement);
    const auto oldSize = box.second - box.first;

    if (!positiveLengths(oldSize))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths",
            "Rejected condition: !positiveLengths(oldSize)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto ratio = sideLengths / oldSize;

    if (!finiteVector(ratio))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setSideLengths",
            "Rejected condition: !finiteVector(ratio)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    // Preserve topology/materials. Resize mesh coordinates around the model pivot.
    for (auto& face : replacement.faces) {
        for (auto& vertex : face.localVertices)
            vertex = c.transform.localBasis + (vertex - c.transform.localBasis) * ratio;
        face.transform.localBasis =
            c.transform.localBasis + (face.transform.localBasis - c.transform.localBasis) * ratio;
        recalculateNormals(face);
    }

    return setModel(id, replacement);
}

bool Scene::setParent(EntityId id, std::optional<EntityId> parent) {
    if (id >= components_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setParent",
            "Rejected condition: id >= components_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto ancestor = parent;
    std::size_t remaining = components_.size();

    while (ancestor) {
        if (*ancestor >= components_.size() || *ancestor == id || remaining-- == 0)
            return Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::setParent",
                "Rejected condition: *ancestor >= components_.size() || *ancestor == id || remaining-- == 0",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
        ancestor = components_[*ancestor].parent;
    }
    components_[id].parent = parent;

    return true;
}

namespace {
glm::mat4 inversePlacement(const glm::mat4& m) {
    const auto linear=glm::inverse(glm::mat3(m));
    glm::mat4 result(linear);
    result[3]=glm::vec4(-linear*glm::vec3(m[3]),1);
    return result;
}
bool invertiblePlacement(const glm::mat4& m) {
    return TransformSystem::validAffine(m) && std::abs(glm::determinant(glm::mat3(m))) > 1e-18f;
}
}
std::optional<glm::mat4> Scene::placementMatrix(EntityId id) const {
    if (id >= components_.size()) return {};
    const auto& c = components_[id];
    const auto parent = c.parent ? worldMatrix(*c.parent) : std::optional<glm::mat4>{glm::mat4(1)};
    if (!parent) return {};
    const auto matrix = *parent * c.attachment * detail::transformMatrix(c.transform);
    return TransformSystem::validAffine(matrix) ? std::optional<glm::mat4>{matrix} : std::nullopt;
}
bool Scene::setAttachment(EntityId id, const glm::mat4& matrix) {
    if (id >= components_.size() || !invertiblePlacement(matrix)) return false;
    components_[id].attachment = matrix;
    return true;
}
bool Scene::setLocalLayers(EntityId id, const std::vector<glm::mat4>& layers) {
    if (id >= components_.size() || layers.size() > 128) return false;
    glm::mat4 composed(1);
    for (const auto& layer : layers) {
        composed *= layer;
        if (!invertiblePlacement(layer) || !invertiblePlacement(composed)) return false;
    }
    components_[id].localLayers = layers;
    return true;
}
bool Scene::setWorldPlacement(EntityId id, const glm::mat4& world) {
    if (id >= components_.size() || !invertiblePlacement(world)) return false;
    const auto& c = components_[id];
    const auto parent = c.parent ? worldMatrix(*c.parent) : std::optional<glm::mat4>{glm::mat4(1)};
    const auto base = detail::transformMatrix(c.transform);
    if (!parent || !invertiblePlacement(*parent) || !invertiblePlacement(base)) return false;
    return setAttachment(id, inversePlacement(*parent) * world * inversePlacement(base));
}
bool Scene::setParentPreserveWorld(EntityId id, std::optional<EntityId> parent) {
    const auto placement = placementMatrix(id);
    if (!placement) return false;
    const auto oldParent = components_[id].parent;
    const auto oldAttachment = components_[id].attachment;
    if (!setParent(id, parent)) return false;
    if (setWorldPlacement(id, *placement)) return true;
    components_[id].parent = oldParent;
    components_[id].attachment = oldAttachment;
    return false;
}

std::optional<glm::mat4> Scene::worldMatrix(EntityId id) const {
    if (id >= components_.size())
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::worldMatrix",
                    "Rejected condition: id >= components_.size()",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    std::vector<EntityId> chain;
    std::optional<EntityId> current = id;

    while (current) {
        if (*current >= components_.size() || chain.size() >= components_.size())
            return (Diagnostics::failAt(
                        __FILE__, __LINE__, PipelineStage::Scene, "Scene::worldMatrix",
                        "Rejected condition: *current >= components_.size() || chain.size() >= components_.size()",
                        "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                    std::nullopt);
        chain.push_back(*current);
        current = components_[*current].parent;
    }
    std::reverse(chain.begin(), chain.end());

    LayerId layer = MotionSystem::rootLayer();
    std::size_t anchor = chain.size() - 1;

    for (std::size_t i = chain.size(); i > 0; --i) {
        const auto binding = getEntityLayer(chain[i - 1]);

        if (binding) {
            layer = *binding;
            anchor = i - 1;
            break;
        }
    }

    const auto motion = motions_.layerMatrices(layer);

    if (!motion)
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::worldMatrix", "Rejected condition: !motion",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    glm::mat4 prefix(1), suffix(1);

    for (std::size_t i = 0; i < chain.size(); ++i) {
        auto& matrix = i <= anchor ? prefix : suffix;

        const auto& component = components_[chain[i]];
        matrix *= component.attachment * detail::transformMatrix(component.transform);
        for (const auto& local : component.localLayers) matrix *= local;
    }

    const auto result = motion->parent * prefix * motion->local * suffix;

    if (!detail::finiteMatrix(result)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Scene::worldMatrix",
                            "World transform overflow after parent and motion composition.",
                            "Reduce coordinate/scale magnitudes and inspect the parent/layer chain.", id);

        return {};
    }

    return result;
}

std::optional<glm::mat4> Scene::faceWorldMatrix(EntityId id, std::size_t faceIndex) const {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size())
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::faceWorldMatrix",
                "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size()",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
            std::nullopt);

    const auto& face = components_[id].polyhedron.faces[faceIndex];
    const auto world = face.transformMode == FaceTransformMode::Hierarchy
                           ? worldMatrix(id) : std::optional<glm::mat4>(glm::mat4(1));

    if (!world)
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::faceWorldMatrix", "Rejected condition: !world",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    const auto result = *world * face.transformFrame * detail::faceLocalMatrix(face);

    if (!detail::finiteMatrix(result)) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Scene::faceWorldMatrix",
                            "Face world transform overflow.", "Inspect local face animation and entity transforms.",
                            id);

        return {};
    }

    return result;
}

std::optional<glm::mat4> Scene::lineWorldMatrix(LineId id) const {
    if (id >= lines_.size())
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::lineWorldMatrix",
                    "Rejected condition: id >= lines_.size()",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id),
                std::nullopt);

    if (lines_[id].owner)
        return worldMatrix(*lines_[id].owner);

    const auto global = motions_.layerMatrices(MotionSystem::rootLayer());

    return global ? std::optional<glm::mat4>(global->parent * global->local) : std::nullopt;
}

bool Scene::bindEntity(EntityId id, LayerId layer) {
    if (id >= components_.size() || !motions_.getLayer(layer))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::bindEntity",
            "Rejected condition: id >= components_.size() || !motions_.getLayer(layer)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    entityLayers_[id] = layer;

    return true;
}

bool Scene::unbindEntity(EntityId id) {
    if (id >= components_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::unbindEntity",
            "Rejected condition: id >= components_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    entityLayers_.erase(id);

    return true;
}

std::optional<LayerId> Scene::getEntityLayer(EntityId id) const {
    if (id >= components_.size())
        return {};

    const auto found = entityLayers_.find(id);

    if (found == entityLayers_.end() || !motions_.getLayer(found->second))
        return {};

    return found->second;
}

bool Scene::setTransform(EntityId id, const Transform3D& transform) {
    if (id >= components_.size() || !DiagramSystem::validTransform(transform)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTransform",
            "Rejected condition: id >= components_.size() || !DiagramSystem::validTransform(transform)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    components_[id].transform = transform;

    return true;
}

bool Scene::setFaceTransformMode(EntityId id, std::size_t faceIndex, FaceTransformMode mode, bool preserveWorld) {
    if (!getMesh(id, faceIndex) || (mode != FaceTransformMode::Hierarchy && mode != FaceTransformMode::Independent))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
                                   "Invalid entity, face index or transform mode.");
    auto& face = components_[id].polyhedron.faces[faceIndex];
    glm::mat4 frame(1);
    if (preserveWorld) {
        if (mode == face.transformMode) return true;
        const auto world = worldMatrix(id);
        if (!world) return false;
        const auto oldParent = face.transformMode == FaceTransformMode::Hierarchy ? *world : glm::mat4(1);
        frame = oldParent * face.transformFrame;
        if (mode == FaceTransformMode::Hierarchy) {
            const float determinant = glm::determinant(*world);
            if (!std::isfinite(determinant) || std::abs(determinant) < 1e-12f)
                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "setFaceTransformMode",
                                           "Cannot preserve world pose under a singular parent transform.");
            frame = glm::inverse(*world) * frame;
        }
    }
    if (!TransformSystem::validAffine(frame)) return false;
    face.transformMode = mode;
    face.transformFrame = frame;
    return true;
}

bool Scene::setFaceTransform(EntityId id, std::size_t faceIndex, const Transform3D& transform, bool independence) {
    if (id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() ||
        !DiagramSystem::validTransform(transform)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setFaceTransform",
            "Rejected condition: id >= components_.size() || faceIndex >= components_[id].polyhedron.faces.size() || !DiagramSystem::validTransform(transform)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }

    PlaneFigure& face = components_[id].polyhedron.faces[faceIndex];

    face.transform = transform;
    face.independence = independence;

    return true;
}

std::optional<LineId> Scene::addLine(const glm::vec3& start, const glm::vec3& end, GlColor color,
                                     std::optional<EntityId> owner) {
    if (!finiteVector(start) || !finiteVector(end) || !validColor(color) ||
        (owner && getComponent(*owner) == nullptr)) {
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::addLine",
                "Rejected condition: !finiteVector(start) || !finiteVector(end) || !validColor(color) || (owner && getComponent(*owner) == nullptr)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);
    }

    const LineId id = lines_.size();

    lines_.push_back({start, end, color, owner});
    geometryRevision_ = nextGeometryRevision();

    return id;
}

std::optional<LineId> Scene::addVector(const glm::vec3& origin, const glm::vec3& vector, GlColor color,
                                       std::optional<EntityId> owner) {
    return addLine(origin, origin + vector, color, owner);
}

std::optional<std::array<LineId, 3>> Scene::addBasis(const glm::vec3& origin, const glm::mat3& basisVectors,
                                                     const glm::vec3& axisLengths, std::optional<EntityId> owner) {
    if (!finiteVector(origin) || !positiveLengths(axisLengths) || (owner && getComponent(*owner) == nullptr)) {
        return (
            Diagnostics::failAt(
                __FILE__, __LINE__, PipelineStage::Scene, "Scene::addBasis",
                "Rejected condition: !finiteVector(origin) || !positiveLengths(axisLengths) || (owner && getComponent(*owner) == nullptr)",
                "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
            std::nullopt);
    }

    for (int i = 0; i < 3; ++i) {
        if (!finiteVector(basisVectors[i]) || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f ||
            !finiteVector(origin + basisVectors[i] * axisLengths[i])) {
            return (
                Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::addBasis",
                    "Rejected condition: !finiteVector(basisVectors[i]) || glm::dot(basisVectors[i], basisVectors[i]) <= 0.0f || !finiteVector(origin + basisVectors[i] * axisLengths[i])",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                std::nullopt);
        }
    }
    // 사전 검증 후 세 선을 함께 등록한다. 실패 입력은 장면을 변경하지 않는다.
    const std::array<GlColor, 3> colors{GlColor{1.0f, 0.2f, 0.2f}, GlColor{0.2f, 1.0f, 0.2f},
                                        GlColor{0.2f, 0.4f, 1.0f}};
    std::array<LineId, 3> ids{};

    for (int i = 0; i < 3; ++i) {
        ids[i] = lines_.size();
        lines_.push_back({origin, origin + basisVectors[i] * axisLengths[i], colors[i], owner});
    }
    geometryRevision_ = nextGeometryRevision();

    return ids;
}

bool Scene::setVector(LineId id, const glm::vec3& origin, const glm::vec3& vector) {
    if (id >= lines_.size() || !finiteVector(origin) || !finiteVector(origin + vector)) {
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVector",
            "Rejected condition: id >= lines_.size() || !finiteVector(origin) || !finiteVector(origin + vector)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    }
    lines_[id].start = origin;
    lines_[id].end = origin + vector;
    geometryRevision_ = nextGeometryRevision();

    return true;
}

void Scene::clear() {
    motions_.clear();
    entityLayers_.clear();

    if (components_.empty() && lines_.empty()) {
        return;
    }
    polyId_.clear();
    components_.clear();
    lines_.clear();
    geometryRevision_ = nextGeometryRevision();
}

bool DiagramSystem::validTransform(const Transform3D& transform) {
    return finiteVector(transform.basis) && finiteVector(transform.localBasis) && finiteVector(transform.scale) &&
           finiteVector(transform.rotation[0]) && finiteVector(transform.rotation[1]) &&
           finiteVector(transform.rotation[2]) && std::abs(glm::determinant(transform.rotation) - 1.0f) < 0.01f &&
           glm::length(glm::transpose(transform.rotation) * transform.rotation[0] - glm::vec3(1, 0, 0)) < 0.01f &&
           glm::length(glm::transpose(transform.rotation) * transform.rotation[1] - glm::vec3(0, 1, 0)) < 0.01f &&
           glm::length(glm::transpose(transform.rotation) * transform.rotation[2] - glm::vec3(0, 0, 1)) < 0.01f &&
           finiteMatrix(detail::transformMatrix(transform));
}

bool DiagramSystem::validPolyhedron(const Polyhedron& polyhedron) {
    if (polyhedron.faces.empty())
        return false;

    for (const auto& face : polyhedron.faces)
        if (!detail::validMesh(face) || !validTransform(face.transform))
            return false;

    return true;
}

Polyhedron DiagramSystem::makePolyhedron(GLShape shape, const glm::vec3& sideLengths) {
    Polyhedron polyhedron;

    if (!positiveLengths(sideLengths)) {
        return polyhedron;
    }
    polyhedron.shape = shape;

    if (shape == GLShape::Cube) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;

        polyhedron.faces.reserve(6);
        appendFace(polyhedron, GLFace::Rect, {{-x, -y, z}, {x, -y, z}, {x, y, z}, {-x, y, z}}, {.9f, .3f, .3f});
        appendFace(polyhedron, GLFace::Rect, {{x, -y, -z}, {-x, -y, -z}, {-x, y, -z}, {x, y, -z}}, {.3f, .8f, .4f});
        appendFace(polyhedron, GLFace::Rect, {{-x, -y, -z}, {-x, -y, z}, {-x, y, z}, {-x, y, -z}}, {.3f, .5f, .9f});
        appendFace(polyhedron, GLFace::Rect, {{x, -y, z}, {x, -y, -z}, {x, y, -z}, {x, y, z}}, {.9f, .8f, .3f});
        appendFace(polyhedron, GLFace::Rect, {{-x, y, z}, {x, y, z}, {x, y, -z}, {-x, y, -z}}, {.3f, .8f, .9f});
        appendFace(polyhedron, GLFace::Rect, {{-x, -y, -z}, {x, -y, -z}, {x, -y, z}, {-x, -y, z}}, {.8f, .3f, .9f});
    } else if (shape == GLShape::Pyramid) {
        const float x = sideLengths.x * 0.5f;
        const float y = sideLengths.y * 0.5f;
        const float z = sideLengths.z * 0.5f;
        const glm::vec3 frontLeft{-x, -y, z};
        const glm::vec3 frontRight{x, -y, z};
        const glm::vec3 backRight{x, -y, -z};
        const glm::vec3 backLeft{-x, -y, -z};
        const glm::vec3 apex{0.0f, y, 0.0f};

        polyhedron.faces.reserve(5);
        // 앞, 오른쪽, 뒤, 왼쪽, 바닥. 삼각형과 사각형은 바깥쪽 winding이다.
        appendFace(polyhedron, GLFace::Triangle, {frontLeft, frontRight, apex}, {.9f, .3f, .3f});
        appendFace(polyhedron, GLFace::Triangle, {frontRight, backRight, apex}, {.3f, .5f, .9f});
        appendFace(polyhedron, GLFace::Triangle, {backRight, backLeft, apex}, {.3f, .8f, .4f});
        appendFace(polyhedron, GLFace::Triangle, {backLeft, frontLeft, apex}, {.9f, .8f, .3f});
        appendFace(polyhedron, GLFace::Rect, {backLeft, backRight, frontRight, frontLeft}, {.8f, .3f, .9f});
    }

    return polyhedron;
}

glm::mat4 DiagramSystem::modelMatrix(const Transform3D& transform) {
    return detail::transformMatrix(transform);
}

glm::mat4 DiagramSystem::faceModelMatrix(const Component_Poly& component, const PlaneFigure& face) {
    return detail::faceMatrix(component, face);
}

bool CameraSystem::validCamera(const Camera& camera) {
    const glm::vec3 direction = camera.target - camera.position;
    const glm::vec3 right = glm::cross(direction, camera.up);

    return finiteVector(camera.position) && finiteVector(camera.target) && finiteVector(camera.up) &&
           finiteVector(direction) && finiteVector(right) && std::isfinite(glm::dot(direction, direction)) &&
           std::isfinite(glm::dot(right, right)) && glm::dot(direction, direction) > 0.000001f &&
           glm::dot(right, right) > 0.000001f && std::isfinite(camera.fieldOfViewDegrees) &&
           camera.fieldOfViewDegrees > 0.0f && camera.fieldOfViewDegrees < 180.0f && std::isfinite(camera.nearPlane) &&
           std::isfinite(camera.farPlane) && camera.nearPlane > 0.0f && camera.farPlane > camera.nearPlane &&
           std::isfinite(camera.orthographicHalfExtent) && camera.orthographicHalfExtent > 0.0f;
}

std::optional<Camera> CameraSystem::attached(const Camera& local, const glm::mat4& frame) {
    if (!validCamera(local) || !TransformSystem::validAffine(frame))
        return std::nullopt;
    const auto forward = glm::vec3(frame * glm::vec4(local.target - local.position, 0));
    const auto up = glm::vec3(frame * glm::vec4(local.up, 0));
    if (glm::length(forward) < 1e-6f || glm::length(glm::cross(forward, up)) < 1e-6f)
        return std::nullopt;
    auto result = local;
    const auto direction = glm::normalize(forward);
    const auto right = glm::normalize(glm::cross(direction, up));
    result.position = glm::vec3(frame * glm::vec4(local.position, 1));
    result.target = result.position + direction;
    result.up = glm::normalize(glm::cross(right, direction));
    return validCamera(result) ? std::optional<Camera>{result} : std::nullopt;
}

glm::mat4 CameraSystem::viewMatrix(const Camera& camera) {
    return glm::lookAt(camera.position, camera.target, camera.up);
}

glm::mat4 CameraSystem::projectionMatrix(const Camera& camera, float aspect) {
    if (camera.orthographic) {
        const float halfWidth = camera.orthographicHalfExtent * (aspect >= 1.0f ? aspect : 1.0f);
        const float halfHeight = camera.orthographicHalfExtent / (aspect < 1.0f ? aspect : 1.0f);

        return glm::ortho(-halfWidth, halfWidth, -halfHeight, halfHeight, camera.nearPlane, camera.farPlane);
    }

    return glm::perspective(glm::radians(camera.fieldOfViewDegrees), aspect, camera.nearPlane, camera.farPlane);
}

std::optional<EntityId> Scene::addModel(const Model& model, const Transform3D& transform, PivotMode pivot,
                                        const glm::vec3& customPivot) {
    if (!DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot))
        return (Diagnostics::failAt(
                    __FILE__, __LINE__, PipelineStage::Scene, "Scene::addModel",
                    "Rejected condition: !DiagramSystem::validPolyhedron(model) || !detail::finiteVector(customPivot)",
                    "Check finite values, valid IDs, dimensions, indices and parent/layer relationships."),
                std::nullopt);

    auto initial = transform;

    if (pivot == PivotMode::BottomCenter)
        initial.localBasis = detail::bottomCenter(model);
    else if (pivot == PivotMode::Custom)
        initial.localBasis = customPivot;

    auto id = push_entity(model, initial);

    if (id)
        components_[*id].pivotMode = pivot;

    return id;
}

bool Scene::setModel(EntityId id, const Model& model) {
    if (id >= components_.size() || !DiagramSystem::validPolyhedron(model))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setModel",
            "Rejected condition: id>=components_.size() || !DiagramSystem::validPolyhedron(model)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    const auto box = detail::bounds(model);

    if (!detail::finiteVector(box.second - box.first))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setModel",
            "Rejected condition: !detail::finiteVector(box.second-box.first)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = components_[id];

    replacement.polyhedron = model;
    replacement.sideLengths = box.second - box.first;
    replacement.transformOnly = false;

    if (replacement.pivotMode == PivotMode::BottomCenter)
        replacement.transform.localBasis = detail::bottomCenter(model);
    components_[id] = std::move(replacement);
    geometryRevision_ = nextGeometryRevision();

    return true;
}

const Mesh* Scene::getMesh(EntityId id, std::size_t meshIndex) const {
    const auto* c = getComponent(id);

    return c && meshIndex < c->polyhedron.faces.size() ? &c->polyhedron.faces[meshIndex] : nullptr;
}

bool Scene::setMesh(EntityId id, std::size_t meshIndex, const Mesh& mesh) {
    if (!getMesh(id, meshIndex))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setMesh", "Rejected condition: !getMesh(id,meshIndex)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    Model replacement = components_[id].polyhedron;

    replacement.faces[meshIndex] = mesh;

    return setModel(id, replacement);
}

bool Scene::setMeshVertices(EntityId id, std::size_t meshIndex, const std::vector<Vertex>& vertices,
                            const std::vector<std::uint32_t>& triangles) {
    const auto* original = getMesh(id, meshIndex);

    if (!original)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setMeshVertices", "Rejected condition: !original",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *original;
    const auto geometry = Mesh::fromVertices(vertices, triangles);

    replacement.localVertices = geometry.localVertices;
    replacement.vertexNormals = geometry.vertexNormals;
    replacement.texcoords = geometry.texcoords;
    replacement.vertexColors = geometry.vertexColors;
    replacement.triangleIndices = triangles;
    replacement.shape = GLFace::Mesh;
    replacement.outlineIndices.clear();

    if (replacement.outline == OutlineMode::Explicit)
        replacement.outline = OutlineMode::None;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setVertexPosition(EntityId id, std::size_t meshIndex, std::size_t vertexIndex, const glm::vec3& position) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh || vertexIndex >= mesh->localVertices.size() || !detail::finiteVector(position))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVertexPosition",
            "Rejected condition: !mesh||vertexIndex>=mesh->localVertices.size()||!detail::finiteVector(position)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.localVertices[vertexIndex] = position;
    recalculateNormals(replacement);

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setTriangleIndices(EntityId id, std::size_t meshIndex, const std::vector<std::uint32_t>& indices) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTriangleIndices", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.shape = GLFace::Mesh;
    replacement.triangleIndices = indices;

    if (!detail::validMesh(replacement))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTriangleIndices",
            "Rejected condition: !detail::validMesh(replacement)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    recalculateNormals(replacement);

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setTexcoords(EntityId id, std::size_t meshIndex, const std::vector<glm::vec2>& uvs) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setTexcoords", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.texcoords = uvs;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setVertexNormals(EntityId id, std::size_t meshIndex, const std::vector<glm::vec3>& normals) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVertexNormals", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.vertexNormals = normals;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setMaterial(EntityId id, std::size_t meshIndex, const Material& material) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setMaterial", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.material = material;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setOutline(EntityId id, std::size_t meshIndex, OutlineMode mode,
                       const std::vector<std::uint32_t>& explicitEdges) {
    const auto* mesh = getMesh(id, meshIndex);

    if (!mesh)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setOutline", "Rejected condition: !mesh",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto replacement = *mesh;

    replacement.outline = mode;
    replacement.outlineIndices = explicitEdges;

    return setMesh(id, meshIndex, replacement);
}

bool Scene::setLocalBasis(EntityId id, PivotMode mode, const glm::vec3& customPivot) {
    if (id >= components_.size() || !detail::finiteVector(customPivot))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setLocalBasis",
            "Rejected condition: id>=components_.size()||!detail::finiteVector(customPivot)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    auto& c = components_[id];

    if (c.transformOnly && mode == PivotMode::BottomCenter)
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setLocalBasis",
            "Rejected condition: c.transformOnly && mode==PivotMode::BottomCenter",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);

    if (mode == PivotMode::BottomCenter)
        c.transform.localBasis = detail::bottomCenter(c.polyhedron);
    else if (mode == PivotMode::Custom)
        c.transform.localBasis = customPivot;
    c.pivotMode = mode;

    return true;
}

bool Scene::setVisible(EntityId id, bool visible) {
    if (id >= components_.size())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::setVisible", "Rejected condition: id>=components_.size()",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    components_[id].visible = visible;

    return true;
}

bool Scene::splitFace(EntityId id, std::size_t faceIndex) {
    if (!getMesh(id, faceIndex))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Scene, "Scene::splitFace", "Rejected condition: !getMesh(id,faceIndex)",
            "Check finite values, valid IDs, dimensions, indices and parent/layer relationships.", id);
    // The same face remains under its parent; vertices and indices are already face-local storage.
    auto& face = components_[id].polyhedron.faces[faceIndex];

    if (!face.independence) {
        face.transform = Transform3D{};
        face.independence = true;
    }

    return true;
}

bool Scene::setFaceSRT(EntityId id, std::size_t faceIndex, const Transform3D& transform) {
    return setFaceTransform(id, faceIndex, transform, true);
}
} // namespace graphics

// --- Shaders.h ---

namespace graphics::detail {
inline constexpr char vertexShader[] = R"GLSL(#version 330 core
layout(location=0) in vec3 vPos;
layout(location=1) in vec3 vColor;
layout(location=2) in vec2 vTexCoord;
layout(location=3) in vec3 vNormal;
uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat3 uNormalMatrix;
out vec3 outColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 WorldPosition;
void main() {
    gl_Position = uMVP * vec4(vPos, 1.0);
    outColor = vColor;
    TexCoord = vTexCoord;
    Normal = uNormalMatrix * vNormal;
    WorldPosition = (uModel * vec4(vPos, 1.0)).xyz;
}
)GLSL";
inline constexpr char fragmentShader[] = R"GLSL(#version 330 core
in vec3 outColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 WorldPosition;
uniform bool uLighting;
uniform vec3 uLightDirection;
uniform vec3 uLightColor;
uniform float uDirectionalIntensity;
uniform vec3 uViewPosition;
uniform vec3 uAmbientColor;
uniform float uMaterialAmbient;
uniform float uMaterialDiffuse;
uniform float uMaterialSpecular;
uniform float uShininess;
struct PointLight { vec3 position; vec3 color; float intensity; float attenuation; };
uniform int uPointLightCount;
uniform PointLight uPointLights[8];
uniform bool uOverrideColor;
uniform vec3 uOutlineColor;
uniform vec3 uTint;
out vec4 FragColor;
float highlight(vec3 n, vec3 lightDirection, vec3 viewDirection) {
    if (dot(n, lightDirection) <= 0.0) return 0.0;
    return uMaterialSpecular * pow(max(dot(viewDirection, reflect(-lightDirection, n)), 0.0), uShininess);
}
void main() {
    if (uOverrideColor) { FragColor = vec4(uOutlineColor, 1.0); return; }
    vec4 color = vec4(outColor * uTint, 1.0);
    if (uLighting && length(Normal) > 0.0) {
        vec3 n = normalize(Normal);
        vec3 viewDelta = uViewPosition - WorldPosition;
        vec3 viewDirection = viewDelta / sqrt(max(dot(viewDelta, viewDelta), 1e-12));
        vec3 sunDirection = normalize(uLightDirection);
        vec3 illumination = uMaterialAmbient * uAmbientColor
            + (uMaterialAmbient + 0.75 * uMaterialDiffuse * max(dot(n, sunDirection), 0.0)
               + highlight(n, sunDirection, viewDirection)) * uLightColor * uDirectionalIntensity;
        for (int i = 0; i < uPointLightCount; ++i) {
            vec3 delta = uPointLights[i].position - WorldPosition;
            float distanceSquared = dot(delta, delta);
            vec3 direction = delta / sqrt(max(distanceSquared, 1e-12));
            illumination += uPointLights[i].color * uPointLights[i].intensity
                * (uMaterialDiffuse * max(dot(n, direction), 0.0) + highlight(n, direction, viewDirection))
                / (1.0 + uPointLights[i].attenuation * distanceSquared);
        }
        color.rgb *= illumination;
    }
    FragColor = color;
}
)GLSL";
}

// --- Renderer.cpp ---

namespace graphics {
std::string readTextFile(const std::filesystem::path& path) {
    std::ifstream input(path, std::ios::binary);

    if (!input)
        return {};

    std::ostringstream buffer;

    buffer << input.rdbuf();

    auto text = buffer.str();

    if (text.compare(0, 3, "\xEF\xBB\xBF") == 0)
        text.erase(0, 3);

    return text;
}

namespace detail {
inline std::string pathForDiagnostics(const std::filesystem::path& path) {
#if defined(_MSVC_EXECUTION_CHARACTER_SET) && _MSVC_EXECUTION_CHARACTER_SET == 65001
    const auto utf8 = path.u8string();
    return std::string(utf8.begin(), utf8.end());
#else
    return path.string();
#endif
}

GLuint compileShader(GLenum kind, const std::string& sourceText, const std::filesystem::path& sourcePath) {
    const auto shader = glCreateShader(kind);

    if (!shader) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glCreateShader", "Shader allocation failed.");

        return 0;
    }

    const char* text = sourceText.c_str();

    glShaderSource(shader, 1, &text, nullptr);
    glCompileShader(shader);

    GLint success = 0;

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        GLint length = 0;

        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

        std::string log(std::max(length, 1), '\0');

        glGetShaderInfoLog(shader, GLsizei(log.size()), nullptr, log.data());
        Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Shader, kind == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader",
            "GLSL file=" + pathForDiagnostics(sourcePath) + "\n" + log,
            "Read the compiler line number; check GLSL version, declarations and shader inputs/outputs.");
        glDeleteShader(shader);

        return 0;
    }

    return shader;
}

bool validLayout(const VertexLayout& layout) {
    if (!layout.stride || layout.stride > std::size_t(std::numeric_limits<GLsizei>::max()) || layout.attributes.empty())
        return false;
    GLint maximum = 0;
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maximum);
    std::vector<GLuint> used;
    for (const auto& a : layout.attributes) {
        std::size_t bytes = 0;
        switch (a.type) {
        case GL_BYTE: case GL_UNSIGNED_BYTE: bytes = 1; break;
        case GL_SHORT: case GL_UNSIGNED_SHORT: bytes = 2; break;
        case GL_INT: case GL_UNSIGNED_INT: case GL_FLOAT: bytes = 4; break;
        default: return false;
        }
        if (a.location >= GLuint(maximum) || a.components < 1 || a.components > 4 ||
            a.offset > layout.stride || std::size_t(a.components)*bytes > layout.stride-a.offset ||
            (a.integer && (a.normalized || a.type == GL_FLOAT)) ||
            std::find(used.begin(), used.end(), a.location) != used.end())
            return false;
        used.push_back(a.location);
    }
    return true;
}

} // namespace detail

bool Renderer::initShader(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
    const auto vertexText = vertexPath.empty() ? std::string(detail::vertexShader) : readTextFile(vertexPath);
    const auto fragmentText = fragmentPath.empty() ? std::string(detail::fragmentShader) : readTextFile(fragmentPath);

    if (vertexText.empty() || fragmentText.empty())
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Shader, "Renderer::initShader",
            "Shader source is empty or unreadable: vertex=" + detail::pathForDiagnostics(vertexPath) +
                "; fragment=" + detail::pathForDiagnostics(fragmentPath),
            "Use embedded shaders (empty shaderDirectory), or copy both GLSL files to the configured project-relative directory.");
#if defined(_MSVC_EXECUTION_CHARACTER_SET) && _MSVC_EXECUTION_CHARACTER_SET == 65001
    // /utf-8 /FC encodes __FILE__ as UTF-8, not the Windows ANSI code page.
    const auto embeddedSource = std::filesystem::u8path(__FILE__).parent_path() / "Shaders.h";
#else
    const auto embeddedSource = std::filesystem::path(__FILE__).parent_path() / "Shaders.h";
#endif
    const auto vertex =
        detail::compileShader(GL_VERTEX_SHADER, vertexText, vertexPath.empty() ? embeddedSource : vertexPath);

    if (!vertex)
        return false;

    const auto fragment =
        detail::compileShader(GL_FRAGMENT_SHADER, fragmentText, fragmentPath.empty() ? embeddedSource : fragmentPath);

    if (!fragment) {
        glDeleteShader(vertex);

        return false;
    }
    shaderProgramID_ = glCreateProgram();

    if (!shaderProgramID_) {
        glDeleteShader(vertex);
        glDeleteShader(fragment);

        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glCreateProgram",
                                   "Program allocation failed.");
    }
    glAttachShader(shaderProgramID_, vertex);
    glAttachShader(shaderProgramID_, fragment);
    glLinkProgram(shaderProgramID_);
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    GLint success = 0;

    glGetProgramiv(shaderProgramID_, GL_LINK_STATUS, &success);

    if (!success) {
        GLint size = 0;

        glGetProgramiv(shaderProgramID_, GL_INFO_LOG_LENGTH, &size);

        std::string log(std::max(size, 1), '\0');

        glGetProgramInfoLog(shaderProgramID_, GLsizei(log.size()), nullptr, log.data());

        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glLinkProgram", log,
                                   "Match vertex outputs and fragment inputs; inspect the linker log.");
    }
    mvpLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.mvp.c_str());
    modelLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.model.c_str());
    normalLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.normal.c_str());
    lightingLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.lighting.c_str());
    lightDirectionLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.lightDirection.c_str());
    lightColorLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.lightColor.c_str());
    overrideLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.overrideColor.c_str());
    outlineColorLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.outlineColor.c_str());
    tintLocation_ = glGetUniformLocation(shaderProgramID_, config_.uniforms.tint.c_str());

    return mvpLocation_ >= 0 || (config_.uniforms.mvp.empty() && bool(config_.bindUniforms)) || Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Shader, "glGetUniformLocation",
                                                    "Required MVP uniform is missing or optimized away: " + config_.uniforms.mvp);
}

bool Renderer::initialize(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath,
                          const RenderConfig& config) {
    if (!detail::validLayout(config.vertexLayout) ||
        (!config.packVertices && config.vertexLayout.stride != sizeof(RenderVertex)))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::initialize",
                                   "Invalid layout, or custom stride without a vertex packer.");
    const auto nextConfig = config; // Copy before destroying resources; config may alias current settings.
    destroy();
    config_ = nextConfig;

    if (!initShader(vertexPath, fragmentPath)) {
        destroy();

        return false;
    }
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glGenBuffers(1, &ebo_);

    if (!vao_ || !vbo_ || !ebo_) {
        destroy();

        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::initialize",
                                   "VAO/VBO/EBO allocation failed.");
    }
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    for (const auto& attribute : config_.vertexLayout.attributes) {
        const auto pointer = reinterpret_cast<const void*>(attribute.offset);
        if (attribute.integer)
            glVertexAttribIPointer(attribute.location, attribute.components, attribute.type,
                                    GLsizei(config_.vertexLayout.stride), pointer);
        else
            glVertexAttribPointer(attribute.location, attribute.components, attribute.type,
                                   attribute.normalized ? GL_TRUE : GL_FALSE,
                                   GLsizei(config_.vertexLayout.stride), pointer);
        glEnableVertexAttribArray(attribute.location);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return detail::checkGlAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::initialize vertex layout");
}

bool Renderer::upload(const Scene& scene) {

    std::vector<RenderVertex> vertices;
    std::vector<GLuint> indices;
    std::vector<DrawRange> ranges;
    for (auto id : scene.getIds()) {
        const auto* c = scene.getComponent(id);

        if (c && c->transformOnly)
            continue;

        if (!c || !DiagramSystem::validPolyhedron(c->polyhedron)) {
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload",
                                       "Missing component or invalid mesh, indices, colors, UVs or normals.",
                                       "Inspect the entity's mesh data before GPU upload.", id);
        }

        for (std::size_t faceIndex = 0; faceIndex < c->polyhedron.faces.size(); ++faceIndex) {
            const auto& face = c->polyhedron.faces[faceIndex];

            if (vertices.size() + face.localVertices.size() > std::numeric_limits<GLuint>::max()) {
                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload",
                                           "Vertex count exceeds the GPU index range.");
            }

            const auto first = GLuint(vertices.size());
            Mesh normals = face;

            if (normals.vertexNormals.empty())
                recalculateNormals(normals);

            for (std::size_t i = 0; i < face.localVertices.size(); ++i) {
                const auto p = face.localVertices[i];
                const auto n = normals.vertexNormals[i];
                auto color = face.vertexColors.empty() ? face.color : face.vertexColors[i];

                color.red *= face.material.color.red;
                color.green *= face.material.color.green;
                color.blue *= face.material.color.blue;

                const auto uv = face.texcoords.empty() ? glm::vec2(0) : face.texcoords[i];

                vertices.push_back(
                    {{p.x, p.y, p.z}, {color.red, color.green, color.blue}, {uv.x, uv.y}, {n.x, n.y, n.z}});
            }

            const auto triangles = detail::triangles(face);
            const auto outline =
                meshOutline(face, face.outline == OutlineMode::None ? OutlineMode::FeatureEdges : face.outline);

            if (triangles.size() > std::numeric_limits<GLsizei>::max() ||
                outline.size() > std::numeric_limits<GLsizei>::max()) {
                return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload",
                                           "Draw count exceeds GLsizei range.");
            }

            DrawRange range;

            range.entityId = id;
            range.faceIndex = faceIndex;
            range.firstIndex = indices.size();
            range.count = GLsizei(triangles.size());

            for (auto index : triangles)
                indices.push_back(first + index);
            range.outlineFirstIndex = indices.size();
            range.outlineCount = GLsizei(outline.size());

            for (auto index : outline)
                indices.push_back(first + index);
            ranges.push_back(range);

        }
    }

    for (std::size_t lineIndex = 0; lineIndex < scene.getLines().size(); ++lineIndex) {
        const auto& line = scene.getLines()[lineIndex];

        if (vertices.size() > std::numeric_limits<GLuint>::max() - 2) {
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload lines",
                                       "Vertex count exceeds the GPU index range.");
        }

        const auto first = GLuint(vertices.size());

        for (auto p : {line.start, line.end})
            vertices.push_back(
                {{p.x, p.y, p.z}, {line.color.red, line.color.green, line.color.blue}, {0, 0}, {0, 0, 0}});

        DrawRange range;

        range.lineId = lineIndex;
        range.firstIndex = indices.size();
        range.count = 2;
        ranges.push_back(range);
        indices.insert(indices.end(), {first, first + 1});
    }

    if (vertices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / config_.vertexLayout.stride ||
        indices.size() > std::size_t(std::numeric_limits<GLsizeiptr>::max()) / sizeof(GLuint)) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::upload",
                                   "Buffer byte size exceeds GLsizeiptr range.");
    }

    const auto byteCount = vertices.size() * config_.vertexLayout.stride;
    std::vector<std::uint8_t> packed;
    if (config_.packVertices) {
        if (!config_.packVertices(vertices, packed) || packed.size() != byteCount) {
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "packVertices",
                                       "Packer failed or byte count does not match vertex count times stride.");
        }
    } else {
        packed.resize(byteCount);
        if (byteCount)
            std::memcpy(packed.data(), vertices.data(), byteCount);
    }

    if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::VertexInput, "before buffer upload")) {
        return false;
    }
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, GLsizeiptr(packed.size()),
                  packed.empty() ? nullptr : packed.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GLsizeiptr(indices.size() * sizeof(GLuint)),
                  indices.empty() ? nullptr : indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::VertexInput, "glBufferData")) {
        return false;
    }

    drawRanges_ = std::move(ranges);

    return true;
}

bool Renderer::draw(const Scene& scene, const glm::mat4& view, const glm::mat4& projection,
                    const RenderOptions& options) const {

    if (!detail::finiteMatrix(view) || !detail::finiteMatrix(projection))
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Renderer::draw",
                                   "View or projection matrix contains NaN/Inf.");

    for (const auto& range : drawRanges_) {
        glm::mat4 matrix(1);

        if (range.lineId) {
            const auto world = scene.lineWorldMatrix(*range.lineId);

            if (!world)
                return false;
            matrix = *world;
        } else {
            const auto world = scene.faceWorldMatrix(range.entityId, range.faceIndex);

            if (!world)
                return false;
            matrix = *world;
        }

        if (!detail::finiteMatrix(matrix) || !detail::finiteMatrix(projection * view * matrix))
            return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Transform, "Renderer::draw",
                                       "World or MVP matrix contains NaN/Inf.",
                                       "Check parent transforms, scales and motion samples.", range.entityId);
    }
    glUseProgram(shaderProgramID_);
    glBindVertexArray(vao_);

    auto cull = [&](const Mesh* face, const glm::mat4& model) {
        const auto mode = face && face->cull != CullMode::Inherit ? face->cull : options.cull;

        glFrontFace(glm::determinant(glm::mat3(model)) < 0 ? GL_CW : GL_CCW);

        if (!face || mode == CullMode::None || mode == CullMode::Inherit)
            glDisable(GL_CULL_FACE);
        else {
            glEnable(GL_CULL_FACE);
            glCullFace(mode == CullMode::Front ? GL_FRONT : GL_BACK);
        }
    };
    glUniform3f(lightDirectionLocation_, options.lightDirection.x, options.lightDirection.y, options.lightDirection.z);
    glUniform3f(lightColorLocation_, options.lightColor.red, options.lightColor.green, options.lightColor.blue);
    const auto eye = glm::vec3(glm::inverse(view)[3]);
    glUniform3f(glGetUniformLocation(shaderProgramID_, config_.uniforms.viewPosition.c_str()), eye.x, eye.y, eye.z);
    const bool lightOn = options.lighting;
    glUniform3f(glGetUniformLocation(shaderProgramID_, config_.uniforms.ambientColor.c_str()),
                 lightOn ? options.ambientColor.red : 0.0f,
                 lightOn ? options.ambientColor.green : 0.0f,
                 lightOn ? options.ambientColor.blue : 0.0f);
    glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.directionalIntensity.c_str()),
                 lightOn ? options.directionalIntensity : 0.0f);
    glUniform1i(glGetUniformLocation(shaderProgramID_, config_.uniforms.pointLightCount.c_str()),
                 lightOn ? int(options.pointLights.size()) : 0);
    for (std::size_t i=0; lightOn && i<options.pointLights.size(); ++i) {
        const auto& light=options.pointLights[i];
        const auto prefix=config_.uniforms.pointLights + "[" + std::to_string(i) + "].";
        glUniform3f(glGetUniformLocation(shaderProgramID_, (prefix+"position").c_str()), light.position.x,light.position.y,light.position.z);
        glUniform3f(glGetUniformLocation(shaderProgramID_, (prefix+"color").c_str()), light.color.red,light.color.green,light.color.blue);
        glUniform1f(glGetUniformLocation(shaderProgramID_, (prefix+"intensity").c_str()),light.intensity);
        glUniform1f(glGetUniformLocation(shaderProgramID_, (prefix+"attenuation").c_str()),light.attenuation);
    }
    glUniform3f(outlineColorLocation_, options.outlineColor.red, options.outlineColor.green,
                 options.outlineColor.blue);

    if (options.wireframe && options.depthTest) {
        // All surfaces write depth first, so a later object's front face hides earlier back edges.
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        for (const auto& range : drawRanges_) {
            if (range.lineId)
                continue;

            const auto* c = scene.getComponent(range.entityId);
            const auto& face = c->polyhedron.faces[range.faceIndex];

            if (!c->visible || !face.visible)
                continue;

            const auto model = *scene.faceWorldMatrix(range.entityId, range.faceIndex);

            cull(&face, model);

            const auto matrix = projection * view * model;

            glUniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(matrix));
            if (config_.bindUniforms && !config_.bindUniforms(
                    {shaderProgramID_, scene, range.entityId, range.lineId, range.faceIndex, model, view, projection, true}))
                return false;
            glDrawElements(GL_TRIANGLES, range.count, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(range.firstIndex * sizeof(GLuint)));
        }
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    }

    for (const auto& range : drawRanges_) {
        glm::mat4 model(1);
        const Mesh* face = nullptr;
        GlColor tint;

        if (range.lineId) {
            const auto& line = scene.getLines()[*range.lineId];

            if (line.owner) {
                const auto* owner = scene.getComponent(*line.owner);

                if (!owner || !owner->visible)
                    continue;
            }

            const auto world = scene.lineWorldMatrix(*range.lineId);

            if (!world)
                return false;
            model = *world;
        } else {
            const auto* c = scene.getComponent(range.entityId);

            if (!c || !c->visible)
                continue;
            face = &c->polyhedron.faces[range.faceIndex];

            if (!face->visible)
                continue;
            tint = c->tint;

            const auto world = scene.faceWorldMatrix(range.entityId, range.faceIndex);

            if (!world)
                return false;
            model = *world;
        }

        const auto mvp = projection * view * model;

        cull(face, model);

        const float determinant = glm::determinant(glm::mat3(model));
        const auto normalMatrix = std::isfinite(determinant) && std::abs(determinant) > 1e-12f
                                      ? glm::transpose(glm::inverse(glm::mat3(model)))
                                      : glm::mat3(1);

        glUniformMatrix4fv(mvpLocation_, 1, GL_FALSE, glm::value_ptr(mvp));
        glUniformMatrix4fv(modelLocation_, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix3fv(normalLocation_, 1, GL_FALSE, glm::value_ptr(normalMatrix));
        glUniform3f(tintLocation_, tint.red, tint.green, tint.blue);
        glUniform1i(lightingLocation_, face && !face->material.unlit);
        const Material material = face ? face->material : Material{};
        glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.materialAmbient.c_str()), material.ambientStrength);
        glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.materialDiffuse.c_str()), material.diffuseStrength);
        glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.materialSpecular.c_str()), material.specularStrength);
        glUniform1f(glGetUniformLocation(shaderProgramID_, config_.uniforms.shininess.c_str()), material.shininess);
        glUniform1i(overrideLocation_, 0);

        if (config_.bindUniforms && !config_.bindUniforms(
                {shaderProgramID_, scene, range.lineId ? std::nullopt : std::optional<EntityId>(range.entityId),
                 range.lineId, range.faceIndex, model, view, projection, false}))
            return false;

        if (!face || !options.wireframe) {
            glDrawElements(face ? GL_TRIANGLES : GL_LINES, range.count, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(range.firstIndex * sizeof(GLuint)));

            if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Raster, "glDrawElements")) {
                glBindVertexArray(0);
                glUseProgram(0);

                return false;
            }
        }

        if (face && (options.wireframe || face->outline != OutlineMode::None)) {
            glUniform1i(lightingLocation_, 0);
            glUniform1i(overrideLocation_, options.wireframe ? 0 : 1);
            glDepthFunc(GL_LEQUAL);
            glDepthMask(GL_FALSE);
            glDrawElements(GL_LINES, range.outlineCount, GL_UNSIGNED_INT,
                            reinterpret_cast<const void*>(range.outlineFirstIndex * sizeof(GLuint)));
            glDepthFunc(GL_LESS);
            glDepthMask(GL_TRUE);
        }
    }
    glUniform1i(overrideLocation_, 0);
    glBindVertexArray(0);
    glUseProgram(0);

    return detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Raster, "Renderer::draw outlines and cleanup");
}

bool Renderer::renderPass(const Scene& scene, const RenderPass& pass, int width, int height) {

    const auto& camera = pass.camera;
    const auto& options = pass.options;
    const auto& viewport = pass.viewport;

    if (!shaderProgramID_ || !vao_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::renderPass",
                                   "Renderer is not initialized.");

    if (width <= 0 || height <= 0)
        return true;

    if (viewport.x < 0 || viewport.y < 0 || viewport.x > width || viewport.y > height || viewport.width < 0 ||
        viewport.height < 0 || viewport.width > width - viewport.x || viewport.height > height - viewport.y)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Renderer::renderPass",
                                   "Viewport lies outside the framebuffer.");

    if (viewport.width == 0 || viewport.height == 0)
        return true;

    bool validLights=options.pointLights.size()<=8 && std::isfinite(options.directionalIntensity) && options.directionalIntensity>=0;

    for (const auto& light:options.pointLights)
        validLights  =    validLights && detail::finiteVector(light.position) && detail::validColor(light.color) &&
            std::isfinite(light.intensity) && light.intensity>=0 && std::isfinite(light.attenuation) && light.attenuation>=0;

    if (!validLights || !CameraSystem::validCamera(camera) || !detail::finiteVector(options.lightDirection) ||
        glm::length(options.lightDirection) < 1e-12f || !detail::validColor(options.lightColor) ||
        !detail::validColor(options.outlineColor) || !detail::validColor(options.ambientColor) || !std::isfinite(options.background.r) ||
        !std::isfinite(options.background.g) || !std::isfinite(options.background.b) ||
        !std::isfinite(options.background.a) ||
        (options.cull != CullMode::Inherit && options.cull != CullMode::None && options.cull != CullMode::Back &&
         options.cull != CullMode::Front))
        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Transform, "Renderer::renderPass",
            "Invalid camera, light, color or culling mode.",
            "Check near/far planes, FOV, target/up directions, positive orthographic extent and finite render options.");

    if (uploadedScene_ != &scene || uploadedRevision_ != scene.getGeometryRevision()) {
        if (!upload(scene))
            return false;
        uploadedScene_ = &scene;
        uploadedRevision_ = scene.getGeometryRevision();
    }
    if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "before Renderer::render"))
        return false;
    glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    glLineWidth(1);

    if (options.depthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_SCISSOR_TEST);
    glScissor(viewport.x, viewport.y, viewport.width, viewport.height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(options.background.r, options.background.g, options.background.b, options.background.a);

    const GLbitfield mask = (pass.clearColor ? GL_COLOR_BUFFER_BIT : 0) | (pass.clearDepth ? GL_DEPTH_BUFFER_BIT : 0);

    if (mask)
        glClear(mask);

    if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "viewport/scissor/clear")) {
        glDisable(GL_SCISSOR_TEST);

        return false;
    }

    const bool success = draw(scene, CameraSystem::viewMatrix(camera),
                              CameraSystem::projectionMatrix(camera, float(viewport.width) / viewport.height), options);

    glDisable(GL_SCISSOR_TEST);
    glDisable(GL_CULL_FACE);
    glFrontFace(GL_CCW);

    return success;
}

bool Renderer::render(const Scene& scene, const Camera& camera, int width, int height, const RenderOptions& options) {
    RenderPass pass;

    pass.camera = camera;
    pass.options = options;
    pass.viewport = {0, 0, width, height};

    return renderPass(scene, pass, width, height);
}

bool Renderer::renderViews(const Scene& scene, const std::vector<RenderPass>& passes, int width, int height) {
    if (width <= 0 || height <= 0)
        return true;

    if (passes.empty())
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Renderer::renderViews",
                                   "Need at least one render pass.");

    if (!shaderProgramID_ || !vao_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::VertexInput, "Renderer::renderViews",
                                   "Renderer is not initialized.");
    glDisable(GL_SCISSOR_TEST);
    glViewport(0, 0, width, height);
    glDepthMask(GL_TRUE);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClearColor(.08f, .08f, .1f, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!detail::checkGlAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Renderer::renderViews full-frame clear"))
        return false;

    for (const auto& pass : passes)
        if (!renderPass(scene, pass, width, height))
            return false;

    return true;
}

void Renderer::destroy() {
    if (ebo_)
        glDeleteBuffers(1, &ebo_);

    if (vbo_)
        glDeleteBuffers(1, &vbo_);

    if (vao_)
        glDeleteVertexArrays(1, &vao_);

    if (shaderProgramID_)
        glDeleteProgram(shaderProgramID_);

    drawRanges_.clear();
    ebo_ = vbo_ = vao_ = shaderProgramID_ = 0;
    uploadedScene_ = nullptr;
    uploadedRevision_ = std::numeric_limits<std::uint64_t>::max();
}
} // namespace graphics

// --- Application.cpp ---

namespace graphics::detail {
bool& applicationActive() {
    static bool active = false;

    return active;
}

unsigned& contextErrorSerial() {
    static unsigned value = 0;

    return value;
}
}

namespace graphics {

Application::~Application() {
    shutdown();
}

bool Application::configureRenderer(const RenderConfig& config, const std::filesystem::path& vertexPath,
                                    const std::filesystem::path& fragmentPath) {
    if (!ensureContext("configureRenderer", __FILE__, __LINE__))
        return false;
    if (renderer_.initialize(vertexPath, fragmentPath, config))
        return true;
    requestClose();
    return false;
}

void Application::setInitHandler(std::function<bool(Application&)> handler) {
    initHandler_ = std::move(handler);
}

void Application::setUpdateHandler(std::function<void(Application&, float)> handler) {
    updateHandler_ = std::move(handler);
}

void Application::setKeyHandler(std::function<void(Application&, int, int)> handler) {
    keyHandler_ = std::move(handler);
}

void Application::setModifiedKeyHandler(std::function<void(Application&, int, int, int)> handler) {
    modifiedKeyHandler_ = std::move(handler);
}

void Application::setFocusHandler(std::function<void(Application&, bool)> handler) {
    focusHandler_ = std::move(handler);
}

void Application::setRenderHandler(std::function<bool(Application&)> handler) {
    renderHandler_ = std::move(handler);
}

bool Application::ensureContext(const char* operation, const char* file, int line) {
    Diagnostics::locate(file, line, operation);

    if (contextFailed_)
        return false;

    if (!window_ || GetCurrentThreadId() != contextThread_) {
        contextFailed_ = true;

        return Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::ensureContext",
            "창이 없거나 컨텍스트를 생성한 스레드와 다른 스레드에서 사용했습니다.",
            "창 생성·렌더링·종료를 같은 스레드에서 수행하세요. Application::run과 사용자 렌더 콜백을 확인하세요.");
    }
    using CurrentProc = void*(WINAPI*)();

    const auto module = GetModuleHandleW(L"opengl32.dll");
    const auto current =
        module ? reinterpret_cast<CurrentProc>(::GetProcAddress(module, "wglGetCurrentContext")) : nullptr;

    if (nativeContext_ && glfwGetCurrentContext() == window_ && current && current() == nativeContext_)
        return true;

    const auto serial = detail::contextErrorSerial();

    glfwMakeContextCurrent(window_);

    if (serial != detail::contextErrorSerial() || glfwGetCurrentContext() != window_ || !current ||
        !current()) {
        contextFailed_ = true;

        return Diagnostics::fail(
            PipelineStage::Runtime, "glfwMakeContextCurrent",
            "WGL이 요청한 컨텍스트를 연결하지 못하여 GL 명령 전에 렌더링을 중단했습니다.",
            "소스 위치는 연결 실패 호출입니다. 창 수명·렌더 스레드·컨텍스트를 해제/파괴하는 외부 콜백을 먼저 확인하세요. 이상이 없으면 GPU 드라이버·세션을 확인하세요. 이 로그만으로 드라이버 원인을 확정할 수 없습니다.");
    }
    nativeContext_ = current();

    return true;
}

bool Application::initialize(const ApplicationConfig& config) {
    contextThread_ = GetCurrentThreadId();
    contextFailed_ = false;
    nativeContext_ = nullptr;

    const auto root = detail::relativePathInside(detail::executableDirectory(), config.projectRoot);
    glfwSetErrorCallback([](int code, const char* message) {
        ++detail::contextErrorSerial();
        Diagnostics::fail(
            PipelineStage::Runtime, "GLFW/WGL", std::to_string(code) + ": " + (message ? message : "알 수 없는 오류"),
            "표시된 소스 위치·호출 경로에서 컨텍스트 연결이 실패했습니다. Application의 창 수명과 스레드를 먼저 확인하세요. OS/드라이버 원문도 함께 보세요. 렌더 패스마다 별도 WGL 컨텍스트를 만들 필요는 없습니다.");
    });

    if (config.width <= 0 || config.height <= 0)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::initialize",
                                   "창 너비와 높이는 양수여야 합니다.");

    if (!glfwInit())
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwInit",
                                   "GLFW 초기화 실패: 앞선 GLFW 오류를 확인하세요.");
    glfwInitialized_ = true;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    window_ = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);

    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glfwCreateWindow",
                                   "OpenGL 3.3 코어 창 생성 실패.",
                                   "앞선 GLFW 오류와 그래픽 드라이버의 OpenGL 지원을 확인하세요.");
    }

    if (!ensureContext("initialize -> attach new window", __FILE__, __LINE__))
        return false;

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "glewInit", "GLEW 초기화 실패.");
    glfwSwapInterval(config.verticalSync ? 1 : 0);

    const auto vertexPath =
        config.shaderDirectory.empty()
            ? std::filesystem::path{}
            : detail::resolveProjectRelative(root, config.shaderDirectory / "VertexShader.glsl");
    const auto fragmentPath =
        config.shaderDirectory.empty()
            ? std::filesystem::path{}
            : detail::resolveProjectRelative(root, config.shaderDirectory / "FragmentShader.glsl");

    Diagnostics::locate(__FILE__, __LINE__, "initialize -> Renderer::initialize -> shader source/path setup");

    if (!renderer_.initialize(vertexPath, fragmentPath, config.renderConfig))
        return false;
    assets_.emplace(root);

    glfwSetInputMode(window_, GLFW_LOCK_KEY_MODS, GLFW_TRUE);
    glfwSetWindowUserPointer(window_, this);
    glfwSetKeyCallback(window_, [](GLFWwindow* window, int key, int, int action, int mods) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.modifiedKeyHandler_)
                app.modifiedKeyHandler_(app, key, action, mods);
            else if (app.keyHandler_)
                app.keyHandler_(app, key, action);
        });
    });
    glfwSetWindowFocusCallback(window_, [](GLFWwindow* window, int focused) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.focusHandler_)
                app.focusHandler_(app, focused == GLFW_TRUE);
        });
    });
    glfwSetMouseButtonCallback(window_, [](GLFWwindow* window, int button, int action, int mods) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));

        app.dispatch([&] {
            if (!app.mouseHandler_)
                return;

            MouseEvent event;

            event.button = button;
            event.action = action;
            event.mods = mods;
            glfwGetCursorPos(window, &event.windowX, &event.windowY);

            int ww = 0, wh = 0, fw = 0, fh = 0;

            glfwGetWindowSize(window, &ww, &wh);
            glfwGetFramebufferSize(window, &fw, &fh);

            if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
                return;
            event.framebufferX = event.windowX * fw / ww;
            event.framebufferY = event.windowY * fh / wh;
            app.mouseHandler_(app, event);
        });
    });
    glfwSetCursorPosCallback(window_, [](GLFWwindow* window, double x, double y) {
        auto& app = *static_cast<Application*>(glfwGetWindowUserPointer(window));

        app.dispatch([&] {
            if (app.cursorHandler_)
                app.cursorHandler_(app, x, y);
        });
    });

    return true;
}

void Application::shutdown() {
    assets_.reset();

    if (window_ != nullptr) {
        if (!contextFailed_ && ensureContext("shutdown -> GPU cleanup", __FILE__, __LINE__))
            renderer_.destroy();
        else
            renderer_.abandon();
        glfwSetWindowUserPointer(window_, nullptr);
        glfwDestroyWindow(window_);
        window_ = nullptr;
    }

    if (glfwInitialized_) {
        glfwTerminate();
        glfwInitialized_ = false;
    }
}

void Application::requestClose() {
    if (window_ != nullptr) {
        glfwSetWindowShouldClose(window_, GLFW_TRUE);
    }
}

bool Application::renderScene() {
    if (window_ == nullptr) {
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderScene",
                                   "활성 창 또는 컨텍스트가 없습니다.");
    }

    if (!ensureContext("renderScene -> single pass", __FILE__, __LINE__))
        return false;

    int width = 0;
    int height = 0;

    glfwGetFramebufferSize(window_, &width, &height);

    return renderer_.render(scene_, camera_, width, height, renderOptions_);
}

std::array<int, 2> Application::getFramebufferSize() const {
    std::array<int, 2> size{};

    if (window_)
        glfwGetFramebufferSize(window_, &size[0], &size[1]);

    return size;
}

bool Application::renderViews(const std::vector<RenderPass>& passes) {
    if (!window_)
        return Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Framebuffer, "Application::renderViews",
                                   "활성 창 또는 컨텍스트가 없습니다.");

    if (!ensureContext("renderViews -> shared context for all viewports", __FILE__, __LINE__))
        return false;

    const auto size = getFramebufferSize();

    return renderer_.renderViews(scene_, passes, size[0], size[1]);
}

int Application::run(const ApplicationConfig& config) {
    Diagnostics::configure(config.diagnostics);
    Diagnostics::clear();

    if (running_) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "run() 재귀 호출은 허용하지 않습니다.");

        return -1; // 콜백에서 run()을 재귀 호출하지 않는다.
    }

    if (detail::applicationActive()) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "동시에 실행 중인 Application은 하나만 지원합니다.");

        return -1;
    }
#if defined(_MSVC_EXECUTION_CHARACTER_SET) && _MSVC_EXECUTION_CHARACTER_SET == 65001
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
#else
    // Default MSVC execution charset uses the Windows ANSI code page (e.g. CP949).
    SetConsoleOutputCP(GetACP());
#endif
    detail::applicationActive() = true;
    callbackError_ = nullptr;
    running_ = true;

    int exitCode = 0;
    std::exception_ptr failure;

    try {
        if (!initialize(config)) {
            std::cerr << "애플리케이션 초기화 실패.\n";
            exitCode = -1;
        } else if (initHandler_ && !initHandler_(*this)) {
            Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Scene, "Application::initHandler",
                                "사용자 초기화 콜백이 false를 반환했습니다.",
                                "앞선 API 오류와 장면 생성 콜백을 확인하세요.");
            std::cerr << "사용자 장면 초기화 실패.\n";
            exitCode = -1;
        } else {
            double previousTime = glfwGetTime();

            while (!glfwWindowShouldClose(window_)) {
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwPollEvents/input callbacks");
                glfwPollEvents();

                if (callbackError_)
                    std::rethrow_exception(callbackError_);

                if (glfwWindowShouldClose(window_)) {
                    break;
                }

                int width = 0;
                int height = 0;

                glfwGetFramebufferSize(window_, &width, &height);

                if (width <= 0 || height <= 0) {
                    glfwWaitEvents();

                    if (callbackError_)
                        std::rethrow_exception(callbackError_);
                    previousTime = glfwGetTime();
                    continue;
                }

                const double currentTime = glfwGetTime();
                const float deltaTime = static_cast<float>(std::clamp(currentTime - previousTime, 0.0, 0.05));

                previousTime = currentTime;

                if (updateHandler_) {
                    updateHandler_(*this, deltaTime);
                }

                if (glfwWindowShouldClose(window_)) {
                    break;
                }

                if (!scene_.updateMotions(deltaTime)) {
                    std::cerr << "모션 갱신 실패.\n";
                    exitCode = -1;
                    break;
                }

                if (!ensureContext("run -> render callback", __FILE__, __LINE__)) {
                    exitCode = -1;
                    break;
                }

                const bool rendered = renderHandler_ ? renderHandler_(*this) : renderScene();

                if (!rendered) {
                    std::cerr << "렌더링 실패.\n";
                    exitCode = -1;
                    break;
                }
                Diagnostics::locate(__FILE__, __LINE__, "run -> glfwSwapBuffers");

                const auto swapSerial = detail::contextErrorSerial();

                glfwSwapBuffers(window_);

                if (swapSerial != detail::contextErrorSerial()) {
                    contextFailed_ = true;
                    exitCode = -1;
                    break;
                }
            }
        }
    } catch (const std::exception& error) {
        Diagnostics::failAt(
            __FILE__, __LINE__, PipelineStage::Runtime, "Application::run", error.what(),
            "앞선 파이프라인 단계/API 오류를 확인하세요. 사용자 콜백 예외는 자원 정리 후 다시 전달됩니다.");
        failure = std::current_exception();
    } catch (...) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Runtime, "Application::run",
                            "콜백 또는 실행 처리에서 표준 예외가 아닌 예외가 발생했습니다.");
        failure = std::current_exception();
    }
    shutdown();
    running_ = false;
    detail::applicationActive() = false;

    // 사용자 콜백 예외는 컨텍스트 자원을 정리한 뒤 전달한다.
    if (failure)
        std::rethrow_exception(failure);

    return exitCode;
}

void Application::setMouseHandler(std::function<void(Application&, const MouseEvent&)> handler) {
    mouseHandler_ = std::move(handler);
}

void Application::setCursorHandler(std::function<void(Application&, double, double)> handler) {
    cursorHandler_ = std::move(handler);
}

std::optional<Ray> Application::screenRayAt(double x, double y) const {
    if (!window_)
        return {};

    int ww = 0, wh = 0, fw = 0, fh = 0;

    glfwGetWindowSize(window_, &ww, &wh);
    glfwGetFramebufferSize(window_, &fw, &fh);

    if (ww <= 0 || wh <= 0 || fw <= 0 || fh <= 0)
        return {};

    return graphics::screenRay(camera_, x * fw / ww, y * fh / wh, {0, 0, double(fw), double(fh)});
}

std::optional<PickResult> Application::pickAt(double x, double y) const {
    const auto ray = screenRayAt(x, y);
    return ray ? graphics::pick(scene_, *ray, renderOptions_.cull) : std::nullopt;
}

std::optional<PickResult> Application::pickAtCursor() const {
    if (!window_)
        return {};

    double x = 0, y = 0;

    glfwGetCursorPos(window_, &x, &y);

    return pickAt(x, y);
}

AssetLoader& Application::getAssets() {
    if (!assets_)
        throw std::logic_error("AssetLoader는 run 실행 중 콜백에서만 사용할 수 있습니다.");

    return *assets_;
}

ModelResult Application::loadObj(const std::filesystem::path& relative, const LoadOptions& options) {
    return getAssets().loadObj(relative, options);
}

std::optional<EntityId> Application::addObj(const std::filesystem::path& relative,
                                            const Transform3D& transform,
                                            const LoadOptions& options) {
    const auto loaded = loadObj(relative, options);
    if (!loaded) {
        Diagnostics::failAt(__FILE__, __LINE__, PipelineStage::Asset, "Application::addObj", loaded.error,
                            "obj/ 폴더 경로와 .obj 파일 문법·LoadOptions 설정을 확인하세요.");
        return std::nullopt;
    }

    return scene_.addModel(*loaded.model, transform, loaded.pivotMode, loaded.localBasis);
}

std::optional<EntityId> Application::addObjAt(const std::filesystem::path& relative,
                                              const glm::vec3& position,
                                              const LoadOptions& options) {
    Transform3D transform;
    transform.basis = position;
    return addObj(relative, transform, options);
}

// EXAM_TRANSFORMS — 시험 코드에서 호출하는 배치·목표값 접근·자전·공전·계층 패턴
namespace exam {

/// [API] 직육면체의 밑면 중심으로 배치한다. 새 엔티티는 월드에 등록된다.
inline std::optional<EntityId> addCuboidAtBottom(Scene& scene, const glm::vec3& size,
                                                 const glm::vec3& bottomCenter) {
    return scene.addCuboid(size, bottomCenter + glm::vec3(0.0f, size.y * 0.5f, 0.0f));
}

/// [API] 현재값을 목표값 쪽으로 최대 maxStep만큼 이동한다. 위치·각도·속도에 같은 단위를 사용한다.
inline double moveToward(double current, double target, double maxStep) {
    if (!std::isfinite(current) || !std::isfinite(target) || !(maxStep > 0.0))
        return current;

    const double remaining = target - current;
    if (std::abs(remaining) <= maxStep)
        return target;
    return current + std::copysign(maxStep, remaining);
}

enum class Axis { X, Y, Z };

inline glm::vec3 axisDirection(Axis axis) {
    switch (axis) {
    case Axis::X: return {1.0f, 0.0f, 0.0f};
    case Axis::Y: return {0.0f, 1.0f, 0.0f};
    case Axis::Z: return {0.0f, 0.0f, 1.0f};
    }
    return glm::vec3(0.0f);
}

/// @brief 모델 좌표의 점을 최종 월드 좌표로 변환한다.
inline std::optional<glm::vec3> worldPoint(const Scene& scene, EntityId id, const glm::vec3& modelPoint) {
    if (!detail::finiteVector(modelPoint))
        return std::nullopt;
    const auto world = scene.worldMatrix(id);
    if (!world)
        return std::nullopt;
    const auto point = glm::vec3(*world * glm::vec4(modelPoint, 1.0f));
    return detail::finiteVector(point) ? std::optional<glm::vec3>{point} : std::nullopt;
}

/// @brief 도형의 기본·사용자 지정 피벗을 월드 좌표로 읽는다.
/// 엔티티의 localBasis에 최종 worldMatrix를 적용해 조상 변환까지 반영한다.
inline std::optional<glm::vec3> worldPivot(const Scene& scene, EntityId id) {
    const auto* entity = scene.getComponent(id);
    if (!entity)
        return std::nullopt;
    return worldPoint(scene, id, entity->transform.localBasis);
}

/// @brief 기준 자세의 로컬 축으로 절대각을 적용한다. 위치·스케일·피벗은 보존한다.
/// 호출마다 rotation 전체를 설정한다. X/Y/Z를 함께 적용하려면 각 축의 행렬을 먼저 합성한다.
inline bool setSpinAngle(Scene& scene, EntityId id, Axis axis, double degrees,
                         const glm::mat3& initialRotation = glm::mat3(1.0f)) {
    const auto* entity = scene.getComponent(id);
    if (!entity)
        return false;
    const auto delta = TransformSystem::rotation({glm::vec3(0.0f), axisDirection(axis)}, degrees);
    if (!delta)
        return false;

    auto transform = entity->transform;
    transform.rotation = initialRotation * glm::mat3(*delta);
    return scene.setTransform(id, transform);
}

/// [API] 초당 각속도와 dt로 현재 각도를 진행하고 자전을 적용한다. 각속도의 부호가 방향이다.
inline bool advanceSpin(Scene& scene, EntityId id, Axis axis, double& degrees, double degreesPerSecond,
                        double dt, const glm::mat3& initialRotation = glm::mat3(1.0f)) {
    if (!std::isfinite(dt) || dt < 0.0)
        return false;

    const double next = degrees + degreesPerSecond * dt;
    if (!std::isfinite(next))
        return false;

    const double angle = std::remainder(next, 360.0);
    if (!setSpinAngle(scene, id, axis, angle, initialRotation))
        return false;
    degrees = angle;
    return true;
}

/// @brief 현재 자세와 계층을 보존하고 피벗의 월드 위치만 갱신한다.
/// 직접 이동할 엔티티에는 MotionSystem 레이어를 바인딩하지 않는다.
inline bool setWorldPivot(Scene& scene, EntityId id, const glm::vec3& targetWorld) {
    const auto* entity = scene.getComponent(id);
    if (!entity || scene.getEntityLayer(id) || !detail::finiteVector(targetWorld))
        return false;
    const auto currentWorld = worldPoint(scene, id, entity->transform.localBasis);
    if (!currentWorld)
        return false;

    auto frame = entity->attachment;
    if (entity->parent) {
        const auto parentWorld = scene.worldMatrix(*entity->parent);
        if (!parentWorld)
            return false;
        frame = *parentWorld * frame;
    } else {
        const auto root = scene.getMotions().layerMatrices(MotionSystem::rootLayer());
        if (!root)
            return false;
        frame = root->parent * frame;
    }
    if (!invertiblePlacement(frame))
        return false;
    const auto localDelta = glm::vec3(inversePlacement(frame) * glm::vec4(targetWorld - *currentWorld, 0.0f));
    return scene.setPosition(id, entity->transform.basis + localDelta);
}

/// @brief 고정 월드 점을 지나는 X/Y/Z 축 주위를 공전한다. 각도 0의 오프셋을 매번 재사용한다.
inline bool orbitAroundPoint(Scene& scene, EntityId id, const glm::vec3& centerWorld,
                             const glm::vec3& startOffsetWorld, Axis axis, double degrees) {
    const auto rotation = TransformSystem::rotation({glm::vec3(0.0f), axisDirection(axis)}, degrees);
    if (!rotation)
        return false;
    const auto offset = glm::vec3(*rotation * glm::vec4(startOffsetWorld, 0.0f));
    return setWorldPivot(scene, id, centerWorld + offset);
}

/// @brief 현재 중심 객체의 월드 피벗을 지나는 월드 축 주위를 공전한다.
/// 0도일 때의 (공전 물체 피벗 - 중심 피벗)을 한 번 저장한다. 프레임마다 중심을 먼저 갱신한다.
inline bool orbitAroundEntity(Scene& scene, EntityId id, EntityId center, const glm::vec3& startOffsetWorld,
                              Axis axis, double degrees) {
    if (id == center)
        return false;
    const auto pivot = worldPivot(scene, center);
    return pivot && orbitAroundPoint(scene, id, *pivot, startOffsetWorld, axis, degrees);
}

/// @brief 자식을 부모 모델 좌표에 배치한다. 기존 attachment는 초기화한다.
inline bool attachAtLocal(Scene& scene, EntityId child, EntityId parent, const glm::vec3& localPivotPosition) {
    const auto* entity = scene.getComponent(child);
    if (!entity)
        return false;

    auto local = entity->transform;
    local.basis = localPivotPosition;
    if (!DiagramSystem::validTransform(local) || !scene.setParent(child, parent))
        return false;
    return scene.setAttachment(child, glm::mat4(1.0f)) && scene.setTransform(child, local);
}

/// @brief 대상의 로컬 점에 피벗을 맞춘다. 수평 유지 대상은 회전하는 부모에서 분리해 둔다.
inline bool followPoint(Scene& scene, EntityId follower, EntityId target, const glm::vec3& targetModelPoint) {
    if (follower == target)
        return false;
    const auto point = worldPoint(scene, target, targetModelPoint);
    return point && setWorldPivot(scene, follower, *point);
}
} // namespace exam
} // namespace graphics

/*
================================================================================
  [5부] 시험장 빠른 가이드 · 기본 도형 OBJ 목록 · lecture_pdf 참고 색인
================================================================================

--------------------------------------------------------------------------------
0. 시험장 빠른 세팅 (gllib.cpp + obj\ + lecture_pdf\)
--------------------------------------------------------------------------------
  [프로젝트 설정]
  1) MSVC C++ 콘솔 프로젝트, x64, ISO C++17 이상. C/C++ 추가 옵션에 /utf-8 권장.
  2) gllib.cpp를 프로젝트의 소스 항목에 추가하고 템플릿 main()이 있는 .cpp는 빌드에서 제외.
     미리 컴파일된 헤더를 요구하는 프로젝트라면 gllib.cpp의 해당 설정을 '사용 안 함'으로 변경.
  3) obj 폴더는 프로젝트 폴더에 둔다. 예: app_.addObjAt("obj/cuboid.obj", 위치).
     별도 진입점이 꼭 필요할 때: main.cpp에서 #define GLLIB_NO_MAIN 후 #include "gllib.cpp" 하고,
     gllib.cpp 자체는 별도 컴파일 항목에서 제외한다. 매크로는 내장 main()만 숨긴다.

  [시험 PC가 제공해야 하는 항목]
  - 컴파일: 추가 포함 디렉터리에는 GL/glew.h, GLFW/glfw3.h, glm/glm.hpp의 각 상위 루트가 필요.
  - 링크: x64 glfw3dll.lib가 든 폴더를 추가 라이브러리 디렉터리에 지정. 파일의 #pragma가 링크를 요청한다.
    Windows SDK 라이브러리는 MSVC 환경에서 제공한다. 내장 로더에는 glew32.lib/glew32.dll이 필요 없다.
    제공된 프로젝트가 GLEW 라이브러리까지 링크하면 gllib.cpp 사용 시 해당 프로젝트의 설정을 확인한다.
  - 실행: 일치하는 x64 glfw3.dll을 실행 파일 옆 또는 시험 PC가 설정한 DLL 검색 경로에 둔다.
    DLL을 찾지 못하면 main() 이전에 실패한다. 프로젝트 폴더와 실행 파일 폴더는 다를 수 있다.
  - 세 반입 항목에 위 헤더·LIB·DLL은 없다. 인터넷 가능 PC는 EXAM_INSTALL(로컬 dependencies\ 자동 배치), 현장 SDK 조회는 EXAM_OFFLINE.
  - 내장 GLSL과 기본 도형으로 시작한다. EXAM_START를 검색하고, OBJ 확인 시에만 미리보기를 켠다.

--------------------------------------------------------------------------------
1. 기본 실행 골격 및 프레임 루프 순서
--------------------------------------------------------------------------------
  [최소 실행 골격]
    graphics::Application app;
    Console console(app); // run() 종료까지 수명 유지
    app.setInitHandler([&](graphics::Application&) { return console.initScene(); });
    app.setModifiedKeyHandler([&](graphics::Application&, int key, int action, int mods) {
        console.onKey(key, action, mods);
    });
    app.setUpdateHandler([&](graphics::Application&, float dt) { console.onUpdate(dt); });
    app.setFocusHandler([&](graphics::Application&, bool focused) { console.onFocus(focused); });

    graphics::ApplicationConfig config;
    config.title = "OpenGL";
    return app.run(config);

  [run() 내부 실행 순서]
    1) initialize() : GLFW/OpenGL 3.3 Core 창 생성 -> 컨텍스트 바인딩 -> glewInit()
                      -> 내장/외부 GLSL 셰이더 및 VAO/VBO/EBO 초기화
    2) initHandler() 1회 호출 (false 반환 시 종료)
    3) 매 프레임 루프:
       glfwPollEvents() -> 최소화(fb <= 0) 대기 -> updateHandler(dt, 0~0.05초 클램프)
       -> scene_.updateMotions(dt) -> renderHandler() 또는 기본 renderScene()
       -> glfwSwapBuffers()


--------------------------------------------------------------------------------
2. 요구사항 -> 행동 -> API 호출 빠른 매핑
--------------------------------------------------------------------------------
  [정적 장면 · 계층 · 관절]
  - 기본 도형   : Scene::addGroup(), addCuboid(), addPyramid()
  - 외부 .obj   : Application::addObjAt("obj/파일.obj", pos, opts) / addObj() / loadObj()
  - 부모 계층   : Scene::setParent(child, parent) (순환 금지), 월드 유지: setParentPreserveWorld()
  - SRT 공식    : T(basis) * R(rotation) * S(scale) * T(-localBasis)
                  -> 기본 도형은 Transform3D.localBasis, OBJ는 LoadOptions의 Custom 피벗 설정
                  -> basis는 피벗이 도착할 위치이며, 부모가 있으면 부모 좌표계 기준

  [키 입력 · 상태 관리]
  - 단발 토글   : action == GLFW_PRESS 일 때만 상태 반전 (GLFW_REPEAT 무시)
  - 지속 이동   : PRESS/RELEASE에서 pressed_[key] 갱신 -> onUpdate(dt)에서 dt 곱해 누적
  - 포커스 상실 : onFocus(false)에서 pressed_ 등 누름 상태 모두 초기화
  - 장면 초기화 : Scene::clear() 호출 후 기존 EntityId 재사용 금지 (새 ID 다시 저장)

  [모션 · 경로]
  - 단계 생성   : Motion::fixed / rotation / followPath / linear / pendulum
  - 시스템 등록 : scene.getMotions().createSpin() / createOrbit() / pushLayer()
  - 엔티티 연결 : scene.bindEntity(entityId, layerId)
  - 시험용 직접 갱신: graphics::exam의 EXAM_TRANSFORMS. 아래 EXAM_TRANSFORM_PATTERNS에서 호출 예제를 찾는다.

  [다중 카메라 · 뷰포트]
  - 실제 해상도 : app.getFramebufferSize() (DPI 배율 반영 픽셀 크기)
  - 분할 렌더   : std::vector<graphics::RenderPass> 구성 후 app.renderViews(passes)
  - 주의        : 물리·시간 업데이트는 프레임당 1회, 렌더만 패스별로 분할

  [조명 · 재질 · 컬링]
  - 렌더 옵션   : RenderOptions { lighting, directionalIntensity, ambientColor, pointLights(최대 8), cull }
  - 재질 속성   : Material { color, ambientStrength, diffuseStrength, specularStrength, shininess, unlit }
  - 참고        : 내장 셰이더는 방향광 + 점광원 Phong 표면 명암 지원 (투영 그림자 cast shadow는 미지원)

  [오류 진단 · 역추적]
  - 원인 확인   : Diagnostics::lastError() 또는 Diagnostics::failAt(__FILE__, __LINE__, ...)

  [실전 패턴 요약 — 지속 입력 · 수평 유지 · 카메라 부착 · 낙하 재사용]
  - 지속 입력   : onKey의 GLFW_PRESS 필터 앞에서 PRESS/RELEASE로 aHeld_/dHeld_ 갱신,
                  onFocus(false)에서 false 초기화, onUpdate(dt)에서 (dHeld_ - aHeld_) * 속도 * dt 누적
  - 부모/수평   : 같이 회전하는 부품은 setParent(child, parent); 위치만 따라가고 수평 유지할 물체는
                  부모 없이 worldMatrix(parent) * vec4(로컬부착점, 1)로 계산한 xyz를 setPosition에 적용
  - 부착 전환   : 부모를 바꿀 때 현재 월드 자세 보존은 setParentPreserveWorld(id, newParent)
  - 카메라 부착 : CameraSystem::attached(localCam, *scene.worldMatrix(rigId)) 결과를 app_.getCamera()에 반영
  - 낙하 재사용 : initScene에서 도형 N개를 한 번 등록하고 onUpdate에서 y -= speed * dt;
                  y <= 0이면 y = topY로 되돌려 setPosition(id, pos) 호출 (매 프레임 새 엔티티 생성 금지)


  [EXAM_TRANSFORM_PATTERNS — EXAM_UPDATE에서 바로 쓰는 자전·공전·계층]
   - 배치: addCuboidAtBottom(scene, size, bottomCenter). 예: bottomCenter.y=0이면 밑면이 y=0에 놓인다.
   - 목표값 접근: angle = graphics::exam::moveToward(angle, target, speed * dt). 반환 각도를 setSpinAngle에 넘긴다.
   - 계속 도는 동작: PRESS에서 direction을 -1, 0, 1로 정한다. EXAM_RUN에서 프레임 콜백을 연결한 다음
     onUpdate에서 advanceSpin(scene, id, axis, angle, direction * degreesPerSecond, dt)를 호출한다.
  - Console 멤버 예: std::optional<graphics::EntityId> body_, center_;
                    double spinDegrees_ = 0.0, orbitDegrees_ = 0.0;
  - initScene()에서 center_와 body_를 만들고 ID를 확인한다. 다음 예는 body_를 중심에서 월드 +X로 2만큼 떨어뜨려 시작한다.
  - 이미 배치한 위치에서 시작하려면 worldPivot(scene, *body_) - worldPivot(scene, *center_)를 사용한다.
    두 결과의 optional을 확인한 뒤 시작 오프셋을 한 번만 저장한다.

    // onUpdate(float dt) 본문에서 사용. 두 각도는 절대각이며 축마다 다른 값을 쓸 수 있다.
    using graphics::exam::Axis;
    auto& scene = app_.getScene();
    if (!body_ || !center_)
        return;
    spinDegrees_ = std::remainder(spinDegrees_ + 90.0 * dt, 360.0);
    orbitDegrees_ = std::remainder(orbitDegrees_ + 35.0 * dt, 360.0);
    graphics::exam::setSpinAngle(scene, *body_, Axis::Z, spinDegrees_);
    graphics::exam::orbitAroundEntity(scene, *body_, *center_, {2.0f, 0.0f, 0.0f}, Axis::Y, orbitDegrees_);

  - 자전은 기준 자세의 로컬 X/Y/Z를 사용한다. 이미 기울여 놓은 자세를 기준으로 할 때는
    시작할 때 component->transform.rotation을 저장하고 setSpinAngle의 마지막 인수로 전달한다.
    공전은 중심을 지나는 월드 축을 사용한다. X축은 YZ, Y축은 XZ, Z축은 XY 평면에서 움직인다.
    시작 오프셋을 축과 평행하게 주면 제자리다. X축은 {0, 2, 0}, Y/Z축은 {2, 0, 0}부터 사용한다.
    움직이는 중심은 먼저 갱신한다. 공전 쪽 엔티티 자체에는 MotionSystem 레이어를 바인딩하지 않는다.
    고정 월드 점을 중심으로 돌릴 때는 orbitAroundPoint에 중심 좌표를 넘긴다.

  [계층 — 함께 회전 / 월드 자세 유지 / 점만 추종]
    // 둘 다 initScene()에서 이미 만든 유효한 ID라고 가정한다.
    // 1) 자식 피벗을 부모 로컬 y=1.5에 붙인다. 부모가 돌면 자식도 같이 돈다.
    graphics::exam::attachAtLocal(scene, *childId, *parentId, {0.0f, 1.5f, 0.0f});
    // 2) 이미 배치한 물체를 현재 월드 자세 그대로 다른 부모로 넘길 때 쓴다.
    scene.setParentPreserveWorld(*childId, *newParentId);
    // 3) 손을 회전하는 부모에 달지 않고, 팔 갱신 뒤 끝점 위치만 따라간다.
    graphics::exam::followPoint(scene, *handId, *armId, {0.0f, -armLength, 0.0f});

  - attachAtLocal은 기존 attachment를 초기화하고 부모 로컬 위치를 사용한다.
    기존 월드 자세를 유지하는 인계에는 Scene의 setParentPreserveWorld를 사용한다.
    followPoint는 위치만 갱신한다. 손이 이미 팔의 자식이면 먼저 setParentPreserveWorld(hand, std::nullopt)로 분리한다.
    끝점 좌표는 모델 원점 기준이다. 예제의 -armLength는 로컬 원점이 팔 윗끝인 모델에 해당한다.

--------------------------------------------------------------------------------
3. EXAM_PRIMITIVES — cuboid.obj 로드 예제 (obj/ 폴더)
--------------------------------------------------------------------------------
  obj/cuboid.obj는 2 x 1 x 0.6 (X/Y/Z) 직육면체이며 원점은 바닥 중심이다.
  아래는 OBJ를 직접 로드할 때 참고하는 예제다.

  [Console::initScene()에서 한 개 로드]
    // 기본 피벗은 바닥 중심이므로 위치의 y를 바닥 윗면 높이로 준다.
    const auto cuboid = app_.addObjAt("obj/cuboid.obj", {0.0f, 0.0f, 0.0f});
    if (!cuboid)
        return false;
    app_.getScene().setTint(*cuboid, {0.35f, 0.70f, 0.95f});

  [다른 피벗이 필요할 때]
    graphics::LoadOptions hinge;
    hinge.pivot = graphics::PivotMode::Custom;
    hinge.customPivot = {0.0f, 0.0f, -0.3f}; // cuboid의 뒤쪽 아래 모서리 가운데
    const auto panel = app_.addObjAt("obj/cuboid.obj", {0.0f, 1.0f, 0.0f}, hinge);
    if (!panel)
        return false;

  [같은 OBJ를 여러 번 배치할 때]
    const auto loaded = app_.loadObj("obj/cuboid.obj");
    if (!loaded)
        return false;
    auto& scene = app_.getScene();
    for (float x : {-1.5f, 1.5f}) {
        graphics::Transform3D t;
        t.basis = {x, 0.0f, 0.0f};
        const auto id = scene.addModel(*loaded.model, t, loaded.pivotMode, loaded.localBasis);
        if (!id)
            return false;
        // 특정 재질을 전체 메시 조각에 적용할 때는 face 개수만큼 호출한다.
        // for (std::size_t i = 0; i < loaded.model->faces.size(); ++i) scene.setMaterial(*id, i, mat);
    }


--------------------------------------------------------------------------------
4. 지참 가능 lecture_pdf\ 빠른 참고 색인 (필요한 개념 -> 바로 열 PDF)
--------------------------------------------------------------------------------
  [Project1의 lecture_pdf\]
  - 지엘1 오픈지엘 기초.pdf        : OpenGL와 그리기 기초
  - 지엘2 GLSL 기초 1.pdf          : 그래픽스 파이프라인과 셰이더 기초
  - 지엘2 GLSL 기초 2.pdf          : 정점 속성·셰이더 연결·도형 그리기 예제
  - 지엘3 좌표계 변환 1.pdf        : GLM과 모델 변환
  - 지엘3 좌표계 변환 2.pdf        : Object → World → Eye → Clip → Window 좌표
  - 3. 2차원 그래픽스의 변환.pdf  : 동차좌표, 이동·회전·배율 등 기초
  - 지엘4 조명.pdf                : 광원, 재질, 법선과 조명 GLSL

  [사전 학습용 PDF — OpenGL-26-2의 lecture_pdf\homework\. 실습 29는 별도 제공본.]
  - 지엘2 GLSL 기초 (실습 10 11).pdf        : VAO/VBO, 셰이더 컴파일, 기본 그리기
  - 지엘2 GLSL 기초 (실습 12).pdf           : 정점 속성·색상·애니메이션 기초
  - 지엘3 좌표계 변환 (실습 131415).pdf     : 3D 다면체, 모델 변환, 기본 회전/이동
  - 지엘3 좌표계 변환 (실습 1617).pdf       : 자전·공전, 피벗 변환, 와이어/컬링
  - 지엘3 좌표계 변환 (실습 1819).pdf       : 복합 계층 변환, 태양계/다중 공전
  - 지엘3 좌표계 변환 2 (실습 20).pdf       : 경로 추적·반복 배치·거리 시계 응용
  - 지엘3 좌표계 변환 (실습 2122).pdf       : 기울어진 방/좌표계 전환, 다중 뷰포트(RenderPass)
  - 지엘3 좌표계 변환 (실습 23).pdf         : 캐릭터 관절 계층, 충돌·점프·접지, 벽 개방
  - 지엘3 좌표계 변환 (실습 24).pdf         : 물체 부착 시점, 카메라 선택, 트럭 이동과 인계
  - 지엘4 조명 (실습 2627).pdf              : 육면체/사각뿔·구 3개, 광원 공전·반경·색 전환
  - 지엘4 조명 (실습 28).pdf                : 탱크 계층·이동·카메라 공전·점광원 제어 (종합 참고)
  - 지엘4 조명 (실습 29).pdf                : 세 공전 경로, 눈 낙하·재사용, 광원 공전·거리·강도
  - 코딩 시험 문제.pdf                      : 실습 24까지 배운 변환을 새 물체에 적용한 이전 사례


--------------------------------------------------------------------------------
5. 실습 28 아키텍처 및 기본 조작 요약 (참고용)
--------------------------------------------------------------------------------
  [4계층 구조]
    App(main + Console)
      -> Application (창 / 루프 / 입력 콜백 / OBJ 로더)
      -> Scene / MotionSystem / TransformSystem / Picking / AssetLoader
      -> Renderer (GPU VAO / VBO / EBO / Shader)

  [실습 28 주요 키 참고]
    - 방향키   : 탱크 XZ 이동          | h/t/l/g/p : 궤도·몸체·상부교환·포신·깃대
    - w/a/s/d  : 카메라 수평 이동      | r/R, y/Y  : 카메라/점광원 공전 토글
    - m / c / o: 조명 On/Off · 광원색 · 팔레트 전환 | q / Esc : 종료

--------------------------------------------------------------------------------
6. EXAM_GLSL — GLSL 작성·C++ 연결·실행 경로
--------------------------------------------------------------------------------
  [내장 GLSL: gllib.cpp만 사용할 때]
  - graphics::detail::vertexShader / fragmentShader의 R"GLSL(... )GLSL" 안이 GLSL 소스다.
  - ApplicationConfig의 shaderDirectory를 비워 두면 이 두 문자열을 컴파일·링크한다.
  - 기본 레이아웃: location 0 vec3 위치, 1 vec3 색, 2 vec2 UV, 3 vec3 법선.
  - 렌더러가 각 드로우의 P * V * M을 uMVP로 전송한다. 정점 셰이더에서
    gl_Position = uMVP * vec4(vPos, 1.0); 으로 사용한다. 선언만 하고 안 쓰면 최적화로 사라질 수 있다.
  - 정점의 out과 프래그먼트의 in은 이름·타입을 맞춘다. 프래그먼트는 out vec4에 최종 RGBA를 쓴다.
  - 정점 형식은 기존 레이아웃을 재사용한다. 예제에서 사용하지 않는 location 2·3은 생략할 수 있다.

  [색만 출력하는 최소 예제]
  VertexShader.glsl (또는 vertexShader 문자열의 내용)
    #version 330 core
    layout(location = 0) in vec3 vPos;
    layout(location = 1) in vec3 vColor;
    uniform mat4 uMVP;
    out vec3 vertexColor;
    void main() {
        gl_Position = uMVP * vec4(vPos, 1.0);
        vertexColor = vColor;
    }
  FragmentShader.glsl (또는 fragmentShader 문자열의 내용)
    #version 330 core
    in vec3 vertexColor;
    uniform vec3 uTint;
    out vec4 FragColor;
    void main() {
        FragColor = vec4(vertexColor * uTint, 1.0);
    }
  - 이 쌍은 색과 변환 연결을 확인하는 예제다. 기본 조명·외곽선 색이 필요하면 내장 쌍을 유지한다.

  [외부 .glsl 파일을 현장에서 작성할 때]
  1) 파일 이름을 VertexShader.glsl, FragmentShader.glsl로 저장한다. .glsl.txt인지 확인한다.
  2) 두 파일을 프로젝트 폴더의 shaders/ 또는 실행 파일 폴더의 shaders/ 아래에 둔다.
     obj/와 동일하게 실행 폴더 → 작업 폴더 → gllib.cpp 폴더 → 상위 2단계 순으로 자동 탐색한다.
     .glsl 파일은 C++ 컴파일 항목이 아니다. 앱 시작 시 드라이버가 해당 텍스트를 컴파일한다.
  3) EXAM_RUN의 runExam()에서 app.run(config) 전에 config.shaderDirectory = "shaders"; 를 설정한다.
     필요하면 위 설정을 비워 내장 GLSL로 돌아가 수정 전 장면과 비교한다.

  [C++에서 다른 uniform을 연결할 때]
  - 예: 프래그먼트에 uniform float uBrightness; 를 선언하고 출력 RGB에 곱한다.
    runExam()의 app.run(config) 앞에서 다음 콜백을 등록한다.
      config.renderConfig.bindUniforms = [](const graphics::DrawUniformContext& draw) {
          return draw.setUniform("uBrightness", 0.8f);
      };
  - uMVP 이름을 uPVM으로 바꾸었다면 config.renderConfig.uniforms.mvp = "uPVM"; 도 맞춘다.
  - 첫 오류를 확인한다. 열기 실패: 실행 폴더·이름. compile: #version과 오류 줄.
    link: 정점 out/프래그먼트 in. Required MVP: 이름과 gl_Position에서 실제 사용 여부.
================================================================================
*/

// ================================================================================
// [6부] 시험 코드 (#ifndef GLLIB_NO_MAIN)
// --------------------------------------------------------------------------------
// 시험에서는 EXAM_START, EXAM_INPUT, EXAM_UPDATE를 찾아 Console만 편집하면 됩니다.
// ================================================================================

#ifndef GLLIB_NO_MAIN

class Console; ///< 시험장 시작용 기본 컨트롤러 (전방 선언)

// EXAM_LAYOUT: 기준 도형의 치수, 배치, 카메라와 색상.
namespace examLayout {
constexpr glm::vec3 floorSize{10.0f, 0.2f, 10.0f};
constexpr float floorTopY = 0.0f;

constexpr glm::vec3 floorCenter{0.0f, floorTopY - floorSize.y * 0.5f, 0.0f};
constexpr glm::vec3 tankTrackSize{0.65f, 0.6f, 1.8f};
constexpr glm::vec3 tankDeckSize{2.3f, 0.25f, 1.4f};
constexpr glm::vec3 tankUpperSize{1.1f, 0.45f, 0.8f};
constexpr float tankTrackOffsetX = 0.8f;
constexpr float tankSpeed = 1.3f; // 월드 단위/초
constexpr float tankTravelLimit = 3.5f;
constexpr double bodyStepDegrees = 15.0;
constexpr double bodyTurnSpeed = 90.0; // 도/초

constexpr glm::vec3 cameraTarget{0.0f, 0.75f, 0.0f};
constexpr float cameraYawDegrees = 50.0f;
constexpr float cameraPitchDegrees = 22.0f;
constexpr float cameraDistance = 9.5f;

constexpr double cameraOrbitSpeed = 45.0; // 도/초

constexpr bool lightingEnabled = true;
constexpr float directionalIntensity = 0.75f;
constexpr graphics::GlColor ambientColor{0.24f, 0.24f, 0.24f};
constexpr graphics::GlColor floorTint{0.45f, 0.45f, 0.45f};
constexpr graphics::GlColor tankTrackTint{0.48f, 0.50f, 0.53f};
constexpr graphics::GlColor tankBodyTint{0.65f, 0.84f, 0.52f};
constexpr float randomColorMin = 0.25f;
constexpr float randomColorMax = 1.0f;
} // namespace examLayout

// EXAM_PATTERN_INDEX — 시험에서 다시 쓰는 함수
// exam::은 graphics::exam::의 줄임 표기. scene은 app_.getScene().
// 등록·좌표 조회의 optional과 변환 함수의 bool 결과를 확인한다.
//
// EXAM_START · 배치와 계층
// - 중심 좌표로 놓기: scene.addCuboid(size, center).
// - 밑면을 바닥에 맞추기: exam::addCuboidAtBottom(scene, size, bottomCenter).
// - 부모에 붙여 같이 돌기: exam::attachAtLocal(scene, child, parent, pivotInParent).
// - 월드 자세를 유지하며 부모 변경: scene.setParentPreserveWorld(child, newParent).
//   분리할 때는 newParent에 std::nullopt를 넘긴다.
// - 잠시 숨기기: scene.setVisible(id, false). 전체 재구성: scene.clear() 후 새 ID를 저장한다.
//
// EXAM_INPUT / EXAM_UPDATE · 이동과 회전
// - 이동: scene.setPosition(id, position). 부모가 있으면 부모 기준 좌표다.
//   월드 피벗 위치로 옮기려면 exam::setWorldPivot(scene, id, targetWorld).
// - 목표값까지 이동: value = exam::moveToward(value, target, speed * dt).
// - 자전: exam::setSpinAngle(scene, id, axis, degrees)는 기준 자세의 로컬 X/Y/Z축 절대각을 적용한다.
//   연속 자전: exam::advanceSpin(scene, id, axis, degrees, degreesPerSecond, dt).
// - 공전: exam::orbitAroundPoint(scene, id, centerWorld, startOffsetWorld, axis, degrees).
//   움직이는 물체가 중심이면 중심을 먼저 갱신한 뒤 exam::orbitAroundEntity(scene, id, centerId,
//   startOffsetWorld, axis, degrees). 월드 X/Y/Z축과 0도일 때의 시작 오프셋을 사용한다.
// - 점 추종: exam::worldPoint(scene, id, modelPoint) 또는 exam::worldPivot(scene, id)로 좌표를 읽는다.
//   위치만 따라가려면 exam::followPoint(scene, follower, target, targetModelPoint)를 쓴다.
//   공전·점 추종으로 직접 움직이는 객체에는 모션 레이어를 바인딩하지 않는다.
//
// EXAM_MOUSE · 선택, 경계 확인, 드래그
// - app_.pickAt(windowX, windowY) → hit->entityId → graphics::groupWorldAabb(scene, hit->entityId, false).
//   마지막 false는 선택 객체의 경계만 구한다는 뜻이다.
// - app_.screenRayAt(windowX, windowY) → graphics::intersectRayPlane(*ray, planePoint, planeNormal).
//   두 결과를 확인한 뒤 월드 교점을 드래그에 쓴다.

// 시험에서 처음 시작하는 순서
// - EXAM_START에서 고정 바닥과 실습 28 배치를 참고한 육면체 탱크를 만든다.
//   움직이거나 선택할 객체의 ID는 이 클래스에 저장한다.
// - EXAM_INPUT의 GLFW_PRESS 처리에 문제의 키를 추가한다. 누른 동안만 움직이는 문제에서는
//   PRESS와 RELEASE로 상태를 기록하고 onFocus()에서 포커스를 잃으면 해제한다.
// - EXAM_RUN에서 필요한 콜백을 연결한다. 직접 이동·회전은 EXAM_UPDATE에서 dt로 진행한다.
//   자동 재생이 필요한 객체는 Scene의 모션 레이어를 사용하고 같은 움직임을 직접 갱신하지 않는다.
// - EXAM_TRANSFORM_PATTERNS에서 밑면 배치·목표값 접근·자전·공전·부착 패턴을 찾는다.
// - pickAt()은 screenRayAt()으로 만든 월드 광선을 쓴다. 작업 평면 교점은 intersectRayPlane()으로 구한다.

/// [역할] 시험장 시작용 기본 컨트롤러: 장면 초기화·키 입력·프레임 갱신을 담당한다.
class Console {
    graphics::Application& app_;
    std::optional<graphics::EntityId> tankRoot_;
    bool leftHeld_ = false, rightHeld_ = false, upHeld_ = false, downHeld_ = false;
    std::optional<graphics::EntityId> bodyId_, trackId_;
    double bodyAngle_ = 0.0;
    double bodyTargetAngle_ = 0.0;
    bool bodySpinning_ = false;

    double trackAngle_ = 0.0;
    double trackTargetAngle_ = 0.0;
    bool trackSpinning_ = false;

    std::optional<graphics::EntityId> cameraRig_;
    bool cameraDir = false;
    glm::vec3 cameraStartOffset_{ 0.0f };
    double cameraOrbitDegrees_ = 0.0;

    struct SavedFaceColors {
        graphics::EntityId entity;
        std::size_t faceIndex;
        std::vector<graphics::GlColor> colors;
    };
    std::vector<SavedFaceColors> savedFaceColors_;
    void randomizeFaceColors();
    void restoreFaceColors();

public:
    /// [API] Application 참조를 보관하고 컨트롤러를 준비한다.
    explicit Console(graphics::Application& app);

    /// [API] 바닥과 육면체 탱크를 만들고 카메라와 조명을 설정한다.
    bool initScene();

    /// [API] 이동 키 상태를 기록한다. T는 차체 자전, R은 한 바퀴 시작, C/Shift+C는 색상 변경·복원이다.
    void onKey(int key, int action, int mods);

    /// [API] 프레임마다 dt(초)로 차체 각도와 탱크 루트의 XZ 위치를 갱신한다.
    void onUpdate(float dt);

    /// [API] 창이 포커스를 잃으면 방향키 누름 상태를 해제한다.
    void onFocus(bool focused);

    /// [API] 왼쪽 클릭으로 엔티티를 선택하고 피벗과 월드 AABB를 출력한다.
    void onMouse(const graphics::MouseEvent& event);
};

inline Console::Console(graphics::Application& app) : app_(app) {}

// EXAM_START: 바닥과 탱크를 만들고, 부품들을 탱크 루트에 연결한다.
inline bool Console::initScene() {
    using namespace examLayout;

    savedFaceColors_.clear();

    auto& camera = app_.getCamera();
    camera.target = cameraTarget;


    const float yaw = glm::radians(cameraYawDegrees);
    const float pitch = glm::radians(cameraPitchDegrees);
    const float horizontal = cameraDistance * std::cos(pitch);

    camera.position = camera.target + glm::vec3(
        horizontal * std::sin(yaw),
        cameraDistance * std::sin(pitch),
        horizontal * std::cos(yaw));

    auto& options = app_.getRenderOptions();
    options.lighting = lightingEnabled;
    options.directionalIntensity = directionalIntensity;
    options.ambientColor = ambientColor;

    auto& scene = app_.getScene();
    const auto addPart = [&scene](const glm::vec3& size, const glm::vec3& center, graphics::GlColor tint,
                                  std::optional<graphics::EntityId> parent) -> std::optional<graphics::EntityId> {
        const auto id = scene.addCuboid(size, center);
        if (!id || (parent && !scene.setParent(*id, parent)) || !scene.setTint(*id, tint))
            return std::nullopt;

        const auto faceCount = scene.getComponent(*id)->polyhedron.faces.size();
        for (std::size_t face = 0; face < faceCount; ++face) {
            if (!scene.setOutline(*id, face, graphics::OutlineMode::FeatureEdges))
                return std::nullopt;
        }
        return id;
    };

    if (!addPart(floorSize, floorCenter, floorTint, std::nullopt))
        return false;

    


    // 1. 트랙은 탱크 루트에 붙인다. 트랙 밑면이 바닥에 닿는다.
    const glm::vec3 trackSize{
        tankTrackSize.z * 2.0f,
        tankTrackSize.y,
        tankTrackSize.z * 2.0f
    };
    const glm::vec3 trackCenter{ 0.0f, trackSize.y * 0.5f, 0.0f };

    trackId_ = scene.addCuboid(trackSize, trackCenter);
    if (!trackId_)
        return false;

    graphics::Transform3D rootTransform;
    rootTransform.basis.y = floorTopY + trackSize.y;
    const auto root = scene.addGroup(rootTransform);
    if (!root)
        return false;

    // 2. 차체는 트랙에 붙인다. 트랙 중심에서 두 높이의 절반만큼 올린다.
    const glm::vec3 bodyCenter{
        0.0f, tankDeckSize.y * 0.5f, 0.0f
    };

    bodyId_ = addPart(tankDeckSize, bodyCenter, tankBodyTint, root);
    if (!bodyId_)
        return false;

    // 3. 상부는 차체에 붙인다. 차체 중심을 기준으로 같은 방식으로 올린다.
    const glm::vec3 upperSize{
        tankUpperSize.x * 0.5f,
        tankUpperSize.y * 1.5f,
        tankUpperSize.z * 0.5f
    };
    const glm::vec3 upperCenter{
        0.0f, (tankDeckSize.y + upperSize.y) * 0.5f, 0.0f
    };

    if (!addPart(upperSize, upperCenter, tankBodyTint, bodyId_))
        return false;




    cameraRig_ = scene.addGroup();
    if (!cameraRig_)
        return false;

    cameraStartOffset_ = camera.position - camera.target;
    cameraOrbitDegrees_ = 0.0;


    tankRoot_ = root;
    
    bodyAngle_ = bodyTargetAngle_ = 0.0;
    leftHeld_ = rightHeld_ = upHeld_ = downHeld_ = false;
    return true;
}

// EXAM_INPUT: 한 번 할 일은 GLFW_PRESS. 누르는 동안 할 일은 PRESS/RELEASE를 아래 필터 앞에서 처리.
inline void Console::onKey(int key, int action, int mods) {
    // 이동 상태를 먼저 갱신하고, 색상·종료 같은 단발 명령은 PRESS에서만 처리한다.
    if (action == GLFW_PRESS || action == GLFW_RELEASE) {
        const bool held = action == GLFW_PRESS;
        switch (key) {
        case GLFW_KEY_LEFT:  leftHeld_ = held; break;
        case GLFW_KEY_RIGHT: rightHeld_ = held; break;
        case GLFW_KEY_UP:    upHeld_ = held; break;
        case GLFW_KEY_DOWN:  downHeld_ = held; break;
        }
    }

    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_Q) {
        app_.requestClose();
    }
    if (key == GLFW_KEY_C) {
        if (mods & GLFW_MOD_SHIFT)
            restoreFaceColors();
        else
            randomizeFaceColors();
    } 
    if (bodyId_) {
        // T는 여기서 적용하고, R은 onUpdate()에서 진행할 목표각을 정한다.
        if (key == GLFW_KEY_T) {
            const double next = bodyAngle_ + examLayout::bodyStepDegrees;
            if (graphics::exam::setSpinAngle(app_.getScene(), *bodyId_, graphics::exam::Axis::Y, next))
                bodyAngle_ = bodyTargetAngle_ = next;
        } 
        
        else if (key == GLFW_KEY_R) {
            bodySpinning_ = !bodySpinning_;
        }
    }
    if (key == GLFW_KEY_X) {
        cameraDir = !cameraDir;
    }
    if (trackId_) {
        if (key == GLFW_KEY_B) {
            trackSpinning_ = !trackSpinning_;

        }
        if (key == GLFW_KEY_W) {
        }
    }
}

inline void Console::randomizeFaceColors() {
    static std::mt19937 randomEngine{std::random_device{}()};
    std::uniform_real_distribution<float> channel{examLayout::randomColorMin, examLayout::randomColorMax};

    auto& scene = app_.getScene();
    const bool saveOriginal = savedFaceColors_.empty();
    for (const auto id : scene.getIds()) {
        const auto* component = scene.getComponent(id);
        if (!component || component->polyhedron.faces.empty())
            continue;

        const auto& faces = component->polyhedron.faces;
        for (std::size_t i = 0; i < faces.size(); ++i) {
            const graphics::GlColor color{channel(randomEngine), channel(randomEngine), channel(randomEngine)};
            const std::vector<graphics::GlColor> solid(faces[i].localVertices.size(), color);
            if (saveOriginal)
                savedFaceColors_.push_back({id, i, faces[i].vertexColors});
            scene.setFaceVertexColors(id, i, solid);
        }
    }
}

inline void Console::restoreFaceColors() {
    auto& scene = app_.getScene();
    for (const auto& saved : savedFaceColors_)
        scene.setFaceVertexColors(saved.entity, saved.faceIndex, saved.colors);
    savedFaceColors_.clear();
}

// EXAM_UPDATE: dt(초)로 상태를 진행하고, 등록해 둔 EntityId에 최종 변환을 적용한다.
inline void Console::onUpdate(float dt) {
    auto& scene = app_.getScene();
    // 이동 입력이 없어도 회전한다. moveToward()가 마지막 프레임을 목표각에 맞춘다.
    // 활성화된 부품을 매 프레임 로컬 Y축으로 회전한다.
    if (bodyId_ && bodySpinning_) {
        graphics::exam::advanceSpin(
            scene, *bodyId_, graphics::exam::Axis::Y,
            bodyAngle_, examLayout::bodyTurnSpeed, dt);
    }

    if (trackId_ && trackSpinning_) {
        graphics::exam::advanceSpin(
            scene, *trackId_, graphics::exam::Axis::Y,
            trackAngle_, examLayout::bodyTurnSpeed, dt);
    }
    //if (bodyId_ && bodyAngle_ != bodyTargetAngle_) {
    //    const double next =  graphics::exam::moveToward(
    //        bodyAngle_, bodyTargetAngle_, examLayout::bodyTurnSpeed * dt);
    // 
    //    if (graphics::exam::setSpinAngle(scene, *bodyId_, graphics::exam::Axis::Y, next))
    //        bodyAngle_ = next;
    //}
    //if (trackId_ && trackAngle_ != trackTargetAngle_) {
    //    
    //    const double next = graphics::exam::moveToward(
    //        trackAngle_, trackTargetAngle_, examLayout::bodyTurnSpeed * dt);
    //    if (graphics::exam::setSpinAngle(scene, *trackId_, graphics::exam::Axis::Y, next))
    //        trackAngle_ = next;
    //}

    const glm::vec3 direction{float(rightHeld_) - float(leftHeld_), 0.0f,
                              float(downHeld_) - float(upHeld_)};
    if (!tankRoot_)
        return;

    if (direction != glm::vec3(0.0f)) {
        const auto* root = scene.getComponent(*tankRoot_);
        if (!root)
            return;

        auto position = root->transform.basis
            + glm::normalize(direction) * examLayout::tankSpeed * dt;
        const float limit = examLayout::tankTravelLimit;
        position.x = std::clamp(position.x, -limit, limit);
        position.z = std::clamp(position.z, -limit, limit);
        scene.setPosition(*tankRoot_, position);
    }

    // 이동을 반영한 탱크의 주목점을 구한다.
    const auto target = graphics::exam::worldPoint(
        scene, *tankRoot_, examLayout::cameraTarget);
    if (!cameraRig_ || !target)
        return;

    
    cameraOrbitDegrees_ = std::remainder(
        cameraOrbitDegrees_ + examLayout::cameraOrbitSpeed * cameraDir * dt, 360.0);

    if (!graphics::exam::orbitAroundPoint(
        scene, *cameraRig_, *target, cameraStartOffset_,
        graphics::exam::Axis::Y, cameraOrbitDegrees_))
        return;

    if (const auto eye = graphics::exam::worldPivot(scene, *cameraRig_)) {
        auto& camera = app_.getCamera();
        camera.position = *eye;
        camera.target = *target;
    }
   
}

inline void Console::onFocus(bool focused) {
    if (!focused)
        leftHeld_ = rightHeld_ = upHeld_ = downHeld_ = false;
}

// EXAM_MOUSE: 클릭한 엔티티의 피벗과 월드 AABB를 출력한다.
inline void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS)
        return;

    const auto hit = app_.pickAt(event.windowX, event.windowY);
    if (!hit) {
        std::cout << "[Pick] miss\n";
        return;
    }

    const auto& scene = app_.getScene();
    const auto* component = scene.getComponent(hit->entityId);
    // 탱크 부품을 판별한다. 부품 아래에 자식을 추가해도 조상의 루트를 찾는다.
    for (auto part = component; part && part->parent; part = scene.getComponent(*part->parent)) {
        if (part->parent == tankRoot_) {
            std::cout << "탱크 부품 선택\n";
            break;
        }
    }
    std::cout << "[Pivot] entity=" << hit->entityId;
    if (component) {
        const auto& localBasis = component->transform.localBasis;
        const auto& basis = component->transform.basis;
        std::cout << " localBasis=(" << localBasis.x << ", " << localBasis.y << ", " << localBasis.z << ')'
                  << " basis=(" << basis.x << ", " << basis.y << ", " << basis.z << ")\n";
    } else {
        std::cout << " unavailable\n";
    }

    const auto bounds = graphics::groupWorldAabb(scene, hit->entityId, false);
    if (!bounds) {
        std::cout << "[Pick] entity=" << hit->entityId << " AABB unavailable\n";
        return;
    }

    const auto& minimum = bounds->minimum;
    const auto& maximum = bounds->maximum;
    std::cout << "[Pick] entity=" << hit->entityId
              << " world AABB min=(" << minimum.x << ", " << minimum.y << ", " << minimum.z << ')'
              << " max=(" << maximum.x << ", " << maximum.y << ", " << maximum.z << ")\n";
}

// EXAM_RUN — 창 설정, 셰이더 설정, 콜백 연결
static int runExam() {
    graphics::Application app;
    Console console(app);
    app.setInitHandler([&console](graphics::Application&) { return console.initScene(); });
    app.setModifiedKeyHandler([&console](graphics::Application&, int key, int action, int mods) {
        console.onKey(key, action, mods);
    });
    app.setUpdateHandler([&console](graphics::Application&, float dt) { console.onUpdate(dt); });
    app.setFocusHandler([&console](graphics::Application&, bool focused) { console.onFocus(focused); });
    app.setMouseHandler([&console](graphics::Application&, const graphics::MouseEvent& event) {
        console.onMouse(event);
    });

    graphics::ApplicationConfig config;
    config.title = "OpenGL";
    return app.run(config);
}
#endif

/*
EXAM_INSTALL — PowerShell 1회 의존성 설치 (Windows 기본 PS 5.1 / PS 7 모두 지원)
VS 솔루션 탐색기에서 프로젝트 우클릭 → '터미널에서 열기'로 gllib.cpp 폴더에서 아래 블록을 붙여 넣는다.
프로젝트 옆 dependencies\에 바로 배치하므로 VS 추가 포함/라이브러리 디렉터리를 수동 입력할 필요가 없다.
중간 실패 시 같은 명령을 다시 실행하면 누락된 파일만 다시 받으며, x64\Debug와 x64\Release에 glfw3.dll도 복사한다.

& {
    $ErrorActionPreference = 'Stop'
    $ProgressPreference = 'SilentlyContinue'
    [Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12
    if (!(Test-Path -LiteralPath 'gllib.cpp' -PathType Leaf)) {
        throw 'gllib.cpp가 있는 프로젝트 폴더에서 실행하세요. (VS: 프로젝트 우클릭 -> 터미널에서 열기)'
    }

    $req = @(
        'dependencies\glew-2.3.1\include\GL\glew.h',
        'dependencies\glfw-3.4.bin.WIN64\include\GLFW\glfw3.h',
        'dependencies\glfw-3.4.bin.WIN64\lib-vc2022\glfw3dll.lib',
        'dependencies\glm\glm\glm.hpp',
        'dependencies\dll\glfw3.dll'
    )
    if (@($req | Where-Object { !(Test-Path -LiteralPath $_ -PathType Leaf) }).Count -gt 0) {
        $null = New-Item -ItemType Directory -Force -Path 'dependencies\dll'
        $tmp = Join-Path $env:TEMP ('gl_deps_' + [guid]::NewGuid().ToString('N'))
        $null = New-Item -ItemType Directory -Force -Path $tmp
        try {
            $packages = [ordered]@{
                'glew' = 'https://github.com/nigels-com/glew/releases/download/glew-2.3.1/glew-2.3.1-win32.zip'
                'glfw' = 'https://github.com/glfw/glfw/releases/download/3.4/glfw-3.4.bin.WIN64.zip'
                'glm'  = 'https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip'
            }
            foreach ($name in $packages.Keys) {
                $zip = Join-Path $tmp "$name.zip"
                Invoke-WebRequest -UseBasicParsing -Uri $packages[$name] -OutFile $zip
                if ($name -eq 'glm') {
                    $stage = Join-Path $tmp 'glm'
                    Expand-Archive -LiteralPath $zip -DestinationPath $stage -Force
                    $glmHpp = Get-ChildItem -LiteralPath $stage -Recurse -File -Filter 'glm.hpp' |
                        Where-Object { $_.Directory.Name -eq 'glm' } | Select-Object -First 1
                    if (!$glmHpp) { throw 'GLM 압축에서 glm/glm.hpp를 찾지 못했습니다.' }
                    if (Test-Path -LiteralPath 'dependencies\glm') { Remove-Item -LiteralPath 'dependencies\glm' -Recurse -Force }
                    $null = New-Item -ItemType Directory -Force -Path 'dependencies\glm'
                    Copy-Item -LiteralPath $glmHpp.Directory.FullName -Destination 'dependencies\glm\glm' -Recurse -Force
                } else {
                    Expand-Archive -LiteralPath $zip -DestinationPath 'dependencies' -Force
                }
            }
            Copy-Item -LiteralPath 'dependencies\glfw-3.4.bin.WIN64\lib-vc2022\glfw3.dll' -Destination 'dependencies\dll\glfw3.dll' -Force
        } finally {
            if (Test-Path -LiteralPath $tmp) { Remove-Item -LiteralPath $tmp -Recurse -Force }
        }
    }

    foreach ($outDir in 'x64\Debug', 'x64\Release') {
        $null = New-Item -ItemType Directory -Force -Path $outDir
        Copy-Item -LiteralPath 'dependencies\dll\glfw3.dll' -Destination (Join-Path $outDir 'glfw3.dll') -Force
    }
    Write-Host '[완료] dependencies\ 설치 및 x64\Debug, x64\Release에 glfw3.dll 복사 완료.'
    Write-Host '출력 폴더가 다른 경우: Copy-Item dependencies\dll\glfw3.dll <EXE폴더>\ -Force'
}

[프로젝트 로컬 dependencies 레이아웃 (gllib.cpp 178·194·206행 자동 탐색 경로)]
  dependencies\glew-2.3.1\include\GL\glew.h
  dependencies\glfw-3.4.bin.WIN64\include\GLFW\glfw3.h
  dependencies\glfw-3.4.bin.WIN64\lib-vc2022\glfw3dll.lib
  dependencies\glm\glm\glm.hpp
  dependencies\dll\glfw3.dll  (실행 시 EXE 옆 x64\Debug\ 또는 x64\Release\에 위치)

EXAM_PDF_REVIEW — OpenGL-26-2의 사전 학습용 추가 PDF
아래 파일은 원본 OpenGL-26-2의 lecture_pdf에 있다. Project1의 반입 PDF 목록은 위 시험 안내를 본다.
  1. 컴퓨터 그래픽스 개요 시스템.pdf      : 파이프라인 전반, 좌표계 흐름, 버퍼와 렌더 기초
  2. 2차원 그래픽스의 기본요소.pdf        : 프리미티브, 정점·면 구성, 래스터 기초
  4. 2차원 그래픽스의 윈도우와 뷰포트.pdf : 윈도우→뷰포트 매핑, 종횡비와 다중 뷰
  5. 조명모델.pdf                       : Ambient/Diffuse/Specular Phong 수식과 벡터
  6. 3차원 그래픽스의 기하변환.pdf        : 3D 축 회전, 임의 축 회전, 뷰와 투영
*/

/*
EXAM_OFFLINE — 시험장 제공 SDK 위치를 한 번에 확인 (인터넷 차단 환경용)
위 EXAM_INSTALL을 쓸 수 없는 오프라인 환경에서 제공 SDK 폴더를 조회해 Visual Studio 경로를 출력한다.

& {
    $ErrorActionPreference = 'Stop'
    $sdkRoot = Read-Host '시험장에 제공된 SDK 폴더 경로'
    if (!(Test-Path -LiteralPath $sdkRoot -PathType Container)) {
        throw "SDK 폴더를 찾지 못했습니다. 제공 경로를 확인하세요. $sdkRoot"
    }

    $includeDirs = foreach ($name in 'GL/glew.h', 'GLFW/glfw3.h', 'glm/glm.hpp') {
        $suffix = '\' + $name.Replace('/', '\')
        $header = Get-ChildItem -LiteralPath $sdkRoot -Recurse -File -Filter (Split-Path $name -Leaf) |
            Where-Object { $_.FullName.EndsWith($suffix, [StringComparison]::OrdinalIgnoreCase) } |
            Select-Object -First 1
        if (!$header) { throw "필수 헤더를 찾지 못했습니다. $name" }
        $header.Directory.Parent.FullName
    }
    "IncludeDirectories = " + ($includeDirs -join ';')

    $dllLibs = @(Get-ChildItem -LiteralPath $sdkRoot -Recurse -File -Filter 'glfw3dll.lib')
    $staticLibs = @(Get-ChildItem -LiteralPath $sdkRoot -Recurse -File -Filter 'glfw3.lib')
    $dlls = @(Get-ChildItem -LiteralPath $sdkRoot -Recurse -File -Filter 'glfw3.dll')
    if ($dllLibs.Count) {
        foreach ($lib in $dllLibs) {
            "LibraryDirectory (DLL용) = " + $lib.DirectoryName
            $dll = Join-Path $lib.DirectoryName 'glfw3.dll'
            if (Test-Path -LiteralPath $dll -PathType Leaf) { "  RuntimeDll = $dll" }
        }
    } elseif ($staticLibs.Count) {
        Write-Host '[주의] glfw3dll.lib 대신 정적 glfw3.lib만 발견되었습니다.'
        foreach ($lib in $staticLibs) { "LibraryDirectory (정적) = " + $lib.DirectoryName }
        Write-Host '-> gllib.cpp의 #define GLFW_DLL을 주석 처리하고 링크 설정의 glfw3dll.lib를 glfw3.lib로 바꾸세요.'
    } else {
        throw 'glfw3dll.lib 또는 glfw3.lib를 찾지 못했습니다. 현장에서 제공한 x64 배포본을 확인하세요.'
    }
    if ($dlls.Count) {
        '제공된 DLL 후보:'
        $dlls | ForEach-Object { $_.FullName }
    }
}

1. 위 IncludeDirectories를 C/C++ → 일반 → 추가 포함 디렉터리에 붙여 넣는다.
2. LibraryDirectory 후보 중 현장 제공 x64 MSVC 배포본의 폴더를 링커 → 일반 → 추가 라이브러리 디렉터리에 넣는다.
   x86과 x64 또는 서로 다른 배포본의 LIB와 DLL을 섞지 않는다. 경로만으로 아키텍처가 불명확하면 제공 구성을 확인한다.
3. x64·C++17 이상·PCH 사용 안 함을 확인하고 빌드한다. 출력의 EXE를 찾아 같은 배포본 GLFW DLL을 옆에 둔다.
4. Ctrl+F5로 바닥과 탱크가 보이는지 확인하고 Q 또는 Esc로 닫는다. 여기까지 되면 EXAM_START에서 구현을 시작한다.

[시험에서 바꿀 부분 고르기]
- 문제의 첫 장면을 EXAM_START에서 만든다. 처음에는 내장 GLSL과 기본 도형으로 모델의 크기·좌표·카메라를 확인한다.
- 움직이는 도형의 EntityId, 키 누름 상태, 현재 각도와 목표만 Console 멤버에 둔다.
- 단발 토글은 EXAM_INPUT의 GLFW_PRESS 뒤에서, 누르는 동안 움직임은 PRESS/RELEASE를 필터 앞에서 처리한다.
  창이 포커스를 잃을 때 누름 상태를 초기화한다. 화면에 반영할 위치와 각도는 EXAM_UPDATE에서 dt로 계산한다.
- 경첩: localBasis는 모델 안의 경첩점, basis는 피벗이 도착하는 위치다. 부모가 있으면 basis는 부모 기준이다.
- 부착: 같이 도는 물체는 setParent. 위치만 따라가면 worldMatrix로 부착점만 계산해 월드에서 배치한다.
- 낙하: initScene에서 여러 ID를 저장하고 update에서 높이만 바꾼다. 바닥에 닿으면 같은 ID의 위치를 위로 돌린다.
- 카메라: 물체에 붙이면 CameraSystem::attached. 창을 나누면 getFramebufferSize → RenderPass → renderViews.
- 적용 예제는 상단 04~08. 시작 → 키 입력/해제 → 중간 반전 또는 재생 → 리셋 → 종료를 화면에서 확인한다.
  OBJ가 필요하면 같은 안내의 상대 경로와 피벗을, 외부 GLSL이 필요하면 EXAM_GLSL을 확인한다.
*/

/*
EXAM_REUSE_REFERENCE — 재사용 함수와 경계 검사 응용

1. 배치와 상태 갱신
- initScene: 바닥, 고정물, 회전축, 움직일 메시 순으로 만든다. 움직일 엔티티의 ID를 저장한다.
  addCuboid(size, center)의 size는 전체 길이다. 밑면을 맞출 때는
  graphics::exam::addCuboidAtBottom(scene, size, bottomCenter)를 사용한다.
- 회전축과 경첩: scene.addGroup()으로 축을 만든 뒤 exam::attachAtLocal(scene, child, parent, localPos)로
  메시를 배치한다. 길이 L인 팔을 관절에서 아래로 뻗으려면 메시 중심을 (0, -L/2, 0)에 둔다.
  Transform3D::localBasis는 모델 내부 피벗, basis는 부모 기준 피벗 위치다.
- 좌표: scene.setPosition()은 부모가 있으면 부모 기준이다. exam::worldPoint()와 exam::worldPivot()으로
  월드 위치를 읽고, 목표가 월드 좌표이면 exam::setWorldPivot()을 쓴다.
- 회전: exam::setSpinAngle()은 기준 자세에 도 단위 절대각을 적용한다.
  계속 돌릴 때는 exam::advanceSpin()에 도/초와 dt를 넣는다.
  여러 축은 회전 행렬을 먼저 합성해 scene.setTransform()으로 한 번 적용한다.
- 열고 닫기: 키에서 목표각만 바꾸고, onUpdate에서 exam::moveToward(current, target, speed * dt)로
  현재각을 갱신한다. 왕복은 위상으로 각도를 계산하거나 Motion::pendulum()을 사용할 수 있다.
- 공전: exam::orbitAroundPoint()에는 0도일 때의 시작 오프셋을 매번 넘긴다.
  중심도 움직이면 중심을 먼저 갱신한 뒤 exam::orbitAroundEntity()를 호출한다.
  원·타원·다각형 경로가 필요하면 PathSystem::circle(), ellipse(), polygon()을 사용한다.
- 위치 추종: 부모의 자세도 이어받으려면 setParent(), 점의 위치만 따라가려면 exam::followPoint().
  저울형 크레인은 기둥 Y 회전 → 위 몸체 Z 회전 → 연결점 이동 → 팔 위치와 회전 → 손 위치 순으로 갱신한다.
  손을 Y축으로만 도는 기둥 그룹 아래에 두고 팔의 (0, -L, 0)을 따라가게 하면 수평을 유지한다.
  setWorldPivot(), followPoint(), orbitAroundPoint()로 직접 옮길 엔티티에는 모션 레이어를 직접 바인딩하지 않는다.
- 입력: 단발 명령은 GLFW_PRESS, 누르는 동안 이동은 PRESS/RELEASE에서 상태를 기록한다.
  onFocus(false)에서 누름 상태를 지우고, onUpdate(dt)에서 거리와 각도를 진행한다.
  등록·좌표 조회 결과의 optional을 확인하고, 변환 API가 false면 실패 원인을 확인한다.

2. 바운더리와 교차 검사
- 입력 범위: std::clamp(값, 최소, 최대). 예: 팔이 바깥 시작점에서 안으로 최대 5회 이동하면 단계를 [0, 5]로
  제한하고 단계에서 연결점의 X를 계산한다. 위치를 부동소수점으로 계속 더하는 대신 정수 단계를 보관한다.
- 모델 로컬 범위: graphics::detail::bounds(model)은 모델 정점의 min/max를 구하는 내부 함수다.
  월드 범위: graphics::groupWorldAabb(scene, id, true)는 하위 엔티티까지 포함한다.
  false는 해당 엔티티의 메시만 검사한다. 메시가 없거나 면 변환 계산에 실패하면 nullopt를 처리한다.
  이 함수는 표시 여부로 면을 거르지 않으므로 숨긴 부품도 경계에 들어갈 수 있다.
- 바닥 밖 이탈: 이동 후보 자세를 임시 적용하고 월드 AABB를 다시 구한다. 물체의 min.x/z와 max.x/z가
  바닥의 min.x/z와 max.x/z 안에 모두 들어오면 채택한다. 벗어나거나 경계를 구하지 못하면 이전 자세로 복원한다.
  회전으로 외곽이 바뀌는 경우에도 후보 자세에서 검사한다. 루트 위치만 제한하면 부품이 넘어갈 수 있다.
- 바닥 착지: 낙하 방향이 -Y라면 후보 AABB의 minimum.y가 바닥 maximum.y보다 낮아질 때 관통 깊이만큼 위로
  보정한다. 유한한 바닥에서 착지를 판정하려면 후보와 바닥의 X/Z 범위도 확인한다.
- 장애물: graphics::overlaps(a, b)는 두 월드 AABB가 겹치거나 맞닿으면 true다. 통과를 막으려면 후보 위치에서
  검사 후 채택 또는 복원한다. 접촉은 허용하고 관통만 막으려면 축별 구간 비교에 엄격 부등호를 쓴다.
  회전한 메시의 상자는 빈 공간을 포함할 수 있고, 큰 한 프레임 이동은 얇은 장애물을 건너뛸 수 있다.
- 클릭 선택: app_.pickAt(windowX, windowY) → hit의 entityId와 faceIndex → 필요 시 대상의 월드 AABB를 출력한다.
  마우스 드래그는 app_.screenRayAt() → graphics::intersectRayPlane() → exam::setWorldPivot() 순서로 적용한다.
  교점이 없으면 기존 위치를 유지한다. 작업 평면이 바닥이면 법선을 (0, 1, 0)으로 두고 X/Z 이동 범위를 확인한다.
- 데이터 검사: DiagramSystem::validPolyhedron(), validTransform(), TransformSystem::validAffine(),
  CameraSystem::validCamera()로 메시·변환·카메라를 확인한다. setParent()는 잘못된 ID와 순환을 거부한다.
  실패한 호출은 반환값과 graphics::Diagnostics::lastError()를 확인한다.
*/

// P2 유의사항 — 전역 모션이 적용된 상태에서 setParentPreserveWorld()로 부모를 바꾸면 월드 위치가 튈 수 있다.
// initScene()에서 setParent()로 연결한 뒤 부모를 이동·회전하면 자식은 렌더링 시 부모 변환을 함께 받는다.
