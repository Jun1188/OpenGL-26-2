#include "Assembly.h"
#include <fstream>
#include <iomanip>
#include <sstream>

namespace assembly {
bool Assembly::save(const std::string& path, std::ostream& out) const {
    const auto file = std::filesystem::absolute(std::filesystem::u8path(path));
    auto temporary = file;
    temporary += L".saving-" + std::to_wstring(GetCurrentProcessId()) + L"-" + std::to_wstring(GetTickCount64());
    try {
        std::ofstream stream(temporary, std::ios::binary | std::ios::trunc);
        require(bool(stream), "레시피 저장 파일을 열 수 없습니다.");
        stream << "\xEF\xBB\xBF# standard assembly recipe v1\nclear\n" << std::setprecision(9);
        for (const auto& line : state_.recipe)
            stream << line << '\n';
        stream << "# @state-begin\n";
        for (const auto& item : state_.nodes) {
            const auto& t = item.second.rest;
            stream << (item.second.affineGroup ? "rest3 " : "rest ") << quote(item.first) << ' ' << t.basis.x << ' ' << t.basis.y << ' ' << t.basis.z;
            for (int c = 0; c < 3; ++c)
                for (int r = 0; r < 3; ++r)
                    stream << ' ' << t.rotation[c][r];
            stream << ' ' << t.scale.x;
            if (item.second.affineGroup) stream << ' ' << t.scale.y << ' ' << t.scale.z;
            stream << ' ' << t.localBasis.x << ' ' << t.localBasis.y << ' ' << t.localBasis.z << '\n';
        }
        const auto& c = app_.getCamera();
        const auto& r = app_.getRenderOptions();
        stream << "camera pose " << c.position.x << ' ' << c.position.y << ' ' << c.position.z << ' '
               << c.target.x << ' ' << c.target.y << ' ' << c.target.z << ' ' << c.up.x << ' ' << c.up.y << ' ' << c.up.z << '\n'
               << "camera projection " << (c.orthographic ? "ortho" : "perspective") << '\n'
               << "camera extent " << c.orthographicHalfExtent << "\ncamera fov " << c.fieldOfViewDegrees
               << "\ncamera clip " << c.nearPlane << ' ' << c.farPlane << '\n'
               << "render wire " << (r.wireframe ? "on" : "off") << "\nrender lighting " << (r.lighting ? "on" : "off")
               << "\nrender depth " << (r.depthTest ? "on" : "off") << "\nview " << (state_.threeViews ? 3 : 1)
               << "\npause " << (state_.paused ? "on" : "off") << '\n'
               << "render cull " << (r.cull == graphics::CullMode::Front ? "front" : r.cull == graphics::CullMode::Back ? "back" : "none") << '\n'
               << "render light-direction " << r.lightDirection.x << ' ' << r.lightDirection.y << ' ' << r.lightDirection.z << '\n'
               << "render light-color " << r.lightColor.red << ' ' << r.lightColor.green << ' ' << r.lightColor.blue << '\n'
               << "render background " << r.background.r << ' ' << r.background.g << ' ' << r.background.b << '\n';
        stream.flush();
        require(bool(stream), "레시피 쓰기에 실패했습니다.");
        stream.close();
        require(bool(stream) && std::filesystem::file_size(temporary) <= 2 * 1024 * 1024, "저장 결과가 2MiB 한도를 넘거나 쓰기를 완료하지 못했습니다.");
        require(MoveFileExW(temporary.c_str(), file.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) != 0,
                "저장 파일 교체 실패. 기존 파일을 보존했습니다.");
        out << "조립 레시피 저장: " << file.u8string() << " (물리/애니메이션 현재 프레임의 스냅샷은 아님)\n";
        return true;
    } catch (...) {
        std::error_code ignored;
        std::filesystem::remove(temporary, ignored);
        throw;
    }
}

bool Assembly::load(const std::string& path, std::ostream& out) {
    const auto requested = std::filesystem::u8path(path);
    auto file = std::filesystem::absolute(requested);
    if (!requested.is_absolute() && !std::filesystem::exists(file)) {
        std::wstring binary(32768, L'\0');
        const DWORD count = GetModuleFileNameW(nullptr, binary.data(), DWORD(binary.size()));
        if (count > 0 && count < binary.size()) {
            binary.resize(count);
            const auto deployed = std::filesystem::path(binary).parent_path() / requested;
            if (std::filesystem::is_regular_file(deployed)) file = deployed;
        }
    }
    require(std::filesystem::file_size(file) <= 2 * 1024 * 1024, "레시피는 2MiB 이하입니다.");
    std::ifstream stream(file, std::ios::binary);
    require(bool(stream), "레시피 파일을 열 수 없습니다.");
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (lines.empty() && line.compare(0, 3, "\xEF\xBB\xBF") == 0)
            line.erase(0, 3);
        require(line.size() <= 4096 && lines.size() < 10000, "명령은 4096바이트 이하, 레시피는 10000줄 미만입니다.");
        lines.push_back(line);
    }
    require(!stream.bad(), "레시피 읽기 실패.");
    clear();
    replaying_ = true;
    std::vector<std::string> authored;
    bool snapshot = false;
    try {
        for (std::size_t i = 0; i < lines.size(); ++i) {
            if (lines[i] == "# @state-begin") snapshot = true;
            if (!snapshot) authored.push_back(lines[i]);
            const auto tokens = tokenize(lines[i]);
            require(tokens.empty() || (tokens[0] != "save" && tokens[0] != "load"), "레시피 안의 save/load는 허용하지 않습니다.");
            require(execute(lines[i], out), "레시피 " + std::to_string(i + 1) + "줄 적용 실패.");
        }
        state_.recipe = std::move(authored);
        replaying_ = false;
        out << "조립 레시피 불러오기: " << file.u8string() << '\n';
        return true;
    } catch (...) {
        replaying_ = false;
        throw;
    }
}
} // namespace assembly
