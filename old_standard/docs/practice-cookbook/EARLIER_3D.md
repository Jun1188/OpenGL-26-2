# 이전 3D 작업 부록

[목차](README.md) · [공통 헤더 전문](COMMON.md)

번호가 붙은 실습 1~12는 현재 소스에서 2D Point/XY 이동/평면 Z회전을 사용하므로 제외했다.
번호 없는 openGL_sample0은 3D 메시·카메라·구-AABB 충돌을 다룬다. 워밍업 2의 6번은 3D 정점/UV/삼각형 검증을 다루므로 그 부분만 포함한다.
워밍업의 2D 보드와 CLI 메뉴·보고서 파일 저장은 3D 구현 패턴에서 제외한다.

| ID | 패턴 | 아래 코드 |
|---|---|---|
| E00-01 | 3D 물체 경계와 카메라의 구-AABB 충돌 | A의 blocked |
| E00-02 | WASD 이동 방향 정규화와 dt 적용 | A의 onUpdate |
| E00-03 | 작은 이동 구간으로 분할해 관통 완화 | A의 steps/step 루프 |
| E00-04 | 포커스 이탈 때 키 상태 정리 | A의 onFocus |
| E06-01 | 정점 [-1,1], UV [0,1]와 유한성 검사 | B의 validateEmbedded |
| E06-02 | 삼각형 코너의 정점·UV 인덱스 확인 | B의 face 루프 |
| E06-03 | 한 면 안의 중복 인덱스·좌표·UV 거부 | B의 코너 쌍 비교 |
| E06-04 | 아주 작은 삼각형에서도 안정적인 일직선 판정 | B의 스케일 정규화 후 cross 검사 |
| E06-05 | 면 사이 데이터 재사용과 정상 면/오류 분리 | B의 EmbeddedResult |
| E06-06 | 검사한 3D 삼각형을 Scene에 등록 | B의 Console::initScene |

A와 B는 별개의 선택 예제다. 같은 이름의 Console을 동시에 넣지 않는다. 모두 같은 standard 폴더의 PracticeCommon.h와 동봉 framework만 사용한다.
모델·셰이더·텍스처 파일을 외부 경로에서 읽지 않는다. 메시 데이터는 B의 코드 안에 들어 있다.

## A: 카메라 이동과 3D 충돌 — Console.h 전문

원본의 구와 AABB 사이 최근접점 계산을 유지하고, GLFW 직접 폴링은 Application의 키/포커스 핸들러로 옮겼다.
예제 물체는 동봉 생성 함수로 만들며 경계도 같은 크기에서 정한다.

```cpp
#pragma once
#include "PracticeCommon.h"

class Console {
    graphics::Application& app_;
    std::array<bool, GLFW_KEY_LAST+1> pressed_{};
    glm::vec3 minimum_{-.6f}, maximum_{.6f};
    bool blocked(const glm::vec3& position) const {
        const auto closest = glm::clamp(position,minimum_,maximum_);
        const auto difference = position-closest;
        return glm::dot(difference,difference) <= .15f*.15f;
    }
public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene() {
        pressed_.fill(false);
        auto& scene = app_.getScene(); scene.clear();
        const auto object = scene.addCuboid({1.2f,1.2f,1.2f},{0,0,0});
        if (!object || !scene.setTransform(*object,graphics::Transform3D{})) return false;
        app_.getCamera() = graphics::Camera{};
        app_.getCamera().position = {0,0,3};
        app_.getCamera().target = {0,0,0};
        return true;
    }
    void onKey(int key,int action) {
        if (key>=0 && key<=GLFW_KEY_LAST) pressed_[key] = action != GLFW_RELEASE;
        if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) app_.requestClose();
    }
    void onFocus(bool focused) { if (!focused) pressed_.fill(false); }
    void onUpdate(float dt) {
        if (!std::isfinite(dt) || dt<0) throw std::runtime_error("Invalid dt.");
        auto& camera = app_.getCamera();
        const auto forward = glm::normalize(camera.target-camera.position);
        const auto right = glm::normalize(glm::cross(forward,camera.up));
        glm::vec3 direction(0);
        if (pressed_[GLFW_KEY_W]) direction += forward;
        if (pressed_[GLFW_KEY_S]) direction -= forward;
        if (pressed_[GLFW_KEY_A]) direction -= right;
        if (pressed_[GLFW_KEY_D]) direction += right;
        if (glm::dot(direction,direction)==0) return;
        const auto movement = glm::normalize(direction)*2.0f*dt;
        const int steps = std::max(1,int(std::ceil(glm::length(movement)/(.15f*.5f))));
        const auto step = movement/float(steps);
        for (int i=0; i<steps; ++i) {
            if (blocked(camera.position+step)) break;
            camera.position += step;
            camera.target += step;
        }
    }
};
```

