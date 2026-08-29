#include "tscc/Binder.h"
#include "tscc/Diagnostic.h"
#include "tscc/Lexer.h"
#include "tscc/Parser.h"
#include "tscc/Semantic.h"
#include "tscc/Source.h"
#include <cstdlib>
#include <iostream>
using namespace tscc;static void fail(const char*m){std::cerr<<m<<'\n';std::exit(1);}
int main(){SourceFile source{"scope.ts",R"TS(
const before = hoisted(1);
function hoisted(value:number){ return value; }
const add = (left:number, right:number = 1) => left + right;
const collect = (...items:number[]) => items;
const nested = (value:number) => { const inner = (value:string) => value; return value; };
class Box { value:number = 1; }
const made = Box;
const {first, second:alias, ...rest} = sourceObject;
let [head, tail] = sourceArray;
)TS",{0}};for(std::size_t i=0;i<source.text.size();++i)if(source.text[i]=='\n')source.line_starts.push_back(i+1);Diagnostics diagnostics;Lexer lexer(source,diagnostics);auto tokens=lexer.lex();Program program;Parser parser(source,tokens,diagnostics);if(!parser.parse(program))fail("fixture parse failed");auto semantic=build_semantic_model(tokens,program);auto binding=bind_semantic_model(tokens,program,semantic);std::size_t arrow_parameters=0,resolved_arrow_uses=0,patterns=0,class_symbol=static_cast<std::size_t>(-1),hoisted_symbol=static_cast<std::size_t>(-1);for(std::size_t i=0;i<binding.symbols.size();++i){const auto&s=binding.symbols[i];if(s.kind==SymbolKind::Parameter&&(s.name=="left"||s.name=="right"||s.name=="items"||s.name=="value"))++arrow_parameters;if(s.kind==SymbolKind::Variable&&(s.name=="first"||s.name=="alias"||s.name=="rest"||s.name=="head"||s.name=="tail"))++patterns;if(s.kind==SymbolKind::Class&&s.name=="Box")class_symbol=i;if(s.kind==SymbolKind::Function&&s.name=="hoisted")hoisted_symbol=i;}for(const auto&r:binding.references){if(r.symbol>=binding.symbols.size())continue;const auto&name=tokens[r.token].text;if((name=="left"||name=="right"||name=="items"||name=="value")&&binding.symbols[r.symbol].kind==SymbolKind::Parameter)++resolved_arrow_uses;if(name=="Box"&&r.symbol!=class_symbol)fail("class reference unresolved");if(name=="hoisted"&&r.symbol!=hoisted_symbol)fail("hoisted function unresolved");}if(arrow_parameters<6||resolved_arrow_uses<5||patterns!=5||class_symbol>=binding.symbols.size()||hoisted_symbol>=binding.symbols.size())fail("declaration/scope facts incomplete");std::cout<<"declaration and scope completion passed\n";}
