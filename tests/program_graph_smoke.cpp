#include "tscc/Project.h"
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
namespace fs=std::filesystem;using namespace tscc;
static void fail(const char*m){std::cerr<<m<<'\n';std::exit(1);}static void write(const fs::path&p,const char*s){std::ofstream(p)<<s;}
int main(){auto dir=fs::temp_directory_path()/"tscc-program-graph-smoke";fs::remove_all(dir);fs::create_directories(dir);write(dir/"a.ts","import {b} from './b.js'; export const a:number=b+1; function keep(value:number):number{return value;} keep(a);");write(dir/"b.ts","import {a} from './a.js'; export const b:number=1;");
 ProgramGraph graph;if(!graph.build({(dir/"a.ts").string()},true))fail("graph build failed");if(graph.roots().size()!=1||graph.files().size()!=2)fail("graph identity/root ownership failed");if(graph.files()[0].dependencies.size()!=1||graph.files()[1].dependencies.size()!=1)fail("graph edges missing");for(const auto&f:graph.files())if(!f.unit||f.unit->stage!=CompilationStage::Checked)fail("graph did not own checked units");
 bool retained_expression=false;for(const auto&file:graph.files())for(const auto&node:file.unit->semantic.nodes)if(node.kind==SemanticNodeKind::ExpressionRoot)retained_expression=true;if(!retained_expression)fail("semantic expression roots missing");
 ProgramGraph roots;if(!roots.build({(dir/"a.ts").string()},false)||roots.files().size()!=1||!roots.files()[0].dependencies.empty())fail("noResolve graph policy failed");fs::remove_all(dir);std::cout<<"program graph ownership passed\n";
}
