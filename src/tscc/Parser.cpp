#include "Parser.h"
#include <algorithm>
#include <cstring>
#include <functional>
#include <unordered_map>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace tscc {

Parser::Parser(const SourceFile& source, const std::vector<Token>& tokens, Diagnostics& diagnostics,
               ParserLimits limits)
    : source_(source), tokens_(tokens), diagnostics_(diagnostics), limits_(limits) {}

bool Parser::at_end() const { return i_ >= tokens_.size() || tokens_[i_].kind == TokenKind::End; }
const Token& Parser::token(std::size_t offset) const {
    const std::size_t p = std::min(i_ + offset, tokens_.size() - 1);
    return tokens_[p];
}
bool Parser::is(const char* text, std::size_t offset) const { return token(offset).text == text; }
bool Parser::consume(const char* text) {
    if (!is(text)) return false;
    ++i_;
    return true;
}
void Parser::error_at(std::size_t index, const std::string& message) {
    if (index >= tokens_.size()) index = tokens_.size() - 1;
    const auto [line, column] = source_.line_col(tokens_[index].begin);
    diagnostics_.error(source_.path, line, column, message, source_.line_text(line));
    if(std::find(recovery_tokens_.begin(),recovery_tokens_.end(),index)==recovery_tokens_.end())recovery_tokens_.push_back(index);
}

void Parser::finalize_syntax(SyntaxNode&node,SyntaxNodeId parent,SyntaxNodeId&next){
    node.id=next++;node.parent_id=parent;
    if(!tokens_.empty()){node.begin_token=std::min(node.begin_token,tokens_.size()-1);node.end_token=std::min(std::max(node.begin_token,node.end_token),tokens_.size()-1);node.begin_offset=tokens_[node.begin_token].begin;node.end_offset=tokens_[node.end_token].end;}
    for(auto&child:node.children)finalize_syntax(child,node.id,next);
}
void Parser::erase_tokens(std::size_t first, std::size_t last_exclusive) {
    if (!program_ || first >= last_exclusive || first >= tokens_.size()) return;
    last_exclusive = std::min(last_exclusive, tokens_.size());
    program_->erasures.push_back({tokens_[first].begin, tokens_[last_exclusive - 1].end});
}

void Parser::add_replacement(std::size_t begin, std::size_t end, std::string text) {
    if (!program_ || begin > end || end > source_.text.size()) return;
    program_->replacements.push_back({begin, end, std::move(text)});
}

void Parser::erase_generic_list(std::size_t open, std::size_t close) {
    if (open < tokens_.size() && close < tokens_.size() && tokens_[open].text == "<" &&
        (tokens_[close].text == ">" || tokens_[close].text == ">>" || tokens_[close].text == ">>>"))
        erase_tokens(open, close + 1);
}

bool Parser::validate_generic_list(std::size_t open, std::size_t close) {
    if (open >= close || open >= tokens_.size()) return false;
    bool expect_parameter = true;
    int nested = 0;
    for (std::size_t p = open + 1; p < close; ++p) {
        if (tokens_[p].kind == TokenKind::Comment) continue;
        const auto& x = tokens_[p].text;
        if (x == "<" || x == "(" || x == "[" || x == "{") { ++nested; continue; }
        if (x == ">" || x == ")" || x == "]" || x == "}") { if (nested) --nested; continue; }
        if (!nested && x == "extends" &&
            (p + 1 >= close || tokens_[p+1].text == ",")) {
            error_at(p, "generic type parameter constraint requires a type");
            return false;
        }
        if (!nested && x == ",") {
            if (expect_parameter) { error_at(p, "expected type parameter before ','"); return false; }
            expect_parameter = true;
            continue;
        }
        if (!nested && expect_parameter) {
            // TypeScript 5.x supports `const T` type parameters. `in`/`out`
            // variance modifiers are identifiers in our lexer and naturally
            // fall through the same declaration grammar.
            if (tokens_[p].text == "const") continue;
            if (tokens_[p].kind != TokenKind::Identifier) {
                error_at(p, "expected type parameter name");
                return false;
            }
            expect_parameter = false;
        }
    }
    // A trailing comma is valid TypeScript (`<T,>`). A dangling modifier such
    // as `<const>` is not: if we're still expecting a parameter, only a comma
    // immediately before the close can explain that state.
    if (expect_parameter) {
        std::size_t last=close;
        while(last>open+1 && tokens_[last-1].kind==TokenKind::Comment)--last;
        if(last>open+1 && tokens_[last-1].text!=",") {
            error_at(last-1,"expected type parameter name");
            return false;
        }
    }
    return true;
}

void Parser::parse_object_literal_range(std::size_t open, std::size_t close) {
    for (std::size_t p = open + 1; p < close; ++p) {
        if (tokens_[p].text == "{") {
            const auto nested = find_matching(p, "{", "}");
            if (nested < close) { parse_object_literal_range(p, nested); p = nested; }
            continue;
        }
        // Generic object method: `f<T>(x:T):T { ... }`.
        if(tokens_[p].kind==TokenKind::Identifier && p+2<close && tokens_[p+1].text=="<") {
            const auto gc=find_matching(p+1,"<",">");
            if(gc<close && gc+1<close && tokens_[gc+1].text=="(") {
                const auto pc=find_matching(gc+1,"(",")");
                if(pc>=close){error_at(gc+1,"unterminated object method parameter list");return;}
                erase_generic_list(p+1,gc);
                parse_parameter_list(gc+1,pc);
                std::size_t after=pc+1;
                if(after<close&&tokens_[after].text==":") {
                    erase_type_annotation(after,close,{"{"});
                    while(after<close&&tokens_[after].text!="{")++after;
                }
                if(after<close&&tokens_[after].text=="{") {
                    const auto bc=find_matching(after,"{","}");
                    if(bc<close){parse_runtime_range(after+1,bc);p=bc;continue;}
                }
            }
        }
        if (tokens_[p].text != "(") continue;
        const auto pc = find_matching(p, "(", ")");
        if (pc >= close) { error_at(p, "unterminated object method parameter list"); return; }
        // A method/accessor parameter list is preceded by a property name, or
        // by `get name` / `set name`. Calls inside property initializers are not.
        if (p == open + 1) continue;
        const auto& prev = tokens_[p - 1];
        if (!(prev.kind == TokenKind::Identifier || prev.kind == TokenKind::String ||
              prev.kind == TokenKind::Number || prev.text == "]")) continue;
        parse_parameter_list(p, pc);
        std::size_t after = pc + 1;
        if (after < close && tokens_[after].text == ":") {
            erase_type_annotation(after, close, {"{"});
            while (after < close && tokens_[after].text != "{") ++after;
        }
        if (after < close && tokens_[after].text == "{") {
            const auto bc = find_matching(after, "{", "}");
            if (bc < close) { parse_runtime_range(after + 1, bc); p = bc; }
        }
    }
}

void Parser::erase_expression_type_arguments(std::size_t first, std::size_t end) {
    // Common runtime type-argument applications: fn<T>(...), new Box<T>(...),
    // obj.method<T>(...). The closing '>' must be followed by a call/member
    // continuation, avoiding ordinary comparison expressions.
    for (std::size_t p = first; p + 2 < end; ++p) {
        if (tokens_[p].text != "<") continue;
        if (p == first) continue;
        // In TSX, `</Tag>` is JSX structure, never an expression type-argument
        // application. Without this guard a ternary child such as
        // `cond ? <A/> : <B/>` can erase the closing `</B>`.
        if (source_.path.size() >= 4 && source_.path.substr(source_.path.size()-4)==".tsx" &&
            p + 1 < end && tokens_[p+1].text == "/") continue;
        const auto& prev = tokens_[p - 1];
        if (!(prev.kind == TokenKind::Identifier || prev.text == ")" || prev.text == "]")) continue;
        int depth = 0;
        std::size_t q = p;
        for (; q < end; ++q) {
            if (tokens_[q].text == "<") ++depth;
            else if (tokens_[q].text == ">") { if (--depth <= 0) break; }
            else if (tokens_[q].text == ">>" || tokens_[q].text == ">>>") {
                depth -= static_cast<int>(tokens_[q].text.size());
                if (depth <= 0) break;
            }
        }
        if (q >= end || q + 1 >= tokens_.size()) continue;
        const auto& next = tokens_[q + 1].text;
        if (next == "(" || next == "." || next == "?." || next == "[" ||
            next == ";" || next == "," || next == ")" || next == "]" ||
            next == "}" || next == "=" || next == "=>") {
            // Expression type arguments contain arbitrary types, not type
            // parameter declarations. Validate only their comma structure;
            // object/tuple/conditional types are all legal arguments.
            bool bad=false, expect_type=true; int nested=0;
            for(std::size_t r=p+1;r<q;++r){
                if(tokens_[r].kind==TokenKind::Comment)continue;
                const auto& gx=tokens_[r].text;
                if(gx=="<"||gx=="("||gx=="["||gx=="{"){++nested;expect_type=false;continue;}
                if(gx==">"||gx==")"||gx=="]"||gx=="}"){if(nested)--nested;expect_type=false;continue;}
                if(!nested&&gx==","){if(expect_type){error_at(r,"expected type argument before ','");bad=true;break;}expect_type=true;continue;}
                if(!nested)expect_type=false;
            }
            if(!bad) erase_generic_list(p,q);
            p=q;
        }
    }
}

