#include "tscc/CompilationUnit.h"
#include "tscc/Transpiler.h"
#include <cstdlib>
#include <iostream>

using namespace tscc;
static void fail(const char*m){std::cerr<<m<<'\n';std::exit(1);}
static SourceFile source(std::string text){SourceFile s{"unit.ts",std::move(text),{0}};for(std::size_t i=0;i<s.text.size();++i)if(s.text[i]=='\n')s.line_starts.push_back(i+1);return s;}

int main(){
 CompilationUnit unit(source("const answer: number = 42;\nconsole.log(answer);\n"));
 if(!unit.analyze()||unit.stage!=CompilationStage::Checked)fail("unit did not reach checked stage");
 if(unit.tokens.empty()||unit.program.variables.size()!=1||unit.semantic.nodes.empty())fail("front-end state was not retained");
 if(unit.binding.symbols.empty()||unit.types.symbol_types.empty())fail("semantic state was not retained");
 if(unit.expressions.nodes().empty()||unit.expressions.nodes().front().kind!=ExpressionKind::Literal||unit.expressions.nodes().front().begin_token>=unit.expressions.nodes().front().end_token)fail("durable expression identity was not retained");
 const auto token_count=unit.tokens.size(),symbol_count=unit.binding.symbols.size();
 TranspileOptions options;if(!transpile_unit(unit,options))fail("checked unit did not emit");
 if(unit.stage!=CompilationStage::Emitted||unit.emitted_text.find(": number")!=std::string::npos)fail("unit did not retain emitted JavaScript");
 if(unit.options.target!="es2022"||unit.options.module!="preserve")fail("unit did not retain effective options");
 if(unit.tokens.size()!=token_count||unit.binding.symbols.size()!=symbol_count||unit.program.variables.size()!=1)fail("emission discarded prior-stage state");
 CompilationUnit bad(source("const broken: number = 'wrong';"));if(bad.analyze()||!bad.diagnostics.has_errors())fail("invalid unit was accepted");
 CompilationUnit next(source("let fresh: number = 1;"));if(!next.analyze()||!transpile_unit(next,options))fail("unit state leaked across files");
 std::cout<<"compilation unit lifecycle passed\n";
}
