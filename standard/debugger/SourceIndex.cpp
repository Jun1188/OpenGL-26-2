#include "SourceIndex.h"
#include <algorithm>
#include <fstream>
#include <optional>
#include <regex>
#include <stdexcept>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace source_debugger {
namespace {
bool excluded(const fs::path& path) {
    const auto name = path.filename().u8string();
    return name == ".git" || name == ".vs" || name == "x64" || name == "Debug" || name == "Release" ||
        name == "dependencies" || name == "docs" || name == "debugger" || name == "old_standard" || name == "graphify-out" ||
        name == ".agents" || name == ".obj-work" || name == "dist" || name == "packages";
}
bool source(const fs::path& path) {
    const auto ext = path.extension().u8string();
    return ext == ".cpp" || ext == ".h" || ext == ".hpp" || ext == ".glsl" || ext == ".props" || ext == ".vcxproj";
}
}
void SourceIndex::project(const fs::path& input) {
    auto path = fs::canonical(input);
    if (fs::is_regular_file(path)) {
        if (path.extension() != ".vcxproj") throw std::runtime_error("프로젝트 폴더 또는 .vcxproj를 지정하세요.");
        path = path.parent_path();
    }
    if (!fs::is_directory(path)) throw std::runtime_error("프로젝트 폴더가 아닙니다.");
    root_ = path;
}
void SourceIndex::refresh() {
    if (root_.empty()) throw std::runtime_error("project \"프로젝트 폴더 또는 .vcxproj 경로\"를 먼저 입력하세요.");
    std::vector<SourceFile> next;
    std::vector<fs::path> paths;
    for (fs::recursive_directory_iterator it(root_), end; it != end; ++it) {
        const auto status = it->symlink_status();
        const auto attributes = GetFileAttributesW(it->path().c_str());
        if (attributes == INVALID_FILE_ATTRIBUTES) throw std::runtime_error("소스 경로 속성을 읽지 못했습니다.");
        if (fs::is_symlink(status) || (attributes & FILE_ATTRIBUTE_REPARSE_POINT) || excluded(it->path())) {
            it.disable_recursion_pending(); continue;
        }
        if (fs::is_regular_file(status) && source(it->path())) paths.push_back(it->path());
    }
    std::sort(paths.begin(), paths.end());
    for (const auto& path : paths) {
        if (fs::file_size(path) > 8 * 1024 * 1024) throw std::runtime_error("소스 파일이 8 MiB를 초과합니다: " + path.u8string());
        std::ifstream input(path, std::ios::binary);
        if (!input) throw std::runtime_error("소스를 열 수 없습니다: " + path.u8string());
        SourceFile file{path, {}};
        std::string line;
        while (std::getline(input, line)) {
            if (!line.empty() && line.back() == '\r') line.pop_back();
            if (file.lines.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3);
            if (line.find('\0') != std::string::npos) throw std::runtime_error("UTF-8 텍스트 소스가 필요합니다: " + path.u8string());
            if (!line.empty() && MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, line.data(), int(line.size()), nullptr, 0) == 0)
                throw std::runtime_error("UTF-8로 읽을 수 없는 소스입니다: " + path.u8string());
            file.lines.push_back(line);
        }
        if (input.bad()) throw std::runtime_error("소스 읽기 실패: " + path.u8string());
        next.push_back(std::move(file));
    }
    std::vector<Block> found;
    const std::regex marker(R"(^\s*//\s*@debug\s+(begin|end)\s+(object|layer|light|camera|render)\s+([A-Za-z_][A-Za-z_0-9]*)\s*$)");
    for (std::size_t f = 0; f < next.size(); ++f) {
        std::optional<Block> open;
        for (std::size_t i = 0; i < next[f].lines.size(); ++i) {
            std::smatch match;
            if (!std::regex_match(next[f].lines[i], match, marker)) continue;
            if (match[1] == "begin") {
                if (open) throw std::runtime_error("중첩 @debug 구획: " + next[f].path.u8string());
                open = Block{f, i, i, match[2], match[3]};
            } else {
                if (!open || open->kind != match[2] || open->name != match[3])
                    throw std::runtime_error("짝이 맞지 않는 @debug end: " + next[f].path.u8string());
                open->last = i; found.push_back(*open); open.reset();
            }
        }
        if (open) throw std::runtime_error("닫히지 않은 @debug 구획: " + next[f].path.u8string());
    }
    files = std::move(next); blocks = std::move(found);
}
const Block& SourceIndex::block(const std::string& kind, const std::string& name) const {
    const Block* result = nullptr;
    for (const auto& item : blocks) if (item.kind == kind && item.name == name) {
        if (result) throw std::runtime_error("같은 구획이 여러 파일에 있습니다. project로 대상 폴더를 좁히세요.");
        result = &item;
    }
    if (!result) throw std::runtime_error("해당 @debug 구획이 없습니다. list/find를 사용하세요.");
    return *result;
}
const SourceFile& SourceIndex::file(const fs::path& input) const {
    const auto path = fs::canonical(input.is_absolute() ? input : root_ / input);
    for (const auto& item : files) if (fs::equivalent(item.path, path)) return item;
    throw std::runtime_error("현재 프로젝트에 색인한 소스 파일이 아닙니다.");
}
void SourceIndex::print(const SourceFile& file, std::size_t first, std::size_t last, std::ostream& out) const {
    if (first == 0 || first > last || last > file.lines.size()) throw std::runtime_error("현재 소스의 행 범위를 벗어났습니다.");
    out << file.path.u8string() << ':' << first << '-' << last << '\n';
    for (std::size_t i = first; i <= last; ++i) out << i << " | " << file.lines[i - 1] << '\n';
}
void proposal(const SourceFile& file, std::size_t first, std::size_t last,
              const std::vector<std::string>& after, const std::string& meaning, std::ostream& out) {
    if (first == 0 || first > file.lines.size() + 1 || last > file.lines.size() || last + 1 < first)
        throw std::runtime_error("제안할 행 범위가 현재 파일과 맞지 않습니다.");
    out << "[수정 제안] " << file.path.u8string() << ':' << first;
    if (last >= first && last != first) out << '-' << last;
    if (last < first) out << " 앞에 삽입";
    out << "\n[위치·의미] " << meaning << "\n[before]\n";
    if (last < first) out << "(없음: 새 코드 삽입)\n";
    else for (auto i = first; i <= last; ++i) out << i << " | " << file.lines[i - 1] << '\n';
    out << "[after: 복사할 C++ 코드]\n";
    for (const auto& line : after) out << line << '\n';
    out << "[주변 코드]\n";
    if (first > 1) out << first - 1 << " | " << file.lines[first - 2] << '\n';
    const auto following = last < first ? first : last + 1;
    if (following <= file.lines.size()) out << following << " | " << file.lines[following - 1] << '\n';
    out << "파일은 사용자가 직접 수정합니다. 다음 명령에서 디스크 소스를 다시 읽습니다.\n";
}
} // namespace source_debugger
