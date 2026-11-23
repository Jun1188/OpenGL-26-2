# OpenGL Practice 28 & Framework 코드베이스 아키텍처

정리 후 `framework`(23개 핵심 파일)와 상위 실습 프로젝트 `openGL_3dAssign_28`(`main.cpp`, `Console.h`, `Console.cpp`)의 계층 의존성 및 실행 루프를 정리한 문서입니다.

---

## 1. Framework 계층 의존성 Flowchart (단방향 DAG)

모든 `detail/*.cpp`는 우산 헤더 `Framework.h`를 역참조하지 않고 자신이 구현하는 공개 헤더와 `detail/Internal.h`만 포함합니다.

```mermaid
flowchart TD
    subgraph App["openGL_3dAssign_28 (실습 28)"]
        MAIN["main.cpp"]
        CON["Console.h / Console.cpp"]
        MAIN --> CON
    end

    FW["Framework.h (공개 진입점)"]
    CON --> FW

    subgraph L3["Layer 3: Application"]
        APPH["include/Application.h"]
        APPCPP["detail/Application.cpp"]
        APPCPP --> APPH
    end

    subgraph L2["Layer 2: Scene · Renderer · Loader · Picking"]
        GRH["include/Graphics.h"]
        PKH["include/Picking.h"]
        MLH["include/ModelLoader.h"]
        MOH["include/Motion.h"]
        RCH["include/RenderConfig.h"]
        TXH["include/Texture.h"]

        SCPP["detail/Scene.cpp"]
        RCPP["detail/Renderer.cpp"]
        OCPP["detail/ObjLoader.cpp"]
        MCPP["detail/Motion.cpp"]

        PKH --> GRH
        GRH --> MOH
        GRH --> RCH
        MLH --> TXH
        SCPP --> GRH
        RCPP --> GRH
        OCPP --> MLH
        MCPP --> MOH
    end

    subgraph L1["Layer 1: Geometry · Transform · Dynamics · Model"]
        GEOH["include/Geometry.h"]
        TRH["include/Transform.h"]
        DYH["include/Dynamics.h"]
        MODH["include/Model.h"]

        TCPP["detail/Transform.cpp"]
        DCPP["detail/Dynamics.cpp"]

        GEOH --> MODH
        TRH --> MODH
        DYH --> MODH
        TCPP --> TRH
        DCPP --> DYH
    end

    subgraph L0["Layer 0: Diagnostics · Native · Internal"]
        DIAH["include/Diagnostics.h"]
        NATH["detail/Native.h"]
        INTH["detail/Internal.h"]
        SHDH["detail/Shaders.h"]

        MODH --> DIAH
        NATH --> DIAH
        INTH --> MODH
        RCPP --> SHDH
    end

    FW --> APPH
    FW --> DYH
    APPH --> GRH
    APPH --> MLH
    APPH --> PKH
    GRH --> GEOH
    MLH --> GEOH
    MOH --> TRH
```

---

## 2. 프로젝트 실행 및 메인 루프 Flowchart (`main.cpp` → `Application::run` ↔ `Console`)

`main.cpp`에서 콜백을 등록한 뒤 `app.run(config)`를 호출하면 아래와 같은 초기화 → 이벤트/갱신/렌더 루프 → 정리 흐름으로 동작합니다.

```mermaid
flowchart TD
    START(["main(): Application · Console 생성<br/>Init / ModifiedKey / Update / Focus 핸들러 등록"])
    RUN["Application::run(config)<br/>중복 실행 검사 · UTF-8 콘솔 설정"]
    INIT{"Application::initialize(config)<br/>GLFW · OpenGL 3.3 창 · Renderer · AssetLoader"}
    UINIT{"Console::initScene()<br/>reset()으로 탱크·조명·카메라 구성"}
    FAIL(["오류 출력 후 exitCode = -1"])

    LOOP{"glfwWindowShouldClose?"}
    POLL["glfwPollEvents()<br/>→ Console::onKey / onFocus"]
    CBERR{"callbackError_ 존재?"}
    RETHROW(["예외 rethrow → shutdown()"])
    CLOSING{"창 닫힘 요청?"}

    FBSIZE{"프레임버퍼 크기 > 0?"}
    WAIT["glfwWaitEvents() 후<br/>previousTime 갱신"]

    DT["deltaTime 계산<br/>clamp(now - prev, 0.0, 0.05)"]
    UPDATE["Console::onUpdate(dt)<br/>이동·회전·스왑·카메라·조명 갱신 → apply()"]
    MOTIONS{"scene_.updateMotions(dt)"}
    CTX{"ensureContext()"}
    RENDER{"renderScene()<br/>(revision 변경 시 GPU upload → draw)"}
    SWAP{"glfwSwapBuffers()<br/>컨텍스트 에러 없음?"}

    SHUT["Application::shutdown()<br/>Renderer 정리 · 창 파괴 · glfwTerminate"]
    END(["run() 종료 코드 반환"])

    START --> RUN --> INIT
    INIT -- 실패 --> FAIL --> SHUT
    INIT -- 성공 --> UINIT
    UINIT -- false --> FAIL
    UINIT -- true --> LOOP

    LOOP -- Yes --> SHUT --> END
    LOOP -- No --> POLL --> CBERR
    CBERR -- Yes --> RETHROW
    CBERR -- No --> CLOSING
    CLOSING -- Yes --> SHUT
    CLOSING -- No --> FBSIZE

    FBSIZE -- "width<=0 or height<=0" --> WAIT --> LOOP
    FBSIZE -- "width>0 && height>0" --> DT --> UPDATE --> MOTIONS
    MOTIONS -- false --> FAIL
    MOTIONS -- true --> CTX
    CTX -- false --> FAIL
    CTX -- true --> RENDER
    RENDER -- false --> FAIL
    RENDER -- true --> SWAP
    SWAP -- false --> FAIL
    SWAP -- true --> LOOP
```