std::size_t Parser::find_matching(std::size_t open_index, const char* open, const char* close) const {
    int depth = 0;
    const bool angles = std::string(open) == "<" && std::string(close) == ">";
    for (std::size_t p = open_index; p < tokens_.size(); ++p) {
        const auto& x = tokens_[p].text;
        if (x == open) { ++depth; continue; }
        if (angles && (x == ">>" || x == ">>>")) {
            depth -= static_cast<int>(x.size());
            if (depth <= 0) return p;
            continue;
        }
        if (x == close && --depth == 0) return p;
    }
    return tokens_.size();
}

std::size_t Parser::find_statement_end(std::size_t start) const {
    int paren = 0, square = 0, brace = 0, angle = 0;
    for (std::size_t p = start; p < tokens_.size(); ++p) {
        const auto& x = tokens_[p].text;
        if (x == "(") ++paren; else if (x == ")" && paren) --paren;
        else if (x == "[") ++square; else if (x == "]" && square) --square;
        else if (x == "{") ++brace; else if (x == "}" && brace) --brace;
        else if (x == "<") ++angle;
        else if (x == ">" && angle) --angle;
        else if ((x == ">>" || x == ">>>") && angle)
            angle = std::max(0, angle - static_cast<int>(x.size()));
        else if (x == ";" && !paren && !square && !brace && !angle) return p;
        if (tokens_[p].kind == TokenKind::End) return p;
    }
    return tokens_.size() - 1;
}

std::size_t Parser::recovery_boundary(std::size_t start) const {
    bool separator = false;
    for (std::size_t p = start; p < tokens_.size(); ++p) {
        if (tokens_[p].kind == TokenKind::End) break;
        if (tokens_[p].kind == TokenKind::Comment) continue;
        if (tokens_[p].text == ";" || tokens_[p].text == "}") { separator = true; continue; }
        if (!separator) continue;
        const auto& x = tokens_[p].text;
        if (x == "export" || x == "declare" || x == "interface" || x == "type" ||
            x == "function" || x == "class" || x == "enum" || x == "namespace" ||
            x == "module" || x == "const" || x == "let" || x == "var") return p;
        separator = false;
    }
    return tokens_.size();
}

void Parser::erase_type_annotation(std::size_t colon, std::size_t limit,
                                   const std::vector<std::string>& terminators) {
    if (colon >= limit || tokens_[colon].text != ":") return;
    int angle = 0, square = 0, paren = 0, brace = 0;
    std::size_t p = colon + 1;
    for (; p < limit; ++p) {
        const auto& x = tokens_[p].text;
        if (!angle && !square && !paren && !brace &&
            std::find(terminators.begin(), terminators.end(), x) != terminators.end())
            break;
        if (x == "<") ++angle;
        else if (x == ">" && angle) --angle;
        else if ((x == ">>" || x == ">>>") && angle)
            angle = std::max(0, angle - static_cast<int>(x.size()));
        else if (x == "[") ++square;
        else if (x == "]" && square) --square;
        else if (x == "(") ++paren;
        else if (x == ")" && paren) --paren;
        else if (x == "{") ++brace;
        else if (x == "}" && brace) --brace;
    }
    std::size_t first_type = colon + 1;
    while (first_type < p && tokens_[first_type].kind == TokenKind::Comment) ++first_type;
    if (first_type >= p || (first_type < limit && (tokens_[first_type].text == ":" || tokens_[first_type].text == "::"))) {
        error_at(colon, "expected type after ':'");
        return;
    }
    if (tokens_[first_type].text == "readonly" &&
        (first_type + 1 >= p || tokens_[first_type + 1].text == "=" ||
         tokens_[first_type + 1].text == "," || tokens_[first_type + 1].text == ";")) {
        error_at(first_type, "readonly type modifier requires an array or tuple type");
        return;
    }
    std::size_t last_type=p;
    while(last_type>first_type && tokens_[last_type-1].kind==TokenKind::Comment)--last_type;
    if(last_type>first_type) {
        const auto& last=tokens_[last_type-1].text;
        if(last=="keyof"||last=="typeof"||last=="readonly"||last=="unique"||
           last=="infer"||last=="extends") {
            error_at(last_type-1,"type operator requires an operand");
            return;
        }
    }
    // Type predicates require a type after `is`.
    for (std::size_t q=first_type; q<p; ++q)
        if (tokens_[q].text=="is" && q+1>=p) {
            error_at(q,"type predicate requires a type after 'is'");
            return;
        }
    erase_tokens(colon, p);
}

void Parser::parse_parameter_list(std::size_t open, std::size_t close) {
    std::size_t p = open + 1;
    int nested_paren = 0, nested_square = 0, nested_brace = 0;
    while (p < close) {
        // A `this: Type` parameter is compile-time only and is not a JavaScript
        // formal parameter. It may only appear first; erase it including the
        // following comma when one is present.
        if (p == open + 1 && tokens_[p].text == "this" && p + 1 < close && tokens_[p + 1].text == ":") {
            std::size_t q = p + 2;
            int angle = 0, square = 0, paren = 0, brace = 0;
            for (; q < close; ++q) {
                const auto& x = tokens_[q].text;
                if (!angle && !square && !paren && !brace && x == ",") { ++q; break; }
                if (x == "<") ++angle; else if (x == ">" && angle) --angle;
                else if (x == "[") ++square; else if (x == "]" && square) --square;
                else if (x == "(") ++paren; else if (x == ")" && paren) --paren;
                else if (x == "{") ++brace; else if (x == "}" && brace) --brace;
            }
            erase_tokens(p, q);
            p = q;
            continue;
        }
        // Modifiers used by constructor parameter properties are erased here;
        // runtime field assignment is a later transform checkpoint.
        if (tokens_[p].text == "public" || tokens_[p].text == "private" ||
            tokens_[p].text == "protected" || tokens_[p].text == "readonly") {
            erase_tokens(p, p + 1);
            ++p;
            continue;
        }
        if (tokens_[p].text == "(") ++nested_paren;
        else if (tokens_[p].text == ")" && nested_paren) --nested_paren;
        else if (tokens_[p].text == "[") ++nested_square;
        else if (tokens_[p].text == "]" && nested_square) --nested_square;
        else if (tokens_[p].text == "{") ++nested_brace;
        else if (tokens_[p].text == "}" && nested_brace) --nested_brace;

        if (!nested_paren && !nested_square && !nested_brace) {
            if (tokens_[p].text == "::") {
                error_at(p, "invalid parameter type separator '::'");
                ++p; continue;
            }
            if (tokens_[p].text == "??" ||
                (tokens_[p].text == "?" && p + 1 < close && tokens_[p + 1].text == "?")) {
                error_at(p, "invalid optional parameter marker");
                ++p; continue;
            }
            if (tokens_[p].text == "?" && p + 1 <= close &&
                (tokens_[p + 1].text == ":" || tokens_[p + 1].text == "=" ||
                 tokens_[p + 1].text == "," || tokens_[p + 1].text == ")"))
                erase_tokens(p, p + 1);
            if (tokens_[p].text == ":" && p > open + 1)
                erase_type_annotation(p, close, {",", ")", "="});
        }
        ++p;
    }
}

