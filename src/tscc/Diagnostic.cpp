#include "Diagnostic.h"
#include <iostream>
#include <iterator>
#include <algorithm>
namespace tscc {
void Diagnostics::error(const std::string& f,std::size_t l,std::size_t c,const std::string&m,const std::string&s,const std::string&code){items_.push_back({Severity::Error,code,f,l,c,m,s});}
void Diagnostics::warning(const std::string& f,std::size_t l,std::size_t c,const std::string&m,const std::string&s,const std::string&code){items_.push_back({Severity::Warning,code,f,l,c,m,s});}
bool Diagnostics::has_errors() const { for(const auto& d:items_) if(d.severity==Severity::Error) return true; return false; }
void Diagnostics::append(Diagnostics&& other) {
    items_.insert(items_.end(),
                  std::make_move_iterator(other.items_.begin()),
                  std::make_move_iterator(other.items_.end()));
    other.items_.clear();
}
void Diagnostics::append(const Diagnostics& other) {
    items_.insert(items_.end(), other.items_.begin(), other.items_.end());
}
void Diagnostics::print(bool color) const {
    auto ordered=items_;
    std::stable_sort(ordered.begin(),ordered.end(),[](const Diagnostic&a,const Diagnostic&b){
        if(a.file!=b.file)return a.file<b.file;
        if(a.line!=b.line)return a.line<b.line;
        if(a.column!=b.column)return a.column<b.column;
        if(a.severity!=b.severity)return a.severity<b.severity;
        if(a.code!=b.code)return a.code<b.code;
        return a.message<b.message;
    });
    for(const auto& d:ordered){
        const char* label=d.severity==Severity::Error?"error":"warning";
        const char* col=d.severity==Severity::Error?"\033[1;31m":"\033[1;33m";
        if(!d.file.empty()) std::cerr<<d.file<<':'<<d.line<<':'<<d.column<<" - ";
        if (color) std::cerr << col;
        std::cerr << label;
        if (color) std::cerr << "\033[0m";
        std::cerr<<' '<<d.code<<": "<<d.message<<'\n';
        if(!d.source_line.empty()){
            std::cerr<<"  "<<d.source_line<<'\n'<<"  ";
            for(std::size_t i=1;i<d.column;++i) std::cerr<<' ';
            if (color) std::cerr << "\033[1;32m";
            std::cerr << '^';
            if (color) std::cerr << "\033[0m";
            std::cerr << '\n';
        }
    }
}
}
