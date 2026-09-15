# 컴퓨터 그래픽스 워밍업 5·6

## 실행

Visual Studio에서 `Computer-Graphics_WarmmingUp2.slnx`를 열고 Debug/x64로 빌드합니다. C++20, 경고 수준 4를 사용합니다.

```powershell
.\x64\Debug\Computer-Graphics_WarmmingUp2.exe --board
.\x64\Debug\Computer-Graphics_WarmmingUp2.exe --parse data/pyramid.txt
```

인수 없이 실행하면 5 / 6 / q 메뉴가 나옵니다. 6번은 입력 경로 하나만 받고 결과·오류를 콘솔에 표시하는 동시에 `outout-6`에 저장합니다. 경로는 현재 작업 폴더 기준이며, 메뉴 입력은 따옴표 없이 입력합니다.

## 5번 규칙

입력은 1~30 좌표 네 개이며 양 끝 칸을 포함합니다. 도형1은 0, 도형2는 x, 겹침은 빨간 #입니다.

| 명령 | 동작 |
|---|---|
| 1x/2x, 1X/2X | 도형별 오른쪽/왼쪽 이동 |
| 1y/2y, 1Y/2Y | 도형별 아래/위 이동 |
| 1s/2s, 1S/2S | 너비·높이 동시 축소/확대 |
| 1a/2a, 1A/2A | 너비 확대·높이 축소 / 너비 축소·높이 확대 |
| 1i/2i, 1I/2I | 너비 확대/축소 |
| 1j/2j, 1J/2J | 높이 확대/축소 |
| 1b/2b | 면적 출력, 다음 명령 바로 입력 |
| c/d | 보드 확대/축소, 10~40 제한 |
| r/q | 좌표 재입력/종료 |

번호를 생략하면 도형 1입니다. **s/S, a/A는 한 축이라도 실패하면 전체 거부합니다.** 따라서 폭이나 높이가 1인 도형에 `s`를 입력해도 어느 축도 줄어들지 않습니다. 실패 사유는 보드 아래에 출력합니다.

실제 위치·크기와 순환 출력을 분리합니다. 이동으로 경계에 걸친 축은 음수 논리 위치를 유지하며 축소할 수 있지만, 이미 양쪽 경계에 닿았으므로 그 축을 포함하는 확대 명령은 전체 거부됩니다. 일반 위치의 확대만으로 새 순환을 시작할 수도 없습니다. 보드 축소 시 범위 밖 또는 음수 논리 위치의 도형이 있으면 재배치 없이 거부합니다.

## 6번 규칙

- v: 정점 3개 성분, [-1,1]
- vt: UV 2개 성분, [0,1]
- f: 세 개의 v 또는 세 개의 v/vt
- # 주석, 공백/탭, UTF-8 BOM 지원
- 한 면 내부의 정점·UV 좌표쌍 중복, 잘못된 인덱스, 거의 일직선인 정점 거부
- 면 사이의 좌표 재사용 및 중복 선언 자체는 허용
- 텍스처 미지정 면은 `texture not specified`
- 모든 오류 면을 확인하고 정상 면과 진단을 콘솔에 출력. 오류가 있으면 최종 상태 실패

출력 파일은 입력 파일의 확장자를 뺀 이름에 사용 가능한 다음 번호를 붙여 `(입력파일이름-번호).txt`로 자동 생성합니다. 예를 들어 `err.txt`는 `outout-6/err-1.txt`, 그 파일이 이미 있으면 `err-2.txt`가 됩니다. 기존 파일은 덮어쓰지 않으며 정상·오류 보고서 모두 저장합니다.

데이터: `data/cube.txt` 12면, PDF에서 복구한 `data/pyramid.txt` 6면, `data/tetrahedron.txt` 4면, 주요 오류 사례를 모은 `data/err.txt`.

CLI 종료 코드: 0 성공, 1 실행·파일 오류, 2 데이터 오류. 메뉴 6은 처리 후 메뉴로 돌아옵니다.

## 구조와 검증

- [전체 아키텍처·멤버·함수·호출 흐름](ARCHITECTURE.md)
- [수정 사항과 검증 리포트](REVIEW.md)
- `tests/WarmupTests.vcxproj`: C++ 직접 회귀 테스트
- `tests/verify_cli.py`: Python 표준 라이브러리만 쓰는 실행 파일·자동 출력 검증

테스트 예:

```powershell
# Visual Studio MSBuild가 PATH에 있는 개발자 터미널
MSBuild tests/WarmupTests.vcxproj /p:Configuration=Debug /p:Platform=x64
.\tests\x64\Debug\WarmupTests.exe
python tests/verify_cli.py
```

실행 파일이 사용 중이면 종료한 뒤 재빌드해야 합니다. `verification-build`는 별도 검증 출력 경로입니다.
