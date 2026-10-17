# 사용자 빌드·실행 확인

에이전트는 빌드와 테스트를 실행하지 않았다.

1. Visual Studio에서 Debug/Release x64를 빌드한다. C++17 이상과 /utf-8을 사용한다.
2. 실행 폴더의 framework/dependencies에 GLFW/Assimp DLL이 배치되는지 확인한다. 별도 SDK·NuGet 복원이 없어야 한다.
3. 실습 폴더 전체를 다른 위치로 복사해 다시 빌드한다. 실행 파일은 작업 폴더를 바꾸어 실행한다.
4. 그룹 계층, 구/원뿔, 면 힌지, 색상 배율과 피킹을 확인한다. 모델/텍스처 기능을 켜고 내부 상대경로 자산 로드와 외부 경로 거부를 확인한다.
5. 창 크기 변경·최소화·포커스 이탈·종료를 확인한다.

기존 API 변경: Transform3D.rotationDegrees/orientation 대신 rotation(mat3)을 사용한다. findShaderDirectory는 제거되었다. 기본 내장 셰이더를 사용하거나 상대 shaderDirectory를 지정한다.
