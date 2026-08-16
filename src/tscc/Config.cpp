#include "Config.h"
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <algorithm>

namespace fs=std::filesystem;
namespace tscc {

static std::string strip_jsonc(const std::string& in) {
    std::string out=in;
    bool string=false,esc=false,line=false,block=false;
    for (std::size_t i=0;i<out.size();++i) {
        char c=out[i],n=i+1<out.size()?out[i+1]:0;
        if (line) { if(c=='\n') line=false; else out[i]=' '; continue; }
        if (block) { if(c=='*'&&n=='/'){out[i]=out[i+1]=' ';++i;block=false;} else if(c!='\n')out[i]=' '; continue; }
        if (string) { if(esc)esc=false; else if(c=='\\')esc=true; else if(c=='"')string=false; continue; }
        if(c=='"'){string=true;continue;}
        if(c=='/'&&n=='/'){out[i]=out[i+1]=' ';++i;line=true;continue;}
        if(c=='/'&&n=='*'){out[i]=out[i+1]=' ';++i;block=true;continue;}
    }
    return out;
}
static bool get_string(const std::string&s,const std::string&key,std::string&v){
    std::regex r("\""+key+"\"\\s*:\\s*\"([^\"]*)\"");
    std::smatch m;if(!std::regex_search(s,m,r))return false;v=m[1].str();return true;
}
static bool get_bool(const std::string&s,const std::string&key,bool&v){
    std::regex r("\""+key+"\"\\s*:\\s*(true|false)");
    std::smatch m;if(!std::regex_search(s,m,r))return false;v=m[1].str()=="true";return true;
}
static std::vector<std::string> get_array(const std::string&s,const std::string&key){
    std::vector<std::string> out;
    std::regex r("\""+key+"\"\\s*:\\s*\\[([^\\]]*)\\]");
    std::smatch m;if(!std::regex_search(s,m,r))return out;
    std::regex q("\"([^\"]+)\"");
    auto b=std::sregex_iterator(m[1].first,m[1].second,q),e=std::sregex_iterator();
    for(;b!=e;++b)out.push_back((*b)[1].str());
    return out;
}

static std::string slash(std::string s){std::replace(s.begin(),s.end(),'\\','/');return s;}
static bool glob_match(const std::string& pattern,const std::string& value){
    const std::string p=slash(pattern),v=slash(value);
    std::string re="^";
    for(std::size_t i=0;i<p.size();++i){
        const char c=p[i];
        if(c=='*'){
            if(i+1<p.size()&&p[i+1]=='*'){++i;if(i+1<p.size()&&p[i+1]=='/'){++i;re+="(?:.*/)?";}else re+=".*";}
            else re+="[^/]*";
        }else if(c=='?')re+="[^/]";
        else {if(std::string(".^$|()[]{}+\\").find(c)!=std::string::npos)re+='\\';re+=c;}
    }
    re+="$";return std::regex_match(v,std::regex(re));
}
static bool matches_any(const std::vector<std::string>&patterns,const std::string&rel){
    for(const auto& p:patterns) {
        if(glob_match(p,rel)) return true;
    }
    return false;
}

bool load_tsconfig(const std::string& path, CompilerOptions& options,
                   std::vector<std::string>& roots, Diagnostics& diagnostics) {
    std::ifstream f(path,std::ios::binary);
    if(!f){diagnostics.error(path,1,1,"cannot read project configuration");return false;}
    std::ostringstream ss;ss<<f.rdbuf();const std::string s=strip_jsonc(ss.str());
    const fs::path dir=fs::absolute(fs::path(path)).parent_path();
    std::string v; bool bv;
    if(get_string(s,"outDir",v)) options.out_dir=(dir/v).lexically_normal().string();
    if(get_string(s,"rootDir",v)) options.root_dir=(dir/v).lexically_normal().string();
    if(get_string(s,"target",v)) options.target=v;
    if(get_string(s,"module",v)) options.module=v;
    if(get_string(s,"jsx",v)) options.jsx=v;
    if(get_bool(s,"removeComments",bv)) options.remove_comments=bv;
    if(get_bool(s,"noEmit",bv)) options.no_emit=bv;
    if(get_bool(s,"noEmitOnError",bv)) options.no_emit_on_error=bv;

    auto files=get_array(s,"files");
    auto include=get_array(s,"include");
    auto exclude=get_array(s,"exclude");
    roots.clear();
    if(!files.empty()) {
        for(const auto& x:files)roots.push_back((dir/x).lexically_normal().string());
    } else {
        // Initial project-mode default mirrors the useful tsc behavior for a
        // simple project: recursively compile .ts sources, excluding obvious
        // generated/dependency trees.
        std::error_code ec;
        fs::path out_abs=options.out_dir.empty()?fs::path():fs::absolute(options.out_dir,ec).lexically_normal();
        for(fs::recursive_directory_iterator it(dir,fs::directory_options::skip_permission_denied,ec),end;it!=end;it.increment(ec)){
            if(ec){ec.clear();continue;}
            const auto p=it->path();
            if(it->is_directory()){
                auto name=p.filename().string();
                if(name=="node_modules"||name==".git"||(!out_abs.empty()&&fs::absolute(p,ec).lexically_normal()==out_abs))
                    it.disable_recursion_pending();
                continue;
            }
            if(it->is_regular_file()&&(p.extension()==".ts"||p.extension()==".tsx")&&p.filename().string().find(".d.ts")==std::string::npos){
                std::error_code rec;const std::string rel=slash(fs::relative(p,dir,rec).string());
                const bool included=include.empty()||matches_any(include,rel);
                const bool excluded=!exclude.empty()&&matches_any(exclude,rel);
                if(included&&!excluded) roots.push_back(p.string());
            }
        }
    }
    if(roots.empty()){diagnostics.error(path,1,1,"project contains no TypeScript input files");return false;}
    if(options.root_dir.empty()) options.root_dir=dir.string();
    return true;
}
}