void Parser::parse_variable_declarators(std::size_t first, std::size_t end) {
    // Declaration-head annotations are unambiguous before '='. Initializers are
    // expressions, except that arrow-function signatures contain their own TS
    // parameter/return annotations which are parsed explicitly below.
    // Retain the declaration facts needed by semantic passes before erasing
    // their source spelling. This first structural slice deliberately records
    // only simple identifier bindings; destructuring remains unsupported by the
    // checker rather than being guessed here.
    std::size_t declaration_begin = first;
    int declaration_paren = 0, declaration_square = 0, declaration_brace = 0;
    for (std::size_t p = first; p <= end; ++p) {
        const bool boundary = p == end || (!declaration_paren && !declaration_square &&
                                           !declaration_brace && tokens_[p].text == ",");
        if (boundary) {
            std::size_t name = declaration_begin;
            while (name < p && tokens_[name].kind == TokenKind::Comment) ++name;
            if (name < p && tokens_[name].kind == TokenKind::Identifier) {
                std::size_t colon = p, equals = p;
                int par = 0, sq = 0, br = 0;
                for (std::size_t q = name + 1; q < p; ++q) {
                    const auto& text = tokens_[q].text;
                    if (text == "(") ++par; else if (text == ")" && par) --par;
                    else if (text == "[") ++sq; else if (text == "]" && sq) --sq;
                    else if (text == "{") ++br; else if (text == "}" && br) --br;
                    if (!par && !sq && !br && text == ":" && colon == p) colon = q;
                    if (!par && !sq && !br && text == "=") { equals = q; break; }
                }
                VariableDeclaration declaration;
                declaration.name_token = name;
                if (colon < p) {
                    declaration.type_begin_token = colon + 1;
                    declaration.type_end_token = equals < p ? equals : p;
                }
                if (equals < p) {
                    declaration.initializer_begin_token = equals + 1;
                    declaration.initializer_end_token = p;
                }
                program_->variables.push_back(declaration);
            }
            declaration_begin = p + 1;
        }
        if (p == end) break;
        const auto& text = tokens_[p].text;
        if (text == "(") ++declaration_paren; else if (text == ")" && declaration_paren) --declaration_paren;
        else if (text == "[") ++declaration_square; else if (text == "]" && declaration_square) --declaration_square;
        else if (text == "{") ++declaration_brace; else if (text == "}" && declaration_brace) --declaration_brace;
    }

    bool in_initializer = false;
    int paren = 0, square = 0, brace = 0, angle = 0;
    for (std::size_t p = first; p < end; ++p) {
        const auto& x = tokens_[p].text;
        if (x == "(") ++paren; else if (x == ")" && paren) --paren;
        else if (x == "[") ++square; else if (x == "]" && square) --square;
        else if (x == "{") ++brace; else if (x == "}" && brace) --brace;
        else if (x == "<") ++angle;
        else if (x == ">" && angle) --angle;
        else if ((x == ">>" || x == ">>>") && angle) angle = std::max(0, angle - static_cast<int>(x.size()));

        if (!paren && !square && !brace && !angle) {
            if (x == "=") { in_initializer = true; continue; }
            if (x == ",") { in_initializer = false; continue; }
            if (!in_initializer && x == ":" && p > first)
                erase_type_annotation(p, end, {"=", ",", ";"});
        }
    }

    // Locate arrow signatures within the initializer. We intentionally parse
    // only the signature boundary here; arrow bodies remain ordinary JS.
    for (std::size_t arrow = first; arrow < end; ++arrow) {
        if (tokens_[arrow].text != "=>") continue;

        // Parenthesized parameters: find the matching '(' immediately owning
        // the ')' before an optional return annotation.
        std::size_t close = arrow;
        if (arrow > first && tokens_[arrow - 1].text == ")") close = arrow - 1;
        else {
            for (std::size_t q = arrow; q-- > first;) {
                if (tokens_[q].text == ":") continue;
                if (tokens_[q].text == ")") { close = q; break; }
                if (tokens_[q].text == "=" || tokens_[q].text == ";") break;
            }
        }

        if (close < arrow && tokens_[close].text == ")") {
            int depth = 0;
            std::size_t open = close;
            for (std::size_t q = close + 1; q-- > first;) {
                if (tokens_[q].text == ")") ++depth;
                else if (tokens_[q].text == "(" && --depth == 0) { open = q; break; }
                if (q == 0) break;
            }
            if (tokens_[open].text == "(") {
                // If the initializer begins with '<' but the parameter list is
                // not immediately preceded by a matching '>', this is a broken
                // generic-arrow delimiter rather than a comparison expression.
                std::size_t init = first;
                while (init < open && tokens_[init].text != "=") ++init;
                if (init + 1 < open && tokens_[init + 1].text == "<" &&
                    !(tokens_[open - 1].text == ">" || tokens_[open - 1].text == ">>" || tokens_[open - 1].text == ">>>"))
                    error_at(init + 1, "unterminated generic arrow type parameter list");
                if (open > first && (tokens_[open - 1].text == ">" ||
                                     tokens_[open - 1].text == ">>" ||
                                     tokens_[open - 1].text == ">>>")) {
                    int depth = 0;
                    for (std::size_t q = open; q-- > first;) {
                        const auto& gx = tokens_[q].text;
                        if (gx == ">") ++depth;
                        else if (gx == ">>" || gx == ">>>") depth += static_cast<int>(gx.size());
                        else if (gx == "<" && --depth == 0) {
                            if (validate_generic_list(q, open - 1)) erase_generic_list(q, open - 1);
                            break;
                        }
                        if (q == 0) break;
                    }
                }
                parse_parameter_list(open, close);
            }
            if (close + 1 < arrow && tokens_[close + 1].text == ":")
                erase_type_annotation(close + 1, arrow, {"=>"});
            if (arrow + 1 < end && tokens_[arrow + 1].text == "{") {
                const auto body_close = find_matching(arrow + 1, "{", "}");
                if (body_close < end) parse_runtime_range(arrow + 2, body_close);
            }
            continue;
        }

        // TypeScript requires a typed arrow parameter to be parenthesized.
        // An unparenthesized `key: value => value` is commonly an object-literal
        // property whose value is an untyped arrow; do not reinterpret its colon.
    }

    // Function expressions inside variable initializers carry the same
    // erasable parameter/result syntax as declarations.
    for(std::size_t p=first;p<end;++p)if(tokens_[p].text=="function"){
        std::size_t open=p+1;while(open<end&&tokens_[open].text!="(")++open;
        if(open>=end)continue;
        const auto close=find_matching(open,"(",")");
        if(close>=end)continue;
        parse_parameter_list(open,close);std::size_t body=close+1;
        if(body<end&&tokens_[body].text==":"){erase_type_annotation(body,end,{"{"});while(body<end&&tokens_[body].text!="{")++body;}
        if(body<end&&tokens_[body].text=="{"){const auto body_close=find_matching(body,"{","}");if(body_close<end){parse_runtime_range(body+1,body_close);p=body_close;}}
    }

    // Object-literal methods/accessors have TypeScript signatures too.
    for (std::size_t p = first; p < end; ++p) {
        if (tokens_[p].text == "{") {
            const auto c = find_matching(p, "{", "}");
            if (c < end) { parse_object_literal_range(p, c); p = c; }
        }
    }
}


void Parser::parse_runtime_range(std::size_t first, std::size_t end) {
    erase_expression_type_arguments(first, end);
    for (std::size_t p = first; p < end; ++p) {
        if (tokens_[p].text == "const" || tokens_[p].text == "let" || tokens_[p].text == "var") {
            int paren = 0, square = 0, brace = 0;
            std::size_t stop = p + 1;
            for (; stop < end; ++stop) {
                const auto& x = tokens_[stop].text;
                if (x == "(") ++paren; else if (x == ")" && paren) --paren;
                else if (x == "[") ++square; else if (x == "]" && square) --square;
                else if (x == "{") ++brace; else if (x == "}" && brace) --brace;
                if (!paren && !square && !brace && x == ";") break;
            }
            parse_variable_declarators(p + 1, stop);
            // Initializers can themselves contain TS-bearing runtime syntax,
            // e.g. object generic methods. Continue walking this declaration
            // range instead of skipping straight to its semicolon.
            for(std::size_t q=p+1;q<stop;++q) {
                if(tokens_[q].text=="{") {
                    const auto close=find_matching(q,"{","}");
                    if(close<stop) { parse_runtime_range(q+1,close); q=close; }
                }
            }
            p = stop < end ? stop : end - 1;
            continue;
        }

        // Object/class method with TypeScript type parameters:
        // `{ f<T>(x:T):T { ... } }`. This is runtime syntax, unlike a generic
        // call expression, so erase both the method type parameters and its
        // parameter/return annotations.
        if (tokens_[p].kind==TokenKind::Identifier && p+2<end &&
            tokens_[p+1].text=="<") {
            const auto generic_close=find_matching(p+1,"<",">");
            if(generic_close<end && generic_close+1<end && tokens_[generic_close+1].text=="(") {
                const auto close=find_matching(generic_close+1,"(",")");
                if(close<end) {
                    erase_generic_list(p+1,generic_close);
                    parse_parameter_list(generic_close+1,close);
                    std::size_t body=close+1;
                    if(body<end&&tokens_[body].text==":") {
                        erase_type_annotation(body,end,{"{"});
                        while(body<end&&tokens_[body].text!="{")++body;
                    }
                    if(body<end&&tokens_[body].text=="{") {
                        const auto body_close=find_matching(body,"{","}");
                        if(body_close<end) {
                            parse_runtime_range(body+1,body_close);
                            p=body_close;
                            continue;
                        }
                    }
                }
            }
        }

        if (tokens_[p].text == "function") {
            std::size_t open = p + 1;
            while (open < end && tokens_[open].text != "(") ++open;
            if (open < end) {
                const auto close = find_matching(open, "(", ")");
                if (close < end) {
                    parse_parameter_list(open, close);
                    std::size_t body = close + 1;
                    if (body < end && tokens_[body].text == ":") {
                        erase_type_annotation(body, end, {"{"});
                        while (body < end && tokens_[body].text != "{") ++body;
                    }
                    if (body < end && tokens_[body].text == "{") {
                        const auto body_close = find_matching(body, "{", "}");
                        if (body_close < end) {
                            parse_runtime_range(body + 1, body_close);
                            p = body_close;
                        }
                    }
                }
            }
            continue;
        }

        if (tokens_[p].text == "{") {
            const auto close = find_matching(p, "{", "}");
            if (close < end) {
                parse_runtime_range(p + 1, close);
                p = close;
            }
        }
    }
}


SyntaxNode Parser::parse_interface() {
    const std::size_t begin = i_++;
    std::size_t open = i_;
    while (open < tokens_.size() && tokens_[open].text != "{") ++open;
    if (open >= tokens_.size()) {
        error_at(begin, "expected '{' after interface declaration");
        return {SyntaxKind::InterfaceDeclaration, begin, begin, {}};
    }
    const std::size_t close = find_matching(open, "{", "}");
    if (close >= tokens_.size()) {
        error_at(open, "unterminated interface declaration");
        i_ = tokens_.size() - 1;
        return {SyntaxKind::InterfaceDeclaration, begin, open, {}};
    }
    for (std::size_t p = open + 1; p < close; ++p) {
        if (tokens_[p].text == "::") { error_at(p, "invalid interface member type separator '::'"); break; }
        if (tokens_[p].text == ":" && p + 1 <= close &&
            (p + 1 == close || tokens_[p+1].text == ";" || tokens_[p+1].text == ",")) {
            error_at(p, "expected type after ':' in interface member");
            break;
        }
    }
    erase_tokens(begin, close + 1);
    i_ = close + 1;
    if (i_ < tokens_.size() && tokens_[i_].text == ";") { erase_tokens(i_, i_ + 1); ++i_; }
    return {SyntaxKind::InterfaceDeclaration, begin, close, {}};
}

