#include "Lexer.h"
#include <cctype>
#include <unordered_set>
namespace tscc {
static bool id0(unsigned char c){return std::isalpha(c)||c=='_'||c=='$'||c>=128;}
static bool idc(unsigned char c){return id0(c)||std::isdigit(c);}
Lexer::Lexer(const SourceFile&s,Diagnostics&d):s_(s),d_(d){}
void Lexer::fail(std::size_t p,const std::string&m){auto [l,c]=s_.line_col(p);d_.error(s_.path,l,c,m,s_.line_text(l));}
std::vector<Token> Lexer::lex(){std::vector<Token> v; static const std::unordered_set<std::string> kws={"interface","type","enum","namespace","declare","abstract","implements","public","private","protected","readonly","as","satisfies","keyof","typeof","infer","extends","function","class","const","let","var","return","export","import","from","async","await","new","this","static"};
 while(i_<s_.text.size()){unsigned char c=s_.text[i_];if(std::isspace(c)){++i_;continue;}std::size_t b=i_;
  if(id0(c)){++i_;while(i_<s_.text.size()&&idc((unsigned char)s_.text[i_]))++i_;auto x=s_.text.substr(b,i_-b);v.push_back({kws.count(x)?TokenKind::Keyword:TokenKind::Identifier,x,b,i_});continue;}
  if(std::isdigit(c)){++i_;while(i_<s_.text.size()&&(std::isalnum((unsigned char)s_.text[i_])||s_.text[i_]=='.'||s_.text[i_]=='_'))++i_;v.push_back({TokenKind::Number,s_.text.substr(b,i_-b),b,i_});continue;}
  if(c=='\''||c=='"'||c=='`'){char q=c;++i_;bool esc=false;for(;i_<s_.text.size();++i_){char x=s_.text[i_];if(esc)esc=false;else if(x=='\\')esc=true;else if(x==q){++i_;break;}}if(i_>s_.text.size()||s_.text[i_-1]!=q)fail(b,"unterminated string/template literal");v.push_back({q=='`'?TokenKind::Template:TokenKind::String,s_.text.substr(b,i_-b),b,i_});continue;}
  if(c=='/'&&i_+1<s_.text.size()&&s_.text[i_+1]=='/'){i_+=2;while(i_<s_.text.size()&&s_.text[i_]!='\n')++i_;v.push_back({TokenKind::Comment,s_.text.substr(b,i_-b),b,i_});continue;}
  if(c=='/'&&i_+1<s_.text.size()&&s_.text[i_+1]=='*'){i_+=2;auto e=s_.text.find("*/",i_);if(e==std::string::npos){fail(b,"unterminated block comment");i_=s_.text.size();}else i_=e+2;v.push_back({TokenKind::Comment,s_.text.substr(b,i_-b),b,i_});continue;}
  static const char* triples[]={"...","===","!==",">>>","**=","?" "?="}; bool done=false;for(auto z:triples){std::string q=z;if(s_.text.compare(i_,q.size(),q)==0){i_+=q.size();v.push_back({TokenKind::Punct,q,b,i_});done=true;break;}}if(done)continue;
  static const char* pairs[]={"=>","==","!=","<=","++","--","&&","||","??","?.","**","<<",">>","+=","-=","*=","/=","%=","::"};for(auto z:pairs){std::string q=z;if(s_.text.compare(i_,q.size(),q)==0){i_+=q.size();v.push_back({TokenKind::Punct,q,b,i_});done=true;break;}}if(done)continue;
  ++i_;v.push_back({TokenKind::Punct,s_.text.substr(b,1),b,i_});
 }
 v.push_back({TokenKind::End,"",s_.text.size(),s_.text.size()});return v;}
}
