#include "SourceMap.h"
namespace tscc { namespace { std::string quoted(const std::string&s){std::string out="\"";for(unsigned char c:s){if(c=='\\'||c=='\"')out.push_back('\\');if(c=='\n')out+="\\n";else if(c=='\r')out+="\\r";else out.push_back(static_cast<char>(c));}return out+'\"';} }
std::string make_line_source_map(const std::string&output,const std::string&source,const std::string&emitted){std::size_t lines=1;for(char c:emitted)if(c=='\n')++lines;std::string mappings;for(std::size_t line=0;line<lines;++line){if(line)mappings.push_back(';');mappings+=line?"AACA":"AAAA";}return "{\"version\":3,\"file\":"+quoted(output)+",\"sourceRoot\":\"\",\"sources\":["+quoted(source)+"],\"names\":[],\"mappings\":"+quoted(mappings)+"}\n";}
}
