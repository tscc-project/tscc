#include "Type.h"

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
}

TypeStore::TypeStore()
    : types_{{TypeKind::Unknown}, {TypeKind::Number}, {TypeKind::String},
             {TypeKind::Boolean}, {TypeKind::BigInt}} {}

TypeKind TypeStore::kind(TypeId id) const {
    return id < types_.size() ? types_[id].kind : TypeKind::Unknown;
}

const char* TypeStore::name(TypeId id) const {
    switch (kind(id)) {
        case TypeKind::Number: return "number";
        case TypeKind::String: return "string";
        case TypeKind::Boolean: return "boolean";
        case TypeKind::BigInt: return "bigint";
        default: return "unknown";
    }
}

TypeModel build_type_model(const std::vector<Token>& tokens, const Program& program,
                           const SemanticModel& semantic, const BindingModel& binding) {
    TypeModel model;
    model.symbol_types.assign(binding.symbols.size(), model.store.unknown());
    for (std::size_t i = 0; i < binding.symbols.size(); ++i) {
        const auto node_index = binding.symbols[i].semantic_node;
        if (node_index >= semantic.nodes.size()) continue;
        const auto& node = semantic.nodes[node_index];
        if (node.kind == SemanticNodeKind::VariableDeclaration &&
            node.variable_index < program.variables.size())
            model.symbol_types[i] = annotation_type(tokens, program.variables[node.variable_index],
                                                    model.store);
    }
    return model;
}

} // namespace tscc
