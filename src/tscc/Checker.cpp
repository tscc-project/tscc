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

struct ExpressionResult {
    TypeId type = 0;
    bool complete = false;
    std::size_t error_token = static_cast<std::size_t>(-1);
    std::string error;
};

class PrimitiveExpressionTyper {
public:
    PrimitiveExpressionTyper(const std::vector<Token>& tokens, std::size_t begin,
                             std::size_t end, const BindingModel& binding,
                             const TypeModel& types)
        : tokens_(tokens), binding_(binding), types_(types), sig_(significant_tokens(tokens, begin, end)) {}

    ExpressionResult run() {
        ExpressionResult result;
        result.type = additive();
        result.complete = pos_ == sig_.size();
        result.error_token = error_token_;
        result.error = error_;
        if (!result.complete) result.type = types_.store.unknown();
        return result;
    }

private:
    const std::vector<Token>& tokens_;
    const BindingModel& binding_;
    const TypeModel& types_;
    std::vector<std::size_t> sig_;
    std::size_t pos_ = 0;
    std::size_t error_token_ = static_cast<std::size_t>(-1);
    std::string error_;

    bool accept(const char* text) {
        if (pos_ >= sig_.size() || tokens_[sig_[pos_]].text != text) return false;
        ++pos_;
        return true;
    }
    TypeId primary() {
        if (pos_ >= sig_.size()) return types_.store.unknown();
        if (accept("(")) {
            const auto value = additive();
            if (!accept(")")) return types_.store.unknown();
            return value;
        }
        const auto token_index = sig_[pos_++];
        const auto& token = tokens_[token_index];
        if (token.kind == TokenKind::Number)
            return !token.text.empty() && token.text.back() == 'n'
                       ? types_.store.bigint() : types_.store.number();
        if (token.kind == TokenKind::String || token.kind == TokenKind::Template)
            return types_.store.string();
        if (token.text == "true" || token.text == "false") return types_.store.boolean();
        if (token.kind == TokenKind::Identifier) {
            const auto symbol = binding_.symbol_for_reference(token_index);
            if (symbol < types_.symbol_types.size()) return types_.symbol_types[symbol];
        }
        return types_.store.unknown();
    }
    TypeId unary() {
        if (pos_ >= sig_.size()) return types_.store.unknown();
        const auto op_token = sig_[pos_];
        const auto& op = tokens_[op_token].text;
        if (op != "+" && op != "-" && op != "~" && op != "!" && op != "typeof")
            return primary();
        ++pos_;
        const auto value = unary();
        const auto kind = types_.store.kind(value);
        if (value == types_.store.unknown()) return value;
        if (op == "!") return types_.store.boolean();
        if (op == "typeof") return types_.store.string();
        if ((op == "-" || op == "~") && kind == TypeKind::BigInt) return types_.store.bigint();
        if (kind == TypeKind::Number) return types_.store.number();
        fail_unary(op_token, op, value);
        return types_.store.unknown();
    }
    TypeId multiplicative() {
        auto left = unary();
        while (pos_ < sig_.size()) {
            const auto op_token = sig_[pos_];
            const auto& op = tokens_[op_token].text;
            if (op != "*" && op != "/" && op != "%" && op != "**") break;
            ++pos_;
            const auto right = unary();
            left = arithmetic(op_token, op, left, right);
        }
        return left;
    }
    TypeId additive() {
        auto left = multiplicative();
        while (pos_ < sig_.size()) {
            const auto op_token = sig_[pos_];
            const auto& op = tokens_[op_token].text;
            if (op != "+" && op != "-") break;
            ++pos_;
            const auto right = multiplicative();
            if (op == "+" && left != types_.store.unknown() && right != types_.store.unknown() &&
                (types_.store.kind(left) == TypeKind::String ||
                 types_.store.kind(right) == TypeKind::String))
                left = types_.store.string();
            else left = arithmetic(op_token, op, left, right);
        }
        return left;
    }
    TypeId arithmetic(std::size_t token, const std::string& op, TypeId left, TypeId right) {
        if (left == types_.store.unknown() || right == types_.store.unknown())
            return types_.store.unknown();
        const auto left_kind = types_.store.kind(left), right_kind = types_.store.kind(right);
        if (left_kind == TypeKind::Number && right_kind == TypeKind::Number)
            return types_.store.number();
        if (left_kind == TypeKind::BigInt && right_kind == TypeKind::BigInt)
            return types_.store.bigint();
        if (error_.empty()) {
            error_token_ = token;
            error_ = std::string("Operator '") + op + "' cannot be applied to types '" +
                     types_.store.name(left) + "' and '" + types_.store.name(right) + "'.";
        }
        return types_.store.unknown();
    }
    void fail_unary(std::size_t token, const std::string& op, TypeId value) {
        if (!error_.empty()) return;
        error_token_ = token;
        error_ = std::string("Operator '") + op + "' cannot be applied to type '" +
                 types_.store.name(value) + "'.";
    }
};

ExpressionResult expression_type(const std::vector<Token>& tokens, std::size_t begin,
                                 std::size_t end, const BindingModel& binding,
                                 const TypeModel& types) {
    return PrimitiveExpressionTyper(tokens, begin, end, binding, types).run();
}

void report_expression_error(const SourceFile& source, const std::vector<Token>& tokens,
                             const ExpressionResult& result, Diagnostics& diagnostics) {
    if (result.error.empty() || result.error_token >= tokens.size()) return;
    const auto [line, column] = source.line_col(tokens[result.error_token].begin);
    diagnostics.error(source.path, line, column, result.error, source.line_text(line));
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
        if (declaration.initializer_end_token <= declaration.initializer_begin_token)
            continue;
        const auto expression = expression_type(tokens, declaration.initializer_begin_token,
                                                declaration.initializer_end_token,
                                                binding, types);
        report_expression_error(source, tokens, expression, diagnostics);
        const auto actual = expression.type;
        if (!expression.error.empty() || expected == types.store.unknown() ||
            actual == types.store.unknown() || actual == expected) continue;
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
        const auto expression = expression_type(tokens, begin, end, binding, types);
        report_expression_error(source, tokens, expression, diagnostics);
        const auto actual = expression.type;
        if (expression.error.empty() && actual != types.store.unknown() && actual != expected)
            report_mismatch(source, tokens, begin, actual, expected, types.store, diagnostics);
    }
    return !diagnostics.has_errors();
}
}
