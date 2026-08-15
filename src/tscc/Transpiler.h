#pragma once
#include "Source.h"
#include "Diagnostic.h"
#include "Lexer.h"
#include <string>
namespace tscc {
struct TranspileOptions { bool remove_comments=false; bool source_map=false; std::string target="es2022"; std::string module="preserve"; };
bool transpile(const SourceFile&,const TranspileOptions&,std::string&,Diagnostics&);
bool transpile_tokens(const SourceFile&,const std::vector<Token>&,const TranspileOptions&,std::string&,Diagnostics&);
}
