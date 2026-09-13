#include "CompilationUnit.h"
#include "Checker.h"
#include "Parser.h"
#include <utility>

namespace tscc {
static bool is_declaration_path(const std::string&path){return path.size()>=5&&path.compare(path.size()-5,5,".d.ts")==0;}
CompilationUnit::CompilationUnit(SourceFile input) : source(std::move(input)),declaration_file(is_declaration_path(source.path)) {}
CompilationUnit::CompilationUnit(SourceFile input, std::vector<Token> input_tokens)
    : source(std::move(input)), tokens(std::move(input_tokens)),declaration_file(is_declaration_path(source.path)), stage(CompilationStage::Lexed) {}

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
    own_semantic_expressions(tokens, semantic, expressions);
    binding = bind_semantic_model(tokens, program, semantic);
    types = build_type_model(tokens, program, semantic, binding);
    stage = CompilationStage::Analyzed;
    if (!check_program(source, tokens, program, semantic, binding, types, expressions, diagnostics)) return false;
    stage = CompilationStage::Checked;
    return true;
}
}
