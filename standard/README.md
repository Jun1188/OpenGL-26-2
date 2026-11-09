# Standard: 0번 장면 작업공간

[변환 조합 템플릿](docs/TRANSFORM_TEMPLATES.html) · [변환 조합 레퍼런스](document.html?doc=docs/TRANSFORM_REFERENCE.md)

실습 재출제를 가정하지 않는다. 요구된 변환과 공간·순서·시간·힘을 조합하는 작업공간이다.

[전체 문서](index.html) · [명령어](document.html?doc=docs/QUICK_COMMANDS.md) · [변환·상속](document.html?doc=docs/TRANSFORM_WORKSPACE.md) · [응용](document.html?doc=docs/APPLICATION_RECIPES.md)

Standard는 0번 작업공간으로 시작한다. 실습 1~22의 번호 전환은 제거했다. 기존 모드와 보조 컨트롤러는 `docs/reference/modes/*.md`에 전체 소스를 담은 레퍼런스 문서로 전환했다. Standard 프로젝트에는 현재 작업공간 구현 파일만 명시한다. `main.cpp`에서 숫자 모드를 지정할 필요가 없다.

## 바로 작성

```text
add box
t 2 0 0
s 2 1 0.5
layer add hinge
layer r z 30
insert spin y 45
save scene.assembly
```

왼쪽 클릭으로 물체를 선택하고 Ctrl+클릭으로 그 부모 물체를 선택한다. 빈 공간 클릭은 작업 평면의 생성 기준점을 지정하고 선택을 해제한다. 표면 클릭은 교차점을 생성 기준점으로 사용한다. `add`는 새 물체를 자동 선택하며 이름 생략 시 `box1`처럼 생성한다. 좌표를 직접 지정할 때는 `add box item at 2 0 0`을 사용한다.

## 입력

- G/R/S → X/Y/Z → 마우스 좌우 이동 또는 방향키: 이동/회전/배율. Shift+방향키는 미세 조작이다.
- Enter 또는 클릭: 확정. Esc: 미리보기 취소. 포커스 이탈도 미리보기를 취소한다.
- `layer select 이름` 상태에서는 같은 단축키가 해당 지역 레이어에 작용한다. `layer select none`으로 물체 배치 편집에 복귀한다.
- 기본 선택은 물체 전체다. `edit part` 또는 `edit face`를 명시한 경우에만 부품·면을 선택한다. 면 모드에서는 t/r/s와 단축키가 선택 면에 작용한다. Esc는 물체 모드로 복귀한다.
- Space: 시간 정지/재개. F12: 명령 도움말. Ctrl+F5: 마지막 읽은 레시피 재읽기. `quit`: 종료.

## 경계

`framework`는 공통 C++ API, Standard는 명령·선택·편집을 맡는다. 문서와 과거 가이드 소스는 실행에 필요하지 않다. 콜백은 main에서 한 번 등록하고 Application이 입력 → 사용자 갱신 → Scene 모션 → 렌더 → swap을 실행한다.

`save`는 현재 작성 상태를 재구성하는 레시피를 저장한다. 선택 대상·레이어·커서 좌표를 명시한 명령으로 기록한다. 물리·모션의 현재 프레임 전체를 저장하는 기능은 아니다. 상대 경로는 **프로세스 작업 폴더** 기준이다. 실행 파일 옆에 쓰려면 그 폴더를 작업 폴더로 지정하거나 절대 경로를 쓴다.

소스 검토와 문서 생성만 수행했다. 빌드·테스트·화면 확인은 [사용자 확인 절차](USER_CHECKS.md)에 따라 사용자가 수행한다. 변경 텍스트는 UTF-8 BOM이다.
