#pragma once
#include "Lexer.h"
#include "Semantic.h"
#include "Syntax.h"
#include <cstddef>
#include <string>
#include <utility>
#include <vector>

namespace tscc {

enum class SymbolKind { Variable, Parameter, Function, Class, Enum, Namespace, Import };
enum class VariableKind { None, Var, Let, Const };

struct BoundScope {
    std::size_t begin_token = 0;
    std::size_t end_token = 0;
    std::size_t parent = static_cast<std::size_t>(-1);
    bool function_scope = false;
};

struct BoundSymbol {
    std::string name;
    SymbolKind kind = SymbolKind::Variable;
    std::size_t declaration_token = 0;
    std::size_t scope = 0;
    std::size_t semantic_node = static_cast<std::size_t>(-1);
    VariableKind variable_kind = VariableKind::None;
};

struct BoundReference {
    std::size_t token = 0;
    std::size_t scope = 0;
    std::size_t symbol = static_cast<std::size_t>(-1);
};

struct BindingModel {
    std::vector<BoundScope> scopes;
    std::vector<BoundSymbol> symbols;
    std::vector<BoundReference> references;
    std::vector<std::pair<std::size_t, std::size_t>> import_ranges;

    std::size_t symbol_for_reference(std::size_t token) const;
    bool is_declaration_token(std::size_t token) const;
    bool is_import_token(std::size_t token) const;
};

BindingModel bind_semantic_model(const std::vector<Token>&, const Program&,
                                 const SemanticModel&);

} // namespace tscc
