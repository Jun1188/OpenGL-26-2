# 후속 수정 2: 디버거 작업 폴더 상속 복구

## 확인한 증거

- 사용자 증상: “디버깅을 시작할 수 없습니다”, 프로젝트 구성 속성을 확인하라는 안내.
- 기존 실습 16 Debug 빌드 로그에는 실행 파일 생성이 기록되어 있고, `openGL_3dAssign_16/x64/Debug/openGL_assign_16.exe`가 존재한다. 이번 작업에서 빌드하거나 실행하지 않았다.
- MSBuild 속성 평가에서 실습 16 Debug/Release와 실습 17 Debug의 `ConfigurationType`은 `Application`, `LocalDebuggerCommand`는 해당 구성의 실행 파일 경로였다. `LocalDebuggerWorkingDirectory`는 빈 값이었다.
- 공용 Framework.props는 Microsoft.Cpp.targets보다 먼저 읽힌다. 이때 `TargetDir`는 아직 정해지지 않아 작업 폴더 지정이 빈 값으로 평가된다.
- 설치된 Microsoft.Cpp.Common.props의 기본 작업 폴더는 `$(ProjectDir)`다. TargetDir와 실행 명령의 후속 초기화는 Microsoft.CppCommon.targets에 있다.

## 전체 변경 전후

다음 여덟 파일에 동일한 변경을 반영했다.

- `framework/Framework.props`
- `openGL_3dAssign_13/framework/Framework.props`
- `openGL_3dAssign_14/framework/Framework.props`
- `openGL_3dAssign_15/framework/Framework.props`
- `openGL_3dAssign_16/framework/Framework.props`
- `openGL_3dAssign_17/framework/Framework.props`
- `openGL_3dAssign_18/framework/Framework.props`
- `openGL_3dAssign_19/framework/Framework.props`

Before:

```xml
  <PropertyGroup>
    <GraphicsFrameworkDirectory>$(MSBuildThisFileDirectory)</GraphicsFrameworkDirectory>
    <LocalDebuggerWorkingDirectory>$(TargetDir)</LocalDebuggerWorkingDirectory>
  </PropertyGroup>
```

After:

```xml
  <PropertyGroup>
    <GraphicsFrameworkDirectory>$(MSBuildThisFileDirectory)</GraphicsFrameworkDirectory>
  </PropertyGroup>
```

수정 후 빌드 타깃 없이 속성만 다시 평가한 결과, 실습 16 Debug의 작업 폴더는 프로젝트 폴더로 복구됐고 실행 명령은 기존 실행 파일 경로를 유지했다.

잘못된 덮어쓰기를 제거하고 Visual Studio의 기본 작업 폴더를 상속한다. 개인 절대경로를 저장하지 않는다. 런타임 자원 탐색은 실행 파일 위치를 기준으로 하므로 작업 폴더를 프로젝트 폴더로 상속해도 기존 자원 배치를 바꿀 필요가 없다. 사용자 .vcxproj.user와 .vs 상태는 변경하지 않았다.

C++ 코드, 진입점, 렌더링 호출 관계는 변경하지 않았다. 기존 `graphify-out/EXECUTION_FLOW.md`와 `framework-analysis/graphify-out`의 코드 구조 분석을 그대로 사용한다.

## 사용자 확인 절차

1. Visual Studio에서 솔루션을 다시 열거나 변경된 프로젝트를 다시 로드한다.
2. 실행할 프로젝트를 시작 프로젝트로 지정하고 `Debug | x64`를 선택한다.
3. 프로젝트 속성 → 구성 속성 → 디버깅에서 로컬 Windows 디버거를 확인한다. 명령은 `$(TargetPath)`, 작업 디렉터리는 상속된 `$(ProjectDir)`여야 한다. 개인 .user 설정이 값을 별도로 지정했다면 그 값도 확인한다.
4. 사용자가 빌드한 뒤 F5로 실행한다. 예상 결과는 디버거가 해당 구성의 실행 파일을 시작하는 것이다.
5. 실습 13~19와 Release x64도 사용자가 확인한다. Release 실행 파일은 해당 구성을 빌드한 뒤에 확인한다.
6. 안내가 계속 나오면 대화상자의 전체 문구, 선택된 시작 프로젝트, 구성, 디버깅의 명령 값을 함께 확인한다.

빌드와 테스트, 디버거 실행은 수행하지 않았다. 빈 작업 폴더 덮어쓰기는 확인하고 제거했지만, 전체 대화상자 문구와 실제 실행 결과가 없어 사용자 증상의 유일한 원인으로 확정하지 않는다.