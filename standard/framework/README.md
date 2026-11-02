# 동봉 프레임워크 사용 안내

2026-11-02 기준 Windows x64 / C++17 / OpenGL 3.3 Core 배포본이다. 진입점 Framework.h는 Application·Patterns·Animation 공개 API를 포함한다.

1. [공통 구현·전체 API 사례](docs/API_REFERENCE.html): 콜백·렌더링 루프·행렬·CPU/GPU 업로드·자원 수명을 상세히 읽는다.
2. [응용패턴과 그네](docs/IMPLEMENTATION_PATTERNS.html): 흐름도·실제 응용 코드·현재 standard 원본을 함께 읽는다.
3. [공개 선언 색인](docs/API_KO.md), [착석·관절 SRT](docs/ANIMATION_API.md), [OBJ·텍스처](docs/OBJ_LOADING.md).
4. [시험 구현 순서](docs/EXAM_IMPLEMENTATION_ORDER.html), [사용자 확인](docs/USER_CHECKS.md).

Application이 Scene·Renderer와 창 수명을 소유한다. Scene의 ID와 비소유 포인터를 구분한다. Scene.clear 후 ID·rig·트랙을 다시 만든다. 새 관절 API의 모션 샘플은 강체이며 rest는 양수 균일 배율을 허용한다. 각 축의 비균일 배율은 자식 메시 크기 편집으로 분리한다.

텍스처 기능은 기본 꺼져 있다. OBJ만 지원하며 FBX/glTF·MTL 자동 로드·투영 그림자를 제공하지 않는다. 외부 의존성·라이선스는 원본을 유지한다. 소스·문서 작성과 실제 빌드·렌더링 확인은 구분하며 실행은 사용자에게 맡긴다.
