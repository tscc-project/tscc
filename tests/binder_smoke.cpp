#include "tscc/Binder.h"
#include "tscc/Diagnostic.h"
#include "tscc/Lexer.h"
#include "tscc/Parser.h"
#include "tscc/Semantic.h"
#include "tscc/Source.h"
#include <cstdlib>
#include <iostream>

using namespace tscc;
static void fail(const char* message) { std::cerr << message << '\n'; std::exit(1); }

int main() {
    SourceFile source;
    source.path = "binder.ts";
    source.text = R"TS(
const value: number = 1;
function outer(parameter: string) {
    const captured: string = parameter;
    {
        const value: string = captured;
        console.log(value);
    }
    function inner() { return captured; }
    return value;
}
try { throw 1; } catch (problem) { console.log(problem); }
for (let index = 0; index < 1; index++) { console.log(index); }
)TS";
    source.line_starts = {0};
    for (std::size_t i = 0; i < source.text.size(); ++i)
        if (source.text[i] == '\n') source.line_starts.push_back(i + 1);
    Diagnostics diagnostics;
    Lexer lexer(source, diagnostics);
    const auto tokens = lexer.lex();
    Program program;
    Parser parser(source, tokens, diagnostics);
    if (!parser.parse(program)) fail("binder fixture did not parse");
    const auto semantic = build_semantic_model(tokens, program);
    const auto binding = bind_semantic_model(tokens, program, semantic);

    std::size_t resolved = 0, unresolved = 0;
    std::size_t block_value_symbol = static_cast<std::size_t>(-1);
    std::size_t global_value_symbol = static_cast<std::size_t>(-1);
    std::size_t catch_symbol = static_cast<std::size_t>(-1);
    std::size_t loop_symbol = static_cast<std::size_t>(-1);
    for (std::size_t i = 0; i < binding.symbols.size(); ++i) {
        const auto& symbol = binding.symbols[i];
        if (symbol.name == "value") {
            if (symbol.scope == 0) global_value_symbol = i;
            else block_value_symbol = i;
        }
        if (symbol.name == "problem") catch_symbol = i;
        if (symbol.name == "index") loop_symbol = i;
    }
    for (const auto& reference : binding.references) {
        if (reference.symbol == static_cast<std::size_t>(-1)) { ++unresolved; continue; }
        ++resolved;
        if (tokens[reference.token].text == "value") {
            const auto line = source.line_col(tokens[reference.token].begin).first;
            if (line == 7 && reference.symbol != block_value_symbol)
                fail("block reference did not resolve to shadowing declaration");
            if (line == 10 && reference.symbol != global_value_symbol)
                fail("outer return did not resolve beyond the block shadow");
        }
        if (tokens[reference.token].text == "problem" && reference.symbol != catch_symbol)
            fail("catch reference did not resolve to catch binding");
        if (tokens[reference.token].text == "index" && reference.symbol != loop_symbol)
            fail("loop reference did not resolve to loop binding");
    }
    if (resolved < 9 || unresolved == 0 || catch_symbol == static_cast<std::size_t>(-1) ||
        loop_symbol == static_cast<std::size_t>(-1)) fail("unexpected bounded binder evidence");
    std::cout << "tscc bounded binder smoke test passed\n";
}