A의 Console.cpp 전문은 아래 한 줄이다. 모든 메서드가 클래스 안에 있으므로 기존 다른 Console 메서드를 남겨 두지 않는다.

```cpp
#include "Console.h"
```

A의 main.cpp 전문:

```cpp
#include "Console.h"
int main() {
    graphics::Application app;
    Console pattern(app);
    app.setInitHandler([&](graphics::Application&) { return pattern.initScene(); });
    app.setKeyHandler([&](graphics::Application&,int key,int action) { pattern.onKey(key,action); });
    app.setFocusHandler([&](graphics::Application&,bool focused) { pattern.onFocus(focused); });
    app.setUpdateHandler([&](graphics::Application&,float dt) { pattern.onUpdate(dt); });
    return app.run();
}
```

물체 회전·크기 변경까지 추가하면 원래 정점의 여덟 경계 꼭짓점을 월드 변환해 AABB를 갱신해야 한다. 위 최소 예제는 고정 육면체다.
사용자가 W/A/S/D와 모서리 접근, 대각 이동 속도, 포커스 이탈을 확인한다. 서브스텝은 연속 충돌 검출을 완전히 대체하지 않는다.

## B: 3D 데이터 검증과 삼각형 생성 — Console.h 전문

외부 파일 파서에 의존하지 않도록 입력을 타입이 정해진 배열로 받는다. 워밍업의 문자열 문법/CLI 전체 복사본은 아니다.
내부 인덱스는 0부터다. 모든 오류를 모으고 정상 면은 진단용 model에 남기지만 오류가 있으면 장면 등록은 거부한다.
한 면 안의 중복은 거부하며 면 사이에서 같은 좌표/UV를 다시 사용하는 것은 허용한다. UV는 이미지 파일 없이 데이터로만 보관한다.

