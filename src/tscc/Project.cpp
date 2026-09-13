#include "Project.h"
#include "Checker.h"
#include "Source.h"
#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <system_error>

namespace fs=std::filesystem;
namespace tscc {
static std::string key_for(const fs::path&p){std::error_code ec;return fs::absolute(p,ec).lexically_normal().string();}
static bool regular(const fs::path&p){std::error_code ec;return fs::is_regular_file(p,ec);}
static std::string package_field(const std::string&text,const char*name){std::smatch match;if(std::regex_search(text,match,std::regex(std::string("\\\"")+name+"\\\"\\s*:\\s*\\\"([^\\\"]+)\\\"")))return match[1].str();return {};}
ResolutionResult resolve_relative_module(const fs::path&importer,const std::string&spec){
 ResolutionResult result;if(!(spec.rfind("./",0)==0||spec.rfind("../",0)==0))return result;result.status=ResolutionStatus::Missing;
 fs::path base=importer.parent_path()/spec;std::vector<fs::path>candidates;
 if(base.has_extension()){candidates.push_back(base);const auto ext=base.extension().string();if(ext==".js"||ext==".mjs"||ext==".cjs"){auto stem=base;stem.replace_extension();for(const auto*suffix:{".ts",".tsx",".mts",".cts",".d.ts",".d.mts",".d.cts"})candidates.push_back(stem.string()+suffix);}else if(ext==".jsx"){auto x=base;x.replace_extension(".tsx");candidates.push_back(x);}}
 else{for(const auto*suffix:{".ts",".tsx",".mts",".cts",".d.ts",".d.mts",".d.cts"})candidates.push_back(base.string()+suffix);for(const auto*suffix:{"index.ts","index.tsx","index.mts","index.cts","index.d.ts","index.d.mts","index.d.cts"})candidates.push_back(base/suffix);}
 if(regular(base/"package.json")){std::ifstream input(base/"package.json",std::ios::binary);std::ostringstream buffer;buffer<<input.rdbuf();const auto text=buffer.str();const auto types=package_field(text,"types"),typings=package_field(text,"typings");if(!types.empty()&&!typings.empty()&&types!=typings&&regular(base/types)&&regular(base/typings)){result.status=ResolutionStatus::Ambiguous;result.candidates={fs::absolute(base/types).lexically_normal(),fs::absolute(base/typings).lexically_normal()};return result;}const auto entry=!types.empty()?types:!typings.empty()?typings:package_field(text,"main");if(!entry.empty())candidates.insert(candidates.begin(),base/entry);}
 std::error_code ec;for(const auto&candidate:candidates)if(regular(candidate)){result.status=ResolutionStatus::Resolved;result.path=fs::absolute(candidate,ec).lexically_normal();result.candidates={result.path};return result;}return result;
}
enum class ImportKind{Named,Default,Namespace};
struct ImportBinding{std::string local,exported;fs::path dependency;ImportKind kind=ImportKind::Named;};
static std::vector<ImportBinding>import_bindings(const CompilationUnit&unit){
 std::vector<ImportBinding>out;const auto&tokens=unit.tokens;
 for(const auto&range:unit.binding.import_ranges){std::size_t module=range.first;while(module<=range.second&&tokens[module].kind!=TokenKind::String)++module;if(module>range.second)continue;const auto resolution=resolve_relative_module(unit.source.path,tokens[module].text.substr(1,tokens[module].text.size()-2));if(resolution.status!=ResolutionStatus::Resolved)continue;const auto&dependency=resolution.path;std::size_t from=range.first+1;while(from<module&&tokens[from].text!="from")++from;if(from==module)continue;std::size_t first=range.first+1;while(first<from&&tokens[first].kind==TokenKind::Comment)++first;if(first<from&&tokens[first].kind==TokenKind::Identifier){out.push_back({tokens[first].text,"default",dependency,ImportKind::Default});while(first<from&&tokens[first].text!=",")++first;if(first<from)++first;}if(first+2<from&&tokens[first].text=="*"&&tokens[first+1].text=="as"&&tokens[first+2].kind==TokenKind::Identifier){out.push_back({tokens[first+2].text,"",dependency,ImportKind::Namespace});continue;}std::size_t open=first;while(open<from&&tokens[open].text!="{")++open;if(open>=from)continue;for(std::size_t i=open+1;i<from&&tokens[i].text!="}";){if(tokens[i].text==","||tokens[i].text=="type"){++i;continue;}if(tokens[i].kind!=TokenKind::Identifier&&tokens[i].text!="default"){++i;continue;}auto exported=tokens[i++].text,local=exported;if(i+1<from&&tokens[i].text=="as"){local=tokens[i+1].text;i+=2;}out.push_back({local,exported,dependency,ImportKind::Named});while(i<from&&tokens[i].text!=","&&tokens[i].text!="}")++i;}}
 return out;
}
static bool exported_symbol(const CompilationUnit&unit,const BoundSymbol&symbol){
 const auto&tokens=unit.tokens;std::size_t begin=symbol.declaration_token;while(begin>0&&tokens[begin-1].text!=";"&&tokens[begin-1].text!="}")--begin;for(auto i=begin;i<symbol.declaration_token;++i)if(tokens[i].text=="export")return true;return false;
}
static std::vector<ProgramFile::ExportFact> collect_exports(const CompilationUnit&unit,std::size_t file){
 std::vector<ProgramFile::ExportFact>out;
 for(std::size_t symbol=0;symbol<unit.binding.symbols.size();++symbol){const auto&bound=unit.binding.symbols[symbol];if(!exported_symbol(unit,bound))continue;std::string name=bound.name;std::size_t begin=bound.declaration_token;while(begin>0&&unit.tokens[begin-1].text!=";"&&unit.tokens[begin-1].text!="}")--begin;bool type_only=false;for(auto i=begin;i<bound.declaration_token;++i){if(unit.tokens[i].text=="default")name="default";if(unit.tokens[i].text=="type"||unit.tokens[i].text=="interface")type_only=true;}out.push_back({name,file,symbol,type_only});}
 std::sort(out.begin(),out.end(),[](const auto&a,const auto&b){return a.name<b.name||(a.name==b.name&&a.symbol<b.symbol);});
 return out;
}
static void link_reexports(std::vector<ProgramFile>&files,const std::unordered_map<std::string,std::size_t>&identities){
 bool changed=true;for(std::size_t round=0;changed&&round<=files.size();++round){changed=false;for(std::size_t owner=0;owner<files.size();++owner){auto&file=files[owner];const auto&tokens=file.unit->tokens;for(std::size_t i=0;i<tokens.size();++i){if(tokens[i].text!="export")continue;std::size_t end=i+1,from=tokens.size(),module=tokens.size();while(end<tokens.size()&&tokens[end].text!=";"&&tokens[end].kind!=TokenKind::End){if(tokens[end].text=="from"&&end+1<tokens.size()&&tokens[end+1].kind==TokenKind::String){from=end;module=end+1;}++end;}if(module>=tokens.size())continue;const auto resolution=resolve_relative_module(file.unit->source.path,tokens[module].text.substr(1,tokens[module].text.size()-2));if(resolution.status!=ResolutionStatus::Resolved)continue;auto found=identities.find(key_for(resolution.path));if(found==identities.end())continue;const auto source=files[found->second].exports;auto add=[&](const ProgramFile::ExportFact&fact,const std::string&name,bool type_only){if(name=="default"&&tokens[i+1].text=="*")return;for(const auto&existing:file.exports)if(existing.name==name&&existing.type_only==type_only)return;file.exports.push_back({name,fact.file,fact.symbol,type_only||fact.type_only});changed=true;};if(i+1<from&&tokens[i+1].text=="*"){for(const auto&fact:source)add(fact,fact.name,false);continue;}std::size_t open=i+1;bool clause_type=false;if(open<from&&tokens[open].text=="type"){clause_type=true;++open;}while(open<from&&tokens[open].text!="{")++open;if(open>=from)continue;for(std::size_t p=open+1;p<from&&tokens[p].text!="}";){if(tokens[p].text==","){++p;continue;}bool type_only=clause_type;if(tokens[p].text=="type"){type_only=true;++p;}if(p>=from)break;const auto imported=tokens[p++].text;auto exported=imported;if(p+1<from&&tokens[p].text=="as"){exported=tokens[p+1].text;p+=2;}for(const auto&fact:source)if(fact.name==imported){add(fact,exported,type_only);break;}while(p<from&&tokens[p].text!=","&&tokens[p].text!="}")++p;}}}}
 for(auto&file:files)std::sort(file.exports.begin(),file.exports.end(),[](const auto&a,const auto&b){if(a.name!=b.name)return a.name<b.name;if(a.type_only!=b.type_only)return a.type_only<b.type_only;if(a.file!=b.file)return a.file<b.file;return a.symbol<b.symbol;});
}
static bool link_import_types(std::vector<ProgramFile>&files,const std::unordered_map<std::string,std::size_t>&identities){
 bool ok=true;
 for(auto&file:files){bool linked=false;for(const auto&binding:import_bindings(*file.unit)){auto dep=identities.find(key_for(binding.dependency));if(dep==identities.end())continue;const auto&exporter_file=files[dep->second];TypeId exported=file.unit->types.store.unknown();if(binding.kind==ImportKind::Namespace){std::vector<TypeProperty>properties;for(const auto&fact:exporter_file.exports)if(!fact.type_only&&fact.file<files.size()){const auto&origin=*files[fact.file].unit;if(fact.symbol<origin.types.symbol_types.size()){auto type=origin.types.symbol_types[fact.symbol];if(type!=origin.types.store.unknown())properties.push_back({fact.name,file.unit->types.store.import_from(origin.types.store,type),false,true});}}if(!properties.empty())exported=file.unit->types.store.object_of(std::move(properties));}else for(const auto&fact:exporter_file.exports)if(!fact.type_only&&fact.name==binding.exported&&fact.file<files.size()){const auto&origin=*files[fact.file].unit;if(fact.symbol<origin.types.symbol_types.size()){auto type=origin.types.symbol_types[fact.symbol];if(type!=origin.types.store.unknown())exported=file.unit->types.store.import_from(origin.types.store,type);}break;}if(exported==file.unit->types.store.unknown())continue;for(std::size_t i=0;i<file.unit->binding.symbols.size();++i)if(file.unit->binding.symbols[i].kind==SymbolKind::Import&&file.unit->binding.symbols[i].name==binding.local){file.unit->types.symbol_types[i]=exported;linked=true;break;}}
  if(linked){Diagnostics graph_check;ExpressionModel expressions;if(!check_program(file.unit->source,file.unit->tokens,file.unit->program,file.unit->semantic,file.unit->binding,file.unit->types,expressions,graph_check))ok=false;file.unit->diagnostics.append(std::move(graph_check));}
 }
 return ok;
}
bool discover_module_dependencies(const fs::path&path,const SourceFile&source,const std::vector<Token>&tokens,std::vector<fs::path>&deps,Diagnostics&diagnostics){
 deps.clear();if(source.text.find("import")==std::string::npos&&source.text.find("from")==std::string::npos)return true;
 for(std::size_t i=0;i+1<tokens.size();++i){std::size_t j=i+1;bool site=false;if(tokens[i].text=="import"){if(j<tokens.size()&&tokens[j].kind==TokenKind::String)site=true;else while(j<tokens.size()&&tokens[j].text!=";"&&tokens[j].kind!=TokenKind::End){if(tokens[j].text=="from"&&j+1<tokens.size()&&tokens[j+1].kind==TokenKind::String){++j;site=true;break;}++j;}}else if(tokens[i].text=="export"){while(j<tokens.size()&&tokens[j].text!=";"&&tokens[j].kind!=TokenKind::End){if(tokens[j].text=="from"&&j+1<tokens.size()&&tokens[j+1].kind==TokenKind::String){++j;site=true;break;}++j;}}else continue;
  if(site&&j<tokens.size()&&tokens[j].kind==TokenKind::String){std::string spec=tokens[j].text.substr(1,tokens[j].text.size()-2);if(spec.rfind("./",0)==0||spec.rfind("../",0)==0){const auto resolution=resolve_relative_module(path,spec);if(resolution.status==ResolutionStatus::Resolved)deps.push_back(resolution.path);else{auto[line,col]=source.line_col(tokens[j].begin);const auto message=resolution.status==ResolutionStatus::Ambiguous?"ambiguous module '"+spec+"'":"cannot resolve module '"+spec+"'";diagnostics.error(source.path,line,col,message,source.line_text(line));}}}}
 std::sort(deps.begin(),deps.end());deps.erase(std::unique(deps.begin(),deps.end()),deps.end());return !diagnostics.has_errors();
}
bool ProgramGraph::build(const std::vector<std::string>&roots,bool follow){
 files_.clear();roots_.clear();identities_.clear();diagnostics_=Diagnostics{};std::vector<fs::path>queue;for(const auto&r:roots)queue.push_back(fs::absolute(r).lexically_normal());
 for(std::size_t i=0;i<queue.size();++i){const auto key=key_for(queue[i]);auto known=identities_.find(key);if(known!=identities_.end()){if(i<roots.size())roots_.push_back(known->second);continue;}
  SourceFile source;std::string error;if(!load_source(key,source,error)){diagnostics_.error(key,1,1,error);continue;}const auto index=files_.size();identities_[key]=index;if(i<roots.size())roots_.push_back(index);ProgramFile file;file.unit=std::make_unique<CompilationUnit>(std::move(source));
  if(file.unit->analyze()){file.exports=collect_exports(*file.unit,index);if(follow){std::vector<fs::path>deps;if(discover_module_dependencies(queue[i],file.unit->source,file.unit->tokens,deps,file.unit->diagnostics))for(const auto&dep:deps){file.dependency_paths.push_back(dep);if(!identities_.count(key_for(dep)))queue.push_back(dep);}}}
  files_.push_back(std::move(file));
 }
 // Resolve forward edge placeholders after every canonical identity exists.
 for(auto&file:files_)for(const auto&path:file.dependency_paths){const auto it=identities_.find(key_for(path));if(it!=identities_.end())file.dependencies.push_back(it->second);}
 bool ok=!diagnostics_.has_errors();if(follow){link_reexports(files_,identities_);ok=link_import_types(files_,identities_)&&ok;}for(const auto&file:files_)ok=!file.unit->diagnostics.has_errors()&&ok;return ok;
}
}
