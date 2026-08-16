#include "Checker.h"

namespace tscc {
namespace {
enum class PrimitiveType { Unsupported, Number, String, Boolean, BigInt };

std::vector<std::size_t> significant_tokens(const std::vector<Token>& tokens,
                                            std::size_t begin, std::size_t end) {
    std::vector<std::size_t> result;
    for (auto i = begin; i < end; ++i)
        if (tokens[i].kind != TokenKind::Comment) result.push_back(i);
    return result;
}

PrimitiveType annotation_type(const std::vector<Token>& tokens,
                              const VariableDeclaration& declaration) {
    const auto significant = significant_tokens(tokens, declaration.type_begin_token,
                                                 declaration.type_end_token);
    if (significant.size() != 1) return PrimitiveType::Unsupported;
    const auto& text = tokens[significant.front()].text;
    if (text == "number") return PrimitiveType::Number;
    if (text == "string") return PrimitiveType::String;
    if (text == "boolean") return PrimitiveType::Boolean;
    return PrimitiveType::Unsupported;
}

PrimitiveType literal_type(const std::vector<Token>& tokens,
                           const VariableDeclaration& declaration) {
    const auto significant = significant_tokens(tokens, declaration.initializer_begin_token,
                                                 declaration.initializer_end_token);
    if (significant.size() == 1) {
        const auto& token = tokens[significant.front()];
        if (token.kind == TokenKind::Number)
            return !token.text.empty() && token.text.back() == 'n' ? PrimitiveType::BigInt
                                                                   : PrimitiveType::Number;
        if (token.kind == TokenKind::String || token.kind == TokenKind::Template)
            return PrimitiveType::String;
        if (token.text == "true" || token.text == "false") return PrimitiveType::Boolean;
    }
    if (significant.size() == 2 &&
        (tokens[significant[0]].text == "+" || tokens[significant[0]].text == "-") &&
        tokens[significant[1]].kind == TokenKind::Number &&
        (tokens[significant[1]].text.empty() || tokens[significant[1]].text.back() != 'n'))
        return PrimitiveType::Number;
    return PrimitiveType::Unsupported;
}

const char* type_name(PrimitiveType type) {
    switch (type) {
        case PrimitiveType::Number: return "number";
        case PrimitiveType::String: return "string";
        case PrimitiveType::Boolean: return "boolean";
        case PrimitiveType::BigInt: return "bigint";
        default: return "unsupported";
    }
}
}

bool check_program(const SourceFile& source, const std::vector<Token>& tokens,
                   const Program& program, const SemanticModel& model,
                   Diagnostics& diagnostics) {
    for (const auto& node : model.nodes) {
        if (node.kind != SemanticNodeKind::VariableDeclaration ||
            node.variable_index >= program.variables.size()) continue;
        const auto& declaration = program.variables[node.variable_index];
        const auto expected = annotation_type(tokens, declaration);
        if (expected == PrimitiveType::Unsupported ||
            declaration.initializer_end_token <= declaration.initializer_begin_token)
            continue;
        const auto actual = literal_type(tokens, declaration);
        if (actual == PrimitiveType::Unsupported || actual == expected) continue;
        const auto offset = tokens[declaration.initializer_begin_token].begin;
        const auto [line, column] = source.line_col(offset);
        diagnostics.error(source.path, line, column,
            std::string("Type '") + type_name(actual) + "' is not assignable to type '" +
            type_name(expected) + "'.", source.line_text(line));
    }
    return !diagnostics.has_errors();
}
}