SyntaxNode Parser::parse_type_alias() {
    const std::size_t begin = i_++;
    const std::size_t end = find_statement_end(i_);
    std::size_t eq = i_;
    while (eq < end && tokens_[eq].text != "=") ++eq;
    // `T=>` lexes as `T`, `=>`; in a type-parameter list this is a missing
    // default type followed by the generic close and alias assignment.
    for (std::size_t p=begin; p<eq && p<end; ++p)
        if (tokens_[p].text=="=>") {
            error_at(p,"generic type parameter default requires a type");
            break;
        }
    if (eq >= end || eq + 1 >= end || tokens_[eq + 1].text == ";")
        error_at(eq < tokens_.size() ? eq : begin, "type alias requires a type expression");
    else {
        // A defaulted generic parameter must have a type between `=` and its
        // closing `>`; accepting `T=>` makes the following alias `=` ambiguous.
        for (std::size_t p=begin; p<eq; ++p)
            if (tokens_[p].text=="=" && p+1<eq && tokens_[p+1].text==">")
                error_at(p,"generic type parameter default requires a type");
        int angle = 0, square = 0, paren = 0, brace = 0;
        for (std::size_t p = eq + 1; p < end; ++p) {
            const auto& x = tokens_[p].text;
            if (x == "||") error_at(p, "logical '||' is not valid in a type expression");
            // Mapped property/value separators and conditional branches must
            // have a type expression on both sides.
            if (x == ":" && (p + 1 >= end || tokens_[p+1].text == "}" || tokens_[p+1].text == "]" ||
                              tokens_[p+1].text == ";" || tokens_[p+1].text == ","))
                error_at(p, "expected type after ':' in type alias");
            if ((x=="keyof"||x=="typeof"||x=="readonly"||x=="unique") &&
                (p+1>=end || tokens_[p+1].text==";" || tokens_[p+1].text=="," ||
                 tokens_[p+1].text=="]" || tokens_[p+1].text=="}"))
                error_at(p, "type operator requires an operand");
            if (x=="import" && p+2<end && tokens_[p+1].text=="(" && tokens_[p+2].text==")")
                error_at(p,"import type query requires a module specifier");
            if (tokens_[p].kind==TokenKind::Template && !tokens_[p].text.empty()) {
                const std::string& raw=tokens_[p].text;
                int interpolation=0;
                for(std::size_t k=0;k+1<raw.size();++k) {
                    if(raw[k]=='$'&&raw[k+1]=='{'){++interpolation;++k;continue;}
                    if(raw[k]=='}'&&interpolation)--interpolation;
                }
                if(interpolation) error_at(p,"unterminated template literal type interpolation");
            }
            if (x == "<") ++angle; else if (x == ">" && angle) --angle;
            else if ((x == ">>" || x == ">>>") && angle) angle = std::max(0, angle-static_cast<int>(x.size()));
            else if (x == "[") ++square; else if (x == "]" && square) --square;
            else if (x == "(") ++paren; else if (x == ")" && paren) --paren;
            else if (x == "{") ++brace; else if (x == "}" && brace) --brace;
        }
        if (angle || square || paren || brace) error_at(eq + 1, "unbalanced delimiters in type alias");
    }
    erase_tokens(begin, std::min(end + 1, tokens_.size()));
    i_ = std::min(end + 1, tokens_.size() - 1);
    return {SyntaxKind::TypeAliasDeclaration, begin, end, {}};
}

SyntaxNode Parser::parse_function() {
    const std::size_t begin = i_++;
    while (i_ < tokens_.size() && tokens_[i_].text != "(" && tokens_[i_].kind != TokenKind::End) ++i_;
    if (i_ >= tokens_.size() || tokens_[i_].text != "(") {
        error_at(begin, "expected parameter list after function declaration");
        return {SyntaxKind::FunctionDeclaration, begin, begin, {}};
    }
    const std::size_t open = i_;
    // `function name<T, U extends X>(...)`
    for (std::size_t p = begin + 1; p < open; ++p) {
        if (tokens_[p].text == "<") {
            const auto generic_close = find_matching(p, "<", ">");
            if (generic_close >= open) error_at(p, "unterminated generic type parameter list");
            else if (validate_generic_list(p, generic_close)) erase_generic_list(p, generic_close);
            break;
        }
    }
    const std::size_t close = find_matching(open, "(", ")");
    if (close >= tokens_.size()) {
        error_at(open, "unterminated function parameter list");
        i_ = tokens_.size() - 1;
        return {SyntaxKind::FunctionDeclaration, begin, open, {}};
    }
    parse_parameter_list(open, close);
    i_ = close + 1;
    if (i_ < tokens_.size() && tokens_[i_].text == ":") {
        erase_type_annotation(i_, tokens_.size(), {"{", "=>", ";"});
        while (i_ < tokens_.size() && tokens_[i_].text != "{" && tokens_[i_].text != ";" &&
               tokens_[i_].kind != TokenKind::End) ++i_;
    }
    if (i_ < tokens_.size() && tokens_[i_].text == "{") {
        const std::size_t body_open = i_;
        const std::size_t body_close = find_matching(body_open, "{", "}");
        if (body_close < tokens_.size()) {
            parse_runtime_range(body_open + 1, body_close);
            i_ = body_close + 1;
            return {SyntaxKind::FunctionDeclaration, begin, body_close, {}};
        }
    }
    const std::size_t end = find_statement_end(i_);
    // A function declaration with a signature and semicolon but no body is an
    // overload/ambient signature and has no JavaScript representation.
    if (end < tokens_.size() && tokens_[end].text == ";")
        erase_tokens(begin, end + 1);
    i_ = std::min(end + 1, tokens_.size() - 1);
    return {SyntaxKind::FunctionDeclaration, begin, end, {}};
}

void Parser::parse_class_body(std::size_t open, std::size_t close, bool derived) {
    std::size_t p = open + 1;
    int brace = 0;
    while (p < close) {
        if (tokens_[p].text == "{") {
            if (brace == 0) {
                const auto body_close = find_matching(p, "{", "}");
                if (body_close < close) {
                    parse_runtime_range(p + 1, body_close);
                    p = body_close + 1;
                    continue;
                }
            }
            ++brace; ++p; continue;
        }
        if (tokens_[p].text == "}" && brace) { --brace; ++p; continue; }
        if (brace) { ++p; continue; }

        if (tokens_[p].text == "constructor" && p + 1 < close && tokens_[p + 1].text == "(") {
            const std::size_t param_open = p + 1;
            const std::size_t param_close = find_matching(param_open, "(", ")");
            if (param_close >= close) {
                error_at(param_open, "unterminated constructor parameter list");
                return;
            }
            std::vector<std::string> parameter_properties;
            for (std::size_t q = param_open + 1; q < param_close; ++q) {
                const bool modifier = tokens_[q].text == "public" || tokens_[q].text == "private" ||
                                      tokens_[q].text == "protected" || tokens_[q].text == "readonly";
                if (!modifier) continue;
                std::size_t n = q;
                while (n < param_close &&
                       (tokens_[n].text == "public" || tokens_[n].text == "private" ||
                        tokens_[n].text == "protected" || tokens_[n].text == "readonly"))
                    ++n;
                if (n < param_close && tokens_[n].kind == TokenKind::Identifier &&
                    std::find(parameter_properties.begin(), parameter_properties.end(),
                              tokens_[n].text) == parameter_properties.end())
                    parameter_properties.push_back(tokens_[n].text);
            }
            parse_parameter_list(param_open, param_close);

            std::size_t body_open = param_close + 1;
            if (body_open < close && tokens_[body_open].text == ";") {
                erase_tokens(p,body_open+1); p=body_open+1; continue;
            }
            while (body_open < close && tokens_[body_open].text != "{" && tokens_[body_open].text != ";") ++body_open;
            if (body_open < close && tokens_[body_open].text == ";") {
                erase_tokens(p,body_open+1); p=body_open+1; continue;
            }
            if (body_open >= close) {
                error_at(param_close, "constructor parameter properties require an implementation body");
                return;
            }
            const std::size_t body_close = find_matching(body_open, "{", "}");
            if (body_close >= close + 1) {
                error_at(body_open, "unterminated constructor body");
                return;
            }

            if (!parameter_properties.empty()) {
                std::string assignments;
                for (const auto& name : parameter_properties)
                    assignments += "this." + name + " = " + name + ";";

                std::size_t insert_pos = tokens_[body_open].end;
                if (derived) {
                    // Parameter properties in derived constructors must be
                    // initialized after `super(...)`.
                    std::size_t s = body_open + 1;
                    bool found_super = false;
                    while (s < body_close) {
                        if (tokens_[s].text == "super" && s + 1 < body_close && tokens_[s + 1].text == "(") {
                            const auto super_close = find_matching(s + 1, "(", ")");
                            if (super_close < body_close) {
                                std::size_t after = super_close + 1;
                                const bool had_semicolon = after < body_close && tokens_[after].text == ";";
                                if (had_semicolon) ++after;
                                insert_pos = after < tokens_.size() ? tokens_[after - 1].end : tokens_[super_close].end;
                                if (!had_semicolon) assignments.insert(0, ";");
                                found_super = true;
                                break;
                            }
                        }
                        ++s;
                    }
                    if (!found_super) {
                        error_at(body_open, "derived constructor parameter properties require a super(...) call");
                        return;
                    }
                }
                add_replacement(insert_pos, insert_pos, assignments);
            }
            parse_runtime_range(body_open + 1, body_close);
            p = body_close + 1;
            continue;
        }

        if (tokens_[p].text == "abstract") {
            std::size_t q = p + 1;
            int paren = 0, angle = 0;
            for (; q < close; ++q) {
                if (tokens_[q].text == "(") ++paren; else if (tokens_[q].text == ")" && paren) --paren;
                else if (tokens_[q].text == "<") ++angle; else if (tokens_[q].text == ">" && angle) --angle;
                if (!paren && !angle && tokens_[q].text == ";") { erase_tokens(p, q + 1); p = q + 1; break; }
                if (!paren && !angle && tokens_[q].text == "{") break;
            }
            if (q < close && tokens_[q].text == ";") continue;
            // Semicolon-less abstract method signatures may terminate at the
            // class closing brace. If we saw a parameter list but no body,
            // erase the declaration through the token before `}`.
            std::size_t po=p+1;
            while(po<close&&tokens_[po].text!="("&&tokens_[po].text!="{")++po;
            if(po<close&&tokens_[po].text=="(") {
                const auto pc=find_matching(po,"(",")");
                if(pc<close) {
                    std::size_t r=pc+1;
                    if(r<close&&tokens_[r].text==":") {
                        ++r; while(r<close&&tokens_[r].text!=";"&&tokens_[r].text!="{"&&tokens_[r].text!="}")++r;
                    }
                    if(r>=close || tokens_[r].text=="}") { erase_tokens(p,close); p=close; continue; }
                }
            }
        }
        if (tokens_[p].text == "public" || tokens_[p].text == "private" ||
            tokens_[p].text == "protected" || tokens_[p].text == "readonly" ||
            tokens_[p].text == "abstract" || tokens_[p].text == "declare" ||
            tokens_[p].text == "override") {
            erase_tokens(p, p + 1);
            ++p; continue;
        }

        // Method/constructor parameter list.
        if (tokens_[p].text == "(") {
            std::size_t member_start = p;
            while (member_start > open + 1 && tokens_[member_start-1].text != ";" &&
                   tokens_[member_start-1].text != "}" && tokens_[member_start-1].text != "{") --member_start;
            // Optional method marker belongs to a type-only declaration or is
            // erased from a method implementation (`f?(){}`).
            if (p >= 2 && tokens_[p-1].text == "?" && tokens_[p-2].kind == TokenKind::Identifier)
                erase_tokens(p-1,p);
            // Generic method declaration: method<T, U>(...)
            if (p > open + 1 && (tokens_[p - 1].text == ">" ||
                                  tokens_[p - 1].text == ">>" ||
                                  tokens_[p - 1].text == ">>>")) {
                int depth = 0;
                for (std::size_t q = p; q-- > open + 1;) {
                    const auto& gx = tokens_[q].text;
                    if (gx == ">") ++depth;
                    else if (gx == ">>" || gx == ">>>") depth += static_cast<int>(gx.size());
                    else if (gx == "<" && --depth == 0) { erase_generic_list(q, p - 1); break; }
                    if (q == 0) break;
                }
            }
            const auto c = find_matching(p, "(", ")");
            if (c < close) {
                parse_parameter_list(p, c);
                std::size_t after = c + 1;
                if (after < close && tokens_[after].text == ":") {
                    // Catch the common missing separator `): Type nextMember(`.
                    if (after + 2 < close && tokens_[after+1].kind == TokenKind::Identifier &&
                        tokens_[after+2].kind == TokenKind::Identifier)
                        error_at(after+2,"expected ';' or method body after return type");
                    erase_type_annotation(after, close, {"{", ";", "="});
                    while (after < close && tokens_[after].text != "{" && tokens_[after].text != ";" && tokens_[after].text != "=") ++after;
                }
                if (after < close && tokens_[after].text == ";") {
                    erase_tokens(member_start,after+1);
                    p=after+1; continue;
                }
                p = c + 1;
                continue;
            }
        }

        // Optional/definite-assignment property markers are TypeScript-only.
        if ((tokens_[p].text == "?" || tokens_[p].text == "!") &&
            p + 1 < close && (tokens_[p + 1].text == ":" ||
                              tokens_[p + 1].text == "=" ||
                              tokens_[p + 1].text == ";")) {
            if (p == open + 1 || tokens_[p - 1].kind != TokenKind::Identifier) {
                error_at(p, "class property marker requires a property name");
            } else erase_tokens(p, p + 1);
        }

        // Field/property annotation before = or ;. Object literal colons are
        // inside initializer braces and therefore not at class-body depth.
        if (tokens_[p].text == ":" && p > open + 1) {
            erase_type_annotation(p, close, {"=", ";", "{"});
        }
        ++p;
    }
}

