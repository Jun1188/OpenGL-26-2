#include "SourceIndex.h"
#include <iostream>
#include <exception>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

int wmain(int argc, wchar_t** argv) {
    SetConsoleOutputCP(CP_UTF8); SetConsoleCP(CP_UTF8);
    source_debugger::SourceIndex sources;
    std::cout << "Source debugger: 실제 디스크 소스의 편집 위치와 코드를 안내합니다.\n";
    if (argc == 2) {
        try { sources.project(std::filesystem::path(argv[1])); sources.refresh(); std::cout << "대상: " << sources.root().u8string() << '\n'; }
        catch (const std::exception& error) { std::cerr << "대상 오류: " << error.what() << '\n'; }
    } else if (argc > 2) { std::cerr << "사용법: debugger.exe [\"프로젝트 폴더 또는 .vcxproj\"]\n"; return 1; }
    source_debugger::help(std::cout);
    std::string line;
    while (std::cout << "dbg> " << std::flush, std::getline(std::cin, line)) {
        try { if (!source_debugger::command(sources, line, std::cout)) break; }
        catch (const std::exception& error) { std::cerr << "명령 거절: " << error.what() << '\n'; }
    }
    return 0;
}
