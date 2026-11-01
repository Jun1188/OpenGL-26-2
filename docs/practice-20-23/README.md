# 최신 추가 수정

실습23 보류를 해제하여 구현했다. 20 원호 코너 회전, 21 e 전체낙하/v Y자전, 22 WGL 컨텍스트 보호·진단 Source/Operation을 반영했다. [재개 결과](RESUME.md) 및 [23](../../openGL_3dAssign_23/README.md)를 먼저 읽는다. 이번 재개에서는 ZIP을 생성하지 않았다. 아래 ZIP과 보류 설명은 이전 작업 기록이다.

# 프레임워크·standard 개선과 실습 20~22

2026-11-01. 사용자가 프레임워크/standard 개선 후 각 실습을 서브에이전트에 맡기는 순서와 수정·단계별 커밋을 승인했다. 실습 23은 함께 개발하기 위해 보류했다. 실습20은 사용자가 삭제한 빈 폴더에서 새로 작성했다.

## 결과와 읽는 순서

1. [공통 API](../../framework/docs/APPLICATION_API.md): 진단·Grid·거리 경로·카메라·조립·충돌·렌더 패스의 계약과 제한.
2. [standard](../../standard/PORTABLE.md): 기존 Motion 유지, 7 Grid/8 실내/0 기존 예제, V 3뷰, D 단계 추적, F 바닥 개방.
3. [실습20](../../openGL_3dAssign_20/README.md): ㄱ자 평면 벨트, 줄무늬30, 번호와 색이 다른 상자5, 공통 거리시간, 코너 방향, 카메라.
4. [실습21](../../openGL_3dAssign_21/README.md): 내부 컬링, 공 반사 최대5개, 바닥의 크기 다른 상자3개, Z 드래그·미끄럼·F 낙하.
5. [실습22](../../openGL_3dAssign_22/README.md): 실습19 전체 탱크 계층을 공통 API로 조립하고 같은 Scene을 3뷰에 렌더링.
6. [전체 변경 전후](BEFORE_AFTER.md), [파일별 변경 목록](CHANGE_MANIFEST.json), [단계별 커밋](COMMITS.md).
7. [탑다운 구조](TOP_DOWN.md), [저장소 그래프](../../graphify-out/graph.html), [프레임워크 그래프](../../framework-analysis/graphify-out/graph.html).
8. [패키지 목록](PACKAGES.json), [임시파일·보존](CLEANUP.md).

## 배포

`python tools/Package-Framework.py`가 standard 및 현재 존재하는 실습15~22를 각각 독립 ZIP으로 만든다. 기본 출력은 `dist/portable-20261101/`이다. 같은 출력이 있으면 보존하고 중단하므로 재생성은 `--output-dir dist/portable-새이름`을 준다. `--project standard --project openGL_3dAssign_20`처럼 대상을 제한할 수 있다. 한도는 각각 3,000,000바이트 미만이며 3MiB보다 엄격하다. IDE/빌드 산출물은 제외하고 SDK·라이선스·OBJ 자료를 보존한다.

standard/20~22는 .sln과 .slnx, C++17, x64, 설치된 도구 집합 선택, /MTd·/MT를 제공한다. VS 2022에서는 .sln으로 열고 v143이 선택되는지 사용자가 확인한다. 기존15~19 프로젝트의 도구 집합은 유지했으므로 VS 2022 이동 시 프로젝트 속성에서 v143으로 바꿔야 할 수 있다. 외부 SDK 설치·NuGet/vcpkg 복원·개발 PC 절대 경로가 필요한 구성은 추가하지 않았다.

동봉 GLFW DLL의 import table은 VCRUNTIME140.dll을 요구한다. 정식 VS Redist의 x64 런타임과 Microsoft 원본 라이선스를 GLFW 옆에 동봉했다. 출처·버전·조건은 [런타임 동봉 정보](../../framework/dependencies/glfw-3.4.bin.WIN64/LICENSE-Microsoft-VC-Runtime.md)에 있다. Windows 10/11 x64의 시스템 DLL·UCRT와 OpenGL 3.3 드라이버는 OS에서 제공해야 한다. Microsoft의 [재배포 설명](https://learn.microsoft.com/en-us/cpp/windows/redistributing-visual-cpp-files)과 [정식 재배포 목록](https://learn.microsoft.com/en-us/visualstudio/releases/2026/redistribution)을 참고했다. 설치 프로그램은 실행하지 않았다.

## 사용자에게 맡기는 확인

에이전트와 서브에이전트는 빌드·실행·테스트·검증기를 실행하지 않았다. 소스·프로젝트 XML·PDF·DLL import table을 읽고, 그래프와 ZIP·문서를 생성했다. 압축 파일 크기는 생성 시 실제 바이트 수다. 컴파일·GL 동작·화면·DLL 로딩·VS 2022 호환성 성공을 의미하지 않는다.

- [실습20 사용자 확인](../../openGL_3dAssign_20/USER_CHECKS.md)
- [실습21 사용자 확인](../../openGL_3dAssign_21/USER_CHECKS.md)
- [실습22 사용자 확인](../../openGL_3dAssign_22/USER_CHECKS.md)
- [standard 및 API 확인](../../framework/docs/APPLICATION_API.md#사용자-확인)

각 .sln의 Debug/Release x64, PDF의 모든 키, 리셋, 창 크기·최소화·포커스·DPI, 다른 경로·다른 PC에서의 상대경로·DLL을 직접 확인한다. 특히20 번호 가독성,21 급드래그/벽 접촉/바닥 낙하,22 3뷰 동일 이동·관절 연결을 확인한다.

공통 충돌은 교육용 축 정렬 대리 모델이다. 정밀 회전 강체·마찰·공끼리/상자끼리 충돌을 구현한 것으로 해석하지 않는다. 실습23 폴더·프로젝트·구현은 만들지 않았고 준비 API만 제공했다. 기존 이론 자료 삭제·미추적 문서는 보존했으며 원격 push는 하지 않았다.
