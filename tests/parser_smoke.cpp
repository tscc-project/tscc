#include "tscc/Diagnostic.h"
#include "tscc/Lexer.h"
#include "tscc/Parser.h"
#include "tscc/Source.h"
#include "tscc/Semantic.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace tscc;

static void fail(const std::string& m) { std::cerr << m << '\n'; std::exit(1); }

int main() {
    SourceFile source;
    source.path = "parser-smoke.ts";
    source.text = R"TS(
interface User<T> { name: string; value: T }
type Maybe<T> = T | null;
function f(user: User<number>, suffix: string = "!"): string {
    const object = {name: "Ada", nested: {x: 1}};
    const typed: number = object.nested.x;
    return user.name + suffix + typed;
}
class Box implements User<number> {
    public readonly name: string = "Ada";
    private value: number = 2;
    method(x: number): number { const y: number = x + 1; return y; }
}
const arrow = (x: number, y: string): string => {
    const z: number = x + 1;
    return y + z;
};
const typedObject: {name: string; x: number} = {name: "Grace", x: 2};
)TS";
    source.line_starts = {0};
    for (std::size_t i=0;i<source.text.size();++i)
        if (source.text[i]=='\n') source.line_starts.push_back(i+1);

    Diagnostics diagnostics;
    Lexer lexer(source, diagnostics);
    auto tokens = lexer.lex();
    if (diagnostics.has_errors()) fail("lexer unexpectedly failed");

    Program program;
    Parser parser(source, tokens, diagnostics);
    if (!parser.parse(program)) fail("parser unexpectedly failed");

    std::size_t interfaces=0, aliases=0, functions=0, classes=0, vars=0;
    for (const auto& node : program.root.children) {
        switch (node.kind) {
            case SyntaxKind::InterfaceDeclaration: ++interfaces; break;
            case SyntaxKind::TypeAliasDeclaration: ++aliases; break;
            case SyntaxKind::FunctionDeclaration: ++functions; break;
            case SyntaxKind::ClassDeclaration: ++classes; break;
            case SyntaxKind::VariableStatement: ++vars; break;
            default: break;
        }
    }
    if (interfaces != 1 || aliases != 1 || functions != 1 || classes != 1 || vars != 2)
        fail("unexpected top-level syntax tree shape");
    if (program.erasures.size() < 10) fail("parser did not record expected TypeScript erasures");
    if (program.variables.size() < 5) fail("parser did not retain variable declaration facts");

    const auto semantic = build_semantic_model(tokens, program);
    std::size_t declarations=0, functions_semantic=0, parameters=0, braces=0, returns=0;
    for (const auto& node : semantic.nodes) {
        if (node.begin_token > node.end_token || node.end_token > tokens.size())
            fail("semantic node has invalid source span");
        switch (node.kind) {
            case SemanticNodeKind::VariableDeclaration: ++declarations; break;
            case SemanticNodeKind::BindingDeclaration: break;
            case SemanticNodeKind::FunctionDeclaration: ++functions_semantic; break;
            case SemanticNodeKind::ArrowFunction: break;
            case SemanticNodeKind::ClassDeclaration: break;
            case SemanticNodeKind::ParameterDeclaration: ++parameters; break;
            case SemanticNodeKind::BraceRegion: ++braces; break;
            case SemanticNodeKind::ReturnStatement: ++returns; break;
            case SemanticNodeKind::ExpressionRoot: break;
            case SemanticNodeKind::LexicalRegion: break;
            case SemanticNodeKind::CatchDeclaration: break;
            case SemanticNodeKind::Statement: break;
            case SemanticNodeKind::Condition: break;
        }
    }
    if (declarations != program.variables.size() || functions_semantic != 1 ||
        parameters != 4 || braces < 5 || returns < 3)
        { std::cerr<<"counts "<<declarations<<' '<<functions_semantic<<' '<<parameters<<' '<<braces<<' '<<returns<<'\n'; fail("unexpected lightweight semantic model shape"); }

    // Object-literal colons must not be parser-owned erasures.
    const auto object_colon = source.text.find("name: \"Ada\"");
    if (object_colon == std::string::npos) fail("fixture broken");
    const auto colon = source.text.find(':', object_colon);
    for (const auto& r : program.erasures)
        if (colon >= r.begin && colon < r.end)
            fail("object-literal colon incorrectly classified as TypeScript annotation");

    std::cout << "tscc parser/AST smoke test passed\n";
}