SyntaxNode Parser::parse_class() {
    const std::size_t begin = i_++;
    std::size_t open = i_;
    std::size_t implements_index = tokens_.size();
    bool derived = false;
    while (open < tokens_.size() && tokens_[open].text != "{") {
        if (tokens_[open].text == "implements") implements_index = open;
        if (tokens_[open].text == "extends") derived = true;
        ++open;
    }
    if (open >= tokens_.size()) {
        error_at(begin, "expected '{' after class declaration");
        return {SyntaxKind::ClassDeclaration, begin, begin, {}};
    }
    // Erase class declaration type parameters and type arguments attached to
    // the runtime base expression, but preserve the actual `extends Base`.
    for (std::size_t p = begin + 1; p < open; ++p) {
        if (tokens_[p].text == "<") {
            const auto q = find_matching(p, "<", ">");
            if (q >= open) { error_at(p, "unterminated class generic type parameter list"); break; }
            if (validate_generic_list(p, q)) { erase_generic_list(p, q); p = q; }
        }
    }
    if (implements_index < open) erase_tokens(implements_index, open);
    const std::size_t close = find_matching(open, "{", "}");
    if (close >= tokens_.size()) {
        error_at(open, "unterminated class declaration");
        i_ = tokens_.size() - 1;
        return {SyntaxKind::ClassDeclaration, begin, open, {}};
    }
    parse_class_body(open, close, derived);
    i_ = close + 1;
    return {SyntaxKind::ClassDeclaration, begin, close, {}};
}

