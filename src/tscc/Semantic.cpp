#include "Semantic.h"

namespace tscc {
namespace {
std::size_t matching(const std::vector<Token>& tokens, std::size_t open,
                     const char* left, const char* right) {
    int depth = 0;
    for (std::size_t i = open; i < tokens.size(); ++i) {
        if (tokens[i].kind == TokenKind::Comment) continue;
        if (tokens[i].text == left) ++depth;
        else if (tokens[i].text == right && --depth == 0) return i;
    }
    return tokens.size();
}

std::size_t first_parameter_name(const std::vector<Token>& tokens,
                                 std::size_t begin, std::size_t end) {
    for (auto i = begin; i < end; ++i) {
        if (tokens[i].kind == TokenKind::Comment) continue;
        if (tokens[i].text == "public" || tokens[i].text == "private" ||
            tokens[i].text == "protected" || tokens[i].text == "readonly" ||
            tokens[i].text == "override" || tokens[i].text == "...")
            continue;
        if (tokens[i].kind == TokenKind::Identifier) return i;
        break; // Destructuring and other complex bindings are outside this slice.
    }
    return static_cast<std::size_t>(-1);
}

std::size_t statement_end(const std::vector<Token>& tokens, std::size_t begin) {
    int paren = 0, square = 0, brace = 0;
    for (std::size_t i = begin; i < tokens.size(); ++i) {
        const auto& text = tokens[i].text;
        if (text == "(") ++paren; else if (text == ")" && paren) --paren;
        else if (text == "[") ++square; else if (text == "]" && square) --square;
        else if (text == "{") ++brace; else if (text == "}" && brace) --brace;
        if (!paren && !square && !brace && text == ";") return i + 1;
        if (tokens[i].kind == TokenKind::End) return i;
    }
    return tokens.size();
}
}

SemanticModel build_semantic_model(const std::vector<Token>& tokens, const Program& program) {
    SemanticModel model;
    for (std::size_t i = 0; i < program.variables.size(); ++i) {
        const auto& variable = program.variables[i];
        std::size_t end = variable.initializer_end_token > variable.initializer_begin_token
                              ? variable.initializer_end_token
                              : variable.type_end_token;
        if (end <= variable.name_token) end = variable.name_token + 1;
        model.nodes.push_back({SemanticNodeKind::VariableDeclaration,
                               variable.name_token, end, variable.name_token, i});
    }

    // Flat object/array binding patterns. Nested patterns and computed keys are
    // deliberately left for the next binder expansion rather than guessed.
    for (std::size_t keyword=0;keyword+2<tokens.size();++keyword) {
        if (tokens[keyword].text!="const"&&tokens[keyword].text!="let"&&tokens[keyword].text!="var") continue;
        const auto open=keyword+1;if(tokens[open].text!="{"&&tokens[open].text!="[")continue;
        const auto close=matching(tokens,open,tokens[open].text.c_str(),tokens[open].text=="{"?"}":"]");if(close>=tokens.size())continue;
        for(std::size_t p=open+1;p<close;++p){if(tokens[p].kind!=TokenKind::Identifier)continue;const bool at_binding_start=p==open+1||tokens[p-1].text==","||tokens[p-1].text=="...";bool binding=tokens[open].text=="["&&at_binding_start;if(tokens[open].text=="{"){const bool alias=p>open+1&&tokens[p-1].text==":";const bool shorthand=at_binding_start&&(p+1>=close||tokens[p+1].text==","||tokens[p+1].text=="}"||tokens[p+1].text=="=");binding=alias||shorthand;}if(binding)model.nodes.push_back({SemanticNodeKind::BindingDeclaration,p,p+1,p});}
        keyword=close;
    }

    for (std::size_t function_token = 0; function_token < tokens.size(); ++function_token) {
        if (tokens[function_token].text != "function") continue;
        std::size_t open = function_token + 1;
        while (open < tokens.size() && tokens[open].text != "(" &&
               tokens[open].text != ";" && tokens[open].text != "{") ++open;
        if (open >= tokens.size() || tokens[open].text != "(") continue;
        const auto close = matching(tokens, open, "(", ")");
        if (close >= tokens.size()) continue;
        std::size_t body = close + 1;
        while (body < tokens.size() && tokens[body].text != "{" &&
               tokens[body].text != ";" && tokens[body].kind != TokenKind::End) ++body;
        if (body >= tokens.size() || (tokens[body].text != "{" && tokens[body].text != ";")) continue;
        const bool ambient = tokens[body].text == ";";
        const auto body_close = ambient ? body : matching(tokens, body, "{", "}");
        if (body_close >= tokens.size()) continue;
        std::size_t name = function_token + 1;
        while (name < open && tokens[name].kind == TokenKind::Comment) ++name;
        if (ambient)
            model.nodes.push_back({SemanticNodeKind::LexicalRegion, open, body_close + 1,
                                   static_cast<std::size_t>(-1), static_cast<std::size_t>(-1),
                                   open, body_close + 1});
        model.nodes.push_back({SemanticNodeKind::FunctionDeclaration,
                               function_token, body_close + 1,
                               name < open ? name : static_cast<std::size_t>(-1),
                               static_cast<std::size_t>(-1), ambient ? open : body});

        std::size_t part = open + 1;
        int paren = 0, square = 0, brace = 0, angle = 0;
        for (std::size_t i = part; i <= close; ++i) {
            const auto& text = tokens[i].text;
            if (text == "(") ++paren; else if (text == ")" && paren) --paren;
            else if (text == "[") ++square; else if (text == "]" && square) --square;
            else if (text == "{") ++brace; else if (text == "}" && brace) --brace;
            else if (text == "<") ++angle; else if (text == ">" && angle) --angle;
            if (i == close || (text == "," && !paren && !square && !brace && !angle)) {
                const auto parameter_name = first_parameter_name(tokens, part, i);
                if (parameter_name != static_cast<std::size_t>(-1))
                    model.nodes.push_back({SemanticNodeKind::ParameterDeclaration,
                                           part, i, parameter_name,
                                           static_cast<std::size_t>(-1), ambient ? open : body});
                part = i + 1;
            }
        }
    }

    // Arrow functions need durable parameter/function-scope facts too. This
    // bounded slice covers identifier parameter lists and single identifiers;
    // destructured parameters remain explicit future binder work.
    for (std::size_t arrow = 0; arrow < tokens.size(); ++arrow) {
        if (tokens[arrow].text != "=>") continue;
        std::size_t body = arrow + 1, end = statement_end(tokens, body), scope = arrow;
        if (body < tokens.size() && tokens[body].text == "{") {
            const auto close = matching(tokens, body, "{", "}");
            if (close >= tokens.size()) continue;
            end = close + 1; scope = body;
        } else {
            model.nodes.push_back({SemanticNodeKind::LexicalRegion, arrow, end,
                                   static_cast<std::size_t>(-1), static_cast<std::size_t>(-1),
                                   arrow, end});
        }
        model.nodes.push_back({SemanticNodeKind::ArrowFunction, arrow, end,
                               static_cast<std::size_t>(-1), static_cast<std::size_t>(-1), scope});
        std::size_t parameter_close=arrow;
        for(std::size_t p=arrow;p-- > 0;){if(tokens[p].text==")"){parameter_close=p;break;}if(tokens[p].text==";"||tokens[p].text=="="||tokens[p].text=="{")break;}
        if (parameter_close < arrow) {
            int depth = 0; std::size_t open = arrow - 1;
            for (std::size_t p = parameter_close+1; p-- > 0;) {
                if (tokens[p].text == ")") ++depth;
                else if (tokens[p].text == "(" && --depth == 0) { open = p; break; }
            }
            std::size_t part = open + 1;
            for (std::size_t p = part; p <= parameter_close; ++p)
                if (p == parameter_close || tokens[p].text == ",") {
                    const auto name = first_parameter_name(tokens, part, p);
                    if (name != static_cast<std::size_t>(-1))
                        model.nodes.push_back({SemanticNodeKind::ParameterDeclaration,
                                               part, p, name, static_cast<std::size_t>(-1), scope});
                    part = p + 1;
                }
        } else if (arrow && tokens[arrow-1].kind == TokenKind::Identifier &&
                   (arrow<2||tokens[arrow-2].text!=":")) {
            model.nodes.push_back({SemanticNodeKind::ParameterDeclaration, arrow-1, arrow,
                                   arrow-1, static_cast<std::size_t>(-1), scope});
        }
    }

    for (std::size_t i = 0; i + 1 < tokens.size(); ++i)
        if (tokens[i].text == "class" && tokens[i+1].kind == TokenKind::Identifier)
            model.nodes.push_back({SemanticNodeKind::ClassDeclaration, i, i+2, i+1});

    for (std::size_t i = 0; i < tokens.size(); ++i) {
        if (tokens[i].kind == TokenKind::Comment) continue;
        if (tokens[i].text == "{") {
            const auto close = matching(tokens, i, "{", "}");
            if (close < tokens.size())
                model.nodes.push_back({SemanticNodeKind::BraceRegion, i, close + 1});
        } else if (tokens[i].text == "return") {
            std::size_t end = i + 1;
            while (end < tokens.size() && tokens[end].text != ";" &&
                   tokens[end].kind != TokenKind::End) ++end;
            model.nodes.push_back({SemanticNodeKind::ReturnStatement, i, end});
        }
    }
    for (std::size_t i = 0; i + 2 < tokens.size(); ++i) {
        if ((tokens[i].text != "for" && tokens[i].text != "catch") ||
            tokens[i+1].text != "(") continue;
        const auto close = matching(tokens, i + 1, "(", ")");
        if (close >= tokens.size()) continue;
        std::size_t body = close + 1;
        while (body < tokens.size() && tokens[body].kind == TokenKind::Comment) ++body;
        std::size_t end = statement_end(tokens, body);
        if (body < tokens.size() && tokens[body].text == "{") {
            const auto body_close = matching(tokens, body, "{", "}");
            if (body_close < tokens.size()) end = body_close + 1;
        }
        model.nodes.push_back({SemanticNodeKind::LexicalRegion, i, end,
                               static_cast<std::size_t>(-1), static_cast<std::size_t>(-1),
                               i, end});
        if (tokens[i].text == "catch") {
            std::size_t name = i + 2;
            while (name < close && tokens[name].kind == TokenKind::Comment) ++name;
            std::size_t after = name + 1;
            while (after < close && tokens[after].kind == TokenKind::Comment) ++after;
            if (name < close && after == close && tokens[name].kind == TokenKind::Identifier)
                model.nodes.push_back({SemanticNodeKind::CatchDeclaration, name, name + 1,
                                       name, static_cast<std::size_t>(-1), i, end});
        }
    }

    // Retain callable expression roots in the semantic graph. The checker can
    // now consume these owned ranges instead of rediscovering calls from the
    // binding-reference table. Nested calls remain children of the outer range.
    for (std::size_t begin = 0; begin < tokens.size(); ++begin) {
        if (tokens[begin].kind != TokenKind::Identifier) continue;
        std::size_t open = begin + 1;
        while (open < tokens.size() && tokens[open].kind == TokenKind::Comment) ++open;
        while (open + 1 < tokens.size() && tokens[open].text == ".") {
            ++open;
            while (open < tokens.size() && tokens[open].kind == TokenKind::Comment) ++open;
            if (open >= tokens.size() || tokens[open].kind != TokenKind::Identifier) break;
            ++open;
            while (open < tokens.size() && tokens[open].kind == TokenKind::Comment) ++open;
        }
        if(open<tokens.size()&&tokens[open].text=="<"){const auto close=matching(tokens,open,"<",">");if(close>=tokens.size())continue;open=close+1;while(open<tokens.size()&&tokens[open].kind==TokenKind::Comment)++open;}
        if (open >= tokens.size() || tokens[open].text != "(") continue;
        const auto close = matching(tokens, open, "(", ")");
        if (close >= tokens.size()) continue;
        bool contained = false;
        for (const auto& node : model.nodes)
            if (node.kind == SemanticNodeKind::ExpressionRoot && begin > node.begin_token && close < node.end_token)
                { contained = true; break; }
        if (!contained)
            model.nodes.push_back({SemanticNodeKind::ExpressionRoot, begin, close + 1});
    }
    return model;
}

} // namespace tscc
