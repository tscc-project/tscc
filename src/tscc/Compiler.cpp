#include "Compiler.h"
#include "CompilationUnit.h"
#include "Project.h"
#include "Source.h"
#include "Transpiler.h"
#include "JSX.h"
#include "SourceMap.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs=std::filesystem;
namespace tscc {
struct PreparedOutput { CompilationUnit* unit=nullptr; fs::path path,map_path; std::string map_text; };
static fs::path common_root(const std::vector<std::string>&roots){if(roots.empty())return fs::current_path();std::error_code ec;fs::path common=fs::absolute(fs::path(roots.front()).parent_path(),ec).lexically_normal();for(std::size_t i=1;i<roots.size();++i){fs::path p=fs::absolute(fs::path(roots[i]).parent_path(),ec).lexically_normal();auto a=common.begin(),b=p.begin();fs::path next;while(a!=common.end()&&b!=p.end()&&*a==*b){next/=*a;++a;++b;}common=next;}return common.empty()?fs::current_path():common;}
static bool prepare(CompilationUnit&unit,const CompilerOptions&o,const fs::path&root,PreparedOutput&prepared){
 const auto&s=unit.source;auto&d=unit.diagnostics;const fs::path source_path(s.path);const bool tsx=source_path.extension()==".tsx";
 if(o.module!="preserve"&&o.module!="esnext"&&o.module!="commonjs"){d.error(s.path,1,1,"unsupported module mode '"+o.module+"' (supported: preserve, esnext, commonjs)");return false;}
 if(tsx&&o.jsx!="preserve"){d.error(s.path,1,1,"TSX input requires --jsx preserve in this compiler checkpoint");return false;}
 if(!o.jsx.empty()&&o.jsx!="preserve"){d.error(s.path,1,1,"unsupported JSX mode '"+o.jsx+"' (currently supported: preserve)");return false;}
 if(tsx&&s.text.find("namespace JSX")!=std::string::npos&&!check_jsx_semantics(unit))return false;
 TranspileOptions to;to.target=o.target;to.module=o.module;to.remove_comments=o.remove_comments;if(!transpile_unit(unit,to))return false;
 fs::path input=fs::absolute(s.path).lexically_normal(),output;if(o.out_dir.empty()){output=input;output.replace_extension(tsx?".jsx":".js");}else{fs::path rel=input.lexically_relative(fs::absolute(root).lexically_normal());if(rel.empty()||rel.string().rfind("..",0)==0)rel=input.filename();output=fs::path(o.out_dir)/rel;output.replace_extension(tsx?".jsx":".js");}
 unit.output_path=output.string();prepared.unit=&unit;prepared.path=output;if(o.source_map){prepared.map_path=output.string()+".map";prepared.map_text=make_line_source_map(output.filename().string(),input.filename().string(),unit.emitted_text);unit.emitted_text+="\n//# sourceMappingURL="+prepared.map_path.filename().string()+"\n";}return true;
}
static bool commit_outputs(const std::vector<PreparedOutput>&outputs,Diagnostics&d,int&count){
 struct Staged{fs::path target,temp;};std::vector<Staged>staged;staged.reserve(outputs.size()*2);
 for(std::size_t i=0;i<outputs.size();++i){const auto&o=outputs[i];const auto&source=o.unit->source.path;std::error_code ec;fs::create_directories(o.path.parent_path(),ec);if(ec){d.error(source,1,1,"cannot create output directory '"+o.path.parent_path().string()+"'");break;}fs::path temp=o.path;temp += ".tscc-tmp-"+std::to_string(i);std::ofstream w(temp,std::ios::binary|std::ios::trunc);if(!w){d.error(source,1,1,"cannot stage output '"+o.path.string()+"'");break;}w<<o.unit->emitted_text;if(!w){d.error(source,1,1,"cannot stage output '"+o.path.string()+"'");break;}w.close();staged.push_back({o.path,temp});if(!o.map_path.empty()){fs::path map_temp=o.map_path;map_temp += ".tscc-tmp-"+std::to_string(i)+"-map";std::ofstream map(map_temp,std::ios::binary|std::ios::trunc);if(!map){d.error(source,1,1,"cannot stage source map '"+o.map_path.string()+"'");break;}map<<o.map_text;map.close();staged.push_back({o.map_path,map_temp});}}
 if(d.has_errors()){for(const auto&s:staged){std::error_code ec;fs::remove(s.temp,ec);}return false;}
 for(const auto&s:staged){std::error_code ec;fs::rename(s.temp,s.target,ec);if(ec){fs::remove(s.target,ec);ec.clear();fs::rename(s.temp,s.target,ec);}if(ec){d.error(s.target.string(),1,1,"cannot commit output file");return false;}if(s.target.extension()!=".map")++count;}return true;
}
int compile_files(const std::vector<std::string>&roots,const CompilerOptions&o){
 Diagnostics d;int emitted=0;std::vector<PreparedOutput>prepared;
 std::vector<std::string>libraries=o.no_lib?std::vector<std::string>{}:o.lib;libraries.insert(libraries.end(),o.types.begin(),o.types.end());
 if(o.target!="es2022")d.error("",1,1,"unsupported target '"+o.target+"' (compiler preview target: es2022)",{},"TSCC3001");
 if(o.module!="preserve"&&o.module!="esnext"&&o.module!="commonjs")d.error("",1,1,"unsupported module mode '"+o.module+"' (supported: preserve, esnext, commonjs)",{},"TSCC3001");
 if(o.module_resolution!="relative"&&o.module_resolution!="node")d.error("",1,1,"unsupported module resolution mode '"+o.module_resolution+"' (supported: relative, node)",{},"TSCC3001");
 if(!o.jsx.empty()&&o.jsx!="preserve")d.error("",1,1,"unsupported JSX mode '"+o.jsx+"' (currently supported: preserve)",{},"TSCC3001");
 for(const auto&library:libraries){TypeModel probe;if(!add_library_bundle(probe,library))d.error("",1,1,"unsupported library bundle '"+library+"' (supported: es2022, node)",{},"TSCC3001");}
 if(d.has_errors()){d.print(o.pretty);return 2;}
 const fs::path root=o.root_dir.empty()?common_root(roots):fs::path(o.root_dir);ProgramGraph graph;graph.build(roots,o.follow_imports,o.module_resolution,o.base_url,o.paths,o.root_dirs,libraries);d.append(graph.diagnostics());
 for(auto&file:graph.files()){auto&unit=*file.unit;if(!unit.declaration_file&&!unit.diagnostics.has_errors()&&unit.stage==CompilationStage::Checked){PreparedOutput output;if(prepare(unit,o,root,output))prepared.push_back(std::move(output));}d.append(unit.diagnostics);}
 const bool compile_errors=d.has_errors();if(!o.no_emit&&(!compile_errors||!o.no_emit_on_error)){Diagnostics emit_d;commit_outputs(prepared,emit_d,emitted);d.append(std::move(emit_d));}d.print(o.pretty);if(d.has_errors())return 2;if(!o.no_emit)std::cout<<"tscc: emitted "<<emitted<<(emitted==1?" file\n":" files\n");return 0;
}
}
