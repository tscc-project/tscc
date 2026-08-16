#include "Checker.h"
#include "Type.h"
#include <unordered_set>

namespace tscc {
namespace {
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
                           std::size_t begin, std::size_t end) {
    const auto significant = significant_tokens(tokens, begin, end);
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

PrimitiveType expression_type(const std::vector<Token>& tokens, std::size_t begin,
                              std::size_t end, const BindingModel& binding,
                              const std::vector<PrimitiveType>& symbol_types) {
    const auto literal = literal_type(tokens, begin, end);
    if (literal != PrimitiveType::Unsupported) return literal;
    const auto significant = significant_tokens(tokens, begin, end);
    if (significant.size() != 1 || tokens[significant.front()].kind != TokenKind::Identifier)
        return PrimitiveType::Unsupported;
    const auto symbol = binding.symbol_for_reference(significant.front());
    return symbol < symbol_types.size() ? symbol_types[symbol] : PrimitiveType::Unsupported;
}

void report_mismatch(const SourceFile& source, const std::vector<Token>& tokens,
                     std::size_t token, PrimitiveType actual, PrimitiveType expected,
                     Diagnostics& diagnostics) {
    const auto [line, column] = source.line_col(tokens[token].begin);
    diagnostics.error(source.path, line, column,
        std::string("Type '") + type_name(actual) + "' is not assignable to type '" +
        type_name(expected) + "'.", source.line_text(line));
}
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

bool check_program(const SourceFile& source, const std::vector<Token>& tokens,
                   const Program& program, const SemanticModel& model,
                   const BindingModel& binding,
                   Diagnostics& diagnostics) {
    std::vector<PrimitiveType> symbol_types(binding.symbols.size(), PrimitiveType::Unsupported);
    for (std::size_t symbol_index = 0; symbol_index < binding.symbols.size(); ++symbol_index) {
        const auto& symbol = binding.symbols[symbol_index];
        if (symbol.semantic_node >= model.nodes.size()) continue;
        const auto& node = model.nodes[symbol.semantic_node];
        if (node.kind != SemanticNodeKind::VariableDeclaration ||
            node.variable_index >= program.variables.size()) continue;
        symbol_types[symbol_index] = annotation_type(tokens, program.variables[node.variable_index]);
    }

    for (const auto& node : model.nodes) {
        if (node.kind != SemanticNodeKind::VariableDeclaration ||
            node.variable_index >= program.variables.size()) continue;
        const auto& declaration = program.variables[node.variable_index];
        const auto expected = annotation_type(tokens, declaration);
        if (expected == PrimitiveType::Unsupported ||
            declaration.initializer_end_token <= declaration.initializer_begin_token)
            continue;
        const auto actual = expression_type(tokens, declaration.initializer_begin_token,
                                            declaration.initializer_end_token,
                                            binding, symbol_types);
        if (actual == PrimitiveType::Unsupported || actual == expected) continue;
        report_mismatch(source, tokens, declaration.initializer_begin_token,
                        actual, expected, diagnostics);
    }

    std::unordered_set<std::size_t> declaration_names;
    for (const auto& symbol : binding.symbols) declaration_names.insert(symbol.declaration_token);
    for (std::size_t equals = 1; equals + 1 < tokens.size(); ++equals) {
        if (tokens[equals].text != "=") continue;
        std::size_t left = equals;
        while (left > 0 && tokens[left-1].kind == TokenKind::Comment) --left;
        if (left == 0) continue;
        --left;
        if (tokens[left].kind != TokenKind::Identifier || declaration_names.count(left)) continue;
        const auto target_symbol = binding.symbol_for_reference(left);
        if (target_symbol >= symbol_types.size()) continue;
        const auto expected = symbol_types[target_symbol];
        if (expected == PrimitiveType::Unsupported) continue;
        std::size_t begin = equals + 1;
        while (begin < tokens.size() && tokens[begin].kind == TokenKind::Comment) ++begin;
        std::size_t end = begin;
        while (end < tokens.size() && tokens[end].text != ";" &&
               tokens[end].text != "," && tokens[end].kind != TokenKind::End) ++end;
        const auto actual = expression_type(tokens, begin, end, binding, symbol_types);
        if (actual != PrimitiveType::Unsupported && actual != expected)
            report_mismatch(source, tokens, begin, actual, expected, diagnostics);
    }
    return !diagnostics.has_errors();
}
}
