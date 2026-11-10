#pragma once
#include <filesystem>
#include <ostream>
#include <string>
#include <vector>

namespace source_debugger {
namespace fs = std::filesystem;
struct SourceFile { fs::path path; std::vector<std::string> lines; };
struct Block { std::size_t file, first, last; std::string kind, name; };
class SourceIndex {
    fs::path root_;
public:
    std::vector<SourceFile> files;
    std::vector<Block> blocks;
    void project(const fs::path& path);
    const fs::path& root() const { return root_; }
    void refresh();
    const Block& block(const std::string& kind, const std::string& name) const;
    const SourceFile& file(const fs::path& path) const;
    void print(const SourceFile& source, std::size_t first, std::size_t last, std::ostream& out) const;
};
void proposal(const SourceFile& file, std::size_t first, std::size_t last,
              const std::vector<std::string>& after, const std::string& meaning, std::ostream& out);
bool command(SourceIndex& sources, const std::string& line, std::ostream& out);
void help(std::ostream& out);
} // namespace source_debugger
