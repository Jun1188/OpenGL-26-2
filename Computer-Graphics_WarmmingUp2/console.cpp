#include "Console.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <iostream>

namespace warmup {
Console::Console() {
    handle_ = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO info{};
    available_ = GetConsoleScreenBufferInfo(handle_, &info) != 0;
    if (available_) originalAttributes_ = info.wAttributes;
    originalInputPage_ = GetConsoleCP();
    originalOutputPage_ = GetConsoleOutputCP();
    if (originalInputPage_ != 0) SetConsoleCP(CP_UTF8);
    if (originalOutputPage_ != 0) SetConsoleOutputCP(CP_UTF8);
}

Console::~Console() noexcept {
    if (available_) SetConsoleTextAttribute(handle_, originalAttributes_);
    if (originalInputPage_ != 0) SetConsoleCP(originalInputPage_);
    if (originalOutputPage_ != 0) SetConsoleOutputCP(originalOutputPage_);
}

void Console::setHighlight(bool highlighted) {
    if (!available_ || highlighted_ == highlighted) return;
    std::cout.flush();
    constexpr WORD foregroundMask = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    const WORD attributes = highlighted
        ? static_cast<WORD>((originalAttributes_ & ~foregroundMask) | FOREGROUND_RED | FOREGROUND_INTENSITY)
        : originalAttributes_;
    if (SetConsoleTextAttribute(handle_, attributes)) highlighted_ = highlighted;
}

void Console::clear() {
    if (!available_) return;
    setHighlight(false);
    std::cout.flush();
    CONSOLE_SCREEN_BUFFER_INFO info{};
    if (!GetConsoleScreenBufferInfo(handle_, &info)) return;
    DWORD written = 0;
    const DWORD count = static_cast<DWORD>(info.dwSize.X) * info.dwSize.Y;
    FillConsoleOutputCharacterW(handle_, L' ', count, {0, 0}, &written);
    FillConsoleOutputAttribute(handle_, originalAttributes_, count, {0, 0}, &written);
    SetConsoleCursorPosition(handle_, {0, 0});
}
}
