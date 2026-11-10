# debugger 시작부터 직접 편집까지

아래 행은 문서 생성 시 저장된 실제 소스에서 계산했다. debugger를 실행해 얻은 출력 기록은 아니다. 파일을 편집한 뒤에는 debugger의 현재 행 번호를 우선한다. debugger는 저장된 파일을 읽으며 IDE 미저장 버퍼를 읽지 않는다.

## 1. 실행

사용자가 standard/debugger/debugger.sln에서 x64로 빌드한 뒤 자기 폴더 경로로 실행한다.

```powershell
$studyStandard = "C:/Work/OpenGL-26-2/standard"
& "$studyStandard/debugger/x64/Debug/debugger.exe" "$studyStandard"
```

## 2. 물체 추가

`add object examPlate box`를 입력한다. debugger가 현재 삽입 위치와 **전체 ObjectSetup 선언·등록 after**를 제안한다. 그 코드를 IDE에 복사해 저장한다. 저장 전에는 show object examPlate가 새 구획을 찾을 수 없다.

현재 삽입 표식은 `standard/SceneSetup.cpp:82`다. 추가한 물체의 size/position/scale을 그 구획에서 직접 정한다. 기본 단위 박스의 왼쪽 피벗은 x=-.5다.

## 3. 현재 panel로 정확한 before/after 연습

### `object panel scale 2 1 0.5`

소스 스냅샷: `standard/SceneSetup.cpp:38`

```cpp
// before
    cfg_panel.scale = {1.f, 1.f, 1.f};
// after
    cfg_panel.scale = {2.f, 1.f, 0.5f};
```

### `layer hinge pivot -1 0 0`

소스 스냅샷: `standard/SceneSetup.cpp:54`

```cpp
// before
    cfg_hinge.pivot = {-1.f, 0.f, 0.f};
// after
    cfg_hinge.pivot = {-1.f, 0.f, 0.f};
```

### `layer hinge rotation 0 45 0`

소스 스냅샷: `standard/SceneSetup.cpp:52`

```cpp
// before
    cfg_hinge.rotationDegrees = {0.f, 0.f, 0.f};
// after
    cfg_hinge.rotationDegrees = {0.f, 45.f, 0.f};
```

### `light key position 3 4 1`

소스 스냅샷: `standard/SceneSetup.cpp:62`

```cpp
// before
    cfg_key.position = {2.f, 4.f, 3.f};
// after
    cfg_key.position = {3.f, 4.f, 1.f};
```


각 명령을 입력하고 절대 경로·실제 행·before를 IDE와 대조한다. after를 직접 저장한다. 이어 where input/update/uniform/shader로 상태 변화와 GLSL 위치를 찾는다. 재빌드·실행하여 비균등 배율, 피벗 고정, m 조명 토글, y/Y 광원 공전, z/Z 반경, c 색, F2 항별 출력을 확인한다. 런타임 상태는 ExamController가 소유하므로 새 입력은 그 컨트롤러를 수정한다.

## 4. 행 번호가 바뀐 뒤

구획 앞에 빈 줄을 넣어 저장한 뒤 같은 명령을 다시 입력한다. before와 행 번호가 현재 디스크를 따라야 한다. 안내만 읽었다고 코드가 저장되거나 실행 중 장면이 변경되는 것은 아니다. 임의 코드 수정은 code/replace/insert로 제안받되 의미 검토와 저장은 직접 한다.
