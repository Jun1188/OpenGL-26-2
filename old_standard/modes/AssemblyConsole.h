#pragma once
#include "../framework/Framework.h"

namespace assembly {
class ConsoleInput {
    HANDLE input_ = INVALID_HANDLE_VALUE, output_ = INVALID_HANDLE_VALUE;
    DWORD originalMode_ = 0;
    bool console_ = false, eof_ = false;
    std::wstring line_;
    std::string redirected_;
    COORD origin_{};
    DWORD echoed_ = 0;
    void redraw();
public:
    ConsoleInput();
    ~ConsoleInput();
    ConsoleInput(const ConsoleInput&) = delete;
    ConsoleInput& operator=(const ConsoleInput&) = delete;
    std::optional<std::string> poll();
    void prompt();
};
} // namespace assembly
