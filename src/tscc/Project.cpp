#include "Project.h"
#include <algorithm>
#include <system_error>

namespace fs=std::filesystem;
namespace tscc {

static std::string key_for(const fs::path&p){std::error_code ec;return fs::absolute(p,ec).lexically_normal().string();}

static bool resolve_relative(const fs::path& importer,const std::string& spec,fs::path& resolved){
    if(!(spec.rfind("./",0)==0||spec.rfind("../",0)==0))return false;
    fs::path base=importer.parent_path()/spec;std::vector<fs::path> candidates;
    if(base.has_extension()){
        candidates.push_back(base);
        if(base.extension()==".js"){auto x=base;x.replace_extension(".ts");candidates.push_back(x);}
        if(base.extension()==".jsx"){auto x=base;x.replace_extension(".tsx");candidates.push_back(x);}
    }
    else{candidates.push_back(base.string()+".ts");candidates.push_back(base.string()+".tsx");candidates.push_back(base/"index.ts");candidates.push_back(base/"index.tsx");}
    std::error_code ec;for(auto&c:candidates){if(fs::is_regular_file(c,ec)){resolved=fs::absolute(c,ec).lexically_normal();return true;}ec.clear();}return false;
}

bool discover_module_dependencies(const fs::path& path,const SourceFile& source,
                                  const std::vector<Token>& tokens,std::vector<fs::path>& deps,
                                  Diagnostics& diagnostics){
    deps.clear();
    if(source.text.find("import")==std::string::npos&&source.text.find("from")==std::string::npos)return true;
    for(std::size_t i=0;i+1<tokens.size();++i){
        std::size_t j=i+1;bool site=false;
        if(tokens[i].text=="import"){
            if(j<tokens.size()&&tokens[j].kind==TokenKind::String)site=true;
            else while(j<tokens.size()&&tokens[j].text!=";"&&tokens[j].kind!=TokenKind::End){
                if(tokens[j].text=="from"&&j+1<tokens.size()&&tokens[j+1].kind==TokenKind::String){++j;site=true;break;}++j;
            }
        }else if(tokens[i].text=="export"){
            while(j<tokens.size()&&tokens[j].text!=";"&&tokens[j].kind!=TokenKind::End){
                if(tokens[j].text=="from"&&j+1<tokens.size()&&tokens[j+1].kind==TokenKind::String){++j;site=true;break;}++j;
            }
        }else continue;
        if(site&&j<tokens.size()&&tokens[j].kind==TokenKind::String){
            std::string spec=tokens[j].text.substr(1,tokens[j].text.size()-2);
            if(spec.rfind("./",0)==0||spec.rfind("../",0)==0){
                fs::path dep;if(!resolve_relative(path,spec,dep)){auto [line,col]=source.line_col(tokens[j].begin);diagnostics.error(source.path,line,col,"cannot resolve module '"+spec+"'",source.line_text(line));}
                else deps.push_back(dep);
            }
        }
    }
    std::sort(deps.begin(),deps.end());deps.erase(std::unique(deps.begin(),deps.end()),deps.end());
    return !diagnostics.has_errors();
}

bool ModuleGraph::resolve_relative(const fs::path& importer,const std::string&specifier,fs::path&resolved)const{return tscc::resolve_relative(importer,specifier,resolved);}
bool ModuleGraph::visit(const fs::path&path,Diagnostics&diagnostics){
    const std::string key=key_for(path);if(seen_.count(key))return true;const std::size_t index=files_.size();seen_[key]=index;files_.push_back({});files_[index].path=key;
    std::string error;if(!load_source(key,files_[index].source,error)){diagnostics.error(key,1,1,error);return false;}
    Lexer lexer(files_[index].source,diagnostics);files_[index].tokens=lexer.lex();if(diagnostics.has_errors())return false;
    if(!discover_module_dependencies(path,files_[index].source,files_[index].tokens,files_[index].dependencies,diagnostics))return false;
    bool ok=true;for(const auto&dep:files_[index].dependencies)ok=visit(dep,diagnostics)&&ok;return ok;
}
bool ModuleGraph::build(const std::vector<std::string>&roots,Diagnostics&diagnostics){files_.clear();seen_.clear();bool ok=true;for(const auto&r:roots)ok=visit(r,diagnostics)&&ok;return ok&&!diagnostics.has_errors();}
}
