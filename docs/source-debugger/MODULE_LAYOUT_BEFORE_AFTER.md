# debugger 내부 배치 변경 전후

이번 배치 변경 직전 커밋 `15f88bef3fb818026f6693a578d0c418eb896043` 기준이다.

## 이동 파일

| 이전 경로 | 현재 경로 |
|---|---|
| debugger/Commands.cpp | standard/debugger/Commands.cpp |
| debugger/debugger.sln | standard/debugger/debugger.sln |
| debugger/debugger.vcxproj | standard/debugger/debugger.vcxproj |
| debugger/main.cpp | standard/debugger/main.cpp |
| debugger/README.md | standard/debugger/README.md |
| debugger/SourceIndex.cpp | standard/debugger/SourceIndex.cpp |
| debugger/SourceIndex.h | standard/debugger/SourceIndex.h |
| debugger/Toolchain.props | standard/debugger/Toolchain.props |

이동한 파일의 변경 없는 본문은 중복하지 않는다. 소스/설정의 실제 변경 블록은 아래에 모두 기록한다. 문서·그래프 생성물은 전체 CHANGE_MANIFEST에서 확인한다. 런타임 코드의 변경은 검색 제외 디렉터리에 debugger를 추가한 부분이다. 빌드·테스트·실행·검증기는 수행하지 않았다.

## OpenGL-26-2.sln

### 블록 1 · before L7 / after L7

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
Project("{BC8A1FFA-BEE3-4634-8014-F334798102B3}") = "debugger", "debugger\debugger.vcxproj", "{48D466D1-8954-4D75-A111-D8BAC65250A8}"
````

**after**

````text
Project("{BC8A1FFA-BEE3-4634-8014-F334798102B3}") = "debugger", "standard\debugger\debugger.vcxproj", "{48D466D1-8954-4D75-A111-D8BAC65250A8}"
````


### 블록 2 · before L16 / after L16

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Debug|x64.Build.0 = Debug|x64
````

**after: 없음**



### 블록 3 · before L18 / after L17

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
		{48D466D1-8954-4D75-A111-D8BAC65250A8}.Release|x64.Build.0 = Release|x64
````

**after: 없음**



## standard/debugger/SourceIndex.cpp

### 블록 1 · before L16 / after L16

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
        name == "dependencies" || name == "docs" || name == "old_standard" || name == "graphify-out" ||
````

**after**

````text
        name == "dependencies" || name == "docs" || name == "debugger" || name == "old_standard" || name == "graphify-out" ||
````


## tools/Package-Standard.py

### 블록 1 · before L31 / after L31

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
        if relative.parts[0] in {'.vs', 'x64', '.git', '__pycache__'}:
````

**after**

````text
        build_root = relative.parts[1] if len(relative.parts) > 1 and relative.parts[0] == 'debugger' else relative.parts[0]
        if build_root in {'.vs', 'x64', '.git', '__pycache__'}:
````


## tools/Update-StandardDocs.py

### 블록 1 · before L167 / after L167

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
SIZE_EXCLUDED = ('x64', '.vs')
````

**after**

````text
SIZE_EXCLUDED = (('x64',), ('.vs',), ('debugger', 'x64'), ('debugger', '.vs'))
````


### 블록 2 · before L172 / after L172

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
             and p.relative_to(STANDARD).parts[0] not in SIZE_EXCLUDED]
````

**after**

````text
             and not any(p.relative_to(STANDARD).parts[:len(prefix)] == prefix for prefix in SIZE_EXCLUDED)]
````


## tools/Write-SourceDebuggerDocs.py

### 블록 1 · before L23 / after L23

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
`debugger/debugger.sln`에서 debugger를 빌드한다. 저장소 루트 `OpenGL-26-2.sln`에는 Standard와 debugger가 별도 프로젝트로 들어 있다. 각 프로젝트의 단독 솔루션도 사용할 수 있다. debugger는 OpenGL·GLFW·GLEW·Standard 실행 파일에 의존하지 않는 Windows C++17 콘솔 프로그램이다.
````

