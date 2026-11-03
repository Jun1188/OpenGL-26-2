# API 관계 분석

## 01 소유·참조·상속 관계

화살표의 소유·참조 표시를 구분한다. ID·parent·LayerId 연결은 메모리 소유권 이전이 아니다.

- `standard/main.cpp`
- `standard/Console.h`
- `standard/framework/include/Application.h`
- `standard/framework/include/Graphics.h`
- `standard/framework/include/Model.h`

## 02 호출함·호출됨과 실제 프레임 루프

GPU 처리는 비동기다. GL 함수의 CPU 복귀는 GPU 완료를 뜻하지 않는다. 이벤트가 없으면 입력 콜백도 실행되지 않는다.

- `standard/main.cpp`
- `standard/Console.cpp`
- `standard/framework/detail/Application.cpp`
- `standard/framework/detail/Renderer.cpp`
- `standard/framework/detail/Native.h`

## 03 함수 지역 인스턴싱·반환·수명

지역 CPU 컨테이너가 사라져도 업로드된 GPU 버퍼는 Renderer::destroy까지 유지된다. 참조·ID 반환은 객체 생성과 다르다.

- `standard/main.cpp`
- `standard/framework/detail/Renderer.cpp`
- `standard/framework/detail/Patterns.cpp`
- `standard/framework/detail/Animation.cpp`

## 04 엔티티 계층·모션 연결·변환·GPU 바인딩

엔티티 부모, 모션 레이어, GL 바인딩은 독립된 관계다. shader 이후 고정 단계는 OpenGL 파이프라인 설명이며 별도 C++ 함수 호출이 아니다.

- `standard/framework/detail/Scene.cpp`
- `standard/framework/detail/Motion.cpp`
- `standard/framework/include/Model.h`
- `standard/framework/detail/Renderer.cpp`
- `standard/framework/detail/Shaders.h`

## 종합: main에서 GPU까지, 다음 프레임과 종료까지

한 실행 프로젝트 standard의 흐름이다. framework/examples의 독립 main을 이 실행 경로에 합치지 않는다.

- `standard/main.cpp`
- `standard/Console.cpp`
- `standard/framework/detail/Application.cpp`
- `standard/framework/detail/Renderer.cpp`

빌드·테스트·브라우저 확인은 사용자에게 맡긴다.
