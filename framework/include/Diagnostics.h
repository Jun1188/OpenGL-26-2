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
    static Diagnostic& context() { static Diagnostic value; return value; }
    static void locate(const char* file,int line,const char* operation) {context().file=file;context().line=line;context().operation=operation;}
    static bool failAt(const char* file,int line,PipelineStage stage,const char* api,const std::string& cause,
                       const std::string& action="Inspect the source location and this API's input contract.",
                       std::optional<std::uint64_t> object=std::nullopt) {
        const auto saved=context();locate(file,line,api);
        const bool result=fail(stage,api,cause,action,object);context()=saved;return result;
    }
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
        return "unknown";
    }
    static void configure(DiagnosticOptions value) { options() = value; }
    static const std::optional<Diagnostic>& lastError() { return issue(); }
    static void clear() { issue().reset(); }
    static void trace(PipelineStage stage, const char* api) {
        if (options().trace) std::clog << "[OpenGL][" << stageName(stage) << "][" << api << "] begin\n";
    }
    static bool fail(PipelineStage stage, const char* api, const std::string& cause,
                     const std::string& action = "Check this API's input contract in framework/docs/APPLICATION_API.md.",
                     std::optional<std::uint64_t> object = std::nullopt) {
        issue() = Diagnostic{stage, api, cause, action, object};
        issue()->file=context().file;issue()->line=context().line;issue()->operation=context().operation;
        if (options().errors) {
            std::cerr << "[OpenGL][" << stageName(stage) << "][" << api << "] ERROR";
            if (object) std::cerr << " object=" << *object;
            std::cerr << ": " << cause << "\n  Action: " << action << '\n';
            std::cerr << "  Source: " << context().file << ':' << context().line << "\n  Operation: " << context().operation << '\n';
        }
        return false;
    }
};
} // namespace graphics