**after**

````text
저장소 기준 `standard/debugger/debugger.sln`에서 debugger를 빌드한다. `standard/standard.sln`은 Standard만 참조한다. 루트 `OpenGL-26-2.sln`은 debugger도 표시하는 편의 솔루션이며 기본 전체 빌드는 Standard만 대상으로 한다. 이 편의 솔루션에서는 debugger 프로젝트를 선택해 별도로 빌드한다. debugger의 단독 솔루션에서는 debugger를 빌드한다. Standard와 framework는 debugger의 소스·헤더·라이브러리·실행 파일을 참조하지 않는다. debugger도 OpenGL·GLFW·GLEW·Standard 실행 파일에 의존하지 않는 Windows C++17 콘솔 프로그램이다.

사용자가 Debug/x64로 debugger를 빌드한 뒤 실행할 경로는 다음과 같다.

```powershell
$glStandard = 'C:/Work/OpenGL-26-2/standard'
& "$glStandard/debugger/x64/Debug/debugger.exe" "$glStandard"
```

이 경로는 자기 복사 위치로 바꾼다. Standard 실행 파일은 `standard/x64/Debug/standard.exe`이며 debugger 실행 파일과 별개다.
````


### 블록 2 · before L106 / after L115

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
색인은 선택한 폴더의 `.cpp/.h/.hpp/.glsl/.props/.vcxproj` 텍스트를 읽는다. `.vcxproj` 입력은 해당 파일의 상위 폴더를 선택한다. MSBuild의 조건·매크로·실제 컴파일 목록을 평가하는 기능은 아니다. `dependencies`, `docs`, `old_standard`, 빌드/IDE/그래프 폴더와 링크·재분석 지점은 순회하지 않는다. 파일은 UTF-8(BOM 허용), 개별 8 MiB 이하여야 한다. 다른 인코딩이나 읽기 오류는 경로와 함께 거절한다.
````

**after**

````text
색인은 선택한 폴더의 `.cpp/.h/.hpp/.glsl/.props/.vcxproj` 텍스트를 읽는다. `.vcxproj` 입력은 해당 파일의 상위 폴더를 선택한다. MSBuild의 조건·매크로·실제 컴파일 목록을 평가하는 기능은 아니다. `dependencies`, `docs`, 하위 `debugger`, `old_standard`, 빌드/IDE/그래프 폴더와 링크·재분석 지점은 순회하지 않는다. 따라서 Standard를 대상으로 찾을 때 편집 도구 자체의 구현은 섞이지 않는다. debugger 폴더 자체를 project 대상으로 지정하면 그 안의 소스는 읽을 수 있다. 파일은 UTF-8(BOM 허용), 개별 8 MiB 이하여야 한다. 다른 인코딩이나 읽기 오류는 경로와 함께 거절한다.
````


### 블록 3 · before L113 / after L122

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
    write('debugger/README.md', DEBUGGER)
````

**after**

````text
    write('standard/debugger/README.md', DEBUGGER)
````


### 블록 4 · before L115 / after L124

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before: 없음**


**after**

