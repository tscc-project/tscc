#include "Compiler.h"
#include "Diagnostic.h"
#include "Lexer.h"
#include "Project.h"
#include "Source.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <unordered_set>

namespace fs=std::filesystem;
namespace tscc {
static fs::path common_root(const std::vector<std::string>&roots){if(roots.empty())return fs::current_path();std::error_code ec;fs::path common=fs::absolute(fs::path(roots.front()).parent_path(),ec).lexically_normal();for(std::size_t i=1;i<roots.size();++i){fs::path p=fs::absolute(fs::path(roots[i]).parent_path(),ec).lexically_normal();auto a=common.begin(),b=p.begin();fs::path next;while(a!=common.end()&&b!=p.end()&&*a==*b){next/=*a;++a;++b;}common=next;}return common.empty()?fs::current_path():common;}
static bool emit(const SourceFile&s,const std::vector<Token>&tokens,const CompilerOptions&o,const fs::path&root,Diagnostics&d,int&count){
 const fs::path source_path(s.path);
 const bool tsx = source_path.extension()==".tsx";
 if(o.module!="preserve" && o.module!="esnext" && o.module!="commonjs"){
   d.error(s.path,1,1,"unsupported module mode '"+o.module+"' (supported: preserve, esnext, commonjs)");
   return false;
 }
 if(tsx && o.jsx!="preserve"){
   d.error(s.path,1,1,"TSX input requires --jsx preserve in this compiler checkpoint");
   return false;
 }
 if(!o.jsx.empty() && o.jsx!="preserve"){
   d.error(s.path,1,1,"unsupported JSX mode '"+o.jsx+"' (currently supported: preserve)");
   return false;
 }
 std::string js;TranspileOptions to;to.target=o.target;to.module=o.module;to.remove_comments=o.remove_comments;if(!transpile_tokens(s,tokens,to,js,d))return false;if(o.no_emit)return true;
 fs::path input=fs::absolute(s.path).lexically_normal(),output;if(o.out_dir.empty()){output=input;output.replace_extension(tsx?".jsx":".js");}else{fs::path rel=input.lexically_relative(fs::absolute(root).lexically_normal());if(rel.empty()||rel.string().rfind("..",0)==0)rel=input.filename();output=fs::path(o.out_dir)/rel;output.replace_extension(tsx?".jsx":".js");}
 std::error_code ec;fs::create_directories(output.parent_path(),ec);if(ec){d.error(s.path,1,1,"cannot create output directory '"+output.parent_path().string()+"'");return false;}std::ofstream w(output,std::ios::binary|std::ios::trunc);if(!w){d.error(s.path,1,1,"cannot write output '"+output.string()+"'");return false;}w<<js;++count;return true;
}
int compile_files(const std::vector<std::string>&roots,const CompilerOptions&o){
 Diagnostics d;int emitted=0;const fs::path root=o.root_dir.empty()?common_root(roots):fs::path(o.root_dir);std::vector<fs::path> queue;queue.reserve(roots.size());for(const auto&r:roots)queue.push_back(fs::absolute(r).lexically_normal());std::unordered_set<std::string>seen;seen.reserve(queue.size()*2+16);
 for(std::size_t qi=0;qi<queue.size();++qi){fs::path path=queue[qi];const std::string key=path.string();if(!seen.insert(key).second)continue;SourceFile source;std::string error;if(!load_source(key,source,error)){d.error(key,1,1,error);continue;}Lexer lexer(source,d);auto tokens=lexer.lex();if(d.has_errors())continue;
  if(o.follow_imports){std::vector<fs::path>deps;if(discover_module_dependencies(path,source,tokens,deps,d))for(auto&dep:deps)if(!seen.count(dep.string()))queue.push_back(std::move(dep));}
  emit(source,tokens,o,root,d,emitted);
 }
 d.print(o.pretty);if(d.has_errors())return 2;if(!o.no_emit)std::cout<<"tscc: emitted "<<emitted<<(emitted==1?" file\n":" files\n");return 0;
}
}