---

## 3. 실습 28 (`Console`) 탱크 씬 그래프 계층

`Console::reset()`이 생성하고 `Console::apply()`가 매 프레임 갱신하는 엔티티 계층 구조입니다.

```mermaid
flowchart LR
    SCENE["Scene (CPU 엔티티 저장소)"]
    GROUND["ground (Group)<br/>바닥 판 12x0.1x12"]
    LAMP["lamp_ (Cuboid, unlit)<br/>포인트 라이트 위치 마커"]
    ROOT["root_ (Group)<br/>방향키 이동 position_"]

    LOW0["lower_[0] (왼쪽 궤도)"]
    LOW1["lower_[1] (오른쪽 궤도)"]
    CENT["central_ (Group)<br/>h 키 높이 + t 키 Y회전"]
    BODY["중앙 몸체 (2.3x0.25x1.4)"]

    UP0["upper_[0] (Group)<br/>l 키 좌우 스왑 cos/sin"]
    UP1["upper_[1] (Group)<br/>l 키 좌우 스왑 cos/sin"]

    GUN0["guns_[0] (g 키 반대 yaw)"]
    POLE0["poles_[0] (p 키 반대 pitch)"]
    GUN1["guns_[1] (g 키 반대 yaw)"]
    POLE1["poles_[1] (p 키 반대 pitch)"]

    SCENE --> GROUND
    SCENE --> LAMP
    SCENE --> ROOT
    ROOT --> LOW0
    ROOT --> LOW1
    ROOT --> CENT
    CENT --> BODY
    CENT --> UP0
    CENT --> UP1
    UP0 --> GUN0
    UP0 --> POLE0
    UP1 --> GUN1
    UP1 --> POLE1
```

---

## 4. 파일별 책임 요약 (경량화 후 23개)

| 계층 | 파일 | 책임 |
|---|---|---|
| 진입점 | `Framework.h` | 사용자용 우산 헤더 (`Application.h`, `Dynamics.h`) |
| Layer 3 | `include/Application.h`, `detail/Application.cpp` | GLFW 창 수명, 이벤트 디스패치, `run()` 메인 루프 |
| Layer 2 | `include/Graphics.h`, `detail/Scene.cpp`, `detail/Renderer.cpp` | `Scene`(엔티티·계층·라인), `DiagramSystem`, `CameraSystem`, `Renderer`(VAO/VBO/EBO·셰이더·라이트) |
| Layer 2 | `include/Motion.h`, `detail/Motion.cpp` | 시간 기반 `Motion` 단계 합성 및 `MotionSystem` 레이어 트리 |
| Layer 2 | `include/ModelLoader.h`, `detail/ObjLoader.cpp`, `include/Texture.h` | `.obj` 파싱(`AssetLoader::loadObj`) 및 Windows WIC 텍스처 디코딩 |
| Layer 2 | `include/Picking.h`, `include/RenderConfig.h` | 화면 광선 삼각형 피킹, 커스텀 정점 레이아웃/유니폼 설정 |
| Layer 1 | `include/Model.h`, `include/Geometry.h`, `include/Transform.h`, `detail/Transform.cpp`, `include/Dynamics.h`, `detail/Dynamics.cpp` | 핵심 데이터 구조, UV/법선/윤곽선 계산, 아핀/경로 수학, 비관성 기준프레임 역학 |
| Layer 0 | `include/Diagnostics.h`, `detail/Native.h`, `detail/Internal.h`, `detail/Shaders.h` | 오류 진단, 동적 GL/GLFW 로딩, 내부 공유 선언, 내장 GLSL |