SyntaxNode Parser::parse_enum() {
    const std::size_t begin = i_++;
    if (i_ >= tokens_.size() || tokens_[i_].kind != TokenKind::Identifier) {
        error_at(begin, "expected enum name");
        return {SyntaxKind::EnumDeclaration, begin, begin, {}};
    }
    const std::string name = tokens_[i_++].text;
    if (i_ >= tokens_.size() || tokens_[i_].text != "{") {
        error_at(i_, "expected '{' after enum name");
        return {SyntaxKind::EnumDeclaration, begin, i_, {}};
    }
    const std::size_t open = i_;
    const std::size_t close = find_matching(open, "{", "}");
    if (close >= tokens_.size()) {
        error_at(open, "unterminated enum declaration"); i_ = tokens_.size() - 1;
        return {SyntaxKind::EnumDeclaration, begin, open, {}};
    }
    auto quote=[](const std::string&s){std::string q="\"";for(char c:s){if(c=='\\'||c=='\"')q.push_back('\\');q.push_back(c);}q.push_back('\"');return q;};
    std::vector<std::string> prior;
    std::unordered_map<std::string,double> numeric_values;
    std::string js="var "+name+";(function("+name+"){";
    double next_numeric=0.0; bool have_numeric=true;

    auto format_number=[](double v){
        std::ostringstream out; out<<std::setprecision(15)<<v; return out.str();
    };
    auto parse_number=[](std::string n,double& v)->bool{
        n.erase(std::remove(n.begin(),n.end(),'_'),n.end());
        try{
            if(n.size()>2&&n[0]=='0'&&(n[1]=='b'||n[1]=='B')){long long x=0;for(std::size_t k=2;k<n.size();++k){if(n[k]!='0'&&n[k]!='1')return false;x=x*2+(n[k]-'0');}v=static_cast<double>(x);return true;}
            if(n.size()>2&&n[0]=='0'&&(n[1]=='o'||n[1]=='O')){v=static_cast<double>(std::stoll(n.substr(2),nullptr,8));return true;}
            if(n.size()>2&&n[0]=='0'&&(n[1]=='x'||n[1]=='X')){v=static_cast<double>(std::stoll(n.substr(2),nullptr,16));return true;}
            v=std::stod(n);return true;
        }catch(...){return false;}
    };
    auto eval_numeric = [&](std::size_t first, std::size_t last, double& result) -> bool {
        std::size_t pos=first;bool ok=true;
        std::function<double()> parse_or,parse_xor,parse_and,parse_shift,parse_add,parse_mul,parse_unary,parse_pow,parse_primary;
        parse_primary=[&]()->double{
            if(pos>=last){ok=false;return 0;}
            if(tokens_[pos].text=="("){++pos;double v=parse_or();if(pos>=last||tokens_[pos].text!=")"){ok=false;return 0;}++pos;return v;}
            if(tokens_[pos].kind==TokenKind::Number){double v=0;if(!parse_number(tokens_[pos++].text,v))ok=false;return v;}
            if(tokens_[pos].kind==TokenKind::Identifier){auto it=numeric_values.find(tokens_[pos++].text);if(it==numeric_values.end()){ok=false;return 0;}return it->second;}
            ok=false;return 0;
        };
        parse_pow=[&]()->double{double v=parse_primary();if(ok&&pos<last&&tokens_[pos].text=="**"){++pos;double r=parse_unary();v=std::pow(v,r);}return v;};
        parse_unary=[&]()->double{if(pos<last&&(tokens_[pos].text=="+"||tokens_[pos].text=="-"||tokens_[pos].text=="~")){auto op=tokens_[pos++].text;double v=parse_unary();if(op=="-")return -v;if(op=="~")return static_cast<double>(~static_cast<long long>(v));return v;}return parse_pow();};
        parse_mul=[&]()->double{double v=parse_unary();while(ok&&pos<last&&(tokens_[pos].text=="*"||tokens_[pos].text=="/"||tokens_[pos].text=="%")){auto op=tokens_[pos++].text;double r=parse_unary();if((op=="/"||op=="%")&&r==0){ok=false;return 0;}if(op=="*")v*=r;else if(op=="/")v/=r;else v=std::fmod(v,r);}return v;};
        parse_add=[&]()->double{double v=parse_mul();while(ok&&pos<last&&(tokens_[pos].text=="+"||tokens_[pos].text=="-")){auto op=tokens_[pos++].text;double r=parse_mul();v=op=="+"?v+r:v-r;}return v;};
        parse_shift=[&]()->double{double v=parse_add();while(ok&&pos<last&&(tokens_[pos].text=="<<"||tokens_[pos].text==">>"||tokens_[pos].text==">>>")){auto op=tokens_[pos++].text;long long l=static_cast<long long>(v),r=static_cast<long long>(parse_add());if(op=="<<")l<<=r;else if(op==">>")l>>=r;else l=static_cast<unsigned long long>(l)>>r;v=static_cast<double>(l);}return v;};
        parse_and=[&]()->double{double v=parse_shift();while(ok&&pos<last&&tokens_[pos].text=="&"){++pos;v=static_cast<double>(static_cast<long long>(v)&static_cast<long long>(parse_shift()));}return v;};
        parse_xor=[&]()->double{double v=parse_and();while(ok&&pos<last&&tokens_[pos].text=="^"){++pos;v=static_cast<double>(static_cast<long long>(v)^static_cast<long long>(parse_and()));}return v;};
        parse_or=[&]()->double{double v=parse_xor();while(ok&&pos<last&&tokens_[pos].text=="|"){++pos;v=static_cast<double>(static_cast<long long>(v)|static_cast<long long>(parse_xor()));}return v;};
        result=parse_or();return ok&&pos==last&&std::isfinite(result);
    };
    std::size_t p=open+1;
    while(p<close){
        if(tokens_[p].text==","){
            if (p == open + 1 || (p > open + 1 && tokens_[p-1].text == ",")) error_at(p,"expected enum member between commas");
            ++p;continue;
        }
        if(tokens_[p].kind!=TokenKind::Identifier&&tokens_[p].kind!=TokenKind::String&&tokens_[p].kind!=TokenKind::Number){error_at(p,"unsupported enum member name");break;}
        std::string member=tokens_[p].text;if(tokens_[p].kind==TokenKind::String&&member.size()>=2)member=member.substr(1,member.size()-2);++p;
        bool is_string=false;std::string value;
        if(p<close&&tokens_[p].text=="="){
            ++p;const std::size_t eb=p;int paren=0,square=0,brace=0;
            while(p<close){const auto&x=tokens_[p].text;if(!paren&&!square&&!brace&&x==",")break;if(x=="(")++paren;else if(x==")"&&paren)--paren;else if(x=="[")++square;else if(x=="]"&&square)--square;else if(x=="{")++brace;else if(x=="}"&&brace)--brace;++p;}
            if(eb==p){error_at(p? p-1:open,"expected enum initializer");break;}
            const std::size_t count=p-eb;
            if(count==1&&tokens_[eb].kind==TokenKind::String){is_string=true;value=tokens_[eb].text;have_numeric=false;}
            else if((count==1&&tokens_[eb].kind==TokenKind::Number)||(count==2&&(tokens_[eb].text=="-"||tokens_[eb].text=="+")&&tokens_[eb+1].kind==TokenKind::Number)){
                value=source_.text.substr(tokens_[eb].begin,tokens_[p-1].end-tokens_[eb].begin);
                double v=0;have_numeric=parse_number(tokens_[count==2?eb+1:eb].text,v);if(have_numeric){if(count==2&&tokens_[eb].text=="-")v=-v;next_numeric=v+1;numeric_values[member]=v;}
            }else{
                std::size_t cursor=tokens_[eb].begin;
                for(std::size_t q=eb;q<p;++q){value+=source_.text.substr(cursor,tokens_[q].begin-cursor);bool replaced=false;if(tokens_[q].kind==TokenKind::Identifier){for(const auto&pm:prior)if(tokens_[q].text==pm){value+=name+"["+quote(pm)+"]";replaced=true;break;}}if(!replaced)value+=source_.text.substr(tokens_[q].begin,tokens_[q].end-tokens_[q].begin);cursor=tokens_[q].end;}
                double evaluated=0;
                if (eval_numeric(eb,p,evaluated)) { next_numeric=evaluated+1; have_numeric=true; numeric_values[member]=evaluated; }
                else have_numeric=false;
            }
        }else{
            if(!have_numeric){error_at(p?p-1:open,"enum member must have initializer after a non-numeric member");break;}
            const double current=next_numeric++;
            value=format_number(current);
            numeric_values[member]=current;
        }
        const std::string key=quote(member);
        if(is_string)js+=name+"["+key+"]="+value+";";else js+=name+"["+name+"["+key+"]="+value+"]="+key+";";
        // Simple numeric initializer branches set next_numeric above; retain the
        // actual member value so later computed initializers can reference it.
        if (!is_string && !numeric_values.count(member) && have_numeric)
            numeric_values[member]=next_numeric-1;
        prior.push_back(member);if(p<close&&tokens_[p].text==",")++p;
    }
    js+="})("+name+"||("+name+"={}));";
    std::size_t end_token=close;if(close+1<tokens_.size()&&tokens_[close+1].text==";")end_token=close+1;
    add_replacement(tokens_[begin].begin,tokens_[end_token].end,js);i_=end_token+1;
    return {SyntaxKind::EnumDeclaration,begin,end_token,{}};
}

SyntaxNode Parser::parse_namespace() {
    const std::size_t begin=i_++;
    if(i_>=tokens_.size()||tokens_[i_].kind!=TokenKind::Identifier){error_at(begin,"expected namespace name");return {SyntaxKind::Statement,begin,begin,{}};}

    // Support dotted namespace declarations (`namespace A.B { ... }`) by
    // lowering directly to nested object/IIFE setup rather than recursively
    // pretending each dotted component had its own source braces.
    std::vector<std::string> parts;
    parts.push_back(tokens_[i_++].text);
    while(i_+1<tokens_.size()&&tokens_[i_].text=="."&&tokens_[i_+1].kind==TokenKind::Identifier){++i_;parts.push_back(tokens_[i_++].text);}
    if(i_>=tokens_.size()||tokens_[i_].text!="{"){error_at(i_,"expected '{' after namespace name");return {SyntaxKind::Statement,begin,i_,{}};}
    const std::size_t open=i_,close=find_matching(open,"{","}");
    if(close>=tokens_.size()){error_at(open,"unterminated namespace declaration");i_=tokens_.size()-1;return {SyntaxKind::Statement,begin,open,{}};}

    const std::string root=parts.front();
    std::string target=root;
    std::string prefix="var "+root+";";
    if(parts.size()>1) prefix+=root+"="+root+"||{};";
    for(std::size_t k=1;k<parts.size();++k){prefix+=target+"."+parts[k]+"="+target+"."+parts[k]+"||{};";target+="."+parts[k];}
    prefix+="(function(__ns){";
    add_replacement(tokens_[begin].begin,tokens_[open].end,prefix);
    parse_runtime_range(open+1,close);

    for(std::size_t p=open+1;p<close;++p){
        if(tokens_[p].text=="{"){const auto bc=find_matching(p,"{","}");if(bc<close){p=bc;continue;}}
        if(tokens_[p].text!="export")continue;
        erase_tokens(p,p+1);
        if(p+2<close&&(tokens_[p+1].text=="namespace"||tokens_[p+1].text=="module")&&tokens_[p+2].kind==TokenKind::Identifier){
            const std::string member=tokens_[p+2].text;std::size_t saved=i_;i_=p+1;auto node=parse_namespace();(void)node;
            const std::size_t nested_close=i_?i_-1:p+2;add_replacement(tokens_[nested_close].end,tokens_[nested_close].end,"__ns."+member+"="+member+";");p=nested_close;i_=saved;continue;
        }
        if(p+2<close&&tokens_[p+1].text=="enum"&&tokens_[p+2].kind==TokenKind::Identifier){
            const std::string member=tokens_[p+2].text;std::size_t saved=i_;i_=p+1;auto node=parse_enum();const std::size_t nested_end=node.end_token;add_replacement(tokens_[nested_end].end,tokens_[nested_end].end,"__ns."+member+"="+member+";");p=nested_end;i_=saved;continue;
        }
        if(p+2<close&&(tokens_[p+1].text=="const"||tokens_[p+1].text=="let"||tokens_[p+1].text=="var")&&tokens_[p+2].kind==TokenKind::Identifier){
            const std::string member=tokens_[p+2].text;
            // Namespace members often omit semicolons. Find the initializer
            // boundary inside this namespace rather than scanning past `}`.
            std::size_t e=p+3;int par=0,sq=0,br=0;
            for(;e<close;++e){const auto&x=tokens_[e].text;if(x=="(")++par;else if(x==")"&&par)--par;else if(x=="[")++sq;else if(x=="]"&&sq)--sq;else if(x=="{")++br;else if(x=="}"&&br)--br;if(!par&&!sq&&!br&&x==";")break;if(!par&&!sq&&!br&&e+1<close&&tokens_[e+1].text=="export")break;}
            const std::size_t pos=(e<close&&tokens_[e].text==";")?tokens_[e].end:(e<close?tokens_[e].begin:tokens_[close].begin);
            add_replacement(pos,pos,";__ns."+member+"="+member+";");p=e<close?e-1:e;continue;
        }
        if(p+2<close&&tokens_[p+1].text=="function"&&tokens_[p+2].kind==TokenKind::Identifier){
            const std::string member=tokens_[p+2].text;std::size_t body=p+3;while(body<close&&tokens_[body].text!="{")++body;
            if(body<close){const auto bc=find_matching(body,"{","}");if(bc<close){add_replacement(tokens_[bc].end,tokens_[bc].end,"__ns."+member+"="+member+";");p=bc;}}
        }
    }
    add_replacement(tokens_[close].begin,tokens_[close].end,"})("+target+"||("+target+"={}));");i_=close+1;
    return {SyntaxKind::Statement,begin,close,{}};
}
SyntaxNode Parser::parse_variable_statement() {
    const std::size_t begin = i_++;
    const std::size_t end = find_statement_end(i_);
    erase_expression_type_arguments(i_, end);
    parse_variable_declarators(i_, end);
    i_ = std::min(end + 1, tokens_.size() - 1);
    return {SyntaxKind::VariableStatement, begin, end, {}};
}

