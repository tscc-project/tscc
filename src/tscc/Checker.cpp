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

TypeId literal_type(const std::vector<Token>& tokens,
                    std::size_t begin, std::size_t end, const TypeStore& store) {
    const auto significant = significant_tokens(tokens, begin, end);
    if (significant.size() == 1) {
        const auto& token = tokens[significant.front()];
        if (token.kind == TokenKind::Number)
            return !token.text.empty() && token.text.back() == 'n' ? store.bigint()
                                                                   : store.number();
        if (token.kind == TokenKind::String || token.kind == TokenKind::Template)
            return store.string();
        if (token.text == "true" || token.text == "false") return store.boolean();
    }
    if (significant.size() == 2 &&
        (tokens[significant[0]].text == "+" || tokens[significant[0]].text == "-") &&
        tokens[significant[1]].kind == TokenKind::Number &&
        (tokens[significant[1]].text.empty() || tokens[significant[1]].text.back() != 'n'))
        return store.number();
    return store.unknown();
}

TypeId expression_type(const std::vector<Token>& tokens, std::size_t begin,
                       std::size_t end, const BindingModel& binding,
                       const TypeModel& types) {
    const auto literal = literal_type(tokens, begin, end, types.store);
    if (literal != types.store.unknown()) return literal;
    const auto significant = significant_tokens(tokens, begin, end);
    if (significant.size() != 1 || tokens[significant.front()].kind != TokenKind::Identifier)
        return types.store.unknown();
    const auto symbol = binding.symbol_for_reference(significant.front());
    return symbol < types.symbol_types.size() ? types.symbol_types[symbol]
                                              : types.store.unknown();
}

void report_mismatch(const SourceFile& source, const std::vector<Token>& tokens,
                     std::size_t token, TypeId actual, TypeId expected,
                     const TypeStore& types,
                     Diagnostics& diagnostics) {
    const auto [line, column] = source.line_col(tokens[token].begin);
    diagnostics.error(source.path, line, column,
        std::string("Type '") + types.name(actual) + "' is not assignable to type '" +
        types.name(expected) + "'.", source.line_text(line));
}
}

bool check_program(const SourceFile& source, const std::vector<Token>& tokens,
                   const Program& program, const SemanticModel& model,
                   const BindingModel& binding, const TypeModel& types,
                   Diagnostics& diagnostics) {
    for (const auto& node : model.nodes) {
        if (node.kind != SemanticNodeKind::VariableDeclaration ||
            node.variable_index >= program.variables.size()) continue;
        const auto& declaration = program.variables[node.variable_index];
        TypeId expected = types.store.unknown();
        for (std::size_t i = 0; i < binding.symbols.size(); ++i)
            if (binding.symbols[i].declaration_token == declaration.name_token) {
                expected = types.symbol_types[i]; break;
            }
        if (expected == types.store.unknown() ||
            declaration.initializer_end_token <= declaration.initializer_begin_token)
            continue;
        const auto actual = expression_type(tokens, declaration.initializer_begin_token,
                                            declaration.initializer_end_token,
                                            binding, types);
        if (actual == types.store.unknown() || actual == expected) continue;
        report_mismatch(source, tokens, declaration.initializer_begin_token,
                        actual, expected, types.store, diagnostics);
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
        if (target_symbol >= types.symbol_types.size()) continue;
        const auto expected = types.symbol_types[target_symbol];
        if (expected == types.store.unknown()) continue;
        std::size_t begin = equals + 1;
        while (begin < tokens.size() && tokens[begin].kind == TokenKind::Comment) ++begin;
        std::size_t end = begin;
        while (end < tokens.size() && tokens[end].text != ";" &&
               tokens[end].text != "," && tokens[end].kind != TokenKind::End) ++end;
        const auto actual = expression_type(tokens, begin, end, binding, types);
        if (actual != types.store.unknown() && actual != expected)
            report_mismatch(source, tokens, begin, actual, expected, types.store, diagnostics);
    }
    return !diagnostics.has_errors();
}
}
