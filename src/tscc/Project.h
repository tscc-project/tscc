#pragma once
#include "Diagnostic.h"
#include "Lexer.h"
#include "Source.h"
#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace tscc {

struct ModuleFile {
    std::filesystem::path path;
    SourceFile source;
    std::vector<Token> tokens;
    std::vector<std::filesystem::path> dependencies;
};

bool discover_module_dependencies(const std::filesystem::path& path,
                                  const SourceFile& source,
                                  const std::vector<Token>& tokens,
                                  std::vector<std::filesystem::path>& dependencies,
                                  Diagnostics& diagnostics);

class ModuleGraph {
public:
    bool build(const std::vector<std::string>& roots, Diagnostics& diagnostics);
    const std::vector<ModuleFile>& files() const { return files_; }

private:
    std::vector<ModuleFile> files_;
    std::unordered_map<std::string, std::size_t> seen_;

    bool visit(const std::filesystem::path& path, Diagnostics& diagnostics);
    bool resolve_relative(const std::filesystem::path& importer,
                          const std::string& specifier,
                          std::filesystem::path& resolved) const;
};

} // namespace tscc
