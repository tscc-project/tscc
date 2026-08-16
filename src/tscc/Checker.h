#pragma once
#include "Diagnostic.h"
#include "Lexer.h"
#include "Source.h"
#include "Syntax.h"
#include "Semantic.h"
#include "Binder.h"

namespace tscc {
bool check_program(const SourceFile&, const std::vector<Token>&, const Program&,
                   const SemanticModel&, const BindingModel&, Diagnostics&);
}
