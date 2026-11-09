#include "Assembly.h"

namespace assembly {
void Assembly::help(std::ostream& out) const {
    out << "Standard 0: 마우스로 객체 선택. 명령어 [결과] - 인수 의미\n"
        << "add <종류> <이름> X Y Z [SX SY SZ] [객체 생성] - XYZ: 위치, S: 형상 크기\n"
        << "select <이름> [대상 선택] - 이름: 등록된 객체/부품\n"
        << "move <이름> DX DY DZ [상대 이동] - D: 이동 거리\n"
        << "rotate <이름> <축> <도> [회전] - 축: x/y/z, 도: 회전량\n"
        << "parent <자식> <부모/none> [상속 연결] - none: 연결 해제\n"
        << "pivot <이름> center [피벗 변경] - center: 형상 중심\n"
        << "spin <ID> <이름> <축> <도/초> [자전 추가] - ID: 동작 이름\n"
        << "orbit <ID> <이름> <축> <도/초> CX CY CZ [공전 추가] - C: 중심\n"
        << "bind <키> <press/hold> \"명령\" [입력 연결] - press: 1회, hold: 초당 이동/회전\n"
        << "pause <on/off> [시간 정지/재개] - on: 정지, off: 재개\n"
        << "save/load <파일> [레시피 저장/읽기] - 파일: 작업 폴더 기준 .assembly 경로\n"
        << "list [객체 목록] - 인수 없음\n"
        << "inspect/diagnose <이름> [객체 상태] - 이름: 조회 대상\n"
        << "help 또는 F12 [명령 도움말] - 인수 없음\n"
        << "clear [장면 비우기] - 인수 없음\n"
        << "quit [종료] - 인수 없음\n";
}
void Assembly::guide(const std::string&, std::ostream& out) const { help(out); }
} // namespace assembly
