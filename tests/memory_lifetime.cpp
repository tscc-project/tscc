#include "tscc/Binder.h"
#include "tscc/Checker.h"
#include "tscc/ControlFlow.h"
#include "tscc/Diagnostic.h"
#include "tscc/Lexer.h"
#include "tscc/Parser.h"
#include "tscc/Semantic.h"
#include "tscc/Source.h"
#include "tscc/Transpiler.h"
#include "tscc/Type.h"
#include <cstdlib>
#include <iostream>
#include <string>

using namespace tscc;

static void lines(SourceFile& s) {
    s.line_starts={0};
    for (std::size_t i=0;i<s.text.size();++i) if(s.text[i]=='\n') s.line_starts.push_back(i+1);
}
static void fail(const char* m){std::cerr<<m<<'\n';std::exit(1);}

static void compile_memory(const std::string& text,const std::string& path,
                           const std::string& module,const bool expect_ok) {
    SourceFile source; source.path=path; source.text=text; lines(source);
    Diagnostics diagnostics;
    Lexer lexer(source,diagnostics); auto tokens=lexer.lex();
    Program program; Parser parser(source,tokens,diagnostics);
    bool parsed=!diagnostics.has_errors() && parser.parse(program);
    if(parsed && !diagnostics.has_errors()){
        auto semantic=build_semantic_model(tokens,program);
        auto binding=bind_semantic_model(tokens,program,semantic);
        auto types=build_type_model(tokens,program,semantic,binding);
        ExpressionModel expressions;own_semantic_expressions(tokens,semantic,expressions);
        auto control_flow=build_control_flow(tokens,semantic,expressions);
        check_program(source,tokens,program,semantic,binding,types,expressions,control_flow,diagnostics);
        std::string js; TranspileOptions opts; opts.module=module;
        if(!transpile_tokens(source,tokens,opts,js,diagnostics)) parsed=false;
    }
    const bool ok=parsed && !diagnostics.has_errors();
    if(ok!=expect_ok) { std::cerr<<"fixture mismatch: "<<path<<" expect="<<expect_ok<<"\n"; fail(expect_ok?"valid lifetime fixture failed":"invalid lifetime fixture succeeded"); }
}

int main(int argc,char**argv){
    int iterations=argc>1?std::atoi(argv[1]):100;
    if(iterations<1) iterations=1;
    const std::string heavy=R"TS(
import read, {value, bump} from "./state.js";
interface User<T>{name:string;value:T}
type Maybe<T>=T|null;
enum Direction {Up,Down=3,Left,Label="label"}
namespace Boxed { export const answer:number=42; }
class Box<T>{constructor(public value:T,readonly label:string){}}
function calc(x:number,y:number):number {
  let total:number=x+y*2;
  for(let i:number=0;i<5;i+=1){total+=i;}
  try { if(total<0) throw new Error("x"); } catch(problem) { total+=1; }
  return total;
}
const user:User<number>={name:"Ada",value:3};
const box=new Box<number>(user.value,"x");
bump();
console.log(read(),value,box.value,Direction.Left,Boxed.answer,calc(1,2));
)TS";
    const std::string cjs=R"TS(
import def, {value as live} from "./dep.js";
export const n:number=live+1;
export default function twice(x:number):number{return def(x)*2;}
)TS";
    const std::string tsx=R"TS(
interface Props{count:number}
const View=(p:Props):unknown=><main><span>{p.count as number}</span></main>;
)TS";
    const std::string bad1="const value: number = ; function broken( {";
    const std::string bad2="const x=<div><span></div>;";
    for(int i=0;i<iterations;++i){
        compile_memory(heavy,"heavy.ts","preserve",true);
        compile_memory(cjs,"module.ts","commonjs",true);
        compile_memory(tsx,"view.tsx","preserve",true);
        compile_memory(bad1,"broken.ts","preserve",false);
        compile_memory(bad2,"broken.tsx","preserve",false);
        // Tiny successful source after failures catches stale retained state.
        compile_memory("let n:number=1; n+=1;","tiny.ts","preserve",true);
    }
    std::cout<<"tscc compiler lifetime corpus passed "<<iterations<<" iterations\n";
}
