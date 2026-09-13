#pragma once
#include "CompilationUnit.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace tscc {
enum class ResolutionStatus { NotApplicable, Resolved, Missing, Ambiguous };
struct ResolutionResult {
    ResolutionStatus status = ResolutionStatus::NotApplicable;
    std::filesystem::path path;
    std::vector<std::filesystem::path> candidates;
};
ResolutionResult resolve_relative_module(const std::filesystem::path& importer,
                                         const std::string& specifier);
ResolutionResult resolve_module(const std::filesystem::path& importer,
                                const std::string& specifier,
                                const std::string& mode);
bool discover_module_dependencies(const std::filesystem::path&,const SourceFile&,
                                  const std::vector<Token>&,
                                  std::vector<std::filesystem::path>&,Diagnostics&,
                                  const std::string& module_resolution="relative");

struct ProgramFile {
    struct ExportFact { std::string name; std::size_t file; std::size_t symbol; bool type_only=false; };
    std::unique_ptr<CompilationUnit> unit;
    std::vector<std::size_t> dependencies;
    std::vector<std::filesystem::path> dependency_paths;
    std::vector<ExportFact> exports;
};

class ProgramGraph {
public:
    bool build(const std::vector<std::string>& roots,bool follow_imports,
               const std::string& module_resolution="relative");
    std::vector<ProgramFile>& files(){return files_;}
    const std::vector<ProgramFile>& files()const{return files_;}
    const std::vector<std::size_t>& roots()const{return roots_;}
    Diagnostics& diagnostics(){return diagnostics_;}
    const Diagnostics& diagnostics()const{return diagnostics_;}
private:
    std::vector<ProgramFile> files_;
    std::vector<std::size_t> roots_;
    std::unordered_map<std::string,std::size_t> identities_;
    Diagnostics diagnostics_;
};
}
