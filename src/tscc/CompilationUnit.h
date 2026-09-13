#pragma once
#include "Binder.h"
#include "Diagnostic.h"
#include "Expression.h"
#include "Lexer.h"
#include "Semantic.h"
#include "Source.h"
#include "Syntax.h"
#include "Type.h"
#include <string>
#include <vector>

namespace tscc {
enum class CompilationStage { Source, Lexed, Parsed, Analyzed, Checked, Emitted };
struct UnitOptions { std::string target="es2022"; std::string module="preserve"; bool remove_comments=false; };

class CompilationUnit {
public:
    explicit CompilationUnit(SourceFile source);
    CompilationUnit(SourceFile source, std::vector<Token> tokens);
    bool analyze();

    SourceFile source;
    std::vector<Token> tokens;
    Program program;
    SemanticModel semantic;
    BindingModel binding;
    TypeModel types;
    ExpressionModel expressions;
    Diagnostics diagnostics;
    UnitOptions options;
    std::string emitted_text;
    std::string output_path;
    bool declaration_file = false;
    CompilationStage stage = CompilationStage::Source;
};
}
