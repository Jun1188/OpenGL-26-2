# 원문·현재 소스·문서 코드의 범위

## 근거와 상태

과제 PDF의 세부 요구사항과 실습 13~23의 현재 Console.h/Console.cpp/main.cpp를 읽었다.
실습별 MD는 현재 함수 본문을 제공한다. 공유 보조 함수만 COMMON.md로 옮겼으며 GLFW/GLM의 별도 include 대신 동봉 Framework를 사용한다.
외부 실습 소스 include, 개인 절대 경로, 외부 DLL 탐색, 새 패키지 설치는 코드 블록에 없다.
기존 standard 데모는 그대로다. 사용자가 문서 블록을 선택해 같은 standard 폴더에 구성해야 한다.

## 구분해야 할 요구사항

| 항목 | 원문/기존 설명 | 현재 소스와 이 문서의 처리 |
|---|---|---|
| 실습 13~14의 기울기 | X/Y 약 30도 | 실습 13은 모델 회전과 카메라, 14는 표시용 카메라를 사용한다. 같은 회전 구현이라고 묶지 않는다. |
| 실습 15 면 2 교환 | 반대 방향 90도 공전 | 현재 소스는 0→90→0으로 교환·복귀를 반복한다. 일회 교환과 구분한다. |
| 실습 16 전면 열기/평면 펼치기 | 문서상 180도/90도 | 현재 좌표에서 atan2(height,halfWidth)로 실제 힌지 각도를 구한다. 전면 개방 약 233.13도, 평면 펼침 약 116.565도다. 숫자 180/90을 그대로 회전행렬에 넣지 않는다. |
| 실습 18 달 초기 궤도 | 화면과 평행한 경로 | 현재 달 위치는 로컬 XZ에서 계산된다. PDF의 화면 평행과 동일하다고 단정하지 않는다. 아래 대체 블록으로 카메라 기준 프레임을 구성할 수 있다. |
| 실습 21의 E/V | PDF에 없는 키 | 현재 소스의 전체 이탈/방 Y자전 확장이다. 원문 요구와 별도 항목으로 수록한다. |
| 실습 23 벽 개방 | 앞면 위 슬라이드 설명과 O의 스케일 설명이 함께 있음 | 기존 사용자 결정인 카메라 방향 벽 선택·중심 기준 균등 축소·같은 벽 복원을 기준으로 한다. 이전 사본 동기화에서 잃은 균등 축소는 ea1ebb7에서 복원했다. |
| 실습 23 보행 정지 | 현재 Space 확장 | 수평 속도를 0으로 계산하며 점프/중력은 계속 계산한다. 완전한 시간 정지와 다르다. |
| 방·로봇 충돌 | 공간 내부 반사·장애물 착지 | 축 정렬 대리 상자 기반이다. 메시별 정밀 충돌, 마찰, 상호 강체 충돌을 구현한 것으로 설명하지 않는다. |

## 화면과 평행한 궤도 프레임 대체 블록

이 함수는 원하는 궤도 프레임을 카메라 평면과 평행하게 만든다. 같은 부모 아래의 궤도 선과 달 위치가 같은 프레임을 사용해야 한다.
월드 중심을 인자로 받으며 함수 내부에서 다른 파일이나 전역 상태를 읽지 않는다. 부모는 양수 단위 회전/이동 프레임이어야 한다.
프레임 자체에 Motion 레이어가 연결되어 있으면 그 기여를 이 함수가 덮어쓸 수 없으므로 거부한다.

`Console.cpp`의 함수들보다 앞에 추가할 수 있다. 필요한 헤더는 COMMON.md에 제공했다.

```cpp
bool orientScreenParallelOrbitFrame(graphics::Scene& scene, graphics::EntityId frame,
                                     const graphics::Camera& camera, const glm::vec3& centerWorld) {
    const auto* component = scene.getComponent(frame);
    if (!component || !graphics::CameraSystem::validCamera(camera) || scene.getEntityLayer(frame)) return false;
    glm::mat4 parentWorld(1);
    if (component->parent) {
        const auto parent = scene.worldMatrix(*component->parent);
        if (!parent) return false;
        parentWorld = *parent;
    }
    const auto parentBasis = glm::mat3(parentWorld);
    const auto gram = glm::transpose(parentBasis)*parentBasis;
    for (int c=0; c<3; ++c)
        for (int r=0; r<3; ++r)
            if (std::abs(gram[c][r] - (c == r ? 1.0f : 0.0f)) > 1e-4f) return false;
    if (glm::determinant(parentBasis) <= 0) return false;
    const auto forward = glm::normalize(camera.target-camera.position);
    const auto right = glm::normalize(glm::cross(forward,camera.up));
    const auto screenUp = glm::cross(right,forward);
    auto transform = component->transform;
    transform.basis = glm::vec3(glm::inverse(parentWorld)*glm::vec4(centerWorld,1));
    transform.rotation = glm::transpose(parentBasis)*glm::mat3(right,forward,screenUp);
    transform.scale = glm::vec3(1);
    transform.localBasis = glm::vec3(0);
    return scene.setTransform(frame,transform);
}
```

사용 시 기존 apply 함수의 달 프레임 설정을 위 함수로 대체한다. 달 좌표와 선은 계속 로컬 XZ를 사용한다.
프레임 이름만 바꾸거나 카메라 기울기를 추측해서 상수를 넣는 방법과 구분한다. 이 대체 함수도 빌드·화면 확인은 사용자 담당이다.

## 원문 위치

- [실습 13~15 PDF](<../../../lecture_pdf/homework/지엘3 좌표계 변환 (실습 131415).pdf>) 2~4쪽.
- [실습 16~17 PDF](<../../../lecture_pdf/homework/지엘3 좌표계 변환 (실습 1617).pdf>) 2~3쪽.
- [실습 18~19 PDF](<../../../lecture_pdf/homework/지엘3 좌표계 변환 (실습 1819).pdf>) 2~4쪽.
- [실습 20 PDF](<../../../lecture_pdf/homework/지엘3 좌표계 변환 2 (실습 20).pdf>) 1~2쪽.
- [실습 21~22 PDF](<../../../lecture_pdf/homework/지엘3 좌표계 변환 (실습 2122).pdf>) 2~3쪽.
- [실습 23 PDF](<../../../lecture_pdf/homework/지엘3 좌표계 변환 (실습 23).pdf>) 2쪽.

## 사용자 확인

블록을 해당 파일에 구성한 뒤 빌드한다. 모든 키와 재입력, 방향 반전/정지 구분, 리셋, 포커스 이탈, 세 뷰포트의 시간 일치를 확인한다.
개인 PC 경로가 없는 다른 standard 사본에서도 동봉 framework로 빌드되는지 확인한다. 새 SDK나 패키지를 요구하면 구성과 include 경로를 확인한다.
문서 작성 중에는 빌드·테스트·검증기·앱 실행을 하지 않았다. 정적 소스 추출과 실제 동작 확인을 구분한다.
