# 과거 구현 참조: PracticeRegistry.cpp

- 원래 위치: `standard/modes/PracticeRegistry.cpp`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makePracticeMode(int mode, graphics::Application& app) {
    switch (mode) {
    case 0: return makeAssemblyMode(app);
    case 10: return makePractice13(app);
    case 11: return makePractice14(app);
    case 12: return makePractice15(app);
    case 13: return makePractice16(app);
    case 14: return makePractice17(app);
    case 15: return makePractice18(app);
    case 16: return makePractice19(app);
    case 17: return makePractice20(app);
    case 18: return makePractice21(app);
    case 19: return makePractice22(app);
    case 20: return makePractice23(app);
    case 21: return makeCameraCollision(app);
    case 22: return makeEmbeddedMesh(app);
    default: return {};
    }
}
````
