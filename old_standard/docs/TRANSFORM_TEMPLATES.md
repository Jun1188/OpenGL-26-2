# 변환 조합 템플릿

기존 실습이 시험에 재등장한다는 가정은 사용하지 않는다. **요구한 움직임을 변환·좌표계·계층·시간 함수·힘으로 나누고 필요한 블록을 조합한다.** 실습 소스는 그 연산을 구현한 코드 위치를 찾을 때만 참고한다.

아래는 완성된 문제 풀이가 아닌 최소 동작 블록이다. 각 파일은 clear로 시작하므로 별도 장면에서 읽는다. 합칠 때는 clear·카메라 초기화를 제외하고 객체 이름을 구분한다. 입력은 실제 구현된 Standard 명령이며 실행 확인은 사용자가 수행한다.

| 필요한 연산 | 템플릿 | 바꿀 인수 |
|---|---|---|
| 이동한 축·회전한 축에서 다음 연산을 적용할 때 | [지역 변환의 순서](../examples/assembly/templates/01-local-order.assembly) | 좌표·배율·기간·대상 |
| 중심·끝점·관절·임의 기준점 주위로 돌 때 | [피벗을 고정한 회전](../examples/assembly/templates/02-pivot.assembly) | 축·중심·각속도 |
| 서로 다른 물체가 공통 이동을 따라갈 때 | [월드 좌표로 부착하고 상속](../examples/assembly/templates/03-inherit-world.assembly) | 축·중심·각속도 |
| 위치 변화와 자체 회전을 각각 조절할 때 | [공전과 자전의 독립 조합](../examples/assembly/templates/04-orbit-spin.assembly) | 축·중심·각속도 |
| 한쪽 끝은 그대로 두고 길이·폭만 바꿀 때 | [끝점을 고정한 비균등 배율](../examples/assembly/templates/05-fixed-end-scale.assembly) | 좌표·배율·기간·대상 |
| 단순 주기 운동을 물리 적분 없이 작성할 때 | [시간 함수로 반복 회전](../examples/assembly/templates/06-periodic.assembly) | 좌표·배율·기간·대상 |
| 한 번 누름과 길게 누름을 구분할 때 | [입력량과 시간당 변화량](../examples/assembly/templates/07-input-rate.assembly) | 좌표·배율·기간·대상 |
| 시각 객체와 관찰·광원 배치를 함께 조립할 때 | [같은 변환을 조명·카메라에 사용](../examples/assembly/templates/08-light-camera.assembly) | 좌표·배율·기간·대상 |
| 좌표축이 아닌 방향 벡터를 기준으로 돌 때 | [임의 축 회전](../examples/assembly/templates/09-arbitrary-axis.assembly) | 축·중심·각속도 |
| 관찰 장치가 이동하는 대상의 자세를 함께 따를 때 | [움직이는 기준과 부착 시점](../examples/assembly/templates/10-attached-view.assembly) | 축·중심·각속도 |
| 물체를 서로 다른 움직이는 기준 사이에서 인계할 때 | [실행 중 부착 대상 변경](../examples/assembly/templates/11-attachment-change.assembly) | 축·중심·각속도 |
| 광원·표지·위성의 중심 회전과 거리 조절을 나눌 때 | [공통 회전과 독립 반지름](../examples/assembly/templates/12-orbit-radius.assembly) | 축·중심·각속도 |
| 빛 위치·색과 표면 반사 특성을 독립적으로 바꿀 때 | [광원색과 재질 응답](../examples/assembly/templates/13-phong-material.assembly) | 좌표·배율·기간·대상 |

[HTML 템플릿 목차](TRANSFORM_TEMPLATES.html) · [연산 선택과 C++ 레퍼런스](TRANSFORM_REFERENCE.md) · [전체 명령](QUICK_COMMANDS.md)

저울은 중력·기준 프레임·회전·힘 콜백을 조합하는 [응용 예제](APPLICATION_RECIPES.md) 하나다. 템플릿을 저울 형상에 종속시키지 않는다.
