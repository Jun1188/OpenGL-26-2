# 과거 구현 참조: TankCommon.h

- 원래 위치: `standard/modes/TankCommon.h`
- 현재 역할: 변환·자료 흐름을 읽는 레퍼런스 문서. Standard의 컴파일 입력이 아니다.
- 아래는 전환 직전 소스 전체다. 당시 Console·PracticeMode·컨테이너 인터페이스에 종속되므로 현재 프레임워크와 그대로 컴파일하는 예제가 아니다.

[변환 조합 레퍼런스](../../TRANSFORM_REFERENCE.md) · [참조 목차](../../REFERENCE_PRACTICES.md)

````cpp
#pragma once
#include "PracticeMode.h"

std::unique_ptr<PracticeMode> makeTankMode(graphics::Application& app, bool threeViews);
````
