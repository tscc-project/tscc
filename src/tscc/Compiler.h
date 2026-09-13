#pragma once
#include "Transpiler.h"
#include <string>
#include <vector>
#include <utility>
namespace tscc {
struct CompilerOptions {
    std::string out_dir;
    std::string root_dir;
    std::string target="es2022";
    std::string module="preserve";
    std::string module_resolution="relative";
    std::string jsx;
    std::string base_url;
    std::vector<std::string> root_dirs;
    std::vector<std::string> lib={"es2022"};
    std::vector<std::string> types;
    std::vector<std::pair<std::string,std::vector<std::string>>> paths;
    bool remove_comments=false,no_emit=false,no_emit_on_error=false,no_lib=false,pretty=true,source_map=false,declaration=false,declaration_map=false;
    bool follow_imports=true;
};
int compile_files(const std::vector<std::string>& files,const CompilerOptions& options);
}
