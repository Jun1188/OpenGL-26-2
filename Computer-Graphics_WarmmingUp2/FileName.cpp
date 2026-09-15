#include "Application.h"
#include "Console.h"
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
std::string toUtf8(const wchar_t* text) {
    const int bytes = WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text, -1, nullptr, 0, nullptr, nullptr);
    if (bytes == 0) throw std::runtime_error("Invalid Unicode command-line argument.");
    std::string result(static_cast<std::size_t>(bytes), '\0');
    if (WideCharToMultiByte(CP_UTF8, WC_ERR_INVALID_CHARS, text, -1, result.data(), bytes, nullptr, nullptr) == 0)
        throw std::runtime_error("Command-line encoding conversion failed.");
    result.pop_back();
    return result;
}

std::filesystem::path outputDirectory() {
    std::wstring buffer(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, buffer.data(), static_cast<DWORD>(buffer.size()));
    if (length == 0 || length == buffer.size())
        return std::filesystem::current_path() / "outout-6";
    buffer.resize(length);
    std::filesystem::path directory = std::filesystem::path(buffer).parent_path();
    for (;;) {
        std::error_code error;
        if (std::filesystem::exists(directory / "Computer-Graphics_WarmmingUp2.vcxproj", error) && !error)
            return directory / "outout-6";
        const std::filesystem::path parent = directory.parent_path();
        if (parent == directory || parent.empty()) break;
        directory = parent;
    }
    return std::filesystem::path(buffer).parent_path() / "outout-6";
}
}

int wmain(int argc, wchar_t* argv[]) {
    try {
        warmup::Console console;
        std::vector<std::string> arguments;
        for (int i = 1; i < argc; ++i) arguments.push_back(toUtf8(argv[i]));
        warmup::Application application(&console, outputDirectory());
        return application.run(arguments);
    } catch (const std::exception& error) {
        std::cerr << "Application error: " << error.what() << '\n';
        return 1;
    }
}
