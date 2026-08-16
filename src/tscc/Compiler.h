#pragma once
#include "Transpiler.h"
#include <string>
#include <vector>
namespace tscc {
struct CompilerOptions {
    std::string out_dir;
    std::string root_dir;
    std::string target="es2022";
    std::string module="preserve";
    std::string jsx;
    bool remove_comments=false,no_emit=false,no_emit_on_error=false,pretty=true;
    bool follow_imports=true;
};
int compile_files(const std::vector<std::string>& files,const CompilerOptions& options);
}
