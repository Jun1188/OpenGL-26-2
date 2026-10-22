# standard 전체 코드·설정 변경 전후

- 날짜: 2026-10-22.
- 작업 시작 전 standard 폴더는 없었다. 아래 새 작성 파일의 before는 모두 없음이다.
- 단계 1 커밋: 39e3dd0 — 독립 프로젝트, Console 샘플, 공용 프레임워크의 전체 사본.
- 단계 2: README·사용자 확인 절차·이 문서·구조 분석 갱신. 해당 커밋은 완료 보고에 기록한다.
- 실습 1~19와 루트 framework의 소스·설정·문서는 이 샘플 생성에서 수정하지 않았다. 이전 작업의 미커밋 변경은 그대로 보존한다.
- 공통 Motion·Transform·레이어 API는 앞선 프레임워크 작업에서 구현한 버전을 재사용했다. 이번에 공통 API를 더 추가할 필요가 없어 루트나 과제 사본을 다시 수정하지 않았다.
- 빌드·실행·테스트·검증기·화면·다른 PC 배포·사용자 이해 확인은 수행하지 않았다.

## standard/main.cpp

입력: Application과 Console. 상태: 등록된 콜백. 출력: run의 종료 코드. main은 샘플 수명과 연결만 소유한다.

before: 없음

after:

```cpp
#include "Console.h"

int main() {
    graphics::Application app;
    Console sample(app);

    app.setInitHandler([&](graphics::Application&) {
        return sample.initScene();
    });
    app.setKeyHandler([&](graphics::Application&, int key, int action) {
        sample.onKey(key, action);
    });
    app.setMouseHandler([&](graphics::Application&, const graphics::MouseEvent& event) {
        sample.onMouse(event);
    });

    graphics::ApplicationConfig config;
    config.title = "standard - Motion and layers";
    return app.run(config);
}
```

## standard/Console.h

공유 Motion/Layer ID와 키 상태를 Console에 둔다. 프레임워크는 과제 전용 상태를 알 필요가 없다.

before: 없음

after:

```cpp
#pragma once
#include "framework/Framework.h"

class Console {
    graphics::Application& app_;
    std::optional<graphics::MotionId> pathId_;
    std::optional<graphics::MotionId> spinId_;
    std::optional<graphics::MotionId> globalId_;
    std::optional<graphics::LayerId> mirrorLayer_;
    std::optional<graphics::LayerId> normalSpinLayer_;
    std::optional<graphics::LayerId> insertedLayer_;
    bool paused_ = false;
    bool spinEnabled_ = true;
    bool globalEnabled_ = false;
    int reflection_ = 1;

    bool choosePath(int kind);
    bool changeReflection();
    bool toggleInsertedLayer();
    void printHelp() const;

public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene();
    void onKey(int key, int action);
    void onMouse(const graphics::MouseEvent& event);
};
```

## standard/Console.cpp

initScene/choosePath/changeReflection/toggleInsertedLayer/onKey/onMouse/printHelp 전체다. 사용자 입력이 설정을 바꾸고, 시간 갱신과 최종 행렬 계산은 Application과 MotionSystem에 맡긴다.

before: 없음

after:

```cpp
#include "Console.h"
#include <iostream>

bool Console::choosePath(int kind) {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);

    if (kind == 1) {
        path.path = graphics::PathSystem::ellipse(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 2) {
        path.path = graphics::PathSystem::rectangle(center, {.8f, 0, 0}, {0, .5f, 0});
    } else if (kind == 3) {
        path.path = graphics::PathSystem::polygon({
            {1, -.5f, 0},
            {2.4f, -.4f, 0},
            {2.1f, .6f, 0},
            {1.4f, .8f, 0},
            {.9f, .2f, 0}
        });
    } else if (kind == 4) {
        path.path = [center](double u) {
            const double angle = u * 6.283185307179586;
            return center + glm::vec3(
                float(.7 * std::sin(2 * angle)),
                float(.6 * std::sin(angle)),
                float(.3 * std::cos(angle)));
        };
        path.interpolation = graphics::PathSystem::smoothStep;
    } else if (kind == 5) {
        path.path = graphics::PathSystem::circle(center, .65f, {0, 0, 1});
    } else if (kind == 6) {
        path.path = graphics::PathSystem::polyline({
            {1, -.5f, 0},
            {2.4f, -.5f, 0},
            {2, .6f, 0}
        });
        path.playback = graphics::Playback::PingPong;
    } else {
        return false;
    }

    graphics::Motion motion;
    auto& motions = app_.getScene().getMotions();
    if (!pathId_ || !motion.append(graphics::Motion::followPath(path))) {
        return false;
    }
    if (!motions.setMotion(*pathId_, motion) || !motions.seekMotion(*pathId_, 0)) {
        return false;
    }
    std::cout << "경로 " << kind << ": 시작 시각 0초로 변경\n";
    return true;
}

bool Console::initScene() {
    auto& scene = app_.getScene();
    scene.clear();
    paused_ = false;
    spinEnabled_ = true;
    globalEnabled_ = false;
    reflection_ = 1;
    insertedLayer_.reset();

    const auto first = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
    const auto second = scene.addCuboid({.4f, .6f, .3f}, {0, 0, 0});
    if (!first || !second) {
        return false;
    }
    if (!scene.setTint(*first, {1, .35f, .2f})
        || !scene.setTint(*second, {.2f, .7f, 1})) {
        return false;
    }

    auto& motions = scene.getMotions();
    pathId_ = motions.createMotion(graphics::Motion{});
    graphics::RotationMotion spin;
    spin.degreesPerSecond = 90;
    spinId_ = motions.createSpin(spin);
    graphics::RotationMotion global;
    global.degreesPerSecond = 15;
    globalId_ = motions.createOrbit(global);
    if (!pathId_ || !spinId_ || !globalId_ || !choosePath(1)) {
        return false;
    }
    if (!motions.setMotionEnabled(*globalId_, false)
        || !motions.bindMotion(graphics::MotionSystem::rootLayer(), *globalId_)) {
        return false;
    }

    const auto normalPath = motions.pushLayer(graphics::MotionSystem::rootLayer());
    const auto reflection = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
    if (!normalPath || !reflection) {
        return false;
    }
    graphics::LayerConfig mirror;
    mirror.transform = *reflection;
    mirrorLayer_ = motions.pushLayer(graphics::MotionSystem::rootLayer(), mirror);
    if (!mirrorLayer_) {
        return false;
    }
    const auto mirroredPath = motions.pushLayer(*mirrorLayer_);
    normalSpinLayer_ = motions.pushLayer(*normalPath);
    if (!mirroredPath || !normalSpinLayer_) {
        return false;
    }
    const auto mirroredSpin = motions.pushLayer(*mirroredPath);
    if (!mirroredSpin
        || !motions.bindMotion(*normalPath, *pathId_)
        || !motions.bindMotion(*mirroredPath, *pathId_)
        || !motions.bindMotion(*normalSpinLayer_, *spinId_)
        || !motions.bindMotion(*mirroredSpin, *spinId_)
        || !scene.bindEntity(*first, *normalSpinLayer_)
        || !scene.bindEntity(*second, *mirroredSpin)) {
        return false;
    }
    if (!scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *first)
        || !scene.addBasis({0, 0, 0}, glm::mat3(1), {.55f, .55f, .55f}, *second)
        || !scene.addLine({0, -1.4f, 0}, {0, 1.4f, 0}, {.5f, .5f, .5f})) {
        return false;
    }

    auto& camera = app_.getCamera();
    camera.position = {5, 3, 8};
    camera.target = {0, 0, 0};
    camera.up = {0, 1, 0};
    printHelp();
    return true;
}

bool Console::changeReflection() {
    const int next = (reflection_ + 1) % 4;
    std::optional<glm::mat4> matrix = glm::mat4(1);
    if (next == 1) {
        matrix = graphics::TransformSystem::reflectPlane({0, 0, 0}, {1, 0, 0});
    } else if (next == 2) {
        matrix = graphics::TransformSystem::reflectPoint({0, 0, 0});
    } else if (next == 3) {
        matrix = graphics::TransformSystem::reflectLine(graphics::AxisLine{});
    }
    if (!mirrorLayer_ || !matrix) {
        return false;
    }
    graphics::LayerConfig config;
    config.transform = *matrix;
    if (!app_.getScene().getMotions().setLayer(*mirrorLayer_, config)) {
        return false;
    }
    reflection_ = next;
    const char* names[] = {"없음", "YZ평면 반사", "원점 점대칭", "Y축 직선대칭"};
    std::cout << "반사: " << names[reflection_] << '\n';
    return true;
}

bool Console::toggleInsertedLayer() {
    auto& motions = app_.getScene().getMotions();
    if (insertedLayer_) {
        if (!motions.removeLayer(*insertedLayer_)) {
            return false;
        }
        insertedLayer_.reset();
        std::cout << "추가 크기 레이어 삭제\n";
        return true;
    }
    const auto scale = graphics::TransformSystem::scaling(glm::vec3(1.5f));
    if (!normalSpinLayer_ || !scale) {
        return false;
    }
    graphics::LayerConfig config;
    config.space = graphics::TransformSpace::Local;
    config.transform = *scale;
    insertedLayer_ = motions.insertLayer(*normalSpinLayer_, config);
    if (!insertedLayer_) {
        return false;
    }
    std::cout << "첫 객체에 1.5배 Local 크기 레이어 삽입\n";
    return true;
}

void Console::onKey(int key, int action) {
    if (action != GLFW_PRESS) {
        return;
    }
    auto& motions = app_.getScene().getMotions();
    bool ok = true;
    if (key == GLFW_KEY_ESCAPE) {
        app_.requestClose();
    } else if (key >= GLFW_KEY_1 && key <= GLFW_KEY_6) {
        ok = choosePath(key - GLFW_KEY_1 + 1);
    } else if (key == GLFW_KEY_SPACE) {
        const bool playing = paused_;
        ok = pathId_ && spinId_ && globalId_
            && motions.setMotionPlaying(*pathId_, playing)
            && motions.setMotionPlaying(*spinId_, playing)
            && motions.setMotionPlaying(*globalId_, playing);
        if (ok) {
            paused_ = !paused_;
            std::cout << (paused_ ? "시간 정지\n" : "시간 재개\n");
        }
    } else if (key == GLFW_KEY_S) {
        ok = spinId_ && motions.setMotionEnabled(*spinId_, !spinEnabled_);
        if (ok) {
            spinEnabled_ = !spinEnabled_;
            std::cout << "자전 기여: " << spinEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_G) {
        ok = globalId_ && motions.setMotionEnabled(*globalId_, !globalEnabled_);
        if (ok) {
            globalEnabled_ = !globalEnabled_;
            std::cout << "전체 공전 기여: " << globalEnabled_ << '\n';
        }
    } else if (key == GLFW_KEY_M) {
        ok = changeReflection();
    } else if (key == GLFW_KEY_I) {
        ok = toggleInsertedLayer();
    } else if (key == GLFW_KEY_H) {
        printHelp();
    }
    if (!ok) {
        std::cerr << "샘플 명령 적용 실패. 장면을 종료합니다.\n";
        app_.requestClose();
    }
}

void Console::onMouse(const graphics::MouseEvent& event) {
    if (event.button != GLFW_MOUSE_BUTTON_LEFT || event.action != GLFW_PRESS) {
        return;
    }
    const auto hit = app_.pickAt(event.windowX, event.windowY);
    if (hit) {
        std::cout << "entity=" << hit->entityId << " face=" << hit->faceIndex << '\n';
    }
}

void Console::printHelp() const {
    std::cout << "standard: 프레임워크 Motion·반사·레이어 샘플\n"
              << "1 타원 / 2 사각형 / 3 다각형 / 4 사용자 3D 경로+보간 / 5 원 / 6 왕복 꺾은선\n"
              << "Space 시간 정지·재개 / S 자전 기여 / G 전체 공전 기여\n"
              << "M 반사 변경 / I 크기 레이어 삽입·삭제 / H 도움말 / Esc 종료\n"
              << "초기 반사: YZ평면. M은 점대칭 -> Y축 직선대칭 -> 없음 -> YZ평면 순환\n"
              << "왼쪽 클릭: 물체의 entity/face ID 출력\n";
}
```

## standard/standard.slnx

독립 standard 프로젝트 하나와 x64 플랫폼을 등록한다.

before: 없음

after:

```xml
<Solution>
  <Configurations>
    <Platform Name="x64" />
  </Configurations>
  <Project Path="standard.vcxproj" Id="ad50dc3f-7731-4954-923f-907bb2a8277c" />
</Solution>
```

## standard/standard.vcxproj

main.cpp와 Console.cpp만 컴파일한다. C++17/v145/SDK 10.0, Debug/Release x64, 동적 CRT를 지정하고 폴더 내부 Framework.props를 가져온다.

before: 없음

after:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project DefaultTargets="Build" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup Label="ProjectConfigurations">
    <ProjectConfiguration Include="Debug|x64">
      <Configuration>Debug</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
    <ProjectConfiguration Include="Release|x64">
      <Configuration>Release</Configuration>
      <Platform>x64</Platform>
    </ProjectConfiguration>
  </ItemGroup>
  <PropertyGroup Label="Globals">
    <ProjectGuid>{AD50DC3F-7731-4954-923F-907BB2A8277C}</ProjectGuid>
    <Keyword>Win32Proj</Keyword>
    <ProjectName>standard</ProjectName>
    <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.Default.props" />
  <PropertyGroup Label="Configuration">
    <ConfigurationType>Application</ConfigurationType>
    <PlatformToolset>v145</PlatformToolset>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries Condition="'$(Configuration)'=='Debug'">true</UseDebugLibraries>
    <UseDebugLibraries Condition="'$(Configuration)'=='Release'">false</UseDebugLibraries>
  </PropertyGroup>
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />
  <PropertyGroup>
    <OutDir>$(ProjectDir)x64\$(Configuration)\</OutDir>
    <IntDir>$(ProjectDir)x64\$(Configuration)\obj\</IntDir>
    <TargetName>standard</TargetName>
  </PropertyGroup>
  <ItemDefinitionGroup>
    <ClCompile>
      <LanguageStandard>stdcpp17</LanguageStandard>
      <WarningLevel>Level4</WarningLevel>
      <RuntimeLibrary Condition="'$(Configuration)'=='Debug'">MultiThreadedDebugDLL</RuntimeLibrary>
      <RuntimeLibrary Condition="'$(Configuration)'=='Release'">MultiThreadedDLL</RuntimeLibrary>
    </ClCompile>
    <Link>
      <SubSystem>Console</SubSystem>
      <GenerateDebugInformation>true</GenerateDebugInformation>
    </Link>
  </ItemDefinitionGroup>
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Transform.h" />
  </ItemGroup>
  <Import Project="framework\Framework.props" />
  <Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />
</Project>
```

## standard/standard.vcxproj.filters

새 소스·헤더의 프로젝트 표시 항목을 등록한다. 다른 main이나 절대경로를 추가하지 않는다.

before: 없음

after:

```xml
<?xml version="1.0" encoding="utf-8"?>
<Project xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <ClCompile Include="main.cpp" />
    <ClCompile Include="Console.cpp" />
    <ClInclude Include="Console.h" />
    <ClInclude Include="framework\Framework.h" />
    <ClInclude Include="framework\include\Motion.h" />
    <ClInclude Include="framework\include\Transform.h" />
  </ItemGroup>
</Project>
```

## framework-analysis/update_graph.py의 모든 변경 블록

분석 대상에서 standard/framework 등 배포 사본을 제외하고 standard/main.cpp에 등록한 Console 콜백 본문을 소스 위치로 연결한다. 이 변경은 분석용이며 프로그램 런타임을 바꾸지 않는다.

### 블록 1: before L68~84, after L68~86

before:

```python
            extraction['nodes'].append(node);anchors[(rel,symbol)]=node
            extraction['edges'].append({'source':file_id,'target':ident,'relation':'defines',
                'confidence':'EXTRACTED','source_file':rel,'source_location':f'L{line}'})
    for number in range(13,20):
        rel=f'openGL_3dAssign_{number}/main.cpp'
        if (rel,'main') not in anchors:continue
        text=(root/rel).read_text(encoding='utf-8-sig')
        for match in re.finditer(r'practice\.(initScene|onKey|onUpdate|onFocus)\s*\(',text):
            target=anchors.get((f'openGL_3dAssign_{number}/Console.cpp','Console::'+match.group(1)))
            if not target:continue
            extraction['edges'].append({'source':anchors[(rel,'main')]['id'],'target':target['id'],
                'relation':'registered_callback_body_calls','confidence':'EXTRACTED','source_file':rel,
                'source_location':'L'+str(text[:match.start()].count('\n')+1),
                'context':f'practice {number}: lambda executes later through Application callbacks'})
    return len(anchors)

