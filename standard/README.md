# standard: Motion·격자·방·그네·관절 SRT 샘플

현재 기준: 2026-11-02. Windows x64, C++17, OpenGL 3.3 Core를 사용한다. 빌드·실행은 사용자가 수행한다.

## 문서 선택

- [모든 문서 목차](DOCUMENT_INDEX.md)
- [공통 구현과 전체 API 실제 사례](docs/API_REFERENCE.html)
- [응용패턴·그네·패턴별 흐름도·실제 코드](docs/IMPLEMENTATION_PATTERNS.html)
- [시험 현장 구현 순서](docs/EXAM_IMPLEMENTATION_ORDER.html)
- [직접 실행 확인](USER_CHECKS.md), [독립 배포](PORTABLE.md), [변경 전후](BEFORE_AFTER.md), [폴더 용량](SIZE_REPORT.md)

## 시작과 조작

standard.sln 또는 지원되는 VS의 standard.slnx를 연다. x64 Debug/Release를 선택한다. 프로젝트는 framework/Toolchain.props와 Framework.props를 포함하며 특정 v145를 강제하지 않는다. DLL·라이선스를 동봉한 폴더 구조를 유지한다.

| 입력 | 실제 동작 |
|---|---|
| 0 / 7 / 8 / 9 | Motion / 격자·거리 / 회전 방 / 그네·사람형·선형보간 |
| 1~6 (Motion) | 타원·사각형·다각형·사용자 3D 경로·원·왕복 꺾은선 |
| Space | 현재 모드의 시간 정지·재개 |
| S / G / M / I (Motion) | 자전 기여 / 전체 공전 기여 / 반사 종류 / 레이어 삽입·삭제 |
| V / D / F | 3뷰 / 단계 로그 / 방 모드 바닥 개방 |
| Tab / Backspace (9번) | 다음 / 이전 관절·프레임 선택 |
| X / Y / Z (9번) | 편집 축 선택 |
| ← / → (9번) | 선택 관절 로컬 회전 -5° / +5° |
| ↑ / ↓ (9번) | 부모 좌표 basis 이동 +.05 / -.05 |
| PageUp / PageDown (9번) | 모델 좌표 localBasis 피벗 +.05 / -.05 |
| + 또는 = / - (9번) | 양수 균일 배율 확대 / 축소 (.1~3) |
| R (9번) | 선택 관절의 초기 기준 SRT·피벗 복원 |
| 9 재입력 | 장면·트랙·모든 편집값 전체 재생성 |
| 왼쪽 클릭 / H / Esc | 객체·면 선택 / 도움말 / 종료 |

## 상태와 실행 흐름

main → Application/Console 생성 → 콜백 저장 → run의 초기화 → initScene.
프레임은 PollEvents → onUpdate(dt) → Scene.updateMotions(dt) → onRender → SwapBuffers다. onUpdate는 7/8/9 모드의 시계·물리·관절 상태를 갱신하고, 공통 MotionSystem은 Application이 자동 진행한다. 렌더 패스에서 시간을 다시 진행하지 않는다.

9번의 사람은 좌석에 붙어 앉은 자세다. 관절 편집값은 animation_의 rest에 저장되므로 재생 중에도 덮어써지지 않는다. 회전은 로컬 축, 이동은 부모 공간, 피벗은 모델 공간이다. 배율은 .1~3 범위의 균일 배율이다. 비균일 배율과 shear를 지원한다고 가정하지 않는다.

기본 실행은 외부 모델·텍스처를 요구하지 않는다. 문서의 별도 자산 예제는 사용자가 파일을 준비한다. 제3자 SDK·라이선스는 원본을 유지한다. 새로 수정한 소스·문서는 UTF-8 BOM이다.

## 3D 실습 세부 구현 코드 모음

[실습 13~23의 세부 116개·공통 구현·이전 3D 부록](docs/practice-cookbook/README.md)을 읽는다. 같은 standard 폴더에 구성할 코드 전문을 제공하며 다른 실습 폴더나 개인 경로를 참조하지 않는다. 실제 Console/main은 자동 교체하지 않았다.