````text
    write('standard/docs/MODULE_LAYOUT.md', '''# Standard·framework·debugger 모듈 경계

```text
standard/
  standard.sln             # Standard만 참조
  standard.vcxproj         # main/Console/SceneSetup + 자기 framework
  SceneSetup.cpp           # 사용자가 직접 편집하는 C++ 원본
  framework/               # 독립 배포 사본, debugger 참조 없음
  debugger/
    debugger.sln           # 도구만 빌드하는 단독 솔루션
    debugger.vcxproj       # 자기 C++ 소스 + Windows/표준 라이브러리
    Toolchain.props        # 도구 내부 설정
    x64/Debug/debugger.exe # 사용자가 빌드한 뒤 생기는 실행 파일
```

| 모듈 | 실제 빌드 입력 | 다른 모듈과의 관계 |
|---|---|---|
| Standard | main.cpp, Console.cpp, SceneSetup.cpp, framework/Framework.props | framework API 사용. debugger 소스·프로젝트·실행 파일 참조 없음 |
| framework 원본/배포 사본 | 자기 헤더·detail 소스·셰이더·SDK·런타임 DLL | debugger를 include/import/link/실행하지 않음 |
| debugger | 자기 main.cpp, Commands.cpp, SourceIndex.cpp, SourceIndex.h, Toolchain.props | Standard/framework를 링크하지 않음. 사용자가 고른 소스 파일만 읽음 |

저장소 루트 OpenGL-26-2.sln은 두 프로젝트를 함께 보여주는 편의 파일이다. debugger의 기본 Build.0 선택을 두지 않는다. Standard만 빌드할 때는 standard.sln을 사용하고, 도구는 내부 debugger.sln이나 루트 솔루션의 debugger 프로젝트를 명시적으로 선택한다. 프로젝트 사이의 ProjectReference/ProjectDependencies를 추가하지 않는다.

debugger의 출력은 C++ 편집 제안이며 실행 중인 Standard에 호출하는 API가 아니다. @debug 표시는 위치 검색용 주석이라서 Standard/framework 실행 의존성이 아니다. Standard를 대상으로 색인할 때 내부 debugger 폴더는 순회하지 않는다. 도구 자체를 편집하려면 debugger 폴더를 project 대상으로 명시한다.

standard 전체 폴더 또는 debugger 폴더만 별도 위치로 복사할 수 있다. debugger를 뺀 Standard 복사본도 standard.sln/standard.vcxproj와 framework의 참조를 그대로 사용한다. 폴더 분리 후 빌드·실행 확인은 사용자가 수행한다. 이번 작업에서는 소스 참조와 프로젝트 구성을 읽었고 빌드·테스트·앱은 실행하지 않았다.
''')
````


### 블록 5 · before L143 / after L179

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
[전체 문서](index.html) · [작성 흐름](docs/DIRECT_CPP_WORKFLOW.md) · [두 물체 튜토리얼](docs/WORKSPACE_TUTORIAL.md) · [디버거 명령](docs/SOURCE_DEBUGGER.md) · [조명](docs/LIGHTING_AND_MATERIALS.md) · [사용자 확인](USER_CHECKS.md)
````

**after**

````text
[전체 문서](index.html) · [모듈 경계](docs/MODULE_LAYOUT.md) · [작성 흐름](docs/DIRECT_CPP_WORKFLOW.md) · [두 물체 튜토리얼](docs/WORKSPACE_TUTORIAL.md) · [디버거 명령](docs/SOURCE_DEBUGGER.md) · [조명](docs/LIGHTING_AND_MATERIALS.md) · [사용자 확인](USER_CHECKS.md)
````


### 블록 6 · before L145 / after L181

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
- `standard.sln`: 현재 직접 작성 프로젝트. 루트 솔루션에는 별도 debugger 프로젝트도 있다.
````

**after**

````text
- `standard.sln`: Standard만 참조하는 직접 작성 프로젝트. debugger 폴더가 없어도 이 솔루션에 도구 참조가 남지 않는다.
````


### 블록 7 · before L149 / after L185

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before: 없음**


**after**

````text
- `debugger/`: 소스 편집 안내용 독립 콘솔 프로젝트. Standard와 framework의 필수 모듈이 아니다. `debugger/debugger.sln`으로 따로 연다.
````


### 블록 8 · before L188 / after L225

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
Standard는 자체 framework·셰이더·헤더·라이브러리·실행 DLL을 가진다. debugger는 자체 소스와 Toolchain.props만 사용한다. 합본 솔루션의 프로젝트 경로는 편의용이며 각 단독 솔루션의 빌드/실행 의존성은 아니다. 실제 소스 위치는 debugger 실행 인수나 project 명령으로 선택한다. 폴더를 다른 위치로 복사한 뒤 빌드·실행하는 확인은 사용자가 수행한다.
````

**after**

