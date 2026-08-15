#pragma once
#include "Source.h"
#include "Diagnostic.h"
#include <string>
#include <vector>
namespace tscc {
enum class TokenKind { Identifier, Keyword, Number, String, Template, Punct, Comment, End };
struct Token { TokenKind kind; std::string text; std::size_t begin,end; };
class Lexer { public: Lexer(const SourceFile&,Diagnostics&); std::vector<Token> lex(); private: const SourceFile& s_; Diagnostics& d_; std::size_t i_=0; void fail(std::size_t,const std::string&); };
}
