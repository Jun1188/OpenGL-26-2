# 사용자가 수행할 빌드와 실행 확인

작성자는 빌드, 링크, OpenGL 실행, 동작 테스트를 수행하지 않았다. AST 구문 분석,
DLL 내보내기 함수와 내부 include 경로 확인은 실행 성공의 증거가 아니다.

## 프로젝트 설정

1. Windows x64의 빈 C++ 프로젝트를 만든다. C++17 이상을 선택한다.
2. 완전한 framework 폴더를 프로젝트 안으로 복사한다. 기존 저장소 과제는 수정하지 않는다.
3. 사용자 main에서 `#include "framework/Framework.h"`만 사용한다.
4. framework/src의 .cpp, Framework.props, 외부 include/LIB 디렉터리를 등록하지 않는다.
5. examples/basic.cpp를 main으로 참고해 Debug와 Release를 사용자가 직접 빌드한다.
6. 다른 .cpp에서도 Framework.h를 include해 정의 중복·링크 오류 여부를 확인한다.

## 기본 도형과 부모·면 SRT

- 기본 설정에서 직육면체를 표시한다. Assimp·이미지 파일을 지정하지 않는다.
- 삼각형 내부 대각선이 없는 경계를 확인한다. AllTriangleEdges로 변경하면 대각선도 보인다.
- `splitFace()`만 호출한 직후 면의 위치와 모양이 바뀌지 않는지 확인한다.
- 부모의 비균일 scale, 세 축 rotation, translation을 설정한다.
- 면의 localBasis를 면 중심으로 설정하고 추가 SRT를 적용한다. 부모 SRT가 남아 있어야 한다.
- 면 추가 SRT를 유지한 채 부모를 다시 움직인다. 분리한 면도 부모를 따라야 한다.
- setVertexPosition, setTriangleIndices, setTexcoords, setMaterial 이후 화면 갱신을 확인한다.
- 잘못된 인덱스, NaN 좌표, 텍스처와 UV 개수 불일치 입력은 false를 반환하며 장면이 유지되어야 한다.
- setSideLengths는 메시/재질/UV를 유지한다. 길이가 0인 축을 가진 평면에는 사용하지 않는다.

## 경로와 모드

- 기본 features에서 loadModel과 loadTexture 호출은 모드 비활성 오류를 반환해야 한다.
- 기본 모드에서는 Assimp DLL을 로드하지 않는다. 디버거의 모듈 목록으로 확인한다.
- projectRoot를 실제 프로젝트 루트로 지정하고, 그 아래 상대경로로 모델·텍스처를 전달한다.
- 한글 프로젝트 폴더와 다른 실행 작업 폴더에서 같은 명시적 projectRoot로 확인한다.
- absolute 경로와 projectRoot 밖으로 나가는 상대경로는 오류여야 한다.
- framework/examples/data/cube.txt를 기본 도형 로더로 읽는다.
  원본은 정점 8개·UV 4개·삼각형 12개이며, 기본 병합 후 논리적 면 메시 6개를 기대한다.
- 하단 중점은 (0,-0.5,0)이다. 이 값을 localBasis로 사용하면 월드 basis가 도형의 바닥 중점이 된다.
- 모델 모드를 켜고 OBJ, glTF 등 사용자의 모델 파일을 로드한다. 노드 배치와 재질을 확인한다.
- 모델의 뼈대 애니메이션은 이 정적 모델 로더의 범위에 포함되지 않는다.

## 텍스처

- textures를 켠 뒤 PNG/JPEG/BMP 등 Windows WIC 지원 이미지를 loadTexture로 읽는다.
- RGB/RGBA와 폭이 4바이트 정렬에 맞지 않는 이미지를 확인한다.
- flipTextureRows와 repeatTexture 옵션을 바꿔 UV 방향·반복 처리를 확인한다.
- 모델이 참조하는 이미지와 임베디드 이미지를 각각 확인한다.
- WIC가 지원하지 않는 이미지 형식은 디코딩 오류를 반환한다. RGBA TextureImage 데이터로 직접 지정할 수 있다.
- TextureImage는 불변 공유 데이터로 취급한다. 변경 시 새 이미지와 setMaterial을 사용한다.

## 피킹과 자원 수명

- 왼쪽 클릭 시 실제 선택한 객체/면/삼각형 ID가 출력되어야 한다.
- 겹친 객체 중 가장 가까운 면을 반환하는지 확인한다. 숨긴 객체와 면은 제외되어야 한다.
- 원근·직교 카메라, 회전/비균일 scale/분리한 면, 창 크기, Windows 배율 100%/150%에서 확인한다.
- near/far 영역 밖, 창 밖, 최소화 상태의 클릭은 선택 결과가 없어야 한다.
- 일반 선·축 자체와 투명 픽셀은 별도 판정하지 않는다. 피킹은 표시 상태의 삼각형 기하를 대상으로 한다.
- 창을 닫고 Application::run을 다시 호출해 자원 재초기화를 확인한다.
- 콜백 예외 후 GPU 자원이 정리되는지 확인한다. 동시에 두 Application 루프를 실행하지 않는다.
- 배포 시 실행 파일 옆에 framework 폴더를 둔다. 원본 소스 위치가 없는 다른 위치에서 다시 확인한다.
- standalone Renderer는 해당 GL 컨텍스트를 current로 설정한 뒤 initialize/render/destroy를 호출한다.

## 결과 기록

실제 사용한 MSVC 버전, Debug/Release, 모델·이미지 파일, 창 배율과 결과를 기록한다.
실패 시 빌드 로그 또는 오류 문자열과 해당 호출을 전달한다. 실행 확인 전 성공으로 기록하지 않는다.
