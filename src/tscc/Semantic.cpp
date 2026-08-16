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
        if (body >= tokens.size() || tokens[body].text != "{") continue;
        const auto body_close = matching(tokens, body, "{", "}");
        if (body_close >= tokens.size()) continue;
        std::size_t name = function_token + 1;
        while (name < open && tokens[name].kind == TokenKind::Comment) ++name;
        model.nodes.push_back({SemanticNodeKind::FunctionDeclaration,
                               function_token, body_close + 1,
                               name < open ? name : static_cast<std::size_t>(-1),
                               static_cast<std::size_t>(-1), body});

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
                                           static_cast<std::size_t>(-1), body});
                part = i + 1;
            }
        }
    }

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
    return model;
}

} // namespace tscc
