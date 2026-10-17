# 수정 1: Windows CreateWindow 매크로 충돌

기준 커밋: ac6110d. 실습 16에서 보고된 CreateWindowExW 멤버 오류와 쉼표 구문 오류의 공통 원인을 수정한다. 루트와 실습 13~19의 동일한 프레임워크에 적용한다. 사용자 변경이 없는 작업 트리에서 시작했다.

## 원인과 처리 흐름

Windows SDK WinUser.h에서 UNICODE 구성은 CreateWindow를 CreateWindowW로 치환한다. CreateWindowW(...)는 CreateWindowExW(0L, ...)로 다시 확장된다. 따라서 GlfwApi의 멤버 호출이 GLFW 함수 포인터 호출로 남지 않고 Win32 매크로에 의해 바뀐다. 매크로는 C++의 namespace·클래스·멤버 접근을 구분하지 않는다. ASCII 구성도 CreateWindowA를 통해 같은 종류의 충돌이 발생한다.

전처리 → C++ 파싱 단계의 문제다. 정점 데이터·변환·GPU 렌더링에 도달하기 전에 실패한다. 쉼표 구문 오류도 이 잘못된 매크로 확장에 뒤따를 수 있다. 다른 독립적인 구문 오류까지 실행 확인한 것은 아니다.

프레임워크 내부 멤버를 createWindow로 바꾼다. Windows 매크로를 undef하지 않으며, DLL export 이름 glfwCreateWindow와 함수 포인터 형식은 유지한다. 공개 Application API·호출 인자·자원 수명·렌더링은 바꾸지 않는다.

## 변경 블록 전체

다음 before/after는 각각 루트 framework/detail 및 openGL_3dAssign_13~19/framework/detail의 동일한 파일에 적용한다. 총 8개 배포 위치, 16개 소스 파일이다.

### Native.h — GlfwApi 함수 포인터 선언

before:
```cpp
    decltype(&::glfwCreateWindow) CreateWindow = nullptr;
```

after:
```cpp
    decltype(&::glfwCreateWindow) createWindow = nullptr;
```

멤버 이름이 Win32의 객체형 매크로와 겹치지 않게 한다.

### Native.h — GlfwApi::load의 export 연결

before:
```cpp
        CreateWindow = module.symbol<decltype(CreateWindow)>("glfwCreateWindow");
```

after:
```cpp
        createWindow = module.symbol<decltype(createWindow)>("glfwCreateWindow");
```

바뀐 멤버에 같은 DLL export를 연결한다. 문자열 glfwCreateWindow는 변경하지 않는다.

### Application.inl — Application::initialize의 창 생성

before:
```cpp
    window_ = detail::glfw().CreateWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
```

after:
```cpp
    window_ = detail::glfw().createWindow(config.width, config.height,
                              config.title.c_str(), nullptr, nullptr);
```

같은 5개 인자로 GLFW 함수 포인터를 호출한다. 전처리 매크로의 추가 인자 삽입을 피한다.

## 적용 위치

- framework/detail/Native.h, framework/detail/Application.inl
- openGL_3dAssign_13/framework/detail/Native.h, openGL_3dAssign_13/framework/detail/Application.inl
- openGL_3dAssign_14/framework/detail/Native.h, openGL_3dAssign_14/framework/detail/Application.inl
- openGL_3dAssign_15/framework/detail/Native.h, openGL_3dAssign_15/framework/detail/Application.inl
- openGL_3dAssign_16/framework/detail/Native.h, openGL_3dAssign_16/framework/detail/Application.inl
- openGL_3dAssign_17/framework/detail/Native.h, openGL_3dAssign_17/framework/detail/Application.inl
- openGL_3dAssign_18/framework/detail/Native.h, openGL_3dAssign_18/framework/detail/Application.inl
- openGL_3dAssign_19/framework/detail/Native.h, openGL_3dAssign_19/framework/detail/Application.inl

## 구조와 사용자 확인

main → Application::run → initialize → GlfwApi::load가 번들 DLL의 glfwCreateWindow를 createWindow에 연결 → createWindow 호출 → 컨텍스트·입력·업데이트·렌더 → shutdown 흐름은 유지한다. 창 종료는 기존 DestroyWindow 멤버와 glfwDestroyWindow를 사용한다. 전체 구조와 별도 진입점은 graphify-out/EXECUTION_FLOW.md와 PROJECTS.json에 있다.

빌드·테스트·전처리 실행은 하지 않았다. 사용자가 실습 16을 Debug/Release x64로 다시 빌드하여 두 오류가 사라지는지 확인한다. 이어서 실습 13~19도 각각 빌드하고 창 생성·렌더·Q/Esc 종료를 확인한다. 매크로를 정의한 Windows SDK 헤더를 읽은 결과와 실제 빌드 성공은 구분한다.

기존 BEFORE_AFTER.md는 최초 8단계 구현의 기록이다. 이번 수정의 전체 코드 대조는 이 문서가 기준이다. 코드·함수 호출 앵커를 갱신한 구조 그래프는 컴파일 성공을 증명하지 않는다.

## 분석 메타데이터 변경 전후

framework-analysis/update_graph.py의 SOURCE_MANIFEST 생성은 최초 8단계 이후의 수정에도 같은 도구를 사용하므로 상태 필드 이름을 일반화한다. 실제 추출 범위와 실행 동작은 유지한다. graphify-out/README.md의 같은 필드 설명도 함께 갱신한다.

before:
```python
         'source_commit':commit,'includes_uncommitted_step8':True,'files':hashes})
```

after:
```python
         'source_commit':commit,'includes_uncommitted_changes':True,'files':hashes})
```

구조 분석 결과는 프레임워크 17개 코드 파일, 저장소 전체 105개 코드 파일을 대상으로 갱신했다. 미해결 AST 관계는 각 GRAPH_HEALTH.md에 남겨 두었다. 빌드·테스트 결과로 해석하지 않는다.