```cpp
#pragma once
#include "PracticeCommon.h"

struct EmbeddedCorner {
    std::size_t vertex = 0;
    std::optional<std::size_t> uv;
};
using EmbeddedTriangle = std::array<EmbeddedCorner,3>;
struct EmbeddedResult {
    graphics::Model model;
    std::vector<std::string> errors;
};

inline EmbeddedResult validateEmbedded(const std::vector<glm::vec3>& positions,
        const std::vector<glm::vec2>& uvs,const std::vector<EmbeddedTriangle>& triangles) {
    EmbeddedResult result;
    result.model.shape = graphics::GLShape::Custom;
    const auto validPosition = [](const glm::vec3& p) {
        return std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z)
            && glm::all(glm::lessThanEqual(glm::abs(p),glm::vec3(1)));
    };
    const auto validUV = [](const glm::vec2& uv) {
        return std::isfinite(uv.x) && std::isfinite(uv.y)
            && glm::all(glm::greaterThanEqual(uv,glm::vec2(0)))
            && glm::all(glm::lessThanEqual(uv,glm::vec2(1)));
    };
    for (std::size_t i=0; i<positions.size(); ++i)
        if (!validPosition(positions[i])) result.errors.push_back("Invalid vertex "+std::to_string(i));
    for (std::size_t i=0; i<uvs.size(); ++i)
        if (!validUV(uvs[i])) result.errors.push_back("Invalid UV "+std::to_string(i));
    for (std::size_t face=0; face<triangles.size(); ++face) {
        const auto& corners = triangles[face];
        const bool textured = corners[0].uv.has_value();
        bool valid = true;
        std::array<glm::vec3,3> points{};
        std::array<glm::vec2,3> texcoords{};
        for (std::size_t i=0; i<3; ++i) {
            if (corners[i].vertex>=positions.size() || corners[i].uv.has_value()!=textured) { valid=false; break; }
            points[i] = positions[corners[i].vertex];
            if (!validPosition(points[i])) { valid=false; break; }
            if (textured) {
                if (*corners[i].uv>=uvs.size()) { valid=false; break; }
                texcoords[i] = uvs[*corners[i].uv];
                if (!validUV(texcoords[i])) { valid=false; break; }
            }
        }
        if (valid)
            for (std::size_t i=0; i<3; ++i)
                for (std::size_t j=i+1; j<3; ++j)
                    if (corners[i].vertex==corners[j].vertex || glm::all(glm::equal(points[i],points[j]))
                        || (textured && glm::all(glm::equal(texcoords[i],texcoords[j])))) valid=false;
        glm::dvec3 normal(0);
        if (valid) {
            auto a=glm::dvec3(points[1])-glm::dvec3(points[0]);
            auto b=glm::dvec3(points[2])-glm::dvec3(points[0]);
            const auto scaleA=std::max({std::abs(a.x),std::abs(a.y),std::abs(a.z)});
            const auto scaleB=std::max({std::abs(b.x),std::abs(b.y),std::abs(b.z)});
            if (scaleA==0 || scaleB==0) valid=false;
            else {
                a/=scaleA; b/=scaleB; normal=glm::cross(a,b);
                if (glm::dot(normal,normal)<=1e-12*glm::dot(a,a)*glm::dot(b,b)) valid=false;
            }
        }
        if (!valid) { result.errors.push_back("Invalid triangle "+std::to_string(face)); continue; }
        std::vector<graphics::Vertex> vertices(3);
        for (std::size_t i=0; i<3; ++i) {
            vertices[i].position=points[i];
            vertices[i].normal=glm::vec3(glm::normalize(normal));
            vertices[i].uv=texcoords[i];
            vertices[i].color={.8f,.6f,.2f};
        }
        result.model.faces.push_back(graphics::Mesh::fromVertices(vertices,{0,1,2}));
    }
    if (result.model.faces.empty()) result.errors.push_back("No valid triangles.");
    return result;
}

class Console {
    graphics::Application& app_;
public:
    explicit Console(graphics::Application& app) : app_(app) {}
    bool initScene() {
        const std::vector<glm::vec3> positions{
            {-.5f,-.5f,.5f},{.5f,-.5f,.5f},{0,.5f,0},{-.5f,-.5f,-.5f},{.5f,-.5f,-.5f}};
        const std::vector<glm::vec2> uvs{{0,0},{1,0},{.5f,1}};
        const auto face=[](std::size_t a,std::size_t b,std::size_t c) {
            return EmbeddedTriangle{EmbeddedCorner{a,0},EmbeddedCorner{b,1},EmbeddedCorner{c,2}};
        };
        const auto result=validateEmbedded(positions,uvs,
            {face(0,1,2),face(1,4,2),face(4,3,2),face(3,0,2),face(0,3,4),face(0,4,1)});
        for (const auto& error : result.errors) std::cerr << error << '\n';
        if (!result.errors.empty()) return false;
        auto& scene=app_.getScene(); scene.clear();
        const auto model=scene.addModel(result.model,graphics::Transform3D{},graphics::PivotMode::Preserve);
        app_.getCamera()=graphics::Camera{};
        app_.getCamera().position={2,1.5f,3};
        return model.has_value();
    }
    void onKey(int key,int action) {
        if (key==GLFW_KEY_ESCAPE && action==GLFW_PRESS) app_.requestClose();
    }
};
```

B의 Console.cpp 전문:

```cpp
#include "Console.h"
```

B의 main.cpp 전문:

```cpp
#include "Console.h"
int main() {
    graphics::Application app;
    Console pattern(app);
    app.setInitHandler([&](graphics::Application&) { return pattern.initScene(); });
    app.setKeyHandler([&](graphics::Application&,int key,int action) { pattern.onKey(key,action); });
    return app.run();
}
```

원본 워밍업의 double 정밀도/좌표 비교 허용오차와 모든 오류 문구까지 복제한 것은 아니다. 위 응용 예는 standard의 float 정점과 명시한 판정 기준을 쓴다.
사용자가 정상 메시, 범위 밖 인덱스, 중복 좌표/UV, 일직선 삼각형을 넣고 등록 거절과 오류 목록을 확인한다.
이 문서의 코드도 빌드·실행·테스트하지 않았다. 외부 파일을 읽거나 다른 실습 헤더를 참조하지 않는다.

## 근거

- 번호 없는 3D 샘플의 카메라 이동/충돌: openGL_sample0/main.cpp.
- 워밍업의 3D 형식/검증: Computer-Graphics_WarmmingUp2/README.md 및 MeshValidator.cpp.
- 이 경로들은 근거 표시이며 위 C++ 블록의 include나 런타임 파일 경로가 아니다.
