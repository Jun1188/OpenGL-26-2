#include "AssemblyConsole.h"
#include <algorithm>
#include <iostream>

namespace assembly {
ConsoleInput::ConsoleInput() {
    input_ = GetStdHandle(STD_INPUT_HANDLE);
    output_ = GetStdHandle(STD_OUTPUT_HANDLE);
    if (input_ != INVALID_HANDLE_VALUE && input_ && GetConsoleMode(input_, &originalMode_)) {
        const DWORD mode = (originalMode_ | ENABLE_EXTENDED_FLAGS) &
            ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_QUICK_EDIT_MODE);
        console_ = SetConsoleMode(input_, mode) != 0;
    }
}

ConsoleInput::~ConsoleInput() {
    if (console_) {
        SetConsoleMode(input_, originalMode_);
        std::cout << '\n';
    }
}

void ConsoleInput::prompt() {
    if (!console_) return;
    std::cout << "asm> " << std::flush;
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(output_, &info)) origin_ = info.dwCursorPosition;
    echoed_ = 0;
}

void ConsoleInput::redraw() {
    DWORD written = 0;
    if (!SetConsoleCursorPosition(output_, origin_)) return;
    FillConsoleOutputCharacterW(output_, L' ', echoed_, origin_, &written);
    WriteConsoleW(output_, line_.data(), DWORD(line_.size()), &written, nullptr);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (GetConsoleScreenBufferInfo(output_, &info)) {
        const int count = (info.dwCursorPosition.Y - origin_.Y) * info.dwSize.X + info.dwCursorPosition.X - origin_.X;
        echoed_ = DWORD(std::max(0, count));
    }
}

std::optional<std::string> ConsoleInput::poll() {
    if (console_) {
        for (unsigned processed = 0; processed < 128; ++processed) {
            DWORD available = 0;
            if (!GetNumberOfConsoleInputEvents(input_, &available) || available == 0) return {};
            INPUT_RECORD record{};
            DWORD read = 0;
            if (!ReadConsoleInputW(input_, &record, 1, &read) || read == 0) return {};
            if (record.EventType != KEY_EVENT || !record.Event.KeyEvent.bKeyDown) continue;
            const auto& key = record.Event.KeyEvent;
            if (key.wVirtualKeyCode == VK_RETURN) {
                std::cout << '\n';
                const int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, line_.data(), int(line_.size()), nullptr, 0, nullptr, nullptr);
                std::string command(std::max(0, bytes), '\0');
                if (bytes > 0) WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, line_.data(), int(line_.size()), command.data(), bytes, nullptr, nullptr);
                line_.clear();
                return command;
            }
            if (key.wVirtualKeyCode == VK_ESCAPE) line_.clear();
            else if (key.wVirtualKeyCode == VK_BACK && !line_.empty()) {
                const auto last = line_.back();
                line_.pop_back();
                if (last >= 0xDC00 && last <= 0xDFFF && !line_.empty() && line_.back() >= 0xD800 && line_.back() <= 0xDBFF)
                    line_.pop_back();
            } else if (key.uChar.UnicodeChar >= 32 && line_.size() < 2048) {
                for (WORD repeat = 0; repeat < key.wRepeatCount && line_.size() < 2048; ++repeat)
                    line_ += key.uChar.UnicodeChar;
            }
            redraw();
        }
        return {};
    }
    const auto newline = redirected_.find('\n');
    if (newline != std::string::npos) {
        auto command = redirected_.substr(0, newline);
        redirected_.erase(0, newline + 1);
        if (!command.empty() && command.back() == '\r') command.pop_back();
        if (command.compare(0, 3, "\xEF\xBB\xBF") == 0) command.erase(0, 3);
        return command;
    }
    if (eof_) {
        if (redirected_.empty()) return {};
        auto command = std::move(redirected_);
        redirected_.clear();
        return command;
    }
    if (!input_ || input_ == INVALID_HANDLE_VALUE) { eof_ = true; return {}; }
    const DWORD type = GetFileType(input_);
    DWORD available = 0;
    if (type == FILE_TYPE_PIPE) {
        if (!PeekNamedPipe(input_, nullptr, 0, nullptr, &available, nullptr)) { eof_ = true; return {}; }
        if (available == 0) return {};
    } else if (type != FILE_TYPE_DISK) return {};
    char buffer[512];
    DWORD read = 0;
    const DWORD count = type == FILE_TYPE_PIPE ? std::min<DWORD>(available, DWORD(sizeof(buffer))) : DWORD(sizeof(buffer));
    if (!ReadFile(input_, buffer, count, &read, nullptr) || read == 0) { eof_ = true; return {}; }
    redirected_.append(buffer, read);
    if (redirected_.size() > 65536) {
        redirected_.clear();
        std::cout << "표준 입력 행이 너무 깁니다. 4096바이트 이하 명령을 사용하세요.\n";
    }
    return {};
}
} // namespace assembly
