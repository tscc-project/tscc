#pragma once
#include "Lexer.h"
#include "Syntax.h"
#include <cstddef>
#include <vector>

namespace tscc {

enum class SemanticNodeKind {
    VariableDeclaration,
    FunctionDeclaration,
    ParameterDeclaration,
    BraceRegion,
    ReturnStatement,
};

struct SemanticNode {
    SemanticNodeKind kind = SemanticNodeKind::BraceRegion;
    std::size_t begin_token = 0;
    std::size_t end_token = 0; // exclusive
    std::size_t name_token = static_cast<std::size_t>(-1);
    std::size_t variable_index = static_cast<std::size_t>(-1);
    std::size_t scope_token = static_cast<std::size_t>(-1);
};

struct SemanticModel {
    std::vector<SemanticNode> nodes;
};

SemanticModel build_semantic_model(const std::vector<Token>&, const Program&);

} // namespace tscc
