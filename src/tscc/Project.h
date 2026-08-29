#pragma once
#include "CompilationUnit.h"
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace tscc {
bool discover_module_dependencies(const std::filesystem::path&,const SourceFile&,
                                  const std::vector<Token>&,
                                  std::vector<std::filesystem::path>&,Diagnostics&);

struct ProgramFile {
    std::unique_ptr<CompilationUnit> unit;
    std::vector<std::size_t> dependencies;
    std::vector<std::filesystem::path> dependency_paths;
};

class ProgramGraph {
public:
    bool build(const std::vector<std::string>& roots,bool follow_imports);
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
