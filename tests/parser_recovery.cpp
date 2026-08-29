#include "tscc/Diagnostic.h"
#include "tscc/Lexer.h"
#include "tscc/Parser.h"
#include "tscc/Source.h"
#include <cstdlib>
#include <iostream>
using namespace tscc;
static void fail(const char*m){std::cerr<<m<<'\n';std::exit(1);}
static SourceFile make_source(const std::string&text){SourceFile s{"recovery.ts",text,{0}};for(std::size_t i=0;i<text.size();++i)if(text[i]=='\n')s.line_starts.push_back(i+1);return s;}
struct Result{Program program;Diagnostics diagnostics;};
static Result parse(const std::string&text,ParserLimits limits={}){auto source=make_source(text);Result result;Lexer lexer(source,result.diagnostics);auto tokens=lexer.lex();Parser parser(source,tokens,result.diagnostics,limits);parser.parse(result.program);return result;}
int main(){
 const std::string malformed="function broken(;\nconst next:number=1;\ninterface Open { value:string;\nconst tail:string='ok';\n";
 auto first=parse(malformed),second=parse(malformed);if(!first.diagnostics.has_errors()||first.diagnostics.size()!=second.diagnostics.size())fail("recovery diagnostics missing or unstable");
 if(first.program.node_count()!=second.program.node_count())fail("recovery tree count unstable");
 std::size_t variables=0,recoveries=0;for(const auto&node:first.program.root.children){if(node.kind==SyntaxKind::VariableStatement)++variables;if(node.kind==SyntaxKind::Recovery)++recoveries;const auto*other=second.program.find_node(node.id);if(!other||other->kind!=node.kind||other->begin_offset!=node.begin_offset||other->end_offset!=node.end_offset)fail("recovery identity unstable");}
 if(variables!=2||recoveries<2)fail("following declarations consumed during recovery");
 auto bounded=parse("const a:number=1;const b:number=2;",ParserLimits{1});if(!bounded.diagnostics.has_errors()||bounded.diagnostics.items().back().message!="parser top-level work limit exceeded")fail("parser budget not enforced");
 std::cout<<"deterministic parser recovery passed\n";
}
