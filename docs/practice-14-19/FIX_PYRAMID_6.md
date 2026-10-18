# 후속 수정 3: 실습 16의 6번 바깥쪽 회전

대상: `openGL_3dAssign_16/Console.cpp`, `Console::applyPyramid()`의 `PyramidMotion::All` 분기.

Before:

```cpp
        if(pyramidMotion_==PyramidMotion::All)
            matrix=rotateAround(hinges[i],axes[i],-2*inward*pyramidProgress_);
```

After:

```cpp
        if(pyramidMotion_==PyramidMotion::All)
            matrix=rotateAround(hinges[i],axes[i],(360.0f-2*inward)*pyramidProgress_);
```

`inward = degrees(atan2(0.8, 0.4))`는 약 63.435°다. 기존 각도는 진행률 0→1에서 0→−126.870°였다. 각 면의 축 방향 `{+X, -Z, -X, +Z}`에서 이 음수 각도는 옆면을 처음에 안쪽으로 회전시킨다. 새 각도는 0→+233.130°다. 양수 방향으로 바깥쪽을 지나 아래로 회전한다. −126.870°에 360°를 더했으므로 완료 시점의 회전행렬과 아래 공통 꼭짓점에서 만나는 자세는 같다. 단순히 부호만 반전하면 완료 자세가 달라진다.

고정점은 `hinges[i]`, 축은 `axes[i]`로 유지한다. `T(hinge) × R(angle) × T(-hinge)`를 사용하므로 하단 모서리의 양 끝점은 고정된다. 진행률과 같은 키로 방향을 바꾸는 처리는 유지한다. 이동 시간은 기존 2초이며, 회전 경로가 길어져 각속도는 커진다.

함수·진입점·호출 관계는 변경하지 않았다. 기존 graphify 구조 자료를 재사용하며 이 작은 수식 변경에 그래프를 재생성하지 않는다.

## 사용자 확인

빌드와 테스트는 실행하지 않았다. 사용자가 Debug x64로 빌드한 뒤 다음을 확인한다.

1. 초기 상태에서 6을 누르면 네 옆면이 먼저 바깥쪽으로 벌어진다.
2. 각 하단 모서리는 고정되고 바닥 면은 움직이지 않는다.
3. 완료 시 네 면의 꼭짓점이 아래에서 만난다.
4. 진행 중 또는 완료 후 6을 다시 누르면 같은 경로로 반대 방향으로 움직인다.
5. 7~9의 동작을 확인한다. 해당 분기의 식은 변경하지 않았다.

README의 6번 설명과 실제 각도를 갱신하고 USER_CHECKS에 처음 벌어지는 방향의 관찰 기준을 추가했다. 기존 BEFORE_AFTER 문서는 최초 구현 당시의 기록으로 유지한다.