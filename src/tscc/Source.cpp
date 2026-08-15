#include "Source.h"
#include <fstream>
#include <sstream>
#include <algorithm>
namespace tscc {
bool load_source(const std::string& p,SourceFile&o,std::string&e){std::ifstream f(p,std::ios::binary);if(!f){e="cannot open file";return false;}std::ostringstream s;s<<f.rdbuf();o.path=p;o.text=s.str();o.line_starts={0};for(std::size_t i=0;i<o.text.size();++i)if(o.text[i]=='\n')o.line_starts.push_back(i+1);return true;}
std::pair<std::size_t,std::size_t> SourceFile::line_col(std::size_t off)const{auto it=std::upper_bound(line_starts.begin(),line_starts.end(),off);std::size_t idx=it==line_starts.begin()?0:std::size_t(it-line_starts.begin()-1);return {idx+1,off-line_starts[idx]+1};}
std::string SourceFile::line_text(std::size_t line)const{if(!line||line>line_starts.size())return{};auto a=line_starts[line-1],b=(line<line_starts.size()?line_starts[line]-1:text.size());if(b>a&&text[b-1]=='\r')--b;return text.substr(a,b-a);}
}
