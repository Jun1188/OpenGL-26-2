# 공통 구현: standard 안에서만 사용하는 보조 코드

이 문서의 첫 블록 전문을 `standard/PracticeCommon.h`로 저장한다. C++17과 standard에 동봉된 framework만 사용한다. 다른 실습 폴더, 개인 경로, 외부 SDK, 추가 라이브러리를 참조하지 않는다.

## C00 공통 헤더 전문

```cpp
#pragma once
#include "framework/Framework.h"
#include <algorithm>
#include <array>
#include <cmath>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

namespace practice_common {
inline void assignVertexColors(graphics::Polyhedron& shape) {
    const std::array<graphics::GlColor, 8> palette{graphics::GlColor{1.0f, 0.2f, 0.2f},
                                                   {0.2f, 1.0f, 0.2f},
                                                   {0.2f, 0.4f, 1.0f},
                                                   {1.0f, 1.0f, 0.2f},
                                                   {1.0f, 0.2f, 1.0f},
                                                   {0.2f, 1.0f, 1.0f},
                                                   {1.0f, 0.6f, 0.2f},
                                                   {0.7f, 0.3f, 1.0f}};

    for (auto& face : shape.faces) {
        for (const auto& vertex : face.localVertices) {
            const int index = (vertex.x > 0.0f ? 1 : 0) | (vertex.y > 0.0f ? 2 : 0) | (vertex.z > 0.0f ? 4 : 0);

            face.vertexColors.push_back(palette[index]);
        }
    }
}

inline glm::mat4 rotateAround(const glm::vec3& pivot, const glm::vec3& axis, float degrees) {
    return glm::translate(glm::mat4(1.0f), pivot) * glm::rotate(glm::mat4(1.0f), glm::radians(degrees), axis) *
           glm::translate(glm::mat4(1.0f), -pivot);
}

inline glm::mat4 scaleAround(const glm::vec3& pivot, float scale) {
    return glm::translate(glm::mat4(1.0f), pivot) * glm::scale(glm::mat4(1.0f), glm::vec3(scale)) *
           glm::translate(glm::mat4(1.0f), -pivot);
}

inline glm::mat3 turn(float degrees, glm::vec3 axis) {
    return glm::mat3(glm::rotate(glm::mat4(1), glm::radians(degrees), axis));
}

inline void require(bool success, const char* operation) {
    if (!success)
        throw std::runtime_error(operation);
}

inline graphics::EntityId requireEntity(std::optional<graphics::EntityId> id, const char* reason) {
    if (!id)
        throw std::runtime_error(reason);

    return *id;
}
} // namespace practice_common
```

## 공통 패턴을 가져오는 위치

| 공통 처리 | 실제 코드 | 사용하는 곳 |
|---|---|---|
| 꼭짓점 색 일관성 | assignVertexColors | 14·15의 초기 모델 구성 |
| 점/선 중심 회전 | rotateAround | 15·16의 면·꼭짓점 회전 |
| 중심 고정 축소 | scaleAround | 15·16의 뒷면 축소 |
| 도 단위 회전 기저 | turn | 17·18·19 |
| 실패 전파 | require / requireEntity | 20·22·23 |

C++ 블록은 하나만 골라도 필요한 클래스 상태와 다른 함수가 있을 수 있다. 각 실습 문서의 `Console.h`, `Console.cpp` 보조부와 필요한 함수 블록을 함께 사용한다. 한 실습을 통째로 옮기려면 해당 문서의 블록을 순서대로 복사한다. 파일 내용은 모두 이 문서 묶음에 제공하며 원본 실습 파일을 include하지 않는다.

## 공통 실행 계약

- 초기화는 ID와 기준 상태를 만들고, 입력은 상태를 바꾸며, onUpdate는 dt초로 상태를 진행한 뒤 Scene에 반영한다.
- 토글은 GLFW_PRESS에서 처리한다. 지속 입력은 PRESS/RELEASE 상태와 포커스 해제를 처리한다.
- Shift와 Caps Lock은 XOR로 대문자를 판단한다.
- Scene.clear 뒤에는 모든 ID와 포인터를 다시 받는다.
- 본체 회전과 면 행렬을 분리하고 매 프레임 기준 상태에서 계산한다.
- 여러 뷰라도 상태·시간 갱신은 한 번이다. Application이 모션을 자동 진행하므로 추가 updateMotions를 호출하지 않는다.
- 기존 standard 데모 Console과 새 Console을 동시에 정의하지 않는다. 교체할 파일은 아래 3개뿐이다. PracticeCommon.h는 위 전문을 추가한다.

| 파일 | 넣을 코드 |
|---|---|
| standard/Console.h | 선택한 실습의 전체 클래스 상태·선언 블록 |
| standard/Console.cpp | 보조부 뒤에 모든 함수 블록을 순서대로 연결 |
| standard/main.cpp | 해당 실습의 콜백 연결 블록 |

기존 내용을 보존한 뒤 사용자가 교체한다. 이 작업은 실제 standard 런타임 파일을 교체하지 않았다. 빌드·실행·입력·화면 확인은 사용자 담당이다.

## 공통 요구사항 묶음

| ID | 공통 패턴 | 가져올 위치 |
|---|---|---|
| C01 | Q/Esc 종료와 정상 자원 정리 | 각 문서의 onKey와 main |
| C02 | 깊이·와이어·투영 옵션의 독립 설정 | 14/18/20의 onKey, 22의 onRender |
| C03 | 토글/반전/키 유지/키 해제의 구분 | 15의 beginMotion, 17/19의 onKey |
| C04 | Shift·Caps Lock 조합 | 14/17~23의 onKey |
| C05 | 포커스 이탈의 입력 초기화 | 14/17~22의 onFocus |
| C06 | 기준 상태와 현재 시간의 분리 | 15의 applyFaceMotion, 17의 begin/onUpdate |
| C07 | SRT·계층·표시 크기의 분리 | 17~19의 apply, 22의 reset/apply |
| C08 | 여러 뷰에서 한 번만 갱신 | 22의 onUpdate/onRender |
| C09 | 리셋 후 ID 재발급과 초기 상태 | 각 문서의 initScene/reset |
| C10 | 결과 확인과 실패 전파 | require/requireEntity 및 각 initScene |

위 공통 패턴은 실습별 세부 116개와 구분한다. 같은 공통 코드가 필요하다고 여러 실습의 전체 Console을 중복 정의하지 않는다.