````text
Standard는 자체 framework·셰이더·헤더·라이브러리·실행 DLL을 가진다. 선택 도구는 `standard/debugger`에 있으며 자체 소스와 Toolchain.props만 사용한다. Standard의 vcxproj와 framework의 헤더/props/런타임은 debugger를 참조하지 않는다. Standard를 직접 빌드·실행할 때 debugger가 필요하지 않다. debugger도 Standard나 framework를 링크하지 않고 실행 인수로 선택한 디스크 소스를 읽는다.

standard.sln은 Standard만 참조한다. 저장소 루트의 편의 솔루션은 두 프로젝트를 표시하지만 ProjectReference/ProjectDependencies를 추가하지 않으며 debugger의 Build.0 기본 선택도 제외한다. 도구를 빌드하려면 그 프로젝트를 선택하거나 내부 단독 솔루션을 사용한다. standard 폴더 전체를 복사하면 내부 두 프로젝트와 framework 사본이 함께 이동하며 상위 저장소에 실행/빌드 의존성을 두지 않는다. debugger 폴더를 뺀 복사본도 standard.sln/standard.vcxproj와 framework 소스를 그대로 사용한다. debugger 폴더 자체도 별도로 복사할 수 있다. 폴더 분리 후 실제 빌드·실행 확인은 사용자가 수행한다.
````


### 블록 9 · before L305 / after L344

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
1. 루트 `OpenGL-26-2.sln`을 Visual Studio에서 열고 Standard와 debugger를 각각 빌드한다. Standard 컴파일 목록에 Assembly*.cpp가 없고 SceneSetup.cpp와 framework/detail/SceneAuthoring.cpp가 있는지 확인한다.
````

**after**

````text
1. `standard/standard.sln`은 Standard만 참조하고 루트 `OpenGL-26-2.sln`은 두 프로젝트 중 Standard만 기본 빌드하는지 확인한다. 루트 솔루션에서 debugger는 프로젝트를 선택해 별도로 빌드한다. `standard/debugger/debugger.sln`에서는 도구만 빌드한다. Standard 컴파일 목록에 Assembly*.cpp/debugger 소스/ProjectReference가 없고 SceneSetup.cpp와 framework/detail/SceneAuthoring.cpp가 있는지 확인한다.
````


### 블록 10 · before L312 / after L351

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
8. `find "SceneAuthoring::update"`를 framework 대상으로 실행하고 실제 결과를 `code`로 읽는다. 구획이 없는 일반 실습에도 find/code/replace/insert의 before가 실제 파일과 같은지 확인한다. 이 명령은 MSBuild 활성 분기나 C++ 의미를 판정하지 않는다.
````

**after**

````text
8. `find "SceneAuthoring::update"`를 framework 대상으로 실행하고 실제 결과를 `code`로 읽는다. Standard 대상으로는 하위 debugger 소스가 검색되지 않고 debugger 폴더를 명시한 project 대상에서는 그 구현이 검색되는지 확인한다. 구획이 없는 일반 실습에도 find/code/replace/insert의 before가 실제 파일과 같은지 확인한다. 이 명령은 MSBuild 활성 분기나 C++ 의미를 판정하지 않는다.
````


### 블록 11 · before L314 / after L353

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
10. standard 폴더와 debugger 폴더를 각각 다른 위치로 복사해 단독 솔루션 빌드·실행을 확인한다. Standard의 framework 배포 사본과 DLL/셰이더 복사 결과를 확인한다.
````

**after**

````text
10. standard 폴더 전체와 그 안의 debugger 폴더를 각각 다른 위치로 복사해 단독 프로젝트 빌드·실행을 확인한다. 별도 복사본에서 debugger를 빼도 standard.sln/standard.vcxproj와 framework가 도구 없이 빌드·실행되는지 확인한다. Standard의 framework 배포 사본과 DLL/셰이더 복사 결과도 확인한다. Package-Standard와 용량 집계는 standard 및 내부 debugger의 x64/.vs를 제외하지만 framework/dependencies의 SDK 입력은 보존한다.
````


### 블록 12 · before L344 / after L383

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before: 없음**


**after**