def main():
```

after:

```python
            extraction['nodes'].append(node);anchors[(rel,symbol)]=node
            extraction['edges'].append({'source':file_id,'target':ident,'relation':'defines',
                'confidence':'EXTRACTED','source_file':rel,'source_location':f'L{line}'})
    callback_projects=[(f'openGL_3dAssign_{number}','practice') for number in range(13,20)]
    callback_projects.append(('standard','sample'))
    for directory,receiver in callback_projects:
        rel=f'{directory}/main.cpp'
        if (rel,'main') not in anchors:continue
        text=(root/rel).read_text(encoding='utf-8-sig')
        for match in re.finditer(rf'{receiver}\.(initScene|onKey|onUpdate|onFocus|onMouse)\s*\(',text):
            target=anchors.get((f'{directory}/Console.cpp','Console::'+match.group(1)))
            if not target:continue
            extraction['edges'].append({'source':anchors[(rel,'main')]['id'],'target':target['id'],
                'relation':'registered_callback_body_calls','confidence':'EXTRACTED','source_file':rel,
                'source_location':'L'+str(text[:match.start()].count('\n')+1),
                'context':f'{directory}: lambda executes later through Application callbacks'})
    return len(anchors)

def main():
```

### 블록 2: before L93~99, after L95~101

before:

```python
        relative=p.relative_to(root)
        if not p.is_file() or p.suffix.lower() not in {'.cpp','.h','.hpp','.inl'}:continue
        if any(part in excluded for part in relative.parts):continue
        if not framework and len(relative.parts)>1 and relative.parts[0].startswith('openGL') and 'framework' in relative.parts:continue
        files.append(p)
    files.sort()
    detection=detect(root,extra_excludes=['**/dependencies/**','**/packages/**','**/x64/**','**/archive/**','**/.vs/**'],cache_root=cache)
```

after:

```python
        relative=p.relative_to(root)
        if not p.is_file() or p.suffix.lower() not in {'.cpp','.h','.hpp','.inl'}:continue
        if any(part in excluded for part in relative.parts):continue
        if not framework and 'framework' in relative.parts and relative.parts[0]!='framework':continue
        files.append(p)
    files.sort()
    detection=detect(root,extra_excludes=['**/dependencies/**','**/packages/**','**/x64/**','**/archive/**','**/.vs/**'],cache_root=cache)
