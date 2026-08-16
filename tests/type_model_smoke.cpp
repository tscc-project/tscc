#include "tscc/Binder.h"
#include "tscc/Diagnostic.h"
#include "tscc/Lexer.h"
#include "tscc/Parser.h"
#include "tscc/Semantic.h"
#include "tscc/Source.h"
#include "tscc/Type.h"
#include <cstdlib>
#include <iostream>

using namespace tscc;
static void fail(const char* message) { std::cerr << message << '\n'; std::exit(1); }

int main() {
    SourceFile source;
    source.path = "types.ts";
    source.text = "const n:number=1;const s:string='x';let b:boolean=true;const complex:number|string=1;";
    source.line_starts = {0};
    Diagnostics diagnostics;
    Lexer lexer(source, diagnostics);
    const auto tokens = lexer.lex();
    Program program;
    Parser parser(source, tokens, diagnostics);
    if (!parser.parse(program)) fail("type-model fixture did not parse");
    const auto semantic = build_semantic_model(tokens, program);
    const auto binding = bind_semantic_model(tokens, program, semantic);
    const auto types = build_type_model(tokens, program, semantic, binding);
    if (types.symbol_types.size() != binding.symbols.size()) fail("type facts lost symbol identity");
    for (std::size_t i = 0; i < binding.symbols.size(); ++i) {
        const auto& name = binding.symbols[i].name;
        const auto kind = types.store.kind(types.symbol_types[i]);
        if (name == "n" && kind != TypeKind::Number) fail("number fact missing");
        if (name == "s" && kind != TypeKind::String) fail("string fact missing");
        if (name == "b" && kind != TypeKind::Boolean) fail("boolean fact missing");
        if (name == "complex" && kind != TypeKind::Unknown) fail("unsupported type was guessed");
    }
    std::cout << "tscc durable type-model smoke test passed\n";
}
