# 30MB 미만 소스 배포 ZIP

`python tools/Package-Framework.py`로 `dist/framework-samples.zip`을 생성한다. 다른 이름은 `--output dist/다른이름.zip`으로 지정한다. 기존 ZIP은 덮어쓰지 않는다. Python 표준 라이브러리만 사용한다. 빌드·테스트 명령이 아니다.

압축 후 파일 크기가 30,000,000바이트 이상이면 완성본으로 게시하지 않는다. 30MiB보다 엄격한 십진 30MB 기준이다. 실제 크기·SHA256은 ZIP 옆 manifest에 남긴다. 압축 내부의 DISTRIBUTION_MANIFEST.json은 포함된 원본 파일의 경로·바이트 수·SHA256을 기록한다.

포함: 루트 framework, standard, openGL_sample0, 현재 존재하는 실습 15~20, OpenGL.props, 설명 문서와 배포 스크립트. 프로젝트별 framework 사본과 GLFW·GLEW·GLM SDK를 그대로 보존한다. SDK 안의 Release/x64 디렉터리도 포함한다. 모델·텍스처·셰이더와 라이선스를 보존한다. 프로젝트의 현재 사용자 변경도 배포본에 포함된다.

제외: .git, .vs, x64/Debug/Release 빌드 출력(SDK 내부는 예외), packages 캐시, 실행 파일·PDB·중간 파일, 사용자별 IDE 설정, 그래프·강의 PDF·변경 전후 대형 보고서. 원본 폴더의 해당 파일을 삭제하지 않는다. OBJ 텍스트를 확장자만 보고 제외하지 않는다.

압축 크기는 기능 실행의 증거가 아니다. 사용자가 ZIP을 새 폴더에 풀고 각 .slnx를 x64 Debug/Release로 빌드·실행한다. standard의 기존 동작·레이어, 실습의 조작·렌더링·종료, OBJ 예제의 읽기·경고·오류를 직접 확인한다. openGL_sample0는 상위 OpenGL.props와 루트 framework가 함께 있어야 한다. 에이전트는 빌드·실행 테스트를 하지 않았다.