```

## 동봉 프레임워크: 복사 범위와 원본 대응

공통 코드를 다시 구현하지 않고 작업 시작 시의 루트 framework 전체를 standard/framework로 복사했다. 아래 파일들은 새로운 소스 로직을 작성한 것이 아니라 배포 사본이다. 각 원본은 저장소 루트의 framework/<상대경로>이고, after는 standard/framework/<같은 상대경로>다. 사본 파일은 before 없음이며 원본과 after의 바이트 일치를 표에 기록한다. 제3자 SDK 소스와 라이선스를 재작성하거나 인코딩 변환하지 않았다.

직접 작성한 응용 코드·프로젝트 설정은 위에서 전체 before/after를 제공했다. 공통 코드의 API·계산은 동봉한 framework/README.md와 framework/docs/APPLICATION_PATTERNS.md에 있고, 공통 코드 자체의 앞선 변경 전후는 저장소의 framework-analysis/MOTION_BEFORE_AFTER.md에 있다.

| 사본 상대경로 (standard/framework 기준) | 바이트 수 | SHA-256 | 루트 원본과 일치 |
|---|---:|---|---|
| `dependencies/assimp/bin/assimp-vc145-mt.dll` | 8010240 | 77d2ece8ab30adcd2fdca061e0abd3f8816e9bbde63eafa1d270170e798bc6cd | True |
| `dependencies/assimp/include/assimp/.editorconfig` | 186 | 2969524d7267bc878aea2262d09b86a38f8827d976973b806e5953e34fc6d12a | True |
| `dependencies/assimp/include/assimp/aabb.h` | 2634 | d18eb17f81ecfc4f73836c094029c9aaf21d15439485ef3582af17187989c91e | True |
| `dependencies/assimp/include/assimp/ai_assert.h` | 2798 | 90d9c7d7c225bf5f0154bee032b32e2723bcd95a840ccbbdaf7f9e2febf1da9b | True |
| `dependencies/assimp/include/assimp/anim.h` | 19005 | 8a623cb7eb7739f83ba1fd1295eaeda7833027ec3ab9aafa79f3594fb6eb82b0 | True |
| `dependencies/assimp/include/assimp/AssertHandler.h` | 3509 | e5c09bf64c5ea05eef6fa2d27f90ff8d1df0092f3f17b097d093eec11fa20460 | True |
| `dependencies/assimp/include/assimp/Base64.hpp` | 3748 | d162620cdd69bca7c60626416c2ceaa2855b47749aad8128df01c21e5581fdee | True |
| `dependencies/assimp/include/assimp/BaseImporter.h` | 17297 | 614bea1de5f63697c09caabf97ecb79d60c6ec16f039a196d53a9eddbcb66335 | True |
| `dependencies/assimp/include/assimp/Bitmap.h` | 4394 | a183fb24114eaef15ff433b7fe1e4638115ec0f995f5c716f360c3b529ca8dc7 | True |
| `dependencies/assimp/include/assimp/BlobIOSystem.h` | 10312 | f1059157f2242337a7ea2ebccbff303e92cccceea468e83fa19be0082931380d | True |
| `dependencies/assimp/include/assimp/ByteSwapper.h` | 9240 | bda73c692caeb7264fc25adbc6a041b49137d27e453c52849a1f6aa1b4093ed8 | True |
| `dependencies/assimp/include/assimp/camera.h` | 8346 | 452cf1d994d07550897745d513a028b9a902f612401e1702027041328854d45d | True |
| `dependencies/assimp/include/assimp/cexport.h` | 13114 | f2de14e5236eafa520be87f476d3dba12136f4dc6e58aa840cdf7723ab5e2454 | True |
| `dependencies/assimp/include/assimp/cfileio.h` | 4786 | 273f616cc22949d25e315c4aa50fe3be9e067aa7268a2419812ee992f2a285d7 | True |
| `dependencies/assimp/include/assimp/cimport.h` | 51045 | 0c6b745aa0ab70846b358453d457802124df025b0b640401401daf85c8c4a1d7 | True |
| `dependencies/assimp/include/assimp/ColladaMetaData.h` | 2097 | 30f40a85b592dbbae9abc1135a2771b46b653e87186eb2e5d2e4bcc87dc51a5d | True |
| `dependencies/assimp/include/assimp/color4.h` | 3621 | 589dcb55179a451326472a969a3f24392030e174c8a23b1e49c56f19802f2e95 | True |
| `dependencies/assimp/include/assimp/color4.inl` | 9267 | caf43dc3507f39ffb295a7ff813a39c52c78e11cb460ec73dbb8988cdb8d70bd | True |
| `dependencies/assimp/include/assimp/commonMetaData.h` | 2889 | c1160677914999239b780bf3ebd69a5501251f460f320cc4b22127c40d4921a7 | True |
| `dependencies/assimp/include/assimp/Compiler/poppack1.h` | 818 | e3e43209d1c7e1b107dd00c382446209334d4a575ca530a00505cdda1944a02c | True |
| `dependencies/assimp/include/assimp/Compiler/pstdint.h` | 31966 | a29e0cdf4d0dcedaece2628a06aba1df533c840450816bb23e5e41c1a0ff98e7 | True |
| `dependencies/assimp/include/assimp/Compiler/pushpack1.h` | 1257 | 04fa75107b449d38e652deb927f63280338529715ee688c49adddd13a42a3885 | True |
| `dependencies/assimp/include/assimp/config.h` | 51268 | 739a792acfa8cc5786f88222febb311b104c315fbfc051b38024b0a6cf124e22 | True |
| `dependencies/assimp/include/assimp/config.h.in` | 51270 | 0449b9123ae52c7893992270089bbbd8a3603bdbc6e32816e6266719d9813f0f | True |
| `dependencies/assimp/include/assimp/CreateAnimMesh.h` | 3017 | efb7aee34d2974c57df06186c88bb9150b1146b5dea5da6d7f4ec9186758857f | True |
| `dependencies/assimp/include/assimp/DefaultIOStream.h` | 4978 | c14cb61a37b921e8a307531dec3fe164354484ea926d9624d79c687c140fa1a5 | True |
| `dependencies/assimp/include/assimp/DefaultIOSystem.h` | 3822 | edee4bd350839d2de25bc6b284d577bb50902e23289d3b46efedfe298dcf71e8 | True |
| `dependencies/assimp/include/assimp/DefaultLogger.hpp` | 8560 | 63c430e0f6983f6bd3fb49d1fcfb4b72dcd9cf3592eefa1a52e947ff05fbf354 | True |
| `dependencies/assimp/include/assimp/defs.h` | 11937 | 31a6b171eca8f840be5b664b8629246b1cc5b82e14fe403d2a1eed550933ae35 | True |
| `dependencies/assimp/include/assimp/Exceptional.h` | 7590 | 9f26c6eb3679504be30fd417372744083f590e40a61fe92550e204dbe77a75ca | True |
| `dependencies/assimp/include/assimp/Exporter.hpp` | 23514 | bf5c7aea72bf5ab56bb75e9dccf40b8a9328132c1c4c9d3b4abd3b33ddb47666 | True |
| `dependencies/assimp/include/assimp/fast_atof.h` | 12672 | 2f5e9cf84c8d4b945ce24ad1ec265357ba1f1901c46ce3917218f39f81a6727c | True |
| `dependencies/assimp/include/assimp/GenericProperty.h` | 4580 | 8549425caa47602f6aa80be7385a22a12450c0bdef5af8d45ac0030149cc1441 | True |
| `dependencies/assimp/include/assimp/GltfMaterial.h` | 3280 | bdd5682ec54580de92dd433344746c51a4310628dd607ba505b8afe5bc9f4cf6 | True |
| `dependencies/assimp/include/assimp/Hash.h` | 4316 | 0905017021b24ac12c5d89435310b2caf9ef558ddbe7315506e66bf6c392f695 | True |
| `dependencies/assimp/include/assimp/Importer.hpp` | 32159 | 221d38c793c1c96d29ac8f03757898ebf8e0c26b6381504b6b013d422ed0b916 | True |
| `dependencies/assimp/include/assimp/importerdesc.h` | 6237 | c975774820bb5a887b216a694ee0a6f1dd0248b581ccb43d32aa85e492d673c7 | True |
| `dependencies/assimp/include/assimp/IOStream.hpp` | 4752 | 1fe3415c0803156f6659560bad631be6155170ecc791d0933bf92183691d0db7 | True |
| `dependencies/assimp/include/assimp/IOStreamBuffer.h` | 9483 | bc3fa92fd27b87b76648c449c6bc41a1af379d5c57beaef64415ac447d50d344 | True |
| `dependencies/assimp/include/assimp/IOSystem.hpp` | 12334 | dfa5b258e665f69d28cb04da0083c66d2de378de04378f90f79ef407b6c7effd | True |
| `dependencies/assimp/include/assimp/light.h` | 9332 | 8c8c239c559a8f5db20c2f4e34a6cb30e0eff271a19f91b01c2635a880c30c80 | True |
| `dependencies/assimp/include/assimp/LineSplitter.h` | 9333 | 39d95aa36dbc701f7ef557fefee3a5ffd380ae875086881eb35a99f835307ea8 | True |
| `dependencies/assimp/include/assimp/LogAux.h` | 4177 | d0519d72ad40b2635997d4ee4e45cce8c20450ad1fd00c27ef3a4b6ee4b22a5c | True |
| `dependencies/assimp/include/assimp/Logger.hpp` | 11407 | de3e53bd13d386db51e3897c1a8932f5eb639ca6635a6cf8b464ea1ea57b5eed | True |
| `dependencies/assimp/include/assimp/LogStream.hpp` | 4019 | 549c4a6a7a9cec664f518a093a3a96b625899d89bb9f1ee4c77e9406bb72976a | True |
| `dependencies/assimp/include/assimp/material.h` | 64293 | 8e7eea5375d2deda154aaff5fb52a16bb04c3d21f324b4f9759d968c58b91616 | True |
| `dependencies/assimp/include/assimp/material.inl` | 15569 | 066e60d984746f47bdf5ad62ac2737757f6998daa1e99af351b510c3b1adbf15 | True |
| `dependencies/assimp/include/assimp/MathFunctions.h` | 3314 | 703e21b6249980314ce56a179954242104d999bbb5aea8156c0b96ddd25b5430 | True |
| `dependencies/assimp/include/assimp/matrix3x3.h` | 6660 | 7eb4e59c01e559ef71528b9d6bb14e60f8234d6d86c768f4b1731d76b178140c | True |
| `dependencies/assimp/include/assimp/matrix3x3.inl` | 13272 | e78a0bb0b30e387432d4f95338b418d53ae23cb1d47e578c423ca8f412bce6d3 | True |
| `dependencies/assimp/include/assimp/matrix4x4.h` | 12007 | ba23ba65abc16d56a2f904dccc4b66fa4e36cd713dcd20e943720c9e27334100 | True |
| `dependencies/assimp/include/assimp/matrix4x4.inl` | 24995 | 6de5e30c3ee26fe211c474ac009169754fa67c742c2df207d9892dbeffa4acfb | True |
| `dependencies/assimp/include/assimp/MemoryIOWrapper.h` | 8167 | b934edbb900dcdad5dcc0bf9b76b587be9979d0df1bbffc9697862a902219f71 | True |
| `dependencies/assimp/include/assimp/mesh.h` | 40112 | 9c9ef751fc1b3f9955ea2b5c28f5429e18644f180f53c08bbe90b257b027b5b3 | True |
| `dependencies/assimp/include/assimp/metadata.h` | 18582 | 62e0bbe315095f4aa6d63ba2da070a111ca98438d4ba19699f68ad9256c1d9a1 | True |
| `dependencies/assimp/include/assimp/module.modulemap` | 800 | 715453dbf84509c1db9f9993be1595d9028cf922f66d7ba0c5d49477c0213d56 | True |
| `dependencies/assimp/include/assimp/NullLogger.hpp` | 3618 | 61686bce180c1b570972b3fa675f0caa3cf9162ee8542cade8ad039a90087982 | True |
| `dependencies/assimp/include/assimp/ObjMaterial.h` | 3032 | 362953e72da21f88bb57c7e8c776c5d97c683b6b0b25c08755da856eaa4bd270 | True |
| `dependencies/assimp/include/assimp/ParsingUtils.h` | 13018 | b53c9db286270493ec8d86006de9e05a0660e0ffc8063192ca937bfb4eedcc81 | True |
| `dependencies/assimp/include/assimp/pbrmaterial.h` | 4225 | ad4cc8dd89db456a8d779da7cefd8d256c8e1b845e4b69d23cdaf476bcb4304d | True |
| `dependencies/assimp/include/assimp/port/AndroidJNI/AndroidJNIIOSystem.h` | 3387 | 42c0a850361e0cd82e08ac59a980012899da304a5a2ab3092dc20a9d22f2b53f | True |
| `dependencies/assimp/include/assimp/port/AndroidJNI/BundledAssetIOSystem.h` | 3615 | 89171a36a45e267aae25b852b827768e27e1ebf55e9fcc507ef1c37f5970393d | True |
| `dependencies/assimp/include/assimp/postprocess.h` | 34418 | 5d9ed1ecce8b6bab8cc2f0a076475591339459e91df0461f7f4d282667bad952 | True |
| `dependencies/assimp/include/assimp/Profiler.h` | 3490 | fa64482b79d971aa8e8835cfcb0a21c97ad9bf938a6fe45d80e0d9d874df074b | True |
| `dependencies/assimp/include/assimp/ProgressHandler.hpp` | 5971 | 74ad71fb3365b1e7be21b5d2ffb0ad596dc1254c62bcb7eb85180e29ea6803e4 | True |
| `dependencies/assimp/include/assimp/qnan.h` | 6029 | d1ed7fd4ee2dc53086e88f63c21f03f5f3a78a461e9ed344449b8401ffce9225 | True |
| `dependencies/assimp/include/assimp/quaternion.h` | 4931 | 4dbd1539a67ff550f87f330fdebed6a41a2efd3ef35915c5a6419ca0051c2e15 | True |
| `dependencies/assimp/include/assimp/quaternion.inl` | 12204 | 5a7288721a82b211203409b2b343fc6c66b60f5e6e38923cd8ca61a1aaff860e | True |
| `dependencies/assimp/include/assimp/RemoveComments.h` | 3720 | 1561088d0064caab3d07531f1cfdc21183d2b7ca758c894ea7393b1d3e8659f3 | True |
| `dependencies/assimp/include/assimp/revision.h` | 853 | 1fac11a76f24aedb8ab3e43131db4c7e67fcef5cd21e0ddbdbbee65b7edc5d17 | True |
| `dependencies/assimp/include/assimp/revision.h.in` | 1076 | 484615dda3ddd8d7d6354dc78da1fecbaefece82aef4075105f06da546dcb403 | True |
| `dependencies/assimp/include/assimp/scene.h` | 18025 | 2d1e684b201dd216b9b5cbfae5bfdead4f0f0bd9128d82889cbede4c8d339429 | True |
| `dependencies/assimp/include/assimp/SceneCombiner.h` | 14994 | 4f025f1fdf7ed1f01e46adea4b5e167fb3d7abb6946b1c0460e8a7a41176d6d1 | True |
| `dependencies/assimp/include/assimp/SGSpatialSort.h` | 5998 | 9e8d6d53598aed43fd79057fa85c76330f38ed44d1faff2e19842d27a38f385a | True |
| `dependencies/assimp/include/assimp/SkeletonMeshBuilder.h` | 4480 | e4a4b49d2854f9df790e94978360735ffc9fb5241906e38ed72fe9268742295c | True |
| `dependencies/assimp/include/assimp/SmallVector.h` | 5182 | 5e23bfce09791da6d3dcd72ad157888c765587b6b2c9268e44d389edbcbb7883 | True |
| `dependencies/assimp/include/assimp/SmoothingGroups.h` | 3978 | 603d6f7ac6130f89ca340b4289248d77962911a172f545bce73fe5a96e33d031 | True |
| `dependencies/assimp/include/assimp/SmoothingGroups.inl` | 5386 | b2a5294d07ae1ebc4e064f4d862d074a4413e18e3de4e19127d20412ee49afd0 | True |
| `dependencies/assimp/include/assimp/SpatialSort.h` | 8962 | b55157d66e82d903467188266c3c88c8166df0b5b56ccd5443a5c3947a97c071 | True |
| `dependencies/assimp/include/assimp/StandardShapes.h` | 7042 | cdd1dce70ebe5bcb00aa17d4b66bd8ba221cae7877616a72e6842877a395457d | True |
| `dependencies/assimp/include/assimp/StreamReader.h` | 13575 | 26482b73cc18424b2a1f6ca5dc265b6040da4cfb2be5c054fa9318964049ec69 | True |
| `dependencies/assimp/include/assimp/StreamWriter.h` | 10494 | 6a48cd1348bf00172acb7348b2b789aa6c86f9bf6e706e92940185e8a7a6bff1 | True |
| `dependencies/assimp/include/assimp/StringComparison.h` | 7078 | 8fdbd691334c8aa9b1b2a0c11b9c9bc75377c36b94435fa711e17d53e287c90f | True |
| `dependencies/assimp/include/assimp/StringUtils.h` | 11326 | 7115c321e893e477cba6a41069c1ec6f6a488cf226e17a09f459028727430080 | True |
| `dependencies/assimp/include/assimp/Subdivision.h` | 5013 | 7eeaeaf437783f767a76f94769c7610bbd532cc6e9740b761e0f3c28b618cff6 | True |
| `dependencies/assimp/include/assimp/texture.h` | 8251 | 5cdf80bbe825352065c22227dfd1018139907d4b65c6f5eb5cb8db7b663bcc85 | True |
| `dependencies/assimp/include/assimp/TinyFormatter.h` | 6426 | 107d800fdacd3bd9116755a6c7363f34c82829d80758a243027f18543e26fc74 | True |
| `dependencies/assimp/include/assimp/types.h` | 18101 | f771e0da6ac18baa85db336a6de4e16ef13b9e9b7854b70636be3d427839a603 | True |
| `dependencies/assimp/include/assimp/vector2.h` | 3503 | f507076f5165340f3f110ab15d59afb94d558ce59551a5d2a6fde71b442e985d | True |
| `dependencies/assimp/include/assimp/vector2.inl` | 8307 | 7c53be6dfae0e0585933a95048feb308dad9040be5c376048abd533b111ec8d4 | True |
| `dependencies/assimp/include/assimp/vector3.h` | 5658 | 0218aba1c6262a9a59069d15bddd76700f8098c25e239d9cb252def793f30d8c | True |
| `dependencies/assimp/include/assimp/vector3.inl` | 12054 | cb9c177dd1fc3a5db1b3e25ac35b7b5f9c515d42a0f7abe6cd4f2864b9325b78 | True |
| `dependencies/assimp/include/assimp/version.h` | 4798 | e9c884e2aa65006323d26d881b2e9047eae6c96e903d695818c0dba52c19ae7c | True |
| `dependencies/assimp/include/assimp/Vertex.h` | 11562 | bd06281a1049630f78e149066117ab595d093d9628c3c285e6f0a8dfeb7c60a3 | True |
| `dependencies/assimp/include/assimp/XmlParser.h` | 20435 | 116e4b60b31664fa2403299478ecea3c4d2883edbff4e340294e187806f8041d | True |
| `dependencies/assimp/include/assimp/XMLTools.h` | 3062 | c66184a0e58bcbb8d199cb879cef92f1683d076095482271aa0e4f0362f0ee9b | True |
| `dependencies/assimp/include/assimp/ZipArchiveIOSystem.h` | 4055 | c8e974c1f352dc09fa337348cac721404b50ce0f6562a9740bb25107bbd491c9 | True |
| `dependencies/assimp/LICENSE.txt` | 3872 | 8dcb9e02284dc239cfa334d28a6aef2b257a8f73ace9ef2db9315fcb862a8bfe | True |
| `dependencies/glew-2.3.1/bin/Release/x64/glew32.dll` | 472576 | a4428b3283ded13b701797d433343eb923d29efe198d27d89e1f3f64f6516427 | True |
| `dependencies/glew-2.3.1/include/GL/eglew.h` | 135514 | feb986fe31fb9a48fef1b39f6042be2602daedb97a6f8b9125bc3596c08e41db | True |
| `dependencies/glew-2.3.1/include/GL/glew.h` | 1362237 | 7e8851309fb6ff9a01ec6317ef38d0534d29eaae80ee2e9cbe1050c8f6a5c825 | True |
| `dependencies/glew-2.3.1/include/GL/glxew.h` | 77600 | 313e9191173fe3bc2dfc979c35668d542d44a3185e3119414ca65093e2ab3779 | True |
| `dependencies/glew-2.3.1/include/GL/wglew.h` | 65578 | 0ca21745247a7903204959b1d96f944c3873cac9c6d1d03ed74d9273d312fda2 | True |
| `dependencies/glew-2.3.1/lib/Release/x64/glew32.lib` | 770808 | b8c3e33e1c08beba9c9c60e7a1c795bdb1cd3982db9eb6332f98ed972dcc5f65 | True |
| `dependencies/glew-2.3.1/LICENSE.txt` | 3873 | 38ee953cbae3c9d8b26308bdf92a1d0d6b79629165b58946e92df2f88001f16f | True |
| `dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3.h` | 248376 | 182f851c480be6f79b27b064171e7f43572d3fe195c364dc2932277f98da9d04 | True |
| `dependencies/glfw-3.4.bin.WIN64/include/GLFW/glfw3native.h` | 21867 | b66cb1327a98a5a8ecede8eefe48eca3b702b09c7fc0bec04ad0bf86c7bd81ee | True |
| `dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3.dll` | 232448 | 4429adff46d4d1038bb5836cecc59661eef7126e4941de6fdfde8855e7c14bb1 | True |
| `dependencies/glfw-3.4.bin.WIN64/lib-vc2022/glfw3dll.lib` | 31378 | 3b1e0f2e4e9f443838a0917b7c58aedc4af2c25823a811254a75c5b1ad62e0d3 | True |
| `dependencies/glfw-3.4.bin.WIN64/LICENSE.md` | 930 | 6d091ca4f2cb3e9c53d83112ebb63774ecd40c64b468003b348bf0432386f5a6 | True |
| `dependencies/glm/copying.txt` | 2922 | 142911fd27af238e59643eacefa39d8412c3f9d3bf2026c2c1e2a0bd00d4b4d2 | True |
| `dependencies/glm/glm/CMakeLists.txt` | 2537 | 0c45721afef81c3526612a8ecfe045eef3da48d09ae15ac61f38ba02233ed219 | True |
| `dependencies/glm/glm/common.hpp` | 28188 | 48d0b170fc8d4bfe81c5aea565e874613f989defee4f3d69e8c253a8f36ee737 | True |
| `dependencies/glm/glm/detail/_features.hpp` | 11773 | 50f94f2f93f598cc719388a69d9532b0986a1e47cadbcfb3b424ad5f6275d304 | True |
| `dependencies/glm/glm/detail/_fixes.hpp` | 410 | 56f2af1c102984685206613ebb40335dbce3c14e645de4295ecc8d6043dd1786 | True |
| `dependencies/glm/glm/detail/_noise.hpp` | 2363 | cec1a5818e6e57c8006c5f796239ff130b033f49a0eccacc24e016f472adc424 | True |
| `dependencies/glm/glm/detail/_swizzle.hpp` | 49268 | d898ea2c0c7497438abe4c8ad8ff1c9fa01035433b6a87ce7bf0efcac3d1b7fa | True |
| `dependencies/glm/glm/detail/_swizzle_func.hpp` | 35136 | fbc8edbbaac0ac999a759f2ce5fca903230b9535e8f331004982de9c683f45c9 | True |
| `dependencies/glm/glm/detail/_vectorize.hpp` | 8045 | d793730202775daaec85cdae89cb89c8f9c35954f8621fbb8bdaf8f1fe31701d | True |
| `dependencies/glm/glm/detail/compute_common.hpp` | 1303 | e19139d3505101a73dd8f1a0a6252a01bb0ce6f6afa85bfb3ef367ebb48e5efd | True |
| `dependencies/glm/glm/detail/compute_vector_decl.hpp` | 6279 | 53bc95c34ca4f33e022b2f9fca43e97ca8decceb7fea7d3cc6d9ce9b385036a5 | True |
| `dependencies/glm/glm/detail/compute_vector_relational.hpp` | 633 | 81cb9d8c12f083227a0ed9a94a4147a4b4b5f0c3be809b978571df4b7f86649c | True |
| `dependencies/glm/glm/detail/func_common.inl` | 30670 | 886b8f4cac597a6796bc65c50d7d43d474f850c397f7fbb84b4532314d740c54 | True |
| `dependencies/glm/glm/detail/func_common_simd.inl` | 16970 | 9d12159e26d03440b55cebcc0d6101eb30522d0e6df486b7e3788a5adb188cbe | True |
| `dependencies/glm/glm/detail/func_exponential.inl` | 4654 | d0050a125dc20f0aef0d721b0119ced6af24430e6ed08af5420f1c773eb18221 | True |
| `dependencies/glm/glm/detail/func_exponential_simd.inl` | 853 | 1bf6a6d54c8f7a244e3378ac4e46c598ef332811b7dd4b570b47332c6e89edc0 | True |
| `dependencies/glm/glm/detail/func_geometric.inl` | 8344 | c671e4733da6bb1ea22b6e76eea7f20a737357ba9f0b01a2ac2c567998b8652d | True |
| `dependencies/glm/glm/detail/func_geometric_simd.inl` | 4760 | 1991e7ef10f8a3711b4c85da87448eeb35db8f53b6c2daa1303394623285ee12 | True |
| `dependencies/glm/glm/detail/func_integer.inl` | 15754 | 5de79b2fbf67a6a32d51cf37510c34ef09e683279ecdd3c2d4b0fa6d7b89f574 | True |
| `dependencies/glm/glm/detail/func_integer_simd.inl` | 1756 | baefdb29125860e443e7a534b8d01bd4642dfa5a9875572d063d0df9fa3721ad | True |
| `dependencies/glm/glm/detail/func_matrix.inl` | 16247 | dec91438d926e37422690dc2261b3c19bda5d1b67dbbf9dbb9c3f3aa3bbbe6ad | True |
| `dependencies/glm/glm/detail/func_matrix_simd.inl` | 9048 | ea51aa2c30277a084c87673e6e24dfec7d9e96064a341e97431f45cc4d1647a0 | True |
| `dependencies/glm/glm/detail/func_packing.inl` | 3143 | c35a936c4cb68682c137df454b0d88163f3f3d012cea40c8c3639ae6fec1830a | True |
| `dependencies/glm/glm/detail/func_packing_simd.inl` | 81 | d59a7ff19ca90848710933ada5df8da312439977051b3ddc566aa9bd9903dc12 | True |
| `dependencies/glm/glm/detail/func_trigonometric.inl` | 5614 | 8dce04a74b72b88d312ee9f3025e0ccae235beb1bf5835d16fc7284cd2ee07f1 | True |
| `dependencies/glm/glm/detail/func_trigonometric_simd.inl` | 3 | f1945cd6c19e56b3c1c78943ef5ec18116907a4ca1efc40a57d48ab1db7adfc5 | True |
| `dependencies/glm/glm/detail/func_vector_relational.inl` | 2498 | d7e625f03f3949e06b34fb2075584b8e8b7dae0b4b345a19abd8bc2a27a95165 | True |
| `dependencies/glm/glm/detail/func_vector_relational_simd.inl` | 81 | d59a7ff19ca90848710933ada5df8da312439977051b3ddc566aa9bd9903dc12 | True |
| `dependencies/glm/glm/detail/glm.cpp` | 8725 | 66a3757ef93bcbbd125be1d5b9cc74b155483fc70679ced1cddb211aed1dd665 | True |
| `dependencies/glm/glm/detail/qualifier.hpp` | 7265 | b600bebe59852af7922430378c64741255c08fd723f89ce446bf0cec3fa62cc3 | True |
| `dependencies/glm/glm/detail/setup.hpp` | 43386 | 5cd3c1910234033904cf05e7c26f13bdc15e962d42104ad5af18563b2a9a6990 | True |
| `dependencies/glm/glm/detail/type_float.hpp` | 1645 | 9bc3296ba7ee8abc736bc1702cd235e63a0073088e282b09de1be8936a19940e | True |
| `dependencies/glm/glm/detail/type_half.hpp` | 273 | 3c9a5ee63e531d99b1d40d3a85282f96c43f21901565328b78156a714381028b | True |
| `dependencies/glm/glm/detail/type_half.inl` | 4760 | 341729a03107c010eab6f60da3a9aac7836854790d1c4547a865c56817ab6221 | True |
| `dependencies/glm/glm/detail/type_mat2x2.hpp` | 6769 | 937aa8268f8018992edfaf8c8119afbe7bd44ff2ab59e2a75e35c767af49bf87 | True |
| `dependencies/glm/glm/detail/type_mat2x2.inl` | 15097 | 07a6b9b13a9a4841f2aabaf348472a70491864d412210aa3b12fa3553560deb1 | True |
| `dependencies/glm/glm/detail/type_mat2x3.hpp` | 5779 | 6a80157ac17058876986ea8f555262f391caba30ffea29f23cf85705c234d9d9 | True |
| `dependencies/glm/glm/detail/type_mat2x3.inl` | 14074 | 628403a6c9604dface15711fea9692b0459430a7685ceedd38dfadd68ccc3725 | True |
| `dependencies/glm/glm/detail/type_mat2x4.hpp` | 5840 | 38f2a429bba596470183865a6c6f045d013ad5fd3dbedb6bec27dc8f96c2c890 | True |
| `dependencies/glm/glm/detail/type_mat2x4.inl` | 14675 | 071416302f5c0da02599217dfa618919fbce8b5323161ac7876261bfa87d337f | True |
| `dependencies/glm/glm/detail/type_mat3x2.hpp` | 5873 | d96770fa59e0d341a8811bdc4312d5594f13c3c95f96fd5dd9d93d6991418c51 | True |
| `dependencies/glm/glm/detail/type_mat3x2.inl` | 15124 | 245d79c7b00f355a3840f8b78e658717e2d56bd644fa5f9fd1806be8e63d4105 | True |
| `dependencies/glm/glm/detail/type_mat3x3.hpp` | 6939 | 01d5dc18cfa048cd25f40bcc78e4e32b082ce42b949d6d5b9985ee8fef1d8807 | True |
| `dependencies/glm/glm/detail/type_mat3x3.inl` | 19655 | cabeca31c8af7b4f25558204ea17be9ac3502a077d059f5a153df94365508615 | True |
| `dependencies/glm/glm/detail/type_mat3x4.hpp` | 6024 | 3799b956c1194ae3a37d10e22c4d37e5f7952e39fcb4697e6f1cddd55fc9ce84 | True |
| `dependencies/glm/glm/detail/type_mat3x4.inl` | 17194 | a0dcf42743e0382a2628124878aaaa7c8a981c3e1f3d0e0dae924ec4cfe574aa | True |
| `dependencies/glm/glm/detail/type_mat4x2.hpp` | 5870 | 077207ac339552caafcb335e64b9ed57bc8238088744c6d4de48218c0943946e | True |
| `dependencies/glm/glm/detail/type_mat4x2.inl` | 16973 | d2fd9318adebab558d2c65c7e188f1e2c3a40b605aced78c8cce7782dd04535f | True |
| `dependencies/glm/glm/detail/type_mat4x3.hpp` | 6170 | 5d7c18e8f5dc5a3e7ccca5d0e7f46ca46a6ceb29604a866f351ba9594312e1be | True |
| `dependencies/glm/glm/detail/type_mat4x3.inl` | 18421 | 77397c65708f55f0ad233f6070627a5565a01549c309a514dd76e2ec05df874a | True |
| `dependencies/glm/glm/detail/type_mat4x4.hpp` | 7438 | 642ac8fe4f5571080746fcded2800817dc5bd4314755a3596a1cbe5b3b62c4e2 | True |
| `dependencies/glm/glm/detail/type_mat4x4.inl` | 29247 | e92d519215e7cd4fb739f72973747e852ae81110b4649a271d8d08e169bc792b | True |
| `dependencies/glm/glm/detail/type_mat4x4_simd.inl` | 58 | bcc356a3c9fedb9bd177fc15d5b6ca0cd19a0571f5b0b568894ba8f51ed940a1 | True |
| `dependencies/glm/glm/detail/type_quat.hpp` | 6352 | 61083f3084e17197cb59e8793543d537fe1a30667dd9573803d6f36419429cdd | True |
| `dependencies/glm/glm/detail/type_quat.inl` | 12478 | 97c89e7d114283aeec9ed274421ac0ac14961ca91a8a0330aadc452db6994a00 | True |
| `dependencies/glm/glm/detail/type_quat_simd.inl` | 7206 | 4060eeb8bca977e059493c7b684d564965f9920a645bdce4e975d55ed699a74d | True |
| `dependencies/glm/glm/detail/type_vec1.hpp` | 11715 | 749ad682d740317de0cee106346f650d1317d770903839019d2192f31f6552f6 | True |
| `dependencies/glm/glm/detail/type_vec1.inl` | 14673 | 086c38597bea0f5ecf8b5f1feb7303bd076c4d8e8557a6b77abb2cce22f630a9 | True |
| `dependencies/glm/glm/detail/type_vec2.hpp` | 16328 | 343f6dbc68c89fbfa820fa1f6cbaaded2b917155c10f0bef1e221368fa33ea0d | True |
| `dependencies/glm/glm/detail/type_vec2.inl` | 24244 | 38234ca126a6b52baec79accb5bb6bae91ea5e7ef737ed8dd378cccea01b3f20 | True |
| `dependencies/glm/glm/detail/type_vec3.hpp` | 18335 | c87dede89a8dbb39688d580b8cfcbda18abe73e8b2e07119b2f4168c5d9827b2 | True |
| `dependencies/glm/glm/detail/type_vec3.inl` | 31947 | 0e1033167c913bf20c8254ef32fc26bf74becd7f7f343304a3b1d2df29b737e3 | True |
| `dependencies/glm/glm/detail/type_vec4.hpp` | 23112 | 6804a80268f0de2cfeb8b34c569d7b2a5c9d20afb9abcafe81bdb2670da165bd | True |
| `dependencies/glm/glm/detail/type_vec4.inl` | 37206 | 6f1b350f32078a5d08208a129b633fef24f3d784c68555dd71f3d38a4292bc4a | True |
| `dependencies/glm/glm/detail/type_vec_simd.inl` | 30706 | 269e9e8a5862599c08089c9adcb93d060dd23a0cb4502d157af447991957dec1 | True |
| `dependencies/glm/glm/exponential.hpp` | 5728 | f99fe4662b828fb40b0d82dbaeea5745f6d85ee7358af1f0fa00dde05de70d40 | True |
| `dependencies/glm/glm/ext/_matrix_vectorize.hpp` | 4682 | d47f45c0772ae134d7b125c3b8fbfb1f736e278774fc06542b72ee75ff62cf28 | True |
| `dependencies/glm/glm/ext/matrix_clip_space.hpp` | 34432 | 4e304202722baac68c98f76461b6eddc7cc908c5b494b8c6d043d49a4b822368 | True |
| `dependencies/glm/glm/ext/matrix_clip_space.inl` | 22715 | 7fa1c76868e9bebe00802ab29492d6c4d2f1977ef9ef7a491e8c9883d611b6e8 | True |
| `dependencies/glm/glm/ext/matrix_common.hpp` | 1171 | ff738dde368720033d72ec69c8eaac5edac4b117af39eba265762f6abe5e7bb0 | True |
| `dependencies/glm/glm/ext/matrix_common.inl` | 1185 | 83989be3cc04bd8bf58c8dffbf50059ff89c919a35a5d901d7b1136786fee04d | True |
| `dependencies/glm/glm/ext/matrix_double2x2.hpp` | 742 | 8988aae2320d0ef8f0241545eb13e7a5648f434f5361a7e0a9bb35965a41701e | True |
| `dependencies/glm/glm/ext/matrix_double2x2_precision.hpp` | 3022 | d30ec80b9397ae666807c3ddc63a624de95ec7bd6a3d638cba4abdc36cea3d9e | True |
| `dependencies/glm/glm/ext/matrix_double2x3.hpp` | 472 | 0249cdd96d25361f1f2e79de3f3d2a68e5e835b2fbbfcc7295fb0ebe3d94f618 | True |
| `dependencies/glm/glm/ext/matrix_double2x3_precision.hpp` | 1624 | 984659c78e14736235014216bf7a6f921e31cc4e674cebbcc42e8c024c63813c | True |
| `dependencies/glm/glm/ext/matrix_double2x4.hpp` | 472 | 399e591e95a51c0ff287a76270035fe0038ccc93b6288a43568391f9e8c35272 | True |
| `dependencies/glm/glm/ext/matrix_double2x4_precision.hpp` | 1624 | 734d6eb388aa0360209173b481af00af3e8cff8cc0db17bc5f4ff647baccacdb | True |
| `dependencies/glm/glm/ext/matrix_double3x2.hpp` | 472 | 3631794438a7042c6a849c0fa0b85ab0eb6dc36a2eaff23c31c30031d6f9c742 | True |
| `dependencies/glm/glm/ext/matrix_double3x2_precision.hpp` | 1624 | 18bff31b2d832d1f685f738446779f69cede287ec01375af434976586b3233c3 | True |
| `dependencies/glm/glm/ext/matrix_double3x3.hpp` | 742 | 524d18afb1f977ad4da6a1a3cf7d4a7a408878e04a5c072740804bfb1aa1af0c | True |
| `dependencies/glm/glm/ext/matrix_double3x3_precision.hpp` | 3022 | 52ad25d2c39fa2d1829a024cf1285e4dbc4af0e20636563e13e2491aa484ce72 | True |
| `dependencies/glm/glm/ext/matrix_double3x4.hpp` | 472 | d310d2f564f69576a8492841be71815bd90f3dead089d448227dd7f925e821ff | True |
| `dependencies/glm/glm/ext/matrix_double3x4_precision.hpp` | 1624 | 7c2de092da7dddc03c5c40dbea83d0b9c4ba2b394eb5e3514da691ef35f39d6b | True |
| `dependencies/glm/glm/ext/matrix_double4x2.hpp` | 472 | ffc3fe35dd8e8ab655ccb826621e769cc9dd5c138ef251f6ed33c45e4b9fb435 | True |
| `dependencies/glm/glm/ext/matrix_double4x2_precision.hpp` | 1624 | 8a732fe4f2f8820a688e5c6b36c61678565d432139ef610ad193531863437e2a | True |
| `dependencies/glm/glm/ext/matrix_double4x3.hpp` | 472 | 12a9e5cbfba247e7c719e58f4d6d2e37322a245606e294935c352f4c244b02d6 | True |
| `dependencies/glm/glm/ext/matrix_double4x3_precision.hpp` | 1624 | c02846c09ab692fd77dc82c9b929b2f7a2817a6860b98035f0c376c82528a2ea | True |
| `dependencies/glm/glm/ext/matrix_double4x4.hpp` | 742 | 35f2047fe26f495443c74218fbdc1b404a9bdb14cbd876113929377794255864 | True |
| `dependencies/glm/glm/ext/matrix_double4x4_precision.hpp` | 3022 | 51d5680c95c49f96c04b37829bfff215dd626b3eba495f2efb64eaab3e0435f7 | True |
| `dependencies/glm/glm/ext/matrix_float2x2.hpp` | 737 | 841936453083c65220e826de9eeca359f4519eefc7769671cc99906c9a8aede8 | True |
| `dependencies/glm/glm/ext/matrix_float2x2_precision.hpp` | 3007 | 6ca2a15c6e71ad30b64bc2bc4546858d12c2305a60424eeffc35e906cc3fd4ac | True |
| `dependencies/glm/glm/ext/matrix_float2x3.hpp` | 469 | 6dae07ddd823bea70f59c1f7b74cabd44b8796e05bc80ae5f9f4a54fd36561c6 | True |
| `dependencies/glm/glm/ext/matrix_float2x3_precision.hpp` | 1616 | c3947ce6c001ed94bbf124d11f403c39825d7dd51f4963343f8a04da1f0b655d | True |
| `dependencies/glm/glm/ext/matrix_float2x4.hpp` | 469 | 9dd80a13352ae4f12e29c22bef63bd09803bfa4f1f5f4843c4ec87a8d585f5fd | True |
| `dependencies/glm/glm/ext/matrix_float2x4_precision.hpp` | 1616 | 587470650fad9c426a74fae76e12ca9b339e1246f1604a7ca96c7c32b3d3d362 | True |
| `dependencies/glm/glm/ext/matrix_float3x2.hpp` | 470 | 6f9ed2d1b89187032d12da29c3953831293e92e4c3f8879457cde25bda5ce108 | True |
| `dependencies/glm/glm/ext/matrix_float3x2_precision.hpp` | 1616 | f54ebc067b1528c4439bd56244a17e9f57ed90f6d98b97b47148f4a19be430a6 | True |
| `dependencies/glm/glm/ext/matrix_float3x3.hpp` | 739 | cdf6291801c42f465ba0fcfd07bddd5235be87fb919a4ea6250c11cda22422f8 | True |
| `dependencies/glm/glm/ext/matrix_float3x3_precision.hpp` | 3007 | ee880ab0a3c60ad13d1493dcd75b0d26899a76277b27d9592f0b3e858924cb24 | True |
| `dependencies/glm/glm/ext/matrix_float3x4.hpp` | 470 | 4ac3173d621582453abab434303a475b6dd5251d1ca6ae8810cdf00345ac41e3 | True |
| `dependencies/glm/glm/ext/matrix_float3x4_precision.hpp` | 1616 | 5b8f8a5eff4c138486d7ea4b0f447a47e5d693e9f7781082401e58029403dccc | True |
| `dependencies/glm/glm/ext/matrix_float4x2.hpp` | 470 | 6c0c92f24889e80251b6db6ffaf3efcefddab75d34355356db349322f2ba1ac7 | True |
| `dependencies/glm/glm/ext/matrix_float4x2_precision.hpp` | 1616 | bb2499e042f10111a2cb82cd5eb7c5b124e2883310159b32d12963f5669af3c0 | True |
| `dependencies/glm/glm/ext/matrix_float4x3.hpp` | 470 | f312a05ce56f83f4ae030fc05b00bc6be2755b880a20340ba9dbb77cd0855921 | True |
| `dependencies/glm/glm/ext/matrix_float4x3_precision.hpp` | 1616 | ec7be67780fe92c5226137ab2efa38042abf341cee1ba1e506bd9a214055f10a | True |
| `dependencies/glm/glm/ext/matrix_float4x4.hpp` | 736 | df618841315d9cb6027d9b49537206719e9323332ad31273254c48b348b8527c | True |
| `dependencies/glm/glm/ext/matrix_float4x4_precision.hpp` | 3007 | 944dfc764179cf8d9faced4215b75d603d875f837be4ac7a6aa356233abfe842 | True |
| `dependencies/glm/glm/ext/matrix_int2x2.hpp` | 842 | a620147e0f9124b3038b8b01c03805cb1b5757f812e03eae07fda3873a2b8978 | True |
| `dependencies/glm/glm/ext/matrix_int2x2_sized.hpp` | 1758 | bfc3abaca841ccb6cfde266fbf9a774b274bb6b435ac68611d5c9c8518cd94d7 | True |
| `dependencies/glm/glm/ext/matrix_int2x3.hpp` | 730 | 4d74abaeb3d02348a1ab42af58c4656a21e9b796c725ec2807dc710af5594bc4 | True |
| `dependencies/glm/glm/ext/matrix_int2x3_sized.hpp` | 1231 | f70854e696b1b2a95e1125499140ca8867d7da2f8fd3564874d780337bcd1a40 | True |
| `dependencies/glm/glm/ext/matrix_int2x4.hpp` | 730 | f667fb414097e7ddf2fd7f722056e3e21afb8d21f20ab75898e820f5b16a56be | True |
| `dependencies/glm/glm/ext/matrix_int2x4_sized.hpp` | 1231 | e28652fd0fd8c49eb565a07885f277daa494b6e978526d7f2d65cae49656c91e | True |
| `dependencies/glm/glm/ext/matrix_int3x2.hpp` | 730 | bff626c579914f8f1af96762eac40ce549652ad158f978c02eaba602fe6c04b2 | True |
| `dependencies/glm/glm/ext/matrix_int3x2_sized.hpp` | 1231 | 2013364a7a1dc87bf9dfff9cfe9a640a77a24d81d253040db54c1aa1b0643ffd | True |
| `dependencies/glm/glm/ext/matrix_int3x3.hpp` | 842 | 87d02d438d5c506d2632fa80b62957447c41990b4600396ea7fa6c6075c4a17f | True |
| `dependencies/glm/glm/ext/matrix_int3x3_sized.hpp` | 1758 | d5505b8f0e34a00057c77024699adbae5f17efaafe434bc52552c12daed9e3a5 | True |
| `dependencies/glm/glm/ext/matrix_int3x4.hpp` | 730 | c9e8e6a204e085dd4e8c4ba7885562e6cfcdb9eda5641d004530902aa58b7982 | True |
| `dependencies/glm/glm/ext/matrix_int3x4_sized.hpp` | 1231 | f56444a1efb44b45cd6ad44f9ee2479b05d7a42a5613dda316043d5b9bbe31f2 | True |
| `dependencies/glm/glm/ext/matrix_int4x2.hpp` | 730 | 30b615dddcff99eeb9afc34740610d751c196a847fe146c77694b3d4c65fb64c | True |
| `dependencies/glm/glm/ext/matrix_int4x2_sized.hpp` | 1231 | 8544d801d4d4f4632dace8eebbf224836cd9ea0c9f00bd398a02d17d48eac2d1 | True |
| `dependencies/glm/glm/ext/matrix_int4x3.hpp` | 730 | 7b2108328b78bac9a9f919660918b81cff4f6e04ccc8c2504ad1e0dba6d7ce7b | True |
| `dependencies/glm/glm/ext/matrix_int4x3_sized.hpp` | 1231 | a41fce94c7a9df4d128826ee84531ae987c4be9b7e75ca1e3ee5400e4d4045dd | True |
| `dependencies/glm/glm/ext/matrix_int4x4.hpp` | 842 | 20619f8dc7b01a378fd1a84f949b64286ee658fc9bc1bd410c3d9ff8e094a45a | True |
| `dependencies/glm/glm/ext/matrix_int4x4_sized.hpp` | 1758 | 340140a14b834fee2fd8ea22bf96e26fa1901522ecaf719807e6feeab4d4deca | True |
| `dependencies/glm/glm/ext/matrix_integer.hpp` | 4327 | 9ef29c5f2dff02aadd85a12bd008102e71c96df013cbccae536c1ef2a02e0859 | True |
| `dependencies/glm/glm/ext/matrix_integer.inl` | 1406 | ed5e4698b532e03e9b3a149bd256c30f26ba9e5464a1057f8cdc394660578b01 | True |
| `dependencies/glm/glm/ext/matrix_projection.hpp` | 8208 | 5da44fb43797d760397f0b0c282e982adf1f070b655ef380ac0f7223959e1203 | True |
| `dependencies/glm/glm/ext/matrix_projection.inl` | 4070 | bb0a942e7219ae7083ba196bb6a95cb3fe096162a332f5489a715508dfd8e0a1 | True |
| `dependencies/glm/glm/ext/matrix_relational.hpp` | 6845 | 5d157ffea4189ebc5a7d7f9d1d4efe426d76ecfbb1df1ccd7216e6639356721c | True |
| `dependencies/glm/glm/ext/matrix_relational.inl` | 3179 | 9dc556c33e7f6eeee4cac3f72af288f9642ebb0a515f84cf2785b0b7e0e88d82 | True |
| `dependencies/glm/glm/ext/matrix_transform.hpp` | 7680 | e95ec6ad6d6e403fc3d87fb5c769ad78fd5bfdf3f440fde8165f7a5578534595 | True |
| `dependencies/glm/glm/ext/matrix_transform.inl` | 7599 | 7a90941a5079340f903f8b6c19ac3d8dbc4e5ab82f4feb18e9d3d495a8c2316c | True |
| `dependencies/glm/glm/ext/matrix_uint2x2.hpp` | 857 | 872ebf8d37765e29fc048ea24c1ec087ce90ecf52075fe00d513e6e421ad1992 | True |
| `dependencies/glm/glm/ext/matrix_uint2x2_sized.hpp` | 1798 | 54704646ed5e103a86055c3c503690ea2f7c22fcf56f09326152946973efc9de | True |
| `dependencies/glm/glm/ext/matrix_uint2x3.hpp` | 741 | 2750219cf54b2f06983af4b7d2bd7f7020bddf61291fcbe89a56cd51b42ed5ae | True |
| `dependencies/glm/glm/ext/matrix_uint2x3_sized.hpp` | 1255 | 637ac63599d715801dedfd0f3dc6f78f8c18569792ae5525719725690ac7e512 | True |
| `dependencies/glm/glm/ext/matrix_uint2x4.hpp` | 740 | 5866045e6c781649c96760a8fd6f615941f8be34ba89ae544fd2904e6e7ecaa5 | True |
| `dependencies/glm/glm/ext/matrix_uint2x4_sized.hpp` | 1255 | 204c9a207e17b416cab76d46727edf57432a38f54e49b1987d8385128b8b1610 | True |
| `dependencies/glm/glm/ext/matrix_uint3x2.hpp` | 741 | 2bacd8d27fde66217ba7d7e249f4fa0f45a96e00fa8928fd864b7b6923e304ad | True |
| `dependencies/glm/glm/ext/matrix_uint3x2_sized.hpp` | 1247 | 4d73e62033ac02d65b16343384dbcf6b338bd0be167d07e17b16587e2e6e8a10 | True |
| `dependencies/glm/glm/ext/matrix_uint3x3.hpp` | 857 | 5cf424210dddc3c92608df44c68d6faf7c8c9cf4b76d1051f90654ebc4d3a926 | True |
| `dependencies/glm/glm/ext/matrix_uint3x3_sized.hpp` | 1798 | 68d0dd82b154ed3ccb1e2b7a9fddaa40ec748c1c18bbb6546f15f97d324c8a73 | True |
| `dependencies/glm/glm/ext/matrix_uint3x4.hpp` | 739 | 090fab8a63aeee49aba46a04bbbce100b6e48a0dbbbf535554ff6d9e77a9f6db | True |
| `dependencies/glm/glm/ext/matrix_uint3x4_sized.hpp` | 1255 | 4c2b34a932e0ff1c7404b9b44a10829fd0b51d4b785594b1acb6d4d5a35ecd85 | True |
| `dependencies/glm/glm/ext/matrix_uint4x2.hpp` | 741 | aa2171132e5da762562ac2f25c2e1969e966c0a1df380886d7bca8623363b728 | True |
| `dependencies/glm/glm/ext/matrix_uint4x2_sized.hpp` | 1255 | 1884239d1ce81d55fc84663caab98bd4a4142c636b0e605775599b419004768d | True |
| `dependencies/glm/glm/ext/matrix_uint4x3.hpp` | 741 | 1b73e12c2c03f5f15e25dd9894a2e0b9bcc8006bfb202a872df53dcdc3a95bb5 | True |
| `dependencies/glm/glm/ext/matrix_uint4x3_sized.hpp` | 1255 | 4f7d80da919973d9b7837108cd1e1edfac1829e60895cb5687e0554dee33e8a9 | True |
| `dependencies/glm/glm/ext/matrix_uint4x4.hpp` | 857 | f6123f7e7ec862b2f276212ce1cb45cdd9bee37ba0e8c2db2ea90f3739ba5e93 | True |
| `dependencies/glm/glm/ext/matrix_uint4x4_sized.hpp` | 1798 | ffe916f61d78dec350673e9e97506df77395f5bc69905901daacce05efc6e1bb | True |
| `dependencies/glm/glm/ext/quaternion_common.hpp` | 5020 | 1fb9fc6207c850b593c8a51a776584cd08f1727157f7790b935d3f9a0c898d61 | True |
| `dependencies/glm/glm/ext/quaternion_common.inl` | 4806 | cbcf7fe43a66ca691f044d7a7249db5dd7e76d4d3b1dd671b60758b2db1f1915 | True |
| `dependencies/glm/glm/ext/quaternion_common_simd.inl` | 393 | 16f63f092b0f80d594d28840cc9c3bae7720323606518d5a5eaa19f541284e5c | True |
| `dependencies/glm/glm/ext/quaternion_double.hpp` | 1027 | 12954acc5a09e387c182961c1b1377bc881aba7458933f9b5fbae7cc2c28e8ca | True |
| `dependencies/glm/glm/ext/quaternion_double_precision.hpp` | 1363 | 24f6e7732fd93f9fa87d8cd8326e2f687758cd1c55cbf598b5b6ed975be58b6b | True |
| `dependencies/glm/glm/ext/quaternion_exponential.hpp` | 1807 | eadb3c5cc99be4ccafc32de336c117af139d472651364a94adebf04480a7d5cb | True |
| `dependencies/glm/glm/ext/quaternion_exponential.inl` | 2950 | 5f156104bdcb6611db7343f3c570fbbef65a18d32cc1747cd2b31a19533bb8a9 | True |
| `dependencies/glm/glm/ext/quaternion_float.hpp` | 1018 | 4da67234432fb03d656e0d98ca625245a344d314bc1132d85d40fd7b39f2f7ca | True |
| `dependencies/glm/glm/ext/quaternion_float_precision.hpp` | 1192 | 3b923827b8c728df9d901f05f7c6a573122a9e8f047727f174ed0a97dbccf7ac | True |
| `dependencies/glm/glm/ext/quaternion_geometric.hpp` | 2017 | d5c3823f9e3fbc079f4abffa1e099f2f20dd61e6fd1d7bd347ce13185a754f4a | True |
| `dependencies/glm/glm/ext/quaternion_geometric.inl` | 1350 | e2be0c800cd2a635c26e66747556990628b132336aa2e2769672ba2558a5dbe2 | True |
| `dependencies/glm/glm/ext/quaternion_relational.hpp` | 2019 | d60b00fa64320affda373f34e8f76801414a120ad6371d190e2f89f3b3c14dcf | True |
| `dependencies/glm/glm/ext/quaternion_relational.inl` | 1057 | 6c2ec05dd24cdf1a566c973b41aeb9d9931ffc02f849e515acd40c28a972c7a8 | True |
| `dependencies/glm/glm/ext/quaternion_transform.hpp` | 1381 | 5ea9afad1a212cad00de121a6c50ccd4e4b37546b68439ba436fdc09b274fbda | True |
| `dependencies/glm/glm/ext/quaternion_transform.inl` | 662 | 56b6eabbd2b08fd1bd5400e2c7455ec77ae927376801db92b4f2c6b3fc3ed99e | True |
| `dependencies/glm/glm/ext/quaternion_trigonometric.hpp` | 1925 | 4ad10d61b77f92f1347aac5855b7bc76111220132440f80f8e84435d25976f66 | True |
| `dependencies/glm/glm/ext/quaternion_trigonometric.inl` | 1028 | c2e8992300392f5c63cb107ec04b1a073df8f6aa1a43cab6dd809740dcdb5b95 | True |
| `dependencies/glm/glm/ext/scalar_common.hpp` | 6298 | 89e6c98ad49b61cae2d70c841a110f5c30d2ee802983a070d4e8be2bf070aab2 | True |
| `dependencies/glm/glm/ext/scalar_common.inl` | 4712 | 8202367aa7c4f20b48fa0281e77b28424d55966f3a78f08e1663a6a3435c8be3 | True |
| `dependencies/glm/glm/ext/scalar_constants.hpp` | 1080 | bd1cea6ec26b60549f6a982e4178e53a93b8b0659c258167e4022b50b64bb83a | True |
| `dependencies/glm/glm/ext/scalar_constants.inl` | 803 | 244633feae7f575b4c96c6e245b7cd1aa94a29c0911fd6c99b54de29295a09be | True |
| `dependencies/glm/glm/ext/scalar_int_sized.hpp` | 1402 | fa4d9e88289df1c89fe98bca956489d3f55c1aadc20b3b78f3e67add678c741c | True |
| `dependencies/glm/glm/ext/scalar_integer.hpp` | 2778 | 79ea6f569f61dd5e8dd5b5452be1b4c9261afc86d92247ff1632423617c355f9 | True |
| `dependencies/glm/glm/ext/scalar_integer.inl` | 7256 | 2888ba2a348ba38e3d52e5f5c59de47ea0a20e7c2acd281f3b2f80b02f2fbdb1 | True |
| `dependencies/glm/glm/ext/scalar_packing.hpp` | 711 | 0872250cfc4ea74705ee974d6fded5acfd71fdc1796fd418c9a5691195fa6c70 | True |
| `dependencies/glm/glm/ext/scalar_packing.inl` | 3 | f1945cd6c19e56b3c1c78943ef5ec18116907a4ca1efc40a57d48ab1db7adfc5 | True |
| `dependencies/glm/glm/ext/scalar_reciprocal.hpp` | 3693 | a71422a9b7349fc3d66ae49153fdac2233b451f7c925fd4e85c765388ef47a4c | True |
| `dependencies/glm/glm/ext/scalar_reciprocal.inl` | 3561 | 2c8569ac70e4848ab43250007489111e131169a6e32de8db0c69de5ed4e67177 | True |
| `dependencies/glm/glm/ext/scalar_relational.hpp` | 2383 | 8438969d52284e5e74e44b630129a4ab28636ca95124e9e5f52cd7496363f896 | True |
| `dependencies/glm/glm/ext/scalar_relational.inl` | 1170 | 7f2bbbfeefac15b3aa431cf12c924c36c362d4a5390e3bcbc915188efd071b54 | True |
| `dependencies/glm/glm/ext/scalar_uint_sized.hpp` | 1439 | 40be3227bcbae6e41e6de317f1455a76a2d298def19a99209a8d6670f3fa993a | True |
| `dependencies/glm/glm/ext/scalar_ulp.hpp` | 2171 | f06023e60c8228be85e97b9324619fa451b4737e7d9f01d78d9e96cb7f43c79f | True |
| `dependencies/glm/glm/ext/scalar_ulp.inl` | 7737 | 6df0296f088293af2876483584645085aeb84a444fc425c92b540142f7a4c4b6 | True |
| `dependencies/glm/glm/ext/vector_bool1.hpp` | 682 | 4e66b1635bebb66ad36e9666f01fa79fb2fbb66e0df77d53c95eee00deac9a10 | True |
| `dependencies/glm/glm/ext/vector_bool1_precision.hpp` | 913 | 6d648e99f532b92099f44b5a7059361080b312e878c7b425de1c8502d7e323f6 | True |
| `dependencies/glm/glm/ext/vector_bool2.hpp` | 413 | 4aa38230cb6e0d7da105e2f89567cce9960af50bdd072e60e8ba52c88ff86305 | True |
| `dependencies/glm/glm/ext/vector_bool2_precision.hpp` | 1374 | 028f7315009d69facc70cf14d2582a9005239df2b0c22d13ee8bcb1d2680581c | True |
| `dependencies/glm/glm/ext/vector_bool3.hpp` | 413 | c693f619905f83ee8e16aa776ae86c8c737039ab05292a9430fe1dde74a5a140 | True |
| `dependencies/glm/glm/ext/vector_bool3_precision.hpp` | 1374 | 2f02eb6ce170ab9a17e2f52af4c2bf3ae4ab2640f34f85b28223c6bf7e331ea9 | True |
| `dependencies/glm/glm/ext/vector_bool4.hpp` | 413 | 4a64578845386c0c91698ab8b5f4b5d09baea94ab3a53e2b869da41f4afb4dee | True |
| `dependencies/glm/glm/ext/vector_bool4_precision.hpp` | 1374 | 614c4e94957edd22503e557687d56bbf4490029deff6a5a3933e95fd1019baee | True |
| `dependencies/glm/glm/ext/vector_common.hpp` | 10689 | c1a6eb0bdbf3609ce2f3481449bbcdea43bc8f91af28470f21d716a12e3d7fe8 | True |
| `dependencies/glm/glm/ext/vector_common.inl` | 6189 | 52af3d66fda5958946e046dad3ee1cb52dfb6f84e5c36a8720f155f14bc4236b | True |
| `dependencies/glm/glm/ext/vector_double1.hpp` | 816 | c10b120ee00c980d924f2ab5979c1368033369fffe103464297499b8120ee102 | True |
| `dependencies/glm/glm/ext/vector_double1_precision.hpp` | 1194 | 8f8246a4b9b0bb1f76e59b6e14b021cbdace56304710ae37d735359c3ae367f8 | True |
| `dependencies/glm/glm/ext/vector_double2.hpp` | 449 | baf54247150a183acb161a4c905b01736e6f01313b438a4725d8d72da2c29e67 | True |
| `dependencies/glm/glm/ext/vector_double2_precision.hpp` | 1434 | 12370946f9ab21ec2cce2d9f226d74aa515c0b9e48b567b7daea370f6c4223fc | True |
| `dependencies/glm/glm/ext/vector_double3.hpp` | 449 | e3299c9b32f98d3a8a63686101b18e25292f6a157f557e23e386472d69a71c29 | True |
| `dependencies/glm/glm/ext/vector_double3_precision.hpp` | 1593 | 1355cf513265fa8717c219fc6f3dfd22bed2546b1a201b0b85a03af5509b97ca | True |
| `dependencies/glm/glm/ext/vector_double4.hpp` | 449 | 49ff2cb0b45e88f975cdcdfe8622b1663a4024a1d50cb1167a3c8646d18f204d | True |
| `dependencies/glm/glm/ext/vector_double4_precision.hpp` | 1625 | e943fba185e8b191b691223cd8bec4b92d2cbe909dc1d2f4f838da2565163735 | True |
| `dependencies/glm/glm/ext/vector_float1.hpp` | 807 | 6f4c821805c967856ada823c555efaa9b8e2fe037a4f60691b5ec922fc9109bb | True |
| `dependencies/glm/glm/ext/vector_float1_precision.hpp` | 1189 | 1c7b780b9895f72caab184f21498626f56f4776d87b2279945b923c915bddba2 | True |
| `dependencies/glm/glm/ext/vector_float2.hpp` | 445 | 5c3746bf09b27d0e4444b17e0031b7f375a67b001ddda19134e08bc97e75e690 | True |
| `dependencies/glm/glm/ext/vector_float2_precision.hpp` | 1428 | a5dafa12f4b870c072c538e8145093f97d4b2f5f744ce93836cac2b3ac428774 | True |
| `dependencies/glm/glm/ext/vector_float3.hpp` | 446 | d5fb00ea964bafd7a52cc491fa7d968bd58cbdbdb6065e1663879b56f4200a85 | True |
| `dependencies/glm/glm/ext/vector_float3_precision.hpp` | 1428 | 048bfcd179ef54535fea651f4813c826812b02ddc77449da18b361a511745778 | True |
| `dependencies/glm/glm/ext/vector_float4.hpp` | 446 | 3a8ef67ddcc7dc30ddde9a0609d6954ed025db1ad578f754c399e3ab98a97359 | True |
| `dependencies/glm/glm/ext/vector_float4_precision.hpp` | 1428 | e3d2401739a80b5ab05244bea13f3f8f0d69db54c293b6d3726a55234742f903 | True |
| `dependencies/glm/glm/ext/vector_int1.hpp` | 728 | b9b927b0f6a2fb9cb3f87df7e986e2e3e125850b302417e4b97aec450be40d73 | True |
| `dependencies/glm/glm/ext/vector_int1_sized.hpp` | 1264 | fcfe212a8eeac8b26d6a89fe7f22cd7a8fb4d7edf0b4cd886583a86afc489811 | True |
| `dependencies/glm/glm/ext/vector_int2.hpp` | 426 | 672675ee53c6b56e6512db4f7094349555ae05a38be135c08360255ed555d578 | True |
| `dependencies/glm/glm/ext/vector_int2_sized.hpp` | 1287 | 4c5188a656f0f38d76d98d7986c5ec305d3f9f3d921cac8baa756123746e26e3 | True |
| `dependencies/glm/glm/ext/vector_int3.hpp` | 426 | 2565ece98d1f17928c353935f0bcf480b4be9db70720b1f13e0a54a022ae83c5 | True |
| `dependencies/glm/glm/ext/vector_int3_sized.hpp` | 1287 | 19b06a4e05dd0883762048c5e5f56b8f3e51471aba0ec4f90b9fbbac593fb787 | True |
| `dependencies/glm/glm/ext/vector_int4.hpp` | 426 | 0dcf10a6840a6c2652c9ba79df912d2921f0b7f4fe0f037c8aed4e71bc6e2502 | True |
| `dependencies/glm/glm/ext/vector_int4_sized.hpp` | 1287 | b66d6d6a64d5fe38ad6f7a4ef78f083e8881c48489324e2998a8816b8c093cb3 | True |
| `dependencies/glm/glm/ext/vector_integer.hpp` | 5662 | d13628dd89d1bfb51352a418fe9b50531eec9e55b63fda77811301afad3913da | True |
| `dependencies/glm/glm/ext/vector_integer.inl` | 3419 | ccd91c197259300d89eb7698d5709b3046db72bb6a2092ab0b4cd2222c92ae14 | True |
| `dependencies/glm/glm/ext/vector_packing.hpp` | 705 | 4d090c95c9daea82f9aec67f70ac2f25f59d704fdd86df869f072c408748212f | True |
| `dependencies/glm/glm/ext/vector_packing.inl` | 3 | f1945cd6c19e56b3c1c78943ef5ec18116907a4ca1efc40a57d48ab1db7adfc5 | True |
| `dependencies/glm/glm/ext/vector_reciprocal.hpp` | 3693 | a22d5156397e7bebaa60920674e90df6027694afbfd9f4d270acfe92f683d51d | True |
| `dependencies/glm/glm/ext/vector_reciprocal.inl` | 4356 | 190710a5e39e8566b4b8dd98ca802bd28dd05c476f90758d0dd9572075e3e2dc | True |
| `dependencies/glm/glm/ext/vector_relational.hpp` | 4639 | 4c5de772115a0474718b8f86542c1a561cbf1df67fd4802fc70f947dbd51440f | True |
| `dependencies/glm/glm/ext/vector_relational.inl` | 2541 | c360805dbd2c4de9fcb1ee2aa7b7af16d379fbca5ac615298ea7d3cdf3bc6179 | True |
| `dependencies/glm/glm/ext/vector_uint1.hpp` | 746 | 88dcd41f191dae33cc29a28137a74b23cd85775f4fbba34bcdb4326e17bd75cb | True |
| `dependencies/glm/glm/ext/vector_uint1_sized.hpp` | 1292 | cd9eaf3225ec83c15a1e07777dc2eafbd8cc58749d018717b9d3718b80b540b2 | True |
| `dependencies/glm/glm/ext/vector_uint2.hpp` | 438 | fe84b2317032f3e0f2073c5ad123f8fe0ed13e3990d4dec6046317765d825370 | True |
| `dependencies/glm/glm/ext/vector_uint2_sized.hpp` | 1311 | fa7c2a827e3daa1b755d0013b0469d9f471f5909aa1773b00d102c06b04535b8 | True |
| `dependencies/glm/glm/ext/vector_uint3.hpp` | 438 | 1a73a9736b1613b322d11f19d94e48eaf23c0ae35d79a81dd5e321890255381f | True |
| `dependencies/glm/glm/ext/vector_uint3_sized.hpp` | 1311 | ada490ff5df88c558a6eeab144ca276336b1375206e56a03dade776dd63c4dd8 | True |
| `dependencies/glm/glm/ext/vector_uint4.hpp` | 438 | dd08786412313d539b83113ccb5c9081942abd8ab2969dc8e1d8b1cc472308e2 | True |
| `dependencies/glm/glm/ext/vector_uint4_sized.hpp` | 1311 | 056290350fee7cf0f122056e4ec365ef36c3d67643b098a4c68f178a9a7a1116 | True |
| `dependencies/glm/glm/ext/vector_ulp.hpp` | 4004 | 4fdbec97530a16028e33f381f00a52bb448e1613387ca882787dd5ce433c871e | True |
| `dependencies/glm/glm/ext/vector_ulp.inl` | 2295 | 2625eb167985054bcaed6670d5054535d34b545c2c0655ce7220d751f864ea64 | True |
| `dependencies/glm/glm/ext.hpp` | 9155 | 9c57ebe0b9636d216ed395d26b5aa38d6ae35aeb9d2be7dbf18af9fd161ea876 | True |
| `dependencies/glm/glm/fwd.hpp` | 46778 | e0a9c3a31d3f1000758e7b6a5a98e3eee07d32e20ba2ec47653d9f15ebf63631 | True |
| `dependencies/glm/glm/geometric.hpp` | 5498 | e60e39fb5d98cbca4135f902461ff76e0eb059de9355684f4d10623cbecbf523 | True |
| `dependencies/glm/glm/glm.cppm` | 73247 | 512a3eb2b218f405705da07fd244f027efc816104d8caa197a94a4da3de345ab | True |
| `dependencies/glm/glm/glm.hpp` | 4643 | 9bb34dc8dba5e23e74c50943f804873532919b161fecc3e1a9247f9890d498d9 | True |
| `dependencies/glm/glm/gtc/bitfield.hpp` | 10147 | 944b295cf9c0cf45d3f2baa98680c6ebddb135e7f7ced239d64b71c23d0a7be8 | True |
| `dependencies/glm/glm/gtc/bitfield.inl` | 20997 | 17c33ad1b4459cf024c73a30fca0e48beb020bca6eaeb647967f8f511ec92e37 | True |
| `dependencies/glm/glm/gtc/color_space.hpp` | 2023 | e125a247aa5165ef8b47ad9f2db5429f28a61d80c96edaf8a73b96995362a987 | True |
| `dependencies/glm/glm/gtc/color_space.inl` | 3041 | 66211e68c6bc090bd971efbd20eae1c0bf9e00eaf53b6d8175a91c8050eaacd6 | True |
| `dependencies/glm/glm/gtc/constants.hpp` | 4334 | ef2a4934378a306f57d0fc8daf00e210e3ebc6fcda47d4c2a60714062ea82f23 | True |
| `dependencies/glm/glm/gtc/constants.inl` | 4124 | b6da0725b14830815f4839f8bbcb425d8695be173e058fadc56f74907610ebec | True |
| `dependencies/glm/glm/gtc/epsilon.hpp` | 1831 | f258d822295ba7e5c3a55da3da06ab59af8805567f0fd3e1dfb33139734e8477 | True |
| `dependencies/glm/glm/gtc/epsilon.inl` | 2279 | a7c2f8b051b925998af2d11c445d994a84e2b1abb292ee2032cde41864726fef | True |
| `dependencies/glm/glm/gtc/integer.hpp` | 1097 | 17f146d81d03887d11c753eb580db2d29593d633e488c49f44142e8b42b4ddac | True |
| `dependencies/glm/glm/gtc/integer.inl` | 1087 | ece2e3c9e1ecbf754369c244503a25f4e772299d08ac98dc6692e33a2d24759f | True |
| `dependencies/glm/glm/gtc/matrix_access.hpp` | 1471 | 87c20ed9ae8eaf7d993febf6783fb4d04011667f78636d5eeed15ba0b9d47507 | True |
| `dependencies/glm/glm/gtc/matrix_access.inl` | 1190 | 6bcb64d5245584ebfabf06fc5166c9ffd45b1f832f13077b09d6f5e8607711c4 | True |
| `dependencies/glm/glm/gtc/matrix_integer.hpp` | 13049 | ccfb7d9ae7a65b0eca54af01ed5d079f0e8d75788c4db56b2d81499e4f47f6df | True |
| `dependencies/glm/glm/gtc/matrix_inverse.hpp` | 1520 | 3272b7c4c6abe6d69da51c439a68ad35fbaa3e6d1af1cb85aa35cfaa08b9f493 | True |
| `dependencies/glm/glm/gtc/matrix_inverse.inl` | 5049 | 5d70d3da6d92bb829acad9351ef7022b780ada7c7981e0b6c719c4dff1c4db33 | True |
| `dependencies/glm/glm/gtc/matrix_transform.hpp` | 1229 | 0aef74b23433b727dfca0bbe35f3bb9bb14f313346ba1c5c4f971d825d580a5d | True |
| `dependencies/glm/glm/gtc/matrix_transform.inl` | 91 | 30242dca1319f41aa6286c4cf0374ec6af0a5d6169fae9cfa125dd715f0eb044 | True |
| `dependencies/glm/glm/gtc/noise.hpp` | 1556 | c8a2e2628315549cc8c62e1235cfb05cb52d242b188d9f99e7b17670de25febd | True |
| `dependencies/glm/glm/gtc/noise.inl` | 34008 | 0099a2804df9b679a8c7dd21c1d7508da108a2fca8d9d42b6c734a9452eba5b2 | True |
| `dependencies/glm/glm/gtc/packing.hpp` | 36719 | 7f68657057f311c373eb43237a7d646e97d3b2ce121630f4c9056ec3fc720534 | True |
| `dependencies/glm/glm/gtc/packing.inl` | 26457 | 3e48c8c98836323d2d95d2a6e2fe16864dbfaefda6ae95da74d438549ec49851 | True |
| `dependencies/glm/glm/gtc/quaternion.hpp` | 5697 | 2dbc2c30ad5c24201aadb186f9e757cfc89006bb4b765740a1ab5ae5bce773b2 | True |
| `dependencies/glm/glm/gtc/quaternion.inl` | 6628 | ff39757356667069995e28ed0e2793ee649b2cbe2f5e1b67cdaa7706cd94c2d1 | True |
| `dependencies/glm/glm/gtc/quaternion_simd.inl` | 3 | f1945cd6c19e56b3c1c78943ef5ec18116907a4ca1efc40a57d48ab1db7adfc5 | True |
| `dependencies/glm/glm/gtc/random.hpp` | 2696 | ca608d4c85ad790b64d60d33d441c95f42eb266f9eb0246f096d145c192aa8b8 | True |
| `dependencies/glm/glm/gtc/random.inl` | 8949 | a540bbc165dbb6ab2d26469d924f157a3c407189ffc7cab2b1b20ac4bebcfcbd | True |
| `dependencies/glm/glm/gtc/reciprocal.hpp` | 597 | 7ec8b0a4147138782021371633afc5382a551dce3d8f2db4a43687c5ac291e5a | True |
| `dependencies/glm/glm/gtc/round.hpp` | 5441 | 1cc9d5b26c98b84e6353b6de414ead7e5df611573c63afc3c61dfa1e105c289f | True |
| `dependencies/glm/glm/gtc/round.inl` | 4532 | 13984660fb15b5bb1c5781e99f16ae48bfdecb8346c30044e31b5ae34cae35bc | True |
| `dependencies/glm/glm/gtc/type_aligned.hpp` | 72543 | 58fb1b4309a4e39f2eddbc71f054e85de6d6956e5edd1e424ae4ef815d7c6c97 | True |
| `dependencies/glm/glm/gtc/type_precision.hpp` | 66318 | d594ee0cd495ff3374df469370b0203c401743723e82803dd02c59e7165817ea | True |
| `dependencies/glm/glm/gtc/type_precision.inl` | 52 | 05bbecd4bec385a010755790ec01745fb8efa074056747c6ebdeb6694fbd87a4 | True |
| `dependencies/glm/glm/gtc/type_ptr.hpp` | 7059 | 0b8fdea75e9840608eff08a07215d25da2e282281122c922b35de283a43b2cdf | True |
| `dependencies/glm/glm/gtc/type_ptr.inl` | 9292 | 54c5fd6072d79c81acd6f1f7c8bf70d699cef15cc25b28b47176354b89506d9d | True |
| `dependencies/glm/glm/gtc/ulp.hpp` | 5234 | 7114ff35d57f68928875c61b3a95b5ff07fa44ddf0508a0c82829c484e0df4ee | True |
| `dependencies/glm/glm/gtc/ulp.inl` | 5251 | 5639d486c78c7a28ad5c536350f372420824802b41fba04644868e00a5e6be22 | True |
| `dependencies/glm/glm/gtc/vec1.hpp` | 841 | d7c73593b3b73776a984544d35bbcc52825361b2d6b2a07ed93ad032e06a030b | True |
| `dependencies/glm/glm/gtx/associated_min_max.hpp` | 7797 | f088e363eb2550cf00d21e95246e08e08f602d209c0b7772f631490ab78f86a8 | True |
| `dependencies/glm/glm/gtx/associated_min_max.inl` | 8880 | 37ed87051be23099e0b4959cc0ef65c037b2cafb1769a5c161958657f372621a | True |
| `dependencies/glm/glm/gtx/bit.hpp` | 3111 | 5a31a4322679ce43c0314a90feb2287223e1f0022f7f51b4dc75cb5c25893ab1 | True |
| `dependencies/glm/glm/gtx/bit.inl` | 2357 | acfc45a2147ef4c6e4b5f98bb8a1a74b8199ddb0e23ecd334fa784e0359cd23a | True |
| `dependencies/glm/glm/gtx/closest_point.hpp` | 1351 | 6c49123b2b9b6a75be8eeec3888f6fd90f2ff0cc933cd4aee132662ed8560077 | True |
| `dependencies/glm/glm/gtx/closest_point.inl` | 1165 | 9ba0da204fb39ed0ffbcab1737d84f7b692029ae26b6bc9ddcc7ffcbfbe0673c | True |
| `dependencies/glm/glm/gtx/color_encoding.hpp` | 1705 | 626676411a4141e69019ddaecaaccbc80dcb298110b4f5ea4870901f57358c88 | True |
| `dependencies/glm/glm/gtx/color_encoding.inl` | 1795 | 33ad9c49d55427886d86b4030c4f200f568bedda94e64a5259a7b8db2d51ff65 | True |
| `dependencies/glm/glm/gtx/color_space.hpp` | 2020 | 3710628a54217adab3d4d6a522dcacbc4b8b468e4b6a4ce5e2dacae026dcc549 | True |
| `dependencies/glm/glm/gtx/color_space.inl` | 3481 | f47e6e2ca4c7c42c0f591700ffb4bd5ad468ba36aa0711ab3887c304324b244c | True |
| `dependencies/glm/glm/gtx/color_space_YCoCg.hpp` | 1901 | 9a13bc9dc9af09f1197657937520bdfe8e1b82842e2c1bd16c3afeb79ae8600c | True |
| `dependencies/glm/glm/gtx/color_space_YCoCg.inl` | 2847 | a2eb82983d935ac152e4b8419be2bc571f57bcf5d419d062adbddcfa2de60f73 | True |
| `dependencies/glm/glm/gtx/common.hpp` | 3167 | b796116b0a017700c6d2b8ea35f1ae5f7d050c8f2fb195ce6c1ce85b9ed2eb0d | True |
| `dependencies/glm/glm/gtx/common.inl` | 3738 | e4bc51659970309172115be4ff6eaa298a1a1cfc4987efb04a8cfd002736b603 | True |
| `dependencies/glm/glm/gtx/compatibility.hpp` | 15285 | 472c95fbcbd2ba8c68a323204c3887cd0a990a24955c7f0a8efc2fa5e833b7c5 | True |
| `dependencies/glm/glm/gtx/compatibility.inl` | 1746 | b3545942c4d90cc1ec0d92b5b8c4593b71c9230cd1104a43fdda923b25491912 | True |
| `dependencies/glm/glm/gtx/component_wise.hpp` | 2792 | 93da54d0d08054c6d4f72d1f607f6f4c1f1ecec9c62e9003bfd30b0e8acc434e | True |
| `dependencies/glm/glm/gtx/component_wise.inl` | 4854 | 17fadf08e210c7b51b33cf159bf6d93a40ad0cb664800fdf06aa139a83896760 | True |
| `dependencies/glm/glm/gtx/dual_quaternion.hpp` | 9668 | 69413d31fefcfcc07230bb75129965518ea8dbd50637a7ef71e85024ce354980 | True |
| `dependencies/glm/glm/gtx/dual_quaternion.inl` | 10764 | 4fe3726f9ddc7c4b93e0e988fae986aaf519b91ae2e48cc41ec85e812f25f536 | True |
| `dependencies/glm/glm/gtx/easing.hpp` | 7068 | c95a97d9492b48807c0f3f9669f91faa67d715ad7005df341fe930eca88bab0e | True |
| `dependencies/glm/glm/gtx/easing.inl` | 12156 | f1cc823b653328a374aadce461511436b5bdf5f236c6adbf12d2d9b90d5121a6 | True |
| `dependencies/glm/glm/gtx/euler_angles.hpp` | 11080 | 7b0a85c5f4bd0aac4197ba6f48ea079c1017888aaba59b21a6aaa515c596e354 | True |
| `dependencies/glm/glm/gtx/euler_angles.inl` | 23199 | 2f383b23679461c9a39b494d07f2c1daf6db7c3d9560e4b8c04edf370b45b5a8 | True |
| `dependencies/glm/glm/gtx/extend.hpp` | 1086 | 6974d3d820af31ceceea75f84b776051be7764365be8625b889c58593ccfaf31 | True |
| `dependencies/glm/glm/gtx/extend.inl` | 952 | 68c770692b484a84dd8be8c6713cbd0b9a9c76e18eb6c0ddd8f28ddee04d72e3 | True |
| `dependencies/glm/glm/gtx/extended_min_max.hpp` | 3686 | bbe9704c6bc9052874340b04174f45ea35c941594c5618649c55f2960b9ab0bd | True |
| `dependencies/glm/glm/gtx/extended_min_max.inl` | 2490 | 99807758d5be8ca9f27a6dbf8de28bd78fb552bf99430da5e04d7089fe7de9d6 | True |
| `dependencies/glm/glm/gtx/exterior_product.hpp` | 1370 | 210823a450ece948381ac71c8ce86613e0ec2012e6749f43ea0629b64c8c6cfa | True |
| `dependencies/glm/glm/gtx/exterior_product.inl` | 723 | be51fb66433653dfb91b1dc6511504235e368496bde0653256b6fd3606645527 | True |
| `dependencies/glm/glm/gtx/fast_exponential.hpp` | 3188 | ba3f1c8a479162e925425ed52a1916000e5fda618175038772b82b067db8cec9 | True |
| `dependencies/glm/glm/gtx/fast_exponential.inl` | 4387 | 65f8afa2ce0c65feefa57857b74c204ec1d98e0f7dc1e095bd2627542f62a9de | True |
| `dependencies/glm/glm/gtx/fast_square_root.hpp` | 3226 | e512ee5439d68d182ca3497bcc887cd51f4b11d6297f5c1425d6101e8a651055 | True |
| `dependencies/glm/glm/gtx/fast_square_root.inl` | 2221 | 770303c3ba5af5bee51a1f842fa76a7cb8428feb6099fc73a1bc32e09f4591b7 | True |
| `dependencies/glm/glm/gtx/fast_trigonometry.hpp` | 2449 | f976e42c761d7423d86b9150d1b340f47ce96abe3d1eb4b1089d53097e1258ef | True |
| `dependencies/glm/glm/gtx/fast_trigonometry.inl` | 4159 | a8bf654cd093c92c6214177ea5ce9349f4b3940efb785ac597fc61d6fdb73b59 | True |
| `dependencies/glm/glm/gtx/float_normalize.inl` | 309 | e6039466bf2eb2af27c2415cb94990ae110666571950d6cdc56754d3b2478ce8 | True |
| `dependencies/glm/glm/gtx/functions.hpp` | 1317 | af7708c25d6388432b0b8149ebb0a1a6d6b095ac6775366374344d8a689c8083 | True |
| `dependencies/glm/glm/gtx/functions.inl` | 806 | 296374bc1285d535f75f50a2ae8393f9b79a42ebf59ffd7f63178b4433faff5a | True |
| `dependencies/glm/glm/gtx/gradient_paint.hpp` | 1489 | 6d9cf170682fe4582a025be1c194c63d6a18cb1447893200e097c92e3964314b | True |
| `dependencies/glm/glm/gtx/gradient_paint.inl` | 943 | 2fc2cfc87630fb2cf6b0df649a2c32642d08af783393b4d0f2e889eb135f6659 | True |
| `dependencies/glm/glm/gtx/handed_coordinate_space.hpp` | 1538 | 1cb8639ba63ab373d6be2948ec9af01d8b242e0ab8a4d28e2287e35abdc0bb93 | True |
| `dependencies/glm/glm/gtx/handed_coordinate_space.inl` | 562 | 6a780257eb98ed85aba8c5f3fee5f290e7587c139a67d0d9dfb45940266dca8e | True |
| `dependencies/glm/glm/gtx/hash.hpp` | 4280 | 93768ca2d74e33e6e6cdd24ccc018bc78152f3e6cd52e984c9da9cbe2111adb0 | True |
| `dependencies/glm/glm/gtx/hash.inl` | 5784 | 53ac1b58aadf3268045c556a3dc5d54a0b4832b9b9285df92e64c84bc14fd819 | True |
| `dependencies/glm/glm/gtx/integer.hpp` | 2217 | 132b453d75983b2d73b1482afc56bbae42ee2360581b50a8b3684fe0d3f03d8d | True |
| `dependencies/glm/glm/gtx/integer.inl` | 4045 | 6b84077a7c0b76fee111e048c4c96b2e4787a373281fd3d37c3102ded6cef972 | True |
| `dependencies/glm/glm/gtx/intersect.hpp` | 3417 | 210df0efe55c852db4bf44b6f67dde5d93e399ef55b9bb84b2e30495bba1c83b | True |
| `dependencies/glm/glm/gtx/intersect.inl` | 6865 | 209bd241af8d28aa59435ba0bae077f8fa18353d44a394dad11a3557b1d2f79b | True |
| `dependencies/glm/glm/gtx/io.hpp` | 7859 | db2105d6ab4837426a889d373f4b772a50272fd646b579010c45a0669ed09561 | True |
| `dependencies/glm/glm/gtx/io.inl` | 12766 | 22f57ca9b45a1bc13b6827efc13bbf2cadbeb3ddb069cb2c58027d16fa704d47 | True |
| `dependencies/glm/glm/gtx/iteration.hpp` | 3846 | 9a7083287f735879b191e556003ff7fa80f62ed36f3ccedec9cce718e546f641 | True |
| `dependencies/glm/glm/gtx/iteration.inl` | 4159 | 026b3c703599c6cc708aec0d093c2fb94a37ecd76baca6433ede72bd12fd2636 | True |
| `dependencies/glm/glm/gtx/log_base.hpp` | 1193 | 0cde266163798e0baf1789561b7f8c813dca7bb68a5e64152f8846282ae30473 | True |
| `dependencies/glm/glm/gtx/log_base.inl` | 398 | 85699c013bef828a340a0a5898f1d2a8fe15f7396d75fd89268c4d81fcfbdc4d | True |
| `dependencies/glm/glm/gtx/matrix_cross_product.hpp` | 1347 | c5556a345945603fe37154dbfb4e087987fccc8333512421da1a3ddf09f9b337 | True |
| `dependencies/glm/glm/gtx/matrix_cross_product.inl` | 718 | 06ba79f1135fbf5d66fba4179904df4a510c405f0c41431ace5b0d76102003d4 | True |
| `dependencies/glm/glm/gtx/matrix_decompose.hpp` | 1739 | acbc77276935d672809bc08aa51be6a2c55df6df09bcfbd9e75a383e68d71079 | True |
| `dependencies/glm/glm/gtx/matrix_decompose.inl` | 7903 | 3555fa1ce91cb62cbb4526a8cfdaa5f820d93830b8fe2f64967ac1d4ce15205a | True |
| `dependencies/glm/glm/gtx/matrix_factorisation.hpp` | 2947 | 3c616f1f712fc4865c79d530afc0e75d1f30b0e90feedccf3939c4072fd66f19 | True |
| `dependencies/glm/glm/gtx/matrix_factorisation.inl` | 2868 | 23715c719b138b302710e08f37355e35a8f9838ffca251c89b7fcb2bcd6e4d66 | True |
| `dependencies/glm/glm/gtx/matrix_interpolation.hpp` | 2041 | 3c522c8d8b89c7941e0133d90a5bcf6f4812233bdfff636da7d688dc94049ea5 | True |
| `dependencies/glm/glm/gtx/matrix_interpolation.inl` | 4457 | 973e2cf1232a17e778c3e6164bbc9aa7f2e4192ac6eca478de47acf5e60027c6 | True |
| `dependencies/glm/glm/gtx/matrix_major_storage.hpp` | 3856 | 7e364e1bf00b678ba97ef2edc7821d68b63e1b737de097ae3f60a59f19096dbe | True |
| `dependencies/glm/glm/gtx/matrix_major_storage.inl` | 3824 | 77225638a968bcef1e7586a3a615cb0aaa94ee1d9e691cb29001313d8575d22e | True |
| `dependencies/glm/glm/gtx/matrix_operation.hpp` | 3130 | 2351cffb5895d22e5bc3dd63fb926e43b2eed0e274cee49f40cfbda06c6b210f | True |
| `dependencies/glm/glm/gtx/matrix_operation.inl` | 5823 | 1de136720f3381058c8f7f41fbbe4f0b5baf0da69ec19d278710433540d89a5c | True |
| `dependencies/glm/glm/gtx/matrix_query.hpp` | 2796 | 8411d1a5afa033f870479a4701551bacc7834111bd56e3700cb19368d950f1bf | True |
| `dependencies/glm/glm/gtx/matrix_query.inl` | 3497 | 0a9b95484c088209d7daf1be296524cc4562d7222036fc258a1b3093c03bfe68 | True |
| `dependencies/glm/glm/gtx/matrix_transform_2d.hpp` | 2584 | 187560ddc197b08fc5078cfc21e16df3bcc5ea6db00e2b544e5b8f4990e0c109 | True |
| `dependencies/glm/glm/gtx/matrix_transform_2d.inl` | 1414 | 50c0565a770f1ebf81c0504f80e77850cbdc78a28dc82ac358872137406c28bf | True |
| `dependencies/glm/glm/gtx/mixed_product.hpp` | 1076 | c66320a7c31600053022637ea2e2ee1f8e3921d8beeb2f0a12ef3cba22cbe287 | True |
| `dependencies/glm/glm/gtx/mixed_product.inl` | 271 | d6b74359ecef2ae9cd19ba8a34faf27dd522a33c9cad5bcecddb6614c0015951 | True |
| `dependencies/glm/glm/gtx/norm.hpp` | 2724 | 7e0660877037c349847eaaed4a9984fe7d01dd5299f1c33a4478057ba7868a1c | True |
| `dependencies/glm/glm/gtx/norm.inl` | 2834 | 800c6a55c387e470e74eeeb1ec4eaa7aa46c9227fc9454baa914f44277513017 | True |
| `dependencies/glm/glm/gtx/normal.hpp` | 1066 | 787004d4175773d777a524ccbc254f8d6d63ec1ed29eee658b3ab22833868742 | True |
| `dependencies/glm/glm/gtx/normal.inl` | 289 | d17d6e052713b4ecdf1b7129dd5691ecb3243e9789b6b4b80f9b5406b2cdf3a0 | True |
| `dependencies/glm/glm/gtx/normalize_dot.hpp` | 1553 | 970aea4ff39b03a1c7722466f6d80adb6cdf6a2e9461870939f291a39989b08e | True |
| `dependencies/glm/glm/gtx/normalize_dot.inl` | 511 | badae260727e9176e4f624fd7e4716d6ff2c9a7d3ad463db86aa135e9545ec20 | True |
| `dependencies/glm/glm/gtx/number_precision.hpp` | 1543 | fb1e65e6042e84e72e55217460b2dd9dbad2f3dbdfbe39cbf2f053a7d3542d56 | True |
| `dependencies/glm/glm/gtx/optimum_pow.hpp` | 1302 | 7beabee46c33ae1922b15e2d913c4dc85d26e7841fc32391fe1fac02e0a7a38d | True |
| `dependencies/glm/glm/gtx/optimum_pow.inl` | 405 | 24e9df52744266f60fb1b850be276450af597dc62d62284f598c7d53a667899f | True |
| `dependencies/glm/glm/gtx/orthonormalize.hpp` | 1346 | 3009c895ea6c56d503465ba8132894a1816f1a3f3de636ac30115b8a06fca0b8 | True |
| `dependencies/glm/glm/gtx/orthonormalize.inl` | 636 | 0597119fefbb323ae82e4797298e7af9dc514600436d231eb7cd1eb510d7e0df | True |
| `dependencies/glm/glm/gtx/pca.hpp` | 5863 | 4f63838d8ceaa483f0562ab82543c01d79c88e50dbbd6c63109f7e0615e8a66d | True |
| `dependencies/glm/glm/gtx/pca.inl` | 8557 | 25f4438fd03a61ff119157ca4a26372b14e571ed6ef15c65e363c0a5de415912 | True |
| `dependencies/glm/glm/gtx/perpendicular.hpp` | 1123 | 17c2693ffb9238e2407c07af1571b9d3d52d0b5c2d71450c5affb0ca4d93374a | True |
| `dependencies/glm/glm/gtx/perpendicular.inl` | 212 | 115195d90827188d09925a1608889d634d58922839978fba6d66af42cd605e49 | True |
| `dependencies/glm/glm/gtx/polar_coordinates.hpp` | 1356 | 4a56db9bd4e6ddf008ee5ea07a7df64e8a22169886c3fb093c763fdc38fd94c3 | True |
| `dependencies/glm/glm/gtx/polar_coordinates.inl` | 773 | 34fcbdf27e27caf0d6b76bd66a6296443e85fba396c8b443f5ee70fbd64123dc | True |
| `dependencies/glm/glm/gtx/projection.hpp` | 1110 | e012b18348c77a59d681d9025bba9d2b2b9194cbda94fd5e4a7c20357eab24ab | True |
| `dependencies/glm/glm/gtx/projection.inl` | 245 | ed7608b9ea6b218f5e10cf379d6169169b12f9c5b1a6f37bd679c59363f6a034 | True |
| `dependencies/glm/glm/gtx/quaternion.hpp` | 4774 | a7a914bc351184d766cb821b3e8d5295939453d4c44fc924af5770d22d763b60 | True |
| `dependencies/glm/glm/gtx/quaternion.inl` | 4417 | 6d9655c893a990a31b6529983cccfa7696dc08606d2e34448ceed0e1516ca712 | True |
| `dependencies/glm/glm/gtx/range.hpp` | 2311 | c1b35ae8855c29a7a8b94ae92b0d6c576accde9234738381c426d761f9500266 | True |
| `dependencies/glm/glm/gtx/raw_data.hpp` | 1269 | b5ffe9a52fc8e5e7848d38c68787d01d2d78c6f968fa97b4815301af0d47fd2a | True |
| `dependencies/glm/glm/gtx/raw_data.inl` | 28 | 835ba49219f27f24849da440a3c10ca54e5ed30cdb112c95370518646a44637c | True |
| `dependencies/glm/glm/gtx/rotate_normalized_axis.hpp` | 2279 | 4301eb490af85e6ec3e7376f9e269a43ef87342e8b3d1abfb0f08eeae4819313 | True |
| `dependencies/glm/glm/gtx/rotate_normalized_axis.inl` | 1719 | b5955df01c4fe9ea7bf275edfd2e78e1dcf49a50411a3ebd707ea57077bcdbb2 | True |
| `dependencies/glm/glm/gtx/rotate_vector.hpp` | 3688 | 285a668f8c6d349c727e07ca76578783c1f09ec2bed18431c73f4107c99e3d3e | True |
| `dependencies/glm/glm/gtx/rotate_vector.inl` | 4023 | e4175c3985443ce1bd5034e05c7f740ba0b2c450b3b5ba246bbdddf8faf2b234 | True |
| `dependencies/glm/glm/gtx/scalar_multiplication.hpp` | 2483 | 08ef0279871de09c13b8dca131b442a483284714955b48b1829945a0a82a6562 | True |
| `dependencies/glm/glm/gtx/scalar_relational.hpp` | 922 | 90a1e3eecc2fd3d7da187033827de81e44a0b5d8bf1e0fb88eac7c5e4c97687f | True |
| `dependencies/glm/glm/gtx/scalar_relational.inl` | 1187 | e8f934703017a9224f1d2b477a3c9cb87aaedbb212d9570906032f74c7f6d229 | True |
| `dependencies/glm/glm/gtx/spline.hpp` | 1620 | 270afc994bad59961a0e6fedac2300efde95810a5083c4f67fd4263309750be5 | True |
| `dependencies/glm/glm/gtx/spline.inl` | 1864 | ce468401be713b6c732d3ab58ad5837ad4eba9398526481c5bcf8346f7e4bd8f | True |
| `dependencies/glm/glm/gtx/std_based_type.hpp` | 2056 | 9ef6192990d67cfea5a894fdbba9ae0dcf0b8ffe4f70ef1cdc2f8eaab80d8bc8 | True |
| `dependencies/glm/glm/gtx/std_based_type.inl` | 57 | e9132bf571a9a80d04e06ba1b92401b89177e439a6c306a694aab44125bf609c | True |
| `dependencies/glm/glm/gtx/string_cast.hpp` | 1264 | 4614291c958594c28f437866db2669a8dc62e09b49d7e7da8c6566b1fa79dd37 | True |
| `dependencies/glm/glm/gtx/string_cast.inl` | 18293 | 5e0d0c154bf6f4c3537e80668a09f962443656e681a560469e507e6eb31c268b | True |
| `dependencies/glm/glm/gtx/structured_bindings.hpp` | 3171 | ed9db45581daafbff7c5a899ee006dcc9e7c78f1081e8d01a865066390890a69 | True |
| `dependencies/glm/glm/gtx/structured_bindings.inl` | 1741 | 715f7995372042e97a7f4db66ca3a10a6c023a3e58cd6664a28c252a58fe6b02 | True |
| `dependencies/glm/glm/gtx/texture.hpp` | 1311 | 43b56639e3bc43456d5af6a3230d96d5e368d2d9c920691eb2ec6e5cb3f9c6e0 | True |
| `dependencies/glm/glm/gtx/texture.inl` | 335 | e00a18eacb9cb158de1cd84c2ff8a08e27afa7f03a13475f35ee06c5f7362ba4 | True |
| `dependencies/glm/glm/gtx/transform.hpp` | 1720 | eeba2399bd1a0a7c57aefe8be663b9e9a338fc3fa3ddb14b88dc4817c4f7c839 | True |
| `dependencies/glm/glm/gtx/transform.inl` | 595 | a92ca1ffb9cc998723289f19841d54a49bea0c71a04f875e4736223261be029b | True |
| `dependencies/glm/glm/gtx/transform2.hpp` | 3485 | 1e5cb9960ae28e2e5bfc5fd3c07c48b3b9d810542699143f5936fa270b44240d | True |
| `dependencies/glm/glm/gtx/transform2.inl` | 4007 | b2fe1bab72eac347f1d1fdf0eb3c675b0adeb1d84bf25e337a58316e671a9000 | True |
| `dependencies/glm/glm/gtx/type_aligned.hpp` | 34257 | e7127320c8d967a787c3298e0399ce65bfcdef09a21174b578b7afd5ecb82873 | True |
| `dependencies/glm/glm/gtx/type_aligned.inl` | 55 | 8a13d7cc7e03e83781ba18b8faa4c377a752e2d084b07a3717529ff975eb872d | True |
| `dependencies/glm/glm/gtx/type_trait.hpp` | 2196 | 93dabdba43fb44d3c622b77070c3cf58676cd611a89f1daee2ea683d3b5b14d8 | True |
| `dependencies/glm/glm/gtx/type_trait.inl` | 2103 | a6ddfc7bd958897acf7c64795be4cf126b563517a59030c858e204f298cfed81 | True |
| `dependencies/glm/glm/gtx/vec_swizzle.hpp` | 82457 | 5299aa5e5cb96eb52f0ce49fd572cdf91da86241d3e6a6ff8e5b64e07e1fb7c6 | True |
| `dependencies/glm/glm/gtx/vector_angle.hpp` | 1825 | abd1ec2d18a29f8c236e3979ab6bfd3dd4540cca6edece04a859ebd427296021 | True |
| `dependencies/glm/glm/gtx/vector_angle.inl` | 1538 | e75e5a6bb98cbbee1585ad66ded2bb87a38028697cb3808efade4fad25e5a2da | True |
| `dependencies/glm/glm/gtx/vector_query.hpp` | 2242 | f63e1a4a6b020b05979d265cc656ae949d2b84b01306b96bc568b1c7682e99a1 | True |
| `dependencies/glm/glm/gtx/vector_query.inl` | 4639 | 744768db68d23dedbc1ab33d4ca60764b276cfb5c30def3deaa2033fa789865a | True |
| `dependencies/glm/glm/gtx/wrap.hpp` | 875 | 63ceadb7338652f8db8c487e712e2b2143e5cb534d3274ac1099bb404ed7451d | True |
| `dependencies/glm/glm/gtx/wrap.inl` | 62 | 97cde43039625b06f13c4df01a828474b9cf5b6c2fe61c6d63f5e6fcbeaf86d8 | True |
| `dependencies/glm/glm/integer.hpp` | 10707 | fbb3ca5c518502bbba672ca1006a2e299262b5814e24572e50cb592086641594 | True |
| `dependencies/glm/glm/mat2x2.hpp` | 236 | ad69b7477122d91a9c5f5a4655c2e66ff348c8143c9c97511517e04e45020e44 | True |
| `dependencies/glm/glm/mat2x3.hpp` | 236 | 99f0c981ddac7432e3b7637a6d1d63b3345593a190feb670059c7dacbf2f8053 | True |
| `dependencies/glm/glm/mat2x4.hpp` | 236 | d9fe8b645b3f76186da7706e36750622b25dacc11aadc0171e00b7e8bd3e6d4a | True |
| `dependencies/glm/glm/mat3x2.hpp` | 236 | d55d1216d9701c51feb2d74ed62971d0000548eca60132812edaec6ed2aa589b | True |
| `dependencies/glm/glm/mat3x3.hpp` | 234 | 94d0e032b5458aeeea0233692b0181f820a4b498ca6c0d5ebe38276afb0be9bd | True |
| `dependencies/glm/glm/mat3x4.hpp` | 234 | 28f6826b1cded1ee9eacfa26037775ec377a4157c80534664a2c443a32181978 | True |
| `dependencies/glm/glm/mat4x2.hpp` | 236 | 4f041500e15c4690a19d266394d4585fc9962f99907269ad00cecbbee3598c04 | True |
| `dependencies/glm/glm/mat4x3.hpp` | 234 | 4681e9ebf357d4aca98394a8e056b58a26a575792054b7bf4d2694a1ec020c6f | True |
| `dependencies/glm/glm/mat4x4.hpp` | 236 | 367b21fe6b9052a4dcb3e7f146038fa547384755f58c1c5784a54243b4c014a2 | True |
| `dependencies/glm/glm/matrix.hpp` | 5812 | ca9f427d40a824528d3f4d320dab552c4c4a1034e0b8786f7eede43a34f86e72 | True |
| `dependencies/glm/glm/packing.hpp` | 11073 | a2dcdc7769b43f3d1d3e5c7b959beab1898797df83ffb750d85e20c4059b72a4 | True |
| `dependencies/glm/glm/simd/common.h` | 7356 | 1cfbea079bf62ac76cf7aeb2532a6162d388088807dbdf876ef64053b298929e | True |
| `dependencies/glm/glm/simd/exponential.h` | 400 | b04a68b1fcdf87200873db2593ab50681184820997949de8e75fd0daadbc1fe8 | True |
| `dependencies/glm/glm/simd/geometric.h` | 4435 | 9c740b6f17cfd2092171353381b17b7622c1d2121ee43395d6ed8a6b8954744d | True |
| `dependencies/glm/glm/simd/integer.h` | 3806 | 7361a721e51303b4f89379ad0c752ed90f85332eb2e1c75748abe461916779b6 | True |
| `dependencies/glm/glm/simd/matrix.h` | 41418 | 1a41850417c7daa54a0b7a283e0cab5508f6b3097e7b7936b6bc4be0738467e1 | True |
| `dependencies/glm/glm/simd/neon.h` | 5162 | 247d54d342596361f6e37f3b2b42594aa2a0616b9e02611478a1e04f58398b18 | True |
| `dependencies/glm/glm/simd/packing.h` | 140 | 99efb5a0b5c2ac5d34782de883dbb92d613f1f149fabfb3a3e32e703c2f016ae | True |
| `dependencies/glm/glm/simd/platform.h` | 16267 | f6e3a790cecb95c839aeb6e904d8bf4c865755c6976c2a2b71739e4c087b5593 | True |
| `dependencies/glm/glm/simd/trigonometric.h` | 148 | 1b9492b7c7fb284dcf8a86bf25fa379c84001c38a5893e3b37b7834c3d4b6b3e | True |
| `dependencies/glm/glm/simd/vector_relational.h` | 150 | 50298b14982ff78b6c1cb88d15c8549bf7ee11d2baf5a77d3787ac0deed3b001 | True |
| `dependencies/glm/glm/trigonometric.hpp` | 10890 | 15f7b9d814d244d0f1351329bc7cd3c853a03c95cb350197d9b203ecce463f71 | True |
| `dependencies/glm/glm/vec2.hpp` | 454 | 993c05b83e034d8bc451f25749d0d5a1ef003ed614cf7268693dbb43cdfef567 | True |
| `dependencies/glm/glm/vec3.hpp` | 454 | be9ca0adf100538cf4ff62cb4cbd2d8504cb410baf9f28e9ea748c57c461a14d | True |
| `dependencies/glm/glm/vec4.hpp` | 456 | 8ce1f9cd962932a8fefa61fa84a5972aebb9721cbd07e74106ab2349c4c83ba2 | True |
| `dependencies/glm/glm/vector_relational.hpp` | 6467 | 7faf05e55d5346c10189abc5af01a68b00c240824093742a7363f514360b880c | True |
| `detail/Application.inl` | 9338 | 11af563d7b62ce897ca083fb2435d0ba1286210582c040c51497074e855f06ef | True |
| `detail/AssimpLoader.inl` | 8943 | b4317b8fee7e9101e1774a8146cd4df8e397aca90284e9d969b01e7239281e24 | True |
| `detail/Motion.inl` | 14025 | 4d5a06b0d676c691a58d192891d467600e6e9d99d89c7483a457941f0a7a3131 | True |
| `detail/Native.h` | 13397 | e4dd267391bed263918735f6cb5f687a93c86921ea3755790aa5636fc7711877 | True |
| `detail/PrimitiveLoader.inl` | 5234 | 25ce148d7fd85b3ea7a95c3101b207f32da24eec0f0b0d6d702cf8a55f925cac | True |
| `detail/Renderer.inl` | 17077 | c4f09bbd2efb9a92c6443ddbf7be5e9b76d9a8a01be0af2f89b08c4ac833ccc4 | True |
| `detail/Scene.inl` | 30229 | 241c5341d78382c33b3765f17a6a93394fd00726a5d0c10ff13c80c8ce428cd8 | True |
| `detail/Shaders.h` | 1260 | c5c3efd43b1b581a0daaaf1fff75f8159518a601a9c4acd395e9453e0d2f2789 | True |
| `detail/Transform.inl` | 6334 | 37dd31736c43d0d709e464b2e3df5db46cf070ab426e0a801d8789c090c38980 | True |
| `docs/APPLICATION_PATTERNS.md` | 33895 | 74abdc93b163ca823707e5d87dfdd433542708b5fd626c0f21239f1d7cd9f6fb | True |
| `docs/USER_CHECKS.md` | 5118 | 45d1ac36ed6aa8b4be1ece4efe2b794d6c045f7c3510a42e4237da680ebec1b0 | True |
| `examples/assets.cpp` | 2243 | 47846e15cef104872a4715b6dae7b58c438303ef0ee588d5aa58d29b12c1974f | True |
| `examples/basic.cpp` | 1723 | 058714da90df2d75f157fc9b2f534049c84c6acfd247c679b67dcaa916fc7f0c | True |
| `examples/data/cube.txt` | 363 | c4e1eac287d7c3aa7ab6855e7f37bb56e42653460f525bd758ce3425e2e66a68 | True |
| `examples/motion.cpp` | 7001 | ac1884af0d3bef4800e1c9a0972c357b102e9451fc3dec4e6aba25618694c62c | True |
| `Framework.h` | 133 | a21aa6d5ebb3409d174d69b9883c0b76bf4645da22caa0baa591a4f8e81d8cd4 | True |
| `Framework.props` | 2328 | c3c52042014b3b394325a82cc82652311cd455c2f9021dd685d7bbce89a7a83b | True |
| `include/Application.h` | 3651 | d6124585e37e3eab5b6c0069dee0f30fd0ae299b4b1a332d5ba0649e61983326 | True |
| `include/Geometry.h` | 13547 | 71fb269d8aab0db531e7a71921e23cab3c7b9e12d4e3963f62762394aa68bfe1 | True |
| `include/Graphics.h` | 9956 | 8f4da33f8d634c6b366acdb6ff7cdc0690f8a49b770398b12a00b31aaddc06b1 | True |
| `include/Model.h` | 7537 | ae4602d68f58362c7ac8b800062643c8ef7095daa12d02694954f64f02b635fc | True |
| `include/ModelLoader.h` | 3929 | da8103ecd58ead65a7d70f2e369905eb6d6851cf8ffb87e5241193286bc79624 | True |
| `include/Motion.h` | 4135 | 67e7a6270c08d56b8fc765b230f7f626952ad87a05d346b4a1d87c4da6bbffdf | True |
| `include/Picking.h` | 5371 | aa00ca98c4ec95004e140f3c532853ad9daf1ab8386b2c01d39997340c4ee6ff | True |
| `include/Texture.h` | 3200 | 3466c4344ba2be9a88054f914fec18a8d14784cf6e0dba43d6de02169af83313 | True |
| `include/Transform.h` | 1883 | e54d0f3eefd00199ababfd2f655cfb7ad07bf02d4da334efab5acf7d4792f8b0 | True |
| `LegacyOpenGL.props` | 2558 | 3754770ef88326d316fb85565593d748d0666b8df8912dc34ae05180a8ffe3ff | True |
| `README.md` | 43923 | ed1d45b8f67bf46f136b9f99a35fee0ae7543e9b931d737a6d93075cfa5a0d09 | True |
| `shaders/FragmentShader.glsl` | 678 | 90b9ae6691877ca4cc74970b02139c1ba5429b827baec6d19756351a702c16db | True |
| `shaders/VertexShader.glsl` | 448 | 68a88c03a947db849448d51d2f165344203244edf47801c561b9430aab62750d | True |

프레임워크 사본은 574개 파일이다. 기존 실습·루트 프레임워크의 텍스트 343개를 작업 시작 시점 해시와 읽기 전용으로 대조했다. 차이 경로: []. 이 대조는 컴파일·실행 결과가 아니다.

## 경로·수명·출력의 핵심 조건

- 독립 실행 진입점은 standard/main.cpp 하나다. framework/examples의 main은 프로젝트에서 컴파일하지 않는다.
- Application이 살아 있는 동안 Console이 존재하며, run이 반환된 뒤 둘 다 정리된다. 샘플에는 지속 키 상태·별도 시간 갱신·과제 번호 종속 API가 없다.
- 소스는 폴더 내부 framework/Framework.h를 포함한다. Framework.props는 자기 위치에서 헤더·DLL을 찾고 실행 폴더에 필요한 파일을 배치한다.
- 프로젝트에 개인 PC 절대경로, vcxproj.user, 조기에 평가하는 LocalDebuggerWorkingDirectory=TargetDir 설정을 넣지 않았다.
- 새 작성 코드·설정·문서는 UTF-8 BOM, 문장별 줄바꿈이다. 공통 /utf-8·콘솔 출력 초기화와 Windows createWindow 매크로 충돌 회피를 그대로 사용한다.
- 새로운 기능의 구현·파일 배치·Git 커밋과 실제 실행 가능한 EXE의 사용자 빌드·배포 검증은 구분한다.

## 문서와 구조 산출물

- README.md: 시작 절차·도구 집합·폴더 책임·프레임 실행 순서·키·API 조립·다른 PC 공유.
- USER_CHECKS.md: Debug/Release x64 빌드, 화면·키·피킹·경로 재배치·다른 PC와 실행 폴더 공유를 사용자가 확인할 절차.
- graphify-out: 저장소의 독립 프로젝트/콜백·호출 관계와 소스 위치를 갱신한다. SDK·standard/framework 등 배포 사본은 중복 추출하지 않는다.
- framework-analysis/graphify-out/PROJECTS.json: 새 standard 프로젝트 색인만 맞춘다. 공통 소스 자체는 바뀌지 않아 프레임워크 전용 그래프를 재생성하지 않는다.
- AST의 미해결 참조·동적 콜백 한계는 GRAPH_HEALTH.md와 실행 흐름 문서를 함께 읽는다. AST 추출과 코드 읽기는 프로그램 동작 검증이 아니다.

현재 저장소 그래프: 코드 113개 파일, 3368노드·5348간선. 동적 콜백과 미해결 끝점은 정적 추출의 한계로 남긴다. 빌드·실행 결과를 추가로 얻은 것은 아니다.
