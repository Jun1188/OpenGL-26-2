# 후속 수정 4: Windows 콘솔의 UTF-8 출력

## 증거와 원인

실습 16의 `Console::initScene()`에는 정상 한글 문자열 `Practice 16 키 조작:`이 있다. 공용 Framework.props는 `/utf-8`을 전달한다. 사용자 프로젝트에 추가된 `/utf-8` 설정도 유지했다. 기존 프레임워크에는 `SetConsoleOutputCP` 호출이 없었다.

UTF-8 BOM은 컴파일러가 소스 파일을 읽을 때 사용하는 표식이다. `/utf-8`은 소스와 실행 문자열 인코딩을 UTF-8로 지정한다. Windows 콘솔이 이 문자열의 바이트를 다른 코드 페이지로 해석하면 한글이 깨질 수 있다. 이번 수정은 빠져 있던 콘솔 출력 코드 페이지 설정을 추가한다. 사용자 실행 환경의 수정 전 코드 페이지 값은 측정하지 않았다.

## 전체 코드 변경 전후

루트 `framework/detail/Application.inl` 및 실습 13~19 각각의 `framework/detail/Application.inl`에 같은 변경을 적용했다. 위치는 `Application::run()`이다.

Before:

```cpp
    if(detail::applicationActive()) return -1;
    detail::applicationActive()=true;
```

After:

```cpp
    if(detail::applicationActive()) return -1;
    // /utf-8 encodes narrow strings; the Windows console must decode them as UTF-8.
    SetConsoleOutputCP(CP_UTF8);
    detail::applicationActive()=true;
```

실행 흐름은 `main → Application::run → 콘솔 출력 UTF-8 설정 → initialize → initHandler → Console::initScene → std::cout`이다. 오류 출력과 이후 입력 콜백의 출력에도 같은 콘솔 설정이 적용된다. Windows API 선언은 기존 Graphics.h → Native.h → windows.h 경로로 포함된다. 콘솔이 없는 실행에서는 이 설정이 실패할 수 있지만 렌더링 시작을 막지 않는다. 출력이 파일로 리디렉션되면 문자열 바이트는 UTF-8로 기록되며 콘솔 코드 페이지와 별개다.

프로세스에서 연결된 콘솔의 출력 코드 페이지를 변경한다. 프레임워크 실행 전에 직접 출력하는 문자열은 이 호출의 적용 대상이 아니다. 입력 코드 페이지, 전역 로케일, 시스템 설정은 변경하지 않는다. 기존 1~12는 Application을 사용하지 않으므로 이번 변경의 적용 대상이 아니다.

기존 graphify 구조 자료를 재사용한다. 새 Windows API 한 번의 호출을 추가한 작은 변경이므로 그래프를 재생성하지 않았다. 기존 그래프에는 이 API 호출이 반영되어 있지 않다.

## 사용자 확인

빌드와 테스트는 실행하지 않았다. 사용자가 다음을 수행한다.

1. 실습 16을 Debug x64로 다시 빌드하고 실행한다. 기존 exe만 다시 실행하면 수정이 적용되지 않는다.
2. 콘솔 첫 줄의 `Practice 16 키 조작:`과 이후 한글 안내가 정상 표시되는지 확인한다.
3. Release x64와 실습 13~19에서도 한글 출력을 확인한다.
4. 글자가 여전히 깨지면 실제 컴파일 명령의 `/utf-8` 적용 여부를 확인한다. 글자 대신 빈 사각형이 나오면 콘솔 글꼴의 한글 지원도 확인한다.

실제 CMD 출력 결과는 사용자 확인 전까지 미검증이다.