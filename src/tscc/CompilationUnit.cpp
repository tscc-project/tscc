#include "CompilationUnit.h"
#include "Checker.h"
#include "Parser.h"
#include <utility>

namespace tscc {
CompilationUnit::CompilationUnit(SourceFile input) : source(std::move(input)) {}
CompilationUnit::CompilationUnit(SourceFile input, std::vector<Token> input_tokens)
    : source(std::move(input)), tokens(std::move(input_tokens)), stage(CompilationStage::Lexed) {}

bool CompilationUnit::analyze() {
    if (stage == CompilationStage::Source) {
        Lexer lexer(source, diagnostics);
        tokens = lexer.lex();
        if (diagnostics.has_errors()) return false;
        stage = CompilationStage::Lexed;
    }
    Parser parser(source, tokens, diagnostics);
    if (!parser.parse(program)) return false;
    stage = CompilationStage::Parsed;
    semantic = build_semantic_model(tokens, program);
    binding = bind_semantic_model(tokens, program, semantic);
    types = build_type_model(tokens, program, semantic, binding);
    stage = CompilationStage::Analyzed;
    if (!check_program(source, tokens, program, semantic, binding, types, diagnostics)) return false;
    stage = CompilationStage::Checked;
    return true;
}
}