SyntaxNode Parser::parse_statement() {
    const std::size_t begin = i_;
    const std::size_t end = find_statement_end(i_);
    // Generic statement ranges can contain nested blocks followed by further
    // runtime declarations (for example `{...} const f=(x:T)=>x;` or
    // `try{}catch{} const f=(x:T)=>x;`). Walk the complete range so TypeScript
    // syntax inside those nested/sequential runtime forms is still erased.
    parse_runtime_range(begin, end < tokens_.size() ? end : tokens_.size());
    i_ = std::min(end + 1, tokens_.size() - 1);
    return {SyntaxKind::Statement, begin, end, {}};
}

SyntaxNode Parser::parse_top_level() {
    // `export declare ...` is still ambient; erase the export wrapper first so
    // the ordinary ambient-declaration path owns the entire declaration.
    if (is("export") && is("declare",1)) {
        erase_tokens(i_,i_+1);
        ++i_;
    }

    // Ambient declarations have no runtime representation.
    if (is("declare")) {
        const std::size_t begin = i_;
        if (is("global",1) && is("{",2)) {
            const std::size_t open=i_+2;
            const auto close=find_matching(open,"{","}");
            if(close>=tokens_.size()){
                error_at(open,"unterminated declare global block");
                i_=tokens_.size()-1;
                return {SyntaxKind::Statement,begin,open,{}};
            }
            std::size_t end=close+1;
            if(end<tokens_.size()&&tokens_[end].text==";")++end;
            erase_tokens(begin,end);
            i_=end;
            return {SyntaxKind::Statement,begin,end-1,{}};
        }
        if ((is("namespace",1) || is("module",1))) {
            std::size_t open=i_+2;while(open<tokens_.size()&&tokens_[open].text!="{"&&tokens_[open].kind!=TokenKind::End)++open;
            if(open>=tokens_.size()||tokens_[open].text!="{"){error_at(begin,"expected ambient namespace body");++i_;return {SyntaxKind::Statement,begin,begin,{}};}
            const auto close=find_matching(open,"{","}");if(close>=tokens_.size()){error_at(open,"unterminated ambient namespace");i_=tokens_.size()-1;return {SyntaxKind::Statement,begin,open,{}};}
            std::size_t end=close+1;if(end<tokens_.size()&&tokens_[end].text==";")++end;erase_tokens(begin,end);i_=end;return {SyntaxKind::Statement,begin,end-1,{}};
        }
        if (is("enum", 1)) {
            std::size_t open=i_+2;
            while(open<tokens_.size()&&tokens_[open].text!="{"&&tokens_[open].kind!=TokenKind::End)++open;
            if(open>=tokens_.size()||tokens_[open].text!="{"){error_at(begin,"expected ambient enum body");++i_;return {SyntaxKind::Statement,begin,begin,{}};}
            const auto close=find_matching(open,"{","}");
            if(close>=tokens_.size()){error_at(open,"unterminated ambient enum");i_=tokens_.size()-1;return {SyntaxKind::Statement,begin,open,{}};}
            std::size_t end=close+1;if(end<tokens_.size()&&tokens_[end].text==";")++end;
            erase_tokens(begin,end);i_=end;return {SyntaxKind::Statement,begin,end-1,{}};
        }
        if (is("interface", 1)) {
            std::size_t open=i_+2;
            while(open<tokens_.size()&&tokens_[open].text!="{"&&tokens_[open].kind!=TokenKind::End)++open;
            if(open>=tokens_.size()||tokens_[open].text!="{"){
                error_at(begin,"expected interface body after 'declare interface'");
                ++i_; return {SyntaxKind::Statement,begin,begin,{}};
            }
            const auto close=find_matching(open,"{","}");
            if(close>=tokens_.size()){
                error_at(open,"unterminated ambient interface");
                i_=tokens_.size()-1;
                return {SyntaxKind::Statement,begin,open,{}};
            }
            std::size_t end=close+1;
            if(end<tokens_.size()&&tokens_[end].text==";")++end;
            erase_tokens(begin,end);i_=end;
            return {SyntaxKind::Statement,begin,end-1,{}};
        }
        if (is("class", 1)) {
            std::size_t open = i_ + 2;
            while (open < tokens_.size() && tokens_[open].text != "{" && tokens_[open].kind != TokenKind::End) ++open;
            if (open >= tokens_.size() || tokens_[open].text != "{") {
                error_at(begin, "expected class body after 'declare class'");
                ++i_; return {SyntaxKind::Statement, begin, begin, {}};
            }
            const auto close = find_matching(open, "{", "}");
            if (close >= tokens_.size()) { error_at(open, "unterminated ambient class"); i_ = tokens_.size()-1; return {SyntaxKind::Statement,begin,open,{}}; }
            std::size_t end = close + 1;
            if (end < tokens_.size() && tokens_[end].text == ";") ++end;
            erase_tokens(begin, end); i_ = end;
            return {SyntaxKind::Statement, begin, end ? end-1 : begin, {}};
        }
        const std::size_t end = find_statement_end(i_);
        erase_tokens(begin, std::min(end + 1, tokens_.size()));
        i_ = std::min(end + 1, tokens_.size() - 1);
        return {SyntaxKind::Statement, begin, end, {}};
    }
    // Namespace re-export grammar: `export * as name from "module"`.
    // Validate it here even though the runtime transform lives in Transpiler.
    if (is("export") && is("*",1) && is("as",2)) {
        if (i_+3>=tokens_.size() || tokens_[i_+3].kind!=TokenKind::Identifier) {
            error_at(i_+2,"namespace re-export requires a name after 'as'");
        } else if (i_+4>=tokens_.size() || tokens_[i_+4].text!="from" ||
                   i_+5>=tokens_.size() || tokens_[i_+5].kind!=TokenKind::String) {
            error_at(i_+3,"namespace re-export requires 'from' and a module specifier");
        }
    }

    // Validate common static import/export grammar before the generic
    // statement parser can accidentally preserve malformed module syntax.
    if(is("export") && is("default",1)) {
        const std::size_t end=find_statement_end(i_);
        if(i_+2>=tokens_.size() || tokens_[i_+2].text==";" || tokens_[i_+2].kind==TokenKind::End)
            error_at(i_+1,"export default requires a declaration or expression");
        (void)end;
    }
    if(is("export") && is("{",1)) {
        const auto close=find_matching(i_+1,"{","}");
        if(close<tokens_.size() && close+1<tokens_.size() && tokens_[close+1].text=="from") {
            if(close+2>=tokens_.size() || tokens_[close+2].kind!=TokenKind::String)
                error_at(close+1,"re-export requires a module specifier after 'from'");
        }
    }
    if(is("import") && is("*",1) && is("as",2)) {
        if(i_+3>=tokens_.size() || tokens_[i_+3].kind!=TokenKind::Identifier || tokens_[i_+3].text=="from")
            error_at(i_+2,"namespace import requires a local name after 'as'");
    }
    if(is("import")) {
        const std::size_t end=find_statement_end(i_);
        std::size_t open=i_;
        while(open<end&&tokens_[open].text!="{")++open;
        if(open<end){
            const auto close=find_matching(open,"{","}");
            if(close<end){
                for(std::size_t q=open+1;q+1<close;++q)
                    if(tokens_[q].text=="as" && tokens_[q+1].text=="as")
                        error_at(q+1,"expected imported binding name after 'as'");
            }
        }
    }

    // `export as namespace Name;` is declaration-only metadata for UMD
    // consumers and has no JavaScript runtime representation.
    if (is("export") && is("as",1) && is("namespace",2)) {
        const std::size_t begin=i_;
        const std::size_t end=find_statement_end(i_);
        erase_tokens(begin,std::min(end+1,tokens_.size()));
        i_=std::min(end+1,tokens_.size()-1);
        return {SyntaxKind::Statement,begin,end,{}};
    }

    // `const enum` is runtime-bearing in tscc's transform. We preserve behavior
    // using the normal enum object transform rather than tsc's inlining strategy.
    if (is("const") && is("enum", 1)) {
        erase_tokens(i_, i_ + 1);
        ++i_;
        return parse_enum();
    }
    if (is("import") && i_+2<tokens_.size() &&
        tokens_[i_+1].kind==TokenKind::Identifier && tokens_[i_+2].text=="=") {
        if (i_+3>=tokens_.size() || tokens_[i_+3].text==";" ||
            tokens_[i_+3].kind==TokenKind::End)
            error_at(i_+2,"import-equals declaration requires a module reference");
    }

    // Reject malformed static module clauses before the generic parser can
    // accidentally preserve them as JavaScript-looking text.
    if(is("import")) {
        const std::size_t end=find_statement_end(i_);
        if(is("*",1) && (!is("as",2) || i_+3>=end ||
                         tokens_[i_+3].kind!=TokenKind::Identifier))
            error_at(i_,"namespace import requires '* as name'");
        if(is("*",1) && is("as",2) && i_+3<end &&
           tokens_[i_+3].kind==TokenKind::Identifier &&
           (i_+4>=end || tokens_[i_+4].text!="from" ||
            i_+5>=end || tokens_[i_+5].kind!=TokenKind::String))
            error_at(i_,"namespace import requires 'from' and a module specifier");
        if(i_+3<end && tokens_[i_+1].kind==TokenKind::Identifier &&
           tokens_[i_+2].text=="," && tokens_[i_+3].text=="from")
            error_at(i_+2,"default import comma requires a following import clause");
        std::size_t open=i_;
        while(open<end&&tokens_[open].text!="{")++open;
        if(open<end) {
            const auto close=find_matching(open,"{","}");
            if(close<end) {
                for(std::size_t q=open+1;q<close;++q)
                    if(tokens_[q].text=="as" &&
                       (q+1>=close || tokens_[q+1].text=="as" ||
                        tokens_[q+1].text=="," || tokens_[q+1].text=="}"))
                        error_at(q,"import alias requires a local name");
            }
        }
    }
    if(is("export")) {
        const std::size_t end=find_statement_end(i_);
        if(is("default",1) && (i_+2>=end || tokens_[i_+2].text==","))
            error_at(i_+1,"default export requires an expression or declaration");
        if(is("*",1) && is("as",2) && i_+3<end &&
           tokens_[i_+3].kind==TokenKind::Identifier &&
           (i_+4>=end || tokens_[i_+4].text!="from" ||
            i_+5>=end || tokens_[i_+5].kind!=TokenKind::String))
            error_at(i_,"namespace export requires 'from' and a module specifier");
        std::size_t open=i_;
        while(open<end&&tokens_[open].text!="{")++open;
        if(open<end) {
            const auto close=find_matching(open,"{","}");
            if(close<end) {
                for(std::size_t q=open+1;q<close;++q)
                    if(tokens_[q].text=="as" &&
                       (q+1>=close || tokens_[q+1].text=="as" ||
                        tokens_[q+1].text=="," || tokens_[q+1].text=="}"))
                        error_at(q,"export alias requires an exported name");
            }
        }
    }

    if (is("import") || is("export")) {
        const std::size_t end=find_statement_end(i_);
        for(std::size_t q=i_;q<end;++q) {
            if(tokens_[q].text=="with") {
                if(q+1>=end || tokens_[q+1].text!="{")
                    error_at(q,"import attributes require an object after 'with'");
                else {
                    const auto close=find_matching(q+1,"{","}");
                    if(close>=end) error_at(q+1,"unterminated import attributes object");
                }
                break;
            }
        }
    }

    // Mixed named imports/exports may mark individual specifiers as type-only:
    // `import {type T, value} from ...` / `export {type T, value}`.
    // Remove only those specifiers; if every named specifier is type-only,
    // erase the whole statement so it does not trigger a runtime module load.
    if ((is("import") || is("export")) && !is("type",1)) {
        const std::size_t begin=i_;
        const std::size_t end=find_statement_end(i_);
        std::size_t open=end;
        // Named specifier braces belong to the module clause itself. Do not
        // mistake an import-attributes object (`with {type: ...}`) for
        // `{type T, value}` named specifiers.
        if (begin + 1 < end && tokens_[begin+1].text=="{") open=begin+1;
        else if (tokens_[begin].text=="import" && begin+3<end &&
                 tokens_[begin+1].kind==TokenKind::Identifier &&
                 tokens_[begin+2].text=="," && tokens_[begin+3].text=="{")
            open=begin+3;
        if(open<end&&tokens_[open].text=="{") {
            const auto close=find_matching(open,"{","}");
            if(close<end) {
                bool saw_runtime=false,saw_type=false;
                std::vector<std::pair<std::size_t,std::size_t>> remove;
                std::size_t item=open+1;
                while(item<close) {
                    while(item<close&&(tokens_[item].text==","||tokens_[item].kind==TokenKind::Comment))++item;
                    if(item>=close)break;
                    std::size_t item_end=item;
                    while(item_end<close&&tokens_[item_end].text!=",")++item_end;
                    if(tokens_[item].text=="type") {
                        saw_type=true;
                        std::size_t a=item,b=item_end;
                        if(item_end<close) ++b; // remove following comma
                        else if(item>open+1) { a=item-1; if(tokens_[a].text!=",") a=item; }
                        remove.push_back({a,b});
                    } else saw_runtime=true;
                    item=item_end+1;
                }
                if(saw_type&&!saw_runtime) {
                    erase_tokens(begin,std::min(end+1,tokens_.size()));
                    i_=std::min(end+1,tokens_.size()-1);
                    return {SyntaxKind::Statement,begin,end,{}};
                }
                for(auto [a,b]:remove) erase_tokens(a,b);
            }
        }
    }
    // TypeScript import-equals with external require has a direct runtime
    // representation even when module syntax is otherwise preserved:
    // `import fs = require("fs")` -> `const fs = require("fs")`.
    if (is("import") && i_+5<tokens_.size() &&
        tokens_[i_+1].kind==TokenKind::Identifier &&
        tokens_[i_+2].text=="=" && tokens_[i_+3].text=="require" &&
        tokens_[i_+4].text=="(") {
        const std::size_t begin=i_;
        const std::size_t end=find_statement_end(i_);
        const std::string name=tokens_[i_+1].text;
        const std::size_t rhs_begin=tokens_[i_+3].begin;
        const std::size_t rhs_end=tokens_[end].text==";"?tokens_[end].begin:tokens_[end].end;
        const std::string rhs=source_.text.substr(rhs_begin,rhs_end-rhs_begin);
        add_replacement(tokens_[begin].begin,tokens_[end].end,
                        "const "+name+" = "+rhs+";");
        i_=std::min(end+1,tokens_.size()-1);
        return {SyntaxKind::Statement,begin,end,{}};
    }

    // `export = value` is TypeScript's CommonJS export assignment.
    if (is("export") && is("=",1)) {
        const std::size_t begin=i_;
        const std::size_t end=find_statement_end(i_);
        if(i_+2>=end){error_at(i_+1,"export assignment requires an expression");}
        else {
            const std::size_t expr_begin=tokens_[i_+2].begin;
            const std::size_t expr_end=tokens_[end].text==";"?tokens_[end].begin:tokens_[end].end;
            add_replacement(tokens_[begin].begin,tokens_[end].end,
                            "module.exports = "+source_.text.substr(expr_begin,expr_end-expr_begin)+";");
        }
        i_=std::min(end+1,tokens_.size()-1);
        return {SyntaxKind::Statement,begin,end,{}};
    }

    // Type-only imports/exports have no JavaScript runtime representation.
    if (is("import") && is("type",1)) {
        const std::size_t begin=i_;
        const std::size_t end=find_statement_end(i_);
        erase_tokens(begin,std::min(end+1,tokens_.size()));
        i_=std::min(end+1,tokens_.size()-1);
        return {SyntaxKind::Statement,begin,end,{}};
    }
    if (is("export") && is("type",1) && (is("{",2) || is("*",2))) {
        const std::size_t begin=i_;
        const std::size_t end=find_statement_end(i_);
        erase_tokens(begin,std::min(end+1,tokens_.size()));
        i_=std::min(end+1,tokens_.size()-1);
        return {SyntaxKind::Statement,begin,end,{}};
    }
    if (is("export") && (is("interface",1) || is("type",1))) {
        erase_tokens(i_,i_+1);
    }
    // Preserve runtime export/default/async wrappers; declarations inside still parse.
    while (is("export") || is("default") || is("async")) ++i_;
    if (is("abstract") && is("class", 1)) { erase_tokens(i_, i_ + 1); ++i_; }
    if (is("interface")) return parse_interface();
    if (is("type")) return parse_type_alias();
    if (is("function")) return parse_function();
    if (is("class")) return parse_class();
    if (is("enum")) return parse_enum();
    if (is("namespace") || is("module")) return parse_namespace();
    if (is("const") || is("let") || is("var")) return parse_variable_statement();
    return parse_statement();
}

