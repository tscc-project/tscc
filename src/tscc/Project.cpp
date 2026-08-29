#include "Project.h"
#include "Source.h"
#include <algorithm>
#include <system_error>

namespace fs=std::filesystem;
namespace tscc {
static std::string key_for(const fs::path&p){std::error_code ec;return fs::absolute(p,ec).lexically_normal().string();}
static bool resolve_relative(const fs::path&importer,const std::string&spec,fs::path&resolved){
 if(!(spec.rfind("./",0)==0||spec.rfind("../",0)==0))return false;
 fs::path base=importer.parent_path()/spec;std::vector<fs::path>candidates;
 if(base.has_extension()){candidates.push_back(base);if(base.extension()==".js"){auto x=base;x.replace_extension(".ts");candidates.push_back(x);}if(base.extension()==".jsx"){auto x=base;x.replace_extension(".tsx");candidates.push_back(x);}}
 else{candidates.push_back(base.string()+".ts");candidates.push_back(base.string()+".tsx");candidates.push_back(base/"index.ts");candidates.push_back(base/"index.tsx");}
 std::error_code ec;for(auto&c:candidates){if(fs::is_regular_file(c,ec)){resolved=fs::absolute(c,ec).lexically_normal();return true;}ec.clear();}return false;
}
bool discover_module_dependencies(const fs::path&path,const SourceFile&source,const std::vector<Token>&tokens,std::vector<fs::path>&deps,Diagnostics&diagnostics){
 deps.clear();if(source.text.find("import")==std::string::npos&&source.text.find("from")==std::string::npos)return true;
 for(std::size_t i=0;i+1<tokens.size();++i){std::size_t j=i+1;bool site=false;if(tokens[i].text=="import"){if(j<tokens.size()&&tokens[j].kind==TokenKind::String)site=true;else while(j<tokens.size()&&tokens[j].text!=";"&&tokens[j].kind!=TokenKind::End){if(tokens[j].text=="from"&&j+1<tokens.size()&&tokens[j+1].kind==TokenKind::String){++j;site=true;break;}++j;}}else if(tokens[i].text=="export"){while(j<tokens.size()&&tokens[j].text!=";"&&tokens[j].kind!=TokenKind::End){if(tokens[j].text=="from"&&j+1<tokens.size()&&tokens[j+1].kind==TokenKind::String){++j;site=true;break;}++j;}}else continue;
  if(site&&j<tokens.size()&&tokens[j].kind==TokenKind::String){std::string spec=tokens[j].text.substr(1,tokens[j].text.size()-2);if(spec.rfind("./",0)==0||spec.rfind("../",0)==0){fs::path dep;if(!resolve_relative(path,spec,dep)){auto[line,col]=source.line_col(tokens[j].begin);diagnostics.error(source.path,line,col,"cannot resolve module '"+spec+"'",source.line_text(line));}else deps.push_back(dep);}}}
 std::sort(deps.begin(),deps.end());deps.erase(std::unique(deps.begin(),deps.end()),deps.end());return !diagnostics.has_errors();
}
bool ProgramGraph::build(const std::vector<std::string>&roots,bool follow){
 files_.clear();roots_.clear();identities_.clear();diagnostics_=Diagnostics{};std::vector<fs::path>queue;for(const auto&r:roots)queue.push_back(fs::absolute(r).lexically_normal());
 for(std::size_t i=0;i<queue.size();++i){const auto key=key_for(queue[i]);auto known=identities_.find(key);if(known!=identities_.end()){if(i<roots.size())roots_.push_back(known->second);continue;}
  SourceFile source;std::string error;if(!load_source(key,source,error)){diagnostics_.error(key,1,1,error);continue;}const auto index=files_.size();identities_[key]=index;if(i<roots.size())roots_.push_back(index);ProgramFile file;file.unit=std::make_unique<CompilationUnit>(std::move(source));
  if(file.unit->analyze()&&follow){std::vector<fs::path>deps;if(discover_module_dependencies(queue[i],file.unit->source,file.unit->tokens,deps,file.unit->diagnostics))for(const auto&dep:deps){file.dependency_paths.push_back(dep);if(!identities_.count(key_for(dep)))queue.push_back(dep);}}
  files_.push_back(std::move(file));
 }
 // Resolve forward edge placeholders after every canonical identity exists.
 for(auto&file:files_)for(const auto&path:file.dependency_paths){const auto it=identities_.find(key_for(path));if(it!=identities_.end())file.dependencies.push_back(it->second);}
 bool ok=!diagnostics_.has_errors();for(const auto&file:files_)ok=!file.unit->diagnostics.has_errors()&&ok;return ok;
}
}
