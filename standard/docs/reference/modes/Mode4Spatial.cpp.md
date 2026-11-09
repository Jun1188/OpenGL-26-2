# 과거 구현 참조: Mode4Spatial.cpp

- 원래 위치: `standard/modes/Mode4Spatial.cpp`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#include "Paths.h"

graphics::PathMotion standard_modes::path4() {
    graphics::PathMotion path;
    path.durationSeconds = 6;
    const glm::vec3 center(1.7f, 0, 0);
    path.path = [center](double u) {
        const double angle = u * 6.283185307179586;
        return center + glm::vec3(float(.7 * std::sin(2 * angle)), float(.6 * std::sin(angle)),
                                  float(.3 * std::cos(angle)));
    };
    path.interpolation = graphics::PathSystem::smoothStep;
    return path;
}
````
