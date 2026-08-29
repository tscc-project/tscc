#include "Type.h"
#include <algorithm>

namespace tscc {
namespace {
TypeId annotation_type(const std::vector<Token>& tokens,
                       const VariableDeclaration& declaration,
                       const TypeStore& store) {
    std::size_t found = static_cast<std::size_t>(-1), count = 0;
    for (auto i = declaration.type_begin_token; i < declaration.type_end_token; ++i)
        if (tokens[i].kind != TokenKind::Comment) { found = i; ++count; }
    if (count != 1) return store.unknown();
    if (tokens[found].text == "number") return store.number();
    if (tokens[found].text == "string") return store.string();
    if (tokens[found].text == "boolean") return store.boolean();
    return store.unknown();
}
TypeId named_type(const std::string& text, const TypeStore& store) {
    if (text == "number") return store.number();
    if (text == "string") return store.string();
    if (text == "boolean") return store.boolean();
    if (text == "bigint") return store.bigint();
    return store.unknown();
}
TypeId node_annotation(const std::vector<Token>& tokens, const SemanticNode& node,
                       const TypeStore& store) {
    bool colon = false;
    for (auto i = node.name_token + 1; i < node.end_token && i < tokens.size(); ++i) {
        if (tokens[i].kind == TokenKind::Comment) continue;
        if (tokens[i].text == ":") { colon = true; continue; }
        if (tokens[i].text == "=" || tokens[i].text == "?") continue;
        if (colon) return named_type(tokens[i].text, store);
    }
    return store.unknown();
}
}

TypeStore::TypeStore()
    : types_{{TypeKind::Unknown}, {TypeKind::Number}, {TypeKind::String},
             {TypeKind::Boolean}, {TypeKind::BigInt}, {TypeKind::Function}} {}

TypeKind TypeStore::kind(TypeId id) const {
    return id < types_.size() ? types_[id].kind : TypeKind::Unknown;
}

const char* TypeStore::name(TypeId id) const {
    switch (kind(id)) {
        case TypeKind::Number: return "number";
        case TypeKind::String: return "string";
        case TypeKind::Boolean: return "boolean";
        case TypeKind::BigInt: return "bigint";
        case TypeKind::Function: return "function";
        default: return "unknown";
    }
}

TypeModel build_type_model(const std::vector<Token>& tokens, const Program& program,
                           const SemanticModel& semantic, const BindingModel& binding) {
    TypeModel model;
    model.symbol_types.assign(binding.symbols.size(), model.store.unknown());
    model.function_signatures.resize(binding.symbols.size());
    for (std::size_t i = 0; i < binding.symbols.size(); ++i) {
        const auto node_index = binding.symbols[i].semantic_node;
        if (node_index >= semantic.nodes.size()) continue;
        const auto& node = semantic.nodes[node_index];
        if (node.kind == SemanticNodeKind::VariableDeclaration &&
            node.variable_index < program.variables.size())
            model.symbol_types[i] = annotation_type(tokens, program.variables[node.variable_index],
                                                    model.store);
        else if (node.kind == SemanticNodeKind::ParameterDeclaration)
            model.symbol_types[i] = node_annotation(tokens, node, model.store);
        else if (binding.symbols[i].kind == SymbolKind::Function)
            model.symbol_types[i] = model.store.function();
    }
    for (std::size_t symbol = 0; symbol < binding.symbols.size(); ++symbol) {
        if (binding.symbols[symbol].kind != SymbolKind::Function) continue;
        const auto node_index = binding.symbols[symbol].semantic_node;
        if (node_index >= semantic.nodes.size()) continue;
        const auto& function = semantic.nodes[node_index];
        std::vector<std::size_t> parameters;
        for (std::size_t candidate = 0; candidate < binding.symbols.size(); ++candidate) {
            if (binding.symbols[candidate].kind != SymbolKind::Parameter) continue;
            const auto parameter_node = binding.symbols[candidate].semantic_node;
            if (parameter_node < semantic.nodes.size() &&
                semantic.nodes[parameter_node].scope_token == function.scope_token)
                parameters.push_back(candidate);
        }
        std::sort(parameters.begin(), parameters.end(), [&](auto a, auto b) {
            return binding.symbols[a].declaration_token < binding.symbols[b].declaration_token;
        });
        auto& signature = model.function_signatures[symbol];
        for (auto parameter : parameters) {
            signature.parameters.push_back(model.symbol_types[parameter]);
            const auto& parameter_node = semantic.nodes[binding.symbols[parameter].semantic_node];
            bool optional = false, rest = false;
            for (auto token = parameter_node.begin_token; token < parameter_node.end_token; ++token) {
                optional = optional || tokens[token].text == "?" || tokens[token].text == "=";
                rest = rest || tokens[token].text == "...";
            }
            signature.rest = signature.rest || rest;
            if (!optional && !rest) ++signature.required_parameters;
        }
        std::size_t close = function.begin_token;
        int depth = 0; bool seen = false;
        for (auto i = function.begin_token; i < function.scope_token; ++i) {
            if (tokens[i].text == "(") { ++depth; seen = true; }
            else if (tokens[i].text == ")" && depth && --depth == 0) { close = i; break; }
        }
        for (auto i = close + 1; seen && i < function.scope_token; ++i) {
            if (tokens[i].kind == TokenKind::Comment) continue;
            if (tokens[i].text == ":") {
                do { ++i; } while (i < function.scope_token && tokens[i].kind == TokenKind::Comment);
                if (i < function.scope_token) signature.result = named_type(tokens[i].text, model.store);
                break;
            }
        }
        signature.valid = true;
    }
    return model;
}

} // namespace tscc