````text
편집 보조 도구는 Standard 폴더 내부의 독립 debugger 프로젝트다. framework는 debugger를 include/import/link하거나 실행하지 않는다. debugger 소스와 실행 파일이 없어도 framework를 사용하는 프로젝트의 빌드/실행 계약은 같다.
````


### 블록 13 · before L361 / after L402

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
                ('README.md', 'Standard 시작'), ('docs/DIRECT_CPP_WORKFLOW.md', '작성 위치와 실행 흐름'),
````

**after**

````text
                ('README.md', 'Standard 시작'), ('docs/MODULE_LAYOUT.md', '모듈 경계와 내부 debugger'), ('docs/DIRECT_CPP_WORKFLOW.md', '작성 위치와 실행 흐름'),
````


## tools/Write-SourceDebuggerReport.py

### 블록 1 · before L17 / after L17

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
         'tools/Update-StandardDocs.py', 'tools/Update-StandardModesGraph.py', 'OpenGL-26-2.sln'}
````

**after**

````text
         'tools/Update-StandardDocs.py', 'tools/Update-StandardModesGraph.py', 'tools/Package-Standard.py', 'OpenGL-26-2.sln'}
````


### 블록 2 · before L82 / after L82

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
    write(OUT / 'CHANGE_MANIFEST.json', json.dumps({'baseline': BASE, 'changed_code_blocks': blocks,
        'source_state': 'Saved source during final documentation stage; hashes include current generated artifacts.',
        'excluded_preexisting_user_items': ['openGL_3dAssign_26/framework/detail/Renderer.cpp', 'docs.zip',
            'lecture_pdf/homework/*', 'tools/__pycache__/*'], 'files': manifest}, ensure_ascii=False, indent=2))
````

**after: 없음**



### 블록 3 · before L107 / after L103

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
        '1. wmain은 UTF-8 콘솔을 설정하고 인수 또는 project 명령으로 대상 폴더를 고른다. 독립 실행 프로그램이며 Standard의 메모리/렌더러를 제어하지 않는다.',
        '2. command는 문자열을 토큰화하고 SourceIndex::refresh로 실제 디스크 소스를 다시 읽는다. 시작/끝 구획과 현행 행 번호를 만든다.',
````

**after**

````text
        '1. standard/debugger/main.cpp의 wmain은 UTF-8 콘솔을 설정하고 인수 또는 project 명령으로 대상 폴더를 고른다. 독립 실행 프로그램이며 Standard의 메모리/렌더러를 제어하지 않는다. Standard/framework는 도구를 참조하지 않는다.',
        '2. command는 문자열을 토큰화하고 SourceIndex::refresh로 실제 디스크 소스를 다시 읽는다. 시작/끝 구획과 현행 행 번호를 만든다. Standard 대상 검색에서 하위 debugger는 제외한다.',
````


### 블록 4 · before L147 / after L143

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before: 없음**


**after**

````text
- 배치 변경 승인: debugger를 standard/debugger로 이동. standard.sln은 Standard만 참조하고, 루트 편의 솔루션의 도구 기본 Build.0은 제외한다. framework/Standard에 debugger 의존성을 추가하지 않는다.
````


### 블록 5 · before L155 / after L152

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before**

````text
| 5 | 최종 구조/보고 커밋 | graphify AST, 프로젝트 진입점, 전체 before/after·manifest |
````

**after**

````text
| 5 | 15f88be | graphify AST, 프로젝트 진입점, 전체 before/after·manifest |
| 6 | 13b888e | standard/debugger 이동, 독립 프로젝트 유지, 소스 검색·패키징 경계 |
| 7 | 후속 문서/구조 커밋 | 내부 배치 문서·그래프·전체/추가 before-after 갱신 |
````


### 블록 6 · before L163 / after L162

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before: 없음**


**after**

