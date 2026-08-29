#include "Compiler.h"
#include "CompilationUnit.h"
#include "Project.h"
#include "Source.h"
#include "Transpiler.h"
#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs=std::filesystem;
namespace tscc {
struct PreparedOutput { CompilationUnit* unit=nullptr; fs::path path; };
static fs::path common_root(const std::vector<std::string>&roots){if(roots.empty())return fs::current_path();std::error_code ec;fs::path common=fs::absolute(fs::path(roots.front()).parent_path(),ec).lexically_normal();for(std::size_t i=1;i<roots.size();++i){fs::path p=fs::absolute(fs::path(roots[i]).parent_path(),ec).lexically_normal();auto a=common.begin(),b=p.begin();fs::path next;while(a!=common.end()&&b!=p.end()&&*a==*b){next/=*a;++a;++b;}common=next;}return common.empty()?fs::current_path():common;}
static bool prepare(CompilationUnit&unit,const CompilerOptions&o,const fs::path&root,PreparedOutput&prepared){
 const auto&s=unit.source;auto&d=unit.diagnostics;const fs::path source_path(s.path);const bool tsx=source_path.extension()==".tsx";
 if(o.module!="preserve"&&o.module!="esnext"&&o.module!="commonjs"){d.error(s.path,1,1,"unsupported module mode '"+o.module+"' (supported: preserve, esnext, commonjs)");return false;}
 if(tsx&&o.jsx!="preserve"){d.error(s.path,1,1,"TSX input requires --jsx preserve in this compiler checkpoint");return false;}
 if(!o.jsx.empty()&&o.jsx!="preserve"){d.error(s.path,1,1,"unsupported JSX mode '"+o.jsx+"' (currently supported: preserve)");return false;}
 TranspileOptions to;to.target=o.target;to.module=o.module;to.remove_comments=o.remove_comments;if(!transpile_unit(unit,to))return false;
 fs::path input=fs::absolute(s.path).lexically_normal(),output;if(o.out_dir.empty()){output=input;output.replace_extension(tsx?".jsx":".js");}else{fs::path rel=input.lexically_relative(fs::absolute(root).lexically_normal());if(rel.empty()||rel.string().rfind("..",0)==0)rel=input.filename();output=fs::path(o.out_dir)/rel;output.replace_extension(tsx?".jsx":".js");}
 unit.output_path=output.string();prepared={&unit,std::move(output)};return true;
}
static bool commit_outputs(const std::vector<PreparedOutput>&outputs,Diagnostics&d,int&count){
 struct Staged{fs::path target,temp;};std::vector<Staged>staged;staged.reserve(outputs.size());
 for(std::size_t i=0;i<outputs.size();++i){const auto&o=outputs[i];const auto&source=o.unit->source.path;std::error_code ec;fs::create_directories(o.path.parent_path(),ec);if(ec){d.error(source,1,1,"cannot create output directory '"+o.path.parent_path().string()+"'");break;}fs::path temp=o.path;temp += ".tscc-tmp-"+std::to_string(i);std::ofstream w(temp,std::ios::binary|std::ios::trunc);if(!w){d.error(source,1,1,"cannot stage output '"+o.path.string()+"'");break;}w<<o.unit->emitted_text;if(!w){d.error(source,1,1,"cannot stage output '"+o.path.string()+"'");break;}w.close();staged.push_back({o.path,temp});}
 if(d.has_errors()){for(const auto&s:staged){std::error_code ec;fs::remove(s.temp,ec);}return false;}
 for(const auto&s:staged){std::error_code ec;fs::rename(s.temp,s.target,ec);if(ec){fs::remove(s.target,ec);ec.clear();fs::rename(s.temp,s.target,ec);}if(ec){d.error(s.target.string(),1,1,"cannot commit output file");return false;}++count;}return true;
}
int compile_files(const std::vector<std::string>&roots,const CompilerOptions&o){
 Diagnostics d;int emitted=0;std::vector<PreparedOutput>prepared;const fs::path root=o.root_dir.empty()?common_root(roots):fs::path(o.root_dir);ProgramGraph graph;graph.build(roots,o.follow_imports);d.append(graph.diagnostics());
 for(auto&file:graph.files()){auto&unit=*file.unit;if(!unit.diagnostics.has_errors()&&unit.stage==CompilationStage::Checked){PreparedOutput output;if(prepare(unit,o,root,output))prepared.push_back(std::move(output));}d.append(unit.diagnostics);}
 const bool compile_errors=d.has_errors();if(!o.no_emit&&(!compile_errors||!o.no_emit_on_error)){Diagnostics emit_d;commit_outputs(prepared,emit_d,emitted);d.append(std::move(emit_d));}d.print(o.pretty);if(d.has_errors())return 2;if(!o.no_emit)std::cout<<"tscc: emitted "<<emitted<<(emitted==1?" file\n":" files\n");return 0;
}
}
