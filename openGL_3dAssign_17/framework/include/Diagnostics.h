#pragma once
#include <cstdint>
#include <iostream>
#include <optional>
#include <string>

namespace graphics {
enum class PipelineStage { Runtime, Input, Scene, Motion, VertexInput, Shader, Transform, Raster, Framebuffer, Asset };
struct DiagnosticOptions { bool errors = true; bool trace = false; };
struct Diagnostic {
    PipelineStage stage = PipelineStage::Runtime;
    std::string api, cause, action;
    std::optional<std::uint64_t> object;
    std::string file, operation;
    int line=0;
};
class Diagnostics {
    static DiagnosticOptions& options() { static DiagnosticOptions value; return value; }
    static std::optional<Diagnostic>& issue() { static std::optional<Diagnostic> value; return value; }
public:
    /// [API] 진단에 첨부할 현재 파일·줄·호출 경로 문맥의 참조를 반환한다.
    static Diagnostic& context() { static Diagnostic value; return value; }
    /// [API] 다음 진단에 사용할 소스 파일·줄 번호·호출 경로를 설정한다.
    static void locate(const char* file,int line,const char* operation) {context().file=file;context().line=line;context().operation=operation;}
    /// [API] 주어진 소스 위치와 API 오류를 기록·출력하고 이전 진단 문맥을 복원한 뒤 false를 반환한다.
    static bool failAt(const char* file,int line,PipelineStage stage,const char* api,const std::string& cause,
                       const std::string& action="소스 위치와 해당 API의 입력 조건을 확인하세요.",
                       std::optional<std::uint64_t> object=std::nullopt) {
        const auto saved=context();locate(file,line,api);
        if(!saved.operation.empty()) context().operation=saved.operation+" -> "+api;
        const bool result=fail(stage,api,cause,action,object);context()=saved;return result;
    }
    /// [API] 파이프라인 단계 열거값에 대응하는 로그용 문자열을 반환한다.
    static const char* stageName(PipelineStage stage) {
        switch (stage) {
        case PipelineStage::Runtime: return "runtime";
        case PipelineStage::Input: return "input";
        case PipelineStage::Scene: return "scene";
        case PipelineStage::Motion: return "motion";
        case PipelineStage::VertexInput: return "vertex-input";
        case PipelineStage::Shader: return "shader";
        case PipelineStage::Transform: return "transform";
        case PipelineStage::Raster: return "raster";
        case PipelineStage::Framebuffer: return "framebuffer";
        case PipelineStage::Asset: return "asset";
        }
        return "알 수 없음";
    }
    /// [API] 오류 출력과 단계 추적 출력의 사용 여부를 설정한다.
    static void configure(DiagnosticOptions value) { options() = value; }
    /// [API] 마지막으로 기록한 오류의 optional 참조를 반환한다.
    static const std::optional<Diagnostic>& lastError() { return issue(); }
    /// [API] 마지막 오류 기록을 지운다. 진단 설정과 현재 위치 문맥은 유지한다.
    static void clear() { issue().reset(); }
    /// [API] 추적 옵션이 켜져 있으면 해당 단계·API의 시작을 콘솔에 출력한다.
    static void trace(PipelineStage stage, const char* api) {
        if (options().trace) std::clog << "[OpenGL][" << stageName(stage) << "][" << api << "] 시작\n";
    }
    /// [API] 현재 진단 문맥에서 원인·조치·객체 ID를 저장하고 출력한 뒤 false를 반환한다.
    static bool fail(PipelineStage stage, const char* api, const std::string& cause,
                     const std::string& action = "framework/docs/API_KO.md에서 해당 API의 입력 조건과 반환값을 확인하세요.",
                     std::optional<std::uint64_t> object = std::nullopt) {
        issue() = Diagnostic{stage, api, cause, action, object};
        issue()->file=context().file;issue()->line=context().line;issue()->operation=context().operation;
        if (options().errors) {
            std::cerr << "[OpenGL][" << stageName(stage) << "][" << api << "] 오류";
            if (object) std::cerr << " 객체=" << *object;
            std::cerr << ": " << cause << "\n  조치: " << action << '\n';
            std::cerr << "  소스: " << context().file << ':' << context().line << "\n  호출 경로: " << context().operation << '\n';
        }
        return false;
    }
};
} // namespace graphics
