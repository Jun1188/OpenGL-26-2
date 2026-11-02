# 3단계: 실습 23 콘솔 한글 출력
첫 cout 전에 Windows 콘솔 출력 코드 페이지를 UTF-8로 설정했다. 빌드·실행은 사용자가 수행한다.

## openGL_3dAssign_23/main.cpp
### before
```
#include "Console.h"
#include <iostream>
/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    try {
        graphics::Application app;Console practice(app);
        app.setInitHandler([&](graphics::Application&){return practice.initScene();});
        app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){practice.onKey(key,action,mods);});
        app.setUpdateHandler([&](graphics::Application&,float dt){practice.onUpdate(dt);});
        graphics::ApplicationConfig config;config.title="실습 23 - 로봇";config.width=1100;config.height=800;
        std::cout << "실습 23: WASD 방향, +/- 속도, J 점프, Space 보행 정지/재개, O 벽 열기/같은 벽 복원, I 초기화, x/X z/Z 카메라 이동, y/Y 공전, Q 종료\n";
        return app.run(config);
    } catch(const std::exception& error) {std::cerr<<"실습 23 실패: "<<error.what()<<'\n';return 1;}
}
```
### after
```
#include "Console.h"
#include <iostream>
#include <Windows.h>
/// 콜백을 등록하고 애플리케이션을 실행한다.
int main() {
    SetConsoleOutputCP(CP_UTF8);
    try {
        graphics::Application app;Console practice(app);
        app.setInitHandler([&](graphics::Application&){return practice.initScene();});
        app.setModifiedKeyHandler([&](graphics::Application&,int key,int action,int mods){practice.onKey(key,action,mods);});
        app.setUpdateHandler([&](graphics::Application&,float dt){practice.onUpdate(dt);});
        graphics::ApplicationConfig config;config.title="실습 23 - 로봇";config.width=1100;config.height=800;
        std::cout << "실습 23: WASD 방향, +/- 속도, J 점프, Space 보행 정지/재개, O 벽 열기/같은 벽 복원, I 초기화, x/X z/Z 카메라 이동, y/Y 공전, Q 종료\n";
        return app.run(config);
    } catch(const std::exception& error) {std::cerr<<"실습 23 실패: "<<error.what()<<'\n';return 1;}
}
```

## openGL_3dAssign_23/USER_CHECKS.md
### before
```
# 사용자 확인

1. openGL_assign_23.sln에서 Debug/Release x64 빌드·실행.
2. 코로 정면, 팔·다리 색으로 좌우를 구별한다. WASD 후 계속 보행하고 경계/장애물에서 반전하는지 확인.
3. +/-로 속도와 보행각 변화. Space 후 J 제자리 점프, 보행 중 J로 장애물 위 착지·걸어 내려오기 확인.
4. O를 누르면 선택한 벽 중심은 고정되고 가로·세로가 같은 비율로 줄어 사라지는지 확인한다. 다시 누르면 같은 벽이 원래 크기로 복원되어야 한다. 축소·복원 중 O를 눌러도 현재 크기에서 방향이 전환되어야 한다. y/Y 공전 시 컬링된 벽의 윤곽선이 남지 않고 내부 벽과 일반 물체 표면이 표시되는지 확인한다.
5. I 초기 위치·속도·문·장애물·카메라 복원, 창 최소화/리사이즈/DPI/포커스/종료 확인.

에이전트는 빌드·실행·테스트·검증기를 실행하지 않았다. ZIP은 사용자 요청에 따라 만들지 않았다.

추가 확인: I를 여러 번 눌러 새 장애물 배치와 비겹침을 확인한다. 카메라를 각 옆면으로 돌린 뒤 O를 누르고 Selected wall=0..3과 해당 벽의 축소/복원을 확인한다. 바닥·천장은 문 선택에서 제외한다. 카메라를 돌려도 이미 선택한 벽의 애니메이션 대상은 바뀌지 않는다. Front 컬링으로 가려진 벽은 외부 시점에서 축소가 잘 보이지 않을 수 있으므로 반대쪽 내부 면도 확인한다.
```
### after
```
# 사용자 확인

1. openGL_assign_23.sln에서 Debug/Release x64 빌드·실행.
2. 코로 정면, 팔·다리 색으로 좌우를 구별한다. WASD 후 계속 보행하고 경계/장애물에서 반전하는지 확인.
3. +/-로 속도와 보행각 변화. Space 후 J 제자리 점프, 보행 중 J로 장애물 위 착지·걸어 내려오기 확인.
4. O를 누르면 선택한 벽 중심은 고정되고 가로·세로가 같은 비율로 줄어 사라지는지 확인한다. 다시 누르면 같은 벽이 원래 크기로 복원되어야 한다. 축소·복원 중 O를 눌러도 현재 크기에서 방향이 전환되어야 한다. y/Y 공전 시 컬링된 벽의 윤곽선이 남지 않고 내부 벽과 일반 물체 표면이 표시되는지 확인한다.
5. I 초기 위치·속도·문·장애물·카메라 복원, 창 최소화/리사이즈/DPI/포커스/종료 확인.

에이전트는 빌드·실행·테스트·검증기를 실행하지 않았다. ZIP은 사용자 요청에 따라 만들지 않았다.

추가 확인: I를 여러 번 눌러 새 장애물 배치와 비겹침을 확인한다. 카메라를 각 옆면으로 돌린 뒤 O를 누르고 Selected wall=0..3과 해당 벽의 축소/복원을 확인한다. 바닥·천장은 문 선택에서 제외한다. 카메라를 돌려도 이미 선택한 벽의 애니메이션 대상은 바뀌지 않는다. Front 컬링으로 가려진 벽은 외부 시점에서 축소가 잘 보이지 않을 수 있으므로 반대쪽 내부 면도 확인한다.

## main 시작 안내문의 한글 출력

이 확인은 사용자가 수행한다. 수정 후 빌드·실행은 수행하지 않았다.

1. 실습 23을 x64로 다시 빌드하고 VS에서 실행한다.
2. 창 생성 전 안내문 `실습 23: WASD 방향`부터 한글이 정상으로 표시되어야 한다.
3. 초기 장면에서 난수 시드 안내와 오류 출력도 함께 확인한다.

Framework.props는 이미 /utf-8을 지정한다. main에서 첫 cout 전에 SetConsoleOutputCP(CP_UTF8)를 호출한다.
Application::run 내부의 기존 설정만으로는 run 이전의 출력에 적용되지 않았다.
```
