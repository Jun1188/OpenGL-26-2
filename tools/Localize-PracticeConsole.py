"""승인된 실습 안내와 공통 실행 진단 문구를 한글로 저장한다."""
from pathlib import Path
import os
import re
ROOT=Path(__file__).resolve().parent.parent
TRANSLATIONS={
'Practice 22: same Scene, perspective / XZ top / XY front':'실습 22: 같은 장면을 원근 / XZ 평면도 / XY 정면도로 표시',
'arrows move tank; d lower height; t central spin; l upper swap':'방향키 이동; d 하부 높이; t 중앙 자전; l 상부 교환',
'g guns opposite yaw; p poles opposite pitch; x/X,z/Z camera translation':'g 포신 반대 회전; p 깃대 반대 기울기; x/X,z/Z 카메라 이동',
'y/Y camera yaw, r/R camera orbit (repeat pauses); O/o colors; c reset; q quit':'y/Y 카메라 시선 회전; r/R 공전(재입력 정지); O/o 색 변경; c 초기화; q 종료',
'Cannot add tank box.':'탱크 상자 생성 실패: 크기·위치·부모 ID를 확인하세요.',
'Cannot create tank root.':'탱크 루트 생성 실패.',
'Cannot create central joint.':'중앙 관절 생성 실패.',
'Cannot create ground group.':'바닥 그룹 생성 실패.',
'Cannot create upper joint.':'상부 관절 생성 실패.',
'Cannot create gun body-center pivot.':'포신 중심 피벗 생성 실패.',
'Cannot create pole base pivot.':'깃대 밑점 피벗 생성 실패.',
'Cannot color tank part.':'탱크 부품 색상 적용 실패.',
'Invalid tank position.':'탱크 위치 적용 실패.',
'Invalid central joint transform.':'중앙 관절 변환 적용 실패.',
'Invalid tank part transform.':'탱크 부품 변환 적용 실패.',
'Cannot update camera.':'카메라 갱신 실패: 위치·시선·회전축을 확인하세요.',
'Cannot create three camera views.':'세 카메라 뷰 생성 실패: 프레임버퍼 크기와 카메라를 확인하세요.',
'Practice 22 failed: unknown exception.':'실습 22 실패: 알 수 없는 예외.',
'Practice 22 failed: ':'실습 22 실패: ',
'OpenGL Practice 22 - Perspective / XZ / XY':'실습 22 - 원근 / XZ / XY',
'Practice23 random seed=':'실습 23 난수 시드=',
'; reset continues the random engine':'; 초기화 시 기존 난수 엔진의 다음 수열을 사용합니다.',
'Practice23 camera move failed.':'실습 23 카메라 이동 실패.',
'Practice23 camera orbit failed.':'실습 23 카메라 공전 실패.',
'Practice23 reset failed.':'실습 23 초기화 실패: 앞선 API 오류를 확인하세요.',
'Practice23 character collision failed.':'실습 23 캐릭터 충돌 계산 실패.',
'Practice23 rig pose failed.':'실습 23 로봇 관절 자세 적용 실패.',
'Practice23 door failed.':'실습 23 벽 개방·복원 실패.',
'Practice23 - Robot':'실습 23 - 로봇',
'23: WASD direction, +/- speed, J jump, Space pause walk, O door, I reset, x/X z/Z camera move, y/Y orbit, Q quit':'실습 23: WASD 방향, +/- 속도, J 점프, Space 보행 정지/재개, O 벽 열기/같은 벽 복원, I 초기화, x/X z/Z 카메라 이동, y/Y 공전, Q 종료',
'Missing window or context used from a different thread.':'창이 없거나 컨텍스트를 생성한 스레드와 다른 스레드에서 사용했습니다.',
'Keep window creation, rendering and shutdown on the owning thread; inspect Application::run and the custom render handler.':'창 생성·렌더링·종료를 같은 스레드에서 수행하세요. Application::run과 사용자 렌더 콜백을 확인하세요.',
'WGL did not attach the requested context; rendering stopped before GL commands.':'WGL이 요청한 컨텍스트를 연결하지 못하여 GL 명령 전에 렌더링을 중단했습니다.',
'Source is the failed bind call below. Check window lifetime, render-thread ownership and external handlers that detach/destroy the context. If those are valid, inspect the GPU driver/session; this log alone cannot prove a driver cause.':'소스 위치는 연결 실패 호출입니다. 창 수명·렌더 스레드·컨텍스트를 해제/파괴하는 외부 콜백을 먼저 확인하세요. 이상이 없으면 GPU 드라이버·세션을 확인하세요. 이 로그만으로 드라이버 원인을 확정할 수 없습니다.',
'Context binding failed at Source/Operation. Inspect Application context lifetime/thread first; the OS/driver reason is in the original message. Render passes do not need separate WGL contexts.':'표시된 소스 위치·호출 경로에서 컨텍스트 연결이 실패했습니다. Application의 창 수명과 스레드를 먼저 확인하세요. OS/드라이버 원문도 함께 보세요. 렌더 패스마다 별도 WGL 컨텍스트를 만들 필요는 없습니다.',
'Window width and height must be positive.':'창 너비와 높이는 양수여야 합니다.',
'GLFW initialization failed; see the GLFW error above.':'GLFW 초기화 실패: 앞선 GLFW 오류를 확인하세요.',
'Could not create an OpenGL 3.3 core window.':'OpenGL 3.3 코어 창 생성 실패.',
'Check the GLFW error above and update the graphics driver.':'앞선 GLFW 오류와 그래픽 드라이버의 OpenGL 지원을 확인하세요.',
'No active window/context.':'활성 창 또는 컨텍스트가 없습니다.',
'Recursive run() is not allowed.':'run() 재귀 호출은 허용하지 않습니다.',
'Only one active Application is supported.':'동시에 실행 중인 Application은 하나만 지원합니다.',
'Application initialization failed.':'애플리케이션 초기화 실패.',
'User initialization returned false.':'사용자 초기화 콜백이 false를 반환했습니다.',
'Read the preceding API error; inspect the scene creation callback.':'앞선 API 오류와 장면 생성 콜백을 확인하세요.',
'User scene initialization failed.':'사용자 장면 초기화 실패.',
'Motion update failed.':'모션 갱신 실패.',
'Rendering failed.':'렌더링 실패.',
'Read the preceding stage/API error; user callback exceptions are rethrown after cleanup.':'앞선 파이프라인 단계/API 오류를 확인하세요. 사용자 콜백 예외는 자원 정리 후 다시 전달됩니다.',
'Non-standard exception in a callback or runtime operation.':'콜백 또는 실행 처리에서 표준 예외가 아닌 예외가 발생했습니다.',
'AssetLoader is available during run callbacks only.':'AssetLoader는 run 실행 중 콜백에서만 사용할 수 있습니다.',
'Inspect the source location and this API\'s input contract.':'소스 위치와 해당 API의 입력 조건을 확인하세요.',
'Check this API\'s input contract in framework/docs/APPLICATION_API.md.':'framework/docs/API_KO.md에서 해당 API의 입력 조건과 반환값을 확인하세요.',
'unknown error':'알 수 없는 오류',
'unknown':'알 수 없음',
'begin\\n':'시작\\n',
' ERROR':' 오류',
' object=':' 객체=',
'\\n  Action: ':'\\n  조치: ',
'  Source: ':'  소스: ',
'\\n  Operation: ':'\\n  호출 경로: ',
}
FILES=['openGL_3dAssign_22/Console.cpp','openGL_3dAssign_22/main.cpp','openGL_3dAssign_23/Console.cpp','openGL_3dAssign_23/main.cpp','framework/detail/Application.inl','framework/include/Diagnostics.h']
# 주석·문자 리터럴은 건너뛰고 일반 C++ 문자열 리터럴만 번역한다.
TOKENS=re.compile(r'(?:u8|u|U|L)?R"(?P<delimiter>[^ ()\\\t\r\n]{0,16})\([\s\S]*?\)(?P=delimiter)"|//[^\n]*|/\*[\s\S]*?\*/|(?:u8|u|U|L)?\'(?:\\.|[^\'\\])*\'|(?P<string>(?:u8|u|U|L)?"(?:\\.|[^"\\])*")')
def localize(source):
    def replace(match):
        literal=match.group('string')
        if literal is None:return match[0]
        for old,new in TRANSLATIONS.items():literal=literal.replace(old,new)
        return literal
    return TOKENS.sub(replace,source)
if __name__=='__main__':
    for file in FILES:
        p=ROOT/file;s=localize(p.read_text(encoding='utf-8-sig'))
        if s!=p.read_text(encoding='utf-8-sig'):
            temp=p.with_name(p.name+'.localize.tmp')
            temp.write_text(s,encoding='utf-8-sig',newline='\n');os.replace(temp,p)
