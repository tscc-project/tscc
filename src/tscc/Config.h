#pragma once
#include "Compiler.h"
#include "Diagnostic.h"
#include <string>
#include <vector>

namespace tscc {
bool load_tsconfig(const std::string& path, CompilerOptions& options,
                   std::vector<std::string>& roots, Diagnostics& diagnostics);
}