````text
    layout_before_after()
    listed = {item['path'] for item in manifest}
    for name in ('docs/source-debugger/PLAN.md', 'docs/source-debugger/ARCHITECTURE.md',
                 'docs/source-debugger/MODULE_LAYOUT_BEFORE_AFTER.md', 'graphify-out/source-debugger/GRAPH_REPORT.md'):
        if name not in listed: manifest.append({'path': name, 'preserved_project_relocation': False})
    # Record generated artifacts after their final writes; omit self-referential report/manifest hashes.
    for item in manifest:
        path = ROOT / item['path']
        data = path.read_bytes() if path.is_file() else b''
        item.update(exists=path.is_file(), bytes=len(data), sha256=hashlib.sha256(data).hexdigest() if path.is_file() else None)
    write(OUT / 'CHANGE_MANIFEST.json', json.dumps({'baseline': BASE, 'changed_code_blocks': blocks,
        'source_state': 'Saved source during final documentation stage; hashes include final generated artifacts.',
        'excluded_preexisting_user_items': ['openGL_3dAssign_26/framework/detail/Renderer.cpp', 'docs.zip',
            'lecture_pdf/homework/*', 'tools/__pycache__/*'], 'files': manifest}, ensure_ascii=False, indent=2))
````


### 블록 7 · before L164 / after L177

변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.

**before: 없음**


**after**

````text


def layout_before_after():
    baseline = '15f88bef3fb818026f6693a578d0c418eb896043'
    names = set(git('diff', '--no-renames', '--name-only', baseline, '--').decode('utf-8').splitlines())
    names.update(git('ls-files', '--others', '--exclude-standard').decode('utf-8').splitlines())
    lines = ['# debugger 내부 배치 변경 전후', '', '이번 배치 변경 직전 커밋 `' + baseline + '` 기준이다.', '',
        '## 이동 파일', '', '| 이전 경로 | 현재 경로 |', '|---|---|']
    for path in sorted((ROOT / 'standard/debugger').iterdir()):
        if path.is_file(): lines.append('| debugger/' + path.name + ' | standard/debugger/' + path.name + ' |')
    lines += ['', '이동한 파일의 변경 없는 본문은 중복하지 않는다. 소스/설정의 실제 변경 블록은 아래에 모두 기록한다. '
        '문서·그래프 생성물은 전체 CHANGE_MANIFEST에서 확인한다. 런타임 코드의 변경은 검색 제외 디렉터리에 debugger를 추가한 부분이다. '
        '빌드·테스트·실행·검증기는 수행하지 않았다.', '']
    for name in sorted(names):
        if not (name in TOOLS or name.startswith('standard/debugger/')): continue
        path = ROOT / name
        if not path.is_file() or path.suffix not in {'.cpp', '.h', '.props', '.vcxproj', '.sln', '.py'}: continue
        old_name = 'debugger/' + name[len('standard/debugger/'):] if name.startswith('standard/debugger/') else name
        result = subprocess.run(['git', 'show', baseline + ':' + old_name], cwd=ROOT, capture_output=True)
        left = result.stdout.decode('utf-8-sig').replace('\r\n', '\n').splitlines(keepends=True) if result.returncode == 0 else []
        right = path.read_text(encoding='utf-8-sig').splitlines(keepends=True)
        groups = list(difflib.SequenceMatcher(None, left, right, autojunk=False).get_grouped_opcodes(0))
        if not groups: continue
        lines += ['## ' + name, '']
        for n, group in enumerate(groups, 1):
            first, last = group[0], group[-1]
            i, j, a, b = first[1], last[2], first[3], last[4]
            old, new = ''.join(left[i:j]), ''.join(right[a:b])
            lines += [f'### 블록 {n} · before L{i+1} / after L{a+1}', '',
                '변경 이유: 도구를 Standard 내부에 배치하면서 원본/배포 프레임워크와 Standard의 빌드·실행 독립성을 유지하고 새 경로를 문서/패키징/보고에 연결한다.', '',
                '**before**' if old else '**before: 없음**', '', code(old, 'text') if old else '',
                '**after**' if new else '**after: 없음**', '', code(new, 'text') if new else '', '']
    write(OUT / 'MODULE_LAYOUT_BEFORE_AFTER.md', '\n'.join(lines))
````