bool Parser::parse(Program& program) {
    program_ = &program;
    program.root = {SyntaxKind::Program, 0, tokens_.empty() ? 0 : tokens_.size() - 1, {}};
    program.erasures.clear();
    program.replacements.clear();
    program.variables.clear();
    recovery_tokens_.clear();
    i_ = 0;
    std::size_t top_level_nodes = 0;
    while (!at_end()) {
        if (top_level_nodes++ >= limits_.max_top_level_nodes) {
            error_at(i_, "parser top-level work limit exceeded");
            break;
        }
        const std::size_t before = i_;
        const std::size_t diagnostics_before = diagnostics_.size();
        auto node = parse_top_level();
        if (diagnostics_.size() > diagnostics_before && at_end()) {
            const auto boundary = recovery_boundary(before + 1);
            if (boundary < tokens_.size()) {
                node.end_token = boundary ? boundary - 1 : boundary;
                i_ = boundary;
            }
        }
        program.root.children.push_back(std::move(node));
        if (i_ <= before) ++i_; // parser progress invariant
    }
    for(const auto token_index:recovery_tokens_){SyntaxNode recovery{SyntaxKind::Recovery,token_index,token_index,{}};recovery.recovered=true;program.root.children.push_back(std::move(recovery));}
    std::stable_sort(program.root.children.begin(),program.root.children.end(),[](const SyntaxNode&a,const SyntaxNode&b){return a.begin_token<b.begin_token;});
    SyntaxNodeId next=0;finalize_syntax(program.root,InvalidSyntaxNodeId,next);
    std::sort(program.erasures.begin(), program.erasures.end(),
              [](const EraseRange& a, const EraseRange& b) { return a.begin < b.begin; });
    return !diagnostics_.has_errors();
}

} // namespace tscc
