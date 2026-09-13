#include "tscc/Diagnostic.h"
#include "tscc/Lexer.h"
#include "tscc/Source.h"
#include "tscc/Type.h"
#include "tscc/TypeParser.h"
#include <cstdlib>
#include <iostream>

using namespace tscc;
static void fail(const char* message) { std::cerr << message << '\n'; std::exit(1); }

static std::vector<Token> lex(const std::string& text, SourceFile& source, Diagnostics& diagnostics) {
    source.path = "types.ts";
    source.text = text;
    source.line_starts = {0};
    return Lexer(source, diagnostics).lex();
}

int main() {
    SourceFile source;
    Diagnostics diagnostics;
    const auto tokens = lex("{required:string;maybe?:unknown;children?:unknown;count:number}", source, diagnostics);
    TypeStore store;
    std::size_t at = 0;
    const auto object = parse_type_annotation(tokens, at, tokens.size() - 1, store, nullptr);
    if (store.kind(object) != TypeKind::Object)
        fail("object with unknown-typed members did not parse to an object");
    if (at != tokens.size() - 1)
        fail("object parse did not consume the full annotation");
    const auto* required = store.property(object, "required");
    if (!required || required->type != store.string() || required->optional)
        fail("required:string property missing or misclassified");
    const auto* maybe = store.property(object, "maybe");
    if (!maybe || maybe->type != store.unknown() || !maybe->optional)
        fail("maybe?:unknown property missing or misclassified");
    const auto* children = store.property(object, "children");
    if (!children || children->type != store.unknown() || !children->optional)
        fail("children?:unknown property missing or misclassified");
    const auto* count = store.property(object, "count");
    if (!count || count->type != store.number() || count->optional)
        fail("count:number property missing or misclassified");

    SourceFile bad_source;
    Diagnostics bad_diagnostics;
    const auto bad_tokens = lex("{a:not_a_type}", bad_source, bad_diagnostics);
    std::size_t bad_at = 0;
    const auto bad = parse_type_annotation(bad_tokens, bad_at, bad_tokens.size() - 1, store, nullptr);
    if (bad != store.unknown())
        fail("unsupported member type did not conservatively yield unknown");

    std::cout << "tscc type-annotation object smoke test passed\n";
}