#pragma once
#include "Diagnostic.h"
#include "Lexer.h"
#include "Source.h"
#include "Syntax.h"
#include "Semantic.h"
#include "Binder.h"
#include "Expression.h"
#include "Type.h"

namespace tscc {
bool check_program(const SourceFile&, const std::vector<Token>&, const Program&,
                   const SemanticModel&, const BindingModel&, const TypeModel&,
                   ExpressionModel&, Diagnostics&);
}
