# 과거 구현 참조: PracticeMode.h

- 원래 위치: `standard/modes/PracticeMode.h`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#pragma once
#include "../framework/Framework.h"
#include <memory>

// Each practice owns its state; Console alone owns callback registration and switching.
class PracticeMode {
    graphics::Application& host_;
public:
    explicit PracticeMode(graphics::Application& app) : host_(app) {}
    virtual ~PracticeMode() = default;
    virtual bool initScene() = 0;
    virtual void onKey(int, int, int) = 0;
    virtual void onUpdate(float) {}
    virtual void onMouse(const graphics::MouseEvent&) {}
    virtual void onCursor(double, double) {}
    virtual void onFocus(bool) {}
    virtual bool capturesNumericKeys() const { return false; }
    virtual bool onRender() { return host_.renderScene(); }
};

constexpr int lastStandardMode = 22;
std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice13(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice14(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice15(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice16(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice17(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice18(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice19(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice20(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice21(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice22(graphics::Application& app);
std::unique_ptr<PracticeMode> makePractice23(graphics::Application& app);
std::unique_ptr<PracticeMode> makeCameraCollision(graphics::Application& app);
std::unique_ptr<PracticeMode> makeEmbeddedMesh(graphics::Application& app);

std::unique_ptr<PracticeMode> makeAssemblyMode(graphics::Application& app);
````
