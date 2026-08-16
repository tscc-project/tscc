#include "Transpiler.h"
#include "Checker.h"
#include "Lexer.h"
#include "Parser.h"
#include "SourceEdit.h"
#include "Semantic.h"
#include "Binder.h"
#include <algorithm>
#include <unordered_map>
#include <unordered_set>

namespace tscc {

static void blank_range(std::string& out, std::size_t a, std::size_t b) {
    for (std::size_t i = a; i < b && i < out.size(); ++i)
        if (out[i] != '\n' && out[i] != '\r') out[i] = ' ';
}


static bool validate_tsx_structure(const SourceFile& source,
                                   const std::vector<Token>& tokens,
                                   Diagnostics& diagnostics) {
    struct OpenTag { std::string name; std::size_t token; int expression_level; };
    std::vector<OpenTag> stack;
    int expression_braces = 0;

    auto report = [&](std::size_t index, const std::string& message) {
        if (index >= tokens.size()) index = tokens.size()-1;
        const auto [line,column] = source.line_col(tokens[index].begin);
        diagnostics.error(source.path,line,column,message,source.line_text(line));
    };
    auto prefix_position = [&](std::size_t i) {
        if (i==0) return true;
        const auto& x=tokens[i-1].text;
        return x=="="||x=="("||x=="["||x==","||x==":"||x=="?"||
               x=="??"||x=="&&"||x=="||"||x=="!"||
               x=="return"||x=="=>"||x=="{"||x==";";
    };
    auto generic_arrow_after = [&](std::size_t close) {
        if (close+1>=tokens.size() || tokens[close+1].text!="(") return false;
        int d=0; std::size_t p=close+1;
        for(;p<tokens.size();++p){
            if(tokens[p].text=="(")++d;
            else if(tokens[p].text==")"&&--d==0)break;
        }
        if(p>=tokens.size()) return false;
        ++p;
        if(p<tokens.size()&&tokens[p].text==":"){
            ++p;
            while(p<tokens.size()&&tokens[p].text!="=>"&&tokens[p].text!=";"&&tokens[p].kind!=TokenKind::End)++p;
        }
        return p<tokens.size()&&tokens[p].text=="=>";
    };
    auto unambiguous_tsx_generic = [&](std::size_t open, std::size_t close) {
        // TypeScript requires a TSX generic arrow to be distinguishable from a
        // JSX element. A trailing comma or `extends` makes `<T...>` unambiguous;
        // `<T>(...) =>` itself is parsed as JSX and must be rejected.
        for(std::size_t p=open+1;p<close;++p)
            if(tokens[p].text=="," || tokens[p].text=="extends") return true;
        return false;
    };
    auto tag_end = [&](std::size_t start) {
        int angle=0, brace=0;
        for(std::size_t p=start;p<tokens.size();++p){
            const auto& x=tokens[p].text;
            if(x=="{" ){++brace;continue;}
            if(x=="}"&&brace){--brace;continue;}
            if(brace) continue;
            if(x=="<")++angle;
            else if(x==">"){
                if(--angle==0)return p;
            } else if((x==">>"||x==">>>")&&angle){
                angle-=static_cast<int>(x.size());
                if(angle<=0)return p;
            }
        }
        return tokens.size();
    };
    auto tag_name = [&](std::size_t first, std::size_t end) {
        std::string name;
        std::size_t previous_end = first < tokens.size() ? tokens[first].begin : 0;
        for(std::size_t p=first;p<end;++p){
            const auto& x=tokens[p].text;
            if(p>first && tokens[p].begin != previous_end) break; // whitespace starts attributes
            if(x=="."||x=="-"||tokens[p].kind==TokenKind::Identifier||tokens[p].kind==TokenKind::Keyword) {
                name+=x;
                previous_end=tokens[p].end;
            } else break;
        }
        return name;
    };

    for(std::size_t i=0;i+1<tokens.size();++i){
        // Inside a JSX expression, regex contents are JavaScript, not markup.
        // Skip the complete literal before interpreting `<`/`>` tokens.
        if(!stack.empty() && expression_braces>0 && tokens[i].text=="/" && prefix_position(i)){
            bool in_class=false;
            for(std::size_t q=i+1;q<tokens.size();++q){
                const auto& x=tokens[q].text;
                if(x=="["){in_class=true;continue;}
                if(x=="]"&&in_class){in_class=false;continue;}
                if(x=="/"&&!in_class){
                    bool escaped=false;
                    if(tokens[q].begin>0){
                        std::size_t b=tokens[q].begin,slashes=0;
                        while(b>0&&source.text[b-1]=='\\'){--b;++slashes;}
                        escaped=(slashes%2)==1;
                    }
                    if(!escaped){i=q;break;}
                }
            }
            continue;
        }
        if(!stack.empty() && expression_braces==0 && tokens[i].text=="{"){
            ++expression_braces;
            continue;
        }
        if(!stack.empty() && expression_braces>0){
            if(tokens[i].text=="{"){++expression_braces;continue;}
            if(tokens[i].text=="}"){--expression_braces;continue;}
            const bool closing_nested =
                tokens[i].text=="<" && i+1<tokens.size() && tokens[i+1].text=="/";
            // A closing tag is valid inside `{...}` only when it closes JSX
            // that was itself opened at the current expression depth. Closing
            // an outer tag means the JavaScript expression forgot its `}`.
            if(closing_nested){
                if(stack.empty() || stack.back().expression_level != expression_braces){
                    report(i,"expected '}' before JSX closing tag");
                    return false;
                }
                // fall through so the normal tag-closing logic pops the stack
            } else {
                // Nested JSX inside an expression begins only in
                // expression-prefix position; `a < b` remains JavaScript.
                if(tokens[i].text!="<" || !prefix_position(i)) continue;
            }
        } else if(stack.empty()){
            if(tokens[i].text!="<" || !prefix_position(i)) continue;
        } else if(tokens[i].text!="<") {
            continue;
        }

        const bool closing=i+1<tokens.size()&&tokens[i+1].text=="/";
        std::size_t first=i+1+(closing?1:0);
        if(first>=tokens.size())continue;
        const bool fragment=tokens[first].text==">";
        if(!fragment && tokens[first].kind!=TokenKind::Identifier && tokens[first].kind!=TokenKind::Keyword)
            continue;

        const std::size_t end=tag_end(i);
        if(end>=tokens.size()){
            report(i,"unterminated JSX tag");
            return false;
        }
        if(!closing && generic_arrow_after(end)) {
            if(unambiguous_tsx_generic(i,end)) {
                i=end;
                continue;
            }
            // Ambiguous `<T>(...) =>` is JSX in .tsx and therefore remains an
            // opening element; the missing closing tag will be diagnosed.
        }

        std::string name=fragment?"":tag_name(first,end);
        if(closing){
            if(stack.empty()){
                report(i,"JSX closing tag has no matching opening tag");
                return false;
            }
            if(stack.back().name!=name){
                report(i,"JSX closing tag does not match <"+stack.back().name+">");
                return false;
            }
            stack.pop_back();
        }else{
            bool self_closing=end>i&&tokens[end-1].text=="/";
            if(!self_closing)stack.push_back({name,i,expression_braces});
        }
        i=end;
    }
    if(expression_braces){
        report(stack.empty()?0:stack.back().token,"unterminated JSX expression");
        return false;
    }
    if(!stack.empty()){
        report(stack.back().token,"unterminated JSX element <"+stack.back().name+">");
        return false;
    }
    return true;
}


static std::size_t statement_end_token(const std::vector<Token>& tokens, std::size_t start) {
    int paren=0, square=0, brace=0;
    for(std::size_t p=start;p<tokens.size();++p){
        const auto& x=tokens[p].text;
        if(x=="(")++paren; else if(x==")"&&paren)--paren;
        else if(x=="[")++square; else if(x=="]"&&square)--square;
        else if(x=="{")++brace; else if(x=="}"&&brace)--brace;
        if(!paren&&!square&&!brace&&x==";") return p;
        if(tokens[p].kind==TokenKind::End) return p;
    }
    return tokens.size()-1;
}

static std::string unquote_module(const std::string& text) {
    if(text.size()>=2 && (text.front()=='"' || text.front()=='\'') && text.back()==text.front())
        return text.substr(1,text.size()-2);
    return text;
}

static std::string js_quote(const std::string& text) {
    std::string out="\"";
    for(char c:text){ if(c=='\\'||c=='"') out.push_back('\\'); out.push_back(c); }
    out.push_back('"'); return out;
}

struct ImportedBinding {
    std::string expression;
    std::size_t declaration_begin = 0;
    std::size_t declaration_end = 0;
};

static std::vector<std::string> exported_binding_names(const std::vector<Token>& tokens,
                                                       std::size_t first,
                                                       std::size_t end) {
    std::vector<std::string> names;
    auto add=[&](const std::string& n){if(std::find(names.begin(),names.end(),n)==names.end())names.push_back(n);};
    if(first>=end) return names;
    if(tokens[first].kind==TokenKind::Identifier){ add(tokens[first].text); return names; }
    if(tokens[first].text!="{" && tokens[first].text!="[") return names;
    const std::string close=tokens[first].text=="{"?"}":"]";
    int brace=0,square=0,paren=0;
    for(std::size_t p=first+1;p<end;++p){
        const auto& x=tokens[p].text;
        if(x=="{"){++brace;continue;} if(x=="}"&&brace){--brace;continue;}
        if(x=="["){++square;continue;} if(x=="]"&&square){--square;continue;}
        if(x=="("){++paren;continue;} if(x==")"&&paren){--paren;continue;}
        if(!brace&&!square&&!paren && x==close) break;
        if(tokens[p].kind!=TokenKind::Identifier) continue;
        // Object property key before ':' is not the binding; the identifier after
        // ':' is. Identifiers in default initializers are skipped until a comma.
        if(p+1<end && tokens[p+1].text==":") continue;
        if(p>first && tokens[p-1].text==".") continue;
        bool in_default=false;
        for(std::size_t q=p;q>first+1;){--q;
            if(tokens[q].text==",") break;
            if(tokens[q].text=="="){in_default=true;break;}
            if(tokens[q].text==":" ) break;
        }
        if(in_default) continue;
        add(tokens[p].text);
    }
    return names;
}

static void collect_commonjs_replacements(const std::string& erased_source,
                                          const std::vector<Token>& tokens,
                                          std::vector<Replacement>& replacements,
                                          std::unordered_map<std::string, ImportedBinding>& imported_bindings,
                                          Diagnostics& diagnostics) {
    auto add=[&](std::size_t first,std::size_t last,std::string text){
        if(first>=tokens.size()||last>=tokens.size()||first>last)return;
        replacements.push_back({tokens[first].begin,tokens[last].end,std::move(text)});
    };
    auto import_spec=[&](std::size_t p)->std::string{
        return p<tokens.size()&&tokens[p].kind==TokenKind::String ?
               unquote_module(tokens[p].text):std::string();
    };
    unsigned temp_id=0;

    for(std::size_t i=0;i<tokens.size();++i){
        if(tokens[i].kind==TokenKind::Comment) continue;

        if(tokens[i].text=="import"){
            // Parser owns declaration-only imports and import-equals.
            if(i+1<tokens.size() && tokens[i+1].text=="type") continue;
            if(i+2<tokens.size() && tokens[i+1].kind==TokenKind::Identifier &&
               tokens[i+2].text=="=") continue;

            const std::size_t end=statement_end_token(tokens,i);
            if(end<=i) continue;

            // import "module";
            if(i+1<end && tokens[i+1].kind==TokenKind::String){
                add(i,end,"require("+js_quote(import_spec(i+1))+");");
                i=end; continue;
            }

            std::size_t from=i+1;
            while(from<end && tokens[from].text!="from") ++from;
            if(from>=end || from+1>=end || tokens[from+1].kind!=TokenKind::String)
                continue;
            const std::string spec=import_spec(from+1);
            const std::string req="require("+js_quote(spec)+")";

            std::size_t p=i+1;
            std::string code;

            // Default imports remain live by keeping the module object and
            // rewriting references to its `.default` property.
            if(p<from && tokens[p].kind==TokenKind::Identifier){
                const std::string local=tokens[p].text;
                const std::string temp="__tscc_mod"+std::to_string(temp_id++);
                code+="const "+temp+" = "+req+";";
                imported_bindings[local]={temp+".default",tokens[i].begin,tokens[end].end};
                ++p;
                if(p<from && tokens[p].text==",") ++p;
            }

            if(p<from && tokens[p].text=="*"){
                if(p+2<from && tokens[p+1].text=="as" &&
                   tokens[p+2].kind==TokenKind::Identifier)
                    code+="const "+tokens[p+2].text+" = "+req+";";
            } else if(p<from && tokens[p].text=="{"){
                const std::string temp="__tscc_mod"+std::to_string(temp_id++);
                code+="const "+temp+" = "+req+";";
                ++p;
                while(p<from && tokens[p].text!="}"){
                    if(tokens[p].text==","){++p;continue;}
                    if(tokens[p].text=="type"){
                        ++p;
                        while(p<from && tokens[p].text!="," && tokens[p].text!="}") ++p;
                        continue;
                    }
                    if(tokens[p].kind!=TokenKind::Identifier && tokens[p].text!="default"){++p;continue;}
                    const std::string imported=tokens[p].text;
                    std::string local=imported;
                    if(p+2<from && tokens[p+1].text=="as" &&
                       tokens[p+2].kind==TokenKind::Identifier){
                        local=tokens[p+2].text; p+=3;
                    } else ++p;
                    imported_bindings[local]={temp+"."+imported,tokens[i].begin,tokens[end].end};
                    while(p<from && tokens[p].text!="," && tokens[p].text!="}") ++p;
                }
            }
            add(i,end,code);
            i=end; continue;
        }

        if(tokens[i].text!="export") continue;

        // Function/class declarations are brace-delimited, not semicolon
        // statements. Handle them before generic statement-end discovery so an
        // exported function cannot accidentally consume later declarations.
        if(i+2<tokens.size() &&
           (tokens[i+1].text=="function" || tokens[i+1].text=="class") &&
           tokens[i+2].kind==TokenKind::Identifier) {
            std::size_t open=i+3;
            while(open<tokens.size()&&tokens[open].text!="{")++open;
            if(open<tokens.size()){
                int depth=0;std::size_t close=open;
                for(;close<tokens.size();++close){
                    if(tokens[close].text=="{")++depth;
                    else if(tokens[close].text=="}"&&--depth==0)break;
                }
                if(close<tokens.size()){
                    replacements.push_back({tokens[i].begin,tokens[i+1].begin,""});
                    replacements.push_back({tokens[close].end,tokens[close].end,
                        ";Object.defineProperty(exports,"+js_quote(tokens[i+2].text)+
                        ",{enumerable:true,get:()=>"+tokens[i+2].text+"});"});
                    i=close;continue;
                }
            }
        }

        const std::size_t end=statement_end_token(tokens,i);
        if(end<=i) continue;

        // Declaration-only forms are parser-owned.
        if(i+1<end && (tokens[i+1].text=="type" ||
                       (tokens[i+1].text=="as" && i+2<end && tokens[i+2].text=="namespace")))
            continue;

        // export = is parser-owned.
        if(i+1<end && tokens[i+1].text=="=") continue;

        // export * as ns from "m";
        if(i+5<end && tokens[i+1].text=="*" && tokens[i+2].text=="as" &&
           tokens[i+3].kind==TokenKind::Identifier && tokens[i+4].text=="from" &&
           tokens[i+5].kind==TokenKind::String){
            const std::string temp="__tscc_mod"+std::to_string(temp_id++);
            const std::string name=tokens[i+3].text;
            std::string code="const "+temp+"=require("+js_quote(import_spec(i+5))+");";
            code+="Object.defineProperty(exports,"+js_quote(name)+
                  ",{enumerable:true,get:()=>"+temp+"});";
            add(i,end,code); i=end; continue;
        }

        // export * from "m";
        if(i+3<end && tokens[i+1].text=="*" && tokens[i+2].text=="from" &&
           tokens[i+3].kind==TokenKind::String){
            const std::string temp="__tscc_mod"+std::to_string(temp_id++);
            const std::string spec=import_spec(i+3);
            std::string code="const "+temp+"=require("+js_quote(spec)+");"
                             "for(const k in "+temp+")if(k!==\"default\"&&k!==\"__esModule\")"
                             "Object.defineProperty(exports,k,{enumerable:true,get:()=>"+temp+"[k]});";
            add(i,end,code); i=end; continue;
        }

        // export { a, b as c } [from "m"];
        if(i+1<end && tokens[i+1].text=="{"){
            std::size_t close=i+2;
            while(close<end && tokens[close].text!="}") ++close;
            if(close>=end) continue;
            bool reexport=close+2<end && tokens[close+1].text=="from" &&
                          tokens[close+2].kind==TokenKind::String;
            std::string temp;
            std::string code;
            if(reexport){
                temp="__tscc_mod"+std::to_string(temp_id++);
                code+="const "+temp+"=require("+js_quote(import_spec(close+2))+");";
            }
            std::size_t q=i+2;
            while(q<close){
                if(tokens[q].text==","){++q;continue;}
                if(tokens[q].text=="type"){
                    ++q; while(q<close&&tokens[q].text!=","&&tokens[q].text!="}")++q; continue;
                }
                if(tokens[q].kind!=TokenKind::Identifier && tokens[q].text!="default"){++q;continue;}
                const std::string local=tokens[q].text;
                std::string exported=local;
                if(q+2<close && tokens[q+1].text=="as" &&
                   tokens[q+2].kind==TokenKind::Identifier){
                    exported=tokens[q+2].text; q+=3;
                } else ++q;
                std::string value_expr;
                if(reexport) value_expr=temp+"."+local;
                else {
                    auto imported=imported_bindings.find(local);
                    value_expr=imported==imported_bindings.end()?local:imported->second.expression;
                }
                code+="Object.defineProperty(exports,"+js_quote(exported)+
                      ",{enumerable:true,get:()=>"+value_expr+"});";
                while(q<close&&tokens[q].text!=","&&tokens[q].text!="}")++q;
            }
            add(i,end,code); i=end; continue;
        }

        // export default function/class [Name] ...
        if(i+2<tokens.size() && tokens[i+1].text=="default" &&
           (tokens[i+2].text=="function"||tokens[i+2].text=="class")){
            std::size_t name=i+3;
            const bool named=name<tokens.size()&&tokens[name].kind==TokenKind::Identifier;
            std::size_t open=i+3;
            while(open<tokens.size()&&tokens[open].text!="{")++open;
            if(open>=tokens.size())continue;
            int depth=0;std::size_t close=open;
            for(;close<tokens.size();++close){
                if(tokens[close].text=="{")++depth;
                else if(tokens[close].text=="}"&&--depth==0)break;
            }
            if(close>=tokens.size())continue;
            if(named){
                // Remove only `export default`, preserve the declaration, then
                // append the CommonJS default binding.
                replacements.push_back({tokens[i].begin,tokens[i+2].begin,""});
                replacements.push_back({tokens[close].end,tokens[close].end,
                                        ";exports.default="+tokens[name].text+";"});
            }else{
                const std::string expr=erased_source.substr(tokens[i+2].begin,
                                                            tokens[close].end-tokens[i+2].begin);
                add(i,close,"exports.default = "+expr+";");
            }
            i=close;continue;
        }

        // export default expression;
        if(i+1<end && tokens[i+1].text=="default"){
            const std::size_t expr_first=i+2;
            if(expr_first<end){
                const std::size_t expr_end=tokens[end].text==";"?end:end+1;
                const std::size_t a=tokens[expr_first].begin;
                const std::size_t b=tokens[expr_end-1].end;
                add(i,end,"exports.default = "+erased_source.substr(a,b-a)+";");
            }
            i=end;continue;
        }

        // export const/let/var NAME ...;  Keep declaration and append binding.
        if(i+2<end && (tokens[i+1].text=="const"||tokens[i+1].text=="let"||tokens[i+1].text=="var")){
            std::vector<std::string> names;
            std::size_t q=i+2;
            while(q<end){
                while(q<end && tokens[q].text==",")++q;
                if(q>=end)break;
                std::size_t binding_end=q+1;
                if(tokens[q].text=="{"||tokens[q].text=="["){
                    const std::string open=tokens[q].text, close=open=="{"?"}":"]";
                    int d=0; binding_end=q;
                    for(;binding_end<end;++binding_end){
                        if(tokens[binding_end].text==open)++d;
                        else if(tokens[binding_end].text==close&&--d==0){++binding_end;break;}
                    }
                }
                auto found=exported_binding_names(tokens,q,binding_end);
                for(const auto& n:found) if(std::find(names.begin(),names.end(),n)==names.end())names.push_back(n);
                int paren=0,square=0,brace=0;
                q=binding_end;
                for(;q<end;++q){
                    const auto& x=tokens[q].text;
                    if(x=="(")++paren;else if(x==")"&&paren)--paren;
                    else if(x=="[")++square;else if(x=="]"&&square)--square;
                    else if(x=="{")++brace;else if(x=="}"&&brace)--brace;
                    if(!paren&&!square&&!brace&&x==","){++q;break;}
                }
            }
            replacements.push_back({tokens[i].begin,tokens[i+1].begin,""});
            std::string suffix;
            for(const auto& name:names)
                suffix+="Object.defineProperty(exports,"+js_quote(name)+
                        ",{enumerable:true,get:()=>"+name+"});";
            replacements.push_back({tokens[end].end,tokens[end].end,suffix});
            i=end;continue;
        }

        // Exported enums/namespaces are parser-owned runtime transforms.
        // Remove the export keyword and expose the resulting runtime object
        // after its source declaration closes.
        if(i+2<tokens.size() &&
           (tokens[i+1].text=="enum"||tokens[i+1].text=="namespace") &&
           tokens[i+2].kind==TokenKind::Identifier){
            replacements.push_back({tokens[i].begin,tokens[i+1].begin,""});
            std::size_t open=i+2;while(open<tokens.size()&&tokens[open].text!="{")++open;
            if(open<tokens.size()){
                int depth=0;std::size_t close=open;
                for(;close<tokens.size();++close){
                    if(tokens[close].text=="{")++depth;
                    else if(tokens[close].text=="}"&&--depth==0)break;
                }
                if(close<tokens.size()){
                    replacements.push_back({tokens[close].end,tokens[close].end,
                        ";Object.defineProperty(exports,"+js_quote(tokens[i+2].text)+
                        ",{enumerable:true,get:()=>"+tokens[i+2].text+"});"});
                    i=close;continue;
                }
            }
        }
    }

    (void)diagnostics;
}


static std::vector<std::pair<std::size_t,std::size_t>>
shadow_ranges_for(const std::vector<Token>& tokens, const std::string& name) {
    std::vector<std::pair<std::size_t,std::size_t>> ranges;

    auto matching=[&](std::size_t open,const std::string&a,const std::string&b){
        int depth=0;
        for(std::size_t p=open;p<tokens.size();++p){
            if(tokens[p].text==a)++depth;
            else if(tokens[p].text==b&&--depth==0)return p;
        }
        return tokens.size();
    };

    // Function/method parameters shadow throughout their body.
    for(std::size_t i=0;i+1<tokens.size();++i){
        if(tokens[i].text!="(") continue;
        // Control-flow headers are not parameter lists.
        if(i>0 && (tokens[i-1].text=="if" || tokens[i-1].text=="for" ||
                   tokens[i-1].text=="while" || tokens[i-1].text=="switch" ||
                   tokens[i-1].text=="with" || tokens[i-1].text=="catch")) continue;
        const auto close=matching(i,"(",")");
        if(close>=tokens.size()) continue;
        bool has_name=false;
        int brace=0,square=0;
        bool in_top_level_type=false;
        std::size_t segment_start=i+1;
        for(std::size_t p=i+1;p<close;++p){
            const auto& x=tokens[p].text;
            if(x=="{"){++brace;continue;}
            if(x=="}"&&brace){--brace;continue;}
            if(x=="["){++square;continue;}
            if(x=="]"&&square){--square;continue;}
            if(!brace&&!square&&x==","){segment_start=p+1;in_top_level_type=false;continue;}
            if(!brace&&!square&&x==":"){in_top_level_type=true;continue;}
            if(in_top_level_type) continue;
            if(tokens[p].kind==TokenKind::Identifier&&tokens[p].text==name){
                // In a destructuring pattern `{name: local}`, `name` is only a
                // property key. A simple `name: Type` at the segment start is
                // still a binding.
                const bool property_key=(brace||square) && p+1<close && tokens[p+1].text==":";
                if(!property_key || p==segment_start){has_name=true;break;}
            }
        }
        if(!has_name) continue;
        std::size_t body=close+1;
        while(body<tokens.size()&&tokens[body].text!="{"&&
              tokens[body].text!="=>"&&tokens[body].text!=";")++body;
        if(body<tokens.size()&&tokens[body].text=="=>") {
            ++body;
            if(body<tokens.size()&&tokens[body].text!="{"){
                const auto end=statement_end_token(tokens,body);
                ranges.push_back({tokens[i].begin,tokens[end].end});
                continue;
            }
        }
        if(body<tokens.size()&&tokens[body].text=="{"){
            const auto end=matching(body,"{","}");
            if(end<tokens.size()) ranges.push_back({tokens[i].begin,tokens[end].end});
        }
    }

    // catch(name) shadows inside the catch block.
    for(std::size_t i=0;i+4<tokens.size();++i){
        if(tokens[i].text!="catch"||tokens[i+1].text!="(")continue;
        const auto close=matching(i+1,"(",")");
        if(close>=tokens.size())continue;
        bool match=false;
        for(std::size_t p=i+2;p<close;++p)
            if(tokens[p].kind==TokenKind::Identifier&&tokens[p].text==name){match=true;break;}
        if(!match)continue;
        std::size_t body=close+1;
        while(body<tokens.size()&&tokens[body].text!="{")++body;
        if(body<tokens.size()){
            const auto end=matching(body,"{","}");
            if(end<tokens.size())ranges.push_back({tokens[i].begin,tokens[end].end});
        }
    }

    // for(let/const name ...) shadows across the loop header and body.
    for(std::size_t i=0;i+4<tokens.size();++i){
        if(tokens[i].text!="for" || tokens[i+1].text!="(") continue;
        const auto close=matching(i+1,"(",")");
        if(close>=tokens.size()) continue;
        bool declares=false;
        for(std::size_t q=i+2;q<close;++q){
            if(tokens[q].text!="let"&&tokens[q].text!="const"&&tokens[q].text!="var") continue;
            // The binding may be a plain identifier or a destructuring pattern.
            int paren=0,square=0,brace=0;
            for(std::size_t r=q+1;r<close;++r){
                const auto& x=tokens[r].text;
                if(!paren&&!square&&!brace&&(x=="="||x=="of"||x=="in"||x==";")) break;
                if(tokens[r].kind==TokenKind::Identifier&&tokens[r].text==name){
                    // In `{x: y}`, x is a property key while y is the binding.
                    if(r+1<close&&tokens[r+1].text==":") continue;
                    declares=true;break;
                }
                if(x=="(")++paren; else if(x==")"&&paren)--paren;
                else if(x=="[")++square; else if(x=="]"&&square)--square;
                else if(x=="{")++brace; else if(x=="}"&&brace)--brace;
            }
            if(declares)break;
        }
        if(!declares) continue;
        std::size_t body=close+1;
        while(body<tokens.size()&&tokens[body].kind==TokenKind::Comment)++body;
        if(body<tokens.size()&&tokens[body].text=="{"){
            const auto end=matching(body,"{","}");
            if(end<tokens.size()) ranges.push_back({tokens[i].begin,tokens[end].end});
        } else {
            const auto end=statement_end_token(tokens,body);
            if(end<tokens.size()) ranges.push_back({tokens[i].begin,tokens[end].end});
        }
    }

    // Block-scoped local declarations shadow from declaration to block end.
    for(std::size_t i=0;i+1<tokens.size();++i){
        if(tokens[i].text!="let"&&tokens[i].text!="const"&&tokens[i].text!="class"&&tokens[i].text!="function")
            continue;
        std::size_t p=i+1;
        bool declares=false;
        while(p<tokens.size()&&tokens[p].text!=";"&&tokens[p].kind!=TokenKind::End){
            if(tokens[p].kind==TokenKind::Identifier&&tokens[p].text==name){
                const bool candidate=p==i+1 || tokens[p-1].text==",";
                if(candidate){declares=true;break;}
            }
            if(tokens[p].text=="{"&&tokens[i].text!="function"&&tokens[i].text!="class") break;
            ++p;
        }
        if(!declares)continue;
        int depth=0;
        std::size_t block_open=tokens.size();
        for(std::size_t q=i;q>0;){
            --q;
            if(tokens[q].text=="}")++depth;
            else if(tokens[q].text=="{"){
                if(depth)--depth;
                else{block_open=q;break;}
            }
        }
        if(block_open<tokens.size()){
            const auto end=matching(block_open,"{","}");
            if(end<tokens.size())ranges.push_back({tokens[i].begin,tokens[end].end});
        }
    }
    return ranges;
}

static bool inside_range(std::size_t pos,
                         const std::vector<std::pair<std::size_t,std::size_t>>& ranges) {
    for(const auto&r:ranges) if(pos>=r.first&&pos<r.second)return true;
    return false;
}

static bool object_literal_context(const std::vector<Token>& tokens, std::size_t index) {
    int paren=0,square=0,brace=0;
    for(std::size_t p=index;p>0;){
        --p;
        const auto& x=tokens[p].text;
        if(x==")"){++paren;continue;}
        if(x=="]"){++square;continue;}
        if(x=="}"){++brace;continue;}
        if(x=="("){if(paren){--paren;continue;} return false;}
        if(x=="["){if(square){--square;continue;} return false;}
        if(x=="{"){
            if(brace){--brace;continue;}
            if(paren||square) continue;
            if(p==0) return false;
            const auto& before=tokens[p-1].text;
            return before=="="||before=="("||before=="["||before==","||
                   before==":"||before=="return"||before=="=>"||
                   before=="?"||before=="??"||before=="||"||before=="&&";
        }
    }
    return false;
}

static void add_live_import_reference_replacements(
    const std::vector<Token>& tokens,
    const std::unordered_map<std::string, ImportedBinding>& bindings,
    const BindingModel& binding_model,
    std::vector<Replacement>& replacements) {

    // Templates are a single lexer token. Rewrite all imported identifiers in
    // one pass so multiple live bindings in the same template cannot create
    // overlapping replacements.
    for(const auto& tok:tokens){
        if(tok.kind!=TokenKind::Template) continue;
        std::string raw=tok.text;
        bool changed=false; int interpolation=0;
        for(std::size_t p=0;p<raw.size();){
            if(p+1<raw.size()&&raw[p]=='$'&&raw[p+1]=='{'){++interpolation;p+=2;continue;}
            if(interpolation&&raw[p]=='}'){--interpolation;++p;continue;}
            if(interpolation && (std::isalpha(static_cast<unsigned char>(raw[p]))||raw[p]=='_'||raw[p]=='$')){
                std::size_t e=p+1;
                while(e<raw.size()&&(std::isalnum(static_cast<unsigned char>(raw[e]))||raw[e]=='_'||raw[e]=='$'))++e;
                const std::string word=raw.substr(p,e-p);
                auto it=bindings.find(word);
                if(it!=bindings.end()){
                    const char before=p?raw[p-1]:'\0';
                    if(before!='.' && before!='?'){
                        raw.replace(p,e-p,it->second.expression);
                        p+=it->second.expression.size();changed=true;continue;
                    }
                }
                p=e;continue;
            }
            ++p;
        }
        if(changed) replacements.push_back({tok.begin,tok.end,raw});
    }

    for(const auto& [name,binding] : bindings){
        const auto shadows=shadow_ranges_for(tokens,name);
        for(std::size_t i=0;i<tokens.size();++i){
            const auto& tok=tokens[i];
            if(tok.kind==TokenKind::Template) continue;
            if(tok.kind!=TokenKind::Identifier || tok.text!=name) continue;
            if(tok.begin>=binding.declaration_begin&&tok.begin<binding.declaration_end) continue;
            // An ordinary identifier resolved by the binder belongs to a local
            // declaration and is not a live reference to the imported binding.
            // The legacy shadow ranges remain as a compatibility bridge for
            // destructuring, arrows, catch bindings, classes and other binding
            // forms outside the first binder contract.
            if (binding_model.symbol_for_reference(i) != static_cast<std::size_t>(-1)) continue;
            std::size_t statement_start=i;
            while(statement_start>0 && tokens[statement_start-1].text!=";" &&
                  tokens[statement_start-1].kind!=TokenKind::End) --statement_start;
            if(statement_start<tokens.size() && tokens[statement_start].text=="import") continue;
            if(inside_range(tok.begin,shadows)) continue;

            if(i>0&&(tokens[i-1].text=="."||tokens[i-1].text=="?.")) continue;
            if(i>0&&(tokens[i-1].text=="break"||tokens[i-1].text=="continue")) continue;
            if(i+1<tokens.size()&&tokens[i+1].text==":") continue;
            if(i+1<tokens.size()&&tokens[i+1].text==":" &&
               (i==0||tokens[i-1].text==";"||tokens[i-1].text=="{"||tokens[i-1].text=="}"))
                continue;

            const bool shorthand =
                i>0 && i+1<tokens.size() &&
                (tokens[i-1].text=="{"||tokens[i-1].text==",") &&
                (tokens[i+1].text=="}"||tokens[i+1].text==",") &&
                object_literal_context(tokens,i);
            replacements.push_back({tok.begin,tok.end,
                shorthand ? name+":"+binding.expression : binding.expression});
        }
    }
}

bool transpile_tokens(const SourceFile& source, const std::vector<Token>& tokens,
                      const TranspileOptions& options,
                      std::string& out, Diagnostics& diagnostics) {
    const bool tsx_source = source.path.size() >= 4 &&
                            source.path.substr(source.path.size()-4) == ".tsx";
    if (tsx_source && !validate_tsx_structure(source,tokens,diagnostics)) return false;

    Program program;
    Parser parser(source, tokens, diagnostics);
    if (!parser.parse(program)) return false;
    const auto semantic = build_semantic_model(tokens, program);
    const auto binding = bind_semantic_model(tokens, program, semantic);
    (void)binding;
    if (!check_program(source, tokens, program, semantic, binding, diagnostics)) return false;

    out = source.text;
    for (const auto& range : program.erasures)
        blank_range(out, range.begin, range.end);

    // Expression-level TypeScript assertions remain a token-level transform for
    // this checkpoint; declarations/signatures are now parser-owned.
    auto module_clause_as = [&](std::size_t pos) {
        if(tokens[pos].text!="as") return false;
        std::size_t start=pos;
        while(start>0 && tokens[start-1].text!=";" && tokens[start-1].kind!=TokenKind::End) --start;
        if(start>=tokens.size() || (tokens[start].text!="import" && tokens[start].text!="export")) return false;
        // `export default (x as T)` is an expression assertion, not a module alias.
        if(start+1<pos && tokens[start].text=="export" && tokens[start+1].text=="default") return false;
        int brace=0;
        for(std::size_t p=start;p<pos;++p){
            if(tokens[p].text=="{")++brace;
            else if(tokens[p].text=="}"&&brace)--brace;
        }
        return brace>0 || (pos>start && tokens[pos-1].text=="*");
    };
    for (std::size_t i = 0; i + 1 < tokens.size(); ++i) {
        if (tokens[i].text != "as" && tokens[i].text != "satisfies") continue;
        if (module_clause_as(i)) continue;
        std::size_t j = i + 1;
        if (tokens[i].text == "as" && j < tokens.size() && tokens[j].text == "const") {
            const std::size_t after = j + 1;
            if (after < tokens.size()) {
                const auto& x=tokens[after].text;
                const bool boundary=x==","||x==";"||x==")"||x=="]"||x=="}"||x=="."||x=="?."||x=="?"||x==":"||x=="as"||x=="satisfies"||tokens[after].kind==TokenKind::End;
                if(!boundary){const auto [line,column]=source.line_col(tokens[i].begin);diagnostics.error(source.path,line,column,"unexpected token after 'as const'",source.line_text(line));return false;}
            }
            blank_range(out,tokens[i].begin,tokens[j].end); i=j; continue;
        }
        int angle = 0, square = 0, paren = 0, brace = 0;
        if (j >= tokens.size() || tokens[j].kind == TokenKind::End || tokens[j].text == "as" || tokens[j].text == "satisfies" ||
            tokens[j].text == ";" || tokens[j].text == "," || tokens[j].text == ")") {
            const auto [line,column]=source.line_col(tokens[i].begin);
            diagnostics.error(source.path,line,column,"expected type after '"+tokens[i].text+"'",source.line_text(line));
            return false;
        }
        for (; j < tokens.size(); ++j) {
            const auto& x = tokens[j].text;
            if (j > i + 1 && tokens[j-1].kind == TokenKind::Identifier && tokens[j].kind == TokenKind::Identifier &&
                tokens[j-1].text != "keyof" && tokens[j-1].text != "typeof" && tokens[j-1].text != "infer" &&
                tokens[j-1].text != "readonly" && tokens[j-1].text != "extends" && tokens[j-1].text != "is" && tokens[j-1].text != "asserts") {
                const auto [line,column]=source.line_col(tokens[j].begin);
                diagnostics.error(source.path,line,column,"unexpected identifier in type assertion",source.line_text(line)); return false;
            }
            if (!angle && !square && !paren && !brace &&
                (x == "," || x == ";" || x == ")" || x == "}" || x == "." || x == "?" || x == ":" || x == "as" || x == "satisfies"))
                break;
            if (x == "<") ++angle;
            else if (x == ">" && angle) --angle;
            else if (x == "[") ++square;
            else if (x == "]" && square) --square;
            else if (x == "(") ++paren;
            else if (x == ")" && paren) --paren;
            else if (x == "{") ++brace;
            else if (x == "}" && brace) --brace;
        }
        blank_range(out, tokens[i].begin, j < tokens.size() ? tokens[j].begin : tokens[i].end);
        if (j) i = j - 1;
    }

    // Angle-bracket assertions (`<Type>expr`) are TypeScript-only outside
    // TSX. In .tsx files `<...>` belongs to JSX/generic syntax, so this pass is
    // deliberately disabled exactly where TypeScript disallows angle assertions.
    if (!tsx_source) for (std::size_t i = 0; i + 2 < tokens.size(); ++i) {
        if (tokens[i].text != "<") continue;
        const bool prefix = i == 0 || tokens[i-1].text == "=" || tokens[i-1].text == "(" ||
                            tokens[i-1].text == "[" || tokens[i-1].text == "," || tokens[i-1].text == ":" ||
                            tokens[i-1].text == "return" || tokens[i-1].text == "=>";
        if (!prefix) continue;
        int depth = 0; std::size_t close = i;
        for (; close < tokens.size(); ++close) {
            if (tokens[close].text == "<") ++depth;
            else if (tokens[close].text == ">" && --depth == 0) break;
            else if (tokens[close].text == ">>" || tokens[close].text == ">>>") {
                depth -= static_cast<int>(tokens[close].text.size()); if (depth <= 0) break;
            }
        }
        if (close >= tokens.size() || close == i + 1 || close + 1 >= tokens.size() || tokens[close+1].kind == TokenKind::End) {
            const auto [line,column]=source.line_col(tokens[i].begin);
            diagnostics.error(source.path,line,column,close==i+1?"expected type inside angle assertion":"unterminated angle assertion",source.line_text(line));
            return false;
        }
        // `<T>(...) =>` is a generic arrow; `<Type>(expr)` is an assertion.
        if (tokens[close+1].text == "(") {
            std::size_t pc=close+1;int pd=0;for(;pc<tokens.size();++pc){if(tokens[pc].text=="(")++pd;else if(tokens[pc].text==")"&&--pd==0)break;}
            std::size_t after=pc+1;while(after<tokens.size()&&tokens[after].text==":" ){++after;while(after<tokens.size()&&tokens[after].text!="=>"&&tokens[after].text!=";"&&tokens[after].kind!=TokenKind::End)++after;}
            if(after<tokens.size()&&tokens[after].text=="=>") continue;
        }
        blank_range(out, tokens[i].begin, tokens[close].end);
        i = close;
    }

    // Postfix non-null assertions may be chained (`value!!`). Erase every `!`
    // in a postfix run when the run follows a value and is followed by a legal
    // value continuation.
    for (std::size_t i = 1; i + 1 < tokens.size(); ++i) {
        if (tokens[i].text != "!") continue;
        std::size_t first = i;
        while (first > 0 && tokens[first-1].text == "!") --first;
        std::size_t after = i;
        while (after + 1 < tokens.size() && tokens[after+1].text == "!") ++after;
        const auto& prev = tokens[first-1];
        const auto& next = tokens[after+1].text;
        const bool value_before = prev.kind == TokenKind::Identifier || prev.kind == TokenKind::Number ||
                                  prev.kind == TokenKind::String || prev.text == ")" || prev.text == "]";
        const bool continuation = next == "." || next == "?." || next == "(" || next == "[" ||
                                  next == ";" || next == "," || next == ")" || next == "}";
        if (value_before && continuation) {
            for (std::size_t q = first; q <= after; ++q) blank_range(out,tokens[q].begin,tokens[q].end);
            i = after;
        }
    }

    if (options.remove_comments)
        for (const auto& token : tokens)
            if (token.kind == TokenKind::Comment) blank_range(out, token.begin, token.end);

    // Runtime-bearing TypeScript constructs (currently enums and constructor
    // parameter properties) are parser-owned replacements/insertions. Apply
    // from the end of the source so original offsets remain stable.
    auto replacements = program.replacements;
    if (options.module=="commonjs") {
        std::unordered_map<std::string, ImportedBinding> imported_bindings;
        collect_commonjs_replacements(out,tokens,replacements,imported_bindings,diagnostics);
        add_live_import_reference_replacements(tokens,imported_bindings,binding,replacements);
    }
    if (!apply_replacements(source, out, replacements, diagnostics)) return false;

    // Trim erasure-introduced trailing spaces while preserving line shape.
    std::string clean;
    clean.reserve(out.size());
    std::size_t a = 0;
    while (a < out.size()) {
        const auto newline = out.find('\n', a);
        auto b = newline == std::string::npos ? out.size() : newline;
        while (b > a && (out[b - 1] == ' ' || out[b - 1] == '\t')) --b;
        clean.append(out, a, b - a);
        if (newline == std::string::npos) break;
        clean.push_back('\n');
        a = newline + 1;
    }
    out.swap(clean);
    return !diagnostics.has_errors();
}

bool transpile(const SourceFile& source, const TranspileOptions& options,
               std::string& out, Diagnostics& diagnostics) {
    Lexer lexer(source, diagnostics);
    const auto tokens=lexer.lex();
    if (diagnostics.has_errors()) return false;
    return transpile_tokens(source,tokens,options,out,diagnostics);
}

} // namespace tscc
