#include "Project.h"
#include "Checker.h"
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
 else{candidates.push_back(base.string()+".ts");candidates.push_back(base.string()+".tsx");candidates.push_back(base.string()+".d.ts");candidates.push_back(base/"index.ts");candidates.push_back(base/"index.tsx");candidates.push_back(base/"index.d.ts");}
 std::error_code ec;for(auto&c:candidates){if(fs::is_regular_file(c,ec)){resolved=fs::absolute(c,ec).lexically_normal();return true;}ec.clear();}return false;
}
enum class ImportKind{Named,Default,Namespace};
struct ImportBinding{std::string local,exported;fs::path dependency;ImportKind kind=ImportKind::Named;};
static std::vector<ImportBinding>import_bindings(const CompilationUnit&unit){
 std::vector<ImportBinding>out;const auto&tokens=unit.tokens;
 for(const auto&range:unit.binding.import_ranges){std::size_t module=range.first;while(module<=range.second&&tokens[module].kind!=TokenKind::String)++module;if(module>range.second)continue;fs::path dependency;if(!resolve_relative(unit.source.path,tokens[module].text.substr(1,tokens[module].text.size()-2),dependency))continue;std::size_t from=range.first+1;while(from<module&&tokens[from].text!="from")++from;if(from==module)continue;std::size_t first=range.first+1;while(first<from&&tokens[first].kind==TokenKind::Comment)++first;if(first<from&&tokens[first].kind==TokenKind::Identifier){out.push_back({tokens[first].text,"default",dependency,ImportKind::Default});while(first<from&&tokens[first].text!=",")++first;if(first<from)++first;}if(first+2<from&&tokens[first].text=="*"&&tokens[first+1].text=="as"&&tokens[first+2].kind==TokenKind::Identifier){out.push_back({tokens[first+2].text,"",dependency,ImportKind::Namespace});continue;}std::size_t open=first;while(open<from&&tokens[open].text!="{")++open;if(open>=from)continue;for(std::size_t i=open+1;i<from&&tokens[i].text!="}";){if(tokens[i].text==","||tokens[i].text=="type"){++i;continue;}if(tokens[i].kind!=TokenKind::Identifier&&tokens[i].text!="default"){++i;continue;}auto exported=tokens[i++].text,local=exported;if(i+1<from&&tokens[i].text=="as"){local=tokens[i+1].text;i+=2;}out.push_back({local,exported,dependency,ImportKind::Named});while(i<from&&tokens[i].text!=","&&tokens[i].text!="}")++i;}}
 return out;
}
static bool exported_symbol(const CompilationUnit&unit,const BoundSymbol&symbol){
 const auto&tokens=unit.tokens;std::size_t begin=symbol.declaration_token;while(begin>0&&tokens[begin-1].text!=";"&&tokens[begin-1].text!="}")--begin;for(auto i=begin;i<symbol.declaration_token;++i)if(tokens[i].text=="export")return true;return false;
}
static std::vector<ProgramFile::ExportFact> collect_exports(const CompilationUnit&unit){
 std::vector<ProgramFile::ExportFact>out;
 for(std::size_t symbol=0;symbol<unit.binding.symbols.size();++symbol){const auto&bound=unit.binding.symbols[symbol];if(!exported_symbol(unit,bound))continue;std::string name=bound.name;std::size_t begin=bound.declaration_token;while(begin>0&&unit.tokens[begin-1].text!=";"&&unit.tokens[begin-1].text!="}")--begin;for(auto i=begin;i<bound.declaration_token;++i)if(unit.tokens[i].text=="default")name="default";out.push_back({name,symbol,false});}
 std::sort(out.begin(),out.end(),[](const auto&a,const auto&b){return a.name<b.name||(a.name==b.name&&a.symbol<b.symbol);});
 return out;
}
static bool link_import_types(std::vector<ProgramFile>&files,const std::unordered_map<std::string,std::size_t>&identities){
 bool ok=true;
 for(auto&file:files){bool linked=false;for(const auto&binding:import_bindings(*file.unit)){auto dep=identities.find(key_for(binding.dependency));if(dep==identities.end())continue;const auto&exporter_file=files[dep->second];const auto&exporter=*exporter_file.unit;TypeId exported=exporter.types.store.unknown();if(binding.kind==ImportKind::Namespace){std::vector<TypeProperty>properties;for(const auto&fact:exporter_file.exports)if(!fact.type_only&&fact.symbol<exporter.types.symbol_types.size()){auto type=exporter.types.symbol_types[fact.symbol];if(type!=exporter.types.store.unknown())properties.push_back({fact.name,file.unit->types.store.import_from(exporter.types.store,type),false,true});}if(!properties.empty())exported=file.unit->types.store.object_of(std::move(properties));}else for(const auto&fact:exporter_file.exports)if(!fact.type_only&&fact.name==binding.exported&&fact.symbol<exporter.types.symbol_types.size()){exported=exporter.types.symbol_types[fact.symbol];break;}if(exported==exporter.types.store.unknown())continue;for(std::size_t i=0;i<file.unit->binding.symbols.size();++i)if(file.unit->binding.symbols[i].kind==SymbolKind::Import&&file.unit->binding.symbols[i].name==binding.local){file.unit->types.symbol_types[i]=binding.kind==ImportKind::Namespace?exported:file.unit->types.store.import_from(exporter.types.store,exported);linked=true;break;}}
  if(linked){Diagnostics graph_check;ExpressionModel expressions;if(!check_program(file.unit->source,file.unit->tokens,file.unit->program,file.unit->semantic,file.unit->binding,file.unit->types,expressions,graph_check))ok=false;file.unit->diagnostics.append(std::move(graph_check));}
 }
 return ok;
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
  if(file.unit->analyze()){file.exports=collect_exports(*file.unit);if(follow){std::vector<fs::path>deps;if(discover_module_dependencies(queue[i],file.unit->source,file.unit->tokens,deps,file.unit->diagnostics))for(const auto&dep:deps){file.dependency_paths.push_back(dep);if(!identities_.count(key_for(dep)))queue.push_back(dep);}}}
  files_.push_back(std::move(file));
 }
 // Resolve forward edge placeholders after every canonical identity exists.
 for(auto&file:files_)for(const auto&path:file.dependency_paths){const auto it=identities_.find(key_for(path));if(it!=identities_.end())file.dependencies.push_back(it->second);}
 bool ok=!diagnostics_.has_errors();if(follow)ok=link_import_types(files_,identities_)&&ok;for(const auto&file:files_)ok=!file.unit->diagnostics.has_errors()&&ok;return ok;
}
}
