# Modern OpenGL C++ framework

이 폴더 전체를 복사하고 Framework.props를 프로젝트에서 import한다. C++17·Windows x64·Visual Studio C++ 도구와 Windows SDK가 필요하다. 헤더·라이브러리·런타임 DLL·셰이더·라이선스는 폴더 안의 배포 사본을 사용한다.

현재 작성 방식은 [SceneAuthoring C++ API](docs/SOURCE_AUTHORING.md)다. ObjectSetup·LayerSetup·PointLight·Camera 값을 소스에서 직접 작성한다. 프레임워크에 명령 파서나 assembly 파일 실행 의존성을 추가하지 않는다. 낮은 수준의 Scene·TransformSystem·PatternSystem·Motion·Dynamics API도 그대로 직접 사용할 수 있다.

편집 보조 도구는 Standard 폴더 내부의 독립 debugger 프로젝트다. framework는 debugger를 include/import/link하거나 실행하지 않는다. debugger 소스와 실행 파일이 없어도 framework를 사용하는 프로젝트의 빌드/실행 계약은 같다.

[기존 공통 API 레퍼런스](docs/API_REFERENCE.html)는 계속 제공한다. 과거 Standard 명령·레시피·모드 설명은 이전 버전 자료이며 저장소의 old_standard에 보존한다. 현재 Standard의 작성 흐름은 SceneSetup.cpp와 새 문서를 따른다. 기존 명령 예제가 새 Standard에서 실행된다고 간주하지 않는다.

조명은 기존 방향광·점광원·독립 주변광·재질 Ka/Kd/Ks/shininess와 법선 변환을 사용한다. 추가 개념은 사용자가 지정한 ComputerGraphics 저장소의 해당 소스를 확인한 뒤 반영한다. 빌드·실행·화면 확인은 사용자가 수행한다.
